/********************************************************************************************************
 * @file	s7816.h
 *
 * @brief	This is the header file for TLSR8258
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
#ifndef S7816_H_
#define S7816_H_

#include "gpio.h"

/**
 *  @brief  Define 7816 TRx pin
 */
typedef enum{
	S7816_TRX_D0,
	S7816_TRX_D3,
	S7816_TRX_D7,
}S7816_TRx_PinDef;

typedef enum {
	S7816_TX,
	S7816_RX,
}S7816_Half_Duplex_ModeDef;

/**
 * @brief      	This function is used to initiate 7816 module of MCU
 * @param[in]  	Div	-set the divider of clock of 7816 module:
 * 				7816clk = sysclk/(0x7b[6:4]*2),	0x7b[7]:enable 7816clk
 * 				7816clk:  0x40-2Mhz   0x20-4Mhz
 * 				baudrate: 0x40-16194  0x20-32388
 * @return     	none
 */
extern void s7816_set_clk(unsigned char Div);

/**
 * @brief      	This function is used to set address and size of buffer 7816 module of MCU
 * @param[in]  	*RecvAddr		-set the address of buffer to receive data
 * @param[in]  	RecvBufLen		-set the length of buffer to receive data
 * @return     	none
 */
extern void s7816_set_rx_buf(unsigned short *RecvAddr, unsigned short RecvBufLen);

/**
 * @brief      	This function is used to initiate 7816 module of MCU
 * @param[in]  	Pin_7816_TRX	-select the I/O 	pin of 7816 module
 * @param[in]	Pin_7816_RST	-select the RST 	pin of 7816 module
 * @param[in]	Pin_7816_VCC	-select the VCC 	pin of 7816 module
 * @return     	none
 */
extern void s7816_set_pin(S7816_TRx_PinDef Pin_7816_TRX);


/**
 * @brief      	This function is used to send data to ID card,after succeeding in getting ATR
 * @param[in]  	*TransAddr	- data is waitting to send
 * @return     	none
 */
extern unsigned char s7816_dma_send(unsigned char *TransAddr);

/**
 * @brief      	This function is used to transform half duplex mode of 7816
 * @param[in]  	mode	- half_duplex_mode_TX/RX is transformed by setting 0x9b[5]
 * 						0x9b[5]=1:half_duplex_mode_RX;0x9b[5]=0:half_duplex_mode_TX
 * @return     	none
 */
extern void s7816_set_half_duplex(S7816_Half_Duplex_ModeDef mode);


#endif /* S7816_H_ */

