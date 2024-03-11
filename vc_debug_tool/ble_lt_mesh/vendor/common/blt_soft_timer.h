/********************************************************************************************************
 * @file	blt_soft_timer.h
 *
 * @brief	for TLSR chips
 *
 * @author	public@telink-semi.com;
 * @date	Sep. 18, 2015
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
/*
 * blt_soft_timer.h
 *
 *  Created on: 2016-10-28
 *      Author: Administrator
 */

#ifndef BLT_SOFT_TIMER_H_
#define BLT_SOFT_TIMER_H_
#include "proj/mcu/config.h"
#if(MCU_CORE_TYPE != MCU_CORE_8269)


//user define
#ifndef BLT_SOFTWARE_TIMER_ENABLE
#define BLT_SOFTWARE_TIMER_ENABLE					0   //enable or disable
#endif


#define 	MAX_TIMER_NUM							4   //timer max number


#define		MAINLOOP_ENTRY							0
#define 	CALLBACK_ENTRY							1



//if t1 < t2  return 1
#define		TIME_COMPARE_SMALL(t1,t2)   ( (u32)((t2) - (t1)) < BIT(30)  )

// if t1 > t2 return 1
#define		TIME_COMPARE_BIG(t1,t2)   ( (u32)((t1) - (t2)) < BIT(30)  )


#define		BLT_TIMER_SAFE_MARGIN_PRE	  (CLOCK_16M_SYS_TIMER_CLK_1US<<7)  //128 us
#define		BLT_TIMER_SAFE_MARGIN_POST	  (CLOCK_16M_SYS_TIMER_CLK_1S<<3)   // 8S
static int inline blt_is_timer_expired(u32 t, u32 now) {
	return ((u32)(now + BLT_TIMER_SAFE_MARGIN_PRE - t) < BLT_TIMER_SAFE_MARGIN_POST);
}



#include "tl_common.h"
#if LLSYNC_ENABLE
#include "ble_qiot_utils_mesh.h"

typedef void (*blt_timer_callback_t)(void *param);	//#define blt_timer_callback_t	ble_timer_cb
#else
typedef int (*blt_timer_callback_t)(void);
#endif



typedef struct blt_time_event_t {
	blt_timer_callback_t    cb;
	u32                     t;
	u32                     interval;
} blt_time_event_t;


// timer table managemnt
typedef struct blt_soft_timer_t {
	blt_time_event_t	timer[MAX_TIMER_NUM];  //timer0 - timer3
	u8					currentNum;  //total valid timer num
} blt_soft_timer_t;




//////////////////////// USER  INTERFACE ///////////////////////////////////
//return 0 means Fail, others OK
int 	blt_soft_timer_add(blt_timer_callback_t func, u32 interval_us);
int 	blt_soft_timer_delete(blt_timer_callback_t func);
int 	blt_soft_timer_update(blt_timer_callback_t func, u32 interval_us);




//////////////////////// SOFT TIMER MANAGEMENT  INTERFACE ///////////////////////////////////
void 	blt_soft_timer_init(void);
void  	blt_soft_timer_process(int type);
int 	blt_soft_timer_delete_by_index(u8 index);


int is_timer_expired(blt_timer_callback_t *e);
int is_soft_timer_exist(blt_timer_callback_t func);
u8 blt_soft_timer_cur_num();

#endif
#endif /* BLT_SOFT_TIMER_H_ */
