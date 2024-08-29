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

#include "../../config.h"
#include "mesh_config.h"

#if __TLSR_RISCV_EN__ // for B91m chips
	#if MESH_IRONMAN_AP2T31F80_EN
#define SW_VERSION_SPEC			(4)		// "3" means SIG MESH 1.0.x, "4" means SIG MESH 1.1.x; 0 mean beta version.
#define SW_VERSION_MAJOR		(1)		// "1" means single connection"2", means multi connection.
#define SW_VERSION_MINOR		(0)		// 
#define SW_VERSION_2ND_MINOR	(0)		// second minor or fix version
	#elif BLE_MULTIPLE_CONNECTION_ENABLE
#define SW_VERSION_SPEC			(4)		// "3" means SIG MESH 1.0.x, "4" means SIG MESH 1.1.x; 0 mean beta version.
#define SW_VERSION_MAJOR		(2)		// "2" means multi connection, "1" means single connection.
#define SW_VERSION_MINOR		(0)		// 
#define SW_VERSION_2ND_MINOR	(0)		// second minor or fix version
	#else
#define SW_VERSION_SPEC			(4)		// "3" means SIG MESH 1.0.x, "4" means SIG MESH 1.1.x; 0 mean beta version.
#define SW_VERSION_MAJOR		(1)		// "1" means single connection"2", means multi connection.
#define SW_VERSION_MINOR		(0)		// 
#define SW_VERSION_2ND_MINOR	(0)		// second minor or fix version
	#endif
#else // for B85m chips
#define SW_VERSION_SPEC			(4)		// "3" means SIG MESH 1.0.x, "4" means SIG MESH 1.1.x; 0 mean beta version.
#define SW_VERSION_MAJOR		(1)		// "1" means single connection"2", means multi connection. 
#define SW_VERSION_MINOR		(0)		// 
#define SW_VERSION_2ND_MINOR	(1)		// second minor or fix version
#endif

// big endian
#define FW_VERSION_TELINK_RELEASE   ((SW_VERSION_SPEC << 4) + (SW_VERSION_MAJOR << 0) + \
									 (SW_VERSION_MINOR << 12) + (SW_VERSION_2ND_MINOR << 8))    // user can't modify, big endian.

#define VER_NUM2CHAR(num)		((((num) >= 0)&&((num) <= 9)) ? ((num) + '0') : ((((num) >= 0x0a)&&((num) <= 0x0f)) ? ((num)-0x0a + 'a') : (num)))
#define U8_HIGH2CHAR(v_u8)		(VER_NUM2CHAR(((v_u8) >> 4) & 0x0f))
#define U8_LOW2CHAR(v_u8)		(VER_NUM2CHAR((v_u8) & 0x0f))
#define U8_2CHAR(v_u8)			((U8_HIGH2CHAR((v_u8)) << 8) + U8_LOW2CHAR((v_u8)))
#define VER_CHAR2NUM(c)		    ((((c) >= '0')&&((c) <= '9')) ? ((c) - '0') : ((((c) >= 'a')&&((c) <= 'f')) ? ((c)-'a' + 0x0a) : ((((c) >= 'A')&&((c) <= 'F')) ? ((c)-'A' + 0x0a) : (c))))

#if(CHIP_TYPE == CHIP_TYPE_8258)
#define PID_CHIP_TYPE			0 // set 0 for compatibility.
#elif(CHIP_TYPE == CHIP_TYPE_8278)
#define PID_CHIP_TYPE			1
#elif(CHIP_TYPE == CHIP_TYPE_8269)
#define PID_CHIP_TYPE			2
#elif(CHIP_TYPE == CHIP_TYPE_9518)
#define PID_CHIP_TYPE			3
#else
#error error mcu core type
#endif

/*
 * demo PID filed: 
 * {
 *     minor product type	: 8 // lowest 8 bit
 *     major product type	: 4
 *     MCU chip type	    : 4
 * }
*/
#define PID_DEV_TYPE_LEN		(12)
#define PID_UNKNOW              (0x0000)
// ------ light ------
#define PID_LIGHT				((PID_CHIP_TYPE << PID_DEV_TYPE_LEN) | LIGHT_TYPE_SEL)
// ------ gateway ------
#define PID_GATEWAY             ((PID_CHIP_TYPE << PID_DEV_TYPE_LEN) | 0x0101)
// ------ LPN ------
#define PID_LPN                 ((PID_CHIP_TYPE << PID_DEV_TYPE_LEN) | 0x0201)
// ------ SWITCH ------
#define PID_SWITCH              ((PID_CHIP_TYPE << PID_DEV_TYPE_LEN) | 0x0301)
// ------ SPIRIT_LPN ------
#define PID_SPIRIT_LPN          ((PID_CHIP_TYPE << PID_DEV_TYPE_LEN) | 0x0401)
// ------ gateway node with homekit ------
#define PID_GW_NODE_HK          ((PID_CHIP_TYPE << PID_DEV_TYPE_LEN) | 0x0501)

// ------ HOME KIT ------
// from 0xC000 -- 0xFFFF


/*
MESH_PID_SEL : PID is product ID,
MESH_VID: VID is software version ID,

PID and VID are used in composition data: model_sig_cfg_s_cps.
format: please refer to spec "4.2.1.1 Composition Data Page 0"
user can be allowed to redefined PID and VID if needed.
*/

#if (WIN32)
#define MESH_PID_SEL		(PID_LIGHT)
#define MESH_VID		    FW_VERSION_TELINK_RELEASE       // user can redefine
#elif (__PROJECT_MESH_PRO__ || __PROJECT_MESH_GW_NODE__)  // must define in TC32_CC_Assembler ->General , too. because cstartup.s can't read predefine value in TC32_compiler-->symbols
#define MESH_PID_SEL		(PID_GATEWAY)
#define MESH_VID		    FW_VERSION_TELINK_RELEASE       // user can redefine
#elif (__PROJECT_MESH_LPN__)  // must define in TC32_CC_Assembler ->General , too. because cstartup.s can't read predefine value in TC32_compiler-->symbols
#define MESH_PID_SEL		(PID_LPN)
#define MESH_VID		    FW_VERSION_TELINK_RELEASE       // user can redefine
#elif (__PROJECT_MESH_SWITCH__)  // must define in TC32_CC_Assembler ->General , too. because cstartup.s can't read predefine value in TC32_compiler-->symbols
#define MESH_PID_SEL		(PID_SWITCH)
#define MESH_VID		    FW_VERSION_TELINK_RELEASE       // user can redefine
#elif (__PROJECT_SPIRIT_LPN__)  // must define in TC32_CC_Assembler ->General , too. because cstartup.s can't read predefine value in TC32_compiler-->symbols
#define MESH_PID_SEL		(PID_SPIRIT_LPN)
#define MESH_VID		    FW_VERSION_TELINK_RELEASE       // user can redefine
#elif (__PROJECT_8267_MASTER_KMA_DONGLE__)
#define MESH_PID_SEL		(PID_UNKNOW)
#define MESH_VID		    FW_VERSION_TELINK_RELEASE       // user can redefine
#elif (__PROJECT_MESH__)   // light
#define MESH_PID_SEL		(PID_LIGHT)
	#if DU_ENABLE
#define MESH_VID		    DU_FW_VER       // in the du mode ,we will use to set version .
	#else
#define MESH_VID		    FW_VERSION_TELINK_RELEASE       // user can redefine
	#endif
#elif (__PROJECT_MESH_GW_NODE_HK__)   // light
#define MESH_PID_SEL		(PID_GW_NODE_HK)
#define MESH_VID		    FW_VERSION_TELINK_RELEASE       // user can redefine
#elif (__PROJECT_BOOTLOADER__ || __PROJECT_ACL_PER_DEMO__)   // light
#define MESH_PID_SEL		(PID_LIGHT)						// 
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

#if (__TLSR_RISCV_EN__)
// TODO
#else
// must set ram size according to the chip type
#ifndef RAM_SIZE_MAX
#define RAM_SIZE_MAX            		(64*1024)
#endif

#ifndef __IRQ_STACK_SIZE__
	#ifdef __PROJECT_MESH_GW_NODE_HK__
#define __IRQ_STACK_SIZE__            	(0x400)
	#else
		#if EXTENDED_ADV_ENABLE
#define __IRQ_STACK_SIZE__            	(0x280)	// cost about 0x1D0 for demo SDK. // because call irq_mesh_sec_msg_check_cache in irq state.
		#else
#define __IRQ_STACK_SIZE__            	(0x180)
		#endif
	#endif
#endif
#endif
