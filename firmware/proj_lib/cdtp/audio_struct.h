/********************************************************************************************************
 * @file	audio_struct.h
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
