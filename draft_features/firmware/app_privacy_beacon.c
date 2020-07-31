/********************************************************************************************************
 * @file     app_privacy_beacon.c 
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

#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj_lib/ble/blt_config.h"
#include "app_beacon.h"
#include "../../vendor/common/app_provison.h"
#include "../../proj/common/types.h"
#include "app_privacy_beacon.h"

#if MD_PRIVACY_BEA
model_private_beacon_t model_private_beacon;
u32 mesh_md_pri_beacon_addr = FLASH_ADR_MD_PRIVATE_BEACON;

void mesh_prov_para_random_generate()
{
#if !WIN32
	generateRandomNum(sizeof(prov_para.priv_random),prov_para.priv_random);
	prov_para.priv_rand_gen_s = clock_time_s();
#endif
}


void mesh_private_proxy_change_by_gatt_proxy(u8 private_sts,u8 *p_private_proxy)
{
	u8 proxy_sts = model_sig_cfg_s.gatt_proxy;
	if(!PRIVATE_PROXY_FUN_EN){
		*p_private_proxy = PRIVATE_PROXY_NOT_SUPPORT;
	}
	if(proxy_sts == GATT_PROXY_NOT_SUPPORT){
		*p_private_proxy = PRIVATE_PROXY_NOT_SUPPORT;
	}else if (proxy_sts == GATT_PROXY_SUPPORT_ENABLE){
		*p_private_proxy = PRIVATE_PROXY_DISABLE;
	}else if (proxy_sts == GATT_PROXY_SUPPORT_DISABLE){
		*p_private_proxy = private_sts;
	}
}

int mesh_private_cmd_st_rsp(u16 op_code,u8 *p_rsp,u8 len ,mesh_cb_fun_par_t *cb_par)
{
    model_private_beacon_ser_t *p_model = (model_private_beacon_ser_t *)cb_par->model;
    return mesh_tx_cmd_rsp(op_code,p_rsp,len,p_model->com.ele_adr,cb_par->adr_src,0,0);
}

void mesh_update_node_identity_val()
{
	foreach(i,NET_KEY_MAX){
		mesh_net_key_t *p_netkey_base = &mesh_key.net_key[i][0];
		if(!p_netkey_base->valid){
			continue;
		}
		u8 key_phase = p_netkey_base->key_phase;
		if(KEY_REFRESH_PHASE2 == key_phase){
			p_netkey_base += 1;		// use new key
		}
		caculate_proxy_adv_hash(p_netkey_base);
	}
}
void mesh_node_identity_refresh()
{
#if !WIN32
	prov_para.rand_gen_s = clock_time_s();
	generateRandomNum(sizeof(prov_para.random),prov_para.random);
	// need to use the new random to calculate the node_identity part 
	mesh_update_node_identity_val();	
#endif
}

void mesh_tx_sec_privacy_beacon(mesh_net_key_t *p_nk_base, u8 blt_sts)
{
#if !WIN32
	u8 key_phase = p_nk_base->key_phase;
    mesh_net_key_t *p_netkey = p_nk_base;
	if(KEY_REFRESH_PHASE2 == key_phase){
		p_netkey += 1;		// use new key
	}
	u8 pri_key_flag;
	u8 pri_ivi[4];
	get_iv_update_key_refresh_flag((mesh_ctl_fri_update_flag_t *)&pri_key_flag, pri_ivi, key_phase);
	mesh_cmd_bear_unseg_t bc_bear;
    memset(&bc_bear, 0, sizeof(bc_bear));
    bc_bear.type = MESH_ADV_TYPE_BEACON;
    bc_bear.len = 0x1c; // 1+1+sizeof(privacy_beacon)
	bc_bear.beacon.type = PRIVACY_BEACON;
	mesh_sec_pri_beacon(pri_key_flag,pri_ivi,prov_para.priv_random,p_netkey->prik,bc_bear.beacon.data);
	if(blt_sts){
		notify_pkts((u8 *)(&bc_bear.beacon.type),sizeof(mesh_beacon_privacy_t)+1,GATT_PROXY_HANDLE,MSG_MESH_BEACON);
	}else{
    	mesh_bear_tx2mesh_and_gatt((u8 *)&bc_bear, MESH_ADV_TYPE_BEACON, TRANSMIT_DEF_PAR_BEACON);
	}
#endif

}
static auth_tag_cache_t auth_cache;
u8 private_beacon_auth_cache_proc(u8 *p_auth)
{
	// compare with all the cache part 
	u8 *auth_tag;
	for(int i=0;i<MAX_SUPPORT_AUTH_TAG_CNT;i++){
		auth_tag = &(auth_cache.auth_tag[i][0]);
		if(!memcmp(auth_tag,p_auth,8)){
			return 1;//find the auth 
		}
	}
	// if not find the auth 
	auth_cache.idx = (auth_cache.idx+1)%MAX_SUPPORT_AUTH_TAG_CNT;
	auth_tag = &(auth_cache.auth_tag[auth_cache.idx][0]);// find the pointer
	memcpy(auth_tag,p_auth,8);
	return 0;// not find ,and add the tag into the cache auth part 
}


int mesh_rc_data_beacon_privacy(u8 *p_payload, u32 t)
{
	int err = 0;
    mesh_cmd_bear_unseg_t *bc_bear = GET_BEAR_FROM_ADV_PAYLOAD(p_payload);
    mesh_beacon_t *p_bc = &bc_bear->beacon;
	LOG_MSG_INFO(TL_LOG_COMMON, 0, 0, "rcv private beacon", 0);
    if(is_lpn_support_and_en && is_in_mesh_friend_st_lpn()){
        return 0;    // LPN should not receive beacon msg when in friend state.
    }
    mesh_beacon_privacy_t *p_sec_privacy = (mesh_beacon_privacy_t *)(p_bc->data);
	if(private_beacon_auth_cache_proc(p_sec_privacy->auth_tag)){
		return 0; // the auth tag have already exist .
	}
	u8 key_flag,ivi_idx[4];
    err = mesh_privacy_beacon_dec((u8 *)p_sec_privacy,&key_flag,ivi_idx);
    if(err){
		LOG_MSG_INFO(TL_LOG_COMMON, 0, 0, "check private beacon fail", 0);
		return 100;
	}// decrypt error 
	LOG_MSG_INFO(TL_LOG_COMMON, ivi_idx, sizeof(ivi_idx), "check suc,key flag is %x", key_flag);
    return iv_update_key_refresh_rx_handle((mesh_ctl_fri_update_flag_t *)&key_flag,ivi_idx);
}

#if MD_SERVER_EN
void mesh_private_para_init()
{
	model_private_beacon_ser_t *p_beacon_ser = model_private_beacon.srv;
	p_beacon_ser->beacon_sts = PRIVATE_BEACON_DISABLE;
	mesh_private_proxy_change_by_gatt_proxy(GATT_PROXY_SUPPORT_ENABLE,&(p_beacon_ser->proxy_sts));
	p_beacon_ser->random_inv_step = 0x3c;// 10 min
	mesh_prov_para_random_generate();
}

int mesh_cmd_sig_beacon_sts_rsp(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	model_private_beacon_ser_t *p_pri_server = &(model_private_beacon.srv[0]);
	private_beacon_str_t  pri_beacon;
	pri_beacon.beacon_sts = p_pri_server->beacon_sts;
	pri_beacon.random_inv_step = p_pri_server->random_inv_step;
    return mesh_private_cmd_st_rsp(PRIVATE_BEACON_STATUS,(u8 *)(&pri_beacon),sizeof(private_beacon_str_t),cb_par);
}

int mesh_cmd_sig_beacon_sts(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	return err;
}

int mesh_cmd_sig_beacon_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	return mesh_cmd_sig_beacon_sts_rsp(par,par_len,cb_par);
}

//u8 special_PRB_BV01 =0;
int mesh_cmd_sig_beacon_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	// the random_inv_step iss optional 
	private_beacon_str_t *p_pri_beacon = (private_beacon_str_t *)par;
	model_private_beacon_ser_t *p_pri_server = (model_private_beacon.srv);
	if(p_pri_beacon->beacon_sts <= PRIVATE_BEACON_ENABLE && par_len <= sizeof(private_beacon_str_t)){
		if(par_len == sizeof(private_beacon_str_t)){
			//special_PRB_BV01 = 0;
			p_pri_server->random_inv_step = p_pri_beacon->random_inv_step;
		}else{
			//special_PRB_BV01 =1;
		}
		p_pri_server->beacon_sts = p_pri_beacon->beacon_sts;
	}else{
		return -1;// the value is prohibit
	}
    return mesh_cmd_sig_beacon_sts_rsp(par,par_len,cb_par);
}




int mesh_cmd_sig_gatt_proxy_sts_rsp(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	model_private_beacon_ser_t *p_pri_server = (model_private_beacon.srv);
	private_gatt_proxy_str_t gatt_proxy;
	gatt_proxy.proxy_sts = p_pri_server->proxy_sts;
    return mesh_private_cmd_st_rsp(PRIVATE_GATT_PROXY_STATUS,(u8 *)(&gatt_proxy),sizeof(private_gatt_proxy_str_t),cb_par);
}

int mesh_cmd_sig_gatt_proxy_sts(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	return err;
}

int mesh_cmd_sig_gatt_proxy_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	return mesh_cmd_sig_gatt_proxy_sts_rsp(par,par_len,cb_par);
}

int mesh_cmd_sig_gatt_proxy_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	model_private_beacon_ser_t *p_pri_server = (model_private_beacon.srv);
	private_gatt_proxy_str_t *p_gatt_proxy = (private_gatt_proxy_str_t *)par;
	if(p_gatt_proxy->proxy_sts >= PRIVATE_PROXY_PROHIBIT){// ignore invalid parameters 
		return -1;
	}
	mesh_private_proxy_change_by_gatt_proxy(p_gatt_proxy->proxy_sts,&(p_pri_server->proxy_sts));
	return mesh_cmd_sig_gatt_proxy_sts_rsp(par,par_len,cb_par);
}

int mesh_cmd_sig_private_node_identity_sts(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	return err;
	
}

int mesh_cmd_sig_private_node_identity_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	private_node_identity_get_str_t* p_node_identity = (private_node_identity_get_str_t*)par;
	if(p_node_identity->netkey_idx >=0x8000){
		return -1;
	}
	mesh_net_key_t* p_net = mesh_get_netkey_by_idx(p_node_identity->netkey_idx);
	private_node_identity_sts_str_t identi_sts;
	identi_sts.netkey_idx = p_node_identity->netkey_idx;	
	if(p_net == NULL){
		identi_sts.sts = MESH_CFG_STS_INVALID_NETKEY_IND;
		identi_sts.private_identity = PRIVATE_NODE_IDENTITY_DISABLE;
	}else{
		identi_sts.sts = MESH_CFG_STS_SUCCESS;
		identi_sts.private_identity = p_net->priv_identity;
	}
	
	return mesh_private_cmd_st_rsp(PRIVATE_NODE_IDENTITY_STATUS,(u8 *)(&identi_sts),sizeof(identi_sts),cb_par);
}

int mesh_cmd_sig_private_node_identity_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	private_node_identity_set_str_t* p_node_identity = (private_node_identity_set_str_t*)par;
	if(p_node_identity->netkey_idx >=0x8000){
		return -1;
	}
	mesh_net_key_t* p_net = mesh_get_netkey_by_idx(p_node_identity->netkey_idx);
	private_node_identity_sts_str_t identi_sts;
	identi_sts.netkey_idx = p_node_identity->netkey_idx;
	if(p_net == NULL){
		identi_sts.sts = MESH_CFG_STS_INVALID_NETKEY_IND;
		identi_sts.private_identity = PRIVATE_NODE_IDENTITY_DISABLE;
	}else{
		
		if(p_node_identity->private_identity <= PRIVATE_NODE_IDENTITY_NOT_SUPPORT){
			if(	p_net->priv_identity != PRIVATE_NODE_IDENTITY_NOT_SUPPORT &&
				p_node_identity->private_identity == PRIVATE_NODE_IDENTITY_NOT_SUPPORT){
				return -1;
			}
			p_net->priv_identity = p_node_identity->private_identity;
			if(p_net->priv_identity == PRIVATE_NODE_IDENTITY_ENABLE){
				p_net->priv_identity_s = clock_time_s();
			}
			mesh_key_save();
		}else{
			return -1;
		}
		identi_sts.sts = MESH_CFG_STS_SUCCESS;
		identi_sts.private_identity = p_node_identity->private_identity;
	}
	
	return mesh_private_cmd_st_rsp(PRIVATE_NODE_IDENTITY_STATUS,(u8 *)(&identi_sts),sizeof(identi_sts),cb_par);
}



void mesh_prov_para_random_proc()
{
#if !WIN32
	model_private_beacon_ser_t *p_beacon_ser = model_private_beacon.srv;
	if(	is_provision_success()){
		if(clock_time_exceed_s(prov_para.priv_rand_gen_s,p_beacon_ser->random_inv_step*10)){
			mesh_prov_para_random_generate();// update priv_random ,and the calculate will excute every time 
		}
		// update the random for about 10min 
		if(clock_time_exceed_s(prov_para.rand_gen_s,600)){
			mesh_node_identity_refresh();
		}
	}
#endif
}
/*when we need to send the private beacon by the gatt connection ,weshould follow
Proxy Privacy state :
	When both the GATT Proxy state and the Node Identity state are disabled, and either the Private GATT
	Proxy state or the Private Node Identity state is enabled, the Proxy Privacy parameter (see Section 6.5)
	for the connection shall be Enabled.
*/

int mesh_tx_private_beacon_enable(mesh_net_key_t *p_netkey)
{
	model_private_beacon_ser_t *p_pri_server = (model_private_beacon.srv);
	if(p_netkey->node_identity == NODE_IDENTITY_SUB_NET_STOP && 
		model_sig_cfg_s.gatt_proxy == PRIVATE_PROXY_DISABLE){
		if(p_pri_server->proxy_sts == PRIVATE_PROXY_ENABLE ||
			p_netkey->priv_identity == PRIVATE_NODE_IDENTITY_ENABLE){
			return 1;
		}
	}
	return 0;
}


void mesh_tx_privacy_nw_beacon_all_net(u8 blt_sts)
{
	
	model_private_beacon_ser_t *p_beacon_ser = &(model_private_beacon.srv[0]);
	if(p_beacon_ser->beacon_sts == PRIVATE_BEACON_ENABLE){
		foreach(i,NET_KEY_MAX){
			mesh_net_key_t *p_netkey_base = &mesh_key.net_key[i][0];
			if(!p_netkey_base->valid){
				continue;
			}
			// if the random_inv_step is 0 ,the beacon should change every time .
			if(p_beacon_ser->random_inv_step == 0){
				mesh_prov_para_random_generate();// update priv_random ,and the calculate will excute every time 
			}
			if(blt_sts){
				// in the connection state ,it will not send the private beacon in the connection state 
				if(!mesh_tx_private_beacon_enable(p_netkey_base)){
					continue;
				}
			}
			mesh_tx_sec_privacy_beacon(p_netkey_base, blt_sts);
		}
		#if 0
		if(special_PRB_BV01 && p_beacon_ser->random_inv_step != 0){
			p_beacon_ser->beacon_sts = PRIVATE_BEACON_DISABLE;
		}
		#endif
	}
}
	#endif
#endif

