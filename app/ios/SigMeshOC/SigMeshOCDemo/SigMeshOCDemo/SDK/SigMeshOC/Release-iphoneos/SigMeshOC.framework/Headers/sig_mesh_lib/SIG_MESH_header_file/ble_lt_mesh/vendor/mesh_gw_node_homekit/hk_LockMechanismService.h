/********************************************************************************************************
 * @file     hk_LockMechanismService.h
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
#ifndef _HK_LOCK_MECHANISM_SERVICE_H_
#define _HK_LOCK_MECHANISM_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */
//-#include "../../homekit_src/homekit_inc.h"


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_LOCK_MECHANISM_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif


#define CHAR_LOCKMECHANISM_SVC_SIGN_ENABLE      0
#if CHAR_LOCKMECHANISM_SVC_SIGN_ENABLE
    #define LOCKMECHANISM_AS_PRIMARY_SERVICE    	0
    #define LOCKMECHANISM_IS_HIDDEN_SERVICE     	0
    #define LOCKMECHANISM_LINKED_SERVICES_ENABLE	0
    EXTERN const hap_characteristic_desc_t hap_desc_char_lockMechanism_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_lockMechanism_A5_R_S;
    EXTERN unsigned char VAR_LOCKMECHANISM_UNKOWN;
#else
    #define LOCKMECHANISM_AS_PRIMARY_SERVICE    	0
    #define LOCKMECHANISM_IS_HIDDEN_SERVICE     	0
    #define LOCKMECHANISM_LINKED_SERVICES_ENABLE	0
#endif


#define LOCKMECHANISM_LOCKCURRENTSTATE_IND_ENABLE   1
#define LOCKMECHANISM_LOCKTARGETSTATE_IND_ENABLE    1

#define LOCKMECHANISM_LOCKCURRENTSTATE_BRC_ENABLE   0
#define LOCKMECHANISM_LOCKTARGETSTATE_BRC_ENABLE    0


#define DOORLOCK_LED_DEMO_ENABLE    1       //1: Enable Demo with LED_White (default); 0: Enable Real Doorlock Function

#if !DOORLOCK_LED_DEMO_ENABLE
    #define LOCKING_DIRECTION       0       //0: Counter-Clockwise Locking (Default); 1: Clockwise Locking
    #define DOORLOCK_DEBUG_ENABLE   0       //0: Disable Debugged Parameters

    /*****DoorLock Type*****/
    #define TYPE_DEADBOLT   0
    #define TYPE_JIMMYPROOF 1
    #define TYPE_Rim        2

    #define DOORLOCK_TYPE   TYPE_DEADBOLT   //Choose the DoorLock Type here
    /**********************/

#endif


#define LOCKMECHANISM_LOCKCURRENTSTATE_NO 3
#if LOCKMECHANISM_LOCKCURRENTSTATE_IND_ENABLE
    #define LOCKMECHANISM_LOCKCURRENTSTATE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hLockMechanism_Lock_Current_State), ccc_hLockMechanism_Lock_Current_StateWriteCB, 0},
    #define LOCKMECHANISM_LOCKCURRENTSTATE_IND_NO  1
#else
    #define LOCKMECHANISM_LOCKCURRENTSTATE_IND_DESC
    #define LOCKMECHANISM_LOCKCURRENTSTATE_IND_NO  0
#endif

#define LOCKMECHANISM_LOCKTARGETSTATE_NO 3
#if LOCKMECHANISM_LOCKTARGETSTATE_IND_ENABLE
    #define LOCKMECHANISM_LOCKTARGETSTATE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hLockMechanism_Lock_Target_State), ccc_hLockMechanism_Lock_Target_StateWriteCB, 0},
    #define LOCKMECHANISM_LOCKTARGETSTATE_IND_NO  1
#else
    #define LOCKMECHANISM_LOCKTARGETSTATE_IND_DESC
    #define LOCKMECHANISM_LOCKTARGETSTATE_IND_NO  0
#endif

#if CHAR_LOCKMECHANISM_SVC_SIGN_ENABLE
#define CHAR_LOCKMECHANISM_SVC_A5_SIGN_TABLE   \
    {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_lockMechanism_A5_R_S, &hap_desc_service_lockMechanism_A5_R_S},  \
    {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_LOCKMECHANISM_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_LOCKMECHANISM_UNKOWN, 0, 0},    \
        {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + LOCKMECHANISM_INST_NO + 4), 0, 0},
#define LOCKMECHANISM_SVC_SIGN_NO   3
#else
#define CHAR_LOCKMECHANISM_SVC_A5_SIGN_TABLE
#define LOCKMECHANISM_SVC_SIGN_NO   0
#endif


#define LOCKMECHANISM_NO_DELTA  LOCKMECHANISM_LOCKCURRENTSTATE_IND_NO   \
                                +LOCKMECHANISM_LOCKTARGETSTATE_IND_NO
#define LOCKMECHANISM_NO        12 +LOCKMECHANISM_SVC_SIGN_NO
#define LOCKMECHANISM_HANDLE_NUM (LOCKMECHANISM_NO + LOCKMECHANISM_NO_DELTA)


#define HOMEKIT_LOCKMECHANISM_SERVICE_ATT_TABLE \
    {LOCKMECHANISM_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(lockMechanismServiceUUID),sizeof(lockMechanismServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(lockMechanismServiceUUID), 0, 0}, \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(insts + LOCKMECHANISM_INST_NO), 0, 0}, \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_NAME, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(my_lockmechanismServiceName), 0, (u8*)(nameUUID), (u8*)(my_lockmechanismServiceName), 0, 0},  \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + LOCKMECHANISM_INST_NO + 1), 0, 0},   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_Lock_Current_State_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,1,0, (u8*)(charLockCurrentStateUUID), (u8*)&Lock_Current_State, 0, LockCurrentStateReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + LOCKMECHANISM_INST_NO + 2), 0, 0},    \
            LOCKMECHANISM_LOCKCURRENTSTATE_IND_DESC \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_Lock_Target_State_R_S, 0},   \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(Lock_Target_State),0, (u8*)(charLockTargetStateUUID), (u8*)&Lock_Target_State, LockTargetStateWriteCallback, LockTargetStateReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + LOCKMECHANISM_INST_NO + 3), 0, 0},    \
            LOCKMECHANISM_LOCKTARGETSTATE_IND_DESC \
        CHAR_LOCKMECHANISM_SVC_A5_SIGN_TABLE


#define LOCKMECHANISM_HANDLE_COMMON_NUM     8

#define LOCKMECHANISM_HANDLE_NUM_LOCKCURRENTSTATE   LOCKMECHANISM_HANDLE_START_NUM +LOCKMECHANISM_HANDLE_COMMON_NUM
#define LOCKMECHANISM_HANDLE_NUM_LOCKTARGETSTATE    LOCKMECHANISM_HANDLE_NUM_LOCKCURRENTSTATE +LOCKMECHANISM_LOCKCURRENTSTATE_NO +LOCKMECHANISM_LOCKCURRENTSTATE_IND_NO
#define LOCKMECHANISM_HANDLE_NUM_THE_END


#define LOCKMECHANISM_STORE_CHAR_VALUE_T_LOCK_CURRENT_STATE           u32 LockCurrentState:2;   /* 0~3 */
#define LOCKMECHANISM_STORE_CHAR_VALUE_T_LOCK_CURRENT_STATE_STORE         STORE_CHAR(LockCurrentState,Lock_Current_State)
#define LOCKMECHANISM_STORE_CHAR_VALUE_T_LOCK_CURRENT_STATE_RESTORE       RESTORE_CHAR(LockCurrentState,Lock_Current_State)

#define LOCKMECHANISM_STORE_CHAR_VALUE_T_LOCK_TARGET_STATE            u32 LockTargetState:1;    /* 0~1 */
#define LOCKMECHANISM_STORE_CHAR_VALUE_T_LOCK_TARGET_STATE_STORE          STORE_CHAR(LockTargetState,Lock_Target_State)
#define LOCKMECHANISM_STORE_CHAR_VALUE_T_LOCK_TARGET_STATE_RESTORE        RESTORE_CHAR(LockTargetState,Lock_Target_State)

#define LOCKMECHANISM_STORE_CHAR_VALUE_T            \
                    LOCKMECHANISM_STORE_CHAR_VALUE_T_LOCK_CURRENT_STATE \
                    LOCKMECHANISM_STORE_CHAR_VALUE_T_LOCK_TARGET_STATE

#define LOCKMECHANISM_STORE_CHAR_TO_FLASH           \
                    LOCKMECHANISM_STORE_CHAR_VALUE_T_LOCK_CURRENT_STATE_STORE   \
                    LOCKMECHANISM_STORE_CHAR_VALUE_T_LOCK_TARGET_STATE_STORE

#define LOCKMECHANISM_RESTORE_CHAR_FROM_FLASH       \
                    LOCKMECHANISM_STORE_CHAR_VALUE_T_LOCK_CURRENT_STATE_RESTORE \
                    LOCKMECHANISM_STORE_CHAR_VALUE_T_LOCK_TARGET_STATE_RESTORE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN unsigned char Lock_Current_State;
EXTERN unsigned char Lock_Target_State;

EXTERN unsigned short ccc_hLockMechanism_Lock_Current_State;
EXTERN unsigned short ccc_hLockMechanism_Lock_Target_State;

#if LOCKMECHANISM_LOCKCURRENTSTATE_BRC_ENABLE
    #define BC_PARA_LOCKMECHANISM_LOCKCURRENTSTATE ((u8*)&bc_para_hLockMechanism_Lock_Current_State)
    EXTERN unsigned char bc_para_hLockMechanism_Lock_Current_State;
#else
    #define BC_PARA_LOCKMECHANISM_LOCKCURRENTSTATE 0
#endif

#if LOCKMECHANISM_LOCKTARGETSTATE_BRC_ENABLE
    #define BC_PARA_LOCKMECHANISM_LOCKTARGETSTATE ((u8*)&bc_para_hLockMechanism_Lock_Target_State)
    EXTERN unsigned char bc_para_hLockMechanism_Lock_Target_State;
#else
    #define BC_PARA_LOCKMECHANISM_LOCKTARGETSTATE 0
#endif

#if LOCKMECHANISM_LINKED_SERVICES_ENABLE
	#define LOCKMECHANISM_LINKED_SVCS			hLockMechanism_linked_svcs
	#define LOCKMECHANISM_LINKED_SVCS_LENGTH 	(sizeof(hLockMechanism_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * hLockMechanism_linked_svcs[];
#else
	#define LOCKMECHANISM_LINKED_SVCS			0
	#define LOCKMECHANISM_LINKED_SVCS_LENGTH 	0
#endif

EXTERN u32 LockRunningTime;
EXTERN unsigned char Doorlock_Proc_Flg;

EXTERN const u8 my_lockmechanismServiceName[27];

EXTERN const hap_characteristic_desc_t hap_desc_char_Lock_Current_State_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_Lock_Target_State_R_S;


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN int LockCurrentStateReadCallback(void *pp);
EXTERN int LockTargetStateReadCallback (void *pp);
EXTERN int LockTargetStateWriteCallback(void *pp);
EXTERN int ccc_hLockMechanism_Lock_Current_StateWriteCB(void *pp);
EXTERN int ccc_hLockMechanism_Lock_Target_StateWriteCB(void *pp);
EXTERN void doorlock_proc(void);
EXTERN void lock_init(void);

#endif /* _HK_LOCK_MECHANISM_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
