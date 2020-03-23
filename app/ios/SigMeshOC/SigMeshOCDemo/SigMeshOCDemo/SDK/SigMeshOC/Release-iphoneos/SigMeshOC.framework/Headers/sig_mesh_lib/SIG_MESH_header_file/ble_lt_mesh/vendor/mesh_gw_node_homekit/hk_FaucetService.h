/********************************************************************************************************
 * @file     hk_FaucetService.h
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
#ifndef _HK_FAUCET_SERVICE_H_
#define _HK_FAUCET_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_FAUCET_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif

#define CHAR_FAUCET_SVC_SIGN_ENABLE      0
#if CHAR_FAUCET_SVC_SIGN_ENABLE
    #define FAUCET_AS_PRIMARY_SERVICE    	0
    #define FAUCET_IS_HIDDEN_SERVICE     	0
    #define FAUCET_LINKED_SERVICES_ENABLE	0
    EXTERN const hap_characteristic_desc_t hap_desc_char_faucet_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_faucet_A5_R_S;
    EXTERN unsigned char VAR_FAUCET_UNKOWN;
#else
    #define FAUCET_AS_PRIMARY_SERVICE    	0
    #define FAUCET_IS_HIDDEN_SERVICE     	0
    #define FAUCET_LINKED_SERVICES_ENABLE	0
#endif

#define CHAR_FAUCET_STATUSFAULT_ENABLE					0

#define FAUCET_ACTIVE_IND_ENABLE						0
#define FAUCET_STATUSFAULT_IND_ENABLE					0

#define FAUCET_ACTIVE_BRC_ENABLE						0
#define FAUCET_STATUSFAULT_BRC_ENABLE					0


#define FAUCET_ACTIVE_NO      3
#if FAUCET_ACTIVE_IND_ENABLE
    #define FAUCET_ACTIVE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hFaucetActive), ccc_FaucetActiveWriteCB, 0},
    #define FAUCET_ACTIVE_IND_NO  1
#else
    #define FAUCET_ACTIVE_IND_DESC
    #define FAUCET_ACTIVE_IND_NO  0
#endif

#if CHAR_FAUCET_STATUSFAULT_ENABLE
    #define FAUCET_STATUSFAULT_NO 3
    #define CHAR_FAUCET_STATUSFAULT_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_FAUCET_STATUSFAULT_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof (hFaucetStatusFault), 0, (u8*)(&charStatusFaultUUID), (u8*)&hFaucetStatusFault, 0, FaucetStatusFaultReadCallback},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + FAUCET_INST_NO + 3), 0, 0}, \
            FAUCET_STATUSFAULT_IND_DESC

    #if FAUCET_STATUSFAULT_IND_ENABLE
        #define FAUCET_STATUSFAULT_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hFaucetStatusFault), ccc_FaucetStatusFaultWriteCB, 0},
        #define FAUCET_STATUSFAULT_IND_NO  1
    #else
        #define FAUCET_STATUSFAULT_IND_DESC
        #define FAUCET_STATUSFAULT_IND_NO  0
    #endif
#else
    #define FAUCET_STATUSFAULT_NO 0
    #define CHAR_FAUCET_STATUSFAULT_ATT_TABLE
    #define FAUCET_STATUSFAULT_IND_NO  0
#endif


#if CHAR_FAUCET_SVC_SIGN_ENABLE
    #define FAUCET_SVC_SIGN_NO   3
    #define CHAR_FAUCET_SVC_A5_SIGN_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_faucet_A5_R_S, &hap_desc_service_faucet_A5_R_S},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_FAUCET_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_FAUCET_UNKOWN, 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + FAUCET_INST_NO + 4), 0, 0},
#else
    #define FAUCET_SVC_SIGN_NO   0
    #define CHAR_FAUCET_SVC_A5_SIGN_TABLE
#endif

#define FAUCET_NO_DELTA   FAUCET_ACTIVE_IND_NO \
						+ FAUCET_STATUSFAULT_IND_NO

#define FAUCET_NO   9 + FAUCET_STATUSFAULT_NO \
					+ FAUCET_SVC_SIGN_NO


#define FAUCET_HANDLE_NUM (FAUCET_NO + FAUCET_NO_DELTA)

#define HOMEKIT_FAUCET_SERVICE_ATT_TABLE   \
    {FAUCET_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(faucetServiceUUID),sizeof(faucetServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(faucetServiceUUID), 0, 0},   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(insts + FAUCET_INST_NO), 0, 0},   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_NAME, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(hFaucetServiceName), 0, (u8*)(nameUUID), (u8*)(hFaucetServiceName), 0, 0},  \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + FAUCET_INST_NO + 1), 0, 0}, \
		{0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_FAUCET_ACTIVE_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hFaucetActive),0, (u8*)(charActiveUUID), (u8*)&hFaucetActive, FaucetActiveWriteCallback, FaucetActiveReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + FAUCET_INST_NO + 2), 0, 0},  \
            FAUCET_ACTIVE_IND_DESC \
        CHAR_FAUCET_STATUSFAULT_ATT_TABLE \
        CHAR_FAUCET_SVC_A5_SIGN_TABLE


#define FAUCET_HANDLE_COMMON_NUM     8

#define FAUCET_HANDLE_NUM_ACTIVE					FAUCET_HANDLE_START_NUM +FAUCET_HANDLE_COMMON_NUM
#define FAUCET_HANDLE_NUM_STATUSFAULT				FAUCET_HANDLE_NUM_ACTIVE +FAUCET_ACTIVE_NO +FAUCET_ACTIVE_IND_NO
#define FAUCET_HANDLE_NUM_THE_END


#define FAUCET_STORE_CHAR_VALUE_T_ACTIVE							u32 FaucetActive:1;     /* 0~1 */
#define FAUCET_STORE_CHAR_VALUE_T_ACTIVE_STORE					STORE_CHAR(FaucetActive, hFaucetActive)
#define FAUCET_STORE_CHAR_VALUE_T_ACTIVE_RESTORE					RESTORE_CHAR(FaucetActive, hFaucetActive)

#if CHAR_FAUCET_STATUSFAULT_ENABLE
#define FAUCET_STORE_CHAR_VALUE_T_STATUSFAULT						u32 FaucetStatusFault:1;   /* 0~1 */
#define FAUCET_STORE_CHAR_VALUE_T_STATUSFAULT_STORE				STORE_CHAR(FaucetStatusFault, hFaucetStatusFault)
#define FAUCET_STORE_CHAR_VALUE_T_STATUSFAULT_RESTORE				RESTORE_CHAR(FaucetStatusFault, hFaucetStatusFault)
#else
#define FAUCET_STORE_CHAR_VALUE_T_STATUSFAULT
#define FAUCET_STORE_CHAR_VALUE_T_STATUSFAULT_STORE
#define FAUCET_STORE_CHAR_VALUE_T_STATUSFAULT_RESTORE
#endif


#define FAUCET_STORE_CHAR_VALUE_T            \
        FAUCET_STORE_CHAR_VALUE_T_ACTIVE \
        FAUCET_STORE_CHAR_VALUE_T_STATUSFAULT

#define FAUCET_STORE_CHAR_TO_FLASH           \
        FAUCET_STORE_CHAR_VALUE_T_ACTIVE_STORE \
        FAUCET_STORE_CHAR_VALUE_T_STATUSFAULT_STORE

#define FAUCET_RESTORE_CHAR_FROM_FLASH            \
        FAUCET_STORE_CHAR_VALUE_T_ACTIVE_RESTORE \
        FAUCET_STORE_CHAR_VALUE_T_STATUSFAULT_RESTORE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN unsigned char hFaucetActive;
EXTERN unsigned char hFaucetStatusFault;

EXTERN unsigned short ccc_hFaucetActive;
EXTERN unsigned short ccc_hFaucetStatusFault;


#if FAUCET_ACTIVE_BRC_ENABLE
    #define BC_PARA_FAUCET_ACTIVE ((u8*)&bc_para_hFaucetActive)
    EXTERN unsigned char bc_para_hFaucetActive;
#else
    #define BC_PARA_FAUCET_ACTIVE 0
#endif

#if FAUCET_STATUSFAULT_BRC_ENABLE
    #define BC_PARA_FAUCET_STATUSFAULT ((u8*)&bc_para_hFaucetStatusFault)
    EXTERN unsigned char bc_para_hFaucetStatusFault;
#else
    #define BC_PARA_FAUCET_STATUSFAULT 0
#endif

#if FAUCET_LINKED_SERVICES_ENABLE
	#define FAUCET_LINKED_SVCS			hFaucet_linked_svcs
	#define FAUCET_LINKED_SVCS_LENGTH 	(sizeof(hFaucet_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * hFaucet_linked_svcs[];
#else
	#define FAUCET_LINKED_SVCS			0
	#define FAUCET_LINKED_SVCS_LENGTH 	0
#endif

EXTERN const u8 hFaucetServiceName[13];

EXTERN const hap_characteristic_desc_t hap_desc_char_FAUCET_ACTIVE_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_FAUCET_STATUSFAULT_R_S;


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN int FaucetActiveReadCallback(void *pp);
EXTERN int FaucetActiveWriteCallback(void *pp);
EXTERN int FaucetStatusFaultReadCallback(void *pp);

EXTERN int ccc_FaucetActiveWriteCB(void *pp);
EXTERN int ccc_FaucetStatusFaultWriteCB(void *pp);

#endif /* _HK_FAUCET_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
