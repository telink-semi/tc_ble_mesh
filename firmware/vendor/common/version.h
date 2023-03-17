/********************************************************************************************************
 * @file	version.h
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
#pragma once

// can't use "enum" here, because cstartup.S not support

//#define   BUILD_VERSION  	"Revision: 45:49M"
//#define   BUILD_TIME  		"2012-07-24-21:37:43"

#include "../../proj/mcu/config.h"
#include "mesh_config.h"

#define SW_VERSION_SPEC			(3)		// "3" means SIG MESH 1.0.x
#define SW_VERSION_MAJOR		(3)		// 
#define SW_VERSION_MINOR		(3)		// 
#define SW_VERSION_2ND_MINOR	(5)		// second minor

// big endian
#define FW_VERSION_TELINK_RELEASE   ((SW_VERSION_SPEC << 4) + (SW_VERSION_MAJOR << 0) + \
									 (SW_VERSION_MINOR << 12) + (SW_VERSION_2ND_MINOR << 8))    // user can't modify, big endian.

#define VER_NUM2CHAR(num)		((((num) >= 0)&&((num) <= 9)) ? ((num) + '0') : ((((num) >= 0x0a)&&((num) <= 0x0f)) ? ((num)-0x0a + 'a') : (num)))
#define U8_HIGH2CHAR(v_u8)		(VER_NUM2CHAR(((v_u8) >> 4) & 0x0f))
#define U8_LOW2CHAR(v_u8)		(VER_NUM2CHAR((v_u8) & 0x0f))
#define U8_2CHAR(v_u8)			((U8_HIGH2CHAR((v_u8)) << 8) + U8_LOW2CHAR((v_u8)))

#define PID_UNKNOW              (0x0000)
// ------ light ------
// LIGHT_TYPE_SEL
// ------ gateway ------
#define PID_GATEWAY             (0x0101)
// ------ LPN ------
#define PID_LPN                 (0x0201)
// ------ SWITCH ------
#define PID_SWITCH              (0x0301)
// ------ SPIRIT_LPN ------
#define PID_SPIRIT_LPN          (0x0401)

// ------ HOME KIT ------
// from 0xC000 -- 0xFFFF


/*
MESH_PID_SEL : PID is product ID,
MESH_VID: VID is software version ID,

PID and VID are used in composition data: model_sig_cfg_s_cps.
format: please refer to spec "4.2.1.1 Composition Data Page 0"
*/

#if (WIN32)
#define MESH_PID_SEL		(LIGHT_TYPE_SEL)
#define MESH_VID		    FW_VERSION_TELINK_RELEASE       // user can redefine
#elif (__PROJECT_MESH_PRO__ || __PROJECT_MESH_GW_NODE__)  // must define in TC32_CC_Assember ->General , too. because cstartup.s can't read predefine value in TC32_compiler-->symbols
    #if ((MCU_CORE_TYPE == MCU_CORE_8258) || (MCU_CORE_TYPE == MCU_CORE_8278))
#define MESH_PID_SEL		(PID_GATEWAY)
#define MESH_VID		    FW_VERSION_TELINK_RELEASE       // user can redefine
    #else // if (CHIP_TYPE == CHIP_TYPE_8269)
#define MESH_PID_SEL		(PID_GATEWAY)
#define MESH_VID		    FW_VERSION_TELINK_RELEASE       // user can redefine
    #endif
#elif (__PROJECT_MESH_LPN__)  // must define in TC32_CC_Assember ->General , too. because cstartup.s can't read predefine value in TC32_compiler-->symbols
    #if ((MCU_CORE_TYPE == MCU_CORE_8258) || (MCU_CORE_TYPE == MCU_CORE_8278))
#define MESH_PID_SEL		(PID_LPN)
#define MESH_VID		    FW_VERSION_TELINK_RELEASE       // user can redefine
    #else // if (CHIP_TYPE == CHIP_TYPE_8269)
#define MESH_PID_SEL		(PID_LPN)
#define MESH_VID		    FW_VERSION_TELINK_RELEASE       // user can redefine
    #endif
#elif (__PROJECT_MESH_SWITCH__)  // must define in TC32_CC_Assember ->General , too. because cstartup.s can't read predefine value in TC32_compiler-->symbols
#define MESH_PID_SEL		(PID_SWITCH)
#define MESH_VID		    FW_VERSION_TELINK_RELEASE       // user can redefine
#elif (__PROJECT_SPIRIT_LPN__)  // must define in TC32_CC_Assember ->General , too. because cstartup.s can't read predefine value in TC32_compiler-->symbols
#define MESH_PID_SEL		(PID_SPIRIT_LPN)
#define MESH_VID		    FW_VERSION_TELINK_RELEASE       // user can redefine
#elif (__PROJECT_8267_MASTER_KMA_DONGLE__)
#define MESH_PID_SEL		(PID_UNKNOW)
#define MESH_VID		    FW_VERSION_TELINK_RELEASE       // user can redefine
#elif (__PROJECT_MESH__)   // light
#define MESH_PID_SEL		(LIGHT_TYPE_SEL)
	#if DU_ENABLE
#define MESH_VID		    DU_FW_VER       // in the du mode ,we will use to set version .
	#else
#define MESH_VID		    FW_VERSION_TELINK_RELEASE       // user can redefine
	#endif
#elif (__PROJECT_MESH_GW_NODE_HK__)   // light
#define MESH_PID_SEL		(LIGHT_TYPE_SEL)
#define MESH_VID		    FW_VERSION_TELINK_RELEASE       // user can redefine
#elif (__PROJECT_BOOTLOADER__)   // light
#define MESH_PID_SEL		(LIGHT_TYPE_SEL)
#define MESH_VID		    FW_VERSION_TELINK_RELEASE       // user can redefine
#else
//#error: must define PID, VID, no default value.
#endif

	#if DU_ENABLE
#define BUILD_VERSION		DU_PID	// if value change, must make clean. same sequence with cps
	#else
#define BUILD_VERSION		(MESH_PID_SEL|(MESH_VID << 16))	// if value change, must make clean. same sequence with cps
	#endif

// -- 
#define RUN_128K_IN_20000_EN 			0 // be enable to run 128K in 0x20000(default is 124k), disable to save RAM code.

#if __PROJECT_BOOTLOADER__
#define MCU_RUN_SRAM_WITH_CACHE_EN      1   // must with cache, because read flash by pointer 
#else
#define MCU_RUN_SRAM_WITH_CACHE_EN      0   // 
#endif
#define MCU_RUN_SRAM_EN                 0   // manual set data address in boot.link.can't read flash by pointer 

#if (MI_SWITCH_LPN_EN || __PROJECT_MESH_LPN__ || __PROJECT_SPIRIT_LPN__ || __PROJECT_MESH_SWITCH__)
#define MCU_STACK_INIT_EN               (0) // because stack may be in retention RAM HW, and also to decrease time cost of wake up.
#else
#define MCU_STACK_INIT_EN               (1)
#endif

// must set ram size according to the chip type
#ifndef RAM_SIZE_MAX
#define RAM_SIZE_MAX            		(64*1024)
#endif
