/********************************************************************************************************
 * @file	tl_common.h
 *
 * @brief	This is the header file for TLSR8258
 *
 * @author	author@telink-semi.com;
 * @date	May 8, 2018
 *
 * @par     Copyright (c) 2018, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#include "proj/tl_common.h"

#if !WIN32
#define SHOW_FUNC_IN(msg, arg...) printf("> %s(%d): " msg "\n", __FUNCTION__,__LINE__, ##arg)
#define SHOW_FUNC_OUT(msg, arg...) printf("< %s(%d): " msg "\n\n", __FUNCTION__,__LINE__, ##arg)
#define SHOW_DBG(msg, arg...) printf("%s:%s(%d): " msg "\n", __FILE__, __FUNCTION__,__LINE__, ##arg)

	#if (LLSYNC_ENABLE && (0 == LLSYNC_LOG_EN))
#define ble_qiot_log_d(fmt, args...)                                   
#define ble_qiot_log_i(fmt, args...)                                  
#define ble_qiot_log_w(fmt, args...)                                                             
#define ble_qiot_log_e(fmt, args...)                                                            
#define ble_qiot_log(level, fmt, args...)                                             
#define ble_qiot_log_raw(fmt, args...) 
	#endif
#endif

