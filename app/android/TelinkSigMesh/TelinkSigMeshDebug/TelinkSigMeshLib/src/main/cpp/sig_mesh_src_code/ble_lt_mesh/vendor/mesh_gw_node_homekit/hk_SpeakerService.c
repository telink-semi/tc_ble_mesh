/********************************************************************************************************
 * @file     hk_SpeakerService.c
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
#define _HK_SPEAKER_SERVICE_C_
 
 
/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "hk_SpeakerService.h"
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
unsigned char hSpeakerMute = 0;
unsigned char hSpeakerVolume = 0;
unsigned char VAR_SPEAKER_UNKOWN=0;

unsigned short ccc_hSpeakerMute = 0;
unsigned short ccc_hSpeakerVolume = 0;

#if SPEAKER_MUTE_BRC_ENABLE
    unsigned char bc_para_hSpeakerMute = 1;
#endif

#if SPEAKER_VOLUME_BRC_ENABLE
    unsigned char bc_para_hSpeakerVolume = 1;
#endif

const u8 hSpeakerServiceName[14] = "SpeakerService";

const hap_characteristic_desc_t hap_desc_char_hSpeakerMute_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_BOOL_UNITLESS, 0, 0, 0, 0, 0, 0, 0, BC_PARA_SPEAKER_MUTE};

#if 1 // follow HAP R9 Spec
    const u8 VALID_RANGE_hSpeakerVolume[2] = { 0x00, 0x64};
    const u8 VALID_STEP_hSpeakerVolume = 0x01;
    const hap_characteristic_desc_t hap_desc_char_hSpeakerVolume_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_UINT8_PERCENTAGE, 0, 2, 1, 0, 0, VALID_RANGE_hSpeakerVolume, &VALID_STEP_hSpeakerVolume, BC_PARA_SPEAKER_VOLUME};
#else // follow HAT 4.2.1 error message
    const u8 VALID_RANGE_hSpeakerVolume[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC8, 0x42};  // 0~100 (float)
    const u8 VALID_STEP_hSpeakerVolume[4] = { 0x00, 0x00, 0x80, 0x3F};
    const hap_characteristic_desc_t hap_desc_char_hSpeakerVolume_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_FLOAT32_PERCENTAGE, 0, 8, 4, 0, 0, VALID_RANGE_hSpeakerVolume, VALID_STEP_hSpeakerVolume, BC_PARA_SPEAKER_VOLUME};
#endif

const hap_characteristic_desc_t hap_desc_char_speaker_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if SPEAKER_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * hSpeaker_linked_svcs[] = {};
#endif

#if(SPEAKER_IS_HIDDEN_SERVICE == 1)
const hap_service_desc_t hap_desc_service_speaker_A5_R_S = {SPEAKER_INST_NO+1, (0x02 + SPEAKER_AS_PRIMARY_SERVICE), SPEAKER_LINKED_SVCS_LENGTH, SPEAKER_LINKED_SVCS};
#else
const hap_service_desc_t hap_desc_service_speaker_A5_R_S = {SPEAKER_INST_NO+1, (0x00 + SPEAKER_AS_PRIMARY_SERVICE), SPEAKER_LINKED_SVCS_LENGTH, SPEAKER_LINKED_SVCS};
#endif


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
int SpeakerMuteReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int SpeakerMuteWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~1
        assign_current_flash_char_value_addr();
    #if SPEAKER_SERVICE_ENABLE
        SPEAKER_STORE_CHAR_VALUE_T_SPEAKERMUTE_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int SpeakerVolumeReadCallback(void *pp)
{
	session_timeout_tick = clock_time () | 1;
	return 1;
}

int SpeakerVolumeWriteCallback(void *pp)
{
	blt_adv_set_global_state ();

	if (hap_char_boundary_check()) {  // 0~100
        assign_current_flash_char_value_addr();
    #if SPEAKER_SERVICE_ENABLE
        SPEAKER_STORE_CHAR_VALUE_T_SPEAKERVOLUME_RESTORE
    #endif
	} else {
        store_char_value_to_flash();
	}

	session_timeout_tick = clock_time () | 1;
	return 1;
}

int ccc_hSpeakerMuteWriteCB(void *pp)
{
    static u16 ccc_hSpeakerMute_pre = 0;

    if(ccc_hSpeakerMute_pre != ccc_hSpeakerMute)
    {
        if(ccc_hSpeakerMute == 0)
        {
            ccc_hSpeakerMute_pre = ccc_hSpeakerMute;
            EventNotifyCNT--;
            indicate_handle_no_remove(SPEAKER_HANDLE_NUM_MUTE);
        }
        else
        {
            ccc_hSpeakerMute_pre = ccc_hSpeakerMute;
            EventNotifyCNT++;
            indicate_handle_no_reg(SPEAKER_HANDLE_NUM_MUTE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSpeakerVolumeWriteCB(void *pp)
{
    static u16 ccc_hSpeakerVolume_pre = 0;

    if(ccc_hSpeakerVolume_pre != ccc_hSpeakerVolume)
    {
        if(ccc_hSpeakerVolume == 0)
        {
            ccc_hSpeakerVolume_pre = ccc_hSpeakerVolume;
            EventNotifyCNT--;
            indicate_handle_no_remove(SPEAKER_HANDLE_NUM_VOLUME);
        }
        else
        {
            ccc_hSpeakerVolume_pre = ccc_hSpeakerVolume;
            EventNotifyCNT++;
            indicate_handle_no_reg(SPEAKER_HANDLE_NUM_VOLUME);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
