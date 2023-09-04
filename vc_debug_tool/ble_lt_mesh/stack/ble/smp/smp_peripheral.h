/********************************************************************************************************
 * @file	smp_peripheral.h
 *
 * @brief	for TLSR chips
 *
 * @author	BLE Group
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
 * smp_peripheral.h
 *
 *  Created on: 2018-12-4
 *      Author: Administrator
 */

#ifndef SMP_PERIPHERAL_H_
#define SMP_PERIPHERAL_H_






// "SecReq" refer to "security request"
typedef enum {
	SecReq_NOT_SEND = 0,   // do not send "security request" after link layer connection established
	SecReq_IMM_SEND = BIT(0),   //"IMM" refer to immediate, send "security request" immediately after link layer connection established
	SecReq_PEND_SEND = BIT(1),  //"PEND" refer to pending,  pending "security request" for some time after link layer connection established, when pending time arrived. send it
}secReq_cfg;



typedef struct{
	u8  secReq_conn;    //BIT<7:4> reConn;  BIT<3:0> newConn;
	u8  secReq_pending;
	u16 pending_ms;
}secReq_ctl_t;

extern _attribute_aligned_(4) secReq_ctl_t	blc_SecReq_ctrl;




/******************************* User Interface  *****************************************/
int 		blc_smp_peripheral_init (void);

void 		blc_smp_configSecurityRequestSending( secReq_cfg newConn_cfg,  secReq_cfg re_conn_cfg, u16 pending_ms);
int 		blc_smp_sendSecurityRequest (void);




/************************* Stack Interface, user can not use!!! ***************************/

int 		blc_smp_peripheral_init (void);



u8 *		bls_smp_pushPkt (int type);
int 		bls_smp_encryption_done(u16 connHandle);
void 		bls_smp_peripheral_paring_loop(void);
paring_sts_t	bls_smp_get_paring_statas(u16 connHandle);

void 		blc_smp_procParingEnd(u8 err_reason);

u8 * 		l2cap_smp_handler(u16 connHandle, u8 * p);




#endif /* SMP_PERIPHERAL_H_ */
