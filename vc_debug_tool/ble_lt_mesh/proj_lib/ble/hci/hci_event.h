/********************************************************************************************************
 * @file	hci_event.h
 *
 * @brief	for TLSR chips
 *
 * @author	telink
 * @date	Sep. 30, 2010
 *
 * @par     Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#ifndef HCI_EVENT_H_
#define HCI_EVENT_H_

#include "hci.h"
#include "../ble_common.h"

/**
 *  @brief  Definition for general HCI event packet
 */
typedef struct {
	hci_type_t type;
	u8         eventCode;
	u8         paraLen;
	u8         parameters[1];
} hci_event_t;



typedef struct {
	u8         status;
	u16        connHandle;
	u8         reason;
} hci_disconnectionCompleteEvt_t;



typedef struct {
	u8         numHciCmds;
	u8         opCode_OCF;
	u8		   opCode_OGF;
	u8         returnParas[1];
} hci_cmdCompleteEvt_t;


typedef struct {
	u8         status;
	u8         numHciCmds;
	u8         opCode_OCF;
	u8		   opCode_OGF;
} hci_cmdStatusEvt_t;



typedef struct {
	hci_type_t type;
	u8         eventCode;
	u8         paraLen;
	u8         subEventCode;
	u8         parameters[1];
} hci_le_metaEvt_t;


typedef struct {
	u8         subEventCode;
	u8         status;
	u16        connHandle;
	u8         role;
	u8         peerAddrType;
	u8         peerAddr[BLE_ADDR_LEN];
	u16        connInterval;
	u16        slaveLatency;
	u16        supervisionTimeout;
	u8         masterClkAccuracy;
} hci_le_connectionCompleteEvt_t;


typedef struct {
	u8         subEventCode;
	u8         status;
	u16        connHandle;
	u16        connInterval;
	u16        connLatency;
	u16        supervisionTimeout;
} hci_le_connectionUpdateCompleteEvt_t;


typedef struct {
	u8         subEventCode;
	u8         status;
	u16        connHandle;
	u8		   feature[LL_FEATURE_SIZE];
} hci_le_readRemoteFeaturesCompleteEvt_t;


typedef struct {
	u8         subEventCode;
	u16        connHandle;  // not aligned, but it's fine as long as it's not a pointer.
	u16  	   maxTxOct;
	u16		   maxTxtime;
	u16  	   maxRxOct;
	u16		   maxRxtime;
} hci_le_dataLengthChangeEvt_t;


/**
 *  @brief  Definition for HCI LE Read Local P-256 Public Key Complete event
 */
typedef struct {
	u8         subEventCode;
	u8         status;
	u8         localP256Key[64];
} hci_le_readLocalP256KeyCompleteEvt_t;

/**
 *  @brief  Definition for HCI LE generate DHKey Complete event
 */
typedef struct {
	u8         subEventCode;
	u8         status;
	u8         DHKey[32];
} hci_le_generateDHKeyCompleteEvt_t;


/**
 *  @brief  Definition for HCI long term key request event
 */
typedef struct {
	u8         subEventCode;
	u16        connHandle;
	u8         random[8];
	u16        ediv;
} hci_le_longTermKeyRequestEvt_t;


/**
 *  @brief  Definition for HCI Encryption Change event
 */
typedef struct {
	u8         status;
	u16        connHandle;
	u8         encryption_enable;
} hci_le_encryptEnableEvt_t;

/**
 *  @brief  Definition for HCI Encryption Key Refresh Complete event
 */
typedef struct {
	u8         status;
	u16        connHandle;
} hci_le_encryptKeyRefreshEvt_t;













void hci_disconnectionComplete_evt(u8 status, u16 connHandle, u8 reason);
int  hci_cmdComplete_evt(u8 numHciCmds, u8 opCode_ocf, u8 opCode_ogf, u8 paraLen, u8 *para, u8 *result);
void hci_cmdStatus_evt(u8 numHciCmds, u8 opCode_ocf, u8 opCode_ogf, u8 status, u8 *result);


void hci_le_connectionComplete_evt(u8 status, u16 connHandle, u8 role, u8 peerAddrType, u8 *peerAddr,
                                   u16 connInterval, u16 slaveLatency, u16 supervisionTimeout, u8 masterClkAccuracy);
void hci_le_connectionUpdateComplete_evt(u8 status, u16 connHandle, u16 connInterval,
        									u16 connLatency, u16 supervisionTimeout);
void hci_le_readRemoteFeaturesComplete_evt(u8 status, u16 connHandle, u8 * feature);




int hci_le_longTermKeyRequest_evt(u16 connHandle, u8* random, u16 ediv, u8* result);
int hci_le_readLocalP256KeyComplete_evt(u8* localP256Key, u8* result);
int hci_le_generateDHKeyComplete_evt(u8* DHkey,  u8* result);
int hci_le_encryptChange_evt(u16 connhandle,  u8 encrypt_en);
int hci_le_encryptKeyRefresh_evt(u16 connhandle);

int hci_remoteNateReqComplete_evt (u8* bd_addr);
#endif /* HCI_EVENT_H_ */













#if 0
typedef struct {
	hci_type_t type;
	u8         eventCode;
	u8         paraLen;
	u8         numHciCmds;
	u16        opCode;
	u8         returnParas[1];
} hci_cmdCompleteEvt_t;


typedef struct {
	hci_type_t type;
	u8         eventCode;
	u8         paraLen;
	u8         status;
	u8         numHciCmds;
	u16        opCode;
} hci_cmdStatusEvt_t;

/**
 *  @brief  Definition for HCI LE Read Local P-256 Public Key Complete event
 */
typedef struct {
	hci_type_t type;
	u8         eventCode;
	u8         paraLen;
	u8         subEventCode;
	u8         status;
	u8         localP256Key[32];
} hci_le_readLocalP256KeyCompleteEvt_t;

/**
 *  @brief  Definition for HCI LE generate DHKey Complete event
 */
typedef struct {
	hci_type_t type;
	u8         eventCode;
	u8         paraLen;
	u8         subEventCode;
	u8         status;
	u8         DHKey[32];
} hci_le_generateDHKeyCompleteEvt_t;


/**
 *  @brief  Definition for HCI long term key request event
 */
typedef struct {
	hci_type_t type;
	u8         eventCode;
	u8         paraLen;
	u8         subEventCode;
	u16        connHandle;
	u8         random[8];
	u16        ediv;
} hci_le_longTermKeyRequestEvt_t;
#endif
