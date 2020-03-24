/********************************************************************************************************
 * @file     hk_ServiceLabelService.c
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
#define _HK_SERVICE_LABEL_SERVICE_C_
 
 
/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "hk_ServiceLabelService.h"
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
unsigned char hServiceLabelNamespace = 0;

unsigned char VAR_SERVICE_LABEL_UNKOWN=0;

const u8 hServiceLabelServiceName[19] = "ServiceLabelService";

const u8 SERVICE_LABEL_NAMESPACE_VALID_RANGE[2] = {0x00, 0x01};

const u8 SERVICE_LABEL_NAMESPACE_VALID_STEP = 0x01;

const hap_characteristic_desc_t hap_desc_char_ServiceLabelNamespace_R_S = {hap_SecureR_10, 0, CFG_UINT8, 0, 2, 1, 0, 0, SERVICE_LABEL_NAMESPACE_VALID_RANGE, &SERVICE_LABEL_NAMESPACE_VALID_STEP, 0};

const hap_characteristic_desc_t hap_desc_char_service_label_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if SERVICE_LABEL_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * hServiceLabel_linked_svcs[] = {};
#endif

#if(SERVICE_LABEL_IS_HIDDEN_SERVICE == 1)
const hap_service_desc_t hap_desc_service_service_label_A5_R_S = {SERVICE_LABEL_INST_NO+1, (0x02 + SERVICE_LABEL_AS_PRIMARY_SERVICE), SERVICE_LABEL_LINKED_SVCS_LENGTH, SERVICE_LABEL_LINKED_SVCS};
#else
const hap_service_desc_t hap_desc_service_service_label_A5_R_S = {SERVICE_LABEL_INST_NO+1, (0x00 + SERVICE_LABEL_AS_PRIMARY_SERVICE), SERVICE_LABEL_LINKED_SVCS_LENGTH, SERVICE_LABEL_LINKED_SVCS};
#endif

/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
int ServiceLabelNamespaceReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}


/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
