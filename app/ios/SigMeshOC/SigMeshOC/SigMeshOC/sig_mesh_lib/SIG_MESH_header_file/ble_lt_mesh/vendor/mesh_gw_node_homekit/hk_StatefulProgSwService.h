/********************************************************************************************************
 * @file     hk_StatefulProgSwService.h
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
#ifndef _HK_STATEFUL_PROG_SW_SERVICE_H_
#define _HK_STATEFUL_PROG_SW_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_STATEFUL_PROG_SW_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif


#define CHAR_STATEFUL_PROG_SW_SVC_SIGN_ENABLE      0
#if CHAR_STATEFUL_PROG_SW_SVC_SIGN_ENABLE
    #define STATEFUL_PROG_SW_AS_PRIMARY_SERVICE    		0
    #define STATEFUL_PROG_SW_IS_HIDDEN_SERVICE     		0
    #define STATEFUL_PROG_SW_LINKED_SERVICES_ENABLE		0
    EXTERN const hap_characteristic_desc_t hap_desc_char_stateful_prog_sw_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_stateful_prog_sw_A5_R_S;
    EXTERN unsigned char VAR_STATEFUL_PROG_SW_UNKOWN;
#else
    #define STATEFUL_PROG_SW_AS_PRIMARY_SERVICE    		0
    #define STATEFUL_PROG_SW_IS_HIDDEN_SERVICE     		0
    #define STATEFUL_PROG_SW_LINKED_SERVICES_ENABLE		0
#endif


#define STATEFULPROGSW_SWEVENT_IND_ENABLE       0
#define STATEFULPROGSW_SWOUTPUTSTATE_IND_ENABLE 0

#define STATEFULPROGSW_SWEVENT_BRC_ENABLE       0
#define STATEFULPROGSW_SWOUTPUTSTATE_BRC_ENABLE 0


#define STATEFULPROG_SWEVENT_NO 3
#if STATEFULPROGSW_SWEVENT_IND_ENABLE
    #define STATEFULPROGSW_SWEVENT_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hStatefulProg_SwEvent), ccc_hStatefulProg_SwEventWriteCB, 0},
    #define STATEFULPROGSW_SWEVENT_IND_NO  1
#else
    #define STATEFULPROGSW_SWEVENT_IND_DESC
    #define STATEFULPROGSW_SWEVENT_IND_NO  0
#endif

#define STATEFULPROG_SWOUTPUTSTATE_NO 3
#if STATEFULPROGSW_SWOUTPUTSTATE_IND_ENABLE
    #define STATEFULPROGSW_SWOUTPUTSTATE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hStatefulProg_SwOutputState), ccc_hStatefulProg_SwOutputStateWriteCB, 0},
    #define STATEFULPROGSW_SWOUTPUTSTATE_IND_NO  1
#else
    #define STATEFULPROGSW_SWOUTPUTSTATE_IND_DESC
    #define STATEFULPROGSW_SWOUTPUTSTATE_IND_NO  0
#endif

#if CHAR_STATEFUL_PROG_SW_SVC_SIGN_ENABLE
#define CHAR_STATEFUL_PROG_SW_SVC_A5_SIGN_TABLE   \
    {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_stateful_prog_sw_A5_R_S, &hap_desc_service_stateful_prog_sw_A5_R_S},  \
    {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_STATEFUL_PROG_SW_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_STATEFUL_PROG_SW_UNKOWN, 0, 0},    \
        {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + STATEFUL_PROG_SW_INST_NO + 4), 0, 0},
#define STATEFUL_PROG_SW_SVC_SIGN_NO   3
#else
#define CHAR_STATEFUL_PROG_SW_SVC_A5_SIGN_TABLE
#define STATEFUL_PROG_SW_SVC_SIGN_NO   0
#endif


#define STATEFUL_PROG_SW_NO_DELTA   STATEFULPROGSW_SWEVENT_IND_NO \
                                    +STATEFULPROGSW_SWOUTPUTSTATE_IND_NO

#define STATEFUL_PROG_SW_NO         12 +STATEFUL_PROG_SW_SVC_SIGN_NO
#define STATEFUL_PROG_SW_HANDLE_NUM (STATEFUL_PROG_SW_NO + STATEFUL_PROG_SW_NO_DELTA)


#define HOMEKIT_STATEFUL_PROG_SW_SERVICE_ATT_TABLE  \
    {STATEFUL_PROG_SW_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(stateful_prog_swServiceUUID),sizeof(stateful_prog_swServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(stateful_prog_swServiceUUID), 0, 0},  \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(insts + STATEFUL_PROG_SW_INST_NO), 0, 0},  \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_NAME, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(hStateful_Prog_SwServiceName), 0, (u8*)(nameUUID), (u8*)(hStateful_Prog_SwServiceName), 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + STATEFUL_PROG_SW_INST_NO + 1), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hStateful_Prog_SwEvent_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hStateful_Prog_SwEvent),0, (u8*)(charProgSwEventUUID), (u8*)&hStateful_Prog_SwEvent, 0, hStateful_Prog_SwEventReadCallback},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + STATEFUL_PROG_SW_INST_NO + 2), 0, 0}, \
            STATEFULPROGSW_SWEVENT_IND_DESC \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hStateful_Prog_SwOutputState_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hStateful_Prog_SwOutputState),0, (u8*)(charProgSwOutputStateUUID), (u8*)&hStateful_Prog_SwOutputState, hStateful_Prog_SwOutputStateWriteCallback, hStateful_Prog_SwOutputStateReadCallback},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + STATEFUL_PROG_SW_INST_NO + 3), 0, 0}, \
            STATEFULPROGSW_SWOUTPUTSTATE_IND_DESC \
        CHAR_STATEFUL_PROG_SW_SVC_A5_SIGN_TABLE


#define STATEFULPROG_HANDLE_COMMON_NUM          8

#define STATEFULPROG_HANDLE_NUM_SWEVENT         STATEFUL_PROG_SW_HANDLE_START_NUM +STATEFULPROG_HANDLE_COMMON_NUM
#define STATEFULPROG_HANDLE_NUM_SWOUTPUTSTATE   STATEFULPROG_HANDLE_NUM_SWEVENT +STATEFULPROG_SWEVENT_NO +STATEFULPROGSW_SWEVENT_IND_NO
#define STATEFULPROG_HANDLE_NUM_THE_END


#define STATEFUL_PROG_SW_STORE_CHAR_VALUE_T_STATEFUL_PROG_SWEVENT               u32 Stateful_Prog_SwEvent:1;    /* 0~1 */
#define STATEFUL_PROG_SW_STORE_CHAR_VALUE_T_STATEFUL_PROG_SWEVENT_STORE             STORE_CHAR(Stateful_Prog_SwEvent,hStateful_Prog_SwEvent)
#define STATEFUL_PROG_SW_STORE_CHAR_VALUE_T_STATEFUL_PROG_SWEVENT_RESTORE           RESTORE_CHAR(Stateful_Prog_SwEvent,hStateful_Prog_SwEvent)

#define STATEFUL_PROG_SW_STORE_CHAR_VALUE_T_STATEFUL_PROG_SWOUTPUTSTATE         u32 Stateful_Prog_SwOutputState:1;    /* 0~1 */
#define STATEFUL_PROG_SW_STORE_CHAR_VALUE_T_STATEFUL_PROG_SWOUTPUTSTATE_STORE       STORE_CHAR(Stateful_Prog_SwOutputState,hStateful_Prog_SwOutputState)
#define STATEFUL_PROG_SW_STORE_CHAR_VALUE_T_STATEFUL_PROG_SWOUTPUTSTATE_RESTORE     RESTORE_CHAR(Stateful_Prog_SwOutputState,hStateful_Prog_SwOutputState)

#define STATEFUL_PROG_SW_STORE_CHAR_VALUE_T   \
                    STATEFUL_PROG_SW_STORE_CHAR_VALUE_T_STATEFUL_PROG_SWEVENT  \
                    STATEFUL_PROG_SW_STORE_CHAR_VALUE_T_STATEFUL_PROG_SWOUTPUTSTATE

#define STATEFUL_PROG_SW_STORE_CHAR_TO_FLASH  \
                    STATEFUL_PROG_SW_STORE_CHAR_VALUE_T_STATEFUL_PROG_SWEVENT_STORE    \
                    STATEFUL_PROG_SW_STORE_CHAR_VALUE_T_STATEFUL_PROG_SWOUTPUTSTATE_STORE

#define STATEFUL_PROG_SW_RESTORE_CHAR_FROM_FLASH  \
                    STATEFUL_PROG_SW_STORE_CHAR_VALUE_T_STATEFUL_PROG_SWEVENT_RESTORE  \
                    STATEFUL_PROG_SW_STORE_CHAR_VALUE_T_STATEFUL_PROG_SWOUTPUTSTATE_RESTORE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN unsigned char hStateful_Prog_SwEvent;
EXTERN unsigned char hStateful_Prog_SwOutputState;

EXTERN unsigned short ccc_hStatefulProg_SwEvent;
EXTERN unsigned short ccc_hStatefulProg_SwOutputState;

#if STATEFULPROGSW_SWEVENT_BRC_ENABLE
    #define BC_PARA_STATEFULPROGSW_SWEVENT ((u8*)&bc_para_hStatefulProg_SwEvent)
    EXTERN unsigned char bc_para_hStatefulProg_SwEvent;
#else
    #define BC_PARA_STATEFULPROGSW_SWEVENT 0
#endif

#if STATEFULPROGSW_SWOUTPUTSTATE_BRC_ENABLE
    #define BC_PARA_STATEFULPROGSW_SWOUTPUTSTATE ((u8*)&bc_para_hStatefulProg_SwOutputState)
    EXTERN unsigned char bc_para_hStatefulProg_SwOutputState;
#else
    #define BC_PARA_STATEFULPROGSW_SWOUTPUTSTATE 0
#endif

#if STATEFUL_PROG_SW_LINKED_SERVICES_ENABLE
	#define STATEFUL_PROG_SW_LINKED_SVCS			hStatefulProgSw_linked_svcs
	#define STATEFUL_PROG_SW_LINKED_SVCS_LENGTH 	(sizeof(hStatefulProgSw_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * hStatefulProgSw_linked_svcs[];
#else
	#define STATEFUL_PROG_SW_LINKED_SVCS			0
	#define STATEFUL_PROG_SW_LINKED_SVCS_LENGTH 	0
#endif

EXTERN const u8 hStateful_Prog_SwServiceName[23];

EXTERN const hap_characteristic_desc_t hap_desc_char_hStateful_Prog_SwEvent_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hStateful_Prog_SwOutputState_R_S;


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN int hStateful_Prog_SwEventReadCallback(void *pp);
EXTERN int hStateful_Prog_SwOutputStateReadCallback(void *pp);
EXTERN int hStateful_Prog_SwOutputStateWriteCallback(void *pp);
EXTERN int ccc_hStatefulProg_SwEventWriteCB(void *pp);
EXTERN int ccc_hStatefulProg_SwOutputStateWriteCB(void *pp);


#endif /* _HK_STATEFUL_PROG_SW_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
