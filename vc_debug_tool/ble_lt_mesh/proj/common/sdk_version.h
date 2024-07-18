/********************************************************************************************************
 * @file	sdk_version.h
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
#pragma once

/*
 * It is only used by SDK, and the customer is not allowed to modify it
 */


/*
 * Release Tool need to change this macro to match the release version,
 * the replace rules is: "$$$B85m_driver_sdk_"#sdk_version_num"$$$", The "#sdk_version_num"
 * will replace with this macro value.
 */

#define SDK_VER_SPEC			(4)		// SDK version, don't change!!! //"3" means SIG MESH 1.0.x
#define SDK_VER_MAJOR			(1)		// SDK version, don't change!!! 
#define SDK_VER_MINOR			(0)		// SDK version, don't change!!! 
#define SDK_VER_2ND_MINOR		(1)		// SDK version, don't change!!!	// second minor

//#define SDK_VER_PATCH_EN		(0)		// SDK version, don't change!!!	// patch version, 0 or nothing means no patch.

//#define	SDK_VERSION1(sdk_version_num)	"$$$B85m_driver_sdk_"#sdk_version_num"$$$"
//#define	SDK_VERSION(sdk_version_num)	SDK_VERSION1(sdk_version_num)


