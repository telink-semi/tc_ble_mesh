/* Includes ------------------------------------------------------------------*/
//#include <stddef.h>
//#include <string.h>
#include "mible_api.h"
#include "mible_log.h"
#include "mible_trace.h"
#include "common/crc32.h"
#include "common/mible_rxfer.h"
#include "common/queue.h"
#include "mible_dfu_auth.h"
#include "mible_dfu_flash.h"
#include "mible_dfu_main.h"
#if MI_API_ENABLE
/* Private define ------------------------------------------------------------*/
#define PROTOCAL_VERSION                                       0x0002
#define MAX_FRAGMENT_SIZE                                      2048

#define MIBLE_DFU_REQ_PROTOCOL_VERSION                         0x0001
#define MIBLE_DFU_REQ_FRAGMENT_SIZE                            0x0002
#define MIBLE_DFU_REQ_LAST_FRAGMENT                            0x0003
#define MIBLE_DFU_REQ_ACTIVATE_NEW_FW                          0x0004

#define MIBLE_DFU_EVENT_RSP_OP_STATUS                          0x01
#define MIBLE_DFU_EVENT_FRAG_TRANS_STATUS                      0x02

#define MIBLE_DFU_STATUS_SUCC                                  0x00
#define MIBLE_DFU_STATUS_ERR_NO_CMD                            0x01
#define MIBLE_DFU_STATUS_ERR_AUTH_FAIL                         0x02
#define MIBLE_DFU_STATUS_ERR_INVALID                           0x03
#define MIBLE_DFU_STATUS_ERR_NO_MEM                            0x04
#define MIBLE_DFU_STATUS_ERR_BUSY                              0x05
#define MIBLE_DFU_STATUS_ERR_UNSIGNED                          0x06
#define MIBLE_DFU_STATUS_ERR_RX_FAIL                           0x07
#define MIBLE_DFU_STATUS_ERR_UNKNOWN                           0xFF

#define MIBLE_DFU_TIMER_PERIOD                                 20
#define MIBLE_DFU_EVT_SIZE_MAX                                 12
#define MIBLE_DFU_EVT_CNT_MAX                                  2
#define MIBLE_DFU_STORE_RETRY_MAX                              3
#define MIBLE_DFU_CALLBACK_MAX                                 1

#define RECORD_DFU_INFO                                        5

#define HI_UINT16(a)                            (((uint16_t)(a) >> 8) & 0xFF)
#define LO_UINT16(a)                            ((uint16_t)(a) & 0xFF)

/* Private typedef -----------------------------------------------------------*/

typedef struct {
    mible_dfu_init_t init;
    uint16_t current_size;
    uint32_t switch_delay;
    void *   timer_id;
    rxfer_cb_t rxfer_dfu;
    pt_t     pt_rx_ctrl;
    pt_t     pt_xfer;
    pt_t     pt_rsp;
    pt_t     pt_delay_ctrl;
    pt_t     pt_flash;
    struct pt_sem sem_delay;
    queue_t  evt_queue;
    uint16_t fragment_size;
} mible_dfu_config_t;

#if defined ( __CC_ARM )

__PACKED struct mible_dfu_cmd {
    uint16_t command;
    uint8_t  para_len;
    uint8_t  para[1];
};
typedef struct mible_dfu_cmd mible_dfu_cmd_t;

__PACKED struct mible_dfu_evt {
    uint8_t event;
    uint8_t para_len;
    uint8_t para[1];
};
typedef struct mible_dfu_evt mible_dfu_evt_t;

__PACKED struct mible_dfu_fragment {
    uint16_t index;
    uint8_t data[MAX_FRAGMENT_SIZE];
};
typedef struct mible_dfu_fragment mible_dfu_fragment_t;

#elif defined   ( __GNUC__ )

struct __PACKED mible_dfu_cmd {
    uint16_t command;
    uint8_t  para_len;
    uint8_t  para[1];
};
typedef struct mible_dfu_cmd mible_dfu_cmd_t;

struct __PACKED mible_dfu_evt {
    uint8_t event;
    uint8_t para_len;
    uint8_t para[1];
};
typedef struct mible_dfu_evt mible_dfu_evt_t;

struct __PACKED mible_dfu_fragment {
    uint16_t index;
    uint8_t data[MAX_FRAGMENT_SIZE];
};
typedef struct mible_dfu_fragment mible_dfu_fragment_t;

#endif

/* Private function prototypes -----------------------------------------------*/

static mible_status_t mible_dfu_rsp(uint8_t event, uint8_t * param, uint16_t len);
static void mible_dfu_timer_callback(void * arg);
static int  mible_dfu_program_flash(pt_t * pt);
static int  mible_dfu_xfer_process(pt_t * pt);
static int  mible_dfu_rsp_process(pt_t * pt);
static int  mible_dfu_delay_handler(pt_t * pt);
static void mible_dfu_reset(void);

/* Private variables ---------------------------------------------------------*/

static int retry_num;
static mible_dfu_config_t m_dfu;
static mible_dfu_fragment_t dfu_fragbuf;
static uint8_t evt_buffer[MIBLE_DFU_EVT_SIZE_MAX * MIBLE_DFU_EVT_CNT_MAX];
static uint32_t dfu_timer_tick;
static pstimer_t dfu_timer = {
        .p_curr_tick = &dfu_timer_tick,
        .tick_interval_ms = MIBLE_DFU_TIMER_PERIOD,
};
static mible_dfu_callback_t dfu_callback[MIBLE_DFU_CALLBACK_MAX] = {0};
static struct {
    uint8_t is_downloading:1;
    uint8_t   has_verified:1;
    uint8_t            rfu:6;
} flags;
static struct {
    uint8_t  version  :4;
    uint8_t  rfu      :4;
    uint8_t  rfu8     :8;
    uint16_t last_index;
    uint32_t crc32;
    uint32_t recv_bytes;
    uint32_t activate_delay;
} m_dfu_info;

static void foreach_callback(mible_dfu_state_t state, mible_dfu_param_t *param)
{
    for (int index = 0; index < MIBLE_DFU_CALLBACK_MAX; index++) {
        if (dfu_callback[index])
            dfu_callback[index](state, param);
    }
}

/* Exported functions --------------------------------------------------------*/
mible_status_t mible_dfu_init(mible_dfu_init_t * p_init)
{
    mible_status_t ret;

    if (NULL == p_init) {
        return MI_ERR_INVALID_PARAM;
    }

    /* Init parameters for DFU config */
    m_dfu.init = *p_init;
    m_dfu.switch_delay = 0xFFFFFFFF;
    ret = mible_dfu_flash_init(p_init->flash_start, p_init->flash_size);
    if (MI_SUCCESS != ret) {
        MI_LOG_ERROR("Fail to init flash (err %d)\n", ret);
        return ret;
    }

    /* Init queue for sending event */
    ret = queue_init(&m_dfu.evt_queue, evt_buffer,
                     MIBLE_DFU_EVT_CNT_MAX, MIBLE_DFU_EVT_SIZE_MAX);
    if (MI_SUCCESS != ret) {
        MI_LOG_ERROR("Fail to init queue for event (err %d)\n", ret);
        return ret;
    }

    /* Init parameters for Segmentation And Reassembly */
    rxfer_init(&m_dfu.rxfer_dfu, &dfu_timer);
    m_dfu.rxfer_dfu.conn_handle  = p_init->conn_handle;
    m_dfu.rxfer_dfu.srv_handle   = p_init->srv_handle;
    m_dfu.rxfer_dfu.value_handle = p_init->data_handle;

    /* Init protothread objects */
    PT_INIT(&m_dfu.pt_xfer);
    PT_INIT(&m_dfu.pt_rsp);
    PT_INIT(&m_dfu.pt_delay_ctrl);
    PT_INIT(&m_dfu.pt_flash);
    PT_SEM_INIT(&m_dfu.sem_delay, 0);

    memset(&flags, 0, sizeof(flags));

    /* Init and enable timer */
    if (NULL != m_dfu.timer_id) {
        mible_timer_delete(m_dfu.timer_id);
        m_dfu.timer_id = NULL;
    }
    ret = mible_timer_create(&m_dfu.timer_id, mible_dfu_timer_callback,
                             MIBLE_TIMER_REPEATED);
    if (MI_SUCCESS != ret) {
        MI_LOG_ERROR("Fail to create timer.\n");
        return ret;
    }

    return MI_SUCCESS;
}

mible_status_t mible_dfu_ctrl(uint8_t * buffer, uint16_t size)
{
    mible_dfu_cmd_t * p_cmd = (mible_dfu_cmd_t *)buffer;
    mible_status_t ret = MI_ERR_INVALID_PARAM;
    mible_dfu_param_t dfu_param;

    if (NULL == p_cmd ||
        size < offsetof(mible_dfu_cmd_t, para) ||
        size != offsetof(mible_dfu_cmd_t, para) + p_cmd->para_len) {
        return ret;
    }

    switch (p_cmd->command) {
        case MIBLE_DFU_REQ_PROTOCOL_VERSION: {
            uint8_t param[5];
            param[0] = LO_UINT16(p_cmd->command);
            param[1] = HI_UINT16(p_cmd->command);
            param[2] = MIBLE_DFU_STATUS_SUCC;
            param[3] = LO_UINT16(PROTOCAL_VERSION);
            param[4] = HI_UINT16(PROTOCAL_VERSION);
            ret = mible_dfu_rsp(MIBLE_DFU_EVENT_RSP_OP_STATUS, param, sizeof(param));
            mible_timer_start(m_dfu.timer_id, MIBLE_DFU_TIMER_PERIOD, NULL);
            break;
        }
        case MIBLE_DFU_REQ_FRAGMENT_SIZE: {
            flags.is_downloading = 1;
            uint8_t simul_retrans, dmtu;
            rxfer_features_get(&simul_retrans, &dmtu);
            m_dfu.fragment_size = dmtu > 18 ? MAX_FRAGMENT_SIZE : 512;

            uint8_t param[5];
            param[0] = LO_UINT16(p_cmd->command);
            param[1] = HI_UINT16(p_cmd->command);
            param[2] = MIBLE_DFU_STATUS_SUCC;
            param[3] = LO_UINT16(m_dfu.fragment_size);
            param[4] = HI_UINT16(m_dfu.fragment_size);
            ret = mible_dfu_rsp(MIBLE_DFU_EVENT_RSP_OP_STATUS, param, sizeof(param));
            dfu_param.start.fragment_size = m_dfu.fragment_size;
            foreach_callback(MIBLE_DFU_STATE_START, &dfu_param);
            mible_timer_start(m_dfu.timer_id, MIBLE_DFU_TIMER_PERIOD, NULL);
            break;
        }
        case MIBLE_DFU_REQ_LAST_FRAGMENT: {
            if (mible_record_read(RECORD_DFU_INFO, (void*)&m_dfu_info, sizeof(m_dfu_info)) != MI_SUCCESS)
                mible_dfu_reset();

            MI_LOG_INFO("last index %d, crc32 %x\n", m_dfu_info.last_index, m_dfu_info.crc32);

            uint8_t param[9];
            param[0] = LO_UINT16(p_cmd->command);
            param[1] = HI_UINT16(p_cmd->command);
            param[2] = MIBLE_DFU_STATUS_SUCC;
            param[3] = LO_UINT16(m_dfu_info.last_index);
            param[4] = HI_UINT16(m_dfu_info.last_index);
            memcpy(&param[5], &m_dfu_info.crc32, 4);
            ret = mible_dfu_rsp(MIBLE_DFU_EVENT_RSP_OP_STATUS, param, sizeof(param));
            break;
        }
        case MIBLE_DFU_REQ_ACTIVATE_NEW_FW: {
            flags.is_downloading = 0;
            mible_record_delete(RECORD_DFU_INFO);

            uint8_t param[3];
            param[0] = LO_UINT16(p_cmd->command);
            param[1] = HI_UINT16(p_cmd->command);
            dfu_param.verify.value = MIBLE_DFU_VERIFY_FAIL;
            if (sizeof(m_dfu.switch_delay) == p_cmd->para_len) {
                MI_LOG_DEBUG("begin verify, fw_size: %d, pid: %d\n",
                             m_dfu_info.recv_bytes, m_dfu.init.product_id);
                ret = mible_dfu_auth(m_dfu_info.recv_bytes, m_dfu.init.product_id);
                if (MI_SUCCESS == ret) {
                    flags.has_verified = 1;
                    dfu_param.verify.value = MIBLE_DFU_VERIFY_SUCC;
                    param[2] = MIBLE_DFU_STATUS_SUCC;
                    memcpy(&m_dfu.switch_delay, p_cmd->para, p_cmd->para_len);
                    MI_LOG_DEBUG("delay %d seconds\n", m_dfu.switch_delay);
                } else if (MI_ERR_INTERNAL == ret) {
                    param[2] = MIBLE_DFU_STATUS_ERR_AUTH_FAIL;
                } else {
                    param[2] = MIBLE_DFU_STATUS_ERR_UNSIGNED;
                }
            } else {
                param[2] = MIBLE_DFU_STATUS_ERR_INVALID;
            }
            ret = mible_dfu_rsp(MIBLE_DFU_EVENT_RSP_OP_STATUS, param, sizeof(param));

            foreach_callback(MIBLE_DFU_STATE_VERIFY, &dfu_param);
            break;
        }
        default: {
            uint8_t param[3];
            param[0] = LO_UINT16(p_cmd->command);
            param[1] = HI_UINT16(p_cmd->command);
            param[2] = MIBLE_DFU_STATUS_ERR_NO_CMD;
            ret = mible_dfu_rsp(MIBLE_DFU_EVENT_RSP_OP_STATUS, param, sizeof(param));
            break;
        }
    }

    return ret;
}

_attribute_ram_code_ mible_status_t mible_dfu_data(uint8_t * buffer, uint16_t size)
{
    rxfer_process(&m_dfu.rxfer_dfu, buffer, size);
    return MI_SUCCESS;
}

mible_status_t mible_dfu_upgrade(void)
{
    mible_status_t ret = MI_SUCCESS;
    mible_dfu_state_t dfu_state;
    mible_timer_stop(m_dfu.timer_id);

    if (flags.has_verified) {
        dfu_state = MIBLE_DFU_STATE_SWITCH;
        if (0 == m_dfu.switch_delay) {
            MI_LOG_DEBUG("upgrade right now\n");
        } else if (0xFFFFFFFF != m_dfu.switch_delay) {
            mible_timer_start(m_dfu.timer_id, 1000, NULL);
            PT_SEM_SIGNAL(0, &m_dfu.sem_delay);
        }
    } else {
        dfu_state = MIBLE_DFU_STATE_CANCEL;
        mible_timer_delete(m_dfu.timer_id);
        m_dfu.timer_id = NULL;
        MI_LOG_WARNING("no valid firmware\n");
    }

    foreach_callback(dfu_state, NULL);

    switch (dfu_state) {
    case MIBLE_DFU_STATE_SWITCH:
        mible_upgrade_firmware();
        break;
    default:
        break;
    }

    return ret;
}

mible_status_t mible_dfu_callback_register(mible_dfu_callback_t cb)
{
    int index;
    mible_status_t ret;

    if (NULL == cb) {
        return MI_ERR_INVALID_PARAM;
    }

    CRITICAL_SECTION_ENTER();
    for (index = 0; index < MIBLE_DFU_CALLBACK_MAX; ++index) {
        if (cb == dfu_callback[index]) {
            ret = MI_SUCCESS;
            goto end;
        }
    }

    for (index = 0; index < MIBLE_DFU_CALLBACK_MAX; ++index) {
        if (NULL == dfu_callback[index]) {
            dfu_callback[index] = cb;
            ret = MI_SUCCESS;
            goto end;
        }
    }

    ret = MI_ERR_RESOURCES;

end:
    CRITICAL_SECTION_EXIT();

    return ret;
}

mible_status_t mible_dfu_callback_unregister(mible_dfu_callback_t cb)
{
    int index;

    if (NULL == cb) {
        return MI_ERR_INVALID_PARAM;
    }

    CRITICAL_SECTION_ENTER();
    for (index = 0; index < MIBLE_DFU_CALLBACK_MAX; ++index) {
        if (cb == dfu_callback[index]) {
            dfu_callback[index] = NULL;
            break;
        }
    }
    CRITICAL_SECTION_EXIT();

    return MI_SUCCESS;
}

static void mible_dfu_reset(void)
{
    m_dfu.switch_delay = 0xFFFFFFFF;
    memset(&m_dfu_info, 0, sizeof(m_dfu_info));
    MI_LOG_WARNING("reset previous dfu info\n");
}

static mible_status_t mible_dfu_rsp(uint8_t event, uint8_t * param, uint16_t len)
{
    uint8_t tmp[MIBLE_DFU_EVT_SIZE_MAX] = {0};
    mible_dfu_evt_t * p_evt;

    if (NULL == param || len > MIBLE_DFU_EVT_SIZE_MAX - offsetof(mible_dfu_evt_t, para)) {
        return MI_ERR_INVALID_PARAM;
    }

    p_evt = (mible_dfu_evt_t *)tmp;
    p_evt->event = event;
    p_evt->para_len = len;
    memcpy(p_evt->para, param, len);
    if (MI_SUCCESS != enqueue(&m_dfu.evt_queue, tmp)) {
        MI_LOG_ERROR("Fail to save event into dfu queue.\n");
        return MI_ERR_NO_MEM;
    }

    return MI_SUCCESS;
}


#define BUF_SIZE 256
static int mible_dfu_program_flash(pt_t * pt)
{
    PT_BEGIN(pt);

    uint16_t data_size = m_dfu.current_size - sizeof(dfu_fragbuf.index);
    /* Try to write the fragment into flash */
    TIMING_BEGIN();
    mible_dfu_flash_write(dfu_fragbuf.data,
                          data_size,
                          (dfu_fragbuf.index - 1) * m_dfu.fragment_size);
    TIMING_END("WR fragment");

    uint32_t crc32      = dfu_fragbuf.index == 1 ? 0 : m_dfu_info.crc32;
    uint32_t crc32_orig = soft_crc32(dfu_fragbuf.data, data_size, crc32);
    /* Read the copy of fragment to RAM */
    TIMING_BEGIN();
    for (int i = 0, r = data_size % BUF_SIZE; i <= data_size / BUF_SIZE; i++) {
        uint8_t buffer[BUF_SIZE];
        uint32_t offset = i * BUF_SIZE;

        if (i < data_size / BUF_SIZE) {
            mible_dfu_flash_read(buffer, BUF_SIZE, (dfu_fragbuf.index - 1) * m_dfu.fragment_size + offset);
            crc32 = soft_crc32(buffer, BUF_SIZE, crc32);
        } else if (r) {
            mible_dfu_flash_read(buffer, r, (dfu_fragbuf.index - 1) * m_dfu.fragment_size + offset);
            crc32 = soft_crc32(buffer, r, crc32);
        }
    }
    TIMING_END("RD fragment");

    /* Check the integrity of fragment */
    if (crc32 == crc32_orig) {
        if (dfu_fragbuf.index == 1) {
            m_dfu_info.recv_bytes = data_size;
        } else if (dfu_fragbuf.index > m_dfu_info.last_index) {
            m_dfu_info.recv_bytes += data_size;
        }
        MI_LOG_DEBUG("recv amount %d bytes, crc32 %X\n", m_dfu_info.recv_bytes, crc32);
        m_dfu_info.last_index = dfu_fragbuf.index;
        m_dfu_info.crc32      = crc32;
        mible_record_write(RECORD_DFU_INFO, (void*)&m_dfu_info, sizeof(m_dfu_info));

        uint8_t param[3];
        param[0] = MIBLE_DFU_STATUS_SUCC;
        param[1] = LO_UINT16(dfu_fragbuf.index);
        param[2] = HI_UINT16(dfu_fragbuf.index);
        mible_dfu_rsp(MIBLE_DFU_EVENT_FRAG_TRANS_STATUS, param, sizeof(param));
    } else if (--retry_num > 0) {
        MI_LOG_DEBUG("Fail to save fragment (cracked CRC %X), try again\n", crc32);
        PT_RESTART(pt);
    } else {
        PT_EXIT(pt);
    }

    PT_END(pt);
}

static void rsp_frag_xfer_status(uint16_t index, uint8_t stat)
{
    uint8_t param[3];
    param[0] = stat;
    param[1] = LO_UINT16(index);
    param[2] = HI_UINT16(index);
    MI_LOG_DEBUG("rsp fragment status %d\n", stat);
    mible_dfu_rsp(MIBLE_DFU_EVENT_FRAG_TRANS_STATUS, param, sizeof(param));
}

static int mible_dfu_xfer_process(pt_t * pt)
{
    uint8_t stat;

    PT_BEGIN(pt);

    format_rx_cb(&m_dfu.rxfer_dfu, &dfu_fragbuf, m_dfu.fragment_size + 2);
    PT_SPAWN(pt, &m_dfu.pt_rx_ctrl, stat = rxfer_rx_thd(&m_dfu.pt_rx_ctrl,
                                                 &m_dfu.rxfer_dfu,
                                                 PASS_THROUGH,
                                                 &m_dfu.current_size));
    if (stat == PT_EXITED) {
        stat = MIBLE_DFU_STATUS_ERR_RX_FAIL;
        goto EXIT;
    }

    retry_num = MIBLE_DFU_STORE_RETRY_MAX;
    PT_SPAWN(pt, &m_dfu.pt_flash, stat = mible_dfu_program_flash(&m_dfu.pt_flash));
    if (stat == PT_EXITED) {
        stat = MIBLE_DFU_STATUS_ERR_BUSY;
        goto EXIT;
    }

    PT_END(pt);

EXIT:
    flags.is_downloading = 0;
    rsp_frag_xfer_status(dfu_fragbuf.index, stat);
    PT_EXIT(pt);
}

static int mible_dfu_rsp_process(pt_t * pt)
{
    static uint8_t tmp[MIBLE_DFU_EVT_SIZE_MAX];
    mible_dfu_evt_t * p_evt = NULL;

    PT_BEGIN(pt);

    while (MI_SUCCESS == dequeue(&m_dfu.evt_queue, tmp)){
        p_evt = (mible_dfu_evt_t *)tmp;
        PT_WAIT_UNTIL(pt, MI_SUCCESS == mible_gatts_notify_or_indicate(m_dfu.init.conn_handle,
                                         m_dfu.init.srv_handle,
                                         m_dfu.init.ctrl_handle,
                                         0,
                                         tmp,
                                         p_evt->para_len + offsetof(mible_dfu_evt_t, para),
                                         1));
    }

    PT_END(pt);
}

static int mible_dfu_delay_handler(pt_t * pt)
{
    PT_BEGIN(pt);

    PT_SEM_WAIT(pt, &m_dfu.sem_delay);
    if (m_dfu.switch_delay > 0 && m_dfu.switch_delay != 0xFFFFFFFF) {
        m_dfu.switch_delay--;
        if (0 == m_dfu.switch_delay) {
            MI_LOG_DEBUG("start to upgrade\n");
            mible_upgrade_firmware();
        } else {
            PT_SEM_SIGNAL(pt, &m_dfu.sem_delay);
        }
    }

    PT_END(pt);
}

static void mible_dfu_timer_callback(void * arg)
{
    dfu_timer_tick++;

    if (flags.is_downloading && mible_dfu_xfer_process(&m_dfu.pt_xfer) == PT_ENDED) {
        MI_LOG_DEBUG("received fragment %d\n", dfu_fragbuf.index);
    }

    mible_dfu_rsp_process(&m_dfu.pt_rsp);
    mible_dfu_delay_handler(&m_dfu.pt_delay_ctrl);
}
#endif