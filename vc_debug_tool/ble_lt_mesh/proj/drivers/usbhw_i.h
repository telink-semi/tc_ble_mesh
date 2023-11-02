/********************************************************************************************************
 * @file	usbhw_i.h
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

#include "../common/bit.h"
#include "../common/assert.h"
#include "../common/tutility.h"
#include "../mcu/irq_i.h"

#ifdef WIN32
	#include "../simu/usb_simu.h"
	#include <stdio.h>
#endif

static inline void usbhw_set_printer_threshold(u8 th) {
	reg_usb_ep8_send_thre = th;
}

static inline u32 usbhw_get_ctrl_ep_irq(void) {
	return reg_ctrl_ep_irq_sta;
}

static inline void usbhw_clr_ctrl_ep_irq(int irq) {
#ifdef WIN32
	CLR_FLD(reg_ctrl_ep_irq_sta, irq);
#else
	reg_ctrl_ep_irq_sta = irq;
#endif
}
static inline void usbhw_write_ctrl_ep_ctrl(u8 data) {
	reg_ctrl_ep_ctrl = data;
}

// Reset the buffer pointer
static inline void usbhw_reset_ctrl_ep_ptr(void) {
	reg_ctrl_ep_ptr = 0;
}

#if 0
#define usbhw_read_ctrl_ep_data()	(reg_ctrl_ep_dat)
#else
static inline u8 usbhw_read_ctrl_ep_data(void) {
#ifdef WIN32
	return 0;// usb_sim_ctrl_ep_buffer[usb_sim_ctrl_ep_ptr++];
#else
	return reg_ctrl_ep_dat;
#endif
}
#endif

static inline void usbhw_write_ctrl_ep_data(u8 data) {
	reg_ctrl_ep_dat = data;
#ifdef WIN32
	printf("%02x,", data);
#endif
}

static inline u8 usbhw_is_ctrl_ep_busy() {
	return reg_ctrl_ep_irq_sta & FLD_USB_EP_BUSY;
}

static inline u8 usbhw_read_ep_data(u32 ep) {
	return reg_usb_ep_dat(ep & 0x07);
}

static inline void usbhw_write_ep_data(u32 ep, u8 data) {
	reg_usb_ep_dat(ep & 0x07) = data;
#ifdef WIN32
	printf("%02x,", data);
#endif
}

static inline u32 usbhw_is_ep_busy(u32 ep) {
	return reg_usb_ep_ctrl(ep & 0x07) & FLD_USB_EP_BUSY;
}

static inline void usbhw_data_ep_ack(u32 ep) {
	reg_usb_ep_ctrl(ep & 0x07) = FLD_USB_EP_BUSY;
}

static inline void usbhw_data_ep_stall(u32 ep) {
	reg_usb_ep_ctrl(ep & 0x07) = FLD_USB_EP_STALL;
}

static inline void usbhw_reset_ep_ptr(u32 ep) {
	reg_usb_ep_ptr(ep & 0x07) = 0;
}


