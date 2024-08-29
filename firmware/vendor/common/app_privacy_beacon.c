/********************************************************************************************************
 * @file	app_privacy_beacon.c
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
#include "tl_common.h"
#include "proj_lib/ble/blt_config.h"
#include "app_beacon.h"
#include "vendor/common/app_provison.h"
#include "app_privacy_beacon.h"

#if MD_PRIVACY_BEA
#if !WIN32
void generate_non_resolvable_mac(u8*p_mac)
{
	generateRandomNum(6,p_mac);	
	p_mac[5]=p_mac[5] & 0x3f;// make sure the three most significant bit to 0. // MSB 2bit: non_resolvable: 0b00, resolvable: 0b01.
}
#endif

void mesh_prov_para_random_generate()
{
#if !WIN32
	generateRandomNum(sizeof(prov_para.priv_random),prov_para.priv_random);
	generate_non_resolvable_mac(prov_para.priv_non_resolvable);
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
    model_private_beacon_srv_t *p_model = (model_private_beacon_srv_t *)cb_par->model;
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
		
		calculate_proxy_adv_hash(p_netkey_base);
	}
}
void mesh_node_identity_refresh_private()
{
#if !WIN32
	prov_para.rand_gen_s = clock_time_s();
	generateRandomNum(sizeof(prov_para.random),prov_para.random);
	// need to use the new random to calculate the node_identity part 
	mesh_update_node_identity_val();	
#endif
}

int mesh_tx_sec_privacy_beacon(mesh_net_key_t *p_nk_base, u8 blt_sts)
{
	int err = 0;
#if !WIN32
	u8 key_phase = p_nk_base->key_phase;
    mesh_net_key_t *p_netkey = p_nk_base;
	if(KEY_REFRESH_PHASE2 == key_phase){
		p_netkey += 1;		// use new key
	}
	u8 pri_key_flag=0;
	u8 pri_ivi[4];
	get_iv_update_key_refresh_flag((mesh_ctl_fri_update_flag_t *)&pri_key_flag, pri_ivi, key_phase);
	mesh_cmd_bear_t bc_bear;
    memset(&bc_bear, 0, sizeof(bc_bear));
    bc_bear.type = MESH_ADV_TYPE_BEACON;
    bc_bear.len = 0x1c; // 1+1+sizeof(privacy_beacon)
	bc_bear.beacon.type = PRIVACY_BEACON;
	#if 0 // print_tx_security_beacon_test_en
	LOG_MSG_LIB(TL_LOG_NODE_SDK,pri_ivi, 4,"send private beacon discovery mode: %d, flag: %02x, \r\n", iv_idx_st.searching_flag, pri_key_flag);
	#endif
	mesh_sec_pri_beacon(pri_key_flag,pri_ivi,prov_para.priv_random,p_netkey->prik,bc_bear.beacon.data);
	if(blt_sts){
		__UNUSED u8 conn_handle = BLS_HANDLE_MIN;
		#if BLE_MULTIPLE_CONNECTION_ENABLE
		for(u16 conn_handle=BLS_HANDLE_MIN; conn_handle<BLS_HANDLE_MAX; conn_handle++){
			if(blc_ll_isAclConnEstablished(conn_handle)){
		#endif
				err = notify_pkts(conn_handle, (u8 *)(&bc_bear.beacon.type),sizeof(mesh_beacon_privacy_t)+1,GATT_PROXY_HANDLE,MSG_MESH_BEACON);
		#if BLE_MULTIPLE_CONNECTION_ENABLE
			}
		}
		#endif
	}else{
    	err = mesh_bear_tx_beacon_adv_channel_only((u8 *)&bc_bear, TRANSMIT_PAR_SECURITY_BEACON);
	}
#endif

    return err;
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

/**
 * @brief       This function server to decrypt and process private beacon.
 * @param[in]   p_payload	- pointer of private beacon.
 * @param[in]   t			- unused.
 * @return      0: success. other: failure.
 * @note        
 */
int mesh_rc_data_beacon_privacy(u8 *p_payload, u32 t)
{
	int err = 0;
    mesh_cmd_bear_t *bc_bear = GET_BEAR_FROM_ADV_PAYLOAD(p_payload);
    mesh_beacon_t *p_bc = &bc_bear->beacon;
	LOG_MSG_INFO(TL_LOG_COMMON, 0, 0, "rcv private beacon");
    if(is_lpn_support_and_en && is_in_mesh_friend_st_lpn()){
        return 0;    // LPN should not receive beacon msg when in friend state.
    }
    
    #if 1
    mesh_beacon_privacy_t backup_rc;
    memcpy(&backup_rc, p_bc->data, sizeof(backup_rc)); // backup to make sure it can not be over written.
    mesh_beacon_privacy_t *p_sec_privacy = &backup_rc;
    #else
    mesh_beacon_privacy_t *p_sec_privacy = (mesh_beacon_privacy_t *)(p_bc->data);
    #endif
	if(private_beacon_auth_cache_proc(p_sec_privacy->auth_tag)){
		return 0; // the auth tag have already exist .
	}
	u8 key_flag,ivi_idx[4];
    err = mesh_privacy_beacon_dec((u8 *)p_sec_privacy,&key_flag,ivi_idx);
	#if 0 // print_tx_security_beacon_test_en
	LOG_MSG_LIB(TL_LOG_NODE_SDK,ivi_idx, 4,"rcv private beacon %02x\r\n",key_flag);
	#endif
    if(err){
		LOG_MSG_INFO(TL_LOG_COMMON, 0, 0, "check private beacon fail");
		return 100;
	}// decrypt error 
	LOG_MSG_INFO(TL_LOG_COMMON, ivi_idx, sizeof(ivi_idx), "check suc,key flag is %x", key_flag);
    return iv_update_key_refresh_rx_handle((mesh_ctl_fri_update_flag_t *)&key_flag,ivi_idx);
}

#if MD_SERVER_EN
void mesh_private_proxy_sts_init()
{
	mesh_privacy_beacon_save_t *p_beacon_srv = &g_mesh_model_misc_save.privacy_bc;
	mesh_private_proxy_change_by_gatt_proxy(p_beacon_srv->proxy_sts, &(p_beacon_srv->proxy_sts)); // check Binding with GATT Proxy.
	mesh_prov_para_random_generate();
}

int mesh_cmd_sig_beacon_sts_rsp(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	mesh_privacy_beacon_save_t *p_pri_server = &g_mesh_model_misc_save.privacy_bc;
	private_beacon_str_t  pri_beacon;
	pri_beacon.beacon_sts = p_pri_server->beacon_sts;
	pri_beacon.random_inv_step = p_pri_server->random_inv_step;
    return mesh_private_cmd_st_rsp(PRIVATE_BEACON_STATUS,(u8 *)(&pri_beacon),sizeof(private_beacon_str_t),cb_par);
}

int mesh_cmd_sig_beacon_sts(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err = 0;
	return err;
}

int mesh_cmd_sig_beacon_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
#if 0
	mesh_privacy_beacon_save_t *p_pri_server = &g_mesh_model_misc_save.privacy_bc;
	private_beacon_str_t  pri_beacon;
	if(p_pri_server->random_inv_step){
		pri_beacon.beacon_sts = 0;
	}else{
		pri_beacon.beacon_sts = p_pri_server->beacon_sts;
	}
	
	pri_beacon.random_inv_step = p_pri_server->random_inv_step;
    return mesh_private_cmd_st_rsp(PRIVATE_BEACON_STATUS,(u8 *)(&pri_beacon),sizeof(private_beacon_str_t),cb_par);
#else
	return mesh_cmd_sig_beacon_sts_rsp(par,par_len,cb_par);
#endif
}

//u8 special_PRB_BV01 =0;
int mesh_cmd_sig_beacon_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	// the random_inv_step iss optional 
	private_beacon_str_t *p_pri_beacon = (private_beacon_str_t *)par;
	mesh_privacy_beacon_save_t *p_pri_server = &g_mesh_model_misc_save.privacy_bc;
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
	
	mesh_privacy_beacon_save();
    return mesh_cmd_sig_beacon_sts_rsp(par,par_len,cb_par);
}




int mesh_cmd_sig_gatt_proxy_sts_rsp(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	mesh_privacy_beacon_save_t *p_pri_server = &g_mesh_model_misc_save.privacy_bc;
	private_gatt_proxy_str_t gatt_proxy;
	gatt_proxy.proxy_sts = p_pri_server->proxy_sts;
    return mesh_private_cmd_st_rsp(PRIVATE_GATT_PROXY_STATUS,(u8 *)(&gatt_proxy),sizeof(private_gatt_proxy_str_t),cb_par);
}

int mesh_cmd_sig_gatt_proxy_sts(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err = 0;
	return err;
}

int mesh_cmd_sig_gatt_proxy_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	return mesh_cmd_sig_gatt_proxy_sts_rsp(par,par_len,cb_par);
}

int mesh_cmd_sig_gatt_proxy_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	mesh_privacy_beacon_save_t *p_pri_server = &g_mesh_model_misc_save.privacy_bc;
	private_gatt_proxy_str_t *p_gatt_proxy = (private_gatt_proxy_str_t *)par;
	if(p_gatt_proxy->proxy_sts >= PRIVATE_PROXY_PROHIBIT){// ignore invalid parameters 
		return -1;
	}
	mesh_private_proxy_change_by_gatt_proxy(p_gatt_proxy->proxy_sts,&(p_pri_server->proxy_sts));
	mesh_privacy_beacon_save();
	return mesh_cmd_sig_gatt_proxy_sts_rsp(par,par_len,cb_par);
}

int mesh_cmd_sig_private_node_identity_sts(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err = 0;
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
	mesh_privacy_beacon_save_t *p_beacon_srv = &g_mesh_model_misc_save.privacy_bc;
	if(	is_provision_success()){
		if(clock_time_exceed_s(prov_para.priv_rand_gen_s,p_beacon_srv->random_inv_step*10)){
			mesh_prov_para_random_generate();// update priv_random ,and the calculate will execute every time 
		}
		// update the random for about 10min 
		if(clock_time_exceed_s(prov_para.rand_gen_s,600)){
			mesh_node_identity_refresh_private();
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
	mesh_privacy_beacon_save_t *p_pri_server = &g_mesh_model_misc_save.privacy_bc;
	if(p_netkey->node_identity == NODE_IDENTITY_SUB_NET_STOP && 
		model_sig_cfg_s.gatt_proxy == PRIVATE_PROXY_DISABLE){
		if(p_pri_server->proxy_sts == PRIVATE_PROXY_ENABLE ||
			p_netkey->priv_identity == PRIVATE_NODE_IDENTITY_ENABLE){
			return 1;
		}
	}
	return 0;
}

void mesh_key_add_trigger_beacon_send(u8 idx)
{
	mesh_privacy_beacon_save_t *p_beacon_srv = &g_mesh_model_misc_save.privacy_bc;
	if(p_beacon_srv->beacon_sts == PRIVATE_BEACON_ENABLE){
		mesh_tx_sec_privacy_beacon(&mesh_key.net_key[idx][0], 1);	
	}	
}

int mesh_tx_privacy_nw_beacon_all_net(u8 blt_sts)
{
	int err = 0;    // default success.
	mesh_privacy_beacon_save_t *p_beacon_srv = &g_mesh_model_misc_save.privacy_bc;
	if(!is_provision_success()||MI_API_ENABLE){// if not provisioned it will not send private beacon .
		return err;
	}
	if(blt_sts == 0 && p_beacon_srv->beacon_sts != PRIVATE_BEACON_ENABLE){ // in the adv state
		return 0;
	}
	foreach(i,NET_KEY_MAX){
		mesh_net_key_t *p_netkey_base = &mesh_key.net_key[i][0]; // just to check valid flag here, not determine which key will be used to send beacon.
		if(!p_netkey_base->valid){
			continue;
		}
		if(blt_sts){// in the connect state
			if(!prov_para.privacy_para){
				return err;
			}
		}
		// if the random_inv_step is 0 ,the beacon should change every time .
		if(p_beacon_srv->random_inv_step == 0){
			mesh_prov_para_random_generate();// update priv_random ,and the calculate will execute every time 
		}
		#if TESTCASE_FLAG_ENABLE
			/* in the pts private beacon proxy bv-08c , it should not send 
			two secure beacon on gatt connection , other wise the filter sts will fail*/
			err = mesh_tx_sec_privacy_beacon(p_netkey_base, blt_sts);
			if(blt_sts && beacon_send.conn_beacon_flag ){
				break;
			}
		#else
			err = mesh_tx_sec_privacy_beacon(p_netkey_base, blt_sts);
		#endif

		#if 0
		if(special_PRB_BV01 && p_beacon_srv->random_inv_step != 0){
			p_beacon_srv->beacon_sts = PRIVATE_BEACON_DISABLE;
		}
		#endif
	}

	return err;
}
	#endif
#else
void mesh_prov_para_random_generate() {return;}
#endif

