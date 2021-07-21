/********************************************************************************************************
 * @file     telink_sdk_mesh_api.h
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
#ifndef _TELINK_SDK_MESH_API_C_
#define _TELINK_SDK_MESH_API_C_
#include "./mijia_ble_api/mible_api.h"
#include "proj/tl_common.h"
#include "proj_lib/ble/ll/ll.h"
#include "proj_lib/ble/blt_config.h"
#include "vendor/common/user_config.h"
#include "proj_lib/ble/service/ble_ll_ota.h"
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
