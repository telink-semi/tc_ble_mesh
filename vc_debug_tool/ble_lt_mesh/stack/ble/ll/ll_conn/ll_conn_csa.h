/********************************************************************************************************
 * @file	ll_conn_csa.h
 *
 * @brief	for TLSR chips
 *
 * @author	BLE Group
 * @date	July. 4, 2019
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
#ifndef LL_CONN_CSA_H_
#define LL_CONN_CSA_H_



/******************************* Macro & Enumeration variables for Stack, user can not use!!!!  ********************************/

//See the Core_v5.0(Vol 6/Part B/4.5.8, "Data Channel Index Selection") for more information.
typedef enum {
	CHANNAL_SELECTION_ALGORITHM_1      	=	0x00,
	CHANNAL_SELECTION_ALGORITHM_2      	=	0x01,
} channel_algorithm_t;



typedef u8 (*ll_chn_index_calc_callback_t)(u8*, u16, u16);
extern ll_chn_index_calc_callback_t	ll_chn_index_calc_cb;






/********************************  Macro & Enumeration variables for User *****************************************************/













/********************************  User Interface  ****************************************************************************/
void blc_ll_initChannelSelectionAlgorithm_2_feature(void);















/*******************************  Stack Interface, user can not use!!! *******************************************************/
 u8 blc_calc_remapping_table(u8 chm[5]);

 u8 blc_ll_channel_index_calc_csa2(u8 chm[5], u16 event_cntr, u16 channel_id);




#endif /* LL_CONN_CSA_H_ */
