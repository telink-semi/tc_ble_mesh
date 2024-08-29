/********************************************************************************************************
 * @file	mesh_lpn.c
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
#include "vendor/common/user_config.h"
#include "app_health.h"
#include "proj_lib/sig_mesh/app_mesh.h"
#include "lighting_model.h"
#include "vendor/common/sensors_model.h"
#include "proj_lib/mesh_crypto/sha256_telink.h"
#include "mesh_common.h"
#include "vendor/common/app_heartbeat.h"
#include "blt_soft_timer.h"
#include "vendor/common/mesh_ota.h"
#if(HCI_ACCESS == HCI_USE_UART)
#include "drivers.h"
#endif
#if UI_KEYBOARD_ENABLE
#if __TLSR_RISCV_EN__
#include "application/keyboard/keyboard.h"
#else
#include "proj/drivers/keyboard.h"
#endif
#endif

#if (FEATURE_LOWPOWER_EN || SPIRIT_PRIVATE_LPN_EN)
u8 lpn_provision_ok = 0;
#endif


#if FEATURE_LOWPOWER_EN
#if (IS_VC_PROJECT || TESTCASE_FLAG_ENABLE)
#define			DEBUG_SUSPEND				1
#else
#define			DEBUG_SUSPEND				0
#endif

#define RETRY_POLL_AGAIN_FLIP_FSN_EN		1	// change FSN to retry the 2nd round.

//u8 fri_request_retry_max = FRI_REQ_RETRY_MAX;
lpn_sub_list_event_t mesh_lpn_subsc_pending;
u8 mesh_lpn_poll_md_pending = 0;
#if PTS_TEST_EN
u8 mesh_lpn_rx_master_key = 1;
#else
u8 mesh_lpn_rx_master_key = 0;
#endif

u8 lpn_mode = LPN_MODE_NORMAL;
u32 lpn_mode_tick = 0;
u32 lpn_wakeup_tick = 0;

STATIC_ASSERT(LPN_ADV_INTERVAL_MS > (FRI_ESTABLISH_REC_DELAY_MS + FRI_ESTABLISH_WIN_MS + 10));//FRI_ESTABLISH_PERIOD_MS
STATIC_ASSERT(FRI_REQ_TIMEOUT_MS > FRI_ESTABLISH_PERIOD_MS);
STATIC_ASSERT(LPN_POLL_TIMEOUT_100MS * 100 >= FRI_POLL_INTERVAL_MS * 2); // timeout should not be too short.
STATIC_ASSERT(FRI_POLL_INTERVAL_MS <= 40*1000); // FRI_POLL_INTERVAL_MS max 40s, because intervalMin in bls_ll_setAdvInterval is u16, max is (0xffff*625)us = 40s.
STATIC_ASSERT(SUB_LIST_MAX_LPN <= SUB_LIST_MAX);    // SUB_LIST_MAX_LPN should be equal to sub_list_max later
STATIC_ASSERT(5 == SUB_LIST_MAX_IN_ONE_MSG);    // because subscription add/remove must be unsegment by spec.
STATIC_ASSERT(BLT_TIMER_SAFE_MARGIN_POST >= FRI_POLL_INTERVAL_MS*1000*sys_tick_per_us); // BLT_TIMER_SAFE_MARGIN_POST should set to larger than ADV_INTERVAL_MAX

#if GATT_LPN_EN
STATIC_ASSERT(MESH_DLE_MODE != MESH_DLE_MODE_EXTEND_BEAR); // extend bear need extend adv for friend ship
#endif

mesh_fri_ship_proc_lpn_t fri_ship_proc_lpn = {};	// for VC
mesh_lpn_subsc_list_t lpn_subsc_list;
mesh_subsc_list_retry_t subsc_list_retry = {};  // for retry procedure
mesh_lpn_par_t mesh_lpn_par = {0};

STATIC_ASSERT((FRI_REC_DELAY_MS > FRI_ESTABLISH_REC_DELAY_MS));

void friend_ship_establish_ok_cb_lpn()
{
	gatt_adv_send_flag = 0;
	rf_link_light_event_callback(LGT_CMD_FRIEND_SHIP_OK);
    friend_send_current_subsc_list();
    #if LPN_VENDOR_SENSOR_EN
        mesh_vd_lpn_pub_set();
    #endif    

	#if MD_DF_CFG_SERVER_EN
	foreach(i, NET_KEY_MAX){
		lpn_df_backup[i] = model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[i].directed_control.directed_forwarding;
		if(DIRECTED_FORWARDING_ENABLE == model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[i].directed_control.directed_forwarding){
			model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[i].directed_control.directed_forwarding = DIRECTED_FORWARDING_DISABLE;
		}
	}
	#endif
}

void friend_ship_disconnect_cb_lpn()
{
	#if GATT_LPN_EN
	gatt_adv_send_flag = GATT_LPN_EN;

	if(gatt_adv_send_flag){
		blt_soft_timer_add(&mesh_lpn_send_gatt_adv, ADV_INTERVAL_MS*1000);
	}
	#endif
	
	#if MD_DF_CFG_SERVER_EN
	foreach(i, NET_KEY_MAX){
		model_sig_g_df_sbr_cfg.df_cfg.directed_forward.subnet_state[i].directed_control.directed_forwarding = lpn_df_backup[i];
	}
	#endif
}

int is_friend_ship_link_ok_lpn()
{
	return mesh_lpn_par.link_ok;
}

#if 0
u8 mesh_lpn_tx_network_cb(mesh_match_type_t *p_match_type, u8 sec_type)
{
#if 0 //LPN_RESPONSE_WITH_SAME_KEY //  should not always return friend key, only when LPN want to send message with DF path. details refer to "3.6.6.2 Friendship security"
	if(p_match_type->mat.immutable_flag && is_friend_ship_link_ok_lpn()){
		sec_type = (mesh_key.sec_type_sel<=FRIENDSHIP) ? mesh_key.sec_type_sel:sec_type;
	}
#endif
	return sec_type;
}
#endif

int is_unicast_friend_msg_to_fn(mesh_cmd_nw_t *p_nw)
{
    return (is_in_mesh_friend_st_lpn()
        && (mesh_lpn_par.LPNAdr == p_nw->src)
        && (mesh_lpn_par.FriAdr == p_nw->dst));    // must because of group address
}

void friend_cmd_send_request()
{
	mesh_lpn_par.link_ok = 0;
    mesh_lpn_par.req.LPNCounter++;   // must before
	//LOG_MSG_LIB(TL_LOG_FRIEND,(u8 *)&mesh_lpn_par.req, sizeof(mesh_ctl_fri_req_t),"send friend request sno:0x%x par:",mesh_adv_tx_cmd_sno);
    mesh_tx_cmd_layer_upper_ctl(CMD_CTL_REQUEST, (u8 *)&mesh_lpn_par.req, sizeof(mesh_ctl_fri_req_t), ele_adr_primary, ADR_ALL_FRIEND,0);
}

void friend_cmd_send_poll()
{
	//LOG_MSG_LIB(TL_LOG_FRIEND,(u8 *)(&mesh_lpn_par.poll), sizeof(mesh_ctl_fri_poll_t),"send friend poll sno:0x%x par:",mesh_adv_tx_cmd_sno);
    fri_ship_proc_lpn.poll_tick = clock_time()|1;
    mesh_tx_cmd_layer_upper_ctl(CMD_CTL_POLL, (u8 *)(&mesh_lpn_par.poll), sizeof(mesh_ctl_fri_poll_t), ele_adr_primary, mesh_lpn_par.FriAdr,0);	
}

void friend_cmd_send_subsc_add(u8 *par_subsc, u32 len)  // only LPN support
{
	LOG_MSG_LIB(TL_LOG_FRIEND,(u8 *)par_subsc, len,"send friend sub list add, sno:0x%x par:",mesh_adv_tx_cmd_sno);
    subsc_list_retry.tick = clock_time() | 1;
    mesh_tx_cmd_layer_upper_ctl(CMD_CTL_SUBS_LIST_ADD, (u8 *)par_subsc, len, ele_adr_primary, mesh_lpn_par.FriAdr,0);
}

void friend_cmd_send_subsc_rmv(u8 *par_subsc, u32 len)  // only LPN support
{
	LOG_MSG_LIB(TL_LOG_FRIEND,(u8 *)par_subsc, len,"send friend sub list remove sno:0x%x par:",mesh_adv_tx_cmd_sno);
    subsc_list_retry.tick = clock_time() | 1;
    mesh_tx_cmd_layer_upper_ctl(CMD_CTL_SUBS_LIST_REMOVE, (u8 *)par_subsc, len, ele_adr_primary, mesh_lpn_par.FriAdr,0);
}

void friend_subsc_repeat()
{
    if(fri_ship_proc_lpn.poll_retry){
        fri_ship_proc_lpn.poll_tick = clock_time()|1;
    }
    
    if(SUBSC_ADD == subsc_list_retry.retry_type){
        friend_cmd_send_subsc_add(&subsc_list_retry.TransNo, 1+subsc_list_retry.subsc_cnt*2);
    }else if(SUBSC_REMOVE == subsc_list_retry.retry_type){
        friend_cmd_send_subsc_rmv(&subsc_list_retry.TransNo, 1+subsc_list_retry.subsc_cnt*2);
    }
}

inline void friend_subsc_stop()
{
    subsc_list_retry.retry_cnt = 0;
	subsc_list_retry.tick = 0;
	
	if(0 == mesh_lpn_subsc_pending.cnt){
		mesh_lpn_subsc_pending.op = 0;		
	}
}

int friend_subsc_list_cmd_start(u8 type, u16 *adr_list, u32 subsc_cnt)
{
	/* subscription add/remove must be unsegment, please refer to spec section of "Friendship" -> "Low Power feature":
	   All transport control messages originated by a Low Power node shall be sent as Unsegmented Control
	   messages with the SRC field set to the unicast address of the primary element of the node that supports
	   the Low Power feature.
	*/
    if(subsc_cnt > SUB_LIST_MAX_IN_ONE_MSG){
		u16 op = (type == SUBSC_ADD)?CMD_CTL_SUBS_LIST_ADD:CMD_CTL_SUBS_LIST_REMOVE;
		mesh_lpn_subsc_pending_add(op, adr_list+SUB_LIST_MAX_IN_ONE_MSG, subsc_cnt - SUB_LIST_MAX_IN_ONE_MSG, 1);
		subsc_cnt = SUB_LIST_MAX_IN_ONE_MSG;
    }
	else{
		mesh_lpn_subsc_pending.cnt = 0;	
	}
	
    if(subsc_cnt > LPN_SUBSC_LIST_ADD_CNT_MAX){ // default is 10.
    	// subsc_cnt will be trimmed to be 10 in mesh_lpn_subsc_pending_add_().
    }
    
    subsc_list_retry.subsc_cnt = subsc_cnt;
    subsc_list_retry.TransNo = lpn_subsc_list.TransNo;
    memset(subsc_list_retry.adr, 0, sizeof(subsc_list_retry.adr));
    memcpy(subsc_list_retry.adr, adr_list, subsc_cnt * 2);
    subsc_list_retry.retry_cnt = lpn_get_poll_retry_max();
    subsc_list_retry.retry_type = type;
    
    lpn_subsc_list.TransNo++;

    friend_subsc_repeat();  // send first message

    return 0;
}

void friend_subsc_add(u16 *adr_list, u32 subsc_cnt)
{
    friend_subsc_list_cmd_start(SUBSC_ADD, adr_list, subsc_cnt);
}

void friend_subsc_rmv(u16 *adr_list, u32 subsc_cnt)
{
    friend_subsc_list_cmd_start(SUBSC_REMOVE, adr_list, subsc_cnt);
    friend_subsc_list_rmv_adr((lpn_adr_list_t *)(&lpn_subsc_list.adr), (lpn_adr_list_t *)adr_list, subsc_cnt);
}

void mesh_friend_ship_set_st_lpn(u8 st)
{
	if(FRI_ST_REQUEST == st){
		mesh_lpn_subsc_pending.op = 0;
		friend_subsc_stop();		
		mesh_friend_ship_proc_init_lpn(); // clear retry cnt
	    mesh_friend_ship_clear_LPN();
	}
	
    fri_ship_proc_lpn.status = st;
	mesh_lpn_adv_interval_update(1);
}

int is_in_mesh_friend_st_lpn()
{
	if(is_lpn_support_and_en){
    	return (is_friend_ship_link_ok_lpn() && (mesh_lpn_par.FriAdr != 0));
	}else{
		return 0;
	}
}

void mesh_friend_ship_proc_init_lpn()
{
    memset(&fri_ship_proc_lpn, 0, sizeof(mesh_fri_ship_proc_lpn_t));
}

void mesh_friend_ship_clear_LPN()
{
    mesh_ctl_fri_req_t req_backup;
    memcpy(&req_backup, &mesh_lpn_par.req, sizeof(req_backup));
    memset(&mesh_lpn_par, 0, sizeof(mesh_lpn_par));
    memcpy(&mesh_lpn_par.req, &req_backup, sizeof(req_backup));
}

int mesh_lpn_subsc_pending_add(u16 op, u16 *p_sublist, int sub_cnt, int overwrite_flag)
{
	if(mesh_lpn_subsc_pending.op && (op != mesh_lpn_subsc_pending.op)){
		return -1;
	}

	mesh_lpn_subsc_pending.op = op;
	if(overwrite_flag){
		if(sub_cnt > ARRAY_SIZE(mesh_lpn_subsc_pending.sub_list)){
			sub_cnt = ARRAY_SIZE(mesh_lpn_subsc_pending.sub_list);
		}
	
		memcpy(mesh_lpn_subsc_pending.sub_list, p_sublist, sub_cnt << 1);
		mesh_lpn_subsc_pending.cnt = sub_cnt;
	}
	else{		
		foreach(add_idx, sub_cnt){
			if(mesh_lpn_subsc_pending.cnt >= ARRAY_SIZE(mesh_lpn_subsc_pending.sub_list)){
				return 0;
			}
			
			int is_exist = 0;
			foreach(list_idx, mesh_lpn_subsc_pending.cnt){
				if(p_sublist[add_idx] == mesh_lpn_subsc_pending.sub_list[list_idx]){
					is_exist = 1;
					break;
				}
			}

			if(0 == is_exist){
				mesh_lpn_subsc_pending.sub_list[mesh_lpn_subsc_pending.cnt++] = p_sublist[add_idx];
			}
		}
	}
	
	return 0;
}

void mesh_lpn_gatt_adv_refresh()
{
#if (GATT_LPN_EN)
	if(lpn_provision_ok && gatt_adv_send_flag){
		blt_soft_timer_add(&mesh_lpn_send_gatt_adv, ADV_INTERVAL_MS*1000);
	}
#else
	gatt_adv_send_flag = !lpn_provision_ok;
#endif
}

//------------softer timer cb function----------------------//
int mesh_lpn_send_gatt_adv()
{
	if(gatt_adv_send_flag && (BLS_LINK_STATE_ADV == blc_ll_getCurrentState())){
		lpn_quick_tx(1);
		return (ADV_INTERVAL_MS + (rand()%ADV_INTERVAL_RANDOM_MS))*1000;
	}
	return -1;
}

int mesh_lpn_send_mesh_cmd()
{
	int ret = 0;
	if(!is_mesh_adv_cmd_fifo_empty()){
		lpn_quick_tx(0);
		ret = 1;
	}
	return ret;
}

int mesh_lpn_poll_receive_timeout(void)
{
	ENABLE_SUSPEND_MASK;
	blc_ll_setScanEnable (0, 0);
	rf_set_tx_rx_off();// disable tx rx in manual mode immediately, must 	
	CLEAR_ALL_RFIRQ_STATUS;
	
	return -1;
}

int mesh_lpn_rcv_delay_wakeup(void)
{
	app_enable_scan_all_device ();	
	mesh_send_adv2scan_mode(0);
	bls_pm_setSuspendMask (SUSPEND_DISABLE); // not enter sleep to receive packets	
	if(is_friend_ship_link_ok_lpn()){
		
		blt_soft_timer_add(&mesh_lpn_poll_receive_timeout, mesh_lpn_par.offer.RecWin*1000);
	}
	return -1;
}

int mesh_lpn_poll_md_wakeup(void)
{
	int ret = -1;	
	if(!is_busy_tx_seg(0)){
		#if (!BLE_MULTIPLE_CONNECTION_ENABLE)
		set_sdk_mainLoop_run_flag(1); // single connection can sent adv immediately, not run mesh_loop_process in early wakeup to save time
		#endif
	}
	else{
		ret = CMD_INTERVAL_MS * 1000; // for tx segment retry
	}
	
	if(mesh_lpn_send_mesh_cmd()){// fifo not empty, send first, maybe status message
		if(my_fifo_get(&mesh_adv_cmd_fifo)){
			ret = get_mesh_adv_interval()+(rand()%10)*1000;// add 0~10ms random
		}
		u8 *p_buf = my_fifo_get(&mesh_adv_cmd_fifo);
		if(is_in_mesh_friend_st_lpn() && is_busy_tx_seg(0) && is_tx_seg_one_round_ok()){
			if(0 == p_buf){
				ret = FRI_LPN_WAIT_SEG_ACK_MS * 1000; 
				fri_ship_proc_lpn.poll_retry = 0;
				mesh_lpn_poll_md_pending = 1; // poll for ack after one round
			}
		}	
	}
	else if(mesh_lpn_poll_md_pending){
		mesh_friend_ship_start_poll();
	}
	else if(mesh_lpn_subsc_pending.op){
		if(0 == subsc_list_retry.tick){
			u16 op = mesh_lpn_subsc_pending.op;			
			u16 sub_list[mesh_lpn_subsc_pending.cnt];
			memcpy(sub_list, mesh_lpn_subsc_pending.sub_list, mesh_lpn_subsc_pending.cnt<<1);
			if(CMD_CTL_SUBS_LIST_ADD == op){
				friend_subsc_add(sub_list, mesh_lpn_subsc_pending.cnt);
			}
			else if(CMD_CTL_SUBS_LIST_REMOVE == op){
				friend_subsc_rmv(sub_list, mesh_lpn_subsc_pending.cnt);
			}
		}
	}
	else{
	}

	if((-1 == ret) && (mesh_lpn_subsc_pending.op || mesh_lpn_poll_md_pending)){
		ret = (mesh_lpn_par.req.RecDelay + FRI_POLL_DELAY_FOR_MD_MS) * 1000;
	}
	return ret;
}

#if (BLT_SOFTWARE_TIMER_ENABLE)
void soft_timer_mesh_adv_proc()
{
	if(my_fifo_data_cnt_get(&mesh_adv_cmd_fifo)){
		if(!is_soft_timer_exist(&mesh_lpn_poll_md_wakeup)){
			mesh_lpn_sleep_prepare(CMD_ST_NORMAL_TX);
		}
	}
}
#endif

//------------end softer timer cb function----------------------//
void mesh_friend_ship_proc_LPN(u8 *bear)
{
    if(blc_ll_getCurrentState() == BLS_LINK_STATE_CONN){	// at GATT connected state, not friendship state.
        return ;
    }
    
    static u32 t_rec_delay_and_win = 0;
    u32 poll_retry_interval_ms = t_rec_delay_and_win;
	u32 timeout_ms = (poll_retry_interval_ms*1000) * (2*2+1)/2;    // confirm later
    #if (0 == DEBUG_PROXY_FRIEND_SHIP && PTS_TEST_EN)
    if(fri_ship_proc_lpn.status){
        // retry poll should be more quicklier during establish friend ship.
        #define RETRY_POLL_INTV_MS_MAX      170 // FRND-LPN-BI01 need retry 3 times during 1 second after get offer.
        if(poll_retry_interval_ms > RETRY_POLL_INTV_MS_MAX){
            poll_retry_interval_ms = RETRY_POLL_INTV_MS_MAX;
        }
    }
    #endif
    
	if(!bear && is_mesh_adv_cmd_fifo_empty()){
	    if(fri_ship_proc_lpn.poll_retry && clock_time_exceed(fri_ship_proc_lpn.poll_tick, poll_retry_interval_ms*1000)){
	        fri_ship_proc_lpn.poll_retry--;
	        if(0 == fri_ship_proc_lpn.poll_retry){
	            if(FRI_ST_UPDATE == fri_ship_proc_lpn.status){
					mesh_friend_ship_retry();
	            }else{
	                friend_subsc_stop();
	                mesh_cmd_sig_lowpower_heartbeat();
	                friend_ship_disconnect_cb_lpn();
	                mesh_friend_ship_set_st_lpn(FRI_ST_REQUEST);    // restart establish procedure
	            }
	        }else{
	        	#if RETRY_POLL_AGAIN_FLIP_FSN_EN
	        	if((FRI_POLL_RETRY_MAX == fri_ship_proc_lpn.poll_retry) && is_in_mesh_friend_st_lpn()){
	        		mesh_lpn_par.poll.FSN = !mesh_lpn_par.poll.FSN;	// change to poll the 2nd round.
					LOG_MSG_LIB(TL_LOG_FRIEND, 0, 0,"LPN flip FSN, then try again");
	        	}
	        	#endif
	            friend_cmd_send_poll();  // retry
	        }
	    }
	    else if(subsc_list_retry.retry_cnt && clock_time_exceed(subsc_list_retry.tick, timeout_ms)){
	        subsc_list_retry.tick = clock_time() | 1;   // also refresh when send_subsc
	        subsc_list_retry.retry_cnt--;
	        if(0 == subsc_list_retry.retry_cnt){
	            mesh_friend_ship_set_st_lpn(FRI_ST_REQUEST);
	        }else{
	            LOG_MSG_LIB(TL_LOG_FRIEND, 0, 0,"friend_subsc_repeat_***********************");
	            friend_subsc_repeat();
	        }
	    }
	}

    mesh_cmd_bear_t *p_bear = (mesh_cmd_bear_t *)bear;
    //mesh_cmd_nw_t *p_nw = &p_bear->nw;
    mesh_cmd_lt_ctl_unseg_t *p_lt_ctl_unseg = &p_bear->lt_ctl_unseg;
    u8 op = -1;
    if(bear){
	    op = p_lt_ctl_unseg->opcode;
    }
    
    if(0 == fri_ship_proc_lpn.status){ // means at friendship establish ok state or before state of send request message.
        if(bear){
            if(CMD_CTL_SUBS_LIST_CONF == op){
                mesh_ctl_fri_subsc_list_t *p_subsc = CONTAINER_OF(p_lt_ctl_unseg->data,mesh_ctl_fri_subsc_list_t,TransNo);
                if(p_subsc->TransNo == (subsc_list_retry.TransNo)){   //TransNo have increased
                	LOG_MSG_LIB(TL_LOG_FRIEND,(u8 *)&p_subsc->TransNo, sizeof(p_subsc->TransNo),"rcv sub list confirm:");
                    if(SUBSC_ADD == subsc_list_retry.retry_type){
                        friend_subsc_list_add_adr((lpn_adr_list_t *)(&lpn_subsc_list.adr), (lpn_adr_list_t *)(&subsc_list_retry.adr), subsc_list_retry.subsc_cnt);
                    }
                    friend_subsc_stop();
                }
            }else if(CMD_CTL_UPDATE == op){
                mesh_ctl_fri_update_t *p_update = (mesh_ctl_fri_update_t *)(p_lt_ctl_unseg->data);
				LOG_MSG_LIB(TL_LOG_FRIEND,(u8 *)p_update, sizeof(mesh_ctl_fri_update_t),"NW_IVI %d, sno: 0x%02x%02x%02x, rcv friend update:", p_bear->nw.ivi, p_bear->nw.sno[2], p_bear->nw.sno[1], p_bear->nw.sno[0]);
                iv_update_key_refresh_rx_handle(&p_update->flag, p_update->IVIndex);

            }
        }else{
        	if(is_friend_ship_link_ok_lpn() && is_mesh_adv_cmd_fifo_empty()  && clock_time_exceed(fri_ship_proc_lpn.poll_tick, get_lpn_poll_interval_ms() * 1000)){ // send poll after mesh message and subscription list add done.
				if(!mesh_lpn_subsc_pending.op && !subsc_list_retry.tick){
					mesh_friend_ship_start_poll();
				}
				else if(mesh_lpn_subsc_pending.op && !subsc_list_retry.tick){
					mesh_lpn_sleep_prepare(CMD_ST_POLL_MD);
				}
			}
        }
    }else{
        switch(fri_ship_proc_lpn.status){   // Be true only during establishing friend ship.
            case FRI_ST_REQUEST:
				if(is_mesh_adv_cmd_fifo_empty() && clock_time_exceed(fri_ship_proc_lpn.req_tick, FRI_REQ_TIMEOUT_MS * 1000)){
	                fri_ship_proc_lpn.req_tick = clock_time();
					
	                friend_cmd_send_request();

	                // init par
	                mesh_friend_ship_clear_LPN();
	                mesh_lpn_par.LPNAdr = ele_adr_primary;
	                mesh_friend_ship_set_st_lpn(FRI_ST_OFFER);
				}
                break;
            case FRI_ST_OFFER:
                if(bear){
                    if(CMD_CTL_OFFER == p_lt_ctl_unseg->opcode){
                        if(0 != lpn_rx_offer_handle(bear)){
                            break;
                        }
                    }
                }else{
                    if(clock_time_exceed(fri_ship_proc_lpn.req_tick, FRI_ESTABLISH_PERIOD_MS*1000)){
                        if(mesh_lpn_par.FriAdr){
                            mesh_lpn_par.link_ok = 1;
                            mesh_friend_key_update_all_nk(0, 0);
							#if PTS_TEST_EN
							mesh_cmd_sig_lowpower_heartbeat(); // MESH/NODE/CFG/HBP/BV-05-C: PTS think establish friend ship is ok when receive "offer" no need "update".
							#endif
                        }
                        mesh_friend_ship_set_st_lpn(FRI_ST_POLL);
                    }
                }
                break;
            case FRI_ST_POLL:
                if(is_friend_ship_link_ok_lpn()){
					if(is_mesh_adv_cmd_fifo_empty()){
	                    mesh_lpn_par.poll.FSN = 0;   // init
	                    // send poll
	                    mesh_friend_ship_start_poll();
	                    t_rec_delay_and_win = mesh_lpn_par.req.RecDelay + mesh_lpn_par.offer.RecWin;
	                    mesh_friend_ship_set_st_lpn(FRI_ST_UPDATE);
					}
                }else{
                    lpn_no_offer_handle();
                }
                break;
            case FRI_ST_UPDATE:
                if(bear){   // current state is establishing friend ship
                    if(CMD_CTL_UPDATE == p_lt_ctl_unseg->opcode){
                        mesh_ctl_fri_update_t *p_update = (mesh_ctl_fri_update_t *)(p_lt_ctl_unseg->data);
						LOG_MSG_LIB(TL_LOG_FRIEND,(u8 *)p_update, sizeof(mesh_ctl_fri_update_t),"rcv friend update:");
                        memcpy(&mesh_lpn_par.update, p_update, sizeof(mesh_ctl_fri_update_t));
                        //friend ship establish done
                        mesh_lpn_par.req.PreAdr = mesh_lpn_par.FriAdr;
                        #if !PTS_TEST_EN
						mesh_cmd_sig_lowpower_heartbeat();
						#endif
                        iv_update_key_refresh_rx_handle(&p_update->flag, p_update->IVIndex);
                        mesh_friend_ship_proc_init_lpn();

                        friend_ship_establish_ok_cb_lpn();
                    }
                }else{
                	#if 0 // timeout checking in count down of poll retry. if not, sometimes will trigger retry request when poll retry is not 0.
                    if(clock_time_exceed(fri_ship_proc_lpn.poll_tick, t_rec_delay_and_win*1000)){
                    	mesh_friend_ship_retry();
                    }
                    #endif
                }
                break;
            default:
                break;
        }
    }
}







u8 lpn_get_poll_retry_max()
{
    return FRI_POLL_RETRY_MAX;
}

#if LPN_DEBUG_PIN_EN
void lpn_debug_set_current_pin(u8 level)
{
    #define CURRENT_PIN_MESH      GPIO_PC4
    gpio_set_func(CURRENT_PIN_MESH, AS_GPIO);
    gpio_set_output_en(CURRENT_PIN_MESH, 1);
    gpio_write(CURRENT_PIN_MESH, level);
}

void lpn_debug_set_debug_pin(u8 level)
{
    //#define DEBUG_PIN_MESH      GPIO_PB7 // conflic with GPIO_VBAT_DETECT	// TBD
    gpio_set_func(DEBUG_PIN_MESH, AS_GPIO);
    gpio_set_output_en(DEBUG_PIN_MESH, 1);
    gpio_write(DEBUG_PIN_MESH, level);
}

void lpn_debug_set_irq_pin(u8 level)
{
    #define IRQ_PIN_MESH      GPIO_PD2
    gpio_set_func(IRQ_PIN_MESH, AS_GPIO);
    gpio_set_output_en(IRQ_PIN_MESH, 1);
    gpio_write(IRQ_PIN_MESH, level);
}

void lpn_debug_set_event_handle_pin(u8 level)
{
    #define EVENT_PIN_MESH      GPIO_PB5
    gpio_set_func(EVENT_PIN_MESH, AS_GPIO);
    gpio_set_output_en(EVENT_PIN_MESH, 1);
    gpio_write(EVENT_PIN_MESH, level);
}

void lpn_debug_alter_debug_pin(int reset)
{
    static u8 debug_pin_level = 0;
    if(reset){
        debug_pin_level = 0;
    }else{
        debug_pin_level = !debug_pin_level;
    }
    
    lpn_debug_set_debug_pin(debug_pin_level);
}
#endif

#if PTS_TEST_EN
enum{
    TS_LPN_BV04 = 4,
    TS_LPN_BV05 = 5,
    TS_LPN_BV08 = 8,
    TS_LPN_BV05_2 = 0x85,   // auto set
};

static volatile u8 lpn_pts_test_cmd;

void pts_test_case_lpn()
{
	u32 tick = clock_time();
	LOG_MSG_LIB(TL_LOG_FRIEND,0, 0,"waiting for input pts case ID through BDT to send command ......");
	while((0 == lpn_pts_test_cmd) && !clock_time_exceed(tick, (LPN_POLL_TIMEOUT_100MS - 10)*100*1000)) // less than LPN_POLL_TIMEOUT_100MS
	{
		u8 cmd_flash = *(u8 *)0x78000;
		if(cmd_flash != 0xff){
			lpn_pts_test_cmd = cmd_flash;
			break;
		}
		wd_clear();
	}
	LOG_MSG_LIB(TL_LOG_FRIEND,0, 0,"case ID:%d",lpn_pts_test_cmd);

	if(lpn_pts_test_cmd){
		u8 cmd_backup = lpn_pts_test_cmd;
		if((TS_LPN_BV04 == lpn_pts_test_cmd)){
			subsc_add_rmv_test(1);
		}else if(TS_LPN_BV05 == lpn_pts_test_cmd){
			subsc_add_rmv_test(1);
			lpn_pts_test_cmd = TS_LPN_BV05_2;
		}else if(TS_LPN_BV05_2 == lpn_pts_test_cmd){
			subsc_add_rmv_test(0);
		}else if(TS_LPN_BV08 == lpn_pts_test_cmd){
			friend_cmd_send_fn(0, CMD_CTL_CLEAR);
		}

		if(cmd_backup != TS_LPN_BV05){
			lpn_pts_test_cmd = 0;
		}
	}
}
#endif

void mesh_feature_set_lpn(){
    #if FRI_SAMPLE_EN
	friend_ship_sample_message_test();
    #else
    mesh_ctl_fri_req_t *p_req = &mesh_lpn_par.req;
    p_req->Criteria.MinCacheSizeLog = LPN_MIN_CACHE_SIZE_LOG; // 3;
    p_req->Criteria.RecWinFac = FRI_REC_WIN_FAC;
    p_req->Criteria.RSSIFac = FRI_REC_RSSI_FAC;
    p_req->RecDelay = FRI_REC_DELAY_MS;
    p_req->PollTimeout = LPN_POLL_TIMEOUT_100MS;  // 0x057e40 = 10*3600 second
    p_req->PreAdr = 0;
    p_req->NumEle = g_ele_cnt;
    p_req->LPNCounter = 0 - 1;  // increase counter later
    #endif
}

void lpn_set_poll_ready()
{
  	fri_ship_proc_lpn.poll_tick = fri_ship_proc_lpn.poll_tick - BIT(31);  
}

void mesh_friend_ship_start_poll()
{
    if(is_in_mesh_friend_st_lpn()){
        friend_cmd_send_poll();
		mesh_lpn_poll_md_pending = 0;
		#if RETRY_POLL_AGAIN_FLIP_FSN_EN
		fri_ship_proc_lpn.poll_retry = PTS_TEST_EN ? 20 : (FRI_POLL_RETRY_MAX * 2);	// *2 means 2 rounds. change FSN to retry the 2nd round.
		#else
		fri_ship_proc_lpn.poll_retry = PTS_TEST_EN ? 20 : (FRI_POLL_RETRY_MAX + 1);
		#endif
    }
}

void mesh_friend_ship_stop_poll()
{
	blt_soft_timer_delete(&mesh_lpn_rcv_delay_wakeup);
	fri_ship_proc_lpn.poll_retry = 0;
}

void subsc_add_rmv_test(int add)
{
    if(!fri_ship_proc_lpn.status){
        if(is_friend_ship_link_ok_lpn()){
            u16 adr[] = {0xc001, 0xc002, 0xc003, 0xc004, 0xc005};
            if(add){
                friend_subsc_add(adr, ARRAY_SIZE(adr));
            }else{
                friend_subsc_rmv(adr, ARRAY_SIZE(adr));
            }
        }
    }
}

void lpn_subsc_list_update_by_sub_set_cmd(u16 op, u16 sub_adr)
{
    if((CFG_MODEL_SUB_ADD == op)||(CFG_MODEL_SUB_VIRTUAL_ADR_ADD == op)){
		mesh_lpn_subsc_pending_add(CMD_CTL_SUBS_LIST_ADD, &sub_adr, 1, 0);
    }else if((CFG_MODEL_SUB_DEL == op)||(CFG_MODEL_SUB_VIRTUAL_ADR_DEL == op)){
		mesh_lpn_subsc_pending_add(CMD_CTL_SUBS_LIST_REMOVE, &sub_adr, 1, 0);
    }else if(((CFG_MODEL_SUB_OVER_WRITE == op)||(CFG_MODEL_SUB_VIRTUAL_ADR_OVER_WRITE == op))
    	   ||(CFG_MODEL_SUB_DEL_ALL == op)){
    }
}

void friend_send_current_subsc_list()
{
#if MD_SERVER_EN 
    int j = 0;
    model_common_t * model_list[] = {
    	&model_sig_g_onoff_level.onoff_srv[0].com,
    	#if MD_MESH_OTA_EN
		&model_mesh_ota.blob_trans_srv.com,
		#endif
    };
		
	u16 adr_list[LPN_SUBSC_LIST_ADD_CNT_MAX]; // subsc_cnt will be trimmed to LPN_SUBSC_LIST_ADD_CNT_MAX in mesh_lpn_subsc_pending_add_().
    foreach_arr(k, model_list){
	    model_common_t *p_model_com = model_list[k]; 
	    #if MD_MESH_OTA_EN
	    if(p_model_com == &model_mesh_ota.blob_trans_srv.com){
		    if(is_fw_update_start_before()){
		    	// because it may re-establish friend ship during OTA.
		    }else{
		    	// not expected to report,because we don't want to report when establishing friendship after OTA.
		    	continue; // no need to report in this case.
		    }
	    }
	    #endif
	    
	    #if (VIRTUAL_ADDR_STAND_ALONE_SIZE_EN && (0 == VIRTUAL_ADDR_ENABLE))
	    foreach_arr(i, p_model_com->sub_buf.sub_addr)
	    #else
	    foreach_arr(i, p_model_com->sub_list)
	    #endif
	    {
	    	#if (VIRTUAL_ADDR_STAND_ALONE_SIZE_EN)
	        u16 sub_adr = p_model_com->sub_buf.sub_addr[i];
	    	#else
	        u16 sub_adr = p_model_com->sub_list[i];
	        #endif
	        if((sub_adr & 0x8000) && (!is_fixed_group(sub_adr))){
				int existed_flag = 0;
				foreach(z, j){
					if(adr_list[z] == sub_adr){
						existed_flag = 1;
						break;
					}	
				}
				
				if(existed_flag == 0){					
	            	adr_list[j++] = sub_adr;
				}
	        }
			
	        if(j >= ARRAY_SIZE(adr_list)){
	            break;
	        }
	    }
    }
    
    if(j){
        friend_subsc_add(adr_list, j);
    }
#endif
}

void lpn_node_io_init()
{
#if ((!IS_VC_PROJECT) && FEATURE_LOWPOWER_EN)
    lpn_debug_set_current_pin(1);
	#if (UI_KEYBOARD_ENABLE)
	ui_keyboard_wakeup_io_init();
	#endif
#endif

#if LPN_VENDOR_SENSOR_EN
    i2c_io_init();
#endif
}

void mesh_lpn_sleep_prepare(u16 op)
{
	if(is_lpn_support_and_en && (BLS_LINK_STATE_CONN != blc_ll_getCurrentState())){
		if(CMD_ST_NORMAL_TX != op){  
			mesh_lpn_poll_receive_timeout();
			blt_soft_timer_delete(&mesh_lpn_poll_receive_timeout);
		}
		
		
		if(CMD_CTL_REQUEST == op){
			blt_soft_timer_add(&mesh_lpn_rcv_delay_wakeup, FRI_ESTABLISH_REC_DELAY_MS*1000);
		}
		else if((CMD_CTL_POLL == op) || (CMD_CTL_SUBS_LIST_REMOVE == op) || (CMD_CTL_SUBS_LIST_ADD == op)){
			blt_soft_timer_add(&mesh_lpn_rcv_delay_wakeup, mesh_lpn_par.req.RecDelay*1000);
		}
		else if(CMD_ST_NORMAL_TX == op){
			int interval = get_mesh_adv_interval();			
			blt_soft_timer_add(&mesh_lpn_poll_md_wakeup, interval ? interval : 10*1000);
		}
		else if(CMD_ST_POLL_MD == op){ // message from friend is not friend update, poll more data. 
			mesh_lpn_poll_md_pending = 1;
			u32 sleep_ms = FRI_POLL_DELAY_FOR_MD_MS;
			#if MD_MESH_OTA_EN
			if(is_blob_chunk_transfer_ready()){
				sleep_ms = 10;	// waiting for chunk message from FN retransmit completed
			}
			#endif
			blt_soft_timer_add(&mesh_lpn_poll_md_wakeup, sleep_ms * 1000);
			#if (!WIN32 && !BLE_MULTIPLE_CONNECTION_ENABLE)
			blt_rxfifo.rptr = blt_rxfifo.wptr - 1;// clear buf, blt_rxfifo.rptr will ++ in lib
		    #endif
		}
		else if(CMD_CTL_UPDATE == op){
			#if (!WIN32 && !BLE_MULTIPLE_CONNECTION_ENABLE)
			blt_rxfifo.rptr = blt_rxfifo.wptr - 1;// clear buf, blt_rxfifo.rptr will ++ in lib
		    #endif
			blt_soft_timer_delete(&mesh_lpn_poll_receive_timeout);
		}
		else{//CMD_ST_SLEEP

		}
	}
	else if(CMD_ST_NORMAL_TX == op){
		int interval = get_mesh_adv_interval();
		blt_soft_timer_add(&mesh_lpn_poll_md_wakeup, interval ? interval : 10*1000);
	}
}

void suspend_enter(u32 sleep_ms, int deep_retention_flag)
{
#if DEBUG_SUSPEND
    sleep_us(sleep_ms*1000);
#else
    int sleep_mode = 0; // SUSPEND_MODE default
     #if(__TL_LIB_8258__ || (MCU_CORE_TYPE == MCU_CORE_8258) || (MCU_CORE_TYPE == MCU_CORE_8278)||(MCU_CORE_TYPE == MCU_CORE_9518))
    sleep_mode = deep_retention_flag ? RETENTION_RAM_SIZE_USE : SUSPEND_MODE;
    #endif
	
    cpu_sleep_wakeup(sleep_mode, PM_WAKEUP_TIMER, clock_time() + sleep_ms*CLOCK_SYS_CLOCK_1MS);
	lpn_wakeup_tick = clock_time();
#endif
}

#if (UI_KEYBOARD_ENABLE)
static int	long_pressed;
u8   key_released =1;

#define	KEY_SCAN_WAKEUP_INTERVAL		(40 * 1000)	// unit:us. wakeup interval to scan key.
#define GPIO_WAKEUP_KEYPROC_CNT			3
/**
 * @brief       This function servers to quickly scan keyboard after wakeup from deep sleep and hold this data to the cache.
 * @param[in]   none 
 * @return      none
 * @note        
 */
void deep_wakeup_proc(void)
{
#if(DEEPBACK_FAST_KEYSCAN_ENABLE)
	if(kb_scan_key (KB_NUMLOCK_STATUS_POWERON, 1)){
		deepback_key_state = DEEPBACK_KEY_CACHE;
		memcpy(&kb_event_cache,&kb_event,sizeof(kb_event));
	}
#endif

	return;
}

/**
 * @brief       This function servers to set gpio wakeup source before sleep when key release.
 * @param[in]   e	- event code BLT_EV_FLAG_SUSPEND_ENTER
 * @param[in]   p	- NULL
 * @param[in]   n	- 0
 * @return      none
 * @note        
 */
void  lpn_set_sleep_wakeup (u8 e, u8 *p, int n)
{
#if (UI_KEYBOARD_ENABLE)
	if(key_released){
		bls_pm_setWakeupSource(PM_WAKEUP_PAD);
	}
#endif
}

/**
 * @brief       This function servers to process keyboard event from deep_wakeup_proc().
 * @param[io]   *det_key - detect key flag. 0: no key detect, use deepback cache if exist. other: key had been detected.
 * @return      none
 * @note        
 */
void deepback_pre_proc(int *det_key)
{
#if (DEEPBACK_FAST_KEYSCAN_ENABLE)
	if(!(*det_key) && deepback_key_state == DEEPBACK_KEY_CACHE){
		memcpy(&kb_event,&kb_event_cache,sizeof(kb_event));
		*det_key = 1;
		deepback_key_state = DEEPBACK_KEY_IDLE;
	}
#endif

	return;
}

int soft_timer_key_scan()
{	
	return key_released ? -1 : 0;
}
#endif

void lpn_proc_keyboard (u8 e, u8 *p, int n)
{
	static u32 tick_scan;
	static int gpio_wakeup_proc_cnt;

#if BLE_REMOTE_PM_ENABLE
	if(e == BLT_EV_FLAG_GPIO_EARLY_WAKEUP){
		#if BLT_SOFTWARE_TIMER_ENABLE
		blt_soft_timer_add(&soft_timer_key_scan, KEY_SCAN_WAKEUP_INTERVAL);
		#endif
		
		//when key press GPIO wake_up sleep, process key_scan at least GPIO_WAKEUP_KEYPROC_CNT times
		gpio_wakeup_proc_cnt = GPIO_WAKEUP_KEYPROC_CNT;
	}
	else if(gpio_wakeup_proc_cnt){	
		gpio_wakeup_proc_cnt--;
	}
#endif

	if(gpio_wakeup_proc_cnt || clock_time_exceed(tick_scan, 10 * 1000)){ //key scan interval: 10ms
		tick_scan = clock_time();
	}
	else{
		return;
	}
	
	static u32		tick_key_pressed;
	static u8		kb_last[2];
	extern kb_data_t	kb_event;
	kb_event.keycode[0] = 0;
	kb_event.keycode[1] = 0;
	int det_key = kb_scan_key (0, 1);
#if(DEEPBACK_FAST_KEYSCAN_ENABLE)
	if(deepback_key_state != DEEPBACK_KEY_IDLE){
		deepback_pre_proc(&det_key);
	}
#endif	
	///////////////////////////////////////////////////////////////////////////////////////
	//			key change:pressed or released
	///////////////////////////////////////////////////////////////////////////////////////
	if (det_key)	{
		/////////////////////////// key pressed  /////////////////////////////////////////
		key_released = 0;
		if (kb_event.cnt == 2)	 //two key press, do  not process
		{
		}
		else if(kb_event.cnt == 1)
		{			
			if(KEY_CMD == kb_event.keycode[0]){
				#if PTS_TEST_EN
		    	pts_test_case_lpn();
		    	#else
		        static u8 test_onoff;
				access_cmd_onoff(ADR_ALL_NODES, 0, (test_onoff++) & 1, CMD_NO_ACK, 0);
		        #endif
			}
		}
		///////////////////////////   key released	///////////////////////////////////////
		else {
			if(!long_pressed){
				if(KEY_RESET == kb_last[0]){
					if(LPN_MODE_GATT_OTA == lpn_mode){
			            lpn_mode_set(LPN_MODE_NORMAL);
			        }else{
			            lpn_mode_set(LPN_MODE_GATT_OTA);
			        }
				}
			}
			
			key_released = 1;
			long_pressed = 0;
		}

		tick_key_pressed = clock_time ();
		kb_last[0] = kb_event.keycode[0];
		kb_last[1] = kb_event.keycode[1];
	}
	//////////////////////////////////////////////////////////////////////////////////////////
	//				no key change event
	//////////////////////////////////////////////////////////////////////////////////////////
	else if (kb_last[0])
	{
		//	long pressed
		if (!long_pressed && clock_time_exceed(tick_key_pressed, 3*1000*1000))
		{
			long_pressed = 1;

			if(KEY_RESET == kb_last[0]){
				cfg_cmd_reset_node(ele_adr_primary);
			}

		}

	}else{
		key_released = 1;
		long_pressed = 0;
	}

	return;
}

int mesh_friend_offer_is_valid(mesh_ctl_fri_offer_t *p_offer)
{
    #if 0
    if(p_offer->RecWin > FRI_REC_WIN_MS){   // the larger receive window may cost more power when lost messages.
        // return 0;
    }
    
    if(p_offer->SubsListSize < SUB_LIST_MAX_LPN){
        // return 0;
    }
    #endif
    
    return ((p_offer->RecWin >= 1)&&(p_offer->CacheSize >= (1 << mesh_lpn_par.req.Criteria.MinCacheSizeLog))); // must
}

int mesh_is_better_offer(mesh_ctl_fri_offer_t *p_offer_new, mesh_ctl_fri_offer_t *p_offer)
{
    return (p_offer_new->RSSI > p_offer->RSSI);
}

int lpn_rx_offer_handle(u8 *bear)
{
    mesh_cmd_bear_t *p_bear = (mesh_cmd_bear_t *)bear;
    mesh_cmd_nw_t *p_nw = &p_bear->nw;
    mesh_cmd_lt_ctl_unseg_t *p_lt_ctl_unseg = &p_bear->lt_ctl_unseg;
    mesh_ctl_fri_offer_t *p_offer = (mesh_ctl_fri_offer_t *)(p_lt_ctl_unseg->data);
	LOG_MSG_LIB(TL_LOG_FRIEND,(u8 *)p_offer, sizeof(mesh_ctl_fri_offer_t),"rcv friend offer:");
    if(0 == mesh_friend_offer_is_valid(p_offer)){
        return -1;
    }
    
    //if(FN_RSSI_INVALID == p_offer->RSSI){
        adv_report_extend_t *p_extend = get_adv_report_extend(&p_bear->len);
        p_offer->RSSI = p_extend->rssi;    // rssi is measured by LPN self, instead of using rssi of Friend Node's request message which is recorded in offer message.
    //}
    
    u8 better = (!mesh_lpn_par.FriAdr) || mesh_is_better_offer(p_offer, &mesh_lpn_par.offer);
    if(better){
        memcpy(&mesh_lpn_par.offer, p_offer, sizeof(mesh_ctl_fri_offer_t));
        mesh_lpn_par.FriAdr = p_nw->src;
		#if BLE_MULTIPLE_CONNECTION_ENABLE
		if(mesh_lpn_par.offer.RecWin < 50){
			mesh_lpn_par.offer.RecWin = 50; // adv sending is not immediately in multiple connection now.
		}
		#endif
    }

    return 0;
}

void mesh_friend_ship_retry()
{
    if(fri_ship_proc_lpn.req_retrys++ < FRI_REQ_RETRY_MAX){      
    }else{
 //       mesh_friend_ship_proc_init_lpn(); // not init fri_ship_proc_lpn.req_tick for next friend request tick in loop.       
    }
	mesh_lpn_sleep_prepare(CMD_ST_SLEEP);
	mesh_friend_ship_set_st_lpn(FRI_ST_REQUEST);
}

void lpn_no_offer_handle()  // only call after send request message during establish friend ship
{
    if(is_lpn_support_and_en){
        if(fri_ship_proc_lpn.req_retrys < FRI_REQ_RETRY_MAX){
        #if WIN32
            u16 rand_ms = 0;
        #else
            //u16 rand_ms = (rand() & 0x7F);
        #endif
            mesh_friend_ship_retry();   // should be before suspend,because of deep retention.
        }else{
            mesh_friend_ship_retry();
        }
    }else{
        // should not happen here
        if(clock_time_exceed(fri_ship_proc_lpn.req_tick, FRI_REQ_TIMEOUT_MS*1000)){
            mesh_friend_ship_retry();
        }
    }
}

/**
 * @brief       This function server to manage LPN work state.
 * @return      none
 * @note        
 */
void mesh_lpn_state_proc()
{	
	//static u32 A_req_tick;
	static u8 fri_request_send = 1;
	if(fri_request_send && (LPN_MODE_GATT_OTA != lpn_mode)){
		fri_request_send = 0;
		//A_req_tick = clock_time();
		//if((!is_in_mesh_friend_st_lpn()) && (!fri_ship_proc_lpn.status)){
			if(is_provision_success()){
        		mesh_friend_ship_set_st_lpn(FRI_ST_REQUEST);
        	}
        //}
	}
	
    if(LPN_MODE_GATT_OTA == lpn_mode){
        // use BLE PM flow: BLE_REMOTE_PM_ENABLE
        if(blc_ll_getCurrentState() != BLS_LINK_STATE_CONN){
            if(clock_time_exceed(lpn_mode_tick, LPN_GATT_OTA_ADV_STATE_MAX_TIME_MS * 1000)){
                lpn_mode_set(LPN_MODE_NORMAL);
            }else{
                if(!is_led_busy()){
                    cfg_led_event (LED_EVENT_FLASH_1HZ_1T);
                }
            }
        }
        return ;
    }
    
	if(lpn_provision_ok){
		#if (!DEBUG_SUSPEND)
		if((blc_ll_getCurrentState() == BLS_LINK_STATE_ADV) && clock_time_exceed(lpn_wakeup_tick, LPN_WORKING_TIMEOUT_MS*1000)){
			#if BLE_REMOTE_PM_ENABLE
			if(!is_friend_ship_link_ok_lpn() && (FRI_ST_IDLE == fri_ship_proc_lpn.status)){
				mesh_friend_ship_set_st_lpn(FRI_ST_REQUEST);// prevent abnormal working time. if no sleep mode, lpn_wakeup_tick need to be refreshed which can refer to demo code in mesh_lpn_pm_proc().
			}
			#endif
		}
		#endif
	}else{
	    if(!is_provision_success()){
	        if(!is_provision_working() && !is_tlk_gatt_ota_busy()){   // not being provision
    	        if(clock_time_exceed(0, LPN_SCAN_PROVISION_START_TIMEOUT_MS*1000)){
    	            light_onoff_all(0);
                    cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);
                    while(1);   // wait reboot
    	        }
	        }
	    }else{
    	    if((!lpn_provision_ok) && node_binding_tick && clock_time_exceed(node_binding_tick, LPN_START_REQUEST_AFTER_BIND_MS*1000)){
				lpn_provision_ok = 1;// provison and key bind finish
				gatt_adv_send_flag = GATT_LPN_EN;				
				mesh_friend_ship_set_st_lpn(FRI_ST_REQUEST);
				if(BLS_LINK_STATE_CONN == blc_ll_getCurrentState()){
					bls_ll_terminateConnection(0x13); // disconnect to establish friendship
				}
    	    }
	    }
	    return ;
	}

	return;
}

/**
 * @brief       This function server to manage LPN power management. 
 * @return      none
 * @note        
 */
void mesh_lpn_pm_proc()
{
	if((BLS_LINK_STATE_CONN != blc_ll_getCurrentState())){	    
		set_blt_busy(0); 								// trigger pm in blt_sdk_main_loop to save power. (blt_busy will 1 after cpu_sleep_wakeup return STATUS_GPIO_ERR_NO_ENTER_PM while key pressing, and will not enter sleep in this interval.)
	}
	
	if(lpn_provision_ok){ 
		if(blc_ll_getCurrentState() == BLS_LINK_STATE_CONN){
			ENABLE_SUSPEND_MASK; 					// always enable pm in gatt connecting to save power.
	    }
	}

#if 0
// demo: lpn disable pm 20 seconds by key press, will overwrite previous pm setting. 
	extern u32 lpn_key_wakeup_start_tick;
	if(lpn_key_wakeup_start_tick){
		lpn_wakeup_tick = clock_time(); 			// for not to enter FRI_ST_REQUEST in mesh_lpn_state_proc() after LPN_WORKING_TIMEOUT_MS	
		set_sdk_mainLoop_run_flag(0); 				// for not to enter sleep after receive poll update, otherwise enter sleep directly after receive friend update.
		bls_pm_setSuspendMask(SUSPEND_DISABLE); 	// disable pm.
		
		if(clock_time_exceed(lpn_key_wakeup_start_tick, 20*1000*1000)){
			lpn_key_wakeup_start_tick = 0;
			ENABLE_SUSPEND_MASK;
		}
	}
#endif
}

void mesh_main_loop_LPN()
{
}

u8 mesh_lpn_quick_tx_flag = 0;

/**
 * @brief       This function server for lpn to send message in mesh_adv_cmd_fifo quickly.
 * @param[io]   is_quick_tx	- 0:send message once. 1:send all transmit cnt of the message in mesh_adv_cmd_fifo and pop it.
 * @return      ret
 * @note        
 */
int lpn_quick_tx(u8 is_quick_tx)
{
    u32 r = irq_disable();
	mesh_lpn_quick_tx_flag = is_quick_tx;
	int ret = mesh_send_adv2scan_mode(1);	
	mesh_lpn_quick_tx_flag = 0;
    lpn_debug_alter_debug_pin(0);
    irq_restore(r);
	return ret;

}

u32 get_lpn_poll_interval_ms()
{
	#if (MD_MESH_OTA_EN && MESH_OTA_PULL_MODE_EN)
	extern _align_4_ fw_update_srv_proc_t    fw_update_srv_proc;
	if(fw_update_srv_proc.pull_chunk_tick && (fw_update_srv_proc.busy || fw_update_srv_proc.blob_trans_busy)){
		return min(FRI_POLL_INTERVAL_MS_MESH_OTA, FRI_POLL_INTERVAL_MS);
	}
	#endif
	
	return FRI_POLL_INTERVAL_MS;
}

int mesh_lpn_adv_interval_update(u8 adv_tick_refresh)
{
	int ret = 0;

	u16 interval = (LPN_MODE_GATT_OTA == lpn_mode) ? ADV_INTERVAL_MS:(is_lpn_support_and_en?(is_friend_ship_link_ok_lpn() ? get_lpn_poll_interval_ms() : (FRI_REQ_TIMEOUT_MS-FRI_ESTABLISH_PERIOD_MS)):GET_ADV_INTERVAL_MS(ADV_INTERVAL_UNIT));
	ret = bls_ll_setAdvParam_interval(interval, 0);
#if !BLE_MULTIPLE_CONNECTION_ENABLE
	if(adv_tick_refresh){
		extern u32 blt_advExpectTime;
		blt_advExpectTime = clock_time() + blta.adv_interval;
	}
#endif

	return ret;
}

void lpn_mode_set(int mode)
{
    if(LPN_MODE_GATT_OTA == mode){
        LOG_MSG_LIB(TL_LOG_FRIEND,0, 0,"SW1:enter GATT OTA");
        lpn_mode = LPN_MODE_GATT_OTA;
		gatt_adv_send_flag = 1;
		ENABLE_SUSPEND_MASK;
        mesh_friend_ship_proc_init_lpn();
		mesh_friend_ship_clear_LPN(); 		// disconnect friendship
        lpn_mode_tick = clock_time();

		if(lpn_provision_ok){
			blc_ll_setScanEnable (0, 0);// disable scan to save power
		}
    }else if(LPN_MODE_NORMAL == mode){
        LOG_MSG_LIB(TL_LOG_FRIEND,0, 0,"SW1:exit GATT OTA");
        lpn_mode = LPN_MODE_NORMAL;
        mesh_friend_ship_set_st_lpn(FRI_ST_REQUEST);    // start to send friend request flow
        lpn_mode_tick = 0;
		gatt_adv_send_flag = 0;
        // cfg_led_event_stop();	// can't stop like this, because LED state needs to be recovered.
    }
	mesh_lpn_adv_interval_update(1);
}
#else
int lpn_quick_tx(u8 is_quick_tx){return 0;}
#endif 




