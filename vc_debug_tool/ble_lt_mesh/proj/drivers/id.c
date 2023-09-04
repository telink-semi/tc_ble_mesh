/********************************************************************************************************
 * @file	id.c
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
#include "../mcu/register.h"
#include "../common/assert.h"
#include "id.h"

static void id_set_magic_enable(){
	reg_id_wr_en = ID_WRITE_ENABLE_MAGIC;
}

void id_set_product_id(u8 function_id, u8 version_id, u16 production_id){
	id_set_magic_enable();
	reg_product_id = (production_id << 16 | version_id << 8 | function_id);
}

