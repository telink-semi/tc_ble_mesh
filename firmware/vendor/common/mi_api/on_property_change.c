/********************************************************************************************************
 * @file	on_property_change.c
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
 * on_property_change.c
 *
 *  Created on: 2020Äê12ÔÂ14ÈÕ
 *      Author: mi
 */
#include "miio_user_api.h"
#include "iid.h"
#if MI_API_ENABLE

void P_2_1_On_doChange(bool newValue)
{
    if(miio_mesh_properties_changed(2, 1, property_value_new_boolean(newValue)) != 0){
        MI_LOG_ERROR("send_property_changed failed!\n");
    }
}

void P_2_1_Mode_doChange(uint8_t newValue)
{
    if(miio_mesh_properties_changed(2, 2, property_value_new_uchar(newValue)) != 0){
        MI_LOG_ERROR("send_property_changed failed!\n");
    }
}

void P_3_1_On_doChange(bool newValue)
{
    if(miio_mesh_properties_changed(3, 1, property_value_new_boolean(newValue)) != 0){
        MI_LOG_ERROR("send_property_changed failed!\n");
    }
}

void P_3_2_Mode_doChange(uint8_t newValue)
{
    if(miio_mesh_properties_changed(3, 2, property_value_new_uchar(newValue)) != 0){
        MI_LOG_ERROR("send_property_changed failed!\n");
    }
}

void P_4_1_Anti_Flk_doChange(bool newValue)
{
    if(miio_mesh_properties_changed(4, 1, property_value_new_boolean(newValue)) != 0){
        MI_LOG_ERROR("send_property_changed failed!\n");
    }
}

void P_5_1_Relative_Humidity_doChange(float newValue)
{
    if(miio_mesh_properties_changed(5, 1, property_value_new_float(newValue)) != 0){
        MI_LOG_ERROR("send_property_changed failed!\n");
    }
}

void P_5_2_Temperature_doChange(float newValue)
{
    if(miio_mesh_properties_changed(5, 2, property_value_new_float(newValue)) != 0){
        MI_LOG_ERROR("send_property_changed failed!\n");
    }
}

void P_6_1_Status_doChange(uint8_t newValue)
{
    if(miio_mesh_properties_changed(6, 1, property_value_new_uchar(newValue)) != 0){
        MI_LOG_ERROR("send_property_changed failed!\n");
    }
}

void P_6_2_Fault_doChange(int64_t newValue)
{
    if(miio_mesh_properties_changed(6, 2, property_value_new_longlong(newValue)) != 0){
        MI_LOG_ERROR("send_property_changed failed!\n");
    }
}
#endif
