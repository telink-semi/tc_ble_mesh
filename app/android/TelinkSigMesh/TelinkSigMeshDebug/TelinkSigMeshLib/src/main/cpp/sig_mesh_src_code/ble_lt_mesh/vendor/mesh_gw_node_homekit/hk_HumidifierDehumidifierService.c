/********************************************************************************************************
 * @file     hk_HumidifierDehumidifierService.c
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
#define _HK_HUMIDIFIER_DEHUMIDIFIER_SERVICE_C_
 
 
/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "hk_HumidifierDehumidifierService.h"
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
unsigned char hHumidifierDehumidifierActive = 1;
unsigned int hHumidifierDehumidifierCurrentRelativeHumidity = 0x41C80000;	//25.0
unsigned char hHumidifierDehumidifierCurrentHumidifierDehumidifierState = 0;
unsigned char hHumidifierDehumidifierTargetHumidifierDehumidifierState = 0;
unsigned int hHumidifierDehumidifierRelativeHumidityDehumidifierThreshold = 0;
unsigned int hHumidifierDehumidifierRelativeHumidityHumidifierThreshold = 0;
unsigned int hHumidifierDehumidifierRotationSpeed = 0;
unsigned char hHumidifierDehumidifierSwingMode = 0;
unsigned int hHumidifierDehumidifierWaterLevel = 0;
unsigned char hHumidifierDehumidifierLockPhysicalControls = 0;

unsigned char VAR_HUMIDIFIER_DEHUMIDIFIER_UNKOWN=0;

unsigned short ccc_hHumidifierDehumidifierActive = 0;
unsigned short ccc_hHumidifierDehumidifierCurrentRelativeHumidity = 0;
unsigned short ccc_hHumidifierDehumidifierCurrentHumidifierDehumidifierState = 0;
unsigned short ccc_hHumidifierDehumidifierTargetHumidifierDehumidifierState = 0;
unsigned short ccc_hHumidifierDehumidifierRelativeHumidityDehumidifierThreshold = 0;
unsigned short ccc_hHumidifierDehumidifierRelativeHumidityHumidifierThreshold = 0;
unsigned short ccc_hHumidifierDehumidifierRotationSpeed = 0;
unsigned short ccc_hHumidifierDehumidifierSwingMode = 0;
unsigned short ccc_hHumidifierDehumidifierWaterLevel = 0;
unsigned short ccc_hHumidifierDehumidifierLockPhysicalControls = 0;

#if HUMIDIFIERDEHUMIDIFIER_ACTIVE_BRC_ENABLE
    unsigned char bc_para_hHumidifierDehumidifierActive = 1;
#endif

#if HUMIDIFIERDEHUMIDIFIER_CURRENTRELATIVEHUMIDITY_BRC_ENABLE
    unsigned char bc_para_hHumidifierDehumidifierCurrentRelativeHumidity = 1;
#endif

#if HUMIDIFIERDEHUMIDIFIER_CURRENTHUMIDIFIERDEHUMIDIFIERSTATE_BRC_ENABLE
    unsigned char bc_para_hHumidifierDehumidifierCurrentHumidifierDehumidifierState = 1;
#endif

#if HUMIDIFIERDEHUMIDIFIER_TARGETHUMIDIFIERDEHUMIDIFIERSTATE_BRC_ENABLE
    unsigned char bc_para_hHumidifierDehumidifierTargetHumidifierDehumidifierState = 1;
#endif

#if HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD_BRC_ENABLE
    unsigned char bc_para_hHumidifierDehumidifierRelativeHumidityDehumidifierThreshold = 1;
#endif

#if HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD_BRC_ENABLE
    unsigned char bc_para_hHumidifierDehumidifierRelativeHumidityHumidifierThreshold = 1;
#endif

#if HUMIDIFIERDEHUMIDIFIER_ROTATIONSPEED_BRC_ENABLE
    unsigned char bc_para_hHumidifierDehumidifierRotationSpeed = 1;
#endif

#if HUMIDIFIERDEHUMIDIFIER_SWINGMODE_BRC_ENABLE
    unsigned char bc_para_hHumidifierDehumidifierSwingMode = 1;
#endif

#if HUMIDIFIERDEHUMIDIFIER_WATERLEVEL_BRC_ENABLE
    unsigned char bc_para_hHumidifierDehumidifierWaterLevel = 1;
#endif

#if HUMIDIFIERDEHUMIDIFIER_LOCKPHYSICALCONTROLS_BRC_ENABLE
    unsigned char bc_para_hHumidifierDehumidifierLockPhysicalControls = 1;
#endif

const u8 hHumidifierDehumidifierServiceName[29] = "HumidifierDehumidifierService";

const u8 HUMIDIFIERDEHUMIDIFIERACTIVE_VALID_RANGE[2] = {0x00, 0x01};
const u8 HUMIDIFIERDEHUMIDIFIERCURRENTRELATIVEHUMIDITY_VALID_RANGE[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC8, 0x42};	 // 0~100 (float)
const u8 HUMIDIFIERDEHUMIDIFIERCURRENTHUMIDIFIERDEHUMIDIFIERSTATE_VALID_RANGE[2] = {0x00, 0x03};
const u8 HUMIDIFIERDEHUMIDIFIERTARGETHUMIDIFIERDEHUMIDIFIERSTATE_VALID_RANGE[2] = {0x00, 0x02};
const u8 HUMIDIFIERDEHUMIDIFIERRELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD_VALID_RANGE[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC8, 0x42};	 // 0~100 (float)
const u8 HUMIDIFIERDEHUMIDIFIERRELATIVEHUMIDITYHUMIDIFIERTHRESHOLD_VALID_RANGE[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC8, 0x42};	 // 0~100 (float)
const u8 HUMIDIFIERDEHUMIDIFIERROTATIONSPEED_VALID_RANGE[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC8, 0x42};	 // 0~100 (float)
const u8 HUMIDIFIERDEHUMIDIFIERSWINGMODE_VALID_RANGE[2] = {0x00, 0x01};
const u8 HUMIDIFIERDEHUMIDIFIERWATERLEVEL_VALID_RANGE[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC8, 0x42};	 // 0~100 (float)
const u8 HUMIDIFIERDEHUMIDIFIERLOCKPHYSICALCONTROLS_VALID_RANGE[2] = {0x00, 0x01};

const u8 HUMIDIFIERDEHUMIDIFIERACTIVE_VALID_STEP = 0x01;
const u8 HUMIDIFIERDEHUMIDIFIERCURRENTRELATIVEHUMIDITY_VALID_STEP[4] = {0x00, 0x00, 0x80, 0x3F};	 // 1 (float)
const u8 HUMIDIFIERDEHUMIDIFIERCURRENTHUMIDIFIERDEHUMIDIFIERSTATE_VALID_STEP = 0x01;
const u8 HUMIDIFIERDEHUMIDIFIERTARGETHUMIDIFIERDEHUMIDIFIERSTATE_VALID_STEP = 0x01;
const u8 HUMIDIFIERDEHUMIDIFIERRELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD_VALID_STEP[4] = {0x00, 0x00, 0x80, 0x3F};	 // 1 (float)
const u8 HUMIDIFIERDEHUMIDIFIERRELATIVEHUMIDITYHUMIDIFIERTHRESHOLD_VALID_STEP[4] = {0x00, 0x00, 0x80, 0x3F};	 // 1 (float)
const u8 HUMIDIFIERDEHUMIDIFIERROTATIONSPEED_VALID_STEP[4] = {0x00, 0x00, 0x80, 0x3F};	 // 1 (float)
const u8 HUMIDIFIERDEHUMIDIFIERSWINGMODE_VALID_STEP = 0x01;
const u8 HUMIDIFIERDEHUMIDIFIERWATERLEVEL_VALID_STEP[4] = {0x00, 0x00, 0x80, 0x3F};	 // 1 (float)
const u8 HUMIDIFIERDEHUMIDIFIERLOCKPHYSICALCONTROLS_VALID_STEP = 0x01;

const hap_characteristic_desc_t hap_desc_char_HUMIDIFIERDEHUMIDIFIER_ACTIVE_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, HUMIDIFIERDEHUMIDIFIERACTIVE_VALID_RANGE, &HUMIDIFIERDEHUMIDIFIERACTIVE_VALID_STEP, BC_PARA_HUMIDIFIERDEHUMIDIFIER_ACTIVE};
const hap_characteristic_desc_t hap_desc_char_HUMIDIFIERDEHUMIDIFIER_CURRENTRELATIVEHUMIDITY_R_S = {hap_PRead_NOTIFY, 0, CFG_FLOAT32_PERCENTAGE, 0, 8, 4, 0, 0, HUMIDIFIERDEHUMIDIFIERCURRENTRELATIVEHUMIDITY_VALID_RANGE, HUMIDIFIERDEHUMIDIFIERCURRENTRELATIVEHUMIDITY_VALID_STEP, BC_PARA_HUMIDIFIERDEHUMIDIFIER_CURRENTRELATIVEHUMIDITY};
const hap_characteristic_desc_t hap_desc_char_HUMIDIFIERDEHUMIDIFIER_CURRENTHUMIDIFIERDEHUMIDIFIERSTATE_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, HUMIDIFIERDEHUMIDIFIERCURRENTHUMIDIFIERDEHUMIDIFIERSTATE_VALID_RANGE, &HUMIDIFIERDEHUMIDIFIERCURRENTHUMIDIFIERDEHUMIDIFIERSTATE_VALID_STEP, BC_PARA_HUMIDIFIERDEHUMIDIFIER_CURRENTHUMIDIFIERDEHUMIDIFIERSTATE};
const hap_characteristic_desc_t hap_desc_char_HUMIDIFIERDEHUMIDIFIER_TARGETHUMIDIFIERDEHUMIDIFIERSTATE_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, HUMIDIFIERDEHUMIDIFIERTARGETHUMIDIFIERDEHUMIDIFIERSTATE_VALID_RANGE, &HUMIDIFIERDEHUMIDIFIERTARGETHUMIDIFIERDEHUMIDIFIERSTATE_VALID_STEP, BC_PARA_HUMIDIFIERDEHUMIDIFIER_TARGETHUMIDIFIERDEHUMIDIFIERSTATE};
const hap_characteristic_desc_t hap_desc_char_HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_FLOAT32_PERCENTAGE, 0, 8, 4, 0, 0, HUMIDIFIERDEHUMIDIFIERRELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD_VALID_RANGE, HUMIDIFIERDEHUMIDIFIERRELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD_VALID_STEP, BC_PARA_HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD};
const hap_characteristic_desc_t hap_desc_char_HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_FLOAT32_PERCENTAGE, 0, 8, 4, 0, 0, HUMIDIFIERDEHUMIDIFIERRELATIVEHUMIDITYHUMIDIFIERTHRESHOLD_VALID_RANGE, HUMIDIFIERDEHUMIDIFIERRELATIVEHUMIDITYHUMIDIFIERTHRESHOLD_VALID_STEP, BC_PARA_HUMIDIFIERDEHUMIDIFIER_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD};
const hap_characteristic_desc_t hap_desc_char_HUMIDIFIERDEHUMIDIFIER_ROTATIONSPEED_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_FLOAT32_PERCENTAGE, 0, 8, 4, 0, 0, HUMIDIFIERDEHUMIDIFIERROTATIONSPEED_VALID_RANGE, HUMIDIFIERDEHUMIDIFIERROTATIONSPEED_VALID_STEP, BC_PARA_HUMIDIFIERDEHUMIDIFIER_ROTATIONSPEED};
const hap_characteristic_desc_t hap_desc_char_HUMIDIFIERDEHUMIDIFIER_SWINGMODE_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, HUMIDIFIERDEHUMIDIFIERSWINGMODE_VALID_RANGE, &HUMIDIFIERDEHUMIDIFIERSWINGMODE_VALID_STEP, BC_PARA_HUMIDIFIERDEHUMIDIFIER_SWINGMODE};
const hap_characteristic_desc_t hap_desc_char_HUMIDIFIERDEHUMIDIFIER_WATERLEVEL_R_S = {hap_PRead_NOTIFY, 0, CFG_FLOAT32_PERCENTAGE, 0, 8, 4, 0, 0, HUMIDIFIERDEHUMIDIFIERWATERLEVEL_VALID_RANGE, HUMIDIFIERDEHUMIDIFIERWATERLEVEL_VALID_STEP, BC_PARA_HUMIDIFIERDEHUMIDIFIER_WATERLEVEL};
const hap_characteristic_desc_t hap_desc_char_HUMIDIFIERDEHUMIDIFIER_LOCKPHYSICALCONTROLS_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, HUMIDIFIERDEHUMIDIFIERLOCKPHYSICALCONTROLS_VALID_RANGE, &HUMIDIFIERDEHUMIDIFIERLOCKPHYSICALCONTROLS_VALID_STEP, BC_PARA_HUMIDIFIERDEHUMIDIFIER_LOCKPHYSICALCONTROLS};

const hap_characteristic_desc_t hap_desc_char_humidifier_dehumidifier_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if HUMIDIFIERDEHUMIDIFIER_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * hHumidifierDehumidifier_linked_svcs[] = {};
#endif

#if(HUMIDIFIERDEHUMIDIFIER_IS_HIDDEN_SERVICE == 1)
const hap_service_desc_t hap_desc_service_humidifier_dehumidifier_A5_R_S = {HUMIDIFIERDEHUMIDIFIER_INST_NO+1, (0x02 + HUMIDIFIERDEHUMIDIFIER_AS_PRIMARY_SERVICE), HUMIDIFIERDEHUMIDIFIER_LINKED_SVCS_LENGTH, HUMIDIFIERDEHUMIDIFIER_LINKED_SVCS};
#else
const hap_service_desc_t hap_desc_service_humidifier_dehumidifier_A5_R_S = {HUMIDIFIERDEHUMIDIFIER_INST_NO+1, (0x00 + HUMIDIFIERDEHUMIDIFIER_AS_PRIMARY_SERVICE), HUMIDIFIERDEHUMIDIFIER_LINKED_SVCS_LENGTH, HUMIDIFIERDEHUMIDIFIER_LINKED_SVCS};
#endif


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */

int HumidifierDehumidifierActiveReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int HumidifierDehumidifierActiveWriteCallback(void *pp)
{
	blt_adv_set_global_state ();

	if (hap_char_boundary_check()) {
		assign_current_flash_char_value_addr();
	#if HUMIDIFIER_DEHUMIDIFIER_SERVICE_ENABLE
		HD_STORE_CHAR_VALUE_T_HD_ACTIVE_RESTORE
	#endif
	} else {
		store_char_value_to_flash();
	}

	session_timeout_tick = clock_time () | 1;
	return 1;
}

int HumidifierDehumidifierCurrentRelativeHumidityReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int HumidifierDehumidifierCurrentHumidifierDehumidifierStateReadCallback(void *pp)
{
	session_timeout_tick = clock_time () | 1;
	return 1;
}

int HumidifierDehumidifierTargetHumidifierDehumidifierStateReadCallback(void *pp)
{
	session_timeout_tick = clock_time () | 1;
	return 1;
}

int HumidifierDehumidifierTargetHumidifierDehumidifierStateWriteCallback(void *pp)
{
	blt_adv_set_global_state ();

	if (hap_char_boundary_check()) {
		assign_current_flash_char_value_addr();
	#if HUMIDIFIER_DEHUMIDIFIER_SERVICE_ENABLE
		HD_STORE_CHAR_VALUE_T_HD_TARGETHUMIDIFIERDEHUMIDIFIERSTATE_RESTORE
	#endif
	} else {
		store_char_value_to_flash();
	}

	session_timeout_tick = clock_time () | 1;
	return 1;
}

int HumidifierDehumidifierRelativeHumidityDehumidifierThresholdReadCallback(void *pp)
{
	session_timeout_tick = clock_time () | 1;
	return 1;
}

int HumidifierDehumidifierRelativeHumidityDehumidifierThresholdWriteCallback(void *pp)
{
	blt_adv_set_global_state ();

	if (hap_char_boundary_check()) {
		assign_current_flash_char_value_addr();
	#if HUMIDIFIER_DEHUMIDIFIER_SERVICE_ENABLE
		HD_STORE_CHAR_VALUE_T_HD_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD_RESTORE
	#endif
	} else {
		store_char_value_to_flash();
	}

	session_timeout_tick = clock_time () | 1;
	return 1;
}

int HumidifierDehumidifierRelativeHumidityHumidifierThresholdReadCallback(void *pp)
{
	session_timeout_tick = clock_time () | 1;
	return 1;
}

int HumidifierDehumidifierRelativeHumidityHumidifierThresholdWriteCallback(void *pp)
{
	blt_adv_set_global_state ();

	if (hap_char_boundary_check()) {
		assign_current_flash_char_value_addr();
	#if HUMIDIFIER_DEHUMIDIFIER_SERVICE_ENABLE
		HD_STORE_CHAR_VALUE_T_HD_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD_RESTORE
	#endif
	} else {
		store_char_value_to_flash();
	}

	session_timeout_tick = clock_time () | 1;
	return 1;
}

int HumidifierDehumidifierRotationSpeedReadCallback(void *pp)
{
	session_timeout_tick = clock_time () | 1;
	return 1;
}

int HumidifierDehumidifierRotationSpeedWriteCallback(void *pp)
{
	blt_adv_set_global_state ();

	if (hap_char_boundary_check()) {
		assign_current_flash_char_value_addr();
	#if HUMIDIFIER_DEHUMIDIFIER_SERVICE_ENABLE
		HD_STORE_CHAR_VALUE_T_HD_ROTATIONSPEED_RESTORE
	#endif
	} else {
		store_char_value_to_flash();
	}

	session_timeout_tick = clock_time () | 1;
	return 1;
}

int HumidifierDehumidifierSwingModeReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int HumidifierDehumidifierSwingModeWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~1
        assign_current_flash_char_value_addr();
    #if HUMIDIFIER_DEHUMIDIFIER_SERVICE_ENABLE
        HD_STORE_CHAR_VALUE_T_HD_SWINGMODE_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}


int HumidifierDehumidifierWaterLevelReadCallback(void *pp)
{
	session_timeout_tick = clock_time () | 1;
	return 1;
}

int HumidifierDehumidifierLockPhysicalControlsReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int HumidifierDehumidifierLockPhysicalControlsWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~1
        assign_current_flash_char_value_addr();
    #if HUMIDIFIER_DEHUMIDIFIER_SERVICE_ENABLE
        HD_STORE_CHAR_VALUE_T_HD_LOCKPHYSICALCONTROLS_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_HumidifierDehumidifierActiveWriteCB(void *pp)
{
	static u16 ccc_hHumidifierDehumidifierActive_pre = 0;

	if(ccc_hHumidifierDehumidifierActive_pre != ccc_hHumidifierDehumidifierActive)
	{
		if(ccc_hHumidifierDehumidifierActive == 0)
		{
			ccc_hHumidifierDehumidifierActive_pre = ccc_hHumidifierDehumidifierActive;
			EventNotifyCNT--;
			indicate_handle_no_remove(HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_ACTIVE);
		}
		else
		{
			ccc_hHumidifierDehumidifierActive_pre = ccc_hHumidifierDehumidifierActive;
			EventNotifyCNT++;
			indicate_handle_no_reg(HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_ACTIVE);
		}
	}
	session_timeout_tick = clock_time () | 1;
	return 1;
}

int ccc_HumidifierDehumidifierCurrentRelativeHumidityWriteCB(void *pp)
{
	static u16 ccc_hHumidifierDehumidifierCurrentRelativeHumidity_pre = 0;

	if(ccc_hHumidifierDehumidifierCurrentRelativeHumidity_pre != ccc_hHumidifierDehumidifierCurrentRelativeHumidity)
	{
		if(ccc_hHumidifierDehumidifierCurrentRelativeHumidity == 0)
		{
			ccc_hHumidifierDehumidifierCurrentRelativeHumidity_pre = ccc_hHumidifierDehumidifierCurrentRelativeHumidity;
			EventNotifyCNT--;
			indicate_handle_no_remove(HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_CURRENTRELATIVEHUMIDITY);
		}
		else
		{
			ccc_hHumidifierDehumidifierCurrentRelativeHumidity_pre = ccc_hHumidifierDehumidifierCurrentRelativeHumidity;
			EventNotifyCNT++;
			indicate_handle_no_reg(HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_CURRENTRELATIVEHUMIDITY);
		}
	}
	session_timeout_tick = clock_time () | 1;
	return 1;
}

int ccc_HumidifierDehumidifierCurrentHumidifierDehumidifierStateWriteCB(void *pp)
{
	static u16 ccc_hHumidifierDehumidifierCurrentHumidifierDehumidifierState_pre = 0;

	if(ccc_hHumidifierDehumidifierCurrentHumidifierDehumidifierState_pre != ccc_hHumidifierDehumidifierCurrentHumidifierDehumidifierState)
	{
		if(ccc_hHumidifierDehumidifierCurrentHumidifierDehumidifierState == 0)
		{
			ccc_hHumidifierDehumidifierCurrentHumidifierDehumidifierState_pre = ccc_hHumidifierDehumidifierCurrentHumidifierDehumidifierState;
			EventNotifyCNT--;
			indicate_handle_no_remove(HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_CURRENTHUMIDIFIERDEHUMIDIFIERSTATE);
		}
		else
		{
			ccc_hHumidifierDehumidifierCurrentHumidifierDehumidifierState_pre = ccc_hHumidifierDehumidifierCurrentHumidifierDehumidifierState;
			EventNotifyCNT++;
			indicate_handle_no_reg(HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_CURRENTHUMIDIFIERDEHUMIDIFIERSTATE);
		}
	}
	session_timeout_tick = clock_time () | 1;
	return 1;
}

int ccc_HumidifierDehumidifierTargetHumidifierDehumidifierStateWriteCB(void *pp)
{
	static u16 ccc_hHumidifierDehumidifierTargetHumidifierDehumidifierState_pre = 0;

	if(ccc_hHumidifierDehumidifierTargetHumidifierDehumidifierState_pre != ccc_hHumidifierDehumidifierTargetHumidifierDehumidifierState)
	{
		if(ccc_hHumidifierDehumidifierTargetHumidifierDehumidifierState == 0)
		{
			ccc_hHumidifierDehumidifierTargetHumidifierDehumidifierState_pre = ccc_hHumidifierDehumidifierTargetHumidifierDehumidifierState;
			EventNotifyCNT--;
			indicate_handle_no_remove(HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_TARGETHUMIDIFIERDEHUMIDIFIERSTATE);
		}
		else
		{
			ccc_hHumidifierDehumidifierTargetHumidifierDehumidifierState_pre = ccc_hHumidifierDehumidifierTargetHumidifierDehumidifierState;
			EventNotifyCNT++;
			indicate_handle_no_reg(HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_TARGETHUMIDIFIERDEHUMIDIFIERSTATE);
		}
	}
	session_timeout_tick = clock_time () | 1;
	return 1;
}

int ccc_HumidifierDehumidifierRelativeHumidityDehumidifierThresholdWriteCB(void *pp)
{
	static u16 ccc_hHumidifierDehumidifierRelativeHumidityDehumidifierThreshold_pre = 0;

	if(ccc_hHumidifierDehumidifierRelativeHumidityDehumidifierThreshold_pre != ccc_hHumidifierDehumidifierRelativeHumidityDehumidifierThreshold)
	{
		if(ccc_hHumidifierDehumidifierRelativeHumidityDehumidifierThreshold == 0)
		{
			ccc_hHumidifierDehumidifierRelativeHumidityDehumidifierThreshold_pre = ccc_hHumidifierDehumidifierRelativeHumidityDehumidifierThreshold;
			EventNotifyCNT--;
			indicate_handle_no_remove(HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD);
		}
		else
		{
			ccc_hHumidifierDehumidifierRelativeHumidityDehumidifierThreshold_pre = ccc_hHumidifierDehumidifierRelativeHumidityDehumidifierThreshold;
			EventNotifyCNT++;
			indicate_handle_no_reg(HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_RELATIVEHUMIDITYDEHUMIDIFIERTHRESHOLD);
		}
	}
	session_timeout_tick = clock_time () | 1;
	return 1;
}

int ccc_HumidifierDehumidifierRelativeHumidityHumidifierThresholdWriteCB(void *pp)
{
	static u16 ccc_hHumidifierDehumidifierRelativeHumidityHumidifierThreshold_pre = 0;

	if(ccc_hHumidifierDehumidifierRelativeHumidityHumidifierThreshold_pre != ccc_hHumidifierDehumidifierRelativeHumidityHumidifierThreshold)
	{
		if(ccc_hHumidifierDehumidifierRelativeHumidityHumidifierThreshold == 0)
		{
			ccc_hHumidifierDehumidifierRelativeHumidityHumidifierThreshold_pre = ccc_hHumidifierDehumidifierRelativeHumidityHumidifierThreshold;
			EventNotifyCNT--;
			indicate_handle_no_remove(HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD);
		}
		else
		{
			ccc_hHumidifierDehumidifierRelativeHumidityHumidifierThreshold_pre = ccc_hHumidifierDehumidifierRelativeHumidityHumidifierThreshold;
			EventNotifyCNT++;
			indicate_handle_no_reg(HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_RELATIVEHUMIDITYHUMIDIFIERTHRESHOLD);
		}
	}
	session_timeout_tick = clock_time () | 1;
	return 1;
}

int ccc_HumidifierDehumidifierRotationSpeedWriteCB(void *pp)
{
	static u16 ccc_hHumidifierDehumidifierRotationSpeed_pre = 0;

	if(ccc_hHumidifierDehumidifierRotationSpeed_pre != ccc_hHumidifierDehumidifierRotationSpeed)
	{
		if(ccc_hHumidifierDehumidifierRotationSpeed == 0)
		{
			ccc_hHumidifierDehumidifierRotationSpeed_pre = ccc_hHumidifierDehumidifierRotationSpeed;
			EventNotifyCNT--;
			indicate_handle_no_remove(HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_ROTATIONSPEED);
		}
		else
		{
			ccc_hHumidifierDehumidifierRotationSpeed_pre = ccc_hHumidifierDehumidifierRotationSpeed;
			EventNotifyCNT++;
			indicate_handle_no_reg(HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_ROTATIONSPEED);
		}
	}
	session_timeout_tick = clock_time () | 1;
	return 1;
}

int ccc_HumidifierDehumidifierSwingModeWriteCB(void *pp)
{
    static u16 ccc_hHumidifierDehumidifierSwingMode_pre = 0;

    if(ccc_hHumidifierDehumidifierSwingMode_pre != ccc_hHumidifierDehumidifierSwingMode)
    {
        if(ccc_hHumidifierDehumidifierSwingMode == 0)
        {
            ccc_hHumidifierDehumidifierSwingMode_pre = ccc_hHumidifierDehumidifierSwingMode;
            EventNotifyCNT--;
            indicate_handle_no_remove(HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_SWINGMODE);
        }
        else
        {
            ccc_hHumidifierDehumidifierSwingMode_pre = ccc_hHumidifierDehumidifierSwingMode;
            EventNotifyCNT++;
            indicate_handle_no_reg(HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_SWINGMODE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_HumidifierDehumidifierWaterLevelWriteCB(void *pp)
{
	static u16 ccc_hHumidifierDehumidifierWaterLevel_pre = 0;

	if(ccc_hHumidifierDehumidifierWaterLevel_pre != ccc_hHumidifierDehumidifierWaterLevel)
	{
		if(ccc_hHumidifierDehumidifierWaterLevel == 0)
		{
			ccc_hHumidifierDehumidifierWaterLevel_pre = ccc_hHumidifierDehumidifierWaterLevel;
			EventNotifyCNT--;
			indicate_handle_no_remove(HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_WATERLEVEL);
		}
		else
		{
			ccc_hHumidifierDehumidifierWaterLevel_pre = ccc_hHumidifierDehumidifierWaterLevel;
			EventNotifyCNT++;
			indicate_handle_no_reg(HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_WATERLEVEL);
		}
	}
	session_timeout_tick = clock_time () | 1;
	return 1;
}

int ccc_HumidifierDehumidifierLockPhysicalControlsWriteCB(void *pp)
{
    static u16 ccc_hHumidifierDehumidifierLockPhysicalControls_pre = 0;

    if(ccc_hHumidifierDehumidifierLockPhysicalControls_pre != ccc_hHumidifierDehumidifierLockPhysicalControls)
    {
        if(ccc_hHumidifierDehumidifierLockPhysicalControls == 0)
        {
            ccc_hHumidifierDehumidifierLockPhysicalControls_pre = ccc_hHumidifierDehumidifierLockPhysicalControls;
            EventNotifyCNT--;
            indicate_handle_no_remove(HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_LOCKPHYSICALCONTROLS);
        }
        else
        {
            ccc_hHumidifierDehumidifierLockPhysicalControls_pre = ccc_hHumidifierDehumidifierLockPhysicalControls;
            EventNotifyCNT++;
            indicate_handle_no_reg(HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM_LOCKPHYSICALCONTROLS);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
