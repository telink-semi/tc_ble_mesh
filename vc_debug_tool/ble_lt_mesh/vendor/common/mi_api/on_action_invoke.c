/********************************************************************************************************
 * @file	on_action_invoke.c
 *
 * @brief	This is the source file for BLE SDK
 *
 * @author	Mesh Group
 * @date	2021
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
/*
 * user_operation.c
 *
 *  Created on: 2020��12��11��
 *      Author: mi
 */
#include "miio_user_api.h"
#include "iid.h"
#if MI_API_ENABLE
static operation_code_t A_2_1_Toggle()
{
    // TODO: ������ִ��������Action����
    MI_LOG_WARNING("[action] toggle left-key <%s>\n", two_key_switch.left_on? "on->off" : "off->on");
    if(two_key_switch.left_on){
        two_key_switch.left_on = false;
    }else{
        two_key_switch.left_on = true;
    }

    P_2_1_On_doChange(two_key_switch.left_on);

    return OPERATION_OK;
}

static operation_code_t A_3_1_Toggle()
{
    // TODO: ������ִ��������Action����
    MI_LOG_WARNING("[action] toggle right-key <%s>\n", two_key_switch.right_on? "on->off" : "off->on");
    if(two_key_switch.right_on){
        two_key_switch.right_on = false;
    }else{
        two_key_switch.right_on = true;
    }

    P_3_1_On_doChange(two_key_switch.right_on);

    return OPERATION_OK;
}

/**
 * ��Action����ת��Ϊǿ���͵ĺ������á�
 */
static void A_2_1_Toggle_doInvoke(action_operation_t *o)
{
    // ��������������
#if 0
    if (o->in->size != 0)
    {
        o->code = OPERATION_INVALID;
        return;
    }
#endif
    // ����������
    o->out->size = 0;

    // ����������ִ�к���
    o->code = A_2_1_Toggle();
}

/**
 * ��Action����ת��Ϊǿ���͵ĺ������á�
 */
static void A_3_1_Toggle_doInvoke(action_operation_t *o)
{
    // ��������������
    if (o->in->size != 0)
    {
        o->code = OPERATION_INVALID;
        return;
    }

    // ����������
    o->out->size = 0;

    // ����������ִ�к���
    o->code = A_3_1_Toggle();
}

static void S_2_Switch_doAction(action_operation_t *o)
{
    switch (o->aiid)
    {
        case IID_2_1_Toggle:
            A_2_1_Toggle_doInvoke(o);
            break;

        default:
            o->code = OPERATION_INVALID;
            break;
    }
}

static void S_3_Switch_doAction(action_operation_t *o)
{
    switch (o->aiid)
    {
        case IID_2_1_Toggle:
            A_3_1_Toggle_doInvoke(o);
            break;

        default:
            o->code = OPERATION_INVALID;
            break;
    }
}

void on_action_invoke(action_operation_t *o)
{
    MI_LOG_INFO("[on_action_invoke]siid %d, aiid %d\n", o->siid, o->aiid);

    switch(o->siid){
    case IID_2_Switch:
        S_2_Switch_doAction(o);
        break;
    case IID_3_Switch:
        S_3_Switch_doAction(o);
        break;
    default:
        o->code = OPERATION_INVALID;
        break;
    }
}
#endif
