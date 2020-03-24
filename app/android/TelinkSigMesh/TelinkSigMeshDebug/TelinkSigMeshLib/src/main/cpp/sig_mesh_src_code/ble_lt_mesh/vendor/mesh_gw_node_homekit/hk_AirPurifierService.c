/********************************************************************************************************
 * @file     hk_AirPurifierService.c
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
#define _HK_AIR_PURIFIER_SERVICE_C_
 
 
/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "hk_AirPurifierService.h"
#include "ble_UUID.h"
#include "hk_TopUUID.h"
#include "homekit_src/hk_indicate_queue.h"

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
unsigned char hAirPurifierActive = 0;
unsigned char hCurrentAirPurifierState = 0;
unsigned char hTargetAirPurifierState = 0;
int hAirPurifierRotationSpeed = 0;
unsigned char hAirPurifierSwingMode = 0;
unsigned char hAirPurifierLockPhysicalControls = 0;

unsigned char VAR_AIR_PURIFIER_UNKOWN=0;

unsigned short ccc_hAirPurifierActive = 0;
unsigned short ccc_hCurrentAirPurifierState = 0;
unsigned short ccc_hTargetAirPurifierState = 0;
unsigned short ccc_hAirPurifierRotationSpeed = 0;
unsigned short ccc_hAirPurifierSwingMode = 0;
unsigned short ccc_hAirPurifierLockPhysicalControls = 0;

#if AIRPURIFIER_ACTIVE_BRC_ENABLE
    unsigned char bc_para_hAirPurifierActive = 1;
#endif

#if AIRPURIFIER_CURRENT_AIRPURIFIER_STATE_BRC_ENABLE
    unsigned char bc_para_hCurrentAirPurifierState = 1;
#endif

#if AIRPURIFIER_TARGET_AIRPURIFIER_STATE_BRC_ENABLE
    unsigned char bc_para_hTargetAirPurifierState = 1;
#endif

#if AIRPURIFIER_ROTATION_SPEED_BRC_ENABLE
    unsigned char bc_para_hAirPurifierRotationSpeed = 1;
#endif

#if AIRPURIFIER_SWING_MODE_BRC_ENABLE
    unsigned char bc_para_hAirPurifierSwingMode = 1;
#endif

#if AIRPURIFIER_LOCK_PHYSICAL_CONTROLS_BRC_ENABLE
    unsigned char bc_para_hAirPurifierLockPhysicalControls = 1;
#endif

const u8 hAirPurifierServiceName[18] = "AirPurifierService";

const u8 AIRPURIFIER_ACTIVE_VALID_RANGE[2] = {0x00, 0x01};
const u8 CURRENT_AIRPURIFIER_STATE_VALID_RANGE[2] = {0x00, 0x02};
const u8 TARGET_AIRPURIFIER_STATE_VALID_RANGE[2] = {0x00, 0x01};
const u8 AIRPURIFIER_ROTATION_SPEED_VALID_RANGE[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC8, 0x42};
const u8 AIRPURIFIER_SWING_MODE_VALID_RANGE[2] = {0x00, 0x01};
const u8 AIRPURIFIER_LOCK_PHYSICAL_CONTROLS_VALID_RANGE[2] = {0x00, 0x01};

const u8 AIRPURIFIER_ACTIVE_VALID_STEP = 0x01;
const u8 CURRENT_AIRPURIFIER_STATE_VALID_STEP = 0x01;
const u8 TARGET_AIRPURIFIER_STATE_VALID_STEP = 0x01;
const u8 AIRPURIFIER_ROTATION_SPEED_VALID_STEP[4] = { 0x00, 0x00, 0x80, 0x3F};
const u8 AIRPURIFIER_SWING_MODE_VALID_STEP = 0x01;
const u8 AIRPURIFIER_LOCK_PHYSICAL_CONTROLS_VALID_STEP = 0x01;

const hap_characteristic_desc_t hap_desc_char_AirPurifier_Active_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, AIRPURIFIER_ACTIVE_VALID_RANGE, &AIRPURIFIER_ACTIVE_VALID_STEP, BC_PARA_AIRPURIFIER_ACTIVE};
const hap_characteristic_desc_t hap_desc_char_AirPurifier_CurrentAirPurifierState_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, CURRENT_AIRPURIFIER_STATE_VALID_RANGE, &CURRENT_AIRPURIFIER_STATE_VALID_STEP, BC_PARA_AIRPURIFIER_CURRENT_AIRPURIFIER_STATE};
const hap_characteristic_desc_t hap_desc_char_AirPurifier_TargetAirPurifierState_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, TARGET_AIRPURIFIER_STATE_VALID_RANGE, &TARGET_AIRPURIFIER_STATE_VALID_STEP, BC_PARA_AIRPURIFIER_TARGET_AIRPURIFIER_STATE};
const hap_characteristic_desc_t hap_desc_char_AirPurifier_RotationSpeed_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_FLOAT32_PERCENTAGE, 0, 8, 4, 0, 0, AIRPURIFIER_ROTATION_SPEED_VALID_RANGE, AIRPURIFIER_ROTATION_SPEED_VALID_STEP, BC_PARA_AIRPURIFIER_ROTATION_SPEED};
const hap_characteristic_desc_t hap_desc_char_AirPurifier_SwingMode_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, AIRPURIFIER_SWING_MODE_VALID_RANGE, &AIRPURIFIER_SWING_MODE_VALID_STEP, BC_PARA_AIRPURIFIER_SWING_MODE};
const hap_characteristic_desc_t hap_desc_char_AirPurifier_LockPhysicalControls_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, AIRPURIFIER_LOCK_PHYSICAL_CONTROLS_VALID_RANGE, &AIRPURIFIER_LOCK_PHYSICAL_CONTROLS_VALID_STEP, BC_PARA_AIRPURIFIER_LOCK_PHYSICAL_CONTROLS};

const hap_characteristic_desc_t hap_desc_char_air_purifier_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if AIR_PURIFIER_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * hAirPurifier_linked_svcs[] = {};
#endif

#if(AIR_PURIFIER_IS_HIDDEN_SERVICE == 1)
const hap_service_desc_t hap_desc_service_air_purifier_A5_R_S = {AIR_PURIFIER_INST_NO+1, (0x02 + AIR_PURIFIER_AS_PRIMARY_SERVICE), AIR_PURIFIER_LINKED_SVCS_LENGTH, AIR_PURIFIER_LINKED_SVCS};
#else
const hap_service_desc_t hap_desc_service_air_purifier_A5_R_S = {AIR_PURIFIER_INST_NO+1, (0x00 + AIR_PURIFIER_AS_PRIMARY_SERVICE), AIR_PURIFIER_LINKED_SVCS_LENGTH, AIR_PURIFIER_LINKED_SVCS};
#endif

/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
int AirPurifierActiveReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int AirPurifierActiveWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~1
        assign_current_flash_char_value_addr();
    #if AIR_PURIFIER_SERVICE_ENABLE
        AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_ACTIVE_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int CurrentAirPurifierStateReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int TargetAirPurifierStateReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int TargetAirPurifierStateWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~1
        assign_current_flash_char_value_addr();
    #if AIR_PURIFIER_SERVICE_ENABLE
        AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_TARGET_AIRPURIFIER_STATE_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int AirPurifierRotationSpeedReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int AirPurifierRotationSpeedWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~100
        assign_current_flash_char_value_addr();
    #if AIR_PURIFIER_SERVICE_ENABLE
        AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_ROTATION_SPEED_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int AirPurifierSwingModeReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int AirPurifierSwingModeWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~1
        assign_current_flash_char_value_addr();
    #if AIR_PURIFIER_SERVICE_ENABLE
        AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_SWING_MODE_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int AirPurifierLockPhysicalControlsReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int AirPurifierLockPhysicalControlsWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~1
        assign_current_flash_char_value_addr();
    #if AIR_PURIFIER_SERVICE_ENABLE
        AIR_PURIFIER_STORE_CHAR_VALUE_T_HAIR_PURIFIER_LOCK_PHYSICAL_CONTROLS_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}


int ccc_hAirPurifierActiveWriteCB(void *pp)
{
    static u16 ccc_hAirPurifierActive_pre = 0;

    if(ccc_hAirPurifierActive_pre != ccc_hAirPurifierActive)
    {
        if(ccc_hAirPurifierActive == 0)
        {
            ccc_hAirPurifierActive_pre = ccc_hAirPurifierActive;
            EventNotifyCNT--;
            indicate_handle_no_remove(AIR_PURIFIER_HANDLE_NUM_HAIR_PURIFIER_ACTIVE);
        }
        else
        {
            ccc_hAirPurifierActive_pre = ccc_hAirPurifierActive;
            EventNotifyCNT++;
            indicate_handle_no_reg(AIR_PURIFIER_HANDLE_NUM_HAIR_PURIFIER_ACTIVE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hCurrentAirPurifierStateWriteCB(void *pp)
{
    static u16 ccc_hCurrentAirPurifierState_pre = 0;

    if(ccc_hCurrentAirPurifierState_pre != ccc_hCurrentAirPurifierState)
    {
        if(ccc_hCurrentAirPurifierState == 0)
        {
            ccc_hCurrentAirPurifierState_pre = ccc_hCurrentAirPurifierState;
            EventNotifyCNT--;
            indicate_handle_no_remove(AIR_PURIFIER_HANDLE_NUM_HAIR_PURIFIER_CURRENT_AIRPURIFIER_STATE);
        }
        else
        {
            ccc_hCurrentAirPurifierState_pre = ccc_hCurrentAirPurifierState;
            EventNotifyCNT++;
            indicate_handle_no_reg(AIR_PURIFIER_HANDLE_NUM_HAIR_PURIFIER_CURRENT_AIRPURIFIER_STATE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hTargetAirPurifierStateWriteCB(void *pp)
{
    static u16 ccc_hTargetAirPurifierState_pre = 0;

    if(ccc_hTargetAirPurifierState_pre != ccc_hTargetAirPurifierState)
    {
        if(ccc_hTargetAirPurifierState == 0)
        {
            ccc_hTargetAirPurifierState_pre = ccc_hTargetAirPurifierState;
            EventNotifyCNT--;
            indicate_handle_no_remove(AIR_PURIFIER_HANDLE_NUM_HAIR_PURIFIER_TARGET_AIRPURIFIER_STATE);
        }
        else
        {
            ccc_hTargetAirPurifierState_pre = ccc_hTargetAirPurifierState;
            EventNotifyCNT++;
            indicate_handle_no_reg(AIR_PURIFIER_HANDLE_NUM_HAIR_PURIFIER_TARGET_AIRPURIFIER_STATE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hAirPurifierRotationSpeedWriteCB(void *pp)
{
    static u16 ccc_hAirPurifierRotationSpeed_pre = 0;

    if(ccc_hAirPurifierRotationSpeed_pre != ccc_hAirPurifierRotationSpeed)
    {
        if(ccc_hAirPurifierRotationSpeed == 0)
        {
            ccc_hAirPurifierRotationSpeed_pre = ccc_hAirPurifierRotationSpeed;
            EventNotifyCNT--;
            indicate_handle_no_remove(AIR_PURIFIER_HANDLE_NUM_HAIR_PURIFIER_ROTATIONSPEED);
        }
        else
        {
            ccc_hAirPurifierRotationSpeed_pre = ccc_hAirPurifierRotationSpeed;
            EventNotifyCNT++;
            indicate_handle_no_reg(AIR_PURIFIER_HANDLE_NUM_HAIR_PURIFIER_ROTATIONSPEED);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hAirPurifierSwingModeWriteCB(void *pp)
{
    static u16 ccc_hAirPurifierSwingMode_pre = 0;

    if(ccc_hAirPurifierSwingMode_pre != ccc_hAirPurifierSwingMode)
    {
        if(ccc_hAirPurifierSwingMode == 0)
        {
            ccc_hAirPurifierSwingMode_pre = ccc_hAirPurifierSwingMode;
            EventNotifyCNT--;
            indicate_handle_no_remove(AIR_PURIFIER_HANDLE_NUM_HAIR_PURIFIER_SWINGMODE);
        }
        else
        {
            ccc_hAirPurifierSwingMode_pre = ccc_hAirPurifierSwingMode;
            EventNotifyCNT++;
            indicate_handle_no_reg(AIR_PURIFIER_HANDLE_NUM_HAIR_PURIFIER_SWINGMODE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hAirPurifierLockPhysicalControlsWriteCB(void *pp)
{
    static u16 ccc_hAirPurifierLockPhysicalControls_pre = 0;

    if(ccc_hAirPurifierLockPhysicalControls_pre != ccc_hAirPurifierLockPhysicalControls)
    {
        if(ccc_hAirPurifierLockPhysicalControls == 0)
        {
            ccc_hAirPurifierLockPhysicalControls_pre = ccc_hAirPurifierLockPhysicalControls;
            EventNotifyCNT--;
            indicate_handle_no_remove(AIR_PURIFIER_HANDLE_NUM_HAIR_PURIFIER_LOCKPHYSICALCONTROLS);
        }
        else
        {
            ccc_hAirPurifierLockPhysicalControls_pre = ccc_hAirPurifierLockPhysicalControls;
            EventNotifyCNT++;
            indicate_handle_no_reg(AIR_PURIFIER_HANDLE_NUM_HAIR_PURIFIER_LOCKPHYSICALCONTROLS);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
