/********************************************************************************************************
 * @file	telink_sdk_mesh_api.h
 *
 * @brief	for TLSR chips
 *
 * @author	telink
 * @date	Sep. 30, 2010
 *
 * @par     Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/
#ifndef _TELINK_SDK_MESH_API_C_
#define _TELINK_SDK_MESH_API_C_
#include "./mijia_ble_api/mible_api.h"
#include "tl_common.h"
#include "proj_lib/ble/blt_config.h"
#include "vendor/common/user_config.h"
#include "../app_health.h"
#include "proj_lib/sig_mesh/app_mesh.h"
#include "proj_lib/mesh_crypto/le_crypto.h"
#include "../mesh_common.h"
#include "../ev.h"
#if MI_API_ENABLE

#define MI_REGSUCC_TIMEOUT                      5000
extern void *mible_conn_timer ;
extern uint16_t mible_conn_handle ;
void process_mesh_node_init_event(void);
void process_mesh_node_reset();
int32_t generic_on_off_server_data(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int32_t light_lightness_server_data(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int32_t light_ctl_server_data(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int32_t miot_server_data(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int32_t mijia_server_data(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int cfg_server_receive_peek(u8 *par, int par_len,mesh_cb_fun_par_t *cb_par);
int mible_mesh_device_deinit_stack(void);
int mible_mesh_device_init_node(void);
int mible_mesh_device_set_provsion_data(mible_mesh_provisioning_data_t *param);

int mible_mesh_device_provsion_done(void);
int mible_mesh_node_reset(void);
int mible_mesh_device_unprovsion_done(void);
int mible_mesh_device_login_done(uint8_t status);
int mible_mesh_device_set_network_transmit_param(uint8_t count, uint8_t interval_steps);
int mible_mesh_device_set_relay(uint8_t enabled, uint8_t count, uint8_t interval);
int mible_mesh_device_get_relay(uint8_t *enabled, uint8_t *count, uint8_t *step);
int mible_mesh_device_get_seq(uint16_t element, uint32_t* seq, uint32_t* iv, uint8_t* flags);
int mible_mesh_device_update_iv_info(uint32_t iv_index, uint8_t flags);
int mible_mesh_device_set_netkey(mible_mesh_op_t op, uint16_t netkey_index, uint8_t *netkey);
int mible_mesh_device_set_appkey(mible_mesh_op_t op, uint16_t netkey_index, uint16_t appkey_index, uint8_t * appkey);
int mible_mesh_device_set_model_app(mible_mesh_op_t op, uint16_t elem_index, uint16_t company_id, uint16_t model_id, uint16_t appkey_index);
int mible_mesh_device_set_sub_address(mible_mesh_op_t op, uint16_t element, uint16_t company_id, uint16_t model_id, uint16_t sub_addr);
int mible_mesh_node_generic_control(mible_mesh_access_message_t *param);
uint64_t mible_mesh_get_exact_systicks(void);
void mible_conn_timeout_cb(void *p_context);
int mesh_mi_rx_cb(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
u16 mesh_mi_trans_op_little2big(u16 op);
u16 mesh_mi_trans_op_big2little(u16 op);
void process_mesh_ivi_update();

#endif

















#endif
