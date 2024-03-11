/********************************************************************************************************
 * @file	user_nmw.h
 *
 * @brief	This is the header file for BLE SDK
 *
 * @author	Mesh Group
 * @date	2021
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
#ifndef __USER_NMW_H
#define __USER_NMW_H
#include "tl_common.h"
#include "proj_lib/ble/ll/ll.h"

#define MAX_DEVICE_CACHE_NUM	16
#define NMW_OTA_SEG_WAIT_MS		(240*10)

// module version
#define NMW_MAIN_VERSION			(1)  // 1 byte
#define NMW_SUB_VERSION				(0)  // 1 byte
#define NMW_MODULE_VERSION			((NMW_MAIN_VERSION<<8) | NMW_SUB_VERSION)

#define NMW_MAIN_VERSION_BYTE0		VER_NUM2CHAR(NMW_MAIN_VERSION%10),
#if (NMW_MAIN_VERSION/100)
#define NMW_MAIN_VERSION_BYTE2		VER_NUM2CHAR(NMW_MAIN_VERSION/100),
#define NMW_MAIN_VERSION_BYTE1		VER_NUM2CHAR((NMW_MAIN_VERSION/10)%10),
#elif((NMW_MAIN_VERSION/10))
#define NMW_MAIN_VERSION_BYTE2
#define NMW_MAIN_VERSION_BYTE1		VER_NUM2CHAR((NMW_MAIN_VERSION/10)%10),
#else
#define NMW_MAIN_VERSION_BYTE2
#define NMW_MAIN_VERSION_BYTE1
#endif

#define NMW_SUB_VERSION_BYTE0		VER_NUM2CHAR(NMW_SUB_VERSION%10),
#if (NMW_SUB_VERSION/100)
#define NMW_SUB_VERSION_BYTE2		VER_NUM2CHAR(NMW_SUB_VERSION/100),
#define NMW_SUB_VERSION_BYTE1		VER_NUM2CHAR((NMW_SUB_VERSION/10)%10),
#elif((NMW_SUB_VERSION/10))
#define NMW_SUB_VERSION_BYTE2
#define NMW_SUB_VERSION_BYTE1		VER_NUM2CHAR((NMW_SUB_VERSION/10)%10)
#else
#define NMW_SUB_VERSION_BYTE2
#define NMW_SUB_VERSION_BYTE1
#endif

#define NMW_CACHE_NUM_GET		0x0100
#define NMW_CACHE_RSP			0x0180	

#define NMW_OTA_REQ				0x0200
#define NMW_OTA_RSP				0x0280

#define NMW_DEV_INFO_GET   		0x0400
#define NMW_DEV_INFO_RSP   		0x0480

#define NMW_ERR_REPORT			0x0080	
#define NMW_OTA_ST_REPORT		0x8180
#define NMW_OTA_RESULT_REPORT	0x0380

#define NMW_MODULE_DEV_NAME			{'l','i','g','h','t'}
#define NMW_MODULE_SUB_DEV_NAME		{'L','1'}

// NMW_DEV_INFO_RSP slave-->host
enum{
	TYPE_MODULE_DEV_NAME 		= 0,
	TYPE_MODULE_SUB_DEV_NAME 	= 1,
	TYPE_MODULE_DEV_TYPE 		= 2,
	TYPE_MODULE_DEV_ID 			= 3,
	TYPE_MODULE_FW_VERSION 		= 4,
	TYPE_MCU_DEV_NAME 			= 0x10,
	TYPE_MCU_SUB_DEV_NAME 		= 0x11,
	TYPE_MCU_DEV_TYPE 			= 0x12,
	TYPE_MCU_DEV_ID 			= 0x13,
	TYPE_MCU_FW_VERSION 		= 0x14,
};

// NMW_OTA_REQ host-->slave 
enum{
	TYPE_PID,
	TYPE_FW_VER,
	TYPE_FW_SIZE,
	TYPE_MCU_VER,
	TYPE_MCU_SIZE,
};

enum{
	CMD_UNKNOWN = 1,
	CMD_DEC_ERR		= 2,
	CMD_WRONG_PAR	= 3,
};

enum{
	OTA_SUPPORT,
	OTA_SAME_VER,
	OTA_PID_UNMATCH,
	OTA_SW_VER_ERR,
	OTA_FW_VER_ERR,
	OTA_HW_VER_ERR,
	OTA_SIZE_OVERFLOW,
	OTA_NOT_READY,
};

enum{
	MODULE_HASH_LIST,
	MODULE_SIGNATURE,
	MODULE_FILE,
	MCU_HASH_LIST,
	MCU_SIGNATURE,
	MCU_FILE,
};

enum{
	NMW_OTA_SUCCESS,
	NMW_OTA_WRITE_FLASH_ERR,
	OTA_OTA_SIGNATURE_ERR,
};

typedef struct{
	u16 mcu_version;
	u16 fw_version;
	u8 hw_version;
}nmw_version_t;

#define NMW_RSP_BUF_LEN			64
typedef struct{
	u16 op;
	u8 payload[NMW_RSP_BUF_LEN];
}nmw_rsp_t;

typedef struct{
	u16 op;
	u8 payload[1];
}nmw_ctl_t;

typedef struct{
	u8 type;
	u8 len;
	u8 value[1];
}nmw_tlv_t;

typedef struct{
	u16 seg_index;
	union{
		u8 seg_payload[1];
		struct{
			u16 tlv_size;
			u8 tag;
			u8 len;
			u8 file_type;
			u32 file_size;
			u8 data[1];
		};
	};
}nmw_ota_write_t;

typedef struct{
	u16 start_index;
	u64 seg_map;
}rec_seg_map_t;

typedef struct{
	u8 file_type;
	u8 result;
	struct{
		u16 op;
		u8 errno_pending;
		u8 errno;
	};
	u8 rfu[2];
	u32 image_size;
	u32 image_offset;
	
	s16 ota_index;
	u64 seg_map;	
	u32 seg_timeout_tick;
}nmw_ota_st_t;

extern const u8 NMW_CONTROL_HANDLE;
extern nmw_ota_st_t nmw_ota_state;

int	nmw_ctl_write (void *p);
int	nmw_ota_write (void *p);
int nmw_ota_seg_report();
int nmw_ota_st_report(u16 op, u8 errno);
#endif
