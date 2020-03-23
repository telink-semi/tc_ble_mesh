/********************************************************************************************************
 * @file     hk_SensorSmokeService.h
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
#ifndef _HK_SENSOR_SMOKE_SERVICE_H_
#define _HK_SENSOR_SMOKE_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */
//-#include "../../homekit_src/homekit_inc.h"


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_SENSOR_SMOKE_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif


#define CHAR_SENSOR_SMOKE_SVC_SIGN_ENABLE      0
#if CHAR_SENSOR_SMOKE_SVC_SIGN_ENABLE
    #define SENSOR_SMOKE_AS_PRIMARY_SERVICE    		0
    #define SENSOR_SMOKE_IS_HIDDEN_SERVICE     		0
    #define SENSOR_SMOKE_LINKED_SERVICES_ENABLE		0
    EXTERN const hap_characteristic_desc_t hap_desc_char_sensor_smoke_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_sensor_smoke_A5_R_S;
    EXTERN unsigned char VAR_SENSOR_SMOKE_UNKOWN;
#else
    #define SENSOR_SMOKE_AS_PRIMARY_SERVICE    		0
    #define SENSOR_SMOKE_IS_HIDDEN_SERVICE     		0
    #define SENSOR_SMOKE_LINKED_SERVICES_ENABLE		0
#endif


#define CHAR_SENSORSMOKE_STATUSACTIVE_ENABLE       0
#define CHAR_SENSORSMOKE_STATUSFAULT_ENABLE        0
#define CHAR_SENSORSMOKE_STATUSTAMPERED_ENABLE     0
#define CHAR_SENSORSMOKE_STATUSLOWBATTERY_ENABLE   0

#define SENSORSMOKE_DETECTED_IND_ENABLE          0
#define SENSORSMOKE_STATUSACTIVE_IND_ENABLE      0
#define SENSORSMOKE_STATUSFAULT_IND_ENABLE       0
#define SENSORSMOKE_STATUSTAMPERED_IND_ENABLE    0
#define SENSORSMOKE_STATUSLOWBATTERY_IND_ENABLE  0

#define SENSORSMOKE_DETECTED_BRC_ENABLE          0
#define SENSORSMOKE_STATUSACTIVE_BRC_ENABLE      0
#define SENSORSMOKE_STATUSFAULT_BRC_ENABLE       0
#define SENSORSMOKE_STATUSTAMPERED_BRC_ENABLE    0
#define SENSORSMOKE_STATUSLOWBATTERY_BRC_ENABLE  0


#define SENSORSMOKE_DETECTED_NO 3
#if SENSORSMOKE_DETECTED_IND_ENABLE
    #define SENSORSMOKE_DETECTED_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorSmoke_Detected), ccc_hSensorSmoke_DetectedWriteCB, 0},
    #define SENSORSMOKE_DETECTED_IND_NO  1
#else
    #define SENSORSMOKE_DETECTED_IND_DESC
    #define SENSORSMOKE_DETECTED_IND_NO  0
#endif


#if CHAR_SENSORSMOKE_STATUSACTIVE_ENABLE
    #define SENSORSMOKE_STATUSACTIVE_NO 3
    #define CHAR_SENSORSMOKE_STATUSACTIVE_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Smoke_StatusActive_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Smoke_StatusActive),0, (u8*)(charStatusActiveUUID), (u8*)&hSensor_Smoke_StatusActive, 0, hSensor_Smoke_StatusActiveReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_SMOKE_INST_NO + 3), 0, 0},    \
            SENSORSMOKE_STATUSACTIVE_IND_DESC

	#if SENSORSMOKE_STATUSACTIVE_IND_ENABLE
	    #define SENSORSMOKE_STATUSACTIVE_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorSmoke_StatusActive), ccc_hSensorSmoke_StatusActiveWriteCB, 0},
	    #define SENSORSMOKE_STATUSACTIVE_IND_NO  1
	#else
	    #define SENSORSMOKE_STATUSACTIVE_IND_DESC
	    #define SENSORSMOKE_STATUSACTIVE_IND_NO  0
	#endif
#else
    #define SENSORSMOKE_STATUSACTIVE_NO 0
    #define CHAR_SENSORSMOKE_STATUSACTIVE_ATT_TABLE
    #define SENSORSMOKE_STATUSACTIVE_IND_NO  0
#endif

#if CHAR_SENSORSMOKE_STATUSFAULT_ENABLE
    #define SENSORSMOKE_STATUSFAULT_NO 3
    #define CHAR_SENSORSMOKE_STATUSFAULT_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Smoke_StatusFault_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Smoke_StatusFault),0, (u8*)(charStatusFaultUUID), (u8*)&hSensor_Smoke_StatusFault, 0, hSensor_Smoke_StatusFaultReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_SMOKE_INST_NO + 4), 0, 0},    \
            SENSORSMOKE_STATUSFAULT_IND_DESC

	#if SENSORSMOKE_STATUSFAULT_IND_ENABLE
	    #define SENSORSMOKE_STATUSFAULT_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorSmoke_StatusFault), ccc_hSensorSmoke_StatusFaultWriteCB, 0},
	    #define SENSORSMOKE_STATUSFAULT_IND_NO  1
	#else
	    #define SENSORSMOKE_STATUSFAULT_IND_DESC
	    #define SENSORSMOKE_STATUSFAULT_IND_NO  0
	#endif
#else
    #define SENSORSMOKE_STATUSFAULT_NO 0
    #define CHAR_SENSORSMOKE_STATUSFAULT_ATT_TABLE
    #define SENSORSMOKE_STATUSFAULT_IND_NO  0
#endif


#if CHAR_SENSORSMOKE_STATUSTAMPERED_ENABLE
    #define SENSORSMOKE_STATUSTAMPERED_NO 3
    #define CHAR_SENSORSMOKE_STATUSTAMPERED_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Smoke_StatusTampered_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Smoke_StatusTampered),0, (u8*)(charStatusTamperedUUID), (u8*)&hSensor_Smoke_StatusTampered, 0, hSensor_Smoke_StatusTamperedReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_SMOKE_INST_NO + 5), 0, 0},    \
            SENSORSMOKE_STATUSTAMPERED_IND_DESC

	#if SENSORSMOKE_STATUSTAMPERED_IND_ENABLE
	    #define SENSORSMOKE_STATUSTAMPERED_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorSmoke_StatusTampered), ccc_hSensorSmoke_StatusTamperedWriteCB, 0},
	    #define SENSORSMOKE_STATUSTAMPERED_IND_NO  1
	#else
	    #define SENSORSMOKE_STATUSTAMPERED_IND_DESC
	    #define SENSORSMOKE_STATUSTAMPERED_IND_NO  0
	#endif
#else
    #define SENSORSMOKE_STATUSTAMPERED_NO 0
    #define CHAR_SENSORSMOKE_STATUSTAMPERED_ATT_TABLE
    #define SENSORSMOKE_STATUSTAMPERED_IND_NO  0
#endif

#if CHAR_SENSORSMOKE_STATUSLOWBATTERY_ENABLE
    #define SENSORSMOKE_STATUSLOWBATTERY_NO 3
    #define CHAR_SENSORSMOKE_STATUSLOWBATTERY_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Smoke_StatusLowBattery_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Smoke_StatusLowBattery),0, (u8*)(charStatusLowBatteryUUID), (u8*)&hSensor_Smoke_StatusLowBattery, 0, hSensor_Smoke_StatusLowBatteryReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_SMOKE_INST_NO + 6), 0, 0},    \
            SENSORSMOKE_STATUSLOWBATTERY_IND_DESC

	#if SENSORSMOKE_STATUSLOWBATTERY_IND_ENABLE
	    #define SENSORSMOKE_STATUSLOWBATTERY_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorSmoke_StatusLowBattery), ccc_hSensorSmoke_StatusLowBatteryWriteCB, 0},
	    #define SENSORSMOKE_STATUSLOWBATTERY_IND_NO  1
	#else
	    #define SENSORSMOKE_STATUSLOWBATTERY_IND_DESC
	    #define SENSORSMOKE_STATUSLOWBATTERY_IND_NO  0
	#endif
#else
    #define SENSORSMOKE_STATUSLOWBATTERY_NO 0
    #define CHAR_SENSORSMOKE_STATUSLOWBATTERY_ATT_TABLE
    #define SENSORSMOKE_STATUSLOWBATTERY_IND_NO  0
#endif


#if CHAR_SENSOR_SMOKE_SVC_SIGN_ENABLE
#define CHAR_SENSOR_SMOKE_SVC_A5_SIGN_TABLE   \
    {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_sensor_smoke_A5_R_S, &hap_desc_service_sensor_smoke_A5_R_S},  \
    {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_SENSOR_SMOKE_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_SENSOR_SMOKE_UNKOWN, 0, 0},    \
        {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_SMOKE_INST_NO + 7), 0, 0},
#define SENSOR_SMOKE_SVC_SIGN_NO   3
#else
#define CHAR_SENSOR_SMOKE_SVC_A5_SIGN_TABLE
#define SENSOR_SMOKE_SVC_SIGN_NO   0
#endif

#define SENSOR_SMOKE_NO_DELTA   SENSORSMOKE_DETECTED_IND_NO \
                                +SENSORSMOKE_STATUSACTIVE_IND_NO    \
                                +SENSORSMOKE_STATUSFAULT_IND_NO \
                                +SENSORSMOKE_STATUSTAMPERED_IND_NO  \
                                +SENSORSMOKE_STATUSLOWBATTERY_IND_NO

#define SENSOR_SMOKE_NO         9 +SENSORSMOKE_STATUSACTIVE_NO	\
								+SENSORSMOKE_STATUSFAULT_NO \
								+SENSORSMOKE_STATUSTAMPERED_NO	\
								+SENSORSMOKE_STATUSLOWBATTERY_NO \
								+SENSOR_SMOKE_SVC_SIGN_NO

#define SENSOR_SMOKE_HANDLE_NUM (SENSOR_SMOKE_NO + SENSOR_SMOKE_NO_DELTA)


#define HOMEKIT_SENSOR_SMOKE_SERVICE_ATT_TABLE    \
    {SENSOR_SMOKE_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(sensor_smokeServiceUUID),sizeof(sensor_smokeServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(sensor_smokeServiceUUID), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(insts + SENSOR_SMOKE_INST_NO), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_NAME, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(hSensor_SmokeServiceName), 0, (u8*)(nameUUID), (u8*)(hSensor_SmokeServiceName), 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + SENSOR_SMOKE_INST_NO + 1), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Smoke_Detected_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Smoke_Detected),0, (u8*)(charSmokeDetectedUUID), (u8*)&hSensor_Smoke_Detected, 0, hSensor_Smoke_DetectedReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_SMOKE_INST_NO + 2), 0, 0},    \
            SENSORSMOKE_DETECTED_IND_DESC \
            CHAR_SENSORSMOKE_STATUSACTIVE_ATT_TABLE \
            CHAR_SENSORSMOKE_STATUSFAULT_ATT_TABLE \
            CHAR_SENSORSMOKE_STATUSTAMPERED_ATT_TABLE \
            CHAR_SENSORSMOKE_STATUSLOWBATTERY_ATT_TABLE \
        	CHAR_SENSOR_SMOKE_SVC_A5_SIGN_TABLE


#define SENSORSMOKE_HANDLE_COMMON_NUM     8

#define SENSORSMOKE_HANDLE_NUM_DETECTED         SENSOR_SMOKE_HANDLE_START_NUM +SENSORSMOKE_HANDLE_COMMON_NUM
#define SENSORSMOKE_HANDLE_NUM_STATUSACTIVE     SENSORSMOKE_HANDLE_NUM_DETECTED +SENSORSMOKE_DETECTED_NO +SENSORSMOKE_DETECTED_IND_NO
#define SENSORSMOKE_HANDLE_NUM_STATUSFAULT      SENSORSMOKE_HANDLE_NUM_STATUSACTIVE +SENSORSMOKE_STATUSACTIVE_NO +SENSORSMOKE_STATUSACTIVE_IND_NO
#define SENSORSMOKE_HANDLE_NUM_STATUSTAMPERED   SENSORSMOKE_HANDLE_NUM_STATUSFAULT +SENSORSMOKE_STATUSFAULT_NO +SENSORSMOKE_STATUSFAULT_IND_NO
#define SENSORSMOKE_HANDLE_NUM_STATUSLOWBATTERY SENSORSMOKE_HANDLE_NUM_STATUSTAMPERED +SENSORSMOKE_STATUSTAMPERED_NO +SENSORSMOKE_STATUSTAMPERED_IND_NO
#define SENSORSMOKE_HANDLE_NUM_THE_END


#define SENSOR_SMOKE_STORE_CHAR_VALUE_T_SENSOR_SMOKE_DETECTED                 u32 Sensor_Smoke_Detected:1;  /* 0~1 */
#define SENSOR_SMOKE_STORE_CHAR_VALUE_T_SENSOR_SMOKE_DETECTED_STORE               STORE_CHAR(Sensor_Smoke_Detected,hSensor_Smoke_Detected)
#define SENSOR_SMOKE_STORE_CHAR_VALUE_T_SENSOR_SMOKE_DETECTED_RESTORE             RESTORE_CHAR(Sensor_Smoke_Detected,hSensor_Smoke_Detected)

#define SENSOR_SMOKE_STORE_CHAR_VALUE_T_SENSOR_SMOKE_STATUSACTIVE             u32 Sensor_Smoke_StatusActive:1;  /* 0~1 */
#define SENSOR_SMOKE_STORE_CHAR_VALUE_T_SENSOR_SMOKE_STATUSACTIVE_STORE           STORE_CHAR(Sensor_Smoke_StatusActive,hSensor_Smoke_StatusActive)
#define SENSOR_SMOKE_STORE_CHAR_VALUE_T_SENSOR_SMOKE_STATUSACTIVE_RESTORE         RESTORE_CHAR(Sensor_Smoke_StatusActive,hSensor_Smoke_StatusActive)

#define SENSOR_SMOKE_STORE_CHAR_VALUE_T_SENSOR_SMOKE_STATUSFAULT              u32 Sensor_Smoke_StatusFault:1;  /* 0~1 */
#define SENSOR_SMOKE_STORE_CHAR_VALUE_T_SENSOR_SMOKE_STATUSFAULT_STORE            STORE_CHAR(Sensor_Smoke_StatusFault,hSensor_Smoke_StatusFault)
#define SENSOR_SMOKE_STORE_CHAR_VALUE_T_SENSOR_SMOKE_STATUSFAULT_RESTORE          RESTORE_CHAR(Sensor_Smoke_StatusFault,hSensor_Smoke_StatusFault)

#define SENSOR_SMOKE_STORE_CHAR_VALUE_T_SENSOR_SMOKE_STATUSTAMPERED           u32 Sensor_Smoke_StatusTampered:1;  /* 0~1 */
#define SENSOR_SMOKE_STORE_CHAR_VALUE_T_SENSOR_SMOKE_STATUSTAMPERED_STORE         STORE_CHAR(Sensor_Smoke_StatusTampered,hSensor_Smoke_StatusTampered)
#define SENSOR_SMOKE_STORE_CHAR_VALUE_T_SENSOR_SMOKE_STATUSTAMPERED_RESTORE       RESTORE_CHAR(Sensor_Smoke_StatusTampered,hSensor_Smoke_StatusTampered)

#define SENSOR_SMOKE_STORE_CHAR_VALUE_T_SENSOR_SMOKE_STATUSLOWBATTERY         u32 Sensor_Smoke_StatusLowBattery:1;  /* 0~1 */
#define SENSOR_SMOKE_STORE_CHAR_VALUE_T_SENSOR_SMOKE_STATUSLOWBATTERY_STORE       STORE_CHAR(Sensor_Smoke_StatusLowBattery,hSensor_Smoke_StatusLowBattery)
#define SENSOR_SMOKE_STORE_CHAR_VALUE_T_SENSOR_SMOKE_STATUSLOWBATTERY_RESTORE     RESTORE_CHAR(Sensor_Smoke_StatusLowBattery,hSensor_Smoke_StatusLowBattery)

#define SENSOR_SMOKE_STORE_CHAR_VALUE_T   \
                    SENSOR_SMOKE_STORE_CHAR_VALUE_T_SENSOR_SMOKE_DETECTED  \
                    SENSOR_SMOKE_STORE_CHAR_VALUE_T_SENSOR_SMOKE_STATUSACTIVE   \
                    SENSOR_SMOKE_STORE_CHAR_VALUE_T_SENSOR_SMOKE_STATUSFAULT  \
                    SENSOR_SMOKE_STORE_CHAR_VALUE_T_SENSOR_SMOKE_STATUSTAMPERED   \
                    SENSOR_SMOKE_STORE_CHAR_VALUE_T_SENSOR_SMOKE_STATUSLOWBATTERY

#define SENSOR_SMOKE_STORE_CHAR_TO_FLASH  \
                    SENSOR_SMOKE_STORE_CHAR_VALUE_T_SENSOR_SMOKE_DETECTED_STORE    \
                    SENSOR_SMOKE_STORE_CHAR_VALUE_T_SENSOR_SMOKE_STATUSACTIVE_STORE \
                    SENSOR_SMOKE_STORE_CHAR_VALUE_T_SENSOR_SMOKE_STATUSFAULT_STORE    \
                    SENSOR_SMOKE_STORE_CHAR_VALUE_T_SENSOR_SMOKE_STATUSTAMPERED_STORE \
                    SENSOR_SMOKE_STORE_CHAR_VALUE_T_SENSOR_SMOKE_STATUSLOWBATTERY_STORE

#define SENSOR_SMOKE_RESTORE_CHAR_FROM_FLASH  \
                    SENSOR_SMOKE_STORE_CHAR_VALUE_T_SENSOR_SMOKE_DETECTED_RESTORE  \
                    SENSOR_SMOKE_STORE_CHAR_VALUE_T_SENSOR_SMOKE_STATUSACTIVE_RESTORE   \
                    SENSOR_SMOKE_STORE_CHAR_VALUE_T_SENSOR_SMOKE_STATUSFAULT_RESTORE  \
                    SENSOR_SMOKE_STORE_CHAR_VALUE_T_SENSOR_SMOKE_STATUSTAMPERED_RESTORE   \
                    SENSOR_SMOKE_STORE_CHAR_VALUE_T_SENSOR_SMOKE_STATUSLOWBATTERY_RESTORE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN unsigned char hSensor_Smoke_Detected;
EXTERN unsigned char hSensor_Smoke_StatusActive;
EXTERN unsigned char hSensor_Smoke_StatusFault;
EXTERN unsigned char hSensor_Smoke_StatusTampered;
EXTERN unsigned char hSensor_Smoke_StatusLowBattery;

EXTERN unsigned short ccc_hSensorSmoke_Detected;
EXTERN unsigned short ccc_hSensorSmoke_StatusActive;
EXTERN unsigned short ccc_hSensorSmoke_StatusFault;
EXTERN unsigned short ccc_hSensorSmoke_StatusTampered;
EXTERN unsigned short ccc_hSensorSmoke_StatusLowBattery;

#if SENSORSMOKE_DETECTED_BRC_ENABLE
    #define BC_PARA_SENSORSMOKE_DETECTED ((u8*)&bc_para_hSensorSmoke_Detected)
    EXTERN unsigned char bc_para_hSensorSmoke_Detected;
#else
    #define BC_PARA_SENSORSMOKE_DETECTED 0
#endif

#if SENSORSMOKE_STATUSACTIVE_BRC_ENABLE
    #define BC_PARA_SENSORSMOKE_STATUSACTIVE ((u8*)&bc_para_hSensorSmoke_StatusActive)
    EXTERN unsigned char bc_para_hSensorSmoke_StatusActive;
#else
    #define BC_PARA_SENSORSMOKE_STATUSACTIVE 0
#endif

#if SENSORSMOKE_STATUSFAULT_BRC_ENABLE
    #define BC_PARA_SENSORSMOKE_STATUSFAULT ((u8*)&bc_para_hSensorSmoke_StatusFault)
    EXTERN unsigned char bc_para_hSensorSmoke_StatusFault;
#else
    #define BC_PARA_SENSORSMOKE_STATUSFAULT 0
#endif

#if SENSORSMOKE_STATUSTAMPERED_BRC_ENABLE
    #define BC_PARA_SENSORSMOKE_STATUSTAMPERED ((u8*)&bc_para_hSensorSmoke_StatusTampered)
    EXTERN unsigned char bc_para_hSensorSmoke_StatusTampered;
#else
    #define BC_PARA_SENSORSMOKE_STATUSTAMPERED 0
#endif

#if SENSORSMOKE_STATUSLOWBATTERY_BRC_ENABLE
    #define BC_PARA_SENSORSMOKE_STATUSLOWBATTERY ((u8*)&bc_para_hSensorSmoke_StatusLowBattery)
    EXTERN unsigned char bc_para_hSensorSmoke_StatusLowBattery;
#else
    #define BC_PARA_SENSORSMOKE_STATUSLOWBATTERY 0
#endif

#if SENSOR_SMOKE_LINKED_SERVICES_ENABLE
	#define SENSOR_SMOKE_LINKED_SVCS			hSensorSmoke_linked_svcs
	#define SENSOR_SMOKE_LINKED_SVCS_LENGTH 	(sizeof(hSensorSmoke_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * hSensorSmoke_linked_svcs[];
#else
	#define SENSOR_SMOKE_LINKED_SVCS			0
	#define SENSOR_SMOKE_LINKED_SVCS_LENGTH 	0
#endif

EXTERN const u8 hSensor_SmokeServiceName[19];

EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Smoke_Detected_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Smoke_StatusActive_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Smoke_StatusFault_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Smoke_StatusTampered_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Smoke_StatusLowBattery_R_S;


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN int hSensor_Smoke_DetectedReadCallback(void *pp);
EXTERN int hSensor_Smoke_StatusActiveReadCallback(void *pp);
EXTERN int hSensor_Smoke_StatusFaultReadCallback(void *pp);
EXTERN int hSensor_Smoke_StatusTamperedReadCallback(void *pp);
EXTERN int hSensor_Smoke_StatusLowBatteryReadCallback(void *pp);
EXTERN int ccc_hSensorSmoke_DetectedWriteCB(void *pp);
EXTERN int ccc_hSensorSmoke_StatusActiveWriteCB(void *pp);
EXTERN int ccc_hSensorSmoke_StatusFaultWriteCB(void *pp);
EXTERN int ccc_hSensorSmoke_StatusTamperedWriteCB(void *pp);
EXTERN int ccc_hSensorSmoke_StatusLowBatteryWriteCB(void *pp);


#endif /* _HK_SENSOR_SMOKE_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
