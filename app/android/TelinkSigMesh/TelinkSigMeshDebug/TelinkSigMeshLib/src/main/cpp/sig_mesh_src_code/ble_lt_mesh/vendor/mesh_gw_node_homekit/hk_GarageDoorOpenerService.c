/********************************************************************************************************
 * @file     hk_GarageDoorOpenerService.c
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
#define _HK_GARAGE_DOOR_OPENER_SERVICE_C_


/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "hk_GarageDoorOpenerService.h"
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
unsigned char Current_Door_State = 0;
unsigned char Target_Door_State = 0;
unsigned char Obstruction_Detected = 0;
unsigned char hGarageDoor_LockCurrentState = 0;
unsigned char hGarageDoor_LockTargetState = 0;

unsigned char VAR_GARAGEDOOROPENER_UNKOWN=0;

unsigned short ccc_hCurrent_Door_State = 0;
unsigned short ccc_hTarget_Door_State = 0;
unsigned short ccc_hObstruction_Detected = 0;
unsigned short ccc_hLock_Current_State = 0;
unsigned short ccc_hLock_Target_State = 0;

#if GARAGEDOOR_CURRENTDOORSTATE_BRC_ENABLE
    unsigned char bc_para_hCurrent_Door_State = 1;
#endif

#if GARAGEDOOR_TARGETDOORSTATE_BRC_ENABLE
    unsigned char bc_para_hTarget_Door_State = 1;
#endif

#if GARAGEDOOR_OBSTRUCTIONDETECTED_BRC_ENABLE
    unsigned char bc_para_hObstruction_Detected = 1;
#endif

#if GARAGEDOOR_LOCKCURRENTSTATE_BRC_ENABLE
    unsigned char bc_para_hLock_Current_State = 1;
#endif

#if GARAGEDOOR_LOCKTARGETSTATE_BRC_ENABLE
    unsigned char bc_para_hLock_Target_State = 1;
#endif

const u8 my_gdoServiceName[26] = "Garage Door Opener Service";

const u8 CurrentDoorState_VALID_RANGE[2] = { 0x00, 0x04};
const u8 TargetDoorState_VALID_RANGE[2] = { 0x00, 0x01};
const u8 hGarageDoor_LockCurrentState_VALID_RANGE[2] = { 0x00, 0x03};
const u8 hGarageDoor_LockTargetState_VALID_RANGE[2] = { 0x00, 0x01};

const u8 CurrentDoorState_VALID_STEP = 0x01;
const u8 TargetDoorState_VALID_STEP = 0x01;
const u8 hGarageDoor_LockCurrentState_VALID_STEP = 0x01;
const u8 hGarageDoor_LockTargetState_VALID_STEP = 0x01;

const hap_characteristic_desc_t hap_desc_char_Current_Door_State_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, CurrentDoorState_VALID_RANGE, &CurrentDoorState_VALID_STEP, BC_PARA_GARAGEDOOR_CURRENTDOORSTATE};
const hap_characteristic_desc_t hap_desc_char_Target_Door_State_R_S = {hap_PR_PW_TW_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, TargetDoorState_VALID_RANGE, &TargetDoorState_VALID_STEP, BC_PARA_GARAGEDOOR_TARGETDOORSTATE};
const hap_characteristic_desc_t hap_desc_char_Obstruction_Detected_R_S = {hap_PRead_NOTIFY, 0, CFG_BOOL_UNITLESS, 0, 0, 0, 0, 0, 0, 0, BC_PARA_GARAGEDOOR_OBSTRUCTIONDETECTED};
const hap_characteristic_desc_t hap_desc_char_hGarageDoor_LockCurrentState_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, hGarageDoor_LockCurrentState_VALID_RANGE, &hGarageDoor_LockCurrentState_VALID_STEP, BC_PARA_GARAGEDOOR_LOCKCURRENTSTATE};
const hap_characteristic_desc_t hap_desc_char_hGarageDoor_LockTargetState_R_S = {hap_PR_PW_TW_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, hGarageDoor_LockTargetState_VALID_RANGE, &hGarageDoor_LockTargetState_VALID_STEP, BC_PARA_GARAGEDOOR_LOCKTARGETSTATE};

const hap_characteristic_desc_t hap_desc_char_garageDoor_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if GARAGEDOOROPENER_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * hGarageDoorOpener_linked_svcs[] = {};
#endif

#if(GARAGEDOOROPENER_IS_HIDDEN_SERVICE == 1)
const hap_service_desc_t hap_desc_service_garageDoor_A5_R_S = {GARAGEDOOROPENER_INST_NO+1, (0x02 + GARAGEDOOROPENER_AS_PRIMARY_SERVICE), GARAGEDOOROPENER_LINKED_SVCS_LENGTH, GARAGEDOOROPENER_LINKED_SVCS};
#else
const hap_service_desc_t hap_desc_service_garageDoor_A5_R_S = {GARAGEDOOROPENER_INST_NO+1, (0x00 + GARAGEDOOROPENER_AS_PRIMARY_SERVICE), GARAGEDOOROPENER_LINKED_SVCS_LENGTH, GARAGEDOOROPENER_LINKED_SVCS};
#endif


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
int CurrentDoorStateReadCallback(void *pp)      //
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int TargetDoorStateReadCallback(void *pp)       //
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int TargetDoorStateWriteCallback(void *pp)      //
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~1
        assign_current_flash_char_value_addr();
    #if GARAGEDOOROPENER_SERVICE_ENABLE
        GDO_STORE_CHAR_VALUE_T_GDO_TARGET_DOOR_STATE_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
        Target_Door_State &= 0x01;
        Current_Door_State = Target_Door_State;
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ObstructionDetectedReadCallback(void *pp)       //
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int GarageDoorLockCurrentStateReadCallback(void *pp)      //
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int GarageDoorLockTargetStateReadCallback(void *pp)       //
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int GarageDoorLockTargetStateWriteCallback(void *pp)  //
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~1
        assign_current_flash_char_value_addr();
    #if GARAGEDOOROPENER_SERVICE_ENABLE
        GDO_STORE_CHAR_VALUE_T_GDO_LOCK_TARGET_STATE_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
        hGarageDoor_LockTargetState &= 0x01;
        hGarageDoor_LockCurrentState = hGarageDoor_LockTargetState;
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hCurrent_Door_StateWriteCB(void *pp)
{
    static u16 ccc_hCurrent_Door_State_pre = 0;

    if(ccc_hCurrent_Door_State_pre != ccc_hCurrent_Door_State)
    {
        if(ccc_hCurrent_Door_State == 0)
        {
            ccc_hCurrent_Door_State_pre = ccc_hCurrent_Door_State;
            EventNotifyCNT--;
            indicate_handle_no_remove(GDOPENER_HANDLE_NUM_CURRENTDOORTSTATE);
        }
        else
        {
            ccc_hCurrent_Door_State_pre = ccc_hCurrent_Door_State;
            EventNotifyCNT++;
            indicate_handle_no_reg(GDOPENER_HANDLE_NUM_CURRENTDOORTSTATE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hTarget_Door_StateWriteCB(void *pp)
{
    static u16 ccc_hTarget_Door_State_pre = 0;

    if(ccc_hTarget_Door_State_pre != ccc_hTarget_Door_State)
    {
        if(ccc_hTarget_Door_State == 0)
        {
            ccc_hTarget_Door_State_pre = ccc_hTarget_Door_State;
            EventNotifyCNT--;
            indicate_handle_no_remove(GDOPENER_HANDLE_NUM_TARGETDOORSTATE);
        }
        else
        {
            ccc_hTarget_Door_State_pre = ccc_hTarget_Door_State;
            EventNotifyCNT++;
            indicate_handle_no_reg(GDOPENER_HANDLE_NUM_TARGETDOORSTATE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hObstruction_DetectedWriteCB(void *pp)
{
    static u16 ccc_hObstruction_Detected_pre = 0;

    if(ccc_hObstruction_Detected_pre != ccc_hObstruction_Detected)
    {
        if(ccc_hObstruction_Detected == 0)
        {
            ccc_hObstruction_Detected_pre = ccc_hObstruction_Detected;
            EventNotifyCNT--;
            indicate_handle_no_remove(GDOPENER_HANDLE_NUM_OBSTRUCTIONDETECTED);
        }
        else
        {
            ccc_hObstruction_Detected_pre = ccc_hObstruction_Detected;
            EventNotifyCNT++;
            indicate_handle_no_reg(GDOPENER_HANDLE_NUM_OBSTRUCTIONDETECTED);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hLock_Current_StateWriteCB(void *pp)
{
    static u16 ccc_hLock_Current_State_pre = 0;

    if(ccc_hLock_Current_State_pre != ccc_hLock_Current_State)
    {
        if(ccc_hLock_Current_State == 0)
        {
            ccc_hLock_Current_State_pre = ccc_hLock_Current_State;
            EventNotifyCNT--;
            indicate_handle_no_remove(GDOPENER_HANDLE_NUM_LOCKCURRENTSTATE);
        }
        else
        {
            ccc_hLock_Current_State_pre = ccc_hLock_Current_State;
            EventNotifyCNT++;
            indicate_handle_no_reg(GDOPENER_HANDLE_NUM_LOCKCURRENTSTATE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hLock_Target_StateWriteCB(void *pp)
{
    static u16 ccc_hLock_Target_State_pre = 0;

    if(ccc_hLock_Target_State_pre != ccc_hLock_Target_State)
    {
        if(ccc_hLock_Target_State == 0)
        {
            ccc_hLock_Target_State_pre = ccc_hLock_Target_State;
            EventNotifyCNT--;
            indicate_handle_no_remove(GDOPENER_HANDLE_NUM_LOCKTARGETSTATE);
        }
        else
        {
            ccc_hLock_Target_State_pre = ccc_hLock_Target_State;
            EventNotifyCNT++;
            indicate_handle_no_reg(GDOPENER_HANDLE_NUM_LOCKTARGETSTATE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
