/********************************************************************************************************
 * @file     hk_SpeakerService.h
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
#ifndef _HK_SPEAKER_SERVICE_H_
#define _HK_SPEAKER_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */
//-#include "../../homekit_src/homekit_inc.h"


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_SPEAKER_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif

#define CHAR_SPEAKER_SVC_SIGN_ENABLE      0
#if CHAR_SPEAKER_SVC_SIGN_ENABLE
    #define SPEAKER_AS_PRIMARY_SERVICE    	0
    #define SPEAKER_IS_HIDDEN_SERVICE     	0
    #define SPEAKER_LINKED_SERVICES_ENABLE	0
    EXTERN const hap_characteristic_desc_t hap_desc_char_speaker_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_speaker_A5_R_S;
    EXTERN unsigned char VAR_SPEAKER_UNKOWN;
#else
    #define SPEAKER_AS_PRIMARY_SERVICE    	0
    #define SPEAKER_IS_HIDDEN_SERVICE     	0
    #define SPEAKER_LINKED_SERVICES_ENABLE	0
#endif


#define CHAR_SPEAKER_VOLUME_ENABLE   1

#define SPEAKER_MUTE_IND_ENABLE      1
#define SPEAKER_VOLUME_IND_ENABLE    1

#define SPEAKER_MUTE_BRC_ENABLE      0
#define SPEAKER_VOLUME_BRC_ENABLE    0


#define SPEAKER_MUTE_NO 3
#if SPEAKER_MUTE_IND_ENABLE
    #define SPEAKER_MUTE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSpeakerMute), ccc_hSpeakerMuteWriteCB, 0},
    #define SPEAKER_MUTE_IND_NO  1
#else
    #define SPEAKER_MUTE_IND_DESC
    #define SPEAKER_MUTE_IND_NO  0
#endif

#if CHAR_SPEAKER_VOLUME_ENABLE
    #define SPEAKER_VOLUME_NO 3
    #define CHAR_SPEAKER_VOLUME_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSpeakerVolume_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSpeakerVolume),0, (u8*)(charVolumeUUID), (u8*)&hSpeakerVolume, SpeakerVolumeWriteCallback, SpeakerVolumeReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SPEAKER_INST_NO + 3), 0, 0},  \
            SPEAKER_VOLUME_IND_DESC

    #if SPEAKER_VOLUME_IND_ENABLE
        #define SPEAKER_VOLUME_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSpeakerVolume), ccc_hSpeakerVolumeWriteCB, 0},
        #define SPEAKER_VOLUME_IND_NO  1
    #else
        #define SPEAKER_VOLUME_IND_DESC
        #define SPEAKER_VOLUME_IND_NO  0
    #endif
#else
    #define SPEAKER_VOLUME_NO 0
    #define CHAR_SPEAKER_VOLUME_ATT_TABLE
    #define SPEAKER_VOLUME_IND_NO  0
#endif


#if CHAR_SPEAKER_SVC_SIGN_ENABLE
#define CHAR_SPEAKER_SVC_A5_SIGN_TABLE   \
    {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_speaker_A5_R_S, &hap_desc_service_speaker_A5_R_S},  \
    {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_SPEAKER_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_SPEAKER_UNKOWN, 0, 0},    \
        {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SPEAKER_INST_NO + 4), 0, 0},
#define SPEAKER_SVC_SIGN_NO   3
#else
#define CHAR_SPEAKER_SVC_A5_SIGN_TABLE
#define SPEAKER_SVC_SIGN_NO   0
#endif

#define SPEAKER_NO_DELTA    SPEAKER_MUTE_IND_NO    \
                            +SPEAKER_VOLUME_IND_NO

#define SPEAKER_NO          9 +SPEAKER_VOLUME_NO +SPEAKER_SVC_SIGN_NO
#define SPEAKER_HANDLE_NUM (SPEAKER_NO + SPEAKER_NO_DELTA)


#define HOMEKIT_SPEAKER_SERVICE_ATT_TABLE   \
    {SPEAKER_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(speakerServiceUUID),sizeof(speakerServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(speakerServiceUUID), 0, 0},   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(insts + SPEAKER_INST_NO), 0, 0},   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_NAME, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(hSpeakerServiceName), 0, (u8*)(nameUUID), (u8*)(hSpeakerServiceName), 0, 0},  \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + SPEAKER_INST_NO + 1), 0, 0}, \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSpeakerMute_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSpeakerMute),0, (u8*)(charMuteUUID), (u8*)&hSpeakerMute, SpeakerMuteWriteCallback, SpeakerMuteReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SPEAKER_INST_NO + 2), 0, 0},  \
            SPEAKER_MUTE_IND_DESC \
        CHAR_SPEAKER_VOLUME_ATT_TABLE \
        CHAR_SPEAKER_SVC_A5_SIGN_TABLE


#define SPEAKER_HANDLE_COMMON_NUM     8

#define SPEAKER_HANDLE_NUM_MUTE          SPEAKER_HANDLE_START_NUM +SPEAKER_HANDLE_COMMON_NUM
#define SPEAKER_HANDLE_NUM_VOLUME        SPEAKER_HANDLE_NUM_MUTE +SPEAKER_MUTE_NO +SPEAKER_MUTE_IND_NO
#define SPEAKER_HANDLE_NUM_THE_END


#define SPEAKER_STORE_CHAR_VALUE_T_SPEAKERMUTE             u32 SpeakerMute:1; /* 0~1 */
#define SPEAKER_STORE_CHAR_VALUE_T_SPEAKERMUTE_STORE           STORE_CHAR(SpeakerMute,hSpeakerMute)
#define SPEAKER_STORE_CHAR_VALUE_T_SPEAKERMUTE_RESTORE         RESTORE_CHAR(SpeakerMute,hSpeakerMute)

#if CHAR_SPEAKER_VOLUME_ENABLE
#define SPEAKER_STORE_CHAR_VALUE_T_SPEAKERVOLUME           u32 SpeakerVolume:1; /* 0~100 */
#define SPEAKER_STORE_CHAR_VALUE_T_SPEAKERVOLUME_STORE         STORE_CHAR(SpeakerVolume,hSpeakerVolume)
#define SPEAKER_STORE_CHAR_VALUE_T_SPEAKERVOLUME_RESTORE       RESTORE_CHAR(SpeakerVolume,hSpeakerVolume)
#else
#define SPEAKER_STORE_CHAR_VALUE_T_SPEAKERVOLUME
#define SPEAKER_STORE_CHAR_VALUE_T_SPEAKERVOLUME_STORE
#define SPEAKER_STORE_CHAR_VALUE_T_SPEAKERVOLUME_RESTORE
#endif

#define SPEAKER_STORE_CHAR_VALUE_T            \
                    SPEAKER_STORE_CHAR_VALUE_T_SPEAKERMUTE   \
                    SPEAKER_STORE_CHAR_VALUE_T_SPEAKERVOLUME

#define SPEAKER_STORE_CHAR_TO_FLASH           \
                    SPEAKER_STORE_CHAR_VALUE_T_SPEAKERMUTE_STORE \
                    SPEAKER_STORE_CHAR_VALUE_T_SPEAKERVOLUME_STORE

#define SPEAKER_RESTORE_CHAR_FROM_FLASH       \
                    SPEAKER_STORE_CHAR_VALUE_T_SPEAKERMUTE_RESTORE   \
                    SPEAKER_STORE_CHAR_VALUE_T_SPEAKERVOLUME_RESTORE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN unsigned char hSpeakerMute;
EXTERN unsigned char hSpeakerVolume;

EXTERN unsigned short ccc_hSpeakerMute;
EXTERN unsigned short ccc_hSpeakerVolume;

#if SPEAKER_MUTE_BRC_ENABLE
    #define BC_PARA_SPEAKER_MUTE ((u8*)&bc_para_hSpeakerMute)
    EXTERN unsigned char bc_para_hSpeakerMute;
#else
    #define BC_PARA_SPEAKER_MUTE 0
#endif

#if SPEAKER_VOLUME_BRC_ENABLE
    #define BC_PARA_SPEAKER_VOLUME ((u8*)&bc_para_hSpeakerVolume)
    EXTERN unsigned char bc_para_hSpeakerVolume;
#else
    #define BC_PARA_SPEAKER_VOLUME 0
#endif

#if SPEAKER_LINKED_SERVICES_ENABLE
	#define SPEAKER_LINKED_SVCS			hSpeaker_linked_svcs
	#define SPEAKER_LINKED_SVCS_LENGTH 	(sizeof(hSpeaker_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * hSpeaker_linked_svcs[];
#else
	#define SPEAKER_LINKED_SVCS			0
	#define SPEAKER_LINKED_SVCS_LENGTH 	0
#endif

EXTERN const u8 hSpeakerServiceName[14];

EXTERN const hap_characteristic_desc_t hap_desc_char_hSpeakerMute_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSpeakerVolume_R_S;


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN int SpeakerMuteWriteCallback(void *pp);
EXTERN int SpeakerMuteReadCallback(void *pp);
EXTERN int SpeakerVolumeWriteCallback(void *pp);
EXTERN int SpeakerVolumeReadCallback(void *pp);
EXTERN int ccc_hSpeakerMuteWriteCB(void *pp);
EXTERN int ccc_hSpeakerVolumeWriteCB(void *pp);

#endif /* _HK_SPEAKER_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
