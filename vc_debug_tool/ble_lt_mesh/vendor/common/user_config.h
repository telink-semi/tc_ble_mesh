/********************************************************************************************************
 * @file	user_config.h
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

#if ANDROID_APP_ENABLE || IOS_APP_ENABLE 
#pragma pack(1)
#endif
#include "config.h"

#define BOOT_MARK_ADDR_B85M		(0x08)
#define BOOT_MARK_ADDR_B91M		(0x20)


#if (__PROJECT_8266_MASTER_KMA_DONGLE__ || __PROJECT_8267_MASTER_KMA_DONGLE__)
	#include "../8267_master_kma_dongle/app_config.h"
#elif (__PROJECT_MESH__ || WIN32)	// include MESH / lib_sig_mesh.
	#if((MCU_CORE_TYPE == MCU_CORE_8269) || WIN32)
	#include "../mesh/app_config_8269.h"
	#elif(MCU_CORE_TYPE == MCU_CORE_8258)
	#include "../mesh/app_config_8258.h"
	#elif(MCU_CORE_TYPE == MCU_CORE_8278)
	#include "../mesh/app_config_8278.h"
	#elif(MCU_CORE_TYPE == MCU_CORE_9518)
	#include "../mesh/app_config_B91.h"
	#endif
#elif (__PROJECT_MESH_LPN__)		// include MESH_LPN / lib_sig_mesh_LPN.
	#if(MCU_CORE_TYPE == MCU_CORE_8269)
	#include "../mesh_lpn/app_config_8269.h"
	#elif(MCU_CORE_TYPE == MCU_CORE_8258)
	#include "../mesh_lpn/app_config_8258.h"
	#elif(MCU_CORE_TYPE == MCU_CORE_8278)
	#include "../mesh_lpn/app_config_8278.h"
	#elif(MCU_CORE_TYPE == MCU_CORE_9518)
	#include "../mesh_lpn/app_config_B91.h"
	#endif
#elif (__PROJECT_MESH_SWITCH__)
	#if(MCU_CORE_TYPE == MCU_CORE_8269)
	#include "../mesh_switch/app_config_8269.h"
	#elif(MCU_CORE_TYPE == MCU_CORE_8258)
	#include "../mesh_switch/app_config_8258.h"
	#elif(MCU_CORE_TYPE == MCU_CORE_8278)
	#include "../mesh_switch/app_config_8278.h"
	#elif(MCU_CORE_TYPE == MCU_CORE_9518)
	#include "../mesh_switch/app_config_B91.h"
	#endif
#elif (__PROJECT_MESH_PRO__ || __PROJECT_MESH_GW_NODE__)
	#if(MCU_CORE_TYPE == MCU_CORE_8269)
	#include "../mesh_provision/app_config_8269.h"
	#elif(MCU_CORE_TYPE == MCU_CORE_8258)
	#if(__PROJECT_MESH_GW_NODE_HK__)
	#include "../mesh_gw_node_homekit/app_config_8258.h"
	#else
	#include "../mesh_provision/app_config_8258.h"
	#endif
	#elif(MCU_CORE_TYPE == MCU_CORE_8278)
	#include "../mesh_provision/app_config_8278.h"
	#elif(MCU_CORE_TYPE == MCU_CORE_9518)
	#include "../mesh_provision/app_config_B91.h"
	#endif
#elif (__PROJECT_SPIRIT_LPN__)
	#if(MCU_CORE_TYPE == MCU_CORE_8258)
	#include "../spirit_lpn/app_config_8258.h"
	#elif(MCU_CORE_TYPE == MCU_CORE_8278)
	#include "../spirit_lpn/app_config_8278.h"
	#elif(MCU_CORE_TYPE == MCU_CORE_9518)
	#include "../spirit_lpn/app_config_B91.h"
	#endif
#elif (__PROJECT_BOOTLOADER__)
	#if(MCU_CORE_TYPE == MCU_CORE_8269)
	#include "../boot_loader/app_config_8269.h"
	#elif(MCU_CORE_TYPE == MCU_CORE_8258)
	#include "../boot_loader/app_config_8258.h"
	#elif(MCU_CORE_TYPE == MCU_CORE_8278)
	#include "../boot_loader/app_config_8278.h"
	#elif(MCU_CORE_TYPE == MCU_CORE_9518)
	#include "../boot_loader/app_config_B91.h"
	#endif
#elif(__PROJECT_ACL_PER_DEMO__)
	#include "vendor/acl_peripheral_demo/app_config.h"
#else
	#include "../common/default_config.h"
#endif

