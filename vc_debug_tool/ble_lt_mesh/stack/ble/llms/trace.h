/********************************************************************************************************
 * @file	trace.h
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
#ifndef TRACE_H_
#define TRACE_H_

#define TR_T_irq					1
#define TR_T_irq_rx					2
#define	TR_T_irq_tx					3
#define TR_T_irq_sysTimer			4
#define TR_T_irq_adv				5


#define TR_T_irq_cmddone			6
#define TR_T_irq_rxTmt				7
#define TR_T_irq_rxFirstTmt			8
#define TR_T_irq_fsmTmt				9



#define TR_T_ll_brx_start	        10
#define TR_T_ll_brx_post	        11

#define TR_T_ll_btx_start	        12
#define TR_T_ll_btx_post            13
#define	TR_T_ll_1stRx				14

#define TR_T_ll_terminate			15

#define	TR_T_rx_crc					18
#define	TR_T_rx_conn				20


#define	TR_T_slot_update			20
#define	TR_T_slot_runout			22
#define TR_T_slot_rebuild			23
#define TR_T_slot_locate			24
#define TR_T_slot_task				25

#define TR_T_slot_jump				26

#define	TR_T_slave_sync				30
#define	TR_T_slave_sync_done		31


#define TR_24_ll_chn            	0
#define TR_24_ll_duration			1


#endif
