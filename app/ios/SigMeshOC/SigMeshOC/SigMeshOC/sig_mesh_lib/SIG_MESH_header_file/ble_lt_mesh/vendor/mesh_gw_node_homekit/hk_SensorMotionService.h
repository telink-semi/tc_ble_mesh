/********************************************************************************************************
 * @file     hk_SensorMotionService.h
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
#ifndef _HK_SENSOR_MOTION_SERVICE_H_
#define _HK_SENSOR_MOTION_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */
//-#include "../../homekit_src/homekit_inc.h"


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_SENSOR_MOTION_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif


#define CHAR_SENSOR_MOTION_SVC_SIGN_ENABLE      0
#if CHAR_SENSOR_MOTION_SVC_SIGN_ENABLE
    #define SENSOR_MOTION_AS_PRIMARY_SERVICE    	0
    #define SENSOR_MOTION_IS_HIDDEN_SERVICE     	0
    #define SENSOR_MOTION_LINKED_SERVICES_ENABLE	0
    EXTERN const hap_characteristic_desc_t hap_desc_char_sensor_motion_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_sensor_motion_A5_R_S;
    EXTERN unsigned char VAR_SENSOR_MOTION_UNKOWN;
#else
    #define SENSOR_MOTION_AS_PRIMARY_SERVICE    	0
    #define SENSOR_MOTION_IS_HIDDEN_SERVICE     	0
    #define SENSOR_MOTION_LINKED_SERVICES_ENABLE	0
#endif


#define CHAR_SENSORMOTION_STATUSACTIVE_ENABLE       0
#define CHAR_SENSORMOTION_STATUSFAULT_ENABLE        0
#define CHAR_SENSORMOTION_STATUSTAMPERED_ENABLE     0
#define CHAR_SENSORMOTION_STATUSLOWBATTERY_ENABLE   0


#define SENSORMOTION_DETECTED_IND_ENABLE          0
#define SENSORMOTION_STATUSACTIVE_IND_ENABLE      0
#define SENSORMOTION_STATUSFAULT_IND_ENABLE       0
#define SENSORMOTION_STATUSTAMPERED_IND_ENABLE    0
#define SENSORMOTION_STATUSLOWBATTERY_IND_ENABLE  0

#define SENSORMOTION_DETECTED_BRC_ENABLE          0
#define SENSORMOTION_STATUSACTIVE_BRC_ENABLE      0
#define SENSORMOTION_STATUSFAULT_BRC_ENABLE       0
#define SENSORMOTION_STATUSTAMPERED_BRC_ENABLE    0
#define SENSORMOTION_STATUSLOWBATTERY_BRC_ENABLE  0


#define SENSORMOTION_DETECTED_NO 3
#if SENSORMOTION_DETECTED_IND_ENABLE
    #define SENSORMOTION_DETECTED_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorMotion_Detected), ccc_hSensorMotion_DetectedWriteCB, 0},
    #define SENSORMOTION_DETECTED_IND_NO  1
#else
    #define SENSORMOTION_DETECTED_IND_DESC
    #define SENSORMOTION_DETECTED_IND_NO  0
#endif


#if CHAR_SENSORMOTION_STATUSACTIVE_ENABLE
    #define SENSORMOTION_STATUSACTIVE_NO 3
    #define CHAR_SENSORMOTION_STATUSACTIVE_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Motion_StatusActive_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Motion_StatusActive),0, (u8*)(charStatusActiveUUID), (u8*)&hSensor_Motion_StatusActive, 0, hSensor_Motion_StatusActiveReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_MOTION_INST_NO + 3), 0, 0},    \
            SENSORMOTION_STATUSACTIVE_IND_DESC

	#if SENSORMOTION_STATUSACTIVE_IND_ENABLE
	    #define SENSORMOTION_STATUSACTIVE_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorMotion_StatusActive), ccc_hSensorMotion_StatusActiveWriteCB, 0},
	    #define SENSORMOTION_STATUSACTIVE_IND_NO  1
	#else
	    #define SENSORMOTION_STATUSACTIVE_IND_DESC
	    #define SENSORMOTION_STATUSACTIVE_IND_NO  0
	#endif
#else
    #define SENSORMOTION_STATUSACTIVE_NO 0
    #define CHAR_SENSORMOTION_STATUSACTIVE_ATT_TABLE
    #define SENSORMOTION_STATUSACTIVE_IND_NO  0
#endif

#if CHAR_SENSORMOTION_STATUSFAULT_ENABLE
    #define SENSORMOTION_STATUSFAULT_NO 3
    #define CHAR_SENSORMOTION_STATUSFAULT_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Motion_StatusFault_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Motion_StatusFault),0, (u8*)(charStatusFaultUUID), (u8*)&hSensor_Motion_StatusFault, 0, hSensor_Motion_StatusFaultReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_MOTION_INST_NO + 4), 0, 0},    \
            SENSORMOTION_STATUSFAULT_IND_DESC

	#if SENSORMOTION_STATUSFAULT_IND_ENABLE
	    #define SENSORMOTION_STATUSFAULT_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorMotion_StatusFault), ccc_hSensorMotion_StatusFaultWriteCB, 0},
	    #define SENSORMOTION_STATUSFAULT_IND_NO  1
	#else
	    #define SENSORMOTION_STATUSFAULT_IND_DESC
	    #define SENSORMOTION_STATUSFAULT_IND_NO  0
	#endif
#else
    #define SENSORMOTION_STATUSFAULT_NO 0
    #define CHAR_SENSORMOTION_STATUSFAULT_ATT_TABLE
    #define SENSORMOTION_STATUSFAULT_IND_NO  0
#endif


#if CHAR_SENSORMOTION_STATUSTAMPERED_ENABLE
    #define SENSORMOTION_STATUSTAMPERED_NO 3
    #define CHAR_SENSORMOTION_STATUSTAMPERED_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Motion_StatusTampered_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Motion_StatusTampered),0, (u8*)(charStatusTamperedUUID), (u8*)&hSensor_Motion_StatusTampered, 0, hSensor_Motion_StatusTamperedReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_MOTION_INST_NO + 5), 0, 0},    \
            SENSORMOTION_STATUSTAMPERED_IND_DESC

	#if SENSORMOTION_STATUSTAMPERED_IND_ENABLE
	    #define SENSORMOTION_STATUSTAMPERED_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorMotion_StatusTampered), ccc_hSensorMotion_StatusTamperedWriteCB, 0},
	    #define SENSORMOTION_STATUSTAMPERED_IND_NO  1
	#else
	    #define SENSORMOTION_STATUSTAMPERED_IND_DESC
	    #define SENSORMOTION_STATUSTAMPERED_IND_NO  0
	#endif
#else
    #define SENSORMOTION_STATUSTAMPERED_NO 0
    #define CHAR_SENSORMOTION_STATUSTAMPERED_ATT_TABLE
    #define SENSORMOTION_STATUSTAMPERED_IND_NO  0
#endif

#if CHAR_SENSORMOTION_STATUSLOWBATTERY_ENABLE
    #define SENSORMOTION_STATUSLOWBATTERY_NO 3
    #define CHAR_SENSORMOTION_STATUSLOWBATTERY_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Motion_StatusLowBattery_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Motion_StatusLowBattery),0, (u8*)(charStatusLowBatteryUUID), (u8*)&hSensor_Motion_StatusLowBattery, 0, hSensor_Motion_StatusLowBatteryReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_MOTION_INST_NO + 6), 0, 0},    \
            SENSORMOTION_STATUSLOWBATTERY_IND_DESC

	#if SENSORMOTION_STATUSLOWBATTERY_IND_ENABLE
	    #define SENSORMOTION_STATUSLOWBATTERY_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorMotion_StatusLowBattery), ccc_hSensorMotion_StatusLowBatteryWriteCB, 0},
	    #define SENSORMOTION_STATUSLOWBATTERY_IND_NO  1
	#else
	    #define SENSORMOTION_STATUSLOWBATTERY_IND_DESC
	    #define SENSORMOTION_STATUSLOWBATTERY_IND_NO  0
	#endif
#else
    #define SENSORMOTION_STATUSLOWBATTERY_NO 0
    #define CHAR_SENSORMOTION_STATUSLOWBATTERY_ATT_TABLE
    #define SENSORMOTION_STATUSLOWBATTERY_IND_NO  0
#endif


#if CHAR_SENSOR_MOTION_SVC_SIGN_ENABLE
#define CHAR_SENSOR_MOTION_SVC_A5_SIGN_TABLE   \
    {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_sensor_motion_A5_R_S, &hap_desc_service_sensor_motion_A5_R_S},  \
    {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_SENSOR_MOTION_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_SENSOR_MOTION_UNKOWN, 0, 0},    \
        {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_MOTION_INST_NO + 7), 0, 0},
#define SENSOR_MOTION_SVC_SIGN_NO   3
#else
#define CHAR_SENSOR_MOTION_SVC_A5_SIGN_TABLE
#define SENSOR_MOTION_SVC_SIGN_NO   0
#endif

#define SENSOR_MOTION_NO_DELTA  SENSORMOTION_DETECTED_IND_NO \
                                +SENSORMOTION_STATUSACTIVE_IND_NO   \
                                +SENSORMOTION_STATUSFAULT_IND_NO    \
                                +SENSORMOTION_STATUSTAMPERED_IND_NO \
                                +SENSORMOTION_STATUSLOWBATTERY_IND_NO

#define SENSOR_MOTION_NO        9 +SENSORMOTION_STATUSACTIVE_NO	\
								+SENSORMOTION_STATUSFAULT_NO	\
								+SENSORMOTION_STATUSTAMPERED_NO \
								+SENSORMOTION_STATUSLOWBATTERY_NO \
								+SENSOR_MOTION_SVC_SIGN_NO
								
#define SENSOR_MOTION_HANDLE_NUM (SENSOR_MOTION_NO + SENSOR_MOTION_NO_DELTA)


#define HOMEKIT_SENSOR_MOTION_SERVICE_ATT_TABLE    \
    {SENSOR_MOTION_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(sensor_motionServiceUUID),sizeof(sensor_motionServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(sensor_motionServiceUUID), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(insts + SENSOR_MOTION_INST_NO), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_NAME, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(hSensor_MotionServiceName), 0, (u8*)(nameUUID), (u8*)(hSensor_MotionServiceName), 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + SENSOR_MOTION_INST_NO + 1), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Motion_Detected_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Motion_Detected),0, (u8*)(charMotionDetectedUUID), (u8*)&hSensor_Motion_Detected, 0, hSensor_Motion_DetectedReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_MOTION_INST_NO + 2), 0, 0},    \
            SENSORMOTION_DETECTED_IND_DESC \
            CHAR_SENSORMOTION_STATUSACTIVE_ATT_TABLE \
            CHAR_SENSORMOTION_STATUSFAULT_ATT_TABLE \
            CHAR_SENSORMOTION_STATUSTAMPERED_ATT_TABLE \
            CHAR_SENSORMOTION_STATUSLOWBATTERY_ATT_TABLE \
        	CHAR_SENSOR_MOTION_SVC_A5_SIGN_TABLE


#define SENSORMOTION_HANDLE_COMMON_NUM     8

#define SENSORMOTION_HANDLE_NUM_DETECTED          SENSOR_MOTION_HANDLE_START_NUM +SENSORMOTION_HANDLE_COMMON_NUM
#define SENSORMOTION_HANDLE_NUM_STATUSACTIVE      SENSORMOTION_HANDLE_NUM_DETECTED +SENSORMOTION_DETECTED_NO +SENSORMOTION_DETECTED_IND_NO
#define SENSORMOTION_HANDLE_NUM_STATUSFAULT       SENSORMOTION_HANDLE_NUM_STATUSACTIVE +SENSORMOTION_STATUSACTIVE_NO +SENSORMOTION_STATUSACTIVE_IND_NO
#define SENSORMOTION_HANDLE_NUM_STATUSTAMPERED    SENSORMOTION_HANDLE_NUM_STATUSFAULT +SENSORMOTION_STATUSFAULT_NO +SENSORMOTION_STATUSFAULT_IND_NO
#define SENSORMOTION_HANDLE_NUM_STATUSLOWBATTERY  SENSORMOTION_HANDLE_NUM_STATUSTAMPERED +SENSORMOTION_STATUSTAMPERED_NO +SENSORMOTION_STATUSTAMPERED_IND_NO
#define SENSORMOTION_HANDLE_NUM_THE_END


#define SENSOR_MOTION_STORE_CHAR_VALUE_T_SENSOR_MOTION_DETECTED                 u32 Sensor_Motion_Detected:1;   /* 0~1 */
#define SENSOR_MOTION_STORE_CHAR_VALUE_T_SENSOR_MOTION_DETECTED_STORE               STORE_CHAR(Sensor_Motion_Detected,hSensor_Motion_Detected)
#define SENSOR_MOTION_STORE_CHAR_VALUE_T_SENSOR_MOTION_DETECTED_RESTORE             RESTORE_CHAR(Sensor_Motion_Detected,hSensor_Motion_Detected)

#define SENSOR_MOTION_STORE_CHAR_VALUE_T_SENSOR_MOTION_STATUSACTIVE             u32 Sensor_Motion_StatusActive:1;   /* 0~1 */
#define SENSOR_MOTION_STORE_CHAR_VALUE_T_SENSOR_MOTION_STATUSACTIVE_STORE           STORE_CHAR(Sensor_Motion_StatusActive,hSensor_Motion_StatusActive)
#define SENSOR_MOTION_STORE_CHAR_VALUE_T_SENSOR_MOTION_STATUSACTIVE_RESTORE         RESTORE_CHAR(Sensor_Motion_StatusActive,hSensor_Motion_StatusActive)

#define SENSOR_MOTION_STORE_CHAR_VALUE_T_SENSOR_MOTION_STATUSFAULT              u32 Sensor_Motion_StatusFault:1;   /* 0~1 */
#define SENSOR_MOTION_STORE_CHAR_VALUE_T_SENSOR_MOTION_STATUSFAULT_STORE            STORE_CHAR(Sensor_Motion_StatusFault,hSensor_Motion_StatusFault)
#define SENSOR_MOTION_STORE_CHAR_VALUE_T_SENSOR_MOTION_STATUSFAULT_RESTORE          RESTORE_CHAR(Sensor_Motion_StatusFault,hSensor_Motion_StatusFault)

#define SENSOR_MOTION_STORE_CHAR_VALUE_T_SENSOR_MOTION_STATUSTAMPERED           u32 Sensor_Motion_StatusTampered:1;   /* 0~1 */
#define SENSOR_MOTION_STORE_CHAR_VALUE_T_SENSOR_MOTION_STATUSTAMPERED_STORE         STORE_CHAR(Sensor_Motion_StatusTampered,hSensor_Motion_StatusTampered)
#define SENSOR_MOTION_STORE_CHAR_VALUE_T_SENSOR_MOTION_STATUSTAMPERED_RESTORE       RESTORE_CHAR(Sensor_Motion_StatusTampered,hSensor_Motion_StatusTampered)

#define SENSOR_MOTION_STORE_CHAR_VALUE_T_SENSOR_MOTION_STATUSLOWBATTERY         u32 Sensor_Motion_StatusLowBattery:1;   /* 0~1 */
#define SENSOR_MOTION_STORE_CHAR_VALUE_T_SENSOR_MOTION_STATUSLOWBATTERY_STORE       STORE_CHAR(Sensor_Motion_StatusLowBattery,hSensor_Motion_StatusLowBattery)
#define SENSOR_MOTION_STORE_CHAR_VALUE_T_SENSOR_MOTION_STATUSLOWBATTERY_RESTORE     RESTORE_CHAR(Sensor_Motion_StatusLowBattery,hSensor_Motion_StatusLowBattery)

#define SENSOR_MOTION_STORE_CHAR_VALUE_T   \
                    SENSOR_MOTION_STORE_CHAR_VALUE_T_SENSOR_MOTION_DETECTED  \
                    SENSOR_MOTION_STORE_CHAR_VALUE_T_SENSOR_MOTION_STATUSACTIVE   \
                    SENSOR_MOTION_STORE_CHAR_VALUE_T_SENSOR_MOTION_STATUSFAULT  \
                    SENSOR_MOTION_STORE_CHAR_VALUE_T_SENSOR_MOTION_STATUSTAMPERED   \
                    SENSOR_MOTION_STORE_CHAR_VALUE_T_SENSOR_MOTION_STATUSLOWBATTERY

#define SENSOR_MOTION_STORE_CHAR_TO_FLASH  \
                    SENSOR_MOTION_STORE_CHAR_VALUE_T_SENSOR_MOTION_DETECTED_STORE    \
                    SENSOR_MOTION_STORE_CHAR_VALUE_T_SENSOR_MOTION_STATUSACTIVE_STORE \
                    SENSOR_MOTION_STORE_CHAR_VALUE_T_SENSOR_MOTION_STATUSFAULT_STORE    \
                    SENSOR_MOTION_STORE_CHAR_VALUE_T_SENSOR_MOTION_STATUSTAMPERED_STORE \
                    SENSOR_MOTION_STORE_CHAR_VALUE_T_SENSOR_MOTION_STATUSLOWBATTERY_STORE

#define SENSOR_MOTION_RESTORE_CHAR_FROM_FLASH  \
                    SENSOR_MOTION_STORE_CHAR_VALUE_T_SENSOR_MOTION_DETECTED_RESTORE  \
                    SENSOR_MOTION_STORE_CHAR_VALUE_T_SENSOR_MOTION_STATUSACTIVE_RESTORE   \
                    SENSOR_MOTION_STORE_CHAR_VALUE_T_SENSOR_MOTION_STATUSFAULT_RESTORE  \
                    SENSOR_MOTION_STORE_CHAR_VALUE_T_SENSOR_MOTION_STATUSTAMPERED_RESTORE   \
                    SENSOR_MOTION_STORE_CHAR_VALUE_T_SENSOR_MOTION_STATUSLOWBATTERY_RESTORE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN unsigned char hSensor_Motion_Detected;
EXTERN unsigned char hSensor_Motion_StatusActive;
EXTERN unsigned char hSensor_Motion_StatusFault;
EXTERN unsigned char hSensor_Motion_StatusTampered;
EXTERN unsigned char hSensor_Motion_StatusLowBattery;

EXTERN unsigned short ccc_hSensorMotion_Detected;
EXTERN unsigned short ccc_hSensorMotion_StatusActive;
EXTERN unsigned short ccc_hSensorMotion_StatusFault;
EXTERN unsigned short ccc_hSensorMotion_StatusTampered;
EXTERN unsigned short ccc_hSensorMotion_StatusLowBattery;

#if SENSORMOTION_DETECTED_BRC_ENABLE
    #define BC_PARA_SENSORMOTION_DETECTED ((u8*)&bc_para_hSensorMotion_Detected)
    EXTERN unsigned char bc_para_hSensorMotion_Detected;
#else
    #define BC_PARA_SENSORMOTION_DETECTED 0
#endif

#if SENSORMOTION_STATUSACTIVE_BRC_ENABLE
    #define BC_PARA_SENSORMOTION_STATUSACTIVE ((u8*)&bc_para_hSensorMotion_StatusActive)
    EXTERN unsigned char bc_para_hSensorMotion_StatusActive;
#else
    #define BC_PARA_SENSORMOTION_STATUSACTIVE 0
#endif

#if SENSORMOTION_STATUSFAULT_BRC_ENABLE
    #define BC_PARA_SENSORMOTION_STATUSFAULT ((u8*)&bc_para_hSensorMotion_StatusFault)
    EXTERN unsigned char bc_para_hSensorMotion_StatusFault;
#else
    #define BC_PARA_SENSORMOTION_STATUSFAULT 0
#endif

#if SENSORMOTION_STATUSTAMPERED_BRC_ENABLE
    #define BC_PARA_SENSORMOTION_STATUSTAMPERED ((u8*)&bc_para_hSensorMotion_StatusTampered)
    EXTERN unsigned char bc_para_hSensorMotion_StatusTampered;
#else
    #define BC_PARA_SENSORMOTION_STATUSTAMPERED 0
#endif

#if SENSORMOTION_STATUSLOWBATTERY_BRC_ENABLE
    #define BC_PARA_SENSORMOTION_STATUSLOWBATTERY ((u8*)&bc_para_hSensorMotion_StatusLowBattery)
    EXTERN unsigned char bc_para_hSensorMotion_StatusLowBattery;
#else
    #define BC_PARA_SENSORMOTION_STATUSLOWBATTERY 0
#endif

#if SENSOR_MOTION_LINKED_SERVICES_ENABLE
	#define SENSOR_MOTION_LINKED_SVCS			hSensorMotion_linked_svcs
	#define SENSOR_MOTION_LINKED_SVCS_LENGTH 	(sizeof(hSensorMotion_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * hSensorMotion_linked_svcs[];
#else
	#define SENSOR_MOTION_LINKED_SVCS			0
	#define SENSOR_MOTION_LINKED_SVCS_LENGTH 	0
#endif

EXTERN const u8 hSensor_MotionServiceName[20];

EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Motion_Detected_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Motion_StatusActive_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Motion_StatusFault_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Motion_StatusTampered_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Motion_StatusLowBattery_R_S;


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN int hSensor_Motion_DetectedReadCallback(void *pp);
EXTERN int hSensor_Motion_StatusActiveReadCallback(void *pp);
EXTERN int hSensor_Motion_StatusFaultReadCallback(void *pp);
EXTERN int hSensor_Motion_StatusTamperedReadCallback(void *pp);
EXTERN int hSensor_Motion_StatusLowBatteryReadCallback(void *pp);
EXTERN int ccc_hSensorMotion_DetectedWriteCB(void *pp);
EXTERN int ccc_hSensorMotion_StatusActiveWriteCB(void *pp);
EXTERN int ccc_hSensorMotion_StatusFaultWriteCB(void *pp);
EXTERN int ccc_hSensorMotion_StatusTamperedWriteCB(void *pp);
EXTERN int ccc_hSensorMotion_StatusLowBatteryWriteCB(void *pp);


#endif /* _HK_SENSOR_MOTION_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
