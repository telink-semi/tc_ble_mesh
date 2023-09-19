/********************************************************************************************************
 * @file	llms_init.h
 *
 * @brief	This is the header file for BLE SDK
 *
 * @author	BLE SDK
 * @date	2017
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
/*
 * llms_init.h
 *
 *  Created on: 2019-5-25
 *      Author: Administrator
 */

#ifndef LLMS_INIT_H_
#define LLMS_INIT_H_








extern	int blms_create_connection;
extern	u32 blms_timeout_connectDevice;
extern	u32	blms_tick_connectDevice;



/************************************ User Interface  ******************************************************/




/*********************************** Stack Interface, user can not use!!! **********************************/
int  blt_llms_procInitPkt(u8 *raw_pkt);




#endif /* LLMS_INIT_H_ */
