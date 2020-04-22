#ifndef DIRECTED_FORWARDING_H
#define DIRECTED_FORWARDING_H
#include "../../proj/tl_common.h"
#include "../../proj_lib/sig_mesh/app_mesh.h"

#define DIRECTED_PROXY_EN					FEATURE_PROXY_EN
#define DIRECTED_FRIEND_EN					0//FEATURE_FRIEND_EN

#define MAX_FORWARDING_TBL_ENTRIES_CNT		0x40 // at least 20 , for ram debug now
#define MAX_DISCOVERY_TBL_ENTRIES_CNT		0x10
#define MAX_CONCURRENT_CNT					4
#define MAX_DEPENDENT_LIST_SIZE				MAX_LPN_NUM
#define PATH_REPLY_DELAY_MS					500 // unit:ms
#define PATH_REQUEST_DELAY_MS				150 // unit:ms
//STATIC_ASSERT(MAX_FORWARDING_TBL_ENTRIES_CNT>=20);

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
	DIRECTED_FORWARDING_DISABLE,
	DIRECTED_FORWARDING_ENABLE,	
	DIRECTED_FORWARDING_PROHIBIT,
};

enum{
	DIRECTED_RELAY_DISABLE,
	DIRECTED_RELAY_ENABLE,	
	DIRECTED_RELAY_PROHIBIT,
};

enum{
	DIRECTED_PROXY_DISABLE,
	DIRECTED_PROXY_ENABLE,
	DIRECTED_PROXY_NOT_SUPPORT,
	DIRECTED_PROXY_PROHIBIT,
	DIRECTED_PROXY_IGNORE = 0xff,
};

enum{
	DIRECTED_PROXY_USE_DEFAULT_DISABLE,
	DIRECTED_PROXY_USE_DEFAULT_ENABLE,
	DIRECTED_PROXY_USE_DEFAULT_NOT_SUPPORT,
	DIRECTED_PROXY_USE_DEFAULT_IGNORE = 0xff,
};

enum{
	DIRECTED_FRIEND_DISABLE,
	DIRECTED_FRIEND_ENABLE,
	DIRECTED_FRIEND_NOT_SUPPORT,
	DIRECTED_FRIEND_IGNORE = 0xff,
};

typedef enum{
	BEAR_UNASSIGNED = 0,
	BEAR_ADV = BIT(0),
	BEAR_GATT = BIT(1),
}bear_index_t;

#define BEAR_SUPPORT	(BEAR_ADV|BEAR_GATT)

enum{
	METRIC_TYPE_NODE_COUNT,
};

enum{
	PATH_LIFETIME_12MINS,
	PATH_LIFETIME_2HOURS,
	PATH_LIFETIME_24HOURS,
	PATH_LIFETIME_10DAYS,
};

#define GET_PATH_LIFETIME(lifetime)     ((lifetime==PATH_LIFETIME_12MINS) ? 12*60 : (  \
					                 (lifetime==PATH_LIFETIME_2HOURS) ? 2*60*60 : (  \
					                 (lifetime==PATH_LIFETIME_24HOURS) ? 24*60*60 : 10*24*60*60 \
					                 )))

enum{
	PATH_DISCOVERY_INTERVAL_5S=5,
	PATH_DISCOVERY_INTERVAL_30S=30,
};

enum{
	LANE_GUARD_INTERVAL_2S=2,
	LANE_GUARD_INTERVAL_10S=10,
};

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
	u8  par[1];
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
	u8 	par[1];
}forwarding_tbl_dependengts_add_t;

typedef struct{
	u16 netkey_index;
	u16 path_origin;
	u16 destination;
	u8  dependent_origin_list_size;
	u8  dependent_target_list_size;
	u16 addr[1];
}forwarding_tbl_dependengts_delete_t;

enum{
 	DEPENDENT_GET_PATH_ORIGIN_MATCH = BIT(0),
 	DEPENDENT_GET_DESTINATION_MATCH = BIT(1),
};
 
typedef struct{
	u16 netkey_index:12;
	u16 dependents_list_mask:2;
	u16 fixed_path_flag:1;
	u16 prohibited:1;
	u16 start_index;
	u16 path_origin;
	u16 destination;
	u16 update_identifier; // optinal
}forwarding_tbl_dependents_get_t;

typedef struct{
	u8 size;
	addr_range_little_endian_t addr_range[MAX_DEPENDENT_LIST_SIZE];
}dependents_addr_range_list_t;

typedef struct{
	u8 status;
	u16 netkey_index:12;
	u16 dependents_list_mask:2;
	u16 fixed_path_flag:1;
	u16 prohibited:1;
	u16 start_index;
	u16 path_origin;
	u16 destination;
	u8  par[2+2+(MAX_DEPENDENT_LIST_SIZE)*sizeof(addr_range_little_endian_t)]; // 2(identifier) + 2(list size) + list
}forwarding_tbl_dependents_get_sts_t;

typedef struct{
	u16 fixed_cnt;
	u16 non_fixed_cnt;
}entries_cnt_t;

typedef struct{
	u8 status;
	u16 netkey_index;
	u16 forwarding_tbl_update_identifier;
	entries_cnt_t entries_cnt;
}forwarding_tbl_entries_count_st_t;

enum{
	ENTRIES_GET_FIXED_PATH = BIT(0),
 	ENTRIES_GET_NON_FIXED_PATH = BIT(1),
 	ENTRIES_GET_PATH_ORIGIN_MATCH = BIT(2),
 	ENTRIES_GET_DST_MATCH = BIT(3),
};

typedef struct{
	u16 netkey_index:12;
	u16 filter_mask:4;
	u16 start_index;
	u16 path_origin;
	u16 destination;
	u16 update_identifier;
}forwarding_tbl_entries_get_t;

typedef struct{
	u8 status;
	u16 netkey_index:12;
	u16 filter_mask:4;
	u16 start_index;
	u8 par[100]; // list size, confirm later
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
	u32 status;
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
	u8 path_echo_interval;
}path_echo_interval_set_t;

typedef struct{
	u8 status;
	u16 netkey_index;
	u8 path_echo_interval;
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

typedef struct{
	u16 fixed_path_flag:1;
	u16 unicast_destination_flag:1;
	u16 backward_path_validated_flag:1;
	u16 bearer_toward_path_origin_indicator:1;
	u16 bearer_toward_path_target_indicator:1;
	u16 dependent_origin_list_size_indicator:2;
	u16 dependent_target_list_size_indicator:2;
	u16 rfu:7;
}forwarding_table_entry_head_t;

#if 0
typedef struct{	// one entry of the forwarding table
	forwarding_table_entry_head_t entry_head;
	union {
		u16 src_addr;
		unicast_addr_range_t path_origin_unicast_addr_range;
	};
	u16 dependent_origin_list_size; // 1 or 2 byte indicated in entry head
	u16 bearer_toward_path_origin;
	union {
		u16 dst_addr;			// unicast or group
		unicast_addr_range_t path_target_unicast_addr_range;
	};
	u16 dependent_target_list_size;// 1 or 2 byte indicated in entry head
	u16 bearer_toward_path_target; 
}fixed_path_entry_t;

typedef struct{	// one entry of the forwarding table
	forwarding_table_entry_head_t entry_head;
	u8 lane_counter;
	u16 path_lifetime;
	u8 path_origin_forwarding_number;
	union {
		u16 src_addr;
		unicast_addr_range_t path_origin_unicast_addr_range;
	};
	u16 dependent_origin_list_size;// 1 or 2 byte indicated in entry head
	u16 bearer_toward_path_origin;
	union {
		u16 dst_addr;			// unicast or group
		unicast_addr_range_t path_target_unicast_addr_range;
	};
	u16 dependent_target_list_size;// 1 or 2 byte indicated in entry head
	u16 bearer_toward_path_target; 
}non_fixed_path_entry_t;

typedef struct{
	union{
		forwarding_table_entry_head_t entry_head;
		fixed_path_entry_t fixed_path_entry;
		non_fixed_path_entry_t non_fixed_path_entry;
	};
}path_entry_t;
#endif

typedef struct{	// one entry of the forwarding table
	u8  fixed_path:1;
	u8  backward_path_validated:1;
	u8  path_not_ready:1;
	u8  rfu:5;
	u16 path_origin; 
	u8  path_origin_snd_ele_cnt;
	path_addr_t dependent_origin[MAX_DEPENDENT_LIST_SIZE];
	u16 destination;
	u8  path_target_snd_ele_cnt;
	path_addr_t dependent_target[MAX_DEPENDENT_LIST_SIZE];
	u8  forwarding_number;
	u8  lane_counter;
	u16  bearer_toward_path_origin;
	u16  bearer_toward_path_target;
}forwarding_table_entry_t;

typedef struct{
	u8 path_need;
	u32 lifetime_s;
	u32 path_echo_timer_s;
	u32 path_echo_reply_timeout_timer;
	u32 path_monitor_s;
}forwarding_timer_t;

typedef struct{
	forwarding_timer_t state;
	forwarding_table_entry_t entry;
}forwarding_entry_par_t;

typedef struct{
	u16 update_identifier;
	forwarding_entry_par_t fwd_entry_par[MAX_FORWARDING_TBL_ENTRIES_CNT];
}forwarding_table_t;

typedef struct{
	u8 path_need;
	u32 discovery_timer;
	u32 discovery_guard_timer;
	u32 discovery_retry_timer;
	u32 reply_delay_timer;
	u32 request_delay_timer;
	u8 first_reply_msg;
	u8 path_confirm_sent;
	u8 new_lane_established;
	u8 lant_counter;
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

typedef struct{
	u8 forwarding_number;
	discovery_entry_par_t dsc_entry_par[MAX_DISCOVERY_TBL_ENTRIES_CNT];
}discovery_table_t;

typedef struct{
	u8 st;
	u16 dst;
	u8 path_need;
}origin_state_t;

typedef struct{
	origin_state_t state[MAX_FORWARDING_TBL_ENTRIES_CNT];
}path_origin_state_t;

int is_directed_forwarding_en(u16 netkey_offset);
int is_directed_relay_en(u16 netkey_offset);
int is_directed_proxy_en(u16 netkey_offset);
int is_directed_friend_en(u16 netkey_offset);
int is_directed_forwarding_op(u16 op);
void mesh_directed_forwarding_default_val_init();
forwarding_entry_par_t * get_forwarding_entry(u16 netkey_offset, u16 path_origin, u16 destination, u8 entry_type);
int directed_forwarding_initial_start(u16 netkey_index, u16 destination, addr_range_big_endian_t *p_dependent);
int directed_forwarding_dependents_update_start(u16 netkey_offset, u8 type, u16 path_enpoint, addr_range_big_endian_t dependent_range_addr);
void mesh_directed_forwarding_proc(u8 *bear, int src_type);
int is_address_in_dependent_list(forwarding_table_entry_t *p_fwd_entry, u16 addr);
int forwarding_tbl_dependent_add(u16 range_start, u8 range_length, path_addr_t *p_dependent_list);
forwarding_entry_par_t * add_new_path_entry_in_forwarding_tbl(u16 netkey_offset, forwarding_table_entry_t *p);

discovery_entry_par_t * get_discovery_entry_correspond2_path_request(u16 netkey_offset, u16 path_origin, u8 forwarding_number);
/**
 * @brief      This function process config message for directed forwarding
 * @param[in]  par
 * @param[in]  par_len
 * @param[in]  cb_par -  the resource for the message
 * @return     status
 */
int mesh_cmd_sig_cfg_directed_control_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_directed_control_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_directed_control_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_path_metric_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_path_metric_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_path_metric_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_dsc_tbl_capa_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_dsc_tbl_capa_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_dsc_tbl_capa_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_forwarding_tbl_add(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_forwarding_tbl_delete(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_forwarding_tbl_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_forwarding_tbl_dependents_add(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_forwarding_tbl_dependents_delete(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_forwarding_tbl_dependents_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_forwarding_tbl_dependents_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_forwarding_tbl_dependents_get_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_forwarding_tbl_entries_count_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_forwarding_tbl_entries_count_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_forwarding_tbl_entries_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_forwarding_tbl_entries_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_wanted_lanes_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_wanted_lanes_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_wanted_lanes_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_two_way_path_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_two_way_path_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_two_way_path_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_path_echo_interval_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_path_echo_interval_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_path_echo_interval_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_directed_network_transmit_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_directed_network_transmit_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_directed_network_transmit_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_directed_relay_retransmit_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_directed_relay_retransmit_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_directed_relay_retransmit_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_rssi_threshold_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_rssi_threshold_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_rssi_threshold_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_directed_paths_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_directed_paths_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_directed_publish_policy_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_directed_publish_policy_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_directed_publish_policy_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_path_discovery_timing_control_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_path_discovery_timing_control_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_path_discovery_timing_control_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_directed_control_network_transmit_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_directed_control_network_transmit_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_directed_control_network_transmit_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_directed_control_relay_transmit_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_directed_control_relay_transmit_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_directed_control_relay_transmit_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);

#endif
