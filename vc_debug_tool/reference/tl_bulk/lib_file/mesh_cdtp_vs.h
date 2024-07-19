/********************************************************************************************************
 * @file	mesh_cdtp_vs.h
 *
 * @brief	This is the header file for BLE SDK
 *
 * @author	BLE GROUP
 * @date	2020.06
 *
 * @par     Copyright (c) 2020, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#include"../../../ble_lt_mesh/proj/common/types.h"

#define CDTP_OBJECT_SIZE_MAX				(1024*1024)
#define CDTP_OBJECT_UNCOMPRESS_MAX			(10 * CDTP_OBJECT_SIZE_MAX)


typedef struct{
	u32	read_len; 	// read object size
	u32	write_len; 	// write object size
	u32	tick;
	u32 last_pos;
	u32 cal_crc_tx; // tx to phone.
	u32 cal_crc_rx; // crc got from phone when app write JSON to gateway.
	u32 cal_crc_get_flag;
}cdtp_read_write_handle_t;


void CDTP_ots_rx_handle(u8 *rx_buff, int buff_len);
void WriteFile_host_handle(u8 *buff, int n);
void mesh_cdtp_test(void);
void CdtpInputDbFromJSON(const char *filename);
void CdtpOutputDbToJSON(const char *filename);

