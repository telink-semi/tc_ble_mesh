/********************************************************************************************************
 * @file     hk_DoorService.c
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
#define _HK_DOOR_SERVICE_C_
 
 
/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "hk_DoorService.h"
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
unsigned char hDoorCurrentPosition      = 0;    // 1
unsigned char hDoorTargetPosition       = 0;    // 2
unsigned char hDoorPositionState        = 0;    // 3
unsigned char hDoorHoldPosition         = 0;    // 5
unsigned char hDoorObstructionDetected  = 0;    // 6
unsigned char VAR_DOOR_UNKOWN=0;

unsigned short ccc_hDoorCurrentPosition = 0;
unsigned short ccc_hDoorTargetPosition = 0;
unsigned short ccc_hDoorPositionState = 0;
unsigned short ccc_hDoorObstructionDetected = 0;

#if DOOR_CURRENTPOSITION_BRC_ENABLE
    unsigned char bc_para_hDoorCurrentPosition = 1;
#endif

#if DOOR_TARGETPOSITION_BRC_ENABLE
    unsigned char bc_para_hDoorTargetPosition = 1;
#endif

#if DOOR_POSITIONSTATE_BRC_ENABLE
    unsigned char bc_para_hDoorPositionState = 1;
#endif

#if DOOR_OBSTRUCTIONDETECTED_BRC_ENABLE
    unsigned char bc_para_hDoorObstructionDetected = 1;
#endif

const u8 hDoorServiceName[11] = "DoorService";

const u8 VALID_RANGE_hDoorCurrentPosition[2]    = { 0x00, 0x64};
const u8 VALID_RANGE_hDoorTargetPosition[2]     = { 0x00, 0x64};
const u8 VALID_RANGE_hDoorPositionState[2]      = { 0x00, 0x02};

const u8 VALID_STEP_hDoorCurrentPosition        = 0x01;
const u8 VALID_STEP_hDoorTargetPosition         = 0x01;
const u8 VALID_STEP_hDoorPositionState          = 0x01;

const hap_characteristic_desc_t hap_desc_char_hDoorCurrentPosition_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8_PERCENTAGE, 0, 2, 1, 0, 0, VALID_RANGE_hDoorCurrentPosition, &VALID_STEP_hDoorCurrentPosition, BC_PARA_DOOR_CURRENTPOSITION};
const hap_characteristic_desc_t hap_desc_char_hDoorTargetPosition_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_UINT8_PERCENTAGE, 0, 2, 1, 0, 0, VALID_RANGE_hDoorTargetPosition, &VALID_STEP_hDoorTargetPosition, BC_PARA_DOOR_TARGETPOSITION};
const hap_characteristic_desc_t hap_desc_char_hDoorPositionState_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hDoorPositionState, &VALID_STEP_hDoorPositionState, BC_PARA_DOOR_POSITIONSTATE};
const hap_characteristic_desc_t hap_desc_char_hDoorHoldPosition_R_S = {hap_SecureW_20, 0, CFG_BOOL_UNITLESS, 0, 0, 0, 0, 0, 0, 0, 0};
const hap_characteristic_desc_t hap_desc_char_hDoorObstructionDetected_R_S = {hap_PRead_NOTIFY, 0, CFG_BOOL_UNITLESS, 0, 0, 0, 0, 0, 0, 0, BC_PARA_DOOR_OBSTRUCTIONDETECTED};

const hap_characteristic_desc_t hap_desc_char_door_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if DOOR_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * hDoor_linked_svcs[] = {};
#endif

#if(DOOR_IS_HIDDEN_SERVICE == 1)
const hap_service_desc_t hap_desc_service_door_A5_R_S = {DOOR_INST_NO+1, (0x02 + DOOR_AS_PRIMARY_SERVICE), DOOR_LINKED_SVCS_LENGTH, DOOR_LINKED_SVCS};
#else
const hap_service_desc_t hap_desc_service_door_A5_R_S = {DOOR_INST_NO+1, (0x00 + DOOR_AS_PRIMARY_SERVICE), DOOR_LINKED_SVCS_LENGTH, DOOR_LINKED_SVCS};
#endif


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
int hDoorCurrentPositionReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hDoorTargetPositionReadCallback(void *pp)
{
    hDoorCurrentPosition = hDoorTargetPosition; // update to Current state

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hDoorTargetPositionWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~100
        assign_current_flash_char_value_addr();
    #if DOOR_SERVICE_ENABLE
        DOOR_STORE_CHAR_VALUE_T_DOORTARGETPOSITION_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
        hDoorCurrentPosition = hDoorTargetPosition; // update to Current state
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hDoorPositionStateReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hDoorHoldPositionWriteCallback(void *pp)
{
    //blt_adv_set_global_state ();	//this char do not support Notify, so no need update the GSN.

    if (hap_char_boundary_check()) {  // 0~1
        assign_current_flash_char_value_addr();
    #if DOOR_SERVICE_ENABLE
        DOOR_STORE_CHAR_VALUE_T_DOORHOLDPOSITION_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hDoorObstructionDetectedReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hDoorCurrentPositionWriteCB(void *pp)
{
    static u16 ccc_hDoorCurrentPosition_pre = 0;

    if(ccc_hDoorCurrentPosition_pre != ccc_hDoorCurrentPosition)
    {
        if(ccc_hDoorCurrentPosition == 0)
        {
            ccc_hDoorCurrentPosition_pre = ccc_hDoorCurrentPosition;
            EventNotifyCNT--;
            indicate_handle_no_remove(DOOR_HANDLE_NUM_CURRENTPOSITION);
        }
        else
        {
            ccc_hDoorCurrentPosition_pre = ccc_hDoorCurrentPosition;
            EventNotifyCNT++;
            indicate_handle_no_reg(DOOR_HANDLE_NUM_CURRENTPOSITION);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hDoorTargetPositionWriteCB(void *pp)
{
    static u16 ccc_hDoorTargetPosition_pre = 0;

    if(ccc_hDoorTargetPosition_pre != ccc_hDoorTargetPosition)
    {
        if(ccc_hDoorTargetPosition == 0)
        {
            ccc_hDoorTargetPosition_pre = ccc_hDoorTargetPosition;
            EventNotifyCNT--;
            indicate_handle_no_remove(DOOR_HANDLE_NUM_TARGETPOSITION);
        }
        else
        {
            ccc_hDoorTargetPosition_pre = ccc_hDoorTargetPosition;
            EventNotifyCNT++;
            indicate_handle_no_reg(DOOR_HANDLE_NUM_TARGETPOSITION);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hDoorPositionStateWriteCB(void *pp)
{
    static u16 ccc_hDoorPositionState_pre = 0;

    if(ccc_hDoorPositionState_pre != ccc_hDoorPositionState)
    {
        if(ccc_hDoorPositionState == 0)
        {
            ccc_hDoorPositionState_pre = ccc_hDoorPositionState;
            EventNotifyCNT--;
            indicate_handle_no_remove(DOOR_HANDLE_NUM_POSITIONSTATE);
        }
        else
        {
            ccc_hDoorPositionState_pre = ccc_hDoorPositionState;
            EventNotifyCNT++;
            indicate_handle_no_reg(DOOR_HANDLE_NUM_POSITIONSTATE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hDoorObstructionDetectedWriteCB(void *pp)
{
    static u16 ccc_hDoorObstructionDetected_pre = 0;

    if(ccc_hDoorObstructionDetected_pre != ccc_hDoorObstructionDetected)
    {
        if(ccc_hDoorObstructionDetected == 0)
        {
            ccc_hDoorObstructionDetected_pre = ccc_hDoorObstructionDetected;
            EventNotifyCNT--;
            indicate_handle_no_remove(DOOR_HANDLE_NUM_OBSTRUCTIONDETECTED);
        }
        else
        {
            ccc_hDoorObstructionDetected_pre = ccc_hDoorObstructionDetected;
            EventNotifyCNT++;
            indicate_handle_no_reg(DOOR_HANDLE_NUM_OBSTRUCTIONDETECTED);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
