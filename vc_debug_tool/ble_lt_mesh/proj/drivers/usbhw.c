/********************************************************************************************************
 * @file	usbhw.c
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
#include "usbhw.h"
#include "../mcu/irq_i.h"
#include "../common/bit.h"
#include "../common/assert.h"
#include "../common/tutility.h"
#include "usbhw_i.h"

// Endpont8 is the alias of endpoint0
void usbhw_disable_manual_interrupt(int m) {
	SET_FLD(reg_ctrl_ep_irq_mode, m);
}

void usbhw_enable_manual_interrupt(int m) {
	CLR_FLD(reg_ctrl_ep_irq_mode, m);
}

void usbhw_write_ep(u32 ep, u8 * data, int len) {
	assert(ep < 8);
	reg_usb_ep_ptr(ep) = 0;

	foreach(i,len){
		reg_usb_ep_dat(ep) = data[i];
	}
	reg_usb_ep_ctrl(ep) = FLD_EP_DAT_ACK;		// ACK
}

// handy help function
void usbhw_write_ctrl_ep_u16(u16 v){
	usbhw_write_ctrl_ep_data(v & 0xff);
	usbhw_write_ctrl_ep_data(v >> 8);
}

u16 usbhw_read_ctrl_ep_u16(void){
	u16 v = usbhw_read_ctrl_ep_data();
	return (usbhw_read_ctrl_ep_data() << 8) | v;
} 

