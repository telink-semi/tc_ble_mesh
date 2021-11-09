#include"telink_sdk_mesh_api.h"
#include"mible_mesh_api.h"
#include"mible_log.h"
#include "proj_lib/sig_mesh/app_mesh.h"
#include "vendor/common/lighting_model.h"
#if MI_API_ENABLE

void process_mesh_node_init_event(void)
{
		mible_mesh_template_map_t node_map[5] = {
			[0] = {
				.siid = 0,
				.piid = 0,
				.model_id = MIBLE_MESH_MIOT_SPEC_SERVER_MODEL,
				.company_id = MIBLE_MESH_COMPANY_ID_XIAOMI,
				.element = 0,
				.appkey_idx = 0,
			},
			[1] = {
				.siid = 0,
				.piid = 0,
				.model_id = MIBLE_MESH_MIJIA_SERVER_MODEL,
				.company_id = MIBLE_MESH_COMPANY_ID_XIAOMI,
				.element = 0,
				.appkey_idx = 0,
			},
#if defined(MI_MESH_TEMPLATE_LIGHTNESS) || defined(MI_MESH_TEMPLATE_LIGHTCTL) || defined(MI_MESH_TEMPLATE_ONE_KEY_SWITCH) \
	|| defined(MI_MESH_TEMPLATE_TWO_KEY_SWITCH) || defined(MI_MESH_TEMPLATE_THREE_KEY_SWITCH) || defined(MI_MESH_TEMPLATE_FAN)
			[2] = {
				.siid = 2,
				.piid = 1,
				.model_id = MIBLE_MESH_MODEL_ID_GENERIC_ONOFF_SERVER,
				.company_id = MIBLE_MESH_COMPANY_ID_SIG,
				.element = 0,
				.appkey_idx = 0,
			},
#if defined(MI_MESH_TEMPLATE_TWO_KEY_SWITCH) || defined(MI_MESH_TEMPLATE_THREE_KEY_SWITCH)
			[3] = {
				.siid = 3,
				.piid = 1,
				.model_id = MIBLE_MESH_MODEL_ID_GENERIC_ONOFF_SERVER,
				.company_id = MIBLE_MESH_COMPANY_ID_SIG,
				.element = 1,
				.appkey_idx = 0,
			},
    #if defined(MI_MESH_TEMPLATE_THREE_KEY_SWITCH)
			[4] = {
				.siid = 4,
				.piid = 1,
				.model_id = MIBLE_MESH_MODEL_ID_GENERIC_ONOFF_SERVER,
				.company_id = MIBLE_MESH_COMPANY_ID_SIG,
				.element = 2,
				.appkey_idx = 0,
			},
    #else
			[4] = {0},
    #endif
#elif defined(MI_MESH_TEMPLATE_LIGHTNESS) || defined(MI_MESH_TEMPLATE_LIGHTCTL)
			[3] = {
				.siid = 2,
				.piid = 2,
				.model_id = MIBLE_MESH_MODEL_ID_LIGHTNESS_SERVER,
				.company_id = MIBLE_MESH_COMPANY_ID_SIG,
				.element = 0,
				.appkey_idx = 0,
			},
    #if defined(MI_MESH_TEMPLATE_LIGHTCTL)
			[4] = {
				.siid = 2,
				.piid = 3,
				.model_id = MIBLE_MESH_MODEL_ID_CTL_TEMPEATURE_SERVER,
				.company_id = MIBLE_MESH_COMPANY_ID_SIG,
				.element = 1,
				.appkey_idx = 0,
			},
    #else
			[4] = {0},
    #endif
#else
			[3] = {0},
			[4] = {0},
#endif
#else
			[2] = {0},
			[3] = {0},
			[4] = {0},
#endif
		};

    mible_mesh_node_init_t node_info;
    node_info.map = (mible_mesh_template_map_t *)&node_map;
    node_info.provisioned = get_provision_state();
    node_info.lpn_node = 0;
	if(node_info.provisioned){
		node_info.address = ele_adr_primary;
		node_info.ivi = mesh_get_ivi_cur_val_little();
	}else{
		node_info.address = 0;
		node_info.ivi = 0;	
	}
	MI_LOG_WARNING("[DEVICE_INIT_DONE]\n");
    mible_mesh_event_callback(MIBLE_MESH_EVENT_DEVICE_INIT_DONE, &node_info);
}

void process_mesh_ivi_update()
{
	mible_mesh_iv_t mesh_iv;
	mesh_ctl_fri_update_flag_t *p_flag =(mesh_ctl_fri_update_flag_t *)&(mesh_iv.flags);
	mesh_iv.iv_index = mesh_get_ivi_cur_val_little();
	p_flag->IVUpdate = (IV_UPDATE_STEP1 == iv_idx_st.update_proc_flag);
	MI_LOG_WARNING("[DEVICE_IVI_UPDATE]\n");
	mible_mesh_event_callback(MIBLE_MESH_EVENT_IV_UPDATE, &mesh_iv);
}

void process_mesh_node_reset()
{
    mible_mesh_event_params_t evt_vendor_param;
    evt_vendor_param.config_msg.opcode.opcode = MIBLE_MESH_MSG_CONFIG_NODE_RESET;
    evt_vendor_param.config_msg.opcode.company_id = MIBLE_MESH_COMPANY_ID_SIG;
    mible_mesh_event_callback(MIBLE_MESH_EVENT_CONFIG_MESSAGE_CB, &evt_vendor_param);
}

// need to proc the reset part and the node init part .

int32_t generic_on_off_server_data(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    mible_mesh_event_params_t evt_generic_param;
	u16 op = mesh_mi_trans_op_little2big(cb_par->op);
    memset(&evt_generic_param.generic_msg, 0, sizeof(mible_mesh_access_message_t));
    evt_generic_param.generic_msg.opcode.opcode = op;
	if(op == MIBLE_MESH_MSG_GENERIC_ONOFF_SET || 
       op == MIBLE_MESH_MSG_GENERIC_ONOFF_SET_UNACKNOWLEDGED){
	    evt_generic_param.generic_msg.buf_len = par_len;
	    memcpy(evt_generic_param.generic_msg.buff, par, par_len);
    }
    evt_generic_param.generic_msg.opcode.company_id = MIBLE_MESH_COMPANY_ID_SIG;
    evt_generic_param.generic_msg.meta_data.dst_addr = cb_par->adr_dst;
    evt_generic_param.generic_msg.meta_data.src_addr = cb_par->adr_src;
	if(is_unicast_adr(cb_par->adr_dst)){
		evt_generic_param.generic_msg.meta_data.elem_index = cb_par->adr_dst-ele_adr_primary;
	}else{
		evt_generic_param.generic_msg.meta_data.elem_index = 0;
	}
    mible_mesh_event_callback(MIBLE_MESH_EVENT_GENERIC_MESSAGE_CB, &evt_generic_param);
    return 0;
}

int32_t light_lightness_server_data(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    mible_mesh_event_params_t evt_generic_param;
	u16 op = mesh_mi_trans_op_little2big(cb_par->op);
    memset(&evt_generic_param.generic_msg, 0, sizeof(mible_mesh_access_message_t));
    evt_generic_param.generic_msg.opcode.opcode = op;
	if(op == MIBLE_MESH_MSG_LIGHT_LIGHTNESS_SET || 
       op == MIBLE_MESH_MSG_LIGHT_LIGHTNESS_SET_UNACKNOWLEDGED){
	    evt_generic_param.generic_msg.buf_len = par_len;
	    memcpy(evt_generic_param.generic_msg.buff, par, par_len);
    }
    evt_generic_param.generic_msg.opcode.company_id = MIBLE_MESH_COMPANY_ID_SIG;
    evt_generic_param.generic_msg.meta_data.dst_addr = cb_par->adr_dst;
    evt_generic_param.generic_msg.meta_data.src_addr = cb_par->adr_src;
    if(is_unicast_adr(cb_par->adr_dst)){
		evt_generic_param.generic_msg.meta_data.elem_index = cb_par->adr_dst-ele_adr_primary;
	}else{
		evt_generic_param.generic_msg.meta_data.elem_index = 0;
	}
    mible_mesh_event_callback(MIBLE_MESH_EVENT_GENERIC_MESSAGE_CB, &evt_generic_param);
    return 0;
}

int32_t light_ctl_server_data(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    mible_mesh_event_params_t evt_generic_param;
    u16 op = mesh_mi_trans_op_little2big(cb_par->op);
    memset(&evt_generic_param.generic_msg, 0, sizeof(mible_mesh_access_message_t));
    evt_generic_param.generic_msg.opcode.opcode = op;
    if(op == MIBLE_MESH_MSG_LIGHT_CTL_TEMPERATURE_SET || 
       op == MIBLE_MESH_MSG_LIGHT_CTL_TEMPERATURE_SET_UNACKNOWLEDGED){
        evt_generic_param.generic_msg.buf_len = par_len;
        memcpy(evt_generic_param.generic_msg.buff, par, par_len);
    }
    evt_generic_param.generic_msg.opcode.company_id = MIBLE_MESH_COMPANY_ID_SIG;
    evt_generic_param.generic_msg.meta_data.dst_addr = cb_par->adr_dst;
    evt_generic_param.generic_msg.meta_data.src_addr = cb_par->adr_src;
    if(is_unicast_adr(cb_par->adr_dst)){
		evt_generic_param.generic_msg.meta_data.elem_index = cb_par->adr_dst-ele_adr_primary;
	}else{
		evt_generic_param.generic_msg.meta_data.elem_index = 1;
	}
    mible_mesh_event_callback(MIBLE_MESH_EVENT_GENERIC_MESSAGE_CB, &evt_generic_param);
    return 0;
}

/**
 * @brief miot server (038F0000) data callback
 * @param[in] pmodel_info: light lightness server model handler
 * @param[in] type: data type
 * @param[in, out] pargs: data need to process
 */
int32_t miot_server_data(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    mible_mesh_event_params_t evt_vendor_param;
    memset(&evt_vendor_param.generic_msg, 0, sizeof(mible_mesh_access_message_t));
    evt_vendor_param.generic_msg.opcode.opcode = cb_par->op;
    evt_vendor_param.generic_msg.buf_len = par_len;
    if(evt_vendor_param.generic_msg.buf_len > 8)
        evt_vendor_param.generic_msg.p_buf = par;
    else
        memcpy(evt_vendor_param.generic_msg.buff, par, evt_vendor_param.generic_msg.buf_len);
    evt_vendor_param.generic_msg.opcode.company_id = MIBLE_MESH_COMPANY_ID_XIAOMI;
    evt_vendor_param.generic_msg.meta_data.dst_addr = cb_par->adr_dst;
    evt_vendor_param.generic_msg.meta_data.src_addr = cb_par->adr_src;
    evt_vendor_param.generic_msg.meta_data.elem_index = 0;
    mible_mesh_event_callback(MIBLE_MESH_EVENT_GENERIC_MESSAGE_CB, &evt_vendor_param);
    return 0;
}

/**
 * @brief mijia server (038F0002) data callback
 * @param[in] pmodel_info: light lightness server model handler
 * @param[in] type: data type
 * @param[in, out] pargs: data need to process
 */
int32_t mijia_server_data(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    return 0;
}

int cfg_server_receive_peek(u8 *par, int par_len,mesh_cb_fun_par_t *cb_par)
{
    int ret =1;   
    mible_mesh_event_params_t evt_vendor_param;
	u16 op = mesh_mi_trans_op_little2big(cb_par->op);
    memset(&evt_vendor_param.config_msg, 0, sizeof(mible_mesh_config_status_t));
    evt_vendor_param.config_msg.opcode.opcode = op;
    evt_vendor_param.config_msg.opcode.company_id = MIBLE_MESH_COMPANY_ID_SIG;
    evt_vendor_param.config_msg.meta_data.dst_addr = cb_par->adr_dst;
    evt_vendor_param.config_msg.meta_data.src_addr = cb_par->adr_src;
    evt_vendor_param.config_msg.meta_data.elem_index = cb_par->adr_dst-ele_adr_primary;
    
    switch (op)
    {
    case MIBLE_MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_OVERWRITE:
    case MIBLE_MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_DELETE:
    {
    	mesh_cfg_model_sub_set_t *p_sub_set = (mesh_cfg_model_sub_set_t *)(par);
        evt_vendor_param.config_msg.model_sub_set.elem_addr = p_sub_set->ele_adr;
        evt_vendor_param.config_msg.model_sub_set.address = p_sub_set->sub_adr;
        if(par_len == sizeof(mesh_cfg_model_sub_set_t)){
            evt_vendor_param.config_msg.model_sub_set.model_id.model_id = (p_sub_set->model_id) >> 16;
            evt_vendor_param.config_msg.model_sub_set.model_id.company_id = (p_sub_set->model_id) & 0xffff;
        }else{
            evt_vendor_param.config_msg.model_sub_set.model_id.model_id = (p_sub_set->model_id) & 0xffff;
            evt_vendor_param.config_msg.model_sub_set.model_id.company_id = MIBLE_MESH_COMPANY_ID_SIG;
        }

        mible_mesh_event_callback(MIBLE_MESH_EVENT_CONFIG_MESSAGE_CB, &evt_vendor_param);
    }
        break;
    case MIBLE_MESH_MSG_CONFIG_RELAY_SET:
    {
        mesh_cfg_model_relay_set_t *p_set = (mesh_cfg_model_relay_set_t *)par;
        evt_vendor_param.config_msg.relay_set.relay = p_set->relay;
        evt_vendor_param.config_msg.relay_set.relay_retrans_cnt = p_set->transmit.count;
        evt_vendor_param.config_msg.relay_set.relay_retrans_intvlsteps = p_set->transmit.invl_steps;
        mible_mesh_event_callback(MIBLE_MESH_EVENT_CONFIG_MESSAGE_CB, &evt_vendor_param);
    }
        break;
    default:
        break;
    }
    return ret;
}

/**
 *@brief    deinit mesh stack.
 *          report event: MIBLE_MESH_EVENT_STACK_DEINIT_DONE, data: NULL.
 *@return   0: success, negetive value: failure
 */
int mible_mesh_device_deinit_stack(void)
{
    return 0;
}
/**
 *@brief    async method, init mesh device
 *          load self info, include unicast address, iv, seq_num, init model;
 *          clear local db, related appkey_list, netkey_list, device_key_list,
 *          we will load latest data for cloud;
 *          report event: MIBLE_MESH_EVENT_DEVICE_INIT_DONE, data: NULL.
 *@param    [in] info : init parameters corresponding to gateway
 *@return   0: success, negetive value: failure
 */
int mible_mesh_device_init_node(void)
{
    /* send event in process_mesh_node_init_event */
    /** start mesh stack */
    return 0;
}

/**
 *@brief    set node provsion data.
 *@param    [in] param : prov data include devkey, netkey, netkey idx,
 *          uni addr, iv idx, key flag
 *@return   0: success, negetive value: failure
 */
int mible_mesh_device_set_provsion_data(mible_mesh_provisioning_data_t *param)
{
	set_dev_key(param->devkey);
	provison_net_info_str prov_net;
	prov_net.unicast_address = param->address;
	prov_net.flags = param->flags;
	prov_net.key_index = param->net_idx;
	memcpy(prov_net.net_work_key,param->netkey,sizeof(param->netkey));
	memcpy(prov_net.iv_index,(u8*)&(param->iv),4);
	endianness_swap_u32(prov_net.iv_index);
	prov_para.provison_rcv_state = STATE_PRO_SUC;
	#if DUAL_VENDOR_EN
		provision_mag.dual_vendor_st = DUAL_VENDOR_ST_MI;
	#endif
	mesh_provision_par_handle((u8 *)&prov_net);
	mesh_node_prov_event_callback(EVENT_MESH_NODE_RC_LINK_SUC);
	process_mesh_node_init_event();
    return 0;
}

/**
 *@brief    mesh provsion done. need update node info and
 *          callback MIBLE_MESH_EVENT_DEVICE_INIT_DONE event
 *@return   0: success, negetive value: failure
 */
void *mible_conn_timer = NULL;


int mible_mesh_device_provsion_done(void)
{
    mible_timer_start(mible_conn_timer, MI_REGSUCC_TIMEOUT, NULL);
    return 0;
}

/**
 *@brief    reset node, 4.3.2.53 Config Node Reset, Report 4.3.2.54 Config Node Reset Status.
 *          report event: MIBLE_MESH_EVENT_CONFIG_MESSAGE_CB, data: mible_mesh_config_status_t.
 *@return   0: success, negetive value: failure
 */
int mible_mesh_node_reset(void)
{
    // erase mesh data
    factory_reset();
	light_ev_with_sleep(4, 200*1000);// save time for the node reset
    return 0;
}

/**
 *@brief    mesh unprovsion done. need update node info and
 *          callback MIBLE_MESH_EVENT_DEVICE_INIT_DONE event
 *@return   0: success, negetive value: failure
 */
int mible_mesh_device_unprovsion_done(void)
{
    return mible_reboot();
}

/**
 *@brief    mesh login done.
 *@return   0: success, negetive value: failure
 */
int mible_mesh_device_login_done(uint8_t status)
{
    mible_timer_stop(mible_conn_timer);
    if(status){
        MI_LOG_INFO("LOGIN SUCCESS, stop TIMER_ID_CONN_TIMEOUT\n");
    }else{
        MI_LOG_INFO("LOGIN FAIL\n");
        mible_conn_timeout_cb(NULL);
    }
    return 0;
}

/**
 *@brief    set local provisioner network transmit params.
 *@param    [in] count : advertise counter for every adv packet, adv transmit times
 *@param    [in] interval_steps : adv interval = interval_steps*0.625ms
 *@return   0: success, negetive value: failure
 */
int mible_mesh_device_set_network_transmit_param(uint8_t count, uint8_t interval_steps)
{
    MI_LOG_WARNING("[mible_mesh_gateway_set_network_transmit_param] \n");
    // TODO: Store in rom
    mesh_transmit_t transmit;
	transmit.invl_steps = interval_steps;
	transmit.count = count;
    memcpy(&model_sig_cfg_s.nw_transmit, (u8*)&transmit, sizeof(mesh_transmit_t));
	mesh_model_store_cfg_s();
    return 0;
}

/**
 *@brief    set node relay onoff.
 *@param    [in] enabled : 0: relay off, 1: relay on
 *@param    [in] count: Number of relay transmissions beyond the initial one. Range: 0-7
 *@param    [in] interval: Relay retransmit interval steps. 10*(1+steps) milliseconds. Range: 0-31.
 *@return   0: success, negetive value: failure
 */
int mible_mesh_device_set_relay(uint8_t enabled, uint8_t count, uint8_t interval)
{
	mesh_transmit_t relay;
	relay.count = count;
	relay.invl_steps = interval;
	if(enabled){
		memcpy(&model_sig_cfg_s.relay_retransmit,(u8*)&relay, sizeof(model_sig_cfg_s.relay_retransmit));
		mesh_model_store_cfg_s();
	}
    return 0;
}

/**
 *@brief    get node relay state.
 *@param    [out] enabled : 0: relay off, 1: relay on
 *@param    [out] count: Number of relay transmissions beyond the initial one. Range: 0-7
 *@param    [out] interval: Relay retransmit interval steps. 10*(1+steps) milliseconds. Range: 0-31.
 *@return   0: success, negetive value: failure
 */
int mible_mesh_device_get_relay(uint8_t *enabled, uint8_t *count, uint8_t *step)
{
    *enabled = model_sig_cfg_s.relay;
    *count = model_sig_cfg_s.relay_retransmit.count;
    *step = model_sig_cfg_s.relay_retransmit.invl_steps;
    MI_LOG_WARNING("relay\t stat %d\t cnt %d\t interval %d\n", *enabled, *count, (*step+1)*10);
    return 0;
}

/**
 *@brief    get seq number.
 *@param    [in] element : model element
 *@param    [out] seq : current sequence numer
 *@param    [out] iv_index : current IV Index
 *@param    [out] flags : IV Update Flag
 *@return   0: success, negetive value: failure
 */
int mible_mesh_device_get_seq(uint16_t element, uint32_t* seq, uint32_t* iv, uint8_t* flags)
{
    if (is_provision_success()){
        if(NULL != seq){
			*seq = mesh_adv_tx_cmd_sno;
		}
        if(NULL != iv){
			
            *iv = (uint32_t)mesh_get_ivi_cur_val_little();
        }
        if(NULL != flags){
            *flags = iv_idx_st.rx_update;
        }
        MI_LOG_DEBUG("Seq %u, Iv %u, flags %u\n", mesh_adv_tx_cmd_sno,
                        *iv, iv_idx_st.rx_update);
        return 0;
    }
    return -1;
}

/**
 *@brief    update iv index, .
 *@param    [in] iv_index : current IV Index
 *@param    [in] flags : contains the Key Refresh Flag and IV Update Flag
 *@return   0: success, negetive value: failure
 */
int mible_mesh_device_update_iv_info(uint32_t iv_index, uint8_t flags)
{
    MI_LOG_WARNING("[mible_mesh_gateway_update_iv_info]  \n");
    /* store iv index */
	u32 ivi_big = iv_index;
	endianness_swap_u32((u8*)&ivi_big);
	iv_idx_st.searching_flag =1;// force to accept the para part 
    iv_update_key_refresh_rx_handle((mesh_ctl_fri_update_flag_t *)&flags, (u8*)&ivi_big);
    return 0;
}

/**
 *@brief    add/delete local netkey.
 *@param    [in] op : add or delete
 *@param    [in] netkey_index : key index for netkey
 *@param    [in] netkey : netkey value
 *@param    [in|out] stack_netkey_index : [in] default value: 0xFFFF, [out] stack generates netkey_index
 *          if your stack don't manage netkey_index and stack_netkey_index relationships, update stack_netkey_index.
 *@return   0: success, negetive value: failure
 */
int mible_mesh_device_set_netkey(mible_mesh_op_t op, uint16_t netkey_index, uint8_t *netkey)
{
    MI_LOG_WARNING("[mible_mesh_gateway_set_netkey] \n");
    uint8_t key[16];
    memcpy(key, netkey, sizeof(key));
    if(op == MIBLE_MESH_OP_ADD){
        /* store netkey */
        mesh_net_key_set(NETKEY_ADD,key,netkey_index, 1);
    }else{
        mesh_net_key_set(NETKEY_DEL,key,netkey_index, 1);
    }
    return 0;
}

/**
 *@brief    add/delete local appkey.
 *@param    [in] op : add or delete
 *@param    [in] netkey_index : key index for netkey
 *@param    [in] appkey_index : key index for appkey
 *@param    [in] appkey : appkey value
 *@param    [in|out] stack_appkey_index : [in] default value: 0xFFFF, [out] stack generates appkey_index
 *          if your stack don't manage appkey_index and stack_appkey_index relationships, update stack_appkey_index.
 *@return   0: success, negetive value: failure
 */
int mible_mesh_device_set_appkey(mible_mesh_op_t op, uint16_t netkey_index, uint16_t appkey_index, uint8_t * appkey)
{
    MI_LOG_WARNING("[mible_mesh_gateway_set_appkey] \n");
    uint8_t key[16];
    memcpy(key, appkey, sizeof(key));
    if(op == MIBLE_MESH_OP_ADD){
        mesh_app_key_set(APPKEY_ADD,key,appkey_index,netkey_index, 1);
    }else{
        mesh_app_key_set(APPKEY_DEL,key,appkey_index,netkey_index, 1);
    }
    return 0;
}

/**
 *@brief    bind/unbind model app.
 *@param    [in] op : bind is MIBLE_MESH_OP_ADD, unbind is MIBLE_MESH_OP_DELETE
 *@param    [in] company_id: company id
 *@param    [in] model_id : model_id
 *@param    [in] appkey_index : key index for appkey
 *@return   0: success, negetive value: failure
 */
int mible_mesh_device_set_model_app(mible_mesh_op_t op, uint16_t elem_index, uint16_t company_id, uint16_t model_id, uint16_t appkey_index)
{
    uint16_t result = 0;
	u8 sig_model;
	u32 model_id_res;
	if(company_id == MIBLE_MESH_COMPANY_ID_XIAOMI){
		sig_model = 0;
		model_id_res = (company_id)|(model_id <<16);
	}else{
		sig_model = 1;
		model_id_res = model_id;
	}

    if(op == MIBLE_MESH_OP_ADD){
        MI_LOG_DEBUG("mi_appkey_bind: bind model(0x%08x-%d) appkey index(%d)", model_id, elem_index, appkey_index);
        mesh_appkey_bind(MODE_APP_BIND, ele_adr_primary+elem_index,model_id_res, sig_model, appkey_index);
    }else{
        MI_LOG_DEBUG("mi_appkey_bind: unbind model(0x%08x-%d) appkey index(%d)", model_id, elem_index, appkey_index);
        mesh_appkey_bind(MODE_APP_UNBIND, ele_adr_primary+elem_index,model_id_res, sig_model, appkey_index);
    }
    return result;
}

/**
 *@brief    add/delete subscription params.
 *@param    [in] op : add or delete
 *@param    [in] element : model element
 *@param    [in] company_id: company id
 *@param    [in] model_id : model_id
 *@param    [in] sub_addr: subscription address params
 *@return   0: success, negetive value: failure
 */
int mible_mesh_device_set_sub_address(mible_mesh_op_t op, uint16_t element, uint16_t company_id, uint16_t model_id, uint16_t sub_addr)
{
	int err =-1;
	u8 sig_model;
	u32 model_id_res;
	u16 adr = ele_adr_primary + element;
	if(company_id == MIBLE_MESH_COMPANY_ID_XIAOMI){
		sig_model = 0;
		model_id_res = (company_id)|(model_id <<16);
	}else{
		sig_model = 1;
		model_id_res = model_id;
	}
	MI_LOG_INFO("modle is 0x%08x, sig model is %d,sub_adr is 0x%04x,ele is%04x", model_id_res,sig_model,sub_addr,element);
	if(op == MIBLE_MESH_OP_ADD){
        MI_LOG_DEBUG("mi_sub_address: sub model(0x%08x-%d) group address(%d)", model_id, element, sub_addr);
        err = mesh_sub_search_and_set(CFG_MODEL_SUB_ADD,adr,sub_addr,0,model_id_res,sig_model);
    }else{
        MI_LOG_DEBUG("mi_sub_address: unsub model(0x%08x-%d) group address(%d)", model_id, element, sub_addr);
        err = mesh_sub_search_and_set(CFG_MODEL_SUB_DEL,adr,sub_addr,0,model_id_res,sig_model);
    }
	MI_LOG_INFO("SUB result is 0x%08x/r/n",err);
    return 0;
}

/**
 *@brief    generic message, Mesh model 3.2, 4.2, 5.2, 6.3, or 7 Summary.
 *          report event: MIBLE_MESH_EVENT_GENERIC_OPTION_CB, data: mible_mesh_access_message_t.
 *@param    [in] param : control parameters corresponding to node
 *          according to opcode, generate a mesh message; extral params: ack_opcode, tid, get_or_set.
 *@return   0: success, negetive value: failure
 */
int mible_mesh_node_generic_control(mible_mesh_access_message_t *param)
{
    uint16_t opcode = mesh_mi_trans_op_big2little(param->opcode.opcode);
	u8 *p_buf;
	u16 src,dst;
	if(param->buf_len >8){
		p_buf = param->p_buf;
	}else{
		p_buf = param->buff;
	}
	src = param->meta_data.src_addr;
	dst = param->meta_data.dst_addr;
	MI_LOG_INFO("send generic control msg%04x,%04x,%04x\r\n",src,dst,opcode);
    return mesh_tx_cmd2normal(opcode, p_buf, param->buf_len, src, dst, 0);
}

/**
 *@brief    get system time.
 *@return   systicks in ms.
 */
uint64_t mible_mesh_get_exact_systicks(void)
{
    //TODO: need 1s poll timer
    //MI_LOG_DEBUG("mible_mesh_get_exact_systicks system %d, ticks %d\n", systime*1000, ticks);
    return (uint64_t)system_time_ms;
}
uint16_t mible_conn_handle = 0xFFFF;
void mible_conn_timeout_cb(void *p_context)
{
    MI_LOG_INFO("[mible_conn_timeout_cb]disconnect handle: %04x\r\n", mible_conn_handle);
    if (0xFFFF != mible_conn_handle)
    {
        mible_gap_disconnect(mible_conn_handle);
    }
}
// need to trans little endiness to big endiness
u16 mesh_mi_trans_op_little2big(u16 op)
{
	if(SIZE_OF_OP(op)== OP_TYPE_SIG2){
		return swap_u16_data(op);
	}else{
		return op;
	}
}

u16 mesh_mi_trans_op_big2little(u16 op)
{
	u16 op_little = swap_u16_data(op);
	// change to little first 
	if(SIZE_OF_OP(op_little)== OP_TYPE_SIG2){
		// need to trans little first 
		return op_little;
	}else{
		return op;
	}
}

int mesh_mi_rx_cb(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	u16 op = cb_par->op;
	mesh_op_resource_t *p_res = (mesh_op_resource_t *)cb_par->res;
	switch(op){
		case NODE_RESET:
			process_mesh_node_reset();
			break;
		case G_ONOFF_GET:
		case G_ONOFF_SET:
		case G_ONOFF_SET_NOACK:
			generic_on_off_server_data(par,par_len,cb_par);
			break;
		case LIGHTNESS_GET:
		case LIGHTNESS_SET:
		case LIGHTNESS_SET_NOACK:
			light_lightness_server_data(par,par_len,cb_par);
			break;
		case LIGHT_CTL_TEMP_GET:
		case LIGHT_CTL_TEMP_SET:
		case LIGHT_CTL_TEMP_SET_NOACK:
			light_ctl_server_data(par,par_len,cb_par);
			break;
		default:
			break;
	}
	if(p_res->id == SIG_MD_CFG_SERVER){
		// config mode
		cfg_server_receive_peek(par,par_len,cb_par);
	}
	// vendor mode proc
	if(p_res->id == MIOT_SEPC_VENDOR_MODEL_SER){
		miot_server_data(par,par_len,cb_par);
	}else if (p_res->id == MIOT_VENDOR_MD_SER){
		mijia_server_data(par,par_len,cb_par);
	}
	return 0;
}
#endif




