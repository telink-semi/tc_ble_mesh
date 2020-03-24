/********************************************************************************************************
 * @file     hk_WindowCoveringService.c
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
#define _HK_WINDOW_COVERING_SERVICE_C_
 
 
/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "hk_WindowCoveringService.h"
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
unsigned char hWindow_CoveringTargetPosition = 0;   // 1
unsigned char hWindow_CoveringCurrentPosition = 0;  // 2
unsigned char hWindow_CoveringPositionState = 0;    // 3
unsigned char hWindow_CoveringHoldPosition = 0;     // 5
int hWindow_CoveringCurrentHTA = 0;                 // 6
int hWindow_CoveringTargetHTA = 0;                  // 7
int hWindow_CoveringCurrentVTA = 0;                 // 8
int hWindow_CoveringTargetVTA = 0;                  // 9
unsigned char hWindow_CoveringObstructionDetected = 0;  // 10
unsigned char VAR_WINDOWCOVERING_UNKOWN=0;

unsigned short ccc_hWindow_CoveringTargetPosition = 0;
unsigned short ccc_hWindow_CoveringCurrentPosition = 0;
unsigned short ccc_hWindow_CoveringPositionState = 0;
unsigned short ccc_hWindow_CoveringCurrentHTA = 0;
unsigned short ccc_hWindow_CoveringTargetHTA = 0;
unsigned short ccc_hWindow_CoveringCurrentVTA = 0;
unsigned short ccc_hWindow_CoveringTargetVTA = 0;
unsigned short ccc_hWindow_CoveringObstructionDetected = 0;

#if WINDOWCOVERING_TARGETPOSITION_BRC_ENABLE
    unsigned char bc_para_hWindow_CoveringTargetPosition = 1;
#endif

#if WINDOWCOVERING_CURRENTPOSITION_BRC_ENABLE
    unsigned char bc_para_hWindow_CoveringCurrentPosition = 1;
#endif

#if WINDOWCOVERING_POSITIONSTATE_BRC_ENABLE
    unsigned char bc_para_hWindow_CoveringPositionState = 1;
#endif

#if WINDOWCOVERING_CURRENTHTA_BRC_ENABLE
    unsigned char bc_para_hWindow_CoveringCurrentHTA = 1;
#endif

#if WINDOWCOVERING_TARGETHTA_BRC_ENABLE
    unsigned char bc_para_hWindow_CoveringTargetHTA = 1;
#endif

#if WINDOWCOVERING_CURRENTVTA_BRC_ENABLE
    unsigned char bc_para_hWindow_CoveringCurrentVTA = 1;
#endif

#if WINDOWCOVERING_TARGETVTA_BRC_ENABLE
    unsigned char bc_para_hWindow_CoveringTargetVTA = 1;
#endif

#if WINDOWCOVERING_OBSTRUCTIONDETECTED_BRC_ENABLE
    unsigned char bc_para_hWindow_CoveringObstructionDetected = 1;
#endif

const u8 hWindow_CoveringServiceName[22] = "Window_CoveringService";

const u8 VALID_RANGE_hWindow_CoveringTargetPosition[2]  = { 0x00, 0x64};
const u8 VALID_RANGE_hWindow_CoveringCurrentPosition[2] = { 0x00, 0x64};
const u8 VALID_RANGE_hWindow_CoveringPositionState[2]   = { 0x00, 0x02};
const u8 VALID_RANGE_hWindow_CoveringCurrentHTA[8]      = { 0xA6, 0xFF, 0xFF, 0xFF, 0x5A, 0x00, 0x00, 0x00};    // -90~90 (int)
const u8 VALID_RANGE_hWindow_CoveringTargetHTA[8]       = { 0xA6, 0xFF, 0xFF, 0xFF, 0x5A, 0x00, 0x00, 0x00};    // -90~90 (int)
const u8 VALID_RANGE_hWindow_CoveringCurrentVTA[8]      = { 0xA6, 0xFF, 0xFF, 0xFF, 0x5A, 0x00, 0x00, 0x00};    // -90~90 (int)
const u8 VALID_RANGE_hWindow_CoveringTargetVTA[8]       = { 0xA6, 0xFF, 0xFF, 0xFF, 0x5A, 0x00, 0x00, 0x00};    // -90~90 (int)

const u8 VALID_STEP_hWindow_CoveringTargetPosition  = 0x01;
const u8 VALID_STEP_hWindow_CoveringCurrentPosition = 0x01;
const u8 VALID_STEP_hWindow_CoveringPositionState   = 0x01;
const u8 VALID_STEP_hWindow_CoveringCurrentHTA[4]   = { 0x01, 0x00, 0x00, 0x00};
const u8 VALID_STEP_hWindow_CoveringTargetHTA[4]    = { 0x01, 0x00, 0x00, 0x00};
const u8 VALID_STEP_hWindow_CoveringCurrentVTA[4]   = { 0x01, 0x00, 0x00, 0x00};
const u8 VALID_STEP_hWindow_CoveringTargetVTA[4]    = { 0x01, 0x00, 0x00, 0x00};

const hap_characteristic_desc_t hap_desc_char_hWindow_CoveringTargetPosition_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_UINT8_PERCENTAGE, 0, 2, 1, 0, 0, VALID_RANGE_hWindow_CoveringTargetPosition, &VALID_STEP_hWindow_CoveringTargetPosition, BC_PARA_WINDOWCOVERING_TARGETPOSITION};
const hap_characteristic_desc_t hap_desc_char_hWindow_CoveringCurrentPosition_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8_PERCENTAGE, 0, 2, 1, 0, 0, VALID_RANGE_hWindow_CoveringCurrentPosition, &VALID_STEP_hWindow_CoveringCurrentPosition, BC_PARA_WINDOWCOVERING_CURRENTPOSITION};
const hap_characteristic_desc_t hap_desc_char_hWindow_CoveringPositionState_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hWindow_CoveringPositionState, &VALID_STEP_hWindow_CoveringPositionState, BC_PARA_WINDOWCOVERING_POSITIONSTATE};
const hap_characteristic_desc_t hap_desc_char_hWindow_CoveringHoldPosition_R_S = {hap_SecureW_20, 0, CFG_BOOL_UNITLESS, 0, 0, 0, 0, 0, 0, 0, 0};
const hap_characteristic_desc_t hap_desc_char_hWindow_CoveringCurrentHTA_R_S = {hap_PRead_NOTIFY, 0, CFG_INT32_ARCDEGREE, 0, 8, 4, 0, 0, VALID_RANGE_hWindow_CoveringCurrentHTA, VALID_STEP_hWindow_CoveringCurrentHTA, BC_PARA_WINDOWCOVERING_CURRENTHTA};
const hap_characteristic_desc_t hap_desc_char_hWindow_CoveringTargetHTA_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_INT32_ARCDEGREE, 0, 8, 4, 0, 0, VALID_RANGE_hWindow_CoveringTargetHTA, VALID_STEP_hWindow_CoveringTargetHTA, BC_PARA_WINDOWCOVERING_TARGETHTA};
const hap_characteristic_desc_t hap_desc_char_hWindow_CoveringCurrentVTA_R_S = {hap_PRead_NOTIFY, 0, CFG_INT32_ARCDEGREE, 0, 8, 4, 0, 0, VALID_RANGE_hWindow_CoveringCurrentVTA, VALID_STEP_hWindow_CoveringCurrentVTA, BC_PARA_WINDOWCOVERING_CURRENTVTA};
const hap_characteristic_desc_t hap_desc_char_hWindow_CoveringTargetVTA_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_INT32_ARCDEGREE, 0, 8, 4, 0, 0, VALID_RANGE_hWindow_CoveringTargetVTA, VALID_STEP_hWindow_CoveringTargetVTA, BC_PARA_WINDOWCOVERING_TARGETVTA};
const hap_characteristic_desc_t hap_desc_char_hWindow_CoveringObstructionDetected_R_S = {hap_PRead_NOTIFY, 0, CFG_BOOL_UNITLESS, 0, 0, 0, 0, 0, 0, 0, BC_PARA_WINDOWCOVERING_OBSTRUCTIONDETECTED};

const hap_characteristic_desc_t hap_desc_char_windowcovering_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if WINDOWCOVERING_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * hWindowCovering_linked_svcs[] = {};
#endif

#if(WINDOWCOVERING_IS_HIDDEN_SERVICE == 1)
const hap_service_desc_t hap_desc_service_windowcovering_A5_R_S = {WINDOW_COVERING_INST_NO+1, (0x02 + WINDOWCOVERING_AS_PRIMARY_SERVICE), WINDOWCOVERING_LINKED_SVCS_LENGTH, WINDOWCOVERING_LINKED_SVCS};
#else
const hap_service_desc_t hap_desc_service_windowcovering_A5_R_S = {WINDOW_COVERING_INST_NO+1, (0x00 + WINDOWCOVERING_AS_PRIMARY_SERVICE), WINDOWCOVERING_LINKED_SVCS_LENGTH, WINDOWCOVERING_LINKED_SVCS};
#endif


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
int hWindow_CoveringTargetPositionReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hWindow_CoveringTargetPositionWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~100
        assign_current_flash_char_value_addr();
    #if WINDOW_COVERING_SERVICE_ENABLE
        WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGTARGETPOSITION_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
        hWindow_CoveringCurrentPosition = hWindow_CoveringTargetPosition;   // update to Current state
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hWindow_CoveringCurrentPositionReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hWindow_CoveringPositionStateReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hWindow_CoveringHoldPositionWriteCallback(void *pp)
{
    //blt_adv_set_global_state ();	//this char do not support Notify, so no need update the GSN.

    if (hap_char_boundary_check()) {  // 0~1
        assign_current_flash_char_value_addr();
    #if WINDOW_COVERING_SERVICE_ENABLE
        WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGHOLDPOSITION_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hWindow_CoveringCurrentHTAReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hWindow_CoveringTargetHTAReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hWindow_CoveringTargetHTAWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // -90~90 degree
        assign_current_flash_char_value_addr();
    #if WINDOW_COVERING_SERVICE_ENABLE
        WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGTARGETHTA_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
        hWindow_CoveringCurrentHTA = hWindow_CoveringTargetHTA;
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hWindow_CoveringCurrentVTAReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hWindow_CoveringTargetVTAReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hWindow_CoveringTargetVTAWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // -90~90 degree
        assign_current_flash_char_value_addr();
    #if WINDOW_COVERING_SERVICE_ENABLE
        WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGTARGETVTA_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
        hWindow_CoveringCurrentVTA = hWindow_CoveringTargetVTA;
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hWindow_CoveringObstructionDetectedReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hWindow_CoveringTargetPositionWriteCB(void *pp)
{
    static u16 ccc_hWindow_CoveringTargetPosition_pre = 0;

    if(ccc_hWindow_CoveringTargetPosition_pre != ccc_hWindow_CoveringTargetPosition)
    {
        if(ccc_hWindow_CoveringTargetPosition == 0)
        {
            ccc_hWindow_CoveringTargetPosition_pre = ccc_hWindow_CoveringTargetPosition;
            EventNotifyCNT--;
            indicate_handle_no_remove(WINDOWCOVERING_HANDLE_NUM_TARGETPOSITION);
        }
        else
        {
            ccc_hWindow_CoveringTargetPosition_pre = ccc_hWindow_CoveringTargetPosition;
            EventNotifyCNT++;
            indicate_handle_no_reg(WINDOWCOVERING_HANDLE_NUM_TARGETPOSITION);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hWindow_CoveringCurrentPositionWriteCB(void *pp)
{
    static u16 ccc_hWindow_CoveringCurrentPosition_pre = 0;

    if(ccc_hWindow_CoveringCurrentPosition_pre != ccc_hWindow_CoveringCurrentPosition)
    {
        if(ccc_hWindow_CoveringCurrentPosition == 0)
        {
            ccc_hWindow_CoveringCurrentPosition_pre = ccc_hWindow_CoveringCurrentPosition;
            EventNotifyCNT--;
            indicate_handle_no_remove(WINDOWCOVERING_HANDLE_NUM_CURRENTPOSITION);
        }
        else
        {
            ccc_hWindow_CoveringCurrentPosition_pre = ccc_hWindow_CoveringCurrentPosition;
            EventNotifyCNT++;
            indicate_handle_no_reg(WINDOWCOVERING_HANDLE_NUM_CURRENTPOSITION);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hWindow_CoveringPositionStateWriteCB(void *pp)
{
    static u16 ccc_hWindow_CoveringPositionState_pre = 0;

    if(ccc_hWindow_CoveringPositionState_pre != ccc_hWindow_CoveringPositionState)
    {
        if(ccc_hWindow_CoveringPositionState == 0)
        {
            ccc_hWindow_CoveringPositionState_pre = ccc_hWindow_CoveringPositionState;
            EventNotifyCNT--;
            indicate_handle_no_remove(WINDOWCOVERING_HANDLE_NUM_POSITIONSTATE);
        }
        else
        {
            ccc_hWindow_CoveringPositionState_pre = ccc_hWindow_CoveringPositionState;
            EventNotifyCNT++;
            indicate_handle_no_reg(WINDOWCOVERING_HANDLE_NUM_POSITIONSTATE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hWindow_CoveringCurrentHTAWriteCB(void *pp)
{
    static u16 ccc_hWindow_CoveringCurrentHTA_pre = 0;

    if(ccc_hWindow_CoveringCurrentHTA_pre != ccc_hWindow_CoveringCurrentHTA)
    {
        if(ccc_hWindow_CoveringCurrentHTA == 0)
        {
            ccc_hWindow_CoveringCurrentHTA_pre = ccc_hWindow_CoveringCurrentHTA;
            EventNotifyCNT--;
            indicate_handle_no_remove(WINDOWCOVERING_HANDLE_NUM_CURRENTHTA);
        }
        else
        {
            ccc_hWindow_CoveringCurrentHTA_pre = ccc_hWindow_CoveringCurrentHTA;
            EventNotifyCNT++;
            indicate_handle_no_reg(WINDOWCOVERING_HANDLE_NUM_CURRENTHTA);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hWindow_CoveringTargetHTAWriteCB(void *pp)
{
    static u16 ccc_hWindow_CoveringTargetHTA_pre = 0;

    if(ccc_hWindow_CoveringTargetHTA_pre != ccc_hWindow_CoveringTargetHTA)
    {
        if(ccc_hWindow_CoveringTargetHTA == 0)
        {
            ccc_hWindow_CoveringTargetHTA_pre = ccc_hWindow_CoveringTargetHTA;
            EventNotifyCNT--;
            indicate_handle_no_remove(WINDOWCOVERING_HANDLE_NUM_TARGETHTA);
        }
        else
        {
            ccc_hWindow_CoveringTargetHTA_pre = ccc_hWindow_CoveringTargetHTA;
            EventNotifyCNT++;
            indicate_handle_no_reg(WINDOWCOVERING_HANDLE_NUM_TARGETHTA);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hWindow_CoveringCurrentVTAWriteCB(void *pp)
{
    static u16 ccc_hWindow_CoveringCurrentVTA_pre = 0;

    if(ccc_hWindow_CoveringCurrentVTA_pre != ccc_hWindow_CoveringCurrentVTA)
    {
        if(ccc_hWindow_CoveringCurrentVTA == 0)
        {
            ccc_hWindow_CoveringCurrentVTA_pre = ccc_hWindow_CoveringCurrentVTA;
            EventNotifyCNT--;
            indicate_handle_no_remove(WINDOWCOVERING_HANDLE_NUM_CURRENTVTA);
        }
        else
        {
            ccc_hWindow_CoveringCurrentVTA_pre = ccc_hWindow_CoveringCurrentVTA;
            EventNotifyCNT++;
            indicate_handle_no_reg(WINDOWCOVERING_HANDLE_NUM_CURRENTVTA);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hWindow_CoveringTargetVTAWriteCB(void *pp)
{
    static u16 ccc_hWindow_CoveringTargetVTA_pre = 0;

    if(ccc_hWindow_CoveringTargetVTA_pre != ccc_hWindow_CoveringTargetVTA)
    {
        if(ccc_hWindow_CoveringTargetVTA == 0)
        {
            ccc_hWindow_CoveringTargetVTA_pre = ccc_hWindow_CoveringTargetVTA;
            EventNotifyCNT--;
            indicate_handle_no_remove(WINDOWCOVERING_HANDLE_NUM_TARGETVTA);
        }
        else
        {
            ccc_hWindow_CoveringTargetVTA_pre = ccc_hWindow_CoveringTargetVTA;
            EventNotifyCNT++;
            indicate_handle_no_reg(WINDOWCOVERING_HANDLE_NUM_TARGETVTA);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hWindow_CoveringObstructionDetectedWriteCB(void *pp)
{
    static u16 ccc_hWindow_CoveringObstructionDetected_pre = 0;

    if(ccc_hWindow_CoveringObstructionDetected_pre != ccc_hWindow_CoveringObstructionDetected)
    {
        if(ccc_hWindow_CoveringObstructionDetected == 0)
        {
            ccc_hWindow_CoveringObstructionDetected_pre = ccc_hWindow_CoveringObstructionDetected;
            EventNotifyCNT--;
            indicate_handle_no_remove(WINDOWCOVERING_HANDLE_NUM_OBSTRUCTIONDETECTED);
        }
        else
        {
            ccc_hWindow_CoveringObstructionDetected_pre = ccc_hWindow_CoveringObstructionDetected;
            EventNotifyCNT++;
            indicate_handle_no_reg(WINDOWCOVERING_HANDLE_NUM_OBSTRUCTIONDETECTED);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
