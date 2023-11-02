/********************************************************************************************************
 * @file	timer.h
 *
 * @brief	This is the source file for TLSR8278
 *
 * @author	Driver Group
 * @date	May 8, 2018
 *
 * @par     Copyright (c) 2018, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#ifndef TIMER_H_
#define TIMER_H_
#include "compiler.h"
#include "register.h"
#include "analog.h"
#include "gpio.h"

/**
 * @brief   Type of Timer
 */
typedef enum{
	TIMER0		=0,
	TIMER1		=1,
	TIMER2		=2,
}TIMER_TypeDef;

/**
 * @brief   Mode of Timer
 */
typedef enum{
	TIMER_MODE_SYSCLK		=0,
	TIMER_MODE_GPIO_TRIGGER	=1,
	TIMER_MODE_GPIO_WIDTH	=2,
	TIMER_MODE_TICK			=3,
}TIMER_ModeTypeDef;

#define	    sys_tick_per_us   				16
#define		CLOCK_SYS_CLOCK_1250US			(1250 * sys_tick_per_us)

/**
 * @brief   system Timer : 16Mhz, Constant
 */
enum{
	CLOCK_16M_SYS_TIMER_CLK_1S  =  16*1000*1000,
	CLOCK_16M_SYS_TIMER_CLK_1MS = 16*1000,
	CLOCK_16M_SYS_TIMER_CLK_1US = 16,
};

#if 1 // add by weixiong in mesh
#define CLOCK_SYS_CLOCK_1S		CLOCK_16M_SYS_TIMER_CLK_1S
#define CLOCK_SYS_CLOCK_1MS		CLOCK_16M_SYS_TIMER_CLK_1MS
#define CLOCK_SYS_CLOCK_1US		CLOCK_16M_SYS_TIMER_CLK_1US

enum{
	CLOCK_MCU_RUN_CODE_1S = (CLOCK_SYS_CLOCK_HZ),         // 8258: not same with CLOCK_SYS_CLOCK_1S.
	CLOCK_MCU_RUN_CODE_1MS = (CLOCK_MCU_RUN_CODE_1S / 1000),
	CLOCK_MCU_RUN_CODE_1US = (CLOCK_MCU_RUN_CODE_1S / 1000000),
};

#define 	sys_tick_per_us					16
#define		CLOCK_SYS_CLOCK_1250US			(1250 * sys_tick_per_us)
#endif

/**
 * @brief     This function performs to gets system timer0 address.
 * @param[in] none.
 * @return    timer0 address.
 */

static inline unsigned long clock_time(void)
{
	return reg_system_tick;
}

/**
 * @brief     This function performs to set the system timer irq capture tick.
 * @param[in] none.
 * @return    timer0 address.
 */
static inline void systimer_set_capture_tick(unsigned long tick)
{
	//0x744 bit[2:0] is always 0, to ensure system timer can be trigger correctly, bit[2:0] of the setting tick should 
	//set to 0 to match the value of 0x744
	reg_system_tick_irq = (tick & (~((unsigned long)0x07)));
}


/**
 * @brief     This function performs to set sleep us.
 * @param[in] us - microseconds need to delay.
 * @return    none
 */

extern void sleep_us(unsigned long us);

#define ClockTime			clock_time
//#define WaitUs				sleep_us // comment by weixiong in mesh
#define WaitMs(t)			sleep_us((t)*1000)
#define sleep_ms(t)			sleep_us((t)*1000)

/**
 * @brief     This function performs to calculation exceed us of the timer.
 * @param[in] ref - Variable of reference timer address.
 * @param[in] span_us - Variable of span us.
 * @return    the exceed.
 */

static inline unsigned int clock_time_exceed(unsigned int ref, unsigned int us)
{
	return ((unsigned int)(clock_time() - ref) > us * 16);
}

/**
 * @brief     initiate GPIO for gpio trigger and gpio width mode of timer0.
 * @param[in] pin - select pin for timer0.
 * @param[in] pol - select polarity for gpio trigger and gpio width
 * @return    none
 */
extern void timer0_gpio_init(GPIO_PinTypeDef pin, GPIO_PolTypeDef pol);

/**
 * @brief     initiate GPIO for gpio trigger and gpio width mode of timer1.
 * @param[in] pin - select pin for timer1.
 * @param[in] pol - select polarity for gpio trigger and gpio width
 * @return    none
 */
extern void timer1_gpio_init(GPIO_PinTypeDef pin, GPIO_PolTypeDef pol);

/**
 * @brief     initiate GPIO for gpio trigger and gpio width mode of timer2.
 * @param[in] pin - select pin for timer2.
 * @param[in] pol - select polarity for gpio trigger and gpio width
 * @return    none
 */
extern void timer2_gpio_init(GPIO_PinTypeDef pin,GPIO_PolTypeDef pol);

/**
 * @brief     set mode, initial tick and capture of timer0.
 * @param[in] mode - select mode for timer0.
 * @param[in] init_tick - initial tick.
 * @param[in] cap_tick  - tick of capture.
 * @return    none
 */
extern void timer0_set_mode(TIMER_ModeTypeDef mode,unsigned int init_tick, unsigned int cap_tick);

/**
 * @brief     set mode, initial tick and capture of timer1.
 * @param[in] mode - select mode for timer1.
 * @param[in] init_tick - initial tick.
 * @param[in] cap_tick  - tick of capture.
 * @return    none
 */
extern void timer1_set_mode(TIMER_ModeTypeDef mode,unsigned int init_tick, unsigned int cap_tick);

/**
 * @brief     set mode, initial tick and capture of timer2.
 * @param[in] mode - select mode for timer2.
 * @param[in] init_tick - initial tick.
 * @param[in] cap_tick  - tick of capture.
 * @return    none
 */
extern void timer2_set_mode(TIMER_ModeTypeDef mode,unsigned int init_tick, unsigned int cap_tick);

/**
 * @brief     the specifed timer start working.
 * @param[in] type - select the timer to start.
 * @return    none
 */
extern void timer_start(TIMER_TypeDef type);

/**
 * @brief     the specifed timer stop working.
 * @param[in] type - select the timer to stop.
 * @return    none
 */
extern void timer_stop(TIMER_TypeDef type);



#endif /* TIMER_H_ */
