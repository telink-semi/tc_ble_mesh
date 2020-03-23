/********************************************************************************************************
 * @file     hk_FanV2Service.h
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
#ifndef _HK_FAN_V2_SERVICE_H_
#define _HK_FAN_V2_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_FAN_V2_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif

#define CHAR_FAN_V2_SVC_SIGN_ENABLE      0
#if CHAR_FAN_V2_SVC_SIGN_ENABLE
    #define FAN_V2_AS_PRIMARY_SERVICE    	0
    #define FAN_V2_IS_HIDDEN_SERVICE     	0
    #define FAN_V2_LINKED_SERVICES_ENABLE	0
    EXTERN const hap_characteristic_desc_t hap_desc_char_fan_v2_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_fan_v2_A5_R_S;
    EXTERN unsigned char VAR_FAN_V2_UNKOWN;
#else
    #define FAN_V2_AS_PRIMARY_SERVICE    	0
    #define FAN_V2_IS_HIDDEN_SERVICE     	0
    #define FAN_V2_LINKED_SERVICES_ENABLE	0
#endif


#define CHAR_FAN_V2_CURRENT_FAN_STATE_ENABLE    	0
#define CHAR_FAN_V2_TARGET_FAN_STATE_ENABLE     	0
#define CHAR_FAN_V2_ROTATION_DIRECTION_ENABLE		0
#define CHAR_FAN_V2_ROTATION_SPEED_ENABLE			0
#define CHAR_FAN_V2_SWING_MODE_ENABLE				0
#define CHAR_FAN_V2_LOCK_PHYSICAL_CONTROLS_ENABLE	0

#define FAN_V2_ACTIVE_IND_ENABLE                	0
#define FAN_V2_CURRENT_FAN_STATE_IND_ENABLE     	0
#define FAN_V2_TARGET_FAN_STATE_IND_ENABLE      	0
#define FAN_V2_ROTATION_DIRECTION_IND_ENABLE		0
#define FAN_V2_ROTATION_SPEED_IND_ENABLE			0
#define FAN_V2_SWING_MODE_IND_ENABLE				0
#define FAN_V2_LOCK_PHYSICAL_CONTROLS_IND_ENABLE	0

#define FAN_V2_ACTIVE_BRC_ENABLE                	0
#define FAN_V2_CURRENT_FAN_STATE_BRC_ENABLE     	0
#define FAN_V2_TARGET_FAN_STATE_BRC_ENABLE      	0
#define FAN_V2_ROTATION_DIRECTION_BRC_ENABLE		0
#define FAN_V2_ROTATION_SPEED_BRC_ENABLE			0
#define FAN_V2_SWING_MODE_BRC_ENABLE				0
#define FAN_V2_LOCK_PHYSICAL_CONTROLS_BRC_ENABLE	0


#define FAN_V2_ACTIVE_NO      3
#if FAN_V2_ACTIVE_IND_ENABLE
    #define FAN_V2_ACTIVE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hFanV2Active), ccc_hFanV2ActiveWriteCB, 0},
    #define FAN_V2_ACTIVE_IND_NO  1
#else
    #define FAN_V2_ACTIVE_IND_DESC
    #define FAN_V2_ACTIVE_IND_NO  0
#endif

#if CHAR_FAN_V2_CURRENT_FAN_STATE_ENABLE
    #define FAN_V2_CURRENT_FAN_STATE_NO   3
    #define CHAR_FAN_V2_CURRENT_FAN_STATE_ATT_TABLE   \
            {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_FanV2_CurrentFanState_R_S, 0},    \
            {0,ATT_PERMISSIONS_RDWR,16,sizeof(hFanV2CurrentFanState),0, (u8*)(charCurrentFanStateUUID), (u8*)&hFanV2CurrentFanState, 0, FanV2CurrentFanStateReadCallback}, \
                {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + FAN_V2_INST_NO + 3), 0, 0}, \
                FAN_V2_CURRENT_FAN_STATE_IND_DESC

    #if FAN_V2_CURRENT_FAN_STATE_IND_ENABLE
        #define FAN_V2_CURRENT_FAN_STATE_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hFanV2CurrentFanState), ccc_hFanV2CurrentFanStateWriteCB, 0},
        #define FAN_V2_CURRENT_FAN_STATE_IND_NO  1
    #else
        #define FAN_V2_CURRENT_FAN_STATE_IND_DESC
        #define FAN_V2_CURRENT_FAN_STATE_IND_NO  0
    #endif
#else
    #define FAN_V2_CURRENT_FAN_STATE_NO   0
    #define CHAR_FAN_V2_CURRENT_FAN_STATE_ATT_TABLE
    #define FAN_V2_CURRENT_FAN_STATE_IND_NO  0
#endif

#if CHAR_FAN_V2_TARGET_FAN_STATE_ENABLE
    #define FAN_V2_TARGET_FAN_STATE_NO   3
    #define CHAR_FAN_V2_TARGET_FAN_STATE_ATT_TABLE   \
            {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_FanV2_TargetFanState_R_S, 0},    \
            {0,ATT_PERMISSIONS_RDWR,16,sizeof(hFanV2TargetFanState),0, (u8*)(charTargetFanStateUUID), (u8*)&hFanV2TargetFanState, FanV2TargetFanStateWriteCallback, FanV2TargetFanStateReadCallback}, \
                {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + FAN_V2_INST_NO + 4), 0, 0}, \
                FAN_V2_TARGET_FAN_STATE_IND_DESC

    #if FAN_V2_TARGET_FAN_STATE_IND_ENABLE
        #define FAN_V2_TARGET_FAN_STATE_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hFanV2TargetFanState), ccc_hFanV2TargetFanStateWriteCB, 0},
        #define FAN_V2_TARGET_FAN_STATE_IND_NO  1
    #else
        #define FAN_V2_TARGET_FAN_STATE_IND_DESC
        #define FAN_V2_TARGET_FAN_STATE_IND_NO  0
    #endif
#else
    #define FAN_V2_TARGET_FAN_STATE_NO   0
    #define CHAR_FAN_V2_TARGET_FAN_STATE_ATT_TABLE
    #define FAN_V2_TARGET_FAN_STATE_IND_NO  0
#endif

#if CHAR_FAN_V2_ROTATION_DIRECTION_ENABLE
    #define FAN_V2_ROTATION_DIRECTION_NO 3
    #define CHAR_FAN_V2_ROTATION_DIRECTION_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_FanV2_RotationDirection_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof (hFanV2RotationDirection), 0, (u8*)(&charRotationDirectionUUID), (u8*)&hFanV2RotationDirection, FanV2RotationDirectionWriteCallback, FanV2RotationDirectionReadCallback},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + FAN_V2_INST_NO + 5), 0, 0}, \
            FAN_V2_ROTATION_DIRECTION_IND_DESC

    #if FAN_V2_ROTATION_DIRECTION_IND_ENABLE
        #define FAN_V2_ROTATION_DIRECTION_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hFanV2RotationDirection), ccc_hFanV2RotationDirectionWriteCB, 0},
        #define FAN_V2_ROTATION_DIRECTION_IND_NO  1
    #else
        #define FAN_V2_ROTATION_DIRECTION_IND_DESC
        #define FAN_V2_ROTATION_DIRECTION_IND_NO  0
    #endif
#else
    #define FAN_V2_ROTATION_DIRECTION_NO 0
    #define CHAR_FAN_V2_ROTATION_DIRECTION_ATT_TABLE
    #define FAN_V2_ROTATION_DIRECTION_IND_NO  0
#endif

#if CHAR_FAN_V2_ROTATION_SPEED_ENABLE
    #define FAN_V2_ROTATION_SPEED_NO 3
    #define CHAR_FAN_V2_ROTATION_SPEED_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_FanV2_RotationSpeed_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof (hFanV2RotationSpeed), 0, (u8*)(&charRotationSpeedUUID), (u8*)&hFanV2RotationSpeed, FanV2RotationSpeedWriteCallback, FanV2RotationSpeedReadCallback},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + FAN_V2_INST_NO + 6), 0, 0}, \
            FAN_V2_ROTATION_SPEED_IND_DESC

    #if FAN_V2_ROTATION_SPEED_IND_ENABLE
        #define FAN_V2_ROTATION_SPEED_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hFanV2RotationSpeed), ccc_hFanV2RotationSpeedWriteCB, 0},
        #define FAN_V2_ROTATION_SPEED_IND_NO  1
    #else
        #define FAN_V2_ROTATION_SPEED_IND_DESC
        #define FAN_V2_ROTATION_SPEED_IND_NO  0
    #endif
#else
    #define FAN_V2_ROTATION_SPEED_NO 0
    #define CHAR_FAN_V2_ROTATION_SPEED_ATT_TABLE
    #define FAN_V2_ROTATION_SPEED_IND_NO  0
#endif

#if CHAR_FAN_V2_SWING_MODE_ENABLE
    #define FAN_V2_SWING_MODE_NO   3
    #define CHAR_FAN_V2_SWING_MODE_ATT_TABLE   \
            {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_FanV2_SwingMode_R_S, 0},    \
            {0,ATT_PERMISSIONS_RDWR,16,sizeof(hFanV2SwingMode),0, (u8*)(charSwingModeUUID), (u8*)&hFanV2SwingMode, FanV2SwingModeWriteCallback, FanV2SwingModeReadCallback}, \
                {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + FAN_V2_INST_NO + 7), 0, 0}, \
                FAN_V2_SWING_MODE_IND_DESC

    #if FAN_V2_SWING_MODE_IND_ENABLE
        #define FAN_V2_SWING_MODE_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hFanV2SwingMode), ccc_hFanV2SwingModeWriteCB, 0},
        #define FAN_V2_SWING_MODE_IND_NO  1
    #else
        #define FAN_V2_SWING_MODE_IND_DESC
        #define FAN_V2_SWING_MODE_IND_NO  0
    #endif
#else
    #define FAN_V2_SWING_MODE_NO   0
    #define CHAR_FAN_V2_SWING_MODE_ATT_TABLE
    #define FAN_V2_SWING_MODE_IND_NO  0
#endif

#if CHAR_FAN_V2_LOCK_PHYSICAL_CONTROLS_ENABLE
    #define FAN_V2_LOCK_PHYSICAL_CONTROLS_NO   3
    #define CHAR_FAN_V2_LOCK_PHYSICAL_CONTROLS_ATT_TABLE   \
            {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_FanV2_LockPhysicalControls_R_S, 0},    \
            {0,ATT_PERMISSIONS_RDWR,16,sizeof(hFanV2LockPhysicalControls),0, (u8*)(charLockPhysicalControlsUUID), (u8*)&hFanV2LockPhysicalControls, FanV2LockPhysicalControlsWriteCallback, FanV2LockPhysicalControlsReadCallback}, \
                {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + FAN_V2_INST_NO + 8), 0, 0}, \
                FAN_V2_LOCK_PHYSICAL_CONTROLS_IND_DESC

    #if FAN_V2_LOCK_PHYSICAL_CONTROLS_IND_ENABLE
        #define FAN_V2_LOCK_PHYSICAL_CONTROLS_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hFanV2LockPhysicalControls), ccc_hFanV2LockPhysicalControlsWriteCB, 0},
        #define FAN_V2_LOCK_PHYSICAL_CONTROLS_IND_NO  1
    #else
        #define FAN_V2_LOCK_PHYSICAL_CONTROLS_IND_DESC
        #define FAN_V2_LOCK_PHYSICAL_CONTROLS_IND_NO  0
    #endif
#else
    #define FAN_V2_LOCK_PHYSICAL_CONTROLS_NO   0
    #define CHAR_FAN_V2_LOCK_PHYSICAL_CONTROLS_ATT_TABLE
    #define FAN_V2_LOCK_PHYSICAL_CONTROLS_IND_NO  0
#endif


#if CHAR_FAN_V2_SVC_SIGN_ENABLE
    #define FAN_V2_SVC_SIGN_NO   3
    #define CHAR_FAN_V2_SVC_A5_SIGN_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_fan_v2_A5_R_S, &hap_desc_service_fan_v2_A5_R_S},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_FAN_V2_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_FAN_V2_UNKOWN, 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + FAN_V2_INST_NO + 9), 0, 0},
#else
    #define FAN_V2_SVC_SIGN_NO   0
    #define CHAR_FAN_V2_SVC_A5_SIGN_TABLE
#endif

#define FAN_V2_NO_DELTA    FAN_V2_ACTIVE_IND_NO + FAN_V2_CURRENT_FAN_STATE_IND_NO + FAN_V2_TARGET_FAN_STATE_IND_NO \
						+ FAN_V2_ROTATION_DIRECTION_IND_NO + FAN_V2_ROTATION_SPEED_IND_NO + FAN_V2_SWING_MODE_IND_NO \
						+ FAN_V2_LOCK_PHYSICAL_CONTROLS_IND_NO
						
#define FAN_V2_NO          9 + FAN_V2_CURRENT_FAN_STATE_NO + FAN_V2_TARGET_FAN_STATE_NO +  FAN_V2_ROTATION_DIRECTION_NO \
						+ FAN_V2_ROTATION_SPEED_NO + FAN_V2_SWING_MODE_NO + FAN_V2_LOCK_PHYSICAL_CONTROLS_NO \
						+ FAN_V2_SVC_SIGN_NO
						
#define FAN_V2_HANDLE_NUM (FAN_V2_NO + FAN_V2_NO_DELTA)


#define HOMEKIT_FAN_V2_SERVICE_ATT_TABLE   \
    {FAN_V2_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(fan_v2ServiceUUID),sizeof(fan_v2ServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(fan_v2ServiceUUID), 0, 0},   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(insts + FAN_V2_INST_NO), 0, 0},   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_NAME, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(hFanV2ServiceName), 0, (u8*)(nameUUID), (u8*)(hFanV2ServiceName), 0, 0},  \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + FAN_V2_INST_NO + 1), 0, 0}, \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_FanV2_Active_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,1,0, (u8*)(charActiveUUID), (u8*)&hFanV2Active, FanV2ActiveWriteCallback, FanV2ActiveReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + FAN_V2_INST_NO + 2), 0, 0},  \
            FAN_V2_ACTIVE_IND_DESC \
        CHAR_FAN_V2_CURRENT_FAN_STATE_ATT_TABLE \
        CHAR_FAN_V2_TARGET_FAN_STATE_ATT_TABLE \
        CHAR_FAN_V2_ROTATION_DIRECTION_ATT_TABLE \
        CHAR_FAN_V2_ROTATION_SPEED_ATT_TABLE \
        CHAR_FAN_V2_SWING_MODE_ATT_TABLE \
        CHAR_FAN_V2_LOCK_PHYSICAL_CONTROLS_ATT_TABLE \
        CHAR_FAN_V2_SVC_A5_SIGN_TABLE


#define FAN_V2_HANDLE_COMMON_NUM     8

#define FAN_V2_HANDLE_NUM_HFAN_V2_ACTIVE            	FAN_V2_HANDLE_START_NUM +FAN_V2_HANDLE_COMMON_NUM
#define FAN_V2_HANDLE_NUM_HFAN_V2_CURRENTFANSTATE   	FAN_V2_HANDLE_NUM_HFAN_V2_ACTIVE +FAN_V2_ACTIVE_NO+FAN_V2_ACTIVE_IND_NO
#define FAN_V2_HANDLE_NUM_HFAN_V2_TARGETFANSTATE    	FAN_V2_HANDLE_NUM_HFAN_V2_CURRENTFANSTATE +FAN_V2_CURRENT_FAN_STATE_NO +FAN_V2_CURRENT_FAN_STATE_IND_NO
#define FAN_V2_HANDLE_NUM_HFAN_V2_ROTATIONDIRECTION		FAN_V2_HANDLE_NUM_HFAN_V2_TARGETFANSTATE +FAN_V2_TARGET_FAN_STATE_NO +FAN_V2_TARGET_FAN_STATE_IND_NO
#define FAN_V2_HANDLE_NUM_HFAN_V2_ROTATIONSPEED    		FAN_V2_HANDLE_NUM_HFAN_V2_ROTATIONDIRECTION +FAN_V2_ROTATION_DIRECTION_NO +FAN_V2_ROTATION_DIRECTION_IND_NO
#define FAN_V2_HANDLE_NUM_HFAN_V2_SWINGMODE    			FAN_V2_HANDLE_NUM_HFAN_V2_ROTATIONSPEED +FAN_V2_ROTATION_SPEED_NO +FAN_V2_ROTATION_SPEED_IND_NO
#define FAN_V2_HANDLE_NUM_HFAN_V2_LOCKPHYSICALCONTROLS	FAN_V2_HANDLE_NUM_HFAN_V2_SWINGMODE +FAN_V2_SWING_MODE_NO +FAN_V2_SWING_MODE_IND_NO
#define FAN_V2_HANDLE_NUM_THE_END


#define FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_ACTIVE                    u32 FanV2Active:1;    /* 0~1 */
#define FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_ACTIVE_STORE                  STORE_CHAR(FanV2Active,hFanV2Active)
#define FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_ACTIVE_RESTORE                RESTORE_CHAR(FanV2Active,hFanV2Active)

#define FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_CURRENTFANSTATE           u32 FanV2CurrentFanState:2;    /* 0~2 */
#define FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_CURRENTFANSTATE_STORE         STORE_CHAR(FanV2CurrentFanState,hFanV2CurrentFanState)
#define FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_CURRENTFANSTATE_RESTORE       RESTORE_CHAR(FanV2CurrentFanState,hFanV2CurrentFanState)

#define FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_TARGETFANSTATE            u32 FanV2TargetFanState:1;    /* 0~1 */
#define FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_TARGETFANSTATE_STORE          STORE_CHAR(FanV2TargetFanState,hFanV2TargetFanState)
#define FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_TARGETFANSTATE_RESTORE        RESTORE_CHAR(FanV2TargetFanState,hFanV2TargetFanState)

#define FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_ROTATIONDIRECTION         u32 FanV2RotationDirection:1;    /* 0~1 */
#define FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_ROTATIONDIRECTION_STORE       STORE_CHAR(FanV2RotationDirection,hFanV2RotationDirection)
#define FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_ROTATIONDIRECTION_RESTORE     RESTORE_CHAR(FanV2RotationDirection,hFanV2RotationDirection)

#define FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_ROTATIONSPEED             u32 FanV2RotationSpeed:1;    /* float */
#define FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_ROTATIONSPEED_STORE           STORE_CHAR(FanV2RotationSpeed,hFanV2RotationSpeed)
#define FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_ROTATIONSPEED_RESTORE         RESTORE_CHAR(FanV2RotationSpeed,hFanV2RotationSpeed)

#define FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_SWINGMODE                 u32 FanV2SwingMode:1;    /* 0~1 */
#define FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_SWINGMODE_STORE               STORE_CHAR(FanV2SwingMode,hFanV2SwingMode)
#define FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_SWINGMODE_RESTORE             RESTORE_CHAR(FanV2SwingMode,hFanV2SwingMode)

#define FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_LOCKPHYSICALCONTROLS      u32 FanV2LockPhysicalControls:1;    /* 0~1 */
#define FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_LOCKPHYSICALCONTROLS_STORE    STORE_CHAR(FanV2LockPhysicalControls,hFanV2LockPhysicalControls)
#define FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_LOCKPHYSICALCONTROLS_RESTORE  RESTORE_CHAR(FanV2LockPhysicalControls,hFanV2LockPhysicalControls)


#define FAN_V2_STORE_CHAR_VALUE_T  \
                    FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_ACTIVE  \
                    FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_CURRENTFANSTATE \
					FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_TARGETFANSTATE \
					FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_ROTATIONDIRECTION \
					FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_ROTATIONSPEED \
					FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_SWINGMODE \
					FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_LOCKPHYSICALCONTROLS
	
#define FAN_V2_STORE_CHAR_TO_FLASH  \
                    FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_ACTIVE_STORE  \
                    FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_CURRENTFANSTATE_STORE \
                    FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_TARGETFANSTATE_STORE \
					FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_ROTATIONDIRECTION_STORE \
                    FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_ROTATIONSPEED_STORE \
                    FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_SWINGMODE_STORE \
                    FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_LOCKPHYSICALCONTROLS_STORE 

#define FAN_V2_RESTORE_CHAR_FROM_FLASH  \
                    FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_ACTIVE_RESTORE  \
                    FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_CURRENTFANSTATE_RESTORE \
                    FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_TARGETFANSTATE_RESTORE \
					FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_ROTATIONDIRECTION_RESTORE \
                    FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_ROTATIONSPEED_RESTORE \
                    FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_SWINGMODE_RESTORE \
                    FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_LOCKPHYSICALCONTROLS_RESTORE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN unsigned char hFanV2Active;
EXTERN unsigned char hFanV2CurrentFanState;
EXTERN unsigned char hFanV2TargetFanState;
EXTERN int hFanV2RotationDirection;
EXTERN int hFanV2RotationSpeed;
EXTERN unsigned char hFanV2SwingMode;
EXTERN unsigned char hFanV2LockPhysicalControls;

EXTERN unsigned short ccc_hFanV2Active;
EXTERN unsigned short ccc_hFanV2CurrentFanState;
EXTERN unsigned short ccc_hFanV2TargetFanState;
EXTERN unsigned short ccc_hFanV2RotationDirection;
EXTERN unsigned short ccc_hFanV2RotationSpeed;
EXTERN unsigned short ccc_hFanV2SwingMode;
EXTERN unsigned short ccc_hFanV2LockPhysicalControls;

#if FAN_V2_ACTIVE_BRC_ENABLE
    #define BC_PARA_FAN_V2_ACTIVE ((u8*)&bc_para_hFanV2Active)
    EXTERN unsigned char bc_para_hFanV2Active;
#else
    #define BC_PARA_FAN_V2_ACTIVE 0
#endif

#if FAN_V2_CURRENT_FAN_STATE_BRC_ENABLE
    #define BC_PARA_FAN_V2_CURRENT_FAN_STATE ((u8*)&bc_para_hFanV2CurrentFanState)
    EXTERN unsigned char bc_para_hFanV2CurrentFanState;
#else
    #define BC_PARA_FAN_V2_CURRENT_FAN_STATE 0
#endif

#if FAN_V2_TARGET_FAN_STATE_BRC_ENABLE
    #define BC_PARA_FAN_V2_TARGET_FAN_STATE ((u8*)&bc_para_hFanV2TargetFanState)
    EXTERN unsigned char bc_para_hFanV2TargetFanState;
#else
    #define BC_PARA_FAN_V2_TARGET_FAN_STATE 0
#endif

#if FAN_V2_ROTATION_DIRECTION_BRC_ENABLE
    #define BC_PARA_FAN_V2_ROTATION_DIRECTION ((u8*)&bc_para_hFanV2RotationDirection)
    EXTERN unsigned char bc_para_hFanRotationDirection;
#else
    #define BC_PARA_FAN_V2_ROTATION_DIRECTION 0
#endif

#if FAN_V2_ROTATION_SPEED_BRC_ENABLE
    #define BC_PARA_FAN_V2_ROTATION_SPEED ((u8*)&bc_para_hFanV2RotationSpeed)
    EXTERN unsigned char bc_para_hFanV2RotationSpeed;
#else
    #define BC_PARA_FAN_V2_ROTATION_SPEED 0
#endif

#if FAN_V2_SWING_MODE_BRC_ENABLE
    #define BC_PARA_FAN_V2_SWING_MODE ((u8*)&bc_para_hFanV2SwingMode)
    EXTERN unsigned char bc_para_hFanV2SwingMode;
#else
    #define BC_PARA_FAN_V2_SWING_MODE 0
#endif

#if FAN_V2_LOCK_PHYSICAL_CONTROLS_BRC_ENABLE
    #define BC_PARA_FAN_V2_LOCK_PHYSICAL_CONTROLS ((u8*)&bc_para_hFanV2LockPhysicalControls)
    EXTERN unsigned char bc_para_hFanV2LockPhysicalControls;
#else
    #define BC_PARA_FAN_V2_LOCK_PHYSICAL_CONTROLS 0
#endif

#if FAN_V2_LINKED_SERVICES_ENABLE
	#define FAN_V2_LINKED_SVCS			hFanV2_linked_svcs
	#define FAN_V2_LINKED_SVCS_LENGTH 	(sizeof(hFanV2_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * hFanV2_linked_svcs[];
#else
	#define FAN_V2_LINKED_SVCS			0
	#define FAN_V2_LINKED_SVCS_LENGTH 	0
#endif

EXTERN const u8 hFanV2ServiceName[12];

EXTERN const hap_characteristic_desc_t hap_desc_char_FanV2_Active_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_FanV2_CurrentFanState_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_FanV2_TargetFanState_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_FanV2_RotationDirection_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_FanV2_RotationSpeed_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_FanV2_SwingMode_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_FanV2_LockPhysicalControls_R_S;

/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN int FanV2ActiveReadCallback(void *pp);
EXTERN int FanV2ActiveWriteCallback(void *pp);
EXTERN int FanV2CurrentFanStateReadCallback(void *pp);
EXTERN int FanV2TargetFanStateReadCallback(void *pp);
EXTERN int FanV2TargetFanStateWriteCallback(void *pp);
EXTERN int FanV2RotationDirectionReadCallback(void *pp);
EXTERN int FanV2RotationDirectionWriteCallback(void *pp);
EXTERN int FanV2RotationSpeedReadCallback(void *pp);
EXTERN int FanV2RotationSpeedWriteCallback(void *pp);
EXTERN int FanV2SwingModeReadCallback(void *pp);
EXTERN int FanV2SwingModeWriteCallback(void *pp);
EXTERN int FanV2LockPhysicalControlsReadCallback(void *pp);
EXTERN int FanV2LockPhysicalControlsWriteCallback(void *pp);
	
EXTERN int ccc_hFanV2ActiveWriteCB(void *pp);
EXTERN int ccc_hFanV2CurrentFanStateWriteCB(void *pp);
EXTERN int ccc_hFanV2TargetFanStateWriteCB(void *pp);
EXTERN int ccc_hFanV2RotationDirectionWriteCB(void *pp);
EXTERN int ccc_hFanV2RotationSpeedWriteCB(void *pp);
EXTERN int ccc_hFanV2SwingModeWriteCB(void *pp);
EXTERN int ccc_hFanV2LockPhysicalControlsWriteCB(void *pp);

#endif /* _HK_FAN_V2_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
