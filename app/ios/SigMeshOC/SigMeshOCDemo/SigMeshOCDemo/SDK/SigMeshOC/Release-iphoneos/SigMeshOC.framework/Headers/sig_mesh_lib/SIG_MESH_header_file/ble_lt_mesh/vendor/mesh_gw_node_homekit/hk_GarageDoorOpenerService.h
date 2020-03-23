/********************************************************************************************************
 * @file     hk_GarageDoorOpenerService.h
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
#ifndef _HK_GARAGE_DOOR_OPENER_SERVICE_H_
#define _HK_GARAGE_DOOR_OPENER_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_GARAGE_DOOR_OPENER_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif

#define CHAR_GARAGEDOOROPENER_SVC_SIGN_ENABLE      0
#if CHAR_GARAGEDOOROPENER_SVC_SIGN_ENABLE
    #define GARAGEDOOROPENER_AS_PRIMARY_SERVICE    		0
    #define GARAGEDOOROPENER_IS_HIDDEN_SERVICE     		0
    #define GARAGEDOOROPENER_LINKED_SERVICES_ENABLE		0
    EXTERN const hap_characteristic_desc_t hap_desc_char_garageDoor_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_garageDoor_A5_R_S;
    EXTERN unsigned char VAR_GARAGEDOOROPENER_UNKOWN;
#else
    #define GARAGEDOOROPENER_AS_PRIMARY_SERVICE    		0
    #define GARAGEDOOROPENER_IS_HIDDEN_SERVICE     		0
    #define GARAGEDOOROPENER_LINKED_SERVICES_ENABLE		0
#endif


#define CHAR_LOCK_CURRENT_STATE_ENABLE	            0
#define CHAR_LOCK_TARGET_STATE_ENABLE	            0

#define GARAGEDOOR_CURRENTDOORSTATE_IND_ENABLE      1
#define GARAGEDOOR_TARGETDOORSTATE_IND_ENABLE       1
#define GARAGEDOOR_OBSTRUCTIONDETECTED_IND_ENABLE   1
#define GARAGEDOOR_LOCKCURRENTSTATE_IND_ENABLE      0
#define GARAGEDOOR_LOCKTARGETSTATE_IND_ENABLE       0

#define GARAGEDOOR_CURRENTDOORSTATE_BRC_ENABLE      0
#define GARAGEDOOR_TARGETDOORSTATE_BRC_ENABLE       0
#define GARAGEDOOR_OBSTRUCTIONDETECTED_BRC_ENABLE   0
#define GARAGEDOOR_LOCKCURRENTSTATE_BRC_ENABLE      0
#define GARAGEDOOR_LOCKTARGETSTATE_BRC_ENABLE       0


#define GDOPENER_CURRENTDOORTSTATE_NO 3
#if GARAGEDOOR_CURRENTDOORSTATE_IND_ENABLE
    #define GARAGEDOOR_CURRENTDOORSTATE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hCurrent_Door_State), ccc_hCurrent_Door_StateWriteCB, 0},
    #define GARAGEDOOR_CURRENTDOORSTATE_IND_NO  1
#else
    #define GARAGEDOOR_CURRENTDOORSTATE_IND_DESC
    #define GARAGEDOOR_CURRENTDOORSTATE_IND_NO  0
#endif

#define GDOPENER_TARGETDOORSTATE_NO 3
#if GARAGEDOOR_TARGETDOORSTATE_IND_ENABLE
    #define GARAGEDOOR_TARGETDOORSTATE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hTarget_Door_State), ccc_hTarget_Door_StateWriteCB, 0},
    #define GARAGEDOOR_TARGETDOORSTATE_IND_NO  1
#else
    #define GARAGEDOOR_TARGETDOORSTATE_IND_DESC
    #define GARAGEDOOR_TARGETDOORSTATE_IND_NO  0
#endif

#define GDOPENER_OBSTRUCTIONDETECTED_NO 3
#if GARAGEDOOR_OBSTRUCTIONDETECTED_IND_ENABLE
    #define GARAGEDOOR_OBSTRUCTIONDETECTED_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hObstruction_Detected), ccc_hObstruction_DetectedWriteCB, 0},
    #define GARAGEDOOR_OBSTRUCTIONDETECTED_IND_NO  1
#else
    #define GARAGEDOOR_OBSTRUCTIONDETECTED_IND_DESC
    #define GARAGEDOOR_OBSTRUCTIONDETECTED_IND_NO  0
#endif


#if CHAR_LOCK_CURRENT_STATE_ENABLE
    #define LOCK_CURRENT_STATE_NO   3
    #define CHAR_LOCK_CURRENT_STATE_ATT_TABLE   \
            {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hGarageDoor_LockCurrentState_R_S, 0},    \
            {0,ATT_PERMISSIONS_RDWR,16,1,0, (u8*)(charLockCurrentStateUUID), (u8*)&hGarageDoor_LockCurrentState, 0, GarageDoorLockCurrentStateReadCallback}, \
                {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + GARAGEDOOROPENER_INST_NO + 5), 0, 0}, \
                GARAGEDOOR_LOCKCURRENTSTATE_IND_DESC

    #if GARAGEDOOR_LOCKCURRENTSTATE_IND_ENABLE
        #define GARAGEDOOR_LOCKCURRENTSTATE_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hLock_Current_State), ccc_hLock_Current_StateWriteCB, 0},
        #define GARAGEDOOR_LOCKCURRENTSTATE_IND_NO  1
    #else
        #define GARAGEDOOR_LOCKCURRENTSTATE_IND_DESC
        #define GARAGEDOOR_LOCKCURRENTSTATE_IND_NO  0
    #endif
#else
    #define LOCK_CURRENT_STATE_NO   0
    #define CHAR_LOCK_CURRENT_STATE_ATT_TABLE
    #define GARAGEDOOR_LOCKCURRENTSTATE_IND_NO  0
#endif

#if CHAR_LOCK_TARGET_STATE_ENABLE
    #define LOCK_TARGET_STATE_NO    3
    #define CHAR_LOCK_TARGET_STATE_ATT_TABLE    \
            {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hGarageDoor_LockTargetState_R_S, 0}, \
            {0,ATT_PERMISSIONS_RDWR,16,1,0, (u8*)(charLockTargetStateUUID), (u8*)&hGarageDoor_LockTargetState, GarageDoorLockTargetStateWriteCallback, GarageDoorLockTargetStateReadCallback},   \
                {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + GARAGEDOOROPENER_INST_NO + 6), 0, 0}, \
                GARAGEDOOR_LOCKTARGETSTATE_IND_DESC

    #if GARAGEDOOR_LOCKTARGETSTATE_IND_ENABLE
        #define GARAGEDOOR_LOCKTARGETSTATE_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hLock_Target_State), ccc_hLock_Target_StateWriteCB, 0},
        #define GARAGEDOOR_LOCKTARGETSTATE_IND_NO  1
    #else
        #define GARAGEDOOR_LOCKTARGETSTATE_IND_DESC
        #define GARAGEDOOR_LOCKTARGETSTATE_IND_NO  0
    #endif
#else
    #define LOCK_TARGET_STATE_NO    0
    #define CHAR_LOCK_TARGET_STATE_ATT_TABLE
    #define GARAGEDOOR_LOCKTARGETSTATE_IND_NO  0
#endif


#if CHAR_GARAGEDOOROPENER_SVC_SIGN_ENABLE
#define CHAR_GARAGEDOOROPENER_SVC_A5_SIGN_TABLE   \
    {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_garageDoor_A5_R_S, &hap_desc_service_garageDoor_A5_R_S},  \
    {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_GARAGEDOOROPENER_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_GARAGEDOOROPENER_UNKOWN, 0, 0},    \
        {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + GARAGEDOOROPENER_INST_NO + 7), 0, 0},
#define GARAGEDOOROPENER_SVC_SIGN_NO   3
#else
#define CHAR_GARAGEDOOROPENER_SVC_A5_SIGN_TABLE
#define GARAGEDOOROPENER_SVC_SIGN_NO   0
#endif

#define GARAGEDOOROPENER_NO_DELTA GARAGEDOOR_CURRENTDOORSTATE_IND_NO  \
                            +GARAGEDOOR_TARGETDOORSTATE_IND_NO  \
                            +GARAGEDOOR_OBSTRUCTIONDETECTED_IND_NO  \
                            +GARAGEDOOR_LOCKCURRENTSTATE_IND_NO \
                            +GARAGEDOOR_LOCKTARGETSTATE_IND_NO
#define GARAGEDOOROPENER_NO 15 + LOCK_CURRENT_STATE_NO + LOCK_TARGET_STATE_NO + GARAGEDOOROPENER_SVC_SIGN_NO
#define GARAGEDOOROPENER_HANDLE_NUM (GARAGEDOOROPENER_NO + GARAGEDOOROPENER_NO_DELTA)


#define HOMEKIT_GARAGEDOOROPENER_SERVICE_ATT_TABLE  \
    {GARAGEDOOROPENER_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(garageDoorServiceUUID),sizeof(garageDoorServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(garageDoorServiceUUID), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(insts + GARAGEDOOROPENER_INST_NO), 0, 0},  \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_NAME, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(my_gdoServiceName), 0, (u8*)(nameUUID), (u8*)(my_gdoServiceName), 0, 0},  \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + GARAGEDOOROPENER_INST_NO + 1), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_Current_Door_State_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,1,0, (u8*)(charCurrentDoorStateUUID), (u8*)&Current_Door_State, 0, CurrentDoorStateReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + GARAGEDOOROPENER_INST_NO + 2), 0, 0}, \
            GARAGEDOOR_CURRENTDOORSTATE_IND_DESC \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_Target_Door_State_R_S, 0},   \
        {0,ATT_PERMISSIONS_RDWR,16,1,0, (u8*)(charTargetDoorStateUUID), (u8*)&Target_Door_State, TargetDoorStateWriteCallback, TargetDoorStateReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + GARAGEDOOROPENER_INST_NO + 3), 0, 0}, \
            GARAGEDOOR_TARGETDOORSTATE_IND_DESC \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_Obstruction_Detected_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,1,0, (u8*)(charObstructionDetectedUUID), (u8*)&Obstruction_Detected, 0, ObstructionDetectedReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + GARAGEDOOROPENER_INST_NO + 4), 0, 0}, \
            GARAGEDOOR_OBSTRUCTIONDETECTED_IND_DESC \
        CHAR_LOCK_CURRENT_STATE_ATT_TABLE   \
        CHAR_LOCK_TARGET_STATE_ATT_TABLE    \
        CHAR_GARAGEDOOROPENER_SVC_A5_SIGN_TABLE


#define GDOPENER_HANDLE_COMMON_NUM     8

#define GDOPENER_HANDLE_NUM_CURRENTDOORTSTATE   GARAGEDOOROPENER_HANDLE_START_NUM +GDOPENER_HANDLE_COMMON_NUM
#define GDOPENER_HANDLE_NUM_TARGETDOORSTATE     GDOPENER_HANDLE_NUM_CURRENTDOORTSTATE +GDOPENER_CURRENTDOORTSTATE_NO +GARAGEDOOR_CURRENTDOORSTATE_IND_NO
#define GDOPENER_HANDLE_NUM_OBSTRUCTIONDETECTED GDOPENER_HANDLE_NUM_TARGETDOORSTATE +GDOPENER_TARGETDOORSTATE_NO +GARAGEDOOR_TARGETDOORSTATE_IND_NO
#define GDOPENER_HANDLE_NUM_LOCKCURRENTSTATE    GDOPENER_HANDLE_NUM_OBSTRUCTIONDETECTED +GDOPENER_OBSTRUCTIONDETECTED_NO +GARAGEDOOR_OBSTRUCTIONDETECTED_IND_NO
#define GDOPENER_HANDLE_NUM_LOCKTARGETSTATE     GDOPENER_HANDLE_NUM_LOCKCURRENTSTATE +LOCK_CURRENT_STATE_NO +GARAGEDOOR_LOCKCURRENTSTATE_IND_NO
#define GDOPENER_HANDLE_NUM_THE_END


#define GDO_STORE_CHAR_VALUE_T_GDO_CURRENT_DOOR_STATE               u32 GDO_Current_Door_State:3;   /* 0~4 */
#define GDO_STORE_CHAR_VALUE_T_GDO_CURRENT_DOOR_STATE_STORE             STORE_CHAR(GDO_Current_Door_State,Current_Door_State)
#define GDO_STORE_CHAR_VALUE_T_GDO_CURRENT_DOOR_STATE_RESTORE           RESTORE_CHAR(GDO_Current_Door_State,Current_Door_State)

#define GDO_STORE_CHAR_VALUE_T_GDO_TARGET_DOOR_STATE                u32 GDO_Target_Door_State:1;    /* 0~1 */
#define GDO_STORE_CHAR_VALUE_T_GDO_TARGET_DOOR_STATE_STORE              STORE_CHAR(GDO_Target_Door_State,Target_Door_State)
#define GDO_STORE_CHAR_VALUE_T_GDO_TARGET_DOOR_STATE_RESTORE            RESTORE_CHAR(GDO_Target_Door_State,Target_Door_State)

#define GDO_STORE_CHAR_VALUE_T_GDO_OBSTRUCTION_DETECTED             u32 GDO_Obstruction_Detected:1; /* 0~1 */
#define GDO_STORE_CHAR_VALUE_T_GDO_OBSTRUCTION_DETECTED_STORE           STORE_CHAR(GDO_Obstruction_Detected,Obstruction_Detected)
#define GDO_STORE_CHAR_VALUE_T_GDO_OBSTRUCTION_DETECTED_RESTORE         RESTORE_CHAR(GDO_Obstruction_Detected,Obstruction_Detected)

#define GDO_STORE_CHAR_VALUE_T_GDO_LOCK_CURRENT_STATE               u32 GDO_LOCK_CURRENT_STATE:2;   /* 0~3 */
#define GDO_STORE_CHAR_VALUE_T_GDO_LOCK_CURRENT_STATE_STORE             STORE_CHAR(GDO_LOCK_CURRENT_STATE,hGarageDoor_LockCurrentState)
#define GDO_STORE_CHAR_VALUE_T_GDO_LOCK_CURRENT_STATE_RESTORE           RESTORE_CHAR(GDO_LOCK_CURRENT_STATE,hGarageDoor_LockCurrentState)

#define GDO_STORE_CHAR_VALUE_T_GDO_LOCK_TARGET_STATE                u32 GDO_LOCK_TARGET_STATE:1;    /* 0~1 */
#define GDO_STORE_CHAR_VALUE_T_GDO_LOCK_TARGET_STATE_STORE              STORE_CHAR(GDO_LOCK_TARGET_STATE,hGarageDoor_LockTargetState)
#define GDO_STORE_CHAR_VALUE_T_GDO_LOCK_TARGET_STATE_RESTORE            RESTORE_CHAR(GDO_LOCK_TARGET_STATE,hGarageDoor_LockTargetState)


#define GARAGEDOOROPENER_STORE_CHAR_VALUE_T  \
                    GDO_STORE_CHAR_VALUE_T_GDO_CURRENT_DOOR_STATE   \
                    GDO_STORE_CHAR_VALUE_T_GDO_TARGET_DOOR_STATE    \
                    GDO_STORE_CHAR_VALUE_T_GDO_OBSTRUCTION_DETECTED \
                    GDO_STORE_CHAR_VALUE_T_GDO_LOCK_CURRENT_STATE   \
                    GDO_STORE_CHAR_VALUE_T_GDO_LOCK_TARGET_STATE

#define GARAGEDOOROPENER_STORE_CHAR_TO_FLASH \
                    GDO_STORE_CHAR_VALUE_T_GDO_CURRENT_DOOR_STATE_STORE \
                    GDO_STORE_CHAR_VALUE_T_GDO_TARGET_DOOR_STATE_STORE  \
                    GDO_STORE_CHAR_VALUE_T_GDO_OBSTRUCTION_DETECTED_STORE   \
                    GDO_STORE_CHAR_VALUE_T_GDO_LOCK_CURRENT_STATE_STORE \
                    GDO_STORE_CHAR_VALUE_T_GDO_LOCK_TARGET_STATE_STORE

#define GARAGEDOOROPENER_RESTORE_CHAR_FROM_FLASH \
                    GDO_STORE_CHAR_VALUE_T_GDO_CURRENT_DOOR_STATE_RESTORE   \
                    GDO_STORE_CHAR_VALUE_T_GDO_TARGET_DOOR_STATE_RESTORE    \
                    GDO_STORE_CHAR_VALUE_T_GDO_OBSTRUCTION_DETECTED_RESTORE \
                    GDO_STORE_CHAR_VALUE_T_GDO_LOCK_CURRENT_STATE_RESTORE   \
                    GDO_STORE_CHAR_VALUE_T_GDO_LOCK_TARGET_STATE_RESTORE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN unsigned char Current_Door_State;
EXTERN unsigned char Target_Door_State;
EXTERN unsigned char Obstruction_Detected;
EXTERN unsigned char hGarageDoor_LockCurrentState;
EXTERN unsigned char hGarageDoor_LockTargetState;

EXTERN unsigned short ccc_hCurrent_Door_State;
EXTERN unsigned short ccc_hTarget_Door_State;
EXTERN unsigned short ccc_hObstruction_Detected;
EXTERN unsigned short ccc_hLock_Current_State;
EXTERN unsigned short ccc_hLock_Target_State;

#if GARAGEDOOR_CURRENTDOORSTATE_BRC_ENABLE
    #define BC_PARA_GARAGEDOOR_CURRENTDOORSTATE ((u8*)&bc_para_hCurrent_Door_State)
    EXTERN unsigned char bc_para_hCurrent_Door_State;
#else
    #define BC_PARA_GARAGEDOOR_CURRENTDOORSTATE 0
#endif

#if GARAGEDOOR_TARGETDOORSTATE_BRC_ENABLE
    #define BC_PARA_GARAGEDOOR_TARGETDOORSTATE ((u8*)&bc_para_hTarget_Door_State)
    EXTERN unsigned char bc_para_hTarget_Door_State;
#else
    #define BC_PARA_GARAGEDOOR_TARGETDOORSTATE 0
#endif

#if GARAGEDOOR_OBSTRUCTIONDETECTED_BRC_ENABLE
    #define BC_PARA_GARAGEDOOR_OBSTRUCTIONDETECTED ((u8*)&bc_para_hObstruction_Detected)
    EXTERN unsigned char bc_para_hObstruction_Detected;
#else
    #define BC_PARA_GARAGEDOOR_OBSTRUCTIONDETECTED 0
#endif

#if GARAGEDOOR_LOCKCURRENTSTATE_BRC_ENABLE
    #define BC_PARA_GARAGEDOOR_LOCKCURRENTSTATE ((u8*)&bc_para_hLock_Current_State)
    EXTERN unsigned char bc_para_hLock_Current_State;
#else
    #define BC_PARA_GARAGEDOOR_LOCKCURRENTSTATE 0
#endif

#if GARAGEDOOR_LOCKTARGETSTATE_BRC_ENABLE
    #define BC_PARA_GARAGEDOOR_LOCKTARGETSTATE ((u8*)&bc_para_hLock_Target_State)
    EXTERN unsigned char bc_para_hLock_Target_State;
#else
    #define BC_PARA_GARAGEDOOR_LOCKTARGETSTATE 0
#endif

#if GARAGEDOOROPENER_LINKED_SERVICES_ENABLE
	#define GARAGEDOOROPENER_LINKED_SVCS			hGarageDoorOpener_linked_svcs
	#define GARAGEDOOROPENER_LINKED_SVCS_LENGTH 	(sizeof(hGarageDoorOpener_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * hGarageDoorOpener_linked_svcs[];
#else
	#define GARAGEDOOROPENER_LINKED_SVCS			0
	#define GARAGEDOOROPENER_LINKED_SVCS_LENGTH 	0
#endif

EXTERN const u8 my_gdoServiceName[26];

EXTERN const hap_characteristic_desc_t hap_desc_char_Current_Door_State_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_Target_Door_State_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_Obstruction_Detected_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hGarageDoor_LockCurrentState_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hGarageDoor_LockTargetState_R_S;


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN int CurrentDoorStateReadCallback(void *pp);
EXTERN int TargetDoorStateReadCallback(void *pp);
EXTERN int TargetDoorStateWriteCallback(void *pp);
EXTERN int ObstructionDetectedReadCallback(void *pp);
EXTERN int GarageDoorLockCurrentStateReadCallback(void *pp);
EXTERN int GarageDoorLockTargetStateReadCallback(void *pp);
EXTERN int GarageDoorLockTargetStateWriteCallback(void *pp);
EXTERN int ccc_hCurrent_Door_StateWriteCB(void *pp);
EXTERN int ccc_hTarget_Door_StateWriteCB(void *pp);
EXTERN int ccc_hObstruction_DetectedWriteCB(void *pp);
EXTERN int ccc_hLock_Current_StateWriteCB(void *pp);
EXTERN int ccc_hLock_Target_StateWriteCB(void *pp);

#endif /* _HK_GARAGE_DOOR_OPENER_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
