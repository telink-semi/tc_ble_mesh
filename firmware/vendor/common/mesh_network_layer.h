/********************************************************************************************************
 * @file	mesh_network_layer.h
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
#include "proj_lib/sig_mesh/app_mesh.h"

u16 mesh_tx_network_layer_cb(mesh_cmd_bear_t *p_bear, mesh_match_type_t *p_match_type, u8 *sec_type);
int mesh_rc_network_layer_cb(mesh_cmd_bear_t *p_bear, mesh_nw_retrans_t *p_nw_retrans);
int mesh_network_df_initial_start(u8 op, u16 netkey_offset, u16 destination, u16 dpt_org, u16 dpt_ele_cnt);
int mesh_rc_data_layer_network (u8 *p_payload, int src_type, u8 need_proxy_and_trans_par_val);

void cache_init(u16 ele_adr);
void mesh_update_rpl(u8 *p, int idx);
void mesh_add_rpl(u8 *p);
int is_exist_in_cache(u8 *p, u8 friend_key_flag, int save);
u16 get_mesh_current_cache_num(); // Note, there may be several elements in a node, but there is often only one element that is in cache.
