/********************************************************************************************************
 * @file	led.c
 *
 * @brief	for TLSR chips
 *
 * @author	telink
 * @date	Sep. 30, 2010
 *
 * @par     Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#include "led.h"
#include "../../vendor/common/led_cfg.h"

led_count_init_function led_count_init_cb;
static inline void led_on(int led){
	gpio_write(led, 1);
}

static inline void led_off(int led){
	gpio_write(led, 0);
}

extern led_ctrl_t led_ctrl[];

void led_set_pattern(int led, int pat){
    if(led_count_init_cb){
        led_count_init_cb();
    }
	led_ctrl[led].pattern = pat;
	ET_INIT(&led_ctrl[led].et);
}

int led_handler(int led){
	int t;
	et_t *et = &led_ctrl[led].et;
	ET_BEGIN(et);

	t = led_ctrl[led].pattern_time[led_ctrl[led].pattern * 2];
	if(t > 0){
		led_on(led_ctrl[led].gpio);
		led_ctrl[led].duration = (t << 0);	// convert to us
		ET_YIELD_TIME_R(et, led_ctrl[led].duration, LED_MIN_RESP_TIME);
		
		t = led_ctrl[led].pattern_time[led_ctrl[led].pattern * 2 + 1];
		if(t > 0){		//	else < 0, always on...
			led_off(led_ctrl[led].gpio);
			if(t > LED_MIN_RESP_TIME){
				led_ctrl[led].duration = (t - LED_MIN_RESP_TIME) << 0;
				ET_YIELD_TIME_R(et, led_ctrl[led].duration, LED_MIN_RESP_TIME);
			}
		}
        led_set_next_pattern(led);
		
	}else{
		led_off(led_ctrl[led].gpio);
	}
	ET_END_R(et, LED_MIN_RESP_TIME);

}

int led_timer_handler(void *data){
	foreach_arr(i, led_ctrl){
		led_handler(i);
	}
	return 0;
}

void led_init(void){
	static ev_time_event_t led_timer = {led_timer_handler};
	ev_on_timer(&led_timer, LED_MIN_RESP_TIME*1000);
}

