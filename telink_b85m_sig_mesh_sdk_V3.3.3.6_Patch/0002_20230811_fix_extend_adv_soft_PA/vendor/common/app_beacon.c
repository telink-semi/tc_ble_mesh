/********************************************************************************************************
 * @file	app_beacon.c
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
#if WIN32 
#include "../../../reference/tl_bulk/lib_file/app_config.h"
#include "../../../reference/tl_bulk/lib_file/gatt_provision.h"
#endif
#include "proj_lib/ble/ll/ll.h"
#include "proj_lib/ble/blt_config.h"
#include "app_beacon.h"
#include "vendor/common/app_provison.h"
#include "proj/common/types.h"
#include "app_privacy_beacon.h"
#include "blt_soft_timer.h"
#include "vendor/common/mi_api/telink_sdk_mible_api.h"


int mesh_bear_tx_beacon_adv_channel_only(u8 *bear, u8 trans_par_val)
{
	#if FEATURE_RELAY_EN	// use relay buffer should be better
	mesh_adv_fifo_relay.num = mesh_adv_fifo_relay.num;	// will be optimized, just for sure that relay buffer is existed.
	mesh_cmd_bear_t *p_bear = (mesh_cmd_bear_t *)bear;
	p_bear->trans_par_val = trans_par_val;
	int err = my_fifo_push_relay(p_bear, mesh_bear_len_get(p_bear), 0);
	#else
	int err = mesh_bear_tx2mesh(bear, trans_par_val);
	#endif

	return err;
}

int mesh_beacon_send_proc()
{
	int err = -1;
#if !WIN32
	if(blt_state == BLS_LINK_STATE_CONN){
		if (proxy_Out_ccc[0]==1 && proxy_Out_ccc[1]==0){
			if(is_provision_success()&& beacon_send.conn_beacon_flag){
				err = mesh_tx_sec_private_beacon_proc(1);// send conn beacon to the provisioner
				if(0 == err){				 
					beacon_send.conn_beacon_flag =0;
				}				
			}
		}
		return err;
	}else{
		reset_all_ccc(); 
	}
	// dispatch when connected whether it need to send the unprovisioned beacon 
	if(beacon_send.en && clock_time_exceed(beacon_send.tick ,beacon_send.inter)&&!is_provision_success()){
		beacon_send.tick = clock_time();
		#if (!(DEBUG_MESH_DONGLE_IN_VC_EN && (!IS_VC_PROJECT)) )
			if(!is_provision_working()){
				#if !__PROJECT_MESH_PRO__
					#if VENDOR_MD_NORMAL_EN
				err = unprov_beacon_send(MESH_UNPROVISION_BEACON_WITH_URI,0); // send the adv beacon to the provisioner
					#endif
				#endif 
				
			}
			
		#endif
	}
#endif 
	return err;
}

int check_pkt_is_unprovision_beacon(u8 *dat)
{
	mesh_cmd_bear_t *p_bear = CONTAINER_OF((mesh_cmd_nw_t *)dat, mesh_cmd_bear_t, nw);;
	beacon_str *p_beacon = (beacon_str *)p_bear;
	if(	p_beacon->bea_data.header.type == MESH_ADV_TYPE_BEACON&&
		p_beacon->bea_data.beacon_type == UNPROVISION_BEACON){
		return 0;
	}else{
		return -1;
	}
}

u8  beacon_data_init_without_uri(beacon_str *p_str ,u8 *p_uuid,u8 *p_info){
	#if LPN_CONTROL_EN
	if(mi_mesh_get_state()){
		p_str->trans_par_val = TRANSMIT_DEF_PAR_BEACON;		
	}else{
		// only send once 
		p_str->trans_par_val = 0;
	}
	#else
	p_str->trans_par_val = TRANSMIT_DEF_PAR_BEACON;
	#endif
	p_str->bea_data.header.len = 20;
	p_str->bea_data.header.type = MESH_ADV_TYPE_BEACON ;
	p_str->bea_data.beacon_type = UNPROVISION_BEACON;
	memcpy(p_str->bea_out_uri.device_uuid,p_uuid,16);
	u8 *p_oob = p_str->bea_out_uri.oob_info;
	p_oob[0] = p_info[1];
	p_oob[1] = p_info[0];
	return 1;
}

u8  beacon_data_init_uri(beacon_str *p_str ,u8 *p_uuid,u8 *p_info,u8 *p_hash){
	#if LPN_CONTROL_EN
	if(mi_mesh_get_state()){
		p_str->trans_par_val = TRANSMIT_DEF_PAR_BEACON;		
	}else{
		// only send once 
		p_str->trans_par_val = 0;
	}
	#else
	p_str->trans_par_val = TRANSMIT_DEF_PAR_BEACON;
	#endif
	p_str->bea_data.header.len = 24;
	p_str->bea_data.header.type = MESH_ADV_TYPE_BEACON ;
	p_str->bea_data.beacon_type = UNPROVISION_BEACON;
	memcpy(p_str->bea_data.device_uuid,p_uuid,16);
	memcpy(p_str->bea_data.uri_hash,p_hash,4);
	u8 *p_oob = p_str->bea_out_uri.oob_info;
	p_oob[0] = p_info[1];
	p_oob[1] = p_info[0];
	return 1;
}

u8 beacon_test_case(u8*p_tc,u8 len )
{
    beacon_str  beaconData = {{0}};
	beaconData.trans_par_val = TRANSMIT_DEF_PAR_BEACON;
	beaconData.bea_testcase_id.header.len = 10;
	beaconData.bea_testcase_id.header.type = MESH_ADV_TYPE_TESTCASE_ID;
	beaconData.bea_testcase_id.beacon_type = UNPROVISION_BEACON;
	memcpy(beaconData.bea_testcase_id.case_id,p_tc,len);
	mesh_tx_cmd_add_packet((u8 *)(&beaconData));
	return 1;
}

// mode =1; with uri , mode =0 means without uri 
//#define URI_DATA    {0x17,0x2f,0x2f,0x50,0x54,0x53,0x2e,0x43,0x4f,0x4d}
// URI_HASH             {72 26 a2 7f};  // sample data for URI_DATA


u8  is_unprovision_beacon_with_uri(event_adv_report_t *report)
{
	if(report->len == 25){
		return 1;
	}else{
		return 0;
	}
}


int unprov_beacon_send(u8 mode ,u8 blt_sts)
{
	int err = -1;
/*
	Device UUID : 70cf7c9732a345b691494810d2e9cbf4
	OOB : Number, Inside Manual
	OOB Information : 4020
	URI : https://www.example.com/mesh/products/light-switch-v3
	URI Data : 172f2f7777772e6578616d706c652e636f6d2f6d6573682f70726f6475637473
	2f6c696768742d7377697463682d7633
	URI Hash : d97478b3667f4839487469c72b8e5e9e
	Beacon : 0070cf7c9732a345b691494810d2e9cbf44020d97478b3
*/
    beacon_str  beaconData = {{0}};

	if(mode == MESH_UNPROVISION_BEACON_WITH_URI){
		u8 hash_tmp[16];
		u8 uri_dat[] = URI_DATA;
		mesh_sec_func_s1 (hash_tmp, uri_dat, sizeof(uri_dat));
		beacon_data_init_uri(&beaconData ,prov_para.device_uuid,prov_para.oob_info,hash_tmp);
	}else if(mode == MESH_UNPROVISION_BEACON_WITHOUT_URI){
		beacon_data_init_without_uri(&beaconData ,prov_para.device_uuid,prov_para.oob_info);
	}else{}
	if(blt_sts){
	    #if (!WIN32)
		err = notify_pkts((u8 *)(&(beaconData.bea_data)),sizeof(beacon_data_pk),PROVISION_ATT_HANDLE,MSG_MESH_BEACON);
		#endif
	}else{
		err = mesh_tx_cmd_add_packet((u8 *)(&beaconData));
	}
	return err;
}	

int mesh_tx_sec_nw_beacon(mesh_net_key_t *p_nk_base, u8 blt_sts)
{
	int err = -1;
    u8 key_phase = p_nk_base->key_phase;
    mesh_net_key_t *p_netkey = p_nk_base;
	if(KEY_REFRESH_PHASE2 == key_phase){
		p_netkey += 1;		// use new key
	}

    mesh_cmd_bear_t bc_bear;
    memset(&bc_bear, 0, sizeof(bc_bear));
	#if (0) // keep sending 2.5s if need
	bc_bear.tx_head.par_type = BEAR_TX_PAR_TYPE_REMAINING_TIMES;
	bc_bear.tx_head.val[0] = 255;
	#endif
   	bc_bear.type = MESH_ADV_TYPE_BEACON;
    bc_bear.len = 23; // 1+1+sizeof(mesh_beacon_sec_nw_t)+8;
	bc_bear.beacon.type = SECURE_BEACON;
    // beacon
    mesh_beacon_sec_nw_t *p_bc_sec = (mesh_beacon_sec_nw_t *)bc_bear.beacon.data;
	get_iv_update_key_refresh_flag(&p_bc_sec->flag, p_bc_sec->iv_idx, key_phase);
    memcpy(p_bc_sec->nwId, p_netkey->nw_id, sizeof(p_bc_sec->nwId));
    #if 0
    #if (HCI_ACCESS == HCI_USE_USB)
	SET_TC_FIFO(TSCRIPT_MESH_TX, (u8 *)&bc_bear.len, bc_bear.len+1);
    #else
    LOG_MSG_LIB(TL_LOG_NODE_SDK,&bc_bear.len, bc_bear.len+1,"xxx TX beacon,nk arr idx:%d, new:%d, ",GetNKArrayIdxByPointer(p_nk_base),(KEY_REFRESH_PHASE2 == key_phase));
	#endif
	#endif
    mesh_sec_beacon_auth(p_netkey->bk, (u8 *)&p_bc_sec->flag, 0);
    if(blt_sts){
    	#if WIN32
		err = prov_write_data_trans((u8 *)(&bc_bear.beacon.type),sizeof(mesh_beacon_sec_nw_t)+1,MSG_MESH_BEACON);
		#else
		err = notify_pkts((u8 *)(&bc_bear.beacon.type),sizeof(mesh_beacon_sec_nw_t)+1,GATT_PROXY_HANDLE,MSG_MESH_BEACON);
		#endif
		if(0 == err){	
			LOG_MSG_LIB(TL_LOG_IV_UPDATE,(&bc_bear.len), bc_bear.len+1,"tx GATT secure NW beacon:",0);
		}
	}else{
		// static u32 iv_idx_st_A1;iv_idx_st_A1++;
    	err = mesh_bear_tx_beacon_adv_channel_only((u8 *)&bc_bear, TRANSMIT_PAR_SECURITY_BEACON);
	}
    return err;
}

int mesh_tx_sec_nw_beacon_all_net(u8 blt_sts)
{
	int err = 0;
	if((0 == is_need_send_sec_nw_beacon()) && !blt_sts){// force notify security while gatt connecting
		return err;
	}
	if(!is_provision_success()||MI_API_ENABLE){// in the mi mode will never send secure beacon .
		return err;
	}
	foreach(i,NET_KEY_MAX){
		mesh_net_key_t *p_netkey_base = &mesh_key.net_key[i][0];
		if(!p_netkey_base->valid){
			continue;
		}
		#if TESTCASE_FLAG_ENABLE
		/* in the pts private beacon proxy bv-07c , it should not send 
		two secure beacon on gatt connection , other wise the filter sts will fail*/
		err = mesh_tx_sec_nw_beacon(p_netkey_base, blt_sts);
		if(blt_sts && beacon_send.conn_beacon_flag ){
			break;
		}
		#else
		err = mesh_tx_sec_nw_beacon(p_netkey_base, blt_sts);
		#endif
	}
	return err;
}

int mesh_tx_sec_private_beacon_proc(u8 blt_sts)
{
	int err = mesh_tx_sec_nw_beacon_all_net(blt_sts);
	#if MD_PRIVACY_BEA
		#if MD_SERVER_EN
		err = mesh_tx_privacy_nw_beacon_all_net(blt_sts);
		#endif
	#endif
	return err;
}

int iv_update_key_refresh_rx_handle_cb(mesh_ctl_fri_update_flag_t *p_ivi_flag, u8 *p_iv_idx)
{
	#if __PROJECT_MESH_SWITCH__
	LOG_MSG_INFO(TL_LOG_IV_UPDATE,0, 0,"switch receive security network beacon time_s:%d", clock_time_s());
	extern int soft_timer_rcv_beacon_timeout();
	soft_timer_rcv_beacon_timeout();
	blt_soft_timer_delete(&soft_timer_rcv_beacon_timeout);
	switch_iv_update_time_refresh(); 
	#endif
	return 0;
}
