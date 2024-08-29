/********************************************************************************************************
 * @file	pair_provision.c
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
//#include "tl_common.h"
#include "proj_lib/sig_mesh/app_mesh.h"
#include "pair_provision.h"

#if PAIR_PROVISION_ENABLE
void mesh_pro_rc_beacon_dispatch(pro_PB_ADV *p_adv,u8 *p_mac);

#define LOG_PAIR_PROV_DEBUG(pbuf, len, format, ...)    LOG_MSG_LIB(TL_LOG_NODE_BASIC, pbuf, len, format, ##__VA_ARGS__)
#define DEBUG_EXCLUDE_CACHE_ADDR_EN					(0)


static pair_prov_proc_t pair_prov_proc = {0};
static const char PAIR_PROC_ST_STRING[PAIR_PROV_PROC_ST_MAX][12] = {
	{"idle"},
	{"OnlineNodes"},
	{"scan"},
	{"distribute"},
	{"confirm"},
	{"close"}
};

STATIC_ASSERT(ELE_CNT == 1);	// because provisioner will always assign only one element address.
STATIC_ASSERT((PAIR_PROV_UNPROV_ADDRESS_START == 0x7000) || (PAIR_PROV_UNPROV_ADDRESS_START == 0x6000)
				|| (PAIR_PROV_UNPROV_ADDRESS_START == 0x4000)); // must be a value with continuous 1 for binary format.


void pair_proc_set_st(u8 st)
{
	pair_prov_proc.st = st;
	pair_prov_proc.tick = clock_time() | 1;
	LOG_PAIR_PROV_DEBUG(0, 0, "pair proc set state: %d (%s)", st, PAIR_PROC_ST_STRING[st]);
}

void pair_prov_set_scan_mac(u8 *mac)
{
	memcpy(pair_prov_proc.distribute_data.mac_scan, mac, sizeof(pair_prov_proc.distribute_data.mac_scan));
	LOG_PAIR_PROV_DEBUG(mac, 6, "pair proc scan mac: ");
}

// save all cache address before reset network
void pair_proc_save_all_cache_addr()
{
	pair_prov_proc_t *p_proc = &pair_prov_proc;
	int cnt = 0;
	foreach(i, g_cache_buf_max){
		if(cache_buf[i].src){
			p_proc->cache_addr[cnt++] = cache_buf[i].src;
			if(cnt >= ARRAY_SIZE(p_proc->cache_addr)){
				break;
			}
		}
	}

	p_proc->cache_cnt = cnt;

	#if 1 // DEBUG_EXCLUDE_CACHE_ADDR_EN
	LOG_PAIR_PROV_DEBUG((u8 *)p_proc->cache_addr, p_proc->cache_cnt * sizeof(p_proc->cache_addr[0]), "pair proc cache addr before reset network, count: %d, addr: ", p_proc->cache_cnt);
	#endif
}

int pair_proc_is_node_addr_exist_in_cache(u16 addr_src)
{
	pair_prov_proc_t *p_proc = &pair_prov_proc;
	if(addr_src){
	    foreach_arr(i, p_proc->cache_addr){
	        if(addr_src == p_proc->cache_addr[i]){
	        	return 1;
	        }
		}
	}
	
	return 0;
}

void pair_proc_add_new_addr2cache(u16 addr)
{
	pair_prov_proc_t *p_proc = &pair_prov_proc;
	if(pair_proc_is_node_addr_exist_in_cache(addr)){
		return ;
	}
	
	if(p_proc->cache_cnt < ARRAY_SIZE(p_proc->cache_addr)){
		p_proc->cache_addr[p_proc->cache_cnt++] = addr;
		#if DEBUG_EXCLUDE_CACHE_ADDR_EN
		LOG_PAIR_PROV_DEBUG(0, 0, "pair proc cache addr add: 0x%04x", addr);
		#endif
	}
}

u16 pair_prov_get_unicast_address()
{
	u16 address = 0;
	while(0 == address){ // -4: element count of a node may be up to 4.
		address = rand() & 0x7fff;
		#if DEBUG_EXCLUDE_CACHE_ADDR_EN
		static volatile u32 A_1_test_address = 0;
		if(A_1_test_address){
			address = A_1_test_address & 0x7fff;
			A_1_test_address = 0;
		}
		#endif

		if((address <= PAIR_PROV_FIRST_NODE_ADDR) || (address >= (PAIR_PROV_UNPROV_ADDRESS_START - 4))){
			address = 0;
			continue;
		}
		
		if(pair_proc_is_node_addr_exist_in_cache(address) || is_own_ele(address)){ // but cache has been cleared in mesh_reset_network_(), because iv may be different, so need to clear.
			LOG_PAIR_PROV_DEBUG(0, 0, "exist in cache: 0x%04x, so to regenerate unicase address.", address);
			address = 0; // to regenerate
			continue;
		}
	}

	return address;
}

static inline void pair_proc_set_unicast_addr(u16 addr)
{
	pair_prov_proc.distribute_data.net_info.unicast_address = addr;
}

void get_random_n(u8 *out, int len)
{
	u32 rand_val = 0;
	int index = 0;
	while(len > 0){
		int cnt = min(len, 4);
		rand_val = rand();
		sleep_us(20);
		memcpy(out + index, ((u8 *)&rand_val), cnt);
		index += cnt;
		len -= cnt;
	}
}

void pair_proc_create_new_network()
{
	LOG_PAIR_PROV_DEBUG(0, 0, "pair proc start");
	if(0 == is_provision_success()){
		LOG_PAIR_PROV_DEBUG(0, 0, "pair proc create new network");
		// ------- set net info -----------
		provison_net_info_str net_info = {0};
		get_random_n(net_info.net_work_key, sizeof(net_info.net_work_key));
		//net_info.key_index = 0;
		//net_info.flags = 0;
		net_info.iv_index[3] = 1; // init iv index is 0x00000001 (big endianness)
		net_info.unicast_address = PAIR_PROV_FIRST_NODE_ADDR;
	
		u8 app_key[16];
		get_random_n(app_key, sizeof(app_key));
		
		mesh_provision_and_bind_self((u8 *)&net_info, mesh_key.dev_key, 0, app_key);
	}
}

void pair_proc_start()
{
	pair_prov_proc_t *p_proc = &pair_prov_proc;
	if(PAIR_PROV_PROC_ST_IDLE == p_proc->st){
		if(0 == is_provision_success()){
			pair_proc_create_new_network();
		}
		
		access_cmd_onoff_get(ADR_ALL_NODES, 0);
		pair_proc_set_st(PAIR_PROV_PROC_ST_GET_ONLINE_NODES);
	}
}

void pair_proc_set_start_scan()
{
	pair_prov_proc_t *p_proc = &pair_prov_proc;
	if(PAIR_PROV_PROC_ST_GET_ONLINE_NODES == p_proc->st){
		memset(p_proc, 0, sizeof(pair_prov_proc));
		if(is_provision_success()){
			// ------- get net info -----------
			provison_net_info_str *p_info = &p_proc->distribute_data.net_info;
			u8 nk_array_idx = get_nk_arr_idx_first_valid();
			u8 ak_array_idx = get_ak_arr_idx_first_valid(nk_array_idx);
			mesh_net_key_t *p_key_current = &mesh_key.net_key[nk_array_idx][0];
			mesh_app_key_t *p_appkey_current = &p_key_current->app_key[ak_array_idx];
			
			memcpy(p_info->net_work_key, p_key_current->key, sizeof(p_info->net_work_key));
			p_info->key_index = p_key_current->index;
			get_iv_update_key_refresh_flag(&p_info->prov_flags, p_info->iv_index, KEY_REFRESH_NORMAL);
			pair_proc_set_unicast_addr(0); 		// set in distribute state // must set 0 here
			
			// ------- get dev key, app key -----------
			vd_cmd_pair_prov_distribute_t *p_dist_data = &p_proc->distribute_data;
			memcpy(p_dist_data->device_key, mesh_key.dev_key, sizeof(p_dist_data->device_key));	// use the same dev key, because would not save device key.
			memcpy(p_dist_data->app_key, p_appkey_current->key, sizeof(p_dist_data->app_key));
			p_dist_data->appkey_idx = p_appkey_current->index;
			//p_dist_data->mac_scan 			// set in distribute state
			
			p_proc->revert_network_flag = 1;
			pair_proc_save_all_cache_addr();
			mesh_reset_network(0); // reset to default network
			LOG_PAIR_PROV_DEBUG(0, 0, "pair proc reset to default network");
		}else{
			LOG_PAIR_PROV_DEBUG(0, 0, "pair proc start error");
			return ;
		}

		pair_proc_set_st(PAIR_PROV_PROC_ST_SCAN);
	}
}

const char PAIR_PROV_UUID_HEAD_FLAG[] = PAIR_PROV_UUID_FLAG;
STATIC_ASSERT(sizeof(PAIR_PROV_UUID_HEAD_FLAG) <= 6); // should not too much.

void pair_prov_rx_unprov_beacon_handle(beacon_data_pk *p_beacon, u8 *mac)
{
	pair_prov_proc_t *p_proc = &pair_prov_proc;
	if(PAIR_PROV_PROC_ST_SCAN == p_proc->st){
		if((MESH_ADV_TYPE_BEACON == p_beacon->header.type) && (UNPROVISION_BEACON == p_beacon->beacon_type)){
			char head_flag[] = PAIR_PROV_UUID_FLAG;
			if(0 == memcmp(PAIR_PROV_UUID_HEAD_FLAG, p_beacon->device_uuid, sizeof(head_flag))){
				if(0 != memcmp(p_proc->distribute_data.mac_scan, mac, sizeof(p_proc->distribute_data.mac_scan))){
					pair_proc_add_new_addr2cache(p_proc->distribute_data.net_info.unicast_address);
					pair_proc_set_unicast_addr(0);	// clear to regenerate unicast address later in PAIR_PROV_PROC_ST_DISTRIBUTE_
				}
				
				pair_prov_set_scan_mac(mac);
				LOG_PAIR_PROV_DEBUG((u8 *)p_beacon, p_beacon->header.len + 1, "rx unprov beacon:");
				pair_proc_set_st(PAIR_PROV_PROC_ST_DISTRIBUTE);
				p_proc->retry_cnt = PAIR_PROV_DISTRIBUTE_RETRY_CNT;
			}
		}
	}
}

static inline int pair_prov_is_mac_match(const u8 *mac1, const u8 *mac2)
{
	return (0 == memcmp(mac1, mac2, 6));
}

// ------- message received handling start -------------
int cb_vd_mesh_pair_prov_distribute(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	pair_prov_proc_t *p_proc = &pair_prov_proc;
	int err = 0;
	if(PAIR_PROV_PROC_ST_IDLE == p_proc->st){
		vd_cmd_pair_prov_distribute_t *p_distr = (vd_cmd_pair_prov_distribute_t *)par;
		if(pair_prov_is_mac_match(p_distr->mac_scan, tbl_mac) && (0 == is_provision_success())){
			err = vd_cmd_pair_prov_confirm(p_distr->net_info.unicast_address, cb_par->adr_src); // if set ele_adr_primary as source address, it can not be received by provisioner when they use the same element address.
			mesh_provision_and_bind_self((u8 *)&p_distr->net_info, p_distr->device_key, p_distr->appkey_idx, p_distr->app_key);
		}
	}else{
		// error state, should not happen here.
	}
	
	return err;
}

int cb_vd_mesh_pair_prov_confirm(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	pair_prov_proc_t *p_proc = &pair_prov_proc;
	int err = 0;
	if(PAIR_PROV_PROC_ST_CONFIRM == p_proc->st){
		vd_cmd_pair_prov_confirm_t *p_confirm = (vd_cmd_pair_prov_confirm_t *)par;
		if(pair_prov_is_mac_match(p_confirm->mac_self, p_proc->distribute_data.mac_scan)
		|| (cb_par->adr_src == p_proc->distribute_data.net_info.unicast_address)){
			p_proc->cnt_fail = 0;
			pair_proc_add_new_addr2cache(p_proc->distribute_data.net_info.unicast_address);
			pair_proc_set_st(PAIR_PROV_PROC_ST_SCAN);		// scan next node.
		}
	}
	
	return err;
}

int cb_vd_mesh_pair_prov_reset_all_nodes(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err = 0;
	node_reset_start(0); // delay reboot for self also.
	return err;
}
// ------- message received handling end ----------------


// ------- message TX(sending) handling start -----------
static inline void vd_cmd_pair_prov_set_additional_retransmit(bear_head_t * tx_head_out, u8 retransmit_cnt)
{
	tx_head_out->par_type = BEAR_TX_PAR_TYPE_REMAINING_TIMES;
	tx_head_out->val[0] = retransmit_cnt; // because no retry flow now (PAIR_PROV_DISTRIBUTE_RETRY_CNT == 0), so set to be a little more.
}

STATIC_ASSERT(EXTENDED_ADV_ENABLE == 1); // because we use a single packet to send key and address at the same time.
int vd_cmd_pair_prov_distribute_data(u16 assign_unicast_addr)
{
	pair_prov_proc_t *p_proc = &pair_prov_proc;
	pair_proc_set_unicast_addr(assign_unicast_addr);
	bear_head_t tx_head = {0};
	int count = PAIR_PROV_ADDITIONAL_RETRANSMIT_CNT;
	int cnt_less = 6;
	if(count > cnt_less){
		count -= cnt_less; // a little less should be better to avoid much crossing tx time.
	}
	
	vd_cmd_pair_prov_set_additional_retransmit(&tx_head, max2(count, 4));
	
	int err = mesh_tx_cmd2normal_with_tx_head(VD_PAIR_PROV_DISTRIBUTE_DATA, (u8 *)&p_proc->distribute_data, sizeof(p_proc->distribute_data), 
												ele_adr_primary, ADR_ALL_NODES, 0, &tx_head);
	return err;
}

int vd_cmd_pair_prov_confirm(u16 addr_src, u16 addr_dst)
{
	bear_head_t tx_head = {0};
	vd_cmd_pair_prov_set_additional_retransmit(&tx_head, PAIR_PROV_ADDITIONAL_RETRANSMIT_CNT);
	vd_cmd_pair_prov_confirm_t cmd_confirm = {{0}};
	memcpy(cmd_confirm.mac_self, tbl_mac, sizeof(cmd_confirm.mac_self));
	
	int err = mesh_tx_cmd2normal_with_tx_head(VD_PAIR_PROV_CONFIRM, (u8 *)&cmd_confirm, sizeof(cmd_confirm), 
												addr_src, addr_dst, 0, &tx_head);
	return err;
}

int vd_cmd_pair_prov_reset_all_nodes(u16 addr_dst)
{
	bear_head_t tx_head = {0};
	vd_cmd_pair_prov_set_additional_retransmit(&tx_head, PAIR_PROV_ADDITIONAL_RETRANSMIT_CNT);
	u8 par[1];
	int err = mesh_tx_cmd2normal_with_tx_head(VD_PAIR_PROV_RESET_ALL_NODES, par, 0, ele_adr_primary, addr_dst, 0, &tx_head);
	return err;
}

void pair_prov_kick_out_all_nodes()
{
	if(PAIR_PROV_PROC_ST_IDLE == pair_prov_proc.st){
		vd_cmd_pair_prov_reset_all_nodes(ADR_ALL_NODES);
	}
}
// ------- message TX(sending) handling end -------------


void pair_provision_proc()
{
	pair_prov_proc_t *p_proc = &pair_prov_proc;

	if(PAIR_PROV_PROC_ST_IDLE == p_proc->st){
		return ;
	}
	
	if(!is_mesh_adv_cmd_fifo_empty() || is_busy_segment_or_reliable_flow()){
		return ;
	}

	switch(p_proc->st){
		case PAIR_PROV_PROC_ST_GET_ONLINE_NODES:
			// waiting for address of online nodes to be added to cache buffer.
			if(clock_time_exceed(p_proc->tick, 2000*1000)){
				pair_proc_set_start_scan(); // pair_proc_set_st(PAIR_PROV_PROC_ST_SCAN) inside.
			}
			
			break;
			
		case PAIR_PROV_PROC_ST_SCAN:
			// waiting for unprovision beacon in pair_prov_rx_unprov_beacon_handle().
			#if (0 == DEBUG_EXCLUDE_CACHE_ADDR_EN)
			if(clock_time_exceed(p_proc->tick, PAIR_PROV_SCAN_TIMEOUT_MS*1000)){
				LOG_PAIR_PROV_DEBUG(0, 0, "pair proc scan timeout");
				pair_proc_set_st(PAIR_PROV_PROC_ST_CLOSE);
			}
			#endif
			
			break;
			
		case PAIR_PROV_PROC_ST_DISTRIBUTE:
		{
			u16 unicast_address = p_proc->distribute_data.net_info.unicast_address;
			if(0 == unicast_address){
				unicast_address = pair_prov_get_unicast_address();
			}else{
				// for retry
			}
			
			LOG_PAIR_PROV_DEBUG(0, 0, "pair proc distribute unicast addr: 0x%04x", unicast_address);
			vd_cmd_pair_prov_distribute_data(unicast_address);
			pair_proc_set_st(PAIR_PROV_PROC_ST_CONFIRM);
		}
			break;
			
		case PAIR_PROV_PROC_ST_CONFIRM:
			// waiting for confirm message from provisione.
			if(clock_time_exceed(p_proc->tick, 1000*1000)){
				#if PAIR_PROV_DISTRIBUTE_RETRY_CNT
				if(p_proc->retry_cnt){
					p_proc->retry_cnt--;
					LOG_PAIR_PROV_DEBUG(0, 0, "pair proc distribute retry remain times: %d", p_proc->retry_cnt);
					pair_proc_set_st(PAIR_PROV_PROC_ST_DISTRIBUTE); 	// retry
				}else
				#endif
				{
					if(++p_proc->cnt_fail >= 6){
						pair_proc_set_st(PAIR_PROV_PROC_ST_CLOSE); 		// terminate.
					}else{
						LOG_PAIR_PROV_DEBUG(0, 0, "pair proc distribute fail(or miss confirm message) times(xxxxxxxxxxxxxx): %d", p_proc->cnt_fail);
						pair_proc_set_st(PAIR_PROV_PROC_ST_SCAN); 		// scan next node.
					}
				}
			}
			
			break;
			
		case PAIR_PROV_PROC_ST_CLOSE:
			if(p_proc->revert_network_flag){
				LOG_PAIR_PROV_DEBUG(0, 0, "pair proc reboot to back to normal network: %d");
				start_reboot(); // TBD
			}
			
			break;
			
		default:
			break;
	}

	if(!is_led_busy()){
        rf_link_light_event_callback(PROV_START_LED_CMD);
    }
}
#endif
