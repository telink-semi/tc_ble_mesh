/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */


#include "tl_common.h"
#include "proj_lib/sig_mesh/app_mesh.h"
#include "ble_qiot_import.h"


#define GET_1BYTE_VENDOR_OP(op3byte)		(op3byte >> 16)


#define LLSYNC_VND_OP_SET          			GET_1BYTE_VENDOR_OP(LLSYNC_MESH_VND_MODEL_OP_SET)
#define LLSYNC_VND_OP_GET          			GET_1BYTE_VENDOR_OP(LLSYNC_MESH_VND_MODEL_OP_GET)
#define LLSYNC_VND_OP_SET_UNACK    			GET_1BYTE_VENDOR_OP(LLSYNC_MESH_VND_MODEL_OP_SET_UNACK)
#define LLSYNC_VND_OP_STATUS       			GET_1BYTE_VENDOR_OP(LLSYNC_MESH_VND_MODEL_OP_STATUS)
#define LLSYNC_VND_OP_INDICATION   			GET_1BYTE_VENDOR_OP(LLSYNC_MESH_VND_MODEL_OP_INDICATION)
#define LLSYNC_VND_OP_CONFIRM 				GET_1BYTE_VENDOR_OP(LLSYNC_MESH_VND_MODEL_OP_CONFIRMATION)


#define LLSYNC_GROUP_ADDR_COMMON			(0xf000)



extern ble_device_info_t sg_dev_info;
extern llsync_mesh_distribution_net_t sg_mesh_net;


int llsync_tlk_mesh_recv_data_handle(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
void llsync_tlk_init_three_para_and_mac();
void llsync_set_dev_auth();
void llsync_tlk_init_scan_rsp();
ble_qiot_ret_status_t llsync_mesh_dev_info_get_from_flash(ble_device_info_t *dev_info);
void llsync_mesh_timer_period_proc(u32 ms_now, u32 interval_cnt);
void llsync_mesh_timer_cb(void *param);
void llsync_mesh_adv_start(uint8_t adv_type);
void llsync_stop_silence_adv_timeout_check();



