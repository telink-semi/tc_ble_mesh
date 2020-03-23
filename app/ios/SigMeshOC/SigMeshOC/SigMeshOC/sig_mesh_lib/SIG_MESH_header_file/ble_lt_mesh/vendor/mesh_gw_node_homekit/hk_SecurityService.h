/********************************************************************************************************
 * @file     hk_SecurityService.h
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
#ifndef _HK_SECURITY_SERVICE_H_
#define _HK_SECURITY_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_SECURITY_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif


#define CHAR_SECURITY_SVC_SIGN_ENABLE      0
#if CHAR_SECURITY_SVC_SIGN_ENABLE
    #define SECURITY_AS_PRIMARY_SERVICE    		0
    #define SECURITY_IS_HIDDEN_SERVICE     		0
    #define SECURITY_LINKED_SERVICES_ENABLE		0
    EXTERN const hap_characteristic_desc_t hap_desc_char_security_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_security_A5_R_S;
    EXTERN unsigned char VAR_SECURITY_UNKOWN;
#else
    #define SECURITY_AS_PRIMARY_SERVICE    		0
    #define SECURITY_IS_HIDDEN_SERVICE     		0
    #define SECURITY_LINKED_SERVICES_ENABLE		0
#endif

#define CHAR_SECURITY_ALARMTYPE_ENABLE       0
#define CHAR_SECURITY_STATUSFAULT_ENABLE     0
#define CHAR_SECURITY_STATUSTAMPERED_ENABLE  0


#define SECURITY_CURRENTSTATE_IND_ENABLE    0
#define SECURITY_TARGETSTATE_IND_ENABLE     0
#define SECURITY_ALARMTYPE_IND_ENABLE       0
#define SECURITY_STATUSFAULT_IND_ENABLE     0
#define SECURITY_STATUSTAMPERED_IND_ENABLE  0

#define SECURITY_CURRENTSTATE_BRC_ENABLE    0
#define SECURITY_TARGETSTATE_BRC_ENABLE     0
#define SECURITY_ALARMTYPE_BRC_ENABLE       0
#define SECURITY_STATUSFAULT_BRC_ENABLE     0
#define SECURITY_STATUSTAMPERED_BRC_ENABLE  0


#define SECURITY_CURRENTSTATE_NO 3
#if SECURITY_CURRENTSTATE_IND_ENABLE
    #define SECURITY_CURRENTSTATE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSecurityCurrentState), ccc_hSecurityCurrentStateWriteCB, 0},
    #define SECURITY_CURRENTSTATE_IND_NO  1
#else
    #define SECURITY_CURRENTSTATE_IND_DESC
    #define SECURITY_CURRENTSTATE_IND_NO  0
#endif

#define SECURITY_TARGETSTATE_NO 3
#if SECURITY_TARGETSTATE_IND_ENABLE
    #define SECURITY_TARGETSTATE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSecurityTargetState), ccc_hSecurityTargetStateWriteCB, 0},
    #define SECURITY_TARGETSTATE_IND_NO  1
#else
    #define SECURITY_TARGETSTATE_IND_DESC
    #define SECURITY_TARGETSTATE_IND_NO  0
#endif

#if CHAR_SECURITY_ALARMTYPE_ENABLE
	#define SECURITY_ALARMTYPE_NO 3
	#define CHAR_SECURITY_ALARMTYPE_ATT_TABLE	\
		{0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSecurityAlarmType_R_S, 0},	\
		{0,ATT_PERMISSIONS_RDWR,16,sizeof(hSecurityAlarmType),0, (u8*)(charSecurityAlarmTypeUUID), (u8*)&hSecurityAlarmType, 0, hSecurityAlarmTypeReadCallback}, \
			{0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SECURITY_INST_NO + 4), 0, 0}, \
			SECURITY_ALARMTYPE_IND_DESC
			
		#if SECURITY_ALARMTYPE_IND_ENABLE
			#define SECURITY_ALARMTYPE_IND_DESC	\
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSecurityAlarmType), ccc_hSecurityAlarmTypeWriteCB, 0},
			#define SECURITY_ALARMTYPE_IND_NO  1
		#else
			#define SECURITY_ALARMTYPE_IND_DESC
			#define SECURITY_ALARMTYPE_IND_NO  0
		#endif
#else
	#define SECURITY_ALARMTYPE_NO 0
	#define CHAR_SECURITY_ALARMTYPE_ATT_TABLE
	#define SECURITY_ALARMTYPE_IND_NO  0
#endif

#if CHAR_SECURITY_STATUSFAULT_ENABLE
	#define SECURITY_STATUSFAULT_NO 3
	#define CHAR_SECURITY_STATUSFAULT_ATT_TABLE	\
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSecurityStatusFault_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSecurityStatusFault),0, (u8*)(charStatusFaultUUID), (u8*)&hSecurityStatusFault, 0, hSecurityStatusFaultReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SECURITY_INST_NO + 5), 0, 0}, \
			SECURITY_STATUSFAULT_IND_DESC
			
	#if SECURITY_STATUSFAULT_IND_ENABLE
	    #define SECURITY_STATUSFAULT_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSecurityStatusFault), ccc_hSecurityStatusFaultWriteCB, 0},
	    #define SECURITY_STATUSFAULT_IND_NO  1
	#else
	    #define SECURITY_STATUSFAULT_IND_DESC
	    #define SECURITY_STATUSFAULT_IND_NO  0
	#endif
#else
	#define SECURITY_STATUSFAULT_NO 0
	#define CHAR_SECURITY_STATUSFAULT_ATT_TABLE
	#define SECURITY_STATUSFAULT_IND_NO  0
#endif

#if CHAR_SECURITY_STATUSTAMPERED_ENABLE
	#define SECURITY_STATUSTAMPERED_NO 3
	#define CHAR_SECURITY_STATUSTAMPERED_ATT_TABLE	\
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSecurityStatusTampered_R_S, 0}, \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSecurityStatusTampered),0, (u8*)(charStatusTamperedUUID), (u8*)&hSecurityStatusTampered, 0, hSecurityStatusTamperedReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SECURITY_INST_NO + 6), 0, 0}, \
			SECURITY_STATUSTAMPERED_IND_DESC
			
	#if SECURITY_STATUSTAMPERED_IND_ENABLE
	    #define SECURITY_STATUSTAMPERED_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSecurityStatusTampered), ccc_hSecurityStatusTamperedWriteCB, 0},
	    #define SECURITY_STATUSTAMPERED_IND_NO  1
	#else
	    #define SECURITY_STATUSTAMPERED_IND_DESC
	    #define SECURITY_STATUSTAMPERED_IND_NO  0
	#endif
#else
	#define SECURITY_STATUSTAMPERED_NO 0
	#define CHAR_SECURITY_STATUSTAMPERED_ATT_TABLE
	#define SECURITY_STATUSTAMPERED_IND_NO  0
#endif


#if CHAR_SECURITY_SVC_SIGN_ENABLE
#define CHAR_SECURITY_SVC_A5_SIGN_TABLE   \
    {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_security_A5_R_S, &hap_desc_service_security_A5_R_S},  \
    {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_SECURITY_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_SECURITY_UNKOWN, 0, 0},    \
        {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SECURITY_INST_NO + 7), 0, 0},
#define SECURITY_SVC_SIGN_NO   3
#else
#define CHAR_SECURITY_SVC_A5_SIGN_TABLE
#define SECURITY_SVC_SIGN_NO   0
#endif

#define SECURITY_NO_DELTA   SECURITY_CURRENTSTATE_IND_NO  \
                            +SECURITY_TARGETSTATE_IND_NO  \
                            +SECURITY_ALARMTYPE_IND_NO  \
                            +SECURITY_STATUSFAULT_IND_NO  \
                            +SECURITY_STATUSTAMPERED_IND_NO

#define SECURITY_NO         12 +SECURITY_ALARMTYPE_NO	\
							+SECURITY_STATUSFAULT_NO	\
							+SECURITY_STATUSTAMPERED_NO	\
							+SECURITY_SVC_SIGN_NO
							
#define SECURITY_HANDLE_NUM (SECURITY_NO + SECURITY_NO_DELTA)


#define HOMEKIT_SECURITY_SERVICE_ATT_TABLE  \
    {SECURITY_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(securityServiceUUID),sizeof(securityServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(securityServiceUUID), 0, 0},  \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(insts + SECURITY_INST_NO), 0, 0},  \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_NAME, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(hSecurityServiceName), 0, (u8*)(nameUUID), (u8*)(hSecurityServiceName), 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + SECURITY_INST_NO + 1), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSecurityCurrentState_R_S, 0},   \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSecurityCurrentState),0, (u8*)(charSecurityCurrentStateUUID), (u8*)&hSecurityCurrentState, 0, hSecurityCurrentStateReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SECURITY_INST_NO + 2), 0, 0}, \
            SECURITY_CURRENTSTATE_IND_DESC \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSecurityTargetState_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSecurityTargetState),0, (u8*)(charSecurityTargetStateUUID), (u8*)&hSecurityTargetState, hSecurityTargetStateWriteCallback, hSecurityTargetStateReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SECURITY_INST_NO + 3), 0, 0}, \
            SECURITY_TARGETSTATE_IND_DESC \
            CHAR_SECURITY_ALARMTYPE_ATT_TABLE	\
            CHAR_SECURITY_STATUSFAULT_ATT_TABLE	\
            CHAR_SECURITY_STATUSTAMPERED_ATT_TABLE	\
        	CHAR_SECURITY_SVC_A5_SIGN_TABLE


#define SECURITY_HANDLE_COMMON_NUM     8

#define SECURITY_HANDLE_NUM_CURRENTSTATE    SECURITY_HANDLE_START_NUM +SECURITY_HANDLE_COMMON_NUM
#define SECURITY_HANDLE_NUM_TARGETSTATE     SECURITY_HANDLE_NUM_CURRENTSTATE +SECURITY_CURRENTSTATE_NO +SECURITY_CURRENTSTATE_IND_NO
#define SECURITY_HANDLE_NUM_ALARMTYPE       SECURITY_HANDLE_NUM_TARGETSTATE +SECURITY_TARGETSTATE_NO +SECURITY_TARGETSTATE_IND_NO
#define SECURITY_HANDLE_NUM_STATUSFAULT     SECURITY_HANDLE_NUM_ALARMTYPE +SECURITY_ALARMTYPE_NO +SECURITY_ALARMTYPE_IND_NO
#define SECURITY_HANDLE_NUM_STATUSTAMPERED  SECURITY_HANDLE_NUM_STATUSFAULT +SECURITY_STATUSFAULT_NO +SECURITY_STATUSFAULT_IND_NO
#define SECURITY_HANDLE_NUM_THE_END


#define SECURITY_STORE_CHAR_VALUE_T_SECURITYCURRENTSTATE          u32 SecurityCurrentState:3;   /* 0~4 */
#define SECURITY_STORE_CHAR_VALUE_T_SECURITYCURRENTSTATE_STORE        STORE_CHAR(SecurityCurrentState,hSecurityCurrentState)
#define SECURITY_STORE_CHAR_VALUE_T_SECURITYCURRENTSTATE_RESTORE      RESTORE_CHAR(SecurityCurrentState,hSecurityCurrentState)

#define SECURITY_STORE_CHAR_VALUE_T_SECURITYTARGETSTATE           u32 SecurityTargetState:2;    /* 0~3 */
#define SECURITY_STORE_CHAR_VALUE_T_SECURITYTARGETSTATE_STORE         STORE_CHAR(SecurityTargetState,hSecurityTargetState)
#define SECURITY_STORE_CHAR_VALUE_T_SECURITYTARGETSTATE_RESTORE       RESTORE_CHAR(SecurityTargetState,hSecurityTargetState)

#define SECURITY_STORE_CHAR_VALUE_T_SECURITYALARMTYPE             u32 SecurityAlarmType:1;  /* 0~1 */
#define SECURITY_STORE_CHAR_VALUE_T_SECURITYALARMTYPE_STORE           STORE_CHAR(SecurityAlarmType,hSecurityAlarmType)
#define SECURITY_STORE_CHAR_VALUE_T_SECURITYALARMTYPE_RESTORE         RESTORE_CHAR(SecurityAlarmType,hSecurityAlarmType)

#define SECURITY_STORE_CHAR_VALUE_T_SECURITYSTATUSFAULT           u32 SecurityStatusFault:1;  /* 0~1 */
#define SECURITY_STORE_CHAR_VALUE_T_SECURITYSTATUSFAULT_STORE         STORE_CHAR(SecurityStatusFault,hSecurityStatusFault)
#define SECURITY_STORE_CHAR_VALUE_T_SECURITYSTATUSFAULT_RESTORE       RESTORE_CHAR(SecurityStatusFault,hSecurityStatusFault)

#define SECURITY_STORE_CHAR_VALUE_T_HSECURITYSTATUSTAMPERED       u32 SecurityStatusTampered:1;  /* 0~1 */
#define SECURITY_STORE_CHAR_VALUE_T_HSECURITYSTATUSTAMPERED_STORE     STORE_CHAR(SecurityStatusTampered,hSecurityStatusTampered)
#define SECURITY_STORE_CHAR_VALUE_T_HSECURITYSTATUSTAMPERED_RESTORE   RESTORE_CHAR(SecurityStatusTampered,hSecurityStatusTampered)

#define SECURITY_STORE_CHAR_VALUE_T   \
                    SECURITY_STORE_CHAR_VALUE_T_SECURITYCURRENTSTATE  \
                    SECURITY_STORE_CHAR_VALUE_T_SECURITYTARGETSTATE   \
                    SECURITY_STORE_CHAR_VALUE_T_SECURITYALARMTYPE  \
                    SECURITY_STORE_CHAR_VALUE_T_SECURITYSTATUSFAULT   \
                    SECURITY_STORE_CHAR_VALUE_T_HSECURITYSTATUSTAMPERED

#define SECURITY_STORE_CHAR_TO_FLASH  \
                    SECURITY_STORE_CHAR_VALUE_T_SECURITYCURRENTSTATE_STORE    \
                    SECURITY_STORE_CHAR_VALUE_T_SECURITYTARGETSTATE_STORE \
                    SECURITY_STORE_CHAR_VALUE_T_SECURITYALARMTYPE_STORE    \
                    SECURITY_STORE_CHAR_VALUE_T_SECURITYSTATUSFAULT_STORE \
                    SECURITY_STORE_CHAR_VALUE_T_HSECURITYSTATUSTAMPERED_STORE

#define SECURITY_RESTORE_CHAR_FROM_FLASH  \
                    SECURITY_STORE_CHAR_VALUE_T_SECURITYCURRENTSTATE_RESTORE  \
                    SECURITY_STORE_CHAR_VALUE_T_SECURITYTARGETSTATE_RESTORE   \
                    SECURITY_STORE_CHAR_VALUE_T_SECURITYALARMTYPE_RESTORE  \
                    SECURITY_STORE_CHAR_VALUE_T_SECURITYSTATUSFAULT_RESTORE   \
                    SECURITY_STORE_CHAR_VALUE_T_HSECURITYSTATUSTAMPERED_RESTORE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN unsigned char hSecurityCurrentState;     // 1
EXTERN unsigned char hSecurityTargetState;      // 2
EXTERN unsigned char hSecurityAlarmType;        // 4
EXTERN unsigned char hSecurityStatusFault;      // 5
EXTERN unsigned char hSecurityStatusTampered;   // 6

EXTERN unsigned short ccc_hSecurityCurrentState;
EXTERN unsigned short ccc_hSecurityTargetState;
EXTERN unsigned short ccc_hSecurityAlarmType;
EXTERN unsigned short ccc_hSecurityStatusFault;
EXTERN unsigned short ccc_hSecurityStatusTampered;

#if SECURITY_CURRENTSTATE_BRC_ENABLE
    #define BC_PARA_SECURITY_CURRENTSTATE ((u8*)&bc_para_hSecurityCurrentState)
    EXTERN unsigned char bc_para_hSecurityCurrentState;
#else
    #define BC_PARA_SECURITY_CURRENTSTATE 0
#endif

#if SECURITY_TARGETSTATE_BRC_ENABLE
    #define BC_PARA_SECURITY_TARGETSTATE ((u8*)&bc_para_hSecurityTargetState)
    EXTERN unsigned char bc_para_hSecurityTargetState;
#else
    #define BC_PARA_SECURITY_TARGETSTATE 0
#endif

#if SECURITY_ALARMTYPE_BRC_ENABLE
    #define BC_PARA_SECURITY_ALARMTYPE ((u8*)&bc_para_hSecurityAlarmType)
    EXTERN unsigned char bc_para_hSecurityAlarmType;
#else
    #define BC_PARA_SECURITY_ALARMTYPE 0
#endif

#if SECURITY_STATUSFAULT_BRC_ENABLE
    #define BC_PARA_SECURITY_STATUSFAULT ((u8*)&bc_para_hSecurityStatusFault)
    EXTERN unsigned char bc_para_hSecurityStatusFault;
#else
    #define BC_PARA_SECURITY_STATUSFAULT 0
#endif

#if SECURITY_STATUSTAMPERED_BRC_ENABLE
    #define BC_PARA_SECURITY_STATUSTAMPERED ((u8*)&bc_para_hSecurityStatusTampered)
    EXTERN unsigned char bc_para_hSecurityStatusTampered;
#else
    #define BC_PARA_SECURITY_STATUSTAMPERED 0
#endif

#if SECURITY_LINKED_SERVICES_ENABLE
	#define SECURITY_LINKED_SVCS			hSecurity_linked_svcs
	#define SECURITY_LINKED_SVCS_LENGTH 	(sizeof(hSecurity_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * hSecurity_linked_svcs[];
#else
	#define SECURITY_LINKED_SVCS			0
	#define SECURITY_LINKED_SVCS_LENGTH 	0
#endif

EXTERN const u8 hSecurityServiceName[15];

EXTERN const hap_characteristic_desc_t hap_desc_char_hSecurityCurrentState_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSecurityTargetState_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSecurityAlarmType_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSecurityStatusFault_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSecurityStatusTampered_R_S;


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN int hSecurityCurrentStateReadCallback(void *pp);
EXTERN int hSecurityTargetStateReadCallback(void *pp);
EXTERN int hSecurityTargetStateWriteCallback(void *pp);
EXTERN int hSecurityAlarmTypeReadCallback(void *pp);
EXTERN int hSecurityStatusFaultReadCallback(void *pp);
EXTERN int hSecurityStatusTamperedReadCallback(void *pp);
EXTERN int ccc_hSecurityCurrentStateWriteCB(void *pp);
EXTERN int ccc_hSecurityTargetStateWriteCB(void *pp);
EXTERN int ccc_hSecurityAlarmTypeWriteCB(void *pp);
EXTERN int ccc_hSecurityStatusFaultWriteCB(void *pp);
EXTERN int ccc_hSecurityStatusTamperedWriteCB(void *pp);

#endif /* _HK_SECURITY_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
