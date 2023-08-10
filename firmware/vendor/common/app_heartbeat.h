/********************************************************************************************************
 * @file	app_heartbeat.h
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
#ifndef APP_HEARTBEAT_H_
#define APP_HEARTBEAT_H_
#include "tl_common.h"
#include "proj_lib/sig_mesh/app_mesh.h"
#include "vendor/common/mesh_node.h"

typedef struct{
	u8 iniTTL:7;
	u8 rfu:1;
	u16 fea;
}mesh_hb_msg_t;

typedef struct{
	u16 src;
	u16 dst;
	mesh_hb_msg_t hb_msg;
}mesh_hb_msg_cb_t;

typedef enum{
	MESH_HB_RELAY_BIT = 0,
	MESH_HB_PROXY_BIT ,	
	MESH_HB_FRI_BIT ,	
	MESH_HB_LOWPOWER_BIT,
}mesh_heartbeat_feature_t;

typedef struct{
	u16 dst;
	u8 count_log;
	u8 period_log;
	u8 ttl;
	u16 features;
	u16 netkeyindex;
}mesh_cfg_model_heartbeat_pub_set_t;

typedef struct{
	u16 src ;
	u16 dst ;
	u8 period_log;
}mesh_cfg_model_heartbeat_sub_set_t;

typedef struct{
	u8 status;
	u16 src;
	u16 dst;
	u8 periodlog;
	u8 countlog;
	u8 min_hop;
	u8 max_hop;
}mesh_cfg_model_heartbeat_sub_status_t;

typedef struct{
	u8 status ;
	u16 dst;
	u8 countlog;
	u8 periodlog;
	u8 ttl;
	u16 feature;
	u16 netkeyIndex;
}mesh_heartbeat_pub_status_t;

extern u32 hb_pub_tick ;
extern u32 hb_sub_tick ;
extern u8 heartbeat_en ;
extern u8 hb_sts_change ;
extern u32 hb_pub_100ms ;
extern u32 hb_sub_100ms ;
extern u8 hb_sts_change ;

void init_heartbeat_str();
u8 cal_heartbeat_count_log(u16 val);
void mesh_heartbeat_poll_100ms();

void mesh_cmd_sig_lowpower_heartbeat();
void heartbeat_cmd_send_conf();
// heart beat part dispatch 
int mesh_cmd_sig_heart_pub_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_heartbeat_pub_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_heartbeat_pub_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_heartbeat_sub_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_heartbeat_sub_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_heartbeat_sub_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
void mesh_process_hb_sub(mesh_cmd_bear_t *p_bear);
void mesh_heartbeat_cb_data(u16 src, u16 dst,u8 *p_hb);

#endif
