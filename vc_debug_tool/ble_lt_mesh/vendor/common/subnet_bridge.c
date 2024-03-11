/********************************************************************************************************
 * @file	subnet_bridge.c
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
#include "subnet_bridge.h"
#include "directed_forwarding.h"

#if MD_SBR_CFG_CLIENT_EN
STATIC_ASSERT(MD_CLIENT_EN);
#endif

#if (MD_DF_CFG_SERVER_EN || MD_SBR_CFG_SERVER_EN || MD_SOLI_PDU_RPL_EN)
model_g_df_sbr_t model_sig_g_df_sbr_cfg;
u32 mesh_md_df_sbr_addr = FLASH_ADR_MD_DF_SBR;
#if !WIN32
STATIC_ASSERT(MD_PROPERTY_EN == 0); // use the same flash sector to save par in mesh_md_adr_map, so can not enable at the same time.
#if GATEWAY_ENABLE
STATIC_ASSERT(FLASH_ADR_MD_DF_SBR != FLASH_ADR_VC_NODE_INFO);
#endif
#endif
#endif

#if (MD_SBR_CFG_SERVER_EN)
int is_subnet_bridge_en()
{
	return model_sig_g_df_sbr_cfg.bridge_cfg.srv.bridge_en;
}

int is_subnet_bridge_addr(u16 addr1, u16 addr2)
{
	for(u8 i=0; i<MAX_BRIDGE_ENTRIES; i++){
		mesh_bridge_entry_t *p_entry = (mesh_bridge_entry_t *)&model_sig_g_df_sbr_cfg.bridge_cfg.bridge_entry[i];
		if((addr1 == p_entry->addr1) && (addr2 == p_entry->addr2)){
			return 1;
		}
		else if(TWO_WAY_DIRECTION == p_entry->directions){
			if((addr2 == p_entry->addr1) && (addr1 == p_entry->addr2)){
				return 1;
			}
		}
	}
	return 0;
}

int get_subnet_bridge_index(u16 netkey_index, u16 src, u16 dst)
{
	for(u8 i=0; i<MAX_BRIDGE_ENTRIES; i++){
		mesh_bridge_entry_t *p_entry = (mesh_bridge_entry_t *)&model_sig_g_df_sbr_cfg.bridge_cfg.bridge_entry[i];		
		if(p_entry->addr1){
			if((src == p_entry->addr1) && (dst == p_entry->addr2) && (netkey_index == p_entry->netkey_index1)	){
				return p_entry->netkey_index2;
			}
			else if((src == p_entry->addr2) && (dst == p_entry->addr1) && (netkey_index == p_entry->netkey_index2) && (TWO_WAY_DIRECTION == p_entry->directions)){
				return p_entry->netkey_index1;
			}
		}
		else{
			break;
		}
	}
	return -1;
}

void mesh_sort_bridge_tbl()
{
	u8 i,j;
	for(i=0; i<MAX_BRIDGE_ENTRIES; i++){
		if(model_sig_g_df_sbr_cfg.bridge_cfg.bridge_entry[i].addr1){
			continue;
		}
		
	 	for(j=i+1; j<MAX_BRIDGE_ENTRIES; j++){
			if(model_sig_g_df_sbr_cfg.bridge_cfg.bridge_entry[j].addr1){
				model_sig_g_df_sbr_cfg.bridge_cfg.bridge_entry[i] = model_sig_g_df_sbr_cfg.bridge_cfg.bridge_entry[j];	
				memset(&model_sig_g_df_sbr_cfg.bridge_cfg.bridge_entry[j], 0, sizeof(mesh_bridge_entry_t));
				break;
			}
		}		

		if(MAX_BRIDGE_ENTRIES == j) break;
	}
}

void mesh_remove_node_dependent_by_subnet_bridge()
{
	#if (MD_DF_CFG_SERVER_EN && !FEATURE_LOWPOWER_EN)
	foreach(i, MAX_BRIDGE_ENTRIES){
		mesh_bridge_entry_t *p_entry = (mesh_bridge_entry_t *)&model_sig_g_df_sbr_cfg.bridge_cfg.bridge_entry[i];
		if(model_sig_g_df_sbr_cfg.bridge_cfg.bridge_entry[i].addr1){
			foreach(netkey_offset, NET_KEY_MAX){
				directed_forwarding_dependents_update_start(netkey_offset, DEPENDENT_TYPE_REMOVE, ele_adr_primary, p_entry->addr1, 1);
				directed_forwarding_dependents_update_start(netkey_offset, DEPENDENT_TYPE_REMOVE, ele_adr_primary, p_entry->addr2, 1);
			}
		}
	}
	#endif
}

void mesh_subnet_bridge_bind_state_update()
{
	foreach(i, MAX_BRIDGE_ENTRIES){
		mesh_bridge_entry_t *p_entry = (mesh_bridge_entry_t *)&model_sig_g_df_sbr_cfg.bridge_cfg.bridge_entry[i];
		if(model_sig_g_df_sbr_cfg.bridge_cfg.bridge_entry[i].addr1){
			if((0==is_mesh_net_key_exist(p_entry->netkey_index1)) || (0==is_mesh_net_key_exist(p_entry->netkey_index2))){
				memset(p_entry, 0, sizeof(mesh_bridge_entry_t));
			}
		}
	}

	mesh_sort_bridge_tbl();
}

int mesh_cmd_sig_cfg_subnet_bridge_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	return mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&model_sig_g_df_sbr_cfg.bridge_cfg.srv.bridge_en),1,cb_par->adr_src);
}

int mesh_cmd_sig_cfg_subnet_bridge_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{	
	u8 subnet_bridge = par[0];
	if(subnet_bridge < SUBNET_BRIDGE_PROHIBITED){
		model_sig_g_df_sbr_cfg.bridge_cfg.srv.bridge_en = subnet_bridge;
		mesh_model_store(1, SIG_MD_BRIDGE_CFG_SERVER);
	}
	else{
		return -1;
	}
	
	int err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,&subnet_bridge,1,cb_par->adr_src);
	#if MD_DF_CFG_SERVER_EN
	if(0 == subnet_bridge){
		mesh_remove_node_dependent_by_subnet_bridge();		
	}
	#endif

	return err;
}

int mesh_cmd_sig_cfg_bridging_tbl_add(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	mesh_bridge_entry_t *p_tbl_add = (mesh_bridge_entry_t *)par;
	bridging_tbl_sts_t bridging_tbl_sts;
	bridging_tbl_sts.status = ST_SUCCESS;
	memcpy((u8 *)&bridging_tbl_sts.status+1, p_tbl_add, sizeof(bridging_tbl_sts_t)-OFFSETOF(bridging_tbl_sts_t,status));
	if( (p_tbl_add->netkey_index1 == p_tbl_add->netkey_index2) || (p_tbl_add->addr1 == p_tbl_add->addr2) || (!is_unicast_adr(p_tbl_add->addr1)) ||
		(ADR_UNASSIGNED==p_tbl_add->addr1) || (ADR_UNASSIGNED==p_tbl_add->addr2) || 
		(p_tbl_add->directions < ONE_WAY_DIRECTION) || (p_tbl_add->directions > TWO_WAY_DIRECTION) ||  
		((ONE_WAY_DIRECTION==p_tbl_add->directions) && (ADR_ALL_NODES==p_tbl_add->addr2)) ||
		((TWO_WAY_DIRECTION==p_tbl_add->directions) && !is_unicast_adr(p_tbl_add->addr2))){
		return -1;
	}
	
	if(is_mesh_net_key_exist(p_tbl_add->netkey_index1) && is_mesh_net_key_exist(p_tbl_add->netkey_index2)){
		foreach(i, MAX_BRIDGE_ENTRIES){
			mesh_bridge_entry_t *p_entry = (mesh_bridge_entry_t *)&model_sig_g_df_sbr_cfg.bridge_cfg.bridge_entry[i];
			if(	(0 == model_sig_g_df_sbr_cfg.bridge_cfg.bridge_entry[i].addr1) ||
				((p_tbl_add->netkey_index1 == p_entry->netkey_index1) && (p_tbl_add->netkey_index2 == p_entry->netkey_index2) && (p_tbl_add->addr1 == p_entry->addr1) && (p_tbl_add->addr2 == p_entry->addr2))
			){
				memcpy(&model_sig_g_df_sbr_cfg.bridge_cfg.bridge_entry[i], p_tbl_add, sizeof(mesh_bridge_entry_t));
				mesh_model_store(1, SIG_MD_BRIDGE_CFG_SERVER);
				#if MD_DF_CFG_SERVER_EN
				cfg_cmd_send_path_solicitation(p_tbl_add->netkey_index2, &p_tbl_add->addr2, 1);
				if(TWO_WAY_DIRECTION==p_tbl_add->directions){
					cfg_cmd_send_path_solicitation(p_tbl_add->netkey_index1, &p_tbl_add->addr1, 1);
				}
				#endif
				break;
			}

			if((MAX_BRIDGE_ENTRIES-1) == i){
				bridging_tbl_sts.status = ST_INSUFFICIENT_RES;
			}
		}
	}
	else{
		bridging_tbl_sts.status = ST_INVALID_NETKEY;
	}
	return mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&bridging_tbl_sts),sizeof(bridging_tbl_sts_t),cb_par->adr_src);
}

int mesh_cmd_sig_cfg_bridging_tbl_remove(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	bridging_tbl_remove_t *p_tbl_remove = (bridging_tbl_remove_t *)par;
	bridging_tbl_sts_t bridging_tbl_sts;
	bridging_tbl_sts.status = ST_SUCCESS;
	bridging_tbl_sts.current_directions = 0;
	memcpy((u8 *)&bridging_tbl_sts.status + 2, p_tbl_remove, sizeof(bridging_tbl_sts_t)-OFFSETOF(bridging_tbl_sts_t,status));
	u8 remove = 0;

	if(!is_unicast_adr(p_tbl_remove->addr1) || (ADR_ALL_NODES == p_tbl_remove->addr2) || (p_tbl_remove->netkey_index1 == p_tbl_remove->netkey_index2) ||
		(p_tbl_remove->addr1 && (p_tbl_remove->addr1 == p_tbl_remove->addr2))){
		return -1;
	}
	
	if(is_mesh_net_key_exist(p_tbl_remove->netkey_index1) && is_mesh_net_key_exist(p_tbl_remove->netkey_index2)){
		foreach(i, MAX_BRIDGE_ENTRIES){
			mesh_bridge_entry_t *p_entry = (mesh_bridge_entry_t *)&model_sig_g_df_sbr_cfg.bridge_cfg.bridge_entry[i];
			if(p_entry->addr1 && (p_tbl_remove->netkey_index1 == p_entry->netkey_index1) && (p_tbl_remove->netkey_index2 == p_entry->netkey_index2)){
				if(((p_tbl_remove->addr1 == p_entry->addr1) && (p_tbl_remove->addr2 == p_entry->addr2)) ||
					((p_tbl_remove->addr1 == p_entry->addr1) && (ADR_UNASSIGNED == p_tbl_remove->addr2)) ||
					((ADR_UNASSIGNED == p_tbl_remove->addr1) && (p_tbl_remove->addr2 == p_entry->addr2)) ||
					((ADR_UNASSIGNED == p_tbl_remove->addr1) && (ADR_UNASSIGNED == p_tbl_remove->addr2))
				){
					memset(p_entry, 0x00, sizeof(mesh_bridge_entry_t));		
					remove = 1;
				}
			}
		}
	}
	else{
		bridging_tbl_sts.status = ST_INVALID_NETKEY;
	}
	
	if(remove){
		#if (MD_DF_CFG_SERVER_EN && !FEATURE_LOWPOWER_EN)
		foreach(netkey_offset, NET_KEY_MAX){
			directed_forwarding_dependents_update_start(netkey_offset, DEPENDENT_TYPE_REMOVE, ele_adr_primary, p_tbl_remove->addr1, 1);
			directed_forwarding_dependents_update_start(netkey_offset, DEPENDENT_TYPE_REMOVE, ele_adr_primary, p_tbl_remove->addr2, 1);
		}
		#endif
		mesh_sort_bridge_tbl();
		mesh_model_store(1, SIG_MD_BRIDGE_CFG_SERVER);
	}
	return mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&bridging_tbl_sts),sizeof(bridging_tbl_sts_t),cb_par->adr_src);
}

int mesh_cmd_sig_cfg_bridged_subnets_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	bridged_subnets_get_t *p_subnets_get = (bridged_subnets_get_t *)par;
	bridged_subnets_list_t subnets_list;
	u16 via_len = 0;
	memcpy(&subnets_list, p_subnets_get, sizeof(bridged_subnets_get_t));

	if(p_subnets_get->prohibited){
		return -1;
	}
	
	for(int i=p_subnets_get->start_index; i<MAX_BRIDGE_ENTRIES; i++){
		if(model_sig_g_df_sbr_cfg.bridge_cfg.bridge_entry[i].addr1){
			mesh_bridge_entry_t *p_entry = (mesh_bridge_entry_t *)&model_sig_g_df_sbr_cfg.bridge_cfg.bridge_entry[i];
			subnet_list_entry_t *p_list = (subnet_list_entry_t *)(subnets_list.par+via_len);
			if((REPORT_ALL_PAIRS == p_subnets_get->filter) || 
				((REPORT_FIRST_SUBNET == p_subnets_get->filter) && (p_subnets_get->netkey_index == p_entry->netkey_index1)) ||
				((REPORT_SND_SUBNET == p_subnets_get->filter) && (p_subnets_get->netkey_index == p_entry->netkey_index2)) ||
				((REPORT_ANY_SUBNET == p_subnets_get->filter) && ((p_subnets_get->netkey_index == p_entry->netkey_index1) || (p_subnets_get->netkey_index == p_entry->netkey_index2)))
			){
				p_list->netkey_index1 = p_entry->netkey_index1;
				p_list->netkey_index2 = p_entry->netkey_index2;
				via_len += 3;
			}
			
		}
	}

	return mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)&subnets_list,via_len+OFFSETOF(bridged_subnets_list_t, par),cb_par->adr_src);
}

int mesh_cmd_sig_cfg_bridging_tbl_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	bridging_tbl_get_t *p_tbl_get = (bridging_tbl_get_t *)par;
	bridging_tbl_list_t tbl_list;
	tbl_list.status = ST_SUCCESS;
	u8 addr_cnt = 0;
	memcpy((u8 *)&tbl_list+1, p_tbl_get, 5);
	if(0 == model_sig_g_df_sbr_cfg.bridge_cfg.srv.bridge_en){
		//whether delete table or not
	}
	else if(is_mesh_net_key_exist(p_tbl_get->netkey_index1) && is_mesh_net_key_exist(p_tbl_get->netkey_index2)){
		for(u8 i=p_tbl_get->start_index; i<MAX_BRIDGE_ENTRIES; i++){
			mesh_bridge_entry_t *p_entry = (mesh_bridge_entry_t *)&model_sig_g_df_sbr_cfg.bridge_cfg.bridge_entry[i];
			if(model_sig_g_df_sbr_cfg.bridge_cfg.bridge_entry[i].addr1){
				if((p_tbl_get->netkey_index1 == p_entry->netkey_index1) && (p_tbl_get->netkey_index2 == p_entry->netkey_index2)){
					tbl_list.bridge_list[addr_cnt].addr1 = p_entry->addr1;
					tbl_list.bridge_list[addr_cnt].addr2 = p_entry->addr2;
					tbl_list.bridge_list[addr_cnt].directions = p_entry->directions;
					addr_cnt++;
				}
			}
		}
	}
	else{
		tbl_list.status = ST_INVALID_NETKEY;
	}
	
	return mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)&tbl_list,OFFSETOF(bridging_tbl_list_t, bridge_list)+addr_cnt*sizeof(bridged_addr_lst_entry_t),cb_par->adr_src);
}

int mesh_cmd_sig_cfg_bridge_capa_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	bridge_capa_st_t bridge_capa;
	bridge_capa.max_bridge_subnets = NET_KEY_MAX;
	bridge_capa.max_bridge_tbls = MAX_BRIDGE_ENTRIES;
	
	return mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)&bridge_capa,sizeof(bridge_capa_st_t),cb_par->adr_src);
}
#endif

#if MD_SBR_CFG_CLIENT_EN
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

int mesh_cmd_sig_cfg_bridge_capa_list(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err = 0;
	if(cb_par->model){	// model may be Null for status message
	}
	return err;
}

#endif
