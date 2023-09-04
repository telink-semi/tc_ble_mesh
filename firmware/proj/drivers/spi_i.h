/********************************************************************************************************
 * @file	spi_i.h
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
#include "proj/mcu/config.h"
#include "../config/user_config.h"
#include "../mcu/gpio.h"
#include "../mcu/register.h"
#ifndef WIN32
#if(__TL_LIB_8258__ || MCU_CORE_TYPE == MCU_CORE_8258)
#include "drivers/8258/spi_i.h"
#elif(MCU_CORE_TYPE == MCU_CORE_8278)
#include "drivers/8278/spi_i.h"
#else

// use static inline, because, spi flash code must reside in memory..
// these code may be embedd in flash code

static inline void mspi_wait(void){
	while(reg_master_spi_ctrl & FLD_MASTER_SPI_BUSY)
		;
}

static inline void mspi_high(void){
	reg_master_spi_ctrl = FLD_MASTER_SPI_CS;
}

static inline void mspi_low(void){
	reg_master_spi_ctrl = 0;
}

static inline u8 mspi_get(void){
	return reg_master_spi_data;
}

static inline void mspi_write(u8 c){
	reg_master_spi_data = c;
}

static inline void mspi_ctrl_write(u8 c){
	reg_master_spi_ctrl = c;
}

static inline u8 mspi_read(void){
	mspi_write(0);		// dummy, issue clock 
	mspi_wait();
	return mspi_get();
}
#endif
#endif
