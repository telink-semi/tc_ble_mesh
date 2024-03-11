/********************************************************************************************************
 * @file	user_proc.h
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
#ifndef __USER_PROC_H
#define __USER_PROC_H
#include "tl_common.h"
#include "proj_lib/ble/ll/ll.h"
#include "proj_lib/ble/blt_config.h"
#include "vendor/common/user_config.h"
#include "user_ali.h"
#include "nl_api/nl_common.h"

u8 ais_pri_data_set(u8 *p);
void user_sha256_data_proc();
void user_node_oob_set();
void user_power_on_proc();
void user_mesh_cps_init();
void user_set_def_sub_adr();
void user_system_time_proc();
void mesh_provision_para_init(u8 *p_random);
void user_prov_multi_oob();
void user_prov_multi_device_uuid();
int user_node_rc_link_open_callback();

#endif
