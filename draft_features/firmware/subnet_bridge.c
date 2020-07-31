/********************************************************************************************************
 * @file     subnet_bridge.c 
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

#include "subnet_bridge.h"

#if (MD_SBR_EN)
model_bridge_cfg_t model_sig_bridge_cfg; 

#if MD_SERVER_EN
int mesh_cmd_sig_cfg_subnet_bridge_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	return mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&model_sig_bridge_cfg.srv.bridge_en),1,cb_par->adr_src);
}

int mesh_cmd_sig_cfg_subnet_bridge_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{	
	u8 subnet_bridge = par[0];
	if(subnet_bridge < SUBNET_BRIDGE_PROHIBITED){
		model_sig_bridge_cfg.srv.bridge_en = subnet_bridge;
		#if PTS_TEST_EN
		//mesh_common_store(FLASH_ADR_MD_SBR);
		#endif
	}
	return mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,&subnet_bridge,1,cb_par->adr_src);
}

int mesh_cmd_sig_cfg_bridging_tbl_add(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	mesh_bridge_entry_t *p_tbl_add = (mesh_bridge_entry_t *)par;
	bridging_tbl_sts_t bridging_tbl_sts;
	bridging_tbl_sts.status = ST_SUCCESS;
	memcpy((u8 *)&bridging_tbl_sts.status+1, p_tbl_add, sizeof(bridging_tbl_sts_t)-OFFSETOF(bridging_tbl_sts_t,status));
	foreach(i, MAX_BRIDGE_ENTRIES){
		if(0 == model_sig_bridge_cfg.bridge_entry[i].addr1){
			memcpy(&model_sig_bridge_cfg.bridge_entry[i], p_tbl_add, sizeof(mesh_bridge_entry_t));
		}
	}
	return mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&bridging_tbl_sts),sizeof(bridging_tbl_sts_t),cb_par->adr_src);
}

int mesh_cmd_sig_cfg_bridging_tbl_remove(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	bridging_tbl_remove_t *p_tbl_remove = (bridging_tbl_remove_t *)par;
	bridging_tbl_sts_t bridging_tbl_sts;
	bridging_tbl_sts.status = ST_SUCCESS;
	memcpy((u8 *)&bridging_tbl_sts.status+1, p_tbl_remove, sizeof(bridging_tbl_sts_t)-OFFSETOF(bridging_tbl_sts_t,status));
	foreach(i, MAX_BRIDGE_ENTRIES){
		if((p_tbl_remove->addr1 == model_sig_bridge_cfg.bridge_entry[i].addr1) && (p_tbl_remove->addr2 == model_sig_bridge_cfg.bridge_entry[i].addr2) &&
			(p_tbl_remove->netkey_index1 == model_sig_bridge_cfg.bridge_entry[i].netkey_index1) && (p_tbl_remove->netkey_index2 == model_sig_bridge_cfg.bridge_entry[i].netkey_index2)	
		){
			memset(&model_sig_bridge_cfg.bridge_entry[i], 0x00, sizeof(mesh_bridge_entry_t));
		}
	}
	return mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&bridging_tbl_sts),sizeof(bridging_tbl_sts_t),cb_par->adr_src);
}

int mesh_cmd_sig_cfg_bridged_subnets_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	// bridged_subnets_list_t subnets_list;
	return mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&model_sig_bridge_cfg.srv.bridge_en),1,cb_par->adr_src);
}

int mesh_cmd_sig_cfg_bridging_tbl_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	return mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&model_sig_bridge_cfg.srv.bridge_en),1,cb_par->adr_src);
}

#endif

#if MD_CLIENT_EN
int mesh_cmd_sig_cfg_subnet_bridge_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err = 0;
	if(cb_par->model){	// model may be Null for status message
	}
	return err;
}

int mesh_cmd_sig_cfg_bridging_tbl_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err = 0;
	if(cb_par->model){	// model may be Null for status message
	}
	return err;
}

int mesh_cmd_sig_cfg_bridged_subnets_list(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err = 0;
	if(cb_par->model){	// model may be Null for status message
	}
	return err;
}

int mesh_cmd_sig_cfg_bridging_tbl_list(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err = 0;
	if(cb_par->model){	// model may be Null for status message
	}
	return err;
}

#endif
#endif
