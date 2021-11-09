/********************************************************************************************************
 * @file     gatt_provision.c 
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


#include "gatt_provision.h"
#include "../../../ble_lt_mesh/proj_lib/mesh_crypto/le_crypto.h"
#include "../../../ble_lt_mesh/proj_lib/mesh_crypto/mesh_crypto.h"
#include "../../../ble_lt_mesh/vendor/common/remote_prov.h"


#if VC_APP_ENABLE
#include <SDKDDKVer.h>

#include "../stdafx.h"
#include "../ScanDlg.h"
#include "../tl_ble_moduleDlg.h"
#include "../../../ble_lt_mesh/vendor/common/certify_base/certify_base_crypto.h"
#elif ANDROID_APP_ENABLE
//#include "mesh_lib.h"
#include "string.h"
#endif



u8 provision_write_handle =PROVISION_WRITE_HANDLER;
u8 proxy_write_handle =  PROXY_WRITE_HANDLER;
u8 online_st_write_handle =  0; // 0 meas not existed this uuid

u8 gatt_pro_cloud_en =0;

void set_gatt_pro_cloud_en(u8 en)
{
	gatt_pro_cloud_en = en ;
	return;
}

// just for test about the function ,and the lib version part 
unsigned int get_lib_version()
{
	u32 tmp_version;
	tmp_version = SIG_MESH_LIB_VERSION;
	return tmp_version;
}
typedef struct{
	u8  test_u8;
	u16 test_u16;
	u32 test_u32;
}test_data_str1;

unsigned char test_pragma_pack()
{
	return sizeof(test_data_str1);
}


u8 gatt_para_pro[400];
u16 gatt_para_len;
gatt_pro_para_mag  gatt_provision_mag;
PB_GATT_ADV_DAT    gatt_provision_adv;


// CTL_privision dialog
unsigned char prov_newkey[16];
unsigned char prov_key_index[2];
unsigned char prov_ivi_update_flag=0;
unsigned char prov_iv_index[4];
unsigned char prov_unicast_address[2];

u8  gatt_pro_ppk[64] = {0x2c,0x31,0xa4,0x7b,0x57,0x79,0x80,0x9e, 0xf4,0x4c,0xb5,0xea,0xaf,0x5c,0x3e,0x43,
						0xd5,0xf8,0xfa,0xad,0x4a,0x87,0x94,0xcb, 0x98,0x7e,0x9b,0x03,0x74,0x5c,0x78,0xdd,
						0x91,0x95,0x12,0x18,0x38,0x98,0xdf,0xbe, 0xcd,0x52,0xe2,0x40,0x8e,0x43,0x87,0x1f,
						0xd0,0x21,0x10,0x91,0x17,0xbd,0x3e,0xd4, 0xea,0xf8,0x43,0x77,0x43,0x71,0x5d,0x4f};

u8 gatt_pro_psk[32]={0x06,0xa5,0x16,0x69,0x3c,0x9a,0xa3,0x1a, 0x60,0x84,0x54,0x5d,0x0c,0x5d,0xb6,0x41,
					   0xb4,0x85,0x72,0xb9,0x72,0x03,0xdd,0xff, 0xb7,0xac,0x73,0xf7,0xd0,0x45,0x76,0x63};
u8 gatt_pro_edch[32];
u8 gatt_pro_random[32];
u8 gatt_pro_comfirm[32];
u8 gatt_pro_input[0x91];
u8  gatt_pro_auth[32] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
u8  gatt_pro_comfirm_key[16];

u8 gatt_dev_random[32];
u8 gatt_dev_comfirm[32];
u8 gatt_pro_session_key[16];
u8 gatt_pro_session_nonce_tmp[16];
u8 gatt_pro_session_nonce[16];
u8 gatt_pro_dat[0x40] = {0xef,0xb2,0x25,0x5e,0x64,0x22,0xd3,0x30, 0x08,0x8e,0x09,0xbb,0x01,0x5e,0xd7,0x07,	
						 0x05,0x67,0x00,0x01,0x02,0x03,0x04,0x0b, 0x0c};
u8 gatt_dev_key[16];

void set_provision_mag_oob_info(u8 oob_type, u8 *p_oob,u8 len )
{
	gatt_provision_mag.oob_type = oob_type;
	if(oob_type == MESH_STATIC_OOB){
		if(p_oob!= 0 && len>0){
		memcpy(gatt_provision_mag.oob_buf,p_oob,len);
		}
	}else if (oob_type == MESH_NO_OOB){
		memset(gatt_provision_mag.oob_buf,0,sizeof(gatt_provision_mag.oob_buf));
	}else{}
	memcpy(gatt_pro_session_key,gatt_provision_mag.oob_buf,16);
	return ;
}
u8 set_app_key_pro_data(u8 *p_dat,u8 len)
{
	if(len>26){
		return 0;
	}
	memcpy(gatt_pro_dat,p_dat,len);
	return 1;
}

void set_gatt_provision_net_info(u8 p_netkey[16],u16*p_key_index,u8*p_flag,u8 p_ivi[4],u16*p_unicast)
{
	provison_net_info_str *p_net_info = (provison_net_info_str *)gatt_pro_dat;
	if(p_netkey!=0){
		memcpy(p_net_info->net_work_key,p_netkey,sizeof(p_net_info->net_work_key));
	}
	if(p_key_index!=0){
		p_net_info->key_index = *p_key_index;
	}
	if(p_flag!=0){
		p_net_info->flags = *p_flag;
	}
	if(p_ivi!=0){
		memcpy(p_net_info->iv_index,p_ivi,sizeof(p_net_info->iv_index));
	}
	if(p_unicast!=0){
		p_net_info->unicast_address = *p_unicast;
	}
	
	return ;
}

	
// serperate the different device by the uuid 
void set_provision_adv(u8 *p_service_uuid ,u8 *p_service_data, u8 *p_oob_info)
{
	if(p_service_uuid !=0){
		memcpy(gatt_provision_adv.service_uuid,p_service_uuid,2);
	}
	if(p_service_data !=0){
		memcpy(gatt_provision_adv.service_data,p_service_data,16);
	}
	if(p_oob_info != 0){
		memcpy(gatt_provision_adv.oob_info,p_oob_info,2);
	}
	return ;
}
unsigned char  store_data_to_app(PRO_STORE_DATABASE *p_base)
{
	memcpy(p_base->service_uuid ,gatt_provision_adv.service_uuid,2);
	memcpy(p_base->service_data ,gatt_provision_adv.service_data,16);
	memcpy(p_base->oob_info		,gatt_provision_adv.oob_info,2);
	memcpy(p_base->net_work_key ,gatt_pro_dat,25);

	memcpy(p_base->gatt_pro_ppk,gatt_pro_ppk,sizeof(gatt_pro_ppk));
	memcpy(p_base->gatt_pro_psk,gatt_pro_psk,sizeof(gatt_pro_psk));

	memcpy(p_base->gatt_pro_edch,gatt_pro_edch,sizeof(gatt_pro_edch));
	memcpy(p_base->gatt_pro_random,gatt_pro_random,sizeof(gatt_pro_random));
	memcpy(p_base->gatt_pro_session_key,gatt_pro_session_key,sizeof(gatt_pro_session_key));
	memcpy(p_base->gatt_pro_session_nonce,gatt_pro_session_nonce,sizeof(gatt_pro_session_nonce));
	memcpy(p_base->gatt_pro_dat,gatt_pro_dat,sizeof(gatt_pro_dat));
	return 1;
}
unsigned char app_to_restore_data(PRO_STORE_DATABASE *p_base)
{
	memcpy(gatt_provision_adv.service_uuid,p_base->service_uuid ,2);
	memcpy(gatt_provision_adv.service_data,p_base->service_data ,16);
	memcpy(gatt_provision_adv.oob_info,p_base->oob_info		,2);
	memcpy(gatt_pro_dat,p_base->net_work_key ,25);

	memcpy(gatt_pro_ppk,p_base->gatt_pro_ppk,sizeof(gatt_pro_ppk));
	memcpy(gatt_pro_psk,p_base->gatt_pro_psk,sizeof(gatt_pro_psk));

	memcpy(gatt_pro_edch,p_base->gatt_pro_edch,sizeof(gatt_pro_edch));
	memcpy(gatt_pro_random,p_base->gatt_pro_random,sizeof(gatt_pro_random));
	memcpy(gatt_pro_session_key,p_base->gatt_pro_session_key,sizeof(gatt_pro_session_key));
	memcpy(gatt_pro_session_nonce,p_base->gatt_pro_session_nonce,sizeof(gatt_pro_session_nonce));
	memcpy(gatt_pro_dat,p_base->gatt_pro_dat,sizeof(gatt_pro_dat));
	return 1;
}

void gatt_pro_para_mag_init()
{
	gatt_provision_mag.provison_send_state = LINK_UNPROVISION_STATE;
	gatt_provision_mag.provison_rcv_state= LINK_UNPROVISION_STATE;
	gatt_provision_mag.initial_pro_roles = MESH_INI_ROLE_PROVISIONER;
	memset(gatt_para_pro,0,sizeof(gatt_para_pro));
	gatt_para_len = sizeof(gatt_para_pro);
	prov_timer_clr();
}
u8 gatt_set_pro_invite(mesh_pro_data_structer *p_str,u8 att_dura)
{
	set_pro_invite(p_str,att_dura);
	return 1;
}
u8 gatt_set_pro_capa(mesh_pro_data_structer *p_str,u8 ele_num,u16 alogr,u8 keytype,
				u8 outOOBsize,u16 outAct,u8 inOOBsize,u16 inOOBact)
{
	set_pro_capa(p_str,ele_num,alogr,keytype,outOOBsize,outAct,inOOBsize,inOOBact);
	return 1;
}
u8 gatt_set_pro_start(mesh_pro_data_structer *p_str,u8 alogri,u8 pubkey,
					u8 authmeth,u8 authact,u8 authsize )
{
	set_pro_start(p_str,alogri,pubkey,authmeth,authact,authsize);
	return 1;
}
u8 gatt_set_pro_pub_key(mesh_pro_data_structer *p_str,u8 *p_pubkeyx,u8 *p_pubkeyy){
	set_pro_pub_key(p_str,p_pubkeyx,p_pubkeyy);
	return 1;
}


u8 gatt_set_pro_comfirm(mesh_pro_data_structer *p_str,u8 *p_comfirm,u8 len){
	set_pro_comfirm(p_str,p_comfirm,len);
	return 1;
}

u8 gatt_set_pro_random(mesh_pro_data_structer *p_str,u8 *p_random,u8 len){
	set_pro_random(p_str,p_random,len);
	return 1;
}

u8 gatt_set_pro_data(mesh_pro_data_structer *p_str, u8 *p_data,u8 *p_mic){
	set_pro_data(p_str,p_data,p_mic);
	return 1;
}
u8 gatt_set_pro_complete(mesh_pro_data_structer *p_str){
	set_pro_complete(p_str);
	return 1;
}
// add the operation for the gatt receive part for the sar timeout 
u8 debug_sar[255];
u16 debug_sar_idx =0;
u8 gatt_pkt_pb_gatt_data(u8 *p,u8 direct,u8 len,u8* proxy_buf,u16* p_proxy_len)
{
	//package the data str 
	static u8 idx_num =0;
	u8 l2cap_len =0;
	pb_gatt_proxy_str *p_gatt;
	if(direct){
		p_gatt = (pb_gatt_proxy_str *)(p);
		l2cap_len = len+3;
	}else{
		p_gatt	= (pb_gatt_proxy_str *)(p+9);
		l2cap_len = p[2];
	}
	/*
	debug_sar[debug_sar_idx++]= p_gatt->sar;
	LOG_MSG_INFO(TL_LOG_MESH,debug_sar, 255 ,"debug printf the msg of the gatt sar \r\n");
	if(debug_sar_idx == 255){
		debug_sar_idx =0;
		memset(debug_sar,0,sizeof(debug_sar));
	}
	*/
	if(p_gatt->sar == SAR_START){
		idx_num =0;
		memcpy(proxy_buf,p_gatt->data,l2cap_len-4);
		idx_num +=(l2cap_len-4);
		mesh_proxy_sar_start();
		return MSG_COMPOSITE_WAIT;
	}else if(p_gatt->sar == SAR_CONTINUS){
		memcpy(proxy_buf+idx_num,p_gatt->data,l2cap_len-4);
		idx_num +=(l2cap_len-4);
		mesh_proxy_sar_continue();
		return MSG_COMPOSITE_WAIT;
	}else if(p_gatt->sar == SAR_END){
		memcpy(proxy_buf+idx_num,p_gatt->data,l2cap_len-4);
		idx_num +=(l2cap_len-4);
		mesh_proxy_sar_end();
	}else if(p_gatt->sar == SAR_COMPLETE){
		idx_num =0;
		memcpy(proxy_buf+idx_num,p_gatt->data,l2cap_len-4);
		idx_num +=(l2cap_len-4);
		mesh_proxy_sar_complete();
	}else{}
	mesh_proxy_sar_err_terminate();
	*p_proxy_len = idx_num ;
	return p_gatt->msgType;
	// after package the data and the para 
}

u32 prov_start_tick =0;
void prov_timer_start()
{
	prov_start_tick = clock_time()|1;
}
void prov_timer_clr()
{
	prov_start_tick =0;
}

#if WIN32
#define MAX_RP_TIMEOUT_VAL 	60*1000*1000
#define MAX_GATT_TIMEOUT_VAL 	30*1000*1000

void check_prov_timeout()
{
	if(prov_start_tick && clock_time_exceed(prov_start_tick,MAX_RP_TIMEOUT_VAL)){
	    prov_start_tick =0;
	    #if (MD_REMOTE_PROV&&MD_CLIENT_EN) // clear remote prov para part 
		mesh_cmd_sig_rp_cli_send_link_close(rp_client.node_adr,REMOTE_PROV_LINK_CLOSE_FAIL);
        mesh_rp_client_para_reset();   
	    #endif
		provision_end_callback(PROV_TIMEOUT_RET);
	}
}
#endif 


u8 prov_write_data_trans(u8 *p,u16 len,u8 msg_type)
{
    gatt_write_transaction_callback(p,len,msg_type);
    return 1;
}
void gatt_prov_send_invite()
{
 	mesh_pro_data_structer *p_str = (mesh_pro_data_structer *)(gatt_para_pro);
	gatt_set_pro_invite(p_str,0);
	LOG_MSG_INFO(TL_LOG_GATT_PROVISION,(u8 *)p_str,sizeof(pro_trans_invite),"SEND:provisioner send invite cmd \r\n",0);
	gatt_pro_input[0]=0;
	gatt_para_len = sizeof(pro_trans_invite);
}

void start_provision_invite()
{
	gatt_pro_para_mag_init();
	gatt_prov_send_invite();
	gatt_provision_mag.provison_send_state = STATE_PRO_INVITE_ACK;
	prov_timer_start();
	if(get_mesh_rp_proc_en()){
        mesh_cmd_sig_cp_cli_send_invite(gatt_para_pro,gatt_para_len);
	}else{
        prov_write_data_trans(gatt_para_pro,gatt_para_len,MSG_PROVISION_PDU);
	}
	return ;
}

void start_provision_record()
{
	gatt_pro_para_mag_init();
	prov_timer_start();
	mesh_pro_data_structer *p_str = (mesh_pro_data_structer *)(gatt_para_pro);
	gatt_prov_send_record_get(&(p_str->rec_get));
	gatt_provision_mag.provison_send_state = STATE_PRO_REC_GET_ACK;
	prov_write_data_trans(gatt_para_pro,gatt_para_len,MSG_PROVISION_PDU);
}

void start_provision_process()
{
	if(certify_base_en){
		start_provision_record();
	}else{
		start_provision_invite();
	}
}



void set_pro_psk_ppk(u8 * random,u8 random_len )
{
	random[0]=rand();
	random[1]=rand();
	tn_p256_keypair_mac(gatt_pro_psk,gatt_pro_ppk,gatt_pro_ppk+32,random,random_len);
}

/*call by APP*/
#if ((ANDROID_APP_ENABLE || IOS_APP_ENABLE))
void master_terminate_ble_callback()
{
	//reset the parameters about the state 
	gatt_pro_para_mag_init();
	//reset the cfgcl parameters 
	mesh_ker_cfgcl_proc_init();
	reset_host_fifo();
	mesh_rp_client_para_reset();
	pair_login_ok = 0;
	APP_set_mesh_ota_pause_flag(1); // only for app, not for VC
}
#endif



u8 gatt_node_ele =0;
int gatt_get_node_ele_cnt(u16 unicast)
{
	int ele_cnt = 0;
	VC_node_info_t *p_info = get_VC_node_info(unicast, 1); // have been sure no same node address in node info
	if(p_info){
		ele_cnt = p_info->element_cnt;

	}
	return ele_cnt;
}

void gatt_prov_rcv_pro_fail(mesh_pro_data_structer *p_rcv)
{
    LOG_MSG_INFO(TL_LOG_GATT_PROVISION,(u8 *)&(p_rcv->invite),sizeof(pro_trans_invite),"RCV:rcv provision fail cmd \r\n",0);
	mesh_proxy_master_terminate_cmd();
	provision_end_callback(PROV_FAIL_CMD_ERR);
}


void gatt_prov_rcv_capa(mesh_pro_data_structer *p_rcv,mesh_pro_data_structer *p_send)
{
    memset(&prov_oob,0,sizeof(prov_oob));// clean the prov oob for gatt provision part 
	memcpy(gatt_pro_input+1,&(p_rcv->capa.ele_num),11);
	gatt_node_ele = p_rcv->capa.ele_num;
	swap_mesh_pro_capa(p_rcv);// swap the endiness for the capa data 
	memcpy(&prov_oob.capa , &p_rcv->capa,sizeof(p_rcv->capa));
	get_pubkey_oob_info_by_capa(&prov_oob);
	LOG_MSG_INFO(TL_LOG_GATT_PROVISION,(u8*)&prov_oob.capa,sizeof(prov_oob.capa),"RCV:the provision capa data is \r\n",0);
}    

void gatt_prov_send_start(mesh_pro_data_structer *p_rcv,mesh_pro_data_structer *p_send)
{
	#if 1
	set_start_para_by_capa(&prov_oob);
	if(mesh_prov_oob_auth_data(&prov_oob)){
		if(MESH_STATIC_OOB == prov_oob.prov_key){
			LOG_MSG_INFO(TL_LOG_GATT_PROVISION, 0, 0,"can't find static oob, try with no oob!\r\n",0);
			prov_oob.prov_key = MESH_NO_OOB;
			set_start_para_by_capa(&prov_oob);
		}
	}
	#else
	// just for the test 
	pro_trans_start *p_start = &(prov_oob.start);
	p_start->alogrithms = 0;
	p_start->pubKey =  MESH_PUB_KEY_WITHOUT_OOB;
	p_start->authMeth = MESH_NO_OOB;
	p_start->authAct =0;
    p_start->authSize = 0;
	#endif
	set_pro_start_simple(p_send,&(prov_oob.start));
	memcpy(gatt_pro_input+12,&(p_send->start.alogrithms),5);
	gatt_para_len = sizeof(pro_trans_start);
	LOG_MSG_INFO(TL_LOG_GATT_PROVISION,(u8*)&prov_oob.start,sizeof(prov_oob.start),"SEND:the provision start is \r\n",0);

}

void gatt_prov_send_pubkey(mesh_pro_data_structer *p_rcv,mesh_pro_data_structer *p_send)
{
    // send the pub_key pkt 
	gatt_set_pro_pub_key(p_send,gatt_pro_ppk,gatt_pro_ppk+32);
	gatt_para_len = sizeof(pro_trans_pubkey);
	LOG_MSG_INFO(TL_LOG_GATT_PROVISION,gatt_para_pro+1,(u8)gatt_para_len-1,"SEND:provisioner send pubkey is \r\n",0);
	
}

void gatt_prov_rcv_pubkey(mesh_pro_data_structer *p_rcv)
{
	memcpy(gatt_pro_input+0x11,gatt_pro_ppk,sizeof(gatt_pro_ppk));
	memcpy(gatt_pro_input+0x11+64,p_rcv->pubkey.pubKeyX,64);
	LOG_MSG_INFO(TL_LOG_GATT_PROVISION,p_rcv->pubkey.pubKeyX,64,"RCV:the pubkey of the device is \r\n",0);
	tn_p256_dhkey (gatt_pro_edch, gatt_pro_psk,gatt_pro_input+0x11+0x40 ,gatt_pro_input+0x11+0x60);    
}

void gatt_prov_send_comfirm(mesh_pro_data_structer *p_send)
{
	mesh_sec_prov_confirmation_sec (gatt_pro_comfirm, gatt_pro_input, 
								145, gatt_pro_edch, gatt_pro_random, gatt_pro_auth);
	gatt_set_pro_comfirm(p_send,gatt_pro_comfirm,get_prov_comfirm_value_len());
	gatt_para_len =get_prov_comfirm_len();
	LOG_MSG_INFO(TL_LOG_GATT_PROVISION,gatt_para_pro+1,(u8)gatt_para_len-1,"SEND:the provisioner's comfirm is \r\n",0);    
}

void gatt_prov_rcv_comfirm(mesh_pro_data_structer *p_rcv,mesh_pro_data_structer *p_send)
{
	pro_trans_comfirm *p_comfirm = &(p_rcv->comfirm);
	#if PROV_AUTH_LEAK_REFLECT_EN
	if(prov_comfirm_check_same_or_not(p_comfirm->comfirm,gatt_pro_comfirm)){
		mesh_proxy_master_terminate_cmd();
	}
	#endif
	// store the dev comfrim part 
	LOG_MSG_INFO(TL_LOG_GATT_PROVISION,p_comfirm->comfirm,get_prov_comfirm_value_len(),"RCV:the device's comfirm is \r\n",0);
	memcpy(gatt_dev_comfirm,p_comfirm->comfirm,get_prov_comfirm_value_len());    
}

void gatt_prov_send_random(mesh_pro_data_structer *p_rcv,mesh_pro_data_structer *p_send)
{
    gatt_set_pro_random(p_send,gatt_pro_random,get_prov_random_len()-1);
	gatt_para_len = get_prov_random_len();
	LOG_MSG_INFO(TL_LOG_GATT_PROVISION,gatt_para_pro+1,(u8)gatt_para_len-1,"SEND:the provisioner's random is \r\n",0);    
}

void gatt_prov_rcv_random(mesh_pro_data_structer *p_rcv,mesh_pro_data_structer *p_send)
{
	memcpy(gatt_dev_random,p_rcv->random.random,get_prov_random_value_len());
	LOG_MSG_INFO(TL_LOG_GATT_PROVISION,p_rcv->random.random,get_prov_random_value_len(),"RCV:the device's random is \r\n",0);
	// caculate the dev comfirm part ,and compare the comfirm 
	
	u8 gatt_dev_comfirm_tmp[32];
	mesh_sec_prov_confirmation_sec (gatt_dev_comfirm_tmp, gatt_pro_input, 
							145, gatt_pro_edch, gatt_dev_random, gatt_pro_auth);
	if(prov_comfirm_check_right_or_not(gatt_dev_comfirm,gatt_dev_comfirm_tmp)){
		LOG_MSG_INFO(TL_LOG_GATT_PROVISION,gatt_dev_comfirm_tmp,16,"provision comfirm fail\r\n",0);
		mesh_proxy_master_terminate_cmd();
	}
	LOG_MSG_INFO(TL_LOG_GATT_PROVISION,0,0,"the device comfirm check is success",0);
	//calculate the dev_key part 
	u8 prov_salt[16];
	mesh_sec_prov_salt_fun(prov_salt,gatt_pro_input,gatt_pro_random,gatt_dev_random,is_prov_oob_hmac_sha256());
	//calculate the dev_key part 
	mesh_sec_dev_key(gatt_dev_key,prov_salt,gatt_pro_edch);
	   
}

void gatt_prov_send_pro_data(mesh_pro_data_structer *p_rcv,mesh_pro_data_structer *p_send)
{
    gatt_provision_net_info_callback();
	LOG_MSG_INFO(TL_LOG_GATT_PROVISION,gatt_pro_dat,25,"SEND:the provisioner's device info is  \r\n",0);
	u16 unicast_address =gatt_pro_dat[23]+(gatt_pro_dat[24]<<8);
	#if (IS_VC_PROJECT_MASTER)
	LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"gatt_prov_send_pro_data set provision data",0);
		#if MD_REMOTE_PROV
		if(mesh_prov_dkri_is_valid()){
			mesh_prov_set_adr_dev_candi(unicast_address,gatt_dev_key);
		}else{
			VC_node_dev_key_save(unicast_address, gatt_dev_key,gatt_node_ele);
		}
		#else
		VC_node_dev_key_save(unicast_address, gatt_dev_key,gatt_node_ele);
		#endif
	#endif 
	LOG_MSG_INFO(TL_LOG_GATT_PROVISION,gatt_dev_key,16,"the node's dev key: \r\n",0);
	mesh_sec_prov_session_key_fun (gatt_pro_session_key, gatt_pro_session_nonce_tmp, gatt_pro_input, 145, gatt_pro_edch, gatt_pro_random, gatt_dev_random,is_prov_oob_hmac_sha256());
	memcpy(gatt_pro_session_nonce,gatt_pro_session_nonce_tmp+3,13); 
    u8 gatt_pro_dat_tmp[40];
	memcpy(gatt_pro_dat_tmp,gatt_pro_dat,sizeof(gatt_pro_dat_tmp));
	mesh_prov_sec_msg_enc (gatt_pro_session_key, gatt_pro_session_nonce, gatt_pro_dat_tmp, 25, 8);
	gatt_set_pro_data(p_send,gatt_pro_dat_tmp,gatt_pro_dat_tmp+25);
	gatt_para_len = sizeof(pro_trans_data);    
}

void gatt_prov_rcv_pro_complete()
{
	LOG_MSG_INFO(TL_LOG_GATT_PROVISION,0,0,"RCV:rcv the provision completet cmd,provision success",0);
}

void gatt_prov_send_record_get(pro_trans_record_get *p_rec_get)
{
	
	set_pro_record_get(p_rec_get);
	gatt_para_len = sizeof(pro_trans_record_get); 
	LOG_MSG_INFO(TL_LOG_GATT_PROVISION,(u8 *)p_rec_get,gatt_para_len,"prov send record cmd is  \r\n",0);
}

void gatt_prov_rcv_record_list(pro_trans_record_list *p_rec_list,u8 len)
{
	prov_clear_all_rec();
	int rec_cnt = (len -3)/2;
	// switch the endiness first 
	for(int i=0;i<rec_cnt;i++){
		endianness_swap_u16((u8*)(p_rec_list->rec_id+i));
	}
	prov_set_rec_id(p_rec_list->rec_id,rec_cnt);
	record_mag_init();
	record_mag_set(DEV_CERTIFY_ID,MAX_PROV_FRAG_SIZE,0);
}

void gatt_prov_send_record_request(pro_trans_record_request *p_rec_req)
{
	// firstly ,we suppose will only get the dev certify.
	u16 rec_id,max_size,offset;
	record_mag_get(&rec_id,&max_size,&offset);
	set_pro_record_request(p_rec_req,rec_id,offset,max_size);
	gatt_para_len = sizeof(pro_trans_record_request); 
	LOG_MSG_INFO(TL_LOG_GATT_PROVISION,(u8 *)p_rec_req,sizeof(pro_trans_record_request),
							"send record request cmd is ",0);
}

int gatt_prov_rcv_record_rsp(pro_trans_record_rsp *p_rec_rsp,u8 rec_data_len)
{
	// need to swap endinesss first 
	u16 max_size,frag_off;
	if(p_rec_rsp->sts != RECORD_PROV_SUC){
		return 2;
	}
	record_mag_get_max_size(&max_size);
	endianness_swap_u16((u8 *)(&p_rec_rsp->rec_id));
	endianness_swap_u16((u8 *)(&p_rec_rsp->frag_off));
	endianness_swap_u16((u8 *)(&p_rec_rsp->total_len));
	// set the record buf first 
	prov_set_buf_len(p_rec_rsp->rec_id,p_rec_rsp->frag_off,p_rec_rsp->data,rec_data_len,p_rec_rsp->total_len);
	if((p_rec_rsp->frag_off + rec_data_len) >= p_rec_rsp->total_len){
		// it means gatt record get end
		record_mag_init();
		return 0; //record proc end 
	}
	frag_off = p_rec_rsp->frag_off+rec_data_len;
	record_mag_set(p_rec_rsp->rec_id,max_size,frag_off);
	return 1;
}


u8 win32_prov_working()
{
    u8 state = gatt_provision_mag.provison_send_state;
    if(state>=STATE_PRO_INVITE && state <= STATE_PRO_SUC){
        return 1;
    }else{
        return 0;
    }
}

void gatt_rcv_pro_pkt_dispatch(u8 *p ,u8 len )
{
	mesh_pro_data_structer *p_rcv = (mesh_pro_data_structer *)(p);
	mesh_pro_data_structer *p_send = (mesh_pro_data_structer *)gatt_para_pro;
	if(p_rcv->invite.header.type == PRO_FAIL){
		gatt_prov_rcv_pro_fail(p_rcv);
	}
	switch(gatt_provision_mag.provison_send_state){
		case STATE_PRO_REC_GET_ACK:
			if(p_rcv->invite.header.type == PRO_REC_LIST){
				// get the record id part 
				LOG_MSG_INFO(TL_LOG_GATT_PROVISION,(u8*)&(p_rcv->rec_list),
									len,"pro rcv rec list is \r\n",0);
				gatt_prov_rcv_record_list(&(p_rcv->rec_list),len);
				// send the record req cmd part
				gatt_prov_send_record_request(&(p_send->rec_req));
				prov_write_data_trans(gatt_para_pro,gatt_para_len,MSG_PROVISION_PDU);
				gatt_provision_mag.provison_send_state = STATE_DEV_REC_RSP_ACK;
			}
			break;
		case STATE_DEV_REC_RSP_ACK:
			if(p_rcv->invite.header.type == PRO_REC_RSP){
				// until receive all the record rsp ,then jump to the  invite send part 
				LOG_MSG_INFO(TL_LOG_GATT_PROVISION,(u8 *)&(p_rcv->rec_rsp),
									len,"pro rcv rec rsp is \r\n",0);
				u8 rec_data_len = len - 8;
				int ret = gatt_prov_rcv_record_rsp(&(p_rcv->rec_rsp),rec_data_len);
				if(ret == 0){
					// jump to the invite proc part
					gatt_prov_send_invite();
					prov_write_data_trans(gatt_para_pro,gatt_para_len,MSG_PROVISION_PDU);
					gatt_provision_mag.provison_send_state = STATE_PRO_INVITE_ACK;
				}else if (ret == 1){
					// send the record req cmd part
					gatt_prov_send_record_request(&(p_send->rec_req));
					prov_write_data_trans(gatt_para_pro,gatt_para_len,MSG_PROVISION_PDU);
				}else if (ret == 2){
					LOG_MSG_INFO(TL_LOG_GATT_PROVISION,0,0,"provision comfirm fail\r\n",0);
		    		mesh_proxy_master_terminate_cmd();
				}
			}break;
		case STATE_PRO_INVITE_ACK:
			if(p_rcv->invite.header.type == PRO_CAPABLI){
				// send the provison start packet 
				prov_timer_start();// monitor the provision timeout proc 
				gatt_prov_rcv_capa(p_rcv,p_send);
				gatt_prov_send_start(p_rcv,p_send);
				#if PROV_AUTH_LEAK_RECREATE_KEY_EN
				init_ecc_key_pair(1);
				#endif
				prov_write_data_trans(gatt_para_pro,gatt_para_len,MSG_PROVISION_PDU);
				gatt_prov_send_pubkey(p_rcv,p_send);
				prov_write_data_trans(gatt_para_pro,gatt_para_len,MSG_PROVISION_PDU);
				gatt_provision_mag.provison_send_state = STATE_PRO_PUB_KEY_ACK;
				if(certify_base_en){
					// proc the pubkey part 
					mesh_pro_data_structer cert_pubkey;
					cert_pubkey.pubkey.header.type = PRO_PUB_KEY;
					prov_use_rec_id_get_pubkey(DEV_CERTIFY_ID,cert_pubkey.pubkey.pubKeyX);
					gatt_prov_rcv_pubkey(&cert_pubkey);
                	gatt_prov_send_comfirm(p_send);
					prov_write_data_trans(gatt_para_pro,gatt_para_len,MSG_PROVISION_PDU);
					gatt_provision_mag.provison_send_state = STATE_PRO_CONFIRM_ACK;
				}
			}	
			break;
		case STATE_PRO_PUB_KEY_ACK:
			if(p_rcv->invite.header.type == PRO_PUB_KEY){
                gatt_prov_rcv_pubkey(p_rcv);
                gatt_prov_send_comfirm(p_send);
				prov_write_data_trans(gatt_para_pro,gatt_para_len,MSG_PROVISION_PDU);
				gatt_provision_mag.provison_send_state = STATE_PRO_CONFIRM_ACK;
			}
			break;
		case STATE_PRO_CONFIRM_ACK:
			if(p_rcv->invite.header.type == PRO_CONFIRM){
                gatt_prov_rcv_comfirm(p_rcv,p_send);
				gatt_prov_send_random(p_rcv,p_send);
				prov_write_data_trans(gatt_para_pro,gatt_para_len,MSG_PROVISION_PDU);
				gatt_provision_mag.provison_send_state = STATE_PRO_RANDOM_ACK;
			}
			break;
		case STATE_PRO_RANDOM_ACK:
			if(p_rcv->invite.header.type == PRO_RANDOM){
				// send the data packet 
                gatt_prov_rcv_random(p_rcv,p_send);
                gatt_prov_send_pro_data(p_rcv,p_send);
				prov_write_data_trans(gatt_para_pro,gatt_para_len,MSG_PROVISION_PDU);
				gatt_provision_mag.provison_send_state = STATE_PRO_COMPLETE_ACK;
				// then send the data packet 	
			}
			break;
		case STATE_PRO_COMPLETE_ACK:
			if(p_rcv->invite.header.type == PRO_COMPLETE){
			    prov_timer_clr();
				#if WIN32
				gatt_prov_rcv_pro_complete();
				#endif 
				provision_end_callback(PROV_NORMAL_RET);
				gatt_provision_mag.provison_send_state = STATE_PRO_SUC;
				// set filter internal 
				#if VC_APP_ENABLE	// because VC no need disconnect BLE.
				mesh_proxy_set_filter_init(ele_adr_primary);
				filter_send_flag =0;
				#endif
			}
			break;
		default:
			break;
	}
}

void proxy_dispatch_pkt(u8 *p,u8 len,u8* proxy_buf,u16 proxy_len)
{
	u8 tmp[0x40];
	u8 l2cap_len;
	l2cap_len = len+3;
	tmp[0]=0;
	tmp[1]=l2cap_len+4;//rf_len 
	tmp[2]=l2cap_len;
	tmp[3]=0;
	tmp[4]=4;
	tmp[5]=0;
	tmp[6]=0x1b;
	tmp[7]=0x11;
	tmp[8]=0;
	memcpy(tmp+9,p,len);
	if(gatt_pkt_pb_gatt_data(tmp,0,0,proxy_buf,&proxy_len)== MSG_COMPOSITE_WAIT){
			return;
		}
	
}
u8 provision_dispatch_direct(u8 *p,u8 len,u8* proxy_buf,u16* p_proxy_len)
{
	//pb_gatt_proxy_str *p_str;
	u8 type;
	// the packet is the pb-gatt packet 
	type = gatt_pkt_pb_gatt_data(p,1,len,proxy_buf,p_proxy_len);
	if(type == MSG_COMPOSITE_WAIT){
			return MSG_COMPOSITE_WAIT;
		}
#if VC_APP_ENABLE
		provision_flow_Log(INPUT_LOG_AND,proxy_buf,*p_proxy_len);
#endif
	
	return type;
}


void provision_dispatch(u8 *p,u8* proxy_buf,u16 proxy_len)
{
	//pb_gatt_proxy_str *p_str;

	if(p[4]==0x04&&p[6]==0x1b&&p[7]==0x11){
	// the packet is the pb-gatt packet 
		if(gatt_pkt_pb_gatt_data(p,0,0,proxy_buf,&proxy_len)== MSG_COMPOSITE_WAIT){
			return;
		}
#if VC_APP_ENABLE
		provision_flow_Log(INPUT_LOG_AND,proxy_buf,proxy_len);
#endif
		gatt_rcv_pro_pkt_dispatch(proxy_buf,(u8)proxy_len);
	}
}
void provision_dispatch_pkt(u8 *p ,u8 len,u8* proxy_buf,u16 proxy_len)
{
	u8 tmp[0x40];
	u8 l2cap_len;
	l2cap_len = len+3;
	tmp[0]=0;
	tmp[1]=l2cap_len+4;//rf_len 
	tmp[2]=l2cap_len;
	tmp[3]=0;
	tmp[4]=4;
	tmp[5]=0;
	tmp[6]=0x1b;
	tmp[7]=0x11;
	tmp[8]=0;
	memcpy(tmp+9,p,len);
	provision_dispatch(tmp,proxy_buf,proxy_len);
	return ;
}


