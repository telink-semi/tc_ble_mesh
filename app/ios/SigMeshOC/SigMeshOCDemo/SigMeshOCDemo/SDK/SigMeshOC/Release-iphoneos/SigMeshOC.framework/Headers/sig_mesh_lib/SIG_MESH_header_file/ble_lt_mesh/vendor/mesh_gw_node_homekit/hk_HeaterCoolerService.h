/********************************************************************************************************
 * @file     hk_HeaterCoolerService.h
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
#ifndef _HK_HEATER_COOLER_SERVICE_H_
#define _HK_HEATER_COOLER_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_HEATER_COOLER_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif

#define CHAR_HEATERCOOLER_SVC_SIGN_ENABLE      0
#if CHAR_HEATERCOOLER_SVC_SIGN_ENABLE
    #define HEATERCOOLER_AS_PRIMARY_SERVICE    		0
    #define HEATERCOOLER_IS_HIDDEN_SERVICE     		0
    #define HEATERCOOLER_LINKED_SERVICES_ENABLE		0
    EXTERN const hap_characteristic_desc_t hap_desc_char_heater_cooler_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_heater_cooler_A5_R_S;
    EXTERN unsigned char VAR_HEATER_COOLER_UNKOWN;
#else
    #define HEATERCOOLER_AS_PRIMARY_SERVICE    		0
    #define HEATERCOOLER_IS_HIDDEN_SERVICE     		0
    #define HEATERCOOLER_LINKED_SERVICES_ENABLE		0
#endif


#define CHAR_HEATERCOOLER_ROTATIONSPEED_ENABLE              0
#define CHAR_HEATERCOOLER_TEMPERATUREDISPLAYUNITS_ENABLE    0
#define CHAR_HEATERCOOLER_SWINGMODE_ENABLE					0
#define CHAR_HEATERCOOLER_COOLINGTHRESHOLDTEMP_ENABLE       0
#define CHAR_HEATERCOOLER_HEATINGTHRESHOLDTEMP_ENABLE       0
#define CHAR_HEATERCOOLER_LOCKPHYSICALCONTROLS_ENABLE		0

#define HEATERCOOLER_ACTIVE_IND_ENABLE                      0
#define HEATERCOOLER_CURRENTTEMPERATURE_IND_ENABLE          0
#define HEATERCOOLER_CURRENTHEATERCOOLERSTATE_IND_ENABLE    0
#define HEATERCOOLER_TARGETHEATERCOOLERSTATE_IND_ENABLE     0
#define HEATERCOOLER_ROTATIONSPEED_IND_ENABLE               0
#define HEATERCOOLER_TEMPERATUREDISPLAYUNITS_IND_ENABLE     0
#define HEATERCOOLER_SWINGMODE_IND_ENABLE        			0
#define HEATERCOOLER_COOLINGTHRESHOLDTEMP_IND_ENABLE        0
#define HEATERCOOLER_HEATINGTHRESHOLDTEMP_IND_ENABLE        0
#define HEATERCOOLER_LOCKPHYSICALCONTROLS_IND_ENABLE        0

#define HEATERCOOLER_ACTIVE_BRC_ENABLE                      0
#define HEATERCOOLER_CURRENTTEMPERATURE_BRC_ENABLE          0
#define HEATERCOOLER_CURRENTHEATERCOOLERSTATE_BRC_ENABLE    0
#define HEATERCOOLER_TARGETHEATERCOOLERSTATE_BRC_ENABLE     0
#define HEATERCOOLER_ROTATIONSPEED_BRC_ENABLE               0
#define HEATERCOOLER_TEMPERATUREDISPLAYUNITS_BRC_ENABLE     0
#define HEATERCOOLER_SWINGMODE_BRC_ENABLE     				0
#define HEATERCOOLER_COOLINGTHRESHOLDTEMP_BRC_ENABLE        0
#define HEATERCOOLER_HEATINGTHRESHOLDTEMP_BRC_ENABLE        0
#define HEATERCOOLER_LOCKPHYSICALCONTROLS_BRC_ENABLE     	0

#define HEATERCOOLER_ACTIVE_NO      3
#if HEATERCOOLER_ACTIVE_IND_ENABLE
    #define HEATERCOOLER_ACTIVE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hHeaterCoolerActive), ccc_HeaterCoolerActiveWriteCB, 0},
    #define HEATERCOOLER_ACTIVE_IND_NO  1
#else
    #define HEATERCOOLER_ACTIVE_IND_DESC
    #define HEATERCOOLER_ACTIVE_IND_NO  0
#endif

#define HEATERCOOLER_CURRENTTEMPERATURE_NO      3
#if HEATERCOOLER_CURRENTTEMPERATURE_IND_ENABLE
    #define HEATERCOOLER_CURRENTTEMPERATURE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hHeaterCoolerCurrentTemperature), ccc_HeaterCoolerCurrentTemperatureWriteCB, 0},
    #define HEATERCOOLER_CURRENTTEMPERATURE_IND_NO  1
#else
    #define HEATERCOOLER_CURRENTTEMPERATURE_IND_DESC
    #define HEATERCOOLER_CURRENTTEMPERATURE_IND_NO  0
#endif

#define HEATERCOOLER_CURRENTHEATERCOOLERSTATE_NO      3
#if HEATERCOOLER_CURRENTHEATERCOOLERSTATE_IND_ENABLE
    #define HEATERCOOLER_CURRENTHEATERCOOLERSTATE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hHeaterCoolerCurrentHeaterCoolerState), ccc_HeaterCoolerCurrentHeaterCoolerStateWriteCB, 0},
    #define HEATERCOOLER_CURRENTHEATERCOOLERSTATE_IND_NO  1
#else
    #define HEATERCOOLER_CURRENTHEATERCOOLERSTATE_IND_DESC
    #define HEATERCOOLER_CURRENTHEATERCOOLERSTATE_IND_NO  0
#endif

#define HEATERCOOLER_TARGETHEATERCOOLERSTATE_NO      3
#if HEATERCOOLER_TARGETHEATERCOOLERSTATE_IND_ENABLE
    #define HEATERCOOLER_TARGETHEATERCOOLERSTATE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hHeaterCoolerTargetHeaterCoolerState), ccc_HeaterCoolerTargetHeaterCoolerStateWriteCB, 0},
    #define HEATERCOOLER_TARGETHEATERCOOLERSTATE_IND_NO  1
#else
    #define HEATERCOOLER_TARGETHEATERCOOLERSTATE_IND_DESC
    #define HEATERCOOLER_TARGETHEATERCOOLERSTATE_IND_NO  0
#endif

#if CHAR_HEATERCOOLER_ROTATIONSPEED_ENABLE
    #define HEATERCOOLER_ROTATIONSPEED_NO 3
    #define CHAR_HEATERCOOLER_ROTATIONSPEED_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_HEATERCOOLER_ROTATIONSPEED_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof (hHeaterCoolerRotationSpeed), 0, (u8*)(&charRotationSpeedUUID), (u8*)&hHeaterCoolerRotationSpeed, HeaterCoolerRotationSpeedWriteCallback, HeaterCoolerRotationSpeedReadCallback},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + HEATERCOOLER_INST_NO + 6), 0, 0}, \
            HEATERCOOLER_ROTATIONSPEED_IND_DESC

    #if HEATERCOOLER_ROTATIONSPEED_IND_ENABLE
        #define HEATERCOOLER_ROTATIONSPEED_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hHeaterCoolerRotationSpeed), ccc_HeaterCoolerRotationSpeedWriteCB, 0},
        #define HEATERCOOLER_ROTATIONSPEED_IND_NO  1
    #else
        #define HEATERCOOLER_ROTATIONSPEED_IND_DESC
        #define HEATERCOOLER_ROTATIONSPEED_IND_NO  0
    #endif
#else
    #define HEATERCOOLER_ROTATIONSPEED_NO 0
    #define CHAR_HEATERCOOLER_ROTATIONSPEED_ATT_TABLE
    #define HEATERCOOLER_ROTATIONSPEED_IND_NO  0
#endif

#if CHAR_HEATERCOOLER_TEMPERATUREDISPLAYUNITS_ENABLE
    #define HEATERCOOLER_TEMPERATUREDISPLAYUNITS_NO 3
    #define CHAR_HEATERCOOLER_TEMPERATUREDISPLAYUNITS_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_HEATERCOOLER_TEMPERATUREDISPLAYUNITS_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof (hHeaterCoolerTemperatureDisplayUnits), 0, (u8*)(&charTemperatureDisplayUnitsUUID), (u8*)&hHeaterCoolerTemperatureDisplayUnits, HeaterCoolerTemperatureDisplayUnitsWriteCallback, HeaterCoolerTemperatureDisplayUnitsReadCallback},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + HEATERCOOLER_INST_NO + 7), 0, 0}, \
            HEATERCOOLER_TEMPERATUREDISPLAYUNITS_IND_DESC

    #if HEATERCOOLER_TEMPERATUREDISPLAYUNITS_IND_ENABLE
        #define HEATERCOOLER_TEMPERATUREDISPLAYUNITS_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hHeaterCoolerTemperatureDisplayUnits), ccc_HeaterCoolerTemperatureDisplayUnitsWriteCB, 0},
        #define HEATERCOOLER_TEMPERATUREDISPLAYUNITS_IND_NO  1
    #else
        #define HEATERCOOLER_TEMPERATUREDISPLAYUNITS_IND_DESC
        #define HEATERCOOLER_TEMPERATUREDISPLAYUNITS_IND_NO  0
    #endif
#else
    #define HEATERCOOLER_TEMPERATUREDISPLAYUNITS_NO 0
    #define CHAR_HEATERCOOLER_TEMPERATUREDISPLAYUNITS_ATT_TABLE
    #define HEATERCOOLER_TEMPERATUREDISPLAYUNITS_IND_NO  0
#endif

#if CHAR_HEATERCOOLER_SWINGMODE_ENABLE
    #define HEATERCOOLER_SWINGMODE_NO   3
    #define CHAR_HEATERCOOLER_SWINGMODE_ATT_TABLE   \
            {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_HEATERCOOLER_SWINGMODE_R_S, 0},    \
            {0,ATT_PERMISSIONS_RDWR,16,sizeof(hHeaterCoolerSwingMode),0, (u8*)(charSwingModeUUID), (u8*)&hHeaterCoolerSwingMode, HeaterCoolerSwingModeWriteCallback, HeaterCoolerSwingModeReadCallback}, \
                {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + HEATERCOOLER_INST_NO + 8), 0, 0}, \
                HEATERCOOLER_SWINGMODE_IND_DESC

    #if HEATERCOOLER_SWINGMODE_IND_ENABLE
        #define HEATERCOOLER_SWINGMODE_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hHeaterCoolerSwingMode), ccc_HeaterCoolerSwingModeWriteCB, 0},
        #define HEATERCOOLER_SWINGMODE_IND_NO  1
    #else
        #define HEATERCOOLER_SWINGMODE_IND_DESC
        #define HEATERCOOLER_SWINGMODE_IND_NO  0
    #endif
#else
    #define HEATERCOOLER_SWINGMODE_NO   0
    #define CHAR_HEATERCOOLER_SWINGMODE_ATT_TABLE
    #define HEATERCOOLER_SWINGMODE_IND_NO  0
#endif


#if CHAR_HEATERCOOLER_COOLINGTHRESHOLDTEMP_ENABLE
    #define HEATERCOOLER_COOLINGTHRESHOLDTEMP_NO 3
    #define CHAR_HEATERCOOLER_COOLINGTHRESHOLDTEMP_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_HEATERCOOLER_COOLINGTHRESHOLDTEMP_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof (hHeaterCoolerCoolingThresholdTemp), 0, (u8*)(&charCoolingThresholdTemperatureUUID), (u8*)&hHeaterCoolerCoolingThresholdTemp, HeaterCoolerCoolingThresholdTempWriteCallback, HeaterCoolerCoolingThresholdTempReadCallback},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + HEATERCOOLER_INST_NO + 9), 0, 0}, \
            HEATERCOOLER_COOLINGTHRESHOLDTEMP_IND_DESC

    #if HEATERCOOLER_COOLINGTHRESHOLDTEMP_IND_ENABLE
        #define HEATERCOOLER_COOLINGTHRESHOLDTEMP_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hHeaterCoolerCoolingThresholdTemp), ccc_HeaterCoolerCoolingThresholdTempWriteCB, 0},
        #define HEATERCOOLER_COOLINGTHRESHOLDTEMP_IND_NO  1
    #else
        #define HEATERCOOLER_COOLINGTHRESHOLDTEMP_IND_DESC
        #define HEATERCOOLER_COOLINGTHRESHOLDTEMP_IND_NO  0
    #endif
#else
    #define HEATERCOOLER_COOLINGTHRESHOLDTEMP_NO 0
    #define CHAR_HEATERCOOLER_COOLINGTHRESHOLDTEMP_ATT_TABLE
    #define HEATERCOOLER_COOLINGTHRESHOLDTEMP_IND_NO  0
#endif

#if CHAR_HEATERCOOLER_HEATINGTHRESHOLDTEMP_ENABLE
    #define HEATERCOOLER_HEATINGTHRESHOLDTEMP_NO 3
    #define CHAR_HEATERCOOLER_HEATINGTHRESHOLDTEMP_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_HEATERCOOLER_HEATINGTHRESHOLDTEMP_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof (hHeaterCoolerHeatingThresholdTemp), 0, (u8*)(&charHeatingThresholdTemperatureUUID), (u8*)&hHeaterCoolerHeatingThresholdTemp, HeaterCoolerHeatingThresholdTempWriteCallback, HeaterCoolerHeatingThresholdTempReadCallback},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + HEATERCOOLER_INST_NO + 10), 0, 0}, \
            HEATERCOOLER_HEATINGTHRESHOLDTEMP_IND_DESC

    #if HEATERCOOLER_HEATINGTHRESHOLDTEMP_IND_ENABLE
        #define HEATERCOOLER_HEATINGTHRESHOLDTEMP_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hHeaterCoolerHeatingThresholdTemp), ccc_HeaterCoolerHeatingThresholdTempWriteCB, 0},
        #define HEATERCOOLER_HEATINGTHRESHOLDTEMP_IND_NO  1
    #else
        #define HEATERCOOLER_HEATINGTHRESHOLDTEMP_IND_DESC
        #define HEATERCOOLER_HEATINGTHRESHOLDTEMP_IND_NO  0
    #endif
#else
    #define HEATERCOOLER_HEATINGTHRESHOLDTEMP_NO 0
    #define CHAR_HEATERCOOLER_HEATINGTHRESHOLDTEMP_ATT_TABLE
    #define HEATERCOOLER_HEATINGTHRESHOLDTEMP_IND_NO  0
#endif

#if CHAR_HEATERCOOLER_LOCKPHYSICALCONTROLS_ENABLE
    #define HEATERCOOLER_LOCKPHYSICALCONTROLS_NO   3
    #define CHAR_HEATERCOOLER_LOCKPHYSICALCONTROLS_ATT_TABLE   \
            {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_HEATERCOOLER_LOCKPHYSICALCONTROLS_R_S, 0},    \
            {0,ATT_PERMISSIONS_RDWR,16,sizeof(hHeaterCoolerLockPhysicalControls),0, (u8*)(charLockPhysicalControlsUUID), (u8*)&hHeaterCoolerLockPhysicalControls, HeaterCoolerLockPhysicalControlsWriteCallback, HeaterCoolerLockPhysicalControlsReadCallback}, \
                {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + HEATERCOOLER_INST_NO + 11), 0, 0}, \
                HEATERCOOLER_LOCKPHYSICALCONTROLS_IND_DESC

    #if HEATERCOOLER_LOCKPHYSICALCONTROLS_IND_ENABLE
        #define HEATERCOOLER_LOCKPHYSICALCONTROLS_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hHeaterCoolerLockPhysicalControls), ccc_HeaterCoolerLockPhysicalControlsWriteCB, 0},
        #define HEATERCOOLER_LOCKPHYSICALCONTROLS_IND_NO  1
    #else
        #define HEATERCOOLER_LOCKPHYSICALCONTROLS_IND_DESC
        #define HEATERCOOLER_LOCKPHYSICALCONTROLS_IND_NO  0
    #endif
#else
    #define HEATERCOOLER_LOCKPHYSICALCONTROLS_NO   0
    #define CHAR_HEATERCOOLER_LOCKPHYSICALCONTROLS_ATT_TABLE
    #define HEATERCOOLER_LOCKPHYSICALCONTROLS_IND_NO  0
#endif


#if CHAR_HEATERCOOLER_SVC_SIGN_ENABLE
    #define HEATERCOOLER_SVC_SIGN_NO   3
    #define CHAR_HEATERCOOLER_SVC_A5_SIGN_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_heater_cooler_A5_R_S, &hap_desc_service_heater_cooler_A5_R_S},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_HEATER_COOLER_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_HEATER_COOLER_UNKOWN, 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + HEATERCOOLER_INST_NO + 12), 0, 0},
#else
    #define HEATERCOOLER_SVC_SIGN_NO   0
    #define CHAR_HEATERCOOLER_SVC_A5_SIGN_TABLE
#endif

#define HEATERCOOLER_NO_DELTA   HEATERCOOLER_ACTIVE_IND_NO \
                                + HEATERCOOLER_CURRENTTEMPERATURE_IND_NO \
                                + HEATERCOOLER_CURRENTHEATERCOOLERSTATE_IND_NO \
                                + HEATERCOOLER_TARGETHEATERCOOLERSTATE_IND_NO \
                                + HEATERCOOLER_ROTATIONSPEED_IND_NO \
                                + HEATERCOOLER_TEMPERATUREDISPLAYUNITS_IND_NO \
                                + HEATERCOOLER_SWINGMODE_IND_NO \
                                + HEATERCOOLER_COOLINGTHRESHOLDTEMP_IND_NO \
                                + HEATERCOOLER_HEATINGTHRESHOLDTEMP_IND_NO \
                                + HEATERCOOLER_LOCKPHYSICALCONTROLS_IND_NO

#define HEATERCOOLER_NO         18 + HEATERCOOLER_ROTATIONSPEED_NO \
                                + HEATERCOOLER_TEMPERATUREDISPLAYUNITS_NO \
                                + HEATERCOOLER_SWINGMODE_NO \
                                + HEATERCOOLER_COOLINGTHRESHOLDTEMP_NO \
                                + HEATERCOOLER_HEATINGTHRESHOLDTEMP_NO \
                                + HEATERCOOLER_LOCKPHYSICALCONTROLS_NO \
                                + HEATERCOOLER_SVC_SIGN_NO

#define HEATERCOOLER_HANDLE_NUM (HEATERCOOLER_NO + HEATERCOOLER_NO_DELTA)


#define HOMEKIT_HEATERCOOLER_SERVICE_ATT_TABLE   \
    {HEATERCOOLER_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(heater_coolerServiceUUID),sizeof(heater_coolerServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(heater_coolerServiceUUID), 0, 0},   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(insts + HEATERCOOLER_INST_NO), 0, 0},   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_NAME, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(hHeaterCoolerServiceName), 0, (u8*)(nameUUID), (u8*)(hHeaterCoolerServiceName), 0, 0},  \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + HEATERCOOLER_INST_NO + 1), 0, 0}, \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_HEATERCOOLER_ACTIVE_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hHeaterCoolerActive),0, (u8*)(charActiveUUID), (u8*)&hHeaterCoolerActive, HeaterCoolerActiveWriteCallback, HeaterCoolerActiveReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + HEATERCOOLER_INST_NO + 2), 0, 0},  \
            HEATERCOOLER_ACTIVE_IND_DESC \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_HEATERCOOLER_CURRENTTEMPERATURE_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hHeaterCoolerCurrentTemperature),0, (u8*)(charCurrentTemperatureUUID), (u8*)&hHeaterCoolerCurrentTemperature, 0, HeaterCoolerCurrentTemperatureReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + HEATERCOOLER_INST_NO + 3), 0, 0},  \
            HEATERCOOLER_CURRENTTEMPERATURE_IND_DESC \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_HEATERCOOLER_CURRENTHEATERCOOLERSTATE_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hHeaterCoolerCurrentHeaterCoolerState),0, (u8*)(charCurrentHeaterCoolerStateUUID), (u8*)&hHeaterCoolerCurrentHeaterCoolerState, 0, HeaterCoolerCurrentHeaterCoolerStateReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + HEATERCOOLER_INST_NO + 4), 0, 0},  \
            HEATERCOOLER_CURRENTHEATERCOOLERSTATE_IND_DESC \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_HEATERCOOLER_TARGETHEATERCOOLERSTATE_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hHeaterCoolerTargetHeaterCoolerState),0, (u8*)(charTargetHeaterCoolerStateUUID), (u8*)&hHeaterCoolerTargetHeaterCoolerState, HeaterCoolerTargetHeaterCoolerStateWriteCallback, HeaterCoolerTargetHeaterCoolerStateReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + HEATERCOOLER_INST_NO + 5), 0, 0},  \
            HEATERCOOLER_TARGETHEATERCOOLERSTATE_IND_DESC \
        CHAR_HEATERCOOLER_ROTATIONSPEED_ATT_TABLE \
        CHAR_HEATERCOOLER_TEMPERATUREDISPLAYUNITS_ATT_TABLE \
        CHAR_HEATERCOOLER_SWINGMODE_ATT_TABLE \
        CHAR_HEATERCOOLER_COOLINGTHRESHOLDTEMP_ATT_TABLE \
        CHAR_HEATERCOOLER_HEATINGTHRESHOLDTEMP_ATT_TABLE \
        CHAR_HEATERCOOLER_LOCKPHYSICALCONTROLS_ATT_TABLE \
        CHAR_HEATERCOOLER_SVC_A5_SIGN_TABLE


#define HEATERCOOLER_HANDLE_COMMON_NUM     8

#define HEATERCOOLER_HANDLE_NUM_ACTIVE                      HEATERCOOLER_HANDLE_START_NUM +HEATERCOOLER_HANDLE_COMMON_NUM
#define HEATERCOOLER_HANDLE_NUM_CURRENTTEMPERATURE          HEATERCOOLER_HANDLE_NUM_ACTIVE +HEATERCOOLER_ACTIVE_NO +HEATERCOOLER_ACTIVE_IND_NO
#define HEATERCOOLER_HANDLE_NUM_CURRENTHEATERCOOLERSTATE    HEATERCOOLER_HANDLE_NUM_CURRENTTEMPERATURE +HEATERCOOLER_CURRENTTEMPERATURE_NO +HEATERCOOLER_CURRENTTEMPERATURE_IND_NO
#define HEATERCOOLER_HANDLE_NUM_TARGETHEATERCOOLERSTATE     HEATERCOOLER_HANDLE_NUM_CURRENTHEATERCOOLERSTATE +HEATERCOOLER_CURRENTHEATERCOOLERSTATE_NO +HEATERCOOLER_CURRENTHEATERCOOLERSTATE_IND_NO
#define HEATERCOOLER_HANDLE_NUM_ROTATIONSPEED               HEATERCOOLER_HANDLE_NUM_TARGETHEATERCOOLERSTATE +HEATERCOOLER_TARGETHEATERCOOLERSTATE_NO +HEATERCOOLER_TARGETHEATERCOOLERSTATE_IND_NO
#define HEATERCOOLER_HANDLE_NUM_TEMPERATUREDISPLAYUNITS     HEATERCOOLER_HANDLE_NUM_ROTATIONSPEED +HEATERCOOLER_ROTATIONSPEED_NO +HEATERCOOLER_ROTATIONSPEED_IND_NO
#define HEATERCOOLER_HANDLE_NUM_SWINGMODE     				HEATERCOOLER_HANDLE_NUM_TEMPERATUREDISPLAYUNITS +HEATERCOOLER_TEMPERATUREDISPLAYUNITS_NO +HEATERCOOLER_TEMPERATUREDISPLAYUNITS_IND_NO
#define HEATERCOOLER_HANDLE_NUM_COOLINGTHRESHOLDTEMP        HEATERCOOLER_HANDLE_NUM_SWINGMODE +HEATERCOOLER_SWINGMODE_NO +HEATERCOOLER_SWINGMODE_IND_NO
#define HEATERCOOLER_HANDLE_NUM_HEATINGTHRESHOLDTEMP        HEATERCOOLER_HANDLE_NUM_COOLINGTHRESHOLDTEMP +HEATERCOOLER_COOLINGTHRESHOLDTEMP_NO +HEATERCOOLER_COOLINGTHRESHOLDTEMP_IND_NO
#define HEATERCOOLER_HANDLE_NUM_LOCKPHYSICALCONTROLS        HEATERCOOLER_HANDLE_NUM_HEATINGTHRESHOLDTEMP +HEATERCOOLER_HEATINGTHRESHOLDTEMP_NO +HEATERCOOLER_HEATINGTHRESHOLDTEMP_IND_NO
#define HEATERCOOLER_HANDLE_NUM_THE_END

#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_ACTIVE                         u32 HeaterCoolerActive:1;     /* 0~1 */
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_ACTIVE_STORE                       STORE_CHAR(HeaterCoolerActive, hHeaterCoolerActive)
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_ACTIVE_RESTORE                     RESTORE_CHAR(HeaterCoolerActive, hHeaterCoolerActive)

#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_CURRENTTEMPERATURE             u32 HeaterCoolerCurrentTemperature;   /* 0~100 */
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_CURRENTTEMPERATURE_STORE           STORE_CHAR(HeaterCoolerCurrentTemperature, hHeaterCoolerCurrentTemperature)
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_CURRENTTEMPERATURE_RESTORE         RESTORE_CHAR(HeaterCoolerCurrentTemperature, hHeaterCoolerCurrentTemperature)

#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_CURRENTHEATERCOOLERSTATE       u32 HeaterCoolerCurrentHeaterCoolerState:2;   /* 0~3 */
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_CURRENTHEATERCOOLERSTATE_STORE     STORE_CHAR(HeaterCoolerCurrentHeaterCoolerState, hHeaterCoolerCurrentHeaterCoolerState)
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_CURRENTHEATERCOOLERSTATE_RESTORE   RESTORE_CHAR(HeaterCoolerCurrentHeaterCoolerState, hHeaterCoolerCurrentHeaterCoolerState)

#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_TARGETHEATERCOOLERSTATE        u32 HeaterCoolerTargetHeaterCoolerState:2;   /* 0~2 */
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_TARGETHEATERCOOLERSTATE_STORE      STORE_CHAR(HeaterCoolerTargetHeaterCoolerState, hHeaterCoolerTargetHeaterCoolerState)
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_TARGETHEATERCOOLERSTATE_RESTORE    RESTORE_CHAR(HeaterCoolerTargetHeaterCoolerState, hHeaterCoolerTargetHeaterCoolerState)


#if CHAR_HEATERCOOLER_ROTATIONSPEED_ENABLE
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_ROTATIONSPEED                  u32 HeaterCoolerRotationSpeed;   /* float */
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_ROTATIONSPEED_STORE                STORE_CHAR(HeaterCoolerRotationSpeed, hHeaterCoolerRotationSpeed)
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_ROTATIONSPEED_RESTORE              RESTORE_CHAR(HeaterCoolerRotationSpeed, hHeaterCoolerRotationSpeed)
#else
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_ROTATIONSPEED
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_ROTATIONSPEED_STORE
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_ROTATIONSPEED_RESTORE
#endif

#if CHAR_HEATERCOOLER_TEMPERATUREDISPLAYUNITS_ENABLE
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_TEMPERATUREDISPLAYUNITS        u32 HeaterCoolerTemperatureDisplayUnits:1;   /* 0~1 */
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_TEMPERATUREDISPLAYUNITS_STORE      STORE_CHAR(HeaterCoolerTemperatureDisplayUnits, hHeaterCoolerTemperatureDisplayUnits)
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_TEMPERATUREDISPLAYUNITS_RESTORE    RESTORE_CHAR(HeaterCoolerTemperatureDisplayUnits, hHeaterCoolerTemperatureDisplayUnits)
#else
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_TEMPERATUREDISPLAYUNITS
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_TEMPERATUREDISPLAYUNITS_STORE
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_TEMPERATUREDISPLAYUNITS_RESTORE
#endif

#if CHAR_HEATERCOOLER_SWINGMODE_ENABLE
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_SWINGMODE                 		u32 HeaterCoolerSwingMode:1;    /* 0~1 */
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_SWINGMODE_STORE               		STORE_CHAR(HeaterCoolerSwingMode,hHeaterCoolerSwingMode)
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_SWINGMODE_RESTORE             		RESTORE_CHAR(HeaterCoolerSwingMode,hHeaterCoolerSwingMode)
#else
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_SWINGMODE
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_SWINGMODE_STORE
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_SWINGMODE_RESTORE
#endif

#if CHAR_HEATERCOOLER_COOLINGTHRESHOLDTEMP_ENABLE
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_COOLINGTHRESHOLDTEMP           u32 HeaterCoolerCoolingThresholdTemp;   /* float */
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_COOLINGTHRESHOLDTEMP_STORE         STORE_CHAR(HeaterCoolerCoolingThresholdTemp, hHeaterCoolerCoolingThresholdTemp)
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_COOLINGTHRESHOLDTEMP_RESTORE       RESTORE_CHAR(HeaterCoolerCoolingThresholdTemp, hHeaterCoolerCoolingThresholdTemp)
#else
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_COOLINGTHRESHOLDTEMP
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_COOLINGTHRESHOLDTEMP_STORE
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_COOLINGTHRESHOLDTEMP_RESTORE
#endif

#if CHAR_HEATERCOOLER_HEATINGTHRESHOLDTEMP_ENABLE
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_HEATINGTHRESHOLDTEMP           u32 HeaterCoolerHeatingThresholdTemp;   /* float */
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_HEATINGTHRESHOLDTEMP_STORE         STORE_CHAR(HeaterCoolerHeatingThresholdTemp, hHeaterCoolerHeatingThresholdTemp)
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_HEATINGTHRESHOLDTEMP_RESTORE       RESTORE_CHAR(HeaterCoolerHeatingThresholdTemp, hHeaterCoolerHeatingThresholdTemp)
#else
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_HEATINGTHRESHOLDTEMP
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_HEATINGTHRESHOLDTEMP_STORE
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_HEATINGTHRESHOLDTEMP_RESTORE
#endif

#if CHAR_HEATERCOOLER_LOCKPHYSICALCONTROLS_ENABLE
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_LOCKPHYSICALCONTROLS      		u32 HeaterCoolerLockPhysicalControls:1;    /* 0~1 */
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_LOCKPHYSICALCONTROLS_STORE  		STORE_CHAR(HeaterCoolerLockPhysicalControls,hHeaterCoolerLockPhysicalControls)
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_LOCKPHYSICALCONTROLS_RESTORE  		RESTORE_CHAR(HeaterCoolerLockPhysicalControls,hHeaterCoolerLockPhysicalControls)
#else
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_LOCKPHYSICALCONTROLS
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_LOCKPHYSICALCONTROLS_STORE
#define HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_LOCKPHYSICALCONTROLS_RESTORE
#endif

#define HEATERCOOLER_STORE_CHAR_VALUE_T            \
        HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_ACTIVE \
        HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_CURRENTTEMPERATURE \
        HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_CURRENTHEATERCOOLERSTATE \
        HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_TARGETHEATERCOOLERSTATE \
        HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_ROTATIONSPEED \
        HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_TEMPERATUREDISPLAYUNITS \
        HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_SWINGMODE \
        HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_COOLINGTHRESHOLDTEMP \
        HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_HEATINGTHRESHOLDTEMP \
        HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_LOCKPHYSICALCONTROLS

#define HEATERCOOLER_STORE_CHAR_TO_FLASH           \
        HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_ACTIVE_STORE \
        HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_CURRENTTEMPERATURE_STORE \
        HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_CURRENTHEATERCOOLERSTATE_STORE \
        HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_TARGETHEATERCOOLERSTATE_STORE \
        HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_ROTATIONSPEED_STORE \
        HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_TEMPERATUREDISPLAYUNITS_STORE \
        HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_SWINGMODE_STORE \
        HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_COOLINGTHRESHOLDTEMP_STORE \
        HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_HEATINGTHRESHOLDTEMP_STORE \
        HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_LOCKPHYSICALCONTROLS_STORE

#define HEATERCOOLER_RESTORE_CHAR_FROM_FLASH       \
        HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_ACTIVE_RESTORE \
        HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_CURRENTTEMPERATURE_RESTORE \
        HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_CURRENTHEATERCOOLERSTATE_RESTORE \
        HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_TARGETHEATERCOOLERSTATE_RESTORE \
        HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_ROTATIONSPEED_RESTORE \
        HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_TEMPERATUREDISPLAYUNITS_RESTORE \
        HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_SWINGMODE_RESTORE \
        HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_COOLINGTHRESHOLDTEMP_RESTORE \
        HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_HEATINGTHRESHOLDTEMP_RESTORE \
        HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_LOCKPHYSICALCONTROLS_RESTORE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN unsigned char hHeaterCoolerActive;
EXTERN unsigned int hHeaterCoolerCurrentTemperature;
EXTERN unsigned char hHeaterCoolerCurrentHeaterCoolerState;
EXTERN unsigned char hHeaterCoolerTargetHeaterCoolerState;
EXTERN unsigned int hHeaterCoolerRotationSpeed;
EXTERN unsigned char hHeaterCoolerTemperatureDisplayUnits;
EXTERN unsigned char hHeaterCoolerSwingMode;
EXTERN unsigned int hHeaterCoolerCoolingThresholdTemp;
EXTERN unsigned int hHeaterCoolerHeatingThresholdTemp;
EXTERN unsigned char hHeaterCoolerLockPhysicalControls;

EXTERN unsigned short ccc_hHeaterCoolerActive;
EXTERN unsigned short ccc_hHeaterCoolerCurrentTemperature;
EXTERN unsigned short ccc_hHeaterCoolerCurrentHeaterCoolerState;
EXTERN unsigned short ccc_hHeaterCoolerTargetHeaterCoolerState;
EXTERN unsigned short ccc_hHeaterCoolerRotationSpeed;
EXTERN unsigned short ccc_hHeaterCoolerTemperatureDisplayUnits;
EXTERN unsigned short ccc_hHeaterCoolerSwingMode;
EXTERN unsigned short ccc_hHeaterCoolerCoolingThresholdTemp;
EXTERN unsigned short ccc_hHeaterCoolerHeatingThresholdTemp;
EXTERN unsigned short ccc_hHeaterCoolerLockPhysicalControls;

#if HEATERCOOLER_ACTIVE_BRC_ENABLE
    #define BC_PARA_HEATERCOOLER_ACTIVE ((u8*)&bc_para_hHeaterCoolerActive)
    EXTERN unsigned char bc_para_hHeaterCoolerActive;
#else
    #define BC_PARA_HEATERCOOLER_ACTIVE 0
#endif

#if HEATERCOOLER_CURRENTTEMPERATURE_BRC_ENABLE
    #define BC_PARA_HEATERCOOLER_CURRENTTEMPERATURE ((u8*)&bc_para_hHeaterCoolerCurrentTemperature)
    EXTERN unsigned char bc_para_hHeaterCoolerCurrentTemperature;
#else
    #define BC_PARA_HEATERCOOLER_CURRENTTEMPERATURE 0
#endif

#if HEATERCOOLER_CURRENTHEATERCOOLERSTATE_BRC_ENABLE
    #define BC_PARA_HEATERCOOLER_CURRENTHEATERCOOLERSTATE ((u8*)&bc_para_hHeaterCoolerCurrentHeaterCoolerState)
    EXTERN unsigned char bc_para_hHeaterCoolerCurrentHeaterCoolerState;
#else
    #define BC_PARA_HEATERCOOLER_CURRENTHEATERCOOLERSTATE 0
#endif

#if HEATERCOOLER_TARGETHEATERCOOLERSTATE_BRC_ENABLE
    #define BC_PARA_HEATERCOOLER_TARGETHEATERCOOLERSTATE ((u8*)&bc_para_hHeaterCoolerTargetHeaterCoolerState)
    EXTERN unsigned char bc_para_hHeaterCoolerTargetHeaterCoolerState;
#else
    #define BC_PARA_HEATERCOOLER_TARGETHEATERCOOLERSTATE 0
#endif

#if HEATERCOOLER_ROTATIONSPEED_BRC_ENABLE
    #define BC_PARA_HEATERCOOLER_ROTATIONSPEED ((u8*)&bc_para_hHeaterCoolerRotationSpeed)
    EXTERN unsigned char bc_para_hHeaterCoolerRotationSpeed;
#else
    #define BC_PARA_HEATERCOOLER_ROTATIONSPEED 0
#endif

#if HEATERCOOLER_TEMPERATUREDISPLAYUNITS_BRC_ENABLE
    #define BC_PARA_HEATERCOOLER_TEMPERATUREDISPLAYUNITS ((u8*)&bc_para_hHeaterCoolerTemperatureDisplayUnits)
    EXTERN unsigned char bc_para_hHeaterCoolerTemperatureDisplayUnits;
#else
    #define BC_PARA_HEATERCOOLER_TEMPERATUREDISPLAYUNITS 0
#endif

#if HEATERCOOLER_SWINGMODE_BRC_ENABLE
    #define BC_PARA_HEATERCOOLER_SWINGMODE ((u8*)&bc_para_hHeaterCoolerSwingMode)
    EXTERN unsigned char bc_para_hHeaterCoolerSwingMode;
#else
    #define BC_PARA_HEATERCOOLER_SWINGMODE 0
#endif

#if HEATERCOOLER_COOLINGTHRESHOLDTEMP_BRC_ENABLE
    #define BC_PARA_HEATERCOOLER_COOLINGTHRESHOLDTEMP ((u8*)&bc_para_hHeaterCoolerCoolingThresholdTemp)
    EXTERN unsigned char bc_para_hHeaterCoolerCoolingThresholdTemp;
#else
    #define BC_PARA_HEATERCOOLER_COOLINGTHRESHOLDTEMP 0
#endif

#if HEATERCOOLER_HEATINGTHRESHOLDTEMP_BRC_ENABLE
    #define BC_PARA_HEATERCOOLER_HEATINGTHRESHOLDTEMP ((u8*)&bc_para_hHeaterCoolerHeatingThresholdTemp)
    EXTERN unsigned char bc_para_hHeaterCoolerHeatingThresholdTemp;
#else
    #define BC_PARA_HEATERCOOLER_HEATINGTHRESHOLDTEMP 0
#endif

#if HEATERCOOLER_LOCKPHYSICALCONTROLS_BRC_ENABLE
    #define BC_PARA_HEATERCOOLER_LOCKPHYSICALCONTROLS ((u8*)&bc_para_hHeaterCoolerLockPhysicalControls)
    EXTERN unsigned char bc_para_hHeaterCoolerLockPhysicalControls;
#else
    #define BC_PARA_HEATERCOOLER_LOCKPHYSICALCONTROLS 0
#endif

#if HEATERCOOLER_LINKED_SERVICES_ENABLE
	#define HEATERCOOLER_LINKED_SVCS			hHeaterCooler_linked_svcs
	#define HEATERCOOLER_LINKED_SVCS_LENGTH 	(sizeof(hHeaterCooler_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * hHeaterCooler_linked_svcs[];
#else
	#define HEATERCOOLER_LINKED_SVCS			0
	#define HEATERCOOLER_LINKED_SVCS_LENGTH 	0
#endif

EXTERN const u8 hHeaterCoolerServiceName[19];

EXTERN const hap_characteristic_desc_t hap_desc_char_HEATERCOOLER_ACTIVE_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_HEATERCOOLER_CURRENTTEMPERATURE_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_HEATERCOOLER_CURRENTHEATERCOOLERSTATE_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_HEATERCOOLER_TARGETHEATERCOOLERSTATE_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_HEATERCOOLER_ROTATIONSPEED_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_HEATERCOOLER_TEMPERATUREDISPLAYUNITS_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_HEATERCOOLER_SWINGMODE_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_HEATERCOOLER_COOLINGTHRESHOLDTEMP_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_HEATERCOOLER_HEATINGTHRESHOLDTEMP_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_HEATERCOOLER_LOCKPHYSICALCONTROLS_R_S;


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN int HeaterCoolerActiveReadCallback(void *pp);
EXTERN int HeaterCoolerActiveWriteCallback(void *pp);
EXTERN int HeaterCoolerCurrentTemperatureReadCallback(void *pp);
EXTERN int HeaterCoolerCurrentHeaterCoolerStateReadCallback(void *pp);
EXTERN int HeaterCoolerTargetHeaterCoolerStateReadCallback(void *pp);
EXTERN int HeaterCoolerTargetHeaterCoolerStateWriteCallback(void *pp);
EXTERN int HeaterCoolerRotationSpeedReadCallback(void *pp);
EXTERN int HeaterCoolerRotationSpeedWriteCallback(void *pp);
EXTERN int HeaterCoolerTemperatureDisplayUnitsReadCallback(void *pp);
EXTERN int HeaterCoolerTemperatureDisplayUnitsWriteCallback(void *pp);
EXTERN int HeaterCoolerSwingModeReadCallback(void *pp);
EXTERN int HeaterCoolerSwingModeWriteCallback(void *pp);
EXTERN int HeaterCoolerCoolingThresholdTempReadCallback(void *pp);
EXTERN int HeaterCoolerCoolingThresholdTempWriteCallback(void *pp);
EXTERN int HeaterCoolerHeatingThresholdTempReadCallback(void *pp);
EXTERN int HeaterCoolerHeatingThresholdTempWriteCallback(void *pp);
EXTERN int HeaterCoolerLockPhysicalControlsReadCallback(void *pp);
EXTERN int HeaterCoolerLockPhysicalControlsWriteCallback(void *pp);

EXTERN int ccc_HeaterCoolerActiveWriteCB(void *pp);
EXTERN int ccc_HeaterCoolerCurrentTemperatureWriteCB(void *pp);
EXTERN int ccc_HeaterCoolerCurrentHeaterCoolerStateWriteCB(void *pp);
EXTERN int ccc_HeaterCoolerTargetHeaterCoolerStateWriteCB(void *pp);
EXTERN int ccc_HeaterCoolerRotationSpeedWriteCB(void *pp);
EXTERN int ccc_HeaterCoolerTemperatureDisplayUnitsWriteCB(void *pp);
EXTERN int ccc_HeaterCoolerSwingModeWriteCB(void *pp);
EXTERN int ccc_HeaterCoolerCoolingThresholdTempWriteCB(void *pp);
EXTERN int ccc_HeaterCoolerHeatingThresholdTempWriteCB(void *pp);
EXTERN int ccc_HeaterCoolerLockPhysicalControlsWriteCB(void *pp);


#endif /* _HK_HEATER_COOLER_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
