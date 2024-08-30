/********************************************************************************************************
 * @file	user_ali.h
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
#ifndef __USER_ALI_H
#define __USER_ALI_H
#include "tl_common.h"
#if __TLSR_RISCV_EN__
#include "stack/ble/ble.h"
#else
#include "proj_lib/ble/ll/ll.h"
#include "proj_lib/ble/service/ble_ll_ota.h"
#endif
#include "proj_lib/ble/blt_config.h"
#include "vendor/common/user_config.h"

#if !WIN32
extern const char num2char[17];
#endif
extern u32 con_product_id;// little endianness 
#if ((MESH_USER_DEFINE_MODE == MESH_SPIRIT_ENABLE) || (MESH_USER_DEFINE_MODE == MESH_CLOUD_ENABLE))
extern const u8 con_mac_address[6];//small endianness
#else
extern u8  con_mac_address[6];//small endianness
#endif
extern u8 con_sec_data[16];
extern void *ctx;
#if !__TLSR_RISCV_EN__
extern _attribute_data_retention_ int ota_adr_index;
#endif
extern _attribute_data_retention_	ota_startCb_t		otaStartCb;

typedef struct{
	//u8 rev;
	u16 cid;
	union{
		u8 pid;
		struct{
		u8	adv_ver:4;
		u8 	auth_en:1;
		u8  ota_en:1;
		u8  ble_ver:2;
		};
	};
	u8  product_id[4];
	u8 	mac[6];
	union{
		u8 feature_flag;
		struct{		
			u8 adv_flag:1;
			u8 uuid_ver:7;
		};
	};
	u8 	rfu[2];
}sha256_dev_uuid_str;

// ali IOT service ota
typedef struct{
	u8 auth_ok;
	u8 ble_key[16];
}ais_gatt_auth_t;

typedef struct{
	u8 device_type;
	u32 fw_version;
	u32 fw_size;
	u16 fw_crc16;
	u8 ota_flag;
}ais_ota_req_t;

typedef struct{
	u8 allow_ota;
	u32 trans_size_last;
	u8 one_round_pkts;
}ais_ota_rsp_t;

typedef struct{
	u8 seg_index;
	u32 trans_size_last;
}ais_ota_receive_t;

#define AIS_MAX_DATA_SIZE	32// please makesure have enough for aes128 padding
typedef struct{
	union{
		u8 header;
		struct{
			u8 msg_id:4;
			u8 enc_flag:1;
			u8 fw_ver:3;
		};
	};
	u8 msg_type;
	union{
		u16 frame_desc;
		struct{
			u8 frame_seq:4;
			u8 frame_total:4;
			u8 length;
		};
	};
	union{
		u8 data[AIS_MAX_DATA_SIZE];  
		u8 device_type;
		u8 ota_result;
		ais_ota_req_t ais_ota_req;
		ais_ota_rsp_t ais_ota_rsp;
		ais_ota_receive_t ais_ota_rcv;
	};
}ais_msg_t;

typedef struct{
	u8 device_type;
	u32 fw_version;
}ais_fw_info_t;

#define AIS_INDICATE_HANDLE	0x32
#define AIS_NOTIFY_HANDLE	0x36

#define AIS_AUTH_RANDOM			0x10
#define AIS_AES_CIPHER			0x11
#define AIS_AUTH_CHECK			0x12
#define AIS_AUTH_RESULT			0x13
#define AIS_FW_VERSION_GET		0x20
#define AIS_FW_VERSION_RSP		0x21
#define	AIS_OTA_REQ				0x22
#define	AIS_OTA_RSP				0x23
#define	AIS_OTA_RECEIVED		0x24
#define	AIS_OTA_END				0x25
#define	AIS_OTA_RESULT			0x26
#define	AIS_OTA_DATA			0x2f

extern ais_gatt_auth_t ais_gatt_auth;
extern const u8 iv[];

#if BLE_MULTIPLE_CONNECTION_ENABLE
int ais_otaWrite(u16 conn_handle, void * p);
int ais_write_pipe(u16 conn_handle, void *p);
#else
int ais_otaWrite(void * p);
int ais_write_pipe(void *p);
#endif

void set_sha256_init_para_mode(u8 mode);
void set_dev_uuid_for_sha256();
void create_sha256_input_string(char *p_input,u8 *pid,u8 *p_mac,u8 *p_secret);
void create_sha256_input_string_node(char *p_input,u8 *pid,u8 *p_mac,u8 *p_secret);
void calculate_sha256_to_create_static_oob();
void calculate_sha256_to_create_pro_oob(u8 *pro_auth,u8 *random);
void calculate_sha256_node_auth_value(u8 *auth_value);
void ais_gatt_auth_init();
void calculate_sha256_node_oob(u8 *p_oob ,u8 *p_random);
void mesh_ais_global_var_set();
void calculate_auth_value();

#endif

