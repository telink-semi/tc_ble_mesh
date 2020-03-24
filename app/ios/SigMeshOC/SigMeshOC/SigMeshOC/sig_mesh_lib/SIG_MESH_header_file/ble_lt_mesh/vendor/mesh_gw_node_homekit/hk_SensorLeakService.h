/********************************************************************************************************
 * @file     hk_SensorLeakService.h
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
#ifndef _HK_SENSOR_LEAK_SERVICE_H_
#define _HK_SENSOR_LEAK_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */
//-#include "../../homekit_src/homekit_inc.h"


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_SENSOR_LEAK_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif


#define CHAR_SENSOR_LEAK_SVC_SIGN_ENABLE      0
#if CHAR_SENSOR_LEAK_SVC_SIGN_ENABLE
    #define SENSOR_LEAK_AS_PRIMARY_SERVICE    	0
    #define SENSOR_LEAK_IS_HIDDEN_SERVICE     	0
    #define SENSOR_LEAK_LINKED_SERVICES_ENABLE	0
    EXTERN const hap_characteristic_desc_t hap_desc_char_sensor_leak_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_sensor_leak_A5_R_S;
    EXTERN unsigned char VAR_SENSOR_LEAK_UNKOWN;
#else
    #define SENSOR_LEAK_AS_PRIMARY_SERVICE    	0
    #define SENSOR_LEAK_IS_HIDDEN_SERVICE     	0
    #define SENSOR_LEAK_LINKED_SERVICES_ENABLE	0
#endif


#define CHAR_SENSORLEAK_STATUSACTIVE_ENABLE       0
#define CHAR_SENSORLEAK_STATUSFAULT_ENABLE        0
#define CHAR_SENSORLEAK_STATUSTAMPERED_ENABLE     0
#define CHAR_SENSORLEAK_STATUSLOWBATTERY_ENABLE   0

#define SENSORLEAK_DETECTED_IND_ENABLE          0
#define SENSORLEAK_STATUSACTIVE_IND_ENABLE      0
#define SENSORLEAK_STATUSFAULT_IND_ENABLE       0
#define SENSORLEAK_STATUSTAMPERED_IND_ENABLE    0
#define SENSORLEAK_STATUSLOWBATTERY_IND_ENABLE  0

#define SENSORLEAK_DETECTED_BRC_ENABLE          0
#define SENSORLEAK_STATUSACTIVE_BRC_ENABLE      0
#define SENSORLEAK_STATUSFAULT_BRC_ENABLE       0
#define SENSORLEAK_STATUSTAMPERED_BRC_ENABLE    0
#define SENSORLEAK_STATUSLOWBATTERY_BRC_ENABLE  0


#define SENSORLEAK_DETECTED_NO 3
#if SENSORLEAK_DETECTED_IND_ENABLE
    #define SENSORLEAK_DETECTED_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensor_Leak_Detected), ccc_hSensor_Leak_DetectedWriteCB, 0},
    #define SENSORLEAK_DETECTED_IND_NO  1
#else
    #define SENSORLEAK_DETECTED_IND_DESC
    #define SENSORLEAK_DETECTED_IND_NO  0
#endif


#if CHAR_SENSORLEAK_STATUSACTIVE_ENABLE
    #define SENSORLEAK_STATUSACTIVE_NO 3
    #define CHAR_SENSORLEAK_STATUSACTIVE_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Leak_StatusActive_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Leak_StatusActive),0, (u8*)(charStatusActiveUUID), (u8*)&hSensor_Leak_StatusActive, 0, hSensor_Leak_StatusActiveReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_LEAK_INST_NO + 3), 0, 0},    \
            SENSORLEAK_STATUSACTIVE_IND_DESC

	#if SENSORLEAK_STATUSACTIVE_IND_ENABLE
	    #define SENSORLEAK_STATUSACTIVE_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensor_Leak_StatusActive), ccc_hSensor_Leak_StatusActiveWriteCB, 0},
	    #define SENSORLEAK_STATUSACTIVE_IND_NO  1
	#else
	    #define SENSORLEAK_STATUSACTIVE_IND_DESC
	    #define SENSORLEAK_STATUSACTIVE_IND_NO  0
	#endif
#else
    #define SENSORLEAK_STATUSACTIVE_NO 0
    #define CHAR_SENSORLEAK_STATUSACTIVE_ATT_TABLE
    #define SENSORLEAK_STATUSACTIVE_IND_NO  0
#endif

#if CHAR_SENSORLEAK_STATUSFAULT_ENABLE
    #define SENSORLEAK_STATUSFAULT_NO 3
    #define CHAR_SENSORLEAK_STATUSFAULT_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Leak_StatusFault_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Leak_StatusFault),0, (u8*)(charStatusFaultUUID), (u8*)&hSensor_Leak_StatusFault, 0, hSensor_Leak_StatusFaultReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_LEAK_INST_NO + 4), 0, 0},    \
            SENSORLEAK_STATUSFAULT_IND_DESC

	#if SENSORLEAK_STATUSFAULT_IND_ENABLE
	    #define SENSORLEAK_STATUSFAULT_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensor_Leak_StatusFault), ccc_hSensor_Leak_StatusFaultWriteCB, 0},
	    #define SENSORLEAK_STATUSFAULT_IND_NO  1
	#else
	    #define SENSORLEAK_STATUSFAULT_IND_DESC
	    #define SENSORLEAK_STATUSFAULT_IND_NO  0
	#endif
#else
    #define SENSORLEAK_STATUSFAULT_NO 0
    #define CHAR_SENSORLEAK_STATUSFAULT_ATT_TABLE
    #define SENSORLEAK_STATUSFAULT_IND_NO  0
#endif


#if CHAR_SENSORLEAK_STATUSTAMPERED_ENABLE
    #define SENSORLEAK_STATUSTAMPERED_NO 3
    #define CHAR_SENSORLEAK_STATUSTAMPERED_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Leak_StatusTampered_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Leak_StatusTampered),0, (u8*)(charStatusTamperedUUID), (u8*)&hSensor_Leak_StatusTampered, 0, hSensor_Leak_StatusTamperedReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_LEAK_INST_NO + 5), 0, 0},    \
            SENSORLEAK_STATUSTAMPERED_IND_DESC

	#if SENSORLEAK_STATUSTAMPERED_IND_ENABLE
	    #define SENSORLEAK_STATUSTAMPERED_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensor_Leak_StatusTampered), ccc_hSensor_Leak_StatusTamperedWriteCB, 0},
	    #define SENSORLEAK_STATUSTAMPERED_IND_NO  1
	#else
	    #define SENSORLEAK_STATUSTAMPERED_IND_DESC
	    #define SENSORLEAK_STATUSTAMPERED_IND_NO  0
	#endif
#else
    #define SENSORLEAK_STATUSTAMPERED_NO 0
    #define CHAR_SENSORLEAK_STATUSTAMPERED_ATT_TABLE
    #define SENSORLEAK_STATUSTAMPERED_IND_NO  0
#endif

#if CHAR_SENSORLEAK_STATUSLOWBATTERY_ENABLE
    #define SENSORLEAK_STATUSLOWBATTERY_NO 3
    #define CHAR_SENSORLEAK_STATUSLOWBATTERY_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Leak_StatusLowBattery_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Leak_StatusLowBattery),0, (u8*)(charStatusLowBatteryUUID), (u8*)&hSensor_Leak_StatusLowBattery, 0, hSensor_Leak_StatusLowBatteryReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_LEAK_INST_NO + 6), 0, 0},    \
            SENSORLEAK_STATUSLOWBATTERY_IND_DESC

	#if SENSORLEAK_STATUSLOWBATTERY_IND_ENABLE
	    #define SENSORLEAK_STATUSLOWBATTERY_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensor_Leak_StatusLowBattery), ccc_hSensor_Leak_StatusLowBatteryWriteCB, 0},
	    #define SENSORLEAK_STATUSLOWBATTERY_IND_NO  1
	#else
	    #define SENSORLEAK_STATUSLOWBATTERY_IND_DESC
	    #define SENSORLEAK_STATUSLOWBATTERY_IND_NO  0
	#endif
#else
    #define SENSORLEAK_STATUSLOWBATTERY_NO 0
    #define CHAR_SENSORLEAK_STATUSLOWBATTERY_ATT_TABLE
    #define SENSORLEAK_STATUSLOWBATTERY_IND_NO  0
#endif


#if CHAR_SENSOR_LEAK_SVC_SIGN_ENABLE
#define CHAR_SENSOR_LEAK_SVC_A5_SIGN_TABLE   \
    {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_sensor_leak_A5_R_S, &hap_desc_service_sensor_leak_A5_R_S},  \
    {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_SENSOR_LEAK_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_SENSOR_LEAK_UNKOWN, 0, 0},    \
        {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_LEAK_INST_NO + 7), 0, 0},
#define SENSOR_LEAK_SVC_SIGN_NO   3
#else
#define CHAR_SENSOR_LEAK_SVC_A5_SIGN_TABLE
#define SENSOR_LEAK_SVC_SIGN_NO   0
#endif

#define SENSOR_LEAK_NO_DELTA    SENSORLEAK_DETECTED_IND_NO  \
                                +SENSORLEAK_STATUSACTIVE_IND_NO \
                                +SENSORLEAK_STATUSFAULT_IND_NO  \
                                +SENSORLEAK_STATUSTAMPERED_IND_NO   \
                                +SENSORLEAK_STATUSLOWBATTERY_IND_NO

#define SENSOR_LEAK_NO          9 +SENSORLEAK_STATUSACTIVE_NO \
								+SENSORLEAK_STATUSFAULT_NO  \
								+SENSORLEAK_STATUSTAMPERED_NO   \
								+SENSORLEAK_STATUSLOWBATTERY_NO \
								+SENSOR_LEAK_SVC_SIGN_NO
								
#define SENSOR_LEAK_HANDLE_NUM (SENSOR_LEAK_NO + SENSOR_LEAK_NO_DELTA)


#define HOMEKIT_SENSOR_LEAK_SERVICE_ATT_TABLE    \
    {SENSOR_LEAK_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(sensor_leakServiceUUID),sizeof(sensor_leakServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(sensor_leakServiceUUID), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(insts + SENSOR_LEAK_INST_NO), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_NAME, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(hSensor_LeakServiceName), 0, (u8*)(nameUUID), (u8*)(hSensor_LeakServiceName), 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + SENSOR_LEAK_INST_NO + 1), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Leak_Detected_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Leak_Detected),0, (u8*)(charLeakDetectedUUID), (u8*)&hSensor_Leak_Detected, 0, hSensor_Leak_DetectedReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_LEAK_INST_NO + 2), 0, 0},    \
            SENSORLEAK_DETECTED_IND_DESC \
            CHAR_SENSORLEAK_STATUSACTIVE_ATT_TABLE \
            CHAR_SENSORLEAK_STATUSFAULT_ATT_TABLE \
            CHAR_SENSORLEAK_STATUSTAMPERED_ATT_TABLE \
            CHAR_SENSORLEAK_STATUSLOWBATTERY_ATT_TABLE \
        	CHAR_SENSOR_LEAK_SVC_A5_SIGN_TABLE


#define SENSORLEAK_HANDLE_COMMON_NUM     8

#define SENSORLEAK_HANDLE_NUM_DETECTED          SENSOR_LEAK_HANDLE_START_NUM +SENSORLEAK_HANDLE_COMMON_NUM
#define SENSORLEAK_HANDLE_NUM_STATUSACTIVE      SENSORLEAK_HANDLE_NUM_DETECTED +SENSORLEAK_DETECTED_NO +SENSORLEAK_DETECTED_IND_NO
#define SENSORLEAK_HANDLE_NUM_STATUSFAULT       SENSORLEAK_HANDLE_NUM_STATUSACTIVE +SENSORLEAK_STATUSACTIVE_NO +SENSORLEAK_STATUSACTIVE_IND_NO
#define SENSORLEAK_HANDLE_NUM_STATUSTAMPERED    SENSORLEAK_HANDLE_NUM_STATUSFAULT +SENSORLEAK_STATUSFAULT_NO +SENSORLEAK_STATUSFAULT_IND_NO
#define SENSORLEAK_HANDLE_NUM_STATUSLOWBATTERY  SENSORLEAK_HANDLE_NUM_STATUSTAMPERED +SENSORLEAK_STATUSTAMPERED_NO +SENSORLEAK_STATUSTAMPERED_IND_NO
#define SENSORLEAK_HANDLE_NUM_THE_END


#define SENSOR_LEAK_STORE_CHAR_VALUE_T_SENSOR_LEAK_DETECTED                 u32 Sensor_Leak_Detected:1; /* 0~1 */
#define SENSOR_LEAK_STORE_CHAR_VALUE_T_SENSOR_LEAK_DETECTED_STORE               STORE_CHAR(Sensor_Leak_Detected,hSensor_Leak_Detected)
#define SENSOR_LEAK_STORE_CHAR_VALUE_T_SENSOR_LEAK_DETECTED_RESTORE             RESTORE_CHAR(Sensor_Leak_Detected,hSensor_Leak_Detected)

#define SENSOR_LEAK_STORE_CHAR_VALUE_T_SENSOR_LEAK_STATUSACTIVE             u32 Sensor_Leak_StatusActive:1; /* 0~1 */
#define SENSOR_LEAK_STORE_CHAR_VALUE_T_SENSOR_LEAK_STATUSACTIVE_STORE           STORE_CHAR(Sensor_Leak_StatusActive,hSensor_Leak_StatusActive)
#define SENSOR_LEAK_STORE_CHAR_VALUE_T_SENSOR_LEAK_STATUSACTIVE_RESTORE         RESTORE_CHAR(Sensor_Leak_StatusActive,hSensor_Leak_StatusActive)

#define SENSOR_LEAK_STORE_CHAR_VALUE_T_SENSOR_LEAK_STATUSFAULT              u32 Sensor_Leak_StatusFault:1; /* 0~1 */
#define SENSOR_LEAK_STORE_CHAR_VALUE_T_SENSOR_LEAK_STATUSFAULT_STORE            STORE_CHAR(Sensor_Leak_StatusFault,hSensor_Leak_StatusFault)
#define SENSOR_LEAK_STORE_CHAR_VALUE_T_SENSOR_LEAK_STATUSFAULT_RESTORE          RESTORE_CHAR(Sensor_Leak_StatusFault,hSensor_Leak_StatusFault)

#define SENSOR_LEAK_STORE_CHAR_VALUE_T_SENSOR_LEAK_STATUSTAMPERED           u32 Sensor_Leak_StatusTampered:1; /* 0~1 */
#define SENSOR_LEAK_STORE_CHAR_VALUE_T_SENSOR_LEAK_STATUSTAMPERED_STORE         STORE_CHAR(Sensor_Leak_StatusTampered,hSensor_Leak_StatusTampered)
#define SENSOR_LEAK_STORE_CHAR_VALUE_T_SENSOR_LEAK_STATUSTAMPERED_RESTORE       RESTORE_CHAR(Sensor_Leak_StatusTampered,hSensor_Leak_StatusTampered)

#define SENSOR_LEAK_STORE_CHAR_VALUE_T_SENSOR_LEAK_STATUSLOWBATTERY         u32 Sensor_Leak_StatusLowBattery:1; /* 0~1 */
#define SENSOR_LEAK_STORE_CHAR_VALUE_T_SENSOR_LEAK_STATUSLOWBATTERY_STORE       STORE_CHAR(Sensor_Leak_StatusLowBattery,hSensor_Leak_StatusLowBattery)
#define SENSOR_LEAK_STORE_CHAR_VALUE_T_SENSOR_LEAK_STATUSLOWBATTERY_RESTORE     RESTORE_CHAR(Sensor_Leak_StatusLowBattery,hSensor_Leak_StatusLowBattery)

#define SENSOR_LEAK_STORE_CHAR_VALUE_T   \
                    SENSOR_LEAK_STORE_CHAR_VALUE_T_SENSOR_LEAK_DETECTED  \
                    SENSOR_LEAK_STORE_CHAR_VALUE_T_SENSOR_LEAK_STATUSACTIVE   \
                    SENSOR_LEAK_STORE_CHAR_VALUE_T_SENSOR_LEAK_STATUSFAULT  \
                    SENSOR_LEAK_STORE_CHAR_VALUE_T_SENSOR_LEAK_STATUSTAMPERED   \
                    SENSOR_LEAK_STORE_CHAR_VALUE_T_SENSOR_LEAK_STATUSLOWBATTERY

#define SENSOR_LEAK_STORE_CHAR_TO_FLASH  \
                    SENSOR_LEAK_STORE_CHAR_VALUE_T_SENSOR_LEAK_DETECTED_STORE    \
                    SENSOR_LEAK_STORE_CHAR_VALUE_T_SENSOR_LEAK_STATUSACTIVE_STORE \
                    SENSOR_LEAK_STORE_CHAR_VALUE_T_SENSOR_LEAK_STATUSFAULT_STORE    \
                    SENSOR_LEAK_STORE_CHAR_VALUE_T_SENSOR_LEAK_STATUSTAMPERED_STORE \
                    SENSOR_LEAK_STORE_CHAR_VALUE_T_SENSOR_LEAK_STATUSLOWBATTERY_STORE

#define SENSOR_LEAK_RESTORE_CHAR_FROM_FLASH  \
                    SENSOR_LEAK_STORE_CHAR_VALUE_T_SENSOR_LEAK_DETECTED_RESTORE  \
                    SENSOR_LEAK_STORE_CHAR_VALUE_T_SENSOR_LEAK_STATUSACTIVE_RESTORE   \
                    SENSOR_LEAK_STORE_CHAR_VALUE_T_SENSOR_LEAK_STATUSFAULT_RESTORE  \
                    SENSOR_LEAK_STORE_CHAR_VALUE_T_SENSOR_LEAK_STATUSTAMPERED_RESTORE   \
                    SENSOR_LEAK_STORE_CHAR_VALUE_T_SENSOR_LEAK_STATUSLOWBATTERY_RESTORE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN unsigned char hSensor_Leak_Detected;
EXTERN unsigned char hSensor_Leak_StatusActive;
EXTERN unsigned char hSensor_Leak_StatusFault;
EXTERN unsigned char hSensor_Leak_StatusTampered;
EXTERN unsigned char hSensor_Leak_StatusLowBattery;

EXTERN unsigned short ccc_hSensor_Leak_Detected;
EXTERN unsigned short ccc_hSensor_Leak_StatusActive;
EXTERN unsigned short ccc_hSensor_Leak_StatusFault;
EXTERN unsigned short ccc_hSensor_Leak_StatusTampered;
EXTERN unsigned short ccc_hSensor_Leak_StatusLowBattery;

#if SENSORLEAK_DETECTED_BRC_ENABLE
    #define BC_PARA_SENSORLEAK_DETECTED ((u8*)&bc_para_hSensor_Leak_Detected)
    EXTERN unsigned char bc_para_hSensor_Leak_Detected;
#else
    #define BC_PARA_SENSORLEAK_DETECTED 0
#endif

#if SENSORLEAK_STATUSACTIVE_BRC_ENABLE
    #define BC_PARA_SENSORLEAK_STATUSACTIVE ((u8*)&bc_para_hSensor_Leak_StatusActive)
    EXTERN unsigned char bc_para_hSensor_Leak_StatusActive;
#else
    #define BC_PARA_SENSORLEAK_STATUSACTIVE 0
#endif

#if SENSORLEAK_STATUSFAULT_BRC_ENABLE
    #define BC_PARA_SENSORLEAK_STATUSFAULT ((u8*)&bc_para_hSensor_Leak_StatusFault)
    EXTERN unsigned char bc_para_hSensor_Leak_StatusFault;
#else
    #define BC_PARA_SENSORLEAK_STATUSFAULT 0
#endif

#if SENSORLEAK_STATUSTAMPERED_BRC_ENABLE
    #define BC_PARA_SENSORLEAK_STATUSTAMPERED ((u8*)&bc_para_hSensor_Leak_StatusTampered)
    EXTERN unsigned char bc_para_hSensor_Leak_StatusTampered;
#else
    #define BC_PARA_SENSORLEAK_STATUSTAMPERED 0
#endif

#if SENSORLEAK_STATUSLOWBATTERY_BRC_ENABLE
    #define BC_PARA_SENSORLEAK_STATUSLOWBATTERY ((u8*)&bc_para_hSensor_Leak_StatusLowBattery)
    EXTERN unsigned char bc_para_hSensor_Leak_StatusLowBattery;
#else
    #define BC_PARA_SENSORLEAK_STATUSLOWBATTERY 0
#endif

#if SENSOR_LEAK_LINKED_SERVICES_ENABLE
	#define SENSOR_LEAK_LINKED_SVCS			hSensorLeak_linked_svcs
	#define SENSOR_LEAK_LINKED_SVCS_LENGTH 	(sizeof(hSensorLeak_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * hSensorLeak_linked_svcs[];
#else
	#define SENSOR_LEAK_LINKED_SVCS			0
	#define SENSOR_LEAK_LINKED_SVCS_LENGTH 	0
#endif

EXTERN const u8 hSensor_LeakServiceName[18];

EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Leak_Detected_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Leak_StatusActive_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Leak_StatusFault_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Leak_StatusTampered_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Leak_StatusLowBattery_R_S;


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN int hSensor_Leak_DetectedReadCallback(void *pp);
EXTERN int hSensor_Leak_StatusActiveReadCallback(void *pp);
EXTERN int hSensor_Leak_StatusFaultReadCallback(void *pp);
EXTERN int hSensor_Leak_StatusTamperedReadCallback(void *pp);
EXTERN int hSensor_Leak_StatusLowBatteryReadCallback(void *pp);
EXTERN int ccc_hSensor_Leak_DetectedWriteCB(void *pp);
EXTERN int ccc_hSensor_Leak_StatusActiveWriteCB(void *pp);
EXTERN int ccc_hSensor_Leak_StatusFaultWriteCB(void *pp);
EXTERN int ccc_hSensor_Leak_StatusTamperedWriteCB(void *pp);
EXTERN int ccc_hSensor_Leak_StatusLowBatteryWriteCB(void *pp);


#endif /* _HK_SENSOR_LEAK_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
