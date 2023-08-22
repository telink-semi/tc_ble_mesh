/********************************************************************************************************
 * @file	ll_init.h
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
/*
 * ll_init.h
 *
 *  Created on: 2017-3-9
 *      Author: Administrator
 */

#ifndef LL_INIT_H_
#define LL_INIT_H_



typedef struct {
	u8		init_en;
	u8		conn_policy;
	u8		conn_advType;
	u8		conn_established;

	u32		create_conn_startTick;

	u16     rsvd;
	u8		conn_mac[6];

}st_ll_init_t;

//st_ll_init_t  blti;

extern u32 blm_timeout_connectDevice;



extern rf_packet_ll_init_t	pkt_init;




typedef int  (*ll_module_init_callback_t)(u8 *, u32);





static inline u32 rf_get_ble_access_code_16to32 (u16 code)
{
	u32 r = 0;
	int i;
	for (i=0; i<4; i++) {
		r = r << 2;
		r |= code & BIT(i) ? 1 : 2;
	}

	for (i=4; i<8; i++) {  //no more 24 transitions
		r = r << 4;
		r |= code & BIT(i) ? 0x0c : 0x03;
	}

	for (i=12; i<16; i++) {
		r = r << 2;
		r |= code & BIT(i) ? 1 : 2;
	}

	return r;
}




/******************************* User Interface  ************************************/
void	 	blc_ll_initInitiating_module(void);


ble_sts_t 	blc_ll_createConnection (u16 scan_interval, u16 scan_window, init_fp_type_t initiator_filter_policy,
							  u8 adr_type, u8 *mac, u8 own_adr_type,
							  u16 conn_min, u16 conn_max, u16 conn_latency, u16 timeout,
							  u16 ce_min, u16 ce_max );

ble_sts_t 	blc_ll_createConnectionCancel (void);


ble_sts_t   blc_ll_setCreateConnectionTimeout (u32 timeout_ms);


/************************* Stack Interface, user can not use!!! ***************************/
ble_sts_t 	blc_ll_setInitEnable (u8 en);


#endif /* LL_INIT_H_ */
