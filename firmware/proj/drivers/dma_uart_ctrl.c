/********************************************************************************************************
 * @file	dma_uart_ctrl.c
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

#if 0

/**********************************************************************
 * INCLUDES
 */
#include "dma_uart_ctrl.h"
#include "dma_uart_hw.h"


/**********************************************************************
 * LOCAL CONSTANTS
 */

 
/**********************************************************************
 * LOCAL TYPES
 */

/**********************************************************************
 * LOCAL VARIABLES
 */
 
DmaUart_ctrl_t DmaUart_vs;

DmaUart_ctrl_t *DmaUart_v = &DmaUart_vs;

/**********************************************************************
 * GLOBAL VARIABLES
 */

extern u8* G_RxDestBuf;
extern u16 G_RxDestBufSize;
extern u8* G_TxSrcBuf;
extern u16 G_TxSrcBufSize;

/**********************************************************************
 * LOCAL FUNCTIONS
 */

uart_sts_t DmaUart_sendBulkData(void)
{
	
	u16 len;
	
	/* Get the length to send in this bulk transaction */
	len = (DmaUart_v->lenToSend > G_TxSrcBufSize) ? G_TxSrcBufSize : DmaUart_v->lenToSend;
	if (len == 0) 
	{
		return UART_SUCC;
	}

	//Every time before we send dma data, we need to check if the hardware is idle
	while(DmaUartTxHw_is_busy())
	{
		WaitMs(1);
	}
	
	DmaUart_v->lenToSend -= len;
	
	// Copy to DMA and start dma transmission
	G_TxSrcBuf[0] = len&(0xff);
	G_TxSrcBuf[1] = (len>>8)&(0xff);
	G_TxSrcBuf[2] = (len>>16)&(0xff);
	G_TxSrcBuf[3] = (len>>24)&(0xff);
	memcpy(&G_TxSrcBuf[4], &(DmaUart_v->txBuf[DmaUart_v->lastSendIndex]),len);
	DmaUart_v->lastSendIndex += len;
	StartDmaUartTx();

	return UART_SUCC;
}



uart_sts_t DmaUart_sendData(u8 *buf, u32 len)
{
	uart_sts_t SendBulkDataResult;

	/*If it is busy, return to add it to the queue*/
	if ((DmaUart_v->txBuf) || (DmaUartTxHw_is_busy())) 
	{
		return UART_BUSY;
	}


	/* Init the bulk transfer */
	DmaUart_v->lenToSend = len;
	DmaUart_v->txBuf = buf;
	DmaUart_v->lastSendIndex = 0;

	/* Send first bulk */
	SendBulkDataResult = DmaUart_sendBulkData();

	return SendBulkDataResult;
	
   
}

/*To check if DmaUart buffer (software) is available */
u8 DmaUart_isAvailable(void)
{
	return (DmaUart_v->txBuf == NULL);
}






#endif //END OF DMA_UART_ENABLE


