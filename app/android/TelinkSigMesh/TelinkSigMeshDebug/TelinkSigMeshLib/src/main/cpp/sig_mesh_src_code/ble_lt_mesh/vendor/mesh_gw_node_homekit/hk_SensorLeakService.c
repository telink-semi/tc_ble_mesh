/********************************************************************************************************
 * @file     hk_SensorLeakService.c
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
#define _HK_SENSOR_LEAK_SERVICE_C_


/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "hk_SensorLeakService.h"
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
unsigned char hSensor_Leak_Detected         = 0;  // 1
unsigned char hSensor_Leak_StatusActive     = 0;  // 3
unsigned char hSensor_Leak_StatusFault      = 0;  // 4
unsigned char hSensor_Leak_StatusTampered   = 0;  // 5
unsigned char hSensor_Leak_StatusLowBattery = 0;  // 6
unsigned char VAR_SENSOR_LEAK_UNKOWN=0;


unsigned short ccc_hSensor_Leak_Detected = 0;
unsigned short ccc_hSensor_Leak_StatusActive = 0;
unsigned short ccc_hSensor_Leak_StatusFault = 0;
unsigned short ccc_hSensor_Leak_StatusTampered = 0;
unsigned short ccc_hSensor_Leak_StatusLowBattery = 0;

#if SENSORLEAK_DETECTED_BRC_ENABLE
    unsigned char bc_para_hSensor_Leak_Detected = 1;
#endif

#if SENSORLEAK_STATUSACTIVE_BRC_ENABLE
    unsigned char bc_para_hSensor_Leak_StatusActive = 1;
#endif

#if SENSORLEAK_STATUSFAULT_BRC_ENABLE
    unsigned char bc_para_hSensor_Leak_StatusFault = 1;
#endif

#if SENSORLEAK_STATUSTAMPERED_BRC_ENABLE
    unsigned char bc_para_hSensor_Leak_StatusTampered = 1;
#endif

#if SENSORLEAK_STATUSLOWBATTERY_BRC_ENABLE
    unsigned char bc_para_hSensor_Leak_StatusLowBattery = 1;
#endif

const u8 hSensor_LeakServiceName[18] = "Sensor_LeakService";

const u8 VALID_RANGE_hSensor_Leak_Detected[2]         = { 0x00, 0x01};
const u8 VALID_RANGE_hSensor_Leak_StatusFault[2]      = { 0x00, 0x01};
const u8 VALID_RANGE_hSensor_Leak_StatusTampered[2]   = { 0x00, 0x01};
const u8 VALID_RANGE_hSensor_Leak_StatusLowBattery[2] = { 0x00, 0x01};

const u8 VALID_STEP_hSensor_Leak_Detected         = 0x01;
const u8 VALID_STEP_hSensor_Leak_StatusFault      = 0x01;
const u8 VALID_STEP_hSensor_Leak_StatusTampered   = 0x01;
const u8 VALID_STEP_hSensor_Leak_StatusLowBattery = 0x01;

const hap_characteristic_desc_t hap_desc_char_hSensor_Leak_Detected_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSensor_Leak_Detected, &VALID_STEP_hSensor_Leak_Detected, BC_PARA_SENSORLEAK_DETECTED};
const hap_characteristic_desc_t hap_desc_char_hSensor_Leak_StatusActive_R_S = {hap_PRead_NOTIFY, 0, CFG_BOOL_UNITLESS, 0, 0, 0, 0, 0, 0, 0, BC_PARA_SENSORLEAK_STATUSACTIVE};
const hap_characteristic_desc_t hap_desc_char_hSensor_Leak_StatusFault_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSensor_Leak_StatusFault, &VALID_STEP_hSensor_Leak_StatusFault, BC_PARA_SENSORLEAK_STATUSFAULT};
const hap_characteristic_desc_t hap_desc_char_hSensor_Leak_StatusTampered_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSensor_Leak_StatusTampered, &VALID_STEP_hSensor_Leak_StatusTampered, BC_PARA_SENSORLEAK_STATUSTAMPERED};
const hap_characteristic_desc_t hap_desc_char_hSensor_Leak_StatusLowBattery_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSensor_Leak_StatusLowBattery, &VALID_STEP_hSensor_Leak_StatusLowBattery, BC_PARA_SENSORLEAK_STATUSLOWBATTERY};

const hap_characteristic_desc_t hap_desc_char_sensor_leak_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if SENSOR_LEAK_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * hSensorLeak_linked_svcs[] = {};
#endif

#if(SENSOR_LEAK_IS_HIDDEN_SERVICE == 1)
const hap_service_desc_t hap_desc_service_sensor_leak_A5_R_S = {SENSOR_LEAK_INST_NO+1, (0x02 + SENSOR_LEAK_AS_PRIMARY_SERVICE), SENSOR_LEAK_LINKED_SVCS_LENGTH, SENSOR_LEAK_LINKED_SVCS};
#else
const hap_service_desc_t hap_desc_service_sensor_leak_A5_R_S = {SENSOR_LEAK_INST_NO+1, (0x00 + SENSOR_LEAK_AS_PRIMARY_SERVICE), SENSOR_LEAK_LINKED_SVCS_LENGTH, SENSOR_LEAK_LINKED_SVCS};
#endif


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
int hSensor_Leak_DetectedReadCallback(void *pp)
{
    if ((hSensor_Leak_Detected <= 1) && (hSensor_Leak_Detected >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_LEAK_SERVICE_ENABLE
        SENSOR_LEAK_STORE_CHAR_VALUE_T_SENSOR_LEAK_DETECTED_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Leak_StatusActiveReadCallback(void *pp)
{
    if ((hSensor_Leak_StatusActive <= 1) && (hSensor_Leak_StatusActive >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_LEAK_SERVICE_ENABLE
        SENSOR_LEAK_STORE_CHAR_VALUE_T_SENSOR_LEAK_STATUSACTIVE_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Leak_StatusFaultReadCallback(void *pp)
{
    if ((hSensor_Leak_StatusFault <= 1) && (hSensor_Leak_StatusFault >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_LEAK_SERVICE_ENABLE
        SENSOR_LEAK_STORE_CHAR_VALUE_T_SENSOR_LEAK_STATUSFAULT_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Leak_StatusTamperedReadCallback(void *pp)
{
    if ((hSensor_Leak_StatusTampered <= 1) && (hSensor_Leak_StatusTampered >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_LEAK_SERVICE_ENABLE
        SENSOR_LEAK_STORE_CHAR_VALUE_T_SENSOR_LEAK_STATUSTAMPERED_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Leak_StatusLowBatteryReadCallback(void *pp)
{
    if ((hSensor_Leak_StatusLowBattery <= 1) && (hSensor_Leak_StatusLowBattery >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_LEAK_SERVICE_ENABLE
        SENSOR_LEAK_STORE_CHAR_VALUE_T_SENSOR_LEAK_STATUSLOWBATTERY_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensor_Leak_DetectedWriteCB(void *pp)
{
    static u16 ccc_hSensor_Leak_Detected_pre = 0;

    if(ccc_hSensor_Leak_Detected_pre != ccc_hSensor_Leak_Detected)
    {
        if(ccc_hSensor_Leak_Detected == 0)
        {
            ccc_hSensor_Leak_Detected_pre = ccc_hSensor_Leak_Detected;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORLEAK_HANDLE_NUM_DETECTED);
        }
        else
        {
            ccc_hSensor_Leak_Detected_pre = ccc_hSensor_Leak_Detected;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORLEAK_HANDLE_NUM_DETECTED);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensor_Leak_StatusActiveWriteCB(void *pp)
{
    static u16 ccc_hSensor_Leak_StatusActive_pre = 0;

    if(ccc_hSensor_Leak_StatusActive_pre != ccc_hSensor_Leak_StatusActive)
    {
        if(ccc_hSensor_Leak_StatusActive == 0)
        {
            ccc_hSensor_Leak_StatusActive_pre = ccc_hSensor_Leak_StatusActive;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORLEAK_HANDLE_NUM_STATUSACTIVE);
        }
        else
        {
            ccc_hSensor_Leak_StatusActive_pre = ccc_hSensor_Leak_StatusActive;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORLEAK_HANDLE_NUM_STATUSACTIVE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensor_Leak_StatusFaultWriteCB(void *pp)
{
    static u16 ccc_hSensor_Leak_StatusFault_pre = 0;

    if(ccc_hSensor_Leak_StatusFault_pre != ccc_hSensor_Leak_StatusFault)
    {
        if(ccc_hSensor_Leak_StatusFault == 0)
        {
            ccc_hSensor_Leak_StatusFault_pre = ccc_hSensor_Leak_StatusFault;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORLEAK_HANDLE_NUM_STATUSFAULT);
        }
        else
        {
            ccc_hSensor_Leak_StatusFault_pre = ccc_hSensor_Leak_StatusFault;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORLEAK_HANDLE_NUM_STATUSFAULT);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensor_Leak_StatusTamperedWriteCB(void *pp)
{
    static u16 ccc_hSensor_Leak_StatusTampered_pre = 0;

    if(ccc_hSensor_Leak_StatusTampered_pre != ccc_hSensor_Leak_StatusTampered)
    {
        if(ccc_hSensor_Leak_StatusTampered == 0)
        {
            ccc_hSensor_Leak_StatusTampered_pre = ccc_hSensor_Leak_StatusTampered;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORLEAK_HANDLE_NUM_STATUSTAMPERED);
        }
        else
        {
            ccc_hSensor_Leak_StatusTampered_pre = ccc_hSensor_Leak_StatusTampered;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORLEAK_HANDLE_NUM_STATUSTAMPERED);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensor_Leak_StatusLowBatteryWriteCB(void *pp)
{
    static u16 ccc_hSensor_Leak_StatusLowBattery_pre = 0;

    if(ccc_hSensor_Leak_StatusLowBattery_pre != ccc_hSensor_Leak_StatusLowBattery)
    {
        if(ccc_hSensor_Leak_StatusLowBattery == 0)
        {
            ccc_hSensor_Leak_StatusLowBattery_pre = ccc_hSensor_Leak_StatusLowBattery;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORLEAK_HANDLE_NUM_STATUSLOWBATTERY);
        }
        else
        {
            ccc_hSensor_Leak_StatusLowBattery_pre = ccc_hSensor_Leak_StatusLowBattery;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORLEAK_HANDLE_NUM_STATUSLOWBATTERY);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
