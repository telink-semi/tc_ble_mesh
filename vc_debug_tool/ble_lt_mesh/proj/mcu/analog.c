/********************************************************************************************************
 * @file	analog.c
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
#include "compiler.h"
#include "../common/bit.h"
#include "../common/tutility.h"
#include "analog.h"
#include "irq_i.h"
#include "register.h"

#if WIN32
extern u8 rega_simu_buffer[];
#endif

#if (MCU_CORE_TYPE != MCU_CORE_3520)

static inline void analog_wait(){
	while(reg_ana_ctrl & FLD_ANA_BUSY){}
}

_attribute_ram_code_ u8 analog_read(u8 addr){
	u8 r = irq_disable();

	reg_ana_addr = addr;
	reg_ana_ctrl = (FLD_ANA_START);
//   Can't use one line setting "reg_ana_ctrl32 = ((FLD_ANA_START | FLD_ANA_RSV) << 16) | addr;"
//   This will fail because of time sequence and more over size is bigger
	analog_wait();
#if !WIN32
	u8 data = reg_ana_data;
#else
	u8 data = rega_simu_buffer[addr];
#endif	
	reg_ana_ctrl = 0;		// finish

	irq_restore(r);

	return data;
}

_attribute_ram_code_ void analog_write(u8 addr, u8 v){
	u8 r = irq_disable();

	reg_ana_addr = addr;
	reg_ana_data = v;
	reg_ana_ctrl = (FLD_ANA_START | FLD_ANA_RW);
//	 Can't use one line setting "reg_ana_ctrl32 = ((FLD_ANA_START | FLD_ANA_RW) << 16) | (v << 8) | addr;"
//   This will fail because of time sequence and more over size is bigger
	analog_wait();
	reg_ana_ctrl = 0; 		// finish
#if WIN32
	rega_simu_buffer[addr] = v;
#endif
	
	irq_restore(r);
}

void analog_read_multi(u8 addr, u8 *v, int len){
	u8 r = irq_disable();

	reg_ana_ctrl = 0;		// issue clock
	reg_ana_addr = addr;
	while(len--){
		reg_ana_ctrl = FLD_ANA_CYC | FLD_ANA_START;
		analog_wait();
#if !WIN32
		*v++ = reg_ana_data;
#else
		*v++ = rega_simu_buffer[addr++];
#endif
	}
	reg_ana_ctrl = 0; 		// finish

	irq_restore(r);
}

void analog_write_multi(u8 addr, u8 *v, int len){
	u8 r = irq_disable();

	reg_ana_addr = addr;
	while(len--){
		reg_ana_data = *v++;
#if WIN32
		rega_simu_buffer[addr++] = *v++;
#endif
		reg_ana_ctrl = FLD_ANA_CYC | FLD_ANA_START | FLD_ANA_RW; 	// multi write
		analog_wait();
	}
	reg_ana_ctrl = 0; 		// finish

	irq_restore(r);
}

#endif
