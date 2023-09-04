/********************************************************************************************************
 * @file	config_model.h
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
#pragma once

#include "tl_common.h"
#include "proj_lib/sig_mesh/app_mesh.h"

// to increase efficiency
STATIC_ASSERT(sizeof(model_common_t) % 4 == 0);
STATIC_ASSERT(sizeof(model_g_light_s_t) % 4 == 0);

static inline void mesh_model_store_cfg_s()
{
	mesh_model_store(1, SIG_MD_CFG_SERVER);
}

typedef struct{
	u8 page;
	u16 offset;
}large_cps_get_t;

typedef struct{
	u8 page;
	u16 offset;
	u16 total_size;
	u8 data[1];
}large_cps_st_head_t;

typedef struct{
	u8 meta_page;
	u16 offset;
}models_meta_get_t;

typedef struct{
	u8 page;
	u16 offset;
	u16 total_size;
	u8 data[1];
}models_meta_status_head_t;

extern u32 mesh_on_demand_proxy_time;

u8 mesh_get_network_transmit();
u8 mesh_get_relay_retransmit();
u8 mesh_get_ttl();
u8 mesh_get_hb_pub_ttl();
u8 mesh_get_gatt_proxy();
u8 mesh_get_friend();
u8 mesh_get_relay();
u8 mesh_cmd_sig_cfg_model_sub_set2(u16 op, u16 sub_adr, u8 *uuid, model_common_t *p_model, u32 model_id, bool4 sig_model);
int mesh_sec_prov_comfirmation_key_fun(unsigned char *key, unsigned char *input, int n, unsigned char ecdh[32],unsigned char auth[32],u8 hmac);
int mesh_sec_prov_confirmation_fun(unsigned char *cfm, unsigned char *input, int n, unsigned char ecdh[32],
				unsigned char random[32],unsigned char auth[32],u8 hmac);
void mesh_sec_prov_salt_fun(unsigned char prov_salt[16],unsigned char *input,unsigned char randomProv[32], unsigned char randomDev[32],u8 hmac);
int mesh_sec_prov_session_key_fun(unsigned char sk[16], unsigned char *sn, unsigned char *input, int n, unsigned char ecdh[32],
									unsigned char randomProv[32], unsigned char randomDev[32],u8 hmac);

int mesh_cmd_sig_cfg_sar_transmitter_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_sar_transmitter_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_sar_receiver_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_sar_receiver_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_on_demand_proxy_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_on_demand_proxy_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
#if (MD_SERVER_EN)
int mesh_cmd_sig_cfg_large_cps_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_models_metadata_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
#else
#define mesh_cmd_sig_cfg_large_cps_get									(0)
#define mesh_cmd_sig_cfg_models_metadata_get							(0)
#endif

#if MD_CLIENT_EN
int mesh_cmd_sig_cfg_sar_transmitter_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_sar_receiver_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_on_demand_proxy_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_large_cps_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_models_metadata_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
#else
#define mesh_cmd_sig_cfg_sar_transmitter_status							(0)
#define mesh_cmd_sig_cfg_sar_receiver_status							(0)
#define mesh_cmd_sig_cfg_on_demand_proxy_status							(0)
#define mesh_cmd_sig_cfg_large_cps_status								(0)
#define mesh_cmd_sig_cfg_models_metadata_status							(0)
#endif

