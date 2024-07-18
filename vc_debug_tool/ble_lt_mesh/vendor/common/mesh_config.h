/********************************************************************************************************
 * @file	mesh_config.h
 *
 * @brief	for TLSR chips
 *
 * @author	telink
 * @date	Sep. 30, 2010
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
#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

#include "../user_app/user_app_config.h"

//------------hardware parameters
// HCI Select
#define PROXY_HCI_GATT					1
#define PROXY_HCI_USB					2
#define PROXY_HCI_SEL					PROXY_HCI_GATT

#define GATEWAY_MODEL_PLUS_EN   ((!WIN32) && __PROJECT_MESH_PRO__ && (CHIP_TYPE != CHIP_TYPE_8269))

#if (__TLSR_RISCV_EN__)
#define MESH_IRONMAN_AP2T31F80_EN		0
#endif

#if (PROXY_HCI_SEL == PROXY_HCI_GATT)
    #if (__DEBUG_MESH_DONGLE_IN_VC__)
#define DEBUG_MESH_DONGLE_IN_VC_EN		1		// must 1
    #else
        #if WIN32
#define DEBUG_MESH_DONGLE_IN_VC_EN		0
        #else
#define DEBUG_MESH_DONGLE_IN_VC_EN		0
        #endif
    #endif
#else
#define DEBUG_MESH_DONGLE_IN_VC_EN		0		// must 0
#endif

#ifndef MESH_RX_TEST
#define MESH_RX_TEST					(0)
#endif

#define DEBUG_PUBLISH_REDUCE_COLLISION_TEST_EN	0

#define DEBUG_CFG_CMD_USE_AK_WHEN_GROUP_EN2		(0 || DEBUG_PUBLISH_REDUCE_COLLISION_TEST_EN || MESH_RX_TEST) // used only by telink internal
#define DEBUG_CFG_CMD_GROUP_AK_EN				(0) // not for user
#define DEBUG_CFG_CMD_GROUP_USE_AK(addr)    	((DEBUG_CFG_CMD_GROUP_AK_EN || DEBUG_CFG_CMD_USE_AK_WHEN_GROUP_EN2) && (addr & 0x8000))
#if (!WIN32)
#define DEBUG_LOG_SETTING_DEVELOP_MODE_EN		0 //
#endif

#define SHOW_VC_SELF_NO_NW_ENC			1
#define SHOW_VC_SELF_NW_ENC				2	// can not send reliable cmd with segment, such as netkey add,...

#if (WIN32 && (0 == DEBUG_MESH_DONGLE_IN_VC_EN))
#define NODE_CAN_SEND_ADV_FLAG			0
#define DEBUG_SHOW_VC_SELF_EN			0 // SHOW_VC_SELF_NO_NW_ENC // SHOW_VC_SELF_NW_ENC
#define VC_CHECK_NEXT_SEGMENT_EN		1		// must 1
#else
#define NODE_CAN_SEND_ADV_FLAG			1		// must 1
#define DEBUG_SHOW_VC_SELF_EN			0		// must 0
#define VC_CHECK_NEXT_SEGMENT_EN		0		// must 0
#endif

#define MESH_DLE_MODE_NONE              0
#define MESH_DLE_MODE_GATT              1       // only GATT, mesh unsegment size will still be 11 bytes.
#define MESH_DLE_MODE_EXTEND_BEAR       2       // mesh unsegment size will depend on DLE length.
//------------ project define-------------
#define PROJECT_VC_DONGLE 	1
#define PROJECT_VC_MESH 	2
#define PROJECT_VC_MAX 		10	// VC project max
#define PROJECT_FW_MESH		11	//FW start from 11

#if WIN32
#define PROJECT_SEL			PROJECT_VC_DONGLE // PROJECT_VC_MESH // 
#else
#define PROJECT_SEL			PROJECT_FW_MESH
#endif

#define IS_VC_PROJECT			(PROJECT_SEL <= PROJECT_VC_MAX)
#define IS_VC_PROJECT_MASTER	(IS_VC_PROJECT && (!DEBUG_MESH_DONGLE_IN_VC_EN))

#define PROXY_GATT_WITH_HEAD ((DEBUG_MESH_DONGLE_IN_VC_EN)||(PROXY_HCI_SEL == PROXY_HCI_USB))
#if WIN32
#define 	__PROJECT_MESH_PRO__	 	1 // "PRO" means provisioner, will be set as 1 for 8258_mesh_gw (gateway) and sig_mesh_tool.exe.
#define FAST_PROVISION_ENABLE		 	1
#else
#define FAST_PROVISION_ENABLE		 	0
#endif

#if (__PROJECT_MESH_PRO__ || __PROJECT_MESH_GW_NODE__)
/*
 * GW SMART PROVISION REMOTE CONTROL PM EN: a product which is a provisioner without "sig_mesh_tool.exe",
 * and has keyboard to send control message, and is at low power state at most of time.
 */
#define GW_SMART_PROVISION_REMOTE_CONTROL_PM_EN		0
	#if GW_SMART_PROVISION_REMOTE_CONTROL_PM_EN
#define SMART_PROVISION_ENABLE		 	1
	#else
#define SMART_PROVISION_ENABLE		 	0 // just for gateway, and node must open PROVISION_FLOW_SIMPLE_EN
	#endif
#endif

#define BEACON_ENABLE					0
//ms, should be multiple of 10
#define BEACON_INTERVAL					100



#define ATT_TAB_SWITCH_ENABLE 			1
#if(__PROJECT_MESH__)
#define ATT_REPLACE_PROXY_SERVICE_EN	0 // 1:0x7fdd will not existed in att
#endif

#if WIN32
#define TESTCASE_FLAG_ENABLE 		0	// must 0
#define PTS_TEST_EN                 0   // must 0
#else
#define TESTCASE_FLAG_ENABLE 		0
    #if (0 == TESTCASE_FLAG_ENABLE)
#define PTS_TEST_EN                 0
    #else
#define PTS_TEST_EN                 1
    #endif
#define DEBUG_EVB_EN 		        0   // just telink internal used, don't modity
#endif

#define PTS_TEST_OTA_EN             (PTS_TEST_EN || 0)
#define PTS_TEST_KEY_REFRESH_EN		(PTS_TEST_EN || 0)

#define IV_UPDATE_TEST_EN		 	0// just for test

#if __PROJECT_MESH_LPN__
#define LPN_VENDOR_SENSOR_EN        0
#else
#define LPN_VENDOR_SENSOR_EN        0
#endif

#if (__PROJECT_MESH__)
#define GATT_LPN_EN					0   // only mesh and lpn project can enable
#elif(__PROJECT_MESH_LPN__)
#define GATT_LPN_EN					0   // only mesh and lpn project can enable
#endif

#if (__PROJECT_MESH_SWITCH__)
#define SWITCH_PB_ADV_EN			1   // switch support pb adv, time ADV_TIMEOUT
#endif

//------------ mesh audio config -------------
#if __TLSR_RISCV_EN__
	#if __PROJECT_MESH__ || __PROJECT_MESH_PRO__ 	// only for B91 mesh and gateway project
#define AUDIO_MESH_EN					0
		#if AUDIO_MESH_EN
#define CODEC_ALGORITHM_SBC             0
#define CODEC_ALGORITHM_LC3	            1

#define AUDIO_MESH_MULTY_NODES_TX_EN	0	// only support audio LC3 + 8k sample rate now for multy nodes mode.
		#if AUDIO_MESH_MULTY_NODES_TX_EN
#define AUDIO_DATA_NO_TX_WHEN_SILENCE_EN		1
			#if AUDIO_DATA_NO_TX_WHEN_SILENCE_EN
#define AUDIO_RX_NODES_MAX				5	// define a node that can receive audio from how many nodes.
			#else
#define AUDIO_RX_NODES_MAX				3	// define a node that can receive audio from how many nodes.
			#endif
#define CODEC_ALGORITHM_SEL 			CODEC_ALGORITHM_LC3
		#else
#define AUDIO_DATA_NO_TX_WHEN_SILENCE_EN		0
#define CODEC_ALGORITHM_SEL 			CODEC_ALGORITHM_SBC
#define AUDIO_RX_NODES_MAX				1	// must 1
		#endif

#define PAIR_PROVISION_ENABLE		 	0 	// provision without app and gateway. // usually for audio mesh
#define AUDIO_I2S_EN					0   // 0 means analog sampling, 1 means using I2S sampling

/**
 * @brief 		enum value: sample rate in RF packet
 */
#define RF_SEND_AUDIO_SAMPLE_RATE_8K	1
#define RF_SEND_AUDIO_SAMPLE_RATE_16K	2

		#if(CODEC_ALGORITHM_SEL == CODEC_ALGORITHM_SBC)
#define RF_SEND_AUDIO_SAMPLE_RATE_SEL	RF_SEND_AUDIO_SAMPLE_RATE_8K	// must 8k for SBC
#define MIC_SAMPLES_PER_PACKET      	240		
#define SBC_FRAME_SAMPLES          		(MIC_SAMPLES_PER_PACKET/2)
#define SBC_BIT_POOL                    20 // 26:compression ratio is 4.53. 20:compression ratio is 5.71. 17:compression ratio is 6.67.  12:compression ratio is 8.89.

#define SBC_BLOCK_NUM                   (SBC_FRAME_SAMPLES / 8)
#define SBC_FRAME_SIZE                  ((SBC_BIT_POOL * SBC_BLOCK_NUM + 7) / 8 + 4) // for 8k16bit the Compression ratio is SBC_FRAME_SAMPLES*2/SBC_FRAME_SIZE = 8.89
#define MIC_ENC_SIZE					SBC_FRAME_SIZE
		#elif(CODEC_ALGORITHM_SEL == CODEC_ALGORITHM_LC3)
#define RF_SEND_AUDIO_SAMPLE_RATE_SEL	RF_SEND_AUDIO_SAMPLE_RATE_8K 	// Supports 16k and 8k
#define MIC_SAMPLES_PER_PACKET      	480 // can't change
#define LC3_BIT_RATE					58400 // base on 480 mic sample which is 10ms frame of 48k16bit // for 8k16bit is (58400/(48/8)) = 9733, then Compression ratio is 13.1.
#define LC3_ENC_SIZE					((LC3_BIT_RATE*(MIC_SAMPLES_PER_PACKET/120)+3199)/3200) // default is 73
#define MIC_ENC_SIZE					LC3_ENC_SIZE
		#endif
		
#define AUDIO_RX_TIMEOUT				1000	//unit:ms

#define MESH_AUDIO_RESAMPLE_EN			(RF_SEND_AUDIO_SAMPLE_RATE_SEL != RF_SEND_AUDIO_SAMPLE_RATE_16K)	// because SPEEX_SAMPLERATE of ENC is fixed 16k.
#define MIC_NUM_MESH_TX					((ACCESS_NO_MIC_LEN_MAX_UNSEG + CONST_DELTA_EXTEND_AND_NORMAL - 3 - 3)/MIC_ENC_SIZE) // MIC_NUM_MESH_TX is 3 for LC3 // param details: 3:vendor opcode size, 3:OFFSETOF(vd_audio_t, data) 
		#endif
	#endif
#endif
//------------ mesh audio config end -------------


//------------ mesh config-------------
#define MD_CFG_CLIENT_EN            (__PROJECT_MESH_PRO__ || __PROJECT_MESH_GW_NODE__ || TESTCASE_FLAG_ENABLE)   // don't modify
#define RELIABLE_CMD_EN             (__PROJECT_MESH_PRO__ || __PROJECT_MESH_GW_NODE__)   // don't modify
#define RX_SEGMENT_REJECT_CACHE_EN	(__PROJECT_MESH_PRO__ || __PROJECT_MESH_GW_NODE__)   // 

//------------ mesh config (user can config)-------------
#define MESH_NORMAL_MODE			0
#define MESH_CLOUD_ENABLE			1
#define MESH_SPIRIT_ENABLE			2	// use this mode should burn in the para in 0x78000,or use init para should enable the  con_sec_data
#define MESH_AES_ENABLE 			3
#define MESH_GN_ENABLE 		    	4
#define MESH_MI_ENABLE          	5
#define MESH_MI_SPIRIT_ENABLE   	6   // dual vendor
#define MESH_IRONMAN_MENLO_ENABLE   7   // include boot_loader.bin and light.bin
#define MESH_ZB_BL_DUAL_ENABLE      8   // mesh && zigbee normal dual mode with bootloader
#define MESH_PIPA_ENABLE        	9   // 
#define MESH_TAIBAI_ENABLE			10
#define MESH_SIG_PVT_DUAL_ENABLE	11 // sig mesh && private mesh dual mode with bootloader
#define MESH_NMW_ENABLE				12
#define MESH_LLSYNC_ENABLE			13
#define MESH_TELINK_PLATFORM_ENABLE	14

#ifndef MESH_USER_DEFINE_MODE
#if __PROJECT_MESH_PRO__
#define MESH_USER_DEFINE_MODE 		MESH_NORMAL_MODE 	// must normal
#elif __PROJECT_SPIRIT_LPN__
#define MESH_USER_DEFINE_MODE 		MESH_NORMAL_MODE
#else
#define MESH_USER_DEFINE_MODE 		MESH_NORMAL_MODE	// platform selection
#endif
#endif

#if (TESTCASE_FLAG_ENABLE)
#define PROV_AUTH_LEAK_REFLECT_EN		0
#define PROV_AUTH_LEAK_RECREATE_KEY_EN	0 // 
#else
#define PROV_AUTH_LEAK_REFLECT_EN		1
#define PROV_AUTH_LEAK_RECREATE_KEY_EN	1 // recreate public key and private key
#endif

// vendor id list
#define SHA256_BLE_MESH_PID				0x01A8
#define VENDOR_ID_MI		    		0x038F
#define VENDOR_ID_NMW					0x027D
#define VENDOR_ID_LLSYNC				0x013A

// mi product type 
#define MI_PRODUCT_TYPE_CT_LIGHT		0x01
#define MI_PRODUCT_TYPE_LAMP			0x02
#define MI_PRODUCT_TYPE_ONE_ONOFF		0x11
#define MI_PRODUCT_TYPE_TWO_ONOFF		0x12
#define MI_PRODUCT_TYPE_THREE_ONOFF		0x13
#define MI_PRODUCT_TYPE_PLUG			0x21
#define MI_PRODUCT_TYPE_FANS			0x30

// SHARE subscription list and publish address
#if (MESH_USER_DEFINE_MODE == MESH_SPIRIT_ENABLE)
#define VENDOR_ID 					SHA256_BLE_MESH_PID
#define AIS_ENABLE					1
#define PROVISION_FLOW_SIMPLE_EN    1
#define ALI_MD_TIME_EN				0
#define ALI_NEW_PROTO_EN			0
#elif(MESH_USER_DEFINE_MODE == MESH_TAIBAI_ENABLE)
#define VENDOR_ID 					0x011c
#define AIS_ENABLE					1
#define PROVISION_FLOW_SIMPLE_EN    1
#define ALI_MD_TIME_EN				0
#define ALI_NEW_PROTO_EN			0
#define DU_FW_MAIN_VER				1
#define DU_FW_ADD_VER				0
#define DU_FW_INFO_VER				0
#define DU_FW_VER					((DU_FW_MAIN_VER<<12)|(DU_FW_ADD_VER<<8)|(DU_FW_INFO_VER))



#elif(MESH_USER_DEFINE_MODE == MESH_CLOUD_ENABLE)
#define VENDOR_ID 					SHA256_BLE_MESH_PID
#define AIS_ENABLE					1
#define PROVISION_FLOW_SIMPLE_EN    1
#elif(MESH_USER_DEFINE_MODE == MESH_GN_ENABLE)
#define PROVISION_FLOW_SIMPLE_EN    1
#elif(MESH_USER_DEFINE_MODE == MESH_MI_ENABLE)
#define AIS_ENABLE					0
#define PROVISION_FLOW_SIMPLE_EN    0
#define MI_API_ENABLE               1
#define MI_SWITCH_LPN_EN			0   // only support 825x serials 

//#define MI_MESH_TEMPLATE_LIGHTNESS		0
#define MI_MESH_TEMPLATE_LIGHTCTL			1
//#define MI_MESH_TEMPLATE_ONE_KEY_SWITCH	2
//#define MI_MESH_TEMPLATE_TWO_KEY_SWITCH	3
//#define MI_MESH_TEMPLATE_THREE_KEY_SWITCH	4
//#define MI_MESH_TEMPLATE_FAN				5

#if  defined(MI_MESH_TEMPLATE_LIGHTNESS)
#define MI_PRODUCT_TYPE				MI_PRODUCT_TYPE_LAMP
#elif defined(MI_MESH_TEMPLATE_LIGHTCTL)
#define MI_PRODUCT_TYPE				MI_PRODUCT_TYPE_CT_LIGHT
#elif defined(MI_MESH_TEMPLATE_ONE_KEY_SWITCH)
#define MI_PRODUCT_TYPE				MI_PRODUCT_TYPE_ONE_ONOFF
#elif defined(MI_MESH_TEMPLATE_TWO_KEY_SWITCH)
#define MI_PRODUCT_TYPE				MI_PRODUCT_TYPE_TWO_ONOFF
#elif defined(MI_MESH_TEMPLATE_THREE_KEY_SWITCH)
#define MI_PRODUCT_TYPE				MI_PRODUCT_TYPE_THREE_ONOFF
#elif defined(MI_MESH_TEMPLATE_FAN)
#define MI_PRODUCT_TYPE				MI_PRODUCT_TYPE_FANS
#endif
/*
	#if MI_SWITCH_LPN_EN
#define BLT_SOFTWARE_TIMER_ENABLE	1
#define MI_PRODUCT_TYPE				LIGHT_TYPE_PANEL
	#else
#define MI_PRODUCT_TYPE				LIGHT_TYPE_CT	
	#endif
*/
#elif(MESH_USER_DEFINE_MODE == MESH_MI_SPIRIT_ENABLE)
#define VENDOR_ID                   VENDOR_ID_MI // use mi vendor_id and mi mac by default
#define AIS_ENABLE					1
#define PROVISION_FLOW_SIMPLE_EN    1	
#define ALI_MD_TIME_EN				0
#define MI_API_ENABLE               1
#define MI_SWITCH_LPN_EN			0   // only support 825x serials 
	#if MI_SWITCH_LPN_EN
#define BLT_SOFTWARE_TIMER_ENABLE	1
#define MI_PRODUCT_TYPE				MI_PRODUCT_TYPE_ONE_ONOFF
	#else
#define MI_PRODUCT_TYPE				MI_PRODUCT_TYPE_CT_LIGHT	
	#endif
#elif((MESH_USER_DEFINE_MODE == MESH_IRONMAN_MENLO_ENABLE)||(MESH_USER_DEFINE_MODE == MESH_ZB_BL_DUAL_ENABLE))
    #if __PROJECT_BOOTLOADER__
#define FW_START_BY_LEGACY_BOOTLOADER_EN   0
    #else
#define FW_START_BY_LEGACY_BOOTLOADER_EN   1
    #endif
#define DUAL_MESH_ZB_BL_EN          1
#elif(MESH_USER_DEFINE_MODE == MESH_SIG_PVT_DUAL_ENABLE)
#define PROVISION_FLOW_SIMPLE_EN    0
    #if __PROJECT_BOOTLOADER__
#define FW_START_BY_LEGACY_BOOTLOADER_EN   0
    #else
#define FW_START_BY_LEGACY_BOOTLOADER_EN   1
    #endif
#define DUAL_MESH_SIG_PVT_EN        1
#define MESH_NAME					"telink_mesh1"
#define MESH_PWD					"123"
#define MESH_LTK					{0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf}
#elif (MESH_USER_DEFINE_MODE == MESH_NMW_ENABLE)
#define VENDOR_ID 					VENDOR_ID_NMW
#define NMW_ENABLE					1
#define AIS_ENABLE					1
#define PROVISION_FLOW_SIMPLE_EN    1
#define ALI_MD_TIME_EN				0
#define ALI_NEW_PROTO_EN			0
#elif (MESH_USER_DEFINE_MODE == MESH_LLSYNC_ENABLE)
#define LLSYNC_ENABLE				1
#define LLSYNC_PROVISION_AUTH_OOB	LLSYNC_ENABLE
#define LLSYNC_LOG_EN				(1 && LLSYNC_ENABLE && HCI_LOG_FW_EN)	// print log of llsync SDK
#define BLT_SOFTWARE_TIMER_ENABLE	LLSYNC_ENABLE

#define PROVISION_SUCCESS_QUICK_RECONNECT_ENABLE	1
#define VENDOR_ID 					VENDOR_ID_LLSYNC
#define PROVISION_FLOW_SIMPLE_EN    1
#elif (MESH_USER_DEFINE_MODE == MESH_TELINK_PLATFORM_ENABLE)
#define PLATFORM_TELINK_EN			1
#define PROVISION_FLOW_SIMPLE_EN    1	// need to be 1, because cps of node may be different from database in some abnormal case when testing.
#elif (DEBUG_CFG_CMD_GROUP_AK_EN)
#define PROVISION_FLOW_SIMPLE_EN    1
#else // (MESH_USER_DEFINE_MODE == MESH_NORMAL_MODE  and all other)
// nothing special
#endif

#ifndef VENDOR_ID
#define VENDOR_ID					0x0211
#endif

#define SUBSCRIPTION_BOUND_STATE_SHARE_EN		1	/* must 1 to follow spec. same to "SUBSCRIPTION_SHARE_EN" of legacy version.
													refer to "1.4.1.1 Bound states -> 1.4.1.1.1 Subscription Lists on bound states".
													default just share between models with bound states, 
													if want to bind more, such as vendor model, please refert to "SHARE_ALL_LIGHT_STATE_MODEL_EN" 
													*/
										
#if (MESH_USER_DEFINE_MODE == MESH_TAIBAI_ENABLE)
#define DU_ENABLE 			1
#define DU_LPN_EN			0
#define DU_ULTRA_PROV_EN	0	

	#if DU_LPN_EN
#define LPN_CONTROL_EN		1
#define LPN_FAST_OTA_EN		1
	#endif
	#if DU_LPN_EN
		#if LPN_CONTROL_EN
	#define RTC_USE_32K_RC_ENABLE		0
	#define DU_ADV_INTER_MS				600
	#define DU_ADV_INTER_VAL			(DU_ADV_INTER_MS*1000/625)
	#define BLT_SOFTWARE_TIMER_ENABLE	1
	#define DU_PID                  	0x006b448d
		#else
	#define RTC_USE_32K_RC_ENABLE		1
	#define DU_ADV_INTER_MS				1700
	#define DU_ADV_INTER_VAL			(DU_ADV_INTER_MS*1000/625)
	#define BLT_SOFTWARE_TIMER_ENABLE	1
	#define DU_PID                  	0x006b2d1d
		#endif
	#else
	#define DU_PID						7003001
	#endif
	#if(DU_PID == 0x006b2d1d)
	#define DU_BIND_CHECK_EN            1
	#else
	#define DU_BIND_CHECK_EN            0
	#endif
#else
#define DU_LPN_EN						0
#define DU_ENABLE 						0
#endif

#ifndef MI_API_ENABLE
#define MI_API_ENABLE 					0
#endif
#ifndef NL_API_ENABLE
#define NL_API_ENABLE 					0
#endif
#ifndef FW_START_BY_LEGACY_BOOTLOADER_EN
#define FW_START_BY_LEGACY_BOOTLOADER_EN  0
#endif
#ifndef DUAL_MESH_ZB_BL_EN
#define DUAL_MESH_ZB_BL_EN  			0
#endif
#ifndef DUAL_MESH_SIG_PVT_EN
#define DUAL_MESH_SIG_PVT_EN  			0
#endif
#ifndef NMW_ENABLE
#define NMW_ENABLE	  					0
#endif
#ifndef AIS_ENABLE
#define AIS_ENABLE						0
#endif
#ifndef LLSYNC_ENABLE
#define LLSYNC_ENABLE					0
#endif
#ifndef PROVISION_FLOW_SIMPLE_EN
#define PROVISION_FLOW_SIMPLE_EN		0
#endif

#if MI_API_ENABLE
	#if MCU_CORE_TYPE == MCU_CORE_8269
	#define BLT_SOFTWARE_TIMER_ENABLE	0
	#endif
	#ifndef BLT_SOFTWARE_TIMER_ENABLE
	#define BLT_SOFTWARE_TIMER_ENABLE 	1
	#endif
#endif


#if __PROJECT_SPIRIT_LPN__
#define SPIRIT_PRIVATE_LPN_EN			1 // must
	#if AIS_ENABLE
#define SPIRIT_VENDOR_EN				1 // must
	#else
#define SPIRIT_VENDOR_EN				0	
	#endif
#else
#define SPIRIT_PRIVATE_LPN_EN			0
#if((MESH_USER_DEFINE_MODE == MESH_SPIRIT_ENABLE) ||\
	(MESH_USER_DEFINE_MODE == MESH_MI_SPIRIT_ENABLE)||\
	(MESH_USER_DEFINE_MODE == MESH_TAIBAI_ENABLE))
#define SPIRIT_VENDOR_EN				1
#else
#define SPIRIT_VENDOR_EN				0 // must
#endif
#endif

#if (MESH_USER_DEFINE_MODE == MESH_PIPA_ENABLE)
#define DUAL_OTA_NEED_LOGIN_EN      	1
#define ENCODE_OTA_BIN_EN           	1
#endif

#ifndef MI_SWITCH_LPN_EN
#define MI_SWITCH_LPN_EN 				0
#endif

#define VENDOR_MD_MI_EN             	(MI_API_ENABLE)
#define VENDOR_MD_NORMAL_EN         	((!MI_API_ENABLE) || AIS_ENABLE)    // include ali
#define VENDOR_SUB_OP_USER_DEMO_EN      (0) // add user demo subcommand for "VD_GROUP_G_GET, VD_GROUP_G_SET, VD_GROUP_G_SET_NOACK, VD_GROUP_G_STATUS"
#define VENDOR_OP_USER_DEMO_EN          (0) // add user demo vendor command

#define DUAL_VENDOR_EN              	(VENDOR_MD_MI_EN && VENDOR_MD_NORMAL_EN)

#define DRAFT_FEATURE_VENDOR_TYPE_NONE          0
#define DRAFT_FEATURE_VENDOR_TYPE_ONE_OP        1   // only use "C0"
#define DRAFT_FEATURE_VENDOR_TYPE_MULTI_OP      2

#if (WIN32)
#define DEBUG_VENDOR_CMD_EN 		    0		// must 0, if 1, debug vendor id as server node.
#define DRAFT_FEATURE_VENDOR_TYPE_SEL   DRAFT_FEATURE_VENDOR_TYPE_NONE
#define VC_SUPPORT_ANY_VENDOR_CMD_EN	(!DEBUG_VENDOR_CMD_EN && (DRAFT_FEATURE_VENDOR_TYPE_SEL == DRAFT_FEATURE_VENDOR_TYPE_NONE))
#else
#define DRAFT_FEATURE_VENDOR_TYPE_SEL   DRAFT_FEATURE_VENDOR_TYPE_NONE
    #if (MESH_USER_DEFINE_MODE == MESH_IRONMAN_MENLO_ENABLE)
#define DEBUG_VENDOR_CMD_EN 		    0       // must 0.
    #else
        #if (DRAFT_FEATURE_VENDOR_TYPE_SEL != DRAFT_FEATURE_VENDOR_TYPE_NONE)
#define DEBUG_VENDOR_CMD_EN 		    0       // must 0. because they use the same op code.
        #else
#define DEBUG_VENDOR_CMD_EN 		    1
        #endif
    #endif
#define VC_SUPPORT_ANY_VENDOR_CMD_EN	__PROJECT_MESH_PRO__		
#endif

#if WIN32
#define TLV_ENABLE					0
#elif __PROJECT_MESH_PRO__
#define TLV_ENABLE					0
#else
#define TLV_ENABLE					0
#define GATT_RP_EN					0	// just for internal test, only can be enable in 8258 mesh project.
#endif



#define DRAFT_FEAT_VD_MD_EN             (DRAFT_FEATURE_VENDOR_TYPE_SEL == DRAFT_FEATURE_VENDOR_TYPE_MULTI_OP)

#define LIGHT_TYPE_NONE				0
#define LIGHT_TYPE_CT				1
#define LIGHT_TYPE_HSL				2
#define LIGHT_TYPE_XYL				3
#define LIGHT_TYPE_POWER			4
#define LIGHT_TYPE_CT_HSL			5
#define LIGHT_TYPE_DIM			    6   // only single PWM
#define LIGHT_TYPE_PANEL			7   // only ONOFF model
#define LIGHT_TYPE_LPN_ONOFF_LEVEL  8   // only ONOFF , LEVEL model
#define TYPE_TOOTH_BRUSH			9
#define LIGHT_TYPE_NLC_CTRL_CLIENT	10	/* NLC: network lightness control; include: DICMP(Dimming Control Mesh Profile),BSSMP(Basic Scene Selector Mesh Profile).
										   Select 82xx_mesh_switch for this type please*/
//#define LIGHT_TYPE_NLC_BLC 			// set NLCP_BLC_EN to 1 to enable Basic Lightness Controller Mesh Profile.
#define LIGHT_TYPE_NLC_SENSOR 		11	// include: ALSMP(Ambient Light Sensor Mesh Profile),ENMMP(Energy Monitor Mesh Profile),OCSMP(Occupancy Sensor Mesh Profile).


/*LIGHT_TYPE_SEL   means instance type select*/
#ifndef LIGHT_TYPE_SEL  // user can define in user_app_config.h
#if (WIN32 || GATEWAY_MODEL_PLUS_EN) // __PROJECT_MESH_PRO__
#define LIGHT_TYPE_SEL				LIGHT_TYPE_CT_HSL  	// for APP and gateway
#elif __PROJECT_MESH_LPN__
#define LIGHT_TYPE_SEL				LIGHT_TYPE_LPN_ONOFF_LEVEL // LIGHT_TYPE_CT
#elif __PROJECT_MESH_GW_NODE_HK__
#define LIGHT_TYPE_SEL				LIGHT_TYPE_HSL
#elif (__PROJECT_MESH_SWITCH__ || __PROJECT_SPIRIT_LPN__)
#define LIGHT_TYPE_SEL				LIGHT_TYPE_PANEL	// LIGHT_TYPE_NLC_CTRL_CLIENT
#elif GATT_LPN_EN
#define LIGHT_TYPE_SEL 				LIGHT_TYPE_LPN_ONOFF_LEVEL
#elif AUDIO_MESH_EN
#define LIGHT_TYPE_SEL 				LIGHT_TYPE_DIM
#else
	#if MI_API_ENABLE
		#if MI_PRODUCT_TYPE == MI_PRODUCT_TYPE_CT_LIGHT
#define LIGHT_TYPE_SEL				LIGHT_TYPE_CT
		#elif MI_PRODUCT_TYPE == MI_PRODUCT_TYPE_LAMP
#define LIGHT_TYPE_SEL				LIGHT_TYPE_CT		
		#elif (	MI_PRODUCT_TYPE == MI_PRODUCT_TYPE_ONE_ONOFF ||\
				MI_PRODUCT_TYPE == MI_PRODUCT_TYPE_TWO_ONOFF ||\
				MI_PRODUCT_TYPE == MI_PRODUCT_TYPE_THREE_ONOFF)
#define LIGHT_TYPE_SEL				LIGHT_TYPE_PANEL
		#elif (MI_PRODUCT_TYPE == MI_PRODUCT_TYPE_PLUG)
#define LIGHT_TYPE_SEL				LIGHT_TYPE_PANEL
		#elif (MI_PRODUCT_TYPE == MI_PRODUCT_TYPE_FANS)
#define LIGHT_TYPE_SEL				LIGHT_TYPE_PANEL
		#endif
	#else
#define LIGHT_TYPE_SEL				LIGHT_TYPE_CT	// 
	#endif
#endif
#endif


/************************ mesh model config ************************************/

/***************** mesh V1.1 NLC feature *************************/
#if (LIGHT_TYPE_SEL == LIGHT_TYPE_NLC_CTRL_CLIENT)
#include "nlc/nlc_ctrl_client_model_config.h"
#elif (LIGHT_TYPE_SEL == LIGHT_TYPE_NLC_SENSOR)
#include "nlc/nlc_sensor_model_config.h"
#else

/**
 * NLCP_BLC_EN: Network Lighting Control profile, Basic Lightness Control, Enable.
 * refer to spec "BLCNLCP_v1.0.pdf" or a later version.
*/
#if ((LIGHT_TYPE_SEL == LIGHT_TYPE_CT) || (LIGHT_TYPE_SEL == LIGHT_TYPE_HSL) || \
	 (LIGHT_TYPE_SEL == LIGHT_TYPE_XYL) || (LIGHT_TYPE_SEL == LIGHT_TYPE_DIM))
#define NLCP_BLC_EN					0	// BLCNLCP: Basic Lightness Controller Network Lighting Profile
#else
#define NLCP_BLC_EN					0	// must 0
#endif

// other NLC profiles please refer to NLCP_DIC_EN, NLCP_BSS_EN, NLCP_TYPE_ALS, NLCP_TYPE_OCS, NLCP_TYPE_ENM in other files.

/***************** mesh V1.1 NLC feature end *********************/

#ifndef LIGHT_TYPE_CT_EN
#if (LIGHT_TYPE_SEL == LIGHT_TYPE_CT) || (LIGHT_TYPE_SEL == LIGHT_TYPE_CT_HSL)
#define LIGHT_TYPE_CT_EN            1
#elif __PROJECT_MESH_SWITCH__
#define LIGHT_TYPE_CT_EN            1
#else
#define LIGHT_TYPE_CT_EN            0
#endif
#endif

#ifndef LIGHT_TYPE_HSL_EN
#if (LIGHT_TYPE_SEL == LIGHT_TYPE_HSL) || (LIGHT_TYPE_SEL == LIGHT_TYPE_CT_HSL)
#define LIGHT_TYPE_HSL_EN           1
#else
#define LIGHT_TYPE_HSL_EN           0
#endif
#endif

#if ((LIGHT_TYPE_CT_EN) || (LIGHT_TYPE_HSL_EN) || (LIGHT_TYPE_SEL == LIGHT_TYPE_DIM) || (LIGHT_TYPE_SEL == LIGHT_TYPE_XYL))
    #if(WIN32)
#define MD_LIGHT_CONTROL_EN			1	// must 1
    #else
#define MD_LIGHT_CONTROL_EN			(0 || NLCP_BLC_EN)
		#if MD_LIGHT_CONTROL_EN
#define LIGHT_CONTROL_MODEL_LOCATE_PRIMARY_ELEMENT			0
#define LIGHT_CONTROL_SERVER_LOCATE_EXCLUSIVE_ELEMENT_EN	(MD_LIGHT_CONTROL_EN && MD_SERVER_EN && (0 == LIGHT_CONTROL_MODEL_LOCATE_PRIMARY_ELEMENT))
		#endif
    #endif
#else
#define MD_LIGHT_CONTROL_EN			0	// must 0
#endif

#if __PROJECT_MESH_SWITCH__
#define MD_LIGHTNESS_EN             1
#define MD_LEVEL_EN                 0
#elif ((LIGHT_TYPE_SEL == LIGHT_TYPE_NONE) || (LIGHT_TYPE_SEL == LIGHT_TYPE_PANEL) || (LIGHT_TYPE_SEL == TYPE_TOOTH_BRUSH))
#define MD_LIGHTNESS_EN             0
#define MD_LIGHTNESS_EN             0
#define MD_LEVEL_EN                 0
#elif (LIGHT_TYPE_SEL == LIGHT_TYPE_LPN_ONOFF_LEVEL)
#define MD_LIGHTNESS_EN             0
#define MD_LEVEL_EN                 1
#else
#define MD_LIGHTNESS_EN             1
#define MD_LEVEL_EN                 1
#endif

#if(DUAL_VENDOR_EN)
#define CMD_LINEAR_EN               0
#else
#define CMD_LINEAR_EN               1
#endif

/************************ mesh V1.1 feature ************************************/
#if 1
/**
 * PROV_EPA_EN: provision EPA(Enhanced Provisioning Authentication) enable.
 * add new Algorithms "BTM_ECDH_P256_HMAC_SHA256_AES_CCM" in "5.4.1.2 Provisioning Capabilities" of spec "MshPRT_v1.1.pdf".
 */
#if WIN32
#define PROV_EPA_EN					1
#elif __PROJECT_MESH_PRO__
#define PROV_EPA_EN					1	// enhance provision auth: support two Algorithms in capability.
#else
	#if AIS_ENABLE
#define PROV_EPA_EN					0	// in the ais mode ,it will not support EPA
	#else
#define PROV_EPA_EN					1	// enhance provision auth: support two Algorithms in capability.
	#endif
#endif

/**
 * CERTIFY_BASE_ENABLE: Certificate-based provisioning.
 * refer to "5.5 Certificate-based provisioning" of spec "MshPRT_v1.1.pdf".
 */
#if WIN32
#define CERTIFY_BASE_ENABLE 	1
#else
	#if MI_API_ENABLE
#define CERTIFY_BASE_ENABLE 	0		// must 0
	#else
#define CERTIFY_BASE_ENABLE		0
	#endif
#endif

/**
 * MD_REMOTE_PROV: model of remote provision.
 * refer to "4.4.5 Remote Provisioning Server model" and "4.4.6 Remote Provisioning Client model" of spec "MshPRT_v1.1.pdf".
 */
#ifndef MD_REMOTE_PROV
#if (WIN32)
#define MD_REMOTE_PROV              1
#elif (MESH_USER_DEFINE_MODE == MESH_IRONMAN_MENLO_ENABLE)
#define MD_REMOTE_PROV              0   // default disable
#elif (MI_API_ENABLE)
#define MD_REMOTE_PROV              0   // must 0
#elif (__PROJECT_MESH__)
#define MD_REMOTE_PROV              0   // dufault disable
#elif (__PROJECT_MESH_PRO__)
#define MD_REMOTE_PROV              0   // dufault disable
#else
#define MD_REMOTE_PROV              0   // must 0, other project not support due to low power application.
#endif
#endif

#if	MD_REMOTE_PROV
#define REMOTE_PROV_SCAN_GATT_EN	0
#define URI_DATA_ADV_ENABLE			0
#define ACTIVE_SCAN_ENABLE  		0
	#if TESTCASE_FLAG_ENABLE
#define REMOTE_SET_RETRY_EN			0	
#define DEVICE_KEY_REFRESH_ENABLE	1 	// must 1
	#else
#define REMOTE_SET_RETRY_EN			1
		#if WIN32
#define DEVICE_KEY_REFRESH_ENABLE	1	
		#else
#define DEVICE_KEY_REFRESH_ENABLE	0	// default 0 to compatible with previous version's VC_node_info[]
		#endif
	#endif
#else
#define REMOTE_PROV_SCAN_GATT_EN	0
#define URI_DATA_ADV_ENABLE			0
#define ACTIVE_SCAN_ENABLE  		0
#endif

/**
 * MD_MESH_OTA_EN: model mesh OTA enable. OTA is the same as DFU(device firmware update).
 * refer to spec MshDFU_v1.0(mesh device firmware update) and MshMBT_v1.0(The Mesh Binary Large Object Transfer) profile.
 */
#ifndef MD_MESH_OTA_EN
#if (DEBUG_CFG_CMD_GROUP_AK_EN || DEBUG_PUBLISH_REDUCE_COLLISION_TEST_EN)
#define MD_MESH_OTA_EN				1	// just for internal test.
#elif (__PROJECT_MESH_PRO__)   // app & gateway
    #if WIN32
#define MD_MESH_OTA_EN				1
    #else // gateway
#define MD_MESH_OTA_EN				0   // dufault disable
    #endif
#elif __PROJECT_MESH_SWITCH__			// xxxx_mesh_switch compile project
#define MD_MESH_OTA_EN				0
#else
	#if ((MESH_USER_DEFINE_MODE == MESH_MI_ENABLE) || SPIRIT_PRIVATE_LPN_EN || (LIGHT_TYPE_SEL == TYPE_TOOTH_BRUSH))
#define MD_MESH_OTA_EN				0   // 
    #elif (MESH_USER_DEFINE_MODE == MESH_IRONMAN_MENLO_ENABLE)
#define MD_MESH_OTA_EN				0	// 
    #elif (AIS_ENABLE)
#define MD_MESH_OTA_EN				0	// default disable
	#else
#define MD_MESH_OTA_EN				0   // dufault disable
	#endif
#endif
#endif

#if (MD_MESH_OTA_EN)
	#if(__PROJECT_MESH_PRO__ || __PROJECT_MESH_GW_NODE__)
#define DISTRIBUTOR_UPDATE_CLIENT_EN    1
#define DISTRIBUTOR_UPDATE_SERVER_EN    DISTRIBUTOR_UPDATE_CLIENT_EN
		#if (WIN32 && DISTRIBUTOR_UPDATE_CLIENT_EN)
#define INITIATOR_CLIENT_EN             1   // only app or VC enable
		#endif
	#else
#define DISTRIBUTOR_UPDATE_CLIENT_EN    0	// must 0
		#if (__PROJECT_MESH__)				// lpn, switch should not enable
#define DISTRIBUTOR_UPDATE_SERVER_EN    0   // include distribute server, update client and blob transfer client model
		#endif
	#endif
#endif

#if (WIN32 || DU_ENABLE)
#define OTA_ADOPT_RULE_CHECK_PID_EN		0	// in the du mode ,it will not allow to check the pid.because it will change format
#else
#ifndef OTA_ADOPT_RULE_CHECK_PID_EN			// for both GATT and ADV
#define OTA_ADOPT_RULE_CHECK_PID_EN		1
#define OTA_ADOPT_RULE_CHECK_VID_EN		0
#endif
#endif

#if (DISTRIBUTOR_UPDATE_SERVER_EN && (DISTRIBUTOR_UPDATE_CLIENT_EN || PTS_TEST_OTA_EN)) // because PTS testing OTA update server will enable PTS_TEST_BLOB_TRANSFER_FLOW_EN. 
#define DISTRIBUTOR_START_TLK_EN        1   // only used in internal to be compatible with old version INI. 
#endif

/**
 * MD_PRIVACY_BEA: the full name is model private beacon. for private security network beacon.
 *                 refer to "3.10.4 Mesh Private beacon" of spec "MshPRT_v1.1.pdf".
 * PRIVATE_PROXY_FUN_EN: the full name is private proxy function enable. for private .
 *                 refer to "3.4.6.6 Private Proxy functionality" and "3.4.6.8 Private Node Identity functionality" of spec "MshPRT_v1.1.pdf".
 */
#if WIN32
#define MD_PRIVACY_BEA				1
#define PRIVATE_PROXY_FUN_EN		1
#else
#define MD_PRIVACY_BEA				0	// support advertising with private security beacon (there is no private unprovision beacon)
#define PRIVATE_PROXY_FUN_EN		0	// support advertising with private network identity and private node identity 
#endif

/**
 * COMPOSITION_DATA_PAGE1_PRESENT_EN: report compositon data page 1 when receive composition date get command.
 */
#define COMPOSITION_DATA_PAGE1_PRESENT_EN 			0

/**
 * MD_DF_CFG_SERVER_EN: model directed forwarding server model.
 * refer to "4.4.7 Directed Forwarding Configuration Server model" and so on of spec "MshPRT_v1.1.pdf".
 */
 
/**
 * MD_DF_CFG_CLIENT_EN: model directed forwarding client model.
 * refer to "4.4.8 Directed Forwarding Configuration Client model" and so on of spec "MshPRT_v1.1.pdf".
 */
 
/**
 * MD_SBR_CFG_SERVER_EN: model subnet bridge server model.
 * refer to "4.4.9 Bridge Configuration Server model" and so on of spec "MshPRT_v1.1.pdf".
 */
 
/**
 * MD_SBR_CFG_CLIENT_EN: model subnet bridge client model.
 * refer to "4.4.10 Bridge Configuration Client model" and so on of spec "MshPRT_v1.1.pdf".
 */
 
/**
 * MD_SAR_EN: model Segmentation And Reassembly Enable.
 * refer to "4.4.15 SAR Configuration Server model" and "4.4.16 SAR Configuration Client model" and so on of spec "MshPRT_v1.1.pdf".
 */
 
/**
 * MD_ON_DEMAND_PROXY_EN: model On-Demand Private Proxy Enable.
 * refer to "4.4.13 On-Demand Private Proxy Server model" and "4.4.14 On-Demand Private Proxy Client model" and so on of spec "MshPRT_v1.1.pdf".
 */
 
/**
 * MD_OP_AGG_EN: model Opcodes Aggregator Enable.
 * refer to "4.4.19 Opcodes Aggregator Server model" and "4.4.20 Opcodes Aggregator Client model" and so on of spec "MshPRT_v1.1.pdf".
 */
 
/**
 * MD_LARGE_CPS_EN: model Large Composition Data Enable.
 * refer to "4.4.21 Large Composition Data Server model" and "4.4.22 Large Composition Data Client model" and so on of spec "MshPRT_v1.1.pdf".
 */
 
/**
 * MD_SOLI_PDU_RPL_EN: model Solicitation PDU(Protocol Data Unit) RPL(Replay Protection List) Enable.
 * refer to "4.4.17 Solicitation PDU RPL Configuration Server model" and "4.4.18 Solicitation PDU RPL Configuration Client model" and so on of spec "MshPRT_v1.1.pdf".
 */

/**
 * NLC profiles: Network Lighting Control profiles.
 * include BLCNLCP(Basic Lightness Controller Network Lighting Profile). refer to NLCP_BLC_EN and spec "BLCNLCP_v1.0.pdf".
 * other NLC profiles please refer to the definition location next to NLCP_BLC_EN.
 */ 
#endif
/************************ mesh V1.1 feature end ********************************/

#define MD_ONOFF_EN                 1

#if WIN32
#define MD_DEF_TRANSIT_TIME_EN      1   // must 1
#define MD_POWER_ONOFF_EN           1   // must 1
#define MD_TIME_EN                  1   // must 1
#define MD_SCENE_EN                 1   // must 1
#define MD_SCHEDULE_EN              1   // must 1
#define MD_PROPERTY_EN				1	
#define	MD_LOCATION_EN				1	// 
#define MD_BATTERY_EN				1
#define MD_DF_CFG_SERVER_EN			0
#define MD_DF_CFG_CLIENT_EN			1   // directed forwarding client model. 
#define MD_SBR_CFG_SERVER_EN		0
#define MD_SBR_CFG_CLIENT_EN		1   // subnet bridge client model. 
#define MD_SAR_EN					1
#define MD_ON_DEMAND_PROXY_EN		1
#define MD_SOLI_PDU_RPL_EN			1
#define	MD_OP_AGG_EN				1
#define MD_LARGE_CPS_EN				1
#define MD_CMR_EN					(0 && FEATURE_RELAY_EN)  // controlled mesh relay models

#if DEBUG_SHOW_VC_SELF_EN
#define MD_SERVER_EN                1   // SIG and vendor models, and exclude sensor model.
#else
#define MD_SERVER_EN                0   // SIG and vendor models, and exclude sensor model.
#endif
#define MD_CLIENT_EN                1   // just SIG models, and exclude sensor model.
#define MD_CLIENT_VENDOR_EN         1
#define MD_VENDOR_2ND_EN            (DEBUG_VENDOR_CMD_EN && MI_API_ENABLE)
#define MD_SENSOR_SERVER_EN			MD_SERVER_EN	
#define MD_SENSOR_CLIENT_EN		    MD_CLIENT_EN	
#elif(1 == TESTCASE_FLAG_ENABLE)
#define MD_DEF_TRANSIT_TIME_EN      1  
#define MD_POWER_ONOFF_EN           1   
#define MD_TIME_EN                  1  
#define MD_SCENE_EN                 1   
#define MD_SCHEDULE_EN              1   
#define MD_PROPERTY_EN				0
#define	MD_LOCATION_EN				0	// location,sensor,battery,property use same flash addr, but one sector max store 6 models			
#define MD_BATTERY_EN				0
#define MD_DF_CFG_SERVER_EN			0   // directed forwarding server model.
#define MD_DF_CFG_CLIENT_EN			0
#define MD_SBR_CFG_SERVER_EN		0   // subnet bridge server model.
#define MD_SBR_CFG_CLIENT_EN		0
#define MD_SAR_EN					0
#define MD_ON_DEMAND_PROXY_EN		(0 && PRIVATE_PROXY_FUN_EN)
#define MD_SOLI_PDU_RPL_EN			MD_ON_DEMAND_PROXY_EN
#define	MD_OP_AGG_EN				0
#define MD_LARGE_CPS_EN				0
#define MD_CMR_EN					(0 && FEATURE_RELAY_EN)  // controlled mesh relay models

#define MD_SERVER_EN                1   // SIG and vendor models, and exclude sensor model.
#define MD_CLIENT_EN                1   // just SIG models, and exclude sensor model.
#define MD_CLIENT_VENDOR_EN         1

#define MD_SENSOR_SERVER_EN			0	
#define MD_SENSOR_CLIENT_EN		    0	

#elif(__PROJECT_MESH_PRO__ || __PROJECT_MESH_GW_NODE__)     // GATEWAY
	#if MCU_CORE_TYPE == MCU_CORE_8269 // ram is not enough ,it will have different settings for 69 and 5x .
#define MD_DEF_TRANSIT_TIME_EN      1
#define MD_POWER_ONOFF_EN           MD_DEF_TRANSIT_TIME_EN 	// because both of them save in same flash sector.
#define MD_TIME_EN                  0
#define MD_SCENE_EN                 0
#define MD_SCHEDULE_EN              MD_TIME_EN  // because both of them save in same flash sector.
#define MD_PROPERTY_EN				0
#define	MD_LOCATION_EN				0	// location,sensor,battery use same flash addr, but one sector max store 6 models
#define MD_BATTERY_EN				0
		#if(__PROJECT_MESH_GW_NODE__)
#define MD_SERVER_EN                1  
#define MD_CLIENT_EN                1 
		#else
#define MD_SERVER_EN                0   // SIG and vendor models
#define MD_CLIENT_EN                (!MD_SERVER_EN) // just SIG models
		#endif
#define MD_CLIENT_VENDOR_EN         0
#define MD_VENDOR_2ND_EN            (DEBUG_VENDOR_CMD_EN && MI_API_ENABLE)
#define MD_SENSOR_SERVER_EN			0	
#define MD_SENSOR_CLIENT_EN		    0	
	#else // core type is 825x serial ,and the ram is enough .
#define MD_DEF_TRANSIT_TIME_EN      1
#define MD_POWER_ONOFF_EN           MD_DEF_TRANSIT_TIME_EN 	// because both of them save in same flash sector.
#define MD_TIME_EN                  1
#define MD_SCENE_EN                 1
#define MD_SCHEDULE_EN              MD_TIME_EN  // because both of them save in same flash sector.
#define MD_PROPERTY_EN				0
#define	MD_LOCATION_EN				0	// location,sensor,battery use same flash addr, but one sector max store 6 models
#define MD_BATTERY_EN				0
#define MD_DF_CFG_SERVER_EN			0   // directed forwarding server model.
#define MD_DF_CFG_CLIENT_EN			(0 || MD_DF_CFG_SERVER_EN)
#define MD_SBR_CFG_SERVER_EN		0   // subnet bridge server model.
#define MD_SBR_CFG_CLIENT_EN		(0 || MD_SBR_CFG_SERVER_EN)
#define MD_SAR_EN					0
#define MD_ON_DEMAND_PROXY_EN		(0 && PRIVATE_PROXY_FUN_EN)
#define MD_SOLI_PDU_RPL_EN			MD_ON_DEMAND_PROXY_EN
#define	MD_OP_AGG_EN				0
#define MD_LARGE_CPS_EN				0
#define MD_CMR_EN					(0 && FEATURE_RELAY_EN)  // controlled mesh relay models

		#if(__PROJECT_MESH_GW_NODE__)
#define MD_SERVER_EN                1  
#define MD_CLIENT_EN                1 
		#else
#define MD_SERVER_EN                0   // SIG and vendor models, and exclude sensor model.
#define MD_CLIENT_EN                (!MD_SERVER_EN) // just SIG models, and exclude sensor model.
		#endif
#define MD_CLIENT_VENDOR_EN         1
#define MD_VENDOR_2ND_EN            (DEBUG_VENDOR_CMD_EN && MI_API_ENABLE)
#define MD_SENSOR_SERVER_EN			MD_SERVER_EN	// 0
#define MD_SENSOR_CLIENT_EN		    MD_CLIENT_EN	// 0
	#endif
#else                           // light node
    #if ((LIGHT_TYPE_SEL == LIGHT_TYPE_PANEL) || (LIGHT_TYPE_SEL == LIGHT_TYPE_LPN_ONOFF_LEVEL) \
        || DUAL_VENDOR_EN || (LIGHT_TYPE_SEL == TYPE_TOOTH_BRUSH))
#define MD_DEF_TRANSIT_TIME_EN      0
    #else
#define MD_DEF_TRANSIT_TIME_EN      1
    #endif
    #if (MESH_USER_DEFINE_MODE == MESH_SPIRIT_ENABLE || MESH_USER_DEFINE_MODE == MESH_TAIBAI_ENABLE)
#define MD_POWER_ONOFF_EN           0						// compatible with older version
    #else
#define MD_POWER_ONOFF_EN           MD_DEF_TRANSIT_TIME_EN 	// because both of them save in same flash sector.
    #endif
#define MD_TIME_EN                  0
#define MD_SCENE_EN                 (0 || NLCP_BLC_EN)
#define MD_SCHEDULE_EN              MD_TIME_EN  // because both of them save in same flash sector.
#define MD_PROPERTY_EN				0
#define	MD_LOCATION_EN				0	// location,sensor,battery use same flash addr, but one sector max store 6 models
#define MD_BATTERY_EN				0
	#if (__PROJECT_MESH_LPN__||__PROJECT_MESH_SWITCH__||__PROJECT_SPIRIT_LPN__)
// not support directed forwarding model and subnet bridge model.
	#else
#define MD_DF_CFG_SERVER_EN			0   // directed forwarding server model.
#define MD_DF_CFG_CLIENT_EN			0
#define MD_SBR_CFG_SERVER_EN		0   // subnet bridge server model.
#define MD_SBR_CFG_CLIENT_EN		0
	#endif
#define MD_SAR_EN					0
#define MD_ON_DEMAND_PROXY_EN		(0 && PRIVATE_PROXY_FUN_EN)
#define MD_SOLI_PDU_RPL_EN			MD_ON_DEMAND_PROXY_EN
#define	MD_OP_AGG_EN				0
#define MD_LARGE_CPS_EN				0
#define MD_CMR_EN					(0 && FEATURE_RELAY_EN)  // controlled mesh relay models

	#if __PROJECT_MESH_SWITCH__
#define MD_SERVER_EN                0   // SIG and vendor models, and exclude sensor model.
#define MD_CLIENT_EN                1   // just SIG models, and exclude sensor model.
	#else
#define MD_SERVER_EN                1   // SIG and vendor models, and exclude sensor model.
#define MD_CLIENT_EN                0   // just SIG models, and exclude sensor model.
	#endif
    #if(DUAL_VENDOR_EN)
#define MD_CLIENT_VENDOR_EN         0
    #elif ((LIGHT_TYPE_SEL == LIGHT_TYPE_PANEL) || SPIRIT_VENDOR_EN)
#define MD_CLIENT_VENDOR_EN         1
    #else
#define MD_CLIENT_VENDOR_EN         (0 || MD_CLIENT_EN)
    #endif
#define MD_VENDOR_2ND_EN            (DEBUG_VENDOR_CMD_EN && MI_API_ENABLE)
#define MD_SENSOR_SERVER_EN			0	// MD_SERVER_EN
#define MD_SENSOR_CLIENT_EN		    0	// MD_CLIENT_EN
#endif

#endif
/************************ mesh model config end ************************************/

#define MD_SENSOR_EN	    		(MD_SENSOR_SERVER_EN || MD_SENSOR_CLIENT_EN)

#define STRUCT_MD_TIME_SCHEDULE_EN          		(MD_TIME_EN || MD_SCHEDULE_EN)
#define STRUCT_MD_DEF_TRANSIT_TIME_POWER_ONOFF_EN   (MD_DEF_TRANSIT_TIME_EN || MD_POWER_ONOFF_EN)

#define FACTORY_TEST_MODE_ENABLE    		1
#define MANUAL_FACTORY_RESET_TX_STATUS_EN   1
#if (!WIN32)
#define KEEP_ONOFF_STATE_AFTER_OTA			1
#endif
#define DF_TEST_MODE_EN  					(0 && MD_DF_CFG_SERVER_EN) // Path lifetime is 12 minute in test mode.
#if DF_TEST_MODE_EN
#define DF_TEST_EXHIBITION_EN	0	// It's for the trade show test, all led of the node participating in forwarding blinks
#endif


//----------------------------------------------------------------------------------

/*ELE_CNT_EVERY_LIGHT means element count of one instance*/
#if (__PROJECT_MESH_PRO__ || (0 == MD_SERVER_EN) || PAIR_PROVISION_ENABLE) // && (!DEBUG_SHOW_VC_SELF_EN))
#define LEVEL_STATE_CNT_EVERY_LIGHT			1   // APP and gateway use 1 element always,
#else
    #if (LIGHT_TYPE_SEL == LIGHT_TYPE_CT)
        #if (MESH_USER_DEFINE_MODE == MESH_SPIRIT_ENABLE || MESH_USER_DEFINE_MODE == MESH_TAIBAI_ENABLE)
#define LEVEL_STATE_CNT_EVERY_LIGHT         1   // 2, confirm later
        #else
#define LEVEL_STATE_CNT_EVERY_LIGHT         2
        #endif
    #elif (LIGHT_TYPE_SEL == LIGHT_TYPE_HSL)
        #if (MESH_USER_DEFINE_MODE == MESH_SPIRIT_ENABLE || MESH_USER_DEFINE_MODE == MESH_TAIBAI_ENABLE)
#define LEVEL_STATE_CNT_EVERY_LIGHT         1   // 3, confirm later
        #else
#define LEVEL_STATE_CNT_EVERY_LIGHT         3
        #endif
    #elif (LIGHT_TYPE_SEL == LIGHT_TYPE_XYL)
#define LEVEL_STATE_CNT_EVERY_LIGHT         3
    #elif (LIGHT_TYPE_SEL == LIGHT_TYPE_CT_HSL)
        #if (MESH_USER_DEFINE_MODE == MESH_SPIRIT_ENABLE || MESH_USER_DEFINE_MODE == MESH_TAIBAI_ENABLE || LLSYNC_ENABLE)
#define LEVEL_STATE_CNT_EVERY_LIGHT         1   // 4, confirm later
        #else
#define LEVEL_STATE_CNT_EVERY_LIGHT         4
        #endif
    #else
#define LEVEL_STATE_CNT_EVERY_LIGHT         1
    #endif
#endif

#if LIGHT_CONTROL_SERVER_LOCATE_EXCLUSIVE_ELEMENT_EN
#define ELE_CNT_EVERY_LIGHT         (LEVEL_STATE_CNT_EVERY_LIGHT + 1) // light control model locate at a exclusive element.
#else
#define ELE_CNT_EVERY_LIGHT         LEVEL_STATE_CNT_EVERY_LIGHT
#endif

#define DEBUG_VC_FUNCTION			0
#define DEBUG_BLE_EVENT_ENABLE 		0
#define DEBUG_PREINSTALL_APP_KEY_EN 0

#define REFRESH_ST_BY_HW_EN			0

#ifndef MESH_MONITOR_EN
#define MESH_MONITOR_EN 			0
#endif

#if VC_APP_ENABLE ||__PROJECT_MESH_PRO__   
#define MD_BIND_WHITE_LIST_EN 		0
#else
#define MD_BIND_WHITE_LIST_EN 		0
#endif

#if WIN32
// log enable
#define LOG_VC_RX_TEST_EN			0
#define LOG_WRITE_CMD_FIFO_EN		1
#define LOG_PROVISION_EN 			0
#endif

// feature part ,enable or disable to cut down the ram cost 
#if (__PROJECT_MESH_LPN__ && (!WIN32))
#define FEATURE_FRIEND_EN 		0
#define FEATURE_LOWPOWER_EN		1
#define FEATURE_PROV_EN 		1
#define FEATURE_RELAY_EN		0
#define FEATURE_PROXY_EN 		1
#elif (__PROJECT_SPIRIT_LPN__ && (!WIN32))
#define FEATURE_FRIEND_EN 		0
#define FEATURE_LOWPOWER_EN		0
#define FEATURE_PROV_EN 		1
#define FEATURE_RELAY_EN		0
#define FEATURE_PROXY_EN 		1
#elif (MI_SWITCH_LPN_EN &&(!WIN32))
#define FEATURE_FRIEND_EN 		0
#define FEATURE_LOWPOWER_EN		0
#define FEATURE_PROV_EN 		1
#define FEATURE_RELAY_EN		0
#define FEATURE_PROXY_EN 		0
#elif ((GATT_LPN_EN || __PROJECT_MESH_SWITCH__)&&(!WIN32))
#define FEATURE_FRIEND_EN 		0
#define FEATURE_LOWPOWER_EN		0
#define FEATURE_PROV_EN 		1
#define FEATURE_RELAY_EN		(0 || SWITCH_ALWAYS_MODE_GATT_EN)
#define FEATURE_PROXY_EN 		1
#else
	#if DU_ENABLE
#define FEATURE_FRIEND_EN		0
	#else
#define FEATURE_FRIEND_EN 		1   // WIN 32 should be support disable: model_sig_cfg_s.frid
	#endif
#define FEATURE_LOWPOWER_EN		0
#define FEATURE_PROV_EN 		1
    #if (0 == NODE_CAN_SEND_ADV_FLAG)
#define FEATURE_RELAY_EN		0	// must 0, because it must be proxyed by another node. and messages have been relay by this node.
#define FEATURE_PROXY_EN 		0	// must 0, 
    #else
#define FEATURE_RELAY_EN		1
#define FEATURE_PROXY_EN 		1
    #endif
#endif

#if (FEATURE_LOWPOWER_EN || SPIRIT_PRIVATE_LPN_EN)
#define MAX_LPN_NUM					1   // must 1
#elif FEATURE_FRIEND_EN
#define MAX_LPN_NUM					2   // should be less than or equal to 16
#define FN_PRIVATE_SEG_CACHE_EN     0   // not push all segments to friend cache at the same time
#else
#define MAX_LPN_NUM					0   // must 0
#endif

#if 1 // for debug
#define DEBUG_PROXY_FRIEND_SHIP		0
#define PROXY_FRIEND_SHIP_MAC_FN	(0x82690014)
#define PROXY_FRIEND_SHIP_MAC_LPN1	(0x82690015)
#define PROXY_FRIEND_SHIP_MAC_LPN2	(0x82690016)

#define DEBUG_IV_UPDATE_TEST_EN     (0)
#endif

#define USER_DEFINE_SET_CCC_ENABLE 	1   // must 1

#define PROXY_PDU_TIMEOUT_TICK 		20*1000*1000

#ifndef SEND_STATUS_WHEN_POWER_ON
#if (WIN32 || __PROJECT_MESH_SWITCH__)
#define SEND_STATUS_WHEN_POWER_ON			0
#elif (DEBUG_MESH_DONGLE_IN_VC_EN && (!IS_VC_PROJECT))
#define SEND_STATUS_WHEN_POWER_ON			0
#else
#define SEND_STATUS_WHEN_POWER_ON			1
#endif
#endif


#if VENDOR_MD_NORMAL_EN
// vendor op mode define
#define VENDOR_OP_MODE_SPIRIT       1
#define VENDOR_OP_MODE_DEFAULT      2

#if (AIS_ENABLE)
#define VENDOR_OP_MODE_SEL          VENDOR_OP_MODE_SPIRIT
#else
#define VENDOR_OP_MODE_SEL          VENDOR_OP_MODE_DEFAULT  // don't modify this setting
#endif
#endif

#if (!__PROJECT_MESH_PRO__)
#define PROV_APP_KEY_SETUP_TIMEOUT_CHECK_EN     0
#endif

// flash save flag 
#define SAVE_FLAG_PRE		(0xAF)
#define SAVE_FLAG       	(0xA5)

//------------ mesh keycode-------------
#define	RC_KEY_1_ON			0x01
#define	RC_KEY_1_OFF		0x02
#define	RC_KEY_2_ON			0x03
#define	RC_KEY_2_OFF		0x04
#define	RC_KEY_3_ON			0x05
#define	RC_KEY_3_OFF		0x06
#define	RC_KEY_4_ON			0x07
#define	RC_KEY_4_OFF		0x08
#define	RC_KEY_A_ON			0x09
#define	RC_KEY_A_OFF		0x0a
#define	RC_KEY_UP			0x0b
#define	RC_KEY_DN			0x0c
#define	RC_KEY_L			0x0d
#define	RC_KEY_R			0x0e
#define	RC_KEY_M			0x0f

//-------------CMD
#define REPORT_KEY_ONLY_EN		0

#define			LGT_CMD_LIGHT_ON				0x10
#define			LGT_CMD_LIGHT_OFF				0x11//internal use
#define			LGT_CMD_LUM_UP					0x12//internal use
#define			LGT_CMD_LUM_DOWN				0x13//internal use
#define			LGT_CMD_LEFT_KEY				0x14//internal use
#define			LGT_CMD_RIGHT_KEY				0x15//internal use

#define			IRQ_TIMER1_ENABLE  			    0
#define			IRQ_TIME1_INTERVAL			    (1000) // unit: us
#define			IRQ_GPIO_ENABLE  			    0

#if (WIN32)
// support extend adv, but disable as default. to change by selecting "Extend Adv" item list.
#else
// extend adv should be running on the chip with 64k or more RAM.
// as default, only some OTA op code use extend adv defined in is_not_use_extend_adv() for compatibility.
// if needed, user can change the rule defined in is_not_use_extend_adv.
#define EXTENDED_ADV_ENABLE				(0 || AUDIO_MESH_EN)   // BLE mesh extend ADV
	#if EXTENDED_ADV_ENABLE
#define EXTENDED_ADV_PROV_ENABLE		0
	#else
#define EXTENDED_ADV_PROV_ENABLE		0	// must 0
	#endif
#endif

#if EXTENDED_ADV_ENABLE
#define ADV_RF_LEN_MAX					(255) // (253)
#define ADV_PDU_LEN_MAX					(ADV_RF_LEN_MAX-6-4)    // 6: mac, 4: extend adv head;   // payload
#else
#define ADV_RF_LEN_MAX					(37)
#define ADV_PDU_LEN_MAX					(ADV_RF_LEN_MAX-6)      // 6: mac
#endif

#if ((MD_LIGHT_CONTROL_EN == 0) && (MD_SENSOR_SERVER_EN || MD_SENSOR_CLIENT_EN))
/**
 * @brief	user define sensor mode to turn light on when light node receives sensor status,
 * 			and then auto turn off after SENSOR_LIGHTING_CTRL_ON_MS.
 */
#define SENSOR_LIGHTING_CTRL_USER_MODE_EN		0
	#if SENSOR_LIGHTING_CTRL_USER_MODE_EN
#define SENSOR_GPIO_PIN             			GPIO_PD5
#define SENSOR_LIGHTING_CTRL_ON_MS  			10000       // ms
	#endif
#endif

/**
 * @brief: PUBLISH_REDUCE_COLLISION_EN is used to reduce collision when too many nodes are publish with a short publish period.
 *         1. it is done by checking relay buffer which can indicate how many nodes are publishing now.
 *         2. if is_publish_allow() return 0 means too many other nodes are publishing so it will delay current publish, 
 *            but will not delay more than 1/2 period.
 */
#if (FEATURE_RELAY_EN && MD_SERVER_EN)
#define PUBLISH_REDUCE_COLLISION_EN		1 // 
#endif

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
