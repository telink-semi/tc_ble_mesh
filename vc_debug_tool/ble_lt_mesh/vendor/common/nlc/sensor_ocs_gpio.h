/********************************************************************************************************
 * @file	sensor_ocs_gpio.h
 *
 * @brief	for TLSR chips
 *
 * @author	telink
 * @date	June. 25, 2024
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

#include "nlc_sensor.h"

#if (NLC_SENSOR_SEL == SENSOR_OCS_GPIO)
#define OCCUPANCY_SENSOR_OUTPUT_PIN			GPIO_PD7

void sensor_init_ocs_gpio(void);
u32 sensor_get_ocs_gpio(void);

#endif


