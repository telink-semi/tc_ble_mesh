/********************************************************************************************************
 * @file	app_heartbeat.c
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
#include "app_heartbeat.h"
#if DU_ENABLE
#include "user_du.h"
u8 heartbeat_en =1;
#else
u8 heartbeat_en =1;
#endif
u8 hb_sts_change = 0;
u32 hb_pub_100ms =0;
u32 hb_sub_100ms =0;


/**
 * @brief       This function trigger sending heartbeat by states changed of low power node.
 * @return      none
 * @note        
 */
void mesh_cmd_sig_lowpower_heartbeat()
{
	if(model_sig_cfg_s.hb_pub.feature & BIT(MESH_HB_LOWPOWER_BIT)){
		hb_sts_change = 1;
		hb_pub_100ms = clock_time_100ms();
	}
}

// heart beat part dispatch 

/**
 * @brief       This function will be called when receive the opcode of "Config Heartbeat Publication Status"
 * @param[in]   par		- parameter of this message
 * @param[in]   par_len	- parameter length
 * @param[in]   cb_par	- parameters output by callback function which handle the opcode received.
 * @return      0: success, others: error.
 * @note        
 */
int mesh_cmd_sig_heart_pub_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	// reserve to diaptch the status 
	int err = 0;
	if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

/**
 * @brief       This function send response of "Config Heartbeat Publication Status".
 * @param[in]   adr_src	- source address of "Config Heartbeat Publication Get/Set"
 * @param[in]   sts		- status of "Config Heartbeat Publication Status"
 * @return      0: success, others: error code.
 * @note        
 */
int mesh_cmd_sig_heartbeat_pub_sts(u8 sts,u16 adr_src)
{
	int err =-1;
	mesh_heartbeat_pub_status_t heart_pub_status;
	heart_pub_status.status = sts;
	heart_pub_status.dst = model_sig_cfg_s.hb_pub.dst_adr;
	heart_pub_status.countlog = model_sig_cfg_s.hb_pub.cnt_log;
	heart_pub_status.periodlog = model_sig_cfg_s.hb_pub.per_log;
	heart_pub_status.ttl = model_sig_cfg_s.hb_pub.ttl;
	heart_pub_status.feature = model_sig_cfg_s.hb_pub.feature;
	//heart_pub_status.netkeyIndex = mesh_key.net_key[mesh_key.netkey_sel_dec][0].index;
	heart_pub_status.netkeyIndex = model_sig_cfg_s.hb_pub.netkeyidx;
	return err = mesh_tx_cmd_rsp_cfg_model(HEARTBEAT_PUB_STATUS,(u8 *)(&heart_pub_status.status),sizeof(heart_pub_status),adr_src);
}

/**
 * @brief       This function will be called when receive the opcode of "Config Heartbeat Publication Get"
 * @param[in]   par		- parameter of this message
 * @param[in]   par_len	- parameter length
 * @param[in]   cb_par	- parameters output by callback function which handle the opcode received.
 * @return      0: success, others: error code.
 * @note        
 */
int mesh_cmd_sig_heartbeat_pub_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	return mesh_cmd_sig_heartbeat_pub_sts(ST_SUCCESS,cb_par->adr_src);
}

/**
 * @brief       This function get count from log. (2^n)
 * @param[in]   val	- log value
 * @return      2^val
 * @note        
 */
u16 get_cnt_log_to_val(u8 val)
{
	if((val == 0)||(val>0x11 && val <0xff)){
		return 0;
	}else if(val == 0x11){
		return 0xfffe;
	}else if(val == 0xff){
		return 0xffff;
	}else{
		return 1<<(val-1);
	}
}

/**
 * @brief       This function check if parameters are valid.
 * @param[in]   p_pub	- pointer of "Config Heartbeat Publication Set"
 * @return      0: success, others: error code.
 * @note        
 */
u8 mesh_heartbeat_para_valid(mesh_cfg_model_heartbeat_pub_set_t *p_pub)
{
	if(p_pub->count_log>0x11 &&p_pub->count_log<0xff ){// cnt log
		return ST_INVALID_PUB_PAR;
	}else if (p_pub->period_log>0x11){// period bigger than 0x11 is invalid
		return ST_INVALID_PUB_PAR;
	}else if (p_pub->ttl>0x7f){// ttl is invalid
		return ST_UNSPEC_ERR;
	/*}else if (p_pub->features&0xfff0 != 0){//  // should skip reserved filed. // higher bit is reserved
		return ST_FEATURE_NOT_SUPPORT;*/
	}else if (mesh_net_key_find(p_pub->netkeyindex) == NET_KEY_MAX){
		return ST_INVALID_NETKEY;
	}else{
		return ST_SUCCESS;
	}
}

/**
 * @brief       This function heartbeat pub set
 * @param[in]   cb_par	- Callback parameter pointer
 * @param[in]   par		- param
 * @param[in]   par_len	- param len
 * @return      -1:rsp fail; 0:rsp suc
 * @note        
 */
int mesh_cmd_sig_heartbeat_pub_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err = -1;
	u8 sts;
	mesh_heartbeat_pub_str *p_pub = &(model_sig_cfg_s.hb_pub);
	mesh_cfg_model_heartbeat_pub_set_t *p_pub_set = (mesh_cfg_model_heartbeat_pub_set_t *)(par);
	sts = mesh_heartbeat_para_valid(p_pub_set);
	if(sts!= ST_SUCCESS){
		// rsp invalid sts directly
		mesh_heartbeat_pub_status_t rsp;
		rsp.status = sts;
		memcpy(&rsp.dst, p_pub_set, sizeof(mesh_heartbeat_pub_status_t) - OFFSETOF(mesh_heartbeat_pub_status_t, dst));
		return mesh_tx_cmd_rsp_cfg_model(HEARTBEAT_PUB_STATUS,(u8 *)(&rsp),sizeof(rsp),cb_par->adr_src);
	}
	// only the para is valid ,can set the para part.
	p_pub->dst_adr = p_pub_set->dst;
	p_pub->cnt_log = p_pub_set->count_log;
	p_pub->cnt_val = get_cnt_log_to_val(p_pub_set->count_log);
	p_pub->per_log = p_pub_set->period_log;
	p_pub->ttl = p_pub_set->ttl;
	p_pub->feature = (p_pub_set->features)&0x000f;
	p_pub->netkeyidx = p_pub_set->netkeyindex;
	// leave the key index settings .
	//rsp the status 
	err = mesh_cmd_sig_heartbeat_pub_sts(ST_SUCCESS,cb_par->adr_src);
#if TESTCASE_FLAG_ENABLE
	hb_pub_100ms = clock_time_100ms(); // test case required, because count in response of get command, should be same with set command.
#else
	hb_pub_100ms = clock_time_100ms()-BIT(31);  // Ali required, send once at once after received command.
#endif
	mesh_common_store(FLASH_ADR_MD_CFG_S);
#if DU_ENABLE
	du_bind_end_proc(cb_par->adr_src,4);
#endif

	return err;
}

/**
 * @brief       This function will be called when receive the opcode of "Config Heartbeat Publication Set"
 * @param[in]   par		- parameter of this message
 * @param[in]   par_len	- parameter length
 * @param[in]   cb_par	- parameters output by callback function which handle the opcode received.
 * @return      0: success, others: error code.
 * @note        
 */
int mesh_cmd_sig_heartbeat_sub_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	mesh_cfg_model_heartbeat_sub_status_t sub_status;
	mesh_heartbeat_sub_str *p_cfg_hb_sub =&(model_sig_cfg_s.hb_sub);
	sub_status.status = ST_SUCCESS;

	memcpy((u8 *)(&sub_status)+1,(u8 *)(p_cfg_hb_sub),sizeof(mesh_cfg_model_heartbeat_sub_status_t)-1);
	if(	p_cfg_hb_sub->adr == 0||
		p_cfg_hb_sub->dst == 0){
		if(cb_par->op == HEARTBEAT_SUB_GET){
			memset((u8 *)(&sub_status)+1,0,8);
		}
	}
	err = mesh_tx_cmd_rsp_cfg_model(HEARTBEAT_SUB_STATUS,(u8 *)(&sub_status.status),sizeof(sub_status),cb_par->adr_src);
	return err;
}

/**
 * @brief       This function will be called when receive the opcode of "Config Heartbeat Subscription Set"
 * @param[in]   par		- parameter of this message
 * @param[in]   par_len	- parameter length
 * @param[in]   cb_par	- parameters output by callback function which handle the opcode received.
 * @return      0: success, others: error code.
 * @note        
 */
int mesh_cmd_sig_heartbeat_sub_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err =-1;
	mesh_cfg_model_heartbeat_sub_set_t *p_sub_set = (mesh_cfg_model_heartbeat_sub_set_t *)(par);
	mesh_heartbeat_sub_str *p_cfg_hb_sub =&(model_sig_cfg_s.hb_sub);
	if(is_unicast_adr(p_sub_set->src)||p_sub_set->src == 0){
		p_cfg_hb_sub->adr = p_sub_set->src;
	}else{
		return err;
	}
	
	if((p_sub_set->dst == ele_adr_primary) || (is_group_adr(p_sub_set->dst) || (p_sub_set->dst==0))){
		p_cfg_hb_sub->dst = p_sub_set->dst;
	}
	else{
		return err;
	}
	p_cfg_hb_sub->per_log = p_sub_set->period_log;
	
	if((p_sub_set->src==0) || (p_sub_set->dst==0)){
		p_cfg_hb_sub->adr = 0;
		p_cfg_hb_sub->dst = 0;
		p_cfg_hb_sub->per_log = 0;
		p_cfg_hb_sub->cnt_log =0;
		p_cfg_hb_sub->cnt_val =0;
	}
	if( is_unicast_adr(p_sub_set->src)&&
		(is_group_adr(p_sub_set->dst)||is_unicast_adr(p_sub_set->dst))&&
		(p_sub_set->period_log!=0)){
		p_cfg_hb_sub->cnt_log =0;
		p_cfg_hb_sub->cnt_val =0;
		p_cfg_hb_sub->min_hops = 0x7f;
		p_cfg_hb_sub->max_hops = 0;
	}
	
	p_cfg_hb_sub->per_val = get_cnt_log_to_val(p_cfg_hb_sub->per_log);
	
	// how to rsp 
	mesh_common_store(FLASH_ADR_MD_CFG_S);
	err = mesh_cmd_sig_heartbeat_sub_get(par,par_len,cb_par);
	
	#if 0 //MESH_NODE_CFG_HBS_BV_02_C
	//This behavior is inconsistent with MESH.TS, which is suspected to be a problem with V1.1 PTS. V1.0 PTS does not require this modification
	if(p_sub_set->period_log == 0){
		p_cfg_hb_sub->cnt_log = 0;
	}
	#endif
	
	hb_sub_100ms = clock_time_100ms();
	return err;
}

/**
 * @brief       This function will be called when receive the opcode of "Config Heartbeat Subscription Status"
 * @param[in]   par		- parameter of this message
 * @param[in]   par_len	- parameter length
 * @param[in]   cb_par	- parameters output by callback function which handle the opcode received.
 * @return      0: success, others: error code.
 * @note        
 */
int mesh_cmd_sig_heartbeat_sub_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err = 0;
	if(cb_par->model){  // model may be Null for status message
    }
	return err;
}

/**
 * @brief       This function will be called when receive CMD_CTL_HEARTBEAT(Control message of Heartbeat).
                This function process this command.
 * @param[in]   p_bear	- bearer pointer of this command.
 * @return      none
 * @note        
 */
void mesh_process_hb_sub(mesh_cmd_bear_t *p_bear)
{
	mesh_cmd_nw_t *p_nw = &(p_bear->nw);
	mesh_heartbeat_sub_str *p_sub = &(model_sig_cfg_s.hb_sub);
	#if WIN32 
	mesh_heartbeat_cb_data(p_bear);
	#endif
	#if GATEWAY_ENABLE
	mesh_hb_msg_cb_t hb_par;
	hb_par.src = p_nw->src;
	hb_par.dst = p_nw->dst;
	memcpy(&hb_par.hb_msg, p_bear->lt_ctl_unseg.data, sizeof(mesh_hb_msg_t));
	gateway_heartbeat_cb((u8 *)&hb_par,sizeof(mesh_hb_msg_cb_t));
	#endif
	if(p_sub->dst == 0 || p_sub->adr == 0 || (p_sub->per_log == 0)){
		return ;
	}
	if(p_sub->adr != p_nw->src ){
		return ;
	}
	// increase the count log part 
	mesh_hb_msg_t *p_hb =(mesh_hb_msg_t *)(p_bear->lt_ctl_unseg.data);
	// swap the ttl part 
	endianness_swap_u16((u8 *)(&p_hb->fea));
	if(p_hb->iniTTL){
		p_sub->min_hops = 1;
	}else{
		p_sub->min_hops = 0;
	}
	p_sub->max_hops = p_hb->iniTTL- (p_bear->nw.ttl)+1;
	p_sub->cnt_val++;
	p_sub->cnt_log = cal_heartbeat_count_log(p_sub->cnt_val);
}

/**
 * @brief       This function send control message of Heartbeat.
 * @return      none
 * @note        
 */
void heartbeat_cmd_send_conf()
{
    mesh_hb_msg_t hb_msg;
    memset(&hb_msg, 0, sizeof(hb_msg));
	hb_msg.iniTTL = model_sig_cfg_s.hb_pub.ttl;
	mesh_page_feature_t *p_feature = (mesh_page_feature_t *)&hb_msg.fea;
#if FEATURE_RELAY_EN
	p_feature->relay = (RELAY_SUPPORT_ENABLE == model_sig_cfg_s.relay);
#endif
#if FEATURE_PROXY_EN
	p_feature->proxy = (GATT_PROXY_SUPPORT_ENABLE == model_sig_cfg_s.gatt_proxy);
#endif
#if FEATURE_FRIEND_EN
	p_feature->frid = (FRIEND_SUPPORT_ENABLE == model_sig_cfg_s.frid);	// spec "3.6.5.10 Heartbeat" indicate "in use or not in use", more details refer to "MESH/NODE/CFG/HBP/BV-04-C [Triggered publishing of Heartbeat messages - Friend state changes]"
#endif
#if FEATURE_LOWPOWER_EN
	p_feature->low_power = is_friend_ship_link_ok_lpn();				// spec "3.6.5.10 Heartbeat" indicate "in use or not in use", refer to "MESH/NODE/CFG/HBP/BV-05-C [Triggered publishing of Heartbeat messages - Low Power state changes]"
#endif
	
	mesh_tx_cmd_layer_upper_ctl(CMD_CTL_HEARTBEAT, (u8 *)(&hb_msg), sizeof(hb_msg), ele_adr_primary, model_sig_cfg_s.hb_pub.dst_adr,0);
	LOG_MSG_INFO(TL_LOG_MESH, 0, 0,"send heartbeat ",0);
	return ;
}

// mesh send heartbeat message 
/**
 * @brief       This function is a poll and process function for procedure of Heartbeat Subscription.
 * @return      none
 * @note        
 */
void mesh_heartbeat_sub_poll()
{
	// dispatch the heartbeat subscription part 
	mesh_heartbeat_sub_str *p_sub = &(model_sig_cfg_s.hb_sub);
	if(p_sub->per_val){
    	u32 heartbeat_sub_per =0;
    	if(p_sub->per_log == 0){
    		heartbeat_sub_per = 0;
    	}else if (p_sub->per_log <=0x11){
    		heartbeat_sub_per = (1<<(p_sub->per_log-1));
    	}else{
    		return ;
    	}
    	
    	if(clock_time_exceed_100ms(hb_sub_100ms,heartbeat_sub_per*10)){
    		hb_sub_100ms = clock_time_100ms();
    		p_sub->per_val-- ;
    		p_sub->per_log = cal_heartbeat_count_log(p_sub->per_val);
    	}
	}
}

/**
 * @brief       This function is a poll and process function for procedure of Heartbeat Publish.
 * @return      none
 * @note        
 */
void mesh_heartbeat_pub_poll()
{
	// dispatch the heartbeat publication part 
	mesh_heartbeat_pub_str *p_pub = &(model_sig_cfg_s.hb_pub);
	if(p_pub->dst_adr){
    	if(mesh_net_key_find(p_pub->netkeyidx)== NET_KEY_MAX){
    		return ;
    	}

    	u32 heartbeat_pb_per =0;
    	if(p_pub->per_log == 0){
    		heartbeat_pb_per = 1;
    	}else if (p_pub->per_log <=0x11){
    		heartbeat_pb_per = (1<<(p_pub->per_log-1));
    	}else{
    		return ;
    	}
    	if(p_pub->cnt_val && (!hb_sts_change)
    	&& clock_time_exceed_100ms(hb_pub_100ms,heartbeat_pb_per*10)){
    		hb_pub_100ms = clock_time_100ms();
    		if(p_pub->cnt_log == 0xff){
    			// infinite 
    		}else if (p_pub->cnt_log <= 0x11){
    			p_pub->cnt_val--;
    			p_pub->cnt_log = cal_heartbeat_count_log(p_pub->cnt_val);
    		}else{
    			//prohibit
    			return ;
    		}
    		friend_cmd_send_fn(0, CMD_CTL_HEARTBEAT);
    	}	
    	
    	if(hb_sts_change && clock_time_exceed_100ms(hb_pub_100ms,1)){
    		hb_pub_100ms = clock_time_100ms();
    		hb_sts_change--;
    		friend_cmd_send_fn(0, CMD_CTL_HEARTBEAT);
    	}
	}
}

/**
 * @brief       This function is a poll function for heartbeat.
 * @return      none
 * @note        
 */
void mesh_heartbeat_poll_100ms()
{
    if(!heartbeat_en){
        return ;
    }
	mesh_heartbeat_sub_poll();
	mesh_heartbeat_pub_poll();
}

#define HEARTBEAT_TTL_VALUE		2
#define HEARTBEAT_MIN_HOPS		0x01
#define HEARTBEAT_MAX_HOPS		0x7f

/**
 * @brief       This function set heartbeat feature
 * @return      none
 * @note        
 */
void set_heartbeat_feature()
{
	//set the heartbeat feature enable by the feature 
	mesh_page_feature_t *p_ft = (mesh_page_feature_t *)&model_sig_cfg_s.hb_pub.feature;
	p_ft->relay = is_relay_support_and_en;
	p_ft->proxy = is_proxy_enable_st;
	p_ft->frid = is_fn_support_and_en;
	p_ft->low_power = is_lpn_support_and_en;
}

/**
 * @brief       This function get high bits 
 * @param[in]   val	- value
 * @return      bit
 * @note        
 */
u8 get_high_bits(u16 val)
{
	u8 i;
	for(i=0;i<16;i++){
		if(val<BIT(i)){
			return i-1;
		}
	}
	return 15;
}

/**
 * @brief       This function calculate heartbeat count log
 * @param[in]   val	- value
 * @return      count of log
 * @note        
 */
u8 cal_heartbeat_count_log(u16 val)
{
	if(val ==0){
		return 0;
	}else if(val ==0xffff){
		return 0xff;
	}else{
		return (get_high_bits(val)+1);
	}
}

/**
 * @brief       This function check val of "heartbeat count log" is valid or not.
 * @param[in]   val	- value
 * @return      1: valid, 0: invalid.
 * @note        
 */
u8 dispatch_heartbeat_count_log(u8 val)
{
	if(val>=12 && val<=0xfe){
		return 0;
	}else{
		return 1;
	}
	return 0;
}

/**
 * @brief       This function check val of "heartbeat publish TTL" is valid or not.
 * @param[in]   val	- value
 * @return      1: valid, 0: invalid.
 * @note        
 */
u8 dispatch_heartbeat_pub_ttl(u8 val)
{
	if(val>=0x80){
		return 0;
	}else{
		return 1;
	}
}

/**
 * @brief       This function initial heartbeat
 * @return      none
 * @note        
 */
void init_heartbeat_str()
{
	// need to trigger the heartbeat msg ,at the first time 
	//hb_pub_100ms = clock_time_100ms()- BIT(31);//reserve 32767s

	// heartbeat publication 
	/*
	model_sig_cfg_s.hb_pub.dst_adr = 0xffff;
	model_sig_cfg_s.hb_pub.cnt_val = 0x0000;
	model_sig_cfg_s.hb_pub.per_log = 3;
	model_sig_cfg_s.hb_pub.cnt_log = cal_heartbeat_count_log(model_sig_cfg_s.hb_pub.cnt_val);
	model_sig_cfg_s.hb_pub.ttl = HEARTBEAT_TTL_VALUE;
	set_heartbeat_feature();
	// heartbeat subscription 
	model_sig_cfg_s.hb_sub.adr = 0;
	model_sig_cfg_s.hb_sub.dst = ele_adr_primary;
	model_sig_cfg_s.hb_sub.cnt_val= 0;
	model_sig_cfg_s.hb_sub.cnt_log =cal_heartbeat_count_log(model_sig_cfg_s.hb_sub.cnt_val); 
	model_sig_cfg_s.hb_sub.per_val = 0;
	model_sig_cfg_s.hb_sub.per_log = cal_heartbeat_count_log(model_sig_cfg_s.hb_sub.per_val);
	model_sig_cfg_s.hb_sub.min_hops = 0;
	model_sig_cfg_s.hb_sub.max_hops = 0;

	model_sig_cfg_s.hb_sub.state=1;
	model_sig_cfg_s.hb_sub.tick =clock_time();
	*/
	return ;
}

