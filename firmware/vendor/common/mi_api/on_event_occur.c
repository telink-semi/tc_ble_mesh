/********************************************************************************************************
 * @file	on_event_occur.c
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
 * on_event_occur.c
 *
 *  Created on: 2020Äê12ÔÂ14ÈÕ
 *      Author: mi
 */
#include "miio_user_api.h"
#include "iid.h"
#if MI_API_ENABLE

void E_6_1_Click_doEvent(void)
{
    if(miio_mesh_event_occurred(6, 1, NULL) != 0){
        MI_LOG_ERROR("send_event_occurred failed!\n");
    }
}

void E_6_2_Long_Press_doEvent(uint32_t arg1, int64_t arg2)
{
    arguments_t *newArgs = arguments_new();
    if(newArgs == NULL){
        MI_LOG_ERROR("arguments_new failed!\n");
        return;
    }
    newArgs->size = 2;
    newArgs->arguments[0].piid = 1;
    newArgs->arguments[0].value = property_value_new_ulong(arg1);
    newArgs->arguments[1].piid = 2;
    newArgs->arguments[1].value = property_value_new_longlong(arg2);

    if(miio_mesh_event_occurred(6, 2, newArgs) != 0){
        MI_LOG_ERROR("send_event_occurred failed!\n");
    }
}
#endif
