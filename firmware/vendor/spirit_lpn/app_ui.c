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
#include "drivers.h"
#if(MCU_CORE_TYPE == MCU_CORE_8278)
#include "stack/ble_8278/ble.h"
#else
#include "stack/ble/ble.h"
#endif
#include "app.h"
#include "app_ui.h"
#include "proj_lib/sig_mesh/app_mesh.h"
#include "vendor/common/subnet_bridge.h"
#include "vendor/common/blt_soft_timer.h"
#if (UI_KEYBOARD_ENABLE)
static int	long_pressed;
u8   key_released =1;

#define	KEY_SCAN_WAKEUP_INTERVAL		(40 * 1000)	// unit:us. wakeup interval to scan key.
#define GPIO_WAKEUP_KEYPROC_CNT			3

void spirit_lpn_wakeup_init(u8 e, u8 *p, int n)
{
	if(get_blt_busy()){
		mesh_sleep_time.appWakeup_flg = 0;
		mesh_sleep_time.last_tick = clock_time()|1;
		bls_pm_setSuspendMask (SUSPEND_DISABLE);
		app_enable_scan_all_device();
	}
	else{ // early wakeup, not need scan.
		mesh_sleep_time.appWakeup_flg = 1;
		blc_ll_setScanEnable (0, 0); 
	}
}

void spirit_lpn_ui_init()
{
#if (UI_KEYBOARD_ENABLE)
	ui_keyboard_wakeup_io_init();
#endif
}

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
	if(kb_scan_key (KB_NUMLOCK_STATUS_POWERON, 1)){
		deepback_key_state = DEEPBACK_KEY_CACHE;
		memcpy(&kb_event_cache,&kb_event,sizeof(kb_event));
	}
#endif

	return;
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
		#if 0 
			if(KEY_SW2 == kb_event.keycode[0]){
				static u8 onoff=1;
				onoff = !onoff;
				access_cmd_onoff(0xffff, 0, onoff, CMD_NO_ACK, 0);
			}
		#endif
		}
		///////////////////////////   key released	///////////////////////////////////////
		else {
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
		}

	}else{
		key_released = 1;
		long_pressed = 0;
	}

	return;
}
#endif

void proc_ui()
{
#if (UI_KEYBOARD_ENABLE)
	mesh_proc_keyboard(0, 0, 0);
#endif

	return;
}

