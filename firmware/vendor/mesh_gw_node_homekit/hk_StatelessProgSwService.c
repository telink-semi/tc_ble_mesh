/********************************************************************************************************
 * @file     hk_StatelessProgSwService.c
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
#define _HK_STATELESS_PROG_SW_SERVICE_C_
 
 
/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "hk_StatelessProgSwService.h"
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
unsigned char hStateless_Prog_SwEvent = 0;
unsigned char hStateless_Prog_Sw_ServiceLabelIndex = 1;

unsigned char VAR_STATELESS_PROG_SW_UNKOWN=0;

unsigned short ccc_hStatelessProg_SwEvent = 0;

#if STATELESS_PROG_SW_SWEVENT_BRC_ENABLE
    unsigned char bc_para_hStatelessProg_SwEvent = 1;
#endif

const u8 hStateless_Prog_SwServiceName[24] = "Stateless_Prog_SwService";

const u8 VALID_RANGE_STATELESS_PROG_SW_EVENT[2] = { 0x00, 0x02};
const u8 VALID_RANGE_SERVICE_LABEL_INDEX[2] = { 0x01, 0xFF};

const u8 VALID_STEP_STATELESS_PROG_SW_EVENT = 0x01;
const u8 VALID_STEP_SERVICE_LABEL_INDEX = 0x01;

const hap_characteristic_desc_t hap_desc_char_hStateless_Prog_SwEvent_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_STATELESS_PROG_SW_EVENT, &VALID_STEP_STATELESS_PROG_SW_EVENT, BC_PARA_STATELESS_PROG_SW_SWEVENT};
const hap_characteristic_desc_t hap_desc_char_hServiceLabelIndex_R_S = {hap_SecureR_10, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_SERVICE_LABEL_INDEX, &VALID_STEP_SERVICE_LABEL_INDEX, 0};

const hap_characteristic_desc_t hap_desc_char_stateless_prog_sw_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if STATELESS_PROG_SW_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * StatelessProgSw_linked_svcs[] = {};
#endif

#if(STATELESS_PROG_SW_IS_HIDDEN_SERVICE == 1)
const hap_service_desc_t hap_desc_service_stateless_prog_sw_A5_R_S = {STATELESS_PROG_SW_INST_NO+1, (0x02 + STATELESS_PROG_SW_AS_PRIMARY_SERVICE), STATELESS_PROG_SW_LINKED_SVCS_LENGTH, STATELESS_PROG_SW_LINKED_SVCS};
#else
const hap_service_desc_t hap_desc_service_stateless_prog_sw_A5_R_S = {STATELESS_PROG_SW_INST_NO+1, (0x00 + STATELESS_PROG_SW_AS_PRIMARY_SERVICE), STATELESS_PROG_SW_LINKED_SVCS_LENGTH, STATELESS_PROG_SW_LINKED_SVCS};
#endif


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
int Stateless_Prog_SwEventReadCallback(void *pp)
{
    hStateless_Prog_SwEvent = -1;   // need to return null value while read

    session_timeout_tick = clock_time () | 1;
    return 1;
}

#if 0
int Stateless_Prog_SwEventWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~2
        assign_current_flash_char_value_addr();
    #if STATELESS_PROG_SW_SERVICE_ENABLE
        STATELESS_PROG_SW_STORE_CHAR_VALUE_T_STATELESS_PROG_SWEVENT_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}
#endif

int Stateless_Prog_Sw_ServiceLabelIndexReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hStatelessProg_SwEventWriteCB(void *pp)
{
    static u16 ccc_hStatelessProg_SwEvent_pre = 0;

    if(ccc_hStatelessProg_SwEvent_pre != ccc_hStatelessProg_SwEvent)
    {
        if(ccc_hStatelessProg_SwEvent == 0)
        {
            ccc_hStatelessProg_SwEvent_pre = ccc_hStatelessProg_SwEvent;
            EventNotifyCNT--;
            indicate_handle_no_remove(STATELESSPROG_HANDLE_NUM_SWEVENT);
        }
        else
        {
            ccc_hStatelessProg_SwEvent_pre = ccc_hStatelessProg_SwEvent;
            EventNotifyCNT++;
            indicate_handle_no_reg(STATELESSPROG_HANDLE_NUM_SWEVENT);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}


/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
