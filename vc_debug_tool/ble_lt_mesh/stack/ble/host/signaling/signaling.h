/********************************************************************************************************
 * @file    signaling.h
 *
 * @brief   This is the header file for BLE SDK
 *
 * @author  BLE GROUP
 * @date    06,2022
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

#pragma once


//MPS - Maximum PDU Payload Size
//MTU - Maximum Transmission Unit
//CID - Channel Identifier
//SPSM - Simplified Protocol/Service Multiplexer
#define SIGNAL_MINIMUM_MTU			23
#define SIGNAL_MINIMUM_MPS			23
#define SIGNAL_MAXIMUM_MPS			65533

#define SIGNAL_CREDIT_MINIMUM_MTU	64
#define SIGNAL_CREDIT_MINIMUM_MPS	64
#define SIGNAL_CREDIT_MAXIMUM_MPS	65533

typedef struct{
	u16 connHandle;
	u16 SPSM;
	u16 srcCID;
	u16 dstCID;
	u16 mtu;
	u16 mps;
	u16 sduLen;
	u16 recvLen;
	u16 sendTotalLen;
	u16 sendOffsetLen;
	u16 recvCredits;
	u16 sendCredits;

	u8* pRxSdu;
	u8* pTxSdu;
}l2cap_coc_cid_t;

//LE Only supported CID 0x0040 to 0x007F
//0x0020 to 0x003E LE-U Reserved for future use
//0xFFFF for send reconfigure request
typedef struct{
	u16 connHandle;
	u16 SPSM;
	u16 dstCIDNum;
	u16 dstCID[5];
}l2cap_coc_acl_t;

#if L2CAP_SERVER_FEATURE_SUPPORTED_EATT
//tx or rx packet, for MTU size
#define COC_MODULE_BUFFER_SIZE(createConnCnt, cocCidCnt, eattCidCnt, mtu)			\
		(\
			(createConnCnt)*sizeof(l2cap_coc_acl_t)  \
			+(cocCidCnt+eattCidCnt)*sizeof(l2cap_coc_cid_t) \
			+(mtu)*(cocCidCnt+2*eattCidCnt)  \
		)
#else
//tx or rx packet, for MTU size
#define COC_MODULE_BUFFER_SIZE(createConnCnt, cocCidCnt, mtu)			\
		(\
			(createConnCnt)*sizeof(l2cap_coc_acl_t)  \
			+(cocCidCnt)*sizeof(l2cap_coc_cid_t) \
			+(mtu)*(cocCidCnt)  \
		)
#endif


typedef struct{
	u16 SPSM;	//supported SPSM.
	u16 MTU;	//CID receive MTU.
	u16 createConnCnt;	//supported maximum ACL connect, create COC connect.
	u16 cocCidCnt;	//supported maximum COC CID count.
#if L2CAP_SERVER_FEATURE_SUPPORTED_EATT
	u16 eattCidCnt;	//supported maximum EATT CID count.
#endif
}blc_coc_initParam_t;

int blc_l2cap_registerCocModule(blc_coc_initParam_t* param, u8 *pBuffer, u16 buffLen);
ble_sts_t blc_l2cap_disconnectCocChannel(u16 connHandle, u16 srcCID);
ble_sts_t blc_l2cap_createLeCreditBasedConnect(u16 connHandle);
ble_sts_t blc_l2cap_createCreditBasedConnect(u16 connHandle, u8 srcCnt);
ble_sts_t blc_l2cap_sendCocData(u16 connHandle, u16 srcCID, u8* data, u16 dataLen);
void blc_l2cap_cocMainLoop(void);
ble_sts_t blc_signal_sendConnectParameterUpdateReq(u16 connHandle, u16 min_interval, u16 max_interval, u16 latency, u16 timeout);
ble_sts_t blc_signal_sendConnectParameterUpdateRsp(u16 connHandle, u8 identifier, conn_para_up_rsp result);
