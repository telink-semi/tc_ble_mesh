/*
 * ota_vendor_cmd.h
 *
 *  Created on: 2019-2-15
 *      Author: Administrator
 */

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
