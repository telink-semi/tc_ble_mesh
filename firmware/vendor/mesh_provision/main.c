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
#include "stack/ble/ble.h"
#include "drivers.h"
#include "vendor/common/mesh_common.h"
extern void user_init();
extern void main_loop ();

#if (HCI_ACCESS==HCI_USE_UART)
extern my_fifo_t hci_rx_fifo;

u16 uart_tx_irq=0, uart_rx_irq=0;

_attribute_ram_code_ void irq_uart_handle()
{
	unsigned char irqS = reg_dma_rx_rdy0;
	if(irqS & FLD_DMA_CHN_UART_RX)	//rx
	{
		uart_rx_irq++;
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
		uart_tx_irq++;
		reg_dma_rx_rdy0 = FLD_DMA_CHN_UART_TX;
	}

    #if(MCU_CORE_TYPE == MCU_CORE_TC321X)
	if(reg_uart_status1(UART0) & FLD_UART_TX_DONE)
	{
		uart0_tx_done_flag = 1;
		uart_clr_tx_done(UART0);
	}
    #endif
}
#endif

_attribute_ram_code_ void irq_handler(void)
{
	irq_blt_sdk_handler ();  //ble irq proc

#if (HCI_ACCESS==HCI_USE_UART)
	irq_uart_handle();
#endif
}

FLASH_ADDRESS_DEFINE;

#if (PM_DEEPSLEEP_RETENTION_ENABLE)
_attribute_ram_code_
#endif
int main (void) // must run in ramcode if enable retention sleep
{
#if BLE_OTA_SERVER_ENABLE
    blc_ota_setFirmwareSizeAndBootAddress(FW_SIZE_MAX_K, FLASH_ADR_UPDATE_NEW_FW);
#endif
	blc_pm_select_internal_32k_crystal();

#if(MCU_CORE_TYPE == MCU_CORE_825x)
    cpu_wakeup_init();
#else
    cpu_wakeup_init(LDO_MODE,INTERNAL_CAP_XTAL24M);
#endif

	int deepRetWakeUp = pm_is_MCU_deepRetentionWakeup();  //MCU deep retention wakeUp

	rf_drv_ble_init();

	gpio_init(!deepRetWakeUp );  //analog resistance will keep available in deepSleep mode, so no need initialize again

	clock_init(SYS_CLK_TYPE);

#if	(PM_DEEPSLEEP_RETENTION_ENABLE)
	if(deepRetWakeUp){
		user_init_deepRetn();
	}
	else
#endif
	{
		user_init();
	}

    irq_enable();

	while (1) {
#if (MODULE_WATCHDOG_ENABLE)
		wd_clear(); //clear watch dog
#endif
		main_loop();
	}
}

