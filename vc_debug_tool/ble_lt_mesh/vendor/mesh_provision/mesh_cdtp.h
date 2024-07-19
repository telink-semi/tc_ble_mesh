/********************************************************************************************************
 * @file	mesh_cdtp.h
 *
 * @brief	This is the header file for BLE SDK
 *
 * @author	BLE GROUP
 * @date	2020.06
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

#pragma  once

#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"
#include "app_l2cap_coc.h"

#if 1
#include "proj_lib/cdtp/audio_cap.h"
#include "proj_lib/cdtp/audio_inner.h"
#include "proj_lib/cdtp/audio_common.h"
#include "proj_lib/cdtp/audio_config.h"
#include "proj_lib/cdtp/audio_define.h"
#include "proj_lib/cdtp/audio_handle.h"
#include "proj_lib/cdtp/audio_otp.h"
#include "proj_lib/cdtp/audio_sdp.h"
#include "proj_lib/cdtp/audio_struct.h"
#elif 1
//#include "stack/ble/profile/audio/audio_define.h"
//#include "stack/ble/profile/audio/audio_struct.h"
//#include "stack/ble/profile/audio/audio_inner.h"
#include "stack/ble/profile/audio/cap/audio_handle.h"
//#include "stack/ble/profile/audio/audio_common.h"
#include "stack/ble/profile/audio/cap/audio_sdp.h"
#include "stack/ble/profile/audio/otp/audio_otp.h"
#else
#include "stack/profile/audio/audio_cap.h"
#include "stack/profile/audio/audio_common.h"
#include "stack/profile/audio/audio_config.h"
#include "stack/profile/audio/audio_define.h"
#include "stack/profile/audio/audio_handle.h"
#include "stack/profile/audio/audio_otp.h"
#include "stack/profile/audio/audio_sdp.h"
#include "stack/profile/audio/audio_struct.h"
#endif
//#include "app_att.h"

#if 0
// OTS  Service & Characteristic -- ok
#define SERVICE_UUID_OBJECT_TRANSFER                         0x1825 //Object Transfer Service
#define CHARACTERISTIC_UUID_OTS_FEATURE                      0x2ABD //M Mandatory:Read; Optional:
#define CHARACTERISTIC_UUID_OTS_OBJECT_NAME                  0x2ABE //M Mandatory:Read; Optional:Write
#define CHARACTERISTIC_UUID_OTS_OBJECT_TYPE                  0x2ABF //M Mandatory:Read; Optional:
#define CHARACTERISTIC_UUID_OTS_OBJECT_SIZE                  0x2AC0 //M Mandatory:Read; Optional:
#define CHARACTERISTIC_UUID_OTS_OBJECT_FIRST_CREATED         0x2AC1 //O Mandatory:Read; Optional:Write
#define CHARACTERISTIC_UUID_OTS_OBJECT_LAST_CREATED          0x2AC2 //O Mandatory:Read; Optional:Write
#define CHARACTERISTIC_UUID_OTS_OBJECT_ID                    0x2AC3 //O Mandatory:Read; Optional:
#define CHARACTERISTIC_UUID_OTS_OBJECT_PROPERTIES            0x2AC4 //M Mandatory:Read; Optional:Write
#define CHARACTERISTIC_UUID_OTS_OBJECT_ACTION_CONTROL_POINT  0x2AC5 //M Mandatory:Write,Indicate; Optional:
#define CHARACTERISTIC_UUID_OTS_OBJECT_LIST_CONTROL_POINT    0x2AC6 //O Mandatory:Write,Indicate; Optional:
#define CHARACTERISTIC_UUID_OTS_OBJECT_LIST_FILTER           0x2AC7 //O Mandatory:Read,Write; Optional:
#define CHARACTERISTIC_UUID_OTS_OBJECT_CHANGED               0x2AC8 //O Mandatory:Indicate; Optional:

typedef enum{
	OACP_CreateOpCodeSupported					= BIT(0),
	OACP_DeleteOpCodeSupported					= BIT(1),
	OACP_CalculateChecksumOpCodeSupported		= BIT(2),
	OACP_ExecuteOpCodeSupported					= BIT(3),
	OACP_ReadOpCodeSupported					= BIT(4),
	OACP_WriteOpCodeSupported					= BIT(5),
	AppendingAdditionalDatatoObjectsSupported	= BIT(6),
	TruncationofObjectsSupported				= BIT(7),
	PatchingofObjectsSupported					= BIT(8),
	OACP_AbortOpCodeSupported					= BIT(9),
}ots_oacp_features_field_e; // refer to op code of BLC_AUDIO_OTP_OACP_OPCODE_ENUM.

typedef enum{
	OLCP_GoToOpCode								= BIT(0),
	OLCP_OrderOpCodeSupported					= BIT(1),
	OLCP_RequestNumberofObjectsOpCodeSupported	= BIT(2),
	OLCP_ClearMarkingOpCodeSupported			= BIT(3),
}otp_olcp_features_field_e; // refer to op code of BLC_AUDIO_OTP_OLCP_OPCODE_ENUM.
#endif

typedef struct{
	u32	read_len; 		// read object size
	u32	write_len; 		// write object size
	u32	tick;
	u32 last_pos;
	u32 cal_crc;
	u8 crc_ready;		// get crc from VS
}cdtp_read_handle_t;

typedef struct{
	u32 tick;
	u16 timeout_ms;
	u16 handle;
	u8 size_ready;		// get size from VS ready
}cdtp_read_object_size_proc_t;


extern OtsObjectSizeValue_t my_OtsObjectSizeValue;

extern u16 app_aclConnHandle;
extern u16 app_cisConnHandle;
extern u16 gAppsAclConnHandle;
extern u8  gAppsCocIsReady;
extern u32 gAppsCocSendTimer;
extern u16 gAppsCocSendScid;

u8 u32to_int_buf(u32 val, u8 *out, u32 len_max);
void mesh_cdtp_loop(void);
void mesh_cdtp_init(void);
void app_audio_acl_connect(u16 aclHandle, u8 *pPkt, bool isReconn);
void app_audio_acl_disconn(u16 aclHandle, u8 *pPkt);
void app_coc_ble_connect_cb(u8 *p);
void app_coc_ble_disconnect_cb(u8 *p);
int app_l2cap_coc_datacb(u16 aclHandle, u16 scid, u16 dcid, u16 dataLen, u8 *pData);

void app_audio_gatt_init(void);
//int app_att_audioOtsRead(u16 connHandle, void *p);
//int app_att_audioOtsWrite(u16 connHandle, void *p);
int gateway_common_cmd_rsp_ll(u8 *p_par, u16 len, u8 *head, u8 head_len);
int gateway_ots_rx_rsp_cmd(u8 *p_par, u16 len, u8 ots_oacp_opcode);
void CDTP_update_object_size(u32 current_size, u32 max_size);

