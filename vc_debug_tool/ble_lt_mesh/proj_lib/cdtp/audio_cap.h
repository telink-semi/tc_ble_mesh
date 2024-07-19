/********************************************************************************************************
 * @file	audio_cap.h
 *
 * @brief	This is the header file for BLE SDK
 *
 * @author	BLE GROUP
 * @date	2020.06
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
#ifndef _AUDIO_CAP_H_
#define _AUDIO_CAP_H_

#include "audio_config.h"

#if (BLC_AUDIO_PROFILE_EN)
//#include "stack/ble/host/att/att.h"
#include "audio_define.h"
#include "audio_struct.h"

int  blc_audio_init(void);

bool blc_audio_isBusy(u16 connHandle);
bool blc_audio_sdpIsBusy(u16 connHandle);

bool blc_audio_setConnState(u16 connHandle, BLC_AUDIO_ROLE_ENUM role, BLC_AUDIO_STATE_ENUM state);

void blc_audio_regEventCB(u8 connHandle, BlcAudioEventCB evtCB);
void blc_audio_regEventCBByRole(u8 role, BlcAudioEventCB evtCB);
void blc_audio_regEventCBForAll(BlcAudioEventCB evtCB);

int  blc_audio_setService(u8 connHandle, BLC_AUDIO_SERVICE_ENUM srvType, attribute_t *pService);
int  blc_audio_setServiceByRole(u8 role, BLC_AUDIO_SERVICE_ENUM srvType, attribute_t *pService);

int  blc_audio_gattHandler(u16 connHandle, u8 *pkt);

void blc_audio_handler(void);


extern u16 blc_audio_getConnHandle(u16 cisHandle, u8 role);



#if (BLC_AUDIO_MCS_ENABLE)
	extern int blc_audio_mcpAttRead(u16 connHandle, void* p);
	extern int blc_audio_mcpAttWrite(u16 connHandle, void* p);
	extern int blc_audio_mcpEnable(u16 connHandle);
	extern int blc_audio_mcpDisable(u16 connHandle);
	extern int blc_audio_mcpSendCtrl(u16 connHandle, u8 opcode, u8 *pData, u16 dataLen);
	extern int blc_audio_mcpSetMediaState(u16 connHandle, u8 state, bool isNoty);
	extern int blt_audio_mcpSetTrackPosition(u16 connHandle, u32 value, bool isNoty);
#endif


#if (BLC_AUDIO_OTS_ENABLE)
	extern int blc_audio_otpAttRead(u16 connHandle, void* p);
	extern int blc_audio_otpAttWrite(u16 connHandle, void* p);
	extern int blc_audio_otpSetObjectName(u16 connHandle, u8 *pName, u16 nameLen);
	extern int blc_audio_otpSendActionCtrl(u16 connHandle, u8 opcode, u8 *pData, u8 dataLen);
	extern int blc_audio_otpSendListCtrl(u16 connHandle, u8 opcode, u8 *pData, u8 dataLen);
#endif


#endif //#if (BLC_AUDIO_PROFILE_EN)

#endif //_AUDIO_CAP_H_

