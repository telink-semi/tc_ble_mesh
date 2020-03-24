/********************************************************************************************************
 * @file     hk_BatteryService.h
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2016, Telink Semiconductor (Shanghai) Co., Ltd.
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
#ifndef _HK_BATTERY_SERVICE_H_
#define _HK_BATTERY_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_BATTERY_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif


#define CHAR_BATTERY_SVC_SIGN_ENABLE      0
#if CHAR_BATTERY_SVC_SIGN_ENABLE
    #define BATTERY_AS_PRIMARY_SERVICE    	0
    #define BATTERY_IS_HIDDEN_SERVICE     	0
    #define BATTERY_LINKED_SERVICES_ENABLE	0
    EXTERN const hap_characteristic_desc_t hap_desc_char_battery_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_battery_A5_R_S;
    EXTERN unsigned char VAR_BATTERY_UNKOWN;
#else
    #define BATTERY_AS_PRIMARY_SERVICE    	0
    #define BATTERY_IS_HIDDEN_SERVICE     	0
    #define BATTERY_LINKED_SERVICES_ENABLE	0
#endif


#define BATTERY_LEVEL_IND_ENABLE            0
#define BATTERY_CHARGINGSTATE_IND_ENABLE    0
#define BATTERY_STATELOWBATTERY_IND_ENABLE  0

#define BATTERY_LEVEL_BRC_ENABLE            0
#define BATTERY_CHARGINGSTATE_BRC_ENABLE    0
#define BATTERY_STATELOWBATTERY_BRC_ENABLE  0


#define BATTERY_LEVEL_NO 3
#if BATTERY_LEVEL_IND_ENABLE
    #define BATTERY_LEVEL_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hBatteryLevel), ccc_hBatteryLevelWriteCB, 0},
    #define BATTERY_LEVEL_IND_NO  1
#else
    #define BATTERY_LEVEL_IND_DESC
    #define BATTERY_LEVEL_IND_NO  0
#endif

#define BATTERY_CHARGINGSTATE_NO 3
#if BATTERY_CHARGINGSTATE_IND_ENABLE
    #define BATTERY_CHARGINGSTATE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hBatteryChargingState), ccc_hBatteryChargingStateWriteCB, 0},
    #define BATTERY_CHARGINGSTATE_IND_NO  1
#else
    #define BATTERY_CHARGINGSTATE_IND_DESC
    #define BATTERY_CHARGINGSTATE_IND_NO  0
#endif

#define BATTERY_STATUSLOWBATTERY_NO 3
#if BATTERY_STATELOWBATTERY_IND_ENABLE
    #define BATTERY_STATELOWBATTERY_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hBatteryStateLowBattery), ccc_hBatteryStateLowBatteryWriteCB, 0},
    #define BATTERY_STATELOWBATTERY_IND_NO  1
#else
    #define BATTERY_STATELOWBATTERY_IND_DESC
    #define BATTERY_STATELOWBATTERY_IND_NO  0
#endif

#if CHAR_BATTERY_SVC_SIGN_ENABLE
#define CHAR_BATTERY_SVC_A5_SIGN_TABLE   \
    {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_battery_A5_R_S, &hap_desc_service_battery_A5_R_S},  \
    {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_BATTERY_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_BATTERY_UNKOWN, 0, 0},    \
        {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + BATTERY_INST_NO + 5), 0, 0},
#define BATTERY_SVC_SIGN_NO   3
#else
#define CHAR_BATTERY_SVC_A5_SIGN_TABLE
#define BATTERY_SVC_SIGN_NO   0
#endif

#define BATTERY_NO_DELTA    BATTERY_LEVEL_IND_NO   \
                            +BATTERY_CHARGINGSTATE_IND_NO   \
                            +BATTERY_STATELOWBATTERY_IND_NO

#define BATTERY_NO          15 +BATTERY_SVC_SIGN_NO
#define BATTERY_HANDLE_NUM (BATTERY_NO + BATTERY_NO_DELTA)

#define HOMEKIT_BATTERY_SERVICE_ATT_TABLE	\
    {BATTERY_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(batteryServiceUUID),sizeof(batteryServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(batteryServiceUUID), 0, 0},   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(insts + BATTERY_INST_NO), 0, 0},   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_NAME, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(hBatteryServiceName), 0, (u8*)(nameUUID), (u8*)(hBatteryServiceName), 0, 0},  \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + BATTERY_INST_NO + 1), 0, 0}, \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hBatteryLevel_R_S, 0},   \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hBatteryLevel),0, (u8*)(charBatteryLevelUUID), (u8*)&hBatteryLevel, 0, hBatteryLevelReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + BATTERY_INST_NO + 2), 0, 0},  \
            BATTERY_LEVEL_IND_DESC \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hBatteryChargingState_R_S, 0},   \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hBatteryChargingState),0, (u8*)(charChargingStateUUID), (u8*)&hBatteryChargingState, 0, hBatteryChargingStateReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + BATTERY_INST_NO + 3), 0, 0},  \
            BATTERY_CHARGINGSTATE_IND_DESC \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hBatteryStateLowBattery_R_S, 0}, \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hBatteryStateLowBattery),0, (u8*)(charStatusLowBatteryUUID), (u8*)&hBatteryStateLowBattery, 0, hBatteryStateLowBatteryReadCallback},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + BATTERY_INST_NO + 4), 0, 0},  \
            BATTERY_STATELOWBATTERY_IND_DESC \
        CHAR_BATTERY_SVC_A5_SIGN_TABLE


#define BATTERY_HANDLE_COMMON_NUM     8

#define BATTERY_HANDLE_NUM_LEVEL            BATTERY_HANDLE_START_NUM +BATTERY_HANDLE_COMMON_NUM
#define BATTERY_HANDLE_NUM_CHARGINGSTATE    BATTERY_HANDLE_NUM_LEVEL +BATTERY_LEVEL_NO +BATTERY_LEVEL_IND_NO
#define BATTERY_HANDLE_NUM_STATUSLOWBATTERY BATTERY_HANDLE_NUM_CHARGINGSTATE +BATTERY_CHARGINGSTATE_NO +BATTERY_CHARGINGSTATE_IND_NO
#define BATTERY_HANDLE_NUM_THE_END


#define BATTERY_STORE_CHAR_VALUE_T_BATTERYLEVEL                     u32 BatteryLevel:7; /* 0~100 */
#define BATTERY_STORE_CHAR_VALUE_T_BATTERYLEVEL_STORE                   STORE_CHAR(BatteryLevel,hBatteryLevel)
#define BATTERY_STORE_CHAR_VALUE_T_BATTERYLEVEL_RESTORE                 RESTORE_CHAR(BatteryLevel,hBatteryLevel)

#define BATTERY_STORE_CHAR_VALUE_T_BATTERYCHARGINGSTATE             u32 BatteryChargingState:2; /* 0~2 */
#define BATTERY_STORE_CHAR_VALUE_T_BATTERYCHARGINGSTATE_STORE           STORE_CHAR(BatteryChargingState,hBatteryChargingState)
#define BATTERY_STORE_CHAR_VALUE_T_BATTERYCHARGINGSTATE_RESTORE         RESTORE_CHAR(BatteryChargingState,hBatteryChargingState)

#define BATTERY_STORE_CHAR_VALUE_T_BATTERYSTATELOWBATTERY           u32 BatteryStateLowBattery:1; /* 0~1 */
#define BATTERY_STORE_CHAR_VALUE_T_BATTERYSTATELOWBATTERY_STORE         STORE_CHAR(BatteryStateLowBattery,hBatteryStateLowBattery)
#define BATTERY_STORE_CHAR_VALUE_T_BATTERYSTATELOWBATTERY_RESTORE       RESTORE_CHAR(BatteryStateLowBattery,hBatteryStateLowBattery)

#define BATTERY_STORE_CHAR_VALUE_T   \
                    BATTERY_STORE_CHAR_VALUE_T_BATTERYLEVEL  \
                    BATTERY_STORE_CHAR_VALUE_T_BATTERYCHARGINGSTATE   \
                    BATTERY_STORE_CHAR_VALUE_T_BATTERYSTATELOWBATTERY

#define BATTERY_STORE_CHAR_TO_FLASH  \
                    BATTERY_STORE_CHAR_VALUE_T_BATTERYLEVEL_STORE    \
                    BATTERY_STORE_CHAR_VALUE_T_BATTERYCHARGINGSTATE_STORE \
                    BATTERY_STORE_CHAR_VALUE_T_BATTERYSTATELOWBATTERY_STORE

#define BATTERY_RESTORE_CHAR_FROM_FLASH  \
                    BATTERY_STORE_CHAR_VALUE_T_BATTERYLEVEL_RESTORE  \
                    BATTERY_STORE_CHAR_VALUE_T_BATTERYCHARGINGSTATE_RESTORE   \
                    BATTERY_STORE_CHAR_VALUE_T_BATTERYSTATELOWBATTERY_RESTORE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN unsigned char hBatteryLevel;
EXTERN unsigned char hBatteryChargingState;
EXTERN unsigned char hBatteryStateLowBattery;

EXTERN unsigned short ccc_hBatteryLevel;
EXTERN unsigned short ccc_hBatteryChargingState;
EXTERN unsigned short ccc_hBatteryStateLowBattery;

#if BATTERY_LEVEL_BRC_ENABLE
    #define BC_PARA_BATTERY_LEVEL ((u8*)&bc_para_hBatteryLevel)
    EXTERN unsigned char bc_para_hBatteryLevel;
#else
    #define BC_PARA_BATTERY_LEVEL 0
#endif

#if BATTERY_CHARGINGSTATE_BRC_ENABLE
    #define BC_PARA_BATTERY_CHARGINGSTATE ((u8*)&bc_para_hBatteryChargingState)
    EXTERN unsigned char bc_para_hBatteryChargingState;
#else
    #define BC_PARA_BATTERY_CHARGINGSTATE 0
#endif

#if BATTERY_STATELOWBATTERY_BRC_ENABLE
    #define BC_PARA_BATTERY_STATELOWBATTERY ((u8*)&bc_para_hBatteryStateLowBattery)
    EXTERN unsigned char bc_para_hBatteryStateLowBattery;
#else
    #define BC_PARA_BATTERY_STATELOWBATTERY 0
#endif

#if BATTERY_LINKED_SERVICES_ENABLE
	#define BATTERY_LINKED_SVCS			hBattery_linked_svcs
	#define BATTERY_LINKED_SVCS_LENGTH 	(sizeof(hBattery_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * hBattery_linked_svcs[];
#else
	#define BATTERY_LINKED_SVCS			0
	#define BATTERY_LINKED_SVCS_LENGTH 	0
#endif

EXTERN const u8 hBatteryServiceName[14];

EXTERN const hap_characteristic_desc_t hap_desc_char_hBatteryLevel_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hBatteryChargingState_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hBatteryStateLowBattery_R_S;

/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN int hBatteryLevelReadCallback(void *pp);
EXTERN int hBatteryChargingStateReadCallback(void *pp);
EXTERN int hBatteryStateLowBatteryReadCallback(void *pp);
EXTERN int ccc_hBatteryLevelWriteCB(void *pp);
EXTERN int ccc_hBatteryChargingStateWriteCB(void *pp);
EXTERN int ccc_hBatteryStateLowBatteryWriteCB(void *pp);

#endif /* _HK_BATTERY_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
