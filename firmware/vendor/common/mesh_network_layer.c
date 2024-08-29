/********************************************************************************************************
 * @file	mesh_network_layer.c
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
#include "mesh_network_layer.h"
#include "directed_forwarding.h"
#include "subnet_bridge.h"

//--------------------network layer callback------------------------------//
#if !WIN32
int mesh_rc_network_layer_cb(mesh_cmd_bear_t *p_bear, mesh_nw_retrans_t *p_nw_retrans)
{
	int conn_idx = 0;
	
   if((MESH_BEAR_GATT == p_nw_retrans->src_bear) && is_unicast_adr(p_bear->nw.src)){		
		#if BLE_MULTIPLE_CONNECTION_ENABLE
		bear_conn_handle_t *p_bear_handle = (bear_conn_handle_t *)&p_bear->tx_head;
		if(BEAR_TX_PAR_TYPE_CONN_HANDLE == p_bear_handle->par_type){
			conn_idx = get_slave_idx_by_conn_handle(p_bear_handle->conn_handle);
		}
		#endif
		
	  	proxy_proc_filter_mesh_cmd(conn_idx, p_bear->nw.src);	 
   	}

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
					p_nw_retrans->dpt_ele_cnt = get_directed_friend_dependent_ele_cnt(mesh_key.netkey_sel_dec, p_bear->nw.src);					
					if(p_fwd_entry){
						mesh_df_path_monitoring(p_fwd_entry);						
						if(is_addr_in_entry(p_bear->nw.src, p_bear->nw.dst, p_fwd_entry)){	// p_bear->nw.src exist in dependent list.
							p_nw_retrans->sec_type = DIRECTED;
						}
						else if(!p_fwd_entry->fixed_path){
							p_nw_retrans->pending_op = CMD_CTL_DEPENDENT_NODE_UPDATE;
						}
						else{
							//Create Path Origin State Machine
							p_nw_retrans->pending_op = CMD_CTL_PATH_REQUEST;
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
			if((MASTER == mesh_key.sec_type_sel) && is_proxy_use_directed(conn_idx, mesh_key.netkey_sel_dec)){	
				p_nw_retrans->dpt_ele_cnt = get_directed_proxy_dependent_ele_cnt(conn_idx, mesh_key.netkey_sel_dec, p_bear->nw.src);
				if(p_nw_retrans->dpt_ele_cnt){
					p_fwd_entry = get_forwarding_entry(p_nw_retrans->netkey_sel_enc, ele_adr_primary, p_bear->nw.dst);
					if(p_fwd_entry){
						mesh_df_path_monitoring(p_fwd_entry);					
						if(is_addr_in_entry(p_bear->nw.src, p_bear->nw.dst, p_fwd_entry)){ // p_bear->nw.src exist in dependent list.
							p_nw_retrans->sec_type = DIRECTED; 
						}
						else if(!p_fwd_entry->fixed_path){
							p_nw_retrans->pending_op = CMD_CTL_DEPENDENT_NODE_UPDATE;
						}
						else{
							//Create Path Origin State Machine
							p_nw_retrans->pending_op = CMD_CTL_PATH_REQUEST;
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
				if(is_addr_in_entry(p_bear->nw.src, p_bear->nw.dst, p_fwd_entry)){	// p_bear->nw.src exist in dependent list.
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

#if MESH_RX_TEST
	if((CMD_CTL_TTC_CMD == p_bear->lt_ctl_unseg.opcode) || (CMD_CTL_TTC_CMD_NACK == p_bear->lt_ctl_unseg.opcode) || (CMD_CTL_TTC_CMD_STATUS == p_bear->lt_ctl_unseg.opcode)){
		p_nw_retrans->sec_type = SEC_TYPE_NONE;		// no encryption in network layer, will encrypt in relay_adv_prepare_handler().
		adv_report_extend_t * pa = get_adv_report_extend(&p_bear->len);
		p_bear->tx_head.par_type = BEAR_TX_PAR_TYPE_MESH_RX_TEST;
		u32 base_tick = pa->timeStamp >> RX_TEST_BASE_TIME_SHIFT;
		p_bear->tx_head.val[0] = (u8)base_tick;
		p_bear->tx_head.val[1] = base_tick >> 8;
		p_bear->tx_head.val[2] = base_tick >> 16;
	}
#endif

	int need_relay = 0;

	#ifndef WIN32
	if(MESH_BEAR_GATT == p_nw_retrans->src_bear){
		#if (FEATURE_PROXY_EN)
		if(p_nw_retrans->print_proxy2adv_error){
			// please check App setting of extend ADV mode
			LOG_MSG_LIB(TL_LOG_NODE_BASIC,0, 0 ,"can not proxy to ADV due to len overflow"); // for remind checking exent ADV setting on App.
		}
		#endif
	}else{
		need_relay = !is_pkt_notify_only(p_bear->nw.dst, 1);
	}

	#if (0 == ALONE_BUFF_FOR_RELAY_EN)
	if((blc_ll_getCurrentState() == BLS_LINK_STATE_CONN) && my_fifo_data_cnt_get(&mesh_adv_cmd_fifo)){
		need_relay = 0;
	}
	#endif
	#endif
		
	if(need_relay == 0){
		p_nw_retrans->relay_en = 0;
	}

	#if DF_TEST_MODE_EN
	if((MESH_BEAR_GATT == p_nw_retrans->src_bear) || ((p_bear->nw.ttl >= 2) && ((!is_busy_mesh_tx_cmd(0)) && (p_nw_retrans->relay_en || p_nw_retrans->need_bridge)))){
		if(DIRECTED == p_nw_retrans->sec_type){
			mesh_df_led_event(mesh_key.net_key[0][0].nid_d, p_bear->nw.ctl);
		}else{
			#if DF_TEST_EXHIBITION_EN
			if((!p_bear->nw.ctl) && (!is_directed_forwarding_en(mesh_key.netkey_sel_dec))){
				mesh_cmd_lt_unseg_t *p_lt_unseg2 = &p_bear->lt_unseg;
				if(!p_lt_unseg2->seg){
					u8 ut_unseg_dec[6]; // include MIC, just decryp the first 2 bytes.
					memcpy(&ut_unseg_dec, p_bear->lt_unseg.data, sizeof(ut_unseg_dec));
		            mesh_sec_msg_dec_apl(ut_unseg_dec, sizeof(ut_unseg_dec), (u8 *)&p_bear->nw, 0);

					u16 df_opcode = ut_unseg_dec[1] << 8 | ut_unseg_dec[0];			
					if((df_opcode == G_ONOFF_SET) && (p_bear->nw.dst != 0xffff)){
						mesh_df_led_event(mesh_key.net_key[0][0].nid_d, p_bear->nw.ctl);
					}
				}
			}
			#endif
		}
	}
	#endif
			
	return 0;
}

u16 mesh_tx_network_layer_cb(mesh_cmd_bear_t *p_bear, mesh_match_type_t *p_match_type, u8 *p_sec_type)
{
	__UNUSED u16 path_target = 0;	
	
#if (MD_DF_CFG_SERVER_EN)
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
	
#endif

#if MESH_RX_TEST
	if((CMD_CTL_TTC_CMD == p_bear->lt_ctl_unseg.opcode) || (CMD_CTL_TTC_CMD_NACK == p_bear->lt_ctl_unseg.opcode) || (CMD_CTL_TTC_CMD_STATUS == p_bear->lt_ctl_unseg.opcode)){
		*p_sec_type = SEC_TYPE_NONE;	// no encryption in network layer, will encrypt in app_advertise_prepare_handler().
		p_bear->nw.ttl = mesh_get_ttl(); // because it was set to 0 in mesh_tx_cmd_layer_upper_ctl_ll.
	}
#endif

#if DF_TEST_MODE_EN
	if(DIRECTED == *p_sec_type){
		mesh_df_led_event(mesh_key.net_key[0][0].nid_d, p_bear->nw.ctl);
	}
#endif

#if MESH_TX_RX_SELF_EN
	mesh_set_iv_idx_rx(p_bear->nw.ivi); 
	is_exist_in_cache((u8 *)&p_bear->nw, 0, 1); // update RPL to prevent receiving it again.
#endif

#if FEATURE_LOWPOWER_EN
	#if BLE_MULTIPLE_CONNECTION_ENABLE
	if(p_bear->nw.ctl){
		p_match_type->trans.invl_steps = 0; // quick send control message, such as friend poll.
		LOG_USER_MSG_INFO(0, 0, "lpn_quick", 0);
	}
	#endif
	
	if(is_lpn_support_and_en){	
		#if 0 //LPN_RESPONSE_WITH_SAME_KEY //  should not always return friend key, only when LPN want to send message with DF path. details refer to "3.6.6.2 Friendship security"
		if(p_match_type->mat.immutable_flag && is_friend_ship_link_ok_lpn()){
			if(mesh_key.sec_type_sel <= FRIENDSHIP){
				*p_sec_type = mesh_key.sec_type_sel;
			}
		}
		#endif
	}
#endif

	return path_target;
}

int mesh_network_df_initial_start(u8 op, u16 netkey_offset, u16 destination, u16 dpt_org, u16 dpt_ele_cnt)
{
	int ret = 0;
	
#if (MD_DF_CFG_SERVER_EN)
	if(CMD_CTL_DEPENDENT_NODE_UPDATE == op){ 	
		ret = directed_forwarding_dependents_update_start(netkey_offset, DEPENDENT_TYPE_ADD, ele_adr_primary, dpt_org, dpt_ele_cnt);
	}
	else if(CMD_CTL_PATH_REQUEST == op){ 		
		ret = directed_forwarding_initial_start(netkey_offset, destination, dpt_org, dpt_ele_cnt);
	}
#endif

	return ret;
}
#endif
//--------------------network layer callback end------------------------------//

// ---- message relay protect list process
cache_buf_t cache_buf[CACHE_BUF_MAX];
u16 g_cache_buf_max = CACHE_BUF_MAX;
#ifndef WIN32
//STATIC_ASSERT(sizeof(cache_buf_t) % 4 == 0);   // for compare sno quickly
STATIC_ASSERT(CACHE_BUF_MAX > 0);
#endif

/**
 * @brief       This function servers to init RPL.
 * @return      none
 * @note        
 */
void mesh_network_cache_buf_init()
{
    memset(cache_buf, 0, sizeof(cache_buf));
}

/**
 * @brief       This function servers to init item in RPL.
 * @param[in]   ele_adr	- element address in RPL. if it's a unicast address, just init the corresponding item, otherwise init all items.
 * @return      none
 * @note        
 */
void cache_init(u16 ele_adr)
{
    int del_all = !is_unicast_adr(ele_adr);
    if(del_all){
        mesh_network_cache_buf_init();
    }else{
        foreach(i, g_cache_buf_max){
            if(ele_adr == cache_buf[i].src){
                memset(&cache_buf[i], 0, sizeof(cache_buf[i]));
                break;
            }
        }
    }
	
	if((mesh_rx_seg_par.src != 0) &&  (0 == mesh_rx_seg_par.tick_last)){
		mesh_seg_rx_init();
	}
}


/**
 * @brief       This function servers to update RPL.
 * @param[in]   p	- pointer of network PDU.
 * @param[in]   idx	- index of item in RPL.
 * @return      none
 * @note        
 */
void mesh_update_rpl(u8 *p, int idx)
{    // can not use op code,because it is encryption now
    mesh_cmd_nw_t *p_nw = (mesh_cmd_nw_t *)p;

    //memset(&cache_buf[idx], 0, sizeof(cache_buf_t));    // init
    cache_buf[idx].src = p_nw->src;
    cache_buf[idx].ivi = (u8)iv_idx_st.iv_rx;
    u32 sno = 0;
    memcpy(&sno, p_nw->sno, SIZE_SNO);
    cache_buf[idx].sno = sno;
}

/**
 * @brief       This function servers to handle message against replay attacks and update replay protection list.
 * @param[in]   p				- pointer of network PDU.
 * @param[in]   friend_key_flag	- 0: the PDU is not decrypted by friend key. 1: the PDU is decrypted by friend key. 
 * @param[in]   save			- 0: not need to update RPL. 1: need to update RPL.
 * @return      1: the network PDU will be discarded since SEQ is old. 0: the network PDU will will not be discarded.
 * @note        if parameter "save" is 0, means the network PDU was just handled by network layer obfuscation, we can discard the PDU in advance because the source address and the sequence number of the network PDU are known after obfuscation.
 *				if parameter "save" is 1, means the network PDU has been successfully decrypted, need update RPL. 
 */
int is_exist_in_cache(u8 *p, u8 friend_key_flag, int save)
{
#define MESH_SR_PROX_BVXY3C 	0

#if MESH_MONITOR_EN
    if(!monitor_filter_sno_en){
        return 0;
    }
#endif

#if (MESH_TX_RX_SELF_EN || IV_UPDATE_DISABLE)
	u32 sno_delta = 0;
	int update_flag = 0;
	int exist_old_idx = 0;
#endif

    mesh_cmd_nw_t *p_nw = (mesh_cmd_nw_t *)p;   // only ivi, src and sno can be use when "save" is 0.
    int exist_old = 0;

	foreach(i, g_cache_buf_max){
        if(p_nw->src == cache_buf[i].src){
            u32 sno = 0;
            memcpy(&sno, p_nw->sno, SIZE_SNO);
            #if FEATURE_LOWPOWER_EN
			static u32 lpn_skip_cache_cnt;
			#endif
			
			if(((u8)iv_idx_st.iv_rx) != cache_buf[i].ivi){
			    if(save){ // decryption ok
			        if(iv_idx_st.iv_rx != iv_idx_st.iv_cur){ // means ivi is less 1 than current ivi
			        // this case happen means ivi of cache is new, iv rx is old, which may happen when other nodes relay with old iv.
			        //such as some nodes may relay with small iv but not original iv in raw packet. for example, both cache and current have been 1, and node A tx command with 1 but this command may be relay with 0 and then cause this case.
			        	#if MESH_SR_PROX_BVXY3C // TODO: just for private beacon proxy bv-03c which haiwei think case is not reasonable. //TESTCASE_FLAG_ENABLE
						// private beacon proxy bv-03c, it need to relay by the old ivi.
						LOG_MSG_INFO(TL_LOG_MESH,0, 0 ,"is_exist_in_cache1 ivi %d",p_nw->ivi);
						// should only relay, but now it will both relay and handle. fix later.
						// but if relay without saving the sno, it may case sending too many relay packet.
						return 0;
						#else
			            return 1; // exist_old = 1;
						#endif
			        }else{
			        	// will call add2cache_() to clear and update cache. 
			        }
			    }
			}
			#if !(MESH_TX_RX_SELF_EN || IV_UPDATE_DISABLE)
			else if(sno <= cache_buf[i].sno){ // && (iv_idx_st.rx[3] == cache_buf[i].ivi)){
            	#if FEATURE_LOWPOWER_EN
            	int lpn_seg_cache_flag = is_in_mesh_friend_st_lpn() && friend_key_flag;
            	if((0 == save) && lpn_seg_cache_flag){
            	    return 0;   // if "save" is 0, means "lt" is still in encryption state.
            	}
            	
                mesh_cmd_bear_t *p_bear = CONTAINER_OF(p_nw,mesh_cmd_bear_t,nw);
                if((save && p_bear->lt_unseg.seg) && lpn_seg_cache_flag){
                	if(sno == cache_buf[i].sno){// because Friend send cache message if from the last one.
                    	if(lpn_skip_cache_cnt++ < 16){
                    		exist_old = 1;
                    	}
                    }
                }else
                #endif
                {
                    if((!factory_test_cache_not_equal_mode_en) || !is_activated_factory_test_mode() || (sno == cache_buf[i].sno)){
                	    exist_old = 1;
                	}
                }
            }
            
            if(!exist_old && save){
                #if FEATURE_LOWPOWER_EN
            	lpn_skip_cache_cnt = 0;		// init
            	#endif
                mesh_update_rpl(p, i);            // update
            }
            
            return exist_old;
            #else
			else if(sno <= cache_buf[i].sno){
				sno_delta = cache_buf[i].sno - sno;
				if(sno_delta >= (IV_UPDATE_START_SNO - 1 - RPL_PROTECT_SNO_DELTA)){ // update RPL, mesh_adv_tx_cmd_sno loop back to 1 after reach IV_UPDATE_START_SNO in mesh_iv_update_start_check().
					update_flag = 1;
				}
				else if(sno_delta <= RPL_PROTECT_SNO_DELTA){	// exist in RPL.
					exist_old = 1;
					exist_old_idx = i;
				}
			}else{
				sno_delta = sno - cache_buf[i].sno;
				if(sno_delta <= RPL_PROTECT_SNO_DELTA){
					update_flag = 1;
				}
				else if(sno_delta >= (IV_UPDATE_START_SNO - 1 - RPL_PROTECT_SNO_DELTA)){ // exist in RPL, mesh_adv_tx_cmd_sno loop back to 1 after reach IV_UPDATE_START_SNO in mesh_iv_update_start_check().
					exist_old = 1;
					exist_old_idx = i;
				}					
			}
			#endif          
        }

		#if (MESH_TX_RX_SELF_EN || IV_UPDATE_DISABLE)
		if(update_flag){			
			if(save){
				if(exist_old){
					memset(&cache_buf[exist_old_idx], 0, sizeof(cache_buf[exist_old_idx])); // clear the old one.
				}
				mesh_update_rpl(p, i); 	// update RPL.           
			}
			
			return 0;
		}

		if((i == (CACHE_BUF_MAX -1)) && exist_old){	
			return 1;	// exist in RPL.
		}
		#endif
    }

    if(save){
        //new device found
        mesh_add_rpl(p);
    }
    return 0;
}

/**
 * @brief       This function servers to get the number of items in RPL.
 * @return      the number of items in RPL.
 * @note        
 */
u16 get_mesh_current_cache_num() // Note, there may be several elements in a node, but there is often only one element that is in cache.
{
    u16 num = 0;
    foreach(i, g_cache_buf_max){
        if(cache_buf[i].src != 0){
            num++;
        }
   }
    return num;
}
