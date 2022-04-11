/********************************************************************************************************
 * @file	on_property_get.c
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
 * on_property_get.c
 *
 *  Created on: 2020��12��11��
 *      Author: mi
 */
#include "miio_user_api.h"
#include "iid.h"
#if MI_API_ENABLE

switch_demo_t two_key_switch;

static void P_2_1_On_doGet(property_operation_t *o)
{
    o->value = property_value_new_boolean(two_key_switch.left_on); // TODO: ������Ҫ��������������ֵ
}

static void P_2_2_Mode_doGet(property_operation_t *o)
{
    o->value = property_value_new_uchar(two_key_switch.left_mode); // TODO: ������Ҫ��������������ֵ
}

static void P_2_3_val_doGet(property_operation_t *o)
{
    o->value = property_value_new_ulong(two_key_switch.ctl); // TODO: ������Ҫ��������������ֵ
}

static void P_3_1_On_doGet(property_operation_t *o)
{
    o->value = property_value_new_boolean(two_key_switch.right_on); // TODO: ������Ҫ��������������ֵ
}

static void P_3_2_Mode_doGet(property_operation_t *o)
{
    o->value = property_value_new_uchar(two_key_switch.right_mode); // TODO: ������Ҫ��������������ֵ
}

static void P_4_1_Anti_Flk_doGet(property_operation_t *o)
{
    o->value = property_value_new_uchar(two_key_switch.anti_flk); // TODO: ������Ҫ��������������ֵ
}

static void P_5_1_Relative_Humidity_doGet(property_operation_t *o)
{
    o->value = property_value_new_float(two_key_switch.humidity); // TODO: ������Ҫ��������������ֵ
}

static void P_5_2_Temperature_doGet(property_operation_t *o)
{
    o->value = property_value_new_float(two_key_switch.temperature); // TODO: ������Ҫ��������������ֵ
}

static void P_6_1_Status_doGet(property_operation_t *o)
{
    o->value = property_value_new_uchar(0); // TODO: ������Ҫ��������������ֵ
}

static void P_6_2_Fault_doGet(property_operation_t *o)
{
    o->value = property_value_new_longlong(-99999); // TODO: ������Ҫ��������������ֵ
}

static void S_2_Switch_doGet(property_operation_t *o)
{
    switch (o->piid)
    {
        case IID_2_1_On:
            P_2_1_On_doGet(o);
            break;

        case IID_2_2_Mode:
            P_2_2_Mode_doGet(o);
            break;
		case IID_2_3_val:
			P_2_3_val_doGet(o);
			break;
        default:
            o->code = OPERATION_ERROR_CANNOT_READ;
            break;
    }
}

static void S_3_Switch_doGet(property_operation_t *o)
{
    switch (o->piid)
    {
        case IID_3_1_On:
            P_3_1_On_doGet(o);
            break;

        case IID_3_2_Mode:
            P_3_2_Mode_doGet(o);
            break;

        default:
            o->code = OPERATION_ERROR_CANNOT_READ;
            break;
    }
}

static void S_4_Power_Manage_doGet(property_operation_t *o)
{
    switch (o->piid)
    {
        case IID_4_1_Anti_Flk:
            P_4_1_Anti_Flk_doGet(o);
            break;

        default:
            o->code = OPERATION_ERROR_CANNOT_READ;
            break;
    }
}

static void S_5_Environment_doGet(property_operation_t *o)
{
    switch (o->piid)
    {
        case IID_5_1_Relative_Humidity:
            P_5_1_Relative_Humidity_doGet(o);
            break;

        case IID_5_2_Temperature:
            P_5_2_Temperature_doGet(o);
            break;

        default:
            o->code = OPERATION_ERROR_CANNOT_READ;
            break;
    }
}

static void S_6_Switch_Sensort_doGet(property_operation_t *o)
{
    switch (o->piid)
    {
        case IID_6_1_Status:
            P_6_1_Status_doGet(o);
            break;

        case IID_6_2_Fault:
            P_6_2_Fault_doGet(o);
            break;

        default:
            o->code = OPERATION_ERROR_CANNOT_READ;
            break;
    }
}

void on_property_get(property_operation_t *o)
{
    MI_LOG_INFO("[on_property_get]siid %d, piid %d\n", o->siid, o->piid);

    switch (o->siid)
    {
        case IID_2_Switch:
            S_2_Switch_doGet(o);
            break;

        case IID_3_Switch:
            S_3_Switch_doGet(o);
            break;

        case IID_4_Power_Manage:
            S_4_Power_Manage_doGet(o);
            break;

        case IID_5_Environment:
            S_5_Environment_doGet(o);
            break;

        case IID_6_Switch_Sensort:
            S_6_Switch_Sensort_doGet(o);
            break;

        default:
            o->code = OPERATION_OK;
            break;
    }
}
#endif
