#include "directed_forwarding.h"
#include "mesh_node.h"

#if DIRECTED_FORWARDING_MODULE_EN
forwarding_table_t forwarding_table[NET_KEY_MAX];
discovery_table_t discovery_table[NET_KEY_MAX];
#if 0
path_origin_state_t path_origin_state[NET_KEY_MAX];
#endif

//--------------directed forwarding command interface---------------------------//
int is_directed_forwarding_en(u16 netkey_offset)
{
	return (DIRECTED_FORWARDING_ENABLE == model_sig_cfg_s.directed_forward.subnet_state[netkey_offset].directed_control.directed_forwarding);
}

int is_directed_relay_en(u16 netkey_offset)
{
	return (DIRECTED_RELAY_ENABLE == model_sig_cfg_s.directed_forward.subnet_state[netkey_offset].directed_control.directed_relay);
}

int is_directed_proxy_en(u16 netkey_offset)
{
	return (DIRECTED_PROXY_ENABLE == model_sig_cfg_s.directed_forward.subnet_state[netkey_offset].directed_control.directed_proxy);
}

int is_directed_friend_en(u16 netkey_offset)
{
	return (DIRECTED_FRIEND_ENABLE == model_sig_cfg_s.directed_forward.subnet_state[netkey_offset].directed_control.directed_friend);
}

int is_directed_forwarding_op(u16 op)
{
    if((op>=CMD_CTL_PATH_REQUEST) && (op<=CMD_CTL_PATH_REQUEST_SOLICITATION)){
		return 1;
	}
    return 0;
}

int is_address_in_dependent_list(forwarding_table_entry_t *p_fwd_entry, u16 addr)
{	
	foreach(i, MAX_DEPENDENT_LIST_SIZE){
		if(is_ele_in_node(addr, p_fwd_entry->dependent_origin[i].addr, p_fwd_entry->dependent_origin[i].snd_ele_cnt+1) ||
			(p_fwd_entry->backward_path_validated && is_ele_in_node(addr, p_fwd_entry->dependent_target[i].addr, p_fwd_entry->dependent_target[i].snd_ele_cnt+1))
		){
			return 1;
		}
	}
	return 0;
}

forwarding_entry_par_t * get_forwarding_entry(u16 netkey_offset, u16 src_address, u16 destination, u8 entry_type)
{
	if(netkey_offset < NET_KEY_MAX){
		foreach(i, MAX_FORWARDING_TBL_ENTRIES_CNT){
			forwarding_entry_par_t *p_fwd_entry = &forwarding_table[netkey_offset].fwd_entry_par[i];
			if((ANY_PATH != entry_type) && (entry_type != p_fwd_entry->entry.fixed_path)){
				continue;
			}
			if((is_ele_in_node(src_address, p_fwd_entry->entry.path_origin, p_fwd_entry->entry.path_origin_snd_ele_cnt+1) || is_address_in_dependent_list(&p_fwd_entry->entry, src_address)) && 
				(is_ele_in_node(destination, p_fwd_entry->entry.destination, p_fwd_entry->entry.path_target_snd_ele_cnt+1) || is_address_in_dependent_list(&p_fwd_entry->entry, destination))
			){
				return p_fwd_entry;
			}
			else if((p_fwd_entry->entry.backward_path_validated == 1) && (is_ele_in_node(destination, p_fwd_entry->entry.path_origin, p_fwd_entry->entry.path_origin_snd_ele_cnt+1) || is_address_in_dependent_list(&p_fwd_entry->entry, destination)) &&
				(is_ele_in_node(src_address, p_fwd_entry->entry.destination, p_fwd_entry->entry.path_target_snd_ele_cnt+1) || is_address_in_dependent_list(&p_fwd_entry->entry, src_address))
			){
				return p_fwd_entry;
			}
			else if(ADR_ALL_DIRECTED_FORWARD == destination){
				return p_fwd_entry;
			}
		}
	}

	return 0;
}

//--------------directed forwarding command interface end---------------------------//
void mesh_directed_forwarding_bind_state_update(u16 key_offset)
{
	if(DIRECTED_FORWARDING_DISABLE == model_sig_cfg_s.directed_forward.subnet_state[key_offset].directed_control.directed_forwarding){	
		model_sig_cfg_s.directed_forward.subnet_state[key_offset].directed_control.directed_relay = DIRECTED_RELAY_DISABLE;
		if(model_sig_cfg_s.directed_forward.subnet_state[key_offset].directed_control.directed_proxy < DIRECTED_PROXY_NOT_SUPPORT){
			model_sig_cfg_s.directed_forward.subnet_state[key_offset].directed_control.directed_proxy = DIRECTED_PROXY_DISABLE;
		}

		if(model_sig_cfg_s.directed_forward.subnet_state[key_offset].directed_control.directed_friend < DIRECTED_FRIEND_NOT_SUPPORT){
			model_sig_cfg_s.directed_forward.subnet_state[key_offset].directed_control.directed_friend = DIRECTED_FRIEND_DISABLE;
		}
	}

	if(GATT_PROXY_SUPPORT_DISABLE == model_sig_cfg_s.gatt_proxy ){
		if(model_sig_cfg_s.directed_forward.subnet_state[key_offset].directed_control.directed_proxy < DIRECTED_PROXY_NOT_SUPPORT){
			model_sig_cfg_s.directed_forward.subnet_state[key_offset].directed_control.directed_proxy = DIRECTED_PROXY_DISABLE;
		}
	}

	if(DIRECTED_PROXY_DISABLE == model_sig_cfg_s.directed_forward.subnet_state[key_offset].directed_control.directed_proxy){		
		model_sig_cfg_s.directed_forward.subnet_state[key_offset].directed_control.directed_proxy_use_directed_default = DIRECTED_PROXY_USE_DEFAULT_NOT_SUPPORT;
	}

	if((FRIEND_SUPPORT_DISABLE == model_sig_cfg_s.frid) && (model_sig_cfg_s.directed_forward.subnet_state[key_offset].directed_control.directed_friend < DIRECTED_FRIEND_NOT_SUPPORT)){
		model_sig_cfg_s.directed_forward.subnet_state[key_offset].directed_control.directed_friend = DIRECTED_FRIEND_DISABLE;
	}
	return;
}

void mesh_directed_forwarding_default_val_init()
{
	foreach(i, NET_KEY_MAX){
		model_sig_cfg_s.directed_forward.subnet_state[i].directed_control.directed_forwarding = DIRECTED_FORWARDING_ENABLE;//DIRECTED_FORWARDING_DISABLE; 	
		model_sig_cfg_s.directed_forward.subnet_state[i].directed_control.directed_relay = DIRECTED_RELAY_ENABLE;//DIRECTED_RELAY_DISABLE; 
		model_sig_cfg_s.directed_forward.subnet_state[i].directed_control.directed_proxy = DIRECTED_PROXY_EN ? DIRECTED_PROXY_DISABLE : DIRECTED_PROXY_NOT_SUPPORT;
		model_sig_cfg_s.directed_forward.subnet_state[i].directed_control.directed_proxy_use_directed_default = DIRECTED_PROXY_EN ? DIRECTED_PROXY_USE_DEFAULT_DISABLE : DIRECTED_PROXY_USE_DEFAULT_NOT_SUPPORT;
		model_sig_cfg_s.directed_forward.subnet_state[i].directed_control.directed_friend = DIRECTED_FRIEND_EN ? DIRECTED_FRIEND_DISABLE:DIRECTED_FRIEND_NOT_SUPPORT;
		mesh_directed_forwarding_bind_state_update(i);

		model_sig_cfg_s.directed_forward.subnet_state[i].path_metric.metric_type = METRIC_TYPE_NODE_COUNT;
		model_sig_cfg_s.directed_forward.subnet_state[i].path_metric.path_lifetime = PATH_LIFETIME_12MINS;
		model_sig_cfg_s.directed_forward.subnet_state[i].max_concurrent_init = MAX_CONCURRENT_CNT;	
		model_sig_cfg_s.directed_forward.subnet_state[i].wanted_lanes = 1;
		model_sig_cfg_s.directed_forward.subnet_state[i].two_way_path = 1;
		model_sig_cfg_s.directed_forward.subnet_state[i].path_echo_interval = 10; // 0:not executed.  validation interval = path lifetime * Path Echo Interval / 100
	}

	model_sig_cfg_s.directed_forward.transmit.count = TRANSMIT_CNT_DEF; // 1 in spec P243
	model_sig_cfg_s.directed_forward.transmit.invl_steps = TRANSMIT_INVL_STEPS_DEF; // 9 in spec (100ms)
	model_sig_cfg_s.directed_forward.relay_transmit.count = TRANSMIT_CNT_DEF; // 2 in spec
	model_sig_cfg_s.directed_forward.relay_transmit.invl_steps = TRANSMIT_INVL_STEPS_DEF;// 9 in spec (100ms)
	model_sig_cfg_s.directed_forward.control_transmit.count = TRANSMIT_CNT_DEF;
	model_sig_cfg_s.directed_forward.control_transmit.invl_steps = TRANSMIT_INVL_STEPS_DEF;
	model_sig_cfg_s.directed_forward.control_relay_transmit.count = TRANSMIT_CNT_DEF;
	model_sig_cfg_s.directed_forward.control_relay_transmit.invl_steps = TRANSMIT_INVL_STEPS_DEF;
		
	model_sig_cfg_s.directed_forward.rssi_threshold.default_rssi_threshold = -80; // should be 10 dB above the receiver sensitivity
	#if DF_TEST_MODE_EN
	model_sig_cfg_s.directed_forward.rssi_threshold.rssi_margin = 0; // default 0x14 in spec
	#else
	model_sig_cfg_s.directed_forward.rssi_threshold.rssi_margin = 10;
	#endif
	model_sig_cfg_s.directed_forward.directed_paths.node_paths = 20;
	model_sig_cfg_s.directed_forward.directed_paths.relay_paths = 20;
	model_sig_cfg_s.directed_forward.directed_paths.proxy_paths = 20;
	model_sig_cfg_s.directed_forward.directed_paths.friend_paths = 20;

	model_sig_cfg_s.directed_forward.discovery_timing.path_monitoring_interval = 120; // unit:second
	model_sig_cfg_s.directed_forward.discovery_timing.path_discovery_retry_interval = 300; 
	model_sig_cfg_s.directed_forward.discovery_timing.path_discovery_interval = 0; // 0:5s 1:30s
	model_sig_cfg_s.directed_forward.discovery_timing.lane_discovery_guard_interval = 0;  // 0:2s 1:10s
	
	return;
}

//----------------------------discovery table process---------------------//
u32 df_clock_time_exceed_s(u32 ref, u32 span_s){
	return ((u32)((clock_time_s()|1) - ref) > span_s);
}

discovery_entry_par_t * get_discovery_entry_correspond2_path_request(u16 netkey_offset, u16 path_origin, u8 forwarding_number)
{
	if(netkey_offset < NET_KEY_MAX){
		foreach(i, MAX_FORWARDING_TBL_ENTRIES_CNT){
			discovery_entry_t * p_dsc_entry = &discovery_table[netkey_offset].dsc_entry_par[i].entry;		
			if((path_origin == p_dsc_entry->path_origin.addr) && (forwarding_number == p_dsc_entry->forwarding_number)){
				return &discovery_table[netkey_offset].dsc_entry_par[i];
			}
		}
	}
	return 0;
}

discovery_entry_par_t * get_discovery_entry_correspond2_path_destination(u16 netkey_offset, u16 path_dst)
{
	if(netkey_offset < NET_KEY_MAX){
		foreach(i, MAX_FORWARDING_TBL_ENTRIES_CNT){
			discovery_entry_par_t * p_dsc_entry = &discovery_table[netkey_offset].dsc_entry_par[i];
			if(path_dst == p_dsc_entry->entry.destination){
				return p_dsc_entry;
			}
		}
	}
	return 0;
}

void del_discovery_entry_correspond2_path_destination(u16 netkey_offset, u16 path_origin, u16 path_dst)
{
	if(netkey_offset < NET_KEY_MAX){
		foreach(i, MAX_FORWARDING_TBL_ENTRIES_CNT){
			discovery_entry_t * p_dsc_entry = &discovery_table[netkey_offset].dsc_entry_par[i].entry;		
			if((path_origin == p_dsc_entry->path_origin.addr) && (path_dst == p_dsc_entry->destination)){
				memset(&discovery_table[netkey_offset].dsc_entry_par[i], 0x00, sizeof(discovery_table[netkey_offset].dsc_entry_par[i])) ;
			}
		}
	}
}

discovery_entry_par_t * get_discovery_entry_correspond2_forwarding_entry(u16 netkey_offset, forwarding_table_entry_t *p_fwd_entry)
{
	return get_discovery_entry_correspond2_path_request(netkey_offset, p_fwd_entry->path_origin, p_fwd_entry->forwarding_number);
}

int update_discovery_entry_by_path_request(discovery_entry_t *p_dsc_entry, u16 network_src, u8 src_type, mesh_ctl_path_req_t *p_path_request)
{
	if(p_path_request->origin_path_metric < p_dsc_entry->path_metric){
		p_dsc_entry->path_metric = p_path_request->origin_path_metric;
		p_dsc_entry->next_toward_path_origin = network_src;
		p_dsc_entry->bearer_toward_path_origin = src_type;
		LOG_MSG_LIB(TL_LOG_DIRECTED_FORWARDING, 0, 0, " update_discovery_entry_by_path_request", 0);
		return 1;
	}
	return 0;
}

discovery_entry_par_t * get_discovery_entry_correspond2_path_reply(u16 netkey_offset, mesh_ctl_path_reply_t *p_path_reply)
{
	if(netkey_offset < NET_KEY_MAX){
		foreach(i, MAX_FORWARDING_TBL_ENTRIES_CNT){
			discovery_entry_t * p_dsc_entry = &discovery_table[netkey_offset].dsc_entry_par[i].entry;	
			addr_range_big_endian_t *p_range_addr = (addr_range_big_endian_t *)p_path_reply->addr_par;
			if((p_path_reply->path_origin == p_dsc_entry->path_origin.addr) && (p_path_reply->forwarding_number == p_dsc_entry->forwarding_number) &&
				(is_group_adr(p_dsc_entry->destination) || is_virtual_adr(p_dsc_entry->destination) || is_ele_in_node(p_dsc_entry->destination, p_range_addr->range_start, p_range_addr->range_length))
			){
				return &discovery_table[netkey_offset].dsc_entry_par[i];
			}
		}
	}
	return 0;
}

discovery_entry_par_t * add_discovery_table_entry(u16 netkey_offset, addr_range_big_endian_t *path_origin, u16 destination, addr_range_big_endian_t *p_dependent, u16 next_toward_path_origin, u8 bearer_toward_path_origin, mesh_ctl_path_req_t *p_path_req)
{
	// if recevive path request, discard path_origin, destination, p_dependent
	discovery_entry_t dsc_tbl_entry;
	memset(&dsc_tbl_entry, 0x00, sizeof(dsc_tbl_entry));
	dsc_tbl_entry.path_metric_type = p_path_req?p_path_req->path_metric_type:model_sig_cfg_s.directed_forward.subnet_state[netkey_offset].path_metric.metric_type;
	dsc_tbl_entry.path_lifetime = p_path_req?p_path_req->path_lifetime:model_sig_cfg_s.directed_forward.subnet_state[netkey_offset].path_metric.path_lifetime; 
	dsc_tbl_entry.path_discovery_interval = p_path_req?p_path_req->discovery_interval:model_sig_cfg_s.directed_forward.discovery_timing.path_discovery_interval;
	dsc_tbl_entry.forwarding_number = p_path_req?p_path_req->forwarding_number:(discovery_table[netkey_offset].forwarding_number);
	dsc_tbl_entry.path_metric = p_path_req?p_path_req->origin_path_metric:0;
	dsc_tbl_entry.destination = p_path_req?p_path_req->destination:destination;
	
	dsc_tbl_entry.next_toward_path_origin = next_toward_path_origin;
	dsc_tbl_entry.bearer_toward_path_origin = bearer_toward_path_origin;
	addr_range_big_endian_t *p_range_addr = p_path_req?(addr_range_big_endian_t *)p_path_req->addr_par:path_origin;
	dsc_tbl_entry.path_origin.addr = p_range_addr->range_start;
	u8 via_len = 0;
	if(p_range_addr->length_present){ // path origin
		dsc_tbl_entry.path_origin.snd_ele_cnt = p_range_addr->range_length-1;
		via_len += 3;
	}
	else{
		dsc_tbl_entry.path_origin.snd_ele_cnt = 0;	
		via_len += 2;
	}

	p_range_addr = p_path_req?(addr_range_big_endian_t *)(p_path_req->addr_par+via_len):p_dependent;
	if(p_dependent || p_path_req->on_behalf_of_dependent_origin){// path dependent
		dsc_tbl_entry.on_behalf_of_dependent_origin = 1;
		dsc_tbl_entry.dependent_origin.addr = p_range_addr->range_start;
		if(p_range_addr->length_present){
			dsc_tbl_entry.dependent_origin.snd_ele_cnt = p_range_addr->range_length-1;
		}
		else{
			dsc_tbl_entry.dependent_origin.snd_ele_cnt = 0;
		}
	}
	else{
		dsc_tbl_entry.on_behalf_of_dependent_origin = 0;
	}
	
	foreach(i, MAX_DISCOVERY_TBL_ENTRIES_CNT){
		if(0 == discovery_table[netkey_offset].dsc_entry_par[i].entry.path_origin.addr){
			LOG_USER_MSG_INFO(0, 0, "add discovery_table origin:0x%x des:0x%x", dsc_tbl_entry.path_origin, dsc_tbl_entry.destination);
			discovery_table[netkey_offset].dsc_entry_par[i].entry = dsc_tbl_entry;					
			return &discovery_table[netkey_offset].dsc_entry_par[i];
		}
	}
	return 0;
}

void start_path_discovery_timer(discovery_entry_par_t *p_dsc_entry)
{
	LOG_USER_MSG_INFO(0, 0, "start_path_discovery_timer origin:%x target:%x", p_dsc_entry->entry.path_origin.addr, p_dsc_entry->entry.destination);
	p_dsc_entry->state.discovery_timer = clock_time()|1;
	p_dsc_entry->state.first_reply_msg = 1;
	p_dsc_entry->state.path_confirm_sent = 0;
	p_dsc_entry->state.new_lane_established = 0;
}

void stop_path_discovery_timer(discovery_entry_par_t *p_dsc_entry)
{
	LOG_USER_MSG_INFO(0, 0, "stop_path_discovery_timer origin:%x target:%x", p_dsc_entry->entry.path_origin.addr, p_dsc_entry->entry.destination);
	p_dsc_entry->state.discovery_timer = 0;	
}

void start_path_discovery_guard_timer(discovery_entry_par_t *p_dsc_entry)
{
	LOG_USER_MSG_INFO(0, 0, "start_path_discovery_guard_timer origin:%x target:%x", p_dsc_entry->entry.path_origin.addr, p_dsc_entry->entry.destination);
	p_dsc_entry->state.discovery_guard_timer = clock_time()|1;
}

void stop_path_discovery_guard_timer(discovery_entry_par_t *p_dsc_entry)
{
	LOG_USER_MSG_INFO(0, 0, "start_path_discovery_guard_timer origin:%x target:%x", p_dsc_entry->entry.path_origin.addr, p_dsc_entry->entry.destination);
	p_dsc_entry->state.discovery_guard_timer = 0;
}

void start_path_discovery_retry_timer(discovery_entry_par_t *p_dsc_entry)
{
	LOG_USER_MSG_INFO(0, 0, "start_path_discovery_retry_timer origin:%x target:%x", p_dsc_entry->entry.path_origin.addr, p_dsc_entry->entry.destination);
	p_dsc_entry->state.discovery_retry_timer = clock_time()|1;
}

void stop_path_discovery_retry_timer(discovery_entry_par_t *p_dsc_entry)
{
	LOG_USER_MSG_INFO(0, 0, "stop_path_discovery_retry_timer origin:%x target:%x", p_dsc_entry->entry.path_origin.addr, p_dsc_entry->entry.destination);
	p_dsc_entry->state.discovery_retry_timer = 0;
}


void start_path_reply_delay_timer(discovery_entry_par_t *p_dsc_entry)
{
	LOG_USER_MSG_INFO(0, 0, "start_path_reply_delay_timer origin:%x target:%x", p_dsc_entry->entry.path_origin.addr, p_dsc_entry->entry.destination);
	p_dsc_entry->state.reply_delay_timer = clock_time()|1;
}

void stop_path_reply_delay_timer(discovery_entry_par_t *p_dsc_entry)
{
	LOG_USER_MSG_INFO(0, 0, "stop_path_reply_delay_timer origin:%x target:%x", p_dsc_entry->entry.path_origin.addr, p_dsc_entry->entry.destination);
	p_dsc_entry->state.reply_delay_timer = 0;
}

void start_path_request_delay_timer(discovery_entry_par_t *p_dsc_entry)
{
	LOG_USER_MSG_INFO(0, 0, "start_path_request_delay_timer origin:%x target:%x", p_dsc_entry->entry.path_origin.addr, p_dsc_entry->entry.destination);
	p_dsc_entry->state.request_delay_timer = clock_time()|1;
}

void stop_path_request_delay_timer(discovery_entry_par_t *p_dsc_entry)
{
	LOG_USER_MSG_INFO(0, 0, "stop_path_request_delay_timer origin:%x target:%x", p_dsc_entry->entry.path_origin.addr, p_dsc_entry->entry.destination);
	p_dsc_entry->state.request_delay_timer = 0;
}

void directed_discovery_entry_remove(discovery_entry_par_t *p_dsc_entry)
{
	LOG_MSG_LIB(TL_LOG_DIRECTED_FORWARDING, 0, 0, "directed_discovery_entry_remove origin:%x target:%x", p_dsc_entry->entry.path_origin.addr, p_dsc_entry->entry.destination);
	memset(p_dsc_entry, 0x00, sizeof(discovery_entry_par_t));
}

int direced_forwarding_concurrent_count_check(u16 netkey_offset)
{
	int num = 0;
	foreach_arr(i, discovery_table[netkey_offset].dsc_entry_par){
		if(discovery_table[netkey_offset].dsc_entry_par[i].entry.path_origin.addr){
			num++;
		}
	}

	if(num < model_sig_cfg_s.directed_forward.subnet_state[netkey_offset].max_concurrent_init){
		return 1;
	}

	return 0;
}
//---------------------------discovery table process end-------------------------//

//---------------------------forwarding table process-------------------------//
void start_path_lifetime_timer(forwarding_entry_par_t *p_fwd_entry)
{
	p_fwd_entry->state.lifetime_s = clock_time_s()|1;
	LOG_USER_MSG_INFO(0, 0, "start_path_lifetime_timer origin:%x dst:%x timer_s:%d", p_fwd_entry->entry.path_origin, p_fwd_entry->entry.destination, p_fwd_entry->state.lifetime_s);
}

void stop_path_lifetime_timer(forwarding_entry_par_t *p_fwd_entry)
{
	LOG_USER_MSG_INFO(0, 0, "stop_path_reply_delay_timer origin:%x dst:%x", p_fwd_entry->entry.path_origin, p_fwd_entry->entry.destination);
	p_fwd_entry->state.lifetime_s = 0;
}

void start_path_echo_timer(forwarding_entry_par_t *p_fwd_entry)
{
	LOG_USER_MSG_INFO(0, 0, "start_path_echo_timer origin:%x dst:%x", p_fwd_entry->entry.path_origin, p_fwd_entry->entry.destination);
	p_fwd_entry->state.path_echo_timer_s = clock_time_s()|1;;
}

void stop_path_echo_timer(forwarding_entry_par_t *p_fwd_entry)
{
	LOG_USER_MSG_INFO(0, 0, "stop_path_echo_timer origin:%x dst:%x", p_fwd_entry->entry.path_origin, p_fwd_entry->entry.destination);
	p_fwd_entry->state.path_echo_timer_s = 0;
}

void start_path_echo_reply_timeout_timer(forwarding_entry_par_t *p_fwd_entry)
{
	LOG_USER_MSG_INFO(0, 0, "start_path_echo_reply_timeout_timer origin:%x dst:%x", p_fwd_entry->entry.path_origin, p_fwd_entry->entry.destination);
	p_fwd_entry->state.path_echo_reply_timeout_timer = clock_time()|1;;
}

void stop_path_echo_reply_timeout_timer(forwarding_entry_par_t *p_fwd_entry)
{
	LOG_USER_MSG_INFO(0, 0, "stop_path_echo_reply_timeout_timer origin:%x dst:%x", p_fwd_entry->entry.path_origin, p_fwd_entry->entry.destination);
	p_fwd_entry->state.path_echo_reply_timeout_timer = 0;
}

void start_path_monitor_timer(forwarding_entry_par_t *p_fwd_entry)
{
	LOG_USER_MSG_INFO(0, 0, "start_path_echo_timer origin:%x dst:%x", p_fwd_entry->entry.path_origin, p_fwd_entry->entry.destination);
	p_fwd_entry->state.path_monitor_s = clock_time_s()|1;;
}

void stop_path_monitor_timer(forwarding_entry_par_t *p_fwd_entry)
{
	LOG_USER_MSG_INFO(0, 0, "stop_path_echo_timer origin:%x dst:%x", p_fwd_entry->entry.path_origin, p_fwd_entry->entry.destination);
	p_fwd_entry->state.path_monitor_s = 0;
}

int update_forwarding_tbl_update_identifier(u16 netkey_offset)
{
	if(netkey_offset < NET_KEY_MAX){
		forwarding_table[netkey_offset].update_identifier++;
		return 1;
	}
	return 0;
}

int set_forwarding_tbl_update_identifier(u16 inentifier, u16 netkey_offset)
{
	if(netkey_offset < NET_KEY_MAX){
		forwarding_table[netkey_offset].update_identifier = inentifier;
		return 1;
	}
	return 0;
}

forwarding_entry_par_t * get_empty_forwarding_tbl_entry(u16 netkey_offset)
{
	if(netkey_offset < NET_KEY_MAX){
		foreach(i, MAX_FORWARDING_TBL_ENTRIES_CNT){
			if(0 == forwarding_table[netkey_offset].fwd_entry_par[i].entry.path_origin){
				return &forwarding_table[netkey_offset].fwd_entry_par[i];
			}
		}
	}
	return 0;
}

int get_offset_in_dependent_list(path_addr_t *p_dependent_list, u16 addr)
{
	foreach(i, MAX_DEPENDENT_LIST_SIZE){
		if(addr == p_dependent_list[i].addr){
			return i;
		}
	}
	return -1;
}

forwarding_entry_par_t * get_corresponding_forwarding_fixed_entry(u16 netkey_offset, u16 path_origin, u16 destination)
{
	return get_forwarding_entry(netkey_offset, path_origin, destination, FIXED_PATH);
}

forwarding_entry_par_t * get_corresponding_forwarding_non_fixed_entry(u16 netkey_offset, u16 path_origin, u16 destination)
{
	return get_forwarding_entry(netkey_offset, path_origin, destination, NON_FIXED_PATH);
}

forwarding_entry_par_t * get_forwarding_entry_correspond2_path_request(u16 netkey_offset, mesh_ctl_path_req_t *p_path_req)
{
	if(netkey_offset < NET_KEY_MAX){
		foreach(i, MAX_FORWARDING_TBL_ENTRIES_CNT){
			forwarding_entry_par_t *p_fwd_entry = &forwarding_table[netkey_offset].fwd_entry_par[i];
			addr_range_big_endian_t *p_path_origin = (addr_range_big_endian_t *)p_path_req->addr_par;
			if((0 == p_fwd_entry->entry.fixed_path) && (p_path_origin->range_start == p_fwd_entry->entry.path_origin) && 
				(is_ele_in_node(p_path_req->destination, p_fwd_entry->entry.destination, p_fwd_entry->entry.path_target_snd_ele_cnt+1) || is_address_in_dependent_list(&p_fwd_entry->entry, p_path_req->destination))
			){
				return p_fwd_entry;
			}
		}
	}
	return 0;
}

forwarding_entry_par_t * get_forwarding_entry_correspond2_discovery_entry(u16 netkey_offset, discovery_entry_t *p_dsc_entry)
{
	if(netkey_offset < NET_KEY_MAX){
		foreach(i, MAX_FORWARDING_TBL_ENTRIES_CNT){
			forwarding_entry_par_t *p_fwd_entry = &forwarding_table[netkey_offset].fwd_entry_par[i];
			if((0 == p_fwd_entry->entry.fixed_path) && (p_dsc_entry->path_origin.addr == p_fwd_entry->entry.path_origin) && (p_dsc_entry->forwarding_number == p_fwd_entry->entry.forwarding_number)){
				return p_fwd_entry;
			}
		}
	}
	return 0;
}


int delete_path_entry_in_forwarding_tbl(u16 netkey_offset, forwarding_table_entry_t *p_entry)
{
	int err = -1;
	if(netkey_offset < NET_KEY_MAX){
		foreach(i, MAX_FORWARDING_TBL_ENTRIES_CNT){
			forwarding_entry_par_t *p_fwd_entry = &forwarding_table[netkey_offset].fwd_entry_par[i];
			if((p_entry->path_origin == p_fwd_entry->entry.path_origin) &&
				(p_entry->destination == p_fwd_entry->entry.destination)){
				 memset(p_fwd_entry, 0x00, sizeof(forwarding_entry_par_t));
			}	
		}
	}
	if(0 == err){
		update_forwarding_tbl_update_identifier(netkey_offset);
	}
	return err;
}

forwarding_entry_par_t * add_new_path_entry_in_forwarding_tbl(u16 netkey_offset, forwarding_table_entry_t *p)
{
	forwarding_entry_par_t * p_entry = get_empty_forwarding_tbl_entry(netkey_offset);
	memset(p_entry, 0x00, sizeof(forwarding_entry_par_t));
	
	if(p_entry){
		update_forwarding_tbl_update_identifier(netkey_offset);		
		memcpy(&p_entry->entry, p, sizeof(forwarding_table_entry_t));		
	}
	LOG_USER_MSG_INFO(0, 0, "add_new_path_entry_in_forwarding_tbl origin:0x%x dst:0x%x", p_entry->entry.path_origin, p_entry->entry.destination);	
	return p_entry;
}

int forwarding_tbl_dependent_add(u16 range_start, u8 range_length, path_addr_t *p_dependent_list)
{
	int err = -1;	

	foreach(i,MAX_DEPENDENT_LIST_SIZE){	
		if(p_dependent_list[i].addr == range_start){// exist
			err = 0;
			break;
		}
		
		if(0 == p_dependent_list[i].addr){ // add
			err = 0;
			p_dependent_list[i].addr = range_start;
			p_dependent_list[i].snd_ele_cnt = range_length?(range_length - 1):0;
			break;
		}	
	}

	return err;
}

int forwarding_tbl_dependent_delete(u16 addr, path_addr_t *p_dependent_list)
{
	int err = -1;
	foreach(i,MAX_DEPENDENT_LIST_SIZE){	
		if(p_dependent_list[i].addr == addr){// exist
			memset(&p_dependent_list[i], 0x00, sizeof(path_addr_t));
			continue;					
		}	

		if(0 == p_dependent_list[i].addr){
			break;
		}
	}	

	foreach(i, MAX_DEPENDENT_LIST_SIZE){ // sort dependent list 
		if(0 == p_dependent_list[i].addr){
			for(int j=i+1; j<MAX_DEPENDENT_LIST_SIZE; j++){
				if(p_dependent_list[j].addr){
					p_dependent_list[i] = p_dependent_list[j];
					memset(&p_dependent_list[j], 0x00, sizeof(path_addr_t));
					break;
				}
				if(j == MAX_DEPENDENT_LIST_SIZE-1){
					i = MAX_DEPENDENT_LIST_SIZE; // break;
				}
			}
		}
	}
	return err;
}

forwarding_entry_par_t * add_forwarding_entry_by_path_reply_delay_timer_expired(u16 netkey_offset, discovery_entry_t *p_dsc_entry)
{
	forwarding_table_entry_t tbl_entry;
	memset(&tbl_entry, 0x00, sizeof(tbl_entry));

	tbl_entry.fixed_path = 0;
	tbl_entry.backward_path_validated = 0;
	tbl_entry.path_not_ready = 0;
	tbl_entry.path_origin = p_dsc_entry->path_origin.addr;
	tbl_entry.path_origin_snd_ele_cnt = p_dsc_entry->path_origin.snd_ele_cnt;
	tbl_entry.forwarding_number = p_dsc_entry->forwarding_number;
	tbl_entry.bearer_toward_path_origin = p_dsc_entry->bearer_toward_path_origin;
	
	if(p_dsc_entry->dependent_origin.addr){
		forwarding_tbl_dependent_add(p_dsc_entry->dependent_origin.addr, p_dsc_entry->dependent_origin.snd_ele_cnt+1, tbl_entry.dependent_origin);
	}

	if(is_group_adr(p_dsc_entry->destination) || is_virtual_adr(p_dsc_entry->destination)){
		tbl_entry.destination = p_dsc_entry->destination;				
	}
	else{
		tbl_entry.destination = ele_adr_primary;
		tbl_entry.path_target_snd_ele_cnt = g_ele_cnt - 1;
	}
		
	u16 match_F2L = mesh_mac_match_friend(p_dsc_entry->destination);
	foreach(i, g_max_lpn_num){
		if((match_F2L>>i) & 0x01){						
			forwarding_tbl_dependent_add(fn_other_par[i].LPNAdr, fn_req[i].NumEle, tbl_entry.dependent_target);
			break;
		}
	}

	tbl_entry.bearer_toward_path_target = BEAR_UNASSIGNED;
	tbl_entry.lane_counter = 1;
	
	forwarding_entry_par_t *p_fwd_entry = get_corresponding_forwarding_non_fixed_entry(netkey_offset, tbl_entry.path_origin, tbl_entry.destination);
	if(p_fwd_entry){
		delete_path_entry_in_forwarding_tbl(netkey_offset, &p_fwd_entry->entry);
	}
	return add_new_path_entry_in_forwarding_tbl(netkey_offset, &tbl_entry);
}

void update_forwarding_entry_by_path_reply_delay_timer_expired(u8 netkey_offset, forwarding_entry_par_t *p_fwd_entry, discovery_entry_par_t *p_dsc_entry)
{
	update_forwarding_tbl_update_identifier(netkey_offset);	
	p_fwd_entry->entry.bearer_toward_path_origin = p_dsc_entry->entry.bearer_toward_path_origin;
	p_fwd_entry->entry.lane_counter++;
}

forwarding_entry_par_t * get_forwarding_entry_correspond2_path_reply(u16 netkey_offset, mesh_ctl_path_reply_t *p_path_reply)
{
	if(netkey_offset < NET_KEY_MAX){
		foreach(i, MAX_FORWARDING_TBL_ENTRIES_CNT){
			forwarding_entry_par_t *p_fwd_entry = &forwarding_table[netkey_offset].fwd_entry_par[i];
			if((p_path_reply->path_origin == p_fwd_entry->entry.path_origin) && (p_path_reply->forwarding_number == p_fwd_entry->entry.forwarding_number)){
				return p_fwd_entry;
			}
		}
	}
	return 0;
}

forwarding_entry_par_t * add_forwarding_tbl_entry_by_rcv_path_reply(u16 netkey_offset, u8 bearer_toward_path_origin, discovery_entry_par_t *p_dsc_entry, mesh_ctl_path_reply_t *p_path_reply)
{
	forwarding_table_entry_t tbl_entry;
	memset(&tbl_entry, 0x00, sizeof(tbl_entry));
	
	addr_range_big_endian_t *p_path_target = (addr_range_big_endian_t *)p_path_reply->addr_par;
	tbl_entry.fixed_path = 0;
	if(is_own_ele(p_path_reply->path_origin) && (0 == p_path_target->range_start)){
		tbl_entry.path_not_ready = 1;
	}

	tbl_entry.path_origin = p_dsc_entry->entry.path_origin.addr;
	tbl_entry.path_origin_snd_ele_cnt = p_dsc_entry->entry.path_origin.snd_ele_cnt;
	tbl_entry.forwarding_number = p_dsc_entry->entry.forwarding_number;

	if(!is_own_ele(p_path_reply->path_origin)){
		tbl_entry.bearer_toward_path_origin = p_dsc_entry->entry.bearer_toward_path_origin;
	}
	else{
		tbl_entry.bearer_toward_path_origin = BEAR_UNASSIGNED;
	}
	
	if(p_dsc_entry->entry.dependent_origin.addr){
		forwarding_tbl_dependent_add(p_dsc_entry->entry.dependent_origin.addr, p_dsc_entry->entry.dependent_origin.snd_ele_cnt+1, tbl_entry.dependent_origin);
	}

	if(is_group_adr(p_dsc_entry->entry.destination) || is_virtual_adr(p_dsc_entry->entry.destination)){
		tbl_entry.destination = p_dsc_entry->entry.destination;				
	}
	else{
		tbl_entry.destination = p_path_target->range_start;
		tbl_entry.path_target_snd_ele_cnt = p_path_target->length_present?(p_path_target->range_length-1):0;
	}

	if(p_path_reply->on_behalf_of_dependent_target){	
		addr_range_big_endian_t *p_dependent_target = (addr_range_big_endian_t *)(p_path_reply->addr_par + (p_path_target->length_present?3:2));
		forwarding_tbl_dependent_add(p_dependent_target->range_start, p_dependent_target->length_present?p_dependent_target->range_length:1, tbl_entry.dependent_target);
	}

	tbl_entry.bearer_toward_path_target = bearer_toward_path_origin;
	tbl_entry.lane_counter = 1;

	forwarding_entry_par_t *p_fwd_entry = get_corresponding_forwarding_non_fixed_entry(netkey_offset, tbl_entry.path_origin, tbl_entry.destination);
	if(p_fwd_entry){
		delete_path_entry_in_forwarding_tbl(netkey_offset, &p_fwd_entry->entry);
	}
	
	return add_new_path_entry_in_forwarding_tbl(netkey_offset, &tbl_entry);
}

void update_forwarding_entry_by_path_reply(u8 netkey_offset, u8 src_type, forwarding_entry_par_t *p_fwd_entry, discovery_entry_par_t *p_dsc_entry)
{
	update_forwarding_tbl_update_identifier(netkey_offset);	
	if(!is_own_ele(p_dsc_entry->entry.path_origin.addr)){
		p_fwd_entry->entry.bearer_toward_path_origin = p_dsc_entry->entry.bearer_toward_path_origin;
	}
	p_fwd_entry->entry.bearer_toward_path_target = src_type;
	if(p_dsc_entry->state.first_reply_msg){		
		p_fwd_entry->entry.lane_counter++;
	}
}

forwarding_entry_par_t * get_forwarding_entry_correspond2_path_confirm(u16 netkey_offset, mesh_ctl_path_confirmation_t *p_path_comfirm)
{
	if(netkey_offset < NET_KEY_MAX){
		foreach(i, MAX_FORWARDING_TBL_ENTRIES_CNT){
			forwarding_entry_par_t *p_fwd_entry = &forwarding_table[netkey_offset].fwd_entry_par[i];
			if((0 == p_fwd_entry->entry.fixed_path) && (p_path_comfirm->path_origin == p_fwd_entry->entry.path_origin) && (p_path_comfirm->path_target == p_fwd_entry->entry.destination)){
				p_fwd_entry->entry.backward_path_validated = 1;
				update_forwarding_tbl_update_identifier(netkey_offset);
				return p_fwd_entry;
			}
		}
	}
	return 0;
}

void update_forwarding_entry_by_path_confirm(u16 netkey_offset, forwarding_table_entry_t *p_fwd_entry, mesh_ctl_path_confirmation_t *p_path_comfirm)
{
	update_forwarding_tbl_update_identifier(netkey_offset);
	p_fwd_entry->backward_path_validated = 1;
}

forwarding_entry_par_t * get_forwarding_entry_correspond2_path_echo_request(u16 netkey_offset, u16 src_addr, u16 dst_addr)
{
	if(netkey_offset < NET_KEY_MAX){
		foreach(i, MAX_FORWARDING_TBL_ENTRIES_CNT){
			forwarding_entry_par_t *p_fwd_entry = &forwarding_table[netkey_offset].fwd_entry_par[i];
			if((0 == p_fwd_entry->entry.fixed_path) && (src_addr == p_fwd_entry->entry.path_origin) && (dst_addr == p_fwd_entry->entry.destination)){
				return p_fwd_entry;
			}
		}
	}
	return 0;
}

forwarding_entry_par_t * get_forwarding_entry_correspond2_path_echo_reply(u16 netkey_offset, u16 network_dst, mesh_ctl_path_echo_reply_t *p_path_echo_reply)
{
	if(netkey_offset < NET_KEY_MAX){
		foreach(i, MAX_FORWARDING_TBL_ENTRIES_CNT){
			forwarding_entry_par_t *p_fwd_entry = &forwarding_table[netkey_offset].fwd_entry_par[i];
			if((0 == p_fwd_entry->entry.fixed_path) && (network_dst == p_fwd_entry->entry.path_origin) && (p_path_echo_reply->destination == p_fwd_entry->entry.destination)){
				return p_fwd_entry;
			}
		}
	}
	return 0;
}

forwarding_entry_par_t * get_forwarding_entry_correspond2_dependent_node_update(u16 netkey_offset, mesh_ctl_dependent_node_update_t *p_dependent_node_update)
{
	if(netkey_offset < NET_KEY_MAX){
		foreach(i, MAX_FORWARDING_TBL_ENTRIES_CNT){
			forwarding_entry_par_t *p_fwd_entry = &forwarding_table[netkey_offset].fwd_entry_par[i];
			if(0 == p_fwd_entry->entry.fixed_path){
				if(((p_dependent_node_update->path_endpoint==p_fwd_entry->entry.path_origin) && (0 == p_dependent_node_update->type) && is_address_in_dependent_list(&p_fwd_entry->entry, p_dependent_node_update->dependent_addr.range_start)) ||
					((p_dependent_node_update->path_endpoint==p_fwd_entry->entry.path_origin) && (1 == p_dependent_node_update->type) && !is_address_in_dependent_list(&p_fwd_entry->entry, p_dependent_node_update->dependent_addr.range_start)) ||
					((p_dependent_node_update->path_endpoint==p_fwd_entry->entry.destination) && (0 == p_dependent_node_update->type) && is_address_in_dependent_list(&p_fwd_entry->entry, p_dependent_node_update->dependent_addr.range_start)) ||
					((p_dependent_node_update->path_endpoint==p_fwd_entry->entry.destination) && (1 == p_dependent_node_update->type) && p_fwd_entry->entry.backward_path_validated && !is_address_in_dependent_list(&p_fwd_entry->entry, p_dependent_node_update->dependent_addr.range_start))
				){
					return p_fwd_entry;
				}
			}
		}
	}
	return 0;
}

int update_forwarding_by_dependent_node_update(u16 netkey_offset, mesh_ctl_dependent_node_update_t *p_dependent_update)
{
	int ret = 0;
	if(netkey_offset < NET_KEY_MAX){
		foreach(i, MAX_FORWARDING_TBL_ENTRIES_CNT){
			forwarding_entry_par_t *p_fwd_entry = &forwarding_table[netkey_offset].fwd_entry_par[i];
			if(0 == p_fwd_entry->entry.fixed_path){
				if(((p_dependent_update->path_endpoint==p_fwd_entry->entry.path_origin) && (0==p_dependent_update->type) && is_address_in_dependent_list(&p_fwd_entry->entry, p_dependent_update->dependent_addr.range_start)) ||
					((p_dependent_update->path_endpoint==p_fwd_entry->entry.path_origin) && (1==p_dependent_update->type) && !is_address_in_dependent_list(&p_fwd_entry->entry, p_dependent_update->dependent_addr.range_start)) ||
					((p_dependent_update->path_endpoint==p_fwd_entry->entry.destination) && (0==p_dependent_update->type) && is_address_in_dependent_list(&p_fwd_entry->entry, p_dependent_update->dependent_addr.range_start))	||
					((p_dependent_update->path_endpoint==p_fwd_entry->entry.destination) && (1==p_dependent_update->type) && p_fwd_entry->entry.backward_path_validated && !is_address_in_dependent_list(&p_fwd_entry->entry, p_dependent_update->dependent_addr.range_start))				
				){
					if(p_dependent_update->path_endpoint==p_fwd_entry->entry.path_origin){
						if(p_dependent_update->type){
							forwarding_tbl_dependent_add(p_dependent_update->dependent_addr.range_start, p_dependent_update->dependent_addr.length_present?p_dependent_update->dependent_addr.range_length:1, p_fwd_entry->entry.dependent_origin);
						}
						else{
							forwarding_tbl_dependent_delete(p_dependent_update->dependent_addr.range_start, p_fwd_entry->entry.dependent_origin);
						}
					}
					else if(p_dependent_update->path_endpoint==p_fwd_entry->entry.destination){
						if(p_dependent_update->type){
							forwarding_tbl_dependent_add(p_dependent_update->dependent_addr.range_start, p_dependent_update->dependent_addr.length_present?p_dependent_update->dependent_addr.range_length:1, p_fwd_entry->entry.dependent_target);
						}
						else{
							forwarding_tbl_dependent_delete(p_dependent_update->dependent_addr.range_start, p_fwd_entry->entry.dependent_target);
						}	
					}
					ret = 1;
					update_forwarding_tbl_update_identifier(netkey_offset);
				}
			}
		}
	}
	return ret;
}

forwarding_entry_par_t * get_forwarding_entry_correspond2_path_request_solication(u16 netkey_offset, mesh_ctl_path_request_solication_t *p_request_solication, int par_len)
{
	if(netkey_offset < NET_KEY_MAX){
		foreach(i, MAX_FORWARDING_TBL_ENTRIES_CNT){
			forwarding_entry_par_t *p_fwd_entry = &forwarding_table[netkey_offset].fwd_entry_par[i];
			if((0 == p_fwd_entry->entry.fixed_path) && is_own_ele(p_fwd_entry->entry.path_origin)){
				foreach(i, par_len/2){
					if(p_fwd_entry->entry.destination == p_request_solication->addr_list[i]){
						return p_fwd_entry;
					}
				}
			}
		}
	}
	return 0;
}

//---------------------------forwarding table process end---------------------//

//---------------------------forwarding config message-----------------------------------//
int mesh_cmd_sig_cfg_directed_control_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	directed_control_sts_t directed_control_sts;	
	u16 netkey_index = par[0] + (par[1]<<8);
	directed_control_sts.status = ST_SUCCESS;
	directed_control_sts.netkey_index = netkey_index; 
	int key_offset = get_mesh_net_key_offset(netkey_index);	
	if(-1 != key_offset){
		memcpy(&directed_control_sts.directed_control, &model_sig_cfg_s.directed_forward.subnet_state[key_offset].directed_control, sizeof(directed_control_t));
	}
	else{
		directed_control_sts.status = ST_INVALID_NETKEY;
		memset(&directed_control_sts.directed_control, 0x00, sizeof(directed_control_sts.directed_control));
	}

	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&directed_control_sts),sizeof(directed_control_sts),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_directed_control_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	directed_control_set_t *p_set = (directed_control_set_t *)par;
	directed_control_sts_t directed_control_sts;
	directed_control_sts.status = ST_SUCCESS;
	directed_control_sts.netkey_index = p_set->netkey_index;
	int key_offset = get_mesh_net_key_offset(p_set->netkey_index);	
	if(-1 != key_offset){
		if(p_set->directed_control.directed_forwarding < DIRECTED_FORWARDING_PROHIBIT){
			model_sig_cfg_s.directed_forward.subnet_state[key_offset].directed_control.directed_forwarding = p_set->directed_control.directed_forwarding;
		}
		
		if(p_set->directed_control.directed_relay < DIRECTED_RELAY_PROHIBIT){
			model_sig_cfg_s.directed_forward.subnet_state[key_offset].directed_control.directed_relay = p_set->directed_control.directed_relay;
		}
		
		if(p_set->directed_control.directed_proxy < DIRECTED_PROXY_NOT_SUPPORT){
			model_sig_cfg_s.directed_forward.subnet_state[key_offset].directed_control.directed_proxy = p_set->directed_control.directed_proxy;
		}
		
		if(p_set->directed_control.directed_proxy_use_directed_default < DIRECTED_PROXY_USE_DEFAULT_NOT_SUPPORT){
			model_sig_cfg_s.directed_forward.subnet_state[key_offset].directed_control.directed_proxy_use_directed_default = p_set->directed_control.directed_proxy_use_directed_default;
		}
		
		if(p_set->directed_control.directed_friend < DIRECTED_FRIEND_NOT_SUPPORT){
			model_sig_cfg_s.directed_forward.subnet_state[key_offset].directed_control.directed_friend = p_set->directed_control.directed_friend;
		}

		
		if(p_set->directed_control.directed_forwarding == 0){
			// Welson to do:remove non-fixed path entries , destroy all instances of the Path Origin State Machine, cancel all Directed Forwarding procedures
		}
	}	
	else{
		directed_control_sts.status = ST_INVALID_NETKEY;		
	}
	
	memcpy(&directed_control_sts.directed_control, &model_sig_cfg_s.directed_forward.subnet_state[key_offset].directed_control, sizeof(directed_control_t));
	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&directed_control_sts),sizeof(directed_control_sts),cb_par->adr_src);
	mesh_common_store(FLASH_ADR_MD_CFG_S);
	return err;
}

int mesh_cmd_sig_cfg_directed_control_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_path_metric_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	path_metric_sts_t path_metric_sts;	
	u16 netkey_index = par[0] + (par[1]<<8);
	path_metric_sts.status = ST_SUCCESS;
	path_metric_sts.netkey_index = netkey_index;

	int key_offset = get_mesh_net_key_offset(netkey_index);	
	if(-1 != key_offset){
		memcpy(&path_metric_sts.path_metric, &model_sig_cfg_s.directed_forward.subnet_state[key_offset].path_metric, sizeof(path_metric_t));
	}
	else{
		path_metric_sts.status = ST_INVALID_NETKEY;
		memset(&path_metric_sts.path_metric, 0x00, sizeof(path_metric_t));
	}

	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&path_metric_sts),sizeof(path_metric_sts),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_path_metric_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	path_metric_set_t *p_metric_set = (path_metric_set_t *)par;	
	path_metric_sts_t path_metric_sts;

	path_metric_sts.status = ST_SUCCESS;
	path_metric_sts.netkey_index = p_metric_set->netkey_index;
	memcpy(&path_metric_sts.path_metric, &p_metric_set->path_metric, sizeof(path_metric_t));
	
	int key_offset = get_mesh_net_key_offset(p_metric_set->netkey_index);	
	if(-1 != key_offset){
		memcpy(&model_sig_cfg_s.directed_forward.subnet_state[key_offset].path_metric, &p_metric_set->path_metric, sizeof(path_metric_t));
	}
	else{
		path_metric_sts.status = ST_INVALID_NETKEY;		
	}

	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&path_metric_sts),sizeof(path_metric_sts),cb_par->adr_src);
	mesh_common_store(FLASH_ADR_MD_CFG_S);
	return err;
}

int mesh_cmd_sig_cfg_path_metric_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_dsc_tbl_capa_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	dsc_tbl_capa_sts_t dsc_tbl_capa_sts;	
	u16 netkey_index = par[0] + (par[1]<<8);
	dsc_tbl_capa_sts.status = ST_SUCCESS;
	dsc_tbl_capa_sts.netkey_index = netkey_index;

	int key_offset = get_mesh_net_key_offset(netkey_index);	
	if(-1 != key_offset){
		dsc_tbl_capa_sts.max_concurrent_init = model_sig_cfg_s.directed_forward.subnet_state[key_offset].max_concurrent_init;
		dsc_tbl_capa_sts.max_dsc_tbl_entries_cnt = MAX_CONCURRENT_CNT; 
	}
	else{
		dsc_tbl_capa_sts.status = ST_INVALID_NETKEY;
		dsc_tbl_capa_sts.max_concurrent_init = 0;
		dsc_tbl_capa_sts.max_dsc_tbl_entries_cnt = 0; 
	}

	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&dsc_tbl_capa_sts),sizeof(dsc_tbl_capa_sts),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_dsc_tbl_capa_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	dsc_tbl_capa_sts_t dsc_tbl_capa_sts;
	dsc_tbl_capa_set_t *p_capa_set = (dsc_tbl_capa_set_t *)par;	
	dsc_tbl_capa_sts.status = ST_SUCCESS;
	dsc_tbl_capa_sts.netkey_index = p_capa_set->netkey_index;

	int key_offset = get_mesh_net_key_offset(p_capa_set->netkey_index);	
	if((-1 == key_offset)){
		dsc_tbl_capa_sts.status = ST_INVALID_NETKEY;
		dsc_tbl_capa_sts.max_concurrent_init = 0;
		dsc_tbl_capa_sts.max_dsc_tbl_entries_cnt = 0; 
	}
	else if((p_capa_set->max_concurrent_init<2) || (p_capa_set->max_concurrent_init>MAX_CONCURRENT_CNT)){
		dsc_tbl_capa_sts.status = ST_CAN_NOT_SET;
		dsc_tbl_capa_sts.max_concurrent_init = 0;
		dsc_tbl_capa_sts.max_dsc_tbl_entries_cnt = 0; 
	}
	else{
		model_sig_cfg_s.directed_forward.subnet_state[key_offset].max_concurrent_init = p_capa_set->max_concurrent_init;
		dsc_tbl_capa_sts.max_concurrent_init = p_capa_set->max_concurrent_init;
		dsc_tbl_capa_sts.max_dsc_tbl_entries_cnt = MAX_CONCURRENT_CNT; 
	}

	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&dsc_tbl_capa_sts),sizeof(dsc_tbl_capa_sts),cb_par->adr_src);
	mesh_common_store(FLASH_ADR_MD_CFG_S);
	return err;
}

int mesh_cmd_sig_cfg_dsc_tbl_capa_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_forwarding_tbl_add(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	forwarding_tbl_status_t forwarding_tbl_status;	
	forwarding_table_entry_t tbl_entry;
	forwarding_entry_par_t *p_fwd_entry;
	forwarding_tbl_add_t *p_tbl_add = (forwarding_tbl_add_t *)par;
	forwarding_tbl_status.status = ST_SUCCESS;
	forwarding_tbl_status.netkey_index = p_tbl_add->netkey_index;

	int key_offset = get_mesh_net_key_offset(p_tbl_add->netkey_index);	
	if(-1 != key_offset){
		memset(&tbl_entry, 0x00, sizeof(tbl_entry));
		addr_range_little_endian_t* p_adr_range = (addr_range_little_endian_t*)p_tbl_add->par;
	
		tbl_entry.fixed_path = 1;
		tbl_entry.backward_path_validated = p_tbl_add->backward_path_validated_flag;
		tbl_entry.path_not_ready = 0;
		tbl_entry.lane_counter = 1;
		tbl_entry.path_origin = p_adr_range->range_start;
		tbl_entry.path_origin_snd_ele_cnt = p_adr_range->length_present ? (p_adr_range->range_length-1):0;
		u8 cur_par_offset = p_adr_range->length_present ? 3:2;
		p_adr_range = (addr_range_little_endian_t*)(p_tbl_add->par + cur_par_offset);

		if(p_tbl_add->unicast_destination_flag){
			tbl_entry.destination = p_adr_range->range_start;
			tbl_entry.path_target_snd_ele_cnt = p_adr_range->length_present ? (p_adr_range->range_length-1):0;
			cur_par_offset += (p_adr_range->length_present ? 3:2);
			
		}
		else{
			tbl_entry.destination = p_adr_range->multicast_destination;		
			cur_par_offset += 2;
		}
		tbl_entry.bearer_toward_path_origin = p_tbl_add->par[cur_par_offset] + (p_tbl_add->par[cur_par_offset+1]<<8);
		cur_par_offset += 2;
		tbl_entry.bearer_toward_path_target = p_tbl_add->par[cur_par_offset] + (p_tbl_add->par[cur_par_offset+1]<<8);

		//mesh_match_type_t match_type;
    	//mesh_match_group_mac(&match_type, tbl_entry.destination, 0, 0, 0);
		p_fwd_entry = get_corresponding_forwarding_fixed_entry(key_offset, tbl_entry.path_origin, tbl_entry.destination);
		if((tbl_entry.bearer_toward_path_origin>BEAR_SUPPORT) || (tbl_entry.bearer_toward_path_target > BEAR_SUPPORT)
			// Welson to do: add error conditions: P417
		){ // invalid bear
			forwarding_tbl_status.status = ST_INVALID_BEARER;
		}
		else{
			if(p_fwd_entry){// exist, update entry
				p_fwd_entry->entry.backward_path_validated = tbl_entry.backward_path_validated;
				p_fwd_entry->entry.bearer_toward_path_origin = tbl_entry.bearer_toward_path_origin;
				p_fwd_entry->entry.bearer_toward_path_target = tbl_entry.bearer_toward_path_target;
				update_forwarding_tbl_update_identifier(key_offset);;
			}
			else{// not exist, add a new fixed path entry
				p_fwd_entry = add_new_path_entry_in_forwarding_tbl(key_offset, &tbl_entry);
				if(add_new_path_entry_in_forwarding_tbl(key_offset, &tbl_entry)){
					forwarding_tbl_status.status = ST_INSUFFICIENT_RES;
				}
				else{
					err = 0;
				}
			}
		}
		
	}
	else{
		forwarding_tbl_status.status = ST_INVALID_NETKEY;
	}
	forwarding_tbl_status.path_origin = tbl_entry.path_origin;
	forwarding_tbl_status.destination = tbl_entry.destination;	
	
	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&forwarding_tbl_status),sizeof(forwarding_tbl_status),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_forwarding_tbl_delete(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	forwarding_tbl_status_t forwarding_tbl_status;
	forwarding_tbl_delete_t *p_tbl_delete = (forwarding_tbl_delete_t *)par;	
	forwarding_tbl_status.status = ST_SUCCESS;
	forwarding_tbl_status.netkey_index = p_tbl_delete->netkey_index;
	forwarding_tbl_status.path_origin = p_tbl_delete->path_origin;
	forwarding_tbl_status.destination = p_tbl_delete->destination;
		
	int key_offset = get_mesh_net_key_offset(p_tbl_delete->netkey_index);	
	if((-1 == key_offset)){
		forwarding_tbl_status.status = ST_INVALID_NETKEY;
	}
	else{
		forwarding_table_entry_t tbl_entry;
		tbl_entry.path_origin = p_tbl_delete->path_origin;
		tbl_entry.destination = p_tbl_delete->destination;
		delete_path_entry_in_forwarding_tbl(key_offset, &tbl_entry);
	}

	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&forwarding_tbl_status),sizeof(forwarding_tbl_status),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_forwarding_tbl_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_forwarding_tbl_dependents_add(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	forwarding_tbl_status_t forwarding_tbl_status;	
	forwarding_entry_par_t *p_entry;
	forwarding_tbl_dependengts_add_t *p_tbl_dependents_add = (forwarding_tbl_dependengts_add_t *)par;
	forwarding_tbl_status.status = ST_SUCCESS;
	forwarding_tbl_status.netkey_index = p_tbl_dependents_add->netkey_index;

	int key_offset = get_mesh_net_key_offset(p_tbl_dependents_add->netkey_index);	
	if(-1 != key_offset){
		p_entry = get_corresponding_forwarding_fixed_entry(key_offset, p_tbl_dependents_add->path_origin, p_tbl_dependents_add->destination);
		
		if(p_entry){
			u8 range_par_offset = 0;
			addr_range_little_endian_t * p_addr_range = 0;
			foreach(i, p_tbl_dependents_add->dependent_origin_list_size){
				p_addr_range = (addr_range_little_endian_t *)(p_tbl_dependents_add->par+range_par_offset);
				range_par_offset += (p_addr_range->length_present ? 3:2);
				if(forwarding_tbl_dependent_add(p_addr_range->range_start, p_addr_range->length_present?p_addr_range->range_length:1, p_entry->entry.dependent_origin)){
					forwarding_tbl_status.status = ST_INSUFFICIENT_RES;
				}
			}

			if(p_tbl_dependents_add->dependent_target_list_size){
				foreach(i, p_tbl_dependents_add->dependent_target_list_size){
					p_addr_range = (addr_range_little_endian_t *)(p_tbl_dependents_add->par+range_par_offset);
					range_par_offset += (p_addr_range->length_present ? 3:2);
					if(forwarding_tbl_dependent_add(p_addr_range->range_start, p_addr_range->length_present?p_addr_range->range_length:1, p_entry->entry.dependent_target)){
						forwarding_tbl_status.status = ST_INSUFFICIENT_RES;
					}
				}
			}

			update_forwarding_tbl_update_identifier(key_offset);
		}				
	}
	else{
		forwarding_tbl_status.status = ST_INVALID_NETKEY;
	}
	forwarding_tbl_status.path_origin = p_tbl_dependents_add->path_origin;
	forwarding_tbl_status.destination = p_tbl_dependents_add->destination;	
	
	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&forwarding_tbl_status),sizeof(forwarding_tbl_status),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_forwarding_tbl_dependents_delete(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	forwarding_tbl_status_t forwarding_tbl_status;
	forwarding_entry_par_t *p_entry;
	forwarding_tbl_dependengts_delete_t *p_tbl_delete = (forwarding_tbl_dependengts_delete_t *)par;	
	forwarding_tbl_status.status = ST_SUCCESS;
	forwarding_tbl_status.netkey_index = p_tbl_delete->netkey_index;
	forwarding_tbl_status.path_origin = p_tbl_delete->path_origin;
	forwarding_tbl_status.destination = p_tbl_delete->destination;
		
	int key_offset = get_mesh_net_key_offset(p_tbl_delete->netkey_index);	
	if((-1 == key_offset)){
		forwarding_tbl_status.status = ST_INVALID_NETKEY;
	}
	else{		
		p_entry = get_corresponding_forwarding_fixed_entry(key_offset, p_tbl_delete->path_origin, p_tbl_delete->destination);
		
		if(p_entry){
			u16 addr = 0;
			if(p_tbl_delete->dependent_origin_list_size){
				foreach(i, p_tbl_delete->dependent_origin_list_size){
					addr = p_tbl_delete->addr[i];
					if(forwarding_tbl_dependent_delete(addr, p_entry->entry.dependent_origin)){
						
					}
				}
			}
			
			if(p_tbl_delete->dependent_target_list_size){
				foreach(i, p_tbl_delete->dependent_target_list_size){
					addr = p_tbl_delete->addr[p_tbl_delete->dependent_origin_list_size + i];
					if(forwarding_tbl_dependent_delete(addr, p_entry->entry.dependent_target)){
						
					}
				}
			}
		}		
	}
	forwarding_tbl_status.path_origin = p_tbl_delete->path_origin;
	forwarding_tbl_status.destination = p_tbl_delete->destination;	
	
	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&forwarding_tbl_status),sizeof(forwarding_tbl_status),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_forwarding_tbl_dependents_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int get_dependent_list_num(path_addr_t *p_list)
{
	int num = 0;
	for(num=0; num<MAX_DEPENDENT_LIST_SIZE; num++){
		if(0 == p_list[num].addr){
			break;
		}
	}
	return num;
}

int mesh_cmd_sig_cfg_forwarding_tbl_dependents_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	forwarding_tbl_dependents_get_t *p_dependents_get = (forwarding_tbl_dependents_get_t *) par;
	static forwarding_tbl_dependents_get_sts_t dependengts_get_sts;
	forwarding_entry_par_t *p_fwd_entry;

	u32 cur_par_len = OFFSETOF(forwarding_tbl_dependents_get_sts_t, par);
	u8 identifier_exist = (par_len >= sizeof(forwarding_tbl_dependents_get_t));	
	u8 via_par_len = identifier_exist ? 2:0;
	memset(&dependengts_get_sts, 0x00, sizeof(dependengts_get_sts));
	dependengts_get_sts.status = ST_SUCCESS;
	dependengts_get_sts.netkey_index = p_dependents_get->netkey_index;
	memcpy(&dependengts_get_sts.status+1, p_dependents_get, sizeof(forwarding_tbl_dependents_get_t));

	int key_offset = get_mesh_net_key_offset(p_dependents_get->netkey_index);	
	if(-1 != key_offset){	
		p_fwd_entry = get_corresponding_forwarding_fixed_entry(key_offset, p_dependents_get->path_origin, p_dependents_get->destination);	
		if(p_fwd_entry){
			
			if(identifier_exist){
				set_forwarding_tbl_update_identifier( p_dependents_get->update_identifier, key_offset);
			}

			u8 dependent_origin_num = get_dependent_list_num(p_fwd_entry->entry.dependent_origin);
			u8 dependent_target_num = get_dependent_list_num(p_fwd_entry->entry.dependent_target);				
			if((p_dependents_get->dependents_list_mask & DEPENDENT_GET_PATH_ORIGIN_MATCH) && (p_dependents_get->start_index<get_dependent_list_num(p_fwd_entry->entry.dependent_origin))){
				dependengts_get_sts.par[via_par_len] = dependent_origin_num - p_dependents_get->start_index;;
				via_par_len += 1;
			}
			else{
				dependent_origin_num = 0;
			}

			if((p_dependents_get->dependents_list_mask & DEPENDENT_GET_DESTINATION_MATCH) && (p_dependents_get->start_index<get_dependent_list_num(p_fwd_entry->entry.dependent_target))){
				dependengts_get_sts.par[via_par_len] = dependent_target_num - p_dependents_get->start_index;
				via_par_len += 1;
			}
			else{
				dependent_target_num = 0;
			}

			if(dependent_origin_num){
				for(u16 index=p_dependents_get->start_index; index<dependent_origin_num; index++){
					addr_range_little_endian_t *p_addr_range = (addr_range_little_endian_t *)(dependengts_get_sts.par+via_par_len);
					p_addr_range->range_start = p_fwd_entry->entry.dependent_origin[index].addr;
					if(p_fwd_entry->entry.dependent_origin[index].snd_ele_cnt){
						p_addr_range->length_present = 1;
						p_addr_range->range_length = p_fwd_entry->entry.dependent_origin[index].snd_ele_cnt + 1;
						via_par_len += 3;
					}
					else{
						via_par_len += 2;
					}
				}
			}
			
			if(dependent_target_num){
				for(u16 index=p_dependents_get->start_index; index<dependent_target_num; index++){
					addr_range_little_endian_t *p_addr_range = (addr_range_little_endian_t *)(dependengts_get_sts.par+via_par_len);
					p_addr_range->range_start = p_fwd_entry->entry.dependent_target[index].addr;
					if(p_fwd_entry->entry.dependent_target[index].snd_ele_cnt){
						p_addr_range->length_present = 1;
						p_addr_range->range_length = p_fwd_entry->entry.dependent_target[index].snd_ele_cnt + 1;
						via_par_len += 3;
					}
					else{
						via_par_len += 2;
					}
				}
			}
			
		}				
		
	}
	else{
		dependengts_get_sts.status = ST_INVALID_NETKEY;
	}

	cur_par_len += via_par_len;
	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&dependengts_get_sts),cur_par_len,cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_forwarding_tbl_dependents_get_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

entries_cnt_t get_forwarding_tbl_entries_count(u16 netkey_index)
{
	entries_cnt_t count = {0};
	u8 netkey_offset = get_mesh_net_key_offset(netkey_index);
	if(-1 != netkey_offset){
		foreach(i, MAX_FORWARDING_TBL_ENTRIES_CNT){
			forwarding_entry_par_t *p_fwd_entry = &forwarding_table[netkey_offset].fwd_entry_par[i];
			if(p_fwd_entry->entry.path_origin){// not empty entry
				if(p_fwd_entry->entry.fixed_path){
					count.fixed_cnt++;
				}
				else{
					count.non_fixed_cnt++;
				}
			}
		}
	}

	return count;
}

int mesh_cmd_sig_cfg_forwarding_tbl_entries_count_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	forwarding_tbl_entries_count_st_t entries_count_sts;	
	u16 netkey_index = par[0] + (par[1]<<8);
	memset(&entries_count_sts, 0x00, sizeof(entries_count_sts));
	entries_count_sts.status = ST_SUCCESS;
	entries_count_sts.netkey_index = netkey_index;

	int key_offset = get_mesh_net_key_offset(netkey_index);	
	if(-1 != key_offset){
		entries_count_sts.forwarding_tbl_update_identifier = forwarding_table[key_offset].update_identifier;
		entries_count_sts.entries_cnt = get_forwarding_tbl_entries_count(netkey_index);
	}
	else{
		entries_count_sts.status = ST_INVALID_NETKEY;
	}

	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&entries_count_sts),(ST_SUCCESS==entries_count_sts.status) ? sizeof(entries_count_sts):OFFSETOF(forwarding_tbl_entries_count_st_t, forwarding_tbl_update_identifier),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_forwarding_tbl_entries_count_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_forwarding_tbl_entries_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	forwarding_tbl_entries_count_st_t entries_count_sts;	
	u16 netkey_index = par[0] + (par[1]<<8);
	entries_count_sts.status = ST_SUCCESS;
	entries_count_sts.netkey_index = netkey_index;

	int key_offset = get_mesh_net_key_offset(netkey_index);	
	if(-1 != key_offset){
	
	}
	else{

	}

	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&entries_count_sts),sizeof(entries_count_sts),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_forwarding_tbl_entries_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_wanted_lanes_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	wanted_lanes_sts_t wanted_lanes_sts;	
	u16 netkey_index = par[0] + (par[1]<<8);
	wanted_lanes_sts.status = ST_SUCCESS;
	wanted_lanes_sts.netkey_index = netkey_index;

	int key_offset = get_mesh_net_key_offset(netkey_index);	
	if(-1 != key_offset){
		wanted_lanes_sts.wanted_lanes = model_sig_cfg_s.directed_forward.subnet_state[key_offset].wanted_lanes;
	}
	else{
		wanted_lanes_sts.status = ST_INVALID_NETKEY;
		wanted_lanes_sts.wanted_lanes = 0;
	}

	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&wanted_lanes_sts),sizeof(wanted_lanes_sts),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_wanted_lanes_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	wanted_lanes_sts_t wanted_lanes_sts;
	wanted_lanes_set_t *p_lanes_set = (wanted_lanes_set_t *)par;
	memset(&wanted_lanes_sts, 0x00, sizeof(wanted_lanes_sts));
	wanted_lanes_sts.status = ST_SUCCESS;
	wanted_lanes_sts.netkey_index = p_lanes_set->netkey_index;
	wanted_lanes_sts.wanted_lanes = p_lanes_set->wanted_lanes;
	
	int key_offset = get_mesh_net_key_offset(p_lanes_set->netkey_index);	
	if((-1 == key_offset)){
		wanted_lanes_sts.status = ST_INVALID_NETKEY;
	}
	else{
		model_sig_cfg_s.directed_forward.subnet_state[key_offset].wanted_lanes = p_lanes_set->wanted_lanes;
	}

	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&wanted_lanes_sts),sizeof(wanted_lanes_sts_t),cb_par->adr_src);
	mesh_common_store(FLASH_ADR_MD_CFG_S);
	return err;
}

int mesh_cmd_sig_cfg_wanted_lanes_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_two_way_path_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	two_way_path_sts_t two_way_path_sts;	
	u16 netkey_index = par[0] + (par[1]<<8);
	memset(&two_way_path_sts, 0, sizeof(two_way_path_sts));
	two_way_path_sts.status = ST_SUCCESS;
	two_way_path_sts.netkey_index = netkey_index;
	
	int key_offset = get_mesh_net_key_offset(netkey_index);
	if(-1 != key_offset){
		two_way_path_sts.two_way_path = model_sig_cfg_s.directed_forward.subnet_state[key_offset].two_way_path;
	}
	else{
		two_way_path_sts.status = ST_INVALID_NETKEY;
		two_way_path_sts.two_way_path = 0;
	}

	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&two_way_path_sts),sizeof(two_way_path_sts),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_two_way_path_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	two_way_path_sts_t two_way_path_sts;	
	two_way_path_set_t *p_set = (two_way_path_set_t*)par;
	memset(&two_way_path_sts, 0, sizeof(two_way_path_sts));
	two_way_path_sts.status = ST_SUCCESS;
	two_way_path_sts.netkey_index = p_set->netkey_index;
	two_way_path_sts.two_way_path = p_set->two_way_path;

	int key_offset = get_mesh_net_key_offset(p_set->netkey_index);	
	if((-1 != key_offset)){
		model_sig_cfg_s.directed_forward.subnet_state[key_offset].two_way_path = p_set->two_way_path;
	}
	else{
		two_way_path_sts.status = ST_INVALID_NETKEY;
	}
	
	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&two_way_path_sts),sizeof(two_way_path_sts),cb_par->adr_src);
	mesh_common_store(FLASH_ADR_MD_CFG_S);
	return err;
}

int mesh_cmd_sig_cfg_two_way_path_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_path_echo_interval_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	path_echo_interval_sts_t path_echo_interval_sts;	
	u16 netkey_index = par[0] + (par[1]<<8);
	memset(&path_echo_interval_sts, 0, sizeof(path_echo_interval_sts));
	path_echo_interval_sts.status = ST_SUCCESS;
	path_echo_interval_sts.netkey_index = netkey_index;
	
	int key_offset = get_mesh_net_key_offset(netkey_index);	
	if(-1 != key_offset){
		path_echo_interval_sts.path_echo_interval = model_sig_cfg_s.directed_forward.subnet_state[key_offset].path_echo_interval;
	}
	else{
		path_echo_interval_sts.status = ST_INVALID_NETKEY;
		path_echo_interval_sts.path_echo_interval = 0;
	}

	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&path_echo_interval_sts),sizeof(path_echo_interval_sts),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_path_echo_interval_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	path_echo_interval_sts_t path_echo_interval_sts;	
	path_echo_interval_set_t *p_set = (path_echo_interval_set_t*)par;
	memset(&path_echo_interval_sts, 0, sizeof(path_echo_interval_sts));
	path_echo_interval_sts.status = ST_SUCCESS;
	path_echo_interval_sts.netkey_index = p_set->netkey_index;
	path_echo_interval_sts.path_echo_interval = p_set->path_echo_interval;

	int key_offset = get_mesh_net_key_offset(p_set->netkey_index);	
	if((-1 != key_offset)){
		model_sig_cfg_s.directed_forward.subnet_state[key_offset].path_echo_interval = p_set->path_echo_interval;
	}
	else{
		path_echo_interval_sts.status = ST_INVALID_NETKEY;
	}
	
	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&path_echo_interval_sts),sizeof(path_echo_interval_sts),cb_par->adr_src);
	mesh_common_store(FLASH_ADR_MD_CFG_S);
	return err;
}

int mesh_cmd_sig_cfg_path_echo_interval_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_directed_network_transmit_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	mesh_transmit_t transmit;	

	transmit.val = model_sig_cfg_s.directed_forward.transmit.val;
	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&transmit),sizeof(transmit),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_directed_network_transmit_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	mesh_transmit_t transmit;	
	
	model_sig_cfg_s.directed_forward.transmit.val = par[0];
	transmit.val = model_sig_cfg_s.directed_forward.transmit.val;
	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&transmit),sizeof(transmit),cb_par->adr_src);
	mesh_common_store(FLASH_ADR_MD_CFG_S);
	return err;
}

int mesh_cmd_sig_cfg_directed_network_transmit_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_directed_relay_retransmit_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	mesh_transmit_t transmit;	

	transmit.val = model_sig_cfg_s.directed_forward.relay_transmit.val;
	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&transmit),sizeof(transmit),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_directed_relay_retransmit_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	mesh_transmit_t transmit;	
	
	model_sig_cfg_s.directed_forward.relay_transmit.val = par[0];
	transmit.val = model_sig_cfg_s.directed_forward.relay_transmit.val;
	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&transmit),sizeof(transmit),cb_par->adr_src);
	mesh_common_store(FLASH_ADR_MD_CFG_S);
	return err;
}

int mesh_cmd_sig_cfg_directed_relay_retransmit_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_rssi_threshold_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	rssi_threshold_t sts;	

	sts = model_sig_cfg_s.directed_forward.rssi_threshold;
	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&sts),sizeof(sts),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_rssi_threshold_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	rssi_threshold_t sts;	
	
	model_sig_cfg_s.directed_forward.rssi_threshold.rssi_margin = par[0];
	sts = model_sig_cfg_s.directed_forward.rssi_threshold;
	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&sts),sizeof(sts),cb_par->adr_src);
	mesh_common_store(FLASH_ADR_MD_CFG_S);
	return err;
}

int mesh_cmd_sig_cfg_rssi_threshold_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_directed_paths_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	directed_paths_t sts;	
	
	sts = model_sig_cfg_s.directed_forward.directed_paths;
	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&sts),sizeof(sts),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_directed_paths_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

u8 mesh_model_publish_policy_get(u16 ele_adr, u32 model_id, int sig_model, u16 adr_src)
{
    u8 st = ST_SUCCESS;
    u8 model_idx = 0;
    model_common_t *p_model = (model_common_t *)mesh_find_ele_resource_in_model(ele_adr, model_id, sig_model, &model_idx, 0);

    if(!p_model){   // response error status after this function.
    	st = ST_INVALID_MODEL;
    }else if(p_model->no_pub){
    	st = ST_INVALID_MODEL;	// model not support publish
    }else{
        directed_pub_policy_st_t pub_oplicy_st;
        pub_oplicy_st.status = ST_SUCCESS;
		pub_oplicy_st.directed_pub_policy = p_model->directed_pub_policy;
        pub_oplicy_st.ele_addr = ele_adr;
        pub_oplicy_st.model_id = model_id;
        
        mesh_tx_cmd_rsp_cfg_model(DIRECTED_PUBLISH_POLICY_STATUS, (u8 *)&pub_oplicy_st, sizeof(pub_oplicy_st)-FIX_SIZE(sig_model), adr_src);
    }

    return st;
}

int mesh_cmd_sig_cfg_directed_publish_policy_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	u8 st = ST_UNSPEC_ERR;
    int sig_model = (sizeof(directed_pub_policy_get_t) - 2 == par_len);
    directed_pub_policy_get_t *p_policy_get = (directed_pub_policy_get_t *)par;
    u32 model_id = sig_model ? (p_policy_get->model_id & 0xFFFF) : p_policy_get->model_id;
    
    if(is_unicast_adr(p_policy_get->ele_addr)){
        mesh_adr_list_t adr_list;
        st = find_ele_support_model_and_match_dst(&adr_list, p_policy_get->ele_addr, model_id, sig_model);
    	
        if((adr_list.adr_cnt)){
            st = mesh_model_publish_policy_get(p_policy_get->ele_addr, model_id, sig_model, cb_par->adr_src);
        }
    }else{
        st = ST_INVALID_ADR;
    }

	if(st != ST_SUCCESS){
		directed_pub_policy_st_t pub_oplicy_st;
        pub_oplicy_st.status = st;
		pub_oplicy_st.directed_pub_policy = 0;
        pub_oplicy_st.ele_addr = p_policy_get->ele_addr;
        pub_oplicy_st.model_id = model_id;
        mesh_tx_cmd_rsp_cfg_model(DIRECTED_PUBLISH_POLICY_STATUS, (u8 *)&pub_oplicy_st, sizeof(pub_oplicy_st)-FIX_SIZE(sig_model), cb_par->adr_src);
	}
	
	return 0;
}

u8 mesh_pub_policy_search_and_set(u16 ele_adr, u8 *par_set, u32 model_id, int sig_model)
{
    u8 st = ST_UNSPEC_ERR;
	directed_pub_policy_set_t *p_policy_set = (directed_pub_policy_set_t *)par_set;
    mesh_adr_list_t adr_list;
    st = find_ele_support_model_and_match_dst(&adr_list, ele_adr, model_id, sig_model);
	if(st != ST_SUCCESS) return st;
	
	int change_falg = 0;
    if(adr_list.adr_cnt){
        u8 model_idx = 0;
        model_common_t *p_model = (model_common_t *)mesh_find_ele_resource_in_model(ele_adr, model_id, sig_model,&model_idx, 0);
        if(p_model){// && (!p_model->no_pub)){
        	st = ST_SUCCESS;
			if(p_model->directed_pub_policy != p_policy_set->directed_pub_policy){
				change_falg = 1;
			}
        	p_model->directed_pub_policy = p_policy_set->directed_pub_policy;
        }else{
			st = ST_INVALID_PUB_PAR;
        }
    }

    if((ST_SUCCESS == st) && change_falg){
		mesh_model_store(sig_model, model_id);
    }

	return st;
}

int mesh_cmd_sig_cfg_directed_publish_policy_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	u8 st = ST_SUCCESS;
	int sig_model = (sizeof(directed_pub_policy_set_t) - 2 == par_len);
    directed_pub_policy_set_t *p_policy_set = (directed_pub_policy_set_t *)par;
    u32 model_id = sig_model ? (p_policy_set->model_id & 0xFFFF) : p_policy_set->model_id;
	
	u16 ele_adr = p_policy_set->ele_addr;
    if(is_unicast_adr(ele_adr)){
        st = mesh_pub_policy_search_and_set(ele_adr, (u8 *)p_policy_set, model_id, sig_model);
        if(st == ST_SUCCESS){
            st = mesh_model_publish_policy_get(p_policy_set->ele_addr, model_id, sig_model, cb_par->adr_src);
        }
    }else{
        st = ST_INVALID_ADR;
    }

	if(st != ST_SUCCESS){
		directed_pub_policy_st_t pub_oplicy_st;
        pub_oplicy_st.status = st;
		pub_oplicy_st.directed_pub_policy = 0;
        pub_oplicy_st.ele_addr = p_policy_set->ele_addr;
        pub_oplicy_st.model_id = model_id;
        mesh_tx_cmd_rsp_cfg_model(DIRECTED_PUBLISH_POLICY_STATUS, (u8 *)&pub_oplicy_st, sizeof(pub_oplicy_st)-FIX_SIZE(sig_model), cb_par->adr_src);
	}
	
	return 0;
}

int mesh_cmd_sig_cfg_directed_publish_policy_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_path_discovery_timing_control_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	discovery_timing_t sts;
	
	sts = model_sig_cfg_s.directed_forward.discovery_timing;
	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&sts),sizeof(sts),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_path_discovery_timing_control_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	discovery_timing_t sts;
	memcpy(&sts, par, sizeof(discovery_timing_t));
	model_sig_cfg_s.directed_forward.discovery_timing = sts;

	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&sts),sizeof(sts),cb_par->adr_src);
	mesh_common_store(FLASH_ADR_MD_CFG_S);
	return err;
}

int mesh_cmd_sig_cfg_path_discovery_timing_control_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_directed_control_network_transmit_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	mesh_transmit_t transmit;	

	transmit.val = model_sig_cfg_s.directed_forward.control_transmit.val;
	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&transmit),sizeof(transmit),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_directed_control_network_transmit_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	mesh_transmit_t transmit;	
	
	model_sig_cfg_s.directed_forward.control_transmit.val = par[0];
	transmit.val = model_sig_cfg_s.directed_forward.control_transmit.val;
	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&transmit),sizeof(transmit),cb_par->adr_src);
	mesh_common_store(FLASH_ADR_MD_CFG_S);
	return err;
}

int mesh_cmd_sig_cfg_directed_control_network_transmit_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_directed_control_relay_transmit_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	mesh_transmit_t transmit;	

	transmit.val = model_sig_cfg_s.directed_forward.control_relay_transmit.val;
	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&transmit),sizeof(transmit),cb_par->adr_src);
	return err;
}

int mesh_cmd_sig_cfg_directed_control_relay_transmit_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	mesh_transmit_t transmit;	
	
	model_sig_cfg_s.directed_forward.control_relay_transmit.val = par[0];
	transmit.val = model_sig_cfg_s.directed_forward.control_relay_transmit.val;
	err = mesh_tx_cmd_rsp_cfg_model(cb_par->op_rsp,(u8 *)(&transmit),sizeof(transmit),cb_par->adr_src);
	mesh_common_store(FLASH_ADR_MD_CFG_S);
	return err;
}

int mesh_cmd_sig_cfg_directed_control_relay_transmit_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}
//-------------------------forwarding config message end-----------------------------------//

//---------------------------directed forwarding procedures-------------------//
int path_request_msg_set(discovery_entry_par_t * p_dsc_entry, mesh_ctl_path_req_t * p_path_req)
{
	u8 via_par_len = 0;
	memset(p_path_req, 0x00, sizeof(mesh_ctl_path_req_t));
	
	p_path_req->path_metric_type = p_dsc_entry->entry.path_metric_type;
	p_path_req->path_lifetime = p_dsc_entry->entry.path_lifetime;
	p_path_req->discovery_interval = p_dsc_entry->entry.path_discovery_interval;
	p_path_req->forwarding_number = p_dsc_entry->entry.forwarding_number;
	if(is_own_ele(p_dsc_entry->entry.path_origin.addr)){
		p_path_req->origin_path_metric = 0;
	}
	else{
		p_path_req->origin_path_metric = p_dsc_entry->entry.path_metric + 1;
	}
	p_path_req->destination = p_dsc_entry->entry.destination;	
	
	addr_range_big_endian_t *p_addr_range = (addr_range_big_endian_t *)p_path_req->addr_par; // path origin
	p_addr_range->range_start = p_dsc_entry->entry.path_origin.addr;	
	if(p_dsc_entry->entry.path_origin.snd_ele_cnt){
		p_addr_range->length_present = 1;		
		p_addr_range->range_length = p_dsc_entry->entry.path_origin.snd_ele_cnt + 1;
		via_par_len += 3;
	}
	else{
		via_par_len += 2;
	}
	
	if(p_dsc_entry->entry.dependent_origin.addr){// path dependent
		p_addr_range = (addr_range_big_endian_t *)(p_path_req->addr_par + via_par_len); 
		p_path_req->on_behalf_of_dependent_origin = 1;
		p_addr_range->range_start = p_dsc_entry->entry.dependent_origin.addr;
		if(p_dsc_entry->entry.dependent_origin.snd_ele_cnt){
			p_addr_range->length_present = 1;
			p_addr_range->range_length = p_dsc_entry->entry.dependent_origin.snd_ele_cnt + 1;
			via_par_len += 3;
		}
		else{
			via_par_len += 2;
		}
	}
	return (OFFSETOF(mesh_ctl_path_req_t, addr_par)+via_par_len);
}

int prepare_and_send_path_reply(u16 netkey_offset, discovery_entry_par_t *p_dsc_entry)
{
	mesh_ctl_path_reply_t path_reply;
	u8 via_par_len = 0;
	memset(&path_reply, 0x00, sizeof(mesh_ctl_path_reply_t));
	if(is_unicast_adr(p_dsc_entry->entry.destination)){
		forwarding_entry_par_t *p_fwd_entry = get_forwarding_entry_correspond2_discovery_entry(netkey_offset, &p_dsc_entry->entry);
		path_reply.unicast_destination = 1;
		addr_range_big_endian_t *p_path_target = (addr_range_big_endian_t *)path_reply.addr_par;
		if(p_fwd_entry){
			p_path_target->range_start = p_fwd_entry->entry.destination;
			via_par_len += 2;
			if(p_fwd_entry->entry.path_target_snd_ele_cnt){
				p_path_target->length_present = 1;
				p_path_target->range_length = p_fwd_entry->entry.path_target_snd_ele_cnt;
				via_par_len += 1;
			}
		}
		else{ // path target
			p_path_target->range_start = ele_adr_primary;
			via_par_len += 2;
			if(g_ele_cnt > 1){
				p_path_target->length_present = 1;
				p_path_target->range_length = g_ele_cnt;
				via_par_len += 1;
			}
		}
		int offset = -1;
		if(p_fwd_entry){
			offset = get_offset_in_dependent_list(p_fwd_entry->entry.dependent_target, p_dsc_entry->entry.destination);
		}
		else{
			path_reply.confirmation_request = model_sig_cfg_s.directed_forward.subnet_state[netkey_offset].two_way_path;
		}			
		
		if(-1 != offset){
			path_reply.on_behalf_of_dependent_target = 1;
			addr_range_big_endian_t *p_dependent_target = (addr_range_big_endian_t *)(path_reply.addr_par + via_par_len);
			p_dependent_target->range_start = p_fwd_entry->entry.dependent_target[offset].addr;
			via_par_len += 2;
			if(p_fwd_entry->entry.dependent_target[offset].snd_ele_cnt){
				via_par_len += 1;
				p_dependent_target->length_present = 1;
				p_dependent_target->range_length = p_fwd_entry->entry.dependent_target[offset].snd_ele_cnt + 1;
			}
		}	
	}
	else{
		path_reply.unicast_destination = 0;
		via_par_len += 2;
	}
	
	path_reply.path_origin = p_dsc_entry->entry.path_origin.addr;
	path_reply.forwarding_number = p_dsc_entry->entry.forwarding_number;
	if(path_reply.unicast_destination){
		path_reply.confirmation_request = 1;
	}

	return mesh_tx_cmd_layer_upper_ctl(CMD_CTL_PATH_REPLY, (u8 *)&path_reply, OFFSETOF(mesh_ctl_path_reply_t, addr_par)+via_par_len, ele_adr_primary, p_dsc_entry->entry.next_toward_path_origin, 0);
}

int cfg_cmd_send_path_request(mesh_ctl_path_req_t *p_path_req, u8 len)
{
	LOG_MSG_LIB(TL_LOG_DIRECTED_FORWARDING, (u8 *)p_path_req, len, "send path request:", 0);
	return mesh_tx_cmd_layer_upper_ctl(CMD_CTL_PATH_REQUEST, (u8 *)p_path_req, len, ele_adr_primary, ADR_ALL_DIRECTED_FORWARD, 0);
}

int cfg_cmd_send_path_echo_request(forwarding_entry_par_t * p_fwd_entry)
{
	LOG_MSG_LIB(TL_LOG_DIRECTED_FORWARDING, 0, 0, "cfg_cmd_send_path_echo_request", 0);
	return mesh_tx_cmd_layer_upper_ctl(CMD_CTL_PATH_ECHO_REQUEST, 0, 0, ele_adr_primary, p_fwd_entry->entry.destination, 0);
}

int cfg_cmd_send_path_echo_reply(forwarding_entry_par_t * p_fwd_entry)
{
	LOG_MSG_LIB(TL_LOG_DIRECTED_FORWARDING, 0, 0, "cfg_cmd_send_path_echo_reply", 0);
	return mesh_tx_cmd_layer_upper_ctl(CMD_CTL_PATH_ECHO_REPLY, (u8 *)&p_fwd_entry->entry.path_origin, 2, ele_adr_primary, p_fwd_entry->entry.path_origin, 0);
}

int path_discovery_by_discovery_entry(u16 netkey_offset, discovery_entry_par_t *p_dsc_entry){
	mesh_ctl_path_req_t path_req;
	p_dsc_entry->state.path_need = 0;
	start_path_discovery_timer(p_dsc_entry); 
	u8 len = path_request_msg_set(p_dsc_entry, &path_req);
	return cfg_cmd_send_path_request(&path_req, len);
}

int path_discovery_by_forwarding_entry(u16 netkey_offset, forwarding_entry_par_t *p_fwd_entry){
	return 1;
}

extern u8 mesh_rsp_random_delay_step;
int directed_forwarding_confirm_start(u16 netkey_offset, forwarding_entry_par_t *p_fwd_entry)
{
	int err = -1;
	discovery_entry_par_t *p_dsc_entry = get_discovery_entry_correspond2_forwarding_entry(netkey_offset, &p_fwd_entry->entry);
	mesh_ctl_path_confirmation_t path_confirm;
	path_confirm.path_target = p_fwd_entry->entry.destination;
	path_confirm.path_origin = p_fwd_entry->entry.path_origin;
	LOG_MSG_LIB(TL_LOG_DIRECTED_FORWARDING, (u8 *)&path_confirm, sizeof(mesh_ctl_path_confirmation_t), "directed_forwarding_confirm_start:", 0);
	mesh_need_random_delay = 1;
	mesh_rsp_random_delay_step = rand()%3;
	err =  mesh_tx_cmd_layer_upper_ctl(CMD_CTL_PATH_CONFIRMATION, (u8 *)&path_confirm, sizeof(mesh_ctl_path_confirmation_t), ele_adr_primary, ADR_ALL_DIRECTED_FORWARD, 0);
	if(0 == err){
		p_dsc_entry->state.path_confirm_sent = 1;
	}
	mesh_need_random_delay = 0;
	return err;
}

int directed_forwarding_solication_start(u16 netkey_offset, mesh_ctl_path_request_solication_t *p_addr_list, u8 list_num)
{
	int err = -1;
	if(0){// to be done
		err =  mesh_tx_cmd_layer_upper_ctl(CMD_CTL_PATH_REQUEST_SOLICITATION, (u8 *)&p_addr_list, list_num<<1, ele_adr_primary, ADR_ALL_DIRECTED_FORWARD, 0);
	}
	return err; 
}

int is_path_target(u16 destination)
{
	u8 model_idx = 0;
	u8 *p_model = mesh_find_ele_resource_in_model(ele_adr_primary, SIG_MD_G_ONOFF_S, 1,&model_idx, 0);// search all model id later
	if( is_own_ele(destination)|| (p_model&&is_subscription_adr((model_common_t *)p_model, destination)) || mesh_mac_match_friend(destination) || mesh_group_match_friend(destination)){
		return 1;
	}
	return 0;
}


void forwarding_table_timing_proc(u16 netkey_offset)
{
 	foreach(fwd_tbl_offset, MAX_FORWARDING_TBL_ENTRIES_CNT){
		forwarding_entry_par_t *p_fwd_entry = &forwarding_table[netkey_offset].fwd_entry_par[fwd_tbl_offset];
		if(p_fwd_entry->entry.path_origin && !p_fwd_entry->entry.fixed_path){
			#if DF_TEST_MODE_EN
			u32 path_lifetime_s = 60;
			#else
			u32 path_lifetime_s = GET_PATH_LIFETIME(model_sig_cfg_s.directed_forward.subnet_state[netkey_offset].path_metric.path_lifetime);
			#endif
			if(is_own_ele(p_fwd_entry->entry.path_origin)){ // path origin
				int path_use_timer = max2(model_sig_cfg_s.directed_forward.discovery_timing.path_discovery_interval, path_lifetime_s-model_sig_cfg_s.directed_forward.discovery_timing.path_discovery_interval-model_sig_cfg_s.directed_forward.discovery_timing.path_monitoring_interval);
				if(p_fwd_entry->state.lifetime_s && df_clock_time_exceed_s(p_fwd_entry->state.lifetime_s, path_use_timer)){
					
				}
			}
			
			if(p_fwd_entry->state.lifetime_s && df_clock_time_exceed_s(p_fwd_entry->state.lifetime_s, path_lifetime_s) ){
				LOG_MSG_LIB(TL_LOG_DIRECTED_FORWARDING, 0, 0, "life timer expiredp_fwd_entry->state.lifetime_s:%d  path_lifetime_s:%d cur_timer_s:%d", p_fwd_entry->state.lifetime_s, path_lifetime_s, clock_time_s());
				memset(p_fwd_entry, 0x00, sizeof(forwarding_entry_par_t));
			}

			u32 validation_interval = path_lifetime_s * model_sig_cfg_s.directed_forward.subnet_state[netkey_offset].path_echo_interval / 100;
			if(p_fwd_entry->state.path_echo_timer_s && df_clock_time_exceed_s(p_fwd_entry->state.path_echo_timer_s, validation_interval)){
				cfg_cmd_send_path_echo_request(p_fwd_entry); // Path Validation Started
				start_path_echo_timer(p_fwd_entry);
			}

			if(p_fwd_entry->state.path_echo_reply_timeout_timer && df_clock_time_exceed_s(p_fwd_entry->state.path_echo_reply_timeout_timer, (model_sig_cfg_s.directed_forward.discovery_timing.path_discovery_interval?PATH_DISCOVERY_INTERVAL_30S:PATH_DISCOVERY_INTERVAL_5S)*1000*1000)){
				stop_path_echo_reply_timeout_timer(p_fwd_entry); //   Path Validation Failed
				path_discovery_by_forwarding_entry(netkey_offset, p_fwd_entry);
			}
		}
	}
}

void discovery_table_timing_proc(u16 netkey_offset)
{
	foreach(dsc_tbl_offset, MAX_DISCOVERY_TBL_ENTRIES_CNT){
		discovery_entry_par_t *p_dsc_entry = &discovery_table[netkey_offset].dsc_entry_par[dsc_tbl_offset];
		forwarding_entry_par_t *p_fwd_entry = 0;
		if(p_dsc_entry && (p_dsc_entry->state.discovery_timer || p_dsc_entry->state.discovery_guard_timer || p_dsc_entry->state.reply_delay_timer || p_dsc_entry->state.request_delay_timer)){
			if(p_dsc_entry->state.discovery_timer && clock_time_exceed(p_dsc_entry->state.discovery_timer, (model_sig_cfg_s.directed_forward.discovery_timing.path_discovery_interval?PATH_DISCOVERY_INTERVAL_30S:PATH_DISCOVERY_INTERVAL_5S)*1000*1000)){
				LOG_MSG_LIB(TL_LOG_DIRECTED_FORWARDING, 0, 0, "path discovery timer expired", 0);
				stop_path_discovery_timer(p_dsc_entry);						
				if(is_own_ele(p_dsc_entry->entry.path_origin.addr)){ // path origin
					p_fwd_entry = get_corresponding_forwarding_non_fixed_entry(netkey_offset, p_dsc_entry->entry.path_origin.addr, p_dsc_entry->entry.destination);									
					if(p_fwd_entry){
						if(p_fwd_entry->entry.path_not_ready){
							p_fwd_entry->entry.path_not_ready = 0;
						}
						
						if(p_dsc_entry->state.new_lane_established){
							LOG_MSG_LIB(TL_LOG_DIRECTED_FORWARDING, 0, 0, "new path lane establish counter:%d", p_fwd_entry->entry.lane_counter);
							if(p_fwd_entry->entry.lane_counter < model_sig_cfg_s.directed_forward.subnet_state[netkey_offset].wanted_lanes){
								LOG_MSG_LIB(TL_LOG_DIRECTED_FORWARDING, 0, 0, "start discovery guard timer", 0);
								start_path_discovery_guard_timer(p_dsc_entry);// start discovery guard timer.
							}	
							else{
								directed_discovery_entry_remove(p_dsc_entry);// Path Discovery Succeeded
							}
						}
						else{
							directed_discovery_entry_remove(p_dsc_entry);							 
						}
					}
					else{
						#if DF_TEST_MODE_EN
						directed_discovery_entry_remove(p_dsc_entry); // always create path origin state machine
						#else
						start_path_discovery_retry_timer(p_dsc_entry); // remove dsc entry after retry timer expired						
						#endif
					}
				}
				else { // directed relay or path target
					directed_discovery_entry_remove(p_dsc_entry);
				}
			}

			if(p_dsc_entry->state.discovery_guard_timer && clock_time_exceed(p_dsc_entry->state.discovery_guard_timer, (model_sig_cfg_s.directed_forward.discovery_timing.lane_discovery_guard_interval?LANE_GUARD_INTERVAL_10S:LANE_GUARD_INTERVAL_2S)*1000*1000)){
				stop_path_discovery_guard_timer(p_dsc_entry); 
				start_path_discovery_timer(p_dsc_entry); 
				path_discovery_by_discovery_entry(netkey_offset, p_dsc_entry);
			}

			if(p_dsc_entry->state.discovery_retry_timer && clock_time_exceed(p_dsc_entry->state.discovery_retry_timer, model_sig_cfg_s.directed_forward.discovery_timing.path_discovery_retry_interval)){
				stop_path_discovery_retry_timer(p_dsc_entry); 
				if(p_dsc_entry->state.path_need){
					start_path_discovery_timer(p_dsc_entry); 
					path_discovery_by_discovery_entry(netkey_offset, p_dsc_entry);
				}
				else{
					directed_discovery_entry_remove(p_dsc_entry);
				}
			}
			
			if(p_dsc_entry->state.request_delay_timer && clock_time_exceed(p_dsc_entry->state.request_delay_timer, ((PATH_REQUEST_DELAY_MS+(rand()%30))*1000))){
				stop_path_request_delay_timer(p_dsc_entry);
				path_discovery_by_discovery_entry(netkey_offset, p_dsc_entry);
			}

			if(p_dsc_entry->state.reply_delay_timer && clock_time_exceed(p_dsc_entry->state.reply_delay_timer, (is_unicast_adr(p_dsc_entry->entry.destination)?PATH_REPLY_DELAY_MS:(PATH_REPLY_DELAY_MS+(rand()%500)))*1000)){							
				stop_path_reply_delay_timer(p_dsc_entry);
				p_fwd_entry = get_corresponding_forwarding_non_fixed_entry(netkey_offset, p_dsc_entry->entry.path_origin.addr, p_dsc_entry->entry.destination);
				if(p_fwd_entry){// forwarding table entry exist, update
					update_forwarding_entry_by_path_reply_delay_timer_expired(netkey_offset, p_fwd_entry, p_dsc_entry);
				}
				else{
					p_fwd_entry = add_forwarding_entry_by_path_reply_delay_timer_expired(netkey_offset, &p_dsc_entry->entry);
					if(p_fwd_entry){
						start_path_lifetime_timer(p_fwd_entry);
					}
				}	
				prepare_and_send_path_reply(netkey_offset, p_dsc_entry);
			}
		}
	}
}

int directed_forwarding_dependents_update_start(u16 netkey_offset, u8 type, u16 path_enpoint, addr_range_big_endian_t dependent_range_addr)
{
	int err = -1;
	mesh_ctl_dependent_node_update_t dependent_node_update;
	memset(&dependent_node_update, 0x00, sizeof(dependent_node_update));
	dependent_node_update.type = type;
	dependent_node_update.path_endpoint = path_enpoint;
	dependent_node_update.dependent_addr = dependent_range_addr;
	forwarding_entry_par_t *p_fwd_entry = get_forwarding_entry_correspond2_dependent_node_update(netkey_offset, &dependent_node_update);
	if(p_fwd_entry){
		update_forwarding_by_dependent_node_update(netkey_offset, &dependent_node_update);
		err =  mesh_tx_cmd_layer_upper_ctl(CMD_CTL_DEPENDENT_NODE_UPDATE, (u8 *)&dependent_node_update, OFFSETOF(mesh_ctl_dependent_node_update_t, dependent_addr) + (dependent_range_addr.length_present?3:2), ele_adr_primary, ADR_ALL_DIRECTED_FORWARD, 0);
	}

	return err; 
}
#if 0
void set_path_origin_st(u16 netkey_offset, u16 destination, u8 st)
{
	foreach(i, MAX_FORWARDING_TBL_ENTRIES_CNT){
		if(destination == path_origin_state[netkey_offset].state[i].dst){
			LOG_MSG_LIB(TL_LOG_DIRECTED_FORWARDING, 0, 0, " set_path_origin_st netkey_offset:%d dst:0x%x st:%d", netkey_offset, destination, st);
			return 1;
		}
	}
}

int add_path_origin_st_entry(u16 netkey_offset, u16 destination, u8 st, u8 is_initial)
{
	foreach(i, MAX_FORWARDING_TBL_ENTRIES_CNT){
		if(0 == path_origin_state[netkey_offset].state[i].dst){
			path_origin_state[netkey_offset].state[i].dst = destination;
			path_origin_state[netkey_offset].state[i].st = st; 
			path_origin_state[netkey_offset].state[i].path_need = is_initial;
			LOG_MSG_LIB(TL_LOG_DIRECTED_FORWARDING, 0, 0, " add_path_origin_st_entry netkey_offset:%d dst:0x%x st:%d", netkey_offset, destination, st);
			return 1;					
		}
	}
	return 0;
}

int mesh_create_path_origin_state_machine(u16 netkey_index, u16 destination, addr_range_big_endian_t *p_dependent_origin, u8 is_initial)
{
	u8 netkey_offset = get_mesh_net_key_offset(netkey_index);
	if(-1 != directed_forwarding_initial_start(netkey_index, destination, p_dependent_origin)){
		add_path_origin_st_entry(netkey_offset, destination, DF_PATH_DSC, is_initial);
	}
}

void mesh_df_path_origin_state_proc()
{
	foreach(netkey_offset, NET_KEY_MAX){
		foreach(i, MAX_FORWARDING_TBL_ENTRIES_CNT){
			if(DF_IDLE != path_origin_state[netkey_offset].state[i].st){
				switch(path_origin_state[netkey_offset].state[i].st){
					case :
						break;
					default:
						break;
				}
			}
		}
	}
}
#endif

int directed_forwarding_initial_start(u16 netkey_offset, u16 destination, addr_range_big_endian_t *p_dependent_origin)
{
	int err = -1;
	if((ADR_UNASSIGNED==destination) || (ADR_ALL_NODES == destination) || is_own_ele(destination)){
		return err;
	}
	
	addr_range_big_endian_t path_origin;
	path_origin.range_start = ele_adr_primary;
	if(g_ele_cnt > 1){
		path_origin.length_present = 1;
		path_origin.range_length = g_ele_cnt;
	}
	else{
		path_origin.length_present = 0;
	}
	
	if(netkey_offset < NET_KEY_MAX){
		if(model_sig_cfg_s.directed_forward.subnet_state[netkey_offset].directed_control.directed_forwarding == DIRECTED_FORWARDING_ENABLE){
			//del_discovery_entry_correspond2_path_destination(netkey_offset, ele_adr_primary, destination);
			discovery_entry_par_t *p_dsc_entry = get_discovery_entry_correspond2_path_destination(netkey_offset, destination);
			if(p_dsc_entry && p_dsc_entry->state.discovery_retry_timer){
				p_dsc_entry->state.path_need = 1; // will trigle path discovery after retry timer expired
			}
			else if(direced_forwarding_concurrent_count_check(netkey_offset)){
				discovery_table[netkey_offset].forwarding_number++;
				discovery_entry_par_t * p_dsc_entry = add_discovery_table_entry(netkey_offset, &path_origin, destination, p_dependent_origin, ADR_UNASSIGNED, BEAR_UNASSIGNED, 0);
				if(p_dsc_entry){	
					err = 0; 
					p_dsc_entry->state.path_need = 1;
					path_discovery_by_discovery_entry(netkey_offset, p_dsc_entry);
				}
			}
		}
	}

	return err;
}

void mesh_directed_forwarding_proc(u8 *bear, int src_type)
{
	if(!bear){	// main loop
		foreach(netkey_offset, NET_KEY_MAX){
			forwarding_table_timing_proc(netkey_offset);	
			discovery_table_timing_proc(netkey_offset);
		}
	}
	else // receive packet
	{
		mesh_cmd_bear_unseg_t *p_bear = (mesh_cmd_bear_unseg_t *)bear;
		mesh_cmd_nw_t *p_nw = &p_bear->nw;
        mesh_cmd_lt_ctl_unseg_t *p_lt_ctl_unseg = &p_bear->lt_ctl_unseg;
        u8 op = p_lt_ctl_unseg->opcode;
		u8 netkey_offset = mesh_key.netkey_sel_dec;
		forwarding_entry_par_t *p_fwd_entry = 0;
		if(CMD_CTL_PATH_REQUEST == op){		
			mesh_ctl_path_req_t *p_path_req = (mesh_ctl_path_req_t *)(p_lt_ctl_unseg->data);
			addr_range_big_endian_t *p_addr_origin = (addr_range_big_endian_t *)p_path_req->addr_par;				
			adv_report_extend_t *p_extend = get_adv_report_extend(&p_bear->len);;
			s8 rssi = p_extend->rssi;  
			LOG_MSG_LIB(TL_LOG_DIRECTED_FORWARDING, (u8 *)p_path_req, sizeof(mesh_ctl_path_req_t), "receive CMD_CTL_PATH_REQUEST rssi:%d p_nw->src:0x%x dst:0x%x ", rssi, p_nw->src, p_path_req->destination);
			if(rssi > (model_sig_cfg_s.directed_forward.rssi_threshold.default_rssi_threshold + model_sig_cfg_s.directed_forward.rssi_threshold.rssi_margin)){
				if(p_path_req->path_metric_type == model_sig_cfg_s.directed_forward.subnet_state[netkey_offset].path_metric.metric_type){					
					p_fwd_entry = get_forwarding_entry_correspond2_path_request(netkey_offset, p_path_req);
					#if 0 
					if((!p_fwd_entry) || ((p_path_req->forwarding_number-p_fwd_entry->entry.forwarding_number)<128){ 
					#else
					if(1){// dorwarding number always 0 after powerup now.
					#endif
						u8 model_idx = 0;
						u8 *p_model = mesh_find_ele_resource_in_model(ele_adr_primary, SIG_MD_G_ONOFF_S, 1,&model_idx, 0);// search all model id later
						if( is_own_ele(p_path_req->destination) || (p_model&&is_subscription_adr((model_common_t *)p_model, p_path_req->destination)) || (mesh_mac_match_friend(p_path_req->destination) || mesh_group_match_friend(p_path_req->destination)) || (DIRECTED_RELAY_ENABLE == model_sig_cfg_s.directed_forward.subnet_state[netkey_offset].directed_control.directed_relay)){
							discovery_entry_par_t *p_dsc_entry = get_discovery_entry_correspond2_path_request(netkey_offset, p_addr_origin->range_start, p_path_req->forwarding_number);
						    u8 start_path_request = 0;
							if(!p_dsc_entry){
								p_dsc_entry = add_discovery_table_entry(netkey_offset, 0, 0, 0, p_nw->src, src_type, p_path_req);
								LOG_MSG_LIB(TL_LOG_DIRECTED_FORWARDING, 0, 0, " add_discovery_table_entry:%x path_metric=%d", p_dsc_entry, p_dsc_entry->entry.path_metric);
								if(p_dsc_entry){
									start_path_discovery_timer(p_dsc_entry);
									if(is_path_target(p_path_req->destination)){
										start_path_reply_delay_timer(p_dsc_entry);											
									}
									start_path_request = 1;										
								}
							}
							else{
								LOG_MSG_LIB(TL_LOG_DIRECTED_FORWARDING, 0, 0, " origin_path_metric:%d entry.path_metric:%d", p_path_req->origin_path_metric, p_dsc_entry->entry.path_metric);
								if(update_discovery_entry_by_path_request(&p_dsc_entry->entry, p_nw->src, src_type, p_path_req)){
									start_path_request = 1;
								}					
							}

							if(start_path_request && (!p_dsc_entry->state.request_delay_timer) && (DIRECTED_RELAY_ENABLE == model_sig_cfg_s.directed_forward.subnet_state[netkey_offset].directed_control.directed_relay) &&
								((!is_path_target(p_path_req->destination)) || is_group_adr(p_path_req->destination) || is_virtual_adr(p_path_req->destination))
							){
								start_path_request_delay_timer(p_dsc_entry);
							}							
						}
					}
				}
			}
			else{
				// discard
			}

		}
		else if(CMD_CTL_PATH_REPLY == op){	
			mesh_ctl_path_reply_t *p_path_reply = (mesh_ctl_path_reply_t *)(p_lt_ctl_unseg->data);
			LOG_MSG_LIB(TL_LOG_DIRECTED_FORWARDING, p_lt_ctl_unseg->data, sizeof(mesh_ctl_path_reply_t), "receive CMD_CTL_PATH_REPLY p_nw->src:0x%x origin:0x%x ", p_nw->src, p_path_reply->path_origin);
			discovery_entry_par_t *p_dsc_entry = get_discovery_entry_correspond2_path_reply(netkey_offset, p_path_reply);
			if(p_dsc_entry){
				p_fwd_entry = get_forwarding_entry_correspond2_path_reply(netkey_offset, p_path_reply);
				if(p_fwd_entry){
					update_forwarding_entry_by_path_reply(netkey_offset, src_type, p_fwd_entry, p_dsc_entry);
				}
				else{
					p_fwd_entry = add_forwarding_tbl_entry_by_rcv_path_reply(netkey_offset, src_type, p_dsc_entry, p_path_reply);				
					start_path_lifetime_timer(p_fwd_entry);
				}

				if(p_fwd_entry && p_dsc_entry->state.first_reply_msg){
					p_dsc_entry->state.new_lane_established = 1;
					p_dsc_entry->state.first_reply_msg = 0;
					if(is_own_ele(p_path_reply->path_origin)){// path origin
						if(1 == p_path_reply->confirmation_request){ 
							directed_forwarding_confirm_start(netkey_offset, p_fwd_entry);
						}

						if((!p_fwd_entry->state.path_echo_timer_s) && (model_sig_cfg_s.directed_forward.subnet_state[netkey_offset].path_echo_interval>0)){
							start_path_echo_timer(p_fwd_entry);
						}
					}
					else{
						start_path_reply_delay_timer(p_dsc_entry);
					}
				}
			}
		}
		else if(CMD_CTL_PATH_CONFIRMATION == op){
			LOG_MSG_LIB(TL_LOG_DIRECTED_FORWARDING, p_lt_ctl_unseg->data, sizeof(mesh_ctl_path_confirmation_t), "receive CMD_CTL_PATH_CONFIRMATION ", 0);
			mesh_ctl_path_confirmation_t *p_path_confirm = (mesh_ctl_path_confirmation_t *)p_lt_ctl_unseg->data;
			p_fwd_entry = get_forwarding_entry_correspond2_path_confirm(netkey_offset, p_path_confirm);
			if(p_fwd_entry){
				discovery_entry_par_t *p_dsc_entry = get_discovery_entry_correspond2_forwarding_entry(netkey_offset, &p_fwd_entry->entry);
				if(p_fwd_entry->entry.backward_path_validated){
					if(!is_path_target(p_path_confirm->path_target)){
						if(p_dsc_entry && !p_dsc_entry->state.path_confirm_sent){
							directed_forwarding_confirm_start(netkey_offset, p_fwd_entry);
						}
					}
				}
				else{
					update_forwarding_entry_by_path_confirm(netkey_offset, &p_fwd_entry->entry, p_path_confirm);
					if(!is_path_target(p_path_confirm->path_target)){
						directed_forwarding_confirm_start(netkey_offset, p_fwd_entry);
					}
				}
			}
		}
		else if(CMD_CTL_PATH_ECHO_REQUEST == op){
			LOG_MSG_LIB(TL_LOG_DIRECTED_FORWARDING, 0, 0, "receive CMD_CTL_PATH_ECHO_REQUEST ", 0);
			p_fwd_entry = get_forwarding_entry_correspond2_path_echo_request(netkey_offset, p_nw->src, p_nw->dst);
			if(p_fwd_entry){
				cfg_cmd_send_path_echo_reply(p_fwd_entry);
			}			
		}
		else if(CMD_CTL_PATH_ECHO_REPLY == op){
			LOG_MSG_LIB(TL_LOG_DIRECTED_FORWARDING, 0, 0, "receive CMD_CTL_PATH_ECHO_REPLY ", 0);
			mesh_ctl_path_echo_reply_t *p_path_echo_reply = (mesh_ctl_path_echo_reply_t *)p_lt_ctl_unseg->data;
			p_fwd_entry = get_forwarding_entry_correspond2_path_echo_reply(netkey_offset, p_nw->dst, p_path_echo_reply);
			if(p_fwd_entry){
				if(p_fwd_entry->state.path_echo_reply_timeout_timer){
					stop_path_echo_reply_timeout_timer(p_fwd_entry); // Path Validation Succeeded
					if(model_sig_cfg_s.directed_forward.subnet_state[netkey_offset].path_echo_interval){
						start_path_echo_timer(p_fwd_entry);
					}
				}
			}
		}
		else if(CMD_CTL_DEPENDENT_NODE_UPDATE == op){
			LOG_MSG_LIB(TL_LOG_DIRECTED_FORWARDING, p_lt_ctl_unseg->data, sizeof(mesh_ctl_dependent_node_update_t), "receive CMD_CTL_DEPENDENT_NODE_UPDATE ", 0);
			mesh_ctl_dependent_node_update_t *p_dependent_node_update = (mesh_ctl_dependent_node_update_t *)p_lt_ctl_unseg->data;
			p_fwd_entry = get_forwarding_entry_correspond2_dependent_node_update(netkey_offset, p_dependent_node_update);
			if(p_fwd_entry){
				if(update_forwarding_by_dependent_node_update(netkey_offset, p_dependent_node_update)){
					directed_forwarding_dependents_update_start(netkey_offset, p_dependent_node_update->type, p_dependent_node_update->path_endpoint, p_dependent_node_update->dependent_addr);
				}
			}
		}
		else if(CMD_CTL_PATH_REQUEST_SOLICITATION == op){
			LOG_MSG_LIB(TL_LOG_DIRECTED_FORWARDING, p_lt_ctl_unseg->data, sizeof(mesh_ctl_dependent_node_update_t), "receive CMD_CTL_PATH_REQUEST_SOLICITATION ", 0);
			mesh_ctl_path_request_solication_t *p_path_req_solicat = (mesh_ctl_path_request_solication_t *)p_lt_ctl_unseg->data;
			u8 list_num = (p_bear->len + OFFSETOF(mesh_cmd_bear_unseg_t, type) - OFFSETOF(mesh_cmd_bear_unseg_t, lt_ctl_unseg) - OFFSETOF(mesh_cmd_lt_ctl_unseg_t, data)) / 2;
			foreach(i, list_num){
				p_fwd_entry = get_corresponding_forwarding_non_fixed_entry(netkey_offset, ele_adr_primary, p_path_req_solicat->addr_list[i]);
				if(p_fwd_entry){
					// create path origin states machines
				}
			}
		}
	}
}
#endif
