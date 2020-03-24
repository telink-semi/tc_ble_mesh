/********************************************************************************************************
 * @file     hk_ValveService.h
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
#ifndef _HK_VALVE_SERVICE_H_
#define _HK_VALVE_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_VALVE_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif

#define CHAR_VALVE_SVC_SIGN_ENABLE      0
#if CHAR_VALVE_SVC_SIGN_ENABLE
    #define VALVE_AS_PRIMARY_SERVICE    	0
    #define VALVE_IS_HIDDEN_SERVICE     	0
    #define VALVE_LINKED_SERVICES_ENABLE	0
    EXTERN const hap_characteristic_desc_t hap_desc_char_valve_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_valve_A5_R_S;
    EXTERN unsigned char VAR_VALVE_UNKOWN;
#else
    #define VALVE_AS_PRIMARY_SERVICE    	0
    #define VALVE_IS_HIDDEN_SERVICE     	0
    #define VALVE_LINKED_SERVICES_ENABLE	0
#endif

#define CHAR_VALVE_SETDURATION_ENABLE				0
#define CHAR_VALVE_REMAININGDURATION_ENABLE			0
#define CHAR_VALVE_ISCONFIGURED_ENABLE				0
#define CHAR_VALVE_SERVICELABELINDEX_ENABLE   		0
#define CHAR_VALVE_STATUSFAULT_ENABLE				0

#define VALVE_ACTIVE_IND_ENABLE						0
#define VALVE_INUSE_IND_ENABLE						0
#define VALVE_VALVETYPE_IND_ENABLE					0
#define VALVE_SETDURATION_IND_ENABLE				0
#define VALVE_REMAININGDURATION_IND_ENABLE			0
#define VALVE_ISCONFIGURED_IND_ENABLE				0
#define VALVE_STATUSFAULT_IND_ENABLE				0

#define VALVE_ACTIVE_BRC_ENABLE						0
#define VALVE_INUSE_BRC_ENABLE						0
#define VALVE_VALVETYPE_BRC_ENABLE					0
#define VALVE_SETDURATION_BRC_ENABLE				0
#define VALVE_REMAININGDURATION_BRC_ENABLE			0
#define VALVE_ISCONFIGURED_BRC_ENABLE				0
#define VALVE_STATUSFAULT_BRC_ENABLE				0


#define VALVE_ACTIVE_NO      3
#if VALVE_ACTIVE_IND_ENABLE
    #define VALVE_ACTIVE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hValveActive), ccc_ValveActiveWriteCB, 0},
    #define VALVE_ACTIVE_IND_NO  1
#else
    #define VALVE_ACTIVE_IND_DESC
    #define VALVE_ACTIVE_IND_NO  0
#endif

#define VALVE_INUSE_NO      3
#if VALVE_INUSE_IND_ENABLE
    #define VALVE_INUSE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hValveInUse), ccc_ValveInUseWriteCB, 0},
    #define VALVE_INUSE_IND_NO  1
#else
    #define VALVE_INUSE_IND_DESC
    #define VALVE_INUSE_IND_NO  0
#endif

#define VALVE_VALVETYPE_NO      3
#if VALVE_VALVETYPE_IND_ENABLE
    #define VALVE_VALVETYPE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hValveValveType), ccc_ValveValveTypeWriteCB, 0},
    #define VALVE_VALVETYPE_IND_NO  1
#else
    #define VALVE_VALVETYPE_IND_DESC
    #define VALVE_VALVETYPE_IND_NO  0
#endif


#if CHAR_VALVE_SETDURATION_ENABLE
    #define VALVE_SETDURATION_NO 3
    #define CHAR_VALVE_SETDURATION_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_VALVE_SETDURATION_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof (hValveSetDuration), 0, (u8*)(&charSetDurationUUID), (u8*)&hValveSetDuration, ValveSetDurationWriteCallback, ValveSetDurationReadCallback},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + VALVE_INST_NO + 5), 0, 0}, \
            VALVE_SETDURATION_IND_DESC

    #if VALVE_SETDURATION_IND_ENABLE
        #define VALVE_SETDURATION_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hValveSetDuration), ccc_ValveSetDurationWriteCB, 0},
        #define VALVE_SETDURATION_IND_NO  1
    #else
        #define VALVE_SETDURATION_IND_DESC
        #define VALVE_SETDURATION_IND_NO  0
    #endif
#else
    #define VALVE_SETDURATION_NO 0
    #define CHAR_VALVE_SETDURATION_ATT_TABLE
    #define VALVE_SETDURATION_IND_NO  0
#endif

#if CHAR_VALVE_REMAININGDURATION_ENABLE
    #define VALVE_REMAININGDURATION_NO 3
    #define CHAR_VALVE_REMAININGDURATION_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_VALVE_REMAININGDURATION_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof (hValveRemainingDuration), 0, (u8*)(&charRemainingDurationUUID), (u8*)&hValveRemainingDuration, 0, ValveRemainingDurationReadCallback},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + VALVE_INST_NO + 6), 0, 0}, \
            VALVE_REMAININGDURATION_IND_DESC

    #if VALVE_REMAININGDURATION_IND_ENABLE
        #define VALVE_REMAININGDURATION_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hValveRemainingDuration), ccc_ValveRemainingDurationWriteCB, 0},
        #define VALVE_REMAININGDURATION_IND_NO  1
    #else
        #define VALVE_REMAININGDURATION_IND_DESC
        #define VALVE_REMAININGDURATION_IND_NO  0
    #endif
#else
    #define VALVE_REMAININGDURATION_NO 0
    #define CHAR_VALVE_REMAININGDURATION_ATT_TABLE
    #define VALVE_REMAININGDURATION_IND_NO  0
#endif

#if CHAR_VALVE_ISCONFIGURED_ENABLE
    #define VALVE_ISCONFIGURED_NO 3
    #define CHAR_VALVE_ISCONFIGURED_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_VALVE_ISCONFIGURED_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof (hValveIsConfigured), 0, (u8*)(&charIsConfiguredUUID), (u8*)&hValveIsConfigured, ValveIsConfiguredWriteCallback, ValveIsConfiguredReadCallback},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + VALVE_INST_NO + 7), 0, 0}, \
            VALVE_ISCONFIGURED_IND_DESC

    #if VALVE_ISCONFIGURED_IND_ENABLE
        #define VALVE_ISCONFIGURED_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hValveIsConfigured), ccc_ValveIsConfiguredWriteCB, 0},
        #define VALVE_ISCONFIGURED_IND_NO  1
    #else
        #define VALVE_ISCONFIGURED_IND_DESC
        #define VALVE_ISCONFIGURED_IND_NO  0
    #endif
#else
    #define VALVE_ISCONFIGURED_NO 0
    #define CHAR_VALVE_ISCONFIGURED_ATT_TABLE
    #define VALVE_ISCONFIGURED_IND_NO  0
#endif

#if CHAR_VALVE_SERVICELABELINDEX_ENABLE
    #define VALVE_SERVICELABELINDEX_NO 3
    #define CHAR_VALVE_SERVICELABELINDEX_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_VALVE_SERVICELABELINDEX_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hValveServiceLabelIndex),0, (u8*)(charServiceLabelIndexUUID), (u8*)&hValveServiceLabelIndex, 0, ValveServiceLabelIndexReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + VALVE_INST_NO + 8), 0, 0},
#else
    #define VALVE_SERVICELABELINDEX_NO 0
    #define CHAR_VALVE_SERVICELABELINDEX_ATT_TABLE
#endif


#if CHAR_VALVE_STATUSFAULT_ENABLE
    #define VALVE_STATUSFAULT_NO 3
    #define CHAR_VALVE_STATUSFAULT_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_VALVE_STATUSFAULT_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof (hValveStatusFault), 0, (u8*)(&charStatusFaultUUID), (u8*)&hValveStatusFault, 0, ValveStatusFaultReadCallback},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + VALVE_INST_NO + 9), 0, 0}, \
            VALVE_STATUSFAULT_IND_DESC

    #if VALVE_STATUSFAULT_IND_ENABLE
        #define VALVE_STATUSFAULT_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hValveStatusFault), ccc_ValveStatusFaultWriteCB, 0},
        #define VALVE_STATUSFAULT_IND_NO  1
    #else
        #define VALVE_STATUSFAULT_IND_DESC
        #define VALVE_STATUSFAULT_IND_NO  0
    #endif
#else
    #define VALVE_STATUSFAULT_NO 0
    #define CHAR_VALVE_STATUSFAULT_ATT_TABLE
    #define VALVE_STATUSFAULT_IND_NO  0
#endif


#if CHAR_VALVE_SVC_SIGN_ENABLE
    #define VALVE_SVC_SIGN_NO   3
    #define CHAR_VALVE_SVC_A5_SIGN_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_valve_A5_R_S, &hap_desc_service_valve_A5_R_S},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_VALVE_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_VALVE_UNKOWN, 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + VALVE_INST_NO + 10), 0, 0},
#else
    #define VALVE_SVC_SIGN_NO   0
    #define CHAR_VALVE_SVC_A5_SIGN_TABLE
#endif


#define VALVE_NO_DELTA   VALVE_ACTIVE_IND_NO \
                        + VALVE_INUSE_IND_NO \
                        + VALVE_VALVETYPE_IND_NO \
                        + VALVE_SETDURATION_IND_NO \
                        + VALVE_REMAININGDURATION_IND_NO \
                        + VALVE_ISCONFIGURED_IND_NO \
                        + VALVE_STATUSFAULT_IND_NO


#define VALVE_NO   15 + VALVE_SETDURATION_NO \
	                + VALVE_REMAININGDURATION_NO \
	                + VALVE_ISCONFIGURED_NO \
	                + VALVE_SERVICELABELINDEX_NO \
	                + VALVE_STATUSFAULT_NO \
	                + VALVE_SVC_SIGN_NO


#define VALVE_HANDLE_NUM (VALVE_NO + VALVE_NO_DELTA)


#define HOMEKIT_VALVE_SERVICE_ATT_TABLE   \
    {VALVE_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(valveServiceUUID),sizeof(valveServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(valveServiceUUID), 0, 0},   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(insts + VALVE_INST_NO), 0, 0},   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_NAME, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(hValveServiceName), 0, (u8*)(nameUUID), (u8*)(hValveServiceName), 0, 0},  \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + VALVE_INST_NO + 1), 0, 0}, \
		{0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_VALVE_ACTIVE_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hValveActive),0, (u8*)(charActiveUUID), (u8*)&hValveActive, ValveActiveWriteCallback, ValveActiveReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + VALVE_INST_NO + 2), 0, 0},  \
            VALVE_ACTIVE_IND_DESC \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_VALVE_INUSE_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hValveInUse),0, (u8*)(charInUseUUID), (u8*)&hValveInUse, 0, ValveInUseReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + VALVE_INST_NO + 3), 0, 0},  \
            VALVE_INUSE_IND_DESC \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_VALVE_VALVETYPE_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hValveValveType),0, (u8*)(charValveTypeUUID), (u8*)&hValveValveType, 0, ValveValveTypeReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + VALVE_INST_NO + 4), 0, 0},  \
            VALVE_VALVETYPE_IND_DESC \
        CHAR_VALVE_SETDURATION_ATT_TABLE \
        CHAR_VALVE_REMAININGDURATION_ATT_TABLE \
        CHAR_VALVE_ISCONFIGURED_ATT_TABLE \
        CHAR_VALVE_SERVICELABELINDEX_ATT_TABLE \
        CHAR_VALVE_STATUSFAULT_ATT_TABLE \
        CHAR_VALVE_SVC_A5_SIGN_TABLE


#define VALVE_HANDLE_COMMON_NUM     8

#define VALVE_HANDLE_NUM_ACTIVE					VALVE_HANDLE_START_NUM +VALVE_HANDLE_COMMON_NUM
#define VALVE_HANDLE_NUM_INUSE					VALVE_HANDLE_NUM_ACTIVE +VALVE_ACTIVE_NO +VALVE_ACTIVE_IND_NO
#define VALVE_HANDLE_NUM_VALVETYPE				VALVE_HANDLE_NUM_INUSE +VALVE_INUSE_NO +VALVE_INUSE_IND_NO
#define VALVE_HANDLE_NUM_SETDURATION			VALVE_HANDLE_NUM_VALVETYPE +VALVE_VALVETYPE_NO +VALVE_VALVETYPE_IND_NO
#define VALVE_HANDLE_NUM_REMAININGDURATION		VALVE_HANDLE_NUM_SETDURATION +VALVE_SETDURATION_NO +VALVE_SETDURATION_IND_NO
#define VALVE_HANDLE_NUM_ISCONFIGURED			VALVE_HANDLE_NUM_REMAININGDURATION +VALVE_REMAININGDURATION_NO +VALVE_REMAININGDURATION_IND_NO
#define VALVE_HANDLE_NUM_SERVICELABELINDEX		VALVE_HANDLE_NUM_ISCONFIGURED +VALVE_ISCONFIGURED_NO +VALVE_ISCONFIGURED_IND_NO
#define VALVE_HANDLE_NUM_STATUSFAULT			VALVE_HANDLE_NUM_SERVICELABELINDEX +VALVE_SERVICELABELINDEX_NO +0
#define VALVE_HANDLE_NUM_THE_END


#define VALVE_STORE_CHAR_VALUE_T_ACTIVE							u32 ValveActive:1;     /* 0~1 */
#define VALVE_STORE_CHAR_VALUE_T_ACTIVE_STORE					STORE_CHAR(ValveActive, hValveActive)
#define VALVE_STORE_CHAR_VALUE_T_ACTIVE_RESTORE					RESTORE_CHAR(ValveActive, hValveActive)

#define VALVE_STORE_CHAR_VALUE_T_INUSE							u32 ValveInUse:1;   /* 0~1 */
#define VALVE_STORE_CHAR_VALUE_T_INUSE_STORE					STORE_CHAR(ValveInUse, hValveInUse)
#define VALVE_STORE_CHAR_VALUE_T_INUSE_RESTORE					RESTORE_CHAR(ValveInUse, hValveInUse)

#define VALVE_STORE_CHAR_VALUE_T_VALVETYPE						u32 ValveValveType:2;    /* 0~3 */
#define VALVE_STORE_CHAR_VALUE_T_VALVETYPE_STORE				STORE_CHAR(ValveValveType, hValveValveType)
#define VALVE_STORE_CHAR_VALUE_T_VALVETYPE_RESTORE				RESTORE_CHAR(ValveValveType, hValveValveType)

#if CHAR_VALVE_SETDURATION_ENABLE
#define VALVE_STORE_CHAR_VALUE_T_SETDURATION					u32 ValveSetDuration:12;   /* 0~3600 */
#define VALVE_STORE_CHAR_VALUE_T_SETDURATION_STORE       		STORE_CHAR(ValveSetDuration, hValveSetDuration)
#define VALVE_STORE_CHAR_VALUE_T_SETDURATION_RESTORE			RESTORE_CHAR(ValveSetDuration, hValveSetDuration)
#else
#define VALVE_STORE_CHAR_VALUE_T_SETDURATION
#define VALVE_STORE_CHAR_VALUE_T_SETDURATION_STORE
#define VALVE_STORE_CHAR_VALUE_T_SETDURATION_RESTORE
#endif

#if CHAR_VALVE_REMAININGDURATION_ENABLE
#define VALVE_STORE_CHAR_VALUE_T_REMAININGDURATION				u32 ValveRemainingDuration:12;   /* 0~3600 */
#define VALVE_STORE_CHAR_VALUE_T_REMAININGDURATION_STORE       	STORE_CHAR(ValveRemainingDuration, hValveRemainingDuration)
#define VALVE_STORE_CHAR_VALUE_T_REMAININGDURATION_RESTORE		RESTORE_CHAR(ValveRemainingDuration, hValveRemainingDuration)
#else
#define VALVE_STORE_CHAR_VALUE_T_REMAININGDURATION
#define VALVE_STORE_CHAR_VALUE_T_REMAININGDURATION_STORE
#define VALVE_STORE_CHAR_VALUE_T_REMAININGDURATION_RESTORE
#endif

#if CHAR_VALVE_ISCONFIGURED_ENABLE
#define VALVE_STORE_CHAR_VALUE_T_ISCONFIGURED					u32 ValveIsConfigured:1;   /* 0~1 */
#define VALVE_STORE_CHAR_VALUE_T_ISCONFIGURED_STORE       		STORE_CHAR(ValveIsConfigured, hValveIsConfigured)
#define VALVE_STORE_CHAR_VALUE_T_ISCONFIGURED_RESTORE			RESTORE_CHAR(ValveIsConfigured, hValveIsConfigured)
#else
#define VALVE_STORE_CHAR_VALUE_T_ISCONFIGURED
#define VALVE_STORE_CHAR_VALUE_T_ISCONFIGURED_STORE
#define VALVE_STORE_CHAR_VALUE_T_ISCONFIGURED_RESTORE
#endif

#if CHAR_VALVE_SERVICELABELINDEX_ENABLE
#define VALVE_STORE_CHAR_VALUE_T_SERVICELABELINDEX                          u32 ValveServiceLabelIndex:8; /* Min Value is 1 */
#define VALVE_STORE_CHAR_VALUE_T_SERVICELABELINDEX_STORE                        STORE_CHAR(ValveServiceLabelIndex,hValveServiceLabelIndex)
#define VALVE_STORE_CHAR_VALUE_T_SERVICELABELINDEX_RESTORE                      RESTORE_CHAR(ValveServiceLabelIndex,hValveServiceLabelIndex)
#else
#define VALVE_STORE_CHAR_VALUE_T_SERVICELABELINDEX
#define VALVE_STORE_CHAR_VALUE_T_SERVICELABELINDEX_STORE
#define VALVE_STORE_CHAR_VALUE_T_SERVICELABELINDEX_RESTORE
#endif

#if CHAR_VALVE_STATUSFAULT_ENABLE
#define VALVE_STORE_CHAR_VALUE_T_STATUSFAULT					u32 ValveStatusFault:1;   /* 0~1 */
#define VALVE_STORE_CHAR_VALUE_T_STATUSFAULT_STORE				STORE_CHAR(ValveStatusFault, hValveStatusFault)
#define VALVE_STORE_CHAR_VALUE_T_STATUSFAULT_RESTORE			RESTORE_CHAR(ValveStatusFault, hValveStatusFault)
#else
#define VALVE_STORE_CHAR_VALUE_T_STATUSFAULT
#define VALVE_STORE_CHAR_VALUE_T_STATUSFAULT_STORE
#define VALVE_STORE_CHAR_VALUE_T_STATUSFAULT_RESTORE
#endif


#define VALVE_STORE_CHAR_VALUE_T            \
        VALVE_STORE_CHAR_VALUE_T_ACTIVE \
        VALVE_STORE_CHAR_VALUE_T_INUSE \
        VALVE_STORE_CHAR_VALUE_T_VALVETYPE \
        VALVE_STORE_CHAR_VALUE_T_SETDURATION \
        VALVE_STORE_CHAR_VALUE_T_REMAININGDURATION \
        VALVE_STORE_CHAR_VALUE_T_ISCONFIGURED \
        VALVE_STORE_CHAR_VALUE_T_SERVICELABELINDEX \
        VALVE_STORE_CHAR_VALUE_T_STATUSFAULT

#define VALVE_STORE_CHAR_TO_FLASH            \
        VALVE_STORE_CHAR_VALUE_T_ACTIVE_STORE \
        VALVE_STORE_CHAR_VALUE_T_INUSE_STORE \
        VALVE_STORE_CHAR_VALUE_T_VALVETYPE_STORE \
        VALVE_STORE_CHAR_VALUE_T_SETDURATION_STORE \
        VALVE_STORE_CHAR_VALUE_T_REMAININGDURATION_STORE \
        VALVE_STORE_CHAR_VALUE_T_ISCONFIGURED_STORE \
        VALVE_STORE_CHAR_VALUE_T_SERVICELABELINDEX_STORE \
        VALVE_STORE_CHAR_VALUE_T_STATUSFAULT_STORE

#define VALVE_RESTORE_CHAR_FROM_FLASH            \
        VALVE_STORE_CHAR_VALUE_T_ACTIVE_RESTORE \
        VALVE_STORE_CHAR_VALUE_T_INUSE_RESTORE \
        VALVE_STORE_CHAR_VALUE_T_VALVETYPE_RESTORE \
        VALVE_STORE_CHAR_VALUE_T_SETDURATION_RESTORE \
        VALVE_STORE_CHAR_VALUE_T_REMAININGDURATION_RESTORE \
        VALVE_STORE_CHAR_VALUE_T_ISCONFIGURED_RESTORE \
        VALVE_STORE_CHAR_VALUE_T_SERVICELABELINDEX_RESTORE \
        VALVE_STORE_CHAR_VALUE_T_STATUSFAULT_RESTORE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN unsigned char hValveActive;
EXTERN unsigned char hValveInUse;
EXTERN unsigned char hValveValveType;
EXTERN unsigned int hValveSetDuration;
EXTERN unsigned int hValveRemainingDuration;
EXTERN unsigned char hValveIsConfigured;
EXTERN unsigned char hValveServiceLabelIndex;
EXTERN unsigned char hValveStatusFault;

	
EXTERN unsigned short ccc_hValveActive;
EXTERN unsigned short ccc_hValveInUse;
EXTERN unsigned short ccc_hValveValveType;
EXTERN unsigned short ccc_hValveSetDuration;
EXTERN unsigned short ccc_hValveRemainingDuration;
EXTERN unsigned short ccc_hValveIsConfigured;
EXTERN unsigned short ccc_hValveStatusFault;


#if VALVE_ACTIVE_BRC_ENABLE
    #define BC_PARA_VALVE_ACTIVE ((u8*)&bc_para_hValveActive)
    EXTERN unsigned char bc_para_hValveActive;
#else
    #define BC_PARA_VALVE_ACTIVE 0
#endif

#if VALVE_INUSE_BRC_ENABLE
    #define BC_PARA_VALVE_INUSE ((u8*)&bc_para_hValveInUse)
    EXTERN unsigned char bc_para_hValveInUse;
#else
    #define BC_PARA_VALVE_INUSE 0
#endif

#if VALVE_VALVETYPE_BRC_ENABLE
    #define BC_PARA_VALVE_VALVETYPE ((u8*)&bc_para_hValveValveType)
    EXTERN unsigned char bc_para_hValveValveType;
#else
    #define BC_PARA_VALVE_VALVETYPE 0
#endif

#if VALVE_SETDURATION_BRC_ENABLE
    #define BC_PARA_VALVE_SETDURATION ((u8*)&bc_para_hValveSetDuration)
    EXTERN unsigned char bc_para_hValveSetDuration;
#else
    #define BC_PARA_VALVE_SETDURATION 0
#endif

#if VALVE_REMAININGDURATION_BRC_ENABLE
    #define BC_PARA_VALVE_REMAININGDURATION ((u8*)&bc_para_hValveRemainingDuration)
    EXTERN unsigned char bc_para_hValveRemainingDuration;
#else
    #define BC_PARA_VALVE_REMAININGDURATION 0
#endif

#if VALVE_ISCONFIGURED_BRC_ENABLE
    #define BC_PARA_VALVE_ISCONFIGURED ((u8*)&bc_para_hValveIsConfigured)
    EXTERN unsigned char bc_para_hValveIsConfigured;
#else
    #define BC_PARA_VALVE_ISCONFIGURED 0
#endif

#if VALVE_STATUSFAULT_BRC_ENABLE
    #define BC_PARA_VALVE_STATUSFAULT ((u8*)&bc_para_hValveStatusFault)
    EXTERN unsigned char bc_para_hValveStatusFault;
#else
    #define BC_PARA_VALVE_STATUSFAULT 0
#endif

#if VALVE_LINKED_SERVICES_ENABLE
	#define VALVE_LINKED_SVCS			hValve_linked_svcs
	#define VALVE_LINKED_SVCS_LENGTH 	(sizeof(hValve_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * hValve_linked_svcs[];
#else
	#define VALVE_LINKED_SVCS			0
	#define VALVE_LINKED_SVCS_LENGTH 	0
#endif

EXTERN const u8 hValveServiceName[12];


EXTERN const hap_characteristic_desc_t hap_desc_char_VALVE_ACTIVE_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_VALVE_INUSE_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_VALVE_VALVETYPE_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_VALVE_SETDURATION_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_VALVE_REMAININGDURATION_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_VALVE_ISCONFIGURED_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_VALVE_SERVICELABELINDEX_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_VALVE_STATUSFAULT_R_S;


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN int ValveActiveReadCallback(void *pp);
EXTERN int ValveActiveWriteCallback(void *pp);
EXTERN int ValveInUseReadCallback(void *pp);
EXTERN int ValveValveTypeReadCallback(void *pp);
EXTERN int ValveSetDurationReadCallback(void *pp);
EXTERN int ValveSetDurationWriteCallback(void *pp);
EXTERN int ValveRemainingDurationReadCallback(void *pp);
EXTERN int ValveIsConfiguredReadCallback(void *pp);
EXTERN int ValveIsConfiguredWriteCallback(void *pp);
EXTERN int ValveServiceLabelIndexReadCallback(void *pp);
EXTERN int ValveStatusFaultReadCallback(void *pp);


EXTERN int ccc_ValveActiveWriteCB(void *pp);
EXTERN int ccc_ValveInUseWriteCB(void *pp);
EXTERN int ccc_ValveValveTypeWriteCB(void *pp);
EXTERN int ccc_ValveSetDurationWriteCB(void *pp);
EXTERN int ccc_ValveRemainingDurationWriteCB(void *pp);
EXTERN int ccc_ValveIsConfiguredWriteCB(void *pp);
EXTERN int ccc_ValveStatusFaultWriteCB(void *pp);

#endif /* _HK_VALVE_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
