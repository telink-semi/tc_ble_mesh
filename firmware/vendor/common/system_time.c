/********************************************************************************************************
 * @file     system_time.c 
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
#include "proj/tl_common.h"
#ifndef WIN32
#include "proj/mcu/watchdog_i.h"
#endif 
#include "proj_lib/ble/ll/ll.h"
#include "proj_lib/ble/blt_config.h"
#include "vendor/common/user_config.h"
#include "app_health.h"
#include "proj_lib/sig_mesh/app_mesh.h"
#include "vendor/common/time_model.h"
#include "time_model.h"
#include "lighting_model_LC.h"
#include "sensors_model.h"
#include "system_time.h"
#include "fast_provision_model.h"
#include "app_heartbeat.h"
#include "directed_forwarding.h"

#if ALI_MD_TIME_EN
#include "user_ali_time.h"
#endif

#ifndef __PROJECT_MESH_SWITCH__
#define __PROJECT_MESH_SWITCH__     0
#endif

#if WIN32 // just for compile
u32 flash_sector_mac_address = 0x76000;
u32 flash_sector_calibration = 0x77000;
#endif

u32 system_time_ms = 0;
u32 system_time_100ms = 0;
u32 system_time_s = 0;
u32 system_time_tick;

#ifndef RTC_USE_32K_RC_ENABLE
#define RTC_USE_32K_RC_ENABLE		0 // enable should be better when pm enable
#endif

#if RTC_USE_32K_RC_ENABLE
#define CHECK_INTERVAL      (500*CLOCK_16M_SYS_TIMER_CLK_1MS)
#else
#define CHECK_INTERVAL      (1 * CLOCK_SYS_CLOCK_1MS)
#endif

#define RTC_LEFT_MS 	(system_time_ms%1000+(tick_32k-tick_32k_begin)/32)
#define LOG_RTC_DEBUG(pbuf,len,format,...)		//LOG_USER_MSG_INFO(pbuf,len,format,__VA_ARGS__)

#if RTC_USE_32K_RC_ENABLE
u32 cal_unit_32k;
u32 cal_unit_16m;
u32 tick_16m_begin;
u32 tick_32k_begin;
u32 tick_32k_to_16m = 0;
u8 rtc_adjust_flag = 0;
_attribute_ram_code_  void read_tick_32k_16m(u8 *tick_32k, u32 * tick_16m)
{
	u8 r=irq_disable();
	u8 pre = analog_read(0x40);
	while( pre == analog_read(0x40) );
	(*tick_16m) = clock_time();			
	tick_32k[0] = analog_read(0x40);
	tick_32k[1] = analog_read(0x41);
	tick_32k[2] = analog_read(0x42);
	tick_32k[3] = analog_read(0x43);
	irq_restore(r);
	return;
}

void rtc_cal_init(u8 tick_start)
{
	u32 tmp_32k_1, tmp_32k_2;
	u32 tmp_16m_1, tmp_16m_2;
	read_tick_32k_16m((u8 *)&tmp_32k_1, (u32 *)&tmp_16m_1);
	sleep_us(512*1000);
	read_tick_32k_16m((u8 *)&tmp_32k_2, (u32 *)&tmp_16m_2);

	cal_unit_32k = tmp_32k_2 - tmp_32k_1;
	cal_unit_16m = tmp_16m_2 - tmp_16m_1;
	
	if(tick_start){
		tick_32k_begin = tmp_32k_1;
		tick_16m_begin = tick_32k_to_16m = tmp_16m_1;
	}
	LOG_RTC_DEBUG(0, 0, "rtc_cal_init", 0);
}
#endif

void system_timer_handle_ms()
{
#if MD_SERVER_EN
	light_transition_proc();
#endif
}

void system_timer_handle_100ms()
{
    if(!is_lpn_support_and_en){
        #if (!__PROJECT_MESH_SWITCH__)
        mesh_beacon_poll_100ms();
        #endif
	}
	mesh_heartbeat_poll_100ms();
#if ALI_MD_TIME_EN
	user_ali_time_proc();
#endif
#if !WIN32 && SENSOR_LIGHTING_CTRL_EN
    sensor_lighting_ctrl_proc();
#endif
#if (MD_DF_EN && MD_SERVER_EN && !WIN32 && !FEATURE_LOWPOWER_EN)
	mesh_directed_forwarding_proc(0, 0, 0, MESH_BEAR_ADV);
#endif
}

void system_time_init(){
	system_time_tick = clock_time();
#if RTC_USE_32K_RC_ENABLE
	LOG_RTC_DEBUG(0, 0, "system_time_init", 0);
	rtc_cal_init(1);							
#endif	
}

#if (FEATURE_LOWPOWER_EN || GATT_LPN_EN)
_attribute_ram_code_
#endif
void system_time_run(){
    mesh_iv_update_start_poll();
    
    u32 clock_tmp = clock_time();
    u32 t_delta = (u32)(clock_tmp - system_time_tick);
	#if RTC_USE_32K_RC_ENABLE
	if(t_delta < BIT(31))
	#else
    if(t_delta >= CHECK_INTERVAL)
	#endif
	{
		#if RTC_USE_32K_RC_ENABLE 
		u32 tick_32k, tick_16m;
		read_tick_32k_16m((u8 *)&tick_32k, (u32 *)&tick_16m);
		u32 unit_cnt = (u32)(tick_32k - tick_32k_begin)/cal_unit_32k;
		tick_32k_begin += unit_cnt*cal_unit_32k;
		tick_16m_begin += unit_cnt*cal_unit_16m;
		u32 delta = tick_16m_begin - tick_32k_to_16m;
		u32 interval_cnt = delta/CLOCK_SYS_CLOCK_1MS;
		if(interval_cnt){		
			tick_32k_to_16m += interval_cnt*CLOCK_SYS_CLOCK_1MS;
			if(rtc_adjust_flag){
				rtc_adjust_flag = 0;
				interval_cnt -= 13;// minus 13ms per minute				
			}
		}
		#else
        u32 interval_cnt = t_delta/CHECK_INTERVAL;
		#endif
        if(interval_cnt){
            u32 inc_100ms = (system_time_ms % 100 + interval_cnt) / 100;
			system_time_ms += interval_cnt;
						
			if(!(RTC_USE_32K_RC_ENABLE || FEATURE_LOWPOWER_EN || SPIRIT_PRIVATE_LPN_EN || GATT_LPN_EN || __PROJECT_MESH_SWITCH__||DU_LPN_EN)){ // (!is_lpn_support_and_en){ // it will cost several tens ms from wake up
                #if (PM_DEEPSLEEP_RETENTION_ENABLE)
                // no need to run system_timer_handle_ms_,
                // and it will take too much time to run system_timer_handle_ms_() for low power device which is wakeup after a long time sleep.
                #else
			    foreach(i,interval_cnt){
			        system_timer_handle_ms();
			    }
			    #endif
			}
			
			if(inc_100ms){
                u32 inc_s = (system_time_100ms % 10 + inc_100ms) / 10;
    			system_time_100ms += inc_100ms;
				
                if(RTC_USE_32K_RC_ENABLE || is_lpn_support_and_en || SPIRIT_PRIVATE_LPN_EN || __PROJECT_MESH_SWITCH__||DU_LPN_EN){ // it will cost several ms from wake up
                    system_timer_handle_100ms();	// only run once to save time
				}else{
					foreach(i,inc_100ms){
				        system_timer_handle_100ms();
				    }
				}
				
    			if(inc_s){		
					#if RTC_USE_32K_RC_ENABLE
					u32 now_s = system_time_s+inc_s;
					LOG_RTC_DEBUG(0, 0, "%02d:%02d:%02d.%03d", (now_s)/60/60, (now_s/60)%60, now_s%60+RTC_LEFT_MS/1000, RTC_LEFT_MS%1000);
					#endif
					foreach(i,inc_s){
						system_time_s++;
                        mesh_iv_update_st_poll_s();
						user_system_time_proc();
						#if VC_APP_ENABLE
						void sys_timer_refresh_time_ui();
						sys_timer_refresh_time_ui();
						#endif
						#if RTC_USE_32K_RC_ENABLE
						if((system_time_s%60) == 0){
							rtc_adjust_flag = 1;
							if((system_time_s%1800) == 0){
								rtc_cal_init(0);
							}
						}
						#endif
				    }					
    			}
            }

		}
		#if RTC_USE_32K_RC_ENABLE 
		system_time_tick = clock_time() + CHECK_INTERVAL;
		#else
        system_time_tick += interval_cnt * CHECK_INTERVAL;
		#endif
        // proc handle 1ms or greater
        light_par_save_proc();
	
	#if FAST_PROVISION_ENABLE
		mesh_fast_prov_proc();
	#endif
    #if MD_SERVER_EN
        #if MD_TIME_EN
        mesh_time_proc();
        #endif
	    #if (MD_LIGHT_CONTROL_EN)
        LC_property_proc();
	    #endif
	    #if (MD_SCENE_EN)
        scene_status_change_check_all();
	    #endif
        #if ONLINE_STATUS_EN
        online_st_proc();
        #endif

        #if MD_SERVER_EN
	    if(publish_powerup_random_ms && clock_time_exceed_ms(0, publish_powerup_random_ms)){
	        publish_powerup_random_ms = 0;
			publish_when_powerup();
	    }
	    #endif
    #endif
    }

#if MD_SERVER_EN
    #if(MD_SENSOR_SERVER_EN)
	sensor_measure_proc();
	#endif
#endif
}

u32 clock_time_exceed_ms(u32 ref, u32 span_ms){
	return ((u32)(clock_time_ms() - ref) > span_ms);
}

u32 clock_time_exceed_100ms(u32 ref, u32 span_100ms){
	return ((u32)(clock_time_100ms() - ref) > span_100ms);
}

u32 clock_time_exceed_s(u32 ref, u32 span_s){
	return ((u32)(clock_time_s() - ref) > span_s);
}

