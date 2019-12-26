/********************************************************************************************************
 * @file     hk_HeaterCoolerService.c
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
#define _HK_HEATER_COOLER_SERVICE_C_
 
 
/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "hk_HeaterCoolerService.h"
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
unsigned char hHeaterCoolerActive = 0;
unsigned int hHeaterCoolerCurrentTemperature = 0;
unsigned char hHeaterCoolerCurrentHeaterCoolerState = 0;
unsigned char hHeaterCoolerTargetHeaterCoolerState = 0;
unsigned int hHeaterCoolerRotationSpeed = 0;
unsigned char hHeaterCoolerTemperatureDisplayUnits = 0;
unsigned char hHeaterCoolerSwingMode = 0;
unsigned int hHeaterCoolerCoolingThresholdTemp = 0x41200000;  // 10
unsigned int hHeaterCoolerHeatingThresholdTemp = 0;
unsigned char hHeaterCoolerLockPhysicalControls = 0;

unsigned char VAR_HEATER_COOLER_UNKOWN=0;

unsigned short ccc_hHeaterCoolerActive = 0;
unsigned short ccc_hHeaterCoolerCurrentTemperature = 0;
unsigned short ccc_hHeaterCoolerCurrentHeaterCoolerState = 0;
unsigned short ccc_hHeaterCoolerTargetHeaterCoolerState = 0;
unsigned short ccc_hHeaterCoolerRotationSpeed = 0;
unsigned short ccc_hHeaterCoolerTemperatureDisplayUnits = 0;
unsigned short ccc_hHeaterCoolerSwingMode = 0;
unsigned short ccc_hHeaterCoolerCoolingThresholdTemp = 0;
unsigned short ccc_hHeaterCoolerHeatingThresholdTemp = 0;
unsigned short ccc_hHeaterCoolerLockPhysicalControls = 0;

#if HEATERCOOLER_ACTIVE_BRC_ENABLE
    unsigned char bc_para_hHeaterCoolerActive = 1;
#endif

#if HEATERCOOLER_CURRENTTEMPERATURE_BRC_ENABLE
    unsigned char bc_para_hHeaterCoolerCurrentTemperature = 1;
#endif

#if HEATERCOOLER_CURRENTHEATERCOOLERSTATE_BRC_ENABLE
    unsigned char bc_para_hHeaterCoolerCurrentHeaterCoolerState = 1;
#endif

#if HEATERCOOLER_TARGETHEATERCOOLERSTATE_BRC_ENABLE
    unsigned char bc_para_hHeaterCoolerTargetHeaterCoolerState = 1;
#endif

#if HEATERCOOLER_ROTATIONSPEED_BRC_ENABLE
    unsigned char bc_para_hHeaterCoolerRotationSpeed = 1;
#endif

#if HEATERCOOLER_TEMPERATUREDISPLAYUNITS_BRC_ENABLE
    unsigned char bc_para_hHeaterCoolerTemperatureDisplayUnits = 1;
#endif

#if HEATERCOOLER_SWINGMODE_BRC_ENABLE
    unsigned char bc_para_hHeaterCoolerSwingMode = 1;
#endif

#if HEATERCOOLER_COOLINGTHRESHOLDTEMP_BRC_ENABLE
    unsigned char bc_para_hHeaterCoolerCoolingThresholdTemp = 1;
#endif

#if HEATERCOOLER_HEATINGTHRESHOLDTEMP_BRC_ENABLE
    unsigned char bc_para_hHeaterCoolerHeatingThresholdTemp = 1;
#endif

#if HEATERCOOLER_LOCKPHYSICALCONTROLS_BRC_ENABLE
    unsigned char bc_para_hHeaterCoolerLockPhysicalControls = 1;
#endif

const u8 hHeaterCoolerServiceName[19] = "HeaterCoolerService";

const u8 HEATERCOOLERACTIVE_VALID_RANGE[2] = {0x00, 0x01};
const u8 HEATERCOOLERCURRENTTEMPERATURE_VALID_RANGE[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC8, 0x42};	 // 0~100 (float)
const u8 HEATERCOOLERCURRENTHEATERCOOLERSTATE_VALID_RANGE[2] = {0x00, 0x03};
const u8 HEATERCOOLERTARGETHEATERCOOLERSTATE_VALID_RANGE[2] = {0x00, 0x02};
const u8 HEATERCOOLERROTATIONSPEED_VALID_RANGE[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC8, 0x42};  // 0~100 (float)
const u8 HEATERCOOLERTEMPERATUREDISPLAYUNITS_VALID_RANGE[2] = {0x00, 0x01};
const u8 HEATERCOOLERSWINGMODE_VALID_RANGE[2] = {0x00, 0x01};
const u8 HEATERCOOLERCOOLINGTHRESHOLDTEMP_VALID_RANGE[8] = {0x00, 0x00, 0x20, 0x41, 0x00, 0x00, 0x0C, 0x42};	// 10~35 (float)
const u8 HEATERCOOLERHEATINGTHRESHOLDTEMP_VALID_RANGE[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC8, 0x41};	// 0~25 (float)
const u8 HEATERCOOLERLOCKPHYSICALCONTROLS_VALID_RANGE[2] = {0x00, 0x01};

const u8 HEATERCOOLERACTIVE_VALID_STEP = 0x01;
const u8 HEATERCOOLERCURRENTTEMPERATURE_VALID_STEP[4] = {0xCD, 0xCC, 0xCC, 0x3D};
const u8 HEATERCOOLERCURRENTHEATERCOOLERSTATE_VALID_STEP = 0x01;
const u8 HEATERCOOLERTARGETHEATERCOOLERSTATE_VALID_STEP = 0x01;
const u8 HEATERCOOLERROTATIONSPEED_VALID_STEP[4] = {0x00, 0x00, 0x80, 0x3F};
const u8 HEATERCOOLERTEMPERATUREDISPLAYUNITS_VALID_STEP = 0x01;
const u8 HEATERCOOLERSWINGMODE_VALID_STEP = 0x01;
const u8 HEATERCOOLERCOOLINGTHRESHOLDTEMP_VALID_STEP[4] = {0xCD, 0xCC, 0xCC, 0x3D};
const u8 HEATERCOOLERHEATINGTHRESHOLDTEMP_VALID_STEP[4] = {0xCD, 0xCC, 0xCC, 0x3D};
const u8 HEATERCOOLERLOCKPHYSICALCONTROLS_VALID_STEP = 0x01;

const hap_characteristic_desc_t hap_desc_char_HEATERCOOLER_ACTIVE_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, HEATERCOOLERACTIVE_VALID_RANGE, &HEATERCOOLERACTIVE_VALID_STEP, BC_PARA_HEATERCOOLER_ACTIVE};
const hap_characteristic_desc_t hap_desc_char_HEATERCOOLER_CURRENTTEMPERATURE_R_S = {hap_PRead_NOTIFY, 0, CFG_FLOAT32_CELSIUS, 0, 8, 4, 0, 0, HEATERCOOLERCURRENTTEMPERATURE_VALID_RANGE, HEATERCOOLERCURRENTTEMPERATURE_VALID_STEP, BC_PARA_HEATERCOOLER_CURRENTTEMPERATURE};
const hap_characteristic_desc_t hap_desc_char_HEATERCOOLER_CURRENTHEATERCOOLERSTATE_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, HEATERCOOLERCURRENTHEATERCOOLERSTATE_VALID_RANGE, &HEATERCOOLERCURRENTHEATERCOOLERSTATE_VALID_STEP, BC_PARA_HEATERCOOLER_CURRENTHEATERCOOLERSTATE};
const hap_characteristic_desc_t hap_desc_char_HEATERCOOLER_TARGETHEATERCOOLERSTATE_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, HEATERCOOLERTARGETHEATERCOOLERSTATE_VALID_RANGE, &HEATERCOOLERTARGETHEATERCOOLERSTATE_VALID_STEP, BC_PARA_HEATERCOOLER_TARGETHEATERCOOLERSTATE};
const hap_characteristic_desc_t hap_desc_char_HEATERCOOLER_ROTATIONSPEED_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_FLOAT32_PERCENTAGE, 0, 8, 4, 0, 0, HEATERCOOLERROTATIONSPEED_VALID_RANGE, HEATERCOOLERROTATIONSPEED_VALID_STEP, BC_PARA_HEATERCOOLER_ROTATIONSPEED};
const hap_characteristic_desc_t hap_desc_char_HEATERCOOLER_TEMPERATUREDISPLAYUNITS_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, HEATERCOOLERTEMPERATUREDISPLAYUNITS_VALID_RANGE, &HEATERCOOLERTEMPERATUREDISPLAYUNITS_VALID_STEP, BC_PARA_HEATERCOOLER_TEMPERATUREDISPLAYUNITS};
const hap_characteristic_desc_t hap_desc_char_HEATERCOOLER_SWINGMODE_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, HEATERCOOLERSWINGMODE_VALID_RANGE, &HEATERCOOLERSWINGMODE_VALID_STEP, BC_PARA_HEATERCOOLER_SWINGMODE};
const hap_characteristic_desc_t hap_desc_char_HEATERCOOLER_COOLINGTHRESHOLDTEMP_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_FLOAT32_CELSIUS, 0, 8, 4, 0, 0, HEATERCOOLERCOOLINGTHRESHOLDTEMP_VALID_RANGE, HEATERCOOLERCOOLINGTHRESHOLDTEMP_VALID_STEP, BC_PARA_HEATERCOOLER_COOLINGTHRESHOLDTEMP};
const hap_characteristic_desc_t hap_desc_char_HEATERCOOLER_HEATINGTHRESHOLDTEMP_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_FLOAT32_CELSIUS, 0, 8, 4, 0, 0, HEATERCOOLERHEATINGTHRESHOLDTEMP_VALID_RANGE, HEATERCOOLERHEATINGTHRESHOLDTEMP_VALID_STEP, BC_PARA_HEATERCOOLER_HEATINGTHRESHOLDTEMP};
const hap_characteristic_desc_t hap_desc_char_HEATERCOOLER_LOCKPHYSICALCONTROLS_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, HEATERCOOLERLOCKPHYSICALCONTROLS_VALID_RANGE, &HEATERCOOLERLOCKPHYSICALCONTROLS_VALID_STEP, BC_PARA_HEATERCOOLER_LOCKPHYSICALCONTROLS};

const hap_characteristic_desc_t hap_desc_char_heater_cooler_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if HEATERCOOLER_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * hHeaterCooler_linked_svcs[] = {};
#endif

#if(HEATERCOOLER_IS_HIDDEN_SERVICE == 1)
const hap_service_desc_t hap_desc_service_heater_cooler_A5_R_S = {HEATERCOOLER_INST_NO+1, (0x02 + HEATERCOOLER_AS_PRIMARY_SERVICE), HEATERCOOLER_LINKED_SVCS_LENGTH, HEATERCOOLER_LINKED_SVCS};
#else
const hap_service_desc_t hap_desc_service_heater_cooler_A5_R_S = {HEATERCOOLER_INST_NO+1, (0x00 + HEATERCOOLER_AS_PRIMARY_SERVICE), HEATERCOOLER_LINKED_SVCS_LENGTH, HEATERCOOLER_LINKED_SVCS};
#endif

/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
int HeaterCoolerActiveReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int HeaterCoolerActiveWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {
        assign_current_flash_char_value_addr();
    #if HEATER_COOLER_SERVICE_ENABLE
        HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_ACTIVE_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int HeaterCoolerCurrentTemperatureReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int HeaterCoolerCurrentHeaterCoolerStateReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int HeaterCoolerTargetHeaterCoolerStateReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int HeaterCoolerTargetHeaterCoolerStateWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {
        assign_current_flash_char_value_addr();
    #if HEATER_COOLER_SERVICE_ENABLE
        HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_TARGETHEATERCOOLERSTATE_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int HeaterCoolerRotationSpeedReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int HeaterCoolerRotationSpeedWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {
        assign_current_flash_char_value_addr();
    #if HEATER_COOLER_SERVICE_ENABLE
        HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_ROTATIONSPEED_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int HeaterCoolerTemperatureDisplayUnitsReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int HeaterCoolerTemperatureDisplayUnitsWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {
        assign_current_flash_char_value_addr();
    #if HEATER_COOLER_SERVICE_ENABLE
        HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_TEMPERATUREDISPLAYUNITS_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int HeaterCoolerSwingModeReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int HeaterCoolerSwingModeWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~1
        assign_current_flash_char_value_addr();
    #if HEATER_COOLER_SERVICE_ENABLE
        HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_SWINGMODE_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int HeaterCoolerCoolingThresholdTempReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int HeaterCoolerCoolingThresholdTempWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {
        assign_current_flash_char_value_addr();
    #if HEATER_COOLER_SERVICE_ENABLE
        HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_COOLINGTHRESHOLDTEMP_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int HeaterCoolerHeatingThresholdTempReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int HeaterCoolerHeatingThresholdTempWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {
        assign_current_flash_char_value_addr();
    #if HEATER_COOLER_SERVICE_ENABLE
        HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_HEATINGTHRESHOLDTEMP_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int HeaterCoolerLockPhysicalControlsReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int HeaterCoolerLockPhysicalControlsWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~1
        assign_current_flash_char_value_addr();
    #if HEATER_COOLER_SERVICE_ENABLE
        HEATERCOOLER_STORE_CHAR_VALUE_T_HEATERCOOLER_LOCKPHYSICALCONTROLS_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}


int ccc_HeaterCoolerActiveWriteCB(void *pp)
{
    static u16 ccc_hHeaterCoolerActive_pre = 0;

    if(ccc_hHeaterCoolerActive_pre != ccc_hHeaterCoolerActive)
    {
        if(ccc_hHeaterCoolerActive == 0)
        {
            ccc_hHeaterCoolerActive_pre = ccc_hHeaterCoolerActive;
            EventNotifyCNT--;
            indicate_handle_no_remove(HEATERCOOLER_HANDLE_NUM_ACTIVE);
        }
        else
        {
            ccc_hHeaterCoolerActive_pre = ccc_hHeaterCoolerActive;
            EventNotifyCNT++;
            indicate_handle_no_reg(HEATERCOOLER_HANDLE_NUM_ACTIVE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_HeaterCoolerCurrentTemperatureWriteCB(void *pp)
{
    static u16 ccc_hHeaterCoolerCurrentTemperature_pre = 0;

    if(ccc_hHeaterCoolerCurrentTemperature_pre != ccc_hHeaterCoolerCurrentTemperature)
    {
        if(ccc_hHeaterCoolerCurrentTemperature == 0)
        {
            ccc_hHeaterCoolerCurrentTemperature_pre = ccc_hHeaterCoolerCurrentTemperature;
            EventNotifyCNT--;
            indicate_handle_no_remove(HEATERCOOLER_HANDLE_NUM_CURRENTTEMPERATURE);
        }
        else
        {
            ccc_hHeaterCoolerCurrentTemperature_pre = ccc_hHeaterCoolerCurrentTemperature;
            EventNotifyCNT++;
            indicate_handle_no_reg(HEATERCOOLER_HANDLE_NUM_CURRENTTEMPERATURE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_HeaterCoolerCurrentHeaterCoolerStateWriteCB(void *pp)
{
    static u16 ccc_hHeaterCoolerCurrentHeaterCoolerState_pre = 0;

    if(ccc_hHeaterCoolerCurrentHeaterCoolerState_pre != ccc_hHeaterCoolerCurrentHeaterCoolerState)
    {
        if(ccc_hHeaterCoolerCurrentHeaterCoolerState == 0)
        {
            ccc_hHeaterCoolerCurrentHeaterCoolerState_pre = ccc_hHeaterCoolerCurrentHeaterCoolerState;
            EventNotifyCNT--;
            indicate_handle_no_remove(HEATERCOOLER_HANDLE_NUM_CURRENTHEATERCOOLERSTATE);
        }
        else
        {
            ccc_hHeaterCoolerCurrentHeaterCoolerState_pre = ccc_hHeaterCoolerCurrentHeaterCoolerState;
            EventNotifyCNT++;
            indicate_handle_no_reg(HEATERCOOLER_HANDLE_NUM_CURRENTHEATERCOOLERSTATE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_HeaterCoolerTargetHeaterCoolerStateWriteCB(void *pp)
{
    static u16 ccc_hHeaterCoolerTargetHeaterCoolerState_pre = 0;

    if(ccc_hHeaterCoolerTargetHeaterCoolerState_pre != ccc_hHeaterCoolerTargetHeaterCoolerState)
    {
        if(ccc_hHeaterCoolerTargetHeaterCoolerState == 0)
        {
            ccc_hHeaterCoolerTargetHeaterCoolerState_pre = ccc_hHeaterCoolerTargetHeaterCoolerState;
            EventNotifyCNT--;
            indicate_handle_no_remove(HEATERCOOLER_HANDLE_NUM_TARGETHEATERCOOLERSTATE);
        }
        else
        {
            ccc_hHeaterCoolerTargetHeaterCoolerState_pre = ccc_hHeaterCoolerTargetHeaterCoolerState;
            EventNotifyCNT++;
            indicate_handle_no_reg(HEATERCOOLER_HANDLE_NUM_TARGETHEATERCOOLERSTATE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_HeaterCoolerRotationSpeedWriteCB(void *pp)
{
    static u16 ccc_hHeaterCoolerRotationSpeed_pre = 0;

    if(ccc_hHeaterCoolerRotationSpeed_pre != ccc_hHeaterCoolerRotationSpeed)
    {
        if(ccc_hHeaterCoolerRotationSpeed == 0)
        {
            ccc_hHeaterCoolerRotationSpeed_pre = ccc_hHeaterCoolerRotationSpeed;
            EventNotifyCNT--;
            indicate_handle_no_remove(HEATERCOOLER_HANDLE_NUM_ROTATIONSPEED);
        }
        else
        {
            ccc_hHeaterCoolerRotationSpeed_pre = ccc_hHeaterCoolerRotationSpeed;
            EventNotifyCNT++;
            indicate_handle_no_reg(HEATERCOOLER_HANDLE_NUM_ROTATIONSPEED);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_HeaterCoolerTemperatureDisplayUnitsWriteCB(void *pp)
{
    static u16 ccc_hHeaterCoolerTemperatureDisplayUnits_pre = 0;

    if(ccc_hHeaterCoolerTemperatureDisplayUnits_pre != ccc_hHeaterCoolerTemperatureDisplayUnits)
    {
        if(ccc_hHeaterCoolerTemperatureDisplayUnits == 0)
        {
            ccc_hHeaterCoolerTemperatureDisplayUnits_pre = ccc_hHeaterCoolerTemperatureDisplayUnits;
            EventNotifyCNT--;
            indicate_handle_no_remove(HEATERCOOLER_HANDLE_NUM_TEMPERATUREDISPLAYUNITS);
        }
        else
        {
            ccc_hHeaterCoolerTemperatureDisplayUnits_pre = ccc_hHeaterCoolerTemperatureDisplayUnits;
            EventNotifyCNT++;
            indicate_handle_no_reg(HEATERCOOLER_HANDLE_NUM_TEMPERATUREDISPLAYUNITS);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_HeaterCoolerSwingModeWriteCB(void *pp)
{
    static u16 ccc_hHeaterCoolerSwingMode_pre = 0;

    if(ccc_hHeaterCoolerSwingMode_pre != ccc_hHeaterCoolerSwingMode)
    {
        if(ccc_hHeaterCoolerSwingMode == 0)
        {
            ccc_hHeaterCoolerSwingMode_pre = ccc_hHeaterCoolerSwingMode;
            EventNotifyCNT--;
            indicate_handle_no_remove(HEATERCOOLER_HANDLE_NUM_SWINGMODE);
        }
        else
        {
            ccc_hHeaterCoolerSwingMode_pre = ccc_hHeaterCoolerSwingMode;
            EventNotifyCNT++;
            indicate_handle_no_reg(HEATERCOOLER_HANDLE_NUM_SWINGMODE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}


int ccc_HeaterCoolerCoolingThresholdTempWriteCB(void *pp)
{
    static u16 ccc_hHeaterCoolerCoolingThresholdTemp_pre = 0;

    if(ccc_hHeaterCoolerCoolingThresholdTemp_pre != ccc_hHeaterCoolerCoolingThresholdTemp)
    {
        if(ccc_hHeaterCoolerCoolingThresholdTemp == 0)
        {
            ccc_hHeaterCoolerCoolingThresholdTemp_pre = ccc_hHeaterCoolerCoolingThresholdTemp;
            EventNotifyCNT--;
            indicate_handle_no_remove(HEATERCOOLER_HANDLE_NUM_COOLINGTHRESHOLDTEMP);
        }
        else
        {
            ccc_hHeaterCoolerCoolingThresholdTemp_pre = ccc_hHeaterCoolerCoolingThresholdTemp;
            EventNotifyCNT++;
            indicate_handle_no_reg(HEATERCOOLER_HANDLE_NUM_COOLINGTHRESHOLDTEMP);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_HeaterCoolerHeatingThresholdTempWriteCB(void *pp)
{
    static u16 ccc_hHeaterCoolerHeatingThresholdTemp_pre = 0;

    if(ccc_hHeaterCoolerHeatingThresholdTemp_pre != ccc_hHeaterCoolerHeatingThresholdTemp)
    {
        if(ccc_hHeaterCoolerHeatingThresholdTemp == 0)
        {
            ccc_hHeaterCoolerHeatingThresholdTemp_pre = ccc_hHeaterCoolerHeatingThresholdTemp;
            EventNotifyCNT--;
            indicate_handle_no_remove(HEATERCOOLER_HANDLE_NUM_HEATINGTHRESHOLDTEMP);
        }
        else
        {
            ccc_hHeaterCoolerHeatingThresholdTemp_pre = ccc_hHeaterCoolerHeatingThresholdTemp;
            EventNotifyCNT++;
            indicate_handle_no_reg(HEATERCOOLER_HANDLE_NUM_HEATINGTHRESHOLDTEMP);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_HeaterCoolerLockPhysicalControlsWriteCB(void *pp)
{
    static u16 ccc_hHeaterCoolerLockPhysicalControls_pre = 0;

    if(ccc_hHeaterCoolerLockPhysicalControls_pre != ccc_hHeaterCoolerLockPhysicalControls)
    {
        if(ccc_hHeaterCoolerLockPhysicalControls == 0)
        {
            ccc_hHeaterCoolerLockPhysicalControls_pre = ccc_hHeaterCoolerLockPhysicalControls;
            EventNotifyCNT--;
            indicate_handle_no_remove(HEATERCOOLER_HANDLE_NUM_LOCKPHYSICALCONTROLS);
        }
        else
        {
            ccc_hHeaterCoolerLockPhysicalControls_pre = ccc_hHeaterCoolerLockPhysicalControls;
            EventNotifyCNT++;
            indicate_handle_no_reg(HEATERCOOLER_HANDLE_NUM_LOCKPHYSICALCONTROLS);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}


/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
