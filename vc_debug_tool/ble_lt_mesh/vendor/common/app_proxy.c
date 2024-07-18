/********************************************************************************************************
 * @file	app_proxy.c
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
#include "tl_common.h"
#include "proj_lib/ble/blt_config.h"
#include "vendor/common/app_provison.h"
#include "app_beacon.h"
#include "app_proxy.h"
#include "proj_lib/sig_mesh/app_mesh.h"
#include "app_privacy_beacon.h"
#include "directed_forwarding.h"
#include "subnet_bridge.h"
#if DU_ENABLE
#include "user_du.h"
#endif

u16 app_adr[ACL_PERIPHR_MAX_NUM];
proxy_config_mag_str proxy_mag[ACL_PERIPHR_MAX_NUM];
mesh_proxy_protocol_sar_t  proxy_sar[ACL_PERIPHR_MAX_NUM];
u8 proxy_filter_initial_mode = FILTER_WHITE_LIST;
u8 proxy_para_buf[ACL_PERIPHR_MAX_NUM][PROXY_GATT_MAX_LEN]; 

//------------------------------proxy reassembly-----------------------------
u8 *mesh_get_proxy_sar_buf(u8 idx)
{
	return (u8 *)proxy_para_buf[idx];
}

// add the sar part logical 
void mesh_proxy_sar_para_init(u8 idx)
{
	memset(&proxy_sar[idx], 0x00, sizeof(proxy_sar[idx]));
}

void mesh_proxy_sar_start(u8 idx)
{	
	if(0 == proxy_sar[idx].sar_tick){
		proxy_sar[idx].sar_tick = clock_time()|1;
	}else{
		#if DEBUG_PROXY_SAR_ENABLE
		static u32 A_debug_start_err = 0;
		A_debug_start_err++;
		LOG_MSG_INFO(TL_LOG_PROXY,0, 0 ,"mesh_proxy_sar_start:debug printf sar start err");
		#endif 
		proxy_sar[idx].err_flag = 1;
	}	
}

void mesh_proxy_sar_continue(u8 idx)
{
	if(proxy_sar[idx].sar_tick){
		proxy_sar[idx].sar_tick = clock_time()|1;
	}else{
		#if DEBUG_PROXY_SAR_ENABLE
		static u32 A_debug_start_continu = 0;
		A_debug_start_continu++;
		LOG_MSG_INFO(TL_LOG_PROXY,0, 0 ,"mesh_proxy_sar_continue:debug printf sar continue err");
		#endif 
		proxy_sar[idx].err_flag = 1;
	}	
}

void mesh_proxy_sar_end(u8 idx)
{
	if(proxy_sar[idx].sar_tick){
		proxy_sar[idx].sar_tick = 0;
	}else{
		#if DEBUG_PROXY_SAR_ENABLE
		static u32 A_debug_start_end = 0;
		A_debug_start_end++;
		LOG_MSG_INFO(TL_LOG_PROXY,0, 0 ,"mesh_proxy_sar_end:debug printf sar end err");
		#endif 
		proxy_sar[idx].err_flag = 1;
	}	
}

void mesh_proxy_sar_complete(u8 idx)
{
	// reset the status 
	if(0 == proxy_sar[idx].sar_tick){
		//proxy_sar.sar_tick = 0;
	}else{
	#if DEBUG_PROXY_SAR_ENABLE
		static u32 A_debug_start_complete = 0;
		A_debug_start_complete++;
		LOG_MSG_INFO(TL_LOG_PROXY,0, 0 ,"mesh_proxy_sar_complete:debug printf sar complete err");
	#endif 
		proxy_sar[idx].err_flag = 1;
	}
}

void mesh_proxy_sar_err_terminate(u8 idx)
{
	if(proxy_sar[idx].err_flag){
	    LOG_MSG_ERR(TL_LOG_PROXY,0, 0 ,"TL_LOG_PROXY:sar complete err");
		//reset para part 
		mesh_proxy_sar_para_init(idx);
		// send terminate ind cmd to the master part 
		#if WIN32 
		// the upper tester should reliaze the function 
		//mesh_proxy_master_terminate_cmd(); 
		#else
		#if DEBUG_PROXY_SAR_ENABLE
		irq_disable();
		while(1);
		#endif 
		//bls_ll_terminateConnection(0x13); 
		#endif 
	}	
}

void mesh_proxy_sar_timeout_terminate()
{
	foreach(idx, ACL_PERIPHR_MAX_NUM){
		if(proxy_sar[idx].sar_tick&&clock_time_exceed(proxy_sar[idx].sar_tick,PROXY_PDU_TIMEOUT_TICK)){
			mesh_proxy_sar_para_init(idx);
			LOG_MSG_ERR(TL_LOG_PROXY,0, 0 ,"TL_LOG_PROXY:sar timeout terminate");
			#if WIN32 
			mesh_proxy_master_terminate_cmd();
			#else
				#if BLE_MULTIPLE_CONNECTION_ENABLE
			blc_ll_disconnect(proxy_sar[idx].conn_handle, HCI_ERR_REMOTE_USER_TERM_CONN);
				#else
			bls_ll_terminateConnection(0x13);
				#endif
			#endif
		}
	}
}

/**
 * @brief       This function server to reassemble Proxy PDU.
 * @param[in]   p			- pointer of rf packet.
 * @param[in]   l2cap_type	- proxy message type
 * @param[in]   p_rcv		- buffer for reassemble Proxy PDU.
 * @param[io]   p_rcv_len	- last reassembled length, the offset of buffer to cache current packet.  
 * @return      0: failure. 1: success.
 * @note        
 */
int pkt_pb_gatt_data(int idx, rf_packet_att_data_t *p, u8 l2cap_type)
{
	int ret = 0;
	u8 *p_rcv = 0;

	//package the data str 
	mesh_cmd_bear_t *p_bear = (mesh_cmd_bear_t *)mesh_get_proxy_sar_buf(idx);
	pb_gatt_proxy_str *p_gatt = (pb_gatt_proxy_str *)(p->dat);
	if((l2cap_type == L2CAP_PROVISON_TYPE) && (p_gatt->msgType == MSG_PROVISION_PDU)){
		p_rcv = (u8 *)p_bear;
	}else if(((l2cap_type == L2CAP_PROXY_TYPE) && (p_gatt->msgType == MSG_PROXY_CONFIG)) || ((l2cap_type == L2CAP_NETWORK_TYPE) && (p_gatt->msgType == MSG_NETWORK_PDU))){
		p_rcv = (u8 *)&p_bear->nw;
	}else if((l2cap_type == L2CAP_BEACON_TYPE) && (p_gatt->msgType == MSG_MESH_BEACON)){
		p_rcv = (u8 *)&p_bear->beacon;
	}

	if(0 == p_rcv){
		return ret;
	}

	#if DEBUG_PROXY_SAR_ENABLE
	static u8 A_proxy_sar_debug[256];
	static u8 proxy_idx =0;
	A_proxy_sar_debug[proxy_idx++]= p_gatt->sar;
	if(proxy_idx ==255){
		memset(A_proxy_sar_debug,0,sizeof(A_proxy_sar_debug));
	}
	#endif 
	#if DEBUG_PROXY_SAR_ENABLE
	/*
	static u8 A_debug_record_last_lost[32][16];
	static u8 A_debug_record_last_idx =0;
	memcpy((u8*)A_debug_record_last_lost+A_debug_record_last_idx*16,p_gatt->data,(p->l2cap-4)>16?16:4);
	A_debug_record_last_idx++;
	if(A_debug_record_last_idx == 16){
		A_debug_record_last_idx = 0;
	}
	*/
	#endif 

    u16 len_payload = p->l2cap-4;
	
	if(p_gatt->sar == SAR_START){
		proxy_sar[idx].sar_len = 0;
		mesh_proxy_sar_start(idx);
	}else if(p_gatt->sar == SAR_CONTINUE){
		mesh_proxy_sar_continue(idx);
	}else if(p_gatt->sar == SAR_END){
		mesh_proxy_sar_end(idx);
		ret = 1;
	}else{
		proxy_sar[idx].sar_len = 0;
		mesh_proxy_sar_complete(idx);
		ret = 1;
	} 

	if((proxy_sar[idx].sar_len + len_payload + OFFSETOF(mesh_cmd_bear_t, nw)) > PROXY_GATT_MAX_LEN){    // over folw.
	    static u8 para_pro_overflow_cnt;para_pro_overflow_cnt++;
		mesh_proxy_sar_para_init(idx);
	
	    return 0;
	}
	
	memcpy(p_rcv + proxy_sar[idx].sar_len, p_gatt->data, len_payload);
	proxy_sar[idx].sar_len += len_payload;
	#if DEBUG_PROXY_SAR_ENABLE
	if(proxy_sar[idx].err_flag){
		static u8 A_debug_record_last_lost1[32];
		memcpy(A_debug_record_last_lost1,p_gatt->data,len_payload);
		LOG_MSG_ERR(TL_LOG_PROXY,p_gatt->data, len_payload ,"TL_LOG_PROXY:sar complete err");
	}
	#endif 
	
	mesh_proxy_sar_err_terminate(idx);
	
	return ret;
	// after package the data and the para 
}

//------------------------------att callback function-----------------------------
#if !ATT_REPLACE_PROXY_SERVICE_EN
u8 proxy_Out_ccc[2]={0x00,0x00};
u8 proxy_In_ccc[2]=	{0x01,0x00};
#endif

u8  provision_In_ccc[2]={0x01,0x00};// set it can work enable 
u8  provision_Out_ccc[2]={0x00,0x00}; 

void reset_all_ccc()
{
	// wait for the whole dispatch 	
	#if BLE_MULTIPLE_CONNECTION_ENABLE
	if(0 == blc_ll_getCurrentSlaveRoleNumber())
	#endif
	{
		memset(provision_Out_ccc,0,sizeof(provision_Out_ccc));
		#ifndef WIN32 
		memset(proxy_Out_ccc,0,sizeof(proxy_Out_ccc));
		#endif 
	}
	return ;
}

#if BLE_MULTIPLE_CONNECTION_ENABLE
int pb_gatt_provision_out_ccc_cb(u16 connHandle, void *p)
#else
int pb_gatt_provision_out_ccc_cb(void *p)
#endif
{
	rf_packet_att_data_t *pw = (rf_packet_att_data_t *)p;
	provision_Out_ccc[0] = pw->dat[0];
	provision_Out_ccc[1] = pw->dat[1];
	return 1;	
}

/**
 * @brief       This function server to process Proxy PDU from provision service.
 * @param[in]   p	- pointer of rf packet.
 * @return      unused.
 * @note        
 */
#if BLE_MULTIPLE_CONNECTION_ENABLE
int	pb_gatt_Write(u16 connHandle, void *p)
#else
int	pb_gatt_Write (void *p)
#endif
{
	int idx = 0;
	#if BLE_MULTIPLE_CONNECTION_ENABLE
	idx = get_slave_idx_by_conn_handle(connHandle);
	#else
	u16 connHandle = BLS_HANDLE_MIN;
	#endif
	
	if(provision_In_ccc[0]==0 && provision_In_ccc[1]==0){
		return 0;
	}
	
	#if ATT_REPLACE_PROXY_SERVICE_EN
	extern int proxy_gatt_Write(void *p);
	u8 service_uuid[] = SIG_MESH_PROXY_SERVICE;
	if(0 == memcmp(my_pb_gattUUID, service_uuid, sizeof(my_pb_gattUUID) )){
		return proxy_gatt_Write(p);
	}
	#endif
	
	#if FEATURE_PROV_EN
	rf_packet_att_data_t *pw = (rf_packet_att_data_t *)p;
	// package the data 
	if(!pkt_pb_gatt_data(idx, pw, L2CAP_PROVISON_TYPE)){
		return 0;
	}
	
	dispatch_pb_gatt(connHandle, mesh_get_proxy_sar_buf(idx), proxy_sar[idx].sar_len);
	#endif 
	
	return 1;
}

#if BLE_MULTIPLE_CONNECTION_ENABLE
int proxy_out_ccc_cb(u16 connHandle, void *p)
#else
int proxy_out_ccc_cb(void *p)
#endif
{
	rf_packet_att_data_t *pw = (rf_packet_att_data_t *)p;
	proxy_Out_ccc[0] = pw->dat[0];
	proxy_Out_ccc[1] = pw->dat[1];
	beacon_send.conn_beacon_flag =1;
	beacon_send.tick = clock_time();

	if (proxy_Out_ccc[0]==1 && proxy_Out_ccc[1]==0){
		if(is_provision_success()){
			mesh_tx_sec_private_beacon_proc(1);// send conn beacon to the provisioner		 
			beacon_send.conn_beacon_flag =0;						
		}
					
		#if (MD_DF_CFG_SERVER_EN && !WIN32)
			#if !BLE_MULTIPLE_CONNECTION_ENABLE
		u16 connHandle = BLS_HANDLE_MIN;
			#endif
		mesh_directed_proxy_capa_report_upon_connection(connHandle); // report after security network beacon.
		#endif
	}
	
	return 1;	
}

/**
 * @brief       This function server to process Proxy PDU of proxy service from GATT master, such as cell phone.
 * @param[in]   p	- pointer of rf packet.
 * @return      unused.
 * @note        
 */
#if BLE_MULTIPLE_CONNECTION_ENABLE
int proxy_gatt_Write(u16 connHandle, void *p)
#else
int proxy_gatt_Write(void *p)
#endif
{
	int idx = 0;
	#if BLE_MULTIPLE_CONNECTION_ENABLE
	idx = get_slave_idx_by_conn_handle(connHandle);
	#endif
	
	if(proxy_In_ccc[0]==0 && proxy_In_ccc[1]==0){
		return 0;
	}
	
	#if FEATURE_PROXY_EN
	rf_packet_att_data_t *pw = (rf_packet_att_data_t *)p;
	pb_gatt_proxy_str *p_gatt = (pb_gatt_proxy_str *)(pw->dat);
	mesh_cmd_bear_t *p_bear = (mesh_cmd_bear_t *)mesh_get_proxy_sar_buf(idx);
	
	#if BLE_MULTIPLE_CONNECTION_ENABLE
	bear_conn_handle_t *p_bear_handle = (bear_conn_handle_t *)&p_bear->tx_head;
	p_bear_handle->par_type = BEAR_TX_PAR_TYPE_CONN_HANDLE;
	p_bear_handle->conn_handle = connHandle;
	#endif
	
	if(p_gatt->msgType == MSG_PROXY_CONFIG ){
		if(!pkt_pb_gatt_data(idx, pw, L2CAP_PROXY_TYPE)){
			return 0;
		}
		p_bear->trans_par_val = TRANSMIT_DEF_PAR;
		p_bear->len = proxy_sar[idx].sar_len + 1;
		p_bear->type = MESH_ADV_TYPE_MESSAGE;
		// different dispatch 
		//send the data by the SIG MESH layer 
		if(0 == mesh_rc_data_cfg_gatt((u8 *)p_bear)){
		    proxy_config_dispatch(p_bear, proxy_sar[idx].sar_len);
		}
	}else if (p_gatt->msgType == MSG_MESH_BEACON){
		if(!pkt_pb_gatt_data(idx, pw, L2CAP_BEACON_TYPE)){
			return 0;
		}
		if(SECURE_BEACON == p_bear->beacon.type){
			p_bear->len =23;
			//mesh_bear_tx2mesh((u8 *)p_bear, TRANSMIT_DEF_PAR_BEACON);
			int err = mesh_rc_data_beacon_sec(&p_bear->len, 0);		
			if(err != 100){
                LOG_MSG_LIB(TL_LOG_IV_UPDATE,&p_bear->len, p_bear->len+1,"RX secure GATT beacon,nk arr idx:%d, new:%d, pkt:",mesh_key.netkey_sel_dec,mesh_key.new_netkey_dec);
			}
		}else if (PRIVACY_BEACON == p_bear->beacon.type){
			// no handle for other beacon now
			#if MD_PRIVACY_BEA
			p_bear->len =28;
			int err = mesh_rc_data_beacon_privacy(&p_bear->len, 0);		
			if(err != 100){
                LOG_MSG_INFO(TL_LOG_IV_UPDATE,&p_bear->len, p_bear->len+1,"RX prrivacy GATT beacon,nk arr idx:%d, new:%d, pkt:",mesh_key.netkey_sel_dec,mesh_key.new_netkey_dec);
			}
			#endif
		}else{
			// no handle for other beacon now
		}
	}else if(p_gatt->msgType == MSG_NETWORK_PDU){
		if(!pkt_pb_gatt_data(idx, pw, L2CAP_NETWORK_TYPE)){
			return 0;
		}
		// and then how to use the data ,make a demo to turn on or turn off the light 
		p_bear->trans_par_val = TRANSMIT_DEF_PAR;
		p_bear->len = proxy_sar[idx].sar_len + 1;
		p_bear->type = MESH_ADV_TYPE_MESSAGE;
		mesh_nw_pdu_from_gatt_handle((u8 *)p_bear);
	}
#if MESH_RX_TEST
	else if((p_gatt->msgType == MSG_RX_TEST_PDU)&&(p_gatt->data[0] == (u8)HCI_CMD_BULK_CMD2DEBUG) && (p_gatt->data[1] == (HCI_CMD_BULK_CMD2DEBUG >> 8))){
		u16 adr_dst = p_gatt->data[2] + (p_gatt->data[3] << 8);
		u8 ack = p_gatt->data[5];
		cmd_ctl_ttc_t ctl_ttc;
		memset(&ctl_ttc, 0x00, sizeof(cmd_ctl_ttc_t));
		ctl_ttc.sno_cmd = p_gatt->data[6] + (p_gatt->data[7] << 8);
		ctl_ttc.onoff = ctl_ttc.sno_cmd & 0x01;
		
		mesh_tx_cmd_layer_upper_ctl_primary(ack ? CMD_CTL_TTC_CMD : CMD_CTL_TTC_CMD_NACK, (u8 *)&ctl_ttc, sizeof(ctl_ttc), adr_dst);		
	}
#endif
	else{
	}
#endif
	return 0;
}
//-----------------------------end att cb-----------------------------

void set_proxy_initial_mode(u8 special_mode)
{
	if(special_mode){
		proxy_filter_initial_mode =FILTER_BLACK_LIST;
	}else{
		proxy_filter_initial_mode =FILTER_WHITE_LIST;
	}

	return;
}
	

void proxy_cfg_list_init_upon_connection(u16 conn_handle)
{
	int idx = 0;
	#if BLE_MULTIPLE_CONNECTION_ENABLE
	idx = get_slave_idx_by_conn_handle(conn_handle);
	#endif
	
	memset(&proxy_mag[idx], 0x00, sizeof(proxy_mag[idx]));
	proxy_mag[idx].filter_type = proxy_filter_initial_mode;
	#if DU_ENABLE
	proxy_proc_filter_mesh_cmd(idx, VD_DU_GROUP_DST);
	#endif
	prov_para.privacy_para = mesh_get_proxy_privacy_para();
	
	return ;
}

/**
 * @brief       This function server to check whether the address is gatt master's unicast address or not.
 * @param[in]   addr- unicast address
 * @return      0: addr is not master's address. other: addr is master's address.
 * @note        
 */
int is_app_addr(u16 addr)
{
	foreach(i, ACL_PERIPHR_MAX_NUM){
		if(addr == app_adr[i]){
			return 1;
		}
	}

	return 0;
}

int find_data_in_list(int idx, u16 adr)
{
	int ret = -1;
	foreach(i,MAX_LIST_LEN){
		if(adr == proxy_mag[idx].addr_list[i]){
			ret = i;
			break;
		}
	}
	
	return ret;
}

int is_addr_in_proxy_list(int idx, u16 addr)
{
	return (-1 != find_data_in_list(idx, addr));
}

int is_valid_addr_in_proxy_list(int idx, u16 addr)
{
	return (FILTER_WHITE_LIST == proxy_mag[idx].filter_type) ? is_addr_in_proxy_list(idx, addr) : !is_addr_in_proxy_list(idx, addr);
}

/**
 * @brief       This function server to check whether destination of network PDU is in proxy whitelist, the network PDU will report to corresponding connection handle in mesh_nw_pdu_report_to_gatt().
 * @param[in]   dst_addr- destination of the network PDU.
 * @return      1: address is in whitelist. 0: address isn't in whitelist.
 * @note        
 */
int is_valid_adv_with_proxy_filter(u16 dst_addr)
{
	int valid = 1;
	if(dst_addr == PROXY_CONFIG_FILTER_DST_ADR){
	}else{
		foreach(idx, ACL_PERIPHR_MAX_NUM){
			valid = is_valid_addr_in_proxy_list(idx, dst_addr);
			if(valid){
				break;
			}
		}
		
		if(!valid){
			static u16 filter_error_cnt;	filter_error_cnt++;
		}
	}

	return valid;
}

#if MD_DF_CFG_SERVER_EN
int is_proxy_client_addr(u16 addr)
{
	foreach(idx, ACL_PERIPHR_MAX_NUM){
		if((DIRECTED_PROXY_CLIENT == proxy_mag[idx].proxy_client_type) || (PROXY_CLIENT == proxy_mag[idx].proxy_client_type)){
			if(DIRECTED_PROXY_CLIENT == proxy_mag[idx].proxy_client_type){
				foreach(netkey_offset, NET_KEY_MAX){			
					if(addr == proxy_mag[idx].directed_server[netkey_offset].client_addr){
						return 1;
					}
				}
			}
				
			if(FILTER_WHITE_LIST == proxy_mag[idx].filter_type){
				foreach(i, MAX_LIST_LEN){
					if(addr == proxy_mag[idx].addr_list[i]){
						return 1;
					}
				}
			}
		}	
	}

	return 0;
}

/**
 * @brief       This function server to get addr's corresponding acl index and netkey offset
 * @param[in]   addr			- unicast address
 * @param[out]  netkey_offset	- network offset
 * @return      acl index
 * @note        
 */
int get_directed_proxy_client_idx(u16 addr, u8 *netkey_offset)
{
	foreach(acl_idx, ACL_PERIPHR_MAX_NUM){
		if(DIRECTED_PROXY_CLIENT == proxy_mag[acl_idx].proxy_client_type){
			for(u8 key_idx = 0; key_idx < NET_KEY_MAX; key_idx++){			
				if(addr == proxy_mag[acl_idx].directed_server[key_idx].client_addr){
					*netkey_offset = key_idx;
					return acl_idx;
				}
			}
		}
	}

	return -1;
}
#endif

int get_list_cnt(int idx)
{
	int cnt=0;
	foreach(i,MAX_LIST_LEN){
		if(0 == proxy_mag[idx].addr_list[i]){
			break;
		}
		cnt++;
	}
	return cnt;
}

int add_data_to_list(int idx, u16 addr)
{
    if(is_addr_in_proxy_list(idx, addr)){
        return 0;
    }

	foreach(i, MAX_LIST_LEN){
		if(0 == proxy_mag[idx].addr_list[i]){
			proxy_mag[idx].addr_list[i] = addr;			
			return 0;
		}
	}
	
	return -1;
}

int delete_data_from_list(int idx, u16 src)
{
	int offset = find_data_in_list(idx, src);
	if(-1 == offset){
        return -1;
	}

	proxy_mag[idx].addr_list[offset] = 0;
	memcpy(&proxy_mag[idx].addr_list[offset], &proxy_mag[idx].addr_list[offset+1], 2*(MAX_LIST_LEN-(offset+1)));
	return 0;
}

int send_filter_sts(int idx, mesh_cmd_nw_t *p_nw)
{
	int err = -1;
	mesh_filter_sts_str mesh_filter_sts;
	memset(&mesh_filter_sts,0,sizeof(mesh_filter_sts_str));
	u8 filter_sts = PROXY_FILTER_STATUS; 
	mesh_filter_sts.fil_type = proxy_mag[idx].filter_type;
	mesh_filter_sts.list_size = get_list_cnt(idx);
	// swap the list size part 
	endianness_swap_u16((u8 *)(&mesh_filter_sts.list_size));

	u16 conn_handle = BLS_HANDLE_MIN;
	#if BLE_MULTIPLE_CONNECTION_ENABLE
	conn_handle = get_slave_conn_handle_by_idx(idx);
	#endif
	
#if 1
	err = mesh_tx_cmd_layer_proxy_cfg_primary(conn_handle, filter_sts, (u8 *)(&mesh_filter_sts), sizeof(mesh_filter_sts), PROXY_CONFIG_FILTER_DST_ADR);
#else
#endif 

#if 0 // (!MESH_BLE_NOTIFY_FIFO_EN)
	// push into notify fifo directly
	if(err){
		LOG_MSG_ERR(TL_LOG_MESH,0, 0 ,"tx filters status error: cnt:%d, err:%d",mesh_filter_sts.list_size, err);
	}
#endif
	return err;
}

#ifdef WIN32
void set_pair_login_ok(u8 val)
{
	pair_login_ok = val;
}
#endif

u8 proxy_proc_filter_mesh_cmd(int idx, u16 src)
{
	if(idx < ACL_PERIPHR_MAX_NUM){
	    if(proxy_mag[idx].filter_type == FILTER_WHITE_LIST){
	        add_data_to_list(idx, src);
	    }else if (proxy_mag[idx].filter_type == FILTER_BLACK_LIST){
	        delete_data_from_list(idx, src);
	    }else{

	    }

		if(is_unicast_adr(src)){
			app_adr[idx] = src;
		}
	}

    return 0;
}

/**
 * @brief       This function server to process the configuration message.
 * @param[in]   p	- pointer of command bear which including configuration message.
 * @param[in]   len	- length of configuration message.
 * @return      1: done.
 * @note        
 */
u8 proxy_config_dispatch(mesh_cmd_bear_t *p_bear, u8 len)
{
	mesh_cmd_nw_t *p_nw = &p_bear->nw;
	proxy_config_pdu_sr *p_str = (proxy_config_pdu_sr *)(p_nw->data);
	u8 *p_addr = p_str->para;
	u8 para_len;
	u16 proxy_unicast=0;
	u8 i=0;
	
	int idx = 0;
	__UNUSED bear_conn_handle_t *p_bear_handle = (bear_conn_handle_t *)&p_bear->tx_head;
	#if BLE_MULTIPLE_CONNECTION_ENABLE
	if(BEAR_TX_PAR_TYPE_CONN_HANDLE == p_bear_handle->par_type){
		idx = get_slave_idx_by_conn_handle(p_bear_handle->conn_handle);
	}
	#endif
	
	// if not set ,use the white list 
	SET_TC_FIFO(TSCRIPT_PROXY_SERVICE, (u8 *)p_str, len-17);
	if(p_nw->src){
		#if (!PTS_TEST_EN && (NLCP_BLC_EN == 0) && (SWITCH_ALWAYS_MODE_GATT_EN == 0)) // BLCMP/BLC/PERF/BV-01-I, When sending packets, the return value of is_pkt_notify_only() must be 0
		// not record to app_adr, because this address is not added to filter list.
		// in this case, PTS send filter set, then only send network message through ADV, then message response to PTS will not be sent through both ADV and GATT, so PTS will fail to receive response.
		proxy_proc_filter_mesh_cmd(idx, p_nw->src);	
		#endif
	}
	
	switch(p_str->opcode & 0x3f){
		case PROXY_FILTER_SET_TYPE:
			// switch the list part ,and if switch ,it should clear the certain list 
			LOG_MSG_LIB(TL_LOG_NODE_SDK,0, 0,"set filter type %d ",p_str->para[0]);
			#if (MD_DF_CFG_SERVER_EN && !FEATURE_LOWPOWER_EN && !WIN32)
			if(FILTER_BLACK_LIST ==  p_str->para[0]){
				directed_proxy_dependent_node_delete(idx);
				proxy_mag[idx].proxy_client_type = BLACK_LIST_CLIENT;
				for(int i=0; i<NET_KEY_MAX; i++){
					proxy_mag[idx].directed_server[i].use_directed = 0;
				}
			}
			#endif
			proxy_mag[idx].filter_type = p_str->para[0];
			memset(proxy_mag[idx].addr_list, 0, sizeof(proxy_mag[idx].addr_list));
			send_filter_sts(idx, p_nw);
			pair_login_ok = 1;
		
			break;
		case PROXY_FILTER_ADD_ADR:
			// we suppose the num is 2
			// 18 means nid(1)ttl(1) sno(3) src(2) dst(2) opcode(1) encpryt(8)
			para_len = len-18;
			LOG_MSG_LIB(TL_LOG_NODE_SDK,p_addr,para_len,"add filter adr part ");
			for(i=0;i<para_len/2;i++){
				// swap the endianness part 
				endianness_swap_u16(p_addr+2*i);
				// suppose the data is little endianness 
				proxy_unicast = p_addr[2*i]+(p_addr[2*i+1]<<8);
				add_data_to_list(idx, proxy_unicast);
				#if (MD_DF_CFG_SERVER_EN && !FEATURE_LOWPOWER_EN && !WIN32)
				if(FILTER_WHITE_LIST == proxy_mag[idx].filter_type){
					directed_forwarding_solication_start(mesh_key.netkey_sel_dec, (mesh_ctl_path_request_solication_t *)&proxy_unicast, 1);
				}
				#endif
			}
			send_filter_sts(idx, p_nw);
			pair_login_ok = 1;
			break;
		case PROXY_FILTER_RM_ADR:
			//we suppose the num is 2
			para_len =len-18;
			LOG_MSG_LIB(TL_LOG_NODE_SDK,p_addr,para_len,"remove filter adr part ");
			for(i=0;i<para_len/2;i++){
				endianness_swap_u16(p_addr+2*i);
				proxy_unicast = p_addr[2*i]+(p_addr[2*i+1]<<8);
				delete_data_from_list(idx, proxy_unicast);
			}
			send_filter_sts(idx, p_nw);
			break;
		#if (MD_DF_CFG_SERVER_EN && !WIN32)
		case DIRECTED_PROXY_CONTROL:{
				directed_proxy_ctl_t *p_directed_ctl = (directed_proxy_ctl_t *)p_str->para;
				endianness_swap_u16((u8 *)&p_directed_ctl->addr_range);
				LOG_MSG_LIB(TL_LOG_NODE_SDK, (u8 *)p_directed_ctl, sizeof(directed_proxy_ctl_t),"directed proxy control ");
				int key_offset = get_mesh_net_key_offset(mesh_key.netkey_sel_dec);
				direct_proxy_server_t *p_direct_proxy = &proxy_mag[idx].directed_server[key_offset];
				if(!((BLACK_LIST_CLIENT == proxy_mag[idx].proxy_client_type) || (PROXY_CLIENT == proxy_mag[idx].proxy_client_type)) && (p_directed_ctl->use_directed < USE_DIRECTED_PROHIBITED)){
					if(is_directed_proxy_en(key_offset)){
						if(UNSET_CLIENT == proxy_mag[idx].proxy_client_type){					
							proxy_mag[idx].proxy_client_type = DIRECTED_PROXY_CLIENT;
							for(int i=0; i<NET_KEY_MAX; i++){
								proxy_mag[idx].directed_server[i].use_directed = 0;
							}						
						}
						
						p_direct_proxy->use_directed = p_directed_ctl->use_directed;
						if(p_directed_ctl->use_directed){
							p_direct_proxy->client_addr = p_directed_ctl->addr_range.range_start_b;
							if(p_directed_ctl->addr_range.length_present_b){
								p_direct_proxy->client_2nd_ele_cnt = p_directed_ctl->addr_range.range_length - 1;
							}					
						}
						else{
							#if !FEATURE_LOWPOWER_EN
							directed_proxy_dependent_node_delete(idx);
							#endif
						}
					}
					mesh_directed_proxy_capa_report(p_bear_handle->conn_handle, key_offset);
				}										
			}
			break;
		#endif
		default:
			break;
	}
	
	#if MD_DF_CFG_SERVER_EN
	if(UNSET_CLIENT == proxy_mag[idx].proxy_client_type){
		proxy_mag[idx].proxy_client_type = PROXY_CLIENT; 
	}
	#endif
	return 1;

}

void tn_aes_128(u8 *key, u8 *plaintext, u8 *result);
extern u8 aes_ecb_encryption(u8 *key, u8 mStrLen, u8 *mStr, u8 *result); // is the same as tn_aes_128, but no len. so use tn_aes_128 instead to decrease code size.
#if WIN32
void aes_win32(char *p, int plen, char *key);
#endif
int proxy_adv_calc_with_node_identity(u8 random[8],u8 node_key[16],u16 ele_adr,u8 hash[8])
{
	u8 adv_para[16];
	memset(adv_para,0,sizeof(adv_para));
	endianness_swap_u16((u8 *)&ele_adr);
	memset(adv_para,0,6);
	memcpy(adv_para+6,random,8);
	memcpy(adv_para+14,(u8 *)&ele_adr,2);
	#if WIN32
	aes_win32((char *)adv_para,sizeof(adv_para),(char *)node_key);
	memcpy(hash,adv_para+8,8);
	#else
	u8 adv_hash[16];
	tn_aes_128(node_key, adv_para, adv_hash);
	memcpy(hash,adv_hash+8,8);
	#endif 
	return 1;
}

#if MD_PRIVACY_BEA
int proxy_adv_calc_with_private_net_id(u8 random[8],u8 net_id[8],u8 idk[16],u8 hash[8])
{
	u8 adv_para[16];
	memcpy(adv_para,net_id,8);
	memcpy(adv_para+8,random,8);
	#if WIN32
	aes_win32((char *)adv_para,sizeof(adv_para),(char *)idk);
	memcpy(hash,adv_para+8,8);
	#else
	u8 adv_hash[16];
	tn_aes_128(idk,adv_para,adv_hash);
	memcpy(hash,adv_hash+8,8);
	#endif
	return 1;
}

int proxy_adv_calc_with_private_node_identity(u8 random[8],u8 node_key[16],u16 ele_adr,u8 hash[8])
{
	u8 adv_para[16];
	endianness_swap_u16((u8 *)&ele_adr);
	memset(adv_para,0,5);
	adv_para[5] = PRIVATE_NODE_IDENTITY_TYPE;
	memcpy(adv_para+6,random,8);
	memcpy(adv_para+14,(u8 *)&ele_adr,2);
	#if WIN32
	aes_win32((char *)adv_para,sizeof(adv_para),(char *)node_key);
	memcpy(hash,adv_para+8,8);
	#else
	u8 adv_hash[16];
	tn_aes_128(node_key,adv_para,adv_hash);
	memcpy(hash,adv_hash+8,8);
	#endif
	return 1;

}
#endif

void calculate_proxy_adv_hash(mesh_net_key_t *p_netkey )
{
	proxy_adv_calc_with_node_identity(prov_para.random,p_netkey->idk,ele_adr_primary,p_netkey->ident_hash);	
#if MD_PRIVACY_BEA
	proxy_adv_calc_with_private_net_id(prov_para.random,p_netkey->nw_id,p_netkey->idk,p_netkey->priv_net_hash);
	proxy_adv_calc_with_private_node_identity(prov_para.random,p_netkey->idk,ele_adr_primary,p_netkey->priv_ident_hash);
#endif
}

void set_proxy_adv_header(proxy_adv_node_identity * p_proxy)
{
	p_proxy->flag_len = 0x02;
	p_proxy->flag_type = 0x01;
	p_proxy->flag_data = 0x06;
	p_proxy->uuid_len = 0x03;
	p_proxy->uuid_type = 0x03;
	p_proxy->uuid_data[0]= SIG_MESH_PROXY_SRV_VAL &0xff;
	p_proxy->uuid_data[1]= (SIG_MESH_PROXY_SRV_VAL>>8)&0xff;

}

#if MD_PRIVACY_BEA
/********************************************************
1. private gatt proxy  bind with gatt proxy  
	4.2.45.1 Binding with GATT Proxy
	If the value of the GATT Proxy state of the node is Not Supported (see Table 4.27), then the value of the 
	Private GATT Proxy state shall be Not Supported and shall not be changed.
	If the value of the GATT Proxy state of the node is Enabled (see Table 4.27), then the value of the Private 
	GATT Proxy state shall be Disabled. Therefore, to change the Private GATT Proxy state to Enabled, the 
	GATT Proxy state must be set to Disabled.
		GATT Proxy state		Private GATT Proxy state
		not_supported			not_supported
		 Enabled				diabled
		 disabled				enabled
		 disabled		        disabled
		 enabled                enabled(not support)

2. private Node Identity  Binding with Node Identity 
	4.2.46.1 Binding with Node Identity
	If the value of the Node Identity state of the node is Not Supported (see Table 4.28), then the value of the 
	Private Node Identity state shall be Not Supported and shall not be changed.
	If the value of the Node Identity state of the node for any subnet is Enabled (see Table 4.28), then the 
	value of the Private Node Identity state shall be Disabled for each known subnet. Therefore, to change 
	the Private Node Identity state to Enabled, the Node Identity state must be set to Disabled for all subnets
		Node Identity 		    private Node Identity
		not_supported			not_supported
		 Enabled				diabled
		 disabled				enabled
		 disabled		        disabled
		 enabled                enabled(not support)
	so the relation ship with both have the logic of the tables 
3. 7.2.2.2.1 Advertising:

GATT Proxy state 	Private GATT Proxy 						state Advertising
	0x00 			Does Not Exist 							No Proxy Advertising
	0x00 			Disable (0x00) 							No Proxy Advertising
	0x00 			Enable (0x01) 							Private Network Identity
	0x01 			Does Not Exist or Disable (0x00) 		Network ID
	0x02 			Does Not Exist or Not Supported (0x02) 	No Proxy Advertising

Node Identity state Private Node 							Identity state Advertising
	0x00 			Does Not Exist 							No Identity Advertising
	0x00 			Disable (0x00) 							No Identity Advertising
	0x00 			Enable (0x01) 							Private Node Identity
	0x01 			Does Not Exist or Disable (0x00) 		Node Identity
	0x02 			Does Not Exist or Not Supported (0x02) 	No Identity Advertising

********************************************************/

u8 mesh_get_identity_type(mesh_net_key_t *p_netkey)
{
 	// for both MD_SERVER_EN and MD_CLIENT_EN
	u8 gatt_proxy_sts = model_sig_cfg_s.gatt_proxy;
	u8 priv_proxy_sts = g_mesh_model_misc_save.privacy_bc.proxy_sts;
	u8 node_identi = NODE_NO_PROXY_ADVERTISING;
	// node identity have higher prior
	if(p_netkey->node_identity == NODE_IDENTITY_SUB_NET_STOP && p_netkey->priv_identity == PRIVATE_NODE_IDENTITY_ENABLE){
		node_identi = PRIVATE_NODE_IDENTITY_TYPE;
	}else if (p_netkey->node_identity == NODE_IDENTITY_SUB_NET_RUN &&
			(p_netkey->priv_identity == PRIVATE_NODE_IDENTITY_NOT_SUPPORT || p_netkey->priv_identity == PRIVATE_NODE_IDENTITY_DISABLE)){
		node_identi = NODE_IDENTITY_TYPE;
	}else if ( 	p_netkey->node_identity == NODE_IDENTITY_SUB_NET_RUN && p_netkey->priv_identity == PRIVATE_NODE_IDENTITY_ENABLE){
		/*************************************************************************** 
		if the node_identity enable ,then the priv_identity will be disabled ,
		or if priv_identity enable ,node_identity will be disabled,
		they can not enable at the same time .
		****************************************************************************/
	}else if(gatt_proxy_sts == GATT_PROXY_SUPPORT_DISABLE && priv_proxy_sts == PRIVATE_PROXY_ENABLE){
		node_identi = PRIVATE_NETWORK_ID_TYPE;
	}else if ( gatt_proxy_sts == GATT_PROXY_SUPPORT_ENABLE &&
			   (priv_proxy_sts == PRIVATE_PROXY_DISABLE ||priv_proxy_sts == PRIVATE_PROXY_NOT_SUPPORT)){
		node_identi = NETWORK_ID_TYPE;
	}else if (	gatt_proxy_sts == GATT_PROXY_SUPPORT_ENABLE && priv_proxy_sts == PRIVATE_PROXY_ENABLE){
		/*************************************************************************** 
		if the gatt_proxy_sts enable ,then the priv_proxy_sts will be disabled ,
		or if priv_proxy_sts enable ,gatt_proxy_sts will be disabled,
		they can not enable at the same time .
		****************************************************************************/
	}else{
		// will not have proxy advertise 
	}
	return node_identi;
}

#endif

u8 mesh_get_proxy_privacy_para()// only can proc in the connection state
{
	#if MD_PRIVACY_BEA
	foreach(i,NET_KEY_MAX){
		mesh_net_key_t *p_netkey_base = &mesh_key.net_key[i][0];
		if(!p_netkey_base->valid){
			continue;
		}
		u8 node_identity_type = mesh_get_identity_type(p_netkey_base);
		if(	node_identity_type == PRIVATE_NETWORK_ID_TYPE || node_identity_type == PRIVATE_NODE_IDENTITY_TYPE){
			return 1;
		}else{
			return 0;
		}
	}
	return 0;
	#else
	return 0;
	#endif
}



u8 set_proxy_adv_pkt(u8 *p ,u8 *pRandom,mesh_net_key_t *p_netkey,u8 *p_len)
{
	// get the key part 
	u8 key_phase = p_netkey->key_phase;
	if(KEY_REFRESH_PHASE2 == key_phase){
		p_netkey += 1;		// use new key
	}
	// get_node_identity type part 
	u8 node_identity_type =0;
	#if MD_PRIVACY_BEA
	node_identity_type = mesh_get_identity_type(p_netkey);
		#if MD_ON_DEMAND_PROXY_EN
	if(mesh_on_demand_proxy_time){
		if(node_identity_type < NODE_NO_PROXY_ADVERTISING){
			mesh_on_demand_private_gatt_proxy_stop();
		}else{
			if(clock_time_exceed(mesh_on_demand_proxy_time, g_mesh_model_misc_save.on_demand_proxy*1000*1000)){
				mesh_on_demand_private_gatt_proxy_stop();
			}else{
				node_identity_type = PRIVATE_NETWORK_ID_TYPE;
			}
		}
	}
		#endif
	#else
	u8 gatt_proxy_sts = model_sig_cfg_s.gatt_proxy;
	if(gatt_proxy_sts == GATT_PROXY_SUPPORT_ENABLE){
		if(p_netkey->node_identity == NODE_IDENTITY_SUB_NET_RUN){
			node_identity_type = NODE_IDENTITY_TYPE;
		}else{
			node_identity_type = NETWORK_ID_TYPE;
		}
	}else{
		node_identity_type = NODE_NO_PROXY_ADVERTISING;
	}
	#endif
	
	// set the base para for the proxy adv 
	proxy_adv_node_identity * p_proxy = (proxy_adv_node_identity *)p;
	set_proxy_adv_header(p_proxy);
	p_proxy->serv_len = 0x14;
	p_proxy->serv_type = 0x16;
	p_proxy->identify_type= node_identity_type;
	if(node_identity_type == NETWORK_ID_TYPE){
		proxy_adv_net_id *p_net_id = (proxy_adv_net_id *)p;
		p_net_id->serv_len = 0x0c;
		p_net_id->serv_type = 0x16;
		p_proxy->serv_uuid[0]= SIG_MESH_PROXY_SRV_VAL &0xff;
		p_proxy->serv_uuid[1]= (SIG_MESH_PROXY_SRV_VAL>>8)&0xff;
		memcpy(p_net_id->net_id,p_netkey->nw_id,8);
	}else if(node_identity_type == NODE_IDENTITY_TYPE){
		// calculate the demo part of the proxy adv 'hash
		p_proxy->serv_len = 0x14;
		p_proxy->serv_type = 0x16;
		p_proxy->serv_uuid[0]= SIG_MESH_PROXY_SRV_VAL &0xff;
		p_proxy->serv_uuid[1]= (SIG_MESH_PROXY_SRV_VAL>>8)&0xff;
		memcpy(p_proxy->random,pRandom,8);
		memcpy(p_proxy->hash,p_netkey->ident_hash,8);
	}
	#if MD_PRIVACY_BEA
	else if (node_identity_type == PRIVATE_NETWORK_ID_TYPE){
		// calculate the demo part of the proxy adv 'hash
		p_proxy->serv_len = 0x14;
		p_proxy->serv_type = 0x16;
		p_proxy->serv_uuid[0]= SIG_MESH_PROXY_SRV_VAL &0xff;
		p_proxy->serv_uuid[1]= (SIG_MESH_PROXY_SRV_VAL>>8)&0xff;
		memcpy(p_proxy->random,pRandom,8);
		memcpy(p_proxy->hash,p_netkey->priv_net_hash,8);
	}else if (node_identity_type == PRIVATE_NODE_IDENTITY_TYPE){
		// calculate the demo part of the proxy adv 'hash
		p_proxy->serv_len = 0x14;
		p_proxy->serv_type = 0x16;
		p_proxy->serv_uuid[0]= SIG_MESH_PROXY_SRV_VAL &0xff;
		p_proxy->serv_uuid[1]= (SIG_MESH_PROXY_SRV_VAL>>8)&0xff;
		memcpy(p_proxy->random,pRandom,8);
		memcpy(p_proxy->hash,p_netkey->priv_ident_hash,8);
	}
	#endif
	else{// if(node_identity_type >= NODE_NO_PROXY_ADVERTISING){// in some condition it will not allow to send proxy adv
		return 0;
	}
	
	*p_len = p_proxy->serv_len + 8;
	return 1;
}

void push_proxy_raw_data(u8 *p_unseg){
	mesh_tx_cmd_add_packet(p_unseg);
}

u8 set_proxy_pdu_data(proxy_msg_str *p_proxy,u8 msg_type , u8 *para ,u8 para_len ){
	static u8 para_idx;
	p_proxy->type = msg_type;
	if(para_len<=MAX_PROXY_RSV_LEN){
		p_proxy->sar = COMPLETE_MSG ;
		memcpy(p_proxy->data ,para , para_len);
		push_proxy_raw_data((u8*)(p_proxy));
	}else{
		para_idx =0;
		para_len -= MAX_PROXY_RSV_LEN;
		p_proxy->sar = FIRST_SEG_MSG ;
		memcpy(p_proxy->data ,para , MAX_PROXY_RSV_LEN);
		para_idx += MAX_PROXY_RSV_LEN;
		while(para_len){
			u8 pkt_len;
			pkt_len =(para_len>=MAX_PROXY_RSV_LEN)?MAX_PROXY_RSV_LEN:para_len ;
			para_len -= pkt_len;
			if(pkt_len == MAX_PROXY_RSV_LEN){
				p_proxy->sar = CONTINUE_SEG_MSG;
				memcpy(p_proxy->data , para+para_idx,pkt_len);
				push_proxy_raw_data((u8*)(p_proxy));
			}else{
				// the last packet 
				p_proxy->sar = CONTINUE_SEG_MSG;
				memcpy(p_proxy->data , para+para_idx,pkt_len);
				push_proxy_raw_data((u8*)(p_proxy));
			}
			para_idx += pkt_len;
		}
	}
	return 1;
}
#if 0
void test_fun_private_node_identity()
{
	static u8 A_debug_hash[8];
	u8 random[8] = {0x34,0xae,0x60,0x8f,0xbb,0xc1,0xf2,0xc6};
	u8 identity_key[16] = {	0x84,0x39,0x6c,0x43,0x5a,0xc4,0x85,0x60,
							0xb5,0x96,0x53,0x85,0x25,0x3e,0x21,0x0c};
	u8 net_id[8]= {0x3e,0xca,0xff,0x67,0x2f,0x67,0x33,0x70};
	proxy_adv_calc_with_private_net_id(random,net_id,identity_key,A_debug_hash);
}

void test_fun_private_node_identity1()
{
	static u8 A_debug_hash1[8];
	u8 random[8] = {0x34,0xae,0x60,0x8f,0xbb,0xc1,0xf2,0xc6};
	u8 identity_key[16] = {	0x84,0x39,0x6c,0x43,0x5a,0xc4,0x85,0x60,
							0xb5,0x96,0x53,0x85,0x25,0x3e,0x21,0x0c};
	u16 ele_adr = 0x1201;
	proxy_adv_calc_with_private_node_identity(random,identity_key, ele_adr,A_debug_hash1);
}
void test_fun_private()
{
	test_fun_private_node_identity();
	test_fun_private_node_identity1();
	irq_disable();
	while(1);
}
#endif



