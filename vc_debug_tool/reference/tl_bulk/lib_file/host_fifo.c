/********************************************************************************************************
 * @file	host_fifo.c
 *
 * @brief	for TLSR chips
 *
 * @author	Mesh Group
 * @date	2017
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
#include "host_fifo.h"
#include "gatt_provision.h"
#include "hw_fun.h"
#include "../../../ble_lt_mesh/vendor/common/app_provison.h"
#include "../../../ble_lt_mesh/vendor/common/mesh_ota.h"

MYFIFO_INIT(hci_vc_cmd2dongle_usb_fifo, 512, 32);	// packet size may be 380

void CDTP_vs_loop();
int json_valid_iv_index();


int fifo_push_vc_cmd2dongle_usb(u8*p_tc,u8 len )
{
	return my_fifo_push(&hci_vc_cmd2dongle_usb_fifo, p_tc,len,0,0);
}

int usb_nw_pdu2dongle_with_hci_cmd(u8 *p_hci_cmd, u8 len, u16 hci_cmd)
{
	// set the header two bytes about the tmp_buf
	p_hci_cmd[0]= hci_cmd & 0xFF;
	p_hci_cmd[1]= (hci_cmd >> 8) & 0xFF;
#if VC_APP_ENABLE 
	WriteFile_host_handle(p_hci_cmd, len+HCI_CMD_LEN);
#endif
	int log_len = len+HCI_CMD_LEN;
	return log_len;
}

void mesh_rc_monitor_cb(u8 *p_payload)
{
	mesh_cmd_bear_t *p_bear = GET_BEAR_FROM_ADV_PAYLOAD(p_payload);
	u32 len_log = p_bear->len + 1;
	
	if(p_bear->lt_unseg.seg){
		len_log -= SZMIC_NW32;
		LOG_MSG_INFO(TL_LOG_MESH,p_payload,len_log,"mesh_rc_monitor_cb:Segment pkt: \r\n");
	}else{
		if(p_bear->nw.ctl){
			len_log -= SZMIC_NW64;
			LOG_MSG_INFO(TL_LOG_MESH,p_payload,len_log,"mesh_rc_monitor_cb:Control pkt: \r\n");
		}else{
			len_log -= (SZMIC_TRNS_UNSEG + SZMIC_NW32);
			LOG_MSG_INFO(TL_LOG_MESH,p_payload,len_log,"mesh_rc_monitor_cb:unSegment pkt: \r\n");
		}
	}
}

void write_cmd_fifo_poll();
u32 usb_dongle_write_intval_us = USB_DONGLE_WRITE_INTER;

// HOST Write config information to usb  ,if the fifo is empty ,it will send other cmd .
void write_dongle_cmd_fifo_poll()
{
	static u32 tick_dongle_cmd;
	if(clock_time_exceed(tick_dongle_cmd, usb_dongle_write_intval_us)){
		tick_dongle_cmd = clock_time();
		usb_dongle_write_intval_us = USB_DONGLE_WRITE_INTER;
		
		my_fifo_buf_t *fifo = (my_fifo_buf_t *)my_fifo_get(&hci_vc_cmd2dongle_usb_fifo);
		if(fifo){
			#if VC_APP_ENABLE 
			WriteFile_host_handle(fifo->data,fifo->len);
			LOG_MSG_INFO(TL_LOG_MESH,fifo->data,(u8)fifo->len,"write_dongle_cmd_fifo_poll: ble cmd to dongle:\r\n");
			#else
			// should not happen for IOS / ANDROID
			#endif 
			if(ble_module_id_is_kmadongle()){
			    u8 *p_hci_data = fifo->data;
			    if(HCI_TYPE_ACL_DATA == p_hci_data[0]){ //
			        u16 l2cap_len = p_hci_data[5] + p_hci_data[6]*256;
			        u32 rf_cnt = (l2cap_len + 22)/23;
			        if(rf_cnt > 2){ // mesh OTA chunk data
			            usb_dongle_write_intval_us = 50*1000;
			        }
			    }
			}
			my_fifo_pop(&hci_vc_cmd2dongle_usb_fifo);
		}else{
			write_cmd_fifo_poll();
		}
	}
}
/********************* write_cmd_fifo_poll****************
HOST Write special cmd :
1. when connect to the kma dongle ,it can let the kma dongle send the write cmd to ble slave
2. when the node is just a mesh node ,it can let the node send adv pkt .
**********************************/
void write_cmd_fifo_poll()	// VC send RF command to mesh dongle
{
	static u32 tick_write_cmd;
	if(clock_time_exceed(tick_write_cmd, USB_WRITE_ADV_INTERVAL)){
		tick_write_cmd = clock_time();
	
		mesh_cmd_bear_t *p_br = (mesh_cmd_bear_t*)my_fifo_get(&mesh_adv_cmd_fifo);
		if(p_br){
			int log_len = 0;

			u8 len_nw = mesh_nw_len_get_by_bear(p_br);
			u8 len_br = mesh_bear_len_get(p_br);
			u8 hci_cmd_buf[1024];
			// cp the buf data into ,and use the write file to the usb
			mesh_cmd_bear_t *p_br_send = (mesh_cmd_bear_t *)(hci_cmd_buf+HCI_CMD_LEN);
			memcpy(p_br_send, p_br, len_br);

			#if (DEBUG_MESH_DONGLE_IN_VC_EN)
			u16 hci_cmd;
			if(MESH_ADV_TYPE_PROXY_FLAG == p_br_send->type){
				hci_cmd = HCI_CMD_ADV_DEBUG_MESH_DONGLE2GATT;
			}else{
				hci_cmd = HCI_CMD_ADV_DEBUG_MESH_DONGLE2BEAR;
			}
			log_len=usb_nw_pdu2dongle_with_hci_cmd(hci_cmd_buf ,len_br, hci_cmd);
			#else
			#if (PROXY_HCI_SEL == PROXY_HCI_GATT)
			if(MESH_ADV_TYPE_BEACON == p_br_send->type){
				prov_write_data_trans(&p_br_send->len, len_br, MSG_MESH_BEACON);
			}else{
				LOG_MSG_INFO(TL_LOG_MESH,0, 0,"write_cmd_fifo_poll:push the cmd into the ble fifo ");					
				prov_write_data_trans((u8 *)&p_br_send->nw, len_nw, MSG_NETWORK_PDU);
			}
			//log_len = len_nw;	// not output log now
			#else
			log_len=usb_nw_pdu2dongle_with_hci_cmd(hci_cmd_buf ,len_br, HCI_CMD_ADV_PKT);
			#endif
			#endif
			
			#if LOG_WRITE_CMD_FIFO_EN
			if(log_len > 0){
				LOG_MSG_INFO(TL_LOG_MESH,hci_cmd_buf,log_len,
					"write_cmd_fifo_poll:%d ms, write file: adv to dongle:\r\n", clock_time()/CLOCK_SYS_CLOCK_1MS);
			}
			#else
			log_len = log_len;
			#endif
		
			my_fifo_pop(&mesh_adv_cmd_fifo);
		}
	}
}
/******************************hci_rx_fifo_poll************************
when we use the mesh_dongle ,with vc ,we can set the vc parameter ,just by the UI
*********************************************************************/
void hci_rx_fifo_poll()	// VC receive command from VC button
{
	my_fifo_buf_t *fifo = (my_fifo_buf_t *)my_fifo_get(&hci_rx_fifo);
	if(fifo){
		u8 hci_rx_cmd[1024];
		memcpy(hci_rx_cmd, fifo, fifo->len + sizeof(fifo->len));	// copy to other ram should be better
		fifo = (my_fifo_buf_t *)hci_rx_cmd;
		app_hci_cmd_from_usb_handle(fifo->data, fifo->len);
		my_fifo_pop(&hci_rx_fifo);
	}
}
/*************hci_tx_fifo_poll ***********************
Recv data about the part of the cmd from the usb:
we can recv data and dipatch by the data just by the fifo 
of the hci_tx_fifo by the function of the OnAppendLog_vs
******************************************************/ 
void reset_host_fifo()
{
	hci_tx_fifo.rptr = hci_tx_fifo.wptr;
	mesh_adv_cmd_fifo.rptr = mesh_adv_cmd_fifo.wptr;
}
_attribute_no_inline_ void hci_tx_fifo_poll()	// VC receive ADV or other pkt from mesh dongle
{

	my_fifo_buf_t *fifo = (my_fifo_buf_t *)my_fifo_get(&hci_tx_fifo);
	if(fifo){
		if(fifo->len < 1){
			return ;	// when GATT connected, after pull out master dongle, and the insert, the length of the first packet is 0 in release version.
		}
		u8 hci_tx_cmd[1024] = { 0 };
		memcpy(hci_tx_cmd, fifo, fifo->len + sizeof(fifo->len));	// copy to other ram should be better
		fifo = (my_fifo_buf_t *)hci_tx_cmd;
		#if (PROJECT_SEL == PROJECT_VC_DONGLE)
		u8 report_type = fifo->data[0];
		if(0){
		}
		else if((MESH_MONITOR_DATA | TSCRIPT_MESH_RX) == report_type){
			mesh_rc_monitor_cb(fifo->data+1);
		}
		#if PROXY_GATT_WITH_HEAD
		else if((MESH_ADV_PAYLOAD | TSCRIPT_MESH_RX) == report_type){
			u8 *p_payload = (fifo->data+1);
			#if DEBUG_MESH_DONGLE_IN_VC_EN
			app_event_handler_adv(p_payload, MESH_BEAR_ADV, 1);
			#else
			app_event_handler_adv(p_payload, MESH_BEAR_ADV, 0);
			#endif
		}
		#endif
		#if DEBUG_MESH_DONGLE_IN_VC_EN
		else if((MESH_ADV_BEAR_GATT | TSCRIPT_MESH_RX) == report_type){
			mesh_gatt_bear_handle(fifo->data+1);
		}
		else if(MESH_ADV_BLE_ST == report_type){
			extern unsigned char connect_flag_debug_mesh;
			connect_flag_debug_mesh = fifo->data[1];
			if(connect_flag_debug_mesh){
				LOG_MSG_INFO(TL_LOG_MESH,0,0,"hci_tx_fifo_poll:BLE connected");
			}else{
				LOG_MSG_INFO(TL_LOG_MESH,0,0,"hci_tx_fifo_poll:BLE disconnected");
			}
		}
		else if(MESH_ADV_ONE_PKT_COMPLETED == report_type){
			check_and_send_next_segment_pkt();
		}
		#endif
		else if((MESH_PROV | TSCRIPT_MESH_RX) == report_type){
			mesh_provision_par_set((provison_net_info_str *)(fifo->data+1));
		}else{ 
			OnAppendLog_vs(fifo->data, fifo->len);
		}
		#else
		OnAppendLog_vs(fifo->data, fifo->len);
		#endif
		my_fifo_pop(&hci_tx_fifo);
	}
}

int IsSendOpBusy(int reliable, u16 adr_dst)
{
	if((reliable && is_busy_reliable_cmd(adr_dst)
	        #if VC_APP_ENABLE
	        && (!fw_distribut_srv_proc.st_distr) // need to send other control command while OTA.
	        #endif
	    )|| is_busy_tx_seg(adr_dst)){
	    LOG_MSG_ERR (TL_LOG_COMMON, 0, 0, "tx cmd busy!........................");
		return 1;
	}
	return 0;
}

int SendOpByINI(u8 *ini_buf, u32 len)
{
	int err =0;
	mesh_bulk_cmd_par_t *cmd = (mesh_bulk_cmd_par_t *)(ini_buf + HCI_CMD_LEN);
    mesh_vendor_par_ini_t *par_vendor = (mesh_vendor_par_ini_t *)(&cmd->op);
    u16 op = get_op_u16(&cmd->op);
	int reliable = is_reliable_cmd(op, par_vendor->op_rsp);
	if(IsSendOpBusy(reliable, cmd->adr_dst)){
		return TX_ERRNO_TX_BUSY;
	}

	#if 0
	if(!json_valid_iv_index()){
		return TX_ERRNO_IV_INVALID;
	}
	#endif
	
	if(is_unicast_adr(cmd->adr_dst) && reliable && (cmd->rsp_max > 1)){
		cmd->rsp_max = 1;
	}

#if VC_APP_ENABLE
    extern u8 ble_module_id_is_gateway();
    if(ble_module_id_is_gateway()){
        ini_buf[0] =  HCI_CMD_GATEWAY_CMD & 0xFF;
        ini_buf[1] = (HCI_CMD_GATEWAY_CMD >> 8) & 0xFF;
		LOG_MSG_INFO (TL_LOG_COMMON, ini_buf, len, "VC send to gateway is");
        WriteFile_host_handle(ini_buf ,len);
    }else{
        extern void SendOpPara_vc(u8 *cmd, int len);
        SendOpPara_vc(ini_buf, len);
    }
#else
    LOG_MSG_DBG (TL_LOG_COMMON, ini_buf, len, " -- Tx cmd use INI format -- : \r\n");
    int err2 = my_fifo_push(&hci_rx_fifo, ini_buf, len, 0, 0);
    if(err2){
        err = TX_ERRNO_TX_FIFO_FULL;
    }
#endif

    return err;
}

/**************************SendOpParaDebug_VC************************
UI push data into the tx_fifo part ******************************/
int SendOpParaDebug_VC(u16 adr_dst, u8 rsp_max, u16 op, u8 *par, int len)
{
	int err =-1;
	unsigned char cmd_buf[sizeof(mesh_bulk_cmd_par_t) + HCI_CMD_LEN + 2] = {0}; // +2: for vendor op
	cmd_buf[0]= HCI_CMD_BULK_CMD2DEBUG & 0xFF;
	cmd_buf[1]= (HCI_CMD_BULK_CMD2DEBUG >> 8) & 0xFF;
	mesh_bulk_cmd_par_t *cmd = (mesh_bulk_cmd_par_t *)(cmd_buf + HCI_CMD_LEN);
	cmd->adr_dst = adr_dst;
	cmd->retry_cnt = g_reliable_retry_cnt_def;
	cmd->rsp_max = rsp_max;
	cmd->op = op & 0xff;
    if(OP_TYPE_SIG2 == GET_OP_TYPE(op)){
        if(len + 2 > MESH_CMD_ACCESS_LEN_MAX){
            LOG_MSG_ERR (TL_LOG_COMMON, 0, 0, "length error!........................");
            return TX_ERRNO_PAR_LEN_OVER_FLOW;
        }
        cmd->par[0] = op >> 8;
	    memcpy(cmd->par+1, par, len);
	    len += 1;
	}else{
        if(len + 1 > MESH_CMD_ACCESS_LEN_MAX){
            LOG_MSG_ERR (TL_LOG_COMMON, 0, 0, "length error!........................");
            return TX_ERRNO_PAR_LEN_OVER_FLOW;
        }
	    memcpy(cmd->par, par, len);
	}

	// proxy adv pkt
	int len2 = len + HCI_CMD_LEN + OFFSETOF(mesh_bulk_cmd_par_t, par);

	return SendOpByINI(cmd_buf, len2);
}

#if DEBUG_MESH_DONGLE_IN_VC_EN
int DebugMeshLed(int id, u16 y, int pol)
{
	int err =-1;
	unsigned char cmd_buf[HCI_CMD_LEN + sizeof(mesh_light_control_t)];
	cmd_buf[0]= HCI_CMD_ADV_DEBUG_MESH_LED & 0xFF;
	cmd_buf[1]= (HCI_CMD_ADV_DEBUG_MESH_LED >> 8) & 0xFF;
	mesh_light_control_t *p = (mesh_light_control_t *)(cmd_buf+HCI_CMD_LEN);
	p->id = id;
	p->pol = pol;
	p->val = y;

	fifo_push_vc_cmd2dongle_usb(cmd_buf, sizeof(cmd_buf));
	
	return 0;
}
#endif

#if VC_HOST_FIFO_EN
int VC_cb_tx_cmd_err(material_tx_cmd_t *p_mat, int err)
{
	if(is_tx_cmd_err_number_fatal(err)){
		mesh_ota_cb_tx_cmd_dev_app_key_not_found(p_mat);
	}
	return 0;
}
#endif

unsigned short crc16 (const unsigned char *pD, int len)
{
    static unsigned short poly[2]={0, 0xa001};              //0x8005 <==> 0xa001
    unsigned short crc = 0xffff;
    //unsigned char ds;
    int i,j;

    for(j=len; j>0; j--)
    {
        unsigned char ds = *pD++;
        for(i=0; i<8; i++)
        {
            crc = (crc >> 1) ^ poly[(crc ^ ds ) & 1];
            ds = ds >> 1;
        }
    }

     return crc;
}

void push_notify_into_fifo(u8 *p ,u32 len )
{
	my_fifo_push_hci_tx_fifo(p, len, 0, 0);
	return ;
}

void Thread_main_process()
{
	write_dongle_cmd_fifo_poll();
	hci_tx_fifo_poll();
	hci_rx_fifo_poll();
	mesh_loop_process();
	#if FEATURE_LOWPOWER_EN
	mesh_lpn_proc_suspend(); // must at last of main_loop()
	#endif
	CDTP_vs_loop();
}
