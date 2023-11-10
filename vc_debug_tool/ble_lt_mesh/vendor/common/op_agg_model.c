/********************************************************************************************************
 * @file	op_agg_model.c
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
#include "op_agg_model.h"

int is_op_agg_model(u32 model_id) 
{
#if MD_OP_AGG_EN
	if((SIG_MD_OP_AGG_S == model_id) || (SIG_MD_OP_AGG_C == model_id)){
		return 1;
	}
#endif
	return 0;
}

int is_in_op_agg_mode()
{
#if MD_OP_AGG_EN
	// have made sure not be tx seg busy before tx aggregator in mesh_op_agg_status_item_format_add_(), and resource of tx segment flow will used.
	return (CFG_OP_AGG_SEQ == mesh_tx_seg_par.match_type.mat.op_rsp); // CFG_OP_AGG_STATUS is the last op in agg mode
#else
	return 0;
#endif
}

int is_op_agg_use_device_key(u8 *acc, int len) // op agg use devicekey/appkey depend on the payload
{
#if MD_OP_AGG_EN
	u16 op = rf_link_get_op_by_ac(acc);
	u16 offset = 0; 
	if(CFG_OP_AGG_SEQ == op){
		offset = OFFSETOF(op_agg_seq_t, par) + 2;
	}
	else if(CFG_OP_AGG_STATUS == op){
		offset = OFFSETOF(op_agg_seq_st_t, par);
	}
	else{
		return 0;
	}
	
	mesh_op_resource_t op_resource = {0};
	for(u16 i=offset; i<len;){ 
		if(acc[i]&0x01){
			agg_item_long_t *p_long_item = (agg_item_long_t *) &acc[i];
			if(p_long_item->length){				
		        if(0 == mesh_search_model_id_by_op(&op_resource, rf_link_get_op_by_ac(p_long_item->acc_par), 1)){
		            if(is_use_device_key(op_resource.id, op_resource.sig)){
		                return 1;
		            }
					else{
						return 0;
					}
		        }				
			}
			i += (p_long_item->length + OFFSETOF(agg_item_long_t, acc_par));
		}
		else{
			agg_item_short_t *p_short_item = (agg_item_short_t *) &acc[i];
			if(p_short_item->length){				
		        if(0 == mesh_search_model_id_by_op(&op_resource, rf_link_get_op_by_ac(p_short_item->acc_par), 1)){
		            if(is_use_device_key(op_resource.id, op_resource.sig)){
		                return 1;
		            }
					else{
						return 0;
					}
		        }				
			}
			i += (p_short_item->length + OFFSETOF(agg_item_short_t, acc_par));
		}
	}
#endif

	return 1;
}

#define CUR_AGG_PAR_LEN 	mesh_tx_seg_par.match_type.mat.len_ac

int mesh_op_agg_status_item_format_add(u32 len)
{
#if MD_OP_AGG_EN
	op_agg_seq_st_t *p_op_agg_st =(op_agg_seq_st_t *) mesh_cmd_ut_tx_seg;
	u32 ac_len = OFFSETOF(op_agg_seq_st_t, par)+ CUR_AGG_PAR_LEN + ((len<BIT(7)) ? 1:2) + len + GET_SEG_SZMIC(SZMIC_SEG_FLAG);
	if(is_busy_tx_seg(0) || (ac_len > ACCESS_WITH_MIC_LEN_MAX)){
		return 0;
	}

	if(len < BIT(7)){
		agg_item_short_t *p_agg_item = (agg_item_short_t *)&p_op_agg_st->par[CUR_AGG_PAR_LEN];
		p_agg_item->format = 0;
		p_agg_item->length = len;
		CUR_AGG_PAR_LEN += 1;
	}
	else{
		agg_item_long_t *p_agg_item = (agg_item_long_t *)&p_op_agg_st->par[CUR_AGG_PAR_LEN];
		p_agg_item->format = 1;
		p_agg_item->length = len;
		CUR_AGG_PAR_LEN += 2;
	}
#endif
	return 1;
}

#if MD_OP_AGG_EN
#if MD_SERVER_EN
int mesh_cmd_sig_cfg_op_agg_seq(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{	
	int err = -1;
	op_agg_seq_t *p_op_agg = (op_agg_seq_t *)par;
	op_agg_seq_st_t *p_op_agg_st =(op_agg_seq_st_t *) mesh_cmd_ut_tx_seg;
	p_op_agg_st->status = OP_AGG_SUC;
	p_op_agg_st->ele_addr = p_op_agg->ele_addr;
	
	if(is_busy_tx_seg(0)){
		return err;
	}
	
	mesh_tx_seg_par.match_type.mat.len_ac = 0;
	if(is_own_ele(p_op_agg->ele_addr)){	
		u16 last_st_len = CUR_AGG_PAR_LEN;
		int ret = 0;
		u8 item_num = 0;
		mesh_tx_seg_par.match_type.mat.op_rsp = CFG_OP_AGG_SEQ; // use as op agg mode flag temporarily
		
		for(u16 i=0; i<par_len-OFFSETOF(op_agg_seq_t, par);){			
			if(p_op_agg->par[i]&0x01){// length long present
				agg_item_long_t *p_long_item = (agg_item_long_t *) &p_op_agg->par[i];
				ret = mesh_rc_data_layer_access(p_long_item->acc_par, p_long_item->length, cb_par->p_nw);
				if(0 == ret){
					i += (p_long_item->length + OFFSETOF(agg_item_long_t, acc_par));
				}
			}
			else{
				agg_item_short_t *p_short_item = (agg_item_short_t *) &p_op_agg->par[i];
				ret = mesh_rc_data_layer_access(p_short_item->acc_par, p_short_item->length, cb_par->p_nw);
				if(0 == ret){
					i += (p_short_item->length + OFFSETOF(agg_item_short_t, acc_par));
				}
			}
			if(last_st_len == CUR_AGG_PAR_LEN){// not reliable message, add empty item
				mesh_op_agg_status_item_format_add(0);
			}
			last_st_len = CUR_AGG_PAR_LEN;
			if(ret){
				p_op_agg_st->status = (-1==ret) ? OP_AGG_UNKNOWN_MSG:ret;
				break;
			}
			item_num++;
		}

		mesh_tx_seg_par.match_type.mat.op_rsp = 0; // exit op agg mode
	}
	else{
		p_op_agg_st->status = OP_AGG_INVALID_ADDR;
		if(!is_unicast_adr(p_op_agg->ele_addr)){
			return err;
		}
	}

	err = mesh_tx_cmd_rsp(cb_par->op_rsp, &p_op_agg_st->status, CUR_AGG_PAR_LEN+3, ele_adr_primary, cb_par->adr_src, 0, 0);

	return err;
}
#endif

#if MD_CLIENT_EN
int mesh_cmd_sig_cfg_op_agg_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err = 0;
	if(cb_par->model){	// model may be Null for status message
	}
	return err;
}
#endif
#endif
