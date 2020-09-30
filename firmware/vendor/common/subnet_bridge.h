/********************************************************************************************************
 * @file     subnet_bridge.h 
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

#ifndef SUBNET_DIRECTED_H
#define SUBNET_DIRECTED_H
#include "proj/tl_common.h"
#include "proj_lib/sig_mesh/app_mesh.h"

#define MAX_BRIDGE_ENTRIES			0x10

#define SUBNET_BRIDGE_GET			0x65BF
#define SUBNET_BRIDGE_SET 			0x66BF
#define SUBNET_BRIDGE_STATUS		0x67BF
#define BRIDGING_TABLE_ADD			0x68BF
#define BRIDGING_TABLE_REMOVE		0x69BF
#define BRIDGING_TABLE_STATUS		0x6ABF
#define BRIDGED_SUBNETS_GET 		0x6BBF
#define BRIDGED_SUBNETS_LIST		0x6CBF
#define BRIDGING_TABLE_GET			0x6DBF
#define BRIDGING_TABLE_LIST			0x6EBF
#define BRIDGING_TABLE_SIZE_GET	 	0x6FBF
#define BRIDGING_TABLE_SIZE_STATUS	0x70BF

enum{
	SUBNET_BRIDGE_DISABLE,
	SUBNET_BRIDGE_ENABLE,
	SUBNET_BRIDGE_PROHIBITED,
};

typedef struct{
	u32 directions:8;
	u32 netkey_index1:12;
	u32 netkey_index2:12;
	u16 addr1;
	u16 addr2;
}mesh_bridge_entry_t;

typedef struct{
	model_common_t com;
	u8 bridge_en;
}model_bridge_cfg_common_t;

typedef struct{
#if MD_SERVER_EN
	model_bridge_cfg_common_t srv;
	mesh_bridge_entry_t bridge_entry[MAX_BRIDGE_ENTRIES];
#endif
#if MD_CLIENT_EN
	model_client_common_t clnt;
#endif
}model_bridge_cfg_t;

typedef struct{
	u64 netkey_index1:12;
	u64 netkey_index2:12;
	u64 addr1:16;
	u64 addr2:16;
}bridging_tbl_remove_t;

typedef struct{
	u8 status;
	u32 current_directions:8;	
	u32 netkey_index1:12;
	u32 netkey_index2:12;
	u16 addr1:16;
	u16 addr2:16;
}bridging_tbl_sts_t;

typedef struct{
	u32 filter:2;	
	u32 prohibited:2;
	u32 netkey_index:12;
	u32 start_index:8;
}bridged_subnets_get_t;

typedef struct{
	u16 filter:2;	
	u16 prohibited:2;
	u16 netkey_index:12;
	u8 start_index;
	u8 par[MAX_BRIDGE_ENTRIES*3];
}bridged_subnets_list_t;

typedef struct{
	u64 netkey_index1:12;	
	u64 netkey_index2:12;
	u64 start_index:16;
}bridging_tbl_get_t;

typedef struct{
	u16 addr1;
	u16 addr2;
	u8 directions;
}bridged_addr_lst_entry_t; 

typedef struct{
	u32 status:8;
	u32 netkey_index1:12;	
	u32 netkey_index2:12;
	u16 start_index:16;
	bridged_addr_lst_entry_t bridge_list[MAX_BRIDGE_ENTRIES];
}bridging_tbl_list_t;

extern model_bridge_cfg_t model_sig_bridge_cfg; 

#if MD_SERVER_EN
int mesh_cmd_sig_cfg_subnet_bridge_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_subnet_bridge_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_bridging_tbl_add(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_bridging_tbl_remove(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_bridged_subnets_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_bridging_tbl_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
#else
#define mesh_cmd_sig_cfg_subnet_bridge_get		(0)
#define mesh_cmd_sig_cfg_subnet_bridge_set		(0)
#define mesh_cmd_sig_cfg_bridging_tbl_add		(0)
#define mesh_cmd_sig_cfg_bridging_tbl_remove	(0)
#define mesh_cmd_sig_cfg_bridged_subnets_get	(0)
#define mesh_cmd_sig_cfg_bridging_tbl_get		(0)
#endif

#if MD_CLIENT_EN
int mesh_cmd_sig_cfg_subnet_bridge_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_bridging_tbl_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_bridged_subnets_list(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_bridging_tbl_list(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
#else
#define mesh_cmd_sig_cfg_subnet_bridge_status	(0)
#define mesh_cmd_sig_cfg_bridging_tbl_status	(0)
#define mesh_cmd_sig_cfg_bridged_subnets_list	(0)
#define mesh_cmd_sig_cfg_bridging_tbl_list		(0)
#endif
#endif

