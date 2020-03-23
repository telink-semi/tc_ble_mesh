/********************************************************************************************************
 * @file     hk_SensorOccupancyService.h
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
#ifndef _HK_SENSOR_OCCUPANCY_SERVICE_H_
#define _HK_SENSOR_OCCUPANCY_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */
//-#include "../../homekit_src/homekit_inc.h"


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_SENSOR_OCCUPANCY_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif


#define CHAR_SENSOR_OCCUPANCY_SVC_SIGN_ENABLE      0
#if CHAR_SENSOR_OCCUPANCY_SVC_SIGN_ENABLE
    #define SENSOR_OCCUPANCY_AS_PRIMARY_SERVICE    		0
    #define SENSOR_OCCUPANCY_IS_HIDDEN_SERVICE     		0
    #define SENSOR_OCCUPANCY_LINKED_SERVICES_ENABLE		0
    EXTERN const hap_characteristic_desc_t hap_desc_char_sensor_occupancy_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_sensor_occupancy_A5_R_S;
    EXTERN unsigned char VAR_SENSOR_OCCUPANCY_UNKOWN;
#else
    #define SENSOR_OCCUPANCY_AS_PRIMARY_SERVICE    		0
    #define SENSOR_OCCUPANCY_IS_HIDDEN_SERVICE     		0
    #define SENSOR_OCCUPANCY_LINKED_SERVICES_ENABLE		0
#endif


#define CHAR_SENSOROCCUPANCY_STATUSACTIVE_ENABLE       0
#define CHAR_SENSOROCCUPANCY_STATUSFAULT_ENABLE        0
#define CHAR_SENSOROCCUPANCY_STATUSTAMPERED_ENABLE     0
#define CHAR_SENSOROCCUPANCY_STATUSLOWBATTERY_ENABLE   0

#define SENSOROCCUPANCY_DETECTED_IND_ENABLE          0
#define SENSOROCCUPANCY_STATUSACTIVE_IND_ENABLE      0
#define SENSOROCCUPANCY_STATUSFAULT_IND_ENABLE       0
#define SENSOROCCUPANCY_STATUSTAMPERED_IND_ENABLE    0
#define SENSOROCCUPANCY_STATUSLOWBATTERY_IND_ENABLE  0

#define SENSOROCCUPANCY_DETECTED_BRC_ENABLE          0
#define SENSOROCCUPANCY_STATUSACTIVE_BRC_ENABLE      0
#define SENSOROCCUPANCY_STATUSFAULT_BRC_ENABLE       0
#define SENSOROCCUPANCY_STATUSTAMPERED_BRC_ENABLE    0
#define SENSOROCCUPANCY_STATUSLOWBATTERY_BRC_ENABLE  0


#define SENSOROCCUPANCY_DETECTED_NO 3
#if SENSOROCCUPANCY_DETECTED_IND_ENABLE
    #define SENSOROCCUPANCY_DETECTED_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorOccupancy_Detected), ccc_hSensorOccupancy_DetectedWriteCB, 0},
    #define SENSOROCCUPANCY_DETECTED_IND_NO  1
#else
    #define SENSOROCCUPANCY_DETECTED_IND_DESC
    #define SENSOROCCUPANCY_DETECTED_IND_NO  0
#endif


#if CHAR_SENSOROCCUPANCY_STATUSACTIVE_ENABLE
    #define SENSOROCCUPANCY_STATUSACTIVE_NO 3
    #define CHAR_SENSOROCCUPANCY_STATUSACTIVE_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Occupancy_StatusActive_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Occupancy_StatusActive),0, (u8*)(charStatusActiveUUID), (u8*)&hSensor_Occupancy_StatusActive, 0, hSensor_Occupancy_StatusActiveReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_OCCUPANCY_INST_NO + 3), 0, 0},    \
            SENSOROCCUPANCY_STATUSACTIVE_IND_DESC

	#if SENSOROCCUPANCY_STATUSACTIVE_IND_ENABLE
	    #define SENSOROCCUPANCY_STATUSACTIVE_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorOccupancy_StatusActive), ccc_hSensorOccupancy_StatusActiveWriteCB, 0},
	    #define SENSOROCCUPANCY_STATUSACTIVE_IND_NO  1
	#else
	    #define SENSOROCCUPANCY_STATUSACTIVE_IND_DESC
	    #define SENSOROCCUPANCY_STATUSACTIVE_IND_NO  0
	#endif
#else
    #define SENSOROCCUPANCY_STATUSACTIVE_NO 0
    #define CHAR_SENSOROCCUPANCY_STATUSACTIVE_ATT_TABLE
    #define SENSOROCCUPANCY_STATUSACTIVE_IND_NO  0
#endif

#if CHAR_SENSOROCCUPANCY_STATUSFAULT_ENABLE
    #define SENSOROCCUPANCY_STATUSFAULT_NO 3
    #define CHAR_SENSOROCCUPANCY_STATUSFAULT_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Occupancy_StatusFault_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Occupancy_StatusFault),0, (u8*)(charStatusFaultUUID), (u8*)&hSensor_Occupancy_StatusFault, 0, hSensor_Occupancy_StatusFaultReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_OCCUPANCY_INST_NO + 4), 0, 0},    \
            SENSOROCCUPANCY_STATUSFAULT_IND_DESC

	#if SENSOROCCUPANCY_STATUSFAULT_IND_ENABLE
	    #define SENSOROCCUPANCY_STATUSFAULT_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorOccupancy_StatusFault), ccc_hSensorOccupancy_StatusFaultWriteCB, 0},
	    #define SENSOROCCUPANCY_STATUSFAULT_IND_NO  1
	#else
	    #define SENSOROCCUPANCY_STATUSFAULT_IND_DESC
	    #define SENSOROCCUPANCY_STATUSFAULT_IND_NO  0
	#endif
#else
    #define SENSOROCCUPANCY_STATUSFAULT_NO 0
    #define CHAR_SENSOROCCUPANCY_STATUSFAULT_ATT_TABLE
    #define SENSOROCCUPANCY_STATUSFAULT_IND_NO  0
#endif


#if CHAR_SENSOROCCUPANCY_STATUSTAMPERED_ENABLE
    #define SENSOROCCUPANCY_STATUSTAMPERED_NO 3
    #define CHAR_SENSOROCCUPANCY_STATUSTAMPERED_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Occupancy_StatusTampered_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Occupancy_StatusTampered),0, (u8*)(charStatusTamperedUUID), (u8*)&hSensor_Occupancy_StatusTampered, 0, hSensor_Occupancy_StatusTamperedReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_OCCUPANCY_INST_NO + 5), 0, 0},    \
            SENSOROCCUPANCY_STATUSTAMPERED_IND_DESC

	#if SENSOROCCUPANCY_STATUSTAMPERED_IND_ENABLE
	    #define SENSOROCCUPANCY_STATUSTAMPERED_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorOccupancy_StatusTampered), ccc_hSensorOccupancy_StatusTamperedWriteCB, 0},
	    #define SENSOROCCUPANCY_STATUSTAMPERED_IND_NO  1
	#else
	    #define SENSOROCCUPANCY_STATUSTAMPERED_IND_DESC
	    #define SENSOROCCUPANCY_STATUSTAMPERED_IND_NO  0
	#endif
#else
    #define SENSOROCCUPANCY_STATUSTAMPERED_NO 0
    #define CHAR_SENSOROCCUPANCY_STATUSTAMPERED_ATT_TABLE
    #define SENSOROCCUPANCY_STATUSTAMPERED_IND_NO  0
#endif

#if CHAR_SENSOROCCUPANCY_STATUSLOWBATTERY_ENABLE
    #define SENSOROCCUPANCY_STATUSLOWBATTERY_NO 3
    #define CHAR_SENSOROCCUPANCY_STATUSLOWBATTERY_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Occupancy_StatusLowBattery_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Occupancy_StatusLowBattery),0, (u8*)(charStatusLowBatteryUUID), (u8*)&hSensor_Occupancy_StatusLowBattery, 0, hSensor_Occupancy_StatusLowBatteryReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_OCCUPANCY_INST_NO + 6), 0, 0},    \
            SENSOROCCUPANCY_STATUSLOWBATTERY_IND_DESC

	#if SENSOROCCUPANCY_STATUSLOWBATTERY_IND_ENABLE
	    #define SENSOROCCUPANCY_STATUSLOWBATTERY_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorOccupancy_StatusLowBattery), ccc_hSensorOccupancy_StatusLowBatteryWriteCB, 0},
	    #define SENSOROCCUPANCY_STATUSLOWBATTERY_IND_NO  1
	#else
	    #define SENSOROCCUPANCY_STATUSLOWBATTERY_IND_DESC
	    #define SENSOROCCUPANCY_STATUSLOWBATTERY_IND_NO  0
	#endif
#else
    #define SENSOROCCUPANCY_STATUSLOWBATTERY_NO 0
    #define CHAR_SENSOROCCUPANCY_STATUSLOWBATTERY_ATT_TABLE
    #define SENSOROCCUPANCY_STATUSLOWBATTERY_IND_NO  0
#endif


#if CHAR_SENSOR_OCCUPANCY_SVC_SIGN_ENABLE
#define CHAR_SENSOR_OCCUPANCY_SVC_A5_SIGN_TABLE   \
    {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_sensor_occupancy_A5_R_S, &hap_desc_service_sensor_occupancy_A5_R_S},  \
    {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_SENSOR_OCCUPANCY_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_SENSOR_OCCUPANCY_UNKOWN, 0, 0},    \
        {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_OCCUPANCY_INST_NO + 7), 0, 0},
#define SENSOR_OCCUPANCY_SVC_SIGN_NO   3
#else
#define CHAR_SENSOR_OCCUPANCY_SVC_A5_SIGN_TABLE
#define SENSOR_OCCUPANCY_SVC_SIGN_NO   0
#endif

#define SENSOR_OCCUPANCY_NO_DELTA   SENSOROCCUPANCY_DETECTED_IND_NO \
                                    +SENSOROCCUPANCY_STATUSACTIVE_IND_NO    \
                                    +SENSOROCCUPANCY_STATUSFAULT_IND_NO \
                                    +SENSOROCCUPANCY_STATUSTAMPERED_IND_NO  \
                                    +SENSOROCCUPANCY_STATUSLOWBATTERY_IND_NO

#define SENSOR_OCCUPANCY_NO         9 +SENSOROCCUPANCY_STATUSACTIVE_NO	\
									+SENSOROCCUPANCY_STATUSFAULT_NO \
									+SENSOROCCUPANCY_STATUSTAMPERED_NO	\
									+SENSOROCCUPANCY_STATUSLOWBATTERY_NO \
									+SENSOR_OCCUPANCY_SVC_SIGN_NO
									
#define SENSOR_OCCUPANCY_HANDLE_NUM (SENSOR_OCCUPANCY_NO + SENSOR_OCCUPANCY_NO_DELTA)


#define HOMEKIT_SENSOR_OCCUPANCY_SERVICE_ATT_TABLE    \
    {SENSOR_OCCUPANCY_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(sensor_occupancyServiceUUID),sizeof(sensor_occupancyServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(sensor_occupancyServiceUUID), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(insts + SENSOR_OCCUPANCY_INST_NO), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_NAME, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(hSensor_OccupancyServiceName), 0, (u8*)(nameUUID), (u8*)(hSensor_OccupancyServiceName), 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + SENSOR_OCCUPANCY_INST_NO + 1), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Occupancy_Detected_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Occupancy_Detected),0, (u8*)(charOccupancyDetectedUUID), (u8*)&hSensor_Occupancy_Detected, 0, hSensor_Occupancy_DetectedReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_OCCUPANCY_INST_NO + 2), 0, 0},    \
            SENSOROCCUPANCY_DETECTED_IND_DESC \
            CHAR_SENSOROCCUPANCY_STATUSACTIVE_ATT_TABLE \
            CHAR_SENSOROCCUPANCY_STATUSFAULT_ATT_TABLE \
            CHAR_SENSOROCCUPANCY_STATUSTAMPERED_ATT_TABLE \
            CHAR_SENSOROCCUPANCY_STATUSLOWBATTERY_ATT_TABLE \
        	CHAR_SENSOR_OCCUPANCY_SVC_A5_SIGN_TABLE


#define SENSOROCCUPANCY_HANDLE_COMMON_NUM     8

#define SENSOROCCUPANCY_HANDLE_NUM_DETECTED          SENSOR_OCCUPANCY_HANDLE_START_NUM +SENSOROCCUPANCY_HANDLE_COMMON_NUM
#define SENSOROCCUPANCY_HANDLE_NUM_STATUSACTIVE      SENSOROCCUPANCY_HANDLE_NUM_DETECTED +SENSOROCCUPANCY_DETECTED_NO +SENSOROCCUPANCY_DETECTED_IND_NO
#define SENSOROCCUPANCY_HANDLE_NUM_STATUSFAULT       SENSOROCCUPANCY_HANDLE_NUM_STATUSACTIVE +SENSOROCCUPANCY_STATUSACTIVE_NO +SENSOROCCUPANCY_STATUSACTIVE_IND_NO
#define SENSOROCCUPANCY_HANDLE_NUM_STATUSTAMPERED    SENSOROCCUPANCY_HANDLE_NUM_STATUSFAULT +SENSOROCCUPANCY_STATUSFAULT_NO +SENSOROCCUPANCY_STATUSFAULT_IND_NO
#define SENSOROCCUPANCY_HANDLE_NUM_STATUSLOWBATTERY  SENSOROCCUPANCY_HANDLE_NUM_STATUSTAMPERED +SENSOROCCUPANCY_STATUSTAMPERED_NO +SENSOROCCUPANCY_STATUSTAMPERED_IND_NO
#define SENSOROCCUPANCY_HANDLE_NUM_THE_END


#define SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T_SENSOR_OCCUPANCY_DETECTED                 u32 Sensor_Occupancy_Detected:1;  /* 0~1 */
#define SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T_SENSOR_OCCUPANCY_DETECTED_STORE               STORE_CHAR(Sensor_Occupancy_Detected,hSensor_Occupancy_Detected)
#define SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T_SENSOR_OCCUPANCY_DETECTED_RESTORE             RESTORE_CHAR(Sensor_Occupancy_Detected,hSensor_Occupancy_Detected)

#define SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T_SENSOR_OCCUPANCY_STATUSACTIVE             u32 Sensor_Occupancy_StatusActive:1;  /* 0~1 */
#define SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T_SENSOR_OCCUPANCY_STATUSACTIVE_STORE           STORE_CHAR(Sensor_Occupancy_StatusActive,hSensor_Occupancy_StatusActive)
#define SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T_SENSOR_OCCUPANCY_STATUSACTIVE_RESTORE         RESTORE_CHAR(Sensor_Occupancy_StatusActive,hSensor_Occupancy_StatusActive)

#define SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T_SENSOR_OCCUPANCY_STATUSFAULT              u32 Sensor_Occupancy_StatusFault:1;  /* 0~1 */
#define SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T_SENSOR_OCCUPANCY_STATUSFAULT_STORE            STORE_CHAR(Sensor_Occupancy_StatusFault,hSensor_Occupancy_StatusFault)
#define SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T_SENSOR_OCCUPANCY_STATUSFAULT_RESTORE          RESTORE_CHAR(Sensor_Occupancy_StatusFault,hSensor_Occupancy_StatusFault)

#define SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T_SENSOR_OCCUPANCY_STATUSTAMPERED           u32 Sensor_Occupancy_StatusTampered:1;  /* 0~1 */
#define SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T_SENSOR_OCCUPANCY_STATUSTAMPERED_STORE         STORE_CHAR(Sensor_Occupancy_StatusTampered,hSensor_Occupancy_StatusTampered)
#define SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T_SENSOR_OCCUPANCY_STATUSTAMPERED_RESTORE       RESTORE_CHAR(Sensor_Occupancy_StatusTampered,hSensor_Occupancy_StatusTampered)

#define SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T_SENSOR_OCCUPANCY_STATUSLOWBATTERY         u32 Sensor_Occupancy_StatusLowBattery:1;  /* 0~1 */
#define SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T_SENSOR_OCCUPANCY_STATUSLOWBATTERY_STORE       STORE_CHAR(Sensor_Occupancy_StatusLowBattery,hSensor_Occupancy_StatusLowBattery)
#define SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T_SENSOR_OCCUPANCY_STATUSLOWBATTERY_RESTORE     RESTORE_CHAR(Sensor_Occupancy_StatusLowBattery,hSensor_Occupancy_StatusLowBattery)

#define SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T   \
                    SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T_SENSOR_OCCUPANCY_DETECTED  \
                    SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T_SENSOR_OCCUPANCY_STATUSACTIVE   \
                    SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T_SENSOR_OCCUPANCY_STATUSFAULT  \
                    SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T_SENSOR_OCCUPANCY_STATUSTAMPERED   \
                    SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T_SENSOR_OCCUPANCY_STATUSLOWBATTERY

#define SENSOR_OCCUPANCY_STORE_CHAR_TO_FLASH  \
                    SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T_SENSOR_OCCUPANCY_DETECTED_STORE    \
                    SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T_SENSOR_OCCUPANCY_STATUSACTIVE_STORE \
                    SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T_SENSOR_OCCUPANCY_STATUSFAULT_STORE    \
                    SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T_SENSOR_OCCUPANCY_STATUSTAMPERED_STORE \
                    SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T_SENSOR_OCCUPANCY_STATUSLOWBATTERY_STORE

#define SENSOR_OCCUPANCY_RESTORE_CHAR_FROM_FLASH  \
                    SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T_SENSOR_OCCUPANCY_DETECTED_RESTORE  \
                    SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T_SENSOR_OCCUPANCY_STATUSACTIVE_RESTORE   \
                    SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T_SENSOR_OCCUPANCY_STATUSFAULT_RESTORE  \
                    SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T_SENSOR_OCCUPANCY_STATUSTAMPERED_RESTORE   \
                    SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T_SENSOR_OCCUPANCY_STATUSLOWBATTERY_RESTORE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN unsigned char hSensor_Occupancy_Detected;
EXTERN unsigned char hSensor_Occupancy_StatusActive;
EXTERN unsigned char hSensor_Occupancy_StatusFault;
EXTERN unsigned char hSensor_Occupancy_StatusTampered;
EXTERN unsigned char hSensor_Occupancy_StatusLowBattery;

EXTERN unsigned short ccc_hSensorOccupancy_Detected;
EXTERN unsigned short ccc_hSensorOccupancy_StatusActive;
EXTERN unsigned short ccc_hSensorOccupancy_StatusFault;
EXTERN unsigned short ccc_hSensorOccupancy_StatusTampered;
EXTERN unsigned short ccc_hSensorOccupancy_StatusLowBattery;

#if SENSOROCCUPANCY_DETECTED_BRC_ENABLE
    #define BC_PARA_SENSOROCCUPANCY_DETECTED ((u8*)&bc_para_hSensorOccupancy_Detected)
    EXTERN unsigned char bc_para_hSensorOccupancy_Detected;
#else
    #define BC_PARA_SENSOROCCUPANCY_DETECTED 0
#endif

#if SENSOROCCUPANCY_STATUSACTIVE_BRC_ENABLE
    #define BC_PARA_SENSOROCCUPANCY_STATUSACTIVE ((u8*)&bc_para_hSensorOccupancy_StatusActive)
    EXTERN unsigned char bc_para_hSensorOccupancy_StatusActive;
#else
    #define BC_PARA_SENSOROCCUPANCY_STATUSACTIVE 0
#endif

#if SENSOROCCUPANCY_STATUSFAULT_BRC_ENABLE
    #define BC_PARA_SENSOROCCUPANCY_STATUSFAULT ((u8*)&bc_para_hSensorOccupancy_StatusFault)
    EXTERN unsigned char bc_para_hSensorOccupancy_StatusFault;
#else
    #define BC_PARA_SENSOROCCUPANCY_STATUSFAULT 0
#endif

#if SENSOROCCUPANCY_STATUSTAMPERED_BRC_ENABLE
    #define BC_PARA_SENSOROCCUPANCY_STATUSTAMPERED ((u8*)&bc_para_hSensorOccupancy_StatusTampered)
    EXTERN unsigned char bc_para_hSensorOccupancy_StatusTampered;
#else
    #define BC_PARA_SENSOROCCUPANCY_STATUSTAMPERED 0
#endif

#if SENSOROCCUPANCY_STATUSLOWBATTERY_BRC_ENABLE
    #define BC_PARA_SENSOROCCUPANCY_STATUSLOWBATTERY ((u8*)&bc_para_hSensorOccupancy_StatusLowBattery)
    EXTERN unsigned char bc_para_hSensorOccupancy_StatusLowBattery;
#else
    #define BC_PARA_SENSOROCCUPANCY_STATUSLOWBATTERY 0
#endif

#if SENSOR_OCCUPANCY_LINKED_SERVICES_ENABLE
	#define SENSOR_OCCUPANCY_LINKED_SVCS			hSensorOccupancy_linked_svcs
	#define SENSOR_OCCUPANCY_LINKED_SVCS_LENGTH 	(sizeof(hSensorOccupancy_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * hSensorOccupancy_linked_svcs[];
#else
	#define SENSOR_OCCUPANCY_LINKED_SVCS			0
	#define SENSOR_OCCUPANCY_LINKED_SVCS_LENGTH 	0
#endif

EXTERN const u8 hSensor_OccupancyServiceName[23];

EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Occupancy_Detected_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Occupancy_StatusActive_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Occupancy_StatusFault_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Occupancy_StatusTampered_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Occupancy_StatusLowBattery_R_S;


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN int hSensor_Occupancy_DetectedReadCallback(void *pp);
EXTERN int hSensor_Occupancy_StatusActiveReadCallback(void *pp);
EXTERN int hSensor_Occupancy_StatusFaultReadCallback(void *pp);
EXTERN int hSensor_Occupancy_StatusTamperedReadCallback(void *pp);
EXTERN int hSensor_Occupancy_StatusLowBatteryReadCallback(void *pp);
EXTERN int ccc_hSensorOccupancy_DetectedWriteCB(void *pp);
EXTERN int ccc_hSensorOccupancy_StatusActiveWriteCB(void *pp);
EXTERN int ccc_hSensorOccupancy_StatusFaultWriteCB(void *pp);
EXTERN int ccc_hSensorOccupancy_StatusTamperedWriteCB(void *pp);
EXTERN int ccc_hSensorOccupancy_StatusLowBatteryWriteCB(void *pp);


#endif /* _HK_SENSOR_OCCUPANCY_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
