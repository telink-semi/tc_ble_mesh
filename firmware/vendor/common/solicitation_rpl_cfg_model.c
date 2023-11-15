/********************************************************************************************************
 * @file	solicitation_rpl_cfg_model.c
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
#include "config_model.h"
#ifndef WIN32
#include "proj/mcu/watchdog_i.h"
#endif 
#include "proj_lib/ble/ll/ll.h"
#include "proj_lib/ble/blt_config.h"
#include "vendor/common/user_config.h"
#include "solicitation_rpl_cfg_model.h"

#if MD_SOLI_PDU_RPL_EN
soli_rpl_t soli_rpl[SOLI_RPL_MAX];

#if PTS_TEST_EN
soli_srv_dat_t soli_service_data;
u32 soli_sno_tx = 0;
int soli_pdu_adv_cnt = 0;
#endif

void add2_soli_rpl(u8 *p, int idx){    
	u32 sno = 0;
	mesh_cmd_nw_t *p_nw = (mesh_cmd_nw_t *)p;
    soli_rpl[idx].ssrc = p_nw->src;   
    memcpy(&sno, p_nw->sno, SIZE_SNO);
    soli_rpl[idx].ssno = sno;
}

void clear_soli_rpl(u16 addr){
	if(is_own_ele(addr)){
		memset((u8 *)soli_rpl, 0x00, sizeof(soli_rpl));
	}
	return;
}

int is_exist_in_soli_rpl(u8 *p)
{
	mesh_cmd_nw_t *p_nw = (mesh_cmd_nw_t *)p;
	if(mesh_adr_check_src_own_rx(p_nw->src)){
        return ERR_NO_NW_DEC_ADR_INVALID;
    }
	
	u32 sno = 0;
    memcpy(&sno, p_nw->sno, SIZE_SNO);
	foreach_arr(i, soli_rpl){
		if(p_nw->src == soli_rpl[i].ssrc){ 
			if(sno <= soli_rpl[i].ssno){
				return ERR_NO_NW_DEC_CACHE_OLD;
			}
			else{
				add2_soli_rpl(p, i);
				return 0;				
			}
		}
	}

	static u8 rpl_idx = 0;	
	add2_soli_rpl(p, rpl_idx);
	rpl_idx = (rpl_idx + 1) % SOLI_RPL_MAX;
	return 0;
}

#if MD_SERVER_EN
int mesh_cmd_sig_cfg_soli_rpl_clear(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err = 0;
	addr_range_t *p_addr_range = (addr_range_t *)par;
	if(p_addr_range->length_present_l){
		clear_soli_rpl(p_addr_range->range_start_l);
	}
	else{
		clear_soli_rpl(p_addr_range->multicast_addr);
	}
	if(cb_par->op_rsp != STATUS_NONE){
		err = mesh_tx_cmd_rsp(cb_par->op_rsp, par, par_len, ele_adr_primary, cb_par->adr_src, 0, 0);
	}
	return err;
}
#endif

#if MD_CLIENT_EN
int mesh_cmd_sig_cfg_soli_rpl_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err = 0;
	if(cb_par->model){	// model may be Null for status message
	}
	return err;
}
#endif

#if PTS_TEST_EN
int mesh_send_proxy_solicitation_pdu(u16 adr_dst)
{
	mesh_cmd_bear_t cmd_bear;
	mesh_cmd_nw_t *p_nw = &cmd_bear.nw;
	p_nw->ivi = 0;
	p_nw->ctl = 1;
	p_nw->ttl = 0;
	p_nw->src = ele_adr_primary;
	p_nw->dst = adr_dst;
	memcpy(p_nw->sno, &soli_sno_tx, sizeof(p_nw->sno));
	soli_sno_tx++;
	mesh_sec_msg_enc_nw((u8 *)p_nw, 0, SWAP_TYPE_NONE, MASTER, 0, 0, MESH_ADV_TYPE_MESSAGE, NONCE_TYPE_SOLICITATION, 0, 0);
	soli_service_data.id_type = SOLI_WITH_RPL_TYPE;
	memcpy(soli_service_data.id_para, p_nw, sizeof(soli_service_data.id_para));
	soli_pdu_adv_cnt = SOLICI_PDU_CNT;
	return 1;
}
#endif
#endif
