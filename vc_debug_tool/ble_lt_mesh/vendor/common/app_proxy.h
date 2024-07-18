/********************************************************************************************************
 * @file	app_proxy.h
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
#ifndef APP_PROXY_H
#define APP_PROXY_H
#include "app_provison.h"
#include "vendor/common/light.h"
#include "proj_lib/sig_mesh/app_mesh.h"

#define MAX_LIST_LEN 	16

#define ADV_PROXY_TYPE_NET_ID	0
#define ADV_PROXY_TYPE_NODE_IDENTTITY	1
typedef struct {
	u8 flag_len;
	u8 flag_type;
	u8 flag_data;
	u8 uuid_len;
	u8 uuid_type;
	u8 uuid_data[2];
	u8 serv_len;
	u8 serv_type;
	u8 serv_uuid[2];
	u8 identify_type;
	u8 net_id[8];
}proxy_adv_net_id;

typedef struct {
	u8 flag_len;
	u8 flag_type;
	u8 flag_data;
	u8 uuid_len;
	u8 uuid_type;
	u8 uuid_data[2];
	u8 serv_len;
	u8 serv_type;
	u8 serv_uuid[2];
	u8 identify_type;
	u8 hash[8];
	u8 random[8];
}proxy_adv_node_identity;

typedef enum{
	NETWORK_ID_TYPE=0,
	NODE_IDENTITY_TYPE,
#if MD_PRIVACY_BEA
	PRIVATE_NETWORK_ID_TYPE,
	PRIVATE_NODE_IDENTITY_TYPE,
#endif
	NODE_NO_PROXY_ADVERTISING
}node_identity_enum_t;

typedef enum{
	COMPLETE_MSG = 0,
	FIRST_SEG_MSG,
	CONTINUE_SEG_MSG,
	LAST_SEG_MSG,
}PROXY_DATA_SAR_TYPE;
	
typedef enum{
	PROXY_NET_WORK=0,
	PROXY_MESH_BEACON,
	PROXY_CONFIG,	
	PROXY_PROVISION,
	PROXY_RFU,
}PROXY_MSG_TYPE;
	
#define MAX_PROXY_RSV_LEN 	30
typedef struct{
	u8 trans_par_val;
	u8 type:6;
	u8 sar:2;
	u8 data[MAX_PROXY_RSV_LEN];
}proxy_msg_str;

typedef struct{
	u8 opcode;
	u8 para[50];
}proxy_config_pdu_sr;

typedef enum{
	PROXY_FILTER_SET_TYPE =0,
	PROXY_FILTER_ADD_ADR,
	PROXY_FILTER_RM_ADR,
	PROXY_FILTER_STATUS,
	DIRECTED_PROXY_CAPA_STATUS,
	DIRECTED_PROXY_CONTROL,
}proxy_opcode_str;
	
typedef enum{
	FILTER_WHITE_LIST = 0,
	FILTER_BLACK_LIST,
	FILTER_RFU,
}filter_type_str;

enum{
	UNSET_CLIENT,
	PROXY_CLIENT,
	DIRECTED_PROXY_CLIENT,
	BLACK_LIST_CLIENT,
};

enum{
	USE_DIRECTED_ENABLE,
	USE_DIRECTED_DISABLE,
	USE_DIRECTED_PROHIBITED,
};

typedef struct{
	u8 use_directed;
	u16 client_addr;
	u8 	client_2nd_ele_cnt;
}direct_proxy_server_t;

typedef struct{
	u8 filter_type;
	u16 addr_list[MAX_LIST_LEN];
	#if MD_DF_CFG_SERVER_EN
	u8 proxy_client_type;
	direct_proxy_server_t directed_server[NET_KEY_MAX];
	#endif
}proxy_config_mag_str;
	
typedef struct{
	u8 fil_type;
	u16 list_size;
}mesh_filter_sts_str;

typedef struct{
	u16 conn_handle;
//	u16 att_handle;
	u16 sar_len;
	u8 err_flag ;
	u32 sar_tick ; // 0:reassemble idle/complete, non-0:reassemble busy.
}mesh_proxy_protocol_sar_t;
extern _align_4_ mesh_proxy_protocol_sar_t  proxy_sar[ACL_PERIPHR_MAX_NUM];

// proxy sar function part 
void mesh_proxy_sar_para_init(u8 idx);
void mesh_proxy_sar_start(u8 idx);
void mesh_proxy_sar_continue(u8 idx);
void mesh_proxy_sar_end(u8 idx);
void mesh_proxy_sar_complete(u8 idx);
void mesh_proxy_sar_err_terminate(u8 idx);
void mesh_proxy_sar_timeout_terminate();
int pkt_pb_gatt_data(int idx, rf_packet_att_data_t *p, u8 l2cap_type);
void mesh_proxy_sar_timeout_terminate();

u8 proxy_config_dispatch(mesh_cmd_bear_t *p_bear, u8 len);
extern u8 proxy_filter_change_by_mesh_net(u16 unicast_adr);

extern u8 set_proxy_adv_pkt(u8 *p ,u8 *pRandom,mesh_net_key_t *p_netkey,u8*p_len);

extern _align_4_ proxy_config_mag_str proxy_mag[];
int is_app_addr(u16 addr);
int is_valid_addr_in_proxy_list(int idx, u16 addr);
extern int is_valid_adv_with_proxy_filter(u16 dst_addr);
int is_proxy_client_addr(u16 addr);
int get_directed_proxy_client_idx(u16 addr, u8 *netkey_offset);
extern void proxy_cfg_list_init_upon_connection(u16 conn_handle);
extern void set_proxy_initial_mode(u8 special_mode);
extern void set_pair_login_ok(u8 val);
extern u8 proxy_proc_filter_mesh_cmd(int idx, u16 src);
void calculate_proxy_adv_hash(mesh_net_key_t *p_netkey );

int proxy_adv_calc_with_node_identity(u8 random[8],u8 node_key[16],u16 ele_adr,u8 hash[8]);
int proxy_adv_calc_with_private_net_id(u8 random[8],u8 net_id[8],u8 idk[16],u8 hash[8]);
int proxy_adv_calc_with_private_node_identity(u8 random[8],u8 node_key[16],u16 ele_adr,u8 hash[8]);

extern u8 proxy_Out_ccc[2];
extern u8 proxy_In_ccc[2];
extern u8 provision_In_ccc[2];
extern u8 provision_Out_ccc[2];
void reset_all_ccc();
#if BLE_MULTIPLE_CONNECTION_ENABLE
int pb_gatt_provision_out_ccc_cb(u16 connHandle, void *p);
int	pb_gatt_Write(u16 connHandle, void *p);
int proxy_out_ccc_cb(u16 connHandle, void *p);
int proxy_gatt_Write(u16 connHandle, void *p);
#else
int pb_gatt_provision_out_ccc_cb(void *p);
int	pb_gatt_Write (void *p);
int proxy_out_ccc_cb(void *p);
int proxy_gatt_Write(void *p);
#endif
u8 mesh_get_proxy_privacy_para();

#endif 

