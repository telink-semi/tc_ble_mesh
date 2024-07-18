/********************************************************************************************************
 * @file	gateway_common.c
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
#include "gateway_common.h"
#if __TLSR_RISCV_EN__
#include "stack/ble/ble.h"
#else
#include "proj_lib/ble/service/ble_ll_ota.h"
#endif
#include "proj_lib/sig_mesh/app_mesh.h"
#include "remote_prov.h"
#include "mesh_ota.h"

#if GATEWAY_ENABLE

//u8		peer_type;
//u8		peer_mac[12];
#if (BLE_REMOTE_PM_ENABLE)
u32 gateway_iv_updata_s = 0;

#define SLEEP_MAX_S							(32*60*60)	// max 37 hours,but use 32 to be multiplied of SWITCH_IV_SEARCHING_INVL_S

	#if IV_UPDATE_TEST_EN
#define GATEWAY_IV_SEARCHING_INTERVLAL_S	(IV_UPDATE_KEEP_TMIE_MIN_RX_S)
	#else
#define GATEWAY_IV_SEARCHING_INTERVLAL_S	(96*60*60)	// keep searching for SWITCH_IV_RCV_WINDOW_S(10s default)
	#endif
#define GATEWAY_IV_SEARCHING_INVL_S			(GATEWAY_IV_SEARCHING_INTERVLAL_S - 1)
#define GATEWAY_IV_SEARCHING_INVL_S			(GATEWAY_IV_SEARCHING_INTERVLAL_S - 1)	// -1 to make sure "clock time exceed" is true when time is just expired.
#define GATEWAY_LONG_SLEEP_TIME_S			(min(SLEEP_MAX_S, GATEWAY_IV_SEARCHING_INTERVLAL_S))
#define GATEWAY_IV_RCV_WINDOW_S				(10)

/**
 * @brief       This function refresh gateway iv update time
 * @return      none
 * @note        
 */
void gateway_iv_update_time_refresh()
{
	gateway_iv_updata_s = clock_time_s();
}

/**
 * @brief       This function judge gateway whether force wakeup to receive beacon
 * @param[in]   force	- whether force to receive beacon
 * @return      none
 * @note        
 */
void gateway_trigger_iv_search_mode(int force)
{	
	if(force || clock_time_exceed_s(gateway_iv_updata_s, GATEWAY_IV_SEARCHING_INVL_S)){
		gateway_iv_update_time_refresh();		
		app_enable_scan_all_device (); 	// enable scan
		bls_pm_setSuspendMask (SUSPEND_DISABLE);
		mesh_beacon_poll_1s();
		#if GW_SMART_PROVISION_REMOTE_CONTROL_PM_EN
		blt_soft_timer_add(&soft_timer_rcv_beacon_timeout, GATEWAY_IV_RCV_WINDOW_S*1000*1000);
		#endif
		mesh_send_adv2scan_mode(0);		// enter scan mode immediately
	}
}
#endif

#if MD_SENSOR_EN
STATIC_ASSERT(FLASH_ADR_MD_SENSOR != FLASH_ADR_VC_NODE_INFO);   // address conflict
#endif

static u8 gateway_provision_para_enable=0;
void set_gateway_provision_sts(unsigned char en)
{
	gateway_provision_para_enable =en;
	return ;
}
unsigned char get_gateway_provisison_sts()
{
	unsigned char ret;
	ret = gateway_provision_para_enable;
	return ret;
}
void set_gateway_provision_para_init()
{
	gateway_adv_filter_init();
	set_provision_stop_flag_act(1);
	set_gateway_provision_sts(0);//disable the provision sts part 

}
u8 mesh_get_hci_tx_fifo_cnt()
{
#if (HCI_ACCESS == HCI_USE_USB)
	return hci_tx_fifo.size;
#elif (HCI_ACCESS == HCI_USE_UART)
	return hci_tx_fifo.size-0x10;
#else
	return 0;
#endif
}

int gateway_common_cmd_rsp_ll(u8 *p_par, u16 len, u8 *head, u8 head_len)
{
	u16 valid_fifo_size = mesh_get_hci_tx_fifo_cnt() - head_len; // 2: length
	if(len + head_len > valid_fifo_size){
		return gateway_sar_pkt_segment(p_par, len, valid_fifo_size, head, head_len);
	}
	else{
		return my_fifo_push_hci_tx_fifo(p_par,len, head, head_len); 
	}
}

int gateway_common_cmd_rsp(u8 code,u8 *p_par,u16 len )
{
	u8 head[2] = {TSCRIPT_GATEWAY_DIR_RSP};
	head[1] = code;
#if SMART_PROVISION_ENABLE
	mesh_smart_provision_rsp_handle(code, p_par, len);
#endif
	
	return gateway_common_cmd_rsp_ll(p_par, len, head, sizeof(head));
}

u8 gateway_provision_rsp_cmd(u16 unicast_adr)
{
	return gateway_common_cmd_rsp(HCI_GATEWAY_RSP_UNICAST , (u8*)(&unicast_adr),2);
}
u8 gateway_keybind_rsp_cmd(u8 opcode )
{
	return gateway_common_cmd_rsp(HCI_GATEWAY_KEY_BIND_RSP , (u8*)(&opcode),1);
}

u8 gateway_model_cmd_rsp(u8 *para,u16 len )
{
	return gateway_common_cmd_rsp(HCI_GATEWAY_RSP_OP_CODE , para,len);
}

u8 gateway_heartbeat_cb(u8 *para,u8 len )
{
	//para reference to struct:  mesh_hb_msg_cb_t 
	return gateway_common_cmd_rsp(HCI_GATEWAY_CMD_HEARTBEAT , para,len);
}

u8 gateway_upload_mac_address(u8 *p_mac,u8 *p_adv)
{
	u8 para[40];//mac(6 byte),adv data(LTV structure), rssi(1byte),dc(2byte)
	u8 len;
	len = p_adv[0];
	memcpy(para,p_mac,6);
	memcpy(para+6,p_adv,len+4); // rssi = para[6+1+len];
#if (HCI_ACCESS != HCI_USE_NONE)
	if(my_fifo_free_cnt_get(&hci_tx_fifo) < 3){ 
		return -1; // reserve 2 for communication between tool and gateway
	}
#endif

	return gateway_common_cmd_rsp(HCI_GATEWAY_CMD_UPDATE_MAC,para,len+10);
}

u8 gateway_upload_provision_suc_event(u8 evt,u16 adr,u8 *p_mac,u8 *p_uuid)
{
    gateway_prov_event_t prov;
    prov.eve = evt;
    prov.adr = adr;
    memcpy(prov.mac,p_mac,6);
    memcpy(prov.uuid,p_uuid,16);
	return gateway_common_cmd_rsp(HCI_GATEWAY_CMD_PROVISION_EVT,(u8*)&prov,sizeof(prov));
}

u8 gateway_upload_keybind_event(u8 evt)
{
	return gateway_common_cmd_rsp(HCI_GATEWAY_CMD_KEY_BIND_EVT,&evt,1);
}

u8 gateway_upload_node_ele_cnt(u8 ele_cnt)
{
	return gateway_common_cmd_rsp(HCI_GATEWAY_CMD_SEND_ELE_CNT,&ele_cnt,1);
}
u8 gateway_upload_node_info(u16 unicast)
{
	VC_node_info_t * p_info;
	p_info = get_VC_node_info(unicast,1);
	if(p_info){
		return gateway_common_cmd_rsp(HCI_GATEWAY_CMD_SEND_NODE_INFO,(u8 *)p_info,sizeof(VC_node_info_t));
	}
	
    LOG_MSG_ERR(TL_LOG_COMMON,0, 0,"upload node info failed");
	return -1;
}

int gateway_upload_gatt_ota_sts(u8 result)
{
	return gateway_common_cmd_rsp(HCI_GATEWAY_CMD_SEND_GATT_OTA_STS, &result, sizeof(result));
}

#if FAST_PROVISION_ENABLE
int fast_provision_upload_node_info(u16 unicast, u16 pid)
{
	fast_prov_node_info_t node_info;
	VC_node_info_t * p_info = get_VC_node_info(unicast,1);
	if(p_info){
		node_info.pid = pid;
		memcpy(&node_info.node_info, p_info, sizeof(VC_node_info_t));
		
		return gateway_common_cmd_rsp(HCI_GATEWAY_CMD_SEND_NODE_INFO,(u8 *)&node_info,sizeof(node_info));
	}
	
    LOG_MSG_ERR(TL_LOG_COMMON,0, 0,"upload node info failed");
	return -1;
}
#endif

u8 gateway_upload_provision_self_sts(u8 sts)
{
	u8 buf[26];
	buf[0]=sts;
	if(sts){
		memcpy(buf+1,(u8 *)(&provision_mag.pro_net_info),25);
	}
	provison_net_info_str* p_net = (provison_net_info_str*)(buf+1);
	p_net->unicast_address = provision_mag.unicast_adr_last;
	get_iv_big_endian(p_net->iv_index, (u8 *)&iv_idx_st.iv_cur);
	return gateway_common_cmd_rsp(HCI_GATEWAY_CMD_PRO_STS_RSP,buf,sizeof(buf));
}

int gateway_upload_primary_info_get()
{
	provision_primary_mesh_info_t mesh_info;
	memset(&mesh_info, 0x00, sizeof(mesh_info));
	mesh_net_key_t *p_netkey = get_nk_first_valid();
	if(p_netkey){
		memcpy(mesh_info.provision_data.net_work_key, p_netkey->key, 16);
		mesh_info.provision_data.key_index = p_netkey->index;
		get_iv_big_endian(mesh_info.provision_data.iv_index, (u8 *)&iv_idx_st.iv_cur);
		mesh_info.provision_data.unicast_address = ele_adr_primary;
		mesh_info.appkey.apk_idx = p_netkey->app_key[0].index;
		memcpy(mesh_info.appkey.app_key, p_netkey->app_key[0].key, 16);
		mesh_info.alloc_adr = provision_mag.unicast_adr_last;
	}
	return gateway_common_cmd_rsp(HCI_GATEWAY_CMD_PRIMARY_INFO_STATUS, (u8 *)&mesh_info,sizeof(mesh_info));
}

int gateway_upload_primary_info_set(provision_primary_mesh_info_t *p)
{
	mesh_provision_and_bind_self(&p->provision_data, p->appkey.app_key, p->appkey.apk_idx, p->appkey.app_key);
	
	provision_mag.unicast_adr_last = p->alloc_adr;
	provision_mag_cfg_s_store();

	mesh_tx_sec_private_beacon_proc(0);	
	return gateway_upload_primary_info_get();
}

u8 gateway_upload_mesh_ota_sts(u8 *p_dat,int len)
{
	return gateway_common_cmd_rsp(HCI_GATEWAY_CMD_SEND_MESH_OTA_STS,p_dat,len);
}

u8 gateway_upload_mesh_sno_val()
{
    return gateway_common_cmd_rsp(HCI_GATEWAY_CMD_SEND_SNO_RSP,
                        (u8 *)&mesh_adv_tx_cmd_sno,sizeof(mesh_adv_tx_cmd_sno));

}

u8 gateway_upload_dev_uuid(u8 *p_uuid,u8 *p_mac)
{
    u8 uuid_mac[22];
    memcpy(uuid_mac,p_uuid,16);
    memcpy(uuid_mac+16,p_mac,6);
    return gateway_common_cmd_rsp(HCI_GATEWAY_CMD_SEND_UUID,
                        (u8 *)uuid_mac,sizeof(uuid_mac));
}

int gateway_upload_ividx(misc_save_gw2vc_t *p_misc)
{
	if(is_iv_index_invalid() || !is_provision_success()){
    	return -1;
	}
	
	return gateway_common_cmd_rsp(HCI_GATEWAY_CMD_SEND_IVI, (u8 *)p_misc, sizeof(misc_save_gw2vc_t));
}

u8 gateway_upload_mesh_src_cmd(u16 op,u16 src,u8 *p_ac_par)
{
    gateway_upload_mesh_src_t cmd;
    cmd.op = op;
    cmd.src = src;
    memcpy(cmd.ac_par,p_ac_par,sizeof(cmd.ac_par));
    return gateway_common_cmd_rsp(HCI_GATEWAY_CMD_SEND_SRC_CMD,
                            (u8*)&cmd,sizeof(cmd));
}
#define GATEWAY_MAX_UPLOAD_CNT 0x20
u8 gateway_upload_prov_cmd(u8 *p_cmd,u8 cmd)
{
    u8 len =0;
    len = get_mesh_pro_str_len(cmd);
    if(len){
        if(len>GATEWAY_MAX_UPLOAD_CNT){
            len = GATEWAY_MAX_UPLOAD_CNT;
        }
        return gateway_common_cmd_rsp(HCI_GATEWAY_CMD_SEND,
                            (u8*)p_cmd,len);
    }
    return 0;
}

u8 gateway_upload_prov_rsp_cmd(u8 *p_rsp,u8 cmd)
{
    u8 len =0;
    len = get_mesh_pro_str_len(cmd);
    if(len){
        if(len>GATEWAY_MAX_UPLOAD_CNT){
            len = GATEWAY_MAX_UPLOAD_CNT;
        }
        return gateway_common_cmd_rsp(HCI_GATEWAY_DEV_RSP,
                            (u8*)p_rsp,len);
    }
    return 0;
}

u8 gateway_upload_prov_link_open(u8 *p_cmd,u8 len)
{
    return gateway_common_cmd_rsp(HCI_GATEWAY_CMD_LINK_OPEN,
                            (u8*)p_cmd,len);
}

u8 gateway_upload_prov_link_cls(u8 *p_rsp,u8 len)
{
    return gateway_common_cmd_rsp(HCI_GATEWAY_CMD_LINK_CLS,
                            (u8*)p_rsp,len);
}

u8 gateway_upload_mesh_cmd_back_vc(material_tx_cmd_t *p)
{
	gateway_upload_mesh_cmd_str gateway_cmd;
	u8 len ;
	gateway_cmd.src = p->adr_src;
	gateway_cmd.dst = p->adr_dst;
	gateway_cmd.opcode = p->op;
	if(p->par_len >sizeof(gateway_cmd.para)){
		len = sizeof(gateway_cmd.para);
	}else{
		len = p->par_len;
	}
	memcpy(gateway_cmd.para , p->par , len);
	len+=6;
	return gateway_common_cmd_rsp(HCI_GATEWAY_CMD_SEND_BACK_VC,
                            (u8*)(&gateway_cmd),len);
}
u8 gateway_upload_log_info(u8 *p_data,u8 len ,char *format,...) //gateway upload the print info to the vc
{
	// get the info part 
	char log_str[128];
	va_list list;
	va_start( list, format );
	char *p_buf;
	char **pp_buf;
	
	p_buf = log_str;
	pp_buf = &(p_buf);
	
	u32 head_len = print(PP_GET_PRINT_BUF_LEN_FLAG,format,list);
	if(head_len > sizeof(log_str)){
    	LOG_MSG_ERR (TL_LOG_NODE_BASIC, 0, 0, "not enough resource to print: %d", head_len);
		return 0;
	}
	
	head_len = print(pp_buf,format,list);	// log_dst[] is enough ram.
	if(head_len > sizeof(log_str)){
		return 0;	// check again
	}

	gateway_common_cmd_rsp(HCI_GATEWAY_CMD_LOG_BUF,p_data,len);
	return gateway_common_cmd_rsp(HCI_GATEWAY_CMD_LOG_STRING,(u8 *)log_str,head_len);
}

#if DEBUG_CFG_CMD_GROUP_AK_EN
u8 comm_send_cnt = 0;
u16 comm_adr_dst = 0;
u32 comm_send_flag = 0;
u32 comm_send_tick = 0;

int mesh_tx_comm_cmd(u16 adr)
{
	comm_send_tick = clock_time();
	comm_send_flag = 0;
	u8 par[32] = {0};
	mesh_bulk_vd_cmd_par_t *p_bulk_vd_cmd = (mesh_bulk_vd_cmd_par_t *)par;
	p_bulk_vd_cmd->nk_idx = 0;
    p_bulk_vd_cmd->ak_idx = 0;
	p_bulk_vd_cmd->retry_cnt = g_reliable_retry_cnt_def;
	p_bulk_vd_cmd->rsp_max = 1;
	p_bulk_vd_cmd->adr_dst = adr;
	p_bulk_vd_cmd->op = VD_MESH_TRANS_TIME_GET;
	p_bulk_vd_cmd->vendor_id = g_vendor_id;
	p_bulk_vd_cmd->op_rsp = VD_MESH_TRANS_TIME_STS;
	p_bulk_vd_cmd->tid_pos = 0;
	u8 par_len = OFFSETOF(mesh_bulk_vd_cmd_par_t, par) + (p_bulk_vd_cmd->tid_pos?2:1);
	return mesh_bulk_cmd((mesh_bulk_cmd_par_t*)p_bulk_vd_cmd, par_len);
}

void mesh_ota_comm_test()
{
	int err =-1;
	if(comm_send_flag && comm_send_cnt>0){
		err = mesh_tx_comm_cmd(comm_adr_dst);
		comm_send_cnt--;
	}
}
#endif

u8 gateway_upload_extend_adv_option(u8 option_val)
{
    return gateway_common_cmd_rsp(HCI_GATEWAY_CMD_SEND_EXTEND_ADV_OPTION,(u8 *)&option_val,sizeof(option_val));
}

#if DONGLE_PROVISION_EN
u8 is_only_report_iv = 0;
#endif
u8 ivi_beacon_key[16];

int gateway_cmd_from_host_ctl(u8 *p, u16 len )
{
	if(len<=0){
		return 0;
	}
	u8 op_code = p[0];

	if(is_iv_index_invalid()){
		if((op_code == HCI_GATEWAY_CMD_FAST_PROV_START) || (op_code == HCI_GATEWAY_CMD_RP_START) || (op_code == HCI_GATEWAY_CMD_SET_NODE_PARA)){
			return -1;
		}
	}
	
	if(op_code == HCI_GATEWAY_CMD_START){
		set_provision_stop_flag_act(0);
	}else if (op_code == HCI_GATEWAY_CMD_STOP){
		set_provision_stop_flag_act(1);
	}else if (op_code == HCI_GATEWAY_CMD_RESET){
        factory_reset();
        light_ev_with_sleep(4, 100*1000);	//10hz for about the 1s 
        start_reboot();
	}else if (op_code == HCI_GATEWAY_CMD_CLEAR_NODE_INFO){
		// clear the provision store  information 
		VC_cmd_clear_all_node_info(0xffff);// clear all node
	}else if (op_code == HCI_GATEWAY_CMD_SET_ADV_FILTER){
		set_gateway_adv_filter(p+1);
	}else if (op_code == HCI_GATEWAY_CMD_SET_PRO_PARA){
		// set provisioner net info para 
		provison_net_info_str *p_net;
		p_net = (provison_net_info_str *)(p+1);
		set_provisioner_para(p_net->net_work_key,p_net->key_index,
								p_net->flags,p_net->iv_index,p_net->unicast_address);
		provision_mag.unicast_adr_last = p_net->unicast_address + g_ele_cnt;
		provision_mag_cfg_s_store();
		// use the para (node_unprovision_flag) ,and the flag will be 0 
		
	}else if (op_code == HCI_GATEWAY_CMD_SET_NODE_PARA){
		// set the provisionee's netinfo para 
		if(is_provision_working()){
			LOG_MSG_INFO(TL_LOG_GATT_PROVISION,0,0,"gw provision is in process");
			return 0;
		}
		provison_net_info_str *p_net = (provison_net_info_str *)(p+1);
		// set the pro_data information 
		set_provisionee_para(p_net->net_work_key,p_net->key_index,
								p_net->flags,p_net->iv_index,p_net->unicast_address);
		provision_mag.unicast_adr_last = p_net->unicast_address;
		set_gateway_provision_sts(1);

	}else if (op_code == HCI_GATEWAY_CMD_START_KEYBIND){
		extern u8 pro_dat[40];
		provison_net_info_str *p_str = (provison_net_info_str *)pro_dat;
		mesh_gw_appkey_bind_str *p_bind = (mesh_gw_appkey_bind_str *)(p+1);
		mesh_cfg_keybind_start_trigger_event(p_bind->key_idx,p_bind->key,
			p_str->unicast_address,p_str->key_index,p_bind->fastbind);
	}else if (op_code == HCI_GATEWAY_CMD_SET_DEV_KEY){
        mesh_gw_set_devkey_str *p_set_devkey = (mesh_gw_set_devkey_str *)(p+1);
		if(is_own_ele(p_set_devkey->unicast)){
       	 	set_dev_key(p_set_devkey->dev_key);
		}
		else{
        	#if (DONGLE_PROVISION_EN)
			VC_node_dev_key_save(p_set_devkey->unicast,p_set_devkey->dev_key,2);
	    	#endif
		}
	}else if (op_code == HCI_GATEWAY_CMD_GET_SNO){
        gateway_upload_mesh_sno_val();
		is_only_report_iv = 1;
		mesh_misc_store();
		is_only_report_iv = 0;
	}else if (op_code == HCI_GATEWAY_CMD_SET_SNO){
        u32 sno = 0;
        memcpy((u8 *)&sno,p+1,3);
        mesh_adv_tx_cmd_sno = sno;
        mesh_misc_store();
	    LOG_MSG_LIB(TL_LOG_NODE_BASIC,0,0,"set sno by sig_mesh_tool: 0x%06x (%d)", sno, sno);
	}else if (op_code == HCI_GATEWAY_CMD_GET_PRO_SELF_STS){
		gateway_upload_provision_self_sts(is_provision_success());
		gateway_upload_node_ele_cnt(g_ele_cnt);
	}else if (op_code == HCI_GATEWAY_CMD_STATIC_OOB_RSP){
		prov_para.prov_oob_len = (len-1) >= 32 ? 32 : 16;
		mesh_set_pro_auth(p+1,prov_para.prov_oob_len);
	    //LOG_MSG_LIB(TL_LOG_NODE_BASIC,0,0,"get oob and oob len: %d, len rx: %d", provision_mag.oob_len, len-1);
	}else if (op_code == HCI_GATEWAY_CMD_GET_UUID_MAC){
        // rsp the uuid part 
        gateway_upload_dev_uuid(prov_para.device_uuid,tbl_mac);
	}else if (op_code == HCI_GATEWAY_CMD_DEL_VC_NODE_INFO){
        u16 unicast = p[1]|(p[2]<<8);
        del_vc_node_info_by_unicast(unicast);
	}else if (op_code == HCI_GATEWAY_CMD_SEND_VC_NODE_INFO){
		VC_node_info_t *p_info = (VC_node_info_t *)(p+1);
		VC_node_dev_key_save(p_info->node_adr,p_info->dev_key,p_info->element_cnt);
		gateway_common_cmd_rsp(HCI_GATEWAY_CMD_SEND_VC_NODE_INFO, NULL, 0);
	}else if (op_code == HCI_GATEWAY_CMD_MESH_OTA_ADR_SEND){
		#if MD_MESH_OTA_EN
		mesh_fw_distibut_set(1);
		mesh_cmd_sig_fw_distribut_start(p+1,len-1, 0);
		#endif
	}
	#if MESH_RX_TEST
	else if (op_code == HCI_GATEWAY_CMD_MESH_RX_TEST) {
		u8 *data = &p[1];
		u16 adr_dst = data[2] + (data[3] << 8);
		u8 ack = data[5];
		cmd_ctl_ttc_t ctl_ttc;
		memset(&ctl_ttc, 0x00, sizeof(cmd_ctl_ttc_t));
		ctl_ttc.sno_cmd = data[6] + (data[7] << 8);
		ctl_ttc.onoff = ctl_ttc.sno_cmd & 0x01;
		
		mesh_tx_cmd_layer_upper_ctl_primary(ack ? CMD_CTL_TTC_CMD : CMD_CTL_TTC_CMD_NACK, (u8 *)&ctl_ttc, sizeof(ctl_ttc), adr_dst);		
#if 0 // old 		
		mesh_cmd_g_onoff_set_t onoff_set = {0};
		u16 adr_dst = data[2] + (data[3] << 8);
		u8 rsp_max = data[4];	
		u8 ack = data[5];
		onoff_set.onoff = data[6] & 0x01;
		onoff_set.cmd_tick = clock_time();
		onoff_set.cmd_index = data[6] + (data[7] << 8);	
		int par_len = max2(data[8], sizeof(mesh_cmd_g_onoff_set_t));
		onoff_set.rsp_len = data[9];
		mesh_rsp_rec_addr = data[10] + (data[11] << 8);

		SendOpParaDebug(adr_dst, rsp_max, ack ? G_ONOFF_SET : G_ONOFF_SET_NOACK, 
						   (u8 *)&onoff_set, par_len);
#endif
	}
	#endif
	#if DEBUG_CFG_CMD_GROUP_AK_EN
	else if (op_code == HCI_GATEWAY_CMD_MESH_COMMUNICATE_TEST){
	    comm_adr_dst = p[1]|(p[2]<<8);
		comm_send_cnt = p[3];
		comm_send_flag = 1;
		mesh_tx_comm_cmd(comm_adr_dst);
	}
	#endif
	else if (op_code == HCI_GATEWAY_CMD_SET_EXTEND_ADV_OPTION){
	    u8 option_val = p[1];
	    #if EXTENDED_ADV_ENABLE
	    g_gw_extend_adv_option = option_val;
	    #else
	    if(option_val){
	        LOG_MSG_ERR(TL_LOG_NODE_BASIC,0,0,"not support extend adv option:%d",option_val);
	    }
	    option_val = EXTEND_ADV_OPTION_NONE;
	    #endif
	    LOG_MSG_LIB(TL_LOG_NODE_BASIC,0,0,"set extend adv option:%d",option_val);
	    gateway_upload_extend_adv_option(option_val);
	}
	else if(op_code == HCI_GATEWAY_CMD_FAST_PROV_START ){
		u16 pid = p[1] + (p[2]<<8);
		u16 addr = p[3] + (p[4]<<8);
		mesh_fast_prov_start(pid, addr);
	}else if (op_code == HCI_GATEWAY_CMD_RP_LINK_OPEN){
		#if GATEWAY_ENABLE&&MD_REMOTE_PROV
		u16 adr = p[1] + (p[2]<<8);
		u8 *p_uuid = p+3;
		mesh_rp_proc_en(1);
		mesh_rp_proc_set_node_adr(adr);
		mesh_cmd_sig_rp_cli_send_link_open(adr,p_uuid,0);
		mesh_rp_client_set_prov_sts(RP_PROV_IDLE_STS);
		mesh_seg_filter_adr_set(adr);
		memcpy(rp_dev_mac,p_uuid+10,6);
		memcpy(rp_dev_uuid,p_uuid,16);
		mesh_rp_pdu_retry_clear();// avoid the cmd resending part .
		#endif
	}else if (op_code == HCI_GATEWAY_CMD_RP_START){
		#if GATEWAY_ENABLE&&MD_REMOTE_PROV
		// set the provisionee's netinfo para 
		if(is_rp_working()){
			LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"remote-prov is in process",0);
			return 0;
		}
		provison_net_info_str *p_net = (provison_net_info_str *)(p+1);
		// set the pro_data information 
		set_provisionee_para(p_net->net_work_key,p_net->key_index,
								p_net->flags,p_net->iv_index,p_net->unicast_address);
		provision_mag.unicast_adr_last = p_net->unicast_address;
		// need to send invite first.
		gw_rp_send_invite();
		#endif
	}
	#if __TLSR_RISCV_EN__
	else if (op_code == HCI_GATEWAY_CMD_GET_USB_ID){
	    u16 usb_id = REG_ADDR16(0x1401fe);
	    //LOG_MSG_LIB(TL_LOG_NODE_BASIC,0,0,"usb_id:0x%x",usb_id);
		gateway_common_cmd_rsp(HCI_GATEWAY_CMD_RSP_USB_ID,(u8 *)&usb_id,sizeof(usb_id));
	}
	#endif
	else if(op_code == HCI_GATEWAY_CMD_PRIMARY_INFO_GET){
		gateway_upload_primary_info_get();
	}
	else if(op_code == HCI_GATEWAY_CMD_PRIMARY_INFO_SET){
		gateway_upload_primary_info_set((provision_primary_mesh_info_t *)(p+1));
	}
	else if (op_code == HCI_GATEWAY_CMD_SEND_NET_KEY){
		#if GATEWAY_ENABLE
		// use the netkey to create beaconkey .
		u8* p_netkey = p+1;
		mesh_sec_get_beacon_key (ivi_beacon_key, p_netkey);
		#endif
	}
	else if (op_code == HCI_GATEWAY_CMD_OTS_TX){
		#if MESH_CDTP_ENABLE
		void gateway_ots_rx_vc_cmd_handle(u8 *p_hci_cmd, u16 len);
		gateway_ots_rx_vc_cmd_handle(p, len);
		#endif
	}
	
	return 1;
}

u8 gateway_cmd_from_host_ota(u8 *p, u16 len )
{
	rf_packet_att_data_t local_ota;
	u8 dat_len ;
	dat_len = p[0];
#if (0 == __TLSR_RISCV_EN__)
	local_ota.dma_len = dat_len+9;
#endif
	local_ota.type = 0;
	local_ota.rf_len = dat_len+7;
	local_ota.l2cap = dat_len+3;
	local_ota.chanid = 4;
	local_ota.att = 0;
#if (__TLSR_RISCV_EN__)
	local_ota.handle = 0;
#else
	local_ota.hl = 0;
	local_ota.hh = 0;
#endif
	memcpy(local_ota.dat,p+1,dat_len);
	// enable ota flag 
	pair_login_ok = 1;
	u16 ota_adr =  local_ota.dat[0] | (local_ota.dat[1]<<8);
	if(ota_adr == CMD_OTA_START){
//		u32 irq_en = irq_disable();
		#if __TLSR_RISCV_EN__
		blt_ota_reset();
		#endif
		bls_ota_clearNewFwDataArea(0);
//		irq_restore(irq_en);
	}
#if BLE_MULTIPLE_CONNECTION_ENABLE
	otaWrite(BLS_HANDLE_MIN, (u8 *)&local_ota);
#else
	otaWrite((u8 *)&local_ota);
#endif
	return 0;
}

u8 gateway_cmd_from_host_mesh_ota(u8 *p, u16 len )
{
	u8 op_type =0;
	op_type = p[0];
	if(op_type == MESH_OTA_SET_TYPE){
		set_ota_reboot_flag(p[1]);
	}else if(op_type == MESH_OTA_ERASE_CTL){
		// need to erase the ota part 
		bls_ota_clearNewFwDataArea(0);
	}else{}
	return 1;
}
#endif

