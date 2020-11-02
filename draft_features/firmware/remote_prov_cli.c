/********************************************************************************************************
 * @file     remote_prov_cli.c
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/

#include "remote_prov.h"
#include "proj/tl_common.h"
#include "user_config.h"
#include "lighting_model.h"
#include "sensors_model.h"
#include "lighting_model_LC.h"
#include "mesh_ota.h"
#include "mesh_common.h"
#include "proj_lib/ble/ll/ll.h"
#include "proj_lib/ble/blt_config.h"
#include "proj_lib/ble/service/ble_ll_ota.h"
#include "app_health.h"
#include "proj_lib/sig_mesh/app_mesh.h"
#include "proj_lib/mesh_crypto/sha256_telink.h"
#include "proj_lib/mesh_crypto/le_crypto.h"
#if WIN32 // remote prov client proc part ,only concern about the gatt provision part 
#include "../../../reference/tl_bulk/lib_file/host_fifo.h"
#include "../../../reference/tl_bulk/lib_file/gatt_provision.h"
#include "../../../reference/tl_bulk/Sig_mesh_json_info.h"
#if MI_API_ENABLE 
#include "vendor/common/mi_api/telink_sdk_mible_api.h"
#endif 
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
	int len ;
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
    LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:SEND_RP_PROV_INVITE_CMD",0);
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
        LOG_MSG_INFO(TL_LOG_REMOTE_PROV,(u8 *)p_scan,sizeof(remote_prov_scan_sts),"CLIENT:REMOTE_PROV_SCAN_STS",0);
    }else if (p_event->opcode == REMOTE_PROV_SCAN_REPORT){
        remote_prov_scan_report *p_report = (remote_prov_scan_report *)p_rp_data;
        LOG_MSG_INFO(TL_LOG_REMOTE_PROV,(u8 *)p_report,sizeof(remote_prov_scan_report),"CLIENT:REMOTE_PROV_SCAN_REPORT",0);
        remote_prov_scan_report_win32 rep_win32;
        rep_win32.unicast = src_adr;
        memcpy((u8 *)&(rep_win32.scan_report),p_report,sizeof(remote_prov_scan_report));
        remote_prov_scan_report_cb((u8 *)&rep_win32,sizeof(remote_prov_scan_report_win32));        
    }else if (p_event->opcode == REMOTE_PROV_EXTEND_SCAN_REPORT){
        remote_prov_extend_scan_report *p_report = (remote_prov_extend_scan_report *)p_rp_data;
        LOG_MSG_INFO(TL_LOG_REMOTE_PROV,(u8 *)p_report,sizeof(remote_prov_extend_scan_report),"CLIENT:REMOTE_PROV_EXTEND_SCAN_REPORT",0);
		remote_prov_scan_report_win32 rep_win32;
        rep_win32.unicast = src_adr;
        memcpy(rep_win32.scan_report.uuid,p_report->uuid,sizeof(p_report->uuid));
		rep_win32.scan_report.oob = p_report->OOBinformation;
        remote_prov_scan_report_cb((u8 *)&rep_win32,sizeof(remote_prov_scan_report_win32));
    }else if (p_event->opcode == REMOTE_PROV_LINK_STS){
        remote_prov_link_status *p_link_sts = (remote_prov_link_status *)p_rp_data;
        LOG_MSG_INFO(TL_LOG_REMOTE_PROV,(u8 *)p_link_sts,sizeof(remote_prov_link_status),"CLIENT:REMOTE_PROV_LINK_STS",0);
    }else if (p_event->opcode == REMOTE_PROV_LINK_REPORT){ 
        remote_prov_link_report *p_link_rep = (remote_prov_link_report *)p_rp_data;
        LOG_MSG_INFO(TL_LOG_REMOTE_PROV,(u8 *)p_link_rep,sizeof(remote_prov_link_report),"CLIENT:REMOTE_PROV_LINK_REPORT",0);
    }else if(p_event->opcode == REMOTE_PROV_PDU_OUTBOUND_REPORT || REMOTE_PROV_PDU_REPORT){
        mesh_pro_data_structer *p_send = (mesh_pro_data_structer *)gatt_para_pro;
        mesh_pro_data_structer *p_rcv = (mesh_pro_data_structer *)p_event->ProvisioningPDU;
		if(p_event->opcode == REMOTE_PROV_PDU_OUTBOUND_REPORT){
			LOG_MSG_INFO(TL_LOG_REMOTE_PROV,&(p_event->InboundPDUNumber),0x31,"REMOTE_PROV_PDU_REPORT,inbound num is %d",p_event->InboundPDUNumber);
		}else if(p_event->opcode == REMOTE_PROV_PDU_REPORT){
			LOG_MSG_INFO(TL_LOG_REMOTE_PROV,&(p_event->InboundPDUNumber),0x31,"REMOTE_PROV_PDU_OUTBOUND_REPORT data is ",0);
		}
        u8 prov_code = p_event->ProvisioningPDU[0];
        if(prov_code == PRO_FAIL && p_event->opcode == REMOTE_PROV_PDU_REPORT){
            // terminate,need to send link close cmd 
            // reset all the status part 
            LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:PRO_FAIL",0);
            mesh_rp_client_para_reset();
            mesh_cmd_sig_rp_cli_send_link_close(rp_client.node_adr,UNEXPECT_ERROR);        
        }
        //prov the rp sts ,and the rp opcode part 
        switch(p_rp->prov_sts){
            // rcv capa and send start cmd 
            case RP_PROV_INVITE_CMD_ACK:
				if ((p_event->opcode == REMOTE_PROV_PDU_OUTBOUND_REPORT) && 
					(p_event->InboundPDUNumber == rp_client.outbound) ){
					LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_INVITE_CMD_ACK",0);
					mesh_rp_client_set_prov_sts(RP_PROV_CAPA_RSP);
				}
				break;
            case RP_PROV_CAPA_RSP:
                if(p_event->opcode == REMOTE_PROV_PDU_REPORT){
                    // receive the capa cmd 
                    if(prov_code == PRO_CAPABLI){
                        gatt_prov_rcv_capa(p_rcv,0);
                        LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_CAPA_RSP",0);
                        mesh_rp_client_set_prov_sts(RP_PROV_START_CMD);
						gatt_prov_send_start(p_rcv,p_send);
    					mesh_cmd_sig_rp_cli_send_pdu(gatt_para_pro,gatt_para_len);  
						LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_START_CMD",0);
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
					LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_START_CMD_ACK",0);
					mesh_rp_client_set_prov_sts(RP_PROV_PUBKEY_CMD);
					gatt_prov_send_pubkey(p_rcv,p_send);
                    mesh_cmd_sig_rp_cli_send_pdu(gatt_para_pro,gatt_para_len);
                    LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_PUBKEY_CMD",0);
					mesh_rp_client_set_prov_sts(RP_PROV_PUBKEY_CMD_ACK);
				}
				break;
            case RP_PROV_PUBKEY_CMD_ACK:
                if((p_event->opcode == REMOTE_PROV_PDU_OUTBOUND_REPORT) && 
					(p_event->InboundPDUNumber == rp_client.outbound) ){
					LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_PUBKEY_CMD_ACK",0);
                    mesh_rp_client_set_prov_sts(RP_PROV_PUBKEY_RSP);
                }
                break;
            case RP_PROV_PUBKEY_RSP:
                if(p_event->opcode == REMOTE_PROV_PDU_REPORT){
                    // receive the pubkey from the node 
                    if(prov_code == PRO_PUB_KEY){
                        gatt_prov_rcv_pubkey(p_rcv);
                        LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_PUBKEY_RSP",0);
                        mesh_rp_client_set_prov_sts(RP_PROV_COMFIRM_CMD);
						gatt_prov_send_comfirm(p_send);
                		mesh_cmd_sig_rp_cli_send_pdu(gatt_para_pro,gatt_para_len);
                		LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_COMFIRM_CMD",0);
						mesh_rp_client_set_prov_sts(RP_PROV_COMFIRM_CMD_ACK);
                    }else{
                        break;
                    }
                }else{
                    break;
                }
			case RP_PROV_COMFIRM_CMD_ACK:
				if((p_event->opcode == REMOTE_PROV_PDU_OUTBOUND_REPORT) && 
					(p_event->InboundPDUNumber == rp_client.outbound) ){
					LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_COMFIRM_CMD_ACK",0);
                    mesh_rp_client_set_prov_sts(RP_PROV_COMFIRM_RSP);
                }
                break;
            case RP_PROV_COMFIRM_RSP:
                if(p_event->opcode == REMOTE_PROV_PDU_REPORT){
                    // receive the pubkey from the node 
                    if(prov_code == PRO_CONFIRM){
                        gatt_prov_rcv_comfirm(p_rcv,p_send);
                        LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_COMFIRM_RSP",0);
                        mesh_rp_client_set_prov_sts(RP_PROV_RANDOM_CMD);
						gatt_prov_send_random(p_rcv,p_send);
                		mesh_cmd_sig_rp_cli_send_pdu(gatt_para_pro,gatt_para_len);
                		LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_RANDOM_CMD",0);
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
					LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_RANDOM_CMD_ACK",0);
                    mesh_rp_client_set_prov_sts(RP_PROV_RANDOM_RSP);
                }
                break;
            case RP_PROV_RANDOM_RSP:
                if(p_event->opcode == REMOTE_PROV_PDU_REPORT){
                    // receive the pubkey from the node 
                    if(prov_code == PRO_RANDOM){
                        gatt_prov_rcv_random(p_rcv,p_send);
                        LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_RANDOM_RSP",0);
                        mesh_rp_client_set_prov_sts(RP_PROV_DATA_CMD);
						gatt_prov_send_pro_data(0,p_send);
                		mesh_cmd_sig_rp_cli_send_pdu(gatt_para_pro,gatt_para_len);
                		LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_DATA_CMD",0);
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
					LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_DATA_CMD_ACK",0);
                    mesh_rp_client_set_prov_sts(RP_PROV_COMPLETE_RSP);
                }
                break;
            case RP_PROV_COMPLETE_RSP:
                if(p_event->opcode == REMOTE_PROV_PDU_REPORT){
                    // receive the pubkey from the node 
                    if(prov_code == PRO_COMPLETE){
                        prov_timer_clr();
                        gatt_prov_rcv_pro_complete();
						mesh_prov_dev_candi_store_proc(src_adr);
                        provision_end_callback(PROV_NORMAL_RET);
                        gatt_provision_mag.provison_send_state = STATE_PRO_SUC;
						mesh_cmd_sig_rp_cli_send_link_close(rp_client.node_adr,REMOTE_PROV_LINK_CLOSE_SUC);
                        mesh_rp_client_para_reset();
                        LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_PROV_COMPLETE_RSP",0);
                        cache_init(0xffff);
						
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

void mesh_prov_dev_candi_store_proc(u16 cmd_src)
{
	LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"mesh_prov_dev_candi_store_proc ",0);
	if(mesh_prov_dkri_is_valid()){
		rp_mag.dkri_cli &= ~REMOTE_PROV_DKRI_EN_FLAG;
		LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"mesh_prov_dkri_is_valid ",0);
	}
	if(rp_mag.dkri_cli == RP_DKRI_DEV_KEY_REFRESH){
		LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"RP_DKRI_DEV_KEY_REFRESH proc",0);
		VC_node_dev_key_save_candi(rp_mag.adr_src,rp_mag.dev_candi);		
	}else if(rp_mag.dkri_cli == RP_DKRI_NODE_ADR_REFRESH){
		VC_node_replace_devkey_candi_adr(cmd_src,rp_mag.adr_src,rp_mag.dev_candi);// update the candi part 
		//update the json file part .
		json_update_node_adr_devicekey(rp_mag.adr_src,cmd_src,rp_mag.dev_candi);
	}else if (rp_mag.dkri_cli == RP_DKRI_NODE_CPS_REFRESH){
		VC_node_dev_key_save_candi(rp_mag.adr_src,rp_mag.dev_candi);
	}
}


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
				mesh_rp_pdu_retry_clear();
			}
        }
    }
}
void mesh_rp_pdu_retry_clear()
{
    rp_mag_cli_str *p_rp = &rp_client;
    p_rp->retry_flag =0;
    p_rp->tick =0;
}
#endif

