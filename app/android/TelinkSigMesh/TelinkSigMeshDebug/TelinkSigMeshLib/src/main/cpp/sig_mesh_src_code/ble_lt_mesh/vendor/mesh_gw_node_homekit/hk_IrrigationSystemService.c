/********************************************************************************************************
 * @file     hk_IrrigationSystemService.c
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
#define _HK_IRRIGATION_SYSTEM_SERVICE_C_
 
 
/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "hk_IrrigationSystemService.h"
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
unsigned char hIrrigationSystemActive = 0;
unsigned char hIrrigationSystemProgramMode = 0;
unsigned char hIrrigationSystemInUse = 0;
unsigned int hIrrigationSystemRemainingDuration = 0;
unsigned char hIrrigationSystemStatusFault = 0;

unsigned char VAR_IRRIGATION_SYSTEM_UNKOWN=0;

unsigned short ccc_hIrrigationSystemActive = 0;
unsigned short ccc_hIrrigationSystemProgramMode = 0;
unsigned short ccc_hIrrigationSystemInUse = 0;
unsigned short ccc_hIrrigationSystemRemainingDuration = 0;
unsigned short ccc_hIrrigationSystemStatusFault = 0;


#if IRRIGATIONSYSTEM_ACTIVE_BRC_ENABLE
    unsigned char bc_para_hIrrigationSystemActive = 1;
#endif

#if IRRIGATIONSYSTEM_PROGRAMMODE_BRC_ENABLE
    unsigned char bc_para_hIrrigationSystemProgramMode = 1;
#endif

#if IRRIGATIONSYSTEM_INUSE_BRC_ENABLE
    unsigned char bc_para_hIrrigationSystemInUse = 1;
#endif

#if IRRIGATIONSYSTEM_REMAININGDURATION_BRC_ENABLE
    unsigned char bc_para_hIrrigationSystemRemainingDuration = 1;
#endif

#if IRRIGATIONSYSTEM_STATUSFAULT_BRC_ENABLE
    unsigned char bc_para_hIrrigationSystemStatusFault = 1;
#endif

const u8 hIrrigationSystemServiceName[23] = "IrrigationSystemService";

const u8 IRRIGATIONSYSTEM_ACTIVE_VALID_RANGE[2] = {0x00, 0x01};
const u8 IRRIGATIONSYSTEM_PROGRAMMODE_VALID_RANGE[2] = {0x00, 0x02};
const u8 IRRIGATIONSYSTEM_INUSE_VALID_RANGE[2] = {0x00, 0x01};
const u8 IRRIGATIONSYSTEM_REMAININGDURATION_VALID_RANGE[8] = {0x00, 0x00, 0x00, 0x00, 0x10, 0x0E, 0x00, 0x00};  /* 0~3600 */
const u8 IRRIGATIONSYSTEM_STATUSFAULT_VALID_RANGE[2] = {0x00, 0x01};

const u8 IRRIGATIONSYSTEM_ACTIVE_VALID_STEP = 0x01;
const u8 IRRIGATIONSYSTEM_PROGRAMMODE_VALID_STEP = 0x01;
const u8 IRRIGATIONSYSTEM_INUSE_VALID_STEP = 0x01;
const u8 IRRIGATIONSYSTEM_REMAININGDURATION_VALID_STEP[4] = {0x01, 0x00, 0x00, 0x00};
const u8 IRRIGATIONSYSTEM_STATUSFAULT_VALID_STEP = 0x01;


const hap_characteristic_desc_t hap_desc_char_IRRIGATIONSYSTEM_ACTIVE_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, IRRIGATIONSYSTEM_ACTIVE_VALID_RANGE, &IRRIGATIONSYSTEM_ACTIVE_VALID_STEP, BC_PARA_IRRIGATIONSYSTEM_ACTIVE};
const hap_characteristic_desc_t hap_desc_char_IRRIGATIONSYSTEM_PROGRAMMODE_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, IRRIGATIONSYSTEM_PROGRAMMODE_VALID_RANGE, &IRRIGATIONSYSTEM_PROGRAMMODE_VALID_STEP, BC_PARA_IRRIGATIONSYSTEM_PROGRAMMODE};
const hap_characteristic_desc_t hap_desc_char_IRRIGATIONSYSTEM_INUSE_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, IRRIGATIONSYSTEM_INUSE_VALID_RANGE, &IRRIGATIONSYSTEM_INUSE_VALID_STEP, BC_PARA_IRRIGATIONSYSTEM_INUSE};
const hap_characteristic_desc_t hap_desc_char_IRRIGATIONSYSTEM_REMAININGDURATION_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT32, 0, 8, 4, 0, 0, IRRIGATIONSYSTEM_REMAININGDURATION_VALID_RANGE, IRRIGATIONSYSTEM_REMAININGDURATION_VALID_STEP, BC_PARA_IRRIGATIONSYSTEM_REMAININGDURATION};
const hap_characteristic_desc_t hap_desc_char_IRRIGATIONSYSTEM_STATUSFAULT_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, IRRIGATIONSYSTEM_STATUSFAULT_VALID_RANGE, &IRRIGATIONSYSTEM_STATUSFAULT_VALID_STEP, BC_PARA_IRRIGATIONSYSTEM_STATUSFAULT};

const hap_characteristic_desc_t hap_desc_char_irrigation_system_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if IRRIGATIONSYSTEM_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * hIrrigationSystem_linked_svcs[] = {};
//const u16 * hIrrigationSystem_linked_svcs[] = {(insts + VALVE_INST_NO)};
#endif

#if(IRRIGATIONSYSTEM_IS_HIDDEN_SERVICE == 1)
const hap_service_desc_t hap_desc_service_irrigation_system_A5_R_S = {IRRIGATIONSYSTEM_INST_NO+1, (0x02 + IRRIGATIONSYSTEM_AS_PRIMARY_SERVICE), IRRIGATIONSYSTEM_LINKED_SVCS_LENGTH, IRRIGATIONSYSTEM_LINKED_SVCS};
#else
const hap_service_desc_t hap_desc_service_irrigation_system_A5_R_S = {IRRIGATIONSYSTEM_INST_NO+1, (0x00 + IRRIGATIONSYSTEM_AS_PRIMARY_SERVICE), IRRIGATIONSYSTEM_LINKED_SVCS_LENGTH, IRRIGATIONSYSTEM_LINKED_SVCS};
#endif

/* ------------------------------------
    Function Prototypes
   ------------------------------------ */

int IrrigationSystemActiveReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int IrrigationSystemActiveWriteCallback(void *pp)
{
	blt_adv_set_global_state ();

	if (hap_char_boundary_check()) {
		assign_current_flash_char_value_addr();
	#if IRRIGATION_SYSTEM_SERVICE_ENABLE
		IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T_ACTIVE_RESTORE
	#endif
	} else {
		store_char_value_to_flash();
	}

	session_timeout_tick = clock_time () | 1;
	return 1;
}

int IrrigationSystemProgramModeReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int IrrigationSystemInUseReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int IrrigationSystemRemainingDurationReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int IrrigationSystemStatusFaultReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_IrrigationSystemActiveWriteCB(void *pp)
{
	static u16 ccc_hIrrigationSystemActive_pre = 0;

	if(ccc_hIrrigationSystemActive_pre != ccc_hIrrigationSystemActive)
	{
		if(ccc_hIrrigationSystemActive == 0)
		{
			ccc_hIrrigationSystemActive_pre = ccc_hIrrigationSystemActive;
			EventNotifyCNT--;
			indicate_handle_no_remove(IRRIGATIONSYSTEM_HANDLE_NUM_ACTIVE);
		}
		else
		{
			ccc_hIrrigationSystemActive_pre = ccc_hIrrigationSystemActive;
			EventNotifyCNT++;
			indicate_handle_no_reg(IRRIGATIONSYSTEM_HANDLE_NUM_ACTIVE);
		}
	}
	session_timeout_tick = clock_time () | 1;
	return 1;
}

int ccc_IrrigationSystemProgramModeWriteCB(void *pp)
{
	static u16 ccc_hIrrigationSystemProgramMode_pre = 0;

	if(ccc_hIrrigationSystemProgramMode_pre != ccc_hIrrigationSystemProgramMode)
	{
		if(ccc_hIrrigationSystemProgramMode == 0)
		{
			ccc_hIrrigationSystemProgramMode_pre = ccc_hIrrigationSystemProgramMode;
			EventNotifyCNT--;
			indicate_handle_no_remove(IRRIGATIONSYSTEM_HANDLE_NUM_PROGRAMMODE);
		}
		else
		{
			ccc_hIrrigationSystemProgramMode_pre = ccc_hIrrigationSystemProgramMode;
			EventNotifyCNT++;
			indicate_handle_no_reg(IRRIGATIONSYSTEM_HANDLE_NUM_PROGRAMMODE);
		}
	}
	session_timeout_tick = clock_time () | 1;
	return 1;
}

int ccc_IrrigationSystemInUseWriteCB(void *pp)
{
	static u16 ccc_hIrrigationSystemInUse_pre = 0;

	if(ccc_hIrrigationSystemInUse_pre != ccc_hIrrigationSystemInUse)
	{
		if(ccc_hIrrigationSystemInUse == 0)
		{
			ccc_hIrrigationSystemInUse_pre = ccc_hIrrigationSystemInUse;
			EventNotifyCNT--;
			indicate_handle_no_remove(IRRIGATIONSYSTEM_HANDLE_NUM_INUSE);
		}
		else
		{
			ccc_hIrrigationSystemInUse_pre = ccc_hIrrigationSystemInUse;
			EventNotifyCNT++;
			indicate_handle_no_reg(IRRIGATIONSYSTEM_HANDLE_NUM_INUSE);
		}
	}
	session_timeout_tick = clock_time () | 1;
	return 1;
}

int ccc_IrrigationSystemRemainingDurationWriteCB(void *pp)
{
	static u16 ccc_hIrrigationSystemRemainingDuration_pre = 0;

	if(ccc_hIrrigationSystemRemainingDuration_pre != ccc_hIrrigationSystemRemainingDuration)
	{
		if(ccc_hIrrigationSystemRemainingDuration == 0)
		{
			ccc_hIrrigationSystemRemainingDuration_pre = ccc_hIrrigationSystemRemainingDuration;
			EventNotifyCNT--;
			indicate_handle_no_remove(IRRIGATIONSYSTEM_HANDLE_NUM_REMAININGDURATION);
		}
		else
		{
			ccc_hIrrigationSystemRemainingDuration_pre = ccc_hIrrigationSystemRemainingDuration;
			EventNotifyCNT++;
			indicate_handle_no_reg(IRRIGATIONSYSTEM_HANDLE_NUM_REMAININGDURATION);
		}
	}
	session_timeout_tick = clock_time () | 1;
	return 1;
}

int ccc_IrrigationSystemStatusFaultWriteCB(void *pp)
{
	static u16 ccc_hIrrigationSystemStatusFault_pre = 0;

	if(ccc_hIrrigationSystemStatusFault_pre != ccc_hIrrigationSystemStatusFault)
	{
		if(ccc_hIrrigationSystemStatusFault == 0)
		{
			ccc_hIrrigationSystemStatusFault_pre = ccc_hIrrigationSystemStatusFault;
			EventNotifyCNT--;
			indicate_handle_no_remove(IRRIGATIONSYSTEM_HANDLE_NUM_STATUSFAULT);
		}
		else
		{
			ccc_hIrrigationSystemStatusFault_pre = ccc_hIrrigationSystemStatusFault;
			EventNotifyCNT++;
			indicate_handle_no_reg(IRRIGATIONSYSTEM_HANDLE_NUM_STATUSFAULT);
		}
	}
	session_timeout_tick = clock_time () | 1;
	return 1;
}

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
