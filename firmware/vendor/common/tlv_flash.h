/********************************************************************************************************
 * @file	tlv_flash.h
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
#ifndef __TLV_FLASH_H_
#define __TLV_FLASH_H_
#include "proj/tl_common.h"
#include "vendor/mesh/app.h"

#define TLV_FLASH_VERSION 		1
#define TLV_STORE_VALID_FLAG	0xa55a
#define TLV_REC_MAX_BUF_CNT		(16)
#define TLV_EMPTY_ID			0xffffffff

#define TLV_END_VALID_VALUE 	0xa5a5a5a5
#define TLV_END_MIDDLE_VALUE	0xafafafaf

typedef struct{
	u32 start;
	u32 end;
	u32 recycle_adr;
}tlv_rec_mag_t;

typedef struct{
	u16 valid;
	u32 id;
	u16 ver;
	u8 en:1;
	u8 rsv:7;
	u8 rfu[3];
	u16 len_check;
	u16 len;		// "len" must be just before "buf".
	u8 buf[TLV_REC_MAX_BUF_CNT];
	u16 crc;
}tlv_str_t;
#define TLV_REC_WHOLE_PACKET_BUF (0x400)
typedef struct{
	u16 valid;
	u32 id;
	u16 ver;
	u8 en:1;
	u8 rsv:7;
	u8 rfu[3];
	u16 len_check;
	u16 len;		// "len" must be just before "buf".
	u8 buf[TLV_REC_WHOLE_PACKET_BUF];
	u16 crc;
}tlv_str_max_t;

#define TLV_HEADER_LEN			(OFFSETOF(tlv_str_t, buf))
#define TLV_CRC_LEN				(SIZEOF_MEMBER(tlv_str_t, crc))
#define TLV_CONTENT_LEN			(TLV_HEADER_LEN+TLV_CRC_LEN)

// reserve the 0x36000~0x3ffff,the fw can extend to 192k+24k =216k
#define TLV_SEC_SIZE					0x1000
#define TLV_FLASH_SECTOR_END_ADR		(TLV_SEC_SIZE-4)
#define TLV_FLASH_START_ADR		(0x36000)
#define TLV_FLASH_MAX_INTER		(0xa000)
#define TLV_FLASH_LAST_SECTOR	(TLV_FLASH_START_ADR+TLV_FLASH_MAX_INTER-TLV_SEC_SIZE)
#define TLV_FLASH_REC_MAX_ADR 	(TLV_FLASH_START_ADR+TLV_FLASH_MAX_INTER-1)

#define TLV_SECTOR_START(a)     ((a/TLV_SEC_SIZE)*TLV_SEC_SIZE)

void tlv_init();
u8 tlv_rec_write(u32 id,u8 *p_buf,u16 len );
u8 tlv_rec_read(u32 id,u8 *p_buf,u16 len);
#endif
