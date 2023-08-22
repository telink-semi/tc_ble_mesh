/********************************************************************************************************
 * @file	button.c
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
#include "button.h"

#if(0)

u32 button_table[MAX_BUTTON_NUM];
u8  button_curNum = 0;
buttonPressedCb_t button_cb;
u32 buttonLastTime = 0;

void button_config(u32 pin)
{
	if (button_curNum >= MAX_BUTTON_NUM) {
		return;
	}

	gpio_set_func(pin, AS_GPIO); 
	gpio_set_input_en(pin, 1);
	gpio_set_interrupt(pin, 1);

	reg_irq_mask |= FLD_IRQ_GPIO_RISC2_EN;

	button_table[button_curNum++] = pin;
}



void button_registerCb(buttonPressedCb_t cb)
{
	button_cb = cb;
}



/*********************************************************************
 * @fn      gpio_user_irq_handler
 *
 * @brief   GPIO interrupt handler.
 *
 * @param   None
 *
 * @return  None
 */

u8 T_BTN_IRQ;
void gpio0_user_irq_handler(void)
{    
    u32 arg = 0;
    u8 i;
	T_BTN_IRQ++;

#if(SP_SRC_SPI)
	extern void spi_irq_callback();
	spi_irq_callback();
#endif
	/* This procedure is used to DISAPPEARS SHAKES */
    if ( !clock_time_exceed(buttonLastTime, BUTTON_DISAPPEAR_SHAKE_TIMER) ) {
        return;
    }
    
    buttonLastTime = clock_time();
    for(i = 0; i < button_curNum; i++) {
    	if (0 == gpio_read(button_table[i])) {
    		arg = button_table[i];
    		break;
    	}
    }

    if(arg != 0 && button_cb) {
        EV_SCHEDULE_TASK((ev_task_callback_t)button_cb, (void*)arg);
    }

}
#endif

