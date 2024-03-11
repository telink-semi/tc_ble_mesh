/********************************************************************************************************
 * @file	directed_forwarding.h
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
#ifndef DIRECTED_FORWARDING_H
#define DIRECTED_FORWARDING_H
#include "tl_common.h"
#include "proj_lib/sig_mesh/app_mesh.h"

#define DIRECTED_PROXY_EN					FEATURE_PROXY_EN
#define DIRECTED_FRIEND_EN					FEATURE_FRIEND_EN

#define MAX_FIXED_PATH						(PTS_TEST_EN?2:16)
#define MAX_NON_FIXED_PATH					(PTS_TEST_EN?2:64)
#define MAX_DEPENDENT_NUM					(PTS_TEST_EN?1:(2+1))   // 2 + 1: dependent node(lpn and directed client). 

#define MAX_DSC_TBL							(PTS_TEST_EN?4:0x10)
#define MAX_CONCURRENT_CNT					4

#define PATH_REPLY_DELAY_MS					500 // unit:ms
#define PATH_REQUEST_DELAY_MS				150 // unit:ms
//STATIC_ASSERT(MAX_FORWARDING_TBL_ENTRIES_CNT>=20);

#define DIRECTED_CONTROL_GET						0x7B80
#define DIRECTED_CONTROL_SET						0x7C80
#define DIRECTED_CONTROL_STATUS						0x7D80
#define PATH_METRIC_GET								0x7E80
#define PATH_METRIC_SET								0x7F80		
#define PATH_METRIC_STATUS							0x8080
#define DISCOVERY_TABLE_CAPABILITIES_GET  			0x8180
#define DISCOVERY_TABLE_CAPABILITIES_SET  			0x8280
#define DISCOVERY_TABLE_CAPABILITIES_STATUS  		0x8380
#define FORWARDING_TABLE_ADD  						0x8480
#define FORWARDING_TABLE_DELETE  					0x8580
#define FORWARDING_TABLE_STATUS  					0x8680
#define FORWARDING_TABLE_DEPENDENTS_ADD  			0x8780
#define FORWARDING_TABLE_DEPENDENTS_DELETE  		0x8880
#define FORWARDING_TABLE_DEPENDENTS_STATUS			0x8980
#define FORWARDING_TABLE_DEPENDENTS_GET  			0x8A80
#define FORWARDING_TABLE_DEPENDENTS_GET_STATUS  	0x8B80
#define FORWARDING_TABLE_ENTRIES_COUNT_GET  		0x8C80
#define FORWARDING_TABLE_ENTRIES_COUNT_STATUS  		0x8D80
#define FORWARDING_TABLE_ENTRIES_GET  				0x8E80
#define FORWARDING_TABLE_ENTRIES_STATUS  			0x8F80
#define WANTED_LANES_GET							0x9080
#define WANTED_LANES_SET							0x9180
#define WANTED_LANES_STATUS							0x9280
#define TWO_WAY_PATH_GET							0x9380
#define TWO_WAY_PATH_SET							0x9480
#define	TWO_WAY_PATH_STATUS							0x9580
#define PATH_ECHO_INTERVAL_GET						0x9680
#define	PATH_ECHO_INTERVAL_SET						0x9780
#define	PATH_ECHO_INTERVAL_STATUS					0x9880
#define DIRECTED_NETWORK_TRANSMIT_GET				0x9980
#define DIRECTED_NETWORK_TRANSMIT_SET				0x9A80
#define DIRECTED_NETWORK_TRANSMIT_STATUS			0x9B80
#define DIRECTED_RELAY_RETRANSMIT_GET  				0x9C80
#define DIRECTED_RELAY_RETRANSMIT_SET  				0x9D80
#define DIRECTED_RELAY_RETRANSMIT_STATUS  			0x9E80
#define RSSI_THRESHOLD_GET							0x9F80
#define RSSI_THRESHOLD_SET							0xA080
#define RSSI_THRESHOLD_STATUS						0xA180
#define DIRECTED_PATHS_GET							0xA280
#define DIRECTED_PATHS_STATUS						0xA380
#define DIRECTED_PUBLISH_POLICY_GET   				0xA480
#define DIRECTED_PUBLISH_POLICY_SET  				0xA580
#define DIRECTED_PUBLISH_POLICY_STATUS  			0xA680
#define PATH_DISCOVERY_TIMING_CONTROL_GET			0xA780
#define PATH_DISCOVERY_TIMING_CONTROL_SET			0xA880
#define PATH_DISCOVERY_TIMING_CONTROL_STATUS		0xA980
#define DIRECTED_CONTROL_NETWORK_TRANSMIT_GET		0xAB80
#define DIRECTED_CONTROL_NETWORK_TRANSMIT_SET		0xAC80
#define DIRECTED_CONTROL_NETWORK_TRANSMIT_STATUS	0xAD80
#define DIRECTED_CONTROL_RELAY_RETRANSMIT_GET		0xAE80
#define DIRECTED_CONTROL_RELAY_RETRANSMIT_SET		0xAF80
#define DIRECTED_CONTROL_RELAY_RETRANSMIT_STATUS	0xB080

enum{
	NON_FIXED_PATH=0, 
	FIXED_PATH,	
	ANY_PATH,
};

enum{
	DF_IDLE,
	DF_INITIAL,
	DF_POWER_UP,
	DF_PATH_DSC,
	DF_PATH_IN_USE,
	DF_PATH_VALIDAT,
	DF_PATH_MONITOR,
	DF_PATH_DSC_RETRY_WAIT,
};

enum{
	METRIC_TYPE_NODE_COUNT,
	METRIC_TYPE_RFU,
};

enum{
	PATH_LIFETIME_12MINS,
	PATH_LIFETIME_2HOURS,
	PATH_LIFETIME_24HOURS,
	PATH_LIFETIME_10DAYS,
};

enum{
	PATH_DISCOVERY_INTERVAL_5S=5*1000,
	PATH_DISCOVERY_INTERVAL_30S=30*1000,
};

enum{
	LANE_GUARD_INTERVAL_2S=2*1000,
	LANE_GUARD_INTERVAL_10S=10*1000,
};

enum{
	RSSI_MARGIN_PROHIBITED=0x33,
};

enum{
	DEPENDENT_TYPE_REMOVE=0,
	DEPENDENT_TYPE_ADD,	
};

enum{
	ECHO_INVL_DISABLE,
	ECHO_INVL_VALID=0x63,
	ECHO_INVL_NOT_CHANGE=0xff,
};

#define GET_PATH_LIFETIME_MS(lifetime)     ((DF_TEST_MODE_EN ? 2*60:((lifetime==PATH_LIFETIME_12MINS) ? 12*60 : (  \
					                 (lifetime==PATH_LIFETIME_2HOURS) ? 2*60*60 : (  \
					                 (lifetime==PATH_LIFETIME_24HOURS) ? 24*60*60 : 10*24*60*60 \
					                 ))))*1000)

#define GET_PATH_DSC_INTERVAL_MS(dsc_interval)	 		(dsc_interval?PATH_DISCOVERY_INTERVAL_30S:PATH_DISCOVERY_INTERVAL_5S)


#define GET_LANE_GUARD_INTERVAL_MS(guard_interval)		(guard_interval?LANE_GUARD_INTERVAL_10S:LANE_GUARD_INTERVAL_2S)

// directed forwarding message
typedef struct{
	u8 directed_forwarding;
	u8 directed_relay;
	u8 directed_proxy;
	u8 directed_proxy_directed_default;
	u8 directed_friend;
}directed_control_t;

typedef struct{
	u8  metric_type:3;	
	u8  path_lifetime:2;
	u8  rfu:3;
}path_metric_t;

typedef struct{
	s8 default_rssi_threshold;
	s8 rssi_margin;
}rssi_threshold_t;

typedef struct{
	u16 node_paths;
	u16 relay_paths;
	u16 proxy_paths;
	u16 friend_paths;
}directed_paths_t;

typedef struct{
	u16 path_monitoring_interval;
	u16 path_discovery_retry_interval;
	u8 path_discovery_interval:1;
	u8 lane_discovery_guard_interval:1;
	u8 prohibited:6;
}discovery_timing_t;

typedef struct{
	directed_control_t directed_control;
	path_metric_t path_metric;
	u8 max_concurrent_init;
	u8 wanted_lanes;
	u8 two_way_path;
	u8 unicast_echo_interval;
	u8 multicast_echo_interval;
}mesh_directed_subnet_state_t;

typedef struct{
	mesh_directed_subnet_state_t subnet_state[NET_KEY_MAX];
	mesh_transmit_t transmit;
	mesh_transmit_t relay_transmit;
	rssi_threshold_t rssi_threshold;
	directed_paths_t directed_paths;
	discovery_timing_t discovery_timing;
	mesh_transmit_t	control_transmit;
	mesh_transmit_t	control_relay_transmit;
}mesh_directed_forward_t;

typedef struct{
	u16 netkey_index;
	directed_control_t directed_control;	
}directed_control_set_t;

typedef struct{
	u8 status;
	u16 netkey_index;
	directed_control_t directed_control;
}directed_control_sts_t;

typedef struct{
	u16 netkey_index;
	path_metric_t path_metric;
}path_metric_set_t;

typedef struct{
	u8 status;
	u16 netkey_index;
	path_metric_t path_metric;
}path_metric_sts_t;

typedef struct{
	u16 netkey_index;
	u8  max_concurrent_init;
}dsc_tbl_capa_set_t;

typedef struct{
	u8 status;
	u16 netkey_index;
	u8  max_concurrent_init;
	u8  max_dsc_tbl_entries_cnt;
}dsc_tbl_capa_sts_t;

typedef struct{
	u16 netkey_index:12;
	u16 prohibited:2;
	u16 unicast_destination_flag:1;
	u16 backward_path_validated_flag:1;
	u8  par[10]; 
}forwarding_tbl_add_t;

typedef struct{
	u16 netkey_index;
	u16 path_origin;
	u16 destination;
}forwarding_tbl_delete_t;

typedef struct{
	u8  status;
	u16 netkey_index;
	u16 path_origin;
	u16 destination;
}forwarding_tbl_status_t;

typedef struct{
	u16 netkey_index;
	u16 path_origin;
	u16 destination;
	u8  dependent_origin_list_size;
	u8  dependent_target_list_size;
	u8 	par[MAX_DEPENDENT_NUM*sizeof(addr_range_t)*2]; // 2: for dependent origin and target
}forwarding_tbl_dependengts_add_t;

typedef struct{
	u16 netkey_index;
	u16 path_origin;
	u16 destination;
	u8  dependent_origin_list_size;
	u8  dependent_target_list_size;
	u16 addr[1];
}forwarding_tbl_dependengts_delete_t;
 
typedef struct{
	u16 netkey_index:12;
	u16 path_origin_mask:1;
	u16 path_target_mask:1;
	u16 fixed_path_flag:1;
	u16 prohibited:1;
	u16 start_index;
	u16 path_origin;
	u16 destination;
	u16 up_id; // optional
}forwarding_tbl_dependents_get_t;

typedef struct{
	u8 status;
	u16 netkey_index:12;
	u16 path_origin_mask:1;
	u16 path_target_mask:1;
	u16 fixed_path_flag:1;
	u16 prohibited:1;
	u16 start_index;
	u16 path_origin;
	u16 destination;
	u16 up_id;
	u8  dependent_origin_size;
	u8  dependent_target_size;
	u8  range_list[4*MAX_DEPENDENT_NUM*sizeof(addr_range_t)];
}forwarding_tbl_dependents_get_sts_t;

typedef struct{
	u8 status;
	u16 netkey_index;
	u16 update_id;
	u16 fixed_cnt;
	u16 non_fixed_cnt;
}forwarding_tbl_entries_count_st_t;

enum{
	ENTRIES_GET_FIXED_PATH = BIT(0),
 	ENTRIES_GET_NON_FIXED_PATH = BIT(1),
 	ENTRIES_GET_PATH_ORIGIN_MATCH = BIT(2),
 	ENTRIES_GET_DST_MATCH = BIT(3),
};

enum{
	DEPENDENT_LIST_SIZE_ZERO,
	DEPENDENT_LIST_SIZE_ONE,
	DEPENDENT_LIST_SIZE_TWO,
	DEPENDENT_LIST_SIZE_ERR,	
};
	
typedef struct{
	u16 netkey_index:12;
	u16 filter_mask:4;
	u16 start_index;
	u8 par[6]; // origin + target + tbl update id
}forwarding_tbl_entries_get_t;

typedef struct{
	u16 fixed_path_flag:1;
	u16 unicast_destination_flag:1;
	u16 backward_path_validated_flag:1;
	u16 bearer_toward_path_origin_indicator:1;
	u16 bearer_toward_path_target_indicator:1;
	u16 dependent_origin_list_size_indicator:2;
	u16 dependent_target_list_size_indicator:2;
	u16 prohibited:7;
}forwarding_table_entry_head_t;

typedef struct{	
	forwarding_table_entry_head_t entry_head;
	union {
		u16 src_addr;
		addr_range_t path_origin_unicast_addr_range;
	};
	u16 dependent_origin_list_size; 
	u16 bearer_toward_path_origin;
	union {
		u16 dst_addr;			
		addr_range_t path_target_unicast_addr_range;
	};
	u16 dependent_target_list_size;
	u16 bearer_toward_path_target; 
}fixed_path_st_t;

typedef struct{	
	forwarding_table_entry_head_t entry_head;
	u8 lane_counter;
	u16 path_lifetime;
	u8 path_origin_forwarding_number;
	union {
		u16 src_addr;
		addr_range_t path_origin_unicast_addr_range;
	};
	u16 dependent_origin_list_size;
	u16 bearer_toward_path_origin;
	union {
		u16 dst_addr;			
		addr_range_t path_target_unicast_addr_range;
	};
	u16 dependent_target_list_size;
	u16 bearer_toward_path_target; 
}non_fixed_path_st_t;

#define MAX_ENTRY_STS_LEN		100
typedef struct{
	u8 status;
	u16 netkey_index:12;
	u16 filter_mask:4;
	u16 start_index;
	u8 par[MAX_ENTRY_STS_LEN]; //
}forwarding_tbl_entries_st_t;

typedef struct{
	u16 netkey_index;
	u8 wanted_lanes;
}wanted_lanes_set_t;

typedef struct{
	u8 status;
	u16 netkey_index;
	u8 wanted_lanes;
}wanted_lanes_sts_t;

typedef struct{
	u32 netkey_index:16;
	u32 two_way_path:1;
	u32 Prohibited:7;
}two_way_path_set_t;

typedef struct{
	u8 status;
	u32 netkey_index:16;
	u32 two_way_path:1;
	u32 Prohibited:7;
}two_way_path_sts_t;

typedef struct{
	u16 ele_addr;
	u32 model_id;
}directed_pub_policy_get_t;

typedef struct{
	u8 directed_pub_policy;
	u16 ele_addr;
	u32 model_id;
}directed_pub_policy_set_t;

typedef struct{
	u8 status;
	u8 directed_pub_policy;
	u16 ele_addr;
	u32 model_id;
}directed_pub_policy_st_t;

typedef struct{
	u16 netkey_index;
	u8 unicast_echo_interval;
	u8 multicast_echo_interval;
}path_echo_interval_set_t;

typedef struct{
	u8 status;
	u16 netkey_index;
	u8 unicast_echo_interval;
	u8 multicast_echo_interval;
}path_echo_interval_sts_t;

typedef struct{
	u16 monitoring_interval;
	u16 discovery_retry_interval;
	u16 discovery_interval;
	u16 discovery_guard_interval;
}path_discovery_timing_t;

typedef struct{
	u16 addr;
	u8 	snd_ele_cnt;
}path_addr_t;

typedef struct{	// one entry of the forwarding table
	u8  fixed_path:1;
	u8  backward_path_validated:1;
	u8  path_not_ready:1;
	u8  rfu:5;
	u16 path_origin; 
	u8  path_origin_snd_ele_cnt;
	path_addr_t dependent_origin[MAX_DEPENDENT_NUM];
	u16 destination;
	u8  path_target_snd_ele_cnt;
	path_addr_t dependent_target[MAX_DEPENDENT_NUM];
	u16  bearer_toward_path_origin;
	u16  bearer_toward_path_target;
}path_entry_com_t;

typedef struct{
	u8 path_need:1;
	u8 path_monitoring:1;
	u32 lifetime_ms;
	u32 path_echo_timer_ms;
	u32 path_echo_reply_timeout_timer;
	u8  forwarding_number;
	u8  lane_counter;
}non_fixed_entry_state_t;

typedef struct{
	non_fixed_entry_state_t state;
	path_entry_com_t entry;
}non_fixed_entry_t;

#if MD_DF_CFG_SERVER_EN
typedef struct{
	non_fixed_entry_t path[MAX_NON_FIXED_PATH];
//	u16 update_id; // use model_sig_g_df_sbr_cfg.df_cfg.fixed_fwd_tbl[netkey_offset].update_id instead
}non_fixed_fwd_tbl_t;

typedef struct{	
	path_entry_com_t path[MAX_FIXED_PATH];
	u16 update_id;
}fixed_fwd_tbl_t;
#endif

typedef struct{
	u8 path_need;
	u8 path_pending;
	u32 discovery_timer;
	u32 discovery_guard_timer;
	u32 discovery_retry_timer;
	u32 reply_delay_timer;
	u32 request_delay_timer;
	u8 first_reply_msg;
	u8 path_confirm_sent;
	u8 new_lane_established;
	u8 lane_counter;
}discovery_state_t;

typedef struct{
	path_addr_t path_origin;
	path_addr_t dependent_origin;
	u8 forwarding_number;
	u8 prohibited:1;
	u8 path_discovery_interval:1;
	u8 path_lifetime:2;
	u8 path_metric_type:3;
	u8 on_behalf_of_dependent_origin:1;	
	u8 path_metric;
	u16 destination;
	u16 next_toward_path_origin;
	u8 bearer_toward_path_origin;
}discovery_entry_t;

typedef struct{
	discovery_entry_t entry;
	discovery_state_t state;
}discovery_entry_par_t;

#if MD_DF_CFG_SERVER_EN
typedef struct{
	u8 forwarding_number;
	discovery_entry_par_t dsc_entry_par[MAX_DSC_TBL];
}discovery_table_t;

typedef struct{
	mesh_directed_forward_t directed_forward;
	fixed_fwd_tbl_t fixed_fwd_tbl[NET_KEY_MAX];
}model_df_cfg_t;
#endif

extern int path_monitoring_test_mode;
void mesh_df_led_event(u8 nid);
int is_directed_forwarding_en(u16 netkey_offset);
int is_directed_relay_en(u16 netkey_offset);
int is_directed_proxy_en(u16 netkey_offset);
int is_directed_friend_en(u16 netkey_offset);
int is_directed_forwarding_op(u16 op);
int is_path_target(u16 origin, u16 destination);
u8 get_directed_proxy_dependent_ele_cnt(u16 netkey_offset, u16 addr);
u8 get_directed_friend_dependent_ele_cnt(u16 netkey_offset, u16 addr);
int is_proxy_use_directed(u16 netkey_offset);
void directed_proxy_dependent_node_delete();
void mesh_directed_forwarding_bind_state_update();
void mesh_directed_forwarding_default_val_init();
int mesh_directed_proxy_capa_report(int netkey_offset);
int mesh_directed_proxy_capa_report_upon_connection();
path_entry_com_t *get_forwarding_entry(u16 netkey_offset, u16 path_origin, u16 destination);
int mesh_df_path_monitoring(path_entry_com_t *p_entry);
int directed_forwarding_initial_start(u16 netkey_index, u16 destination, u16 dependent_addr, u16 dependent_ele_cnt);
int directed_forwarding_dependents_update_start(u16 netkey_offset, u8 type, u16 path_endpoint, u16 dependent_addr, u8 dependent_ele_cnt);
void mesh_directed_forwarding_proc(u8 *p_bear, u8 *par, int par_len, int src_type);
int is_address_in_dependent_origin(path_entry_com_t *p_fwd_entry, u16 addr);
int is_address_in_dependent_target(path_entry_com_t *p_fwd_entry, u16 addr);
int forwarding_tbl_dependent_add(u16 range_start, u8 range_length, path_addr_t *p_dependent_list);
int directed_forwarding_solication_start(u16 netkey_offset, mesh_ctl_path_request_solication_t *p_addr_list, u8 list_num);

discovery_entry_par_t * get_discovery_entry_correspond2_path_request(u16 netkey_offset, u16 path_origin, u8 forwarding_number);
int cfg_cmd_send_path_request(mesh_ctl_path_req_t *p_path_req, u8 len, u16 netkey_offset);
int cfg_cmd_path_metric_get(u16 node_adr, u16 nk_idx);
int cfg_cmd_path_metric_set(u16 node_adr, u16 nk_idx, u8 metric_type, u8 lifetime);
int cfg_cmd_send_directed_control_set(u16 adr_dst, directed_control_set_t *directed_control);
int cfg_cmd_send_directed_control_get(u16 adr_dst, u16 netkey_index);
int cfg_cmd_max_concur_init_get(u16 node_adr, u16 nk_idx);
int cfg_cmd_max_concur_init_set(u16 node_adr, u16 nk_idx, u8 max_concur_init);
int cfg_cmd_forwarding_tbl_add(u16 node_adr, u16 nk_idx, u16 adr_origin, u8 range_len_origin, u16 adr_dst, u8 range_len_dst, u16 bear2_origin, u16 bear2_dst);
int cfg_cmd_forwarding_tbl_dependents_add(u16 node_adr, u16 nk_idx, u16 path_origin, u16 path_target, int dpt_origin_num, int dpt_target_num, u8 dpt_tbl[], int tbl_size);
int cfg_cmd_forwarding_tbl_entries_count_get(u16 node_adr, u16 nk_idx);
int cfg_cmd_forwarding_tbl_entries_get(u16 node_adr, u8 *par, int len);
int cfg_cmd_forwarding_tbl_dependents_get(u16 node_adr, u8 *par, int len);
int cfg_cmd_forwarding_tbl_delete(u16 node_adr, u16 nk_idx, u16 path_origin, u16 destination);
int cfg_cmd_wanted_lanes_get(u16 node_adr, u16 nk_idx);
int cfg_cmd_wanted_lanes_set(u16 node_adr, u16 nk_idx, u8 wanted_lanes);
int cfg_cmd_two_way_path_get(u16 node_adr, u16 nk_idx);
int cfg_cmd_two_way_path_set(u16 node_adr, u16 nk_idx, u8 two_way_path);
int cfg_cmd_path_echo_interval_get(u16 node_adr, u16 nk_idx);
int cfg_cmd_path_echo_interval_set(u16 node_adr, u16 nk_idx, u8 unicast_invl, u8 multi_invl);
int cfg_cmd_directed_network_transmit_get(u16 node_adr);
int cfg_cmd_directed_network_transmit_set(u16 node_adr, u8 directed_transmit);
int cfg_cmd_directed_relay_retransmit_get(u16 node_adr);
int cfg_cmd_directed_relay_retransmit_set(u16 node_adr, u8 retransmit);
int cfg_cmd_rssi_threshold_get(u16 node_adr);
int cfg_cmd_rssi_threshold_set(u16 node_adr, s8 rssi_margin);
int cfg_cmd_directed_path_get(u16 node_adr);
int cfg_cmd_directed_publish_policy_get(u16 node_adr, u16 ele_addr, u32 model_id);
int cfg_cmd_directed_publish_policy_set(u16 node_adr, u8 policy, u16 ele_addr, u32 model_id);
int cfg_cmd_path_dsc_timing_ctl_get(u16 node_adr);	
int cfg_cmd_path_dsc_timing_ctl_set(u16 node_adr, u16 monitor_interval, u16 dsc_retry_interval, bool dsc_interval, bool dsc_guard_interval);
int cfg_cmd_directed_ctl_network_transmit_get(u16 node_adr);
int cfg_cmd_directed_ctl_network_transmit_set(u16 node_adr, u8 transmit);
int cfg_cmd_directed_ctl_relay_retransmit_get(u16 node_adr);
int cfg_cmd_directed_ctl_relay_retransmit_set(u16 node_adr, u8 transmit);

#if (MD_DF_CFG_SERVER_EN && !WIN32)
int mesh_cmd_sig_cfg_directed_control_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_directed_control_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
#else
#define mesh_cmd_sig_cfg_directed_control_get							(0)	
#define mesh_cmd_sig_cfg_directed_control_set							(0)	
#endif
 
 #if (MD_DF_CFG_SERVER_EN&&!FEATURE_LOWPOWER_EN&&!WIN32)
int mesh_cmd_sig_cfg_path_metric_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_path_metric_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_dsc_tbl_capa_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_dsc_tbl_capa_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_forwarding_tbl_add(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_forwarding_tbl_delete(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_forwarding_tbl_dependents_add(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_forwarding_tbl_dependents_delete(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_forwarding_tbl_dependents_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_forwarding_tbl_entries_count_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_forwarding_tbl_entries_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_wanted_lanes_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_wanted_lanes_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_two_way_path_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_two_way_path_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_path_echo_interval_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_path_echo_interval_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_directed_network_transmit_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_directed_network_transmit_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_directed_relay_retransmit_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_directed_relay_retransmit_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_rssi_threshold_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_rssi_threshold_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_directed_paths_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_directed_publish_policy_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_directed_publish_policy_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_path_discovery_timing_control_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_path_discovery_timing_control_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_directed_control_network_transmit_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_directed_control_network_transmit_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_directed_control_relay_transmit_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_directed_control_relay_transmit_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
#else
//#define mesh_cmd_sig_cfg_directed_control_set							(0)	
#define mesh_cmd_sig_cfg_path_metric_get								(0)
#define mesh_cmd_sig_cfg_path_metric_set								(0)
#define mesh_cmd_sig_cfg_dsc_tbl_capa_get								(0)
#define mesh_cmd_sig_cfg_dsc_tbl_capa_set								(0)
#define mesh_cmd_sig_cfg_forwarding_tbl_add								(0)		
#define mesh_cmd_sig_cfg_forwarding_tbl_delete							(0)
#define mesh_cmd_sig_cfg_forwarding_tbl_dependents_add					(0)
#define mesh_cmd_sig_cfg_forwarding_tbl_dependents_delete				(0)
#define mesh_cmd_sig_cfg_forwarding_tbl_dependents_get					(0)
#define mesh_cmd_sig_cfg_forwarding_tbl_entries_count_get				(0)
#define mesh_cmd_sig_cfg_forwarding_tbl_entries_get						(0)
#define mesh_cmd_sig_cfg_wanted_lanes_get								(0)
#define mesh_cmd_sig_cfg_wanted_lanes_set								(0)
#define mesh_cmd_sig_cfg_two_way_path_get								(0)
#define mesh_cmd_sig_cfg_two_way_path_set								(0)		
#define mesh_cmd_sig_cfg_path_echo_interval_get							(0)
#define mesh_cmd_sig_cfg_path_echo_interval_set							(0)
#define mesh_cmd_sig_cfg_directed_network_transmit_get					(0)
#define mesh_cmd_sig_cfg_directed_network_transmit_set					(0)
#define mesh_cmd_sig_cfg_directed_relay_retransmit_get					(0)
#define mesh_cmd_sig_cfg_directed_relay_retransmit_set					(0)
#define mesh_cmd_sig_cfg_rssi_threshold_get								(0)
#define mesh_cmd_sig_cfg_rssi_threshold_set								(0)
#define mesh_cmd_sig_cfg_directed_paths_get								(0)
#define mesh_cmd_sig_cfg_directed_publish_policy_get					(0)
#define mesh_cmd_sig_cfg_directed_publish_policy_set					(0)
#define mesh_cmd_sig_cfg_path_discovery_timing_control_get				(0)
#define mesh_cmd_sig_cfg_path_discovery_timing_control_set				(0)
#define mesh_cmd_sig_cfg_directed_control_network_transmit_get			(0)
#define mesh_cmd_sig_cfg_directed_control_network_transmit_set			(0)
#define mesh_cmd_sig_cfg_directed_control_relay_transmit_get			(0)
#define mesh_cmd_sig_cfg_directed_control_relay_transmit_set			(0)
#endif

#if MD_DF_CFG_CLIENT_EN
int mesh_cmd_sig_cfg_directed_control_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_path_metric_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_dsc_tbl_capa_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_forwarding_tbl_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_forwarding_tbl_dependents_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_forwarding_tbl_dependents_get_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_forwarding_tbl_entries_count_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_forwarding_tbl_entries_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_wanted_lanes_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_two_way_path_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_path_echo_interval_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_directed_network_transmit_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_directed_relay_retransmit_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_rssi_threshold_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_directed_paths_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_directed_publish_policy_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_path_discovery_timing_control_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_directed_control_network_transmit_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_directed_control_relay_transmit_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
#else
#define mesh_cmd_sig_cfg_directed_control_status						(0)
#define mesh_cmd_sig_cfg_path_metric_status								(0)
#define mesh_cmd_sig_cfg_dsc_tbl_capa_status							(0)
#define mesh_cmd_sig_cfg_forwarding_tbl_status							(0)
#define mesh_cmd_sig_cfg_forwarding_tbl_dependents_status				(0)	
#define mesh_cmd_sig_cfg_forwarding_tbl_dependents_get_status 			(0)
#define mesh_cmd_sig_cfg_forwarding_tbl_entries_count_status			(0)
#define mesh_cmd_sig_cfg_forwarding_tbl_entries_status					(0)
#define mesh_cmd_sig_cfg_wanted_lanes_status							(0)
#define mesh_cmd_sig_cfg_two_way_path_status							(0)
#define mesh_cmd_sig_cfg_path_echo_interval_status						(0)
#define mesh_cmd_sig_cfg_directed_network_transmit_status				(0)
#define mesh_cmd_sig_cfg_directed_relay_retransmit_status				(0)	
#define mesh_cmd_sig_cfg_rssi_threshold_status				 			(0)
#define mesh_cmd_sig_cfg_directed_paths_status							(0)
#define mesh_cmd_sig_cfg_directed_publish_policy_status					(0)
#define mesh_cmd_sig_cfg_path_discovery_timing_control_status 			(0)
#define mesh_cmd_sig_cfg_directed_control_network_transmit_status		(0)
#define mesh_cmd_sig_cfg_directed_control_relay_transmit_status			(0)
#endif

static inline non_fixed_entry_t *GET_NON_FIXED_ENTRY_POINT(u8 *p){
	return (non_fixed_entry_t *)(p - OFFSETOF(non_fixed_entry_t,entry));
}

#endif
