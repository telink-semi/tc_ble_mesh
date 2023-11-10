/********************************************************************************************************
 * @file	hw_fun.h
 *
 * @brief	for TLSR chips
 *
 * @author	Mesh Group
 * @date	2017
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
#ifndef __HW_FUN_H
#define __HW_FUN_H
#pragma pack(1)

#include <stdio.h>

enum{
	FILE_OPEN_OK = 0,
	FILE_NOT_EXIST = 2,
	FILE_HAVE_OPENED = 13,
};

#define MAX_CONFIG_SIZE (1024*1024)
#define MAX_SECTOR_SIZE (4*1024)
#define FILE_PROV_PARA_SAVE "provision.bin"

extern unsigned int clock_time_offset;

extern void  master_clock_init();
extern unsigned int  clock_time_vc_hw();
extern unsigned int  clock_time();
extern unsigned int  clock_time_exceed(unsigned int ref, unsigned int span_us);
extern void sleep_us (unsigned int us);
void sleep_ms(unsigned int ms);

extern void flash_write_page(unsigned int addr, unsigned int len, unsigned char *buf);
extern void flash_read_page(unsigned int addr, unsigned int len, unsigned char *buf);
extern void flash_erase_sector(unsigned int addr);
void flash_erase_sector_VC(u32 addr, u32 size);
extern void flash_erase_512K();

u32 vc_file_read_all(u8* data_out, u32 out_len_max, const char* filename);
errno_t vc_file_write_with_new_or_rewrite(u8* data_in, u32 len, const char* filename);
unsigned char ble_moudle_id_is_gateway();
void gateway_VC_factory_reset();
void gateway_VC_factory_reset_and_del_JSON();
#endif 
