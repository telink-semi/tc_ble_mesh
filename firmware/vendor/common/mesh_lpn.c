/********************************************************************************************************
 * @file     mesh_lpn.c 
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
#include "../../proj/tl_common.h"
#if !WIN32
#include "../../proj/mcu/watchdog_i.h"
#endif 
#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../vendor/common/user_config.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/service/ble_ll_ota.h"
#include "app_health.h"
#include "../../proj_lib/sig_mesh/app_mesh.h"
#include "lighting_model.h"
#include "../../vendor/common/sensors_model.h"
#include "../../proj_lib/mesh_crypto/sha256_telink.h"
#include "mesh_common.h"
#include "../../vendor/common/app_heartbeat.h"
#if(HCI_ACCESS == HCI_USE_UART)
#include "../../proj/drivers/uart.h"
#endif

#if (IS_VC_PROJECT || TESTCASE_FLAG_ENABLE)
#define			DEBUG_SUSPEND				1
#else
#define			DEBUG_SUSPEND				0
#endif

//u8 fri_request_retry_max = FRI_REQ_RETRY_MAX;
u8 lpn_deep_retention_en = PM_DEEPSLEEP_RETENTION_ENABLE;
u8 mesh_lpn_rx_master_key = 0;
const u32 mesh_lpn_key_map[] = {SW1_GPIO, SW2_GPIO};
#define	MESH_LPN_CMD_KEY		        (SW2_GPIO)
#define	MESH_LPN_FACTORY_RESET_KEY		(SW1_GPIO)
u32 mesh_lpn_wakeup_key = 0;
u8 key_not_release = 0;
u32 active_time;

#if (FEATURE_LOWPOWER_EN || SPIRIT_PRIVATE_LPN_EN)
u8 lpn_provision_ok = 0;
#endif

#if FEATURE_LOWPOWER_EN
mesh_lpn_sleep_t  lpn_sleep;
u32 lpn_wakeup_tick = 0;

STATIC_ASSERT(LPN_ADV_INTERVAL_MS > (FRI_ESTABLISH_REC_DELAY_MS + FRI_ESTABLISH_WIN_MS + 10));//FRI_ESTABLISH_PERIOD_MS
STATIC_ASSERT(FRI_REQ_TIMEOUT_MS > 1100);
STATIC_ASSERT(LPN_POLL_TIMEOUT_100MS * 100 >= FRI_POLL_INTERVAL_MS * 2); // timeout should not be too short.
STATIC_ASSERT(SUB_LIST_MAX_LPN <= SUB_LIST_MAX);    // SUB_LIST_MAX_LPN should be equal to sub_list_max later
STATIC_ASSERT(SUB_LIST_MAX_LPN == SUB_LIST_MAX_IN_ONE_MSG);    // comfirm later



mesh_fri_ship_proc_lpn_t fri_ship_proc_lpn = {};	// for VC
mesh_lpn_subsc_list_t lpn_subsc_list;
mesh_subsc_list_retry_t subsc_list_retry = {};  // for retry procedure
mesh_lpn_par_t mesh_lpn_par = {0};

STATIC_ASSERT(((GET_ADV_INTERVAL_MS(ADV_INTERVAL_MAX))* TRANSMIT_CNT_DEF < FRI_ESTABLISH_REC_DELAY_MS) && (FRI_REC_DELAY_MS > FRI_ESTABLISH_REC_DELAY_MS));

int is_friend_ship_link_ok_lpn()
{
	return mesh_lpn_par.link_ok;
}

int is_unicast_friend_msg_to_fn(mesh_cmd_nw_t *p_nw)
{
    return (is_in_mesh_friend_st_lpn()
        && (mesh_lpn_par.LPNAdr == p_nw->src)
        && (mesh_lpn_par.FriAdr == p_nw->dst));    // must because of group address
}

void friend_cmd_send_request()
{
	LOG_MSG_INFO(TL_LOG_FRIEND,(u8 *)&mesh_lpn_par.req, sizeof(mesh_ctl_fri_req_t),"send friend request:",0);
	mesh_lpn_par.link_ok = 0;
    mesh_lpn_par.req.LPNCounter++;   // must before
    mesh_tx_cmd_layer_upper_ctl(CMD_CTL_REQUEST, (u8 *)&mesh_lpn_par.req, sizeof(mesh_ctl_fri_req_t), ele_adr_primary, ADR_ALL_NODES,0);
}

void friend_cmd_send_poll()
{
	LOG_MSG_INFO(TL_LOG_FRIEND,(u8 *)(&mesh_lpn_par.poll), sizeof(mesh_ctl_fri_poll_t),"send friend poll:",0);
    fri_ship_proc_lpn.poll_tick = clock_time()|1;
    mesh_tx_cmd_layer_upper_ctl(CMD_CTL_POLL, (u8 *)(&mesh_lpn_par.poll), sizeof(mesh_ctl_fri_poll_t), ele_adr_primary, mesh_lpn_par.FriAdr,0);
}

void friend_cmd_send_subsc_add(u8 *par_subsc, u32 len)  // only LPN support
{
	LOG_MSG_INFO(TL_LOG_FRIEND,(u8 *)par_subsc, len,"send friend sub list add:",0);
    subsc_list_retry.tick = clock_time();
    mesh_tx_cmd_layer_upper_ctl(CMD_CTL_SUBS_LIST_ADD, (u8 *)par_subsc, len, ele_adr_primary, mesh_lpn_par.FriAdr,0);
}

void friend_cmd_send_subsc_rmv(u8 *par_subsc, u32 len)  // only LPN support
{
	LOG_MSG_INFO(TL_LOG_FRIEND,(u8 *)par_subsc, len,"send friend sub list remove:",0);
    subsc_list_retry.tick = clock_time();
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
}

int friend_subsc_list_cmd_start(u8 type, u16 *adr_list, u32 subsc_cnt)
{
    if(subsc_cnt > SUB_LIST_MAX_IN_ONE_MSG){
        return -1;
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
    fri_ship_proc_lpn.status = st;
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
    mesh_ctl_fri_req_t req_bacup;
    memcpy(&req_bacup, &mesh_lpn_par.req, sizeof(req_bacup));
    memset(&mesh_lpn_par, 0, sizeof(mesh_lpn_par));
    memcpy(&mesh_lpn_par.req, &req_bacup, sizeof(req_bacup));
}

void mesh_friend_ship_proc_LPN(u8 *bear)
{
    if(blt_state == BLS_LINK_STATE_CONN){
        return ;
    }
    
    static u32 t_rec_delay_and_win = 0;
    u32 poll_retry_interval_ms = t_rec_delay_and_win;
	u32 timeout_ms = (poll_retry_interval_ms*1000) * (2*2+1)/2;    // comfirm later
    #if (0 == DEBUG_PROXY_FRIEND_SHIP)
    if(pts_test_en && fri_ship_proc_lpn.status){
        // retry poll should be more quicklier during establish friend ship.
        #define RETRY_POLL_INTV_MS_MAX      170 // FRND-LPN-BI01 need retry 3 times during 1 second after get offer.
        if(poll_retry_interval_ms > RETRY_POLL_INTV_MS_MAX){
            poll_retry_interval_ms = RETRY_POLL_INTV_MS_MAX;
        }
    }
    #endif
    if(fri_ship_proc_lpn.poll_retry && (!bear)
     && clock_time_exceed(fri_ship_proc_lpn.poll_tick, poll_retry_interval_ms*1000)){
        fri_ship_proc_lpn.poll_retry--;
        if(0 == fri_ship_proc_lpn.poll_retry){
            if(FRI_ST_UPDATE == fri_ship_proc_lpn.status){
                // retry request in "case FRI_ST_UPDATE:"
            }else{
                friend_subsc_stop();
                mesh_cmd_sig_lowpower_heartbeat();
                friend_ship_disconnect_cb_lpn();
                mesh_friend_ship_set_st_lpn(FRI_ST_REQUEST);    // restart establish procedure
            }
        }else{
            friend_cmd_send_poll();  // retry
        }
    }
    else if(subsc_list_retry.retry_cnt && (!bear) && clock_time_exceed(subsc_list_retry.tick, timeout_ms)){
        subsc_list_retry.tick = clock_time();   // also refresh when send_subsc
        subsc_list_retry.retry_cnt--;
        if(0 == subsc_list_retry.retry_cnt){
            // whether restart establish procedure or not
            mesh_friend_ship_set_st_lpn(FRI_ST_REQUEST);    // restart establish procedure
        }else{
            LOG_MSG_INFO(TL_LOG_FRIEND, 0, 0,"friend_subsc_repeat_***********************",0);
            friend_subsc_repeat();
        }
    }
	
    mesh_cmd_bear_unseg_t *p_bear = (mesh_cmd_bear_unseg_t *)bear;
    //mesh_cmd_nw_t *p_nw = &p_bear->nw;
    mesh_cmd_lt_ctl_unseg_t *p_lt_ctl_unseg = &p_bear->lt_ctl_unseg;
    u8 op = -1;
    if(bear){
	    op = p_lt_ctl_unseg->opcode;
    }
    
    if(0 == fri_ship_proc_lpn.status){
        if(bear){
            if(CMD_CTL_SUBS_LIST_CONF == op){
                mesh_ctl_fri_subsc_list_t *p_subsc = CONTAINER_OF(p_lt_ctl_unseg->data,mesh_ctl_fri_subsc_list_t,TransNo);
                if(p_subsc->TransNo == (subsc_list_retry.TransNo)){   //TransNo have increased
                	LOG_MSG_INFO(TL_LOG_FRIEND,(u8 *)&p_subsc->TransNo, sizeof(p_subsc->TransNo),"rcv sub list confirm:",0);
                    if(SUBSC_ADD == subsc_list_retry.retry_type){
                        friend_subsc_list_add_adr((lpn_adr_list_t *)(&lpn_subsc_list.adr), (lpn_adr_list_t *)(&subsc_list_retry.adr), subsc_list_retry.subsc_cnt);
                    }
                    friend_subsc_stop();
                }
            }else if(CMD_CTL_UPDATE == op){
                mesh_ctl_fri_update_t *p_update = (mesh_ctl_fri_update_t *)(p_lt_ctl_unseg->data);
				LOG_MSG_INFO(TL_LOG_FRIEND,(u8 *)p_update, sizeof(mesh_ctl_fri_update_t),"NW_IVI %d, rcv friend update:",p_bear->nw.ivi);
                iv_update_key_refresh_rx_handle(&p_update->flag, p_update->IVIndex);

            }
        }else{
        }
    }else{
        switch(fri_ship_proc_lpn.status){   // Be true only during establish friend ship.
            case FRI_ST_REQUEST:
                fri_ship_proc_lpn.req_tick = clock_time();
                friend_cmd_send_request();

                // init par
                mesh_friend_ship_clear_LPN();
                mesh_lpn_par.LPNAdr = ele_adr_primary;
                mesh_friend_ship_set_st_lpn(FRI_ST_OFFER);
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
                        }
                        mesh_friend_ship_set_st_lpn(FRI_ST_POLL);
                    }
                }
                break;
            case FRI_ST_POLL:
                if(is_friend_ship_link_ok_lpn()){
                    mesh_lpn_par.poll.FSN = 0;   // init
                    // send poll
                    fri_ship_proc_lpn.poll_retry = FRI_GET_UPDATE_RETRY_MAX + 1;
                    friend_cmd_send_poll();
                    
                    t_rec_delay_and_win = mesh_lpn_par.req.RecDelay + mesh_lpn_par.offer.RecWin;
                    mesh_friend_ship_set_st_lpn(FRI_ST_UPDATE);
                }else{
                    lpn_no_offer_handle();
                }
                break;
            case FRI_ST_UPDATE:
                if(bear){   // current state is establishing friend ship
                    if(CMD_CTL_UPDATE == p_lt_ctl_unseg->opcode){
                        mesh_ctl_fri_update_t *p_update = (mesh_ctl_fri_update_t *)(p_lt_ctl_unseg->data);
						LOG_MSG_INFO(TL_LOG_FRIEND,(u8 *)p_update, sizeof(mesh_ctl_fri_update_t),"rcv friend update:",0);
                        memcpy(&mesh_lpn_par.update, p_update, sizeof(mesh_ctl_fri_update_t));
                        //friend ship establish done
                        mesh_lpn_par.req.PreAdr = mesh_lpn_par.FriAdr;
						mesh_cmd_sig_lowpower_heartbeat();
                        iv_update_key_refresh_rx_handle(&p_update->flag, p_update->IVIndex);
                        mesh_friend_ship_proc_init_lpn();

                        friend_ship_establish_ok_cb_lpn();
                    }
                }else{
                    if(clock_time_exceed(fri_ship_proc_lpn.poll_tick, t_rec_delay_and_win*1000)){
                        mesh_friend_ship_retry();
                    }
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
    #define DEBUG_PIN_MESH      GPIO_PB7
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

void pts_test_case_lpn()
{
    static u32 tick_ts_test;
	static u8 lpn_pts_test_cmd;
	if(lpn_pts_test_cmd){
		if((TS_LPN_BV04 == lpn_pts_test_cmd)){
			subsc_add_rmv_test(1);
		}else if(TS_LPN_BV05 == lpn_pts_test_cmd){
		    tick_ts_test = clock_time()|1;
			subsc_add_rmv_test(1);
		}else if(TS_LPN_BV05_2 == lpn_pts_test_cmd){
			subsc_add_rmv_test(0);
		}else if(TS_LPN_BV08 == lpn_pts_test_cmd){
			friend_cmd_send_fn(0, CMD_CTL_CLEAR);
		}
		lpn_pts_test_cmd = 0;
	}

	if(tick_ts_test && clock_time_exceed(tick_ts_test, 1500*1000)){
	    tick_ts_test = 0;
	    lpn_pts_test_cmd = TS_LPN_BV05_2;
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
    p_req->NumEle = ELE_CNT;
    p_req->LPNCounter = 0 - 1;  // increase counter later
    #endif
}

void mesh_friend_ship_start_poll()
{
    if(is_in_mesh_friend_st_lpn()){
        friend_cmd_send_poll();
		fri_ship_proc_lpn.poll_retry = PTS_TEST_EN ? 20 : (FRI_POLL_RETRY_MAX + 1);
    }
}

void mesh_friend_ship_stop_poll()
{
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
        friend_subsc_add(&sub_adr, 1);
    }else if((CFG_MODEL_SUB_DEL == op)||(CFG_MODEL_SUB_VIRTUAL_ADR_DEL == op)){
        friend_subsc_rmv(&sub_adr, 1);  // check other model later
    }else if(((CFG_MODEL_SUB_OVER_WRITE == op)||(CFG_MODEL_SUB_VIRTUAL_ADR_OVER_WRITE == op))
    	   ||(CFG_MODEL_SUB_DEL_ALL == op)){
    }
}

void friend_send_current_subsc_list()
{
#if MD_SERVER_EN
    u16 adr_list[SUB_LIST_MAX_IN_ONE_MSG];
    int j = 0;
    model_common_t *p_model_com = &model_sig_g_onoff_level.onoff_srv[0].com; 
    foreach_arr(i, p_model_com->sub_list){
        u16 sub_adr = p_model_com->sub_list[i];
        if((sub_adr & 0x8000) && (!is_fixed_group(sub_adr))){
            adr_list[j++] = p_model_com->sub_list[i];
        }
        if(j >= SUB_LIST_MAX_IN_ONE_MSG){
            break;
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
    #define WAKEUP_LEVEL_LPN    0       // level : 1 (high); 0 (low)
    foreach_arr(i,mesh_lpn_key_map)
    {
        gpio_set_wakeup (mesh_lpn_key_map[i], WAKEUP_LEVEL_LPN, 1);         // level : 1 (high); 0 (low)
        cpu_set_gpio_wakeup (mesh_lpn_key_map[i], WAKEUP_LEVEL_LPN, 1);     // level : 1 (high); 0 (low)
    }
    gpio_core_wakeup_enable_all (1);
#endif

#if LPN_VENDOR_SENSOR_EN
    i2c_io_init();
#endif
}

void mesh_lpn_sleep_prepare(u16 op, u32 sleep_ms)
{
    lpn_sleep.sleep_ready = 0;  	// set in app_advertise_prepare_handler() ->mesh_lpn_sleep_set_ready()  later.
    lpn_sleep.sleep_ms = sleep_ms;
    lpn_sleep.op = op;
    lpn_sleep.tick_tx = clock_time();
}

void mesh_lpn_sleep_set_ready()
{
    lpn_sleep.sleep_ready = 1;
    //lpn_debug_alter_debug_pin(0);
}

void mesh_lpn_sleep_later_op(u16 op, u32 sleep_ms)	// sleep at mesh_lpn_proc_suspend()
{
    mesh_lpn_sleep_prepare(op, sleep_ms);
    mesh_lpn_sleep_set_ready();
}

void mesh_lpn_sleep_enter_normal_unseg()
{
    mesh_lpn_sleep_prepare(CMD_ST_NORMAL_UNSEG, FRI_POLL_INTERVAL_MS);
}

void mesh_lpn_sleep_enter_normal_seg()
{
    mesh_lpn_sleep_prepare(CMD_ST_NORMAL_SEG, CMD_INTERVAL_MS * 2);	// 0xf0
}

void mesh_lpn_sleep_enter_later()
{
    mesh_lpn_sleep_prepare(CMD_ST_SLEEP, FRI_POLL_INTERVAL_MS);
    #if !WIN32
    my_fifo_reset(&blt_rxfifo);     // clear buf
    #endif
}

u32 mesh_lpn_get_sleep_time()
{
	u32 time_cost_ms = (clock_time() - lpn_sleep.tick_tx)/CLOCK_SYS_CLOCK_1MS;
	if(time_cost_ms < lpn_sleep.sleep_ms){
		return (lpn_sleep.sleep_ms - time_cost_ms);
	}
	return 0;
}

void suspend_enter(u32 sleep_ms, int deep_retention_flag)
{
#if DEBUG_SUSPEND
    sleep_us(sleep_ms*1000);
#else
    int sleep_mode = 0; // SUSPEND_MODE default
    #if(__TL_LIB_8258__ || (MCU_CORE_TYPE == MCU_CORE_8258) || (MCU_CORE_TYPE == MCU_CORE_8278))
    sleep_mode = deep_retention_flag ? DEEPSLEEP_MODE_RET_SRAM_LOW32K : SUSPEND_MODE;
    #endif
	
	u8 pm_wakeup_type = PM_WAKEUP_TIMER;//in receive delay window(lpn_sleep.op is in range of 1~0x100), not allow pad wakeup.
	if((lpn_sleep.op==0) || (lpn_sleep.op>0x100)){
		pm_wakeup_type |= PM_WAKEUP_PAD;  
	}
    cpu_sleep_wakeup(sleep_mode, pm_wakeup_type, clock_time() + sleep_ms*CLOCK_SYS_CLOCK_1MS);
	lpn_wakeup_tick = clock_time();
#endif
}

u32 mesh_lpn_wakeup_key_io_get()
{
	#define WAKEUP_TRIGGER_LEVEL	0
	foreach_arr(i,mesh_lpn_key_map){
		if(WAKEUP_TRIGGER_LEVEL == gpio_read (mesh_lpn_key_map[i])){
			key_not_release = 1;	
			return mesh_lpn_key_map[i];
		}
	}

	return 0;
}

u8 mesh_lpn_key_not_release()
{
	if(key_not_release&&(mesh_lpn_wakeup_key_io_get()==0)){
		key_not_release++;
		if(key_not_release == 3){
			key_not_release = 0;
		}
	}
	return key_not_release;
}

int is_lpn_key_cmd_tx_trigger(int sleep_en)
{
    int trigger_flag = 0;
    static u32 lpn_cmd_key_last = 0;
    u32 key_val = mesh_lpn_wakeup_key_io_get();
    if(MESH_LPN_CMD_KEY == key_val){
        if(sleep_en){
            #if 0   // test add/remove subscription list
            subsc_add_rmv_test(1);  // sleep parameter will set when send control command
            #else   // test send message
            test_cmd_wakeup_lpn();
            #endif

            if(mesh_tx_seg_par.busy){
                mesh_lpn_sleep_enter_normal_seg();
            }else{
                mesh_lpn_sleep_enter_normal_unseg();
            }
            // should disable RX
        }else{
            if(0 == lpn_cmd_key_last){
                test_cmd_wakeup_lpn();
            }
        }
        
        trigger_flag = 1;
    }
    lpn_cmd_key_last = key_val;
    
    return trigger_flag;
}

void lpn_key_factory_reset_check()
{
    // long press key
    u32 detkey = 0;
    int long_press = 0;
    u32 long_tick = clock_time();
    do{
        detkey = mesh_lpn_wakeup_key_io_get();
        if(MESH_LPN_FACTORY_RESET_KEY == detkey){
            if(clock_time_exceed(long_tick, LONG_PRESS_TRIGGER_MS*1000)){
                long_press = 1;
                irq_disable();
                kick_out(); // reboot inside // show_factory_reset();
                detkey = 0;
                
                break;
            }
            
            if(blt_state != BLS_LINK_STATE_CONN){
                cpu_sleep_wakeup(0, PM_WAKEUP_TIMER, clock_time()+KEY_SCAN_INTERVAL_MS*1000*sys_tick_per_us);
            }
        }else{
            break;
        }
    }while(detkey);
}

#if DEBUG_SUSPEND
void cpu_sleep_wakeup_core_debug()
{
    while(1){
		sleep_us(1000);
		mesh_lpn_wakeup_key = mesh_lpn_wakeup_key_io_get();
		if(mesh_lpn_wakeup_key){
			break;
		}
    }
}
#endif

void mesh_lpn_suspend_timer_ll(int sleep_ms, int io_wakeup_en, int deep_retention_flag)
{
	if(!sleep_ms && !io_wakeup_en){
		return ;
	}
	
    u8 r = irq_disable();
    #if (!DEBUG_SUSPEND)
    usb_dp_pullup_en (0);
	//light_onoff_all(0);
    #endif
    lpn_debug_set_current_pin(0);

    if(sleep_ms){
        #if(__TL_LIB_8258__ || (MCU_CORE_TYPE == MCU_CORE_8258) || (MCU_CORE_TYPE == MCU_CORE_8278))
        suspend_enter(sleep_ms, deep_retention_flag);
        #else
        #define SLEEP_TIME_MAX_MS       (3000)
        while(sleep_ms){
            if(sleep_ms > SLEEP_TIME_MAX_MS){
                suspend_enter(SLEEP_TIME_MAX_MS, deep_retention_flag);
                sleep_ms -= SLEEP_TIME_MAX_MS;
            }else{
                suspend_enter(sleep_ms, deep_retention_flag);
                sleep_ms = 0;
            }
        }
        #endif
    }else{
        suspend_enter(1000, 0);   // wait for key release and then suspend
        #if DEBUG_SUSPEND
        cpu_sleep_wakeup_core_debug();
        #else
        cpu_sleep_wakeup(0, PM_WAKEUP_CORE, 0) ;
        #endif
    }
    
    lpn_debug_set_current_pin(1);
    #if (!DEBUG_SUSPEND)
    usb_dp_pullup_en (1);       // recover  to initial status
    #endif
    active_time = clock_time ();

    #if !WIN32
    my_fifo_reset(&blt_rxfifo);		// clear buf
    if(blt_state == BLS_LINK_STATE_ADV){
        reg_system_tick_irq = clock_time() + 15*CLOCK_SYS_CLOCK_1MS;   // make sure adv send check
    }
    irq_clr_src();
    #endif
    system_time_run();          // check time
    lpn_debug_set_current_pin(0);
    irq_restore (r);
    lpn_debug_set_current_pin(1);
}

void mesh_lpn_suspend_timer(int sleep_ms, int deep_retention_flag)
{
#if(HCI_LOG_FW_EN&&!DEBUG_SUSPEND)
	unsigned char uart_tx_is_busy();
	u32 begin_tick = clock_time();
	while(uart_tx_is_busy ()&& (clock_time() - begin_tick) < 500*CLOCK_SYS_CLOCK_1MS);
#endif
	mesh_lpn_suspend_timer_ll(sleep_ms, 0, deep_retention_flag);
	if(deep_retention_flag){
	    // should never run here.
        LOG_MSG_ERR(TL_LOG_COMMON,0,0,"enter retention sleep error, please check GPIO state", 0);
    }
}

void mesh_lpn_stop_to_wait_io_wakeup()
{
#if 1
    mesh_lpn_suspend_timer_ll(0, 1, 0);
#else
    //if(is_lpn_support_and_en){
        u8 r = irq_disable();
        usb_dp_pullup_en (0);
        cpu_sleep_wakeup(0, PM_WAKEUP_CORE, 0) ;
        usb_dp_pullup_en (1);       // recover  to initial status
        active_time = clock_time ();
        irq_restore (r);
    //}
#endif
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
    mesh_cmd_bear_unseg_t *p_bear = (mesh_cmd_bear_unseg_t *)bear;
    mesh_cmd_nw_t *p_nw = &p_bear->nw;
    mesh_cmd_lt_ctl_unseg_t *p_lt_ctl_unseg = &p_bear->lt_ctl_unseg;
    mesh_ctl_fri_offer_t *p_offer = (mesh_ctl_fri_offer_t *)(p_lt_ctl_unseg->data);
	LOG_MSG_INFO(TL_LOG_FRIEND,(u8 *)p_offer, sizeof(mesh_ctl_fri_offer_t),"rcv friend offer:",0);
    if(0 == mesh_friend_offer_is_valid(p_offer)){
        return -1;
    }
    
    //if(FN_RSSI_INVALID == p_offer->RSSI){
        event_adv_report_t *pa = CONTAINER_OF(&p_bear->len,event_adv_report_t,data[0]);
        adv_report_extend_t *p_extend = (adv_report_extend_t *)(pa->data+(p_bear->len+1));
        p_offer->RSSI = p_extend->rssi;    // rssi: measure by LPN self,
    //}
    
    u8 better = (!mesh_lpn_par.FriAdr) || mesh_is_better_offer(p_offer, &mesh_lpn_par.offer);
    if(better){
        memcpy(&mesh_lpn_par.offer, p_offer, sizeof(mesh_ctl_fri_offer_t));
        mesh_lpn_par.FriAdr = p_nw->src;
    }

    return 0;
}

void mesh_friend_ship_retry()
{
    if(fri_ship_proc_lpn.req_retrys++ < FRI_REQ_RETRY_MAX){
        mesh_friend_ship_set_st_lpn(FRI_ST_REQUEST);
    }else{
        mesh_friend_ship_proc_init_lpn();
        mesh_lpn_sleep_enter_later();
    }
}

void lpn_no_offer_handle()  // only call after send request message during establish friend ship
{
    if(is_lpn_support_and_en){
        if(fri_ship_proc_lpn.req_retrys < FRI_REQ_RETRY_MAX){
        #if WIN32
            u16 rand_ms = 0;
        #else
            u16 rand_ms = (rand() & 0x7F);
        #endif
            mesh_friend_ship_retry();   // should be before suspend,because of deep retention.
            mesh_lpn_suspend_timer(FRI_REQ_TIMEOUT_MS + rand_ms - FRI_ESTABLISH_PERIOD_MS, lpn_deep_retention_en);
            // can't add any function here, because of deep retention.
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

#if PM_DEEPSLEEP_RETENTION_ENABLE
lpn_deep_handle_t lpn_deep_handle = {0};

#define MESH_LPN_SUSPEND_TYPE       HANDLE_RETENTION_DEEP_PRE
#else
#define MESH_LPN_SUSPEND_TYPE       HANDLE_SUSPEND_NORMAL
#endif

/*
    func: include request interval and poll interval.
*/
void suspend_handle_next_poll_interval(int handle_type)
{
#if PM_DEEPSLEEP_RETENTION_ENABLE
    lpn_deep_handle.type = handle_type;
    lpn_deep_handle.event = LPN_SUSPEND_EVENT_NEXT_POLL_INV;
#endif
    int deep_retention_flag = (handle_type != HANDLE_SUSPEND_NORMAL);
    if(is_friend_ship_link_ok_lpn()){
        if(handle_type & HANDLE_RETENTION_DEEP_PRE){
            lpn_debug_alter_debug_pin(0);
            lpn_debug_set_irq_pin(0);
            #if 0   // test
            mesh_lpn_stop_to_wait_io_wakeup();
                #if (!DEBUG_SUSPEND)
            mesh_lpn_wakeup_key = mesh_lpn_wakeup_key_io_get();
                #endif
            #else
            mesh_lpn_suspend_timer(FRI_POLL_INTERVAL_MS, deep_retention_flag);
            #endif
            /* it should never run here, if "handle_type" is HANDLE_RETENTION_DEEP_PRE,
                     after wakeup,it will jump to user_init_deepRetn()->suspend_handle_next_poll_interval_().
                  */
        }

        if(handle_type & HANDLE_RETENTION_DEEP_AFTER){
            lpn_debug_alter_debug_pin(1);
            lpn_debug_alter_debug_pin(0);
            
            if(0 == is_lpn_key_cmd_tx_trigger(1)){
                mesh_friend_ship_start_poll();
            }
        }
    }else{
        if(handle_type & HANDLE_RETENTION_DEEP_PRE){
            u32 adv_inv_1 = 0;
            #if (LPN_ADV_INTERVAL_EN)
            adv_inv_1 = LPN_ADV_INTERVAL_MS - FRI_ESTABLISH_PERIOD_MS;
                #if PM_DEEPSLEEP_RETENTION_ENABLE
            lpn_deep_handle.type = HANDLE_RETENTION_DEEP_ADV_PRE;   // type after wakeup
            lpn_deep_handle.adv_cnt = 1;            // confirm later
            mesh_lpn_suspend_timer(adv_inv_1, 1);       // reboot here, if deep
                #else
            send_gatt_adv_right_now();
            mesh_lpn_suspend_timer(adv_inv_1, 0);
                #endif
            #endif
            u32 rand_val = rand() & 0x7f;    // 0--128ms
            mesh_lpn_suspend_timer(FRI_REQ_RETRY_IDLE_MS + rand_val - adv_inv_1, deep_retention_flag);
            
            /* it should never run here, if "handle_type" is HANDLE_RETENTION_DEEP_PRE,
                       after wakeup,it will jump to user_init_deepRetn()->suspend_handle_next_poll_interval_().
                   */
        }
        
        #if PM_DEEPSLEEP_RETENTION_ENABLE
        if(handle_type & HANDLE_RETENTION_DEEP_ADV_PRE){
            u32 adv_inv_1 = LPN_ADV_INTERVAL_MS - FRI_ESTABLISH_PERIOD_MS;
            lpn_deep_handle.type = HANDLE_RETENTION_DEEP_PRE;   // type after wakeup
            lpn_deep_handle.adv_cnt = 0;            // confirm later
            send_gatt_adv_right_now();
            if(blt_state == BLS_LINK_STATE_ADV){
                u32 rand_val = rand() & 0x7f;    // 0--128ms
                mesh_lpn_suspend_timer(FRI_REQ_RETRY_IDLE_MS + rand_val - adv_inv_1, 1);
            }else{ // BLS_LINK_STATE_CONN
                //mesh_lpn_sleep_enter_later();   // preset sleep parameters when BLE disconnect.
                mesh_lpn_sleep_later_op(CMD_ST_SLEEP, FRI_POLL_INTERVAL_MS);   // preset sleep parameters when BLE disconnect.
            }
        }
        #endif
        
        if(handle_type & HANDLE_RETENTION_DEEP_AFTER){
            //mesh_lpn_stop_to_wait_io_wakeup();
            //if(0 == is_lpn_key_cmd_tx_trigger()){ // check in mesh_lpn_proc_suspend_()
                mesh_friend_ship_set_st_lpn(FRI_ST_REQUEST);    // restart establish
            //}
        }
    }
}

void suspend_handle_wakeup_rx(int handle_type)
{
#if PM_DEEPSLEEP_RETENTION_ENABLE
    lpn_deep_handle.type = handle_type;
    lpn_deep_handle.event = LPN_SUSPEND_EVENT_WAKEUP_RX;
#endif

    if(handle_type & HANDLE_RETENTION_DEEP_PRE){
        lpn_debug_alter_debug_pin(0);
        mesh_lpn_suspend_timer(mesh_lpn_get_sleep_time(), (handle_type != HANDLE_SUSPEND_NORMAL));
        /* it should never run here, if "handle_type" is HANDLE_RETENTION_DEEP_PRE,
                 after wakeup,it will jump to user_init_deepRetn()->suspend_handle_wakeup_rx_().
            */
    }
    
    if(handle_type & HANDLE_RETENTION_DEEP_AFTER){
        if(CMD_ST_POLL_MD == lpn_sleep.op){
            mesh_friend_ship_start_poll();
        }
        lpn_debug_alter_debug_pin(0);
        memset(&lpn_sleep, 0, sizeof(lpn_sleep));   // init
    }
}

void mesh_lpn_proc_suspend ()
{
    // don't send command in mesh_lpn_proc_suspend(), if not it will cause nested, because lpn_quick_tx_and_suspend().
    lpn_key_factory_reset_check(); // include mesh_lpn_wakeup_key_io_get_();   // check release
    
	if(lpn_provision_ok){
	    #if (FRI_ESTABLISH_WIN_MS > 100)
	    if(FRI_ST_OFFER == fri_ship_proc_lpn.status){ // MCU is active and waiting for rx offer.
            static u32 tick_lpn_key_scan;
	        if(clock_time_exceed(tick_lpn_key_scan, KEY_SCAN_INTERVAL_MS * 1000)){
	            tick_lpn_key_scan = clock_time();
	            is_lpn_key_cmd_tx_trigger(0);    // check key and tx command
	        }
	    }
	    #endif
	    
        if(blt_state == BLS_LINK_STATE_CONN){
            return ;
        }
		#if (!DEBUG_SUSPEND)
		if(clock_time_exceed(lpn_wakeup_tick, LPN_WORKING_TIMEOUT_MS*1000)){
			mesh_friend_ship_set_st_lpn(FRI_ST_REQUEST);// prevent abnormal working time.
		}
		#endif
	}else{
	    if(!is_provision_success()){
	        if(!is_provision_working()){   // not being provision
    	        if(clock_time_exceed(0, LPN_SCAN_PROVISION_START_TIMEOUT_MS*1000)){
    	            light_onoff_all(0);
                    cpu_sleep_wakeup(1, PM_WAKEUP_PAD, 0);
                    while(1);   // wait reboot
    	        }
	        }
	    }else{
    	    if(node_binding_tick && clock_time_exceed(node_binding_tick, LPN_START_REQUEST_AFTER_BIND_MS*1000)){
    	        start_reboot();//lpn_provision_ok = 1;
    	    }
	    }
	    return ;
	}
	
    if(lpn_sleep.op && is_lpn_support_and_en){
        if((CMD_ST_SLEEP != lpn_sleep.op)/*&&(CMD_ST_NORMAL_SEG != lpn_sleep.op)*/){
            if(0 == lpn_sleep.sleep_ready){
                return ;
            }
        }

    	if(my_fifo_get(&mesh_adv_cmd_fifo)){
    	    #if 0
    	    lpn_quick_tx_and_suspend(0);     // may nested loop,
    	    #else
    	    lpn_quick_send_adv();
    	    return ;
    	    #endif
    	}
		
		if(mesh_lpn_key_not_release()&&((lpn_sleep.op==0)||(lpn_sleep.op>=0x100))){//detect key release, if lpn_sleep.op is in range of 1~0x100, not allow pad wakeup.
			cpu_sleep_wakeup(0, PM_WAKEUP_TIMER, clock_time()+40*1000*sys_tick_per_us);
			return;
		}

        u8 r = irq_disable();
        if((CMD_ST_SLEEP == lpn_sleep.op) || (CMD_ST_NORMAL_UNSEG == lpn_sleep.op)){
            // enter to long sleep
            memset(&lpn_sleep, 0, sizeof(lpn_sleep));
            if(!fri_ship_proc_lpn.status){
                suspend_handle_next_poll_interval(MESH_LPN_SUSPEND_TYPE);
            }
        }else if(CMD_ST_NORMAL_SEG == lpn_sleep.op){    // no need to enter retention deep sleep
            // wake up for send next segment packet
            mesh_lpn_suspend_timer(mesh_lpn_get_sleep_time(), 0);
			mesh_friend_ship_stop_poll();
            if(mesh_tx_seg_par.busy){
                if(is_tx_seg_one_round_ok()){
	                memset(&lpn_sleep, 0, sizeof(lpn_sleep));   // no need
					mesh_lpn_suspend_timer(FRI_LPN_WAIT_SEG_ACK_MS, 0);	// wait for ACK from destination
					mesh_friend_ship_start_poll();
				}
            }
        }else{
            // wake up for rx after send ctrl op, such as POLL,...
            // can't use HANDLE_RETENTION_DEEP_PRE now, because it is from lpn_quick_tx_and_suspend(), not at the end of main loop
            suspend_handle_wakeup_rx(HANDLE_SUSPEND_NORMAL);
        }
        irq_restore(r);
    }
}

void mesh_main_loop_LPN()
{
}

void suspend_quick_check()
{
    if(lpn_sleep.sleep_ready || lpn_sleep.op){
        mesh_lpn_proc_suspend();    // run only when send ctrl messages, so no need care it is not at the last of main loop.
    }
}

u8 send_gatt_adv_now_flag = 0;
void send_gatt_adv_right_now() // must be in irq disable state.
{
    send_gatt_adv_now_flag = 1;
    blt_send_adv2scan_mode(1);
    send_gatt_adv_now_flag = 0;
}

void lpn_quick_tx_and_suspend(int suspend, u8 op)
{
    u8 r = irq_disable();
	blt_send_adv2scan_mode(1);
    lpn_debug_alter_debug_pin(0);
    if(suspend){
        #if LPN_ADV_EN
        static u32 tick_adv_lpn;
        if((CMD_CTL_REQUEST == op)||(CMD_CTL_POLL == op)){
            if(clock_time_exceed(tick_adv_lpn, 1000*1000)){
                tick_adv_lpn = clock_time();
                send_gatt_adv_right_now();
            }
        }
        #endif
        suspend_quick_check(); // tx handle ok
    }
    irq_restore(r);
}
#else
void suspend_quick_check(){}
void lpn_quick_tx_and_suspend(int suspend, u8 op){}
#endif 




