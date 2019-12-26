/********************************************************************************************************
 * @file     hk_SecurityService.c
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
#define _HK_SECURITY_SERVICE_C_
 
 
/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "hk_SecurityService.h"
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
unsigned char hSecurityCurrentState = 0;    // 1
unsigned char hSecurityTargetState = 0;     // 2
unsigned char hSecurityAlarmType = 0;       // 4
unsigned char hSecurityStatusFault = 0;     // 5
unsigned char hSecurityStatusTampered = 0;  // 6

unsigned char VAR_SECURITY_UNKOWN=0;

unsigned short ccc_hSecurityCurrentState = 0;
unsigned short ccc_hSecurityTargetState = 0;
unsigned short ccc_hSecurityAlarmType = 0;
unsigned short ccc_hSecurityStatusFault = 0;
unsigned short ccc_hSecurityStatusTampered = 0;

#if SECURITY_CURRENTSTATE_BRC_ENABLE
    unsigned char bc_para_hSecurityCurrentState = 1;
#endif

#if SECURITY_TARGETSTATE_BRC_ENABLE
    unsigned char bc_para_hSecurityTargetState = 1;
#endif

#if SECURITY_ALARMTYPE_BRC_ENABLE
    unsigned char bc_para_hSecurityAlarmType = 1;
#endif

#if SECURITY_STATUSFAULT_BRC_ENABLE
    unsigned char bc_para_hSecurityStatusFault = 1;
#endif

#if SECURITY_STATUSTAMPERED_BRC_ENABLE
    unsigned char bc_para_hSecurityStatusTampered = 1;
#endif

const u8 hSecurityServiceName[15] = "SecurityService";

const u8 VALID_RANGE_hSecurityCurrentState[2]   = { 0x00, 0x04};
const u8 VALID_RANGE_hSecurityTargetState[2]    = { 0x00, 0x03};
const u8 VALID_RANGE_hSecurityAlarmType[2]      = { 0x00, 0x01};
const u8 VALID_RANGE_hSecurityStatusFault[2]    = { 0x00, 0x01};
const u8 VALID_RANGE_hSecurityStatusTampered[2] = { 0x00, 0x01};

const u8 VALID_STEP_hSecurityCurrentState   = 0x01;
const u8 VALID_STEP_hSecurityTargetState    = 0x01;
const u8 VALID_STEP_hSecurityAlarmType      = 0x01;
const u8 VALID_STEP_hSecurityStatusFault    = 0x01;
const u8 VALID_STEP_hSecurityStatusTampered = 0x01;

const hap_characteristic_desc_t hap_desc_char_hSecurityCurrentState_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSecurityCurrentState, &VALID_STEP_hSecurityCurrentState, BC_PARA_SECURITY_CURRENTSTATE};
const hap_characteristic_desc_t hap_desc_char_hSecurityTargetState_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSecurityTargetState, &VALID_STEP_hSecurityTargetState, BC_PARA_SECURITY_TARGETSTATE};
const hap_characteristic_desc_t hap_desc_char_hSecurityAlarmType_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSecurityAlarmType, &VALID_STEP_hSecurityAlarmType, BC_PARA_SECURITY_ALARMTYPE};
const hap_characteristic_desc_t hap_desc_char_hSecurityStatusFault_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSecurityStatusFault, &VALID_STEP_hSecurityStatusFault, BC_PARA_SECURITY_STATUSFAULT};
const hap_characteristic_desc_t hap_desc_char_hSecurityStatusTampered_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSecurityStatusTampered, &VALID_STEP_hSecurityStatusTampered, BC_PARA_SECURITY_STATUSTAMPERED};

const hap_characteristic_desc_t hap_desc_char_security_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if SECURITY_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * hSecurity_linked_svcs[] = {};
#endif

#if(SECURITY_IS_HIDDEN_SERVICE == 1)
const hap_service_desc_t hap_desc_service_security_A5_R_S = {SECURITY_INST_NO+1, (0x02 + SECURITY_AS_PRIMARY_SERVICE), SECURITY_LINKED_SVCS_LENGTH, SECURITY_LINKED_SVCS};
#else
const hap_service_desc_t hap_desc_service_security_A5_R_S = {SECURITY_INST_NO+1, (0x00 + SECURITY_AS_PRIMARY_SERVICE), SECURITY_LINKED_SVCS_LENGTH, SECURITY_LINKED_SVCS};
#endif


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
int hSecurityCurrentStateReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSecurityTargetStateReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSecurityTargetStateWriteCallback(void *pp)
{
	blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~3
        assign_current_flash_char_value_addr();
    #if SECURITY_SERVICE_ENABLE
        SECURITY_STORE_CHAR_VALUE_T_SECURITYTARGETSTATE_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
        hSecurityTargetState = hSecurityTargetState & 0x3;
        hSecurityCurrentState = hSecurityTargetState;   // update to Current state
    }

	session_timeout_tick = clock_time () | 1;
	return 1;
}

int hSecurityAlarmTypeReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSecurityStatusFaultReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSecurityStatusTamperedReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSecurityCurrentStateWriteCB(void *pp)
{
    static u16 ccc_hSecurityCurrentState_pre = 0;

    if(ccc_hSecurityCurrentState_pre != ccc_hSecurityCurrentState)
    {
        if(ccc_hSecurityCurrentState == 0)
        {
            ccc_hSecurityCurrentState_pre = ccc_hSecurityCurrentState;
            EventNotifyCNT--;
            indicate_handle_no_remove(SECURITY_HANDLE_NUM_CURRENTSTATE);
        }
        else
        {
            ccc_hSecurityCurrentState_pre = ccc_hSecurityCurrentState;
            EventNotifyCNT++;
            indicate_handle_no_reg(SECURITY_HANDLE_NUM_CURRENTSTATE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSecurityTargetStateWriteCB(void *pp)
{
    static u16 ccc_hSecurityTargetState_pre = 0;

    if(ccc_hSecurityTargetState_pre != ccc_hSecurityTargetState)
    {
        if(ccc_hSecurityTargetState == 0)
        {
            ccc_hSecurityTargetState_pre = ccc_hSecurityTargetState;
            EventNotifyCNT--;
            indicate_handle_no_remove(SECURITY_HANDLE_NUM_TARGETSTATE);
        }
        else
        {
            ccc_hSecurityTargetState_pre = ccc_hSecurityTargetState;
            EventNotifyCNT++;
            indicate_handle_no_reg(SECURITY_HANDLE_NUM_TARGETSTATE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSecurityAlarmTypeWriteCB(void *pp)
{
    static u16 ccc_hSecurityAlarmType_pre = 0;

    if(ccc_hSecurityAlarmType_pre != ccc_hSecurityAlarmType)
    {
        if(ccc_hSecurityAlarmType == 0)
        {
            ccc_hSecurityAlarmType_pre = ccc_hSecurityAlarmType;
            EventNotifyCNT--;
            indicate_handle_no_remove(SECURITY_HANDLE_NUM_ALARMTYPE);
        }
        else
        {
            ccc_hSecurityAlarmType_pre = ccc_hSecurityAlarmType;
            EventNotifyCNT++;
            indicate_handle_no_reg(SECURITY_HANDLE_NUM_ALARMTYPE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSecurityStatusFaultWriteCB(void *pp)
{
    static u16 ccc_hSecurityStatusFault_pre = 0;

    if(ccc_hSecurityStatusFault_pre != ccc_hSecurityStatusFault)
    {
        if(ccc_hSecurityStatusFault == 0)
        {
            ccc_hSecurityStatusFault_pre = ccc_hSecurityStatusFault;
            EventNotifyCNT--;
            indicate_handle_no_remove(SECURITY_HANDLE_NUM_STATUSFAULT);
        }
        else
        {
            ccc_hSecurityStatusFault_pre = ccc_hSecurityStatusFault;
            EventNotifyCNT++;
            indicate_handle_no_reg(SECURITY_HANDLE_NUM_STATUSFAULT);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSecurityStatusTamperedWriteCB(void *pp)
{
    static u16 ccc_hSecurityStatusTampered_pre = 0;

    if(ccc_hSecurityStatusTampered_pre != ccc_hSecurityStatusTampered)
    {
        if(ccc_hSecurityStatusTampered == 0)
        {
            ccc_hSecurityStatusTampered_pre = ccc_hSecurityStatusTampered;
            EventNotifyCNT--;
            indicate_handle_no_remove(SECURITY_HANDLE_NUM_STATUSTAMPERED);
        }
        else
        {
            ccc_hSecurityStatusTampered_pre = ccc_hSecurityStatusTampered;
            EventNotifyCNT++;
            indicate_handle_no_reg(SECURITY_HANDLE_NUM_STATUSTAMPERED);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
