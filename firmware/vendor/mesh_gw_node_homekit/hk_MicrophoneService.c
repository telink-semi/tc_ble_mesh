/********************************************************************************************************
 * @file     hk_MicrophoneService.c
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
#define _HK_MICROPHONE_SERVICE_C_
 
 
/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "hk_MicrophoneService.h"
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
unsigned char hMicrophoneMute = 0;
unsigned char hMicrophoneVolume = 0;
unsigned char VAR_MICROPHONE_UNKOWN=0;


unsigned short ccc_hMicrophoneMute = 0;
unsigned short ccc_hMicrophoneVolume = 0;

#if MICROPHONE_MUTE_BRC_ENABLE
    unsigned char bc_para_hMicrophoneMute = 1;
#endif

#if MICROPHONE_VOLUME_BRC_ENABLE
    unsigned char bc_para_hMicrophoneVolume = 1;
#endif

const u8 hMicrophoneServiceName[17] = "MicrophoneService";

const hap_characteristic_desc_t hap_desc_char_hMicrophoneMute_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_BOOL_UNITLESS, 0, 0, 0, 0, 0, 0, 0, BC_PARA_MICROPHONE_MUTE};

#if 1 // follow HAP R9 Spec
    const u8 VALID_RANGE_hMicrophoneVolume[2] = { 0x00, 0x64};
    const u8 VALID_STEP_hMicrophoneVolume = 0x01;
    const hap_characteristic_desc_t hap_desc_char_hMicrophoneVolume_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_UINT8_PERCENTAGE, 0, 2, 1, 0, 0, VALID_RANGE_hMicrophoneVolume, &VALID_STEP_hMicrophoneVolume, BC_PARA_MICROPHONE_VOLUME};
#else // follow HAT 4.2.1 error message
    const u8 VALID_RANGE_hMicrophoneVolume[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC8, 0x42};  // 0~100 (float)
    const u8 VALID_STEP_hMicrophoneVolume[4] = { 0x00, 0x00, 0x80, 0x3F};
    const hap_characteristic_desc_t hap_desc_char_hMicrophoneVolume_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_FLOAT32_PERCENTAGE, 0, 8, 4, 0, 0, VALID_RANGE_hMicrophoneVolume, VALID_STEP_hMicrophoneVolume, BC_PARA_MICROPHONE_VOLUME};
#endif

const hap_characteristic_desc_t hap_desc_char_microphone_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if MICROPHONE_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * hMicrophone_linked_svcs[] = {};
#endif

#if(MICROPHONE_IS_HIDDEN_SERVICE == 1)
const hap_service_desc_t hap_desc_service_microphone_A5_R_S = {MICROPHONE_INST_NO+1, (0x02 + MICROPHONE_AS_PRIMARY_SERVICE), MICROPHONE_LINKED_SVCS_LENGTH, MICROPHONE_LINKED_SVCS};
#else
const hap_service_desc_t hap_desc_service_microphone_A5_R_S = {MICROPHONE_INST_NO+1, (0x00 + MICROPHONE_AS_PRIMARY_SERVICE), MICROPHONE_LINKED_SVCS_LENGTH, MICROPHONE_LINKED_SVCS};
#endif


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
int MicrophoneMuteReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int MicrophoneMuteWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~1
        assign_current_flash_char_value_addr();
    #if MICROPHONE_SERVICE_ENABLE
        MICROPHONE_STORE_CHAR_VALUE_T_MICROPHONEMUTE_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int MicrophoneVolumeReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int MicrophoneVolumeWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~100
        assign_current_flash_char_value_addr();
    #if MICROPHONE_SERVICE_ENABLE
        MICROPHONE_STORE_CHAR_VALUE_T_MICROPHONEVOLUME_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hMicrophoneMuteWriteCB(void *pp)
{
    static u16 ccc_hMicrophoneMute_pre = 0;

    if(ccc_hMicrophoneMute_pre != ccc_hMicrophoneMute)
    {
        if(ccc_hMicrophoneMute == 0)
        {
            ccc_hMicrophoneMute_pre = ccc_hMicrophoneMute;
            EventNotifyCNT--;
            indicate_handle_no_remove(MICROPHONE_HANDLE_NUM_MUTE);
        }
        else
        {
            ccc_hMicrophoneMute_pre = ccc_hMicrophoneMute;
            EventNotifyCNT++;
            indicate_handle_no_reg(MICROPHONE_HANDLE_NUM_MUTE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hMicrophoneVolumeWriteCB(void *pp)
{
    static u16 ccc_hMicrophoneVolume_pre = 0;

    if(ccc_hMicrophoneVolume_pre != ccc_hMicrophoneVolume)
    {
        if(ccc_hMicrophoneVolume == 0)
        {
            ccc_hMicrophoneVolume_pre = ccc_hMicrophoneVolume;
            EventNotifyCNT--;
            indicate_handle_no_remove(MICROPHONE_HANDLE_NUM_VOLUME);
        }
        else
        {
            ccc_hMicrophoneVolume_pre = ccc_hMicrophoneVolume;
            EventNotifyCNT++;
            indicate_handle_no_reg(MICROPHONE_HANDLE_NUM_VOLUME);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
