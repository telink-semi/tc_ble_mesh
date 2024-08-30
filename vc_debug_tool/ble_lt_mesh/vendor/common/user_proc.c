/********************************************************************************************************
 * @file	user_proc.c
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
#include "user_proc.h"
#include "app_health.h"
#include "proj_lib/sig_mesh/app_mesh.h"
#include "vendor/common/lighting_model.h"
#include "vendor/common/generic_model.h"
#include "vendor_model.h"
#include "fast_provision_model.h"
#include "proj_lib/mesh_crypto/aes_att.h"
#include "proj_lib/mesh_crypto/mesh_md5.h"
#include "vendor/common/certify_base/certify_base_crypto.h"
#if PAIR_PROVISION_ENABLE
#include "pair_provision.h"
#endif

#if(AIS_ENABLE)
extern const u16 du_pri_service_uuid_16 ;
extern const u16 ais_pri_service_uuid ;
	#if DU_ENABLE
#include "user_du.h"
#include "vendor/common/mi_api/telink_sdk_mible_api.h"
#define AIS_DEVICE_NAME	"Mesh__Du"
	#elif NMW_ENABLE
#define AIS_DEVICE_NAME	"Mesh_NMW"
	#else
#define AIS_DEVICE_NAME	"Mesh Ali"
	#endif
	
#define AIS_UUID_LEN	(NMW_ENABLE?0:4)

u8 ais_pri_data_set(u8 *p)
{
	u8 device_name[]={AIS_DEVICE_NAME};
	u8 name_len = (sizeof(device_name) - 1)>9?9:sizeof(device_name) - 1;//max 31 -( 4 bytes uuid + 16 byte vendor data) - 2 bytes head

	if(AIS_UUID_LEN){
		//service uuid
		p[0] = 3;
		p[1] = 2;//incomplete  service uuid
		#if DU_ENABLE
		p[2] = du_pri_service_uuid_16&0xff;
		p[3] = du_pri_service_uuid_16>>8;
		#else
		p[2] = ais_pri_service_uuid&0xff;
		p[3] = ais_pri_service_uuid>>8;
		#endif
	}
	//name
	p[AIS_UUID_LEN] = name_len + 1;
	p[AIS_UUID_LEN+1] = 0x09;
	memcpy(p+AIS_UUID_LEN+2, device_name, name_len);
	//ais data
	ais_pri_t *pri_data = (ais_pri_t *)(p+name_len+AIS_UUID_LEN+2);
	pri_data->length = sizeof(ais_pri_t) - 1;
	pri_data->type = 0xff;
	pri_data->cid = g_vendor_id;
	pri_data->vid = 0x85;
	pri_data->ble_version = 0x01;
	pri_data->ota_support = 1;
	pri_data->authen_en = 1;
	pri_data->secret_type = 1;
	pri_data->prov_flag = is_provision_success();
	pri_data->rfu = 0;
	extern u32 con_product_id;
	pri_data->pid = con_product_id;
	memcpy(pri_data->mac, tbl_mac, 6);

	u8 ais_rsp_len = AIS_UUID_LEN+name_len+2+sizeof(ais_pri_t);
	return ais_rsp_len;
}
#endif

#if (MESH_USER_DEFINE_MODE == MESH_AES_ENABLE)
u8 magic_aes_nums[16]={	0x11,0xff,0x22,0xee,0x33,0xdd,0x44,0xcc,
						0x55,0xbb,0x66,0xaa,0x77,0x99,0x88,0x88};

void calculate_aes_to_create_static_oob()
{
	#if !WIN32 
	u8 aes_in[16];
	u8 aes_out[16];
	memset(aes_in,0,sizeof(aes_in));
	memcpy(aes_in,tbl_mac,6);
	tn_aes_128(magic_aes_nums,aes_in,aes_out);
	mesh_set_dev_auth(aes_out, 16);
	#endif
}
#endif
void user_sha256_data_proc()
{
    #if (AIS_ENABLE)
    set_sha256_init_para_mode(1);
    #elif LLSYNC_PROVISION_AUTH_OOB
	llsync_tlk_init_three_para_and_mac();
    #endif
}


void user_node_oob_set()
{
	#if CERTIFY_BASE_ENABLE
	if(cert_base_func_init()){
		cert_base_oob_set();
		prov_para.cert_base_en =1;
		// need to set the flag of the certify base flag in the oob part 
		u16 oob_info_data = BIT(OOB_PROV_RECORD)|BIT(OOB_CERT_BASE);
		prov_para.oob_info[0] |= (oob_info_data&0xff);
		prov_para.oob_info[1] |= (oob_info_data>>8);
		return ;
	}else{
		prov_para.cert_base_en =0;
	}	
	return ;
	#endif

    if(AIS_ENABLE || (MESH_USER_DEFINE_MODE == MESH_AES_ENABLE)){
        set_node_prov_para_no_pubkey_static_oob();
    }else {
        #if 0 // output oob test
        //set_node_prov_para_no_pubkey_output_oob();//debug for the bluez 
        #else
        if(0){ // oob test // read from flash later
            set_node_prov_para_no_pubkey_static_oob();  // set static oob type
            u8 oob[16] = {0xff};
			mesh_set_dev_auth(oob,sizeof(oob));
        }else{
			u8 oob[32] = {0};
			if(memcmp(dev_auth, oob, 16)){
				set_node_prov_para_no_pubkey_static_oob();// static oob
			}
			else{
            	set_node_prov_capa_oob_init(); // no oob
			}
        }
        #endif
    }
}


#if MD_SERVER_EN
void user_power_on_proc()
{
    #if ((MESH_USER_DEFINE_MODE != MESH_SPIRIT_ENABLE)&&(MESH_USER_DEFINE_MODE != MESH_TAIBAI_ENABLE)&&!MI_API_ENABLE)
    foreach(i,LIGHT_CNT){
        __UNUSED u16 adr_src = ele_adr_primary + (ELE_CNT_EVERY_LIGHT * i);
        adr_src = adr_src; // will be optimized, just for compile warning.
		mesh_key.netkey_sel_dec = 0; // make sure key is valid when call mesh_tx_cmd_rsp()
		mesh_key.appkey_sel_dec = 0; // make sure key is valid when call mesh_tx_cmd_rsp()
        #if MD_LIGHTNESS_EN
        mesh_tx_cmd_lightness_st(i, adr_src, 0xffff, LIGHTNESS_STATUS, 0, 0);
        #elif MD_LEVEL_EN
        mesh_tx_cmd_g_level_st(i, adr_src, 0xffff, 0, 0); // not support lightness as default
        #elif MD_SENSOR_EN
        // tx nothing
        #elif MD_ONOFF_EN
        mesh_tx_cmd_g_onoff_st(i, adr_src, 0xffff, 0, 0, G_ONOFF_STATUS);   // will send every onoff status
        #endif
    }
    #endif
}
#endif

void user_mesh_cps_init()
{
	if(AIS_ENABLE){
		//gp_page0->head.cid = g_vendor_id;     // have been set default value
		#if !DU_ENABLE
		gp_page0->head.pid = 0;
		gp_page0->head.vid = 0x0001;
		gp_page0->head.crpl = 100;
		#endif
	}else{
	    // use pre-define value
	}
}

void user_set_def_sub_adr()
{
    #if (AIS_ENABLE || LLSYNC_PROVISION_AUTH_OOB)
    	#if LLSYNC_ENABLE
    const u16 group_def_set[] = {LLSYNC_GROUP_ADDR_COMMON};
    	#else
    const u16 group_def_set[] = {0xc000, 0xcfff};
    	#endif
    foreach_arr(i,group_def_set){
        foreach(ele_idx, ELE_CNT){
        	share_model_sub_onoff_server_extend(CFG_MODEL_SUB_ADD, group_def_set[i], 0, ele_adr_primary + ele_idx);
		}
    }
    #endif
}

void user_system_time_proc()
{
#if(AIS_ENABLE && !SPIRIT_PRIVATE_LPN_EN)
	#if !DU_ENABLE
	// in the du mode ,it will not stop send unprovision beacon 
	sha256_dev_uuid_str *p_uuid = (sha256_dev_uuid_str *)prov_para.device_uuid;
	if((p_uuid->adv_flag == 0)&&(clock_time_exceed_s(beacon_send.start_time_s, 10*60))){
		beacon_send.inter = 60*1000*1000;
		p_uuid->adv_flag = 1;
	}
	#endif
#endif
}

//if return 0, will not send transaction ack for link open cmd
int user_node_rc_link_open_callback()
{
	#if (MESH_USER_DEFINE_MODE == MESH_SPIRIT_ENABLE ||MESH_USER_DEFINE_MODE == MESH_TAIBAI_ENABLE)
		#if DU_ENABLE
			#if DU_LPN_EN
			mi_mesh_state_set(1);
			#endif
			update_du_busy_s(60);
		#endif
	sha256_dev_uuid_str *p_uuid = (sha256_dev_uuid_str *)prov_para.device_uuid;
	if(p_uuid->adv_flag){
		return 0;
	}	
	#endif
	return 1;
}

/*********************** static oob data info *********************
for device:
	1. static oob length change from 16 to 32 bytes in the flash due to EPA function.
	2. if the first 16 bytes is all 0xff, it means no oob mode, or it is static oob mode.
	   support both EPA and normal mode when "PROV_EPA EN" is enable.
	   normal mode: use first 16 bytes as static oob.
	   EPA mode:    use 32 bytes of the flash as static oob. 
	3. in the "AIS ENABLE" mode ,it will not support epa mode ,just use normal mode
for provisioner:
	1. use EPA mode only when unprovision device support epa and length of static oob in database is 32 byte.
**********************************************************/
#ifdef FLASH_ADR_STATIC_OOB
u8 mesh_static_oob_data_by_flash()
{
	u8 oob_data[32];
	flash_read_page(FLASH_ADR_STATIC_OOB, 32, oob_data);
	if(!get_flash_data_is_valid(oob_data, 16)){
		// if the first 16 byte is all 0xff, it will be no oob mode .
		memset(dev_auth, 0, 32);
		// will set in the func of the user_node_oob_set
		return 0;
	}
	
	// set the 32 bytes to dev_auth.
	mesh_set_dev_auth(oob_data, sizeof(oob_data));
	return 1;	
}
#endif

void mesh_provision_para_init(u8 *p_random)
{
	mesh_provision_para_reset();
	#if URI_DATA_ADV_ENABLE
	prov_para.oob_info[0]=BIT(OOB_URI);
	prov_para.oob_info[1]=0x00;
	#else
	prov_para.oob_info[0]=0x00;
	prov_para.oob_info[1]=0x00;
	#endif
	provision_mag.pro_stop_flag = 1;// make the provisioner to initial state to stop
	//provision_mag.unicast_adr_last =1;
	prov_para.ele_cnt =1;
	memcpy(prov_para.random,p_random,sizeof(prov_para.random));
	prov_para.rand_gen_s = clock_time_s();
	#if !WIN32
	user_prov_multi_device_uuid();// use the mac address part to create the device uuid part
	#if (!AIS_ENABLE && !LLSYNC_PROVISION_AUTH_OOB && !TESTCASE_FLAG_ENABLE)
	if(mesh_static_oob_data_by_flash()){//oob was burned in flash
		// it means it will enter oob mode .
	}else
	#endif
	{
		user_prov_multi_oob();	
	}
	#endif
	user_node_oob_set();
}

void user_prov_multi_oob()
{
#if !WIN32
    #if (AIS_ENABLE)
		calculate_sha256_to_create_static_oob();
    #elif LLSYNC_PROVISION_AUTH_OOB
		llsync_set_dev_auth();
    #elif (MESH_USER_DEFINE_MODE == MESH_AES_ENABLE)
        calculate_aes_to_create_static_oob();
    #else 
        // use static oob mode 
    #endif
#endif
}

#if (FAST_PROVISION_ENABLE && !WIN32)
STATIC_ASSERT(MD_REMOTE_PROV == 0); // can not enable both, because app can not get the same device uuid calculated from mac.
#endif

/**
 * @brief       This function get device uuid by md5 algorithm.
 * @param[in]	mac	- Mac address
 * @param[out]	uuid- device uuid
 * @return      none
 * @note        
 */
void uuid_create_by_mac(u8 *mac,u8 *uuid)
{
// test md5 function part 
	/*
	static char name_string[]="www.widgets.com";
	uuid_create_md5_from_name((uuid_mesh_t *)uuid, NameSpace_DNS, name_string, 15);
	*/
    uuid_mesh_t NameSpace_DNS = { /* 6ba7b810-9dad-11d1-80b4-00c04fd430c8 */
            0x6ba7b810,
            0x9dad,
            0x11d1,
            0x80, 0xb4, 
            {0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8}
    };
        
	char name_string[16] = {0};
	memcpy(name_string,mac,6);
	uuid_create_md5_from_name((uuid_mesh_t *)uuid, NameSpace_DNS, name_string, 15);

    //special proc to set the mac address into the uuid part 
    #if PAIR_PROVISION_ENABLE
    char head_flag[] = PAIR_PROV_UUID_FLAG;
    memcpy(uuid,head_flag, sizeof(head_flag));
    #elif (!WIN32 && MD_REMOTE_PROV)
	uuid_mesh_t * p_uuid = (uuid_mesh_t * )uuid;
    memcpy(p_uuid->node,mac,6);	// just for showing mac on UI of VC remote scanning.
    #endif
}

#define NORMAL_MODE_DEV_UUID_CUSTOMIZE_EN       (0)

void user_prov_multi_device_uuid()
{
#if !WIN32
    #if (AIS_ENABLE)
        set_dev_uuid_for_sha256();
    #elif (MESH_USER_DEFINE_MODE == MESH_AES_ENABLE)
        uuid_create_by_mac(tbl_mac,prov_para.device_uuid);
    #elif (MESH_USER_DEFINE_MODE == MESH_GN_ENABLE)
        set_dev_uuid_for_simple_flow(prov_para.device_uuid);
    #elif (MESH_USER_DEFINE_MODE == MESH_MI_ENABLE)
        // NO NEED DEV UUID
	#elif LLSYNC_PROVISION_AUTH_OOB
		llsync_mesh_dev_uuid_get(LLSYNC_MESH_UNNET_ADV_BIT, prov_para.device_uuid, sizeof(prov_para.device_uuid));
	#elif PLATFORM_TELINK_EN
		set_dev_uuid_for_platform_telink(prov_para.device_uuid);
    #else // (MESH_USER_DEFINE_MODE == MESH_NORMAL_MODE  and all other)
        if(PROVISION_FLOW_SIMPLE_EN){
		    set_dev_uuid_for_simple_flow(prov_para.device_uuid);
	    }else{
	    	#if (CERTIFY_BASE_ENABLE )
				cert_set_uuid(prov_para.device_uuid);
				return ;
			#endif
			
	        #if NORMAL_MODE_DEV_UUID_CUSTOMIZE_EN   // TBD
            u8 uuid_read[16];
            flash_read_page(FLASH_ADR_DEV_UUID,16,uuid_read);
            if(get_flash_data_is_valid(uuid_read,sizeof(uuid_read))){//device uuid was burned in flash
                memcpy(prov_para.device_uuid, uuid_read, sizeof(prov_para.device_uuid));
            }else
            #endif
            {
			    uuid_create_by_mac(tbl_mac,prov_para.device_uuid);
			}
		}
    #endif
#endif
}

