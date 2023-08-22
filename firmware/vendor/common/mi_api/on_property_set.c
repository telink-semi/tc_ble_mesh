/*
 * on_property_set.c
 *
 *  Created on: 2020年12月11日
 *      Author: mi
 */
#include "miio_user_api.h"
#include "iid.h"
#include "vendor/common/light.h"
#include "vendor/common/lighting_model.h"
#include "vendor/common/generic_model.h"
#if MI_API_ENABLE

void property_para_init()
{
	two_key_switch.left_on =1;
	two_key_switch.left_mode =100;
	two_key_switch.ctl = 800;
}

void property_set_onoff(u8 onoff,u8 ele_idx,u8 tid)
{
	mesh_cmd_g_onoff_set_t par_set;
	st_pub_list_t pub_list = {{0}};
	par_set.onoff = onoff;
	par_set.tid = tid;
	g_onoff_set((mesh_cmd_g_onoff_set_t *)&par_set, 2, 0, ele_idx, 0, &pub_list);
}

void property_set_lightness(u8 val,u8 ele_idx,u8 tid)
{
	mesh_cmd_lightness_set_t par_t ;
	st_pub_list_t pub_list = {{0}};
	par_t.lightness = (val)*655;
	par_t.tid = tid;
	lightness_set((mesh_cmd_lightness_set_t *)&par_t, 2, LIGHTNESS_SET, ele_idx, 0, &pub_list);

}
static void P_2_1_On_doSet(property_operation_t *o)
{
    // 判断数据格式是否正确，如果错误，返回代码: OPERATION_ERROR_VALUE
    if (o->value->format != PROPERTY_FORMAT_BOOL)
    {
        o->code = OPERATION_ERROR_VALUE;
        return;
    }

    // TODO: 执行写操作: o->value->data.boolean.value;
    two_key_switch.left_on = o->value->data.boolean;
	// need to set the model part
	two_key_switch.tid++;
	property_set_onoff(two_key_switch.left_on,0,two_key_switch.tid);	
    // 如果成功，返回代码: OPERATION_OK
    o->code = OPERATION_OK;

    return;
}

static void P_2_2_Mode_doSet(property_operation_t *o)
{
    // 判断数据格式是否正确，如果错误，返回代码: OPERATION_ERROR_VALUE
    MI_LOG_INFO("P_2_2_Mode_doSet\n");
    if (o->value->format != PROPERTY_FORMAT_UCHAR && o->value->format != PROPERTY_FORMAT_NUMBER)
    {
    	MI_LOG_INFO("P_2_2_Mode_doSet FORMAT ERR\n");
        o->code = OPERATION_ERROR_VALUE;
        return;
    }
	MI_LOG_INFO("P_2_2_Mode_doSet %02x\n",o->value->data.number.ucharValue);
    // 简单判断数据取值范围是否合法，如果不合法，返回代码: OPERATION_ERROR_VALUE

    // TODO: 执行写操作: o->value->data.number.value.integerValue;
    two_key_switch.left_mode = o->value->data.number.ucharValue;
	two_key_switch.tid++;
	property_set_lightness(two_key_switch.left_mode,0,two_key_switch.tid);
    // 如果成功，返回代码: OPERATION_OK
    o->code = OPERATION_OK;
    return;
}

static void P_2_3_value_doSet(property_operation_t *o)
{
    // 判断数据格式是否正确，如果错误，返回代码: OPERATION_ERROR_VALUE
    int err =-1;
    MI_LOG_INFO("P_2_3_value_doSet\n");
    if (o->value->format != PROPERTY_FORMAT_UCHAR && o->value->format != PROPERTY_FORMAT_NUMBER)
    {
    	MI_LOG_INFO("P_2_3_value_doSet FORMAT ERR\n");
        o->code = OPERATION_ERROR_VALUE;
        return;
    }
    // TODO: 执行写操作: o->value->data.number.value.integerValue;
    two_key_switch.ctl = o->value->data.number.ushortValue;
	MI_LOG_INFO("P_2_3_value_doSet%04x\n",two_key_switch.ctl);
	two_key_switch.tid++;
	mesh_cmd_light_ctl_set_t set;
	set.lightness =0;
	set.temp = two_key_switch.ctl;
	set.delta_uv = 0;
	set.tid = two_key_switch.tid;
	set_ct_mode(1);
	st_pub_list_t pub_list = {{0}};
	err = light_ctl_temp_set(&set, sizeof(set),LIGHT_CTL_TEMP_SET, 0, 0, &pub_list);
	MI_LOG_INFO("P_2_3_value_doSet err = %04x\n",err);
	// 如果成功，返回代码: OPERATION_OK
    o->code = OPERATION_OK;
    return;
}



static void P_3_1_On_doSet(property_operation_t *o)
{
    // 判断数据格式是否正确，如果错误，返回代码: OPERATION_ERROR_VALUE
    if (o->value->format != PROPERTY_FORMAT_BOOL)
    {
        o->code = OPERATION_ERROR_VALUE;
        return;
    }

    // TODO: 执行写操作: o->value->data.boolean.value;
    two_key_switch.right_on = o->value->data.boolean;
	two_key_switch.tid++;
	property_set_onoff(two_key_switch.right_on,1,two_key_switch.tid);
    // 如果成功，返回代码: OPERATION_OK
    o->code = OPERATION_OK;

    return;
}

static void P_3_2_Mode_doSet(property_operation_t *o)
{
    // 判断数据格式是否正确，如果错误，返回代码: OPERATION_ERROR_VALUE
    if (o->value->format != PROPERTY_FORMAT_UCHAR && o->value->format != PROPERTY_FORMAT_NUMBER)
    {
        o->code = OPERATION_ERROR_VALUE;
        return;
    }
    // TODO: 执行写操作: o->value->data.number.value.integerValue;
    two_key_switch.right_mode = o->value->data.number.ucharValue;
	two_key_switch.tid++;
	property_set_lightness(two_key_switch.right_mode,1,two_key_switch.tid);
    // 如果成功，返回代码: OPERATION_OK
    o->code = OPERATION_OK;

    return;
}

static void P_4_1_Anti_Flk_doSet(property_operation_t *o)
{
    // 判断数据格式是否正确，如果错误，返回代码: OPERATION_ERROR_VALUE
    if (o->value->format != PROPERTY_FORMAT_BOOL)
    {
        o->code = OPERATION_ERROR_VALUE;
        return;
    }

    // TODO: 执行写操作: o->value->data.number.value.integerValue;
    two_key_switch.anti_flk = o->value->data.boolean;

    // 如果成功，返回代码: OPERATION_OK
    o->code = OPERATION_OK;

    return;
}

static void P_128_1_GMT_Offset_doSet(property_operation_t *o)
{
    // 判断数据格式是否正确，如果错误，返回代码: OPERATION_ERROR_VALUE
    if (o->value->format != PROPERTY_FORMAT_LONG && o->value->format != PROPERTY_FORMAT_NUMBER)
    {
        o->code = OPERATION_ERROR_VALUE;
        return;
    }

    // TODO: 执行写操作: o->value->data.number.value.integerValue;
    two_key_switch.gmt_offset = o->value->data.number.integerValue;

    // 如果成功，返回代码: OPERATION_OK
    o->code = OPERATION_OK;

    return;
}

static void P_128_2_Weather_doSet(property_operation_t *o)
{
    // 判断数据格式是否正确，如果错误，返回代码: OPERATION_ERROR_VALUE
    if (o->value->format != PROPERTY_FORMAT_LONG && o->value->format != PROPERTY_FORMAT_NUMBER)
    {
        o->code = OPERATION_ERROR_VALUE;
        return;
    }

    // TODO: 执行写操作: o->value->data.number.value.integerValue;
    two_key_switch.weather = o->value->data.number.integerValue;

    // 如果成功，返回代码: OPERATION_OK
    o->code = OPERATION_OK;

    return;
}

static void P_128_3_UTC_Time_doSet(property_operation_t *o)
{
    // 判断数据格式是否正确，如果错误，返回代码: OPERATION_ERROR_VALUE
    if (o->value->format != PROPERTY_FORMAT_ULONG && o->value->format != PROPERTY_FORMAT_NUMBER)
    {
        o->code = OPERATION_ERROR_VALUE;
        return;
    }

    // TODO: 执行写操作: o->value->data.number.value.integerValue;
    two_key_switch.utc_time = o->value->data.number.ulongValue;

    // 如果成功，返回代码: OPERATION_OK
    o->code = OPERATION_OK;

    return;
}


static void S_2_Switch_doSet(property_operation_t *o)
{
    switch (o->piid)
    {
        case IID_2_1_On:
            P_2_1_On_doSet(o);
            break;

        case IID_2_2_Mode:
            P_2_2_Mode_doSet(o);
            break;
		case IID_2_3_val:
			P_2_3_value_doSet(o);
			break;
        default:
            o->code = OPERATION_ERROR_CANNOT_WRITE;
            break;
    }
}

static void S_3_Switch_doSet(property_operation_t *o)
{
    switch (o->piid)
    {
        case IID_3_1_On:
            P_3_1_On_doSet(o);
            break;

        case IID_3_2_Mode:
            P_3_2_Mode_doSet(o);
            break;

        default:
            o->code = OPERATION_ERROR_CANNOT_WRITE;
            break;
    }
}

static void S_4_Power_Manage_doSet(property_operation_t *o)
{
    switch (o->piid)
    {
        case IID_4_1_Anti_Flk:
            P_4_1_Anti_Flk_doSet(o);
            break;

        default:
            o->code = OPERATION_ERROR_CANNOT_WRITE;
            break;
    }
}

static void S_128_Server_Info_doSet(property_operation_t *o)
{
    switch (o->piid)
    {
        case IID_128_1_GMT_Offset:
            P_128_1_GMT_Offset_doSet(o);
            break;

        case IID_128_2_Weather:
            P_128_2_Weather_doSet(o);
            break;

        case IID_128_3_UTC_Time:
            P_128_3_UTC_Time_doSet(o);
            break;

        default:
            o->code = OPERATION_ERROR_CANNOT_WRITE;
            break;
    }
}

void on_property_set(property_operation_t *o)
{
    MI_LOG_INFO("[on_property_set]siid %d, piid %d\n", o->siid, o->piid);

    if (o->value == NULL)
    {
        MI_LOG_ERROR("value is NULL\n");
        return;
    }

    switch (o->siid)
    {
        case IID_2_Switch:
            S_2_Switch_doSet(o);
            break;

        case IID_3_Switch:
            S_3_Switch_doSet(o);
            break;

        case IID_4_Power_Manage:
            S_4_Power_Manage_doSet(o);
            break;

        case IID_128_Server_Info:
            S_128_Server_Info_doSet(o);
            break;

        default:
            o->code = OPERATION_OK;
            break;
    }
}
#endif
