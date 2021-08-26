/********************************************************************************************************
 * @file     op_agg_model.h 
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


#pragma once

#include "proj/tl_common.h"
#include "proj_lib/sig_mesh/app_mesh.h"

enum{
	OP_AGG_SUC,
	OP_AGG_INVALID_ADDR,
	OP_AGG_INVALID_MODEL,
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

extern model_sig_op_agg_t model_sig_op_agg;
extern u32 mesh_md_op_agg_addr;

int is_op_agg_model(u32 model_id);
int is_in_op_agg_mode(u32 op);
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

