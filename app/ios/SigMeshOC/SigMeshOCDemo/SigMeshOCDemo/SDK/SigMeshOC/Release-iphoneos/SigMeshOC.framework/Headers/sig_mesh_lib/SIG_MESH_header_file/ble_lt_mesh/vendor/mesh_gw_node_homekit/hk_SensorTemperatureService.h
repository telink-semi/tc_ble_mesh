/********************************************************************************************************
 * @file     hk_SensorTemperatureService.h
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
#ifndef _HK_SENSOR_TEMPERATURE_SERVICE_H_
#define _HK_SENSOR_TEMPERATURE_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */
//-#include "../../homekit_src/homekit_inc.h"


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_SENSOR_TEMPERATURE_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif


#define CHAR_SENSOR_TEMPERATURE_SVC_SIGN_ENABLE      0
#if CHAR_SENSOR_TEMPERATURE_SVC_SIGN_ENABLE
    #define SENSOR_TEMPERATURE_AS_PRIMARY_SERVICE    	0
    #define SENSOR_TEMPERATURE_IS_HIDDEN_SERVICE     	0
    #define SENSOR_TEMPERATURE_LINKED_SERVICES_ENABLE	0
    EXTERN const hap_characteristic_desc_t hap_desc_char_sensor_temperature_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_sensor_temperature_A5_R_S;
    EXTERN unsigned char VAR_SENSOR_TEMPERATURE_UNKOWN;
#else
    #define SENSOR_TEMPERATURE_AS_PRIMARY_SERVICE    	0
    #define SENSOR_TEMPERATURE_IS_HIDDEN_SERVICE     	0
    #define SENSOR_TEMPERATURE_LINKED_SERVICES_ENABLE	0
#endif

#define CHAR_SENSORTEMPERATURE_STATUSACTIVE_ENABLE		0
#define CHAR_SENSORTEMPERATURE_STATUSFAULT_ENABLE		0
#define CHAR_SENSORTEMPERATURE_STATUSLOWBATTERY_ENABLE	0
#define CHAR_SENSORTEMPERATURE_STATUSTAMPERED_ENABLE	0

#define SENSORTEMPERATURE_CURRENTTEMPERATURE_IND_ENABLE 0
#define SENSORTEMPERATURE_STATUSACTIVE_IND_ENABLE       0
#define SENSORTEMPERATURE_STATUSFAULT_IND_ENABLE        0
#define SENSORTEMPERATURE_STATUSTAMPERED_IND_ENABLE     0
#define SENSORTEMPERATURE_STATUSLOWBATTERY_IND_ENABLE   0

#define SENSORTEMPERATURE_CURRENTTEMPERATURE_BRC_ENABLE 0
#define SENSORTEMPERATURE_STATUSACTIVE_BRC_ENABLE       0
#define SENSORTEMPERATURE_STATUSFAULT_BRC_ENABLE        0
#define SENSORTEMPERATURE_STATUSTAMPERED_BRC_ENABLE     0
#define SENSORTEMPERATURE_STATUSLOWBATTERY_BRC_ENABLE   0


#define SENSORTEMPERATURE_CURRENTTEMPERATURE_NO 3
#if SENSORTEMPERATURE_CURRENTTEMPERATURE_IND_ENABLE
    #define SENSORTEMPERATURE_CURRENTTEMPERATURE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorTemperature_CurrentTemperature), ccc_hSensorTemperature_CurrentTemperatureWriteCB, 0},
    #define SENSORTEMPERATURE_CURRENTTEMPERATURE_IND_NO  1
#else
    #define SENSORTEMPERATURE_CURRENTTEMPERATURE_IND_DESC
    #define SENSORTEMPERATURE_CURRENTTEMPERATURE_IND_NO  0
#endif


#if CHAR_SENSORTEMPERATURE_STATUSACTIVE_ENABLE
    #define SENSORTEMPERATURE_STATUSACTIVE_NO 3
    #define CHAR_SENSORTEMPERATURE_STATUSACTIVE_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Temperature_StatusActive_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Temperature_StatusActive),0, (u8*)(charStatusActiveUUID), (u8*)&hSensor_Temperature_StatusActive, 0, hSensor_Temperature_StatusActiveReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_TEMPERATURE_INST_NO + 3), 0, 0},    \
            SENSORTEMPERATURE_STATUSACTIVE_IND_DESC

	#if SENSORTEMPERATURE_STATUSACTIVE_IND_ENABLE
	    #define SENSORTEMPERATURE_STATUSACTIVE_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorTemperature_StatusActive), ccc_hSensorTemperature_StatusActiveWriteCB, 0},
	    #define SENSORTEMPERATURE_STATUSACTIVE_IND_NO  1
	#else
	    #define SENSORTEMPERATURE_STATUSACTIVE_IND_DESC
	    #define SENSORTEMPERATURE_STATUSACTIVE_IND_NO  0
	#endif
#else
    #define SENSORTEMPERATURE_STATUSACTIVE_NO 0
    #define CHAR_SENSORTEMPERATURE_STATUSACTIVE_ATT_TABLE
    #define SENSORTEMPERATURE_STATUSACTIVE_IND_NO  0
#endif


#if CHAR_SENSORTEMPERATURE_STATUSFAULT_ENABLE
    #define SENSORTEMPERATURE_STATUSFAULT_NO 3
    #define CHAR_SENSORTEMPERATURE_STATUSFAULT_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Temperature_StatusFault_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Temperature_StatusFault),0, (u8*)(charStatusFaultUUID), (u8*)&hSensor_Temperature_StatusFault, 0, hSensor_Temperature_StatusFaultReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_TEMPERATURE_INST_NO + 4), 0, 0},    \
            SENSORTEMPERATURE_STATUSFAULT_IND_DESC

	#if SENSORTEMPERATURE_STATUSFAULT_IND_ENABLE
	    #define SENSORTEMPERATURE_STATUSFAULT_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorTemperature_StatusFault), ccc_hSensorTemperature_StatusFaultWriteCB, 0},
	    #define SENSORTEMPERATURE_STATUSFAULT_IND_NO  1
	#else
	    #define SENSORTEMPERATURE_STATUSFAULT_IND_DESC
	    #define SENSORTEMPERATURE_STATUSFAULT_IND_NO  0
	#endif
#else
    #define SENSORTEMPERATURE_STATUSFAULT_NO 0
    #define CHAR_SENSORTEMPERATURE_STATUSFAULT_ATT_TABLE
    #define SENSORTEMPERATURE_STATUSFAULT_IND_NO  0
#endif


#if CHAR_SENSORTEMPERATURE_STATUSLOWBATTERY_ENABLE
    #define SENSORTEMPERATURE_STATUSLOWBATTERY_NO 3
    #define CHAR_SENSORTEMPERATURE_STATUSLOWBATTERY_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Temperature_StatusLowBattery_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Temperature_StatusLowBattery),0, (u8*)(charStatusLowBatteryUUID), (u8*)&hSensor_Temperature_StatusLowBattery, 0, hSensor_Temperature_StatusLowBatteryReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_TEMPERATURE_INST_NO + 5), 0, 0},    \
            SENSORTEMPERATURE_STATUSLOWBATTERY_IND_DESC

	#if SENSORTEMPERATURE_STATUSLOWBATTERY_IND_ENABLE
	    #define SENSORTEMPERATURE_STATUSLOWBATTERY_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorTemperature_StatusLowBattery), ccc_hSensorTemperature_StatusLowBatteryWriteCB, 0},
	    #define SENSORTEMPERATURE_STATUSLOWBATTERY_IND_NO  1
	#else
	    #define SENSORTEMPERATURE_STATUSLOWBATTERY_IND_DESC
	    #define SENSORTEMPERATURE_STATUSLOWBATTERY_IND_NO  0
	#endif
#else
    #define SENSORTEMPERATURE_STATUSLOWBATTERY_NO 0
    #define CHAR_SENSORTEMPERATURE_STATUSLOWBATTERY_ATT_TABLE
    #define SENSORTEMPERATURE_STATUSLOWBATTERY_IND_NO  0
#endif


#if CHAR_SENSORTEMPERATURE_STATUSTAMPERED_ENABLE
    #define SENSORTEMPERATURE_STATUSTAMPERED_NO 3
    #define CHAR_SENSORTEMPERATURE_STATUSTAMPERED_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Temperature_StatusTampered_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Temperature_StatusTampered),0, (u8*)(charStatusTamperedUUID), (u8*)&hSensor_Temperature_StatusTampered, 0, hSensor_Temperature_StatusTamperedReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_TEMPERATURE_INST_NO + 6), 0, 0},    \
            SENSORTEMPERATURE_STATUSTAMPERED_IND_DESC

	#if SENSORTEMPERATURE_STATUSTAMPERED_IND_ENABLE
	    #define SENSORTEMPERATURE_STATUSTAMPERED_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorTemperature_StatusTampered), ccc_hSensorTemperature_StatusTamperedWriteCB, 0},
	    #define SENSORTEMPERATURE_STATUSTAMPERED_IND_NO  1
	#else
	    #define SENSORTEMPERATURE_STATUSTAMPERED_IND_DESC
	    #define SENSORTEMPERATURE_STATUSTAMPERED_IND_NO  0
	#endif
#else
    #define SENSORTEMPERATURE_STATUSTAMPERED_NO 0
    #define CHAR_SENSORTEMPERATURE_STATUSTAMPERED_ATT_TABLE
    #define SENSORTEMPERATURE_STATUSTAMPERED_IND_NO  0
#endif


#if CHAR_SENSOR_TEMPERATURE_SVC_SIGN_ENABLE
#define CHAR_SENSOR_TEMPERATURE_SVC_A5_SIGN_TABLE   \
    {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_sensor_temperature_A5_R_S, &hap_desc_service_sensor_temperature_A5_R_S},  \
    {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_SENSOR_TEMPERATURE_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_SENSOR_TEMPERATURE_UNKOWN, 0, 0},    \
        {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_TEMPERATURE_INST_NO + 7), 0, 0},
#define SENSOR_TEMPERATURE_SVC_SIGN_NO   3
#else
#define CHAR_SENSOR_TEMPERATURE_SVC_A5_SIGN_TABLE
#define SENSOR_TEMPERATURE_SVC_SIGN_NO   0
#endif

#define SENSOR_TEMPERATURE_NO_DELTA SENSORTEMPERATURE_CURRENTTEMPERATURE_IND_NO \
                                    +SENSORTEMPERATURE_STATUSACTIVE_IND_NO  \
                                    +SENSORTEMPERATURE_STATUSFAULT_IND_NO   \
                                    +SENSORTEMPERATURE_STATUSTAMPERED_IND_NO    \
                                    +SENSORTEMPERATURE_STATUSLOWBATTERY_IND_NO

#define SENSOR_TEMPERATURE_NO       9 +SENSORTEMPERATURE_STATUSACTIVE_NO \
									+SENSORTEMPERATURE_STATUSFAULT_NO \
									+SENSORTEMPERATURE_STATUSLOWBATTERY_NO \
									+SENSORTEMPERATURE_STATUSTAMPERED_NO \
									+SENSOR_TEMPERATURE_SVC_SIGN_NO
									
#define SENSOR_TEMPERATURE_HANDLE_NUM (SENSOR_TEMPERATURE_NO + SENSOR_TEMPERATURE_NO_DELTA)


#define HOMEKIT_SENSOR_TEMPERATURE_SERVICE_ATT_TABLE    \
    {SENSOR_TEMPERATURE_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(sensor_temperatureServiceUUID),sizeof(sensor_temperatureServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(sensor_temperatureServiceUUID), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(insts + SENSOR_TEMPERATURE_INST_NO), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_NAME, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(hSensor_TemperatureServiceName), 0, (u8*)(nameUUID), (u8*)(hSensor_TemperatureServiceName), 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + SENSOR_TEMPERATURE_INST_NO + 1), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Temperature_CurrentTemperature_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Temperature_CurrentTemperature),0, (u8*)(charCurrentTemperatureUUID), (u8*)&hSensor_Temperature_CurrentTemperature, 0, hSensor_Temperature_CurrentTemperatureReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_TEMPERATURE_INST_NO + 2), 0, 0},    \
            SENSORTEMPERATURE_CURRENTTEMPERATURE_IND_DESC \
			CHAR_SENSORTEMPERATURE_STATUSACTIVE_ATT_TABLE \
			CHAR_SENSORTEMPERATURE_STATUSFAULT_ATT_TABLE \
			CHAR_SENSORTEMPERATURE_STATUSLOWBATTERY_ATT_TABLE \
			CHAR_SENSORTEMPERATURE_STATUSTAMPERED_ATT_TABLE	\
        	CHAR_SENSOR_TEMPERATURE_SVC_A5_SIGN_TABLE


#define SENSORTEMPERATURE_HANDLE_COMMON_NUM     8

#define SENSORTEMPERATURE_HANDLE_NUM_CURRENTTEMPERATURE SENSOR_TEMPERATURE_HANDLE_START_NUM +SENSORTEMPERATURE_HANDLE_COMMON_NUM
#define SENSORTEMPERATURE_HANDLE_NUM_STATUSACTIVE       SENSORTEMPERATURE_HANDLE_NUM_CURRENTTEMPERATURE +SENSORTEMPERATURE_CURRENTTEMPERATURE_NO +SENSORTEMPERATURE_CURRENTTEMPERATURE_IND_NO
#define SENSORTEMPERATURE_HANDLE_NUM_STATUSFAULT        SENSORTEMPERATURE_HANDLE_NUM_STATUSACTIVE +SENSORTEMPERATURE_STATUSACTIVE_NO +SENSORTEMPERATURE_STATUSACTIVE_IND_NO
#define SENSORTEMPERATURE_HANDLE_NUM_STATUSTAMPERED     SENSORTEMPERATURE_HANDLE_NUM_STATUSFAULT +SENSORTEMPERATURE_STATUSFAULT_NO +SENSORTEMPERATURE_STATUSFAULT_IND_NO
#define SENSORTEMPERATURE_HANDLE_NUM_STATUSLOWBATTERY   SENSORTEMPERATURE_HANDLE_NUM_STATUSTAMPERED +SENSORTEMPERATURE_STATUSTAMPERED_NO +SENSORTEMPERATURE_STATUSTAMPERED_IND_NO
#define SENSORTEMPERATURE_HANDLE_NUM_THE_END


#define SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T_SENSOR_TEMPERATURE_CURRENTTEMPERATURE       u32 Sensor_Temperature_CurrentTemperature;    /* float */
#define SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T_SENSOR_TEMPERATURE_CURRENTTEMPERATURE_STORE     STORE_CHAR(Sensor_Temperature_CurrentTemperature,hSensor_Temperature_CurrentTemperature)
#define SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T_SENSOR_TEMPERATURE_CURRENTTEMPERATURE_RESTORE   RESTORE_CHAR(Sensor_Temperature_CurrentTemperature,hSensor_Temperature_CurrentTemperature)

#define SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T_SENSOR_TEMPERATURE_STATUSACTIVE             u32 Sensor_Temperature_StatusActive:1;    /* 0~1 */
#define SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T_SENSOR_TEMPERATURE_STATUSACTIVE_STORE           STORE_CHAR(Sensor_Temperature_StatusActive,hSensor_Temperature_StatusActive)
#define SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T_SENSOR_TEMPERATURE_STATUSACTIVE_RESTORE         RESTORE_CHAR(Sensor_Temperature_StatusActive,hSensor_Temperature_StatusActive)

#define SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T_SENSOR_TEMPERATURE_STATUSFAULT              u32 Sensor_Temperature_StatusFault:1;    /* 0~1 */
#define SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T_SENSOR_TEMPERATURE_STATUSFAULT_STORE            STORE_CHAR(Sensor_Temperature_StatusFault,hSensor_Temperature_StatusFault)
#define SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T_SENSOR_TEMPERATURE_STATUSFAULT_RESTORE          RESTORE_CHAR(Sensor_Temperature_StatusFault,hSensor_Temperature_StatusFault)

#define SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T_SENSOR_TEMPERATURE_STATUSTAMPERED           u32 Sensor_Temperature_StatusTampered:1;    /* 0~1 */
#define SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T_SENSOR_TEMPERATURE_STATUSTAMPERED_STORE         STORE_CHAR(Sensor_Temperature_StatusTampered,hSensor_Temperature_StatusTampered)
#define SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T_SENSOR_TEMPERATURE_STATUSTAMPERED_RESTORE       RESTORE_CHAR(Sensor_Temperature_StatusTampered,hSensor_Temperature_StatusTampered)

#define SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T_SENSOR_TEMPERATURE_STATUSLOWBATTERY         u32 Sensor_Temperature_StatusLowBattery:1;    /* 0~1 */
#define SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T_SENSOR_TEMPERATURE_STATUSLOWBATTERY_STORE       STORE_CHAR(Sensor_Temperature_StatusLowBattery,hSensor_Temperature_StatusLowBattery)
#define SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T_SENSOR_TEMPERATURE_STATUSLOWBATTERY_RESTORE     RESTORE_CHAR(Sensor_Temperature_StatusLowBattery,hSensor_Temperature_StatusLowBattery)

#define SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T   \
                    SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T_SENSOR_TEMPERATURE_CURRENTTEMPERATURE  \
                    SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T_SENSOR_TEMPERATURE_STATUSACTIVE   \
                    SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T_SENSOR_TEMPERATURE_STATUSFAULT  \
                    SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T_SENSOR_TEMPERATURE_STATUSTAMPERED   \
                    SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T_SENSOR_TEMPERATURE_STATUSLOWBATTERY

#define SENSOR_TEMPERATURE_STORE_CHAR_TO_FLASH  \
                    SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T_SENSOR_TEMPERATURE_CURRENTTEMPERATURE_STORE    \
                    SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T_SENSOR_TEMPERATURE_STATUSACTIVE_STORE \
                    SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T_SENSOR_TEMPERATURE_STATUSFAULT_STORE    \
                    SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T_SENSOR_TEMPERATURE_STATUSTAMPERED_STORE \
                    SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T_SENSOR_TEMPERATURE_STATUSLOWBATTERY_STORE

#define SENSOR_TEMPERATURE_RESTORE_CHAR_FROM_FLASH  \
                    SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T_SENSOR_TEMPERATURE_CURRENTTEMPERATURE_RESTORE  \
                    SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T_SENSOR_TEMPERATURE_STATUSACTIVE_RESTORE   \
                    SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T_SENSOR_TEMPERATURE_STATUSFAULT_RESTORE  \
                    SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T_SENSOR_TEMPERATURE_STATUSTAMPERED_RESTORE   \
                    SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T_SENSOR_TEMPERATURE_STATUSLOWBATTERY_RESTORE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN unsigned int  hSensor_Temperature_CurrentTemperature;
EXTERN unsigned char hSensor_Temperature_StatusActive;
EXTERN unsigned char hSensor_Temperature_StatusFault;
EXTERN unsigned char hSensor_Temperature_StatusTampered;
EXTERN unsigned char hSensor_Temperature_StatusLowBattery;

EXTERN unsigned short ccc_hSensorTemperature_CurrentTemperature;
EXTERN unsigned short ccc_hSensorTemperature_StatusActive;
EXTERN unsigned short ccc_hSensorTemperature_StatusFault;
EXTERN unsigned short ccc_hSensorTemperature_StatusTampered;
EXTERN unsigned short ccc_hSensorTemperature_StatusLowBattery;

#if SENSORTEMPERATURE_CURRENTTEMPERATURE_BRC_ENABLE
    #define BC_PARA_SENSORTEMPERATURE_CURRENTTEMPERATURE ((u8*)&bc_para_hSensorTemperature_CurrentTemperature)
    EXTERN unsigned char bc_para_hSensorTemperature_CurrentTemperature;
#else
    #define BC_PARA_SENSORTEMPERATURE_CURRENTTEMPERATURE 0
#endif

#if SENSORTEMPERATURE_STATUSACTIVE_BRC_ENABLE
    #define BC_PARA_SENSORTEMPERATURE_STATUSACTIVE ((u8*)&bc_para_hSensorTemperature_StatusActive)
    EXTERN unsigned char bc_para_hSensorTemperature_StatusActive;
#else
    #define BC_PARA_SENSORTEMPERATURE_STATUSACTIVE 0
#endif

#if SENSORTEMPERATURE_STATUSFAULT_BRC_ENABLE
    #define BC_PARA_SENSORTEMPERATURE_STATUSFAULT ((u8*)&bc_para_hSensorTemperature_StatusFault)
    EXTERN unsigned char bc_para_hSensorTemperature_StatusFault;
#else
    #define BC_PARA_SENSORTEMPERATURE_STATUSFAULT 0
#endif

#if SENSORTEMPERATURE_STATUSTAMPERED_BRC_ENABLE
    #define BC_PARA_SENSORTEMPERATURE_STATUSTAMPERED ((u8*)&bc_para_hSensorTemperature_StatusTampered)
    EXTERN unsigned char bc_para_hSensorTemperature_StatusTampered;
#else
    #define BC_PARA_SENSORTEMPERATURE_STATUSTAMPERED 0
#endif

#if SENSORTEMPERATURE_STATUSLOWBATTERY_BRC_ENABLE
    #define BC_PARA_SENSORTEMPERATURE_STATUSLOWBATTERY ((u8*)&bc_para_hSensorTemperature_StatusLowBattery)
    EXTERN unsigned char bc_para_hSensorTemperature_StatusLowBattery;
#else
    #define BC_PARA_SENSORTEMPERATURE_STATUSLOWBATTERY 0
#endif

#if SENSOR_TEMPERATURE_LINKED_SERVICES_ENABLE
	#define SENSOR_TEMPERATURE_LINKED_SVCS			hSensorTemperature_linked_svcs
	#define SENSOR_TEMPERATURE_LINKED_SVCS_LENGTH 	(sizeof(hSensorTemperature_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * hSensorTemperature_linked_svcs[];
#else
	#define SENSOR_TEMPERATURE_LINKED_SVCS			0
	#define SENSOR_TEMPERATURE_LINKED_SVCS_LENGTH 	0
#endif

EXTERN const u8 hSensor_TemperatureServiceName[25];

EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Temperature_CurrentTemperature_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Temperature_StatusActive_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Temperature_StatusFault_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Temperature_StatusTampered_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Temperature_StatusLowBattery_R_S;


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN int hSensor_Temperature_CurrentTemperatureReadCallback(void *pp);
EXTERN int hSensor_Temperature_StatusActiveReadCallback(void *pp);
EXTERN int hSensor_Temperature_StatusFaultReadCallback(void *pp);
EXTERN int hSensor_Temperature_StatusTamperedReadCallback(void *pp);
EXTERN int hSensor_Temperature_StatusLowBatteryReadCallback(void *pp);
EXTERN int ccc_hSensorTemperature_CurrentTemperatureWriteCB(void *pp);
EXTERN int ccc_hSensorTemperature_StatusActiveWriteCB(void *pp);
EXTERN int ccc_hSensorTemperature_StatusFaultWriteCB(void *pp);
EXTERN int ccc_hSensorTemperature_StatusTamperedWriteCB(void *pp);
EXTERN int ccc_hSensorTemperature_StatusLowBatteryWriteCB(void *pp);


#endif /* _HK_SENSOR_TEMPERATURE_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
