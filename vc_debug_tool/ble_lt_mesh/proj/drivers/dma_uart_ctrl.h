/********************************************************************************************************
 * @file	dma_uart_ctrl.h
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

#include "../common/types.h"
#include "../common/static_assert.h"
#include "../common/bit.h"
#include "uart.h"
//#include "../usbstd/CDCClassCommon.h"
//#include "../usbstd/CDCClassDevice.h"

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
    extern "C" {
#endif

typedef struct {
	u8 *rxBuf;
	u8 *txBuf;

	/* Following variables are used in the RX more than DMA_MAX_SIZE */
	u16 lenToSend;
	u16 lastSendIndex;
	
} DmaUart_ctrl_t;

uart_sts_t DmaUart_sendData(u8* buf, u32 len);
uart_sts_t DmaUart_sendBulkData(void);
u8   DmaUart_isAvailable(void);

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
    }
#endif

