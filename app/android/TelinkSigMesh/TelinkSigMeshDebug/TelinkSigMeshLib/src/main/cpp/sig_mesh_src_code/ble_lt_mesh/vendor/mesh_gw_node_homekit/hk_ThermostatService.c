/********************************************************************************************************
 * @file     hk_ThermostatService.c
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
#define _HK_THERMOSTAT_SERVICE_C_
 
 
/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "hk_ThermostatService.h"
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
unsigned char hThermostatCurrentHeatingCoolingState = 0;
unsigned char hThermostatTargetHeatingCoolingState = 0;
unsigned int hThermostatCurrentTemperature = 0;
unsigned int hThermostatTargetTemperature = 0x41200000;  // 10
unsigned char hThermostatTemperatureDisplayUnits = 0;
unsigned int hThermostatCoolingThresholdTemperature = 0x41200000;  // 10
unsigned int hThermostatCurrentRelativeHumidity = 0;
unsigned int hThermostatHeatingThresholdTemperature = 0;
unsigned int hThermostatTargetRelativeHumidity = 0;
unsigned char VAR_THERMOSTAT_UNKOWN=0;

unsigned short ccc_hThermostatCurrentHeatingCoolingState = 0;
unsigned short ccc_hThermostatTargetHeatingCoolingState = 0;
unsigned short ccc_hThermostatCurrentTemperature = 0;
unsigned short ccc_hThermostatTargetTemperature = 0;
unsigned short ccc_hThermostatTemperatureDisplayUnits = 0;
unsigned short ccc_hThermostatCoolingThresholdTemperature = 0;
unsigned short ccc_hThermostatCurrentRelativeHumidity = 0;
unsigned short ccc_hThermostatHeatingThresholdTemperature = 0;
unsigned short ccc_hThermostatTargetRelativeHumidity = 0;

#if THERMOSTAT_CURRENTHEATINGCOOLINGSTATE_BRC_ENABLE
    unsigned char bc_para_hThermostatCurrentHeatingCoolingState = 1;
#endif

#if THERMOSTAT_TARGETHEATINGCOOLINGSTATE_BRC_ENABLE
    unsigned char bc_para_hThermostatTargetHeatingCoolingState = 1;
#endif

#if THERMOSTAT_CURRENTTEMPERATURE_BRC_ENABLE
    unsigned char bc_para_hThermostatCurrentTemperature = 1;
#endif

#if THERMOSTAT_TARGETTEMPERATURE_BRC_ENABLE
    unsigned char bc_para_hThermostatTargetTemperature = 1;
#endif

#if THERMOSTAT_TEMPERATUREDISPLAYUNITS_BRC_ENABLE
    unsigned char bc_para_hThermostatTemperatureDisplayUnits = 1;
#endif

#if THERMOSTAT_COOLINGTHRESHOLDTEMPERATURE_BRC_ENABLE
    unsigned char bc_para_hThermostatCoolingThresholdTemperature = 1;
#endif

#if THERMOSTAT_CURRENTRELATIVEHUMIDITY_BRC_ENABLE
    unsigned char bc_para_hThermostatCurrentRelativeHumidity = 1;
#endif

#if THERMOSTAT_HEATINGTHRESHOLDTEMPERATURE_BRC_ENABLE
    unsigned char bc_para_hThermostatHeatingThresholdTemperature = 1;
#endif

#if THERMOSTAT_TARGETRELATIVEHUMIDITY_BRC_ENABLE
    unsigned char bc_para_hThermostatTargetRelativeHumidity = 1;
#endif


const u8 hThermostatServiceName[17] = "ThermostatService";

const u8 VALID_RANGE_hThermostatCurrentHeatingCoolingState[2] = { 0x00, 0x02};
const u8 VALID_RANGE_hThermostatTargetHeatingCoolingState[2] = { 0x00, 0x03};
const u8 VALID_RANGE_hThermostatCurrentTemperature[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC8, 0x42};	// 0~100 (float)
const u8 VALID_RANGE_hThermostatTargetTemperature[8] = { 0x00, 0x00, 0x20, 0x41, 0x00, 0x00, 0x18, 0x42};	// 10~38 (float)
const u8 VALID_RANGE_hThermostatTemperatureDisplayUnits[2] = { 0x00, 0x01};

const u8 VALID_RANGE_hThermostatCoolingThresholdTemperature[8]  = { 0x00, 0x00, 0x20, 0x41, 0x00, 0x00, 0x0C, 0x42};	// 10~35 (float)
const u8 VALID_RANGE_hThermostatCurrentRelativeHumidity[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC8, 0x42};	// 0~100 (float)
const u8 VALID_RANGE_hThermostatHeatingThresholdTemperature[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC8, 0x41};	// 0~25 (float)
const u8 VALID_RANGE_hThermostatTargetRelativeHumidity[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC8, 0x42};	// 0~100 (float)

const u8 VALID_STEP_hThermostatCurrentHeatingCoolingState = 0x01;
const u8 VALID_STEP_hThermostatTargetHeatingCoolingState = 0x01;
const u8 VALID_STEP_hThermostatCurrentTemperature[4] = { 0xCD, 0xCC, 0xCC, 0x3D};
const u8 VALID_STEP_hThermostatTargetTemperature[4] = { 0xCD, 0xCC, 0xCC, 0x3D};
const u8 VALID_STEP_hThermostatTemperatureDisplayUnits = 0x01;

const u8 VALID_STEP_hThermostatCoolingThresholdTemperature[4] = { 0xCD, 0xCC, 0xCC, 0x3D};
const u8 VALID_STEP_hThermostatCurrentRelativeHumidity[4] = { 0x00, 0x00, 0x80, 0x3F};
const u8 VALID_STEP_hThermostatHeatingThresholdTemperature[4] = { 0xCD, 0xCC, 0xCC, 0x3D};
const u8 VALID_STEP_hThermostatTargetRelativeHumidity[4] = { 0x00, 0x00, 0x80, 0x3F};

const hap_characteristic_desc_t hap_desc_char_hThermostatCurrentHeatingCoolingState_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hThermostatCurrentHeatingCoolingState, &VALID_STEP_hThermostatCurrentHeatingCoolingState, BC_PARA_THERMOSTAT_CURRENTHEATINGCOOLINGSTATE};
const hap_characteristic_desc_t hap_desc_char_hThermostatTargetHeatingCoolingState_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hThermostatTargetHeatingCoolingState, &VALID_STEP_hThermostatTargetHeatingCoolingState, BC_PARA_THERMOSTAT_TARGETHEATINGCOOLINGSTATE};
const hap_characteristic_desc_t hap_desc_char_hThermostatCurrentTemperature_R_S = {hap_PRead_NOTIFY, 0, CFG_FLOAT32_CELSIUS, 0, 8, 4, 0, 0, VALID_RANGE_hThermostatCurrentTemperature, VALID_STEP_hThermostatCurrentTemperature, BC_PARA_THERMOSTAT_CURRENTTEMPERATURE};
const hap_characteristic_desc_t hap_desc_char_hThermostatTargetTemperature_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_FLOAT32_CELSIUS, 0, 8, 4, 0, 0, VALID_RANGE_hThermostatTargetTemperature, VALID_STEP_hThermostatTargetTemperature, BC_PARA_THERMOSTAT_TARGETTEMPERATURE};
const hap_characteristic_desc_t hap_desc_char_hThermostatTemperatureDisplayUnits_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hThermostatTemperatureDisplayUnits, &VALID_STEP_hThermostatTemperatureDisplayUnits, BC_PARA_THERMOSTAT_TEMPERATUREDISPLAYUNITS};

const hap_characteristic_desc_t hap_desc_char_hThermostatCoolingThresholdTemperature_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_FLOAT32_CELSIUS, 0, 8, 4, 0, 0, VALID_RANGE_hThermostatCoolingThresholdTemperature, VALID_STEP_hThermostatCoolingThresholdTemperature, BC_PARA_THERMOSTAT_COOLINGTHRESHOLDTEMPERATURE};
const hap_characteristic_desc_t hap_desc_char_hThermostatCurrentRelativeHumidity_R_S = {hap_PRead_NOTIFY, 0, CFG_FLOAT32_PERCENTAGE, 0, 8, 4, 0, 0, VALID_RANGE_hThermostatCurrentRelativeHumidity, VALID_STEP_hThermostatCurrentRelativeHumidity, BC_PARA_THERMOSTAT_CURRENTRELATIVEHUMIDITY};
const hap_characteristic_desc_t hap_desc_char_hThermostatHeatingThresholdTemperature_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_FLOAT32_CELSIUS, 0, 8, 4, 0, 0, VALID_RANGE_hThermostatHeatingThresholdTemperature, VALID_STEP_hThermostatHeatingThresholdTemperature, BC_PARA_THERMOSTAT_HEATINGTHRESHOLDTEMPERATURE};
const hap_characteristic_desc_t hap_desc_char_hThermostatTargetRelativeHumidity_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_FLOAT32_PERCENTAGE, 0, 8, 4, 0, 0, VALID_RANGE_hThermostatTargetRelativeHumidity, VALID_STEP_hThermostatTargetRelativeHumidity, BC_PARA_THERMOSTAT_TARGETRELATIVEHUMIDITY};

const hap_characteristic_desc_t hap_desc_char_thermostat_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if THERMOSTAT_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * hThermostat_linked_svcs[] = {};
#endif

#if(THERMOSTAT_IS_HIDDEN_SERVICE == 1)
const hap_service_desc_t hap_desc_service_thermostat_A5_R_S = {THERMOSTAT_INST_NO+1, (0x02 + THERMOSTAT_AS_PRIMARY_SERVICE), THERMOSTAT_LINKED_SVCS_LENGTH, THERMOSTAT_LINKED_SVCS};
#else
const hap_service_desc_t hap_desc_service_thermostat_A5_R_S = {THERMOSTAT_INST_NO+1, (0x00 + THERMOSTAT_AS_PRIMARY_SERVICE), THERMOSTAT_LINKED_SVCS_LENGTH, THERMOSTAT_LINKED_SVCS};
#endif


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
int hThermostatCurrentHeatingCoolingStateReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hThermostatTargetHeatingCoolingStateReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hThermostatTargetHeatingCoolingStateWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~3
        assign_current_flash_char_value_addr();
    #if THERMOSTAT_SERVICE_ENABLE
        THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATTARGETHEATINGCOOLINGSTATE_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
        hThermostatTargetHeatingCoolingState = hThermostatTargetHeatingCoolingState & 0x3;
        hThermostatCurrentHeatingCoolingState = hThermostatTargetHeatingCoolingState;   // update to Current state
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hThermostatCurrentTemperatureReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hThermostatTargetTemperatureReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hThermostatTargetTemperatureWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 10.0~38.0
        assign_current_flash_char_value_addr();
    #if THERMOSTAT_SERVICE_ENABLE
        THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATTARGETTEMPERATURE_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
        hThermostatCurrentTemperature = hThermostatTargetTemperature;   // update to Current state
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hThermostatTemperatureDisplayUnitsReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hThermostatCoolingThresholdTemperatureReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hThermostatCoolingThresholdTemperatureWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~3
        assign_current_flash_char_value_addr();
    #if THERMOSTAT_SERVICE_ENABLE
        THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATCOOLINGTHRESHOLDTEMPERATURE_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hThermostatCurrentRelativeHumidityReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}


int hThermostatHeatingThresholdTemperatureReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hThermostatHeatingThresholdTemperatureWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~3
        assign_current_flash_char_value_addr();
    #if THERMOSTAT_SERVICE_ENABLE
        THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATHEATINGTHRESHOLDTEMPERATURE_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}


int hThermostatTargetRelativeHumidityReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hThermostatTargetRelativeHumidityWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~3
        assign_current_flash_char_value_addr();
    #if THERMOSTAT_SERVICE_ENABLE
        THERMOSTAT_STORE_CHAR_VALUE_T_THERMOSTATTARGETRELATIVEHUMIDITY_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}



int ccc_hThermostatCurrentHeatingCoolingStateWriteCB(void *pp)
{
    static u16 ccc_hThermostatCurrentHeatingCoolingState_pre = 0;

    if(ccc_hThermostatCurrentHeatingCoolingState_pre != ccc_hThermostatCurrentHeatingCoolingState)
    {
        if(ccc_hThermostatCurrentHeatingCoolingState == 0)
        {
            ccc_hThermostatCurrentHeatingCoolingState_pre = ccc_hThermostatCurrentHeatingCoolingState;
            EventNotifyCNT--;
            indicate_handle_no_remove(THERMOSTAT_HANDLE_NUM_CURRENTHEATINGCOOLINGSTATE);
        }
        else
        {
            ccc_hThermostatCurrentHeatingCoolingState_pre = ccc_hThermostatCurrentHeatingCoolingState;
            EventNotifyCNT++;
            indicate_handle_no_reg(THERMOSTAT_HANDLE_NUM_CURRENTHEATINGCOOLINGSTATE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hThermostatTargetHeatingCoolingStateWriteCB(void *pp)
{
    static u16 ccc_hThermostatTargetHeatingCoolingState_pre = 0;

    if(ccc_hThermostatTargetHeatingCoolingState_pre != ccc_hThermostatTargetHeatingCoolingState)
    {
        if(ccc_hThermostatTargetHeatingCoolingState == 0)
        {
            ccc_hThermostatTargetHeatingCoolingState_pre = ccc_hThermostatTargetHeatingCoolingState;
            EventNotifyCNT--;
            indicate_handle_no_remove(THERMOSTAT_HANDLE_NUM_TARGETHEATINGCOOLINGSTATE);
        }
        else
        {
            ccc_hThermostatTargetHeatingCoolingState_pre = ccc_hThermostatTargetHeatingCoolingState;
            EventNotifyCNT++;
            indicate_handle_no_reg(THERMOSTAT_HANDLE_NUM_TARGETHEATINGCOOLINGSTATE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hThermostatCurrentTemperatureWriteCB(void *pp)
{
    static u16 ccc_hThermostatCurrentTemperature_pre = 0;

    if(ccc_hThermostatCurrentTemperature_pre != ccc_hThermostatCurrentTemperature)
    {
        if(ccc_hThermostatCurrentTemperature == 0)
        {
            ccc_hThermostatCurrentTemperature_pre = ccc_hThermostatCurrentTemperature;
            EventNotifyCNT--;
            indicate_handle_no_remove(THERMOSTAT_HANDLE_NUM_CURRENTTEMPERATURE);
        }
        else
        {
            ccc_hThermostatCurrentTemperature_pre = ccc_hThermostatCurrentTemperature;
            EventNotifyCNT++;
            indicate_handle_no_reg(THERMOSTAT_HANDLE_NUM_CURRENTTEMPERATURE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hThermostatTargetTemperatureWriteCB(void *pp)
{
    static u16 ccc_hThermostatTargetTemperature_pre = 0;

    if(ccc_hThermostatTargetTemperature_pre != ccc_hThermostatTargetTemperature)
    {
        if(ccc_hThermostatTargetTemperature == 0)
        {
            ccc_hThermostatTargetTemperature_pre = ccc_hThermostatTargetTemperature;
            EventNotifyCNT--;
            indicate_handle_no_remove(THERMOSTAT_HANDLE_NUM_TARGETTEMPERATURE);
        }
        else
        {
            ccc_hThermostatTargetTemperature_pre = ccc_hThermostatTargetTemperature;
            EventNotifyCNT++;
            indicate_handle_no_reg(THERMOSTAT_HANDLE_NUM_TARGETTEMPERATURE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hThermostatTemperatureDisplayUnitsWriteCB(void *pp)
{
    static u16 ccc_hThermostatTemperatureDisplayUnits_pre = 0;

    if(ccc_hThermostatTemperatureDisplayUnits_pre != ccc_hThermostatTemperatureDisplayUnits)
    {
        if(ccc_hThermostatTemperatureDisplayUnits == 0)
        {
            ccc_hThermostatTemperatureDisplayUnits_pre = ccc_hThermostatTemperatureDisplayUnits;
            EventNotifyCNT--;
            indicate_handle_no_remove(THERMOSTAT_HANDLE_NUM_TEMPDISUNITS);
        }
        else
        {
            ccc_hThermostatTemperatureDisplayUnits_pre = ccc_hThermostatTemperatureDisplayUnits;
            EventNotifyCNT++;
            indicate_handle_no_reg(THERMOSTAT_HANDLE_NUM_TEMPDISUNITS);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hThermostatCoolingThresholdTemperatureWriteCB(void *pp)
{
    static u16 ccc_hThermostatCoolingThresholdTemperature_pre = 0;

    if(ccc_hThermostatCoolingThresholdTemperature_pre != ccc_hThermostatCoolingThresholdTemperature)
    {
        if(ccc_hThermostatCoolingThresholdTemperature == 0)
        {
            ccc_hThermostatCoolingThresholdTemperature_pre = ccc_hThermostatCoolingThresholdTemperature;
            EventNotifyCNT--;
            indicate_handle_no_remove(THERMOSTAT_HANDLE_NUM_COOLINGTHRESHOLDTEMPERATURE);
        }
        else
        {
            ccc_hThermostatCoolingThresholdTemperature_pre = ccc_hThermostatCoolingThresholdTemperature;
            EventNotifyCNT++;
            indicate_handle_no_reg(THERMOSTAT_HANDLE_NUM_COOLINGTHRESHOLDTEMPERATURE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hThermostatCurrentRelativeHumidityWriteCB(void *pp)
{
    static u16 ccc_hThermostatCurrentRelativeHumidity_pre = 0;

    if(ccc_hThermostatCurrentRelativeHumidity_pre != ccc_hThermostatCurrentRelativeHumidity)
    {
        if(ccc_hThermostatCurrentRelativeHumidity == 0)
        {
            ccc_hThermostatCurrentRelativeHumidity_pre = ccc_hThermostatCurrentRelativeHumidity;
            EventNotifyCNT--;
            indicate_handle_no_remove(THERMOSTAT_HANDLE_NUM_CURRENTRELATIVEHUMIDITY);
        }
        else
        {
            ccc_hThermostatCurrentRelativeHumidity_pre = ccc_hThermostatCurrentRelativeHumidity;
            EventNotifyCNT++;
            indicate_handle_no_reg(THERMOSTAT_HANDLE_NUM_CURRENTRELATIVEHUMIDITY);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hThermostatHeatingThresholdTemperatureWriteCB(void *pp)
{
    static u16 ccc_hThermostatHeatingThresholdTemperature_pre = 0;

    if(ccc_hThermostatHeatingThresholdTemperature_pre != ccc_hThermostatHeatingThresholdTemperature)
    {
        if(ccc_hThermostatHeatingThresholdTemperature == 0)
        {
            ccc_hThermostatHeatingThresholdTemperature_pre = ccc_hThermostatHeatingThresholdTemperature;
            EventNotifyCNT--;
            indicate_handle_no_remove(THERMOSTAT_HANDLE_NUM_HEATINGTHRESHOLDTEMPERATURE);
        }
        else
        {
            ccc_hThermostatHeatingThresholdTemperature_pre = ccc_hThermostatHeatingThresholdTemperature;
            EventNotifyCNT++;
            indicate_handle_no_reg(THERMOSTAT_HANDLE_NUM_HEATINGTHRESHOLDTEMPERATURE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hThermostatTargetRelativeHumidityWriteCB(void *pp)
{
    static u16 ccc_hThermostatTargetRelativeHumidity_pre = 0;

    if(ccc_hThermostatTargetRelativeHumidity_pre != ccc_hThermostatTargetRelativeHumidity)
    {
        if(ccc_hThermostatTargetRelativeHumidity == 0)
        {
            ccc_hThermostatTargetRelativeHumidity_pre = ccc_hThermostatTargetRelativeHumidity;
            EventNotifyCNT--;
            indicate_handle_no_remove(THERMOSTAT_HANDLE_NUM_TARGETRELATIVEHUMIDITY);
        }
        else
        {
            ccc_hThermostatTargetRelativeHumidity_pre = ccc_hThermostatTargetRelativeHumidity;
            EventNotifyCNT++;
            indicate_handle_no_reg(THERMOSTAT_HANDLE_NUM_TARGETRELATIVEHUMIDITY);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
