/********************************************************************************************************
 * @file     hk_DoorbellService.h
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
#ifndef _HK_DOORBELL_SERVICE_H_
#define _HK_DOORBELL_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */
//-#include "../../homekit_src/homekit_inc.h"


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_DOORBELL_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif

#define CHAR_DOORBELL_SVC_SIGN_ENABLE      0
#if CHAR_DOORBELL_SVC_SIGN_ENABLE
    #define DOORBELL_AS_PRIMARY_SERVICE    		0
    #define DOORBELL_IS_HIDDEN_SERVICE     		0
    #define DOORBELL_LINKED_SERVICES_ENABLE		0
    EXTERN const hap_characteristic_desc_t hap_desc_char_doorbell_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_doorbell_A5_R_S;
    EXTERN unsigned char VAR_DOORBELL_UNKOWN;
#else
    #define DOORBELL_AS_PRIMARY_SERVICE    		0
    #define DOORBELL_IS_HIDDEN_SERVICE     		0
    #define DOORBELL_LINKED_SERVICES_ENABLE		0
#endif


#define DOORBELL_PROGSWEVENT_IND_ENABLE     0
#define DOORBELL_VOLUME_IND_ENABLE          0
#define DOORBELL_BRIGHTNESS_IND_ENABLE      0

#define DOORBELL_PROGSWEVENT_BRC_ENABLE     0
#define DOORBELL_VOLUME_BRC_ENABLE          0
#define DOORBELL_BRIGHTNESS_BRC_ENABLE      0


#define DOORBELL_PROGSWEVENT_NO 3
#if DOORBELL_PROGSWEVENT_IND_ENABLE
    #define DOORBELL_PROGSWEVENT_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hDoorbellProgSwEvent), ccc_hDoorbellProgSwEventWriteCB, 0},
    #define DOORBELL_PROGSWEVENT_IND_NO  1
#else
    #define DOORBELL_PROGSWEVENT_IND_DESC
    #define DOORBELL_PROGSWEVENT_IND_NO  0
#endif

#define DOORBELL_VOLUME_NO 3
#if DOORBELL_VOLUME_IND_ENABLE
    #define DOORBELL_VOLUME_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hDoorbellVolume), ccc_hDoorbellVolumeWriteCB, 0},
    #define DOORBELL_VOLUME_IND_NO  1
#else
    #define DOORBELL_VOLUME_IND_DESC
    #define DOORBELL_VOLUME_IND_NO  0
#endif

#define DOORBELL_BRIGHTNESS_NO 3
#if DOORBELL_BRIGHTNESS_IND_ENABLE
    #define DOORBELL_BRIGHTNESS_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hDoorbellBrightness), ccc_hDoorbellBrightnessWriteCB, 0},
    #define DOORBELL_BRIGHTNESS_IND_NO  1
#else
    #define DOORBELL_BRIGHTNESS_IND_DESC
    #define DOORBELL_BRIGHTNESS_IND_NO  0
#endif

#if CHAR_DOORBELL_SVC_SIGN_ENABLE
#define CHAR_DOORBELL_SVC_A5_SIGN_TABLE   \
    {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_doorbell_A5_R_S, &hap_desc_service_doorbell_A5_R_S},  \
    {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_DOORBELL_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_DOORBELL_UNKOWN, 0, 0},    \
        {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + DOORBELL_INST_NO + 5), 0, 0},
#define DOORBELL_SVC_SIGN_NO   3
#else
#define CHAR_DOORBELL_SVC_A5_SIGN_TABLE
#define DOORBELL_SVC_SIGN_NO   0
#endif

#define DOORBELL_NO_DELTA   DOORBELL_PROGSWEVENT_IND_NO  \
                            +DOORBELL_VOLUME_IND_NO \
                            +DOORBELL_BRIGHTNESS_IND_NO

#define DOORBELL_NO         15 +DOORBELL_SVC_SIGN_NO
#define DOORBELL_HANDLE_NUM (DOORBELL_NO + DOORBELL_NO_DELTA)

#define HOMEKIT_DOORBELL_SERVICE_ATT_TABLE  \
    {DOORBELL_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(doorbellServiceUUID),sizeof(doorbellServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(doorbellServiceUUID), 0, 0},  \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(insts + DOORBELL_INST_NO), 0, 0},  \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_NAME, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(hDoorbellServiceName), 0, (u8*)(nameUUID), (u8*)(hDoorbellServiceName), 0, 0},  \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + DOORBELL_INST_NO + 1), 0, 0}, \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hDoorbellProgSwEvent_R_S, 0}, \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hDoorbellProgSwEvent),0, (u8*)(charProgSwEventUUID), (u8*)&hDoorbellProgSwEvent, 0, DoorbellProgSwEventReadCallback},  \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + DOORBELL_INST_NO + 2), 0, 0}, \
            DOORBELL_PROGSWEVENT_IND_DESC \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hDoorbellVolume_R_S, 0}, \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hDoorbellVolume),0, (u8*)(charVolumeUUID), (u8*)&hDoorbellVolume, DoorbellVolumeWriteCallback, DoorbellVolumeReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + DOORBELL_INST_NO + 3), 0, 0}, \
            DOORBELL_VOLUME_IND_DESC \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hDoorbellBrightness_R_S, 0}, \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hDoorbellBrightness),0, (u8*)(charBrightnessUUID), (u8*)&hDoorbellBrightness, DoorbellBrightnessWriteCallback, DoorbellBrightnessReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + DOORBELL_INST_NO + 4), 0, 0}, \
            DOORBELL_BRIGHTNESS_IND_DESC \
        CHAR_DOORBELL_SVC_A5_SIGN_TABLE


#define DOORBELL_HANDLE_COMMON_NUM     8

#define DOORBELL_HANDLE_NUM_PROGSWEVENT     DOORBELL_HANDLE_START_NUM +DOORBELL_HANDLE_COMMON_NUM
#define DOORBELL_HANDLE_NUM_VOLUME          DOORBELL_HANDLE_NUM_PROGSWEVENT +DOORBELL_PROGSWEVENT_NO +DOORBELL_PROGSWEVENT_IND_NO
#define DOORBELL_HANDLE_NUM_BRIGHTNESS      DOORBELL_HANDLE_NUM_VOLUME +DOORBELL_VOLUME_NO +DOORBELL_VOLUME_IND_NO
#define DOORBELL_HANDLE_NUM_THE_END


#define DOORBELL_STORE_CHAR_VALUE_T_DOORBELLPROGSWEVENT             u32 DoorbellProgSwEvent:1; /* 0~1 */
#define DOORBELL_STORE_CHAR_VALUE_T_DOORBELLPROGSWEVENT_STORE           STORE_CHAR(DoorbellProgSwEvent,hDoorbellProgSwEvent)
#define DOORBELL_STORE_CHAR_VALUE_T_DOORBELLPROGSWEVENT_RESTORE         RESTORE_CHAR(DoorbellProgSwEvent,hDoorbellProgSwEvent)

#define DOORBELL_STORE_CHAR_VALUE_T_DOORBELLVOLUME                  u32 DoorbellVolume:7; /* 0~100 */
#define DOORBELL_STORE_CHAR_VALUE_T_DOORBELLVOLUME_STORE                STORE_CHAR(DoorbellVolume,hDoorbellVolume)
#define DOORBELL_STORE_CHAR_VALUE_T_DOORBELLVOLUME_RESTORE              RESTORE_CHAR(DoorbellVolume,hDoorbellVolume)

#define DOORBELL_STORE_CHAR_VALUE_T_DOORBELLBRIGHTNESS              u32 DoorbellBrightness:7; /* 0~100 */
#define DOORBELL_STORE_CHAR_VALUE_T_DOORBELLBRIGHTNESS_STORE            STORE_CHAR(DoorbellBrightness,hDoorbellBrightness)
#define DOORBELL_STORE_CHAR_VALUE_T_DOORBELLBRIGHTNESS_RESTORE          RESTORE_CHAR(DoorbellBrightness,hDoorbellBrightness)

#define DOORBELL_STORE_CHAR_VALUE_T            \
                    DOORBELL_STORE_CHAR_VALUE_T_DOORBELLPROGSWEVENT \
                    DOORBELL_STORE_CHAR_VALUE_T_DOORBELLVOLUME   \
                    DOORBELL_STORE_CHAR_VALUE_T_DOORBELLBRIGHTNESS

#define DOORBELL_STORE_CHAR_TO_FLASH           \
                    DOORBELL_STORE_CHAR_VALUE_T_DOORBELLPROGSWEVENT_STORE   \
                    DOORBELL_STORE_CHAR_VALUE_T_DOORBELLVOLUME_STORE \
                    DOORBELL_STORE_CHAR_VALUE_T_DOORBELLBRIGHTNESS_STORE

#define DOORBELL_RESTORE_CHAR_FROM_FLASH       \
                    DOORBELL_STORE_CHAR_VALUE_T_DOORBELLPROGSWEVENT_RESTORE \
                    DOORBELL_STORE_CHAR_VALUE_T_DOORBELLVOLUME_RESTORE   \
                    DOORBELL_STORE_CHAR_VALUE_T_DOORBELLBRIGHTNESS_RESTORE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN unsigned char hDoorbellProgSwEvent;
EXTERN unsigned char hDoorbellVolume;
EXTERN unsigned char hDoorbellBrightness;

EXTERN unsigned short ccc_hDoorbellProgSwEvent;
EXTERN unsigned short ccc_hDoorbellVolume;
EXTERN unsigned short ccc_hDoorbellBrightness;

#if DOORBELL_PROGSWEVENT_BRC_ENABLE
    #define BC_PARA_DOORBELL_PROGSWEVENT ((u8*)&bc_para_hDoorbellProgSwEvent)
    EXTERN unsigned char bc_para_hDoorbellProgSwEvent;
#else
    #define BC_PARA_DOORBELL_PROGSWEVENT 0
#endif

#if DOORBELL_VOLUME_BRC_ENABLE
    #define BC_PARA_DOORBELL_VOLUME ((u8*)&bc_para_hDoorbellVolume)
    EXTERN unsigned char bc_para_hDoorbellVolume;
#else
    #define BC_PARA_DOORBELL_VOLUME 0
#endif

#if DOORBELL_BRIGHTNESS_BRC_ENABLE
    #define BC_PARA_DOORBELL_BRIGHTNESS ((u8*)&bc_para_hDoorbellBrightness)
    EXTERN unsigned char bc_para_hDoorbellBrightness;
#else
    #define BC_PARA_DOORBELL_BRIGHTNESS 0
#endif

#if DOORBELL_LINKED_SERVICES_ENABLE
	#define DOORBELL_LINKED_SVCS			hDoorbell_linked_svcs
	#define DOORBELL_LINKED_SVCS_LENGTH 	(sizeof(hDoorbell_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * hDoorbell_linked_svcs[];
#else
	#define DOORBELL_LINKED_SVCS			0
	#define DOORBELL_LINKED_SVCS_LENGTH 	0
#endif

EXTERN const u8 hDoorbellServiceName[15];

EXTERN const hap_characteristic_desc_t hap_desc_char_hDoorbellProgSwEvent_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hDoorbellVolume_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hDoorbellBrightness_R_S;


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN int DoorbellProgSwEventReadCallback(void *pp);
EXTERN int DoorbellVolumeWriteCallback(void *pp);
EXTERN int DoorbellVolumeReadCallback(void *pp);
EXTERN int DoorbellBrightnessWriteCallback(void *pp);
EXTERN int DoorbellBrightnessReadCallback(void *pp);
EXTERN int ccc_hDoorbellProgSwEventWriteCB(void *pp);
EXTERN int ccc_hDoorbellVolumeWriteCB(void *pp);
EXTERN int ccc_hDoorbellBrightnessWriteCB(void *pp);

#endif /* _HK_DOORBELL_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
