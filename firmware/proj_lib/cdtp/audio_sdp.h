/********************************************************************************************************
 * @file	audio_sdp.h
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
#ifndef _AUDIO_SDP_H_
#define _AUDIO_SDP_H_


#if 1//(BLC_AUDIO_SDP_ENABLE)

typedef enum{
	BLT_AUDIO_SDP_FLAG_NONE            = 0x00,
	BLT_AUDIO_SDP_FLAG_WAIT            = 0x80,
	BLT_AUDIO_SDP_FLAG_SEARCH_SERVICE  = 0x01,
	BLT_AUDIO_SDP_FLAG_INIT_SERVICE    = 0x02,
	BLT_AUDIO_SDP_FLAG_LOOP_SERVICE    = 0x04,
		
	BLT_AUDIO_SDP_FLAG_DEFAULT         = BLT_AUDIO_SDP_FLAG_SEARCH_SERVICE | BLT_AUDIO_SDP_FLAG_INIT_SERVICE,
}BLT_AUDIO_SDP_FLAGS_ENUM;
typedef enum{
	BLT_AUDIO_SRV_FLAG_NONE     = 0x00,
	BLT_AUDIO_SRV_FLAG_WAIT     = 0x80,
	BLT_AUDIO_SRV_FLAG_AUTO     = 0x40, //Auto do: SEARCH_INCLUDE,SEARCH_CHARACT,FIND_CHARACT,AUTO_ENABLE_CCC
	BLT_AUDIO_SRV_FLAG_OTHR     = 0x20,
	BLT_AUDIO_SRV_FLAG_SEARCH_INCLUDE  = 0x01, 
	BLT_AUDIO_SRV_FLAG_SEARCH_CHARACT  = 0x02,
	BLT_AUDIO_SRV_FLAG_FIND_CHARACT    = 0x04,
	BLT_AUDIO_SRV_FLAG_ENABLE_CCC      = 0x08,
	
	BLT_AUDIO_SRV_FLAG_DEFAULT        = BLT_AUDIO_SRV_FLAG_SEARCH_CHARACT | BLT_AUDIO_SRV_FLAG_FIND_CHARACT,
}BLT_AUDIO_SRV_FLAG_ENUM;

typedef enum{
	BLT_AUDIO_CHAR_MODE_IDLE = 0x00,//Fill Character if idle
	BLT_AUDIO_CHAR_MODE_UUID = 0x01,//Find Character by charUUID
}BLT_AUDIO_CHAR_MODE_ENUM;
#endif //#if (BLC_AUDIO_SDP_ENABLE)

#endif //_AUDIO_SDP_H_

