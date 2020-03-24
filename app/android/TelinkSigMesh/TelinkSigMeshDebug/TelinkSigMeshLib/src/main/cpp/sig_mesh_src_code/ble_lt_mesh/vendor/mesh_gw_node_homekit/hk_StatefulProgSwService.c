/********************************************************************************************************
 * @file     hk_StatefulProgSwService.c
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
#define _HK_STATEFUL_PROG_SW_SERVICE_C_
 
 
/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "hk_StatefulProgSwService.h"
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
unsigned char hStateful_Prog_SwEvent = 0;       // 1
unsigned char hStateful_Prog_SwOutputState = 0; // 2
unsigned char VAR_STATEFUL_PROG_SW_UNKOWN=0;

unsigned short ccc_hStatefulProg_SwEvent = 0;
unsigned short ccc_hStatefulProg_SwOutputState = 0;

#if STATEFULPROGSW_SWEVENT_BRC_ENABLE
    unsigned char bc_para_hStatefulProg_SwEvent = 1;
#endif

#if STATEFULPROGSW_SWOUTPUTSTATE_BRC_ENABLE
    unsigned char bc_para_hStatefulProg_SwOutputState = 1;
#endif

const u8 hStateful_Prog_SwServiceName[23] = "Stateful_Prog_SwService";

const u8 VALID_RANGE_hStateful_Prog_SwEvent[2]          = { 0x00, 0x01};
const u8 VALID_RANGE_hStateful_Prog_SwOutputState[2]    = { 0x00, 0x01};

const u8 VALID_STEP_hStateful_Prog_SwEvent			= 0x01;
const u8 VALID_STEP_hStateful_Prog_SwOutputState	= 0x01;

const hap_characteristic_desc_t hap_desc_char_hStateful_Prog_SwEvent_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hStateful_Prog_SwEvent, &VALID_STEP_hStateful_Prog_SwEvent, BC_PARA_STATEFULPROGSW_SWEVENT};
const hap_characteristic_desc_t hap_desc_char_hStateful_Prog_SwOutputState_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hStateful_Prog_SwOutputState, &VALID_STEP_hStateful_Prog_SwOutputState, BC_PARA_STATEFULPROGSW_SWOUTPUTSTATE};

const hap_characteristic_desc_t hap_desc_char_stateful_prog_sw_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if STATEFUL_PROG_SW_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * hStatefulProgSw_linked_svcs[] = {};
#endif

#if(STATEFUL_PROG_SW_IS_HIDDEN_SERVICE == 1)
const hap_service_desc_t hap_desc_service_stateful_prog_sw_A5_R_S = {STATEFUL_PROG_SW_INST_NO+1, (0x02 + STATEFUL_PROG_SW_AS_PRIMARY_SERVICE), STATEFUL_PROG_SW_LINKED_SVCS_LENGTH, STATEFUL_PROG_SW_LINKED_SVCS};
#else
const hap_service_desc_t hap_desc_service_stateful_prog_sw_A5_R_S = {STATEFUL_PROG_SW_INST_NO+1, (0x00 + STATEFUL_PROG_SW_AS_PRIMARY_SERVICE), STATEFUL_PROG_SW_LINKED_SVCS_LENGTH, STATEFUL_PROG_SW_LINKED_SVCS};
#endif

/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
int hStateful_Prog_SwEventReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hStateful_Prog_SwOutputStateReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hStateful_Prog_SwOutputStateWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~1
        assign_current_flash_char_value_addr();
    #if STATEFUL_PROG_SW_SERVICE_ENABLE
        STATEFUL_PROG_SW_STORE_CHAR_VALUE_T_STATEFUL_PROG_SWOUTPUTSTATE_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
        hStateful_Prog_SwEvent = hStateful_Prog_SwOutputState;
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hStatefulProg_SwEventWriteCB(void *pp)
{
    static u16 ccc_hStatefulProg_SwEvent_pre = 0;

    if(ccc_hStatefulProg_SwEvent_pre != ccc_hStatefulProg_SwEvent)
    {
        if(ccc_hStatefulProg_SwEvent == 0)
        {
            ccc_hStatefulProg_SwEvent_pre = ccc_hStatefulProg_SwEvent;
            EventNotifyCNT--;
            indicate_handle_no_remove(STATEFULPROG_HANDLE_NUM_SWEVENT);
        }
        else
        {
            ccc_hStatefulProg_SwEvent_pre = ccc_hStatefulProg_SwEvent;
            EventNotifyCNT++;
            indicate_handle_no_reg(STATEFULPROG_HANDLE_NUM_SWEVENT);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hStatefulProg_SwOutputStateWriteCB(void *pp)
{
    static u16 ccc_hStatefulProg_SwOutputState_pre = 0;

    if(ccc_hStatefulProg_SwOutputState_pre != ccc_hStatefulProg_SwOutputState)
    {
        if(ccc_hStatefulProg_SwOutputState == 0)
        {
            ccc_hStatefulProg_SwOutputState_pre = ccc_hStatefulProg_SwOutputState;
            EventNotifyCNT--;
            indicate_handle_no_remove(STATEFULPROG_HANDLE_NUM_SWOUTPUTSTATE);
        }
        else
        {
            ccc_hStatefulProg_SwOutputState_pre = ccc_hStatefulProg_SwOutputState;
            EventNotifyCNT++;
            indicate_handle_no_reg(STATEFULPROG_HANDLE_NUM_SWOUTPUTSTATE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
