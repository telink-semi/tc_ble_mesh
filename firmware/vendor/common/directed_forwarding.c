/********************************************************************************************************
 * @file	directed_forwarding.c
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
#include "directed_forwarding.h"
#include "mesh_node.h"
#include "subnet_bridge.h"
#include "solicitation_rpl_cfg_model.h"

//--------------------network layer callback------------------------------//
#if !WIN32
#if (MD_DF_CFG_SERVER_EN || MD_SBR_CFG_SERVER_EN)
int mesh_rc_network_layer_df_sbr_cb(mesh_cmd_bear_t *p_bear, mesh_nw_retrans_t *p_nw_retrans)
{
#if MD_DF_CFG_SERVER_EN
	path_entry_com_t *p_fwd_entry = 0;
	if(p_nw_retrans->relay_en && is_directed_forwarding_en(p_nw_retrans->netkey_sel_enc)){			
		if(MESH_BEAR_ADV == p_nw_retrans->src_bear){ 
			if(DIRECTED == mesh_key.sec_type_sel){
				p_nw_retrans->relay_en = 0;
				if(is_directed_relay_en(p_nw_retrans->netkey_sel_enc)){
					if(get_forwarding_entry(p_nw_retrans->netkey_sel_enc, p_bear->nw.src, p_bear->nw.dst)){
						p_nw_retrans->sec_type = DIRECTED;
						p_nw_retrans->relay_en = 1;
					}
				}
			}
			else if(FRIENDSHIP == mesh_key.sec_type_sel){ 
				p_fwd_entry = get_forwarding_entry(p_nw_retrans->netkey_sel_enc, ele_adr_primary, p_bear->nw.dst);
				if(is_directed_friend_en(p_nw_retrans->netkey_sel_enc)){
					p_nw_retrans->dpt_ele_cnt = get_directed_friend_dependent_ele_cnt(mesh_key.sec_type_sel, p_bear->nw.src);					
					if(p_fwd_entry && !p_fwd_entry->fixed_path){
						mesh_df_path_monitoring(p_fwd_entry);						
						if(is_address_in_dependent_origin(p_fwd_entry, p_bear->nw.src)){
							p_nw_retrans->sec_type = DIRECTED;
						}
						else{
							p_nw_retrans->pending_op = CMD_CTL_DEPENDENT_NODE_UPDATE;
						}
					}
					else{
						//Create Path Origin State Machine
						p_nw_retrans->pending_op = CMD_CTL_PATH_REQUEST;
					}
				}
			}
			
		}
		else if(MESH_BEAR_GATT == p_nw_retrans->src_bear){
			if((MASTER == mesh_key.sec_type_sel) && is_proxy_use_directed(mesh_key.sec_type_sel)){	
				p_nw_retrans->dpt_ele_cnt = get_directed_proxy_dependent_ele_cnt(mesh_key.sec_type_sel, p_bear->nw.src);
				if(p_nw_retrans->dpt_ele_cnt){
					p_fwd_entry = get_forwarding_entry(p_nw_retrans->netkey_sel_enc, ele_adr_primary, p_bear->nw.dst);
					if(p_fwd_entry && !p_fwd_entry->fixed_path){
						mesh_df_path_monitoring(p_fwd_entry);					
						if(is_address_in_dependent_origin(p_fwd_entry, p_bear->nw.src)){
							p_nw_retrans->sec_type = DIRECTED; 
						}
						else{
							p_nw_retrans->pending_op = CMD_CTL_DEPENDENT_NODE_UPDATE; 					
						}
					}
					else{
						//Create Path Origin State Machine							
						p_nw_retrans->pending_op = CMD_CTL_PATH_REQUEST;			
					}
				}
			}
		}
	}
#endif

#if (MD_SBR_CFG_SERVER_EN)
	if(is_subnet_bridge_en()){
		int index = get_subnet_bridge_index(mesh_key.net_key[mesh_key.netkey_sel_dec][mesh_key.new_netkey_dec].index, p_bear->nw.src, p_bear->nw.dst);
		if(-1 != index){
			p_nw_retrans->netkey_sel_enc = get_mesh_net_key_offset((u16)index);
			p_nw_retrans->need_bridge = 1;	// in pts, relay is disable in initial condition, but need to bridge
			#if MD_DF_CFG_SERVER_EN
			p_fwd_entry = get_forwarding_entry(p_nw_retrans->netkey_sel_enc, ele_adr_primary, p_bear->nw.dst);
			if(p_fwd_entry && !p_fwd_entry->fixed_path){
				if(is_address_in_dependent_origin(p_fwd_entry, p_bear->nw.src)){
					p_nw_retrans->sec_type = DIRECTED;
				}
				else{
					p_nw_retrans->pending_op = CMD_CTL_DEPENDENT_NODE_UPDATE;
				}
			}
			else{
				p_nw_retrans->pending_op = CMD_CTL_PATH_REQUEST;
			}
			#endif
		}
	}
#endif

#if MD_DF_CFG_SERVER_EN
	if(!(!p_bear->nw.ctl && p_bear->lt_unseg.akf)){
		p_nw_retrans->pending_op = 0;
	}
#endif

	return 0;
}

#if (MD_DF_CFG_SERVER_EN)
u16 mesh_tx_network_layer_df_cb(mesh_cmd_bear_t *p_bear, mesh_match_type_t *p_match_type, u8 *p_sec_type)
{
	u16 path_target = 0;	
	if(is_directed_forwarding_en(p_match_type->mat.nk_array_idx) && !p_match_type->mat.immutable_flag){
		path_entry_com_t *p_entry = get_forwarding_entry(p_match_type->mat.nk_array_idx, p_bear->nw.src, p_bear->nw.dst);
		if(p_entry){
			mesh_df_path_monitoring(p_entry);			
			*p_sec_type = DIRECTED;
		}
		else{
			if((!p_bear->nw.ctl) && p_bear->lt_unseg.akf){
				path_target = p_bear->nw.dst;				
			}
		}
	}

	return path_target;
}

int mesh_network_df_initial_start(u8 op, u16 netkey_offset, u16 destination, u16 dpt_org, u16 dpt_ele_cnt)
{
	int ret = 0;
	if(CMD_CTL_DEPENDENT_NODE_UPDATE == op){ 	
		ret = directed_forwarding_dependents_update_start(netkey_offset, DEPENDENT_TYPE_ADD, ele_adr_primary, dpt_org, dpt_ele_cnt);
	}
	else if(CMD_CTL_PATH_REQUEST == op){ 		
		ret = directed_forwarding_initial_start(netkey_offset, destination, dpt_org, dpt_ele_cnt);
	}
	return ret;
}
#endif
#endif
#endif
//--------------------network layer callback end------------------------------//

#if PTS_TEST_EN
int path_monitoring_test_mode = 0;
#endif

#if MD_DF_CFG_SERVER_EN
non_fixed_fwd_tbl_t non_fixed_fwd_tbl[NET_KEY_MAX];
discovery_table_t discovery_table[NET_KEY_MAX];
#if 0
path_origin_state_t path_origin_state[NET_KEY_MAX];
#endif

#if DF_TEST_MODE_EN
void mesh_df_led_event(u8 nid)
{
	foreach(i,NET_KEY_MAX){
		u32 cnt = is_key_refresh_use_old_and_new_key(i) ? 2 : 1;
    	foreach(k,cnt){
			mesh_net_key_t *p_netkey = &mesh_key.net_key[i][0];
			if((p_netkey->valid)&&(nid == p_netkey->nid_d)){
				cfg_led_event(LED_EVENT_FLASH_2HZ_2S);
			}
    	}
	}
}
#endif

//--------------directed forwarding command interface---------------------------//
int is_directed_forwarding_en(u16 netkey_offset)
{
	return ((netkey_offset < NET_KEY_MAX) && (DIRECTED_FORWARDING_ENABLE == model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[netkey_offset].directed_control.directed_forwarding));
}

int is_directed_relay_en(u16 netkey_offset)
{
	return ((netkey_offset < NET_KEY_MAX) && (DIRECTED_RELAY_ENABLE == model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[netkey_offset].directed_control.directed_relay));
}

int is_directed_proxy_en(u16 netkey_offset)
{
	return ((netkey_offset < NET_KEY_MAX) && (DIRECTED_PROXY_ENABLE == model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[netkey_offset].directed_control.directed_proxy));
}

int is_directed_friend_en(u16 netkey_offset)
{
	return ((netkey_offset < NET_KEY_MAX) && (DIRECTED_FRIEND_ENABLE == model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[netkey_offset].directed_control.directed_friend));
}

int is_directed_forwarding_op(u16 op)
{
    if((op>=CMD_CTL_PATH_REQUEST) && (op<=CMD_CTL_PATH_REQUEST_SOLICITATION)){
		return 1;
	}
    return 0;
}

int is_proxy_use_directed(u16 netkey_offset)
{
	return ((PROXY_CLIENT == proxy_mag.proxy_client_type) || (DIRECTED_PROXY_CLIENT == proxy_mag.proxy_client_type)) && proxy_mag.directed_server[netkey_offset].use_directed;
}

void mesh_directed_forwarding_bind_state_update()
{
	for(int key_offset=0; key_offset<NET_KEY_MAX; key_offset++){
		directed_control_t *p_control = &model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[key_offset].directed_control;
		if(DIRECTED_FORWARDING_DISABLE == model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[key_offset].directed_control.directed_forwarding){	
			p_control->directed_relay = DIRECTED_RELAY_DISABLE;
			if(p_control->directed_proxy < DIRECTED_PROXY_NOT_SUPPORT){
				p_control->directed_proxy = DIRECTED_PROXY_DISABLE;
			}

			if(p_control->directed_friend < DIRECTED_FRIEND_NOT_SUPPORT){
				p_control->directed_friend = DIRECTED_FRIEND_DISABLE;
			}
		}

		if(GATT_PROXY_SUPPORT_DISABLE == model_sig_cfg_s.gatt_proxy ){
			if(p_control->directed_proxy < DIRECTED_PROXY_NOT_SUPPORT){
				p_control->directed_proxy = DIRECTED_PROXY_DISABLE;
			}
		}

		if((DIRECTED_PROXY_DISABLE == p_control->directed_proxy) || (DIRECTED_PROXY_NOT_SUPPORT == p_control->directed_proxy_directed_default)){		
			p_control->directed_proxy_directed_default = DIRECTED_PROXY_NOT_SUPPORT;
		}

		if((FRIEND_SUPPORT_DISABLE == model_sig_cfg_s.frid) && (p_control->directed_friend < DIRECTED_FRIEND_NOT_SUPPORT)){
			p_control->directed_friend = DIRECTED_FRIEND_DISABLE;
		}

		#if (!FEATURE_LOWPOWER_EN && !WIN32)
		if(DIRECTED_FRIEND_DISABLE == p_control->directed_friend){
			foreach(i, MAX_LPN_NUM){
				if(fn_other_par[i].LPNAdr){
					directed_forwarding_dependents_update_start(key_offset, DEPENDENT_TYPE_REMOVE, ele_adr_primary, fn_other_par[i].LPNAdr, get_directed_friend_dependent_ele_cnt(key_offset, fn_other_par[i].LPNAdr));	
				}
			}
		}

		if(DIRECTED_PROXY_DISABLE == p_control->directed_proxy){
			directed_proxy_dependent_node_delete();
		}
		#endif
	}

	return;
}

u8 get_directed_proxy_dependent_ele_cnt(u16 netkey_offset, u16 addr)
{
	u8 ele_cnt=0;
#if MD_SERVER_EN
	if(DIRECTED_PROXY_CLIENT == proxy_mag.proxy_client_type){
		if(is_ele_in_node(addr, proxy_mag.directed_server[netkey_offset].client_addr, proxy_mag.directed_server[netkey_offset].client_2nd_ele_cnt+1)){
			ele_cnt = proxy_mag.directed_server[netkey_offset].client_2nd_ele_cnt+1;
		}
	}
	else if(PROXY_CLIENT == proxy_mag.proxy_client_type){
		if(is_proxy_client_addr(addr)){
			ele_cnt = 1;
		}
	}
#endif
	return ele_cnt;
}

u8 get_directed_friend_dependent_ele_cnt(u16 netkey_offset, u16 addr)
{
	u8 ele_cnt=0;
#if (MD_SERVER_EN && !FEATURE_LOWPOWER_EN)
	u16 match_F2L = mesh_mac_match_friend(addr);
	foreach(i,g_max_lpn_num){
		if(match_F2L & BIT(i)){
			ele_cnt = fn_req[i].NumEle;
			break;
		}
	}
#endif
	return ele_cnt;
}

int is_address_in_dependent_origin(path_entry_com_t *p_fwd_entry, u16 addr)
{	
	foreach(i, MAX_DEPENDENT_NUM){
		if(is_ele_in_node(addr, p_fwd_entry->dependent_origin[i].addr, p_fwd_entry->dependent_origin[i].snd_ele_cnt+1)){
			return 1;
		}
	}
	
	return 0;
}

int is_address_in_dependent_target(path_entry_com_t *p_fwd_entry, u16 addr)
{	
	foreach(i, MAX_DEPENDENT_NUM){
		if(is_ele_in_node(addr, p_fwd_entry->dependent_target[i].addr, p_fwd_entry->dependent_target[i].snd_ele_cnt+1)){
			return 1;
		}
	}
	
	return 0;
}

int is_addr_in_entry(u16 src_addr, u16 destination, path_entry_com_t *p_fwd_entry)
{
	if((is_ele_in_node(src_addr, p_fwd_entry->path_origin, p_fwd_entry->path_origin_snd_ele_cnt+1) || is_address_in_dependent_origin(p_fwd_entry, src_addr)) && 
		(is_ele_in_node(destination, p_fwd_entry->destination, p_fwd_entry->path_target_snd_ele_cnt+1) || is_address_in_dependent_target(p_fwd_entry, destination))
	){
		return 1;
	}
	else if((p_fwd_entry->backward_path_validated == 1) && (is_ele_in_node(destination, p_fwd_entry->path_origin, p_fwd_entry->path_origin_snd_ele_cnt+1) || is_address_in_dependent_origin(p_fwd_entry, destination)) &&
		(is_ele_in_node(src_addr, p_fwd_entry->destination, p_fwd_entry->path_target_snd_ele_cnt+1) || is_address_in_dependent_target(p_fwd_entry, src_addr))
	){
		return 1;
	}
	else if(ADR_ALL_DIRECTED_FORWARD == destination){
		return 1;
	}
	return 0;
}

path_entry_com_t * get_fixed_path_entry(u16 netkey_offset, u16 src_address, u16 destination)
{
	if(netkey_offset < NET_KEY_MAX){
		foreach(i, MAX_FIXED_PATH){
			path_entry_com_t *p_fwd_entry = &model_sig_g_df_sbr_cfg.df_cfg.fixed_fwd_tbl[netkey_offset].path[i];
			if(is_addr_in_entry(src_address, destination, p_fwd_entry)){
				if(p_fwd_entry->path_not_ready){
					return 0;
				}
				else{
					return p_fwd_entry;
				}
			}
		}
	}

	return 0;
}

non_fixed_entry_t * get_non_fixed_path_entry(u16 netkey_offset, u16 src_address, u16 destination)
{
	if(netkey_offset < NET_KEY_MAX){
		foreach(i, MAX_NON_FIXED_PATH){
			non_fixed_entry_t *p_fwd_entry = &non_fixed_fwd_tbl[netkey_offset].path[i];
			if(is_addr_in_entry(src_address, destination, &p_fwd_entry->entry)){
				return p_fwd_entry;
			}
		}
	}

	return 0;
}

path_entry_com_t *get_forwarding_entry(u16 netkey_offset, u16 src, u16 dst)
{
	path_entry_com_t *p_entry=get_fixed_path_entry(netkey_offset, src, dst);
	if(p_entry){
		return p_entry;
	}
	
	non_fixed_entry_t *p_non_fixed_entry = get_non_fixed_path_entry(netkey_offset, src, dst);
	if(p_non_fixed_entry){
		p_entry = &p_non_fixed_entry->entry;
	}
	
	return p_entry;	
}

int mesh_df_path_monitoring(path_entry_com_t *p_entry){
	if((0 == p_entry->fixed_path) && is_own_ele(p_entry->path_origin)){
		non_fixed_entry_t *p_non_fixed_entry = GET_NON_FIXED_ENTRY_POINT((u8*) p_entry);
		if(p_non_fixed_entry->state.path_monitoring){
			p_non_fixed_entry->state.path_need = 1;
			return 1;
		}
	}
	
	return 0;
}
#endif

//--------------directed forwarding command interface end---------------------------//
#if (MD_DF_CFG_SERVER_EN&&!WIN32)
#define LOG_DF_DEBUG(pbuf,len,format,...)		LOG_MSG_LIB(TL_LOG_DIRECTED_FORWARDING,pbuf,len,format,__VA_ARGS__)

void mesh_directed_forwarding_default_val_init()
{
	mesh_directed_forward_t *p_df = &model_sig_g_df_sbr_cfg.df_cfg.directed_forward;
	foreach(i, NET_KEY_MAX){
		p_df->subnet_state[i].directed_control.directed_forwarding = DF_TEST_MODE_EN ? DIRECTED_FORWARDING_ENABLE : DIRECTED_FORWARDING_DISABLE;//DIRECTED_FORWARDING_ENABLE; 	
		p_df->subnet_state[i].directed_control.directed_relay = DF_TEST_MODE_EN ? DIRECTED_RELAY_ENABLE : DIRECTED_RELAY_DISABLE;//DIRECTED_RELAY_ENABLE; 
		p_df->subnet_state[i].directed_control.directed_proxy = DIRECTED_PROXY_EN ? (DF_TEST_MODE_EN ? DIRECTED_PROXY_ENABLE : DIRECTED_PROXY_DISABLE) : DIRECTED_PROXY_NOT_SUPPORT;
		p_df->subnet_state[i].directed_control.directed_proxy_directed_default = DIRECTED_PROXY_EN ? (DF_TEST_MODE_EN ? DIRECTED_PROXY_DEFAULT_ENABLE : DIRECTED_PROXY_DEFAULT_DISABLE) : DIRECTED_PROXY_NOT_SUPPORT;
		p_df->subnet_state[i].directed_control.directed_friend = DIRECTED_FRIEND_EN ? DIRECTED_FRIEND_DISABLE : DIRECTED_FRIEND_NOT_SUPPORT;
		
		p_df->subnet_state[i].path_metric.metric_type = METRIC_TYPE_NODE_COUNT;
		p_df->subnet_state[i].path_metric.path_lifetime = PATH_LIFETIME_24HOURS;
		p_df->subnet_state[i].max_concurrent_init = PTS_TEST_EN ? 2 : MAX_CONCURRENT_CNT;	
		p_df->subnet_state[i].wanted_lanes = DF_TEST_MODE_EN ? 1 : 2; 	// the number of lanes to be discovered for each path in a subnet.
		p_df->subnet_state[i].two_way_path = 1;
		p_df->subnet_state[i].unicast_echo_interval = 0; // 0:not executed.  validation interval = path lifetime * Path Echo Interval / 100
		p_df->subnet_state[i].multicast_echo_interval = 0;
	}
		
	p_df->transmit.count = PTS_TEST_EN ? 1 : TRANSMIT_CNT_DEF; 
	p_df->transmit.invl_steps = PTS_TEST_EN ? 9 : TRANSMIT_INVL_STEPS_DEF; 
	p_df->relay_transmit.count = PTS_TEST_EN ? 2 : TRANSMIT_CNT_DEF; 
	p_df->relay_transmit.invl_steps = PTS_TEST_EN ? 9 : TRANSMIT_INVL_STEPS_DEF;
	p_df->control_transmit.count = PTS_TEST_EN ? 1 : TRANSMIT_CNT_DEF;
	p_df->control_transmit.invl_steps = PTS_TEST_EN ? 9 : TRANSMIT_INVL_STEPS_DEF;
	p_df->control_relay_transmit.count = PTS_TEST_EN ? 2 : TRANSMIT_CNT_DEF;
	p_df->control_relay_transmit.invl_steps = PTS_TEST_EN ? 9 : TRANSMIT_INVL_STEPS_DEF;
		
	p_df->rssi_threshold.default_rssi_threshold = -86; // should be 10 dB above the receiver sensitivity, user can set bigger in test mode.
	p_df->rssi_threshold.rssi_margin = 0x0e; // default 0x14 in spec

	p_df->directed_paths.node_paths = 20;
	p_df->directed_paths.relay_paths = 20;
	p_df->directed_paths.proxy_paths = 20;
	p_df->directed_paths.friend_paths = 20;

	p_df->discovery_timing.path_monitoring_interval = DF_TEST_MODE_EN ? 30 : 120; // unit:second
	p_df->discovery_timing.path_discovery_retry_interval = 300; 
	p_df->discovery_timing.path_discovery_interval = DF_TEST_MODE_EN ? 0 : 1; // 0:5s 1:30s
	p_df->discovery_timing.lane_discovery_guard_interval = DF_TEST_MODE_EN ? 0 : 1;  // 0:2s 1:10s

	mesh_directed_forwarding_bind_state_update();
	#if PTS_TEST_EN // init state for PTS
	memset(non_fixed_fwd_tbl, 0x00, sizeof(non_fixed_fwd_tbl));
	memset(discovery_table, 0x00, sizeof(discovery_table));
	memset(model_sig_g_df_sbr_cfg.df_cfg.fixed_fwd_tbl, 0x00, sizeof(model_sig_g_df_sbr_cfg.df_cfg.fixed_fwd_tbl));
	#if MD_SBR_CFG_SERVER_EN
	memset(model_sig_g_df_sbr_cfg.bridge_cfg.bridge_entry, 0x00, sizeof(model_sig_g_df_sbr_cfg.bridge_cfg.bridge_entry));
	#endif
	#endif
	return;
}

int mesh_directed_proxy_capa_report(int netkey_offset)
{
	int ret = 0;
	if(!is_provision_success() || (model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[netkey_offset].directed_control.directed_proxy>DIRECTED_PROXY_ENABLE)){
		return ret;
	}
	directed_proxy_capa_sts proxy_capa;	
	mesh_net_key_t *p_netkey_base = &mesh_key.net_key[netkey_offset][0];
	if(!p_netkey_base->valid){
		return ret;
	}
	proxy_capa.directed_proxy = model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[netkey_offset].directed_control.directed_proxy;
	proxy_capa.use_directed = proxy_mag.directed_server[netkey_offset].use_directed;
	return mesh_tx_cmd_layer_cfg_primary_specified_key(DIRECTED_PROXY_CAPA_STATUS,(u8 *)(&proxy_capa),sizeof(proxy_capa),PROXY_CONFIG_FILTER_DST_ADR, mesh_key.net_key[netkey_offset][0].index);
}

int mesh_directed_proxy_capa_report_upon_connection()
{
	int ret = -1;
	proxy_mag.proxy_client_type = UNSET_CLIENT;
	for(int i=0; i<NET_KEY_MAX; i++){
		proxy_mag.directed_server[i].use_directed = (DIRECTED_PROXY_DEFAULT_ENABLE == model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[i].directed_control.directed_proxy_directed_default);
		proxy_mag.directed_server[i].client_addr = ADR_UNASSIGNED;
		proxy_mag.directed_server[i].client_2nd_ele_cnt = 0;			
		ret = mesh_directed_proxy_capa_report(i);
	}

	return ret;
}			

#if !FEATURE_LOWPOWER_EN
void directed_proxy_dependent_node_delete()
{
	foreach(netkey_offset, NET_KEY_MAX){
		if(DIRECTED_FORWARDING_DISABLE == model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[netkey_offset].directed_control.directed_forwarding){
			continue;
		}
		
		if(DIRECTED_PROXY_CLIENT == proxy_mag.proxy_client_type){			
			if(proxy_mag.directed_server[netkey_offset].client_addr){
				directed_forwarding_dependents_update_start(netkey_offset, DEPENDENT_TYPE_REMOVE, ele_adr_primary,  proxy_mag.directed_server[netkey_offset].client_addr, proxy_mag.directed_server[netkey_offset].client_2nd_ele_cnt+1);
			}
		}
		else if((PROXY_CLIENT == proxy_mag.proxy_client_type) && (FILTER_WHITE_LIST == proxy_mag.filter_type)){
			foreach(idx, MAX_LIST_LEN){
				if(proxy_mag.addr_list[idx] && is_unicast_adr(proxy_mag.addr_list[idx])){					
					directed_forwarding_dependents_update_start(netkey_offset, DEPENDENT_TYPE_REMOVE, ele_adr_primary, proxy_mag.addr_list[idx], 1);
				}
			}
		}	
	}
}

//----------------------------discovery table process---------------------//
discovery_entry_par_t * get_discovery_entry_correspond2_path_request(u16 netkey_offset, u16 path_origin, u8 forwarding_number)
{
	if(netkey_offset < NET_KEY_MAX){
		foreach(i, MAX_DSC_TBL){
			discovery_entry_t * p_dsc_entry = &discovery_table[netkey_offset].dsc_entry_par[i].entry;		
			if((path_origin == p_dsc_entry->path_origin.addr) && (forwarding_number == p_dsc_entry->forwarding_number)){
				return &discovery_table[netkey_offset].dsc_entry_par[i];
			}
		}
	}
	return 0;
}

discovery_entry_par_t * get_discovery_entry_correspond2_path_destination(u16 netkey_offset, u16 path_dst)
{
	if(netkey_offset < NET_KEY_MAX){
		foreach(i, MAX_DSC_TBL){
			discovery_entry_par_t * p_dsc_entry = &discovery_table[netkey_offset].dsc_entry_par[i];
			if(path_dst == p_dsc_entry->entry.destination){
				return p_dsc_entry;
			}
		}
	}
	return 0;
}

void del_discovery_entry_correspond2_path_destination(u16 netkey_offset, u16 path_origin, u16 path_dst)
{
	if(netkey_offset < NET_KEY_MAX){
		foreach(i, MAX_DSC_TBL){
			discovery_entry_t * p_dsc_entry = &discovery_table[netkey_offset].dsc_entry_par[i].entry;		
			if((path_origin == p_dsc_entry->path_origin.addr) && (path_dst == p_dsc_entry->destination)){
				memset(&discovery_table[netkey_offset].dsc_entry_par[i], 0x00, sizeof(discovery_table[netkey_offset].dsc_entry_par[i])) ;
			}
		}
	}
}

discovery_entry_par_t * get_discovery_entry_correspond2_forwarding_entry(u16 netkey_offset, non_fixed_entry_t *p_fwd_entry)
{
	return get_discovery_entry_correspond2_path_request(netkey_offset, p_fwd_entry->entry.path_origin, p_fwd_entry->state.forwarding_number);
}

int update_discovery_entry_by_path_request(discovery_entry_t *p_dsc_entry, u16 network_src, u8 src_type, mesh_ctl_path_req_t *p_path_request)
{
	if(p_path_request->origin_path_metric < p_dsc_entry->path_metric){
		p_dsc_entry->path_metric = p_path_request->origin_path_metric;
		p_dsc_entry->next_toward_path_origin = network_src;
		p_dsc_entry->bearer_toward_path_origin = src_type;
		LOG_DF_DEBUG(0, 0, " update_discovery_entry_by_path_request", 0);
		return 1;
	}
	return 0;
}

discovery_entry_par_t * get_discovery_entry_correspond2_path_reply(u16 netkey_offset, mesh_ctl_path_reply_t *p_path_reply)
{
	if(netkey_offset < NET_KEY_MAX){
		foreach(i, MAX_DSC_TBL){
			discovery_entry_t * p_dsc_entry = &discovery_table[netkey_offset].dsc_entry_par[i].entry;	
			addr_range_t *p_range_addr = (addr_range_t *)p_path_reply->addr_par;
			if((p_path_reply->path_origin == p_dsc_entry->path_origin.addr) && (p_path_reply->forwarding_number == p_dsc_entry->forwarding_number) &&
				(is_group_adr(p_dsc_entry->destination) || is_virtual_adr(p_dsc_entry->destination) || is_ele_in_node(p_dsc_entry->destination, p_range_addr->range_start_b, p_range_addr->range_length))
			){
				return &discovery_table[netkey_offset].dsc_entry_par[i];
			}
		}
	}
	return 0;
}

discovery_entry_par_t * add_discovery_table_entry(u16 netkey_offset, addr_range_t *p_path_origin, u16 destination, addr_range_t *p_dependent_origin, u16 next_toward_path_origin, u8 bearer_toward_path_origin, mesh_ctl_path_req_t *p_path_req)
{
	// if recevive path request, discard path_origin, destination, p_dependent
	discovery_entry_t dsc_tbl_entry;
	memset(&dsc_tbl_entry, 0x00, sizeof(dsc_tbl_entry));
	dsc_tbl_entry.path_metric_type = p_path_req?p_path_req->path_metric_type:model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[netkey_offset].path_metric.metric_type;
	dsc_tbl_entry.path_lifetime = p_path_req?p_path_req->path_lifetime:model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[netkey_offset].path_metric.path_lifetime; 
	dsc_tbl_entry.path_discovery_interval = p_path_req?p_path_req->discovery_interval:model_sig_g_df_sbr_cfg.df_cfg.directed_forward.discovery_timing.path_discovery_interval;
	dsc_tbl_entry.forwarding_number = p_path_req?p_path_req->forwarding_number:(discovery_table[netkey_offset].forwarding_number);
	dsc_tbl_entry.path_metric = p_path_req?p_path_req->origin_path_metric:0;
	dsc_tbl_entry.destination = p_path_req?p_path_req->destination:destination;
	
	dsc_tbl_entry.next_toward_path_origin = next_toward_path_origin;
	dsc_tbl_entry.bearer_toward_path_origin = bearer_toward_path_origin;
	addr_range_t *p_range_addr = p_path_req?(addr_range_t *)p_path_req->addr_par:p_path_origin;
	dsc_tbl_entry.path_origin.addr = p_range_addr->range_start_b;
	u8 via_len = 0;
	if(p_range_addr->length_present_b){ // path origin
		dsc_tbl_entry.path_origin.snd_ele_cnt = p_range_addr->range_length-1;
		via_len += 3;
	}
	else{
		dsc_tbl_entry.path_origin.snd_ele_cnt = 0;	
		via_len += 2;
	}

	p_range_addr = p_path_req?(addr_range_t *)(p_path_req->addr_par+via_len):p_dependent_origin;
	if(p_dependent_origin || (p_path_req&&p_path_req->on_behalf_of_dependent_origin)){// path dependent
		dsc_tbl_entry.on_behalf_of_dependent_origin = 1;
		dsc_tbl_entry.dependent_origin.addr = p_range_addr->range_start_b;
		if(p_range_addr->length_present_b){
			dsc_tbl_entry.dependent_origin.snd_ele_cnt = p_range_addr->range_length-1;
		}
		else{
			dsc_tbl_entry.dependent_origin.snd_ele_cnt = 0;
		}
	}
	else{
		dsc_tbl_entry.on_behalf_of_dependent_origin = 0;
	}
	
	foreach(i, MAX_DSC_TBL){
		if(0 == discovery_table[netkey_offset].dsc_entry_par[i].entry.path_origin.addr){
			LOG_DF_DEBUG(0, 0, "add discovery_table origin:0x%x target:0x%x", dsc_tbl_entry.path_origin.addr, dsc_tbl_entry.destination);
			discovery_table[netkey_offset].dsc_entry_par[i].entry = dsc_tbl_entry;					
			return &discovery_table[netkey_offset].dsc_entry_par[i];
		}
	}
	return 0;
}

void start_path_discovery_timer(discovery_entry_par_t *p_dsc_entry)
{
	LOG_DF_DEBUG(0, 0, "start_path_discovery_timer origin:%x target:%x", p_dsc_entry->entry.path_origin.addr, p_dsc_entry->entry.destination);
	p_dsc_entry->state.discovery_timer = (clock_time()|1) + GET_PATH_DSC_INTERVAL_MS(p_dsc_entry->entry.path_discovery_interval)*1000*sys_tick_per_us;
	p_dsc_entry->state.first_reply_msg = 1;
	p_dsc_entry->state.path_confirm_sent = 0;
	p_dsc_entry->state.new_lane_established = 0;
}

void stop_path_discovery_timer(discovery_entry_par_t *p_dsc_entry)
{
	LOG_DF_DEBUG(0, 0, "stop_path_discovery_timer origin:%x target:%x", p_dsc_entry->entry.path_origin.addr, p_dsc_entry->entry.destination);
	p_dsc_entry->state.discovery_timer = 0;	
}

void start_path_discovery_guard_timer(discovery_entry_par_t *p_dsc_entry)
{
	LOG_DF_DEBUG(0, 0, "start_path_discovery_guard_timer origin:%x target:%x", p_dsc_entry->entry.path_origin.addr, p_dsc_entry->entry.destination);
	discovery_timing_t *p_dsc_timing = &model_sig_g_df_sbr_cfg.df_cfg.directed_forward.discovery_timing;
	p_dsc_entry->state.discovery_guard_timer = (clock_time()|1) + GET_LANE_GUARD_INTERVAL_MS(p_dsc_timing->lane_discovery_guard_interval)*1000*sys_tick_per_us;
}

void stop_path_discovery_guard_timer(discovery_entry_par_t *p_dsc_entry)
{
	LOG_DF_DEBUG(0, 0, "start_path_discovery_guard_timer origin:%x target:%x", p_dsc_entry->entry.path_origin.addr, p_dsc_entry->entry.destination);
	p_dsc_entry->state.discovery_guard_timer = 0;
}

void start_path_discovery_retry_timer(discovery_entry_par_t *p_dsc_entry)
{
	discovery_timing_t *p_dsc_timing = &model_sig_g_df_sbr_cfg.df_cfg.directed_forward.discovery_timing;
	u16 retry_interval_ms = p_dsc_timing->path_discovery_retry_interval*1000;
	if(is_own_ele(p_dsc_entry->entry.path_origin.addr)){
		retry_interval_ms = max2(retry_interval_ms, GET_LANE_GUARD_INTERVAL_MS(p_dsc_timing->lane_discovery_guard_interval));// Path retry interval should >= guard interval, PTS wait 33s while discovery interval is 30s and retry interval is 0
	}
	
	LOG_DF_DEBUG(0, 0, "start_path_discovery_retry_timer invl:%dms origin:%x target:%x", retry_interval_ms, p_dsc_entry->entry.path_origin.addr, p_dsc_entry->entry.destination);
	p_dsc_entry->state.discovery_retry_timer = (clock_time_ms()|1) + retry_interval_ms;
}

void stop_path_discovery_retry_timer(discovery_entry_par_t *p_dsc_entry)
{
	LOG_DF_DEBUG(0, 0, "stop_path_discovery_retry_timer origin:%x target:%x", p_dsc_entry->entry.path_origin.addr, p_dsc_entry->entry.destination);
	p_dsc_entry->state.discovery_retry_timer = 0;
}


void start_path_reply_delay_timer(discovery_entry_par_t *p_dsc_entry)
{
	LOG_DF_DEBUG(0, 0, "start_path_reply_delay_timer origin:%x target:%x", p_dsc_entry->entry.path_origin.addr, p_dsc_entry->entry.destination);
	p_dsc_entry->state.reply_delay_timer = (clock_time()|1) + (is_unicast_adr(p_dsc_entry->entry.destination)?PATH_REPLY_DELAY_MS:(PATH_REPLY_DELAY_MS+(rand()%500)))*1000*sys_tick_per_us;
}

void stop_path_reply_delay_timer(discovery_entry_par_t *p_dsc_entry)
{
	LOG_DF_DEBUG(0, 0, "%s origin:%x target:%x", __func__, p_dsc_entry->entry.path_origin.addr, p_dsc_entry->entry.destination);
	p_dsc_entry->state.reply_delay_timer = 0;
}

void start_path_request_delay_timer(discovery_entry_par_t *p_dsc_entry)
{
	LOG_DF_DEBUG(0, 0, "start_path_request_delay_timer origin:%x target:%x", p_dsc_entry->entry.path_origin.addr, p_dsc_entry->entry.destination);
	p_dsc_entry->state.request_delay_timer = (clock_time()|1) + (PATH_REQUEST_DELAY_MS+(rand()%30))*1000*sys_tick_per_us;
}

void stop_path_request_delay_timer(discovery_entry_par_t *p_dsc_entry)
{
	LOG_DF_DEBUG(0, 0, "stop_path_request_delay_timer origin:%x target:%x", p_dsc_entry->entry.path_origin.addr, p_dsc_entry->entry.destination);
	p_dsc_entry->state.request_delay_timer = 0;
}

void directed_discovery_entry_remove(discovery_entry_par_t *p_dsc_entry)
{
	LOG_DF_DEBUG(0, 0, "directed_discovery_entry_remove origin:%x target:%x", p_dsc_entry->entry.path_origin.addr, p_dsc_entry->entry.destination);
	memset(p_dsc_entry, 0x00, sizeof(discovery_entry_par_t));
}

int direced_forwarding_concurrent_count_check(u16 netkey_offset)
{
	int num = 0;
	foreach_arr(i, discovery_table[netkey_offset].dsc_entry_par){
		if(discovery_table[netkey_offset].dsc_entry_par[i].entry.path_origin.addr){
			num++;
		}
	}

	if(num < model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[netkey_offset].max_concurrent_init){
		return 1;
	}

	return 0;
}
//---------------------------discovery table process end-------------------------//

//---------------------------forwarding table process-------------------------//
void start_path_lifetime_timer(non_fixed_entry_t *p_fwd_entry, discovery_entry_t *p_dsc_entry)
{
	u32 path_lifetime_ms = GET_PATH_LIFETIME_MS(p_dsc_entry->path_lifetime);		
	int path_use_timer_ms = max2(GET_PATH_DSC_INTERVAL_MS(p_dsc_entry->path_discovery_interval), path_lifetime_ms-GET_PATH_DSC_INTERVAL_MS(p_dsc_entry->path_discovery_interval)-model_sig_g_df_sbr_cfg.df_cfg.directed_forward.discovery_timing.path_monitoring_interval*1000);
	#if PTS_TEST_EN
	if(path_monitoring_test_mode){
		path_use_timer_ms = GET_PATH_DSC_INTERVAL_MS(p_dsc_entry->path_discovery_interval);
	}
	#endif

	p_fwd_entry->state.lifetime_ms = (clock_time_ms()|1) + (is_own_ele(p_fwd_entry->entry.path_origin)?path_use_timer_ms:path_lifetime_ms);
	LOG_DF_DEBUG(0, 0, "%s origin:%x dst:%x lifetime_ms:%d", __func__, p_fwd_entry->entry.path_origin, p_fwd_entry->entry.destination, is_own_ele(p_fwd_entry->entry.path_origin)?path_use_timer_ms:path_lifetime_ms);
}

void stop_path_lifetime_timer(non_fixed_entry_t *p_fwd_entry)
{
	LOG_DF_DEBUG(0, 0, "%s origin:%x dst:%x", __func__, p_fwd_entry->entry.path_origin, p_fwd_entry->entry.destination);
	p_fwd_entry->state.lifetime_ms = 0;
}

void start_path_echo_timer(u16 netkey_offset, non_fixed_entry_t *p_fwd_entry)
{
	u32 validation_interval = GET_PATH_LIFETIME_MS(model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[netkey_offset].path_metric.path_lifetime) * (is_unicast_adr(p_fwd_entry->entry.destination)?model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[netkey_offset].unicast_echo_interval:model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[netkey_offset].multicast_echo_interval) / 100;			
	p_fwd_entry->state.path_echo_timer_ms = (clock_time_ms()|1) + validation_interval;
	LOG_DF_DEBUG(0, 0, "%s origin:%x dst:%x interval_ms:%d", __func__, p_fwd_entry->entry.path_origin, p_fwd_entry->entry.destination, validation_interval);
}

void stop_path_echo_timer(non_fixed_entry_t *p_fwd_entry)
{
	LOG_DF_DEBUG(0, 0, "%s origin:%x dst:%x", __func__, p_fwd_entry->entry.path_origin, p_fwd_entry->entry.destination);
	p_fwd_entry->state.path_echo_timer_ms = 0;
}

void start_path_echo_reply_timeout_timer(non_fixed_entry_t *p_fwd_entry)
{
	LOG_DF_DEBUG(0, 0, "%s origin:%x dst:%x", __func__, p_fwd_entry->entry.path_origin, p_fwd_entry->entry.destination);
	if(0 == p_fwd_entry->state.path_echo_reply_timeout_timer){// in active, don't refresh
		p_fwd_entry->state.path_echo_reply_timeout_timer = (clock_time_ms()|1) + GET_PATH_DSC_INTERVAL_MS(model_sig_g_df_sbr_cfg.df_cfg.directed_forward.discovery_timing.path_discovery_interval);
	}
	stop_path_echo_timer(p_fwd_entry);
}

void stop_path_echo_reply_timeout_timer(non_fixed_entry_t *p_fwd_entry)
{
	LOG_DF_DEBUG(0, 0, "%s origin:%x dst:%x", __func__, p_fwd_entry->entry.path_origin, p_fwd_entry->entry.destination);
	p_fwd_entry->state.path_echo_reply_timeout_timer = 0;
	stop_path_echo_timer(p_fwd_entry);
}

void start_path_monitor(non_fixed_entry_t *p_fwd_entry)
{
	if(model_sig_g_df_sbr_cfg.df_cfg.directed_forward.discovery_timing.path_monitoring_interval){
		LOG_DF_DEBUG(0, 0, "%s origin:%x dst:%x", __func__, p_fwd_entry->entry.path_origin, p_fwd_entry->entry.destination);
		p_fwd_entry->state.path_monitoring = 1;
	}
}

void stop_path_monitor(non_fixed_entry_t *p_fwd_entry)
{
	LOG_DF_DEBUG(0, 0, "%s origin:%x dst:%x", __func__, p_fwd_entry->entry.path_origin, p_fwd_entry->entry.destination);
	p_fwd_entry->state.path_monitoring = 0;
}

int get_offset_in_dependent_list(path_addr_t *p_dependent_list, u16 addr)
{
	foreach(i, MAX_DEPENDENT_NUM){
		if(addr == p_dependent_list[i].addr){
			return i;
		}
	}
	return -1;
}

non_fixed_entry_t * get_forwarding_entry_correspond2_path_request(u16 netkey_offset, mesh_ctl_path_req_t *p_path_req)
{
	if(netkey_offset < NET_KEY_MAX){
		foreach(i, MAX_NON_FIXED_PATH){
			non_fixed_entry_t *p_fwd_entry = &non_fixed_fwd_tbl[netkey_offset].path[i];
			addr_range_t *p_path_origin = (addr_range_t *)p_path_req->addr_par;
			if((0 == p_fwd_entry->entry.fixed_path) && (p_path_origin->range_start_b == p_fwd_entry->entry.path_origin) && 
				(is_ele_in_node(p_path_req->destination, p_fwd_entry->entry.destination, p_fwd_entry->entry.path_target_snd_ele_cnt+1) || is_address_in_dependent_target(&p_fwd_entry->entry, p_path_req->destination))
			){
				return p_fwd_entry;
			}
		}
	}
	return 0;
}

non_fixed_entry_t * get_forwarding_entry_correspond2_discovery_entry(u16 netkey_offset, discovery_entry_t *p_dsc_entry)
{
	if(netkey_offset < NET_KEY_MAX){
		foreach(i, MAX_NON_FIXED_PATH){
			non_fixed_entry_t *p_fwd_entry = &non_fixed_fwd_tbl[netkey_offset].path[i];
			if((0 == p_fwd_entry->entry.fixed_path) && (p_dsc_entry->path_origin.addr == p_fwd_entry->entry.path_origin) && (p_dsc_entry->forwarding_number == p_fwd_entry->state.forwarding_number)){
				return p_fwd_entry;
			}
		}
	}
	return 0;
}

int delete_fixed_path(u16 netkey_offset, path_entry_com_t *p_entry)
{
	int err = -1;
	if(netkey_offset < NET_KEY_MAX){
		foreach(i, MAX_FIXED_PATH){
			path_entry_com_t *p_fwd_entry = &model_sig_g_df_sbr_cfg.df_cfg.fixed_fwd_tbl[netkey_offset].path[i];
			if((p_entry->path_origin == p_fwd_entry->path_origin) &&
				(p_entry->destination == p_fwd_entry->destination)){
				 memset(p_fwd_entry, 0x00, sizeof(path_entry_com_t));
				 err = 0;
			}	
		}
	}
	if(0 == err){
		model_sig_g_df_sbr_cfg.df_cfg.fixed_fwd_tbl[netkey_offset].update_id++;
		mesh_common_store(FLASH_ADR_MD_DF_SBR);
	}
	return err;
}

int delete_non_fixed_path(u16 netkey_offset, path_entry_com_t *p_entry)
{
	int err = -1;
	if(netkey_offset < NET_KEY_MAX){
		foreach(i, MAX_NON_FIXED_PATH){
			non_fixed_entry_t *p_fwd_entry = &non_fixed_fwd_tbl[netkey_offset].path[i];
			if((p_entry->path_origin == p_fwd_entry->entry.path_origin) &&
				(p_entry->destination == p_fwd_entry->entry.destination)){
				 memset(p_fwd_entry, 0x00, sizeof(non_fixed_entry_t));
				 err = 0;
				 LOG_DF_DEBUG(0, 0, "%s origin:0x%x target:0x%x",__func__, p_entry->path_origin, p_entry->destination);
				 break;
			}	
		}
	}

	return err;
}

path_entry_com_t * add_new_path_in_fixed_fwd_tbl(u16 netkey_offset, path_entry_com_t *p)
{
	path_entry_com_t *p_entry = 0;
	u16 path_index = 0;
	for(path_index=0; path_index<MAX_FIXED_PATH; path_index++){
		if(0 == model_sig_g_df_sbr_cfg.df_cfg.fixed_fwd_tbl[netkey_offset].path[path_index].path_origin){
			break;
		}
	}
	
	if(path_index < MAX_FIXED_PATH){
		p_entry = &model_sig_g_df_sbr_cfg.df_cfg.fixed_fwd_tbl[netkey_offset].path[path_index];
		model_sig_g_df_sbr_cfg.df_cfg.fixed_fwd_tbl[netkey_offset].update_id++;		
		memcpy(p_entry, p, sizeof(path_entry_com_t));	
		mesh_common_store(FLASH_ADR_MD_DF_SBR);
		LOG_DF_DEBUG(0, 0, "%s origin:0x%x target:0x%x",__func__, p_entry->path_origin, p_entry->destination);
	}
	return p_entry;
}

non_fixed_entry_t * add_new_path_in_non_fixed_fwd_tbl(u16 netkey_offset, non_fixed_entry_t *p)
{
	non_fixed_entry_t *p_entry = 0;
	u16 path_index = 0;
	for(path_index=0; path_index<MAX_NON_FIXED_PATH; path_index++){
		if(0 == non_fixed_fwd_tbl[netkey_offset].path[path_index].entry.path_origin){
			break;
		}
	}
	
	if(path_index < MAX_NON_FIXED_PATH){	
		p_entry = &non_fixed_fwd_tbl[netkey_offset].path[path_index];
		memcpy(p_entry, p, sizeof(non_fixed_entry_t));	
		model_sig_g_df_sbr_cfg.df_cfg.fixed_fwd_tbl[netkey_offset].update_id++;
	}
	return p_entry;
}

int forwarding_tbl_dependent_add(u16 range_start, u8 snd_ele_cnt, path_addr_t *p_dependent_list)
{
	int err = -1;	
	foreach(i,MAX_DEPENDENT_NUM){			
		if((0 == p_dependent_list[i].addr) || (p_dependent_list[i].addr == range_start)){ // add
			err = 0;
			p_dependent_list[i].addr = range_start;
			p_dependent_list[i].snd_ele_cnt = snd_ele_cnt;
			break;
		}	
	}

	return err;
}

int forwarding_tbl_dependent_delete(u16 addr, path_addr_t *p_dependent_list)
{
	int err = -1;
	foreach(i,MAX_DEPENDENT_NUM){	
		if(p_dependent_list[i].addr == addr){// exist
			memset(&p_dependent_list[i], 0x00, sizeof(path_addr_t));
			err = 0;
			continue;					
		}	

		if(0 == p_dependent_list[i].addr){
			break;
		}
	}	

	foreach(i, MAX_DEPENDENT_NUM){ // sort dependent list 
		if(0 == p_dependent_list[i].addr){
			for(int j=i+1; j<MAX_DEPENDENT_NUM; j++){
				if(p_dependent_list[j].addr){
					p_dependent_list[i] = p_dependent_list[j];
					memset(&p_dependent_list[j], 0x00, sizeof(path_addr_t));
					break;
				}
				if(j == MAX_DEPENDENT_NUM-1){
					i = MAX_DEPENDENT_NUM; // break;
				}
			}
		}
	}
	return err;
}

non_fixed_entry_t * add_forwarding_entry_by_path_reply_delay_timer_expired(u16 netkey_offset, discovery_entry_t *p_dsc_entry)
{
	non_fixed_entry_t tbl_entry;
	memset(&tbl_entry, 0x00, sizeof(tbl_entry));

	tbl_entry.entry.fixed_path = 0;
	tbl_entry.entry.backward_path_validated = 0;
	tbl_entry.entry.path_not_ready = 0;
	tbl_entry.entry.path_origin = p_dsc_entry->path_origin.addr;
	tbl_entry.entry.path_origin_snd_ele_cnt = p_dsc_entry->path_origin.snd_ele_cnt;
	tbl_entry.state.forwarding_number = p_dsc_entry->forwarding_number;
	tbl_entry.entry.bearer_toward_path_origin = p_dsc_entry->bearer_toward_path_origin;
	
	if(p_dsc_entry->dependent_origin.addr){
		forwarding_tbl_dependent_add(p_dsc_entry->dependent_origin.addr, p_dsc_entry->dependent_origin.snd_ele_cnt, tbl_entry.entry.dependent_origin);
	}

	if(is_group_adr(p_dsc_entry->destination) || is_virtual_adr(p_dsc_entry->destination)){
		tbl_entry.entry.destination = p_dsc_entry->destination;				
	}
	else{
		tbl_entry.entry.destination = ele_adr_primary;
		tbl_entry.entry.path_target_snd_ele_cnt = g_ele_cnt - 1;

		u16 match_F2L = mesh_mac_match_friend(p_dsc_entry->destination);
		if(match_F2L){
			foreach(i, g_max_lpn_num){
				if((match_F2L>>i) & 0x01){						
					forwarding_tbl_dependent_add(fn_other_par[i].LPNAdr, fn_req[i].NumEle-1, tbl_entry.entry.dependent_target);
					break;
				}
			}
		}
		else{
			u8 snd_ele_cnt = 0;
			if(is_proxy_client_addr(p_dsc_entry->destination)){
				if(DIRECTED_PROXY_CLIENT == proxy_mag.proxy_client_type){
					foreach(i, NET_KEY_MAX){
						if(p_dsc_entry->destination == proxy_mag.directed_server[i].client_addr){
							snd_ele_cnt = proxy_mag.directed_server[i].client_2nd_ele_cnt;
							break;
						}
					}	
				}
				forwarding_tbl_dependent_add(p_dsc_entry->destination, snd_ele_cnt, tbl_entry.entry.dependent_target);
			}
		}
	}

	tbl_entry.entry.bearer_toward_path_target = MESH_BEAR_UNASSIGNED;
	tbl_entry.state.lane_counter = 1;
	
	non_fixed_entry_t *p_fwd_entry = get_non_fixed_path_entry(netkey_offset, tbl_entry.entry.path_origin, tbl_entry.entry.destination);
	if(p_fwd_entry){
		delete_non_fixed_path(netkey_offset, &p_fwd_entry->entry);
	}
	return add_new_path_in_non_fixed_fwd_tbl(netkey_offset, &tbl_entry);
}

void update_forwarding_entry_by_path_reply_delay_timer_expired(u16 netkey_offset, non_fixed_entry_t *p_fwd_entry, discovery_entry_par_t *p_dsc_entry)
{
	model_sig_g_df_sbr_cfg.df_cfg.fixed_fwd_tbl[netkey_offset].update_id++;
	p_fwd_entry->entry.bearer_toward_path_origin = p_dsc_entry->entry.bearer_toward_path_origin;
	p_fwd_entry->state.lane_counter++;
}

non_fixed_entry_t * get_forwarding_entry_correspond2_path_reply(u16 netkey_offset, mesh_ctl_path_reply_t *p_path_reply)
{
	if(netkey_offset < NET_KEY_MAX){
		foreach(i, MAX_NON_FIXED_PATH){
			non_fixed_entry_t *p_fwd_entry = &non_fixed_fwd_tbl[netkey_offset].path[i];
			if((p_path_reply->path_origin == p_fwd_entry->entry.path_origin) && (p_path_reply->forwarding_number == p_fwd_entry->state.forwarding_number)){
				return p_fwd_entry;
			}
		}
	}
	return 0;
}

non_fixed_entry_t * add_forwarding_tbl_entry_by_rcv_path_reply(u16 netkey_offset, u8 bearer_toward_path_target, discovery_entry_par_t *p_dsc_entry, mesh_ctl_path_reply_t *p_path_reply)
{
	non_fixed_entry_t tbl_entry;
	memset(&tbl_entry, 0x00, sizeof(tbl_entry));
	
	addr_range_t *p_path_target = (addr_range_t *)p_path_reply->addr_par;
	if(is_own_ele(p_path_reply->path_origin) && (0 == p_path_target->range_start_b)){
		tbl_entry.entry.path_not_ready = 1;
	}

	tbl_entry.entry.path_origin = p_dsc_entry->entry.path_origin.addr;
	tbl_entry.entry.path_origin_snd_ele_cnt = p_dsc_entry->entry.path_origin.snd_ele_cnt;
	tbl_entry.state.forwarding_number = p_dsc_entry->entry.forwarding_number;

	if(!is_own_ele(p_path_reply->path_origin)){
		tbl_entry.entry.bearer_toward_path_origin = p_dsc_entry->entry.bearer_toward_path_origin;
	}
	else{
		tbl_entry.entry.bearer_toward_path_origin = MESH_BEAR_UNASSIGNED;
	}
	
	if(p_dsc_entry->entry.dependent_origin.addr){
		forwarding_tbl_dependent_add(p_dsc_entry->entry.dependent_origin.addr, p_dsc_entry->entry.dependent_origin.snd_ele_cnt, tbl_entry.entry.dependent_origin);
	}

	if(is_group_adr(p_dsc_entry->entry.destination) || is_virtual_adr(p_dsc_entry->entry.destination)){
		tbl_entry.entry.destination = p_dsc_entry->entry.destination;				
	}
	else{
		tbl_entry.entry.destination = p_path_target->range_start_b;
		tbl_entry.entry.path_target_snd_ele_cnt = p_path_target->length_present_b?(p_path_target->range_length-1):0;
	}

	if(p_path_reply->on_behalf_of_dependent_target){	
		addr_range_t *p_dependent_target = (addr_range_t *)(p_path_reply->addr_par + (p_path_target->length_present_b?3:2));
		forwarding_tbl_dependent_add(p_dependent_target->range_start_b, p_dependent_target->length_present_b?(p_dependent_target->range_length-1):0, tbl_entry.entry.dependent_target);
	}

	tbl_entry.entry.bearer_toward_path_target = bearer_toward_path_target;
	tbl_entry.state.lane_counter = 1;

	non_fixed_entry_t *p_fwd_entry = get_non_fixed_path_entry(netkey_offset, tbl_entry.entry.path_origin, tbl_entry.entry.destination);
	if(p_fwd_entry){
		delete_non_fixed_path(netkey_offset, &p_fwd_entry->entry);
	}
	
	return add_new_path_in_non_fixed_fwd_tbl(netkey_offset, &tbl_entry);
}

void update_forwarding_entry_by_path_reply(u8 netkey_offset, u8 src_type, non_fixed_entry_t *p_fwd_entry, discovery_entry_par_t *p_dsc_entry)
{
	model_sig_g_df_sbr_cfg.df_cfg.fixed_fwd_tbl[netkey_offset].update_id++;
	if(!is_own_ele(p_dsc_entry->entry.path_origin.addr)){
		p_fwd_entry->entry.bearer_toward_path_origin = p_dsc_entry->entry.bearer_toward_path_origin;
	}
	p_fwd_entry->entry.bearer_toward_path_target = src_type;
	if(p_dsc_entry->state.first_reply_msg){		
		p_fwd_entry->state.lane_counter++;
	}
}

non_fixed_entry_t * get_forwarding_entry_correspond2_path_confirm(u16 netkey_offset, mesh_ctl_path_confirmation_t *p_path_comfirm)
{
	if(netkey_offset < NET_KEY_MAX){
		foreach(i, MAX_NON_FIXED_PATH){
			non_fixed_entry_t *p_fwd_entry = &non_fixed_fwd_tbl[netkey_offset].path[i];
			if((0 == p_fwd_entry->entry.fixed_path) && (p_path_comfirm->path_origin == p_fwd_entry->entry.path_origin) && (p_path_comfirm->path_target == p_fwd_entry->entry.destination)){
				p_fwd_entry->entry.backward_path_validated = 1;
				model_sig_g_df_sbr_cfg.df_cfg.fixed_fwd_tbl[netkey_offset].update_id++;
				return p_fwd_entry;
			}
		}
	}
	return 0;
}

void update_forwarding_entry_by_path_confirm(u16 netkey_offset, non_fixed_entry_t *p_fwd_entry, mesh_ctl_path_confirmation_t *p_path_comfirm)
{
	model_sig_g_df_sbr_cfg.df_cfg.fixed_fwd_tbl[netkey_offset].update_id++;
	p_fwd_entry->entry.backward_path_validated = 1;
}

non_fixed_entry_t * get_forwarding_entry_correspond2_path_echo_request(u16 netkey_offset, u16 src_addr, u16 dst_addr)
{
	if(netkey_offset < NET_KEY_MAX){
		foreach(i, MAX_NON_FIXED_PATH){
			non_fixed_entry_t *p_fwd_entry = &non_fixed_fwd_tbl[netkey_offset].path[i];
			if((0 == p_fwd_entry->entry.fixed_path) && (src_addr == p_fwd_entry->entry.path_origin) && (dst_addr == p_fwd_entry->entry.destination)){
				return p_fwd_entry;
			}
		}
	}
	return 0;
}

non_fixed_entry_t * get_forwarding_entry_correspond2_path_echo_reply(u16 netkey_offset, u16 network_dst, mesh_ctl_path_echo_reply_t *p_path_echo_reply)
{
	if(netkey_offset < NET_KEY_MAX){
		foreach(i, MAX_NON_FIXED_PATH){
			non_fixed_entry_t *p_fwd_entry = &non_fixed_fwd_tbl[netkey_offset].path[i];
			if((0 == p_fwd_entry->entry.fixed_path) && (network_dst == p_fwd_entry->entry.path_origin) && (p_path_echo_reply->destination == p_fwd_entry->entry.destination)){
				return p_fwd_entry;
			}
		}
	}
	return 0;
}

non_fixed_entry_t * get_forwarding_entry_correspond2_dependent_node_update(u16 netkey_offset, mesh_ctl_dependent_node_update_t *p_dependent_node_update)
{
	u16 dependent_addr = p_dependent_node_update->dependent_addr.length_present_b ? p_dependent_node_update->dependent_addr.range_start_b:p_dependent_node_update->dependent_addr.multicast_addr;
	if(netkey_offset < NET_KEY_MAX){
		foreach(i, MAX_NON_FIXED_PATH){
			non_fixed_entry_t *p_fwd_entry = &non_fixed_fwd_tbl[netkey_offset].path[i];
			if(0 == p_fwd_entry->entry.fixed_path){
				if(((p_dependent_node_update->path_endpoint==p_fwd_entry->entry.path_origin) && (DEPENDENT_TYPE_REMOVE == p_dependent_node_update->type) && is_address_in_dependent_origin(&p_fwd_entry->entry, dependent_addr)) ||
					((p_dependent_node_update->path_endpoint==p_fwd_entry->entry.path_origin) && (DEPENDENT_TYPE_ADD == p_dependent_node_update->type) && !is_address_in_dependent_origin(&p_fwd_entry->entry, dependent_addr)) ||
					((p_dependent_node_update->path_endpoint==p_fwd_entry->entry.destination) && (DEPENDENT_TYPE_REMOVE == p_dependent_node_update->type) && is_address_in_dependent_target(&p_fwd_entry->entry, dependent_addr)) ||
					((p_dependent_node_update->path_endpoint==p_fwd_entry->entry.destination) && (DEPENDENT_TYPE_ADD == p_dependent_node_update->type) && p_fwd_entry->entry.backward_path_validated && !is_address_in_dependent_target(&p_fwd_entry->entry, dependent_addr))
				){
					return p_fwd_entry;
				}
			}
		}
	}
	return 0;
}

int update_forwarding_by_dependent_node_update(u16 netkey_offset, mesh_ctl_dependent_node_update_t *p_dependent_update)
{
	int ret = -1;
	if(netkey_offset < NET_KEY_MAX){
		foreach(i, MAX_NON_FIXED_PATH){
			non_fixed_entry_t *p_fwd_entry = &non_fixed_fwd_tbl[netkey_offset].path[i];
			if(0 == p_fwd_entry->entry.fixed_path){
				if(((p_dependent_update->path_endpoint==p_fwd_entry->entry.path_origin) && (DEPENDENT_TYPE_REMOVE==p_dependent_update->type) && is_address_in_dependent_origin(&p_fwd_entry->entry, p_dependent_update->dependent_addr.range_start_b)) ||
					((p_dependent_update->path_endpoint==p_fwd_entry->entry.path_origin) && (DEPENDENT_TYPE_ADD==p_dependent_update->type) && !is_address_in_dependent_origin(&p_fwd_entry->entry, p_dependent_update->dependent_addr.range_start_b)) ||
					((p_dependent_update->path_endpoint==p_fwd_entry->entry.destination) && (DEPENDENT_TYPE_REMOVE==p_dependent_update->type) && is_address_in_dependent_target(&p_fwd_entry->entry, p_dependent_update->dependent_addr.range_start_b))	||
					((p_dependent_update->path_endpoint==p_fwd_entry->entry.destination) && (DEPENDENT_TYPE_ADD==p_dependent_update->type) && p_fwd_entry->entry.backward_path_validated && !is_address_in_dependent_target(&p_fwd_entry->entry, p_dependent_update->dependent_addr.range_start_b))				
				){
					if(p_dependent_update->path_endpoint==p_fwd_entry->entry.path_origin){
						if(p_dependent_update->type){
							ret = forwarding_tbl_dependent_add(p_dependent_update->dependent_addr.range_start_b, p_dependent_update->dependent_addr.length_present_b?(p_dependent_update->dependent_addr.range_length-1):0, p_fwd_entry->entry.dependent_origin);
						}
						else{
							ret = forwarding_tbl_dependent_delete(p_dependent_update->dependent_addr.range_start_b, p_fwd_entry->entry.dependent_origin);
						}
					}
					else if(p_dependent_update->path_endpoint==p_fwd_entry->entry.destination){
						if(p_dependent_update->type){
							ret = forwarding_tbl_dependent_add(p_dependent_update->dependent_addr.range_start_b, p_dependent_update->dependent_addr.length_present_b?(p_dependent_update->dependent_addr.range_length-1):0, p_fwd_entry->entry.dependent_target);
						}
						else{
							ret = forwarding_tbl_dependent_delete(p_dependent_update->dependent_addr.range_start_b, p_fwd_entry->entry.dependent_target);
						}	
					}
					model_sig_g_df_sbr_cfg.df_cfg.fixed_fwd_tbl[netkey_offset].update_id++;
				}
			}
		}
	}
	return ret;
}

non_fixed_entry_t * get_forwarding_entry_correspond2_path_request_solication(u16 netkey_offset, mesh_ctl_path_request_solication_t *p_request_solication, int par_len)
{
	if(netkey_offset < NET_KEY_MAX){
		foreach(i, MAX_NON_FIXED_PATH){
			non_fixed_entry_t *p_fwd_entry = &non_fixed_fwd_tbl[netkey_offset].path[i];
			if((0 == p_fwd_entry->entry.fixed_path) && is_own_ele(p_fwd_entry->entry.path_origin)){
				foreach(i, par_len/2){
					if(p_fwd_entry->entry.destination == p_request_solication->addr_list[i]){
						return p_fwd_entry;
					}
				}
			}
		}
	}
	return 0;
}
#endif
//---------------------------forwarding table process end---------------------//

//---------------------------forwarding config message-----------------------------------//
int mesh_cmd_sig_cfg_directed_control_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	directed_control_sts_t directed_control_sts;	
	u16 netkey_index = par[0] + (par[1]<<8);
	directed_control_sts.status = ST_SUCCESS;
	directed_control_sts.netkey_index = netkey_index; 
	if(is_netkey_index_prohibited(netkey_index)){
		return err;
	}
	
	int key_offset = get_mesh_net_key_offset(netkey_index);	
	if(-1 != key_offset){
		memcpy(&directed_control_sts.directed_control, &model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[key_offset].directed_control, sizeof(directed_control_t));
	}
	else{
		directed_control_sts.status = ST_INVALID_NETKEY;
		memset(&directed_control_sts.directed_control, 0x00, sizeof(directed_control_sts.directed_control));
	}

	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&directed_control_sts),sizeof(directed_control_sts),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_directed_control_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	directed_control_set_t *p_set = (directed_control_set_t *)par;
	directed_control_sts_t directed_control_sts;
	directed_control_sts.status = ST_SUCCESS;
	memcpy(&directed_control_sts.netkey_index, p_set, sizeof(directed_control_sts_t)-OFFSETOF(directed_control_sts_t, netkey_index));
	directed_control_sts.netkey_index = p_set->netkey_index;
	if(is_netkey_index_prohibited(p_set->netkey_index)){
		return err;
	}
	
	int key_offset = get_mesh_net_key_offset(p_set->netkey_index);	
	if(-1 != key_offset){
		if((p_set->directed_control.directed_forwarding > DIRECTED_FORWARDING_ENABLE) || 
			(p_set->directed_control.directed_relay > DIRECTED_RELAY_ENABLE) ||
			((p_set->directed_control.directed_proxy > DIRECTED_PROXY_ENABLE) && (p_set->directed_control.directed_proxy != DIRECTED_PROXY_IGNORE)) || 			
			((p_set->directed_control.directed_proxy_directed_default > DIRECTED_PROXY_DEFAULT_ENABLE) && (p_set->directed_control.directed_proxy_directed_default != DIRECTED_PROXY_DEFAULT_IGNORE)) ||
			((p_set->directed_control.directed_friend > DIRECTED_FRIEND_ENABLE) && (p_set->directed_control.directed_friend != DIRECTED_FRIEND_IGNORE))){
				return err;
		}

		if(((DIRECTED_FORWARDING_DISABLE == p_set->directed_control.directed_forwarding))
		){
			if((DIRECTED_RELAY_ENABLE == p_set->directed_control.directed_relay) ||
			   (DIRECTED_PROXY_ENABLE == p_set->directed_control.directed_proxy) ||
			   (DIRECTED_PROXY_DEFAULT_IGNORE != p_set->directed_control.directed_proxy_directed_default) ||
			   (DIRECTED_FRIEND_IGNORE != p_set->directed_control.directed_friend)){
				return err;
			}
			memset(non_fixed_fwd_tbl, 0x00, sizeof(non_fixed_fwd_tbl));
			memset(discovery_table, 0x00, sizeof(discovery_table));
		}	
		else{
			if((DIRECTED_PROXY_ENABLE == p_set->directed_control.directed_proxy) && (DIRECTED_PROXY_DEFAULT_IGNORE == p_set->directed_control.directed_proxy_directed_default)){
				return err;
			}
		}
		
		u8 need_capa_report = 0;
		directed_control_t *p_control = (directed_control_t *)&model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[key_offset].directed_control;
	
		if((UNSET_CLIENT == proxy_mag.proxy_client_type) || (DIRECTED_PROXY_CLIENT == proxy_mag.proxy_client_type)){
			if((DIRECTED_PROXY_ENABLE==p_control->directed_proxy) && (DIRECTED_PROXY_DISABLE == p_set->directed_control.directed_proxy)){// directed proxy enable to disable
				need_capa_report = 1;
			}	
			else if(((DIRECTED_PROXY_DISABLE==p_control->directed_proxy) && (DIRECTED_PROXY_ENABLE == p_set->directed_control.directed_proxy))){ //directed proxy disable to enable
				need_capa_report = 1;
			}
		}
	
		p_control->directed_forwarding = p_set->directed_control.directed_forwarding;
		p_control->directed_relay = p_set->directed_control.directed_relay;
		
		if(DIRECTED_PROXY_IGNORE != p_set->directed_control.directed_proxy){
			p_control->directed_proxy = p_set->directed_control.directed_proxy;
		}
		
		if(DIRECTED_PROXY_DEFAULT_IGNORE != p_set->directed_control.directed_proxy_directed_default){
			p_control->directed_proxy_directed_default = p_set->directed_control.directed_proxy_directed_default;
		}
		
		if(DIRECTED_FRIEND_IGNORE != p_set->directed_control.directed_friend){
			p_control->directed_friend = p_set->directed_control.directed_friend;
		}
		
		mesh_directed_forwarding_bind_state_update();	
		if((DIRECTED_PROXY_DISABLE == p_set->directed_control.directed_proxy)){
			memset(&proxy_mag.directed_server[key_offset], 0x00, sizeof(proxy_mag.directed_server[key_offset]));// must after mesh_directed_forwarding_bind_state_update
		}
		proxy_mag.directed_server[key_offset].use_directed = (DIRECTED_PROXY_DEFAULT_ENABLE == p_control->directed_proxy_directed_default);
		if(need_capa_report){
			mesh_directed_proxy_capa_report(key_offset);
		}
		
		memcpy(&directed_control_sts.directed_control, &model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[key_offset].directed_control, sizeof(directed_control_t));
		mesh_common_store(FLASH_ADR_MD_DF_SBR);
	}	
	else{
		directed_control_sts.status = ST_INVALID_NETKEY;		
	}
		
	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&directed_control_sts),sizeof(directed_control_sts),cb_par->adr_src);
	return err;
}

#if !FEATURE_LOWPOWER_EN
int mesh_cmd_sig_cfg_path_metric_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	path_metric_sts_t path_metric_sts;	
	u16 netkey_index = par[0] + (par[1]<<8);
	path_metric_sts.status = ST_SUCCESS;
	path_metric_sts.netkey_index = netkey_index;
	if(is_netkey_index_prohibited(netkey_index)){
		return err;
	}
	
	int key_offset = get_mesh_net_key_offset(netkey_index);	
	if(-1 != key_offset){
		memcpy(&path_metric_sts.path_metric, &model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[key_offset].path_metric, sizeof(path_metric_t));
	}
	else{
		path_metric_sts.status = ST_INVALID_NETKEY;
		memset(&path_metric_sts.path_metric, 0x00, sizeof(path_metric_t));
	}

	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&path_metric_sts),sizeof(path_metric_sts),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_path_metric_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	path_metric_set_t *p_metric_set = (path_metric_set_t *)par;	
	path_metric_sts_t path_metric_sts;

	path_metric_sts.status = ST_SUCCESS;
	path_metric_sts.netkey_index = p_metric_set->netkey_index;
	memcpy(&path_metric_sts.path_metric, &p_metric_set->path_metric, sizeof(path_metric_t));
	if(is_netkey_index_prohibited(p_metric_set->netkey_index) || (p_metric_set->path_metric.metric_type>=METRIC_TYPE_RFU) || p_metric_set->path_metric.rfu){
		return err;
	}
	
	int key_offset = get_mesh_net_key_offset(p_metric_set->netkey_index);	
	if(-1 != key_offset){
		mesh_directed_subnet_state_t *p_subnet_state = &model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[key_offset];
		p_subnet_state->path_metric = p_metric_set->path_metric;
		
		
		p_subnet_state->path_metric.path_lifetime = p_metric_set->path_metric.path_lifetime;
		mesh_common_store(FLASH_ADR_MD_DF_SBR);
	}
	else{
		path_metric_sts.status = ST_INVALID_NETKEY;		
	}

	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&path_metric_sts),sizeof(path_metric_sts),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_dsc_tbl_capa_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	dsc_tbl_capa_sts_t dsc_tbl_capa_sts;	
	u16 netkey_index = par[0] + (par[1]<<8);
	dsc_tbl_capa_sts.status = ST_SUCCESS;
	dsc_tbl_capa_sts.netkey_index = netkey_index;
	if(is_netkey_index_prohibited(netkey_index)){
		return err;
	}
	
	int key_offset = get_mesh_net_key_offset(netkey_index);	
	if(-1 != key_offset){
		dsc_tbl_capa_sts.max_concurrent_init = model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[key_offset].max_concurrent_init;
		dsc_tbl_capa_sts.max_dsc_tbl_entries_cnt = MAX_CONCURRENT_CNT; 
	}
	else{
		dsc_tbl_capa_sts.status = ST_INVALID_NETKEY;
		dsc_tbl_capa_sts.max_concurrent_init = 0;
		dsc_tbl_capa_sts.max_dsc_tbl_entries_cnt = 0; 
	}

	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&dsc_tbl_capa_sts),sizeof(dsc_tbl_capa_sts),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_dsc_tbl_capa_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	dsc_tbl_capa_sts_t dsc_tbl_capa_sts;
	dsc_tbl_capa_set_t *p_capa_set = (dsc_tbl_capa_set_t *)par;	
	dsc_tbl_capa_sts.status = ST_SUCCESS;
	dsc_tbl_capa_sts.netkey_index = p_capa_set->netkey_index;
	dsc_tbl_capa_sts.max_concurrent_init = p_capa_set->max_concurrent_init;
	dsc_tbl_capa_sts.max_dsc_tbl_entries_cnt = MAX_DSC_TBL; 
	if(is_netkey_index_prohibited(p_capa_set->netkey_index) || (0 == p_capa_set->max_concurrent_init)){
		return err;
	}
		
	int key_offset = get_mesh_net_key_offset(p_capa_set->netkey_index);	
	if((-1 == key_offset)){
		dsc_tbl_capa_sts.status = ST_INVALID_NETKEY;
	}
	else if(p_capa_set->max_concurrent_init>MAX_DSC_TBL){
		dsc_tbl_capa_sts.status = ST_CAN_NOT_SET;
	}
	else{
		model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[key_offset].max_concurrent_init = p_capa_set->max_concurrent_init;
		mesh_common_store(FLASH_ADR_MD_DF_SBR);
	}

	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&dsc_tbl_capa_sts),sizeof(dsc_tbl_capa_sts),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_forwarding_tbl_add(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	forwarding_tbl_status_t forwarding_tbl_status;	
	path_entry_com_t tbl_entry;
	path_entry_com_t *p_fwd_entry;
	forwarding_tbl_add_t *p_tbl_add = (forwarding_tbl_add_t *)par;
	forwarding_tbl_status.status = ST_SUCCESS;
	forwarding_tbl_status.netkey_index = p_tbl_add->netkey_index;

	memset(&tbl_entry, 0x00, sizeof(tbl_entry));
	addr_range_t* p_adr_range = (addr_range_t*)p_tbl_add->par;
	tbl_entry.fixed_path = 1;
	tbl_entry.backward_path_validated = p_tbl_add->backward_path_validated_flag;
	tbl_entry.path_not_ready = 0;
	tbl_entry.path_origin = p_adr_range->range_start_l;
	tbl_entry.path_origin_snd_ele_cnt = p_adr_range->length_present_l ? (p_adr_range->range_length-1):0;
	u8 cur_par_offset = p_adr_range->length_present_l ? 3:2;
	p_adr_range = (addr_range_t*)(p_tbl_add->par + cur_par_offset);
	if(p_tbl_add->unicast_destination_flag){
		tbl_entry.destination = p_adr_range->range_start_l;
		tbl_entry.path_target_snd_ele_cnt = p_adr_range->length_present_l ? (p_adr_range->range_length-1):0;
		cur_par_offset += (p_adr_range->length_present_l ? 3:2);						
	}
	else{
		tbl_entry.destination = p_adr_range->multicast_addr;		
		cur_par_offset += 2;	
	}
	tbl_entry.bearer_toward_path_origin = p_tbl_add->par[cur_par_offset] + (p_tbl_add->par[cur_par_offset+1]<<8);
	cur_par_offset += 2;
	tbl_entry.bearer_toward_path_target = p_tbl_add->par[cur_par_offset] + (p_tbl_add->par[cur_par_offset+1]<<8);

	if((p_tbl_add->backward_path_validated_flag && !is_unicast_adr(tbl_entry.path_origin+tbl_entry.path_origin_snd_ele_cnt)) ||
		(ADR_UNASSIGNED == tbl_entry.path_origin)  || p_tbl_add->prohibited ||
		!((tbl_entry.path_origin>tbl_entry.destination+tbl_entry.path_target_snd_ele_cnt) || ((tbl_entry.destination>tbl_entry.path_origin+tbl_entry.path_origin_snd_ele_cnt))) ||
		(ADR_UNASSIGNED == tbl_entry.destination) || is_fixed_group(tbl_entry.destination)){
		return err;
	}
	
	int key_offset = get_mesh_net_key_offset(p_tbl_add->netkey_index);	
	if(-1 != key_offset){
		//mesh_match_type_t match_type;
    	//mesh_match_group_mac(&match_type, tbl_entry.destination, 0, 0, 0);
		p_fwd_entry = get_fixed_path_entry(key_offset, tbl_entry.path_origin, tbl_entry.destination);
		if(is_own_ele(tbl_entry.path_origin)){
			if(!((MESH_BEAR_UNASSIGNED == tbl_entry.bearer_toward_path_origin) && (tbl_entry.bearer_toward_path_target && (tbl_entry.bearer_toward_path_target<MESH_BEAR_SUPPORT)))){
				forwarding_tbl_status.status = ST_INVALID_BEARER;
			}
		}
		else if(is_own_ele(tbl_entry.destination)){
			if(!((MESH_BEAR_UNASSIGNED == tbl_entry.bearer_toward_path_target) && (tbl_entry.bearer_toward_path_origin && (tbl_entry.bearer_toward_path_origin<MESH_BEAR_SUPPORT)))){
				forwarding_tbl_status.status = ST_INVALID_BEARER;
			}
		}
		else if(!((tbl_entry.bearer_toward_path_origin && (tbl_entry.bearer_toward_path_origin<MESH_BEAR_SUPPORT)) && (tbl_entry.bearer_toward_path_target && (tbl_entry.bearer_toward_path_target<MESH_BEAR_SUPPORT)))){
			forwarding_tbl_status.status = ST_INVALID_BEARER;
		}
		
		if(ST_SUCCESS == forwarding_tbl_status.status){
			if(p_fwd_entry){// exist, update entry
				p_fwd_entry->backward_path_validated = tbl_entry.backward_path_validated;
				p_fwd_entry->bearer_toward_path_origin = tbl_entry.bearer_toward_path_origin;
				p_fwd_entry->bearer_toward_path_target = tbl_entry.bearer_toward_path_target;
				model_sig_g_df_sbr_cfg.df_cfg.fixed_fwd_tbl[key_offset].update_id++;
			}
			else{// not exist, add a new fixed path entry
				if(add_new_path_in_fixed_fwd_tbl(key_offset, &tbl_entry)){
					err = 0;
				}
				else{
					forwarding_tbl_status.status = ST_INSUFFICIENT_RES;					
				}
			}
		}
		
	}
	else{
		forwarding_tbl_status.status = ST_INVALID_NETKEY;
	}

	if(ST_SUCCESS == forwarding_tbl_status.status){
		mesh_common_store(FLASH_ADR_MD_DF_SBR);
	}
	
	forwarding_tbl_status.path_origin = tbl_entry.path_origin;
	forwarding_tbl_status.destination = tbl_entry.destination;	
	
	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&forwarding_tbl_status),sizeof(forwarding_tbl_status),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_forwarding_tbl_delete(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	forwarding_tbl_status_t forwarding_tbl_status;
	forwarding_tbl_delete_t *p_tbl_delete = (forwarding_tbl_delete_t *)par;	
	forwarding_tbl_status.status = ST_SUCCESS;
	forwarding_tbl_status.netkey_index = p_tbl_delete->netkey_index;
	forwarding_tbl_status.path_origin = p_tbl_delete->path_origin;
	forwarding_tbl_status.destination = p_tbl_delete->destination;
	if(is_netkey_index_prohibited(p_tbl_delete->netkey_index) || !is_unicast_adr(p_tbl_delete->path_origin) || is_fixed_group(p_tbl_delete->destination) ||
	    (p_tbl_delete->path_origin == p_tbl_delete->destination) || (ADR_UNASSIGNED == p_tbl_delete->path_origin) || (ADR_UNASSIGNED == p_tbl_delete->destination) ||
	    is_fixed_group(p_tbl_delete->destination)){
		return err;
	}
	
	int key_offset = get_mesh_net_key_offset(p_tbl_delete->netkey_index);	
	if((-1 == key_offset)){
		forwarding_tbl_status.status = ST_INVALID_NETKEY;
	}
	else{
		path_entry_com_t tbl_entry;
		tbl_entry.path_origin = p_tbl_delete->path_origin;
		tbl_entry.destination = p_tbl_delete->destination;
		delete_fixed_path(key_offset, &tbl_entry);
	}

	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&forwarding_tbl_status),sizeof(forwarding_tbl_status),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_forwarding_tbl_dependents_add(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	forwarding_tbl_status_t forwarding_tbl_status;	
	path_entry_com_t *p_entry;
	forwarding_tbl_dependengts_add_t *p_tbl_dependents_add = (forwarding_tbl_dependengts_add_t *)par;
	forwarding_tbl_status.status = ST_SUCCESS;
	forwarding_tbl_status.netkey_index = p_tbl_dependents_add->netkey_index;
	if(is_netkey_index_prohibited(p_tbl_dependents_add->netkey_index) || !is_unicast_adr(p_tbl_dependents_add->path_origin) || (ADR_UNASSIGNED == p_tbl_dependents_add->destination) || (p_tbl_dependents_add->path_origin == p_tbl_dependents_add->destination) ||
		!(p_tbl_dependents_add->dependent_origin_list_size || p_tbl_dependents_add->dependent_target_list_size) || is_fixed_group(p_tbl_dependents_add->destination)){
		return err;
	}
	
	int key_offset = get_mesh_net_key_offset(p_tbl_dependents_add->netkey_index);	
	if(-1 != key_offset){
		p_entry = get_fixed_path_entry(key_offset, p_tbl_dependents_add->path_origin, p_tbl_dependents_add->destination);
		if(!p_entry){
			forwarding_tbl_status.status = ST_INVALID_PATH_ENTRY;
		}

		u8 range_par_offset = 0;
		addr_range_t * p_addr_range = 0;
		u16 dep_org_addr = 0;
		u8 dep_org_len = 0;
		u8 range_len = 0;
		int dependent_change = 0;
		foreach(i, p_tbl_dependents_add->dependent_origin_list_size){
			p_addr_range = (addr_range_t *)(p_tbl_dependents_add->par+range_par_offset);
			dep_org_addr = p_addr_range->range_start_l;
			dep_org_len = p_addr_range->length_present_l?(p_addr_range->range_length-1):0;
			range_par_offset += (p_addr_range->length_present_l ? 3:2);			
			if(!is_unicast_adr(dep_org_addr+dep_org_len) || (dep_org_addr == p_tbl_dependents_add->path_origin)){
				return err;
			}
			
			if(p_entry){
				if(forwarding_tbl_dependent_add(dep_org_addr, dep_org_len, p_entry->dependent_origin)){
					forwarding_tbl_status.status = ST_INSUFFICIENT_RES;
				}
				else{
					dependent_change = 1;
				}
			}
		}

		if(p_tbl_dependents_add->dependent_target_list_size){
			foreach(i, p_tbl_dependents_add->dependent_target_list_size){
				p_addr_range = (addr_range_t *)(p_tbl_dependents_add->par+range_par_offset);
				range_len = p_addr_range->length_present_l?(p_addr_range->range_length-1):0;
				range_par_offset += (p_addr_range->length_present_l ? 3:2);
				if(!is_unicast_adr(p_addr_range->range_start_l+range_len) || (p_addr_range->range_start_l == p_tbl_dependents_add->path_origin) ||
					!((dep_org_addr > (p_addr_range->range_start_l+range_len)) || (p_addr_range->range_start_l > (dep_org_addr + dep_org_len)))){				
					return err;
				}

				if(p_entry){	
					if(forwarding_tbl_dependent_add(p_addr_range->range_start_l, range_len, p_entry->dependent_target)){
						forwarding_tbl_status.status = ST_INSUFFICIENT_RES;
					}
					else{
						dependent_change = 1;
					}
				}
			}
		}
		
		if(dependent_change){
			model_sig_g_df_sbr_cfg.df_cfg.fixed_fwd_tbl[key_offset].update_id++;
			mesh_common_store(FLASH_ADR_MD_DF_SBR);
		}		
	}
	else{
		forwarding_tbl_status.status = ST_INVALID_NETKEY;
	}
	forwarding_tbl_status.path_origin = p_tbl_dependents_add->path_origin;
	forwarding_tbl_status.destination = p_tbl_dependents_add->destination;	
	
	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&forwarding_tbl_status),sizeof(forwarding_tbl_status),cb_par->adr_src);
	
	return err;
}

int mesh_cmd_sig_cfg_forwarding_tbl_dependents_delete(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	forwarding_tbl_status_t forwarding_tbl_status;
	path_entry_com_t *p_entry;
	forwarding_tbl_dependengts_delete_t *p_tbl_delete = (forwarding_tbl_dependengts_delete_t *)par;	
	forwarding_tbl_status.status = ST_SUCCESS;
	forwarding_tbl_status.netkey_index = p_tbl_delete->netkey_index;
	forwarding_tbl_status.path_origin = p_tbl_delete->path_origin;
	forwarding_tbl_status.destination = p_tbl_delete->destination;
	if(is_netkey_index_prohibited(p_tbl_delete->netkey_index) || !is_unicast_adr(p_tbl_delete->path_origin) || (ADR_UNASSIGNED == p_tbl_delete->destination) || (p_tbl_delete->path_origin == p_tbl_delete->destination) ||
		!(p_tbl_delete->dependent_origin_list_size || p_tbl_delete->dependent_target_list_size) || is_fixed_group(p_tbl_delete->destination)){
		return err;
	}
	
	int key_offset = get_mesh_net_key_offset(p_tbl_delete->netkey_index);	
	if((-1 == key_offset)){
		forwarding_tbl_status.status = ST_INVALID_NETKEY;
	}
	else{		
		p_entry = get_fixed_path_entry(key_offset, p_tbl_delete->path_origin, p_tbl_delete->destination);
		if(!p_entry){
			forwarding_tbl_status.status = ST_INVALID_PATH_ENTRY;
		}
		
		u16 dep_org_addr = 0;
		int dependent_change = 0;
		if(p_tbl_delete->dependent_origin_list_size){
			foreach(i, p_tbl_delete->dependent_origin_list_size){
				dep_org_addr = p_tbl_delete->addr[i];
				if(!is_unicast_adr(dep_org_addr) || (dep_org_addr == p_tbl_delete->path_origin)){
					return err;
				}

				if(p_entry){
					if(!forwarding_tbl_dependent_delete(dep_org_addr, p_entry->dependent_origin)){
						dependent_change = 1;
					}
				}
			}
		}
		
		if(p_tbl_delete->dependent_target_list_size){
			foreach(i, p_tbl_delete->dependent_target_list_size){
				u16 addr = p_tbl_delete->addr[p_tbl_delete->dependent_origin_list_size + i];
				if(!is_unicast_adr(addr) || (addr == p_tbl_delete->path_origin) || (dep_org_addr == addr)){
					return err;
				}
				
				if(p_entry){
					if(!forwarding_tbl_dependent_delete(addr, p_entry->dependent_target)){
						dependent_change = 1;
					}
				}
			}
		}
		
		if(dependent_change){
			model_sig_g_df_sbr_cfg.df_cfg.fixed_fwd_tbl[key_offset].update_id++;
			mesh_common_store(FLASH_ADR_MD_DF_SBR);
		}
	}
	forwarding_tbl_status.path_origin = p_tbl_delete->path_origin;
	forwarding_tbl_status.destination = p_tbl_delete->destination;	
	
	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&forwarding_tbl_status),sizeof(forwarding_tbl_status),cb_par->adr_src);
	return err;
}

int get_dependent_list_num(path_addr_t *p_list)
{
	int num = 0;
	foreach(i, MAX_DEPENDENT_NUM){
		if(p_list[i].addr){
			num++;
		}
	}
	return num;
}

int get_dependent_addr_range_list(u8 *addr_list, path_addr_t *p_dependent_list, u16 start_offset)
{
	int len = 0;
	foreach_range(i, start_offset, MAX_DEPENDENT_NUM){
		if(p_dependent_list[i].addr){
			addr_range_t *p_addr_range = (addr_range_t *)&addr_list[len];
			len += 2;
			p_addr_range->range_start_l = p_dependent_list[i].addr;
			if(p_dependent_list[i].snd_ele_cnt){
				len += 1;
				p_addr_range->length_present_l = 1;
				p_addr_range->range_length = p_dependent_list[i].snd_ele_cnt + 1;
			}
		}
	}
	
	return len;
}

int mesh_cmd_sig_cfg_forwarding_tbl_dependents_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	forwarding_tbl_dependents_get_t *p_dependents_get = (forwarding_tbl_dependents_get_t *) par;
	static forwarding_tbl_dependents_get_sts_t dependengts_get_sts;
	path_entry_com_t *p_fwd_entry=0;

	if(!is_unicast_adr(p_dependents_get->path_origin) || (ADR_UNASSIGNED == p_dependents_get->destination) || p_dependents_get->prohibited ||
		!(p_dependents_get->path_origin_mask || p_dependents_get->path_target_mask) || is_fixed_group(p_dependents_get->path_origin) || is_fixed_group(p_dependents_get->destination)){
		return err;
	}
	
	u32 cur_par_len = OFFSETOF(forwarding_tbl_dependents_get_sts_t, range_list);
	u8 identifier_exist = (par_len >= sizeof(forwarding_tbl_dependents_get_t));	
	u8 via_par_len = 0;
	memset(&dependengts_get_sts, 0x00, sizeof(dependengts_get_sts));
	dependengts_get_sts.status = ST_SUCCESS;
	dependengts_get_sts.netkey_index = p_dependents_get->netkey_index;
	memcpy(&dependengts_get_sts.status+1, p_dependents_get, OFFSETOF(forwarding_tbl_dependents_get_t, up_id));

	int key_offset = get_mesh_net_key_offset(p_dependents_get->netkey_index);		
	u16 fwd_tbl_id = model_sig_g_df_sbr_cfg.df_cfg.fixed_fwd_tbl[key_offset].update_id;//p_dependents_get->fixed_path_flag ? model_sig_g_df_sbr_cfg.df_cfg.fixed_fwd_tbl[key_offset].update_id:non_fixed_fwd_tbl[key_offset].update_id;
	if(-1 == key_offset){
		dependengts_get_sts.status = ST_INVALID_NETKEY;
		cur_par_len = OFFSETOF(forwarding_tbl_dependents_get_sts_t, up_id);
	}
	else if(identifier_exist && (p_dependents_get->up_id != fwd_tbl_id)){
		dependengts_get_sts.status = ST_OBSOLETE_INFO;
		cur_par_len = OFFSETOF(forwarding_tbl_dependents_get_sts_t, dependent_origion_size);
	}

	if(ST_SUCCESS == dependengts_get_sts.status){
		if(p_dependents_get->fixed_path_flag){
			p_fwd_entry = get_fixed_path_entry(key_offset, p_dependents_get->path_origin, p_dependents_get->destination);
		}
		else{
			non_fixed_entry_t *p_non_fixed_entry = get_non_fixed_path_entry(key_offset, p_dependents_get->path_origin, p_dependents_get->destination);
			p_fwd_entry = &p_non_fixed_entry->entry;			
		}	
		
		if(p_fwd_entry){			
			u8 origin_num = p_dependents_get->path_origin_mask ? get_dependent_list_num(p_fwd_entry->dependent_origin):0;			
			u8 target_num = p_dependents_get->path_target_mask ? get_dependent_list_num(p_fwd_entry->dependent_target):0;
			
			u8 total_dependent_num = origin_num + target_num;
			u16 origin_start_index = 0;
			u16 target_start_index = 0;
			if(p_dependents_get->start_index){
				if(p_dependents_get->start_index < total_dependent_num){
					if(p_dependents_get->start_index < origin_num){
						origin_num -= p_dependents_get->start_index;	
						origin_start_index = p_dependents_get->start_index;
					}
					else{						
						target_num = total_dependent_num - p_dependents_get->start_index;
						target_start_index = p_dependents_get->start_index - origin_num;
						origin_num = 0;
					}
				}
				else{
					origin_num = target_num = 0;
				}
			}

			if(origin_num){
				via_par_len = get_dependent_addr_range_list(&dependengts_get_sts.range_list[0], p_fwd_entry->dependent_origin, origin_start_index);
			}

			if(target_num){
				via_par_len += get_dependent_addr_range_list(&dependengts_get_sts.range_list[via_par_len], p_fwd_entry->dependent_target, target_start_index);
			}
			
			dependengts_get_sts.dependent_origion_size = origin_num;
			dependengts_get_sts.dependent_target_size = target_num;
			dependengts_get_sts.up_id = fwd_tbl_id;
		}					
	}

	cur_par_len += via_par_len;
	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&dependengts_get_sts),cur_par_len,cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_forwarding_tbl_entries_count_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	forwarding_tbl_entries_count_st_t entries_count_sts;	
	u16 netkey_index = par[0] + (par[1]<<8);
	memset(&entries_count_sts, 0x00, sizeof(entries_count_sts));
	entries_count_sts.status = ST_SUCCESS;
	entries_count_sts.netkey_index = netkey_index;
	if(is_netkey_index_prohibited(netkey_index)){
		return err;
	}
	
	int key_offset = get_mesh_net_key_offset(netkey_index);	
	if(-1 != key_offset){
		foreach(path_offset, MAX_NON_FIXED_PATH){
			path_entry_com_t *p_fwd_entry = &non_fixed_fwd_tbl[key_offset].path[path_offset].entry;
			if(p_fwd_entry->path_origin){
				entries_count_sts.non_fixed_cnt++;
			}
		}

		foreach(path_offset, MAX_FIXED_PATH){
			path_entry_com_t *p_fwd_entry = &model_sig_g_df_sbr_cfg.df_cfg.fixed_fwd_tbl[key_offset].path[path_offset];
			if(p_fwd_entry->path_origin){
				entries_count_sts.fixed_cnt++;				
			}
		}
		entries_count_sts.update_id = model_sig_g_df_sbr_cfg.df_cfg.fixed_fwd_tbl[key_offset].update_id;
	}
	else{
		entries_count_sts.status = ST_INVALID_NETKEY;
	}

	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&entries_count_sts),(ST_SUCCESS==entries_count_sts.status) ? sizeof(entries_count_sts):OFFSETOF(forwarding_tbl_entries_count_st_t, update_id),cb_par->adr_src);
	return err;
}

u16 mesh_fill_fwd_tbl_entry_list(int key_offset, path_entry_com_t *p_entry, u8 *p_list, u8 fix_path)
{
	u16 via_len = 0;
	forwarding_table_entry_head_t *p_head = (forwarding_table_entry_head_t *)p_list;
	memset(p_head, 0x00, sizeof(forwarding_table_entry_head_t));
	p_head->fixed_path_flag = fix_path;
	p_head->backward_path_validated_flag = p_entry->backward_path_validated;						
	via_len += sizeof(forwarding_table_entry_head_t);

	if(!fix_path){
		non_fixed_entry_t *p_non_fixed_entry = GET_NON_FIXED_ENTRY_POINT((u8*) p_entry);
		p_list[via_len++] = p_non_fixed_entry->state.lane_counter;
		u16 remaining_time = (GET_PATH_LIFETIME_MS(model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[key_offset].path_metric.path_lifetime) - (clock_time_ms()-p_non_fixed_entry->state.lifetime_ms))/1000/60;
		memcpy(p_list+via_len, &remaining_time, 2);
		via_len +=2;
		p_list[via_len++] = p_non_fixed_entry->state.forwarding_number;
	}
	
	addr_range_t *p_origin = (addr_range_t *)(p_list+via_len);
	p_origin->range_start_l = p_entry->path_origin;
	if(p_entry->path_origin_snd_ele_cnt){
		p_origin->length_present_l = 1;
		p_origin->range_length = p_entry->path_origin_snd_ele_cnt+1;
		via_len += 3;
	}
	else{						
		via_len += 2;
	}

	u8 dependent_num = get_dependent_list_num(p_entry->dependent_origin);
	if(dependent_num){
		p_head->dependent_origin_list_size_indicator = DEPENDENT_LIST_SIZE_ONE;
		p_list[via_len] = dependent_num;
		via_len += DEPENDENT_LIST_SIZE_ONE;
	}

	if(p_entry->bearer_toward_path_origin){
		p_head->bearer_toward_path_origin_indicator = 1;
		memcpy(p_list+via_len, &p_entry->bearer_toward_path_origin, 2);
		via_len += 2;
	}

	addr_range_t *p_target = (addr_range_t *)(p_list+via_len);
	if(is_unicast_adr(p_entry->destination)){
		p_head->unicast_destination_flag = 1;
		p_target->range_start_l = p_entry->destination;
		via_len += 2;
		if(p_entry->path_target_snd_ele_cnt){
			p_target->length_present_l = 1;
			p_target->range_length = p_entry->path_target_snd_ele_cnt + 1;
			via_len += 1;
		}
	}
	else{
		p_target->multicast_addr = p_entry->destination;
		via_len += 2;
	}				

	dependent_num = get_dependent_list_num(p_entry->dependent_target);
	if(dependent_num){
		p_head->dependent_target_list_size_indicator = DEPENDENT_LIST_SIZE_ONE;
		p_list[via_len] = dependent_num;
		via_len += DEPENDENT_LIST_SIZE_ONE;
	}

	if(p_entry->bearer_toward_path_target){
		p_head->bearer_toward_path_target_indicator = 1;
		memcpy(p_list+via_len, &p_entry->bearer_toward_path_target, 2);
		via_len += 2;
	}	
	return via_len;
}

int mesh_cmd_sig_cfg_forwarding_tbl_entries_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	forwarding_tbl_entries_get_t *p_entries_get = (forwarding_tbl_entries_get_t *)par;
	forwarding_tbl_entries_st_t entries_sts;		
	u16 netkey_index = p_entries_get->netkey_index;
	u16 path_origin = 0;
	u16 destination = 0;
	u16 par_offset = 0;	
	u16 update_id = 0;
	u8 update_id_existed = 0;
	if(0 == p_entries_get->filter_mask){
		return err;	
	}
	
	if(p_entries_get->filter_mask & ENTRIES_GET_PATH_ORIGIN_MATCH){
		path_origin = p_entries_get->par[par_offset] + (p_entries_get->par[par_offset+1]<<8);
		par_offset += 2;
		if(!is_unicast_adr(path_origin)){
			return err;
		}
	}

	if(p_entries_get->filter_mask & ENTRIES_GET_DST_MATCH){
		destination = p_entries_get->par[par_offset] + (p_entries_get->par[par_offset+1]<<8);
		par_offset += 2;
		if((ADR_UNASSIGNED == destination) || is_fixed_group(destination)){
			return err;
		}
	}

	memset(&entries_sts, 0x00, sizeof(entries_sts));
	entries_sts.status = ST_SUCCESS;
	entries_sts.netkey_index = netkey_index;
	entries_sts.filter_mask = p_entries_get->filter_mask;
	entries_sts.start_index = p_entries_get->start_index;
	memcpy(entries_sts.par, p_entries_get->par, par_offset);
	int key_offset = get_mesh_net_key_offset(netkey_index);	

	update_id_existed = (par_len>OFFSETOF(forwarding_tbl_entries_get_t, par) + par_offset);	
	u16 fwd_tbl_id = model_sig_g_df_sbr_cfg.df_cfg.fixed_fwd_tbl[key_offset].update_id;
	memcpy(entries_sts.par+par_offset, &fwd_tbl_id, 2);
	par_offset += 2;
	if(-1 == key_offset){
		entries_sts.status = ST_INVALID_NETKEY;
		par_offset -= 2;
	}
	else if(update_id_existed){
		update_id = p_entries_get->par[0] + (p_entries_get->par[1]<<8);
		if(update_id != fwd_tbl_id ){
			entries_sts.status = ST_OBSOLETE_INFO;
		}
	}

	if(ST_SUCCESS == entries_sts.status){
		u16 valid_index = 0;		
		
		for(u8 mask=0; mask<2; mask++){
			if(!(p_entries_get->filter_mask & BIT(mask))){
				continue;
			}
			u8 is_fix_path = (0 == mask);
			u16 max_paths = is_fix_path ? MAX_FIXED_PATH:MAX_NON_FIXED_PATH;
			u16 entry_length = is_fix_path ? sizeof(fixed_path_st_t):sizeof(non_fixed_path_st_t);							
			for(u16 path_offset=0; path_offset<max_paths; path_offset++){	
				if(par_offset+entry_length > MAX_ENTRY_STS_LEN){// buf not enough
					break;
				}
				
				path_entry_com_t *p_entry = is_fix_path ? &model_sig_g_df_sbr_cfg.df_cfg.fixed_fwd_tbl[key_offset].path[path_offset]:&non_fixed_fwd_tbl[key_offset].path[path_offset].entry;
				
				if((!p_entry->path_origin) || (valid_index < p_entries_get->start_index)){
					if(p_entry->path_origin){
						valid_index++;
					}
					continue;
				}				
				
				if(((p_entries_get->filter_mask&ENTRIES_GET_PATH_ORIGIN_MATCH) && (path_origin != p_entry->path_origin)) ||
					((p_entries_get->filter_mask&ENTRIES_GET_DST_MATCH) && (destination != p_entry->destination))){
					continue;
				}
									
				u16 par_len = mesh_fill_fwd_tbl_entry_list(key_offset, p_entry, entries_sts.par+par_offset, is_fix_path);
				par_offset += par_len;	
			}

			if(update_id_existed){// update id existed
				model_sig_g_df_sbr_cfg.df_cfg.fixed_fwd_tbl[key_offset].update_id = update_id;					
				mesh_common_store(FLASH_ADR_MD_DF_SBR);
			}
		}
	}

	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp, (u8 *)(&entries_sts), OFFSETOF(forwarding_tbl_entries_st_t, par)+par_offset, cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_wanted_lanes_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	wanted_lanes_sts_t wanted_lanes_sts;	
	u16 netkey_index = par[0] + (par[1]<<8);
	wanted_lanes_sts.status = ST_SUCCESS;
	wanted_lanes_sts.netkey_index = netkey_index;
	if(is_netkey_index_prohibited(netkey_index)){
		return err;
	}
	
	int key_offset = get_mesh_net_key_offset(netkey_index);	
	if(-1 != key_offset){
		wanted_lanes_sts.wanted_lanes = model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[key_offset].wanted_lanes;
	}
	else{
		wanted_lanes_sts.status = ST_INVALID_NETKEY;
		wanted_lanes_sts.wanted_lanes = 0;
	}

	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&wanted_lanes_sts),sizeof(wanted_lanes_sts),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_wanted_lanes_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	wanted_lanes_sts_t wanted_lanes_sts;
	wanted_lanes_set_t *p_lanes_set = (wanted_lanes_set_t *)par;
	memset(&wanted_lanes_sts, 0x00, sizeof(wanted_lanes_sts));
	wanted_lanes_sts.status = ST_SUCCESS;
	wanted_lanes_sts.netkey_index = p_lanes_set->netkey_index;
	wanted_lanes_sts.wanted_lanes = p_lanes_set->wanted_lanes;
	if(is_netkey_index_prohibited(p_lanes_set->netkey_index) || !p_lanes_set->wanted_lanes){
		return err;
	}
	
	int key_offset = get_mesh_net_key_offset(p_lanes_set->netkey_index);	
	if((-1 == key_offset)){
		wanted_lanes_sts.status = ST_INVALID_NETKEY;
	}
	else{
		model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[key_offset].wanted_lanes = p_lanes_set->wanted_lanes;
		mesh_common_store(FLASH_ADR_MD_DF_SBR);
	}

	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&wanted_lanes_sts),sizeof(wanted_lanes_sts_t),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_two_way_path_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	two_way_path_sts_t two_way_path_sts;	
	u16 netkey_index = par[0] + (par[1]<<8);
	memset(&two_way_path_sts, 0, sizeof(two_way_path_sts));
	two_way_path_sts.status = ST_SUCCESS;
	two_way_path_sts.netkey_index = netkey_index;

	if(is_netkey_index_prohibited(netkey_index)){
		return err;
	}
	
	int key_offset = get_mesh_net_key_offset(netkey_index);
	if(-1 != key_offset){
		two_way_path_sts.two_way_path = model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[key_offset].two_way_path;
	}
	else{
		two_way_path_sts.status = ST_INVALID_NETKEY;
	}

	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&two_way_path_sts),sizeof(two_way_path_sts),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_two_way_path_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	two_way_path_sts_t two_way_path_sts;	
	two_way_path_set_t *p_set = (two_way_path_set_t*)par;
	memset(&two_way_path_sts, 0, sizeof(two_way_path_sts));
	two_way_path_sts.status = ST_SUCCESS;
	two_way_path_sts.netkey_index = p_set->netkey_index;
	two_way_path_sts.two_way_path = p_set->two_way_path;

	if(p_set->Prohibited || is_netkey_index_prohibited(p_set->netkey_index)){
		return err;
	}
	
	int key_offset = get_mesh_net_key_offset(p_set->netkey_index);	
	if((-1 != key_offset)){
		model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[key_offset].two_way_path = p_set->two_way_path;
		mesh_common_store(FLASH_ADR_MD_DF_SBR);
	}
	else{
		two_way_path_sts.status = ST_INVALID_NETKEY;
	}
	
	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&two_way_path_sts),sizeof(two_way_path_sts),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_path_echo_interval_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	path_echo_interval_sts_t path_echo_interval_sts;	
	u16 netkey_index = par[0] + (par[1]<<8);
	memset(&path_echo_interval_sts, 0, sizeof(path_echo_interval_sts));
	path_echo_interval_sts.status = ST_SUCCESS;
	path_echo_interval_sts.netkey_index = netkey_index;

	if(is_netkey_index_prohibited(netkey_index)){
		return err;
	}
	
	int key_offset = get_mesh_net_key_offset(netkey_index);	
	if(-1 != key_offset){
		path_echo_interval_sts.unicast_echo_interval = model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[key_offset].unicast_echo_interval;
		path_echo_interval_sts.multicast_echo_interval = model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[key_offset].multicast_echo_interval;
	}
	else{
		path_echo_interval_sts.status = ST_INVALID_NETKEY;
	}

	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&path_echo_interval_sts),sizeof(path_echo_interval_sts),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_path_echo_interval_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	path_echo_interval_sts_t path_echo_interval_sts;	
	path_echo_interval_set_t *p_set = (path_echo_interval_set_t*)par;
	memset(&path_echo_interval_sts, 0, sizeof(path_echo_interval_sts));
	path_echo_interval_sts.status = ST_SUCCESS;
	memcpy(&path_echo_interval_sts.netkey_index, p_set, sizeof(path_echo_interval_set_t));

	if(is_netkey_index_prohibited(p_set->netkey_index)){
		return err;
	}
	
	int key_offset = get_mesh_net_key_offset(p_set->netkey_index);	
	if((-1 != key_offset)){
		if(p_set->unicast_echo_interval <= ECHO_INVL_VALID){
			model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[key_offset].unicast_echo_interval = p_set->unicast_echo_interval;
		}
		else if(p_set->unicast_echo_interval != ECHO_INVL_NOT_CHANGE){
			return err;
		}
		else if(p_set->multicast_echo_interval == ECHO_INVL_NOT_CHANGE){
			return err;
		}
		if(p_set->multicast_echo_interval <= ECHO_INVL_VALID){
			model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[key_offset].multicast_echo_interval = p_set->multicast_echo_interval;
		}
		else if(p_set->multicast_echo_interval != ECHO_INVL_NOT_CHANGE){
			return err;
		}
		
		path_echo_interval_sts.unicast_echo_interval = model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[key_offset].unicast_echo_interval;
		path_echo_interval_sts.multicast_echo_interval = model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[key_offset].multicast_echo_interval;
		mesh_common_store(FLASH_ADR_MD_DF_SBR);
	}
	else{
		path_echo_interval_sts.status = ST_INVALID_NETKEY;
	}
	
	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&path_echo_interval_sts),sizeof(path_echo_interval_sts),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_directed_network_transmit_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	mesh_transmit_t transmit;	

	transmit.val = model_sig_g_df_sbr_cfg.df_cfg.directed_forward.transmit.val;
	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&transmit),sizeof(transmit),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_directed_network_transmit_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	mesh_transmit_t transmit;	
	
	model_sig_g_df_sbr_cfg.df_cfg.directed_forward.transmit.val = par[0];
	transmit.val = model_sig_g_df_sbr_cfg.df_cfg.directed_forward.transmit.val;
	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&transmit),sizeof(transmit),cb_par->adr_src);
	mesh_common_store(FLASH_ADR_MD_DF_SBR);
	return err;
}

int mesh_cmd_sig_cfg_directed_relay_retransmit_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	mesh_transmit_t transmit;	

	transmit.val = model_sig_g_df_sbr_cfg.df_cfg.directed_forward.relay_transmit.val;
	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&transmit),sizeof(transmit),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_directed_relay_retransmit_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	mesh_transmit_t transmit;	
	
	model_sig_g_df_sbr_cfg.df_cfg.directed_forward.relay_transmit.val = par[0];
	transmit.val = model_sig_g_df_sbr_cfg.df_cfg.directed_forward.relay_transmit.val;
	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&transmit),sizeof(transmit),cb_par->adr_src);
	mesh_common_store(FLASH_ADR_MD_DF_SBR);
	return err;
}

int mesh_cmd_sig_cfg_rssi_threshold_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	rssi_threshold_t sts;	

	sts = model_sig_g_df_sbr_cfg.df_cfg.directed_forward.rssi_threshold;
	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&sts),sizeof(sts),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_rssi_threshold_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	rssi_threshold_t sts;	
	if(par[0] < RSSI_MARGIN_PROHIBITED){	
		model_sig_g_df_sbr_cfg.df_cfg.directed_forward.rssi_threshold.rssi_margin = par[0];
		mesh_common_store(FLASH_ADR_MD_DF_SBR);
	}else{
		return err;
	}
	sts = model_sig_g_df_sbr_cfg.df_cfg.directed_forward.rssi_threshold;
	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&sts),sizeof(sts),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_directed_paths_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	directed_paths_t sts;	
	
	sts = model_sig_g_df_sbr_cfg.df_cfg.directed_forward.directed_paths;
	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&sts),sizeof(sts),cb_par->adr_src);
	return err;
}

u8 mesh_model_publish_policy_get(u16 ele_adr, u32 model_id, bool4 sig_model, u16 adr_src)
{
    u8 st = ST_SUCCESS;
    u8 model_idx = 0;
    model_common_t *p_model = (model_common_t *)mesh_find_ele_resource_in_model(ele_adr, model_id, sig_model, &model_idx, 0);
    directed_pub_policy_st_t pub_oplicy_st;
	
	pub_oplicy_st.directed_pub_policy = p_model?p_model->directed_pub_policy:0;
	pub_oplicy_st.ele_addr = ele_adr;
	pub_oplicy_st.model_id = model_id;
	
	if(!is_own_ele(ele_adr)){
		st = ST_INVALID_ADR;
	}
	
    else if(!p_model){   // response error status after this function.
    	st = ST_INVALID_MODEL;
    }else if(p_model->no_pub){
    	st = ST_INVALID_MODEL;	// model not support publish
    }
	
    pub_oplicy_st.status = st;

    return mesh_tx_cmd_rsp_cfg_model(DIRECTED_PUBLISH_POLICY_STATUS, (u8 *)&pub_oplicy_st, sizeof(pub_oplicy_st)-FIX_SIZE(sig_model), adr_src);
}

int mesh_cmd_sig_cfg_directed_publish_policy_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err = -1;
	u8 st = ST_SUCCESS;
	directed_pub_policy_st_t pub_oplicy_st;	
	memset(&pub_oplicy_st, 0x00, sizeof(pub_oplicy_st));
    bool4 sig_model = (sizeof(directed_pub_policy_get_t) - 2 == par_len);
    directed_pub_policy_get_t *p_policy_get = (directed_pub_policy_get_t *)par;
    u32 model_id = sig_model ? (p_policy_get->model_id & 0xFFFF) : p_policy_get->model_id;

	pub_oplicy_st.ele_addr = p_policy_get->ele_addr;
	pub_oplicy_st.model_id = model_id;

	if(!is_unicast_adr(p_policy_get->ele_addr) || (U32_MAX == model_id)){
		return err;
	}
	
    if(is_own_ele(p_policy_get->ele_addr)){
		u8 model_idx;
		model_common_t *p_model = (model_common_t *)mesh_find_ele_resource_in_model(p_policy_get->ele_addr, model_id, sig_model, &model_idx, 0);
		if(!p_model || p_model->no_pub){
	    	st = ST_INVALID_MODEL;
	    }else{
			pub_oplicy_st.directed_pub_policy = p_model->directed_pub_policy;
		}

    }
	else{
		st = ST_INVALID_ADR;
	}
	pub_oplicy_st.status = st;
	
	return mesh_tx_cmd_rsp_cfg_model(DIRECTED_PUBLISH_POLICY_STATUS, (u8 *)&pub_oplicy_st, sizeof(pub_oplicy_st)-FIX_SIZE(sig_model), cb_par->adr_src);
}

u8 mesh_pub_policy_search_and_set(u16 ele_adr, u8 *par_set, u32 model_id, bool4 sig_model)
{
    u8 st = ST_UNSPEC_ERR;
	directed_pub_policy_set_t *p_policy_set = (directed_pub_policy_set_t *)par_set;
    mesh_adr_list_t adr_list;
    st = find_ele_support_model_and_match_dst(&adr_list, ele_adr, model_id, sig_model);
	if(st != ST_SUCCESS) return st;
	
	int change_falg = 0;
    if(adr_list.adr_cnt){
        u8 model_idx = 0;
        model_common_t *p_model = (model_common_t *)mesh_find_ele_resource_in_model(ele_adr, model_id, sig_model,&model_idx, 0);
        if(p_model && (!is_use_device_key(model_id, sig_model))){
        	st = ST_SUCCESS;
			if(p_model->directed_pub_policy != p_policy_set->directed_pub_policy){
				change_falg = 1;
			}
        	p_model->directed_pub_policy = p_policy_set->directed_pub_policy;
        }else{
			st = ST_INVALID_PUB_PAR;
        }
    }

    if((ST_SUCCESS == st) && change_falg){
		mesh_model_store(sig_model, model_id);
    }

	return st;
}

int mesh_cmd_sig_cfg_directed_publish_policy_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err = 0;
	u8 st = ST_SUCCESS;
	directed_pub_policy_st_t pub_oplicy_st;	
	memset(&pub_oplicy_st, 0x00, sizeof(pub_oplicy_st));
	
	bool4 sig_model = (sizeof(directed_pub_policy_set_t) - 2 == par_len);
    directed_pub_policy_set_t *p_policy_set = (directed_pub_policy_set_t *)par;
    u32 model_id = sig_model ? (p_policy_set->model_id & 0xFFFF) : p_policy_set->model_id;
	
	u16 ele_adr = p_policy_set->ele_addr;
	if(!is_unicast_adr(ele_adr) || (U32_MAX == model_id)){
		return err;
	}
	
    if(is_own_ele(ele_adr)){
        st = mesh_pub_policy_search_and_set(ele_adr, (u8 *)p_policy_set, model_id, sig_model);        
    }else{
    	st = ST_INVALID_ADR;
    }

	pub_oplicy_st.status = st;
	pub_oplicy_st.directed_pub_policy = p_policy_set->directed_pub_policy;
	pub_oplicy_st.ele_addr = p_policy_set->ele_addr;
	pub_oplicy_st.model_id = model_id;

	return mesh_tx_cmd_rsp_cfg_model(DIRECTED_PUBLISH_POLICY_STATUS, (u8 *)&pub_oplicy_st, sizeof(pub_oplicy_st)-FIX_SIZE(sig_model), cb_par->adr_src);
}

int mesh_cmd_sig_cfg_path_discovery_timing_control_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	discovery_timing_t sts;
	
	sts = model_sig_g_df_sbr_cfg.df_cfg.directed_forward.discovery_timing;
	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&sts),sizeof(sts),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_path_discovery_timing_control_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	discovery_timing_t sts;
	discovery_timing_t *p_dsc_set = (discovery_timing_t *) par;
	
	if(p_dsc_set->prohibited){
		return err;
	}
	memcpy(&sts, par, sizeof(discovery_timing_t));
	sts.prohibited = 0;
	model_sig_g_df_sbr_cfg.df_cfg.directed_forward.discovery_timing = sts;

	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&sts),sizeof(sts),cb_par->adr_src);
	mesh_common_store(FLASH_ADR_MD_DF_SBR);
	return err;
}

int mesh_cmd_sig_cfg_directed_control_network_transmit_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	mesh_transmit_t transmit;	

	transmit.val = model_sig_g_df_sbr_cfg.df_cfg.directed_forward.control_transmit.val;
	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&transmit),sizeof(transmit),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_directed_control_network_transmit_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	mesh_transmit_t transmit;	
	
	model_sig_g_df_sbr_cfg.df_cfg.directed_forward.control_transmit.val = par[0];
	transmit.val = model_sig_g_df_sbr_cfg.df_cfg.directed_forward.control_transmit.val;
	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&transmit),sizeof(transmit),cb_par->adr_src);
	mesh_common_store(FLASH_ADR_MD_DF_SBR);
	return err;
}

int mesh_cmd_sig_cfg_directed_control_relay_transmit_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	mesh_transmit_t transmit;	

	transmit.val = model_sig_g_df_sbr_cfg.df_cfg.directed_forward.control_relay_transmit.val;
	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&transmit),sizeof(transmit),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_directed_control_relay_transmit_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	mesh_transmit_t transmit;	
	
	model_sig_g_df_sbr_cfg.df_cfg.directed_forward.control_relay_transmit.val = par[0];
	transmit.val = model_sig_g_df_sbr_cfg.df_cfg.directed_forward.control_relay_transmit.val;
	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&transmit),sizeof(transmit),cb_par->adr_src);
	mesh_common_store(FLASH_ADR_MD_DF_SBR);
	return err;
}
//-------------------------forwarding config message end-----------------------------------//

//---------------------------directed forwarding procedures-------------------//
int path_request_msg_set(u16 netkey_offset, discovery_entry_par_t * p_dsc_entry, mesh_ctl_path_req_t * p_path_req)
{
	u8 via_par_len = 0;
	memset(p_path_req, 0x00, sizeof(mesh_ctl_path_req_t));
	
	p_path_req->path_metric_type = p_dsc_entry->entry.path_metric_type;
	p_path_req->path_lifetime = p_dsc_entry->entry.path_lifetime;
	p_path_req->discovery_interval = p_dsc_entry->entry.path_discovery_interval;
	p_path_req->forwarding_number = p_dsc_entry->entry.forwarding_number;
	if(is_own_ele(p_dsc_entry->entry.path_origin.addr)){
		p_path_req->origin_path_metric = 0;
	}
	else{
		non_fixed_entry_t *p_fwd_entry = get_forwarding_entry_correspond2_discovery_entry(netkey_offset, &p_dsc_entry->entry);
		p_path_req->origin_path_metric = p_dsc_entry->entry.path_metric + 1 + (p_fwd_entry?p_fwd_entry->state.lane_counter:0);
	}
	p_path_req->destination = p_dsc_entry->entry.destination;	
	
	addr_range_t *p_addr_range = (addr_range_t *)p_path_req->addr_par; // path origin
	p_addr_range->range_start_b = p_dsc_entry->entry.path_origin.addr;	
	if(p_dsc_entry->entry.path_origin.snd_ele_cnt){
		p_addr_range->length_present_b = 1;		
		p_addr_range->range_length = p_dsc_entry->entry.path_origin.snd_ele_cnt + 1;
		via_par_len += 3;
	}
	else{
		via_par_len += 2;
	}
	
	if(p_dsc_entry->entry.dependent_origin.addr){// path dependent
		p_addr_range = (addr_range_t *)(p_path_req->addr_par + via_par_len); 
		p_path_req->on_behalf_of_dependent_origin = 1;
		p_addr_range->range_start_b = p_dsc_entry->entry.dependent_origin.addr;
		if(p_dsc_entry->entry.dependent_origin.snd_ele_cnt){
			p_addr_range->length_present_b = 1;
			p_addr_range->range_length = p_dsc_entry->entry.dependent_origin.snd_ele_cnt + 1;
			via_par_len += 3;
		}
		else{
			via_par_len += 2;
		}
	}
	return (OFFSETOF(mesh_ctl_path_req_t, addr_par)+via_par_len);
}

int prepare_and_send_path_reply(u16 netkey_offset, discovery_entry_par_t *p_dsc_entry, u8 confirm_req)
{
	mesh_ctl_path_reply_t path_reply;
	u8 via_par_len = 0;
	memset(&path_reply, 0x00, sizeof(mesh_ctl_path_reply_t));
	non_fixed_entry_t *p_fwd_entry = get_forwarding_entry_correspond2_discovery_entry(netkey_offset, &p_dsc_entry->entry);
	if(!p_fwd_entry){
		return -1;
	}

	path_reply.confirmation_request = confirm_req;
	if(is_unicast_adr(p_dsc_entry->entry.destination)){
		addr_range_t *p_path_target = (addr_range_t *)path_reply.addr_par;
		path_reply.unicast_destination = 1;
		via_par_len += 2;
		if(is_path_target(p_dsc_entry->entry.path_origin.addr, p_dsc_entry->entry.destination)){
			path_reply.confirmation_request = model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[netkey_offset].two_way_path;
			p_path_target->range_start_b = ele_adr_primary;
			if(g_ele_cnt > 1){
				p_path_target->length_present_b = 1;
				p_path_target->range_length = g_ele_cnt;
				via_par_len += 1;
			}
		}
		else{
			p_path_target->range_start_b = p_fwd_entry->entry.destination;
			if(p_fwd_entry->entry.path_target_snd_ele_cnt){			
				p_path_target->length_present_b = 1;
				p_path_target->range_length = p_fwd_entry->entry.path_target_snd_ele_cnt + 1;
				via_par_len += 1;
			}
		}

		if(p_dsc_entry->entry.destination != p_fwd_entry->entry.destination){
			path_reply.on_behalf_of_dependent_target = 1;	
			addr_range_t *p_dependent_target = (addr_range_t *)(path_reply.addr_par + via_par_len);
			p_dependent_target->range_start_b = p_dsc_entry->entry.destination;
			via_par_len += 2;
			int offset = get_offset_in_dependent_list(p_fwd_entry->entry.dependent_target, p_dsc_entry->entry.destination);
			if(-1 != offset){
				if(p_fwd_entry->entry.dependent_target[offset].snd_ele_cnt){
					via_par_len += 1;
					p_dependent_target->length_present_b = 1;
					p_dependent_target->range_length = p_fwd_entry->entry.dependent_target[offset].snd_ele_cnt+1;					
				}
			}
		}	
	}
	
	path_reply.path_origin = p_dsc_entry->entry.path_origin.addr;
	path_reply.forwarding_number = p_dsc_entry->entry.forwarding_number;

	return mesh_tx_cmd_layer_upper_ctl_primary_specified_key(CMD_CTL_PATH_REPLY, (u8 *)&path_reply, OFFSETOF(mesh_ctl_path_reply_t, addr_par)+via_par_len, p_dsc_entry->entry.next_toward_path_origin, netkey_offset);
}

int cfg_cmd_send_path_request(mesh_ctl_path_req_t *p_path_req, u8 len, u16 netkey_offset)
{
	return mesh_tx_cmd_layer_upper_ctl_primary_specified_key(CMD_CTL_PATH_REQUEST, (u8 *)p_path_req, len, ADR_ALL_DIRECTED_FORWARD, netkey_offset);
}

int cfg_cmd_send_path_echo_request(non_fixed_entry_t * p_fwd_entry)
{
	start_path_echo_reply_timeout_timer(p_fwd_entry);
	return mesh_tx_cmd_layer_upper_ctl(CMD_CTL_PATH_ECHO_REQUEST, 0, 0, ele_adr_primary, p_fwd_entry->entry.destination, 0);
}

int cfg_cmd_send_path_echo_reply(non_fixed_entry_t * p_fwd_entry)
{
	return mesh_tx_cmd_layer_upper_ctl(CMD_CTL_PATH_ECHO_REPLY, (u8 *)&p_fwd_entry->entry.destination, 2, ele_adr_primary, p_fwd_entry->entry.path_origin, 0);
}

int path_discovery_by_discovery_entry(u16 netkey_offset, discovery_entry_par_t *p_dsc_entry){
	mesh_ctl_path_req_t path_req;
	if(is_own_ele(p_dsc_entry->entry.path_origin.addr)){
		p_dsc_entry->entry.forwarding_number = discovery_table[netkey_offset].forwarding_number;
		discovery_table[netkey_offset].forwarding_number++;	
	}
	p_dsc_entry->state.path_need = p_dsc_entry->state.path_pending = 0;
	u8 len = path_request_msg_set(netkey_offset, p_dsc_entry, &path_req);
	LOG_DF_DEBUG(0, 0, "%s target:0x%x", __func__, p_dsc_entry->entry.destination);
	return cfg_cmd_send_path_request(&path_req, len, netkey_offset);
}

int directed_forwarding_confirm_start(u16 netkey_offset, non_fixed_entry_t *p_fwd_entry)
{
	int err = -1;
	discovery_entry_par_t *p_dsc_entry = get_discovery_entry_correspond2_forwarding_entry(netkey_offset, p_fwd_entry);
	mesh_ctl_path_confirmation_t path_confirm;
	path_confirm.path_target = p_fwd_entry->entry.destination;
	path_confirm.path_origin = p_fwd_entry->entry.path_origin;
#ifndef WIN32
	mesh_tx_with_random_delay_ms = rand()%30;
#endif
	err =  mesh_tx_cmd_layer_upper_ctl(CMD_CTL_PATH_CONFIRMATION, (u8 *)&path_confirm, sizeof(mesh_ctl_path_confirmation_t), ele_adr_primary, ADR_ALL_DIRECTED_FORWARD, 0);
	if(0 == err){
		p_fwd_entry->entry.backward_path_validated = 1;
		p_dsc_entry->state.path_confirm_sent = 1;
	}
	return err;
}

int directed_forwarding_solication_start(u16 netkey_offset, mesh_ctl_path_request_solication_t *p_addr_list, u8 list_num)
{
	int err = -1;
	if(is_provision_success()){// to be done
		err =  mesh_tx_cmd_layer_upper_ctl_specified_key(CMD_CTL_PATH_REQUEST_SOLICITATION, (u8 *)p_addr_list, list_num<<1, ele_adr_primary, ADR_ALL_DIRECTED_FORWARD, 0, netkey_offset);
	}
	return err; 
}

int is_path_target(u16 origin, u16 destination)
{
	if( is_own_ele(destination)|| (is_subscription_adr(0, destination)) || mesh_mac_match_friend(destination) 
		|| mesh_group_match_friend(destination) || is_proxy_client_addr(destination) 
		#if MD_SBR_CFG_SERVER_EN
		|| is_subnet_bridge_addr(origin, destination)
		#endif
		){
		return 1;
	}
	return 0;
}


void forwarding_table_timing_proc(u16 netkey_offset)
{
 	foreach(fwd_tbl_offset, MAX_NON_FIXED_PATH){
		non_fixed_entry_t *p_fwd_entry = &non_fixed_fwd_tbl[netkey_offset].path[fwd_tbl_offset];
		if(p_fwd_entry->entry.path_origin && !p_fwd_entry->entry.fixed_path){
			discovery_timing_t *p_dsc_timing = &model_sig_g_df_sbr_cfg.df_cfg.directed_forward.discovery_timing;
			if(p_fwd_entry->state.lifetime_ms && clock_time_ms_expired(p_fwd_entry->state.lifetime_ms)){
				if(is_own_ele(p_fwd_entry->entry.path_origin)){
					if(clock_time_exceed_ms(p_fwd_entry->state.lifetime_ms, p_dsc_timing->path_monitoring_interval*1000)){
						stop_path_monitor(p_fwd_entry);
						if(p_fwd_entry->state.path_need){
							directed_forwarding_initial_start(netkey_offset, p_fwd_entry->entry.destination, 0, 0);
						} 
						delete_non_fixed_path(netkey_offset, &p_fwd_entry->entry);
					}
					else{
						start_path_monitor(p_fwd_entry);
					}
				}
				else{
					delete_non_fixed_path(netkey_offset, &p_fwd_entry->entry);
				}
			}

			int echo_interval = (is_unicast_adr(p_fwd_entry->entry.destination)?model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[netkey_offset].unicast_echo_interval:model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[netkey_offset].multicast_echo_interval);
			if((0 == echo_interval) && p_fwd_entry->state.path_echo_timer_ms){
				stop_path_echo_timer(p_fwd_entry);
			}			
			if(p_fwd_entry->state.path_echo_timer_ms && clock_time_ms_expired(p_fwd_entry->state.path_echo_timer_ms)){
				cfg_cmd_send_path_echo_request(p_fwd_entry); // Path Validation Started	
			}
			
			if(p_fwd_entry->state.path_echo_reply_timeout_timer && clock_time_ms_expired(p_fwd_entry->state.path_echo_reply_timeout_timer)){
				stop_path_echo_reply_timeout_timer(p_fwd_entry); //   Path Validation Failed
				directed_forwarding_initial_start(netkey_offset, p_fwd_entry->entry.destination, 0, 0);
				//path_discovery_by_forwarding_entry(netkey_offset, p_fwd_entry);
				if(p_fwd_entry){
					delete_non_fixed_path(netkey_offset, &p_fwd_entry->entry);
					model_sig_g_df_sbr_cfg.df_cfg.fixed_fwd_tbl[netkey_offset].update_id++;
				}
			}		
		}
	}
}

void discovery_table_timing_proc(u16 netkey_offset)
{
	foreach(dsc_tbl_offset, MAX_DSC_TBL){
		discovery_entry_par_t *p_dsc_entry = &discovery_table[netkey_offset].dsc_entry_par[dsc_tbl_offset];
		non_fixed_entry_t *p_fwd_entry = get_non_fixed_path_entry(netkey_offset, p_dsc_entry->entry.path_origin.addr, p_dsc_entry->entry.destination);
		if(p_dsc_entry->entry.destination){
			if(p_dsc_entry->state.discovery_timer && clock_time_expired(p_dsc_entry->state.discovery_timer)){
				LOG_DF_DEBUG(0, 0, "path discovery timer expired", 0);
				stop_path_discovery_timer(p_dsc_entry);						
				if(is_own_ele(p_dsc_entry->entry.path_origin.addr)){ // path origin
					if(p_fwd_entry){
						if(p_fwd_entry->entry.path_not_ready){
							p_fwd_entry->entry.path_not_ready = 0;
						}
						
						if(p_dsc_entry->state.new_lane_established){
							LOG_DF_DEBUG(0, 0, "new path lane establish counter:%d", p_fwd_entry->state.lane_counter);
							if(p_fwd_entry->state.lane_counter < model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[netkey_offset].wanted_lanes){
								LOG_DF_DEBUG(0, 0, "start discovery guard timer", 0);
								start_path_discovery_guard_timer(p_dsc_entry);// start discovery guard timer.
							}	
							else{
								directed_discovery_entry_remove(p_dsc_entry);// Path Discovery Succeeded
							}
						}
						else{
							directed_discovery_entry_remove(p_dsc_entry);							 
						}
					}
					else{
						#if DF_TEST_MODE_EN
						directed_discovery_entry_remove(p_dsc_entry); // always create path origin state machine
						#else
						start_path_discovery_retry_timer(p_dsc_entry); // remove dsc entry after retry timer expired						
						#endif
					}
				}
				else { // directed relay or path target
					directed_discovery_entry_remove(p_dsc_entry);
				}
			}

			if(p_dsc_entry->state.discovery_guard_timer && clock_time_expired(p_dsc_entry->state.discovery_guard_timer)){
				stop_path_discovery_guard_timer(p_dsc_entry); 
				start_path_discovery_timer(p_dsc_entry); 
				path_discovery_by_discovery_entry(netkey_offset, p_dsc_entry);
			}

			if(p_dsc_entry->state.discovery_retry_timer && clock_time_ms_expired(p_dsc_entry->state.discovery_retry_timer)){
				stop_path_discovery_retry_timer(p_dsc_entry); 
				if(p_dsc_entry->state.path_need){
					start_path_discovery_timer(p_dsc_entry); 
					path_discovery_by_discovery_entry(netkey_offset, p_dsc_entry);
				}
				else{
					directed_discovery_entry_remove(p_dsc_entry);
				}
			}
			
			if(p_dsc_entry->state.request_delay_timer && clock_time_expired(p_dsc_entry->state.request_delay_timer)){
				stop_path_request_delay_timer(p_dsc_entry);
				path_discovery_by_discovery_entry(netkey_offset, p_dsc_entry);
			}

			if(p_dsc_entry->state.reply_delay_timer && clock_time_expired(p_dsc_entry->state.reply_delay_timer)){							
				stop_path_reply_delay_timer(p_dsc_entry);
				p_fwd_entry = get_forwarding_entry_correspond2_discovery_entry(netkey_offset, &p_dsc_entry->entry);
				if(p_fwd_entry){// forwarding table entry exist, update
					update_forwarding_entry_by_path_reply_delay_timer_expired(netkey_offset, p_fwd_entry, p_dsc_entry);
				}
				else{
					p_fwd_entry = add_forwarding_entry_by_path_reply_delay_timer_expired(netkey_offset, &p_dsc_entry->entry);
					if(p_fwd_entry){
						start_path_lifetime_timer(p_fwd_entry, &p_dsc_entry->entry);
					}
				}
				#if MD_SBR_CFG_SERVER_EN
				if(p_fwd_entry && is_subnet_bridge_addr(p_dsc_entry->entry.path_origin.addr, p_dsc_entry->entry.destination)){
					forwarding_tbl_dependent_add(p_dsc_entry->entry.destination, 0, p_fwd_entry->entry.dependent_target);					
				}
				#endif
				prepare_and_send_path_reply(netkey_offset, p_dsc_entry, 0);
			}
		}
	}
}

int directed_forwarding_dependents_update_start(u16 netkey_offset, u8 type, u16 path_enpoint, u16 dependent_addr, u8 dependent_ele_cnt)
{
	int err = -1;
	mesh_ctl_dependent_node_update_t dependent_node_update;
	memset(&dependent_node_update, 0x00, sizeof(dependent_node_update));
	dependent_node_update.type = type;
	dependent_node_update.path_endpoint = path_enpoint;	
	if(dependent_ele_cnt>1){
		dependent_node_update.dependent_addr.range_start_b = dependent_addr;
		dependent_node_update.dependent_addr.length_present_b = 1;
		dependent_node_update.dependent_addr.range_length = dependent_ele_cnt;
	}
	else{
		dependent_node_update.dependent_addr.multicast_addr = dependent_addr;
	}
	non_fixed_entry_t *p_fwd_entry = get_forwarding_entry_correspond2_dependent_node_update(netkey_offset, &dependent_node_update);
	if(p_fwd_entry){
		if(-1 != update_forwarding_by_dependent_node_update(netkey_offset, &dependent_node_update)){
			err =  mesh_tx_cmd_layer_upper_ctl(CMD_CTL_DEPENDENT_NODE_UPDATE, (u8 *)&dependent_node_update, OFFSETOF(mesh_ctl_dependent_node_update_t, dependent_addr) + ((dependent_ele_cnt>1)?3:2), ele_adr_primary, ADR_ALL_DIRECTED_FORWARD, 0);
		}
	}

	return err; 
}

void directed_forwarding_pending_check(u16 netkey_offset)
{
	int num = 0;
	discovery_entry_par_t *p_dsc_pending = 0;
	foreach_arr(i, discovery_table[netkey_offset].dsc_entry_par){
		discovery_entry_par_t *p_dsc_entry = &discovery_table[netkey_offset].dsc_entry_par[i];
		if(p_dsc_entry->entry.path_origin.addr){
			if(p_dsc_entry->state.path_pending){
				p_dsc_pending = p_dsc_entry;
			}
			else{
				num++;
			}
		}
	}

	if(p_dsc_pending && (num < model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[netkey_offset].max_concurrent_init)){
		path_discovery_by_discovery_entry(netkey_offset, p_dsc_pending);;
	}
}

int directed_forwarding_initial_start(u16 netkey_offset, u16 destination, u16 dependent_addr, u16 dependent_ele_cnt)
{
	int err = -1;
	if((ADR_UNASSIGNED==destination) || (ADR_ALL_NODES == destination) || is_own_ele(destination)){
		return err;
	}
	addr_range_t dependent_origin;
	dependent_origin.length_present_b = (dependent_ele_cnt>1)?1:0;
	dependent_origin.range_start_b = dependent_addr;
	dependent_origin.range_length = dependent_ele_cnt;

	addr_range_t path_origin;
	path_origin.length_present_b = (g_ele_cnt > 1)?1:0;
	path_origin.range_start_b = ele_adr_primary;
	path_origin.range_length = g_ele_cnt;
	
	if(netkey_offset < NET_KEY_MAX){
		if(model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[netkey_offset].directed_control.directed_forwarding == DIRECTED_FORWARDING_ENABLE){
			//del_discovery_entry_correspond2_path_destination(netkey_offset, ele_adr_primary, destination);
			discovery_entry_par_t *p_dsc_entry = get_discovery_entry_correspond2_path_destination(netkey_offset, destination);
			if(p_dsc_entry){
				err = 0;
				p_dsc_entry->state.path_need = 1; // will trigle path discovery after retry timer expired
				LOG_DF_DEBUG(0, 0, "%s exist, wait discovery retry", __func__);										
			}
			else if(direced_forwarding_concurrent_count_check(netkey_offset)){				
				discovery_entry_par_t * p_dsc_entry = add_discovery_table_entry(netkey_offset, &path_origin, destination, &dependent_origin, ADR_UNASSIGNED, MESH_BEAR_UNASSIGNED, 0);
				start_path_discovery_timer(p_dsc_entry);
				LOG_DF_DEBUG(0, 0, "%s dst:%x", __func__, destination);
				if(p_dsc_entry){	
					err = 0; 
					path_discovery_by_discovery_entry(netkey_offset, p_dsc_entry);
				}
			}
			else{
				discovery_entry_par_t * p_dsc_entry = add_discovery_table_entry(netkey_offset, &path_origin, destination, &dependent_origin, ADR_UNASSIGNED, MESH_BEAR_UNASSIGNED, 0);
				if(p_dsc_entry){
					p_dsc_entry->state.path_pending = 1;
					LOG_DF_DEBUG(0, 0, "%s max_concurrent_init, pending", __func__);
					err = 0;
				}
			}
		}
	}

	return err;
}

void mesh_directed_forwarding_proc(u8 *bear, u8 *par, int par_len, int src_type)
{
	if(!bear){	// main loop
		foreach(netkey_offset, NET_KEY_MAX){
			forwarding_table_timing_proc(netkey_offset);	
			discovery_table_timing_proc(netkey_offset);
			directed_forwarding_pending_check(netkey_offset);
		}
	}
	else // receive packet
	{
		mesh_cmd_bear_t *p_bear = (mesh_cmd_bear_t *)bear;
		mesh_cmd_nw_t *p_nw = &p_bear->nw;
        mesh_cmd_lt_ctl_unseg_t *p_lt_ctl_unseg = &p_bear->lt_ctl_unseg;
        u8 op = p_lt_ctl_unseg->opcode;
		u8 netkey_offset = mesh_key.netkey_sel_dec;
		non_fixed_entry_t *p_fwd_entry = 0;
		LOG_MSG_LIB(TL_LOG_NODE_SDK, par, par_len, "receive directed control pdu ttl:%d,src:0x%04x,dst:0x%04x sno:0x%06x op:0x%04x(%s), par_len:%d,par:", p_nw->ttl, p_nw->src, p_nw->dst, p_nw->sno[0] + (p_nw->sno[1]<<8) + (p_nw->sno[1]<<16), op, get_op_string_ctl(op, 0), par_len);
		if(CMD_CTL_PATH_REQUEST == op){		
			mesh_ctl_path_req_t *p_path_req = (mesh_ctl_path_req_t *)par;
			addr_range_t *p_addr_origin = (addr_range_t *)p_path_req->addr_par;				
			adv_report_extend_t *p_extend = get_adv_report_extend(&p_bear->len);;
			s8 rssi = p_extend->rssi;  
			LOG_DF_DEBUG(par, par_len, "PATH_REQUEST rssi:%d threshold:%d margin:%d par:", rssi, model_sig_g_df_sbr_cfg.df_cfg.directed_forward.rssi_threshold.default_rssi_threshold,  model_sig_g_df_sbr_cfg.df_cfg.directed_forward.rssi_threshold.rssi_margin);
			if(rssi >= (model_sig_g_df_sbr_cfg.df_cfg.directed_forward.rssi_threshold.default_rssi_threshold + model_sig_g_df_sbr_cfg.df_cfg.directed_forward.rssi_threshold.rssi_margin)){				
				if(p_path_req->path_metric_type == model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[netkey_offset].path_metric.metric_type){					
					addr_range_t *p_org_range = (addr_range_t *)p_path_req->addr_par;					
					u16 origin_addr = (p_org_range->length_present_b)?p_org_range->range_start_b:p_org_range->multicast_addr;

					addr_range_t *p_dpt_range = (addr_range_t *) (p_path_req->addr_par +(p_org_range->length_present_b ? 3:2));
					u16 dpt_addr = p_path_req->on_behalf_of_dependent_origin ? (p_dpt_range->length_present_b ? p_dpt_range->range_start_b : p_dpt_range->multicast_addr) : 0;
					LOG_DF_DEBUG(0, 0, "CMD_CTL_PATH_REQUEST ok, path_metric:%d origin:0x%x target:0x%x dpt_adr:0x%x", p_path_req->origin_path_metric, origin_addr, p_path_req->destination, dpt_addr);
					if((p_path_req->prohibited) || (0 == p_path_req->destination) || is_fixed_group(p_path_req->destination) || (p_path_req->on_behalf_of_dependent_origin && (par_len < (OFFSETOF(mesh_ctl_path_req_t, addr_par)+4))) ||
						!is_unicast_adr(origin_addr+(p_org_range->length_present_b?p_org_range->range_length:0)) || (p_org_range->length_present_b && (p_org_range->range_length<2)) || 
						(p_path_req->on_behalf_of_dependent_origin && (!is_unicast_adr(dpt_addr+(p_dpt_range->length_present_b?p_dpt_range->range_length:0)) || (p_dpt_range->length_present_b && (p_dpt_range->range_length<2)))) ||
						(ADR_ALL_DIRECTED_FORWARD != p_nw->dst)  || p_nw->ttl || (DIRECTED != mesh_key.sec_type_sel)){
							return;
					}
					
					p_fwd_entry = get_forwarding_entry_correspond2_path_request(netkey_offset, p_path_req);
					#if 1 
					if((!p_fwd_entry) || ((u8)(p_path_req->forwarding_number-p_fwd_entry->state.forwarding_number)<128)){ 
					#else
					if(1){// dorwarding number always 0 after powerup now.
					#endif
						if(is_path_target(origin_addr, p_path_req->destination) ||(DIRECTED_RELAY_ENABLE == model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[netkey_offset].directed_control.directed_relay)){
							discovery_entry_par_t *p_dsc_entry = get_discovery_entry_correspond2_path_request(netkey_offset, p_addr_origin->range_start_b, p_path_req->forwarding_number);
						    u8 start_path_request = 0;
							if(!p_dsc_entry){
								p_dsc_entry = add_discovery_table_entry(netkey_offset, 0, 0, 0, p_nw->src, src_type, p_path_req);
								if(p_dsc_entry){
									start_path_discovery_timer(p_dsc_entry);
									if(is_path_target(origin_addr, p_path_req->destination)){
										start_path_reply_delay_timer(p_dsc_entry);											
									}
									start_path_request = 1;										
								}
							}
							else{
								LOG_DF_DEBUG(0, 0, " origin_path_metric:%d entry.path_metric:%d", p_path_req->origin_path_metric, p_dsc_entry->entry.path_metric);
								if(update_discovery_entry_by_path_request(&p_dsc_entry->entry, p_nw->src, src_type, p_path_req)){
									start_path_request = 1;
								}					
							}

							if(start_path_request && (!p_dsc_entry->state.request_delay_timer) && (DIRECTED_RELAY_ENABLE == model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[netkey_offset].directed_control.directed_relay) &&
								((!is_path_target(origin_addr, p_path_req->destination)) || is_group_adr(p_path_req->destination) || is_virtual_adr(p_path_req->destination))
							){
								start_path_request_delay_timer(p_dsc_entry);
							}							
						}
					}
				}
			}
			else{
				// discard
			}

		}
		else if(CMD_CTL_PATH_REPLY == op){	
			mesh_ctl_path_reply_t *p_path_reply = (mesh_ctl_path_reply_t *)par;
			discovery_entry_par_t *p_dsc_entry = get_discovery_entry_correspond2_path_reply(netkey_offset, p_path_reply);
			if(p_path_reply->prohibit){
				return;
			}

			if(p_dsc_entry){
				LOG_DF_DEBUG(par, par_len, "discovery entry found origin:0x%x ", p_path_reply->path_origin);
				p_fwd_entry = get_forwarding_entry_correspond2_path_reply(netkey_offset, p_path_reply);
				if(p_fwd_entry){
					update_forwarding_entry_by_path_reply(netkey_offset, src_type, p_fwd_entry, p_dsc_entry);
				}
				else{
					p_fwd_entry = add_forwarding_tbl_entry_by_rcv_path_reply(netkey_offset, src_type, p_dsc_entry, p_path_reply);				
					start_path_lifetime_timer(p_fwd_entry, &p_dsc_entry->entry);
				}

				if(p_fwd_entry && p_dsc_entry->state.first_reply_msg){
					p_dsc_entry->state.new_lane_established = 1;
					p_dsc_entry->state.first_reply_msg = 0;
					if(is_own_ele(p_path_reply->path_origin)){// path origin
						if(1 == p_path_reply->confirmation_request){ 
							directed_forwarding_confirm_start(netkey_offset, p_fwd_entry);
						}

						if((!p_fwd_entry->state.path_echo_timer_ms) && (is_unicast_adr(p_fwd_entry->entry.destination)?model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[netkey_offset].unicast_echo_interval:model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[netkey_offset].multicast_echo_interval)){
							start_path_echo_timer(netkey_offset, p_fwd_entry);
						}
					}
					else{
						prepare_and_send_path_reply(netkey_offset, p_dsc_entry, p_path_reply->confirmation_request);
					}
				}
			}
		}
		else if(CMD_CTL_PATH_CONFIRMATION == op){
			mesh_ctl_path_confirmation_t *p_path_confirm = (mesh_ctl_path_confirmation_t *)par;
			p_fwd_entry = get_forwarding_entry_correspond2_path_confirm(netkey_offset, p_path_confirm);
			if(p_fwd_entry){
				LOG_DF_DEBUG(par, par_len, "forwarding entry found origin:0x%04x target:0x%04x", p_path_confirm->path_origin, p_path_confirm->path_target);
				discovery_entry_par_t *p_dsc_entry = get_discovery_entry_correspond2_forwarding_entry(netkey_offset, p_fwd_entry);
				if(p_fwd_entry->entry.backward_path_validated){
					if(!is_path_target(p_path_confirm->path_origin, p_path_confirm->path_target)){
						if(p_dsc_entry && !p_dsc_entry->state.path_confirm_sent){
							directed_forwarding_confirm_start(netkey_offset, p_fwd_entry);
						}
					}
				}
				else{
					update_forwarding_entry_by_path_confirm(netkey_offset, p_fwd_entry, p_path_confirm);
					if(!is_path_target(p_path_confirm->path_origin, p_path_confirm->path_target)){
						directed_forwarding_confirm_start(netkey_offset, p_fwd_entry);
					}
				}
			}
		}
		else if(CMD_CTL_PATH_ECHO_REQUEST == op){
			if((TTL_MAX != p_nw->ttl) || (DIRECTED != mesh_key.sec_type_sel)){
				return;
			}
			p_fwd_entry = get_forwarding_entry_correspond2_path_echo_request(netkey_offset, p_nw->src, p_nw->dst);
			if(p_fwd_entry){
				cfg_cmd_send_path_echo_reply(p_fwd_entry);
			}			
		}
		else if(CMD_CTL_PATH_ECHO_REPLY == op){
			mesh_ctl_path_echo_reply_t *p_path_echo_reply = (mesh_ctl_path_echo_reply_t *)par;
			p_fwd_entry = get_forwarding_entry_correspond2_path_echo_reply(netkey_offset, p_nw->dst, p_path_echo_reply);
			if(p_fwd_entry){
				LOG_DF_DEBUG(0, 0, "forwarding entry found target:0x%x", p_path_echo_reply->destination);
				if((p_fwd_entry->entry.backward_path_validated && (DIRECTED != mesh_key.sec_type_sel)) || 
					((!p_fwd_entry->entry.backward_path_validated && (MASTER != mesh_key.sec_type_sel)))){
					return;
				}
				
				if(p_fwd_entry->state.path_echo_reply_timeout_timer){
					stop_path_echo_reply_timeout_timer(p_fwd_entry); // Path Validation Succeeded
					if(is_unicast_adr(p_fwd_entry->entry.destination)?model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[netkey_offset].unicast_echo_interval:model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[netkey_offset].multicast_echo_interval){
						start_path_echo_timer(netkey_offset, p_fwd_entry);
					}
				}
			}
		}
		else if(CMD_CTL_DEPENDENT_NODE_UPDATE == op){
			mesh_ctl_dependent_node_update_t *p_dependent_node_update = (mesh_ctl_dependent_node_update_t *)par;
			if(p_nw->ttl || (ADR_ALL_DIRECTED_FORWARD != p_nw->dst) || (DIRECTED != mesh_key.sec_type_sel) || (p_dependent_node_update->dependent_addr.length_present_b && (p_dependent_node_update->dependent_addr.range_length<2))){
				return;
			}
			
			p_fwd_entry = get_forwarding_entry_correspond2_dependent_node_update(netkey_offset, p_dependent_node_update);
			if(p_fwd_entry){
				LOG_DF_DEBUG(0, 0, "forwarding entry found, endpoint:0x%x dpt_addr:0x%x", p_dependent_node_update->path_endpoint, p_dependent_node_update->dependent_addr);
				directed_forwarding_dependents_update_start(netkey_offset, p_dependent_node_update->type, p_dependent_node_update->path_endpoint, p_dependent_node_update->dependent_addr.length_present_b ? p_dependent_node_update->dependent_addr.range_start_b:p_dependent_node_update->dependent_addr.multicast_addr, p_dependent_node_update->dependent_addr.length_present_b ? p_dependent_node_update->dependent_addr.range_length:0);
			}
		}
		else if(CMD_CTL_PATH_REQUEST_SOLICITATION == op){
			mesh_ctl_path_request_solication_t *p_path_req_solicat = (mesh_ctl_path_request_solication_t *)par;
			foreach(i, par_len/2){
				if((ADR_ALL_DIRECTED_FORWARD != p_nw->dst) || (TTL_MAX != p_nw->ttl) || (DIRECTED != mesh_key.sec_type_sel) || (ADR_ALL_DIRECTED_FORWARD == p_path_req_solicat->addr_list[i]) || (ADR_ALL_NODES == p_path_req_solicat->addr_list[i]) || (ADR_ALL_RELAY == p_path_req_solicat->addr_list[i])){
					return;
				}
				
				p_fwd_entry = get_non_fixed_path_entry(netkey_offset, ele_adr_primary, p_path_req_solicat->addr_list[i]);
				if(p_fwd_entry){
					LOG_DF_DEBUG(0, 0, "path entry found", 0);
					discovery_entry_par_t *p_dsc_entry = get_discovery_entry_correspond2_forwarding_entry(netkey_offset, p_fwd_entry);
					if(p_dsc_entry){
						directed_discovery_entry_remove(p_dsc_entry);
					}
					delete_non_fixed_path(netkey_offset, &p_fwd_entry->entry);
					directed_forwarding_initial_start(netkey_offset, p_path_req_solicat->addr_list[i], 0, 0);
				}
			}
		}
	}
}
#endif
#endif

#if MD_DF_CFG_CLIENT_EN
int mesh_cmd_sig_cfg_directed_control_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_path_metric_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_dsc_tbl_capa_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_forwarding_tbl_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_forwarding_tbl_dependents_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_forwarding_tbl_dependents_get_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_forwarding_tbl_entries_count_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_forwarding_tbl_entries_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_wanted_lanes_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_two_way_path_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_path_echo_interval_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_directed_network_transmit_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_directed_relay_retransmit_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_rssi_threshold_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_directed_paths_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_directed_publish_policy_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_path_discovery_timing_control_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_directed_control_network_transmit_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_directed_control_relay_transmit_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int cfg_cmd_send_directed_control_get(u16 adr_dst, u16 netkey_index)
{
 	return SendOpParaDebug(adr_dst, 1, DIRECTED_CONTROL_GET, (u8 *)&netkey_index, sizeof(netkey_index));
}

int cfg_cmd_send_directed_control_set(u16 adr_dst, directed_control_set_t *directed_control)
{	
 	return SendOpParaDebug(adr_dst, 1, DIRECTED_CONTROL_SET, (u8 *)directed_control, sizeof(directed_control_set_t));
}

int mesh_directed_proxy_control_set(u8 use_directed, u16 range_start, u8 range_len)
{
	directed_proxy_ctl_t proxy_ctl;
	memset(&proxy_ctl, 0x00, sizeof(proxy_ctl));
	proxy_ctl.use_directed = use_directed;
	proxy_ctl.addr_range.range_start_b = range_start;
	if(range_len > 1){
		proxy_ctl.addr_range.length_present_b = 1;
		proxy_ctl.addr_range.range_length = range_len;
	}
	u8 par_len = OFFSETOF(directed_proxy_ctl_t, addr_range) + (proxy_ctl.addr_range.length_present_b?3:2);
	#if WIN32
	LOG_MSG_INFO(TL_LOG_NODE_BASIC,(u8 *)&proxy_ctl,par_len ,"mesh_directed_proxy_control_set",0);
	#endif
	return mesh_tx_cmd_layer_cfg_primary(DIRECTED_PROXY_CONTROL,(u8 *)&proxy_ctl, par_len,PROXY_CONFIG_FILTER_DST_ADR);
}

int cfg_cmd_path_metric_get(u16 node_adr, u16 nk_idx)
{
	return SendOpParaDebug(node_adr, 0, PATH_METRIC_GET, (u8 *)&nk_idx, sizeof(nk_idx));
}

int cfg_cmd_path_metric_set(u16 node_adr, u16 nk_idx, u8 metric_type, u8 lifetime)
{
	path_metric_set_t path_metric;
	path_metric.netkey_index = nk_idx;
	path_metric.path_metric.metric_type = metric_type;
	path_metric.path_metric.path_lifetime = lifetime;
	return SendOpParaDebug(node_adr, 0, PATH_METRIC_SET, (u8 *)&path_metric, sizeof(path_metric_set_t));
}

int cfg_cmd_max_concur_init_get(u16 node_adr, u16 nk_idx)
{
	return SendOpParaDebug(node_adr, 0, DISCOVERY_TABLE_CAPABILITIES_GET, (u8 *)&nk_idx, sizeof(nk_idx));
}

int cfg_cmd_max_concur_init_set(u16 node_adr, u16 nk_idx, u8 max_concur_init)
{
	dsc_tbl_capa_set_t capa_set;
	capa_set.netkey_index = nk_idx;
	capa_set.max_concurrent_init = max_concur_init;
	return SendOpParaDebug(node_adr, 0, DISCOVERY_TABLE_CAPABILITIES_SET, (u8 *)&capa_set, sizeof(capa_set));
}

int cfg_cmd_forwarding_tbl_add(u16 node_adr, u16 nk_idx, u16 adr_origin, u8 range_len_origin, u16 adr_dst, u8 range_len_dst, u16 bear2_origin, u16 bear2_dst)
{
	forwarding_tbl_add_t tbl_add;
	memset(&tbl_add, 0x00, sizeof(tbl_add));
	tbl_add.unicast_destination_flag = is_unicast_adr(adr_dst);
	tbl_add.backward_path_validated_flag = is_unicast_adr(adr_dst);// two way
	tbl_add.netkey_index = nk_idx;
	tbl_add.prohibited = 0;
	int par_len = 0;
	addr_range_t* p_adr_range = (addr_range_t*)tbl_add.par;
	if(range_len_origin > 1){
		par_len += 3;
		p_adr_range->length_present_l = 1;
		p_adr_range->range_start_l = adr_origin;
		p_adr_range->range_length = range_len_origin;
	}
	else{
		par_len += 2;
		p_adr_range->multicast_addr = adr_origin;
	}

	p_adr_range = (addr_range_t*)(tbl_add.par + par_len);
	if(range_len_dst > 1){
		par_len += 3;
		p_adr_range->length_present_l = 1;
		p_adr_range->range_start_l = adr_dst;
		p_adr_range->range_length = range_len_dst;
	}
	else{
		par_len += 2;
		p_adr_range->multicast_addr = adr_dst;
	}
	memcpy(tbl_add.par+par_len, &bear2_origin, 2);
	par_len += 2;
	memcpy(tbl_add.par+par_len, &bear2_dst, 2);
	par_len += 2;

	return SendOpParaDebug(node_adr, 0, FORWARDING_TABLE_ADD, (u8 *)&tbl_add, par_len+OFFSETOF(forwarding_tbl_add_t, par));
}

int cfg_cmd_forwarding_tbl_dependents_add(u16 node_adr, u16 nk_idx, u16 path_origin, u16 path_target, int dpt_origin_num, int dpt_target_num, u8 dpt_tbl[], int tbl_size)
{
	forwarding_tbl_dependengts_add_t dpt_add;
	if(tbl_size > sizeof(dpt_add.par)){
		return -1;
	}
	dpt_add.netkey_index = nk_idx;
	dpt_add.path_origin = path_origin;
	dpt_add.destination = path_target;
	dpt_add.dependent_origin_list_size = dpt_origin_num;
	dpt_add.dependent_target_list_size = dpt_target_num;
	memcpy(dpt_add.par, dpt_tbl, tbl_size);

	return SendOpParaDebug(node_adr, 0, FORWARDING_TABLE_DEPENDENTS_ADD, (u8 *)&dpt_add, OFFSETOF(forwarding_tbl_dependengts_add_t, par)+tbl_size);
}

int cfg_cmd_forwarding_tbl_entries_count_get(u16 node_adr, u16 nk_idx)
{
	u16 netkey_index = nk_idx;
	return SendOpParaDebug(node_adr, 0, FORWARDING_TABLE_ENTRIES_COUNT_GET, (u8 *)&netkey_index, sizeof(netkey_index));
}

int cfg_cmd_forwarding_tbl_entries_get(u16 node_adr, u8 *par, int len)
{
	return SendOpParaDebug(node_adr, 0, FORWARDING_TABLE_ENTRIES_GET, par, len);
}

int cfg_cmd_forwarding_tbl_dependents_get(u16 node_adr, u8 *par, int len)
{
	return SendOpParaDebug(node_adr, 0, FORWARDING_TABLE_DEPENDENTS_GET, par, len);
}

int cfg_cmd_forwarding_tbl_delete(u16 node_adr, u16 nk_idx, u16 path_origin, u16 destination)
{
	forwarding_tbl_delete_t tbl_delete;
	tbl_delete.netkey_index = nk_idx;
	tbl_delete.path_origin = path_origin;
	tbl_delete.destination = destination;
	return SendOpParaDebug(node_adr, 0, FORWARDING_TABLE_DELETE, (u8 *)&tbl_delete, sizeof(forwarding_tbl_delete_t));
}

int cfg_cmd_wanted_lanes_get(u16 node_adr, u16 nk_idx)
{
	u16 netkey_idx = nk_idx;
	return SendOpParaDebug(node_adr, 0, WANTED_LANES_GET, (u8 *)&netkey_idx, sizeof(netkey_idx));
}

int cfg_cmd_wanted_lanes_set(u16 node_adr, u16 nk_idx, u8 wanted_lanes)
{
	wanted_lanes_set_t lanes_set;
	lanes_set.netkey_index = nk_idx;
	lanes_set.wanted_lanes = wanted_lanes;
	return SendOpParaDebug(node_adr, 0, WANTED_LANES_SET, (u8 *)&lanes_set, sizeof(lanes_set));
}

int cfg_cmd_two_way_path_get(u16 node_adr, u16 nk_idx)
{
	u16 netkey_idx = nk_idx;
	return SendOpParaDebug(node_adr, 0, TWO_WAY_PATH_GET, (u8 *)&netkey_idx, sizeof(netkey_idx));
}

int cfg_cmd_two_way_path_set(u16 node_adr, u16 nk_idx, u8 two_way_path)
{
	two_way_path_set_t two_way;
	two_way.netkey_index = nk_idx;
	two_way.Prohibited = 0;
	two_way.two_way_path = two_way_path;
	return SendOpParaDebug(node_adr, 0, TWO_WAY_PATH_SET, (u8 *)&two_way, sizeof(two_way));
}

int cfg_cmd_path_echo_interval_get(u16 node_adr, u16 nk_idx)
{
	u16 netkey_index = nk_idx;
	return SendOpParaDebug(node_adr, 0, PATH_ECHO_INTERVAL_GET, (u8 *)&netkey_index, sizeof(netkey_index));
}

int cfg_cmd_path_echo_interval_set(u16 node_adr, u16 nk_idx, u8 unicast_invl, u8 multi_invl)
{
	path_echo_interval_set_t echo_set;
	echo_set.netkey_index = nk_idx;
	echo_set.unicast_echo_interval = unicast_invl;
	echo_set.multicast_echo_interval = multi_invl;
	return SendOpParaDebug(node_adr, 0, PATH_ECHO_INTERVAL_SET, (u8 *)&echo_set, sizeof(echo_set));
}

int cfg_cmd_directed_network_transmit_get(u16 node_adr)
{
	return SendOpParaDebug(node_adr, 0, DIRECTED_NETWORK_TRANSMIT_GET, 0, 0);
}

int cfg_cmd_directed_network_transmit_set(u16 node_adr, u8 directed_transmit)
{
	u8 transmit = directed_transmit;
	return SendOpParaDebug(node_adr, 0, DIRECTED_NETWORK_TRANSMIT_SET, &transmit, sizeof(transmit));
}

int cfg_cmd_directed_relay_retransmit_get(u16 node_adr)
{
	return SendOpParaDebug(node_adr, 0, DIRECTED_RELAY_RETRANSMIT_GET, 0, 0);
}

int cfg_cmd_directed_relay_retransmit_set(u16 node_adr, u8 retransmit)
{
	u8 transmit = retransmit;
	return SendOpParaDebug(node_adr, 0, DIRECTED_RELAY_RETRANSMIT_SET, &transmit, sizeof(transmit));
}

int cfg_cmd_rssi_threshold_get(u16 node_adr)
{
	return SendOpParaDebug(node_adr, 0, RSSI_THRESHOLD_GET, 0, 0);
}

int cfg_cmd_rssi_threshold_set(u16 node_adr, s8 rssi_margin)
{
	s8 rssi = rssi_margin;
	return SendOpParaDebug(node_adr, 0, RSSI_THRESHOLD_SET, (u8 *)&rssi, sizeof(rssi));
}

int cfg_cmd_directed_path_get(u16 node_adr)
{
	return SendOpParaDebug(node_adr, 0, DIRECTED_PATHS_GET, 0, 0);
}

int cfg_cmd_directed_publish_policy_get(u16 node_adr, u16 ele_addr, u32 model_id)
{
	directed_pub_policy_get_t policy_get;
	policy_get.ele_addr = ele_addr;
	policy_get.model_id = model_id;
	return SendOpParaDebug(node_adr, 0, DIRECTED_PUBLISH_POLICY_GET, (u8 *)&policy_get, sizeof(policy_get));
}

int cfg_cmd_directed_publish_policy_set(u16 node_adr, u8 policy, u16 ele_addr, u32 model_id)
{
	directed_pub_policy_set_t policy_set;
	policy_set.directed_pub_policy = policy;
	policy_set.ele_addr = ele_addr;
	policy_set.model_id = model_id;
	return SendOpParaDebug(node_adr, 0, DIRECTED_PUBLISH_POLICY_SET, (u8 *)&policy_set, sizeof(policy_set));
}

int cfg_cmd_path_dsc_timing_ctl_get(u16 node_adr)
{
	return SendOpParaDebug(node_adr, 0, PATH_DISCOVERY_TIMING_CONTROL_GET, 0, 0);
}

int cfg_cmd_path_dsc_timing_ctl_set(u16 node_adr, u16 monitor_interval, u16 dsc_retry_interval, bool dsc_interval, bool dsc_guard_interval)
{
	discovery_timing_t timing;
	timing.path_monitoring_interval = monitor_interval;
	timing.path_discovery_retry_interval = dsc_retry_interval;
	timing.path_discovery_interval = dsc_interval;
	timing.lane_discovery_guard_interval = dsc_guard_interval;
	timing.prohibited = 0;
	return SendOpParaDebug(node_adr, 0, PATH_DISCOVERY_TIMING_CONTROL_SET, (u8 *)&timing, sizeof(timing));
}

int cfg_cmd_directed_ctl_network_transmit_get(u16 node_adr)
{
	return SendOpParaDebug(node_adr, 0, DIRECTED_CONTROL_NETWORK_TRANSMIT_GET, 0, 0);
}

int cfg_cmd_directed_ctl_network_transmit_set(u16 node_adr, u8 transmit)
{
	u8 ctl_transmit = transmit;
	return SendOpParaDebug(node_adr, 0, DIRECTED_CONTROL_NETWORK_TRANSMIT_SET, &ctl_transmit, sizeof(ctl_transmit));
}

int cfg_cmd_directed_ctl_relay_retransmit_get(u16 node_adr)
{
	return SendOpParaDebug(node_adr, 0, DIRECTED_CONTROL_RELAY_RETRANSMIT_GET, 0, 0);
}

int cfg_cmd_directed_ctl_relay_retransmit_set(u16 node_adr, u8 transmit)
{
	u8 ctl_transmit = transmit;
	return SendOpParaDebug(node_adr, 0, DIRECTED_CONTROL_RELAY_RETRANSMIT_SET, &ctl_transmit, sizeof(ctl_transmit));
}

int cfg_cmd_send_path_solicitation(u16 netkey_offset, u16 *addr_list, int num)
{
	return mesh_tx_cmd_layer_upper_ctl_primary_specified_key(CMD_CTL_PATH_REQUEST_SOLICITATION, (u8 *)addr_list, num<<1, ADR_ALL_DIRECTED_FORWARD, netkey_offset);
}
#endif
