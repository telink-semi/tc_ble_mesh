/********************************************************************************************************
 * @file     sig_mesh_json_info.h 
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
#ifndef _SIG_MESH_JSON_INFO_H_
#define _SIG_MESH_JSON_INFO_H_
#include "../../ble_lt_mesh/proj/common/types.h"
#include "../../ble_lt_mesh/vendor/common/app_provison.h"
#define JSON_FILE_ENABLE 	1
#define JSON_DATABASE_STRING_MAX	100
#define MAX_NETKEY_TOTAL_NUM	400
#define MAX_APPKEY_TOTAL_NUM	400
#define MAX_PROVISION_NUM 		200
#define MAX_MESH_NODE_NUM		3000
#define MAX_MESH_NODE_ELE_NUM	4

#if (DEBUG_CFG_CMD_GROUP_AK_EN)
#define GATEWAY_300_NODES_TEST_ENABLE	1
#else
#define GATEWAY_300_NODES_TEST_ENABLE	0
#endif

typedef struct{
	u8 valid;
	char name[JSON_DATABASE_STRING_MAX];
	int index;
	u8 phase;
	u8 key[16];
	char minSecurity[JSON_DATABASE_STRING_MAX];
	u8 oldkey[16];
	u32 timestamps;
}mesh_json_netkeys_str;

typedef struct{
	u8 valid;
	char name[JSON_DATABASE_STRING_MAX];
	int index;
	int bound_netkey;
	u8 key[16];
	u8 oldKey[16];
}mesh_json_appkeys_str;

typedef struct{
	u16 highAddress;
	u16 lowAddress;
}mesh_json_pro_GroupRange;
#define MAX_PRO_GROUP_RANGE_MAX 	100

typedef struct{
	u16 lowAddress;
	u16 highAddress;
}mesh_json_pro_UnicastRange;

typedef struct{
	u16 firstScene;
	u16 lastScene;
}mesh_json_pro_SceneRange;
#if GATEWAY_300_NODES_TEST_ENABLE
#define JSON_PROV_UNICAST_RANGE 0x7ff
#else
#define JSON_PROV_UNICAST_RANGE 0x3ff
#endif
typedef struct{
	u8 valid;
	char provisionerName[JSON_DATABASE_STRING_MAX];
	u8 uuid[16];
	mesh_json_pro_UnicastRange unicastRange[MAX_PRO_GROUP_RANGE_MAX];
	mesh_json_pro_GroupRange groupRange[MAX_PRO_GROUP_RANGE_MAX];
	mesh_json_pro_SceneRange sceneRange[MAX_PRO_GROUP_RANGE_MAX];
}mesh_provisioners_str;

#define MAX_GROUP_NUM 	400
typedef struct{
	u8 valid;
	char name[JSON_DATABASE_STRING_MAX];
	u16 address;
	u16 parentAddress;
}mesh_group_str;

#define MAX_SCENE_NUM 	MAX_GROUP_NUM
typedef struct{
	u8 valid;
	char name[JSON_DATABASE_STRING_MAX];
	u16 addresses[MAX_GROUP_NUM];
	u16 number;
}mesh_scene_str;

// nodes part 
typedef struct{
	u8 relay;
	u8 proxy;
	u8 fri;
	u8 lowpower;
}mesh_nodes_feature_str;

typedef struct{
	u8 count;
	u32 interval;
}mesh_nodes_networkTransmit_str;

typedef struct{
	u16 address;
	int index;
	u8 ttl;
	u8 numberOfSteps;
	u32 resolution;
	u8 credentials;
	u8 count;
	u32 interval;
}mesh_models_publish_str;

#define MAX_SUBSCRIBE_NUM	8
#define MAX_BIND_ADDRESS_NUM	10
typedef struct{
	u8 valid;
	u16 bind;
}mesh_model_bind_str;

typedef struct{
	u8 valid;
	u8 vendor_flag;
	int modelId;
	u16 subscribe[MAX_SUBSCRIBE_NUM];
	mesh_model_bind_str bind[MAX_BIND_ADDRESS_NUM];
	mesh_models_publish_str publish;
}mesh_nodes_model_str;

#define MAX_MODELS_IN_ELE_CNT	80
typedef struct{
	u8 valid;
	char name[JSON_DATABASE_STRING_MAX];
	int index;
	u16 location;
	mesh_nodes_model_str models[MAX_MODELS_IN_ELE_CNT];
}mesh_nodes_ele_str;

typedef struct{
	u8 count;
	u32 interval;
}mesh_nodes_relay_str;

typedef struct{
	u8 valid;
	u16 index;
	u8 update;
}mesh_nodes_net_appkey_str;
typedef struct{
	u8 valid;
	u8 uuid[16];
	u16 unicastAddress;
	u8 deviceKey[16];
	char security[JSON_DATABASE_STRING_MAX];
	mesh_nodes_net_appkey_str netkeys[MAX_NETKEY_TOTAL_NUM];
	u8 configComplete;
	char name[JSON_DATABASE_STRING_MAX];
	u16 cid;
	u16 pid;
	u16 vid;
	u16 crpl;
	mesh_nodes_feature_str features;
	u8  secureNetworkBeacon;
	u8  defaultTTL;
	mesh_nodes_networkTransmit_str networkTransmit;
	mesh_nodes_relay_str relayRetransmit;
	mesh_nodes_net_appkey_str appkeys[MAX_APPKEY_TOTAL_NUM];
	mesh_nodes_ele_str elements[MAX_MESH_NODE_ELE_NUM];
	u8 excluded;
	u32 sno;
	u8 macAddress[6];
}mesh_node_str;

typedef struct{
	u8 valid;
	u8 uuid[16];
	u16 unicastAddress;
	u8 deviceKey[16];
	char security[JSON_DATABASE_STRING_MAX];
	mesh_nodes_net_appkey_str netkeys[MAX_NETKEY_TOTAL_NUM];
	u8 configComplete;
	char name[JSON_DATABASE_STRING_MAX];
	u16 cid;
	u16 pid;
	u16 vid;
	u16 crpl;
	mesh_nodes_feature_str features;
	u8  secureNetworkBeacon;
	u8  defaultTTL;
	mesh_nodes_networkTransmit_str networkTransmit;
	mesh_nodes_relay_str relayRetransmit;
	mesh_nodes_net_appkey_str appkeys[MAX_APPKEY_TOTAL_NUM];
	mesh_nodes_ele_str elements[MAX_MESH_NODE_ELE_NUM];
	u8 excluded;
}mesh_node_static_str;


typedef struct{
	char schema[JSON_DATABASE_STRING_MAX];
	char version[JSON_DATABASE_STRING_MAX];
	char id[JSON_DATABASE_STRING_MAX];
	u8 mesh_uuid[16];
	char meshName[JSON_DATABASE_STRING_MAX];
	char timestamp[JSON_DATABASE_STRING_MAX];
	u8 partial;
	mesh_provisioners_str 	provisioners[MAX_PROVISION_NUM];
	mesh_json_netkeys_str 	netKeys[MAX_NETKEY_TOTAL_NUM];
	mesh_json_appkeys_str 	appkeys[MAX_APPKEY_TOTAL_NUM];
	mesh_group_str			groups[MAX_GROUP_NUM];
	mesh_scene_str  		scene[MAX_SCENE_NUM];
	mesh_node_str			nodes[MAX_MESH_NODE_NUM];
	u8 ivi_idx[4];
}sig_mesh_json_database;

#define JSON_PTS_ENABLE	0

#if JSON_PTS_ENABLE
#define JSON_IVI_INDEX_ENABLE 	0
#else
#define JSON_IVI_INDEX_ENABLE 	1
#endif


typedef struct{
	char schema[JSON_DATABASE_STRING_MAX];
	char version[JSON_DATABASE_STRING_MAX];
	char id[JSON_DATABASE_STRING_MAX];
	u8 mesh_uuid[16];
	char meshName[JSON_DATABASE_STRING_MAX];
	char timestamp[JSON_DATABASE_STRING_MAX];
	u8 partial;
	mesh_provisioners_str 	provisioners[MAX_PROVISION_NUM];
	mesh_json_netkeys_str 	netKeys[MAX_NETKEY_TOTAL_NUM];
	mesh_json_appkeys_str 	appkeys[MAX_APPKEY_TOTAL_NUM];
	mesh_group_str			groups[MAX_GROUP_NUM];
	mesh_scene_str  		scene[MAX_SCENE_NUM];
	mesh_node_static_str	nodes[MAX_MESH_NODE_NUM];
	#if JSON_IVI_INDEX_ENABLE
	u8 ivi_idx[4];
	#endif
}sig_mesh_json_database_static;


extern u8 json_unicast_is_in_range_or_not(u16 unicast,u8*p_uuid);
extern sig_mesh_json_database json_database_check;
extern sig_mesh_json_database json_database;
extern sig_mesh_json_database_static json_db_static;

extern int json_add_net_info_doc(u8 *p_guid,u8 *p_mac);
extern int json_set_model_doc(mesh_node_str *p_node);
extern int  prov_get_net_info_from_json(provison_net_info_str *net_info,int *p_net_idx,int node_idx);
extern int update_VC_info_from_json(int net_idx,VC_node_info_t *p_vc_node);
extern int provision_self_to_json(provison_net_info_str *net_info,int *p_net_idx,u8 *p_guid,u8 *p_devkey);
extern int bind_self_to_json(u16 net_idx,u8 *p_app_key,u16 app_idx);
extern int json_add_provisioner_info(provison_net_info_str *p_net_info,u8 *p_uuid);
extern int mesh_json_update_ivi_index(u8 *p_index);
extern int  provision_find_json_node_by_uuid(u8 *p_uuid);
extern int get_app_key_idx_by_net_idx(u16 net_idx,provision_appkey_t *p_appkey);
mesh_node_str* json_mesh_find_node(u16 unicast);
mesh_nodes_model_str *json_mesh_find_model(mesh_node_str* p_node,u16 ele_adr,int model_id);
u8 mesh_json_appkey_add(mesh_node_str* p_node,u8 *app_key,u16 net_idx,u16 app_idx);
u8 json_model_app_bind(mesh_nodes_model_str *p_model,u16 app_idx);

extern int json_find_valid_prov();
extern u8 mesh_json_set_node_info(u16 adr,u8 *p_mac);
int json_find_first_empty_node_idx();
u8 * json_get_netkey_by_idx(u16 idx);
int json_use_appkey_get_netkey(u16 app_key_idx);
u8 *json_use_appkey_get_key_val(u16 app_key_idx);
u8 *json_get_first_prov_uuid();
u16 json_use_uuid_to_get_unicast(u8 *p_uuid);
u8 json_proc_appkey_self(u16 appkey_idx,u8 *p_uuid);
int json_set_appkey_bind_self_proc(u8 *p_uuid,u16 appkey_idx);
mesh_node_str * json_get_mesh_node(u8 *p_uuid);
u8 json_get_node_cps_info(u16 unicast_adr,VC_node_info_t *p_node);
u16 json_del_mesh_info(u8 *p_uuid);
int json_get_next_unicast_adr_val(u8 *p_uuid);
u8 * json_get_uuid(u16 src);
int database_static_to_normal();
int database_normal_to_static();
u32 json_use_uuid_to_get_sno(u8 *p_uuid) ;
u32 json_use_uuid_to_set_sno(u8 *p_uuid,u32 sno);

u16 json_del_mesh_vc_node_info(u8 *p_uuid);
u8 json_update_node_adr_devicekey(u16 new_adr,u16 old_adr,u8* devkey);

#endif
