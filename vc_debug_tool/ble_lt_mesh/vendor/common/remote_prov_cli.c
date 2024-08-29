/********************************************************************************************************
 * @file	remote_prov_cli.c
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
#if WIN32 // remote prov client proc part ,only concern about the gatt provision part 
#include "../../../reference/tl_bulk/lib_file/host_fifo.h"
#include "../../../reference/tl_bulk/lib_file/gatt_provision.h"
#include "../../../reference/tl_bulk/Sig_mesh_json_info.h"
#endif
#if MI_API_ENABLE 
#include "vendor/common/mi_api/telink_sdk_mible_api.h"
#endif 

#if (MD_REMOTE_PROV&&__PROJECT_MESH_PRO__)

rp_mag_cli_str rp_client;

int mesh_cmd_sig_rp_cli_send_capa(u16 node_adr)
{
    return SendOpParaDebug(node_adr, 1, REMOTE_PROV_SCAN_CAPA_GET, 0, 0);
}

int mesh_cmd_sig_rp_cli_send_scan_get(u16 node_adr)
{
    return SendOpParaDebug(node_adr, 1, REMOTE_PROV_SCAN_GET, 0, 0);
}

int mesh_cmd_sig_rp_cli_send_scan_start(u16 node_adr,u8 scan_limit,u8 timeout,u8 *p_uuid)
{
    remote_prov_scan_start scan_start;
    scan_start.scannedItemsLimit = scan_limit;
    scan_start.timeout = timeout;
    memcpy(scan_start.uuid,p_uuid,16);
    return SendOpParaDebug(node_adr, 1, REMOTE_PROV_SCAN_START, (u8 *)(&scan_start), sizeof(scan_start));
}

int mesh_cmd_sig_rp_cli_send_scan_stop(u16 node_adr)
{
    return SendOpParaDebug(node_adr, 1, REMOTE_PROV_SCAN_STOP, 0, 0);
}

int mesh_cmd_sig_rp_cli_send_extend_scan_start(u16 node_adr,u8 adcnt,
                                                             u8 *p_adfilter,u8 *p_uuid,u8 timeout)
{
    remote_prov_extend_scan_start ex_scan_start;
    ex_scan_start.ADTypeFilterCount = adcnt;
    memcpy(ex_scan_start.ADTypeFilter,p_adfilter,adcnt);
    // get the uuid pointer part
    u8 *p_start = (u8 *)(&ex_scan_start)+1+adcnt;
    memcpy(p_start,p_uuid,16);
    p_start[17] = timeout;
    return SendOpParaDebug(node_adr, 1, REMOTE_PROV_EXTEND_SCAN_START, 
                                (u8 *)&ex_scan_start, 18+adcnt);

}

int mesh_cmd_sig_rp_cli_send_link_get(u16 node_adr)
{
    return SendOpParaDebug(node_adr, 1, REMOTE_PROV_LINK_GET, 0, 0);
}
void mesh_rp_proc_en(u8 en)
{
    rp_client.rp_flag = en;
}

u8 get_mesh_rp_proc_en()
{
    return rp_client.rp_flag;
}

void mesh_rp_proc_set_node_adr(u16 unicast)
{
    rp_client.node_adr = unicast;
}

int mesh_cmd_sig_rp_cli_send_link_open(u16 node_adr,u8 *p_uuid,u8 dkri)
{
    remote_prov_link_open link_open;
	int len=0;
	if(p_uuid != NULL){
		memcpy(link_open.uuid,p_uuid,16);
		link_open.timeout = 10;
		len = sizeof(link_open);
	}else if (mesh_rp_link_dkri_is_valid(dkri)){
		link_open.dkri = dkri;
		len = 1;
	}
    rp_client.rp_flag =1;
    return SendOpParaDebug(node_adr, 1, REMOTE_PROV_LINK_OPEN, (u8 *)&link_open,len);
}

int mesh_cmd_sig_rp_cli_send_link_close(u16 node_adr,u8 reason)
{
    remote_prov_link_close link_cls;
    link_cls.reason = reason;
    rp_client.rp_flag =0;
    return SendOpParaDebug(node_adr, 1, REMOTE_PROV_LINK_CLOSE, (u8 *)&link_cls, sizeof(link_cls));
}

int mesh_cmd_sig_rp_cli_send_prov_pdu(u16 node_adr,u8 *p_pdu,u16 len)
{
    remote_prov_pdu_send prov_pdu;
    prov_pdu.OutboundPDUNumber =rp_client.outbound;
    memcpy(prov_pdu.ProvisioningPDU,p_pdu,len);
    return SendOpParaDebug(node_adr, 1, REMOTE_PROV_PDU_SEND, (u8 *)&prov_pdu, len+1);
}

void mesh_rp_client_set_retry_send()
{
    rp_mag_cli_str *p_rp = &rp_client;
    p_rp->retry_flag = 1;
    p_rp->tick = (clock_time()-BIT(30))|1;
	rp_client.outbound++;
}

int mesh_cmd_sig_cp_cli_send_invite(u8 *p,u16 len)
{
    rp_client.prov_sts = RP_PROV_INVITE_CMD_ACK;
    LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:SEND_RP_PROV_INVITE_CMD");
    return mesh_cmd_sig_rp_cli_send_pdu(p,len);
    
}

int mesh_cmd_sig_rp_cli_send_pdu(u8 *p_pdu,u16 len)
{
    mesh_rp_client_set_retry_send();
    return 1;
}

int mesh_cmd_sig_rp_cli_send_pdu_direct(u8 *p_pdu,u16 len)
{
    return mesh_cmd_sig_rp_cli_send_prov_pdu(rp_client.node_adr,p_pdu,len);
}
void mesh_rp_client_para_reset()
{
	mesh_seg_filter_adr_set(0);
    memset(&rp_client,0,sizeof(rp_client));
}

void mesh_rp_client_set_prov_sts(u8 sts)
{
    rp_mag_cli_str *p_rp = &rp_client;
    p_rp->prov_sts = sts;
}

u8 mesh_prov_is_segment(u8 data_len)
{
	if(data_len>11){
		return 1;
	}else{
		return 0;
	}
}

u8 mesh_prov_dkri_is_valid()
{
	if(rp_mag.dkri_cli & REMOTE_PROV_DKRI_EN_FLAG){
		return 1;
	}else{
		return 0;
	}
}

void mesh_prov_set_cli_dkri(u8 dkri)
{
	rp_mag.dkri_cli = dkri|REMOTE_PROV_DKRI_EN_FLAG;
}

void mesh_prov_set_adr_dev_candi(u16 adr,u8 *p_dev)
{
	rp_mag.adr_src = adr;
	memcpy(rp_mag.dev_candi,p_dev,16);
	LOG_MSG_INFO(TL_LOG_REMOTE_PROV,p_dev,16,"the adr is %d,and the devkey is ",adr);
}


void mesh_rp_pdu_retry_clear()
{
    rp_mag_cli_str *p_rp = &rp_client;
    p_rp->retry_flag =0;
    p_rp->tick =0;
}

// remote prov proc part 
int send_rp_scan_start(u16 adr,u8 limit,u8 timeout)
{
    remote_prov_scan_start scan_start;
    scan_start.scannedItemsLimit = limit;
    scan_start.timeout = timeout;
    return SendOpParaDebug(adr, 0, REMOTE_PROV_SCAN_START, (u8 *)&scan_start, 2);
}

int send_rp_extend_scan_start(u16 adr,u8* p_adtype,u8 cnt)
{
	remote_prov_extend_scan_start scan_start;
	if(p_adtype == 0 || cnt>8){
		return 0;
	}
	scan_start.ADTypeFilterCount = cnt;
	memcpy(scan_start.ADTypeFilter,p_adtype,cnt);
	LOG_MSG_ERR (TL_LOG_REMOTE_PROV, 0, 0, "send remote prov extend scan start ");
	return SendOpParaDebug(adr, 0, REMOTE_PROV_EXTEND_SCAN_START, (u8 *)&scan_start, 1+cnt);
}

u8  is_rp_working()
{
	rp_mag_cli_str *p_rp = &rp_client;
	if(p_rp->prov_sts>=RP_PROV_INVITE_CMD_ACK && p_rp->prov_sts<RP_PROV_COMPLETE_RSP){
		return 1;
	}else{
		return 0;
	}
}

u16 seg_filter_adr =0;
void mesh_seg_filter_adr_set(u16 adr)
{
	seg_filter_adr = adr;
}



#if WIN32
void mesh_rp_pdu_retry_send()
{
    rp_mag_cli_str *p_rp = &rp_client;
    if(p_rp->retry_flag){
        if( p_rp->tick!=0 && 
            clock_time_exceed(p_rp->tick,REMOTE_PROV_PDU_CLI_INTER)&&
            !is_busy_tx_segment_or_reliable_flow()){
            p_rp->tick = clock_time()|1;
            mesh_cmd_sig_rp_cli_send_pdu_direct(gatt_para_pro,gatt_para_len);
			if(mesh_prov_is_segment(gatt_para_len+2)){// if this cmd is segment pkt it will only send once .
				//mesh_rp_pdu_retry_clear();
			}
        }
    }
}

int mesh_rp_client_rx_cb(mesh_rc_rsp_t *rsp)
{
	u8 * p_data = rsp->data;
	u16 src_adr = rsp->src;
	u16 dst_adr = rsp->dst;
    rp_mag_cli_str *p_rp = &rp_client;
    remote_prov_pdu_report *p_event = (remote_prov_pdu_report *)p_data;
    u8 *p_rp_data = &p_event->InboundPDUNumber;
    if(p_event->opcode == REMOTE_PROV_SCAN_CAPA_STS){
        remote_prov_scan_cap_sts *p_capa = (remote_prov_scan_cap_sts *)p_rp_data;
        LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:REMOTE_PROV_SCAN_CAPA_STS,\
                maxScannedItems is 0x%x,activeScan is 0x%x",p_capa->maxScannedItems,p_capa->activeScan);
        remote_prov_capa_sts_cb(p_capa->maxScannedItems,p_capa->activeScan);
    }else if (p_event->opcode == REMOTE_PROV_SCAN_STS){
        remote_prov_scan_sts *p_scan = (remote_prov_scan_sts *)p_rp_data;
        LOG_MSG_INFO(TL_LOG_REMOTE_PROV,(u8 *)p_scan,sizeof(remote_prov_scan_sts),"CLIENT:REMOTE_PROV_SCAN_STS");
    }else if (p_event->opcode == REMOTE_PROV_SCAN_REPORT){
        remote_prov_scan_report *p_report = (remote_prov_scan_report *)p_rp_data;
        LOG_MSG_INFO(TL_LOG_REMOTE_PROV,(u8 *)p_report,sizeof(remote_prov_scan_report),"CLIENT:REMOTE_PROV_SCAN_REPORT(rssi:%2ddB)",p_report->rssi);
        remote_prov_scan_report_win32 rep_win32;
        // if(0x0002 == src_adr) // for test pointing special one to be remote proxy node.
		{
	        rep_win32.unicast = src_adr;
	        memcpy((u8 *)&(rep_win32.scan_report),p_report,sizeof(remote_prov_scan_report));
	        remote_prov_scan_report_cb((u8 *)&rep_win32,sizeof(remote_prov_scan_report_win32));
        }
    }else if (p_event->opcode == REMOTE_PROV_EXTEND_SCAN_REPORT){
        remote_prov_extend_scan_report *p_report = (remote_prov_extend_scan_report *)p_rp_data;
        LOG_MSG_INFO(TL_LOG_REMOTE_PROV,(u8 *)p_report,sizeof(remote_prov_extend_scan_report),"CLIENT:REMOTE_PROV_EXTEND_SCAN_REPORT");
		remote_prov_scan_report_win32 rep_win32;
        rep_win32.unicast = src_adr;
        memcpy(rep_win32.scan_report.uuid,p_report->uuid,sizeof(p_report->uuid));
		rep_win32.scan_report.oob = p_report->OOBinformation;
        remote_prov_scan_report_cb((u8 *)&rep_win32,sizeof(remote_prov_scan_report_win32));
    }else if (p_event->opcode == REMOTE_PROV_LINK_STS){
        remote_prov_link_status *p_link_sts = (remote_prov_link_status *)p_rp_data;
        LOG_MSG_INFO(TL_LOG_REMOTE_PROV,(u8 *)p_link_sts,sizeof(remote_prov_link_status),"CLIENT:REMOTE_PROV_LINK_STS");
		if(p_rp->prov_sts == RP_PROV_COMPLETE_RSP){
			// wait for the link close status ,and then change the adr part .
			mesh_prov_dev_candi_store_proc(src_adr);
	        provision_end_callback(PROV_NORMAL_RET);
	        gatt_provision_mag.provison_send_state = STATE_PRO_SUC;
	        mesh_rp_client_para_reset();
	        LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_COMPLETE_RSP");
	        cache_init(ADR_ALL_NODES);
		}
	}else if (p_event->opcode == REMOTE_PROV_LINK_REPORT){ 
        remote_prov_link_report *p_link_rep = (remote_prov_link_report *)p_rp_data;
        LOG_MSG_INFO(TL_LOG_REMOTE_PROV,(u8 *)p_link_rep,sizeof(remote_prov_link_report),"CLIENT:REMOTE_PROV_LINK_REPORT");
    }else if(p_event->opcode == REMOTE_PROV_PDU_OUTBOUND_REPORT || REMOTE_PROV_PDU_REPORT){
        mesh_pro_data_t *p_send = (mesh_pro_data_t *)gatt_para_pro;
        mesh_pro_data_t *p_rcv = (mesh_pro_data_t *)p_event->ProvisioningPDU;
		if(p_event->opcode == REMOTE_PROV_PDU_OUTBOUND_REPORT){
			LOG_MSG_INFO(TL_LOG_REMOTE_PROV,&(p_event->InboundPDUNumber),1,"REMOTE_PROV_PDU_REPORT,inbound num is %d",p_event->InboundPDUNumber);
		}else if(p_event->opcode == REMOTE_PROV_PDU_REPORT){
			LOG_MSG_INFO(TL_LOG_REMOTE_PROV,&(p_event->InboundPDUNumber),2,"REMOTE_PROV_PDU_OUTBOUND_REPORT data is ");
		}
        u8 prov_code = p_event->ProvisioningPDU[0];
        if(prov_code == PRO_FAIL && p_event->opcode == REMOTE_PROV_PDU_REPORT){
            // terminate,need to send link close cmd 
            // reset all the status part 
            LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:PRO_FAIL");
            mesh_rp_client_para_reset();
            mesh_cmd_sig_rp_cli_send_link_close(rp_client.node_adr,UNEXPECTED_ERROR);        
        }
        //prov the rp sts ,and the rp opcode part 
        switch(p_rp->prov_sts){
            // rcv capa and send start cmd 
            case RP_PROV_INVITE_CMD_ACK:
				if ((p_event->opcode == REMOTE_PROV_PDU_OUTBOUND_REPORT) && 
					(p_event->InboundPDUNumber == rp_client.outbound) ){
					LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_INVITE_CMD_ACK");
					mesh_rp_client_set_prov_sts(RP_PROV_CAPA_RSP);
					mesh_rp_pdu_retry_clear();
				}
				break;
            case RP_PROV_CAPA_RSP:
                if(p_event->opcode == REMOTE_PROV_PDU_REPORT){
                    // receive the capa cmd 
                    if(prov_code == PRO_CAPABLI){
                        gatt_prov_rcv_capa(p_rcv,0);
                        LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_CAPA_RSP");
                        mesh_rp_client_set_prov_sts(RP_PROV_START_CMD);
						gatt_prov_send_start(p_rcv,p_send);
    					mesh_cmd_sig_rp_cli_send_pdu(gatt_para_pro,gatt_para_len);  
						LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_START_CMD");
						mesh_rp_client_set_prov_sts(RP_PROV_START_CMD_ACK);
						break;
                    }else{
                        break;
                    }
                }else{
                    break;
                } 
			case RP_PROV_START_CMD_ACK:
				if(p_event->opcode == REMOTE_PROV_PDU_OUTBOUND_REPORT && 
						p_event->InboundPDUNumber == rp_client.outbound ){
					LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_START_CMD_ACK");
					mesh_rp_pdu_retry_clear();
					mesh_rp_client_set_prov_sts(RP_PROV_PUBKEY_CMD);
					gatt_prov_send_pubkey(p_rcv,p_send);
                    mesh_cmd_sig_rp_cli_send_pdu(gatt_para_pro,gatt_para_len);
                    LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_PUBKEY_CMD");
					mesh_rp_client_set_prov_sts(RP_PROV_PUBKEY_CMD_ACK);
				}
				break;
            case RP_PROV_PUBKEY_CMD_ACK:
                if((p_event->opcode == REMOTE_PROV_PDU_OUTBOUND_REPORT) && 
					(p_event->InboundPDUNumber == rp_client.outbound) ){
					LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_PUBKEY_CMD_ACK");
					mesh_rp_pdu_retry_clear();
					mesh_rp_client_set_prov_sts(RP_PROV_PUBKEY_RSP);
                }
                break;
            case RP_PROV_PUBKEY_RSP:
                if(p_event->opcode == REMOTE_PROV_PDU_REPORT){
                    // receive the pubkey from the node 
                    if(prov_code == PRO_PUB_KEY){
                        gatt_prov_rcv_pubkey(p_rcv);
                        LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_PUBKEY_RSP");
                        mesh_rp_client_set_prov_sts(RP_PROV_CONFIRM_CMD);
						gatt_prov_send_confirm(p_send);
                		mesh_cmd_sig_rp_cli_send_pdu(gatt_para_pro,gatt_para_len);
                		LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_CONFIRM_CMD");
						mesh_rp_client_set_prov_sts(RP_PROV_CONFIRM_CMD_ACK);
                    }else{
                        break;
                    }
                }else{
                    break;
                }
			case RP_PROV_CONFIRM_CMD_ACK:
				if((p_event->opcode == REMOTE_PROV_PDU_OUTBOUND_REPORT) && 
					(p_event->InboundPDUNumber == rp_client.outbound) ){
					mesh_rp_pdu_retry_clear();
					LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_CONFIRM_CMD_ACK");
                    mesh_rp_client_set_prov_sts(RP_PROV_CONFIRM_RSP);
                }
                break;
            case RP_PROV_CONFIRM_RSP:
                if(p_event->opcode == REMOTE_PROV_PDU_REPORT){
                    // receive the pubkey from the node 
                    if(prov_code == PRO_CONFIRM){
                        gatt_prov_rcv_confirm(p_rcv,p_send);
                        LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_CONFIRM_RSP");
                        mesh_rp_client_set_prov_sts(RP_PROV_RANDOM_CMD);
						gatt_prov_send_random(p_rcv,p_send);
                		mesh_cmd_sig_rp_cli_send_pdu(gatt_para_pro,gatt_para_len);
                		LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_RANDOM_CMD");
						mesh_rp_client_set_prov_sts(RP_PROV_RANDOM_CMD_ACK);
                    }else{
                        break;
                    }
                }else{
                    break;
                }
            case RP_PROV_RANDOM_CMD_ACK:
				if((p_event->opcode == REMOTE_PROV_PDU_OUTBOUND_REPORT) && 
					(p_event->InboundPDUNumber == rp_client.outbound) ){
					mesh_rp_pdu_retry_clear();
					LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_RANDOM_CMD_ACK");
                    mesh_rp_client_set_prov_sts(RP_PROV_RANDOM_RSP);
                }
                break;
            case RP_PROV_RANDOM_RSP:
                if(p_event->opcode == REMOTE_PROV_PDU_REPORT){
                    // receive the pubkey from the node 
                    if(prov_code == PRO_RANDOM){
                        gatt_prov_rcv_random(p_rcv,p_send);
                        LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_RANDOM_RSP");
                        mesh_rp_client_set_prov_sts(RP_PROV_DATA_CMD);
						gatt_prov_send_pro_data(0,p_send);
                		mesh_cmd_sig_rp_cli_send_pdu(gatt_para_pro,gatt_para_len);
                		LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_DATA_CMD");
						mesh_rp_client_set_prov_sts(RP_PROV_DATA_CMD_ACK);
                    }else{
                        break;
                    }
                }else{
                    break;
                }
            case RP_PROV_DATA_CMD_ACK:
				if((p_event->opcode == REMOTE_PROV_PDU_OUTBOUND_REPORT) && 
					(p_event->InboundPDUNumber == rp_client.outbound) ){
					mesh_rp_pdu_retry_clear();
					LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_DATA_CMD_ACK");
                    mesh_rp_client_set_prov_sts(RP_PROV_COMPLETE_RSP);
                }
                break;
            case RP_PROV_COMPLETE_RSP:
                if(p_event->opcode == REMOTE_PROV_PDU_REPORT){
                    // receive the pubkey from the node 
                    if(prov_code == PRO_COMPLETE){
                        prov_timer_clr();
                        gatt_prov_rcv_pro_complete();
						LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"the link close adr is %d",rp_client.node_adr);
						mesh_cmd_sig_rp_cli_send_link_close(rp_client.node_adr,REMOTE_PROV_LINK_CLOSE_SUC);
						mesh_rp_pdu_retry_clear();
                    }
                }
                break;
            default: break;
               
        } 
    }
	return 1;
}

void mesh_rp_start_settings(u16 adr,u8 *p_uuid,u8 dkri)
{
    prov_timer_start();
    mesh_rp_proc_en(1);
	mesh_rp_proc_set_node_adr(adr);
	mesh_cmd_sig_rp_cli_send_link_open(adr,p_uuid,dkri);
	mesh_rp_client_set_prov_sts(RP_PROV_IDLE_STS);
	mesh_seg_filter_adr_set(adr);
	mesh_rp_pdu_retry_clear();// avoid the cmd resending part .
}

void mesh_prov_dev_candi_store_proc(u16 cmd_src)
{
	LOG_MSG_INFO(TL_LOG_REMOTE_PROV,rp_mag.dev_candi,16,"mesh_prov_dev_candi_store_proc");
	if(mesh_prov_dkri_is_valid()){
		rp_mag.dkri_cli &= ~REMOTE_PROV_DKRI_EN_FLAG;
		LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"mesh_prov_dkri_is_valid ");
	}
	if(rp_mag.dkri_cli == RP_DKRI_DEV_KEY_REFRESH){
		LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"RP_DKRI_DEV_KEY_REFRESH proc");
		VC_node_dev_key_save_candi(rp_mag.adr_src,rp_mag.dev_candi);		
	}else if(rp_mag.dkri_cli == RP_DKRI_NODE_ADR_REFRESH){
		VC_node_replace_devkey_candi_adr(cmd_src,rp_mag.adr_src,rp_mag.dev_candi);// update the candi part 
		//update the json file part .
		json_update_node_adr_devicekey(rp_mag.adr_src,cmd_src,rp_mag.dev_candi);
	}else if (rp_mag.dkri_cli == RP_DKRI_NODE_CPS_REFRESH){
		VC_node_dev_key_save_candi(rp_mag.adr_src,rp_mag.dev_candi);
	}
}
#else

u8 rp_pro_input[0x91];
u8 rp_dev_random[32];
u8 rp_pro_random[32];
u8 rp_pro_dev_confirm[32];
u8 rp_pro_confirm[32];
u8 rp_ecdh_secret[32];
u8 rp_node_ele_cnt =0;
u8 rp_gw_ppk[64];
u8 rp_gw_psk[32];
u8 pro_dev_key[16];
u16 pro_uni_adr =0;
u8 gw_rp_mode = 0;
u8 rp_dev_mac[6];
u8 rp_dev_uuid[16];

#define MAX_GW_RP_TIMEOUT_S 60
u32 gw_rp_tick =0;
void gw_rp_time_set()
{
	gw_rp_tick = clock_time()|1;
}

void gw_rp_time_clear()
{
	gw_rp_tick = 0;
}

void gw_rp_timeout_proc()
{
	if(gw_rp_tick && clock_time_exceed(gw_rp_tick,MAX_GW_RP_TIMEOUT_S*1000*1000)){
		gw_rp_tick = 0;
		LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"gw_rp_timeout_proc,link adr is %d",rp_client.node_adr);
        mesh_rp_client_para_reset();
        mesh_cmd_sig_rp_cli_send_link_close(rp_client.node_adr,UNEXPECTED_ERROR);  
	}
}

void mesh_prov_dev_candi_store_proc(u16 cmd_src)
{
	LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"mesh_prov_dev_candi_store_proc ");
	if(mesh_prov_dkri_is_valid()){
		rp_mag.dkri_cli &= ~REMOTE_PROV_DKRI_EN_FLAG;
		LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"mesh_prov_dkri_is_valid ");
	}
	if(rp_mag.dkri_cli == RP_DKRI_DEV_KEY_REFRESH){
		LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"RP_DKRI_DEV_KEY_REFRESH proc");
		VC_node_dev_key_save_candi(rp_mag.adr_src,rp_mag.dev_candi);		
	}else if(rp_mag.dkri_cli == RP_DKRI_NODE_ADR_REFRESH){
		VC_node_replace_devkey_candi_adr(cmd_src,rp_mag.adr_src,rp_mag.dev_candi);// update the candi part 
		//update the json file part.
		//json_update_node_adr_devicekey(rp_mag.adr_src,cmd_src,rp_mag.dev_candi);
	}else if (rp_mag.dkri_cli == RP_DKRI_NODE_CPS_REFRESH){
		VC_node_dev_key_save_candi(rp_mag.adr_src,rp_mag.dev_candi);
	}
}

void gw_rp_send_invite()
{
	mesh_pro_data_t *p_send = (mesh_pro_data_t *)(para_pro);
	set_pro_invite(p_send,0);
	para_len = sizeof(pro_trans_invite);
	mesh_cmd_sig_cp_cli_send_invite(para_pro,para_len);
	gateway_upload_prov_cmd((u8 *)p_send,PRO_INVITE);
	gw_rp_time_set();
}

void gw_prov_rcv_capa(mesh_pro_data_t *p_rcv,mesh_pro_data_t *p_send)
{
    memset(&prov_oob,0,sizeof(prov_oob));// clean the prov oob for gatt provision part 
	memcpy(rp_pro_input+1,&(p_rcv->capa.ele_num),11);
	rp_node_ele_cnt = p_rcv->capa.ele_num;
	gateway_upload_node_ele_cnt(rp_node_ele_cnt);
	swap_mesh_pro_capa(p_rcv);// swap the endianness for the capa data 
	memcpy(&prov_oob.capa , &p_rcv->capa,sizeof(p_rcv->capa));
	get_pubkey_oob_info_by_capa(&prov_oob);
	LOG_MSG_INFO(TL_LOG_GATT_PROVISION,(u8*)&prov_oob.capa,sizeof(prov_oob.capa),"RCV:the provision capa data is \r\n");
}  

void gw_prov_send_start(mesh_pro_data_t *p_rcv,mesh_pro_data_t *p_send)
{
	set_start_para_by_capa(&prov_oob);
	if(mesh_prov_oob_auth_data(&prov_oob)){
		if(MESH_STATIC_OOB == prov_oob.prov_key){
			LOG_MSG_INFO(TL_LOG_GATT_PROVISION, 0, 0,"can't find static oob, try with no oob!\r\n");
			prov_oob.prov_key = MESH_NO_OOB;
			set_start_para_by_capa(&prov_oob);
		}
	}
	set_pro_start_simple(p_send,&(prov_oob.start));
	memcpy(rp_pro_input+12,&(p_send->start.algorithms),5);
	para_len = sizeof(pro_trans_start);
	LOG_MSG_INFO(TL_LOG_GATT_PROVISION,(u8*)&prov_oob.start,sizeof(prov_oob.start),"SEND:the provision start is \r\n");
	gw_rp_time_set();
}

void gw_prov_send_pubkey(mesh_pro_data_t *p_rcv,mesh_pro_data_t *p_send)
{
    // send the pub_key pkt 
	get_public_key(rp_gw_ppk);
	set_pro_pub_key(p_send,rp_gw_ppk,rp_gw_ppk+32);
	para_len = sizeof(pro_trans_pubkey);
	LOG_MSG_INFO(TL_LOG_GATT_PROVISION,para_pro+1,(u8)para_len-1,"SEND:provisioner send pubkey is \r\n");
	
}

void gw_prov_rcv_pubkey(mesh_pro_data_t *p_rcv)
{
	get_private_key(rp_gw_psk);
	memcpy(rp_pro_input+0x11,rp_gw_ppk,sizeof(rp_gw_ppk));
	memcpy(rp_pro_input+0x11+64,p_rcv->pubkey.pubKeyX,64);
	LOG_MSG_INFO(TL_LOG_GATT_PROVISION,p_rcv->pubkey.pubKeyX,64,"RCV:the pubkey of the device is \r\n");
	tn_p256_dhkey_fast(rp_ecdh_secret, rp_gw_psk,rp_pro_input+0x11+0x40 ,rp_pro_input+0x11+0x60);    
}

void gw_prov_send_confirm(mesh_pro_data_t *p_send)
{
	
	mesh_sec_prov_confirmation_sec (rp_pro_confirm, rp_pro_input, 
								145, rp_ecdh_secret, rp_pro_random, pro_auth);
	set_pro_confirm(p_send,rp_pro_confirm,get_prov_confirm_value_len());
	para_len =get_prov_confirm_len();
	LOG_MSG_INFO(TL_LOG_GATT_PROVISION,para_pro+1,(u8)para_len-1,"SEND:the provisioner's confirm is \r\n");    
}

void gw_prov_rcv_confirm(mesh_pro_data_t *p_rcv,mesh_pro_data_t *p_send)
{
	pro_trans_confirm *p_confirm = &(p_rcv->confirm);
	#if PROV_AUTH_LEAK_REFLECT_EN
	if(prov_confirm_check_same_or_not(p_confirm->confirm,rp_pro_confirm)){
		LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:PRO_FAIL");
        mesh_rp_client_para_reset();
        mesh_cmd_sig_rp_cli_send_link_close(rp_client.node_adr,UNEXPECTED_ERROR); 
		gw_rp_time_clear();
	}
	#endif
	// store the dev comfrim part 
	LOG_MSG_INFO(TL_LOG_GATT_PROVISION,p_confirm->confirm,get_prov_confirm_value_len(),"RCV:the device's confirm is \r\n");
	memcpy(rp_pro_dev_confirm,p_confirm->confirm,get_prov_confirm_value_len());    
}

void gw_prov_send_random(mesh_pro_data_t *p_rcv,mesh_pro_data_t *p_send)
{
    set_pro_random(p_send,rp_pro_random,get_prov_random_len()-1);
	para_len = get_prov_random_len();
	LOG_MSG_INFO(TL_LOG_GATT_PROVISION,para_pro+1,(u8)para_len-1,"SEND:the provisioner's random is \r\n");    
}

void gw_prov_rcv_random(mesh_pro_data_t *p_rcv,mesh_pro_data_t *p_send)
{
	memcpy(rp_dev_random,p_rcv->random.random,get_prov_random_value_len());
	LOG_MSG_INFO(TL_LOG_GATT_PROVISION,p_rcv->random.random,get_prov_random_value_len(),"RCV:the device's random is \r\n");
	u8 pro_dev_confirm_tmp[32];
	mesh_sec_prov_confirmation_sec (pro_dev_confirm_tmp, rp_pro_input, 
							145, rp_ecdh_secret, rp_dev_random, pro_auth);
	if(prov_confirm_check_right_or_not(rp_pro_dev_confirm,pro_dev_confirm_tmp)){
		LOG_MSG_INFO(TL_LOG_GATT_PROVISION,pro_dev_confirm_tmp,get_prov_confirm_value_len(),"provision confirm fail\r\n");
		LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:PRO_FAIL");
        mesh_rp_client_para_reset();
        mesh_cmd_sig_rp_cli_send_link_close(rp_client.node_adr,UNEXPECTED_ERROR);
		gw_rp_time_clear();
	}
	LOG_MSG_INFO(TL_LOG_GATT_PROVISION,0,0,"the device confirm check is success");
	//calculate the dev_key part 
	u8 prov_salt[16];
	mesh_sec_prov_salt_fun(prov_salt,rp_pro_input,rp_pro_random,rp_dev_random,is_prov_oob_hmac_sha256());
	//calculate the dev_key part 
	mesh_sec_dev_key(pro_dev_key,prov_salt,rp_ecdh_secret);
	   
}

void gw_prov_send_pro_data(mesh_pro_data_t *p_rcv,mesh_pro_data_t *p_send)
{
	u8 pro_session_key[16];
	u8 pro_session_nonce_tmp[16];
	u8 pro_session_nonce[16];
	LOG_MSG_INFO(TL_LOG_REMOTE_PROV,pro_dat,25,"SEND:the provisioner's device info is  \r\n");
	#if (IS_VC_PROJECT_MASTER)
	LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"gatt_prov_send_pro_data set provision data");
		#if MD_REMOTE_PROV
		if(mesh_prov_dkri_is_valid()){
			mesh_prov_set_adr_dev_candi(unicast_address,pro_dev_key);
		}else{
			VC_node_dev_key_save(unicast_address, pro_dev_key,rp_node_ele_cnt);
		}
		#else
		VC_node_dev_key_save(unicast_address, pro_dev_key,rp_node_ele_cnt);
		#endif
	#endif 
	LOG_MSG_INFO(TL_LOG_REMOTE_PROV,pro_dev_key,16,"the node's dev key: \r\n");
	mesh_sec_prov_session_key_fun(pro_session_key, pro_session_nonce_tmp, rp_pro_input, 145, rp_ecdh_secret, rp_pro_random, rp_dev_random,is_prov_oob_hmac_sha256());
	memcpy(pro_session_nonce,pro_session_nonce_tmp+3,13); 
    u8 gatt_pro_dat_tmp[40];
	memcpy(gatt_pro_dat_tmp,pro_dat,sizeof(gatt_pro_dat_tmp));
	mesh_prov_sec_msg_enc (pro_session_key, pro_session_nonce, gatt_pro_dat_tmp, 25, 8);
	set_pro_data(p_send,gatt_pro_dat_tmp,gatt_pro_dat_tmp+25);
	para_len = sizeof(pro_trans_data);    
}

void mesh_rp_pdu_retry_send()
{
	int err =-1;
    rp_mag_cli_str *p_rp = &rp_client;
	gw_rp_timeout_proc();
    if(p_rp->retry_flag){
        if( p_rp->tick!=0 && 
            clock_time_exceed(p_rp->tick,REMOTE_PROV_PDU_CLI_INTER)&&
            !is_busy_tx_segment_or_reliable_flow()){
            p_rp->tick = clock_time()|1;
            err = mesh_cmd_sig_rp_cli_send_pdu_direct(para_pro,para_len);
			if(mesh_prov_is_segment(para_len+2)&&err == 0){// if this cmd is segment pkt it will only send once .
				//mesh_rp_pdu_retry_clear();
			}
        }
    }
}

int mesh_rp_client_rx_cb(mesh_rc_rsp_t *rsp)
{
	u8 * p_data = rsp->data;
	u16 src_adr = rsp->src;
    rp_mag_cli_str *p_rp = &rp_client;
    remote_prov_pdu_report *p_event = (remote_prov_pdu_report *)p_data;
    u8 *p_rp_data = &p_event->InboundPDUNumber;
    if(p_event->opcode == REMOTE_PROV_SCAN_CAPA_STS){
        remote_prov_scan_cap_sts *p_capa = (remote_prov_scan_cap_sts *)p_rp_data;
        p_capa = p_capa;
        LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:REMOTE_PROV_SCAN_CAPA_STS,\
                maxScannedItems is 0x%x,activeScan is 0x%x",p_capa->maxScannedItems,p_capa->activeScan);
    }else if (p_event->opcode == REMOTE_PROV_SCAN_STS){
        remote_prov_scan_sts *p_scan = (remote_prov_scan_sts *)p_rp_data;
        p_scan = p_scan;
        LOG_MSG_INFO(TL_LOG_REMOTE_PROV,(u8 *)p_scan,sizeof(remote_prov_scan_sts),"CLIENT:REMOTE_PROV_SCAN_STS");
    }else if (p_event->opcode == REMOTE_PROV_SCAN_REPORT){
        remote_prov_scan_report *p_report = (remote_prov_scan_report *)p_rp_data;
        p_report = p_report;
        LOG_MSG_INFO(TL_LOG_REMOTE_PROV,(u8 *)p_report,sizeof(remote_prov_scan_report),"CLIENT:REMOTE_PROV_SCAN_REPORT");
    }else if (p_event->opcode == REMOTE_PROV_EXTEND_SCAN_REPORT){
        remote_prov_extend_scan_report *p_report = (remote_prov_extend_scan_report *)p_rp_data;
        LOG_MSG_INFO(TL_LOG_REMOTE_PROV,(u8 *)p_report,sizeof(remote_prov_extend_scan_report),"CLIENT:REMOTE_PROV_EXTEND_SCAN_REPORT");
		remote_prov_scan_report_win32 rep_gw_ex;
        rep_gw_ex.unicast = src_adr;
        memcpy(rep_gw_ex.scan_report.uuid,p_report->uuid,sizeof(p_report->uuid));
		rep_gw_ex.scan_report.oob = p_report->OOBinformation;
        //remote_prov_scan_report_cb((u8 *)&rep_gw_ex,sizeof(remote_prov_scan_report_win32));
    }else if (p_event->opcode == REMOTE_PROV_LINK_STS){
        remote_prov_link_status *p_link_sts = (remote_prov_link_status *)p_rp_data;
        p_link_sts = p_link_sts;
        LOG_MSG_INFO(TL_LOG_REMOTE_PROV,(u8 *)p_link_sts,sizeof(remote_prov_link_status),"CLIENT:REMOTE_PROV_LINK_STS");
    }else if (p_event->opcode == REMOTE_PROV_LINK_REPORT){ 
        remote_prov_link_report *p_link_rep = (remote_prov_link_report *)p_rp_data;
        p_link_rep = p_link_rep;
        LOG_MSG_INFO(TL_LOG_REMOTE_PROV,(u8 *)p_link_rep,sizeof(remote_prov_link_report),"CLIENT:REMOTE_PROV_LINK_REPORT");
    }else if(p_event->opcode == REMOTE_PROV_PDU_OUTBOUND_REPORT || REMOTE_PROV_PDU_REPORT){
		mesh_pro_data_t *p_send = (mesh_pro_data_t *)(para_pro);
		mesh_pro_data_t *p_rcv = (mesh_pro_data_t *)p_event->ProvisioningPDU;
		if(p_event->opcode == REMOTE_PROV_PDU_OUTBOUND_REPORT){
			LOG_MSG_INFO(TL_LOG_REMOTE_PROV,&(p_event->InboundPDUNumber),1,"REMOTE_PROV_PDU_REPORT,inbound num is %d",p_event->InboundPDUNumber);
		}else if(p_event->opcode == REMOTE_PROV_PDU_REPORT){
			LOG_MSG_INFO(TL_LOG_REMOTE_PROV,&(p_event->InboundPDUNumber),2,"REMOTE_PROV_PDU_OUTBOUND_REPORT data is ");
		}
        u8 prov_code = p_event->ProvisioningPDU[0];
        if(prov_code == PRO_FAIL && p_event->opcode == REMOTE_PROV_PDU_REPORT){
            // terminate,need to send link close cmd 
            // reset all the status part 
            LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:PRO_FAIL");
            mesh_rp_client_para_reset();
            mesh_cmd_sig_rp_cli_send_link_close(rp_client.node_adr,UNEXPECTED_ERROR);        
        }
        //prov the rp sts ,and the rp opcode part 
        switch(p_rp->prov_sts){
            // rcv capa and send start cmd 
            case RP_PROV_INVITE_CMD_ACK:
				if ((p_event->opcode == REMOTE_PROV_PDU_OUTBOUND_REPORT) && 
					(p_event->InboundPDUNumber == rp_client.outbound) ){
					LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_INVITE_CMD_ACK");
					mesh_rp_client_set_prov_sts(RP_PROV_CAPA_RSP);
					mesh_rp_pdu_retry_clear();
				}
				break;
            case RP_PROV_CAPA_RSP:
                if(p_event->opcode == REMOTE_PROV_PDU_REPORT){
                    // receive the capa cmd 
                    if(prov_code == PRO_CAPABLI){
						#if PROV_AUTH_LEAK_RECREATE_KEY_EN
						init_ecc_key_pair(1);
						#endif
						// init the random part
						generateRandomNum(sizeof(rp_pro_random),rp_pro_random);
						gateway_upload_prov_cmd((u8 *)p_rcv,PRO_CAPABLI);
                        gw_prov_rcv_capa(p_rcv,0);
                        LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_CAPA_RSP");
                        mesh_rp_client_set_prov_sts(RP_PROV_START_CMD);
						gw_prov_send_start(p_rcv,p_send);
						gateway_upload_prov_cmd((u8 *)p_send,PRO_START);
    					mesh_cmd_sig_rp_cli_send_pdu(para_pro,para_len);
						LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_START_CMD");
						mesh_rp_client_set_prov_sts(RP_PROV_START_CMD_ACK);
						break;
                    }else{
                        break;
                    }
                }else{
                    break;
                } 
			case RP_PROV_START_CMD_ACK:
				if(p_event->opcode == REMOTE_PROV_PDU_OUTBOUND_REPORT && 
						p_event->InboundPDUNumber == rp_client.outbound ){
					LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_START_CMD_ACK");
					mesh_rp_pdu_retry_clear();
					mesh_rp_client_set_prov_sts(RP_PROV_PUBKEY_CMD);
					gw_prov_send_pubkey(p_rcv,p_send);
					gateway_upload_prov_cmd((u8 *)p_send,PRO_PUB_KEY);
                    mesh_cmd_sig_rp_cli_send_pdu(para_pro,para_len);
                    LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_PUBKEY_CMD");
					mesh_rp_client_set_prov_sts(RP_PROV_PUBKEY_CMD_ACK);
				}
				break;
            case RP_PROV_PUBKEY_CMD_ACK:
                if((p_event->opcode == REMOTE_PROV_PDU_OUTBOUND_REPORT) && 
					(p_event->InboundPDUNumber == rp_client.outbound) ){
					mesh_rp_pdu_retry_clear();
					LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_PUBKEY_CMD_ACK");
                    mesh_rp_client_set_prov_sts(RP_PROV_PUBKEY_RSP);
                }
                break;
            case RP_PROV_PUBKEY_RSP:
                if(p_event->opcode == REMOTE_PROV_PDU_REPORT){
                    // receive the pubkey from the node 
                    if(prov_code == PRO_PUB_KEY){
						gateway_upload_prov_cmd((u8 *)p_rcv,PRO_PUB_KEY);
                        gw_prov_rcv_pubkey(p_rcv);
                        LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_PUBKEY_RSP");
                        mesh_rp_client_set_prov_sts(RP_PROV_CONFIRM_CMD);
						gw_prov_send_confirm(p_send);
						gateway_upload_prov_cmd((u8 *)p_send,PRO_CONFIRM);
                		mesh_cmd_sig_rp_cli_send_pdu(para_pro,para_len);
                		LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_CONFIRM_CMD");
						mesh_rp_client_set_prov_sts(RP_PROV_CONFIRM_CMD_ACK);
                    }else{
                        break;
                    }
                }else{
                    break;
                }
			case RP_PROV_CONFIRM_CMD_ACK:
				if((p_event->opcode == REMOTE_PROV_PDU_OUTBOUND_REPORT) && 
					(p_event->InboundPDUNumber == rp_client.outbound) ){
					mesh_rp_pdu_retry_clear();
					LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_CONFIRM_CMD_ACK");
                    mesh_rp_client_set_prov_sts(RP_PROV_CONFIRM_RSP);
                }
                break;
            case RP_PROV_CONFIRM_RSP:
                if(p_event->opcode == REMOTE_PROV_PDU_REPORT){
                    // receive the pubkey from the node 
                    if(prov_code == PRO_CONFIRM){
						gateway_upload_prov_cmd((u8 *)p_rcv,PRO_CONFIRM);
                        gw_prov_rcv_confirm(p_rcv,p_send);
                        LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_CONFIRM_RSP");
                        mesh_rp_client_set_prov_sts(RP_PROV_RANDOM_CMD);
						gw_prov_send_random(p_rcv,p_send);
						gateway_upload_prov_cmd((u8 *)p_send,PRO_RANDOM);
                		mesh_cmd_sig_rp_cli_send_pdu(para_pro,para_len);
                		LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_RANDOM_CMD");
						mesh_rp_client_set_prov_sts(RP_PROV_RANDOM_CMD_ACK);
                    }else{
                        break;
                    }
                }else{
                    break;
                }
            case RP_PROV_RANDOM_CMD_ACK:
				if((p_event->opcode == REMOTE_PROV_PDU_OUTBOUND_REPORT) && 
					(p_event->InboundPDUNumber == rp_client.outbound) ){
					mesh_rp_pdu_retry_clear();
					LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_RANDOM_CMD_ACK");
                    mesh_rp_client_set_prov_sts(RP_PROV_RANDOM_RSP);
                }
                break;
            case RP_PROV_RANDOM_RSP:
                if(p_event->opcode == REMOTE_PROV_PDU_REPORT){
                    // receive the pubkey from the node 
                    if(prov_code == PRO_RANDOM){
						pro_uni_adr=pro_dat[23]+(pro_dat[24]<<8);
						gateway_upload_prov_cmd((u8 *)p_rcv,PRO_RANDOM);
                        gw_prov_rcv_random(p_rcv,p_send);
                        LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_RANDOM_RSP,data adr is %d",pro_uni_adr);
                        mesh_rp_client_set_prov_sts(RP_PROV_DATA_CMD);
						gw_prov_send_pro_data(0,p_send);
						LOG_MSG_INFO(TL_LOG_REMOTE_PROV,para_pro,para_len,"p_send is %08x, parapro is %08x",(u32)p_send,(u32)para_pro);
						gateway_upload_prov_cmd((u8 *)p_send,PRO_DATA);
						#if (DONGLE_PROVISION_EN)
							gateway_provision_rsp_cmd(pro_uni_adr);
							VC_node_dev_key_save(pro_uni_adr, pro_dev_key,rp_node_ele_cnt);
						#endif
                		mesh_cmd_sig_rp_cli_send_pdu(para_pro,para_len);
						mesh_rp_client_set_prov_sts(RP_PROV_DATA_CMD_ACK);
                    }else{
                        break;
                    }
                }else{
                    break;
                }
            case RP_PROV_DATA_CMD_ACK:
				if((p_event->opcode == REMOTE_PROV_PDU_OUTBOUND_REPORT) && 
					(p_event->InboundPDUNumber == rp_client.outbound) ){
					mesh_rp_pdu_retry_clear();
					LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_DATA_CMD_ACK");
                    mesh_rp_client_set_prov_sts(RP_PROV_COMPLETE_RSP);
                }
                break;
            case RP_PROV_COMPLETE_RSP:
                if(p_event->opcode == REMOTE_PROV_PDU_REPORT){
                    // receive the pubkey from the node 
                    if(prov_code == PRO_COMPLETE){
                        LOG_MSG_INFO(TL_LOG_GATT_PROVISION,0,0,"RCV:rcv the provision completet cmd,provision success");
						//mesh_prov_dev_candi_store_proc(src_adr);
						gateway_upload_prov_cmd((u8 *)p_rcv,PRO_COMPLETE);
						mesh_cmd_sig_rp_cli_send_link_close(rp_client.node_adr,REMOTE_PROV_LINK_CLOSE_SUC);
                        mesh_rp_client_para_reset();
                        LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_COMPLETE_RSP");
                        cache_init(ADR_ALL_NODES);
						#if GATEWAY_ENABLE
						gateway_upload_node_info(pro_uni_adr);
						// the gateway need the device's mac address and the uuid part 
	        			gateway_upload_provision_suc_event(1,pro_uni_adr,
	                        rp_dev_mac,rp_dev_uuid);// update the suc event
						#endif
						gw_rp_time_clear();
						mesh_rp_pdu_retry_clear();
						
                    }
                }
                break;
            default: break;
               
        } 
    }
	return 1;
}


#endif
#endif
