/********************************************************************************************************
 * @file	sensor_zsir1000.h
 *
 * @brief	for TLSR chips
 *
 * @author	telink
 * @date	June. 11, 2024
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

#if (NLC_SENSOR_SEL == SENSOR_ZSIR1000)
#define I2C_IC_ID_SLAVE						(0x54)			// has been left shift one bit
#define	NLC_I2C_CLK_SPEED					(200 * 1000) 	// unit: Hz

	#if (MCU_CORE_TYPE == MCU_CORE_9518)
#define NLC_I2C_IO_SDA						I2C_GPIO_SDA_B3
#define NLC_I2C_IO_SCL						I2C_GPIO_SCL_B2
	#else // B85m
#define	NLC_I2C_GPIO_GROUP					I2C_GPIO_GROUP_C2C3
	#endif

	#if (NLC_SENSOR_TYPE_SEL == NLCP_TYPE_OCS)
#define ZSIR1000_MOTION_SENSED_EN			1
	#elif (NLC_SENSOR_TYPE_SEL == NLCP_TYPE_ALS)
#define ZSIR1000_AMBIENT_LIGHT_SENSED_EN	1
	#endif

	#if ZSIR1000_MOTION_SENSED_EN
#define OCCUPANCY_SENSOR_OUTPUT_PIN			GPIO_PD7
	#endif

void sensor_init_zsir1000(void);
u32 sensor_get_zsir1000();
#endif

