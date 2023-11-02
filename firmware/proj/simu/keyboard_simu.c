/********************************************************************************************************
 * @file	keyboard_simu.c
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
#include "../tl_common.h"
#include "../os/ev.h"
#include "../drivers/usbkeycode.h"
#include "../drivers/keyboard.h"
#include "vendor/common/keycode_custom.h"

u8 kb_simu_is_func_key;
u32 kb_simu_pressed_matrix[8];    // normally max 8 key rows
kb_data_t kb_simu_key_data;

int kb_simu_timer(void *data){
    static int key_cnt = 0;
    static u8 key_code = VK_1;
    
    kb_simu_key_data.cnt = 1;
    kb_simu_key_data.ctrl_key = 0;
    kb_simu_key_data.keycode[0] = key_code;
    ++key_code; 
    if(VK_0 < key_code){
        key_code = VK_1;
        kb_simu_key_data.cnt = 0;
        return (1000*1000);
    }
    return KEYBOARD_SCAN_INTERVAL;
}

void kb_simu_init(void){
    static ev_time_event_t kb_timer = {kb_simu_timer};
    ev_on_timer(&kb_timer, KEYBOARD_SCAN_INTERVAL);
}


