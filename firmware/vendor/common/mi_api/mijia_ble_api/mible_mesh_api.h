/*
 * mible_mesh_api.h
 *
 *  Created on: 2020Äê11ÔÂ26ÈÕ
 *      Author: mi
 */

#ifndef MIJIA_BLE_API_MIBLE_MESH_API_H_
#define MIJIA_BLE_API_MIBLE_MESH_API_H_

#include "mible_type.h"

#define MIBLE_MESH_GATEWAY_GROUP                                    0xFEFF
#define MIBLE_MESH_COMPANY_ID_SIG                                   0xFFFF
#define MIBLE_MESH_COMPANY_ID_XIAOMI                                0x038F

/*SIG Generic model ID */
#define MIBLE_MESH_MODEL_ID_CONFIGURATION_SERVER                    0x0000
#define MIBLE_MESH_MODEL_ID_GENERIC_ONOFF_SERVER                    0x1000
#define MIBLE_MESH_MODEL_ID_LIGHTNESS_SERVER                        0x1300
#define MIBLE_MESH_MODEL_ID_CTL_TEMPEATURE_SERVER                   0x1306
/** Mesh vendor models **/
#define MIBLE_MESH_MIOT_SPEC_SERVER_MODEL                           0000 //0x038f0000
#define MIBLE_MESH_MIOT_SPEC_CLIENT_MODEL                           0001 //0x038f0001
#define MIBLE_MESH_MIJIA_SERVER_MODEL                               0002 //0x038f0002
#define MIBLE_MESH_MIJIA_CLIENT_MODEL                               0003 //0x038f0003

/******** Mesh Opcodes **********/
#define MIBLE_MESH_MSG_CONFIG_BEACON_GET                            0x8009
#define MIBLE_MESH_MSG_CONFIG_BEACON_SET                            0x800A
#define MIBLE_MESH_MSG_CONFIG_BEACON_STATUS                         0x800B
#define MIBLE_MESH_MSG_CONFIG_COMPOSITION_DATA_GET                  0x8008
#define MIBLE_MESH_MSG_CONFIG_COMPOSITION_DATA_STATUS               0x02
#define MIBLE_MESH_MSG_CONFIG_DEFAULT_TTL_GET                       0x800C
#define MIBLE_MESH_MSG_CONFIG_DEFAULT_TTL_SET                       0x800D
#define MIBLE_MESH_MSG_CONFIG_DEFAULT_TTL_STATUS                    0x800E
#define MIBLE_MESH_MSG_CONFIG_GATT_PROXY_GET                        0x8012
#define MIBLE_MESH_MSG_CONFIG_GATT_PROXY_SET                        0x8013
#define MIBLE_MESH_MSG_CONFIG_GATT_PROXY_STATUS                     0x8014
#define MIBLE_MESH_MSG_CONFIG_FRIEND_GET                            0x800F
#define MIBLE_MESH_MSG_CONFIG_FRIEND_SET                            0x8010
#define MIBLE_MESH_MSG_CONFIG_FRIEND_STATUS                         0x8011
#define MIBLE_MESH_MSG_CONFIG_MODEL_PUBLICATION_GET                 0x8018
#define MIBLE_MESH_MSG_CONFIG_MODEL_PUBLICATION_SET                 0x03
#define MIBLE_MESH_MSG_CONFIG_MODEL_PUBLICATION_STATUS              0x8019
#define MIBLE_MESH_MSG_CONFIG_MODEL_PUBLICATION_VIRTUAL_ADDRESS_SET 0x801A
#define MIBLE_MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_ADD                0x801B
#define MIBLE_MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_DELETE             0x801C
#define MIBLE_MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_DELETE_ALL         0x801D
#define MIBLE_MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_OVERWRITE          0x801E
#define MIBLE_MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_STATUS             0x801F
#define MIBLE_MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_VIRTUAL_ADDRESS_ADD 0x8020
#define MIBLE_MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_VIRTUAL_ADDRESS_DELETE  0x8021
#define MIBLE_MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_VIRTUAL_ADDRESS_OVERWRITE   0x8022
#define MIBLE_MESH_MSG_CONFIG_NETWORK_TRANSMIT_GET                  0x8023
#define MIBLE_MESH_MSG_CONFIG_NETWORK_TRANSMIT_SET                  0x8024
#define MIBLE_MESH_MSG_CONFIG_NETWORK_TRANSMIT_STATUS               0x8025
#define MIBLE_MESH_MSG_CONFIG_RELAY_GET                             0x8026
#define MIBLE_MESH_MSG_CONFIG_RELAY_SET                             0x8027
#define MIBLE_MESH_MSG_CONFIG_RELAY_STATUS                          0x8028
#define MIBLE_MESH_MSG_CONFIG_SIG_MODEL_SUBSCRIPTION_GET            0x8029
#define MIBLE_MESH_MSG_CONFIG_SIG_MODEL_SUBSCRIPTION_LIST           0x802A
#define MIBLE_MESH_MSG_CONFIG_VENDOR_MODEL_SUBSCRIPTION_GET         0x802B
#define MIBLE_MESH_MSG_CONFIG_VENDOR_MODEL_SUBSCRIPTION_LIST        0x802C
#define MIBLE_MESH_MSG_CONFIG_LOW_POWER_NODE_POLL_TIMEOUT_GET       0x802D
#define MIBLE_MESH_MSG_CONFIG_LOW_POWER_NODE_POLL_TIMEOUT_STATUS    0x802E
#define MIBLE_MESH_MSG_CONFIG_NETKEY_ADD                            0x8040
#define MIBLE_MESH_MSG_CONFIG_NETKEY_DELETE                         0x8041
#define MIBLE_MESH_MSG_CONFIG_NETKEY_GET                            0x8042
#define MIBLE_MESH_MSG_CONFIG_NETKEY_LIST                           0x8043
#define MIBLE_MESH_MSG_CONFIG_NETKEY_STATUS                         0x8044
#define MIBLE_MESH_MSG_CONFIG_NETKEY_UPDATE                         0x8045
#define MIBLE_MESH_MSG_CONFIG_APPKEY_ADD                            0x00
#define MIBLE_MESH_MSG_CONFIG_APPKEY_UPDATE                         0x01
#define MIBLE_MESH_MSG_CONFIG_APPKEY_DELETE                         0x8000
#define MIBLE_MESH_MSG_CONFIG_APPKEY_GET                            0x8001
#define MIBLE_MESH_MSG_CONFIG_APPKEY_LIST                           0x8002
#define MIBLE_MESH_MSG_CONFIG_APPKEY_STATUS                         0x8003
#define MIBLE_MESH_MSG_CONFIG_MODEL_APP_BIND                        0x803D
#define MIBLE_MESH_MSG_CONFIG_MODEL_APP_STATUS                      0x803E
#define MIBLE_MESH_MSG_CONFIG_MODEL_APP_UNBIND                      0x803F
#define MIBLE_MESH_MSG_CONFIG_SIG_MODEL_APP_GET                     0x804B
#define MIBLE_MESH_MSG_CONFIG_SIG_MODEL_APP_LIST                    0x804C
#define MIBLE_MESH_MSG_CONFIG_VENDOR_MODEL_APP_GET                  0x804D
#define MIBLE_MESH_MSG_CONFIG_VENDOR_MODEL_APP_LIST                 0x804E
#define MIBLE_MESH_MSG_CONFIG_NODE_IDENTITY_GET                     0x8046
#define MIBLE_MESH_MSG_CONFIG_NODE_IDENTITY_SET                     0x8047
#define MIBLE_MESH_MSG_CONFIG_NODE_IDENTITY_STATUS                  0x8048
#define MIBLE_MESH_MSG_CONFIG_NODE_RESET                            0x8049
#define MIBLE_MESH_MSG_CONFIG_NODE_RESET_STATUS                     0x804A
#define MIBLE_MESH_MSG_CONFIG_KEY_REFRESH_PHASE_GET                 0x8015
#define MIBLE_MESH_MSG_CONFIG_KEY_REFRESH_PHASE_SET                 0x8016
#define MIBLE_MESH_MSG_CONFIG_KEY_REFRESH_PHASE_STATUS              0x8017
#define MIBLE_MESH_MSG_CONFIG_HEARTBEAT_PUBLICATION_GET             0x8038
#define MIBLE_MESH_MSG_CONFIG_HEARTBEAT_PUBLICATION_SET             0x8039
#define MIBLE_MESH_MSG_CONFIG_HEARTBEAT_PUBLICATION_STATUS          0x06
#define MIBLE_MESH_MSG_CONFIG_HEARTBEAT_SUBSCRIPTION_GET            0x803A
#define MIBLE_MESH_MSG_CONFIG_HEARTBEAT_SUBSCRIPTION_SET            0x803B
#define MIBLE_MESH_MSG_CONFIG_HEARTBEAT_SUBSCRIPTION_STATUS         0x803C

/*Generic On Off Model Message Definition*/
#define MIBLE_MESH_MSG_GENERIC_ONOFF_GET                            0x8201
#define MIBLE_MESH_MSG_GENERIC_ONOFF_SET                            0x8202
#define MIBLE_MESH_MSG_GENERIC_ONOFF_SET_UNACKNOWLEDGED             0x8203
#define MIBLE_MESH_MSG_GENERIC_ONOFF_STATUS                         0x8204

/*LIGHTNESS Model Message Definition*/
#define MIBLE_MESH_MSG_LIGHT_LIGHTNESS_GET                               0x824B
#define MIBLE_MESH_MSG_LIGHT_LIGHTNESS_SET                               0x824C
#define MIBLE_MESH_MSG_LIGHT_LIGHTNESS_SET_UNACKNOWLEDGED                0x824D
#define MIBLE_MESH_MSG_LIGHT_LIGHTNESS_STATUS                            0x824E
#define MIBLE_MESH_MSG_LIGHT_LIGHTNESS_LINEAR_GET                        0x824F
#define MIBLE_MESH_MSG_LIGHT_LIGHTNESS_LINEAR_SET                        0x8250
#define MIBLE_MESH_MSG_LIGHT_LIGHTNESS_LINEAR_SET_UNACKNOWLEDGED         0x8251
#define MIBLE_MESH_MSG_LIGHT_LIGHTNESS_LINEAR_STATUS                     0x8252
#define MIBLE_MESH_MSG_LIGHT_LIGHTNESS_LAST_GET                          0x8253
#define MIBLE_MESH_MSG_LIGHT_LIGHTNESS_LAST_STATUS                       0x8254
#define MIBLE_MESH_MSG_LIGHT_LIGHTNESS_DEFAULT_GET                       0x8255
#define MIBLE_MESH_MSG_LIGHT_LIGHTNESS_DEFAULT_STATUS                    0x8256
#define MIBLE_MESH_MSG_LIGHT_LIGHTNESS_RANGE_GET                         0x8257
#define MIBLE_MESH_MSG_LIGHT_LIGHTNESS_RANGE_STATUS                      0x8258
#define MIBLE_MESH_MSG_LIGHT_LIGHTNESS_DEFAULT_SET                       0x8259
#define MIBLE_MESH_MSG_LIGHT_LIGHTNESS_DEFAULT_SET_UNACKNOWLEDGED        0x825A
#define MIBLE_MESH_MSG_LIGHT_LIGHTNESS_RANGE_SET                         0x825B
#define MIBLE_MESH_MSG_LIGHT_LIGHTNESS_RANGE_SET_UNACKNOWLEDGED          0x825C

/*CTL Model Message Definition*/
#define MIBLE_MESH_MSG_LIGHT_CTL_GET                                     0x825D
#define MIBLE_MESH_MSG_LIGHT_CTL_SET                                     0x825E
#define MIBLE_MESH_MSG_LIGHT_CTL_SET_UNACKNOWLEDGED                      0x825F
#define MIBLE_MESH_MSG_LIGHT_CTL_STATUS                                  0x8260
#define MIBLE_MESH_MSG_LIGHT_CTL_TEMPERATURE_GET                         0x8261
#define MIBLE_MESH_MSG_LIGHT_CTL_TEMPERATURE_RANGE_GET                   0x8262
#define MIBLE_MESH_MSG_LIGHT_CTL_TEMPERATURE_RANGE_STATUS                0x8263
#define MIBLE_MESH_MSG_LIGHT_CTL_TEMPERATURE_SET                         0x8264
#define MIBLE_MESH_MSG_LIGHT_CTL_TEMPERATURE_SET_UNACKNOWLEDGED          0x8265
#define MIBLE_MESH_MSG_LIGHT_CTL_TEMPERATURE_STATUS                      0x8266
#define MIBLE_MESH_MSG_LIGHT_CTL_DEFAULT_GET                             0x8267
#define MIBLE_MESH_MSG_LIGHT_CTL_DEFAULT_STATUS                          0x8268
#define MIBLE_MESH_MSG_LIGHT_CTL_DEFAULT_SET                             0x8269
#define MIBLE_MESH_MSG_LIGHT_CTL_DEFAULT_SET_UNACKNOWLEDGED              0x826A
#define MIBLE_MESH_MSG_LIGHT_CTL_TEMPERATURE_RANGE_SET                   0x826B
#define MIBLE_MESH_MSG_LIGHT_CTL_TEMPERATURE_RANGE_SET_UNACKNOWLEDGED    0x826C

/*** Mesh vendor opcodes ***/
#define MIBLE_MESH_MIOT_SPEC_GET                                    0x00C1 //0xC1038F
#define MIBLE_MESH_MIOT_SPEC_SET                                    0x00C3 //0xC3038F
#define MIBLE_MESH_MIOT_SPEC_SET_NOACK                              0x00C4 //0xC4038F
#define MIBLE_MESH_MIOT_SPEC_STATUS                                 0x00C5 //0xC5038F
#define MIBLE_MESH_SYNC_PROPS_REQ                                   0x00C6 //0xC6038F
#define MIBLE_MESH_SYNC_PROPS_RSP                                   0x00C7 //0xC7038F
#define MIBLE_MESH_MIOT_SPEC_EVENT                                  0x00C8 //0xC8038F
#define MIBLE_MESH_MIOT_SPEC_EVENT_TLV                              0x00C9 //0xC9038F
#define MIBLE_MESH_MIOT_SPEC_ACTION                                 0x00CA //0xCA038F
#define MIBLE_MESH_MIOT_SPEC_ACTION_ACK                             0x00CB //0xCB038F
#define MIBLE_MESH_MIOT_SPEC_INDICATION                             0x00CE //0xCE038F
#define MIBLE_MESH_MIOT_SPEC_INDICATION_ACK                         0x00CF //0xCF038F
#define MIBLE_MESH_MIOT_SPEC_VENDOR_CONFIG_REQ                      0x00FE
#define MIBLE_MESH_MIOT_SPEC_VENDOR_CONFIG_RSP                      0x00FF

#define MIBLE_MESH_MIOT_SPEC_VENDOR_CONFIG_SUB                      0x01
#define MIBLE_MESH_MIOT_SPEC_VENDOR_CONFIG_PROVISIONER_FOUND        0x02
#define MIBLE_MESH_MIOT_SPEC_VENDOR_CONFIG_NODE_FOUND               0x03
#define MIBLE_MESH_MIOT_SPEC_VENDOR_CONFIG_DEVICE_PARAM             0x05
#define MIBLE_MESH_MIOT_SPEC_VENDOR_CONFIG_DEVICE_VERSION           0x06
#define MIBLE_MESH_MIOT_SPEC_VENDOR_CONFIG_NODE_UPDATE_PARAMS       0x80

#define RECORD_ID_MESH_BASE                             0x20
#define RECORD_ID_MESH_STATE                            (RECORD_ID_MESH_BASE + 1)

#if defined ( __CC_ARM )
/**
 * @brief mible mesh model description.
 */
__PACKED typedef struct {
    uint16_t model_id;
    uint16_t company_id;
} mible_mesh_model_id_t;

/**
 * @brief mible opcode description
 * you can't omit bit flag 0b0, 0b10, 0b11 for opcode.
 * Opcode   Format Notes
 * 0xxxxxxx (excluding 01111111) 1-octet Opcodes
 * 10xxxxxx xxxxxxxx 2-octet Opcodes
 * 11xxxxxx zzzzzzzz 3-octet Opcodes
 */
__PACKED typedef struct {
    uint16_t opcode;        /**< operation code, defined mesh profile 3.7.3.1 Operation codes */
    uint16_t company_id;    /**< SIG ID: 0xFFFF */
} mible_mesh_opcode_t;

/**
 * @brief mible mesh provisioning data description.
 */
__PACKED typedef struct {
    uint8_t  devkey[16];
    uint8_t  netkey[16];
    uint16_t net_idx;
    uint8_t  flags;
    uint32_t iv;
    uint16_t address;
} mible_mesh_provisioning_data_t;

/**
 * @brief mible mesh iv info.
 */
__PACKED typedef struct {
    uint32_t iv_index;  /**< mesh network iv index */
    uint8_t  flags;     /**< IV Update Flag     0: Normal operation 1: IV Update active*/
} mible_mesh_iv_t;

/**
 * @brief mesh message meta data.
 */
__PACKED typedef struct {
    uint16_t src_addr;      /**< [mandatary]  source address */
    uint16_t dst_addr;      /**< [mandatary]  maybe group address, or provisioner addr */
    uint16_t appkey_index;  /**< [mandatary]  appkey index for this message */
    uint16_t netkey_index;  /**< [optional]  if not, default value 0xFFFF */
    //int8_t   rssi;          /**< [optional]  if not, default value -1 */
    uint8_t  ttl;           /**< [optional]  if not, default value 0 */
    uint8_t  elem_index;    /**< [optional]  massage element index */
} mible_mesh_message_meta_t;

/**********************************************************************//**
 * Config Message Status Definitions
 **********************************************************************/

/**
 * @brief Mesh Config Relay Status event paramater type.
 */
__PACKED typedef struct {
    uint8_t relay;
    uint8_t relay_retrans_cnt;
    uint8_t relay_retrans_intvlsteps;
} mible_mesh_conf_relay_set_t;

/**
 * @brief Mesh Config Model Sublication Status event paramater type.
 */
__PACKED typedef struct {
    uint16_t elem_addr;
    uint16_t address;
    mible_mesh_model_id_t model_id;
} mible_mesh_conf_model_sub_set_t;

/**
 * @brief Mesh Model Client event paramater type.
 * you should implement all status corresponsing to mible_mesh_node_*.
 */
__PACKED typedef struct {
    mible_mesh_opcode_t opcode;
    mible_mesh_message_meta_t meta_data;
    __PACKED union{
        mible_mesh_conf_relay_set_t     relay_set;
        mible_mesh_conf_model_sub_set_t model_sub_set;
    };
}mible_mesh_config_status_t;

/**
 * @brief mesh message.
 */
__PACKED typedef struct {
    mible_mesh_opcode_t opcode;
    mible_mesh_message_meta_t meta_data;
    uint16_t buf_len;       /* mesh raw data len*/
    uint8_t  buff[8];
    uint8_t* p_buf;        /* mesh raw data, see mesh profile 4.3 */
} mible_mesh_access_message_t;

__PACKED typedef struct {
    uint8_t siid;
    __PACKED union{
        uint8_t piid;
        uint8_t eiid;
        uint8_t aiid;
    };
} mible_spec_id_t;

__PACKED typedef struct {
    uint8_t  siid;
    uint8_t  piid;
    uint16_t company_id;
    uint16_t model_id;
    uint16_t appkey_idx;
    uint16_t element;
} mible_mesh_template_map_t;

__PACKED typedef struct {
    uint8_t provisioned;
    uint8_t lpn_node;
    uint16_t address;
    uint32_t ivi;
    mible_mesh_template_map_t *map;
    //mible_mesh_template_map_t map[5];
}mible_mesh_node_init_t;
#elif defined   ( __GNUC__ )
/**
 * @brief mible mesh model description.
 */
typedef struct __PACKED{
    uint16_t model_id;
    uint16_t company_id;
} mible_mesh_model_id_t;

/**
 * @brief mible opcode description
 * you can't omit bit flag 0b0, 0b10, 0b11 for opcode.
 * Opcode   Format Notes
 * 0xxxxxxx (excluding 01111111) 1-octet Opcodes
 * 10xxxxxx xxxxxxxx 2-octet Opcodes
 * 11xxxxxx zzzzzzzz 3-octet Opcodes
 */
typedef struct __PACKED{
    uint16_t opcode;        /**< operation code, defined mesh profile 3.7.3.1 Operation codes */
    uint16_t company_id;    /**< SIG ID: 0xFFFF */
} mible_mesh_opcode_t;

/**
 * @brief mible mesh provisioning data description.
 */
typedef struct __PACKED{
    uint8_t  devkey[16];
    uint8_t  netkey[16];
    uint16_t net_idx;
    uint8_t  flags;
    uint32_t iv;
    uint16_t address;
} mible_mesh_provisioning_data_t;

/**
 * @brief mible mesh iv info.
 */
typedef struct __PACKED{
    uint32_t iv_index;  /**< mesh network iv index */
    uint8_t  flags;     /**< IV Update Flag     0: Normal operation 1: IV Update active*/
} mible_mesh_iv_t;

/**
 * @brief mesh message meta data.
 */
typedef struct __PACKED{
    uint16_t src_addr;      /**< [mandatary]  source address */
    uint16_t dst_addr;      /**< [mandatary]  maybe group address, or provisioner addr */
    uint16_t appkey_index;  /**< [mandatary]  appkey index for this message */
    uint16_t netkey_index;  /**< [optional]  if not, default value 0xFFFF */
    int8_t   rssi;          /**< [optional]  if not, default value -1 */
    //uint8_t  ttl;           /**< [optional]  if not, default value 0 */
    uint8_t  elem_index;    /**< [optional]  massage element index */
} mible_mesh_message_meta_t;

/**********************************************************************//**
 * Config Message Status Definitions
 **********************************************************************/

/**
 * @brief Mesh Config Relay Status event paramater type.
 */
typedef struct __PACKED{
    uint8_t relay;
    uint8_t relay_retrans_cnt;
    uint8_t relay_retrans_intvlsteps;
} mible_mesh_conf_relay_set_t;

/**
 * @brief Mesh Config Model Sublication Status event paramater type.
 */
typedef struct __PACKED{
    uint16_t elem_addr;
    uint16_t address;
    mible_mesh_model_id_t model_id;
} mible_mesh_conf_model_sub_set_t;

/**
 * @brief Mesh Model Client event paramater type.
 * you should implement all status corresponsing to mible_mesh_node_*.
 */
typedef struct __PACKED{
    mible_mesh_opcode_t opcode;
    mible_mesh_message_meta_t meta_data;
    union __PACKED{
        mible_mesh_conf_relay_set_t     relay_set;
        mible_mesh_conf_model_sub_set_t model_sub_set;
    };
}mible_mesh_config_status_t;

/**
 * @brief mesh message.
 */
typedef struct __PACKED{
    mible_mesh_opcode_t opcode;
    mible_mesh_message_meta_t meta_data;
    uint16_t buf_len;       /* mesh raw data len*/
    uint8_t  buff[8];
    uint8_t* p_buf;        /* mesh raw data, see mesh profile 4.3 */
} mible_mesh_access_message_t;

typedef struct __PACKED{
    uint8_t siid;
    union __PACKED{
        uint8_t piid;
        uint8_t eiid;
        uint8_t aiid;
    };
} mible_spec_id_t;

typedef struct __PACKED{
    uint8_t  siid;
    uint8_t  piid;
    uint16_t company_id;
    uint16_t model_id;
    uint16_t appkey_idx;
    uint16_t element;
} mible_mesh_template_map_t;

typedef struct __PACKED{
    uint8_t provisioned;
    uint8_t lpn_node;
    uint16_t address;
    uint32_t ivi;
    mible_mesh_template_map_t *map;
    //mible_mesh_template_map_t map[5];
}mible_mesh_node_init_t;
#endif

/**
 * @brief ADD contains add and update operation.
 *        This is only used to mesh provisioner local opertions for mible_mesh_gateway_*.
 *        DO NOT BE USE TO mible_mesh_node_* functions.
 *        Firstly, you should execute get-operation,
 *        and then, according to get result, choose you really add or update method.
 *        if get-operation return null, you can call add method, otherwise, call update method.
 */
typedef enum {
    MIBLE_MESH_OP_ADD=0,
    MIBLE_MESH_OP_DELETE,
}mible_mesh_op_t;

typedef union {
    mible_mesh_node_init_t node_init;
    mible_mesh_iv_t mesh_iv;
    mible_mesh_config_status_t config_msg;
    mible_mesh_access_message_t generic_msg;
} mible_mesh_event_params_t;

/* mible_mesh_event and callback data */
typedef enum {
    MIBLE_MESH_EVENT_STACK_INIT_DONE=0,     /**< NULL */
    MIBLE_MESH_EVENT_DEVICE_INIT_DONE,      /**< Mesh node info */
    MIBLE_MESH_EVENT_ADV_PACKAGE,           /**< Deprecated event */
    MIBLE_MESH_EVENT_IV_UPDATE,             /**< mible_mesh_iv_t */
    MIBLE_MESH_EVENT_CONFIG_MESSAGE_CB,     /**< Mesh Profile definition message */
    MIBLE_MESH_EVENT_GENERIC_MESSAGE_CB,    /**< Mesh Model definition message */
} mible_mesh_event_type_t;

/**
 *@brief    mible mesh event callback, report to applications
 *@param    [in] type : mible_mesh_event_type_t definitions
 *@param    [in] data : mible_mesh_event_params_t pointer.
 *@return   0: success, negetive value: failure
 */
typedef int (*mible_mesh_event_cb_t)(mible_mesh_event_type_t type, mible_mesh_event_params_t *data);

typedef union {
    mible_gap_evt_param_t gap_evt_param;
}user_event_params_t;

/* user_event and callback data */
typedef enum {
    MIBLE_USER_PROV_CB_TYPE_UNPROV=0,
    MIBLE_USER_PROV_CB_TYPE_PROVED,
    MIBLE_USER_PROV_CB_TYPE_START,
    MIBLE_USER_PROV_CB_TYPE_COMPLETE,
    MIBLE_USER_PROV_CB_TYPE_FAIL,
    MIBLE_USER_PROV_CB_TYPE_RESET,
    MIBLE_USER_LOGIN_CB_TYPE_START,
    MIBLE_USER_LOGIN_CB_TYPE_COMPLETE,
    MIBLE_USER_LOGIN_CB_TYPE_FAIL,
    MIBLE_USER_GAP_CB_DISCONNECTED,
    MIBLE_USER_GAP_CB_CONNECTED,
} user_event_type_t;

/**
 *@brief    mible mesh event callback, report to applications
 *@param    [in] type : mible_mesh_event_type_t definitions
 *@param    [in] data : mible_mesh_event_params_t pointer.
 *@return   0: success, negetive value: failure
 */
typedef int (*mible_user_event_cb_t)(uint8_t type, void *data);

/**
 *@brief    sync method, execute event callback
 *@param    [in] type : mible_mesh_event_type_t definitions
 *@param    [in] data : mible_mesh_event_params_t pointer.
 *@return   0: success, negetive value: failure
 */
int mible_mesh_event_callback(mible_mesh_event_type_t type, void * data);

/**
 *@brief    sync method, register event callback
 *@param    [in] mible_mesh_event_cb : event callback
 *@return   0: success, negetive value: failure
 */
int mible_mesh_device_register_event_callback(mible_mesh_event_cb_t mible_mesh_event_cb);

/**
 *@brief    sync method, unregister event callback
 *@param    [in] mible_mesh_event_cb : event callback
 *@return   0: success, negetive value: failure
 */
int mible_mesh_device_unregister_event_callback(mible_mesh_event_cb_t mible_mesh_event_cb);

/**
 *@brief    sync method, execute event callback
 *@param    [in] type : mible_mesh_event_type_t definitions
 *@param    [in] data : mible_mesh_event_params_t pointer.
 *@return   0: success, negetive value: failure
 */
int mible_mesh_user_event_callback(uint8_t type, void * data);

/**
 *@brief    sync method, register event callback
 *@param    [in] mible_mesh_event_cb : event callback
 *@return   0: success, negetive value: failure
 */
int mible_mesh_user_event_register_event_callback(mible_user_event_cb_t user_event_cb);

/**
 *@brief    sync method, unregister event callback
 *@param    [in] mible_mesh_event_cb : event callback
 *@return   0: success, negetive value: failure
 */
int mible_mesh_user_event_unregister_event_callback(mible_user_event_cb_t user_event_cb);

/**
 *@brief    async method, init mesh stack.
 *          report event: MIBLE_MESH_EVENT_STACK_INIT_DONE, data: NULL.
 *@return   0: success, negetive value: failure
 */
int mible_mesh_device_init_stack(void);

/**
 *@brief    deinit mesh stack.
 *          report event: MIBLE_MESH_EVENT_STACK_DEINIT_DONE, data: NULL.
 *@return   0: success, negetive value: failure
 */
int mible_mesh_device_deinit_stack(void);

/**
 *@brief    async method, init mesh device
 *          load self info, include unicast address, iv, seq_num, init model;
 *          clear local db, related appkey_list, netkey_list, device_key_list,
 *          we will load latest data for cloud;
 *          report event: MIBLE_MESH_EVENT_PROVISIONER_INIT_DONE, data: NULL.
 *@param    [in] info : init parameters corresponding to gateway
 *@return   0: success, negetive value: failure
 */
int mible_mesh_device_init_node(void);

/**
 *@brief    set node provsion data.
 *@param    [in] param : prov data include devkey, netkey, netkey idx,
 *          uni addr, iv idx, key flag
 *@return   0: success, negetive value: failure
 */
int mible_mesh_device_set_provsion_data(mible_mesh_provisioning_data_t *param);

/**
 *@brief    mesh provsion done. need update node info and
 *          report event: MIBLE_MESH_EVENT_DEVICE_INIT_DONE
 *@return   0: success, negetive value: failure
 */
int mible_mesh_device_provsion_done(void);

/**
 *@brief    reset node, 4.3.2.53 Config Node Reset, Report 4.3.2.54 Config Node Reset Status.
 *          report event: MIBLE_MESH_EVENT_CONFIG_MESSAGE_CB, data: mible_mesh_config_status_t.
 *@return   0: success, negetive value: failure
 */
int mible_mesh_node_reset(void);

/**
 *@brief    mesh provsion done. need update node info and
 *          report event: MIBLE_MESH_EVENT_DEVICE_INIT_DONE
 *@return   0: success, negetive value: failure
 */
int mible_mesh_device_unprovsion_done(void);

/**
 *@brief    mesh login done.
 *@return   0: success, negetive value: failure
 */
int mible_mesh_device_login_done(uint8_t status);

/**
 *@brief    set local provisioner network transmit params.
 *@param    [in] count : advertise counter for every adv packet, adv transmit times
 *@param    [in] interval_steps : adv interval = interval_steps*0.625ms
 *@return   0: success, negetive value: failure
 */
int mible_mesh_device_set_network_transmit_param(uint8_t count, uint8_t interval_steps);

/**
 *@brief    set node relay onoff.
 *@param    [in] enabled : 0: relay off, 1: relay on
 *@param    [in] count: Number of relay transmissions beyond the initial one. Range: 0-7
 *@param    [in] interval: Relay retransmit interval steps. 10*(1+steps) milliseconds. Range: 0-31.
 *@return   0: success, negetive value: failure
 */
int mible_mesh_device_set_relay(uint8_t enabled,uint8_t count,uint8_t interval);

/**
 *@brief    get node relay state.
 *@param    [out] enabled : 0: relay off, 1: relay on
 *@param    [out] count: Number of relay transmissions beyond the initial one. Range: 0-7
 *@param    [out] interval: Relay retransmit interval steps. 10*(1+steps) milliseconds. Range: 0-31.
 *@return   0: success, negetive value: failure
 */
int mible_mesh_device_get_relay(uint8_t *enabled, uint8_t *count, uint8_t *step);

/**
 *@brief    get seq number.
 *@param    [in] element : model element
 *@param    [out] seq : current sequence numer
 *@param    [out] iv_index : current IV Index
 *@param    [out] flags : IV Update Flag
 *@return   0: success, negetive value: failure
 */
int mible_mesh_device_get_seq(uint16_t element, uint32_t* seq, uint32_t* iv, uint8_t* flags);

/**
 *@brief    update iv index, .
 *@param    [in] iv_index : current IV Index
 *@param    [in] flags : IV Update Flag     0: Normal operation 1: IV Update active
 *@return   0: success, negetive value: failure
 */
int mible_mesh_device_update_iv_info(uint32_t iv_index, uint8_t flags);

/**
 *@brief    add/delete local netkey.
 *@param    [in] op : add or delete
 *@param    [in] netkey_index : key index for netkey
 *@param    [in] netkey : netkey value
 *@param    [in|out] stack_netkey_index : [in] default value: 0xFFFF, [out] stack generates netkey_index
 *          if your stack don't manage netkey_index and stack_netkey_index relationships, update stack_netkey_index.
 *@return   0: success, negetive value: failure
 */
int mible_mesh_device_set_netkey(mible_mesh_op_t op, uint16_t netkey_index, uint8_t *netkey);

/**
 *@brief    add/delete local appkey.
 *@param    [in] op : add or delete
 *@param    [in] netkey_index : key index for netkey
 *@param    [in] appkey_index : key index for appkey
 *@param    [in] appkey : appkey value
 *@param    [in|out] stack_appkey_index : [in] default value: 0xFFFF, [out] stack generates appkey_index
 *          if your stack don't manage appkey_index and stack_appkey_index relationships, update stack_appkey_index.
 *@return   0: success, negetive value: failure
 */
int mible_mesh_device_set_appkey(mible_mesh_op_t op, uint16_t netkey_index, uint16_t appkey_index, uint8_t * appkey);

/**
 *@brief    bind/unbind model app.
 *@param    [in] op : bind is MIBLE_MESH_OP_ADD, unbind is MIBLE_MESH_OP_DELETE
 *@param    [in] element : model element
 *@param    [in] company_id: company id
 *@param    [in] model_id : model_id
 *@param    [in] appkey_index : key index for appkey
 *@return   0: success, negetive value: failure
 */
int mible_mesh_device_set_model_app(mible_mesh_op_t op, uint16_t elem_index, uint16_t company_id,
            uint16_t model_id, uint16_t appkey_index);

/**
 *@brief    add/delete subscription params.
 *@param    [in] op : add or delete
 *@param    [in] element : model element
 *@param    [in] company_id: company id
 *@param    [in] model_id : model_id
 *@param    [in] sub_addr: subscription address params
 *@return   0: success, negetive value: failure
 */
int mible_mesh_device_set_sub_address(mible_mesh_op_t op, uint16_t element, uint16_t company_id,
            uint16_t model_id, uint16_t sub_addr);

/**
 *@brief    generic message, Mesh model 3.2, 4.2, 5.2, 6.3, or 7 Summary.
 *          report event: MIBLE_MESH_EVENT_GENERIC_OPTION_CB, data: mible_mesh_access_message_t.
 *@param    [in] param : control parameters corresponding to node
 *          according to opcode, generate a mesh message; extral params: ack_opcode, tid, get_or_set.
 *@return   0: success, negetive value: failure
 */
int mible_mesh_node_generic_control(mible_mesh_access_message_t *param);

/**
 *@brief    get system time.
 *@return   systicks in ms.
 */
uint64_t mible_mesh_get_exact_systicks(void);

#endif /* MIJIA_BLE_API_MIBLE_MESH_API_H_ */
