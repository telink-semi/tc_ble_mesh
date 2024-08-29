/********************************************************************************************************
 * @file	blt_config.h
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

//////////////////////////////////////////////////////////////////////////////
/**
 *  @brief  Definition for Device info
 */
#if !WIN32
//#include "proj/mcu/config.h"
//#include "proj/mcu/analog.h"
//#include "../rf_drv.h"
#endif

#include "vendor/common/user_config.h"
#include "drivers.h"
#include "../pm.h"

#define PA_ENABLE	0

#ifndef MY_RF_POWER_INDEX
	#if(PA_ENABLE)
		#if(MCU_CORE_TYPE == MCU_CORE_8269)
#define		MY_RF_POWER_INDEX		RF_POWER_0dBm
		#elif(MCU_CORE_TYPE == MCU_CORE_8258)
#define		MY_RF_POWER_INDEX		RF_POWER_P0p04dBm
		#elif(MCU_CORE_TYPE == MCU_CORE_8278)
#define		MY_RF_POWER_INDEX		RF_POWER_N0p28dBm
		#endif
	#else
		#if(MCU_CORE_TYPE == MCU_CORE_8269)
#define		MY_RF_POWER_INDEX		RF_POWER_8dBm
		#elif(MCU_CORE_TYPE == MCU_CORE_8258)
    		#if (MESH_USER_DEFINE_MODE == MESH_IRONMAN_MENLO_ENABLE || DUAL_MESH_ZB_BL_EN) // keep same with zb
#define		MY_RF_POWER_INDEX		RF_POWER_P10p46dBm
    		#elif MI_API_ENABLE
#define 	MY_RF_POWER_INDEX		RF_POWER_P10p46dBm
    		#elif DU_ENABLE
#define		MY_RF_POWER_INDEX		RF_POWER_P7p02dBm
			#else
#define		MY_RF_POWER_INDEX		RF_POWER_P3p01dBm	
    		#endif
		#elif(MCU_CORE_TYPE == MCU_CORE_8278)
#define		MY_RF_POWER_INDEX		RF_POWER_P3p50dBm
		#endif
	#endif
#endif

#define  MAX_DEV_NAME_LEN 				18

#ifndef DEV_NAME
#define DEV_NAME                        "tModule"
#endif

enum{
	TYPE_TLK_MESH		    = 0x000000A3,// don't change, must same with telink mesh SDK
	TYPE_SIG_MESH		    = 0x0000003A,// don't change, must same with telink mesh SDK
	TYPE_TLK_BLE_SDK	    = 0x000000C3,// don't change, must same with telink mesh SDK
	TYPE_TLK_ZIGBEE 	    = 0x0000003C,// don't change, must same with telink mesh SDK
    TYPE_DUAL_MODE_STANDBY  = 0x00000065,// dual mode state was standby to be selected
	TYPE_DUAL_MODE_RECOVER 	= 0x00000056,// don't change, must same with zigbee SDK, recover from zigbee.
    TYPE_DUAL_MODE_ZIGBEE_RESET  = 0x00000053,// don't change.
    TYPE_TLK_HOMEKIT        = 0x00000095,// don't change.
};

#ifndef FLASH_R_BASE_ADDR
#define FLASH_R_BASE_ADDR			0
#endif

#if (MESH_USER_DEFINE_MODE == MESH_IRONMAN_MENLO_ENABLE)
/*** static sector information ***/

/* macro for the offset of the data element
 *
 * note: make sure you update the new element when you add to dev_info_t!
 * 
 */

typedef struct {
	u8 version;                     // 0x00
	u32 checksum;
	u32 block_len;
	u8 mac_mesh[6];                 // 0x09
	u8 mac_zigbee[8];
	u8 mesh_static_oob[32];         // 0x17 // change from 16 to 32 due to EPA.
	u8 zb_pre_install_code[17];
	u8 device_serial_number[16];
	u8 build_info[32];
	u16 min_color_k;
	u16 max_color_k;
	u32 cal_data_1;
	u32 cal_data_2;
	u32 cal_data_3;
	u32 cal_data_4;
} static_dev_info_t;

#define STATIC_DEV_INFO_ADR         (0x6000)

#define STATIC_ADDR_MAC_MESH	    (STATIC_DEV_INFO_ADR + OFFSETOF(static_dev_info_t, mac_mesh))
#define STATIC_ADDR_MAC_ZB		    (STATIC_DEV_INFO_ADR + OFFSETOF(static_dev_info_t, mac_zigbee))
#define STATIC_ADDR_MESH_STATIC_OOB (STATIC_DEV_INFO_ADR + OFFSETOF(static_dev_info_t, mesh_static_oob))
#define STATIC_ZB_PRE_INSTALL_CODE  (STATIC_DEV_INFO_ADR + OFFSETOF(static_dev_info_t, zb_pre_install_code))

/*!! TO BE DEFINED - OFFSETOF(static_dev_info_t, tx_cal_power) !!*/
//----------------------------     0x7000   // DC (CFG_SECTOR_ADR_CALIBRATION_CODE)
#define STATIC_ADDR_TX_PWR_OFFSET	0x7001


/* end of static sector information */
#elif ((MESH_USER_DEFINE_MODE == MESH_ZB_BL_DUAL_ENABLE) || (MESH_USER_DEFINE_MODE == MESH_SIG_PVT_DUAL_ENABLE))
typedef struct {
	u8 mesh_static_oob[32]; // change from 16 to 32 due to EPA.
	u8 zb_pre_install_code[17];
} static_dev_info_t;

#define STATIC_DEV_INFO_ADR         (0x6000)

#define STATIC_ADDR_MESH_STATIC_OOB (STATIC_DEV_INFO_ADR + OFFSETOF(static_dev_info_t, mesh_static_oob))
#define STATIC_ZB_PRE_INSTALL_CODE  (STATIC_DEV_INFO_ADR + OFFSETOF(static_dev_info_t, zb_pre_install_code))
#endif

/** Calibration Information FLash Address Offset of  CFG_ADR_CALIBRATION_xx_FLASH ***/
#define		CALIB_OFFSET_CAP_INFO								(0x0)
#define		CALIB_OFFSET_TP_INFO								(0x40)
#define		OFFSET_CUST_RC32K_CAP_INFO                          (0x80)
#define     CALIB_OFFSET_FLASH_VREF								0x1c0

/** Calibration Information end ***/

/**************************** 128 K Flash *****************************/
#if 0
#define		CFG_ADR_MAC_128K_FLASH								0x1F000 // don't change
#define		CFG_ADR_CALIBRATION_128K_FLASH						0x1E000 // don't change
#endif
/**************************** 512 K Flash *****************************/
#define		CFG_ADR_MAC_512K_FLASH								0x76000 // don't change
#define		CFG_ADR_CALIBRATION_512K_FLASH						0x77000 // don't change

/**************************** 1 M Flash *******************************/
#define		CFG_ADR_MAC_1M_FLASH		   						0xFF000 // don't change
#define		CFG_ADR_CALIBRATION_1M_FLASH						0xFE000 // don't change

/**************************** 2 M Flash *******************************/
#define		CFG_ADR_MAC_2M_FLASH		   						0x1FF000
#define		CFG_ADR_CALIBRATION_2M_FLASH						0x1FE000

/**************************** 4 M Flash *******************************/
#define		CFG_ADR_MAC_4M_FLASH		   						0x3FF000
#define		CFG_ADR_CALIBRATION_4M_FLASH						0x3FE000

#if FLASH_PLUS_ENABLE
    #if (MESH_USER_DEFINE_MODE == MESH_IRONMAN_MENLO_ENABLE)
#define		CFG_SECTOR_ADR_MAC_CODE		        (STATIC_ADDR_MAC_MESH)
#define		CFG_SECTOR_ADR_CALIBRATION_CODE     (0x7000)
    #else
#define		CFG_SECTOR_ADR_MAC_CODE		        CFG_ADR_MAC_1M_FLASH
#define		CFG_SECTOR_ADR_CALIBRATION_CODE     CFG_ADR_CALIBRATION_1M_FLASH
    #endif
#else
#define		CFG_SECTOR_ADR_MAC_CODE		        CFG_ADR_MAC_512K_FLASH
#define		CFG_SECTOR_ADR_CALIBRATION_CODE     CFG_ADR_CALIBRATION_512K_FLASH
#endif

#if (DUAL_MESH_ZB_BL_EN||DUAL_MESH_SIG_PVT_EN||(MCU_CORE_TYPE == MCU_CORE_8267)||(MCU_CORE_TYPE == MCU_CORE_8269)\
   ||(MESH_USER_DEFINE_MODE == MESH_IRONMAN_MENLO_ENABLE)||(MESH_USER_DEFINE_MODE == MESH_PIPA_ENABLE))
#define AUTO_ADAPT_MAC_ADDR_TO_FLASH_TYPE_EN    (0) // must 0
#else
#define AUTO_ADAPT_MAC_ADDR_TO_FLASH_TYPE_EN    ((CFG_SECTOR_ADR_MAC_CODE == CFG_ADR_MAC_512K_FLASH)||(CFG_SECTOR_ADR_MAC_CODE == CFG_ADR_MAC_1M_FLASH))
#endif


#if (!(WIN32 || __TLSR_RISCV_EN__ || FLASH_PLUS_ENABLE || TLV_ENABLE))
	#if (APP_FLASH_PROTECTION_ENABLE && (CFG_SECTOR_ADR_MAC_CODE == CFG_ADR_MAC_512K_FLASH))
/* for flash protection function, automatically move some sectors that require frequent write operations to after 0x70000(such as FLASH_LOCK_LOW_448K_MID1360C8),
 * include FLASH_ADR_RESET_CNT, FLASH_ADR_MISC, and FLASH_ADR_SW_LEVEL.
 */
#define FLASH_MAP_AUTO_EXCHANGE_SOME_SECTORS_EN		    1 // If it is a new product development, there is no need to consider compatibity with previous versions, then FLASH_MAP_AUTO_EXCHANGE_SOME_SECTORS_EN can be disabled to save firmware size.

#define FLASH_MAP_AUTO_EXCHANGE_SOME_SECTORS_ASSERT_EN  1 // must enable to do checking.
	#endif
#endif

#ifndef FLASH_MAP_AUTO_EXCHANGE_SOME_SECTORS_EN
#define FLASH_MAP_AUTO_EXCHANGE_SOME_SECTORS_EN		    0 // other case must be 0
#endif
#ifndef FLASH_MAP_AUTO_EXCHANGE_SOME_SECTORS_ASSERT_EN
#define FLASH_MAP_AUTO_EXCHANGE_SOME_SECTORS_ASSERT_EN  0 // other case must be 0
#endif

/////////////////// Flash  Address Config ////////////////////////////
#define	FLASH_SECTOR_SIZE       (4096)

#define	FLASH_ADDRESS_DEFINE						\
		u32 flash_adr_misc      = FLASH_ADR_MISC;							
#define FLASH_ADDRESS_CONFIG						
		//flash_adr_misc 			= FLASH_ADR_MISC;			
#define FLASH_ADDRESS_EXTERN						\
		extern unsigned int flash_adr_misc;	

#if (0 == FLASH_PLUS_ENABLE)
#define 		FLASH_ADR_AREA_FIRMWARE_END	0x30000
/* Flash adr 0x00000 ~ 0x2ffff  is firmware area*/
#define			FLASH_ADR_AREA_1_START		0x30000
#define			FLASH_ADR_MESH_KEY			FLASH_ADR_AREA_1_START
#define			FLASH_ADR_MD_CFG_S			0x31000
#define			FLASH_ADR_MD_HEALTH			0x32000
#define			FLASH_ADR_MD_G_ONOFF_LEVEL	0x33000
#define			FLASH_ADR_MD_TIME_SCHEDULE	0x34000
#define			FLASH_ADR_MD_LIGHTNESS		0x35000	// share with power level
#define			FLASH_ADR_MD_LIGHT_CTL		0x36000
#define			FLASH_ADR_MD_LIGHT_LC		0x37000
#define 		FLASH_ADR_MD_MESH_OTA		0x38000 // V4.1.0.0 or earlier version is sw_level
#define 		FLASH_ADR_MD_SENSOR		    0x39000
#define 		FLASH_ADR_PROVISION_CFG_S	0x3a000 // this sector can be deleted for non gateway project later.
#define			FLASH_ADR_MD_LIGHT_HSL		0x3b000 // cps before V23
#define			FLASH_ADR_FRIEND_SHIP		0x3c000 // backup both FLASH_ADR_MISC and FLASH_ADR_VC_NODE_INFO(gateway) temporarily
#define			FLASH_ADR_MD_VD_LIGHT		0x3d000 // V4.1.0.0 or earlier version is misc with sno and iv
#define			FLASH_ADR_MD_MISC_PAR		0x3e000 // V4.1.0.0 or earlier version is factory reset cnt
#if WIN32
#define			FLASH_ADR_MD_PROPERTY		0x40000
#define			FLASH_ADR_MD_DF_SBR			0x41000 
#else
#define			FLASH_ADR_MD_PROPERTY		0x3f000 // just test
#define			FLASH_ADR_MD_CMR			0x3f000 // just test
#if GATEWAY_ENABLE
#define			FLASH_ADR_MD_DF_SBR			FLASH_ADR_MESH_TYPE_FLAG // 3f000 had been use as FLASH_ADR_VC_NODE_INFO in gateway.
#else
#define			FLASH_ADR_MD_DF_SBR			0x3f000
#endif
#endif

#if (__PROJECT_MESH_PRO__ || __PROJECT_MESH_GW_NODE__)
	#if WIN32
#define 		FLASH_ADR_VC_NODE_INFO		0x80000		//  from 0x00000 to 0x40000 (256K)
	#else
		#if DEBUG_CFG_CMD_GROUP_AK_EN
#define         FLASH_ADR_VC_NODE_INFO      0x78000 	// vcnode info, occupy sector num dependent on  MESH_NODE_MAX_NUM
#define 		FLASH_ADR_VC_NODE_INFO_END	0x7A000		// not included
		#else
#define 		FLASH_ADR_VC_NODE_INFO		0x3f000		//
#define 		FLASH_ADR_VC_NODE_INFO_END	0x40000		// not included
		#endif
	#endif
#endif
#define			FLASH_ADR_AREA_1_END		0x40000

/* Flash adr 0x40000 ~ 0x6ffff  is firmware(OTA) area*/
#define			FLASH_ADR_AREA_2_START		0x70000
#define			FLASH_ADR_MISC				FLASH_ADR_AREA_2_START // V4.1.0.0 or earlier version is vendor model
#define			FLASH_ADR_MD_G_POWER_ONOFF	0x71000
#define			FLASH_ADR_MD_SCENE			0x72000

#define			FLASH_ADR_MESH_TYPE_FLAG	0x73000	// don't change, must same with telink mesh SDK
#define			FLASH_ADR_SW_LEVEL			0x74000 // V4.1.0.0 or earlier version is mesh ota model
	#if TLV_ENABLE
//#define			FLASH_ADR_RESET_CNT			0x35000 // TODO
	#else
#define			FLASH_ADR_RESET_CNT			0x75000	// V4.1.0.0 or earlier version is parameter of misc model
	#endif

#define			FLASH_ADR_AREA_2_END		0x76000


/*******vendor define here, from 0x7ffff ~ 0x78000 ...
vendor use from 0x7ffff to 0x78000 should be better, because telink may use 0x78000,0x79000 later.
********************************************/
#if (MESH_USER_DEFINE_MODE == MESH_MI_ENABLE)
#define 		FLASH_ADR_MI_RECORD			0x78000
#define 		FLASH_ADR_MI_RECORD_TMP		0x79000
#define 		FLASH_ADR_MI_RECORD_MAX		0x7a000

//                      MI_BLE_MESH_CER_ADR	        0x7F000 // don't modify
#elif (MESH_USER_DEFINE_MODE == MESH_MI_SPIRIT_ENABLE)
#define 		FLASH_ADR_THREE_PARA_ADR	0x78000
#define         FLASH_ADR_THREE_PARA_ADR_0x100_0xF00    //please refer to "FLASH_ADR_EDCH_PARA"
#define 		FLASH_ADR_MI_RECORD			0x79000
#define 		FLASH_ADR_MI_RECORD_TMP		0x7a000
#define 		FLASH_ADR_MI_RECORD_MAX		0x7b000
    #if (ALI_MD_TIME_EN)
#define 		FLASH_ADR_VD_TIME_INFO	    FLASH_ADR_MI_RECORD_MAX  // 0x7b000
    #endif

//                      MI_BLE_MESH_CER_ADR	        0x7F000 // don't modify
#elif(AIS_ENABLE)
#if(MESH_USER_DEFINE_MODE == MESH_TAIBAI_ENABLE)
#define DU_STORE_ADR		0x7e000
#define DU_OTA_REBOOT_ADR	0x7f000

#endif

#define 		FLASH_ADR_THREE_PARA_ADR	0x78000
#define         FLASH_ADR_THREE_PARA_ADR_0x100_0xF00    //please refer to "FLASH_ADR_EDCH_PARA"
    #if (ALI_MD_TIME_EN)
#define 		FLASH_ADR_VD_TIME_INFO		0x79000
    #endif
#elif(DUAL_MODE_WITH_TLK_MESH_EN)
#define			FLASH_ADR_DUAL_MODE_4K		0x78000 // backup dual mode 4K firmware
#elif(LLSYNC_ENABLE)
#define			FLASH_ADR_THREE_PARA_ADR	0x78000
#elif (CERTIFY_BASE_ENABLE)
#define 		FLASH_ADR_CERTIFY_ADR		0x78000 // we will burn in an 4k bin
#endif

#if BLE_REMOTE_SECURITY_ENABLE
#define 		FLASH_ADR_SMP_PARA_START	0x79000 // 0x7A000 will be use also, smp para use 2 sequential sectors.
#define 		FLASH_ADR_SMP_PARA_2		0x7a000 // 
#endif
#if MI_API_ENABLE
#define         MI_BLE_MESH_CER_ADR 	    0x7E000// because the certify is stored in the 0x7e800
#define 		FLASH_ADR_PAR_USER_MAX		MI_BLE_MESH_CER_ADR
#else

#define			FLASH_ADR_PAR_USER_MAX		0x80000
#endif

#if FLASH_MAP_AUTO_EXCHANGE_SOME_SECTORS_EN // flash map legacy value of V4.1.0.0 or earlier versions(FW_VERSION_TELINK_RELEASE)
#define FLASH_ADR_MISC_LEGACY_VERSION				(0x3d000) // exchange with vendor model
#define FLASH_ADR_SW_LEVEL_LEGACY_VERSION			(0x38000) // exchange with mesh ota model
#define FLASH_ADR_RESET_CNT_LEGACY_VERSION			(0x3e000) // exchange with parameter of misc model
#define FLASH_ADR_MD_VD_LIGHT_LEGACY_VERSION		(0x70000) // exchange with misc sno/iv model

// exchange flash sector address between ADDR_x_A and ADDR_x_B
#define FLASH_MAP_AUTO_EXCHANGE_ADDR_1_A            (FLASH_ADR_MISC)        // must be FLASH_ADR_MISC
#define FLASH_MAP_AUTO_EXCHANGE_ADDR_1_B            (FLASH_ADR_MD_VD_LIGHT) // must be FLASH_ADR_MD_VD_LIGHT
#define FLASH_MAP_AUTO_EXCHANGE_ADDR_2_A            (FLASH_ADR_SW_LEVEL)
#define FLASH_MAP_AUTO_EXCHANGE_ADDR_2_B            (FLASH_ADR_MD_MESH_OTA)
#define FLASH_MAP_AUTO_EXCHANGE_ADDR_3_A            (FLASH_ADR_RESET_CNT)
#define FLASH_MAP_AUTO_EXCHANGE_ADDR_3_B            (FLASH_ADR_MD_MISC_PAR)
#endif

#else // 1M flash // (FLASH_PLUS_ENABLE == 1)
#if ((MESH_USER_DEFINE_MODE == MESH_IRONMAN_MENLO_ENABLE)||(MESH_USER_DEFINE_MODE == MESH_ZB_BL_DUAL_ENABLE)||(MESH_USER_DEFINE_MODE == MESH_SIG_PVT_DUAL_ENABLE))
#define			FLASH_ADR_RESET_CNT			0x23000
#define			FLASH_ADR_MESH_TYPE_FLAG	0x24000	// don't change, must same with zigbee mesh SDK
#define         FLASH_ADR_EDCH_PARA_SECTOR	0x25000

#define			FLASH_ADR_AREA_1_START		0x26000
#define			FLASH_ADR_MESH_KEY			FLASH_ADR_AREA_1_START
#define			FLASH_ADR_MD_CFG_S			0x27000
#define			FLASH_ADR_MD_HEALTH			0x28000
#define			FLASH_ADR_MD_G_ONOFF_LEVEL	0x29000
#define			FLASH_ADR_MD_TIME_SCHEDULE	0x2A000
#define			FLASH_ADR_MD_LIGHTNESS		0x2B000	// share with power level
#define			FLASH_ADR_MD_LIGHT_CTL		0x2C000
#define			FLASH_ADR_MD_LIGHT_LC		0x2D000
#define			FLASH_ADR_SW_LEVEL			0x2E000
#define   		FLASH_ADR_MD_SENSOR			0x2F000
#define 		FLASH_ADR_PROVISION_CFG_S	0x30000
#define			FLASH_ADR_MD_LIGHT_HSL		0x31000 // cps before V23
#define			FLASH_ADR_FRIEND_SHIP		0x32000
#define			FLASH_ADR_MISC				0x33000
#define			FLASH_ADR_MD_PROPERTY		0x34000 // just test
#define			FLASH_ADR_MD_VD_LIGHT		0x35000
#define			FLASH_ADR_MD_G_POWER_ONOFF	0x36000
#define			FLASH_ADR_MD_SCENE			0x37000
#define			FLASH_ADR_MD_MESH_OTA		0x38000
//#define                                   0x39000 // reserve, before is remote provision. 
#define 		FLASH_ADR_VC_NODE_INFO		0x3A000	//
#define 		FLASH_ADR_VC_NODE_INFO_END	0x3B000	// not included
#define			FLASH_ADR_AREA_1_END		0x3B000
// FLASH_ADR_AREA_1_END to start of user is reserve for telink
/*******SIG mesh vendor define here, from FLASH_ADR_USER_MESH_END ~ FLASH_ADR_USER_MESH_START, vendor define from behined to head should be better, .*/
//#define			FLASH_ADR_USER_MESH_START	0x3E000

//#define			FLASH_ADR_USER_MESH_END	    0x40000

// 0x40000 ~ 0xFFFFF for firmware
/*MESH_IRONMAN_MENLO_ENABLE end*/
#elif(0 ==SWITCH_FW_ENABLE) // normal mode
    #if PINGPONG_OTA_DISABLE
#define 		FLASH_ADR_AREA_FIRMWARE_END	0x5A000 // 360K
#define         FLASH_ADR_UPDATE_NEW_FW     FLASH_ADR_AREA_FIRMWARE_END
/* Firmware Run area 0x00000 ~ 0x59fff, Firmware OTA receive area 0x5a000 ~ 0xb3fff*/
    #else
#define 		FLASH_ADR_AREA_FIRMWARE_END	0x3F000 // 252K
/* Firmware Run area A: 0x00000 ~ 0x3efff, Firmware Run area A: 0x40000 ~ 0x7efff*/
    #endif
/* Flash adr 0x00000 ~ 0xb3fff  is firmware area*/
#define			FLASH_ADR_AREA_1_START		0xB4000 // should be greater than or equal to FLASH_ADR_AREA_FIRMWARE_END * 2, because use same address whether disable pingpong or not.
#define			FLASH_ADR_MESH_KEY			FLASH_ADR_AREA_1_START
#define			FLASH_ADR_MD_CFG_S			0xB5000
#define			FLASH_ADR_MD_HEALTH			0xB6000
#define			FLASH_ADR_MD_G_ONOFF_LEVEL	0xB7000
#define			FLASH_ADR_MD_TIME_SCHEDULE	0xB8000
#define			FLASH_ADR_MD_LIGHTNESS		0xB9000	// share with power level
#define			FLASH_ADR_MD_LIGHT_CTL		0xBA000
#define			FLASH_ADR_MD_LIGHT_LC		0xBB000
#define	FLASH_ADR_SW_LEVEL_1M_LEGACY    0xBC000     // can not be used, because it is FLASH_ADR_SW_LEVEL_ at version V4.1.0.0 or earlier.
#define   		FLASH_ADR_MD_SENSOR			0xBD000
#define 		FLASH_ADR_PROVISION_CFG_S	0xBE000
#define			FLASH_ADR_MD_LIGHT_HSL		0xBF000 // cps before V23
#define FLASH_ADR_PROTECT_END	        0xC0000     // must, for flash protection default area, not parameter address definition.
#define			FLASH_ADR_FRIEND_SHIP		0xC0000
#define			FLASH_ADR_MISC				0xC1000
#define			FLASH_ADR_RESET_CNT			0xC2000
#define			FLASH_ADR_MD_DF_SBR			0xC3000 // this sector did not be used at version V4.1.0.0 or earlier.
#define			FLASH_ADR_MD_PROPERTY		0xC4000 // just test
#define			FLASH_ADR_MD_VD_LIGHT		0xC5000
#define			FLASH_ADR_MD_G_POWER_ONOFF	0xC6000
#define			FLASH_ADR_MD_SCENE			0xC7000
// 				                            0xC8000 // reserved for scene id list which may over 4k. 
#define			FLASH_ADR_MD_MESH_OTA		0xC9000
//#define                                   0xCA000 // reserve, before is remote provision. 
#define			FLASH_ADR_AREA_1_END		0xCB000 // need to change if add parameters before. not set to 0xF0000 to save time when factory reset.

#define         FLASH_MAP_AUTO_MOVE_SW_LEVEL_SECTOR_TO_NEW_ADDR_EN      1

#if (__PROJECT_MESH_PRO__ || __PROJECT_MESH_GW_NODE__) // erase independently when factory reset
#define 		FLASH_ADR_VC_NODE_INFO		0xCB000	//
#define 		FLASH_ADR_VC_NODE_INFO_END	0xCD000	// not included
//#define 		reserve for VC NODE INFO	0xCE000	// when more nodes
//#define 		reserve for VC NODE INFO	0xCF000	// when more nodes
//#define 		reserve for VC NODE INFO	0xD0000	// when more nodes
//#define 		reserve for VC NODE INFO	0xD1000	// when more nodes
#else // for not gateway
// reserved for future use. if use, need to modify factory reset function.
#endif


// from FLASH_ADR_AREA_1_END to FLASH_ADR_USER_MESH_START(0xD2000) is reserved for telink

/*******SIG mesh vendor define here, from FLASH_ADR_USER_MESH_END ~ FLASH_ADR_USER_MESH_START, vendor define from behined to head should be better, .*/
#define			FLASH_ADR_USER_MESH_START	0xD2000

#if BLE_REMOTE_SECURITY_ENABLE                      // erase independently when factory reset
#define 		FLASH_ADR_SMP_PARA_START	0xD3000 // 0xD4000 will be used also, because smp para use 2 sequential sectors.
#define 		FLASH_ADR_SMP_PARA_2		0xD4000 // 
#endif

// add here for user parameters

#define			FLASH_ADR_USER_MESH_END	    0xE0000 // not include
/*user define END*/

// from FLASH_ADR_USER_MESH_END(0xE0000) to FLASH_ADR_USER_MESH_START(0xF0000) is reserved for telink
// reserved for later FLASH_ADR_MISC_		0xE0000 // 
// reserved for later FLASH_ADR_RESET_CNT_	0xE1000 // 

#define			FLASH_ADR_AREA_2_START		0xF0000

#define			FLASH_ADR_SW_LEVEL			FLASH_ADR_AREA_2_START
#define			FLASH_ADR_MD_MISC_PAR		0xF1000 // 

#define			FLASH_ADR_AREA_2_END		0xF2000 // not include

/**
FLASH_ADR_AREA_2_END -- 0xFCFFF is reserved for telink
**/
// for FLASH_ADR_MESH_TYPE_FLAG             0xFD000 // defined at place before "CFG_ADR_MAC"
// for CUST_CAP_INFO_ADDR                   0xFE000
// for CFG_ADR_MAC(with FLASH_ADR_EDCH_PARA)0xFF000

#else//SWITCH_FW_ENABLE == 1
#define 		FLASH_ADR_AREA_FIRMWARE_END		0x40000 // 256K
#define         FLASH_ADR_LIGHT_TELINK_MESH    	0x80000 
#define         FLASH_ADR_UPDATE_NEW_FW     	0xc0000

#define			FLASH_ADR_AREA_1_START		0x40000 // = FLASH_ADR_AREA_FIRMWARE_END * 2, use same address whether disable pingpong or not.
#define			FLASH_ADR_MESH_KEY			FLASH_ADR_AREA_1_START
#define			FLASH_ADR_MD_CFG_S			0x41000
#define			FLASH_ADR_MD_HEALTH			0x42000
#define			FLASH_ADR_MD_G_ONOFF_LEVEL	0x43000
#define			FLASH_ADR_MD_TIME_SCHEDULE	0x44000
#define			FLASH_ADR_MD_LIGHTNESS		0x45000	// share with power level
#define			FLASH_ADR_MD_LIGHT_CTL		0x46000
#define			FLASH_ADR_MD_LIGHT_LC		0x47000
#define			FLASH_ADR_SW_LEVEL			0x48000
#define   		FLASH_ADR_MD_SENSOR			0x49000
#define 		FLASH_ADR_PROVISION_CFG_S	0x4A000
#define			FLASH_ADR_MD_LIGHT_HSL		0x4B000 // cps before V23
#define			FLASH_ADR_FRIEND_SHIP		0x4C000
#define			FLASH_ADR_MISC				0x4D000
#define			FLASH_ADR_RESET_CNT			0x4E000
#define			FLASH_ADR_MD_PROPERTY		0x4F000 // just test
#define			FLASH_ADR_MD_VD_LIGHT		0x50000
#define			FLASH_ADR_MD_G_POWER_ONOFF	0x51000
#define			FLASH_ADR_MD_SCENE			0x52000
// 				                                            0x53000 // reserve now
#define			FLASH_ADR_MD_MESH_OTA		0x54000
//#define                                   0x55000 // reserve, before is remote provision. 
#define 		FLASH_ADR_VC_NODE_INFO		0x56000	//
#define 		FLASH_ADR_VC_NODE_INFO_END	0x57000	// not included
#define			FLASH_ADR_AREA_1_END		0x57000
// FLASH_ADR_AREA_1_END to start of user is reserve for telink
/*******SIG mesh vendor define here, from FLASH_ADR_USER_MESH_END ~ FLASH_ADR_USER_MESH_START, vendor define from behined to head should be better, .*/
#define			FLASH_ADR_USER_MESH_START	0x58000
// TODO
#define			FLASH_ADR_USER_MESH_END	    0x60000
#define			FLASH_ADR_BOOT_FLAG		    0x7F000
/*SIG mesh END*/

#if BLE_REMOTE_SECURITY_ENABLE
#define 		FLASH_ADR_SMP_PARA_START	0x59000 // 0x5A000 will be use also, smp para use 2 sequential sectors.
#define 		FLASH_ADR_SMP_PARA_2		0x5a000 // 
#endif

// ----
// 0xFD000: FLASH_ADR_MESH_TYPE_FLAG; 0xFE000: CUST_CAP_INFO_ADDR;  0xFF000: CFG_ADR_MAC (with FLASH_ADR_EDCH_PARA);  
#endif /*end of (1 ==SWITCH_FW_ENABLE)*/

#endif /*end of (1 == FLASH_PLUS_ENABLE)*/

#if 1// common
#if (FLASH_PLUS_ENABLE && (0 == DUAL_MESH_ZB_BL_EN) && (0 == DUAL_MESH_SIG_PVT_EN))
#define			FLASH_ADR_MESH_TYPE_FLAG	0xFD000	// don't change, must same with telink mesh SDK
#endif

#define		CFG_ADR_MAC					flash_sector_mac_address
#if ((DUAL_MESH_ZB_BL_EN || DUAL_MESH_SIG_PVT_EN) && (!__PROJECT_8267_MASTER_KMA_DONGLE__))
//#define		    CUST_CAP_INFO_ADDR			flash_sector_calibration // == (CFG_ADR_MAC + STATIC_ADDR_FREQ_OFFSET)
#define         FLASH_ADR_STATIC_OOB	    (STATIC_ADDR_MESH_STATIC_OOB)
#define         FLASH_ADR_EDCH_PARA		    (FLASH_ADR_EDCH_PARA_SECTOR) // size = 0x68 = sizeof(mesh_ecdh_key_str)
#define 		SECTOR_PAR_SIZE_MAX			        0x100
#else
#if (0 == FW_START_BY_LEGACY_BOOTLOADER_EN)
#define			CFG_ADR_DUAL_MODE_EN		(CFG_SECTOR_ADR_MAC_CODE + 0x80) // use fixed addr
#endif
#define			CFG_ADR_DUAL_CALI_VAL_FLAG	(CFG_SECTOR_ADR_MAC_CODE + 0x84) // use for DUAL_MODE_WITH_TLK_MESH_EN
#define			CFG_ADR_DUAL_CALI_VAL		(CFG_SECTOR_ADR_MAC_CODE + 0x88) // use for DUAL_MODE_WITH_TLK_MESH_EN
#if AIS_ENABLE  // FLASH_ADR_THREE_PARA_ADR_0x100_0xF00
#define         FLASH_ADR_EDCH_PARA	 	    (FLASH_ADR_THREE_PARA_ADR + 0x100)
#else           // CFG_ADR_MAC_0x100_0xF00
#define         FLASH_ADR_EDCH_PARA		    (flash_sector_mac_address + 0x100) // size = 0x68 = sizeof(mesh_ecdh_key_str)
#endif
#define 		SECTOR_PAR_SIZE_MAX			        0x200
		
	#if((MCU_CORE_TYPE == MCU_CORE_8267)||(MCU_CORE_TYPE == MCU_CORE_8269))
#define		CUST_CAP_INFO_ADDR			(flash_sector_calibration + CALIB_OFFSET_CAP_INFO)
#define			CUST_TP_INFO_ADDR			(flash_sector_calibration + CALIB_OFFSET_TP_INFO)
#define			CUST_RC32K_CAP_INFO_ADDR	(flash_sector_calibration + OFFSET_CUST_RC32K_CAP_INFO)
	#endif
// 0x100 ~ 0x7ff reserve for sihui
    #if (!AIS_ENABLE)
// oob reading regular, please refer to mesh_static_oob_data_by_flash().
#define         FLASH_ADR_STATIC_OOB	    (CFG_SECTOR_ADR_CALIBRATION_CODE + 0x800)   // use const value // flash_sector_calibration
// max size of static OOB is 32 due to EPA
// reserve 16 byte for a independent 16 byte static oob, which means it is not from the first 16byte of 32byte OOB.
        #if (!PROVISION_FLOW_SIMPLE_EN)
#define         FLASH_ADR_DEV_UUID	    	(CFG_SECTOR_ADR_CALIBRATION_CODE + 0x830)   // use const value // flash_sector_calibration
        #endif
    #endif
#endif
#endif

enum{
	// send cmd part ----------------------------------
	HCI_GATEWAY_CMD_START					= 0x00,
	HCI_GATEWAY_CMD_STOP					= 0x01,
	HCI_GATEWAY_CMD_RESET       			= 0x02,
	HCI_GATEWAY_CMD_CLEAR_NODE_INFO			= 0x06,
	HCI_GATEWAY_CMD_SET_ADV_FILTER 			= 0x08,
	HCI_GATEWAY_CMD_SET_PRO_PARA 			= 0x09,
	HCI_GATEWAY_CMD_SET_NODE_PARA			= 0x0a,
	HCI_GATEWAY_CMD_START_KEYBIND 			= 0x0b,
	HCI_GATEWAY_CMD_GET_PRO_SELF_STS 		= 0x0c,
    HCI_GATEWAY_CMD_SET_DEV_KEY     		= 0x0d,
    HCI_GATEWAY_CMD_GET_SNO         		= 0x0e,
    HCI_GATEWAY_CMD_SET_SNO         		= 0x0f,
    HCI_GATEWAY_CMD_GET_UUID_MAC        	= 0x10,
    HCI_GATEWAY_CMD_DEL_VC_NODE_INFO    	= 0x11,
    HCI_GATEWAY_CMD_SEND_VC_NODE_INFO		= 0x12,
    HCI_GATEWAY_CMD_MESH_OTA_ADR_SEND 		= 0x13, // HCI send back the static oob information 
    HCI_GATEWAY_CMD_MESH_COMMUNICATE_TEST 	= 0x14, // HCI send back the static oob information
    HCI_GATEWAY_CMD_MESH_RX_TEST        	= 0x15,
    HCI_GATEWAY_CMD_SET_EXTEND_ADV_OPTION 	= 0x16,
    HCI_GATEWAY_CMD_FAST_PROV_START			= 0x17,

	HCI_GATEWAY_CMD_RP_MODE_SET		  		= 0x18,	// used in B91 mesh.
	HCI_GATEWAY_CMD_RP_SCAN_START_SET   	= 0x19,	// used in B91 mesh.
	HCI_GATEWAY_CMD_RP_LINK_OPEN		  	= 0x1a,
	HCI_GATEWAY_CMD_RP_START			  	= 0x1b,
	HCI_GATEWAY_CMD_GET_USB_ID			  	= 0x1c, // for B91

	HCI_GATEWAY_CMD_PRIMARY_INFO_GET	  	= 0x1d,
	HCI_GATEWAY_CMD_PRIMARY_INFO_SET	  	= 0x1e,
	HCI_GATEWAY_CMD_PRIMARY_INFO_STATUS	  	= 0x1f,
	
	HCI_GATEWAY_CMD_SEND_NET_KEY    		= 0x20,
	HCI_GATEWAY_CMD_OTS_TX					= 0x21,
	
	// rsp cmd part ----------------------------------
	HCI_GATEWAY_RSP_UNICAST					= 0x80,
	HCI_GATEWAY_RSP_OP_CODE					= 0x81,
	HCI_GATEWAY_KEY_BIND_RSP 				= 0x82,
	HCI_GATEWAY_CMD_STATIC_OOB_RSP 			= 0x87, // HCI send back the static oob information 
	HCI_GATEWAY_CMD_UPDATE_MAC	 			= 0x88,
	HCI_GATEWAY_CMD_PROVISION_EVT 			= 0x89,
	HCI_GATEWAY_CMD_KEY_BIND_EVT 			= 0x8a,
	HCI_GATEWAY_CMD_PRO_STS_RSP 			= 0x8b,
	HCI_GATEWAY_CMD_SEND_ELE_CNT 			= 0x8c,
	HCI_GATEWAY_CMD_SEND_NODE_INFO 			= 0x8d,
	//HCI_GATEWAY_CMD_SEND_CPS_INFO			= 0x8e,
	HCI_GATEWAY_CMD_HEARTBEAT				= 0x8f,

	HCI_GATEWAY_CMD_SECURE_IVI 				= 0x90,
	HCI_GATEWAY_CMD_PRIVATE_BEACON 			= 0x91,
	
	HCI_GATEWAY_CMD_SEND_GATT_OTA_STS		= 0x97, // firmware down to gateway ota area
	HCI_GATEWAY_CMD_SEND_MESH_OTA_STS		= 0x98,
	HCI_GATEWAY_CMD_SEND_UUID   			= 0x99,
	HCI_GATEWAY_CMD_SEND_IVI    			= 0x9a,
	HCI_GATEWAY_CMD_SEND_EXTEND_ADV_OPTION 	= 0x9b,
	HCI_GATEWAY_CMD_SEND_SRC_CMD 			= 0x9c,
	HCI_GATEWAY_CMD_ONLINE_ST				= 0x9d,
	HCI_GATEWAY_CMD_RSP_USB_ID				= 0x9e,
	
	HCI_GATEWAY_CMD_SEND_SNO_RSP    		= 0xa0,
	HCI_GATEWAY_CMD_SEND       				= 0xb1,
	HCI_GATEWAY_DEV_RSP         			= 0xb2,
	HCI_GATEWAY_CMD_LINK_OPEN   			= 0xb3,
	HCI_GATEWAY_CMD_LINK_CLS    			= 0xb4,
	HCI_GATEWAY_CMD_SEND_BACK_VC 			= 0xb5,
	HCI_GATEWAY_CMD_LOG_STRING				= 0xb6,
	HCI_GATEWAY_CMD_LOG_BUF					= 0xb7,
	HCI_GATEWAY_CMD_OTS_RX					= 0xb8,
};
//---------hci_cmd_from_usb
enum{
	// 0xff00 -- 0xff7f for user
	HCI_CMD_USER_START			= 0xFF00,
	HCI_CMD_USER				= HCI_CMD_USER_START,
	HCI_CMD_USER_END			= 0xFF7F,
	HCI_CMD_TSCRIPT				= 0xFFA0,
	HCI_CMD_BULK_CMD2MODEL		= 0xFFA1,
	HCI_CMD_BULK_SET_PAR		= 0xFFA2,			// to VC node
	HCI_CMD_BULK_CMD2DEBUG		= 0xFFA3,
	HCI_CMD_ADV_PKT				= 0xFFA4,			// to bear and GATT
	HCI_CMD_ADV_DEBUG_MESH_DONGLE2BEAR	= 0xFFA5,	// only to bear
	HCI_CMD_ADV_DEBUG_MESH_DONGLE2GATT	= 0xFFA6,	// only to gatt
	HCI_CMD_ADV_DEBUG_MESH_LED	= 0xFFA7,			// control LED
	HCI_CMD_BULK_SET_PAR2USB	= 0xFFA8,
	HCI_CMD_SET_VC_PAR			= 0xFFAA,
	HCI_CMD_MESH_OTA_INITIATOR_START	= 0xFFAB,
	HCI_CMD_MESH_PTS_TEST_CASE_CMD		= 0xFFAC,
	//
	HCI_CMD_KMA_DONGLE_SET_MAC  			= 0xFFE0,
	HCI_CMD_KMA_DONGLE_SPP_TEST_DATA		= 0xFFE1,
	HCI_CMD_KMA_DONGLE_SPP_TEST_STATUS  	= 0xFFE2,
	HCI_CMD_KMA_DONGLE_SET_ADV_REPORT_EN	= 0xFFE3,
	HCI_CMD_USER_DEFINE			= 0xFFE4,
	HCI_CMD_GATEWAY_CMD			= 0xFFE8,
	HCI_CMD_GATEWAY_CTL			= 0xFFE9,
	HCI_CMD_GATEWAY_OTA			= 0xFFEA,
	HCI_CMD_MESH_OTA			= 0xFFEB,
	HCI_CMD_GATEWAY_ONLINE_SET	= 0xFFEC,
	//
	HCI_CMD_PROVISION			= 0xFEA0,
};
enum{
	MESH_OTA_SET_TYPE			= 1,//cmd for the mesh ota ctl part 
	MESH_OTA_ERASE_CTL			= 2,
};
#define HCI_CMD_LEN				(2)

#define HCI_CMD_LOW(cmd)		(cmd & 0xFF)
#define HCI_CMD_HIGH(cmd)		((cmd >> 8) & 0xFF)

enum{
    GATT_OTA_SUCCESS                    = 0,
    GATT_OTA_FW_SIZE_TOO_BIG            = 1,
    GATT_OTA_TIMEOUT_DISCONNECT         = 2,
    GATT_OTA_START                      = 3,
};

enum{
    FLD_OTA_REBOOT_FLAG                 = BIT(0),
    FLD_LOW_BATT_FLG                   	= BIT(1),
    FLD_LOW_BATT_LOOP_FLG             	= BIT(2),	// 0 means check by user_init, 1 means by main loop
    FLD_MESH_OTA_100_FLAG               = BIT(3),	// for LPN: it is gatt connected flag before OTA reboot.
};/*DEEP_ANA_REG0*/


extern u32 flash_sector_mac_address;
extern u32 flash_sector_calibration;

#if AUTO_ADAPT_MAC_ADDR_TO_FLASH_TYPE_EN
void blc_readFlashSize_autoConfigCustomFlashSector(void);
#else
#define blc_readFlashSize_autoConfigCustomFlashSector()     // null
#endif

#ifndef WIN32
#if(__TL_LIB_8258__ || (MCU_CORE_TYPE == MCU_CORE_8258))
#include "stack/ble/blt_config.h"
#elif(MCU_CORE_TYPE == MCU_CORE_8278)
#include "stack/ble_8278/blt_config.h"
#elif(MCU_CORE_TYPE == MCU_CORE_9518)
#include "vendor/common/blt_common.h"
#else

#define RAMCODE_OPTIMIZE_CONN_POWER_NEGLECT_ENABLE			0

typedef struct{
	u8 conn_mark;
	u8 ext_crystal_en;
}misc_para_t;

misc_para_t blt_miscParam;
static inline void blc_app_setExternalCrystalCapEnable(u8  en)
{
	blt_miscParam.ext_crystal_en = en;
}

static inline void blc_app_loadCustomizedParameters(void)
{
	 if(!blt_miscParam.ext_crystal_en)
	 {
		 //customize freq_offset adjust cap value, if not customized, default ana_81 is 0xd0
		 if( (*(unsigned char*) (CUST_CAP_INFO_ADDR)) != 0xff ){
			 //ana_81<4:0> is cap value(0x00 - 0x1f)
			 analog_write(0x81, (analog_read(0x81)&0xe0) | ((*(unsigned char*) (CUST_CAP_INFO_ADDR))&0x1f) );
		 }else if( (*(unsigned char*) (0x76010)) != 0xff ){ // no 1M flash for 8269
			 analog_write(0x81, (analog_read(0x81)&0xe0) | ((*(unsigned char*) (0x76010))&0x1f) );
		 }
	 }


	 // customize TP0/TP1
	 if( ((*(unsigned char*) (CUST_TP_INFO_ADDR)) != 0xff) && ((*(unsigned char*) (CUST_TP_INFO_ADDR+1)) != 0xff) ){
		 rf_update_tp_value(*(unsigned char*) (CUST_TP_INFO_ADDR), *(unsigned char*) (CUST_TP_INFO_ADDR+1));
	 }else if( ((*(unsigned char*) (0x76011)) != 0xff) && ((*(unsigned char*) (0x76011+1)) != 0xff) ){
		 rf_update_tp_value(*(unsigned char*) (0x76011), *(unsigned char*) (0x76011+1)); // no 1M flash for 8269
	 }

	  //customize 32k RC cap, if not customized, default ana_32 is 0x80
	 if( (*(unsigned char*) (CUST_RC32K_CAP_INFO_ADDR)) != 0xff ){
		 //ana_81<4:0> is cap value(0x00 - 0x1f)
		 analog_write(0x32, *(unsigned char*) (CUST_RC32K_CAP_INFO_ADDR) );
	 }
}
#endif
#endif










/////////////////// Code Zise & Feature ////////////////////////////

#if ( __TL_LIB_8261__ || (MCU_CORE_TYPE == MCU_CORE_8261) )
	#define BLE_STACK_SIMPLIFY_4_SMALL_FLASH_ENABLE		1
	#define BLE_CORE42_DATA_LENGTH_EXTENSION_ENABLE		0
#endif


#ifndef BLE_STACK_SIMPLIFY_4_SMALL_FLASH_ENABLE
#define BLE_STACK_SIMPLIFY_4_SMALL_FLASH_ENABLE			0
#endif




//for 8261 128k flash
#if (BLE_STACK_SIMPLIFY_4_SMALL_FLASH_ENABLE)
	#define		BLS_ADV_INTERVAL_CHECK_ENABLE					0
#endif




#ifndef BLE_P256_PUBLIC_KEY_ENABLE
#define BLE_P256_PUBLIC_KEY_ENABLE								0
#endif







#ifndef BLE_CORE42_DATA_LENGTH_EXTENSION_ENABLE
#define BLE_CORE42_DATA_LENGTH_EXTENSION_ENABLE			1
#endif





//default ll_master_multi connection
#ifndef  LL_MASTER_SINGLE_CONNECTION
#define  LL_MASTER_SINGLE_CONNECTION					0
#endif

#ifndef  LL_MASTER_MULTI_CONNECTION
#define  LL_MASTER_MULTI_CONNECTION						0
#endif

//#if (LL_MASTER_SINGLE_CONNECTION )
//	#define  LL_MASTER_MULTI_CONNECTION					0
//#else
//	#define  LL_MASTER_MULTI_CONNECTION					1
//#endif











#if (BLE_MODULE_LIB_ENABLE || BLE_MODULE_APPLICATION_ENABLE)  //for ble module
	#define		BLS_DMA_DATA_LOSS_DETECT_AND_SOLVE_ENABLE		1
	#define		BLS_SEND_TLK_MODULE_EVENT_ENABLE				1
	#define		BLS_ADV_INTERVAL_CHECK_ENABLE					0
#endif



//when rf dma & uart dma work together
#ifndef		BLS_DMA_DATA_LOSS_DETECT_AND_SOLVE_ENABLE
#define		BLS_DMA_DATA_LOSS_DETECT_AND_SOLVE_ENABLE		0
#endif

#ifndef		BLS_SEND_TLK_MODULE_EVENT_ENABLE
#define 	BLS_SEND_TLK_MODULE_EVENT_ENABLE				0
#endif



#ifndef		BLS_ADV_INTERVAL_CHECK_ENABLE
#define		BLS_ADV_INTERVAL_CHECK_ENABLE					1
#endif

#if (0 == __TLSR_RISCV_EN__)
#if LIB_TELINK_MESH_SCAN_MODE_ENABLE
#define		BLS_TELINK_MESH_SCAN_MODE_ENABLE				1
#endif

/////////////////  scan mode config  //////////////////////////
#ifndef		BLS_TELINK_MESH_SCAN_MODE_ENABLE
#define		BLS_TELINK_MESH_SCAN_MODE_ENABLE				0
#endif

#if(BLS_TELINK_MESH_SCAN_MODE_ENABLE)
	#define		BLS_BT_STD_SCAN_MODE_ENABLE					0
#else
	#ifndef		BLS_BT_STD_SCAN_MODE_ENABLE
	#define		BLS_BT_STD_SCAN_MODE_ENABLE					1
	#endif
#endif
#endif



#ifndef BLE_LL_ADV_IN_MAINLOOP_ENABLE
#define BLE_LL_ADV_IN_MAINLOOP_ENABLE					1
#endif

#if FW_START_BY_BOOTLOADER_EN
#define FW_SIZE_MAX_K			    (FW_SIZE_MAX_USER_K)
#elif (DUAL_MESH_ZB_BL_EN || DUAL_MESH_SIG_PVT_EN)
#define FW_SIZE_MAX_K			    (192) // 192  //192K // can set to 256k max for dual mode with 1M flash.
#elif (MESH_USER_DEFINE_MODE == MESH_PIPA_ENABLE)
#define FW_SIZE_MAX_K			    (184)
#elif WIN32
#define FW_SIZE_MAX_K				(1024)
#define NEW_FW_MAX_SIZE     		(FW_SIZE_MAX_K * 1024)
#else
#define FW_SIZE_MAX_K			    (FLASH_ADR_AREA_FIRMWARE_END / 1024) // 192  //192K
#endif

#if __TLSR_RISCV_EN__
#define OTA_CMD_INTER_TIMEOUT_S		1000	// this is total timeout. blc_ota_setOtaDataPacketTimeout_(4s) is packet timeout.
#else
#define OTA_CMD_INTER_TIMEOUT_S		30
#endif
#define OTA_KMADONGLE_PAUSE_TIMEOUT_S   OTA_CMD_INTER_TIMEOUT_S

#ifndef PM_DEEPSLEEP_RETENTION_ENABLE
#define PM_DEEPSLEEP_RETENTION_ENABLE					0
#endif

#ifndef ONLINE_STATUS_EN
#define ONLINE_STATUS_EN					            0
#endif

#ifndef DUAL_MODE_ADAPT_EN
#define DUAL_MODE_ADAPT_EN					            0
#endif
#ifndef DUAL_MODE_WITH_TLK_MESH_EN
#define DUAL_MODE_WITH_TLK_MESH_EN					    0
#endif

#ifndef SWITCH_ALWAYS_MODE_GATT_EN
#define SWITCH_ALWAYS_MODE_GATT_EN						0
#endif

///////////////////////////////////////dbg channels///////////////////////////////////////////
#ifndef	DBG_CHN0_TOGGLE
#define DBG_CHN0_TOGGLE
#endif

#ifndef	DBG_CHN0_HIGH
#define DBG_CHN0_HIGH
#endif

#ifndef	DBG_CHN0_LOW
#define DBG_CHN0_LOW
#endif

#ifndef	DBG_CHN1_TOGGLE
#define DBG_CHN1_TOGGLE
#endif

#ifndef	DBG_CHN1_HIGH
#define DBG_CHN1_HIGH
#endif

#ifndef	DBG_CHN1_LOW
#define DBG_CHN1_LOW
#endif

#ifndef	DBG_CHN2_TOGGLE
#define DBG_CHN2_TOGGLE
#endif

#ifndef	DBG_CHN2_HIGH
#define DBG_CHN2_HIGH
#endif

#ifndef	DBG_CHN2_LOW
#define DBG_CHN2_LOW
#endif

#ifndef	DBG_CHN3_TOGGLE
#define DBG_CHN3_TOGGLE
#endif

#ifndef	DBG_CHN3_HIGH
#define DBG_CHN3_HIGH
#endif

#ifndef	DBG_CHN3_LOW
#define DBG_CHN3_LOW
#endif

#ifndef	DBG_CHN4_TOGGLE
#define DBG_CHN4_TOGGLE
#endif

#ifndef	DBG_CHN4_HIGH
#define DBG_CHN4_HIGH
#endif

#ifndef	DBG_CHN4_LOW
#define DBG_CHN4_LOW
#endif

#ifndef	DBG_CHN5_TOGGLE
#define DBG_CHN5_TOGGLE
#endif

#ifndef	DBG_CHN5_HIGH
#define DBG_CHN5_HIGH
#endif

#ifndef	DBG_CHN5_LOW
#define DBG_CHN5_LOW
#endif


