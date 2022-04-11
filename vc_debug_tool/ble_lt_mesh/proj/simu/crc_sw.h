/********************************************************************************************************
 * @file	crc_sw.h
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

/**************************** Public Macro Definitions ***********************/

#define CRC_MODE0_POLY  0x1021
#define CRC_MODE1_POLY  0x8005
#define CRC_MODE2_POLY  0x8bb7
#define CRC_MODE3_POLY  0x04c11db7

enum {
    CRC_MODE_0 = 0,
    CRC_MODE_1,
    CRC_MODE_2,
    CRC_MODE_3,
};

/**************************** Public Type Definitions ************************/

/* None */

/**************************** Public Enum Definitions ************************/

/* None */

/**************************** Public Variable Declaration ********************/

extern u16 crc16_mode_poly[3];

/**************************** Public Constant Declaration ********************/

/* None */

/**************************** Public Function Prototypes *********************/

void crc32_buildTable(u8 endian);
u32 crc32_getCrc(const u8 *str, u32 count, u8 endian);
u32 crc_reverseBitU32(const u32 src);
u8 reverseBitU8(const u8 src);
u32 crc32_bitwise(u8 *p_stream, u32 len_m1, u32 crc32_poly);
u16 crc16_bitwise(u8 *p_stream, u32 len_m1, u16 crc16_poly);

u16 crc16_calc(u8* crcArray, u8 len);



