/********************************************************************************************************
 * @file     hk_OutletService.c
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
#define _HK_OUTLET_SERVICE_C_
 
 
/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "hk_OutletService.h"
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
unsigned char hOuletOn = 0;
unsigned char hOutletInUse = 0;
unsigned char VAR_OUTLET_UNKOWN=0;

unsigned short ccc_hOuletOn = 0;
unsigned short ccc_hOutletInUse = 0;

#if OUTLET_ON_BRC_ENABLE
    unsigned char bc_para_hOuletOn = 1;
#endif

#if OUTLET_IN_USE_BRC_ENABLE
    unsigned char bc_para_hOutletInUse = 1;
#endif

const u8 hOutletServiceName[13] = "OutletService";

const hap_characteristic_desc_t hap_desc_char_OUTLET_ON_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_BOOL_UNITLESS, 0, 0, 0, 0, 0, 0, 0, BC_PARA_OUTLET_ON};
const hap_characteristic_desc_t hap_desc_char_OUTLET_IN_USE_R_S = {hap_PRead_NOTIFY, 0, CFG_BOOL_UNITLESS, 0, 0, 0, 0, 0, 0, 0, BC_PARA_OUTLET_IN_USE};

const hap_characteristic_desc_t hap_desc_char_outlet_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if OUTLET_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * hOutlet_linked_svcs[] = {};
#endif

#if(OUTLET_IS_HIDDEN_SERVICE == 1)
const hap_service_desc_t hap_desc_service_outlet_A5_R_S = {OUTLET_INST_NO+1, (0x02 + OUTLET_AS_PRIMARY_SERVICE), OUTLET_LINKED_SVCS_LENGTH, OUTLET_LINKED_SVCS};
#else
const hap_service_desc_t hap_desc_service_outlet_A5_R_S = {OUTLET_INST_NO+1, (0x00 + OUTLET_AS_PRIMARY_SERVICE), OUTLET_LINKED_SVCS_LENGTH, OUTLET_LINKED_SVCS};
#endif

/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
int OuletOnReadCallback(void *pp)       //
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int OuletOnWriteCallback(void *pp)      //
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~1
        assign_current_flash_char_value_addr();
    #if OUTLET_SERVICE_ENABLE
        OUTLET_STORE_CHAR_VALUE_T_OULETON_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int OutletInUseReadCallback(void *pp)       //
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hOuletOnWriteCB(void *pp)
{
    static u16 ccc_hOuletOn_pre = 0;

    if(ccc_hOuletOn_pre != ccc_hOuletOn)
    {
        if(ccc_hOuletOn == 0)
        {
            ccc_hOuletOn_pre = ccc_hOuletOn;
            EventNotifyCNT--;
            indicate_handle_no_remove(OUTLET_HANDLE_NUM_ON);
        }
        else
        {
            ccc_hOuletOn_pre = ccc_hOuletOn;
            EventNotifyCNT++;
            indicate_handle_no_reg(OUTLET_HANDLE_NUM_ON);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hOutletInUseWriteCB(void *pp)
{
    static u16 ccc_hOutletInUse_pre = 0;

    if(ccc_hOutletInUse_pre != ccc_hOutletInUse)
    {
        if(ccc_hOutletInUse == 0)
        {
            ccc_hOutletInUse_pre = ccc_hOutletInUse;
            EventNotifyCNT--;
            indicate_handle_no_remove(OUTLET_HANDLE_NUM_INUSE);
        }
        else
        {
            ccc_hOutletInUse_pre = ccc_hOutletInUse;
            EventNotifyCNT++;
            indicate_handle_no_reg(OUTLET_HANDLE_NUM_INUSE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
