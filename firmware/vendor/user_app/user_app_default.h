/********************************************************************************************************
 * @file	user_app_default.h
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

// ----- user can define in user_app_config.h ------ //
#ifndef LIGHTNESS_DATA_REFORMAT_EN          
#define LIGHTNESS_DATA_REFORMAT_EN          0
#endif

#ifndef LIGHT_RES_HW_USER_EN          
#define LIGHT_RES_HW_USER_EN                0
#endif

#ifndef LIGHT_PAR_USER_EN          
#define LIGHT_PAR_USER_EN                   0
#endif

#ifndef USER_MESH_CMD_VD_ARRAY          
#define USER_MESH_CMD_VD_ARRAY              // NULL
#endif

#ifndef USER_REDEFINE_SCAN_RSP_EN          
#define USER_REDEFINE_SCAN_RSP_EN           0
#endif

// PWM_FREQ
// HCI_LOG_FW_EN
// TRANSITION_TIME_DEFAULT_VAL
// GPIO_LED

// ------- function Macro ---------
#ifndef CB_USER_FACTORY_RESET_ADDITIONAL
#define CB_USER_FACTORY_RESET_ADDITIONAL()          
#endif

#ifndef CB_USER_LIGHT_INIT_ON_CONDITION
#define CB_USER_LIGHT_INIT_ON_CONDITION()   (1)
#endif

#ifndef CB_USER_BLE_CONNECT
#define CB_USER_BLE_CONNECT(e, p, n)                      
#endif

#ifndef CB_USER_INIT
#define CB_USER_INIT()                      
#endif

#ifndef CB_USER_MAIN_LOOP
#define CB_USER_MAIN_LOOP()                      
#endif

// CB_USER_PROC_LED_ONOFF_DRIVER
// CFG_LED_EVENT_SET_MESH_INFO
// CFG_LED_EVENT_SET_SUBSCRIPTION
// CB_USER_IS_CMD_WITH_TID_VENDOR

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
