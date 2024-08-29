/********************************************************************************************************
 * @file	pair_provision.h
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

// ------ parameters setting ---------
#define PAIR_PROV_SCAN_TIMEOUT_MS			(10*1000)
#define PAIR_PROV_DISTRIBUTE_RETRY_CNT		(1)		// retry if provisionee did not receive distribute date. but make no sense if provisioner did not receive comfirm, because after provisionee send confirm, net key has been updated.
#define PAIR_PROV_ADDITIONAL_RETRANSMIT_CNT	(16)	// to make sure provisioner receive confirm command. so set to be a little more.

#define PAIR_PROV_FIRST_NODE_ADDR			(1)		// first node will generate network
#define PAIR_PROV_UUID_FLAG					{'M','P','a','i','r'};	// because auto triger provison, so need some special flag to identify whether support pair provision.
#define PAIR_PROV_UNPROV_ADDRESS_START		(0x7000)// element address of default network is between 0x7000 and 0x7fff


typedef enum{
	PAIR_PROV_PROC_ST_IDLE					= 0,
	PAIR_PROV_PROC_ST_GET_ONLINE_NODES,
	PAIR_PROV_PROC_ST_SCAN,
	PAIR_PROV_PROC_ST_DISTRIBUTE,					// distribute key and unicast address.
	PAIR_PROV_PROC_ST_CONFIRM,
	PAIR_PROV_PROC_ST_CLOSE,
	PAIR_PROV_PROC_ST_MAX,
}pair_prov_proc_st_e;


typedef struct{
	provison_net_info_str net_info; // size is 25
	u8 device_key[16];
	u8 app_key[16];
	u16 appkey_idx;
	u8 mac_scan[6];
}vd_cmd_pair_prov_distribute_t;

typedef struct{
	u8 mac_self[6];
}vd_cmd_pair_prov_confirm_t;

typedef struct{
	vd_cmd_pair_prov_distribute_t distribute_data; // size is 65
	_align_4_ u32 tick;
	u16 cache_cnt;
	u16 cache_addr[CACHE_BUF_MAX];	// cache_addr_before_reset_network
	u8 retry_cnt;	// retry count for every state.
	u8 cnt_fail;	// count of failure
	u8 revert_network_flag;
	u8 st;
}pair_prov_proc_t;



void pair_proc_set_st(u8 st);
void pair_proc_start();
void pair_proc_set_start_scan();
void pair_provision_proc();

int cb_vd_mesh_pair_prov_distribute(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int cb_vd_mesh_pair_prov_confirm(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int cb_vd_mesh_pair_prov_reset_all_nodes(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int vd_cmd_pair_prov_confirm(u16 addr_src, u16 addr_dst);
void pair_prov_kick_out_all_nodes();


