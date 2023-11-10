/********************************************************************************************************
 * @file	audio_define.h
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
#ifndef _AUDIO_DEFINE_H_
#define _AUDIO_DEFINE_H_

#if (BLC_AUDIO_PROFILE_EN)


typedef enum{
	BLC_AUDIO_ROLE_CLIENT = 0x01,
	BLC_AUDIO_ROLE_SERVER = 0x02,
	
	BLC_AUDIO_ROLE_BSINK     = 0x06,
	BLC_AUDIO_ROLE_BSOURCE   = 0x07,
	BLC_AUDIO_ROLE_ASSISTANT = 0x08,
	BLC_AUDIO_ROLE_DELEGATOR = 0x09,
}BLC_AUDIO_ROLE_ENUM;

typedef enum{
	BLC_AUDIO_STATE_DISCONN   = 0x00,
	BLC_AUDIO_STATE_CONNECT   = 0x01,
	BLC_AUDIO_STATE_RECONNECT = 0x02,
}BLC_AUDIO_STATE_ENUM;

typedef enum{
	BLC_AUDIO_SUCCESS   =  0x00,
	BLC_AUDIO_EBUSY,
	BLC_AUDIO_EFAIL,
	BLC_AUDIO_EROLE,
	BLC_AUDIO_EASEID,
	BLC_AUDIO_EQUOTA,
	BLC_AUDIO_EALIGN,
	BLC_AUDIO_EPARAM,
	BLC_AUDIO_EHANDLE,
	BLC_AUDIO_EREPEAT,
	BLC_AUDIO_ESTATUS,
	BLC_AUDIO_ELENGTH,
	BLC_AUDIO_ENOSUPP, //not supperted
	BLC_AUDIO_ENOREADY, 
	BLC_AUDIO_ENOOBJECT,
}BLC_AUDIO_ERROR_ENUM;


typedef enum{
	BLC_AUDIO_SERVICE_PACS = 0,
	BLC_AUDIO_SERVICE_ASCS,
	BLC_AUDIO_SERVICE_CSIS,
	BLC_AUDIO_SERVICE_RAAS,
	BLC_AUDIO_SERVICE_MICS,
	BLC_AUDIO_SERVICE_VCS,
	BLC_AUDIO_SERVICE_MCS,
	BLC_AUDIO_SERVICE_TBS,
	BLC_AUDIO_SERVICE_OTS,
	BLC_AUDIO_SERVICE_UNICAST_MAX,
	
	BLC_AUDIO_SERVICE_BASS = BLC_AUDIO_SERVICE_UNICAST_MAX,
	BLC_AUDIO_SERVICE_MAX,
}BLC_AUDIO_SERVICE_ENUM;

typedef enum{
	BLC_AUDIO_EVTID_NONE,

	//SDP
	BLC_AUDIO_EVTID_SDP_OVER,
	
	///ASCS Event
	BLC_AUDIO_EVTID_ASE_ENABLE,
	BLC_AUDIO_EVTID_ASE_UPDATE,
	BLC_AUDIO_EVTID_ASE_DISABLE,
	BLC_AUDIO_EVTID_ASE_RELEASE,
	BLC_AUDIO_EVTID_RECV_START, //For Server and Client
	BLC_AUDIO_EVTID_RECV_STOP,  //For Server and Client

	//CSIS Event
	BLC_AUDIO_EVTID_CSIS_READY,
	
	///MICP Event
	BLC_AUDIOC_EVTID_MICS_READY, //For Audio Client
	BLC_AUDIO_EVTID_MICS_MUTE,   //For Server and Client
	BLC_AUDIO_EVTID_MICS_AICS_STATE, //For Server and Client
	BLC_AUDIO_EVTID_MICS_AICS_STATUS, //For Server and Client
	
	///VCS Event
	BLC_AUDIOC_EVTID_VCS_READY, //For Audio Client
	BLC_AUDIO_EVTID_VCS_STATE, //For Server and Client
	BLC_AUDIO_EVTID_VCS_FLAG, //For Server and Client
	BLC_AUDIO_EVTID_VCS_AICS_STATE, //For Server and Client
	BLC_AUDIO_EVTID_VCS_VOCS_STATE, //For Server and Client
	BLC_AUDIO_EVTID_VCS_AICS_STATUS, //For Server and Client
	BLC_AUDIO_EVTID_VCS_VOCS_LOCATION, //For Server and Client
	BLC_AUDIO_EVTID_VCS_VOCS_DESCRIBLE, //For Server and Client
	BLC_AUDIO_EVTID_VCS_AICS_DESCRIBLE, //For Server and Client

	///MCP Event
	BLC_AUDIOS_EVTID_MCS_READY, //For server
	BLC_AUDIOS_EVTID_MCS_STATE, //For Server
	BLC_AUDIOS_EVTID_MCS_NOTY,  //For Server
	BLC_AUDIOC_EVTID_MCP_CTRL,  //For Client	
	
	///PACP Event
	BLC_AUDIOC_EVTID_PACS_READY, //For Audio Client
	
	///ASCP Event
	BLC_AUDIOC_EVTID_ASCS_READY, //For Audio Client

	//TBS Event
	BLC_AUDIOS_EVTID_TBS_READY,
	BLC_AUDIOS_EVTID_TBS_CALL_STATE,

	//OTS Event
	BLC_AUDIOS_EVTID_OTS_READY,
	
	
}BLC_AUDIO_EVTID_ENUM;



#if 0
// TODO: TBD
// PACS Service & Characteristic
#define SERVICE_UUID_PUBLISHED_AUDIO_CAPABILITIES            0x2310 //PACS
#define CHARACTERISTIC_UUID_PACS_SINK_PAC                    0x2311 //Mandatory:Read; Optional:Notify
#define CHARACTERISTIC_UUID_PACS_SINK_AUDIO_LOCATION         0x2312 //Mandatory:Read; Optional:Notify,Write
#define CHARACTERISTIC_UUID_PACS_SOURCE_PAC                  0x2313 //Mandatory:Read; Optional:Notify
#define CHARACTERISTIC_UUID_PACS_SOURCE_AUDIO_LOCATION       0x2314 //Mandatory:Read; Optional:Notify,Write
#define CHARACTERISTIC_UUID_PACS_AVAILABLE_AUDIO_CONTEXT     0x2315 //Mandatory:Read,Notify; Optional:
#define CHARACTERISTIC_UUID_PACS_SUPPORTED_AUDIO_CONTEXT     0x2316 //Mandatory:Read; Optional:Notify
// ASCS Service & Characteristic
#define SERVICE_UUID_AUDIO_STREAM_CONTROL                    0x2320 
#define CHARACTERISTIC_UUID_ASCS_ASE                         0x2321 //Read, Notify
#define CHARACTERISTIC_UUID_ASCS_ASE_CONTROL_PPOINT          0x2322 //WriteWithoutResponse, Notify
// BACS Service & Characteristic
#define SERVICE_UUID_BROADCAST_AUDIO_SCAN                    0x2330 //PACS
// CSIS Service & Characteristic -- ok
#define SERVICE_UUID_COPRDINATED_SET_IDENTIFICATION          0x1846
#define CHARACTERISTIC_UUID_CSIS_SIRK                        0x2B84 //M  Mandatory:Read; Optional:Notify
#define CHARACTERISTIC_UUID_CSIS_SIZE                        0x2B85 //O  Mandatory:Read; Optional:Notify
#define CHARACTERISTIC_UUID_CSIS_LOCK                        0x2B86 //O  Mandatory:Read,Write,Notify; Optional:
#define CHARACTERISTIC_UUID_CSIS_RANK                        0x2B87 //O  Mandatory:Read; Optional:
// RAAS Service & Characteristic
#define SERVICE_UUID_ROUTING_ACTIVE_AUDIO                    0x2350
#define CHARACTERISTIC_UUID_RAAS_SELECTABLE_ARE              0x2351 // Selectable Audio Route Endpoint List
#define CHARACTERISTIC_UUID_RAAS_CONFIGURED_ARE              0x2352 // Configured Audio Routes List
#define CHARACTERISTIC_UUID_RAAS_CONFIGURED_CONTENT          0x2353 // Configured Audio Routes Content Type List
#define CHARACTERISTIC_UUID_RAAS_MODIFY_ARC                  0x2354 // Modify Audio Routing Control Point

// AICS Service & Characteristic -- ok
#define SERVICE_UUID_AUDIO_INPUT_CONTROL                     0x1843 //O
#define CHARACTERISTIC_UUID_AICS_INPUT_STATE                 0x2B77 //M Mandatory:Read,Notify;   Optional:
#define CHARACTERISTIC_UUID_AICS_GAIN_SETTING                0x2B78 //M Mandatory:Read;   Optional:
#define CHARACTERISTIC_UUID_AICS_INPUT_TYPE                  0x2B79 //M Mandatory:Read;   Optional:
#define CHARACTERISTIC_UUID_AICS_INPUT_STATUS                0x2B7A //M Mandatory:Read,Notify;   Optional:
#define CHARACTERISTIC_UUID_AICS_INPUT_CONTROL               0x2B7B //M Mandatory:Write;   Optional:
#define CHARACTERISTIC_UUID_AICS_INPUT_DESCRIP               0x2B7C //M Mandatory:Read;   Optional:Write Without Response, Notify

// MICS Service & Characteristic   -- ok
#define SERVICE_UUID_MICROPHONE_CONTROL                      0x184D
#define CHARACTERISTIC_UUID_MICS_MUTE                        0x2BC3 //M Mandatory:Read,Write,Notify;   Optional:

// VCS  Service & Characteristic -- ok
#define SERVICE_UUID_VOLUME_CONTROL                          0x1844
#define CHARACTERISTIC_UUID_VCS_STATE                        0x2B7D //Mandatory:Read,Notify;   Optional:
#define CHARACTERISTIC_UUID_VCS_CONTROL_POINT                0x2B7E //Mandatory:Write;   Optional:
#define CHARACTERISTIC_UUID_VCS_FLAGS                        0x2B7F //Mandatory:Read,Notify;   Optional:
// VOCS  Service & Characteristic -- ok
#define SERVICE_UUID_VOLUME_OFFSET_CONTROL                    0x1845
#define CHARACTERISTIC_UUID_VOCS_STATE                        0x2B80
#define CHARACTERISTIC_UUID_VOCS_LOCATION                     0x2B81
#define CHARACTERISTIC_UUID_VOCS_CONTROL                      0x2B82
#define CHARACTERISTIC_UUID_VOCS_DESCRIPT                     0x2B83

// MCS  Service & Characteristic -- ok
#define SERVICE_UUID_MEDIA_CONTROL                            0x1848 // Media Control Service
#define SERVICE_UUID_GENERIC_MEDIA_CONTROL                    0x1849 // Generic Media Control Service
#define CHARACTERISTIC_UUID_MCS_PLAYER_NAME                   0x2B93 // Media Player Name
#define CHARACTERISTIC_UUID_MCS_ICON_OBJECT_ID                0x2B94 // Media Player Icon Object ID
#define CHARACTERISTIC_UUID_MCS_ICON_URL                      0x2B95 // Media Player Icon URL
#define CHARACTERISTIC_UUID_MCS_TRACK_CHANGED                 0x2B96 // Track Changed
#define CHARACTERISTIC_UUID_MCS_TRACK_TITLE                   0x2B97 // Track Title
#define CHARACTERISTIC_UUID_MCS_TRACK_DURATION                0x2B98 // Track Duration
#define CHARACTERISTIC_UUID_MCS_TRACK_POSITION                0x2B99 // Track Position
#define CHARACTERISTIC_UUID_MCS_PLAYBACK_SPEED                0x2B9A // Playback Speed
#define CHARACTERISTIC_UUID_MCS_SEEKING_SPEED                 0x2B9B // Seeking Speed
#define CHARACTERISTIC_UUID_MCS_SEGMENTS_OBJECT_ID            0x2B9C // Current Track Segments Object ID
#define CHARACTERISTIC_UUID_MCS_CURRENT_OBJECT_ID             0x2B9D // Current Track Object ID
#define CHARACTERISTIC_UUID_MCS_NEXT_OBJECT_ID                0x2B9E // Next Track Object ID
#define CHARACTERISTIC_UUID_MCS_PARENT_GOUP_OBJECT_ID         0x2B9F // Parent Group Object ID
#define CHARACTERISTIC_UUID_MCS_CURRENT_GOUP_OBJECT_ID        0x2BA0 // Current Group Object ID
#define CHARACTERISTIC_UUID_MCS_PLAYING_ORDER                 0x2BA1 // Playing Order
#define CHARACTERISTIC_UUID_MCS_PLAYING_ORDER_SUPPEORTED      0x2BA2 // Playing Order Supported
#define CHARACTERISTIC_UUID_MCS_MEDIA_STATE                   0x2BA3 // Media State
#define CHARACTERISTIC_UUID_MCS_MEDIA_CONTROL_POINT           0x2BA4 // Media Control Point
#define CHARACTERISTIC_UUID_MCS_OPCODES_SUPPORTED             0x2BA5 // Media Control Point Opcodes Supported
#define CHARACTERISTIC_UUID_MCS_SEARCH_RESULTS_OBJECT_ID      0x2BA6 // Search Results Object ID
#define CHARACTERISTIC_UUID_MCS_SEARCH_CONTROL_POINT          0x2BA7 // Search Control Point
#define CHARACTERISTIC_UUID_MCS_MEDIA_PLAYER_ICON_OBJECT_TYPE 0x2BA9 // Player icon object type
#define CHARACTERISTIC_UUID_MCS_TRACK_SEGMENTS_OBJECT_TYPE    0x2BAA // Track segments object type
#define CHARACTERISTIC_UUID_MCS_TRACK_OBJECT_TYPE             0x2BAB // Track object type
#define CHARACTERISTIC_UUID_MCS_GROUP_OBJECT_TYPE             0x2BAC // Group object type
#define CHARACTERISTIC_UUID_MCS_CONTENT_CONTROL_ID            0x2BBA // Content Control ID (CCID)
// CCS  Service & Characteristic
#define SERVICE_UUID_CALL_CONTROL                             0x2430


// TBS  Service & Characteristic -- ok
#define SERVICE_UUID_TELEPHONE_BEARER                        0x184B //Telephone Bearer Service
#define SERVICE_UUID_GENERIC_TELEPHONE_BEARER                0x184C //Generic Telephone Bearer Service
#define CHARACTERISTIC_UUID_TBS_PROVIDER_NAME                0x2BB3 //M Mandatory:Read, Notify;   Optional:
#define CHARACTERISTIC_UUID_TBS_CALLER_IDENTIFIER            0x2BB4 //M Mandatory:Read;   Optional:
#define CHARACTERISTIC_UUID_TBS_TECHNOLOGY                   0x2BB5 //M Mandatory:Read, Notify;   Optional:
#define CHARACTERISTIC_UUID_TBS_URI_LIST                     0x2BB6 //M Mandatory:Read;   Optional:Notify
#define CHARACTERISTIC_UUID_TBS_SIGNAL_STRENGTH              0x2BB7 //O Mandatory:Read, Notify;   Optional:
#define CHARACTERISTIC_UUID_TBS_SIGNAL_REPORT_INTERVAL       0x2BB8 //O Mandatory:Read, Write, Write Without Response;   Optional:
#define CHARACTERISTIC_UUID_TBS_CURRENT_CALL_LIST            0x2BB9 //M Mandatory:Read, Notify;   Optional:
#define CHARACTERISTIC_UUID_TBS_CCID                         0x2BBA //M Mandatory:Read;   Optional:
#define CHARACTERISTIC_UUID_TBS_STATUS_FLAGS                 0x2BBB //M Mandatory:Read, Notify;   Optional:
#define CHARACTERISTIC_UUID_TBS_INCOMING_CALL_URI            0x2BBC //O Mandatory:Read, Notify;   Optional:
#define CHARACTERISTIC_UUID_TBS_CALL_STATE                   0x2BBD //M Mandatory:Read, Notify;   Optional:
#define CHARACTERISTIC_UUID_TBS_CALL_CONTROL_POINT           0x2BBE //M Mandatory:Write, Write Without Response, Notify;   Optional:
#define CHARACTERISTIC_UUID_TBS_CCP_OPTIONAL_OPCODES         0x2BBF //M Mandatory:Read;   Optional:
#define CHARACTERISTIC_UUID_TBS_TERMINATIONO_REASON          0x2BC0 //M Mandatory:Notify;   Optional:
#define CHARACTERISTIC_UUID_TBS_INCOMING_CALL                0x2BC1 //M Mandatory:Read, Notify;   Optional:
#define CHARACTERISTIC_UUID_TBS_CALL_FREIENDLY_NAME          0x2BC2 //O Mandatory:Read, Notify;   Optional:
#endif

// OTS  Service & Characteristic -- ok
#define SERVICE_UUID_OBJECT_TRANSFER                         0x1825 //Object Transfer Service
#define CHARACTERISTIC_UUID_OTS_FEATURE                      0x2ABD //M Mandatory:Read; Optional:
#define CHARACTERISTIC_UUID_OTS_OBJECT_NAME                  0x2ABE //M Mandatory:Read; Optional:Write
#define CHARACTERISTIC_UUID_OTS_OBJECT_TYPE                  0x2ABF //M Mandatory:Read; Optional:
#define CHARACTERISTIC_UUID_OTS_OBJECT_SIZE                  0x2AC0 //M Mandatory:Read; Optional:
#define CHARACTERISTIC_UUID_OTS_OBJECT_FIRST_CREATED         0x2AC1 //O Mandatory:Read; Optional:Write
#define CHARACTERISTIC_UUID_OTS_OBJECT_LAST_CREATED          0x2AC2 //O Mandatory:Read; Optional:Write
#define CHARACTERISTIC_UUID_OTS_OBJECT_ID                    0x2AC3 //O Mandatory:Read; Optional:
#define CHARACTERISTIC_UUID_OTS_OBJECT_PROPERTIES            0x2AC4 //M Mandatory:Read; Optional:Write
#define CHARACTERISTIC_UUID_OTS_OBJECT_ACTION_CONTROL_POINT  0x2AC5 //M Mandatory:Write,Indicate; Optional:
#define CHARACTERISTIC_UUID_OTS_OBJECT_LIST_CONTROL_POINT    0x2AC6 //O Mandatory:Write,Indicate; Optional:
#define CHARACTERISTIC_UUID_OTS_OBJECT_LIST_FILTER           0x2AC7 //O Mandatory:Read,Write; Optional:
#define CHARACTERISTIC_UUID_OTS_OBJECT_CHANGED               0x2AC8 //O Mandatory:Indicate; Optional:


///BAP
////////////////////////////////////////////////////////////////////

// Context Type
#define BLC_AUDIO_CONTEXT_TYPE_UNSPECIFIED         BIT(0) // Unspecified. Matches any audio content.
#define BLC_AUDIO_CONTEXT_TYPE_CONVERSATIONAL      BIT(1) // Conversation between humans as, for example, in telephony or video calls.
#define BLC_AUDIO_CONTEXT_TYPE_MEDIA               BIT(2) // Media as, for example, in music, public radio, podcast or video soundtrack.
#define BLC_AUDIO_CONTEXT_TYPE_INSTRUCTIONAL       BIT(3) // Instructional audio as, for example, in navigation, traffic announcements or user guidance
#define BLC_AUDIO_CONTEXT_TYPE_ATTENTION_SEEKING   BIT(4) // Attention seeking audio as, for example, in beeps signalling arrival of a message or keyboard clicks. 
#define BLC_AUDIO_CONTEXT_TYPE_IMMEDIATE_ALERT     BIT(5) // Immediate alerts as, for example, in a low battery alarm, timer expiry or alarm clock.
#define BLC_AUDIO_CONTEXT_TYPE_MAN_MACHINE         BIT(6) // Man machine communication as, for example, with voice recognition or virtual assistant.
#define BLC_AUDIO_CONTEXT_TYPE_EMERGENCY_ALERT     BIT(7) // Emergency alerts as, for example, with fire alarms or other urgent alerts.
#define BLC_AUDIO_CONTEXT_TYPE_RINGTONE            BIT(8) // Ringtone as in a call alert.
#define BLC_AUDIO_CONTEXT_TYPE_TV                  BIT(9) // Audio associated with a television program and/or with metadata conforming to the Bluetooth Broadcast TV profile.

// Codec ID
#define BLC_AUDIO_CODECID_LC3                      0x00000001

// Audio Codec_Specific_Capabilties parameters
#define BLC_AUDIO_CAPTYPE_SUP_SAMPLE_FREQUENCY     0x01 // Supported_Sampling_Frequencies
#define BLC_AUDIO_CAPTYPE_SUP_FRAME_DURATION       0x02 // Supported_Frame_Durations
#define BLC_AUDIO_CAPTYPE_SUP_CHANNELS_COUNTS      0x03 // Audio_Channel_Counts
#define BLC_AUDIO_CAPTYPE_SUP_FRAME_OCTETS	       0x04 // Supported_Octets_Per_Codec_Frame


// Audio Codec_Specific_Configuration parameters
#define BLC_AUDIO_CAPTYPE_CFG_SAMPLE_FREQUENCY     0x01 // Sampling_Freqeuncy
#define BLC_AUDIO_CAPTYPE_CFG_FRAME_DURATION       0x02 // 
#define BLC_AUDIO_CAPTYPE_CFG_CHANNELS_ALLOCATION  0x03 // Audio_Channel_Allocation
#define BLC_AUDIO_CAPTYPE_CFG_FRAME_OCTETS	       0x04 // 

// Audio Meta Type
#define BLC_AUDIO_METATYPE_PREFERRED_CONTEXTS      0x01 // Preferred_Audio_Contexts
#define BLC_AUDIO_METATYPE_STREAMING_CONTEXTS      0x02 // Streaming_Audio_Contexts

// Audio Direction
#define BLC_AUDIO_DIRECTION_SINK                   0x01 //Server is Audio Sink
#define BLC_AUDIO_DIRECTION_SOURCE                 0x02 //Server is Audio Source

// Audio Support Location (bitfield, for PACS or ASCS)
#define BLC_AUDIO_LOCATION_FLAG_FL                 BIT(0)  // Front Left
#define BLC_AUDIO_LOCATION_FLAG_FR                 BIT(1)  // Front Right
#define BLC_AUDIO_LOCATION_FLAG_FC                 BIT(2)  // Front Center
#define BLC_AUDIO_LOCATION_FLAG_LFE1               BIT(3)  // Low Frequency Effects 1
#define BLC_AUDIO_LOCATION_FLAG_BL                 BIT(4)  // Back Left 
#define BLC_AUDIO_LOCATION_FLAG_BR                 BIT(5)  // Back Right
#define BLC_AUDIO_LOCATION_FLAG_FLc                BIT(6)  // Front Left of Center
#define BLC_AUDIO_LOCATION_FLAG_FRc                BIT(7)  // Front Right of Center
#define BLC_AUDIO_LOCATION_FLAG_BC                 BIT(8)  // Back Center
#define BLC_AUDIO_LOCATION_FLAG_LFE2               BIT(9)  // Low Frequency Effects 2
#define BLC_AUDIO_LOCATION_FLAG_SiL                BIT(10) // Side Left 
#define BLC_AUDIO_LOCATION_FLAG_SiR                BIT(11) // Side Right
#define BLC_AUDIO_LOCATION_FLAG_TpFL               BIT(12) // Top Front Left
#define BLC_AUDIO_LOCATION_FLAG_TpFR               BIT(13) // Top Front Right
#define BLC_AUDIO_LOCATION_FLAG_TpFC               BIT(14) // Top Front Center
#define BLC_AUDIO_LOCATION_FLAG_TpC                BIT(15) // Top Center
#define BLC_AUDIO_LOCATION_FLAG_TpBL               BIT(16) // Top Back Left
#define BLC_AUDIO_LOCATION_FLAG_TpBR               BIT(17) // Top Back Right
#define BLC_AUDIO_LOCATION_FLAG_TpSiL              BIT(18) // Top Side Left
#define BLC_AUDIO_LOCATION_FLAG_TpSiR              BIT(19) // Top Side Right
#define BLC_AUDIO_LOCATION_FLAG_TpBC               BIT(20) // Top Back Center
#define BLC_AUDIO_LOCATION_FLAG_BtFC               BIT(21) // Bottom Front Center
#define BLC_AUDIO_LOCATION_FLAG_BtFL               BIT(22) // Bottom Front Left
#define BLC_AUDIO_LOCATION_FLAG_BtFR               BIT(23) // Bottom Front Right
#define BLC_AUDIO_LOCATION_FLAG_FLw                BIT(24) // Front Left Wide
#define BLC_AUDIO_LOCATION_FLAG_FRw                BIT(25) // Front Right Wide
#define BLC_AUDIO_LOCATION_FLAG_LS                 BIT(26) // Left Surround
#define BLC_AUDIO_LOCATION_FLAG_RS                 BIT(27) // Right Surround

// Audio Support Frame Frequency (bitfield, for PACS)
#define BLC_AUDIO_FREQUENCY_FLAG_8000              BIT(0) // 8000 Hz
#define BLC_AUDIO_FREQUENCY_FLAG_16000             BIT(1) // 16000 Hz
#define BLC_AUDIO_FREQUENCY_FLAG_24000             BIT(2) // 24000 Hz
#define BLC_AUDIO_FREQUENCY_FLAG_32000             BIT(3) // 32000 Hz
#define BLC_AUDIO_FREQUENCY_FLAG_44100             BIT(4) // 44100 Hz
#define BLC_AUDIO_FREQUENCY_FLAG_48000             BIT(5) // 48000 Hz
// Audio Frame Frequency (numbric, for ASCS)
#define BLC_AUDIO_FREQUENCY_CFG_8000               1 // 8000 Hz
#define BLC_AUDIO_FREQUENCY_CFG_16000              2 // 16000 Hz
#define BLC_AUDIO_FREQUENCY_CFG_24000              3 // 24000 Hz
#define BLC_AUDIO_FREQUENCY_CFG_32000              4 // 32000 Hz
#define BLC_AUDIO_FREQUENCY_CFG_44100              5 // 44100 Hz
#define BLC_AUDIO_FREQUENCY_CFG_48000              6 // 48000 Hz

// Audio Support Frame Duration (bitfield, for PACS)
#define BLC_AUDIO_DURATION_FLAG_7_5                BIT(0)  //
#define BLC_AUDIO_DURATION_FLAG_10                 BIT(1)
#define BLC_AUDIO_DURATION_FLAG_7_5_PERFERRED      BIT(4)
#define BLC_AUDIO_DURATION_FLAG_10_PERFERRED       BIT(5)
// Audio Frame Duration (numbric, for ASCS)
#define BLC_AUDIO_DURATION_CFG_7_5                 0  //
#define BLC_AUDIO_DURATION_CFG_10                  1


// Audio Channel
#define BLC_AUDIO_CHANNEL_COUNTS_1                 BIT(0)
#define BLC_AUDIO_CHANNEL_COUNTS_2                 BIT(1)
#define BLC_AUDIO_CHANNEL_COUNTS_3                 BIT(2)
#define BLC_AUDIO_CHANNEL_COUNTS_4                 BIT(3)
#define BLC_AUDIO_CHANNEL_COUNTS_5                 BIT(4)
#define BLC_AUDIO_CHANNEL_COUNTS_6                 BIT(5)
#define BLC_AUDIO_CHANNEL_COUNTS_7                 BIT(6)
#define BLC_AUDIO_CHANNEL_COUNTS_8                 BIT(7)

// Audio PHY (bitfield, for ASCS)
#define BLC_AUDIO_PHY_FLAG_1M                      BIT(0) //LE 1M PHY preferred
#define BLC_AUDIO_PHY_FLAG_2M                      BIT(1) //LE 2M PHY preferred
#define BLC_AUDIO_PHY_FLAG_CODED                   BIT(2) //LE Coded PHY preferred

// Audio Framing (bitfield, for ASCS)
#define BLC_AUDIO_FRAMING_UNFRAMED                 0x00 //Unframed ISOAL PDUs preferred
#define BLC_AUDIO_FRAMING_FRAMED                   0x01 //framed ISOAL PDUs preferred


// The Value Of Mute field
#define BLC_AUDIO_MUTE_NOT_MUTED                   0x00
#define BLC_AUDIO_MUTE_MUTED                       0x01
#define BLC_AUDIO_MUTE_DISABLED                    0x02

// Gain Mode field
#define BLC_AUDIO_GAIN_MODE_MANUAL_ONLY            0x00
#define BLC_AUDIO_GAIN_MODE_AUTOMATIC_ONLY         0x01
#define BLC_AUDIO_GAIN_MODE_MANUAL                 0x02
#define BLC_AUDIO_GAIN_MODE_AUTOMATIC              0x03

// AICS Input State
#define BLC_AUDIO_INPUT_STATE_INACTIVE             0x00
#define BLC_AUDIO_INPUT_STATE_ACTIVE               0x01

// TODO: Input Type TBD
// AICS Input Type
#define BLC_AUDIO_INPUT_TYPE_LOCAL                 0x01
#define BLC_AUDIO_INPUT_TYPE_ISOCHRONOUS_STREAM    0x02
#define BLC_AUDIO_INPUT_TYPE_ANALOG_CONNECTOR      0x03
#define BLC_AUDIO_INPUT_TYPE_DIGITAL_CONNECTOR     0x04


// ASCS
#define BLC_AUDIO_CLOCK_ACCURACY_251_500PPM    0x00
#define BLC_AUDIO_CLOCK_ACCURACY_151_250PPM    0x01
#define BLC_AUDIO_CLOCK_ACCURACY_101_150PPM    0x02
#define BLC_AUDIO_CLOCK_ACCURACY_76_100PPM     0x03
#define BLC_AUDIO_CLOCK_ACCURACY_51_75PPM      0x04
#define BLC_AUDIO_CLOCK_ACCURACY_31_50PPM      0x05
#define BLC_AUDIO_CLOCK_ACCURACY_21_30PPM      0x06
#define BLC_AUDIO_CLOCK_ACCURACY_0_20PPM       0x07
#define BLC_AUDIO_CLOCK_ACCURACY_DEFAULT       BLC_AUDIO_CLOCK_ACCURACY_76_100PPM
// ASCS
#define BLC_AUDIO_PACKING_SEQUENTIAL           0x00
#define BLC_AUDIO_PACKING_INTERLEAVED          0x01
#define BLC_AUDIO_PACKING_DEFAULT              BLC_AUDIO_PACKING_SEQUENTIAL



// MCP  Media State
typedef enum{
	BLC_AUDIO_MCP_MEDIA_STATE_INACTIVE  = 0x00,
	BLC_AUDIO_MCP_MEDIA_STATE_PLAYING   = 0x01,
	BLC_AUDIO_MCP_MEDIA_STATE_PAUSED    = 0x02,
	BLC_AUDIO_MCP_MEDIA_STATE_SEEKING   = 0x03,
}BLC_AUDIO_MCP_MEDIA_STATE_ENUM;
// MCP  Supported Opcode
typedef enum{
	BLC_AUDIO_MCP_CP_SUPPORTED_OPCODE_PLAY				= 0x00000001,
	BLC_AUDIO_MCP_CP_SUPPORTED_OPCODE_PAUSE 			= 0x00000002,
	BLC_AUDIO_MCP_CP_SUPPORTED_OPCODE_FAST_REWIND		= 0x00000004,
	BLC_AUDIO_MCP_CP_SUPPORTED_OPCODE_FAST_FORWARD		= 0x00000008,
	BLC_AUDIO_MCP_CP_SUPPORTED_OPCODE_STOP				= 0x00000010,
	BLC_AUDIO_MCP_CP_SUPPORTED_OPCODE_MOVE_RELATIVE 	= 0x00000020,
	BLC_AUDIO_MCP_CP_SUPPORTED_OPCODE_PREVIOUS_SEGMENT	= 0x00000040,
	BLC_AUDIO_MCP_CP_SUPPORTED_OPCODE_NEXT_SEGMENT		= 0x00000080,
	BLC_AUDIO_MCP_CP_SUPPORTED_OPCODE_FIRST_SEGMENT 	= 0x00000100,
	BLC_AUDIO_MCP_CP_SUPPORTED_OPCODE_LAST_SEGMENT		= 0x00000200,
	BLC_AUDIO_MCP_CP_SUPPORTED_OPCODE_GOTO_SEGMENT		= 0x00000400,
	BLC_AUDIO_MCP_CP_SUPPORTED_OPCODE_PREVIOUS_TRACK	= 0x00000800,
	BLC_AUDIO_MCP_CP_SUPPORTED_OPCODE_NEXT_TRACK		= 0x00001000,
	BLC_AUDIO_MCP_CP_SUPPORTED_OPCODE_FIRST_TRACK		= 0x00002000,
	BLC_AUDIO_MCP_CP_SUPPORTED_OPCODE_LAST_TRACK		= 0x00004000,
	BLC_AUDIO_MCP_CP_SUPPORTED_OPCODE_GOTO_TRACK		= 0x00008000,
	BLC_AUDIO_MCP_CP_SUPPORTED_OPCODE_PRIVOUS_GROUP 	= 0x00010000,
	BLC_AUDIO_MCP_CP_SUPPORTED_OPCODE_NEXT_GROUP		= 0x00020000,
	BLC_AUDIO_MCP_CP_SUPPORTED_OPCODE_FIRST_FROUP		= 0x00040000,
	BLC_AUDIO_MCP_CP_SUPPORTED_OPCODE_LAST_GROUP		= 0x00080000,
	BLC_AUDIO_MCP_CP_SUPPORTED_OPCODE_GOTO_GROUP		= 0x00100000,
}BLC_AUDIO_MCP_CP_SUPPORTED_OPCODE_ENUM;
// MCP   Opcode for Control Point
typedef enum{
	BLC_AUDIO_MCP_OPCODE_NONE			   = 0x00,
	BLC_AUDIO_MCP_OPCODE_PLAY			   = 0x01,
	BLC_AUDIO_MCP_OPCODE_PAUSE			   = 0x02,
	BLC_AUDIO_MCP_OPCODE_FAST_REWIND	   = 0x03,
	BLC_AUDIO_MCP_OPCODE_FAST_FORWARD	   = 0x04,
	BLC_AUDIO_MCP_OPCODE_STOP			   = 0x05,
	BLC_AUDIO_MCP_OPCODE_MOVE_RELATIVE	   = 0x10,
	BLC_AUDIO_MCP_OPCODE_PREVIOUS_SEGMENT  = 0x20,
	BLC_AUDIO_MCP_OPCODE_NEXT_SEGMENT	   = 0x21,
	BLC_AUDIO_MCP_OPCODE_FIRST_SEGMENT	   = 0x22,
	BLC_AUDIO_MCP_OPCODE_LAST_SEGMENT	   = 0x23,
	BLC_AUDIO_MCP_OPCODE_GOTO_SEGMENT	   = 0x24,
	BLC_AUDIO_MCP_OPCODE_PREVIOUS_TRACK    = 0x30,
	BLC_AUDIO_MCP_OPCODE_NEXT_TRACK 	   = 0x31,
	BLC_AUDIO_MCP_OPCODE_FIRST_TRACK	   = 0x32,
	BLC_AUDIO_MCP_OPCODE_LAST_TRACK 	   = 0x33,
	BLC_AUDIO_MCP_OPCODE_GOTO_TRACK 	   = 0x34,
	BLC_AUDIO_MCP_OPCODE_PREVIOUS_GROUP    = 0x40,
	BLC_AUDIO_MCP_OPCODE_NEXT_GROUP 	   = 0x41,
	BLC_AUDIO_MCP_OPCODE_FIRST_GROUP	   = 0x42,
	BLC_AUDIO_MCP_OPCODE_LAST_GROUP 	   = 0x43,
	BLC_AUDIO_MCP_OPCODE_GOTO_GROUP 	   = 0x44,
}BLC_AUDIO_MCP_OPCODE_ENUM;



// TBS   Opcode for Control Point
typedef enum{
	BLC_AUDIO_TBP_OPCODE_ACCEPT         = 0x00,
	BLC_AUDIO_TBP_OPCODE_TERMINATE      = 0x01,
	BLC_AUDIO_TBP_OPCODE_LOCAL_HOLD     = 0x02,
	BLC_AUDIO_TBP_OPCODE_LOCAL_RETRIEVE = 0x03,
	BLC_AUDIO_TBP_OPCODE_ORIGINATE      = 0x04,
	BLC_AUDIO_TBP_OPCODE_JOIN           = 0x05,
}BLC_AUDIO_TBP_OPCODE_ENUM;
// TBS   Call State
typedef enum{
	BLC_AUDIO_TBP_CALL_STATE_INCOMING           = 0x00,
	BLC_AUDIO_TBP_CALL_STATE_DIALING            = 0x01,
	BLC_AUDIO_TBP_CALL_STATE_ALERTING           = 0x02,
	BLC_AUDIO_TBP_CALL_STATE_ACTIVE             = 0x03,
	BLC_AUDIO_TBP_CALL_STATE_LOCAL_HELD         = 0x04,
	BLC_AUDIO_TBP_CALL_STATE_REMOTE_HELD        = 0x05,
	BLC_AUDIO_TBP_CALL_STATE_LOCAL_REMOTE_HELD  = 0x06,
}BLC_AUDIO_TBP_CALL_STATE_ENUM;
// TBS   Call Flags
typedef enum{
	BLC_AUDIO_TBP_CALL_FLAG_OUTCOMING               = BIT(0), //Incoming/Outgoing: 0=Call is an incoming call; 1=Call is an outgoing call
	BLC_AUDIO_TBP_CALL_FLAG_INCOMING                = 0x00,
	BLC_AUDIO_TBP_CALL_FLAG_WITHHELD_BY_SERVER      = BIT(1), //Information withheld by server: 0=Not withheld; 1=Withheld
	BLC_AUDIO_TBP_CALL_FLAG_NOT_WITHHELD_BY_SERVER  = 0x00,
	BLC_AUDIO_TBP_CALL_FLAG_WITHHELD_BY_NETWORK     = BIT(2), //Information withheld by network: 0 = Provided by network; 1 = Withheld by network
	BLC_AUDIO_TBP_CALL_FLAG_PROVIDED_BY_NETWORK     = 0x00,
}BLC_AUDIO_TBP_CALL_FLAGS_ENUM;
// TBS	 Status Flags
typedef enum{
	BLC_AUDIO_TBP_STATUS_FLAG_INBAND_RINGTONE = BIT(0), //0=inband ringtone disabled; 1=inband ringtone enabled
	BLC_AUDIO_TBP_STATUS_FLAG_SILENT_MODE     = BIT(1), //0 = Server is not in silent mode; 1 = Server is in silent mode
}BLC_AUDIO_TBP_STATUS_FLAGS_ENUM;




#endif //#if (BLC_AUDIO_PROFILE_EN)

#endif //_AUDIO_DEFINE_H_
