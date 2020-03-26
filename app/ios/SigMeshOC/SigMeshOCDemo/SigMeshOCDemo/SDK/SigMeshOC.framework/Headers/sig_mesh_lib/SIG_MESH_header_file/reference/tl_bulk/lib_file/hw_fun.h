/********************************************************************************************************
 * @file     hw_fun.h 
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
#ifndef __HW_FUN_H
#define __HW_FUN_H
#pragma pack(1)

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

extern void flash_write_page(unsigned int addr, unsigned int len, const unsigned char *buf);
extern void flash_read_page(unsigned int addr, unsigned int len, unsigned char *buf);
extern void flash_erase_sector(unsigned int addr);
void flash_erase_sector_VC(u32 addr, u32 size);
extern void flash_erase_512K();
#endif 
