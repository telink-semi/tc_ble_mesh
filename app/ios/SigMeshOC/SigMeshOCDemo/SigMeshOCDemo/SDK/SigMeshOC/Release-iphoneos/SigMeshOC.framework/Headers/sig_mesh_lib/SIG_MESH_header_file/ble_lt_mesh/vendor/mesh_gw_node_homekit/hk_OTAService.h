/********************************************************************************************************
 * @file     hk_OTAService.h
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
#ifndef _HK_OTA_SERVICE_H_
#define _HK_OTA_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_OTA_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif

#define OTA_VALID_DATA_CNT_MAX			(16 * 9) //the max value must be multiple of 16.
#define OTA_PACKET_DATA_CNT_MAX			OTA_VALID_DATA_CNT_MAX //the max value expand to 159.
#define OTA_DATA_GROUP_CNT_PER_PACKET		(OTA_VALID_DATA_CNT_MAX/16)



#define CHAR_OTA_SVC_SIGN_ENABLE      1
#if CHAR_OTA_SVC_SIGN_ENABLE
    #define OTA_AS_PRIMARY_SERVICE    	0
    #define OTA_IS_HIDDEN_SERVICE     	1
    #define OTA_LINKED_SERVICES_ENABLE	0
    EXTERN const hap_characteristic_desc_t hap_desc_char_ota_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_ota_A5_R_S;
    EXTERN unsigned char VAR_OTA_UNKOWN;
#else
    #define OTA_AS_PRIMARY_SERVICE    	0
    #define OTA_IS_HIDDEN_SERVICE     	0
    #define OTA_LINKED_SERVICES_ENABLE	0
#endif

#define CHAR_OTA_STATUSFAULT_ENABLE				0

#define OTA_DATA_IND_ENABLE						1
#define OTA_STATUSFAULT_IND_ENABLE				0

#define OTA_DATA_BRC_ENABLE						1
#define OTA_STATUSFAULT_BRC_ENABLE				0


#define OTA_DATA_NO      3
#if OTA_DATA_IND_ENABLE
    #define OTA_DATA_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hOtaData), ccc_OtaDataWriteCB, 0},
    #define OTA_DATA_IND_NO  1
#else
    #define OTA_DATA_IND_DESC
    #define OTA_DATA_IND_NO  0
#endif

#if CHAR_OTA_STATUSFAULT_ENABLE
    #define OTA_STATUSFAULT_NO 3
    #define CHAR_OTA_STATUSFAULT_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_OTA_STATUSFAULT_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof (hOtaStatusFault), 0, (u8*)(&charOtaStatusFaultUUID), (u8*)&hOtaStatusFault, 0, OtaStatusFaultReadCallback},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(hk_ota_insts + 3), 0, 0}, \
            OTA_STATUSFAULT_IND_DESC

    #if OTA_STATUSFAULT_IND_ENABLE
        #define OTA_STATUSFAULT_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hOtaStatusFault), ccc_OtaStatusFaultWriteCB, 0},
        #define OTA_STATUSFAULT_IND_NO  1
    #else
        #define OTA_STATUSFAULT_IND_DESC
        #define OTA_STATUSFAULT_IND_NO  0
    #endif
#else
    #define OTA_STATUSFAULT_NO 0
    #define CHAR_OTA_STATUSFAULT_ATT_TABLE
    #define OTA_STATUSFAULT_IND_NO  0
#endif


#if CHAR_OTA_SVC_SIGN_ENABLE
    #define OTA_SVC_SIGN_NO   3
    #define CHAR_OTA_SVC_A5_SIGN_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_ota_A5_R_S, &hap_desc_service_ota_A5_R_S},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_OTA_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_OTA_UNKOWN, 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(hk_ota_insts + 4), 0, 0},
#else
    #define OTA_SVC_SIGN_NO   0
    #define CHAR_OTA_SVC_A5_SIGN_TABLE
#endif

#define OTA_NO_DELTA   OTA_DATA_IND_NO \
						+ OTA_STATUSFAULT_IND_NO

#define OTA_NO   9 + OTA_STATUSFAULT_NO \
					+ OTA_SVC_SIGN_NO

#define OTA_HANDLE_NUM (OTA_NO + OTA_NO_DELTA)

#define HOMEKIT_OTA_SERVICE_ATT_TABLE   \
    {OTA_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(otaServiceUUID),sizeof(otaServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(otaServiceUUID), 0, 0},   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(hk_ota_insts + 0), 0, 0},   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_NAME, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(hOtaServiceName), 0, (u8*)(nameUUID), (u8*)(hOtaServiceName), 0, 0},  \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(hk_ota_insts + 1), 0, 0}, \
		{0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_OTA_DATA_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hOtaData),0, (u8*)(charOtaDataUUID), (u8*)&hOtaData, OtaDataWriteCallback, OtaDataReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(hk_ota_insts + 2), 0, 0},  \
            OTA_DATA_IND_DESC \
        CHAR_OTA_STATUSFAULT_ATT_TABLE \
        CHAR_OTA_SVC_A5_SIGN_TABLE


#define OTA_HANDLE_COMMON_NUM     8

#define OTA_HANDLE_NUM_DATA					OTA_HANDLE_START_NUM +OTA_HANDLE_COMMON_NUM
#define OTA_HANDLE_NUM_STATUSFAULT				OTA_HANDLE_NUM_DATA +OTA_DATA_NO +OTA_DATA_IND_NO
#define OTA_HANDLE_NUM_THE_END


#define OTA_STORE_CHAR_VALUE_T_DATA							//u32 OtaData:1;     /* 0~1 */
#define OTA_STORE_CHAR_VALUE_T_DATA_STORE					//STORE_CHAR(OtaData, hOtaData)
#define OTA_STORE_CHAR_VALUE_T_DATA_RESTORE					//RESTORE_CHAR(OtaData, hOtaData)

#if CHAR_OTA_STATUSFAULT_ENABLE
#define OTA_STORE_CHAR_VALUE_T_STATUSFAULT						u32 OtaStatusFault:1;   /* 0~1 */
#define OTA_STORE_CHAR_VALUE_T_STATUSFAULT_STORE				STORE_CHAR(OtaStatusFault, hOtaStatusFault)
#define OTA_STORE_CHAR_VALUE_T_STATUSFAULT_RESTORE				RESTORE_CHAR(OtaStatusFault, hOtaStatusFault)
#else
#define OTA_STORE_CHAR_VALUE_T_STATUSFAULT
#define OTA_STORE_CHAR_VALUE_T_STATUSFAULT_STORE
#define OTA_STORE_CHAR_VALUE_T_STATUSFAULT_RESTORE
#endif


#define OTA_STORE_CHAR_VALUE_T            \
        OTA_STORE_CHAR_VALUE_T_DATA \
        OTA_STORE_CHAR_VALUE_T_STATUSFAULT

#define OTA_STORE_CHAR_TO_FLASH           \
        OTA_STORE_CHAR_VALUE_T_DATA_STORE \
        OTA_STORE_CHAR_VALUE_T_STATUSFAULT_STORE

#define OTA_RESTORE_CHAR_FROM_FLASH            \
        OTA_STORE_CHAR_VALUE_T_DATA_RESTORE \
        OTA_STORE_CHAR_VALUE_T_STATUSFAULT_RESTORE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN unsigned char hOtaData;
EXTERN unsigned char hOtaStatusFault;

EXTERN unsigned short ccc_hOtaData;
EXTERN unsigned short ccc_hOtaStatusFault;

EXTERN const unsigned char otaServiceUUID[16];
EXTERN const unsigned char charOtaDataUUID[16];
EXTERN const unsigned char charOtaStatusFaultUUID[16];

EXTERN const u16 hk_ota_insts[6];


#if OTA_DATA_BRC_ENABLE
    #define BC_PARA_OTA_DATA ((u8*)&bc_para_hOtaData)
    EXTERN unsigned char bc_para_hOtaData;
#else
    #define BC_PARA_OTA_DATA 0
#endif

#if OTA_STATUSFAULT_BRC_ENABLE
    #define BC_PARA_OTA_STATUSFAULT ((u8*)&bc_para_hOtaStatusFault)
    EXTERN unsigned char bc_para_hOtaStatusFault;
#else
    #define BC_PARA_OTA_STATUSFAULT 0
#endif

#if OTA_LINKED_SERVICES_ENABLE
	#define OTA_LINKED_SVCS			hOta_linked_svcs
	#define OTA_LINKED_SVCS_LENGTH 	(sizeof(hOta_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * hOta_linked_svcs[];
#else
	#define OTA_LINKED_SVCS			0
	#define OTA_LINKED_SVCS_LENGTH 	0
#endif

EXTERN const u8 hOtaServiceName[10];

EXTERN const hap_characteristic_desc_t hap_desc_char_OTA_DATA_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_OTA_STATUSFAULT_R_S;


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN int OtaDataReadCallback(void *pp);
EXTERN int OtaDataWriteCallback(void *pp);
EXTERN int OtaStatusFaultReadCallback(void *pp);

EXTERN int ccc_OtaDataWriteCB(void *pp);
EXTERN int ccc_OtaStatusFaultWriteCB(void *pp);

#endif /* _HK_OTA_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
