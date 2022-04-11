/********************************************************************************************************
 * @file	socket_test.h
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


#ifdef WIN32

#include<stdio.h>
#include<winsock2.h>

#include "../common/types.h"
#include "../common/assert.h"

#define RECEIVE_MAX_LENGTH  128
#define SEND_MAX_LENGTH     128
#define SOCKET_HOST         "127.0.0.1"
#define SOCKET_PORT_SERVER         7001
#define SOCKET_PORT_CLIENT         7002

#endif

