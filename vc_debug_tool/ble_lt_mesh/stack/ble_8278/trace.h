/********************************************************************************************************
 * @file	trace.h
 *
 * @brief	for TLSR chips
 *
 * @author	public@telink-semi.com;
 * @date	Sep. 18, 2015
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

#define TR_T_irq					0
#define TR_T_irq_rx					1
#define	TR_T_irq_tx					2
#define TR_T_irq_sysTimer			3

#define TR_T_irq_cmddone			4
#define TR_T_irq_rxTmt				5
#define TR_T_irq_rxFirstTmt			6
#define TR_T_irq_fsmTmt				7

#define TR_T_irq_rxCrcErr			8

#define TR_T_ll_brx_start	        9
#define TR_T_ll_brx_end	            10

#define TR_T_ll_durationInit		11
#define TR_T_ll_durationUpdate		12

#define TR_T_ll_1stRx				13
#define TR_T_ll_chn_map   	     	14
#define TR_T_ll_chn_timing      	15
#define TR_T_ll_scanRsp            	16
#define TR_T_ll_connReq	            17

#define TR_T_ll_sync            	18
#define TR_T_ll_adv	            	19

#define TR_T_ll_stopRf				20


#define TR_T_audioTask		        24
#define TR_T_audioData		        25
#define TR_T_adpcm		        	26
#define TR_T_adpcm_enc_overflow    	27



#define TR_T_master_irq				30
#define TR_T_master_rx				31
#define TR_T_master_tx				32
#define TR_T_master_sysTimer		33
#define TR_T_master_connDone		34

#define TR_T_master_adv_report		35

#define TR_T_master_createConn		40
#define TR_T_master_init			41
#define TR_T_master_connect		    42
#define TR_T_master_terminate		43
#define TR_T_master_btx				44
#define TR_T_master_scan			45

#define TR_T_master_update			50
#define TR_T_master_update_req		51




#define TR_24_ll_chn            	0
#define TR_24_ll_duration			1



#define TR_24_slot_idx				5
#define TR_24_scan_chn				6
#define TR_24_master_state			7


#endif
