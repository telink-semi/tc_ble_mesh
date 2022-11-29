/*
 * Copyright (C) 2022 THL A29 Limited, a Tencent company. All rights reserved.
 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifndef TENCENTCLOUD_IOT_EXPLORER_BLE_MESH_SDK_EMBEDDED_DATA_TEMPLATE_BLE_QIOT_TEMPLATE_VENDOR_H_
#define TENCENTCLOUD_IOT_EXPLORER_BLE_MESH_SDK_EMBEDDED_DATA_TEMPLATE_BLE_QIOT_TEMPLATE_VENDOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "ble_qiot_template.h"
#include "proj_lib/sig_mesh/app_mesh.h"
#include "vendor/common/lighting_model.h"
#include "vendor/common/lighting_model_HSL.h"

// ---------------------------------------------------------------------
// data struct
// ---------------------------------------------------------------------

/**
 * @brief attribute type
 *
 */


void mesh_g_onoff_st_rsp_par_fill(mesh_cmd_g_onoff_st_t *rsp, u8 idx);
void mesh_light_hsl_st_rsp_par_fill(mesh_cmd_light_hsl_st_t *rsp, u8 idx, u16 op_rsp);
int access_cmd_set_light_hsl(u16 adr, u8 rsp_max, u16 lightness, u16 hue, u16 sat, int ack, transition_par_t *trs_par);
void mesh_light_ctl_temp_st_rsp_par_fill(mesh_cmd_light_ctl_temp_st_t *rsp, u8 idx);
int access_cmd_set_light_ctl_temp(u16 adr, u8 rsp_max, u16 temp, int ack, transition_par_t *trs_par);


#ifdef __cplusplus
}
#endif

#endif  // TENCENTCLOUD_IOT_EXPLORER_BLE_MESH_SDK_EMBEDDED_DATA_TEMPLATE_BLE_QIOT_TEMPLATE_VENDOR_H_