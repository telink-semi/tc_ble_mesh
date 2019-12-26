/********************************************************************************************************
 * @file     hk_SensorMotionService.c
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
#define _HK_SENSOR_MOTION_SERVICE_C_


/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "hk_SensorMotionService.h"
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
unsigned char hSensor_Motion_Detected         = 0;  // 1
unsigned char hSensor_Motion_StatusActive     = 0;  // 3
unsigned char hSensor_Motion_StatusFault      = 0;  // 4
unsigned char hSensor_Motion_StatusTampered   = 0;  // 5
unsigned char hSensor_Motion_StatusLowBattery = 0;  // 6
unsigned char VAR_SENSOR_MOTION_UNKOWN=0;

unsigned short ccc_hSensorMotion_Detected = 0;
unsigned short ccc_hSensorMotion_StatusActive = 0;
unsigned short ccc_hSensorMotion_StatusFault = 0;
unsigned short ccc_hSensorMotion_StatusTampered = 0;
unsigned short ccc_hSensorMotion_StatusLowBattery = 0;

#if SENSORMOTION_DETECTED_BRC_ENABLE
    unsigned char bc_para_hSensorMotion_Detected = 1;
#endif

#if SENSORMOTION_STATUSACTIVE_BRC_ENABLE
    unsigned char bc_para_hSensorMotion_StatusActive = 1;
#endif

#if SENSORMOTION_STATUSFAULT_BRC_ENABLE
    unsigned char bc_para_hSensorMotion_StatusFault = 1;
#endif

#if SENSORMOTION_STATUSTAMPERED_BRC_ENABLE
    unsigned char bc_para_hSensorMotion_StatusTampered = 1;
#endif

#if SENSORMOTION_STATUSLOWBATTERY_BRC_ENABLE
    unsigned char bc_para_hSensorMotion_StatusLowBattery = 1;
#endif

const u8 hSensor_MotionServiceName[20] = "Sensor_MotionService";

const u8 VALID_RANGE_hSensor_Motion_StatusFault[2]      = { 0x00, 0x01};
const u8 VALID_RANGE_hSensor_Motion_StatusTampered[2]   = { 0x00, 0x01};
const u8 VALID_RANGE_hSensor_Motion_StatusLowBattery[2] = { 0x00, 0x01};

const u8 VALID_STEP_hSensor_Motion_StatusFault      = 0x01;
const u8 VALID_STEP_hSensor_Motion_StatusTampered   = 0x01;
const u8 VALID_STEP_hSensor_Motion_StatusLowBattery = 0x01;

const hap_characteristic_desc_t hap_desc_char_hSensor_Motion_Detected_R_S = {hap_PRead_NOTIFY, 0, CFG_BOOL_UNITLESS, 0, 0, 0, 0, 0, 0, 0, BC_PARA_SENSORMOTION_DETECTED};
const hap_characteristic_desc_t hap_desc_char_hSensor_Motion_StatusActive_R_S = {hap_PRead_NOTIFY, 0, CFG_BOOL_UNITLESS, 0, 0, 0, 0, 0, 0, 0, BC_PARA_SENSORMOTION_STATUSACTIVE};
const hap_characteristic_desc_t hap_desc_char_hSensor_Motion_StatusFault_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSensor_Motion_StatusFault, &VALID_STEP_hSensor_Motion_StatusFault, BC_PARA_SENSORMOTION_STATUSFAULT};
const hap_characteristic_desc_t hap_desc_char_hSensor_Motion_StatusTampered_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSensor_Motion_StatusTampered, &VALID_STEP_hSensor_Motion_StatusTampered, BC_PARA_SENSORMOTION_STATUSTAMPERED};
const hap_characteristic_desc_t hap_desc_char_hSensor_Motion_StatusLowBattery_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSensor_Motion_StatusLowBattery, &VALID_STEP_hSensor_Motion_StatusLowBattery, BC_PARA_SENSORMOTION_STATUSLOWBATTERY};

const hap_characteristic_desc_t hap_desc_char_sensor_motion_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if SENSOR_MOTION_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * hSensorMotion_linked_svcs[] = {};
#endif

#if(SENSOR_MOTION_IS_HIDDEN_SERVICE == 1)
const hap_service_desc_t hap_desc_service_sensor_motion_A5_R_S = {SENSOR_MOTION_INST_NO+1, (0x02 + SENSOR_MOTION_AS_PRIMARY_SERVICE), SENSOR_MOTION_LINKED_SVCS_LENGTH, SENSOR_MOTION_LINKED_SVCS};
#else
const hap_service_desc_t hap_desc_service_sensor_motion_A5_R_S = {SENSOR_MOTION_INST_NO+1, (0x00 + SENSOR_MOTION_AS_PRIMARY_SERVICE), SENSOR_MOTION_LINKED_SVCS_LENGTH, SENSOR_MOTION_LINKED_SVCS};
#endif


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
int hSensor_Motion_DetectedReadCallback(void *pp)
{
    if ((hSensor_Motion_Detected <= 1) && (hSensor_Motion_Detected >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_MOTION_SERVICE_ENABLE
        SENSOR_MOTION_STORE_CHAR_VALUE_T_SENSOR_MOTION_DETECTED_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Motion_StatusActiveReadCallback(void *pp)
{
    if ((hSensor_Motion_StatusActive <= 1) && (hSensor_Motion_StatusActive >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_MOTION_SERVICE_ENABLE
        SENSOR_MOTION_STORE_CHAR_VALUE_T_SENSOR_MOTION_STATUSACTIVE_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Motion_StatusFaultReadCallback(void *pp)
{
    if ((hSensor_Motion_StatusFault <= 1) && (hSensor_Motion_StatusFault >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_MOTION_SERVICE_ENABLE
        SENSOR_MOTION_STORE_CHAR_VALUE_T_SENSOR_MOTION_STATUSFAULT_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Motion_StatusTamperedReadCallback(void *pp)
{
    if ((hSensor_Motion_StatusTampered <= 1) && (hSensor_Motion_StatusTampered >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_MOTION_SERVICE_ENABLE
        SENSOR_MOTION_STORE_CHAR_VALUE_T_SENSOR_MOTION_STATUSTAMPERED_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Motion_StatusLowBatteryReadCallback(void *pp)
{
    if ((hSensor_Motion_StatusLowBattery <= 1) && (hSensor_Motion_StatusLowBattery >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_MOTION_SERVICE_ENABLE
        SENSOR_MOTION_STORE_CHAR_VALUE_T_SENSOR_MOTION_STATUSLOWBATTERY_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorMotion_DetectedWriteCB(void *pp)
{
    static u16 ccc_hSensorMotion_Detected_pre = 0;

    if(ccc_hSensorMotion_Detected_pre != ccc_hSensorMotion_Detected)
    {
        if(ccc_hSensorMotion_Detected == 0)
        {
            ccc_hSensorMotion_Detected_pre = ccc_hSensorMotion_Detected;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORMOTION_HANDLE_NUM_DETECTED);
        }
        else
        {
            ccc_hSensorMotion_Detected_pre = ccc_hSensorMotion_Detected;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORMOTION_HANDLE_NUM_DETECTED);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorMotion_StatusActiveWriteCB(void *pp)
{
    static u16 ccc_hSensorMotion_StatusActive_pre = 0;

    if(ccc_hSensorMotion_StatusActive_pre != ccc_hSensorMotion_StatusActive)
    {
        if(ccc_hSensorMotion_StatusActive == 0)
        {
            ccc_hSensorMotion_StatusActive_pre = ccc_hSensorMotion_StatusActive;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORMOTION_HANDLE_NUM_STATUSACTIVE);
        }
        else
        {
            ccc_hSensorMotion_StatusActive_pre = ccc_hSensorMotion_StatusActive;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORMOTION_HANDLE_NUM_STATUSACTIVE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorMotion_StatusFaultWriteCB(void *pp)
{
    static u16 ccc_hSensorMotion_StatusFault_pre = 0;

    if(ccc_hSensorMotion_StatusFault_pre != ccc_hSensorMotion_StatusFault)
    {
        if(ccc_hSensorMotion_StatusFault == 0)
        {
            ccc_hSensorMotion_StatusFault_pre = ccc_hSensorMotion_StatusFault;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORMOTION_HANDLE_NUM_STATUSFAULT);
        }
        else
        {
            ccc_hSensorMotion_StatusFault_pre = ccc_hSensorMotion_StatusFault;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORMOTION_HANDLE_NUM_STATUSFAULT);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorMotion_StatusTamperedWriteCB(void *pp)
{
    static u16 ccc_hSensorMotion_StatusTampered_pre = 0;

    if(ccc_hSensorMotion_StatusTampered_pre != ccc_hSensorMotion_StatusTampered)
    {
        if(ccc_hSensorMotion_StatusTampered == 0)
        {
            ccc_hSensorMotion_StatusTampered_pre = ccc_hSensorMotion_StatusTampered;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORMOTION_HANDLE_NUM_STATUSTAMPERED);
        }
        else
        {
            ccc_hSensorMotion_StatusTampered_pre = ccc_hSensorMotion_StatusTampered;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORMOTION_HANDLE_NUM_STATUSTAMPERED);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorMotion_StatusLowBatteryWriteCB(void *pp)
{
    static u16 ccc_hSensorMotion_StatusLowBattery_pre = 0;

    if(ccc_hSensorMotion_StatusLowBattery_pre != ccc_hSensorMotion_StatusLowBattery)
    {
        if(ccc_hSensorMotion_StatusLowBattery == 0)
        {
            ccc_hSensorMotion_StatusLowBattery_pre = ccc_hSensorMotion_StatusLowBattery;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORMOTION_HANDLE_NUM_STATUSLOWBATTERY);
        }
        else
        {
            ccc_hSensorMotion_StatusLowBattery_pre = ccc_hSensorMotion_StatusLowBattery;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORMOTION_HANDLE_NUM_STATUSLOWBATTERY);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
