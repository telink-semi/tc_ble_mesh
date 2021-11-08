/*
 * mible_mesh_vendor.h
 *
 *  Created on: 2020Äê12ÔÂ10ÈÕ
 *      Author: mi
 */

#ifndef MIJIA_BLE_LIBS_MESH_AUTH_MIBLE_MESH_VENDOR_H_
#define MIJIA_BLE_LIBS_MESH_AUTH_MIBLE_MESH_VENDOR_H_

#define NODE_CONFIG_SCAN_PARAM      0
#define NODE_CONFIG_RELAY_PARAM     1

#if defined ( __CC_ARM )
__PACKED typedef struct {
    uint8_t siid;
    uint8_t piid;
}vendor_mesh_get_t;

__PACKED typedef struct {
    uint8_t siid;
    uint8_t piid;
    uint8_t payload[4];
    uint8_t tid;
    uint8_t type;
}vendor_mesh_state_t;

__PACKED typedef struct {
    uint8_t siid;
    uint8_t piid;
    uint8_t tid;
    uint8_t retry_times;
}vendor_mesh_sync_t;

__PACKED typedef struct {
    uint8_t siid;
    uint8_t piid;
    uint8_t tid;
    uint8_t type;
    uint8_t payload[4];
}vendor_mesh_sync_ack_t;

__PACKED typedef struct {
    uint8_t siid;
    uint8_t eiid;
    uint8_t tid;
    uint8_t piid;
    uint8_t payload[4];
}vendor_mesh_event_t;

__PACKED typedef struct {
    uint8_t siid;
    uint8_t eiid;
    uint8_t tid;
    uint8_t arg[];
}vendor_mesh_event_tlv_t;

__PACKED typedef struct {
    uint8_t siid;
    uint8_t aiid;
    uint8_t tid;
    uint8_t piid;
    uint8_t payload[4];
}vendor_mesh_action_t;

__PACKED typedef union {
    vendor_mesh_get_t       get;
    vendor_mesh_state_t     state;
    vendor_mesh_sync_t      sync;
    vendor_mesh_sync_ack_t  sync_ack;
    vendor_mesh_event_t     event;
    vendor_mesh_event_tlv_t event_tlv;
    vendor_mesh_action_t    action;
}mible_mesh_vendor_message_t;

__PACKED typedef struct {
    uint16_t primary_grp_addr;
} simple_subscribe_rsp_t;

__PACKED typedef struct {
    uint8_t tid;
} discover_node_req_t;

__PACKED typedef struct {
    uint8_t reserved[1];
} discover_node_rsp_t;

__PACKED typedef struct {
    uint16_t scan_intval;
    uint16_t scan_window;
} node_config_scan_t;

__PACKED typedef struct {
    uint8_t enable;
    uint8_t cnt;
    uint8_t step;
} node_config_relay_t;

__PACKED typedef struct {
    uint8_t subtype;
    __PACKED union{
        node_config_scan_t scan;
        node_config_relay_t relay;
        uint8_t param[6];
    };
} node_config_t;

__PACKED typedef struct {
    uint8_t tid;
    uint8_t retry;
} net_param_req_t;

__PACKED typedef struct {
    uint8_t tid;
    uint8_t pub_interval;
}net_param_rsp_t;

__PACKED typedef struct {
    uint8_t type;
    __PACKED union {
        simple_subscribe_rsp_t   sub_rsp;
        discover_node_req_t discover_req;
        discover_node_rsp_t discover_rsp;
        node_config_t       node_config;
        net_param_req_t     net_para_req;
        net_param_rsp_t     net_para_rsp;
    } value;
}vendor_mesh_config_t;
#elif defined   ( __GNUC__ )
typedef struct __PACKED{
    uint8_t siid;
    uint8_t piid;
}vendor_mesh_get_t;

typedef struct __PACKED{
    uint8_t siid;
    uint8_t piid;
    uint8_t payload[4];
    uint8_t tid;
    uint8_t type;
}vendor_mesh_state_t;

typedef struct __PACKED{
    uint8_t siid;
    uint8_t piid;
    uint8_t tid;
    uint8_t retry_times;
}vendor_mesh_sync_t;

typedef struct __PACKED{
    uint8_t siid;
    uint8_t piid;
    uint8_t tid;
    uint8_t type;
    uint8_t payload[4];
}vendor_mesh_sync_ack_t;

typedef struct __PACKED{
    uint8_t siid;
    uint8_t eiid;
    uint8_t tid;
    uint8_t piid;
    uint8_t payload[4];
}vendor_mesh_event_t;

typedef struct __PACKED{
    uint8_t siid;
    uint8_t eiid;
    uint8_t tid;
    uint8_t arg[];
}vendor_mesh_event_tlv_t;

typedef struct __PACKED{
    uint8_t siid;
    uint8_t aiid;
    uint8_t tid;
    uint8_t piid;
    uint8_t payload[4];
}vendor_mesh_action_t;

typedef union __PACKED{
    vendor_mesh_get_t       get;
    vendor_mesh_state_t     state;
    vendor_mesh_sync_t      sync;
    vendor_mesh_sync_ack_t  sync_ack;
    vendor_mesh_event_t     event;
    vendor_mesh_event_tlv_t event_tlv;
    vendor_mesh_action_t    action;
}mible_mesh_vendor_message_t;

typedef struct __PACKED{
    uint16_t primary_grp_addr;
} simple_subscribe_rsp_t;

typedef struct __PACKED{
    uint8_t tid;
} discover_node_req_t;

typedef struct __PACKED{
    uint8_t reserved[1];
} discover_node_rsp_t;

typedef struct __PACKED{
    uint16_t scan_intval;
    uint16_t scan_window;
} node_config_scan_t;

typedef struct __PACKED{
    uint8_t enable;
    uint8_t cnt;
    uint8_t step;
} node_config_relay_t;

typedef struct __PACKED{
    uint8_t subtype;
    union __PACKED{
        node_config_scan_t scan;
        node_config_relay_t relay;
        uint8_t param[6];
    };
} node_config_t;

typedef struct __PACKED{
    uint8_t tid;
    uint8_t retry;
} net_param_req_t;

typedef struct __PACKED{
    uint8_t tid;
    uint8_t pub_interval;
}net_param_rsp_t;

typedef struct __PACKED{
    uint8_t type;
    union __PACKED{
        simple_subscribe_rsp_t   sub_rsp;
        discover_node_req_t discover_req;
        discover_node_rsp_t discover_rsp;
        node_config_t       node_config;
        net_param_req_t     net_para_req;
        net_param_rsp_t     net_para_rsp;
    } value;
}vendor_mesh_config_t;
#endif

#endif /* MIJIA_BLE_LIBS_MESH_AUTH_MIBLE_MESH_VENDOR_H_ */
