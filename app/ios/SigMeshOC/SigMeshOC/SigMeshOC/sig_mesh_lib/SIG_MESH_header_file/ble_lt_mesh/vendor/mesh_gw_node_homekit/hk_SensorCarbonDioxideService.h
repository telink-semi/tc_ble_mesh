/********************************************************************************************************
 * @file     hk_SensorCarbonDioxideService.h
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
#ifndef _HK_SENSOR_CARBON_DIOXIDE_SERVICE_H_
#define _HK_SENSOR_CARBON_DIOXIDE_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */
//-#include "../../homekit_src/homekit_inc.h"


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_SENSOR_CARBON_DIOXIDE_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif


#define CHAR_SENSOR_CARBON_DIOXIDE_SVC_SIGN_ENABLE      0
#if CHAR_SENSOR_CARBON_DIOXIDE_SVC_SIGN_ENABLE
    #define SENSOR_CARBON_DIOXIDE_AS_PRIMARY_SERVICE    	0
    #define SENSOR_CARBON_DIOXIDE_IS_HIDDEN_SERVICE     	0
    #define SENSOR_CARBON_DIOXIDE_LINKED_SERVICES_ENABLE	0
    EXTERN const hap_characteristic_desc_t hap_desc_char_sensor_carbon_dioxide_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_sensor_carbon_dioxide_A5_R_S;
    EXTERN unsigned char VAR_SENSOR_CARBON_DIOXIDE_UNKOWN;
#else
    #define SENSOR_CARBON_DIOXIDE_AS_PRIMARY_SERVICE    	0
    #define SENSOR_CARBON_DIOXIDE_IS_HIDDEN_SERVICE     	0
    #define SENSOR_CARBON_DIOXIDE_LINKED_SERVICES_ENABLE	0
#endif

#define CHAR_SENSORCARBONDIOXIDE_STATUSACTIVE_ENABLE         0
#define CHAR_SENSORCARBONDIOXIDE_STATUSFAULT_ENABLE          0
#define CHAR_SENSORCARBONDIOXIDE_STATUSTAMPERED_ENABLE       0
#define CHAR_SENSORCARBONDIOXIDE_STATUSLOWBATTERY_ENABLE     0
#define CHAR_SENSORCARBONDIOXIDE_LEVEL_ENABLE                0
#define CHAR_SENSORCARBONDIOXIDE_PEAKLEVEL_ENABLE            0

#define SENSORCARBONDIOXIDE_DETECTED_IND_ENABLE             0
#define SENSORCARBONDIOXIDE_STATUSACTIVE_IND_ENABLE         0
#define SENSORCARBONDIOXIDE_STATUSFAULT_IND_ENABLE          0
#define SENSORCARBONDIOXIDE_STATUSTAMPERED_IND_ENABLE       0
#define SENSORCARBONDIOXIDE_STATUSLOWBATTERY_IND_ENABLE     0
#define SENSORCARBONDIOXIDE_LEVEL_IND_ENABLE                0
#define SENSORCARBONDIOXIDE_PEAKLEVEL_IND_ENABLE            0

#define SENSORCARBONDIOXIDE_DETECTED_BRC_ENABLE             0
#define SENSORCARBONDIOXIDE_STATUSACTIVE_BRC_ENABLE         0
#define SENSORCARBONDIOXIDE_STATUSFAULT_BRC_ENABLE          0
#define SENSORCARBONDIOXIDE_STATUSTAMPERED_BRC_ENABLE       0
#define SENSORCARBONDIOXIDE_STATUSLOWBATTERY_BRC_ENABLE     0
#define SENSORCARBONDIOXIDE_LEVEL_BRC_ENABLE                0
#define SENSORCARBONDIOXIDE_PEAKLEVEL_BRC_ENABLE            0


#define SENSORCARBONDIOXIDE_DETECTED_NO 3
#if SENSORCARBONDIOXIDE_DETECTED_IND_ENABLE
    #define SENSORCARBONDIOXIDE_DETECTED_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorCarbonDioxide_Detected), ccc_hSensorCarbonDioxide_DetectedWriteCB, 0},
    #define SENSORCARBONDIOXIDE_DETECTED_IND_NO  1
#else
    #define SENSORCARBONDIOXIDE_DETECTED_IND_DESC
    #define SENSORCARBONDIOXIDE_DETECTED_IND_NO  0
#endif


#if CHAR_SENSORCARBONDIOXIDE_STATUSACTIVE_ENABLE
    #define SENSORCARBONDIOXIDE_STATUSACTIVE_NO 3
    #define CHAR_SENSORCARBONDIOXIDE_STATUSACTIVE_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Carbon_Dioxide_StatusActive_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Carbon_Dioxide_StatusActive),0, (u8*)(charStatusActiveUUID), (u8*)&hSensor_Carbon_Dioxide_StatusActive, 0, hSensor_Carbon_Dioxide_StatusActiveReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_CARBON_DIOXIDE_INST_NO + 3), 0, 0},    \
            SENSORCARBONDIOXIDE_STATUSACTIVE_IND_DESC

	#if SENSORCARBONDIOXIDE_STATUSACTIVE_IND_ENABLE
	    #define SENSORCARBONDIOXIDE_STATUSACTIVE_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorCarbonDioxide_StatusActive), ccc_hSensorCarbonDioxide_StatusActiveWriteCB, 0},
	    #define SENSORCARBONDIOXIDE_STATUSACTIVE_IND_NO  1
	#else
	    #define SENSORCARBONDIOXIDE_STATUSACTIVE_IND_DESC
	    #define SENSORCARBONDIOXIDE_STATUSACTIVE_IND_NO  0
	#endif
#else
    #define SENSORCARBONDIOXIDE_STATUSACTIVE_NO 0
    #define CHAR_SENSORCARBONDIOXIDE_STATUSACTIVE_ATT_TABLE
    #define SENSORCARBONDIOXIDE_STATUSACTIVE_IND_NO  0
#endif

#if CHAR_SENSORCARBONDIOXIDE_STATUSFAULT_ENABLE
    #define SENSORCARBONDIOXIDE_STATUSFAULT_NO 3
    #define CHAR_SENSORCARBONDIOXIDE_STATUSFAULT_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Carbon_Dioxide_StatusFault_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Carbon_Dioxide_StatusFault),0, (u8*)(charStatusFaultUUID), (u8*)&hSensor_Carbon_Dioxide_StatusFault, 0, hSensor_Carbon_Dioxide_StatusFaultReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_CARBON_DIOXIDE_INST_NO + 4), 0, 0},    \
            SENSORCARBONDIOXIDE_STATUSFAULT_IND_DESC

	#if SENSORCARBONDIOXIDE_STATUSFAULT_IND_ENABLE
	    #define SENSORCARBONDIOXIDE_STATUSFAULT_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorCarbonDioxide_StatusFault), ccc_hSensorCarbonDioxide_StatusFaultWriteCB, 0},
	    #define SENSORCARBONDIOXIDE_STATUSFAULT_IND_NO  1
	#else
	    #define SENSORCARBONDIOXIDE_STATUSFAULT_IND_DESC
	    #define SENSORCARBONDIOXIDE_STATUSFAULT_IND_NO  0
	#endif
#else
    #define SENSORCARBONDIOXIDE_STATUSFAULT_NO 0
    #define CHAR_SENSORCARBONDIOXIDE_STATUSFAULT_ATT_TABLE
    #define SENSORCARBONDIOXIDE_STATUSFAULT_IND_NO  0
#endif


#if CHAR_SENSORCARBONDIOXIDE_STATUSTAMPERED_ENABLE
    #define SENSORCARBONDIOXIDE_STATUSTAMPERED_NO 3
    #define CHAR_SENSORCARBONDIOXIDE_STATUSTAMPERED_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Carbon_Dioxide_StatusTampered_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Carbon_Dioxide_StatusTampered),0, (u8*)(charStatusTamperedUUID), (u8*)&hSensor_Carbon_Dioxide_StatusTampered, 0, hSensor_Carbon_Dioxide_StatusTamperedReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_CARBON_DIOXIDE_INST_NO + 5), 0, 0},    \
            SENSORCARBONDIOXIDE_STATUSTAMPERED_IND_DESC

	#if SENSORCARBONDIOXIDE_STATUSTAMPERED_IND_ENABLE
	    #define SENSORCARBONDIOXIDE_STATUSTAMPERED_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorCarbonDioxide_StatusTampered), ccc_hSensorCarbonDioxide_StatusTamperedWriteCB, 0},
	    #define SENSORCARBONDIOXIDE_STATUSTAMPERED_IND_NO  1
	#else
	    #define SENSORCARBONDIOXIDE_STATUSTAMPERED_IND_DESC
	    #define SENSORCARBONDIOXIDE_STATUSTAMPERED_IND_NO  0
	#endif
#else
    #define SENSORCARBONDIOXIDE_STATUSTAMPERED_NO 0
    #define CHAR_SENSORCARBONDIOXIDE_STATUSTAMPERED_ATT_TABLE
    #define SENSORCARBONDIOXIDE_STATUSTAMPERED_IND_NO  0
#endif

#if CHAR_SENSORCARBONDIOXIDE_STATUSLOWBATTERY_ENABLE
    #define SENSORCARBONDIOXIDE_STATUSLOWBATTERY_NO 3
    #define CHAR_SENSORCARBONDIOXIDE_STATUSLOWBATTERY_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Carbon_Dioxide_StatusLowBattery_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Carbon_Dioxide_StatusLowBattery),0, (u8*)(charStatusLowBatteryUUID), (u8*)&hSensor_Carbon_Dioxide_StatusLowBattery, 0, hSensor_Carbon_Dioxide_StatusLowBatteryReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_CARBON_DIOXIDE_INST_NO + 6), 0, 0},    \
            SENSORCARBONDIOXIDE_STATUSLOWBATTERY_IND_DESC

	#if SENSORCARBONDIOXIDE_STATUSLOWBATTERY_IND_ENABLE
	    #define SENSORCARBONDIOXIDE_STATUSLOWBATTERY_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorCarbonDioxide_StatusLowBattery), ccc_hSensorCarbonDioxide_StatusLowBatteryWriteCB, 0},
	    #define SENSORCARBONDIOXIDE_STATUSLOWBATTERY_IND_NO  1
	#else
	    #define SENSORCARBONDIOXIDE_STATUSLOWBATTERY_IND_DESC
	    #define SENSORCARBONDIOXIDE_STATUSLOWBATTERY_IND_NO  0
	#endif
#else
    #define SENSORCARBONDIOXIDE_STATUSLOWBATTERY_NO 0
    #define CHAR_SENSORCARBONDIOXIDE_STATUSLOWBATTERY_ATT_TABLE
    #define SENSORCARBONDIOXIDE_STATUSLOWBATTERY_IND_NO  0
#endif

#if CHAR_SENSORCARBONDIOXIDE_LEVEL_ENABLE
    #define SENSORCARBONDIOXIDE_LEVEL_NO 3
    #define CHAR_SENSORCARBONDIOXIDE_LEVEL_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Carbon_Dioxide_Level_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Carbon_Dioxide_Level),0, (u8*)(charCarbonDioxideLevelUUID), (u8*)&hSensor_Carbon_Dioxide_Level, 0, hSensor_Carbon_Dioxide_LevelReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_CARBON_DIOXIDE_INST_NO + 7), 0, 0},    \
            SENSORCARBONDIOXIDE_LEVEL_IND_DESC

	#if SENSORCARBONDIOXIDE_LEVEL_IND_ENABLE
	    #define SENSORCARBONDIOXIDE_LEVEL_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorCarbonDioxide_Level), ccc_hSensorCarbonDioxide_LevelWriteCB, 0},
	    #define SENSORCARBONDIOXIDE_LEVEL_IND_NO  1
	#else
	    #define SENSORCARBONDIOXIDE_LEVEL_IND_DESC
	    #define SENSORCARBONDIOXIDE_LEVEL_IND_NO  0
	#endif
#else
    #define SENSORCARBONDIOXIDE_LEVEL_NO 0
    #define CHAR_SENSORCARBONDIOXIDE_LEVEL_ATT_TABLE
    #define SENSORCARBONDIOXIDE_LEVEL_IND_NO  0
#endif

#if CHAR_SENSORCARBONDIOXIDE_PEAKLEVEL_ENABLE
    #define SENSORCARBONDIOXIDE_PEAKLEVEL_NO 3
    #define CHAR_SENSORCARBONDIOXIDE_PEAKLEVEL_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Carbon_Dioxide_PeakLevel_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Carbon_Dioxide_PeakLevel),0, (u8*)(charCarbonDioxidePeakLevelUUID), (u8*)&hSensor_Carbon_Dioxide_PeakLevel, 0, hSensor_Carbon_Dioxide_PeakLevelReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_CARBON_DIOXIDE_INST_NO + 8), 0, 0},    \
            SENSORCARBONDIOXIDE_PEAKLEVEL_IND_DESC

	#if SENSORCARBONDIOXIDE_PEAKLEVEL_IND_ENABLE
	    #define SENSORCARBONDIOXIDE_PEAKLEVEL_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorCarbonDioxide_PeakLevel), ccc_hSensorCarbonDioxide_PeakLevelWriteCB, 0},
	    #define SENSORCARBONDIOXIDE_PEAKLEVEL_IND_NO  1
	#else
	    #define SENSORCARBONDIOXIDE_PEAKLEVEL_IND_DESC
	    #define SENSORCARBONDIOXIDE_PEAKLEVEL_IND_NO  0
	#endif
#else
    #define SENSORCARBONDIOXIDE_PEAKLEVEL_NO 0
    #define CHAR_SENSORCARBONDIOXIDE_PEAKLEVEL_ATT_TABLE
    #define SENSORCARBONDIOXIDE_PEAKLEVEL_IND_NO  0
#endif


#if CHAR_SENSOR_CARBON_DIOXIDE_SVC_SIGN_ENABLE
#define CHAR_SENSOR_CARBON_DIOXIDE_SVC_A5_SIGN_TABLE   \
    {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_sensor_carbon_dioxide_A5_R_S, &hap_desc_service_sensor_carbon_dioxide_A5_R_S},  \
    {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_SENSOR_CARBON_DIOXIDE_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_SENSOR_CARBON_DIOXIDE_UNKOWN, 0, 0},    \
        {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_CARBON_DIOXIDE_INST_NO + 9), 0, 0},
#define SENSOR_CARBON_DIOXIDE_SVC_SIGN_NO   3
#else
#define CHAR_SENSOR_CARBON_DIOXIDE_SVC_A5_SIGN_TABLE
#define SENSOR_CARBON_DIOXIDE_SVC_SIGN_NO   0
#endif

#define SENSOR_CARBON_DIOXIDE_NO_DELTA  SENSORCARBONDIOXIDE_DETECTED_IND_NO  \
                                        +SENSORCARBONDIOXIDE_STATUSACTIVE_IND_NO    \
                                        +SENSORCARBONDIOXIDE_STATUSFAULT_IND_NO \
                                        +SENSORCARBONDIOXIDE_STATUSTAMPERED_IND_NO \
                                        +SENSORCARBONDIOXIDE_STATUSLOWBATTERY_IND_NO    \
                                        +SENSORCARBONDIOXIDE_LEVEL_IND_NO   \
                                        +SENSORCARBONDIOXIDE_PEAKLEVEL_IND_NO

#define SENSOR_CARBON_DIOXIDE_NO        9 +SENSORCARBONDIOXIDE_STATUSACTIVE_NO    \
                                        +SENSORCARBONDIOXIDE_STATUSFAULT_NO \
                                        +SENSORCARBONDIOXIDE_STATUSTAMPERED_NO \
                                        +SENSORCARBONDIOXIDE_STATUSLOWBATTERY_NO    \
                                        +SENSORCARBONDIOXIDE_LEVEL_NO   \
                                        +SENSORCARBONDIOXIDE_PEAKLEVEL_NO \
										+SENSOR_CARBON_DIOXIDE_SVC_SIGN_NO
										
#define SENSOR_CARBON_DIOXIDE_HANDLE_NUM (SENSOR_CARBON_DIOXIDE_NO + SENSOR_CARBON_DIOXIDE_NO_DELTA)


#define HOMEKIT_SENSOR_CARBON_DIOXIDE_SERVICE_ATT_TABLE    \
    {SENSOR_CARBON_DIOXIDE_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(sensor_carbon_dioxideServiceUUID),sizeof(sensor_carbon_dioxideServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(sensor_carbon_dioxideServiceUUID), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(insts + SENSOR_CARBON_DIOXIDE_INST_NO), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_NAME, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(hSensor_Carbon_DioxideServiceName), 0, (u8*)(nameUUID), (u8*)(hSensor_Carbon_DioxideServiceName), 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + SENSOR_CARBON_DIOXIDE_INST_NO + 1), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Carbon_Dioxide_Detected_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Carbon_Dioxide_Detected),0, (u8*)(charCarbonDioxideDetectedUUID), (u8*)&hSensor_Carbon_Dioxide_Detected, 0, hSensor_Carbon_Dioxide_DetectedReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_CARBON_DIOXIDE_INST_NO + 2), 0, 0},    \
            SENSORCARBONDIOXIDE_DETECTED_IND_DESC \
            CHAR_SENSORCARBONDIOXIDE_STATUSACTIVE_ATT_TABLE \
            CHAR_SENSORCARBONDIOXIDE_STATUSFAULT_ATT_TABLE \
            CHAR_SENSORCARBONDIOXIDE_STATUSTAMPERED_ATT_TABLE \
            CHAR_SENSORCARBONDIOXIDE_STATUSLOWBATTERY_ATT_TABLE \
            CHAR_SENSORCARBONDIOXIDE_LEVEL_ATT_TABLE \
            CHAR_SENSORCARBONDIOXIDE_PEAKLEVEL_ATT_TABLE \
        	CHAR_SENSOR_CARBON_DIOXIDE_SVC_A5_SIGN_TABLE


#define SENSORCARBONDIOXIDE_HANDLE_COMMON_NUM     8

#define SENSORCARBONDIOXIDE_HANDLE_NUM_DETECTED         SENSOR_CARBON_DIOXIDE_HANDLE_START_NUM +SENSORCARBONDIOXIDE_HANDLE_COMMON_NUM
#define SENSORCARBONDIOXIDE_HANDLE_NUM_STATUSACTIVE     SENSORCARBONDIOXIDE_HANDLE_NUM_DETECTED +SENSORCARBONDIOXIDE_DETECTED_NO +SENSORCARBONDIOXIDE_DETECTED_IND_NO
#define SENSORCARBONDIOXIDE_HANDLE_NUM_STATUSFAULT      SENSORCARBONDIOXIDE_HANDLE_NUM_STATUSACTIVE +SENSORCARBONDIOXIDE_STATUSACTIVE_NO +SENSORCARBONDIOXIDE_STATUSACTIVE_IND_NO
#define SENSORCARBONDIOXIDE_HANDLE_NUM_STATUSTAMPERED   SENSORCARBONDIOXIDE_HANDLE_NUM_STATUSFAULT +SENSORCARBONDIOXIDE_STATUSFAULT_NO +SENSORCARBONDIOXIDE_STATUSFAULT_IND_NO
#define SENSORCARBONDIOXIDE_HANDLE_NUM_STATUSLOWBATTERY SENSORCARBONDIOXIDE_HANDLE_NUM_STATUSTAMPERED +SENSORCARBONDIOXIDE_STATUSTAMPERED_NO +SENSORCARBONDIOXIDE_STATUSTAMPERED_IND_NO
#define SENSORCARBONDIOXIDE_HANDLE_NUM_LEVEL            SENSORCARBONDIOXIDE_HANDLE_NUM_STATUSLOWBATTERY +SENSORCARBONDIOXIDE_STATUSLOWBATTERY_NO +SENSORCARBONDIOXIDE_STATUSLOWBATTERY_IND_NO
#define SENSORCARBONDIOXIDE_HANDLE_NUM_PEAKLEVEL        SENSORCARBONDIOXIDE_HANDLE_NUM_LEVEL +SENSORCARBONDIOXIDE_LEVEL_NO +SENSORCARBONDIOXIDE_LEVEL_IND_NO
#define SENSORCARBONDIOXIDE_HANDLE_NUM_THE_END


#define SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_DETECTED                  u32 Sensor_Carbon_Dioxide_Detected:1;  /* 0~1 */
#define SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_DETECTED_STORE                STORE_CHAR(Sensor_Carbon_Dioxide_Detected,hSensor_Carbon_Dioxide_Detected)
#define SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_DETECTED_RESTORE              RESTORE_CHAR(Sensor_Carbon_Dioxide_Detected,hSensor_Carbon_Dioxide_Detected)

#define SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_STATUSACTIVE              u32 Sensor_Carbon_Dioxide_StatusActive:1;  /* 0~1 */
#define SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_STATUSACTIVE_STORE            STORE_CHAR(Sensor_Carbon_Dioxide_StatusActive,hSensor_Carbon_Dioxide_StatusActive)
#define SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_STATUSACTIVE_RESTORE          RESTORE_CHAR(Sensor_Carbon_Dioxide_StatusActive,hSensor_Carbon_Dioxide_StatusActive)

#define SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_STATUSFAULT               u32 Sensor_Carbon_Dioxide_StatusFault:1;  /* 0~1 */
#define SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_STATUSFAULT_STORE             STORE_CHAR(Sensor_Carbon_Dioxide_StatusFault,hSensor_Carbon_Dioxide_StatusFault)
#define SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_STATUSFAULT_RESTORE           RESTORE_CHAR(Sensor_Carbon_Dioxide_StatusFault,hSensor_Carbon_Dioxide_StatusFault)

#define SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_STATUSTAMPERED            u32 Sensor_Carbon_Dioxide_StatusTampered:1;  /* 0~1 */
#define SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_STATUSTAMPERED_STORE          STORE_CHAR(Sensor_Carbon_Dioxide_StatusTampered,hSensor_Carbon_Dioxide_StatusTampered)
#define SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_STATUSTAMPERED_RESTORE        RESTORE_CHAR(Sensor_Carbon_Dioxide_StatusTampered,hSensor_Carbon_Dioxide_StatusTampered)

#define SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_STATUSLOWBATTERY          u32 Sensor_Carbon_Dioxide_StatusLowBattery:1;  /* 0~1 */
#define SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_STATUSLOWBATTERY_STORE        STORE_CHAR(Sensor_Carbon_Dioxide_StatusLowBattery,hSensor_Carbon_Dioxide_StatusLowBattery)
#define SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_STATUSLOWBATTERY_RESTORE      RESTORE_CHAR(Sensor_Carbon_Dioxide_StatusLowBattery,hSensor_Carbon_Dioxide_StatusLowBattery)

#define SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_LEVEL                     u32 Sensor_Carbon_Dioxide_Level;    /* float */
#define SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_LEVEL_STORE                   STORE_CHAR(Sensor_Carbon_Dioxide_Level,hSensor_Carbon_Dioxide_Level)
#define SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_LEVEL_RESTORE                 RESTORE_CHAR(Sensor_Carbon_Dioxide_Level,hSensor_Carbon_Dioxide_Level)

#define SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_PEAKLEVEL                 u32 Sensor_Carbon_Dioxide_PeakLevel;    /* float */
#define SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_PEAKLEVEL_STORE               STORE_CHAR(Sensor_Carbon_Dioxide_PeakLevel,hSensor_Carbon_Dioxide_PeakLevel)
#define SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_PEAKLEVEL_RESTORE             RESTORE_CHAR(Sensor_Carbon_Dioxide_PeakLevel,hSensor_Carbon_Dioxide_PeakLevel)

#define SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T   \
                    SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_DETECTED  \
                    SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_STATUSACTIVE   \
                    SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_STATUSFAULT  \
                    SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_STATUSTAMPERED   \
                    SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_STATUSLOWBATTERY  \
                    SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_LEVEL   \
                    SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_PEAKLEVEL

#define SENSOR_CARBON_DIOXIDE_STORE_CHAR_TO_FLASH  \
                    SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_DETECTED_STORE    \
                    SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_STATUSACTIVE_STORE \
                    SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_STATUSFAULT_STORE    \
                    SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_STATUSTAMPERED_STORE \
                    SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_STATUSLOWBATTERY_STORE    \
                    SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_LEVEL_STORE \
                    SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_PEAKLEVEL_STORE

#define SENSOR_CARBON_DIOXIDE_RESTORE_CHAR_FROM_FLASH  \
                    SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_DETECTED_RESTORE  \
                    SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_STATUSACTIVE_RESTORE   \
                    SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_STATUSFAULT_RESTORE  \
                    SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_STATUSTAMPERED_RESTORE   \
                    SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_STATUSLOWBATTERY_RESTORE  \
                    SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_LEVEL_RESTORE   \
                    SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_PEAKLEVEL_RESTORE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN unsigned char hSensor_Carbon_Dioxide_Detected;
EXTERN unsigned char hSensor_Carbon_Dioxide_StatusActive;
EXTERN unsigned char hSensor_Carbon_Dioxide_StatusFault;
EXTERN unsigned char hSensor_Carbon_Dioxide_StatusTampered;
EXTERN unsigned char hSensor_Carbon_Dioxide_StatusLowBattery;
EXTERN unsigned int  hSensor_Carbon_Dioxide_Level;
EXTERN unsigned int  hSensor_Carbon_Dioxide_PeakLevel;

EXTERN unsigned short ccc_hSensorCarbonDioxide_Detected;
EXTERN unsigned short ccc_hSensorCarbonDioxide_StatusActive;
EXTERN unsigned short ccc_hSensorCarbonDioxide_StatusFault;
EXTERN unsigned short ccc_hSensorCarbonDioxide_StatusTampered;
EXTERN unsigned short ccc_hSensorCarbonDioxide_StatusLowBattery;
EXTERN unsigned short ccc_hSensorCarbonDioxide_Level;
EXTERN unsigned short ccc_hSensorCarbonDioxide_PeakLevel;

#if SENSORCARBONDIOXIDE_DETECTED_BRC_ENABLE
    #define BC_PARA_SENSORCARBONDIOXIDE_DETECTED ((u8*)&bc_para_hSensorCarbonDioxide_Detected)
    EXTERN unsigned char bc_para_hSensorCarbonDioxide_Detected;
#else
    #define BC_PARA_SENSORCARBONDIOXIDE_DETECTED 0
#endif

#if SENSORCARBONDIOXIDE_STATUSACTIVE_BRC_ENABLE
    #define BC_PARA_SENSORCARBONDIOXIDE_STATUSACTIVE ((u8*)&bc_para_hSensorCarbonDioxide_StatusActive)
    EXTERN unsigned char bc_para_hSensorCarbonDioxide_StatusActive;
#else
    #define BC_PARA_SENSORCARBONDIOXIDE_STATUSACTIVE 0
#endif

#if SENSORCARBONDIOXIDE_STATUSFAULT_BRC_ENABLE
    #define BC_PARA_SENSORCARBONDIOXIDE_STATUSFAULT ((u8*)&bc_para_hSensorCarbonDioxide_StatusFault)
    EXTERN unsigned char bc_para_hSensorCarbonDioxide_StatusFault;
#else
    #define BC_PARA_SENSORCARBONDIOXIDE_STATUSFAULT 0
#endif

#if SENSORCARBONDIOXIDE_STATUSTAMPERED_BRC_ENABLE
    #define BC_PARA_SENSORCARBONDIOXIDE_STATUSTAMPERED ((u8*)&bc_para_hSensorCarbonDioxide_StatusTampered)
    EXTERN unsigned char bc_para_hSensorCarbonDioxide_StatusTampered;
#else
    #define BC_PARA_SENSORCARBONDIOXIDE_STATUSTAMPERED 0
#endif

#if SENSORCARBONDIOXIDE_STATUSLOWBATTERY_BRC_ENABLE
    #define BC_PARA_SENSORCARBONDIOXIDE_STATUSLOWBATTERY ((u8*)&bc_para_hSensorCarbonDioxide_StatusLowBattery)
    EXTERN unsigned char bc_para_hSensorCarbonDioxide_StatusLowBattery;
#else
    #define BC_PARA_SENSORCARBONDIOXIDE_STATUSLOWBATTERY 0
#endif

#if SENSORCARBONDIOXIDE_LEVEL_BRC_ENABLE
    #define BC_PARA_SENSORCARBONDIOXIDE_LEVEL ((u8*)&bc_para_hSensorCarbonDioxide_Level)
    EXTERN unsigned char bc_para_hSensorCarbonDioxide_Level;
#else
    #define BC_PARA_SENSORCARBONDIOXIDE_LEVEL 0
#endif

#if SENSORCARBONDIOXIDE_PEAKLEVEL_BRC_ENABLE
    #define BC_PARA_SENSORCARBONDIOXIDE_PEAKLEVEL ((u8*)&bc_para_hSensorCarbonDioxide_PeakLevel)
    EXTERN unsigned char bc_para_hSensorCarbonDioxide_PeakLevel;
#else
    #define BC_PARA_SENSORCARBONDIOXIDE_PEAKLEVEL 0
#endif

#if SENSOR_CARBON_DIOXIDE_LINKED_SERVICES_ENABLE
	#define SENSOR_CARBON_DIOXIDE_LINKED_SVCS			hSensorCarbonDioxide_linked_svcs
	#define SENSOR_CARBON_DIOXIDE_LINKED_SVCS_LENGTH 	(sizeof(hSensorCarbonDioxide_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * hSensorCarbonDioxide_linked_svcs[];
#else
	#define SENSOR_CARBON_DIOXIDE_LINKED_SVCS			0
	#define SENSOR_CARBON_DIOXIDE_LINKED_SVCS_LENGTH 	0
#endif

EXTERN const u8 hSensor_Carbon_DioxideServiceName[28];

EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Carbon_Dioxide_Detected_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Carbon_Dioxide_StatusActive_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Carbon_Dioxide_StatusFault_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Carbon_Dioxide_StatusTampered_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Carbon_Dioxide_StatusLowBattery_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Carbon_Dioxide_Level_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Carbon_Dioxide_PeakLevel_R_S;


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN int hSensor_Carbon_Dioxide_DetectedReadCallback(void *pp);
EXTERN int hSensor_Carbon_Dioxide_StatusActiveReadCallback(void *pp);
EXTERN int hSensor_Carbon_Dioxide_StatusFaultReadCallback(void *pp);
EXTERN int hSensor_Carbon_Dioxide_StatusTamperedReadCallback(void *pp);
EXTERN int hSensor_Carbon_Dioxide_StatusLowBatteryReadCallback(void *pp);
EXTERN int hSensor_Carbon_Dioxide_LevelReadCallback(void *pp);
EXTERN int hSensor_Carbon_Dioxide_PeakLevelReadCallback(void *pp);
EXTERN int ccc_hSensorCarbonDioxide_DetectedWriteCB(void *pp);
EXTERN int ccc_hSensorCarbonDioxide_StatusActiveWriteCB(void *pp);
EXTERN int ccc_hSensorCarbonDioxide_StatusFaultWriteCB(void *pp);
EXTERN int ccc_hSensorCarbonDioxide_StatusTamperedWriteCB(void *pp);
EXTERN int ccc_hSensorCarbonDioxide_StatusLowBatteryWriteCB(void *pp);
EXTERN int ccc_hSensorCarbonDioxide_LevelWriteCB(void *pp);
EXTERN int ccc_hSensorCarbonDioxide_PeakLevelWriteCB(void *pp);


#endif /* _HK_SENSOR_CARBON_DIOXIDE_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
