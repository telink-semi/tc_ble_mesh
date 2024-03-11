/********************************************************************************************************
 * @file	clock_i.h
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

#ifndef WIN32
#include "proj/mcu/config.h"
#if(__TL_LIB_8258__ || MCU_CORE_TYPE == MCU_CORE_8258)
#include "drivers/8258/clock.h"
#elif(MCU_CORE_TYPE == MCU_CORE_8278)
#include "drivers/8278/clock.h"
#else

// we use clock insteady of timer, to differentiate OS timers utility
static inline void clock_enable_clock(int tmr, u32 en){
	if(0 == tmr){
		SET_FLD_V(reg_tmr_ctrl, FLD_TMR0_EN, en);
	}else if(1 == tmr){
		SET_FLD_V(reg_tmr_ctrl, FLD_TMR1_EN, en);
	}else{
		SET_FLD_V(reg_tmr_ctrl, FLD_TMR2_EN, en);
	}
}

#if WIN32
static inline unsigned __int64   clock_get_cycle()
{ 
  __asm   _emit   0x0F 
  __asm   _emit   0x31 
}
#endif

static inline u32 clock_time(){
#if WIN32
#if (__LOG_RT_ENABLE__)
	unsigned __int64 tt = (unsigned __int64)clock_get_cycle();
	u32 tick = (u32)(tt * 1000000 / (2*1000*1000*1000));	// assuming the cpu clock of PC is 2 giga HZ
	return tick;
#else
	static u32 tick = 0;
	// Experience data, not too accurate. A better practice is to set a 10-second timer and see how much the actual time differs.
	// and then adjust the ratio yourself. The system clock cannot be used because the timer needs to be kept accurate after the breakpoint
	tick += 2; //(CLOCK_SYS_CLOCK_1US * 20);	
	return tick;
#endif
#else
	//return reg_tmr0_tick;
	return reg_system_tick;
#endif
}

static inline u32 clock_time2(int tmr){
	return reg_tmr_tick(tmr);
}

// check if the current time is exceed span_us from ref time
#ifndef			USE_SYS_TICK_PER_US
static inline u32 clock_time_exceed(u32 ref, u32 span_us){
	return ((u32)(clock_time() - ref) > span_us * CLOCK_SYS_CLOCK_1US);
}
#else
static inline u32 clock_time_exceed(u32 ref, u32 span_us){
	return ((u32)(clock_time() - ref) > span_us * sys_tick_per_us);
}
#endif

// more efficient than clock_set_interval
static inline void clock_set_tmr_interval(int tmr, u32 intv){
	reg_tmr_capt(tmr) = intv;
}

static inline void clock_set_tmr_mode(int tmr, u32 m){
	if(0 == tmr){
		SET_FLD_V(reg_tmr_ctrl16, FLD_TMR0_MODE, m);
	}else if(1 == tmr){
		SET_FLD_V(reg_tmr_ctrl16, FLD_TMR1_MODE, m);
	}else{
		SET_FLD_V(reg_tmr_ctrl16, FLD_TMR2_MODE, m);
	}
}

static inline u32 clock_get_tmr_status(int tmr){
	if(0 == tmr){
		return reg_tmr_ctrl & FLD_TMR0_STA;
	}else if(1 == tmr){
		return reg_tmr_ctrl & FLD_TMR1_STA;
	}else{
		return reg_tmr_ctrl & FLD_TMR2_STA;
	}
}
#endif
#else
#include "../../../reference/tl_bulk/lib_file/hw_fun.h"
#endif
