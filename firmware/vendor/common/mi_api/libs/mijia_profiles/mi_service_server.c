/* Copyright (c) 2010-2017 Xiaomi. All Rights Reserved.
 *
 * The information contained herein is property of Xiaomi.
 * Terms and conditions of usage are described in detail in 
 * STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */
#include <stdarg.h>
//#include <stdlib.h>
//#include <string.h>

#include "mi_config.h"
#include "mible_api.h"
#include "gatt_dfu/mible_dfu_main.h"
#include "common/mible_crypto.h"
#include "common/mible_beacon_internal.h"
#if defined (MI_BLE_ENABLED) && (!HAVE_MSC)
#include "standard_auth/mible_standard_auth.h"
#elif defined (MI_BLE_ENABLED) && (HAVE_MSC)
#include "secure_auth/mible_secure_auth.h"
#elif defined(MI_MESH_ENABLED)
#include "mesh_auth/mible_mesh_auth.h"
#endif

#define MI_LOG_MODULE_NAME "mi srv"
#include "mible_log.h"
#include "mi_service_server.h"
#if MI_API_ENABLE
#if (USE_GATT_SPEC==1)
#include "ble_spec/mible_spec.h"
#endif

/**@brief Xiaomi Service structure.
 *
 * @details This structure contains status information related to the service.
 */
typedef struct {
    uint8_t                  uuid_type;               /**< UUID type for Xiaomi Service Base UUID. */
    uint16_t                 conn_handle;             /**< Handle of the current connection (as provided by the BLE Stack). BLE_CONN_HANDLE_INVALID if not in a connection. */
    uint16_t                 service_handle;          /**< Handle of Xiaomi Service (as provided by the BLE Stack). */

    uint16_t                 version_handle;          /**< Handle related to the characteristic value (as provided by the BLE Stack). */
    uint16_t                 ctrl_point_handle;       /**< Handle related to the characteristic value (as provided by the BLE Stack). */
    uint16_t                 auth_handle;             /**< Handle related to the characteristic value (as provided by the BLE Stack). */
#if (USE_MIBLE_OTA==1)
    uint16_t                 ota_ctrl_point;
    uint16_t                 ota_data;
#endif
#if (USE_GATT_SPEC==1)
    uint16_t                 spec_read_handle;
    uint16_t                 spec_write_handle;
#endif
#if (USE_MIBLE_WAC==1)
    uint16_t                 wifi_status;
#endif

    mi_service_evt_handler_t handler;
} ble_mi_t;

#define BLE_CONN_HANDLE_INVALID         0xFFFF
#define UNIT_10_MS                      10000
#define UNIT_1_25_MS                    1250

#define MS_TO_UNITS(TIME, RESOLUTION) (((TIME) * 1000UL) / (RESOLUTION))

#define BLE_UUID_MI_SERVICE  0xFE95                    /**< The UUID of the Xiaomi Service. */

#define BLE_UUID_MI_VERS     0x0004                    /**< The UUID of the Mi Service Version Characteristic. */
#define BLE_UUID_MI_WIFI_CFG 0x0005                    /**< The UUID of the WiFi Status Characteristic. */
#define BLE_UUID_MI_CTRLP    0x0010                    /**< The UUID of the Control Point Characteristic. */
#define BLE_UUID_MI_SECURE   0x0016                    /**< The UUID of the Secure Auth Characteristic. */
#define BLE_UUID_MI_OTA_CTRL 0x0017                    /**< The UUID of the OTA Control Point Characteristic. */
#define BLE_UUID_MI_OTA_DATA 0x0018                    /**< The UUID of the OTA Data Characteristic. */
#define BLE_UUID_MI_STANDARD 0x0019                    /**< The UUID of the Standard Auth Characteristic. */
#define BLE_UUID_MI_SPEC_RD  0x001A                    /**< The UUID of the MIOT Spec RX Characteristic. */
#define BLE_UUID_MI_SPEC_WR  0x001B                    /**< The UUID of the MIOT Spec TX Characteristic. */

#define SERVICE_CHAR_CNT   (3 + 2*USE_MIBLE_OTA + 2*USE_GATT_SPEC + 1*USE_MIBLE_WAC)

static ble_mi_t mi_srv;
static const uint8_t version[20] = MIBLE_LIB_AND_DEVELOPER_VERSION;

/**@brief Function for handling the @ref BLE_GAP_EVT_CONNECTED event from the BLE Stack.
 *
 * @param[in] p_mi_s    Xiaomi Service structure.
 * @param[in] p_ble_evt Pointer to the event received from BLE stack.
 */
static void on_connect(mible_gap_evt_param_t * p_ble_evt)
{
    mible_status_t errno = MI_SUCCESS;
    mible_gap_conn_param_t prefer_params = {
        .min_conn_interval = MS_TO_UNITS(15, UNIT_1_25_MS),
        .max_conn_interval = MS_TO_UNITS(30, UNIT_1_25_MS),
        .slave_latency     = 0,
        .conn_sup_timeout  = MS_TO_UNITS(30 * 30, UNIT_10_MS)
    };
    if (p_ble_evt->connect.conn_param.max_conn_interval > prefer_params.max_conn_interval) {
        errno = mible_gap_update_conn_params(p_ble_evt->conn_handle, prefer_params);
        MI_ERR_CHECK(errno);
    }
    mi_srv.conn_handle = p_ble_evt->conn_handle;

    MI_ERR_CHECK(errno);

    MI_LOG_WARNING("Connected Peer MAC:\n");
    MI_LOG_HEXDUMP(p_ble_evt->connect.peer_addr, 6);
    MI_LOG_INFO("default interval min: %d ms, max: %d ms\n",
                p_ble_evt->connect.conn_param.min_conn_interval * 1250 / 1000,
                p_ble_evt->connect.conn_param.max_conn_interval * 1250 / 1000);

    auth_setup(mi_srv.conn_handle, mi_srv.service_handle, mi_srv.auth_handle, get_mibeacon_solicited_bit());

    if (mi_srv.handler != NULL)
        mi_srv.handler(1);

#if USE_MIBLE_OTA==1
    mible_dfu_init_t init = {0};
    init.conn_handle = mi_srv.conn_handle;
    init.ctrl_handle = mi_srv.ota_ctrl_point;
    init.data_handle = mi_srv.ota_data;
    init.product_id  = PRODUCT_ID;
    init.model       = MODEL_NAME;
    extern unsigned int	ota_program_offset;
    init.flash_start = ota_program_offset;
    init.flash_size  = 0x30000;
    mible_dfu_init(&init);
#endif
#if (USE_GATT_SPEC==1)
    spec_data_trans_init(mi_srv.conn_handle, mi_srv.service_handle, mi_srv.spec_read_handle, mi_srv.spec_write_handle);
#endif
}

/**@brief Function for handling the @ref BLE_GAP_EVT_DISCONNECTED event from the BLE Stack.
 *
 * @param[in] p_mi_s    Xiaomi Service structure.
 * @param[in] p_ble_evt Pointer to the event received from BLE stack.
 */
static void on_disconnect(mible_gap_evt_param_t * p_ble_evt)
{
    mi_srv.conn_handle = BLE_CONN_HANDLE_INVALID;

    set_mi_unauthorization();
    mi_session_uninit();
    mi_scheduler_stop();

    MI_LOG_WARNING("Disconnect reason %X.\n", p_ble_evt->disconnect.reason);
    
    if (mi_srv.handler != NULL)
        mi_srv.handler(0);

#if USE_MIBLE_OTA==1
    /* If there is a new firmware, then upgrade */
    uint8_t errno = mible_dfu_upgrade();
    MI_ERR_CHECK(errno);
#endif

#if USE_GATT_SPEC==1
    spec_data_trans_deinit();
#endif

}

/**@brief Function for handling the @ref BLE_GAP_EVT_CONN_PARAM_UPDATE event from the BLE Stack.
 *
 * @param[in] p_mi_s    Xiaomi Service structure.
 * @param[in] p_ble_evt Pointer to the event received from BLE stack.
 */
static void on_conn_params_update(mible_gap_evt_param_t * p_ble_evt)
{
    MI_LOG_INFO("update interval max: %d ms, min: %d ms\n",
                p_ble_evt->update_conn.conn_param.min_conn_interval * 1250 / 1000,
                p_ble_evt->update_conn.conn_param.max_conn_interval * 1250 / 1000);
}

/**@brief Function for handling the @ref BLE_GATTS_EVT_WRITE event from the BLE Stack.
 *
 * @param[in] p_mi_s    Xiaomi Service structure.
 * @param[in] p_ble_evt Pointer to the event received from BLE stack.
 */
_attribute_ram_code_ static void on_write(mible_gatts_evt_param_t * p_ble_evt)
{
    uint16_t handle = p_ble_evt->write.value_handle;
    uint8_t     len = p_ble_evt->write.len;
    uint8_t  *pdata = p_ble_evt->write.data;

#if DEBUG_MIBLE
    MI_LOG_DEBUG("write handle %d %d bytes: \n", handle, len);
    MI_LOG_HEXDUMP(pdata, len);
#endif

    if (handle == mi_srv.ctrl_point_handle)
    {
        auth_opcode_process(pdata, len);
    }
    else if (handle == mi_srv.auth_handle)
    {
        auth_data_process(pdata, len);
    }
#if (USE_GATT_SPEC==1)
    else if (handle == mi_srv.spec_read_handle)
    {
        if (get_mi_authorization() == ADMIN_AUTHORIZATION) 
        {
            spec_recv_data_process(pdata, len);
        }
    }
    else if (handle == mi_srv.spec_write_handle)
    {
        if (get_mi_authorization() == ADMIN_AUTHORIZATION) 
        {
            spec_write_data_process(pdata, len);
        }
    }
#endif
    else
    {
        // Do Nothing. This event is not relevant for this service.
    }
}

#if USE_MIBLE_OTA==1
/**@brief Function for handling the @ref BLE_GATTS_EVT_WRITE event from the BLE Stack.
 *
 * @param[in] p_mi_s    Xiaomi Service structure.
 * @param[in] p_ble_evt Pointer to the event received from BLE stack.
 */
static void on_write_permit(mible_gatts_evt_param_t * p_ble_evt)
{
    uint16_t handle = p_ble_evt->write.value_handle;
    uint8_t     len = p_ble_evt->write.len;
    uint8_t  *pdata = p_ble_evt->write.data;

#if DEBUG_MIBLE
    MI_LOG_DEBUG("write permit handle %d: \n", handle);
    MI_LOG_HEXDUMP(pdata, len);
#endif

    if (handle == mi_srv.ota_ctrl_point)
    {
        if (get_mi_authorization() == ADMIN_AUTHORIZATION) {
            mible_gatts_rw_auth_reply(mi_srv.conn_handle, 1, handle, 0, pdata, len, 2);
            mible_dfu_ctrl(pdata, len);
        } else {
            mible_gatts_rw_auth_reply(mi_srv.conn_handle, 0, handle, 0, NULL, 0, 2);
        }
    }
    else if (handle == mi_srv.ota_data)
    {
        if (get_mi_authorization() == ADMIN_AUTHORIZATION) {
            mible_dfu_data(pdata, len);
        }
    }
    else
    {
        // Do Nothing. This event is not relevant for this service.
    }
}
#endif

static void service_init_succ(mible_arch_gatts_srv_init_cmp_t *param)
{
    mible_gatts_srv_db_t *p_srv_db = param->p_gatts_db->p_srv_db;
    mible_gatts_char_db_t *p_char_db = p_srv_db->p_char_db;
    
    MI_LOG_INFO("mi service init succ.\n");

    if (param->status == MI_SUCCESS) {
        mi_srv.service_handle     = p_srv_db->srv_handle;
        mi_srv.version_handle     = (*p_char_db++).char_value_handle;
        MI_LOG_INFO(" version : %d\n", mi_srv.version_handle);
#if (USE_MIBLE_WAC==1)
        mi_srv.wifi_status        = (*p_char_db++).char_value_handle;
        MI_LOG_INFO(" wifi status : %d\n", mi_srv.wifi_status);
#endif
        mi_srv.ctrl_point_handle  = (*p_char_db++).char_value_handle;
        mi_srv.auth_handle        = (*p_char_db++).char_value_handle;
        MI_LOG_INFO(" ctrl point : %d\n auth : %d\n", mi_srv.ctrl_point_handle, mi_srv.auth_handle);
#if (USE_MIBLE_OTA==1)
        mi_srv.ota_ctrl_point     = (*p_char_db++).char_value_handle;
        mi_srv.ota_data           = (*p_char_db++).char_value_handle;
        MI_LOG_INFO(" ota ctrl point : %d\n ota data : %d\n", mi_srv.ota_ctrl_point, mi_srv.ota_data);
#endif
#if (USE_GATT_SPEC==1)
        mi_srv.spec_read_handle   = (*p_char_db++).char_value_handle;
        mi_srv.spec_write_handle  = (*p_char_db++).char_value_handle;
        MI_LOG_INFO(" spec rd : %d\n spec wr : %d\n", mi_srv.spec_read_handle, mi_srv.spec_write_handle);
#endif

        mible_gatts_value_set(mi_srv.service_handle, mi_srv.version_handle, 0, (void*)version, sizeof(version));
    }
    
    //free(p_srv_db->p_char_db);
    //free(p_srv_db);
    
    // TODO: support more than one service databases
}

static void gap_event_handler(mible_gap_evt_t evt,
    mible_gap_evt_param_t* param)
{
    
    switch (evt) {
    case MIBLE_GAP_EVT_CONNECTED:
        on_connect(param);
        break;
    case MIBLE_GAP_EVT_DISCONNECT:
        on_disconnect(param);
        break;
    case MIBLE_GAP_EVT_CONN_PARAM_UPDATED:
        on_conn_params_update(param);
        break;
    case MIBLE_GAP_EVT_ADV_REPORT:
        break;
    }
}

_attribute_ram_code_ static void gatts_event_handler(mible_gatts_evt_t evt,
    mible_gatts_evt_param_t* param)
{
    switch (evt) {
    case MIBLE_GATTS_EVT_WRITE:
        on_write(param);
        break;
    case MIBLE_GATTS_EVT_READ_PERMIT_REQ: break;
    case MIBLE_GATTS_EVT_WRITE_PERMIT_REQ:
#if USE_MIBLE_OTA==1
        on_write_permit(param);
#endif
        break;
    case MIBLE_GATTS_EVT_IND_CONFIRM: break;
    }
}

static void arch_event_handler(mible_arch_event_t evt, 
        mible_arch_evt_param_t* param)
{
    switch (evt) {
    case MIBLE_ARCH_EVT_GATTS_SRV_INIT_CMP:
        if (param->srv_init_cmp.p_gatts_db->p_srv_db->srv_uuid.uuid16 == BLE_UUID_MI_SERVICE)
            service_init_succ(&param->srv_init_cmp);
        break;
    case MIBLE_ARCH_EVT_RECORD_WRITE:
        MI_LOG_INFO("mi record write succ.\n");
        break;
    case MIBLE_ARCH_EVT_RECORD_DELETE:
        MI_LOG_INFO("mi record delete succ.\n");
        break;
    }
}

mible_gatts_srv_db_t mi_gatts_srv_db;
mible_gatts_char_db_t mi_gatt_char_tbl[SERVICE_CHAR_CNT];
uint32_t mi_service_init()
{
    uint32_t      err_code;

    mi_srv.conn_handle             = BLE_CONN_HANDLE_INVALID;

    err_code = mible_gap_register(gap_event_handler);
    MI_ERR_CHECK(err_code);

    err_code = mible_gatts_register(gatts_event_handler);
    MI_ERR_CHECK(err_code);

    err_code = mible_arch_register(arch_event_handler);
    MI_ERR_CHECK(err_code);

    MI_LOG_DEBUG("mijia libs\t : %d.%d.%d\n", MIBLE_LIB_MAJOR, MIBLE_LIB_MINOR, MIBLE_LIB_REVISION);
    MI_LOG_DEBUG("auth mode \t : %d\n", MIBLE_AUTH_MODE);
    MI_LOG_DEBUG("have msc  \t : %d\n", HAVE_MSC);
    MI_LOG_DEBUG("product ID\t : %d\n", PRODUCT_ID);

    // Initialize the service structure.
    MI_LOG_DEBUG("mi service init ... srv %d, char %d\n", sizeof(mible_gatts_srv_db_t), sizeof(mible_gatts_char_db_t) );
    mible_gatts_db_t mi_gatts_db;
    //mible_gatts_srv_db_t *p_mi_service = (mible_gatts_srv_db_t*) malloc( sizeof(mible_gatts_srv_db_t) ); 
    //mible_gatts_char_db_t *p_mi_char = (mible_gatts_char_db_t *) malloc( MI_SECURE_CHAR_NUM * sizeof(mible_gatts_char_db_t) );
	mible_gatts_srv_db_t *p_mi_service = (mible_gatts_srv_db_t*) (&mi_gatts_srv_db );
	mible_gatts_char_db_t *p_mi_char = (mible_gatts_char_db_t *) (mi_gatt_char_tbl );
    mi_gatts_db = (mible_gatts_db_t){
        .p_srv_db = p_mi_service,
        .srv_num = 1,
    };

    // add mi service
    *p_mi_service = (mible_gatts_srv_db_t){
        .srv_type = MIBLE_PRIMARY_SERVICE,
        //.srv_uuid = (mible_uuid_t){.type = 0, .uuid16 = BLE_UUID_MI_SERVICE, },
        .char_num = SERVICE_CHAR_CNT,
        .p_char_db = p_mi_char,
    };
	p_mi_service[0].srv_uuid.type =0;
	p_mi_service[0].srv_uuid.uuid16 = BLE_UUID_MI_SERVICE;
    
    // add char version
    *p_mi_char = (mible_gatts_char_db_t){
       // .char_uuid = (mible_uuid_t){.type = 0, .uuid16 = BLE_UUID_MI_VERS},
        .char_property = MIBLE_READ,
        .char_value_len = sizeof(version),
    };
    p_mi_char->char_uuid.type =0;
    p_mi_char->char_uuid.uuid16 = BLE_UUID_MI_VERS;
	p_mi_char++;
#if (USE_MIBLE_WAC==1)
    // add char wifi status
    *p_mi_char++ = (mible_gatts_char_db_t){
        //.char_uuid = (mible_uuid_t){.type = 0, .uuid16 = BLE_UUID_MI_WIFI_CFG},
        .char_property = MIBLE_READ | MIBLE_NOTIFY,
        .char_value_len = 4,
    };
	p_mi_char->char_uuid.type =0;
    p_mi_char->char_uuid.uuid16 = BLE_UUID_MI_WIFI_CFG;
#endif
    // add char control point
    *p_mi_char = (mible_gatts_char_db_t){
        //.char_uuid = (mible_uuid_t){.type = 0, .uuid16 = BLE_UUID_MI_CTRLP},
        .char_property = MIBLE_WRITE_WITHOUT_RESP | MIBLE_NOTIFY,
        .char_value_len = 4,
    };
	p_mi_char->char_uuid.type = 0;
	p_mi_char->char_uuid.uuid16 = BLE_UUID_MI_CTRLP;
	p_mi_char++;
    // add char authentication
    *p_mi_char = (mible_gatts_char_db_t){
	/*
#if (defined(MI_BLE_ENABLED) && (HAVE_MSC)) || defined(MI_MESH_ENABLED)
        .char_uuid = (mible_uuid_t){.type = 0, .uuid16 =  BLE_UUID_MI_SECURE},
#else
        .char_uuid = (mible_uuid_t){.type = 0, .uuid16 =  BLE_UUID_MI_STANDARD},
#endif
*/
        .char_property = MIBLE_WRITE_WITHOUT_RESP | MIBLE_NOTIFY,
        .char_value_len = MAX_ATT_PAYLOAD,
        .is_variable_len = true,
    };
#if (defined(MI_BLE_ENABLED) && (HAVE_MSC)) || defined(MI_MESH_ENABLED)
        p_mi_char->char_uuid.type = 0;
		p_mi_char->char_uuid.uuid16 = BLE_UUID_MI_SECURE;
#else
        p_mi_char->char_uuid.type = 0;
		p_mi_char->char_uuid.uuid16 = BLE_UUID_MI_STANDARD;
#endif
p_mi_char++;

#if (USE_MIBLE_OTA==1)
    // add char ota control point
    *p_mi_char = (mible_gatts_char_db_t){
        //.char_uuid = (mible_uuid_t){.type = 0, .uuid16 = BLE_UUID_MI_OTA_CTRL},
        .char_property = MIBLE_WRITE | MIBLE_NOTIFY,
        .char_value_len = 20,
        .is_variable_len = true,
        .wr_author = 1
    };
	p_mi_char->char_uuid.type = 0;
	p_mi_char->char_uuid.uuid16 = BLE_UUID_MI_OTA_CTRL;
	p_mi_char++;

    // add char ota data
    *p_mi_char = (mible_gatts_char_db_t){
       // .char_uuid = (mible_uuid_t){.type = 0, .uuid16 = BLE_UUID_MI_OTA_DATA},
        .char_property = MIBLE_WRITE_WITHOUT_RESP | MIBLE_NOTIFY,
        .char_value_len = MAX_ATT_PAYLOAD,
        .is_variable_len = true,
        .wr_author = 1
    };
	p_mi_char->char_uuid.type = 0;
	p_mi_char->char_uuid.uuid16 = BLE_UUID_MI_OTA_DATA;
	p_mi_char++;
#endif

#if (USE_GATT_SPEC==1)
    // add char SPEC
    *p_mi_char = (mible_gatts_char_db_t){
        //.char_uuid = (mible_uuid_t){.type = 0, .uuid16 = BLE_UUID_MI_SPEC_RD},
        .char_property = MIBLE_WRITE_WITHOUT_RESP | MIBLE_NOTIFY,
        .char_value_len = MAX_ATT_PAYLOAD,
        .is_variable_len = true,
    };
	p_mi_char->char_uuid.type = 0;
	p_mi_char->char_uuid.uuid16 = BLE_UUID_MI_SPEC_RD;
	p_mi_char++;
    *p_mi_char = (mible_gatts_char_db_t){
       // .char_uuid = (mible_uuid_t){.type = 0, .uuid16 = BLE_UUID_MI_SPEC_WR},
        .char_property = MIBLE_WRITE_WITHOUT_RESP | MIBLE_NOTIFY,
        .char_value_len = MAX_ATT_PAYLOAD,
        .is_variable_len = true,
    };
	p_mi_char->char_uuid.type = 0;
	p_mi_char->char_uuid.uuid16 = BLE_UUID_MI_SPEC_WR;
	p_mi_char++;
#endif		
    err_code = mible_gatts_service_init(&mi_gatts_db);
    MI_ERR_CHECK(err_code);

    return err_code;
}

void mi_service_event_register(mi_service_evt_handler_t h)
{
    mi_srv.handler = h;
}

uint32_t opcode_send(uint32_t status)
{
    uint32_t errno;

    if (mi_srv.conn_handle == BLE_CONN_HANDLE_INVALID)
    {
        return MIBLE_ERR_INVALID_CONN_HANDLE;
    }

    errno = mible_gatts_notify_or_indicate(mi_srv.conn_handle, mi_srv.service_handle, mi_srv.ctrl_point_handle,
                                   0, (void*) &status, sizeof(status), 1);
    
    if (errno != MI_SUCCESS) {
        MI_LOG_INFO("Cann't send auth : %X\n", errno);
    }

    return errno;
}

uint32_t opcode_recv(void)
{
    uint32_t opcode = 0;
    uint8_t  len;
    mible_gatts_value_get(mi_srv.service_handle, mi_srv.ctrl_point_handle, (uint8_t*)&opcode, &len);
    return opcode;
}

#if (USE_MIBLE_WAC==1)
int set_wifi_status(uint8_t status)
{
    int errno;

    if (mi_srv.wifi_status == 0)
        return MIBLE_ERR_ATT_INVALID_ATT_HANDLE;

    errno = mible_gatts_value_set(mi_srv.service_handle, mi_srv.wifi_status, 0, (void*) &status, sizeof(status));

    if (errno != MI_SUCCESS) {
        MI_LOG_INFO("Cann't set wifi status : %X\n", errno);
    }
    return errno;
}

int report_wifi_status(uint8_t status)
{
    int errno;
    if (mi_srv.conn_handle == BLE_CONN_HANDLE_INVALID)
        return MIBLE_ERR_INVALID_CONN_HANDLE;

    if (mi_srv.wifi_status == 0)
        return MIBLE_ERR_ATT_INVALID_ATT_HANDLE;

    errno = set_wifi_status(status);
    MI_ERR_CHECK(errno);

    errno = mible_gatts_notify_or_indicate(mi_srv.conn_handle, mi_srv.service_handle, mi_srv.wifi_status,
                                   0, (void*) &status, sizeof(status), 1);

    if (errno != MI_SUCCESS) {
        MI_LOG_INFO("Cann't send wifi status : %X\n", errno);
    }
    return errno;
}
#endif
#endif
