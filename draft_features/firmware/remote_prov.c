/********************************************************************************************************
 * @file     remote_prov.c 
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
#ifndef WIN32
#include "proj/mcu/watchdog_i.h"
#endif 
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
#include "proj/common/tstring.h"

#if MI_API_ENABLE 
#include "vendor/common/mi_api/telink_sdk_mible_api.h"
#endif 
#define REMOTE_PROV_PTS_EN	0
#if MD_REMOTE_PROV
model_remote_prov_t model_remote_prov;
u32 mesh_md_rp_addr = FLASH_ADR_MD_REMOTE_PROV;
rp_mag_str rp_mag;
u8 node_devkey_candi[16];
provison_net_info_str node_adr_net_info;

void mesh_rp_para_init()
{
    memset(&rp_mag,0,sizeof(rp_mag));
}

int mesh_rp_cmd_st_rsp(u16 op_code,u8 *p_rsp,u8 len ,mesh_cb_fun_par_t *cb_par)
{
    model_rp_server_common_t *p_model = (model_rp_server_common_t *)cb_par->model;
    return mesh_tx_cmd_rsp(op_code,p_rsp,len,p_model->com.ele_adr,cb_par->adr_src,0,0);
}

int mesh_cmd_sig_rp_scan_capa(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	remote_prov_scan_cap_sts cap_sts;
	cap_sts.activeScan = ACTIVE_SCAN_ENABLE;
	cap_sts.maxScannedItems = MAX_SCAN_ITEMS_UUID_CNT;
	return mesh_rp_cmd_st_rsp(REMOTE_PROV_SCAN_CAPA_STS,(u8 *)(&cap_sts),sizeof(cap_sts),cb_par);
}

int mesh_cmd_sig_rp_scan_capa_sts(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	return err;
}

int mesh_cmd_sig_rp_scan_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    return mesh_cmd_sig_rp_scan_sts(par,par_len,cb_par);
}

int mesh_cmd_sig_get_rp_scan_cnt(int par_len,remote_prov_scan_start *p_scan_start)
{
	if(par_len == sizeof(remote_prov_scan_start)){// it have the uuid part 
		return 1;
	}else{
		return p_scan_start->scannedItemsLimit;
	}
}
u16 mesh_cmd_get_netkey_idx()
{
	mesh_net_key_t *p_net = &mesh_key.net_key[mesh_key.netkey_sel_dec][mesh_key.new_netkey_dec];
	return p_net->index;
}
	
int mesh_cmd_sig_rp_scan_start(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err =-1;
    remote_prov_scan_start *p_scan_start = (remote_prov_scan_start *)par;
    remote_prov_scan_sts_str *p_scan_sts = &(rp_mag.rp_scan_sts);
	if(p_scan_sts->PRScanningState!=STS_PR_SCAN_IDLE && 
		(cb_par->adr_src != p_scan_sts->src || rp_mag.netkey_idx != mesh_cmd_get_netkey_idx())){
		// rsp unvailable 
		// the data is invalid ,so we should use tmp str
		remote_prov_scan_sts_str scan_temp;
		p_scan_sts = &scan_temp;
		p_scan_sts->status = REMOTE_PROV_INVALID_STS;
		p_scan_sts->PRScanningState = STS_PR_SCAN_MULTI_NODES;
		p_scan_sts->scannedItemsLimit = p_scan_start->scannedItemsLimit;
		p_scan_sts->timeout =0;
		return mesh_rp_cmd_st_rsp(REMOTE_PROV_SCAN_STS,(u8 *)(p_scan_sts),4,cb_par);
	}else{
		remote_prov_scan_en(1);
		rp_mag.netkey_idx = mesh_cmd_get_netkey_idx();
		int limit_cnt = mesh_cmd_sig_get_rp_scan_cnt(par_len,p_scan_start);
		if( limit_cnt > MAX_SCAN_ITEMS_UUID_CNT ){
	        p_scan_sts->status = REMOTE_PROV_SCAN_NOT_START;
	        p_scan_sts->PRScanningState = STS_PR_SCAN_IDLE;
	        p_scan_sts->scannedItemsLimit = MAX_SCAN_ITEMS_UUID_CNT;
	        p_scan_sts->timeout =0;
	    }else if (limit_cnt == 0 ){
			p_scan_sts->status = REMOTE_PROV_STS_SUC;
			p_scan_sts->PRScanningState = STS_PR_SCAN_MULTI_NODES;
	    	p_scan_sts->timeout = p_scan_start->timeout;
			p_scan_sts->scannedItemsLimit = MAX_SCAN_ITEMS_UUID_CNT;
	    	p_scan_sts->tick = clock_time();
	    	p_scan_sts->src = cb_par->adr_src;
		}else{
			if(par_len == 2){// on uuid mode 
	        	p_scan_sts->PRScanningState = STS_PR_SCAN_MULTI_NODES;
				p_scan_sts->scannedItemsLimit = p_scan_start->scannedItemsLimit;
		    }else if (par_len == 18){// with uuid mode 
		        p_scan_sts->PRScanningState = STS_PR_SCAN_SINGLE_NODE;
		        // need to store the uuid part 
		        memcpy(p_scan_sts->scan_start_uuid,p_scan_start->uuid,16);
				p_scan_sts->scannedItemsLimit = 1;
		    }else{
		        return err;
		    }
			p_scan_sts->status = REMOTE_PROV_STS_SUC;
	    	p_scan_sts->timeout = p_scan_start->timeout;
	    	p_scan_sts->tick = clock_time();
	    	p_scan_sts->src = cb_par->adr_src;
		}
	}
    // need to clean the str of the uuid part .
    rp_scan_report_str *p_rp_report =(rp_mag.rp_rep);
    memset(p_rp_report,0,sizeof(rp_scan_report_str)*MAX_SCAN_ITEMS_UUID_CNT);
    // rsp the scan sts part 
    return mesh_cmd_sig_rp_scan_sts(par,par_len,cb_par);
}

void mesh_cmd_sig_rsp_scan_init()
{
    remote_prov_scan_sts_str *p_scan_sts = &(rp_mag.rp_scan_sts);
    rp_scan_report_str *p_rep = (rp_mag.rp_rep);
    memset((u8*)p_scan_sts,0,sizeof(remote_prov_scan_sts_str));
    memset((u8*)p_rep,0,sizeof(rp_scan_report_str)*MAX_SCAN_ITEMS_UUID_CNT);
    return ;
}

int mesh_cmd_sig_rp_scan_stop(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	remote_prov_scan_en(0);
	mesh_cmd_sig_rsp_scan_init();
    err = mesh_cmd_sig_rp_scan_sts(par,par_len,cb_par);
	return err;
}

int mesh_cmd_sig_rp_scan_sts(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    remote_prov_scan_sts_str *p_scan = &(rp_mag.rp_scan_sts);
    return mesh_rp_cmd_st_rsp(REMOTE_PROV_SCAN_STS,(u8 *)(p_scan),4,cb_par);
}
u8 remote_prov_get_scan_report_cnt()
{
    u8 cnt=0;
     for(int i=0;i<MAX_SCAN_ITEMS_UUID_CNT;i++){
        rp_scan_report_str *p_rp_report =&(rp_mag.rp_rep[i]);
        if(p_rp_report->valid){
            cnt++;
        }
    }
    return cnt;
}

int remote_prov_find_report_idx(u8 *p_uuid)
{
    int i;
    for(i=0;i<MAX_SCAN_ITEMS_UUID_CNT;i++){
        rp_scan_report_str *p_rp_report =&(rp_mag.rp_rep[i]);
        if(p_rp_report->valid){
            if(!memcmp(p_rp_report->uuid,p_uuid,16)){
                break;
            }
        }
    }
	// it means the uuid can be reported .
    if(i==4){
        // find the first empty str 
        for(int j=0;j<MAX_SCAN_ITEMS_UUID_CNT;j++){
            rp_scan_report_str *p_rp_report =&(rp_mag.rp_rep[j]);
            if(!p_rp_report->valid){
                return j;
            }
        }
    }
    return -1;
}
void remote_prov_scan_en(u8 en)
{
	rp_mag.rp_scan_en = en;
}

u8 get_remote_prov_scan_sts()
{
	return rp_mag.rp_scan_en;
}

void remote_prov_add_report_info(u8 rssi,u8 *p_uuid,u8 * p_oob)
{
	int idx = remote_prov_find_report_idx(p_uuid);
	if(idx>=0){
		rp_scan_report_str *p_rp_report =&(rp_mag.rp_rep[idx]);
		p_rp_report->send_flag =1;
    	p_rp_report->valid = 1;
    	p_rp_report->rssi = rssi;
    	memcpy(p_rp_report->uuid,p_uuid,16);
    	memcpy(p_rp_report->oob,p_oob,2);
	}
}


int remote_prov_report_cb(u8 rssi,u8 *p_uuid,u8 * p_oob)
{
    remote_prov_scan_sts_str *p_scan = &(rp_mag.rp_scan_sts);
	if(get_remote_prov_scan_sts() == 0){
		return 0;
	}
    if( p_scan->PRScanningState == STS_PR_SCAN_MULTI_NODES){
        // add the report information into the info part 
        remote_prov_add_report_info(rssi,p_uuid,p_oob);	
        // get the remote prov report cnt
        u8 rep_cnt = remote_prov_get_scan_report_cnt();
        if(rep_cnt == p_scan->scannedItemsLimit){
              // stop scan 
              remote_prov_scan_en(0);
        }
    }else if (p_scan->PRScanningState == STS_PR_SCAN_SINGLE_NODE){
        // only add one report information into the str part 
        if(!memcmp(p_scan->scan_start_uuid,p_uuid,16)){
            // find the dst uuid beacon ,need to terminate the scan part 
            remote_prov_add_report_info(rssi,p_uuid,p_oob);
            // terminate the scan part 
            remote_prov_scan_en(0);
        }
    }
    return 1;
}

int remote_prov_report_raw_pkt_cb(u8 *p_beacon)
{
    beacon_data_pk *p_adv = (beacon_data_pk *)p_beacon;
    u8 len  = p_adv->header.len;
    u8 rssi = p_beacon[len+1];
    remote_prov_report_cb(rssi,p_adv->device_uuid,p_adv->oob_info);
    return 1;
}


int mesh_cmd_sig_rp_scan_report(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    // just need to enable to send the scan report data
    int err =0;
    return err;
}

int mesh_cmd_get_report_cnt()
{
	int cnt =0;
	for(int i=0;i<MAX_SCAN_ITEMS_UUID_CNT;i++){
		rp_scan_report_str *p_report = &rp_mag.rp_rep[i];
		if(p_report->valid && p_report->send_flag){
			cnt++;
		}
	}
	return cnt;
}

int mesh_cmd_send_scan_report()
{
	remote_prov_scan_sts_str *p_scan_sts = &(rp_mag.rp_scan_sts);
	for(int i=0;i<MAX_SCAN_ITEMS_UUID_CNT;i++){
		rp_scan_report_str *p_report = &rp_mag.rp_rep[i];
		if(p_report->valid && p_report->send_flag){
			if(is_busy_tx_segment_or_reliable_flow()){
				return -1;
			}
			int err = mesh_tx_cmd_rsp(REMOTE_PROV_SCAN_REPORT,&(p_report->rssi),sizeof(rp_scan_report_str)-2,
                      ele_adr_primary,p_scan_sts->src,0,0); 
			if(err == 0){
				p_report->send_flag = 0;// only send once for differnt uuid report .
			}
		}
	}
	return 0;
}


int mesh_cmd_sig_rp_scan_proc()
{
    remote_prov_scan_sts_str *p_scan_sts = &(rp_mag.rp_scan_sts);
    if((clock_time_exceed(p_scan_sts->tick,p_scan_sts->timeout*1000*1000)&& p_scan_sts->timeout)){
        // timeout proc part 
        remote_prov_scan_en(0);
		mesh_cmd_sig_rsp_scan_init();
		p_scan_sts->tick =0;
		p_scan_sts->timeout = 0;
    }else{
		// sending proc part 
		if(mesh_cmd_get_report_cnt()){// still have the report to send .
			mesh_cmd_send_scan_report();
		}
	}
    return 1;
}

int mesh_ad_type_is_valid(u8 ad_type)
{
	if((ad_type>=AD_TYPE_FLAGS && ad_type <= AD_TYPE_TK_VALUE) ||(ad_type == AD_TYPE_URI)){
		if(	ad_type != AD_TYPE_SHORT_LOCAL &&
			ad_type != AD_TYPE_16BITS_UUID &&
			ad_type != AD_TYPE_32BITS_UUID &&
			ad_type != AD_TYPE_128BITS_UUID){
			return 1;
		}
	}
	return 0;
}

int mesh_extend_scan_timeout_is_valid(u8 time_s)
{
	if(time_s>=0&&time_s<=5){
		return 1;
	}else{
		return 0;
	}

}

int extend_ad_type_is_valid(u8 *p_type,u8 cnt)
{
	for(int i=0;i<cnt;i++){
		if(!mesh_ad_type_is_valid(p_type[i])){
			return 0;
		}
	}
	return 1;
}

int mesh_filter_have_uri_ad_types(u8 *p_filter,u8 len)
{
	for(int i=0;i<len;i++){
		if(p_filter[i] == AD_TYPE_URI){
			return 1;
		}
	}
	return 0;
}

int mesh_analyze_filter_proc(u8 *p_filter,u8 len)
{
	if(len==0){
		return 0;
	}
	int uri_ad_flag = mesh_filter_have_uri_ad_types(p_filter,len);
	if(uri_ad_flag){
		return (len==1)?EXTEND_END_WITH_ONLY_URI_AD:EXTEND_END_WITH_MULTI_URI_AD;
	}else{
		return EXTEND_END_WITHOUT_URI_AD;
	}
}

int mesh_proc_filter_part(remote_prov_extend_scan_str *p_scan,remote_prov_extend_scan_start *p_extend_scan)
{
	if(p_extend_scan->ADTypeFilterCount == 0){
		return 0;
	}
	p_scan->ADTypeFilterCount = p_extend_scan->ADTypeFilterCount;
	memcpy(p_scan->ADTypeFilter,p_extend_scan->ADTypeFilter,p_scan->ADTypeFilterCount);
	p_scan->end_flag |= EXTEND_END_ALL_AD;
	return 1;
}

int mesh_tx_extend_scan_report(remote_prov_extend_scan_str *p_scan)
{
	int err = -1;
	u8 rsp_len =0;
	rp_extend_scan_report_str *p_report = &p_scan->report;
	if(is_buf_zero(p_report->uuid,sizeof(p_report->uuid))){
		return 1;
	}
	// we will force to send the oob info part 
	p_report->status = REMOTE_PROV_STS_SUC;
	rsp_len = 19+p_report->adv_str_len;
	err = mesh_tx_cmd_rsp(REMOTE_PROV_EXTEND_SCAN_REPORT,(u8 *)p_report,rsp_len,
                      ele_adr_primary,p_scan->src_adr,0,0); 
	if(err == 0){
		return 1;
	}else{
		return 0;
	}
}

void mesh_extend_set_scan_str(remote_prov_extend_scan_str *p_scan,event_adv_report_t *report)
{
	beacon_data_pk *p_beacon = (beacon_data_pk *)(report->data);
	// cpy the uuid info directly to the report uuid part 
	memcpy(p_scan->report.uuid,p_beacon->device_uuid,16);
	memcpy(p_scan->report.oob_info,p_beacon->oob_info,2);
	// set the unprov's node mac adr ,and then it can operate by the mac adr.
	memcpy(p_scan->mac_adr,report->mac,6);
	p_scan->active_scan =1;
}

int mesh_extend_unprov_beacon_cb(event_adv_report_t *report)
{
	beacon_data_pk *p_beacon = (beacon_data_pk *)(report->data);
	for(int i=0;i<MAX_EXTEND_SCAN_CNT;i++){
		remote_prov_extend_scan_str *p_scan = &(rp_mag.rp_extend[i]);
		if(p_scan->end_flag && is_buf_zero(p_scan->report.uuid,16)){
			if(!is_buf_zero(p_scan->uuid,sizeof(p_scan->uuid))){// should have const uuid.
				if(!memcmp(p_scan->uuid,p_beacon->device_uuid,16)){
					//only the unprov beacon's uuid is the same as the scan's uuid ,we can operate
					mesh_extend_set_scan_str(p_scan,report);
				}
			}else{
				// cpy the uuid info directly to the report uuid part 
				mesh_extend_set_scan_str(p_scan,report);	
				//LOG_MSG_INFO(TL_LOG_REMOTE_PROV,p_scan->mac_adr,6,"set extend unprov para",0);
			}
		}
	}
	return 1;
}

typedef struct{
	u8 len;
	u8 type;
	u8 data[1];
}ad_type_str;

u8 mesh_extend_filter_is_valid(remote_prov_extend_scan_str *p_scan,u8 ad_type)
{
	for(int i=0;i<p_scan->ADTypeFilterCount;i++){
		if(ad_type == p_scan->ADTypeFilter[i]){
			return 1;
		}
	}
	return 0;
}

u8 mesh_extend_filter_clear(remote_prov_extend_scan_str *p_scan,u8 ad_type)
{
	for(int i=0;i<p_scan->ADTypeFilterCount;i++){
		if(ad_type == p_scan->ADTypeFilter[i]){
			p_scan->ADTypeFilter[i] = 0;
			return 1;
		}
	}
	return 0;
}

u8 mesh_extend_filter_set_report_str(remote_prov_extend_scan_str *p_scan,event_adv_report_t *report,u8 uri_flag)
{
	int len =report->len;
	int idx=0;
	rp_extend_scan_report_str *p_report = &(p_scan->report);
	while(idx<=len){
		ad_type_str *p_adv = (ad_type_str *)&(report->data[idx]);
		if(!is_buf_zero(p_scan->ADTypeFilter,sizeof(p_scan->ADTypeFilter))&&
			mesh_extend_filter_is_valid(p_scan,p_adv->type)){
			u8 len = p_report->adv_str_len;
			u8 *buf = &(p_report->adv_str[len]);
			if((len+p_adv->len+1) <= MAX_EXTEND_ADV_LEN){
				memcpy(buf,(u8*)p_adv,p_adv->len+1);// cpy all the data.
				p_report->adv_str_len += p_adv->len+1;
				mesh_extend_filter_clear(p_scan,p_adv->type);
				//LOG_MSG_INFO(TL_LOG_REMOTE_PROV,p_scan->ADTypeFilter,8,"the adv type is %d",p_report->adv_str_len);
				if(uri_flag && p_adv->type == AD_TYPE_URI){
					return 1;
				}
			}else{
				return 0;
			}
		}
		idx+=p_adv->len+1;
	}
	return 0;
}

u8 mesh_tx_send_extend_scan_report(remote_prov_extend_scan_str *p_scan)
{
	int err =-1;
	rp_extend_scan_report_str *p_report = &(p_scan->report);
	//LOG_MSG_INFO(TL_LOG_REMOTE_PROV,(u8 *)p_report,19+p_report->adv_str_len,"mesh_tx_send_extend_scan_report",0);
	err = mesh_tx_cmd_rsp(REMOTE_PROV_EXTEND_SCAN_REPORT,(u8 *)p_report,19+p_report->adv_str_len,
                      ele_adr_primary,p_scan->src_adr,0,0); 
	if(err == 0){
		//clear the scan procedure 
		memset(p_scan,0,sizeof(remote_prov_extend_scan_str));
		return 1;
	}
	return 0;
}

u8 mesh_exend_filter_check_without_uri_ad(remote_prov_extend_scan_str *p_scan,event_adv_report_t *report)
{
	if(p_scan->end_flag & EXTEND_END_WITHOUT_URI_AD){
		if(mesh_extend_filter_set_report_str(p_scan,report,0)){
			return mesh_tx_send_extend_scan_report(p_scan);
		}
	}
	return 0;
}

u8 mesh_exend_filter_check_with_uri_ad(remote_prov_extend_scan_str *p_scan,event_adv_report_t *report)
{
	if( p_scan->end_flag & EXTEND_END_WITH_ONLY_URI_AD ||
		p_scan->end_flag & EXTEND_END_WITH_MULTI_URI_AD ){
		LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"mesh_exend_filter_check_with_uri_ad ",0);
		//proc the extend scan report,whether it have the uri adtype packet and proc 
		if(mesh_extend_filter_set_report_str(p_scan,report,1)){
			return mesh_tx_send_extend_scan_report(p_scan);
		}
	}
	return 0;
}

u8 mesh_extend_filter_check_all_ad_type(remote_prov_extend_scan_str *p_scan,event_adv_report_t *report)
{
	if(p_scan->end_flag&EXTEND_END_ALL_AD){
		// proc all the other cmd .
		//LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"mesh_extend_filter_check_all_ad_type",0);
		mesh_extend_filter_set_report_str(p_scan,report,0);
		// check the adv filter is all proc success or not .
		if(is_buf_zero(p_scan->ADTypeFilter,sizeof(p_scan->ADTypeFilter))){
			return mesh_tx_send_extend_scan_report(p_scan);
		}
	}
	return 0;
}


int mesh_extend_adv_filter_proc(remote_prov_extend_scan_str *p_scan,event_adv_report_t *report)
{
	if((report->event_type&0x0f) == LL_TYPE_SCAN_RSP){
		//LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"mesh rcv scan rsp cmd ",0);
		if(mesh_exend_filter_check_without_uri_ad(p_scan,report)){
			return 1;
		}
	}
	if(mesh_exend_filter_check_with_uri_ad(p_scan,report)){
		return 1;
	}
	mesh_extend_filter_check_all_ad_type(p_scan,report);
	return 1;
}

int mesh_extend_adv_cb(event_adv_report_t *report)
{
	for(int i=0;i<MAX_EXTEND_SCAN_CNT;i++){
		remote_prov_extend_scan_str *p_scan = &(rp_mag.rp_extend[i]);
		// directly compare the mac adr part ,and then set the adv filter part 
		if(!memcmp(p_scan->mac_adr,report->mac,6)){
			mesh_extend_adv_filter_proc(p_scan,report);
		}
	}
	return 1;
}

u8  mesh_cmd_is_unprovision_beacon(event_adv_report_t *report)
{
	if((report->event_type&0x0f) == LL_TYPE_ADV_NONCONN_IND && 
		report->data[1]== MESH_ADV_TYPE_BEACON &&
		report->data[2]== UNPROVISION_BEACON){
		return 1;
	}else{
		return 0;
	}
}

u8  mesh_cmd_is_extend_proc_adv(event_adv_report_t *report)
{
	if((report->event_type&0x0f) == LL_TYPE_ADV_IND || 
		(report->event_type&0x0f) == LL_TYPE_SCAN_RSP){
		return 1;
	}else{
		return 0;
	}
}

int mesh_cmd_extend_loop_cb(event_adv_report_t *report)
{
	// we should suppose when start a extend scan ,should first receive an unprovision beacon ,and then rcv the adv part
	if(mesh_extend_scan_proc_is_valid() == 0){
		return 0;
	}
	if(mesh_cmd_is_unprovision_beacon(report)){
		mesh_extend_unprov_beacon_cb(report);
	}else if(mesh_cmd_is_extend_proc_adv(report)){
		mesh_extend_adv_cb(report);
	}
	return 1;
}	

_attribute_ram_code_ u8 conn_adv_type_is_valid_in_extend(u8* p_adv)
{
#if !WIN32
	if(mesh_extend_scan_proc_is_valid() == 0){
		return 0;
	}
	for(int i=0;i<MAX_EXTEND_SCAN_CNT;i++){
		remote_prov_extend_scan_str *p_scan = &(rp_mag.rp_extend[i]);
		if(!memcmp(p_scan->mac_adr,p_adv,6)){
			if(p_scan->active_scan){
				memcpy(pkt_scan_req.scanA,tbl_mac,6);
				memcpy(pkt_scan_req.advA,p_scan->mac_adr,6);
				rf_start_stx2rx ((void *)&pkt_scan_req, clock_time() + 100);
			}
			return 1;
		}
	}
#endif
	return 0;
}

void mesh_cmd_extend_timeout_proc()
{
	if(mesh_extend_scan_proc_is_valid() == 0){
		return ;
	}
	for(int i=0;i<MAX_EXTEND_SCAN_CNT;i++){
		remote_prov_extend_scan_str *p_scan = &(rp_mag.rp_extend[i]);
		if(!is_buf_zero(p_scan,sizeof(remote_prov_extend_scan_str))){// means data is valid 
			if((p_scan->end_flag & EXTEND_END_TIMEOUT )&& 
				clock_time_exceed_s(p_scan->tick_s,p_scan->time_s)){
				// need to report the extend scan report 
				if(mesh_tx_extend_scan_report(p_scan)){
					memset(p_scan,0,sizeof(remote_prov_extend_scan_str));//clear the info if finish ,else it will retry
				}
			}
		}
	}
}

remote_prov_extend_scan_str * mesh_find_empty_extend_scan()
{
	for(int i=0;i<MAX_EXTEND_SCAN_CNT;i++){
		remote_prov_extend_scan_str *p_scan = &(rp_mag.rp_extend[i]);
		if(is_buf_zero(p_scan,sizeof(remote_prov_extend_scan_str))){
			return p_scan;
		}
	}
	return 0;
}

u8 mesh_extend_scan_proc_is_valid()
{
	for(int i=0;i<MAX_EXTEND_SCAN_CNT;i++){
		remote_prov_extend_scan_str *p_scan = &(rp_mag.rp_extend[i]);
		if(p_scan->end_flag != 0){
			return 1;
		}
	}
	return 0;
}

// extend cmd will reserve to dispatch ??
int mesh_cmd_sig_rp_extend_scan_start(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    remote_prov_extend_scan_start *p_extend_scan = (remote_prov_extend_scan_start *)par;
    remote_prov_extend_scan_str *p_scan = mesh_find_empty_extend_scan();
	if(p_scan == 0){
		return -10;
	}
	// first find a empty extend buffer first.
	memset(p_scan,0,sizeof(remote_prov_extend_scan_str));
	if(p_extend_scan->ADTypeFilterCount == 0 || p_extend_scan->ADTypeFilterCount > MAX_ADTYPE_FILTER_CNT){
		//LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"extend filter cnt is invalid ",0);
		return -1;
	}
	if(!extend_ad_type_is_valid(p_extend_scan->ADTypeFilter,p_extend_scan->ADTypeFilterCount)){
		//LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"extend filter value is invalid ",0);
		return -2;
	}
	if(!mesh_proc_filter_part(p_scan,p_extend_scan)){
		
		return -3;
	}
	p_scan->nid = cb_par->p_nw->nid;
	p_scan->src_adr = cb_par->adr_src;
	p_scan->end_flag |= mesh_analyze_filter_proc(p_extend_scan->ADTypeFilter,p_scan->ADTypeFilterCount);
	//LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"extend filter adr is %d,end flag is %d",p_scan->src_adr,p_scan->end_flag);
	if(par_len == p_extend_scan->ADTypeFilterCount+1){// means only have the filter and ad types part 
		
	}else if(par_len == p_extend_scan->ADTypeFilterCount+18){// suppose the uuid and time at the same time 
		u8 *p_uuid = par + 1 + p_extend_scan->ADTypeFilterCount;
		u8 time_s = p_uuid[16];
		memcpy(p_scan->uuid,p_uuid,16);
		if(!mesh_extend_scan_timeout_is_valid(time_s)){
			return -4;//timeout is invalid 
		}
		p_scan->end_flag |= EXTEND_END_TIMEOUT;
		p_scan->time_s = time_s;
		p_scan->tick_s = clock_time_s();
	}else{
		return -5; // the length is invalid .
	}
    return 0;
}

int mesh_cmd_sig_rp_extend_scan_report(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err =-1;
    return err;
}
int mesh_cmd_sig_rp_send_link_sts(mesh_cb_fun_par_t *cb_par)
{
    remote_prov_link_sts_str *p_link_sts = &(rp_mag.rp_link);
    return mesh_rp_cmd_st_rsp(REMOTE_PROV_LINK_STS,(u8 *)(p_link_sts),2,cb_par);

}

int mesh_cmd_sig_rp_send_link_open_sts()
{
    remote_prov_link_sts_str *p_link_sts = &(rp_mag.rp_link);
    return mesh_tx_cmd_rsp(REMOTE_PROV_LINK_STS,(u8 *)(p_link_sts),2,ele_adr_primary,rp_mag.link_adr,0,0);
}


int mesh_cmd_sig_rp_link_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    return mesh_cmd_sig_rp_send_link_sts(cb_par);
}
void mesh_rp_server_set_sts(u8 sts)
{
    remote_proc_pdu_sts_str *p_rp_pdu = &(rp_mag.rp_pdu);
    p_rp_pdu->sts = sts;
}
void mesh_rp_server_pdu_reset()
{
    remote_proc_pdu_sts_str *p_rp_pdu = &(rp_mag.rp_pdu);
    memset(p_rp_pdu,0,sizeof(remote_proc_pdu_sts_str));
}

void mesh_rp_server_set_link_sts(u8 sts)
{
    remote_prov_link_sts_str *p_link_sts = &(rp_mag.rp_link);
    p_link_sts->RPState = sts;    
}

void mesh_rp_dkri_end_cb()
{
	u8 dkri = prov_para.dkri;
// only process the remote prov dkri success it will proc this cmd .
	if(dkri & REMOTE_PROV_DKRI_EN_FLAG){
		//LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"mesh_rp_dkri_end_cb dkri is valid ",0);
		dkri &=(~REMOTE_PROV_DKRI_EN_FLAG);
		if(dkri == RP_DKRI_DEV_KEY_REFRESH){
			//LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"RP_DKRI_DEV_KEY_REFRESH is valid ",0);
		}else if(dkri == RP_DKRI_NODE_ADR_REFRESH){
			//Set SEQ to its lowest initial value.
			mesh_adv_tx_cmd_sno = 0;
			mesh_misc_store();
			// Cancel all transmission and reception of segmented messages.
			mesh_tx_segment_finished();
			memset(&mesh_rx_seg_par, 0, sizeof(mesh_rx_seg_par));
			//Delete all entries in the message replay protection mechanism
			mesh_network_cache_buf_init();
			//Set all states to the default values when the state default value is specified by the profile specification that defines the state.
			
			//Terminate all active friendships, if applicable.
			#if FEATURE_FRIEND_EN
			mesh_friend_ship_init_all();
			#endif
			#if FEATURE_LOWPOWER_EN
			mesh_friend_ship_clear_LPN();
			mesh_friend_ship_set_st_lpn(FRI_ST_REQUEST);    // restart establish procedure
			#endif
			//Copy Composition Data Page 128 to Composition Data Page 0, ,perhaps no page 128
			mesh_cps_data_update_page0_from_page128();
			// if the node adr ,it should directly update the devkey part 
			memcpy(mesh_key.dev_key,node_devkey_candi,16);
			memset(node_devkey_candi,0,16);// clear the devkey candi.
			// use the provision mag's adr to update the others. have already proc by the mesh_rsp_handle
			memcpy(&provision_mag.pro_net_info,&node_adr_net_info,sizeof(provison_net_info_str));
			// add the info about the gatt mode provision ,should set the cfg data part into the node identity
			mesh_provision_par_handle((u8 *)&provision_mag.pro_net_info);
			}else if (dkri == RP_DKRI_NODE_CPS_REFRESH){
			mesh_cps_data_update_page0_from_page128();
		}
		prov_para.dkri = 0; //clear the status .
	}else{
		return ;
	}
}

void mesh_rp_server_prov_end_cb()
{
	mesh_rp_dkri_end_cb();
    mesh_rp_server_set_link_sts(STS_PR_LINK_IDLE);
	mesh_rp_server_pdu_reset();
}

u8 mesh_remote_prov_link_timeout_is_valid(u8 time_s)
{
	if(time_s>=1 && time_s<=0x3c){
		return 1;
	}else{
		return 0;
	}
}

u8 mesh_rp_link_dkri_is_valid(u8 dkri)
{
	return (dkri <= RP_DKRI_NODE_CPS_REFRESH)?1:0;
}

void mesh_rp_link_open_bound_proc()
{
	remote_proc_pdu_sts_str *p_rp_pdu = &(rp_mag.rp_pdu);
	p_rp_pdu->inbound =0;
	p_rp_pdu->outbound =0;
}


int mesh_cmd_sig_rp_link_open(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    // trigger to send the link open cmd
    remote_prov_link_open *p_link_open = (remote_prov_link_open *)par;
    if(par_len>sizeof(remote_prov_link_open)){
        return -1;
    }
    // link sts proc 
    if(rp_mag.rp_link.RPState == RP_SER_IDLE){
		rp_mag.rp_now_s = clock_time_s();
	    // not use the provision state ,just use the bearer part 
		if(par_len >= sizeof(remote_prov_link_open)-1){// remote prov procedure 
			mesh_rp_link_open_bound_proc();
			if(par_len == sizeof(remote_prov_link_open)){
				if(mesh_remote_prov_link_timeout_is_valid(p_link_open->timeout)){
					rp_mag.link_timeout = p_link_open->timeout;
				}else{
					rp_mag.link_timeout = 10;// init is 10s	
				}
			}else{
				rp_mag.link_timeout = 10;// init is 10s
			}
			rp_mag.link_dkri = RP_DKRI_RFU;// disable the rp dkri part .
			mesh_adv_prov_link_open(p_link_open->uuid);
	    	rp_mag.link_adr = cb_par->adr_src;
	    	mesh_rp_server_set_sts(RP_SER_LINK_OPEN_ACK);
			rp_mag.rp_scan_en = 0;// disable sending part 
			mesh_cmd_sig_rsp_scan_init();// if we receive link open ,we should stop the rp scan part 
		}else{
			if(mesh_rp_link_dkri_is_valid(p_link_open->dkri)){
				mesh_rp_link_open_bound_proc();
				rp_mag.link_dkri = p_link_open->dkri;
				rp_mag.rp_link.status = REMOTE_PROV_STS_SUC;
				mesh_rp_server_set_sts(RP_SER_INVITE_SEND); // wait for the invite cmd proc ,not send the link open cmd .
			}else{
				rp_mag.rp_link.status = REMOTE_PROV_INVALID_STS;
				rp_mag.rp_link.RPState = STS_PR_LINK_IDLE;
			}
		}
	}
	if(rp_mag.rp_link.RPState != STS_PR_LINK_ACTIVE  && rp_mag.link_dkri != RP_DKRI_RFU){
		mesh_rp_server_set_link_sts(STS_PR_LINK_ACTIVE);
		mesh_cmd_sig_rp_send_link_open_sts();//need to loop send link open sts 
		mesh_cmd_send_link_report(STS_PR_LINK_CLS_SUC,STS_PR_LINK_ACTIVE,0,2);//
	}else{
		mesh_rp_server_set_link_sts(STS_PR_LINK_ACTIVE);
		mesh_cmd_sig_rp_send_link_open_sts();//need to loop send link open sts 
	}
    return 1;
}

int mesh_cmd_sig_rp_link_close(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    // trigger to send the link close cmd 
    int err =-1;
    remote_prov_link_close *p_link_cls = (remote_prov_link_close *)par;
    if(par_len != sizeof(remote_prov_link_close)){
        return -1;
    }
    send_rcv_retry_clr();// clear the retry part 
    prov_set_link_close_code(p_link_cls->reason);
    mesh_send_provison_data(BEARS_CTL,LINK_CLOSE,0,0);
    mesh_send_provison_data(BEARS_CTL,LINK_CLOSE,0,0);
    mesh_send_provison_data(BEARS_CTL,LINK_CLOSE,0,0);
    mesh_send_provison_data(BEARS_CTL,LINK_CLOSE,0,0);
	
	if(rp_mag.rp_link.RPState != STS_PR_LINK_IDLE){
		mesh_rp_server_set_link_sts(STS_PR_LINK_IDLE);
		err = mesh_cmd_sig_rp_send_link_sts(cb_par);
		err = mesh_cmd_send_link_report(REMOTE_PROV_STS_SUC,STS_PR_LINK_IDLE,0,2);// no reason field
	}else{
		err = mesh_cmd_sig_rp_send_link_sts(cb_par);
	}
    mesh_rp_server_prov_end_cb();
	mesh_rp_ser_tick_reset();
	return err;
}

int mesh_cmd_sig_rp_link_sts(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err =-1;
    return err;
}

int mesh_cmd_sig_rp_link_report(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
    return err;
	#if 0
    remote_prov_link_sts_str *p_link_sts = &(rp_mag.rp_link);
    return mesh_rp_cmd_st_rsp(REMOTE_PROV_LINK_REPORT,(u8 *)(p_link_sts),sizeof(remote_prov_link_sts_str),cb_par);
	#endif
}

int mesh_cmd_send_link_report(u8 status,u8 RPState,u8 reason,u8 len)
{
	int err =-1;
	remote_prov_link_sts_str link_report;
	link_report.status = status;
	link_report.RPState = RPState;
	link_report.reason = reason;
	// the link report cmd should force to send by access cmd.
	mesh_seg_must_en(1);
	err = mesh_tx_cmd_rsp(REMOTE_PROV_LINK_REPORT,(u8*)&link_report,len,ele_adr_primary,rp_mag.link_adr,0,0);
	mesh_seg_must_en(0);
	return err;
}

void mesh_adv_prov_link_open(u8 *p_uuid)
{
    // not use the provision state ,just use the bearer part 
    memcpy(prov_link_uuid,p_uuid,16);
    send_rcv_retry_set(PRO_BEARS_CTL,LINK_OPEN,0); //test case use start_tick in mesh_send_provison_data
    prov_para.link_id = clock_time();
	mesh_send_provison_data(BEARS_CTL,LINK_OPEN,p_uuid,16);
    return ;
}
#define MAX_REMOTE_PROV_TIME    30*1000*1000
u32 mesh_rp_ser_tick =0;
void mesh_rp_ser_tick_set()
{
    mesh_rp_ser_tick = clock_time()|1;
}
void mesh_rp_ser_tick_reset()
{
    mesh_rp_ser_tick = 0;
}
void mesh_rp_ser_tick_loop()
{
#if !WIN32
    if(mesh_rp_ser_tick && clock_time_exceed(mesh_rp_ser_tick,MAX_REMOTE_PROV_TIME)){
		mesh_rp_ser_tick =0;
		prov_para.link_id_filter = 0;
        mesh_rp_ser_tick_reset();
        mesh_rp_server_pdu_reset();
        mesh_rp_server_prov_end_cb();
	    send_rcv_retry_clr();
		disable_mesh_adv_filter();
    }
#endif
}

void mesh_prov_server_send_cmd(u8 *par,u8 len)
{
#if !WIN32
    remote_proc_pdu_sts_str *p_pdu_sts = &(rp_mag.rp_pdu);
    mesh_pro_data_structer *p_client_rcv = (mesh_pro_data_structer *)(par);
    mesh_pro_data_structer *p_send_str = (mesh_pro_data_structer *)(para_pro);
    u8 prov_cmd = p_client_rcv->invite.header.type;
    switch(p_pdu_sts->sts){
        case RP_SER_INVITE_SEND:
            if(prov_cmd == PRO_INVITE){
                LAYER_PARA_DEBUG(A_DEBUG_PRO_INVITE);
                mesh_rp_ser_tick_set();// timeout start tick 
                mesh_adv_prov_send_invite(p_send_str,p_client_rcv->invite.attentionDura);
                mesh_rp_server_set_sts(PR_SER_CAPA_RCV);
				// only enable the rcv of the provision data part 
				enable_mesh_adv_filter();
				LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_SER_INVITE_SEND",0);
            }
            break;
        case RP_SER_START_SEND:
            if(prov_cmd == PRO_START){
                LAYER_PARA_DEBUG(A_DEBUG_PRO_START);
				mesh_prov_pdu_send_retry_clear();
                mesh_adv_prov_send_start_cmd(p_send_str,&(p_client_rcv->start));
                mesh_rp_server_set_sts(RP_SER_START_ACK);
				enable_mesh_adv_filter();
				LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_SER_START_SEND",0);
            }
            break;
        case RP_SER_PUBKEY_SEND:
            if(prov_cmd == PRO_PUB_KEY){
                LAYER_PARA_DEBUG(A_DEBUG_PRO_PUB_KEY);
				mesh_prov_pdu_send_retry_clear();
                mesh_adv_prov_pubkey_send(p_send_str,(p_client_rcv->pubkey.pubKeyX));
                mesh_rp_server_set_sts(PR_SER_PUBKEY_RSP);
				enable_mesh_adv_filter();
				LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_SER_PUBKEY_SEND",0);
            }
            break;
        case PR_SER_COMFIRM_SEND:
            if(prov_cmd == PRO_CONFIRM){
                LAYER_PARA_DEBUG(A_DEBUG_PRO_CONFIRM);
				mesh_prov_pdu_send_retry_clear();
                mesh_adv_prov_comfirm_cmd(p_send_str,(p_client_rcv->comfirm.comfirm));
                mesh_rp_server_set_sts(PR_SER_COMFIRM_RSP);
				enable_mesh_adv_filter();
				LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:PR_SER_COMFIRM_SEND",0);
            }
            break;
        case PR_SER_RANDOM_SEND:
            if(prov_cmd == PRO_RANDOM){
                LAYER_PARA_DEBUG(A_DEBUG_PRO_RANDOM);
				mesh_prov_pdu_send_retry_clear();
                mesh_adv_prov_random_cmd(p_send_str,(p_client_rcv->random.random));
                mesh_rp_server_set_sts(PR_SER_RANDOM_RSP);
				enable_mesh_adv_filter();
				LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:PR_SER_RANDOM_SEND",0);
            }
            break;
        case PR_SER_DATA_SEND:
            if(prov_cmd == PRO_DATA){
                LAYER_PARA_DEBUG(A_DEBUG_PRO_DATA);
				mesh_prov_pdu_send_retry_clear();
                mesh_adv_prov_data_send(p_send_str,(p_client_rcv->data.encProData));
                mesh_rp_server_set_sts(PR_SER_COMPLETE_RSP);    
				enable_mesh_adv_filter();
				LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:PR_SER_DATA_SEND",0);
            }
            break;
        default:
            break;
    }
#endif
    return ;
}

int mesh_prov_server_to_client_cmd(u8 *prov_dat,u8 len)
{
	int err =-1;
	mesh_seg_must_en(1);
    err = mesh_cmd_sig_send_rp_pdu_send(prov_dat,len);
	mesh_seg_must_en(0);
	return err;
}

void mesh_prov_pdu_send_retry_proc()
{
    remote_prov_retry_str *p_retry = &(rp_mag.rp_pdu.re_send);
    if( p_retry->retry_flag && 
        (p_retry->tick!=0) && 
        clock_time_exceed(p_retry->tick ,(REMOTE_PROV_SERVER_RETRY_INTER))){
        p_retry->tick = clock_time()|1; 
		if (p_retry->retry_flag & REMOTE_PROV_SERVER_OUTBOUND_FLAG&& !is_busy_tx_segment_or_reliable_flow()){
            // proc the rsp of the segment cmd ,and proc one packet for some time.
			//LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"REMOTE OUTBOUND_FLAG send",0);
			if(mesh_cmd_sig_rp_pdu_outbound_send() == 0){
				p_retry->retry_flag &=~(REMOTE_PROV_SERVER_OUTBOUND_FLAG);
				//LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"REMOTE OUTBOUND_FLAG send suc",0);
				return ;
			}
        }
        if(p_retry->retry_flag & REMOTE_PROV_SERVER_CMD_FLAG&&!is_busy_tx_segment_or_reliable_flow()){
			u8 *prov_data = (p_retry->adv).transStart.data;
			u8 pro_cmd = prov_data[0];
			u8 len = get_mesh_pro_str_len(pro_cmd);
			//LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"REMOTE cmd send",0);
			if(mesh_prov_server_to_client_cmd(prov_data,len)== 0){
				p_retry->retry_flag &=~(REMOTE_PROV_SERVER_CMD_FLAG);
				//LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"REMOTE cmd send suc",0);
				return ;
			}
        }
    }
}

void mesh_prov_report_loop_proc()
{
	if(rp_mag.link_timeout && clock_time_exceed_s(rp_mag.rp_now_s ,rp_mag.link_timeout)){
		mesh_cmd_send_link_report(REMOTE_PROV_LINK_OPEN_FAIL,STS_PR_LINK_IDLE,0,2);// no reason field
		mesh_cmd_sig_rsp_scan_init();// if we receive link open ,we should stop the rp scan part 
		rp_mag.rp_link.RPState = RP_SER_IDLE; // should allow to link open again
		rp_mag.link_timeout = 0;
		rp_mag.rp_now_s = clock_time_s();
	}
}

void mesh_prov_pdu_link_open_sts_proc()
{
	remote_proc_pdu_sts_str *p_pdu_sts = &(rp_mag.rp_pdu);
	static u32 link_open_retry_tick =0;
	if(p_pdu_sts->sts == RP_SER_INVITE_SEND && clock_time_exceed(link_open_retry_tick,2*1000*1000)){
		link_open_retry_tick = clock_time();
		//mesh_cmd_sig_rp_send_link_open_sts();//need to loop send link open sts 	
	}
}

void mesh_prov_pdu_send_retry_set_data(u8 *prov_data,u8 flag)
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

void mesh_prov_pdu_send_retry_set(pro_PB_ADV *p_adv,u8 flag)
{
	mesh_prov_pdu_send_retry_set_data(p_adv->transStart.data,flag);
}

void mesh_prov_pdu_send_retry_clear()
{
	remote_prov_retry_str *p_retry = &(rp_mag.rp_pdu.re_send);
	memset(p_retry,0,sizeof(remote_prov_retry_str));
	return ;
}

void mesh_prov_server_rcv_cmd(pro_PB_ADV *p_adv)
{
#if !WIN32
    remote_proc_pdu_sts_str *p_pdu_sts = &(rp_mag.rp_pdu);
    switch(p_pdu_sts->sts){
        case RP_SER_LINK_OPEN_ACK:
            if( p_adv->transBear.bearAck.header.GPCF == BEARS_CTL &&
				p_adv->transBear.bearAck.header.BearCtl == LINK_ACK){
				LAYER_PARA_DEBUG(A_DEBUG_LINK_OPEN_ACK);
				// after receive the link open ack ,we should rsp the link report cmd .
				mesh_cmd_send_link_report(STS_PR_LINK_CLS_SUC,STS_PR_LINK_ACTIVE,0,2);// no reason field
				rp_mag.link_timeout = 0;
				mesh_rp_server_set_link_sts(STS_PR_LINK_ACTIVE);
				mesh_adv_prov_link_open_ack(p_adv);
				disable_mesh_adv_filter();
                mesh_rp_server_set_sts(RP_SER_INVITE_SEND); 
				prov_para.link_id_filter =1;
				LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_SER_LINK_OPEN_ACK",0);
			}
			break;
			#if 0
	    case RP_SER_INVITE_ACK:
	        if( p_adv->transAck.GPCF == TRANS_ACK && 
	            p_adv->trans_num >= prov_para.trans_num_last){
	            LAYER_PARA_DEBUG(A_DEBUG_INVITE_ACK);
                mesh_adv_prov_invite_ack(p_adv);
                mesh_rp_server_set_sts(PR_SER_CAPA_RCV); 
				
	        }
	        break;
			#endif
	    case PR_SER_CAPA_RCV:
	        if(p_adv->transBear.bearAck.header.GPCF == TRANS_START &&
				p_adv->transStart.data[0]== PRO_CAPABLI){
				// RP_SER_INVITE_ACK proc
				LAYER_PARA_DEBUG(A_DEBUG_INVITE_ACK);
                mesh_adv_prov_invite_ack(p_adv);
                mesh_rp_server_set_sts(PR_SER_CAPA_RCV); 

				
				LAYER_PARA_DEBUG(A_DEBUG_PRO_CAPABLI);
                mesh_adv_prov_capa_rcv(p_adv);
                mesh_prov_pdu_send_retry_set(p_adv,REMOTE_PROV_SERVER_CMD_FLAG|REMOTE_PROV_SERVER_OUTBOUND_FLAG);
				disable_mesh_adv_filter();
                mesh_rp_server_set_sts(RP_SER_START_SEND); 
				LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:PR_SER_CAPA_RCV",0);
				
			}
			break;
	    case RP_SER_START_ACK:
	        if( p_adv->transAck.GPCF == TRANS_ACK && 
	            p_adv->trans_num >= prov_para.trans_num_last){
	            LAYER_PARA_DEBUG(A_DEBUG_START_ACK);
                mesh_adv_prov_start_ack(p_adv);
                mesh_prov_pdu_send_retry_set(NULL,REMOTE_PROV_SERVER_OUTBOUND_FLAG);
				disable_mesh_adv_filter();
                mesh_rp_server_set_sts(RP_SER_PUBKEY_SEND);
                LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:RP_SER_START_ACK",0);
	        }
	        break;
			#if 0
	    case PR_SER_PUBKEY_ACK:
	        if( p_adv->transAck.GPCF == TRANS_ACK && 
	            p_adv->trans_num >= prov_para.trans_num_last){
	            LAYER_PARA_DEBUG(A_DEBUG_PUBKEY_ACK);
                mesh_rp_server_set_sts(PR_SER_PUBKEY_RSP);
                
	        }
	        break;
			#endif
	    case PR_SER_PUBKEY_RSP:
	        if( p_adv->transBear.bearAck.header.GPCF == TRANS_START &&
				p_adv->transStart.data[0]== PRO_PUB_KEY){
				// PR_SER_PUBKEY_ACK
				LAYER_PARA_DEBUG(A_DEBUG_PUBKEY_ACK);
                mesh_rp_server_set_sts(PR_SER_PUBKEY_RSP);
				
				LAYER_PARA_DEBUG(A_DEBUG_PUBKEY_RSP);
				// when we need to send cmd ,we should stop provision cmd send first
				mesh_prov_pdu_send_retry_set(p_adv,REMOTE_PROV_SERVER_CMD_FLAG|REMOTE_PROV_SERVER_OUTBOUND_FLAG);
                mesh_adv_prov_pubkey_rsp(p_adv);
				disable_mesh_adv_filter();
                mesh_rp_server_set_sts(PR_SER_COMFIRM_SEND);
				LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:PR_SER_PUBKEY_RSP",0);
                
		    }
		    break;
			#if 0
		case PR_SER_COMFIRM_SEND_ACK:
		    if( p_adv->transAck.GPCF == TRANS_ACK && 
		        p_adv->trans_num >= prov_para.trans_num_last){
		        LAYER_PARA_DEBUG(A_DEBUG_COMFIRM_SEND_ACK);
				mesh_adv_prov_comfirm_ack(p_adv);
                mesh_rp_server_set_sts(PR_SER_COMFIRM_RSP);
				
		    }
		    break;
			#endif
		case PR_SER_COMFIRM_RSP:
		    if( p_adv->transBear.bearAck.header.GPCF == TRANS_START &&
				p_adv->transStart.data[0]== PRO_CONFIRM){
				// PR_SER_COMFIRM_SEND_ACK
				LAYER_PARA_DEBUG(A_DEBUG_COMFIRM_SEND_ACK);
				mesh_adv_prov_comfirm_ack(p_adv);
                mesh_rp_server_set_sts(PR_SER_COMFIRM_RSP);
				
				LAYER_PARA_DEBUG(A_DEBUG_COMFIRM_RSP);
				mesh_adv_prov_comfirm_rsp(p_adv);
				// when we need to send cmd ,we should stop provision cmd send first
				mesh_prov_pdu_send_retry_set(p_adv,REMOTE_PROV_SERVER_CMD_FLAG|REMOTE_PROV_SERVER_OUTBOUND_FLAG);
				disable_mesh_adv_filter();
				mesh_rp_server_set_sts(PR_SER_RANDOM_SEND);
				LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:PR_SER_COMFIRM_RSP",0);
				
		    }
		    break;
		#if 0
		case PR_SER_RANDOM_SEND_ACK:
		    if(p_adv->transAck.GPCF == TRANS_ACK&&p_adv->trans_num >= prov_para.trans_num_last){
                LAYER_PARA_DEBUG(A_DEBUG_RANDOM_SEND_ACK);
				mesh_adv_prov_random_ack_cmd(p_adv);
				mesh_rp_server_set_sts(PR_SER_RANDOM_RSP);	
			}
            break;
		#endif
        case PR_SER_RANDOM_RSP:
            if( p_adv->transBear.bearAck.header.GPCF == TRANS_START &&
				p_adv->transStart.data[0]== PRO_RANDOM){
				// PR_SER_RANDOM_SEND_ACK
				LAYER_PARA_DEBUG(A_DEBUG_RANDOM_SEND_ACK);
				mesh_adv_prov_random_ack_cmd(p_adv);
				mesh_rp_server_set_sts(PR_SER_RANDOM_RSP);
				
				LAYER_PARA_DEBUG(A_DEBUG_RANDOM_RSP);
                mesh_adv_prov_random_rsp(p_adv);
                mesh_prov_pdu_send_retry_set(p_adv,REMOTE_PROV_SERVER_CMD_FLAG|REMOTE_PROV_SERVER_OUTBOUND_FLAG);
				disable_mesh_adv_filter();
                mesh_rp_server_set_sts(PR_SER_DATA_SEND);
				LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:PR_SER_RANDOM_RSP",0);
		    }
		    break;
			#if 0
        case PR_SER_DATA_SEND_ACK:
            if( p_adv->transAck.GPCF == TRANS_ACK && 
                p_adv->trans_num >= prov_para.trans_num_last){
                LAYER_PARA_DEBUG(A_DEBUG_DATA_SEND_ACK);
                mesh_adv_prov_data_ack(p_adv);
                mesh_rp_server_set_sts(PR_SER_COMPLETE_RSP);	
            }
            break;
			#endif
        case PR_SER_COMPLETE_RSP:
            if(p_adv->transBear.bearAck.header.GPCF == TRANS_START &&
				p_adv->transStart.data[0]== PRO_COMPLETE){
				// return the test case result 
				    LAYER_PARA_DEBUG(A_DEBUG_PRO_COMPLETE);
					mesh_adv_prov_data_ack(p_adv);
					p_pdu_sts->sts = PR_SER_COMPLETE_SUC;
					prov_para.link_id_filter = 0;
				    mesh_rp_ser_tick_reset();// timeout start tick 
				    mesh_rp_adv_prov_complete_rsp(p_adv);
				    mesh_prov_pdu_send_retry_set(p_adv,REMOTE_PROV_SERVER_CMD_FLAG|REMOTE_PROV_SERVER_OUTBOUND_FLAG);
					mesh_prov_end_set_tick();// trigger event callback part 
					disable_mesh_adv_filter();
					LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"CLIENT:PR_SER_COMPLETE_RSP",0);
				}
			break;	
	    default:
	        break;
    }
#endif
    return ;
}

#if !WIN32
void mesh_rp_dkri_invite_capa_proc(mesh_pro_data_structer *p_rcv,mesh_pro_data_structer *p_send)
{
	confirm_input[0]= p_rcv->invite.attentionDura;
	set_pro_capa_cpy(p_send,&prov_oob);// set capability part 
	swap_mesh_pro_capa(p_send);
	memcpy(confirm_input+1,&(p_send->capa.ele_num),11);
}

int mesh_rp_dkri_pubkey_rcv_send(mesh_pro_data_structer *p_rcv,mesh_pro_data_structer *p_send)
{
	if(!mesh_check_pubkey_valid(p_rcv->pubkey.pubKeyX)){
		return 0;
	}
	u8 dev_public_key[64];
	get_public_key(dev_public_key);
	memcpy(confirm_input+0x11,p_rcv->pubkey.pubKeyX,0x40);
	memcpy(confirm_input+0x11+0x40,dev_public_key,0x40);
	set_pro_pub_key(p_send,dev_public_key,dev_public_key+32);
	return 1;
}

int mesh_rp_dkri_comfirm_rcv_send(mesh_pro_data_structer *p_rcv,mesh_pro_data_structer *p_send)
{
	if(!mesh_node_oob_auth_data(&prov_oob)){// set the auth part ,only for the no oob ,and static oob
		return 0;
	}
	pro_trans_comfirm *p_comfirm;
	u8 dev_private_key[32];
	p_comfirm = &(p_rcv->comfirm);
	memcpy(pro_comfirm,p_comfirm->comfirm,16);
	get_private_key(dev_private_key);
	tn_p256_dhkey (ecdh_secret, dev_private_key, confirm_input+0x11, confirm_input+0x11+0x20);
	mesh_sec_prov_confirmation (dev_comfirm, confirm_input, 145, ecdh_secret, dev_random, dev_auth);
	set_pro_comfirm(p_send,dev_comfirm);
	return 1;
}

int mesh_rp_dkri_random_rcv_send(mesh_pro_data_structer *p_rcv,mesh_pro_data_structer *p_send)
{
	memcpy(pro_random,p_rcv->random.random,16);
	// use the provision random to caculate the provision comfirm 
	u8 pro_comfirm_tmp[16];
	mesh_sec_prov_confirmation (pro_comfirm_tmp, confirm_input, 145, ecdh_secret, pro_random, dev_auth);
	if(memcmp(pro_comfirm_tmp,pro_comfirm,16)){
		return 0;
	}
	set_pro_random(p_send,dev_random);	
	return 1;
}

void mesh_dkri_set_flag_devkey(u8 dkri,u8 *p_dev_key)
{
	prov_para.dkri = REMOTE_PROV_DKRI_EN_FLAG|dkri;
	memcpy(node_devkey_candi, p_dev_key, 16);
}


int mesh_rp_dkri_prov_data_proc(u8 dkri,u8* p_dev_key,u8 *p_prov_net)
{
	provison_net_info_str *p_net = &(provision_mag.pro_net_info);
	provison_net_info_str *p_net_data = (provison_net_info_str *)(p_prov_net);
	//LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"mesh_rp_dkri_prov_data_proc",0);
	if(dkri == RP_DKRI_DEV_KEY_REFRESH){// only need to update the devkey candidate
		// compare the para part 
		if(	!memcmp(p_net->net_work_key ,p_net_data->net_work_key,sizeof(p_net_data->net_work_key))&&
			(p_net->key_index == p_net_data->key_index)&&
			!memcmp(p_net->iv_index ,p_net_data->iv_index,sizeof(p_net_data->iv_index))&&
			p_net->unicast_address == p_net_data->unicast_address){
				// set device key candidate
				//LOG_MSG_INFO(TL_LOG_REMOTE_PROV,p_dev_key,16,"node's dkri is %d ",dkri);
				mesh_dkri_set_flag_devkey(dkri,p_dev_key);
				return 1;
		}else{
			return 0;
		}
	}else if(dkri == RP_DKRI_NODE_ADR_REFRESH){
		if(	!memcmp(p_net->net_work_key ,p_net_data->net_work_key,sizeof(p_net_data->net_work_key))&&
			(p_net->key_index == p_net_data->key_index)&&
			!memcmp(p_net->iv_index ,p_net_data->iv_index,sizeof(p_net_data->iv_index))){
				mesh_dkri_set_flag_devkey(dkri,p_dev_key);
				// need to store the prov_net_info first ,after the procedure ,we will update it .
				memcpy(&node_adr_net_info,p_prov_net,sizeof(node_adr_net_info));
				return 1;
		}else{
			return 0;
		}
	}else if (dkri == RP_DKRI_NODE_CPS_REFRESH){
		// compare the para part 
		if(	!memcmp(p_net->net_work_key ,p_net_data->net_work_key,sizeof(p_net_data->net_work_key))&&
			(p_net->key_index == p_net_data->key_index)&&
			!memcmp(p_net->iv_index ,p_net_data->iv_index,sizeof(p_net_data->iv_index))&&
			p_net->unicast_address == p_net_data->unicast_address){
				mesh_dkri_set_flag_devkey(dkri,p_dev_key);
				return 1;
		}else{
			return 0;
		}
	}
	return 0;
}

int mesh_rp_dkri_data_rcv_complete(mesh_pro_data_structer *p_rcv,mesh_pro_data_structer *p_send)
{
 	u8 *p_prov_net = (u8 *)(p_rcv)+1;
	u8 dev_key[16];
	u8 prov_salt[16];
	u8 dev_session_key[16];
	u8 dev_session_nonce[16];
	mesh_sec_prov_salt(prov_salt,confirm_input,pro_random,dev_random);
	mesh_sec_prov_session_key (dev_session_key, dev_session_nonce, confirm_input, 
									145, ecdh_secret, pro_random, dev_random);
	int err = mesh_prov_sec_msg_dec (dev_session_key, dev_session_nonce+3, p_prov_net, 33, 8);
	//calculate the dev_key part 
	mesh_sec_dev_key(dev_key,prov_salt,ecdh_secret);
	if(0 == err){
        set_pro_fail(p_send,DECRYPTION_FAIL);
	}else{
    	if(mesh_rp_dkri_prov_data_proc(rp_mag.link_dkri,dev_key,p_prov_net)){
    		if(rp_mag.link_dkri != RP_DKRI_NODE_ADR_REFRESH){
    			memcpy(&provision_mag.pro_net_info,p_prov_net,sizeof(provison_net_info_str));
    			// add the info about the gatt mode provision ,should set the cfg data part into the node identity
    			mesh_provision_par_handle((u8 *)&provision_mag.pro_net_info);
    		}
    		set_pro_complete(p_send);
    	}else{
    		// should be the pro fail cmd to disconnect
    		set_pro_fail(p_send,INVALID_DATA_PROV);
    	}
	}
	return err;
}


void mesh_dkri_precedure_proc(u8 *par,u8 len)
{
#if !WIN32
	remote_proc_pdu_sts_str *p_pdu_sts = &(rp_mag.rp_pdu);
	mesh_pro_data_structer *p_rcv = (mesh_pro_data_structer *)(par);
	mesh_pro_data_structer *p_send = (mesh_pro_data_structer *)(para_pro);
	u8 prov_cmd = p_rcv->invite.header.type;
	// the client will retry ,and the server only need to retry the mesh cmd part .
	switch(p_pdu_sts->sts){
		case RP_SER_INVITE_SEND:
			if(prov_cmd == PRO_INVITE){
				mesh_rp_ser_tick_set();
				LOG_MSG_INFO(TL_LOG_REMOTE_PROV,(u8*)(&p_rcv->invite),sizeof(p_rcv->invite),"dkri:invite cmd is ",0);
				// rsp the outbound ,and set the capability
				mesh_rp_dkri_invite_capa_proc(p_rcv,p_send);	
				mesh_prov_pdu_send_retry_set_data((u8*)p_send,REMOTE_PROV_SERVER_CMD_FLAG|REMOTE_PROV_SERVER_OUTBOUND_FLAG);
				mesh_rp_server_set_sts(RP_SER_START_SEND);
				LOG_MSG_INFO(TL_LOG_REMOTE_PROV,(u8*)(&p_send->capa),sizeof(p_send->capa),"dkri:capa cmd is ",0);
			}
			break;
		case RP_SER_START_SEND:
			if(prov_cmd == PRO_START && dispatch_start_cmd_reliable(p_rcv)){
				//rsp the outbound 
				LOG_MSG_INFO(TL_LOG_REMOTE_PROV,(u8*)(&p_send->start),sizeof(p_rcv->start),"dkri:start cmd is ",0);
				mesh_node_prov_event_callback(EVENT_MESH_NODE_RC_LINK_START);
				set_node_prov_start_oob(p_rcv,&prov_oob);//set the start cmd for the prov oob info 
				memcpy(confirm_input+12,&(p_rcv->start.alogrithms),5);
				mesh_prov_pdu_send_retry_set(NULL,REMOTE_PROV_SERVER_OUTBOUND_FLAG);
				mesh_rp_server_set_sts(RP_SER_PUBKEY_SEND);
			}
			break;
		case RP_SER_PUBKEY_SEND:
			if(prov_cmd == PRO_PUB_KEY){
				LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"dkri:pubkey rcv suc ",0);
				if(mesh_rp_dkri_pubkey_rcv_send(p_rcv,p_send)){
					LOG_MSG_INFO(TL_LOG_REMOTE_PROV,(u8*)(&p_rcv->pubkey),sizeof(p_rcv->pubkey),"dkri:pubkey rcv cmd is ",0);
					mesh_prov_pdu_send_retry_set_data((u8*)p_send,REMOTE_PROV_SERVER_CMD_FLAG|REMOTE_PROV_SERVER_OUTBOUND_FLAG);
					mesh_rp_server_set_sts(PR_SER_COMFIRM_SEND);
					LOG_MSG_INFO(TL_LOG_REMOTE_PROV,(u8*)(&p_send->pubkey),sizeof(p_send->pubkey),"dkri:pubkey send cmd is ",0);
				}else{// pubkey is invalid
					
				}
			}
			break;
		case PR_SER_COMFIRM_SEND:
			if(prov_cmd == PRO_CONFIRM){
				LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"dkri:comfirm rcv suc ",0);
				if(mesh_rp_dkri_comfirm_rcv_send(p_rcv,p_send)){
					LOG_MSG_INFO(TL_LOG_REMOTE_PROV,(u8*)(&p_rcv->comfirm),sizeof(p_rcv->comfirm),"dkri:comfirm rcv cmd is ",0);
					mesh_prov_pdu_send_retry_set_data((u8*)p_send,REMOTE_PROV_SERVER_CMD_FLAG|REMOTE_PROV_SERVER_OUTBOUND_FLAG);
					mesh_rp_server_set_sts(PR_SER_RANDOM_SEND);
					LOG_MSG_INFO(TL_LOG_REMOTE_PROV,(u8*)(&p_send->comfirm),sizeof(p_send->comfirm),"dkri:comfirm send cmd is ",0);
				}else{//auth data is invalid
						
				}
			}
			break;
		case PR_SER_RANDOM_SEND:
			if(prov_cmd == PRO_RANDOM){
				LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"dkri:random rcv suc ",0);
				if(mesh_rp_dkri_random_rcv_send(p_rcv,p_send)){
					LOG_MSG_INFO(TL_LOG_REMOTE_PROV,(u8*)(&p_rcv->random),sizeof(p_rcv->random),"dkri:random rcv cmd is ",0);
					mesh_prov_pdu_send_retry_set_data((u8*)p_send,REMOTE_PROV_SERVER_CMD_FLAG|REMOTE_PROV_SERVER_OUTBOUND_FLAG);
					mesh_rp_server_set_sts(PR_SER_DATA_SEND);
					LOG_MSG_INFO(TL_LOG_REMOTE_PROV,(u8*)(&p_send->random),sizeof(p_send->random),"dkri:random send cmd is ",0);
				}
			}
			break;
		case PR_SER_DATA_SEND:
			if(prov_cmd == PRO_DATA){
				int err = mesh_rp_dkri_data_rcv_complete(p_rcv,p_send);
				LOG_MSG_INFO(TL_LOG_REMOTE_PROV,(u8 *)&provision_mag.pro_net_info,sizeof(provision_mag.pro_net_info),"dkri:complete send cmd is ",0);
				mesh_prov_pdu_send_retry_set_data((u8*)p_send,REMOTE_PROV_SERVER_CMD_FLAG|REMOTE_PROV_SERVER_OUTBOUND_FLAG);
				LOG_MSG_INFO(TL_LOG_REMOTE_PROV,(u8*)(&p_send->complete),sizeof(p_send->complete),"dkri:complete send cmd is ",0);
				// need to wait for some time to send and retry the outbound packet ,and the complete rsp .
				mesh_rp_ser_tick_reset();// clear the timeout proc part 
				if(err){
				     // TODO
				}else{
				    mesh_prov_end_set_tick();// trigger event callback part 
				}
			}
			break;
		default:
			break;
	}
#endif
}
#endif
int mesh_cmd_sig_rp_prov_pdu_send(u8 *par,int par_len)
{
    if(par_len > sizeof(pro_trans_pubkey)){
        return 0;
    }
	if(mesh_rp_link_dkri_is_valid(rp_mag.link_dkri)){
		#if !WIN32
		mesh_dkri_precedure_proc(par,par_len);
		#endif
	}else{
		mesh_prov_server_send_cmd(par,par_len);
	}
    return 1;
}

int mesh_cmd_sig_rp_pdu_outbound_send()
{
    int err =-1;
    remote_proc_pdu_sts_str *p_pdu_sts = &(rp_mag.rp_pdu);
	mesh_seg_must_en(1);
    err = mesh_tx_cmd_rsp(REMOTE_PROV_PDU_OUTBOUND_REPORT,(u8 *)&(p_pdu_sts->outbound),
                                                1,ele_adr_primary,p_pdu_sts->src,0,0);
    mesh_seg_must_en(0);
    return err;
}

int mesh_cmd_sig_rp_pdu_send(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    remote_prov_link_sts_str *p_link_sts = &(rp_mag.rp_link);
    remote_prov_pdu_send *p_pdu = (remote_prov_pdu_send *)par;
    remote_proc_pdu_sts_str *p_pdu_sts = &(rp_mag.rp_pdu);
    p_pdu_sts->outbound = p_pdu->OutboundPDUNumber;
    p_pdu_sts->src = cb_par->adr_src;
    // send provision data part 
    mesh_cmd_sig_rp_prov_pdu_send(p_pdu->ProvisioningPDU,par_len-1);
    p_link_sts->RPState = STS_PR_OUTBOUND_TRANS;
    // need to dispatch by the node provisioner self 
    p_link_sts->RPState = STS_PR_LINK_OPEN;
    return 0;
}

int mesh_cmd_sig_rp_pdu_outbound_report(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err =-1;
    return err;
}

u8 mesh_pr_sts_work_or_not()
{
    remote_prov_link_sts_str *p_link_sts = &(rp_mag.rp_link);
    if( p_link_sts->RPState == STS_PR_LINK_OPEN||
        p_link_sts->RPState == STS_PR_OUTBOUND_TRANS||
        p_link_sts->RPState == STS_PR_LINK_ACTIVE){
        return 1;
    }else{
        return 0;
    }
}

int mesh_cmd_sig_send_rp_pdu_send(u8 *par,int par_len)
{
    int err =-1;
    remote_prov_link_sts_str *p_link_sts = &(rp_mag.rp_link);
    if( p_link_sts->RPState!=STS_PR_LINK_OPEN ||
        p_link_sts->RPState!=STS_PR_OUTBOUND_TRANS   ){
        // link layer not suc 
       // return err;
    }
    remote_proc_pdu_sts_str *p_pdu_sts =&(rp_mag.rp_pdu);
    if(par_len > sizeof(pro_trans_pubkey)){
      //  return -1;
    }
    u8 prov_pdu[sizeof(pro_trans_pubkey)];
    prov_pdu[0]= p_pdu_sts->inbound;
    memcpy(prov_pdu+1,par,par_len);
    err = mesh_tx_cmd_rsp(REMOTE_PROV_PDU_REPORT,prov_pdu,par_len+1,ele_adr_primary,p_pdu_sts->src,0,0);
    if(err != -1){
        // send suc 
        //p_pdu_sts->inbound++;
    }
    return err;
}

int mesh_cmd_sig_rp_pdu_report(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err =-1;
    return err;
}

int mesh_remote_prov_st_publish(u8 idx)
{
	return 1;
}


void mesh_cmd_sig_rp_server_loop_proc()
{
	mesh_prov_pdu_link_open_sts_proc();
    mesh_prov_pdu_send_retry_proc();
    mesh_cmd_sig_rp_scan_proc();
	mesh_prov_report_loop_proc();
}


u8 mesh_rsp_opcode_is_rp(u16 opcode)
{
    if(opcode >= REMOTE_PROV_SCAN_CAPA_GET && opcode<=REMOTE_PROV_PDU_REPORT){
        return 1;
    }else{
        return 0;
    }
}

void mesh_cmd_sig_rp_loop_proc()
{
    #if WIN32
    mesh_rp_pdu_retry_send();
    #else
	mesh_cmd_extend_timeout_proc();
    mesh_rp_ser_tick_loop();
    mesh_cmd_sig_rp_server_loop_proc();
    #endif
}
#endif



