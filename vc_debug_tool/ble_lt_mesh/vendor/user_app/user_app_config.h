/********************************************************************************************************
 * @file	user_app_config.h
 *
 * @brief	for TLSR chips
 *
 * @author	telink
 * @date	Sep. 30, 2010
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

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

/*
    Note: only can use "#define", "#if .. #elif..#endif", etc. here. Don't use "enum" "typedef".
*/

// #define RAM_SIZE_MAX            		(48*1024)	// 8253/8273


// ------- function Macro ---------
// #define CB_USER_FACTORY_RESET_ADDITIONAL()   cb_user_factory_reset_additional()
// #define CB_USER_PROC_LED_ONOFF_DRIVER(on)    cb_user_proc_led_onoff_driver(on)


#include "user_app_default.h"   // must at the end of this file

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
