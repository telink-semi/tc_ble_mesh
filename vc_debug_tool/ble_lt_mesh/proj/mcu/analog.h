/********************************************************************************************************
 * @file	analog.h
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

#include "../common/types.h"
#include "compiler.h"

u8 analog_read(u8 addr);
//_attribute_ram_code_
void analog_write(u8 addr, u8 v);
void analog_read_multi(u8 addr, u8 *v, int len);
void analog_write_multi(u8 addr, u8 *v, int len);
void analog_set_freq_offset(int off);

// the analog version of SET_FLD_V
#define ANALOG_SET_FLD_V(addr, ...)				\
	do{											\
		u8 v = analog_read(addr);		\
		SET_FLD_V(v, __VA_ARGS__);				\
		analog_write(addr, v);			\
	}while(0)

