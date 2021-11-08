/*
 * mible_mesh_api.c
 *
 *  Created on: 2020Äê11ÔÂ26ÈÕ
 *      Author: mi
 */

#include "mible_mesh_api.h"

static mible_mesh_event_cb_t mible_mesh_event_callback_handler;

int mible_mesh_event_callback(mible_mesh_event_type_t type, void * data)
{
    if(mible_mesh_event_callback_handler != NULL){
        mible_mesh_event_callback_handler(type, (mible_mesh_event_params_t *)data);
    }
    return 0;
}

int mible_mesh_device_register_event_callback(mible_mesh_event_cb_t mible_mesh_event_cb)
{
    mible_mesh_event_callback_handler = mible_mesh_event_cb;
    return 0;
}

int mible_mesh_device_unregister_event_callback(mible_mesh_event_cb_t mible_mesh_event_cb)
{
    if(mible_mesh_event_callback_handler == mible_mesh_event_cb){
        mible_mesh_event_callback_handler = NULL;
        return 0;
    }
    return -1;
}

static mible_user_event_cb_t mible_mesh_user_state_callback_handler;

int mible_mesh_user_event_callback(uint8_t type, void * data)
{
    if(mible_mesh_user_state_callback_handler != NULL){
        mible_mesh_user_state_callback_handler(type, data);
    }
    return 0;
}

int mible_mesh_user_event_register_event_callback(mible_user_event_cb_t user_event_cb)
{
    mible_mesh_user_state_callback_handler = user_event_cb;
    return 0;
}

int mible_mesh_user_event_unregister_event_callback(mible_user_event_cb_t user_event_cb)
{
    if(mible_mesh_user_state_callback_handler == user_event_cb){
        mible_mesh_user_state_callback_handler = NULL;
        return 0;
    }
    return -1;
}

__WEAK int mible_mesh_device_init_stack(void)
{
    // init your mesh stack, thread, memory, prepare for provisioner initilization.

    return 0;
}

__WEAK int mible_mesh_device_deinit_stack(void)
{
    // deinit your mesh stack, close thread,free memory, and release firmware bt resource.

    return 0;
}

__WEAK int mible_mesh_device_init_node(void)
{
    /**********************************************************************//**
     * init provisioner, init models/opcodes.
     * load seq_num, iv_index, replay list
     **********************************************************************/
    return 0;
}

__WEAK int mible_mesh_device_set_provsion_data(mible_mesh_provisioning_data_t *param)
{
    return 0;
}

__WEAK int mible_mesh_device_provsion_done(void)
{
    return 0;
}

__WEAK int mible_mesh_node_reset(void)
{
    return 0;
}

__WEAK int mible_mesh_device_unprovsion_done(void)
{
    return 0;
}

__WEAK int mible_mesh_device_login_done(uint8_t status)
{
    return 0;
}

__WEAK int mible_mesh_device_set_network_transmit_param(uint8_t count, uint8_t interval_steps)
{
    // set adv interval and adv transmit times
    return 0;
}

__WEAK int mible_mesh_device_set_relay(uint8_t enabled,uint8_t count,uint8_t interval)
{
    return 0;
}

__WEAK int mible_mesh_device_get_relay(uint8_t *enabled, uint8_t *count, uint8_t *step)
{
    return 0;
}

__WEAK int mible_mesh_device_get_seq(uint16_t element, uint32_t* seq, uint32_t* iv, uint8_t* flags)
{
    return 0;
}

__WEAK int mible_mesh_device_update_iv_info(uint32_t iv_index, uint8_t flags)
{
    // sync and load iv_index from application
    return 0;
}

__WEAK int mible_mesh_device_set_netkey(mible_mesh_op_t op, uint16_t netkey_index, uint8_t *netkey)
{
    if(op == MIBLE_MESH_OP_ADD){
        // add netkey
    }else{
        // delete netkey
    }
    return 0;
}

__WEAK int mible_mesh_device_set_appkey(mible_mesh_op_t op, uint16_t netkey_index, uint16_t appkey_index, uint8_t * appkey)
{
    if(op == MIBLE_MESH_OP_ADD){
        // add app_key, bind netkey index
    }else{
        // delete netkey, unbind netkey index
    }
    return 0;
}

__WEAK int mible_mesh_device_set_model_app(mible_mesh_op_t op, uint16_t elem_index, uint16_t company_id,
        uint16_t model_id, uint16_t appkey_index)
{
    if(op == MIBLE_MESH_OP_ADD){
        // bind model appkey_index
    }else{
        // unbind model appkey_index
    }
    return 0;
}

__WEAK int mible_mesh_device_set_sub_address(mible_mesh_op_t op, uint16_t element, uint16_t company_id,
        uint16_t model_id, uint16_t sub_addr)
{
    if(op == MIBLE_MESH_OP_ADD){
        // add subscription
    }else{
        // delete subscription
    }
    return 0;
}

__WEAK int mible_mesh_node_generic_control(mible_mesh_access_message_t *param)
{
    return 0;
}
