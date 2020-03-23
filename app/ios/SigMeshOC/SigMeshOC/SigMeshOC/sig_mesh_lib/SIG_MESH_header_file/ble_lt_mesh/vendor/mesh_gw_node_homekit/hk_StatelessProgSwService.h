/********************************************************************************************************
 * @file     hk_StatelessProgSwService.h
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
#ifndef _HK_STATELESS_PROG_SW_SERVICE_H_
#define _HK_STATELESS_PROG_SW_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_STATELESS_PROG_SW_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif


#define CHAR_STATELESS_PROG_SW_SVC_SIGN_ENABLE      0
#if CHAR_STATELESS_PROG_SW_SVC_SIGN_ENABLE
    #define STATELESS_PROG_SW_AS_PRIMARY_SERVICE    	0
    #define STATELESS_PROG_SW_IS_HIDDEN_SERVICE     	0
    #define STATELESS_PROG_SW_LINKED_SERVICES_ENABLE	0
    EXTERN const hap_characteristic_desc_t hap_desc_char_stateless_prog_sw_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_stateless_prog_sw_A5_R_S;
    EXTERN unsigned char VAR_STATELESS_PROG_SW_UNKOWN;
#else
    #define STATELESS_PROG_SW_AS_PRIMARY_SERVICE    	0
    #define STATELESS_PROG_SW_IS_HIDDEN_SERVICE     	0
    #define STATELESS_PROG_SW_LINKED_SERVICES_ENABLE	0
#endif


#define CHAR_STATELESS_PROG_SW_SERVICE_LABEL_INDEX_ENABLE   0

#define STATELESS_PROG_SW_SWEVENT_IND_ENABLE                0

#define STATELESS_PROG_SW_SWEVENT_BRC_ENABLE                0


#define STATELESSPROG_SWEVENT_NO 3
#if STATELESS_PROG_SW_SWEVENT_IND_ENABLE
    #define STATELESSPROGSW_SWEVENT_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hStatelessProg_SwEvent), ccc_hStatelessProg_SwEventWriteCB, 0},
    #define STATELESSPROGSW_SWEVENT_IND_NO  1
#else
    #define STATELESSPROGSW_SWEVENT_IND_DESC
    #define STATELESSPROGSW_SWEVENT_IND_NO  0
#endif

#if CHAR_STATELESS_PROG_SW_SERVICE_LABEL_INDEX_ENABLE
    #define STATELESS_PROG_SW_SERVICE_LABEL_INDEX_NO 3
    #define CHAR_STATELESS_PROG_SW_SERVICE_LABEL_INDEX_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_hServiceLabelIndex_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hStateless_Prog_Sw_ServiceLabelIndex),0, (u8*)(charServiceLabelIndexUUID), (u8*)&hStateless_Prog_Sw_ServiceLabelIndex, 0, Stateless_Prog_Sw_ServiceLabelIndexReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + STATELESS_PROG_SW_INST_NO + 3), 0, 0},
#else
    #define STATELESS_PROG_SW_SERVICE_LABEL_INDEX_NO 0
    #define CHAR_STATELESS_PROG_SW_SERVICE_LABEL_INDEX_ATT_TABLE
#endif

#if CHAR_STATELESS_PROG_SW_SVC_SIGN_ENABLE
#define CHAR_STATELESS_PROG_SW_SVC_A5_SIGN_TABLE   \
    {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_stateless_prog_sw_A5_R_S, &hap_desc_service_stateless_prog_sw_A5_R_S},  \
    {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_STATELESS_PROG_SW_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_STATELESS_PROG_SW_UNKOWN, 0, 0},    \
        {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + STATELESS_PROG_SW_INST_NO + 4), 0, 0},
#define STATELESS_PROG_SW_SVC_SIGN_NO   3
#else
#define CHAR_STATELESS_PROG_SW_SVC_A5_SIGN_TABLE
#define STATELESS_PROG_SW_SVC_SIGN_NO   0
#endif

#define STATELESS_PROG_SW_NO_DELTA  STATELESSPROGSW_SWEVENT_IND_NO
#define STATELESS_PROG_SW_NO        9 +STATELESS_PROG_SW_SERVICE_LABEL_INDEX_NO +STATELESS_PROG_SW_SVC_SIGN_NO
#define STATELESS_PROG_SW_HANDLE_NUM (STATELESS_PROG_SW_NO + STATELESS_PROG_SW_NO_DELTA)


#define HOMEKIT_STATELESS_PROG_SW_SERVICE_ATT_TABLE \
    {STATELESS_PROG_SW_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(stateless_prog_swServiceUUID),sizeof(stateless_prog_swServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(stateless_prog_swServiceUUID), 0, 0}, \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(insts + STATELESS_PROG_SW_INST_NO), 0, 0}, \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_NAME, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(hStateless_Prog_SwServiceName), 0, (u8*)(nameUUID), (u8*)(hStateless_Prog_SwServiceName), 0, 0},  \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + STATELESS_PROG_SW_INST_NO + 1), 0, 0},   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hStateless_Prog_SwEvent_R_S, 0}, \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hStateless_Prog_SwEvent),0, (u8*)(charProgSwEventUUID), (u8*)&hStateless_Prog_SwEvent, 0, Stateless_Prog_SwEventReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + STATELESS_PROG_SW_INST_NO + 2), 0, 0},    \
            STATELESSPROGSW_SWEVENT_IND_DESC  \
        CHAR_STATELESS_PROG_SW_SERVICE_LABEL_INDEX_ATT_TABLE  \
        CHAR_STATELESS_PROG_SW_SVC_A5_SIGN_TABLE


#define STATELESSPROG_HANDLE_COMMON_NUM     8

#define STATELESSPROG_HANDLE_NUM_SWEVENT                   STATELESS_PROG_SW_HANDLE_START_NUM +STATELESSPROG_HANDLE_COMMON_NUM
#define STATELESSPROG_HANDLE_NUM_SW_SERVICE_LABEL_INDEX    STATELESSPROG_HANDLE_NUM_SWEVENT +STATELESSPROG_SWEVENT_NO +STATELESSPROGSW_SWEVENT_IND_NO
#define STATELESSPROG_HANDLE_NUM_THE_END

#define STATELESS_PROG_SW_STORE_CHAR_VALUE_T_STATELESS_PROG_SWEVENT             u32 Stateless_Prog_SwEvent:2; /* 0~2 */
#define STATELESS_PROG_SW_STORE_CHAR_VALUE_T_STATELESS_PROG_SWEVENT_STORE           STORE_CHAR(Stateless_Prog_SwEvent,hStateless_Prog_SwEvent)
#define STATELESS_PROG_SW_STORE_CHAR_VALUE_T_STATELESS_PROG_SWEVENT_RESTORE         RESTORE_CHAR(Stateless_Prog_SwEvent,hStateless_Prog_SwEvent)

#if CHAR_STATELESS_PROG_SW_SERVICE_LABEL_INDEX_ENABLE
#define STATELESS_PROG_SW_STORE_CHAR_VALUE_T_SERVICE_LABEL_INDEX                          u32 Stateless_Prog_Sw_ServiceLabelIndex:8; /* Min Value is 1 */
#define STATELESS_PROG_SW_STORE_CHAR_VALUE_T_SERVICE_LABEL_INDEX_STORE                        STORE_CHAR(Stateless_Prog_Sw_ServiceLabelIndex,hStateless_Prog_Sw_ServiceLabelIndex)
#define STATELESS_PROG_SW_STORE_CHAR_VALUE_T_SERVICE_LABEL_INDEX_RESTORE                      RESTORE_CHAR(Stateless_Prog_Sw_ServiceLabelIndex,hStateless_Prog_Sw_ServiceLabelIndex)
#else
#define STATELESS_PROG_SW_STORE_CHAR_VALUE_T_SERVICE_LABEL_INDEX
#define STATELESS_PROG_SW_STORE_CHAR_VALUE_T_SERVICE_LABEL_INDEX_STORE
#define STATELESS_PROG_SW_STORE_CHAR_VALUE_T_SERVICE_LABEL_INDEX_RESTORE
#endif

#define STATELESS_PROG_SW_STORE_CHAR_VALUE_T   \
                    STATELESS_PROG_SW_STORE_CHAR_VALUE_T_STATELESS_PROG_SWEVENT  \
                    STATELESS_PROG_SW_STORE_CHAR_VALUE_T_SERVICE_LABEL_INDEX

#define STATELESS_PROG_SW_STORE_CHAR_TO_FLASH  \
                    STATELESS_PROG_SW_STORE_CHAR_VALUE_T_STATELESS_PROG_SWEVENT_STORE  \
                    STATELESS_PROG_SW_STORE_CHAR_VALUE_T_SERVICE_LABEL_INDEX_STORE

#define STATELESS_PROG_SW_RESTORE_CHAR_FROM_FLASH  \
                    STATELESS_PROG_SW_STORE_CHAR_VALUE_T_STATELESS_PROG_SWEVENT_RESTORE  \
                    STATELESS_PROG_SW_STORE_CHAR_VALUE_T_SERVICE_LABEL_INDEX_RESTORE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN unsigned char hStateless_Prog_SwEvent;
EXTERN unsigned char hStateless_Prog_Sw_ServiceLabelIndex;

EXTERN const u8 hStateless_Prog_SwServiceName[24];

EXTERN unsigned short ccc_hStatelessProg_SwEvent;

#if STATELESS_PROG_SW_SWEVENT_BRC_ENABLE
    #define BC_PARA_STATELESS_PROG_SW_SWEVENT ((u8*)&bc_para_hStatelessProg_SwEvent)
    EXTERN unsigned char bc_para_hStatelessProg_SwEvent;
#else
    #define BC_PARA_STATELESS_PROG_SW_SWEVENT 0
#endif

#if STATELESS_PROG_SW_LINKED_SERVICES_ENABLE
	#define STATELESS_PROG_SW_LINKED_SVCS			StatelessProgSw_linked_svcs
	#define STATELESS_PROG_SW_LINKED_SVCS_LENGTH 	(sizeof(StatelessProgSw_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * StatelessProgSw_linked_svcs[];
#else
	#define STATELESS_PROG_SW_LINKED_SVCS			0
	#define STATELESS_PROG_SW_LINKED_SVCS_LENGTH 	0
#endif

EXTERN const hap_characteristic_desc_t hap_desc_char_hStateless_Prog_SwEvent_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hServiceLabelIndex_R_S;


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN int Stateless_Prog_SwEventReadCallback(void *pp);
//-EXTERN int Stateless_Prog_SwEventWriteCallback(void *pp);
EXTERN int Stateless_Prog_Sw_ServiceLabelIndexReadCallback(void *pp);

EXTERN int ccc_hStatelessProg_SwEventWriteCB(void *pp);


#endif /* _HK_STATELESS_PROG_SW_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
