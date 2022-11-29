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
#ifndef TENCENTCLOUD_IOT_EXPLORER_BLE_MESH_SDK_EMBEDDED_INCLUDE_BLE_QIOT_EXPORT_H_
#define TENCENTCLOUD_IOT_EXPLORER_BLE_MESH_SDK_EMBEDDED_INCLUDE_BLE_QIOT_EXPORT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ble_qiot_utils_mesh.h"
#include "ble_qiot_common.h"

/**
 * @brief Process the mesh data received by the device
 * @param Opcode  Defined device opcode
 * @param data    Specific data content
 * @param data_len Data length
 * @return LLSYNC_MESH_RS_OK for success, or err code for failure
 */
ble_qiot_ret_status_t llsync_mesh_recv_data_handle(uint32_t Opcode, uint8_t *data, uint16_t data_len);

/**
 * @brief Bluetooth scan response data acquisition interface
 * @param data    Specific data content
 * @param data_len Data length
 * @return LLSYNC_MESH_RS_OK for success, or err code for failure
 */
ble_qiot_ret_status_t llsync_mesh_scan_data_get(uint8_t *data, uint8_t *data_len);

/**
 * @brief Device uuid value acquisition interface
 * @param type  LLSYNC_MESH_UNNET_ADV_BIT or LLSYNC_MESH_SILENCE_ADV_BIT
 * @param data    Specific data content
 * @param data_len Data length
 * @return Effective data length for success, or err code for failure
 */
int llsync_mesh_dev_uuid_get(uint8_t type, uint8_t *data, uint8_t data_len);

/**
 * @brief Authcalue calculation result acquisition interface
 * @param random       Input 16-bytes random number
 * @param auth_data    The output calculation results
 * @return LLSYNC_MESH_RS_OK for success, or err code for failure
 */
ble_qiot_ret_status_t llsync_mesh_auth_clac(uint8_t *random, uint8_t *auth_data);

/**
 * @brief Device active reporting interface
 */
void llsync_mesh_vendor_data_report(void);

#ifdef __cplusplus
}
#endif

#endif  // TENCENTCLOUD_IOT_EXPLORER_BLE_MESH_SDK_EMBEDDED_INCLUDE_BLE_QIOT_EXPORT_H_
