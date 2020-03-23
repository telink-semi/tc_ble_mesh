/********************************************************************************************************
 * @file     hk_SensorLightService.h
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
#ifndef _HK_SENSOR_LIGHT_SERVICE_H_
#define _HK_SENSOR_LIGHT_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */
//-#include "../../homekit_src/homekit_inc.h"


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_SENSOR_LIGHT_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif


#define CHAR_SENSOR_LIGHT_SVC_SIGN_ENABLE      0
#if CHAR_SENSOR_LIGHT_SVC_SIGN_ENABLE
    #define SENSOR_LIGHT_AS_PRIMARY_SERVICE		    0
    #define SENSOR_LIGHT_IS_HIDDEN_SERVICE	     	0
    #define SENSOR_LIGHT_LINKED_SERVICES_ENABLE		0
    EXTERN const hap_characteristic_desc_t hap_desc_char_sensor_light_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_sensor_light_A5_R_S;
    EXTERN unsigned char VAR_SENSOR_LIGHT_UNKOWN;
#else
    #define SENSOR_LIGHT_AS_PRIMARY_SERVICE    		0
    #define SENSOR_LIGHT_IS_HIDDEN_SERVICE     		0
    #define SENSOR_LIGHT_LINKED_SERVICES_ENABLE		0
#endif


#define CHAR_SENSORLIGHT_STATUSACTIVE_ENABLE       0
#define CHAR_SENSORLIGHT_STATUSFAULT_ENABLE        0
#define CHAR_SENSORLIGHT_STATUSTAMPERED_ENABLE     0
#define CHAR_SENSORLIGHT_STATUSLOWBATTERY_ENABLE   0

#define SENSORLIGHT_CURRENTAMBIENTLIGHTLEVEL_IND_ENABLE     0
#define SENSORLIGHT_STATUSACTIVE_IND_ENABLE                 0
#define SENSORLIGHT_STATUSFAULT_IND_ENABLE                  0
#define SENSORLIGHT_STATUSTAMPERED_IND_ENABLE               0
#define SENSORLIGHT_STATUSLOWBATTERY_IND_ENABLE             0

#define SENSORLIGHT_CURRENTAMBIENTLIGHTLEVEL_BRC_ENABLE     0
#define SENSORLIGHT_STATUSACTIVE_BRC_ENABLE                 0
#define SENSORLIGHT_STATUSFAULT_BRC_ENABLE                  0
#define SENSORLIGHT_STATUSTAMPERED_BRC_ENABLE               0
#define SENSORLIGHT_STATUSLOWBATTERY_BRC_ENABLE             0


#define SENSORLIGHT_CURRENTAMBIENTLIGHTLEVEL_NO 3
#if SENSORLIGHT_CURRENTAMBIENTLIGHTLEVEL_IND_ENABLE
    #define SENSORLIGHT_CURRENTAMBIENTLIGHTLEVEL_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorLight_CurrentAmbientLightLevel), ccc_hSensorLight_CurrentAmbientLightLevelWriteCB, 0},
    #define SENSORLIGHT_CURRENTAMBIENTLIGHTLEVEL_IND_NO  1
#else
    #define SENSORLIGHT_CURRENTAMBIENTLIGHTLEVEL_IND_DESC
    #define SENSORLIGHT_CURRENTAMBIENTLIGHTLEVEL_IND_NO  0
#endif


#if CHAR_SENSORLIGHT_STATUSACTIVE_ENABLE
    #define SENSORLIGHT_STATUSACTIVE_NO 3
    #define CHAR_SENSORLIGHT_STATUSACTIVE_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Light_StatusActive_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Light_StatusActive),0, (u8*)(charStatusActiveUUID), (u8*)&hSensor_Light_StatusActive, 0, hSensor_Light_StatusActiveReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_LIGHT_INST_NO + 3), 0, 0},    \
            SENSORLIGHT_STATUSACTIVE_IND_DESC

	#if SENSORLIGHT_STATUSACTIVE_IND_ENABLE
	    #define SENSORLIGHT_STATUSACTIVE_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorLight_StatusActive), ccc_hSensorLight_StatusActiveWriteCB, 0},
	    #define SENSORLIGHT_STATUSACTIVE_IND_NO  1
	#else
	    #define SENSORLIGHT_STATUSACTIVE_IND_DESC
	    #define SENSORLIGHT_STATUSACTIVE_IND_NO  0
	#endif
#else
    #define SENSORLIGHT_STATUSACTIVE_NO 0
    #define CHAR_SENSORLIGHT_STATUSACTIVE_ATT_TABLE
    #define SENSORLIGHT_STATUSACTIVE_IND_NO  0
#endif

#if CHAR_SENSORLIGHT_STATUSFAULT_ENABLE
    #define SENSORLIGHT_STATUSFAULT_NO 3
    #define CHAR_SENSORLIGHT_STATUSFAULT_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Light_StatusFault_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Light_StatusFault),0, (u8*)(charStatusFaultUUID), (u8*)&hSensor_Light_StatusFault, 0, hSensor_Light_StatusFaultReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_LIGHT_INST_NO + 4), 0, 0},    \
            SENSORLIGHT_STATUSFAULT_IND_DESC

	#if SENSORLIGHT_STATUSFAULT_IND_ENABLE
	    #define SENSORLIGHT_STATUSFAULT_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorLight_StatusFault), ccc_hSensorLight_StatusFaultWriteCB, 0},
	    #define SENSORLIGHT_STATUSFAULT_IND_NO  1
	#else
	    #define SENSORLIGHT_STATUSFAULT_IND_DESC
	    #define SENSORLIGHT_STATUSFAULT_IND_NO  0
	#endif
#else
    #define SENSORLIGHT_STATUSFAULT_NO 0
    #define CHAR_SENSORLIGHT_STATUSFAULT_ATT_TABLE
    #define SENSORLIGHT_STATUSFAULT_IND_NO  0
#endif


#if CHAR_SENSORLIGHT_STATUSTAMPERED_ENABLE
    #define SENSORLIGHT_STATUSTAMPERED_NO 3
    #define CHAR_SENSORLIGHT_STATUSTAMPERED_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Light_StatusTampered_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Light_StatusTampered),0, (u8*)(charStatusTamperedUUID), (u8*)&hSensor_Light_StatusTampered, 0, hSensor_Light_StatusTamperedReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_LIGHT_INST_NO + 5), 0, 0},    \
            SENSORLIGHT_STATUSTAMPERED_IND_DESC

	#if SENSORLIGHT_STATUSTAMPERED_IND_ENABLE
	    #define SENSORLIGHT_STATUSTAMPERED_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorLight_StatusTampered), ccc_hSensorLight_StatusTamperedWriteCB, 0},
	    #define SENSORLIGHT_STATUSTAMPERED_IND_NO  1
	#else
	    #define SENSORLIGHT_STATUSTAMPERED_IND_DESC
	    #define SENSORLIGHT_STATUSTAMPERED_IND_NO  0
	#endif
#else
    #define SENSORLIGHT_STATUSTAMPERED_NO 0
    #define CHAR_SENSORLIGHT_STATUSTAMPERED_ATT_TABLE
    #define SENSORLIGHT_STATUSTAMPERED_IND_NO  0
#endif

#if CHAR_SENSORLIGHT_STATUSLOWBATTERY_ENABLE
    #define SENSORLIGHT_STATUSLOWBATTERY_NO 3
    #define CHAR_SENSORLIGHT_STATUSLOWBATTERY_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Light_StatusLowBattery_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Light_StatusLowBattery),0, (u8*)(charStatusLowBatteryUUID), (u8*)&hSensor_Light_StatusLowBattery, 0, hSensor_Light_StatusLowBatteryReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_LIGHT_INST_NO + 6), 0, 0},    \
            SENSORLIGHT_STATUSLOWBATTERY_IND_DESC

	#if SENSORLIGHT_STATUSLOWBATTERY_IND_ENABLE
	    #define SENSORLIGHT_STATUSLOWBATTERY_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorLight_StatusLowBattery), ccc_hSensorLight_StatusLowBatteryWriteCB, 0},
	    #define SENSORLIGHT_STATUSLOWBATTERY_IND_NO  1
	#else
	    #define SENSORLIGHT_STATUSLOWBATTERY_IND_DESC
	    #define SENSORLIGHT_STATUSLOWBATTERY_IND_NO  0
	#endif
#else
    #define SENSORLIGHT_STATUSLOWBATTERY_NO 0
    #define CHAR_SENSORLIGHT_STATUSLOWBATTERY_ATT_TABLE
    #define SENSORLIGHT_STATUSLOWBATTERY_IND_NO  0
#endif


#if CHAR_SENSOR_LIGHT_SVC_SIGN_ENABLE
#define CHAR_SENSOR_LIGHT_SVC_A5_SIGN_TABLE   \
    {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_sensor_light_A5_R_S, &hap_desc_service_sensor_light_A5_R_S},  \
    {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_SENSOR_LIGHT_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_SENSOR_LIGHT_UNKOWN, 0, 0},    \
        {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_LIGHT_INST_NO + 7), 0, 0},
#define SENSOR_LIGHT_SVC_SIGN_NO   3
#else
#define CHAR_SENSOR_LIGHT_SVC_A5_SIGN_TABLE
#define SENSOR_LIGHT_SVC_SIGN_NO   0
#endif

#define SENSOR_LIGHT_NO_DELTA   SENSORLIGHT_CURRENTAMBIENTLIGHTLEVEL_IND_NO  \
                                +SENSORLIGHT_STATUSACTIVE_IND_NO    \
                                +SENSORLIGHT_STATUSFAULT_IND_NO \
                                +SENSORLIGHT_STATUSTAMPERED_IND_NO  \
                                +SENSORLIGHT_STATUSLOWBATTERY_IND_NO

#define SENSOR_LIGHT_NO         9 +SENSORLIGHT_STATUSACTIVE_NO	\
								+SENSORLIGHT_STATUSFAULT_NO \
								+SENSORLIGHT_STATUSTAMPERED_NO	\
								+SENSORLIGHT_STATUSLOWBATTERY_NO \
								+SENSOR_LIGHT_SVC_SIGN_NO
								
#define SENSOR_LIGHT_HANDLE_NUM (SENSOR_LIGHT_NO + SENSOR_LIGHT_NO_DELTA)


#define HOMEKIT_SENSOR_LIGHT_SERVICE_ATT_TABLE    \
    {SENSOR_LIGHT_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(sensor_lightServiceUUID),sizeof(sensor_lightServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(sensor_lightServiceUUID), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(insts + SENSOR_LIGHT_INST_NO), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_NAME, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(hSensor_LightServiceName), 0, (u8*)(nameUUID), (u8*)(hSensor_LightServiceName), 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + SENSOR_LIGHT_INST_NO + 1), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Light_CurrentAmbientLightLevel_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Light_CurrentAmbientLightLevel),0, (u8*)(charCurrentAmbientLightLevelUUID), (u8*)&hSensor_Light_CurrentAmbientLightLevel, 0, hSensor_Light_CurrentAmbientLightLevelReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_LIGHT_INST_NO + 2), 0, 0},    \
            SENSORLIGHT_CURRENTAMBIENTLIGHTLEVEL_IND_DESC \
            CHAR_SENSORLIGHT_STATUSACTIVE_ATT_TABLE \
            CHAR_SENSORLIGHT_STATUSFAULT_ATT_TABLE \
            CHAR_SENSORLIGHT_STATUSTAMPERED_ATT_TABLE \
            CHAR_SENSORLIGHT_STATUSLOWBATTERY_ATT_TABLE \
        	CHAR_SENSOR_LIGHT_SVC_A5_SIGN_TABLE


#define SENSORLIGHT_HANDLE_COMMON_NUM     8

#define SENSORLIGHT_HANDLE_NUM_CURRENTAMBIENTLIGHTLEVEL SENSOR_LIGHT_HANDLE_START_NUM +SENSORLIGHT_HANDLE_COMMON_NUM
#define SENSORLIGHT_HANDLE_NUM_STATUSACTIVE             SENSORLIGHT_HANDLE_NUM_CURRENTAMBIENTLIGHTLEVEL +SENSORLIGHT_CURRENTAMBIENTLIGHTLEVEL_NO +SENSORLIGHT_CURRENTAMBIENTLIGHTLEVEL_IND_NO
#define SENSORLIGHT_HANDLE_NUM_STATUSFAULT              SENSORLIGHT_HANDLE_NUM_STATUSACTIVE +SENSORLIGHT_STATUSACTIVE_NO +SENSORLIGHT_STATUSACTIVE_IND_NO
#define SENSORLIGHT_HANDLE_NUM_STATUSTAMPERED           SENSORLIGHT_HANDLE_NUM_STATUSFAULT +SENSORLIGHT_STATUSFAULT_NO +SENSORLIGHT_STATUSFAULT_IND_NO
#define SENSORLIGHT_HANDLE_NUM_STATUSLOWBATTERY         SENSORLIGHT_HANDLE_NUM_STATUSTAMPERED +SENSORLIGHT_STATUSTAMPERED_NO +SENSORLIGHT_STATUSTAMPERED_IND_NO
#define SENSORLIGHT_HANDLE_NUM_THE_END


#define SENSOR_LIGHT_STORE_CHAR_VALUE_T_SENSOR_LIGHT_CURRENTAMBIENTLIGHTLEVEL           u32 Sensor_Light_CurrentAmbientLightLevel;  /* float */
#define SENSOR_LIGHT_STORE_CHAR_VALUE_T_SENSOR_LIGHT_CURRENTAMBIENTLIGHTLEVEL_STORE         STORE_CHAR(Sensor_Light_CurrentAmbientLightLevel,hSensor_Light_CurrentAmbientLightLevel)
#define SENSOR_LIGHT_STORE_CHAR_VALUE_T_SENSOR_LIGHT_CURRENTAMBIENTLIGHTLEVEL_RESTORE       RESTORE_CHAR(Sensor_Light_CurrentAmbientLightLevel,hSensor_Light_CurrentAmbientLightLevel)

#define SENSOR_LIGHT_STORE_CHAR_VALUE_T_SENSOR_LIGHT_STATUSACTIVE                       u32 Sensor_Light_StatusActive:1;    /* 0~1 */
#define SENSOR_LIGHT_STORE_CHAR_VALUE_T_SENSOR_LIGHT_STATUSACTIVE_STORE                     STORE_CHAR(Sensor_Light_StatusActive,hSensor_Light_StatusActive)
#define SENSOR_LIGHT_STORE_CHAR_VALUE_T_SENSOR_LIGHT_STATUSACTIVE_RESTORE                   RESTORE_CHAR(Sensor_Light_StatusActive,hSensor_Light_StatusActive)

#define SENSOR_LIGHT_STORE_CHAR_VALUE_T_SENSOR_LIGHT_STATUSFAULT                        u32 Sensor_Light_StatusFault:1;    /* 0~1 */
#define SENSOR_LIGHT_STORE_CHAR_VALUE_T_SENSOR_LIGHT_STATUSFAULT_STORE                      STORE_CHAR(Sensor_Light_StatusFault,hSensor_Light_StatusFault)
#define SENSOR_LIGHT_STORE_CHAR_VALUE_T_SENSOR_LIGHT_STATUSFAULT_RESTORE                    RESTORE_CHAR(Sensor_Light_StatusFault,hSensor_Light_StatusFault)

#define SENSOR_LIGHT_STORE_CHAR_VALUE_T_SENSOR_LIGHT_STATUSTAMPERED                     u32 Sensor_Light_StatusTampered:1;    /* 0~1 */
#define SENSOR_LIGHT_STORE_CHAR_VALUE_T_SENSOR_LIGHT_STATUSTAMPERED_STORE                   STORE_CHAR(Sensor_Light_StatusTampered,hSensor_Light_StatusTampered)
#define SENSOR_LIGHT_STORE_CHAR_VALUE_T_SENSOR_LIGHT_STATUSTAMPERED_RESTORE                 RESTORE_CHAR(Sensor_Light_StatusTampered,hSensor_Light_StatusTampered)

#define SENSOR_LIGHT_STORE_CHAR_VALUE_T_SENSOR_LIGHT_STATUSLOWBATTERY                   u32 Sensor_Light_StatusLowBattery:1;    /* 0~1 */
#define SENSOR_LIGHT_STORE_CHAR_VALUE_T_SENSOR_LIGHT_STATUSLOWBATTERY_STORE                 STORE_CHAR(Sensor_Light_StatusLowBattery,hSensor_Light_StatusLowBattery)
#define SENSOR_LIGHT_STORE_CHAR_VALUE_T_SENSOR_LIGHT_STATUSLOWBATTERY_RESTORE               RESTORE_CHAR(Sensor_Light_StatusLowBattery,hSensor_Light_StatusLowBattery)

#define SENSOR_LIGHT_STORE_CHAR_VALUE_T   \
                    SENSOR_LIGHT_STORE_CHAR_VALUE_T_SENSOR_LIGHT_CURRENTAMBIENTLIGHTLEVEL  \
                    SENSOR_LIGHT_STORE_CHAR_VALUE_T_SENSOR_LIGHT_STATUSACTIVE   \
                    SENSOR_LIGHT_STORE_CHAR_VALUE_T_SENSOR_LIGHT_STATUSFAULT  \
                    SENSOR_LIGHT_STORE_CHAR_VALUE_T_SENSOR_LIGHT_STATUSTAMPERED   \
                    SENSOR_LIGHT_STORE_CHAR_VALUE_T_SENSOR_LIGHT_STATUSLOWBATTERY

#define SENSOR_LIGHT_STORE_CHAR_TO_FLASH  \
                    SENSOR_LIGHT_STORE_CHAR_VALUE_T_SENSOR_LIGHT_CURRENTAMBIENTLIGHTLEVEL_STORE    \
                    SENSOR_LIGHT_STORE_CHAR_VALUE_T_SENSOR_LIGHT_STATUSACTIVE_STORE \
                    SENSOR_LIGHT_STORE_CHAR_VALUE_T_SENSOR_LIGHT_STATUSFAULT_STORE    \
                    SENSOR_LIGHT_STORE_CHAR_VALUE_T_SENSOR_LIGHT_STATUSTAMPERED_STORE \
                    SENSOR_LIGHT_STORE_CHAR_VALUE_T_SENSOR_LIGHT_STATUSLOWBATTERY_STORE

#define SENSOR_LIGHT_RESTORE_CHAR_FROM_FLASH  \
                    SENSOR_LIGHT_STORE_CHAR_VALUE_T_SENSOR_LIGHT_CURRENTAMBIENTLIGHTLEVEL_RESTORE  \
                    SENSOR_LIGHT_STORE_CHAR_VALUE_T_SENSOR_LIGHT_STATUSACTIVE_RESTORE   \
                    SENSOR_LIGHT_STORE_CHAR_VALUE_T_SENSOR_LIGHT_STATUSFAULT_RESTORE  \
                    SENSOR_LIGHT_STORE_CHAR_VALUE_T_SENSOR_LIGHT_STATUSTAMPERED_RESTORE   \
                    SENSOR_LIGHT_STORE_CHAR_VALUE_T_SENSOR_LIGHT_STATUSLOWBATTERY_RESTORE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN unsigned int  hSensor_Light_CurrentAmbientLightLevel;
EXTERN unsigned char hSensor_Light_StatusActive;
EXTERN unsigned char hSensor_Light_StatusFault;
EXTERN unsigned char hSensor_Light_StatusTampered;
EXTERN unsigned char hSensor_Light_StatusLowBattery;

EXTERN unsigned short ccc_hSensorLight_CurrentAmbientLightLevel;
EXTERN unsigned short ccc_hSensorLight_StatusActive;
EXTERN unsigned short ccc_hSensorLight_StatusFault;
EXTERN unsigned short ccc_hSensorLight_StatusTampered;
EXTERN unsigned short ccc_hSensorLight_StatusLowBattery;

#if SENSORLIGHT_CURRENTAMBIENTLIGHTLEVEL_BRC_ENABLE
    #define BC_PARA_SENSORLIGHT_CURRENTAMBIENTLIGHTLEVEL ((u8*)&bc_para_hSensorLight_CurrentAmbientLightLevel)
    EXTERN unsigned char bc_para_hSensorLight_CurrentAmbientLightLevel;
#else
    #define BC_PARA_SENSORLIGHT_CURRENTAMBIENTLIGHTLEVEL 0
#endif

#if SENSORLIGHT_STATUSACTIVE_BRC_ENABLE
    #define BC_PARA_SENSORLIGHT_STATUSACTIVE ((u8*)&bc_para_hSensorLight_StatusActive)
    EXTERN unsigned char bc_para_hSensorLight_StatusActive;
#else
    #define BC_PARA_SENSORLIGHT_STATUSACTIVE 0
#endif

#if SENSORLIGHT_STATUSFAULT_BRC_ENABLE
    #define BC_PARA_SENSORLIGHT_STATUSFAULT ((u8*)&bc_para_hSensorLight_StatusFault)
    EXTERN unsigned char bc_para_hSensorLight_StatusFault;
#else
    #define BC_PARA_SENSORLIGHT_STATUSFAULT 0
#endif

#if SENSORLIGHT_STATUSTAMPERED_BRC_ENABLE
    #define BC_PARA_SENSORLIGHT_STATUSTAMPERED ((u8*)&bc_para_hSensorLight_StatusTampered)
    EXTERN unsigned char bc_para_hSensorLight_StatusTampered;
#else
    #define BC_PARA_SENSORLIGHT_STATUSTAMPERED 0
#endif

#if SENSORLIGHT_STATUSLOWBATTERY_BRC_ENABLE
    #define BC_PARA_SENSORLIGHT_STATUSLOWBATTERY ((u8*)&bc_para_hSensorLight_StatusLowBattery)
    EXTERN unsigned char bc_para_hSensorLight_StatusLowBattery;
#else
    #define BC_PARA_SENSORLIGHT_STATUSLOWBATTERY 0
#endif

#if SENSOR_LIGHT_LINKED_SERVICES_ENABLE
	#define SENSOR_LIGHT_LINKED_SVCS			hSensorLight_linked_svcs
	#define SENSOR_LIGHT_LINKED_SVCS_LENGTH 	(sizeof(hSensorLight_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * hSensorLight_linked_svcs[];
#else
	#define SENSOR_LIGHT_LINKED_SVCS			0
	#define SENSOR_LIGHT_LINKED_SVCS_LENGTH 	0
#endif

EXTERN const u8 hSensor_LightServiceName[19];

EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Light_CurrentAmbientLightLevel_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Light_StatusActive_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Light_StatusFault_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Light_StatusTampered_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Light_StatusLowBattery_R_S;


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN int hSensor_Light_CurrentAmbientLightLevelReadCallback(void *pp);
EXTERN int hSensor_Light_StatusActiveReadCallback(void *pp);
EXTERN int hSensor_Light_StatusFaultReadCallback(void *pp);
EXTERN int hSensor_Light_StatusTamperedReadCallback(void *pp);
EXTERN int hSensor_Light_StatusLowBatteryReadCallback(void *pp);
EXTERN int ccc_hSensorLight_CurrentAmbientLightLevelWriteCB(void *pp);
EXTERN int ccc_hSensorLight_StatusActiveWriteCB(void *pp);
EXTERN int ccc_hSensorLight_StatusFaultWriteCB(void *pp);
EXTERN int ccc_hSensorLight_StatusTamperedWriteCB(void *pp);
EXTERN int ccc_hSensorLight_StatusLowBatteryWriteCB(void *pp);


#endif /* _HK_SENSOR_LIGHT_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
