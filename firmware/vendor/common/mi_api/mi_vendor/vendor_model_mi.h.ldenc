/********************************************************************************************************
 * @file     vendor_model.h 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/

#pragma once

#include "../../../../proj/tl_common.h"
#include "../../mesh_node.h"

#if (MI_API_ENABLE)
// op cmd 11xxxxxx yyyyyyyy yyyyyyyy (vendor)
// ---------------------------------from 0xF0 to 0xFF
#define VD_RC_KEY_REPORT				0xF0
#if DEBUG_VENDOR_CMD_EN
#define VD_LIGHT_ONOFF_GET				0xF1
#define VD_LIGHT_ONOFF_SET				0xF2
#define VD_LIGHT_ONOFF_SET_NOACK		0xF3
#define VD_LIGHT_ONOFF_STATUS		    0xF4
    #if MD_VENDOR_2ND_EN
#define VD_LIGHT_ONOFF_GET2			    0xF5
#define VD_LIGHT_ONOFF_SET2			    0xF6
#define VD_LIGHT_ONOFF_SET_NOACK2		0xF7
#define VD_LIGHT_ONOFF_STATUS2		    0xF8
    #endif
#endif

// MI VENDOR OPCODE 
#define VD_MI_GET_PROPERTY			0xc1
#define VD_MI_SET_PROPERTY			0xc3
#define VD_MI_SET_PROPERTY_NO_ACK	0xc4
#define VD_MI_PROPERTY_CHANGED		0xc5
#define VD_MI_ACTION				0xc6
#define VD_MI_RELAY_ACTION 			0xc7
#define VD_MI_EVENT_REPORT			0xc8

typedef struct{
	u8 ssid;
	u8 piid;
}vd_mi_head_str;

typedef struct{
	vd_mi_head_str mi_head;
	u8 buf[6];
}vd_mi_get_property_str;


typedef struct{
	vd_mi_head_str mi_head;
	u8 value[4];
	u8 buf[2];
}vd_mi_set_property_str;

typedef struct{
	vd_mi_head_str mi_head;
	u8 value[4];
	u8 buf[2];
}vd_mi_set_noack_property_str;

typedef struct{
	vd_mi_head_str mi_head;
	u8 value[4];
	u8 buf[2];
}vd_mi_property_changed_str;


typedef struct{
	vd_mi_head_str mi_head;
	u8 value1;
	u8 value2;
	u8 value3;
	u8 value4;
	u8 value5;
	u8 value6;
}vd_mi_action_str;

typedef struct{
	vd_mi_head_str mi_head;
	u8 code;
	u8 value[5];
}vd_mi_reply_action_str;

typedef struct{
	vd_mi_head_str mi_head;
	u8 value1;
	u8 value2;
	u8 value3;
	u8 value4;
	u8 value5;
	u8 value6;
}vd_mi_event_report_str;

#define MIOT_SEPC_VENDOR_MODEL_SER	VENDOR_MD_LIGHT_S
#define MIOT_SEPC_VENDOR_MODEL_CLI	VENDOR_MD_LIGHT_C 
#define MIOT_VENDOR_MD_SER			VENDOR_MD_LIGHT_S2	

//------0xE0 ~ 0xFF for customer


//------op parameters
typedef struct{
	u8 code;
	u8 rsv[7];
}vd_rc_key_report_t;

typedef struct{
	u8 onoff;
	u8 tid;
}vd_light_onoff_set_t;

typedef struct{
	u8 present_onoff;
}vd_light_onoff_st_t;

// ------------------


//------------------vendor op end-------------------

int vd_cmd_key_report(u16 adr_dst, u8 key_code);
int vd_cmd_onoff(u16 adr_dst, u8 rsp_max, u8 onoff, int ack);
int vd_light_onoff_st_publish(u8 idx);
int vd_light_onoff_st_publish2(u8 idx);

void APP_set_vd_id_mesh_cmd_vd_func(u16 vd_id);

int mesh_search_model_id_by_op_vendor(mesh_op_resource_t *op_res, u16 op, u8 tx_flag);
int is_cmd_with_tid_vendor(u8 *tid_pos_out, u16 op, u8 tid_pos_vendor_app);
int cb_app_vendor_all_cmd(mesh_cmd_ac_vd_t *ac, int ac_len, mesh_cb_fun_par_vendor_t *cb_par);
#endif

