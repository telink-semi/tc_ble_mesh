/********************************************************************************************************
 * @file     ota_vendor_cmd.h 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
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

#ifndef OTA_VENDOR_CMD_H_
#define OTA_VENDOR_CMD_H_

#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"

#include "../mfi/mfi.h"
#include "vendor/common/led_cfg.h"

extern u8 otabuffer[20];

extern void CustVendorCmdFunctions(void *p);

#endif /* OTA_VENDOR_CMD_H_ */
