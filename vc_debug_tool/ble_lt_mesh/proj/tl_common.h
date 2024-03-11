/********************************************************************************************************
 * @file	tl_common.h
 *
 * @brief	for TLSR chips
 *
 * @author	telink
 * @date	Sep. 30, 2010
 *
 * @par     Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#include "mcu/config.h"
#include "common/types.h"
#include "common/bit.h"
#include "common/tutility.h"
#include "common/static_assert.h"
#include "common/assert.h"
#include "vendor/common/user_config.h"
#include "common/compatibility.h"
#if(MCU_CORE_TYPE == MCU_CORE_8258)
#include "drivers/8258/analog.h"
#include "drivers/8258/compiler.h"
#include "drivers/8258/register.h"
#include "drivers/8258/gpio.h"
#include "drivers/8258/pwm.h"
#include "drivers/8258/dma.h"
#include "drivers/8258/clock.h"
#include "drivers/8258/random.h"
#elif(MCU_CORE_TYPE == MCU_CORE_8278)
#include "drivers/8278/analog.h"
#include "drivers/8278/compiler.h"
#include "drivers/8278/register.h"
#include "drivers/8278/gpio.h"
#include "drivers/8278/pwm.h"
#include "drivers/8278/dma.h"
#include "drivers/8278/clock.h"
#include "drivers/8278/random.h"
#else
#include "mcu/analog.h"
#include "mcu/compiler.h"
#include "mcu/register.h"
#include "mcu/gpio.h"
#include "mcu/pwm.h"
#include "mcu/cpu.h"
#include "mcu/dma.h"
#include "mcu/clock.h"
#include "mcu/clock_i.h"
#include "mcu/random.h"
#endif
#include "mcu/irq_i.h"
#include "common/breakpoint.h"
#include "common/log.h"
#if !WIN32
#include "drivers/8258/flash.h"
#endif
//#include "../ble/ble_globals.h"

#if WIN32
#include <stdio.h>
#include <string.h>
#else
#include "common/printf.h"
#include "common/tstring.h"
#endif

#define DEBUG_STOP()	{reg_tmr_ctrl = 0; reg_gpio_pb_ie = 0xff; while(1);}	// disable watchdog;  DP/DM en
#define DEBUG_SWS_EN()	{reg_tmr_ctrl = 0; reg_gpio_pb_ie = 0xff;}	// disable watchdog;  DP/DM en

