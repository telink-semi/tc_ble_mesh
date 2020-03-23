/********************************************************************************************************
 * @file     hk_ThermostatService.h
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
#ifndef _HK_THERMOSTAT_SERVICE_H_
#define _HK_THERMOSTAT_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_THERMOSTAT_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif


#define CHAR_THERMOSTAT_SVC_SIGN_ENABLE      0
#if CHAR_THERMOSTAT_SVC_SIGN_ENABLE
    #define THERMOSTAT_AS_PRIMARY_SERVICE    	0
    #define THERMOSTAT_IS_HIDDEN_SERVICE     	0
    #define THERMOSTAT_LINKED_SERVICES_ENABLE	0
    EXTERN const hap_characteristic_desc_t hap_desc_char_thermostat_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_thermostat_A5_R_S;
    EXTERN unsigned char VAR_THERMOSTAT_UNKOWN;
#else
    #define THERMOSTAT_AS_PRIMARY_SERVICE    	0
    #define THERMOSTAT_IS_HIDDEN_SERVICE     	0
    #define THERMOSTAT_LINKED_SERVICES_ENABLE	0
#endif


#define CHAR_THERMOSTAT_COOLINGTHRESHOLDTEMPERATURE_ENABLE	0
#define CHAR_THERMOSTAT_CURRENTRELATIVEHUMIDITY_ENABLE		0
#define CHAR_THERMOSTAT_HEATINGTHRESHOLDTEMPERATURE_ENABLE	0
#define CHAR_THERMOSTAT_TARGETRELATIVEHUMIDITY_ENABLE		0


#define THERMOSTAT_CURRENTHEATINGCOOLINGSTATE_IND_ENABLE    0
#define THERMOSTAT_TARGETHEATINGCOOLINGSTATE_IND_ENABLE     0
#define THERMOSTAT_CURRENTTEMPERATURE_IND_ENABLE            0
#define THERMOSTAT_TARGETTEMPERATURE_IND_ENABLE             0
#define THERMOSTAT_TEMPERATUREDISPLAYUNITS_IND_ENABLE       0
#define THERMOSTAT_COOLINGTHRESHOLDTEMPERATURE_IND_ENABLE	0
#define THERMOSTAT_CURRENTRELATIVEHUMIDITY_IND_ENABLE		0
#define THERMOSTAT_HEATINGTHRESHOLDTEMPERATURE_IND_ENABLE	0
#define THERMOSTAT_TARGETRELATIVEHUMIDITY_IND_ENABLE		0

#define THERMOSTAT_CURRENTHEATINGCOOLINGSTATE_BRC_ENABLE    0
#define THERMOSTAT_TARGETHEATINGCOOLINGSTATE_BRC_ENABLE     0
#define THERMOSTAT_CURRENTTEMPERATURE_BRC_ENABLE            0
#define THERMOSTAT_TARGETTEMPERATURE_BRC_ENABLE             0
#define THERMOSTAT_TEMPERATUREDISPLAYUNITS_BRC_ENABLE       0
#define THERMOSTAT_COOLINGTHRESHOLDTEMPERATURE_BRC_ENABLE	0
#define THERMOSTAT_CURRENTRELATIVEHUMIDITY_BRC_ENABLE		0
#define THERMOSTAT_HEATINGTHRESHOLDTEMPERATURE_BRC_ENABLE	0
#define THERMOSTAT_TARGETRELATIVEHUMIDITY_BRC_ENABLE		0

#define THERMOSTAT_CURRENTHEATINGCOOLINGSTATE_NO 3
#if THERMOSTAT_CURRENTHEATINGCOOLINGSTATE_IND_ENABLE
    #define THERMOSTAT_CURRENTHEATINGCOOLINGSTATE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hThermostatCurrentHeatingCoolingState), ccc_hThermostatCurrentHeatingCoolingStateWriteCB, 0},
    #define THERMOSTAT_CURRENTHEATINGCOOLINGSTATE_IND_NO  1
#else
    #define THERMOSTAT_CURRENTHEATINGCOOLINGSTATE_IND_DESC
    #define THERMOSTAT_CURRENTHEATINGCOOLINGSTATE_IND_NO  0
#endif

#define THERMOSTAT_TARGETHEATINGCOOLINGSTATE_NO 3
#if THERMOSTAT_TARGETHEATINGCOOLINGSTATE_IND_ENABLE
    #define THERMOSTAT_TARGETHEATINGCOOLINGSTATE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hThermostatTargetHeatingCoolingState), ccc_hThermostatTargetHeatingCoolingStateWriteCB, 0},
    #define THERMOSTAT_TARGETHEATINGCOOLINGSTATE_IND_NO  1
#else
    #define THERMOSTAT_TARGETHEATINGCOOLINGSTATE_IND_DESC
    #define THERMOSTAT_TARGETHEATINGCOOLINGSTATE_IND_NO  0
#endif

#define THERMOSTAT_CURRENTTEMPERATURE_NO 3
#if THERMOSTAT_CURRENTTEMPERATURE_IND_ENABLE
    #define THERMOSTAT_CURRENTTEMPERATURE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hThermostatCurrentTemperature), ccc_hThermostatCurrentTemperatureWriteCB, 0},
    #define THERMOSTAT_CURRENTTEMPERATURE_IND_NO  1
#else
    #define THERMOSTAT_CURRENTTEMPERATURE_IND_DESC
    #define THERMOSTAT_CURRENTTEMPERATURE_IND_NO  0
#endif

#define THERMOSTAT_TARGETTEMPERATURE_NO 3
#if THERMOSTAT_TARGETTEMPERATURE_IND_ENABLE
    #define THERMOSTAT_TARGETTEMPERATURE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hThermostatTargetTemperature), ccc_hThermostatTargetTemperatureWriteCB, 0},
    #define THERMOSTAT_TARGETTEMPERATURE_IND_NO  1
#else
    #define THERMOSTAT_TARGETTEMPERATURE_IND_DESC
    #define THERMOSTAT_TARGETTEMPERATURE_IND_NO  0
#endif

#define THERMOSTAT_TEMPDISUNITS_NO 3
#if THERMOSTAT_TEMPERATUREDISPLAYUNITS_IND_ENABLE
    #define THERMOSTAT_TEMPERATUREDISPLAYUNITS_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hThermostatTemperatureDisplayUnits), ccc_hThermostatTemperatureDisplayUnitsWriteCB, 0},
    #define THERMOSTAT_TEMPERATUREDISPLAYUNITS_IND_NO  1
#else
    #define THERMOSTAT_TEMPERATUREDISPLAYUNITS_IND_DESC
    #define THERMOSTAT_TEMPERATUREDISPLAYUNITS_IND_NO  0
#endif


#if CHAR_THERMOSTAT_COOLINGTHRESHOLDTEMPERATURE_ENABLE
    #define THERMOSTAT_COOLINGTHRESHOLDTEMPERATURE_NO 3
    #define CHAR_THERMOSTAT_COOLINGTHRESHOLDTEMPERATURE_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hThermostatCoolingThresholdTemperature_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hThermostatCoolingThresholdTemperature),0, (u8*)(charCoolingThresholdTemperatureUUID), (u8*)&hThermostatCoolingThresholdTemperature, hThermostatCoolingThresholdTemperatureWriteCallback, hThermostatCoolingThresholdTemperatureReadCallback},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + THERMOSTAT_INST_NO + 7), 0, 0},   \
            THERMOSTAT_COOLINGTHRESHOLDTEMPERATURE_IND_DESC

	#if THERMOSTAT_COOLINGTHRESHOLDTEMPERATURE_IND_ENABLE
	    #define THERMOSTAT_COOLINGTHRESHOLDTEMPERATURE_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hThermostatCoolingThresholdTemperature), ccc_hThermostatCoolingThresholdTemperatureWriteCB, 0},
	    #define THERMOSTAT_COOLINGTHRESHOLDTEMPERATURE_IND_NO  1
	#else
	    #define THERMOSTAT_COOLINGTHRESHOLDTEMPERATURE_IND_DESC
	    #define THERMOSTAT_COOLINGTHRESHOLDTEMPERATURE_IND_NO  0
	#endif
#else
    #define THERMOSTAT_COOLINGTHRESHOLDTEMPERATURE_NO 0
    #define CHAR_THERMOSTAT_COOLINGTHRESHOLDTEMPERATURE_ATT_TABLE
    #define THERMOSTAT_COOLINGTHRESHOLDTEMPERATURE_IND_NO  0
#endif

#if CHAR_THERMOSTAT_CURRENTRELATIVEHUMIDITY_ENABLE
    #define THERMOSTAT_CURRENTRELATIVEHUMIDITY_NO 3
    #define CHAR_THERMOSTAT_CURRENTRELATIVEHUMIDITY_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hThermostatCurrentRelativeHumidity_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hThermostatCurrentRelativeHumidity),0, (u8*)(charCurrentRelativeHumidityUUID), (u8*)&hThermostatCurrentRelativeHumidity, 0, hThermostatCurrentRelativeHumidityReadCallback},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + THERMOSTAT_INST_NO + 8), 0, 0},   \
            THERMOSTAT_CURRENTRELATIVEHUMIDITY_IND_DESC

	#if THERMOSTAT_CURRENTRELATIVEHUMIDITY_IND_ENABLE
	    #define  THERMOSTAT_CURRENTRELATIVEHUMIDITY_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hThermostatCurrentRelativeHumidity), ccc_hThermostatCurrentRelativeHumidityWriteCB, 0},
	    #define  THERMOSTAT_CURRENTRELATIVEHUMIDITY_IND_NO  1
	#else
	    #define  THERMOSTAT_CURRENTRELATIVEHUMIDITY_IND_DESC
	    #define  THERMOSTAT_CURRENTRELATIVEHUMIDITY_IND_NO  0
	#endif
#else
    #define  THERMOSTAT_CURRENTRELATIVEHUMIDITY_NO 0
    #define CHAR_THERMOSTAT_CURRENTRELATIVEHUMIDITY_ATT_TABLE
    #define  THERMOSTAT_CURRENTRELATIVEHUMIDITY_IND_NO  0
#endif

#if CHAR_THERMOSTAT_HEATINGTHRESHOLDTEMPERATURE_ENABLE
    #define  THERMOSTAT_HEATINGTHRESHOLDTEMPERATURE_NO 3
    #define CHAR_THERMOSTAT_HEATINGTHRESHOLDTEMPERATURE_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hThermostatHeatingThresholdTemperature_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hThermostatHeatingThresholdTemperature),0, (u8*)(charHeatingThresholdTemperatureUUID), (u8*)&hThermostatHeatingThresholdTemperature, hThermostatHeatingThresholdTemperatureWriteCallback, hThermostatHeatingThresholdTemperatureReadCallback},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + THERMOSTAT_INST_NO + 9), 0, 0},   \
             THERMOSTAT_HEATINGTHRESHOLDTEMPERATURE_IND_DESC

	#if THERMOSTAT_HEATINGTHRESHOLDTEMPERATURE_IND_ENABLE
	    #define  THERMOSTAT_HEATINGTHRESHOLDTEMPERATURE_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hThermostatHeatingThresholdTemperature), ccc_hThermostatHeatingThresholdTemperatureWriteCB, 0},
	    #define  THERMOSTAT_HEATINGTHRESHOLDTEMPERATURE_IND_NO  1
	#else
	    #define  THERMOSTAT_HEATINGTHRESHOLDTEMPERATURE_IND_DESC
	    #define  THERMOSTAT_HEATINGTHRESHOLDTEMPERATURE_IND_NO  0
	#endif
#else
    #define  THERMOSTAT_HEATINGTHRESHOLDTEMPERATURE_NO 0
    #define CHAR_THERMOSTAT_HEATINGTHRESHOLDTEMPERATURE_ATT_TABLE
    #define  THERMOSTAT_HEATINGTHRESHOLDTEMPERATURE_IND_NO  0
#endif

#if CHAR_THERMOSTAT_TARGETRELATIVEHUMIDITY_ENABLE
    #define  THERMOSTAT_TARGETRELATIVEHUMIDITY_NO 3
    #define CHAR_THERMOSTAT_TARGETRELATIVEHUMIDITY_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hThermostatTargetRelativeHumidity_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hThermostatTargetRelativeHumidity),0, (u8*)(charTargetRelativeHumidityUUID), (u8*)&hThermostatTargetRelativeHumidity, hThermostatTargetRelativeHumidityWriteCallback, hThermostatTargetRelativeHumidityReadCallback},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + THERMOSTAT_INST_NO + 10), 0, 0},   \
             THERMOSTAT_TARGETRELATIVEHUMIDITY_IND_DESC

	#if THERMOSTAT_TARGETRELATIVEHUMIDITY_IND_ENABLE
	    #define  THERMOSTAT_TARGETRELATIVEHUMIDITY_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hThermostatTargetRelativeHumidity), ccc_hThermostatTargetRelativeHumidityWriteCB, 0},
	    #define  THERMOSTAT_TARGETRELATIVEHUMIDITY_IND_NO  1
	#else
	    #define  THERMOSTAT_TARGETRELATIVEHUMIDITY_IND_DESC
	    #define  THERMOSTAT_TARGETRELATIVEHUMIDITY_IND_NO  0
	#endif
#else
    #define  THERMOSTAT_TARGETRELATIVEHUMIDITY_NO 0
    #define CHAR_THERMOSTAT_TARGETRELATIVEHUMIDITY_ATT_TABLE
    #define  THERMOSTAT_TARGETRELATIVEHUMIDITY_IND_NO  0
#endif


#if CHAR_THERMOSTAT_SVC_SIGN_ENABLE
#define CHAR_THERMOSTAT_SVC_A5_SIGN_TABLE   \
    {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_thermostat_A5_R_S, &hap_desc_service_thermostat_A5_R_S},  \
    {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_THERMOSTAT_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_THERMOSTAT_UNKOWN, 0, 0},    \
        {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + THERMOSTAT_INST_NO + 11), 0, 0},
#define THERMOSTAT_SVC_SIGN_NO   3
#else
#define CHAR_THERMOSTAT_SVC_A5_SIGN_TABLE
#define THERMOSTAT_SVC_SIGN_NO   0
#endif

#define THERMOSTAT_NO_DELTA THERMOSTAT_CURRENTHEATINGCOOLINGSTATE_IND_NO    \
                            +THERMOSTAT_TARGETHEATINGCOOLINGSTATE_IND_NO    \
                            +THERMOSTAT_CURRENTTEMPERATURE_IND_NO   \
                            +THERMOSTAT_TARGETTEMPERATURE_IND_NO    \
                            +THERMOSTAT_TEMPERATUREDISPLAYUNITS_IND_NO	\
                            +THERMOSTAT_COOLINGTHRESHOLDTEMPERATURE_IND_NO	\
							+THERMOSTAT_CURRENTRELATIVEHUMIDITY_IND_NO	\
							+THERMOSTAT_HEATINGTHRESHOLDTEMPERATURE_IND_NO	\
							+THERMOSTAT_TARGETRELATIVEHUMIDITY_IND_NO

#define THERMOSTAT_NO   21 +THERMOSTAT_COOLINGTHRESHOLDTEMPERATURE_NO	\
						+THERMOSTAT_CURRENTRELATIVEHUMIDITY_NO	\
						+THERMOSTAT_HEATINGTHRESHOLDTEMPERATURE_NO	\
						+THERMOSTAT_TARGETRELATIVEHUMIDITY_NO	\
						+THERMOSTAT_SVC_SIGN_NO
						
#define THERMOSTAT_HANDLE_NUM (THERMOSTAT_NO + THERMOSTAT_NO_DELTA)


#define HOMEKIT_THERMOSTAT_SERVICE_ATT_TABLE    \
    {THERMOSTAT_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(thermostatServiceUUID),sizeof(thermostatServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(thermostatServiceUUID), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(insts + THERMOSTAT_INST_NO), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_NAME, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(hThermostatServiceName), 0, (u8*)(nameUUID), (u8*)(hThermostatServiceName), 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + THERMOSTAT_INST_NO + 1), 0, 0},  \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hThermostatCurrentHeatingCoolingState_R_S, 0},   \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hThermostatCurrentHeatingCoolingState),0, (u8*)(charCurrentHeatingCoolingStateUUID), (u8*)&hThermostatCurrentHeatingCoolingState, 0, hThermostatCurrentHeatingCoolingStateReadCallback},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + THERMOSTAT_INST_NO + 2), 0, 0},   \
            THERMOSTAT_CURRENTHEATINGCOOLINGSTATE_IND_DESC \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hThermostatTargetHeatingCoolingState_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hThermostatTargetHeatingCoolingState),0, (u8*)(charTargetHeatingCoolingStateUUID), (u8*)&hThermostatTargetHeatingCoolingState, hThermostatTargetHeatingCoolingStateWriteCallback, hThermostatTargetHeatingCoolingStateReadCallback},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + THERMOSTAT_INST_NO + 3), 0, 0},   \
            THERMOSTAT_TARGETHEATINGCOOLINGSTATE_IND_DESC \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hThermostatCurrentTemperature_R_S, 0},   \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hThermostatCurrentTemperature),0, (u8*)(charCurrentTemperatureUUID), (u8*)&hThermostatCurrentTemperature, 0, hThermostatCurrentTemperatureReadCallback},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + THERMOSTAT_INST_NO + 4), 0, 0},   \
            THERMOSTAT_CURRENTTEMPERATURE_IND_DESC \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hThermostatTargetTemperature_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hThermostatTargetTemperature),0, (u8*)(charTargetTemperatureUUID), (u8*)&hThermostatTargetTemperature, hThermostatTargetTemperatureWriteCallback, hThermostatTargetTemperatureReadCallback},  \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + THERMOSTAT_INST_NO + 5), 0, 0},   \
            THERMOSTAT_TARGETTEMPERATURE_IND_DESC \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hThermostatTemperatureDisplayUnits_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hThermostatTemperatureDisplayUnits),0, (u8*)(charTemperatureDisplayUnitsUUID), (u8*)&hThermostatTemperatureDisplayUnits, 0, hThermostatTemperatureDisplayUnitsReadCallback},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + THERMOSTAT_INST_NO + 6), 0, 0},   \
            THERMOSTAT_TEMPERATUREDISPLAYUNITS_IND_DESC \
            CHAR_THERMOSTAT_COOLINGTHRESHOLDTEMPERATURE_ATT_TABLE	\
            CHAR_THERMOSTAT_CURRENTRELATIVEHUMIDITY_ATT_TABLE	\
            CHAR_THERMOSTAT_HEATINGTHRESHOLDTEMPERATURE_ATT_TABLE	\
            CHAR_THERMOSTAT_TARGETRELATIVEHUMIDITY_ATT_TABLE	\
        	CHAR_THERMOSTAT_SVC_A5_SIGN_TABLE


#if 0
// 6
        {0,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), 0, 0},  \
        {0,16,4,0, (u8*)(&charCoolingThresholdTemperatureUUID), (u8*)&hThermostatCoolingThresholdTemperature, 0, 0},    \
            {0,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + 16), 0, 0},  \
            {0,2,7,7, (u8*)(&CFGUUID), (u8*)(CFG_FLOAT32_CELSIUS), 0, 0},   \
            {0,2,8,8, (u8*)(&ValidRangeUUID), (u8*)(&VALID_RANGE_hThermostatCoolingThresholdTemperature), 0, 0},    \
// 7
        {0,2,1,1, (u8*)(&characterUUID), (u8*)(&readIndicateProp), 0, 0},   \
        {0,16,4,0, (u8*)(&charCurrentRelativeHumidityUUID), (u8*)&hThermostatCurrentRelativeHumidity, 0, 0},    \
            {0,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + 16), 0, 0},  \
            {0,2,7,7, (u8*)(&CFGUUID), (u8*)(CFG_FLOAT32_PERCENTAGE), 0, 0},    \
            {0,2,8,8, (u8*)(&ValidRangeUUID), (u8*)(&VALID_RANGE_hThermostatCurrentRelativeHumidity), 0, 0},    \
// 8
        {0,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), 0, 0},  \
        {0,16,4,0, (u8*)(&charHeatingThresholdTemperatureUUID), (u8*)&hThermostatHeatingThresholdTemperature, 0, 0},    \
            {0,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + 16), 0, 0},  \
            {0,2,7,7, (u8*)(&CFGUUID), (u8*)(CFG_FLOAT32_CELSIUS), 0, 0},   \
            {0,2,8,8, (u8*)(&ValidRangeUUID), (u8*)(&VALID_RANGE_hThermostatHeatingThresholdTemperature), 0, 0},    \
//10
        {0,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), 0, 0},  \
        {0,16,4,0, (u8*)(&charTargetRelativeHumidityUUID), (u8*)&hThermostatTargetRelativeHumidity, 0, 0},  \
            {0,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + 16), 0, 0},  \
            {0,2,7,7, (u8*)(&CFGUUID), (u8*)(CFG_FLOAT32_PERCENTAGE), 0, 0},    \
            {0,2,8,8, (u8*)(&ValidRangeUUID), (u8*)(&VALID_RANGE_hThermostatTargetRelativeHumidity), 0, 0}, \

#endif


#define THERMOSTAT_HANDLE_COMMON_NUM     8

#define THERMOSTAT_HANDLE_NUM_CURRENTHEATINGCOOLINGSTATE    THERMOSTAT_HANDLE_START_NUM +THERMOSTAT_HANDLE_COMMON_NUM
#define THERMOSTAT_HANDLE_NUM_TARGETHEATINGCOOLINGSTATE     THERMOSTAT_HANDLE_NUM_CURRENTHEATINGCOOLINGSTATE +THERMOSTAT_CURRENTHEATINGCOOLINGSTATE_NO +THERMOSTAT_CURRENTHEATINGCOOLINGSTATE_IND_NO
#define THERMOSTAT_HANDLE_NUM_CURRENTTEMPERATURE            THERMOSTAT_HANDLE_NUM_TARGETHEATINGCOOLINGSTATE +THERMOSTAT_TARGETHEATINGCOOLINGSTATE_NO +THERMOSTAT_TARGETHEATINGCOOLINGSTATE_IND_NO
#define THERMOSTAT_HANDLE_NUM_TARGETTEMPERATURE             THERMOSTAT_HANDLE_NUM_CURRENTTEMPERATURE +THERMOSTAT_CURRENTTEMPERATURE_NO +THERMOSTAT_CURRENTTEMPERATURE_IND_NO
#define THERMOSTAT_HANDLE_NUM_TEMPDISUNITS                  THERMOSTAT_HANDLE_NUM_TARGETTEMPERATURE +THERMOSTAT_TARGETTEMPERATURE_NO +THERMOSTAT_TARGETTEMPERATURE_IND_NO
#define THERMOSTAT_HANDLE_NUM_COOLINGTHRESHOLDTEMPERATURE   THERMOSTAT_HANDLE_NUM_TEMPDISUNITS +THERMOSTAT_TEMPDISUNITS_NO +THERMOSTAT_TEMPERATUREDISPLAYUNITS_IND_NO
#define THERMOSTAT_HANDLE_NUM_CURRENTRELATIVEHUMIDITY       THERMOSTAT_HANDLE_NUM_COOLINGTHRESHOLDTEMPERATURE +THERMOSTAT_COOLINGTHRESHOLDTEMPERATURE_NO +THERMOSTAT_COOLINGTHRESHOLDTEMPERATURE_IND_NO
#define THERMOSTAT_HANDLE_NUM_HEATINGTHRESHOLDTEMPERATURE   THERMOSTAT_HANDLE_NUM_CURRENTRELATIVEHUMIDITY +THERMOSTAT_CURRENTRELATIVEHUMIDITY_NO +THERMOSTAT_CURRENTRELATIVEHUMIDITY_IND_NO
#define THERMOSTAT_HANDLE_NUM_TARGETRELATIVEHUMIDITY        THERMOSTAT_HANDLE_NUM_HEATINGTHRESHOLDTEMPERATURE +THERMOSTAT_HEATINGTHRESHOLDTEMPERATURE_NO +THERMOSTAT_HEATINGTHRESHOLDTEMPERATURE_IND_NO
#define THERMOSTAT_HANDLE_NUM_THE_END


#define THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATCURRENTHEATINGCOOLINGSTATE          u32 ThermostatCurrentHeatingCoolingState:2; /* 0~2 */
#define THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATCURRENTHEATINGCOOLINGSTATE_STORE        STORE_CHAR(ThermostatCurrentHeatingCoolingState,hThermostatCurrentHeatingCoolingState)
#define THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATCURRENTHEATINGCOOLINGSTATE_RESTORE      RESTORE_CHAR(ThermostatCurrentHeatingCoolingState,hThermostatCurrentHeatingCoolingState)

#define THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATTARGETHEATINGCOOLINGSTATE           u32 ThermostatTargetHeatingCoolingState:2;  /* 0~3 */
#define THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATTARGETHEATINGCOOLINGSTATE_STORE         STORE_CHAR(ThermostatTargetHeatingCoolingState,hThermostatTargetHeatingCoolingState)
#define THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATTARGETHEATINGCOOLINGSTATE_RESTORE       RESTORE_CHAR(ThermostatTargetHeatingCoolingState,hThermostatTargetHeatingCoolingState)

#define THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATCURRENTTEMPERATURE                  u32 ThermostatCurrentTemperature;    /* float */
#define THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATCURRENTTEMPERATURE_STORE                STORE_CHAR(ThermostatCurrentTemperature,hThermostatCurrentTemperature)
#define THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATCURRENTTEMPERATURE_RESTORE              RESTORE_CHAR(ThermostatCurrentTemperature,hThermostatCurrentTemperature)

#define THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATTARGETTEMPERATURE                   u32 ThermostatTargetTemperature;    /* float */
#define THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATTARGETTEMPERATURE_STORE                 STORE_CHAR(ThermostatTargetTemperature,hThermostatTargetTemperature)
#define THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATTARGETTEMPERATURE_RESTORE               RESTORE_CHAR(ThermostatTargetTemperature,hThermostatTargetTemperature)

#define THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATTEMPERATUREDISPLAYUNITS             u32 ThermostatTemperatureDisplayUnits:1;    /* 0~1 */
#define THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATTEMPERATUREDISPLAYUNITS_STORE           STORE_CHAR(ThermostatTemperatureDisplayUnits,hThermostatTemperatureDisplayUnits)
#define THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATTEMPERATUREDISPLAYUNITS_RESTORE         RESTORE_CHAR(ThermostatTemperatureDisplayUnits,hThermostatTemperatureDisplayUnits)

#define THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATCOOLINGTHRESHOLDTEMPERATURE         u32 ThermostatCoolingThresholdTemperature;    /* float */
#define THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATCOOLINGTHRESHOLDTEMPERATURE_STORE       STORE_CHAR(ThermostatCoolingThresholdTemperature,hThermostatCoolingThresholdTemperature)
#define THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATCOOLINGTHRESHOLDTEMPERATURE_RESTORE     RESTORE_CHAR(ThermostatCoolingThresholdTemperature,hThermostatCoolingThresholdTemperature)

#define THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATCURRENTRELATIVEHUMIDITY             u32 ThermostatCurrentRelativeHumidity;    /* float */
#define THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATCURRENTRELATIVEHUMIDITY_STORE           STORE_CHAR(ThermostatCurrentRelativeHumidity,hThermostatCurrentRelativeHumidity)
#define THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATCURRENTRELATIVEHUMIDITY_RESTORE         RESTORE_CHAR(ThermostatCurrentRelativeHumidity,hThermostatCurrentRelativeHumidity)

#define THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATHEATINGTHRESHOLDTEMPERATURE         u32 ThermostatHeatingThresholdTemperature;    /* float */
#define THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATHEATINGTHRESHOLDTEMPERATURE_STORE       STORE_CHAR(ThermostatHeatingThresholdTemperature,hThermostatHeatingThresholdTemperature)
#define THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATHEATINGTHRESHOLDTEMPERATURE_RESTORE     RESTORE_CHAR(ThermostatHeatingThresholdTemperature,hThermostatHeatingThresholdTemperature)

#define THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATTARGETRELATIVEHUMIDITY              u32 ThermostatTargetRelativeHumidity;    /* float */
#define THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATTARGETRELATIVEHUMIDITY_STORE            STORE_CHAR(ThermostatTargetRelativeHumidity,hThermostatTargetRelativeHumidity)
#define THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATTARGETRELATIVEHUMIDITY_RESTORE          RESTORE_CHAR(ThermostatTargetRelativeHumidity,hThermostatTargetRelativeHumidity)

#define THERMOSTAT_STORE_CHAR_VALUE_T   \
                    THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATCURRENTHEATINGCOOLINGSTATE  \
                    THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATTARGETHEATINGCOOLINGSTATE   \
                    THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATCURRENTTEMPERATURE  \
                    THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATTARGETTEMPERATURE   \
                    THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATTEMPERATUREDISPLAYUNITS  \
                    THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATCOOLINGTHRESHOLDTEMPERATURE   \
                    THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATCURRENTRELATIVEHUMIDITY  \
                    THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATHEATINGTHRESHOLDTEMPERATURE   \
                    THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATTARGETRELATIVEHUMIDITY

#define THERMOSTAT_STORE_CHAR_TO_FLASH  \
                    THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATCURRENTHEATINGCOOLINGSTATE_STORE    \
                    THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATTARGETHEATINGCOOLINGSTATE_STORE \
                    THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATCURRENTTEMPERATURE_STORE    \
                    THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATTARGETTEMPERATURE_STORE \
                    THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATTEMPERATUREDISPLAYUNITS_STORE    \
                    THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATCOOLINGTHRESHOLDTEMPERATURE_STORE \
                    THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATCURRENTRELATIVEHUMIDITY_STORE    \
                    THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATHEATINGTHRESHOLDTEMPERATURE_STORE \
                    THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATTARGETRELATIVEHUMIDITY_STORE

#define THERMOSTAT_RESTORE_CHAR_FROM_FLASH  \
                    THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATCURRENTHEATINGCOOLINGSTATE_RESTORE  \
                    THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATTARGETHEATINGCOOLINGSTATE_RESTORE   \
                    THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATCURRENTTEMPERATURE_RESTORE  \
                    THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATTARGETTEMPERATURE_RESTORE   \
                    THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATTEMPERATUREDISPLAYUNITS_RESTORE  \
                    THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATCOOLINGTHRESHOLDTEMPERATURE_RESTORE   \
                    THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATCURRENTRELATIVEHUMIDITY_RESTORE  \
                    THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATHEATINGTHRESHOLDTEMPERATURE_RESTORE   \
                    THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATTARGETRELATIVEHUMIDITY_RESTORE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN unsigned char hThermostatCurrentHeatingCoolingState; // 1
EXTERN unsigned char hThermostatTargetHeatingCoolingState;  // 2
EXTERN unsigned int hThermostatCurrentTemperature;          // 3
EXTERN unsigned int hThermostatTargetTemperature;           // 4
EXTERN unsigned char hThermostatTemperatureDisplayUnits;    // 5
EXTERN unsigned int hThermostatCoolingThresholdTemperature; // 6
EXTERN unsigned int hThermostatCurrentRelativeHumidity;     // 7
EXTERN unsigned int hThermostatHeatingThresholdTemperature; // 8
EXTERN unsigned int hThermostatTargetRelativeHumidity;      // 10

EXTERN unsigned short ccc_hThermostatCurrentHeatingCoolingState;
EXTERN unsigned short ccc_hThermostatTargetHeatingCoolingState;
EXTERN unsigned short ccc_hThermostatCurrentTemperature;
EXTERN unsigned short ccc_hThermostatTargetTemperature;
EXTERN unsigned short ccc_hThermostatTemperatureDisplayUnits;
EXTERN unsigned short ccc_hThermostatCoolingThresholdTemperature;
EXTERN unsigned short ccc_hThermostatCurrentRelativeHumidity;
EXTERN unsigned short ccc_hThermostatHeatingThresholdTemperature;
EXTERN unsigned short ccc_hThermostatTargetRelativeHumidity;

#if THERMOSTAT_CURRENTHEATINGCOOLINGSTATE_BRC_ENABLE
    #define BC_PARA_THERMOSTAT_CURRENTHEATINGCOOLINGSTATE ((u8*)&bc_para_hThermostatCurrentHeatingCoolingState)
    EXTERN unsigned char bc_para_hThermostatCurrentHeatingCoolingState;
#else
    #define BC_PARA_THERMOSTAT_CURRENTHEATINGCOOLINGSTATE 0
#endif

#if THERMOSTAT_TARGETHEATINGCOOLINGSTATE_BRC_ENABLE
    #define BC_PARA_THERMOSTAT_TARGETHEATINGCOOLINGSTATE ((u8*)&bc_para_hThermostatTargetHeatingCoolingState)
    EXTERN unsigned char bc_para_hThermostatTargetHeatingCoolingState;
#else
    #define BC_PARA_THERMOSTAT_TARGETHEATINGCOOLINGSTATE 0
#endif

#if THERMOSTAT_CURRENTTEMPERATURE_BRC_ENABLE
    #define BC_PARA_THERMOSTAT_CURRENTTEMPERATURE ((u8*)&bc_para_hThermostatCurrentTemperature)
    EXTERN unsigned char bc_para_hThermostatCurrentTemperature;
#else
    #define BC_PARA_THERMOSTAT_CURRENTTEMPERATURE 0
#endif

#if THERMOSTAT_TARGETTEMPERATURE_BRC_ENABLE
    #define BC_PARA_THERMOSTAT_TARGETTEMPERATURE ((u8*)&bc_para_hThermostatTargetTemperature)
    EXTERN unsigned char bc_para_hThermostatTargetTemperature;
#else
    #define BC_PARA_THERMOSTAT_TARGETTEMPERATURE 0
#endif

#if THERMOSTAT_TEMPERATUREDISPLAYUNITS_BRC_ENABLE
    #define BC_PARA_THERMOSTAT_TEMPERATUREDISPLAYUNITS ((u8*)&bc_para_hThermostatTemperatureDisplayUnits)
    EXTERN unsigned char bc_para_hThermostatTemperatureDisplayUnits;
#else
    #define BC_PARA_THERMOSTAT_TEMPERATUREDISPLAYUNITS 0
#endif

#if THERMOSTAT_COOLINGTHRESHOLDTEMPERATURE_BRC_ENABLE
    #define BC_PARA_THERMOSTAT_COOLINGTHRESHOLDTEMPERATURE ((u8*)&bc_para_hThermostatCoolingThresholdTemperature)
    EXTERN unsigned char bc_para_hThermostatCoolingThresholdTemperature;
#else
    #define BC_PARA_THERMOSTAT_COOLINGTHRESHOLDTEMPERATURE 0
#endif

#if THERMOSTAT_CURRENTRELATIVEHUMIDITY_BRC_ENABLE
    #define BC_PARA_THERMOSTAT_CURRENTRELATIVEHUMIDITY ((u8*)&bc_para_hThermostatCurrentRelativeHumidity)
    EXTERN unsigned char bc_para_hThermostatCurrentRelativeHumidity;
#else
    #define BC_PARA_THERMOSTAT_CURRENTRELATIVEHUMIDITY 0
#endif

#if THERMOSTAT_HEATINGTHRESHOLDTEMPERATURE_BRC_ENABLE
    #define BC_PARA_THERMOSTAT_HEATINGTHRESHOLDTEMPERATURE ((u8*)&bc_para_hThermostatHeatingThresholdTemperature)
    EXTERN unsigned char bc_para_hThermostatHeatingThresholdTemperature;
#else
    #define BC_PARA_THERMOSTAT_HEATINGTHRESHOLDTEMPERATURE 0
#endif

#if THERMOSTAT_TARGETRELATIVEHUMIDITY_BRC_ENABLE
    #define BC_PARA_THERMOSTAT_TARGETRELATIVEHUMIDITY ((u8*)&bc_para_hThermostatTargetRelativeHumidity)
    EXTERN unsigned char bc_para_hThermostatTargetRelativeHumidity;
#else
    #define BC_PARA_THERMOSTAT_TARGETRELATIVEHUMIDITY 0
#endif

#if THERMOSTAT_LINKED_SERVICES_ENABLE
	#define THERMOSTAT_LINKED_SVCS			hThermostat_linked_svcs
	#define THERMOSTAT_LINKED_SVCS_LENGTH 	(sizeof(hThermostat_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * hThermostat_linked_svcs[];
#else
	#define THERMOSTAT_LINKED_SVCS			0
	#define THERMOSTAT_LINKED_SVCS_LENGTH 	0
#endif

EXTERN const u8 hThermostatServiceName[17];

EXTERN const hap_characteristic_desc_t hap_desc_char_hThermostatCurrentHeatingCoolingState_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hThermostatTargetHeatingCoolingState_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hThermostatCurrentTemperature_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hThermostatTargetTemperature_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hThermostatTemperatureDisplayUnits_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hThermostatCoolingThresholdTemperature_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hThermostatCurrentRelativeHumidity_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hThermostatHeatingThresholdTemperature_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hThermostatTargetRelativeHumidity_R_S;

/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN int hThermostatCurrentHeatingCoolingStateReadCallback(void *pp);
EXTERN int hThermostatTargetHeatingCoolingStateReadCallback(void *pp);
EXTERN int hThermostatTargetHeatingCoolingStateWriteCallback(void *pp);
EXTERN int hThermostatCurrentTemperatureReadCallback(void *pp);
EXTERN int hThermostatTargetTemperatureReadCallback(void *pp);
EXTERN int hThermostatTargetTemperatureWriteCallback(void *pp);
EXTERN int hThermostatTemperatureDisplayUnitsReadCallback(void *pp);
EXTERN int hThermostatCoolingThresholdTemperatureReadCallback(void *pp);
EXTERN int hThermostatCoolingThresholdTemperatureWriteCallback(void *pp);
EXTERN int hThermostatCurrentRelativeHumidityReadCallback(void *pp);
EXTERN int hThermostatHeatingThresholdTemperatureReadCallback(void *pp);
EXTERN int hThermostatHeatingThresholdTemperatureWriteCallback(void *pp);
EXTERN int hThermostatTargetRelativeHumidityReadCallback(void *pp);
EXTERN int hThermostatTargetRelativeHumidityWriteCallback(void *pp);

EXTERN int ccc_hThermostatCurrentHeatingCoolingStateWriteCB(void *pp);
EXTERN int ccc_hThermostatTargetHeatingCoolingStateWriteCB(void *pp);
EXTERN int ccc_hThermostatCurrentTemperatureWriteCB(void *pp);
EXTERN int ccc_hThermostatTargetTemperatureWriteCB(void *pp);
EXTERN int ccc_hThermostatTemperatureDisplayUnitsWriteCB(void *pp);
EXTERN int ccc_hThermostatCoolingThresholdTemperatureWriteCB(void *pp);
EXTERN int ccc_hThermostatCurrentRelativeHumidityWriteCB(void *pp);
EXTERN int ccc_hThermostatHeatingThresholdTemperatureWriteCB(void *pp);
EXTERN int ccc_hThermostatTargetRelativeHumidityWriteCB(void *pp);

#endif /* _HK_THERMOSTAT_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
