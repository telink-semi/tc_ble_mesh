/********************************************************************************************************
 * @file	flash_fw_check.h
 *
 * @brief	for TLSR chips
 *
 * @author	BLE Group
 * @date	May. 12, 2018
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
#ifndef _FLASH_FW_CHECK_H_
#define _FLASH_FW_CHECK_H_


#include "proj/common/types.h"

/****
 * param--crc_init_value: crc initial value. if set 0.
 * return: 1--crc check fail; 0--crc check ok
 */
bool flash_fw_check( u32 crc_init_value );



#endif
