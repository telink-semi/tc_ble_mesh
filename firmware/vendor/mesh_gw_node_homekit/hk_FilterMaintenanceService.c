/********************************************************************************************************
 * @file     hk_FilterMaintenanceService.c
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
#define _HK_FILTER_MAINTENANCE_SERVICE_C_
 
 
/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "hk_FilterMaintenanceService.h"
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
unsigned char hFilterMaintenanceFilterChangeIndication = 0;
int hFilterMaintenanceFilterLifeLevel = 0;
unsigned char hFilterMaintenanceResetFilterIndication = 1;

unsigned char VAR_FILTER_MAINTENANCE_UNKOWN=0;

unsigned short ccc_hFilterMaintenanceFilterChangeIndication = 0;
unsigned short ccc_hFilterMaintenanceFilterLifeLevel = 0;

#if FILTER_MAINTENANCE_FILTER_CHANGE_INDICATION_BRC_ENABLE
    unsigned char bc_para_hFilterMaintenanceFilterChangeIndication = 1;
#endif

#if FILTER_MAINTENANCE_FILTER_LIFE_LEVEL_BRC_ENABLE
    unsigned char bc_para_hFilterMaintenanceFilterLifeLevel = 1;
#endif



const u8 hFilterMaintenanceServiceName[24] = "FilterMaintenanceService";

const u8 FILTER_MAINTENANCE_FILTER_CHANGE_INDICATION_VALID_RANGE[2] = {0x00, 0x01};
const u8 FILTER_MAINTENANCE_FILTER_LIFE_LEVEL_VALID_RANGE[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC8, 0x42};	// 0~100 (float)
const u8 FILTER_MAINTENANCE_RESET_FILTER_INDICATION_VALID_RANGE[2] = {0x01, 0x01};

const u8 FILTER_MAINTENANCE_FILTER_CHANGE_INDICATION_VALID_STEP = 0x01;
const u8 FILTER_MAINTENANCE_FILTER_LIFE_LEVEL_VALID_STEP[4] = { 0x00, 0x00, 0x80, 0x3F};


const hap_characteristic_desc_t hap_desc_char_FilterMaintenance_FilterChangeIndication_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, FILTER_MAINTENANCE_FILTER_CHANGE_INDICATION_VALID_RANGE, &FILTER_MAINTENANCE_FILTER_CHANGE_INDICATION_VALID_STEP, BC_PARA_FILTER_MAINTENANCE_FILTER_CHANGE_INDICATION};
const hap_characteristic_desc_t hap_desc_char_FilterMaintenance_FilterLifeLevel_R_S = {hap_PRead_NOTIFY, 0, CFG_FLOAT32, 0, 8, 4, 0, 0, FILTER_MAINTENANCE_FILTER_LIFE_LEVEL_VALID_RANGE, FILTER_MAINTENANCE_FILTER_LIFE_LEVEL_VALID_STEP, BC_PARA_FILTER_MAINTENANCE_FILTER_LIFE_LEVEL};
const hap_characteristic_desc_t hap_desc_char_FilterMaintenance_ResetFilterIndication_R_S = {hap_SecureW_20, 0, CFG_UINT8, 0, 2, 0, 0, 0, FILTER_MAINTENANCE_RESET_FILTER_INDICATION_VALID_RANGE, 0, 0};

const hap_characteristic_desc_t hap_desc_char_filter_maintenance_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if FILTER_MAINTENANCE_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * hFilterMaintenance_linked_svcs[] = {};
#endif

#if(FILTER_MAINTENANCE_IS_HIDDEN_SERVICE == 1)
const hap_service_desc_t hap_desc_service_filter_maintenance_A5_R_S = {FILTER_MAINTENANCE_INST_NO+1, (0x02 + FILTER_MAINTENANCE_AS_PRIMARY_SERVICE), FILTER_MAINTENANCE_LINKED_SVCS_LENGTH, FILTER_MAINTENANCE_LINKED_SVCS};
#else
const hap_service_desc_t hap_desc_service_filter_maintenance_A5_R_S = {FILTER_MAINTENANCE_INST_NO+1, (0x00 + FILTER_MAINTENANCE_AS_PRIMARY_SERVICE), FILTER_MAINTENANCE_LINKED_SVCS_LENGTH, FILTER_MAINTENANCE_LINKED_SVCS};
#endif

/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
int FilterMaintenanceFilterChangeIndicationReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int FilterMaintenanceFilterLifeLevelReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int FilterMaintenanceResetFilterIndicationWriteCallback(void *pp)
{
    //blt_adv_set_global_state ();	//this char do not support Notify, so no need update the GSN.

    if (hap_char_boundary_check()) {
        assign_current_flash_char_value_addr();
	#if FILTER_MAINTENANCE_SERVICE_ENABLE
		FILTER_MAINTENANCE_STORE_CHAR_VALUE_T_HFILTER_MAINTENANCE_RESET_FILTER_INDICATION_RESTORE
	#endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}


int ccc_hFilterMaintenanceFilterChangeIndicationWriteCB(void *pp)
{
    static u16 ccc_hFilterMaintenanceFilterChangeIndication_pre = 0;

    if(ccc_hFilterMaintenanceFilterChangeIndication_pre != ccc_hFilterMaintenanceFilterChangeIndication)
    {
        if(ccc_hFilterMaintenanceFilterChangeIndication == 0)
        {
            ccc_hFilterMaintenanceFilterChangeIndication_pre = ccc_hFilterMaintenanceFilterChangeIndication;
            EventNotifyCNT--;
            indicate_handle_no_remove(FILTER_MAINTENANCE_HANDLE_NUM_HFILTER_MAINTENANCE_FILTER_CHANGE_INDICATION);
        }
        else
        {
            ccc_hFilterMaintenanceFilterChangeIndication_pre = ccc_hFilterMaintenanceFilterChangeIndication;
            EventNotifyCNT++;
            indicate_handle_no_reg(FILTER_MAINTENANCE_HANDLE_NUM_HFILTER_MAINTENANCE_FILTER_CHANGE_INDICATION);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hFilterMaintenanceFilterLifeLevelWriteCB(void *pp)
{
    static u16 ccc_hFilterMaintenanceFilterLifeLevel_pre = 0;

    if(ccc_hFilterMaintenanceFilterLifeLevel_pre != ccc_hFilterMaintenanceFilterLifeLevel)
    {
        if(ccc_hFilterMaintenanceFilterLifeLevel == 0)
        {
            ccc_hFilterMaintenanceFilterLifeLevel_pre = ccc_hFilterMaintenanceFilterLifeLevel;
            EventNotifyCNT--;
            indicate_handle_no_remove(FILTER_MAINTENANCE_HANDLE_NUM_HFILTER_MAINTENANCE_FILTER_LIFE_LEVEL);
        }
        else
        {
            ccc_hFilterMaintenanceFilterLifeLevel_pre = ccc_hFilterMaintenanceFilterLifeLevel;
            EventNotifyCNT++;
            indicate_handle_no_reg(FILTER_MAINTENANCE_HANDLE_NUM_HFILTER_MAINTENANCE_FILTER_LIFE_LEVEL);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
