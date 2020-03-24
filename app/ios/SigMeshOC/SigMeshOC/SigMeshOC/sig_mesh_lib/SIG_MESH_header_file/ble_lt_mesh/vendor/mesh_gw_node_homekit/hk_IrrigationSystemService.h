/********************************************************************************************************
 * @file     hk_IrrigationSystemService.h
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
#ifndef _HK_IRRIGATION_SYSTEM_SERVICE_H_
#define _HK_IRRIGATION_SYSTEM_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_IRRIGATION_SYSTEM_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif

#define CHAR_IRRIGATIONSYSTEM_SVC_SIGN_ENABLE      0
#if CHAR_IRRIGATIONSYSTEM_SVC_SIGN_ENABLE
    #define IRRIGATIONSYSTEM_AS_PRIMARY_SERVICE    		0
    #define IRRIGATIONSYSTEM_IS_HIDDEN_SERVICE     		0
    #define IRRIGATIONSYSTEM_LINKED_SERVICES_ENABLE		0
    EXTERN const hap_characteristic_desc_t hap_desc_char_irrigation_system_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_irrigation_system_A5_R_S;
    EXTERN unsigned char VAR_IRRIGATION_SYSTEM_UNKOWN;
#else
    #define IRRIGATIONSYSTEM_AS_PRIMARY_SERVICE    		0
    #define IRRIGATIONSYSTEM_IS_HIDDEN_SERVICE     		0
    #define IRRIGATIONSYSTEM_LINKED_SERVICES_ENABLE		0
#endif

#define CHAR_IRRIGATIONSYSTEM_REMAININGDURATION_ENABLE			0
#define CHAR_IRRIGATIONSYSTEM_STATUSFAULT_ENABLE				0

#define IRRIGATIONSYSTEM_ACTIVE_IND_ENABLE						0
#define IRRIGATIONSYSTEM_PROGRAMMODE_IND_ENABLE					0
#define IRRIGATIONSYSTEM_INUSE_IND_ENABLE						0
#define IRRIGATIONSYSTEM_REMAININGDURATION_IND_ENABLE			0
#define IRRIGATIONSYSTEM_STATUSFAULT_IND_ENABLE					0

#define IRRIGATIONSYSTEM_ACTIVE_BRC_ENABLE						0
#define IRRIGATIONSYSTEM_PROGRAMMODE_BRC_ENABLE					0
#define IRRIGATIONSYSTEM_INUSE_BRC_ENABLE						0
#define IRRIGATIONSYSTEM_REMAININGDURATION_BRC_ENABLE			0
#define IRRIGATIONSYSTEM_STATUSFAULT_BRC_ENABLE					0


#define IRRIGATIONSYSTEM_ACTIVE_NO      3
#if IRRIGATIONSYSTEM_ACTIVE_IND_ENABLE
    #define IRRIGATIONSYSTEM_ACTIVE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hIrrigationSystemActive), ccc_IrrigationSystemActiveWriteCB, 0},
    #define IRRIGATIONSYSTEM_ACTIVE_IND_NO  1
#else
    #define IRRIGATIONSYSTEM_ACTIVE_IND_DESC
    #define IRRIGATIONSYSTEM_ACTIVE_IND_NO  0
#endif

#define IRRIGATIONSYSTEM_PROGRAMMODE_NO      3
#if IRRIGATIONSYSTEM_PROGRAMMODE_IND_ENABLE
    #define IRRIGATIONSYSTEM_PROGRAMMODE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hIrrigationSystemProgramMode), ccc_IrrigationSystemProgramModeWriteCB, 0},
    #define IRRIGATIONSYSTEM_PROGRAMMODE_IND_NO  1
#else
    #define IRRIGATIONSYSTEM_PROGRAMMODE_IND_DESC
    #define IRRIGATIONSYSTEM_PROGRAMMODE_IND_NO  0
#endif

#define IRRIGATIONSYSTEM_INUSE_NO      3
#if IRRIGATIONSYSTEM_INUSE_IND_ENABLE
    #define IRRIGATIONSYSTEM_INUSE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hIrrigationSystemInUse), ccc_IrrigationSystemInUseWriteCB, 0},
    #define IRRIGATIONSYSTEM_INUSE_IND_NO  1
#else
    #define IRRIGATIONSYSTEM_INUSE_IND_DESC
    #define IRRIGATIONSYSTEM_INUSE_IND_NO  0
#endif

#if CHAR_IRRIGATIONSYSTEM_REMAININGDURATION_ENABLE
    #define IRRIGATIONSYSTEM_REMAININGDURATION_NO 3
    #define CHAR_IRRIGATIONSYSTEM_REMAININGDURATION_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_IRRIGATIONSYSTEM_REMAININGDURATION_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof (hIrrigationSystemRemainingDuration), 0, (u8*)(&charRemainingDurationUUID), (u8*)&hIrrigationSystemRemainingDuration, 0, IrrigationSystemRemainingDurationReadCallback},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + IRRIGATIONSYSTEM_INST_NO + 5), 0, 0}, \
            IRRIGATIONSYSTEM_REMAININGDURATION_IND_DESC

    #if IRRIGATIONSYSTEM_REMAININGDURATION_IND_ENABLE
        #define IRRIGATIONSYSTEM_REMAININGDURATION_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hIrrigationSystemRemainingDuration), ccc_IrrigationSystemRemainingDurationWriteCB, 0},
        #define IRRIGATIONSYSTEM_REMAININGDURATION_IND_NO  1
    #else
        #define IRRIGATIONSYSTEM_REMAININGDURATION_IND_DESC
        #define IRRIGATIONSYSTEM_REMAININGDURATION_IND_NO  0
    #endif
#else
    #define IRRIGATIONSYSTEM_REMAININGDURATION_NO 0
    #define CHAR_IRRIGATIONSYSTEM_REMAININGDURATION_ATT_TABLE
    #define IRRIGATIONSYSTEM_REMAININGDURATION_IND_NO  0
#endif

#if CHAR_IRRIGATIONSYSTEM_STATUSFAULT_ENABLE
    #define IRRIGATIONSYSTEM_STATUSFAULT_NO 3
    #define CHAR_IRRIGATIONSYSTEM_STATUSFAULT_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_IRRIGATIONSYSTEM_STATUSFAULT_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof (hIrrigationSystemStatusFault), 0, (u8*)(&charStatusFaultUUID), (u8*)&hIrrigationSystemStatusFault, 0, IrrigationSystemStatusFaultReadCallback},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + IRRIGATIONSYSTEM_INST_NO + 6), 0, 0}, \
            IRRIGATIONSYSTEM_STATUSFAULT_IND_DESC

    #if IRRIGATIONSYSTEM_STATUSFAULT_IND_ENABLE
        #define IRRIGATIONSYSTEM_STATUSFAULT_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hIrrigationSystemStatusFault), ccc_IrrigationSystemStatusFaultWriteCB, 0},
        #define IRRIGATIONSYSTEM_STATUSFAULT_IND_NO  1
    #else
        #define IRRIGATIONSYSTEM_STATUSFAULT_IND_DESC
        #define IRRIGATIONSYSTEM_STATUSFAULT_IND_NO  0
    #endif
#else
    #define IRRIGATIONSYSTEM_STATUSFAULT_NO 0
    #define CHAR_IRRIGATIONSYSTEM_STATUSFAULT_ATT_TABLE
    #define IRRIGATIONSYSTEM_STATUSFAULT_IND_NO  0
#endif


#if CHAR_IRRIGATIONSYSTEM_SVC_SIGN_ENABLE
    #define IRRIGATIONSYSTEM_SVC_SIGN_NO   3
    #define CHAR_IRRIGATIONSYSTEM_SVC_A5_SIGN_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_irrigation_system_A5_R_S, &hap_desc_service_irrigation_system_A5_R_S},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_IRRIGATION_SYSTEM_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_IRRIGATION_SYSTEM_UNKOWN, 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + IRRIGATIONSYSTEM_INST_NO + 7), 0, 0},
#else
    #define IRRIGATIONSYSTEM_SVC_SIGN_NO   0
    #define CHAR_IRRIGATIONSYSTEM_SVC_A5_SIGN_TABLE
#endif

#define IRRIGATIONSYSTEM_NO_DELTA   IRRIGATIONSYSTEM_ACTIVE_IND_NO \
                                + IRRIGATIONSYSTEM_PROGRAMMODE_IND_NO \
                                + IRRIGATIONSYSTEM_INUSE_IND_NO \
                                + IRRIGATIONSYSTEM_REMAININGDURATION_IND_NO \
                                + IRRIGATIONSYSTEM_STATUSFAULT_IND_NO

#define IRRIGATIONSYSTEM_NO   15 + IRRIGATIONSYSTEM_REMAININGDURATION_NO \
                                + IRRIGATIONSYSTEM_STATUSFAULT_NO \
                                + IRRIGATIONSYSTEM_SVC_SIGN_NO


#define IRRIGATIONSYSTEM_HANDLE_NUM (IRRIGATIONSYSTEM_NO + IRRIGATIONSYSTEM_NO_DELTA)

#define HOMEKIT_IRRIGATIONSYSTEM_SERVICE_ATT_TABLE   \
    {IRRIGATIONSYSTEM_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(irrigation_systemServiceUUID),sizeof(irrigation_systemServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(irrigation_systemServiceUUID), 0, 0},   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(insts + IRRIGATIONSYSTEM_INST_NO), 0, 0},   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_NAME, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(hIrrigationSystemServiceName), 0, (u8*)(nameUUID), (u8*)(hIrrigationSystemServiceName), 0, 0},  \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + IRRIGATIONSYSTEM_INST_NO + 1), 0, 0}, \
		{0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_IRRIGATIONSYSTEM_ACTIVE_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hIrrigationSystemActive),0, (u8*)(charActiveUUID), (u8*)&hIrrigationSystemActive, IrrigationSystemActiveWriteCallback, IrrigationSystemActiveReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + IRRIGATIONSYSTEM_INST_NO + 2), 0, 0},  \
            IRRIGATIONSYSTEM_ACTIVE_IND_DESC \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_IRRIGATIONSYSTEM_PROGRAMMODE_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hIrrigationSystemProgramMode),0, (u8*)(charProgramModeUUID), (u8*)&hIrrigationSystemProgramMode, 0, IrrigationSystemProgramModeReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + IRRIGATIONSYSTEM_INST_NO + 3), 0, 0},  \
            IRRIGATIONSYSTEM_PROGRAMMODE_IND_DESC \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_IRRIGATIONSYSTEM_INUSE_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hIrrigationSystemInUse),0, (u8*)(charInUseUUID), (u8*)&hIrrigationSystemInUse, 0, IrrigationSystemInUseReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + IRRIGATIONSYSTEM_INST_NO + 4), 0, 0},  \
            IRRIGATIONSYSTEM_INUSE_IND_DESC \
        CHAR_IRRIGATIONSYSTEM_REMAININGDURATION_ATT_TABLE \
        CHAR_IRRIGATIONSYSTEM_STATUSFAULT_ATT_TABLE \
        CHAR_IRRIGATIONSYSTEM_SVC_A5_SIGN_TABLE


#define IRRIGATIONSYSTEM_HANDLE_COMMON_NUM     8

#define IRRIGATIONSYSTEM_HANDLE_NUM_ACTIVE					IRRIGATIONSYSTEM_HANDLE_START_NUM +IRRIGATIONSYSTEM_HANDLE_COMMON_NUM
#define IRRIGATIONSYSTEM_HANDLE_NUM_PROGRAMMODE				IRRIGATIONSYSTEM_HANDLE_NUM_ACTIVE +IRRIGATIONSYSTEM_ACTIVE_NO +IRRIGATIONSYSTEM_ACTIVE_IND_NO
#define IRRIGATIONSYSTEM_HANDLE_NUM_INUSE					IRRIGATIONSYSTEM_HANDLE_NUM_PROGRAMMODE +IRRIGATIONSYSTEM_PROGRAMMODE_NO +IRRIGATIONSYSTEM_PROGRAMMODE_IND_NO
#define IRRIGATIONSYSTEM_HANDLE_NUM_REMAININGDURATION       IRRIGATIONSYSTEM_HANDLE_NUM_INUSE +IRRIGATIONSYSTEM_INUSE_NO +IRRIGATIONSYSTEM_INUSE_IND_NO
#define IRRIGATIONSYSTEM_HANDLE_NUM_STATUSFAULT				IRRIGATIONSYSTEM_HANDLE_NUM_REMAININGDURATION +IRRIGATIONSYSTEM_REMAININGDURATION_NO +IRRIGATIONSYSTEM_REMAININGDURATION_IND_NO
#define IRRIGATIONSYSTEM_HANDLE_NUM_THE_END


#define IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T_ACTIVE							u32 IrrigationSystemActive:1;     /* 0~1 */
#define IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T_ACTIVE_STORE					STORE_CHAR(IrrigationSystemActive, hIrrigationSystemActive)
#define IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T_ACTIVE_RESTORE					RESTORE_CHAR(IrrigationSystemActive, hIrrigationSystemActive)

#define IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T_PROGRAMMODE						u32 IrrigationSystemProgramMode:2;    /* 0~2 */
#define IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T_PROGRAMMODE_STORE				STORE_CHAR(IrrigationSystemProgramMode, hIrrigationSystemProgramMode)
#define IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T_PROGRAMMODE_RESTORE				RESTORE_CHAR(IrrigationSystemProgramMode, hIrrigationSystemProgramMode)

#define IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T_INUSE							u32 IrrigationSystemInUse:1;   /* 0~1 */
#define IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T_INUSE_STORE						STORE_CHAR(IrrigationSystemInUse, hIrrigationSystemInUse)
#define IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T_INUSE_RESTORE					RESTORE_CHAR(IrrigationSystemInUse, hIrrigationSystemInUse)

#if CHAR_IRRIGATIONSYSTEM_REMAININGDURATION_ENABLE
#define IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T_REMAININGDURATION				u32 IrrigationSystemRemainingDuration:12;   /* 0~3600 */
#define IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T_REMAININGDURATION_STORE       	STORE_CHAR(IrrigationSystemRemainingDuration, hIrrigationSystemRemainingDuration)
#define IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T_REMAININGDURATION_RESTORE		RESTORE_CHAR(IrrigationSystemRemainingDuration, hIrrigationSystemRemainingDuration)
#else
#define IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T_REMAININGDURATION
#define IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T_REMAININGDURATION_STORE
#define IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T_REMAININGDURATION_RESTORE
#endif

#if CHAR_IRRIGATIONSYSTEM_STATUSFAULT_ENABLE
#define IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T_STATUSFAULT						u32 IrrigationSystemStatusFault:1;   /* 0~1 */
#define IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T_STATUSFAULT_STORE				STORE_CHAR(IrrigationSystemStatusFault, hIrrigationSystemStatusFault)
#define IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T_STATUSFAULT_RESTORE				RESTORE_CHAR(IrrigationSystemStatusFault, hIrrigationSystemStatusFault)
#else
#define IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T_STATUSFAULT
#define IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T_STATUSFAULT_STORE
#define IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T_STATUSFAULT_RESTORE
#endif


#define IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T            \
        IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T_ACTIVE \
        IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T_PROGRAMMODE \
        IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T_INUSE \
        IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T_REMAININGDURATION \
        IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T_STATUSFAULT

#define IRRIGATIONSYSTEM_STORE_CHAR_TO_FLASH           \
        IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T_ACTIVE_STORE \
        IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T_PROGRAMMODE_STORE \
        IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T_INUSE_STORE \
        IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T_REMAININGDURATION_STORE \
        IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T_STATUSFAULT_STORE

#define IRRIGATIONSYSTEM_RESTORE_CHAR_FROM_FLASH            \
        IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T_ACTIVE_RESTORE \
        IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T_PROGRAMMODE_RESTORE \
        IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T_INUSE_RESTORE \
        IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T_REMAININGDURATION_RESTORE \
        IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T_STATUSFAULT_RESTORE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN unsigned char hIrrigationSystemActive;
EXTERN unsigned char hIrrigationSystemProgramMode;
EXTERN unsigned char hIrrigationSystemInUse;
EXTERN unsigned int hIrrigationSystemRemainingDuration;
EXTERN unsigned char hIrrigationSystemStatusFault;

EXTERN unsigned short ccc_hIrrigationSystemActive;
EXTERN unsigned short ccc_hIrrigationSystemProgramMode;
EXTERN unsigned short ccc_hIrrigationSystemInUse;
EXTERN unsigned short ccc_hIrrigationSystemRemainingDuration;
EXTERN unsigned short ccc_hIrrigationSystemStatusFault;


#if IRRIGATIONSYSTEM_ACTIVE_BRC_ENABLE
    #define BC_PARA_IRRIGATIONSYSTEM_ACTIVE ((u8*)&bc_para_hIrrigationSystemActive)
    EXTERN unsigned char bc_para_hIrrigationSystemActive;
#else
    #define BC_PARA_IRRIGATIONSYSTEM_ACTIVE 0
#endif

#if IRRIGATIONSYSTEM_PROGRAMMODE_BRC_ENABLE
    #define BC_PARA_IRRIGATIONSYSTEM_PROGRAMMODE ((u8*)&bc_para_hIrrigationSystemProgramMode)
    EXTERN unsigned char bc_para_hIrrigationSystemProgramMode;
#else
    #define BC_PARA_IRRIGATIONSYSTEM_PROGRAMMODE 0
#endif

#if IRRIGATIONSYSTEM_INUSE_BRC_ENABLE
    #define BC_PARA_IRRIGATIONSYSTEM_INUSE ((u8*)&bc_para_hIrrigationSystemInUse)
    EXTERN unsigned char bc_para_hIrrigationSystemInUse;
#else
    #define BC_PARA_IRRIGATIONSYSTEM_INUSE 0
#endif

#if IRRIGATIONSYSTEM_REMAININGDURATION_BRC_ENABLE
    #define BC_PARA_IRRIGATIONSYSTEM_REMAININGDURATION ((u8*)&bc_para_hIrrigationSystemRemainingDuration)
    EXTERN unsigned char bc_para_hIrrigationSystemRemainingDuration;
#else
    #define BC_PARA_IRRIGATIONSYSTEM_REMAININGDURATION 0
#endif

#if IRRIGATIONSYSTEM_STATUSFAULT_BRC_ENABLE
    #define BC_PARA_IRRIGATIONSYSTEM_STATUSFAULT ((u8*)&bc_para_hIrrigationSystemStatusFault)
    EXTERN unsigned char bc_para_hIrrigationSystemStatusFault;
#else
    #define BC_PARA_IRRIGATIONSYSTEM_STATUSFAULT 0
#endif

#if IRRIGATIONSYSTEM_LINKED_SERVICES_ENABLE
	#define IRRIGATIONSYSTEM_LINKED_SVCS			hIrrigationSystem_linked_svcs
	#define IRRIGATIONSYSTEM_LINKED_SVCS_LENGTH 	(sizeof(hIrrigationSystem_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * hIrrigationSystem_linked_svcs[];
#else
	#define IRRIGATIONSYSTEM_LINKED_SVCS			0
	#define IRRIGATIONSYSTEM_LINKED_SVCS_LENGTH 	0
#endif

EXTERN const u8 hIrrigationSystemServiceName[23];

EXTERN const hap_characteristic_desc_t hap_desc_char_IRRIGATIONSYSTEM_ACTIVE_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_IRRIGATIONSYSTEM_PROGRAMMODE_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_IRRIGATIONSYSTEM_INUSE_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_IRRIGATIONSYSTEM_REMAININGDURATION_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_IRRIGATIONSYSTEM_STATUSFAULT_R_S;


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN int IrrigationSystemActiveReadCallback(void *pp);
EXTERN int IrrigationSystemActiveWriteCallback(void *pp);
EXTERN int IrrigationSystemProgramModeReadCallback(void *pp);
EXTERN int IrrigationSystemInUseReadCallback(void *pp);
EXTERN int IrrigationSystemRemainingDurationReadCallback(void *pp);
EXTERN int IrrigationSystemStatusFaultReadCallback(void *pp);

EXTERN int ccc_IrrigationSystemActiveWriteCB(void *pp);
EXTERN int ccc_IrrigationSystemProgramModeWriteCB(void *pp);
EXTERN int ccc_IrrigationSystemInUseWriteCB(void *pp);
EXTERN int ccc_IrrigationSystemRemainingDurationWriteCB(void *pp);
EXTERN int ccc_IrrigationSystemStatusFaultWriteCB(void *pp);

#endif /* _HK_IRRIGATION_SYSTEM_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
