/********************************************************************************************************
 * @file	on_property_request.c
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
 * on_property_request.c
 *
 *  Created on: 2020Äê12ÔÂ14ÈÕ
 *      Author: mi
 */
#include "miio_user_api.h"
#include "iid.h"
#if MI_API_ENABLE

void P_128_1_GMT_Offset_doRequest(void)
{
    if(miio_mesh_request_property(SERVER_GMT_OFFSET) != 0){
        MI_LOG_ERROR("send_property_request failed!\n");
    }
}

void P_128_2_Weather_doRequest(void)
{
    if(miio_mesh_request_property(SERVER_WEATHER) != 0){
        MI_LOG_ERROR("send_property_request failed!\n");
    }
}

void P_128_3_UTC_Time_doRequest(void)
{
    if(miio_mesh_request_property(SERVER_UTC_TIME) != 0){
        MI_LOG_ERROR("send_property_request failed!\n");
    }
}
#endif
