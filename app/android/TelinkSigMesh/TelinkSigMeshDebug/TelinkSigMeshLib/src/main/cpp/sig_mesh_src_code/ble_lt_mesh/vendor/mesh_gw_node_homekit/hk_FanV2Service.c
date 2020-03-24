/********************************************************************************************************
 * @file     hk_FanV2Service.c
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
#define _HK_FAN_V2_SERVICE_C_
 
 
/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "hk_FanV2Service.h"
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
unsigned char hFanV2Active = 0;
unsigned char hFanV2CurrentFanState = 0;
unsigned char hFanV2TargetFanState = 0;
int hFanV2RotationDirection = 0;
int hFanV2RotationSpeed = 0;
unsigned char hFanV2SwingMode = 0;
unsigned char hFanV2LockPhysicalControls = 0;

unsigned char VAR_FAN_V2_UNKOWN=0;

unsigned short ccc_hFanV2Active = 0;
unsigned short ccc_hFanV2CurrentFanState = 0;
unsigned short ccc_hFanV2TargetFanState = 0;
unsigned short ccc_hFanV2RotationDirection = 0;
unsigned short ccc_hFanV2RotationSpeed = 0;
unsigned short ccc_hFanV2SwingMode = 0;
unsigned short ccc_hFanV2LockPhysicalControls = 0;

#if FAN_V2_ACTIVE_BRC_ENABLE
    unsigned char bc_para_hFanV2Active = 1;
#endif

#if FAN_V2_CURRENT_FAN_STATE_BRC_ENABLE
    unsigned char bc_para_hFanV2CurrentFanState = 1;
#endif

#if FAN_V2_TARGET_FAN_STATE_BRC_ENABLE
    unsigned char bc_para_hFanV2TargetFanState = 1;
#endif

#if FAN_V2_ROTATION_DIRECTION_BRC_ENABLE
    unsigned char bc_para_hFanV2RotationDirection = 1;
#endif

#if FAN_V2_ROTATION_SPEED_BRC_ENABLE
    unsigned char bc_para_hFanV2RotationSpeed = 1;
#endif

#if FAN_V2_SWING_MODE_BRC_ENABLE
    unsigned char bc_para_hFanV2SwingMode = 1;
#endif

#if FAN_V2_LOCK_PHYSICAL_CONTROLS_BRC_ENABLE
    unsigned char bc_para_hFanV2LockPhysicalControls = 1;
#endif


const u8 hFanV2ServiceName[12] = "FanV2Service";

const u8 FANV2_ACTIVE_VALID_RANGE[2] = {0x00, 0x01};
const u8 FANV2_CURRENTFANSTATE_VALID_RANGE[2] = {0x00, 0x02};
const u8 FANV2_TARGETFANSTATE_VALID_RANGE[2] = {0x00, 0x01};
const u8 FANV2_ROTATION_DIRECTION_VALID_RANGE[8] = { 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
const u8 FANV2_ROTATION_SPEED_VALID_RANGE[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC8, 0x42};
const u8 FANV2_SWING_MODE_VALID_RANGE[2] = {0x00, 0x01};
const u8 FANV2_LOCK_PHYSICAL_CONTROLS_VALID_RANGE[2] = {0x00, 0x01};

const u8 FANV2_ACTIVE_VALID_STEP = 0x01;
const u8 FANV2_CURRENTFANSTATE_VALID_STEP = 0x01;
const u8 FANV2_TARGETFANSTATE_VALID_STEP = 0x01;
const u8 FANV2_ROTATION_DIRECTION_VALID_STEP[4] = { 0x01, 0x00, 0x00, 0x00};
const u8 FANV2_ROTATION_SPEED_VALID_STEP[4] = { 0x00, 0x00, 0x80, 0x3F};
const u8 FANV2_SWING_MODE_VALID_STEP = 0x01;
const u8 FANV2_LOCK_PHYSICAL_CONTROLS_VALID_STEP = 0x01;

const hap_characteristic_desc_t hap_desc_char_FanV2_Active_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, FANV2_ACTIVE_VALID_RANGE, &FANV2_ACTIVE_VALID_STEP, BC_PARA_FAN_V2_ACTIVE};
const hap_characteristic_desc_t hap_desc_char_FanV2_CurrentFanState_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, FANV2_CURRENTFANSTATE_VALID_RANGE, &FANV2_CURRENTFANSTATE_VALID_STEP, BC_PARA_FAN_V2_CURRENT_FAN_STATE};
const hap_characteristic_desc_t hap_desc_char_FanV2_TargetFanState_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, FANV2_TARGETFANSTATE_VALID_RANGE, &FANV2_TARGETFANSTATE_VALID_STEP, BC_PARA_FAN_V2_TARGET_FAN_STATE};
const hap_characteristic_desc_t hap_desc_char_FanV2_RotationDirection_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_INT32, 0, 8, 4, 0, 0, FANV2_ROTATION_DIRECTION_VALID_RANGE, FANV2_ROTATION_DIRECTION_VALID_STEP, BC_PARA_FAN_V2_ROTATION_DIRECTION};
const hap_characteristic_desc_t hap_desc_char_FanV2_RotationSpeed_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_FLOAT32_PERCENTAGE, 0, 8, 4, 0, 0, FANV2_ROTATION_SPEED_VALID_RANGE, FANV2_ROTATION_SPEED_VALID_STEP, BC_PARA_FAN_V2_ROTATION_SPEED};
const hap_characteristic_desc_t hap_desc_char_FanV2_SwingMode_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, FANV2_SWING_MODE_VALID_RANGE, &FANV2_SWING_MODE_VALID_STEP, BC_PARA_FAN_V2_SWING_MODE};
const hap_characteristic_desc_t hap_desc_char_FanV2_LockPhysicalControls_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, FANV2_LOCK_PHYSICAL_CONTROLS_VALID_RANGE, &FANV2_LOCK_PHYSICAL_CONTROLS_VALID_STEP, BC_PARA_FAN_V2_LOCK_PHYSICAL_CONTROLS};

const hap_characteristic_desc_t hap_desc_char_fan_v2_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if FAN_V2_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * hFanV2_linked_svcs[] = {};
#endif

#if(FAN_V2_IS_HIDDEN_SERVICE == 1)
const hap_service_desc_t hap_desc_service_fan_v2_A5_R_S = {FAN_V2_INST_NO+1, (0x02 + FAN_V2_AS_PRIMARY_SERVICE), FAN_V2_LINKED_SVCS_LENGTH, FAN_V2_LINKED_SVCS};
#else
const hap_service_desc_t hap_desc_service_fan_v2_A5_R_S = {FAN_V2_INST_NO+1, (0x00 + FAN_V2_AS_PRIMARY_SERVICE), FAN_V2_LINKED_SVCS_LENGTH, FAN_V2_LINKED_SVCS};
#endif

/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
int FanV2ActiveReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int FanV2ActiveWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~1
        assign_current_flash_char_value_addr();
    #if FAN_V2_SERVICE_ENABLE
        FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_ACTIVE_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int FanV2CurrentFanStateReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int FanV2TargetFanStateReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int FanV2TargetFanStateWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~1
        assign_current_flash_char_value_addr();
    #if FAN_V2_SERVICE_ENABLE
        FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_TARGETFANSTATE_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int FanV2RotationDirectionReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int FanV2RotationDirectionWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~1
        assign_current_flash_char_value_addr();
    #if FAN_V2_SERVICE_ENABLE
        FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_ROTATIONDIRECTION_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int FanV2RotationSpeedReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int FanV2RotationSpeedWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~100
        assign_current_flash_char_value_addr();
    #if FAN_V2_SERVICE_ENABLE
        FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_ROTATIONSPEED_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int FanV2SwingModeReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int FanV2SwingModeWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~1
        assign_current_flash_char_value_addr();
    #if FAN_V2_SERVICE_ENABLE
        FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_SWINGMODE_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int FanV2LockPhysicalControlsReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int FanV2LockPhysicalControlsWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~1
        assign_current_flash_char_value_addr();
    #if FAN_V2_SERVICE_ENABLE
        FAN_V2_STORE_CHAR_VALUE_T_HFAN_V2_LOCKPHYSICALCONTROLS_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}


int ccc_hFanV2ActiveWriteCB(void *pp)
{
    static u16 ccc_hFanV2Active_pre = 0;

    if(ccc_hFanV2Active_pre != ccc_hFanV2Active)
    {
        if(ccc_hFanV2Active == 0)
        {
            ccc_hFanV2Active_pre = ccc_hFanV2Active;
            EventNotifyCNT--;
            indicate_handle_no_remove(FAN_V2_HANDLE_NUM_HFAN_V2_ACTIVE);
        }
        else
        {
            ccc_hFanV2Active_pre = ccc_hFanV2Active;
            EventNotifyCNT++;
            indicate_handle_no_reg(FAN_V2_HANDLE_NUM_HFAN_V2_ACTIVE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hFanV2CurrentFanStateWriteCB(void *pp)
{
    static u16 ccc_hFanV2CurrentFanState_pre = 0;

    if(ccc_hFanV2CurrentFanState_pre != ccc_hFanV2CurrentFanState)
    {
        if(ccc_hFanV2CurrentFanState == 0)
        {
            ccc_hFanV2CurrentFanState_pre = ccc_hFanV2CurrentFanState;
            EventNotifyCNT--;
            indicate_handle_no_remove(FAN_V2_HANDLE_NUM_HFAN_V2_CURRENTFANSTATE);
        }
        else
        {
            ccc_hFanV2CurrentFanState_pre = ccc_hFanV2CurrentFanState;
            EventNotifyCNT++;
            indicate_handle_no_reg(FAN_V2_HANDLE_NUM_HFAN_V2_CURRENTFANSTATE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hFanV2TargetFanStateWriteCB(void *pp)
{
    static u16 ccc_hFanV2TargetFanState_pre = 0;

    if(ccc_hFanV2TargetFanState_pre != ccc_hFanV2TargetFanState)
    {
        if(ccc_hFanV2TargetFanState == 0)
        {
            ccc_hFanV2TargetFanState_pre = ccc_hFanV2TargetFanState;
            EventNotifyCNT--;
            indicate_handle_no_remove(FAN_V2_HANDLE_NUM_HFAN_V2_TARGETFANSTATE);
        }
        else
        {
            ccc_hFanV2TargetFanState_pre = ccc_hFanV2TargetFanState;
            EventNotifyCNT++;
            indicate_handle_no_reg(FAN_V2_HANDLE_NUM_HFAN_V2_TARGETFANSTATE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hFanV2RotationDirectionWriteCB(void *pp)
{
    static u16 ccc_hFanV2RotationDirection_pre = 0;

    if(ccc_hFanV2RotationDirection_pre != ccc_hFanV2RotationDirection)
    {
        if(ccc_hFanV2RotationDirection == 0)
        {
            ccc_hFanV2RotationDirection_pre = ccc_hFanV2RotationDirection;
            EventNotifyCNT--;
            indicate_handle_no_remove(FAN_V2_HANDLE_NUM_HFAN_V2_ROTATIONDIRECTION);
        }
        else
        {
            ccc_hFanV2RotationDirection_pre = ccc_hFanV2RotationDirection;
            EventNotifyCNT++;
            indicate_handle_no_reg(FAN_V2_HANDLE_NUM_HFAN_V2_ROTATIONDIRECTION);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hFanV2RotationSpeedWriteCB(void *pp)
{
    static u16 ccc_hFanV2RotationSpeed_pre = 0;

    if(ccc_hFanV2RotationSpeed_pre != ccc_hFanV2RotationSpeed)
    {
        if(ccc_hFanV2RotationSpeed == 0)
        {
            ccc_hFanV2RotationSpeed_pre = ccc_hFanV2RotationSpeed;
            EventNotifyCNT--;
            indicate_handle_no_remove(FAN_V2_HANDLE_NUM_HFAN_V2_ROTATIONSPEED);
        }
        else
        {
            ccc_hFanV2RotationSpeed_pre = ccc_hFanV2RotationSpeed;
            EventNotifyCNT++;
            indicate_handle_no_reg(FAN_V2_HANDLE_NUM_HFAN_V2_ROTATIONSPEED);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hFanV2SwingModeWriteCB(void *pp)
{
    static u16 ccc_hFanV2SwingMode_pre = 0;

    if(ccc_hFanV2SwingMode_pre != ccc_hFanV2SwingMode)
    {
        if(ccc_hFanV2SwingMode == 0)
        {
            ccc_hFanV2SwingMode_pre = ccc_hFanV2SwingMode;
            EventNotifyCNT--;
            indicate_handle_no_remove(FAN_V2_HANDLE_NUM_HFAN_V2_SWINGMODE);
        }
        else
        {
            ccc_hFanV2SwingMode_pre = ccc_hFanV2SwingMode;
            EventNotifyCNT++;
            indicate_handle_no_reg(FAN_V2_HANDLE_NUM_HFAN_V2_SWINGMODE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hFanV2LockPhysicalControlsWriteCB(void *pp)
{
    static u16 ccc_hFanV2LockPhysicalControls_pre = 0;

    if(ccc_hFanV2LockPhysicalControls_pre != ccc_hFanV2LockPhysicalControls)
    {
        if(ccc_hFanV2LockPhysicalControls == 0)
        {
            ccc_hFanV2LockPhysicalControls_pre = ccc_hFanV2LockPhysicalControls;
            EventNotifyCNT--;
            indicate_handle_no_remove(FAN_V2_HANDLE_NUM_HFAN_V2_LOCKPHYSICALCONTROLS);
        }
        else
        {
            ccc_hFanV2LockPhysicalControls_pre = ccc_hFanV2LockPhysicalControls;
            EventNotifyCNT++;
            indicate_handle_no_reg(FAN_V2_HANDLE_NUM_HFAN_V2_LOCKPHYSICALCONTROLS);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}


/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
