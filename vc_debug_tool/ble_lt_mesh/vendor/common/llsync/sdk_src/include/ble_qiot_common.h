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
#ifndef TENCENTCLOUD_IOT_EXPLORER_BLE_MESH_SDK_EMBEDDED_SDK_INCLUDE_BLE_QIOT_COMMON_H_
#define TENCENTCLOUD_IOT_EXPLORER_BLE_MESH_SDK_EMBEDDED_SDK_INCLUDE_BLE_QIOT_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ble_qiot_mesh_cfg.h"

typedef enum {
    LLSYNC_MESH_RS_OK       = 0,   // success
    LLSYNC_MESH_RS_ERR      = -1,  // normal error
    LLSYNC_MESH_RS_ERR_PARA = -2,  // parameters error
} ble_qiot_ret_status_t;

#define LLSYNC_MESH_MODEL_OP_3(b0, cid) ((((b0) << 16) | 0xc00000) | (cid))

#define LLSYNC_MESH_VND_MODEL_OP_SET          LLSYNC_MESH_MODEL_OP_3(0x00, LLSYNC_MESH_CID_VENDOR)
#define LLSYNC_MESH_VND_MODEL_OP_GET          LLSYNC_MESH_MODEL_OP_3(0x01, LLSYNC_MESH_CID_VENDOR)
#define LLSYNC_MESH_VND_MODEL_OP_SET_UNACK    LLSYNC_MESH_MODEL_OP_3(0x02, LLSYNC_MESH_CID_VENDOR)
#define LLSYNC_MESH_VND_MODEL_OP_STATUS       LLSYNC_MESH_MODEL_OP_3(0x03, LLSYNC_MESH_CID_VENDOR)
#define LLSYNC_MESH_VND_MODEL_OP_INDICATION   LLSYNC_MESH_MODEL_OP_3(0x04, LLSYNC_MESH_CID_VENDOR)
#define LLSYNC_MESH_VND_MODEL_OP_CONFIRMATION LLSYNC_MESH_MODEL_OP_3(0x05, LLSYNC_MESH_CID_VENDOR)

#define LLSYNC_MESH_PRODUCT_ID_LEN  (10)  // fixed length of product id
#define LLSYNC_MESH_DEVICE_NAME_LEN (48)  // max length of device name
#define LLSYNC_MESH_PSK_LEN         (24)  // fixed length of device secret key
#define LLSYNC_MESH_MAC_LEN         (6)   // fixed length of mac

#define SWAP_32(x) \
    ((((x)&0xFF000000) >> 24) | (((x)&0x00FF0000) >> 8) | (((x)&0x0000FF00) << 8) | (((x)&0x000000FF) << 24))

#define SWAP_16(x) ((((x)&0xFF00) >> 8) | (((x)&0x00FF) << 8))

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define HTONL(x) SWAP_32(x)
#define HTONS(x) SWAP_16(x)
#define NTOHL(x) SWAP_32(x)
#define NTOHS(x) SWAP_16(x)
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define HTONL(x) (x)
#define HTONS(x) (x)
#define NTOHL(x) (x)
#define NTOHS(x) (x)
#else
#error "undefined byte order"
#endif

#ifdef __cplusplus
}
#endif

#endif  // TENCENTCLOUD_IOT_EXPLORER_BLE_MESH_SDK_EMBEDDED_SDK_INCLUDE_BLE_QIOT_COMMON_H_
