/********************************************************************************************************
 * @file	audio_common.h
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

