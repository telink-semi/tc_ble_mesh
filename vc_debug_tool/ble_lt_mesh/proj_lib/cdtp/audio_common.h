/********************************************************************************************************
 * @file	audio_common.h
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
#ifndef _AUDIO_COMMON_H_
#define _AUDIO_COMMON_H_

#if (BLC_AUDIO_PROFILE_EN)
#include "proj_lib/sig_mesh/app_mesh.h"


#define BltAudioWord8ArrayIsEqual(array, word)   ((array)[0] == ((word) & 0xFF) && (array)[1] == (((word) & 0xFF00)>>8))
#define BltAudioIsThisUUID2(uuid, value)         BltAudioWord8ArrayIsEqual(uuid, value)

#define BltAudioValueSplit2Array(Array, value) \
		(Array)[0] = (value) & 0xFF; \
		(Array)[1] = ((value) & 0xFF00) >> 8; 
#define BltAudioValueSplit3Array(Array, value) \
		(Array)[0] = (value) & 0xFF; \
		(Array)[1] = ((value) & 0xFF00) >> 8; \
		(Array)[2] = ((value) & 0xFF0000) >> 16; 
#define BltAudioValueSplit4Array(Array, value) \
		(Array)[0] = (value) & 0xFF; \
		(Array)[1] = ((value) & 0xFF00) >> 8; \
		(Array)[2] = ((value) & 0xFF0000) >> 16; \
		(Array)[3] = ((value) & 0xFF000000) >> 24; 

#define BltAudio2ArraySpellValue(value, Array) \
		(value) = (((u16)(Array)[1])<<8) | ((u16)(Array)[0])
#define BltAudio3ArraySpellValue(value, Array) \
		(value) = (((u32)(Array)[2])<<16) | (((u32)(Array)[1])<<8) | ((u32)(Array)[0])
#define BltAudio4ArraySpellValue(value, Array) \
		(value) = (((u32)(Array)[3])<<24) | (((u32)(Array)[2])<<16) | (((u32)(Array)[1])<<8) | ((u32)(Array)[0])		


#if BLC_AUDIO_DEBUG_ENABLE
	#define CONSOLE_DBGID_FATA                     0xD1
	#define CONSOLE_DBGID_ERROR                    0xD2
	#define CONSOLE_DBGID_INFO                     0xD3
	#define CONSOLE_DBGID_WARN                     0xD4
	#define CONSOLE_DBGID_TRACE                    0xD5

	#define send_dbgmsg_fata(datalen, pData)       blt_audio_sendDbgMsg(CONSOLE_DBGID_FATA, datalen, pData)
	#define send_dbgmsg_error(datalen, pData)      blt_audio_sendDbgMsg(CONSOLE_DBGID_ERROR, datalen, pData)
	#define send_dbgmsg_info(datalen, pData)       blt_audio_sendDbgMsg(CONSOLE_DBGID_INFO, datalen, pData)
	#define send_dbgmsg_warn(datalen, pData)       blt_audio_sendDbgMsg(CONSOLE_DBGID_WARN, datalen, pData)
	#define send_dbgmsg_trace(datalen, pData)      blt_audio_sendDbgMsg(CONSOLE_DBGID_TRACE, datalen, pData)
	void blt_audio_sendDbgMsg(u8 dbgId, u8 datalen, u8 *pData);
#endif //BLC_AUDIO_DEBUG_ENABLE


typedef struct{
	u16 handle;
	u8 uuidLen;
	u8 dataLen;
	u8 *pCCC;
	u8 *pUuid;
	u8 *pData;
}blt_audio_charParam_t;
typedef struct{
	u8	type;
	u8  rf_len;
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8  handle[2];
	u8  value[1];
}blt_audio_attNotify_t;
typedef struct{
	u8	type;
	u8  rf_len;
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8  reqcode;
	u8  handle[2];
	u8  errcode;
}blt_audio_attResp_t;
typedef struct{
	u8	type;
	u8  rf_len;
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8  value[1];
}blt_audio_attPkt_t;

typedef struct{
	u8	type;
	u8  rf_len;
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8 	handle[2];
}blt_audio_attReadReq_t;

typedef struct{
	u8	type;
	u8  rf_len;
	u16	l2capLen;
	u16	chanId;
	u8  opcode;
	u8 	handle[2];
	u8  value[1];
}blt_audio_attWriteReq_t;


static inline u8 blt_audio_getBitCount(u32 value)
{
	u8 count;
	
	count = 0;
	while(value){
		count ++;
		value &= (value-1);
	}
	
	return count;
}

u8 blt_audio_findNextChar(attribute_t *pServer, u8 *pOffset, blt_audio_charParam_t *pParam);
ble_sts_t blt_audio_pushHandleValue(u16 connHandle, u16 attHandle, u8 opcode, u8 *p, int len);
ble_sts_t blt_audio_pushReadRsp(u16 connHandle, u8 *p, int len);
ble_sts_t blt_audio_pushBlobReadRsp(u16 connHandle, u8 *p, int len);
ble_sts_t blt_audio_pushErrorRsp(u16 connHandle, u8 errOpcode, u16 errHandle, u8 errCode);
ble_sts_t blt_audio_pushWriteRsp(u16 connHandle);

int blt_audio_sendEvent(blt_audio_handle_t *pHandle, u16 evtID, u16 dataLen, u8 *pData);


#endif //#if (BLC_AUDIO_PROFILE_EN)

#endif //_AUDIO_COMMON_H_

