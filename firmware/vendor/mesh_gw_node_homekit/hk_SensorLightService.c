/********************************************************************************************************
 * @file     hk_SensorLightService.c
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
#define _HK_SENSOR_LIGHT_SERVICE_C_


/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "hk_SensorLightService.h"
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
unsigned int  hSensor_Light_CurrentAmbientLightLevel = 0x38D1B717;  // 0.0001
unsigned char hSensor_Light_StatusActive             = 0;
unsigned char hSensor_Light_StatusFault              = 0;
unsigned char hSensor_Light_StatusTampered           = 0;
unsigned char hSensor_Light_StatusLowBattery         = 0;
unsigned char VAR_SENSOR_LIGHT_UNKOWN=0;

unsigned short ccc_hSensorLight_CurrentAmbientLightLevel = 0;
unsigned short ccc_hSensorLight_StatusActive = 0;
unsigned short ccc_hSensorLight_StatusFault = 0;
unsigned short ccc_hSensorLight_StatusTampered = 0;
unsigned short ccc_hSensorLight_StatusLowBattery = 0;

#if SENSORLIGHT_CURRENTAMBIENTLIGHTLEVEL_BRC_ENABLE
    unsigned char bc_para_hSensorLight_CurrentAmbientLightLevel = 1;
#endif

#if SENSORLIGHT_STATUSACTIVE_BRC_ENABLE
    unsigned char bc_para_hSensorLight_StatusActive = 1;
#endif

#if SENSORLIGHT_STATUSFAULT_BRC_ENABLE
    unsigned char bc_para_hSensorLight_StatusFault = 1;
#endif

#if SENSORLIGHT_STATUSTAMPERED_BRC_ENABLE
    unsigned char bc_para_hSensorLight_StatusTampered = 1;
#endif

#if SENSORLIGHT_STATUSLOWBATTERY_BRC_ENABLE
    unsigned char bc_para_hSensorLight_StatusLowBattery = 1;
#endif

const u8 hSensor_LightServiceName[19] = "Sensor_LightService";

const u8 VALID_RANGE_hSensor_Light_CurrentAmbientLightLevel[8] = { 0x17, 0xB7, 0xD1, 0x38, 0x00, 0x50, 0xC3, 0x47};	// 0.0001~100000 (float)
const u8 VALID_RANGE_hSensor_Light_StatusFault[2]              = { 0x00, 0x01};
const u8 VALID_RANGE_hSensor_Light_StatusTampered[2]           = { 0x00, 0x01};
const u8 VALID_RANGE_hSensor_Light_StatusLowBattery[2]         = { 0x00, 0x01};

const u8 VALID_STEP_hSensor_Light_StatusFault                 = 0x01;
const u8 VALID_STEP_hSensor_Light_StatusTampered              = 0x01;
const u8 VALID_STEP_hSensor_Light_StatusLowBattery            = 0x01;

const hap_characteristic_desc_t hap_desc_char_hSensor_Light_CurrentAmbientLightLevel_R_S = {hap_PRead_NOTIFY, 0, CFG_FLOAT32_LUX, 0, 8, 0, 0, 0, VALID_RANGE_hSensor_Light_CurrentAmbientLightLevel, 0, BC_PARA_SENSORLIGHT_CURRENTAMBIENTLIGHTLEVEL};
const hap_characteristic_desc_t hap_desc_char_hSensor_Light_StatusActive_R_S = {hap_PRead_NOTIFY, 0, CFG_BOOL_UNITLESS, 0, 0, 0, 0, 0, 0, 0, BC_PARA_SENSORLIGHT_STATUSACTIVE};
const hap_characteristic_desc_t hap_desc_char_hSensor_Light_StatusFault_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSensor_Light_StatusFault, &VALID_STEP_hSensor_Light_StatusFault, BC_PARA_SENSORLIGHT_STATUSFAULT};
const hap_characteristic_desc_t hap_desc_char_hSensor_Light_StatusTampered_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSensor_Light_StatusTampered, &VALID_STEP_hSensor_Light_StatusTampered, BC_PARA_SENSORLIGHT_STATUSTAMPERED};
const hap_characteristic_desc_t hap_desc_char_hSensor_Light_StatusLowBattery_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSensor_Light_StatusLowBattery, &VALID_STEP_hSensor_Light_StatusLowBattery, BC_PARA_SENSORLIGHT_STATUSLOWBATTERY};

const hap_characteristic_desc_t hap_desc_char_sensor_light_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if SENSOR_LIGHT_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * hSensorLight_linked_svcs[] = {};
#endif

#if(SENSOR_LIGHT_IS_HIDDEN_SERVICE == 1)
const hap_service_desc_t hap_desc_service_sensor_light_A5_R_S = {SENSOR_LIGHT_INST_NO+1, (0x02 + SENSOR_LIGHT_AS_PRIMARY_SERVICE), SENSOR_LIGHT_LINKED_SVCS_LENGTH, SENSOR_LIGHT_LINKED_SVCS};
#else
const hap_service_desc_t hap_desc_service_sensor_light_A5_R_S = {SENSOR_LIGHT_INST_NO+1, (0x00 + SENSOR_LIGHT_AS_PRIMARY_SERVICE), SENSOR_LIGHT_LINKED_SVCS_LENGTH, SENSOR_LIGHT_LINKED_SVCS};
#endif



/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
int hSensor_Light_CurrentAmbientLightLevelReadCallback(void *pp)
{
    if ((hSensor_Light_CurrentAmbientLightLevel <= 0x47C35000) && (hSensor_Light_CurrentAmbientLightLevel >= 0x38D1B717)) {  // 0.0001~100000
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_LIGHT_SERVICE_ENABLE
        SENSOR_LIGHT_STORE_CHAR_VALUE_T_SENSOR_LIGHT_CURRENTAMBIENTLIGHTLEVEL_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Light_StatusActiveReadCallback(void *pp)
{
    if ((hSensor_Light_StatusActive <= 1) && (hSensor_Light_StatusActive >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_LIGHT_SERVICE_ENABLE
        SENSOR_LIGHT_STORE_CHAR_VALUE_T_SENSOR_LIGHT_STATUSACTIVE_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Light_StatusFaultReadCallback(void *pp)
{
    if ((hSensor_Light_StatusFault <= 1) && (hSensor_Light_StatusFault >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_LIGHT_SERVICE_ENABLE
        SENSOR_LIGHT_STORE_CHAR_VALUE_T_SENSOR_LIGHT_STATUSFAULT_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Light_StatusTamperedReadCallback(void *pp)
{
    if ((hSensor_Light_StatusTampered <= 1) && (hSensor_Light_StatusTampered >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_LIGHT_SERVICE_ENABLE
        SENSOR_LIGHT_STORE_CHAR_VALUE_T_SENSOR_LIGHT_STATUSTAMPERED_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Light_StatusLowBatteryReadCallback(void *pp)
{
    if ((hSensor_Light_StatusLowBattery <= 1) && (hSensor_Light_StatusLowBattery >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_LIGHT_SERVICE_ENABLE
        SENSOR_LIGHT_STORE_CHAR_VALUE_T_SENSOR_LIGHT_STATUSLOWBATTERY_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorLight_CurrentAmbientLightLevelWriteCB(void *pp)
{
    static u16 ccc_hSensorLight_CurrentAmbientLightLevel_pre = 0;

    if(ccc_hSensorLight_CurrentAmbientLightLevel_pre != ccc_hSensorLight_CurrentAmbientLightLevel)
    {
        if(ccc_hSensorLight_CurrentAmbientLightLevel == 0)
        {
            ccc_hSensorLight_CurrentAmbientLightLevel_pre = ccc_hSensorLight_CurrentAmbientLightLevel;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORLIGHT_HANDLE_NUM_CURRENTAMBIENTLIGHTLEVEL);
        }
        else
        {
            ccc_hSensorLight_CurrentAmbientLightLevel_pre = ccc_hSensorLight_CurrentAmbientLightLevel;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORLIGHT_HANDLE_NUM_CURRENTAMBIENTLIGHTLEVEL);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorLight_StatusActiveWriteCB(void *pp)
{
    static u16 ccc_hSensorLight_StatusActive_pre = 0;

    if(ccc_hSensorLight_StatusActive_pre != ccc_hSensorLight_StatusActive)
    {
        if(ccc_hSensorLight_StatusActive == 0)
        {
            ccc_hSensorLight_StatusActive_pre = ccc_hSensorLight_StatusActive;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORLIGHT_HANDLE_NUM_STATUSACTIVE);
        }
        else
        {
            ccc_hSensorLight_StatusActive_pre = ccc_hSensorLight_StatusActive;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORLIGHT_HANDLE_NUM_STATUSACTIVE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorLight_StatusFaultWriteCB(void *pp)
{
    static u16 ccc_hSensorLight_StatusFault_pre = 0;

    if(ccc_hSensorLight_StatusFault_pre != ccc_hSensorLight_StatusFault)
    {
        if(ccc_hSensorLight_StatusFault == 0)
        {
            ccc_hSensorLight_StatusFault_pre = ccc_hSensorLight_StatusFault;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORLIGHT_HANDLE_NUM_STATUSFAULT);
        }
        else
        {
            ccc_hSensorLight_StatusFault_pre = ccc_hSensorLight_StatusFault;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORLIGHT_HANDLE_NUM_STATUSFAULT);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorLight_StatusTamperedWriteCB(void *pp)
{
    static u16 ccc_hSensorLight_StatusTampered_pre = 0;

    if(ccc_hSensorLight_StatusTampered_pre != ccc_hSensorLight_StatusTampered)
    {
        if(ccc_hSensorLight_StatusTampered == 0)
        {
            ccc_hSensorLight_StatusTampered_pre = ccc_hSensorLight_StatusTampered;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORLIGHT_HANDLE_NUM_STATUSTAMPERED);
        }
        else
        {
            ccc_hSensorLight_StatusTampered_pre = ccc_hSensorLight_StatusTampered;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORLIGHT_HANDLE_NUM_STATUSTAMPERED);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorLight_StatusLowBatteryWriteCB(void *pp)
{
    static u16 ccc_hSensorLight_StatusLowBattery_pre = 0;

    if(ccc_hSensorLight_StatusLowBattery_pre != ccc_hSensorLight_StatusLowBattery)
    {
        if(ccc_hSensorLight_StatusLowBattery == 0)
        {
            ccc_hSensorLight_StatusLowBattery_pre = ccc_hSensorLight_StatusLowBattery;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORLIGHT_HANDLE_NUM_STATUSLOWBATTERY);
        }
        else
        {
            ccc_hSensorLight_StatusLowBattery_pre = ccc_hSensorLight_StatusLowBattery;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORLIGHT_HANDLE_NUM_STATUSLOWBATTERY);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}


/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
