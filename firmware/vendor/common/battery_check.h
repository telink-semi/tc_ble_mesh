/********************************************************************************************************
 * @file	battery_check.h
 *
 * @brief	for TLSR chips
 *
 * @author	public@telink-semi.com;
 * @date	Sep. 18, 2018
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
#ifndef BATTERY_CHECK_H_
#define BATTERY_CHECK_H_

#define MANUAL_MODE_GET_ADC_SAMPLE_RESULT  0
#define VBAT_LEAKAGE_PROTECT_EN				0
#define VBAT_ALRAM_THRES_MV				(2000)   // 2000 mV low battery alarm

#define LOG_BATTERY_CHECK_DEBUG(pbuf,len,format,...)    //LOG_MSG_LIB(TL_LOG_NODE_SDK,pbuf,len,format,__VA_ARGS__)


void battery_set_detect_enable (int en);
int  battery_get_detect_enable (void);

int app_battery_power_check(u16 alram_vol_mv, int loop_flag);
void app_battery_power_check_and_sleep_handle(int loop_flag);
void battery_power_low_handle(int loop_flag);
void app_battery_check_and_re_init_user_adc();


#endif /* APP_BATTDET_H_ */
