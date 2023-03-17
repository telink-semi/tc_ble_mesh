/********************************************************************************************************
 * @file	mouse_type.h
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
#ifndef MOUSE_TYPE_H_
#define MOUSE_TYPE_H_
#include "../../proj/common/types.h"

#define MOUSE_FRAME_DATA_NUM   4

typedef struct {
	//u8 buff_id;
	u8 btn;
	s8 x;
	s8 y;
	s8 wheel;
	//s8 tl_wheel;
	//u8 hotkey;
}mouse_data_t;




#endif /* MOUSE_TYPE_H_ */
