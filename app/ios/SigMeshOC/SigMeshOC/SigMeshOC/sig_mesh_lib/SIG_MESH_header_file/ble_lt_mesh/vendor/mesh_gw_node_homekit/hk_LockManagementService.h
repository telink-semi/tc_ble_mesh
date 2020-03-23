/********************************************************************************************************
 * @file     hk_LockManagementService.h
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
#ifndef _HK_LOCK_MANAGEMENT_SERVICE_H_
#define _HK_LOCK_MANAGEMENT_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_LOCK_MANAGEMENT_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif


#define CHAR_LOCKMANAGEMENT_SVC_SIGN_ENABLE      0
#if CHAR_LOCKMANAGEMENT_SVC_SIGN_ENABLE
    #define LOCKMANAGEMENT_AS_PRIMARY_SERVICE    	0
    #define LOCKMANAGEMENT_IS_HIDDEN_SERVICE     	0
    #define LOCKMANAGEMENT_LINKED_SERVICES_ENABLE	0
    EXTERN const hap_characteristic_desc_t hap_desc_char_lockManagement_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_lockManagement_A5_R_S;
    EXTERN unsigned char VAR_LOCKMANAGEMENT_UNKOWN;
#else
    #define LOCKMANAGEMENT_AS_PRIMARY_SERVICE    	0
    #define LOCKMANAGEMENT_IS_HIDDEN_SERVICE     	0
    #define LOCKMANAGEMENT_LINKED_SERVICES_ENABLE	0
#endif


#define CHAR_LOCKMANAGEMENT_LOGS_ENABLE                     0
#define CHAR_LOCKMANAGEMENT_AUDIO_FEEDBACK_ENABLE           0
#define CHAR_LOCKMANAGEMENT_AUTO_SECURITY_TIMEOUT_ENABLE    0
#define CHAR_LOCKMANAGEMENT_ADMIN_ONLY_ACCESS_ENABLE        0
#define CHAR_LOCKMANAGEMENT_LOCK_LAST_KNOWN_ACTION_ENABLE   0
#define CHAR_LOCKMANAGEMENT_CURRENT_DOOR_STATE_ENABLE       0
#define CHAR_LOCKMANAGEMENT_MOTION_DETECTED_ENABLE          0

#define LOCKMANAGEMENT_LOGS_IND_ENABLE                      0
#define LOCKMANAGEMENT_AUDIO_FEEDBACK_IND_ENABLE            0
#define LOCKMANAGEMENT_AUTO_SECURITY_TIMEOUT_IND_ENABLE     0
#define LOCKMANAGEMENT_ADMIN_ONLY_ACCESS_IND_ENABLE         0
#define LOCKMANAGEMENT_LOCK_LAST_KNOWN_ACTION_IND_ENABLE    0
#define LOCKMANAGEMENT_CURRENT_DOOR_STATE_IND_ENABLE        0
#define LOCKMANAGEMENT_MOTION_DETECTED_IND_ENABLE           0

#define LOCKMANAGEMENT_AUDIO_FEEDBACK_BRC_ENABLE            0
#define LOCKMANAGEMENT_AUTO_SECURITY_TIMEOUT_BRC_ENABLE     0
#define LOCKMANAGEMENT_ADMIN_ONLY_ACCESS_BRC_ENABLE         0
#define LOCKMANAGEMENT_LOCK_LAST_KNOWN_ACTION_BRC_ENABLE    0
#define LOCKMANAGEMENT_CURRENT_DOOR_STATE_BRC_ENABLE        0
#define LOCKMANAGEMENT_MOTION_DETECTED_BRC_ENABLE           0


#define LOCKMANAGEMENT_LOCK_CONTROL_POINT_NO 3
#define LOCKMANAGEMENT_VERSION_NO 3

#if CHAR_LOCKMANAGEMENT_LOGS_ENABLE
    #define LOCKMANAGEMENT_LOGS_NO   3
    #define CHAR_LOCKMANAGEMENT_LOGS_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_LOGS_R_S, 0}, \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(LockManagementLogs), 0, (u8*)(charLogsUUID), (u8*)&LockManagementLogs, 0, LockManagementLogsReadCallback},  \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + LOCKMANAGEMENT_INST_NO + 3), 0, 0},   \
            LOCKMANAGEMENT_LOGS_IND_DESC

    #if LOCKMANAGEMENT_LOGS_IND_ENABLE
        #define LOCKMANAGEMENT_LOGS_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_LockManagementLogs), ccc_LockManagementLogsWriteCB, 0},
        #define LOCKMANAGEMENT_LOGS_IND_NO  1
    #else
        #define LOCKMANAGEMENT_LOGS_IND_DESC
        #define LOCKMANAGEMENT_LOGS_IND_NO  0
    #endif

#else
    #define LOCKMANAGEMENT_LOGS_NO   0
    #define CHAR_LOCKMANAGEMENT_LOGS_ATT_TABLE
    #define LOCKMANAGEMENT_LOGS_IND_NO  0
#endif

#if CHAR_LOCKMANAGEMENT_AUDIO_FEEDBACK_ENABLE
    #define LOCKMANAGEMENT_AUDIO_FEEDBACK_NO   3
    #define CHAR_LOCKMANAGEMENT_AUDIO_FEEDBACK_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_AUDIO_FEEDBACK_R_S, 0}, \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(AudioFeedback), 0, (u8*)(charAudioFeedbackUUID), (u8*)&AudioFeedback, AudioFeedbackWriteCallback, AudioFeedbackReadCallback},  \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + LOCKMANAGEMENT_INST_NO + 4), 0, 0},   \
            LOCKMANAGEMENT_AUDIO_FEEDBACK_IND_DESC

    #if LOCKMANAGEMENT_AUDIO_FEEDBACK_IND_ENABLE
        #define LOCKMANAGEMENT_AUDIO_FEEDBACK_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_AudioFeedback), ccc_AudioFeedbackWriteCB, 0},
        #define LOCKMANAGEMENT_AUDIO_FEEDBACK_IND_NO  1
    #else
        #define LOCKMANAGEMENT_AUDIO_FEEDBACK_IND_DESC
        #define LOCKMANAGEMENT_AUDIO_FEEDBACK_IND_NO  0
    #endif
#else
    #define LOCKMANAGEMENT_AUDIO_FEEDBACK_NO   0
    #define CHAR_LOCKMANAGEMENT_AUDIO_FEEDBACK_ATT_TABLE
    #define LOCKMANAGEMENT_AUDIO_FEEDBACK_IND_NO  0
#endif

#if CHAR_LOCKMANAGEMENT_AUTO_SECURITY_TIMEOUT_ENABLE
    #define LOCKMANAGEMENT_AUTO_SECURITY_TIMEOUT_NO   3
    #define CHAR_LOCKMANAGEMENT_AUTO_SECURITY_TIMEOUT_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_AUTO_SECURITY_TIMEOUT_R_S, 0}, \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(AutoSecurityTimeout), 0, (u8*)(charAutoSecurityTimeoutUUID), (u8*)&AutoSecurityTimeout, AutoSecurityTimeoutWriteCallback, AutoSecurityTimeoutReadCallback},  \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + LOCKMANAGEMENT_INST_NO + 5), 0, 0},   \
            LOCKMANAGEMENT_AUTO_SECURITY_TIMEOUT_IND_DESC

    #if LOCKMANAGEMENT_AUTO_SECURITY_TIMEOUT_IND_ENABLE
        #define LOCKMANAGEMENT_AUTO_SECURITY_TIMEOUT_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_AutoSecurityTimeout), ccc_AutoSecurityTimeoutWriteCB, 0},
        #define LOCKMANAGEMENT_AUTO_SECURITY_TIMEOUT_IND_NO  1
    #else
        #define LOCKMANAGEMENT_AUTO_SECURITY_TIMEOUT_IND_DESC
        #define LOCKMANAGEMENT_AUTO_SECURITY_TIMEOUT_IND_NO  0
    #endif
#else
    #define LOCKMANAGEMENT_AUTO_SECURITY_TIMEOUT_NO   0
    #define CHAR_LOCKMANAGEMENT_AUTO_SECURITY_TIMEOUT_ATT_TABLE
    #define LOCKMANAGEMENT_AUTO_SECURITY_TIMEOUT_IND_NO  0
#endif

#if CHAR_LOCKMANAGEMENT_ADMIN_ONLY_ACCESS_ENABLE
    #define LOCKMANAGEMENT_ADMIN_ONLY_ACCESS_NO   3
    #define CHAR_LOCKMANAGEMENT_ADMIN_ONLY_ACCESS_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_ADMIN_ONLY_ACCESS_R_S, 0}, \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(AdminOnlyAccess), 0, (u8*)(charAdminOnlyAccessUUID), (u8*)&AdminOnlyAccess, AdminOnlyAccessWriteCallback, AdminOnlyAccessReadCallback},  \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + LOCKMANAGEMENT_INST_NO + 6), 0, 0},   \
            LOCKMANAGEMENT_ADMIN_ONLY_ACCESS_IND_DESC

    #if LOCKMANAGEMENT_ADMIN_ONLY_ACCESS_IND_ENABLE
        #define LOCKMANAGEMENT_ADMIN_ONLY_ACCESS_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_AdminOnlyAccess), ccc_AdminOnlyAccessWriteCB, 0},
        #define LOCKMANAGEMENT_ADMIN_ONLY_ACCESS_IND_NO  1
    #else
        #define LOCKMANAGEMENT_ADMIN_ONLY_ACCESS_IND_DESC
        #define LOCKMANAGEMENT_ADMIN_ONLY_ACCESS_IND_NO  0
    #endif
#else
    #define LOCKMANAGEMENT_ADMIN_ONLY_ACCESS_NO   0
    #define CHAR_LOCKMANAGEMENT_ADMIN_ONLY_ACCESS_ATT_TABLE
    #define LOCKMANAGEMENT_ADMIN_ONLY_ACCESS_IND_NO  0
#endif

#if CHAR_LOCKMANAGEMENT_LOCK_LAST_KNOWN_ACTION_ENABLE
    #define LOCKMANAGEMENT_LOCK_LAST_KNOWN_ACTION_NO   3
    #define CHAR_LOCKMANAGEMENT_LOCK_LAST_KNOWN_ACTION_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_LOCK_LAST_KNOWN_ACTION_R_S, 0}, \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(LockLastKnownAction), 0, (u8*)(charLockLastKnownActionUUID), (u8*)&LockLastKnownAction, 0, LockLastKnownActionReadCallback},  \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + LOCKMANAGEMENT_INST_NO + 7), 0, 0},   \
            LOCKMANAGEMENT_LOCK_LAST_KNOWN_ACTION_IND_DESC

    #if LOCKMANAGEMENT_LOCK_LAST_KNOWN_ACTION_IND_ENABLE
        #define LOCKMANAGEMENT_LOCK_LAST_KNOWN_ACTION_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_LockLastKnownAction), ccc_LockLastKnownActionWriteCB, 0},
        #define LOCKMANAGEMENT_LOCK_LAST_KNOWN_ACTION_IND_NO  1
    #else
        #define LOCKMANAGEMENT_LOCK_LAST_KNOWN_ACTION_IND_DESC
        #define LOCKMANAGEMENT_LOCK_LAST_KNOWN_ACTION_IND_NO  0
    #endif
#else
    #define LOCKMANAGEMENT_LOCK_LAST_KNOWN_ACTION_NO   0
    #define CHAR_LOCKMANAGEMENT_LOCK_LAST_KNOWN_ACTION_ATT_TABLE
    #define LOCKMANAGEMENT_LOCK_LAST_KNOWN_ACTION_IND_NO  0
#endif

#if CHAR_LOCKMANAGEMENT_CURRENT_DOOR_STATE_ENABLE
    #define LOCKMANAGEMENT_CURRENT_DOOR_STATE_NO   3
    #define CHAR_LOCKMANAGEMENT_CURRENT_DOOR_STATE_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_CURRENT_DOOR_STATE_R_S, 0}, \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(LMCurrentDoorState), 0, (u8*)(charCurrentDoorStateUUID), (u8*)&LMCurrentDoorState, 0, LMCurrentDoorStateReadCallback},  \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + LOCKMANAGEMENT_INST_NO + 8), 0, 0},   \
            LOCKMANAGEMENT_CURRENT_DOOR_STATE_IND_DESC

    #if LOCKMANAGEMENT_CURRENT_DOOR_STATE_IND_ENABLE
        #define LOCKMANAGEMENT_CURRENT_DOOR_STATE_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_LMCurrentDoorState), ccc_LMCurrentDoorStateWriteCB, 0},
        #define LOCKMANAGEMENT_CURRENT_DOOR_STATE_IND_NO  1
    #else
        #define LOCKMANAGEMENT_CURRENT_DOOR_STATE_IND_DESC
        #define LOCKMANAGEMENT_CURRENT_DOOR_STATE_IND_NO  0
    #endif
#else
    #define LOCKMANAGEMENT_CURRENT_DOOR_STATE_NO   0
    #define CHAR_LOCKMANAGEMENT_CURRENT_DOOR_STATE_ATT_TABLE
    #define LOCKMANAGEMENT_CURRENT_DOOR_STATE_IND_NO  0
#endif

#if CHAR_LOCKMANAGEMENT_MOTION_DETECTED_ENABLE
    #define LOCKMANAGEMENT_MOTION_DETECTED_NO   3
    #define CHAR_LOCKMANAGEMENT_MOTION_DETECTED_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_MOTION_DETECTED_R_S, 0}, \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(LMMotionDetected), 0, (u8*)(charMotionDetectedUUID), (u8*)&LMMotionDetected, 0, LMMotionDetectedReadCallback},  \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + LOCKMANAGEMENT_INST_NO + 9), 0, 0},   \
            LOCKMANAGEMENT_MOTION_DETECTED_IND_DESC

    #if LOCKMANAGEMENT_MOTION_DETECTED_IND_ENABLE
        #define LOCKMANAGEMENT_MOTION_DETECTED_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_LMMotionDetected), ccc_LMMotionDetectedWriteCB, 0},
        #define LOCKMANAGEMENT_MOTION_DETECTED_IND_NO  1
    #else
        #define LOCKMANAGEMENT_MOTION_DETECTED_IND_DESC
        #define LOCKMANAGEMENT_MOTION_DETECTED_IND_NO  0
    #endif
#else
    #define LOCKMANAGEMENT_MOTION_DETECTED_NO   0
    #define CHAR_LOCKMANAGEMENT_MOTION_DETECTED_ATT_TABLE
    #define LOCKMANAGEMENT_MOTION_DETECTED_IND_NO  0
#endif


#if CHAR_LOCKMANAGEMENT_SVC_SIGN_ENABLE
    #define CHAR_LOCKMANAGEMENT_SVC_A5_SIGN_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_lockManagement_A5_R_S, &hap_desc_service_lockManagement_A5_R_S},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_LOCKMANAGEMENT_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_LOCKMANAGEMENT_UNKOWN, 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + LOCKMANAGEMENT_INST_NO + 10), 0, 0},
    #define LOCKMANAGEMENT_SVC_SIGN_NO   3
#else
    #define CHAR_LOCKMANAGEMENT_SVC_A5_SIGN_TABLE
    #define LOCKMANAGEMENT_SVC_SIGN_NO   0
#endif


#define LOCKMANAGEMENT_NO_DELTA LOCKMANAGEMENT_LOGS_IND_NO +LOCKMANAGEMENT_AUDIO_FEEDBACK_IND_NO \
                                +LOCKMANAGEMENT_AUTO_SECURITY_TIMEOUT_IND_NO +LOCKMANAGEMENT_ADMIN_ONLY_ACCESS_IND_NO \
                                +LOCKMANAGEMENT_LOCK_LAST_KNOWN_ACTION_IND_NO +LOCKMANAGEMENT_CURRENT_DOOR_STATE_IND_NO \
                                +LOCKMANAGEMENT_MOTION_DETECTED_IND_NO

#define LOCKMANAGEMENT_NO       9 +LOCKMANAGEMENT_LOGS_NO +LOCKMANAGEMENT_AUDIO_FEEDBACK_NO \
                                +LOCKMANAGEMENT_AUTO_SECURITY_TIMEOUT_NO +LOCKMANAGEMENT_ADMIN_ONLY_ACCESS_NO \
                                +LOCKMANAGEMENT_LOCK_LAST_KNOWN_ACTION_NO +LOCKMANAGEMENT_CURRENT_DOOR_STATE_NO \
                                +LOCKMANAGEMENT_MOTION_DETECTED_NO +LOCKMANAGEMENT_SVC_SIGN_NO

#define LOCKMANAGEMENT_HANDLE_NUM (LOCKMANAGEMENT_NO + LOCKMANAGEMENT_NO_DELTA)


#define HOMEKIT_LOCKMANAGEMENT_SERVICE_ATT_TABLE    \
    {LOCKMANAGEMENT_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(lockManagementServiceUUID),sizeof(lockManagementServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(lockManagementServiceUUID), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(insts + LOCKMANAGEMENT_INST_NO), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_LOCK_CONTROL_POINT, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(Lock_Control_Point),0, (u8*)(charLockControlPointUUID), (u8*)&Lock_Control_Point, LockControlPointWriteCallback, 0},  \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + LOCKMANAGEMENT_INST_NO + 1), 0, 0},   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_VERSION, 0}, \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(Lock_Management_Version),0, (u8*)(charVersionUUID), (u8*)&Lock_Management_Version, 0, LockManagementVersionReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + LOCKMANAGEMENT_INST_NO + 2), 0, 0},   \
        CHAR_LOCKMANAGEMENT_LOGS_ATT_TABLE \
        CHAR_LOCKMANAGEMENT_AUDIO_FEEDBACK_ATT_TABLE \
        CHAR_LOCKMANAGEMENT_AUTO_SECURITY_TIMEOUT_ATT_TABLE \
        CHAR_LOCKMANAGEMENT_ADMIN_ONLY_ACCESS_ATT_TABLE \
        CHAR_LOCKMANAGEMENT_LOCK_LAST_KNOWN_ACTION_ATT_TABLE \
        CHAR_LOCKMANAGEMENT_CURRENT_DOOR_STATE_ATT_TABLE \
        CHAR_LOCKMANAGEMENT_MOTION_DETECTED_ATT_TABLE \
        CHAR_LOCKMANAGEMENT_SVC_A5_SIGN_TABLE


#define LOCKMANAGEMENT_HANDLE_COMMON_NUM     5

#define LOCKMANAGEMENT_HANDLE_NUM_LOCK_CONTROL_POINT        LOCKMANAGEMENT_HANDLE_START_NUM +LOCKMANAGEMENT_HANDLE_COMMON_NUM
#define LOCKMANAGEMENT_HANDLE_NUM_VERSION                   LOCKMANAGEMENT_HANDLE_NUM_LOCK_CONTROL_POINT +LOCKMANAGEMENT_LOCK_CONTROL_POINT_NO +0
#define LOCKMANAGEMENT_HANDLE_NUM_LOGS                      LOCKMANAGEMENT_HANDLE_NUM_VERSION +LOCKMANAGEMENT_VERSION_NO +0
#define LOCKMANAGEMENT_HANDLE_NUM_AUDIO_FEEDBACK            LOCKMANAGEMENT_HANDLE_NUM_LOGS +LOCKMANAGEMENT_LOGS_NO +LOCKMANAGEMENT_LOGS_IND_NO
#define LOCKMANAGEMENT_HANDLE_NUM_AUTO_SECURITY_TIMEOUT     LOCKMANAGEMENT_HANDLE_NUM_AUDIO_FEEDBACK +LOCKMANAGEMENT_AUDIO_FEEDBACK_NO +LOCKMANAGEMENT_AUDIO_FEEDBACK_IND_NO
#define LOCKMANAGEMENT_HANDLE_NUM_ADMIN_ONLY_ACCESS         LOCKMANAGEMENT_HANDLE_NUM_AUTO_SECURITY_TIMEOUT +LOCKMANAGEMENT_AUTO_SECURITY_TIMEOUT_NO +LOCKMANAGEMENT_AUTO_SECURITY_TIMEOUT_IND_NO
#define LOCKMANAGEMENT_HANDLE_NUM_LOCK_LAST_KNOWN_ACTION    LOCKMANAGEMENT_HANDLE_NUM_ADMIN_ONLY_ACCESS +LOCKMANAGEMENT_ADMIN_ONLY_ACCESS_NO +LOCKMANAGEMENT_ADMIN_ONLY_ACCESS_IND_NO
#define LOCKMANAGEMENT_HANDLE_NUM_CURRENT_DOOR_STATE        LOCKMANAGEMENT_HANDLE_NUM_LOCK_LAST_KNOWN_ACTION +LOCKMANAGEMENT_LOCK_LAST_KNOWN_ACTION_NO +LOCKMANAGEMENT_LOCK_LAST_KNOWN_ACTION_IND_NO
#define LOCKMANAGEMENT_HANDLE_NUM_MOTION_DETECTED           LOCKMANAGEMENT_HANDLE_NUM_CURRENT_DOOR_STATE +LOCKMANAGEMENT_CURRENT_DOOR_STATE_NO +LOCKMANAGEMENT_CURRENT_DOOR_STATE_IND_NO
#define LOCKMANAGEMENT_HANDLE_NUM_THE_END


#define LOCKMANAGEMENT_STORE_CHAR_VALUE_T_LOCK_CONTROL_POINT
#define LOCKMANAGEMENT_STORE_CHAR_VALUE_T_LOCK_CONTROL_POINT_STORE
#define LOCKMANAGEMENT_STORE_CHAR_VALUE_T_LOCK_CONTROL_POINT_RESTORE

#define LOCKMANAGEMENT_STORE_CHAR_VALUE_T_LOCK_MANAGEMENT_VERSION
#define LOCKMANAGEMENT_STORE_CHAR_VALUE_T_LOCK_MANAGEMENT_VERSION_STORE
#define LOCKMANAGEMENT_STORE_CHAR_VALUE_T_LOCK_MANAGEMENT_VERSION_RESTORE

#define LOCKMANAGEMENT_STORE_CHAR_VALUE_T_LOGS
#define LOCKMANAGEMENT_STORE_CHAR_VALUE_T_LOGS_STORE
#define LOCKMANAGEMENT_STORE_CHAR_VALUE_T_LOGS_RESTORE

#define LOCKMANAGEMENT_STORE_CHAR_VALUE_T_AUDIO_FEEDBACK      u32 F_AudioFeedback:1;   /* bool */
#define LOCKMANAGEMENT_STORE_CHAR_VALUE_T_AUDIO_FEEDBACK_STORE    STORE_CHAR(F_AudioFeedback,AudioFeedback)
#define LOCKMANAGEMENT_STORE_CHAR_VALUE_T_AUDIO_FEEDBACK_RESTORE  RESTORE_CHAR(F_AudioFeedback,AudioFeedback)

#define LOCKMANAGEMENT_STORE_CHAR_VALUE_T_AUTO_SECURITY_TIMEOUT      u32 F_AutoSecurityTimeout;   /* uint32 */
#define LOCKMANAGEMENT_STORE_CHAR_VALUE_T_AUTO_SECURITY_TIMEOUT_STORE    STORE_CHAR(F_AutoSecurityTimeout,AutoSecurityTimeout)
#define LOCKMANAGEMENT_STORE_CHAR_VALUE_T_AUTO_SECURITY_TIMEOUT_RESTORE  RESTORE_CHAR(F_AutoSecurityTimeout,AutoSecurityTimeout)

#define LOCKMANAGEMENT_STORE_CHAR_VALUE_T_ADMIN_ONLY_ACCESS      u32 F_AdminOnlyAccess:1;   /* bool */
#define LOCKMANAGEMENT_STORE_CHAR_VALUE_T_ADMIN_ONLY_ACCESS_STORE    STORE_CHAR(F_AdminOnlyAccess,AdminOnlyAccess)
#define LOCKMANAGEMENT_STORE_CHAR_VALUE_T_ADMIN_ONLY_ACCESS_RESTORE  RESTORE_CHAR(F_AdminOnlyAccess,AdminOnlyAccess)

#define LOCKMANAGEMENT_STORE_CHAR_VALUE_T_LOCK_LAST_KNOWN_ACTION      u32 F_LockLastKnownAction:4;   /* 0~8 */
#define LOCKMANAGEMENT_STORE_CHAR_VALUE_T_LOCK_LAST_KNOWN_ACTION_STORE    STORE_CHAR(F_LockLastKnownAction,LockLastKnownAction)
#define LOCKMANAGEMENT_STORE_CHAR_VALUE_T_LOCK_LAST_KNOWN_ACTION_RESTORE  RESTORE_CHAR(F_LockLastKnownAction,LockLastKnownAction)

#define LOCKMANAGEMENT_STORE_CHAR_VALUE_T_CURRENT_DOOR_STATE      u32 F_LMCurrentDoorState:3;   /* 0~4 */
#define LOCKMANAGEMENT_STORE_CHAR_VALUE_T_CURRENT_DOOR_STATE_STORE    STORE_CHAR(F_LMCurrentDoorState,LMCurrentDoorState)
#define LOCKMANAGEMENT_STORE_CHAR_VALUE_T_CURRENT_DOOR_STATE_RESTORE  RESTORE_CHAR(F_LMCurrentDoorState,LMCurrentDoorState)

#define LOCKMANAGEMENT_STORE_CHAR_VALUE_T_MOTION_DETECTED      u32 F_LMMotionDetected:1;   /* bool */
#define LOCKMANAGEMENT_STORE_CHAR_VALUE_T_MOTION_DETECTED_STORE    STORE_CHAR(F_LMMotionDetected,LMMotionDetected)
#define LOCKMANAGEMENT_STORE_CHAR_VALUE_T_MOTION_DETECTED_RESTORE  RESTORE_CHAR(F_LMMotionDetected,LMMotionDetected)


#define LOCKMANAGEMENT_STORE_CHAR_VALUE_T            \
        LOCKMANAGEMENT_STORE_CHAR_VALUE_T_LOCK_CONTROL_POINT \
        LOCKMANAGEMENT_STORE_CHAR_VALUE_T_LOCK_MANAGEMENT_VERSION \
        LOCKMANAGEMENT_STORE_CHAR_VALUE_T_LOGS \
        LOCKMANAGEMENT_STORE_CHAR_VALUE_T_AUDIO_FEEDBACK \
        LOCKMANAGEMENT_STORE_CHAR_VALUE_T_AUTO_SECURITY_TIMEOUT \
        LOCKMANAGEMENT_STORE_CHAR_VALUE_T_ADMIN_ONLY_ACCESS \
        LOCKMANAGEMENT_STORE_CHAR_VALUE_T_LOCK_LAST_KNOWN_ACTION \
        LOCKMANAGEMENT_STORE_CHAR_VALUE_T_CURRENT_DOOR_STATE \
        LOCKMANAGEMENT_STORE_CHAR_VALUE_T_MOTION_DETECTED

#define LOCKMANAGEMENT_STORE_CHAR_TO_FLASH           \
        LOCKMANAGEMENT_STORE_CHAR_VALUE_T_LOCK_CONTROL_POINT_STORE \
        LOCKMANAGEMENT_STORE_CHAR_VALUE_T_LOCK_MANAGEMENT_VERSION_STORE \
        LOCKMANAGEMENT_STORE_CHAR_VALUE_T_LOGS_STORE \
        LOCKMANAGEMENT_STORE_CHAR_VALUE_T_AUDIO_FEEDBACK_STORE \
        LOCKMANAGEMENT_STORE_CHAR_VALUE_T_AUTO_SECURITY_TIMEOUT_STORE \
        LOCKMANAGEMENT_STORE_CHAR_VALUE_T_ADMIN_ONLY_ACCESS_STORE \
        LOCKMANAGEMENT_STORE_CHAR_VALUE_T_LOCK_LAST_KNOWN_ACTION_STORE \
        LOCKMANAGEMENT_STORE_CHAR_VALUE_T_CURRENT_DOOR_STATE_STORE \
        LOCKMANAGEMENT_STORE_CHAR_VALUE_T_MOTION_DETECTED_STORE

#define LOCKMANAGEMENT_RESTORE_CHAR_FROM_FLASH       \
        LOCKMANAGEMENT_STORE_CHAR_VALUE_T_LOCK_CONTROL_POINT_RESTORE \
        LOCKMANAGEMENT_STORE_CHAR_VALUE_T_LOCK_MANAGEMENT_VERSION_RESTORE \
        LOCKMANAGEMENT_STORE_CHAR_VALUE_T_LOGS_RESTORE \
        LOCKMANAGEMENT_STORE_CHAR_VALUE_T_AUDIO_FEEDBACK_RESTORE \
        LOCKMANAGEMENT_STORE_CHAR_VALUE_T_AUTO_SECURITY_TIMEOUT_RESTORE \
        LOCKMANAGEMENT_STORE_CHAR_VALUE_T_ADMIN_ONLY_ACCESS_RESTORE \
        LOCKMANAGEMENT_STORE_CHAR_VALUE_T_LOCK_LAST_KNOWN_ACTION_RESTORE \
        LOCKMANAGEMENT_STORE_CHAR_VALUE_T_CURRENT_DOOR_STATE_RESTORE \
        LOCKMANAGEMENT_STORE_CHAR_VALUE_T_MOTION_DETECTED_RESTORE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN unsigned char Lock_Control_Point[24];
EXTERN const unsigned char Lock_Management_Version[4];
EXTERN unsigned char LockManagementLogs[24];
EXTERN unsigned char AudioFeedback;
EXTERN unsigned int AutoSecurityTimeout;
EXTERN unsigned char AdminOnlyAccess;
EXTERN unsigned char LockLastKnownAction;
EXTERN unsigned char LMCurrentDoorState;
EXTERN unsigned char LMMotionDetected;

EXTERN unsigned short ccc_LockManagementLogs;
EXTERN unsigned short ccc_AudioFeedback;
EXTERN unsigned short ccc_AutoSecurityTimeout;
EXTERN unsigned short ccc_AdminOnlyAccess;
EXTERN unsigned short ccc_LockLastKnownAction;
EXTERN unsigned short ccc_LMCurrentDoorState;
EXTERN unsigned short ccc_LMMotionDetected;

#if LOCKMANAGEMENT_AUDIO_FEEDBACK_BRC_ENABLE
    #define BC_PARA_LOCKMANAGEMENT_AUDIO_FEEDBACK ((u8*)&bc_para_AudioFeedback)
    EXTERN unsigned char bc_para_AudioFeedback;
#else
    #define BC_PARA_LOCKMANAGEMENT_AUDIO_FEEDBACK 0
#endif

#if LOCKMANAGEMENT_AUTO_SECURITY_TIMEOUT_BRC_ENABLE
    #define BC_PARA_LOCKMANAGEMENT_AUTO_SECURITY_TIMEOUT ((u8*)&bc_para_AutoSecurityTimeout)
    EXTERN unsigned char bc_para_AutoSecurityTimeout;
#else
    #define BC_PARA_LOCKMANAGEMENT_AUTO_SECURITY_TIMEOUT 0
#endif

#if LOCKMANAGEMENT_ADMIN_ONLY_ACCESS_BRC_ENABLE
    #define BC_PARA_LOCKMANAGEMENT_ADMIN_ONLY_ACCESS ((u8*)&bc_para_AdminOnlyAccess)
    EXTERN unsigned char bc_para_AdminOnlyAccess;
#else
    #define BC_PARA_LOCKMANAGEMENT_ADMIN_ONLY_ACCESS 0
#endif

#if LOCKMANAGEMENT_LOCK_LAST_KNOWN_ACTION_BRC_ENABLE
    #define BC_PARA_LOCKMANAGEMENT_LOCK_LAST_KNOWN_ACTION ((u8*)&bc_para_LockLastKnownAction)
    EXTERN unsigned char bc_para_LockLastKnownAction;
#else
    #define BC_PARA_LOCKMANAGEMENT_LOCK_LAST_KNOWN_ACTION 0
#endif

#if LOCKMANAGEMENT_CURRENT_DOOR_STATE_BRC_ENABLE
    #define BC_PARA_LOCKMANAGEMENT_CURRENT_DOOR_STATE ((u8*)&bc_para_LMCurrentDoorState)
    EXTERN unsigned char bc_para_LMCurrentDoorState;
#else
    #define BC_PARA_LOCKMANAGEMENT_CURRENT_DOOR_STATE 0
#endif

#if LOCKMANAGEMENT_MOTION_DETECTED_BRC_ENABLE
    #define BC_PARA_LOCKMANAGEMENT_MOTION_DETECTED ((u8*)&bc_para_LMMotionDetected)
    EXTERN unsigned char bc_para_LMMotionDetected;
#else
    #define BC_PARA_LOCKMANAGEMENT_MOTION_DETECTED 0
#endif

#if LOCKMANAGEMENT_LINKED_SERVICES_ENABLE
	#define LOCKMANAGEMENT_LINKED_SVCS			hLockManagement_linked_svcs
	#define LOCKMANAGEMENT_LINKED_SVCS_LENGTH 	(sizeof(hLockManagement_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * hLockManagement_linked_svcs[];
#else
	#define LOCKMANAGEMENT_LINKED_SVCS			0
	#define LOCKMANAGEMENT_LINKED_SVCS_LENGTH 	0
#endif

EXTERN const u8 my_lockmanagementServiceName[23];

EXTERN const hap_characteristic_desc_t hap_desc_char_LOGS_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_AUDIO_FEEDBACK_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_AUTO_SECURITY_TIMEOUT_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_ADMIN_ONLY_ACCESS_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_LOCK_LAST_KNOWN_ACTION_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_CURRENT_DOOR_STATE_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_MOTION_DETECTED_R_S;


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN int LockControlPointWriteCallback(void *pp);
EXTERN int LockManagementVersionReadCallback(void *pp);
EXTERN int LockManagementLogsReadCallback(void *pp);
EXTERN int AudioFeedbackWriteCallback(void *pp);
EXTERN int AudioFeedbackReadCallback(void *pp);
EXTERN int AutoSecurityTimeoutWriteCallback(void *pp);
EXTERN int AutoSecurityTimeoutReadCallback(void *pp);
EXTERN int AdminOnlyAccessWriteCallback(void *pp);
EXTERN int AdminOnlyAccessReadCallback(void *pp);
EXTERN int LockLastKnownActionReadCallback(void *pp);
EXTERN int LMCurrentDoorStateReadCallback(void *pp);
EXTERN int LMMotionDetectedReadCallback(void *pp);

EXTERN int ccc_LockManagementLogsWriteCB(void *pp);
EXTERN int ccc_AudioFeedbackWriteCB(void *pp);
EXTERN int ccc_AutoSecurityTimeoutWriteCB(void *pp);
EXTERN int ccc_AdminOnlyAccessWriteCB(void *pp);
EXTERN int ccc_LockLastKnownActionWriteCB(void *pp);
EXTERN int ccc_LMCurrentDoorStateWriteCB(void *pp);
EXTERN int ccc_LMMotionDetectedWriteCB(void *pp);


#endif /* _HK_LOCK_MANAGEMENT_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
