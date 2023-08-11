/********************************************************************************************************
 * @file	main.c
 *
 * @brief	for TLSR chips
 *
 * @author	telink
 * @date	Sep. 30, 2010
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
#include "tl_common.h"

#include "proj/mcu/watchdog_i.h"
#include "vendor/common/user_config.h"
#include "proj_lib/rf_drv.h"
#include "proj_lib/pm.h"
#include "proj_lib/ble/ll/ll.h"
#include "proj_lib/ble/service/ble_ll_ota.h"




#if (    __PROJECT_8261_MASTER_KMA_DONGLE__ || __PROJECT_8266_MASTER_KMA_DONGLE__ \
	  || __PROJECT_8267_MASTER_KMA_DONGLE__ || __PROJECT_8269_MASTER_KMA_DONGLE__ )






#if (HCI_ACCESS==HCI_USE_UART)
extern my_fifo_t hci_rx_fifo;
#endif
extern void user_init();
extern void usb_endpoints_irq_handler (void);
extern void usb_init(void) ;
extern void main_loop (void);

_attribute_ram_code_ void irq_handler(void)
{
	irq_blt_sdk_handler ();


#if (UI_AUDIO_ENABLE)
	if(reg_irq_src & FLD_IRQ_IRQ4_EN){
		usb_endpoints_irq_handler();
	}
#endif


#if (HCI_ACCESS==HCI_USE_UART)
	unsigned char irqS = reg_dma_rx_rdy0;
    if(irqS & FLD_DMA_CHN_UART_RX)	//rx
    {
    	reg_dma_rx_rdy0 = FLD_DMA_CHN_UART_RX;
    	u8* w = hci_rx_fifo.p + (hci_rx_fifo.wptr & (hci_rx_fifo.num-1)) * hci_rx_fifo.size;
    	if(w[0]!=0)
    	{
    		my_fifo_next(&hci_rx_fifo);
    		u8* p = hci_rx_fifo.p + (hci_rx_fifo.wptr & (hci_rx_fifo.num-1)) * hci_rx_fifo.size;
    		reg_dma0_addr = (u16)((u32)p);
    	}
    }

    if(irqS & FLD_DMA_CHN_UART_TX)	//tx
    {
    	reg_dma_rx_rdy0 = FLD_DMA_CHN_UART_TX;
    }
#endif
}

int main (void) {

	cpu_wakeup_init();

	set_tick_per_us (CLOCK_SYS_CLOCK_HZ/1000000);
	clock_init();

	gpio_init();

	rf_drv_init(CRYSTAL_TYPE);

	usb_init ();

	user_init ();

    irq_enable();

	while (1) {
#if (MODULE_WATCHDOG_ENABLE)
		wd_clear(); //clear watch dog
#endif
		main_loop ();
	}
}


#endif
