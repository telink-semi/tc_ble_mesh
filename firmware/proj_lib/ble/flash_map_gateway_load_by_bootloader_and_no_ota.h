/********************************************************************************************************
 * @file	flash_map_gateway_load_by_bootloader_and_no_ota.h
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

#include "vendor/common/user_config.h"

#if FW_LOAD_BY_BOOTLOADER_AND_NO_OTA_EN
#define 		FLASH_ADR_AREA_FIRMWARE_END	0x60000
/* Flash adr 0x00000 ~ 0x2ffff  is firmware area*/
#define			FLASH_ADR_AREA_1_START		0x60000
#define			FLASH_ADR_MESH_KEY			FLASH_ADR_AREA_1_START
#define			FLASH_ADR_MD_CFG_S			0x61000
#define			FLASH_ADR_MD_HEALTH			0x62000
#define			FLASH_ADR_MD_G_ONOFF_LEVEL	0x63000
#define			FLASH_ADR_MD_TIME_SCHEDULE	0x64000
#define			FLASH_ADR_MD_LIGHTNESS		0x65000	// share with power level
#define			FLASH_ADR_MD_LIGHT_CTL		0x66000
#define			FLASH_ADR_MD_LIGHT_LC		0x67000
#define 		FLASH_ADR_MD_MESH_OTA		0x68000 // V4.1.0.0 or earlier version is sw_level
#define 		FLASH_ADR_MD_SENSOR		    0x69000
#define 		FLASH_ADR_PROVISION_CFG_S	0x6a000 // this sector can be deleted for non gateway project later.
#define			FLASH_ADR_MD_LIGHT_HSL		0x6b000 // cps before V23
#define			FLASH_ADR_FRIEND_SHIP		0x6c000 // backup both FLASH_ADR_MISC and FLASH_ADR_VC_NODE_INFO(gateway) temporarily
#define			FLASH_ADR_MD_VD_LIGHT		0x6d000 // V4.1.0.0 or earlier version is misc with sno and iv
#define			FLASH_ADR_MD_MISC_PAR		0x6e000 // V4.1.0.0 or earlier version is factory reset cnt
#define			FLASH_ADR_MD_PROPERTY		0x6f000 // just test
#define			FLASH_ADR_MD_CMR			0x6f000 // just test
#define			FLASH_ADR_MD_DF_SBR			FLASH_ADR_MESH_TYPE_FLAG // 3f000 had been use as FLASH_ADR_VC_NODE_INFO in gateway.

#define 		FLASH_ADR_VC_NODE_INFO		0x6f000		//
#define 		FLASH_ADR_VC_NODE_INFO_END	0x70000		// not included

#define			FLASH_ADR_AREA_1_END		0x70000

//#define         FLASH_ADR_UPDATE_NEW_FW     0x20000   //

#define FLASH_ADR_PROTECT_END           0x60000     // must, for flash protection default area for firmware, not parameter address definition.

/* Flash adr 0x40000 ~ 0x6ffff  is firmware(OTA) area*/
#define			FLASH_ADR_AREA_2_START		0x70000
#define			FLASH_ADR_MISC				FLASH_ADR_AREA_2_START // V4.1.0.0 or earlier version is vendor model
#define			FLASH_ADR_MD_G_POWER_ONOFF	0x71000
#define			FLASH_ADR_MD_SCENE			0x72000

#define			FLASH_ADR_MESH_TYPE_FLAG	0x73000	// don't change, must same with telink mesh SDK
#define			FLASH_ADR_SW_LEVEL			0x74000 // V4.1.0.0 or earlier version is mesh ota model
#define			FLASH_ADR_RESET_CNT			0x75000	// V4.1.0.0 or earlier version is parameter of misc model

#define			FLASH_ADR_AREA_2_END		0x76000

#if (MD_DF_CFG_SERVER_EN)
#define 		FLASH_ADR_FIXED_FWD_TBL          0x78000		// fixed forwarding table
#define 		FLASH_ADR_FIXED_FWD_TBL_END      0x79000		// not included
    #if (CONFIG_ALWAYS_GET_ROUTE_FROM_FLASH)
#define 		FLASH_ADR_NON_FIXED_FWD_TBL      0x79000		// non-fixed forwarding table
#define 		FLASH_ADR_NON_FIXED_FWD_TBL_END  0x7a000		// not included
    #endif
#endif

/*******vendor define here, from 0x7ffff ~ 0x78000 ...
vendor use from 0x7ffff to 0x78000 should be better, because telink may use 0x78000,0x79000 later.
********************************************/
#if BLE_REMOTE_SECURITY_ENABLE
#define 		FLASH_ADR_SMP_PARA_START	0x79000 // 0x7A000 will be use also, smp para use 2 sequential sectors.
#define 		FLASH_ADR_SMP_PARA_2		0x7a000 // 
#endif

#if ENERGY_HARVEST_RX_EN
#define         FLASH_ADR_EH_DEVICE_LIST    0x7b000
#endif

#if SAVE_SNO_CACHE_EN
#define 		FLASH_ADR_SNO_CACHE			0x7c000 // this is a user function, address value can be redefined by user
#endif

#define			FLASH_ADR_PAR_USER_MAX		0x80000
#endif

