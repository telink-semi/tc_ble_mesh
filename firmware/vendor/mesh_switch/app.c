/********************************************************************************************************
 * @file	app.c
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
#include "proj_lib/rf_drv.h"
#include "proj_lib/pm.h"
#include "proj_lib/ble/ll/ll.h"
#include "proj_lib/ble/blt_config.h"
#include "proj_lib/ble/ll/ll_whitelist.h"
#include "proj_lib/ble/trace.h"
#include "proj/mcu/pwm.h"
#include "proj/mcu/watchdog_i.h"
#include "proj_lib/ble/service/ble_ll_ota.h"
#include "proj/drivers/adc.h"
#include "proj_lib/ble/blt_config.h"
#include "proj_lib/ble/ble_smp.h"
#include "proj_lib/mesh_crypto/mesh_crypto.h"
#include "proj_lib/mesh_crypto/mesh_md5.h"

#include "proj_lib/sig_mesh/app_mesh.h"
#include "../common/app_provison.h"
#include "../common/app_beacon.h"
#include "../common/app_proxy.h"
#include "../common/app_health.h"
#include "proj/drivers/keyboard.h"
#include "app.h"
#include "stack/ble/gap/gap.h"
#include "vendor/common/blt_soft_timer.h"
#include "proj/drivers/rf_pa.h"

#if (HCI_ACCESS==HCI_USE_UART)
#include "proj/drivers/uart.h"
#endif

#if AIS_ENABLE
#define BLT_RX_BUF_NUM	16
#else
#define BLT_RX_BUF_NUM	8
#endif
MYFIFO_INIT(blt_rxfifo, 64, BLT_RX_BUF_NUM);
MYFIFO_INIT(blt_txfifo, 40, 16);

#define SLEEP_MAX_S						(32*60*60)	// max 37 hours,but use 32 to be multipled of SWITCH_IV_SEARCHING_INVL_S
#if IV_UPDATE_TEST_EN
#define SWITCH_IV_SEARCHING_INTERVLAL_S	(IV_UPDATE_KEEP_TMIE_MIN_RX_S)
#else
#define SWITCH_IV_SEARCHING_INTERVLAL_S	(96*60*60)	// keep searching for SWITCH_IV_RCV_WINDOW_S(10s default)
#endif
#define SWITCH_IV_SEARCHING_INVL_S		(SWITCH_IV_SEARCHING_INTERVLAL_S - 1)	// -1 to make sure "clock time exceed" is ture when time is just expired.
#define SWITCH_LONG_SLEEP_TIME_S		(min(SLEEP_MAX_S, SWITCH_IV_SEARCHING_INTERVLAL_S))
#define SWITCH_IV_RCV_WINDOW_S			(10)

u8 switch_mode = SWITCH_MODE_NORMAL;
u32 switch_mode_tick = 0;
u8 switch_provision_ok = 0;
u32 switch_iv_updata_s = 0;

STATIC_ASSERT(SWITCH_IV_SEARCHING_INTERVLAL_S % SWITCH_LONG_SLEEP_TIME_S == 0); // must be multipled.

//----------------------- UI ---------------------------------------------
#if (BLT_SOFTWARE_TIMER_ENABLE)
/**
 * @brief   This function is soft timer callback function.
 * @return  <0:delete the timer; ==0:timer use the same interval as prior; >0:timer use the return value as new interval. 
 */
int soft_timer_test0(void)
{
	//gpio 0 toggle to see the effect
	DBG_CHN4_TOGGLE;
	static u32 soft_timer_cnt0 = 0;
	soft_timer_cnt0++;
	return 0;
}
#endif

void switch_iv_update_time_refresh()
{
	switch_iv_updata_s = clock_time_s();
//	LOG_MSG_INFO(TL_LOG_IV_UPDATE,0, 0,"switch_iv_update_time_refresh time_s:%d", switch_iv_updata_s);
}

int soft_timer_rcv_beacon_timeout()
{
	if(SWITCH_MODE_NORMAL == switch_mode){
		ENABLE_SUSPEND_MASK;
		blc_ll_setScanEnable (0, 0);
	}
	return -1;
}

void switch_triger_iv_search_mode()
{
	if(clock_time_exceed_s(switch_iv_updata_s, SWITCH_IV_SEARCHING_INVL_S)){	
		LOG_MSG_INFO(TL_LOG_IV_UPDATE,0, 0,"switch enter iv update search mode time_s:%d", clock_time_s());
		switch_iv_update_time_refresh();		
		app_enable_scan_all_device (); // support pb adv
		bls_pm_setSuspendMask (SUSPEND_DISABLE);
		blt_soft_timer_update(&soft_timer_rcv_beacon_timeout, SWITCH_IV_RCV_WINDOW_S*1000*1000);
		mesh_send_adv2scan_mode(0);
	}
}

//----------------------- handle BLE event ---------------------------------------------
int app_event_handler (u32 h, u8 *p, int n)
{
	static u32 event_cb_num;
	event_cb_num++;
	int send_to_hci = 1;

	if (h == (HCI_FLAG_EVENT_BT_STD | HCI_EVT_LE_META))		//LE event
	{
		u8 subcode = p[0];

	//------------ ADV packet --------------------------------------------
		if (subcode == HCI_SUB_EVT_LE_ADVERTISING_REPORT)	// ADV packet
		{
			event_adv_report_t *pa = (event_adv_report_t *)p;
			if(LL_TYPE_ADV_NONCONN_IND != (pa->event_type & 0x0F)){
				return 0;
			}
			
			#if DEBUG_MESH_DONGLE_IN_VC_EN
			send_to_hci = (0 == mesh_dongle_adv_report2vc(pa->data, MESH_ADV_PAYLOAD));
			#else
			send_to_hci = (0 == app_event_handler_adv(pa->data, MESH_BEAR_ADV, 1));
			#endif
		}

	//------------ connection complete -------------------------------------
		else if (subcode == HCI_SUB_EVT_LE_CONNECTION_COMPLETE)	// connection complete
		{
			event_connection_complete_t *pc = (event_connection_complete_t *)p;
			if (!pc->status)							// status OK
			{
				//app_led_en (pc->handle, 1);

				//peer_type = pc->peer_adr_type;
				//memcpy (peer_mac, pc->mac, 6);
			}
			#if DEBUG_BLE_EVENT_ENABLE
			rf_link_light_event_callback(LGT_CMD_BLE_CONN);
			#endif

			#if DEBUG_MESH_DONGLE_IN_VC_EN
			debug_mesh_report_BLE_st2usb(1);
			#endif
			proxy_cfg_list_init_upon_connection();
			mesh_service_change_report();
		}

	//------------ connection update complete -------------------------------
		else if (subcode == HCI_SUB_EVT_LE_CONNECTION_UPDATE_COMPLETE)	// connection update
		{
		}
	}

	//------------ disconnect -------------------------------------
	else if (h == (HCI_FLAG_EVENT_BT_STD | HCI_EVT_DISCONNECTION_COMPLETE))		//disconnect
	{

		event_disconnection_t	*pd = (event_disconnection_t *)p;
		//app_led_en (pd->handle, 0);
		//terminate reason
		if(pd->reason == HCI_ERR_CONN_TIMEOUT){

		}
		else if(pd->reason == HCI_ERR_REMOTE_USER_TERM_CONN){  //0x13

		}
		else if(pd->reason == SLAVE_TERMINATE_CONN_ACKED || pd->reason == SLAVE_TERMINATE_CONN_TIMEOUT){

		}
		#if DEBUG_BLE_EVENT_ENABLE
		rf_link_light_event_callback(LGT_CMD_BLE_ADV);
		#endif 

		#if DEBUG_MESH_DONGLE_IN_VC_EN
		debug_mesh_report_BLE_st2usb(0);
		#endif
		if(switch_provision_ok){
			switch_mode_set(SWITCH_MODE_NORMAL);
		}
		mesh_ble_disconnect_cb(pd->reason);
	}

	if (send_to_hci)
	{
		//blc_hci_send_data (h, p, n);
	}

	return 0;
}

void proc_ui()
{
	static u32 tick_scan, scan_io_interval_us = 4000;
	if (!clock_time_exceed (tick_scan, scan_io_interval_us))
	{
		return;
	}
	tick_scan = clock_time();

	mesh_proc_keyboard();
}

#define		ADV_TIMEOUT					(30*1000) //30s

#if UI_BUTTON_MODE_ENABLE
#define KEY_PRESS_DEBOUNCE_FILTER_CNT		(2)
u8 key_pressing_cnt = KEY_PRESS_DEBOUNCE_FILTER_CNT;
#define scan_pin_need	key_pressing_cnt

void mesh_proc_keyboard ()
{
#if 0 // have set interval by sleep 40000us in switch_check_and_enter_sleep_().
	static u32 tick, scan_io_interval_us = 40000;
	if (!clock_time_exceed (tick, scan_io_interval_us))
	{
		return;
	}
	tick = clock_time();
#endif

	int key_pressing_flag = 0;
#if 1
	{
		static u8 st_sw1_step;	
	    static u32 long_tick1;
		u8 st_sw1 = !gpio_read(SW1_GPIO);
		key_pressing_flag = st_sw1;
		if(st_sw1){
			if(0 == st_sw1_step){
				st_sw1_step = 1;
				long_tick1 = clock_time();
			}else if(1 == st_sw1_step){
				if(clock_time_exceed(long_tick1, 3000*1000)){
					// long pressed
					st_sw1_step = 2;
					// LOG_MSG_LIB(TL_LOG_NODE_SDK, 0, 0, "key1 long", 0);
					switch_mode_set(SWITCH_MODE_GATT);
				}
			}else{
				// nothing to do
			}
		}else{
			if(1 == st_sw1_step){
				// short press
				// LOG_MSG_LIB(TL_LOG_NODE_SDK, 0, 0, "key1 short", 0);
				if(SWITCH_MODE_GATT == switch_mode){
					switch_mode_set(SWITCH_MODE_NORMAL); // press any key can exit adv mode
				}
				
				static u8 sw_onoff_cnt;
				u8 sw_onoff_flag = (sw_onoff_cnt++) & 1;	// light OFF first
				access_cmd_onoff(ADR_ALL_NODES, 0, sw_onoff_flag, CMD_NO_ACK, 0);
			}else if(2 == st_sw1_step){
				// nothing to do
			}
			st_sw1_step = 0;
		}
	}
#endif
#if 1
	{
		static u8 st_sw2_step;	
	    static u32 long_tick2;
		u8 st_sw2 = !gpio_read(SW2_GPIO);
		key_pressing_flag |= st_sw2;
		if(st_sw2){
			if(0 == st_sw2_step){
				st_sw2_step = 1;
				long_tick2 = clock_time();
			}else if(1 == st_sw2_step){
				if(clock_time_exceed(long_tick2, 3000*1000)){
					// long pressed
					st_sw2_step = 2;
					// LOG_MSG_LIB(TL_LOG_NODE_SDK, 0, 0, "key2 long", 0);
				}
			}else{
				// nothing to do
			}
		}else{
			if(1 == st_sw2_step){
				// short press
				// LOG_MSG_LIB(TL_LOG_NODE_SDK, 0, 0, "key2 short", 0);
			}else if(2 == st_sw2_step){
				// nothing to do
			}
			st_sw2_step = 0;
		}
	}
#endif

	if(key_pressing_flag){
		key_pressing_cnt = KEY_PRESS_DEBOUNCE_FILTER_CNT;
		if(SWITCH_MODE_NORMAL == switch_mode){ // !is_led_busy()
			rf_link_light_event_callback(LGT_CMD_SWITCH_CMD);
		}
	}else if(key_pressing_cnt){
		key_pressing_cnt--;
	}
}


void deep_wakeup_proc(void)
{
	// NULL
}
#else
static int	rc_repeat_key = 0;
static int	rc_key_pressed;
static int	rc_long_pressed;
static u32  mesh_active_time;
static u8   key_released =1;
#define		ACTIVE_INTERVAL				32000

void deep_wakeup_proc(void)
{
#if(DEEPBACK_FAST_KEYSCAN_ENABLE)
	//if deepsleep wakeup is wakeup by GPIO(key press), we must quickly scan this press
	if(kb_scan_key (KB_NUMLOCK_STATUS_POWERON, 1)){
		deepback_key_state = DEEPBACK_KEY_CACHE;
		memcpy(&kb_event_cache,&kb_event,sizeof(kb_event));
	}
#endif
}

void deepback_pre_proc(int *det_key)
{
#if (DEEPBACK_FAST_KEYSCAN_ENABLE)
	// to handle deepback key cache
	if(!(*det_key) && deepback_key_state == DEEPBACK_KEY_CACHE){

		memcpy(&kb_event,&kb_event_cache,sizeof(kb_event));
		*det_key = 1;
		deepback_key_state = DEEPBACK_KEY_IDLE;
	}
#endif
}

#define SWITCH_GROUP_ADDR_START 	0xc000

void mesh_proc_keyboard ()
{
	static u32		tick_key_pressed, tick_key_repeat;
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
	if (det_key) 	{
		if(key_released && (SWITCH_MODE_GATT == switch_mode)){
			switch_mode_set(SWITCH_MODE_NORMAL); // press any key can exit adv mode
		}
		/////////////////////////// key pressed  /////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////
		if (kb_event.keycode[0]) {
		    mesh_active_time = clock_time();
            if ((kb_event.keycode[0] == RC_KEY_A_ON && kb_event.keycode[1] == RC_KEY_1_OFF) ||
                (kb_event.keycode[1] == RC_KEY_A_ON && kb_event.keycode[0] == RC_KEY_1_OFF))
		    {
		    	// trigger the adv mode ,enterring configuration mode
		    	
			}else if ((kb_event.keycode[0] == RC_KEY_A_ON && kb_event.keycode[1] == RC_KEY_4_OFF) ||
                (kb_event.keycode[1] == RC_KEY_A_ON && kb_event.keycode[0] == RC_KEY_4_OFF))
		    {
                //irq_disable();
		    }else{
		    	#if 1
				static u8 last_lum = 100;
				static u8 last_tmp = 100;			
				u16 pub_addr = SWITCH_GROUP_ADDR_START;
				model_common_t *p_model;
				u8 model_idx = 0;
				
    			if (kb_event.keycode[0] == RC_KEY_1_ON){	
					p_model = (model_common_t *)mesh_find_ele_resource_in_model(ele_adr_primary,SIG_MD_G_ONOFF_C, 1,&model_idx, 0);
					if(p_model && p_model->pub_adr){
						pub_addr = p_model->pub_adr;
					}    			
					access_cmd_onoff(pub_addr, 0, G_ON, CMD_NO_ACK, 0);
    			}else if (kb_event.keycode[0] == RC_KEY_2_ON){
    			    p_model = (model_common_t *)mesh_find_ele_resource_in_model(ele_adr_primary+1,SIG_MD_G_ONOFF_C, 1,&model_idx, 0);
					if(p_model && p_model->pub_adr){
						pub_addr = p_model->pub_adr;
					}
					else{
						pub_addr += 1;
					}
					access_cmd_onoff(pub_addr, 0, G_ON, CMD_NO_ACK, 0);
    			}else if (kb_event.keycode[0] == RC_KEY_3_ON){
    			    p_model = (model_common_t *)mesh_find_ele_resource_in_model(ele_adr_primary+2,SIG_MD_G_ONOFF_C, 1,&model_idx, 0);
					if(p_model && p_model->pub_adr){
						pub_addr = p_model->pub_adr;
					} 
					else{
						pub_addr += 2;
					}
					access_cmd_onoff(pub_addr, 0, G_ON, CMD_NO_ACK, 0);
    			}else if (kb_event.keycode[0] == RC_KEY_4_ON){
    			    p_model = (model_common_t *)mesh_find_ele_resource_in_model(ele_adr_primary+3,SIG_MD_G_ONOFF_C, 1,&model_idx, 0);
					if(p_model && p_model->pub_adr){
						pub_addr = p_model->pub_adr;
					} 
					else{
						pub_addr += 3;
					}
					access_cmd_onoff(pub_addr, 0, G_ON, CMD_NO_ACK, 0);
    			}else if (kb_event.keycode[0] == RC_KEY_A_ON){
					access_cmd_onoff(ADR_ALL_NODES, 0, G_ON, CMD_NO_ACK, 0);					
    			}else if (kb_event.keycode[0] == RC_KEY_1_OFF){
    			    p_model = (model_common_t *)mesh_find_ele_resource_in_model(ele_adr_primary,SIG_MD_G_ONOFF_C, 1,&model_idx, 0);
					if(p_model && p_model->pub_adr){
						pub_addr = p_model->pub_adr;
					}    			
					access_cmd_onoff(pub_addr, 0, G_OFF, CMD_NO_ACK, 0);
    			}else if (kb_event.keycode[0] == RC_KEY_2_OFF){
    			    p_model = (model_common_t *)mesh_find_ele_resource_in_model(ele_adr_primary+1,SIG_MD_G_ONOFF_C, 1,&model_idx, 0);
					if(p_model && p_model->pub_adr){
						pub_addr = p_model->pub_adr;
					} 
					else{
						pub_addr += 1;
					}
					access_cmd_onoff(pub_addr, 0, G_OFF, CMD_NO_ACK, 0);
    			}else if (kb_event.keycode[0] == RC_KEY_3_OFF){
    			    p_model = (model_common_t *)mesh_find_ele_resource_in_model(ele_adr_primary+2,SIG_MD_G_ONOFF_C, 1,&model_idx, 0);
					if(p_model && p_model->pub_adr){
						pub_addr = p_model->pub_adr;
					}
					else{
						pub_addr += 2;
					}
					access_cmd_onoff(pub_addr, 0, G_OFF, CMD_NO_ACK, 0);
    			}else if (kb_event.keycode[0] == RC_KEY_4_OFF){
    			    p_model = (model_common_t *)mesh_find_ele_resource_in_model(ele_adr_primary+3,SIG_MD_G_ONOFF_C, 1,&model_idx, 0);
					if(p_model && p_model->pub_adr){
						pub_addr = p_model->pub_adr;
					}
					else{
						pub_addr += 3;
					}
					access_cmd_onoff(pub_addr, 0, G_OFF, CMD_NO_ACK, 0);
    			}else if (kb_event.keycode[0] == RC_KEY_A_OFF){
    				// send all off cmd
					access_cmd_onoff(ADR_ALL_NODES, 0, G_OFF, CMD_NO_ACK, 0);
    			}else if (kb_event.keycode[0] == RC_KEY_UP){					
					last_lum += 20;
					if(last_lum > 100){
						last_lum = 100;
					}
					access_set_lum(ADR_ALL_NODES, 0, last_lum, 0);
    			}else if (kb_event.keycode[0] == RC_KEY_DN){
					last_lum = (last_lum>20)?(last_lum-20):1;
					access_set_lum(ADR_ALL_NODES, 0, last_lum, 0);
    			}else if (kb_event.keycode[0] == RC_KEY_L){
					last_tmp += 20;
					if(last_tmp > 100){
						last_tmp = 100;
					}
					access_cmd_set_light_ctl_temp_100(ADR_ALL_NODES, 0, last_tmp, 0);
    			}else if (kb_event.keycode[0] == RC_KEY_R){
					last_tmp = (last_tmp>20)?(last_tmp-20):1;
					access_cmd_set_light_ctl_temp_100(ADR_ALL_NODES, 0, last_tmp, 0);
    			}else{
    				// invalid key
    				memset4(&kb_event, 0, sizeof(kb_event));
					key_released = 0;
    				return;
    			}
				#else // old
		    	int report_flag = 1;
		        u8 cmd[11] = {0};
    			if (kb_event.keycode[0] == RC_KEY_1_ON){
    			    cmd[0] = LGT_CMD_LIGHT_ON;
    			}else if (kb_event.keycode[0] == RC_KEY_2_ON){
    			    cmd[0] = LGT_CMD_LIGHT_ON;
    			}else if (kb_event.keycode[0] == RC_KEY_3_ON){
    			    cmd[0] = LGT_CMD_LIGHT_ON;
    			}else if (kb_event.keycode[0] == RC_KEY_4_ON){
    			    cmd[0] = LGT_CMD_LIGHT_ON;
    			}else if (kb_event.keycode[0] == RC_KEY_A_ON){
					// send all on cmd 
					access_cmd_onoff(0xffff, 0, G_ON, CMD_NO_ACK, 0);					
					report_flag = 0;
    			}else if (kb_event.keycode[0] == RC_KEY_1_OFF){
    			    cmd[0] = LGT_CMD_LIGHT_OFF;
    			}else if (kb_event.keycode[0] == RC_KEY_2_OFF){
    			    cmd[0] = LGT_CMD_LIGHT_OFF;
    			}else if (kb_event.keycode[0] == RC_KEY_3_OFF){
    			    cmd[0] = LGT_CMD_LIGHT_OFF;
    			}else if (kb_event.keycode[0] == RC_KEY_4_OFF){
    			    cmd[0] = LGT_CMD_LIGHT_OFF;
    			}else if (kb_event.keycode[0] == RC_KEY_A_OFF){
    				// send all off cmd
					access_cmd_onoff(0xffff, 0, G_OFF, CMD_NO_ACK, 0);
					report_flag = 0;
    			}else if (kb_event.keycode[0] == RC_KEY_UP){
    			    cmd[0] = LGT_CMD_LUM_UP;
    			}else if (kb_event.keycode[0] == RC_KEY_DN){
    			    cmd[0] = LGT_CMD_LUM_DOWN;
    			}else if (kb_event.keycode[0] == RC_KEY_L){
    			    cmd[0] = LGT_CMD_LEFT_KEY;
    			}else if (kb_event.keycode[0] == RC_KEY_R){
    			    cmd[0] = LGT_CMD_RIGHT_KEY;
    			}else{
    				// invalid key
    				memset4(&kb_event, 0, sizeof(kb_event));
					key_released = 0;
    				return;
    			}

    			if(report_flag){
    				vd_cmd_key_report(ADR_ALL_NODES, kb_event.keycode[0]);
    			}
				#endif
                rf_link_light_event_callback(LGT_CMD_SWITCH_CMD);
			}
			key_released = 0;
		}
		///////////////////////////   key released  ///////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////
		else {
			rc_repeat_key = 0;
			key_released = 1;
		}

		tick_key_repeat = tick_key_pressed = clock_time ();
		kb_last[0] = kb_event.keycode[0];
		kb_last[1] = kb_event.keycode[1];

		//reg_usb_ep8_dat = kb_last[0];			//debug purpose, output to usb UART
	}
	//////////////////////////////////////////////////////////////////////////////////////////
	//				no key change event
	//////////////////////////////////////////////////////////////////////////////////////////
	else if (kb_last[0])
	{
		//	long pressed
		mesh_active_time = clock_time();
		if (clock_time_exceed(tick_key_pressed, 2000000))		// long pressed
		{
			 if ((kb_last[0] == RC_KEY_A_ON && kb_last[1] == RC_KEY_1_OFF) ||
                 (kb_last[1] == RC_KEY_A_ON && kb_last[0] == RC_KEY_1_OFF))
			 {
			 		if(SWITCH_MODE_NORMAL == switch_mode){
			 			switch_mode_set(SWITCH_MODE_GATT);
			 		}
			 }
			 if ((kb_last[0] == RC_KEY_A_ON && kb_last[1] == RC_KEY_4_OFF) ||
                 (kb_last[1] == RC_KEY_A_ON && kb_last[0] == RC_KEY_4_OFF))
			 {
			 	if(!del_node_tick){
			 		cfg_cmd_reset_node(ele_adr_primary);
			 	}
			 }
		}
		if (clock_time_exceed (tick_key_pressed, 500000))		//repeat key mode
		{
			if (kb_last[0] == VK_UP)
			{
				rc_repeat_key = LGT_CMD_LUM_UP;
			}
			else if (kb_last[0] == VK_DOWN)
			{
				rc_repeat_key = LGT_CMD_LUM_DOWN;
			}
			else if (kb_last[0] == VK_LEFT)
			{
				rc_repeat_key = LGT_CMD_LEFT_KEY;
			}
			else if (kb_last[0] == VK_RIGHT)
			{
				rc_repeat_key = LGT_CMD_RIGHT_KEY;
			}
		}

		if (!rc_long_pressed && clock_time_exceed(tick_key_pressed, 700000))
		{
			rc_long_pressed = 1;
			if (kb_last[0] == VK_1)		//group 1
			{
				
			}
		}

	}else{
		key_released = 1;
	}

	if (rc_repeat_key && clock_time_exceed (tick_key_repeat, ACTIVE_INTERVAL * TRANSMIT_CNT))	// should be longer than interval of command
	{
		tick_key_repeat = clock_time ();
		//tx_command_repeat ();
	}
	rc_key_pressed = kb_last[0];
	/////////////////////////////////////////////////////////////////////////////////
}
#endif

int soft_timer_key_scan()
{
	int ret = -1;
	mesh_proc_keyboard();
	return ret;
}

void switch_check_and_enter_sleep()
{
	if(blc_tlkEvent_pending || del_node_tick || is_busy_tx_segment_or_reliable_flow() || blt_soft_timer_cur_num()){
		// wait proc_node_reset 
	}
	else if(0 == my_fifo_data_cnt_get(&mesh_adv_cmd_fifo)){
		if((!scan_pin_need)){ 
			cfg_led_event_stop();
			led_onoff_gpio(GPIO_LED, 0);
			u16 wakeup_src = switch_provision_ok ? (PM_WAKEUP_PAD|PM_WAKEUP_TIMER):PM_WAKEUP_PAD;
			
			u32 sleep_s = clock_time_s()-switch_iv_updata_s; // switch_iv_updata_s is last update time
			if(sleep_s < SWITCH_LONG_SLEEP_TIME_S){
				sleep_s = SWITCH_LONG_SLEEP_TIME_S - sleep_s;
			}
			else{
				sleep_s = 1;
			}
			// user can set wakeup_tick of 32k rc by self. 
			cpu_long_sleep_wakeup(DEEPSLEEP_MODE_RET_SRAM_LOW32K, wakeup_src, sleep_s*1000*32);
		}
		else{
			if(!is_soft_timer_exist(&mesh_switch_send_mesh_adv) && !is_soft_timer_exist(&soft_timer_key_scan)){
				blt_soft_timer_add(&soft_timer_key_scan, 40000);
			}		
		}
	}
}

void proc_rc_ui_suspend()
{
    if(is_provision_success() && (!switch_provision_ok) && node_binding_tick){ // appkey binding
		if(clock_time_exceed(node_binding_tick, 3*1000*1000)){
			switch_provision_ok = 1;// provison and key bind finish
			gatt_adv_send_flag = 0;	
			switch_mode_set(SWITCH_MODE_NORMAL); 
		}
		else{
			return;
		}
    }

	if(blt_state== BLS_LINK_STATE_CONN || blcOta.ota_start_flag){
		ENABLE_SUSPEND_MASK;
		return ;
	}

	if(SWITCH_MODE_GATT == switch_mode){// led shining
		if(!is_led_busy()){
			if(switch_provision_ok){
	            rf_link_light_event_callback(LGT_CMD_SWITCH_POWERON);
			}
			else if(!is_provision_success() && !is_provision_working()){
	        	rf_link_light_event_callback(LGT_CMD_SWITCH_PROVISION);
	   	 	}
			proc_led();
		}
	}
	
	if(SWITCH_MODE_NORMAL == switch_mode){
		switch_check_and_enter_sleep();
	}
	else if(clock_time_exceed(switch_mode_tick, ADV_TIMEOUT*1000)){
		switch_mode_set(SWITCH_MODE_NORMAL);
		switch_check_and_enter_sleep();
	}	
}

void mesh_switch_init()
{
	mesh_tid.tx[0] = analog_read(REGA_TID);
    ////////// set up wakeup source: driver pin of keyboard  //////////
    #if UI_BUTTON_MODE_ENABLE
	cpu_set_gpio_wakeup (SW1_GPIO, 0, 1); 	// level : 1 (high); 0 (low)
	cpu_set_gpio_wakeup (SW2_GPIO, 0, 1); 	// level : 1 (high); 0 (low)
    #else
    u32 pin[] = KB_DRIVE_PINS;
    for (int i=0; i<sizeof (pin)/sizeof(u32); i++)
    {
        cpu_set_gpio_wakeup (pin[i], 1, 1);     // level : 1 (high); 0 (low)
    }
    #endif
}



int mesh_switch_send_mesh_adv()
{
	int ret = -1;	
	mesh_send_adv2scan_mode(1);
	if(my_fifo_get(&mesh_adv_cmd_fifo)){		
		ret = get_mesh_adv_interval();
	}
	return ret;
}
/////////////////////////////////////////////////////////////////////
// main loop flow
/////////////////////////////////////////////////////////////////////
void main_loop ()
{
	static u32 tick_loop;
	// LOG_MSG_LIB(TL_LOG_NODE_SDK, 0, 0, "m %d", tick_loop);

	tick_loop ++;
#if (BLT_SOFTWARE_TIMER_ENABLE)
	blt_soft_timer_process(MAINLOOP_ENTRY);
	if(BLS_LINK_STATE_ADV == blt_state){
		if(my_fifo_data_cnt_get(&mesh_adv_cmd_fifo)){
			if(!is_soft_timer_exist(&mesh_switch_send_mesh_adv)){
				blt_soft_timer_add(&mesh_switch_send_mesh_adv,get_mesh_adv_interval());	
			}
		}
	}
#endif
	#if DUAL_MODE_ADAPT_EN
	if(RF_MODE_BLE != dual_mode_proc()){    // should be before is mesh latency window()
        proc_ui();
        proc_led();
        factory_reset_cnt_check();
		return ;
	}
	#endif
	
	////////////////////////////////////// BLE entry /////////////////////////////////
	blt_sdk_main_loop ();


	////////////////////////////////////// UI entry /////////////////////////////////
	//  add spp UI task:
#if (BATT_CHECK_ENABLE)
    app_battery_power_check_and_sleep_handle(1);
#endif
	proc_ui();
	proc_led();
	
	mesh_loop_process();
#if ADC_ENABLE
	static u32 adc_check_time;
	if(clock_time_exceed(adc_check_time, 1000*1000)){
		adc_check_time = clock_time();
		static u16 T_adc_val;
		
		#if (BATT_CHECK_ENABLE)
		app_battery_check_and_re_init_user_adc();
		#endif
	#if(MCU_CORE_TYPE == MCU_CORE_8269)     
		T_adc_val = adc_BatteryValueGet();
	#else
		T_adc_val = adc_sample_and_get_result();
	#endif
	}  
#endif

	proc_rc_ui_suspend();
}

void user_init()
{
    #if (BATT_CHECK_ENABLE)
    app_battery_power_check_and_sleep_handle(0); //battery check must do before OTA relative operation
    #endif
	deep_wakeup_proc();// fast detect key when powerup
	switch_provision_ok = is_net_key_save();
	mesh_global_var_init();
	set_blc_hci_flag_fun(0);// disable the hci part of for the lib .
	proc_telink_mesh_to_sig_mesh();		// must at first
		
	#if (DUAL_MODE_ADAPT_EN)
	dual_mode_en_init();    // must before factory_reset_handle, because "dual_mode_state" is used in it.
	#endif
	blc_app_loadCustomizedParameters();  //load customized freq_offset cap value and tp value

	usb_id_init();
	usb_log_init ();
	usb_dp_pullup_en (1);  //open USB enum

	////////////////// BLE stack initialization ////////////////////////////////////
	ble_mac_init();    

	//link layer initialization
	//bls_ll_init (tbl_mac);
#if(MCU_CORE_TYPE == MCU_CORE_8269)
	blc_ll_initBasicMCU(tbl_mac);   //mandatory
#elif((MCU_CORE_TYPE == MCU_CORE_8258) || (MCU_CORE_TYPE == MCU_CORE_8278))
	blc_ll_initBasicMCU();                      //mandatory
	blc_ll_initStandby_module(tbl_mac);				//mandatory
#endif
	blc_ll_initAdvertising_module(tbl_mac); 	//adv module: 		 mandatory for BLE slave,
	blc_ll_initSlaveRole_module();				//slave module: 	 mandatory for BLE slave,
#if BLE_REMOTE_PM_ENABLE
	blc_ll_initPowerManagement_module();		//pm module:		 optional
	ENABLE_SUSPEND_MASK;
	blc_pm_setDeepsleepRetentionThreshold(50, 30); // threshold to enter retention
	blc_pm_setDeepsleepRetentionEarlyWakeupTiming(400); // retention early wakeup time		
#else
	bls_pm_setSuspendMask (SUSPEND_DISABLE);
#endif

	//l2cap initialization
	//blc_l2cap_register_handler (blc_l2cap_packet_receive);
	blc_l2cap_register_handler (app_l2cap_packet_receive);
	
	///////////////////// USER application initialization ///////////////////

	u8 status = bls_ll_setAdvParam( ADV_INTERVAL_MIN, ADV_INTERVAL_MAX, \
			 	 	 	 	 	     ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
			 	 	 	 	 	     0,  NULL,  BLT_ENABLE_ADV_ALL, ADV_FP_NONE);

	if(status != BLE_SUCCESS){  //adv setting err
		write_reg8(0x8000, 0x11);  //debug
		while(1);
	}
	
	// normally use this settings 
	blc_ll_setAdvCustomedChannel (37, 38, 39);
	bls_ll_setAdvEnable(1);  //adv enable

	rf_set_power_level_index (my_rf_power_index);
    blc_hci_le_setEventMask_cmd(HCI_LE_EVT_MASK_ADVERTISING_REPORT|HCI_LE_EVT_MASK_CONNECTION_COMPLETE);

	////////////////// SPP initialization ///////////////////////////////////
#if (HCI_ACCESS != HCI_USE_NONE)
	#if (HCI_ACCESS==HCI_USE_USB)
	//blt_set_bluetooth_version (BLUETOOTH_VER_4_2);
	//bls_ll_setAdvChannelMap (BLT_ENABLE_ADV_ALL);
	usb_bulk_drv_init (0);
	blc_register_hci_handler (app_hci_cmd_from_usb, blc_hci_tx_to_usb);
	#elif (HCI_ACCESS == HCI_USE_UART)	//uart
	uart_drv_init();
	blc_register_hci_handler (blc_rx_from_uart, blc_hci_tx_to_uart);		//default handler
	//blc_register_hci_handler(rx_from_uart_cb,tx_to_uart_cb);				//customized uart handler
	#endif
#endif
#if ADC_ENABLE
	adc_drv_init();	// still init even though BATT_CHECK_ENABLE is enable, beause battery check may not be called in user init.
#endif
	rf_pa_init();
	bls_app_registerEventCallback (BLT_EV_FLAG_CONNECT, (blt_event_callback_t)&mesh_ble_connect_cb);
	blc_hci_registerControllerEventHandler(app_event_handler);		//register event callback
	//bls_hci_mod_setEventMask_cmd(0xffff);			//enable all 15 events,event list see ble_ll.h
	bls_set_advertise_prepare (app_advertise_prepare_handler);
	//bls_set_update_chn_cb(chn_conn_update_dispatch);
	bls_ota_registerStartCmdCb(entry_ota_mode);
	bls_ota_registerResultIndicateCb(show_ota_result);

	// mesh_mode and layer init
	mesh_init_all();

	// OTA init
	#if (DUAL_MODE_ADAPT_EN && (0 == FW_START_BY_BOOTLOADER_EN) || DUAL_MODE_WITH_TLK_MESH_EN)
	if(DUAL_MODE_NOT_SUPPORT == dual_mode_state)
	#endif
	{bls_ota_clearNewFwDataArea(0);	 //must
	}
	mesh_switch_init();
	//blc_ll_initScanning_module(tbl_mac);
	#if((MCU_CORE_TYPE == MCU_CORE_8258) || (MCU_CORE_TYPE == MCU_CORE_8278))
	blc_gap_peripheral_init();    //gap initialization
	#endif
	
	mesh_scan_rsp_init();
	my_att_init (provision_mag.gatt_mode);
	blc_att_setServerDataPendingTime_upon_ClientCmd(10);
#if (BLT_SOFTWARE_TIMER_ENABLE)
	blt_soft_timer_init();
	//blt_soft_timer_add(&soft_timer_test0, 1*1000*1000);
#endif
	/// LOG_MSG_LIB(TL_LOG_NODE_SDK, 0, 0, "init", 0);
}

#if (PM_DEEPSLEEP_RETENTION_ENABLE)
extern u32 blt_advExpectTime;
_attribute_ram_code_ void user_init_deepRetn(void)
{
    blc_app_loadCustomizedParameters();
	blc_ll_initBasicMCU();   //mandatory
	rf_set_power_level_index (my_rf_power_index);

	blc_ll_recoverDeepRetention();	
	if(blt_state == BLS_LINK_STATE_ADV){
		if(!(((blt_advExpectTime-clock_time())<blta.adv_interval) || ((clock_time()-blt_advExpectTime)<blta.adv_interval))){	
			blt_advExpectTime = clock_time();
		}
	}
	// should enable IRQ here, because it may use irq here, for example BLE connect.
    irq_enable();
	if(!iv_idx_st.update_proc_flag){
		iv_idx_st.searching_flag = 1;// always in search mode to process security beacon
	}
	deep_wakeup_proc();// fast detect key when powerup
	DBG_CHN0_HIGH;    //debug
#if (HCI_ACCESS == HCI_USE_UART)	//uart
	uart_drv_init();
#endif
#if ADC_ENABLE
	adc_drv_init();
#endif

    mesh_switch_init();    
	// LOG_MSG_LIB(TL_LOG_NODE_SDK, 0, 0, "init ret", 0);
}

#if (0 == UI_BUTTON_MODE_ENABLE)
void global_reset_new_key_wakeup()
{
    rc_key_pressed = 0;
    rc_long_pressed = rc_repeat_key = 0;
    memset(&kb_event, 0, sizeof(kb_event));
    global_var_no_ret_init_kb();
}
#endif

#endif

void switch_mode_set(int mode)
{
    if(SWITCH_MODE_GATT == mode){
        switch_mode = SWITCH_MODE_GATT;
		gatt_adv_send_flag = 1;
		#if SWITCH_PB_ADV_EN
		beacon_str_init();
		if(0 == switch_provision_ok){
			app_enable_scan_all_device (); // support pb adv
			bls_pm_setSuspendMask (SUSPEND_DISABLE); 
		}
		#endif
        switch_mode_tick = clock_time();
		LOG_MSG_INFO(TL_LOG_NODE_SDK,0, 0,"enter GATT mode",0);
    }else if(SWITCH_MODE_NORMAL == mode){
		blc_ll_setScanEnable (0, 0);// disable scan to save power
		beacon_send.en = 0;
        switch_mode = SWITCH_MODE_NORMAL;
        switch_mode_tick = 0;
		gatt_adv_send_flag = 0;
        cfg_led_event_stop();
		#if SWITCH_PB_ADV_EN
		ENABLE_SUSPEND_MASK;
		#endif
		LOG_MSG_INFO(TL_LOG_NODE_SDK,0, 0,"exit GATT mode",0);
    }
}

