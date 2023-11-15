/********************************************************************************************************
 * @file	dma_uart_hw.c
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
#include "../tl_common.h"
#include "dma_uart_hw.h"

#if(MCU_CORE_TYPE != MCU_CORE_8263)

#define __GPIO_DEBUG__			0
#define DEBUG_GPIO(pin, high)	{if(__GPIO_DEBUG__) gpio_write(pin,high);}

/////  RX  buffer  ///////////////
#define UART_RX_BUF_SIZE	24
u8 uartRxBuf[UART_RX_BUF_SIZE*2];
u8 *uartRxBufPtr = &uartRxBuf[0];

u32 uartBaudrate;
void uartOpen(u32 baudrate){
	uartBaudrate = baudrate;
	// refine this,  看如何做到误差最小,  acceptable now
	if(115200 == baudrate){
		const u8 bpwc = 5;
		reg_uart_clk_div = FLD_UART_CLK_DIV_EN | (((CLOCK_SYS_CLOCK_HZ / bpwc) + baudrate / 2) / baudrate - 1 );
		reg_uart_ctrl0 = (bpwc - 1) | FLD_UART_RX_DMA_EN | FLD_UART_TX_DMA_EN;
	}else{
		const u8 bpwc = 4;
		reg_uart_clk_div = FLD_UART_CLK_DIV_EN | (((CLOCK_SYS_CLOCK_HZ / bpwc) + baudrate / 2) / baudrate - 1 );
		reg_uart_ctrl0 = (bpwc - 1) | FLD_UART_RX_DMA_EN | FLD_UART_TX_DMA_EN;
	}
	// 从示波器看，这个时间计算跟文档不一样
	reg_uart_rx_timeout = (4 * (1 + 8 + 1)) | BIT(8);	//  10 bits timeout,  太长的时间会影响功耗,  因为 rx 完成中断来得太慢
	reg_dma_uart_rx_addr = (u16)(u32)(uartRxBufPtr);
#if(__GPIO_DEBUG__)	
	reg_gpio_pc_oen = ~(GPIO_PC2 | GPIO_PC3 | GPIO_PC4 | GPIO_PC5);
#endif
}

// RX 写成这样是希望能够从uart DMA 直接转到RF DMA,  而不需要拷贝内存。
// 做到最快的透传速度,  但是由于 att write 命令数据不是4 自己对齐，所以无法做到。
static inline void uartSetNextRxBuff(){
	if(uartRxBufPtr == &uartRxBuf[0]){
		uartRxBufPtr += UART_RX_BUF_SIZE;
	}else{
		uartRxBufPtr = &uartRxBuf[0];
	}
	reg_dma_uart_rx_addr = (u16)(u32)(uartRxBufPtr);
}

#if 0
void uart_irq_handler(void){
	u8 irq = reg_dma_rx_rdy0;
	if(irq & FLD_DMA_CHN_UART_TX){		/* Received packet from DMA_UART*/
		DEBUG_GPIO(GPIO_PC4, 1);
		u8* buf = uartRxBufPtr;
		uartSetNextRxBuff();
		reg_dma_rx_rdy0 = 0xff; 	// clear all interrupt flag
		uartRxHandler(buf + 4, buf[0]);
		DEBUG_GPIO(GPIO_PC4, 0);
	}else{
		reg_dma_rx_rdy0 = 0xff; 	// clear all interrupt flag
	}
}
#endif
	
int uartTxBusy(void){
	return reg_dma_tx_rdy0 & FLD_DMA_CHN_UART_TX;
}

#define UART_TX_BUF_CNT		4
u8 *uartTxBufs[UART_TX_BUF_CNT];
u8 uartTxWptr;
u8 uartTxRptr;

u32 uartTxEndTime;
u8 uartTxPending;
static void uartTx(){
	DEBUG_GPIO(GPIO_PC2, 0);
	u8* buf = uartTxBufs[uartTxRptr];
	reg_dma_uart_tx_addr = (u16)(u32)(buf); 		//	DMA_SET_UART_TX_SRC_SIZE(128+1);	//one byte for length
	reg_dma_tx_rdy0 |= FLD_DMA_CHN_UART_TX; 			//	DMA Packet0 Channel 1(TX) ready for transfer, this bit will be cleared automatically
	++uartTxRptr;
	if(uartTxRptr == UART_TX_BUF_CNT){
		uartTxRptr = 0;
	}
	uartTxEndTime = reg_system_tick + (buf[0] * (CLOCK_SYS_CLOCK_1US * 1000*1000) / (uartBaudrate / (1+8+1))) + 200*CLOCK_SYS_CLOCK_1US;
	uartTxPending = 1;
}

void uartWrite(u8 *addr){
	DEBUG_GPIO(GPIO_PC3, 1);
	u8 nextPtr = uartTxWptr + 1;
	if(nextPtr == UART_TX_BUF_CNT){
		nextPtr = 0;
	}
	if(nextPtr != uartTxRptr){
		uartTxBufs[uartTxWptr] = addr;
		uartTxWptr = nextPtr;
	}
	if(!uartTxPending){
		uartTx();
	}
}

void uartTxCheck(){
	if(uartTxPending){
		u32 t = reg_system_tick - uartTxEndTime;
		if(((t > 0) && (t < 0xfffffff))){
			DEBUG_GPIO(GPIO_PC2, 1);
			DEBUG_GPIO(GPIO_PC3, 0);
			if(uartTxRptr != uartTxWptr){
				uartTx();
			}else{
				uartTxPending = 0;
			}
		}
	}else{
		if(uartTxRptr != uartTxWptr){
			uartTx();
		}
	}
}

void uartClose(){
	reg_uart_ctrl0 = 0;
	uartTxPending = uartTxRptr = uartTxWptr = 0;
}

int uartIsIdle(){
	return (0 == uartTxPending) && (uartTxRptr == uartTxWptr);
}

#endif
