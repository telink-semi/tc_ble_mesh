/* Includes ------------------------------------------------------------------*/
//#include <stddef.h>
//#include <string.h>
#include "mi_config.h"
#include "mible_api.h"
#include "mible_log.h"
#include "mible_trace.h"
#include "common/crc32.h"
#include "common/mible_rxfer.h"
#include "common/queue.h"
#include "cryptography/mi_crypto.h"
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
#define MIBLE_DFU_STATUS_ERR_LOW_BATTERY                       0x08
#define MIBLE_DFU_STATUS_ERR_UNKNOWN                           0xFF

#define MIBLE_DFU_TIMER_PERIOD                                 20
#define MIBLE_DFU_EVT_SIZE_MAX                                 12
#define MIBLE_DFU_EVT_CNT_MAX                                  1
#define MIBLE_DFU_STORE_RETRY_MAX                              3

#define RECORD_DFU_INFO                                        5

#define HI_UINT16(a)                            (((uint16_t)(a) >> 8) & 0xFF)
#define LO_UINT16(a)                            ((uint16_t)(a) & 0xFF)

#if (HAVE_MSC)
#define MSC_POWER_ON(pt)                                                        \
do {                                                                            \
    mi_msc_config(NULL, NULL, &dfu_timer);                                      \
    PT_WAIT_THREAD(pt, msc_power_on());                                         \
} while(0)

#define MSC_POWER_OFF(pt)                                                       \
do {                                                                            \
    PT_WAIT_THREAD(pt, msc_power_off());                                        \
} while(0)
#else
#define MSC_POWER_ON(pt)
#define MSC_POWER_OFF(pt)
#endif /* HAVE_MSC */


/* Private typedef -----------------------------------------------------------*/

typedef struct {
    mible_dfu_init_t init;
    uint16_t current_size;
    uint32_t switch_delay;
    void *   timer_id;
    rxfer_cb_t rxfer_dfu;
    pt_t     pt_xfer;
    pt_t     pt_rsp;
    pt_t     pt_switch;
    pt_t     pt_verify;
    pt_t     pt_terminate;
    queue_t  evt_queue;
    uint16_t fragment_size;
    uint8_t  conn_stat;
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
static char  mible_dfu_program_flash(pt_t * pt);
static char  mible_dfu_xfer_process(pt_t * pt);
static char  mible_dfu_rsp_process(pt_t * pt);
static char  mible_dfu_switch_process(pt_t * pt);

/* Private variables ---------------------------------------------------------*/
static uint8_t retry_num;
static mible_dfu_config_t m_dfu;
#if MINIMIZE_RAM_SIZE
#include <stdlib.h>
#define NEW(obj, size)       \
do {obj = malloc(size); } while(0)
#define FREE(obj)            \
do {if (obj != NULL) {free(obj); obj = NULL;}} while(0)
static mible_dfu_fragment_t *p_buf;
#else
#define NEW(obj, size)
#define FREE(obj)
static mible_dfu_fragment_t dfu_buf;
static mible_dfu_fragment_t *p_buf = &dfu_buf;
#endif
static uint8_t evt_buffer[MIBLE_DFU_EVT_SIZE_MAX * MIBLE_DFU_EVT_CNT_MAX];
static uint32_t dfu_timer_tick;
static pstimer_t dfu_timer = {
        .p_curr_tick = &dfu_timer_tick,
        .tick_interval_ms = MIBLE_DFU_TIMER_PERIOD,
};
static mible_dfu_callback_t dfu_callback;
static pre_check_func_t dfu_pre_check;

#define _IDLE          0
#define _PREPARE       1
#define _DOWNLOADING   2
#define _NEED_VERIFY   3
#define _READY_TO_GO   4
#define _TERMINATED    5
static uint8_t curr_state;
static struct {
    uint8_t  version  :4;
    uint8_t  rfu      :4;
    uint8_t  rfu8     :8;
    uint16_t last_index;
    uint32_t crc32;
    uint32_t recv_bytes;
    uint32_t activate_delay;
} m_dfu_info;

static void run_callback(mible_dfu_state_t state, mible_dfu_param_t *param)
{
    if (dfu_callback)
        dfu_callback(state, param);
}

/* Exported functions --------------------------------------------------------*/
mible_status_t mible_dfu_init(mible_dfu_init_t * p_init)
{
    mible_status_t ret;

    if (NULL == p_init) {
        return MI_ERR_INVALID_PARAM;
    }

    /* Init parameters for DFU config */
    m_dfu.init         = *p_init;
    m_dfu.conn_stat    = 1;
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
    PT_INIT(&m_dfu.pt_rsp);
    PT_INIT(&m_dfu.pt_xfer);
    PT_INIT(&m_dfu.pt_verify);
    PT_INIT(&m_dfu.pt_switch);
    PT_INIT(&m_dfu.pt_terminate);

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

    if (NULL == p_cmd ||
        size < offsetof(mible_dfu_cmd_t, para) ||
        size != offsetof(mible_dfu_cmd_t, para) + p_cmd->para_len) {
        return ret;
    }

    switch (p_cmd->command) {
        case MIBLE_DFU_REQ_PROTOCOL_VERSION: {
            curr_state = _PREPARE;
            uint8_t param[5];
            param[0] = LO_UINT16(MIBLE_DFU_REQ_PROTOCOL_VERSION);
            param[1] = HI_UINT16(MIBLE_DFU_REQ_PROTOCOL_VERSION);
            param[2] = MIBLE_DFU_STATUS_SUCC;
            param[3] = LO_UINT16(PROTOCAL_VERSION);
            param[4] = HI_UINT16(PROTOCAL_VERSION);
            ret = mible_dfu_rsp(MIBLE_DFU_EVENT_RSP_OP_STATUS, param, sizeof(param));
            mible_timer_start(m_dfu.timer_id, MIBLE_DFU_TIMER_PERIOD, NULL);
            break;
        }
        case MIBLE_DFU_REQ_FRAGMENT_SIZE: {
            uint8_t simul_retrans, dmtu;
            rxfer_features_get(&simul_retrans, &dmtu);
            m_dfu.fragment_size = dmtu > 18 ? MAX_FRAGMENT_SIZE : 512;

            uint8_t param[5];
            param[0] = LO_UINT16(MIBLE_DFU_REQ_FRAGMENT_SIZE);
            param[1] = HI_UINT16(MIBLE_DFU_REQ_FRAGMENT_SIZE);

            NEW(p_buf, sizeof(mible_dfu_fragment_t));
            if (p_buf == NULL)
                param[2] = MIBLE_DFU_STATUS_ERR_NO_MEM;
            else
                param[2] = dfu_pre_check == NULL ? MIBLE_DFU_STATUS_SUCC : dfu_pre_check();

            param[3] = LO_UINT16(m_dfu.fragment_size);
            param[4] = HI_UINT16(m_dfu.fragment_size);

            ret = mible_dfu_rsp(MIBLE_DFU_EVENT_RSP_OP_STATUS, param, sizeof(param));

            if (param[2] == MIBLE_DFU_STATUS_SUCC) {
                mible_dfu_param_t dfu_param = {
                        .start.fragment_size = m_dfu.fragment_size,
                };
                run_callback(MIBLE_DFU_STATE_START, &dfu_param);
                curr_state = _DOWNLOADING;
            } else {
                curr_state = _TERMINATED;
            }
            mible_timer_start(m_dfu.timer_id, MIBLE_DFU_TIMER_PERIOD, NULL);
            break;
        }
        case MIBLE_DFU_REQ_LAST_FRAGMENT: {
            if (mible_record_read(RECORD_DFU_INFO, (void*)&m_dfu_info, sizeof(m_dfu_info)) != MI_SUCCESS)
                memset(&m_dfu_info, 0, sizeof(m_dfu_info));

            MI_LOG_INFO("last index %d, crc32 %x\n", m_dfu_info.last_index, m_dfu_info.crc32);

            uint8_t param[9];
            param[0] = LO_UINT16(MIBLE_DFU_REQ_LAST_FRAGMENT);
            param[1] = HI_UINT16(MIBLE_DFU_REQ_LAST_FRAGMENT);
            param[2] = MIBLE_DFU_STATUS_SUCC;
            param[3] = LO_UINT16(m_dfu_info.last_index);
            param[4] = HI_UINT16(m_dfu_info.last_index);
            memcpy(&param[5], &m_dfu_info.crc32, 4);
            ret = mible_dfu_rsp(MIBLE_DFU_EVENT_RSP_OP_STATUS, param, sizeof(param));
            break;
        }
        case MIBLE_DFU_REQ_ACTIVATE_NEW_FW: {
            if (sizeof(m_dfu.switch_delay) != p_cmd->para_len) {
                uint8_t param[3];
                param[0] = LO_UINT16(MIBLE_DFU_REQ_ACTIVATE_NEW_FW);
                param[1] = HI_UINT16(MIBLE_DFU_REQ_ACTIVATE_NEW_FW);
                param[2] = MIBLE_DFU_STATUS_ERR_INVALID;
                ret = mible_dfu_rsp(MIBLE_DFU_EVENT_RSP_OP_STATUS, param, sizeof(param));
            } else {
                curr_state = _NEED_VERIFY;
                mible_record_delete(RECORD_DFU_INFO);
                memcpy(&m_dfu.switch_delay, p_cmd->para, p_cmd->para_len);
            }
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
    if (curr_state == _READY_TO_GO) {
        m_dfu.conn_stat = 0;
        return MI_SUCCESS;
    } else if (curr_state != _IDLE) {
        curr_state = _TERMINATED;
        return MI_ERR_NOT_FOUND;
    }
    return MIBLE_ERR_UNKNOWN;
}

mible_status_t mible_dfu_callback_register(mible_dfu_callback_t cb)
{
    mible_status_t ret;

    if (NULL == cb) {
        return MI_ERR_INVALID_PARAM;
    }

    CRITICAL_SECTION_ENTER();
    if (cb == dfu_callback) {
        ret = MI_SUCCESS;
        goto end;
    }

    if (NULL == dfu_callback) {
        dfu_callback = cb;
        ret = MI_SUCCESS;
        goto end;
    }

    ret = MI_ERR_RESOURCES;

end:
    CRITICAL_SECTION_EXIT();

    return ret;
}

mible_status_t mible_dfu_callback_unregister(mible_dfu_callback_t cb)
{
    if (NULL == cb) {
        return MI_ERR_INVALID_PARAM;
    }

    CRITICAL_SECTION_ENTER();
    dfu_callback = NULL;
    CRITICAL_SECTION_EXIT();

    return MI_SUCCESS;
}

void mible_dfu_pre_check_register(pre_check_func_t fn)
{
    dfu_pre_check = fn;
}

void mible_dfu_pre_check_unregister(pre_check_func_t fn)
{
    dfu_pre_check = NULL;
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
static PT_THREAD(mible_dfu_program_flash(pt_t * pt))
{
    PT_BEGIN(pt);

    uint16_t data_size = m_dfu.current_size - sizeof(p_buf->index);
    /* Try to write the fragment into flash */
    TIMING_BEGIN();
    mible_dfu_flash_write(p_buf->data,
                          data_size,
                          (p_buf->index - 1) * m_dfu.fragment_size);
    TIMING_END("WR fragment");

    uint32_t crc32      = p_buf->index == 1 ? 0 : m_dfu_info.crc32;
    uint32_t crc32_orig = soft_crc32(p_buf->data, data_size, crc32);
    /* Read the copy of fragment to RAM */
    TIMING_BEGIN();
    for (int i = 0, r = data_size % BUF_SIZE; i <= data_size / BUF_SIZE; i++) {
        uint8_t buffer[BUF_SIZE];
        uint32_t offset = i * BUF_SIZE;

        if (i < data_size / BUF_SIZE) {
            mible_dfu_flash_read(buffer, BUF_SIZE, (p_buf->index - 1) * m_dfu.fragment_size + offset);
            crc32 = soft_crc32(buffer, BUF_SIZE, crc32);
        } else if (r) {
            mible_dfu_flash_read(buffer, r, (p_buf->index - 1) * m_dfu.fragment_size + offset);
            crc32 = soft_crc32(buffer, r, crc32);
        }
    }
    TIMING_END("RD fragment");

    /* Check the integrity of fragment */
    if (crc32 == crc32_orig) {
        if (p_buf->index == 1) {
            m_dfu_info.recv_bytes = data_size;
        } else if (p_buf->index > m_dfu_info.last_index) {
            m_dfu_info.recv_bytes += data_size;
        }
        MI_LOG_DEBUG("recv amount %d bytes, crc32 %X\n", m_dfu_info.recv_bytes, crc32);
        m_dfu_info.last_index = p_buf->index;
        m_dfu_info.crc32      = crc32;
		if(m_dfu_info.last_index %4 == 0){
			mible_record_write(RECORD_DFU_INFO, (void*)&m_dfu_info, sizeof(m_dfu_info));
		}
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

static PT_THREAD(mible_dfu_xfer_process(pt_t * pt))
{
    static pt_t pt_rx, pt_flash;
    uint8_t stat;

    PT_BEGIN(pt);

    format_rx_cb(&m_dfu.rxfer_dfu, p_buf, m_dfu.fragment_size + 2);
    PT_SPAWN(pt, &pt_rx, stat = rxfer_rx_thd(&pt_rx,
                                             &m_dfu.rxfer_dfu,
                                             PASS_THROUGH,
                                             &m_dfu.current_size));
    if (stat == PT_EXITED) {
        rsp_frag_xfer_status(p_buf->index, MIBLE_DFU_STATUS_ERR_RX_FAIL);
        goto EXIT;
    }

    retry_num = MIBLE_DFU_STORE_RETRY_MAX;
    PT_SPAWN(pt, &pt_flash, stat = mible_dfu_program_flash(&pt_flash));
    if (stat == PT_EXITED) {
        rsp_frag_xfer_status(p_buf->index, MIBLE_DFU_STATUS_ERR_BUSY);
		mible_dfu_param_t dfu_param;
        run_callback(MIBLE_DFU_STATE_WRITEERR,&dfu_param);
        goto EXIT;
    } else {
        rsp_frag_xfer_status(p_buf->index, MIBLE_DFU_STATUS_SUCC);
        MI_LOG_DEBUG("successfully recv fragment %d\n", p_buf->index);
    }

    PT_END(pt);

EXIT:
    PT_EXIT(pt);
}

static PT_THREAD(mible_dfu_rsp_process(pt_t * pt))
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

static PT_THREAD(mible_dfu_verify_process(pt_t *pt))
{
    int stat;
    static uint8_t ret = MIBLE_ERR_UNKNOWN;
    dfu_ctx_t *p_ctx = (dfu_ctx_t*)p_buf;
    ecc256_pk_t parent_pub = {
        0xbe,0xf5,0x8b,0x02,0xda,0xe3,0xff,0xf8,0x54,0x1a,0xa0,0x44,0x8f,0xba,0xc4,0x4d,
        0xb7,0xc6,0x9a,0x2f,0xa8,0xf0,0xb1,0xb6,0xff,0x7a,0xd9,0x51,0xdb,0x66,0x28,0xfa,
        0xd7,0xf0,0x20,0xea,0x39,0xa2,0xee,0x86,0x7f,0xdd,0x78,0x3f,0xdc,0x2f,0xb0,0x86,
        0x09,0x5c,0xc2,0x85,0x04,0x13,0xa2,0x80,0x2c,0x62,0x7d,0xbd,0xc7,0x15,0xf4,0xf9,
    };
    uint8_t child_sha[32];
    uint8_t child_sig[64];

    PT_BEGIN(pt);
    MSC_POWER_ON(pt);
    MI_LOG_DEBUG("begin verify, fw_size: %d, pid: %d\n",
                  m_dfu_info.recv_bytes, m_dfu.init.product_id);

    if (mible_dfu_auth(p_ctx, &m_dfu.init, m_dfu_info.recv_bytes) == MI_SUCCESS) {
        mi_crypto_sha256(p_ctx->server_crt.tbs.p, p_ctx->server_crt.tbs.len, child_sha);
        memcpy(child_sig, p_ctx->server_crt.sig, 64);
        PT_WAIT_THREAD(pt, stat = mi_crypto_ecc_verify(P256R1, parent_pub, child_sha, child_sig));
        if (stat != PT_ENDED) {
            MI_LOG_ERROR("Server cert is invalid.\n");
            ret = MI_ERR_INTERNAL;
            goto EXIT;
        } else {
            MI_LOG_WARNING("Server cert is valid.\n");
        }

        memcpy(parent_pub, p_ctx->server_crt.pub.p, 64);
        mi_crypto_sha256(p_ctx->developer_crt.tbs.p, p_ctx->developer_crt.tbs.len, child_sha);
        memcpy(child_sig, p_ctx->developer_crt.sig, 64);
        PT_WAIT_THREAD(pt, stat = mi_crypto_ecc_verify(P256R1, parent_pub, child_sha, child_sig));
        if (stat != PT_ENDED) {
            MI_LOG_ERROR("Developer cert is invalid.\n");
            ret = MI_ERR_INTERNAL;
            goto EXIT;
        } else {
            MI_LOG_WARNING("Developer cert is valid.\n");
        }

        memcpy(parent_pub, p_ctx->developer_crt.pub.p, 64);
        memcpy(child_sha, p_ctx->pack_sha, 32);
        memcpy(child_sig, p_ctx->pack_sig, 64);
        PT_WAIT_THREAD(pt, stat = mi_crypto_ecc_verify(P256R1, parent_pub, child_sha, child_sig));
        if (stat != PT_ENDED) {
            MI_LOG_ERROR("FW pack signature is invalid.\n");
            ret = MI_ERR_INTERNAL;
        } else {
            MI_LOG_WARNING("FW pack signature is valid.\n");
            ret = MI_SUCCESS;
        }
    } else {
        MI_LOG_ERROR("PARSE TAG / CERT FAILED\n");
    }

EXIT:
    MSC_POWER_OFF(pt);
    {
        uint8_t param[3];
        param[0] = LO_UINT16(MIBLE_DFU_REQ_ACTIVATE_NEW_FW);
        param[1] = HI_UINT16(MIBLE_DFU_REQ_ACTIVATE_NEW_FW);
        if (MI_SUCCESS == ret) {
            curr_state = _READY_TO_GO;
            param[2] = MIBLE_DFU_STATUS_SUCC;
        } else if (MI_ERR_INTERNAL == ret) {
            curr_state = _TERMINATED;
            param[2] = MIBLE_DFU_STATUS_ERR_AUTH_FAIL;
        } else {
            curr_state = _TERMINATED;
            param[2] = MIBLE_DFU_STATUS_ERR_UNSIGNED;
        }
        mible_dfu_rsp(MIBLE_DFU_EVENT_RSP_OP_STATUS, param, sizeof(param));

        mible_dfu_param_t dfu_param = {
                .verify.value = param[2],
        };
        run_callback(MIBLE_DFU_STATE_VERIFY, &dfu_param);
    }
    PT_END(pt);
}

static PT_THREAD(mible_dfu_switch_process(pt_t * pt))
{
    mible_dfu_state_t dfu_state;

    PT_BEGIN(pt);

    /* Wait until peer disconnect, make sure not to upgrade while connected*/
    PT_WAIT_UNTIL(pt, m_dfu.conn_stat == 0);

    mible_timer_stop(m_dfu.timer_id);
    if (m_dfu.switch_delay == 0) {
        MI_LOG_DEBUG("Upgrade immediately\n");
        dfu_state = MIBLE_DFU_STATE_SWITCH;
        mible_dfu_param_t dfu_param = {
                .switch_plan.up_to_app = 0,
        };
        run_callback(dfu_state, &dfu_param);
        mible_upgrade_firmware();
    } else if (m_dfu.switch_delay < 60) {
        MI_LOG_DEBUG("Upgrade will start in %d seconds\n", m_dfu.switch_delay);
        mible_timer_start(m_dfu.timer_id, m_dfu.switch_delay * 1000, NULL);
        m_dfu.switch_delay = 0;
    } else if (m_dfu.switch_delay < 0xFFFFFFFE) {
        MI_LOG_DEBUG("Upgrade will start in %d seconds\n", m_dfu.switch_delay);
        mible_timer_start(m_dfu.timer_id, 60 * 1000, NULL);
        m_dfu.switch_delay -= 60;
    } else if (m_dfu.switch_delay == 0xFFFFFFFE) {
        dfu_state = MIBLE_DFU_STATE_SWITCH;
        mible_dfu_param_t dfu_param = {
                .switch_plan.up_to_app = 1,
        };
        run_callback(dfu_state, &dfu_param);
    }

    PT_END(pt);
}

static PT_THREAD(mible_dfu_terminate_process(pt_t * pt))
{
    mible_dfu_state_t dfu_state;

    PT_BEGIN(pt);

    MSC_POWER_OFF(pt);

    PT_WAIT_UNTIL(pt, m_dfu.conn_stat == 0 || m_dfu.evt_queue.cnt == 0);

    FREE(p_buf);

    curr_state = _IDLE;
    dfu_state = MIBLE_DFU_STATE_CANCEL;
    run_callback(dfu_state, NULL);

    mible_timer_stop(m_dfu.timer_id);
    mible_timer_delete(m_dfu.timer_id);
    m_dfu.timer_id = NULL;

    PT_END(pt);
}


static void mible_dfu_timer_callback(void * arg)
{
    dfu_timer_tick++;

    switch (curr_state) {
    case _DOWNLOADING:
        mible_dfu_xfer_process(&m_dfu.pt_xfer);
        break;

    case _NEED_VERIFY:
        mible_dfu_verify_process(&m_dfu.pt_verify);
        break;

    case _READY_TO_GO:
        mible_dfu_switch_process(&m_dfu.pt_switch);
        break;

    case _TERMINATED:
        mible_dfu_terminate_process(&m_dfu.pt_terminate);
        break;
    }

    mible_dfu_rsp_process(&m_dfu.pt_rsp);
}
#endif