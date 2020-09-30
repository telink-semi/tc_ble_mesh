//#include <stdint.h>
#include <stdarg.h>
//#include <string.h>

#include "mible_api.h"
#include "mible_trace.h"
#include "mible_port.h"
#include "mi_config.h"
#undef  MI_LOG_MODULE_NAME
#define MI_LOG_MODULE_NAME "RXFER"
#include "mible_log.h"

#include "mible_rxfer.h"
#if MI_API_ENABLE
#define STATE_WAIT_CMD   0x00
#define STATE_WAIT_ACK   0x01
#define STATE_TXD        0x02
#define STATE_RXD        0x03
#define STATE_ERROR      0xFF

#define SEG_CMD          0
#define SEG_ACK          1
#define SGL_CMD          2
#define SGL_ACK          3
#define MNG_CMD          4
#define MNG_ACK          5
#define NIL_TYPE         0xFF
#define FRAME_CTRL       0

#define A_SUCCESS        0
#define A_READY          1
#define A_BUSY           2
#define A_TIMEOUT        3
#define A_CANCEL         4
#define A_LOST           5

#define M_FEATURE        0
#define M_LENGTH         1

#define BLE_CONN_HANDLE_INVALID         0xFFFF
#define MAGIC_NUM                       0xA5
#define MAX_CONSECTIVE_INTERVAL         400

#if defined ( __CC_ARM )
__packed struct reliable_xfer_fctrl_struct {
    uint8_t mode;
    uint8_t type;
    uint8_t arg[MAX_ATT_PAYLOAD - 4];
};
typedef struct reliable_xfer_fctrl_struct reliable_fctrl_t;

__packed struct reliable_xfer_frame_struct {
    uint16_t sn;
    __packed union {
        uint8_t              data[MAX_ATT_PAYLOAD - 2];
        reliable_fctrl_t     ctrl;
    };
};
#elif defined   ( __GNUC__ )
struct __PACKED reliable_xfer_fctrl_struct {
    uint8_t mode;
    uint8_t type;
    uint8_t arg[MAX_ATT_PAYLOAD - 4];
};
typedef struct reliable_xfer_fctrl_struct reliable_fctrl_t;

struct __PACKED reliable_xfer_frame_struct {
    uint16_t sn;
    union __PACKED {
        uint8_t              data[MAX_ATT_PAYLOAD - 2];
        reliable_fctrl_t     ctrl;
    };
};
#endif

typedef struct reliable_xfer_frame_struct reliable_xfer_frame_t;

static uint8_t m_dmtu;
static uint8_t m_simul_retran;
static union {
    uint8_t  unseg_pkt_data[MAX_ATT_PAYLOAD - 4];
    uint16_t lost_pkts[(MAX_ATT_PAYLOAD - 4) / 2];
} m_buf;

static uint8_t is_valid_pkt(void *p_in, uint8_t len)
{
    uint8_t magic[len];
    memset(magic, MAGIC_NUM, len);
    if (memcmp(magic, p_in, len) == 0)
        return 0;
    else
        return 1;
}

static uint8_t is_lost_pkt(uint16_t sn)
{
    // todo support sn above 255.
    uint16_t *p = memchr(m_buf.lost_pkts, sn, 2 * m_simul_retran);
    if (p == NULL) {
        return 0;
    } else {
        *p = 0;
        return 1;
    }
}


static uint16_t find_lost_pkt_behind(rxfer_cb_t *pxfer, uint16_t check_sn)
{
    if (check_sn <= 1)
        check_sn = 1;

    uint8_t *p_pkt = pxfer->pdata + (check_sn - 1) * m_dmtu;

    while ( check_sn < pxfer->max_sn && is_valid_pkt(p_pkt, m_dmtu)) {
        check_sn++;
        p_pkt += m_dmtu;
    }

    // check the last packet rx bytes
    if (check_sn == pxfer->max_sn && pxfer->last_rx_bytes != 0) {
        check_sn++;
    }

    if (check_sn > pxfer->max_sn) {
        check_sn = 1;
        return 0;
    }
    else {
        return check_sn;
    }
}

static uint8_t rxfer_rx_decode(rxfer_cb_t *pxfer, uint8_t *pdata, uint8_t len)
{
    reliable_xfer_frame_t      *pframe = (void*)pdata;
    int16_t                   data_len = len - sizeof(pframe->sn);

    if (data_len > 0)
        memcpy(pxfer->pdata + (pframe->sn - 1) * m_dmtu, pframe->data, data_len);
    else
        MI_LOG_ERROR("rxd data len error. \n");

    return data_len > 0 ? data_len : 0;
}

int format_rx_cb(rxfer_cb_t *pxfer, void *p_rxd, uint16_t rxd_bytes)
{
    pxfer->pdata = p_rxd;
    pxfer->data_size = rxd_bytes;
    pxfer->last_rx_bytes = 0;
    pxfer->recv_sn       = 0;

    // padding magic num
    memset(p_rxd, MAGIC_NUM, rxd_bytes);
    return 0;
}

int format_tx_cb(rxfer_cb_t *pxfer, void *p_txd, uint16_t txd_bytes)
{
    pxfer->pdata = p_txd;
    pxfer->data_size = txd_bytes;
    pxfer->lost_pkt_cnt = 0;
    return 0;
}

static int rxfer_tx_cmd(rxfer_cb_t *pxfer, uint8_t cmd)
{
    reliable_xfer_frame_t       frame = {0};
    uint32_t                    errno;
    uint8_t                     flen;

    if (m_simul_retran == 1 || pxfer->data_size > m_dmtu - 2) {
        // segment packet mode
        frame.ctrl.mode = SEG_CMD;
        frame.ctrl.type = cmd;
        *(uint16_t*)frame.ctrl.arg = CEIL_DIV(pxfer->data_size, m_dmtu);
        flen = sizeof(frame.sn) + 4;
    } else {
        // single packet mode
        frame.ctrl.mode = SGL_CMD;
        frame.ctrl.type = cmd;
        memcpy(frame.ctrl.arg, pxfer->pdata, pxfer->data_size);
        flen = sizeof(frame.sn) + 2 + pxfer->data_size;
    }

    errno = mible_gatts_notify_or_indicate(pxfer->conn_handle,
                                           pxfer->srv_handle,
                                           pxfer->value_handle,
                                           0,
                                           (void*) &frame,
                                           flen,
                                           1);
#if (RXFER_VERBOSE == 1)
    if (errno != MI_SUCCESS) {
        MI_LOG_ERROR("Cann't send CMD %X : %d\n", cmd, errno);
    } else {
        MI_LOG_DEBUG("CMD ");
        MI_LOG_HEXDUMP(&frame, flen);
    }
#endif

    return errno;
}

static int rxfer_tx_data(rxfer_cb_t *pxfer, uint16_t sn)
{
    reliable_xfer_frame_t       frame = {0};
    uint32_t                    errno;
    uint8_t                     flen;

    const uint16_t max_sn = CEIL_DIV(pxfer->data_size, m_dmtu);

    if (sn < max_sn) {
        flen = m_dmtu;
    } else if (sn == max_sn) {
        /* last one */
        flen = pxfer->data_size % m_dmtu;
        flen = flen == 0 ? m_dmtu : flen;
    } else {
        return 0;
    }

    memcpy(frame.data, pxfer->pdata + (sn - 1) * m_dmtu, flen);

    frame.sn = sn;
    flen += sizeof(frame.sn);

    errno = mible_gatts_notify_or_indicate(pxfer->conn_handle,
                                           pxfer->srv_handle,
                                           pxfer->value_handle,
                                           0,
                                           (void*) &frame,
                                           flen,
                                           1);
#if (RXFER_VERBOSE == 1)
    if (errno != MI_SUCCESS) {
        MI_LOG_ERROR("Cann't send pkt %d: %X\n", sn, errno);
    } else {
        MI_LOG_HEXDUMP(&frame, flen);
    }
#endif

    return errno;
}

static int rxfer_rx_ack(rxfer_cb_t *pxfer, uint8_t ack, ...)
{
    reliable_xfer_frame_t       frame = {0};
    uint8_t                     flen;
    uint32_t                    errno;

    frame.ctrl.type = ack;
    flen = sizeof(frame.sn) + sizeof(frame.ctrl.mode) + sizeof(frame.ctrl.type);

    if (pxfer->cmd_type == SGL_CMD) {
        // single mode ack
        frame.ctrl.mode = SGL_ACK;
    } else {
        // legacy mode always work in segment mode
        // segment mode ack
        frame.ctrl.mode = SEG_ACK;
        if (ack == A_LOST) {
            va_list ap;
            va_start(ap, ack);
            uint16_t *sn_list = (uint16_t *)va_arg(ap, int);
            uint8_t       cnt = va_arg(ap, int);
            if ( sn_list != NULL ) {
                memcpy(frame.ctrl.arg, sn_list, cnt * 2);
                flen += 2 * cnt;
            }
            va_end(ap);
        }
    }

    errno = mible_gatts_notify_or_indicate(pxfer->conn_handle,
                                           pxfer->srv_handle,
                                           pxfer->value_handle,
                                           0,
                                           (void*) &frame,
                                           flen,
                                           1);
#if (RXFER_VERBOSE == 1)
    if (errno != MI_SUCCESS) {
        MI_LOG_ERROR("Cann't send ACK %x: %X\n", ack, errno);
        // TODO : catch the exception.
    } else {
        MI_LOG_DEBUG("ACK ");
        MI_LOG_HEXDUMP(&frame, flen);
    }
#endif

    return errno;
}

int rxfer_init(rxfer_cb_t *pxfer, pstimer_t *p_timer)
{
    memset(pxfer, 0, sizeof(rxfer_cb_t));
    pxfer->state = STATE_WAIT_CMD;
    pxfer->p_timer = p_timer;

    m_dmtu = 18;
    m_simul_retran = 1;

    if (sizeof(reliable_xfer_frame_t) != MAX_ATT_PAYLOAD)
        MI_LOG_ERROR("xfer frame is not packed!\n");

    return 0;
}

void rxfer_features_get(uint8_t *p_simul_retran, uint8_t *p_dmtu)
{
    p_simul_retran ? *p_simul_retran = m_simul_retran : 0 ;
    p_dmtu ? *p_dmtu = m_dmtu : 0 ;
}

_attribute_ram_code_ void rxfer_process(rxfer_cb_t *pxfer, uint8_t *pdata, uint8_t len)
{
    reliable_xfer_frame_t *pframe = (void*)pdata;
    uint16_t  curr_sn = pframe->sn;

    switch (pxfer->state) {
    case STATE_WAIT_CMD:
        if (curr_sn != FRAME_CTRL) return;
        if (pframe->ctrl.mode == SEG_CMD && len == 6) {
            uint8_t cmd = pframe->ctrl.type;
            switch (cmd) {
            case PASS_THROUGH:
            case ECC_PUBKEY:
            case DEV_LOGIN_INFO:
            case DEV_SHARE_INFO:
            case SERVER_CERT:
            case SERVER_SIGN:
            case MESH_CONFIG:
            case APP_CONFIRMATION:
            case APP_RANDOM:
            case DEV_CONFIRMATION:
            case DEV_RANDOM:
            case BIND_KEY:
            case WIFI_CONFIG:
                pxfer->cmd_type = SEG_CMD;
                pxfer->cmd = cmd;
                memcpy(&pxfer->max_sn, pframe->ctrl.arg, 2);
                break;
            default:
                MI_LOG_ERROR("Unknow rxfer SEG CMD.\n");
            }
        }
        else if (pframe->ctrl.mode == SGL_CMD && len > 4) {
            uint8_t cmd = pframe->ctrl.type;
            switch (cmd) {
            case PASS_THROUGH:
            case ECC_PUBKEY:
            case DEV_SIGNATURE:
            case DEV_LOGIN_INFO:
            case DEV_SHARE_INFO:
            case SERVER_SIGN:
            case MESH_CONFIG:
            case APP_CONFIRMATION:
            case APP_RANDOM:
            case DEV_CONFIRMATION:
            case DEV_RANDOM:
            case BIND_KEY:
            case WIFI_CONFIG:
                pxfer->cmd_type = SGL_CMD;
                pxfer->cmd = cmd;
                pxfer->max_sn = 1;
                // 4 = 2 bytes sn + 1 byte mode + 1 byte type
                pxfer->last_rx_bytes = len - 4;
                memcpy(m_buf.unseg_pkt_data, pframe->ctrl.arg, pxfer->last_rx_bytes);
                break;
            default:
                MI_LOG_ERROR("Unknow rxfer SGL CMD.\n");
            }
        }
        else if (pframe->ctrl.mode == MNG_CMD) {
            // todo
        }
        break;

    case STATE_WAIT_ACK:
        if (curr_sn != FRAME_CTRL) return;
        if (pframe->ctrl.mode == SEG_ACK && len >= 4) {
            uint8_t  ack = pframe->ctrl.type;
            switch (ack) {
            case A_SUCCESS:
                pxfer->ack_type = SEG_ACK;
                pxfer->ack = ack;
                pxfer->lost_pkt_cnt = 0;
                pxfer->state = STATE_WAIT_CMD;
                break;
            case A_READY:
                pxfer->ack_type = SEG_ACK;
                pxfer->ack = ack;
                pxfer->lost_pkt_cnt = 0;
                break;
            case A_LOST:
                pxfer->ack_type = SEG_ACK;
                pxfer->ack = ack;
                pxfer->lost_pkt_cnt = MIN((len - 4) / 2, m_simul_retran);
                for (int i = 0; i < pxfer->lost_pkt_cnt; i++)
                    memcpy(&m_buf.lost_pkts[pxfer->lost_pkt_cnt-1-i], &pframe->ctrl.arg[i*2], 2);
                break;
            default:
                MI_LOG_ERROR("Unknown rxfer SEG ACK.\n");
            }
        }
        else if (pframe->ctrl.mode == SGL_ACK && len == 4) {
            uint8_t ack = pframe->ctrl.type;
            switch (ack) {
            case A_SUCCESS:
            case A_BUSY:
            case A_CANCEL:
                pxfer->ack_type = SGL_ACK;
                pxfer->ack = ack;
                pxfer->state = STATE_WAIT_CMD;
                break;
            default:
                MI_LOG_ERROR("Unknown rxfer SGL ACK.\n");
            }
        }
        else if (pframe->ctrl.mode == MNG_ACK) {
            uint8_t ack = pframe->ctrl.type;
            switch (ack) {
            case M_FEATURE:
                memcpy(m_buf.unseg_pkt_data, pframe->ctrl.arg, 2);
                pxfer->ack_type = MNG_ACK;
                pxfer->state = STATE_WAIT_CMD;
                break;
            case M_LENGTH:
                memcpy(m_buf.unseg_pkt_data, pframe->ctrl.arg, len - 4);
                pxfer->last_rx_bytes = len - 4;
                pxfer->ack_type = MNG_ACK;
                pxfer->state = STATE_WAIT_CMD;
                break;
            default:
                MI_LOG_ERROR("Unknown rxfer MNG ACK.\n");
            }

        }
        break;

    case STATE_RXD:
        if (curr_sn == FRAME_CTRL) return;

        if (pxfer->lost_pkt_cnt > 0 && is_lost_pkt(curr_sn))
            pxfer->lost_pkt_cnt--;

        pxfer->recv_sn = curr_sn;
        if (curr_sn < pxfer->max_sn && len == m_dmtu + 2) {
            rxfer_rx_decode(pxfer, pdata, len);
        } else if (curr_sn == pxfer->max_sn) {
            if (pxfer->max_sn == CEIL_DIV(pxfer->data_size, m_dmtu)) {
                uint8_t last_pkt_bytes = pxfer->data_size % m_dmtu;
                last_pkt_bytes = last_pkt_bytes ? last_pkt_bytes : m_dmtu;
                last_pkt_bytes += sizeof(pframe->sn);
                pxfer->last_rx_bytes = rxfer_rx_decode(pxfer, pdata, MIN(len, last_pkt_bytes));
            } else {
                pxfer->last_rx_bytes = rxfer_rx_decode(pxfer, pdata, len);
            }
        } else {
            MI_LOG_ERROR("recv illegal pkt: SN %d(%d), length %d(%d)\n", curr_sn, pxfer->max_sn, len, m_dmtu+2);
            pxfer->recv_sn = 0;
            // TODO: handle this exception...
        }
        break;

    default:
        MI_LOG_ERROR("rxfer error state.\n");
        break;
    }
}

PT_THREAD(rxfer_rx_thd(pt_t *pt, rxfer_cb_t *pxfer, uint8_t data_type, ...))
{
    static uint16_t n;

    // TODO : exception handler
    if (pxfer->conn_handle == BLE_CONN_HANDLE_INVALID || pxfer->pdata == NULL) {
        MI_LOG_ERROR("rx internal error: not connected or rx buffer not ready.\n");
        PT_EXIT(pt);
    }

    PT_BEGIN(pt);

    /* Recive segment cmd or unsegment data */
    // single or segment packet mode
    PT_WAIT_UNTIL(pt, (pxfer->cmd_type == SGL_CMD || pxfer->cmd_type == SEG_CMD) && pxfer->cmd == data_type && pxfer->max_sn >= 1);
    if (pxfer->cmd_type == SGL_CMD) {
        memcpy(pxfer->pdata, m_buf.unseg_pkt_data, MIN(pxfer->last_rx_bytes, pxfer->data_size));
        PT_WAIT_UNTIL(pt, rxfer_rx_ack(pxfer, A_SUCCESS) == MI_SUCCESS);
    } else {
        /* Send segment ack */
        if (pxfer->max_sn <= CEIL_DIV(pxfer->data_size, m_dmtu)) {
            PT_WAIT_UNTIL(pt, rxfer_rx_ack(pxfer, A_READY) == MI_SUCCESS);
            pxfer->state = STATE_RXD;
            pxfer->lost_pkt_cnt = 0;
        } else {
            PT_WAIT_UNTIL(pt, rxfer_rx_ack(pxfer, A_CANCEL) == MI_SUCCESS);
            pxfer->max_sn = 0;
            PT_EXIT(pt);
        }

        n = 0;
        /* Waiting here until all segments has be received, or no new one receive in a moment */
        pstimer_set(pxfer->p_timer, MAX_CONSECTIVE_INTERVAL);
        while (n < pxfer->max_sn && !pstimer_expired(pxfer->p_timer, NULL)) {
            if (n < pxfer->recv_sn) {
                n = pxfer->recv_sn;
                pstimer_set(pxfer->p_timer, MAX_CONSECTIVE_INTERVAL);
            }
            PT_YIELD(pt);
        }

        /* do lost segments retransmit */
        n = 1;
        while(1) {
            // Find lost segments
            pxfer->lost_pkt_cnt = 0;
            for (uint16_t i = 0, j = 0; i < m_simul_retran; i++) {
                j = find_lost_pkt_behind(pxfer, j+1);
                if (j == 0) break;
                m_buf.lost_pkts[i] = j;
                pxfer->lost_pkt_cnt++;
            }

            // Wait until retransmit completed or timeout
            if (pxfer->lost_pkt_cnt == 0) {
                PT_WAIT_UNTIL(pt, rxfer_rx_ack(pxfer, A_SUCCESS) == MI_SUCCESS);
                break;
            } else {
                MI_LOG_ERROR("lost packets: ");
                MI_LOG_HEXDUMP(m_buf.lost_pkts, pxfer->lost_pkt_cnt * 2);
                PT_WAIT_UNTIL(pt, rxfer_rx_ack(pxfer, A_LOST, m_buf.lost_pkts, pxfer->lost_pkt_cnt) == MI_SUCCESS);
                pstimer_set(pxfer->p_timer, MAX_CONSECTIVE_INTERVAL * CEIL_DIV(m_simul_retran, 6));
                PT_WAIT_UNTIL(pt, pxfer->lost_pkt_cnt == 0 || pstimer_expired(pxfer->p_timer, NULL));
                if (pxfer->lost_pkt_cnt != 0) {
                    n++;
                }
            }

            // If timeout happened 3 times, this rx procedure should be aborted.
            if (n > 3) {
                PT_WAIT_UNTIL(pt, rxfer_rx_ack(pxfer, A_TIMEOUT) == MI_SUCCESS);
                pxfer->state = STATE_WAIT_CMD;
                pxfer->pdata = NULL;
                pxfer->data_size = 0;
                PT_EXIT(pt);
            }
        }
    }

    if (data_type == SERVER_CERT  ||
        data_type == PASS_THROUGH ||
        data_type == MESH_CONFIG  ||
        data_type == WIFI_CONFIG) {
        va_list ap;
        va_start(ap, data_type);
        uint16_t * arg = (uint16_t*)(va_arg(ap, int));
        if ( arg != NULL ) {
            *arg = m_dmtu * (pxfer->max_sn - 1) + pxfer->last_rx_bytes;
        }
        va_end(ap);
    }

    pxfer->state         = STATE_WAIT_CMD;
    pxfer->cmd_type      = NIL_TYPE;
    pxfer->pdata         = NULL;
    pxfer->data_size     = 0;
    pxfer->last_rx_bytes = 0;
    PT_END(pt);
}

PT_THREAD(rxfer_tx_thd(pt_t *pt, rxfer_cb_t *pxfer, uint8_t data_type))
{
    static uint16_t sn;

    // TODO : exception handler
    if (pxfer->conn_handle == BLE_CONN_HANDLE_INVALID || pxfer->pdata == NULL) {
        MI_LOG_ERROR("tx internal error: not connected or no memory.\n");
        PT_EXIT(pt);
    }

    PT_BEGIN(pt);

    /* Send segment cmd or unsegment data */
    PT_WAIT_UNTIL(pt, rxfer_tx_cmd(pxfer, data_type) == MI_SUCCESS);
    pxfer->state = STATE_WAIT_ACK;
    pxfer->ack_type  = NIL_TYPE;

    if (m_simul_retran == 1 || pxfer->data_size > m_dmtu - 2) {
        PT_WAIT_UNTIL(pt, pxfer->ack_type == SEG_ACK);
        if (pxfer->ack != A_READY)
            PT_EXIT(pt);
    } else {
        PT_WAIT_UNTIL(pt, pxfer->ack_type == SGL_ACK);
        if (pxfer->ack != A_SUCCESS)
            PT_RESTART(pt);
        else
            goto EXIT;
    }

    /* Send segment data */
    sn = 1;
    while(sn <= CEIL_DIV(pxfer->data_size, m_dmtu)) {
        PT_WAIT_UNTIL(pt, rxfer_tx_data(pxfer, sn) == MI_SUCCESS);
        sn++;
    }

    /* Retrans lost segment data */
    pxfer->state = STATE_WAIT_ACK;
    while(pxfer->ack_type == SEG_ACK && pxfer->ack != A_SUCCESS) {
        PT_WAIT_UNTIL(pt, pxfer->lost_pkt_cnt != 0 || pxfer->ack == A_SUCCESS);
        if (pxfer->ack == A_SUCCESS) {
            break;
        } else {
            PT_WAIT_UNTIL(pt, rxfer_tx_data(pxfer, m_buf.lost_pkts[pxfer->lost_pkt_cnt-1]) == MI_SUCCESS);
            pxfer->lost_pkt_cnt--;
        }
    }

EXIT:
    pxfer->state     = STATE_WAIT_CMD;
    pxfer->data_size = 0;
    pxfer->pdata     = NULL;
    PT_END(pt);
}


static int rxfer_tx_mng_cmd(rxfer_cb_t *pxfer, uint8_t cmd, uint8_t arg0, uint8_t arg1)
{
    reliable_xfer_frame_t       frame = {0};
    uint8_t                     flen;

    frame.ctrl.mode = MNG_CMD;
    frame.ctrl.type = cmd;
    flen = sizeof(frame.sn) + sizeof(frame.ctrl.mode) + sizeof(frame.ctrl.type);

    if (cmd == M_FEATURE) {
        frame.ctrl.arg[0] = arg0;
        frame.ctrl.arg[1] = arg1;
        flen += 2;
    }
    if (cmd == M_LENGTH) {
        uint8_t dmtu = arg0;
        memset(frame.ctrl.arg, dmtu, dmtu - 2);
        flen += dmtu - 2;
    }

    uint8_t errno = mible_gatts_notify_or_indicate(pxfer->conn_handle,
                                           pxfer->srv_handle,
                                           pxfer->value_handle,
                                           0,
                                           (void*) &frame,
                                           flen,
                                           1);

    if (errno != MI_SUCCESS) {
        MI_LOG_ERROR("Cann't send MNG %x, error %X\n", cmd, errno);
        // TODO : catch the exception.
    } else {
#if (RXFER_VERBOSE == 1)
        MI_LOG_ERROR("MNG CMD ");
        MI_LOG_HEXDUMP(&frame, flen);
#endif
    }

    return errno;
}

PT_THREAD(rxfer_mng_thd(pt_t *pt, rxfer_cb_t *pxfer, uint8_t cmd, ...))
{
    static uint8_t              exchanged_dmtu;

    va_list ap;
    va_start(ap, cmd);
    int arg0 = va_arg(ap, int);
    int arg1 = va_arg(ap, int);
    va_end(ap);

    PT_BEGIN(pt);
    PT_WAIT_UNTIL(pt, rxfer_tx_mng_cmd(pxfer, cmd, arg0, arg1) == MI_SUCCESS);
    pxfer->state = STATE_WAIT_ACK;
    pxfer->ack_type = NIL_TYPE;
    pstimer_set(pxfer->p_timer, MAX_CONSECTIVE_INTERVAL);
    PT_WAIT_UNTIL(pt, pxfer->ack_type == MNG_ACK || pstimer_expired(pxfer->p_timer, NULL));
    if (!pstimer_expired(pxfer->p_timer, NULL)) {
        if (cmd == M_FEATURE) {
            m_simul_retran = MIN(arg0, m_buf.unseg_pkt_data[0]);
            exchanged_dmtu = MIN(arg1, m_buf.unseg_pkt_data[1]);
#if (RXFER_VERBOSE == 1)
            MI_LOG_DEBUG("simul retrans cnt %d, exchanged dmtu %d\n", m_simul_retran, exchanged_dmtu);
#endif
        }
        if (cmd == M_LENGTH) {
            uint8_t len = pxfer->last_rx_bytes;
            uint8_t detect_dmtu = len + 2;
            if (exchanged_dmtu == detect_dmtu && m_buf.unseg_pkt_data[len-1] == detect_dmtu)
                m_dmtu = exchanged_dmtu;
            else
                m_dmtu = 18;
#if (RXFER_VERBOSE == 1 && MI_LOG_ENABLED)
            if (m_buf.unseg_pkt_data[len-1] != detect_dmtu)
                MI_LOG_ERROR("RX pkt len %d is not match EX DMTU %d\n", detect_dmtu, exchanged_dmtu);
            else
                MI_LOG_DEBUG("detect max dmtu %d\n", m_dmtu);
#endif
        }
    } else {
        pxfer->state = STATE_WAIT_CMD;
        MI_LOG_DEBUG("timeout\n");
        PT_EXIT(pt);
    }

    pxfer->state = STATE_WAIT_CMD;
    PT_END(pt);
}
#endif