/********************************************************************************************************
 * @file	gw_node_info.h
 *
 * @brief	for TLSR chips
 *
 * @author	telink
 * @date	Sep. 30, 2010
 *
 * @par     Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/
#ifndef __GW_NODE_INFO_H_
#define __GW_NODE_INFO_H_
#include "vendor/common/mesh_node.h"

#define ELE_LIGHT_MODEL_SIZE  (380-12)	

typedef struct{
	u8 nums;
	u8 numv;
	u8 id[ELE_LIGHT_MODEL_SIZE];
}mesh_element_model_id;

typedef struct{
    u16 len_cps;
    #if 1   // must same with page0_local_t from start to numv
    mesh_page0_head_t page0_head;
    u16 local;
    mesh_element_model_id model_id;
    #endif
}VC_node_cps_t;

#if DONGLE_PROVISION_EN&&!WIN32
typedef struct{
    u16 node_adr;    // primary address
    u8 element_cnt;
    u8 rsv;
    u8 dev_key[16];
	#if (MD_REMOTE_PROV && DEVICE_KEY_REFRESH_ENABLE)
	u8 dev_key_candi[16];
	#endif
}VC_node_info_t;

typedef struct{
	u16 node_adr;
	u8 ele_cnt;
	u16 index;
}gw_node_info_t;

#define SINGLE_NODE_INFO_SIZE	sizeof(VC_node_info_t)
extern _align_4_ gw_node_info_t gw_node_info[];
#else
typedef struct{
    u16 node_adr;    // primary address
    u8 element_cnt;
    u8 rsv;
    u8 dev_key[16];
    VC_node_cps_t cps;
	#if WIN32
	u8 dev_key_candi[16];
	#endif
}VC_node_info_t;    // size is 404(0x194)
#endif



extern _align_4_ VC_node_info_t VC_node_info[];
extern VC_node_cps_t VC_node_cps;

void clear_gw_node_info(u16 addr, int is_must_primary);



// mesh CDTP (both for gateway firmware and VC tool)-------------------------
#define CDTP_READ_LEN_ONCE_MAX				(64 - 8) // for 56 byte and 20ms interval; about 1.8kbyte/s. // sizeof(cdtp_gw_ots_send_data_t) <= COC_MTU_SIZE

typedef struct{
	u8 hci_type; // hci_type_t
	u8 hci_cmd;
	u8 oacp_opcode;
	u16	len;
	u8 data[1];
}cdtp_gw_ots_rx_rsp_t;

typedef struct{
	u16 ini_type;
	u8 hci_cmd;
	u8 oacp_opcode;
	u16	len;
	u8 data[CDTP_READ_LEN_ONCE_MAX];
}cdtp_gw_ots_send_data_t;

typedef struct{
	u16 ini_type;
	u8 hci_cmd;
	u8 oacp_opcode;
	u16	len;
	u32 checksum;
}cdtp_gw_ots_send_checksum_t;

typedef struct{
	u32 offset;
	u32 len;
	u32 checksum; // user define
}cdtp_ots_get_checksum_t;

typedef struct{
	u32	current_size;
	u32	max_size;		// Allocated Size
}OtsObjectSizeValue_t;

typedef struct{
	u16 ini_type;
	u8 hci_cmd;
	u8 oacp_opcode;
	u16	len;
	OtsObjectSizeValue_t size;
}cdtp_gw_ots_send_object_size_t; // object is compressed.

typedef struct{
	u16 ini_type;
	u8 hci_cmd;
	u8 oacp_opcode;
	u16	len;
	u8 onoff;
}cdtp_gw_ots_gatt_adv_onoff_t; // object is compressed.

typedef struct{
	u32	offset;
	u32	len;
}OtsRWOpcodePar_t;





#endif

