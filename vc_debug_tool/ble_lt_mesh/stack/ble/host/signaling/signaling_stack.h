/********************************************************************************************************
 * @file	signaling_stack.h
 *
 * @brief	This is the header file for BLE SDK
 *
 * @author	BLE GROUP
 * @date	06,2022
 *
 * @par		Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd.
 *			All rights reserved.
 *
 *          The information contained herein is confidential property of Telink
 *          Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *          of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *          Co., Ltd. and the licensee or the terms described here-in. This heading
 *          MUST NOT be removed from this file.
 *
 *          Licensee shall not delete, modify or alter (or permit any third party to delete, modify, or
 *          alter) any information contained herein in whole or in part except as expressly authorized
 *          by Telink semiconductor (shanghai) Co., Ltd. Otherwise, licensee shall be solely responsible
 *          for any claim to the extent arising out of or relating to such deletion(s), modification(s)
 *          or alteration(s).
 *
 *          Licensees are granted free, non-transferable use of the information in this
 *          file under Mutual Non-Disclosure Agreement. NO WARRANTY of ANY KIND is provided.
 *
 *******************************************************************************************************/

#pragma once

#include "signaling.h"

#define	BLE_STACK_USED_TX_FIFIO_NUM					2


#define L2CAP_COC_SPSM_EATT							0x0027
#define L2CAP_COC_SPSM_OTS							0x0025
#define L2CAP_COC_SPSM_DYNAMIC_MIN					0x0080
#define L2CAP_COC_SPSM_DYNAMIC_MAX					0x00FF

#define L2CAP_CHANNEL_MAX_COUNT						64
#define L2CAP_COC_CID_START							0x0040
#define L2CAP_COC_CID_END							0x007F


#define SIGNAL_CHECK_DYNAMIC_SPSM(spsm)				(spsm >= L2CAP_COC_SPSM_DYNAMIC_MIN && spsm <= L2CAP_COC_SPSM_DYNAMIC_MAX)
#define SIGNAL_CHECK_SIG_ASSIGNED_SPSM(spsm)		(spsm == L2CAP_COC_SPSM_EATT || spsm == L2CAP_COC_SPSM_OTS)
#define SIGNAL_CHECK_DYNAMIC_CID(cid)				(cid >= L2CAP_COC_CID_START && cid <= L2CAP_COC_CID_END)

#define SIGNAL_DEFAULT_INITIAL_CREDITS				10
#define SIGNAL_CREADIT_CID_MAX_NUM					5

//////////////////signaling connect Callback information////////////////////////

typedef struct {
	u16 connHandle;
	signal_pkt_t *signalTxBuff;
}signalConCb_t;

/** Dispatch table for incoming Signal Command.  Sorted by code. */
typedef struct {
	u8 code;
	u16 expect_len;
	u16 (*signal_rx_fn) (signalConCb_t *pSignalConCb, signal_pkt_t* signal, u16 signalLen);
}signal_rx_proc_t;

#define SIGNAL_PROC_FUN(code)				blt_signal_proc_##code
#define SIGNAL_RX_STR(code, code2)			{L2CAP_##code, sizeof(blt_signal_##code2##_t), SIGNAL_PROC_FUN(code2)}

typedef struct {
	u8 code;
	u8 identifier;
	u16 dataLength;
	u8 data[0];
} blt_signal_commandFormat_t;

typedef struct {
	u16 srcCID;
	u16 dstCID;
}l2cap_cmdRejectCidData_t;		//SIG_INVALID_CID_REQUEST

typedef struct {
	u8 code;		// 0x01 L2CAP_COMMAND_REJECT_RSP
	u8 identifier;
	u16 dataLength;
	u16 reason;
	u8 reasonData[0];
} blt_signal_cmdRejectRsp_t;

typedef struct {
	u8 code;		// 0x06 L2CAP_DISCONNECTION_REQ
	u8 identifier;
	u16 dataLength;
	u16 dstCID;
	u16 srcCID;
} blt_signal_disconnReq_t;

typedef struct {
	u8 code;		// 0x07 L2CAP_DISCONNECTION_RSP
	u8 identifier;
	u16 dataLength;
	u16 dstCID;
	u16 srcCID;
} blt_signal_disconnRsp_t;

typedef struct {
	u8 code;		// 0x12 L2CAP_CONN_PARAM_UPDATE_REQ
	u8 identifier;
	u16 dataLength;
	u16 intervalMin;
	u16 intervalMax;
	u16 latency;
	u16 timeout;
} blt_signal_connParamUpdateReq_t;

typedef struct {
	u8 code;		// 0x13 L2CAP_CONN_PARAM_UPDATE_RSP
	u8 identifier;
	u16 dataLength;
	u16 result;
} blt_signal_connParamUpdateRsp_t;

typedef struct {
	u8 code;		// 0x14 L2CAP_LE_CREDIT_BASED_CONNECTION_REQ
	u8 identifier;
	u16 dataLength;
	u16 SPSM;
	u16 srcCID;
	u16 MTU;
	u16 MPS;
	u16 initialCredits;
} blt_signal_leCreditBasedConnReq_t;

typedef struct {
	u8 code;		// 0x15 L2CAP_LE_CREDIT_BASED_CONNECTION_RSP
	u8 identifier;
	u16 dataLength;
	u16 dstCID;
	u16 MTU;
	u16 MPS;
	u16 initialCredits;
	u16 result;
} blt_signal_leCreditBasedConnRsp_t;

typedef struct {
	u8 code;		// 0x16 L2CAP_FLOW_CONTROL_CREDIT_IND
	u8 identifier;
	u16 dataLength;
	u16 CID;
	u16 credits;
} blt_signal_flowCtrlCreditInd_t;

typedef struct {
	u8 code;		// 0x17 L2CAP_CREDIT_BASED_CONNECTION_REQ
	u8 identifier;
	u16 dataLength;
	u16 SPSM;
	u16 MTU;
	u16 MPS;
	u16 initialCredits;
	u16 srcCID[1];	//1-5
} blt_signal_creditBasedConnReq_t;

typedef struct {
	u8 code;		// 0x18 L2CAP_CREDIT_BASED_CONNECTION_RSP
	u8 identifier;
	u16 dataLength;
	u16 MTU;
	u16 MPS;
	u16 initialCredits;
	u16 result;
	u16 dstCID[1];	//1-5
} blt_signal_creditBasedConnRsp_t;

typedef struct {
	u8 code;		// 0x19 L2CAP_CREDIT_BASED_RECONFIGURE_REQ
	u8 identifier;
	u16 dataLength;
	u16 MTU;
	u16 MPS;
	u16 dstCID[1];	//1-5
} blt_signal_creditBasedRecfgReq_t;

typedef struct {
	u8 code;		// 0x19 L2CAP_CREDIT_BASED_RECONFIGURE_RSP
	u8 identifier;
	u16 dataLength;
	u16 result;
} blt_signal_creditBasedRecfgRsp_t;


///////////////Signaling package payload///////////////////

typedef struct {
	u16 dstCID;
	u16 srcCID;
} signal_pkt_disconnReq_t, signal_pkt_disconnRsp_t;

typedef struct {
	u16 intervalMin;
	u16 intervalMax;
	u16 latency;
	u16 timeout;
} signal_pkt_connParamUpdateReq_t;

typedef struct {
	u16 result;
} signal_pkt_connParamUpdateRsp_t;

typedef struct {
	u16 SPSM;
	u16 srcCID;
	u16 MTU;
	u16 MPS;
	u16 initialCredits;
} signal_pkt_leCreditBasedConnReq_t;

typedef struct {
	u16 dstCID;
	u16 MTU;
	u16 MPS;
	u16 initialCredits;
	u16 result;
}signal_pkt_leCreditBasedConnRsp_t;

typedef struct {
	u16 CID;
	u16 credits;
} signal_pkt_flowCtrlCreditInd_t;

typedef struct {
	u16 SPSM;
	u16 MTU;
	u16 MPS;
	u16 initialCredits;
	u16 srcCID[SIGNAL_CREADIT_CID_MAX_NUM];	//1-5
} signal_pkt_creditBasedConnReq_t;

typedef struct {
	u16 MTU;
	u16 MPS;
	u16 initialCredits;
	u16 result;
	u16 dstCID[SIGNAL_CREADIT_CID_MAX_NUM];	//1-5
} signal_pkt_creditBasedConnRsp_t;

typedef struct {
	u16 MTU;
	u16 MPS;
	u16 dstCID[SIGNAL_CREADIT_CID_MAX_NUM];	//1-5
} signal_pkt_creditBasedCfgReq_t;

typedef struct {
	u16 result;
} signal_pkt_creditBasedCfgRsp_t;



////////////////signaling Connection-oriented channel(CoC) control structural////////////////////////

typedef struct{
	u32 modelBusy;

	u16 MTU;
	u16 MPS;
	u16 SPSM;
	u16 cocCidCnt;
#if L2CAP_SERVER_FEATURE_SUPPORTED_EATT
	u16 eattCidCnt;
#endif
	u16 createConnCnt;

	l2cap_coc_acl_t* pCreateConn;
	l2cap_coc_cid_t* pCocCid;
#if L2CAP_SERVER_FEATURE_SUPPORTED_EATT
	l2cap_coc_cid_t* pEattCid;
#endif

}l2cap_coc_control_t;


typedef void (*coc_data_handler_t)(u16 connHandle, l2cap_pkt_t* pkt);
typedef void (*coc_disconnect_handler_t)(u16 connHandle);
typedef void (*coc_signal_handler_t)(u16 connHandle, rf_packet_l2cap_t* pkt);

extern coc_data_handler_t		coc_data_handler;
extern coc_disconnect_handler_t coc_disconnect_handler;
extern coc_signal_handler_t		coc_signal_handler;
extern signal_rx_proc_t* signal_rx_handlers;
extern const signal_rx_proc_t signal_CoC_handlers[];

//////////////////signaling create new package to tx buffer//////////////////////
u16 blt_signal_packageCmdRejectRspUnderstood(signal_pkt_t* txBuf, u8 identifier);
u16 blt_signal_packageCmdRejectRspInvalidCid(signal_pkt_t* txBuf, u8 identifier, l2cap_cmdRejectCidData_t* invalidCID);
u16 blt_signal_packageDisconnReq(signal_pkt_t* txBuf, u8 identifier, signal_pkt_disconnReq_t* req);
u16 blt_signal_packageDisconnRsp(signal_pkt_t* txBuf, u8 identifier, signal_pkt_disconnRsp_t* rsp);
u16 blt_signal_packageConnParamUpdateReq(signal_pkt_t* txBuf, u8 identifier, signal_pkt_connParamUpdateReq_t* req);
u16 blt_signal_packageConnParamUpdateRsp(signal_pkt_t* txBuf, u8 identifier, signal_pkt_connParamUpdateRsp_t* rsp);
u16 blt_signal_packageLeCreditBasedConnReq(signal_pkt_t* txBuf, u8 identifier, signal_pkt_leCreditBasedConnReq_t* req);
u16 blt_signal_packageLeCreditBasedConnRsp(signal_pkt_t* txBuf, u8 identifier, signal_pkt_leCreditBasedConnRsp_t* rsp);
u16 blt_signal_packageFlowCtrlCreditInd(signal_pkt_t* txBuf, u8 identifier, signal_pkt_flowCtrlCreditInd_t* ind);
u16 blt_signal_packageCreditBasedConnReq(signal_pkt_t* txBuf, u8 identifier, u8 srcCIDNum, signal_pkt_creditBasedConnReq_t* req);
u16 blt_signal_packageCreditBasedConnRsp(signal_pkt_t* txBuf, u8 identifier, u8 dstCIDNum, signal_pkt_creditBasedConnRsp_t* rsp);
u16 blt_signal_packageCreditBasedRecfgReq(signal_pkt_t* txBuf, u8 identifier, u8 dstCIDNum, signal_pkt_creditBasedCfgReq_t* req);
u16 blt_signal_packageCreditBasedRecfgRsp(signal_pkt_t* txBuf, u8 identifier, u16 result);

//////////////////push LE signaling package to link layer/////////////
ble_sts_t blt_signal_sendDisconnReq(l2cap_coc_cid_t* pCid);
ble_sts_t blt_signal_sendLeCreditBasedConnReq(u16 connHandle, u16 SPSM, u16 srcCID);
ble_sts_t blt_signal_sendFlowCtrlCreditInd(u16 connHandle, signal_pkt_flowCtrlCreditInd_t* ind);
ble_sts_t blt_signal_sendCreditBasedConnReq(u16 connHandle, u16 SPSM, u16* srcCID, u8 srcCIDCnt);
ble_sts_t blt_signal_sendCreditReconfigureReq(u16 connHandle, u16 MTU, u16 MPS, u16* dstCID, u8 dstCIDCnt);


//////////////////signaling process package////////////////////
u16 blt_signal_proc_cmdRejectRsp(signalConCb_t *pSignalConCb, signal_pkt_t* signal, u16 signalLen);
u16 blt_signal_proc_disconnReq(signalConCb_t *pSignalConCb, signal_pkt_t* signal, u16 signalLen);
u16 blt_signal_proc_disconnRsp(signalConCb_t *pSignalConCb, signal_pkt_t* signal, u16 signalLen);
u16 blt_signal_proc_connParamUpdateReq(signalConCb_t *pSignalConCb, signal_pkt_t* signal, u16 signalLen);
u16 blt_signal_proc_connParamUpdateRsp(signalConCb_t *pSignalConCb, signal_pkt_t* signal, u16 signalLen);
u16 blt_signal_proc_leCreditBasedConnReq(signalConCb_t *pSignalConCb, signal_pkt_t* signal, u16 signalLen);
u16 blt_signal_proc_leCreditBasedConnRsp(signalConCb_t *pSignalConCb, signal_pkt_t* signal, u16 signalLen);
u16 blt_signal_proc_flowCtrlCreditInd(signalConCb_t *pSignalConCb, signal_pkt_t* signal, u16 signalLen);
u16 blt_signal_proc_creditBasedConnReq(signalConCb_t *pSignalConCb, signal_pkt_t* signal, u16 signalLen);
u16 blt_signal_proc_creditBasedConnRsp(signalConCb_t *pSignalConCb, signal_pkt_t* signal, u16 signalLen);
u16 blt_signal_proc_creditBasedRecfgReq(signalConCb_t *pSignalConCb, signal_pkt_t* signal, u16 signalLen);
u16 blt_signal_proc_creditBasedRecfgRsp(signalConCb_t *pSignalConCb, signal_pkt_t* signal, u16 signalLen);

//////////////////get CoC parameter//////////////
u16 blt_l2cap_cocGetRecvSpsm(void);
u16 blt_l2cap_cocGetRecvMtu(void);
u16 blt_l2cap_cocGetRecvMps(void);

//this function to check SPSM, have available
bool blt_l2cap_cocCheckNotAvailableResources(u16 SPSM);
//this function to create new CID, by SPSM and source CID number.
l2cap_coc_cid_t* blt_l2cap_cocCreateNewCID(u16 connHandle, u16 SPSM, u16 srcCID);
//
l2cap_coc_cid_t* blt_l2cap_cocGetDstCID(u16 connHandle, u16 dstCID);
l2cap_coc_cid_t* blt_l2cap_cocGetSrcCID(u16 connHandle, u16 srcCID);

void blt_l2cap_cocDisconnect(u16 connHandle);

bool blt_l2cap_cocCheckSrcCID(u16 connHandle, u16 srcCID);
l2cap_coc_acl_t* blt_l2cap_getCreateCoc(u16 connHandle);
u8 blt_signal_getIdentifier(u16 connHandle);
void blt_l2cap_cocDataControl(u16 connHandle, l2cap_pkt_t* pkt);

////////////////l2cap CoC event////////////
void blt_l2cap_reportCocConnectEvent(l2cap_coc_cid_t* pCid);
void blt_l2cap_reportCocDisconnectEvent(l2cap_coc_cid_t* pCid);
void blt_l2cap_reportCocReconnfigureEvent(l2cap_coc_cid_t* pCid);
void blt_l2cap_reportCocReceiveDataEvent(l2cap_coc_cid_t* pCid);
void blt_l2cap_reportCocSendDataFinishEvent(l2cap_coc_cid_t* pCid);
void blt_l2cap_reportCocCreateConnectFinishEvent(u16 connHandle, u8 code, u16 result);

//////////////EATT///////////////////
int blt_eatt_l2capEattRxHandle(l2cap_coc_cid_t* pCid);
