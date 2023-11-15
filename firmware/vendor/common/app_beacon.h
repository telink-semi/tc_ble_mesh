/********************************************************************************************************
 * @file	app_beacon.h
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
#ifndef APP_BEACON_H
#define APP_BEACON_H
#include "vendor/common/light.h"
#include "proj_lib/sig_mesh/app_mesh.h"
#include "vendor/common/app_privacy_beacon.h"
#include "security_network_beacon.h"
#if WIN32
#pragma pack(1)
#endif

#define URI_DATA    {0x17,0x2f,0x2f,0x50,0x54,0x53,0x2e,0x43,0x4f,0x4d}

typedef struct {
	u8 len;
	u8 type;
}mesh_beacon_header;
typedef struct {
	mesh_beacon_header header;
	u8 beacon_type;
	u8 case_id[4];
	u8 para[4];
}beacon_testcase_pk;
typedef struct {
	mesh_beacon_header header;
	u8 beacon_type;
	u8 device_uuid[16];
	u8 oob_info[2];
}beacon_dat_without_uri_pk;
typedef struct {
	mesh_beacon_header header;
	u8 beacon_type;
	u8 device_uuid[16];
	u8 oob_info[2];
	u8 uri_hash[4];
}beacon_data_pk;
typedef struct {
	mesh_beacon_header header;
	u8 beacon_type;
	u8 flags;
	u8 networkId[8];
	u8 iviIndex[4];
	u8 authValue[8];
}secure_net_pk;
typedef struct {
	bear_head_t tx_head;
	u8 trans_par_val;
	union{
		beacon_dat_without_uri_pk bea_out_uri;
		beacon_data_pk bea_data;
		secure_net_pk  sur_net;
		beacon_testcase_pk bea_testcase_id;
	};
}beacon_str;
typedef struct{
	u32 start_time_s;
	u32  tick;
	u32  inter;
	u8  conn_beacon_flag;
	u8 	en;
}beacon_send_str;
#define MESH_UNPROVISION_BEACON_WITHOUT_URI	0
#define MESH_UNPROVISION_BEACON_WITH_URI	1



typedef enum{
	UNPROVISION_BEACON = 0,
	SECURE_BEACON ,
	PRIVACY_BEACON,
	BEACON_TESTCASE=0x80,
	BEACON_RFU,
}BEACON_ADV_TYPE;
typedef enum{
	OOB_OTHERS = 0,
	OOB_URI	,
	OOB_READ_CODE,
	OOB_BAR_CODE,
	OOB_NFC,
	OOB_NUM,
	OOB_STRING,
	OOB_CERT_BASE,
	OOB_PROV_RECORD,
	OOB_RSV3,
	OOB_RSV4,
	OOB_ON_BOX,
	OOB_IN_BOX,
	OOB_ON_PAPER,
	OOB_IN_MANUAL,
	OOB_ON_DEVICE,
}OOB_INFO_FIELD;

u8  beacon_data_init_without_uri(beacon_str *p_str ,u8 *p_uuid,u8 *p_info);
u8  beacon_data_init_uri(beacon_str *p_str ,u8 *p_uuid,u8 *p_info,u8 *p_hash);
u8  beacon_secure_init(beacon_str *p_str,u8 flags,u8 * p_netId,u8* p_ivIdx,u8* p_auth);
int unprov_beacon_send(u8 mode ,u8 blt_sts);
u8 beacon_test_case(u8*p_tc,u8 len );
int mesh_tx_sec_nw_beacon(mesh_net_key_t *p_nk_base, u8 blt_sts);
int mesh_tx_sec_nw_beacon_all_net(u8 blt_sts);
int iv_update_key_refresh_rx_handle_cb(mesh_ctl_fri_update_flag_t *p_ivi_flag, u32 iv_idx);
void switch_iv_update_time_refresh();

int mesh_beacon_send_proc();
void beacon_str_init();
int check_pkt_is_unprovision_beacon(u8 *dat);
extern _align_4_ beacon_send_str beacon_send;
int mesh_tx_sec_private_beacon_proc(u8 blt_sts);
u8  is_unprovision_beacon_with_uri(event_adv_report_t *report);
int mesh_bear_tx_beacon_adv_channel_only(u8 *bear, u8 trans_par_val);


#endif 

