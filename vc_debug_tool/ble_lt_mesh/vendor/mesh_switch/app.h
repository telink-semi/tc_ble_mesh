/********************************************************************************************************
 * @file	app.h
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
#pragma once

#include "tl_common.h"

#define SWITCH_GROUP_ADDR_START 	0xc000

enum{
    SWITCH_MODE_NORMAL      = 0,
    SWITCH_MODE_GATT	   	= 1,
};


extern u32 switch_mode_tick;

void user_init_deepRetn(void);
void switch_mode_set(int mode);
void mesh_proc_keyboard ();
void global_reset_new_key_wakeup();
int mesh_switch_send_mesh_adv();


