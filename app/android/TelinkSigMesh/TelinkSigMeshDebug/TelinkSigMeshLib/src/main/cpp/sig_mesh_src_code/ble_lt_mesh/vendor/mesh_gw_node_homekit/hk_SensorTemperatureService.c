/********************************************************************************************************
 * @file     hk_SensorTemperatureService.c
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
#define _HK_SENSOR_TEMPERATURE_SERVICE_C_


/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "hk_SensorTemperatureService.h"
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
unsigned int  hSensor_Temperature_CurrentTemperature = 0;  // 1
unsigned char hSensor_Temperature_StatusActive       = 0;  // 3
unsigned char hSensor_Temperature_StatusFault        = 0;  // 4
unsigned char hSensor_Temperature_StatusTampered     = 0;  // 5
unsigned char hSensor_Temperature_StatusLowBattery   = 0;  // 6
unsigned char VAR_SENSOR_TEMPERATURE_UNKOWN=0;

unsigned short ccc_hSensorTemperature_CurrentTemperature = 0;
unsigned short ccc_hSensorTemperature_StatusActive = 0;
unsigned short ccc_hSensorTemperature_StatusFault = 0;
unsigned short ccc_hSensorTemperature_StatusTampered = 0;
unsigned short ccc_hSensorTemperature_StatusLowBattery = 0;

#if SENSORTEMPERATURE_CURRENTTEMPERATURE_BRC_ENABLE
    unsigned char bc_para_hSensorTemperature_CurrentTemperature = 1;
#endif

#if SENSORTEMPERATURE_STATUSACTIVE_BRC_ENABLE
    unsigned char bc_para_hSensorTemperature_StatusActive = 1;
#endif

#if SENSORTEMPERATURE_STATUSFAULT_BRC_ENABLE
    unsigned char bc_para_hSensorTemperature_StatusFault = 1;
#endif

#if SENSORTEMPERATURE_STATUSTAMPERED_BRC_ENABLE
    unsigned char bc_para_hSensorTemperature_StatusTampered = 1;
#endif

#if SENSORTEMPERATURE_STATUSLOWBATTERY_BRC_ENABLE
    unsigned char bc_para_hSensorTemperature_StatusLowBattery = 1;
#endif

const u8 hSensor_TemperatureServiceName[25] = "Sensor_TemperatureService";

const u8 VALID_RANGE_hSensor_Temperature_CurrentTemperature[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC8, 0x42};    // 0~100 (float)
const u8 VALID_RANGE_hSensor_Temperature_StatusFault[2]        = { 0x00, 0x01};
const u8 VALID_RANGE_hSensor_Temperature_StatusTampered[2]     = { 0x00, 0x01};
const u8 VALID_RANGE_hSensor_Temperature_StatusLowBattery[2]   = { 0x00, 0x01};

const u8 VALID_STEP_hSensor_Temperature_CurrentTemperature[4] = { 0xCD, 0xCC, 0xCC, 0x3D};
const u8 VALID_STEP_hSensor_Temperature_StatusFault           = 0x01;
const u8 VALID_STEP_hSensor_Temperature_StatusTampered        = 0x01;
const u8 VALID_STEP_hSensor_Temperature_StatusLowBattery      = 0x01;

const hap_characteristic_desc_t hap_desc_char_hSensor_Temperature_CurrentTemperature_R_S = {hap_PRead_NOTIFY, 0, CFG_FLOAT32_CELSIUS, 0, 8, 4, 0, 0, VALID_RANGE_hSensor_Temperature_CurrentTemperature, VALID_STEP_hSensor_Temperature_CurrentTemperature, BC_PARA_SENSORTEMPERATURE_CURRENTTEMPERATURE};
const hap_characteristic_desc_t hap_desc_char_hSensor_Temperature_StatusActive_R_S = {hap_PRead_NOTIFY, 0, CFG_BOOL_UNITLESS, 0, 0, 0, 0, 0, 0, 0, BC_PARA_SENSORTEMPERATURE_STATUSACTIVE};
const hap_characteristic_desc_t hap_desc_char_hSensor_Temperature_StatusFault_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSensor_Temperature_StatusFault, &VALID_STEP_hSensor_Temperature_StatusFault, BC_PARA_SENSORTEMPERATURE_STATUSFAULT};
const hap_characteristic_desc_t hap_desc_char_hSensor_Temperature_StatusTampered_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSensor_Temperature_StatusTampered, &VALID_STEP_hSensor_Temperature_StatusTampered, BC_PARA_SENSORTEMPERATURE_STATUSTAMPERED};
const hap_characteristic_desc_t hap_desc_char_hSensor_Temperature_StatusLowBattery_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSensor_Temperature_StatusLowBattery, &VALID_STEP_hSensor_Temperature_StatusLowBattery, BC_PARA_SENSORTEMPERATURE_STATUSLOWBATTERY};

const hap_characteristic_desc_t hap_desc_char_sensor_temperature_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if SENSOR_TEMPERATURE_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * hSensorTemperature_linked_svcs[] = {};
#endif

#if(SENSOR_TEMPERATURE_IS_HIDDEN_SERVICE == 1)
const hap_service_desc_t hap_desc_service_sensor_temperature_A5_R_S = {SENSOR_TEMPERATURE_INST_NO+1, (0x02 + SENSOR_TEMPERATURE_AS_PRIMARY_SERVICE), SENSOR_TEMPERATURE_LINKED_SVCS_LENGTH, SENSOR_TEMPERATURE_LINKED_SVCS};
#else
const hap_service_desc_t hap_desc_service_sensor_temperature_A5_R_S = {SENSOR_TEMPERATURE_INST_NO+1, (0x00 + SENSOR_TEMPERATURE_AS_PRIMARY_SERVICE), SENSOR_TEMPERATURE_LINKED_SVCS_LENGTH, SENSOR_TEMPERATURE_LINKED_SVCS};
#endif

/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
int hSensor_Temperature_CurrentTemperatureReadCallback(void *pp)
{
    if ((hSensor_Temperature_CurrentTemperature <= 0x42C80000) && (hSensor_Temperature_CurrentTemperature >= 0)) {  // 0~100
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_TEMPERATURE_SERVICE_ENABLE
        SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T_SENSOR_TEMPERATURE_CURRENTTEMPERATURE_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Temperature_StatusActiveReadCallback(void *pp)
{
    if ((hSensor_Temperature_StatusActive <= 1) && (hSensor_Temperature_StatusActive >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_TEMPERATURE_SERVICE_ENABLE
        SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T_SENSOR_TEMPERATURE_STATUSACTIVE_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Temperature_StatusFaultReadCallback(void *pp)
{
    if ((hSensor_Temperature_StatusFault <= 1) && (hSensor_Temperature_StatusFault >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_TEMPERATURE_SERVICE_ENABLE
        SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T_SENSOR_TEMPERATURE_STATUSFAULT_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Temperature_StatusTamperedReadCallback(void *pp)
{
    if ((hSensor_Temperature_StatusTampered <= 1) && (hSensor_Temperature_StatusTampered >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_TEMPERATURE_SERVICE_ENABLE
        SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T_SENSOR_TEMPERATURE_STATUSTAMPERED_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Temperature_StatusLowBatteryReadCallback(void *pp)
{
    if ((hSensor_Temperature_StatusLowBattery <= 1) && (hSensor_Temperature_StatusLowBattery >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_TEMPERATURE_SERVICE_ENABLE
        SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T_SENSOR_TEMPERATURE_STATUSLOWBATTERY_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorTemperature_CurrentTemperatureWriteCB(void *pp)
{
    static u16 ccc_hSensorTemperature_CurrentTemperature_pre = 0;

    if(ccc_hSensorTemperature_CurrentTemperature_pre != ccc_hSensorTemperature_CurrentTemperature)
    {
        if(ccc_hSensorTemperature_CurrentTemperature == 0)
        {
            ccc_hSensorTemperature_CurrentTemperature_pre = ccc_hSensorTemperature_CurrentTemperature;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORTEMPERATURE_HANDLE_NUM_CURRENTTEMPERATURE);
        }
        else
        {
            ccc_hSensorTemperature_CurrentTemperature_pre = ccc_hSensorTemperature_CurrentTemperature;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORTEMPERATURE_HANDLE_NUM_CURRENTTEMPERATURE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorTemperature_StatusActiveWriteCB(void *pp)
{
    static u16 ccc_hSensorTemperature_StatusActive_pre = 0;

    if(ccc_hSensorTemperature_StatusActive_pre != ccc_hSensorTemperature_StatusActive)
    {
        if(ccc_hSensorTemperature_StatusActive == 0)
        {
            ccc_hSensorTemperature_StatusActive_pre = ccc_hSensorTemperature_StatusActive;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORTEMPERATURE_HANDLE_NUM_STATUSACTIVE);
        }
        else
        {
            ccc_hSensorTemperature_StatusActive_pre = ccc_hSensorTemperature_StatusActive;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORTEMPERATURE_HANDLE_NUM_STATUSACTIVE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorTemperature_StatusFaultWriteCB(void *pp)
{
    static u16 ccc_hSensorTemperature_StatusFault_pre = 0;

    if(ccc_hSensorTemperature_StatusFault_pre != ccc_hSensorTemperature_StatusFault)
    {
        if(ccc_hSensorTemperature_StatusFault == 0)
        {
            ccc_hSensorTemperature_StatusFault_pre = ccc_hSensorTemperature_StatusFault;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORTEMPERATURE_HANDLE_NUM_STATUSFAULT);
        }
        else
        {
            ccc_hSensorTemperature_StatusFault_pre = ccc_hSensorTemperature_StatusFault;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORTEMPERATURE_HANDLE_NUM_STATUSFAULT);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorTemperature_StatusTamperedWriteCB(void *pp)
{
    static u16 ccc_hSensorTemperature_StatusTampered_pre = 0;

    if(ccc_hSensorTemperature_StatusTampered_pre != ccc_hSensorTemperature_StatusTampered)
    {
        if(ccc_hSensorTemperature_StatusTampered == 0)
        {
            ccc_hSensorTemperature_StatusTampered_pre = ccc_hSensorTemperature_StatusTampered;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORTEMPERATURE_HANDLE_NUM_STATUSTAMPERED);
        }
        else
        {
            ccc_hSensorTemperature_StatusTampered_pre = ccc_hSensorTemperature_StatusTampered;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORTEMPERATURE_HANDLE_NUM_STATUSTAMPERED);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorTemperature_StatusLowBatteryWriteCB(void *pp)
{
    static u16 ccc_hSensorTemperature_StatusLowBattery_pre = 0;

    if(ccc_hSensorTemperature_StatusLowBattery_pre != ccc_hSensorTemperature_StatusLowBattery)
    {
        if(ccc_hSensorTemperature_StatusLowBattery == 0)
        {
            ccc_hSensorTemperature_StatusLowBattery_pre = ccc_hSensorTemperature_StatusLowBattery;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORTEMPERATURE_HANDLE_NUM_STATUSLOWBATTERY);
        }
        else
        {
            ccc_hSensorTemperature_StatusLowBattery_pre = ccc_hSensorTemperature_StatusLowBattery;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORTEMPERATURE_HANDLE_NUM_STATUSLOWBATTERY);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
