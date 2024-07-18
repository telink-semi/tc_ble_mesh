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

#if 1
#include "ll_app_mesh.h"
#if (__TLSR_RISCV_EN__)
#include "stack/ble/controller/ble_controller.h"
#endif
#if LLSYNC_ENABLE

typedef struct{
    u16 op;
    u16 src;
    u16 dst;
}s_ll_vd_op_recv_t;

s_ll_vd_op_recv_t s_ll_vd_op_recv = {0};


STATIC_ASSERT(LLSYNC_MESH_UNNET_ADV_TOTAL_TIME <= (655 * 1000)); // dut to sg_mesh_net.timer_total_cnt is u16.


int llsync_tlk_mesh_recv_data_handle(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	s_ll_vd_op_recv.op = cb_par->op;
	s_ll_vd_op_recv.src = cb_par->adr_src;
	s_ll_vd_op_recv.dst = cb_par->adr_dst;
	ble_qiot_ret_status_t st = llsync_mesh_recv_data_handle(LLSYNC_MESH_MODEL_OP_3(cb_par->op, LLSYNC_MESH_CID_VENDOR), par, par_len);
	memset(&s_ll_vd_op_recv, 0, sizeof(s_ll_vd_op_recv));

	return st;
}

void llsync_mesh_vendor_data_send(uint32_t opcode, uint8_t *data, uint16_t data_len)
{
	u16 op_1byte = GET_1BYTE_VENDOR_OP(opcode);
	if(s_ll_vd_op_recv.op){
		mesh_tx_cmd_rsp(op_1byte, data, data_len, s_ll_vd_op_recv.dst, s_ll_vd_op_recv.src, 0, 0);
	}else{
		u16 addr_dst = 0x0001; // default value ?
		if(LLSYNC_MESH_VND_MODEL_OP_INDICATION == opcode){
			addr_dst = LLSYNC_GROUP_ADDR_COMMON;
		}

		mesh_tx_cmd2normal_primary(op_1byte, data, data_len, addr_dst, 0);
	}
}

void llsync_start_silence_adv_timeout_check()
{
	llsync_mesh_adv_start(LLSYNC_MESH_UNNET_ADV); // just to start timer event to run silence adv event.
}

void llsync_stop_silence_adv_timeout_check()
{
	if((LLSYNC_MESH_SILENCE_ADV_TIMER == sg_mesh_net.timer_type) || (LLSYNC_MESH_UNNET_ADV_TIMER == sg_mesh_net.timer_type)){
		sg_mesh_net.timer_cnt  = 0;
		sg_mesh_net.timer_total_cnt = 0;
		sg_mesh_net.timer_type = LLSYNC_MESH_STOP_TIMER;
		llsync_mesh_timer_stop(sg_mesh_net.mesh_timer); // &llsync_mesh_timer_system
	}
}

void llsync_tlk_init_three_para_and_mac()
{
	llsync_mesh_dev_info_get_from_flash(&sg_dev_info);
	foreach(i, sizeof(tbl_mac)){
		u8 val = VER_CHAR2NUM(sg_dev_info.device_name[i*2]) << 4;
		val |= VER_CHAR2NUM(sg_dev_info.device_name[i*2 + 1]);
		tbl_mac[sizeof(tbl_mac) - 1 - i] = val;
	}

	// llsync init
	sg_mesh_net.timer_type = LLSYNC_MESH_STOP_TIMER; // init to stop state.
	llsync_mesh_init();
	if(!is_provision_success()){
		if(LLSYNC_MESH_STOP_TIMER == sg_mesh_net.timer_type){
			llsync_start_silence_adv_timeout_check();
		}
	}
}

void llsync_set_dev_auth()
{
	u8 sha256_out[32];
	llsync_mesh_auth_clac(dev_random, sha256_out);
	mesh_set_dev_auth(sha256_out, 16);
}

void llsync_tlk_init_scan_rsp()
{
	u8 rsp[31+1];
	u8 rsp_len = 0;
	llsync_mesh_scan_data_get(rsp, &rsp_len);
	bls_ll_setScanRspData(rsp, rsp_len);
}

void llsync_mesh_timer_period_proc(u32 ms_now, u32 interval_cnt) // LLSYNC_MESH_TIMER_PERIOD
{
	if(sg_mesh_net.timer_type != LLSYNC_MESH_STOP_TIMER){ // LLSYNC_MESH_UNNET_ADV_TIMER is also invalid.
		u32 inc_10ms = (ms_now % 10 + interval_cnt) / 10; // LLSYNC_MESH_TIMER_PERIOD
		foreach(i, inc_10ms){
			llsync_mesh_timer_cb(NULL);
		}
	}
}

void llsync_mesh_vendor_data_publish(uint32_t opcode, uint8_t *data, uint16_t data_len)
{
    llsync_mesh_vendor_data_send(opcode, data, data_len);
}
#endif
#endif
