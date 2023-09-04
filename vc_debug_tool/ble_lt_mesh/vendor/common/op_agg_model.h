/********************************************************************************************************
 * @file	op_agg_model.h
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

#include "proj/tl_common.h"
#include "proj_lib/sig_mesh/app_mesh.h"

enum{
	OP_AGG_SUC,
	OP_AGG_INVALID_ADDR,
	OP_AGG_WRONG_KEY,
	OP_AGG_WRONG_OP,
	OP_AGG_UNKNOWN_MSG,
};

typedef struct{
 	u8 format:1;
	u8 length:7;
	u8 acc_par[1];
}agg_item_short_t;

typedef struct{
	u16 format:1;
	u16 length:15;
	u8 acc_par[1];
}agg_item_long_t;

typedef struct{
	u16 ele_addr;
	u8 par[1];
}op_agg_seq_t;

typedef struct{
	u16 op_rsp; // reserve for rsp_op
	u8 status;
	u16 ele_addr;
	u8 par[1];
}op_agg_seq_st_t;


typedef struct{
	#if MD_SERVER_EN
	model_g_light_s_t srv;			// server
    #endif
	#if MD_CLIENT_EN
	model_client_common_t clnt;		        // client
	#endif
}model_sig_op_agg_t;

extern _align_4_ model_sig_op_agg_t model_sig_op_agg;
extern u32 mesh_md_op_agg_addr;

int is_op_agg_model(u32 model_id);
int is_in_op_agg_mode();
int is_op_agg_use_device_key(u8 *par, int len);
int mesh_op_agg_status_item_format_add(u32 len);

#if (MD_SERVER_EN)
int mesh_cmd_sig_cfg_op_agg_seq(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
#else							
#define mesh_cmd_sig_cfg_op_agg_seq										(0)
#endif

#if MD_CLIENT_EN
int mesh_cmd_sig_cfg_op_agg_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
#else
#define mesh_cmd_sig_cfg_op_agg_status									(0)
#endif

