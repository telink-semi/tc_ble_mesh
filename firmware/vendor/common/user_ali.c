/********************************************************************************************************
 * @file	user_ali.c
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
#include "user_ali.h"
#include "app_health.h"
#include "proj_lib/sig_mesh/app_mesh.h"
#include "proj_lib/mesh_crypto/sha256_telink.h"
#include "vendor_model.h"
#include "fast_provision_model.h"
#include "proj_lib/mesh_crypto/aes_cbc.h"
#include "vendor/common/mesh_node.h"
#include "vendor/common/mesh_ota.h"


const char num2char[17] = "0123456789abcdef";


#if(AIS_ENABLE)
STATIC_ASSERT(sizeof(sha256_dev_uuid_str) <= 16);   // because sizeof dev uuid is 16

// sha256 init for three parameters 
#if ALI_NEW_PROTO_EN
	u32 con_product_id = 9873;
	//u8 con_mac_address[6]={0xf8,0xa7,0x63,0x6e,0x46,0x49};// in the flash ,is big endianness 
	u8 con_mac_address[6]={0x49,0x46,0x6e,0x63,0xa7,0xf8};// in the demo ,switch to small endianness .
	u8 con_sec_data[16]={0xc5,0xa2,0x78,0xcf, 0x21,0x4b,0x84,0x63, 0xc7,0xb4,0x70,0xe2, 0x83,0x7d,0x55,0xb5};
	#define SIZE_CON_SEC_DATA   (sizeof(con_sec_data))
	#else
#if(MESH_USER_DEFINE_MODE == MESH_CLOUD_ENABLE)
u32 con_product_id=192941;// little endianness 
	#if(MESH_USER_DEFINE_MODE != MESH_MI_SPIRIT_ENABLE)
const
	#endif
u8 con_mac_address[6]={0xee,0x11,0x33,0x55,0x77,0x03};//small endianness
//char con_sec_data[]="claoePqYe1avDpmf8Jcm4jF52kVOLS1Q";
char con_sec_data[32];
#define  SIZE_CON_SEC_DATA  32

#elif((MESH_USER_DEFINE_MODE == MESH_SPIRIT_ENABLE) || (MESH_USER_DEFINE_MODE == MESH_MI_SPIRIT_ENABLE))
u32 con_product_id=0x00000002;// little endianness 
#if(MESH_USER_DEFINE_MODE != MESH_MI_SPIRIT_ENABLE)
const
#endif
u8  con_mac_address[6]={0x9e,0x16,0x11,0x07,0xda,0x78};//small endianness
    #if 0 // need to open it to make the init three para enable 
u8 con_sec_data[16]={ 0x04,0x6e,0x68,0x11,0x27,0xed,0xe6,0x70,
					  0x94,0x44,0x18,0xdd,0xb1,0xb1,0x7b,0xdc};
    #else
u8 con_sec_data[16];
    #endif
#define SIZE_CON_SEC_DATA   (sizeof(con_sec_data))
#elif((MESH_USER_DEFINE_MODE == MESH_TAIBAI_ENABLE))
	#if 0
	u32 con_product_id=7003001;// little endianness 
	u8  con_mac_address[6]={0x6e,0x79,0x12,0x75,0x60,0xd4};//small endianness
	u8 con_sec_data[16];
	#else
		#if DU_LPN_EN
			#if LPN_CONTROL_EN
	u32 con_product_id=DU_PID;// little endianness 
	//48f3f3bf28b0
	u8  con_mac_address[6]={0xb0,0x28,0xbf,0xf3,0xf3,0x48};
	//e2458e28034096672f7a4cc5dcae47a5
	u8 con_sec_data[16]={0xe2,0x45,0x8e,0x28,	0x03,0x40,0x96,0x67,
						 0x2f,0x7a,0x4c,0xc5,	0xdc,0xae,0x47,0xa5};			
			#else
	//006ae731
	u32 con_product_id=DU_PID;// little endianness 
	//882d53f66826
	u8  con_mac_address[6]={0xc1,0x2d,0x6f,0xda,0xe0,0xcc};//little endianness
	//968829854b47b5dfb09d9edfcc8b9a0f
	u8 con_sec_data[16]={0x54,0x26,0x27,0xd3,	0x60,0x58,0x94,0xcf,
						 0x48,0xa9,0xc8,0x1c,	0x8b,0x8c,0x78,0xc0};
		    #endif
		#elif DU_ULTRA_PROV_EN
		u32 con_product_id=0x6b1df1; 
		u8  con_mac_address[6]={0xb6,0x8c,0x12,0x75,0x60,0xd4};
		u8 con_sec_data[16]={0x7b,0x45,0xe1,0x86,0xe0,0x84,0xc2,0xca,0xdb,0xe3,0x73,0x7c,0x8e,0x9a,0xd4,0x8f};
		#else
	u32 con_product_id=DU_PID;// little endianness 
	u8  con_mac_address[6]={0x7D,0x79,0x12,0x75,0x60,0xd4};//little endianness
	u8 con_sec_data[16]={0x49,0x22,0xeb,0x7a,	0x0a,0x45,0x81,0x8d,
						 0xa4,0x34,0x7c,0xd4,	0xed,0x1b,0x4c,0xf9};

		#endif
	#endif
	#define SIZE_CON_SEC_DATA   (sizeof(con_sec_data))
#elif((MESH_USER_DEFINE_MODE == MESH_NMW_ENABLE))
	u32 con_product_id=0x324a584c;// little endianness
	u8	con_mac_address[6]={0x20,0x00,0x00,0x38,0xc1,0xa4};//little endianness
    #if 0 // need to open it to make the init three para enable 
	u8 con_sec_data[16]={0x30,0x4b,0xbc,0x95,0xeb,0x2b,0xc9,0x82,0xce,0xba,0xc4,0x37,0xce,0x07,0xcf,0x23};
    #else
	u8 con_sec_data[16];
    #endif
	#define SIZE_CON_SEC_DATA   (sizeof(con_sec_data))
#endif
#endif

#if(DUAL_VENDOR_EN)
// default use mi_api's parameters, if device provisioned by spirit, reinit the parameters.
void mesh_ais_global_var_set()
{
    vendor_id_check_and_update();
	#if 0 // can't change mac to ensure reconnect
	memcpy(tbl_mac, con_mac_address, 6);
	extern rf_packet_scan_rsp_t	pkt_scan_rsp;
	memcpy(pkt_adv.advA, tbl_mac, BLE_ADDR_LEN);
	memcpy(pkt_scan_rsp.advA, tbl_mac, BLE_ADDR_LEN);
	#endif
	model_sig_cfg_s.nw_transmit.count = AIS_TRANSMIT_CNT_DEF;
	model_sig_cfg_s.nw_transmit.invl_steps = AIS_TRANSMIT_INVL_STEPS_DEF;
	model_sig_cfg_s.relay_retransmit.count = AIS_TRANSMIT_CNT_DEF_RELAY;
	model_sig_cfg_s.relay_retransmit.invl_steps = AISTRANSMIT_INVL_STEPS_DEF_RELAY;
	
    //provision_mag_cfg_s_store(); // no need, save later in mesh_provision_par_set()
}
#endif

void read_three_para_sha256_from_flash()
{
#if !WIN32
	u8 idx =0;
	u32 product_id_flash;
	flash_read_page(FLASH_ADR_THREE_PARA_ADR,sizeof(product_id_flash),(u8 *)(&product_id_flash));
	if(product_id_flash == 0xffffffff){
		return;
	}
	flash_read_page(FLASH_ADR_THREE_PARA_ADR,sizeof(con_product_id),(u8 *)&con_product_id);
	idx += sizeof(con_product_id);
	flash_read_page(FLASH_ADR_THREE_PARA_ADR+idx,sizeof(tbl_mac),tbl_mac);// read big endian
	endianness_swap_u48(tbl_mac);// change to small endian
	#if(MESH_USER_DEFINE_MODE == MESH_MI_SPIRIT_ENABLE)
	memcpy(con_mac_address, tbl_mac, 6);
	#endif
	idx += sizeof(tbl_mac);
	flash_read_page(FLASH_ADR_THREE_PARA_ADR+idx,SIZE_CON_SEC_DATA,(u8 *)con_sec_data);
#endif
	return ;
}


void set_sha256_init_para_mode(u8 mode)
{
	if(mode){
		// init the mac address part 
		memcpy(tbl_mac,con_mac_address,sizeof(con_mac_address));// get mac from the ini para 
		read_three_para_sha256_from_flash();
	}
	return ;
}

void set_dev_uuid_for_sha256()
{
	sha256_dev_uuid_str dev_uuid;
	sha256_dev_uuid_str *p_uuid = &dev_uuid;
	memset(p_uuid,0,sizeof(sha256_dev_uuid_str));
	p_uuid->cid = VENDOR_ID;
	p_uuid->adv_ver = 0x01;
	p_uuid->auth_en = 1;
	p_uuid->ota_en =1;
	p_uuid->ble_ver = BLE_4_2;
	memcpy(p_uuid->product_id,&con_product_id,sizeof(con_product_id));
	memcpy(p_uuid->mac, tbl_mac, sizeof(tbl_mac));
	#if(MESH_USER_DEFINE_MODE == MESH_MI_SPIRIT_ENABLE)
	memcpy(p_uuid->mac, con_mac_address, sizeof(tbl_mac));
	#endif
	p_uuid->uuid_ver = 1;
	p_uuid->adv_flag = 0;
	memcpy(prov_para.device_uuid,(u8 *)(p_uuid),sizeof(sha256_dev_uuid_str));
}

#if(MESH_USER_DEFINE_MODE == MESH_CLOUD_ENABLE)
void create_sha256_input_string_node(char *p_input,u8 *pid,u8 *p_mac,u8 *p_secret)
{
	u8 idx =0;
	u8 con_product_id_rev[4];
	swap32(con_product_id_rev,pid);
	
	#if(MESH_USER_DEFINE_MODE == MESH_CLOUD_ENABLE)
	memcpy(p_input + idx, con_sec_data, SIZE_CON_SEC_DATA);
	idx += SIZE_CON_SEC_DATA;
	#else	
	for(int i=0;i<16;i++){// need to change to string .
		p_input[idx++] = num2char [(p_secret[i]>>4) & 15];
		p_input[idx++] = num2char [p_secret[i] & 15];
	}
	#endif
	p_input[idx++]=',';
	for(int i=0;i<6;i++){
		p_input[idx++] = num2char [(p_mac[i]>>4) & 15];
		p_input[idx++] = num2char [p_mac[i] & 15];
	}
	p_input[idx++]=',';
	for(int i=0;i<4;i++){
		p_input[idx++] = num2char [(con_product_id_rev[i]>>4) & 15];
		p_input[idx++] = num2char [con_product_id_rev[i] & 15];
	}
}
#endif
#if ALI_NEW_PROTO_EN
void ali_new_create_sha256_input_string(char *p_input,u8 *pid,u8 *p_mac,u8 *p_secret,u8 *p_random)
{
	u8 idx =0;
	u8 con_product_id_rev[4];
	swap32(con_product_id_rev,pid);
	for(int i=0;i<4;i++){
		p_input[idx++] = num2char [(con_product_id_rev[i]>>4) & 15];
		p_input[idx++] = num2char [con_product_id_rev[i] & 15];
	}
	p_input[idx++]=',';
	for(int i=0;i<6;i++){
		p_input[idx++] = num2char [(p_mac[i]>>4) & 15];
		p_input[idx++] = num2char [p_mac[i] & 15];
	}
	p_input[idx++]=',';
	for(int i=0;i<16;i++){// need to change to string .
		p_input[idx++] = num2char [(p_secret[i]>>4) & 15];
		p_input[idx++] = num2char [p_secret[i] & 15];
	}
	p_input[idx++]=',';
	for(int i=0;i<16;i++){// need to change to string .
		p_input[idx++] = num2char [(p_random[i]>>4) & 15];
		p_input[idx++] = num2char [p_random[i] & 15];
	}
}
#endif


void calculate_sha256_node_oob(u8 *p_oob ,u8 *p_random)
{
	#if ALI_NEW_PROTO_EN
	char sha256_in[87];
	#else
	char sha256_in[54];
	#endif
	// calculate the tbl_mac address part ,use big endian
	u8 mac_address_sha256[6];
	#if(MESH_USER_DEFINE_MODE == MESH_MI_SPIRIT_ENABLE)
	swap48(mac_address_sha256,con_mac_address);
	#else
	swap48(mac_address_sha256,tbl_mac);
	#endif
	
	#if(MESH_USER_DEFINE_MODE == MESH_CLOUD_ENABLE)
	create_sha256_input_string_node(sha256_in,(u8 *)&con_product_id,mac_address_sha256,(u8 *)con_sec_data);
	#else
		#if ALI_NEW_PROTO_EN
	ali_new_create_sha256_input_string(sha256_in,(u8 *)&con_product_id,mac_address_sha256,con_sec_data,p_random);
		#else
	create_sha256_input_string(sha256_in,(u8 *)&con_product_id,mac_address_sha256,(u8 *)con_sec_data);
		#endif
	#endif
	mbedtls_sha256((u8 *)sha256_in,sizeof(sha256_in),p_oob,0);
}

void calculate_sha256_node_auth_value(u8 *auth_value)
{
	#ifndef WIN32		// confirm later
	u8 sha256_out[32];
	calculate_sha256_node_oob(sha256_out,0);
	memcpy(auth_value, sha256_out, 16);
	#endif
}

void calculate_sha256_to_create_pro_oob(u8 *pro_auth,u8 *random)
{
	#ifndef WIN32 
	u8 sha256_out[32];
	calculate_sha256_node_oob(sha256_out,random);
	memcpy(pro_auth,sha256_out,16);
	#endif
}

void calculate_sha256_to_create_static_oob()
{
	#if !WIN32		// confirm later
	u8 sha256_out[32];
	calculate_sha256_node_oob(sha256_out,dev_random);
	mesh_set_dev_auth(sha256_out, 16);
	#endif
}

ais_gatt_auth_t ais_gatt_auth;
void ais_gatt_auth_init()
{
	ais_gatt_auth.auth_ok = 0;
}

int ais_auth_cipher(u16 connHandle, ais_msg_t *p_ais_msg)
{
	ais_msg_t ais_msg_result;
	memset(&ais_msg_result, 0x00, sizeof(ais_msg_result));		
	ais_msg_result.header = p_ais_msg->header;
	ais_msg_result.msg_type = AIS_AES_CIPHER;
	ais_msg_result.frame_seq = ais_msg_result.frame_total = 0;
	ais_msg_result.length = 0x10;
	memcpy(ais_msg_result.data, p_ais_msg->data, 16);
	return blc_gatt_pushHandleValueIndicate(connHandle, AIS_INDICATE_HANDLE, (u8 *)&ais_msg_result, OFFSETOF(ais_msg_t, data)+16);
}

AES_ctx ais_aes_ctx;
const u8 iv[] = "123aqwed#*$!(4ju";
#if BLE_MULTIPLE_CONNECTION_ENABLE
int ais_write_pipe(u16 conn_handle, void *p)
#else
int ais_write_pipe(void *p)
#endif
{
	#if !BLE_MULTIPLE_CONNECTION_ENABLE
	u16 conn_handle = BLS_CONN_HANDLE;
	#endif
		
	rf_packet_att_data_t *req = (rf_packet_att_data_t*)p;
	ais_msg_t *ais_p = (ais_msg_t *)req->dat;

	if(ais_gatt_auth.auth_ok){
		u16 len = ((req->l2cap-3)+AES_BLOCKLEN-1)/AES_BLOCKLEN*AES_BLOCKLEN;
		aes_cbc_decrypt(ais_p->data, len>AIS_MAX_DATA_SIZE?0:len, &ais_aes_ctx, ais_gatt_auth.ble_key, iv);
	}
	
	if((!ais_gatt_auth.auth_ok) && (AIS_AUTH_RANDOM == ais_p->msg_type) && (23 == req->l2cap)){
		u8 mac_address_sha256[6];
		char sha256_in[71];
		u8 sha256_out[32];
		
		memcpy(sha256_in, ais_p->data, 16);
		sha256_in[16] = ',';
		swap48(mac_address_sha256,tbl_mac);
		create_sha256_input_string(sha256_in+17,(u8 *)&con_product_id,mac_address_sha256,(u8 *)con_sec_data);
		mbedtls_sha256((u8 *)sha256_in,sizeof(sha256_in),sha256_out,0);
		memcpy(ais_gatt_auth.ble_key, sha256_out, 16);

		aes_cbc_encrypt(ais_p->data, 16, &ais_aes_ctx, ais_gatt_auth.ble_key, iv);
		ais_auth_cipher(conn_handle, ais_p);
	}
	else if(AIS_AUTH_CHECK == ais_p->msg_type){
		ais_p->msg_type = AIS_AUTH_RESULT;
		if(ais_p->data[0]){//fail
			ais_gatt_auth_init();	
			ais_p->data[0] = 0x01; // 1 means fail
		}
		else{
			ais_p->data[0] = 0;	  // 0 means success
			ais_gatt_auth.auth_ok = 1;
		}
		
		return blc_gatt_pushHandleValueIndicate(conn_handle, AIS_INDICATE_HANDLE, (u8 *)req->dat, OFFSETOF(ais_msg_t, data)+1);
	}
	return 0;
}

#define MAIN_VERSION		0
#define SUB_VERSION			0
#define MODIFY_VERSION		0

const ais_fw_info_t  ais_fw_info = { 
	.device_type = (u8)MESH_PID_SEL, // device type
    .fw_version = (MAIN_VERSION<<16) | (SUB_VERSION<<8) | MODIFY_VERSION, 
};

int ais_ota_version_get()
{
	ais_msg_t ais_version;
	memset(&ais_version, 0x00, sizeof(ais_version));
	ais_version.msg_type = AIS_FW_VERSION_RSP;
	ais_version.length = ais_gatt_auth.auth_ok?AES_BLOCKLEN:sizeof(ais_fw_info);
	memcpy(ais_version.data, &ais_fw_info, sizeof(ais_fw_info));
	if(ais_gatt_auth.auth_ok){
		ais_version.enc_flag = 1;
		AES128_pkcs7_padding(ais_version.data, sizeof(ais_fw_info), ais_version.data);
		aes_cbc_encrypt(ais_version.data, sizeof(ais_fw_info), &ais_aes_ctx, ais_gatt_auth.ble_key, iv);
	}
	return bls_att_pushNotifyData(AIS_NOTIFY_HANDLE, (u8 *)&ais_version, OFFSETOF(ais_msg_t,data)+(ais_gatt_auth.auth_ok?AES_BLOCKLEN:sizeof(ais_fw_info)));
}

int ais_ota_req(u8 *p)
{
	ais_msg_t ais_msg_rsp;
	//ais_ota_req_t *ota_req_p = (ais_ota_req_t *)p;
	memset(&ais_msg_rsp, 0x00, sizeof(ais_msg_rsp));		

	ais_msg_rsp.msg_type = AIS_OTA_RSP;
	ais_msg_rsp.length = ais_gatt_auth.auth_ok?AES_BLOCKLEN:sizeof(ais_ota_rsp_t);
	ais_msg_rsp.ais_ota_rsp.one_round_pkts = 0;//must set to 0 now.
#if 0 // set 0 always allow ota.
	if((ota_req_p->device_type == ais_fw_info.device_type) && (ota_req_p->fw_version > ais_fw_info.fw_version)&&(ota_req_p->ota_flag == 0))
#endif
	{
		ais_msg_rsp.ais_ota_rsp.allow_ota = ais_gatt_auth.auth_ok;

		ota_adr_index = -1;
		#if __TLSR_RISCV_EN__
		blotaSvr.ota_busy = ais_gatt_auth.auth_ok;   //set flag
		#else
		blcOta.ota_start_flag = ais_gatt_auth.auth_ok;   //set flag
		#endif
		blt_ota_start_tick = clock_time()|1;  //mark time
		if(otaStartCb && ais_gatt_auth.auth_ok){
			otaStartCb();
		}
	}
	
	if(ais_gatt_auth.auth_ok){	
#if (ZBIT_FLASH_WRITE_TIME_LONG_WORKAROUND_EN)
		check_and_set_1p95v_to_zbit_flash();
#endif
#if APP_FLASH_PROTECTION_ENABLE
		app_flash_protection_ota_begin();
#endif
		ais_msg_rsp.enc_flag = 1;
		AES128_pkcs7_padding(ais_msg_rsp.data, sizeof(ais_ota_rsp_t), ais_msg_rsp.data);
		aes_cbc_encrypt(ais_msg_rsp.data, sizeof(ais_ota_rsp_t), &ais_aes_ctx, ais_gatt_auth.ble_key, iv);
	}
	
	return bls_att_pushNotifyData(AIS_NOTIFY_HANDLE, (u8 *)&ais_msg_rsp, OFFSETOF(ais_msg_t,data)+(ais_gatt_auth.auth_ok?AES_BLOCKLEN:sizeof(ais_ota_rsp_t)));
}

int ais_ota_result(u8 result)
{
	ais_msg_t ais_msg_result;
	memset(&ais_msg_result, 0x00, sizeof(ais_msg_result));		

	ais_msg_result.msg_type = AIS_OTA_RESULT;
	ais_msg_result.length = ais_gatt_auth.auth_ok?AES_BLOCKLEN:1;
	ais_msg_result.ota_result = (OTA_SUCCESS==result) ? 1:0;

	if(ais_gatt_auth.auth_ok){
		ais_msg_result.enc_flag = 1;
		AES128_pkcs7_padding(ais_msg_result.data, 1, ais_msg_result.data);
		aes_cbc_encrypt(ais_msg_result.data, 1, &ais_aes_ctx, ais_gatt_auth.ble_key, iv);
	}
	return bls_att_pushNotifyData(AIS_NOTIFY_HANDLE, (u8 *)&ais_msg_result, OFFSETOF(ais_msg_t,data)+(ais_gatt_auth.auth_ok?AES_BLOCKLEN:1));
}

int ais_ota_rc_report(u8 frame_desc, u32 trans_size)
{
	ais_msg_t ais_msg_result;
	memset(&ais_msg_result, 0x00, sizeof(ais_msg_result));		

	ais_msg_result.msg_type = AIS_OTA_RECEIVED;
	ais_msg_result.length = ais_gatt_auth.auth_ok?AES_BLOCKLEN:sizeof(ais_ota_receive_t);
	ais_msg_result.ais_ota_rcv.seg_index = frame_desc;
	ais_msg_result.ais_ota_rcv.trans_size_last = trans_size;
	if(ais_gatt_auth.auth_ok){
		ais_msg_result.enc_flag = 1;
		AES128_pkcs7_padding(ais_msg_result.data, sizeof(ais_ota_receive_t), ais_msg_result.data);
		aes_cbc_encrypt(ais_msg_result.data, sizeof(ais_ota_receive_t), &ais_aes_ctx, ais_gatt_auth.ble_key, iv);
	}
	return bls_att_pushNotifyData(AIS_NOTIFY_HANDLE, (u8 *)&ais_msg_result, OFFSETOF(ais_msg_t,data)+(ais_gatt_auth.auth_ok?AES_BLOCKLEN:sizeof(ais_ota_receive_t)));
}


const u8 company[4] = {'K', 'N', 'L', 'T'};
#if BLE_MULTIPLE_CONNECTION_ENABLE
int ais_otaWrite(u16 conn_handle, void * p)
#else
int ais_otaWrite(void * p)
#endif
{
	rf_packet_att_data_t *req = (rf_packet_att_data_t*)p;
	static u8 err_flg = OTA_SUCCESS;
	static u32 fw_rcv_total_size = 0;
	ais_msg_t *msg_p = (ais_msg_t *)req->dat;
	if(ais_gatt_auth.auth_ok && (msg_p->msg_type != AIS_OTA_DATA)){
		u16 len = ((req->l2cap-3)+AES_BLOCKLEN-1)/AES_BLOCKLEN*AES_BLOCKLEN;
		aes_cbc_decrypt(msg_p->data, (len>AIS_MAX_DATA_SIZE)?0:len, &ais_aes_ctx, ais_gatt_auth.ble_key, iv);
	}
	
	if(msg_p->msg_type == AIS_FW_VERSION_GET){
		ais_ota_version_get();
	}
	else if(msg_p->msg_type == AIS_OTA_REQ){
		ais_ota_req(msg_p->data);
	}

#if __TLSR_RISCV_EN__
	if(!blotaSvr.ota_busy){
		return 0;
	}
#else
	if(!blcOta.ota_start_flag){
		return 0;
	}
#endif
	
	if(msg_p->msg_type == AIS_OTA_END){
		if(FW_CHECK_AGTHM1 == get_ota_check_type()){
			if(is_valid_ota_check_type1()){
				err_flg = OTA_SUCCESS;
			}
			else{
				err_flg = OTA_DATA_CRC_ERR;
			}
		}
		 	
		blt_ota_finished_flag_set(err_flg);

		ais_ota_result(err_flg);
	}
	else if(msg_p->msg_type == AIS_OTA_DATA){
		u16 cur_index =  ota_adr_index+1;
		if((msg_p->frame_seq == (cur_index%(msg_p->frame_total+1)))){
			#if !__TLSR_RISCV_EN__
			blt_ota_start_tick = clock_time()|1;  //mark time			
			#endif
			u16 data_len = msg_p->length;
			
			if(cur_index == 0){
				if(memcmp(req->dat+12, company, sizeof(company))){
					err_flg = OTA_FIRMWARE_MARK_ERR;
				}
			}
			//log_data(TR_24_ota_adr,ota_adr);
			if((cur_index*data_len)>=(ota_firmware_size_k<<10)){
				err_flg = OTA_FW_SIZE_ERR;
			}else{
				#if __TLSR_RISCV_EN__
				ota_save_data (fw_rcv_total_size, data_len, req->dat + 4);
				#else
				ota_save_data (fw_rcv_total_size, req->dat + 4, data_len);
				#endif
				flash_read_page(ota_program_offset + fw_rcv_total_size, data_len, mesh_cmd_ut_rx_seg);

				if(!memcmp(mesh_cmd_ut_rx_seg,req->dat + 4, data_len)){  //OK
					ota_adr_index++;
					fw_rcv_total_size += data_len;				
					if((!ais_gatt_auth.auth_ok) || (msg_p->frame_total == msg_p->frame_seq)){
						ais_ota_rc_report(msg_p->frame_desc, fw_rcv_total_size);
					}
				}
				else{ //flash write err
					err_flg = OTA_WRITE_FLASH_ERR;
				}
			}
				
		}
		else if(msg_p->frame_seq == (cur_index%(msg_p->frame_total+1))){  //maybe repeated OTA data, we neglect it, do not consider it ERR
			ais_ota_rc_report((msg_p->frame_desc & 0xf0)|(ota_adr_index % (msg_p->frame_total+1)), fw_rcv_total_size);
		}
		else{  //adr index err, missing at least one OTA data
			ais_ota_rc_report((msg_p->frame_desc & 0xf0)|(ota_adr_index % (msg_p->frame_total+1)), fw_rcv_total_size);
		}
	}

	if(err_flg){
		blt_ota_finished_flag_set(err_flg);
	}

	return 0;
}
#else
void set_sha256_init_para_mode(u8 mode){}
void set_dev_uuid_for_sha256(){}
void calculate_sha256_to_create_static_oob(){}

#endif
void create_sha256_input_string(char *p_input,u8 *pid,u8 *p_mac,u8 *p_secret)
{
	u8 idx =0;
	u8 con_product_id_rev[4];
	swap32(con_product_id_rev,pid);
	for(int i=0;i<4;i++){
		p_input[idx++] = num2char [(con_product_id_rev[i]>>4) & 15];
		p_input[idx++] = num2char [con_product_id_rev[i] & 15];
	}
	p_input[idx++]=',';
	for(int i=0;i<6;i++){
		p_input[idx++] = num2char [(p_mac[i]>>4) & 15];
		p_input[idx++] = num2char [p_mac[i] & 15];
	}
	p_input[idx++]=',';
#if(MESH_USER_DEFINE_MODE == MESH_CLOUD_ENABLE)
	memcpy(p_input + idx, con_sec_data, SIZE_CON_SEC_DATA);
	idx += SIZE_CON_SEC_DATA;
#else
	for(int i=0;i<16;i++){// need to change to string .
		p_input[idx++] = num2char [(p_secret[i]>>4) & 15];
		p_input[idx++] = num2char [p_secret[i] & 15];
	}
#endif
}

#define TEST_ALI_AUTH_ENABLE 0
#if TEST_ALI_AUTH_ENABLE
u32 ali_pid = 0x293e2;
u8 ali_mac[6]={0xab,0xcd,0xf0,0xf1,0xf2,0xf3};
u8 ali_secret[16]={	0x53,0xda,0xed,0x80,0x5b,0xc5,0x34,0xa4,
					0xa9,0x3c,0x82,0x5e,0xd2,0x0a,0x70,0x63};
u8 ali_random_dev[16]={	0x78,0x89,0xb0,0xaf,0x41,0x7b,0x96,0x7b,
						0xdc,0xd7,0xb8,0x14,0xd2,0xbb,0xff,0xaf};
u8 ali_random_pro[16]={	0xaa,0x48,0x4b,0x09,0x9a,0xe4,0xc7,0x76,
						0x2f,0xcb,0x1b,0x71,0x96,0x8b,0xa7,0xdf};

void ali_new_create_sha256_input_string(char *p_input,u8 *pid,u8 *p_mac,u8 *p_secret,u8 *p_random)
{
	u8 idx =0;
	u8 con_product_id_rev[4];
	swap32(con_product_id_rev,pid);
	for(int i=0;i<4;i++){
		p_input[idx++] = num2char [(con_product_id_rev[i]>>4) & 15];
		p_input[idx++] = num2char [con_product_id_rev[i] & 15];
	}
	p_input[idx++]=',';
	for(int i=0;i<6;i++){
		p_input[idx++] = num2char [(p_mac[i]>>4) & 15];
		p_input[idx++] = num2char [p_mac[i] & 15];
	}
	p_input[idx++]=',';
	for(int i=0;i<16;i++){// need to change to string .
		p_input[idx++] = num2char [(p_secret[i]>>4) & 15];
		p_input[idx++] = num2char [p_secret[i] & 15];
	}
	p_input[idx++]=',';
	for(int i=0;i<16;i++){// need to change to string .
		p_input[idx++] = num2char [(p_random[i]>>4) & 15];
		p_input[idx++] = num2char [p_random[i] & 15];
	}
}

void calculate_auth_value()
{
	static u8 ali_input_string[87];
	static u8 ali_output_sha[32];
	ali_new_create_sha256_input_string(ali_input_string,(u8*)(&ali_pid),ali_mac,ali_secret,ali_random_pro);
	mbedtls_sha256(ali_input_string,sizeof(ali_input_string),ali_output_sha,0);
}
#endif



