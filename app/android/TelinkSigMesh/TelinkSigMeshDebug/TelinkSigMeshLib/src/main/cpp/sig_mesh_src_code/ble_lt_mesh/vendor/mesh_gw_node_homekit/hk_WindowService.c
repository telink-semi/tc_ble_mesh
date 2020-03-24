/********************************************************************************************************
 * @file     hk_WindowService.c
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
#define _HK_WINDOW_SERVICE_C_
 
 
/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "hk_WindowService.h"
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
unsigned char hWindowCurrentPosition = 0;       // 1
unsigned char hWindowTargetPosition = 0;        // 2
unsigned char hWindowPositionState = 0;         // 3
unsigned char hWindowHoldPosition = 0;          // 5
unsigned char hWindowObstructionDetected = 0;   // 6
unsigned char VAR_WINDOW_UNKOWN=0;

unsigned short ccc_hWindowCurrentPosition = 0;
unsigned short ccc_hWindowTargetPosition = 0;
unsigned short ccc_hWindowPositionState = 0;
unsigned short ccc_hWindowObstructionDetected = 0;

#if WINDOW_CURRENTPOSITION_BRC_ENABLE
    unsigned char bc_para_hWindowCurrentPosition = 1;
#endif

#if WINDOW_TARGETPOSITION_BRC_ENABLE
    unsigned char bc_para_hWindowTargetPosition = 1;
#endif

#if WINDOW_POSITIONSTATE_BRC_ENABLE
    unsigned char bc_para_hWindowPositionState = 1;
#endif

#if WINDOW_OBSTRUCTIONDETECTED_BRC_ENABLE
    unsigned char bc_para_hWindowObstructionDetected = 1;
#endif

const u8 hWindowServiceName[13] = "WindowService";

const u8 VALID_RANGE_hWindowCurrentPosition[2]  = { 0x00, 0x64};
const u8 VALID_RANGE_hWindowTargetPosition[2]   = { 0x00, 0x64};
const u8 VALID_RANGE_hWindowPositionState[2]    = { 0x00, 0x02};

const u8 VALID_STEP_hWindowCurrentPosition  = 0x01;
const u8 VALID_STEP_hWindowTargetPosition   = 0x01;
const u8 VALID_STEP_hWindowPositionState    = 0x01;

const hap_characteristic_desc_t hap_desc_char_hWindowCurrentPosition_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8_PERCENTAGE, 0, 2, 1, 0, 0, VALID_RANGE_hWindowCurrentPosition, &VALID_STEP_hWindowCurrentPosition, BC_PARA_WINDOW_CURRENTPOSITION};
const hap_characteristic_desc_t hap_desc_char_hWindowTargetPosition_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_UINT8_PERCENTAGE, 0, 2, 1, 0, 0, VALID_RANGE_hWindowTargetPosition, &VALID_STEP_hWindowTargetPosition, BC_PARA_WINDOW_TARGETPOSITION};
const hap_characteristic_desc_t hap_desc_char_hWindowPositionState_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hWindowPositionState, &VALID_STEP_hWindowPositionState, BC_PARA_WINDOW_POSITIONSTATE};
const hap_characteristic_desc_t hap_desc_char_hWindowHoldPosition_R_S = {hap_SecureW_20, 0, CFG_BOOL_UNITLESS, 0, 0, 0, 0, 0, 0, 0, 0};
const hap_characteristic_desc_t hap_desc_char_hWindowObstructionDetected_R_S = {hap_PRead_NOTIFY, 0, CFG_BOOL_UNITLESS, 0, 0, 0, 0, 0, 0, 0, BC_PARA_WINDOW_OBSTRUCTIONDETECTED};

const hap_characteristic_desc_t hap_desc_char_window_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if WINDOW_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * hWindow_linked_svcs[] = {};
#endif

#if(WINDOW_IS_HIDDEN_SERVICE == 1)
const hap_service_desc_t hap_desc_service_window_A5_R_S = {WINDOW_INST_NO+1, (0x02 + WINDOW_AS_PRIMARY_SERVICE), WINDOW_LINKED_SVCS_LENGTH, WINDOW_LINKED_SVCS};
#else
const hap_service_desc_t hap_desc_service_window_A5_R_S = {WINDOW_INST_NO+1, (0x00 + WINDOW_AS_PRIMARY_SERVICE), WINDOW_LINKED_SVCS_LENGTH, WINDOW_LINKED_SVCS};
#endif

/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
int hWindowCurrentPositionReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hWindowTargetPositionReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hWindowTargetPositionWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~100
        assign_current_flash_char_value_addr();
    #if WINDOW_SERVICE_ENABLE
        WINDOW_STORE_CHAR_VALUE_T_WINDOWTARGETPOSITION_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
        hWindowCurrentPosition = hWindowTargetPosition; // update to Current state
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hWindowPositionStateReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hWindowHoldPositionWriteCallback(void *pp)
{
    //blt_adv_set_global_state ();	//this char do not support Notify, so no need update the GSN.

    if (hap_char_boundary_check()) {  // 0~1
        assign_current_flash_char_value_addr();
    #if WINDOW_SERVICE_ENABLE
        WINDOW_STORE_CHAR_VALUE_T_WINDOWHOLDPOSITION_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hWindowObstructionDetectedReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hWindowCurrentPositionWriteCB(void *pp)
{
    static u16 ccc_hWindowCurrentPosition_pre = 0;

    if(ccc_hWindowCurrentPosition_pre != ccc_hWindowCurrentPosition)
    {
        if(ccc_hWindowCurrentPosition == 0)
        {
            ccc_hWindowCurrentPosition_pre = ccc_hWindowCurrentPosition;
            EventNotifyCNT--;
            indicate_handle_no_remove(WINDOW_HANDLE_NUM_CURRENTPOSITION);
        }
        else
        {
            ccc_hWindowCurrentPosition_pre = ccc_hWindowCurrentPosition;
            EventNotifyCNT++;
            indicate_handle_no_reg(WINDOW_HANDLE_NUM_CURRENTPOSITION);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hWindowTargetPositionWriteCB(void *pp)
{
    static u16 ccc_hWindowTargetPosition_pre = 0;

    if(ccc_hWindowTargetPosition_pre != ccc_hWindowTargetPosition)
    {
        if(ccc_hWindowTargetPosition == 0)
        {
            ccc_hWindowTargetPosition_pre = ccc_hWindowTargetPosition;
            EventNotifyCNT--;
            indicate_handle_no_remove(WINDOW_HANDLE_NUM_TARGETPOSITION);
        }
        else
        {
            ccc_hWindowTargetPosition_pre = ccc_hWindowTargetPosition;
            EventNotifyCNT++;
            indicate_handle_no_reg(WINDOW_HANDLE_NUM_TARGETPOSITION);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hWindowPositionStateWriteCB(void *pp)
{
    static u16 ccc_hWindowPositionState_pre = 0;

    if(ccc_hWindowPositionState_pre != ccc_hWindowPositionState)
    {
        if(ccc_hWindowPositionState == 0)
        {
            ccc_hWindowPositionState_pre = ccc_hWindowPositionState;
            EventNotifyCNT--;
            indicate_handle_no_remove(WINDOW_HANDLE_NUM_POSITIONSTATE);
        }
        else
        {
            ccc_hWindowPositionState_pre = ccc_hWindowPositionState;
            EventNotifyCNT++;
            indicate_handle_no_reg(WINDOW_HANDLE_NUM_POSITIONSTATE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hWindowObstructionDetectedWriteCB(void *pp)
{
    static u16 ccc_hWindowObstructionDetected_pre = 0;

    if(ccc_hWindowObstructionDetected_pre != ccc_hWindowObstructionDetected)
    {
        if(ccc_hWindowObstructionDetected == 0)
        {
            ccc_hWindowObstructionDetected_pre = ccc_hWindowObstructionDetected;
            EventNotifyCNT--;
            indicate_handle_no_remove(WINDOW_HANDLE_NUM_OBSTRUCTIONDETECTED);
        }
        else
        {
            ccc_hWindowObstructionDetected_pre = ccc_hWindowObstructionDetected;
            EventNotifyCNT++;
            indicate_handle_no_reg(WINDOW_HANDLE_NUM_OBSTRUCTIONDETECTED);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
