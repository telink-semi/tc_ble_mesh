/********************************************************************************************************
 * @file     hk_SlatService.c
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
#define _HK_SLAT_SERVICE_C_
 
 
/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "hk_SlatService.h"
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
unsigned char hCurrentSlatState = 0;
unsigned char hSlatType = 0;
unsigned char hSlatSwingMode = 0;
int hSlatCurrentTiltAngle = 0;
int hSlatTargetTiltAngle = 0;

unsigned char VAR_SLAT_UNKOWN=0;

unsigned short ccc_hCurrentSlatState = 0;
unsigned short ccc_hSlatSwingMode = 0;
unsigned short ccc_hSlatCurrentTiltAngle = 0;
unsigned short ccc_hSlatTargetTiltAngle = 0;

#if SLAT_CURRENT_SLAT_STATE_BRC_ENABLE
    unsigned char bc_para_hCurrentSlatState = 1;
#endif

#if SLAT_SWING_MODE_BRC_ENABLE
    unsigned char bc_para_hSlatSwingMode = 1;
#endif

#if SLAT_CURRENT_TILT_ANGLE_BRC_ENABLE
    unsigned char bc_para_hSlatCurrentTiltAngle = 1;
#endif

#if SLAT_TARGET_TILT_ANGLE_BRC_ENABLE
    unsigned char bc_para_hSlatTargetTiltAngle = 1;
#endif


const u8 hSlatServiceName[11] = "SlatService";

const u8 CURRENT_SLAT_STATE_VALID_RANGE[2] = {0x00, 0x02};
const u8 SLAT_TYPE_VALID_RANGE[2] = {0x00, 0x01};
const u8 SLAT_SWING_MODE_VALID_RANGE[2] = {0x00, 0x01};
const u8 SLAT_CURRENT_TILT_ANGLE_VALID_RANGE[8] = { 0xA6, 0xFF, 0xFF, 0xFF, 0x5A, 0x00, 0x00, 0x00};    // -90~90 (int)
const u8 SLAT_TARGET_TILT_ANGLE_VALID_RANGE[8] = { 0xA6, 0xFF, 0xFF, 0xFF, 0x5A, 0x00, 0x00, 0x00};    // -90~90 (int)

const u8 CURRENT_SLAT_STATE_VALID_STEP = 0x01;
const u8 SLAT_TYPE_VALID_STEP = 0x01;
const u8 SLAT_SWING_MODE_VALID_STEP = 0x01;
const u8 SLAT_CURRENT_TILT_ANGLE_VALID_STEP[4] = { 0x01, 0x00, 0x00, 0x00};
const u8 SLAT_TARGET_TILT_ANGLE_VALID_STEP[4] = { 0x01, 0x00, 0x00, 0x00};

const hap_characteristic_desc_t hap_desc_char_CurrentSlatState_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, CURRENT_SLAT_STATE_VALID_RANGE, &CURRENT_SLAT_STATE_VALID_STEP, BC_PARA_SLAT_CURRENT_SLAT_STATE};
const hap_characteristic_desc_t hap_desc_char_SlatType_R_S = {hap_SecureR_10, 0, CFG_UINT8, 0, 2, 1, 0, 0, SLAT_TYPE_VALID_RANGE, &SLAT_TYPE_VALID_STEP, 0};
const hap_characteristic_desc_t hap_desc_char_SlatSwingMode_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, SLAT_SWING_MODE_VALID_RANGE, &SLAT_SWING_MODE_VALID_STEP, BC_PARA_SLAT_SWING_MODE};
const hap_characteristic_desc_t hap_desc_char_SlatCurrentTiltAngle_R_S = {hap_PRead_NOTIFY, 0, CFG_INT32_ARCDEGREE, 0, 8, 4, 0, 0, SLAT_CURRENT_TILT_ANGLE_VALID_RANGE, SLAT_CURRENT_TILT_ANGLE_VALID_STEP, BC_PARA_SLAT_CURRENT_TILT_ANGLE};
const hap_characteristic_desc_t hap_desc_char_SlatTargetTiltAngle_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_INT32_ARCDEGREE, 0, 8, 4, 0, 0, SLAT_TARGET_TILT_ANGLE_VALID_RANGE, SLAT_TARGET_TILT_ANGLE_VALID_STEP, BC_PARA_SLAT_TARGET_TILT_ANGLE};

const hap_characteristic_desc_t hap_desc_char_slat_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if SLAT_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * hSlat_linked_svcs[] = {};
#endif

#if(SLAT_IS_HIDDEN_SERVICE == 1)
const hap_service_desc_t hap_desc_service_slat_A5_R_S = {SLAT_INST_NO+1, (0x02 + SLAT_AS_PRIMARY_SERVICE), SLAT_LINKED_SVCS_LENGTH, SLAT_LINKED_SVCS};
#else
const hap_service_desc_t hap_desc_service_slat_A5_R_S = {SLAT_INST_NO+1, (0x00 + SLAT_AS_PRIMARY_SERVICE), SLAT_LINKED_SVCS_LENGTH, SLAT_LINKED_SVCS};
#endif

/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
int CurrentSlatStateReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int SlatTypeReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int SlatSwingModeReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int SlatSwingModeWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~1
        assign_current_flash_char_value_addr();
	#if SLAT_SERVICE_ENABLE
		SLAT_STORE_CHAR_VALUE_T_HSLAT_SWING_MODE_RESTORE
	#endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int SlatCurrentTiltAngleReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int SlatTargetTiltAngleReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int SlatTargetTiltAngleWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~1
        assign_current_flash_char_value_addr();
	#if SLAT_SERVICE_ENABLE
		SLAT_STORE_CHAR_VALUE_T_HSLAT_TARGET_TILT_ANGLE_RESTORE
	#endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}


int ccc_hCurrentSlatStateWriteCB(void *pp)
{
    static u16 ccc_hCurrentSlatState_pre = 0;

    if(ccc_hCurrentSlatState_pre != ccc_hCurrentSlatState)
    {
        if(ccc_hCurrentSlatState == 0)
        {
            ccc_hCurrentSlatState_pre = ccc_hCurrentSlatState;
            EventNotifyCNT--;
            indicate_handle_no_remove(SLAT_HANDLE_NUM_HSLAT_CURRENT_SLAT_STATE);
        }
        else
        {
            ccc_hCurrentSlatState_pre = ccc_hCurrentSlatState;
            EventNotifyCNT++;
            indicate_handle_no_reg(SLAT_HANDLE_NUM_HSLAT_CURRENT_SLAT_STATE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSlatSwingModeWriteCB(void *pp)
{
    static u16 ccc_hSlatSwingMode_pre = 0;

    if(ccc_hSlatSwingMode_pre != ccc_hSlatSwingMode)
    {
        if(ccc_hSlatSwingMode == 0)
        {
            ccc_hSlatSwingMode_pre = ccc_hSlatSwingMode;
            EventNotifyCNT--;
            indicate_handle_no_remove(SLAT_HANDLE_NUM_HSLAT_SWING_MODE);
        }
        else
        {
            ccc_hSlatSwingMode_pre = ccc_hSlatSwingMode;
            EventNotifyCNT++;
            indicate_handle_no_reg(SLAT_HANDLE_NUM_HSLAT_SWING_MODE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSlatCurrentTiltAngleWriteCB(void *pp)
{
    static u16 ccc_hSlatCurrentTiltAngle_pre = 0;

    if(ccc_hSlatCurrentTiltAngle_pre != ccc_hSlatCurrentTiltAngle)
    {
        if(ccc_hSlatCurrentTiltAngle == 0)
        {
            ccc_hSlatCurrentTiltAngle_pre = ccc_hSlatCurrentTiltAngle;
            EventNotifyCNT--;
            indicate_handle_no_remove(SLAT_HANDLE_NUM_HSLAT_CURRENT_TILT_ANGLE);
        }
        else
        {
            ccc_hSlatCurrentTiltAngle_pre = ccc_hSlatCurrentTiltAngle;
            EventNotifyCNT++;
            indicate_handle_no_reg(SLAT_HANDLE_NUM_HSLAT_CURRENT_TILT_ANGLE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSlatTargetTiltAngleWriteCB(void *pp)
{
    static u16 ccc_hSlatTargetTiltAngle_pre = 0;

    if(ccc_hSlatTargetTiltAngle_pre != ccc_hSlatTargetTiltAngle)
    {
        if(ccc_hSlatTargetTiltAngle == 0)
        {
            ccc_hSlatTargetTiltAngle_pre = ccc_hSlatTargetTiltAngle;
            EventNotifyCNT--;
            indicate_handle_no_remove(SLAT_HANDLE_NUM_HSLAT_TARGET_TILT_ANGLE);
        }
        else
        {
            ccc_hSlatTargetTiltAngle_pre = ccc_hSlatTargetTiltAngle;
            EventNotifyCNT++;
            indicate_handle_no_reg(SLAT_HANDLE_NUM_HSLAT_TARGET_TILT_ANGLE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
