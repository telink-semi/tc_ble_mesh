/********************************************************************************************************
 * @file    app_ui.c
 *
 * @brief   This is the source file for BLE SDK
 *
 * @author  BLE GROUP
 * @date    06,2022
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#include "app_ui.h"
#include "drivers.h"
#if(MCU_CORE_TYPE == MCU_CORE_8278)
#include "stack/ble_8278/ble.h"
#else
#include "stack/ble/ble.h"
#endif
#include "proj_lib/sig_mesh/app_mesh.h"
#if !__TLSR_RISCV_EN__
#include "proj/drivers/keyboard.h"
#endif
#include "vendor/common/blt_soft_timer.h"

#define SLEEP_MAX_S						(32*60*60)	// max 37 hours,but use 32 to be multiplied of SWITCH_IV_SEARCHING_INVL_S
#if IV_UPDATE_TEST_EN
#define SWITCH_IV_SEARCHING_INTERVLAL_S	(IV_UPDATE_KEEP_TMIE_MIN_RX_S)
#else
#define SWITCH_IV_SEARCHING_INTERVLAL_S	(96*60*60)	// keep searching for SWITCH_IV_RCV_WINDOW_S(10s default)
#endif
#define SWITCH_IV_SEARCHING_INVL_S		(SWITCH_IV_SEARCHING_INTERVLAL_S - 1)	// -1 to make sure "clock time exceed" is true when time is just expired.
#define SWITCH_LONG_SLEEP_TIME_S		(min(SLEEP_MAX_S, SWITCH_IV_SEARCHING_INTERVLAL_S))
#define SWITCH_IV_RCV_WINDOW_S			(10)

u8 switch_mode = SWITCH_MODE_NORMAL;
u32 switch_mode_tick = 0;
u8 switch_provision_ok = 0;
u32 switch_iv_updata_s = 0;

STATIC_ASSERT(SWITCH_IV_SEARCHING_INTERVLAL_S % SWITCH_LONG_SLEEP_TIME_S == 0); // must be multiplied.

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

void switch_trigger_iv_search_mode(int force)
{	
	if(force || clock_time_exceed_s(switch_iv_updata_s, SWITCH_IV_SEARCHING_INVL_S)){
		LOG_MSG_INFO(TL_LOG_IV_UPDATE,0, 0,"switch enter iv update search mode time_s:%d", clock_time_s());
		switch_iv_update_time_refresh();		
		app_enable_scan_all_device ();
		bls_pm_setSuspendMask (SUSPEND_DISABLE);
		mesh_beacon_poll_1s();
		blt_soft_timer_add(&soft_timer_rcv_beacon_timeout, SWITCH_IV_RCV_WINDOW_S*1000*1000);
		mesh_send_adv2scan_mode(0);
	}
}

u16 sw_tx_src_addr_offset = 0;

void switch_send_publish_command(u32 ele_offset, bool4 onoff, u32 select_pub_model_key)
{
#if (LIGHT_TYPE_SEL == LIGHT_TYPE_NLC_CTRL_CLIENT)
	dicmp_switch_send_publish_command(ele_offset, onoff, select_pub_model_key);
#else
	u32 pub_model_id_sig = SIG_MD_G_ONOFF_C; // always send generic onoff command
	u16 pub_addr = SWITCH_GROUP_ADDR_START;
	model_common_t *p_model;
	u8 model_idx = 0;
	p_model = (model_common_t *)mesh_find_ele_resource_in_model(ele_adr_primary + ele_offset, pub_model_id_sig, 1, &model_idx, 0);
	if(p_model && p_model->pub_adr){
		pub_addr = p_model->pub_adr;
	}else{
		pub_addr += ele_offset;
	}

	// sw_tx_src_addr_offset = ele_offset; // disable as default to decrease cache occupied in other nodes.
	access_cmd_onoff(pub_addr, 0, onoff ? G_ON : G_OFF, CMD_NO_ACK, 0);
#endif
}

void proc_ui()
{
#if UI_KEYBOARD_ENABLE
	mesh_proc_keyboard(0, 0, 0);
#endif
}

#define		ADV_TIMEOUT					(30*1000) //30s

#if UI_KEYBOARD_ENABLE
static int	rc_repeat_key = 0;
static int	rc_key_pressed;
static int	rc_long_pressed;
static u32  mesh_active_time;
u8   key_released =1;
#define		ACTIVE_INTERVAL				32000

#define	KEY_SCAN_WAKEUP_INTERVAL		(40 * 1000)	// unit:us. wakeup interval to scan key.
#define GPIO_WAKEUP_KEYPROC_CNT			3

/**
 * @brief       This function server to wakeup to scan key when key press.
 * @return      0: keep same soft timer. -1: delete soft timer.
 * @note        
 */
int soft_timer_key_scan()
{
	return key_released ? -1 : 0;
}

/**
 * @brief       This function servers to quickly scan keyboard after wakeup and hold this data to the cache.
 * @param[in]   none 
 * @return      none
 * @note        
 */
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

/**
 * @brief       This function servers to process keyboard event from deep_wakeup_proc().
 * @param[io]   *det_key - detect key flag. 0: no key detect, use deepback cache if exist. other: key had been detected.
 * @return      none
 * @note        
 */
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

/**
 * @brief       This function servers to process keyboard event.
 * @param[in]   none
 * @return      none
 * @note        
 */
void mesh_proc_keyboard(u8 e, u8 *p, int n)
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
		#if (!KB_LINE_MODE)
		if(key_released && (SWITCH_MODE_GATT == switch_mode)){
			switch_mode_set(SWITCH_MODE_NORMAL); // press any key can exit adv mode
		}
		#endif
		
		if(kb_event.cnt)
		{
			// key was detected pressed. MCU run the code here one time for one press action.  
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
			    	#if KB_LINE_MODE
					if (kb_event.keycode[0] == KEY_CMD){
						static u8 sw_onoff_cnt;
						u8 sw_onoff_flag = (sw_onoff_cnt++) & 1;	// light OFF first
						access_cmd_onoff(ADR_ALL_NODES, 0, sw_onoff_flag, CMD_NO_ACK, 0);
					}
					#else
					static u8 last_lum = 100;
					static u8 last_tmp = 100;
					
					static u8 select_pub_model_key_last = RC_KEY_A_ON;
					u8 key_pressed = kb_event.keycode[0];
					if((RC_KEY_A_ON == key_pressed)||(RC_KEY_A_OFF == key_pressed) ||
						(RC_KEY_UP == key_pressed)||(RC_KEY_DN == key_pressed) ||
						(RC_KEY_L == key_pressed)||(RC_KEY_R == key_pressed)){
						select_pub_model_key_last = key_pressed;
					}
					
	    			if (kb_event.keycode[0] == RC_KEY_1_ON){	
						switch_send_publish_command(0, G_ON, select_pub_model_key_last);
	    			}else if (kb_event.keycode[0] == RC_KEY_2_ON){
						switch_send_publish_command(1, G_ON, select_pub_model_key_last);
	    			}else if (kb_event.keycode[0] == RC_KEY_3_ON){
						switch_send_publish_command(2, G_ON, select_pub_model_key_last);
	    			}else if (kb_event.keycode[0] == RC_KEY_4_ON){
						switch_send_publish_command(3, G_ON, select_pub_model_key_last);
	    			}else if (kb_event.keycode[0] == RC_KEY_A_ON){
						access_cmd_onoff(ADR_ALL_NODES, 0, G_ON, CMD_NO_ACK, 0);
	    			}else if (kb_event.keycode[0] == RC_KEY_1_OFF){
						switch_send_publish_command(0, G_OFF, select_pub_model_key_last);
	    			}else if (kb_event.keycode[0] == RC_KEY_2_OFF){
						switch_send_publish_command(1, G_OFF, select_pub_model_key_last);
	    			}else if (kb_event.keycode[0] == RC_KEY_3_OFF){
						switch_send_publish_command(2, G_OFF, select_pub_model_key_last);
	    			}else if (kb_event.keycode[0] == RC_KEY_4_OFF){
						switch_send_publish_command(3, G_OFF, select_pub_model_key_last);
	    			}else if (kb_event.keycode[0] == RC_KEY_A_OFF){
	    				// send all off cmd
						access_cmd_onoff(ADR_ALL_NODES, 0, G_OFF, CMD_NO_ACK, 0);
					#if (LIGHT_TYPE_SEL == LIGHT_TYPE_NLC_CTRL_CLIENT)
					// no message to send, just to set select_pub_model_key_last which has been set before
					#else
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
						last_tmp = (last_tmp>20)?(last_tmp-20):0;
						access_cmd_set_light_ctl_temp_100(ADR_ALL_NODES, 0, last_tmp, 0);
	    			}else if (kb_event.keycode[0] == RC_KEY_R){
						last_tmp += 20;
						if(last_tmp > 100){
							last_tmp = 100;
						}
						access_cmd_set_light_ctl_temp_100(ADR_ALL_NODES, 0, last_tmp, 0);
					#endif
	    			}else{
	    				// invalid key
	    				memset4(&kb_event, 0, sizeof(kb_event));
						key_released = 0;
	    				return;
	    			}
					#endif
	                rf_link_light_event_callback(LGT_CMD_SWITCH_CMD);
				}
				key_released = 0;
				// LOG_USER_MSG_INFO(0, 0, "key press:%d", kb_event.keycode[0]);
			}
			
			if(kb_event.cnt == 1){
				#if IV_UPDATE_TEST_EN
				mesh_iv_update_test_initiate(kb_event.keycode[0]);
				#endif
			}
		}
		///////////////////////////   key released  ///////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////
		else {
			// key was released . MCU run the code here one time for one release action.  
			#if KB_LINE_MODE
			if(!del_node_tick && (kb_last[0] == KEY_RESET)){ // reuse reset button in keyboard line mode, short press enter/exit adv mode
				if(SWITCH_MODE_NORMAL == switch_mode){
					switch_mode_set(SWITCH_MODE_GATT);
				}
				else{
					switch_mode_set(SWITCH_MODE_NORMAL);
				}
			}
			#endif
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
		//	long pressed // key was detected in a continuously pressed state. for each main_loop, MCU run the code here until the key is released.  
		mesh_active_time = clock_time();
		if (clock_time_exceed(tick_key_pressed, 2000000))	// long pressed // 2000000 is the threshold for long press detection
		{
			 if ((kb_last[0] == RC_KEY_A_ON && kb_last[1] == RC_KEY_1_OFF) ||
                 (kb_last[1] == RC_KEY_A_ON && kb_last[0] == RC_KEY_1_OFF))
			 {
			 		if(SWITCH_MODE_NORMAL == switch_mode){  // long pressed event
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

		#if KB_LINE_MODE
		if (clock_time_exceed(tick_key_pressed, 3*1000*1000))
		{	
			if (kb_last[0] == KEY_RESET)
			{
				if(!del_node_tick){
					cfg_cmd_reset_node(ele_adr_primary);
				}
			}
		}
		#endif
	}else{
		// no key was detected.	
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

void switch_check_and_enter_sleep()
{
#if BLE_REMOTE_PM_ENABLE
	if(my_fifo_data_cnt_get(&mesh_adv_cmd_fifo) || del_node_tick || is_busy_tx_segment_or_reliable_flow() || blt_soft_timer_cur_num()
		#if BLE_MULTIPLE_CONNECTION_ENABLE
		|| is_ble_event_pending() 
		#else
		|| blc_tlkEvent_pending
		#endif
	){
		#if BLE_MULTIPLE_CONNECTION_ENABLE
		blc_ll_setAdvEnable(BLC_ADV_ENABLE);
		#endif
		// wait pending event 
	}
	else{
		if(key_released){
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
			#if BLE_MULTIPLE_CONNECTION_ENABLE
			blc_ll_setAdvEnable(BLC_ADV_DISABLE);
			if(blc_ll_isBleTaskIdle())
			#endif
			{
				cpu_long_sleep_wakeup(RETENTION_RAM_SIZE_USE, wakeup_src, sleep_s*1000*32);
			}
		}
	}
#endif
}

void proc_rc_ui_suspend()
{
    if(is_provision_success() && (!switch_provision_ok) && node_binding_tick){ // appkey binding
		if(clock_time_exceed(node_binding_tick, 3*1000*1000)){
			switch_provision_ok = 1;// provison and key bind finish
			#if !SWITCH_ALWAYS_MODE_GATT_EN
			gatt_adv_send_flag = 0;	
			#endif
			switch_mode_set(SWITCH_MODE_NORMAL); 
		}
		else{
			return;
		}
    }

	if(blc_ll_getCurrentState()== BLS_LINK_STATE_CONN || is_tlk_gatt_ota_busy()){
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
	// mesh_tid.tx[0] = analog_read(REGA_TID);
    ////////// set up wakeup source: driver pin of keyboard  //////////
#if (UI_KEYBOARD_ENABLE)
	ui_keyboard_wakeup_io_init();
#endif
}

int mesh_switch_send_mesh_adv()
{
	int ret = -1;	
	mesh_send_adv2scan_mode(1);
	if(my_fifo_get(&mesh_adv_cmd_fifo)){		
		ret = get_mesh_adv_interval();
	}
	// LOG_MSG_LIB(TL_LOG_NODE_BASIC,0,0,"send adv interval:%d",ret);
	return ret;
}

void global_reset_new_key_wakeup()
{
    rc_key_pressed = 0;
    rc_long_pressed = rc_repeat_key = 0;
    memset(&kb_event, 0, sizeof(kb_event));
    global_var_no_ret_init_kb();
}

void switch_mode_set(int mode)
{
#if SWITCH_ALWAYS_MODE_GATT_EN
	if(SWITCH_MODE_GATT == switch_mode){
		return ;
	}
#endif

    if(SWITCH_ALWAYS_MODE_GATT_EN || (SWITCH_MODE_GATT == mode)){
        switch_mode = SWITCH_MODE_GATT;
		gatt_adv_send_flag = 1;
		#if (SWITCH_PB_ADV_EN || SWITCH_ALWAYS_MODE_GATT_EN)
		beacon_str_init();
		if(0 == switch_provision_ok || SWITCH_ALWAYS_MODE_GATT_EN){
			app_enable_scan_all_device (); // support pb adv
			bls_pm_setSuspendMask (SUSPEND_DISABLE); 
		}
		#endif
        switch_mode_tick = clock_time();
		
		#if BLE_MULTIPLE_CONNECTION_ENABLE
		blc_ll_setAdvEnable(BLC_ADV_ENABLE);
		#endif
		LOG_MSG_INFO(TL_LOG_NODE_SDK,0, 0,"enter GATT mode", 0);
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
		LOG_MSG_INFO(TL_LOG_NODE_SDK,0, 0,"exit GATT mode", 0);
    }
}
