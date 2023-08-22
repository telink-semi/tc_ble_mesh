/********************************************************************************************************
 * @file	config.h
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

#define	CHIP_PACKAGE_QFN64  1
#define	CHIP_PACKAGE_QFN48  2
#define	CHIP_PACKAGE_QFN40  3
#define	CHIP_PACKAGE_QFN32  4

#ifndef CHIP_PACKAGE
#define	CHIP_PACKAGE 		CHIP_PACKAGE_QFN64
#endif

#include "config_chip_type.h"


#ifndef CHIP_TYPE
#if(WIN32)
#define	CHIP_TYPE 			CHIP_TYPE_8269
#else
#define	CHIP_TYPE 			1000
#endif
#endif

#ifndef CHIP_8366_A1
#define CHIP_8366_A1    1
#endif

#define	MCU_CORE_8266 		1
#define	MCU_CORE_8366 		2
#define MCU_CORE_8368		3
#define	MCU_CORE_8267 		4
#define MCU_CORE_8263 		5
#define MCU_CORE_8261 		6
#define MCU_CORE_8269 		7
#define MCU_CORE_8258 		8
#define MCU_CORE_8278 		9

#if(CHIP_TYPE == CHIP_TYPE_8266)
	#define MCU_CORE_TYPE	MCU_CORE_8266
#elif(CHIP_TYPE == CHIP_TYPE_8267)
	#define MCU_CORE_TYPE	MCU_CORE_8267
#elif(CHIP_TYPE == CHIP_TYPE_8366)
	#define MCU_CORE_TYPE	MCU_CORE_8366
#elif(CHIP_TYPE == CHIP_TYPE_8368)
	#define MCU_CORE_TYPE	MCU_CORE_8368
#elif(CHIP_TYPE == CHIP_TYPE_8263)
	#define MCU_CORE_TYPE	MCU_CORE_8263
#elif(CHIP_TYPE == CHIP_TYPE_8261)
	#define MCU_CORE_TYPE	MCU_CORE_8261
#elif(CHIP_TYPE == CHIP_TYPE_8269)
	#define MCU_CORE_TYPE	MCU_CORE_8269
#elif(CHIP_TYPE == CHIP_TYPE_8258)
	#define MCU_CORE_TYPE	MCU_CORE_8258
#elif(CHIP_TYPE == CHIP_TYPE_8278)
	#define MCU_CORE_TYPE	MCU_CORE_8278
#else
	#define MCU_CORE_TYPE	1000
#endif


