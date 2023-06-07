/*
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifndef TENCENTCLOUD_IOT_EXPLORER_BLE_MESH_SDK_EMBEDDED_CFG_BLE_QIOT_MESH_CFG_H_
#define TENCENTCLOUD_IOT_EXPLORER_BLE_MESH_SDK_EMBEDDED_CFG_BLE_QIOT_MESH_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "tl_common.h"

// qiot data storage address
#define LLSYNC_MESH_RECORD_FLASH_ADDR (0x79000) // 0x78000 for FLASH_ADR_THREE_PARA_ADR

// flash page size, see chip datasheet
#define LLSYNC_MESH_RECORD_FLASH_PAGESIZE (4096)

// Total broadcast hours of unallocated broadcasts(ms)
#define LLSYNC_MESH_UNNET_ADV_TOTAL_TIME (10 * 60 * 1000)

// If the unmatching timeout expires and the network is still not matched,
// does it enter the silent broadcast state, which is only used to let Provisioner discover the device
#define LLSYNC_MESH_SILENCE_ADV_ENABLE (1)

// Unallocated Broadcast Single Broadcast Duration(ms)
#define LLSYNC_MESH_UNNET_ADV_DURATION (150)			// No Application, and do not change.

// Silent Broadcast Single Broadcast Duration(ms)
#define LLSYNC_MESH_SILENCE_ADV_DURATION (5000)			// total number of adv is "LLSYNC_MESH_SILENCE_ADV_DURATION / (ADV_INTERVAL_MS + 30)"

// Unallocated broadcast interval(ms)
#define LLSYNC_MESH_UNNET_ADV_INTERVAL (0)				// set to 0 means no ADV stop before LLSYNC_MESH_UNNET_ADV_TOTAL_TIME. ADV interval please refer to ADV_INTERVAL_MS.

// Silent broadcast interval(ms)
#define LLSYNC_MESH_SILENCE_ADV_INTERVAL (60 * 1000)

#ifndef __ORDER_LITTLE_ENDIAN__
#define __ORDER_LITTLE_ENDIAN__ 1234
#endif

#ifndef __ORDER_BIG_ENDIAN__
#define __ORDER_BIG_ENDIAN__ 3412
#endif

#define __BYTE_ORDER__ __ORDER_LITTLE_ENDIAN__

#define MESH_LOG_PRINT(...) printf(__VA_ARGS__)

#define BLE_QIOT_USER_DEFINE_HEXDUMP (0)

#ifdef __cplusplus
}
#endif

#endif  // TENCENTCLOUD_IOT_EXPLORER_BLE_MESH_SDK_EMBEDDED_CFG_BLE_QIOT_MESH_CFG_H_
