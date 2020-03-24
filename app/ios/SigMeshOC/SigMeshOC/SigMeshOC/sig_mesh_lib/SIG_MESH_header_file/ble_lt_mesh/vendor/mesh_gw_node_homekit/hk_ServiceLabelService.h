/********************************************************************************************************
 * @file     hk_ServiceLabelService.h
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
#ifndef _HK_SERVICE_LABEL_SERVICE_H_
#define _HK_SERVICE_LABEL_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_SERVICE_LABEL_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif

#define CHAR_SERVICE_LABEL_SVC_SIGN_ENABLE      0
#if CHAR_SERVICE_LABEL_SVC_SIGN_ENABLE
    #define SERVICE_LABEL_AS_PRIMARY_SERVICE    	0
    #define SERVICE_LABEL_IS_HIDDEN_SERVICE     	0
    #define SERVICE_LABEL_LINKED_SERVICES_ENABLE	0
    EXTERN const hap_characteristic_desc_t hap_desc_char_service_label_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_service_label_A5_R_S;
    EXTERN unsigned char VAR_SERVICE_LABEL_UNKOWN;
#else
    #define SERVICE_LABEL_AS_PRIMARY_SERVICE    	0
    #define SERVICE_LABEL_IS_HIDDEN_SERVICE     	0
    #define SERVICE_LABEL_LINKED_SERVICES_ENABLE	0
#endif


#if CHAR_SERVICE_LABEL_SVC_SIGN_ENABLE
    #define SERVICE_LABEL_SVC_SIGN_NO   3
    #define CHAR_SERVICE_LABEL_SVC_A5_SIGN_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_service_label_A5_R_S, &hap_desc_service_service_label_A5_R_S},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_SERVICE_LABEL_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_SERVICE_LABEL_UNKOWN, 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SERVICE_LABEL_INST_NO + 2), 0, 0},
#else
    #define SERVICE_LABEL_SVC_SIGN_NO   0
    #define CHAR_SERVICE_LABEL_SVC_A5_SIGN_TABLE
#endif

#define SERVICE_LABEL_NO_DELTA    0
#define SERVICE_LABEL_NO          6 +SERVICE_LABEL_SVC_SIGN_NO
#define SERVICE_LABEL_HANDLE_NUM (SERVICE_LABEL_NO + SERVICE_LABEL_NO_DELTA)


#define HOMEKIT_SERVICE_LABEL_SERVICE_ATT_TABLE   \
    {SERVICE_LABEL_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(service_labelServiceUUID),sizeof(service_labelServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(service_labelServiceUUID), 0, 0},   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(insts + SERVICE_LABEL_INST_NO), 0, 0},   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_ServiceLabelNamespace_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,1,0, (u8*)(charServiceLabelNamespaceUUID), (u8*)&hServiceLabelNamespace, 0, ServiceLabelNamespaceReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SERVICE_LABEL_INST_NO + 1), 0, 0},  \
        CHAR_SERVICE_LABEL_SVC_A5_SIGN_TABLE


#define SERVICE_LABEL_HANDLE_COMMON_NUM     5

#define SERVICE_LABEL_HANDLE_NUM_HSERVICE_LABEL_NAMESPACE        SERVICE_LABEL_HANDLE_START_NUM +SERVICE_LABEL_HANDLE_COMMON_NUM
#define SERVICE_LABEL_HANDLE_NUM_THE_END


#define SERVICE_LABEL_STORE_CHAR_VALUE_T_HSERVICE_LABEL_NAMESPACE            u32 ServiceLabelNamespace:1;    /* 0~1 */
#define SERVICE_LABEL_STORE_CHAR_VALUE_T_HSERVICE_LABEL_NAMESPACE_STORE          STORE_CHAR(ServiceLabelNamespace,hServiceLabelNamespace)
#define SERVICE_LABEL_STORE_CHAR_VALUE_T_HSERVICE_LABEL_NAMESPACE_RESTORE        RESTORE_CHAR(ServiceLabelNamespace,hServiceLabelNamespace)


#define SERVICE_LABEL_STORE_CHAR_VALUE_T  \
                    SERVICE_LABEL_STORE_CHAR_VALUE_T_HSERVICE_LABEL_NAMESPACE

#define SERVICE_LABEL_STORE_CHAR_TO_FLASH  \
                    SERVICE_LABEL_STORE_CHAR_VALUE_T_HSERVICE_LABEL_NAMESPACE_STORE

#define SERVICE_LABEL_RESTORE_CHAR_FROM_FLASH  \
                    SERVICE_LABEL_STORE_CHAR_VALUE_T_HSERVICE_LABEL_NAMESPACE_RESTORE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN unsigned char hServiceLabelNamespace;

#if SERVICE_LABEL_LINKED_SERVICES_ENABLE
	#define SERVICE_LABEL_LINKED_SVCS			hServiceLabel_linked_svcs
	#define SERVICE_LABEL_LINKED_SVCS_LENGTH 	(sizeof(hServiceLabel_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * hServiceLabel_linked_svcs[];
#else
	#define SERVICE_LABEL_LINKED_SVCS			0
	#define SERVICE_LABEL_LINKED_SVCS_LENGTH 	0
#endif

EXTERN const u8 hServiceLabelServiceName[19];

EXTERN const hap_characteristic_desc_t hap_desc_char_ServiceLabelNamespace_R_S;


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN int ServiceLabelNamespaceReadCallback(void *pp);


#endif /* _HK_SERVICE_LABEL_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
