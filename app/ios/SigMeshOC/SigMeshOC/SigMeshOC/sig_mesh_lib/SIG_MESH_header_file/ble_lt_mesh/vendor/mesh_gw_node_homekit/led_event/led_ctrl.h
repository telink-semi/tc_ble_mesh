/********************************************************************************************************
 * @file     led_ctrl.h
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2016, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary property of Telink
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in.
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/

#ifndef LED_CTRL_H_
#define LED_CTRL_H_

#include "tl_common.h"
#include "drivers.h"

#include "homekit_src/homekit_inc.h"
/* enable hardware timer 1, to proc led, ui and so on. */

extern void hk_led_ctrl_init();
extern void hk_led_ctrl_proc();


#endif /* LED_CTRL_H_ */
