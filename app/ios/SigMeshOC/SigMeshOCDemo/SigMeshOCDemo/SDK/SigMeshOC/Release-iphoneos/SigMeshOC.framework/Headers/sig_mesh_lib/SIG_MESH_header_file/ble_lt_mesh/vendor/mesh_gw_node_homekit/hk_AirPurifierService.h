/********************************************************************************************************
 * @file     hk_AirPurifierService.h
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
#ifndef _HK_AIR_PURIFIER_SERVICE_H_
#define _HK_AIR_PURIFIER_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_AIR_PURIFIER_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif

#define CHAR_AIR_PURIFIER_SVC_SIGN_ENABLE      0
#if CHAR_AIR_PURIFIER_SVC_SIGN_ENABLE
    #define AIR_PURIFIER_AS_PRIMARY_SERVICE    		0
    #define AIR_PURIFIER_IS_HIDDEN_SERVICE     		0
    #define AIR_PURIFIER_LINKED_SERVICES_ENABLE		0
    EXTERN const hap_characteristic_desc_t hap_desc_char_air_purifier_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_air_purifier_A5_R_S;
    EXTERN unsigned char VAR_AIR_PURIFIER_UNKOWN;
#else
    #define AIR_PURIFIER_AS_PRIMARY_SERVICE    		0
    #define AIR_PURIFIER_IS_HIDDEN_SERVICE     		0
    #define AIR_PURIFIER_LINKED_SERVICES_ENABLE		0
#endif

#define CHAR_AIR_PURIFIER_ROTATION_SPEED_ENABLE			0
#define CHAR_AIR_PURIFIER_SWING_MODE_ENABLE				0
#define CHAR_AIR_PURIFIER_LOCK_PHYSICAL_CONTROLS_ENABLE	0

#define AIR_PURIFIER_ACTIVE_IND_ENABLE                          0
#define AIR_PURIFIER_CURRENT_AIRPURIFIER_STATE_IND_ENABLE       0
#define AIR_PURIFIER_TARGET_AIRPURIFIER_STATE_IND_ENABLE        0
#define AIR_PURIFIER_ROTATION_SPEED_IND_ENABLE        			0
#define AIR_PURIFIER_SWING_MODE_IND_ENABLE        				0
#define AIR_PURIFIER_LOCK_PHYSICAL_CONTROLS_IND_ENABLE        	0

#define AIRPURIFIER_ACTIVE_BRC_ENABLE                       0
#define AIRPURIFIER_CURRENT_AIRPURIFIER_STATE_BRC_ENABLE    0
#define AIRPURIFIER_TARGET_AIRPURIFIER_STATE_BRC_ENABLE     0
#define AIRPURIFIER_ROTATION_SPEED_BRC_ENABLE     			0
#define AIRPURIFIER_SWING_MODE_BRC_ENABLE     				0
#define AIRPURIFIER_LOCK_PHYSICAL_CONTROLS_BRC_ENABLE     	0


#define AIR_PURIFIER_ACTIVE_NO      3
#if AIR_PURIFIER_ACTIVE_IND_ENABLE
    #define AIR_PURIFIER_ACTIVE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hAirPurifierActive), ccc_hAirPurifierActiveWriteCB, 0},
    #define AIR_PURIFIER_ACTIVE_IND_NO  1
#else
    #define AIR_PURIFIER_ACTIVE_IND_DESC
    #define AIR_PURIFIER_ACTIVE_IND_NO  0
#endif

#define AIR_PURIFIER_CURRENT_AIRPURIFIER_STATE_NO      3
#if AIR_PURIFIER_CURRENT_AIRPURIFIER_STATE_IND_ENABLE
    #define AIR_PURIFIER_CURRENT_AIRPURIFIER_STATE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hCurrentAirPurifierState), ccc_hCurrentAirPurifierStateWriteCB, 0},
    #define AIR_PURIFIER_CURRENT_AIRPURIFIER_STATE_IND_NO  1
#else
    #define AIR_PURIFIER_CURRENT_AIRPURIFIER_STATE_IND_DESC
    #define AIR_PURIFIER_CURRENT_AIRPURIFIER_STATE_IND_NO  0
#endif

#define AIR_PURIFIER_TARGET_AIRPURIFIER_STATE_NO      3
#if AIR_PURIFIER_TARGET_AIRPURIFIER_STATE_IND_ENABLE
    #define AIR_PURIFIER_TARGET_AIRPURIFIER_STATE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hTargetAirPurifierState), ccc_hTargetAirPurifierStateWriteCB, 0},
    #define AIR_PURIFIER_TARGET_AIRPURIFIER_STATE_IND_NO  1
#else
    #define AIR_PURIFIER_TARGET_AIRPURIFIER_STATE_IND_DESC
    #define AIR_PURIFIER_TARGET_AIRPURIFIER_STATE_IND_NO  0
#endif

#if CHAR_AIR_PURIFIER_ROTATION_SPEED_ENABLE
    #define AIR_PURIFIER_ROTATION_SPEED_NO 3
    #define CHAR_AIR_PURIFIER_ROTATION_SPEED_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_AirPurifier_RotationSpeed_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof (hAirPurifierRotationSpeed), 0, (u8*)(&charRotationSpeedUUID), (u8*)&hAirPurifierRotationSpeed, AirPurifierRotationSpeedWriteCallback, AirPurifierRotationSpeedReadCallback},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + AIR_PURIFIER_INST_NO + 5), 0, 0}, \
            AIR_PURIFIER_ROTATION_SPEED_IND_DESC

    #if AIR_PURIFIER_ROTATION_SPEED_IND_ENABLE
        #define AIR_PURIFIER_ROTATION_SPEED_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hAirPurifierRotationSpeed), ccc_hAirPurifierRotationSpeedWriteCB, 0},
        #define AIR_PURIFIER_ROTATION_SPEED_IND_NO  1
    #else
        #define AIR_PURIFIER_ROTATION_SPEED_IND_DESC
        #define AIR_PURIFIER_ROTATION_SPEED_IND_NO  0
    #endif
#else
    #define AIR_PURIFIER_ROTATION_SPEED_NO 0
    #define CHAR_AIR_PURIFIER_ROTATION_SPEED_ATT_TABLE
    #define AIR_PURIFIER_ROTATION_SPEED_IND_NO  0
#endif

#if CHAR_AIR_PURIFIER_SWING_MODE_ENABLE
    #define AIR_PURIFIER_SWING_MODE_NO   3
    #define CHAR_AIR_PURIFIER_SWING_MODE_ATT_TABLE   \
            {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_AirPurifier_SwingMode_R_S, 0},    \
            {0,ATT_PERMISSIONS_RDWR,16,sizeof(hAirPurifierSwingMode),0, (u8*)(charSwingModeUUID), (u8*)&hAirPurifierSwingMode, AirPurifierSwingModeWriteCallback, AirPurifierSwingModeReadCallback}, \
                {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + AIR_PURIFIER_INST_NO + 6), 0, 0}, \
                AIR_PURIFIER_SWING_MODE_IND_DESC

    #if AIR_PURIFIER_SWING_MODE_IND_ENABLE
        #define AIR_PURIFIER_SWING_MODE_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hAirPurifierSwingMode), ccc_hAirPurifierSwingModeWriteCB, 0},
        #define AIR_PURIFIER_SWING_MODE_IND_NO  1
    #else
        #define AIR_PURIFIER_SWING_MODE_IND_DESC
        #define AIR_PURIFIER_SWING_MODE_IND_NO  0
    #endif
#else
    #define AIR_PURIFIER_SWING_MODE_NO   0
    #define CHAR_AIR_PURIFIER_SWING_MODE_ATT_TABLE
    #define AIR_PURIFIER_SWING_MODE_IND_NO  0
#endif

#if CHAR_AIR_PURIFIER_LOCK_PHYSICAL_CONTROLS_ENABLE
    #define AIR_PURIFIER_LOCK_PHYSICAL_CONTROLS_NO   3
    #define CHAR_AIR_PURIFIER_LOCK_PHYSICAL_CONTROLS_ATT_TABLE   \
            {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_AirPurifier_LockPhysicalControls_R_S, 0},    \
            {0,ATT_PERMISSIONS_RDWR,16,sizeof(hAirPurifierLockPhysicalControls),0, (u8*)(charLockPhysicalControlsUUID), (u8*)&hAirPurifierLockPhysicalControls, AirPurifierLockPhysicalControlsWriteCallback, AirPurifierLockPhysicalControlsReadCallback}, \
                {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + AIR_PURIFIER_INST_NO + 7), 0, 0}, \
                AIR_PURIFIER_LOCK_PHYSICAL_CONTROLS_IND_DESC

    #if AIR_PURIFIER_LOCK_PHYSICAL_CONTROLS_IND_ENABLE
        #define AIR_PURIFIER_LOCK_PHYSICAL_CONTROLS_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hAirPurifierLockPhysicalControls), ccc_hAirPurifierLockPhysicalControlsWriteCB, 0},
        #define AIR_PURIFIER_LOCK_PHYSICAL_CONTROLS_IND_NO  1
    #else
        #define AIR_PURIFIER_LOCK_PHYSICAL_CONTROLS_IND_DESC
        #define AIR_PURIFIER_LOCK_PHYSICAL_CONTROLS_IND_NO  0
    #endif
#else
    #define AIR_PURIFIER_LOCK_PHYSICAL_CONTROLS_NO   0
    #define CHAR_AIR_PURIFIER_LOCK_PHYSICAL_CONTROLS_ATT_TABLE
    #define AIR_PURIFIER_LOCK_PHYSICAL_CONTROLS_IND_NO  0
#endif


#if CHAR_AIR_PURIFIER_SVC_SIGN_ENABLE
    #define AIR_PURIFIER_SVC_SIGN_NO   3
    #define CHAR_AIR_PURIFIER_SVC_A5_SIGN_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_air_purifier_A5_R_S, &hap_desc_service_air_purifier_A5_R_S},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_AIR_PURIFIER_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_AIR_PURIFIER_UNKOWN, 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + AIR_PURIFIER_INST_NO + 8), 0, 0},
#else
    #define AIR_PURIFIER_SVC_SIGN_NO   0
    #define CHAR_AIR_PURIFIER_SVC_A5_SIGN_TABLE
#endif


#define AIR_PURIFIER_NO_DELTA       AIR_PURIFIER_ACTIVE_IND_NO  \
                                    +AIR_PURIFIER_CURRENT_AIRPURIFIER_STATE_IND_NO  \
                                    +AIR_PURIFIER_TARGET_AIRPURIFIER_STATE_IND_NO \
                                    +AIR_PURIFIER_ROTATION_SPEED_IND_NO \
                                    +AIR_PURIFIER_SWING_MODE_IND_NO \
									+AIR_PURIFIER_LOCK_PHYSICAL_CONTROLS_IND_NO

#define AIR_PURIFIER_NO             15 +AIR_PURIFIER_ROTATION_SPEED_NO \
									+AIR_PURIFIER_SWING_MODE_NO \
									+AIR_PURIFIER_LOCK_PHYSICAL_CONTROLS_NO \
                                    +AIR_PURIFIER_SVC_SIGN_NO

#define AIR_PURIFIER_HANDLE_NUM     (AIR_PURIFIER_NO + AIR_PURIFIER_NO_DELTA)


#define HOMEKIT_AIR_PURIFIER_SERVICE_ATT_TABLE   \
    {AIR_PURIFIER_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(air_purifierServiceUUID),sizeof(air_purifierServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(air_purifierServiceUUID), 0, 0},   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(insts + AIR_PURIFIER_INST_NO), 0, 0},   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_NAME, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(hAirPurifierServiceName), 0, (u8*)(nameUUID), (u8*)(hAirPurifierServiceName), 0, 0},  \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + AIR_PURIFIER_INST_NO + 1), 0, 0}, \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_AirPurifier_Active_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hAirPurifierActive),0, (u8*)(charActiveUUID), (u8*)&hAirPurifierActive, AirPurifierActiveWriteCallback, AirPurifierActiveReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + AIR_PURIFIER_INST_NO + 2), 0, 0},  \
            AIR_PURIFIER_ACTIVE_IND_DESC \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_AirPurifier_CurrentAirPurifierState_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hCurrentAirPurifierState),0, (u8*)(charCurrentAirPurifierStateUUID), (u8*)&hCurrentAirPurifierState, 0, CurrentAirPurifierStateReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + AIR_PURIFIER_INST_NO + 3), 0, 0},  \
            AIR_PURIFIER_CURRENT_AIRPURIFIER_STATE_IND_DESC \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_AirPurifier_TargetAirPurifierState_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hTargetAirPurifierState),0, (u8*)(charTargetAirPurifierStateUUID), (u8*)&hTargetAirPurifierState, TargetAirPurifierStateWriteCallback, TargetAirPurifierStateReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + AIR_PURIFIER_INST_NO + 4), 0, 0},  \
            AIR_PURIFIER_TARGET_AIRPURIFIER_STATE_IND_DESC \
			CHAR_AIR_PURIFIER_ROTATION_SPEED_ATT_TABLE \
			CHAR_AIR_PURIFIER_SWING_MODE_ATT_TABLE \
			CHAR_AIR_PURIFIER_LOCK_PHYSICAL_CONTROLS_ATT_TABLE \
        	CHAR_AIR_PURIFIER_SVC_A5_SIGN_TABLE


#define AIR_PURIFIER_HANDLE_COMMON_NUM     8

#define AIR_PURIFIER_HANDLE_NUM_HAIR_PURIFIER_ACTIVE                        AIR_PURIFIER_HANDLE_START_NUM +AIR_PURIFIER_HANDLE_COMMON_NUM
#define AIR_PURIFIER_HANDLE_NUM_HAIR_PURIFIER_CURRENT_AIRPURIFIER_STATE     AIR_PURIFIER_HANDLE_NUM_HAIR_PURIFIER_ACTIVE +AIR_PURIFIER_ACTIVE_NO +AIR_PURIFIER_ACTIVE_IND_NO
#define AIR_PURIFIER_HANDLE_NUM_HAIR_PURIFIER_TARGET_AIRPURIFIER_STATE      AIR_PURIFIER_HANDLE_NUM_HAIR_PURIFIER_CURRENT_AIRPURIFIER_STATE +AIR_PURIFIER_TARGET_AIRPURIFIER_STATE_NO +AIR_PURIFIER_TARGET_AIRPURIFIER_STATE_IND_NO
#define AIR_PURIFIER_HANDLE_NUM_HAIR_PURIFIER_ROTATIONSPEED    		AIR_PURIFIER_HANDLE_NUM_HAIR_PURIFIER_TARGET_AIRPURIFIER_STATE +AIR_PURIFIER_TARGET_AIRPURIFIER_STATE_NO +AIR_PURIFIER_TARGET_AIRPURIFIER_STATE_IND_NO
#define AIR_PURIFIER_HANDLE_NUM_HAIR_PURIFIER_SWINGMODE    			AIR_PURIFIER_HANDLE_NUM_HAIR_PURIFIER_ROTATIONSPEED +AIR_PURIFIER_ROTATION_SPEED_NO +AIR_PURIFIER_ROTATION_SPEED_IND_NO
#define AIR_PURIFIER_HANDLE_NUM_HAIR_PURIFIER_LOCKPHYSICALCONTROLS	AIR_PURIFIER_HANDLE_NUM_HAIR_PURIFIER_SWINGMODE +AIR_PURIFIER_SWING_MODE_NO +AIR_PURIFIER_SWING_MODE_IND_NO
#define AIR_PURIFIER_HANDLE_NUM_THE_END


#define AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_ACTIVE                            u32 AirPurifierActive:1;    /* 0~1 */
#define AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_ACTIVE_STORE                          STORE_CHAR(AirPurifierActive,hAirPurifierActive)
#define AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_ACTIVE_RESTORE                        RESTORE_CHAR(AirPurifierActive,hAirPurifierActive)

#define AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_CURRENT_AIRPURIFIER_STATE         u32 CurrentAirPurifierState:2;    /* 0~2 */
#define AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_CURRENT_AIRPURIFIER_STATE_STORE       STORE_CHAR(CurrentAirPurifierState,hCurrentAirPurifierState)
#define AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_CURRENT_AIRPURIFIER_STATE_RESTORE     RESTORE_CHAR(CurrentAirPurifierState,hCurrentAirPurifierState)

#define AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_TARGET_AIRPURIFIER_STATE          u32 TargetAirPurifierState:1;    /* 0~1 */
#define AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_TARGET_AIRPURIFIER_STATE_STORE        STORE_CHAR(TargetAirPurifierState,hTargetAirPurifierState)
#define AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_TARGET_AIRPURIFIER_STATE_RESTORE      RESTORE_CHAR(TargetAirPurifierState,hTargetAirPurifierState)

#define AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_ROTATION_SPEED             		u32 AirPurifierRotationSpeed:1;    /* float */
#define AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_ROTATION_SPEED_STORE           		STORE_CHAR(AirPurifierRotationSpeed,hAirPurifierRotationSpeed)
#define AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_ROTATION_SPEED_RESTORE         		RESTORE_CHAR(AirPurifierRotationSpeed,hAirPurifierRotationSpeed)

#define AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_SWING_MODE                 		u32 AirPurifierSwingMode:1;    /* 0~1 */
#define AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_SWING_MODE_STORE               		STORE_CHAR(AirPurifierSwingMode,hAirPurifierSwingMode)
#define AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_SWING_MODE_RESTORE             		RESTORE_CHAR(AirPurifierSwingMode,hAirPurifierSwingMode)

#define AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_LOCK_PHYSICAL_CONTROLS      		u32 AirPurifierLockPhysicalControls:1;    /* 0~1 */
#define AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_LOCK_PHYSICAL_CONTROLS_STORE  		STORE_CHAR(AirPurifierLockPhysicalControls,hAirPurifierLockPhysicalControls)
#define AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_LOCK_PHYSICAL_CONTROLS_RESTORE  		RESTORE_CHAR(AirPurifierLockPhysicalControls,hAirPurifierLockPhysicalControls)


#define AIR_PURIFIER_STORE_CHAR_VALUE_T  \
                    AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_ACTIVE  \
                    AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_CURRENT_AIRPURIFIER_STATE  \
                    AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_TARGET_AIRPURIFIER_STATE \
                    AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_ROTATION_SPEED \
                    AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_SWING_MODE \
                    AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_LOCK_PHYSICAL_CONTROLS
                    

#define AIR_PURIFIER_STORE_CHAR_TO_FLASH  \
                    AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_ACTIVE_STORE  \
                    AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_CURRENT_AIRPURIFIER_STATE_STORE  \
                    AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_TARGET_AIRPURIFIER_STATE_STORE \
                    AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_ROTATION_SPEED_STORE \
                    AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_SWING_MODE_STORE \
                    AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_LOCK_PHYSICAL_CONTROLS_STORE

#define AIR_PURIFIER_RESTORE_CHAR_FROM_FLASH  \
                    AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_ACTIVE_RESTORE  \
                    AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_CURRENT_AIRPURIFIER_STATE_RESTORE  \
                    AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_TARGET_AIRPURIFIER_STATE_RESTORE \
                    AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_ROTATION_SPEED_RESTORE \
                    AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_SWING_MODE_RESTORE \
                    AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_LOCK_PHYSICAL_CONTROLS_RESTORE
                    


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN unsigned char hAirPurifierActive;
EXTERN unsigned char hCurrentAirPurifierState;
EXTERN unsigned char hTargetAirPurifierState;
EXTERN int hAirPurifierRotationSpeed;
EXTERN unsigned char hAirPurifierSwingMode;
EXTERN unsigned char hAirPurifierLockPhysicalControls;

EXTERN unsigned short ccc_hAirPurifierActive;
EXTERN unsigned short ccc_hCurrentAirPurifierState;
EXTERN unsigned short ccc_hTargetAirPurifierState;
EXTERN unsigned short ccc_hAirPurifierRotationSpeed;
EXTERN unsigned short ccc_hAirPurifierSwingMode;
EXTERN unsigned short ccc_hAirPurifierLockPhysicalControls;

#if AIRPURIFIER_ACTIVE_BRC_ENABLE
    #define BC_PARA_AIRPURIFIER_ACTIVE ((u8*)&bc_para_hAirPurifierActive)
    EXTERN unsigned char bc_para_hAirPurifierActive;
#else
    #define BC_PARA_AIRPURIFIER_ACTIVE 0
#endif

#if AIRPURIFIER_CURRENT_AIRPURIFIER_STATE_BRC_ENABLE
    #define BC_PARA_AIRPURIFIER_CURRENT_AIRPURIFIER_STATE ((u8*)&bc_para_hCurrentAirPurifierState)
    EXTERN unsigned char bc_para_hCurrentAirPurifierState;
#else
    #define BC_PARA_AIRPURIFIER_CURRENT_AIRPURIFIER_STATE 0
#endif

#if AIRPURIFIER_TARGET_AIRPURIFIER_STATE_BRC_ENABLE
    #define BC_PARA_AIRPURIFIER_TARGET_AIRPURIFIER_STATE ((u8*)&bc_para_hTargetAirPurifierState)
    EXTERN unsigned char bc_para_hTargetAirPurifierState;
#else
    #define BC_PARA_AIRPURIFIER_TARGET_AIRPURIFIER_STATE 0
#endif

#if AIRPURIFIER_ROTATION_SPEED_BRC_ENABLE
    #define BC_PARA_AIRPURIFIER_ROTATION_SPEED ((u8*)&bc_para_hAirPurifierRotationSpeed)
    EXTERN unsigned char bc_para_hAirPurifierRotationSpeed;
#else
    #define BC_PARA_AIRPURIFIER_ROTATION_SPEED 0
#endif

#if AIRPURIFIER_SWING_MODE_BRC_ENABLE
    #define BC_PARA_AIRPURIFIER_SWING_MODE ((u8*)&bc_para_hAirPurifierSwingMode)
    EXTERN unsigned char bc_para_hAirPurifierSwingMode;
#else
    #define BC_PARA_AIRPURIFIER_SWING_MODE 0
#endif

#if AIRPURIFIER_LOCK_PHYSICAL_CONTROLS_BRC_ENABLE
    #define BC_PARA_AIRPURIFIER_LOCK_PHYSICAL_CONTROLS ((u8*)&bc_para_hAirPurifierLockPhysicalControls)
    EXTERN unsigned char bc_para_hAirPurifierLockPhysicalControls;
#else
    #define BC_PARA_AIRPURIFIER_LOCK_PHYSICAL_CONTROLS 0
#endif

#if AIR_PURIFIER_LINKED_SERVICES_ENABLE
	#define AIR_PURIFIER_LINKED_SVCS			hAirPurifier_linked_svcs
	#define AIR_PURIFIER_LINKED_SVCS_LENGTH 	(sizeof(hAirPurifier_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * hAirPurifier_linked_svcs[];
#else
	#define AIR_PURIFIER_LINKED_SVCS			0
	#define AIR_PURIFIER_LINKED_SVCS_LENGTH 	0
#endif

EXTERN const u8 hAirPurifierServiceName[18];

EXTERN const hap_characteristic_desc_t hap_desc_char_AirPurifier_Active_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_AirPurifier_CurrentAirPurifierState_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_AirPurifier_TargetAirPurifierState_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_AirPurifier_RotationSpeed_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_AirPurifier_SwingMode_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_AirPurifier_LockPhysicalControls_R_S;


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN int AirPurifierActiveReadCallback(void *pp);
EXTERN int AirPurifierActiveWriteCallback(void *pp);
EXTERN int CurrentAirPurifierStateReadCallback(void *pp);
EXTERN int TargetAirPurifierStateReadCallback(void *pp);
EXTERN int TargetAirPurifierStateWriteCallback(void *pp);
EXTERN int AirPurifierRotationSpeedReadCallback(void *pp);
EXTERN int AirPurifierRotationSpeedWriteCallback(void *pp);
EXTERN int AirPurifierSwingModeReadCallback(void *pp);
EXTERN int AirPurifierSwingModeWriteCallback(void *pp);
EXTERN int AirPurifierLockPhysicalControlsReadCallback(void *pp);
EXTERN int AirPurifierLockPhysicalControlsWriteCallback(void *pp);

EXTERN int ccc_hAirPurifierActiveWriteCB(void *pp);
EXTERN int ccc_hCurrentAirPurifierStateWriteCB(void *pp);
EXTERN int ccc_hTargetAirPurifierStateWriteCB(void *pp);
EXTERN int ccc_hAirPurifierRotationSpeedWriteCB(void *pp);
EXTERN int ccc_hAirPurifierSwingModeWriteCB(void *pp);
EXTERN int ccc_hAirPurifierLockPhysicalControlsWriteCB(void *pp);


#endif /* _HK_AIR_PURIFIER_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
