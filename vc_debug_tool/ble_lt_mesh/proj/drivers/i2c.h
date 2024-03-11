/********************************************************************************************************
 * @file	i2c.h
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

#include "../common/types.h"

#define I2C_VENDOR_SEND_WAIT_MODE  1
#define I2C_VENDOR_SEND_DIRECT_MODE 2

#if(MCU_CORE_TYPE == MCU_CORE_8258)
#define PIN_I2C_SCL				GPIO_PA4
#define PIN_I2C_SDA				GPIO_PA3
#elif(MCU_CORE_TYPE == MCU_CORE_8278)
#define PIN_I2C_SCL				GPIO_PA4
#define PIN_I2C_SDA				GPIO_PA3
#else
#define PIN_I2C_SCL				GPIO_CK
#define PIN_I2C_SDA				GPIO_DI
#endif

void i2c_sim_init(void);
void i2c_sim_write(u8 id, u8 addr, u8 dat);
u8 i2c_sim_read(u8 id, u8 addr);
void i2c_sim_burst_read(u8 id, u8 addr, u8 *p, int n);
void i2c_sim_burst_write(u8 id, u8 addr,u8 *p,int n);

void i2c_init (void);
int i2c_burst_write(u8 id, u16 adr, u8 * buff, int len);
int i2c_burst_read(u8 id, u16 adr, u8 * buff, int len);
void i2c_write(u8 id, u16 adr, u8 dat);
u8 i2c_read(u8 id, u16 adr);

typedef void (*i2c_callback_func)(u8 *);
void i2c_slave_register(int pin, i2c_callback_func callback);
void i2c_send_response(u8*,int);

void i2c_vendor_slave_init(u32 rd_mem_addr, u32 wt_mem_addr);
void i2c_vendor_send_response(u8 *buf, int len);
void i2c_vendor_slave_register(u32 rd_mem_addr, u32 wt_mem_addr, u8 rd_buf_size, u8 wt_buf_size, int pin, i2c_callback_func callback);
void i2c_vendor_slave_deregister(void);
int i2c_vendor_rx_data_timerCb (void* arg);
void  i2c_vendor_reset_write_buf(void);
void i2c_write_start(u8 id);
void i2c_read_start_buf(u8 id,u8 *p_buff,u8 len);







