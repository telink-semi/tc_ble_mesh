/********************************************************************************************************
 * @file	app_provison.c
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
#include "proj_lib/ble/blt_config.h"
#include "app_provison.h"
#include "app_beacon.h"
#include "proj_lib/mesh_crypto/le_crypto.h"
#include "proj_lib/mesh_crypto/mesh_crypto.h"
#include "proj_lib/mesh_crypto/mesh_md5.h"
#include "proj_lib/mesh_crypto/sha256_telink.h"

#if WIN32 
#include "../../../reference/tl_bulk/lib_file/app_config.h"
#include "../../../reference/tl_bulk/lib_file/hw_fun.h"
#include "../../../reference/tl_bulk/lib_file/gatt_provision.h"
#else 
#include "vendor/common/mi_api/telink_sdk_mible_api.h"
#endif
#include "vendor/common/remote_prov.h"
#include "vendor/common/certify_base/certify_base_crypto.h"
#include "solicitation_rpl_cfg_model.h"
#if (__TLSR_RISCV_EN__)
#include "stack/ble/ble.h"
#endif
#if SMART_PROVISION_ENABLE
#include "vendor/common/smart_provision.h"
#endif

#define DEBUG_PROXY_SAR_ENABLE 	0
prov_para_proc_t prov_para;
#if TESTCASE_FLAG_ENABLE
_attribute_no_retention_bss_ mesh_cmd_bear_t		pro_adv_pkt;
#endif

pro_para_mag  provision_mag;
u8 prov_link_cls_code;
u8 prov_link_uuid[16];
u8 para_pro[PROVISION_GATT_MAX_LEN]; //it's also used in proxy_gatt_Write(), but network payload is less then 31, because it will be relayed directly.
u8 para_len;

#define OFFSET_START	0x1F
#define OFFSET_CON		0x17
#if CERTIFY_BASE_ENABLE&&GATEWAY_ENABLE // change the pubkey in the newest version.PVNR/CBP/BV-01C.PVNR/CBP/BV-03C
// the cert has already changed ,so need to change the pubkey .
const u8 pub_key_with_oob[64]={
	0x31,0x7a,0x6f,0x16,0x58,0x44,0x72,0x74,0x15,0x10,0x33,0x62,0x5a,0xfb,0xc4,0xf1,
	0xb6,0x0b,0x31,0xa4,0x5d,0x6f,0x45,0x9b,0x50,0x76,0xe1,0x11,0x86,0x44,0xbd,0xbd,
	0x26,0x5e,0x7f,0x37,0xbf,0xfd,0xf9,0xc4,0x34,0x90,0x60,0x23,0x7f,0x57,0x66,0x0e,
	0xae,0x86,0xd4,0xd0,0x58,0x7c,0xef,0x36,0xd9,0x3b,0xa7,0xd0,0x2b,0x54,0x5d,0xe2
};

/*const u8 pub_key_with_oob[64]={
	0x31,0x7A,0x6F,0x16,0x58,0x44,0x72,0x74,0x15,0x10,0x33,0x62,0x5A,0xFB,0xC4,0xF1,
	0xB6,0x0B,0x31,0xA4,0x5D,0x6F,0x45,0x9B,0x50,0x76,0xE1,0x11,0x86,0x44,0xBD,0xBD,
	0x26,0x5E,0x7F,0x37,0xBF,0xFD,0xF9,0xC4,0x34,0x90,0x60,0x23,0x7F,0x57,0x66,0x0E,
	0xAE,0x86,0xD4,0xD0,0x58,0x7C,0xEF,0x36,0xD9,0x3B,0xA7,0xD0,0x2B,0x54,0x5D,0xE2
};*/
#else
// can not use the pubkey as the spec ,or it will fail 
//spec:F465E43FF23D3F1B9DC7DFC04DA8758184DBC966204796ECCF0D6CF5E16500CC0201D048BCBBD899EEEFC424164E33C201C2B010CA6B4D43A8A155CAD8ECB279
/*
const u8 pub_key_with_oob[64]={
					0xF4,0x65,0xE4,0x3F,0xF2,0x3D,0x3F,0x1B,
					0x9D,0xC7,0xDF,0xC0,0x4D,0xA8,0x75,0x81,
					0x84,0xDB,0xC9,0x66,0x20,0x47,0x96,0xEC,
					0xCF,0x0D,0x6C,0xF5,0xE1,0x65,0x00,0xCC,
					0x02,0x01,0xD0,0x48,0xBC,0xBB,0xD8,0x99,
					0xEE,0xEF,0xC4,0x24,0x16,0x4E,0x33,0xC2,
					0x01,0xC2,0xB0,0x10,0xCA,0x6B,0x4D,0x43,
					0xA8,0xA1,0x55,0xCA,0xD8,0xEC,0xB2,0x79};
*/
// if it use the oob in the gateway we should write the psk,ppk
// psk 000000000000006800ffffff000000010000000100000001c830ffff44332222
// ppk 433d397d933dccca5ee860c5860b30dbaa11923be8b40d3922709988428a5a6a10dbf87ca28fe7498c59a2ff133e20d1e2f36401f31f0e6d2c420d0ba1405a30


const u8 pub_key_with_oob[64]={
	0x43,0x3d,0x39,0x7d,0x93,0x3d,0xcc,0xca,0x5e,0xe8,0x60,0xc5,0x86,0x0b,0x30,0xdb,
	0xaa,0x11,0x92,0x3b,0xe8,0xb4,0x0d,0x39,0x22,0x70,0x99,0x88,0x42,0x8a,0x5a,0x6a,
	0x10,0xdb,0xf8,0x7c,0xa2,0x8f,0xe7,0x49,0x8c,0x59,0xa2,0xff,0x13,0x3e,0x20,0xd1,
	0xe2,0xf3,0x64,0x01,0xf3,0x1f,0x0e,0x6d,0x2c,0x42,0x0d,0x0b,0xa1,0x40,0x5a,0x30
};

#endif



// add the provision data part dispatch 

mesh_prov_oob_str prov_oob;

STATIC_ASSERT((PROXY_GATT_MAX_LEN >= PROVISION_GATT_MAX_LEN) && (PROXY_GATT_MAX_LEN <= 255));
#if 0 // MESH_DLE_MODE
STATIC_ASSERT(PROXY_GATT_MAX_LEN >= BEAR_EXTEND_MAX);
#endif


void provision_mag_cfg_s_store()
{
	mesh_common_store(FLASH_ADR_PROVISION_CFG_S);
}

u8 mesh_provision_cmd_process()
{
	// add the test part ,how to send ,and the test_script part		
	unprov_beacon_send(MESH_UNPROVISION_BEACON_WITH_URI,0);
    send_rcv_retry_set(PRO_BEACON,0,0);
	return 1;
}

#define GATT_PROVISION_TIMEOUT 	20*1000*1000 // 10S
u32 provision_timeout_tick=0;

void set_prov_timeout_tick(u32 tick)
{
    provision_timeout_tick = tick;
}

void provision_timeout_cb()
{
	if(provision_timeout_tick && clock_time_exceed(provision_timeout_tick,GATT_PROVISION_TIMEOUT)){
		provision_timeout_tick =0;
		mesh_node_prov_event_callback(EVENT_MESH_NODE_RC_LINK_TIMEOUT);
	}
}

void set_pb_gatt_adv(u8 *p,u8 flags)
{
	PB_GATT_ADV_DAT *p_pb_adv;
	u8 temp_uuid[2] = WRAPPING_BRACES(SIG_MESH_PROVISION_SERVICE);
	p_pb_adv = (PB_GATT_ADV_DAT *)(p);
	p_pb_adv->flag_len = 0x02;
	p_pb_adv->flag_type = 0x01;
	p_pb_adv->flag_content = flags;
	p_pb_adv->uuid_len= 0x03;
	p_pb_adv->uuid_type=0x03;
	p_pb_adv->uuid_pb_uuid[0] = temp_uuid[0];
	p_pb_adv->uuid_pb_uuid[1] = temp_uuid[1];
	p_pb_adv->service_len = 0x15;
	p_pb_adv->service_type = 0x16;
	p_pb_adv->service_uuid[0] = temp_uuid[0];
	p_pb_adv->service_uuid[1] = temp_uuid[1];
	memcpy(p_pb_adv->service_data,prov_para.device_uuid,16);
	memcpy(p_pb_adv->oob_info , prov_para.oob_info,2);// use the small endianness
	return;
}


#if PROVISIONER_GATT_ADV_EN
const u8	provisioner_advData[] = {
	0x02, 0x01, 0x06, 					// DT_FLAGS	// normal discoverable mode and BR/EDR not supported
	0x0c, 0x09, 'p', 'r', 'o', 'v','i','s','i','o','n','e','r',		// DT_COMPLETE_LOCAL_NAME // name
	#if MESH_CDTP_ENABLE
	0x03, 0x02,   0x25, 0x18,			// DT_INCOMPLT_LIST_16BIT_SERVICE_UUID // incomplete list of service class UUIDs (0x1812, 0x180F)
	#endif
};

int set_adv_provisioner(rf_packet_adv_t * p) 
{
	#if MESH_CDTP_ENABLE
	extern u8 CDTP_ots_gatt_adv_en;
	if(0 == CDTP_ots_gatt_adv_en){
		return 0;
	}
	#endif
	
	p->header.type = LL_TYPE_ADV_IND;
	memcpy(p->advA,tbl_mac,6);
	memcpy(p->data, provisioner_advData, sizeof(provisioner_advData));
	p->rf_len = 6 + sizeof(provisioner_advData);
	p->dma_len = p->rf_len + 2;	
	
	return 1;
}
#endif

#if(PTS_TEST_EN && MD_CLIENT_EN && MD_SOLI_PDU_RPL_EN)
int set_adv_solicitation(rf_packet_adv_t * p) 
{
	static u32 solici_pdu_tick = 0;
	if(soli_pdu_adv_cnt && clock_time_exceed(solici_pdu_tick, SOLICI_PDU_INTERVAL_MS*1000)){
		soli_pdu_adv_cnt--;
		solici_pdu_tick = clock_time();
		static soli_pdu_pkt_t soli_pkt;
		soli_pkt.flag_len = 0x02;
		soli_pkt.flag_type = GAP_ADTYPE_FLAGS;
		soli_pkt.flag_content = 0x05;
		soli_pkt.len = 3;
		soli_pkt.type = GAP_ADTYPE_16BIT_COMPLETE;
		soli_pkt.uuid = SIG_MESH_PROXY_SOLI_VAL;
		soli_pkt.service_len = sizeof(soli_srv_dat_t) + 3;
		soli_pkt.service_type = GAP_ADTYPE_SERVICE_DATA_UUID_16BIT;
		soli_pkt.service_uuid = SIG_MESH_PROXY_SOLI_VAL;
		memcpy(&soli_pkt.service_data, &soli_service_data, sizeof(soli_pkt.service_data));
		
		p->header.type = LL_TYPE_ADV_NONCONN_IND;
		memcpy(p->advA,tbl_mac,6);	
		memcpy(p->data, &soli_pkt, sizeof(soli_pkt));
		p->rf_len = 6 + sizeof(soli_pkt);
		p->dma_len = p->rf_len + 2;	
		return 1;
	}
	return 0;
}
#endif


void set_adv_provision(rf_packet_adv_t * p) 
{
	set_pb_gatt_adv(p->data,6);
	p->header.type = LL_TYPE_ADV_IND;
	memcpy(p->advA,tbl_mac,6);
	memcpy(p->data, p->data, 29);
	p->rf_len = 6 + 29;
	p->dma_len = p->rf_len + 2;	
	return ;
} 

#if URI_DATA_ADV_ENABLE
void set_adv_uri_unprov_beacon(rf_packet_adv_t * p)
{
	p->header.type = LL_TYPE_ADV_NONCONN_IND;
	memcpy(p->advA,tbl_mac,6);
	u8 uri_data[]=URI_DATA;
	p->data[0]=1+sizeof(uri_data);
	p->data[1]=AD_TYPE_URI;
	memcpy(p->data+2,uri_data,sizeof(uri_data));
	p->rf_len = 6 + sizeof(uri_data)+2;
	p->dma_len = p->rf_len + 2; 
}
#endif

#if !WIN32
u8 set_adv_proxy(rf_packet_adv_t * p)
{
	u8 dat_len =0;
	u8 ret =0;
	foreach(i,NET_KEY_MAX){
		static u8 proxy_key_idx = 0;
		proxy_key_idx = proxy_key_idx % NET_KEY_MAX;
		mesh_net_key_t *p_netkey = &mesh_key.net_key[proxy_key_idx++][0];
		if(p_netkey->valid){
			ret = set_proxy_adv_pkt(p->data ,prov_para.random,p_netkey,&dat_len);
			break;
		}
	}
	if(ret == 1){
		p->header.type = LL_TYPE_ADV_IND;
		#if (MD_PRIVACY_BEA && PRIVATE_PROXY_FUN_EN) // set to be RPA in private mode.
		if(mesh_get_proxy_privacy_para()){
			memcpy(p->advA, prov_para.priv_non_resolvable, 6);
			p->header.txAddr = 1;
			memcpy(pkt_scan_rsp.advA, prov_para.priv_non_resolvable, 6);	
			pkt_scan_rsp.header.txAddr = 1;
		}else
		#endif
		{
			memcpy(p->advA,tbl_mac,6);
			#if (MD_PRIVACY_BEA && PRIVATE_PROXY_FUN_EN) // restore to public address when not at private mode.
			p->header.txAddr = 0;
			memcpy(pkt_scan_rsp.advA, tbl_mac, 6);
			pkt_scan_rsp.header.txAddr = 0;
			#endif
		}
		//memcpy(p->data, p->data, dat_len);
		p->rf_len = 6 + dat_len;
		p->dma_len = p->rf_len + 2;	
		return 1;
	}else{
		return 0;
	}
}
#endif

#if DUAL_MESH_SIG_PVT_EN
void set_private_mesh_adv(rf_packet_adv_t * p)
{
	gatt_adv_private_mesh_t *p_adv = (gatt_adv_private_mesh_t *)p->data;
	u8 name[]=	MESH_NAME;
	u8 name_len = sizeof(name) - 1;
	
	p_adv->flag_len = 2;
	p_adv->flag_type = GAP_ADTYPE_FLAGS;
	p_adv->flag_content = 5;
	p_adv->name_len = name_len + 1;
	p_adv->name_type = GAP_ADTYPE_LOCAL_NAME_COMPLETE;
	
	memcpy(p_adv->par, name, name_len);
	
	private_mesh_manu_t *p_manu = (private_mesh_manu_t *)&p_adv->par[name_len];
	p_manu->manu_len = sizeof(private_mesh_manu_t)-1;
	p_manu->manu_type = GAP_ADTYPE_MANUFACTURER_SPECIFIC;
	p_manu->cid =  VENDOR_ID;
	p_manu->pid = VENDOR_ID;
	memcpy(p_manu->mac, tbl_mac, 4);

	p->header.type = LL_TYPE_ADV_IND;
	memcpy(p->advA,tbl_mac,6);
	p->rf_len = 6 + p_adv->flag_len+p_adv->name_len+p_manu->manu_len+3;
	p->dma_len = p->rf_len + 2; 
}
#endif

void mesh_provision_para_reset()
{
	// test para init 
#if FEATURE_PROV_EN
    #if MD_REMOTE_PROV
    if(mesh_pr_sts_work_or_not()&&is_provision_success()){
        return ;// remote prov cannot clear the para for the remoteprov server part 
    }
    #endif
	//LOG_MSG_INFO(TL_LOG_PROVISION,0, 0 ,"mesh_provision_para_reset");	
	memset((u8 *)(&prov_para),0,OFFSETOF(prov_para_proc_t, rsp_ack_transnum));
	prov_para.provison_send_state = LINK_UNPROVISION_STATE;
	prov_para.provison_rcv_state= LINK_UNPROVISION_STATE;
	#if (__PROJECT_MESH_PRO__ || __PROJECT_MESH_GW_NODE__)
	prov_para.initial_pro_roles = MESH_INI_ROLE_PROVISIONER;
	#if(__PROJECT_MESH_GW_NODE__)
	if(!is_provision_success()){
		prov_para.initial_pro_roles = MESH_INI_ROLE_NODE;
	}
	#endif
	#else
	prov_para.initial_pro_roles = MESH_INI_ROLE_NODE;
	#endif
	if(provision_mag.gatt_mode != GATT_PROXY_MODE){
		#if PB_GATT_ENABLE 
		provision_mag.gatt_mode = GATT_PROVISION_MODE;
		#else
		provision_mag.gatt_mode = GATT_ADV_NORMAL_MODE;
		#endif 
	}
#endif 
	return ;

}

void reset_uuid_create_flag()
{
	// save provision_mag struct part 
	provision_mag_cfg_s_store();
}

void link_open_init(pro_PB_ADV* p_pb_adv ,u8 transnum,u8 *p_uuid,u8 *p_link)
{
	p_pb_adv->length = 7+sizeof(bear_open_link);
	p_pb_adv->ad_type = MESH_ADV_TYPE_PRO;
	memcpy(p_pb_adv->link_ID,p_link,4);
	p_pb_adv->trans_num = transnum;
	p_pb_adv->transBear.bearOpen.header.BearCtl =LINK_OPEN;
	p_pb_adv->transBear.bearOpen.header.GPCF = BEARS_CTL;
	memcpy(p_pb_adv->transBear.bearOpen.DeviceUuid,p_uuid,16);
}

void link_ack_init(pro_PB_ADV* p_pb_adv,u8 transnum,u8 *p_link)
{
	p_pb_adv->length = 7+ sizeof(bear_ack_link);
	p_pb_adv->ad_type = MESH_ADV_TYPE_PRO;
	memcpy(p_pb_adv->link_ID,p_link,4);
	p_pb_adv->trans_num = transnum;
	p_pb_adv->transBear.bearOpen.header.BearCtl =LINK_ACK;
	p_pb_adv->transBear.bearOpen.header.GPCF = BEARS_CTL;
}

u8 set_pro_rec_get(mesh_pro_data_t *p_str)
{
	pro_trans_record_get *p_rec_get = &(p_str->rec_get);
	p_rec_get->header.type = PRO_REC_GET;
	return 1;
}

u8 set_pro_rec_req(mesh_pro_data_t *p_str,mesh_rec_mag_rec_t * p_rec_mag)
{
	pro_trans_record_request *p_rec_req = &(p_str->rec_req);
	p_rec_req->header.type = PRO_REC_REQ;
	p_rec_req->rec_id = p_rec_mag->rec_id;
	p_rec_req->frag_off = p_rec_mag->offset;
	p_rec_req->frag_max_size = p_rec_mag->frag_size;
	endianness_swap_u16((u8 *)(&p_rec_req->rec_id));
	endianness_swap_u16((u8 *)(&p_rec_req->frag_off));
	endianness_swap_u16((u8 *)(&p_rec_req->frag_max_size));
	return 1;
}

u8 set_pro_invite(mesh_pro_data_t *p_str,u8 att_dura)
{
	pro_trans_invite *p_invite = &(p_str->invite);
	p_invite->header.type = PRO_INVITE;
	p_invite->attentionDura = att_dura ;
	return 1;
}
u8 swap_mesh_pro_capa(mesh_pro_data_t *p_str)
{
	endianness_swap_u16((u8 *)(&p_str->capa.algorithms));
	endianness_swap_u16((u8 *)(&p_str->capa.outPutOOBAct));
	endianness_swap_u16((u8 *)(&p_str->capa.inOOBAct));
	return 1;
}

u8 set_pro_capa_cpy(mesh_pro_data_t *p_str,mesh_prov_oob_str *p_prov_oob)
{
	pro_trans_capa * p_capa = &(p_str->capa);
	p_capa->header.type = PRO_CAPABLI;
	p_capa->header.rfu= 0;
	memcpy((u8 *)&(p_capa->ele_num),(u8 *)&(p_prov_oob->capa.ele_num),sizeof(pro_trans_capa)-1);
	return 1;
}
u8 set_pro_capa(mesh_pro_data_t *p_str,u8 ele_num,u16 alogr,u8 keytype,
				u8 outOOBsize,u16 outAct,u8 inOOBsize,u16 inOOBact)
{
	pro_trans_capa *p_capa =  &(p_str->capa);
	p_capa->header.type = PRO_CAPABLI;
	p_capa->ele_num = ele_num;
	p_capa->algorithms = alogr;
	p_capa->pubKeyType = keytype;
	p_capa->staticOOBType= 0;
	p_capa->outPutOOBSize = outOOBsize;
	p_capa->outPutOOBAct = outAct ;
	p_capa->inOOBSize = inOOBsize;
	p_capa->inOOBAct = inOOBact ;
	return 1;
}
u8 set_pro_start_simple(mesh_pro_data_t *p_str ,pro_trans_start *p_start)
{
	return set_pro_start(p_str,p_start->algorithms,p_start->pubKey,
			p_start->authMeth,p_start->authAct,p_start->authSize);

}
u8 set_pro_start(mesh_pro_data_t *p_str,u8 alogri,u8 pubkey,
					u8 authmeth,u8 authact,u8 authsize )
{
	pro_trans_start *p_start = &(p_str->start);
	p_start->header.type = PRO_START;
	p_start->algorithms = alogri;
	p_start->pubKey = pubkey;
	p_start->authMeth = authmeth;
	p_start->authAct = authact;
	p_start->authSize = authsize;
	return 1;
}

u8 set_pro_pub_key(mesh_pro_data_t *p_str,u8 *p_pubkeyx,u8 *p_pubkeyy){
	pro_trans_pubkey *p_pubkey = &(p_str->pubkey);
	p_pubkey->header.type = PRO_PUB_KEY;
	memcpy(p_pubkey->pubKeyX,p_pubkeyx,32);
	memcpy(p_pubkey->pubKeyY,p_pubkeyy,32);
	return 1;
}

u8 set_pro_input_complete(mesh_pro_data_t *p_str)
{
	pro_trans_incomplete *p_com = &(p_str->inComp);
	p_com->header.type = PRO_INPUT_COM;
	return 1;
}
u8 set_pro_confirm(mesh_pro_data_t *p_str,u8 *p_confirm,u8 len)
{
	pro_trans_confirm *p_confirm_t = &(p_str->confirm);	
	p_confirm_t->header.type = PRO_CONFIRM;
	memcpy(p_confirm_t->confirm,p_confirm,len);
	return 1;
}

u8 set_pro_random(mesh_pro_data_t *p_str,u8 *p_random,u8 len)
{
	pro_trans_random *p_random_t = &(p_str->random);
	p_random_t->header.type = PRO_RANDOM;
	memcpy(p_random_t->random,p_random,len);
	return 1;
}

u8 set_pro_data(mesh_pro_data_t *p_str, u8 *p_data,u8 *p_mic)
{
	pro_trans_data *p_data_t = &(p_str->data);	
	p_data_t->header.type = PRO_DATA;
	memcpy(p_data_t->encProData,p_data,25);
	memcpy(p_data_t->proDataMic,p_mic,8);
	return 1;
}

u8 set_pro_complete(mesh_pro_data_t *p_str)
{
	pro_trans_complete *p_complete = &(p_str->complete);
	p_complete->header.type =PRO_COMPLETE;
	return 1;
}

u8 set_pro_fail(mesh_pro_data_t *p_str ,u8 fail_code)
{
	pro_trans_fail *p_fail = &(p_str->fail);	
	p_fail->header.type =PRO_FAIL;
	p_fail->err_code = fail_code;
	return 1;
}

u8 is_prov_oob_hmac_sha256()
{
	if(prov_oob.start.algorithms == BTM_ECDH_P256_HMAC_SHA256_AES_CCM){
		return 1;
	}else{
		return 0;
	}
}

u8 get_prov_confirm_len()
{
	if(is_prov_oob_hmac_sha256()){
		return sizeof(pro_trans_confirm);
	}else{
		return sizeof(pro_trans_confirm)-16;
	}
}

u8 get_prov_confirm_value_len()
{
	return (get_prov_confirm_len()-1);
}

u8 get_prov_random_len()
{
	if(is_prov_oob_hmac_sha256()){
		return sizeof(pro_trans_random);
	}else{
		return sizeof(pro_trans_random)-16;
	}
}

u8 get_prov_random_value_len()
{
	return (get_prov_random_len()-1);
}

#if CERTIFY_BASE_ENABLE
u8 set_pro_record_request(pro_trans_record_request *p_rec_req ,u16 rec_id,u16 frag_offset,u16 max_size)
{
	p_rec_req->header.type = PRO_REC_REQ;
	p_rec_req->rec_id = swap_u16_data(rec_id);	
	p_rec_req->frag_off = swap_u16_data(frag_offset);
	p_rec_req->frag_max_size = swap_u16_data(max_size);
	return 1;
}

u8 set_pro_record_rsp(pro_trans_record_rsp *p_rec_rsp,u8 sts,u16 rec_id,u16 frag_offset,u16 total,u8 *p_data,u16 data_len)
{
	p_rec_rsp->header.type = PRO_REC_RSP;
	p_rec_rsp->sts = sts;
	p_rec_rsp->rec_id = swap_u16_data(rec_id);
	p_rec_rsp->frag_off = swap_u16_data(frag_offset);
	p_rec_rsp->total_len = swap_u16_data(total);
	memcpy(p_rec_rsp->data , p_data, data_len);
	return 1;
}

u8 set_pro_record_get(pro_trans_record_get *p_rec_get)
{
	p_rec_get->header.type = PRO_REC_GET;
	return 1;
}

u8 set_pro_record_list(pro_trans_record_list *p_rec_list , u16 prov_extension, u16 *p_list,u32 cnt)
{
	p_rec_list->header.type = PRO_REC_LIST;
	p_rec_list->prov_extension = swap_u16_data(prov_extension);
	for(u32 i=0;i<cnt;i++){
		p_rec_list->rec_id[i] =  swap_u16_data(p_list[i]);
	}
	return 1;
}
#endif
u8 mesh_send_provison_data(u8 pdu_type,u8 bearCode,u8 *para,u8 para_len )
{
	#if FEATURE_PROV_EN
	if((para_len > ADV_PDU_LEN_MAX) || (pdu_type > BEARS_CTL)){
		return 0;
	}
	// for the bearerctl
	u8 bearerCode=0;
	u8 bearerLen=0;
	u8 trans_ack_flag =0;
	u8 link_close_flag =0;
	pro_PB_ADV *p_pro_pb_adv;
	
	#if TESTCASE_FLAG_ENABLE
	p_pro_pb_adv = (pro_PB_ADV *)(&(pro_adv_pkt.len));
	#else
    u8 buff_bear[MESH_BEAR_SIZE];
    memset(buff_bear, 0, sizeof(buff_bear));
	mesh_cmd_bear_t *cmd_bear_tmp = (mesh_cmd_bear_t *)buff_bear;   
	p_pro_pb_adv = (pro_PB_ADV *)(&(cmd_bear_tmp->len));
	#endif
	p_pro_pb_adv->ad_type = MESH_ADV_TYPE_PRO;
	p_pro_pb_adv->transAck.GPCF = pdu_type;
	switch(pdu_type){
		case TRANS_START:
		    {
			// initial the segment idx , the first cmd of the provisioner and device will not increase
				prov_para.segmentIdx=0;
				p_pro_pb_adv->length = 4+6+para_len;
				memcpy(p_pro_pb_adv->link_ID,(u8*)(&(prov_para.link_id)),4);
				p_pro_pb_adv->transStart.SegN = prov_para.last_segmentIdx;
				p_pro_pb_adv->transStart.total_len =prov_para.trans_seg_total_len;
				endianness_swap_u16((u8*)(&(p_pro_pb_adv->transStart.total_len)));
				p_pro_pb_adv->transStart.FCS =prov_para.trans_start_fcs;
				memcpy(p_pro_pb_adv->transStart.data,para,para_len);
				u8 prov_cmd = (p_pro_pb_adv->transStart.data[0])&0x3f;
				if(!prov_para.cmd_retry_flag &&prov_para.cert_base_en&&(!prov_para.direct_invite)){
					prov_para.trans_num++;
				}else if(!prov_para.cmd_retry_flag && prov_cmd!= PRO_CAPABLI && prov_cmd!= PRO_INVITE ){
					if(prov_para.err_op_code){
						prov_para.err_op_code = 0;//for the provision err code proc .
					}else{
						prov_para.trans_num++;
					}
					
					// after the sending ,it will change the transnum part 
			    }
		    }
			break;
		case TRANS_ACK:
			trans_ack_flag =1;
			p_pro_pb_adv->length = 1+6;// no para 
			memcpy(p_pro_pb_adv->link_ID,(u8*)(&(prov_para.link_id)),4);
			p_pro_pb_adv->transAck.rfu =0;
			break;
		case TRANS_CON:
			prov_para.segmentIdx++;
			p_pro_pb_adv->length = 1+6+para_len;// no para 
			memcpy(p_pro_pb_adv->link_ID,(u8*)(&(prov_para.link_id)),4);
			p_pro_pb_adv->transCon.SegIdx = prov_para.segmentIdx;
			memcpy(p_pro_pb_adv->transCon.data,para,para_len);
			break;
		case BEARS_CTL:
			bearerCode = bearCode;
			memcpy(p_pro_pb_adv->link_ID,(u8*)(&(prov_para.link_id)),4);
		//	bearerCode = p_pro_pb_adv->transBear.bearOpen.header.BearCtl;
			p_pro_pb_adv->transBear.bearOpen.header.BearCtl=bearerCode;
			if(bearerCode == LINK_OPEN){
				//transaction num should changes when send link open 
				if(!prov_para.cmd_retry_flag){
					prov_para.trans_num++; 
                    prov_para.link_id++;
				}
				prov_para.trans_num =0;
				bearerLen =17;
				memcpy(p_pro_pb_adv->link_ID,(u8 *)(&prov_para.link_id),4);
				memcpy(p_pro_pb_adv->transBear.bearOpen.DeviceUuid,para,16);               
			}else if(bearerCode == LINK_ACK){
				bearerLen = 1;
				trans_ack_flag =1;
			}else if(bearerCode == LINK_CLOSE){
				link_close_flag =1;
				// transaction num should changes when send link close
				p_pro_pb_adv->transBear.bearCls.reason = prov_link_cls_code;
				bearerLen = 2;
			}else{}
			//calculate the total length of the data 
			p_pro_pb_adv->length = 6+bearerLen;
			break;
		default:
			return 0;
			break;
	}
	if(prov_para.initial_pro_roles == MESH_INI_ROLE_PROVISIONER){
		prov_para.trans_num &= 0x7f;
		p_pro_pb_adv->trans_num = prov_para.trans_num;
	}else if(prov_para.initial_pro_roles == MESH_INI_ROLE_NODE){
		prov_para.trans_num |=  0x80;
		p_pro_pb_adv->trans_num = prov_para.trans_num;		
	}else{}
	// add the transaction num 
	if(trans_ack_flag){
		p_pro_pb_adv->trans_num = prov_para.rsp_ack_transnum;
	}
	if(link_close_flag){
		prov_para.trans_num =0;
	}
	// if the data can be successful dispatch ,add the data to the fifo 
	#if(TESTCASE_FLAG_ENABLE)
	pro_adv_pkt.trans_par_val = 0x21;
	mesh_tx_cmd_add_packet((u8 *)(&pro_adv_pkt));
	#else
	cmd_bear_tmp->trans_par_val = 0x02;
	mesh_tx_cmd_add_packet((u8 *)(cmd_bear_tmp));
	#endif
//	LAYER_DATA_DEBUG(A_buf4_pro, (u8 *)(&pro_adv_pkt), sizeof(mesh_cmd_bear_t));

	#endif 
	return 1;
}

void mesh_send_no_extend_data(u8 *para , u8 para_len)
{
	u8 p_idx=0;
	u8 start_pkt_flag =1;
	if(para_len<=20){
		prov_para.last_segmentIdx=0;
		mesh_send_provison_data(TRANS_START,0,para,para_len);
	}else{
		while(para_len){
			u8 data_len;
			if(start_pkt_flag){
				start_pkt_flag =0;
				para_len -=20;
				prov_para.last_segmentIdx=1+para_len/23;
				mesh_send_provison_data(TRANS_START,0,para,20);
				p_idx += 20; 
				continue;
			}
			data_len = (para_len>=23)?23:para_len;
			para_len -=data_len;
			mesh_send_provison_data(TRANS_CON,0,para+p_idx,data_len);
			p_idx += data_len;
		}
	}
}

u8 mesh_send_multi_data(u8 *para , u8 para_len)
{
    // #define PARA_LEN_MAX       (ADV_PDU_LEN_MAX-(31-23))	// confirm later
	prov_para.trans_seg_total_len = para_len;
	prov_para.last_segmentIdx=0;
	prov_para.trans_start_fcs = crc8_rohc(para,(u16)para_len);
#if EXTENDED_ADV_PROV_ENABLE
	if(is_not_use_extend_adv(EXTEND_PROVISION_FLAG_OP)){
		mesh_send_no_extend_data(para,para_len);
	}else{
		prov_para.last_segmentIdx=0;
		mesh_send_provison_data(TRANS_START,0,para,para_len);// in the extend mode ,can send directly 
	}
#else
	mesh_send_no_extend_data(para,para_len);
#endif
	return 1;
}

int mesh_provision_rcv_process (u8 *p_payload, u32 t)
{
	// provison 's flow control
	#if FEATURE_PROV_EN
	mesh_provison_process(prov_para.initial_pro_roles ,(p_payload));
	#endif 
	return 1;
}

u8 get_mesh_pro_str_len(u8 type)
{
	u8 len=0;
	mesh_pro_pdu_content pro_pdu_content;
	mesh_pro_pdu_content *p_content = &pro_pdu_content;
	switch(type){
		case PRO_INVITE:
			len = sizeof(p_content->invite);
			break;
		case PRO_CAPABLI:
			len = sizeof(p_content->capa);
			break;
		case PRO_START:
			len = sizeof(p_content->start);
			break;
		case PRO_PUB_KEY:
			len = sizeof(p_content->pubkey);
			break;
		case PRO_INPUT_COM:
			len = sizeof(p_content->inComp);
			break;
		case PRO_CONFIRM:
			len = get_prov_confirm_len();
			break;
		case PRO_RANDOM:
			len = get_prov_random_len();
			break;
		case PRO_DATA:
			len = sizeof(p_content->data);
			break;
		case PRO_COMPLETE:
			len = sizeof(p_content->complete);
			break;
		case PRO_FAIL:
			len = sizeof(p_content->fail);
			break;
		case PRO_REC_REQ:
			len = sizeof(p_content->rec_req);
			break;
		case PRO_REC_RSP:
			len = prov_para.rec_rsp_len;
			break;
		case PRO_REC_GET:
			len = sizeof(p_content->rec_get);
			break;
		case PRO_REC_LIST:
			len = prov_para.rec_list_len;
			break;
		default:
			break;
	}
	return len ;
}

u8 send_multi_type_data(u8 type,u8 *p_para)
{
	u8 len;
	len = get_mesh_pro_str_len(type);
	p_para[0]=type;
	#if FEATURE_PROV_EN
	mesh_send_multi_data(p_para,len);
	#endif 
	return len;
}

#if (__PROJECT_MESH_PRO__ || __PROJECT_MESH_GW_NODE__)
_attribute_no_retention_bss_ u8  pro_auth[32];
_attribute_no_retention_bss_ u8 pro_dat[40];
_attribute_no_retention_bss_ u8 prov_net_key[16];
const u8 const_prov_ivi[4]={0x11,0x22,0x33,0x44};

u8 set_provision_networkkey_self(u8 *p_key,u8 len )
{
	if(len >16){
		return 0;
	}
	provison_net_info_str *p_str = (provison_net_info_str *)pro_dat;
	memcpy(prov_net_key,p_key,len);
	memcpy(p_str->net_work_key,prov_net_key,16);
	set_pro_dat_part(PROVISION_ELE_ADR);
	return 1;
}

void set_provisionee_para(u8 *p_net_key,u16 key_index,u8 flags,u8 *p_ivi,u16 unicast)
{
	provison_net_info_str *p_str = (provison_net_info_str *)pro_dat;
	if(p_net_key){
		memcpy(p_str->net_work_key,p_net_key,sizeof(p_str->net_work_key));
	}else{
		memcpy(p_str->net_work_key,prov_net_key,sizeof(prov_net_key));
	}
	p_str->key_index = key_index;
	p_str->flags = flags;
	if(p_ivi){
		memcpy(p_str->iv_index,p_ivi,sizeof(p_str->iv_index));
	}else{
		memcpy(p_str->iv_index,const_prov_ivi,sizeof(p_str->iv_index));
	}
	p_str->unicast_address = unicast;
	return ;
}
void set_provisioner_para(u8 *p_net_key,u16 key_index,u8 flags,u8 *p_ivi,u16 unicast)
{
	provison_net_info_str pro_dat_tmp;
	set_provisionee_para(p_net_key,key_index,flags,p_ivi,unicast);
	memcpy((u8 *)&pro_dat_tmp,pro_dat,sizeof(pro_dat_tmp));
	mesh_provision_par_set_dir(&pro_dat_tmp);
}
u8 set_pro_dat_part(u16 ele_adr)
{
	set_provisioner_para(0,0,0,0,ele_adr);
	return 0;
}

void set_pro_unicast_adr(u16 unicast)
{
	provison_net_info_str *p_str = (provison_net_info_str *)pro_dat;
	p_str->unicast_address = unicast;
}

#if WIN32
void pro_random_init()
{

	for(int i=0;i<sizeof(gatt_pro_random);i++){
		gatt_pro_random[i]= win32_create_rand()&0xff;
	}
}
#else
void pro_random_init()
{
	generateRandomNum(sizeof(pro_random), pro_random);
}
#endif
#endif

_attribute_bss_retention_ u8 dev_auth[32];
#if(!__PROJECT_MESH_PRO__)
STATIC_ASSERT(sizeof(mesh_cmd_ut_tx_seg_union.prov_ing.u_confirm_input) % 4 == 0);    // 4byte align
STATIC_ASSERT(sizeof(mesh_cmd_ut_tx_seg_union.prov_ing.pb) % 4 == 0);    // 4byte align
STATIC_ASSERT(sizeof(mesh_cmd_ut_tx_seg_union.ut_tx_seg) >= sizeof(mesh_cmd_ut_tx_seg_union.prov_ing));

STATIC_ASSERT(sizeof(mesh_cmd_ut_rx_seg_union.ut_rx_seg) >= sizeof(mesh_cmd_ut_rx_seg_union.prov_ing));

void dev_random_init()
{
	generateRandomNum(sizeof(dev_random), dev_random);
}
#endif 
void provision_random_data_init()
{
	// use the initial data part 
	#if (__PROJECT_MESH_PRO__ || __PROJECT_MESH_GW_NODE__)
	pro_random_init();
	#endif
	#if(!__PROJECT_MESH_PRO__)
	dev_random_init();
	#endif
}

void send_rcv_retry_set(u8 cmd, u8 bearerCode, u8 ack_retry_flag)
{
	prov_para.pro_cmd = cmd ;
    prov_para.pro_bearerCode = bearerCode;
	prov_para.cmd_send_tick = clock_time();
	prov_para.cmd_send_start_tick = clock_time();
	prov_para.cmd_retry_flag =1;
	prov_para.ack_retry_flag = ack_retry_flag;
}
void prov_set_link_close_code(u8 code)
{
	prov_link_cls_code = code;
}
void send_rcv_retry_clr()
{
	prov_para.cmd_send_tick = clock_time();
	prov_para.cmd_retry_flag =0;
	prov_para.ack_retry_flag =0;
	prov_link_cls_code = LINK_CLOSE_SUCC;
	#if MD_REMOTE_PROV
	if(!is_provision_success()){// in the remote-prov procedure.
		my_fifo_reset(&mesh_adv_cmd_fifo);
	}
	#else
	my_fifo_reset(&mesh_adv_cmd_fifo);
	#endif
}

// send multi packets 
// GATT part dispatch 
int notify_pkts(u16 conn_handle, u8 *p, u16 len, u16 att_handle, u8 proxy_type)
{
	int err = -1;
#if !WIN32 
	u8 tmp[256];
	u8 pkt_no=0;
	u8 buf_idx =0;
	u8 total_len;
	u8 pkt_num =0;
	#if (__TLSR_RISCV_EN__ && BLE_MULTIPLE_CONNECTION_ENABLE)
	u8 valid_len = blt_gap_getEffectiveMTU(conn_handle) - 4; // opcode 1 + handle_id 2 + sar 1
	#else
	u8 valid_len = bltAtt.effective_MTU- 4; // opcode 1 + handle_id 2 + sar 1
	#endif
	
	if(len){
		pkt_num = 1+(len-1)/valid_len;
	}
	total_len =len;
	#if !WIN32 
	// reserve more fifo for the tx fifo part 
	#if (__TLSR_RISCV_EN__ && BLE_MULTIPLE_CONNECTION_ENABLE)
	if(blc_ll_getTxFifoNumber(conn_handle)+ pkt_num >= (blt_llms_get_tx_fifo_max_num(conn_handle) - 2))
	#else
	if(blc_ll_getTxFifoNumber()+ pkt_num >= (blt_txfifo.num - 2))
	#endif
	{
		return err;
	}
	#endif 
	// dispatch the notification flag
	extern u8  provision_Out_ccc[2];
	extern u8  proxy_Out_ccc[2];
	if(att_handle == PROVISION_ATT_HANDLE && 
		provision_Out_ccc[0]==0 && provision_Out_ccc[1]==0 ){
		#if (!DEBUG_MESH_DONGLE_IN_VC_EN)
		return err;
		#endif
	}		
	else if(att_handle == GATT_PROXY_HANDLE && 
		proxy_Out_ccc[0]==0 && proxy_Out_ccc[1]==0 ){
		#if (!DEBUG_MESH_DONGLE_IN_VC_EN)
		return err;
		#endif
	}
	pb_gatt_proxy_str *p_notify = (pb_gatt_proxy_str *)(tmp);
	//can send in one packet
	if(len==0){
		return err;
	}
	if(len>valid_len){
		while(len){			
			if(!pkt_no){
				//send the first pkt
				p_notify->sar = SAR_START;
				p_notify->msgType = proxy_type;					
				memcpy(p_notify->data,p,valid_len);
				err = blc_gatt_pushHandleValueNotify(conn_handle, att_handle, tmp, valid_len+1);
				len = len-valid_len;
				buf_idx +=valid_len;
				pkt_no++;
			}else{
				// the last pkt 
				if(buf_idx+valid_len>=total_len){
					p_notify->sar = SAR_END;
					p_notify->msgType = proxy_type;
					memcpy(p_notify->data,p+buf_idx,total_len-buf_idx);
					err = blc_gatt_pushHandleValueNotify(conn_handle, att_handle, tmp, total_len-buf_idx+1);
					len =0;
				}else{
				// send the continue pkt 
					p_notify->sar = SAR_CONTINUE;
					p_notify->msgType = proxy_type;
					memcpy(p_notify->data,p+buf_idx,valid_len); //
					err = blc_gatt_pushHandleValueNotify(conn_handle, att_handle, tmp, valid_len+1);
					len = len-valid_len;
					buf_idx +=valid_len;
				}
			}

			if(err){
				break;
			}
		}

	}else{
	// send the complete pkt 
		p_notify->sar = SAR_COMPLETE;
		p_notify->msgType = proxy_type;
		memcpy(p_notify->data,p,len);
		err = blc_gatt_pushHandleValueNotify(conn_handle, att_handle, tmp, len+1);
		
	}
	#endif
	return err;

}

int gatt_prov_notify_pkts(u16 conn_handle, u8 *p, u16 len, u16 att_handle, u8 proxy_type)
{
    int err = notify_pkts(conn_handle, p, len, att_handle, proxy_type);
    if(err){
        LOG_MSG_ERR(TL_LOG_NODE_SDK,0, 0,"notify_pkts error! ");
    }
    return 0;
}


u8 prov_auth_en_flag =0;
u8 prov_auth_val = 0;
u32 prov_oob_output_auth = 0;

void mesh_set_dev_auth(u8 *p_auth, u8 len)
{
	memcpy(dev_auth, p_auth, len);
	return;
}

#if (__PROJECT_MESH_PRO__ || __PROJECT_MESH_GW_NODE__)
int get_auth_value_by_uuid(u8 *uuid_in,u8 *oob_out);

void mesh_set_pro_auth(u8 *p_auth, u8 len)
{
	memcpy(pro_auth, p_auth, len);
	return;
}

int mesh_prov_oob_auth_data(mesh_prov_oob_str *p_prov_oob)
{
	
	pro_trans_start *p_start = &(p_prov_oob->start);
	int err = -1;
	if(p_start->authMeth == MESH_NO_OOB) {
		memset(pro_auth ,0,sizeof(pro_auth));
		#if WIN32
		memset(gatt_pro_auth, 0x00, sizeof(gatt_pro_auth));
		#endif
		err = 0;
	}else if (p_start->authMeth == MESH_STATIC_OOB){
	#if WIN32
        u8 prov_oob_static[32] = {0}; // must 32 byte for sha256 output.
		gatt_provision_mag.oob_len = get_auth_value_by_uuid(gatt_provision_mag.device_uuid, prov_oob_static);
		if(gatt_provision_mag.oob_len){
			err = 0;
			memcpy(gatt_pro_auth, prov_oob_static, min(gatt_provision_mag.oob_len, sizeof(gatt_pro_auth)));
		}
	#else
		#if PTS_TEST_EN
			#if CERTIFY_BASE_ENABLE
		const u8 pro_auth_const[16]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
									 0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
		memset(pro_auth ,0,sizeof(pro_auth));
		memcpy(pro_auth,pro_auth_const,sizeof(pro_auth_const));
			#else
		memset(pro_auth ,0,sizeof(pro_auth));
		pro_auth[0]=0x02;
			#endif
		#else
		// TODO: if not found static oob then try no oob mode. because has got response(HCI_GATEWAY_CMD_STATIC_OOB_RSP) from VC database after clicking button of "provision start".
		#endif
		err = 0;
	#endif 
	}else if (p_start->authMeth == MESH_OUTPUT_OOB){
		// need to input the para part 
		if(p_prov_oob->oob_outAct == MESH_OUT_ACT_BLINK){

		}else if (p_prov_oob->oob_outAct == MESH_OUT_ACT_BEEP){
			u8 tmp_auth[4];
			memcpy(tmp_auth,(u8 *)(&prov_oob_output_auth),4);
			endianness_swap_u32(tmp_auth);
			if(is_prov_oob_hmac_sha256()){
				memcpy(pro_auth+28,tmp_auth,4);
			}else{
				memcpy(pro_auth+12,tmp_auth,4);
			}
			err = 0;
		}else if (p_prov_oob->oob_outAct == MESH_OUT_ACT_NUMERIC){
			memset(pro_auth,0,sizeof(pro_auth));
			if(is_prov_oob_hmac_sha256()){
				pro_auth[31]= prov_auth_val;
			}else{
				pro_auth[15]= prov_auth_val;
			}
			
			err = 0;
		}else if (p_prov_oob->oob_outAct == MESH_OUT_ACT_ALPHA){
				
		}
	}else if (p_start->authMeth == MESH_INPUT_OOB){
		memset(pro_auth,0,sizeof(pro_auth));
		if(prov_oob.start.pubKey == MESH_PUB_KEY_WITH_OOB){
			pro_auth[0]=0x32;
		}else{
			if(is_prov_oob_hmac_sha256()){
				pro_auth[31]=2;
			}else{
				pro_auth[15]=2;
			}
		}
		
		err = 0;
	}
	return err ;
}

#define FORCE_START_WITH_NO_OOB		0

/********************the rules about the EPA and normal mode *********
1. if the provisioner don't support EPA,it will force to use normal mode .
2. if the node not support ,use normal mode .
3. if both provisioner and node support EPA , then provisioner will check oob in database :
	3.1. if the oob len is 16 ,it will force to use normal mode. 
	3.2. if the oob len is 32 ,it will force to use epa mode .
***********************************************************************/
u8 get_static_oob_len()
{
	u8 len =0;
	#if WIN32
	u8 oob[32] = {0};
	gatt_provision_mag.oob_len = get_auth_value_by_uuid(gatt_provision_mag.device_uuid, oob);
	len = gatt_provision_mag.oob_len;
	#elif GATEWAY_ENABLE
	len = prov_para.prov_oob_len;
	#endif
	return len ;
}

void set_algorithms_normal(pro_trans_start *p_start,pro_trans_capa *p_capa)
{
	if(p_capa->algorithms & BIT(BTM_ECDH_P256_HMAC_SHA256_AES_CCM)){
		p_start->algorithms = BTM_ECDH_P256_HMAC_SHA256_AES_CCM;
	}else{
		p_start->algorithms = BTM_ECDH_P256_CMAC_AES128_AES_CCM;
	}
}

u8 set_start_para_by_capa(mesh_prov_oob_str *p_prov_oob)
{
	pro_trans_start *p_start = &(p_prov_oob->start);
#if PROV_EPA_EN
	pro_trans_capa *p_capa = &(p_prov_oob->capa);
	#if TESTCASE_FLAG_ENABLE
	set_algorithms_normal(p_start,p_capa);
	#else 
	if(p_capa->staticOOBType){
		int oob_len = get_static_oob_len();
		// if the node not support epa ,should use normal directly
		if((p_capa->algorithms & BIT(BTM_ECDH_P256_HMAC_SHA256_AES_CCM)) && (oob_len == 32)){
			p_start->algorithms = BTM_ECDH_P256_HMAC_SHA256_AES_CCM;
		}else{
			p_start->algorithms = BTM_ECDH_P256_CMAC_AES128_AES_CCM;
		}
		
		LOG_MSG_LIB(TL_LOG_NODE_BASIC, 0, 0 ,"start algorithm: %d, capability algorithm: 0x%x, oob len: %d", p_start->algorithms, p_capa->algorithms, oob_len);
	}else{
		set_algorithms_normal(p_start,p_capa);
	}
	#endif
#else
	p_start->algorithms = BTM_ECDH_P256_CMAC_AES128_AES_CCM;
#endif

	p_start->pubKey =  p_prov_oob->capa.pubKeyType;
	p_start->authMeth = p_prov_oob->prov_key;
	if(p_start->authMeth == MESH_NO_OOB || p_start->authMeth == MESH_STATIC_OOB){
		p_start->authAct =0;
		p_start->authSize = 0;
		#if FORCE_START_WITH_NO_OOB
		p_start->authMeth = MESH_NO_OOB;
		#endif
	}else if (p_start->authMeth == MESH_OUTPUT_OOB ){
		p_start->authAct = p_prov_oob->oob_outAct ;
		p_start->authSize = p_prov_oob->oob_outsize ;
	}else if (p_start->authMeth == MESH_INPUT_OOB ){
		p_start->authAct = p_prov_oob->oob_inAct ;
		p_start->authSize = p_prov_oob->oob_insize ;
	}
	
	return 1;
}

void send_confirm_no_pubkey_cmd()
{
	mesh_pro_data_t *p_send_str = (mesh_pro_data_t *)(para_pro);
	u8 prov_private_key[32];
	get_private_key(prov_private_key);
	tn_p256_dhkey_fast(ecdh_secret, prov_private_key,confirm_input+0x11+0x40 ,confirm_input+0x11+0x60);
	mesh_sec_prov_confirmation_sec (pro_confirm, confirm_input, 
								145, ecdh_secret, pro_random, pro_auth);												
	set_pro_confirm(p_send_str,pro_confirm,get_prov_confirm_value_len());
	send_multi_type_data(PRO_CONFIRM,para_pro);
	send_rcv_retry_set(PRO_CONFIRM,0,0);
	prov_para.trans_num_last = prov_para.trans_num;
	prov_para.provison_send_state = STATE_DEV_CONFIRM;	
}

void send_confirm_no_pubkey_cmd_with_ack()
{
	mesh_pro_data_t *p_send_str = (mesh_pro_data_t *)(para_pro);
	u8 prov_private_key[32];
	get_private_key(prov_private_key);
	tn_p256_dhkey_fast(ecdh_secret, prov_private_key,confirm_input+0x11+0x40 ,confirm_input+0x11+0x60);
	mesh_sec_prov_confirmation_sec (pro_confirm, confirm_input, 
								145, ecdh_secret, pro_random, pro_auth);
	mesh_adv_prov_confirm_cmd(p_send_str,pro_confirm);
	prov_para.provison_send_state = STATE_DEV_CONFIRM;
	
	return ;

}
#endif

#if(!__PROJECT_MESH_PRO__) // the node part dispatch the oob part 
void mesh_set_oob_type(u8 type, u8 *p_oob ,u8 len )
{
	pro_trans_capa * p_capa  = 	&(prov_oob.capa);
	
	if(type == MESH_NO_OOB){
		p_capa->staticOOBType = MESH_NO_OOB;
	}else if (type == MESH_STATIC_OOB){
		p_capa->staticOOBType = MESH_STATIC_OOB;
		if(p_oob!=0){
			mesh_set_dev_auth(p_oob,len );
		}
	}
	p_capa->pubKeyType = 0;
	p_capa->ele_num =g_ele_cnt;
	#if PROV_EPA_EN
	p_capa->algorithms = BIT(BTM_ECDH_P256_CMAC_AES128_AES_CCM)|BIT(BTM_ECDH_P256_HMAC_SHA256_AES_CCM);
	#else
	p_capa->algorithms = BIT(BTM_ECDH_P256_CMAC_AES128_AES_CCM);
	#endif
	p_capa->outPutOOBSize = 0;
	p_capa->outPutOOBAct = 0;
	p_capa->inOOBSize = 0;
	p_capa->inOOBAct = 0;
}

u8 mesh_node_oob_auth_data(mesh_prov_oob_str *p_prov_oob)
{
	
	pro_trans_start *p_start = &(p_prov_oob->start);
	u8 ret =0;
	if(p_start->authMeth == MESH_NO_OOB) {
		memset(dev_auth ,0,sizeof(dev_auth));
		ret = 1;
	}else if (p_start->authMeth == MESH_STATIC_OOB){
        #if DEBUG_EVB_EN
        uint8_t auth_value[16] = {0x0a,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
        mesh_set_dev_auth(auth_value,sizeof(auth_value));
		#elif PTS_TEST_EN
			#if CERTIFY_BASE_ENABLE
			#else
			memset(dev_auth ,0,sizeof(dev_auth));
			dev_auth[0]=0x02;
			#endif
		#endif
		ret =1;
	}else if (p_start->authMeth == MESH_OUTPUT_OOB){
		// need to input the para part 
		if(p_prov_oob->oob_outAct == MESH_OUT_ACT_BLINK){
			// we suppose the key code is 0
			#if (PTS_TEST_EN || 1) // open mainly for the bluez test 
        	memset(dev_auth ,0,sizeof(dev_auth));
			if(is_prov_oob_hmac_sha256()){
				dev_auth[31]= 2;
			}else{
				dev_auth[15]= 2;
			}
			#endif
		}else if (p_prov_oob->oob_outAct == MESH_OUT_ACT_BEEP){
			
		}else if (p_prov_oob->oob_outAct == MESH_OUT_ACT_NUMERIC){
			
		}else if (p_prov_oob->oob_outAct == MESH_OUT_ACT_ALPHA){		
		}
		ret=1;
	}else if (p_start->authMeth == MESH_INPUT_OOB){
		memset(dev_auth ,0,sizeof(dev_auth));
		if(is_prov_oob_hmac_sha256()){
			dev_auth[31]= prov_auth_val;
		}else{
			dev_auth[15]= prov_auth_val;
		}
		if(prov_auth_en_flag ){
			prov_auth_en_flag =0;
		}
		ret =1;
	}
	return ret ;
}

#endif 

u8 change_capa_act_to_start_act(u16 capa_act,u8 out_flag)
{
	u8 act=0;
	if(out_flag){
		if(capa_act&BIT(MESH_OUT_ACT_BLINK)){
			act = MESH_OUT_ACT_BLINK;
		}else if (capa_act&BIT(MESH_OUT_ACT_BEEP)){
			act = MESH_OUT_ACT_BEEP;
		}else if (capa_act & BIT(MESH_OUT_ACT_VIBRATE)){
			act = MESH_OUT_ACT_VIBRATE;
		}else if (capa_act & BIT(MESH_OUT_ACT_NUMERIC)){
			act = MESH_OUT_ACT_NUMERIC;
		}else if (capa_act & BIT(MESH_OUT_ACT_ALPHA)){
			act = MESH_OUT_ACT_ALPHA;
		}else{
			act = MESH_OUT_ACT_RFU;
		}
	}else{
		if(capa_act&BIT(MESH_IN_ACT_PUSH)){
			act = MESH_IN_ACT_PUSH;
		}else if (capa_act &BIT(MESH_IN_ACT_TWIST )){
			act = MESH_IN_ACT_TWIST;
		}else if (capa_act &BIT(MESH_IN_ACT_NUMBER)){
			act = MESH_IN_ACT_NUMBER;
		}else if (capa_act &BIT(MESH_IN_ACT_ALPHA)){
			act = MESH_IN_ACT_ALPHA;
		}else {
			act = MESH_IN_ACT_RFU;
		}
	}
	return act ;
}


u8 get_pubkey_oob_info_by_capa(mesh_prov_oob_str *p_prov_oob)
{
	//u8 pubkey =0;
	u8 oob_type =0;
	pro_trans_capa * p_capa = &(p_prov_oob->capa);
	//pubkey = p_capa->pubKeyType;
	oob_type = p_capa->staticOOBType;
	if(oob_type & BIT(STATIC_OOB_TYPE_VAL)){
		p_prov_oob->prov_key = MESH_STATIC_OOB;
		return 1;
	}else if(p_capa->inOOBSize ||p_capa->outPutOOBSize ){
		if(p_capa->outPutOOBSize>=1 && p_capa->outPutOOBSize <=8){
			p_prov_oob->oob_outsize = p_capa->outPutOOBSize;
			p_prov_oob->prov_key = MESH_OUTPUT_OOB;
			p_prov_oob->oob_outAct =change_capa_act_to_start_act(p_capa->outPutOOBAct,1);
		}else if (p_capa->inOOBSize>=1 && p_capa->inOOBSize <=8){
			p_prov_oob->oob_insize = p_capa->inOOBSize;
			p_prov_oob->prov_key = MESH_INPUT_OOB;
			p_prov_oob->oob_inAct =change_capa_act_to_start_act(p_capa->inOOBAct,0);
		}else if (p_capa->inOOBSize==0 && p_capa->outPutOOBSize==0){
			p_prov_oob->prov_key = MESH_NO_OOB;
		}
	}else{
		p_prov_oob->prov_key = MESH_NO_OOB;
	}
	return 1;
}

u8 prov_cmd_is_valid(u8 op_code)
{
	if(op_code > PRO_REC_LIST){
		return 0;
	}else{
		return 1;
	}
}
u8 prov_fail_cmd_proc(mesh_pro_data_t *p_notify,u8 err_code)
{
	set_pro_fail(p_notify,err_code);
	mesh_provision_para_reset();
	mesh_node_prov_event_callback(EVENT_MESH_NODE_RC_LINK_FAIL_CODE);
	return sizeof(pro_trans_fail);
}

u8 prov_calc_cmd_len_chk(u8 op_code,u8 len)
{
	u8 prov_cmd_len =0;
	prov_cmd_len = get_mesh_pro_str_len(op_code);
	if(prov_cmd_len == len){
		return 1;
	}else{
		return 0;
	}
}
#if !__PROJECT_MESH_PRO__ 

u16 provisionee_gatt_rcv_invite_proc(mesh_pro_data_t *p_notify,mesh_pro_data_t *p_rcv_str)
{

	u16 notify_len=0;
	LOG_MSG_LIB(TL_LOG_NODE_SDK,(u8 *)&(p_rcv_str->invite), 
								sizeof(pro_trans_invite),"rcv provision invite ");
    mesh_provision_para_reset();
	if(is_provision_success()){
		// can not provision again ,and stop by the cmd 
		LOG_MSG_ERR(TL_LOG_PROVISION,0, 0 ,"gatt the dev has already provisioned");
		return 0;
	}
	confirm_input[0]= p_rcv_str->invite.attentionDura;
	set_pro_capa_cpy(p_notify,&prov_oob);// set capability part 
	swap_mesh_pro_capa(p_notify);
	memcpy(confirm_input+1,&(p_notify->capa.ele_num),11);
	notify_len = sizeof(pro_trans_capa);
	prov_para.provison_rcv_state =STATE_DEV_CAPA;
	
	blc_att_setServerDataPendingTime_upon_ClientCmd(1);
	LAYER_PARA_DEBUG(A_provision_invite);
	LOG_MSG_LIB(TL_LOG_NODE_SDK,(u8 *)&(p_notify->capa), 
								sizeof(pro_trans_capa),"send capa cmd ");
	SET_TC_FIFO(TSCRIPT_PROVISION_SERVICE|TSCRIPT_MESH_RX,(u8 *)&p_rcv_str->invite,sizeof(pro_trans_invite));
	SET_TC_FIFO(TSCRIPT_PROVISION_SERVICE,(u8 *)p_notify,notify_len);
	return notify_len;
}
	#if CERTIFY_BASE_ENABLE
int provisionee_gatt_rcv_rec_req_rsp(mesh_pro_data_t *p_notify,mesh_pro_data_t *p_rcv_str)
{
	pro_trans_record_request *p_req = &(p_rcv_str->rec_req);
	endianness_swap_u16((u8*)&p_req->rec_id);
	endianness_swap_u16((u8*)&p_req->frag_off);
	endianness_swap_u16((u8*)&p_req->frag_max_size);
	pro_trans_record_rsp *p_rsp = &(p_notify->rec_rsp);
	u8 cert_buf[MAX_PROV_FRAG_SIZE*2];
	u16 cert_len = 0;
	static u16 total_len =0;
	u8 sts =0;
	int err =0;
	if(p_req->frag_max_size == 0){
		return 0;
	}
	err = cert_item_rsp(p_req->rec_id,p_req->frag_off,p_req->frag_max_size,cert_buf,&cert_len);
	// excced the frag offset part 
	if(err == -1){// record is not exist
		sts = RECORD_REQ_IS_NOT_PRESENT;
		cert_len = 0;
		total_len =0;
	}else if (err ==-2){
		sts = RECORD_REQ_OFF_OUTBOUND;
		cert_len = 0;
	}else if (err == 0 ){
		sts = RECORD_PROV_SUC;
		total_len = use_cert_id_get_len(p_req->rec_id);
	}
	set_pro_record_rsp(p_rsp,sts,p_req->rec_id,p_req->frag_off,
						total_len,cert_buf,cert_len);
	return (sizeof(pro_trans_record_rsp)- MAX_PROV_FRAG_SIZE + cert_len);
}
	#endif
#endif

u8 prov_oob_is_no_oob()
{
	#if WIN32
	extern u8 gatt_pro_input[0x91];
	pro_trans_start *p_start = (pro_trans_start *)(gatt_pro_input+11);
	#else
	pro_trans_start *p_start = (pro_trans_start *)(confirm_input+11);
	#endif
	if(p_start->authMeth == AUTH_MESH_NO_OOB){
		return 1;
	}else{
		return 0;
	}
}

u8 prov_confirm_check_same_or_not(u8 *rcv_comf,u8 *comf)
{
	return !memcmp(rcv_comf,comf,get_prov_confirm_value_len());
}

u8 prov_confirm_check_right_or_not(u8 *rcv_comf,u8 *comf)
{
	return (u8)memcmp(rcv_comf,comf,get_prov_confirm_value_len());
}


int mesh_sec_prov_confirmation_sec (unsigned char *cfm, unsigned char *input, int n, unsigned char ecdh[32],
									unsigned char random[16], unsigned char auth[16])
{
	mesh_sec_prov_confirmation_fun(cfm,input,n,ecdh,random,auth,is_prov_oob_hmac_sha256());
	return 0;
}


int mesh_sec_prov_confirmation_send_confirm_state()
{
	u8 *p_auth = dev_auth;
#if(MESH_USER_DEFINE_MODE == MESH_CLOUD_ENABLE || LLSYNC_PROVISION_AUTH_OOB)
	u8 node_auth[16];
	#if LLSYNC_PROVISION_AUTH_OOB
	llsync_mesh_auth_clac(dev_random, node_auth);
	#else
	calculate_sha256_node_auth_value(node_auth);
	#endif
	p_auth = node_auth;
#endif

	return mesh_sec_prov_confirmation_sec (dev_confirm, confirm_input, 145, ecdh_secret, dev_random, p_auth);
}

int mesh_sec_prov_confirmation_check_confirm_state(u8 *pro_confirm_tmp)
{
	u8 *p_auth = dev_auth;
#if(LLSYNC_PROVISION_AUTH_OOB)
	u8 node_auth[16];
	llsync_mesh_auth_clac(pro_random, node_auth);
	p_auth = node_auth;
#elif ALI_NEW_PROTO_EN
	u8 ali_tmp_auth[32];
	calculate_sha256_to_create_pro_oob(ali_tmp_auth,pro_random);
	p_auth = ali_tmp_auth;
#endif

	return mesh_sec_prov_confirmation_sec (pro_confirm_tmp, confirm_input, 145, ecdh_secret, pro_random, p_auth);
}

/**
 * @brief       This function is for a unprovisioned device to be provisioned through PB_GATT bearer, and handle all messages during provision flow.
 *              if a provison message need to be assembled, it was assembled in pkt_pb_gatt_data() before.
 * @param[in]   p	- pointer of provisioning PDU.
 * @param[in]   len	- length of provisioning PDU.
 * @return      none
 * @note        
 */
void dispatch_pb_gatt(u16 connHandle, u8 *p, u16 len)
{
#if !__PROJECT_MESH_PRO__
	mesh_pro_data_t *p_rcv_str = (mesh_pro_data_t *)p;
	u8 gatt_send_buf[PROVISION_GATT_MAX_LEN];
	memset(gatt_send_buf,0,sizeof(gatt_send_buf));
	mesh_pro_data_t *p_notify = (mesh_pro_data_t *)gatt_send_buf;
	u16 notify_len=0;
	u8 rcv_prov_type;
	rcv_prov_type = p_rcv_str->invite.header.type;
	// dispatch the code is valid or not 
	if(!prov_cmd_is_valid(rcv_prov_type)){
	    LOG_MSG_ERR(TL_LOG_PROVISION,0, 0 ,"gatt prov cmd is invalid,%d",rcv_prov_type);
		notify_len = prov_fail_cmd_proc(p_notify,INVALID_PDU);
		gatt_prov_notify_pkts(connHandle, gatt_send_buf, notify_len, PROVISION_ATT_HANDLE, MSG_PROVISION_PDU);
		return ;
	}
	if(!prov_calc_cmd_len_chk(rcv_prov_type,len)){
	    LOG_MSG_ERR(TL_LOG_PROVISION,0, 0 ,"gatt the length of the prov cmd is invalid,%d",rcv_prov_type);
		notify_len = prov_fail_cmd_proc(p_notify,INVALID_FORMAT);
		gatt_prov_notify_pkts(connHandle, gatt_send_buf, notify_len, PROVISION_ATT_HANDLE, MSG_PROVISION_PDU);
		return ;
	}
	switch(prov_para.provison_rcv_state){
		#if CERTIFY_BASE_ENABLE
		case STATE_PRO_REC_REQ:
			if(prov_para.cert_base_en ){
				if(rcv_prov_type == PRO_REC_REQ){
					//rsp the information part 
					LOG_MSG_INFO(TL_LOG_PROVISION,0, 0 ,"rcv record req");
					notify_len = provisionee_gatt_rcv_rec_req_rsp(p_notify,p_rcv_str);
				}else if (rcv_prov_type == PRO_INVITE){
					// if in the pro req mode ,when receive the invite cmd ,it will jump to the capa rcv part 
					notify_len = provisionee_gatt_rcv_invite_proc(p_notify,p_rcv_str);
					if(notify_len == 0){
						return ;
					}
				}
			}
			break;
		#endif
		case LINK_UNPROVISION_STATE:
			#if CERTIFY_BASE_ENABLE
			if(prov_para.cert_base_en ){
				LOG_MSG_INFO(TL_LOG_PROVISION,0, 0 ,"rcv record get");
				if(rcv_prov_type == PRO_REC_GET){
					u16 rec_id[16];
					u32 rec_cnt = cert_id_get(rec_id);
					set_pro_record_list(&(p_notify->rec_list),0,rec_id,rec_cnt);
					notify_len = (3+rec_cnt*2);
					LOG_MSG_INFO(TL_LOG_PROVISION,(u8*)&(p_notify->rec_list), notify_len ,"rcv list data is ");
					prov_para.provison_rcv_state = STATE_PRO_REC_REQ;
				}else{
					// wait for add the information about the err code part 
				}
			}else 
			#endif
			if(rcv_prov_type == PRO_INVITE){
				notify_len = provisionee_gatt_rcv_invite_proc(p_notify,p_rcv_str);
				if(notify_len == 0){
					return ;
				}
			}
			break;
		case STATE_DEV_CAPA:
			if(rcv_prov_type == PRO_START){
				if(dispatch_start_cmd_reliable(p_rcv_str)){
					#if PROV_AUTH_LEAK_RECREATE_KEY_EN
					init_ecc_key_pair(1);
					#endif
					mesh_node_prov_event_callback(EVENT_MESH_NODE_RC_LINK_START);
					LOG_MSG_LIB(TL_LOG_NODE_SDK,(u8 *)&(p_rcv_str->start), 
								sizeof(pro_trans_start),"rcv start cmd ");
					set_node_prov_start_oob(p_rcv_str,&prov_oob);//set the start cmd for the prov oob info 
					SET_TC_FIFO(TSCRIPT_PROVISION_SERVICE|TSCRIPT_MESH_RX,(u8 *)p,sizeof(pro_trans_start));
					memcpy(confirm_input+12,&(p_rcv_str->start.algorithms),5);
					prov_para.provison_rcv_state =STATE_PRO_START;
					LAYER_PARA_DEBUG(A_provision_start);
				}else{
					// send terminate ind
                    LOG_MSG_ERR(TL_LOG_PROVISION,0, 0 ,"gatt the start cmd is invalid");
					#if !WIN32 
					notify_len = prov_fail_cmd_proc(p_notify,UNEXPECTED_PDU);
					#endif 
					mesh_node_prov_event_callback(EVENT_MESH_NODE_RC_LINK_FAIL_CODE);
				}
			}else{
			    LOG_MSG_ERR(TL_LOG_PROVISION,0, 0 ,"gatt rcv err opcode in the STATE_DEV_CAPA state");
				notify_len = prov_fail_cmd_proc(p_notify,UNEXPECTED_PDU);
			}
			break;
		case STATE_PRO_START:
			if(rcv_prov_type == PRO_PUB_KEY){
				LOG_MSG_LIB(TL_LOG_NODE_SDK,(u8 *)&(p_rcv_str->pubkey), 
								sizeof(pro_trans_pubkey),"rcv pubkey cmd ");
				if(!mesh_check_pubkey_valid(p_rcv_str->pubkey.pubKeyX)){
					notify_len = prov_fail_cmd_proc(p_notify,UNEXPECTED_PDU);
					break;
				}
				u8 dev_public_key[64];
				get_public_key(dev_public_key);
				#if PROV_AUTH_LEAK_REFLECT_EN
				if(!memcmp(p_rcv_str->pubkey.pubKeyX,dev_public_key,sizeof(dev_public_key))){
					// the pubkey is the same with the master .it will need to recreate.
					init_ecc_key_pair(1);
					get_public_key(dev_public_key);
				}
				#endif
				memcpy(confirm_input+0x11,p_rcv_str->pubkey.pubKeyX,0x40);
				memcpy(confirm_input+0x11+0x40,dev_public_key,0x40);
				prov_para.provison_rcv_state =STATE_DEV_PUB_KEY;
				set_pro_pub_key(p_notify,dev_public_key,dev_public_key+32);
				notify_len = sizeof(pro_trans_pubkey);
				LAYER_PARA_DEBUG(A_provision_pubkey);
				SET_TC_FIFO(TSCRIPT_PROVISION_SERVICE|TSCRIPT_MESH_RX,(u8 *)p,sizeof(pro_trans_pubkey));
				SET_TC_FIFO(TSCRIPT_PROVISION_SERVICE,(u8 *)p_notify,notify_len);
				LOG_MSG_LIB(TL_LOG_NODE_SDK,(u8 *)&(p_notify->pubkey),
								sizeof(pro_trans_pubkey),"send pubkey cmd ");
				if(prov_para.cert_base_en){
					// will not send back the pubkey.
					return ;
				}
			}else{
			    LOG_MSG_ERR(TL_LOG_PROVISION,0, 0 ,"gatt rcv err opcode in the STATE_PRO_START state");
				notify_len = prov_fail_cmd_proc(p_notify,UNEXPECTED_PDU);
			}
			break;
		case STATE_DEV_PUB_KEY:
			if(rcv_prov_type == PRO_CONFIRM){
				if(!mesh_node_oob_auth_data(&prov_oob)){// set the auth part ,only for the no oob ,and static oob
					// send terminate ind 
					LOG_MSG_ERR(TL_LOG_PROVISION,0, 0 ,"gatt the start cmd can not suit to capa cmd");
					#if !WIN32 
						#if BLE_MULTIPLE_CONNECTION_ENABLE
					blc_ll_disconnect(connHandle, HCI_ERR_REMOTE_USER_TERM_CONN);
						#else
					bls_ll_terminateConnection(0x13);
						#endif
					#endif 
					mesh_node_prov_event_callback(EVENT_MESH_NODE_RC_LINK_FAIL_CODE);
					return ;
				}
				#if PROV_AUTH_LEAK_REFLECT_EN
				if(prov_confirm_check_same_or_not((u8 *)&(p_rcv_str->confirm),dev_confirm)){
					#if !WIN32 
						#if BLE_MULTIPLE_CONNECTION_ENABLE
					blc_ll_disconnect(connHandle, HCI_ERR_REMOTE_USER_TERM_CONN);
						#else
					bls_ll_terminateConnection(0x13);
						#endif
					#endif
					return;
				}
				#endif
				LOG_MSG_LIB(TL_LOG_NODE_SDK,(u8 *)&(p_rcv_str->confirm), 
								get_prov_confirm_len(),"rcv confirm cmd ");
				pro_trans_confirm *p_confirm;
				u8 dev_private_key[32];
				p_confirm = &(p_rcv_str->confirm);
				memcpy(pro_confirm,p_confirm->confirm,get_prov_confirm_value_len());
				get_private_key(dev_private_key);
				tn_p256_dhkey_fast(ecdh_secret, dev_private_key, confirm_input+0x11, confirm_input+0x11+0x20);
				mesh_sec_prov_confirmation_send_confirm_state();				
				set_pro_confirm(p_notify,dev_confirm,get_prov_confirm_value_len());
				notify_len = get_prov_confirm_len();
				prov_para.provison_rcv_state =STATE_DEV_CONFIRM;
				LAYER_PARA_DEBUG(A_provision_confirm);
				SET_TC_FIFO(TSCRIPT_PROVISION_SERVICE|TSCRIPT_MESH_RX,(u8 *)p,get_prov_confirm_len());
				SET_TC_FIFO(TSCRIPT_PROVISION_SERVICE,(u8 *)p_notify,notify_len);
				LOG_MSG_LIB(TL_LOG_NODE_SDK,(u8 *)&(p_notify->confirm), 
								get_prov_confirm_len(),"send confirm cmd ");
			}else{
			    LOG_MSG_ERR(TL_LOG_PROVISION,0, 0 ,"gatt rcv err cmd in the STATE_DEV_PUB_KEY state");
				notify_len = prov_fail_cmd_proc(p_notify,UNEXPECTED_PDU);
			}
			break;
		case STATE_DEV_CONFIRM:
			if(rcv_prov_type == PRO_RANDOM){
				LOG_MSG_LIB(TL_LOG_NODE_SDK,(u8 *)&(p_rcv_str->random), 
								get_prov_random_len(),"rcv random cmd ");
				memcpy(pro_random,p_rcv_str->random.random,get_prov_random_value_len());
				// use the provision random to calculate the provision confirm 
				u8 pro_confirm_tmp[32];
				mesh_sec_prov_confirmation_check_confirm_state(pro_confirm_tmp);
				if(prov_confirm_check_right_or_not(pro_confirm,pro_confirm_tmp)){
				    LOG_MSG_ERR(TL_LOG_PROVISION,0, 0 ,"gatt the confirm compare fail");
					notify_len = prov_fail_cmd_proc(p_notify,UNEXPECTED_PDU);
					break;
				}

				set_pro_random(p_notify,dev_random,get_prov_random_value_len());		
				notify_len = get_prov_random_len();
				prov_para.provison_rcv_state =STATE_DEV_RANDOM;
				LAYER_PARA_DEBUG(A_provision_random);
				SET_TC_FIFO(TSCRIPT_PROVISION_SERVICE|TSCRIPT_MESH_RX,(u8 *)p,get_prov_random_len());
				SET_TC_FIFO(TSCRIPT_PROVISION_SERVICE,(u8 *)p_notify,notify_len);
				LOG_MSG_LIB(TL_LOG_NODE_SDK,(u8 *)&(p_notify->random),get_prov_random_len(),
								"send random cmd ");
			}else{
			    LOG_MSG_ERR(TL_LOG_PROVISION,0, 0 ,"gatt rcv err cmd in the STATE_DEV_CONFIRM state");
				notify_len = prov_fail_cmd_proc(p_notify,UNEXPECTED_PDU);
			}
			break;
		case STATE_DEV_RANDOM:
			if(rcv_prov_type == PRO_DATA){
				// need to be checked 
				LOG_MSG_LIB(TL_LOG_NODE_SDK,(u8 *)&(p_rcv_str->data),
							sizeof(pro_trans_data),"rcv provision data cmd ");
				u8 *p_prov_net;
				p_prov_net = (u8 *)(p+1);	// provison_net_info_str
				u8 dev_key[16];
				u8 prov_salt[16];
				u8 dev_session_key[16];
				u8 dev_session_nonce[16];
				mesh_sec_prov_salt_fun(prov_salt,confirm_input,pro_random,dev_random,is_prov_oob_hmac_sha256());
				mesh_sec_prov_session_key_fun (dev_session_key, dev_session_nonce, confirm_input, 145, ecdh_secret, pro_random, dev_random,is_prov_oob_hmac_sha256());
				
				int err = mesh_prov_sec_msg_dec (dev_session_key, dev_session_nonce+3, p_prov_net, 33, 8);
				if(err){
                    notify_len = prov_fail_cmd_proc(p_notify,DECRYPTION_FAIL);
    				//LOG_MSG_ERR(TL_LOG_NODE_SDK,0,0,"provision fail, data decryption err: 0x%x",err);
				}else{
                    //calculate the dev_key part 
                    mesh_sec_dev_key(dev_key,prov_salt,ecdh_secret);
                    set_dev_key(dev_key);
                    LOG_MSG_LIB(TL_LOG_PROVISION,dev_key, 16,"device key ");
    				LOG_MSG_LIB(TL_LOG_NODE_SDK,p_prov_net,
    							25,"provision net info is ");
    				memcpy(&provision_mag.pro_net_info,p_prov_net,sizeof(provison_net_info_str));
    				// add the info about the gatt mode provision ,should set the cfg data part into the node identity
    				mesh_provision_par_handle(&provision_mag.pro_net_info);
    				#if !WIN32 
    				mesh_node_prov_event_callback(EVENT_MESH_NODE_RC_LINK_SUC);
    				#endif 
    				// send back the complete cmd 
    				LAYER_PARA_DEBUG(A_provision_data);
    				gatt_send_buf[0]= PRO_COMPLETE ;
    				notify_len=1;
    				cache_init(ADR_ALL_NODES);
    				prov_para.provison_rcv_state = STATE_PRO_DATA;
    				prov_para.provison_rcv_state = STATE_PRO_SUC;
    				SET_TC_FIFO(TSCRIPT_PROVISION_SERVICE|TSCRIPT_MESH_RX,(u8 *)p,sizeof(pro_trans_data));
    				SET_TC_FIFO(TSCRIPT_PROVISION_SERVICE,(u8 *)p_notify,notify_len);
    				LOG_MSG_LIB(TL_LOG_NODE_SDK,0, 0,"provision suc! ");
					#if PROVISION_SUCCESS_QUICK_RECONNECT_ENABLE
					extern u32 g_provision_success_adv_quick_reconnect_tick;
    				g_provision_success_adv_quick_reconnect_tick = clock_time() | 1;
					#endif
    				#if PROV_APP_KEY_SETUP_TIMEOUT_CHECK_EN
    				prov_app_key_setup_tick = clock_time() | 1;
    				#endif
    				mesh_key_node_identity_set_prov_set();
				}
			}else{
			    LOG_MSG_ERR(TL_LOG_PROVISION,0, 0 ,"gatt rcv err cmd in the STATE_DEV_CONFIRM state");
				notify_len = prov_fail_cmd_proc(p_notify,UNEXPECTED_PDU);
				
			}
			break;
		default:
			return;
			break;
	}
	if(notify_len){
		gatt_prov_notify_pkts(connHandle, gatt_send_buf, notify_len, PROVISION_ATT_HANDLE, MSG_PROVISION_PDU);
	}
#endif 
	return;
}

void mesh_adv_provision_retry()
{
	// if the retry time excced about 20s ,it will reset the states 
	if(prov_para.cmd_retry_flag &&clock_time_exceed(prov_para.cmd_send_start_tick,PROV_ADV_TIMEOUT_MS*1000)){
		#if GATEWAY_ENABLE
		mesh_terminate_provision_link_reset(LINK_CLOSE_TIMEOUT,0);
		#else
		mesh_provision_para_reset();
		LOG_MSG_ERR(TL_LOG_NODE_SDK, 0, 0,"provision time_out");
		mesh_node_prov_event_callback(EVENT_MESH_NODE_RC_LINK_TIMEOUT);
		#endif
	}
	if(	prov_para.cmd_retry_flag &&
		clock_time_exceed(prov_para.cmd_send_tick ,PROVISION_ADV_RETRY_MS*1000)){
		
		if(my_fifo_data_cnt_get(&mesh_adv_cmd_fifo)>2){//make sure enough buf
			return;
		}
		
		if(prov_para.pro_cmd == PRO_COMMAND_ACK){
			mesh_send_provison_data(TRANS_ACK,0,0,0);
			mesh_send_provison_data(TRANS_ACK,0,0,0);
		}else if(prov_para.pro_cmd == PRO_BEARS_CTL){
		    if(prov_para.pro_bearerCode == LINK_OPEN){
                mesh_send_provison_data(BEARS_CTL,prov_para.pro_bearerCode,prov_link_uuid,sizeof(prov_link_uuid));
		    }else{
		    	if(prov_para.ack_retry_flag){ //whether need to send link close in the provision end .
					mesh_send_provison_data(TRANS_ACK,0,0,0);
				}
                mesh_send_provison_data(BEARS_CTL,prov_para.pro_bearerCode,0,0);
		    }
		}
        else if(prov_para.pro_cmd == PRO_BEACON){
            unprov_beacon_send(MESH_UNPROVISION_BEACON_WITH_URI,0);
        }
        else{
			if(prov_para.ack_retry_flag){
				#if PTS_TEST_EN
				mesh_send_provison_data(TRANS_ACK,0,0,0);
				#endif
				mesh_send_provison_data(TRANS_ACK,0,0,0);
			}
			send_multi_type_data(prov_para.pro_cmd,para_pro);
		}
		prov_para.cmd_send_tick =clock_time();
		if(prov_para.link_close_flag && prov_para.link_close_cnt){
			prov_para.link_close_cnt-- ;
			if(prov_para.link_close_cnt--){
				mesh_send_provison_data(BEARS_CTL,LINK_CLOSE,0,0);
			}else{
				prov_para.link_close_flag =0;
				prov_para.link_close_cnt =0;
			}
		}
	}
}

void set_rsp_ack_transnum(pro_PB_ADV *p_adv){
	trans_bear_header *p_header = &(p_adv->transBear.bearOpen.header);
	if(	(p_header->GPCF == BEARS_CTL && p_header->BearCtl == LINK_ACK)){
		prov_para.rsp_ack_transnum = 0;
	}else if(p_header->GPCF!= TRANS_ACK){
		prov_para.rsp_ack_transnum = p_adv->trans_num;
	}
	else{
	}
}

void provision_set_ivi_para(provison_net_info_str *p_prov_net_info)
{
	u32 iv_idx = 0;
	get_iv_little_endian((u8 *)&iv_idx, p_prov_net_info->iv_index);
	if(p_prov_net_info->prov_flags.IVUpdate
	&& iv_idx){ // (iv_idx != 0)
		iv_update_set_with_update_flag_true(iv_idx, 0);
	}else{
        int rst_sno = (iv_idx != iv_idx_st.iv_cur);
        mesh_iv_idx_init(iv_idx, rst_sno, 1);
        iv_idx_st.searching_flag = 1; // enter search mode should be better, because an old iv may be used to provision.
	}
}

u8 proc_start_cmd_reliable(pro_trans_start *p_start)
{
  if(p_start->algorithms>BTM_ECDH_P256_HMAC_SHA256_AES_CCM){
		return 0;
	}else if(p_start->pubKey >= 2){
		return 0;
	}else if(p_start->authMeth >= 4){
		return 0;
	}else if(p_start->authMeth == AUTH_MESH_NO_OOB){
		if(p_start->authAct!=0 || p_start->authSize !=0){
			return 0;
		}
	}else if (p_start->authMeth == AUTH_MESH_STATIC_OOB){
		if(p_start->authAct!=0 || p_start->authSize !=0){
			return 0;
		}
	}else if (p_start->authMeth == AUTH_MESH_OUTPUT_OOB){
		if(p_start->authAct>4 || (p_start->authSize == 0 || p_start->authSize>8)){
			return 0;
		}
	}else if (p_start->authMeth == AUTH_MESH_INPUT_OOB){
		if(p_start->authAct>4 || (p_start->authSize == 0 || p_start->authSize>8)){
			return 0;
		}
	}else{
	}  
	return 1;

}

#define ENABLE_NO_OOB_IN_STATIC_OOB     0

u8 compare_capa_with_start(mesh_prov_oob_str *p_oob)
{
#if ENABLE_NO_OOB_IN_STATIC_OOB
    return 1;
#else
    pro_trans_capa *p_capa = &(p_oob->capa);
    pro_trans_start *p_start = &(p_oob->start);
    //compare the pubkey oob part
	if((BIT(p_start->algorithms)&p_capa->algorithms)== 0){
		LOG_MSG_ERR(TL_LOG_NODE_SDK, 0, 0,"algorithms is start%d,capa is %d",p_start->algorithms,p_capa->algorithms);	
		return 0;
	}
    if(p_capa->pubKeyType != p_start->pubKey){
        return 0;
    }
    //compare the sec method 
    if(p_oob->prov_key != p_start->authMeth){
        return 0;
    }
    if( p_start->authMeth == MESH_NO_OOB ||
        p_start->authMeth == MESH_STATIC_OOB){
        if(p_start->authAct!=0 || p_start->authSize!=0){
           return 0;
        }
    }else if (p_start->authMeth == MESH_OUTPUT_OOB){
        if( p_oob->oob_outsize != p_start->authSize ||
            p_oob->oob_outAct  != p_start->authAct){
            return 0;
        }
    }else if (p_start->authMeth == MESH_INPUT_OOB){
        if( p_oob->oob_insize != p_start->authSize ||
            p_oob->oob_inAct  != p_start->authAct){
            return 0;
        }
    }else{}
    return 1;
#endif
}

u8 dispatch_start_cmd_reliable(mesh_pro_data_t *p_rcv_str)
{
	pro_trans_start *p_start = &(p_rcv_str->start);
	if(!proc_start_cmd_reliable(p_start)){
        return 0;
	}
	get_pubkey_oob_info_by_capa(&prov_oob);
	set_node_prov_start_oob(p_rcv_str,&prov_oob);//set the start cmd for the prov oob info 
	if(!compare_capa_with_start(&prov_oob)){
        return 0;
	}
	return 1;
}

int mesh_prov_sec_msg_enc(unsigned char key[16], unsigned char nonce[13], unsigned char* dat, int n, int mic_length)
{
	provison_net_info_str *p_prov = (provison_net_info_str *)(dat);
	endianness_swap_u16((u8 *)(&p_prov->key_index));
	endianness_swap_u16((u8 *)(&p_prov->unicast_address));
	return mesh_sec_msg_enc(key,nonce,dat,n,mic_length);
}

int	mesh_prov_sec_msg_dec (unsigned char key[16], unsigned char nonce[13], unsigned char* dat, int n, int mic_length)
{
	int err=-1;
	err = mesh_sec_msg_dec (key, nonce, dat, n, mic_length);
	provison_net_info_str *p_prov = (provison_net_info_str *)(dat);
	endianness_swap_u16((u8 *)(&p_prov->key_index));
	endianness_swap_u16((u8 *)(&p_prov->unicast_address));
	return err;
}
static u32 tick_check_complete=0;
// the ack should be triggered by the send cmd ,so we just need to dispatch in the receive state

void set_node_prov_capa_oob(mesh_prov_oob_str *p_prov_oob,u8 pub_key_type,
					u8 staticOOBType,u16 outPutOOBAct,u16 inPutOOBAct)
{
	pro_trans_capa *p_capa = &(p_prov_oob->capa);
	memset((u8 *)p_capa,0,sizeof(pro_trans_capa));
	p_capa->ele_num =g_ele_cnt;
	#if PROV_EPA_EN
	p_capa->algorithms = BIT(BTM_ECDH_P256_CMAC_AES128_AES_CCM)|BIT(BTM_ECDH_P256_HMAC_SHA256_AES_CCM);
	#else
	p_capa->algorithms = BIT(BTM_ECDH_P256_CMAC_AES128_AES_CCM);
	#endif
	p_capa->pubKeyType = pub_key_type;
	if(staticOOBType & BIT(STATIC_OOB_TYPE_VAL)){
		p_capa->staticOOBType = BIT(STATIC_OOB_TYPE_VAL);
		p_capa->outPutOOBSize = 0;
		p_capa->outPutOOBAct = 0;
		p_capa->inOOBSize = 0;
		p_capa->inOOBAct = 0;
	}else{
		// no oob ,or the input oob and output oob part 
		if(outPutOOBAct!=0){ // output oob 
			p_capa->staticOOBType = BIT(ONLY_OOB_TYPE_VAL);
			p_capa->outPutOOBAct = BIT(MESH_OUT_ACT_BLINK);
			p_capa->outPutOOBSize = 1;
			p_capa->inOOBSize = 0;
			p_capa->inOOBAct = 0;
		}else if (inPutOOBAct!=0){ // input oob 
			p_capa->staticOOBType = BIT(ONLY_OOB_TYPE_VAL);
			p_capa->outPutOOBSize = 0;
			p_capa->outPutOOBAct = 0;
			p_capa->inOOBAct = BIT(MESH_IN_ACT_PUSH);
			p_capa->inOOBSize = 1;
		}else{// no oob 
			p_capa->staticOOBType = 0;
			p_capa->outPutOOBSize = 0;
			p_capa->outPutOOBAct = 0;
			p_capa->inOOBSize = 0;
			p_capa->inOOBAct = 0;
			
		}
	}
}

void set_node_prov_capa_oob_init()
{
	set_node_prov_capa_oob(&prov_oob,MESH_PUB_KEY_WITHOUT_OOB,0,0,0);
}

void set_node_prov_para_no_pubkey_no_oob()
{
	set_node_prov_capa_oob(&prov_oob,MESH_PUB_KEY_WITHOUT_OOB,0,0,0);
}

void set_node_prov_para_no_pubkey_static_oob()
{
	set_node_prov_capa_oob(&prov_oob,MESH_PUB_KEY_WITHOUT_OOB,MESH_STATIC_OOB,0,0);
}

void set_node_prov_para_no_pubkey_input_oob()
{
	set_node_prov_capa_oob(&prov_oob,MESH_PUB_KEY_WITHOUT_OOB,0,0,1);
}

void set_node_prov_para_no_pubkey_output_oob()
{
	set_node_prov_capa_oob(&prov_oob,MESH_PUB_KEY_WITHOUT_OOB,0,1,0);
}

void set_node_prov_para_pubkey_no_oob()
{
	set_node_prov_capa_oob(&prov_oob,MESH_PUB_KEY_WITH_OOB,0,0,0);
}

void set_node_prov_para_pubkey_static_oob()
{
	set_node_prov_capa_oob(&prov_oob,MESH_PUB_KEY_WITH_OOB,MESH_STATIC_OOB,0,0);
}

void set_node_prov_para_pubkey_input_oob()
{
	set_node_prov_capa_oob(&prov_oob,MESH_PUB_KEY_WITH_OOB,0,0,1);
}

void set_node_prov_para_pubkey_output_oob()
{
	set_node_prov_capa_oob(&prov_oob,MESH_PUB_KEY_WITH_OOB,0,1,0);
}


void set_node_prov_start_oob(mesh_pro_data_t *p_rcv,mesh_prov_oob_str *p_oob)
{
	pro_trans_start *p_start = &(p_rcv->start);
	memcpy((u8 *)(&p_oob->start),(u8 *)p_start,sizeof(pro_trans_start));
}
u32 mesh_node_out_oob_pub_key_tick =0;
#define MESH_NODE_OUT_OOB_PUB_KEY_TIME_S	6*1000*1000
void check_mesh_node_out_oob_pub_key_send_time()
{
	if(clock_time_exceed(mesh_node_out_oob_pub_key_tick,MESH_NODE_OUT_OOB_PUB_KEY_TIME_S)&&
		mesh_node_out_oob_pub_key_tick){
		// stop send the pubkey 
		mesh_node_out_oob_pub_key_tick =0;
		send_rcv_retry_clr();
	}
}
u32 link_close_end_tick =0;
void mesh_prov_link_close_terminate()
{
	if(link_close_end_tick && clock_time_exceed(link_close_end_tick,2*1000*1000)){
		send_rcv_retry_clr();
		mesh_adv_prov_link_close();
		link_close_end_tick =0;	
		mesh_node_prov_event_callback(EVENT_MESH_NODE_RC_LINK_CLOSE);
	}
}
void mesh_terminate_provision_link_reset(u8 code,u8 ack)
{	
	send_rcv_retry_clr();
	if(LINK_CLOSE_SUCC != code){
		gateway_adv_filter_init();
		link_close_end_tick = clock_time()|1;
	}
	if(ack){
		mesh_send_provison_data(TRANS_ACK,0,0,0); 
	}
	prov_set_link_close_code(code);
	mesh_send_provison_data(BEARS_CTL,LINK_CLOSE,0,0);
	send_rcv_retry_set(PRO_BEARS_CTL,LINK_CLOSE, ack); 
	LOG_MSG_LIB(TL_LOG_NODE_SDK, 0, 0,"send link close");	
   	#if GATEWAY_ENABLE
	bear_close_link link_close;
	link_close.header.GPCF = BEARS_CTL;
	link_close.header.BearCtl = LINK_CLOSE;
	link_close.reason = code;
    gateway_upload_prov_link_cls((u8 *)&link_close,sizeof(bear_close_link));
    #endif
}


u32 mesh_check_pubkey_valid(u8 *rcv_ppk)
{
#if WIN32
	return 1;
#else
    const struct uECC_Curve_t * p_curve;
    p_curve = uECC_secp256r1();
	u8 ecc_ppk[64];
	swapX(&rcv_ppk[0],&ecc_ppk[0],32);
    swapX(&rcv_ppk[32],&ecc_ppk[32],32);
    return uECC_valid_public_key((const uint8_t *)ecc_ppk, p_curve);
#endif
}
void check_inputoob_proc()
{
#if !__PROJECT_MESH_PRO__
	mesh_pro_data_t *p_send_str = (mesh_pro_data_t *)(para_pro);
	if(prov_para.provison_rcv_state ==  STATE_DEV_PUB_KEY_INPUT_OOB){
		// need to send the cmd of the input complete			
		if(prov_auth_en_flag ){
			prov_auth_en_flag =0;
			send_rcv_retry_clr();
			set_pro_input_complete(p_send_str);
			send_multi_type_data(PRO_INPUT_COM,para_pro);
			send_rcv_retry_set(PRO_INPUT_COM,0,0);
			prov_para.trans_num_last = prov_para.trans_num;
			prov_para.provison_rcv_state = STATE_DEV_PUB_KEY_ACK;
		}
	}
#endif
}


void mesh_prov_node_fail_proc(pro_PB_ADV *p_adv,mesh_pro_data_t *p_send_str,u8 code)
{
	prov_para.prov_err = 1;
	LOG_MSG_ERR(TL_LOG_PROVISION,0, 0 ,"prov fail proc ");
	set_rsp_ack_transnum(p_adv);
	send_rcv_retry_clr();
	mesh_send_provison_data(TRANS_ACK,0,0,0);
	set_pro_fail(p_send_str,code);
	send_multi_type_data(PRO_FAIL,para_pro);
	send_rcv_retry_set(PRO_FAIL,0,1);
	prov_para.trans_num_last = prov_para.trans_num;
}

/**
 * @brief       This function is for a unprovisioned device to be provisioned through PB_ADV bearer, and handle all messages during provision flow.
 *              if a provison message need to be assembled, it was assembled in mesh_provison_process() before.
 * @param[in]   p_adv	- pointer of provisioning PDU.
 * @return      none
 * @note        
 */
void mesh_node_rc_data_dispatch(pro_PB_ADV *p_adv){
	// unprovision device  
	#if !__PROJECT_MESH_PRO__	
	mesh_pro_data_t *p_send_str = (mesh_pro_data_t *)(para_pro);
	mesh_pro_data_t *p_rcv_str = (mesh_pro_data_t *)(p_adv->transStart.data);
	#if MD_REMOTE_PROV
    if(mesh_pr_sts_work_or_not()){
	    mesh_prov_server_rcv_cmd(p_adv);
        return ;
	}
	#endif
	// if the rcv pkt is a adv pkt ,we should remember the transaction num ,then rsp with the same transaction num 
	if((!prov_para.link_id_filter) && (!(p_adv->transBear.bearOpen.header.GPCF == BEARS_CTL &&
		p_adv->transBear.bearAck.header.BearCtl == LINK_OPEN))){
		return;
	}

	if(p_adv->transBear.bearOpen.header.GPCF == BEARS_CTL &&
			   p_adv->transBear.bearAck.header.BearCtl == LINK_CLOSE){
		//when receive a link close cmd ,it will reset the state 
		SET_TC_FIFO(TSCRIPT_MESH_RX,(u8 *)(&(p_adv->length)),p_adv->length+1);            
		//reset the link id and others 
		LOG_MSG_LIB(TL_LOG_NODE_SDK,0, 0,"rcv link close cmd ");
		mesh_node_prov_event_callback(EVENT_MESH_NODE_RC_LINK_CLOSE);
		mesh_provision_para_reset();
	}
#if TESTCASE_FLAG_ENABLE

	else if((prov_para.provison_rcv_state!=STATE_PRO_FAILED_ACK)&& 
			(p_adv->transBear.bearAck.header.GPCF == TRANS_START) && 
			(p_adv->transStart.data[0]> PRO_REC_LIST)){
		prov_para.err_op_code = 1;	
		mesh_prov_node_fail_proc(p_adv,p_send_str,PROVISION_FAIL_INVALID_PDU);
		LOG_MSG_LIB(TL_LOG_NODE_SDK,0, 0,"retry err code is  %d",p_adv->transStart.data[0]);
	}
#endif
	if((prov_para.cert_ack_hold == 0)&& p_adv->transAck.GPCF == TRANS_ACK&&p_adv->trans_num >=prov_para.trans_num_last){
		LOG_MSG_LIB(TL_LOG_NODE_SDK,0, 
						0,"rcv transaction ack(transaction:0x%02x)",p_adv->trans_num);
		if(prov_para.provison_rcv_state >= STATE_PRO_START_ACK){
			send_rcv_retry_clr();//stop send cmd when receive ack
		}
		prov_para.trans_num_last = p_adv->trans_num+1 ; // receive trans ack once
		if(prov_para.provison_rcv_state == STATE_PRO_COMPLETE){			
			adv_provision_state_dispatch(p_adv);
			mesh_node_prov_event_callback(EVENT_MESH_NODE_RC_LINK_SUC);
		}
		return;
	}
	
	switch(prov_para.provison_rcv_state){
		case LINK_UNPROVISION_STATE:
			if(p_adv->transBear.bearOpen.header.GPCF == BEARS_CTL &&
			   p_adv->transBear.bearAck.header.BearCtl == LINK_OPEN){	
			    if(	prov_para.provison_rcv_state == LINK_UNPROVISION_STATE&&
					!memcmp(prov_para.device_uuid, p_adv->transBear.bearOpen.DeviceUuid,sizeof(prov_para.device_uuid))){
					if(is_provision_success()){
						// can not provision again ,and stop by the cmd
						LOG_MSG_ERR(TL_LOG_PROVISION,0, 0 ,"adv the dev has already provisioned");
						return ;
					}
					LOG_MSG_LIB(TL_LOG_NODE_SDK,(u8 *)(p_adv), 
								p_adv->length+1,"rcv link open cmd ");
					SET_TC_FIFO(TSCRIPT_MESH_RX,(u8 *)(p_adv),p_adv->length+1);
					if(user_node_rc_link_open_callback() == 0){
						return;
					}
					prov_para.cert_ack_hold = 0;
					prov_para.direct_invite = 0;
				   	memcpy((u8 *)(&prov_para.link_id),p_adv->link_ID,4); 
					set_rsp_ack_transnum(p_adv);
                    mesh_send_provison_data(BEARS_CTL,LINK_ACK,0,0);
					send_rcv_retry_set(PRO_BEARS_CTL,LINK_ACK, 0); 
					SET_TC_FIFO(TSCRIPT_MESH_TX,(u8 *)(&(pro_adv_pkt.len)),pro_adv_pkt.len+1);					
					if(prov_para.cert_base_en){
						prov_para.provison_rcv_state = STATE_PRO_REC_REQ;
					}else{
						prov_para.provison_rcv_state = LINK_ESTABLISHED;
					}
					prov_para.link_id_filter =1;
					prov_para.prov_err = 0;
					mesh_node_prov_event_callback(EVENT_MESH_NODE_RC_LINK_START);
			    }
			}
			break;
		#if CERTIFY_BASE_ENABLE
		case STATE_PRO_REC_REQ:
			if(prov_para.cert_base_en ){
				u8 rcv_prov_type = p_adv->transStart.data[0];
				u32 len =0;
				if(rcv_prov_type == PRO_REC_REQ&&p_adv->transBear.bearAck.header.GPCF == TRANS_START){
					//rsp the information part 
					LOG_MSG_INFO(TL_LOG_NODE_SDK,(u8 *)(p_adv), 
								p_adv->length+1,"rcv PRO_REC_REQ cmd ");
					len = provisionee_gatt_rcv_rec_req_rsp(p_send_str,p_rcv_str);
					if(len == 0){// max size is 0
						mesh_prov_node_fail_proc(p_adv,p_send_str,PROVISION_FAIL_INVALID_FORMAT);
					}
					prov_para.rec_rsp_len = len;
					set_rsp_ack_transnum(p_adv);
					mesh_send_provison_data(TRANS_ACK,0,0,0);
					send_multi_type_data(PRO_REC_RSP,para_pro);
					send_rcv_retry_set(PRO_REC_RSP,0,1);
					LOG_MSG_INFO(TL_LOG_NODE_SDK,(u8 *)(para_pro), 
								len,"send PRO_REC_REQ cmd ");
					prov_para.provison_rcv_state = STATE_PRO_REC_REQ_ACK;
				}else if(rcv_prov_type == PRO_REC_GET&&p_adv->transBear.bearAck.header.GPCF == TRANS_START){
					u16 rec_id[16];
					prov_para.cert_ack_hold = 1;
					prov_para.direct_invite = 0;
					LOG_MSG_INFO(TL_LOG_NODE_SDK,(u8 *)(p_adv), 
								p_adv->length+1,"rcv PRO_REC_GET cmd ");
					set_rsp_ack_transnum(p_adv);
					mesh_send_provison_data(TRANS_ACK,0,0,0);
					u32 rec_cnt = cert_id_get(rec_id);
					set_pro_record_list(&(p_send_str->rec_list),0,rec_id,rec_cnt);
					len = (3+rec_cnt*2);
					prov_para.rec_list_len = len;
					send_multi_type_data(PRO_REC_LIST,para_pro);
					prov_para.provison_rcv_state = STATE_PRO_REC_REQ_ACK;
					send_rcv_retry_set(PRO_REC_LIST,0,1);
					LOG_MSG_INFO(TL_LOG_NODE_SDK,(u8 *)(para_pro), 
								len,"send PRO_REC_LIST cmd ");
					// wait to send the record list part .
				}else if (rcv_prov_type == PRO_INVITE){
					// if in the pro req mode ,when receive the invite cmd ,it will jump to the capa rcv part 
					prov_para.provison_rcv_state = LINK_ESTABLISHED;
					if(prov_para.cert_ack_hold == 0){
						prov_para.direct_invite = 1;
					}
					LOG_MSG_INFO(TL_LOG_NODE_SDK,(u8 *)(para_pro), 
								len,"PRO_INVITE1 cmd ");
					//goto INVITE_PROC;
				}
			}
			break;
		case STATE_PRO_REC_REQ_ACK:
			//
			if(p_adv->transAck.GPCF == TRANS_ACK&&p_adv->trans_num >= prov_para.trans_num ){
				send_rcv_retry_clr();
				LOG_MSG_INFO(TL_LOG_NODE_SDK,0, 
								0,"RCV STATE_PRO_REC_REQ_ACK cmd ");
				prov_para.provison_rcv_state = STATE_PRO_REC_REQ;
			}
			break;
		#endif
		case LINK_ESTABLISHED:           
			if(p_adv->transBear.bearAck.header.GPCF == TRANS_START &&
				p_adv->transStart.data[0]== PRO_INVITE){
				#if CERTIFY_BASE_ENABLE // avoid in the certify-base ,it will direct send invite cmd
				prov_para.cert_ack_hold = 1;
				#endif
				// add the rcv invite part 
				LOG_MSG_LIB(TL_LOG_NODE_SDK,(u8 *)&(p_rcv_str->invite), 
								sizeof(pro_trans_invite),"rcv invite cmd ");
				confirm_input[0] = p_rcv_str->invite.attentionDura;
				SET_TC_FIFO(TSCRIPT_MESH_RX,(u8 *)(p_adv),p_adv->length+1);
				set_rsp_ack_transnum(p_adv);
				prov_para.attention_value= p_rcv_str->invite.attentionDura;
				send_rcv_retry_clr();
				mesh_send_provison_data(TRANS_ACK,0,0,0);
				SET_TC_FIFO(TSCRIPT_MESH_TX,(u8 *)(&(pro_adv_pkt.len)),pro_adv_pkt.len+1);
				// need to set the add the prov oob init part 
				set_pro_capa_cpy(p_send_str,&prov_oob);
				// swap the send buf part 
				swap_mesh_pro_capa(p_send_str);
				// use the send message as the input msg 
				memcpy(confirm_input+1,&(p_send_str->capa.ele_num),11);
				send_multi_type_data(PRO_CAPABLI,para_pro);
				SET_TC_FIFO(TSCRIPT_MESH_TX,(u8 *)(&(pro_adv_pkt.len)),pro_adv_pkt.len+1);
				send_rcv_retry_set(PRO_CAPABLI,0,1);
				LOG_MSG_LIB(TL_LOG_NODE_SDK,(u8 *)&(p_send_str->capa), 
								sizeof(pro_trans_capa),"send capa cmd ");
				prov_para.provison_rcv_state = STATE_DEV_CAPA;
				prov_para.trans_num_last = prov_para.trans_num;
			}
			break;
		case STATE_DEV_CAPA:
			if(p_adv->transBear.bearAck.header.GPCF == TRANS_START &&
				p_adv->transStart.data[0]== PRO_START){
					if(dispatch_start_cmd_reliable(p_rcv_str)){
						if(prov_para.prov_err){
							mesh_prov_node_fail_proc(p_adv,p_send_str,PROVISION_FAIL_UNEXPT_PDU);
							return ;
						}
						prov_para.cert_ack_hold =0;
						#if PROV_AUTH_LEAK_RECREATE_KEY_EN
						init_ecc_key_pair(1);
						#endif
						// get the provision start part 
						LOG_MSG_LIB(TL_LOG_NODE_SDK,(u8 *)&(p_rcv_str->start), 
								sizeof(pro_trans_start),"rcv start cmd ");
						set_node_prov_start_oob(p_rcv_str,&prov_oob);
						memcpy(confirm_input+12,&(p_rcv_str->start.algorithms),5);
						SET_TC_FIFO(TSCRIPT_MESH_RX,(u8 *)(p_adv),p_adv->length+1);
						set_rsp_ack_transnum(p_adv);
						send_rcv_retry_clr();
						mesh_send_provison_data(TRANS_ACK,0,0,0); 
						SET_TC_FIFO(TSCRIPT_MESH_TX,(u8 *)(&(pro_adv_pkt.len)),pro_adv_pkt.len+1);
						send_rcv_retry_set(PRO_COMMAND_ACK,0,0);
						prov_para.trans_num_last += 2; // trans_num_last must change when run into new state
						prov_para.provison_rcv_state = STATE_PRO_START_ACK;
						
					}else{
						LOG_MSG_LIB(TL_LOG_NODE_SDK,0, 
								0,"invalid start cmd");
						#if TESTCASE_FLAG_ENABLE
						if(prov_para.prov_err == 0){
							prov_para.prov_err = 1;
							mesh_prov_node_fail_proc(p_adv,p_send_str,PROVISION_FAIL_INVALID_FORMAT);
						}else{
							mesh_prov_node_fail_proc(p_adv,p_send_str,PROVISION_FAIL_UNEXPT_PDU);
							}
						#else
						mesh_prov_node_fail_proc(p_adv,p_send_str,PROVISION_FAIL_INVALID_FORMAT);
						#endif
						
						return ;
					}
				}else if (p_adv->transBear.bearAck.header.GPCF == TRANS_START &&
				(p_adv->transStart.data[0]== PRO_REC_REQ||p_adv->transStart.data[0]== PRO_REC_GET)){
					LOG_MSG_LIB(TL_LOG_NODE_SDK,0, 0,"invalid op in start state");
					mesh_prov_node_fail_proc(p_adv,p_send_str,PROVISION_FAIL_UNEXPT_PDU);
					return ;
				}
				#if PROV_CASE_ENABLE
				else if (p_adv->transBear.bearAck.header.GPCF == TRANS_START &&(p_adv->transStart.data[0]== PRO_INVITE)){
					LOG_MSG_LIB(TL_LOG_NODE_SDK,0, 0,"invalid invite");	
					mesh_prov_node_fail_proc(p_adv,p_send_str,PROVISION_FAIL_UNEXPT_PDU);
				}
				#endif
			break;			
		case STATE_PRO_START_ACK:
			if(p_adv->transBear.bearAck.header.GPCF == TRANS_START &&
				p_adv->transStart.data[0]== PRO_PUB_KEY){
				if(!mesh_check_pubkey_valid(p_rcv_str->pubkey.pubKeyX)){
				    LOG_MSG_ERR(TL_LOG_PROVISION,0, 0 ,"adv the pubkey value is invalid");
					mesh_prov_node_fail_proc(p_adv,p_send_str,PROVISION_FAIL_UNEXPT_PDU);
					return;
				}
				if(prov_para.prov_err ){
					mesh_prov_node_fail_proc(p_adv,p_send_str,PROVISION_FAIL_UNEXPT_PDU);
					return ;
				}
				SET_TC_FIFO(TSCRIPT_MESH_RX,(u8 *)(&rcv_pb),rcv_pb.transStart.total_len+11);
				LOG_MSG_LIB(TL_LOG_NODE_SDK,(u8 *)&(p_rcv_str->pubkey), 
								sizeof(pro_trans_pubkey),"rcv pubkey cmd ");
				u8 dev_public_key[64];
				get_public_key(dev_public_key);
				#if PROV_AUTH_LEAK_REFLECT_EN
				if(!memcmp(p_rcv_str->pubkey.pubKeyX,dev_public_key,sizeof(dev_public_key))){
					// the pubkey is the same with the master .it will need to recreate.
					init_ecc_key_pair(1);
					get_public_key(dev_public_key);
				}
				#endif
				set_rsp_ack_transnum(p_adv);
				send_rcv_retry_clr();
				memcpy(confirm_input+0x11,p_rcv_str->pubkey.pubKeyX,0x40);
				memcpy(confirm_input+0x11+0x40,dev_public_key,0x40);
				if(prov_oob.start.pubKey == MESH_PUB_KEY_WITHOUT_OOB){
					set_pro_pub_key(p_send_str,dev_public_key,dev_public_key+32);
					mesh_send_provison_data(TRANS_ACK,0,0,0);
					SET_TC_FIFO(TSCRIPT_MESH_TX,(u8 *)(&(pro_adv_pkt.len)),pro_adv_pkt.len+1);
					send_multi_type_data(PRO_PUB_KEY,para_pro);
					#if TESTCASE_FLAG_ENABLE
					u8 tmp_data_len = get_mesh_pro_str_len(PRO_PUB_KEY);
					memcpy(tc_seg_buf, (u8 *)(&(pro_adv_pkt.len)),OFFSETOF(pro_PB_ADV,transStart.data));
					memcpy(tc_seg_buf+OFFSETOF(pro_PB_ADV,transStart.data), para_pro, tmp_data_len);
					SET_TC_FIFO(TSCRIPT_MESH_TX, tc_seg_buf,tmp_data_len+OFFSETOF(pro_PB_ADV,transStart.data));
					#endif
					send_rcv_retry_set(PRO_PUB_KEY,0,1);
					if(prov_oob.start.authMeth == MESH_OUTPUT_OOB){
						mesh_node_out_oob_pub_key_tick = clock_time()|1;
					}
					LOG_MSG_LIB(TL_LOG_NODE_SDK,(u8 *)&(p_send_str->pubkey), 
								sizeof(pro_trans_pubkey),"send pubkey cmd ");
					if(prov_oob.start.authMeth == MESH_NO_OOB || prov_oob.start.authMeth == MESH_STATIC_OOB){
						prov_para.provison_rcv_state = STATE_DEV_PUB_KEY_ACK;     
					}else if (prov_oob.start.authMeth == MESH_OUTPUT_OOB){
						prov_para.provison_rcv_state = STATE_DEV_PUB_KEY_ACK;
					}else if (prov_oob.start.authMeth == MESH_INPUT_OOB){
						prov_para.provison_rcv_state = STATE_DEV_PUB_KEY_INPUT_OOB;
					}        
					prov_para.trans_num_last = prov_para.trans_num;
				}else if (prov_oob.start.pubKey == MESH_PUB_KEY_WITH_OOB){
					mesh_send_provison_data(TRANS_ACK,0,0,0);
					send_rcv_retry_set(PRO_COMMAND_ACK,0,0);
					prov_para.trans_num_last += 2;
					if(prov_oob.start.authMeth == MESH_NO_OOB || prov_oob.start.authMeth == MESH_STATIC_OOB){
						prov_para.provison_rcv_state = STATE_DEV_PUB_KEY_ACK;  
					}else if (prov_oob.start.authMeth == MESH_OUTPUT_OOB){
						prov_para.provison_rcv_state = STATE_DEV_PUB_KEY_ACK;
					}else if (prov_oob.start.authMeth == MESH_INPUT_OOB){
						prov_para.provison_rcv_state = STATE_DEV_PUB_KEY_INPUT_OOB;
					}									
				}
			}
			#if PROV_CASE_ENABLE
			else if(p_adv->transAck.GPCF == TRANS_START && p_adv->transStart.data[0]== PRO_INVITE){
				LOG_MSG_LIB(TL_LOG_NODE_SDK,0, 0,"cmd err proc 1");
				mesh_prov_node_fail_proc(p_adv,p_send_str,PROVISION_FAIL_UNEXPT_PDU);
			}
			#endif
			break;
		case STATE_DEV_PUB_KEY_ACK:
			if(p_adv->transBear.bearAck.header.GPCF == TRANS_START &&
				p_adv->transStart.data[0]== PRO_CONFIRM){
					pro_trans_confirm *p_confirm = &(p_rcv_str->confirm);
					u8 dev_private_key[32];
					LOG_MSG_LIB(TL_LOG_NODE_SDK,(u8 *)&(p_rcv_str->confirm), 
								get_prov_confirm_len(),"rcv confirm cmd ");
					SET_TC_FIFO(TSCRIPT_MESH_RX,(u8 *)(p_adv),p_adv->length+1);
					set_rsp_ack_transnum(p_adv);
					send_rcv_retry_clr();
					// wait for the cmd of the confirm part 
					if(!mesh_node_oob_auth_data(&prov_oob)){
					    LOG_MSG_ERR(TL_LOG_PROVISION,0, 0 ,"the prov_oob is invalid");
						return ;
					}
					if(prov_confirm_check_same_or_not(p_confirm->confirm,dev_confirm)){
						LOG_MSG_ERR(TL_LOG_PROVISION,0, 0 ,"prov_confirm_check fail");
						mesh_prov_node_fail_proc(p_adv,p_send_str,PROVISION_FAIL_CONFIRM_FAIL);
						return ;
					}
					memcpy(pro_confirm,p_confirm->confirm,get_prov_confirm_value_len());
					get_private_key(dev_private_key);
					tn_p256_dhkey_fast(ecdh_secret, dev_private_key, confirm_input+0x11, confirm_input+0x11+0x20);
					mesh_sec_prov_confirmation_send_confirm_state();
					set_pro_confirm(p_send_str,dev_confirm,get_prov_confirm_value_len());
					mesh_send_provison_data(TRANS_ACK,0,0,0);
					SET_TC_FIFO(TSCRIPT_MESH_TX,(u8 *)(&(pro_adv_pkt.len)),pro_adv_pkt.len+1);
					send_multi_type_data(PRO_CONFIRM,para_pro);
					SET_TC_FIFO(TSCRIPT_MESH_TX,(u8 *)(&(pro_adv_pkt.len)),pro_adv_pkt.len+1);					
					send_rcv_retry_set(PRO_CONFIRM,0,1);
					LOG_MSG_LIB(TL_LOG_NODE_SDK,(u8 *)&(p_send_str->confirm), 
								get_prov_confirm_len(),"send confirm cmd ");
					prov_para.provison_rcv_state = STATE_DEV_CONFIRM;
					prov_para.trans_num_last = prov_para.trans_num;
					
			}
			#if PROV_CASE_ENABLE
			else if (p_adv->transBear.bearAck.header.GPCF == TRANS_START &&
						(p_adv->transStart.data[0]== PRO_INVITE ||p_adv->transStart.data[0]== PRO_PUB_KEY)){
					LOG_MSG_LIB(TL_LOG_NODE_SDK,0, 
								0,"invalid invite");	
					mesh_prov_node_fail_proc(p_adv,p_send_str,PROVISION_FAIL_UNEXPT_PDU);
				}	
			#endif
			break;
		case STATE_DEV_CONFIRM:
			if(p_adv->transBear.bearAck.header.GPCF == TRANS_START &&
				p_adv->transStart.data[0]== PRO_RANDOM){
				if(prov_para.prov_err ){
					mesh_prov_node_fail_proc(p_adv,p_send_str,PROVISION_FAIL_UNEXPT_PDU);
					return ;
				}
				SET_TC_FIFO(TSCRIPT_MESH_RX,(u8 *)(p_adv),p_adv->length+1);
				LOG_MSG_LIB(TL_LOG_NODE_SDK,(u8 *)&(p_rcv_str->random), 
								get_prov_random_len(),"rcv random cmd ");
				set_rsp_ack_transnum(p_adv);
				send_rcv_retry_clr();
				memcpy(pro_random,p_rcv_str->random.random,get_prov_random_value_len());
				
				// use the provision random to calculate the provision confirm 
				u8 pro_confirm_tmp[32];
				mesh_sec_prov_confirmation_check_confirm_state(pro_confirm_tmp);
				if(prov_confirm_check_right_or_not(pro_confirm,pro_confirm_tmp)){
				    LOG_MSG_ERR(TL_LOG_PROVISION,0, 0 ,"provision confirm fail");
					mesh_prov_node_fail_proc(p_adv,p_send_str,PROVISION_FAIL_CONFIRM_FAIL);
					LOG_MSG_INFO(TL_LOG_NODE_SDK,0, 0 ,"confirm fail");
					return;
				}
				
				set_pro_random(p_send_str,dev_random,get_prov_random_value_len());	
				mesh_send_provison_data(TRANS_ACK,0,0,0);
				SET_TC_FIFO(TSCRIPT_MESH_TX,(u8 *)(&(pro_adv_pkt.len)),pro_adv_pkt.len+1);
				send_multi_type_data(PRO_RANDOM,para_pro);
				SET_TC_FIFO(TSCRIPT_MESH_TX,(u8 *)(&(pro_adv_pkt.len)),pro_adv_pkt.len+1);
				send_rcv_retry_set(PRO_RANDOM,0,1);			
				LOG_MSG_LIB(TL_LOG_NODE_SDK,(u8 *)&(p_send_str->random), 
								get_prov_random_len(),"send random cmd ");
				prov_para.provison_rcv_state = STATE_DEV_RANDOM;
				prov_para.trans_num_last = prov_para.trans_num;
			}
			#if PROV_CASE_ENABLE
			else if (p_adv->transBear.bearAck.header.GPCF == TRANS_START &&
						(p_adv->transStart.data[0]== PRO_INVITE ||p_adv->transStart.data[0]== PRO_PUB_KEY)){
					LOG_MSG_LIB(TL_LOG_NODE_SDK,0, 0,"invalid invite");	
					mesh_prov_node_fail_proc(p_adv,p_send_str,PROVISION_FAIL_UNEXPT_PDU);
				}	
			#endif
			break;
		case STATE_DEV_RANDOM:
			if(p_adv->transBear.bearAck.header.GPCF == TRANS_START &&
				p_adv->transStart.data[0]== PRO_DATA){
				LOG_MSG_LIB(TL_LOG_NODE_SDK,(u8 *)&(p_rcv_str->data), 
								sizeof(pro_trans_data),"rcv provision data cmd ");
				u8 *p_prov_net;
				u8 dev_key[16];
				u8 prov_salt[16];
				u8 dev_session_key[16];
				u8 dev_session_nonce[16];
				if(prov_para.prov_err ){
					mesh_prov_node_fail_proc(p_adv,p_send_str,PROVISION_FAIL_UNEXPT_PDU);
					return ;
				}
				mesh_sec_prov_salt_fun(prov_salt,confirm_input,pro_random,dev_random,is_prov_oob_hmac_sha256());
				mesh_sec_prov_session_key_fun (dev_session_key, dev_session_nonce, confirm_input, 145, ecdh_secret, pro_random, dev_random,is_prov_oob_hmac_sha256());
				//calculate the dev_key part 
				mesh_sec_dev_key(dev_key,prov_salt,ecdh_secret);
				set_dev_key(dev_key);
				LOG_MSG_LIB(TL_LOG_NODE_SDK,dev_key, 16,"device key ");
				p_prov_net = (p_adv->transStart.data)+1;				
				set_rsp_ack_transnum(p_adv);
				send_rcv_retry_clr();
				// trans data 
				int err = mesh_prov_sec_msg_dec (dev_session_key, dev_session_nonce+3, p_prov_net, 33, 8);
				SET_TC_FIFO(TSCRIPT_MESH_RX,(u8 *)(&rcv_pb),rcv_pb.transStart.total_len+11);
				// need to be checked 
				if(err){
					LOG_MSG_LIB(TL_LOG_NODE_SDK,0, 
								0,"decrypt fail");	
					mesh_prov_node_fail_proc(p_adv,p_send_str,PROVISION_FAIL_DECRYT_FAIL);
					return ;
				}
				LOG_MSG_LIB(TL_LOG_NODE_SDK,p_prov_net, 
								25,"the provision net info is  ");
				memcpy(&provision_mag.pro_net_info,p_prov_net,sizeof(provison_net_info_str));
				mesh_send_provison_data(TRANS_ACK,0,0,0);
				
				send_multi_type_data(PRO_COMPLETE,para_pro);
				SET_TC_FIFO(TSCRIPT_MESH_TX,(u8 *)(&(pro_adv_pkt.len)),pro_adv_pkt.len+1);
				send_rcv_retry_set(PRO_COMPLETE,0,1);
				prov_para.trans_num_last = prov_para.trans_num;
				prov_para.provison_rcv_state = STATE_PRO_COMPLETE;
				if(0 == err){
				    tick_check_complete = clock_time()|1;
				}
				#if __PROJECT_MESH_SWITCH__
				mesh_key_node_identity_set_prov_set();
				#endif
				cache_init(ADR_ALL_NODES);
			 }	
			break;
		default:             
			break;
	}
	#endif 
	return ;
}

u8 adv_provision_state_dispatch(pro_PB_ADV * p_adv)
{
	mesh_provision_par_handle(&provision_mag.pro_net_info);
	prov_para.provison_rcv_state = STATE_PRO_SUC;
	SET_TC_FIFO(TSCRIPT_MESH_RX,(u8 *)(p_adv),p_adv->length+1);
	send_rcv_retry_clr();
	SET_RESULT_TESTCASE(0,0);
	return 1;
}

u8 wait_and_check_complete_state()
{
	if( provision_mag.gatt_mode == GATT_PROXY_MODE ){
		tick_check_complete =0;
	}
	if(tick_check_complete && clock_time_exceed(tick_check_complete,4000*1000)){
		tick_check_complete =0;
		mesh_provision_par_handle(&provision_mag.pro_net_info);
		prov_para.provison_rcv_state = STATE_PRO_SUC;
		send_rcv_retry_clr();
		LOG_MSG_LIB(TL_LOG_NODE_SDK,0, 
								0,"adv provision part suc! ");
		mesh_node_prov_event_callback(EVENT_MESH_NODE_RC_LINK_SUC);
	}
	return 1;
}

u8 get_provision_state()
{
	#if TESTCASE_FLAG_ENABLE
	if((CFGCL_CFG_KR_PTS_1 & 0xFFFF0000) == (tc_par.tc_id & 0xFFFF0000)){
		return 1;	// confirm later
	}
	#endif
	if(prov_para.provison_send_state || prov_para.provison_rcv_state){
    	if( prov_para.provison_send_state == STATE_PRO_SUC  || 
    		prov_para.provison_rcv_state == STATE_PRO_SUC){
    		return STATE_DEV_PROVED;
    	}else{
    		return STATE_DEV_PROVING;
    	}
	 }
	if(provision_mag.gatt_mode == GATT_PROXY_MODE){
		return STATE_DEV_PROVED;
	}else{
		return STATE_DEV_UNPROV;
	}
}

/**
 * @brief       This function get whether the node is at provision success state.
 * @return      1: means node is at provision success state. 2: at unprovision state.
 * @note        call this function must be after mesh_init all().
 */
u8 is_provision_success()   // rename is_proved_state(), proved means "prov + ed"
{
    #if WIN32
    return win32_proved_state();
	#elif (__PROJECT_MESH_PRO__ || __PROJECT_MESH_GW_NODE__)
	return (provision_mag.gatt_mode == GATT_PROXY_MODE);
    #else
    return (get_provision_state() == STATE_DEV_PROVED);
    #endif
}

/**
 * @brief       This function get whether the node is at provision working state.
 * @return      1: yes.  0: no.
 * @note        call this function must be after mesh_init all().
 */
u8 is_provision_working()
{
    #if WIN32
    return win32_prov_working();
    #else
	return(get_provision_state() == STATE_DEV_PROVING);
	#endif
}

static u8 gateway_connect_filter[6];
void set_gateway_adv_filter(u8 *p_mac)
{
	memcpy(gateway_connect_filter,p_mac,sizeof(gateway_connect_filter));
}

void gateway_adv_filter_init()
{
	memset(gateway_connect_filter,0,sizeof(gateway_connect_filter));
}

void mesh_pro_rc_beacon_dispatch(pro_PB_ADV *p_adv,u8 *p_mac){
	#if PAIR_PROVISION_ENABLE
	void pair_prov_rx_unprov_beacon_handle(beacon_data_pk *p_beacon, u8 *mac);
	pair_prov_rx_unprov_beacon_handle((beacon_data_pk *)p_adv, p_mac);
	#endif
	
	//provision_mag.provison_send_state = LINK_ESTABLISHED;
	if(	(prov_para.provison_send_state != STATE_PRO_SUC &&
		prov_para.provison_send_state != LINK_UNPROVISION_STATE)||
		prov_para.key_bind_lock){
		return ;
	}

	beacon_data_pk *p_beacon = (beacon_data_pk *)p_adv;
	if(p_beacon->beacon_type == SECURE_BEACON){
		return ;
	}
	
#if MD_REMOTE_PROV
    // remote prov raw pkt  
    remote_prov_report_raw_pkt_cb((u8 *)p_adv);
#endif

	#if !WIN32&&(__PROJECT_MESH_PRO__||__PROJECT_MESH_GW_NODE__)
	//SET_TC_FIFO(TSCRIPT_MESH_RX,(u8 *)&(p_adv->length),sizeof(beacon_data_pk));

	
	
	// if the stop flag ==1 ,can not send link open cmd part 
	if(provision_mag.pro_stop_flag){
		return ;
	}
	// upload the adv part and the device uuid part 
	gateway_upload_mac_address(p_mac,(u8 *)p_adv);
	// dispatch the filter part ,if the filter is the same ,the gateway can send the link open cmd 
#if SMART_PROVISION_ENABLE
	if(is_smart_provision_running()){
		if(!is_existed_in_smart_scan_list(p_beacon->device_uuid, 1) || (SMART_CONFIG_PROVING != mesh_smart_provision_st_get() || is_provision_working())){
			return ;
		}
	}
	else
#endif
	if(memcmp(gateway_connect_filter,p_mac,sizeof(gateway_connect_filter))){
		return ;
	}
	// whether the provisioner interval para set or not 
	if(!is_provision_success()){
		return ;
	}
	// whether the provisionee para set or not 
	if(!get_gateway_provisison_sts()){
		return ;
	}
	mesh_provision_para_reset();
	memcpy(prov_link_uuid,p_beacon->device_uuid,sizeof(p_beacon->device_uuid));
    send_rcv_retry_set(PRO_BEARS_CTL,LINK_OPEN,0); //test case use start_tick in mesh_send_provison_data
    prov_para.link_id = clock_time();
	prov_para.link_id_filter =1;
	mesh_send_provison_data(BEARS_CTL,LINK_OPEN,prov_link_uuid,sizeof(prov_link_uuid));

	u16 oob_info_data = p_beacon->oob_info[0]|(p_beacon->oob_info[0]<<8);
	if(oob_info_data &(BIT(OOB_PROV_RECORD)|BIT(OOB_CERT_BASE))){
		prov_para.cert_base_en =1;
		LOG_MSG_INFO(TL_LOG_PROVISION, 0, 0,"the oob is %04x",oob_info_data);
	}else{
		prov_para.cert_base_en =0;
	}

	#if GATEWAY_ENABLE
	gateway_upload_prov_link_open(prov_link_uuid,sizeof(prov_link_uuid));
	#endif
	SET_TC_FIFO(TSCRIPT_MESH_TX,(u8 *)(&(pro_adv_pkt.len)),pro_adv_pkt.len+1);
	LOG_MSG_INFO(TL_LOG_PROVISION, p_mac, 6,"send link open,mac:");
	prov_para.provison_send_state = LINK_ESTABLISHED_ACK;	
	#endif
	return;
}
// use to set the auth enable part 

void check_oob_out_timeout()
{
#if (__PROJECT_MESH_PRO__ || __PROJECT_MESH_GW_NODE__)
	if((prov_oob.oob_out_tick && clock_time_exceed(prov_oob.oob_out_tick ,20*1000*1000))&&
		prov_auth_en_flag){
		prov_oob.oob_out_tick =0;
		LOG_MSG_INFO(TL_LOG_PROVISION, 0, 0,"output oob tick timeout %d",prov_auth_val);
		prov_auth_en_flag=0;
		send_rcv_retry_clr();
		mesh_prov_oob_auth_data(&prov_oob);
		send_confirm_no_pubkey_cmd();
	}
	return ;
#endif
	
}


u32 mesh_provision_end_tick =0;
void mesh_prov_end_set_tick()
{
    mesh_provision_end_tick = clock_time()|1;
}

u8  mesh_loop_provision_end_process()
{
	u32 prov_retry_time = 2*1000*1000;
	
	#if MD_REMOTE_PROV
	if(mesh_pr_sts_work_or_not()){
		prov_retry_time = 6500*1000;
	}
	#endif
	
	if(mesh_provision_end_tick && clock_time_exceed(mesh_provision_end_tick,prov_retry_time)){
	    #if MD_REMOTE_PROV
        mesh_rp_server_prov_end_cb();
        #endif
	    send_rcv_retry_clr();
		mesh_provision_end_tick = 0;
		prov_para.provison_send_state = STATE_PRO_SUC;
		#if MD_REMOTE_PROV
		if(!mesh_pr_sts_work_or_not())
		#endif
		{
			#if GATEWAY_ENABLE
			u16 adr_store ;
			adr_store = provision_mag.unicast_adr_last;
			#endif
			provision_mag.unicast_adr_last += prov_para.ele_cnt;
			prov_para.link_id_filter =0;
			provision_mag_cfg_s_store();
			mesh_misc_store();
			#if GATEWAY_ENABLE
			gateway_upload_node_info(adr_store);
			// the gateway need the device's mac address and the uuid part 
	        gateway_upload_provision_suc_event(1,adr_store,
	                        gateway_connect_filter,prov_link_uuid);// update the suc event
			set_gateway_provision_para_init();// init the para part 
			#endif
		}
	}
	return 1;
}

u32 link_close_start_tick =0;
void mesh_loop_check_link_close_flag()
{
	if(link_close_start_tick&& clock_time_exceed(link_close_start_tick,6*1000*1000)){
		{
			mesh_terminate_provision_link_reset(LINK_CLOSE_FAIL,0);
			link_close_start_tick = 0;
		}
	}
}


void mesh_adv_prov_link_open_ack(pro_PB_ADV *p_adv)
{
    memset(&prov_oob,0,sizeof(prov_oob));
	SET_TC_FIFO(TSCRIPT_MESH_RX,(u8 *)(p_adv),p_adv->length+1);
	LOG_MSG_INFO(TL_LOG_PROVISION, 0, 0,"rcv link open ack");
	set_rsp_ack_transnum(p_adv);    
}

void mesh_adv_prov_rec_get(mesh_pro_data_t *p_send_str)
{
    set_pro_rec_get(p_send_str);
	send_multi_type_data(PRO_REC_GET,para_pro);
	SET_TC_FIFO(TSCRIPT_MESH_TX,(u8 *)(&(pro_adv_pkt.len)),pro_adv_pkt.len+1);
	LOG_MSG_INFO(TL_LOG_PROVISION, 0, 0,"send rec get");
	send_rcv_retry_set(PRO_REC_GET,0,0);
	prov_para.trans_num_last = prov_para.trans_num;
}

void mesh_adv_prov_rec_req(pro_PB_ADV *p_adv,mesh_pro_data_t *p_send_str,mesh_rec_mag_rec_t  *p_rec_mag)
{
	//mesh_rec_mag_rec_t  *p_rec_mag = &rec_mag[idx];
	set_rsp_ack_transnum(p_adv);
	send_rcv_retry_clr();
	mesh_send_provison_data(TRANS_ACK,0,0,0);
	set_pro_rec_req(p_send_str,p_rec_mag);
	send_multi_type_data(PRO_REC_REQ,para_pro);
	LOG_MSG_INFO(TL_LOG_PROVISION, 0, 0,"send rec req");
	send_rcv_retry_set(PRO_REC_REQ,0,1);
	prov_para.trans_num_last = prov_para.trans_num;
	prov_para.provison_send_state = STATE_PRO_REC_REQ_ACK;
}

void mesh_adv_prov_send_invite(mesh_pro_data_t *p_send_str,u8 dura)
{
    set_pro_invite(p_send_str,dura);
	send_multi_type_data(PRO_INVITE,para_pro);
	SET_TC_FIFO(TSCRIPT_MESH_TX,(u8 *)(&(pro_adv_pkt.len)),pro_adv_pkt.len+1);
	LOG_MSG_INFO(TL_LOG_PROVISION, 0, 0,"send invite");
	send_rcv_retry_set(PRO_INVITE,0,0);
	prov_para.trans_num_last = prov_para.trans_num;
}

void mesh_adv_prov_send_invite_certify(pro_PB_ADV *p_adv,mesh_pro_data_t *p_send_str,u8 dura)
{
	set_rsp_ack_transnum(p_adv);
	send_rcv_retry_clr();
	mesh_send_provison_data(TRANS_ACK,0,0,0);
	#if VALID_CERT_ENABLE_CHECK
	send_rcv_retry_set(PRO_COMMAND_ACK,0,1);
	#else
	set_pro_invite(p_send_str,dura);
	send_multi_type_data(PRO_INVITE,para_pro);
	SET_TC_FIFO(TSCRIPT_MESH_TX,(u8 *)(&(pro_adv_pkt.len)),pro_adv_pkt.len+1);
	LOG_MSG_INFO(TL_LOG_PROVISION, 0, 0,"send invite");
	send_rcv_retry_set(PRO_INVITE,0,1);
	#endif
	prov_para.trans_num_last = prov_para.trans_num;
}


void mesh_adv_prov_invite_ack(pro_PB_ADV *p_adv)
{
    SET_TC_FIFO(TSCRIPT_MESH_RX,(u8 *)p_adv,p_adv->length+1); 
	send_rcv_retry_clr();
}

void mesh_adv_prov_capa_rcv(pro_PB_ADV *p_adv)
{
    SET_TC_FIFO(TSCRIPT_MESH_RX,(u8 *)p_adv,p_adv->length+1);
	LOG_MSG_INFO(TL_LOG_PROVISION, 0, 0,"rcv capabilities");
	set_rsp_ack_transnum(p_adv);
	send_rcv_retry_clr();
	mesh_send_provison_data(TRANS_ACK,0,0,0);
	SET_TC_FIFO(TSCRIPT_MESH_TX,(u8 *)(&(pro_adv_pkt.len)),pro_adv_pkt.len+1);    
}

void mesh_adv_prov_send_start_cmd(mesh_pro_data_t *p_send_str,pro_trans_start *p_start)
{
    set_pro_start_simple(p_send_str,p_start);
	send_multi_type_data(PRO_START,para_pro);
	SET_TC_FIFO(TSCRIPT_MESH_TX,(u8 *)(&(pro_adv_pkt.len)),pro_adv_pkt.len+1);
	LOG_MSG_INFO(TL_LOG_PROVISION, 0, 0,"send prov start");
	send_rcv_retry_set(PRO_START,0,1);
	prov_para.trans_num_last = prov_para.trans_num;
}

void mesh_adv_prov_start_ack(pro_PB_ADV *p_adv)
{
	SET_TC_FIFO(TSCRIPT_MESH_RX,(u8 *)(p_adv),p_adv->length+1);
	send_rcv_retry_clr();    
}

void mesh_adv_prov_pubkey_send(mesh_pro_data_t *p_send_str,u8 *ppk)
{
	set_pro_pub_key(p_send_str,ppk,ppk+32);
	send_multi_type_data(PRO_PUB_KEY,para_pro);
	send_rcv_retry_set(PRO_PUB_KEY,0,0); 
	LOG_MSG_INFO(TL_LOG_PROVISION, 0, 0,"send pub_key:");
	prov_para.trans_num_last = prov_para.trans_num;
}

void mesh_adv_prov_pubkey_rsp(pro_PB_ADV *p_adv)
{
    send_rcv_retry_clr();
    set_rsp_ack_transnum(p_adv);
}

void mesh_adv_prov_confirm_cmd(mesh_pro_data_t *p_send_str,u8 *p_confirm)
{
    set_pro_confirm(p_send_str,p_confirm,get_prov_confirm_value_len());
    mesh_send_provison_data(TRANS_ACK,0,0,0);
    send_multi_type_data(PRO_CONFIRM,para_pro);
    SET_TC_FIFO(TSCRIPT_MESH_TX,(u8 *)(&(pro_adv_pkt.len)),pro_adv_pkt.len+1);  
	LOG_MSG_INFO(TL_LOG_PROVISION, 0, 0,"send prov confirm");
    send_rcv_retry_set(PRO_CONFIRM,0,1);
    prov_para.trans_num_last = prov_para.trans_num;
}

void mesh_adv_prov_confirm_ack(pro_PB_ADV *p_adv)
{
    SET_TC_FIFO(TSCRIPT_MESH_RX,(u8 *)(p_adv),p_adv->length+1);
	send_rcv_retry_clr();
}

void mesh_adv_prov_confirm_rsp(pro_PB_ADV *p_adv)
{
	SET_TC_FIFO(TSCRIPT_MESH_RX,(u8 *)p_adv,p_adv->length+1);
	set_rsp_ack_transnum(p_adv);
	send_rcv_retry_clr();
	mesh_send_provison_data(TRANS_ACK,0,0,0);
	SET_TC_FIFO(TSCRIPT_MESH_TX,(u8 *)(&(pro_adv_pkt.len)),pro_adv_pkt.len+1);    
}

void mesh_adv_prov_random_cmd(mesh_pro_data_t *p_send_str,u8 *p_random)
{
	set_pro_random(p_send_str,p_random,get_prov_random_value_len());
	send_multi_type_data(PRO_RANDOM,para_pro);
	SET_TC_FIFO(TSCRIPT_MESH_TX,(u8 *)(&(pro_adv_pkt.len)),pro_adv_pkt.len+1);
	LOG_MSG_INFO(TL_LOG_PROVISION, 0, 0,"send prov random:");
	send_rcv_retry_set(PRO_RANDOM,0,1);
	prov_para.trans_num_last = prov_para.trans_num;
}

void mesh_adv_prov_random_ack_cmd(pro_PB_ADV *p_adv)
{
	SET_TC_FIFO(TSCRIPT_MESH_RX,(u8 *)(p_adv),p_adv->length+1);
	send_rcv_retry_clr();    
}

void mesh_adv_prov_random_rsp(pro_PB_ADV *p_adv)
{
	set_rsp_ack_transnum(p_adv);
	send_rcv_retry_clr();

}

void mesh_adv_prov_data_send(mesh_pro_data_t *p_send_str,u8 *p_data)
{
    set_pro_data(p_send_str,p_data,p_data+25);
	mesh_send_provison_data(TRANS_ACK,0,0,0);
	SET_TC_FIFO(TSCRIPT_MESH_TX,(u8 *)(&(pro_adv_pkt.len)),pro_adv_pkt.len+1);
	send_multi_type_data(PRO_DATA,para_pro);
	send_rcv_retry_set(PRO_DATA,0,1);  
	prov_para.trans_num_last = prov_para.trans_num;
}

void mesh_adv_prov_data_ack(pro_PB_ADV *p_adv)
{
    SET_TC_FIFO(TSCRIPT_MESH_RX,(u8 *)(p_adv),p_adv->length+1);
	send_rcv_retry_clr();
}

void mesh_rp_adv_prov_complete_rsp(pro_PB_ADV *p_adv)
{
	set_rsp_ack_transnum(p_adv);
	mesh_send_provison_data(TRANS_ACK,0,0,0);
	send_rcv_retry_set(PRO_COMMAND_ACK,0,0);
}

void mesh_adv_prov_complete_rsp(pro_PB_ADV *p_adv)
{
    SET_TC_FIFO(TSCRIPT_MESH_RX,(u8 *)p_adv,p_adv->length+1);               
    set_rsp_ack_transnum(p_adv);
    send_rcv_retry_clr();
	#if 0 //only send tranack 
    mesh_send_provison_data(TRANS_ACK,0,0,0); 
    send_rcv_retry_set(PRO_COMMAND_ACK,0,0);
    #else
    mesh_terminate_provision_link_reset(LINK_CLOSE_SUCC,1);
	#endif
    SET_RESULT_TESTCASE(0,0);
}
void mesh_adv_prov_link_close()
{
#if GATEWAY_ENABLE
    set_gateway_provision_para_init();
#else
    mesh_provision_para_reset();
#endif
}	

#if CERTIFY_BASE_ENABLE
mesh_rec_mag_rec_t rec_mag[REC_ID_MAX_NUM];
u8 rec_mag_cnt =0;
u8 rec_mag_idx =0;

u8 get_rec_idx_by_id(u16 id)
{
	int i=0;
	for(i=0;i<REC_ID_MAX_NUM;i++){
		if(rec_mag[i].rec_id == id){
			return i;
		}
	}
	return REC_ID_MAX_NUM;
}
#endif

#if (__PROJECT_MESH_PRO__ || GATEWAY_ENABLE)
void mesh_proc_prov_fail(pro_PB_ADV *p_adv ,u8 err_code)
{
	mesh_pro_data_t *p_send_str = (mesh_pro_data_t *)(para_pro);
	set_rsp_ack_transnum(p_adv);
	send_rcv_retry_clr();
	mesh_send_provison_data(TRANS_ACK,0,0,0);
	set_pro_fail(p_send_str,err_code);
	send_multi_type_data(PRO_FAIL,para_pro);
	send_rcv_retry_set(PRO_FAIL,0,1);
	prov_para.trans_num_last = prov_para.trans_num;
	prov_para.provison_send_state = STATE_PRO_FAILED_ACK;
	mesh_node_prov_event_callback(EVENT_MESH_NODE_RC_LINK_FAIL_CODE);
}
#endif

/**
 * @brief       This function server the provisioner to provision the new device by PB_ADV.
 * @param[in]   p_adv	- pointer of provisioning PDU.
 * @return      none
 * @note        
 */
void mesh_pro_rc_adv_dispatch(pro_PB_ADV *p_adv){
	//provision function 
#if (__PROJECT_MESH_PRO__ || __PROJECT_MESH_GW_NODE__)

	mesh_pro_data_t *p_send_str = (mesh_pro_data_t *)(para_pro);
	mesh_pro_data_t *p_rcv_str = (mesh_pro_data_t *)(p_adv->transStart.data);
	// if the rcv pkt is a adv pkt ,we should remember the transaction num ,then rsp with the same transaction num 
	if(p_adv->transBear.bearOpen.header.GPCF == BEARS_CTL &&
			   p_adv->transBear.bearAck.header.BearCtl == LINK_CLOSE){            
		//reset the link id and others 				
	    #if GATEWAY_ENABLE
        gateway_upload_prov_link_cls(p_adv->pb_pdu,sizeof(bear_close_link));
	    #endif
		mesh_adv_prov_link_close();
		LOG_MSG_ERR(TL_LOG_PROVISION,0, 0 ,"prov rcv link close");	
	}
	else if((prov_para.cert_ack_hold == 0) &&(p_adv->transBear.bearAck.header.GPCF == TRANS_START) && (p_adv->transStart.data[0]> PRO_REC_LIST)){
	#if TESTCASE_FLAG_ENABLE
		mesh_proc_prov_fail(p_adv,PROVISION_FAIL_UNEXPT_PDU);
	#endif
	}
	else if((prov_para.cert_ack_hold == 0) &&(p_adv->transBear.bearAck.header.GPCF == TRANS_START) && (p_adv->transStart.data[0]== PRO_CAPABLI)){
		u8 rsp_ack_transnum = prov_para.rsp_ack_transnum;
		set_rsp_ack_transnum(p_adv);
	    mesh_send_provison_data(TRANS_ACK,0,0,0); 
		prov_para.rsp_ack_transnum = rsp_ack_transnum;
	}
	else if((0 == link_close_end_tick) && p_adv->transBear.bearAck.header.GPCF == TRANS_START && p_adv->transStart.data[0]== PRO_FAIL){
		#if GATEWAY_ENABLE
		gateway_upload_prov_cmd((u8 *)p_rcv_str,PRO_FAIL);
		#endif
		mesh_terminate_provision_link_reset(LINK_CLOSE_FAIL,0);
		return;
	}
	
	switch(prov_para.provison_send_state){
		case LINK_ESTABLISHED_ACK:
			if(p_adv->transBear.bearAck.header.GPCF == BEARS_CTL &&
				p_adv->transBear.bearAck.header.BearCtl == LINK_ACK){
					// if receive the establish ack 
					// init the prov_oob parameter
					LOG_MSG_INFO(TL_LOG_PROVISION, 0, 0,"rcv link open ack ");
					mesh_adv_prov_link_open_ack(p_adv);
					if(prov_para.cert_base_en ){
						mesh_adv_prov_rec_get(p_send_str);
						#if GATEWAY_ENABLE
						gateway_upload_prov_cmd((u8 *)p_send_str,PRO_REC_GET);
						#endif
						prov_para.provison_send_state = STATE_PRO_REC_REQ;
					}else{
						mesh_adv_prov_send_invite(p_send_str,0);
						#if GATEWAY_ENABLE
						gateway_upload_prov_cmd((u8 *)p_send_str,PRO_INVITE);
						#endif
						confirm_input[0] = p_send_str->invite.attentionDura;	
						prov_para.provison_send_state = STATE_DEV_CAPA_ACK;					
						mesh_node_prov_event_callback(EVENT_MESH_PRO_RC_LINK_START);
					}	
				}
			break;
		#if CERTIFY_BASE_ENABLE
		case STATE_PRO_REC_REQ:
			if(p_adv->transBear.bearAck.header.GPCF == TRANS_START){
				u16 rcv_len = p_adv->transStart.total_len;
				if(p_adv->transStart.data[0]== PRO_REC_LIST){
					prov_para.cert_ack_hold =1;
					// need to send the req get
					pro_trans_record_list *p_rec_list = &(p_rcv_str->rec_list);
					u8 rec_cnt = (rcv_len -3)/2;
					for(int i=0;i<rec_cnt;i++){
						endianness_swap_u16((u8*)(p_rec_list->rec_id+i));
					}
					LOG_MSG_INFO(TL_LOG_PROVISION, (u8*)p_rec_list, sizeof(pro_trans_record_list),"rcv rec list cnt is %d,total rcv len is %d",rec_cnt,rcv_len);
					for(u8 i=0;i<rec_cnt;i++){
						rec_mag[i].rec_id = p_rec_list->rec_id[i];
						rec_mag[i].frag_size = FRAG_MAX_SIZE;
						rec_mag[i].offset = 0;
						rec_mag[i].total_len = 0;
					}
					rec_mag_idx = 0;
					rec_mag_cnt = rec_cnt;
					// send the first rec req cmd 
					mesh_adv_prov_rec_req(p_adv,p_send_str,&rec_mag[0]);
					
				}else if(p_adv->transStart.data[0]== PRO_REC_RSP){
					// nee to send rsp part
					u8 rec_data_len = rcv_len - 8;
					pro_trans_record_rsp *p_rec_rsp = &(p_rcv_str->rec_rsp);
					endianness_swap_u16((u8 *)(&p_rec_rsp->rec_id));
					endianness_swap_u16((u8 *)(&p_rec_rsp->frag_off));
					endianness_swap_u16((u8 *)(&p_rec_rsp->total_len));
					// use the rec_id to get the idx 
					LOG_MSG_INFO(TL_LOG_NODE_SDK,0, 0,"rec_id is %x,frag_off is %x,total_len is %x",
					p_rec_rsp->rec_id,p_rec_rsp->frag_off,p_rec_rsp->total_len);
					u8 idx = get_rec_idx_by_id(p_rec_rsp->rec_id);
					if(idx == REC_ID_MAX_NUM){
						break;
					}
					if(rec_data_len>rec_mag[idx].frag_size){
						break;// invalid length 
					}
					LOG_MSG_INFO(TL_LOG_NODE_SDK,0, 0,"rec idx is  %x,rcv data len is %x",idx,rec_data_len);
					rec_mag[idx].total_len = p_rec_rsp->total_len;
					rec_mag[idx].offset = p_rec_rsp->frag_off+rec_data_len;
					
					if(rec_mag[idx].offset == rec_mag[idx].total_len){
						// move to next rec id .
						rec_mag_idx++;
						rec_mag[idx].offset =0;
						LOG_MSG_INFO(TL_LOG_PROVISION, 0, 0,"rcv the whole rec id %d",rec_mag_idx);
						if(rec_mag_idx == rec_mag_cnt){
							// need to send invite cmd 
							LOG_MSG_INFO(TL_LOG_PROVISION, 0, 0,"send invite cmd");
							mesh_adv_prov_send_invite_certify(p_adv,p_send_str,0);
							prov_para.cert_ack_hold =0;
							#if GATEWAY_ENABLE
							gateway_upload_prov_cmd((u8 *)p_send_str,PRO_INVITE);
							#endif
							confirm_input[0] = p_send_str->invite.attentionDura;	
							prov_para.provison_send_state = STATE_DEV_CAPA_ACK;					
							mesh_node_prov_event_callback(EVENT_MESH_PRO_RC_LINK_START);
						}else{
							// start to read the next req get 
							mesh_adv_prov_rec_req(p_adv,p_send_str,&rec_mag[rec_mag_idx]);
						}
					}else{
						// start to read the next req get 
						mesh_adv_prov_rec_req(p_adv,p_send_str,&rec_mag[rec_mag_idx]);
					}
				}
			}
			break;
		case STATE_PRO_REC_REQ_ACK:
			if(p_adv->transAck.GPCF == TRANS_ACK&& p_adv->trans_num >= prov_para.trans_num_last){
				LOG_MSG_INFO(TL_LOG_NODE_SDK,0, 
								0,"RCV STATE_PRO_REC_REQ_ACK cmd ");
				prov_para.provison_send_state = STATE_PRO_REC_REQ;
			}
			break;
		#endif
		case STATE_DEV_CAPA_ACK:
			//wait for the capa and then rsp the ack,and then send the start cmd 
			if(p_adv->transBear.bearAck.header.GPCF == TRANS_START &&
				p_adv->transStart.data[0]== PRO_CAPABLI){
				    // dispatch the the capbility part 
				    #if GATEWAY_ENABLE
					gateway_upload_prov_rsp_cmd((u8 *)p_rcv_str,PRO_CAPABLI);
					#endif
	                memcpy(confirm_input+1,&(p_rcv_str->capa.ele_num),11);
	                prov_para.ele_cnt = p_rcv_str->capa.ele_num;
	                swap_mesh_pro_capa((mesh_pro_data_t *)p_adv->transStart.data);// swap the endianness for the capa data
					mesh_adv_prov_capa_rcv(p_adv);
					// get the capability para
	                memcpy(&prov_oob.capa , &p_rcv_str->capa,sizeof(p_rcv_str->capa));
	                get_pubkey_oob_info_by_capa(&prov_oob);
	                set_start_para_by_capa(&prov_oob);
					mesh_adv_prov_send_start_cmd(p_send_str,&(prov_oob.start));
					#if PROV_AUTH_LEAK_RECREATE_KEY_EN
					init_ecc_key_pair(1);
					#endif
					#if GATEWAY_ENABLE
					gateway_upload_prov_cmd((u8 *)p_send_str,PRO_START);
					#endif
					memcpy(confirm_input+12,&(p_send_str->start.algorithms),5);
					prov_para.provison_send_state = STATE_PRO_START_ACK;
				}
			break;
		case STATE_PRO_START_ACK:
			//ack and then send the public key cmd 
			if(p_adv->transAck.GPCF == TRANS_ACK&& p_adv->trans_num >= prov_para.trans_num_last){
					u8 prov_public_key[64];
					get_public_key(prov_public_key);
					mesh_adv_prov_start_ack(p_adv);
					mesh_adv_prov_pubkey_send(p_send_str,prov_public_key);
					#if GATEWAY_ENABLE
					gateway_upload_prov_cmd((u8 *)p_send_str,PRO_PUB_KEY);
					#endif
					#if TESTCASE_FLAG_ENABLE
					u8 tmp_data_len = get_mesh_pro_str_len(PRO_PUB_KEY);
					memcpy(tc_seg_buf, (u8 *)(&(pro_adv_pkt.len)),OFFSETOF(pro_PB_ADV,transStart.data));
					memcpy(tc_seg_buf+OFFSETOF(pro_PB_ADV,transStart.data), para_pro, tmp_data_len);
					SET_TC_FIFO(TSCRIPT_MESH_TX, tc_seg_buf,tmp_data_len+OFFSETOF(pro_PB_ADV,transStart.data));
					#endif
					prov_para.provison_send_state = STATE_PRO_PUB_KEY_ACK;
			}
			break;
		case STATE_PRO_PUB_KEY_ACK:
			//wait the ack 
			if(p_adv->transAck.GPCF == TRANS_ACK&&p_adv->trans_num >= prov_para.trans_num_last){
				SET_TC_FIFO(TSCRIPT_MESH_RX,(u8 *)(p_adv),p_adv->length+1);
				if(prov_oob.start.pubKey == MESH_PUB_KEY_WITH_OOB){
					// not need to exchange the part of the pubkey ,just need to send the pubkey
					// only need to send the confirm command 
					if(mesh_prov_oob_auth_data(&prov_oob)){
					    LOG_MSG_ERR(TL_LOG_PROVISION,0, 0 ,"prov oob is invalid");
						return ;
					}
					u8 prov_public_key[64] = {0};					
					get_public_key(prov_public_key);
					send_rcv_retry_clr();
					
					memcpy(confirm_input+0x11,prov_public_key,sizeof(prov_public_key));
					memcpy(confirm_input+0x11+64,pub_key_with_oob,64);
					if(prov_oob.start.authMeth == MESH_OUTPUT_OOB){
						prov_oob.oob_out_tick = clock_time()|1;
						prov_para.provison_send_state = STATE_DEV_PUB_KEY_OUTPUT_OOB ;
					}else if (prov_oob.start.authMeth == MESH_INPUT_OOB){
						prov_para.provison_send_state = STATE_DEV_PUB_KEY_INPUT_OOB;
					}else{
						send_confirm_no_pubkey_cmd();
					}
				}else{
					prov_para.provison_send_state = STATE_DEV_PUB_KEY;
				}	
			}
			break;
		case STATE_DEV_PUB_KEY_INPUT_OOB:
			 // how to input the numbers ,wait for the complete cmd 
			 if(p_adv->transBear.bearAck.header.GPCF == TRANS_START &&
				p_adv->transStart.data[0]== PRO_INPUT_COM){
					set_rsp_ack_transnum(p_adv);
					send_rcv_retry_clr();
					send_confirm_no_pubkey_cmd_with_ack();
			 }
			 break;
		case STATE_PRO_FAILED_ACK:
			// send the link close cmd 
			mesh_terminate_provision_link_reset(LINK_CLOSE_FAIL,0);
			break;
		case STATE_DEV_PUB_KEY_OUTPUT_OOB:
				// switch to mainloop process part
			 break;
		
		case STATE_DEV_PUB_KEY:
			if(p_adv->transBear.bearAck.header.GPCF == TRANS_START &&
				p_adv->transStart.data[0]== PRO_PUB_KEY){
				    #if GATEWAY_ENABLE
					gateway_upload_prov_rsp_cmd((u8 *)p_rcv_str,PRO_PUB_KEY);
					#endif
					SET_TC_FIFO(TSCRIPT_MESH_RX,(u8 *)(&rcv_pb),rcv_pb.transStart.total_len+11);
					LOG_MSG_INFO(TL_LOG_PROVISION, 0, 0,"rcv dev pub key:");
					if(mesh_prov_oob_auth_data(&prov_oob)){
					    LOG_MSG_ERR(TL_LOG_PROVISION,0, 0 ,"prov oob info is invalid");
						return ;
					}
					u8 prov_public_key[64] = {0};
					get_public_key(prov_public_key);
					mesh_adv_prov_pubkey_rsp(p_adv);
					memcpy(confirm_input+0x11,prov_public_key,sizeof(prov_public_key));
					memcpy(confirm_input+0x11+64,p_rcv_str->pubkey.pubKeyX,64);
					LOG_MSG_INFO(TL_LOG_PROVISION, 0, 0,"auth meth is %d",prov_oob.start.authMeth);
					if(prov_oob.start.authMeth == MESH_OUTPUT_OOB)	{
						prov_oob.oob_out_tick = clock_time()|1;
						mesh_send_provison_data(TRANS_ACK,0,0,0);
						send_rcv_retry_set(PRO_COMMAND_ACK,0,0);
						LOG_MSG_INFO(TL_LOG_PROVISION, 0, 0,"output oob mode");
						prov_para.provison_send_state = STATE_DEV_PUB_KEY_OUTPUT_OOB ;

					}else if (prov_oob.start.authMeth == MESH_INPUT_OOB) {
						// need to send the ack retry first ,then jump the state to the STATE_DEV_PUB_KEY_INPUT_OOB and wait the incomplete cmd 
						mesh_send_provison_data(TRANS_ACK,0,0,0);
						send_rcv_retry_set(PRO_COMMAND_ACK,0,0);
						prov_para.provison_send_state = STATE_DEV_PUB_KEY_INPUT_OOB;
					}else{
						// should check whether the pubkey is valid or not 
						if(!mesh_check_pubkey_valid(p_rcv_str->pubkey.pubKeyX)){
							// send the link close cmd 
							mesh_send_provison_data(TRANS_ACK,0,0,0);
							send_rcv_retry_set(PRO_COMMAND_ACK,0,0);
							link_close_start_tick = clock_time()|1;
							prov_para.provison_send_state = STATE_DEV_CONFIRM;
							return;
						}
						send_confirm_no_pubkey_cmd_with_ack();
						#if GATEWAY_ENABLE
					    gateway_upload_prov_cmd((u8 *)p_send_str,PRO_CONFIRM);
					    #endif
					}				
				}else if((p_adv->transStart.data[0]>PRO_PUB_KEY)&&(p_adv->transStart.data[0]<=PRO_REC_LIST) ){
                    #if TESTCASE_FLAG_ENABLE	

					mesh_proc_prov_fail(p_adv,PROVISION_FAIL_UNEXPT_PDU);
					#endif
				}
			break;
			/*
		case STATE_PRO_CONFIRM_ACK:
			if(p_adv->transAck.GPCF == TRANS_ACK&&p_adv->trans_num >= prov_para.trans_num_last){
					mesh_adv_prov_confirm_ack(p_adv);
					provision_mag.provison_send_state = STATE_DEV_CONFIRM;
				}
			break;
			*/
		case STATE_DEV_CONFIRM:
			if(p_adv->transBear.bearAck.header.GPCF == TRANS_START &&
				p_adv->transStart.data[0]== PRO_CONFIRM){
					// get device confirm part 
					LOG_MSG_INFO(TL_LOG_PROVISION, 0, 0,"rcv prov confirm:");
					pro_trans_confirm *p_confirm = &(p_rcv_str->confirm);
					if(prov_confirm_check_same_or_not(p_confirm->confirm,pro_confirm)){
						// send the link close cmd 
						LOG_MSG_INFO(TL_LOG_PROVISION, 0, 0,"confirm is the same");
						mesh_proc_prov_fail(p_adv,PROVISION_FAIL_CONFIRM_FAIL);
						return ;
					}
					#if GATEWAY_ENABLE
					gateway_upload_prov_rsp_cmd((u8 *)p_rcv_str,PRO_CONFIRM);
					#endif
					// store the dev comfrim part 
					memcpy(dev_confirm,p_confirm->confirm,get_prov_confirm_value_len());
					mesh_adv_prov_confirm_rsp(p_adv);
					mesh_adv_prov_random_cmd(p_send_str,pro_random);
					#if GATEWAY_ENABLE
					gateway_upload_prov_cmd((u8 *)p_send_str,PRO_RANDOM);
					#endif
					prov_para.provison_send_state = STATE_DEV_RANDOM;
					
				}
			break;
			/*
		case STATE_PRO_RANDOM_ACK:
			if(p_adv->transAck.GPCF == TRANS_ACK&&p_adv->trans_num >= prov_para.trans_num_last){
					mesh_adv_prov_random_ack_cmd(p_adv);
					provision_mag.provison_send_state = STATE_DEV_RANDOM;
				}
			break;
			*/
		case STATE_DEV_RANDOM:
			if(p_adv->transBear.bearAck.header.GPCF == TRANS_START &&
				p_adv->transStart.data[0]== PRO_RANDOM){
				    #if GATEWAY_ENABLE
					gateway_upload_prov_rsp_cmd((u8 *)p_rcv_str,PRO_RANDOM);
					#endif
					u8 pro_dat_tmp[40];
					SET_TC_FIFO(TSCRIPT_MESH_RX,(u8 *)(p_adv),p_adv->length+1);
					mesh_adv_prov_random_rsp(p_adv);
					memcpy(dev_random,p_rcv_str->random.random,get_prov_random_value_len());
					LOG_MSG_INFO(TL_LOG_PROVISION, 0, 0,"rcv dev random:");
					u8 dev_confirm_tmp[32];
					mesh_sec_prov_confirmation_sec (dev_confirm_tmp, confirm_input, 
												145, ecdh_secret, dev_random, pro_auth);						
					if(prov_confirm_check_right_or_not(dev_confirm,dev_confirm_tmp))
					{
					    LOG_MSG_ERR(TL_LOG_PROVISION,0, 0 ,"confirm with ramdom fail");
						mesh_proc_prov_fail(p_adv,PROVISION_FAIL_CONFIRM_FAIL);
						return ;
					}
					u8 pro_session_nonce[16]; // byte 3~15 is session nonce
					u8 pro_session_key[16];
					mesh_sec_prov_session_key_fun (pro_session_key, pro_session_nonce, confirm_input, 145, ecdh_secret, pro_random, dev_random,is_prov_oob_hmac_sha256());
					//save the src pro_dat
					memcpy(pro_dat_tmp,pro_dat,sizeof(pro_dat));

					u8 dev_key[16];
					u8 prov_salt[16];
					mesh_sec_prov_salt_fun(prov_salt,confirm_input,pro_random,dev_random,is_prov_oob_hmac_sha256());
					//calculate the dev_key part 
					mesh_sec_dev_key(dev_key,prov_salt,ecdh_secret);
					// should not set the mesh key info into the mesh_key ,the devkey in the mesh_key is self
					// set_dev_key(dev_key);
					LOG_MSG_INFO(TL_LOG_PROVISION, dev_key, 16,"dev key is:");
					set_pro_unicast_adr(provision_mag.unicast_adr_last);
					
					#if (DONGLE_PROVISION_EN)
					gateway_provision_rsp_cmd(provision_mag.unicast_adr_last);
					VC_node_dev_key_save(provision_mag.unicast_adr_last, dev_key,prov_para.ele_cnt);
					#endif
					LOG_MSG_INFO(TL_LOG_PROVISION, pro_dat, 25,"send prov data:");
					mesh_prov_sec_msg_enc (pro_session_key, pro_session_nonce+3, pro_dat, 25, 8);
                    mesh_adv_prov_data_send(p_send_str,pro_dat);
					//back the src pro dat
					#if GATEWAY_ENABLE
					gateway_upload_prov_cmd((u8 *)p_send_str,PRO_DATA);
					#endif
					memcpy(pro_dat,pro_dat_tmp,sizeof(pro_dat));
					#if TESTCASE_FLAG_ENABLE
					u8 tmp_data_len = get_mesh_pro_str_len(PRO_DATA);
					memcpy(tc_seg_buf, (u8 *)(&(pro_adv_pkt.len)),OFFSETOF(pro_PB_ADV,transStart.data));
					memcpy(tc_seg_buf+OFFSETOF(pro_PB_ADV,transStart.data), para_pro, tmp_data_len);
					SET_TC_FIFO(TSCRIPT_MESH_TX, tc_seg_buf,tmp_data_len+OFFSETOF(pro_PB_ADV,transStart.data));					
					#endif
					
					prov_para.provison_send_state = STATE_PRO_COMPLETE;
				}
			break;
		/*
		case STATE_PRO_DATA_ACK:
			if(p_adv->transAck.GPCF == TRANS_ACK&&p_adv->trans_num >= prov_para.trans_num_last){
				mesh_adv_prov_data_ack(p_adv);
				provision_mag.provison_send_state = STATE_PRO_COMPLETE;
			}
			break;
		*/
		case STATE_PRO_COMPLETE:
			if(p_adv->transBear.bearAck.header.GPCF == TRANS_START &&
				p_adv->transStart.data[0]== PRO_COMPLETE){
				// return the test case result 
				    #if GATEWAY_ENABLE
					gateway_upload_prov_rsp_cmd((u8 *)p_rcv_str,PRO_COMPLETE);
					#endif
					LOG_MSG_INFO(TL_LOG_PROVISION, 0, 0,"rcv prov complete");
				    mesh_adv_prov_complete_rsp(p_adv);
					mesh_prov_end_set_tick();
					//mesh_node_prov_event_callback(EVENT_MESH_PRO_RC_LINK_SUC);
				}
			break;
		default:
			break;
	}
#endif 
	return ;
}

u8 filter_prov_link_id(pro_PB_ADV *p_adv)
{
	u8 invalid =0;
#if !WIN32
	if(prov_para.link_id_filter){
		if(memcmp(p_adv->link_ID,(u8 *)(&prov_para.link_id),4)){
			invalid = 1;
		}
	}
#endif 
	return invalid;
}

int mesh_provision_seg_check(u8 seg_idx, pro_PB_ADV * p_adv, u8 * rcv_pb_buf)
{
	int ret = -1;
	u16 rcv_pb_idx = 0;
	u16 pkt_len =0;
	if(seg_idx){
		rcv_pb_idx = (seg_idx-1)*OFFSET_CON+ OFFSET_START;
	}

	if((p_adv->trans_num != mesh_prov_seg.trans_num) || (mesh_prov_seg.link_id != prov_para.link_id)){
		memset(&mesh_prov_seg, 0x00, sizeof(mesh_prov_seg));
		mesh_prov_seg.seg_n = 0x80; // first pkt maybe not start PDU
		mesh_prov_seg.trans_num = p_adv->trans_num;
		mesh_prov_seg.link_id = prov_para.link_id;
	}
	
	if(!(mesh_prov_seg.seg_map & BIT(seg_idx))){
		if(seg_idx == 0){
			pkt_len = p_adv->length+1;
			if(pkt_len > sizeof(pro_PB_ADV)){
				memset(&mesh_prov_seg, 0x00, sizeof(mesh_prov_seg));
				return -2;// invalid packet 
			}
			memcpy(rcv_pb_buf,p_adv,pkt_len);
			mesh_prov_seg.seg_n = p_adv->transStart.SegN;
			mesh_prov_seg.fcs_tmp = p_adv->transStart.FCS;
		}
		else{
			pkt_len = p_adv->length - 7;
			if((rcv_pb_idx+pkt_len)>sizeof(pro_PB_ADV)){
				memset(&mesh_prov_seg, 0x00, sizeof(mesh_prov_seg));
				return -2;// invalid packet 
			}
			memcpy(rcv_pb_buf+rcv_pb_idx,p_adv->transCon.data ,pkt_len);
		}
		mesh_prov_seg.seg_map |= BIT(seg_idx);
		mesh_prov_seg.seg_cnt++;
	}
	
	if(mesh_prov_seg.seg_cnt == (mesh_prov_seg.seg_n+1)){
		mesh_prov_seg.seg_map = 0;
		mesh_prov_seg.seg_cnt = 0;
		if(mesh_prov_seg.fcs_tmp == crc8_rohc(rcv_pb.transStart.data,rcv_pb.transStart.total_len)){
			ret = 0;
		}
	}
	
	return ret;
}

/**
 * @brief       This function server to process the provisioning PDU and unprovision beacon in PB_ADV. 
 * @param[in]   ini_role- the role of the node. 
 *				MESH_INI_ROLE_NODE: as Provisionee, new device to be provisioned. 
 *				MESH_INI_ROLE_PROVISIONER: as Provisioner(gateway). 
 * @param[in]   p_rcv	- pointer of the adv payload.
 * @return      unused.
 * @note        
 */
int mesh_provison_process(u8 ini_role,u8 *p_rcv)
{	
	pro_PB_ADV * rcv_pb_buf = (&rcv_pb);
	pro_PB_ADV * p_adv = (pro_PB_ADV *) p_rcv;
	if(p_adv->ad_type == MESH_ADV_TYPE_BEACON ){
	    pro_PB_ADV rcv_beacon;
		u16 pkt_len = p_adv->length+1+7;// in the remote prov beacon rcv part ,we need to get the rssi part.
		if(pkt_len>sizeof(pro_PB_ADV)){
			return -1; // avoid the overflow part
		}
		memcpy((u8*)&rcv_beacon,p_adv,pkt_len);
		event_adv_report_t *pa = (event_adv_report_t *)(p_rcv-11);
		mesh_pro_rc_beacon_dispatch(&rcv_beacon,pa->mac);
		return 1;
	}
	if(filter_prov_link_id((pro_PB_ADV *)p_adv)){
		return 1;
	}
	// if the send pdu is not a start pdu ,we need to pack it 
	if(p_adv->transStart.GPCF == TRANS_START ){
		// if receive the trans start pdu ,it need to swap the length 
		endianness_swap_u16((u8 *)(&(p_adv->transStart.total_len)));
		// it means it 's a single start packet 
		if(p_adv->transStart.SegN == 0){
			// check the fcs is right or not 
			u8 start_fcs = p_adv->transStart.FCS;
			if(start_fcs!= crc8_rohc(p_adv->transStart.data,p_adv->transStart.total_len)){
				return 0;
			}
			rcv_pb_buf = p_adv;
		}else{
			if(mesh_provision_seg_check(0, p_adv, (u8 *)rcv_pb_buf)){
				return 1;
			}
		}
	}else if (p_adv->transCon.GPCF == TRANS_CON ){
		if(mesh_provision_seg_check(p_adv->transCon.SegIdx, p_adv,(u8 *) rcv_pb_buf)){
			return 1;
		}
	}else if(p_adv->transStart.GPCF == TRANS_ACK || p_adv->transStart.GPCF == BEARS_CTL){
		// bearer ctl or ack 
		rcv_pb_buf = p_adv;
	}

	if(ini_role == MESH_INI_ROLE_NODE){
		mesh_node_rc_data_dispatch(rcv_pb_buf);
	}else if(ini_role == MESH_INI_ROLE_PROVISIONER && p_adv->ad_type == MESH_ADV_TYPE_PRO){
		if(provision_mag.pro_stop_flag){
			return 1;
		}
		mesh_pro_rc_adv_dispatch(rcv_pb_buf);
	}else{
	}
	return 1;
}

void mesh_prov_proc_loop()
{
    #if FEATURE_PROV_EN
	mesh_adv_provision_retry();
	check_oob_out_timeout();
	wait_and_check_complete_state();
	check_inputoob_proc();
	mesh_loop_provision_end_process();
	check_mesh_node_out_oob_pub_key_send_time();
	mesh_loop_check_link_close_flag();
	mesh_prov_link_close_terminate();
	#if !WIN32
	provision_timeout_cb();
	#endif
	#endif
}
 
#if (IS_VC_PROJECT_MASTER || DONGLE_PROVISION_EN)
void mesh_cfg_keybind_start_trigger_event(u8* p_idx,u8 *p_key,u16 unicast,u16 nk_idx,u8 fastbind)
{
	prov_para.key_bind_lock = 1;
	u16 key_idx =0;
	key_idx = (p_idx[0]<<8)+p_idx[1];
	mesh_kc_cfgcl_mode_para_set(key_idx,p_key,unicast,nk_idx,fastbind);
	#if !WIN32 
	enable_mesh_kr_cfg_filter();
	#endif 
}
void set_provision_stop_flag_act(u8 stop_flag)
{
	provision_mag.pro_stop_flag = stop_flag;
	#if !WIN32
	disable_mesh_adv_filter();
	disable_mesh_kr_cfg_filter();
	#endif
	if(stop_flag){
		mesh_provision_para_reset();		
		#if !WIN32
		prov_para.link_id_filter =0;		
		gateway_adv_filter_init();
		#endif
//		rf_link_light_event_callback(PROV_END_LED_CMD);
	}else{
//		rf_link_light_event_callback(PROV_START_LED_CMD);	
	}
}

u8 mesh_cfg_keybind_end_event(u8 eve,u16 unicast)
{
	key_refresh_cfgcl_proc_t *p_cfgcl =(key_refresh_cfgcl_proc_t *) &key_refresh_cfgcl_proc;
	if(eve == MESH_KEYBIND_EVE_START){
		// set the lock flag of the key_bind_lock
		prov_para.key_bind_lock = 1;
		// gateway will not enter ,wait for the adv-provision for the keybind part 
		mesh_kc_cfgcl_mode_para_set(0,(u8 *)p_cfgcl->ak,unicast,0,0);
		#if !WIN32 
		enable_mesh_kr_cfg_filter();
		#endif 
	}else if (eve == MESH_KEYBIND_EVE_SUC){
		p_cfgcl->st = KR_CFGCL_NORMAL;
		#if GATEWAY_ENABLE
		//set_provision_stop_flag_act(1);//lock the start flag part 
		gateway_upload_keybind_event(MESH_KEYBIND_EVE_SUC);
		#endif
		#if  WIN32
		App_key_bind_end_callback(MESH_APP_KEY_BIND_EVENT_SUC); 
		#else
		disable_mesh_kr_cfg_filter();
		prov_para.key_bind_lock =0;
		gateway_keybind_rsp_cmd(0);
//		rf_link_light_event_callback(LGT_CMD_SET_MESH_INFO);
		#endif 
		LOG_MSG_INFO(TL_LOG_KEY_BIND,0,0,"SEND: mesh keybind event success");
		
	}else if (eve == MESH_KEYBIND_EVE_TIMEOUT){
		LOG_MSG_INFO(TL_LOG_GATT_PROVISION,(u8 *)p_cfgcl, sizeof(key_refresh_cfgcl_proc_t),"appkey bind terminate\r\n");
		// reset the status of the states of the key_refresh_cfgcl_proc
		p_cfgcl->st = KR_CFGCL_NORMAL;
		// reset the status of the provision part 
		mesh_provision_para_reset();
		// reset the mode of the provision mag part 
		// cancel the part of the bind lock flag 
		prov_para.key_bind_lock =0;
		#if  WIN32
		App_key_bind_end_callback(MESH_APP_KEY_BIND_EVENT_TIMEOUT);	
		#else 
		disable_mesh_kr_cfg_filter();
		#endif 
		#if GATEWAY_ENABLE
		gateway_upload_keybind_event(MESH_KEYBIND_EVE_TIMEOUT);
		gateway_keybind_rsp_cmd(1);
		#endif 
		LOG_MSG_INFO(TL_LOG_KEY_BIND,0,0,"SEND: mesh keybind event timeout");
	}
	return 1;

}
u8 VC_search_and_bind_model()
{
    key_refresh_cfgcl_proc_t *p =(key_refresh_cfgcl_proc_t *) &key_refresh_cfgcl_proc;
    VC_node_info_t *p_info = get_VC_node_info(p->node_adr, 1);
    if(p_info){
        #if DONGLE_PROVISION_EN
        VC_node_cps_t *p_cps = &VC_node_cps;
        #else
        VC_node_cps_t *p_cps = &p_info->cps;
        #endif

        mesh_element_head_t *p_ele = (mesh_element_head_t *)(&p_cps->local);
        foreach(i,p->ele_bind_index){
            p_ele = (mesh_element_head_t *)((u8 *)p_ele + get_cps_ele_len(p_ele));
        }
        u16 ele_adr = p->node_adr + p->ele_bind_index;
        mesh_element_model_id *p_model_id = (mesh_element_model_id *)(&p_ele->nums);

        if(p_info->element_cnt && (p->model_bind_index < p_model_id->nums)){//nums
            u16 model_id;
            memcpy(&model_id, p_model_id->id+p->model_bind_index*2, 2);
            if(is_use_device_key(model_id, 1)){
                p->model_bind_index++;
				mesh_kr_cfgcl_retry_init();
            }else{
                LOG_MSG_INFO(TL_LOG_KEY_BIND,0,0,"SEND: appkey bind addr: 0x%04x,sig model id: 0x%04x ",ele_adr,model_id);

				if((!p->is_new_model) && (p->model_id != model_id)){
					p->is_new_model = 1;
				}
				
				p->model_id = model_id;
                cfg_cmd_ak_bind(p->node_adr, ele_adr, p->ak_idx, model_id, 1);
            }
        }
        else if(p_info->element_cnt && (p->model_bind_index < p_model_id->nums+ p_model_id->numv)){
            u32 model_id;
            memcpy(&model_id, p_model_id->id+p_model_id->nums*2+(p->model_bind_index-p_model_id->nums)*4, 4);
            if(is_use_device_key(model_id, 0)){ // some vendor model also may use device key.
                p->model_bind_index++;
				mesh_kr_cfgcl_retry_init();
            }else{
                LOG_MSG_INFO(TL_LOG_KEY_BIND,0,0,"SEND: appkey bind addr: 0x%04x,vendor model id: 0x%08x ",ele_adr,model_id);
    			if((!p->is_new_model) && (p->model_id != model_id)){
    				p->is_new_model = 1;
    			}
    			p->model_id = model_id;
                cfg_cmd_ak_bind(p->node_adr, ele_adr, p->ak_idx, model_id, 0);
            }
        }
        else{
            if((++p->ele_bind_index) < p_info->element_cnt){
                p->model_bind_index = 0;
				p->is_new_model = 1;
            }else{
                mesh_cfg_keybind_end_event(MESH_KEYBIND_EVE_SUC,0);
            }
        }
        return 0;
    }
    return -1;
}
 //-------------------NET KEY INDEX / APP KEY INDEX
#if 0
 STATIC_ASSERT(sizeof(VC_key_idx_list_t) % 16 == 0);
 
 VC_key_idx_list_t VC_key_idx_list[NET_KEY_LIST_MAX_PROV] = {
	 {
		 KEY_VALID, NET0, {0}, NETKEY_A,
		 {
			 {KEY_VALID, NET0_APP0, {0}, APPKEY_A},
			 {KEY_VALID, NET0_APP1, {0}, APPKEY_B},
			 {KEY_VALID, NET0_APP2, {0}, APPKEY_C},
			 {KEY_VALID, NET0_APP3, {0}, APPKEY_D},
		 },
	 },
 };
#endif
#endif






