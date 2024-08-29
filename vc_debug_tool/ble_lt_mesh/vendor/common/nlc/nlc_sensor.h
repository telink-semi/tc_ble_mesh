/********************************************************************************************************
 * @file	nlc_sensor.h
 *
 * @brief	for TLSR chips
 *
 * @author	telink
 * @date	April. 21, 2023
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
#pragma once

#include "tl_common.h"
#include "proj_lib/sig_mesh/app_mesh.h"

#if ((NLC_SENSOR_TYPE_SEL == NLCP_TYPE_OCS) || (NLC_SENSOR_TYPE_SEL == NLCP_TYPE_ALS))
// -------- sensor define ----------------
#define SENSOR_NONE							0 // use BDT or button to simulate.
#define SENSOR_ZSIR1000						1
#define SENSOR_OCS_GPIO						2 // all sensors with GPIO detecte for occupancy sensor can use this type

#define NLC_SENSOR_SEL						SENSOR_NONE

void nlc_sensor_init(void);
u32 nlc_sensor_get(void);

#endif


