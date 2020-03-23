/********************************************************************************************************
 * @file     hk_OutletService.h
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
#ifndef _HK_OUTLET_SERVICE_H_
#define _HK_OUTLET_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */
//-#include "../../homekit_src/homekit_inc.h"


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_OUTLET_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif


#define CHAR_OUTLET_SVC_SIGN_ENABLE      0
#if CHAR_OUTLET_SVC_SIGN_ENABLE
    #define OUTLET_AS_PRIMARY_SERVICE    	0
    #define OUTLET_IS_HIDDEN_SERVICE     	0
    #define OUTLET_LINKED_SERVICES_ENABLE	0
    EXTERN const hap_characteristic_desc_t hap_desc_char_outlet_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_outlet_A5_R_S;
    EXTERN unsigned char VAR_OUTLET_UNKOWN;
#else
    #define OUTLET_AS_PRIMARY_SERVICE    	0
    #define OUTLET_IS_HIDDEN_SERVICE     	0
    #define OUTLET_LINKED_SERVICES_ENABLE	0
#endif


#define OUTLET_ON_IND_ENABLE       1
#define OUTLET_IN_USE_IND_ENABLE   1

#define OUTLET_ON_BRC_ENABLE        0
#define OUTLET_IN_USE_BRC_ENABLE    0


#define OUTLET_ON_NO 3
#if OUTLET_ON_IND_ENABLE
    #define OUTLET_ON_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hOuletOn), ccc_hOuletOnWriteCB, 0},
    #define OUTLET_ON_IND_NO  1
#else
    #define OUTLET_ON_IND_DESC
    #define OUTLET_ON_IND_NO  0
#endif

#define OUTLET_INUSE_NO 3
#if OUTLET_IN_USE_IND_ENABLE
    #define OUTLET_IN_USE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hOutletInUse), ccc_hOutletInUseWriteCB, 0},
    #define OUTLET_IN_USE_IND_NO  1
#else
    #define OUTLET_IN_USE_IND_DESC
    #define OUTLET_IN_USE_IND_NO  0
#endif


#if CHAR_OUTLET_SVC_SIGN_ENABLE
#define CHAR_OUTLET_SVC_A5_SIGN_TABLE   \
    {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_outlet_A5_R_S, &hap_desc_service_outlet_A5_R_S},  \
    {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_OUTLET_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_OUTLET_UNKOWN, 0, 0},    \
        {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + OUTLET_INST_NO + 4), 0, 0},
#define OUTLET_SVC_SIGN_NO   3
#else
#define CHAR_OUTLET_SVC_A5_SIGN_TABLE
#define OUTLET_SVC_SIGN_NO   0
#endif

#define OUTLET_NO_DELTA OUTLET_ON_IND_NO    \
                        +OUTLET_IN_USE_IND_NO

#define OUTLET_NO       12 +OUTLET_SVC_SIGN_NO
#define OUTLET_HANDLE_NUM (OUTLET_NO + OUTLET_NO_DELTA)


#define HOMEKIT_OUTLET_SERVICE_ATT_TABLE    \
    {OUTLET_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(outletServiceUUID),sizeof(outletServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(outletServiceUUID), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(insts + OUTLET_INST_NO), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_NAME, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(hOutletServiceName), 0, (u8*)(nameUUID), (u8*)(hOutletServiceName), 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + OUTLET_INST_NO + 1), 0, 0},  \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_OUTLET_ON_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hOuletOn),0, (u8*)(charOnUUID), (u8*)&hOuletOn, OuletOnWriteCallback, OuletOnReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + OUTLET_INST_NO + 2), 0, 0},   \
            OUTLET_ON_IND_DESC \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_OUTLET_IN_USE_R_S, 0},   \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hOutletInUse),0, (u8*)(charOutletInUseUUID), (u8*)&hOutletInUse, 0, OutletInUseReadCallback},  \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + OUTLET_INST_NO + 3), 0, 0},	\
            OUTLET_IN_USE_IND_DESC \
        CHAR_OUTLET_SVC_A5_SIGN_TABLE


#define OUTLET_HANDLE_COMMON_NUM     8

#define OUTLET_HANDLE_NUM_ON            OUTLET_HANDLE_START_NUM +OUTLET_HANDLE_COMMON_NUM
#define OUTLET_HANDLE_NUM_INUSE         OUTLET_HANDLE_NUM_ON +OUTLET_ON_NO +OUTLET_ON_IND_NO
#define OUTLET_HANDLE_NUM_THE_END


#define OUTLET_STORE_CHAR_VALUE_T_OULETON                u32 OuletOn:1; /* 0~1 */
#define OUTLET_STORE_CHAR_VALUE_T_OULETON_STORE              STORE_CHAR(OuletOn,hOuletOn)
#define OUTLET_STORE_CHAR_VALUE_T_OULETON_RESTORE            RESTORE_CHAR(OuletOn,hOuletOn)

#define OUTLET_STORE_CHAR_VALUE_T_OUTLETINUSE            u32 OutletInUse:1; /* 0~1 */
#define OUTLET_STORE_CHAR_VALUE_T_OUTLETINUSE_STORE          STORE_CHAR(OutletInUse,hOutletInUse)
#define OUTLET_STORE_CHAR_VALUE_T_OUTLETINUSE_RESTORE        RESTORE_CHAR(OutletInUse,hOutletInUse)

#define OUTLET_STORE_CHAR_VALUE_T            \
                    OUTLET_STORE_CHAR_VALUE_T_OULETON   \
                    OUTLET_STORE_CHAR_VALUE_T_OUTLETINUSE

#define OUTLET_STORE_CHAR_TO_FLASH           \
                    OUTLET_STORE_CHAR_VALUE_T_OULETON_STORE \
                    OUTLET_STORE_CHAR_VALUE_T_OUTLETINUSE_STORE

#define OUTLET_RESTORE_CHAR_FROM_FLASH       \
                    OUTLET_STORE_CHAR_VALUE_T_OULETON_RESTORE   \
                    OUTLET_STORE_CHAR_VALUE_T_OUTLETINUSE_RESTORE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN unsigned char hOuletOn;
EXTERN unsigned char hOutletInUse;

EXTERN unsigned short ccc_hOuletOn;
EXTERN unsigned short ccc_hOutletInUse;

#if OUTLET_ON_BRC_ENABLE
    #define BC_PARA_OUTLET_ON ((u8*)&bc_para_hOuletOn)
    EXTERN unsigned char bc_para_hOuletOn;
#else
    #define BC_PARA_OUTLET_ON 0
#endif

#if OUTLET_IN_USE_BRC_ENABLE
    #define BC_PARA_OUTLET_IN_USE ((u8*)&bc_para_hOutletInUse)
    EXTERN unsigned char bc_para_hOutletInUse;
#else
    #define BC_PARA_OUTLET_IN_USE 0
#endif

#if OUTLET_LINKED_SERVICES_ENABLE
	#define OUTLET_LINKED_SVCS			hOutlet_linked_svcs
	#define OUTLET_LINKED_SVCS_LENGTH 	(sizeof(hOutlet_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * hOutlet_linked_svcs[];
#else
	#define OUTLET_LINKED_SVCS			0
	#define OUTLET_LINKED_SVCS_LENGTH 	0
#endif

EXTERN const u8 hOutletServiceName[13];

EXTERN const hap_characteristic_desc_t hap_desc_char_OUTLET_ON_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_OUTLET_IN_USE_R_S;

/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN int OuletOnReadCallback(void *pp);
EXTERN int OuletOnWriteCallback(void *pp);
EXTERN int OutletInUseReadCallback(void *pp);
EXTERN int ccc_hOuletOnWriteCB(void *pp);
EXTERN int ccc_hOutletInUseWriteCB(void *pp);

#endif /* _HK_OUTLET_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
