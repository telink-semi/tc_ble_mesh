/********************************************************************************************************
 * @file     hk_SensorHumidityService.h
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
#ifndef _HK_SENSOR_HUMIDITY_SERVICE_H_
#define _HK_SENSOR_HUMIDITY_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */
//-#include "../../homekit_src/homekit_inc.h"


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_SENSOR_HUMIDITY_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif


#define CHAR_SENSOR_HUMIDITY_SVC_SIGN_ENABLE      0
#if CHAR_SENSOR_HUMIDITY_SVC_SIGN_ENABLE
    #define SENSOR_HUMIDITY_AS_PRIMARY_SERVICE    	0
    #define SENSOR_HUMIDITY_IS_HIDDEN_SERVICE     	0
    #define SENSOR_HUMIDITY_LINKED_SERVICES_ENABLE	0
    EXTERN const hap_characteristic_desc_t hap_desc_char_sensor_humidity_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_sensor_humidity_A5_R_S;
    EXTERN unsigned char VAR_SENSOR_HUMIDITY_UNKOWN;
#else
    #define SENSOR_HUMIDITY_AS_PRIMARY_SERVICE    	0
    #define SENSOR_HUMIDITY_IS_HIDDEN_SERVICE     	0
    #define SENSOR_HUMIDITY_LINKED_SERVICES_ENABLE	0
#endif

#define CHAR_SENSORHUMIDITY_STATUSACTIVE_ENABLE		0
#define CHAR_SENSORHUMIDITY_STATUSFAULT_ENABLE		0
#define CHAR_SENSORHUMIDITY_STATUSTAMPERED_ENABLE	0
#define CHAR_SENSORHUMIDITY_STATUSLOWBATTERY_ENABLE	0

#define SENSORHUMIDITY_CURRENTRELATIVEHUMIDITY_IND_ENABLE   0
#define SENSORHUMIDITY_STATUSACTIVE_IND_ENABLE              0
#define SENSORHUMIDITY_STATUSFAULT_IND_ENABLE               0
#define SENSORHUMIDITY_STATUSTAMPERED_IND_ENABLE            0
#define SENSORHUMIDITY_STATUSLOWBATTERY_IND_ENABLE          0

#define SENSORHUMIDITY_CURRENTRELATIVEHUMIDITY_BRC_ENABLE   0
#define SENSORHUMIDITY_STATUSACTIVE_BRC_ENABLE              0
#define SENSORHUMIDITY_STATUSFAULT_BRC_ENABLE               0
#define SENSORHUMIDITY_STATUSTAMPERED_BRC_ENABLE            0
#define SENSORHUMIDITY_STATUSLOWBATTERY_BRC_ENABLE          0


#define SENSORHUMIDITY_CURRENTRELATIVEHUMIDITY_NO 3
#if SENSORHUMIDITY_CURRENTRELATIVEHUMIDITY_IND_ENABLE
    #define SENSORHUMIDITY_CURRENTRELATIVEHUMIDITY_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorHumidity_CurrentRelativeHumidity), ccc_hSensorHumidity_CurrentRelativeHumidityWriteCB, 0},
    #define SENSORHUMIDITY_CURRENTRELATIVEHUMIDITY_IND_NO  1
#else
    #define SENSORHUMIDITY_CURRENTRELATIVEHUMIDITY_IND_DESC
    #define SENSORHUMIDITY_CURRENTRELATIVEHUMIDITY_IND_NO  0
#endif

#if CHAR_SENSORHUMIDITY_STATUSACTIVE_ENABLE
    #define SENSORHUMIDITY_STATUSACTIVE_NO 3
    #define CHAR_SENSORHUMIDITY_STATUSACTIVE_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Humidity_StatusActive_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Humidity_StatusActive),0, (u8*)(charStatusActiveUUID), (u8*)&hSensor_Humidity_StatusActive, 0, hSensor_Humidity_StatusActiveReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_HUMIDITY_INST_NO + 3), 0, 0},    \
            SENSORHUMIDITY_STATUSACTIVE_IND_DESC

	#if SENSORHUMIDITY_STATUSACTIVE_IND_ENABLE
	    #define SENSORHUMIDITY_STATUSACTIVE_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorHumidity_StatusActive), ccc_hSensorHumidity_StatusActiveWriteCB, 0},
	    #define SENSORHUMIDITY_STATUSACTIVE_IND_NO  1
	#else
	    #define SENSORHUMIDITY_STATUSACTIVE_IND_DESC
	    #define SENSORHUMIDITY_STATUSACTIVE_IND_NO  0
	#endif
#else
    #define SENSORHUMIDITY_STATUSACTIVE_NO 0
    #define CHAR_SENSORHUMIDITY_STATUSACTIVE_ATT_TABLE
    #define SENSORHUMIDITY_STATUSACTIVE_IND_NO  0
#endif


#if CHAR_SENSORHUMIDITY_STATUSFAULT_ENABLE
	#define SENSORHUMIDITY_STATUSFAULT_NO 3
    #define CHAR_SENSORHUMIDITY_STATUSFAULT_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Humidity_StatusFault_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Humidity_StatusFault),0, (u8*)(charStatusFaultUUID), (u8*)&hSensor_Humidity_StatusFault, 0, hSensor_Humidity_StatusFaultReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_HUMIDITY_INST_NO + 4), 0, 0},    \
            SENSORHUMIDITY_STATUSFAULT_IND_DESC

	#if SENSORHUMIDITY_STATUSFAULT_IND_ENABLE
	    #define SENSORHUMIDITY_STATUSFAULT_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorHumidity_StatusFault), ccc_hSensorHumidity_StatusFaultWriteCB, 0},
	    #define SENSORHUMIDITY_STATUSFAULT_IND_NO  1
	#else
	    #define SENSORHUMIDITY_STATUSFAULT_IND_DESC
	    #define SENSORHUMIDITY_STATUSFAULT_IND_NO  0
	#endif
#else
    #define SENSORHUMIDITY_STATUSFAULT_NO 0
    #define CHAR_SENSORHUMIDITY_STATUSFAULT_ATT_TABLE
    #define SENSORHUMIDITY_STATUSFAULT_IND_NO  0
#endif


#if CHAR_SENSORHUMIDITY_STATUSTAMPERED_ENABLE
    #define SENSORHUMIDITY_STATUSTAMPERED_NO 3
    #define CHAR_SENSORHUMIDITY_STATUSTAMPERED_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Humidity_StatusTampered_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Humidity_StatusTampered),0, (u8*)(charStatusTamperedUUID), (u8*)&hSensor_Humidity_StatusTampered, 0, hSensor_Humidity_StatusTamperedReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_HUMIDITY_INST_NO + 5), 0, 0},    \
            SENSORHUMIDITY_STATUSTAMPERED_IND_DESC

	#if SENSORHUMIDITY_STATUSTAMPERED_IND_ENABLE
	    #define SENSORHUMIDITY_STATUSTAMPERED_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorHumidity_StatusTampered), ccc_hSensorHumidity_StatusTamperedWriteCB, 0},
	    #define SENSORHUMIDITY_STATUSTAMPERED_IND_NO  1
	#else
	    #define SENSORHUMIDITY_STATUSTAMPERED_IND_DESC
	    #define SENSORHUMIDITY_STATUSTAMPERED_IND_NO  0
	#endif
#else
    #define SENSORHUMIDITY_STATUSTAMPERED_NO 0
    #define CHAR_SENSORHUMIDITY_STATUSTAMPERED_ATT_TABLE
    #define SENSORHUMIDITY_STATUSTAMPERED_IND_NO  0
#endif


#if CHAR_SENSORHUMIDITY_STATUSLOWBATTERY_ENABLE
    #define SENSORHUMIDITY_STATUSLOWBATTERY_NO 3
    #define CHAR_SENSORHUMIDITY_STATUSLOWBATTERY_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Humidity_StatusLowBattery_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Humidity_StatusLowBattery),0, (u8*)(charStatusLowBatteryUUID), (u8*)&hSensor_Humidity_StatusLowBattery, 0, hSensor_Humidity_StatusLowBatteryReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_HUMIDITY_INST_NO + 6), 0, 0},    \
            SENSORHUMIDITY_STATUSLOWBATTERY_IND_DESC

	#if SENSORHUMIDITY_STATUSLOWBATTERY_IND_ENABLE
	    #define SENSORHUMIDITY_STATUSLOWBATTERY_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorHumidity_StatusLowBattery), ccc_hSensorHumidity_StatusLowBatteryWriteCB, 0},
	    #define SENSORHUMIDITY_STATUSLOWBATTERY_IND_NO  1
	#else
	    #define SENSORHUMIDITY_STATUSLOWBATTERY_IND_DESC
	    #define SENSORHUMIDITY_STATUSLOWBATTERY_IND_NO  0
	#endif
#else
    #define SENSORHUMIDITY_STATUSLOWBATTERY_NO 0
    #define CHAR_SENSORHUMIDITY_STATUSLOWBATTERY_ATT_TABLE
    #define SENSORHUMIDITY_STATUSLOWBATTERY_IND_NO  0
#endif


#if CHAR_SENSOR_HUMIDITY_SVC_SIGN_ENABLE
#define CHAR_SENSOR_HUMIDITY_SVC_A5_SIGN_TABLE   \
    {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_sensor_humidity_A5_R_S, &hap_desc_service_sensor_humidity_A5_R_S},  \
    {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_SENSOR_HUMIDITY_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_SENSOR_HUMIDITY_UNKOWN, 0, 0},    \
        {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_HUMIDITY_INST_NO + 7), 0, 0},
#define SENSOR_HUMIDITY_SVC_SIGN_NO   3
#else
#define CHAR_SENSOR_HUMIDITY_SVC_A5_SIGN_TABLE
#define SENSOR_HUMIDITY_SVC_SIGN_NO   0
#endif

#define SENSOR_HUMIDITY_NO_DELTA    SENSORHUMIDITY_CURRENTRELATIVEHUMIDITY_IND_NO  \
                                    +SENSORHUMIDITY_STATUSACTIVE_IND_NO \
                                    +SENSORHUMIDITY_STATUSFAULT_IND_NO  \
                                    +SENSORHUMIDITY_STATUSTAMPERED_IND_NO   \
                                    +SENSORHUMIDITY_STATUSLOWBATTERY_IND_NO

#define SENSOR_HUMIDITY_NO          9 +SENSORHUMIDITY_STATUSACTIVE_NO \
									+SENSORHUMIDITY_STATUSFAULT_NO \
									+SENSORHUMIDITY_STATUSTAMPERED_NO \
									+SENSORHUMIDITY_STATUSLOWBATTERY_NO \
									+SENSOR_HUMIDITY_SVC_SIGN_NO
#define SENSOR_HUMIDITY_HANDLE_NUM (SENSOR_HUMIDITY_NO + SENSOR_HUMIDITY_NO_DELTA)


#define HOMEKIT_SENSOR_HUMIDITY_SERVICE_ATT_TABLE    \
    {SENSOR_HUMIDITY_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(sensor_humidityServiceUUID),sizeof(sensor_humidityServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(sensor_humidityServiceUUID), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(insts + SENSOR_HUMIDITY_INST_NO), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_NAME, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(hSensor_HumidityServiceName), 0, (u8*)(nameUUID), (u8*)(hSensor_HumidityServiceName), 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + SENSOR_HUMIDITY_INST_NO + 1), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Humidity_CurrentRelativeHumidity_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Humidity_CurrentRelativeHumidity),0, (u8*)(charCurrentRelativeHumidityUUID), (u8*)&hSensor_Humidity_CurrentRelativeHumidity, 0, hSensor_Humidity_CurrentRelativeHumidityReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_HUMIDITY_INST_NO + 2), 0, 0},    \
            SENSORHUMIDITY_CURRENTRELATIVEHUMIDITY_IND_DESC \
			CHAR_SENSORHUMIDITY_STATUSACTIVE_ATT_TABLE \
			CHAR_SENSORHUMIDITY_STATUSFAULT_ATT_TABLE \
			CHAR_SENSORHUMIDITY_STATUSTAMPERED_ATT_TABLE \
			CHAR_SENSORHUMIDITY_STATUSLOWBATTERY_ATT_TABLE \
        	CHAR_SENSOR_HUMIDITY_SVC_A5_SIGN_TABLE


#define SENSORHUMIDITY_HANDLE_COMMON_NUM     8

#define SENSORHUMIDITY_HANDLE_NUM_CURRENTRELATIVEHUMIDITY   SENSOR_HUMIDITY_HANDLE_START_NUM +SENSORHUMIDITY_HANDLE_COMMON_NUM
#define SENSORHUMIDITY_HANDLE_NUM_STATUSACTIVE              SENSORHUMIDITY_HANDLE_NUM_CURRENTRELATIVEHUMIDITY +SENSORHUMIDITY_CURRENTRELATIVEHUMIDITY_NO +SENSORHUMIDITY_CURRENTRELATIVEHUMIDITY_IND_NO
#define SENSORHUMIDITY_HANDLE_NUM_STATUSFAULT               SENSORHUMIDITY_HANDLE_NUM_STATUSACTIVE +SENSORHUMIDITY_STATUSACTIVE_NO +SENSORHUMIDITY_STATUSACTIVE_IND_NO
#define SENSORHUMIDITY_HANDLE_NUM_STATUSTAMPERED            SENSORHUMIDITY_HANDLE_NUM_STATUSFAULT +SENSORHUMIDITY_STATUSFAULT_NO +SENSORHUMIDITY_STATUSFAULT_IND_NO
#define SENSORHUMIDITY_HANDLE_NUM_STATUSLOWBATTERY          SENSORHUMIDITY_HANDLE_NUM_STATUSTAMPERED +SENSORHUMIDITY_STATUSTAMPERED_NO +SENSORHUMIDITY_STATUSTAMPERED_IND_NO
#define SENSORHUMIDITY_HANDLE_NUM_THE_END


#define SENSOR_HUMIDITY_STORE_CHAR_VALUE_T_SENSOR_HUMIDITY_CURRENTRELATIVEHUMIDITY           u32 Sensor_Humidity_CurrentRelativeHumidity;   /* float */
#define SENSOR_HUMIDITY_STORE_CHAR_VALUE_T_SENSOR_HUMIDITY_CURRENTRELATIVEHUMIDITY_STORE         STORE_CHAR(Sensor_Humidity_CurrentRelativeHumidity,hSensor_Humidity_CurrentRelativeHumidity)
#define SENSOR_HUMIDITY_STORE_CHAR_VALUE_T_SENSOR_HUMIDITY_CURRENTRELATIVEHUMIDITY_RESTORE       RESTORE_CHAR(Sensor_Humidity_CurrentRelativeHumidity,hSensor_Humidity_CurrentRelativeHumidity)

#define SENSOR_HUMIDITY_STORE_CHAR_VALUE_T_SENSOR_HUMIDITY_STATUSACTIVE                      u32 Sensor_Humidity_StatusActive:1;    /* 0~1 */
#define SENSOR_HUMIDITY_STORE_CHAR_VALUE_T_SENSOR_HUMIDITY_STATUSACTIVE_STORE                    STORE_CHAR(Sensor_Humidity_StatusActive,hSensor_Humidity_StatusActive)
#define SENSOR_HUMIDITY_STORE_CHAR_VALUE_T_SENSOR_HUMIDITY_STATUSACTIVE_RESTORE                  RESTORE_CHAR(Sensor_Humidity_StatusActive,hSensor_Humidity_StatusActive)

#define SENSOR_HUMIDITY_STORE_CHAR_VALUE_T_SENSOR_HUMIDITY_STATUSFAULT                       u32 Sensor_Humidity_StatusFault:1;    /* 0~1 */
#define SENSOR_HUMIDITY_STORE_CHAR_VALUE_T_SENSOR_HUMIDITY_STATUSFAULT_STORE                     STORE_CHAR(Sensor_Humidity_StatusFault,hSensor_Humidity_StatusFault)
#define SENSOR_HUMIDITY_STORE_CHAR_VALUE_T_SENSOR_HUMIDITY_STATUSFAULT_RESTORE                   RESTORE_CHAR(Sensor_Humidity_StatusFault,hSensor_Humidity_StatusFault)

#define SENSOR_HUMIDITY_STORE_CHAR_VALUE_T_SENSOR_HUMIDITY_STATUSTAMPERED                    u32 Sensor_Humidity_StatusTampered:1;    /* 0~1 */
#define SENSOR_HUMIDITY_STORE_CHAR_VALUE_T_SENSOR_HUMIDITY_STATUSTAMPERED_STORE                  STORE_CHAR(Sensor_Humidity_StatusTampered,hSensor_Humidity_StatusTampered)
#define SENSOR_HUMIDITY_STORE_CHAR_VALUE_T_SENSOR_HUMIDITY_STATUSTAMPERED_RESTORE                RESTORE_CHAR(Sensor_Humidity_StatusTampered,hSensor_Humidity_StatusTampered)

#define SENSOR_HUMIDITY_STORE_CHAR_VALUE_T_SENSOR_HUMIDITY_STATUSLOWBATTERY                  u32 Sensor_Humidity_StatusLowBattery:1;    /* 0~1 */
#define SENSOR_HUMIDITY_STORE_CHAR_VALUE_T_SENSOR_HUMIDITY_STATUSLOWBATTERY_STORE                STORE_CHAR(Sensor_Humidity_StatusLowBattery,hSensor_Humidity_StatusLowBattery)
#define SENSOR_HUMIDITY_STORE_CHAR_VALUE_T_SENSOR_HUMIDITY_STATUSLOWBATTERY_RESTORE              RESTORE_CHAR(Sensor_Humidity_StatusLowBattery,hSensor_Humidity_StatusLowBattery)

#define SENSOR_HUMIDITY_STORE_CHAR_VALUE_T   \
                    SENSOR_HUMIDITY_STORE_CHAR_VALUE_T_SENSOR_HUMIDITY_CURRENTRELATIVEHUMIDITY  \
                    SENSOR_HUMIDITY_STORE_CHAR_VALUE_T_SENSOR_HUMIDITY_STATUSACTIVE   \
                    SENSOR_HUMIDITY_STORE_CHAR_VALUE_T_SENSOR_HUMIDITY_STATUSFAULT  \
                    SENSOR_HUMIDITY_STORE_CHAR_VALUE_T_SENSOR_HUMIDITY_STATUSTAMPERED   \
                    SENSOR_HUMIDITY_STORE_CHAR_VALUE_T_SENSOR_HUMIDITY_STATUSLOWBATTERY

#define SENSOR_HUMIDITY_STORE_CHAR_TO_FLASH  \
                    SENSOR_HUMIDITY_STORE_CHAR_VALUE_T_SENSOR_HUMIDITY_CURRENTRELATIVEHUMIDITY_STORE    \
                    SENSOR_HUMIDITY_STORE_CHAR_VALUE_T_SENSOR_HUMIDITY_STATUSACTIVE_STORE \
                    SENSOR_HUMIDITY_STORE_CHAR_VALUE_T_SENSOR_HUMIDITY_STATUSFAULT_STORE    \
                    SENSOR_HUMIDITY_STORE_CHAR_VALUE_T_SENSOR_HUMIDITY_STATUSTAMPERED_STORE \
                    SENSOR_HUMIDITY_STORE_CHAR_VALUE_T_SENSOR_HUMIDITY_STATUSLOWBATTERY_STORE

#define SENSOR_HUMIDITY_RESTORE_CHAR_FROM_FLASH  \
                    SENSOR_HUMIDITY_STORE_CHAR_VALUE_T_SENSOR_HUMIDITY_CURRENTRELATIVEHUMIDITY_RESTORE  \
                    SENSOR_HUMIDITY_STORE_CHAR_VALUE_T_SENSOR_HUMIDITY_STATUSACTIVE_RESTORE   \
                    SENSOR_HUMIDITY_STORE_CHAR_VALUE_T_SENSOR_HUMIDITY_STATUSFAULT_RESTORE  \
                    SENSOR_HUMIDITY_STORE_CHAR_VALUE_T_SENSOR_HUMIDITY_STATUSTAMPERED_RESTORE   \
                    SENSOR_HUMIDITY_STORE_CHAR_VALUE_T_SENSOR_HUMIDITY_STATUSLOWBATTERY_RESTORE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN unsigned int  hSensor_Humidity_CurrentRelativeHumidity;
EXTERN unsigned char hSensor_Humidity_StatusActive;
EXTERN unsigned char hSensor_Humidity_StatusFault;
EXTERN unsigned char hSensor_Humidity_StatusTampered;
EXTERN unsigned char hSensor_Humidity_StatusLowBattery;

EXTERN unsigned short ccc_hSensorHumidity_CurrentRelativeHumidity;
EXTERN unsigned short ccc_hSensorHumidity_StatusActive;
EXTERN unsigned short ccc_hSensorHumidity_StatusFault;
EXTERN unsigned short ccc_hSensorHumidity_StatusTampered;
EXTERN unsigned short ccc_hSensorHumidity_StatusLowBattery;

#if SENSORHUMIDITY_CURRENTRELATIVEHUMIDITY_BRC_ENABLE
    #define BC_PARA_SENSORHUMIDITY_CURRENTRELATIVEHUMIDITY ((u8*)&bc_para_hSensorHumidity_CurrentRelativeHumidity)
    EXTERN unsigned char bc_para_hSensorHumidity_CurrentRelativeHumidity;
#else
    #define BC_PARA_SENSORHUMIDITY_CURRENTRELATIVEHUMIDITY 0
#endif

#if SENSORHUMIDITY_STATUSACTIVE_BRC_ENABLE
    #define BC_PARA_SENSORHUMIDITY_STATUSACTIVE ((u8*)&bc_para_hSensorHumidity_StatusActive)
    EXTERN unsigned char bc_para_hSensorHumidity_StatusActive;
#else
    #define BC_PARA_SENSORHUMIDITY_STATUSACTIVE 0
#endif

#if SENSORHUMIDITY_STATUSFAULT_BRC_ENABLE
    #define BC_PARA_SENSORHUMIDITY_STATUSFAULT ((u8*)&bc_para_hSensorHumidity_StatusFault)
    EXTERN unsigned char bc_para_hSensorHumidity_StatusFault;
#else
    #define BC_PARA_SENSORHUMIDITY_STATUSFAULT 0
#endif

#if SENSORHUMIDITY_STATUSTAMPERED_BRC_ENABLE
    #define BC_PARA_SENSORHUMIDITY_STATUSTAMPERED ((u8*)&bc_para_hSensorHumidity_StatusTampered)
    EXTERN unsigned char bc_para_hSensorHumidity_StatusTampered;
#else
    #define BC_PARA_SENSORHUMIDITY_STATUSTAMPERED 0
#endif

#if SENSORHUMIDITY_STATUSLOWBATTERY_BRC_ENABLE
    #define BC_PARA_SENSORHUMIDITY_STATUSLOWBATTERY ((u8*)&bc_para_hSensorHumidity_StatusLowBattery)
    EXTERN unsigned char bc_para_hSensorHumidity_StatusLowBattery;
#else
    #define BC_PARA_SENSORHUMIDITY_STATUSLOWBATTERY 0
#endif

#if SENSOR_HUMIDITY_LINKED_SERVICES_ENABLE
	#define SENSOR_HUMIDITY_LINKED_SVCS			hSensorHumidity_linked_svcs
	#define SENSOR_HUMIDITY_LINKED_SVCS_LENGTH 	(sizeof(hSensorHumidity_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * hSensorHumidity_linked_svcs[];
#else
	#define SENSOR_HUMIDITY_LINKED_SVCS			0
	#define SENSOR_HUMIDITY_LINKED_SVCS_LENGTH 	0
#endif

EXTERN const u8 hSensor_HumidityServiceName[22];

EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Humidity_CurrentRelativeHumidity_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Humidity_StatusActive_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Humidity_StatusFault_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Humidity_StatusTampered_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Humidity_StatusLowBattery_R_S;


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN int hSensor_Humidity_CurrentRelativeHumidityReadCallback(void *pp);
EXTERN int hSensor_Humidity_StatusActiveReadCallback(void *pp);
EXTERN int hSensor_Humidity_StatusFaultReadCallback(void *pp);
EXTERN int hSensor_Humidity_StatusTamperedReadCallback(void *pp);
EXTERN int hSensor_Humidity_StatusLowBatteryReadCallback(void *pp);
EXTERN int ccc_hSensorHumidity_CurrentRelativeHumidityWriteCB(void *pp);
EXTERN int ccc_hSensorHumidity_StatusActiveWriteCB(void *pp);
EXTERN int ccc_hSensorHumidity_StatusFaultWriteCB(void *pp);
EXTERN int ccc_hSensorHumidity_StatusTamperedWriteCB(void *pp);
EXTERN int ccc_hSensorHumidity_StatusLowBatteryWriteCB(void *pp);


#endif /* _HK_SENSOR_HUMIDITY_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
