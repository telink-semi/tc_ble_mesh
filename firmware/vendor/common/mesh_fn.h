/********************************************************************************************************
 * @file	mesh_fn.h
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

enum{
    FS_DISCONNECT_TYPE_POLL_TIMEOUT = 1,
    FS_DISCONNECT_TYPE_CLEAR        = 2,
    FS_DISCONNECT_TYPE_RX_REQUEST   = 3, // receive request when being keeping friend ship.
};

enum{
    FN_CACHE_REPLACE_NONE           = 0,
    FN_CACHE_REPLACE_FINISH	        = 1,
    FN_CACHE_REPLACE_AND_OVERWRITE  = 2,
};

void mesh_feature_set_fn();
void fn_update_RecWin(u8 RecWin);
u32 get_RecWin_connected();
void friend_ship_establish_ok_cb_fn(u8 lpn_idx);
void friend_ship_disconnect_cb_fn(u8 lpn_idx, int type);
u8 mesh_get_fn_cache_size_log_cnt();
void friend_cmd_send_offer(u8 lpn_idx);
void friend_cmd_send_update(u8 lpn_idx, u8 md);
void friend_cmd_send_clear(u16 adr_dst, u8 *par, u32 len);
void friend_cmd_send_clear_conf(u16 adr_dst, u8 *par, u32 len);
void friend_cmd_send_subsc_conf(u16 adr_dst, u8 transNo);
int mesh_tx_cmd_layer_upper_ctl_FN(u8 op, u8 *par, u32 len_par, u16 adr_dst);
void mesh_iv_update_start_poll_fn(u8 iv_update_by_sno, u8 beacon_iv_update_pkt_flag);
u16 mesh_group_match_friend(u16 adr);
int fn_cache_get_extend_adv_short_unseg(u32 ctl, u8 len_ut_max_single_seg_rx);
int is_in_mesh_friend_st_fn_all();


extern u8 g_max_lpn_num;
extern u8 use_mesh_adv_fifo_fn2lpn;

