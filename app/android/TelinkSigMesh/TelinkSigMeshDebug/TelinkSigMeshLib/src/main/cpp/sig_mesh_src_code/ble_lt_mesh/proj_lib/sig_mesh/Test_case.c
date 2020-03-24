/********************************************************************************************************
 * @file     Test_case.c 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/
#include "Test_case.h"
#include "../../proj/common/tutility.h"
#include "../../vendor/common/app_provison.h"
#include "../../vendor/common/App_health.h"
#if TESTCASE_FLAG_ENABLE
#include "../../proj_lib/ble/ble_common.h"
#include "../../vendor/common/cmd_interface.h"
#include "../../vendor/common/sensors_model.h"
#include "../../vendor/common/lighting_model.h"
#include "../../vendor/common/lighting_model_HSL.h"
#include "../../vendor/common/lighting_model_xyL.h"
#include "../../vendor/common/lighting_model_LC.h"
#include "../../vendor/common/scene.h"
#include "../../vendor/common/app_heartbeat.h"
extern my_fifo_t		hci_tx_fifo;

u8 tc_seg_buf[132];

tc_info tc_par;
u8 tc_adv_type = LL_TYPE_ADV_NONCONN_IND;

u8 netkey_list_test_mode_en = 0;

u8 tc_set_fifo(u8 cmd,u8 *pfifo,u8 cmd_len)
{
	u32 len_max = sizeof(tc_seg_buf)-4;
	if(cmd_len>len_max){
		cmd_len = len_max;
	}
	memcpy(tc_seg_buf, pfifo, cmd_len);
	u8 len = cmd_len;
	while(len--){
		tc_seg_buf[len+2] = tc_seg_buf[len];
	}
	tc_seg_buf[0] = cmd;
	tc_seg_buf[1] = cmd_len;
	
	my_fifo_push_hci_tx_fifo(tc_seg_buf, cmd_len+2, 0, 0);  // test
	return 1;
}

void tc_set_result(u8 reason ,u8 para)
{
	u8 tmp_fifo[2];
	tmp_fifo[0] = reason;
	tmp_fifo[1] = para;
	SET_TC_FIFO(TSCRIPT_END,tmp_fifo,sizeof(tmp_fifo));
	return;
}

extern u16 ele_adr_primary;
mesh_cmd_tscript_head_t mesh_cmd_tscript_head;
u8 tc_mesh_cmd[32] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 
                        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,0x1E,0x1F,0x20};

extern int mesh_tx_cmd_rsp_cfg_model(u16 op, u8 *par, u32 par_len, u16 adr_dst);
void test_case_tx_seg_noack(u16 adr_dst, u8 use_appkey)
{
	if(use_appkey){
		mesh_tx_cmd2normal_primary(G_ONOFF_SET_NOACK, (u8 *)tc_mesh_cmd, 18, adr_dst, 0);
	}
	else{   // use device key
		mesh_tx_cmd_rsp_cfg_model(COMPOSITION_DATA_STATUS, tc_mesh_cmd, 19, adr_dst);
	}
}

void test_case_tx_unseg_noack(u16 adr_dst, u8 appkey)
{
	if(appkey){
		access_cmd_onoff(0x01, 1, 1, 0, 0); 
	}
	else{
		mesh_tx_cmd_rsp_cfg_model(COMPOSITION_DATA_STATUS, tc_mesh_cmd, 10, adr_dst);
	}
}

void test_case_netkey_set(u16 op)
{
	mesh_netkey_set_t netkey_set = {0};
	netkey_set.idx = 0x00;
	u32 len_par = sizeof(netkey_set);
	if(NETKEY_UPDATE == op){
		memset(netkey_set.key, 0x28, 16);
	}else if(NETKEY_DEL == op){
        netkey_set.idx = 0x01;
		len_par = OFFSETOF(mesh_netkey_set_t, key);
	}
	mesh_tx_cmd2normal_primary(op, (u8 *)&netkey_set, len_par, 0x01, 0x01);
}

void test_case_netkey_get()
{
	u8 par[1] = {0};
	mesh_tx_cmd2normal_primary(NETKEY_GET, par, 0, 0x01, 0x01);
}

void test_case_appkey_set(u16 op)
{
	mesh_appkey_set_t appkey_set = {{0x00,0x90,0x00}};
	u32 len_par = sizeof(appkey_set);
	if(APPKEY_UPDATE == op){
		memset(appkey_set.appkey, 0x28, 16);
	}else if(APPKEY_DEL == op){
		SET_KEY_INDEX_L(appkey_set.net_app_idx, 0x123);
		SET_KEY_INDEX_H(appkey_set.net_app_idx, 0x456);
		len_par = OFFSETOF(mesh_appkey_set_t, appkey);
	}
	mesh_tx_cmd2normal_primary(op, (u8 *)&appkey_set, len_par, 0x01, 0x01);
}

void test_case_appkey_get()
{
	u16 nk_index = 0;
	mesh_tx_cmd2normal_primary(APPKEY_GET, (u8 *)&nk_index, sizeof(nk_index), 0x01, 0x01);
}

void mesh_rc_cmd_tscript(u8 *buff, int n)
{       
    memcpy(&tc_par.tc_id, buff+2, 8);
    if((buff[0] == 0xa0 && buff[1] == 0xff) ){
    	if(MASK_HIGH16(NODE_IVU_BI_01) == MASK_HIGH16(tc_par.tc_id)){
			// preset parameters for iv update test 
			if(tc_par.sub_id == 0){
				iv_idx_update_change2next_st = 0;	// init
				
		    	if(tc_par.tc_id == NODE_IVU_BI_01){
		    		iv_update_test_mode_en = 0;
					mesh_iv_update_enter_update2normal();
		    	}else if(tc_par.tc_id == NODE_IVU_BI_02){
		    		iv_update_test_mode_en = 0;
					mesh_iv_update_enter_normal();
		    	}else{
		    		iv_update_test_mode_en = 1;
		    	}
		    	return ;
	    	}
    	}
    	
		u8 tc_ttl_tmp = model_sig_cfg_s.ttl_def;
        switch(tc_par.tc_id){
			/*--------------------provisioning procedure ----------------------*/
			case NODE_PROV_UPD_BV_01_C:
				set_node_prov_para_no_pubkey_output_oob();
				break;
			case NODE_PROV_UPD_BV_02_C:
				if(tc_par.sub_id == 0){
					set_node_prov_para_no_pubkey_input_oob();
				}else if ((tc_par.sub_id & 0x80) == 0x80){
					#if !__PROJECT_MESH_PRO__
					// set the auth and the flag part
					extern u8 prov_auth_val;
					extern u8 prov_auth_en_flag;
					prov_auth_val= tc_par.sub_id &0x7f;
					prov_auth_en_flag =1;
					#endif
				}
				break;
			case NODE_PROV_UPD_BV_03_C:
				set_node_prov_para_no_pubkey_static_oob();
				break;
			case NODE_PROV_UPD_BV_04_C:
				set_node_prov_para_pubkey_output_oob();
				break;
		    case NODE_PROV_UPD_BV_05_C:
				set_node_prov_para_pubkey_input_oob();
				break;
			case NODE_PROV_UPD_BV_06_C:
				set_node_prov_para_pubkey_static_oob();
				break;
			case NODE_PROV_UPD_BV_07_C:
				set_node_prov_para_no_pubkey_no_oob();
				break;
			case NODE_PROV_UPD_BV_08_C:
				set_node_prov_para_pubkey_no_oob();
				break;
			case NODE_PROV_UPD_BV_12_C:{
				#if (!__PROJECT_MESH_PRO__)
				extern u8 dev_dpk[];
				dev_dpk[0] += 1;
				#endif
				}
				break;
			case PVNR_PROV_PVN_BV_01_C:
				#if __PROJECT_MESH_PRO__
				{
					extern u8 prov_auth_en_flag ;
					extern u8 prov_auth_val ;
					prov_auth_en_flag = 1;
					prov_auth_val = tc_par.sub_id;
				}
				#endif
				break;
			case PVNR_PROV_PVN_BV_04_C:
				#if __PROJECT_MESH_PRO__
				{
					extern u8 prov_auth_en_flag ;
					extern u32 prov_oob_output_auth ;
					prov_auth_en_flag = 1;
					prov_oob_output_auth = *(u32 *)(&tc_par.sub_id);
				}
				#endif
				break;
			/*--------------------NETWORK LAYER----------------------*/
            case NODE_NET_BV_01:  
				model_sig_cfg_s.ttl_def = tc_par.sub_id;	
				test_case_tx_unseg_noack(0x0001, 0); 
				break;
            case NODE_NET_BV_02:
				model_sig_cfg_s.ttl_def = tc_par.sub_id;
				test_case_tx_unseg_noack(0x8000, 0);
				break;
            case NODE_NET_BV_03:
				model_sig_cfg_s.ttl_def = tc_par.sub_id;
				test_case_tx_unseg_noack(0xC000, 0);
				break;
            case NODE_NET_BV_04:
				model_sig_cfg_s.ttl_def = tc_par.sub_id;
				test_case_tx_unseg_noack(0xFFFC, 0);
				break;
            case NODE_NET_BV_05:
				model_sig_cfg_s.ttl_def = tc_par.sub_id;
				test_case_tx_unseg_noack(0xFFFD, 0);
				break;
            case NODE_NET_BV_06:
				model_sig_cfg_s.ttl_def = tc_par.sub_id;
				test_case_tx_unseg_noack(0xFFFE, 0);
				break;
            case NODE_NET_BV_07:
				model_sig_cfg_s.ttl_def = tc_par.sub_id;
				test_case_tx_unseg_noack(0xFFFF, 0);
				break;
			/*--------------------transport layer----------------------*/
			case NODE_TNPT_BV_03:
				test_case_tx_unseg_noack(0x0001, 1);
				break;
			case NODE_TNPT_BV_04:
				test_case_tx_seg_noack(0x0001, 1);
				break;			
			case NODE_TNPT_BV_05:
				if(tc_par.sub_id == 0){
					test_case_tx_seg_noack(0xD000, 1);
				}
				else if(tc_par.sub_id == 1){					
					mesh_tx_cmd2uuid(G_ONOFF_SET_NOACK, (u8 *)tc_mesh_cmd, 19, ele_adr_primary, 0, 0, prov_para.device_uuid);
				}				
				break;
			/*--------------------IV UPDATE----------------------*/
			case NODE_IVU_BV_02:
				if(tc_par.sub_id == 1){
					mesh_iv_update_enter_update_progress();
				}else if(tc_par.sub_id == 2){
					iv_idx_update_change2next_st = 1;
				}
				break;
			case NODE_IVU_BI_05:
				if(tc_par.sub_id == 1){
					iv_update_test_mode_en = 0;
				}
				break;
			case NODE_IVU_BI_06:
				if(tc_par.sub_id == 1){
					mesh_iv_update_enter_update_progress();
				}
				break;
			/*-----------------------Key Refresh--------------------------*/
			case CFGCL_CFG_KR_PTS_1:
			case CFGCL_CFG_KR_PTS_2:
				{
				u16 adr_dst = (CFGCL_CFG_KR_PTS_1 == tc_par.tc_id) ? 0x0001 : 0x0002;
				mesh_kr_cfgcl_start(adr_dst);
				key_refresh_cfgcl_proc.st = tc_par.sub_id;
				mesh_kr_cfgcl_proc();
				}
				break;
			/*-----------------------Node identity--------------------------*/
			case CFGCL_CFG_NID_BV_01:
				if(tc_par.sub_id == 1){
					cfg_cmd_node_identity_set(0x0001, 0x0000, NODE_IDENTITY_SUBNET_SUPPORT_DISABLE);
				}
				else if(tc_par.sub_id == 2){
					cfg_cmd_node_identity_set(0x0001, 0x0000, NODE_IDENTITY_SUBNET_SUPPORT_ENABLE);
				}
				else if(tc_par.sub_id == 3){
					cfg_cmd_node_identity_get(0x0001, 0x0000);
				}
				break;
			case NODE_CFG_NID_BV_02:
				if(tc_par.sub_id == 1){
					foreach(i,NET_KEY_MAX){
					mesh_key.net_key[i][0].node_identity =1;
					}
				}else if (tc_par.sub_id == 2){
					foreach(i,NET_KEY_MAX){
					mesh_key.net_key[i][0].node_identity =0;
					}					
				}
				break;
			case NODE_CFG_NID_BI_02:
				foreach(i,NET_KEY_MAX){
					if(mesh_key.net_key[i][0].valid){
						mesh_key.net_key[i][0].node_identity = NODE_IDENTITY_SUBNET_NOT_SUPPORT;
					}
				}
				break;
			/*--------------------configuration model----------------------*/
			case CFGC_CFG_RST:
				kick_out();
				break;
			case CFGCL_CFG_SNBP_BV_01:
				if(tc_par.sub_id == 0){
					mesh_tx_cmd2normal_primary(CFG_BEACON_GET, 0, 0, 0x01, 0x01);
				}
				else if(tc_par.sub_id == 1){
					u8 tc_beacon = 0; //sec_nw_beacon
					mesh_tx_cmd2normal_primary(CFG_BEACON_SET, (u8 *)&tc_beacon, 1, 0x01, 0x01);
				}
				else if(tc_par.sub_id == 2){
					u8 tc_beacon = 1; //sec_nw_beacon
					mesh_tx_cmd2normal_primary(CFG_BEACON_SET, (u8 *)&tc_beacon, 1, 0x01, 0x01);
				}
				break;
			case CFGCL_CFG_COMP_BV_01:{
				u8 tc_page = 0xFF;
				mesh_tx_cmd2normal_primary(COMPOSITION_DATA_GET, (u8 *)&tc_page, 1, 0x01, 0x01);}
				break;
			case CFGCL_CFG_DTTL_BV_01:
				if(tc_par.sub_id == 0){
					mesh_tx_cmd2normal_primary(CFG_DEFAULT_TTL_GET, 0, 0, 0x01, 0x01);
				}
				else if(tc_par.sub_id == 1){
					u8 tc_ttl = 0x03; //ttl_def
					mesh_tx_cmd2normal_primary(CFG_DEFAULT_TTL_SET, (u8 *)&tc_ttl, 1, 0x01, 0x01);
				}
				break;
			case NODE_CFG_GPXY_BV_01:
				model_sig_cfg_s.gatt_proxy = tc_par.sub_id;
				break;
			case CFGCL_CFG_GPXY_BV_01:
				if(tc_par.sub_id == 0){
					mesh_tx_cmd2normal_primary(CFG_GATT_PROXY_GET, 0, 0, 0x01, 0x01);
				}
				else if(tc_par.sub_id == 1){
					u8 tc_proxy = GATT_PROXY_SUPPORT_ENABLE;//gatt_proxy = GATT_PROXY_SUPPORT_ENABLE;
					mesh_tx_cmd2normal_primary(CFG_GATT_PROXY_SET, (u8 *)&tc_proxy, 1, 0x01, 0x01);
				}
				break;
			case NODE_CFG_CFGF_BV_01:
				model_sig_cfg_s.frid = tc_par.sub_id;
				break;
			case CFGCL_CFG_CFGF_BV_01:
				if(tc_par.sub_id == 0){
					mesh_tx_cmd2normal_primary(CFG_FRIEND_GET, 0, 0, 0x01, 0x01);
				}
				else if(tc_par.sub_id == 1){
					u8 tc_friend = 0x01;//model_sig_cfg_s.frid
					mesh_tx_cmd2normal_primary(CFG_FRIEND_SET, (u8 *)&tc_friend, 1, 0x01, 0x01);
				}
				break;
			case NODE_CFG_CFGR_BV_01:
				model_sig_cfg_s.relay = tc_par.sub_id;
				if(model_sig_cfg_s.relay == 2){
					model_sig_cfg_s.relay_retransmit.val = 0;
				}
				break;
			case CFGCL_CFG_CFGR_BV_01:
				if(tc_par.sub_id == 0){
					mesh_tx_cmd2normal_primary(CFG_RELAY_GET, 0, 0, 0x01, 0x01);
				}
				else if(tc_par.sub_id == 1){
					mesh_cfg_model_relay_set_t relay_set;
					relay_set.relay= 0x01;//relay = 0x01;
					relay_set.transmit.count = 0x00;//relay_retransmit=relay_retransmit=0;
					relay_set.transmit.invl_steps= 0x00;
					mesh_tx_cmd2normal_primary(CFG_RELAY_SET, (u8 *)&relay_set, sizeof(mesh_cfg_model_relay_set_t), 0x01, 0x01);
				}
				break;
			case CFGCL_CFG_MP_BV_01:
				if(tc_par.sub_id == 0){
					mesh_cfg_model_pub_get_t par = {0};
					par.ele_adr = 0x01;
					par.model_id = 0x02;
					mesh_tx_cmd2normal_primary(CFG_MODEL_PUB_GET, (u8 *)&par, sizeof(mesh_cfg_model_pub_get_t), 0x01, 0x01);
				}
				else if(tc_par.sub_id == 1 || tc_par.sub_id == 3){
					mesh_cfg_model_pub_set_t pub_set = {0};
					pub_set.ele_adr = 0x01;
					if(tc_par.sub_id == 1){
						pub_set.pub_adr = ele_adr_primary;
					}
					else{
						pub_set.pub_adr = 0xC000;
					}
					pub_set.pub_par.appkey_idx = 0;
					pub_set.pub_par.credential_flag = 0;
					pub_set.pub_par.rfu = 0;
					pub_set.pub_par.ttl = 0x3F;
					pub_set.pub_par.period.res= 1;
					pub_set.pub_par.period.steps = 0x0F;
					pub_set.pub_par.transmit.count= 0;
					pub_set.pub_par.transmit.invl_steps = 0;
					pub_set.model_id = 0x02;
					mesh_tx_cmd2normal_primary(CFG_MODEL_PUB_SET, (u8 *)&pub_set, sizeof(mesh_cfg_model_pub_set_t), 0x01, 0x01);
				}
				else if(tc_par.sub_id == 2){
					mesh_cfg_model_pub_virtual_set_t pub_set ={0};
					pub_set.ele_adr = 0x01;
					memcpy(pub_set.pub_uuid, tc_mesh_cmd, 16);
					pub_set.pub_par.appkey_idx = 0;
					pub_set.pub_par.credential_flag = 0;
					pub_set.pub_par.rfu = 0;
					pub_set.pub_par.ttl = 0x3F;
					pub_set.pub_par.period.res= 1;
					pub_set.pub_par.period.steps = 0x0F;
					pub_set.pub_par.transmit.count= 0;
					pub_set.pub_par.transmit.invl_steps = 0;
					pub_set.model_id = 0x02;
					mesh_tx_cmd2normal_primary(CFG_MODEL_PUB_VIRTUAL_ADR_SET, (u8 *)&pub_set, sizeof(mesh_cfg_model_pub_virtual_set_t), 0x01, 0x01);
				}
				break;
			case CFGCL_CFG_NTX_BV_01:
				if(tc_par.sub_id == 0){
					mesh_tx_cmd2normal_primary(CFG_NW_TRANSMIT_GET, 0, 0, 0x01, 0x01);
				}
				else if(tc_par.sub_id == 1){				
					mesh_tx_cmd2normal_primary(CFG_NW_TRANSMIT_SET, (u8 *)&model_sig_cfg_s.nw_transmit, sizeof(mesh_transmit_t), 0x01, 0x01);
				}
				break;
			case CFGCL_CFG_SL_BV_01:
				if(tc_par.sub_id == 0){
					mesh_cfg_model_sub_set_t sub_set;
					sub_set.ele_adr = 0x01;
					sub_set.model_id = VENDOR_MD_LIGHT_S;
					sub_set.sub_adr = 0xC000;
					mesh_tx_cmd2normal_primary(CFG_MODEL_SUB_ADD, (u8 *)&sub_set, sizeof(mesh_cfg_model_sub_set_t), 0x01, 0x01);
				}
				else if(tc_par.sub_id == 1){	
					mesh_cfg_model_sub_set_t sub_set;
					sub_set.ele_adr = 0x01;
					sub_set.model_id = VENDOR_MD_LIGHT_S;
					sub_set.sub_adr = 0xC000;
					mesh_tx_cmd2normal_primary(CFG_MODEL_SUB_DEL, (u8 *)&sub_set, sizeof(mesh_cfg_model_sub_set_t), 0x01, 0x01);
				}
				else if(tc_par.sub_id == 2){
					mesh_cfg_model_sub_set_t sub_set;
					sub_set.ele_adr = 0x01;
					sub_set.model_id = VENDOR_MD_LIGHT_S;
					sub_set.sub_adr = 0xC000;
					mesh_tx_cmd2normal_primary(CFG_MODEL_SUB_OVER_WRITE, (u8 *)&sub_set, sizeof(mesh_cfg_model_sub_set_t), 0x01, 0x01);
				}
				else if(tc_par.sub_id == 3){
					mesh_cfg_model_sub_del_all_t sub_set;
					sub_set.ele_adr = 0x01;
					sub_set.model_id = VENDOR_MD_LIGHT_S;
					mesh_tx_cmd2normal_primary(CFG_MODEL_SUB_DEL_ALL, (u8 *)&sub_set, sizeof(mesh_cfg_model_sub_del_all_t), 0x01, 0x01);
				}
				else if(tc_par.sub_id == 4){
					mesh_cfg_model_sub_get_sig_t sub_get;
					sub_get.ele_adr = 0x01;
					sub_get.model_id = 0x0002;
					mesh_tx_cmd2normal_primary(CFG_SIG_MODEL_SUB_GET, (u8 *)&sub_get, sizeof(mesh_cfg_model_sub_get_sig_t), 0x01, 0x01);
				}
				else if(tc_par.sub_id == 5){
					mesh_cfg_model_sub_get_vendor_t sub_set;
					sub_set.ele_adr = 0x01;
					sub_set.model_id = VENDOR_MD_LIGHT_S;
					mesh_tx_cmd2normal_primary(CFG_VENDOR_MODEL_SUB_GET, (u8 *)&sub_set, sizeof(mesh_cfg_model_sub_get_vendor_t), 0x01, 0x01);
				}
				break;
			case CFGC_CFG_SL_BV_02:
				if(tc_par.sub_id == 0){
					mesh_cfg_model_sub_virtual_set_t set_vr;
					set_vr.ele_adr = 0x01;
					set_vr.model_id = VENDOR_MD_LIGHT_S;
					memcpy(set_vr.sub_uuid, tc_mesh_cmd, 16);
					mesh_tx_cmd2normal_primary(CFG_MODEL_SUB_VIRTUAL_ADR_ADD, (u8 *)&set_vr, sizeof(mesh_cfg_model_sub_virtual_set_t), 0x01, 0x01);
				}
				else if(tc_par.sub_id == 1){	
					mesh_cfg_model_sub_virtual_set_t set_vr;
					set_vr.ele_adr = 0x01;
					set_vr.model_id = VENDOR_MD_LIGHT_S;
					memcpy(set_vr.sub_uuid, tc_mesh_cmd, 16);
					mesh_tx_cmd2normal_primary(CFG_MODEL_SUB_VIRTUAL_ADR_DEL, (u8 *)&set_vr, sizeof(mesh_cfg_model_sub_virtual_set_t), 0x01, 0x01);
				}
				else if(tc_par.sub_id == 2){
					mesh_cfg_model_sub_virtual_set_t set_vr;
					set_vr.ele_adr = 0x01;
					set_vr.model_id = VENDOR_MD_LIGHT_S;
					memcpy(set_vr.sub_uuid, tc_mesh_cmd, 16);
					mesh_tx_cmd2normal_primary(CFG_MODEL_SUB_VIRTUAL_ADR_OVER_WRITE, (u8 *)&set_vr, sizeof(mesh_cfg_model_sub_virtual_set_t), 0x01, 0x01);
				}
				break;
			/*--------------------net key list----------------------*/
			case NODE_CFG_NKL_BI_03:
				netkey_list_test_mode_en = 1;
				break;
			case CFGC_CFG_NKL_BV_01:
				if(tc_par.sub_id == 1){
					test_case_netkey_set(NETKEY_ADD);
				}else if(tc_par.sub_id == 2){
					test_case_netkey_get();
				}
				break;   
			case CFGC_CFG_NKL_BV_02:
				test_case_netkey_set(NETKEY_UPDATE);
				break;   
			case CFGC_CFG_NKL_BV_03:
				test_case_netkey_set(NETKEY_DEL);
				break;
			case CFGC_CFG_AKL_BV_01:
				test_case_appkey_set(APPKEY_ADD);
				break;
			case CFGC_CFG_AKL_BV_02:
				test_case_appkey_set(APPKEY_UPDATE);
				break;
			case CFGC_CFG_AKL_BV_03:
				test_case_appkey_set(APPKEY_DEL);
				break;
			case CFGC_CFG_AKL_BV_04:
				test_case_appkey_get();
				break;
			/*--------------------model to appkey list procedures------------*/
			case CFGCL_CFG_MAKL_BV_01:{
				mesh_app_bind_t bind;
				bind.ele_adr = 0x01;
				bind.index = 0;
				bind.model_id = VENDOR_MD_LIGHT_S;
				mesh_tx_cmd2normal_primary(MODE_APP_BIND, (u8 *)&bind, sizeof(mesh_app_bind_t), 0x01, 0x01);}
				break;
			case CFGCL_CFG_MAKL_BV_02:{
				mesh_app_bind_t bind;
				bind.ele_adr = 0x01;
				bind.index = 0;
				bind.model_id = VENDOR_MD_LIGHT_S;
				mesh_tx_cmd2normal_primary(MODE_APP_UNBIND, (u8 *)&bind, sizeof(mesh_app_bind_t), 0x01, 0x01);}
				break;
			case CFGC_CFG_MAKL_BV_03:
				cfg_cmd_sig_model_app_get(0x01, 0x01, 2);
				break;
			case CFGC_CFG_MAKL_BV_04:
				cfg_cmd_vd_model_app_get(0x01, 0x01, 0);
				break;
			case CFGC_CFG_RST_BV_01:
				cfg_cmd_reset_node(0x0001);
				break;
			case PVNR_PBADV_BV_01:
				mesh_send_provison_data(BEARS_CTL,LINK_CLOSE,0,0);
        		send_rcv_retry_set(PRO_BEARS_CTL,LINK_CLOSE,0);		
				break; 
			case CFGCL_CFG_HBP_BV_01:
				if((tc_par.sub_id == 0) || (tc_par.sub_id > 1)){
					mesh_cfg_model_heartbeat_pub_get_t pub_set;
					if(tc_par.sub_id == 0){
						pub_set.dst = 0x0002;
					}else if(tc_par.sub_id == 2){
						pub_set.dst = 0x0000;
					}else if(tc_par.sub_id == 3){
						pub_set.dst = 0xC000;
					}
					pub_set.count_log = 0x11;
					pub_set.period_log = 0x02;
					pub_set.ttl = 0x7F;
					pub_set.features = 0x0000;
					pub_set.netkeyindex = 0;
					mesh_tx_cmd2normal_primary(HEARTBEAT_PUB_SET, (u8 *)&pub_set, sizeof(mesh_cfg_model_heartbeat_pub_get_t), 0x01, 0x01);
				}else if(tc_par.sub_id == 1){
					mesh_tx_cmd2normal_primary(HEARTBEAT_PUB_GET, 0, 0, 0x01, 0x01);
				}				
				break;
			case CFGCL_CFG_HBS_BV_01:
				if((tc_par.sub_id == 0) || (tc_par.sub_id > 1)){
					mesh_cfg_model_heartbeat_sub_set_t sub_set;
					sub_set.src = 0x01;
					if(tc_par.sub_id == 0){
						sub_set.dst = ele_adr_primary;
					}else if(tc_par.sub_id == 2){
						sub_set.dst = 0x0000;
					}else if(tc_par.sub_id == 3){
						sub_set.dst = 0xC002;
					}					
					sub_set.period_log = 0x11;
					mesh_tx_cmd2normal_primary(HEARTBEAT_SUB_SET, (u8 *)&sub_set, sizeof(mesh_cfg_model_heartbeat_sub_set_t), 0x01, 0x01);
				}else if(tc_par.sub_id == 1){
					mesh_tx_cmd2normal_primary(HEARTBEAT_SUB_GET, 0, 0, 0x01, 0x01);
				}
				break;
			/*--------------------LPN poll timeout-----------------------*/
			case CFGC_CFG_LPNPT_BV_01:
				cfg_cmd_lpn_poll_timeout_get(0x0001, 0x1234);
				break;
			/*--------------------health model-----------------------*/
			case SR_HM_CFS_BV_01:
				memset(model_sig_health.srv.health_mag.cur_sts.fault_array, 0x00, sizeof(model_sig_health.srv.health_mag.cur_sts.fault_array));
				model_sig_health.srv.health_mag.cur_sts.cur_fault_idx = 0;
				break;
			case SR_HM_CFS_BV_02:
				memcpy(model_sig_health.srv.health_mag.cur_sts.fault_array, tc_mesh_cmd, 0x04);
				model_sig_health.srv.health_mag.cur_sts.cur_fault_idx = 4;
				break;
			case SR_HM_RFS_BV_01:
				memset(model_sig_health.srv.health_mag.fault_sts.fault_array, 0x00, sizeof(model_sig_health.srv.health_mag.fault_sts.fault_array));
				model_sig_health.srv.health_mag.fault_sts.cur_fault_idx = 0;
				break;
			case SR_HM_RFS_BV_02:
				memcpy(model_sig_health.srv.health_mag.fault_sts.fault_array, tc_mesh_cmd, 0x04);
				model_sig_health.srv.health_mag.fault_sts.cur_fault_idx = 4;
				break;
			case CL_HM_RFS_BV_01:
				if(tc_par.sub_id == 0){
					u16 tc_company_id = 0x003f;
					mesh_tx_cmd2normal_primary(HEALTH_FAULT_GET, (u8 *)&tc_company_id, sizeof(tc_company_id), 0x01, 0x01);
				}
				else if(tc_par.sub_id == 1){
					u16 tc_company_id = 0x003f;
					mesh_tx_cmd2normal_primary(HEALTH_FAULT_CLEAR, (u8 *)&tc_company_id, sizeof(tc_company_id), 0x01, 0x01);
				}
				else if(tc_par.sub_id == 2){
					u16 tc_company_id = 0x003f;
					mesh_tx_cmd2normal_primary(HEALTH_FAULT_CLEAR_NOACK, (u8 *)&tc_company_id, sizeof(tc_company_id), 0x01, 0x01);
				}
				else if(tc_par.sub_id == 3){
					mesh_health_fault_test_t fault_test ;
					fault_test.company_id = 0x003f;
					fault_test.test_id = 0x00;
					mesh_tx_cmd2normal_primary(HEALTH_FAULT_TEST, (u8 *)&fault_test, sizeof(fault_test), 0x01, 0x01);
				}
				else if(tc_par.sub_id == 4){
					mesh_health_fault_test_t fault_test ;
					fault_test.company_id = 0x003f;
					fault_test.test_id = 0x00;
					mesh_tx_cmd2normal_primary(HEALTH_FAULT_TEST_NOACK, (u8 *)&fault_test, sizeof(fault_test), 0x01, 0x01);
				}
				break;
			case CL_HM_HPS_BV_01:
				if(tc_par.sub_id == 0){
					mesh_tx_cmd2normal_primary(HEALTH_PERIOD_GET, 0, 0, 0x01, 0x01);
				}
				else if(tc_par.sub_id == 1){
					u8 tc_fast_period = 1;
					mesh_tx_cmd2normal_primary(HEALTH_PERIOD_SET, (u8 *)&tc_fast_period, sizeof(tc_fast_period), 0x01, 0x01);
				}
				else if(tc_par.sub_id == 2){
					u8 tc_fast_period = 1;
					mesh_tx_cmd2normal_primary(HEALTH_PERIOD_SET_NOACK, (u8 *)&tc_fast_period, sizeof(tc_fast_period), 0x01, 0x01);
				}
				break;
			case CL_HM_ATS_BV_01:
				if(tc_par.sub_id == 0){
					mesh_tx_cmd2normal_primary(HEALTH_ATTENTION_GET, 0, 0, 0x01, 0x01);
				}
				else if(tc_par.sub_id == 1){
					u8 tc_attention = model_sig_health.srv.health_mag.attention_timer;
					mesh_tx_cmd2normal_primary(HEALTH_ATTENTION_SET, (u8 *)&tc_attention, sizeof(tc_attention), 0x01, 0x01);
				}
				else if(tc_par.sub_id == 2){
					u8 tc_attention = model_sig_health.srv.health_mag.attention_timer;
					mesh_tx_cmd2normal_primary(HEALTH_ATTENTION_SET_NOACK, (u8 *)&tc_attention, sizeof(tc_attention), 0x01, 0x01);
				}
				break;
			/*-------------------Generic Client Models------------------*/
			case CL_GOO_BV_01:
				access_cmd_onoff_get(0x01, 0x01);
				break;
			case CL_GOO_BV_02:
				access_cmd_onoff(0x01, 1, 1, 1, 0);
				break;
			case CL_GOO_BV_03:{
				transition_par_t par;
				par.transit_t = 0x00;
				par.delay = 0;
				access_cmd_onoff(0x01, 1, G_ON, CMD_ACK, &par);}
				break;
			case CL_GOO_BV_04:{
				transition_par_t par;
				par.transit_t = 0x54;
				par.delay = 0x05;
				access_cmd_onoff(0x01, 1, G_ON, CMD_ACK, &par);}
				break;
			case CL_GOO_BV_05:
				access_cmd_onoff(0x01, 0, 0, 0, 0);
				break;
			case CL_GOO_BV_06:{
				transition_par_t par;
				par.transit_t = 0x00;
				par.delay = 0x00;
				access_cmd_onoff(0x01, 0, G_OFF, CMD_NO_ACK, &par);}
				break;
			case CL_GOO_BV_07:{
				transition_par_t par;
				par.transit_t = 0x54;
				par.delay = 0x05;
				access_cmd_onoff(0x01, 0, G_OFF, CMD_NO_ACK, &par);}
				break;
			case CL_GLV_BV_01:
				access_cmd_get_level(0x01, 1);
				break;
			case CL_GLV_BV_02:
				access_cmd_set_level(0x01, 1, 0x1ff, 1, 0);
				break;
			case CL_GLV_BV_03:{
				transition_par_t par;
				par.transit_t = 0xc0;
				par.delay = 0x00;
				access_cmd_set_level(0x01, 1, 0x1ff, CMD_ACK, &par);}
				break;
			case CL_GLV_BV_04:{
				transition_par_t par;
				par.transit_t = 0x54;
				par.delay = 0x05;
				access_cmd_set_level(0x01, 1, 0x1ff, CMD_ACK, &par);}
				break;
			case CL_GLV_BV_05:
				access_cmd_set_level(0x01, 0, 0x1ff, 0, 0);
				break;
			case CL_GLV_BV_06:{
				transition_par_t par;
				par.transit_t = 0xc0;
				par.delay = 0x00;
				access_cmd_set_level(0x01, 0, 0x1ff, CMD_NO_ACK, &par);}
				break;
			case CL_GLV_BV_07:{
				transition_par_t par;
				par.transit_t = 0x54;
				par.delay = 0x05;
				access_cmd_set_level(0x01, 0, 0x1ff, CMD_NO_ACK, &par);}
				break;
			case CL_GLV_BV_08:{
				transition_par_t par;
				par.transit_t = 0xc0;
				par.delay = 0x00;
				access_cmd_set_delta(0x01, 1, 0x1ff, CMD_ACK, &par);}
				break;
			case CL_GLV_BV_09:{
				transition_par_t par;
				par.transit_t = 0xc0;
				par.delay = 0x00;
				access_cmd_set_delta(0x01, 1, 0x1ff, CMD_ACK, &par);}
				break;
			case CL_GLV_BV_10:{
				transition_par_t par;
				par.transit_t = 0x54;
				par.delay = 0x05;
				access_cmd_set_delta(0x01, 1, 0x1ff, CMD_ACK, &par);}
				break;
			case CL_GLV_BV_11:{
				transition_par_t par;
				par.transit_t = 0x54;
				par.delay = 0x05;
				access_cmd_set_delta(0x01, 1, 0x1ff, CMD_NO_ACK, &par);}
				break;
			case CL_GLV_BV_12:{
				transition_par_t par;
				par.transit_t = 0xc0;
				par.delay = 0x00;
				access_cmd_set_delta(0x01, 1, 0x1ff, CMD_NO_ACK, &par);}
				break;
			case CL_GLV_BV_13:{
				transition_par_t par;
				par.transit_t = 0x54;
				par.delay = 0x05;
				access_cmd_set_delta(0x01, 1, 0x1ff, CMD_NO_ACK, &par);}
				break;
			case CL_GLV_BV_14:{
				mesh_cmd_g_level_set_t par ;
				par.level = 0x1ff;
				mesh_tx_cmd2normal_primary(G_MOVE_SET, (u8 *)&par, OFFSETOF(mesh_cmd_g_level_set_t, transit_t), 0x01, 0x01);
				}
				break;
			case CL_GLV_BV_15:{
				mesh_cmd_g_level_set_t par ;
				par.level = 0x1ff;
				mesh_tx_cmd2normal_primary(G_MOVE_SET_NOACK, (u8 *)&par, OFFSETOF(mesh_cmd_g_level_set_t, transit_t), 0x01, 0x00);
				}
				break;
			//generic default transation time cloeent
			case CL_GDTT_BV_01:
				mesh_tx_cmd2normal_primary(G_DEF_TRANS_TIME_GET, 0, 0, 0x01, 0x01);
				break;
			case CL_GDTT_BV_02:{
				trans_time_t par;
				par.val = 0xcf;
				mesh_tx_cmd2normal_primary(G_DEF_TRANS_TIME_SET, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			case CL_GDTT_BV_03:{
				trans_time_t par;
				par.val = 0x8f;
				mesh_tx_cmd2normal_primary(G_DEF_TRANS_TIME_SET_NOACK, (u8 *)&par, sizeof(par), 0x01, 0x00);
				}
				break;
			//generic power onoff client
			case CL_GPOO_BV_01:				
				mesh_tx_cmd2normal_primary(G_ON_POWER_UP_GET, 0, 0, 0x01, 0x01);
				break;
			case CL_GPOO_BV_02:{
				u8 par = 0x01;
				mesh_tx_cmd2normal_primary(G_ON_POWER_UP_SET, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			case CL_GPOO_BV_03:{
				u8 par = 0x02;
				mesh_tx_cmd2normal_primary(G_ON_POWER_UP_SET_NOACK, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			//generic power level client
			case CL_GPL_BV_01:
				mesh_tx_cmd2normal_primary(G_POWER_LEVEL_GET, 0, 0, 0x01, 0x01);				
				break;
			case CL_GPL_BV_02:{					
				mesh_cmd_lightness_set_t par;
				par.lightness = 0x01ff;
				mesh_tx_cmd2normal_primary(G_POWER_LEVEL_SET, (u8 *)&par, OFFSETOF(mesh_cmd_lightness_set_t, transit_t), 0x01, 0x01);
				}
				break;
			case CL_GPL_BV_03:{					
				mesh_cmd_lightness_set_t par;
				par.lightness = 0x01ff;
				par.transit_t = 0xc0;
				par.delay = 0x00;
				mesh_tx_cmd2normal_primary(G_POWER_LEVEL_SET, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			case CL_GPL_BV_04:{					
				mesh_cmd_lightness_set_t par;
				par.lightness = 0x01ff;
				par.transit_t = 0x54;
				par.delay = 0x05;
				mesh_tx_cmd2normal_primary(G_POWER_LEVEL_SET, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			case CL_GPL_BV_05:{					
				mesh_cmd_lightness_set_t par;
				par.lightness = 0x01ff;
				mesh_tx_cmd2normal_primary(G_POWER_LEVEL_SET_NOACK, (u8 *)&par, OFFSETOF(mesh_cmd_lightness_set_t, transit_t), 0x01, 0x00);
				}
				break;
			case CL_GPL_BV_06:{					
				mesh_cmd_lightness_set_t par;
				par.lightness = 0x01ff;
				par.transit_t = 0xc0;
				par.delay = 0x00;
				mesh_tx_cmd2normal_primary(G_POWER_LEVEL_SET_NOACK, (u8 *)&par, sizeof(par), 0x01, 0x00);
				}
				break;
			case CL_GPL_BV_07:{					
				mesh_cmd_lightness_set_t par;
				par.lightness = 0x01ff;
				par.transit_t = 0x54;
				par.delay = 0x05;
				mesh_tx_cmd2normal_primary(G_POWER_LEVEL_SET_NOACK, (u8 *)&par, sizeof(par), 0x01, 0x00);
				}
				break;
			case CL_GPL_BV_08:
				mesh_tx_cmd2normal_primary(G_POWER_LEVEL_LAST_GET, 0, 0, 0x01, 0x01);
				break;
			case CL_GPL_BV_09:
				mesh_tx_cmd2normal_primary(G_POWER_DEF_GET, 0, 0, 0x01, 0x01);
				break;
			case CL_GPL_BV_10:{					
				u16 par = 0x4567;
				mesh_tx_cmd2normal_primary(G_POWER_DEF_SET, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			case CL_GPL_BV_11:{					
				u16 par = 0x3467;
				mesh_tx_cmd2normal_primary(G_POWER_DEF_SET_NOACK, (u8 *)&par, sizeof(par), 0x01, 0x00);
				}
				break;
			case CL_GPL_BV_12:
				mesh_tx_cmd2normal_primary(G_POWER_LEVEL_RANGE_GET, 0, 0, 0x01, 0x01);
				break;
			case CL_GPL_BV_13:{
				mesh_cmd_lightness_range_set_t par;
				par.min = 0x10;
				par.max = 0x1000;
				mesh_tx_cmd2normal_primary(G_POWER_LEVEL_RANGE_SET, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			case CL_GPL_BV_14:{
				mesh_cmd_lightness_range_set_t par;
				par.min = 0xf0;
				par.max = 0xf000;
				mesh_tx_cmd2normal_primary(G_POWER_LEVEL_RANGE_SET_NOACK, (u8 *)&par, sizeof(par), 0x01, 0x00);
				}
				break;
			/*-------------------battery Client Models------------------*/
			case CL_GBAT_BV_01:
				mesh_tx_cmd2normal_primary(G_BATTERY_GET, 0, 0, 0x01, 0x01);
				break;
			//generic location client
			case CL_GLOC_BV_01:
				mesh_tx_cmd2normal_primary(G_LOCATION_GLOBAL_GET, 0, 0, 0x01, 0x01);
				break;
			case CL_GLOC_BV_02:{
				mesh_cmd_location_global_st_t par;
				par.global_latitude = 0x43C65525;
				par.global_longitude = 0xA9192E1F;
				par.global_altitude = 0x0017;
				mesh_tx_cmd2normal_primary(G_LOCATION_GLOBAL_SET, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			case CL_GLOC_BV_03:{
				mesh_cmd_location_global_st_t par;
				par.global_latitude = 0x43C65525;
				par.global_longitude = 0xA9192E1F;
				par.global_altitude = 0x0017;
				mesh_tx_cmd2normal_primary(G_LOCATION_GLOBAL_SET_NOACK, (u8 *)&par, sizeof(par), 0x01, 0x00);
				}
				break;
			case CL_GLOC_BV_04:
				mesh_tx_cmd2normal_primary(G_LOCATION_LOCAL_GET, 0, 0, 0x01, 0x01);
				break;
			case CL_GLOC_BV_05:{
				mesh_cmd_location_local_st_t par;
				par.local_north = 0x1234;
				par.local_east = 0x5678;
				par.local_altitude = 0x7FFD;
				par.floor_number = 0xFD;
				par.uncertainty = 0x5A01;
				mesh_tx_cmd2normal_primary(G_LOCATION_LOCAL_SET, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			case CL_GLOC_BV_06:{
				mesh_cmd_location_local_st_t par;
				par.local_north = 0x1234;
				par.local_east = 0x5678;
				par.local_altitude = 0x7FFD;
				par.floor_number = 0xFD;
				par.uncertainty = 0x5A01;
				mesh_tx_cmd2normal_primary(G_LOCATION_LOCAL_SET_NOACK, (u8 *)&par, sizeof(par), 0x01, 0x00);
				}
				break;
			/*-------------------generic property client------------------*/
			case CL_GPR_BV_01:				
				mesh_tx_cmd2normal_primary(G_USER_PROPERTIES_GET, 0, 0, 0x01, 0x01);
				break;
			case CL_GPR_BV_02:{
				u16 prop_id = 0x6a;
				mesh_tx_cmd2normal_primary(G_USER_PROPERTY_GET, (u8 *)&prop_id, sizeof(prop_id), 0x01, 0x01);
				}
				break;
			case CL_GPR_BV_03:{
				mesh_property_set_user_t par;
				par.id = 0x6a;
				u8 prop_value[3] = {0x12, 0x34, 0x56};
				memcpy(par.data, prop_value, 3);
				mesh_tx_cmd2normal_primary(G_USER_PROPERTY_SET, (u8 *)&par, 5, 0x01, 0x01);
				}
				break;
			case CL_GPR_BV_04:{
				mesh_property_set_user_t par;
				par.id = 0x6a;
				u8 prop_value[3] = {0x65, 0x43, 0x21};
				memcpy(par.data, prop_value, 3);
				mesh_tx_cmd2normal_primary(G_USER_PROPERTY_SET_NOACK, (u8 *)&par, 5, 0x01, 0x00);
				}
				break;
			case CL_GPR_BV_05:
				mesh_tx_cmd2normal_primary(G_ADMIN_PROPERTIES_GET, 0, 0, 0x01, 0x01);
				break;
			case CL_GPR_BV_06:{
				u16 prop_id = 0x6a;
				mesh_tx_cmd2normal_primary(G_ADMIN_PROPERTY_GET, (u8 *)&prop_id, sizeof(prop_id), 0x01, 0x01);
				}
				break;
			case CL_GPR_BV_07:{
				mesh_property_t par;
				par.len_val = 3;
				par.id = 0x6a;
				par.access = 0x01;
				u8 prop_val[3] = {0x16, 0x25, 0x34};
				memcpy(par.val, prop_val, par.len_val);
				mesh_tx_cmd2normal_primary(G_ADMIN_PROPERTY_SET, (u8 *)&par.id, par.len_val+3, 0x01, 0x01);
				}
				break;
			case CL_GPR_BV_08:{
				mesh_property_t par;
				par.len_val = 3;
				par.id = 0x6a;
				par.access = 0x01;
				u8 prop_val[3] = {0x34, 0x25, 0x16};
				memcpy(par.val, prop_val, par.len_val);
				mesh_tx_cmd2normal_primary(G_ADMIN_PROPERTY_SET_NOACK, (u8 *)&par.id, par.len_val+3, 0x01, 0x00);
				}
				break;
			case CL_GPR_BV_09:
				mesh_tx_cmd2normal_primary(G_MFG_PROPERTIES_GET, 0, 0, 0x01, 0x01);
				break;
			case CL_GPR_BV_10:{
				u16 prop_id = 0x4c;
				mesh_tx_cmd2normal_primary(G_MFG_PROPERTY_GET, (u8 *)&prop_id, sizeof(prop_id), 0x01, 0x01);
				}
				break;
			case CL_GPR_BV_11:{
				mesh_property_set_mfg_t par;
				par.id = 0x4c;
				par.access = 0x00;
				mesh_tx_cmd2normal_primary(G_MFG_PROPERTY_SET, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			case CL_GPR_BV_12:{
				mesh_property_set_mfg_t par;
				par.id = 0x4c;
				par.access = 0x01;
				mesh_tx_cmd2normal_primary(G_MFG_PROPERTY_SET_NOACK, (u8 *)&par, sizeof(par), 0x01, 0x00);
				}
				break;
			case CL_GPR_BV_13:{
				u16 prop_id = 0x6a;
				mesh_tx_cmd2normal_primary(G_CLIENT_PROPERTIES_GET, (u8 *)&prop_id, sizeof(prop_id), 0x01, 0x01);
				}
				break;
			/*-------------------Sensor Server Models------------------*/
			#if(MD_SENSOR_EN)
			case SR_SNR_BV_07:
				if(tc_par.sub_id == 1){
					//model_sig_sensor.sensor_states[0].sensor_data = 0;
					sensure_measure_quantity = 0;
				}
				else if(tc_par.sub_id == 2){
					//model_sig_sensor.sensor_states[0].sensor_data = 0x5000;
					sensure_measure_quantity = 0x5000;
				}
				break;
			case SR_SNR_BV_09:
				if(tc_par.sub_id == 1){
					//model_sig_sensor.sensor_states[0].sensor_data = 0x8000;
					sensure_measure_quantity = 0x100000;
				}
				else if(tc_par.sub_id == 2){
					//model_sig_sensor.sensor_states[0].sensor_data = 0x4000;
					sensure_measure_quantity = model_sig_sensor.sensor_states[0].sensor_data + 1;
				}
				else if(tc_par.sub_id == 3){
					//model_sig_sensor.sensor_states[0].sensor_data = 0x2000;
					sensure_measure_quantity = model_sig_sensor.sensor_states[0].sensor_data + 0x80000;
				}
				else if(tc_par.sub_id == 4){
					//model_sig_sensor.sensor_states[0].sensor_data = 0x2000;
					sensure_measure_quantity = model_sig_sensor.sensor_states[0].sensor_data - 1;
				}
				else if(tc_par.sub_id == 5){
					//model_sig_sensor.sensor_states[0].sensor_data = 0x2000;
					sensure_measure_quantity = model_sig_sensor.sensor_states[0].sensor_data - 0x80000;
				}
				break;
			#endif
			/*-------------------Sensor Client Models------------------*/
			case CL_SNR_BV_01:
				mesh_tx_cmd2normal_primary(SENSOR_DESCRIP_GET, 0, 0, 0x01, 0x01);
				break;
			case CL_SNR_BV_02:{
				u16 prop_id = 0x4E;
				mesh_tx_cmd2normal_primary(SENSOR_DESCRIP_GET, (u8 *)&prop_id, sizeof(prop_id), 0x01, 0x01);
				}
				break;
			case CL_SNR_BV_03:{
				u16 prop_id = 0x6F;
				mesh_tx_cmd2normal_primary(SENSOR_CANDECE_GET, (u8 *)&prop_id, sizeof(prop_id), 0x01, 0x01);
				}				
				break;
			case CL_SNR_BV_04:
			case CL_SNR_BV_05:{ // PTS variable len = 3
				sensor_cadence_st_t tc_cadence={0x4e, {0x02, 0x01}};
				tc_cadence.cadence.cadence_unitless.delta_down = 0;
				tc_cadence.cadence.cadence_unitless.delta_up = 0x64;
				tc_cadence.cadence.cadence_unitless.min_interval = 0x02;
				tc_cadence.cadence.cadence_unitless.cadence_low = 0;
				tc_cadence.cadence.cadence_unitless.cadence_hight = 0x1000;		
				mesh_tx_cmd2normal_primary((tc_par.tc_id==CL_SNR_BV_04)?SENSOR_CANDECE_SET:SENSOR_CANDECE_SET_NOACK, (u8 *)&tc_cadence, OFFSETOF(sensor_cadence_st_t, cadence.par)+sizeof(cadence_unitless_t), 0x01, 0x01);
				}
				break;
			case CL_SNR_BV_06:{
				u16 prop_id = 0x4E;
				mesh_tx_cmd2normal_primary(SENSOR_SETTINGS_GET, (u8 *)&prop_id, sizeof(prop_id), 0x01, 0x01);
				}
				break;
			case CL_SNR_BV_07:{
				sensor_setting_get_t setting_get;
				setting_get.prop_id = 0x4E;
				setting_get.setting_id = 0x2001;
				mesh_tx_cmd2normal_primary(SENSOR_SETTING_GET, (u8 *)&setting_get, sizeof(setting_get), 0x01, 0x01);}
				break;
			case CL_SNR_BV_08:
			case CL_SNR_BV_09:{
				sensor_setting_set_t setting_set;
				setting_set.prop_id = 0x4E;
				setting_set.setting_id = 0x2001;
				setting_set.setting_raw = 0x01;
				mesh_tx_cmd2normal_primary((tc_par.tc_id==CL_SNR_BV_08)?SENSOR_SETTING_SET:SENSOR_SETTING_SET_NOACK, (u8 *)&setting_set, sizeof(setting_set), 0x01, 0x01);}
				break;
			case CL_SNR_BV_10:{
				mesh_tx_cmd2normal_primary(SENSOR_GET, 0, 0, 0x01, 0x01);
				}
				break;
			case CL_SNR_BV_11:{
				u16 prop_id = 0x4E;
				mesh_tx_cmd2normal_primary(SENSOR_GET, (u8 *)&prop_id, sizeof(prop_id), 0x01, 0x01);
				}
				break;
			case CL_SNR_BV_12:{
				sensor_col_get_t col_get;
				col_get.prop_id = 0x4E;
				col_get.raw_val_X = 0x00;
				mesh_tx_cmd2normal_primary(SENSOR_COLUMN_GET, (u8 *)&col_get, sizeof(col_get), 0x01, 0x01);
				}
				break;
			case CL_SNR_BV_13:{
				sensor_series_get_t series_get;
				series_get.prop_id = 0x42;
				series_get.raw_val_x1 = 0x10;
				series_get.raw_val_x2 = 0x20;
				mesh_tx_cmd2normal_primary(SENSOR_SERIES_GET, (u8 *)&series_get, sizeof(series_get), 0x01, 0x01);
				}
				break;
			//time and scene client models
			//time client
			case CL_TIM_BV_01:
				mesh_tx_cmd2normal_primary(TIME_GET, 0, 0, 0x01, 0x01);
				break;
			case CL_TIM_BV_02:{
				time_status_t par;
				par.TAI_sec = 0;
				par.TAI_sec_rsv = 0x01;
				par.sub_sec = 0x00;
				par.uncertainty = 0x00;
				par.time_auth = 0x01;
				par.TAI_UTC_delta = 0x1234;
				par.zone_offset = 0x00;
				mesh_tx_cmd2normal_primary(TIME_SET, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			case CL_TIM_BV_03:
				mesh_tx_cmd2normal_primary(TIME_ZONE_GET, 0, 0, 0x01, 0x01);
				break;
			case CL_TIM_BV_04:{
				mesh_time_zone_set_t par;
				par.zone_offset_new = 0x00;
				u8 TAI_zone_change[5] = {0x89, 0x67, 0x45, 0x23, 0x01};
				memcpy(par.TAI_zone_change, TAI_zone_change, sizeof(par.TAI_zone_change));
				mesh_tx_cmd2normal_primary(TIME_ZONE_SET, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			case CL_TIM_BV_05:
				mesh_tx_cmd2normal_primary(TAI_UTC_DELTA_GET, 0, 0, 0x01, 0x01);
				break;
			case CL_TIM_BV_06:{
				mesh_time_TAI_UTC_delta_set_t par;
				par.delta_new = 0x1234;
				u8 TAI_delta_change[5] = {0x89, 0x67, 0x45, 0x23, 0x01};
				memcpy(par.TAI_delta_change, TAI_delta_change, sizeof(par.TAI_delta_change));
				mesh_tx_cmd2normal_primary(TAI_UTC_DELTA_SET, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			case CL_TIM_BV_07:
				mesh_tx_cmd2normal_primary(TIME_ROLE_GET, 0, 0, 0x01, 0x01);
				break;
			case CL_TIM_BV_08:{
				u8 role = 0x03;
				mesh_tx_cmd2normal_primary(TIME_ROLE_SET, (u8 *)&role, sizeof(role), 0x01, 0x01);
				}
				break;
			//scene client
			case CL_SCE_BV_01:
				mesh_tx_cmd2normal_primary(SCENE_GET, 0, 0, 0x01, 0x01);				
				break;
			case CL_SCE_BV_02:
				mesh_tx_cmd2normal_primary(SCENE_REG_GET, 0, 0, 0x01, 0x01);	
				break;
			case CL_SCE_BV_03:{
				u16 sce_id = 1;
				mesh_tx_cmd2normal_primary(SCENE_STORE, (u8 *)&sce_id, sizeof(sce_id), 0x01, 0x01);
				}
				break;
			case CL_SCE_BV_04:{
				u16 sce_id = 1;
				mesh_tx_cmd2normal_primary(SCENE_STORE_NOACK, (u8 *)&sce_id, sizeof(sce_id), 0x01, 0x00);
				}
				break;
			case CL_SCE_BV_05:{
				scene_recall_t par;
				par.id = 1;
				mesh_tx_cmd2normal_primary(SCENE_RECALL, (u8 *)&par, OFFSETOF(scene_recall_t, transit_t), 0x01, 0x01);
				}
				break;
			case CL_SCE_BV_06:{
				scene_recall_t par;
				par.id = 1;
				par.transit_t = 0xc0;
				par.delay = 0;
				mesh_tx_cmd2normal_primary(SCENE_RECALL, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			case CL_SCE_BV_07:{
				scene_recall_t par;
				par.id = 1;
				par.transit_t = 0x7e;
				par.delay = 0;
				mesh_tx_cmd2normal_primary(SCENE_RECALL, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			case CL_SCE_BV_08:{
				scene_recall_t par;
				par.id = 1;
				mesh_tx_cmd2normal_primary(SCENE_RECALL_NOACK, (u8 *)&par, OFFSETOF(scene_recall_t, transit_t), 0x01, 0x00);
				}
				break;
			case CL_SCE_BV_09:{
				scene_recall_t par;
				par.id = 1;
				par.transit_t = 0xc0;
				par.delay = 0;
				mesh_tx_cmd2normal_primary(SCENE_RECALL_NOACK, (u8 *)&par, sizeof(par), 0x01, 0x00);
				}
				break;
			case CL_SCE_BV_10:{
				scene_recall_t par;
				par.id = 1;
				par.transit_t = 0x7e;
				par.delay = 0;
				mesh_tx_cmd2normal_primary(SCENE_RECALL_NOACK, (u8 *)&par, sizeof(par), 0x01, 0x00);
				}
				break;
			//scheduler client
			case CL_SCH_BV_01:
				mesh_tx_cmd2normal_primary(SCHD_GET, 0, 0, 0x01, 0x01);
				break;
			case CL_SCH_BV_02:{
				u8 index = 0x01;
				mesh_tx_cmd2normal_primary(SCHD_ACTION_GET, (u8 *)&index, sizeof(index), 0x01, 0x01);
				}
				break;
			case CL_SCH_BV_03:{
				scheduler_t par;
				par.valid_flag_or_idx = 0x0f;
				par.year = 0x11;
				par.month = 0x0004;
				par.day = 0x18;
				par.hour = 0x18;
				par.minute = 0x3d;
				par.second = 0x3c;
				par.week = 0x10;
				par.action = 0x01;
				par.scene_id = 0x1234;
				par.trans_t = 0x3e;
				mesh_tx_cmd2normal_primary(SCHD_ACTION_SET, (u8 *)&par, OFFSETOF(scheduler_t,rand_hour), 0x01, 0x01);
				}
				break;
			case CL_SCH_BV_04:{
				scheduler_t par;
				par.valid_flag_or_idx = 0x0f;
				par.year = 0x11;
				par.month = 0x0004;
				par.day = 0x18;
				par.hour = 0x18;
				par.minute = 0x3d;
				par.second = 0x3c;
				par.week = 0x10;
				par.action = 0x01;
				par.scene_id = 0x1234;
				par.trans_t = 0x3e;
				mesh_tx_cmd2normal_primary(SCHD_ACTION_SET_NOACK, (u8 *)&par, OFFSETOF(scheduler_t,rand_hour), 0x01, 0x00);
				}
				break;
		//lighting client model
			//light lightness client
			case CL_LLN_BV_01:
				mesh_tx_cmd2normal_primary(LIGHTNESS_GET, 0, 0, 0x01, 0x01);
				break;
			case CL_LLN_BV_02:{
				mesh_cmd_lightness_set_t par;
				par.lightness = 0x190;
				mesh_tx_cmd2normal_primary(LIGHTNESS_SET, (u8 *)&par, OFFSETOF(mesh_cmd_lightness_set_t, transit_t), 0x01, 0x01);
				}
				break;
			case CL_LLN_BV_03:{
				mesh_cmd_lightness_set_t par;
				par.lightness = 0x12c;
				par.transit_t = 0xc0;
				par.delay = 0x00;
				mesh_tx_cmd2normal_primary(LIGHTNESS_SET, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			case CL_LLN_BV_04:{
				mesh_cmd_lightness_set_t par;
				par.lightness = 0x3e8;
				par.transit_t = 0x54;
				par.delay = 0x05;
				mesh_tx_cmd2normal_primary(LIGHTNESS_SET, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			case CL_LLN_BV_05:{
				mesh_cmd_lightness_set_t par;
				par.lightness = 0x3e8;
				mesh_tx_cmd2normal_primary(LIGHTNESS_SET_NOACK, (u8 *)&par, OFFSETOF(mesh_cmd_lightness_set_t, transit_t), 0x01, 0x00);
				}
				break;
			case CL_LLN_BV_06:{
				mesh_cmd_lightness_set_t par;
				par.lightness = 0x3e8;
				par.transit_t = 0xc0;
				par.delay = 0x00;
				mesh_tx_cmd2normal_primary(LIGHTNESS_SET_NOACK, (u8 *)&par,  sizeof(par), 0x01, 0x00);
				}
				break;
			case CL_LLN_BV_07:{
				mesh_cmd_lightness_set_t par;
				par.lightness = 0x3e8;
				par.transit_t = 0x54;
				par.delay = 0x05;
				mesh_tx_cmd2normal_primary(LIGHTNESS_SET_NOACK, (u8 *)&par,  sizeof(par), 0x01, 0x00);
				}				
				break;
			case CL_LLN_BV_08:
				mesh_tx_cmd2normal_primary(LIGHTNESS_LINEAR_GET, 0, 0, 0x01, 0x01);
				break;
			case CL_LLN_BV_09:{
				mesh_cmd_lightness_set_t par;
				par.lightness = 0x2328;
				mesh_tx_cmd2normal_primary(LIGHTNESS_LINEAR_SET, (u8 *)&par, OFFSETOF(mesh_cmd_lightness_set_t, transit_t), 0x01, 0x01);
				}
				break;
			case CL_LLN_BV_10:{
				mesh_cmd_lightness_set_t par;
				par.lightness = 0xbb8;
				par.transit_t = 0xc0;
				par.delay = 0x00;
				mesh_tx_cmd2normal_primary(LIGHTNESS_LINEAR_SET, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			case CL_LLN_BV_11:{
				mesh_cmd_lightness_set_t par;
				par.lightness = 0x3e8;
				par.transit_t = 0x54;
				par.delay = 0x05;
				mesh_tx_cmd2normal_primary(LIGHTNESS_LINEAR_SET, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			case CL_LLN_BV_12:{
				mesh_cmd_lightness_set_t par;
				par.lightness = 0x2710;
				mesh_tx_cmd2normal_primary(LIGHTNESS_LINEAR_SET_NOACK, (u8 *)&par, OFFSETOF(mesh_cmd_lightness_set_t, transit_t), 0x01, 0x00);
				}
				break;
			case CL_LLN_BV_13:{
				mesh_cmd_lightness_set_t par;
				par.lightness = 0x2710;
				par.transit_t = 0xc0;
				par.delay = 0x00;
				mesh_tx_cmd2normal_primary(LIGHTNESS_LINEAR_SET_NOACK, (u8 *)&par, sizeof(par), 0x01, 0x00);
				}				
				break;
			case CL_LLN_BV_14:{
				mesh_cmd_lightness_set_t par;
				par.lightness = 0x7530;
				par.transit_t = 0x54;
				par.delay = 0x05;
				mesh_tx_cmd2normal_primary(LIGHTNESS_LINEAR_SET_NOACK, (u8 *)&par, sizeof(par), 0x01, 0x00);
				}
				break;
			case CL_LLN_BV_15:
				mesh_tx_cmd2normal_primary(LIGHTNESS_LAST_GET, 0, 0, 0x01, 0x01);
				break;
			case CL_LLN_BV_16:
				mesh_tx_cmd2normal_primary(LIGHTNESS_DEFULT_GET, 0, 0, 0x01, 0x01);
				break;
			case CL_LLN_BV_17:{
				mesh_cmd_lightness_def_set_t par;
				par.lightness = 0x1388;
				mesh_tx_cmd2normal_primary(LIGHTNESS_DEFULT_SET, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			case CL_LLN_BV_18:{
				mesh_cmd_lightness_def_set_t par;
				par.lightness = 0x2710;
				mesh_tx_cmd2normal_primary(LIGHTNESS_DEFULT_SET_NOACK, (u8 *)&par, sizeof(par), 0x01, 0x00);
				}
				break;
			case CL_LLN_BV_19:
				mesh_tx_cmd2normal_primary(LIGHTNESS_RANGE_GET, 0, 0, 0x01, 0x01);
				break;
			case CL_LLN_BV_20:{
				mesh_cmd_lightness_range_set_t par;
				par.min = 0x64;
				par.max = 0xf000;
				mesh_tx_cmd2normal_primary(LIGHTNESS_RANGE_SET, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			case CL_LLN_BV_21:{
				mesh_cmd_lightness_range_set_t par;
				par.min = 0x64;
				par.max = 0xf000;
				mesh_tx_cmd2normal_primary(LIGHTNESS_RANGE_SET_NOACK, (u8 *)&par, sizeof(par), 0x01, 0x00);
				}
				break;
			//light CTL client
			case CL_LCTL_BV_01:
				mesh_tx_cmd2normal_primary(LIGHT_CTL_GET, 0, 0, 0x01, 0x01);
				break;
			case CL_LCTL_BV_02:{
				mesh_cmd_light_ctl_set_t par;
				par.lightness = 0x2710;
				par.temp = 0x4000;
				par.delta_uv = 0x7530;
				mesh_tx_cmd2normal_primary(LIGHT_CTL_SET, (u8 *)&par, OFFSETOF(mesh_cmd_light_ctl_set_t, transit_t), 0x01, 0x01);
				}
				break;
			case CL_LCTL_BV_03:{
				mesh_cmd_light_ctl_set_t par;
				par.lightness = 0x2710;
				par.temp = 0x4000;
				par.delta_uv = 0x7530;
				par.transit_t = 0xc0;
				par.delay = 0x00;
				mesh_tx_cmd2normal_primary(LIGHT_CTL_SET, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			case CL_LCTL_BV_04:{
				mesh_cmd_light_ctl_set_t par;
				par.lightness = 0x2710;
				par.temp = 0x4000;
				par.delta_uv = 0x7530;
				par.transit_t = 0x54;
				par.delay = 0x05;
				mesh_tx_cmd2normal_primary(LIGHT_CTL_SET, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			case CL_LCTL_BV_05:{
				mesh_cmd_light_ctl_set_t par;
				par.lightness = 0x2710;
				par.temp = 0x4000;
				par.delta_uv = 0x7530;
				mesh_tx_cmd2normal_primary(LIGHT_CTL_SET_NOACK, (u8 *)&par, OFFSETOF(mesh_cmd_light_ctl_set_t, transit_t), 0x01, 0x00);
				}
				break;
			case CL_LCTL_BV_06:{
				mesh_cmd_light_ctl_set_t par;
				par.lightness = 0x2710;
				par.temp = 0x4000;
				par.delta_uv = 0x7530;
				par.transit_t = 0xc0;
				par.delay = 0x00;
				mesh_tx_cmd2normal_primary(LIGHT_CTL_SET_NOACK, (u8 *)&par, sizeof(par), 0x01, 0x00);
				}
				break;
			case CL_LCTL_BV_07:{
				mesh_cmd_light_ctl_set_t par;
				par.lightness = 0x2710;
				par.temp = 0x4000;
				par.delta_uv = 0x7530;
				par.transit_t = 0x54;
				par.delay = 0x05;
				mesh_tx_cmd2normal_primary(LIGHT_CTL_SET_NOACK, (u8 *)&par, sizeof(par), 0x01, 0x00);
				}
				break;
			case CL_LCTL_BV_08:
				mesh_tx_cmd2normal_primary(LIGHT_CTL_TEMP_GET, 0, 0, 0x01, 0x01);
				break;
			case CL_LCTL_BV_09:{
				mesh_cmd_light_ctl_temp_set_t par;
				par.temp = 0x3000;
				par.delta_uv = 0x7530;
				mesh_tx_cmd2normal_primary(LIGHT_CTL_TEMP_SET, (u8 *)&par, OFFSETOF(mesh_cmd_light_ctl_temp_set_t, transit_t), 0x01, 0x01);
				}
				break;
			case CL_LCTL_BV_10:{
				mesh_cmd_light_ctl_temp_set_t par;
				par.temp = 0x3000;
				par.delta_uv = 0x7530;
				par.transit_t = 0xc0;
				par.delay = 0x00;
				mesh_tx_cmd2normal_primary(LIGHT_CTL_TEMP_SET, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			case CL_LCTL_BV_11:{
				mesh_cmd_light_ctl_temp_set_t par;
				par.temp = 0x3000;
				par.delta_uv = 0x7530;
				par.transit_t = 0x54;
				par.delay = 0x05;
				mesh_tx_cmd2normal_primary(LIGHT_CTL_TEMP_SET, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			case CL_LCTL_BV_12:{
				mesh_cmd_light_ctl_temp_set_t par;
				par.temp = 0x3000;
				par.delta_uv = 0x7530;
				mesh_tx_cmd2normal_primary(LIGHT_CTL_TEMP_SET_NOACK, (u8 *)&par, OFFSETOF(mesh_cmd_light_ctl_temp_set_t, transit_t), 0x01, 0x00);
				}
				break;
			case CL_LCTL_BV_13:{
				mesh_cmd_light_ctl_temp_set_t par;
				par.temp = 0x3000;
				par.delta_uv = 0x7530;
				par.transit_t = 0xc0;
				par.delay = 0x00;
				mesh_tx_cmd2normal_primary(LIGHT_CTL_TEMP_SET_NOACK, (u8 *)&par, sizeof(par), 0x01, 0x00);
				}
				break;
			case CL_LCTL_BV_14:{
				mesh_cmd_light_ctl_temp_set_t par;
				par.temp = 0x3000;
				par.delta_uv = 0x7530;
				par.transit_t = 0x54;
				par.delay = 0x05;
				mesh_tx_cmd2normal_primary(LIGHT_CTL_TEMP_SET_NOACK, (u8 *)&par, sizeof(par), 0x01, 0x00);
				}
				break;
			case CL_LCTL_BV_15:
				mesh_tx_cmd2normal_primary(LIGHT_CTL_DEFULT_GET, 0, 0, 0x01, 0x01);
				break;
			case CL_LCTL_BV_16:{
				mesh_cmd_light_ctl_def_set_t par;
				par.lightness = 0x2710;
				par.temp = 0x3000;
				par.delta_uv = 0x2710;
				mesh_tx_cmd2normal_primary(LIGHT_CTL_DEFULT_SET, (u8 *)&par, sizeof(par), 0x01, 0x01);	
				}
				break;
			case CL_LCTL_BV_17:{
				mesh_cmd_light_ctl_def_set_t par;
				par.lightness = 0x2710;
				par.temp = 0x3000;
				par.delta_uv = 0x2710;
				mesh_tx_cmd2normal_primary(LIGHT_CTL_DEFULT_SET_NOACK, (u8 *)&par, sizeof(par), 0x01, 0x00);	
				}
				break;
			case CL_LCTL_BV_18:
				mesh_tx_cmd2normal_primary(LIGHT_CTL_TEMP_RANGE_GET, 0, 0, 0x01, 0x01);
				break;
			case CL_LCTL_BV_19:{
				mesh_cmd_light_ctl_temp_range_set_t par;
				par.min = 0x600;
				par.max = 0x4000;
				mesh_tx_cmd2normal_primary(LIGHT_CTL_TEMP_RANGE_SET, (u8 *)&par, sizeof(par), 0x01, 0x01);	
				}
				break;
			case CL_LCTL_BV_20:{
				mesh_cmd_light_ctl_temp_range_set_t par;
				par.min = 0x600;
				par.max = 0x4000;
				mesh_tx_cmd2normal_primary(LIGHT_CTL_TEMP_RANGE_SET_NOACK, (u8 *)&par, sizeof(par), 0x01, 0x00);	
				}
				break;	
			//light HSL client
			case CL_LHSL_BV_01:
				mesh_tx_cmd2normal_primary(LIGHT_HSL_GET, 0, 0, 0x01, 0x01);
				break;
			case CL_LHSL_BV_02:{
				mesh_cmd_light_hsl_set_t par;
				par.lightness = 0xd80;
				par.hue = 0x1ed2;
				par.sat = 0x3039;
				mesh_tx_cmd2normal_primary(LIGHT_HSL_SET, (u8 *)&par, OFFSETOF(mesh_cmd_light_hsl_set_t, transit_t), 0x01, 0x01);
				}
				break;
			case CL_LHSL_BV_03:{
				mesh_cmd_light_hsl_set_t par;
				par.lightness = 0x4d2;
				par.hue = 0x162e;
				par.sat = 0x2334;
				par.transit_t = 0xc0;
				par.delay = 0x00;
				mesh_tx_cmd2normal_primary(LIGHT_HSL_SET, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			case CL_LHSL_BV_04:{
				mesh_cmd_light_hsl_set_t par;
				par.lightness = 0x4d2;
				par.hue = 0x162e;
				par.sat = 0x2334;
				par.transit_t = 0x54;
				par.delay = 0x05;
				mesh_tx_cmd2normal_primary(LIGHT_HSL_SET, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			case CL_LHSL_BV_05:{
				mesh_cmd_light_hsl_set_t par;
				par.lightness = 0xd80;
				par.hue = 0x1ed2;
				par.sat = 0x3039;
				mesh_tx_cmd2normal_primary(LIGHT_HSL_SET_NOACK, (u8 *)&par, OFFSETOF(mesh_cmd_light_hsl_set_t, transit_t), 0x01, 0x00);
				}
				break;
			case CL_LHSL_BV_06:{
				mesh_cmd_light_hsl_set_t par;
				par.lightness = 0x4d2;
				par.hue = 0x162e;
				par.sat = 0x2334;
				par.transit_t = 0xc0;
				par.delay = 0x00;
				mesh_tx_cmd2normal_primary(LIGHT_HSL_SET_NOACK, (u8 *)&par, sizeof(par), 0x01, 0x00);
				}
				break;
			case CL_LHSL_BV_07:{
				mesh_cmd_light_hsl_set_t par;
				par.lightness = 0x4d2;
				par.hue = 0x162e;
				par.sat = 0x2334;
				par.transit_t = 0x54;
				par.delay = 0x05;
				mesh_tx_cmd2normal_primary(LIGHT_HSL_SET_NOACK, (u8 *)&par, sizeof(par), 0x01, 0x00);
				}
				break;
			case CL_LHSL_BV_08:
				mesh_tx_cmd2normal_primary(LIGHT_HSL_TARGET_GET, 0, 0, 0x01, 0x01);
				break;
			case CL_LHSL_BV_09:
				mesh_tx_cmd2normal_primary(LIGHT_HSL_DEF_GET, 0, 0, 0x01, 0x01);
				break;
			case CL_LHSL_BV_10:{
				mesh_cmd_light_hsl_def_set_t par;
				par.lightness = 0x64;
				par.hue = 0x7b;
				par.sat = 0x3039;
				mesh_tx_cmd2normal_primary(LIGHT_HSL_DEF_SET, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			case CL_LHSL_BV_11:{
				mesh_cmd_light_hsl_def_set_t par;
				par.lightness = 0x2710;
				par.hue = 0xd05;
				par.sat = 0x4d2;
				mesh_tx_cmd2normal_primary(LIGHT_HSL_DEF_SET_NOACK, (u8 *)&par, sizeof(par), 0x01, 0x00);
				}
				break;
			case CL_LHSL_BV_12:
				mesh_tx_cmd2normal_primary(LIGHT_HSL_RANGE_GET, 0, 0, 0x01, 0x01);
				break;
			case CL_LHSL_BV_13:{
				mesh_cmd_light_hsl_range_set_t par;
				par.hue_min = 0x12c;
				par.hue_max = 0xf000;
				par.sat_min = 0x64;
				par.sat_max = 0xc000;
				mesh_tx_cmd2normal_primary(LIGHT_HSL_RANGE_SET, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			case CL_LHSL_BV_14:{
				mesh_cmd_light_hsl_range_set_t par;
				par.hue_min = 0x12c;
				par.hue_max = 0xf000;
				par.sat_min = 0x64;
				par.sat_max = 0xc000;
				mesh_tx_cmd2normal_primary(LIGHT_HSL_RANGE_SET_NOACK, (u8 *)&par, sizeof(par), 0x01, 0x00);
				}
				break;
			case CL_LHSL_BV_15:
				mesh_tx_cmd2normal_primary(LIGHT_HSL_HUE_GET, 0, 0, 0x01, 0x01);
				break;
			case CL_LHSL_BV_16:{
				mesh_cmd_light_hue_set_t par;
				par.hue = 0x3e8;
				mesh_tx_cmd2normal_primary(LIGHT_HSL_HUE_SET, (u8 *)&par, OFFSETOF(mesh_cmd_light_hue_set_t, transit_t), 0x01, 0x01);
				}
				break;
			case CL_LHSL_BV_17:{
				mesh_cmd_light_hue_set_t par;
				par.hue = 0x2710;
				par.transit_t = 0xc0;
				par.delay = 0x00;
				mesh_tx_cmd2normal_primary(LIGHT_HSL_HUE_SET, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			case CL_LHSL_BV_18:{
				mesh_cmd_light_hue_set_t par;
				par.hue = 0x1388;
				par.transit_t = 0x54;
				par.delay = 0x05;
				mesh_tx_cmd2normal_primary(LIGHT_HSL_HUE_SET, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			case CL_LHSL_BV_19:{
				mesh_cmd_light_hue_set_t par;
				par.hue = 0xbb8;
				mesh_tx_cmd2normal_primary(LIGHT_HSL_HUE_SET_NOACK, (u8 *)&par, OFFSETOF(mesh_cmd_light_hue_set_t, transit_t), 0x01, 0x00);
				}
				break;
			case CL_LHSL_BV_20:{
				mesh_cmd_light_hue_set_t par;
				par.hue = 0x4e20;
				par.transit_t = 0xc0;
				par.delay = 0x00;
				mesh_tx_cmd2normal_primary(LIGHT_HSL_HUE_SET_NOACK, (u8 *)&par, sizeof(par), 0x01, 0x00);
				}
				break;
			case CL_LHSL_BV_21:{
				mesh_cmd_light_hue_set_t par;
				par.hue = 0x1770;
				par.transit_t = 0x54;
				par.delay = 0x05;
				mesh_tx_cmd2normal_primary(LIGHT_HSL_HUE_SET_NOACK, (u8 *)&par, sizeof(par), 0x01, 0x00);
				}
				break;
			case CL_LHSL_BV_22:
				mesh_tx_cmd2normal_primary(LIGHT_HSL_SAT_GET, 0, 0, 0x01, 0x01);
				break;
			case CL_LHSL_BV_23:{
				mesh_cmd_light_sat_set_t par;
				par.sat = 0x3e8;
				mesh_tx_cmd2normal_primary(LIGHT_HSL_SAT_SET, (u8 *)&par, OFFSETOF(mesh_cmd_light_sat_set_t, transit_t), 0x01, 0x01);
				}
				break;
			case CL_LHSL_BV_24:{
				mesh_cmd_light_sat_set_t par;
				par.sat = 0x2710;
				par.transit_t = 0xc0;
				par.delay = 0x00;
				mesh_tx_cmd2normal_primary(LIGHT_HSL_SAT_SET, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			case CL_LHSL_BV_25:{
				mesh_cmd_light_sat_set_t par;
				par.sat = 0x2710;
				par.transit_t = 0x54;
				par.delay = 0x05;
				mesh_tx_cmd2normal_primary(LIGHT_HSL_SAT_SET, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			case CL_LHSL_BV_26:{
				mesh_cmd_light_sat_set_t par;
				par.sat = 0x3e8;
				mesh_tx_cmd2normal_primary(LIGHT_HSL_SAT_SET_NOACK, (u8 *)&par, OFFSETOF(mesh_cmd_light_sat_set_t, transit_t), 0x01, 0x00);
				}
				break;
			case CL_LHSL_BV_27:{
				mesh_cmd_light_sat_set_t par;
				par.sat = 0x3e8;
				par.transit_t = 0xc0;
				par.delay = 0x00;
				mesh_tx_cmd2normal_primary(LIGHT_HSL_SAT_SET_NOACK, (u8 *)&par, sizeof(par), 0x01, 0x00);
				}
				break;
			case CL_LHSL_BV_28:{
				mesh_cmd_light_sat_set_t par;
				par.sat = 0x3e8;
				par.transit_t = 0x54;
				par.delay = 0x05;
				mesh_tx_cmd2normal_primary(LIGHT_HSL_SAT_SET_NOACK, (u8 *)&par, sizeof(par), 0x01, 0x00);
				}
				break;
			//light xyl client
			case CL_LXYL_BV_01:
				mesh_tx_cmd2normal_primary(LIGHT_XYL_GET, 0, 0, 0x01, 0x01);
				break;
			case CL_LXYL_BV_02:{
				mesh_cmd_light_xyl_set_t par;
				par.lightness = 0x2710;
				par.x = 0xbb8;
				par.y = 0xfa0;
				mesh_tx_cmd2normal_primary(LIGHT_XYL_SET, (u8 *)&par, OFFSETOF(mesh_cmd_light_xyl_set_t, transit_t), 0x01, 0x01);
				}
				break;
			case CL_LXYL_BV_03:{
				mesh_cmd_light_xyl_set_t par;
				par.lightness = 0x2710;
				par.x = 0xbb8;
				par.y = 0xfa0;
				par.transit_t = 0xc0;
				par.delay = 0;
				mesh_tx_cmd2normal_primary(LIGHT_XYL_SET, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			case CL_LXYL_BV_04:{
				mesh_cmd_light_xyl_set_t par;
				par.lightness = 0x2710;
				par.x = 0xbb8;
				par.y = 0xfa0;
				par.transit_t = 0x54;
				par.delay = 0x05;
				mesh_tx_cmd2normal_primary(LIGHT_XYL_SET, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			case CL_LXYL_BV_05:{
				mesh_cmd_light_xyl_set_t par;
				par.lightness = 0x2710;
				par.x = 0x1388;
				par.y = 0x1770;
				mesh_tx_cmd2normal_primary(LIGHT_XYL_SET_NOACK, (u8 *)&par, OFFSETOF(mesh_cmd_light_xyl_set_t, transit_t), 0x01, 0x00);
				}
				break;
			case CL_LXYL_BV_06 :{
				mesh_cmd_light_xyl_set_t par;
				par.lightness = 0x2710;
				par.x = 0xbb8;
				par.y = 0xfa0;
				par.transit_t = 0xc0;
				par.delay = 0x00;
				mesh_tx_cmd2normal_primary(LIGHT_XYL_SET_NOACK, (u8 *)&par, sizeof(par), 0x01, 0x00);
				}
				break;
			case CL_LXYL_BV_07:{
				mesh_cmd_light_xyl_set_t par;
				par.lightness = 0x2710;
				par.x = 0xbb8;
				par.y = 0xfa0;
				par.transit_t = 0x54;
				par.delay = 0x05;
				mesh_tx_cmd2normal_primary(LIGHT_XYL_SET_NOACK, (u8 *)&par, sizeof(par), 0x01, 0x00);
				}
				break;
			case CL_LXYL_BV_08:
				mesh_tx_cmd2normal_primary(LIGHT_XYL_TARGET_GET, 0, 0, 0x01, 0x01);
				break;
			case CL_LXYL_BV_09:
				mesh_tx_cmd2normal_primary(LIGHT_XYL_DEF_GET, 0, 0, 0x01, 0x01);
				break;
			case CL_LXYL_BV_10:{
				mesh_cmd_light_xyl_def_set_t par;
				par.lightness = 0x4d2;
				par.x = 0x162e;
				par.y = 0x2334;
				mesh_tx_cmd2normal_primary(LIGHT_XYL_DEF_SET, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			case CL_LXYL_BV_11:{
				mesh_cmd_light_xyl_def_set_t par;
				par.lightness = 0x4d2;
				par.x = 0x162e;
				par.y = 0x2334;
				mesh_tx_cmd2normal_primary(LIGHT_XYL_DEF_SET_NOACK, (u8 *)&par, sizeof(par), 0x01, 0x00);
				}
				break;
			case CL_LXYL_BV_12:
				mesh_tx_cmd2normal_primary(LIGHT_XYL_RANGE_GET, 0, 0, 0x01, 0x01);
				break;
			case CL_LXYL_BV_13:{
				mesh_cmd_light_xyl_range_set_t par;
				par.x_min = 0x64;
				par.x_max = 0x2710;
				par.y_min = 0x64;
				par.y_max = 0x2710;
				mesh_tx_cmd2normal_primary(LIGHT_XYL_RANGE_SET, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			case CL_LXYL_BV_14:{
				mesh_cmd_light_xyl_range_set_t par;
				par.x_min = 0x64;
				par.x_max = 0x2710;
				par.y_min = 0x64;
				par.y_max = 0x2710;
				mesh_tx_cmd2normal_primary(LIGHT_XYL_RANGE_SET_NOACK, (u8 *)&par, sizeof(par), 0x01, 0x00);
				}
				break;
			//light LC client
			case CL_LLC_BV_01:
				mesh_tx_cmd2normal_primary(LIGHT_LC_MODE_GET, 0, 0, 0x01, 0x01);
				break;
			case CL_LLC_BV_02:{
				u8 mode = 0x01;
				mesh_tx_cmd2normal_primary(LIGHT_LC_MODE_SET, (u8 *)&mode, sizeof(mode), 0x01, 0x01);
				}
				break;
			case CL_LLC_BV_03:{
				u8 mode = 0x01;
				mesh_tx_cmd2normal_primary(LIGHT_LC_MODE_SET_NOACK, (u8 *)&mode, sizeof(mode), 0x01, 0x00);
				}
				break;
			case CL_LLC_BV_04:
				mesh_tx_cmd2normal_primary(LIGHT_LC_OM_GET, 0, 0, 0x01, 0x01);
				break;
			case CL_LLC_BV_05:{
				u8 mode = 0x01;
				mesh_tx_cmd2normal_primary(LIGHT_LC_OM_SET, (u8 *)&mode, sizeof(mode), 0x01, 0x01);
				}				
				break;
			case CL_LLC_BV_06:{
				u8 mode = 0x01;
				mesh_tx_cmd2normal_primary(LIGHT_LC_OM_SET_NOACK, (u8 *)&mode, sizeof(mode), 0x01, 0x00);
				}
				break;
			case CL_LLC_BV_07:
				mesh_tx_cmd2normal_primary(LIGHT_LC_ONOFF_GET, 0, 0, 0x01, 0x01);
				break;
			case CL_LLC_BV_08:{
				mesh_cmd_g_onoff_set_t par;
				par.onoff = 1;
				mesh_tx_cmd2normal_primary(LIGHT_LC_ONOFF_SET, (u8 *)&par, OFFSETOF(mesh_cmd_g_onoff_set_t, transit_t), 0x01, 0x01);
				}
				break;
			case CL_LLC_BV_09:{
				mesh_cmd_g_onoff_set_t par;
				par.onoff = 1;
				par.transit_t = 0xc0;
				par.delay = 0x00;
				mesh_tx_cmd2normal_primary(LIGHT_LC_ONOFF_SET, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			case CL_LLC_BV_10:{
				mesh_cmd_g_onoff_set_t par;
				par.onoff = 1;
				par.transit_t = 0x54;
				par.delay = 0x05;
				mesh_tx_cmd2normal_primary(LIGHT_LC_ONOFF_SET, (u8 *)&par, sizeof(par), 0x01, 0x01);
				}
				break;
			case CL_LLC_BV_11:{
				mesh_cmd_g_onoff_set_t par;
				par.onoff = 1;
				mesh_tx_cmd2normal_primary(LIGHT_LC_ONOFF_SET_NOACK, (u8 *)&par, OFFSETOF(mesh_cmd_g_onoff_set_t, transit_t), 0x01, 0x00);
				}
				break;
			case CL_LLC_BV_12:{
				mesh_cmd_g_onoff_set_t par;
				par.onoff = 1;
				par.transit_t = 0xc0;
				par.delay = 0x00;
				mesh_tx_cmd2normal_primary(LIGHT_LC_ONOFF_SET_NOACK, (u8 *)&par, sizeof(par), 0x01, 0x00);
				}
				break;
			case CL_LLC_BV_13:{
				mesh_cmd_g_onoff_set_t par;
				par.onoff = 1;
				par.transit_t = 0x54;
				par.delay = 0x05;
				mesh_tx_cmd2normal_primary(LIGHT_LC_ONOFF_SET_NOACK, (u8 *)&par, sizeof(par), 0x01, 0x00);
				}				
				break;
			case CL_LLC_BV_14:{
				u16 id = 0x2e;
				mesh_tx_cmd2normal_primary(LIGHT_LC_PROPERTY_GET, (u8 *)&id, sizeof(id), 0x01, 0x01);
				}
				break;
			case CL_LLC_BV_15:{
				light_lc_property_t par;
				par.len = 2;
				par.id = 0x2e;
				u8 raw_val[2] = {0x00, 0x20};
				memcpy(par.val, &raw_val, par.len);
				mesh_tx_cmd2normal_primary(LIGHT_LC_PROPERTY_SET, (u8 *)&par.id, par.len+2, 0x01, 0x01);
				}
				break;
			case CL_LLC_BV_16:{
				light_lc_property_t par;
				par.len = 2;
				par.id = 0x2e;
				u8 raw_val[2] = {0x00, 0x30};
				memcpy(par.val, &raw_val, par.len);
				mesh_tx_cmd2normal_primary(LIGHT_LC_PROPERTY_SET_NOACK, (u8 *)&par.id, par.len+2, 0x01, 0x00);
				}
				break;	
    		default:
    			break;
        }
		model_sig_cfg_s.ttl_def = tc_ttl_tmp;
    }
	if(tc_par.tc_id != NODE_NET_BV_00){// not need to show network packet
	 	memset(&tc_par, 0x00, sizeof(tc_par));//tc_id, sub_id, para[2]
	}
}

void test_case_key_refresh_patch()
{
	// use mac filter later
	if(!provision_mag.pro_stop_flag && provision_mag.unicast_adr_last> 2){
		static u32 tick_pro_adr_set;
		if(!tick_pro_adr_set){
			tick_pro_adr_set = clock_time();
		}
		
		if(clock_time_exceed(tick_pro_adr_set, 10*1000*1000)){
			provision_mag.pro_stop_flag = 1;
			provision_mag_cfg_s_store();
		}
	}
	
	static u8 ts_first = 1;
	if(ts_first){			// init parameters, add the same device again for test
		ts_first = 0;
		if(provision_mag.unicast_adr_last> 2){
			provision_mag.unicast_adr_last = 2;
			provision_mag.pro_stop_flag = 0;
		}
	}
}

#endif
