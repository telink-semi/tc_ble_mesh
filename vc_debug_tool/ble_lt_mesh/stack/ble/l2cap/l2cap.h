/********************************************************************************************************
 * @file	l2cap.h
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
#pragma  once





// define the l2cap CID for BLE
#define L2CAP_CID_NULL                   0x0000
#define L2CAP_CID_ATTR_PROTOCOL          0x0004
#define L2CAP_CID_SIG_CHANNEL            0x0005
#define L2CAP_CID_SMP                    0x0006
#define L2CAP_CID_GENERIC                0x0007
#define L2CAP_CID_DYNAMIC                0x0040

#define L2CAP_CID_DYNAMIC                0x0040

#define L2CAP_HEADER_LENGTH              0x0004
#define L2CAP_MTU_SIZE                   23

#define L2CAP_CMD_REJECT                 0x01
#define L2CAP_CMD_DISC_CONN_REQ          0x06
#define L2CAP_CMD_DISC_CONN_RESP         0x07
#define L2CAP_CMD_CONN_UPD_PARA_REQ      0x12
#define L2CAP_CMD_CONN_UPD_PARA_RESP     0x13
#define L2CAP_CMD_CONN_REQ               0x14
#define L2CAP_CMD_CONN_RESP              0x15
#define L2CAP_CMD_FLOW_CTRL_CRED         0x16


#define L2CAP_SIGNAL_MSG_TYPE            1
#define L2CAP_DATA_MSG_TYPE              2
#define L2CAP_REJECT_CMD_NOT_UNDERSTOOD  0
#define L2CAP_REJECT_SIGNAL_MTU_EXCEED   1
#define L2CAP_REJECT_INVALID_CID         2

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


typedef struct
{
	u8 *rx_p;
	u8 *tx_p;
	u16 max_rx_size;

	u16 max_tx_size;
//	u16 init_MTU;
}l2cap_buff_t;

extern l2cap_buff_t l2cap_buff;


extern _attribute_aligned_(4) para_up_req_t	para_upReq;




typedef int (*l2cap_conn_update_rsp_callback_t) (u8 id, u16 result);




typedef enum{
	CONN_PARAM_UPDATE_ACCEPT = 0x0000,
	CONN_PARAM_UPDATE_REJECT = 0x0001,
}conn_para_up_rsp;


#if 1//(L2CAP_CREDIT_BASED_FLOW_CONTROL_MODE_EN)
/**
 *  @brief  Definition for L2CAP signal packet formats
 */
typedef enum{
	L2CAP_COMMAND_REJECT_RSP           		= 0x01,
	L2CAP_CONNECTION_REQ                 	= 0x02,
	L2CAP_CONNECTION_RSP                 	= 0x03,
	L2CAP_CONFIGURATION_REQ                	= 0x04,
	L2CAP_CONFIGURATION_RSP           		= 0x05,
	L2CAP_DISCONNECTION_REQ           		= 0x06,
	L2CAP_DISCONNECTION_RSP           		= 0x07,
	L2CAP_ECHO_REQ          		 		= 0x08,
	L2CAP_ECHO_RSP           				= 0x09,
	L2CAP_INFORMATION_REQ           		= 0x0A,
	L2CAP_INFORMATION_RSP           		= 0x0B,
	L2CAP_CREATE_CHANNEL_REQ          		= 0x0C,
	L2CAP_CREATE_CHANNEL_RSP           		= 0x0D,
	L2CAP_MOVE_CHANNEL_REQ           		= 0x0E,
	L2CAP_MOVE_CHANNEL_RSP           		= 0x0F,
	L2CAP_MOVE_CHANNEL_CONFIRMATION_REQ		= 0x10,
	L2CAP_MOVE_CHANNEL_CONFIRMATION_RSP     = 0x11,
	L2CAP_CONNECTION_PARAMETER_UPDATE_REQ	= 0x12, // = L2CAP_CMD_CONN_UPD_PARA_REQ
	L2CAP_CONNECTION_PARAMETER_UPDATE_RSP	= 0x13, // = L2CAP_CMD_CONN_UPD_PARA_RESP
	L2CAP_LE_CREDIT_BASED_CONNECTION_REQ 	= 0x14,
	L2CAP_LE_CREDIT_BASED_CONNECTION_RSP 	= 0x15,
	L2CAP_FLOW_CONTROL_CREDIT_IND 			= 0x16,
	L2CAP_CREDIT_BASED_CONNECTION_REQ 		= 0x17,	//core_5.2
	L2CAP_CREDIT_BASED_CONNECTION_RSP 		= 0x18,	//core_5.2
	L2CAP_CREDIT_BASED_RECONFIGURE_REQ 		= 0x19,	//core_5.2
	L2CAP_CREDIT_BASED_RECONFIGURE_RSP 		= 0x1A,	//core_5.2
}l2cap_sig_pkt_format;


typedef struct {
	unsigned char opcode;
	unsigned char data[0];
} attr_pkt_t;

typedef struct {
	u8 code;
	u8 identifier;
	u16 dataLen;
	u8 data[0];
} signal_pkt_t;

typedef struct {
	u8 code;
	u8 data[0];
} smp_pkt_t;

typedef struct {
	u16 sduLen;
	u8 info[0];
} coc_start_pkt_t;

typedef struct {
	u8 info[0];
} coc_cont_pkt_t;

typedef struct {
	union{
		coc_start_pkt_t start;
		coc_cont_pkt_t cont;
	};
} coc_pkt_t;

typedef struct{
	u16 pduLen;
	u16 cid;
	union{
		attr_pkt_t att;
		signal_pkt_t signal;
		smp_pkt_t smp;
		coc_pkt_t coc;
	} payload;
} l2cap_pkt_t;

void blt_l2cap_signalDataControl(u16 connHandle, rf_packet_l2cap_t *ptrSig);
int hci_slave_handle_valid(u16 handle);
u16 blt_l2cap_getAclRxBufferSize(void);
#endif


/******************************* User Interface  ************************************/
void		bls_l2cap_requestConnParamUpdate (u16 min_interval, u16 max_interval, u16 latency, u16 timeout);  //Slave

void        bls_l2cap_setMinimalUpdateReqSendingTime_after_connCreate(int time_ms);

void		blc_l2cap_register_handler (void *p);
int 		blc_l2cap_packet_receive (u16 connHandle, u8 * p);
int 		blc_l2cap_send_data (u16 cid, u8 *p, int n);

void 		blc_l2cap_reg_att_sig_hander(void *p);//signaling pkt proc



void  		blc_l2cap_SendConnParamUpdateResponse(u16 connHandle, u8 req_id, conn_para_up_rsp result);
void 		blc_l2cap_registerConnUpdateRspCb(l2cap_conn_update_rsp_callback_t cb);
void blc_l2cap_initMtuBuffer(u8 *pMTU_rx_buff, u16 mtu_rx_size, u8 *pMTU_tx_buff, u16 mtu_tx_size);




/************************* Stack Interface, user can not use!!! ***************************/
ble_sts_t 	blt_update_parameter_request (void);
ble_sts_t   blc_l2cap_pushData_2_controller (u16 connHandle, u16 cid, u8 *format, int format_len, u8 *pDate, int data_len);

//Master
