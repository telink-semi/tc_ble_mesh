/********************************************************************************************************
 * @file	mesh_access_layer.c
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
#include "mesh_access_layer.h"


#if GATEWAY_ENABLE
access_layer_dst_addr	p_access_layer_dst_addr_cb;
void register_access_layer_dst_addr_callback(void *p){
	p_access_layer_dst_addr_cb = p;
}
#endif

//---------- cache status response
#if RELIABLE_CMD_EN
#if GATEWAY_ENABLE
#define MESH_RSP_BUF_CNT        8   // only gateway use hci tx fifo
#else
#if WIN32
#define MESH_RSP_BUF_CNT        8   // should be >= (ELE CNT + 1),    // 2   // hci tx fifo is not used, because it's process immediately in main loop
#else
#define MESH_RSP_BUF_CNT        2   // should be >= (ELE CNT + 1),    // 2   // hci tx fifo is not used, because it's process immediately in main loop
#endif
#endif
#define MESH_RSP_LEN_MAX        (sizeof(mesh_rc_rsp_t) - OFFSETOF(mesh_rc_rsp_t,src))

STATIC_ASSERT(sizeof(mesh_rc_rsp_t) % 2 == 0);

mesh_rc_rsp_t slave_rsp_buf[MESH_RSP_BUF_CNT];
status_record_t slave_status_record[MESH_ELE_MAX_NUM];
u16 slave_status_record_size = sizeof(slave_status_record);
u16 slave_status_record_idx;

u8		slave_status_buffer_num = MESH_RSP_BUF_CNT;
u8 		slave_status_buffer_rptr = 0;
u8 		slave_status_buffer_wptr = 0;

void    rf_link_slave_read_status_par_init()
{
	slave_status_buffer_rptr = slave_status_buffer_wptr = 0;
}

void	rf_link_slave_read_status_start ()
{
    rf_link_slave_read_status_par_init();
	#if WIN32 
	memset(slave_rsp_buf, 0, (int)slave_status_buffer_num*(sizeof(mesh_rc_rsp_t)));
	#else
	memset4(slave_rsp_buf, 0, (int)slave_status_buffer_num*(sizeof(mesh_rc_rsp_t)));
	#endif
	slave_status_record_idx = 0;
	memset(slave_status_record, 0, slave_status_record_size);
}

int		is_slave_read_status_buf_empty ()
{
	return (slave_status_buffer_rptr == slave_status_buffer_wptr);
}

void	rf_link_slave_read_status_stop ()
{
    rf_link_slave_read_status_par_init();
}

int mesh_tx_reliable_rc_rsp_handle(mesh_rc_rsp_t *p_rsp)
{
    int err = 0;
    if(mesh_tx_reliable.busy){
        err = mesh_rsp_handle(p_rsp);
    }
    return err;
}

void	rf_link_slave_read_status_update ()
{
#if GATEWAY_ENABLE
    if(my_fifo_data_cnt_get(&hci_tx_fifo) > 0)
    {
        return;
    }
#endif
    
	while (slave_status_buffer_wptr != slave_status_buffer_rptr)
	{
	    mesh_rc_rsp_t *p_rsp = &slave_rsp_buf[(slave_status_buffer_rptr % slave_status_buffer_num)];
		if (0 == mesh_tx_reliable_rc_rsp_handle(p_rsp)){
            slave_status_buffer_rptr = (slave_status_buffer_rptr+1) % slave_status_buffer_num;
            if(++mesh_tx_reliable.rsp_cnt >= mesh_tx_reliable.mat.rsp_max){
            	if(is_slave_read_status_buf_empty()){	// report the remain node also.
	                mesh_tx_reliable_finish();
	                return;
                }
            }
		}else{
			return;
		}
	}
}

int mesh_rsp_handle(mesh_rc_rsp_t *p_rsp)
{
	static u32 fifo_cnt=0;
	fifo_cnt++;
	LOG_MSG_INFO(TL_LOG_MESH,0,0,"mesh_rsp_handle:add to receive fifo  %d", fifo_cnt);
	int err = 0;
    #if IS_VC_PROJECT
	u8 pu_buf[1024];
	pu_buf[0] = (MESH_CMD_RSP|TSCRIPT_MESH_RX);
	if(p_rsp->len + 3 <= sizeof(pu_buf)){
    	memcpy(pu_buf+1, p_rsp, p_rsp->len + 2);
    	OnAppendLog_vs(pu_buf, p_rsp->len + 3);
	}
	#else
	//err = my_fifo_push_hci_tx_fifo((u8 *)(p_rsp), len+2, 0, 0); // no handle for slave node, now.
	#endif

#if ((IS_VC_PROJECT_MASTER || DONGLE_PROVISION_EN) && (0 == DEBUG_MESH_DONGLE_IN_VC_EN)&&!PTS_TEST_KEY_REFRESH_EN)
	mesh_kr_cfgcl_status_update(p_rsp);
#endif
#if WIN32
    mesh_rsp_handle_proc_win32(p_rsp);
#else
	mesh_rsp_handle_cb(p_rsp);
#endif
	mesh_fast_prov_rsp_handle(p_rsp);

    return err; // always return 0 for VC.
}


int	rf_link_slave_add_status (mesh_rc_rsp_t *p_rsp, u16 op)
{
    int err = -1;
    #if(IS_VC_PROJECT_MASTER && (!DEBUG_SHOW_VC_SELF_EN))
    if(is_cmd_skip_for_vc_self(p_rsp->src, op) && (!is_unicast_adr(mesh_tx_reliable.mat.adr_dst))){
    	return -1;
    }
    #endif

	foreach(i,slave_status_record_idx){
		if((p_rsp->src == slave_status_record[i].src)
		&& (op == slave_status_record[i].op)){
			return 0;
		}
	}

	if(slave_status_record_idx < MESH_ELE_MAX_NUM){
        if(p_rsp->len <= MESH_RSP_LEN_MAX){
            if(((slave_status_buffer_wptr+1) % slave_status_buffer_num) != slave_status_buffer_rptr){
                // record notify data to p_slave_status_buffer
                mesh_rc_rsp_t *p_dst = &slave_rsp_buf[slave_status_buffer_wptr % slave_status_buffer_num];
                slave_status_buffer_wptr = (slave_status_buffer_wptr+1) % slave_status_buffer_num;
                memcpy(p_dst, p_rsp, p_rsp->len + OFFSETOF(mesh_rc_rsp_t, src));
                //memset(p_dst + OFFSETOF(mesh_rc_rsp_t, src) + p_rsp->len, 0, MESH_RSP_LEN_MAX - p_rsp->len);   // for test
                
                // record address to slave_status_record
                slave_status_record[slave_status_record_idx].op = op;
                slave_status_record[slave_status_record_idx++].src = p_rsp->src;
                
                err = 0;
            }
        }
    }
    
	return err;
}

int is_rsp2tx_reliable(u32 op_rsp, u16 adr_src)
{
    if (op_rsp == mesh_tx_reliable.mat.op_rsp) {
        if (is_unicast_adr(mesh_tx_reliable.mat.adr_dst)) {
            if (mesh_tx_reliable.mat.adr_dst == adr_src) {
                return 1;
            }
        }
        else {
            return 1;
        }
    }
    return 0;
}
#endif

int is_support_op(mesh_op_resource_t *op_res, u16 op, u16 adr_dst, u8 tx_flag)
{
    int support_flag = 0;
    //mesh_cmd_ac_sig2_t *p_ac = CONTAINER_OF(par,mesh_cmd_ac_sig2_t,data[0]);
    if(0 == mesh_search_model_id_by_op(op_res, op, tx_flag)){
        if(!is_unicast_adr(adr_dst)){
        	if(is_cfg_model(op_res->id, op_res->sig)){
        	    if(!is_actived_factory_test_mode() || !is_valid_cfg_op_when_factory_test(op)){
        	        #if (! __PROJECT_MESH_PRO__ && !DEBUG_CFG_CMD_GROUP_AK_EN && !DEBUG_CFG_CMD_USE_AK_WHEN_GROUP_EN2)			
                    return 0;   // must unicast for config model
                    #endif
                }
            }
        }
        
        if(op_res->sig){
            op_res->id &= 0xffff;
        }
    
        mesh_adr_list_t adr_list;
        find_ele_support_model_and_match_dst(&adr_list, adr_dst, op_res->id, op_res->sig);
        if(adr_list.adr_cnt){
            support_flag = 1;
            foreach(i,adr_list.adr_cnt){
                u8 model_idx = 0;
                u8 *model = mesh_find_ele_resource_in_model(adr_list.adr[i], op_res->id, op_res->sig,&model_idx, 0);
				if(model){
                    op_res->model_idx[op_res->model_cnt] = model_idx;
                    op_res->model[op_res->model_cnt++] = model;
                }
            }
        }else{
            if(tx_flag || ((0 == tx_flag) && op_res->status_cmd)){
                support_flag = 1;
            }
        }
    }else{
        #if VC_SUPPORT_ANY_VENDOR_CMD_EN
        if(!IS_VENDOR_OP(op))
        #endif
        {
            LOG_MSG_INFO(TL_LOG_COMMON,0,0,"not support op, if TX: send with unreliable flow, or RX: discard!", 0);
        }
    }
    return support_flag;
}

u8 * mesh_get_model_par_by_op_dst(u16 op, u16 ele_adr)
{
	u8 *model = 0;
	mesh_op_resource_t op_res;
	if(0 == mesh_search_model_id_by_op(&op_res, op, 1)){
		u8 model_idx = 0;
		model = mesh_find_ele_resource_in_model(ele_adr, op_res.id, op_res.sig,&model_idx, 0);
	}
	return model;
}

int mesh_rc_data_layer_access2(u8 *ac, int len_ac, mesh_cmd_nw_t *p_nw)
{
    int err = -1;
    u16 adr_src = p_nw->src;
    u16 adr_dst = p_nw->dst;
    u16 op;
    u8 *params;
    int par_len;
	LAYER_PARA_DEBUG(A_debug_access_layer_enter);
	#if (TL_LOG_SEL_VAL	 & (BIT(TL_LOG_NODE_SDK)))
	ut_log_t ut_log;
	int log_len = len_ac + 4;
	ut_log.src = adr_src;
	ut_log.dst = adr_dst;
	if(log_len > sizeof(ut_log.data)){
	    log_len = sizeof(ut_log.data);
	}
	memcpy(ut_log.data, ac, log_len);
    LOG_MSG_INFO(TL_LOG_NODE_SDK_NW_UT,(u8 *)&ut_log,log_len,"UT PDU:",0);
    #endif
    
    int get_op_st = rf_link_get_op_para(ac, len_ac, &op, &params, &par_len);
    if(GET_OP_FAILED == get_op_st){
		LAYER_PARA_DEBUG(A_debug_access_layer_opcode_ret);
		LOG_MSG_ERR(TL_LOG_MESH,ac, len_ac ,"access_layer_opcode err:",0);
        return OP_AGG_WRONG_OP;
    }else if(GET_OP_SUCCESS_EXTEND == get_op_st){
        LOG_MSG_LIB(TL_LOG_NODE_BASIC, ac, OP_TYPE_VENDOR,"RX remove extend op",0);
    }

    #if (WIN32 && (!DEBUG_SHOW_VC_SELF_EN))
    if(is_cmd_skip_for_vc_self(adr_src, op)){
        // LOG_MSG_ERR(TL_LOG_MESH,0, 0 ,"is vc self adr",0); // don't print as normal.
    	return 0;
    }
    #endif
    
    #if WIN32
    if(!is_own_ele(adr_src)){
        LOG_MSG_INFO(TL_LOG_NODE_BASIC,ac,len_ac,"src:%04x,dst:%04x,ac RX:%04x(%s)",adr_src, adr_dst, op, get_op_string(op,0));
    }
	#endif

    #if 0
	static u32 ts_B_11;
	static u8 ts_B_11_ac[20][16];
	u32 len_log = len_ac;
	if(len_log > 16){
	    len_log = 16;
	}
	memset(ts_B_11_ac[(ts_B_11)%ARRAY_SIZE(ts_B_11_ac)], 0, 16);
	memcpy(ts_B_11_ac[(ts_B_11++)%ARRAY_SIZE(ts_B_11_ac)], ac, len_log);
	#endif

    int is_support_flag = 0;
    int is_status_cmd = 0;

    #if (VC_SUPPORT_ANY_VENDOR_CMD_EN && !GATEWAY_ENABLE)
    if(IS_VENDOR_OP(op)){
        is_support_flag = 1;
        if(is_own_ele(adr_dst) || is_fixed_group(adr_dst)){
            is_status_cmd = 1;  // because reliable par may have been clear here.
            mesh_cb_fun_par_vendor_t cb_par = {0};
            cb_par.adr_src = adr_src;
            cb_par.adr_dst = adr_dst;
            cb_par.retransaction = 0;
            cb_app_vendor_all_cmd((mesh_cmd_ac_vd_t *)ac, len_ac, &cb_par);
        }
    }else
    #endif
    {
    if(IS_VENDOR_OP(op)){
   	 	g_msg_vd_id = ac[1] + (ac[2]<<8);
		#if GATEWAY_ENABLE
		if(is_rsp2tx_reliable(op|(g_msg_vd_id<<8), adr_src)){
			is_support_flag = 1;
			is_status_cmd = 1;
		}
		#endif
    }
    mesh_op_resource_t op_res;
    if(is_support_op(&op_res, op, adr_dst, 0)){
        is_support_flag = 1;
        if(is_rx_need_extend_invalid_model(op_res.id, get_op_st)){ // id has been SIG here.
            return -1;
        }
        
		LAYER_PARA_DEBUG(A_debug_access_layer_support_model);
        if(op_res.model_cnt){
            u8 retransaction = 0;
            
            foreach(i,op_res.model_cnt){
                model_common_t *p_model = (model_common_t *)op_res.model[i];
				
                if(is_use_device_key(op_res.id, op_res.sig)){	// cfg model
                	#if (DEBUG_CFG_CMD_GROUP_AK_EN || DEBUG_CFG_CMD_USE_AK_WHEN_GROUP_EN2)
                    if(DEBUG_CFG_CMD_GROUP_USE_AK(adr_dst)){
                    }else
                    #endif
                	if(SECURITY_ENABLE && (!is_own_ele(adr_src))){
                	    if(mesh_key.appkey_sel_dec < APP_KEY_MAX){ // decode by app key
                	    	#if (DRAFT_FEATURE_VENDOR_TYPE_SEL == DRAFT_FEATURE_VENDOR_TYPE_ONE_OP)
                	        if(!is_client_tx_extend_model(op_res.id))
                	        #endif
                	        {
                                LOG_MSG_ERR(TL_LOG_MESH,0, 0 ,"should not use app key to decryption config model",0);
                    		    return OP_AGG_WRONG_KEY;	// may be attacked.
                	        }
                	    }else if(!is_actived_factory_test_mode() && (mesh_key.devkey_self_dec != DEC_BOTH_TWO_DEV_KEY)){
                	        /*(!(((SIG_MD_CFG_SERVER == op_res.id) && (1 == mesh_key.devkey_self_dec))
                	                            &&((SIG_MD_CFG_CLIENT == op_res.id) && (0 == mesh_key.devkey_self_dec))))*/
                	        if(op_res.id == mesh_key.devkey_self_dec){	// model id is for RX node
                                LOG_MSG_ERR(TL_LOG_MESH,0, 0 ,"use wrong device key",0);
                		        return OP_AGG_WRONG_KEY;	// may be attacked.
                		    }
                		}
                	}
                	
                	if(is_cfg_model(op_res.id, op_res.sig) && !is_valid_cfg_op_when_factory_test(op)){
                        LOG_MSG_ERR(TL_LOG_MESH,0, 0 ,"factory mode unsupport this op:0x%04x",op);
                	    return -1;
                	}
                }else{
                	#if MD_OP_AGG_EN
                	if(!is_op_agg_model(op_res.id))
					#endif
					{
	                	if(mesh_key.appkey_sel_dec >= APP_KEY_MAX){
							LOG_MSG_ERR(TL_LOG_MESH,0, 0 ,"should not use device for op:0x%04x",op);
							return OP_AGG_WRONG_KEY; 
						}
	                    // just compare old key is enough, because bind key of old key is alway valid and same with new, if existed.
						mesh_app_key_t *app_key = &mesh_key.net_key[mesh_key.netkey_sel_dec][0].app_key[mesh_key.appkey_sel_dec];
		                if(!is_exist_bind_key(p_model, app_key->index) && !is_exist_bind_key_extend_op(app_key->index)){
	                        LOG_MSG_ERR(TL_LOG_MESH,0, 0 ,"model 0x%4x did't bind this key in element index %d",op_res.id, i);
							err = OP_AGG_WRONG_KEY; 
							continue ;
		                }
                	}
                }

				#if GATEWAY_ENABLE
				u8 is_valid_addr = 0;
				if(p_access_layer_dst_addr_cb && !is_unicast_adr(adr_dst)){
					is_valid_addr = p_access_layer_dst_addr_cb(p_nw);
				}
				#endif
				
                if(is_unicast_adr(adr_dst)
                || is_fixed_group(adr_dst)  // have been checked feature before in mesh_match_group_mac_()
                || is_subscription_adr(p_model, adr_dst)
				#if GATEWAY_ENABLE
				|| is_valid_addr
				#endif
				){
                    if(op_res.cb){
                        mesh_cb_fun_par_t cb_par;
                        cb_par.model = op_res.model[i];
                        cb_par.model_idx = op_res.model_idx[i];;
                        cb_par.adr_src = adr_src;
                        cb_par.adr_dst = adr_dst;
                        cb_par.op = op;

                        #if 1
                        if(0 == i){ // check only once
                            u8 tid_pos;
                            if(
                            #if (!DEBUG_SHOW_VC_SELF_EN)
                            (adr_src != adr_dst) && 
                            #endif
                            is_cmd_with_tid(&tid_pos, op, params, TID_NONE_VENDOR_OP_VC)){
                                LAYER_PARA_DEBUG(A_debug_access_layer_tid_ok);
                                if(is_retransaction(adr_src, params[tid_pos])){
                                    retransaction = 1;
                                }
                            }
                        }
                        #endif
                        cb_par.retransaction = retransaction;
                        cb_par.op_rsp = op_res.op_rsp;
                        cb_par.res = (u8 *)&op_res;
                        cb_par.p_nw = p_nw;
                        err = mesh_rc_data_layer_access_cb(params, par_len, &cb_par);
                        if(err){
                            LOG_MSG_ERR(TL_LOG_MESH,0, 0 ,"mesh_rc_data_layer_access_cb error! op:%04x, err: %d", op, err);
                        }
                    }
                }
            }
        }else{
            if(op_res.status_cmd){  // always action for status message
                if(op_res.cb){
					#if MESH_RX_TEST
					mesh_cb_fun_par_t cb_par = {0};
					cb_par.adr_src = adr_src;
	                cb_par.op = op;
                    err = op_res.cb(params, par_len, &cb_par);
					#else
					    #if ((__PROJECT_MESH_PRO__ || __PROJECT_MESH_GW_NODE__)&& (!DEBUG_SHOW_VC_SELF_EN))
					err = op_res.cb(params, par_len, 0);    // should not for firmware
                    if(err){
                        LOG_MSG_ERR(TL_LOG_MESH,0, 0 ,"prov mesh_rc_data_layer_access_cb error! op:%04x",op);
                    }
                    	#else
						// LOG_MSG_ERR(TL_LOG_MESH,0, 0 ,"no model match! op:%04x",op); // it would print too many logs.
					    #endif
					#endif
                }
            }
        }

        is_status_cmd = op_res.status_cmd;
    }else{
		err = OP_AGG_WRONG_OP;
        #if (GATEWAY_ENABLE)
        if(IS_VENDOR_OP(op) && is_own_ele(adr_src)){
            // not output error for set command from itself, when addr dst is 0xffff.
        }else
        #endif
        {
            LOG_MSG_ERR(TL_LOG_MESH,0, 0 ,"mesh_rc_data_layer_access: not support op or model is not enable! src:0x%x dst:0x%x op:0x%x(%s)", p_nw->src, p_nw->dst, op, get_op_string(op,0));
        }
    }
    }

    #if RELIABLE_CMD_EN
    u32 op_rsp = op;    // include vendor id
    
    #if VC_SUPPORT_ANY_VENDOR_CMD_EN
    if(IS_VENDOR_OP(op)){
        memcpy(&op_rsp,ac,3);
    }
    #endif
    
    if(is_support_flag && is_status_cmd){
        // check more details later
        mesh_rc_rsp_t rc_rsp;
        if(len_ac <= sizeof(rc_rsp.data)){
            rc_rsp.src = adr_src;
            rc_rsp.dst = adr_dst;
            rc_rsp.len = len_ac + (OFFSETOF(mesh_rc_rsp_t,data) - OFFSETOF(mesh_rc_rsp_t,src));

            u32 remove_len = is_op_need_extend_and_remove_op_par(rc_rsp.data, ac, len_ac);
            if(remove_len){
                rc_rsp.len -= remove_len;
            }else{
                memcpy(rc_rsp.data, ac, len_ac);
            }
            
            if(mesh_tx_reliable.busy && is_rsp2tx_reliable(op_rsp, adr_src)){
                rf_link_slave_add_status(&rc_rsp, op);
            }else{
            	//if(rc_rsp.len + 2 <= 100){	// USB buf size:64   // hci_tx_fifo.size
                	mesh_rsp_handle(&rc_rsp);
                //}
            }
        }
    }
    #endif

    return err;
}

int mesh_rc_data_layer_access(u8 *ac, int len_ac, mesh_cmd_nw_t *p_nw)
{
	int err = mesh_rc_data_layer_access2(ac, len_ac, p_nw);
	g_msg_vd_id = g_vendor_id;
	return err;
}

// TID process
void add2tid_cache(u16 adr, u8 tid, int idx){
    mesh_tid.rx[idx].adr = adr;
    mesh_tid.rx[idx].tid = tid;
    mesh_tid.rx[idx].tick_100ms = clock_time_100ms();
}

u32 get_ele_idx(u16 ele_adr)
{
	if(is_own_ele(ele_adr)){
		return (ele_adr - ele_adr_primary);
	}
	return -1;
}

void is_cmd_with_tid_check_and_set(u16 ele_adr, u16 op, u8 *par, u32 par_len, u8 tid_pos_vendor_app)
{
    u8 tid_pos;
    if(is_cmd_with_tid(&tid_pos, op, par, tid_pos_vendor_app)){
        if(0 == par[tid_pos]){  // user can define TID
        	u32 ele_idx = get_ele_idx(ele_adr);
        	if(ele_idx < g_ele_cnt){
                mesh_tid.tx[ele_idx]++;
                if(tid_pos < par_len){
                    par[tid_pos] = mesh_tid.tx[ele_idx];
                }

                mesh_tid_save(ele_idx);
            }
        }
    }
}

int is_retransaction(u16 adr, u8 tid){
    foreach(i, RX_TID_CNT){
        if(adr == mesh_tid.rx[i].adr){
            if(tid == mesh_tid.rx[i].tid){
                return 1;
            }else{
                add2tid_cache(adr, tid, i);            // update
                return 0;
            }
        }
    }

    //new device found
    static u32 tid_cache_idx = 0;
    add2tid_cache(adr, tid, tid_cache_idx);
    tid_cache_idx = (tid_cache_idx + 1) % RX_TID_CNT;
    return 0;
}

void mesh_tid_timeout_check(){
    foreach(i, RX_TID_CNT){
        if(mesh_tid.rx[i].adr){
            if(clock_time_exceed_100ms(mesh_tid.rx[i].tick_100ms, 60)){
                add2tid_cache(0, 0, i);            // clear
            }
        }
    }
}

