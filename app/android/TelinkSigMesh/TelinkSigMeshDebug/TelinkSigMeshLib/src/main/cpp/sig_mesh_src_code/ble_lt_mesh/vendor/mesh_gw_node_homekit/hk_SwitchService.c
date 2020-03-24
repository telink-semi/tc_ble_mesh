/********************************************************************************************************
 * @file     hk_SwitchService.c
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
#define _HK_SWITCH_SERVICE_C_
 
 
/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "hk_SwitchService.h"
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
unsigned char hSwitchOn = 1;
unsigned char VAR_SWITCH_UNKOWN=0;

unsigned short ccc_hSwitchOn = 0;

#if SWITCH_ON_BRC_ENABLE
    unsigned char bc_para_hSwitchOn = 1;
#endif

const u8 hSwitchServiceName[13] = "SwitchService";

const hap_characteristic_desc_t hap_desc_char_SWITCH_ON_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_BOOL_UNITLESS, 0, 0, 0, 0, 0, 0, 0, BC_PARA_SWITCH_ON};

const hap_characteristic_desc_t hap_desc_char_switch_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if SWITCH_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * hSwitch_linked_svcs[] = {};
#endif

#if(SWITCH_IS_HIDDEN_SERVICE == 1)
const hap_service_desc_t hap_desc_service_switch_A5_R_S = {SWITCH_INST_NO+1, (0x02 + SWITCH_AS_PRIMARY_SERVICE), SWITCH_LINKED_SVCS_LENGTH, SWITCH_LINKED_SVCS};
#else
const hap_service_desc_t hap_desc_service_switch_A5_R_S = {SWITCH_INST_NO+1, (0x00 + SWITCH_AS_PRIMARY_SERVICE), SWITCH_LINKED_SVCS_LENGTH, SWITCH_LINKED_SVCS};
#endif


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
int SwitchOnReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int SwitchOnWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~1
        assign_current_flash_char_value_addr();
    #if SWITCH_SERVICE_ENABLE
        SWITCH_STORE_CHAR_VALUE_T_SWITCHON_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSwitchOnWriteCB(void *pp)
{
    static u16 ccc_hSwitchOn_pre = 0;

    if(ccc_hSwitchOn_pre != ccc_hSwitchOn)
    {
        if(ccc_hSwitchOn == 0)
        {
            ccc_hSwitchOn_pre = ccc_hSwitchOn;
            EventNotifyCNT--;
            indicate_handle_no_remove(SWITCH_HANDLE_NUM_ON);
        }
        else
        {
            ccc_hSwitchOn_pre = ccc_hSwitchOn;
            EventNotifyCNT++;
            indicate_handle_no_reg(SWITCH_HANDLE_NUM_ON);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
