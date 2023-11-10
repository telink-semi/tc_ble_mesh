/********************************************************************************************************
 * @file	audio_config.h
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
#ifndef _AUDIO_CONFIG_H_
#define _AUDIO_CONFIG_H_

#include  "tl_common.h"

#if ((CHIP_TYPE == CHIP_TYPE_8258) && (__PROJECT_MESH_PRO__ || __PROJECT_MESH_GW_NODE__))
#define BLC_AUDIO_PROFILE_EN           1 // MESH_CDTP_ENABLE
#else
#define BLC_AUDIO_PROFILE_EN           0
#endif

#if (BLC_AUDIO_PROFILE_EN)
#define BLC_AUDIO_DEBUG_ENABLE         1

#define BLC_AUDIO_SERVER_ENABLE        1


#define BLC_AUDIO_BSINK_ENABLE         1
#define BLC_AUDIO_BSOURCE_ENABLE       1
#define BLC_AUDIO_ASSISTANT_ENABLE     1
#define BLC_AUDIO_DELEGATOR_ENABLE     1

//Assistant
//Delegator

#if (!BLC_AUDIO_SERVER_ENABLE)
	#error "Server & Client must be supported one or more!"
#endif
#if BLC_AUDIO_SERVER_ENABLE
	#define BLC_AUDIO_SERVER_COUNT     1			// must 1 for CDTP. TODO: if "> 1", it will cause running user init() twice.
#endif
	#define BLC_AUDIO_CLIENT_COUNT     0
#define BLC_AUDIO_HANDLE_COUNT         (BLC_AUDIO_SERVER_COUNT+BLC_AUDIO_CLIENT_COUNT)


#define BLC_AUDIO_ROLE_SWICH_ENABLE                  1 // can not disable directly for OTS. do not kown why.
#define BLC_AUDIO_MCP_TBP_RAAP_ROLE_SWICH_ENABLE     1
#define BLC_AUDIO_OTP_ROLE_SWICH_ENABLE              (1 && BLC_AUDIO_ROLE_SWICH_ENABLE)
#define BLC_AUDIO_MCP_ROLE_SWICH_ENABLE              (1 && BLC_AUDIO_ROLE_SWICH_ENABLE)
#define BLC_AUDIO_TBP_ROLE_SWICH_ENABLE              (1 && BLC_AUDIO_ROLE_SWICH_ENABLE)
#define BLC_AUDIO_RAAP_ROLE_SWICH_ENABLE             (1 && BLC_AUDIO_ROLE_SWICH_ENABLE)
#define BLC_AUDIO_MCS_EXTEND_SUPPORT_ENABLE          0
#define BLC_AUDIO_OTS_EXTEND_SUPPORT_ENABLE          0


#define BLC_AUDIO_MCP_ENABLE           1 // can not disable directly for OTS. The IUT failed to return the UUID for Object Transfer Service.
#define BLC_AUDIO_OTP_ENABLE           (1 && BLC_AUDIO_MCP_ENABLE) // can not disable. because some code of OTS locate in OTP.

#define BLC_AUDIO_AICS_ENABLE          1
#define BLC_AUDIO_MCS_ENABLE           (1 && BLC_AUDIO_MCP_ENABLE)
#define BLC_AUDIO_OTS_ENABLE           (1 && BLC_AUDIO_OTP_ENABLE)


#define BLC_AUDIO_SERVER_SDP_ENABLE    0//(1 && BLC_AUDIO_SERVER_ENABLE) // CDTP no need SDP.
#define BLC_AUDIO_SDP_ENABLE           (BLC_AUDIO_SERVER_SDP_ENABLE)





#if (BLC_AUDIO_AICS_ENABLE)
	#define BLC_AUDIO_AICS_DESC_ENABLE    1
	#if (BLC_AUDIO_AICS_DESC_ENABLE)
		#define BLC_AUDIO_AICS_DESC_SIZE  32
	#endif
#endif

#endif //BLC_AUDIO_PROFILE_EN

#endif //_AUDIO_C ?I_'
