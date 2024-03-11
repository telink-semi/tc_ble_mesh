/********************************************************************************************************
 * @file	l2cap.h
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
#pragma  once



// Response Timeout expired
#define L2CAP_RTX_TIMEOUT_MS             2000

#define NEXT_SIG_ID()                    ( ++l2capId == 0 ? l2capId = 1 : l2capId )


#define L2CAP_PKT_HANDLER_SIZE           6
 
 
// l2cap handler type
#define L2CAP_CMD_PKT_HANDLER            0
#define L2CAP_USER_CB_HANDLER            1





#define L2CAP_CONNECTION_PARAMETER_ACCEPTED        0x0000
#define L2CAP_CONNECTION_PARAMETER_REJECTED        0x0001



//RF_LEN_MAX:255: MIC(4)+LL_MAX_LEN(251) => LL_MAX_LEN:l2cap_len(2)+cid(2)+ATT_MTU_MAX(247).
//l2cap buffer max: dma(4)+header(2)+l2cap_len(2)+cid(2)+ATT_MTU_MAX(247).
#define	L2CAP_RX_BUFF_LEN_MAX			  260//257 = 10+247,align 4, here we use 260
//dma(4)+header(2)+l2cap_len(2)+cid(2)+Attribute_data[ATT_MTU]
#define	ATT_RX_MTU_SIZE_MAX		  		  (L2CAP_RX_BUFF_LEN_MAX - 10)

#define TELINK_MTU_SIZE            		  ATT_RX_MTU_SIZE_MAX

#define L2CAP_RX_PDU_OFFSET				  12



typedef struct{
	u16 	connParaUpReq_minInterval;
	u16 	connParaUpReq_maxInterval;
	u16 	connParaUpReq_latency;
	u16 	connParaUpReq_timeout;

	u8		connParaUpReq_pending;
	u8      rsvd[3];
}para_up_req_t;

extern _attribute_aligned_(4) para_up_req_t	para_upReq;




typedef int (*l2cap_conn_update_rsp_callback_t) (u8 id, u16 result);




typedef enum{
	CONN_PARAM_UPDATE_ACCEPT = 0x0000,
	CONN_PARAM_UPDATE_REJECT = 0x0001,
}conn_para_up_rsp;




/******************************* User Interface  ************************************/
void		bls_l2cap_requestConnParamUpdate (u16 min_interval, u16 max_interval, u16 latency, u16 timeout);  //Slave

void        bls_l2cap_setMinimalUpdateReqSendingTime_after_connCreate(int time_ms);

void		blc_l2cap_register_handler (void *p);
int 		blc_l2cap_packet_receive (u16 connHandle, u8 * p);
int 		blc_l2cap_send_data (u16 cid, u8 *p, int n);

void 		blc_l2cap_reg_att_sig_handler(void *p);//signaling pkt proc

void 		blc_l2cap_reg_att_cli_handler(void *p);


void  		blc_l2cap_SendConnParamUpdateResponse(u16 connHandle, u8 req_id, conn_para_up_rsp result);
void 		blc_l2cap_registerConnUpdateRspCb(l2cap_conn_update_rsp_callback_t cb);




/************************* Stack Interface, user can not use!!! ***************************/
void 		blt_update_parameter_request (void);
ble_sts_t   blc_l2cap_pushData_2_controller (u16 connHandle, u16 cid, u8 *format, int format_len, u8 *pDate, int data_len);

//Master
