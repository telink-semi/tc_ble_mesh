/********************************************************************************************************
 * @file	log.c
 *
 * @brief	for TLSR chips
 *
 * @author	telink
 * @date	Sep. 30, 2010
 *
 * @par     Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#include "../tl_common.h"
#include "log.h"


#ifndef __LOG_RT_ENABLE__
#define __LOG_RT_ENABLE__		0
#endif


#if (__LOG_RT_ENABLE__)

#ifndef WIN32	
// the printf will mess the realtime log
// the realtime log use usb printer endpoint
//STATIC_ASSERT((!__DEBUG_PRINT__) && (USB_PRINTER_ENABLE));
#else
#include <stdio.h>
#include <io.h>
FILE *hFile = 0;
#endif

/*
	ID == -1 is invalid
	if you want to shut down logging a specified id,  assigne -1 to it
*/
#if LOG_IN_RAM
_attribute_ram_code_
#endif
static void log_write(int id, int type, u32 dat){
	if(-1 == id) return;
	u8 r = irq_disable();
	reg_usb_ep8_dat = (dat & 0xff);
	reg_usb_ep8_dat = ((dat >> 8) & 0xff);
	reg_usb_ep8_dat = ((dat >> 16)& 0xff);
	reg_usb_ep8_dat = (type)|(id);
	irq_restore(r);

#ifdef WIN32		// write to file directly
	if(!hFile){		// file not open yet
		foreach(i, 10){
			char fn[20];
			sprintf(fn, "vcd_%d.log", i);
			if(access(fn, 0)){		// file not exist
				hFile = fopen(fn, "w");
				assert(hFile);
				if(!hFile){
					while(1);
				}
				break;
			}
		}
		if(!hFile){
			hFile = fopen("vcd_0.log", "w");
		}
	}
	if(hFile){
		char temp[4];
		temp[0] = (dat & 0xff);
		temp[1] = ((dat >> 8) & 0xff);
		temp[2] = ((dat >> 16)& 0xff);
		temp[3] = (type)|(id);
		fwrite(&temp[0], 1, 4, hFile);
	}
#endif	
}

#if LOG_IN_RAM
_attribute_ram_code_
#endif
void log_task_begin(int id){
	log_write(id,LOG_MASK_BEGIN,clock_time());
}


#if LOG_IN_RAM
_attribute_ram_code_
#endif
void log_task_end(int id){
	log_write(id,LOG_MASK_END,clock_time());
}


#if LOG_IN_RAM
_attribute_ram_code_
#endif
void log_event(int id){
	log_write(id,LOG_MASK_TGL,clock_time());
}


#if LOG_IN_RAM
_attribute_ram_code_
#endif
void log_data(int id, u32 dat){
	log_write(id,LOG_MASK_DAT,dat);
}

#endif

