/********************************************************************************************************
 * @file	llms_slave.h
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
 * llms_slave.h
 *
 *  Created on: 2019-5-24
 *      Author: Administrator
 */

#ifndef LLMS_SLAVE_H_
#define LLMS_SLAVE_H_



#define			SLAVE_SYNC_CONN_CREATE							BIT(0)
#define			SLAVE_SYNC_CONN_UPDATE							BIT(1)
#define			SLAVE_SYNC_HIGH_DUTY							BIT(7)


#define			SLAVE_SYNC_HIGHDUTY_SUPPORT_EN					0


#define			BRX_EARLY_SET_SYNC								300
#define			BRX_EARLY_SET_COMMON							200



typedef struct {
	u8		slaveSync;    // conn create or conn_param_update
	u8		conn_rcvd_ack_pkt;
	u8		rsvd1;
	u8		rsvd2;


	u32		connExpectTime;
	u32		conn_start_time;
	u32		conn_duration;

	u32		tick_1st_rx;

} st_llms_s_conn_t;

extern st_llms_s_conn_t	bltcs;



/************************************ User Interface  ******************************************************/




/*********************************** Stack Interface, user can not use!!! **********************************/
int 	blms_s_connect (rf_packet_connect_t * pInit);
int 	blms_brx_start(void);
int 	blms_brx_post(void);




#endif /* LLMS_SLAVE_H_ */
