/********************************************************************************************************
 * @file	dma.h
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

#include "../mcu/register.h"
#ifndef WIN32
static inline void dma_init(){
	reg_dma_chn_irq_msk =
			(RF_RX_USE_DMA_IRQ ? FLD_DMA_CHN_RF_RX : 0) | (RF_TX_USE_DMA_IRQ ? FLD_DMA_CHN_RF_TX : 0);
}

static inline void dma_irq_clear(){
	reg_dma_chn_irq_msk = 0;
}

static inline void dma_channel_enable_all(){
	reg_dma_chn_en = 0xff;
}

static inline void dma_channel_disable_all(){
	reg_dma_chn_en = 0;
}
#endif
