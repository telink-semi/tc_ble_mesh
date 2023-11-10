/********************************************************************************************************
 * @file	subnet_bridge.h
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
#ifndef SUBNET_DIRECTED_H
#define SUBNET_DIRECTED_H
#include "tl_common.h"
#include "proj_lib/sig_mesh/app_mesh.h"
#include "directed_forwarding.h"
#include "solicitation_rpl_cfg_model.h"

#define MAX_BRIDGE_ENTRIES			0x10

#define SUBNET_BRIDGE_GET			0xB180//0xB180
#define SUBNET_BRIDGE_SET 			0xB280//0xB280
#define SUBNET_BRIDGE_STATUS		0xB380//0xB380
#define BRIDGING_TABLE_ADD			0xB480//0xB480
#define BRIDGING_TABLE_REMOVE		0xB580//0xB580
#define BRIDGING_TABLE_STATUS		0xB680//0xB680
#define BRIDGED_SUBNETS_GET 		0xB780//0xB780
#define BRIDGED_SUBNETS_LIST		0xB880//0xB880
#define BRIDGING_TABLE_GET			0xB980//0xB980
#define BRIDGING_TABLE_LIST			0xBA80//0xBA80
#define BRIDGE_TABLE_SIZE_GET	 	0xBB80//0xBB80
#define BRIDGE_TABLE_SIZE_STATUS	0xBC80//0xBC80

enum{
	SUBNET_BRIDGE_DISABLE,
	SUBNET_BRIDGE_ENABLE,
	SUBNET_BRIDGE_PROHIBITED,
};

enum{
	REPORT_ALL_PAIRS,
	REPORT_FIRST_SUBNET,
	REPORT_SND_SUBNET,
	REPORT_ANY_SUBNET,
};

enum{
	ONE_WAY_DIRECTION=1,
	TWO_WAY_DIRECTION=2,
};

typedef struct{
	u32 directions:8;
	u32 netkey_index1:12;
	u32 netkey_index2:12;
	u16 addr1;
	u16 addr2;
}mesh_bridge_entry_t;

#if MD_SBR_CFG_SERVER_EN
typedef struct{
	u8 bridge_en;
}model_bridge_cfg_common_t;

typedef struct{
	model_bridge_cfg_common_t srv;
	mesh_bridge_entry_t bridge_entry[MAX_BRIDGE_ENTRIES];
}model_bridge_cfg_t;
#endif

typedef struct{
	u32 netkey_index1:12;
	u32 netkey_index2:12;
}subnet_list_entry_t;

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
	u16 addr1;
	u16 addr2;
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
	u16 start_index;
	bridged_addr_lst_entry_t bridge_list[MAX_BRIDGE_ENTRIES];
}bridging_tbl_list_t;

typedef struct{
	u8 max_bridge_subnets;
	u8 max_bridge_tbls; // u8 in pts,but u16 in spec
}bridge_capa_st_t;

typedef struct{
#if MD_DF_CFG_SERVER_EN
	model_df_cfg_t df_cfg;
#endif
#if MD_SBR_CFG_SERVER_EN
	model_bridge_cfg_t bridge_cfg;  
#endif
#if MD_SOLI_PDU_RPL_EN
	model_sig_soli_pdu_rpl_t soli_pdu;
#endif
}model_g_df_sbr_t;

extern model_g_df_sbr_t model_sig_g_df_sbr_cfg; 
extern u32 mesh_md_df_sbr_addr;

int is_subnet_bridge_en();
int get_subnet_bridge_index(u16 netkey_index, u16 src, u16 dst);
int is_subnet_bridge_addr(u16 addr1, u16 addr2);
#if MD_SBR_CFG_SERVER_EN
void mesh_remove_node_dependent_by_subnet_bridge();
void mesh_subnet_bridge_bind_state_update();
int mesh_cmd_sig_cfg_subnet_bridge_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_subnet_bridge_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_bridging_tbl_add(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_bridging_tbl_remove(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_bridged_subnets_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_bridging_tbl_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_bridge_capa_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);

#else
#define mesh_cmd_sig_cfg_subnet_bridge_get		(0)
#define mesh_cmd_sig_cfg_subnet_bridge_set		(0)
#define mesh_cmd_sig_cfg_bridging_tbl_add		(0)
#define mesh_cmd_sig_cfg_bridging_tbl_remove	(0)
#define mesh_cmd_sig_cfg_bridged_subnets_get	(0)
#define mesh_cmd_sig_cfg_bridging_tbl_get		(0)
#define mesh_cmd_sig_cfg_bridge_capa_get		(0)
#endif

#if MD_SBR_CFG_CLIENT_EN
int mesh_cmd_sig_cfg_subnet_bridge_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_bridging_tbl_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_bridged_subnets_list(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_bridging_tbl_list(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_bridge_capa_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
#else
#define mesh_cmd_sig_cfg_subnet_bridge_status	(0)
#define mesh_cmd_sig_cfg_bridging_tbl_status	(0)
#define mesh_cmd_sig_cfg_bridged_subnets_list	(0)
#define mesh_cmd_sig_cfg_bridging_tbl_list		(0)
#define mesh_cmd_sig_cfg_bridge_capa_status		(0)
#endif
#endif

