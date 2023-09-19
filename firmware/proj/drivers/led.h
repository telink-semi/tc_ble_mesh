/********************************************************************************************************
 * @file	led.h
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
#pragma once

#include "tl_common.h"
#include "proj/os/ev.h"
#include "proj/os/et.h"

// if pattern changed,  the led will change accordingly within LED_MIN_RESP_TIME in ms
#define LED_MIN_RESP_TIME	50

typedef struct {
	u32 gpio;
	int *pattern_time;
	et_t et;
	int duration;
	u8  pattern;
}led_ctrl_t;

typedef void (*led_count_init_function)(void);
extern led_count_init_function led_count_init_cb;

int led_handler(int led);
void led_set_pattern(int led, int pat);
void led_set_next_pattern(int led);
void led_init(void);


