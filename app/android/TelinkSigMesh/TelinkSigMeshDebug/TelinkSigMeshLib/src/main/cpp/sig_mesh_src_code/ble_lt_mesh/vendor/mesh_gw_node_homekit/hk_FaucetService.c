/********************************************************************************************************
 * @file     hk_FaucetService.c
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
#define _HK_FAUCET_SERVICE_C_
 
 
/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "hk_FaucetService.h"
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
unsigned char hFaucetActive = 0;
unsigned char hFaucetStatusFault = 0;

unsigned char VAR_FAUCET_UNKOWN=0;

unsigned short ccc_hFaucetActive = 0;
unsigned short ccc_hFaucetStatusFault = 0;


#if FAUCET_ACTIVE_BRC_ENABLE
    unsigned char bc_para_hFaucetActive = 1;
#endif

#if FAUCET_STATUSFAULT_BRC_ENABLE
    unsigned char bc_para_hFaucetStatusFault = 1;
#endif

const u8 hFaucetServiceName[13] = "FaucetService";

const u8 FAUCET_ACTIVE_VALID_RANGE[2] = {0x00, 0x01};
const u8 FAUCET_STATUSFAULT_VALID_RANGE[2] = {0x00, 0x01};

const u8 FAUCET_ACTIVE_VALID_STEP = 0x01;
const u8 FAUCET_STATUSFAULT_VALID_STEP = 0x01;


const hap_characteristic_desc_t hap_desc_char_FAUCET_ACTIVE_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, FAUCET_ACTIVE_VALID_RANGE, &FAUCET_ACTIVE_VALID_STEP, BC_PARA_FAUCET_ACTIVE};
const hap_characteristic_desc_t hap_desc_char_FAUCET_STATUSFAULT_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, FAUCET_STATUSFAULT_VALID_RANGE, &FAUCET_STATUSFAULT_VALID_STEP, BC_PARA_FAUCET_STATUSFAULT};

const hap_characteristic_desc_t hap_desc_char_faucet_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if FAUCET_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * hFaucet_linked_svcs[] = {};
//const u16 * hFaucet_linked_svcs[] = {(insts + HEATERCOOLER_INST_NO), (insts + VALVE_INST_NO)};
#endif

#if(FAUCET_IS_HIDDEN_SERVICE == 1)
const hap_service_desc_t hap_desc_service_faucet_A5_R_S = {FAUCET_INST_NO+1, (0x02 + FAUCET_AS_PRIMARY_SERVICE), FAUCET_LINKED_SVCS_LENGTH, FAUCET_LINKED_SVCS};
#else
const hap_service_desc_t hap_desc_service_faucet_A5_R_S = {FAUCET_INST_NO+1, (0x00 + FAUCET_AS_PRIMARY_SERVICE), FAUCET_LINKED_SVCS_LENGTH, FAUCET_LINKED_SVCS};
#endif

/* ------------------------------------
    Function Prototypes
   ------------------------------------ */

int FaucetActiveReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int FaucetActiveWriteCallback(void *pp)
{
	blt_adv_set_global_state ();

	if (hap_char_boundary_check()) {
		assign_current_flash_char_value_addr();
	#if FAUCET_SERVICE_ENABLE
		FAUCET_STORE_CHAR_VALUE_T_ACTIVE_RESTORE
	#endif
	} else {
		store_char_value_to_flash();
	}

	session_timeout_tick = clock_time () | 1;
	return 1;
}

int FaucetStatusFaultReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}


int ccc_FaucetActiveWriteCB(void *pp)
{
	static u16 ccc_hFaucetActive_pre = 0;

	if(ccc_hFaucetActive_pre != ccc_hFaucetActive)
	{
		if(ccc_hFaucetActive == 0)
		{
			ccc_hFaucetActive_pre = ccc_hFaucetActive;
			EventNotifyCNT--;
			indicate_handle_no_remove(FAUCET_HANDLE_NUM_ACTIVE);
		}
		else
		{
			ccc_hFaucetActive_pre = ccc_hFaucetActive;
			EventNotifyCNT++;
			indicate_handle_no_reg(FAUCET_HANDLE_NUM_ACTIVE);
		}
	}
	session_timeout_tick = clock_time () | 1;
	return 1;
}

int ccc_FaucetStatusFaultWriteCB(void *pp)
{
	static u16 ccc_hFaucetStatusFault_pre = 0;

	if(ccc_hFaucetStatusFault_pre != ccc_hFaucetStatusFault)
	{
		if(ccc_hFaucetStatusFault == 0)
		{
			ccc_hFaucetStatusFault_pre = ccc_hFaucetStatusFault;
			EventNotifyCNT--;
			indicate_handle_no_remove(FAUCET_HANDLE_NUM_STATUSFAULT);
		}
		else
		{
			ccc_hFaucetStatusFault_pre = ccc_hFaucetStatusFault;
			EventNotifyCNT++;
			indicate_handle_no_reg(FAUCET_HANDLE_NUM_STATUSFAULT);
		}
	}
	session_timeout_tick = clock_time () | 1;
	return 1;
}

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
