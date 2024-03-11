/********************************************************************************************************
 * @file	gap_event.h
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
 * gap_event.h
 *
 *  Created on: 2018-12-5
 *      Author: Administrator
 */

#ifndef GAP_EVENT_H_
#define GAP_EVENT_H_



#define GAP_EVT_SMP_PARING_BEAGIN		                             0
#define GAP_EVT_SMP_PARING_SUCCESS			                         1
#define GAP_EVT_SMP_PARING_FAIL			                             2
#define GAP_EVT_SMP_CONN_ENCRYPTION_DONE							 3
#define GAP_EVT_SMP_TK_DISPLAY			                             4
#define GAP_EVT_SMP_TK_REQUEST_PASSKEY								 5
#define GAP_EVT_SMP_TK_REQUEST_OOB									 6
#define GAP_EVT_SMP_TK_NUMERIC_COMPARE								 7


#define GAP_EVT_ATT_EXCHANGE_MTU									 16
#define GAP_EVT_GATT_HANDLE_VALUE_CONFIRM							 17

#if 1 // L2CAP_CREDIT_BASED_FLOW_CONTROL_MODE_EN
#define GAP_EVT_L2CAP_COC_CONNECT									 22
#define GAP_EVT_L2CAP_COC_DISCONNECT								 23
#define GAP_EVT_L2CAP_COC_RECONFIGURE								 24
#define GAP_EVT_L2CAP_COC_RECV_DATA									 25
#define GAP_EVT_L2CAP_COC_SEND_DATA_FINISH							 26
#define GAP_EVT_L2CAP_COC_CREATE_CONNECT_FINISH						 27

#define GAP_EVT_L2CAP_CONN_PARAM_UPDATE_REQ                          28
#define GAP_EVT_L2CAP_CONN_PARAM_UPDATE_RSP                          29
#endif

#define GAP_EVT_MASK_NONE                                        	 0x00000000
#define GAP_EVT_MASK_SMP_PARING_BEAGIN                          	 (1<<GAP_EVT_SMP_PARING_BEAGIN)
#define GAP_EVT_MASK_SMP_PARING_SUCCESS                           	 (1<<GAP_EVT_SMP_PARING_SUCCESS)
#define GAP_EVT_MASK_SMP_PARING_FAIL                           		 (1<<GAP_EVT_SMP_PARING_FAIL)
#define GAP_EVT_MASK_SMP_CONN_ENCRYPTION_DONE                     	 (1<<GAP_EVT_SMP_CONN_ENCRYPTION_DONE)
#define GAP_EVT_MASK_SMP_TK_DISPLAY                  				 (1<<GAP_EVT_SMP_TK_DISPLAY)
#define GAP_EVT_MASK_SMP_TK_REQUEST_PASSKEY                  		 (1<<GAP_EVT_SMP_TK_REQUEST_PASSKEY)
#define GAP_EVT_MASK_SMP_TK_REQUEST_OOB	                     		 (1<<GAP_EVT_SMP_TK_REQUEST_OOB)
#define GAP_EVT_MASK_SMP_TK_NUMERIC_COMPARE                     	 (1<<GAP_EVT_SMP_TK_NUMERIC_COMPARE)

#define GAP_EVT_MASK_ATT_EXCHANGE_MTU                     	 		 (1<<GAP_EVT_ATT_EXCHANGE_MTU)
#define GAP_EVT_MASK_GATT_HANDLE_VALUE_CONFIRM                     	 (1<<GAP_EVT_GATT_HANDLE_VALUE_CONFIRM)


#define GAP_EVT_MASK_DEFAULT										( GAP_EVT_MASK_SMP_TK_DISPLAY 			| \
																	  GAP_EVT_MASK_SMP_TK_REQUEST_PASSKEY   | \
																	  GAP_EVT_MASK_SMP_TK_REQUEST_OOB		| \
																	  GAP_EVT_MASK_ATT_EXCHANGE_MTU )



#if 1 // L2CAP_CREDIT_BASED_FLOW_CONTROL_MODE_EN
#define GAP_EVT_MASK_L2CAP_CONN_PARAM_UPDATE_REQ					(1<<GAP_EVT_L2CAP_CONN_PARAM_UPDATE_REQ)
#define GAP_EVT_MASK_L2CAP_CONN_PARAM_UPDATE_RSP					(1<<GAP_EVT_L2CAP_CONN_PARAM_UPDATE_RSP)

#define GAP_EVT_MASK_L2CAP_COC_CONNECT								(1<<GAP_EVT_L2CAP_COC_CONNECT)
#define GAP_EVT_MASK_L2CAP_COC_DISCONNECT							(1<<GAP_EVT_L2CAP_COC_DISCONNECT)
#define GAP_EVT_MASK_L2CAP_COC_RECONFIGURE							(1<<GAP_EVT_L2CAP_COC_RECONFIGURE)
#define GAP_EVT_MASK_L2CAP_COC_RECV_DATA				 			(1<<GAP_EVT_L2CAP_COC_RECV_DATA)
#define GAP_EVT_MASK_L2CAP_COC_SEND_DATA_FINISH						(1<<GAP_EVT_L2CAP_COC_SEND_DATA_FINISH)
#define GAP_EVT_MASK_L2CAP_COC_CREATE_CONNECT_FINISH				(1<<GAP_EVT_L2CAP_COC_CREATE_CONNECT_FINISH)
#endif

typedef struct {
	u16	connHandle;
	u8	secure_conn;
	u8	tk_method;
} gap_smp_paringBeginEvt_t;


typedef struct {
	u16	connHandle;
	u8	bonding;
	u8	bonding_result;
} gap_smp_paringSuccessEvt_t;

typedef struct {
	u16	connHandle;
	u8  reason;
} gap_smp_paringFailEvt_t;

typedef struct {
	u16	connHandle;
	u8	re_connect;   //1: re_connect, encrypt with previous distributed LTK;   0: paring , encrypt with STK
} gap_smp_connEncDoneEvt_t;





typedef struct {
	u16	connHandle;
	u32	tk_pincode;
} gap_smp_TkDisplayEvt_t;

typedef struct {
	u16	connHandle;
} gap_smp_TkReqPassKeyEvt_t;

typedef struct {
	u16	connHandle;
	u16	peer_MTU;
	u16	effective_MTU;
} gap_gatt_mtuSizeExchangeEvt_t;


#if 1 // L2CAP_CREDIT_BASED_FLOW_CONTROL_MODE_EN
/**
 *  @brief  Event Parameters for "GAP_EVT_L2CAP_CONN_PARAM_UPDATE_REQ"
 */
typedef struct {
	u16	connHandle;
	u8	id;
	u16 min_interval;
	u16 max_interval;
	u16 latency;
	u16 timeout;
} gap_l2cap_connParamUpdateReqEvt_t;

/**
 *  @brief  Event Parameters for "GAP_EVT_L2CAP_CONN_PARAM_UPDATE_RSP"
 */
typedef struct {
	u16	connHandle;
	u8  id;
	u16	result;
} gap_l2cap_connParamUpdateRspEvt_t;


typedef struct{
	u16	connHandle;
	u16 spsm;
	u16 mtu;
	u16 srcCid;
	u16 dstCid;
} gap_l2cap_cocConnectEvt_t;

typedef struct {
	u16	connHandle;
	u16 srcCid;
	u16 dstCid;
} gap_l2cap_cocDisconnectEvt_t;

typedef struct {
	u16 connHandle;
	u16 srcCid;
	u16 mtu;
} gap_l2cap_cocReconfigureEvt_t;

typedef struct{
	u16	connHandle;
	u16 dstCid;
	u16 length;
	u8* data;
} gap_l2cap_cocRecvDataEvt_t;

typedef struct{
	u16 connHandle;
	u16 srcCid;
} gap_l2cap_cocSendDataFinishEvt_t;

typedef struct{
	u16 connHandle;
	u8 code;
	u16 result;
} gap_l2cap_cocCreateConnectFinishEvt_t;
#endif

extern u32		gap_eventMask;



typedef int (*gap_event_handler_t) (u32 h, u8 *para, int n);


/******************************* User Interface  ******************************************/
void 	blc_gap_setEventMask(u32 evtMask);

void 	blc_gap_registerHostEventHandler (gap_event_handler_t  handler);







/************************* Stack Interface, user can not use!!! ***************************/
int blc_gap_send_event (u32 h, u8 *para, int n);


#endif /* GAP_EVENT_H_ */
