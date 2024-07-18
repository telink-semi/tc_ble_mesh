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
#include "proj_lib/ble/blt_config.h"
#include "vendor/common/user_config.h"
#include "solicitation_rpl_cfg_model.h"

service_uuid_t *get_service_uuid(u16 uuid, u8 *payload, u8 len)
{
	service_uuid_t *p_service = 0; 
	int offset = 0;

	while(offset < len){
		p_service = (service_uuid_t *)(payload + offset);

		if((p_service->len < 2) || (p_service->type > GAP_ADTYPE_SIMPLE_PAIRING_RAND_R256)){
			return 0;
		}

		if(GAP_ADTYPE_SERVICE_DATA == p_service->type){
			if(p_service->uuid == uuid){
				return p_service;
			}
		}
		
		offset += (p_service->len + 1);
	}

	return 0;
}

#if MD_ON_DEMAND_PROXY_EN
u32 mesh_on_demand_proxy_time = 0; // max 256s, clock_time() is enough

/**
 * @brief       This function get if it is valid state to reveive solicitation PDU.
 * @return      1: yes. 0: no.
 * @note        
 */
_attribute_ram_code_ int mesh_on_demand_is_valid_st_to_rx_solicitation() // ramcode for irq function.
{
	return ((GATT_PROXY_SUPPORT_DISABLE == model_sig_cfg_s.gatt_proxy)
		#if MD_PRIVACY_BEA
			&& (PRIVATE_PROXY_DISABLE == g_mesh_model_misc_save.privacy_bc.proxy_sts)
		#endif
			&& g_mesh_model_misc_save.on_demand_proxy);
}

/**
 * @brief       This function server to start the tick of advertising with private network identity type if support.
 * @return      none
 * @note        
 */
void mesh_on_demand_private_gatt_proxy_start()
{
	if(mesh_on_demand_is_valid_st_to_rx_solicitation()){
		mesh_on_demand_proxy_time = clock_time() | 1;
	}
}

/**
 * @brief       This function server to stop the tick of advertising with private network identity type.
 * @return      none
 * @note        
 */
void mesh_on_demand_private_gatt_proxy_stop()
{
	mesh_on_demand_proxy_time = 0;
}

/**
 * @brief       This function ...
 * @param[in]   payload	- ADV payload of solicitation PDU.
 * @param[in]   len		- length of payload
 * @return      1: is payload of solicitation PDU; 0: not.
 * @note        
 */
int mesh_soli_pdu_handle(u8 *payload, u8 len)
{
#if (MD_SERVER_EN)
	service_uuid_t *p_service = NULL;

	#if VENDOR_IOS_SOLI_PDU_EN
	if(is_ios_soli_pdu(payload)){
		ios_soli_pdu_pkt_t *pa = (ios_soli_pdu_pkt_t *)payload;
		int len_trim = len - OFFSETOF(ios_soli_pdu_pkt_t, service_len);
		if(len_trim > 0){
			p_service = get_service_uuid(SIG_MESH_PROXY_SOLI_VAL, &pa->service_len, len_trim); // ios
		}
	}else
	#endif
	{
		p_service = get_service_uuid(SIG_MESH_PROXY_SOLI_VAL, payload, len); // Android
	}
	
	if(p_service){ // have checked (mesh_on_demand_is_valid_st_to_rx_solicitation()) in adv filter function.
		soli_srv_dat_t *p_soli_dat = (soli_srv_dat_t *)p_service->data;
		if(SOLI_WITH_RPL_TYPE==p_soli_dat->id_type){
			mesh_cmd_nw_t *p_nw = (mesh_cmd_nw_t *)p_soli_dat->id_para;
			int len_dec_nw = p_service->len - 4 - (OFFSETOF(mesh_cmd_nw_t, data) - 2);//2 is sizeof(dst addr)				
			int err = mesh_sec_msg_dec_nw((u8 *)p_nw, len_dec_nw, p_nw->nid, NONCE_TYPE_SOLICITATION, MESH_BEAR_ADV, 0);			

			if(!err && !is_exist_in_soli_rpl((u8 *)p_nw)){
				mesh_on_demand_private_gatt_proxy_start();
			}
		}
		
		return 1;
	}
#endif	

	return 0;
}
#endif	

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
