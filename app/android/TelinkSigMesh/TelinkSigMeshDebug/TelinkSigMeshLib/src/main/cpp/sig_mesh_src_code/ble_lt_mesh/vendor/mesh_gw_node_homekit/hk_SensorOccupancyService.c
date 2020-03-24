/********************************************************************************************************
 * @file     hk_SensorOccupancyService.c
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
#define _HK_SENSOR_OCCUPANCY_SERVICE_C_


/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "hk_SensorOccupancyService.h"
#include "ble_UUID.h"
#include "hk_TopUUID.h"

extern u32 session_timeout_tick;
extern u8 EventNotifyCNT;
extern store_char_change_value_t* char_save_restore;


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions
   ------------------------------------ */
unsigned char hSensor_Occupancy_Detected         = 0;  // 1
unsigned char hSensor_Occupancy_StatusActive     = 0;  // 3
unsigned char hSensor_Occupancy_StatusFault      = 0;  // 4
unsigned char hSensor_Occupancy_StatusTampered   = 0;  // 5
unsigned char hSensor_Occupancy_StatusLowBattery = 0;  // 6
unsigned char VAR_SENSOR_OCCUPANCY_UNKOWN=0;

unsigned short ccc_hSensorOccupancy_Detected = 0;
unsigned short ccc_hSensorOccupancy_StatusActive = 0;
unsigned short ccc_hSensorOccupancy_StatusFault = 0;
unsigned short ccc_hSensorOccupancy_StatusTampered = 0;
unsigned short ccc_hSensorOccupancy_StatusLowBattery = 0;

#if SENSOROCCUPANCY_DETECTED_BRC_ENABLE
    unsigned char bc_para_hSensorOccupancy_Detected = 1;
#endif

#if SENSOROCCUPANCY_STATUSACTIVE_BRC_ENABLE
    unsigned char bc_para_hSensorOccupancy_StatusActive = 1;
#endif

#if SENSOROCCUPANCY_STATUSFAULT_BRC_ENABLE
    unsigned char bc_para_hSensorOccupancy_StatusFault = 1;
#endif

#if SENSOROCCUPANCY_STATUSTAMPERED_BRC_ENABLE
    unsigned char bc_para_hSensorOccupancy_StatusTampered = 1;
#endif

#if SENSOROCCUPANCY_STATUSLOWBATTERY_BRC_ENABLE
    unsigned char bc_para_hSensorOccupancy_StatusLowBattery = 1;
#endif

const u8 hSensor_OccupancyServiceName[23] = "Sensor_OccupancyService";

const u8 VALID_RANGE_hSensor_Occupancy_Detected[2]         = { 0x00, 0x01};
const u8 VALID_RANGE_hSensor_Occupancy_StatusFault[2]      = { 0x00, 0x01};
const u8 VALID_RANGE_hSensor_Occupancy_StatusTampered[2]   = { 0x00, 0x01};
const u8 VALID_RANGE_hSensor_Occupancy_StatusLowBattery[2] = { 0x00, 0x01};

const u8 VALID_STEP_hSensor_Occupancy_Detected         = 0x01;
const u8 VALID_STEP_hSensor_Occupancy_StatusFault      = 0x01;
const u8 VALID_STEP_hSensor_Occupancy_StatusTampered   = 0x01;
const u8 VALID_STEP_hSensor_Occupancy_StatusLowBattery = 0x01;

const hap_characteristic_desc_t hap_desc_char_hSensor_Occupancy_Detected_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSensor_Occupancy_Detected, &VALID_STEP_hSensor_Occupancy_Detected, BC_PARA_SENSOROCCUPANCY_DETECTED};
const hap_characteristic_desc_t hap_desc_char_hSensor_Occupancy_StatusActive_R_S = {hap_PRead_NOTIFY, 0, CFG_BOOL_UNITLESS, 0, 0, 0, 0, 0, 0, 0, BC_PARA_SENSOROCCUPANCY_STATUSACTIVE};
const hap_characteristic_desc_t hap_desc_char_hSensor_Occupancy_StatusFault_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSensor_Occupancy_StatusFault, &VALID_STEP_hSensor_Occupancy_StatusFault, BC_PARA_SENSOROCCUPANCY_STATUSFAULT};
const hap_characteristic_desc_t hap_desc_char_hSensor_Occupancy_StatusTampered_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSensor_Occupancy_StatusTampered, &VALID_STEP_hSensor_Occupancy_StatusTampered, BC_PARA_SENSOROCCUPANCY_STATUSTAMPERED};
const hap_characteristic_desc_t hap_desc_char_hSensor_Occupancy_StatusLowBattery_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSensor_Occupancy_StatusLowBattery, &VALID_STEP_hSensor_Occupancy_StatusLowBattery, BC_PARA_SENSOROCCUPANCY_STATUSLOWBATTERY};

const hap_characteristic_desc_t hap_desc_char_sensor_occupancy_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if SENSOR_OCCUPANCY_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * hSensorOccupancy_linked_svcs[] = {};
#endif

#if(SENSOR_OCCUPANCY_IS_HIDDEN_SERVICE == 1)
const hap_service_desc_t hap_desc_service_sensor_occupancy_A5_R_S = {SENSOR_OCCUPANCY_INST_NO+1, (0x02 + SENSOR_OCCUPANCY_AS_PRIMARY_SERVICE), SENSOR_OCCUPANCY_LINKED_SVCS_LENGTH, SENSOR_OCCUPANCY_LINKED_SVCS};
#else
const hap_service_desc_t hap_desc_service_sensor_occupancy_A5_R_S = {SENSOR_OCCUPANCY_INST_NO+1, (0x00 + SENSOR_OCCUPANCY_AS_PRIMARY_SERVICE), SENSOR_OCCUPANCY_LINKED_SVCS_LENGTH, SENSOR_OCCUPANCY_LINKED_SVCS};
#endif


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
int hSensor_Occupancy_DetectedReadCallback(void *pp)
{
    if ((hSensor_Occupancy_Detected <= 1) && (hSensor_Occupancy_Detected >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_OCCUPANCY_SERVICE_ENABLE
        SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T_SENSOR_OCCUPANCY_DETECTED_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Occupancy_StatusActiveReadCallback(void *pp)
{
    if ((hSensor_Occupancy_StatusActive <= 1) && (hSensor_Occupancy_StatusActive >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_OCCUPANCY_SERVICE_ENABLE
        SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T_SENSOR_OCCUPANCY_STATUSACTIVE_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Occupancy_StatusFaultReadCallback(void *pp)
{
    if ((hSensor_Occupancy_StatusFault <= 1) && (hSensor_Occupancy_StatusFault >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_OCCUPANCY_SERVICE_ENABLE
        SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T_SENSOR_OCCUPANCY_STATUSFAULT_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Occupancy_StatusTamperedReadCallback(void *pp)
{
    if ((hSensor_Occupancy_StatusTampered <= 1) && (hSensor_Occupancy_StatusTampered >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_OCCUPANCY_SERVICE_ENABLE
        SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T_SENSOR_OCCUPANCY_STATUSTAMPERED_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Occupancy_StatusLowBatteryReadCallback(void *pp)
{
    if ((hSensor_Occupancy_StatusLowBattery <= 1) && (hSensor_Occupancy_StatusLowBattery >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_OCCUPANCY_SERVICE_ENABLE
        SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T_SENSOR_OCCUPANCY_STATUSLOWBATTERY_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorOccupancy_DetectedWriteCB(void *pp)
{
    static u16 ccc_hSensorOccupancy_Detected_pre = 0;

    if(ccc_hSensorOccupancy_Detected_pre != ccc_hSensorOccupancy_Detected)
    {
        if(ccc_hSensorOccupancy_Detected == 0)
        {
            ccc_hSensorOccupancy_Detected_pre = ccc_hSensorOccupancy_Detected;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSOROCCUPANCY_HANDLE_NUM_DETECTED);
        }
        else
        {
            ccc_hSensorOccupancy_Detected_pre = ccc_hSensorOccupancy_Detected;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSOROCCUPANCY_HANDLE_NUM_DETECTED);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorOccupancy_StatusActiveWriteCB(void *pp)
{
    static u16 ccc_hSensorOccupancy_StatusActive_pre = 0;

    if(ccc_hSensorOccupancy_StatusActive_pre != ccc_hSensorOccupancy_StatusActive)
    {
        if(ccc_hSensorOccupancy_StatusActive == 0)
        {
            ccc_hSensorOccupancy_StatusActive_pre = ccc_hSensorOccupancy_StatusActive;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSOROCCUPANCY_HANDLE_NUM_STATUSACTIVE);
        }
        else
        {
            ccc_hSensorOccupancy_StatusActive_pre = ccc_hSensorOccupancy_StatusActive;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSOROCCUPANCY_HANDLE_NUM_STATUSACTIVE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorOccupancy_StatusFaultWriteCB(void *pp)
{
    static u16 ccc_hSensorOccupancy_StatusFault_pre = 0;

    if(ccc_hSensorOccupancy_StatusFault_pre != ccc_hSensorOccupancy_StatusFault)
    {
        if(ccc_hSensorOccupancy_StatusFault == 0)
        {
            ccc_hSensorOccupancy_StatusFault_pre = ccc_hSensorOccupancy_StatusFault;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSOROCCUPANCY_HANDLE_NUM_STATUSFAULT);
        }
        else
        {
            ccc_hSensorOccupancy_StatusFault_pre = ccc_hSensorOccupancy_StatusFault;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSOROCCUPANCY_HANDLE_NUM_STATUSFAULT);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorOccupancy_StatusTamperedWriteCB(void *pp)
{
    static u16 ccc_hSensorOccupancy_StatusTampered_pre = 0;

    if(ccc_hSensorOccupancy_StatusTampered_pre != ccc_hSensorOccupancy_StatusTampered)
    {
        if(ccc_hSensorOccupancy_StatusTampered == 0)
        {
            ccc_hSensorOccupancy_StatusTampered_pre = ccc_hSensorOccupancy_StatusTampered;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSOROCCUPANCY_HANDLE_NUM_STATUSTAMPERED);
        }
        else
        {
            ccc_hSensorOccupancy_StatusTampered_pre = ccc_hSensorOccupancy_StatusTampered;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSOROCCUPANCY_HANDLE_NUM_STATUSTAMPERED);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorOccupancy_StatusLowBatteryWriteCB(void *pp)
{
    static u16 ccc_hSensorOccupancy_StatusLowBattery_pre = 0;

    if(ccc_hSensorOccupancy_StatusLowBattery_pre != ccc_hSensorOccupancy_StatusLowBattery)
    {
        if(ccc_hSensorOccupancy_StatusLowBattery == 0)
        {
            ccc_hSensorOccupancy_StatusLowBattery_pre = ccc_hSensorOccupancy_StatusLowBattery;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSOROCCUPANCY_HANDLE_NUM_STATUSLOWBATTERY);
        }
        else
        {
            ccc_hSensorOccupancy_StatusLowBattery_pre = ccc_hSensorOccupancy_StatusLowBattery;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSOROCCUPANCY_HANDLE_NUM_STATUSLOWBATTERY);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
