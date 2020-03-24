/********************************************************************************************************
 * @file     hk_SensorCarbonMonoxideService.h
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
#ifndef _HK_SENSOR_CARBON_MONOXIDE_SERVICE_H_
#define _HK_SENSOR_CARBON_MONOXIDE_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */
//-#include "../../homekit_src/homekit_inc.h"


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_SENSOR_CARBON_MONOXIDE_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif


#define CHAR_SENSOR_CARBON_MONOXIDE_SVC_SIGN_ENABLE      0
#if CHAR_SENSOR_CARBON_MONOXIDE_SVC_SIGN_ENABLE
    #define SENSOR_CARBON_MONOXIDE_AS_PRIMARY_SERVICE    	0
    #define SENSOR_CARBON_MONOXIDE_IS_HIDDEN_SERVICE     	0
    #define SENSOR_CARBON_MONOXIDE_LINKED_SERVICES_ENABLE	0
    EXTERN const hap_characteristic_desc_t hap_desc_char_sensor_carbon_monoxide_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_sensor_carbon_monoxide_A5_R_S;
    EXTERN unsigned char VAR_SENSOR_CARBON_MONOXIDE_UNKOWN;
#else
    #define SENSOR_CARBON_MONOXIDE_AS_PRIMARY_SERVICE    	0
    #define SENSOR_CARBON_MONOXIDE_IS_HIDDEN_SERVICE     	0
    #define SENSOR_CARBON_MONOXIDE_LINKED_SERVICES_ENABLE	0
#endif

#define CHAR_SENSORCARBONMONOXIDE_STATUSACTIVE_ENABLE         0
#define CHAR_SENSORCARBONMONOXIDE_STATUSFAULT_ENABLE          0
#define CHAR_SENSORCARBONMONOXIDE_STATUSTAMPERED_ENABLE       0
#define CHAR_SENSORCARBONMONOXIDE_STATUSLOWBATTERY_ENABLE     0
#define CHAR_SENSORCARBONMONOXIDE_LEVEL_ENABLE                0
#define CHAR_SENSORCARBONMONOXIDE_PEAKLEVEL_ENABLE            0

#define SENSORCARBONMONOXIDE_DETECTED_IND_ENABLE             0
#define SENSORCARBONMONOXIDE_STATUSACTIVE_IND_ENABLE         0
#define SENSORCARBONMONOXIDE_STATUSFAULT_IND_ENABLE          0
#define SENSORCARBONMONOXIDE_STATUSTAMPERED_IND_ENABLE       0
#define SENSORCARBONMONOXIDE_STATUSLOWBATTERY_IND_ENABLE     0
#define SENSORCARBONMONOXIDE_LEVEL_IND_ENABLE                0
#define SENSORCARBONMONOXIDE_PEAKLEVEL_IND_ENABLE            0

#define SENSORCARBONMONOXIDE_DETECTED_BRC_ENABLE             0
#define SENSORCARBONMONOXIDE_STATUSACTIVE_BRC_ENABLE         0
#define SENSORCARBONMONOXIDE_STATUSFAULT_BRC_ENABLE          0
#define SENSORCARBONMONOXIDE_STATUSTAMPERED_BRC_ENABLE       0
#define SENSORCARBONMONOXIDE_STATUSLOWBATTERY_BRC_ENABLE     0
#define SENSORCARBONMONOXIDE_LEVEL_BRC_ENABLE                0
#define SENSORCARBONMONOXIDE_PEAKLEVEL_BRC_ENABLE            0


#define SENSORCARBONMONOXIDE_DETECTED_NO 3
#if SENSORCARBONMONOXIDE_DETECTED_IND_ENABLE
    #define SENSORCARBONMONOXIDE_DETECTED_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorCarbonMonoxide_Detected), ccc_hSensorCarbonMonoxide_DetectedWriteCB, 0},
    #define SENSORCARBONMONOXIDE_DETECTED_IND_NO  1
#else
    #define SENSORCARBONMONOXIDE_DETECTED_IND_DESC
    #define SENSORCARBONMONOXIDE_DETECTED_IND_NO  0
#endif

#if CHAR_SENSORCARBONMONOXIDE_STATUSACTIVE_ENABLE
    #define SENSORCARBONMONOXIDE_STATUSACTIVE_NO 3
    #define CHAR_SENSORCARBONMONOXIDE_STATUSACTIVE_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Carbon_Monoxide_StatusActive_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Carbon_Monoxide_StatusActive),0, (u8*)(charStatusActiveUUID), (u8*)&hSensor_Carbon_Monoxide_StatusActive, 0, hSensor_Carbon_Monoxide_StatusActiveReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_CARBON_MONOXIDE_INST_NO + 3), 0, 0},    \
            SENSORCARBONMONOXIDE_STATUSACTIVE_IND_DESC

	#if SENSORCARBONMONOXIDE_STATUSACTIVE_IND_ENABLE
	    #define SENSORCARBONMONOXIDE_STATUSACTIVE_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorCarbonMonoxide_StatusActive), ccc_hSensorCarbonMonoxide_StatusActiveWriteCB, 0},
	    #define SENSORCARBONMONOXIDE_STATUSACTIVE_IND_NO  1
	#else
	    #define SENSORCARBONMONOXIDE_STATUSACTIVE_IND_DESC
	    #define SENSORCARBONMONOXIDE_STATUSACTIVE_IND_NO  0
	#endif
#else
    #define SENSORCARBONMONOXIDE_STATUSACTIVE_NO 0
    #define CHAR_SENSORCARBONMONOXIDE_STATUSACTIVE_ATT_TABLE
    #define SENSORCARBONMONOXIDE_STATUSACTIVE_IND_NO  0
#endif

#if CHAR_SENSORCARBONMONOXIDE_STATUSFAULT_ENABLE
    #define SENSORCARBONMONOXIDE_STATUSFAULT_NO 3
    #define CHAR_SENSORCARBONMONOXIDE_STATUSFAULT_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Carbon_Monoxide_StatusFault_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Carbon_Monoxide_StatusFault),0, (u8*)(charStatusFaultUUID), (u8*)&hSensor_Carbon_Monoxide_StatusFault, 0, hSensor_Carbon_Monoxide_StatusFaultReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_CARBON_MONOXIDE_INST_NO + 4), 0, 0},    \
            SENSORCARBONMONOXIDE_STATUSFAULT_IND_DESC

	#if SENSORCARBONMONOXIDE_STATUSFAULT_IND_ENABLE
	    #define SENSORCARBONMONOXIDE_STATUSFAULT_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorCarbonMonoxide_StatusFault), ccc_hSensorCarbonMonoxide_StatusFaultWriteCB, 0},
	    #define SENSORCARBONMONOXIDE_STATUSFAULT_IND_NO  1
	#else
	    #define SENSORCARBONMONOXIDE_STATUSFAULT_IND_DESC
	    #define SENSORCARBONMONOXIDE_STATUSFAULT_IND_NO  0
	#endif
#else
    #define SENSORCARBONMONOXIDE_STATUSFAULT_NO 0
    #define CHAR_SENSORCARBONMONOXIDE_STATUSFAULT_ATT_TABLE
    #define SENSORCARBONMONOXIDE_STATUSFAULT_IND_NO  0
#endif

#if CHAR_SENSORCARBONMONOXIDE_STATUSTAMPERED_ENABLE
    #define SENSORCARBONMONOXIDE_STATUSTAMPERED_NO 3
    #define CHAR_SENSORCARBONMONOXIDE_STATUSTAMPERED_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Carbon_Monoxide_StatusTampered_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Carbon_Monoxide_StatusTampered),0, (u8*)(charStatusTamperedUUID), (u8*)&hSensor_Carbon_Monoxide_StatusTampered, 0, hSensor_Carbon_Monoxide_StatusTamperedReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_CARBON_MONOXIDE_INST_NO + 5), 0, 0},    \
            SENSORCARBONMONOXIDE_STATUSTAMPERED_IND_DESC

	#if SENSORCARBONMONOXIDE_STATUSTAMPERED_IND_ENABLE
	    #define SENSORCARBONMONOXIDE_STATUSTAMPERED_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorCarbonMonoxide_StatusTampered), ccc_hSensorCarbonMonoxide_StatusTamperedWriteCB, 0},
	    #define SENSORCARBONMONOXIDE_STATUSTAMPERED_IND_NO  1
	#else
	    #define SENSORCARBONMONOXIDE_STATUSTAMPERED_IND_DESC
	    #define SENSORCARBONMONOXIDE_STATUSTAMPERED_IND_NO  0
	#endif
#else
    #define SENSORCARBONMONOXIDE_STATUSTAMPERED_NO 0
    #define CHAR_SENSORCARBONMONOXIDE_STATUSTAMPERED_ATT_TABLE
    #define SENSORCARBONMONOXIDE_STATUSTAMPERED_IND_NO  0
#endif

#if CHAR_SENSORCARBONMONOXIDE_STATUSLOWBATTERY_ENABLE
    #define SENSORCARBONMONOXIDE_STATUSLOWBATTERY_NO 3
    #define CHAR_SENSORCARBONMONOXIDE_STATUSLOWBATTERY_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Carbon_Monoxide_StatusLowBattery_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Carbon_Monoxide_StatusLowBattery),0, (u8*)(charStatusLowBatteryUUID), (u8*)&hSensor_Carbon_Monoxide_StatusLowBattery, 0, hSensor_Carbon_Monoxide_StatusLowBatteryReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_CARBON_MONOXIDE_INST_NO + 6), 0, 0},    \
            SENSORCARBONMONOXIDE_STATUSLOWBATTERY_IND_DESC

	#if SENSORCARBONMONOXIDE_STATUSLOWBATTERY_IND_ENABLE
	    #define SENSORCARBONMONOXIDE_STATUSLOWBATTERY_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorCarbonMonoxide_StatusLowBattery), ccc_hSensorCarbonMonoxide_StatusLowBatteryWriteCB, 0},
	    #define SENSORCARBONMONOXIDE_STATUSLOWBATTERY_IND_NO  1
	#else
	    #define SENSORCARBONMONOXIDE_STATUSLOWBATTERY_IND_DESC
	    #define SENSORCARBONMONOXIDE_STATUSLOWBATTERY_IND_NO  0
	#endif
#else
    #define SENSORCARBONMONOXIDE_STATUSLOWBATTERY_NO 0
    #define CHAR_SENSORCARBONMONOXIDE_STATUSLOWBATTERY_ATT_TABLE
    #define SENSORCARBONMONOXIDE_STATUSLOWBATTERY_IND_NO  0
#endif

#if CHAR_SENSORCARBONMONOXIDE_LEVEL_ENABLE
    #define SENSORCARBONMONOXIDE_LEVEL_NO 3
    #define CHAR_SENSORCARBONMONOXIDE_LEVEL_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Carbon_Monoxide_Level_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Carbon_Monoxide_Level),0, (u8*)(charCarbonMonoxideLevelUUID), (u8*)&hSensor_Carbon_Monoxide_Level, 0, hSensor_Carbon_Monoxide_LevelReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_CARBON_MONOXIDE_INST_NO + 7), 0, 0},    \
            SENSORCARBONMONOXIDE_LEVEL_IND_DESC

	#if SENSORCARBONMONOXIDE_LEVEL_IND_ENABLE
	    #define SENSORCARBONMONOXIDE_LEVEL_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorCarbonMonoxide_Level), ccc_hSensorCarbonMonoxide_LevelWriteCB, 0},
	    #define SENSORCARBONMONOXIDE_LEVEL_IND_NO  1
	#else
	    #define SENSORCARBONMONOXIDE_LEVEL_IND_DESC
	    #define SENSORCARBONMONOXIDE_LEVEL_IND_NO  0
	#endif
#else
    #define SENSORCARBONMONOXIDE_LEVEL_NO 0
    #define CHAR_SENSORCARBONMONOXIDE_LEVEL_ATT_TABLE
    #define SENSORCARBONMONOXIDE_LEVEL_IND_NO  0
#endif

#if CHAR_SENSORCARBONMONOXIDE_PEAKLEVEL_ENABLE
    #define SENSORCARBONMONOXIDE_PEAKLEVEL_NO 3
    #define CHAR_SENSORCARBONMONOXIDE_PEAKLEVEL_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Carbon_Monoxide_PeakLevel_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Carbon_Monoxide_PeakLevel),0, (u8*)(charCarbonMonoxidePeakLevelUUID), (u8*)&hSensor_Carbon_Monoxide_PeakLevel, 0, hSensor_Carbon_Monoxide_PeakLevelReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_CARBON_MONOXIDE_INST_NO + 8), 0, 0},    \
            SENSORCARBONMONOXIDE_PEAKLEVEL_IND_DESC

	#if SENSORCARBONMONOXIDE_PEAKLEVEL_IND_ENABLE
	    #define SENSORCARBONMONOXIDE_PEAKLEVEL_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorCarbonMonoxide_PeakLevel), ccc_hSensorCarbonMonoxide_PeakLevelWriteCB, 0},
	    #define SENSORCARBONMONOXIDE_PEAKLEVEL_IND_NO  1
	#else
	    #define SENSORCARBONMONOXIDE_PEAKLEVEL_IND_DESC
	    #define SENSORCARBONMONOXIDE_PEAKLEVEL_IND_NO  0
	#endif
#else
    #define SENSORCARBONMONOXIDE_PEAKLEVEL_NO 0
    #define CHAR_SENSORCARBONMONOXIDE_PEAKLEVEL_ATT_TABLE
    #define SENSORCARBONMONOXIDE_PEAKLEVEL_IND_NO  0
#endif


#if CHAR_SENSOR_CARBON_MONOXIDE_SVC_SIGN_ENABLE
#define CHAR_SENSOR_CARBON_MONOXIDE_SVC_A5_SIGN_TABLE   \
    {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_sensor_carbon_monoxide_A5_R_S, &hap_desc_service_sensor_carbon_monoxide_A5_R_S},  \
    {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_SENSOR_CARBON_MONOXIDE_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_SENSOR_CARBON_MONOXIDE_UNKOWN, 0, 0},    \
        {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_CARBON_MONOXIDE_INST_NO + 9), 0, 0},
#define SENSOR_CARBON_MONOXIDE_SVC_SIGN_NO   3
#else
#define CHAR_SENSOR_CARBON_MONOXIDE_SVC_A5_SIGN_TABLE
#define SENSOR_CARBON_MONOXIDE_SVC_SIGN_NO   0
#endif


#define SENSOR_CARBON_MONOXIDE_NO_DELTA SENSORCARBONMONOXIDE_DETECTED_IND_NO    \
                                        +SENSORCARBONMONOXIDE_STATUSACTIVE_IND_NO   \
                                        +SENSORCARBONMONOXIDE_STATUSFAULT_IND_NO    \
                                        +SENSORCARBONMONOXIDE_STATUSTAMPERED_IND_NO    \
                                        +SENSORCARBONMONOXIDE_STATUSLOWBATTERY_IND_NO   \
                                        +SENSORCARBONMONOXIDE_LEVEL_IND_NO  \
                                        +SENSORCARBONMONOXIDE_PEAKLEVEL_IND_NO

#define SENSOR_CARBON_MONOXIDE_NO       9 +SENSORCARBONMONOXIDE_STATUSACTIVE_NO	\
										+SENSORCARBONMONOXIDE_STATUSFAULT_NO	\
										+SENSORCARBONMONOXIDE_STATUSTAMPERED_NO	\
										+SENSORCARBONMONOXIDE_STATUSLOWBATTERY_NO	\
										+SENSORCARBONMONOXIDE_LEVEL_NO	\
										+SENSORCARBONMONOXIDE_PEAKLEVEL_NO	\
										+SENSOR_CARBON_MONOXIDE_SVC_SIGN_NO
#define SENSOR_CARBON_MONOXIDE_HANDLE_NUM (SENSOR_CARBON_MONOXIDE_NO + SENSOR_CARBON_MONOXIDE_NO_DELTA)


#define HOMEKIT_SENSOR_CARBON_MONOXIDE_SERVICE_ATT_TABLE    \
    {SENSOR_CARBON_MONOXIDE_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(sensor_carbon_monoxideServiceUUID),sizeof(sensor_carbon_monoxideServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(sensor_carbon_monoxideServiceUUID), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(insts + SENSOR_CARBON_MONOXIDE_INST_NO), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_NAME, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(hSensor_Carbon_MonoxideServiceName), 0, (u8*)(nameUUID), (u8*)(hSensor_Carbon_MonoxideServiceName), 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + SENSOR_CARBON_MONOXIDE_INST_NO + 1), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Carbon_Monoxide_Detected_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Carbon_Monoxide_Detected),0, (u8*)(charCarbonMonoxideDetectedUUID), (u8*)&hSensor_Carbon_Monoxide_Detected, 0, hSensor_Carbon_Monoxide_DetectedReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_CARBON_MONOXIDE_INST_NO + 2), 0, 0},    \
            SENSORCARBONMONOXIDE_DETECTED_IND_DESC \
            CHAR_SENSORCARBONMONOXIDE_STATUSACTIVE_ATT_TABLE \
            CHAR_SENSORCARBONMONOXIDE_STATUSFAULT_ATT_TABLE \
            CHAR_SENSORCARBONMONOXIDE_STATUSTAMPERED_ATT_TABLE \
            CHAR_SENSORCARBONMONOXIDE_STATUSLOWBATTERY_ATT_TABLE \
            CHAR_SENSORCARBONMONOXIDE_LEVEL_ATT_TABLE \
            CHAR_SENSORCARBONMONOXIDE_PEAKLEVEL_ATT_TABLE \
        	CHAR_SENSOR_CARBON_MONOXIDE_SVC_A5_SIGN_TABLE


#define SENSORCARBONMONOXIDE_HANDLE_COMMON_NUM     8

#define SENSORCARBONMONOXIDE_HANDLE_NUM_DETECTED         SENSOR_CARBON_MONOXIDE_HANDLE_START_NUM +SENSORCARBONMONOXIDE_HANDLE_COMMON_NUM
#define SENSORCARBONMONOXIDE_HANDLE_NUM_STATUSACTIVE     SENSORCARBONMONOXIDE_HANDLE_NUM_DETECTED +SENSORCARBONMONOXIDE_DETECTED_NO +SENSORCARBONMONOXIDE_DETECTED_IND_NO
#define SENSORCARBONMONOXIDE_HANDLE_NUM_STATUSFAULT      SENSORCARBONMONOXIDE_HANDLE_NUM_STATUSACTIVE +SENSORCARBONMONOXIDE_STATUSACTIVE_NO +SENSORCARBONMONOXIDE_STATUSACTIVE_IND_NO
#define SENSORCARBONMONOXIDE_HANDLE_NUM_STATUSTAMPERED   SENSORCARBONMONOXIDE_HANDLE_NUM_STATUSFAULT +SENSORCARBONMONOXIDE_STATUSFAULT_NO +SENSORCARBONMONOXIDE_STATUSFAULT_IND_NO
#define SENSORCARBONMONOXIDE_HANDLE_NUM_STATUSLOWBATTERY SENSORCARBONMONOXIDE_HANDLE_NUM_STATUSTAMPERED +SENSORCARBONMONOXIDE_STATUSTAMPERED_NO +SENSORCARBONMONOXIDE_STATUSTAMPERED_IND_NO
#define SENSORCARBONMONOXIDE_HANDLE_NUM_LEVEL            SENSORCARBONMONOXIDE_HANDLE_NUM_STATUSLOWBATTERY +SENSORCARBONMONOXIDE_STATUSLOWBATTERY_NO +SENSORCARBONMONOXIDE_STATUSLOWBATTERY_IND_NO
#define SENSORCARBONMONOXIDE_HANDLE_NUM_PEAKLEVEL        SENSORCARBONMONOXIDE_HANDLE_NUM_LEVEL +SENSORCARBONMONOXIDE_LEVEL_NO +SENSORCARBONMONOXIDE_LEVEL_IND_NO
#define SENSORCARBONMONOXIDE_HANDLE_NUM_THE_END


#define SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_DETECTED                 u32 Sensor_Carbon_Monoxide_Detected:1;    /* 0~1 */
#define SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_DETECTED_STORE               STORE_CHAR(Sensor_Carbon_Monoxide_Detected,hSensor_Carbon_Monoxide_Detected)
#define SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_DETECTED_RESTORE             RESTORE_CHAR(Sensor_Carbon_Monoxide_Detected,hSensor_Carbon_Monoxide_Detected)

#define SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_STATUSACTIVE             u32 Sensor_Carbon_Monoxide_StatusActive:1;    /* 0~1 */
#define SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_STATUSACTIVE_STORE           STORE_CHAR(Sensor_Carbon_Monoxide_StatusActive,hSensor_Carbon_Monoxide_StatusActive)
#define SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_STATUSACTIVE_RESTORE         RESTORE_CHAR(Sensor_Carbon_Monoxide_StatusActive,hSensor_Carbon_Monoxide_StatusActive)

#define SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_STATUSFAULT              u32 Sensor_Carbon_Monoxide_StatusFault:1;    /* 0~1 */
#define SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_STATUSFAULT_STORE            STORE_CHAR(Sensor_Carbon_Monoxide_StatusFault,hSensor_Carbon_Monoxide_StatusFault)
#define SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_STATUSFAULT_RESTORE          RESTORE_CHAR(Sensor_Carbon_Monoxide_StatusFault,hSensor_Carbon_Monoxide_StatusFault)

#define SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_STATUSTAMPERED           u32 Sensor_Carbon_Monoxide_StatusTampered:1;    /* 0~1 */
#define SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_STATUSTAMPERED_STORE         STORE_CHAR(Sensor_Carbon_Monoxide_StatusTampered,hSensor_Carbon_Monoxide_StatusTampered)
#define SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_STATUSTAMPERED_RESTORE       RESTORE_CHAR(Sensor_Carbon_Monoxide_StatusTampered,hSensor_Carbon_Monoxide_StatusTampered)

#define SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_STATUSLOWBATTERY         u32 Sensor_Carbon_Monoxide_StatusLowBattery:1;    /* 0~1 **/
#define SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_STATUSLOWBATTERY_STORE       STORE_CHAR(Sensor_Carbon_Monoxide_StatusLowBattery,hSensor_Carbon_Monoxide_StatusLowBattery)
#define SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_STATUSLOWBATTERY_RESTORE     RESTORE_CHAR(Sensor_Carbon_Monoxide_StatusLowBattery,hSensor_Carbon_Monoxide_StatusLowBattery)

#define SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_LEVEL                    u32 Sensor_Carbon_Monoxide_Level;    /* float */
#define SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_LEVEL_STORE                  STORE_CHAR(Sensor_Carbon_Monoxide_Level,hSensor_Carbon_Monoxide_Level)
#define SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_LEVEL_RESTORE                RESTORE_CHAR(Sensor_Carbon_Monoxide_Level,hSensor_Carbon_Monoxide_Level)

#define SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_PEAKLEVEL                u32 Sensor_Carbon_Monoxide_PeakLevel;    /* float */
#define SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_PEAKLEVEL_STORE              STORE_CHAR(Sensor_Carbon_Monoxide_PeakLevel,hSensor_Carbon_Monoxide_PeakLevel)
#define SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_PEAKLEVEL_RESTORE            RESTORE_CHAR(Sensor_Carbon_Monoxide_PeakLevel,hSensor_Carbon_Monoxide_PeakLevel)

#define SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T   \
                    SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_DETECTED  \
                    SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_STATUSACTIVE   \
                    SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_STATUSFAULT  \
                    SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_STATUSTAMPERED   \
                    SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_STATUSLOWBATTERY  \
                    SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_LEVEL   \
                    SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_PEAKLEVEL

#define SENSOR_CARBON_MONOXIDE_STORE_CHAR_TO_FLASH  \
                    SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_DETECTED_STORE    \
                    SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_STATUSACTIVE_STORE \
                    SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_STATUSFAULT_STORE    \
                    SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_STATUSTAMPERED_STORE \
                    SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_STATUSLOWBATTERY_STORE    \
                    SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_LEVEL_STORE \
                    SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_PEAKLEVEL_STORE

#define SENSOR_CARBON_MONOXIDE_RESTORE_CHAR_FROM_FLASH  \
                    SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_DETECTED_RESTORE  \
                    SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_STATUSACTIVE_RESTORE   \
                    SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_STATUSFAULT_RESTORE  \
                    SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_STATUSTAMPERED_RESTORE   \
                    SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_STATUSLOWBATTERY_RESTORE  \
                    SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_LEVEL_RESTORE   \
                    SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_PEAKLEVEL_RESTORE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN unsigned char hSensor_Carbon_Monoxide_Detected;
EXTERN unsigned char hSensor_Carbon_Monoxide_StatusActive;
EXTERN unsigned char hSensor_Carbon_Monoxide_StatusFault;
EXTERN unsigned char hSensor_Carbon_Monoxide_StatusTampered;
EXTERN unsigned char hSensor_Carbon_Monoxide_StatusLowBattery;
EXTERN unsigned int  hSensor_Carbon_Monoxide_Level;
EXTERN unsigned int  hSensor_Carbon_Monoxide_PeakLevel;

EXTERN unsigned short ccc_hSensorCarbonMonoxide_Detected;
EXTERN unsigned short ccc_hSensorCarbonMonoxide_StatusActive;
EXTERN unsigned short ccc_hSensorCarbonMonoxide_StatusFault;
EXTERN unsigned short ccc_hSensorCarbonMonoxide_StatusTampered;
EXTERN unsigned short ccc_hSensorCarbonMonoxide_StatusLowBattery;
EXTERN unsigned short ccc_hSensorCarbonMonoxide_Level;
EXTERN unsigned short ccc_hSensorCarbonMonoxide_PeakLevel;

#if SENSORCARBONMONOXIDE_DETECTED_BRC_ENABLE
    #define BC_PARA_SENSORCARBONMONOXIDE_DETECTED ((u8*)&bc_para_hSensorCarbonMonoxide_Detected)
    EXTERN unsigned char bc_para_hSensorCarbonMonoxide_Detected;
#else
    #define BC_PARA_SENSORCARBONMONOXIDE_DETECTED 0
#endif

#if SENSORCARBONMONOXIDE_STATUSACTIVE_BRC_ENABLE
    #define BC_PARA_SENSORCARBONMONOXIDE_STATUSACTIVE ((u8*)&bc_para_hSensorCarbonMonoxide_StatusActive)
    EXTERN unsigned char bc_para_hSensorCarbonMonoxide_StatusActive;
#else
    #define BC_PARA_SENSORCARBONMONOXIDE_STATUSACTIVE 0
#endif

#if SENSORCARBONMONOXIDE_STATUSFAULT_BRC_ENABLE
    #define BC_PARA_SENSORCARBONMONOXIDE_STATUSFAULT ((u8*)&bc_para_hSensorCarbonMonoxide_StatusFault)
    EXTERN unsigned char bc_para_hSensorCarbonMonoxide_StatusActive;
#else
    #define BC_PARA_SENSORCARBONMONOXIDE_STATUSFAULT 0
#endif

#if SENSORCARBONMONOXIDE_STATUSTAMPERED_BRC_ENABLE
    #define BC_PARA_SENSORCARBONMONOXIDE_STATUSTAMPERED ((u8*)&bc_para_hSensorCarbonMonoxide_StatusTampered)
    EXTERN unsigned char bc_para_hSensorCarbonMonoxide_StatusTampered;
#else
    #define BC_PARA_SENSORCARBONMONOXIDE_STATUSTAMPERED 0
#endif

#if SENSORCARBONMONOXIDE_STATUSLOWBATTERY_BRC_ENABLE
    #define BC_PARA_SENSORCARBONMONOXIDE_STATUSLOWBATTERY ((u8*)&bc_para_hSensorCarbonMonoxide_StatusLowBattery)
    EXTERN unsigned char bc_para_hSensorCarbonMonoxide_StatusLowBattery;
#else
    #define BC_PARA_SENSORCARBONMONOXIDE_STATUSLOWBATTERY 0
#endif

#if SENSORCARBONMONOXIDE_LEVEL_BRC_ENABLE
    #define BC_PARA_SENSORCARBONMONOXIDE_LEVEL ((u8*)&bc_para_hSensorCarbonMonoxide_Level)
    EXTERN unsigned char bc_para_hSensorCarbonMonoxide_Level;
#else
    #define BC_PARA_SENSORCARBONMONOXIDE_LEVEL 0
#endif

#if SENSORCARBONMONOXIDE_PEAKLEVEL_BRC_ENABLE
    #define BC_PARA_SENSORCARBONMONOXIDE_PEAKLEVEL ((u8*)&bc_para_hSensorCarbonMonoxide_PeakLevel)
    EXTERN unsigned char bc_para_hSensorCarbonMonoxide_PeakLevel;
#else
    #define BC_PARA_SENSORCARBONMONOXIDE_PEAKLEVEL 0
#endif

#if SENSOR_CARBON_MONOXIDE_LINKED_SERVICES_ENABLE
	#define SENSOR_CARBON_MONOXIDE_LINKED_SVCS			hSensorCarbonMonoxide_linked_svcs
	#define SENSOR_CARBON_MONOXIDE_LINKED_SVCS_LENGTH 	(sizeof(hSensorCarbonMonoxide_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * hSensorCarbonMonoxide_linked_svcs[];
#else
	#define SENSOR_CARBON_MONOXIDE_LINKED_SVCS			0
	#define SENSOR_CARBON_MONOXIDE_LINKED_SVCS_LENGTH 	0
#endif

EXTERN const u8 hSensor_Carbon_MonoxideServiceName[29];

EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Carbon_Monoxide_Detected_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Carbon_Monoxide_StatusActive_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Carbon_Monoxide_StatusFault_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Carbon_Monoxide_StatusTampered_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Carbon_Monoxide_StatusLowBattery_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Carbon_Monoxide_Level_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Carbon_Monoxide_PeakLevel_R_S;


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN int hSensor_Carbon_Monoxide_DetectedReadCallback(void *pp);
EXTERN int hSensor_Carbon_Monoxide_StatusActiveReadCallback(void *pp);
EXTERN int hSensor_Carbon_Monoxide_StatusFaultReadCallback(void *pp);
EXTERN int hSensor_Carbon_Monoxide_StatusTamperedReadCallback(void *pp);
EXTERN int hSensor_Carbon_Monoxide_StatusLowBatteryReadCallback(void *pp);
EXTERN int hSensor_Carbon_Monoxide_LevelReadCallback(void *pp);
EXTERN int hSensor_Carbon_Monoxide_PeakLevelReadCallback(void *pp);
EXTERN int ccc_hSensorCarbonMonoxide_DetectedWriteCB(void *pp);
EXTERN int ccc_hSensorCarbonMonoxide_StatusActiveWriteCB(void *pp);
EXTERN int ccc_hSensorCarbonMonoxide_StatusFaultWriteCB(void *pp);
EXTERN int ccc_hSensorCarbonMonoxide_StatusTamperedWriteCB(void *pp);
EXTERN int ccc_hSensorCarbonMonoxide_StatusLowBatteryWriteCB(void *pp);
EXTERN int ccc_hSensorCarbonMonoxide_LevelWriteCB(void *pp);
EXTERN int ccc_hSensorCarbonMonoxide_PeakLevelWriteCB(void *pp);

#endif /* _HK_SENSOR_CARBON_MONOXIDE_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
