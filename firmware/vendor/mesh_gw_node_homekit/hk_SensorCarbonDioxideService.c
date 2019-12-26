/********************************************************************************************************
 * @file     hk_SensorCarbonDioxideService.c
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
#define _HK_SENSOR_CARBON_DIOXIDE_SERVICE_C_


/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "hk_SensorCarbonDioxideService.h"
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
unsigned char hSensor_Carbon_Dioxide_Detected         = 0;  // 1
unsigned char hSensor_Carbon_Dioxide_StatusActive     = 0;  // 3
unsigned char hSensor_Carbon_Dioxide_StatusFault      = 0;  // 4
unsigned char hSensor_Carbon_Dioxide_StatusTampered   = 0;  // 5
unsigned char hSensor_Carbon_Dioxide_StatusLowBattery = 0;  // 6
unsigned int  hSensor_Carbon_Dioxide_Level            = 0;  // 7
unsigned int  hSensor_Carbon_Dioxide_PeakLevel        = 0;  // 8
unsigned char VAR_SENSOR_CARBON_DIOXIDE_UNKOWN=0;

unsigned short ccc_hSensorCarbonDioxide_Detected = 0;
unsigned short ccc_hSensorCarbonDioxide_StatusActive = 0;
unsigned short ccc_hSensorCarbonDioxide_StatusFault = 0;
unsigned short ccc_hSensorCarbonDioxide_StatusTampered = 0;
unsigned short ccc_hSensorCarbonDioxide_StatusLowBattery = 0;
unsigned short ccc_hSensorCarbonDioxide_Level = 0;
unsigned short ccc_hSensorCarbonDioxide_PeakLevel = 0;

#if SENSORCARBONDIOXIDE_DETECTED_BRC_ENABLE
    unsigned char bc_para_hSensorCarbonDioxide_Detected = 1;
#endif

#if SENSORCARBONDIOXIDE_STATUSACTIVE_BRC_ENABLE
    unsigned char bc_para_hSensorCarbonDioxide_StatusActive = 1;
#endif

#if SENSORCARBONDIOXIDE_STATUSFAULT_BRC_ENABLE
    unsigned char bc_para_hSensorCarbonDioxide_StatusFault = 1;
#endif

#if SENSORCARBONDIOXIDE_STATUSTAMPERED_BRC_ENABLE
    unsigned char bc_para_hSensorCarbonDioxide_StatusTampered = 1;
#endif

#if SENSORCARBONDIOXIDE_STATUSLOWBATTERY_BRC_ENABLE
    unsigned char bc_para_hSensorCarbonDioxide_StatusLowBattery = 1;
#endif

#if SENSORCARBONDIOXIDE_LEVEL_BRC_ENABLE
    unsigned char bc_para_hSensorCarbonDioxide_Level = 1;
#endif

#if SENSORCARBONDIOXIDE_PEAKLEVEL_BRC_ENABLE
    unsigned char bc_para_hSensorCarbonDioxide_PeakLevel = 1;
#endif

const u8 hSensor_Carbon_DioxideServiceName[28] = "Sensor_Carbon_DioxideService";

const u8 VALID_RANGE_hSensor_Carbon_Dioxide_Detected[2]         = { 0x00, 0x01};
const u8 VALID_RANGE_hSensor_Carbon_Dioxide_StatusFault[2]      = { 0x00, 0x01};
const u8 VALID_RANGE_hSensor_Carbon_Dioxide_StatusTampered[2]   = { 0x00, 0x01};
const u8 VALID_RANGE_hSensor_Carbon_Dioxide_StatusLowBattery[2] = { 0x00, 0x01};
const u8 VALID_RANGE_hSensor_Carbon_Dioxide_Level[8]            = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0xC3, 0x47};  // 0~100000 (float)
const u8 VALID_RANGE_hSensor_Carbon_Dioxide_PeakLevel[8]        = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0xC3, 0x47};  // 0~100000 (float)

const u8 VALID_STEP_hSensor_Carbon_Dioxide_Detected         = 0x01;
const u8 VALID_STEP_hSensor_Carbon_Dioxide_StatusFault      = 0x01;
const u8 VALID_STEP_hSensor_Carbon_Dioxide_StatusTampered   = 0x01;
const u8 VALID_STEP_hSensor_Carbon_Dioxide_StatusLowBattery = 0x01;

const hap_characteristic_desc_t hap_desc_char_hSensor_Carbon_Dioxide_Detected_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSensor_Carbon_Dioxide_Detected, &VALID_STEP_hSensor_Carbon_Dioxide_Detected, BC_PARA_SENSORCARBONDIOXIDE_DETECTED};
const hap_characteristic_desc_t hap_desc_char_hSensor_Carbon_Dioxide_StatusActive_R_S = {hap_PRead_NOTIFY, 0, CFG_BOOL_UNITLESS, 0, 0, 0, 0, 0, 0, 0, BC_PARA_SENSORCARBONDIOXIDE_STATUSACTIVE};
const hap_characteristic_desc_t hap_desc_char_hSensor_Carbon_Dioxide_StatusFault_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSensor_Carbon_Dioxide_StatusFault, &VALID_STEP_hSensor_Carbon_Dioxide_StatusFault, BC_PARA_SENSORCARBONDIOXIDE_STATUSFAULT};
const hap_characteristic_desc_t hap_desc_char_hSensor_Carbon_Dioxide_StatusTampered_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSensor_Carbon_Dioxide_StatusTampered, &VALID_STEP_hSensor_Carbon_Dioxide_StatusTampered, BC_PARA_SENSORCARBONDIOXIDE_STATUSTAMPERED};
const hap_characteristic_desc_t hap_desc_char_hSensor_Carbon_Dioxide_StatusLowBattery_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSensor_Carbon_Dioxide_StatusLowBattery, &VALID_STEP_hSensor_Carbon_Dioxide_StatusLowBattery, BC_PARA_SENSORCARBONDIOXIDE_STATUSLOWBATTERY};
const hap_characteristic_desc_t hap_desc_char_hSensor_Carbon_Dioxide_Level_R_S = {hap_PRead_NOTIFY, 0, CFG_FLOAT32, 0, 8, 0, 0, 0, VALID_RANGE_hSensor_Carbon_Dioxide_Level, 0, BC_PARA_SENSORCARBONDIOXIDE_LEVEL};
const hap_characteristic_desc_t hap_desc_char_hSensor_Carbon_Dioxide_PeakLevel_R_S = {hap_PRead_NOTIFY, 0, CFG_FLOAT32, 0, 8, 0, 0, 0, VALID_RANGE_hSensor_Carbon_Dioxide_PeakLevel, 0, BC_PARA_SENSORCARBONDIOXIDE_PEAKLEVEL};

const hap_characteristic_desc_t hap_desc_char_sensor_carbon_dioxide_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if SENSOR_CARBON_DIOXIDE_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * hSensorCarbonDioxide_linked_svcs[] = {};
#endif

#if(SENSOR_CARBON_DIOXIDE_IS_HIDDEN_SERVICE == 1)
const hap_service_desc_t hap_desc_service_sensor_carbon_dioxide_A5_R_S = {SENSOR_CARBON_DIOXIDE_INST_NO+1, (0x02 + SENSOR_CARBON_DIOXIDE_AS_PRIMARY_SERVICE), SENSOR_CARBON_DIOXIDE_LINKED_SVCS_LENGTH, SENSOR_CARBON_DIOXIDE_LINKED_SVCS};
#else
const hap_service_desc_t hap_desc_service_sensor_carbon_dioxide_A5_R_S = {SENSOR_CARBON_DIOXIDE_INST_NO+1, (0x00 + SENSOR_CARBON_DIOXIDE_AS_PRIMARY_SERVICE), SENSOR_CARBON_DIOXIDE_LINKED_SVCS_LENGTH, SENSOR_CARBON_DIOXIDE_LINKED_SVCS};
#endif


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
int hSensor_Carbon_Dioxide_DetectedReadCallback(void *pp)
{
    if ((hSensor_Carbon_Dioxide_Detected <= 1) && (hSensor_Carbon_Dioxide_Detected >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_CARBON_DIOXIDE_SERVICE_ENABLE
        SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_DETECTED_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Carbon_Dioxide_StatusActiveReadCallback(void *pp)
{
    if ((hSensor_Carbon_Dioxide_StatusActive <= 1) && (hSensor_Carbon_Dioxide_StatusActive >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_CARBON_DIOXIDE_SERVICE_ENABLE
        SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_STATUSACTIVE_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Carbon_Dioxide_StatusFaultReadCallback(void *pp)
{
    if ((hSensor_Carbon_Dioxide_StatusFault <= 1) && (hSensor_Carbon_Dioxide_StatusFault >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_CARBON_DIOXIDE_SERVICE_ENABLE
        SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_STATUSFAULT_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Carbon_Dioxide_StatusTamperedReadCallback(void *pp)
{
    if ((hSensor_Carbon_Dioxide_StatusTampered <= 1) && (hSensor_Carbon_Dioxide_StatusTampered >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_CARBON_DIOXIDE_SERVICE_ENABLE
        SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_STATUSTAMPERED_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Carbon_Dioxide_StatusLowBatteryReadCallback(void *pp)
{
    if ((hSensor_Carbon_Dioxide_StatusLowBattery <= 1) && (hSensor_Carbon_Dioxide_StatusLowBattery >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_CARBON_DIOXIDE_SERVICE_ENABLE
        SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_STATUSLOWBATTERY_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Carbon_Dioxide_LevelReadCallback(void *pp)
{
    if ((hSensor_Carbon_Dioxide_Level <= 0x47C35000) && (hSensor_Carbon_Dioxide_Level >= 0)) {  // 0~100000
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_CARBON_DIOXIDE_SERVICE_ENABLE
        SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_LEVEL_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Carbon_Dioxide_PeakLevelReadCallback(void *pp)
{
    if ((hSensor_Carbon_Dioxide_PeakLevel <= 0x47C35000) && (hSensor_Carbon_Dioxide_PeakLevel >= 0)) {  // 0~100000
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_CARBON_DIOXIDE_SERVICE_ENABLE
        SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T_SENSOR_CARBON_DIOXIDE_PEAKLEVEL_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorCarbonDioxide_DetectedWriteCB(void *pp)
{
    static u16 ccc_hSensorCarbonDioxide_Detected_pre = 0;

    if(ccc_hSensorCarbonDioxide_Detected_pre != ccc_hSensorCarbonDioxide_Detected)
    {
        if(ccc_hSensorCarbonDioxide_Detected == 0)
        {
            ccc_hSensorCarbonDioxide_Detected_pre = ccc_hSensorCarbonDioxide_Detected;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORCARBONDIOXIDE_HANDLE_NUM_DETECTED);
        }
        else
        {
            ccc_hSensorCarbonDioxide_Detected_pre = ccc_hSensorCarbonDioxide_Detected;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORCARBONDIOXIDE_HANDLE_NUM_DETECTED);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorCarbonDioxide_StatusActiveWriteCB(void *pp)
{
    static u16 ccc_hSensorCarbonDioxide_StatusActive_pre = 0;

    if(ccc_hSensorCarbonDioxide_StatusActive_pre != ccc_hSensorCarbonDioxide_StatusActive)
    {
        if(ccc_hSensorCarbonDioxide_StatusActive == 0)
        {
            ccc_hSensorCarbonDioxide_StatusActive_pre = ccc_hSensorCarbonDioxide_StatusActive;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORCARBONDIOXIDE_HANDLE_NUM_STATUSACTIVE);
        }
        else
        {
            ccc_hSensorCarbonDioxide_StatusActive_pre = ccc_hSensorCarbonDioxide_StatusActive;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORCARBONDIOXIDE_HANDLE_NUM_STATUSACTIVE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorCarbonDioxide_StatusFaultWriteCB(void *pp)
{
    static u16 ccc_hSensorCarbonDioxide_StatusFault_pre = 0;

    if(ccc_hSensorCarbonDioxide_StatusFault_pre != ccc_hSensorCarbonDioxide_StatusFault)
    {
        if(ccc_hSensorCarbonDioxide_StatusFault == 0)
        {
            ccc_hSensorCarbonDioxide_StatusFault_pre = ccc_hSensorCarbonDioxide_StatusFault;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORCARBONDIOXIDE_HANDLE_NUM_STATUSFAULT);
        }
        else
        {
            ccc_hSensorCarbonDioxide_StatusFault_pre = ccc_hSensorCarbonDioxide_StatusFault;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORCARBONDIOXIDE_HANDLE_NUM_STATUSFAULT);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorCarbonDioxide_StatusTamperedWriteCB(void *pp)
{
    static u16 ccc_hSensorCarbonDioxide_StatusTampered_pre = 0;

    if(ccc_hSensorCarbonDioxide_StatusTampered_pre != ccc_hSensorCarbonDioxide_StatusTampered)
    {
        if(ccc_hSensorCarbonDioxide_StatusTampered == 0)
        {
            ccc_hSensorCarbonDioxide_StatusTampered_pre = ccc_hSensorCarbonDioxide_StatusTampered;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORCARBONDIOXIDE_HANDLE_NUM_STATUSTAMPERED);
        }
        else
        {
            ccc_hSensorCarbonDioxide_StatusTampered_pre = ccc_hSensorCarbonDioxide_StatusTampered;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORCARBONDIOXIDE_HANDLE_NUM_STATUSTAMPERED);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorCarbonDioxide_StatusLowBatteryWriteCB(void *pp)
{
    static u16 ccc_hSensorCarbonDioxide_StatusLowBattery_pre = 0;

    if(ccc_hSensorCarbonDioxide_StatusLowBattery_pre != ccc_hSensorCarbonDioxide_StatusLowBattery)
    {
        if(ccc_hSensorCarbonDioxide_StatusLowBattery == 0)
        {
            ccc_hSensorCarbonDioxide_StatusLowBattery_pre = ccc_hSensorCarbonDioxide_StatusLowBattery;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORCARBONDIOXIDE_HANDLE_NUM_STATUSLOWBATTERY);
        }
        else
        {
            ccc_hSensorCarbonDioxide_StatusLowBattery_pre = ccc_hSensorCarbonDioxide_StatusLowBattery;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORCARBONDIOXIDE_HANDLE_NUM_STATUSLOWBATTERY);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorCarbonDioxide_LevelWriteCB(void *pp)
{
    static u16 ccc_hSensorCarbonDioxide_Level_pre = 0;

    if(ccc_hSensorCarbonDioxide_Level_pre != ccc_hSensorCarbonDioxide_Level)
    {
        if(ccc_hSensorCarbonDioxide_Level == 0)
        {
            ccc_hSensorCarbonDioxide_Level_pre = ccc_hSensorCarbonDioxide_Level;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORCARBONDIOXIDE_HANDLE_NUM_LEVEL);
        }
        else
        {
            ccc_hSensorCarbonDioxide_Level_pre = ccc_hSensorCarbonDioxide_Level;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORCARBONDIOXIDE_HANDLE_NUM_LEVEL);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorCarbonDioxide_PeakLevelWriteCB(void *pp)
{
    static u16 ccc_hSensorCarbonDioxide_PeakLevel_pre = 0;

    if(ccc_hSensorCarbonDioxide_PeakLevel_pre != ccc_hSensorCarbonDioxide_PeakLevel)
    {
        if(ccc_hSensorCarbonDioxide_PeakLevel == 0)
        {
            ccc_hSensorCarbonDioxide_PeakLevel_pre = ccc_hSensorCarbonDioxide_PeakLevel;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORCARBONDIOXIDE_HANDLE_NUM_PEAKLEVEL);
        }
        else
        {
            ccc_hSensorCarbonDioxide_PeakLevel_pre = ccc_hSensorCarbonDioxide_PeakLevel;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORCARBONDIOXIDE_HANDLE_NUM_PEAKLEVEL);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
