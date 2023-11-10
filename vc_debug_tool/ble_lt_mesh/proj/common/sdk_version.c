/********************************************************************************************************
 * @file	sdk_version.c
 *
 * @brief	This is the header file for B85m
 *
 * @author	Driver Group
 * @date	2022
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#include "sdk_version.h"
#include "vendor/common/version.h"

#define SDK_VER_FLAG	'$','$','$'
#define SDK_VER_NAME	't','e','l','i','n','k','_','s','i','g','_','m','e','s','h','_','s','d','k'
#define SDK_VER_CHAR	'V',VER_NUM2CHAR(SDK_VER_SPEC),'.',VER_NUM2CHAR(SDK_VER_MAJOR),'.',VER_NUM2CHAR(SDK_VER_MINOR),'.',VER_NUM2CHAR(SDK_VER_2ND_MINOR)

#if SDK_VER_PATCH
#define SDK_VER_CHAR_PATCH	'.',VER_NUM2CHAR(SDK_VER_PATCH),
#else
#define SDK_VER_CHAR_PATCH	
#endif

volatile __attribute__((section(".sdk_version"))) const unsigned char const_sdk_version[] = {SDK_VER_FLAG,SDK_VER_NAME,'_',SDK_VER_CHAR,SDK_VER_CHAR_PATCH  SDK_VER_FLAG}; // can not extern, due to no loading in cstartup.


