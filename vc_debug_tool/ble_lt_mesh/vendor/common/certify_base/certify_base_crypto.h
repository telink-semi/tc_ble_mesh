/********************************************************************************************************
 * @file	certify_base_crypto.h
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
#ifndef __CERTIFY_BASE_CRYPTO__
#define __CERTIFY_BASE_CRYPTO__

#include <stdint.h>
#include "../user_config.h"
#define URI_ENABLE 0

#ifndef NULL
#define NULL 	0
#endif
#define URI_CERTIFY_ID	0x0000
#define DEV_CERTIFY_ID	0x0001


#define MAX_PEM_CERT_LEN 0x300
extern u8 der_cert_buf[MAX_PEM_CERT_LEN];
extern u32 der_buf_len;

		// suppose the max record id is 16
#define MAX_PROV_RECORD_CNT 	16
/*********************************************
offset (0): 	save fomate type(1 byte)
offset (0x10): 	device_cert(max size is 0x6e0, the first 4bytes is len)
offset (0x700): inter_midate_cert(max size is 0x700, the first 4 bytes is len)
offset (0xe00): cert_pub_key (64 bytes)
offset (0xe40): cert_private_key (32 bytes)
offset (0xe60): cert_uuid (16 bytes)
offset (0xe70): cert_static_oob (32 bytes)
offset (0xf00): crc16. (initial value is 0xffff)
offset (0xf04): valid flag.
**********************************************/
#define CERT_DEVICE_CERT_TYPE_OFFSET	0
#define CERT_DEVICE_CERT_OFFSET			0x10
#define CERT_INTER_CERT_OFFSET			0x700
#define CERT_PUBKEY_OFFSET				0xe00
#define CERT_PRIVATE_OFFSET				0xe40
#define CERT_UUID_OFFSET				0xe60
#define CERT_STATIC_OOB_OFFSET			0xe70
#define CERT_CRC_OFFSET					0xf00
#define CERT_VALID_OFFSET				0xf04

#define CERT_VALID_FLAG					0xa5a55a5a

#define CERT_IS_VALID					1
#define CERT_IS_UNVALID_PROV			2
#define CERT_IS_UNVALID_UNPROV			3


typedef enum{
	RECORD_PROV_SUC =0,
	RECORD_REQ_IS_NOT_PRESENT,
	RECORD_REQ_OFF_OUTBOUND,
	RECORD_RFU,
}RECORD_REQ_STS;

int  cert_base_func_init();
void cert_set_uuid(u8 *p_uuid);
void cert_base_set_key(u8 *pk,u8 *sk);
void get_cert_id_list(u8 *p_list,u32 *p_len);
const char * get_cert_content_by_id(u16 id,u32* p_len);
void cert_id_get(u16 *p_id,u32 *p_cnt);

void prov_clear_all_rec();
void prov_set_rec_id(u16 *p_rec,u8 len);
int prov_set_buf_len(u16 rec_id,u16 offset,u8 *p_buf,u32 len,u32 total);
int prov_use_rec_id_get_pubkey(u16 rec_id,u8 *p_pubkey);
int cert_item_rsp(u16 id,u16 offset,u16 max_size,u8 *p_buf,u16 *p_len);
u32  use_cert_id_get_len(u8 id);

void record_mag_init();
void record_mag_set(u16 rec_id,u16 max_size,u16 offset);
void record_mag_get(u16 *p_rec_id,u16 *p_max_size,u16 *p_offset);
void record_mag_get_max_size(u16 *p_max_size);
void cert_base_oob_set();


#endif
