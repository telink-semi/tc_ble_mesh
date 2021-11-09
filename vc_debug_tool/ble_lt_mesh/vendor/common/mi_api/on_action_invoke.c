/*
 * user_operation.c
 *
 *  Created on: 2020年12月11日
 *      Author: mi
 */
#include "miio_user_api.h"
#include "iid.h"
#if MI_API_ENABLE
static operation_code_t A_2_1_Toggle()
{
    // TODO: 在这里执行真正的Action操作
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
    // TODO: 在这里执行真正的Action操作
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
 * 将Action调用转换为强类型的函数调用。
 */
static void A_2_1_Toggle_doInvoke(action_operation_t *o)
{
    // 检查输入参数个数
#if 0
    if (o->in->size != 0)
    {
        o->code = OPERATION_INVALID;
        return;
    }
#endif
    // 输出结果个数
    o->out->size = 0;

    // 调用真正的执行函数
    o->code = A_2_1_Toggle();
}

/**
 * 将Action调用转换为强类型的函数调用。
 */
static void A_3_1_Toggle_doInvoke(action_operation_t *o)
{
    // 检查输入参数个数
    if (o->in->size != 0)
    {
        o->code = OPERATION_INVALID;
        return;
    }

    // 输出结果个数
    o->out->size = 0;

    // 调用真正的执行函数
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
