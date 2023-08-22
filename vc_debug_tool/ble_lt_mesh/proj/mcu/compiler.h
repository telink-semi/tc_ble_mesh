/********************************************************************************************************
 * @file	compiler.h
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
#pragma once

#ifdef __GNUC__
#if IOS_APP_ENABLE || ANDROID_APP_ENABLE 
#define inline 
#define _attribute_packed_
#define _attribute_aligned_(s)	__declspec(align(s))
#define _attribute_session_(s)
#define _attribute_ram_code_
#define _attribute_custom_code_
#define _attribute_no_inline_   __declspec(noinline)
#else
#define _inline_ 				inline				//   C99 meaning
#define _attribute_packed_		__attribute__((packed))
#define _attribute_aligned_(s)	__attribute__((aligned(s)))
#define _attribute_session_(s)	__attribute__((section(s)))
#define _attribute_ram_code_  	_attribute_session_(".ram_code") __attribute__((noinline))
#define _attribute_custom_code_  	_attribute_session_(".custom") volatile
#define _attribute_no_inline_   __attribute__((noinline)) 
// #define _inline_ 				extern __attribute__ ((gnu_inline)) inline
#define __WEAK                  __attribute__((weak))   // user can define their own function
#define _align_4_				__attribute__((aligned(4)))

//---- just for driver, not for user
#define _attribute_ram_code_sec_      		__attribute__((section(".ram_code")))
#define _attribute_ram_code_sec_noinline_	__attribute__((section(".ram_code"))) __attribute__((noinline))
//---- end

#include "config.h"

#if ((MCU_CORE_TYPE == MCU_CORE_8258) || (MCU_CORE_TYPE == MCU_CORE_8278))
#define	BLC_PM_DEEP_RETENTION_MODE_EN       1
#else
#define	BLC_PM_DEEP_RETENTION_MODE_EN       0   // must 0
#endif

#if (BLC_PM_DEEP_RETENTION_MODE_EN)
//#define _attribute_data_retention_   __attribute__((section(".retention_data")))
// #define _attribute_bss_retention_    __attribute__((section(".retention_bss")))          // not use now, always take a space in firmware for private section.
#if (!__PROJECT_BOOTLOADER__) // if bootloader use this section, it need to fix AT(no_ret_data) and copy position from flash in cstartup.
#define _attribute_no_retention_data_   __attribute__((section(".no_ret_data")))
#endif
#define _attribute_no_retention_bss_   __attribute__((section(".no_ret_bss")))
#endif
#endif
#else
#define _inline_ 				inline				//   C99 meaning
#define _attribute_packed_
#define _attribute_aligned_(s)	__declspec(align(s))
#define _attribute_session_(s)
#define _attribute_ram_code_
#define _attribute_custom_code_
#define _attribute_no_inline_   __declspec(noinline) 
#define _align_4_				__declspec(align(4))

#endif

#ifndef _attribute_data_retention_
#define _attribute_data_retention_
#endif
#ifndef _attribute_bss_retention_
#define _attribute_bss_retention_       //
#endif
#ifndef _attribute_no_retention_data_
#define _attribute_no_retention_data_
#endif
#ifndef _attribute_no_retention_bss_
#define _attribute_no_retention_bss_    //
#endif
#ifndef _align_4_
#define _align_4_						
#endif

#ifndef __WEAK
#define __WEAK                          //
#endif

#define _align_type_4_					_align_4_	// must make sure all pointers of the struct are 4 bytes aligned when used

#define _USER_CAN_REDEFINE_             __WEAK // user can re-define function in user_app.c

