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

// refer to https://www.bluetooth.com/specifications/assigned-numbers
// --- mesh_metadata_ids:
#define METADATA_ID_HEALTH_TESTS_INFORMATION		0x0000
#define METADATA_ID_SENSOR_PROPERTIES				0x0001
#define METADATA_ID_LIGHT_PURPOSE					0x0002
#define METADATA_ID_LIGHT_LIGHTNESS_RANGE			0x0003
#define METADATA_ID_LIGHT_CTL_TEMPERATURE_RANGE		0x0004
#define METADATA_ID_LIGHT_HSL_HUE_RANGE				0x0005
#define METADATA_ID_LIGHT_HSL_SATURATION_RANGE		0x0006
#define METADATA_ID_CLOCK_ACCURACY					0x0007
#define METADATA_ID_TIMEKEEPING_RESERVE				0x0008

// --- mmdl_light_purposes:
#define LIGHT_PURPOSE_UPLIGHT						0x0000
#define LIGHT_PURPOSE_UPLIGHT_LEFT					0x0001
#define LIGHT_PURPOSE_UPLIGHT_CENTER				0x0002
#define LIGHT_PURPOSE_UPLIGHT_RIGHT					0x0003
#define LIGHT_PURPOSE_DOWNLIGHT						0x0004
#define LIGHT_PURPOSE_DOWNLIGHT_LEFT				0x0005
#define LIGHT_PURPOSE_DOWNLIGHT_CENTER				0x0006
#define LIGHT_PURPOSE_DOWNLIGHT_RIGHT				0x0007
#define LIGHT_PURPOSE_INSIDE						0x0008
#define LIGHT_PURPOSE_OUTSIDE						0x0009
#define LIGHT_PURPOSE_BACKLIGHT						0x000A
#define LIGHT_PURPOSE_FLOODLIGHT					0x000B
#define LIGHT_PURPOSE_TASKLIGHT						0x000C
#define LIGHT_PURPOSE_TASKLIGHT_LEFT				0x000D
#define LIGHT_PURPOSE_TASKLIGHT_CENTER				0x000E
#define LIGHT_PURPOSE_TASKLIGHT_RIGHT				0x000F
#define LIGHT_PURPOSE_WARMING_LIGHT					0x0010
#define LIGHT_PURPOSE_EMERGENCY_LIGHT				0x0011
#define LIGHT_PURPOSE_NIGHT_LIGHT					0x0012
#define LIGHT_PURPOSE_INDICATOR_LIGHT				0x0013
#define LIGHT_PURPOSE_UNDERCABINET_LIGHT			0x0014
#define LIGHT_PURPOSE_ACCENT_LIGHT					0x0015
#define LIGHT_PURPOSE_STRIP_LIGHT					0x0016
#define LIGHT_PURPOSE_TROFFER_LIGHT					0x0017
#define LIGHT_PURPOSE_HIGH_BAY_LIGHT				0x0018
#define LIGHT_PURPOSE_WALL_PACK_LIGHT				0x0019


#if MD_ON_DEMAND_PROXY_EN
extern u32 mesh_on_demand_proxy_time;
void mesh_on_demand_private_gatt_proxy_start();
void mesh_on_demand_private_gatt_proxy_stop();
int mesh_on_demand_is_valid_st_to_rx_solicitation();
#endif

int is_publish_allow();
u8 mesh_get_network_transmit();
u8 mesh_get_relay_retransmit();
u8 mesh_get_ttl();
u8 mesh_get_hb_pub_ttl();
u8 mesh_get_gatt_proxy();
u8 mesh_get_friend();
u8 mesh_get_relay();
u8 mesh_cmd_sig_cfg_model_sub_set2(u16 op, u16 sub_adr, u8 *uuid, model_common_t *p_model, u32 model_id, bool4 sig_model);
int mesh_sec_prov_confirmation_key_fun(unsigned char *key, unsigned char *input, int n, unsigned char ecdh[32],unsigned char auth[32],u8 hmac);
int mesh_sec_prov_confirmation_fun(unsigned char *cfm, unsigned char *input, int n, unsigned char ecdh[32],
				unsigned char random[32],unsigned char auth[32],u8 hmac);
void mesh_sec_prov_salt_fun(unsigned char prov_salt[16],unsigned char *input,unsigned char randomProv[32], unsigned char randomDev[32],u8 hmac);
int mesh_sec_prov_session_key_fun(unsigned char sk[16], unsigned char *sn, unsigned char *input, int n, unsigned char ecdh[32],
									unsigned char randomProv[32], unsigned char randomDev[32],u8 hmac);
int mesh_get_on_demand_private_proxy();
u8 mesh_get_private_proxy();
u8 mesh_get_private_node_identity();
int is_existed_sub_addr_and_not_virtual(model_common_t *p_model, u16 group_addr);


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

