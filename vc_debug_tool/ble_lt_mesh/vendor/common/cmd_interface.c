/********************************************************************************************************
 * @file	cmd_interface.c
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
#if WIN32
#include "../../../reference/tl_bulk/lib_file/host_fifo.h"
#include "../../../reference/tl_bulk/lib_file/gatt_provision.h"
#include "../../../reference/tl_bulk/lib_file/hw_fun.h"
#include "app_provison.h"
#else
#include "proj_lib/ble/ll/ll.h"
#include "proj_lib/ble/blt_config.h"
#include "user_config.h"
#include "proj_lib/sig_mesh/app_mesh.h"
#include "proj_lib/mesh_crypto/mesh_crypto.h"
#include "proj_lib/pm.h"
#include "app_proxy.h"
#include "app_health.h"
#endif

#include "subnet_bridge.h"
#include "cmd_interface.h"
#include "solicitation_rpl_cfg_model.h"
#include "app_heartbeat.h"
//-----------access command--------
int access_cmd_get_level(u16 adr,u32 rsp_max)
{
	u8 par[1];
	return SendOpParaDebug(adr, rsp_max, G_LEVEL_GET, par, 0);
}

int access_cmd_set_level(u16 adr, u8 rsp_max, s16 level, int ack, transition_par_t *trs_par)
{
	mesh_cmd_g_level_set_t par = {0};
	u32 par_len = OFFSETOF(mesh_cmd_g_level_set_t,transit_t);
	par.level = level;
	if(trs_par){
		par_len = sizeof(mesh_cmd_g_level_set_t);
		memcpy(&par.transit_t, trs_par, 2);
	}
	
	return SendOpParaDebug(adr, rsp_max, ack ? G_LEVEL_SET : G_LEVEL_SET_NOACK, 
						   (u8 *)&par, par_len);
}

int access_cmd_set_delta(u16 adr, u8 rsp_max, s32 level, int ack, transition_par_t *trs_par)
{
	mesh_cmd_g_level_delta_t par = {0};
	u32 par_len = OFFSETOF(mesh_cmd_g_level_delta_t,transit_t);
	par.level = level;
	if(trs_par){
		par_len = sizeof(mesh_cmd_g_level_delta_t);
		memcpy(&par.transit_t, trs_par, 2);
	}

	return SendOpParaDebug(adr, rsp_max, ack ? G_DELTA_SET : G_DELTA_SET_NOACK, 
						   (u8 *)&par, par_len);
}

#if 1
int access_cmd_onoff(u16 adr_dst, u8 rsp_max, u8 onoff, int ack, transition_par_t *trs_par)
{
	mesh_cmd_g_onoff_set_t par = {0};
	u32 par_len = OFFSETOF(mesh_cmd_g_onoff_set_t,transit_t);
	par.onoff = onoff;
	if(trs_par){
		par_len = sizeof(mesh_cmd_g_onoff_set_t);
		memcpy(&par.transit_t, trs_par, 2);
	}

	return SendOpParaDebug(adr_dst, rsp_max, ack ? G_ONOFF_SET : G_ONOFF_SET_NOACK, 
						   (u8 *)&par, par_len);
}
#else // for long packet test
typedef struct{
	u8 onoff;
	u8 tid;
	u8 transit_t;
	u8 delay;		// unit 5ms
	u8 rsv[219];
}mesh_cmd_g_onoff_set2_t;

int access_cmd_onoff(u16 adr_dst, u8 rsp_max, u8 onoff, int ack, transition_par_t *trs_par)
{
    //ack = 0;
	mesh_cmd_g_onoff_set2_t par = {0};
	u32 par_len = sizeof(par);//OFFSETOF(mesh_cmd_g_onoff_set2_t,transit_t);
	par.onoff = onoff;
	//if(trs_par){
		par_len = sizeof(mesh_cmd_g_onoff_set2_t);
		//memcpy(&par.transit_t, trs_par, 2);
	//}

	return SendOpParaDebug(adr_dst, rsp_max, ack ? G_ONOFF_SET : G_ONOFF_SET_NOACK, 
						   (u8 *)&par, par_len);
}
#endif

int access_cmd_onoff_get(u16 adr, u8 rsp_max)
{
	return SendOpParaDebug(adr, rsp_max, G_ONOFF_GET, 0, 0);
}

//------config command--------
int cfg_cmd_cps_get(u16 node_adr, u8 page)
{
	return SendOpParaDebug (node_adr, 1, COMPOSITION_DATA_GET, &page, 1);
}

int cfg_cmd_large_cps_data_get(u16 node_adr, u8 page, u16 offset)
{
	large_cps_get_t cps;
	cps.page = page;
	cps.offset = offset;
	return SendOpParaDebug (node_adr, 1, LARGE_CPS_GET, (u8 *)&cps, sizeof(cps));
}

int cfg_cmd_models_metadata_get(u16 node_adr, u8 page, u16 offset)
{
	models_meta_get_t meta;
	meta.meta_page = page;
	meta.offset = offset;
	return SendOpParaDebug (node_adr, 1, MODELS_METADATA_GET, (u8 *)&meta, sizeof(meta));
}

int cfg_cmd_sub_get(u16 node_adr, u16 ele_adr, u16 md_id)
{
    node_adr = get_primary_adr_with_check(node_adr, ele_adr);
    
	mesh_cfg_model_sub_get_sig_t par = {0};
	par.ele_adr = ele_adr;
	par.model_id = md_id;
	
	return SendOpParaDebug (node_adr, 1, CFG_SIG_MODEL_SUB_GET,(u8 *)&par, sizeof(mesh_cfg_model_sub_get_sig_t));
}

int group_status[2][32];
mesh_cfg_cmd_sub_set_par_t mesh_cfg_cmd_sub_set_par;

int cfg_cmd_sub_set(u16 op, u16 node_adr, u16 ele_adr, u16 sub_adr, u32 md_id, bool4 sig_model)
{
    LOG_MSG_FUNC_NAME();
    node_adr = get_primary_adr_with_check(node_adr, ele_adr);

	mesh_cfg_model_sub_set_t par = {0};
	par.ele_adr = ele_adr;
	par.sub_adr = sub_adr;
	par.model_id = md_id;

	int par_len = sizeof(mesh_cfg_model_sub_set_t) - (sig_model ? 2 : 0);
	if(CFG_MODEL_SUB_DEL_ALL == op){
		memcpy(&par.sub_adr, &par.model_id, 4);
		par_len -= 2;
	}
	mesh_cfg_cmd_sub_set_par.op = op;
	mesh_cfg_cmd_sub_set_par.ele_adr = ele_adr;
	return SendOpParaDebug(node_adr, 1, op, (u8 *)&par, par_len); 	// sig model
}

int cfg_cmd_pub_get(u16 node_adr, u16 ele_adr, u32 md_id, bool4 sig_model)
{
    node_adr = get_primary_adr_with_check(node_adr, ele_adr);
    
	mesh_cfg_model_pub_get_t par = {0};
	par.ele_adr = ele_adr;
	par.model_id = md_id;
	
	int par_len = sizeof(mesh_cfg_model_pub_get_t) - (sig_model ? 2 : 0);
	return SendOpParaDebug (node_adr, 1, CFG_MODEL_PUB_GET,(u8 *)&par, par_len);
}

int cfg_cmd_pub_set(u16 node_adr, u16 ele_adr, u16 pub_adr, mesh_model_pub_par_t *pub_par, u32 md_id, bool4 sig_model)
{
    node_adr = get_primary_adr_with_check(node_adr, ele_adr);
    
	mesh_cfg_model_pub_set_t par = {0};
	par.ele_adr = ele_adr;
	par.model_id = md_id;
	par.pub_adr = pub_adr;
	memcpy(&par.pub_par, pub_par, sizeof(mesh_model_pub_par_t));
	int par_len = sizeof(mesh_cfg_model_pub_set_t) - (sig_model ? 2 : 0);
	return SendOpParaDebug (node_adr, 1, CFG_MODEL_PUB_SET,(u8 *)&par, par_len);
}

int cfg_cmd_sec_nw_bc_get(u16 node_adr)
{
	u8 par[1];
	return SendOpParaDebug (node_adr, 1, CFG_BEACON_GET, par, 0);
}

int cfg_cmd_sec_nw_bc_set(u16 node_adr, u8 val)
{
	return SendOpParaDebug (node_adr, 1, CFG_BEACON_SET, (u8 *)&val, 1);
}

int cfg_cmd_ttl_get(u16 node_adr)
{
	u8 par[1];
	return SendOpParaDebug (node_adr, 1, CFG_DEFAULT_TTL_GET, par, 0);
}

int cfg_cmd_ttl_set(u16 node_adr, u8 val)
{
	return SendOpParaDebug (node_adr, 1, CFG_DEFAULT_TTL_SET, (u8 *)&val, 1);
}

int cfg_cmd_nw_transmit_get(u16 node_adr)
{
	u8 par[1];
	return SendOpParaDebug (node_adr, 1, CFG_NW_TRANSMIT_GET, par, 0);
}

int cfg_cmd_nw_transmit_set(u16 node_adr, u8 cnt, u8 invl_10ms)
{
	mesh_transmit_t transmit;
	transmit.count = cnt > 0 ? cnt - 1 : 0;
	transmit.invl_steps = invl_10ms > 0 ? invl_10ms - 1 : 0;
	
	return SendOpParaDebug (node_adr, 1, CFG_NW_TRANSMIT_SET, (u8 *)&transmit, sizeof(transmit));
}

int cfg_cmd_relay_get(u16 node_adr)
{
	u8 par[1];
	return SendOpParaDebug (node_adr, 1, CFG_RELAY_GET, par, 0);
}

int cfg_cmd_relay_set(u16 node_adr, mesh_cfg_model_relay_set_t relay_set)
{
	return SendOpParaDebug (node_adr, 1, CFG_RELAY_SET, (u8 *)&relay_set, sizeof(relay_set));
}

int cfg_cmd_friend_get(u16 node_adr)
{
	u8 par[1];
	return SendOpParaDebug (node_adr, 1, CFG_FRIEND_GET, par, 0);
}

int cfg_cmd_friend_set(u16 node_adr, u8 val)
{
	return SendOpParaDebug (node_adr, 1, CFG_FRIEND_SET, (u8 *)&val, 1);
}

int cfg_cmd_proxy_get(u16 node_adr)
{
	u8 par[1];
	return SendOpParaDebug (node_adr, 1, CFG_GATT_PROXY_GET, par, 0);
}

int cfg_cmd_proxy_set(u16 node_adr, u8 val)
{
	return SendOpParaDebug (node_adr, 1, CFG_GATT_PROXY_SET, (u8 *)&val, 1);
}

int cfg_cmd_reset_node(u16 node_adr)
{
	u8 par[1];
	return SendOpParaDebug(node_adr, 0, NODE_RESET, par, 0);
}

int cfg_cmd_sig_model_app_get(u16 node_adr, u16 ele_adr, u16 model_id)
{
    node_adr = get_primary_adr_with_check(node_adr, ele_adr);
    
	mesh_md_app_get_t get;
	get.ele_adr = ele_adr;
	get.model_id = model_id;
	return SendOpParaDebug(node_adr, 0, SIG_MODEL_APP_GET, (u8 *)&get, 4);
}

int cfg_cmd_vd_model_app_get(u16 node_adr, u16 ele_adr, u32 model_id)
{
    node_adr = get_primary_adr_with_check(node_adr, ele_adr);
    
	mesh_md_app_get_t get;
	get.ele_adr = ele_adr;
	get.model_id = model_id;
	return SendOpParaDebug(node_adr, 0, VENDOR_MODEL_APP_GET, (u8 *)&get, sizeof(mesh_md_app_get_t));
}

int cfg_cmd_sig_model_app_set(u16 node_adr ,u8 *p_app_idx,u8 *p_app_key)
{
	mesh_appkey_set_t set;
	memcpy(set.net_app_idx,p_app_idx,sizeof(set.net_app_idx));
	memcpy(set.appkey,p_app_key,sizeof(set.appkey));
	return SendOpParaDebug(node_adr, 0, APPKEY_ADD, (u8 *)&set, sizeof(mesh_appkey_set_t));
}

int cfg_cmd_key_phase_get(u16 node_adr, u16 nk_idx)
{
	mesh_key_refresh_phase_get_t get;
	get.nk_idx = nk_idx;
	return SendOpParaDebug(node_adr, 0, CFG_KEY_REFRESH_PHASE_GET, (u8 *)&get, sizeof(get));
}

int cfg_cmd_key_phase_set(u16 node_adr, u16 nk_idx, u8 transition)
{
	mesh_key_refresh_phase_set_t set;
	set.nk_idx = nk_idx;
	set.transition = transition;
	return SendOpParaDebug(node_adr, 0, CFG_KEY_REFRESH_PHASE_SET, (u8 *)&set, sizeof(set));
}

int cfg_cmd_node_identity_get(u16 node_adr, u16 nk_idx)
{
	mesh_node_identity_get_t get;
	get.nk_idx = nk_idx;
	return SendOpParaDebug(node_adr, 0, NODE_ID_GET, (u8 *)&get, sizeof(get));
}

int cfg_cmd_node_identity_set(u16 node_adr, u16 nk_idx, u8 identity)
{
	mesh_node_identity_set_t set;
	set.nk_idx = nk_idx;
	set.identity = identity;
	return SendOpParaDebug(node_adr, 0, NODE_ID_SET, (u8 *)&set, sizeof(set));
}

int cfg_cmd_lpn_poll_timeout_get(u16 node_adr, u16 adr_lpn)
{
	return SendOpParaDebug(node_adr, 0, CFG_LPN_POLL_TIMEOUT_GET, (u8 *)&adr_lpn, sizeof(adr_lpn));
}

//------------------ key command
int cfg_cmd_nk_set(u16 op, u16 node_adr, u16 nk_idx, u8 *key)
{
	mesh_netkey_set_t set;
	set.idx = nk_idx;
	memcpy(set.key, key, sizeof(set.key));
	return SendOpParaDebug(node_adr, 0, op, (u8 *)&set, sizeof(set));
}

int cfg_cmd_nk_add(u16 node_adr, u16 nk_idx, u8 *key)
{
	return cfg_cmd_nk_set(NETKEY_ADD, node_adr, nk_idx, key);
}

int cfg_cmd_nk_update(u16 node_adr, u16 nk_idx, u8 *key)
{
	return cfg_cmd_nk_set(NETKEY_UPDATE, node_adr, nk_idx, key);
}

int cfg_cmd_nk_del(u16 node_adr, u16 nk_idx)
{
	mesh_netkey_set_t set;
	set.idx = nk_idx;
	return SendOpParaDebug(node_adr, 0, APPKEY_DEL, (u8 *)&set, sizeof(set.idx));
}

int cfg_cmd_ak_set(u16 op, u16 node_adr, u16 nk_idx, u16 ak_idx, u8 *key)
{
	mesh_appkey_set_t set;
	SET_KEY_INDEX(set.net_app_idx, nk_idx, ak_idx);
	memcpy(set.appkey, key, sizeof(set.appkey));
	return SendOpParaDebug(node_adr, 0, op, (u8 *)&set, sizeof(set));
}

int cfg_cmd_ak_add(u16 node_adr, u16 nk_idx, u16 ak_idx, u8 *key)
{
	return cfg_cmd_ak_set(APPKEY_ADD, node_adr, nk_idx, ak_idx, key);
}

int cfg_cmd_ak_update(u16 node_adr, u16 nk_idx, u16 ak_idx, u8 *key)
{
	return cfg_cmd_ak_set(APPKEY_UPDATE, node_adr, nk_idx, ak_idx, key);
}

int cfg_cmd_ak_del(u16 node_adr, u16 nk_idx, u16 ak_idx)
{
	mesh_appkey_set_t set;
	SET_KEY_INDEX(set.net_app_idx, nk_idx, ak_idx);
	return SendOpParaDebug(node_adr, 0, APPKEY_DEL, (u8 *)&set, sizeof(set.appkey));
}

int cfg_cmd_ak_bind(u16 node_adr, u16 ele_adr, u16 ak_idx, u32 md_id, bool4 sig_model)
{
    node_adr = get_primary_adr_with_check(node_adr, ele_adr);
    
	mesh_app_bind_t set;
	set.ele_adr = ele_adr;
	set.index = ak_idx;
	set.model_id = md_id;
	u32 len = sizeof(set);
	if(sig_model){
		len -= 2;
	}
	return SendOpParaDebug(node_adr, 0, MODE_APP_BIND, (u8 *)&set, len);
}

int cfg_cmd_subnet_bridge_get(u16 dst_addr)
{
	return SendOpParaDebug(dst_addr, 0, SUBNET_BRIDGE_GET, 0, 0);	
}

int cfg_cmd_subnet_bridge_set(u16 dst_addr, u8 en)
{
	return SendOpParaDebug(dst_addr, 0, SUBNET_BRIDGE_SET, &en, 1);
}

int cfg_cmd_bridge_table_get(u16 dst_addr, u16 key_idx1, u16 key_idx2, u16 start_idx)
{
	bridging_tbl_get_t tbl_get;
	tbl_get.netkey_index1 = key_idx1;
	tbl_get.netkey_index2 = key_idx2;
	tbl_get.start_index = start_idx;
	return SendOpParaDebug(dst_addr, 0, BRIDGING_TABLE_GET, (u8 *)&tbl_get, sizeof(tbl_get));
}

int cfg_cmd_bridge_table_add(u16 dst_addr, u8 direction, u16 key_idx1, u16 key_idx2, u16 addr1, u16 addr2)
{
	mesh_bridge_entry_t entry;
	entry.directions = direction;
	entry.netkey_index1 = key_idx1;
	entry.netkey_index2 = key_idx2;
	entry.addr1 = addr1;
	entry.addr2 = addr2;
	
	return SendOpParaDebug(dst_addr, 0, BRIDGING_TABLE_ADD, (u8 *)&entry, sizeof(entry));
}


int cfg_cmd_bridge_table_remove(u16 dst_addr, u16 key_idx1, u16 key_idx2, u16 addr1, u16 addr2)
{
	bridging_tbl_remove_t tbl_remove;
	tbl_remove.netkey_index1 = key_idx1;
	tbl_remove.netkey_index2 = key_idx2;
	tbl_remove.addr1 = addr1;
	tbl_remove.addr2 = addr2;
	
	return SendOpParaDebug(dst_addr, 0, BRIDGING_TABLE_REMOVE, (u8 *)&tbl_remove, sizeof(tbl_remove));
}

int cfg_cmd_bridge_subnet_get(u16 dst_addr, u8 filter, u16 netkey_idx, u16 start_idx)
{
	bridged_subnets_get_t subnet_get;
	subnet_get.filter = filter;
	subnet_get.netkey_index = netkey_idx;
	subnet_get.start_index = start_idx;	
	
	return SendOpParaDebug(dst_addr, 0, BRIDGED_SUBNETS_GET, (u8 *)&subnet_get, sizeof(subnet_get));
}

int cfg_cmd_bridge_tbl_size_get(u16 dst_addr)
{	
	return SendOpParaDebug(dst_addr, 0, BRIDGE_CAPABILITY_GET, 0, 0);
}

int cfg_cmd_on_demand_private_proxy_get(u16 dst_addr)
{
	return SendOpParaDebug(dst_addr, 0, CFG_ON_DEMAND_PROXY_GET, 0, 0);
}

int cfg_cmd_on_demand_private_proxy_set(u16 dst_addr, u8 en)
{
	return SendOpParaDebug(dst_addr, 0, CFG_ON_DEMAND_PROXY_SET, &en, 1);
}

int cfg_cmd_sar_transmitter_get(u16 dst_addr)
{
	return SendOpParaDebug(dst_addr, 0, CFG_SAR_TRANSMITTER_GET, 0, 0);
}

int cfg_cmd_sar_transmitter_set(u16 dst_addr, u8 seg_invl_step, u8 unicast_retran_cnt, u8 unicast_retran_cnt_noack, u8 unicast_retran_invl_step, u8 unicast_retran_invl_inc, u8 multicast_retran_cnt, u8 multicast_retran_invl)
{
	sar_transmitter_t sar;
	sar.sar_seg_invl_step = seg_invl_step;
	sar.sar_uni_retrans_cnt = unicast_retran_cnt;
	sar.sar_uni_retrans_cnt_no_ack = unicast_retran_cnt_noack;
	sar.sar_uni_retrans_invl_step = unicast_retran_invl_step;
	sar.sar_uni_retrans_invl_incre = unicast_retran_invl_inc;
	sar.sar_multi_retrans_cnt = multicast_retran_cnt;
	sar.sar_multi_retrans_invl = multicast_retran_invl;
	return SendOpParaDebug(dst_addr, 0, CFG_SAR_TRANSMITTER_SET, (u8 *)&sar, sizeof(sar));
}

int cfg_cmd_sar_receiver_get(u16 dst_addr)
{
	return SendOpParaDebug(dst_addr, 0, CFG_SAR_RECEIVER_GET, 0, 0);
}

int cfg_cmd_sar_receiver_set(u16 dst_addr, u8 seg_threshold, u8 ack_delay_inc, u8 discard_timeout, u8 rcv_seg_invl_step, u8 ack_retrans_cnt)
{
	sar_receiver_t sar;
	sar.sar_seg_thres = seg_threshold;
	sar.sar_ack_delay_inc = ack_delay_inc;
	sar.sar_discard_timeout = discard_timeout;
	sar.sar_rcv_seg_invl_step = rcv_seg_invl_step;
	sar.sar_ack_retrans_cnt = ack_retrans_cnt;
	return SendOpParaDebug(dst_addr, 0, CFG_SAR_RECEIVER_SET, (u8 *)&sar, sizeof(sar));
}

int cfg_cmd_soli_pdu_rpl_clear(u16 dst_addr, u16 start_addr, u8 range_len, int ack)
{
	addr_range_t addr;
	addr.length_present_l = 0;
	addr.range_start_l = start_addr;
	if(range_len > 1){
		addr.range_start_l = start_addr;
		addr.length_present_l = 1;
		addr.range_length = range_len;
	}
	return  SendOpParaDebug(dst_addr, 0, ack?SOLI_PDU_RPL_ITEM_CLEAR:SOLI_PDU_RPL_ITEM_CLEAR_NACK, (u8 *)&addr, addr.length_present_l?3:2);
}

int cfg_cmd_op_agg_sequence(u16 dst_addr, u8 *para, int len)
{
	return SendOpParaDebug(dst_addr, 0, CFG_OP_AGG_SEQ, para, len);
}

int cfg_cmd_heartbeat_pub_set(u16 dst_addr, u16 pub_addr, u8 count_log, u8 period_log, u8 ttl, u16 features, u16 netkey_index)
{
	mesh_cfg_model_heartbeat_pub_set_t heartbeat_pub;
	heartbeat_pub.dst = pub_addr;
	heartbeat_pub.count_log = count_log;
	heartbeat_pub.period_log = period_log;
	heartbeat_pub.ttl = ttl;
	heartbeat_pub.features = features;
	heartbeat_pub.netkeyindex = netkey_index;
	return SendOpParaDebug(dst_addr, 1, HEARTBEAT_PUB_SET, (u8 *)&heartbeat_pub, sizeof(heartbeat_pub));
}
//------------------
int mesh_proxy_set_filter_cmd(u8 opcode,u8 filter_type, u8* dat,u8 len )
{
	if(opcode == PROXY_FILTER_SET_TYPE){
		len =1;// get the first byte of the dat buf 
		dat[0]= filter_type;
	}else if (opcode == PROXY_FILTER_ADD_ADR || opcode == PROXY_FILTER_RM_ADR){
		if(len %2 !=0){
			len = len -1;
		}
		for(int i=0;i<len/2;i++){
			endianness_swap_u16(dat+2*i);
		}
	}
	#if WIN32
	LOG_MSG_INFO(TL_LOG_NODE_BASIC,dat,len ,"filter send cmd is %d",opcode);
	#endif
	return mesh_tx_cmd_layer_cfg_primary(opcode,dat,len ,PROXY_CONFIG_FILTER_DST_ADR);
}
int mesh_proxy_set_filter_type(u8 filter_type)
{
	u8 dat=0;
	return mesh_proxy_set_filter_cmd(PROXY_FILTER_SET_TYPE,filter_type,&dat,0);
}
int mesh_proxy_filter_add_adr(u16 adr)
{
	return mesh_proxy_set_filter_cmd(PROXY_FILTER_ADD_ADR,0,(u8 *)&adr,sizeof(adr));
}

int mesh_proxy_filter_remove_adr(u16 adr)
{
	return mesh_proxy_set_filter_cmd(PROXY_FILTER_RM_ADR,0,(u8 *)&adr,sizeof(adr));
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
	return mesh_tx_cmd_layer_cfg_primary(DIRECTED_PROXY_CONTROL,(u8 *)&proxy_ctl, par_len,PROXY_CONFIG_FILTER_DST_ADR);;
}

int mesh_proxy_set_filter_init(u16 self_adr)
{
	// add the own white list to the list part 
	#if VC_APP_ENABLE
	connect_addr_gatt = 0;
	#endif
	
	mesh_proxy_set_filter_type(FILTER_WHITE_LIST);
	#if 1
	// because no enough notify buffer for LPN to report filter status in some cases when LPN is in extend adv mode.
	// LPN have only 5 buffer, and kma dongle has no DLE function, so it need 6 buffers to respon filter status.
	u16 addr_little[2] = {0};
	addr_little[0] = self_adr;
	addr_little[1] = 0xffff;
	return mesh_proxy_set_filter_cmd(PROXY_FILTER_ADD_ADR,0,(u8 *)addr_little,sizeof(addr_little));
	#else
	mesh_proxy_filter_add_adr(self_adr);
	return mesh_proxy_filter_add_adr(0xffff);
	#endif
}

#if (MD_SOLI_PDU_RPL_EN&&MD_CLIENT_EN)
int mesh_send_proxy_solicitation_pdu(u16 adr_dst)
{
	mesh_cmd_bear_t cmd_bear;
	mesh_cmd_nw_t *p_nw = &cmd_bear.nw;
	p_nw->ivi = 0;
	p_nw->ctl = 1;
	p_nw->ttl = 0;
	p_nw->src = ele_adr_primary;
	p_nw->dst = adr_dst;
	memcpy(p_nw->sno, &soli_sno_tx, sizeof(p_nw->sno));
	soli_sno_tx++;
	mesh_sec_msg_enc_nw((u8 *)p_nw, 0, SWAP_TYPE_LT_UNSEG, MASTER, 0, 0, MESH_ADV_TYPE_MESSAGE, NONCE_TYPE_SOLICITATION, 0, 0);
	soli_service_data.id_type = SOLI_WITH_RPL_TYPE;
	memcpy(soli_service_data.id_para, p_nw, sizeof(soli_service_data.id_para));
	soli_pdu_adv_cnt = SOLICI_PDU_CNT;
	return 1;
}
#endif

int cfg_cmd_send_path_solicitation(u16 netkey_offset, u16 *addr_list, int num)
{
	return mesh_tx_cmd_layer_upper_ctl_primary_specified_key(CMD_CTL_PATH_REQUEST_SOLICITATION, (u8 *)addr_list, num<<1, ADR_ALL_DIRECTED_FORWARD, netkey_offset);
}

#if WIN32
// json data file ,to get data interface part  
// provision end callback ,can call this interface to get data 
void json_get_net_info(u8 *p_netkey_val,u16 *p_netkey_idx,u8 *p_ivi,u8 *ivi_key_flag,u16 *unicast)
{
	provison_net_info_str *p_net_info = (provison_net_info_str *)gatt_pro_dat;
	memcpy(p_netkey_val,p_net_info->net_work_key,sizeof(p_net_info->net_work_key));
	*(p_netkey_idx) = p_net_info->key_index;
	memcpy(p_ivi,p_net_info->iv_index,sizeof(p_net_info->iv_index));
	*(ivi_key_flag) = p_net_info->flags;
	*(unicast)= p_net_info->unicast_address; 
	return ;
}
void json_get_dev_key_info(u8 * dev_key)
{
	memcpy(dev_key,gatt_dev_key,sizeof(gatt_dev_key));
	return ;
}


// keybind end callback ,call the interface part 
void json_get_appkey_info(u8 *appkey_val,u16 *appkey_idx)
{
	*(appkey_idx) = key_refresh_cfgcl_proc.ak_idx;
	memcpy(appkey_val ,key_refresh_cfgcl_proc.ak,sizeof(key_refresh_cfgcl_proc.ak));
}
u8 json_get_node_cps_info(u16 unicast_adr,VC_node_info_t *p_node)
{
	if(!is_unicast_adr(unicast_adr) && (!unicast_adr)){
		return -1;
	}
	for(int i=0;i<MESH_NODE_MAX_NUM;i++){
		if(VC_node_info[i].node_adr == unicast_adr){
			memcpy(p_node,&(VC_node_info[i]),sizeof(VC_node_info_t));	
			return i;
		}
	}
	return -2;
}
#endif


