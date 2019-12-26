/********************************************************************************************************
 * @file     hk_SensorCarbonMonoxideService.c
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
#define _HK_SENSOR_CARBON_MONOXIDE_SERVICE_C_


/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "hk_SensorCarbonMonoxideService.h"
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
unsigned char hSensor_Carbon_Monoxide_Detected         = 0;  // 1
unsigned char hSensor_Carbon_Monoxide_StatusActive     = 0;  // 3
unsigned char hSensor_Carbon_Monoxide_StatusFault      = 0;  // 4
unsigned char hSensor_Carbon_Monoxide_StatusTampered   = 0;  // 5
unsigned char hSensor_Carbon_Monoxide_StatusLowBattery = 0;  // 6
unsigned int  hSensor_Carbon_Monoxide_Level            = 0;  // 7
unsigned int  hSensor_Carbon_Monoxide_PeakLevel        = 0;  // 8
unsigned char VAR_SENSOR_CARBON_MONOXIDE_UNKOWN=0;

unsigned short ccc_hSensorCarbonMonoxide_Detected = 0;
unsigned short ccc_hSensorCarbonMonoxide_StatusActive = 0;
unsigned short ccc_hSensorCarbonMonoxide_StatusFault = 0;
unsigned short ccc_hSensorCarbonMonoxide_StatusTampered = 0;
unsigned short ccc_hSensorCarbonMonoxide_StatusLowBattery = 0;
unsigned short ccc_hSensorCarbonMonoxide_Level = 0;
unsigned short ccc_hSensorCarbonMonoxide_PeakLevel = 0;

#if SENSORCARBONMONOXIDE_DETECTED_BRC_ENABLE
    unsigned char bc_para_hSensorCarbonMonoxide_Detected = 1;
#endif

#if SENSORCARBONMONOXIDE_STATUSACTIVE_BRC_ENABLE
    unsigned char bc_para_hSensorCarbonMonoxide_StatusActive = 1;
#endif

#if SENSORCARBONMONOXIDE_STATUSFAULT_BRC_ENABLE
    unsigned char bc_para_hSensorCarbonMonoxide_StatusFault = 1;
#endif

#if SENSORCARBONMONOXIDE_STATUSTAMPERED_BRC_ENABLE
    unsigned char bc_para_hSensorCarbonMonoxide_StatusTampered = 1;
#endif

#if SENSORCARBONMONOXIDE_STATUSLOWBATTERY_BRC_ENABLE
    unsigned char bc_para_hSensorCarbonMonoxide_StatusLowBattery = 1;
#endif

#if SENSORCARBONMONOXIDE_LEVEL_BRC_ENABLE
    unsigned char bc_para_hSensorCarbonMonoxide_Level = 1;
#endif

#if SENSORCARBONMONOXIDE_PEAKLEVEL_BRC_ENABLE
    unsigned char bc_para_hSensorCarbonMonoxide_PeakLevel = 1;
#endif

const u8 hSensor_Carbon_MonoxideServiceName[29] = "Sensor_Carbon_MonoxideService";

const u8 VALID_RANGE_hSensor_Carbon_Monoxide_Detected[2]         = { 0x00, 0x01};
const u8 VALID_RANGE_hSensor_Carbon_Monoxide_StatusFault[2]      = { 0x00, 0x01};
const u8 VALID_RANGE_hSensor_Carbon_Monoxide_StatusTampered[2]   = { 0x00, 0x01};
const u8 VALID_RANGE_hSensor_Carbon_Monoxide_StatusLowBattery[2] = { 0x00, 0x01};
const u8 VALID_RANGE_hSensor_Carbon_Monoxide_Level[8]            = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC8, 0x42};  // 0~100 (float)
const u8 VALID_RANGE_hSensor_Carbon_Monoxide_PeakLevel[8]        = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC8, 0x42};  // 0~100 (float)

const u8 VALID_STEP_hSensor_Carbon_Monoxide_Detected         = 0x01;
const u8 VALID_STEP_hSensor_Carbon_Monoxide_StatusFault      = 0x01;
const u8 VALID_STEP_hSensor_Carbon_Monoxide_StatusTampered   = 0x01;
const u8 VALID_STEP_hSensor_Carbon_Monoxide_StatusLowBattery = 0x01;

const hap_characteristic_desc_t hap_desc_char_hSensor_Carbon_Monoxide_Detected_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSensor_Carbon_Monoxide_Detected, &VALID_STEP_hSensor_Carbon_Monoxide_Detected, BC_PARA_SENSORCARBONMONOXIDE_DETECTED};
const hap_characteristic_desc_t hap_desc_char_hSensor_Carbon_Monoxide_StatusActive_R_S = {hap_PRead_NOTIFY, 0, CFG_BOOL_UNITLESS, 0, 0, 0, 0, 0, 0, 0, BC_PARA_SENSORCARBONMONOXIDE_STATUSACTIVE};
const hap_characteristic_desc_t hap_desc_char_hSensor_Carbon_Monoxide_StatusFault_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSensor_Carbon_Monoxide_StatusFault, &VALID_STEP_hSensor_Carbon_Monoxide_StatusFault, BC_PARA_SENSORCARBONMONOXIDE_STATUSFAULT};
const hap_characteristic_desc_t hap_desc_char_hSensor_Carbon_Monoxide_StatusTampered_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSensor_Carbon_Monoxide_StatusTampered, &VALID_STEP_hSensor_Carbon_Monoxide_StatusTampered, BC_PARA_SENSORCARBONMONOXIDE_STATUSTAMPERED};
const hap_characteristic_desc_t hap_desc_char_hSensor_Carbon_Monoxide_StatusLowBattery_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSensor_Carbon_Monoxide_StatusLowBattery, &VALID_STEP_hSensor_Carbon_Monoxide_StatusLowBattery, BC_PARA_SENSORCARBONMONOXIDE_STATUSLOWBATTERY};
const hap_characteristic_desc_t hap_desc_char_hSensor_Carbon_Monoxide_Level_R_S = {hap_PRead_NOTIFY, 0, CFG_FLOAT32, 0, 8, 0, 0, 0, VALID_RANGE_hSensor_Carbon_Monoxide_Level, 0, BC_PARA_SENSORCARBONMONOXIDE_LEVEL};
const hap_characteristic_desc_t hap_desc_char_hSensor_Carbon_Monoxide_PeakLevel_R_S  = {hap_PRead_NOTIFY, 0, CFG_FLOAT32, 0, 8, 0, 0, 0, VALID_RANGE_hSensor_Carbon_Monoxide_PeakLevel, 0, BC_PARA_SENSORCARBONMONOXIDE_PEAKLEVEL};

const hap_characteristic_desc_t hap_desc_char_sensor_carbon_monoxide_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if SENSOR_CARBON_MONOXIDE_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * hSensorCarbonMonoxide_linked_svcs[] = {};
#endif

#if(SENSOR_CARBON_MONOXIDE_IS_HIDDEN_SERVICE == 1)
const hap_service_desc_t hap_desc_service_sensor_carbon_monoxide_A5_R_S = {SENSOR_CARBON_MONOXIDE_INST_NO+1, (0x02 + SENSOR_CARBON_MONOXIDE_AS_PRIMARY_SERVICE), SENSOR_CARBON_MONOXIDE_LINKED_SVCS_LENGTH, SENSOR_CARBON_MONOXIDE_LINKED_SVCS};
#else
const hap_service_desc_t hap_desc_service_sensor_carbon_monoxide_A5_R_S = {SENSOR_CARBON_MONOXIDE_INST_NO+1, (0x00 + SENSOR_CARBON_MONOXIDE_AS_PRIMARY_SERVICE), SENSOR_CARBON_MONOXIDE_LINKED_SVCS_LENGTH, SENSOR_CARBON_MONOXIDE_LINKED_SVCS};
#endif


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
int hSensor_Carbon_Monoxide_DetectedReadCallback(void *pp)
{
    if ((hSensor_Carbon_Monoxide_Detected <= 1) && (hSensor_Carbon_Monoxide_Detected >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_CARBON_MONOXIDE_SERVICE_ENABLE
        SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_DETECTED_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Carbon_Monoxide_StatusActiveReadCallback(void *pp)
{
    if ((hSensor_Carbon_Monoxide_StatusActive <= 1) && (hSensor_Carbon_Monoxide_StatusActive >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_CARBON_MONOXIDE_SERVICE_ENABLE
        SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_STATUSACTIVE_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Carbon_Monoxide_StatusFaultReadCallback(void *pp)
{
    if ((hSensor_Carbon_Monoxide_StatusFault <= 1) && (hSensor_Carbon_Monoxide_StatusFault >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_CARBON_MONOXIDE_SERVICE_ENABLE
        SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_STATUSFAULT_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Carbon_Monoxide_StatusTamperedReadCallback(void *pp)
{
    if ((hSensor_Carbon_Monoxide_StatusTampered <= 1) && (hSensor_Carbon_Monoxide_StatusTampered >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_CARBON_MONOXIDE_SERVICE_ENABLE
        SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_STATUSTAMPERED_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Carbon_Monoxide_StatusLowBatteryReadCallback(void *pp)
{
    if ((hSensor_Carbon_Monoxide_StatusLowBattery <= 1) && (hSensor_Carbon_Monoxide_StatusLowBattery >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_CARBON_MONOXIDE_SERVICE_ENABLE
        SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_STATUSLOWBATTERY_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Carbon_Monoxide_LevelReadCallback(void *pp)
{
    if ((hSensor_Carbon_Monoxide_Level <= 0x42C80000) && (hSensor_Carbon_Monoxide_Level >= 0)) {  // 0~100
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_CARBON_MONOXIDE_SERVICE_ENABLE
        SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_LEVEL_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Carbon_Monoxide_PeakLevelReadCallback(void *pp)
{
    if ((hSensor_Carbon_Monoxide_PeakLevel <= 0x42C80000) && (hSensor_Carbon_Monoxide_PeakLevel >= 0)) {  // 0~100
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_CARBON_MONOXIDE_SERVICE_ENABLE
        SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_MONOXIDE_PEAKLEVEL_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorCarbonMonoxide_DetectedWriteCB(void *pp)
{
    static u16 ccc_hSensorCarbonMonoxide_Detected_pre = 0;

    if(ccc_hSensorCarbonMonoxide_Detected_pre != ccc_hSensorCarbonMonoxide_Detected)
    {
        if(ccc_hSensorCarbonMonoxide_Detected == 0)
        {
            ccc_hSensorCarbonMonoxide_Detected_pre = ccc_hSensorCarbonMonoxide_Detected;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORCARBONMONOXIDE_HANDLE_NUM_DETECTED);
        }
        else
        {
            ccc_hSensorCarbonMonoxide_Detected_pre = ccc_hSensorCarbonMonoxide_Detected;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORCARBONMONOXIDE_HANDLE_NUM_DETECTED);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorCarbonMonoxide_StatusActiveWriteCB(void *pp)
{
    static u16 ccc_hSensorCarbonMonoxide_StatusActive_pre = 0;

    if(ccc_hSensorCarbonMonoxide_StatusActive_pre != ccc_hSensorCarbonMonoxide_StatusActive)
    {
        if(ccc_hSensorCarbonMonoxide_StatusActive == 0)
        {
            ccc_hSensorCarbonMonoxide_StatusActive_pre = ccc_hSensorCarbonMonoxide_StatusActive;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORCARBONMONOXIDE_HANDLE_NUM_STATUSACTIVE);
        }
        else
        {
            ccc_hSensorCarbonMonoxide_StatusActive_pre = ccc_hSensorCarbonMonoxide_StatusActive;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORCARBONMONOXIDE_HANDLE_NUM_STATUSACTIVE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorCarbonMonoxide_StatusFaultWriteCB(void *pp)
{
    static u16 ccc_hSensorCarbonMonoxide_StatusFault_pre = 0;

    if(ccc_hSensorCarbonMonoxide_StatusFault_pre != ccc_hSensorCarbonMonoxide_StatusFault)
    {
        if(ccc_hSensorCarbonMonoxide_StatusFault == 0)
        {
            ccc_hSensorCarbonMonoxide_StatusFault_pre = ccc_hSensorCarbonMonoxide_StatusFault;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORCARBONMONOXIDE_HANDLE_NUM_STATUSFAULT);
        }
        else
        {
            ccc_hSensorCarbonMonoxide_StatusFault_pre = ccc_hSensorCarbonMonoxide_StatusFault;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORCARBONMONOXIDE_HANDLE_NUM_STATUSFAULT);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorCarbonMonoxide_StatusTamperedWriteCB(void *pp)
{
    static u16 ccc_hSensorCarbonMonoxide_StatusTampered_pre = 0;

    if(ccc_hSensorCarbonMonoxide_StatusTampered_pre != ccc_hSensorCarbonMonoxide_StatusTampered)
    {
        if(ccc_hSensorCarbonMonoxide_StatusTampered == 0)
        {
            ccc_hSensorCarbonMonoxide_StatusTampered_pre = ccc_hSensorCarbonMonoxide_StatusTampered;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORCARBONMONOXIDE_HANDLE_NUM_STATUSTAMPERED);
        }
        else
        {
            ccc_hSensorCarbonMonoxide_StatusTampered_pre = ccc_hSensorCarbonMonoxide_StatusTampered;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORCARBONMONOXIDE_HANDLE_NUM_STATUSTAMPERED);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorCarbonMonoxide_StatusLowBatteryWriteCB(void *pp)
{
    static u16 ccc_hSensorCarbonMonoxide_StatusLowBattery_pre = 0;

    if(ccc_hSensorCarbonMonoxide_StatusLowBattery_pre != ccc_hSensorCarbonMonoxide_StatusLowBattery)
    {
        if(ccc_hSensorCarbonMonoxide_StatusLowBattery == 0)
        {
            ccc_hSensorCarbonMonoxide_StatusLowBattery_pre = ccc_hSensorCarbonMonoxide_StatusLowBattery;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORCARBONMONOXIDE_HANDLE_NUM_STATUSLOWBATTERY);
        }
        else
        {
            ccc_hSensorCarbonMonoxide_StatusLowBattery_pre = ccc_hSensorCarbonMonoxide_StatusLowBattery;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORCARBONMONOXIDE_HANDLE_NUM_STATUSLOWBATTERY);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorCarbonMonoxide_LevelWriteCB(void *pp)
{
    static u16 ccc_hSensorCarbonMonoxide_Level_pre = 0;

    if(ccc_hSensorCarbonMonoxide_Level_pre != ccc_hSensorCarbonMonoxide_Level)
    {
        if(ccc_hSensorCarbonMonoxide_Level == 0)
        {
            ccc_hSensorCarbonMonoxide_Level_pre = ccc_hSensorCarbonMonoxide_Level;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORCARBONMONOXIDE_HANDLE_NUM_LEVEL);
        }
        else
        {
            ccc_hSensorCarbonMonoxide_Level_pre = ccc_hSensorCarbonMonoxide_Level;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORCARBONMONOXIDE_HANDLE_NUM_LEVEL);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorCarbonMonoxide_PeakLevelWriteCB(void *pp)
{
    static u16 ccc_hSensorCarbonMonoxide_PeakLevel_pre = 0;

    if(ccc_hSensorCarbonMonoxide_PeakLevel_pre != ccc_hSensorCarbonMonoxide_PeakLevel)
    {
        if(ccc_hSensorCarbonMonoxide_PeakLevel == 0)
        {
            ccc_hSensorCarbonMonoxide_PeakLevel_pre = ccc_hSensorCarbonMonoxide_PeakLevel;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORCARBONMONOXIDE_HANDLE_NUM_PEAKLEVEL);
        }
        else
        {
            ccc_hSensorCarbonMonoxide_PeakLevel_pre = ccc_hSensorCarbonMonoxide_PeakLevel;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORCARBONMONOXIDE_HANDLE_NUM_PEAKLEVEL);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
