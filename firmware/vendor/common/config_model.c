/********************************************************************************************************
 * @file	config_model.c
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
#include "config_model.h"
#ifndef WIN32
#include "proj/mcu/watchdog_i.h"
#endif 
#include "proj_lib/ble/ll/ll.h"
#include "proj_lib/ble/blt_config.h"
#include "proj_lib/mesh_crypto/mesh_crypto.h"
#include "vendor/common/user_config.h"
#include "directed_forwarding.h"
#include "app_heartbeat.h"

model_sig_cfg_s_t 		model_sig_cfg_s;  // configuration server model
#if MD_CFG_CLIENT_EN
model_client_common_t   model_sig_cfg_c;
#endif

#if MD_ON_DEMAND_PROXY_EN
u32 mesh_on_demand_proxy_time = 0; // max 256s, clock_time() is enough
#endif

int mesh_sec_prov_comfirmation_key_fun(unsigned char *key, unsigned char *input, int n, unsigned char ecdh[32],unsigned char auth[32],u8 hmac)
{
	#if PROV_EPA_EN
	if(hmac){
		mesh_sec_prov_confirmation_key_hmac(key,input,n,ecdh,auth);
	}else
	#endif
	{
		mesh_sec_prov_confirmation_key(key,input,n,ecdh);
	}
	return 0;
}


int mesh_sec_prov_confirmation_fun(unsigned char *cfm, unsigned char *input, int n, unsigned char ecdh[32],
				unsigned char random[32],unsigned char auth[32],u8 hmac)
{
	#if PROV_EPA_EN
	if(hmac){
		mesh_sec_prov_confirmation_hmac(cfm,input,n,ecdh,random,auth);
	}
	else
	#endif
	{
		mesh_sec_prov_confirmation(cfm,input,n,ecdh,random,auth);
	}
	return 0;
}

void mesh_sec_prov_salt_fun(unsigned char prov_salt[16],unsigned char *input,unsigned char randomProv[32], unsigned char randomDev[32],u8 hmac)
{
	#if PROV_EPA_EN
	if(hmac){
		mesh_sec_prov_salt_hmac(prov_salt,input,randomProv,randomDev);
	}else
	#endif
	{
		mesh_sec_prov_salt(prov_salt,input,randomProv,randomDev);
	}
}

int mesh_sec_prov_session_key_fun(unsigned char sk[16], unsigned char *sn, unsigned char *input, int n, unsigned char ecdh[32],
									unsigned char randomProv[32], unsigned char randomDev[32],u8 hmac)
{
	#if PROV_EPA_EN
	if(hmac){
		mesh_sec_prov_session_key_hmac(sk,sn,input,n,ecdh,randomProv,randomDev);
	}else
	#endif
	{
		mesh_sec_prov_session_key(sk,sn,input,n,ecdh,randomProv,randomDev);
	}
	return 0;
}

u8 mesh_get_network_transmit()
{
	return model_sig_cfg_s.nw_transmit.val;
}

u8 mesh_get_relay_retransmit()
{
	return model_sig_cfg_s.relay_retransmit.val;
}

u8 mesh_get_ttl()
{
	return model_sig_cfg_s.ttl_def;
}

u8 mesh_get_hb_pub_ttl()
{
	return model_sig_cfg_s.hb_pub.ttl;
}

u8 mesh_get_gatt_proxy()
{
	return model_sig_cfg_s.gatt_proxy;
}

u8 mesh_get_friend()
{
	return model_sig_cfg_s.frid;
}

u8 mesh_get_relay()
{
	return model_sig_cfg_s.relay;
}

int is_subscription_adr(model_common_t *p_model, u16 adr)
{
    foreach(i,SUB_LIST_MAX){
        if(adr == p_model->sub_list[i]){
            return 1;
        }
    }
    return 0;
}

int mesh_cmd_sig_cfg_sec_nw_bc_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    return mesh_tx_cmd_rsp_cfg_model(CFG_BEACON_STATUS, &model_sig_cfg_s.sec_nw_beacon, 1, cb_par->adr_src);
}

int mesh_cmd_sig_cfg_sec_nw_bc_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    u8 sec_nw_bc = par[0];
    if(sec_nw_bc < NW_BEACON_ST_MAX){
        model_sig_cfg_s.sec_nw_beacon = sec_nw_bc;
		mesh_model_store_cfg_s();
		
		return mesh_cmd_sig_cfg_sec_nw_bc_get(par, par_len, cb_par);
    } 
	return -1;
}

int mesh_cmd_sig_cfg_def_ttl_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    return mesh_tx_cmd_rsp_cfg_model(CFG_DEFAULT_TTL_STATUS, &model_sig_cfg_s.ttl_def, 1, cb_par->adr_src);
}

int mesh_cmd_sig_cfg_def_ttl_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    u8 ttl = par[0];
    if((ttl <= TTL_MAX) && (ttl != 1)){
        model_sig_cfg_s.ttl_def = ttl;
		mesh_model_store_cfg_s();
		return mesh_cmd_sig_cfg_def_ttl_get(par, par_len, cb_par);
    }
    return  -1;
}

int mesh_cmd_sig_cfg_friend_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    return mesh_tx_cmd_rsp_cfg_model(CFG_FRIEND_STATUS, &model_sig_cfg_s.frid, 1, cb_par->adr_src);
}

int mesh_cmd_sig_cfg_friend_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    u8 frid = par[0];
    if( frid < FRIEND_NOT_SUPPORT){
		if(model_sig_cfg_s.frid < FRIEND_NOT_SUPPORT){
			if(model_sig_cfg_s.frid != frid){
				model_sig_cfg_s.frid = frid;
				if(FRIEND_SUPPORT_DISABLE == frid){
					#if MD_DF_EN
					mesh_directed_forwarding_bind_state_update();					
					mesh_common_store(FLASH_ADR_MD_DF_SBR);
					#endif
					
				    #if FEATURE_FRIEND_EN
				    mesh_friend_ship_init_all();
				    #endif
					
				}
				
				if(model_sig_cfg_s.hb_pub.feature & BIT(MESH_HB_FRI_BIT)){
					hb_sts_change = 1;
					hb_pub_100ms = clock_time_100ms();
				}
	            LOG_MSG_INFO(TL_LOG_FRIEND,0, 0,"friend feature change: %d",par[0]);
			}

			if((0 == pts_test_en) || frid){    // FN-BV10, only clear, no recover, so don't save
			    mesh_model_store_cfg_s();
			}
		}else{
			par[0] = model_sig_cfg_s.frid;
		}
		return mesh_cmd_sig_cfg_friend_get(par, par_len, cb_par);
    }
    return -1;
}

int mesh_cmd_sig_cfg_gatt_proxy_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    return mesh_tx_cmd_rsp_cfg_model(CFG_GATT_PROXY_STATUS, &model_sig_cfg_s.gatt_proxy, 1, cb_par->adr_src);
}

int mesh_cmd_sig_cfg_gatt_proxy_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    u8 gatt_proxy = par[0];
    if(gatt_proxy < GATT_PROXY_NOT_SUPPORT){
		if(model_sig_cfg_s.gatt_proxy < GATT_PROXY_NOT_SUPPORT){
			if(model_sig_cfg_s.gatt_proxy != gatt_proxy){
				model_sig_cfg_s.gatt_proxy = gatt_proxy;
				if(model_sig_cfg_s.hb_pub.feature & BIT(MESH_HB_PROXY_BIT)){
					hb_sts_change = 1;
					hb_pub_100ms = clock_time_100ms();
				}	
			}
			mesh_model_store_cfg_s();
			// and the if the proxy state is being set to 0 ,the connection will be terminate 
			if (model_sig_cfg_s.gatt_proxy == GATT_PROXY_SUPPORT_DISABLE ){
				#ifndef WIN32
				#if MD_DF_EN
				mesh_directed_forwarding_bind_state_update();
				mesh_common_store(FLASH_ADR_MD_DF_SBR);
				#endif
				#if !GATT_LPN_EN
				// send terminate cmd 
				if(blt_state == BLS_LINK_STATE_CONN){
					#if !TESTCASE_FLAG_ENABLE
					bls_ll_terminateConnection(0x13);

					#endif
					// need to disable relay 
				}
				#endif
				#endif
			}	
		}else{
			par[0] = model_sig_cfg_s.gatt_proxy;
		}
		#if MD_PRIVACY_BEA&&!WIN32&&MD_SERVER_EN
		u8 *p_private_sts = &(g_mesh_model_misc_save.privacy_bc.proxy_sts);
		mesh_private_proxy_change_by_gatt_proxy(*p_private_sts,p_private_sts);
		mesh_privacy_beacon_save();
		#endif
		
        #if DEBUG_CFG_CMD_GROUP_AK_EN
        gatt_adv_send_flag = gatt_proxy;
        #endif

		return mesh_cmd_sig_cfg_gatt_proxy_get(par, par_len, cb_par);
    }
	
    return -1;
}

int mesh_cmd_sig_cfg_relay_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    return mesh_tx_cmd_rsp_cfg_model(CFG_RELAY_STATUS, &model_sig_cfg_s.relay, sizeof(mesh_cfg_model_relay_set_t), cb_par->adr_src);
}

int mesh_cmd_sig_cfg_relay_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    mesh_cfg_model_relay_set_t *p_set = (mesh_cfg_model_relay_set_t *)par;
    if(p_set->relay < RELAY_NOT_SUPPORT){
		if(model_sig_cfg_s.relay < RELAY_NOT_SUPPORT){
			if(model_sig_cfg_s.relay != p_set->relay){
				model_sig_cfg_s.relay = p_set->relay;
				//relay bit changes ,and need to send the heartbeat msg 
				if(model_sig_cfg_s.hb_pub.feature & BIT(MESH_HB_RELAY_BIT)){
					hb_sts_change = 1;
					hb_pub_100ms = clock_time_100ms();
				}
			}
			memcpy(&model_sig_cfg_s.relay_retransmit, &p_set->transmit, sizeof(model_sig_cfg_s.relay_retransmit));
			mesh_model_store_cfg_s();
		}
		else{
			memcpy(par, &model_sig_cfg_s.relay_retransmit, sizeof(model_sig_cfg_s.relay_retransmit));
		}
		return mesh_cmd_sig_cfg_relay_get(par, par_len, cb_par);
    }
    return -1;
}

int mesh_cmd_sig_cfg_nw_transmit_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    return mesh_tx_cmd_rsp_cfg_model(CFG_NW_TRANSMIT_STATUS, (u8 *)&model_sig_cfg_s.nw_transmit, sizeof(mesh_transmit_t), cb_par->adr_src);
}

int mesh_cmd_sig_cfg_nw_transmit_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	#if 1	// can not set now
    memcpy(&model_sig_cfg_s.nw_transmit, par, sizeof(mesh_transmit_t));
	mesh_model_store_cfg_s();
	#endif
    return mesh_cmd_sig_cfg_nw_transmit_get(par, par_len, cb_par);
}

#define SUB_ADR_DEF_VAL			(0xffff)
#define IS_VALID_SUB_ADR(adr)	(adr && (adr != 0xffff))

enum{
	SUB_SAVE_ERR = 0,
	SUB_SAVE_0000,		//  found the first 0x0000
	SUB_SAVE_FFFF,		//  found the first 0xffff
};

void mesh_sub_par_set(model_common_t *p_model, u32 sub_index, u16 sub_adr, u8 *uuid)
{
	p_model->sub_list[sub_index] = sub_adr;
	#if VIRTUAL_ADDR_ENABLE
	u32 len_uuid = sizeof(p_model->sub_uuid[sub_index]);
	if((0 == sub_adr)||(SUB_ADR_DEF_VAL == sub_adr)){
		memset(p_model->sub_uuid[sub_index], sub_adr, len_uuid);
	}else{
		if(uuid){
			memcpy(p_model->sub_uuid[sub_index], uuid, len_uuid);
		}else{
			memset(p_model->sub_uuid[sub_index], SUB_ADR_DEF_VAL, len_uuid);
		}
	}
	#endif
}

static inline void mesh_sub_par_set2def_val(model_common_t *p_model, u32 sub_index)
{
	mesh_sub_par_set(p_model, sub_index, SUB_ADR_DEF_VAL, 0);
}

static inline void mesh_sub_par_del(model_common_t *p_model, u32 sub_index)
{
	mesh_sub_par_set(p_model, sub_index, 0, 0);
}

int is_existed_sub_adr(model_common_t *p_model, u32 sub_index, u16 sub_adr, u8 *uuid)
{
	return (sub_adr == p_model->sub_list[sub_index]
		&& (!uuid 
		#if VIRTUAL_ADDR_ENABLE
		|| ((0 == memcmp(uuid, p_model->sub_uuid[sub_index],16)))
		#endif
		));
}

u8 mesh_cmd_sig_cfg_model_sub_set2(u16 op, u16 sub_adr, u8 *uuid, model_common_t *p_model, u32 model_id, bool4 sig_model)
{
	if(is_use_device_key(model_id, sig_model)){
		return ST_NOT_SUB_MODEL;
	}

    int save_flash = 1;
    u8 st = ST_UNSPEC_ERR;
    if((CFG_MODEL_SUB_ADD == op)||(CFG_MODEL_SUB_VIRTUAL_ADR_ADD == op)){
        int add_ok = 0;
        foreach(i,SUB_LIST_MAX){
        	if(is_existed_sub_adr(p_model, i, sub_adr, uuid)){
				add_ok = 1;
				save_flash = 0;
				break;
        	}
        }

        // init empty subscription list to 0xffff, is used to save a new subscription adr.
        if(0 == add_ok){
			u32 save_type = SUB_SAVE_ERR;
			u32 save_index = 0;
			#if FEATURE_LOWPOWER_EN
            foreach(i,SUB_LIST_MAX_LPN)
			#else
            foreach(i,SUB_LIST_MAX)
            #endif
            {
				u16 sub = p_model->sub_list[i];
				if(0 == sub){
					if(SUB_SAVE_ERR == save_type){
						save_type = SUB_SAVE_0000;
						save_index = i; 	// the first empty
					}
				}else if(0xffff == sub){
					save_type = SUB_SAVE_FFFF;
					save_index = i;
					break;	// exist 0xffff, can save in this adr of flash directly.
				}
			}
			
			if(SUB_SAVE_ERR == save_type){
				// set ST_INSUFFICIENT_RES later
			}else{
				if(SUB_SAVE_FFFF == save_type){
				}else if(SUB_SAVE_0000 == save_type){
					foreach(i,SUB_LIST_MAX){
						if(0 == p_model->sub_list[i]){
							mesh_sub_par_set2def_val(p_model, i); // because it will use a new flash zone later.
						}
					}
				}
				
				mesh_sub_par_set(p_model, save_index, sub_adr, uuid);
				add_ok = 1;
				#if !VIRTUAL_ADDR_ENABLE
				if(uuid){
					add_ok = 0;
				}
				#endif		
			}
        }
        
        st = add_ok ? ST_SUCCESS : ST_INSUFFICIENT_RES;
        
		#if (MD_DF_EN && !FEATURE_LOWPOWER_EN && !WIN32)
		if(add_ok && save_flash){
			directed_forwarding_solication_start(mesh_key.netkey_sel_dec, (mesh_ctl_path_request_solication_t *)&sub_adr, 1);
		}
		#endif
    }else if((CFG_MODEL_SUB_DEL == op)||(CFG_MODEL_SUB_VIRTUAL_ADR_DEL == op)){
        foreach(i,SUB_LIST_MAX){
            if(is_existed_sub_adr(p_model, i, sub_adr, uuid)){
                mesh_sub_par_del(p_model, i);
            }
        }
        st = ST_SUCCESS;
    }else if(((CFG_MODEL_SUB_OVER_WRITE == op)||(CFG_MODEL_SUB_VIRTUAL_ADR_OVER_WRITE == op))
    	   ||(CFG_MODEL_SUB_DEL_ALL == op)){
        memset(p_model->sub_list, SUB_ADR_DEF_VAL, sizeof(p_model->sub_list));
		#if VIRTUAL_ADDR_ENABLE
        memset(p_model->sub_uuid, SUB_ADR_DEF_VAL, sizeof(p_model->sub_uuid));
		#endif
		st = ST_SUCCESS;
        if(CFG_MODEL_SUB_DEL_ALL != op){
        	mesh_sub_par_set(p_model, 0, sub_adr, uuid);
			#if !VIRTUAL_ADDR_ENABLE
			if(uuid){
				st = ST_INSUFFICIENT_RES;
			}
			#endif
        }
       
    }

	if(ST_SUCCESS == st){
	    if(save_flash){
		    mesh_model_store(sig_model, model_id);
		}
		rf_link_light_event_callback(LGT_CMD_SET_SUBSCRIPTION);
	}

    return st;
}

int mesh_rsp_err_st_sub_list(u8 st, u16 ele_adr, u32 model_id, bool4 sig_model, u16 adr_dst)
{
    mesh_cfg_model_sub_list_vendor_t sub_list_rsp;
    
    memset(&sub_list_rsp, 0, sizeof(sub_list_rsp));
    sub_list_rsp.status = st;
    sub_list_rsp.ele_adr = ele_adr;
    sub_list_rsp.model_id = model_id;
    u32 len_rsp = sig_model ? 5 : 7;
    u16 op = sig_model ? CFG_SIG_MODEL_SUB_LIST : CFG_VENDOR_MODEL_SUB_LIST;
    return mesh_tx_cmd_rsp_cfg_model(op, (u8 *)&sub_list_rsp, len_rsp, adr_dst);
}

int mesh_sub_list_get(u8 *p_list, model_common_t *p_model)
{    
    int cnt = 0;
    u16 list[SUB_LIST_MAX];
    foreach(i, SUB_LIST_MAX){
        if(p_model->sub_list[i] && (0xffff != p_model->sub_list[i])){
            list[cnt++] = p_model->sub_list[i];
        }
    }
    memcpy(p_list, list, cnt * 2);

    return cnt;
}

u8 mesh_add_pkt_model_sub_list(u16 ele_adr, u32 model_id, bool4 sig_model, u16 adr_src)
{
    u8 st = ST_UNSPEC_ERR;
    u8 model_idx = 0;
    model_common_t *p_model = (model_common_t *)mesh_find_ele_resource_in_model(ele_adr, model_id, sig_model,&model_idx, 0);
    if(p_model && (!p_model->no_sub)){
		st = ST_SUCCESS;
        mesh_cfg_model_sub_list_vendor_t sub_list_rsp;
        sub_list_rsp.status = ST_SUCCESS;
        sub_list_rsp.ele_adr = ele_adr;
        sub_list_rsp.model_id = model_id;
        u8 *p_list = (u8 *)(sub_list_rsp.sub_adr) - FIX_SIZE(sig_model);
        u32 cnt = mesh_sub_list_get(p_list, p_model);

        u32 len_rsp = cnt*2 + OFFSETOF(mesh_cfg_model_sub_list_vendor_t,sub_adr) - FIX_SIZE(sig_model);
        u16 op = sig_model ? CFG_SIG_MODEL_SUB_LIST : CFG_VENDOR_MODEL_SUB_LIST;
        mesh_tx_cmd_rsp_cfg_model(op, (u8 *)&sub_list_rsp, len_rsp, adr_src);
    }else{
    	st = ST_NOT_SUB_MODEL;
    }
    
    return st;
}

#if VIRTUAL_ADDR_ENABLE
int mesh_sec_msg_dec_virtual_ll(u16 ele_adr, u32 model_id, bool4 sig_model, 
			u8 *key, u8 *nonce, u8 *dat, int n, int mic_length, u16 adr_dst, const u8 *dat_org)
{
	u8 model_idx = 0;
	model_common_t *p_model;
	p_model = (model_common_t *)mesh_find_ele_resource_in_model(ele_adr, model_id, sig_model, &model_idx, 0);
	if(p_model){
		foreach_arr(i,p_model->sub_list){
			if(p_model->sub_list[i] == adr_dst){
				if(!mesh_sec_msg_dec_ll(key, nonce, dat, n, p_model->sub_uuid[i], 16, mic_length)){
					return 0;
				}else{
					memcpy(dat, dat_org, n);	// recover data
				}
			}
		}
	}
	return -1;
}
#endif

int mesh_key_phase_st_rsp(u16 nk_idx, u8 st, mesh_net_key_t *p_netkey, mesh_cb_fun_par_t *cb_par)
{
	mesh_key_refresh_phase_status_t rsp = {0};
	rsp.nk_idx = nk_idx;
	if(ST_SUCCESS == st){
		rsp.phase = p_netkey->key_phase;
	}
	rsp.st = st;
    return mesh_tx_cmd_rsp_cfg_model(CFG_KEY_REFRESH_PHASE_STATUS, (u8 *)&rsp, sizeof(rsp), cb_par->adr_src);
}

int mesh_cmd_sig_cfg_key_phase_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	u16 nk_idx = ((mesh_key_refresh_phase_get_t *)par)->nk_idx;
	mesh_net_key_t *p_netkey_exist = is_mesh_net_key_exist(nk_idx);
	u8 st = p_netkey_exist ? ST_SUCCESS : ST_INVALID_NETKEY;
    return mesh_key_phase_st_rsp(nk_idx, st, p_netkey_exist, cb_par);
}

int mesh_cmd_sig_cfg_key_phase_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	mesh_key_refresh_phase_set_t *p_set = (mesh_key_refresh_phase_set_t *)par;
	if((p_set->transition == KEY_REFRESH_TRANS2)||(p_set->transition == KEY_REFRESH_TRANS3)){
		mesh_net_key_t *p_netkey_exist = is_mesh_net_key_exist(p_set->nk_idx);
		int st = ST_SUCCESS;
		mesh_net_key_t *p_nk_dec = &mesh_key.net_key[mesh_key.netkey_sel_dec][0];
		if(p_netkey_exist && (p_netkey_exist->index == p_nk_dec->index)){
			st = mesh_key_refresh_phase_handle(p_set->transition, p_netkey_exist);
		}else{
			st = ST_INVALID_NETKEY;
		}
		
		if(st != -1){
			return mesh_key_phase_st_rsp(p_set->nk_idx, (u8)st, p_netkey_exist, cb_par);
		}
	}
	return 0;
}

int mesh_cmd_sig_cfg_key_phase_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    	mesh_key_refresh_phase_status_t *rsp = (mesh_key_refresh_phase_status_t *)par;
    	if(ST_SUCCESS == rsp->st){
    		mesh_kr_cfgcl_check_set_next_flag();
    	}
    }
    return err;
}

int mesh_node_identity_st_rsp(u16 nk_idx, u8 st, mesh_net_key_t *p_netkey, mesh_cb_fun_par_t *cb_par)
{
	mesh_node_identity_status_t rsp = {0};
	rsp.nk_idx = nk_idx;
	if(ST_SUCCESS == st){
		rsp.identity = p_netkey->node_identity;
	}
	rsp.st = st;
    return mesh_tx_cmd_rsp_cfg_model(NODE_ID_STATUS, (u8 *)&rsp, sizeof(rsp), cb_par->adr_src);
}

int mesh_cmd_sig_cfg_node_identity_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	u16 nk_idx = ((mesh_node_identity_get_t *)par)->nk_idx;
	mesh_net_key_t *p_netkey_exist = is_mesh_net_key_exist(nk_idx);
	u8 st = p_netkey_exist ? ST_SUCCESS : ST_INVALID_NETKEY;
    return mesh_node_identity_st_rsp(nk_idx, st, p_netkey_exist, cb_par);
}

int mesh_cmd_sig_cfg_node_identity_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	mesh_node_identity_set_t *p_set = (mesh_node_identity_set_t *)par;
	mesh_net_key_t *p_netkey_exist = is_mesh_net_key_exist(p_set->nk_idx);
	int st = -1;
	if(p_netkey_exist){
		if(p_netkey_exist->node_identity != NODE_IDENTITY_SUB_NET_NOT_SUPPORT){
		 	if(p_set->identity < NODE_IDENTITY_SUB_NET_NOT_SUPPORT){
				if(p_set->identity == NODE_IDENTITY_SUB_NET_RUN){
					p_netkey_exist->start_identity_s = clock_time_s();
				}
				if(p_netkey_exist->node_identity != p_set->identity){
					p_netkey_exist->node_identity = p_set->identity;
					mesh_private_identity_change_by_proxy_service(p_netkey_exist);
					mesh_key_save();
				}
				st = ST_SUCCESS;
			}
		}else{
			st = ST_SUCCESS;
		}
	}else{
		st = ST_INVALID_NETKEY;
	}

	if(st != -1){
		return mesh_node_identity_st_rsp(p_set->nk_idx, (u8)st, p_netkey_exist, cb_par);
	}

	return 0;
}

int mesh_cmd_sig_cfg_node_identity_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_lpn_poll_timeout_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
#if FEATURE_FRIEND_EN
    mesh_lpn_poll_timeout_status_t rsp = {0};
    rsp.lpn_adr = get_u16_not_aligned(par);
    if((0 == rsp.lpn_adr)||(rsp.lpn_adr & 0x8000)){
        return 0;
    }
    
    rsp.timeout = get_current_poll_timeout_timer_fn(rsp.lpn_adr);
    return mesh_tx_cmd_rsp_cfg_model(CFG_LPN_POLL_TIMEOUT_STATUS, (u8 *)&rsp, 5, cb_par->adr_src);
#else
    return 0;
#endif
}

int mesh_cmd_sig_cfg_lpn_poll_timeout_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

// ----------------NET KEY
int mesh_cmd_sig_cfg_netkey_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	mesh_netkey_set_t *p_set = (mesh_netkey_set_t *)par;
	mesh_netkey_status_t status = {ST_INSUFFICIENT_RES};
	status.idx = p_set->idx;
	mesh_netkey_cb((u8)p_set->idx,cb_par->op);
	status.st = mesh_net_key_set(cb_par->op, p_set->key, p_set->idx, 1);

    return mesh_tx_cmd_rsp_cfg_model(NETKEY_STATUS, (u8 *)&status, sizeof(status), cb_par->adr_src);
}

int mesh_cmd_sig_cfg_netkey_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    	mesh_netkey_status_t *st = (mesh_netkey_status_t *)par;
    	if(ST_SUCCESS == st->st){
    		mesh_kr_cfgcl_check_set_next_flag();
    	}
    }
    return err;
}

int mesh_cmd_sig_cfg_netkey_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	mesh_netkey_list_t list;
	u32 cnt = mesh_net_key_get((u8 *)&list);
	u32 len = GET_KEY_INDEX_LEN(cnt);

    return mesh_tx_cmd_rsp_cfg_model(NETKEY_LIST, (u8 *)&list, len, cb_par->adr_src);
}

int mesh_cmd_sig_cfg_netkey_list(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

// ----------------APP KEY
int mesh_cmd_sig_cfg_appkey_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	mesh_appkey_set_t *p_set = (mesh_appkey_set_t *)par;
	mesh_appkey_status_t status;
	memcpy(&status.net_app_idx,&p_set->net_app_idx,sizeof(status.net_app_idx));
	status.st = mesh_app_key_set(cb_par->op, p_set->appkey, GET_APPKEY_INDEX(p_set->net_app_idx),
								GET_NETKEY_INDEX(p_set->net_app_idx), 1);

    return mesh_tx_cmd_rsp_cfg_model(APPKEY_STATUS, (u8 *)&status, sizeof(status), cb_par->adr_src);
}

int mesh_cmd_sig_cfg_appkey_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    	mesh_appkey_status_t *st = (mesh_appkey_status_t *)par;
    	if(ST_SUCCESS == st->st){
    		mesh_kr_cfgcl_check_set_next_flag();
    	}
    }
    return err;
}

int mesh_cmd_sig_cfg_appkey_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	mesh_appkey_get_t *p_get = (mesh_appkey_get_t *)par;
	mesh_appkey_list_t list;
	list.netkey_idx = p_get->netkey_idx;

	u32 cnt = mesh_app_key_get((u8 *)&list);
	u32 len = OFFSETOF(mesh_appkey_list_t,appkey_idx_enc) + GET_KEY_INDEX_LEN(cnt);

    return mesh_tx_cmd_rsp_cfg_model(APPKEY_LIST, (u8 *)&list, len, cb_par->adr_src);
}

int mesh_cmd_sig_cfg_appkey_list(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_model_app_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	mesh_md_app_get_t *p_get = (mesh_md_app_get_t *)par;
	mesh_sig_md_app_list_t list = {0};

	bool4 sig_model = (SIG_MODEL_APP_GET == cb_par->op);
	u16 ele_adr = p_get->ele_adr;
	u32 model_id = sig_model ? (p_get->model_id & 0xffff) : p_get->model_id;
    u8 st = ST_UNSPEC_ERR;
	u32 cnt = 0;
		
	list.ele_adr = p_get->ele_adr;
	list.model_id = model_id;	// must before SET_KEY_INDEX

	if(is_use_device_key(model_id, sig_model)){
		st = ST_INVALID_MODEL;
	}else{
	    u8 model_idx = 0;
	    model_common_t *p_model = (model_common_t *)mesh_find_ele_resource_in_model(ele_adr, model_id, sig_model, &model_idx, 0);

	    if(p_model){
			memset(list.appkey_idx_enc, 0, sizeof(list.appkey_idx_enc));
			u8 *p_idx = (u8 *)(list.appkey_idx_enc) - FIX_SIZE(sig_model);
			foreach(i,g_bind_key_max){
				if(p_model->bind_key[i].bind_ok){
					if(cnt & 1){
						SET_KEY_INDEX_H(p_idx+((cnt/2)*3), p_model->bind_key[i].idx);
					}else{
						SET_KEY_INDEX_L(p_idx+((cnt/2)*3), p_model->bind_key[i].idx);
					}
		    		cnt++;
	    		}
    		}

			st = ST_SUCCESS;
		}else{
			if(is_own_ele(ele_adr)){
	    		st = ST_INVALID_MODEL;
			}else{
	    		st = ST_INVALID_ADR;
	    	}
	    }
    }
    
	list.st = st;
	
	u32 len = OFFSETOF(mesh_sig_md_app_list_t,appkey_idx_enc) + GET_KEY_INDEX_LEN(cnt) - FIX_SIZE(sig_model);
	u16 op_rsp = sig_model ? SIG_MODEL_APP_LIST : VENDOR_MODEL_APP_LIST;
    return mesh_tx_cmd_rsp_cfg_model(op_rsp, (u8 *)&list, len, cb_par->adr_src);
}

int mesh_cmd_sig_cfg_sig_model_app_list(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_vd_model_app_list(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int is_exist_mesh_appkey_idx(u16 appkey_idx)
{
	foreach(i,NET_KEY_MAX){
		foreach(j,APP_KEY_MAX){
		    // just compare old key is enough, because appkey_idx of old key is alway valid and same with new, if existed.
			mesh_app_key_t *p = &(mesh_key.net_key[i][0].app_key[j]);
			if(p->valid && (p->index == appkey_idx)){
				return 1;
			}
		}
	}
	return 0;
}

bind_key_t * is_exist_bind_key(model_common_t *p_model, u16 appkey_idx)
{
	foreach(i,g_bind_key_max){
		if(p_model->bind_key[i].bind_ok && (p_model->bind_key[i].idx == appkey_idx)){
			return (&(p_model->bind_key[i]));
		}
	}
	return 0;
}

bind_key_t * find_bind_key_empty(model_common_t *p_model)
{
	foreach(i,g_bind_key_max){
		if(!p_model->bind_key[i].bind_ok){
			return (&(p_model->bind_key[i]));
		}
	}
	return 0;
}

u32 get_bind_key_cnt(model_common_t *p_model)
{
	u32 cnt = 0;
	foreach(i,g_bind_key_max){
		if(p_model->bind_key[i].bind_ok){
			cnt++;
		}
	}
	return cnt;
}

void bind_key_set_par(bind_key_t *bind_key, u16 appkey_idx, u32 model_id, bool4 sig_model)
{
	bind_key->bind_ok = 1;
	bind_key->idx = appkey_idx;
	mesh_model_store(sig_model, model_id);
}

void unbind_key_par(model_common_t *p_model, bind_key_t *bind_key, u32 model_id, bool4 sig_model)
{
	if(p_model->pub_adr && (p_model->pub_par.appkey_idx == bind_key->idx)){
		p_model->pub_adr = 0;
		memset(p_model->pub_uuid, 0, sizeof(p_model->pub_uuid));
		memset(&p_model->pub_par, 0, sizeof(p_model->pub_par));
	}
	memset(bind_key, 0, sizeof(bind_key_t));
	mesh_model_store(sig_model, model_id);
}

u8 mesh_appkey_bind(u16 op, u16 ele_adr, u32 model_id, bool4 sig_model, u16 appkey_idx)
{
	if(is_use_device_key(model_id, sig_model)){
		return ST_INVALID_MODEL;
	}

    if(0 == is_exist_mesh_appkey_idx(appkey_idx)){
    	return ST_INVALID_APPKEY;
    }
    
    u8 model_idx = 0;
    model_common_t *p_model = (model_common_t *)mesh_find_ele_resource_in_model(ele_adr, model_id, sig_model, &model_idx, 0);

    int st;
    if(p_model){
    	if(MODE_APP_BIND == op){
	    	if(is_exist_bind_key(p_model, appkey_idx)){
				st = ST_SUCCESS;
	    	}else{
	    		bind_key_t *bind_key = find_bind_key_empty(p_model);
		    	if(bind_key){
		    		bind_key_set_par(bind_key, appkey_idx, model_id, sig_model);
					st = ST_SUCCESS;
	            }else{
					st = ST_INSUFFICIENT_RES;
	            }
	    	}
    	}else{	// unbind
	    	bind_key_t *bind_key = is_exist_bind_key(p_model, appkey_idx);
	    	if(bind_key){
	    		unbind_key_par(p_model, bind_key, model_id, sig_model);
	    	}
			st = ST_SUCCESS;
    	}
    }else{
    	st = ST_INVALID_MODEL;
    }
	mesh_node_refresh_binding_tick();
    return st;
}

u8 mesh_appkey_bind_and_share(u16 op, u16 ele_adr, u32 model_id, bool4 sig_model, u16 appkey_idx)
{
	u8 st = ST_SUCCESS;
	int share_flag = 0;
	#if 0 // SUBSCRIPTION_PUBLISH_SHARE_EN
	if(sig_model){
		if((model_id >= SIG_MD_G_ONOFF_S)&&(model_id <= SIG_MD_G_LEVEL_C)){
			share_flag = 1;
			for(unsigned int i = SIG_MD_G_ONOFF_S; i <= (SIG_MD_G_LEVEL_C); ++i){
				u8 st2 = mesh_appkey_bind(op, ele_adr, i, sig_model,appkey_idx);
				if(model_id == i){
					st = st2;
				}
			}
		}
	}
	#endif

	if(0 == share_flag){
	    st = mesh_appkey_bind(op, ele_adr, model_id, sig_model,appkey_idx);
	}

	return st;
}

int mesh_cmd_sig_cfg_bind(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{    
    //int err = -1;
    bool4 sig_model = (sizeof(mesh_app_bind_t) - 2 == par_len);
	LAYER_PARA_DEBUG(A_debug_mode_keybind_enter);
    mesh_app_bind_t *p_bind = (mesh_app_bind_t *)par;
    u32 model_id = sig_model ? (p_bind->model_id & 0xFFFF) : p_bind->model_id;
    
    mesh_app_bind_status_t bind_rsp;
    memcpy(&bind_rsp.bind, p_bind, sizeof(mesh_app_bind_t));
    if(!is_unicast_adr(p_bind->ele_adr)){
        bind_rsp.st = ST_INVALID_ADR;
		LAYER_PARA_DEBUG(A_debug_mode_keybind_isnot_unicast_err);
    }else{
        mesh_adr_list_t adr_list;
        bind_rsp.st = find_ele_support_model_and_match_dst(&adr_list, p_bind->ele_adr, model_id, sig_model);

        if((adr_list.adr_cnt)){
            if(is_use_device_key(model_id, sig_model)){
                LAYER_PARA_DEBUG(A_debug_mode_keybind_iscfgmodel_err);
                bind_rsp.st = ST_CAN_NOT_BIND;
            }else{
                bind_rsp.st = mesh_appkey_bind_and_share(cb_par->op, p_bind->ele_adr, model_id, sig_model,p_bind->index);
                if(key_bind_all_ele_en && (g_ele_cnt > 1) && (p_bind->ele_adr == ele_adr_primary)){
                    foreach(i,(u32)(g_ele_cnt - 1)){
                        mesh_appkey_bind_and_share(cb_par->op, ele_adr_primary + 1 + i, model_id, sig_model,p_bind->index);
                    }
                }
            }
        }

        if(MODE_APP_UNBIND == cb_par->op){
        	//bind_rsp.st = ST_SUCCESS;	// always
        }
    }
	
    return mesh_tx_cmd_rsp_cfg_model(MODE_APP_STATUS, (u8 *)&bind_rsp, sizeof(bind_rsp)-FIX_SIZE(sig_model), cb_par->adr_src);
}

int mesh_cmd_sig_cfg_bind_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    	mesh_app_bind_status_t *p_st = (mesh_app_bind_status_t *)par;
    	if(ST_SUCCESS == p_st->st){
    		mesh_kr_cfgcl_check_set_next_flag();
    	}
    }
    return err;
}


int mesh_cmd_sig_cfg_node_reset(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	u32 rsp_flag = node_reset_start(cb_par->adr_src);
	if(is_actived_factory_test_mode() && (0 == rsp_flag)){
        return 0;
    }
    
    u8 par_status[1] = {0};
	#if GATT_RP_EN
	rp_gatt_disconnect();
	#endif
    return mesh_tx_cmd_rsp_cfg_model(NODE_RESET_STATUS, par_status, 0, is_own_ele(cb_par->adr_src)?ADR_ALL_NODES:cb_par->adr_src);
}

int mesh_cmd_sig_cfg_node_reset_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

// publish
#define PUB_ADR_AND_PAR_LEN     (2+sizeof(mesh_model_pub_par_t))
inline void mesh_copy_pub_adr_par(void * out,const void * in)
{
    memcpy(out, in, PUB_ADR_AND_PAR_LEN);
}

int mesh_save_pub_and_refresh_tick(model_common_t *p_model, mesh_cfg_model_pub_set_t *p_pub_set, u8 *pub_uuid)
{
    int change_flag = 0;
    if(memcmp(&p_model->pub_adr, &p_pub_set->pub_adr, PUB_ADR_AND_PAR_LEN)){
        mesh_copy_pub_adr_par(&p_model->pub_adr, &p_pub_set->pub_adr);
        change_flag = 1;
    }
    
    if(pub_uuid){
        if(memcmp(p_model->pub_uuid, pub_uuid, sizeof(p_model->pub_uuid))){
            memcpy(p_model->pub_uuid, pub_uuid, sizeof(p_model->pub_uuid));
            change_flag = 1;
        }
    }else{
        u8 zero[16] = {0};
        if(memcmp(p_model->pub_uuid, zero, sizeof(p_model->pub_uuid))){
            memset(p_model->pub_uuid, 0, sizeof(p_model->pub_uuid));
            change_flag = 1;
        }
    }
    
    p_model->cb_tick_ms = clock_time_ms();

    return change_flag;
}

int mesh_rsp_err_st_pub_status(u8 st, u16 ele_adr, u32 model_id, bool4 sig_model, u16 adr_dst)
{
    mesh_cfg_model_pub_st_t pub_st;
    memset(&pub_st, 0, sizeof(pub_st));
    pub_st.status = st;
    pub_st.set.ele_adr = ele_adr;
    pub_st.set.model_id = model_id;
    return mesh_tx_cmd_rsp_cfg_model(CFG_MODEL_PUB_STATUS, (u8 *)&pub_st, sizeof(pub_st)-FIX_SIZE(sig_model), adr_dst);
}

u8 mesh_add_pkt_model_pub(u16 ele_adr, u32 model_id, bool4 sig_model, u16 adr_src)
{
    u8 st = ST_SUCCESS;
    u8 model_idx = 0;
    model_common_t *p_model = (model_common_t *)mesh_find_ele_resource_in_model(ele_adr, model_id, sig_model, &model_idx, 0);

    if(!p_model){   // response error status after this function.
    	st = ST_INVALID_MODEL;
    }else if(p_model->no_pub){
    	st = ST_INVALID_MODEL;	// model not support publish
    }else{
        mesh_cfg_model_pub_st_t pub_st;
        pub_st.status = ST_SUCCESS;
        pub_st.set.ele_adr = ele_adr;
        mesh_copy_pub_adr_par(&pub_st.set.pub_adr, &p_model->pub_adr);
        pub_st.set.model_id = model_id;
        
        mesh_tx_cmd_rsp_cfg_model(CFG_MODEL_PUB_STATUS, (u8 *)&pub_st, sizeof(pub_st)-FIX_SIZE(sig_model), adr_src);
    }

    return st;
}

u8 get_transition_step_res(u32 transition_100ms)
{
	trans_time_t trans_time;
	trans_time.val = 0;
	if(transition_100ms < 64 * RES_100MS){
		trans_time.steps = transition_100ms;
		trans_time.res = TRANSITION_STEP_RES_100MS;
	}else if(transition_100ms < 64 * RES_1S){
		trans_time.steps = transition_100ms / RES_1S;
		trans_time.res = TRANSITION_STEP_RES_1S;
	}else if(transition_100ms < 64 * RES_10S){
		trans_time.steps = transition_100ms / RES_10S;
		trans_time.res = TRANSITION_STEP_RES_10S;
	}else if(transition_100ms < 64 * RES_10MIN){
		trans_time.steps = transition_100ms / RES_10MIN;
		trans_time.res = TRANSITION_STEP_RES_10MIN;
	}else{
		trans_time.val = 0xff;  // step 0x3f means higher than 0x3e
	}

	return trans_time.val;
}

u32 get_transition_100ms(trans_time_t *p_trans_time)
{
	u32 step_res_100ms = RES_10MIN;	// 10 min;
	if(TRANSITION_STEP_RES_100MS == p_trans_time->res){
		step_res_100ms = RES_100MS;
	}else if(TRANSITION_STEP_RES_1S == p_trans_time->res){
		step_res_100ms = RES_1S;
	}else if(TRANSITION_STEP_RES_10S == p_trans_time->res){
		step_res_100ms = RES_10S;
	}else if(TRANSITION_STEP_RES_10MIN == p_trans_time->res){
		step_res_100ms = RES_10MIN;	// 10 min;
	}
	return (p_trans_time->steps * step_res_100ms);
}

void mesh_tx_pub_period(u16 ele_adr, u32 model_id, bool4 sig_model)
{
	model_common_t *p_model;
	u8 model_idx = 0;
	p_model = (model_common_t *)mesh_find_ele_resource_in_model(ele_adr,model_id,sig_model,&model_idx, 0);
	if(p_model){
		
		if(p_model->pub_adr){
		    int pub_flag = 0;
		    if(!light_pub_model_priority || ((u8 *)p_model == light_pub_model_priority)){
    		    if(p_model->pub_trans_flag && (ST_PUB_TRANS_ALL_OK == light_pub_trans_step)){
                    light_pub_model_priority = 0;
                    pub_flag = 1;
    		    }
		    }
			// normal publish 
		    if(p_model->pub_par.period.steps && p_model->cb_pub_st){
    			u32 pub_interval_ms = get_mesh_pub_interval_ms(model_id, sig_model, &p_model->pub_par.period);
    			if(pub_flag || clock_time_exceed_ms(p_model->cb_tick_ms, pub_interval_ms)){
    			    // can't make tick greater than now.
    				p_model->cb_tick_ms = clock_time_ms() - get_random_delay_pub_tick_ms(pub_interval_ms);
    				pub_flag = 1;
    			}
			}

			if(pub_flag){
                if((0 == p_model->cb_pub_st) || p_model->cb_pub_st(model_idx) != -1){
					p_model->pub_trans_flag = 0;
				}
			}
		}
	}
}

int mesh_is_existed_share_model(mesh_model_id_t *md_out, u32 model_id, bool4 sig_model)
{
	if(sig_model){
		u32 model_id_share = -1;
		if(SIG_MD_G_ONOFF_S == model_id){
			model_id_share = SIG_MD_G_LEVEL_S;
		}else if(SIG_MD_G_LEVEL_S == model_id){
			model_id_share = SIG_MD_G_ONOFF_S;
		}else if(SIG_MD_G_ONOFF_C == model_id){
			model_id_share = SIG_MD_G_LEVEL_C;
		}else if(SIG_MD_G_LEVEL_C == model_id){
			model_id_share = SIG_MD_G_ONOFF_C;
		}
		if(-1 != model_id_share){
			md_out->sig_model = 1;
			md_out->id = model_id_share;
			return 1;
		}
	}
	
	return 0;
}

u8 mesh_pub_par_check(model_common_t *p_model, mesh_cfg_model_pub_set_t *p_pub_set){
	u8 st = ST_SUCCESS;
	if(p_pub_set->pub_par.rfu != 0){
		p_pub_set->pub_par.rfu = 0;
	}
	
	if(p_pub_set->pub_adr == 0){
		memset((u8 *)&p_pub_set->pub_par, 0x00, sizeof(mesh_model_pub_par_t));
		return ST_SUCCESS;
	}

	if(!is_exist_bind_key(p_model, p_pub_set->pub_par.appkey_idx)){
		st = ST_INVALID_APPKEY;
	}else if(p_pub_set->pub_par.credential_flag == 1){
		// st = ST_FEATURE_NOT_SUPPORT;
	}
	
	return st;
}

u8 mesh_pub_search_and_set(u16 ele_adr, u8 *pub_set, u32 model_id, bool4 sig_model, int vr_flag)
{
    u8 st = ST_UNSPEC_ERR;
    mesh_adr_list_t adr_list;
    st = find_ele_support_model_and_match_dst(&adr_list, ele_adr, model_id, sig_model);
    //LOG_MSG_LIB(TL_LOG_NODE_SDK,0, 0,"xxx,adr:0x%04x,st:0x%x,",ele_adr,st);
	if(st != ST_SUCCESS) return st;

	int change_falg = 0;
    if(adr_list.adr_cnt){
        u8 model_idx = 0;
        model_common_t *p_model = (model_common_t *)mesh_find_ele_resource_in_model(ele_adr, model_id, sig_model,&model_idx, 0);
        if(p_model && (!p_model->no_pub)){
        	if(vr_flag){
				mesh_cfg_model_pub_virtual_set_t *p_pub_set_vr = (mesh_cfg_model_pub_virtual_set_t *)pub_set;
				mesh_cfg_model_pub_set_t pub_set_temp;
				pub_set_temp.pub_adr = cala_vir_adr_by_uuid(p_pub_set_vr->pub_uuid);
				memcpy(&pub_set_temp.pub_par, &p_pub_set_vr->pub_par, sizeof(mesh_model_pub_par_t));
				st = mesh_pub_par_check(p_model, &pub_set_temp);
				if(ST_SUCCESS == st){
				    change_falg = mesh_save_pub_and_refresh_tick(p_model, &pub_set_temp, p_pub_set_vr->pub_uuid);
				}
        	}else{
				mesh_cfg_model_pub_set_t *p_pub_set = (mesh_cfg_model_pub_set_t *)pub_set;
	        	if(is_virtual_adr(p_pub_set->pub_adr)){
                    //LOG_MSG_LIB(TL_LOG_NODE_SDK,0, 0,"yyy:0x%x,",st);
	        		st = ST_INVALID_ADR;
	        	}else{
	        		st = mesh_pub_par_check(p_model, p_pub_set);
					if(ST_SUCCESS == st){
                        change_falg = mesh_save_pub_and_refresh_tick(p_model, p_pub_set, 0);
		            }
	            }
            }
        }else{
			st = ST_INVALID_PUB_PAR;
        }
    }

    if((ST_SUCCESS == st) && change_falg){
		mesh_model_store(sig_model, model_id);
    }
    //LOG_MSG_LIB(TL_LOG_NODE_SDK,0, 0,"zzz:0x%x,",st);

	return st;
}

int mesh_pub_set(u8 *par, mesh_cb_fun_par_t *cb_par, u32 model_id, bool4 sig_model, int vr_flag)
{
	int err = 0;
    u8 st = ST_SUCCESS;
    mesh_cfg_model_pub_set_t *p_pub_set = (mesh_cfg_model_pub_set_t *)par;
	u16 ele_adr = p_pub_set->ele_adr;

    if(is_unicast_adr(ele_adr)){
        st = mesh_pub_search_and_set(ele_adr, (u8 *)p_pub_set, model_id, sig_model, vr_flag);
        if(st == ST_SUCCESS){
            // retrieve the parameters
            st = mesh_add_pkt_model_pub(ele_adr, model_id, sig_model, cb_par->adr_src);
            if(st == ST_SUCCESS){
                //refresh cb_ticks
                if(SIG_MD_HEALTH_SERVER == model_id){
                    model_sig_health.srv.com.cb_tick_ms = clock_time_ms();
                }
                #if 0 // SUBSCRIPTION_PUBLISH_SHARE_EN
                mesh_model_id_t md_bind;    
                if(mesh_is_existed_share_model(&md_bind, model_id, sig_model)){
                    mesh_pub_search_and_set(ele_adr, (u8 *)p_pub_set, md_bind.id, md_bind.sig_model, vr_flag);
                }
                #endif
            }
        }
    }else{
        st = ST_INVALID_ADR;
    }

	if(st != ST_SUCCESS){
		err = mesh_rsp_err_st_pub_status(st, ele_adr, model_id, sig_model, cb_par->adr_src);
	}

	return err;
}

int mesh_cmd_sig_cfg_model_pub_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    bool4 sig_model = (sizeof(mesh_cfg_model_pub_set_t) - 2 == par_len);
    mesh_cfg_model_pub_set_t *p_pub_set = (mesh_cfg_model_pub_set_t *)par;
    u32 model_id = sig_model ? (p_pub_set->model_id & 0xFFFF) : p_pub_set->model_id;
    return mesh_pub_set(par, cb_par, model_id, sig_model, 0);
}

int mesh_cmd_sig_cfg_model_pub_set_vr(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    bool4 sig_model = (sizeof(mesh_cfg_model_pub_virtual_set_t) - 2 == par_len);
    mesh_cfg_model_pub_virtual_set_t *p_pub_set = (mesh_cfg_model_pub_virtual_set_t *)par;
    u32 model_id = sig_model ? (p_pub_set->model_id & 0xFFFF) : p_pub_set->model_id;
    return mesh_pub_set(par, cb_par, model_id, sig_model, 1);
}

int mesh_cmd_sig_cfg_model_pub_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err = 0;
    u8 st = ST_UNSPEC_ERR;
    bool4 sig_model = (sizeof(mesh_cfg_model_pub_get_t) - 2 == par_len);
    mesh_cfg_model_pub_get_t *p_pub_get = (mesh_cfg_model_pub_get_t *)par;
    u32 model_id = sig_model ? (p_pub_get->model_id & 0xFFFF) : p_pub_get->model_id;
    
    if(is_unicast_adr(p_pub_get->ele_adr)){
        mesh_adr_list_t adr_list;
        st = find_ele_support_model_and_match_dst(&adr_list, p_pub_get->ele_adr, model_id, sig_model);
    	
        if((adr_list.adr_cnt)){
            st = mesh_add_pkt_model_pub(p_pub_get->ele_adr, model_id, sig_model, cb_par->adr_src);
        }
    }else{
        st = ST_INVALID_ADR;
    }

	if(st != ST_SUCCESS){
        err = mesh_rsp_err_st_pub_status(st, p_pub_get->ele_adr, model_id, sig_model, cb_par->adr_src);
	}
    
    return err;
}

int mesh_cmd_sig_cfg_model_pub_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_rsp_sub_status(u8 st, mesh_cfg_model_sub_set_t *p_set, bool4 sig_model, u16 adr_dst)
{
    mesh_cfg_model_sub_status_t sub_status_rsp;
    sub_status_rsp.status = st;
    memcpy(&sub_status_rsp.set, p_set, sizeof(sub_status_rsp.set));
    
    return mesh_tx_cmd_rsp_cfg_model(CFG_MODEL_SUB_STATUS, (u8 *)&sub_status_rsp, sizeof(sub_status_rsp)-FIX_SIZE(sig_model), adr_dst);
}

u8 mesh_sub_search_and_set(u16 op, u16 ele_adr, u16 sub_adr, u8 *uuid, u32 model_id, bool4 sig_model)
{
    u8 st = ST_UNSPEC_ERR;
    mesh_adr_list_t adr_list;
    st = find_ele_support_model_and_match_dst(&adr_list, ele_adr, model_id, sig_model);
	if(st != ST_SUCCESS) return st;
	
    if((adr_list.adr_cnt)){
        u8 model_idx = 0;
        model_common_t *p_model = (model_common_t *)mesh_find_ele_resource_in_model(ele_adr, model_id, sig_model,&model_idx, 0);
        if(p_model && (!p_model->no_sub)){
        	if(((CFG_MODEL_SUB_DEL_ALL != op)&&(0 == uuid)&&(!is_group_adr(sub_adr)))
        	|| (is_fixed_group(sub_adr))){
        		st = ST_INVALID_ADR;
        	}else{
				st = mesh_cmd_sig_cfg_model_sub_set2(op, sub_adr, uuid, p_model, model_id, sig_model);
            }
        }else{
            st = ST_NOT_SUB_MODEL;
        }
    }
    
	return st;
}

int mesh_cmd_sig_cfg_model_sub_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    u8 st = ST_UNSPEC_ERR;
    mesh_cfg_model_sub_set_t sub_set;
    memset(&sub_set, 0, sizeof(sub_set));
    mesh_cfg_model_sub_set_t *p_sub_set = &sub_set;
    p_sub_set->ele_adr = ((mesh_cfg_model_sub_set_t *)par)->ele_adr;
 
    if(!is_unicast_adr(p_sub_set->ele_adr) && !DEBUG_CFG_CMD_GROUP_AK_EN){
        return -1;
    }
    u16 op = cb_par->op;
    bool4 sig_model;
    u32 model_id;
    u8 *uuid = 0;
	u8 cfg_model_virtual=0;
    if(CFG_MODEL_SUB_DEL_ALL == op){
        mesh_cfg_model_sub_del_all_t *p_sub_del_all = (mesh_cfg_model_sub_del_all_t *)par;
        sig_model = (sizeof(mesh_cfg_model_sub_del_all_t) - 2 == par_len);
        model_id = sig_model ? (p_sub_del_all->model_id & 0xFFFF) : p_sub_del_all->model_id;
        
        p_sub_set->model_id = model_id;
    }else if((CFG_MODEL_SUB_VIRTUAL_ADR_ADD == op)
    	   ||(CFG_MODEL_SUB_VIRTUAL_ADR_DEL == op)
    	   ||(CFG_MODEL_SUB_VIRTUAL_ADR_OVER_WRITE == op)){
    	cfg_model_virtual = 1;
        mesh_cfg_model_sub_virtual_set_t *p_set_vr = (mesh_cfg_model_sub_virtual_set_t *)par;
        sig_model = (sizeof(mesh_cfg_model_sub_virtual_set_t) - 2 == par_len);
        model_id = sig_model ? (p_set_vr->model_id & 0xFFFF) : p_set_vr->model_id;
        
        p_sub_set->sub_adr = cala_vir_adr_by_uuid(p_set_vr->sub_uuid);
        p_sub_set->model_id = model_id;
        uuid = p_set_vr->sub_uuid;
    }else{
        memcpy(p_sub_set, par, par_len);
        sig_model = (sizeof(mesh_cfg_model_sub_set_t) - 2 == par_len);
        model_id = sig_model ? (p_sub_set->model_id & 0xFFFF) : p_sub_set->model_id;
    }
#if	DEBUG_CFG_CMD_GROUP_AK_EN
	if(cb_par->p_nw->dst == ADR_ALL_NODES){
		p_sub_set->ele_adr = ele_adr_primary;
	}
#endif
    st = mesh_sub_search_and_set(op, p_sub_set->ele_adr, p_sub_set->sub_adr, uuid, model_id, sig_model);
	if(ST_SUCCESS == st){
		share_model_sub_by_rx_cmd(op, p_sub_set->ele_adr, p_sub_set->sub_adr,cb_par->adr_dst, uuid, model_id, sig_model);
		#if FEATURE_LOWPOWER_EN		
		lpn_subsc_list_update_by_sub_set_cmd(op, p_sub_set->sub_adr);
		#endif
	}
	
	if((ST_SUCCESS != st)&&cfg_model_virtual){
		p_sub_set->sub_adr = 0;
	}
	return mesh_cmd_sig_cfg_model_sub_cb(st,p_sub_set,sig_model,cb_par->adr_src);
}

int mesh_cmd_sig_cfg_model_sub_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_sig_model_sub_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err = 0;
    u8 st = ST_UNSPEC_ERR;
    mesh_cfg_model_sub_get_sig_t *p_sub_get = (mesh_cfg_model_sub_get_sig_t *)par;

    if(!is_unicast_adr(p_sub_get->ele_adr)){
        return -1;
    }
    if(sizeof(mesh_cfg_model_sub_get_sig_t) == par_len){
    
        mesh_adr_list_t adr_list;
        st = find_ele_support_model_and_match_dst(&adr_list, p_sub_get->ele_adr, p_sub_get->model_id, 1);
        if((adr_list.adr_cnt)){
            st = mesh_add_pkt_model_sub_list(p_sub_get->ele_adr, p_sub_get->model_id, 1, cb_par->adr_src);
        }
    }
    
    if(ST_SUCCESS != st){
        err = mesh_rsp_err_st_sub_list(st, p_sub_get->ele_adr, p_sub_get->model_id, 1, cb_par->adr_src);
    }
    return err;
}

int mesh_cmd_sig_sig_model_sub_list(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_vendor_model_sub_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err = 0;
    u8 st = ST_UNSPEC_ERR;
    mesh_cfg_model_sub_get_vendor_t *p_sub_get = (mesh_cfg_model_sub_get_vendor_t *)par;

    if(!is_unicast_adr(p_sub_get->ele_adr)){
        return -1;
    }
    if(sizeof(mesh_cfg_model_sub_get_vendor_t) == par_len){
        mesh_adr_list_t adr_list;
        st = find_ele_support_model_and_match_dst(&adr_list, p_sub_get->ele_adr, p_sub_get->model_id, 0);
        if((adr_list.adr_cnt)){
            st = mesh_add_pkt_model_sub_list(p_sub_get->ele_adr, p_sub_get->model_id, 0, cb_par->adr_src);
        }
    }
    
    if(ST_SUCCESS != st){
        err = mesh_rsp_err_st_sub_list(st, p_sub_get->ele_adr, p_sub_get->model_id, 0, cb_par->adr_src);
    }
    return err;
}

int mesh_cmd_sig_vendor_model_sub_list(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

#if MD_SAR_EN
int mesh_cmd_sig_cfg_sar_transmitter_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    return mesh_tx_cmd_rsp_cfg_model(CFG_SAR_TRANSMITTER_STATUS, (u8 *)&g_mesh_model_misc_save.sar_transmitter, sizeof(g_mesh_model_misc_save.sar_transmitter), cb_par->adr_src);
}

int mesh_cmd_sig_cfg_sar_transmitter_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	memcpy(&g_mesh_model_misc_save.sar_transmitter, par, sizeof(sar_transmitter_t));
	g_mesh_model_misc_save.sar_transmitter.rfu = 0;
	mesh_model_sar_save();
    return mesh_cmd_sig_cfg_sar_transmitter_get(par, par_len, cb_par);
}

int mesh_cmd_sig_cfg_sar_receiver_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{  
	return mesh_tx_cmd_rsp_cfg_model(CFG_SAR_RECEIVER_STATUS, (u8 *)&g_mesh_model_misc_save.sar_receiver, sizeof(g_mesh_model_misc_save.sar_receiver), cb_par->adr_src);
}

int mesh_cmd_sig_cfg_sar_receiver_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    memcpy(&g_mesh_model_misc_save.sar_receiver, par, sizeof(g_mesh_model_misc_save.sar_receiver));
	g_mesh_model_misc_save.sar_receiver.rfu = 0;
	mesh_model_sar_save();
    return mesh_cmd_sig_cfg_sar_receiver_get(par, par_len, cb_par);
}
#endif

#if MD_ON_DEMAND_PROXY_EN
int mesh_cmd_sig_cfg_on_demand_proxy_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{  
	return mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp, &g_mesh_model_misc_save.on_demand_proxy, 1, cb_par->adr_src);
}

int mesh_cmd_sig_cfg_on_demand_proxy_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	g_mesh_model_misc_save.on_demand_proxy = par[0];
	mesh_model_on_demand_save();
    return mesh_cmd_sig_cfg_on_demand_proxy_get(par, par_len, cb_par);
}

#endif

int mesh_cmd_sig_cfg_sec_nw_bc_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_def_ttl_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_friend_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_gatt_proxy_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_relay_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_nw_transmit_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

#if MD_CLIENT_EN
#if MD_SAR_EN
int mesh_cmd_sig_cfg_sar_transmitter_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_sar_receiver_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}
#endif

#if MD_ON_DEMAND_PROXY_EN
int mesh_cmd_sig_cfg_on_demand_proxy_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}
#endif

#if MD_LARGE_CPS_EN
int mesh_cmd_sig_cfg_large_cps_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_models_metadata_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}
#endif

#endif

