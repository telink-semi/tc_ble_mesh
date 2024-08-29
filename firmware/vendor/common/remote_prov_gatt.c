/********************************************************************************************************
 * @file	remote_prov_gatt.c
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
#include "remote_prov.h"
#include "tl_common.h"
#include "user_config.h"
#include "lighting_model.h"
#include "sensors_model.h"
#include "lighting_model_LC.h"
#include "mesh_ota.h"
#include "mesh_common.h"
#include "proj_lib/ble/blt_config.h"
#include "app_health.h"
#include "proj_lib/sig_mesh/app_mesh.h"
#include "proj_lib/mesh_crypto/sha256_telink.h"
#include "proj_lib/mesh_crypto/le_crypto.h"

#if MD_REMOTE_PROV&&GATT_RP_EN
#include "remote_prov_gatt.h"
u8 gatt_prov_in_handle =0x17;
u8 gatt_prov_out_handle =0x13;
u8 gatt_proxy_in_handle =0x20;
u8 gatt_proxy_out_handle =0x1c;

void dongle_add_packet(u8*pbuf,u8 len)
{
	my_fifo_push(&hci_tx_fifo,pbuf,len,0,0);
	sleep_us(100);
}

void dongle_gatt_add_whitelist(u8*pmac)
{
	u8 add_whitelist[11]={0x01,0x11,0x20,0x07,0x00,0x11,0x22,0x33,0x44,0x55,0x66};
	memcpy(add_whitelist+5,pmac,6);
	dongle_add_packet(add_whitelist, sizeof(add_whitelist));
}
void dongle_gatt_create_conn(u8*pmac)
{
	unsigned char create_connection[0x1d] ={0x01,0x0d,0x20,0x19,0x30,0x00,0x20,0x00,   0x00,0x00,0xf3,0xe1,0xe2,0xe3,0xe4,0xc7,
										  0x00,0x20,0x00,0x30,0x00,0x00,0x00,0x80,   0x00,0x00,0x00,0x00,0x00};
	memcpy(create_connection+10,pmac,6);
	dongle_add_packet(create_connection, sizeof(create_connection));
	sleep_us(20000);
}

void rp_gatt_connect(u8 *pmac)
{
	dongle_gatt_add_whitelist(pmac);
	dongle_gatt_create_conn(pmac);
}

void rp_gatt_disconnect()
{
	u8 disconnect[7]={0x01,0x06,0x04,0x03,0x80,0x00,0x13};
	dongle_add_packet(disconnect, sizeof(disconnect));
}

unsigned char rp_gatt_send_single(unsigned short handle,unsigned char *p ,unsigned char  len)
{
	unsigned char		pair_packet[256] = {0x02,0x00,0x20,0x08,0x00,0x04,0x00,0x04,0x00,0x52,0x1e,0x00,0x00};

    if(!handle){
        return 0;
    }
	// packet format 
	pair_packet[3] = len+7;
	pair_packet[5] = len+3; 
	pair_packet[9] = ATT_OP_WRITE_CMD; 
	pair_packet[10]= (unsigned char)(handle);
	memcpy(pair_packet+12,p,len);
	dongle_add_packet(pair_packet, pair_packet[3]+5);
	sleep_ms(100);
	return 1;
}

u16 master_att_mtu = 23;
int gatt_write_transaction_callback(u8 *p,u16 len,u8 msg_type)
{
	u8 tmp[256];
	u16 pkt_no=0;
	u16 buf_idx =0;
	u16 total_len;
	u16 valid_len = master_att_mtu - 4;// opcode 1 + handle_id 2 + sar 1
	u16 handle = gatt_proxy_in_handle;
	if(MSG_PROVISION_PDU == msg_type){
		handle = gatt_prov_in_handle;
	}
	total_len =len;
	pb_gatt_proxy_str *p_notify = (pb_gatt_proxy_str *)(tmp);
	if(len==0){
		return 1;
	}
	if(len>valid_len){
		while(len){
			if(!pkt_no){
				//send the first pkt
				p_notify->sar = SAR_START;
				p_notify->msgType = msg_type;	
				memcpy(p_notify->data,p,valid_len);
				rp_gatt_send_single(handle,tmp,valid_len+1);
				len = len-valid_len;
				buf_idx +=valid_len;
				pkt_no++;
			}else{
				// the last pkt 
				if(buf_idx+valid_len>=total_len){
					p_notify->sar = SAR_END;
					p_notify->msgType = msg_type;
					memcpy(p_notify->data,p+buf_idx,total_len-buf_idx);
					rp_gatt_send_single(handle,tmp,(unsigned char)(total_len-buf_idx+1));
					len =0;
				}else{
				// send the continue pkt 
					p_notify->sar = SAR_CONTINUE;
					p_notify->msgType = msg_type;
					memcpy(p_notify->data,p+buf_idx,valid_len);
					rp_gatt_send_single(handle,tmp,valid_len+1);
					len = len-valid_len;
					buf_idx +=valid_len;
				}
			}
		}
	}else{
	// send the complete pkt 
		p_notify->sar = SAR_COMPLETE;
		p_notify->msgType = msg_type;
		memcpy(p_notify->data,p,len);
		rp_gatt_send_single(handle,tmp,(unsigned char)(len+1));
	}
	return 1;
}

void rp_gatt_send_data(u8*pbuf,u8 len)
{
	LOG_MSG_INFO(TL_LOG_REMOTE_PROV,pbuf,len,"gatt send buf is :");
	gatt_write_transaction_callback(pbuf,len,MSG_PROVISION_PDU);
}


void mesh_prov_server_gatt_send(u8 *par,u8 len)
{
    remote_proc_pdu_sts_str *p_pdu_sts = &(rp_mag.rp_pdu);
    mesh_pro_data_t *p_client_rcv = (mesh_pro_data_t *)(par);
    u8 prov_cmd = p_client_rcv->invite.header.type;
	#if 0// should open for the resending part .
	// rsp the out bound packet.
	// clear the seg busy state.
	//mesh_tx_segment_finished();
	//mesh_prov_pdu_send_retry_set(0,REMOTE_PROV_SERVER_OUTBOUND_FLAG);
	// send back the outbound proc
	mesh_cmd_sig_rp_pdu_outbound_send();
	#endif
    switch(p_pdu_sts->sts){
        case RP_SRV_INVITE_SEND:
            if(prov_cmd == PRO_INVITE){
				mesh_rp_srv_tick_set();// timeout start tick 
                rp_gatt_send_data(par,len);
				mesh_prov_pdu_send_retry_set(0,REMOTE_PROV_SERVER_OUTBOUND_FLAG);
                mesh_rp_server_set_sts(PR_SRV_CAPA_RCV);
				mesh_rp_server_set_link_rp_sts(STS_PR_OUTBOUND_TRANS);
				LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"gatt CLIENT:RP_SRV_INVITE_SEND");
            }
            break;
        case RP_SRV_START_SEND:
            if(prov_cmd == PRO_START){
				mesh_prov_pdu_send_retry_clear();
				mesh_prov_pdu_send_retry_set(0,REMOTE_PROV_SERVER_OUTBOUND_FLAG);
				memcpy(&(prov_oob.start),&(p_client_rcv->start),sizeof(pro_trans_start));
                rp_gatt_send_data(par,len);
                mesh_rp_server_set_sts(RP_SRV_PUBKEY_SEND);
				mesh_rp_server_set_link_rp_sts(STS_PR_OUTBOUND_TRANS);
				LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"gatt CLIENT:RP_SRV_START_SEND");
            }
            break;
        case RP_SRV_PUBKEY_SEND:
            if(prov_cmd == PRO_PUB_KEY){
                rp_gatt_send_data(par,len);
				mesh_prov_pdu_send_retry_clear();
				mesh_prov_pdu_send_retry_set(0,REMOTE_PROV_SERVER_OUTBOUND_FLAG);
                mesh_rp_server_set_sts(PR_SRV_PUBKEY_RSP);
				mesh_rp_server_set_link_rp_sts(STS_PR_OUTBOUND_TRANS);
				LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"gatt CLIENT:RP_SRV_PUBKEY_SEND");
            }
            break;
        case PR_SRV_CONFIRM_SEND:
            if(prov_cmd == PRO_CONFIRM){
				rp_gatt_send_data(par,len);
				mesh_prov_pdu_send_retry_clear();
				mesh_prov_pdu_send_retry_set(0,REMOTE_PROV_SERVER_OUTBOUND_FLAG);
                mesh_rp_server_set_sts(PR_SRV_CONFIRM_RSP);
				mesh_rp_server_set_link_rp_sts(STS_PR_OUTBOUND_TRANS);
				LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"gatt CLIENT:PR_SRV_CONFIRM_SEND");
            }
            break;
        case PR_SRV_RANDOM_SEND:
            if(prov_cmd == PRO_RANDOM){
                rp_gatt_send_data(par,len);
				mesh_prov_pdu_send_retry_clear();
				mesh_prov_pdu_send_retry_set(0,REMOTE_PROV_SERVER_OUTBOUND_FLAG);
                mesh_rp_server_set_sts(PR_SRV_RANDOM_RSP);
				mesh_rp_server_set_link_rp_sts(STS_PR_OUTBOUND_TRANS);
				LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"gatt CLIENT:PR_SRV_RANDOM_SEND");
            }
            break;
        case PR_SRV_DATA_SEND:
            if(prov_cmd == PRO_DATA){
                rp_gatt_send_data(par,len);
				mesh_prov_pdu_send_retry_clear();
				mesh_prov_pdu_send_retry_set(0,REMOTE_PROV_SERVER_OUTBOUND_FLAG);
				mesh_rp_server_set_link_rp_sts(STS_PR_OUTBOUND_TRANS);
                mesh_rp_server_set_sts(PR_SRV_COMPLETE_RSP);    
				LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"gatt CLIENT:PR_SRV_DATA_SEND");
            }
            break;
        default:
            break;
    }
	#if (GATT_RP_EN&&(!REMOTE_PROV_GATT_RESEND_DISABLE))
		mesh_cmd_sig_rp_pdu_outbound_send();
	#endif
    return ;
}

void mesh_remote_prov_gatt_suc()
{
	mesh_rp_server_set_link_rp_sts(STS_PR_LINK_ACTIVE);
	mesh_cmd_send_link_report(REMOTE_PROV_STS_SUC,STS_PR_LINK_ACTIVE,0,2);// no reason field
	rp_mag.link_timeout = 0;
	mesh_rp_server_set_sts(RP_SRV_INVITE_SEND); 
	prov_para.link_id_filter =1;
	LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"gatt CLIENT:RP_SRV_LINK_OPEN_ACK");
}

u8 gatt_rcv_data[PROVISION_GATT_MAX_LEN];

void gatt_rp_pdu_send_retry_set_data(u8 *prov_data,u8 flag)
{
	remote_prov_retry_str *p_retry = &(rp_mag.rp_pdu.re_send);
	u8* rp_data = p_retry->adv.transStart.data;
    p_retry->retry_flag = flag;
    p_retry->tick =(clock_time()-BIT(30))|1;
	if(flag & REMOTE_PROV_SERVER_CMD_FLAG){
		memcpy(rp_data,prov_data,sizeof(pro_PB_ADV));
		rp_mag.rp_pdu.inbound++;
	}
}

void mesh_prov_server_gatt_rcv(u8 *par,u8 len)
{
    remote_proc_pdu_sts_str *p_pdu_sts = &(rp_mag.rp_pdu);
	LOG_MSG_INFO(TL_LOG_REMOTE_PROV,par,len,"gatt_rcv len is %x",len);
	if(len <= PROVISION_GATT_MAX_LEN){
		memcpy(gatt_rcv_data,par,len);
	}else{
		return ;
	}
	LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"prov sts is  %x",p_pdu_sts->sts);
    switch(p_pdu_sts->sts){
	    case PR_SRV_CAPA_RCV:
	        if(gatt_rcv_data[0]== PRO_CAPABLI){
				mesh_rp_server_set_sts(RP_SRV_START_SEND); 
				gatt_rp_pdu_send_retry_set_data(gatt_rcv_data,REMOTE_PROV_SERVER_CMD_FLAG);
				LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"gatt CLIENT:PR_SRV_CAPA_RCV");
			}
			break;
	    case PR_SRV_PUBKEY_RSP:
	        if( gatt_rcv_data[0]== PRO_PUB_KEY){
				// PR_SRV_PUBKEY_ACK
                mesh_rp_server_set_sts(PR_SRV_PUBKEY_RSP);
				gatt_rp_pdu_send_retry_set_data(gatt_rcv_data,REMOTE_PROV_SERVER_CMD_FLAG);
                mesh_rp_server_set_sts(PR_SRV_CONFIRM_SEND);
				LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"gatt CLIENT:PR_SRV_PUBKEY_RSP");
		    }
		    break;
		case PR_SRV_CONFIRM_RSP:
		    if( gatt_rcv_data[0]== PRO_CONFIRM){
				// PR_SRV_CONFIRM_SEND_ACK
                mesh_rp_server_set_sts(PR_SRV_CONFIRM_RSP);
				// when we need to send cmd ,we should stop provision cmd send first
				gatt_rp_pdu_send_retry_set_data(gatt_rcv_data,REMOTE_PROV_SERVER_CMD_FLAG);
				mesh_rp_server_set_sts(PR_SRV_RANDOM_SEND);
				LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"gatt CLIENT:PR_SRV_CONFIRM_RSP");
		    }
		    break;
        case PR_SRV_RANDOM_RSP:
            if( gatt_rcv_data[0]== PRO_RANDOM){
				// PR_SRV_RANDOM_SEND_ACK
				mesh_rp_server_set_sts(PR_SRV_RANDOM_RSP);
				gatt_rp_pdu_send_retry_set_data(gatt_rcv_data,REMOTE_PROV_SERVER_CMD_FLAG);
                mesh_rp_server_set_sts(PR_SRV_DATA_SEND);
				LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"gatt CLIENT:PR_SRV_RANDOM_RSP");
		    }
		    break;
        case PR_SRV_COMPLETE_RSP:
            if(gatt_rcv_data[0]== PRO_COMPLETE){
				// return the test case result 
					p_pdu_sts->sts = PR_SRV_COMPLETE_SUC;
					prov_para.link_id_filter = 0;
				    mesh_rp_srv_tick_reset();// timeout start tick 
				    gatt_rp_pdu_send_retry_set_data(gatt_rcv_data,REMOTE_PROV_SERVER_CMD_FLAG|REMOTE_PROV_SERVER_OUTBOUND_FLAG);
					mesh_prov_end_set_tick();// trigger event callback part 
					LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"gatt CLIENT:PR_SRV_COMPLETE_RSP");
					rp_gatt_disconnect();
			}
			break;	
	    default:
	        break;
    }
    return ;
}


u8 gatt_pkt_rp_seg_data(u8 *p,u8 len,u8* proxy_buf,u16* p_proxy_len)
{
	//package the data str 
	static u8 idx_num =0;
	u8 l2cap_len =0;
	pb_gatt_proxy_str *p_gatt;
	p_gatt = (pb_gatt_proxy_str *)(p);
	l2cap_len = len+3;
	if(p_gatt->sar == SAR_START){
		idx_num =0;
		memcpy(proxy_buf,p_gatt->data,l2cap_len-4);
		idx_num +=(l2cap_len-4);
		mesh_proxy_sar_start();
		return MSG_COMPOSITE_WAIT;
	}else if(p_gatt->sar == SAR_CONTINUE){
		memcpy(proxy_buf+idx_num,p_gatt->data,l2cap_len-4);
		idx_num +=(l2cap_len-4);
		mesh_proxy_sar_continue();
		return MSG_COMPOSITE_WAIT;
	}else if(p_gatt->sar == SAR_END){
		memcpy(proxy_buf+idx_num,p_gatt->data,l2cap_len-4);
		idx_num +=(l2cap_len-4);
		mesh_proxy_sar_end();
	}else if(p_gatt->sar == SAR_COMPLETE){
		idx_num =0;
		memcpy(proxy_buf+idx_num,p_gatt->data,l2cap_len-4);
		idx_num +=(l2cap_len-4);
		mesh_proxy_sar_complete();
	}else{}
	mesh_proxy_sar_err_terminate();
	*p_proxy_len = idx_num ;
	return p_gatt->msgType;
	// after package the data and the para 
}

u8 gatt_rp_send_buf[PROXY_GATT_MAX_LEN]; 
u8 gatt_rp_send_len=0;

// can not use the proxy_para_buf ,and proxy_par_len ,for the node can be the gatt node .
void rp_gatt_rcv_data(u8 *p,u8 len)
{
	// package the data 
	if(gatt_pkt_rp_seg_data(p,len,gatt_rp_send_buf,&gatt_rp_send_len) == MSG_COMPOSITE_WAIT){
		return ;
	}
	// get the whole packet .
	mesh_prov_server_gatt_rcv(gatt_rp_send_buf,gatt_rp_send_len);
}

void rp_gatt_hci_rcv(u8 *pu,u8 len)
{
	LOG_MSG_INFO(TL_LOG_REMOTE_PROV,pu,len,"remote prov rcv buf is ");
	if(pu[0]==DONGLE_REPORT_PROVISION_UUID){
		gatt_prov_out_handle = pu[4];
		gatt_prov_in_handle = pu[5];
		sleep_ms(1000);
		mesh_remote_prov_gatt_suc();
		rp_mag.link_timeout = 0;// clean the link timeout proc part .
		LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"gatt_prov_in_handle is %x",gatt_prov_in_handle);
	}else if (pu[0]==DONGLE_REPORT_PROXY_UUID){
		gatt_proxy_out_handle = pu[4];
		gatt_proxy_in_handle = pu[5];
		LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"gatt_proxy_in_handle is %x",gatt_proxy_in_handle);
		
	}else if(pu[0]==DONGLE_REPORT_ATT_MTU){
		master_att_mtu = pu[2] + (pu[3]<<8);
		if(master_att_mtu>54){ 
			master_att_mtu = 54;
		}
		LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"master_att_mtu is %x",master_att_mtu);
	}else if(pu[0]==DONGLE_REPORT_SPP_DATA){
		u8 data_len = pu[1];
		LOG_MSG_INFO(TL_LOG_REMOTE_PROV,pu+2,data_len,"dongle spp data is  ");
		rp_gatt_rcv_data(pu+2,data_len);
	}else if(pu[0]==MESH_CONNECTION_STS_REPORT){
		if(pu[2] == 0){// receive the link close cmd by the node .
			if(mesh_rp_server_is_active()){
				mesh_cmd_send_link_report(REMOTE_PROV_LINK_CLOSE_BY_DEVICE,STS_PR_LINK_IDLE,0,2);
			}
		}
		LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"rcv the connect sts is %x",pu[2]);
	}
}

#endif
