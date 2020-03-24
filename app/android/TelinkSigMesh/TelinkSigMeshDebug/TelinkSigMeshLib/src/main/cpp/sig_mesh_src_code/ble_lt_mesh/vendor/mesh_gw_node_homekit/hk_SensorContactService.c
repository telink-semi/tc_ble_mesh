/********************************************************************************************************
 * @file     hk_SensorContactService.c
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
#define _HK_SENSOR_CONTACT_SERVICE_C_


/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "hk_SensorContactService.h"
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
unsigned char hSensor_Contact_State            = 0;  // 1
unsigned char hSensor_Contact_StatusActive     = 0;  // 3
unsigned char hSensor_Contact_StatusFault      = 0;  // 4
unsigned char hSensor_Contact_StatusTampered   = 0;  // 5
unsigned char hSensor_Contact_StatusLowBattery = 0;  // 6
unsigned char VAR_SENSOR_CONTACT_UNKOWN=0;

unsigned short ccc_hSensorContact_State = 0;
unsigned short ccc_hSensorContact_StatusActive = 0;
unsigned short ccc_hSensorContact_StatusFault = 0;
unsigned short ccc_hSensorContact_StatusTampered = 0;
unsigned short ccc_hSensorContact_StatusLowBattery = 0;

#if SENSORCONTACT_STATE_BRC_ENABLE
    unsigned char bc_para_hSensorContact_State = 1;
#endif

#if SENSORCONTACT_STATUSACTIVE_BRC_ENABLE
    unsigned char bc_para_hSensorContact_StatusActive = 1;
#endif

#if SENSORCONTACT_STATUSFAULT_BRC_ENABLE
    unsigned char bc_para_hSensorContact_StatusFault = 1;
#endif

#if SENSORCONTACT_STATUSTAMPERED_BRC_ENABLE
    unsigned char bc_para_hSensorContact_StatusTampered = 1;
#endif

#if SENSORCONTACT_STATUSLOWBATTERY_BRC_ENABLE
    unsigned char bc_para_hSensorContact_StatusLowBattery = 1;
#endif

const u8 hSensor_ContactServiceName[21] = "Sensor_ContactService";

const u8 VALID_RANGE_hSensor_Contact_State[2]            = { 0x00, 0x01};
const u8 VALID_RANGE_hSensor_Contact_StatusFault[2]      = { 0x00, 0x01};
const u8 VALID_RANGE_hSensor_Contact_StatusTampered[2]   = { 0x00, 0x01};
const u8 VALID_RANGE_hSensor_Contact_StatusLowBattery[2] = { 0x00, 0x01};

const u8 VALID_STEP_hSensor_Contact_State            = 0x01;
const u8 VALID_STEP_hSensor_Contact_StatusFault      = 0x01;
const u8 VALID_STEP_hSensor_Contact_StatusTampered   = 0x01;
const u8 VALID_STEP_hSensor_Contact_StatusLowBattery = 0x01;

const hap_characteristic_desc_t hap_desc_char_hSensor_Contact_State_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSensor_Contact_State, &VALID_STEP_hSensor_Contact_State, BC_PARA_SENSORCONTACT_STATE};
const hap_characteristic_desc_t hap_desc_char_hSensor_Contact_StatusActive_R_S = {hap_PRead_NOTIFY, 0, CFG_BOOL_UNITLESS, 0, 0, 0, 0, 0, 0, 0, BC_PARA_SENSORCONTACT_STATUSACTIVE};
const hap_characteristic_desc_t hap_desc_char_hSensor_Contact_StatusFault_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSensor_Contact_StatusFault, &VALID_STEP_hSensor_Contact_StatusFault, BC_PARA_SENSORCONTACT_STATUSFAULT};
const hap_characteristic_desc_t hap_desc_char_hSensor_Contact_StatusTampered_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSensor_Contact_StatusTampered, &VALID_STEP_hSensor_Contact_StatusTampered, BC_PARA_SENSORCONTACT_STATUSTAMPERED};
const hap_characteristic_desc_t hap_desc_char_hSensor_Contact_StatusLowBattery_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSensor_Contact_StatusLowBattery, &VALID_STEP_hSensor_Contact_StatusLowBattery, BC_PARA_SENSORCONTACT_STATUSLOWBATTERY};

const hap_characteristic_desc_t hap_desc_char_sensor_contact_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if SENSOR_CONTACT_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * hSensorContact_linked_svcs[] = {};
#endif

#if(SENSOR_CONTACT_IS_HIDDEN_SERVICE == 1)
const hap_service_desc_t hap_desc_service_sensor_contact_A5_R_S = {SENSOR_CONTACT_INST_NO+1, (0x02 + SENSOR_CONTACT_AS_PRIMARY_SERVICE), SENSOR_CONTACT_LINKED_SVCS_LENGTH, SENSOR_CONTACT_LINKED_SVCS};
#else
const hap_service_desc_t hap_desc_service_sensor_contact_A5_R_S = {SENSOR_CONTACT_INST_NO+1, (0x00 + SENSOR_CONTACT_AS_PRIMARY_SERVICE), SENSOR_CONTACT_LINKED_SVCS_LENGTH, SENSOR_CONTACT_LINKED_SVCS};
#endif

/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
int hSensor_Contact_StateReadCallback(void *pp)
{
    if ((hSensor_Contact_State <= 1) && (hSensor_Contact_State >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_CONTACT_SERVICE_ENABLE
        SENSOR_CONTACT_STORE_CHAR_VALUE_T_SENSOR_CONTACT_STATE_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Contact_StatusActiveReadCallback(void *pp)
{
    if ((hSensor_Contact_StatusActive <= 1) && (hSensor_Contact_StatusActive >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_CONTACT_SERVICE_ENABLE
        SENSOR_CONTACT_STORE_CHAR_VALUE_T_SENSOR_CONTACT_STATUSACTIVE_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Contact_StatusFaultReadCallback(void *pp)
{
    if ((hSensor_Contact_StatusFault <= 1) && (hSensor_Contact_StatusFault >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_CONTACT_SERVICE_ENABLE
        SENSOR_CONTACT_STORE_CHAR_VALUE_T_SENSOR_CONTACT_STATUSFAULT_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Contact_StatusTamperedReadCallback(void *pp)
{
    if ((hSensor_Contact_StatusTampered <= 1) && (hSensor_Contact_StatusTampered >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_CONTACT_SERVICE_ENABLE
        SENSOR_CONTACT_STORE_CHAR_VALUE_T_SENSOR_CONTACT_STATUSTAMPERED_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Contact_StatusLowBatteryReadCallback(void *pp)
{
    if ((hSensor_Contact_StatusLowBattery <= 1) && (hSensor_Contact_StatusLowBattery >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_CONTACT_SERVICE_ENABLE
        SENSOR_CONTACT_STORE_CHAR_VALUE_T_SENSOR_CONTACT_STATUSLOWBATTERY_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorContact_StateWriteCB(void *pp)
{
    static u16 ccc_hSensorContact_State_pre = 0;

    if(ccc_hSensorContact_State_pre != ccc_hSensorContact_State)
    {
        if(ccc_hSensorContact_State == 0)
        {
            ccc_hSensorContact_State_pre = ccc_hSensorContact_State;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORCONTACT_HANDLE_NUM_STATE);
        }
        else
        {
            ccc_hSensorContact_State_pre = ccc_hSensorContact_State;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORCONTACT_HANDLE_NUM_STATE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorContact_StatusActiveWriteCB(void *pp)
{
    static u16 ccc_hSensorContact_StatusActive_pre = 0;

    if(ccc_hSensorContact_StatusActive_pre != ccc_hSensorContact_StatusActive)
    {
        if(ccc_hSensorContact_StatusActive == 0)
        {
            ccc_hSensorContact_StatusActive_pre = ccc_hSensorContact_StatusActive;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORCONTACT_HANDLE_NUM_STATUSACTIVE);
        }
        else
        {
            ccc_hSensorContact_StatusActive_pre = ccc_hSensorContact_StatusActive;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORCONTACT_HANDLE_NUM_STATUSACTIVE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorContact_StatusFaultWriteCB(void *pp)
{
    static u16 ccc_hSensorContact_StatusFault_pre = 0;

    if(ccc_hSensorContact_StatusFault_pre != ccc_hSensorContact_StatusFault)
    {
        if(ccc_hSensorContact_StatusFault == 0)
        {
            ccc_hSensorContact_StatusFault_pre = ccc_hSensorContact_StatusFault;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORCONTACT_HANDLE_NUM_STATUSFAULT);
        }
        else
        {
            ccc_hSensorContact_StatusFault_pre = ccc_hSensorContact_StatusFault;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORCONTACT_HANDLE_NUM_STATUSFAULT);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorContact_StatusTamperedWriteCB(void *pp)
{
    static u16 ccc_hSensorContact_StatusTampered_pre = 0;

    if(ccc_hSensorContact_StatusTampered_pre != ccc_hSensorContact_StatusTampered)
    {
        if(ccc_hSensorContact_StatusTampered == 0)
        {
            ccc_hSensorContact_StatusTampered_pre = ccc_hSensorContact_StatusTampered;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORCONTACT_HANDLE_NUM_STATUSTAMPERED);
        }
        else
        {
            ccc_hSensorContact_StatusTampered_pre = ccc_hSensorContact_StatusTampered;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORCONTACT_HANDLE_NUM_STATUSTAMPERED);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorContact_StatusLowBatteryWriteCB(void *pp)
{
    static u16 ccc_hSensorContact_StatusLowBattery_pre = 0;

    if(ccc_hSensorContact_StatusLowBattery_pre != ccc_hSensorContact_StatusLowBattery)
    {
        if(ccc_hSensorContact_StatusLowBattery == 0)
        {
            ccc_hSensorContact_StatusLowBattery_pre = ccc_hSensorContact_StatusLowBattery;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORCONTACT_HANDLE_NUM_STATUSLOWBATTERY);
        }
        else
        {
            ccc_hSensorContact_StatusLowBattery_pre = ccc_hSensorContact_StatusLowBattery;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORCONTACT_HANDLE_NUM_STATUSLOWBATTERY);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
