/********************************************************************************************************
 * @file	rf_pa.c
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
#include "rf_pa.h"
#include "proj_lib/rf_drv.h"
#include "proj_lib/ble/blt_config.h"

#if (PA_ENABLE)
#define USE_SOFTWARE_PA		0

	#if (0 == USE_SOFTWARE_PA)
STATIC_ASSERT(MCU_CORE_TYPE >= MCU_CORE_8258);
		#if ((MCU_CORE_TYPE == MCU_CORE_8258) || (MCU_CORE_TYPE == MCU_CORE_8278))
// if compile error here, please set USE_SOFTWARE_PA to 1.
STATIC_ASSERT(((u32)PA_RXEN_PIN==(u32)RFFE_RX_PB2)||((u32)PA_RXEN_PIN==(u32)RFFE_RX_PC6)||((u32)PA_RXEN_PIN==(u32)RFFE_RX_PD0));
STATIC_ASSERT(((u32)PA_TXEN_PIN==(u32)RFFE_TX_PB3)||((u32)PA_TXEN_PIN==(u32)RFFE_TX_PC7)||((u32)PA_TXEN_PIN==(u32)RFFE_TX_PD1));
		#endif
	#endif
#endif

rf_pa_callback_t  blc_rf_pa_cb = NULL;

#if(PA_ENABLE && USE_SOFTWARE_PA)
_attribute_ram_code_ void app_rf_pa_handler(int type)
{
	u32 r = irq_disable();	// add irq disable should be better due to called in both main loop and irq.
	
	if(type == PA_TYPE_TX_ON){
	    //gpio_set_output_en(PA_RXEN_PIN, 1); // not necessary
	    gpio_write(PA_RXEN_PIN, 0);
	    //gpio_set_output_en(PA_TXEN_PIN, 1); // not necessary
	    gpio_write(PA_TXEN_PIN, 1);
	}
	else if(type == PA_TYPE_RX_ON){
	    //gpio_set_output_en(PA_TXEN_PIN, 1); // not necessary
	    gpio_write(PA_TXEN_PIN, 0);
	    //gpio_set_output_en(PA_RXEN_PIN, 1); // not necessary
	    gpio_write(PA_RXEN_PIN, 1);
	}
	else{
	    //gpio_set_output_en(PA_RXEN_PIN, 1); // not necessary
	    gpio_write(PA_RXEN_PIN, 0);
	    //gpio_set_output_en(PA_TXEN_PIN, 1); // not necessary
	    gpio_write(PA_TXEN_PIN, 0);
	}

	irq_restore(r);
}
#endif

void rf_pa_init(void)
{
#if(PA_ENABLE)
    extern u8 my_rf_power_index;
	rf_set_power_level_index (my_rf_power_index);
	#if(USE_SOFTWARE_PA)
    gpio_set_func(PA_TXEN_PIN, AS_GPIO);
    gpio_set_output_en(PA_TXEN_PIN, 1);
    gpio_write(PA_TXEN_PIN, 0);

    gpio_set_func(PA_RXEN_PIN, AS_GPIO);
    gpio_set_output_en(PA_RXEN_PIN, 1);
    gpio_write(PA_RXEN_PIN, 0);

    blc_rf_pa_cb = app_rf_pa_handler;
	#else
	rf_rffe_set_pin(PA_TXEN_PIN, PA_RXEN_PIN);
	#endif	
#endif
}

