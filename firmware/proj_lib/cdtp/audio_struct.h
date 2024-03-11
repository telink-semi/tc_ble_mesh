/********************************************************************************************************
 * @file	audio_struct.h
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
#ifndef _AUDIO_STRUCT_H_
#define _AUDIO_STRUCT_H_

#if (BLC_AUDIO_PROFILE_EN)


typedef int(*BlcAudioEventCB)(u16 connHandle, u16 evtID, u16 dataLen, u8 *pData);



typedef struct{
	u8  aseID;
	u8  reason;
	u8  reserve;
	u8  errorCode; //0-success, other-fail
	u16 aclHandle;
	u16 attHandle;
}blc_audio_aseEnableEvt_t;
typedef struct{
	u8  aseID;
	u16 aclHandle;
	u16 cisHandle;
	u16 attHandle;
	u16 context;
}blc_audio_aseUpdateEvt_t;
typedef struct{
	u8  aseID;
	u16 aclHandle;
	u16 cisHandle;
	u16 attHandle;
}blc_audio_aseDisableEvt_t, blc_audio_aseReleaseEvt_t;
typedef struct{
	u8  aseID;
	u16 aclHandle;
	u16 cisHandle;
	u16 attHandle;
}blc_audio_aseStartEvt_t, blc_audio_aseStopEvt_t;

typedef struct{
	u16 aclHandle;
	u16 attHandle;
	u8  gainValue;
	u8  gainMute;
	u8  gainMode;
	u8  gainCounter;
}blc_audio_aicsStateChangeEvt_t;
typedef struct{
	u16 aclHandle;
	u16 attHandle;
	u8  status; // 0-Inactive, 1-Active
}blc_audio_aicsStatusChangeEvt_t;
typedef struct{
	u16 aclHandle;
	u16 attHandle;
	u32 dataLen;
	u8 *pData;
}blc_audio_aicsDescribleChangeEvt_t;

typedef struct{
	u16 aclHandle;
	u16 attHandle;
	s16 voffset;
	u8  counter;
}blc_audio_vocsStateChangeEvt_t;
typedef struct{
	u16 aclHandle;
	u16 attHandle;
	u32 location;
}blc_audio_vocpLocationChangeEvt_t;
typedef struct{
	u16 aclHandle;
	u16 attHandle;
	u32 dataLen;
	u8 *pData;
}blc_audio_vocpDescribleChangeEvt_t;


typedef struct{
	u16 aclHandle;
	u16 attHandle;
	u8 mute;
	u8 volume;
}blc_audio_vcpStateChangeEvt_t;
typedef struct{
	u16 aclHandle;
	u16 attHandle;
	u8  flag;
}blc_audio_vcpFlagChangeEvt_t;





typedef struct{
	u8 numb;
	u8 mute;
	u8 volume;
	u16 aclHandle;
}blc_audio_micVolumeChangeEvt_t;


typedef struct{
	u16 aclHandle;
}blc_audio_pacpServiceReadyEvt_t;


typedef struct{
	u16 aclHandle;
	u16 ctlHandle;
	u8  aseCount;
}blc_audio_ascpServiceReadyEvt_t;


typedef struct{
	u16 aclHandle;
	u16 sirkHandle;
	u16 sizeHandle;
	u16 lockHandle;
	u16 rankHandle;
	u8 size;
	u8 lock;
	u8 rank;
	u8 lockCCC;
	u8 SIRK[16];
}blc_audio_csipServiceReadyEvt_t;



typedef struct{
	u16 stateHandle;
	u16 ctrlHandle;
	u16 locaHandle;
	u16 descHandle;
	u16 voffset;
	u8  counter;
	u32 location;
}blc_audio_vocsServiceReadyEvt_t;


typedef struct{
	u16 aclHandle;
	u16 muteHandle;
	
	u8  muteValue;
	u8  aicsCount;
}blc_audio_micpServiceReadyEvt_t;
typedef struct{
	u16 aclHandle;
	u16 muteHandle;
	u8  muteValue;
}blc_audio_micpMuteChangeEvt_t;


typedef struct{
	u16 aclHandle;
	u16 ctlHandle;
	u16 statHandle;
	u16 flagHandle;
	
	u8  mute;
	u8  volume;
	u8  counter;
	u8  flagValue;
	u8  aicsCount;
	u8  vocsCount;
}blc_audio_vcpServiceReadyEvt_t;


typedef struct{
	u16 aclHandle;
	u16 cisHandle;
}blc_audio_mcpServiceReadyEvt_t;
typedef struct{
	u16 aclHandle;
	u16 attHandle;
	u8  opcode; //Refer to BLC_AUDIO_MCP_OPCODE_ENUM
	u8  dataLen;
	u8 *pData;
}blc_audio_mcpCtrlEvt_t;
typedef struct{
	u16 aclHandle;
	u16 attHandle;
	u8  state; //Refer to BLC_AUDIO_MEDIA_STATE_ENUM
}blc_audio_mcpStateEvt_t;


typedef struct{
	u16 CCIDHandle;
	u16 inCallHandle;
	u16 callCtrlHandle;
	u16 provNameHandle;
	u16 callerIDHandle;	
	u16 callStateHandle;
	u16 bearerTechHandle;
	u16 CCPOpcodesHandle;
	u16 termReasonHandle;
	u16 curCallListHandle;
}blc_audio_tbpServiceReadyEvt_t;
typedef struct{
	u8 index;
	u8 state;
	u8 callFlags;
}blc_audio_tbpCallStateEvt_t;




#endif //#if (BLC_AUDIO_PROFILE_EN)

#endif //
