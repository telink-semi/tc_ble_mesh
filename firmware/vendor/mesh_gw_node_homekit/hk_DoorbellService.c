/********************************************************************************************************
 * @file     hk_DoorbellService.c
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
#define _HK_DOORBELL_SERVICE_C_
 
 
/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "hk_DoorbellService.h"
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
unsigned char hDoorbellProgSwEvent = 0;
unsigned char hDoorbellVolume = 0;
unsigned char hDoorbellBrightness = 0;
unsigned char VAR_DOORBELL_UNKOWN=0;

unsigned short ccc_hDoorbellProgSwEvent = 0;
unsigned short ccc_hDoorbellVolume = 0;
unsigned short ccc_hDoorbellBrightness = 0;

#if DOORBELL_PROGSWEVENT_BRC_ENABLE
    unsigned char bc_para_hDoorbellProgSwEvent = 1;
#endif

#if DOORBELL_VOLUME_BRC_ENABLE
    unsigned char bc_para_hDoorbellVolume = 1;
#endif

#if DOORBELL_BRIGHTNESS_BRC_ENABLE
    unsigned char bc_para_hDoorbellBrightness = 1;
#endif

const u8 hDoorbellServiceName[15] = "DoorbellService";

const u8 VALID_RANGE_hDoorbellProgSwEvent[2] = { 0x00, 0x01};
const u8 VALID_RANGE_hDoorbellVolume[2] = { 0x00, 0x64};
const u8 VALID_RANGE_hDoorbellBrightness[8] = { 0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00};    // 0~100 (int32)

const u8 VALID_STEP_hDoorbellProgSwEvent = 0x01;
const u8 VALID_STEP_hDoorbellVolume = 0x01;
const u8 VALID_STEP_hDoorbellBrightness[4]   = { 0x01, 0x00, 0x00, 0x00};

const hap_characteristic_desc_t hap_desc_char_hDoorbellProgSwEvent_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hDoorbellProgSwEvent, &VALID_STEP_hDoorbellProgSwEvent, BC_PARA_DOORBELL_PROGSWEVENT};
const hap_characteristic_desc_t hap_desc_char_hDoorbellVolume_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_UINT8_PERCENTAGE, 0, 2, 1, 0, 0, VALID_RANGE_hDoorbellVolume, &VALID_STEP_hDoorbellVolume, BC_PARA_DOORBELL_VOLUME};
const hap_characteristic_desc_t hap_desc_char_hDoorbellBrightness_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_INT32_PERCENTAGE, 0, 8, 4, 0, 0, VALID_RANGE_hDoorbellBrightness, VALID_STEP_hDoorbellBrightness, BC_PARA_DOORBELL_BRIGHTNESS};

const hap_characteristic_desc_t hap_desc_char_doorbell_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if DOORBELL_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * hDoorbell_linked_svcs[] = {};
#endif

#if(DOORBELL_IS_HIDDEN_SERVICE == 1)
const hap_service_desc_t hap_desc_service_doorbell_A5_R_S = {DOORBELL_INST_NO+1, (0x02 + DOORBELL_AS_PRIMARY_SERVICE), DOORBELL_LINKED_SVCS_LENGTH, DOORBELL_LINKED_SVCS};
#else
const hap_service_desc_t hap_desc_service_doorbell_A5_R_S = {DOORBELL_INST_NO+1, (0x00 + DOORBELL_AS_PRIMARY_SERVICE), DOORBELL_LINKED_SVCS_LENGTH, DOORBELL_LINKED_SVCS};
#endif

/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
int DoorbellProgSwEventReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int DoorbellVolumeReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int DoorbellVolumeWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~100
        assign_current_flash_char_value_addr();
    #if DOORBELL_SERVICE_ENABLE
        DOORBELL_STORE_CHAR_VALUE_T_DOORBELLVOLUME_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int DoorbellBrightnessReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int DoorbellBrightnessWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~100
        assign_current_flash_char_value_addr();
    #if DOORBELL_SERVICE_ENABLE
        DOORBELL_STORE_CHAR_VALUE_T_DOORBELLBRIGHTNESS_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hDoorbellProgSwEventWriteCB(void *pp)
{
    static u16 ccc_hDoorbellProgSwEvent_pre = 0;

    if(ccc_hDoorbellProgSwEvent_pre != ccc_hDoorbellProgSwEvent)
    {
        if(ccc_hDoorbellProgSwEvent == 0)
        {
            ccc_hDoorbellProgSwEvent_pre = ccc_hDoorbellProgSwEvent;
            EventNotifyCNT--;
            indicate_handle_no_remove(DOORBELL_HANDLE_NUM_PROGSWEVENT);
        }
        else
        {
            ccc_hDoorbellProgSwEvent_pre = ccc_hDoorbellProgSwEvent;
            EventNotifyCNT++;
            indicate_handle_no_reg(DOORBELL_HANDLE_NUM_PROGSWEVENT);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hDoorbellVolumeWriteCB(void *pp)
{
    static u16 ccc_hDoorbellVolume_pre = 0;

    if(ccc_hDoorbellVolume_pre != ccc_hDoorbellVolume)
    {
        if(ccc_hDoorbellVolume == 0)
        {
            ccc_hDoorbellVolume_pre = ccc_hDoorbellVolume;
            EventNotifyCNT--;
            indicate_handle_no_remove(DOORBELL_HANDLE_NUM_VOLUME);
        }
        else
        {
            ccc_hDoorbellVolume_pre = ccc_hDoorbellVolume;
            EventNotifyCNT++;
            indicate_handle_no_reg(DOORBELL_HANDLE_NUM_VOLUME);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hDoorbellBrightnessWriteCB(void *pp)
{
    static u16 ccc_hDoorbellBrightness_pre = 0;

    if(ccc_hDoorbellBrightness_pre != ccc_hDoorbellBrightness)
    {
        if(ccc_hDoorbellBrightness == 0)
        {
            ccc_hDoorbellBrightness_pre = ccc_hDoorbellBrightness;
            EventNotifyCNT--;
            indicate_handle_no_remove(DOORBELL_HANDLE_NUM_BRIGHTNESS);
        }
        else
        {
            ccc_hDoorbellBrightness_pre = ccc_hDoorbellBrightness;
            EventNotifyCNT++;
            indicate_handle_no_reg(DOORBELL_HANDLE_NUM_BRIGHTNESS);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
