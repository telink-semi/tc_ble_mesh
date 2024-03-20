/********************************************************************************************************
 * @file	mesh_access_layer.h
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

void cache_init(u16 ele_adr);
void mesh_update_rpl(u8 *p, int idx);
void mesh_add_rpl(u8 *p);
int is_exist_in_cache(u8 *p, u8 friend_key_flag, int save);
u16 get_mesh_current_cache_num(); // Note, there may be several elements in a node, but there is often only one element that is in cache.
