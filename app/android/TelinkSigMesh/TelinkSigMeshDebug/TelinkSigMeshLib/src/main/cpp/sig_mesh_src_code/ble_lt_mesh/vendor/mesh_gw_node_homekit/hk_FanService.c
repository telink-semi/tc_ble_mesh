/********************************************************************************************************
 * @file     hk_FanService.c
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
#define _HK_FAN_SERVICE_C_
 
 
/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "hk_FanService.h"
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
unsigned char hFanOn = 0;
unsigned int hFanRotationDirect = 0;
unsigned int hFanRotationSpeed = 0;
unsigned char VAR_FAN_UNKOWN=0;

const u8 hFanServiceName[10] = "FanService";

unsigned short ccc_hFanOn = 0;
unsigned short ccc_hFanRotationDirect = 0;
unsigned short ccc_hFanRotationSpeed = 0;

#if FAN_ON_BRC_ENABLE
    unsigned char bc_para_hFanOn = 1;
#endif

#if FAN_ROTATIONDIRECT_BRC_ENABLE
    unsigned char bc_para_hFanRotationDirect = 1;
#endif

#if FAN_ROTATIONSPEED_BRC_ENABLE
    unsigned char bc_para_hFanRotationSpeed = 1;
#endif

const u8 ROTATION_DIRECTION_VALID_RANGE[8] = { 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
const u8 ROTATION_SPEED_VALID_RANGE[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC8, 0x42};

const u8 ROTATION_DIRECTION_VALID_STEP[4] = { 0x01, 0x00, 0x00, 0x00};
const u8 ROTATION_SPEED_VALID_STEP[4] = { 0x00, 0x00, 0x80, 0x3F};

const hap_characteristic_desc_t hap_desc_char_FAN_ON_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_BOOL_UNITLESS, 0, 0, 0, 0, 0, 0, 0, BC_PARA_FAN_ON};
const hap_characteristic_desc_t hap_desc_char_ROTATION_DIRECTION_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_INT32, 0, 8, 4, 0, 0, ROTATION_DIRECTION_VALID_RANGE, ROTATION_DIRECTION_VALID_STEP, BC_PARA_FAN_ROTATIONDIRECT};
const hap_characteristic_desc_t hap_desc_char_ROTATION_SPEED_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_FLOAT32_PERCENTAGE, 0, 8, 4, 0, 0, ROTATION_SPEED_VALID_RANGE, ROTATION_SPEED_VALID_STEP, BC_PARA_FAN_ROTATIONSPEED};

const hap_characteristic_desc_t hap_desc_char_fan_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if FAN_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * hFan_linked_svcs[] = {};
#endif

#if(FAN_IS_HIDDEN_SERVICE == 1)
const hap_service_desc_t hap_desc_service_fan_A5_R_S = {FAN_INST_NO+1, (0x02 + FAN_AS_PRIMARY_SERVICE), FAN_LINKED_SVCS_LENGTH, FAN_LINKED_SVCS};
#else
const hap_service_desc_t hap_desc_service_fan_A5_R_S = {FAN_INST_NO+1, (0x00 + FAN_AS_PRIMARY_SERVICE), FAN_LINKED_SVCS_LENGTH, FAN_LINKED_SVCS};
#endif

/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
int FanOnReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int FanOnWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~1
        assign_current_flash_char_value_addr();
    #if FAN_SERVICE_ENABLE
        FAN_STORE_CHAR_VALUE_T_FANON_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hFanRotationDirectReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hFanRotationDirectWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~1
        assign_current_flash_char_value_addr();
    #if FAN_SERVICE_ENABLE
        FAN_STORE_CHAR_VALUE_T_CHAR_FANROTATIONDIRECT_ENABLE_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hFanRotationSpeedReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hFanRotationSpeedWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~100
        assign_current_flash_char_value_addr();
    #if FAN_SERVICE_ENABLE
        FAN_STORE_CHAR_VALUE_T_CHAR_FANROTATIONSPEED_ENABLE_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hFanOnWriteCB(void *pp)
{
    static u16 ccc_hFanOn_pre = 0;

    if(ccc_hFanOn_pre != ccc_hFanOn)
    {
        if(ccc_hFanOn == 0)
        {
            ccc_hFanOn_pre = ccc_hFanOn;
            EventNotifyCNT--;
            indicate_handle_no_remove(FAN_HANDLE_NUM_HFANON);
        }
        else
        {
            ccc_hFanOn_pre = ccc_hFanOn;
            EventNotifyCNT++;
            indicate_handle_no_reg(FAN_HANDLE_NUM_HFANON);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hFanRotationDirectWriteCB(void *pp)
{
    static u16 ccc_hFanRotationDirect_pre = 0;

    if(ccc_hFanRotationDirect_pre != ccc_hFanRotationDirect)
    {
        if(ccc_hFanRotationDirect == 0)
        {
            ccc_hFanRotationDirect_pre = ccc_hFanRotationDirect;
            EventNotifyCNT--;
            indicate_handle_no_remove(FAN_HANDLE_NUM_HFANROTATIONDIRECT);
        }
        else
        {
            ccc_hFanRotationDirect_pre = ccc_hFanRotationDirect;
            EventNotifyCNT++;
            indicate_handle_no_reg(FAN_HANDLE_NUM_HFANROTATIONDIRECT);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hFanRotationSpeedWriteCB(void *pp)
{
    static u16 ccc_hFanRotationSpeed_pre = 0;

    if(ccc_hFanRotationSpeed_pre != ccc_hFanRotationSpeed)
    {
        if(ccc_hFanRotationSpeed == 0)
        {
            ccc_hFanRotationSpeed_pre = ccc_hFanRotationSpeed;
            EventNotifyCNT--;
            indicate_handle_no_remove(FAN_HANDLE_NUM_HFANROTATIONSPEED);
        }
        else
        {
            ccc_hFanRotationSpeed_pre = ccc_hFanRotationSpeed;
            EventNotifyCNT++;
            indicate_handle_no_reg(FAN_HANDLE_NUM_HFANROTATIONSPEED);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
