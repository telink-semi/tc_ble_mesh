/********************************************************************************************************
 * @file	compatibility.h
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
#include "tutility.h"

/////////////////////////////////////////////////////
//  compatibility for old api, 	 no so good but keep them for the moment

#define WriteAnalogReg(addr,v)	analog_write(addr,v)
#define ReadAnalogReg(addr)		analog_read(addr)

//extern void sleep_us (unsigned int us);

#define WaitUs(t)			sleep_us(t)
#define WaitMs(t)			sleep_us((t)*1000)

#ifdef WIN32
#define write_reg8(addr,v)	U8_SET((addr - 0x800000 + REG_BASE_ADDR),v)
#define write_reg16(addr,v)	U16_SET((addr - 0x800000 + REG_BASE_ADDR),v)
#define write_reg32(addr,v)	U32_SET((addr - 0x800000 + REG_BASE_ADDR),v)
#define read_reg8(addr)		U8_GET((addr - 0x800000 + REG_BASE_ADDR))
#define read_reg16(addr)	U16_GET((addr - 0x800000 + REG_BASE_ADDR))
#define read_reg32(addr)	U32_GET((addr - 0x800000 + REG_BASE_ADDR))
#else
#define write_reg8(addr,v)	U8_SET((addr + REG_BASE_ADDR),v)
#define write_reg16(addr,v)	U16_SET((addr + REG_BASE_ADDR),v)
#define write_reg32(addr,v)	U32_SET((addr + REG_BASE_ADDR),v)
#define read_reg8(addr)		U8_GET((addr + REG_BASE_ADDR))
#define read_reg16(addr)	U16_GET((addr + REG_BASE_ADDR))
#define read_reg32(addr)	U32_GET((addr + REG_BASE_ADDR))
#define set_reg8_bit(addr,v)    write_reg8(addr, read_reg8(addr)|(v));
#define reset_reg8_bit(addr,v)  write_reg8(addr, read_reg8(addr)&(~(v)));
#endif

#define WRITE_REG8				write_reg8
#define WRITE_REG16				write_reg16
#define WRITE_REG32				write_reg32

#define READ_REG8				read_reg8
#define READ_REG16				read_reg16
#define READ_REG32				read_reg32

#define TCMD_UNDER_RD		0x80
#define TCMD_UNDER_WR		0x40
#define TCMD_UNDER_BOTH		0xc0
#define TCMD_MASK			0x3f

#define TCMD_WRITE			0x3
#define TCMD_WAIT			0x7
#define TCMD_WAREG			0x8

typedef struct TBLCMDSET {
	union{
		unsigned short	adr;
		unsigned short	ADR;
	};
	union{
		unsigned char	dat;
		unsigned char	DAT;
	};
	union{
		unsigned char	cmd;
		unsigned char	CMD;
	};
} TBLCMDSET;

int LoadTblCmdSet (	const TBLCMDSET * pt, int size);



