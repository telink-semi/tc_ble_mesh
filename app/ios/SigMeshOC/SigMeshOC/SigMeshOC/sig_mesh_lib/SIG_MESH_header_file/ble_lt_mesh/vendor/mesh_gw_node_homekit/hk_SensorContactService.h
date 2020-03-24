/********************************************************************************************************
 * @file     hk_SensorContactService.h
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
#ifndef _HK_SENSOR_CONTACT_SERVICE_H_
#define _HK_SENSOR_CONTACT_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */
//-#include "../../homekit_src/homekit_inc.h"


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_SENSOR_CONTACT_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif


#define CHAR_SENSOR_CONTACT_SVC_SIGN_ENABLE      0
#if CHAR_SENSOR_CONTACT_SVC_SIGN_ENABLE
    #define SENSOR_CONTACT_AS_PRIMARY_SERVICE    	0
    #define SENSOR_CONTACT_IS_HIDDEN_SERVICE     	0
    #define SENSOR_CONTACT_LINKED_SERVICES_ENABLE	0
    EXTERN const hap_characteristic_desc_t hap_desc_char_sensor_contact_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_sensor_contact_A5_R_S;
    EXTERN unsigned char VAR_SENSOR_CONTACT_UNKOWN;
#else
    #define SENSOR_CONTACT_AS_PRIMARY_SERVICE    	0
    #define SENSOR_CONTACT_IS_HIDDEN_SERVICE     	0
    #define SENSOR_CONTACT_LINKED_SERVICES_ENABLE	0
#endif

#define CHAR_SENSORCONTACT_STATUSACTIVE_ENABLE       0
#define CHAR_SENSORCONTACT_STATUSFAULT_ENABLE        0
#define CHAR_SENSORCONTACT_STATUSTAMPERED_ENABLE     0
#define CHAR_SENSORCONTACT_STATUSLOWBATTERY_ENABLE   0


#define SENSORCONTACT_STATE_IND_ENABLE              0
#define SENSORCONTACT_STATUSACTIVE_IND_ENABLE       0
#define SENSORCONTACT_STATUSFAULT_IND_ENABLE        0
#define SENSORCONTACT_STATUSTAMPERED_IND_ENABLE     0
#define SENSORCONTACT_STATUSLOWBATTERY_IND_ENABLE   0

#define SENSORCONTACT_STATE_BRC_ENABLE              0
#define SENSORCONTACT_STATUSACTIVE_BRC_ENABLE       0
#define SENSORCONTACT_STATUSFAULT_BRC_ENABLE        0
#define SENSORCONTACT_STATUSTAMPERED_BRC_ENABLE     0
#define SENSORCONTACT_STATUSLOWBATTERY_BRC_ENABLE   0


#define SENSORCONTACT_STATE_NO 3
#if SENSORCONTACT_STATE_IND_ENABLE
    #define SENSORCONTACT_STATE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorContact_State), ccc_hSensorContact_StateWriteCB, 0},
    #define SENSORCONTACT_STATE_IND_NO  1
#else
    #define SENSORCONTACT_STATE_IND_DESC
    #define SENSORCONTACT_STATE_IND_NO  0
#endif

#if CHAR_SENSORCONTACT_STATUSACTIVE_ENABLE
    #define SENSORCONTACT_STATUSACTIVE_NO 3
    #define CHAR_SENSORCONTACT_STATUSACTIVE_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Contact_StatusActive_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Contact_StatusActive),0, (u8*)(charStatusActiveUUID), (u8*)&hSensor_Contact_StatusActive, 0, hSensor_Contact_StatusActiveReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_CONTACT_INST_NO + 3), 0, 0},    \
            SENSORCONTACT_STATUSACTIVE_IND_DESC

	#if SENSORCONTACT_STATUSACTIVE_IND_ENABLE
	    #define SENSORCONTACT_STATUSACTIVE_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorContact_StatusActive), ccc_hSensorContact_StatusActiveWriteCB, 0},
	    #define SENSORCONTACT_STATUSACTIVE_IND_NO  1
	#else
	    #define SENSORCONTACT_STATUSACTIVE_IND_DESC
	    #define SENSORCONTACT_STATUSACTIVE_IND_NO  0
	#endif
#else
    #define SENSORCONTACT_STATUSACTIVE_NO 0
    #define CHAR_SENSORCONTACT_STATUSACTIVE_ATT_TABLE
    #define SENSORCONTACT_STATUSACTIVE_IND_NO  0
#endif

#if CHAR_SENSORCONTACT_STATUSFAULT_ENABLE
    #define SENSORCONTACT_STATUSFAULT_NO 3
    #define CHAR_SENSORCONTACT_STATUSFAULT_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Contact_StatusFault_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Contact_StatusFault),0, (u8*)(charStatusFaultUUID), (u8*)&hSensor_Contact_StatusFault, 0, hSensor_Contact_StatusFaultReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_CONTACT_INST_NO + 4), 0, 0},    \
            SENSORCONTACT_STATUSFAULT_IND_DESC

	#if SENSORCONTACT_STATUSFAULT_IND_ENABLE
	    #define SENSORCONTACT_STATUSFAULT_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorContact_StatusFault), ccc_hSensorContact_StatusFaultWriteCB, 0},
	    #define SENSORCONTACT_STATUSFAULT_IND_NO  1
	#else
	    #define SENSORCONTACT_STATUSFAULT_IND_DESC
	    #define SENSORCONTACT_STATUSFAULT_IND_NO  0
	#endif
#else
    #define SENSORCONTACT_STATUSFAULT_NO 0
    #define CHAR_SENSORCONTACT_STATUSFAULT_ATT_TABLE
    #define SENSORCONTACT_STATUSFAULT_IND_NO  0
#endif


#if CHAR_SENSORCONTACT_STATUSTAMPERED_ENABLE
    #define SENSORCONTACT_STATUSTAMPERED_NO 3
    #define CHAR_SENSORCONTACT_STATUSTAMPERED_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Contact_StatusTampered_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Contact_StatusTampered),0, (u8*)(charStatusTamperedUUID), (u8*)&hSensor_Contact_StatusTampered, 0, hSensor_Contact_StatusTamperedReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_CONTACT_INST_NO + 5), 0, 0},    \
            SENSORCONTACT_STATUSTAMPERED_IND_DESC

	#if SENSORCONTACT_STATUSTAMPERED_IND_ENABLE
	    #define SENSORCONTACT_STATUSTAMPERED_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorContact_StatusTampered), ccc_hSensorContact_StatusTamperedWriteCB, 0},
	    #define SENSORCONTACT_STATUSTAMPERED_IND_NO  1
	#else
	    #define SENSORCONTACT_STATUSTAMPERED_IND_DESC
	    #define SENSORCONTACT_STATUSTAMPERED_IND_NO  0
	#endif
#else
    #define SENSORCONTACT_STATUSTAMPERED_NO 0
    #define CHAR_SENSORCONTACT_STATUSTAMPERED_ATT_TABLE
    #define SENSORCONTACT_STATUSTAMPERED_IND_NO  0
#endif

#if CHAR_SENSORCONTACT_STATUSLOWBATTERY_ENABLE
    #define SENSORCONTACT_STATUSLOWBATTERY_NO 3
    #define CHAR_SENSORCONTACT_STATUSLOWBATTERY_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Contact_StatusLowBattery_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Contact_StatusLowBattery),0, (u8*)(charStatusLowBatteryUUID), (u8*)&hSensor_Contact_StatusLowBattery, 0, hSensor_Contact_StatusLowBatteryReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_CONTACT_INST_NO + 6), 0, 0},    \
            SENSORCONTACT_STATUSLOWBATTERY_IND_DESC

	#if SENSORCONTACT_STATUSLOWBATTERY_IND_ENABLE
	    #define SENSORCONTACT_STATUSLOWBATTERY_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSensorContact_StatusLowBattery), ccc_hSensorContact_StatusLowBatteryWriteCB, 0},
	    #define SENSORCONTACT_STATUSLOWBATTERY_IND_NO  1
	#else
	    #define SENSORCONTACT_STATUSLOWBATTERY_IND_DESC
	    #define SENSORCONTACT_STATUSLOWBATTERY_IND_NO  0
	#endif
#else
    #define SENSORCONTACT_STATUSLOWBATTERY_NO 0
    #define CHAR_SENSORCONTACT_STATUSLOWBATTERY_ATT_TABLE
    #define SENSORCONTACT_STATUSLOWBATTERY_IND_NO  0
#endif


#if CHAR_SENSOR_CONTACT_SVC_SIGN_ENABLE
#define CHAR_SENSOR_CONTACT_SVC_A5_SIGN_TABLE   \
    {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_sensor_contact_A5_R_S, &hap_desc_service_sensor_contact_A5_R_S},  \
    {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_SENSOR_CONTACT_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_SENSOR_CONTACT_UNKOWN, 0, 0},    \
        {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_CONTACT_INST_NO + 7), 0, 0},
#define SENSOR_CONTACT_SVC_SIGN_NO   3
#else
#define CHAR_SENSOR_CONTACT_SVC_A5_SIGN_TABLE
#define SENSOR_CONTACT_SVC_SIGN_NO   0
#endif

#define SENSOR_CONTACT_NO_DELTA SENSORCONTACT_STATE_IND_NO  \
                                +SENSORCONTACT_STATUSACTIVE_IND_NO  \
                                +SENSORCONTACT_STATUSFAULT_IND_NO   \
                                +SENSORCONTACT_STATUSTAMPERED_IND_NO    \
                                +SENSORCONTACT_STATUSLOWBATTERY_IND_NO

#define SENSOR_CONTACT_NO       9 +SENSORCONTACT_STATUSACTIVE_NO	\
								+SENSORCONTACT_STATUSFAULT_NO	\
								+SENSORCONTACT_STATUSTAMPERED_NO	\
								+SENSORCONTACT_STATUSLOWBATTERY_NO	\
								+SENSOR_CONTACT_SVC_SIGN_NO
								
#define SENSOR_CONTACT_HANDLE_NUM (SENSOR_CONTACT_NO + SENSOR_CONTACT_NO_DELTA)


#define HOMEKIT_SENSOR_CONTACT_SERVICE_ATT_TABLE    \
    {SENSOR_CONTACT_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(sensor_contactServiceUUID),sizeof(sensor_contactServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(sensor_contactServiceUUID), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(insts + SENSOR_CONTACT_INST_NO), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_NAME, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(hSensor_ContactServiceName), 0, (u8*)(nameUUID), (u8*)(hSensor_ContactServiceName), 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + SENSOR_CONTACT_INST_NO + 1), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hSensor_Contact_State_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSensor_Contact_State),0, (u8*)(charContactStateUUID), (u8*)&hSensor_Contact_State, 0, hSensor_Contact_StateReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SENSOR_CONTACT_INST_NO + 2), 0, 0},    \
            SENSORCONTACT_STATE_IND_DESC \
            CHAR_SENSORCONTACT_STATUSACTIVE_ATT_TABLE \
            CHAR_SENSORCONTACT_STATUSFAULT_ATT_TABLE \
            CHAR_SENSORCONTACT_STATUSTAMPERED_ATT_TABLE \
            CHAR_SENSORCONTACT_STATUSLOWBATTERY_ATT_TABLE \
        	CHAR_SENSOR_CONTACT_SVC_A5_SIGN_TABLE


#define SENSORCONTACT_HANDLE_COMMON_NUM     8

#define SENSORCONTACT_HANDLE_NUM_STATE              SENSOR_CONTACT_HANDLE_START_NUM +SENSORCONTACT_HANDLE_COMMON_NUM
#define SENSORCONTACT_HANDLE_NUM_STATUSACTIVE       SENSORCONTACT_HANDLE_NUM_STATE +SENSORCONTACT_STATE_NO +SENSORCONTACT_STATE_IND_NO
#define SENSORCONTACT_HANDLE_NUM_STATUSFAULT        SENSORCONTACT_HANDLE_NUM_STATUSACTIVE +SENSORCONTACT_STATUSACTIVE_NO +SENSORCONTACT_STATUSACTIVE_IND_NO
#define SENSORCONTACT_HANDLE_NUM_STATUSTAMPERED     SENSORCONTACT_HANDLE_NUM_STATUSFAULT +SENSORCONTACT_STATUSFAULT_NO +SENSORCONTACT_STATUSFAULT_IND_NO
#define SENSORCONTACT_HANDLE_NUM_STATUSLOWBATTERY   SENSORCONTACT_HANDLE_NUM_STATUSTAMPERED +SENSORCONTACT_STATUSTAMPERED_NO +SENSORCONTACT_STATUSTAMPERED_IND_NO
#define SENSORCONTACT_HANDLE_NUM_THE_END


#define SENSOR_CONTACT_STORE_CHAR_VALUE_T_SENSOR_CONTACT_STATE                  u32 Sensor_Contact_State:1; /* 0~1 */
#define SENSOR_CONTACT_STORE_CHAR_VALUE_T_SENSOR_CONTACT_STATE_STORE                STORE_CHAR(Sensor_Contact_State,hSensor_Contact_State)
#define SENSOR_CONTACT_STORE_CHAR_VALUE_T_SENSOR_CONTACT_STATE_RESTORE              RESTORE_CHAR(Sensor_Contact_State,hSensor_Contact_State)

#define SENSOR_CONTACT_STORE_CHAR_VALUE_T_SENSOR_CONTACT_STATUSACTIVE           u32 Sensor_Contact_StatusActive:1; /* 0~1 */
#define SENSOR_CONTACT_STORE_CHAR_VALUE_T_SENSOR_CONTACT_STATUSACTIVE_STORE         STORE_CHAR(Sensor_Contact_StatusActive,hSensor_Contact_StatusActive)
#define SENSOR_CONTACT_STORE_CHAR_VALUE_T_SENSOR_CONTACT_STATUSACTIVE_RESTORE       RESTORE_CHAR(Sensor_Contact_StatusActive,hSensor_Contact_StatusActive)

#define SENSOR_CONTACT_STORE_CHAR_VALUE_T_SENSOR_CONTACT_STATUSFAULT            u32 Sensor_Contact_StatusFault:1; /* 0~1 */
#define SENSOR_CONTACT_STORE_CHAR_VALUE_T_SENSOR_CONTACT_STATUSFAULT_STORE          STORE_CHAR(Sensor_Contact_StatusFault,hSensor_Contact_StatusFault)
#define SENSOR_CONTACT_STORE_CHAR_VALUE_T_SENSOR_CONTACT_STATUSFAULT_RESTORE        RESTORE_CHAR(Sensor_Contact_StatusFault,hSensor_Contact_StatusFault)

#define SENSOR_CONTACT_STORE_CHAR_VALUE_T_SENSOR_CONTACT_STATUSTAMPERED         u32 Sensor_Contact_StatusTampered:1; /* 0~1 */
#define SENSOR_CONTACT_STORE_CHAR_VALUE_T_SENSOR_CONTACT_STATUSTAMPERED_STORE       STORE_CHAR(Sensor_Contact_StatusTampered,hSensor_Contact_StatusTampered)
#define SENSOR_CONTACT_STORE_CHAR_VALUE_T_SENSOR_CONTACT_STATUSTAMPERED_RESTORE     RESTORE_CHAR(Sensor_Contact_StatusTampered,hSensor_Contact_StatusTampered)

#define SENSOR_CONTACT_STORE_CHAR_VALUE_T_SENSOR_CONTACT_STATUSLOWBATTERY       u32 Sensor_Contact_StatusLowBattery:1; /* 0~1 */
#define SENSOR_CONTACT_STORE_CHAR_VALUE_T_SENSOR_CONTACT_STATUSLOWBATTERY_STORE     STORE_CHAR(Sensor_Contact_StatusLowBattery,hSensor_Contact_StatusLowBattery)
#define SENSOR_CONTACT_STORE_CHAR_VALUE_T_SENSOR_CONTACT_STATUSLOWBATTERY_RESTORE   RESTORE_CHAR(Sensor_Contact_StatusLowBattery,hSensor_Contact_StatusLowBattery)

#define SENSOR_CONTACT_STORE_CHAR_VALUE_T   \
                    SENSOR_CONTACT_STORE_CHAR_VALUE_T_SENSOR_CONTACT_STATE  \
                    SENSOR_CONTACT_STORE_CHAR_VALUE_T_SENSOR_CONTACT_STATUSACTIVE   \
                    SENSOR_CONTACT_STORE_CHAR_VALUE_T_SENSOR_CONTACT_STATUSFAULT  \
                    SENSOR_CONTACT_STORE_CHAR_VALUE_T_SENSOR_CONTACT_STATUSTAMPERED   \
                    SENSOR_CONTACT_STORE_CHAR_VALUE_T_SENSOR_CONTACT_STATUSLOWBATTERY

#define SENSOR_CONTACT_STORE_CHAR_TO_FLASH  \
                    SENSOR_CONTACT_STORE_CHAR_VALUE_T_SENSOR_CONTACT_STATE_STORE    \
                    SENSOR_CONTACT_STORE_CHAR_VALUE_T_SENSOR_CONTACT_STATUSACTIVE_STORE \
                    SENSOR_CONTACT_STORE_CHAR_VALUE_T_SENSOR_CONTACT_STATUSFAULT_STORE    \
                    SENSOR_CONTACT_STORE_CHAR_VALUE_T_SENSOR_CONTACT_STATUSTAMPERED_STORE \
                    SENSOR_CONTACT_STORE_CHAR_VALUE_T_SENSOR_CONTACT_STATUSLOWBATTERY_STORE

#define SENSOR_CONTACT_RESTORE_CHAR_FROM_FLASH  \
                    SENSOR_CONTACT_STORE_CHAR_VALUE_T_SENSOR_CONTACT_STATE_RESTORE  \
                    SENSOR_CONTACT_STORE_CHAR_VALUE_T_SENSOR_CONTACT_STATUSACTIVE_RESTORE   \
                    SENSOR_CONTACT_STORE_CHAR_VALUE_T_SENSOR_CONTACT_STATUSFAULT_RESTORE  \
                    SENSOR_CONTACT_STORE_CHAR_VALUE_T_SENSOR_CONTACT_STATUSTAMPERED_RESTORE   \
                    SENSOR_CONTACT_STORE_CHAR_VALUE_T_SENSOR_CONTACT_STATUSLOWBATTERY_RESTORE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN unsigned char hSensor_Contact_State;
EXTERN unsigned char hSensor_Contact_StatusActive;
EXTERN unsigned char hSensor_Contact_StatusFault;
EXTERN unsigned char hSensor_Contact_StatusTampered;
EXTERN unsigned char hSensor_Contact_StatusLowBattery;

EXTERN unsigned short ccc_hSensorContact_State;
EXTERN unsigned short ccc_hSensorContact_StatusActive;
EXTERN unsigned short ccc_hSensorContact_StatusFault;
EXTERN unsigned short ccc_hSensorContact_StatusTampered;
EXTERN unsigned short ccc_hSensorContact_StatusLowBattery;

#if SENSORCONTACT_STATE_BRC_ENABLE
    #define BC_PARA_SENSORCONTACT_STATE ((u8*)&bc_para_hSensorContact_State)
    EXTERN unsigned char bc_para_hSensorContact_State;
#else
    #define BC_PARA_SENSORCONTACT_STATE 0
#endif

#if SENSORCONTACT_STATUSACTIVE_BRC_ENABLE
    #define BC_PARA_SENSORCONTACT_STATUSACTIVE ((u8*)&bc_para_hSensorContact_StatusActive)
    EXTERN unsigned char bc_para_hSensorContact_StatusActive;
#else
    #define BC_PARA_SENSORCONTACT_STATUSACTIVE 0
#endif

#if SENSORCONTACT_STATUSFAULT_BRC_ENABLE
    #define BC_PARA_SENSORCONTACT_STATUSFAULT ((u8*)&bc_para_hSensorContact_StatusFault)
    EXTERN unsigned char bc_para_hSensorContact_StatusFault;
#else
    #define BC_PARA_SENSORCONTACT_STATUSFAULT 0
#endif

#if SENSORCONTACT_STATUSTAMPERED_BRC_ENABLE
    #define BC_PARA_SENSORCONTACT_STATUSTAMPERED ((u8*)&bc_para_hSensorContact_StatusTampered)
    EXTERN unsigned char bc_para_hSensorContact_StatusTampered;
#else
    #define BC_PARA_SENSORCONTACT_STATUSTAMPERED 0
#endif

#if SENSORCONTACT_STATUSLOWBATTERY_BRC_ENABLE
    #define BC_PARA_SENSORCONTACT_STATUSLOWBATTERY ((u8*)&bc_para_hSensorContact_StatusLowBattery)
    EXTERN unsigned char bc_para_hSensorContact_StatusLowBattery;
#else
    #define BC_PARA_SENSORCONTACT_STATUSLOWBATTERY 0
#endif

#if SENSOR_CONTACT_LINKED_SERVICES_ENABLE
	#define SENSOR_CONTACT_LINKED_SVCS			hSensorContact_linked_svcs
	#define SENSOR_CONTACT_LINKED_SVCS_LENGTH 	(sizeof(hSensorContact_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * hSensorContact_linked_svcs[];
#else
	#define SENSOR_CONTACT_LINKED_SVCS			0
	#define SENSOR_CONTACT_LINKED_SVCS_LENGTH 	0
#endif

EXTERN const u8 hSensor_ContactServiceName[21];

EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Contact_State_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Contact_StatusActive_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Contact_StatusFault_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Contact_StatusTampered_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hSensor_Contact_StatusLowBattery_R_S;


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN int hSensor_Contact_StateReadCallback(void *pp);
EXTERN int hSensor_Contact_StatusActiveReadCallback(void *pp);
EXTERN int hSensor_Contact_StatusFaultReadCallback(void *pp);
EXTERN int hSensor_Contact_StatusTamperedReadCallback(void *pp);
EXTERN int hSensor_Contact_StatusLowBatteryReadCallback(void *pp);
EXTERN int ccc_hSensorContact_StateWriteCB(void *pp);
EXTERN int ccc_hSensorContact_StatusActiveWriteCB(void *pp);
EXTERN int ccc_hSensorContact_StatusFaultWriteCB(void *pp);
EXTERN int ccc_hSensorContact_StatusTamperedWriteCB(void *pp);
EXTERN int ccc_hSensorContact_StatusLowBatteryWriteCB(void *pp);


#endif /* _HK_SENSOR_CONTACT_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
