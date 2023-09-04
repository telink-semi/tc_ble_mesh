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
#ifndef TENCENTCLOUD_IOT_EXPLORER_BLE_MESH_SDK_EMBEDDED_INCLUDE_BLE_QIOT_IMPORT_H_
#define TENCENTCLOUD_IOT_EXPLORER_BLE_MESH_SDK_EMBEDDED_INCLUDE_BLE_QIOT_IMPORT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ble_qiot_utils_mesh.h"
#include "ble_qiot_common.h"

ble_qiot_ret_status_t llsync_mesh_dev_info_get(ble_device_info_t *dev_info);

ble_qiot_ret_status_t llsync_mesh_adv_handle(e_llsync_mesh_user adv_status, uint8_t *data_buf, uint8_t data_len);

ble_timer_t llsync_mesh_timer_create(uint8_t type, ble_timer_cb timeout_handle);

ble_qiot_ret_status_t llsync_mesh_timer_start(ble_timer_t timer_id, uint32_t period);

ble_qiot_ret_status_t llsync_mesh_timer_stop(ble_timer_t timer_id);

int llsync_mesh_flash_handle(e_llsync_flash_user type, uint32_t flash_addr, char *buf, uint16_t len);

void llsync_mesh_vendor_data_send(uint32_t opcode, uint8_t *data, uint16_t data_len);

#ifdef __cplusplus
}
#endif

#endif  // TENCENTCLOUD_IOT_EXPLORER_BLE_MESH_SDK_EMBEDDED_INCLUDE_BLE_QIOT_IMPORT_H_
