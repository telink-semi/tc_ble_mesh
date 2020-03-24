/********************************************************************************************************
 * @file     hk_BatteryService.c
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
#define _HK_BATTERY_SERVICE_C_


/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "hk_BatteryService.h"
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
unsigned char hBatteryLevel = 0;			// 1
unsigned char hBatteryChargingState = 0;	// 2
unsigned char hBatteryStateLowBattery = 0;	// 3
unsigned char VAR_BATTERY_UNKOWN=0;

unsigned short ccc_hBatteryLevel = 0;
unsigned short ccc_hBatteryChargingState = 0;
unsigned short ccc_hBatteryStateLowBattery = 0;

#if BATTERY_LEVEL_BRC_ENABLE
    unsigned char bc_para_hBatteryLevel = 1;
#endif

#if BATTERY_CHARGINGSTATE_BRC_ENABLE
    unsigned char bc_para_hBatteryChargingState = 1;
#endif

#if BATTERY_STATELOWBATTERY_BRC_ENABLE
    unsigned char bc_para_hBatteryStateLowBattery = 1;
#endif

const u8 hBatteryServiceName[14] = "BatteryService";

const u8 VALID_RANGE_hBatteryLevel[2]           = { 0x00, 0x64};
const u8 VALID_RANGE_hBatteryChargingState[2]   = { 0x00, 0x02};
const u8 VALID_RANGE_hBatteryStateLow[2]        = { 0x00, 0x01};

const u8 VALID_STEP_hBatteryLevel           = 0x01;
const u8 VALID_STEP_hBatteryChargingState   = 0x01;
const u8 VALID_STEP_hBatteryStateLow        = 0x01;

const hap_characteristic_desc_t hap_desc_char_hBatteryLevel_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8_PERCENTAGE, 0, 2, 1, 0, 0, VALID_RANGE_hBatteryLevel, &VALID_STEP_hBatteryLevel, BC_PARA_BATTERY_LEVEL};
const hap_characteristic_desc_t hap_desc_char_hBatteryChargingState_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hBatteryChargingState, &VALID_STEP_hBatteryChargingState, BC_PARA_BATTERY_CHARGINGSTATE};
const hap_characteristic_desc_t hap_desc_char_hBatteryStateLowBattery_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hBatteryStateLow, &VALID_STEP_hBatteryStateLow, BC_PARA_BATTERY_STATELOWBATTERY};

const hap_characteristic_desc_t hap_desc_char_battery_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if BATTERY_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * hBattery_linked_svcs[] = {};
#endif

#if(BATTERY_IS_HIDDEN_SERVICE == 1)
const hap_service_desc_t hap_desc_service_battery_A5_R_S = {BATTERY_INST_NO+1, (0x02 + BATTERY_AS_PRIMARY_SERVICE), BATTERY_LINKED_SVCS_LENGTH, BATTERY_LINKED_SVCS};
#else
const hap_service_desc_t hap_desc_service_battery_A5_R_S = {BATTERY_INST_NO+1, (0x00 + BATTERY_AS_PRIMARY_SERVICE), BATTERY_LINKED_SVCS_LENGTH, BATTERY_LINKED_SVCS};
#endif

/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
int hBatteryLevelReadCallback(void *pp)
{
    if ((hBatteryLevel <= 100) && (hBatteryLevel >= 0)) {  // 0~100
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if BATTERY_SERVICE_ENABLE
        BATTERY_STORE_CHAR_VALUE_T_BATTERYLEVEL_RESTORE
    #endif
    }

	session_timeout_tick = clock_time () | 1;
	return 1;
}

int hBatteryChargingStateReadCallback(void *pp)
{
    if ((hBatteryChargingState <= 2) && (hBatteryChargingState >= 0)) {  // 0~2
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if BATTERY_SERVICE_ENABLE
        BATTERY_STORE_CHAR_VALUE_T_BATTERYCHARGINGSTATE_RESTORE
    #endif
    }

	session_timeout_tick = clock_time () | 1;
	return 1;
}

int hBatteryStateLowBatteryReadCallback(void *pp)
{
    if ((hBatteryStateLowBattery <= 1) && (hBatteryStateLowBattery >= 0)) {  // 0~1
        store_char_value_to_flash();
        hBatteryStateLowBattery &= 1;
    } else {
        assign_current_flash_char_value_addr();
    #if BATTERY_SERVICE_ENABLE
        BATTERY_STORE_CHAR_VALUE_T_BATTERYSTATELOWBATTERY_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hBatteryLevelWriteCB(void *pp)
{
    static u16 ccc_hBatteryLevel_pre = 0;

    if(ccc_hBatteryLevel_pre != ccc_hBatteryLevel)
    {
        if(ccc_hBatteryLevel == 0)
        {
            ccc_hBatteryLevel_pre = ccc_hBatteryLevel;
            EventNotifyCNT--;
            indicate_handle_no_remove(BATTERY_HANDLE_NUM_LEVEL);
        }
        else
        {
            ccc_hBatteryLevel_pre = ccc_hBatteryLevel;
            EventNotifyCNT++;
            indicate_handle_no_reg(BATTERY_HANDLE_NUM_LEVEL);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hBatteryChargingStateWriteCB(void *pp)
{
    static u16 ccc_hBatteryChargingState_pre = 0;

    if(ccc_hBatteryChargingState_pre != ccc_hBatteryChargingState)
    {
        if(ccc_hBatteryChargingState == 0)
        {
            ccc_hBatteryChargingState_pre = ccc_hBatteryChargingState;
            EventNotifyCNT--;
            indicate_handle_no_remove(BATTERY_HANDLE_NUM_CHARGINGSTATE);
        }
        else
        {
            ccc_hBatteryChargingState_pre = ccc_hBatteryChargingState;
            EventNotifyCNT++;
            indicate_handle_no_reg(BATTERY_HANDLE_NUM_CHARGINGSTATE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hBatteryStateLowBatteryWriteCB(void *pp)
{
    static u16 ccc_hBatteryStateLowBattery_pre = 0;

    if(ccc_hBatteryStateLowBattery_pre != ccc_hBatteryStateLowBattery)
    {
        if(ccc_hBatteryStateLowBattery == 0)
        {
            ccc_hBatteryStateLowBattery_pre = ccc_hBatteryStateLowBattery;
            EventNotifyCNT--;
            indicate_handle_no_remove(BATTERY_HANDLE_NUM_STATUSLOWBATTERY);
        }
        else
        {
            ccc_hBatteryStateLowBattery_pre = ccc_hBatteryStateLowBattery;
            EventNotifyCNT++;
            indicate_handle_no_reg(BATTERY_HANDLE_NUM_STATUSLOWBATTERY);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}


/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
