/********************************************************************************************************
 * @file	smart_provision.c
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
#include "smart_provision.h"

#define SMART_SCAN_WHITE_LIST_NUM	32
#define SMART_SCAN_BLACK_LIST_NUM	16
#if GATEWAY_ENABLE
int my_fifo_push_hci_rx_fifo (u8 *p, u16 n, u8 *head, u8 head_len)
{
	my_fifo_t *f = &hci_rx_fifo;
	if (((f->wptr - f->rptr) & 255) >= f->num)
	{
		return -1;
	}

	if (n + (2+head_len) > f->size)    // sizeof(len) == 2
	{
		return -1;
	}
	u32 r = irq_disable();
	u8 *pd = f->p + (f->wptr & (f->num-1)) * f->size;
	*pd++ = (n+head_len) & 0xff;
	*pd++ = (n+head_len) >> 8;
	*pd++ = 0; // 
	*pd++ = 0;
	foreach(i,head_len){
		*pd++ = *head++;
	}
	memcpy (pd, p, n);
	f->wptr++;			// should be last for VC
	irq_restore(r);
	return 0;
}

int smart_gateway_provision_data_set()
{
	u8 head[] = GATEWAY_NET_KEY_HEAD;
	u8 netkey[] = SMART_NETKEY;
	provison_net_info_str net_info;
	memcpy(net_info.net_work_key, netkey, 16);
	net_info.key_index = SMART_NETKEY_INDEX;
	net_info.flags = 0;
	u8 iv[4] = SMART_IV_INDEX;
	memcpy(net_info.iv_index, iv, 4);
	net_info.unicast_address = GATEWAY_UNICAST_ADR;
	return my_fifo_push_hci_rx_fifo((u8 *)&net_info, sizeof(net_info), head, sizeof(head));
}

int smart_provision_gateway_devkey_set()
{
	u8 head[] = GATEWAY_DEV_KEY_HEAD;
	mesh_gw_set_devkey_str dev_key;
	dev_key.unicast = GATEWAY_UNICAST_ADR;
	memcpy(dev_key.dev_key, prov_para.device_uuid, 16);
	return my_fifo_push_hci_rx_fifo((u8 *)&dev_key, sizeof(dev_key), head, sizeof(head));
}

int smart_provision_appkey_add()
{
	u8 head[] = GATEWAY_APPKEY_ADD_HEAD;
	u8 appkey[] = SMART_APPKEY;
	mesh_netkey_set_t appkey_add;
	appkey_add.idx = SMART_APPKEY_INDEX;
	memcpy(appkey_add.key, appkey, 16);
	return my_fifo_push_hci_rx_fifo((u8 *)&appkey_add, sizeof(appkey_add), head, sizeof(head));
}

#if SMART_PROVISION_ENABLE
int smart_provision_st = SMART_CONFIG_IDLE;
static u32 smart_scan_tick = 0;

u8 smart_white_list_cnt = 0;
u8 smart_white_list[SMART_SCAN_WHITE_LIST_NUM][16]; // 16:uuid length
u8 smart_black_list_cnt = 0;
u8 smart_black_list[SMART_SCAN_BLACK_LIST_NUM][16];

int is_smart_provision_running()
{
	return (smart_provision_st != SMART_CONFIG_IDLE);
}

#if 0
int smart_provision_scan_stop()
{
	u8 stop[] = GATEWAY_SCAN_STOP;
	return my_fifo_push_hci_rx_fifo(scan, sizeof(scan), 0, 0);
}
#endif

void mesh_smart_provision_st_set(int st)
{
	smart_provision_st = st;
}

int mesh_smart_provision_st_get()
{
	return smart_provision_st;
}

void mesh_smart_scan_tick_refresh()
{
	smart_scan_tick = clock_time()|1;
}

void mesh_smart_provision_start()
{
	if(!is_smart_provision_running()){
		mesh_smart_provision_st_set(SMART_CONFIG_SCAN_START);
	}
}

void mesh_smart_provision_stop()
{
	mesh_smart_provision_st_set(SMART_CONFIG_IDLE);
}

int smart_provision_scan_start()
{
	u8 scan[] = GATEWAY_SCAN_START;
	return my_fifo_push_hci_rx_fifo(scan, sizeof(scan), 0, 0);
}

void smart_provision_data_get(provison_net_info_str *p)
{
	mesh_net_key_t *p_netkey = get_nk_primary();
	memcpy(p->net_work_key, p_netkey->key, 16);
	p->key_index = p_netkey->index;
	get_iv_update_key_refresh_flag(&p->prov_flags, p->iv_index, p_netkey->key_phase);
	get_iv_big_endian(p->iv_index, (u8 *)&iv_idx_st.iv_cur);
	p->unicast_address = provision_mag.unicast_adr_last;
}

int smart_provision_link_open_start()
{
	u8 head[] = GATEWAY_START_PROVISION_HEAD;
	provison_net_info_str net_info;
	smart_provision_data_get(&net_info);
	return my_fifo_push_hci_rx_fifo((u8 *)&net_info, sizeof(net_info), head, sizeof(head));
}

void smart_scan_list_init()
{
	smart_white_list_cnt = 0;
	smart_black_list_cnt = 0;
	memset(smart_white_list, 0x00, sizeof(smart_white_list));
	memset(smart_black_list, 0x00, sizeof(smart_black_list));
}

int smart_scan_whitelist_remove(u8 *p_uuid)
{
	int ret = 0;
	for(int cnt=0; cnt < smart_white_list_cnt; cnt++){
		if(0 == memcmp(smart_white_list[cnt], p_uuid, 16)){
			memset(smart_white_list[cnt], 0x00, sizeof(smart_white_list[cnt]));		
			smart_white_list_cnt--;
			for(int offset=cnt; offset< smart_white_list_cnt; offset++){
				memcpy(smart_white_list[offset], smart_white_list[offset+1], sizeof(smart_white_list[offset]));
			}
			memset(smart_white_list[smart_white_list_cnt], 0x00, sizeof(smart_white_list[smart_white_list_cnt]));

			ret = -1;
			break;
		}
	}
	return 0;
}

int is_existed_in_smart_scan_list(u8 *p_uuid, int is_whitelist)
{
	int is_exist = 0;
	u8 *p_list = (u8 *)(is_whitelist?smart_white_list:smart_black_list);
	foreach(i, (is_whitelist?smart_white_list_cnt:smart_black_list_cnt)){
		if(0 == memcmp(p_list+i*16, p_uuid, 16)){
			is_exist = 1;
			break;
		}
	}
	return is_exist;
}

int smart_provision_blacklist_add(u8 *p_uuid)
{
	if(!is_existed_in_smart_scan_list(p_uuid, 0)){ // not in blacklist
		if(smart_black_list_cnt < SMART_SCAN_BLACK_LIST_NUM){
			memcpy(smart_black_list[smart_black_list_cnt++], p_uuid, 16);			
			return 1;
		}
	}
	
	return 0;
}

int smart_provision_unprovision_node_add(u8 *p_uuid)
{
	if(!is_existed_in_smart_scan_list(p_uuid, 0)){ // not in blacklist
		if(!is_existed_in_smart_scan_list(p_uuid, 1)){ // not in whitelist
			if(smart_white_list_cnt < SMART_SCAN_WHITE_LIST_NUM){
				memcpy(smart_white_list[smart_white_list_cnt++], p_uuid, 16);			
				return 1;
			}
		}
	}
	
	return 0;
}

int mesh_smart_provision_rsp_handle(u8 code, u8 *p, int len)
{
	if(is_smart_provision_running()){
		switch(code){
			case HCI_GATEWAY_CMD_UPDATE_MAC:{				
					unprov_pkt_t *p_beacon = (unprov_pkt_t *)p;
					if(prov_uuid_fastbind_mode(p_beacon->beacon.device_uuid))
					{
						smart_provision_unprovision_node_add(p_beacon->beacon.device_uuid);
					}
				}
				break;
			case HCI_GATEWAY_CMD_LINK_OPEN:
				mesh_smart_scan_tick_refresh();
				break;
			case HCI_GATEWAY_CMD_LINK_CLS:{
					bear_close_link *p_close = (bear_close_link *)p;
					smart_scan_whitelist_remove(prov_link_uuid);
					if(LINK_CLOSE_SUCC == p_close->reason){					
						mesh_smart_provision_st_set(SMART_CONFIG_BIND_START);
					}
					else{
						smart_provision_blacklist_add(prov_link_uuid);
						mesh_smart_provision_st_set(SMART_CONFIG_SCAN_START);
					}
				}
				break;				
			case HCI_GATEWAY_CMD_PROVISION_EVT:
				smart_scan_whitelist_remove(prov_link_uuid);
				mesh_smart_provision_st_set(SMART_CONFIG_BIND_START);
				break;
			case HCI_GATEWAY_CMD_KEY_BIND_EVT:
				if(MESH_KEYBIND_EVE_TIMEOUT == p[0]){
					cfg_cmd_reset_node(key_refresh_cfgcl_proc.node_adr);
				}			
				mesh_smart_provision_st_set(SMART_CONFIG_SCAN_START);
				break;
			default:
				break;
		}
	}
	return 0;
}

void mesh_smart_provision_proc()
{
	switch(smart_provision_st){
		case SMART_CONFIG_SCAN_START:
			mesh_smart_scan_tick_refresh();
			if(!is_provision_success()){ // should set gateway's network info first
				smart_gateway_provision_data_set();
				smart_provision_gateway_devkey_set();
			}
			smart_provision_scan_start();
			mesh_smart_provision_st_set(SMART_CONFIG_SCANNING);
			break;
		case SMART_CONFIG_SCANNING:		
			if(smart_white_list_cnt){
				mesh_smart_provision_st_set(SMART_CONFIG_PROV_START);
			}
			break;
		case SMART_CONFIG_PROV_START:
			smart_provision_link_open_start();
			mesh_smart_provision_st_set(SMART_CONFIG_PROVING);
			break;
		case SMART_CONFIG_PROVING:			
			break;
		case SMART_CONFIG_BIND_START:
			smart_provision_appkey_add();
			mesh_smart_provision_st_set(SMART_CONFIG_BINDING);
			break;
		default:
			break;
	}
	
	if(smart_scan_tick && clock_time_exceed(smart_scan_tick, SMART_SCAN_TIMEOUT_MS*1000)){
		mesh_smart_provision_stop();
		smart_scan_list_init();
		smart_scan_tick = 0;
	}

	if(is_smart_provision_running() && !is_led_busy()){
        rf_link_light_event_callback(PROV_START_LED_CMD);
    }
}
#endif
#endif
