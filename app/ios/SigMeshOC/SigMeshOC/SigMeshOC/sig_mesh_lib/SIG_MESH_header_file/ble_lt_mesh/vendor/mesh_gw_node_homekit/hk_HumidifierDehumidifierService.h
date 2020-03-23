/********************************************************************************************************
 * @file     hk_HumidifierDehumidifierService.h
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
#ifndef _HK_HUMIDIFIER_DEHUMIDIFIER_SERVICE_H_
#define _HK_HUMIDIFIER_DEHUMIDIFIER_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_HUMIDIFIER_DEHUMIDIFIER_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif

#define CHAR_HUMIDIFIERDEHUMIDIFIER_SVC_SIGN_ENABLE      0
#if CHAR_HUMIDIFIERDEHUMIDIFIER_SVC_SIGN_ENABLE
    #define HUMIDIFIERDEHUMIDIFIER_AS_PRIMARY_SERVICE    	0
    #define HUMIDIFIERDEHUMIDIFIER_IS_HIDDEN_SERVICE     	0
    #define HUMIDIFIERDEHUMIDIFIER_LINKED_SERVICES_ENABLE	0
    EXTERN const hap_characteristic_desc_t hap_desc_char_humidifier_dehumidifier_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_humidifier_dehumidifier_A5_R_S;
    EXTERN unsigned char VAR_HUMIDIFIER_DEHUMIDIFIER_UNKOWN;
#else
    #define HUMIDIFIERDEHUMIDIFIER_AS_PRIMARY_SERVICE    	0
    #define HUMIDIFIERDEHUMIDIFIER_IS_HIDDEN_SERVICE     	0
    #define HUMIDIFIERDEHUMIDIFIER_LINKED_SERVICES_ENABLE	0
#endif


#define CHAR_HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD_ENABLE  0
#define CHAR_HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD_ENABLE    0
#define CHAR_HUMIDIFIERDEHUMIDIFIER_ROTATIONSPEED_ENABLE                          0
#define CHAR_HUMIDIFIERDEHUMIDIFIER_SWINGMODE_ENABLE							  0
#define CHAR_HUMIDIFIERDEHUMIDIFIER_WATERLEVEL_ENABLE                             0
#define CHAR_HUMIDIFIERDEHUMIDIFIER_LOCKPHYSICALCONTROLS_ENABLE					  0

#define HUMIDIFIERDEHUMIDIFIER_ACTIVE_IND_ENABLE                                  0
#define HUMIDIFIERDEHUMIDIFIER_CURRENTRELATIVEHUMIDITY_IND_ENABLE                 0
#define HUMIDIFIERDEHUMIDIFIER_CURRENTHUMIDIFIERDEHUMIDIFIERSTATE_IND_ENABLE      0
#define HUMIDIFIERDEHUMIDIFIER_TARGETHUMIDIFIERDEHUMIDIFIERSTATE_IND_ENABLE       0
#define HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD_IND_ENABLE   0
#define HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD_IND_ENABLE     0
#define HUMIDIFIERDEHUMIDIFIER_ROTATIONSPEED_IND_ENABLE                           0
#define HUMIDIFIERDEHUMIDIFIER_SWINGMODE_IND_ENABLE        						  0
#define HUMIDIFIERDEHUMIDIFIER_WATERLEVEL_IND_ENABLE                              0
#define HUMIDIFIERDEHUMIDIFIER_LOCKPHYSICALCONTROLS_IND_ENABLE        			  0

#define HUMIDIFIERDEHUMIDIFIER_ACTIVE_BRC_ENABLE                                    0
#define HUMIDIFIERDEHUMIDIFIER_CURRENTRELATIVEHUMIDITY_BRC_ENABLE                   0
#define HUMIDIFIERDEHUMIDIFIER_CURRENTHUMIDIFIERDEHUMIDIFIERSTATE_BRC_ENABLE        0
#define HUMIDIFIERDEHUMIDIFIER_TARGETHUMIDIFIERDEHUMIDIFIERSTATE_BRC_ENABLE         0
#define HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD_BRC_ENABLE     0
#define HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD_BRC_ENABLE       0
#define HUMIDIFIERDEHUMIDIFIER_ROTATIONSPEED_BRC_ENABLE                             0
#define HUMIDIFIERDEHUMIDIFIER_SWINGMODE_BRC_ENABLE     							0
#define HUMIDIFIERDEHUMIDIFIER_WATERLEVEL_BRC_ENABLE                                0
#define HUMIDIFIERDEHUMIDIFIER_LOCKPHYSICALCONTROLS_BRC_ENABLE     					0

#define HUMIDIFIERDEHUMIDIFIER_ACTIVE_NO      3
#if HUMIDIFIERDEHUMIDIFIER_ACTIVE_IND_ENABLE
    #define HUMIDIFIERDEHUMIDIFIER_ACTIVE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hHumidifierDehumidifierActive), ccc_HumidifierDehumidifierActiveWriteCB, 0},
    #define HUMIDIFIERDEHUMIDIFIER_ACTIVE_IND_NO  1
#else
    #define HUMIDIFIERDEHUMIDIFIER_ACTIVE_IND_DESC
    #define HUMIDIFIERDEHUMIDIFIER_ACTIVE_IND_NO  0
#endif

#define HUMIDIFIERDEHUMIDIFIER_CURRENTRELATIVEHUMIDITY_NO      3
#if HUMIDIFIERDEHUMIDIFIER_CURRENTRELATIVEHUMIDITY_IND_ENABLE
    #define HUMIDIFIERDEHUMIDIFIER_CURRENTRELATIVEHUMIDITY_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hHumidifierDehumidifierCurrentRelativeHumidity), ccc_HumidifierDehumidifierCurrentRelativeHumidityWriteCB, 0},
    #define HUMIDIFIERDEHUMIDIFIER_CURRENTRELATIVEHUMIDITY_IND_NO  1
#else
    #define HUMIDIFIERDEHUMIDIFIER_CURRENTRELATIVEHUMIDITY_IND_DESC
    #define HUMIDIFIERDEHUMIDIFIER_CURRENTRELATIVEHUMIDITY_IND_NO  0
#endif

#define HUMIDIFIERDEHUMIDIFIER_CURRENTHUMIDIFIERDEHUMIDIFIERSTATE_NO      3
#if HUMIDIFIERDEHUMIDIFIER_CURRENTHUMIDIFIERDEHUMIDIFIERSTATE_IND_ENABLE
    #define HUMIDIFIERDEHUMIDIFIER_CURRENTHUMIDIFIERDEHUMIDIFIERSTATE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hHumidifierDehumidifierCurrentHumidifierDehumidifierState), ccc_HumidifierDehumidifierCurrentHumidifierDehumidifierStateWriteCB, 0},
    #define HUMIDIFIERDEHUMIDIFIER_CURRENTHUMIDIFIERDEHUMIDIFIERSTATE_IND_NO  1
#else
    #define HUMIDIFIERDEHUMIDIFIER_CURRENTHUMIDIFIERDEHUMIDIFIERSTATE_IND_DESC
    #define HUMIDIFIERDEHUMIDIFIER_CURRENTHUMIDIFIERDEHUMIDIFIERSTATE_IND_NO  0
#endif

#define HUMIDIFIERDEHUMIDIFIER_TARGETHUMIDIFIERDEHUMIDIFIERSTATE_NO      3
#if HUMIDIFIERDEHUMIDIFIER_TARGETHUMIDIFIERDEHUMIDIFIERSTATE_IND_ENABLE
    #define HUMIDIFIERDEHUMIDIFIER_TARGETHUMIDIFIERDEHUMIDIFIERSTATE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hHumidifierDehumidifierTargetHumidifierDehumidifierState), ccc_HumidifierDehumidifierTargetHumidifierDehumidifierStateWriteCB, 0},
    #define HUMIDIFIERDEHUMIDIFIER_TARGETHUMIDIFIERDEHUMIDIFIERSTATE_IND_NO  1
#else
    #define HUMIDIFIERDEHUMIDIFIER_TARGETHUMIDIFIERDEHUMIDIFIERSTATE_IND_DESC
    #define HUMIDIFIERDEHUMIDIFIER_TARGETHUMIDIFIERDEHUMIDIFIERSTATE_IND_NO  0
#endif


#if CHAR_HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD_ENABLE
    #define HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD_NO 3
    #define CHAR_HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof (hHumidifierDehumidifierRelativeHumidityDehumidifierThreshold), 0, (u8*)(&charRelativeHumidityDehumidifierThresholdUUID), (u8*)&hHumidifierDehumidifierRelativeHumidityDehumidifierThreshold, HumidifierDehumidifierRelativeHumidityDehumidifierThresholdWriteCallback, HumidifierDehumidifierRelativeHumidityDehumidifierThresholdReadCallback},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + HUMIDIFIERDEHUMIDIFIER_INST_NO + 6), 0, 0}, \
            HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD_IND_DESC

    #if HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD_IND_ENABLE
        #define HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hHumidifierDehumidifierRelativeHumidityDehumidifierThreshold), ccc_HumidifierDehumidifierRelativeHumidityDehumidifierThresholdWriteCB, 0},
        #define HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD_IND_NO  1
    #else
        #define HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD_IND_DESC
        #define HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD_IND_NO  0
    #endif
#else
    #define HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD_NO 0
    #define CHAR_HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD_ATT_TABLE
    #define HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD_IND_NO  0
#endif

#if CHAR_HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD_ENABLE
    #define HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD_NO 3
    #define CHAR_HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof (hHumidifierDehumidifierRelativeHumidityHumidifierThreshold), 0, (u8*)(&charRelativeHumidityHumidifierThresholdUUID), (u8*)&hHumidifierDehumidifierRelativeHumidityHumidifierThreshold, HumidifierDehumidifierRelativeHumidityHumidifierThresholdWriteCallback, HumidifierDehumidifierRelativeHumidityHumidifierThresholdReadCallback},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + HUMIDIFIERDEHUMIDIFIER_INST_NO + 7), 0, 0}, \
            HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD_IND_DESC

    #if HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD_IND_ENABLE
        #define HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hHumidifierDehumidifierRelativeHumidityHumidifierThreshold), ccc_HumidifierDehumidifierRelativeHumidityHumidifierThresholdWriteCB, 0},
        #define HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD_IND_NO  1
    #else
        #define HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD_IND_DESC
        #define HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD_IND_NO  0
    #endif
#else
    #define HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD_NO 0
    #define CHAR_HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD_ATT_TABLE
    #define HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD_IND_NO  0
#endif

#if CHAR_HUMIDIFIERDEHUMIDIFIER_ROTATIONSPEED_ENABLE
    #define HUMIDIFIERDEHUMIDIFIER_ROTATIONSPEED_NO 3
    #define CHAR_HUMIDIFIERDEHUMIDIFIER_ROTATIONSPEED_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_HUMIDIFIERDEHUMIDIFIER_ROTATIONSPEED_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof (hHumidifierDehumidifierRotationSpeed), 0, (u8*)(&charRotationSpeedUUID), (u8*)&hHumidifierDehumidifierRotationSpeed, HumidifierDehumidifierRotationSpeedWriteCallback, HumidifierDehumidifierRotationSpeedReadCallback},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + HUMIDIFIERDEHUMIDIFIER_INST_NO + 8), 0, 0}, \
            HUMIDIFIERDEHUMIDIFIER_ROTATIONSPEED_IND_DESC

    #if HUMIDIFIERDEHUMIDIFIER_ROTATIONSPEED_IND_ENABLE
        #define HUMIDIFIERDEHUMIDIFIER_ROTATIONSPEED_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hHumidifierDehumidifierRotationSpeed), ccc_HumidifierDehumidifierRotationSpeedWriteCB, 0},
        #define HUMIDIFIERDEHUMIDIFIER_ROTATIONSPEED_IND_NO  1
    #else
        #define HUMIDIFIERDEHUMIDIFIER_ROTATIONSPEED_IND_DESC
        #define HUMIDIFIERDEHUMIDIFIER_ROTATIONSPEED_IND_NO  0
    #endif
#else
    #define HUMIDIFIERDEHUMIDIFIER_ROTATIONSPEED_NO 0
    #define CHAR_HUMIDIFIERDEHUMIDIFIER_ROTATIONSPEED_ATT_TABLE
    #define HUMIDIFIERDEHUMIDIFIER_ROTATIONSPEED_IND_NO  0
#endif

#if CHAR_HUMIDIFIERDEHUMIDIFIER_SWINGMODE_ENABLE
    #define HUMIDIFIERDEHUMIDIFIER_SWINGMODE_NO   3
    #define CHAR_HUMIDIFIERDEHUMIDIFIER_SWINGMODE_ATT_TABLE   \
            {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_HUMIDIFIERDEHUMIDIFIER_SWINGMODE_R_S, 0},    \
            {0,ATT_PERMISSIONS_RDWR,16,sizeof(hHumidifierDehumidifierSwingMode),0, (u8*)(charSwingModeUUID), (u8*)&hHumidifierDehumidifierSwingMode, HumidifierDehumidifierSwingModeWriteCallback, HumidifierDehumidifierSwingModeReadCallback}, \
                {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + HUMIDIFIERDEHUMIDIFIER_INST_NO + 9), 0, 0}, \
                HUMIDIFIERDEHUMIDIFIER_SWINGMODE_IND_DESC

    #if HUMIDIFIERDEHUMIDIFIER_SWINGMODE_IND_ENABLE
        #define HUMIDIFIERDEHUMIDIFIER_SWINGMODE_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hHumidifierDehumidifierSwingMode), ccc_HumidifierDehumidifierSwingModeWriteCB, 0},
        #define HUMIDIFIERDEHUMIDIFIER_SWINGMODE_IND_NO  1
    #else
        #define HUMIDIFIERDEHUMIDIFIER_SWINGMODE_IND_DESC
        #define HUMIDIFIERDEHUMIDIFIER_SWINGMODE_IND_NO  0
    #endif
#else
    #define HUMIDIFIERDEHUMIDIFIER_SWINGMODE_NO   0
    #define CHAR_HUMIDIFIERDEHUMIDIFIER_SWINGMODE_ATT_TABLE
    #define HUMIDIFIERDEHUMIDIFIER_SWINGMODE_IND_NO  0
#endif


#if CHAR_HUMIDIFIERDEHUMIDIFIER_WATERLEVEL_ENABLE
    #define HUMIDIFIERDEHUMIDIFIER_WATERLEVEL_NO 3
    #define CHAR_HUMIDIFIERDEHUMIDIFIER_WATERLEVEL_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_HUMIDIFIERDEHUMIDIFIER_WATERLEVEL_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof (hHumidifierDehumidifierWaterLevel), 0, (u8*)(&charWaterLevelUUID), (u8*)&hHumidifierDehumidifierWaterLevel, 0, HumidifierDehumidifierWaterLevelReadCallback},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + HUMIDIFIERDEHUMIDIFIER_INST_NO + 10), 0, 0}, \
            HUMIDIFIERDEHUMIDIFIER_WATERLEVEL_IND_DESC

    #if HUMIDIFIERDEHUMIDIFIER_WATERLEVEL_IND_ENABLE
        #define HUMIDIFIERDEHUMIDIFIER_WATERLEVEL_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hHumidifierDehumidifierWaterLevel), ccc_HumidifierDehumidifierWaterLevelWriteCB, 0},
        #define HUMIDIFIERDEHUMIDIFIER_WATERLEVEL_IND_NO  1
    #else
        #define HUMIDIFIERDEHUMIDIFIER_WATERLEVEL_IND_DESC
        #define HUMIDIFIERDEHUMIDIFIER_WATERLEVEL_IND_NO  0
    #endif
#else
    #define HUMIDIFIERDEHUMIDIFIER_WATERLEVEL_NO 0
    #define CHAR_HUMIDIFIERDEHUMIDIFIER_WATERLEVEL_ATT_TABLE
    #define HUMIDIFIERDEHUMIDIFIER_WATERLEVEL_IND_NO  0
#endif

#if CHAR_HUMIDIFIERDEHUMIDIFIER_LOCKPHYSICALCONTROLS_ENABLE
    #define HUMIDIFIERDEHUMIDIFIER_LOCKPHYSICALCONTROLS_NO   3
    #define CHAR_HUMIDIFIERDEHUMIDIFIER_LOCKPHYSICALCONTROLS_ATT_TABLE   \
            {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_HUMIDIFIERDEHUMIDIFIER_LOCKPHYSICALCONTROLS_R_S, 0},    \
            {0,ATT_PERMISSIONS_RDWR,16,sizeof(hHumidifierDehumidifierLockPhysicalControls),0, (u8*)(charLockPhysicalControlsUUID), (u8*)&hHumidifierDehumidifierLockPhysicalControls, HumidifierDehumidifierLockPhysicalControlsWriteCallback, HumidifierDehumidifierLockPhysicalControlsReadCallback}, \
                {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + HUMIDIFIERDEHUMIDIFIER_INST_NO + 11), 0, 0}, \
                HUMIDIFIERDEHUMIDIFIER_LOCKPHYSICALCONTROLS_IND_DESC

    #if HUMIDIFIERDEHUMIDIFIER_LOCKPHYSICALCONTROLS_IND_ENABLE
        #define HUMIDIFIERDEHUMIDIFIER_LOCKPHYSICALCONTROLS_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hHumidifierDehumidifierLockPhysicalControls), ccc_HumidifierDehumidifierLockPhysicalControlsWriteCB, 0},
        #define HUMIDIFIERDEHUMIDIFIER_LOCKPHYSICALCONTROLS_IND_NO  1
    #else
        #define HUMIDIFIERDEHUMIDIFIER_LOCKPHYSICALCONTROLS_IND_DESC
        #define HUMIDIFIERDEHUMIDIFIER_LOCKPHYSICALCONTROLS_IND_NO  0
    #endif
#else
    #define HUMIDIFIERDEHUMIDIFIER_LOCKPHYSICALCONTROLS_NO   0
    #define CHAR_HUMIDIFIERDEHUMIDIFIER_LOCKPHYSICALCONTROLS_ATT_TABLE
    #define HUMIDIFIERDEHUMIDIFIER_LOCKPHYSICALCONTROLS_IND_NO  0
#endif


#if CHAR_HUMIDIFIERDEHUMIDIFIER_SVC_SIGN_ENABLE
    #define HUMIDIFIERDEHUMIDIFIER_SVC_SIGN_NO   3
    #define CHAR_HUMIDIFIERDEHUMIDIFIER_SVC_A5_SIGN_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_humidifier_dehumidifier_A5_R_S, &hap_desc_service_humidifier_dehumidifier_A5_R_S},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_HUMIDIFIER_DEHUMIDIFIER_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_HUMIDIFIER_DEHUMIDIFIER_UNKOWN, 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + HUMIDIFIERDEHUMIDIFIER_INST_NO + 12), 0, 0},
#else
    #define HUMIDIFIERDEHUMIDIFIER_SVC_SIGN_NO   0
    #define CHAR_HUMIDIFIERDEHUMIDIFIER_SVC_A5_SIGN_TABLE
#endif

#define HUMIDIFIERDEHUMIDIFIER_NO_DELTA   HUMIDIFIERDEHUMIDIFIER_ACTIVE_IND_NO \
                                + HUMIDIFIERDEHUMIDIFIER_CURRENTRELATIVEHUMIDITY_IND_NO \
                                + HUMIDIFIERDEHUMIDIFIER_CURRENTHUMIDIFIERDEHUMIDIFIERSTATE_IND_NO \
                                + HUMIDIFIERDEHUMIDIFIER_TARGETHUMIDIFIERDEHUMIDIFIERSTATE_IND_NO \
                                + HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD_IND_NO \
                                + HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD_IND_NO \
                                + HUMIDIFIERDEHUMIDIFIER_ROTATIONSPEED_IND_NO \
                                + HUMIDIFIERDEHUMIDIFIER_SWINGMODE_IND_NO \
                                + HUMIDIFIERDEHUMIDIFIER_WATERLEVEL_IND_NO \
                                + HUMIDIFIERDEHUMIDIFIER_LOCKPHYSICALCONTROLS_IND_NO

#define HUMIDIFIERDEHUMIDIFIER_NO   18 + HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD_NO \
                                + HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD_NO \
                                + HUMIDIFIERDEHUMIDIFIER_ROTATIONSPEED_NO \
                                + HUMIDIFIERDEHUMIDIFIER_SWINGMODE_NO \
                                + HUMIDIFIERDEHUMIDIFIER_WATERLEVEL_NO \
                                + HUMIDIFIERDEHUMIDIFIER_LOCKPHYSICALCONTROLS_NO \
                                + HUMIDIFIERDEHUMIDIFIER_SVC_SIGN_NO

#define HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM (HUMIDIFIERDEHUMIDIFIER_NO + HUMIDIFIERDEHUMIDIFIER_NO_DELTA)

#define HOMEKIT_HUMIDIFIERDEHUMIDIFIER_SERVICE_ATT_TABLE   \
    {HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(humidifier_dehumidifierServiceUUID),sizeof(humidifier_dehumidifierServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(humidifier_dehumidifierServiceUUID), 0, 0},   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(insts + HUMIDIFIERDEHUMIDIFIER_INST_NO), 0, 0},   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_NAME, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(hHumidifierDehumidifierServiceName), 0, (u8*)(nameUUID), (u8*)(hHumidifierDehumidifierServiceName), 0, 0},  \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + HUMIDIFIERDEHUMIDIFIER_INST_NO + 1), 0, 0}, \
		{0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_HUMIDIFIERDEHUMIDIFIER_ACTIVE_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hHumidifierDehumidifierActive),0, (u8*)(charActiveUUID), (u8*)&hHumidifierDehumidifierActive, HumidifierDehumidifierActiveWriteCallback, HumidifierDehumidifierActiveReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + HUMIDIFIERDEHUMIDIFIER_INST_NO + 2), 0, 0},  \
            HUMIDIFIERDEHUMIDIFIER_ACTIVE_IND_DESC \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_HUMIDIFIERDEHUMIDIFIER_CURRENTRELATIVEHUMIDITY_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hHumidifierDehumidifierCurrentRelativeHumidity),0, (u8*)(charCurrentRelativeHumidityUUID), (u8*)&hHumidifierDehumidifierCurrentRelativeHumidity, 0, HumidifierDehumidifierCurrentRelativeHumidityReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + HUMIDIFIERDEHUMIDIFIER_INST_NO + 3), 0, 0},  \
            HUMIDIFIERDEHUMIDIFIER_CURRENTRELATIVEHUMIDITY_IND_DESC \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_HUMIDIFIERDEHUMIDIFIER_CURRENTHUMIDIFIERDEHUMIDIFIERSTATE_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hHumidifierDehumidifierCurrentHumidifierDehumidifierState),0, (u8*)(charCurrentHumidifierDehumidifierStateUUID), (u8*)&hHumidifierDehumidifierCurrentHumidifierDehumidifierState, 0, HumidifierDehumidifierCurrentHumidifierDehumidifierStateReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + HUMIDIFIERDEHUMIDIFIER_INST_NO + 4), 0, 0},  \
            HUMIDIFIERDEHUMIDIFIER_CURRENTHUMIDIFIERDEHUMIDIFIERSTATE_IND_DESC \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_HUMIDIFIERDEHUMIDIFIER_TARGETHUMIDIFIERDEHUMIDIFIERSTATE_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hHumidifierDehumidifierTargetHumidifierDehumidifierState),0, (u8*)(charTargetHumidifierDehumidifierStateUUID), (u8*)&hHumidifierDehumidifierTargetHumidifierDehumidifierState, HumidifierDehumidifierTargetHumidifierDehumidifierStateWriteCallback, HumidifierDehumidifierTargetHumidifierDehumidifierStateReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + HUMIDIFIERDEHUMIDIFIER_INST_NO + 5), 0, 0},  \
            HUMIDIFIERDEHUMIDIFIER_TARGETHUMIDIFIERDEHUMIDIFIERSTATE_IND_DESC \
        CHAR_HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD_ATT_TABLE \
        CHAR_HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD_ATT_TABLE \
        CHAR_HUMIDIFIERDEHUMIDIFIER_ROTATIONSPEED_ATT_TABLE \
        CHAR_HUMIDIFIERDEHUMIDIFIER_SWINGMODE_ATT_TABLE \
        CHAR_HUMIDIFIERDEHUMIDIFIER_WATERLEVEL_ATT_TABLE \
        CHAR_HUMIDIFIERDEHUMIDIFIER_LOCKPHYSICALCONTROLS_ATT_TABLE \
        CHAR_HUMIDIFIERDEHUMIDIFIER_SVC_A5_SIGN_TABLE

#define HUMIDIFIERDEHUMIDIFIER_HANDLE_COMMON_NUM     8

#define HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_ACTIVE                                  HUMIDIFIERDEHUMIDIFIER_HANDLE_START_NUM +HUMIDIFIERDEHUMIDIFIER_HANDLE_COMMON_NUM
#define HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_CURRENTRELATIVEHUMIDITY                 HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_ACTIVE +HUMIDIFIERDEHUMIDIFIER_ACTIVE_NO +HUMIDIFIERDEHUMIDIFIER_ACTIVE_IND_NO
#define HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_CURRENTHUMIDIFIERDEHUMIDIFIERSTATE      HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_CURRENTRELATIVEHUMIDITY +HUMIDIFIERDEHUMIDIFIER_CURRENTRELATIVEHUMIDITY_NO +HUMIDIFIERDEHUMIDIFIER_CURRENTRELATIVEHUMIDITY_IND_NO
#define HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_TARGETHUMIDIFIERDEHUMIDIFIERSTATE       HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_CURRENTHUMIDIFIERDEHUMIDIFIERSTATE +HUMIDIFIERDEHUMIDIFIER_CURRENTHUMIDIFIERDEHUMIDIFIERSTATE_NO +HUMIDIFIERDEHUMIDIFIER_CURRENTHUMIDIFIERDEHUMIDIFIERSTATE_IND_NO
#define HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD   HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_TARGETHUMIDIFIERDEHUMIDIFIERSTATE +HUMIDIFIERDEHUMIDIFIER_TARGETHUMIDIFIERDEHUMIDIFIERSTATE_NO +HUMIDIFIERDEHUMIDIFIER_TARGETHUMIDIFIERDEHUMIDIFIERSTATE_IND_NO
#define HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD     HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD +HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD_NO +HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD_IND_NO
#define HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_ROTATIONSPEED                           HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD +HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD_NO +HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD_IND_NO
#define HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_SWINGMODE                           	  HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_ROTATIONSPEED +HUMIDIFIERDEHUMIDIFIER_ROTATIONSPEED_NO +HUMIDIFIERDEHUMIDIFIER_ROTATIONSPEED_IND_NO
#define HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_WATERLEVEL                              HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_SWINGMODE +HUMIDIFIERDEHUMIDIFIER_SWINGMODE_NO +HUMIDIFIERDEHUMIDIFIER_SWINGMODE_IND_NO
#define HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_LOCKPHYSICALCONTROLS                    HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_WATERLEVEL +HUMIDIFIERDEHUMIDIFIER_WATERLEVEL_NO +HUMIDIFIERDEHUMIDIFIER_WATERLEVEL_IND_NO
#define HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_THE_END


#define HD_STORE_CHAR_VALUE_T_HD_ACTIVE                                        u32 HumidifierDehumidifierActive:1;     /* 0~1 */
#define HD_STORE_CHAR_VALUE_T_HD_ACTIVE_STORE                                      STORE_CHAR(HumidifierDehumidifierActive, hHumidifierDehumidifierActive)
#define HD_STORE_CHAR_VALUE_T_HD_ACTIVE_RESTORE                                    RESTORE_CHAR(HumidifierDehumidifierActive, hHumidifierDehumidifierActive)

#define HD_STORE_CHAR_VALUE_T_HD_CURRENTRELATIVEHUMIDITY                       u32 HumidifierDehumidifierCurrentRelativeHumidity;    /* float */
#define HD_STORE_CHAR_VALUE_T_HD_CURRENTRELATIVEHUMIDITY_STORE                     STORE_CHAR(HumidifierDehumidifierCurrentRelativeHumidity, hHumidifierDehumidifierCurrentRelativeHumidity)
#define HD_STORE_CHAR_VALUE_T_HD_CURRENTRELATIVEHUMIDITY_RESTORE                   RESTORE_CHAR(HumidifierDehumidifierCurrentRelativeHumidity, hHumidifierDehumidifierCurrentRelativeHumidity)

#define HD_STORE_CHAR_VALUE_T_HD_CURRENTHUMIDIFIERDEHUMIDIFIERSTATE            u32 HumidifierDehumidifierCurrentHumidifierDehumidifierState:2;   /* 0~3 */
#define HD_STORE_CHAR_VALUE_T_HD_CURRENTHUMIDIFIERDEHUMIDIFIERSTATE_STORE          STORE_CHAR(HumidifierDehumidifierCurrentHumidifierDehumidifierState, hHumidifierDehumidifierCurrentHumidifierDehumidifierState)
#define HD_STORE_CHAR_VALUE_T_HD_CURRENTHUMIDIFIERDEHUMIDIFIERSTATE_RESTORE        RESTORE_CHAR(HumidifierDehumidifierCurrentHumidifierDehumidifierState, hHumidifierDehumidifierCurrentHumidifierDehumidifierState)

#define HD_STORE_CHAR_VALUE_T_HD_TARGETHUMIDIFIERDEHUMIDIFIERSTATE             u32 HumidifierDehumidifierTargetHumidifierDehumidifierState:2;   /* 0~2 */
#define HD_STORE_CHAR_VALUE_T_HD_TARGETHUMIDIFIERDEHUMIDIFIERSTATE_STORE           STORE_CHAR(HumidifierDehumidifierTargetHumidifierDehumidifierState, hHumidifierDehumidifierTargetHumidifierDehumidifierState)
#define HD_STORE_CHAR_VALUE_T_HD_TARGETHUMIDIFIERDEHUMIDIFIERSTATE_RESTORE         RESTORE_CHAR(HumidifierDehumidifierTargetHumidifierDehumidifierState, hHumidifierDehumidifierTargetHumidifierDehumidifierState)

#if CHAR_HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD_ENABLE
#define HD_STORE_CHAR_VALUE_T_HD_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD         u32 HumidifierDehumidifierRelativeHumidityDehumidifierThreshold;   /* float */
#define HD_STORE_CHAR_VALUE_T_HD_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD_STORE       STORE_CHAR(HumidifierDehumidifierRelativeHumidityDehumidifierThreshold, hHumidifierDehumidifierRelativeHumidityDehumidifierThreshold)
#define HD_STORE_CHAR_VALUE_T_HD_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD_RESTORE     RESTORE_CHAR(HumidifierDehumidifierRelativeHumidityDehumidifierThreshold, hHumidifierDehumidifierRelativeHumidityDehumidifierThreshold)
#else
#define HD_STORE_CHAR_VALUE_T_HD_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD
#define HD_STORE_CHAR_VALUE_T_HD_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD_STORE
#define HD_STORE_CHAR_VALUE_T_HD_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD_RESTORE
#endif

#if CHAR_HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD_ENABLE
#define HD_STORE_CHAR_VALUE_T_HD_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD            u32 HumidifierDehumidifierRelativeHumidityHumidifierThreshold;   /* float */
#define HD_STORE_CHAR_VALUE_T_HD_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD_STORE          STORE_CHAR(HumidifierDehumidifierRelativeHumidityHumidifierThreshold, hHumidifierDehumidifierRelativeHumidityHumidifierThreshold)
#define HD_STORE_CHAR_VALUE_T_HD_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD_RESTORE        RESTORE_CHAR(HumidifierDehumidifierRelativeHumidityHumidifierThreshold, hHumidifierDehumidifierRelativeHumidityHumidifierThreshold)
#else
#define HD_STORE_CHAR_VALUE_T_HD_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD
#define HD_STORE_CHAR_VALUE_T_HD_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD_STORE
#define HD_STORE_CHAR_VALUE_T_HD_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD_RESTORE
#endif

#if CHAR_HUMIDIFIERDEHUMIDIFIER_ROTATIONSPEED_ENABLE
#define HD_STORE_CHAR_VALUE_T_HD_ROTATIONSPEED									u32 HumidifierDehumidifierRotationSpeed;   /* float */
#define HD_STORE_CHAR_VALUE_T_HD_ROTATIONSPEED_STORE								STORE_CHAR(HumidifierDehumidifierRotationSpeed, hHumidifierDehumidifierRotationSpeed)
#define HD_STORE_CHAR_VALUE_T_HD_ROTATIONSPEED_RESTORE								RESTORE_CHAR(HumidifierDehumidifierRotationSpeed, hHumidifierDehumidifierRotationSpeed)
#else
#define HD_STORE_CHAR_VALUE_T_HD_ROTATIONSPEED
#define HD_STORE_CHAR_VALUE_T_HD_ROTATIONSPEED_STORE
#define HD_STORE_CHAR_VALUE_T_HD_ROTATIONSPEED_RESTORE
#endif

#if CHAR_HUMIDIFIERDEHUMIDIFIER_SWINGMODE_ENABLE
#define HD_STORE_CHAR_VALUE_T_HD_SWINGMODE                 		u32 HumidifierDehumidifierSwingMode:1;    /* 0~1 */
#define HD_STORE_CHAR_VALUE_T_HD_SWINGMODE_STORE               		STORE_CHAR(HumidifierDehumidifierSwingMode,hHumidifierDehumidifierSwingMode)
#define HD_STORE_CHAR_VALUE_T_HD_SWINGMODE_RESTORE             		RESTORE_CHAR(HumidifierDehumidifierSwingMode,hHumidifierDehumidifierSwingMode)
#else
#define HD_STORE_CHAR_VALUE_T_HD_SWINGMODE
#define HD_STORE_CHAR_VALUE_T_HD_SWINGMODE_STORE
#define HD_STORE_CHAR_VALUE_T_HD_SWINGMODE_RESTORE
#endif


#if CHAR_HUMIDIFIERDEHUMIDIFIER_WATERLEVEL_ENABLE
#define HD_STORE_CHAR_VALUE_T_HD_WATERLEVEL                                      u32 HumidifierDehumidifierWaterLevel;   /* float */
#define HD_STORE_CHAR_VALUE_T_HD_WATERLEVEL_STORE                                    STORE_CHAR(HumidifierDehumidifierWaterLevel, hHumidifierDehumidifierWaterLevel)
#define HD_STORE_CHAR_VALUE_T_HD_WATERLEVEL_RESTORE                                  RESTORE_CHAR(HumidifierDehumidifierWaterLevel, hHumidifierDehumidifierWaterLevel)
#else
#define HD_STORE_CHAR_VALUE_T_HD_WATERLEVEL
#define HD_STORE_CHAR_VALUE_T_HD_WATERLEVEL_STORE
#define HD_STORE_CHAR_VALUE_T_HD_WATERLEVEL_RESTORE
#endif

#if CHAR_HUMIDIFIERDEHUMIDIFIER_LOCKPHYSICALCONTROLS_ENABLE
#define HD_STORE_CHAR_VALUE_T_HD_LOCKPHYSICALCONTROLS      		u32 HumidifierDehumidifierLockPhysicalControls:1;    /* 0~1 */
#define HD_STORE_CHAR_VALUE_T_HD_LOCKPHYSICALCONTROLS_STORE  		STORE_CHAR(HumidifierDehumidifierLockPhysicalControls,hHumidifierDehumidifierLockPhysicalControls)
#define HD_STORE_CHAR_VALUE_T_HD_LOCKPHYSICALCONTROLS_RESTORE  		RESTORE_CHAR(HumidifierDehumidifierLockPhysicalControls,hHumidifierDehumidifierLockPhysicalControls)
#else
#define HD_STORE_CHAR_VALUE_T_HD_LOCKPHYSICALCONTROLS
#define HD_STORE_CHAR_VALUE_T_HD_LOCKPHYSICALCONTROLS_STORE
#define HD_STORE_CHAR_VALUE_T_HD_LOCKPHYSICALCONTROLS_RESTORE
#endif


#define HUMIDIFIERDEHUMIDIFIER_STORE_CHAR_VALUE_T            \
        HD_STORE_CHAR_VALUE_T_HD_ACTIVE \
        HD_STORE_CHAR_VALUE_T_HD_CURRENTRELATIVEHUMIDITY \
        HD_STORE_CHAR_VALUE_T_HD_CURRENTHUMIDIFIERDEHUMIDIFIERSTATE \
        HD_STORE_CHAR_VALUE_T_HD_TARGETHUMIDIFIERDEHUMIDIFIERSTATE \
        HD_STORE_CHAR_VALUE_T_HD_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD \
        HD_STORE_CHAR_VALUE_T_HD_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD \
        HD_STORE_CHAR_VALUE_T_HD_ROTATIONSPEED \
        HD_STORE_CHAR_VALUE_T_HD_SWINGMODE \
        HD_STORE_CHAR_VALUE_T_HD_WATERLEVEL \
        HD_STORE_CHAR_VALUE_T_HD_LOCKPHYSICALCONTROLS

#define HUMIDIFIERDEHUMIDIFIER_STORE_CHAR_TO_FLASH           \
        HD_STORE_CHAR_VALUE_T_HD_ACTIVE_STORE \
        HD_STORE_CHAR_VALUE_T_HD_CURRENTRELATIVEHUMIDITY_STORE \
        HD_STORE_CHAR_VALUE_T_HD_CURRENTHUMIDIFIERDEHUMIDIFIERSTATE_STORE \
        HD_STORE_CHAR_VALUE_T_HD_TARGETHUMIDIFIERDEHUMIDIFIERSTATE_STORE \
        HD_STORE_CHAR_VALUE_T_HD_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD_STORE \
        HD_STORE_CHAR_VALUE_T_HD_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD_STORE \
        HD_STORE_CHAR_VALUE_T_HD_ROTATIONSPEED_STORE \
        HD_STORE_CHAR_VALUE_T_HD_SWINGMODE_STORE \
        HD_STORE_CHAR_VALUE_T_HD_WATERLEVEL_STORE \
        HD_STORE_CHAR_VALUE_T_HD_LOCKPHYSICALCONTROLS_STORE

#define HUMIDIFIERDEHUMIDIFIER_RESTORE_CHAR_FROM_FLASH       \
        HD_STORE_CHAR_VALUE_T_HD_ACTIVE_RESTORE \
        HD_STORE_CHAR_VALUE_T_HD_CURRENTRELATIVEHUMIDITY_RESTORE \
        HD_STORE_CHAR_VALUE_T_HD_CURRENTHUMIDIFIERDEHUMIDIFIERSTATE_RESTORE \
        HD_STORE_CHAR_VALUE_T_HD_TARGETHUMIDIFIERDEHUMIDIFIERSTATE_RESTORE \
        HD_STORE_CHAR_VALUE_T_HD_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD_RESTORE \
        HD_STORE_CHAR_VALUE_T_HD_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD_RESTORE \
        HD_STORE_CHAR_VALUE_T_HD_ROTATIONSPEED_RESTORE \
        HD_STORE_CHAR_VALUE_T_HD_SWINGMODE_RESTORE \
        HD_STORE_CHAR_VALUE_T_HD_WATERLEVEL_RESTORE \
        HD_STORE_CHAR_VALUE_T_HD_LOCKPHYSICALCONTROLS_RESTORE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */

EXTERN unsigned char hHumidifierDehumidifierActive;
EXTERN unsigned int hHumidifierDehumidifierCurrentRelativeHumidity;
EXTERN unsigned char hHumidifierDehumidifierCurrentHumidifierDehumidifierState;
EXTERN unsigned char hHumidifierDehumidifierTargetHumidifierDehumidifierState;
EXTERN unsigned int hHumidifierDehumidifierRelativeHumidityDehumidifierThreshold;
EXTERN unsigned int hHumidifierDehumidifierRelativeHumidityHumidifierThreshold;
EXTERN unsigned int hHumidifierDehumidifierRotationSpeed;
EXTERN unsigned char hHumidifierDehumidifierSwingMode;
EXTERN unsigned int hHumidifierDehumidifierWaterLevel;
EXTERN unsigned char hHumidifierDehumidifierLockPhysicalControls;

EXTERN unsigned short ccc_hHumidifierDehumidifierActive;
EXTERN unsigned short ccc_hHumidifierDehumidifierCurrentRelativeHumidity;
EXTERN unsigned short ccc_hHumidifierDehumidifierCurrentHumidifierDehumidifierState;
EXTERN unsigned short ccc_hHumidifierDehumidifierTargetHumidifierDehumidifierState;
EXTERN unsigned short ccc_hHumidifierDehumidifierRelativeHumidityDehumidifierThreshold;
EXTERN unsigned short ccc_hHumidifierDehumidifierRelativeHumidityHumidifierThreshold;
EXTERN unsigned short ccc_hHumidifierDehumidifierRotationSpeed;
EXTERN unsigned short ccc_hHumidifierDehumidifierSwingMode;
EXTERN unsigned short ccc_hHumidifierDehumidifierWaterLevel;
EXTERN unsigned short ccc_hHumidifierDehumidifierLockPhysicalControls;

#if HUMIDIFIERDEHUMIDIFIER_ACTIVE_BRC_ENABLE
    #define BC_PARA_HUMIDIFIERDEHUMIDIFIER_ACTIVE ((u8*)&bc_para_hHumidifierDehumidifierActive)
    EXTERN unsigned char bc_para_hHumidifierDehumidifierActive;
#else
    #define BC_PARA_HUMIDIFIERDEHUMIDIFIER_ACTIVE 0
#endif

#if HUMIDIFIERDEHUMIDIFIER_CURRENTRELATIVEHUMIDITY_BRC_ENABLE
    #define BC_PARA_HUMIDIFIERDEHUMIDIFIER_CURRENTRELATIVEHUMIDITY ((u8*)&bc_para_hHumidifierDehumidifierCurrentRelativeHumidity)
    EXTERN unsigned char bc_para_hHumidifierDehumidifierCurrentRelativeHumidity;
#else
    #define BC_PARA_HUMIDIFIERDEHUMIDIFIER_CURRENTRELATIVEHUMIDITY 0
#endif

#if HUMIDIFIERDEHUMIDIFIER_CURRENTHUMIDIFIERDEHUMIDIFIERSTATE_BRC_ENABLE
    #define BC_PARA_HUMIDIFIERDEHUMIDIFIER_CURRENTHUMIDIFIERDEHUMIDIFIERSTATE ((u8*)&bc_para_hHumidifierDehumidifierCurrentHumidifierDehumidifierState)
    EXTERN unsigned char bc_para_hHumidifierDehumidifierCurrentHumidifierDehumidifierState;
#else
    #define BC_PARA_HUMIDIFIERDEHUMIDIFIER_CURRENTHUMIDIFIERDEHUMIDIFIERSTATE 0
#endif

#if HUMIDIFIERDEHUMIDIFIER_TARGETHUMIDIFIERDEHUMIDIFIERSTATE_BRC_ENABLE
    #define BC_PARA_HUMIDIFIERDEHUMIDIFIER_TARGETHUMIDIFIERDEHUMIDIFIERSTATE ((u8*)&bc_para_hHumidifierDehumidifierTargetHumidifierDehumidifierState)
    EXTERN unsigned char bc_para_hHumidifierDehumidifierTargetHumidifierDehumidifierState;
#else
    #define BC_PARA_HUMIDIFIERDEHUMIDIFIER_TARGETHUMIDIFIERDEHUMIDIFIERSTATE 0
#endif

#if HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD_BRC_ENABLE
    #define BC_PARA_HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD ((u8*)&bc_para_hHumidifierDehumidifierRelativeHumidityDehumidifierThreshold)
    EXTERN unsigned char bc_para_hHumidifierDehumidifierRelativeHumidityDehumidifierThreshold;
#else
    #define BC_PARA_HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD 0
#endif

#if HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD_BRC_ENABLE
    #define BC_PARA_HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD ((u8*)&bc_para_hHumidifierDehumidifierRelativeHumidityHumidifierThreshold)
    EXTERN unsigned char bc_para_hHumidifierDehumidifierRelativeHumidityHumidifierThreshold;
#else
    #define BC_PARA_HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD 0
#endif

#if HUMIDIFIERDEHUMIDIFIER_ROTATIONSPEED_BRC_ENABLE
    #define BC_PARA_HUMIDIFIERDEHUMIDIFIER_ROTATIONSPEED ((u8*)&bc_para_hHumidifierDehumidifierRotationSpeed)
    EXTERN unsigned char bc_para_hHumidifierDehumidifierRotationSpeed;
#else
    #define BC_PARA_HUMIDIFIERDEHUMIDIFIER_ROTATIONSPEED 0
#endif

#if HUMIDIFIERDEHUMIDIFIER_SWINGMODE_BRC_ENABLE
    #define BC_PARA_HUMIDIFIERDEHUMIDIFIER_SWINGMODE ((u8*)&bc_para_hHumidifierDehumidifierSwingMode)
    EXTERN unsigned char bc_para_hHumidifierDehumidifierSwingMode;
#else
    #define BC_PARA_HUMIDIFIERDEHUMIDIFIER_SWINGMODE 0
#endif

#if HUMIDIFIERDEHUMIDIFIER_WATERLEVEL_BRC_ENABLE
    #define BC_PARA_HUMIDIFIERDEHUMIDIFIER_WATERLEVEL ((u8*)&bc_para_hHumidifierDehumidifierWaterLevel)
    EXTERN unsigned char bc_para_hHumidifierDehumidifierWaterLevel;
#else
    #define BC_PARA_HUMIDIFIERDEHUMIDIFIER_WATERLEVEL 0
#endif

#if HUMIDIFIERDEHUMIDIFIER_LOCKPHYSICALCONTROLS_BRC_ENABLE
    #define BC_PARA_HUMIDIFIERDEHUMIDIFIER_LOCKPHYSICALCONTROLS ((u8*)&bc_para_hHumidifierDehumidifierLockPhysicalControls)
    EXTERN unsigned char bc_para_hHumidifierDehumidifierLockPhysicalControls;
#else
    #define BC_PARA_HUMIDIFIERDEHUMIDIFIER_LOCKPHYSICALCONTROLS 0
#endif

#if HUMIDIFIERDEHUMIDIFIER_LINKED_SERVICES_ENABLE
	#define HUMIDIFIERDEHUMIDIFIER_LINKED_SVCS			hHumidifierDehumidifier_linked_svcs
	#define HUMIDIFIERDEHUMIDIFIER_LINKED_SVCS_LENGTH 	(sizeof(hHumidifierDehumidifier_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * hHumidifierDehumidifier_linked_svcs[];
#else
	#define HUMIDIFIERDEHUMIDIFIER_LINKED_SVCS			0
	#define HUMIDIFIERDEHUMIDIFIER_LINKED_SVCS_LENGTH 	0
#endif

EXTERN const u8 hHumidifierDehumidifierServiceName[29];

EXTERN const hap_characteristic_desc_t hap_desc_char_HUMIDIFIERDEHUMIDIFIER_ACTIVE_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_HUMIDIFIERDEHUMIDIFIER_CURRENTRELATIVEHUMIDITY_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_HUMIDIFIERDEHUMIDIFIER_CURRENTHUMIDIFIERDEHUMIDIFIERSTATE_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_HUMIDIFIERDEHUMIDIFIER_TARGETHUMIDIFIERDEHUMIDIFIERSTATE_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_HUMIDIFIERDEHUMIDIFIER_ROTATIONSPEED_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_HUMIDIFIERDEHUMIDIFIER_SWINGMODE_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_HUMIDIFIERDEHUMIDIFIER_WATERLEVEL_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_HUMIDIFIERDEHUMIDIFIER_LOCKPHYSICALCONTROLS_R_S;

/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN int HumidifierDehumidifierActiveReadCallback(void *pp);
EXTERN int HumidifierDehumidifierActiveWriteCallback(void *pp);
EXTERN int HumidifierDehumidifierCurrentRelativeHumidityReadCallback(void *pp);
EXTERN int HumidifierDehumidifierCurrentHumidifierDehumidifierStateReadCallback(void *pp);
EXTERN int HumidifierDehumidifierTargetHumidifierDehumidifierStateReadCallback(void *pp);
EXTERN int HumidifierDehumidifierTargetHumidifierDehumidifierStateWriteCallback(void *pp);
EXTERN int HumidifierDehumidifierRelativeHumidityDehumidifierThresholdReadCallback(void *pp);
EXTERN int HumidifierDehumidifierRelativeHumidityDehumidifierThresholdWriteCallback(void *pp);
EXTERN int HumidifierDehumidifierRelativeHumidityHumidifierThresholdReadCallback(void *pp);
EXTERN int HumidifierDehumidifierRelativeHumidityHumidifierThresholdWriteCallback(void *pp);
EXTERN int HumidifierDehumidifierRotationSpeedReadCallback(void *pp);
EXTERN int HumidifierDehumidifierRotationSpeedWriteCallback(void *pp);
EXTERN int HumidifierDehumidifierSwingModeReadCallback(void *pp);
EXTERN int HumidifierDehumidifierSwingModeWriteCallback(void *pp);
EXTERN int HumidifierDehumidifierWaterLevelReadCallback(void *pp);
EXTERN int HumidifierDehumidifierLockPhysicalControlsReadCallback(void *pp);
EXTERN int HumidifierDehumidifierLockPhysicalControlsWriteCallback(void *pp);

EXTERN int ccc_HumidifierDehumidifierActiveWriteCB(void *pp);
EXTERN int ccc_HumidifierDehumidifierCurrentRelativeHumidityWriteCB(void *pp);
EXTERN int ccc_HumidifierDehumidifierCurrentHumidifierDehumidifierStateWriteCB(void *pp);
EXTERN int ccc_HumidifierDehumidifierTargetHumidifierDehumidifierStateWriteCB(void *pp);
EXTERN int ccc_HumidifierDehumidifierRelativeHumidityDehumidifierThresholdWriteCB(void *pp);
EXTERN int ccc_HumidifierDehumidifierRelativeHumidityHumidifierThresholdWriteCB(void *pp);
EXTERN int ccc_HumidifierDehumidifierRotationSpeedWriteCB(void *pp);
EXTERN int ccc_HumidifierDehumidifierSwingModeWriteCB(void *pp);
EXTERN int ccc_HumidifierDehumidifierWaterLevelWriteCB(void *pp);
EXTERN int ccc_HumidifierDehumidifierLockPhysicalControlsWriteCB(void *pp);

#endif /* _HK_HUMIDIFIER_DEHUMIDIFIER_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
