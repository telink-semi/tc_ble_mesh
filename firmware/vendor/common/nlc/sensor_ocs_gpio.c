/********************************************************************************************************
 * @file	sensor_ocs_gpio.c
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
#include "sensor_ocs_gpio.h"
#if (NLC_SENSOR_SEL == SENSOR_OCS_GPIO)

void sensor_init_ocs_gpio(void)
{
	gpio_set_func(OCCUPANCY_SENSOR_OUTPUT_PIN, AS_GPIO);
    gpio_set_input_en(OCCUPANCY_SENSOR_OUTPUT_PIN, 1);
}

u32 sensor_get_ocs_gpio(void)
{
	return (0 != gpio_read(OCCUPANCY_SENSOR_OUTPUT_PIN));
}
#endif





