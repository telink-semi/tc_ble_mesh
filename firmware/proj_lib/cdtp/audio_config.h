/********************************************************************************************************
 * @file	audio_config.h
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


#define BLC_AUDIO_ROLE_SWITCH_ENABLE                 1 // can not disable directly for OTS. do not known why.
#define BLC_AUDIO_MCP_TBP_RAAP_ROLE_SWITCH_ENABLE    1
#define BLC_AUDIO_OTP_ROLE_SWITCH_ENABLE             (1 && BLC_AUDIO_ROLE_SWITCH_ENABLE)
#define BLC_AUDIO_MCP_ROLE_SWITCH_ENABLE             (1 && BLC_AUDIO_ROLE_SWITCH_ENABLE)
#define BLC_AUDIO_TBP_ROLE_SWITCH_ENABLE             (1 && BLC_AUDIO_ROLE_SWITCH_ENABLE)
#define BLC_AUDIO_RAAP_ROLE_SWITCH_ENABLE            (1 && BLC_AUDIO_ROLE_SWITCH_ENABLE)
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
