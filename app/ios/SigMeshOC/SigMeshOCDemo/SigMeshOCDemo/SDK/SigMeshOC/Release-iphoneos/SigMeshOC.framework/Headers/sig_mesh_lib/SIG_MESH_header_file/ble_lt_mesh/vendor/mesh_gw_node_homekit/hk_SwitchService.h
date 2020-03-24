/********************************************************************************************************
 * @file     hk_SwitchService.h
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
#ifndef _HK_SWITCH_SERVICE_H_
#define _HK_SWITCH_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_SWITCH_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif


#define CHAR_SWITCH_SVC_SIGN_ENABLE      0
#if CHAR_SWITCH_SVC_SIGN_ENABLE
    #define SWITCH_AS_PRIMARY_SERVICE    	0
    #define SWITCH_IS_HIDDEN_SERVICE     	0
    #define SWITCH_LINKED_SERVICES_ENABLE	0
    EXTERN const hap_characteristic_desc_t hap_desc_char_switch_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_switch_A5_R_S;
    EXTERN unsigned char VAR_SWITCH_UNKOWN;
#else
    #define SWITCH_AS_PRIMARY_SERVICE    	0
    #define SWITCH_IS_HIDDEN_SERVICE     	0
    #define SWITCH_LINKED_SERVICES_ENABLE	0
#endif


#define SWITCH_ON_IND_ENABLE    1

#define SWITCH_ON_BRC_ENABLE    0


#define SWITCH_ON_NO 3
#if SWITCH_ON_IND_ENABLE
    #define SWITCH_ON_IND_DESC \
        {0,ATT_PERMISSIONS_READ,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSwitchOn), ccc_hSwitchOnWriteCB, 0},
    #define SWITCH_ON_SWEVENT_IND_NO  1
#else
    #define SWITCH_ON_IND_DESC
    #define SWITCH_ON_SWEVENT_IND_NO  0
#endif

#if CHAR_SWITCH_SVC_SIGN_ENABLE
#define CHAR_SWITCH_SVC_A5_SIGN_TABLE   \
    {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_switch_A5_R_S, &hap_desc_service_switch_A5_R_S},  \
    {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_SWITCH_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_SWITCH_UNKOWN, 0, 0},    \
        {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SWITCH_INST_NO + 3), 0, 0},
#define SWITCH_SVC_SIGN_NO   3
#else
#define CHAR_SWITCH_SVC_A5_SIGN_TABLE
#define SWITCH_SVC_SIGN_NO   0
#endif

#define SWITCH_NO_DELTA SWITCH_ON_SWEVENT_IND_NO
#define SWITCH_NO       9 +SWITCH_SVC_SIGN_NO
#define SWITCH_HANDLE_NUM (SWITCH_NO + SWITCH_NO_DELTA)


#define HOMEKIT_SWITCH_SERVICE_ATT_TABLE    \
    {SWITCH_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(switchServiceUUID),sizeof(switchServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(switchServiceUUID), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(insts + SWITCH_INST_NO), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_NAME, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(hSwitchServiceName), 0, (u8*)(nameUUID), (u8*)(hSwitchServiceName), 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + SWITCH_INST_NO + 1), 0, 0},  \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_SWITCH_ON_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSwitchOn),0, (u8*)(charOnUUID), (u8*)&hSwitchOn, SwitchOnWriteCallback, SwitchOnReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SWITCH_INST_NO + 2), 0, 0},   \
            SWITCH_ON_IND_DESC \
        CHAR_SWITCH_SVC_A5_SIGN_TABLE


#define SWITCH_HANDLE_COMMON_NUM     8

#define SWITCH_HANDLE_NUM_ON       SWITCH_HANDLE_START_NUM +SWITCH_HANDLE_COMMON_NUM
#define SWITCH_HANDLE_NUM_THE_END


#define SWITCH_STORE_CHAR_VALUE_T_SWITCHON           u32 SwitchOn:1;    /* 0~1 */
#define SWITCH_STORE_CHAR_VALUE_T_SWITCHON_STORE         STORE_CHAR(SwitchOn,hSwitchOn)
#define SWITCH_STORE_CHAR_VALUE_T_SWITCHON_RESTORE       RESTORE_CHAR(SwitchOn,hSwitchOn)

#define SWITCH_STORE_CHAR_VALUE_T            \
                    SWITCH_STORE_CHAR_VALUE_T_SWITCHON

#define SWITCH_STORE_CHAR_TO_FLASH           \
                    SWITCH_STORE_CHAR_VALUE_T_SWITCHON_STORE

#define SWITCH_RESTORE_CHAR_FROM_FLASH       \
                    SWITCH_STORE_CHAR_VALUE_T_SWITCHON_RESTORE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN unsigned char hSwitchOn;

EXTERN unsigned short ccc_hSwitchOn;

#if SWITCH_ON_BRC_ENABLE
    #define BC_PARA_SWITCH_ON ((u8*)&bc_para_hSwitchOn)
    EXTERN unsigned char bc_para_hSwitchOn;
#else
    #define BC_PARA_SWITCH_ON 0
#endif

#if SWITCH_LINKED_SERVICES_ENABLE
	#define SWITCH_LINKED_SVCS			hSwitch_linked_svcs
	#define SWITCH_LINKED_SVCS_LENGTH 	(sizeof(hSwitch_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * hSwitch_linked_svcs[];
#else
	#define SWITCH_LINKED_SVCS			0
	#define SWITCH_LINKED_SVCS_LENGTH 	0
#endif

EXTERN const u8 hSwitchServiceName[13];

EXTERN const hap_characteristic_desc_t hap_desc_char_SWITCH_ON_R_S;


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN int SwitchOnReadCallback(void *pp);
EXTERN int SwitchOnWriteCallback(void *pp);
EXTERN int ccc_hSwitchOnWriteCB(void *pp);

#endif /* _HK_SWITCH_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
