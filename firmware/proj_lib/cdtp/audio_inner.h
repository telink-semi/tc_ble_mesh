/********************************************************************************************************
 * @file	audio_inner.h
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
#ifndef _AUDIO_INNER_H_
#define _AUDIO_INNER_H_


#if (BLC_AUDIO_PROFILE_EN)


#define BLT_AUDIO_SINK_AES_MAX        2
#define BLT_AUDIO_SOURCE_AES_MAX      2



typedef enum{
	BLT_AUDIO_BUSY_NONE = 0x0000,
	BLT_AUDIO_BUSY_OTP  = BIT(BLC_AUDIO_SERVICE_OTS),
	
	BLT_AUDIO_BUSY_SDP  = BIT(14),
	BLT_AUDIO_BUSY_SDP_START = BIT(15),
}BLT_AUDIO_BUSY_ENUM;

typedef enum{
	BLT_AUDIO_GFLAG_NONE = 0x0000,
	BLT_AUDIO_GFLAG_PACS = BIT(0),
	BLT_AUDIO_GFLAG_ASCS = BIT(1),
	BLT_AUDIO_GFLAG_BACS = BIT(2),
	BLT_AUDIO_GFLAG_CSIS = BIT(3),
	BLT_AUDIO_GFLAG_RAAS = BIT(4),
	BLT_AUDIO_GFLAG_MICS = BIT(5),
	BLT_AUDIO_GFLAG_VCS  = BIT(6),
	BLT_AUDIO_GFLAG_MCS  = BIT(7),
	BLT_AUDIO_GFLAG_TBS  = BIT(8),
	BLT_AUDIO_GFLAG_OTS  = BIT(9),
}BLT_AUDIO_GFLAGS_ENUM;



#if 1//(BLC_AUDIO_SDP_ENABLE)

#define BLT_AUDIO_SDP_SRV_COUNT       8   //all service,like mcs,vcs
#define BLT_AUDIO_SDP_INC_COUNT       2   //include service
#define BLT_AUDIO_SDP_CHAR_COUNT      25  //
#define BLT_AUDIO_SDP_OTHR_COUNT      2

typedef struct blt_audio_srv_s blt_audio_srv_t;
typedef struct blt_audio_sdp_s blt_audio_sdp_t;
typedef void(*BlcAudioSdpFindServiceFunc)(blt_audio_sdp_t *pSdp, blt_audio_srv_t *pSrv);
typedef void(*BlcAudioSdpInitServiceFunc)(blt_audio_sdp_t *pSdp, blt_audio_srv_t *pSrv);
typedef void(*BlcAudioSdpLoopHandlerFunc)(blt_audio_sdp_t *pSdp, blt_audio_srv_t *pSrv);
typedef void(*BlcAudioSdpOverHandlerFunc)(blt_audio_sdp_t *pSdp, blt_audio_srv_t *pSrv);
typedef  int(*BlcAudioSdpGattHandlerFunc)(blt_audio_sdp_t *pSdp, blt_audio_srv_t *pSrv, u8 *pkt);

typedef struct{
	u16 sHandle;
	u16 eHandle;
	u16 srvUUID;
}blt_audio_inc_t;
typedef struct{ // 12Bytes
	u16 charUUID;
	u16 dHandle; //data handle
	u16 othrUUID[BLT_AUDIO_SDP_OTHR_COUNT];
	u16 othrHandle[BLT_AUDIO_SDP_OTHR_COUNT];
}blt_audio_char_t;
struct blt_audio_srv_s{
	u8	used; //Adapter to multi same ServiceUUID.
	u8  mode; //Refer to BLT_AUDIO_CHAR_MODE_ENUM.
	u8  flags; //Refer to BLT_AUDIO_SRV_FLAG_ENUM.
	u8  curChar; //
	u16 srvUUID;
	u16 sHandle;
	u16 eHandle;
	u16 oHandle;
	BlcAudioSdpFindServiceFunc findFunc;
	BlcAudioSdpInitServiceFunc initFunc;
	BlcAudioSdpLoopHandlerFunc loopFunc;
	BlcAudioSdpOverHandlerFunc overFunc;
	BlcAudioSdpGattHandlerFunc gattFunc;
};
struct blt_audio_sdp_s{
	u8  start;
	u8  flags;
	u8  other;
	u8  curSrv;
	u16 handle; //Connect Handle
	u16 sHandle;
	u16 eHandle;
	u16 oHandle;  //current handle
	u32 startTimer;
	blt_audio_srv_t *servPtr[BLT_AUDIO_SDP_SRV_COUNT];  //It is unique for each service.
	blt_audio_inc_t includes[BLT_AUDIO_SDP_INC_COUNT];  //The number of include service should be dynamic
	blt_audio_char_t charact[BLT_AUDIO_SDP_CHAR_COUNT]; //This will be Shared by multiple services
};
#endif //#if (BLC_AUDIO_SDP_ENABLE)



#if (BLC_AUDIO_AICS_ENABLE)
typedef struct{
	u16 flags;
	u16 resv0;
	u16 sHandle;
	u16 eHandle;
	u16 ctrlHandle;
	u16 stateHandle;
	u16 propeHandle;
	u16 inputHandle;
	u16 statusHandle;
	u16 descHandle;

	u8  gainStateValue;
	u8  gainStateMute;
	u8  gainStateMode;
	u8  gainStateCount;

	u8  gainPropeUnit;
	u8  gainPropeMin;
	u8  gainPropeMax;
	u8  reserve1;

	u8  gainType;
	u8  gainStatus;
	u8  stateCCC;
	u8  statusCCC;

	#if (BLC_AUDIO_AICS_DESC_ENABLE)
		u32 descLen;
		u8  desc[BLC_AUDIO_AICS_DESC_SIZE];
	#endif
}blt_audio_aics_t;
#endif //BLC_AUDIO_AICS_ENABLE


#if (BLC_AUDIO_MCP_ENABLE)
typedef struct{
	u16 flags;
	u8  enable;
	u8  serial;
	u8  isValid;
	u8  cpOpcode; //Control Point Opcode
	u8  resultCode;
	u8  mediaState;
	u8  trackIsStart;
	u8  reserve000;
	u16 playNameHandle;
	u16 trackChangedHandle;
	u16 trackTitleHandle;
	u16 trackDurationHandle;
	u16 trackPositionHandle;
	u16 mediaStateHandle;
	u16 mediaCtrlHandle;
	u16 opcodesSuppHandle;
	u16 CCIDHandle;

	u16 trackDurationValue;
	u32 trackPositionValue; //Should be in [0,trackDurationValue]
	
	#if (BLC_AUDIO_MCS_EXTEND_SUPPORT_ENABLE)
	u8  seekingSpeedValue;
	u8  searchControlStatus;
	u16 seekingSpeedHandle;
	u16 searchControlHandle;
	u16 searchResultHandle;
	u8  searchObjectID[6];
	#endif
}blt_audio_mcpCtrl_t;    //client and server all will use.
#endif //BLC_AUDIO_MCP_ENABLE


//#define CHARACTERISTIC_UUID_TBS_PROVIDER_NAME          //M Mandatory:Read, Notify;   Optional:
//#define CHARACTERISTIC_UUID_TBS_CALLER_IDENTIFIER      //M Mandatory:Read;   Optional:
//#define CHARACTERISTIC_UUID_TBS_TECHNOLOGY             //M Mandatory:Read, Notify;   Optional:
//#define CHARACTERISTIC_UUID_TBS_URI_LIST               //M Mandatory:Read;   Optional:Notify
//#define CHARACTERISTIC_UUID_TBS_CURRENT_CALL_LIST      //M Mandatory:Read, Notify;   Optional:
//#define CHARACTERISTIC_UUID_TBS_CCID                   //M Mandatory:Read;   Optional:
//#define CHARACTERISTIC_UUID_TBS_STATUS_FLAGS           //M Mandatory:Read, Notify;   Optional:
//#define CHARACTERISTIC_UUID_TBS_CALL_STATE             //M Mandatory:Read, Notify;   Optional:
//#define CHARACTERISTIC_UUID_TBS_CALL_CONTROL_POINT     //M Mandatory:Write, Write Without Response, Notify;   Optional:
//#define CHARACTERISTIC_UUID_TBS_CCP_OPTIONAL_OPCODES   //M Mandatory:Read;   Optional:
//#define CHARACTERISTIC_UUID_TBS_TERMINATIONO_REASON    //M Mandatory:Notify;   Optional:
//#define CHARACTERISTIC_UUID_TBS_INCOMING_CALL          //M Mandatory:Read, Notify;   Optional:

#if (BLC_AUDIO_OTP_ENABLE)


typedef struct object
{
  u8  object_index;
  u8  ID[8];             //6bytes valid
  u8  name[32];          //all maybe used
  u8  name_length;
  u16 type;              //2byte
  u32 current_size;
  u32 allocated_size;
  u8  first_created[8];  //7byte valid
  u8  last_modified[8];  //7byte valid
  u32 property;          //access permission for the current object
} object_t;

#define BLC_AUDIO_OTP_NAME_BUFFER_LEN      32
#define BLC_AUDIO_OTP_FILTER_BUFFER_LEN    32

typedef struct{
	u16 flags;

	u8  oacpOpcode;
	u8  oacpStatus;
	
	u16 featureHandle;
	u16 objectNameHandle;
	u16 objectTypeHandle;
	u16 objectSizeHandle;
	u16 objectPropertiesHandle;
	u16 objectActionCPHandle;
	
	u8  objectNameLength;

	u16 objectTypeValue;
	u32 objectCurrentSizeValue;
	u32 objectAllocatedSizeValue;
	u32 objectPropertiesValue;
	u8  featureValue[8];
	u8  objectNameValue[BLC_AUDIO_OTP_NAME_BUFFER_LEN];
	
	#if (BLC_AUDIO_OTS_EXTEND_SUPPORT_ENABLE)
	u16 firstCreatedHandle;
	u16 lastCreatedHandle;
	u16 objectIDHandle;
	u16 objectListCPHandle;
	u16 listFilterHandle;
	u16 objectChangedHandle;

	u8  listFilterLength;

	u8  olcpOpcode;
	u8  olcpStatus;
	
	u8  firstCreatedValue[8]; //Valid:7
	u8  lastCreatedValue[8]; //Valid:7
	u8  objectIDValue[8]; //Valid:6
	u8  listFilterValue[BLC_AUDIO_OTP_FILTER_BUFFER_LEN];
	#endif //BLC_AUDIO_OTS_EXTEND_SUPPORT_ENABLE
	u8  object_list_index;//1 -> 10   stand for the current object in object list. if the index is 0x00,it is invalid ,the current index is empty.
}blt_audio_otpCtrl_t;
#endif //BLC_AUDIO_OTP_ENABLE


typedef struct{
	u8  role; //Refer to BLC_AUDIO_ROLE_ENUM
	u8  conn; //True if used, false if not used
	u16 busy; //Refer to BLT_AUDIO_BUSY_ENUM
	u16 handle; //Connect Handle
	u16 gFlags; //Global flags (this is not change). Refer to BLT_AUDIO_GFLAGS_ENUM
	u8  status;
	BlcAudioEventCB evtCB;
	#if (BLC_AUDIO_MCP_ENABLE)
	blt_audio_mcpCtrl_t mcp;
	#endif 
	#if (BLC_AUDIO_OTS_ENABLE)
	blt_audio_otpCtrl_t otp;
	#endif

	#if (BLC_AUDIO_SERVER_SDP_ENABLE)
	blt_audio_sdp_t sdp;
	#endif
	#if !(BLC_AUDIO_MCP_ROLE_SWITCH_ENABLE)
		#if (BLC_AUDIO_MCS_ENABLE)
		blt_audio_srv_t mcsSrv;
		#endif //BLC_AUDIO_MCS_ENABLE
	#endif //BLC_AUDIO_MCP_ROLE_SWITCH_ENABLE
	#if !(BLC_AUDIO_OTP_ROLE_SWITCH_ENABLE)
		#if (BLC_AUDIO_OTS_ENABLE)
		blt_audio_srv_t otsSrv;
		#endif //BLC_AUDIO_OTS_ENABLE
	#endif //BLC_AUDIO_OTP_ROLE_SWITCH_ENABLE
}blt_audio_server_t;

typedef struct{
	u8  role; //Refer to BLC_AUDIO_ROLE_ENUM
	u8  conn; //True if used, false if not used
	u16 busy; //Refer to BLT_AUDIO_BUSY_ENUM
	u16 handle; //Connect Handle
	u16 gFlags; //Global flags. Refer to BLT_AUDIO_GFLAGS_ENUM
	u8  status;
	BlcAudioEventCB evtCB;
	#if (BLC_AUDIO_MCP_ENABLE)
	blt_audio_mcpCtrl_t mcp;
	#endif 
	#if (BLC_AUDIO_OTS_ENABLE)
	blt_audio_otpCtrl_t otp;
	#endif
	
}blt_audio_client_t;

typedef struct{
	u8  role; //Refer to BLC_AUDIO_ROLE_ENUM
	u8  conn; //True if used, false if not used
	u16 busy; //Refer to BLT_AUDIO_BUSY_ENUM
	u16 handle; //Connect Handle
	u16 gFlags; //Global flags. Refer to BLT_AUDIO_GFLAGS_ENUM
	u8  status;
	BlcAudioEventCB evtCB;
	#if (BLC_AUDIO_MCP_ENABLE)
	blt_audio_mcpCtrl_t mcp;
	#endif 
	#if (BLC_AUDIO_OTS_ENABLE)
	blt_audio_otpCtrl_t otp;
	#endif
}blt_audio_handle_t;  //global.



typedef struct{
	int  (*Init)(blt_audio_handle_t *pHandle);
	int  (*GattIn)(blt_audio_handle_t *pHandle, u8 *pPkt);
	int  (*SetServ)(blt_audio_handle_t *pHandle, attribute_t *pService);
	void (*SetConn)(blt_audio_handle_t *pHandle, BLC_AUDIO_STATE_ENUM state);
	void (*Process)(blt_audio_handle_t *pHandle);
}blt_audio_func_t;


#endif //BLC_AUDIO_PROFILE_EN

#endif //_AUDIO_INNER_H_
