/********************************************************************************************************
 * @file	blt_led.c
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

#include "../common/blt_led.h"

device_led_t device_led;


void device_led_on_off(u8 on)
{
	gpio_write( device_led.gpio_led, on^device_led.polar );
	gpio_set_output_en(device_led.gpio_led,on);
	device_led.isOn = on;
}

void device_led_init(u32 gpio,u8 polarity){  //polarity: 1 for high led on, 0 for low led on
	device_led.gpio_led = gpio;
	device_led.polar = !polarity;
    gpio_set_func(device_led.gpio_led,AS_GPIO);
    gpio_set_input_en(device_led.gpio_led,0);
    gpio_set_output_en(device_led.gpio_led,0);

    device_led_on_off(0);
}

int device_led_setup(led_cfg_t led_cfg)
{

	if( device_led.repeatCount &&  device_led.priority >= led_cfg.priority){
		return 0; //new led event priority not higher than the not ongoing one
	}
	else{
		device_led.onTime_ms = led_cfg.onTime_ms;
		device_led.offTime_ms = led_cfg.offTime_ms;
		device_led.repeatCount = led_cfg.repeatCount;
		device_led.priority = led_cfg.priority;

        if(led_cfg.repeatCount == 0xff){ //for long on/long off
        	device_led.repeatCount = 0;
        }
        else{ //process one of on/off Time is zero situation
        	if(!device_led.onTime_ms){  //onTime is zero
        		device_led.offTime_ms *= device_led.repeatCount;
        		device_led.repeatCount = 1;
        	}
        	else if(!device_led.offTime_ms){
        		device_led.onTime_ms *= device_led.repeatCount;
        	    device_led.repeatCount = 1;
        	}
        }

        device_led.startTick = clock_time();
        device_led_on_off(device_led.onTime_ms ? 1 : 0);

		return 1;
	}
}


void led_proc(void)
{
	if(device_led.isOn){
		if(clock_time_exceed(device_led.startTick,(device_led.onTime_ms-5)*1000)){
			device_led_on_off(0);
			if(device_led.offTime_ms){ //offTime not zero
				device_led.startTick += device_led.onTime_ms*CLOCK_SYS_CLOCK_1MS;
			}
			else{
				device_led.repeatCount = 0;
			}
		}
	}
	else{
		if(clock_time_exceed(device_led.startTick,(device_led.offTime_ms-5)*1000)){
			if(--device_led.repeatCount){
				device_led_on_off(1);
				device_led.startTick += device_led.offTime_ms*CLOCK_SYS_CLOCK_1MS;
			}
		}
	}
}



