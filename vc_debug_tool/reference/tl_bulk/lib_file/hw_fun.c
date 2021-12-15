/********************************************************************************************************
 * @file     hw_fun.c 
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
	QueryPerformanceFrequency(&litmp);//获得时钟频率  
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
