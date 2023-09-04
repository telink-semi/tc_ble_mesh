/********************************************************************************************************
 * @file	nl_common.h
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
#include "../user_config.h"
#include "nl_model_level.h"
#include "nl_model_schedules.h"

#if NL_API_ENABLE

#define CB_NL_PAR_NUM_1(cb, a)	do{if(cb){cb(a);}}while(0)
#define CB_NL_PAR_NUM_2(cb, a, b)	do{if(cb){cb(a, b);}}while(0)
#define CB_NL_PAR_NUM_3(cb, a, b, c)	do{if(cb){cb(a, b, c);}}while(0)

void register_nl_callback();

#else

#define CB_NL_PAR_NUM_1(cb, a)
#define CB_NL_PAR_NUM_2(cb, a, b)	
#define CB_NL_PAR_NUM_3(cb, a, b, c)	

#define register_nl_callback() 
#endif
