/********************************************************************************************************
 * @file     hk_SensorHumidityService.c
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
#define _HK_SENSOR_HUMIDITY_SERVICE_C_


/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "hk_SensorHumidityService.h"
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
unsigned int  hSensor_Humidity_CurrentRelativeHumidity = 0;  // 1
unsigned char hSensor_Humidity_StatusActive            = 0;  // 3
unsigned char hSensor_Humidity_StatusFault             = 0;  // 4
unsigned char hSensor_Humidity_StatusTampered          = 0;  // 5
unsigned char hSensor_Humidity_StatusLowBattery        = 0;  // 6
unsigned char VAR_SENSOR_HUMIDITY_UNKOWN=0;

unsigned short ccc_hSensorHumidity_CurrentRelativeHumidity = 0;
unsigned short ccc_hSensorHumidity_StatusActive = 0;
unsigned short ccc_hSensorHumidity_StatusFault = 0;
unsigned short ccc_hSensorHumidity_StatusTampered = 0;
unsigned short ccc_hSensorHumidity_StatusLowBattery = 0;

#if SENSORHUMIDITY_CURRENTRELATIVEHUMIDITY_BRC_ENABLE
    unsigned char bc_para_hSensorHumidity_CurrentRelativeHumidity = 1;
#endif

#if SENSORHUMIDITY_STATUSACTIVE_BRC_ENABLE
    unsigned char bc_para_hSensorHumidity_StatusActive = 1;
#endif

#if SENSORHUMIDITY_STATUSFAULT_BRC_ENABLE
    unsigned char bc_para_hSensorHumidity_StatusFault = 1;
#endif

#if SENSORHUMIDITY_STATUSTAMPERED_BRC_ENABLE
    unsigned char bc_para_hSensorHumidity_StatusTampered = 1;
#endif

#if SENSORHUMIDITY_STATUSLOWBATTERY_BRC_ENABLE
    unsigned char bc_para_hSensorHumidity_StatusLowBattery = 1;
#endif

const u8 hSensor_HumidityServiceName[22] = "Sensor_HumidityService";

const u8 VALID_RANGE_hSensor_Humidity_CurrentRelativeHumidity[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC8, 0x42};	// 0~100 (float)
const u8 VALID_RANGE_hSensor_Humidity_StatusFault[2]             = { 0x00, 0x01};
const u8 VALID_RANGE_hSensor_Humidity_StatusTampered[2]          = { 0x00, 0x01};
const u8 VALID_RANGE_hSensor_Humidity_StatusLowBattery[2]        = { 0x00, 0x01};

const u8 VALID_STEP_hSensor_Humidity_CurrentRelativeHumidity[4] = { 0x00, 0x00, 0x80, 0x3F};
const u8 VALID_STEP_hSensor_Humidity_StatusFault                = 0x01;
const u8 VALID_STEP_hSensor_Humidity_StatusTampered             = 0x01;
const u8 VALID_STEP_hSensor_Humidity_StatusLowBattery           = 0x01;

const hap_characteristic_desc_t hap_desc_char_hSensor_Humidity_CurrentRelativeHumidity_R_S = {hap_PRead_NOTIFY, 0, CFG_FLOAT32_PERCENTAGE, 0, 8, 4, 0, 0, VALID_RANGE_hSensor_Humidity_CurrentRelativeHumidity, VALID_STEP_hSensor_Humidity_CurrentRelativeHumidity, BC_PARA_SENSORHUMIDITY_CURRENTRELATIVEHUMIDITY};
const hap_characteristic_desc_t hap_desc_char_hSensor_Humidity_StatusActive_R_S = {hap_PRead_NOTIFY, 0, CFG_BOOL_UNITLESS, 0, 0, 0, 0, 0, 0, 0, BC_PARA_SENSORHUMIDITY_STATUSACTIVE};
const hap_characteristic_desc_t hap_desc_char_hSensor_Humidity_StatusFault_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSensor_Humidity_StatusFault, &VALID_STEP_hSensor_Humidity_StatusFault, BC_PARA_SENSORHUMIDITY_STATUSFAULT};
const hap_characteristic_desc_t hap_desc_char_hSensor_Humidity_StatusTampered_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSensor_Humidity_StatusTampered, &VALID_STEP_hSensor_Humidity_StatusTampered, BC_PARA_SENSORHUMIDITY_STATUSTAMPERED};
const hap_characteristic_desc_t hap_desc_char_hSensor_Humidity_StatusLowBattery_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSensor_Humidity_StatusLowBattery, &VALID_STEP_hSensor_Humidity_StatusLowBattery, BC_PARA_SENSORHUMIDITY_STATUSLOWBATTERY};

const hap_characteristic_desc_t hap_desc_char_sensor_humidity_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if SENSOR_HUMIDITY_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * hSensorHumidity_linked_svcs[] = {};
#endif

#if(SENSOR_HUMIDITY_IS_HIDDEN_SERVICE == 1)
const hap_service_desc_t hap_desc_service_sensor_humidity_A5_R_S = {SENSOR_HUMIDITY_INST_NO+1, (0x02 + SENSOR_HUMIDITY_AS_PRIMARY_SERVICE), SENSOR_HUMIDITY_LINKED_SVCS_LENGTH, SENSOR_HUMIDITY_LINKED_SVCS};
#else
const hap_service_desc_t hap_desc_service_sensor_humidity_A5_R_S = {SENSOR_HUMIDITY_INST_NO+1, (0x00 + SENSOR_HUMIDITY_AS_PRIMARY_SERVICE), SENSOR_HUMIDITY_LINKED_SVCS_LENGTH, SENSOR_HUMIDITY_LINKED_SVCS};
#endif


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
int hSensor_Humidity_CurrentRelativeHumidityReadCallback(void *pp)
{
    if ((hSensor_Humidity_CurrentRelativeHumidity <= 0x42C80000) && (hSensor_Humidity_CurrentRelativeHumidity >= 0)) {  // 0~100
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_HUMIDITY_SERVICE_ENABLE
        SENSOR_HUMIDITY_STORE_CHAR_VALUE_T_SENSOR_HUMIDITY_CURRENTRELATIVEHUMIDITY_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Humidity_StatusActiveReadCallback(void *pp)
{
    if ((hSensor_Humidity_StatusActive <= 1) && (hSensor_Humidity_StatusActive >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_HUMIDITY_SERVICE_ENABLE
        SENSOR_HUMIDITY_STORE_CHAR_VALUE_T_SENSOR_HUMIDITY_STATUSACTIVE_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Humidity_StatusFaultReadCallback(void *pp)
{
    if ((hSensor_Humidity_StatusFault <= 1) && (hSensor_Humidity_StatusFault >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_HUMIDITY_SERVICE_ENABLE
        SENSOR_HUMIDITY_STORE_CHAR_VALUE_T_SENSOR_HUMIDITY_STATUSFAULT_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Humidity_StatusTamperedReadCallback(void *pp)
{
    if ((hSensor_Humidity_StatusTampered <= 1) && (hSensor_Humidity_StatusTampered >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_HUMIDITY_SERVICE_ENABLE
        SENSOR_HUMIDITY_STORE_CHAR_VALUE_T_SENSOR_HUMIDITY_STATUSTAMPERED_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Humidity_StatusLowBatteryReadCallback(void *pp)
{
    if ((hSensor_Humidity_StatusLowBattery <= 1) && (hSensor_Humidity_StatusLowBattery >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_HUMIDITY_SERVICE_ENABLE
        SENSOR_HUMIDITY_STORE_CHAR_VALUE_T_SENSOR_HUMIDITY_STATUSLOWBATTERY_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorHumidity_CurrentRelativeHumidityWriteCB(void *pp)
{
    static u16 ccc_hSensorHumidity_CurrentRelativeHumidity_pre = 0;

    if(ccc_hSensorHumidity_CurrentRelativeHumidity_pre != ccc_hSensorHumidity_CurrentRelativeHumidity)
    {
        if(ccc_hSensorHumidity_CurrentRelativeHumidity == 0)
        {
            ccc_hSensorHumidity_CurrentRelativeHumidity_pre = ccc_hSensorHumidity_CurrentRelativeHumidity;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORHUMIDITY_HANDLE_NUM_CURRENTRELATIVEHUMIDITY);
        }
        else
        {
            ccc_hSensorHumidity_CurrentRelativeHumidity_pre = ccc_hSensorHumidity_CurrentRelativeHumidity;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORHUMIDITY_HANDLE_NUM_CURRENTRELATIVEHUMIDITY);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorHumidity_StatusActiveWriteCB(void *pp)
{
    static u16 ccc_hSensorHumidity_StatusActive_pre = 0;

    if(ccc_hSensorHumidity_StatusActive_pre != ccc_hSensorHumidity_StatusActive)
    {
        if(ccc_hSensorHumidity_StatusActive == 0)
        {
            ccc_hSensorHumidity_StatusActive_pre = ccc_hSensorHumidity_StatusActive;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORHUMIDITY_HANDLE_NUM_STATUSACTIVE);
        }
        else
        {
            ccc_hSensorHumidity_StatusActive_pre = ccc_hSensorHumidity_StatusActive;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORHUMIDITY_HANDLE_NUM_STATUSACTIVE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorHumidity_StatusFaultWriteCB(void *pp)
{
    static u16 ccc_hSensorHumidity_StatusFault_pre = 0;

    if(ccc_hSensorHumidity_StatusFault_pre != ccc_hSensorHumidity_StatusFault)
    {
        if(ccc_hSensorHumidity_StatusFault == 0)
        {
            ccc_hSensorHumidity_StatusFault_pre = ccc_hSensorHumidity_StatusFault;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORHUMIDITY_HANDLE_NUM_STATUSFAULT);
        }
        else
        {
            ccc_hSensorHumidity_StatusFault_pre = ccc_hSensorHumidity_StatusFault;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORHUMIDITY_HANDLE_NUM_STATUSFAULT);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorHumidity_StatusTamperedWriteCB(void *pp)
{
    static u16 ccc_hSensorHumidity_StatusTampered_pre = 0;

    if(ccc_hSensorHumidity_StatusTampered_pre != ccc_hSensorHumidity_StatusTampered)
    {
        if(ccc_hSensorHumidity_StatusTampered == 0)
        {
            ccc_hSensorHumidity_StatusTampered_pre = ccc_hSensorHumidity_StatusTampered;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORHUMIDITY_HANDLE_NUM_STATUSTAMPERED);
        }
        else
        {
            ccc_hSensorHumidity_StatusTampered_pre = ccc_hSensorHumidity_StatusTampered;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORHUMIDITY_HANDLE_NUM_STATUSTAMPERED);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorHumidity_StatusLowBatteryWriteCB(void *pp)
{
    static u16 ccc_hSensorHumidity_StatusLowBattery_pre = 0;

    if(ccc_hSensorHumidity_StatusLowBattery_pre != ccc_hSensorHumidity_StatusLowBattery)
    {
        if(ccc_hSensorHumidity_StatusLowBattery == 0)
        {
            ccc_hSensorHumidity_StatusLowBattery_pre = ccc_hSensorHumidity_StatusLowBattery;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORHUMIDITY_HANDLE_NUM_STATUSLOWBATTERY);
        }
        else
        {
            ccc_hSensorHumidity_StatusLowBattery_pre = ccc_hSensorHumidity_StatusLowBattery;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORHUMIDITY_HANDLE_NUM_STATUSLOWBATTERY);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
