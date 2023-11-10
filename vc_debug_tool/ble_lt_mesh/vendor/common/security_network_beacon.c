/********************************************************************************************************
 * @file	security_network_beacon.c
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
#include "security_network_beacon.h"
#include "blt_soft_timer.h"

#if IV_UPDATE_SKIP_96HOUR_EN
u8 iv_update_test_mode_en = 1;
u8 iv_idx_update_change2next_st = 0;
#endif
static u8 beacon_iv_update_pkt = 0;

int is_enough_time_keep_state()
{
	return (is_iv_update_keep_enough_time_ll()
		 #if IV_UPDATE_SKIP_96HOUR_EN
		 || (iv_update_test_mode_en && iv_idx_update_change2next_st)
		 #endif
		 );
}

int is_iv_index_invalid()
{
	return (MESH_INVALID_IV_INDEX == iv_idx_st.iv_cur);
}

void mesh_iv_idx_init(u32 iv_index, int rst_sno, int save_flag)
{
	u8 update_flag_backup = iv_idx_st.update_trigger_by_save;
	memset(&iv_idx_st, 0, sizeof(iv_idx_st));
	iv_idx_st.iv_cur = iv_index;
	iv_idx_st.iv_tx = iv_index;
	
    if(rst_sno){
        mesh_adv_tx_cmd_sno = 1;    // default value
        save_flag = 1;
    }else{
        iv_idx_st.update_trigger_by_save = update_flag_backup;
    }
	mesh_iv_idx_init_cb(rst_sno);
    mesh_rx_seg_par.seqAuth = 0;
    mesh_ivi_event_cb(iv_idx_st.searching_flag);
    #if IV_UPDATE_SKIP_96HOUR_EN
    iv_idx_update_change2next_st = 0;
    #endif

    if(save_flag){
    	mesh_misc_store();
    	#if WIN32
    	if(rst_sno){
    	}
    	#endif
    }
}

void mesh_iv_update_enter_search_mode()
{
    mesh_iv_idx_init(iv_idx_st.iv_cur, 0, 0);
    iv_idx_st.searching_flag = 1;
    LOG_MSG_INFO(TL_LOG_IV_UPDATE,(u8 *)&iv_idx_st, sizeof(iv_idx_st),"IV index searching:",0);
}

#if TESTCASE_FLAG_ENABLE
void mesh_iv_update_enter_normal()	// just for test case // enter normal with step 0.
{
	mesh_iv_idx_init(iv_idx_st.iv_cur, 0, 0);
}

void mesh_iv_update_enter_update_progress()
{
	iv_idx_st.rx_update = 1;
	iv_idx_st.searching_flag = 0;
	iv_idx_update_change2next_st = 0;
}
#endif

void mesh_iv_update_enter_update2normal() // enter normal with step2, not normal with step 0.
{
	mesh_iv_idx_init(iv_idx_st.iv_cur, 1, 0);
	iv_idx_st.update_proc_flag = IV_UPDATE_STEP2;
	mesh_ivi_event_cb(iv_idx_st.searching_flag);
	mesh_node_identity_refresh();
    LOG_MSG_LIB(TL_LOG_IV_UPDATE,(u8 *)&iv_idx_st, sizeof(iv_idx_st),"IV index enter step2: ",0);
	mesh_iv_update_report_between_gatt();
}

void set_iv_update_rx_flag()
{
    iv_idx_st.rx_update = 1;
    beacon_iv_update_pkt = 1;
	mesh_ivi_event_cb(iv_idx_st.searching_flag);
}

void mesh_check_and_set_iv_update_rx_flag(u32 iv_idx)
{
    if(!iv_idx_st.update_proc_flag){
        if(iv_idx - iv_idx_st.iv_cur == 1){
            set_iv_update_rx_flag();
        }
    }
    // if push for every iv update messages, it would cause FN cache fifo be full with friend update messages.
}

void mesh_iv_update_report_between_gatt()
{
#if WIN32
    // if(connect_flag) // no need
#else
	if(blt_state == BLS_LINK_STATE_CONN)
#endif
	{
		mesh_tx_sec_private_beacon_proc(1);
	}

#if MD_PRIVACY_BEA
	mesh_prov_para_random_generate();
#endif
}

int mesh_iv_update_step_change()
{
	int err = -1;
	if(IV_UPDATE_STEP1 == iv_idx_st.update_proc_flag){
		// init parameters
		if(!is_busy_segment_or_reliable_flow()){
			mesh_iv_update_enter_update2normal(); // enter step2
			err = 0;
		}
	}else if(IV_UPDATE_STEP2 == iv_idx_st.update_proc_flag){
		if(!is_busy_segment_or_reliable_flow()){
			#if MD_PRIVACY_BEA
			mesh_prov_para_random_generate(); // maybe no need
			#endif
			iv_idx_st.update_proc_flag = 0;
			mesh_ivi_event_cb(iv_idx_st.searching_flag);
			err = 0;
            LOG_MSG_LIB(TL_LOG_IV_UPDATE,(u8 *)&iv_idx_st, sizeof(iv_idx_st),"IV index enter Normal:",0);
		}
	}
	
	#if IV_UPDATE_SKIP_96HOUR_EN
	if(0 == err){
		iv_idx_update_change2next_st = 0;
	}
	#endif

	return err;
}


void get_iv_update_key_refresh_flag(mesh_ctl_fri_update_flag_t *p_flag_out, u8 *iv_idx_out, u8 key_phase)
{
    p_flag_out->IVUpdate = (IV_UPDATE_STEP1 == iv_idx_st.update_proc_flag);
    p_flag_out->KeyRefresh = (KEY_REFRESH_PHASE2 == key_phase);
	get_iv_big_endian(iv_idx_out, (u8 *)&iv_idx_st.iv_cur);
}

void iv_update_set_with_update_flag_ture(u32 iv_idx, u32 search_mode)
{
    u32 iv_save = iv_idx - 1;			// -1
    int rst_sno = (iv_save != iv_idx_st.iv_cur);
    mesh_iv_idx_init(iv_save, rst_sno, 1);
    iv_idx_st.iv_cur += 1; 	// +1
    mesh_iv_update_set_start_flag(0);
	mesh_ivi_event_cb(iv_idx_st.searching_flag);
    if(search_mode){
        LOG_MSG_INFO(TL_LOG_IV_UPDATE,(u8 *)&iv_idx_st, sizeof(iv_idx_st),"IV index enter step1 by search: ",0);
    }else{
        LOG_MSG_INFO(TL_LOG_IV_UPDATE,(u8 *)&iv_idx_st, sizeof(iv_idx_st),"IV index enter step1 by provision: ",0);
    }
}

int iv_update_key_refresh_rx_handle_cb(mesh_ctl_fri_update_flag_t *p_ivi_flag, u32 iv_idx)
{
	#if __PROJECT_MESH_SWITCH__
	LOG_MSG_INFO(TL_LOG_IV_UPDATE,0, 0,"switch receive security network beacon time_s:%d", clock_time_s());
	extern int soft_timer_rcv_beacon_timeout();
	soft_timer_rcv_beacon_timeout();
	blt_soft_timer_delete(&soft_timer_rcv_beacon_timeout);
	switch_iv_update_time_refresh(); 
	#endif
	return 0;
}

int iv_update_key_refresh_rx_handle(mesh_ctl_fri_update_flag_t *p_ivi_flag, u8 *p_iv_idx)
{
	// copy to make sure not be over write, even though adv filter function may have checked to prevent being over write.
	mesh_ctl_fri_update_flag_t p_flag[1];
	u32 iv_idx;
	get_iv_little_endian((u8 *)&iv_idx, p_iv_idx);
	p_flag[0]= *p_ivi_flag;
	#if TESTCASE_FLAG_ENABLE
	LOG_MSG_INFO(TL_LOG_IV_UPDATE,0, 0,"rx sec beacon, flag:0x%x, iv:0x%x",*((u8 *)p_flag),iv_idx);
	#endif
	iv_update_key_refresh_rx_handle_cb(p_ivi_flag, iv_idx);

	#if GATEWAY_ENABLE
	if(is_iv_index_invalid()){
		mesh_iv_idx_init(iv_idx, 1, 1);
		LOG_MSG_LIB(TL_LOG_IV_UPDATE,(u8 *)&iv_idx_st, sizeof(iv_idx_st),"Gateway recover IV index",0);	
	}
	#endif
	
	if(iv_idx < iv_idx_st.iv_cur){		// less
        return -1;
    }
    
	if((u32)(iv_idx - iv_idx_st.iv_cur) >= (42 + 1)){
        return -1;
    }

	// has been sure that "iv rx >= iv cur", and "iv rx <= iv cur + 42" here.
    
    /************************ key refresh handle ****************/
    mesh_key_refresh_rx_fri_update(p_flag->KeyRefresh);

    /************************ iv update handle ****************/
    if(iv_idx_st.searching_flag){
        /*only (>=0 && <=1) is valid for APP, because filter set command is used before receive beacon.*/
		if(iv_idx - iv_idx_st.iv_cur >= 1){
            if(p_flag->IVUpdate){
                if((iv_idx - iv_idx_st.iv_cur == 1) && (!iv_idx_st.update_proc_flag)){
                    // active at once, need to consider about rx segment busy flow.
                    set_iv_update_rx_flag();
                    iv_idx_st.searching_flag = 0;    // for spec "3.10.6 IV Index Recovery procedure", it should exist searching mode and keep current step for 96 hours
                }else{
                    iv_update_set_with_update_flag_ture(iv_idx, 1); // active at once, regardless of rx segment busy state.
                    iv_idx_st.searching_flag = 0;
                }
            }else{
                mesh_iv_idx_init(iv_idx, 1, 1);
                iv_idx_st.searching_flag = 0;
				mesh_iv_update_report_between_gatt();
				LOG_MSG_LIB(TL_LOG_IV_UPDATE,(u8 *)&iv_idx_st, sizeof(iv_idx_st),"IV index discover new iv:",0);
            }
        }else if(iv_idx >= iv_idx_st.iv_cur){
            if(IV_UPDATE_NORMAL == iv_idx_st.update_proc_flag){
                // skip
            }else if(IV_UPDATE_STEP1 == iv_idx_st.update_proc_flag){
                if(p_flag->IVUpdate){
                    // skip
                }else{
                    mesh_iv_update_enter_update2normal();  // enter step2 // will set iv_idx_st.searching_flag = 0 inside
                }
            }else if(IV_UPDATE_STEP2 == iv_idx_st.update_proc_flag){
                if(p_flag->IVUpdate){
                    // skip
                }else{
                    // keep 96 hour
                }
            }
        }else{
            // no need else case, because have returned before.
        }

        #if FEATURE_LOWPOWER_EN
        if(is_lpn_support_and_en && (0 == iv_idx_st.rx_beacon_ok_after_searching)){
            #ifndef WIN32
			if(BLS_LINK_STATE_CONN != blt_state)
			#endif
			{
            	suspend_enter(200, 0);		// delay for FN send beacon ok.
                mesh_friend_ship_set_st_lpn(FRI_ST_REQUEST);
            }
        }
        iv_idx_st.rx_beacon_ok_after_searching = 1;	// will be cleared when enter search mode.
        #endif
    }

    // no else, because need to handle in both normal mode or searching mode.
    {
    	if(IV_UPDATE_NORMAL == iv_idx_st.update_proc_flag){	// normal mode
    		if(iv_idx - iv_idx_st.iv_cur >= 2){		// have been sure "<= i + 42" before.
    		    // from spec v1.0.1,  section 3.1.5"IV Update procedure" page 130.
    			mesh_iv_update_enter_search_mode();
    			return 0;
    		}
    	}
        
		/************************ iv update handle ****************/
    	// iv index delta = + 1 or +0;
        if(p_flag->IVUpdate){
			if((iv_idx > iv_idx_st.iv_cur) && (IV_UPDATE_NORMAL == iv_idx_st.update_proc_flag)){
				mesh_net_key_t * p_nk = is_mesh_net_key_exist(NET_KEY_PRIMARY);
				if(p_nk){
					if(NET_KEY_PRIMARY != mesh_key.net_key[mesh_key.netkey_sel_dec][0].index){
						LOG_MSG_INFO(TL_LOG_IV_UPDATE,0, 0,"ignore subnet beacon which iv update flag ==1",0);
						return -1;	// ignore (iv update flag ==1) from subnet, IVU_BI04
					}
				}
			}

            mesh_check_and_set_iv_update_rx_flag(iv_idx);
        }else{
			if(iv_idx_st.update_proc_flag){
				if(iv_idx == iv_idx_st.iv_cur){
					// enter normal mode at once ?
					if(IV_UPDATE_STEP1 == iv_idx_st.update_proc_flag){
					    #if 0 // for which case ??? comfirm later
                        int searching_trigger_update = (0 == memcmp(iv_idx_st.tx, iv_idx_st.cur, 4)); // should never ture
                        if(searching_trigger_update){   // why ?? 
                            LOG_MSG_INFO(TL_LOG_IV_UPDATE,0, 0,"into step2, and reset sno ",0);
    						mesh_iv_update_enter_update2normal();    // enter step2 // attention: will reset sno
						}
						#elif (IV_UPDATE_SKIP_96HOUR_EN)
						iv_idx_update_change2next_st = 1;   // skip 96 hour
						#else
						if(is_iv_update_keep_enough_time_rx()){
						    mesh_iv_update_step_change();   // if it's segment busy now, discard it. confirm later.
						}
						#endif
					}else if(IV_UPDATE_STEP2 == iv_idx_st.update_proc_flag){
						#if (IV_UPDATE_SKIP_96HOUR_EN)
						iv_idx_update_change2next_st = 1;   // skip 96 hour
						#endif
					}
				}else{	// delta == 1
					// discard
				}
			}else{
				if(iv_idx - iv_idx_st.iv_cur == 1){
					// accept at once ? or discard ? or enter search mode ? spec V1.01 "3.10.5 IV Update procedure"
					mesh_receive_ivi_plus_one_in_normal_cb();
					// discard, refer to TestCase spec 4.10.8
				}else{	// delta == 0
					// discard
				}
			}
        }
    }

    return 0;
}

int mesh_rc_data_beacon_sec (u8 *p_payload, u32 t)
{
	int err = 0;
    mesh_cmd_bear_t *bc_bear = GET_BEAR_FROM_ADV_PAYLOAD(p_payload);
    mesh_beacon_t *p_bc = &bc_bear->beacon;
    if(is_lpn_support_and_en && is_in_mesh_friend_st_lpn()){
        return 0;    // LPN should not receive beacon msg when in friend state.
    }

    // -- if want to use cache to discard the same iv quickly, it should be in normal state. during update process, the first beacon may not be handled, such as segment busy.
	#if 1
    mesh_beacon_sec_nw_t backup_rc;
    memcpy(&backup_rc, p_bc->data, sizeof(backup_rc)); // backup to make sure it can not be over written, even though adv filter function may have checked to prevent being over write.
    mesh_beacon_sec_nw_t *p_sec_nw = &backup_rc;
    #else
    mesh_beacon_sec_nw_t *p_sec_nw = (mesh_beacon_sec_nw_t *)(p_bc->data);
    #endif
    err = mesh_sec_beacon_dec((u8 *)&p_sec_nw->flag);
    if(err){return 100;}
	
	#if MESH_MONITOR_EN
	if(monitor_security_en){
		mesh_dongle_adv_report2vc(p_payload, MESH_MONITOR_DATA); 
	}
	#endif
	
	#if (TESTCASE_FLAG_ENABLE && (HCI_ACCESS == HCI_USE_USB))
	//SET_TC_FIFO(TSCRIPT_MESH_RX, (u8 *)&bc_bear->len, bc_bear->len+1);
		#if 0 // print_tx_security_beacon_test_en
    LOG_MSG_LIB(TL_LOG_IV_UPDATE,&bc_bear->len, bc_bear->len+1,"RX beacon,nk arr idx:%d, new:%d, pkt:\r\n",mesh_key.netkey_sel_dec,mesh_key.new_netkey_dec);
    	#endif
    #endif
    return iv_update_key_refresh_rx_handle(&(p_sec_nw->flag), p_sec_nw->iv_idx);
}

void mesh_iv_update_set_start_flag(int keep_search_flag)
{
	iv_idx_st.update_proc_flag = IV_UPDATE_STEP1;
	iv_idx_st.keep_time_s = 0;
	if(!keep_search_flag){
	    iv_idx_st.searching_flag = 0;
	}

    if(!is_sno_exhausted()){
	    mesh_misc_store();	// save iv_update_trigger_flag by other nodes.
	}
	iv_idx_st.update_trigger_by_save = 0;   // only trigger once after power up.
	mesh_iv_update_report_between_gatt();
}

void mesh_iv_update_start_check()
{
#if IV_UPDATE_DISABLE
    return ;
#endif

    if((IV_UPDATE_STEP1 == iv_idx_st.update_proc_flag)
    || ((IV_UPDATE_STEP2 == iv_idx_st.update_proc_flag)&&(!is_enough_time_keep_state()))){
        iv_idx_st.rx_update = 0;   // ignored
    }else{
        if(!is_busy_segment_or_reliable_flow()){	// no need, wait to confirm
			iv_idx_st.iv_cur += 1;
            mesh_iv_update_set_start_flag(1);   // should be keep search flag when trigger by self after power up.
			mesh_ivi_event_cb(iv_idx_st.searching_flag);
			LOG_MSG_LIB(TL_LOG_IV_UPDATE,(u8 *)&iv_idx_st, sizeof(iv_idx_st),"IV index enter step1, cur tx sno:0x%06x, iv:",mesh_adv_tx_cmd_sno);
			// can't save for step 1, because if power restart will make iv index error.
        }else{
            // defer
        }
    }
}

void mesh_iv_update_start_poll()
{
	if(is_iv_index_invalid()){ 
		return;
	}
	
    static u8 iv_update_by_sno = 0;
    int flag_sno_exhausted = is_sno_exhausted() || iv_idx_st.update_trigger_by_save;
    if(flag_sno_exhausted && (!iv_idx_st.update_proc_flag)){
        iv_update_by_sno = 1;
    }
    
    if(flag_sno_exhausted || iv_idx_st.rx_update){
        mesh_iv_update_start_check();
    }
    
    if(IV_UPDATE_STEP1 == iv_idx_st.update_proc_flag){
        #if FEATURE_FRIEND_EN
        mesh_iv_update_start_poll_fn(iv_update_by_sno, beacon_iv_update_pkt);
        #endif
    }
    
	if(iv_idx_st.update_proc_flag){
        if(iv_update_by_sno){
            iv_update_by_sno = 0;
        }
    }
    beacon_iv_update_pkt = 0;    // reset
}

void mesh_iv_update_st_poll_s()
{
    iv_idx_st.keep_time_s++;
    
	if(iv_idx_st.update_proc_flag && is_enough_time_keep_state()){ // only for trigger role
	    mesh_iv_update_step_change();
	}
}


