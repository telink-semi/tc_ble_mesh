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
#include "proj/tl_common.h"
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
					bls_ll_terminateConnection(0x13);
				}
				#endif
				#endif
			}	
		}else{
			par[0] = model_sig_cfg_s.gatt_proxy;
		}
		#if MD_PRIVACY_BEA&&!WIN32&&MD_SERVER_EN
		u8 *p_private_sts = &(model_private_beacon.srv[0].proxy_sts);
		mesh_private_proxy_change_by_gatt_proxy(*p_private_sts,p_private_sts);
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

				#if (MD_DF_EN && !FEATURE_LOWPOWER_EN && !WIN32)
				directed_forwarding_solication_start(mesh_key.netkey_sel_dec, (mesh_ctl_path_request_solication_t *)&sub_adr, 1);
				#endif
			}
        }
        st = add_ok ? ST_SUCCESS : ST_INSUFFICIENT_RES;
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

#if MD_SAR_EN
int mesh_cmd_sig_cfg_sar_transmitter_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    return mesh_tx_cmd_rsp_cfg_model(CFG_SAR_TRANSMITTER_STATUS, (u8 *)&model_sig_cfg_s.sar_transmitter, sizeof(model_sig_cfg_s.sar_transmitter), cb_par->adr_src);
}

int mesh_cmd_sig_cfg_sar_transmitter_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	memcpy(&model_sig_cfg_s.sar_transmitter, par, sizeof(sar_transmitter_t));
	model_sig_cfg_s.sar_transmitter.rfu = 0;
	mesh_model_store_cfg_s();
    return mesh_cmd_sig_cfg_sar_transmitter_get(par, par_len, cb_par);
}

int mesh_cmd_sig_cfg_sar_receiver_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{  
	return mesh_tx_cmd_rsp_cfg_model(CFG_SAR_RECEIVER_STATUS, (u8 *)&model_sig_cfg_s.sar_receiver, sizeof(model_sig_cfg_s.sar_receiver), cb_par->adr_src);
}

int mesh_cmd_sig_cfg_sar_receiver_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    memcpy(&model_sig_cfg_s.sar_receiver, par, sizeof(model_sig_cfg_s.sar_receiver));
	model_sig_cfg_s.sar_receiver.rfu = 0;
	mesh_model_store_cfg_s();
    return mesh_cmd_sig_cfg_sar_receiver_get(par, par_len, cb_par);
}
#endif

#if MD_ON_DEMAND_PROXY_EN
int mesh_cmd_sig_cfg_on_demand_proxy_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{  
	return mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp, &model_sig_cfg_s.on_demand_proxy, 1, cb_par->adr_src);
}

int mesh_cmd_sig_cfg_on_demand_proxy_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	model_sig_cfg_s.on_demand_proxy = par[0];
	mesh_model_store_cfg_s();
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

