/********************************************************************************************************
 * @file	audio_cap.h
 *
 * @brief	This is the header file for BLE SDK
 *
 * @author	BLE GROUP
 * @date	2020.06
 *
 * @par     Copyright (c) 2020, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Redistribution and use in source and binary forms, with or without
 *          modification, are permitted provided that the following conditions are met:
 *
 *              1. Redistributions of source code must retain the above copyright
 *              notice, this list of conditions and the following disclaimer.
 *
 *              2. Unless for usage inside a TELINK integrated circuit, redistributions
 *              in binary form must reproduce the above copyright notice, this list of
 *              conditions and the following disclaimer in the documentation and/or other
 *              materials provided with the distribution.
 *
 *              3. Neither the name of TELINK, nor the names of its contributors may be
 *              used to endorse or promote products derived from this software without
 *              specific prior written permission.
 *
 *              4. This software, with or without modification, must only be used with a
 *              TELINK integrated circuit. All other usages are subject to written permission
 *              from TELINK and different commercial license may apply.
 *
 *              5. Licensee shall be solely responsible for any claim to the extent arising out of or
 *              relating to such deletion(s), modification(s) or alteration(s).
 *
 *          THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *          ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *          WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *          DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 *          DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *          (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *          LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *          ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *          (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *          SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

