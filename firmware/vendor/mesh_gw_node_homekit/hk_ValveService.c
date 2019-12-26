/********************************************************************************************************
 * @file     hk_ValveService.c
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
#define _HK_VALVE_SERVICE_C_
 
 
/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "hk_ValveService.h"
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
unsigned char hValveActive = 0;
unsigned char hValveInUse = 0;
unsigned char hValveValveType = 0;
unsigned int hValveSetDuration = 0;
unsigned int hValveRemainingDuration = 0;
unsigned char hValveIsConfigured = 0;
unsigned char hValveServiceLabelIndex = 1;
unsigned char hValveStatusFault = 0;

unsigned char VAR_VALVE_UNKOWN=0;

unsigned short ccc_hValveActive = 0;
unsigned short ccc_hValveInUse = 0;
unsigned short ccc_hValveValveType = 0;
unsigned short ccc_hValveSetDuration = 0;
unsigned short ccc_hValveRemainingDuration = 0;
unsigned short ccc_hValveIsConfigured = 0;
unsigned short ccc_hValveStatusFault = 0;


#if VALVE_ACTIVE_BRC_ENABLE
    unsigned char bc_para_hValveActive = 1;
#endif

#if VALVE_INUSE_BRC_ENABLE
    unsigned char bc_para_hValveInUse = 1;
#endif

#if VALVE_VALVETYPE_BRC_ENABLE
    unsigned char bc_para_hValveValveType = 1;
#endif

#if VALVE_SETDURATION_BRC_ENABLE
    unsigned char bc_para_hValveSetDuration = 1;
#endif

#if VALVE_REMAININGDURATION_BRC_ENABLE
    unsigned char bc_para_hValveRemainingDuration = 1;
#endif

#if VALVE_ISCONFIGURED_BRC_ENABLE
    unsigned char bc_para_hValveIsConfigured = 1;
#endif

#if VALVE_STATUSFAULT_BRC_ENABLE
    unsigned char bc_para_hValveStatusFault = 1;
#endif


const u8 hValveServiceName[12] = "ValveService";


const u8 VALVE_ACTIVE_VALID_RANGE[2] = {0x00, 0x01};
const u8 VALVE_INUSE_VALID_RANGE[2] = {0x00, 0x01};
const u8 VALVE_VALVETYPE_VALID_RANGE[2] = {0x00, 0x03};
const u8 VALVE_SETDURATION_VALID_RANGE[8] = {0x00, 0x00, 0x00, 0x00, 0x10, 0x0E, 0x00, 0x00};  /* 0~3600 */
const u8 VALVE_REMAININGDURATION_VALID_RANGE[8] = {0x00, 0x00, 0x00, 0x00, 0x10, 0x0E, 0x00, 0x00};  /* 0~3600 */
const u8 VALVE_ISCONFIGURED_VALID_RANGE[2] = {0x00, 0x01};
const u8 VALVE_SERVICELABELINDEX_VALID_RANGE[2] = { 0x01, 0xFF};
const u8 VALVE_STATUSFAULT_VALID_RANGE[2] = {0x00, 0x01};

const u8 VALVE_ACTIVE_VALID_STEP = 0x01;
const u8 VALVE_INUSE_VALID_STEP = 0x01;
const u8 VALVE_VALVETYPE_VALID_STEP = 0x01;
const u8 VALVE_SETDURATION_VALID_STEP[4] = {0x01, 0x00, 0x00, 0x00};
const u8 VALVE_REMAININGDURATION_VALID_STEP[4] = {0x01, 0x00, 0x00, 0x00};
const u8 VALVE_ISCONFIGURED_VALID_STEP = 0x01;
const u8 VALVE_SERVICELABELINDEX_VALID_STEP = 0x01;
const u8 VALVE_STATUSFAULT_VALID_STEP = 0x01;

const hap_characteristic_desc_t hap_desc_char_VALVE_ACTIVE_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALVE_ACTIVE_VALID_RANGE, &VALVE_ACTIVE_VALID_STEP, BC_PARA_VALVE_ACTIVE};
const hap_characteristic_desc_t hap_desc_char_VALVE_INUSE_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALVE_INUSE_VALID_RANGE, &VALVE_INUSE_VALID_STEP, BC_PARA_VALVE_INUSE};
const hap_characteristic_desc_t hap_desc_char_VALVE_VALVETYPE_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALVE_VALVETYPE_VALID_RANGE, &VALVE_VALVETYPE_VALID_STEP, BC_PARA_VALVE_VALVETYPE};
const hap_characteristic_desc_t hap_desc_char_VALVE_SETDURATION_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_UINT32, 0, 8, 4, 0, 0, VALVE_SETDURATION_VALID_RANGE, VALVE_SETDURATION_VALID_STEP, BC_PARA_VALVE_SETDURATION};
const hap_characteristic_desc_t hap_desc_char_VALVE_REMAININGDURATION_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT32, 0, 8, 4, 0, 0, VALVE_REMAININGDURATION_VALID_RANGE, VALVE_REMAININGDURATION_VALID_STEP, BC_PARA_VALVE_REMAININGDURATION};
const hap_characteristic_desc_t hap_desc_char_VALVE_ISCONFIGURED_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALVE_ISCONFIGURED_VALID_RANGE, &VALVE_ISCONFIGURED_VALID_STEP, BC_PARA_VALVE_ISCONFIGURED};
const hap_characteristic_desc_t hap_desc_char_VALVE_SERVICELABELINDEX_R_S = {hap_SecureR_10, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALVE_SERVICELABELINDEX_VALID_RANGE, &VALVE_SERVICELABELINDEX_VALID_STEP, 0};
const hap_characteristic_desc_t hap_desc_char_VALVE_STATUSFAULT_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALVE_STATUSFAULT_VALID_RANGE, &VALVE_STATUSFAULT_VALID_STEP, BC_PARA_VALVE_STATUSFAULT};

const hap_characteristic_desc_t hap_desc_char_valve_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if VALVE_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * hValve_linked_svcs[] = {};
#endif

#if(VALVE_IS_HIDDEN_SERVICE == 1)
const hap_service_desc_t hap_desc_service_valve_A5_R_S = {VALVE_INST_NO+1, (0x02 + VALVE_AS_PRIMARY_SERVICE), VALVE_LINKED_SVCS_LENGTH, VALVE_LINKED_SVCS};
#else
const hap_service_desc_t hap_desc_service_valve_A5_R_S = {VALVE_INST_NO+1, (0x00 + VALVE_AS_PRIMARY_SERVICE), VALVE_LINKED_SVCS_LENGTH, VALVE_LINKED_SVCS};
#endif


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
int ValveActiveReadCallback(void *pp)
{
	session_timeout_tick = clock_time () | 1;
	return 1;
}

int ValveActiveWriteCallback(void *pp)
{
	blt_adv_set_global_state ();

	if (hap_char_boundary_check()) {
		assign_current_flash_char_value_addr();
	#if VALVE_SERVICE_ENABLE
		VALVE_STORE_CHAR_VALUE_T_ACTIVE_RESTORE
	#endif
	} else {
		store_char_value_to_flash();
	}

	session_timeout_tick = clock_time () | 1;
	return 1;
}

int ValveInUseReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ValveValveTypeReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ValveSetDurationReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ValveSetDurationWriteCallback(void *pp)
{
	blt_adv_set_global_state ();

	if (hap_char_boundary_check()) {
		assign_current_flash_char_value_addr();
	#if VALVE_SERVICE_ENABLE
		VALVE_STORE_CHAR_VALUE_T_SETDURATION_RESTORE
	#endif
	} else {
		store_char_value_to_flash();
	}

	session_timeout_tick = clock_time () | 1;
	return 1;
}

int ValveRemainingDurationReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ValveIsConfiguredReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ValveIsConfiguredWriteCallback(void *pp)
{
	blt_adv_set_global_state ();

	if (hap_char_boundary_check()) {
		assign_current_flash_char_value_addr();
	#if VALVE_SERVICE_ENABLE
		VALVE_STORE_CHAR_VALUE_T_ISCONFIGURED_RESTORE
	#endif
	} else {
		store_char_value_to_flash();
	}

	session_timeout_tick = clock_time () | 1;
	return 1;
}

int ValveServiceLabelIndexReadCallback(void *pp)
{
	session_timeout_tick = clock_time () | 1;
	return 1;
}


int ValveStatusFaultReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}


int ccc_ValveActiveWriteCB(void *pp)
{
	static u16 ccc_hValveActive_pre = 0;

	if(ccc_hValveActive_pre != ccc_hValveActive)
	{
		if(ccc_hValveActive == 0)
		{
			ccc_hValveActive_pre = ccc_hValveActive;
			EventNotifyCNT--;
			indicate_handle_no_remove(VALVE_HANDLE_NUM_ACTIVE);
		}
		else
		{
			ccc_hValveActive_pre = ccc_hValveActive;
			EventNotifyCNT++;
			indicate_handle_no_reg(VALVE_HANDLE_NUM_ACTIVE);
		}
	}
	session_timeout_tick = clock_time () | 1;
	return 1;
}

int ccc_ValveInUseWriteCB(void *pp)
{
	static u16 ccc_hValveInUse_pre = 0;

	if(ccc_hValveInUse_pre != ccc_hValveInUse)
	{
		if(ccc_hValveInUse == 0)
		{
			ccc_hValveInUse_pre = ccc_hValveInUse;
			EventNotifyCNT--;
			indicate_handle_no_remove(VALVE_HANDLE_NUM_INUSE);
		}
		else
		{
			ccc_hValveInUse_pre = ccc_hValveInUse;
			EventNotifyCNT++;
			indicate_handle_no_reg(VALVE_HANDLE_NUM_INUSE);
		}
	}
	session_timeout_tick = clock_time () | 1;
	return 1;
}

int ccc_ValveValveTypeWriteCB(void *pp)
{
	static u16 ccc_hValveValveType_pre = 0;

	if(ccc_hValveValveType_pre != ccc_hValveValveType)
	{
		if(ccc_hValveValveType == 0)
		{
			ccc_hValveValveType_pre = ccc_hValveValveType;
			EventNotifyCNT--;
			indicate_handle_no_remove(VALVE_HANDLE_NUM_VALVETYPE);
		}
		else
		{
			ccc_hValveValveType_pre = ccc_hValveValveType;
			EventNotifyCNT++;
			indicate_handle_no_reg(VALVE_HANDLE_NUM_VALVETYPE);
		}
	}
	session_timeout_tick = clock_time () | 1;
	return 1;
}

int ccc_ValveSetDurationWriteCB(void *pp)
{
	static u16 ccc_hValveSetDuration_pre = 0;

	if(ccc_hValveSetDuration_pre != ccc_hValveSetDuration)
	{
		if(ccc_hValveSetDuration == 0)
		{
			ccc_hValveSetDuration_pre = ccc_hValveSetDuration;
			EventNotifyCNT--;
			indicate_handle_no_remove(VALVE_HANDLE_NUM_SETDURATION);
		}
		else
		{
			ccc_hValveSetDuration_pre = ccc_hValveSetDuration;
			EventNotifyCNT++;
			indicate_handle_no_reg(VALVE_HANDLE_NUM_SETDURATION);
		}
	}
	session_timeout_tick = clock_time () | 1;
	return 1;
}

int ccc_ValveRemainingDurationWriteCB(void *pp)
{
	static u16 ccc_hValveRemainingDuration_pre = 0;

	if(ccc_hValveRemainingDuration_pre != ccc_hValveRemainingDuration)
	{
		if(ccc_hValveRemainingDuration == 0)
		{
			ccc_hValveRemainingDuration_pre = ccc_hValveRemainingDuration;
			EventNotifyCNT--;
			indicate_handle_no_remove(VALVE_HANDLE_NUM_REMAININGDURATION);
		}
		else
		{
			ccc_hValveRemainingDuration_pre = ccc_hValveRemainingDuration;
			EventNotifyCNT++;
			indicate_handle_no_reg(VALVE_HANDLE_NUM_REMAININGDURATION);
		}
	}
	session_timeout_tick = clock_time () | 1;
	return 1;
}

int ccc_ValveIsConfiguredWriteCB(void *pp)
{
	static u16 ccc_hValveIsConfigured_pre = 0;

	if(ccc_hValveIsConfigured_pre != ccc_hValveIsConfigured)
	{
		if(ccc_hValveIsConfigured == 0)
		{
			ccc_hValveIsConfigured_pre = ccc_hValveIsConfigured;
			EventNotifyCNT--;
			indicate_handle_no_remove(VALVE_HANDLE_NUM_ISCONFIGURED);
		}
		else
		{
			ccc_hValveIsConfigured_pre = ccc_hValveIsConfigured;
			EventNotifyCNT++;
			indicate_handle_no_reg(VALVE_HANDLE_NUM_ISCONFIGURED);
		}
	}
	session_timeout_tick = clock_time () | 1;
	return 1;
}

int ccc_ValveStatusFaultWriteCB(void *pp)
{
	static u16 ccc_hValveStatusFault_pre = 0;

	if(ccc_hValveStatusFault_pre != ccc_hValveStatusFault)
	{
		if(ccc_hValveStatusFault == 0)
		{
			ccc_hValveStatusFault_pre = ccc_hValveStatusFault;
			EventNotifyCNT--;
			indicate_handle_no_remove(VALVE_HANDLE_NUM_STATUSFAULT);
		}
		else
		{
			ccc_hValveStatusFault_pre = ccc_hValveStatusFault;
			EventNotifyCNT++;
			indicate_handle_no_reg(VALVE_HANDLE_NUM_STATUSFAULT);
		}
	}
	session_timeout_tick = clock_time () | 1;
	return 1;
}

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
