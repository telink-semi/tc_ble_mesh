/********************************************************************************************************
 * @file	trace.h
 *
 * @brief	for TLSR chips
 *
 * @author	telink
 * @date	Sep. 30, 2010
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


#define TR_T_abuf_add           0
#define TR_T_abuf_dec           1
#define TR_T_abuf_usb           2
#define TR_T_abuf_zero          3
#define TR_T_abuf_overflow      4
#define TR_T_abuf_overflow_mic  5
#define TR_T_abuf_overflow_dec  6

#define TR_T_state_err		    7
#define TR_T_predict_none    	8
#define TR_T_master_rx			9
#define TR_T_master_sys		    10
#define TR_T_abuf_voice         11
#define TR_T_abuf_clear         12
#define TR_T_miss_voice_pkt		13
#define TR_T_miss_usb_iso		14
#define TR_T_abuf_nothing		15
#define TR_T_temp_buf_full		16
#define TR_T_trig_start_tmt		17
#define TR_T_trig_stop_tmt		18
#define TR_T_predict_first     	19
#define TR_T_predict_update     20
#define TR_T_uapi_in	     	21
#define TR_T_att_mic		    22


#define TR_24_abuf_mic_wptr          0
#define TR_24_abuf_dec_wptr          1
#define TR_24_abuf_dec_rptr          2
#define TR_24_abuf_reset             3
#define TR_24_abuf_rf_header         4
#define TR_24_master_state	         5

#endif
