/********************************************************************************************************
 * @file	battery.h
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

/*******ADC define******/
#define Vref_1V3	0x00
#define Vref_AVDD	0x01

#define ANA_VDD		0x0c

#define ANA0_IO		0x01
#define ANA1_IO		0x02
#define ANA2_IO		0x03
#define ANA3_IO		0x04

#define Rsl_9		0x08
#define Rsl_10		0x10
#define Rsl_11		0x18
#define Rsl_12		0x20
#define Rsl_13		0x28
#define Rsl_14		0x30	//0x38 is still Rsl_14

#define BATTERY_INPUT 	0
#define VDD_INPUT 		1

//#define LOW_POWER_LED			    gpio_func_set(SWM,GPIO);gpio_oe_set(SWM)

//#define LED_OFF		gpio_out_high(SWM)
//#define	LED_ON		gpio_out_low(SWM)

//#define  POWER_VOLTAGE_3P3          WriteAnalogReg(0x09, ((ReadAnalogReg(0x09)) & (~0x07)) | 0x00);
#define  POWER_VOLTAGE_3P3          WriteAnalogReg(0x09, ((ReadAnalogReg(0x09)) & (~0x07)) | 0x01);
#define  POWER_VOLTAGE_3P0			WriteAnalogReg(0x09, ((ReadAnalogReg(0x09)) & (~0x07)) | 0x01);
#define  POWER_VOLTAGE_2P8          WriteAnalogReg(0x09, ((ReadAnalogReg(0x09)) & (~0x07)) | 0x02);
#define	 POWER_VOLTAGE_2P6			WriteAnalogReg(0x09, ((ReadAnalogReg(0x09)) & (~0x07)) | 0x02);
#define  POWER_VOLTAGE_2P4          WriteAnalogReg(0x09, ((ReadAnalogReg(0x09)) & (~0x07)) | 0x03);
#define  POWER_VOLTAGE_2P2 			WriteAnalogReg(0x09, ((ReadAnalogReg(0x09)) & (~0x07)) | 0x04);
#define  POWER_VOLTAGE_2P0 			WriteAnalogReg(0x09, ((ReadAnalogReg(0x09)) & (~0x07)) | 0x07);

enum{
	BATT_STAT_NORMAL,
	BATT_STAT_CHARGING,
	BATT_STAT_FULL,
	BATT_STAT_LOW,
	BATT_STAT_NO_PWR,
};

typedef struct{
	u8 batt_volt_stat;
	u8 batt_last_volt_stat;
}batt_stat_t;

extern batt_stat_t batt_stat;

int batt_check(void);
void batt_init(void);

