/********************************************************************************************************
 * @file	hw_fun.c
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
#include "app_config.h"
#include <string.h>
#if VC_APP_ENABLE
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#endif
#include "hw_fun.h"
void * memset4(void * dest, int val, unsigned int len);

unsigned int clock_time_offset=0;
#if VC_APP_ENABLE
unsigned int  clock_time_vc_hw()
{
	LARGE_INTEGER litmp; 
	LONGLONG qt1,dff;
	QueryPerformanceFrequency(&litmp);//get PC clock frequence
	dff = litmp.QuadPart;
	QueryPerformanceCounter(&litmp);  
	qt1=litmp.QuadPart; 
	qt1 = (qt1*32*1000000)/dff;
	return (unsigned int)qt1;
}

#if 0
// reserve for future to get real cpu tick. QueryPerformanceFrequency() is just 2M now.
unsigned long long GetCycleCount()
{
    __asm _emit 0x0F
    __asm _emit 0x31
}
#endif
#endif
void  master_clock_init()
{
	clock_time_offset = clock_time_vc_hw();
}
unsigned int  clock_time()
{
	return (clock_time_vc_hw() - clock_time_offset);
}

unsigned int  clock_time_exceed(unsigned int ref, unsigned int span_us)
{
	return ((unsigned int)(clock_time() - ref) > span_us * 32);
}

void sleep_us (u32 us)
{
	u32 t = clock_time();
	while(!clock_time_exceed(t, us)){
	}
}

void sleep_ms(u32 ms)
{
	sleep_us(ms*1000);
}
// flash interface about the operation 
#if VC_APP_ENABLE

u8 src_buf[MAX_CONFIG_SIZE];

void flash_write_page(u32 addr, u32 len, u8 *buf) // sync with flash driver which can not use "const"  for buffer.
{
	errno_t err_fp;
	FILE *fp;
	err_fp=fopen_s(&fp,FILE_PROV_PARA_SAVE,"rb");
	if(err_fp != 0){
		return ;
	}
	
	fseek(fp,0,SEEK_SET );
	fread(src_buf,MAX_CONFIG_SIZE,1,fp);
	memcpy(src_buf+addr,buf,len);
	fclose(fp);
	
	err_fp=fopen_s(&fp,FILE_PROV_PARA_SAVE,"wb");
	if(0 == err_fp){
		fseek(fp,0,SEEK_SET );
		fwrite(src_buf,MAX_CONFIG_SIZE,1,fp);
		fclose(fp);
	}else{
		static u32 a_1;a_1++;	// for test break point
	}
}

void flash_read_page(u32 addr, u32 len, u8 *buf)
{
	 errno_t err_fp;
	 FILE *fp;
	 err_fp=fopen_s(&fp,FILE_PROV_PARA_SAVE,"rb");
	 if(err_fp != 0){
		 return ;
	 }
	 fseek(fp,addr,SEEK_SET );
	 fread(buf,len,1,fp);
	 fclose(fp);
}
#endif

void flash_erase_sector_VC(u32 addr, u32 size)
{
	u8 write_buf[MAX_SECTOR_SIZE];
	addr = addr & 0xfffff000;
	size = ((size + (MAX_SECTOR_SIZE - 1)) >> 12) << 12;
	if(size > MAX_SECTOR_SIZE){
	    size = MAX_SECTOR_SIZE;
	}
	memset4(write_buf, 0xffffffff, size);
	flash_write_page(addr, size, write_buf);
}

void flash_erase_sector(u32 addr)
{
    flash_erase_sector_VC(addr, MAX_SECTOR_SIZE);
}

void flash_erase_512K()
{
	for(int i=0;i<MAX_CONFIG_SIZE/MAX_SECTOR_SIZE;i++){
		flash_erase_sector_VC(i*MAX_SECTOR_SIZE, MAX_SECTOR_SIZE);
	}
}
