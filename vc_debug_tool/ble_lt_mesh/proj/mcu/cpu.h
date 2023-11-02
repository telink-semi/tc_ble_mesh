/********************************************************************************************************
 * @file	cpu.h
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
#include "register.h"
#ifndef WIN32
static inline void cpu_reset(void){
	SET_FLD(reg_rst_clk0, FLD_RST_MCU | FLD_RST_SOFT_MCIC);		// Reset instruction cache
}

static inline void cpu_set_wakeup_src(int src){
	SET_FLD(reg_wakeup_en, src);
}

static inline void cpu_clr_wakeup_src(int src){
	CLR_FLD(reg_wakeup_en, src);
}

static inline void cpu_disable_wakeup(void){
	reg_wakeup_en = 0;
}

void cpu_suspend(void);
void cpu_reboot(void);
#endif
#ifdef __GNUC__
#define _ASM_NOP_				asm("tnop")
#else
#define _ASM_NOP_
#endif

