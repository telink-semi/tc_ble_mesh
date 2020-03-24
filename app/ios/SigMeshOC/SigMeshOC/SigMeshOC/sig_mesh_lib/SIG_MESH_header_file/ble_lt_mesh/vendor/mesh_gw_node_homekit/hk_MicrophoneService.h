/********************************************************************************************************
 * @file     hk_MicrophoneService.h
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
#ifndef _HK_MICROPHONE_SERVICE_H_
#define _HK_MICROPHONE_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */
//-#include "../../homekit_src/homekit_inc.h"


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_MICROPHONE_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif

#define CHAR_MICROPHONE_SVC_SIGN_ENABLE      0
#if CHAR_MICROPHONE_SVC_SIGN_ENABLE
    #define MICROPHONE_AS_PRIMARY_SERVICE    	0
    #define MICROPHONE_IS_HIDDEN_SERVICE     	0
    #define MICROPHONE_LINKED_SERVICES_ENABLE	0
    EXTERN const hap_characteristic_desc_t hap_desc_char_microphone_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_microphone_A5_R_S;
    EXTERN unsigned char VAR_MICROPHONE_UNKOWN;
#else
    #define MICROPHONE_AS_PRIMARY_SERVICE	    0
    #define MICROPHONE_IS_HIDDEN_SERVICE     	0
    #define MICROPHONE_LINKED_SERVICES_ENABLE	0
#endif


#define MICROPHONE_MUTE_IND_ENABLE      0
#define MICROPHONE_VOLUME_IND_ENABLE    0

#define MICROPHONE_MUTE_BRC_ENABLE      0
#define MICROPHONE_VOLUME_BRC_ENABLE    0


#define MICROPHONE_MUTE_NO 3
#if MICROPHONE_MUTE_IND_ENABLE
    #define MICROPHONE_MUTE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hMicrophoneMute), ccc_hMicrophoneMuteWriteCB, 0},
    #define MICROPHONE_MUTE_IND_NO  1
#else
    #define MICROPHONE_MUTE_IND_DESC
    #define MICROPHONE_MUTE_IND_NO  0
#endif

#define MICROPHONE_VOLUME_NO 3
#if MICROPHONE_VOLUME_IND_ENABLE
    #define MICROPHONE_VOLUME_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hMicrophoneVolume), ccc_hMicrophoneVolumeWriteCB, 0},
    #define MICROPHONE_VOLUME_IND_NO  1
#else
    #define MICROPHONE_VOLUME_IND_DESC
    #define MICROPHONE_VOLUME_IND_NO  0
#endif


#if CHAR_MICROPHONE_SVC_SIGN_ENABLE
#define CHAR_MICROPHONE_SVC_A5_SIGN_TABLE   \
    {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_microphone_A5_R_S, &hap_desc_service_microphone_A5_R_S},  \
    {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_MICROPHONE_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_MICROPHONE_UNKOWN, 0, 0},    \
        {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + MICROPHONE_INST_NO + 4), 0, 0},
#define MICROPHONE_SVC_SIGN_NO   3
#else
#define CHAR_MICROPHONE_SVC_A5_SIGN_TABLE
#define MICROPHONE_SVC_SIGN_NO   0
#endif

#define MICROPHONE_NO_DELTA MICROPHONE_MUTE_IND_NO    \
                            +MICROPHONE_VOLUME_IND_NO

#define MICROPHONE_NO       12 +MICROPHONE_SVC_SIGN_NO
#define MICROPHONE_HANDLE_NUM (MICROPHONE_NO + MICROPHONE_NO_DELTA)


#define HOMEKIT_MICROPHONE_SERVICE_ATT_TABLE    \
    {MICROPHONE_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(microphoneServiceUUID),sizeof(microphoneServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(microphoneServiceUUID), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(insts + MICROPHONE_INST_NO), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_NAME, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(hMicrophoneServiceName), 0, (u8*)(nameUUID), (u8*)(hMicrophoneServiceName), 0, 0},  \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + MICROPHONE_INST_NO + 1), 0, 0}, \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hMicrophoneMute_R_S, 0}, \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hMicrophoneMute),0, (u8*)(charMuteUUID), (u8*)&hMicrophoneMute, MicrophoneMuteWriteCallback, MicrophoneMuteReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + MICROPHONE_INST_NO + 2), 0, 0},   \
            MICROPHONE_MUTE_IND_DESC \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hMicrophoneVolume_R_S, 0},   \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hMicrophoneVolume),0, (u8*)(charVolumeUUID), (u8*)&hMicrophoneVolume, MicrophoneVolumeWriteCallback, MicrophoneVolumeReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + MICROPHONE_INST_NO + 3), 0, 0},   \
            MICROPHONE_VOLUME_IND_DESC \
        CHAR_MICROPHONE_SVC_A5_SIGN_TABLE


#define MICROPHONE_HANDLE_COMMON_NUM     8

#define MICROPHONE_HANDLE_NUM_MUTE          MICROPHONE_HANDLE_START_NUM +MICROPHONE_HANDLE_COMMON_NUM
#define MICROPHONE_HANDLE_NUM_VOLUME        MICROPHONE_HANDLE_NUM_MUTE +MICROPHONE_MUTE_NO +MICROPHONE_MUTE_IND_NO
#define MICROPHONE_HANDLE_NUM_THE_END


#define MICROPHONE_STORE_CHAR_VALUE_T_MICROPHONEMUTE             u32 MicrophoneMute:1; /* 0~1 */
#define MICROPHONE_STORE_CHAR_VALUE_T_MICROPHONEMUTE_STORE           STORE_CHAR(MicrophoneMute,hMicrophoneMute)
#define MICROPHONE_STORE_CHAR_VALUE_T_MICROPHONEMUTE_RESTORE         RESTORE_CHAR(MicrophoneMute,hMicrophoneMute)

#define MICROPHONE_STORE_CHAR_VALUE_T_MICROPHONEVOLUME           u32 MicrophoneVolume:1; /* 0~100 */
#define MICROPHONE_STORE_CHAR_VALUE_T_MICROPHONEVOLUME_STORE         STORE_CHAR(MicrophoneVolume,hMicrophoneVolume)
#define MICROPHONE_STORE_CHAR_VALUE_T_MICROPHONEVOLUME_RESTORE       RESTORE_CHAR(MicrophoneVolume,hMicrophoneVolume)

#define MICROPHONE_STORE_CHAR_VALUE_T            \
                    MICROPHONE_STORE_CHAR_VALUE_T_MICROPHONEMUTE   \
                    MICROPHONE_STORE_CHAR_VALUE_T_MICROPHONEVOLUME

#define MICROPHONE_STORE_CHAR_TO_FLASH           \
                    MICROPHONE_STORE_CHAR_VALUE_T_MICROPHONEMUTE_STORE \
                    MICROPHONE_STORE_CHAR_VALUE_T_MICROPHONEVOLUME_STORE

#define MICROPHONE_RESTORE_CHAR_FROM_FLASH       \
                    MICROPHONE_STORE_CHAR_VALUE_T_MICROPHONEMUTE_RESTORE   \
                    MICROPHONE_STORE_CHAR_VALUE_T_MICROPHONEVOLUME_RESTORE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN unsigned char hMicrophoneMute;
EXTERN unsigned char hMicrophoneVolume;

EXTERN unsigned short ccc_hMicrophoneMute;
EXTERN unsigned short ccc_hMicrophoneVolume;

#if MICROPHONE_MUTE_BRC_ENABLE
    #define BC_PARA_MICROPHONE_MUTE ((u8*)&bc_para_hMicrophoneMute)
    EXTERN unsigned char bc_para_hMicrophoneMute;
#else
    #define BC_PARA_MICROPHONE_MUTE 0
#endif

#if MICROPHONE_VOLUME_BRC_ENABLE
    #define BC_PARA_MICROPHONE_VOLUME ((u8*)&bc_para_hMicrophoneVolume)
    EXTERN unsigned char bc_para_hMicrophoneVolume;
#else
    #define BC_PARA_MICROPHONE_VOLUME 0
#endif

#if MICROPHONE_LINKED_SERVICES_ENABLE
	#define MICROPHONE_LINKED_SVCS			hMicrophone_linked_svcs
	#define MICROPHONE_LINKED_SVCS_LENGTH 	(sizeof(hMicrophone_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * hMicrophone_linked_svcs[];
#else
	#define MICROPHONE_LINKED_SVCS			0
	#define MICROPHONE_LINKED_SVCS_LENGTH 	0
#endif

EXTERN const u8 hMicrophoneServiceName[17];

EXTERN const hap_characteristic_desc_t hap_desc_char_hMicrophoneMute_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hMicrophoneVolume_R_S;


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN int MicrophoneMuteWriteCallback(void *pp);
EXTERN int MicrophoneMuteReadCallback(void *pp);
EXTERN int MicrophoneVolumeWriteCallback(void *pp);
EXTERN int MicrophoneVolumeReadCallback(void *pp);
EXTERN int ccc_hMicrophoneMuteWriteCB(void *pp);
EXTERN int ccc_hMicrophoneVolumeWriteCB(void *pp);

#endif /* _HK_MICROPHONE_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
