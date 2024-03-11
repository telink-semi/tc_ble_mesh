/********************************************************************************************************
 * @file	mesh_test_cmd.h
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
#include "tl_common.h"

#define SEC_MES_DEBUG_EN    0
#define FRIEND_MSG_TEST_EN  0

#define FRI_SAMPLE_EN       0
#define	SEG_DEBUG_LED(on)	//do{ light_onoff_all(on);}while(0)


void mesh_message6();
void friend_cmd_send_sample_message(u8 op);
void friend_ship_sample_message_test();
void mesh_message_fri_msg_ctl_seg();
int mesh_tx_cmd_layer_upper_ctl_primary(u8 op, u8 *par, u32 len_par, u16 adr_dst);
void test_cmd_wakeup_lpn();
void test_cmd_tdebug();
void mesh_iv_update_test_initiate(u8 key_code);


