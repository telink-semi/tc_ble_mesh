/********************************************************************************************************
 * @file	app_mesh.h
 *
 * @brief	for TLSR chips
 *
 * @author	Mesh Group
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
#ifndef APP_MESH_H_
#define APP_MESH_H_
#include "../../proj_lib/ble/ble_common.h"
#include "../../proj_lib/ble/blt_config.h"

#include "../../vendor/common/light.h"
#include "../../vendor/common/user_proc.h"

#include "Test_case.h"
#include <stdarg.h>

#define SPP_CMD_SET_ADV_INTV								0xFF01
#define SPP_CMD_SET_ADV_DATA								0xFF02

#define SPP_CMD_SET_ADV_ENABLE                             	0xFF0A
#define SPP_CMD_GET_BUF_SIZE								0xFF0C
#define SPP_CMD_SET_ADV_TYPE								0xFF0D
#define SPP_CMD_SET_ADV_DIRECT_ADDR							0xFF0E
#define SPP_CMD_ADD_WHITE_LST_ENTRY							0xFF0F
#define SPP_CMD_DEL_WHITE_LST_ENTRY							0xFF10
#define SPP_CMD_RST_WHITE_LST								0xFF11
#define SPP_CMD_SET_FLT_POLICY								0xFF12
#define SPP_CMD_SET_DEV_NAME								0xFF13
#define SPP_CMD_GET_CONN_PARA								0xFF14
#define SPP_CMD_SET_CONN_PARA								0xFF15
#define SPP_CMD_GET_CUR_STATE								0xFF16
#define SPP_CMD_TERMINATE									0xFF17
#define SPP_CMD_RESTART_MOD									0xFF18
#define SPP_CMD_SEND_NOTIFY_DATA                            0xFF1C

//------- log
#if WIN32
#define LOG_FW_FUNC_EN      	1
#define ALL_CMD_WITH_STRING_EN	1
#else
	#ifndef LOG_FW_FUNC_EN
#define LOG_FW_FUNC_EN      (HCI_LOG_FW_EN)
	#endif

	#if LOG_FW_FUNC_EN
	#ifndef ALL_CMD_WITHOUT_STRING_EN
#define ALL_CMD_WITHOUT_STRING_EN	0	// set 1 to save code size
		#if (0 == ALL_CMD_WITHOUT_STRING_EN)
#define ALL_CMD_WITH_STRING_EN		(0 || DEBUG_LOG_SETTING_DEVELOP_MODE_EN)	// set 0 to save code size
		#endif
	#endif
	#endif
#endif

#ifndef	ALL_CMD_WITHOUT_STRING_EN
#define ALL_CMD_WITHOUT_STRING_EN	0
#endif
#ifndef	ALL_CMD_WITH_STRING_EN
#define ALL_CMD_WITH_STRING_EN		0
#endif
#ifndef	TL_LOG_ALL_MODULE_EN
#define TL_LOG_ALL_MODULE_EN		0	// must be 0, this MACRO is not for user, only used in library.
#endif

#define LOG_OP_STRING_EN			(LOG_FW_FUNC_EN && (0 == ALL_CMD_WITHOUT_STRING_EN))

// CMD_NO_STR: command with string; CMD_YS_STR: command without string to save code size;
#define CMD_NO_STR(op, status_cmd, model_id_tx, model_id_rx, cb, op_rsp)\
	{op, status_cmd, model_id_tx, model_id_rx, cb, op_rsp}

#if LOG_FW_FUNC_EN
#define CMD_YS_STR(op, status_cmd, model_id_tx, model_id_rx, cb, op_rsp)\
	{op, status_cmd, model_id_tx, model_id_rx, cb, op_rsp, #op}

	#if ALL_CMD_WITH_STRING_EN
#undef	CMD_NO_STR
#define CMD_NO_STR				CMD_YS_STR
	#endif
#else
#define CMD_YS_STR				CMD_NO_STR
#endif

//----------------------------------- test

#if WIN32
#define MESH_NODE_MAX_NUM       1000  // 1000
#elif (FEATURE_LOWPOWER_EN)
#define MESH_NODE_MAX_NUM       105 // no need to many for LPN to save retention RAM.
#elif DEBUG_CFG_CMD_GROUP_AK_EN
#define MESH_NODE_MAX_NUM       305
#else
#define MESH_NODE_MAX_NUM       105 // gateway and node should keep the same, because of mesh command cache..
#endif

/************* this setting is for min by default, not for user  ************/
#if DONGLE_PROVISION_EN         // gateway
	#if MD_REMOTE_PROV
#define MAX_NODE_REMOTE_PROV_NUM	110 //  max num in remote-prov mode for single sector
	#else
#define MAX_NODE_REMOTE_PROV_NUM	200 //  max num in normal mode for single sector
	#endif
	#if (MESH_NODE_MAX_NUM < MAX_NODE_REMOTE_PROV_NUM)
#undef MESH_NODE_MAX_NUM
#define MESH_NODE_MAX_NUM	MAX_NODE_REMOTE_PROV_NUM // don't modify, just keep compatible with the previous version
	#endif
#endif
/************* end ************/

#if (GATEWAY_ENABLE)
#if(DEBUG_CFG_CMD_GROUP_AK_EN)
#define VC_NODE_INFO_MULTI_SECTOR_EN	1		
#else
//enable to use 2 or more sector to save VC_node_info(default use one sector, max support 200 nodes)
#define VC_NODE_INFO_MULTI_SECTOR_EN	0   // if enable, it will use coustom flash area(such as 0x78000 and 0x79000, please check)
#endif
#endif

// ---- message cache setting
#define SIZE_SNO        3

typedef struct{
	u16 src;
	u32 sno     :24;
	u32 ivi     :1;
	u32         :7;
}cache_buf_t;

#if DONGLE_PROVISION_EN // gateway
#define CACHE_BUF_MAX              MESH_NODE_MAX_NUM // should be set to node max
#else
#define CACHE_BUF_MAX              MESH_NODE_MAX_NUM // 8
#endif

extern _align_4_ cache_buf_t cache_buf[];
extern u16 g_cache_buf_max;
void mesh_network_cache_buf_init();

#if DEBUG_CFG_CMD_GROUP_AK_EN
typedef struct{
	u16 unicast;
	u16 sum_num;
	u8 directed_num;
	u8 relay_num;
	u8 mac[2];
}record_info_t;

typedef struct{
	record_info_t record_info[MESH_NODE_MAX_NUM];
	u16 notify_num;
	u16 drop_num;
	u16 total_onoff_st_num;
	u16 total_adv_num;
}nw_notify_record_t;

extern _align_4_ nw_notify_record_t nw_notify_record;
#endif
// ----- cache setting end

extern u8  tbl_mac [6];
#if(MESH_USER_DEFINE_MODE == MESH_MI_SPIRIT_ENABLE)
extern u8  con_mac_address [6];
#endif
extern const u8	const_tbl_advData[22] ;
extern const u8	const_tbl_scanRsp [9] ;
#define ADR_PROV        0x0003
#define ADR_FND1        0x2345
#define ADR_FND2        0x2FE3
#define ADR_LPN1        0x1201
#define ADR_LPN2		0x1210

#define ADR_UNASSIGNED  			(0)
#define ADR_FIXED_GROUP_START 		(0xFF00)
#define ADR_ALL_DIRECTED_FORWARD	(0xFFFB)
#define ADR_ALL_PROXY 				(0xFFFC)
#define ADR_ALL_FRIEND 				(0xFFFD)
#define ADR_ALL_RELAY 				(0xFFFE)
#define ADR_ALL_NODES   			(0xFFFF)
#define SPECIAL_INTERNAL_UNICAST_ADR	0x7f00
#define PROXY_CONFIG_FILTER_DST_ADR 0x0000

#define NODE_ADR_AUTO  		    (0)

//----------------------------------- op code
// op cmd 0xxxxxxx (SIG)
#define APPKEY_ADD		                0x00
#define APPKEY_UPDATE		            0x01
#define COMPOSITION_DATA_STATUS		    0x02
#define CFG_MODEL_PUB_SET		        0x03
#define HEALTH_CURRENT_STATUS		    0x04
#define HEALTH_FAULT_STATUS		        0x05
#define HEARTBEAT_PUB_STATUS 			0x06


// op cmd 10xxxxxx xxxxxxxx (SIG)
// config
#define APPKEY_DEL		                0x0080
#define APPKEY_GET		                0x0180
#define APPKEY_LIST		                0x0280
#define APPKEY_STATUS		            0x0380

// attenttion timer 
#define HEALTH_ATTENTION_GET			0x0480
#define HEALTH_ATTENTION_SET			0x0580
#define HEALTH_ATTENTION_SET_NOACK		0x0680
#define HEALTH_ATTENTION_STATUS			0x0780

#define COMPOSITION_DATA_GET		    0x0880
#define CFG_BEACON_GET		            0x0980
#define CFG_BEACON_SET		            0x0A80
#define CFG_BEACON_STATUS		        0x0B80
#define CFG_DEFAULT_TTL_GET		        0x0C80
#define CFG_DEFAULT_TTL_SET		        0x0D80
#define CFG_DEFAULT_TTL_STATUS		    0x0E80
#define CFG_FRIEND_GET		            0x0F80
#define CFG_FRIEND_SET		            0x1080
#define CFG_FRIEND_STATUS		        0x1180
#define CFG_GATT_PROXY_GET		        0x1280
#define CFG_GATT_PROXY_SET		        0x1380
#define CFG_GATT_PROXY_STATUS		    0x1480
#define CFG_KEY_REFRESH_PHASE_GET	    0x1580
#define CFG_KEY_REFRESH_PHASE_SET		0x1680
#define CFG_KEY_REFRESH_PHASE_STATUS    0x1780
#define CFG_MODEL_PUB_GET               0x1880
#define CFG_MODEL_PUB_STATUS		    0x1980
#define CFG_MODEL_PUB_VIRTUAL_ADR_SET   0x1A80
#define CFG_MODEL_SUB_ADD               0x1B80
#define CFG_MODEL_SUB_DEL		        0x1C80
#define CFG_MODEL_SUB_DEL_ALL		    0x1D80
#define CFG_MODEL_SUB_OVER_WRITE        0x1E80
#define CFG_MODEL_SUB_STATUS		    0x1F80
#define CFG_MODEL_SUB_VIRTUAL_ADR_ADD   0x2080
#define CFG_MODEL_SUB_VIRTUAL_ADR_DEL   0x2180
#define CFG_MODEL_SUB_VIRTUAL_ADR_OVER_WRITE    0x2280
#define CFG_NW_TRANSMIT_GET		        0x2380
#define CFG_NW_TRANSMIT_SET		        0x2480
#define CFG_NW_TRANSMIT_STATUS		    0x2580
#define CFG_RELAY_GET		            0x2680
#define CFG_RELAY_SET		            0x2780
#define CFG_RELAY_STATUS		        0x2880
#define CFG_SIG_MODEL_SUB_GET		    0x2980
#define CFG_SIG_MODEL_SUB_LIST		    0x2A80
#define CFG_VENDOR_MODEL_SUB_GET        0x2B80
#define CFG_VENDOR_MODEL_SUB_LIST       0x2C80
#define CFG_LPN_POLL_TIMEOUT_GET		0x2D80
#define CFG_LPN_POLL_TIMEOUT_STATUS		0x2E80

#define HEALTH_FAULT_CLEAR		        0x2F80
#define HEALTH_FAULT_CLEAR_NOACK        0x3080
#define HEALTH_FAULT_GET		        0x3180
#define HEALTH_FAULT_TEST		        0x3280
#define HEALTH_FAULT_TEST_NOACK         0x3380

#define HEALTH_PERIOD_GET		        0x3480
#define HEALTH_PERIOD_SET		        0x3580
#define HEALTH_PERIOD_SET_NOACK         0x3680
#define HEALTH_PERIOD_STATUS		    0x3780

#define HEARTBEAT_PUB_GET		        0x3880
#define HEARTBEAT_PUB_SET		        0x3980
#define HEARTBEAT_SUB_GET				0x3A80
#define HEARTBEAT_SUB_SET				0x3B80
#define HEARTBEAT_SUB_STATUS			0x3C80

#define MODE_APP_BIND		            0x3D80
#define MODE_APP_STATUS		            0x3E80
#define MODE_APP_UNBIND		            0x3F80
#define NETKEY_ADD		                0x4080
#define NETKEY_DEL		                0x4180
#define NETKEY_GET		                0x4280
#define NETKEY_LIST		                0x4380
#define NETKEY_STATUS		            0x4480
#define NETKEY_UPDATE		            0x4580
#define NODE_ID_GET		                0x4680
#define NODE_ID_SET		                0x4780
#define NODE_ID_STATUS		            0x4880
#define NODE_RESET		                0x4980
#define NODE_RESET_STATUS		        0x4A80
#define SIG_MODEL_APP_GET		        0x4B80
#define SIG_MODEL_APP_LIST		        0x4C80
#define VENDOR_MODEL_APP_GET		    0x4D80
#define VENDOR_MODEL_APP_LIST		    0x4E80

// generic
#define G_ONOFF_GET		                0x0182
#define G_ONOFF_SET		                0x0282
#define G_ONOFF_SET_NOACK		        0x0382
#define G_ONOFF_STATUS		            0x0482

#define G_LEVEL_GET		                0x0582
#define G_LEVEL_SET		                0x0682
#define G_LEVEL_SET_NOACK		        0x0782
#define G_LEVEL_STATUS		            0x0882
#define G_DELTA_SET		                0x0982
#define G_DELTA_SET_NOACK		        0x0A82
#define G_MOVE_SET		                0x0B82
#define G_MOVE_SET_NOACK		        0x0C82

#define G_DEF_TRANS_TIME_GET		    0x0D82
#define G_DEF_TRANS_TIME_SET		    0x0E82
#define G_DEF_TRANS_TIME_SET_NOACK		0x0F82
#define G_DEF_TRANS_TIME_STATUS		    0x1082

#define G_ON_POWER_UP_GET		        0x1182
#define G_ON_POWER_UP_STATUS		    0x1282
#define G_ON_POWER_UP_SET		        0x1382
#define G_ON_POWER_UP_SET_NOACK	        0x1482

#define G_POWER_LEVEL_GET		        0x1582
#define G_POWER_LEVEL_SET		        0x1682
#define G_POWER_LEVEL_SET_NOACK		    0x1782
#define G_POWER_LEVEL_STATUS		    0x1882
#define G_POWER_LEVEL_LAST_GET		    0x1982
#define G_POWER_LEVEL_LAST_STATUS		0x1A82
#define G_POWER_DEF_GET		            0x1B82
#define G_POWER_DEF_STATUS		        0x1C82
#define G_POWER_LEVEL_RANGE_GET		    0x1D82
#define G_POWER_LEVEL_RANGE_STATUS		0x1E82
#define G_POWER_DEF_SET		            0x1F82
#define G_POWER_DEF_SET_NOACK		    0x2082
#define G_POWER_LEVEL_RANGE_SET		    0x2182
#define G_POWER_LEVEL_RANGE_SET_NOACK	0x2282

#define G_BATTERY_GET					0x2382
#define G_BATTERY_STATUS				0x2482

#define G_LOCATION_GLOBAL_GET			0x2582
#define G_LOCATION_GLOBAL_STATUS		0x40
#define G_LOCATION_LOCAL_GET			0x2682
#define G_LOCATION_LOCAL_STATUS			0x2782
#define G_LOCATION_GLOBAL_SET			0x41
#define G_LOCATION_GLOBAL_SET_NOACK		0x42
#define G_LOCATION_LOCAL_SET			0x2882
#define G_LOCATION_LOCAL_SET_NOACK		0x2982

#define CFG_SAR_TRANSMITTER_GET			0x03b8
#define CFG_SAR_TRANSMITTER_SET			0x04b8
#define CFG_SAR_TRANSMITTER_STATUS		0x05b8
#define CFG_SAR_RECEIVER_GET			0x06b8
#define CFG_SAR_RECEIVER_SET			0x07b8
#define CFG_SAR_RECEIVER_STATUS			0x08b8

#define CFG_ON_DEMAND_PROXY_GET			0x00b8
#define CFG_ON_DEMAND_PROXY_SET			0x01b8
#define CFG_ON_DEMAND_PROXY_STATUS		0x02b8

#define CFG_OP_AGG_SEQ					0x09b8
#define CFG_OP_AGG_STATUS				0x10b8 

#define LARGE_CPS_GET					0x11b8
#define LARGE_CPS_STATUS				0x12b8
#define MODELS_METADATA_GET				0x13b8
#define MODELS_METADATA_STATUS			0x14b8

#define SOLI_PDU_RPL_ITEM_CLEAR			0x15b8
#define SOLI_PDU_RPL_ITEM_CLEAR_NACK	0x16b8
#define SOLI_PDU_RPL_ITEM_STATUS		0x17b8
//----------------------------------- status code
#define ST_SUCCESS		                (0)
#define ST_INVALID_ADR		            (1)
#define ST_INVALID_MODEL		        (2)
#define ST_INVALID_APPKEY		        (3)
#define ST_INVALID_NETKEY		        (4)
#define ST_INSUFFICIENT_RES		        (5)
#define ST_KEYIDX_ALREADY_STORE	        (6)
#define ST_INVALID_PUB_PAR		        (7)
#define ST_NOT_SUB_MODEL		        (8)
#define ST_STORAGE_FAIL		            (9)
#define ST_FEATURE_NOT_SUPPORT		    (0x0A)
#define ST_CAN_NOT_UPDATE		        (0x0B)
#define ST_CAN_NOT_REMOVE		        (0x0C)
#define ST_CAN_NOT_BIND		            (0x0D)
#define ST_TEMP_UNABLE_CHANGE_STATE	    (0x0E)
#define ST_CAN_NOT_SET		            (0x0F)
#define ST_UNSPEC_ERR		            (0x10)
#define ST_INVALID_BIND		            (0x11)
#define ST_INVALID_PATH_ENTRY			(0x12)
#define ST_CANNOT_GET					(0x13)
#define ST_OBSOLETE_INFO				(0x14)
#define ST_INVALID_BEARER				(0x15)
//----0x17~0xFF reserve for future use
//----------------------------------- parameter setting
#if WIN32
#define MESH_ELE_MAX_NUM        MESH_NODE_MAX_NUM
#else
    #if (__TL_LIB_8269__ || MCU_CORE_TYPE == MCU_CORE_8269 )
#define MESH_ELE_MAX_NUM        32	// to save RAM
    #else
#define MESH_ELE_MAX_NUM        MESH_NODE_MAX_NUM
    #endif
#endif
#define ADV_INTERVAL_UNIT		(ADV_INTERVAL_10MS)	

#define ADV_INTERVAL_1_2_S		1920 //625*1920 = 1.2s

#if MI_SWITCH_LPN_EN
#define ADV_INTERVAL_MIN		(ADV_INTERVAL_1_2_S)
#define ADV_INTERVAL_MAX		(ADV_INTERVAL_1_2_S)
#elif DU_LPN_EN
#define ADV_INTERVAL_MIN		(DU_ADV_INTER_VAL-(DU_ADV_INTER_VAL/10))
#define ADV_INTERVAL_MAX		(DU_ADV_INTER_VAL+(DU_ADV_INTER_VAL/10))
#elif SPIRIT_PRIVATE_LPN_EN
#define ADV_INTERVAL_MIN		(ADV_INTERVAL_360MS)
#define ADV_INTERVAL_MAX		(ADV_INTERVAL_360MS)
#elif (GATT_LPN_EN || __PROJECT_MESH_SWITCH__)
#define ADV_INTERVAL_MIN		(ADV_INTERVAL_160MS)
#define ADV_INTERVAL_MAX		(ADV_INTERVAL_200MS)
#else
#define ADV_INTERVAL_MIN		(ADV_INTERVAL_UNIT)
#define ADV_INTERVAL_MAX		(ADV_INTERVAL_UNIT)
#endif

#if MI_API_ENABLE
	#if MI_SWITCH_LPN_EN
#define ADV_INTERVAL_MS			(20)   // no random 20ms
#define ADV_INTERVAL_MS_PROVED	(20)
	#elif DU_LPN_EN
#define ADV_INTERVAL_MS			(20)   // no random 40ms
#define ADV_INTERVAL_MS_PROVED	(20)	
	#else
#define ADV_INTERVAL_MS			(105-20)   // with random (0~30ms)
#define ADV_INTERVAL_MS_PROVED	(120-20)   // change it send more frequently 
	#endif
#elif HOMEKIT_EN	
#define ADV_INTERVAL_MS			(ADV_INTERVAL_30MS)
#define ADV_SWITCH_MESH_TIMES 	(4)
#else
#define ADV_INTERVAL_MS			(160)   // with random (0~30ms)
#endif

#define ADV_INTERVAL_RANDOM_MS  (30)

#define GET_ADV_INTERVAL_MS(level)		((level*100)/160)   // level unit: 0.625ms
#define GET_ADV_INTERVAL_LEVEL(ms)		((ms*160)/100)      // level unit: 0.625ms

#if FEATURE_LOWPOWER_EN
#define TRANSMIT_CNT_LPN_CTL_CMD	(3)// control command, because adv retry cnt,
#endif

#if MI_API_ENABLE
#define TRANSMIT_CNT_DEF		(7)
#define TRANSMIT_INVL_STEPS_DEF	(0)	
	#if(AIS_ENABLE)//MI AIS dual mode
	#define AIS_TRANSMIT_CNT_DEF		(5)
	#define AIS_TRANSMIT_INVL_STEPS_DEF	(2)	
	#endif
#elif SPIRIT_PRIVATE_LPN_EN
#define TRANSMIT_CNT_DEF		(7)
#define TRANSMIT_INVL_STEPS_DEF	(0)
#elif DU_ENABLE
#define TRANSMIT_CNT_DEF		(7)
#define TRANSMIT_INVL_STEPS_DEF	(0)
#else
#define TRANSMIT_CNT_DEF		(5)
#define TRANSMIT_INVL_STEPS_DEF	(2)
#endif



#define TRANSMIT_DEF_PAR		(model_sig_cfg_s.nw_transmit.val)
#define TRANSMIT_CNT			(model_sig_cfg_s.nw_transmit.count)
#define TRANSMIT_INVL_STEPS		(model_sig_cfg_s.nw_transmit.invl_steps)
	#if MI_API_ENABLE
#define TRANSMIT_CNT_DEF_RELAY  (2)
#define TRANSMIT_INVL_STEPS_DEF_RELAY	(4)
		#if(AIS_ENABLE)
		#define AIS_TRANSMIT_CNT_DEF_RELAY  (AIS_TRANSMIT_CNT_DEF)
		#define AISTRANSMIT_INVL_STEPS_DEF_RELAY	(AIS_TRANSMIT_INVL_STEPS_DEF)
		#endif
	#else
#define TRANSMIT_CNT_DEF_RELAY          (TRANSMIT_CNT_DEF) // (3)
#define TRANSMIT_INVL_STEPS_DEF_RELAY	(TRANSMIT_INVL_STEPS_DEF) // (0)
	#endif

#define CMD_RAND_DELAY_MAX_MS   (10)
#define CMD_INTERVAL_MS         ((u32)((GET_ADV_INTERVAL_MS(ADV_INTERVAL_UNIT)*(TRANSMIT_INVL_STEPS+1) + CMD_RAND_DELAY_MAX_MS) * (TRANSMIT_CNT + 1)))
#define SEG_GROUP_RX_TIMEOUT_MS (1500)

// sar receiver
#define SAR_SEG_THRESHOLD_DEF					3
#define SAR_ACK_DELAY_INC_DEF					2 // unit:segment transmission interval steps
#define SAR_DISCARD_TIMEOUT_DEF					2 // unit:5s
#define SAR_ACK_RETRANS_CNT_DEF					1
#define SAR_RCV_SEG_INVL_STEP_DEF				8// unit:10ms

#if MD_SAR_EN	
#define SAR_SEG_THRESHOLD						(model_sig_cfg_s.sar_receiver.sar_seg_thres)
#define SAR_RCV_SEG_INVL_STEP_MS				((model_sig_cfg_s.sar_receiver.sar_rcv_seg_invl_step + 1) *10)
#define SAR_ACK_RETRANS_CNT						(model_sig_cfg_s.sar_receiver.sar_ack_retrans_cnt + 1)	// sno will change
#define SAR_ACK_DELAY_MS(seg_N)					(min2(seg_N*10+5, (model_sig_cfg_s.sar_receiver.sar_ack_delay_inc*10 + 15)) * SAR_RCV_SEG_INVL_STEP_MS/10)
#define	SAR_DISCARD_TIMEOUT_MS					(is_seg_block_ack(mesh_rx_seg_par.dst) ? (model_sig_cfg_s.sar_receiver.sar_discard_timeout+1)*5000 : SEG_GROUP_RX_TIMEOUT_MS) // unit:ms
#define SAR_ACK_DELAY_INC						((model_sig_cfg_s.sar_receiver.sar_ack_delay_inc*10+15)/10)
#else
#define SAR_SEG_THRESHOLD						(SAR_SEG_THRESHOLD_DEF)
#define SAR_RCV_SEG_INVL_STEP_MS				((SAR_RCV_SEG_INVL_STEP_DEF+1)*10)
#define SAR_ACK_RETRANS_CNT						(SAR_ACK_RETRANS_CNT_DEF+1)	// sno will change
#define SAR_ACK_DELAY_MS(seg_N)					(min2(seg_N*10+5, (SAR_ACK_DELAY_INC_DEF*10 + 15)) * SAR_RCV_SEG_INVL_STEP_MS/10)
#define	SAR_DISCARD_TIMEOUT_MS					(is_seg_block_ack(mesh_rx_seg_par.dst) ? (SAR_DISCARD_TIMEOUT_DEF + 1)*5000 : SEG_GROUP_RX_TIMEOUT_MS) // unit:ms
#define SAR_ACK_DELAY_INC						((SAR_ACK_DELAY_INC_DEF*10 + 15)/10)
#endif
#define SAR_ACK_RETRANS_INVL_MS					((mesh_rx_seg_par.seg_cnt == (mesh_rx_seg_par.seg_N+1))?(SAR_RCV_SEG_INVL_STEP_MS*SAR_ACK_DELAY_INC):SAR_RCV_SEG_INVL_STEP_MS)
#if MI_API_ENABLE
#define SEG_RX_ACK_IDLE_MS						200
#else
#define SEG_RX_ACK_IDLE_MS						((mesh_rx_seg_par.seg_ack_cnt == SAR_ACK_RETRANS_CNT)? SAR_ACK_DELAY_MS(mesh_rx_seg_par.seg_N):SAR_ACK_RETRANS_INVL_MS)
#endif
// sar transmitter
#define SAR_SEG_INVL_STEP_DEF						1 // unit:10ms
#define SAR_UNICAST_RETRANS_CNT_DEF					7 // max 4bit	
#define SAR_UNICAST_RETRANS_INVL_STEP_DEF			7 // unit:25ms
#define SAR_UNICAST_RETRANS_CNT_NO_ACK_DEF			SAR_UNICAST_RETRANS_CNT_DEF //(SAR_ACK_RETRANS_CNT_DEF + 3) // 2 or more than sar ack cnt
#define SAR_UNICAST_RETRANS_INVL_INCRE				1 // unit:25ms			
#define SAR_MULTICAST_RETRANS_CNT_DEF				0
#define SAR_MULTICAST_RETRANS_INVL_DEF				3 // unit:25ms

#if MD_SAR_EN
#define SAR_SEG_INVL_STEP_MS					((model_sig_cfg_s.sar_transmitter.sar_seg_invl_step+1)*10)
#define SAR_UNICAST_RETRANS_CNT					(model_sig_cfg_s.sar_transmitter.sar_uni_retrans_cnt)
#define SAR_UNICAST_RETRANS_CNT_NO_ACK			(model_sig_cfg_s.sar_transmitter.sar_uni_retrans_cnt_no_ack)
#define SAR_UNICAST_RETRANS_INVL_STEP_MS		((model_sig_cfg_s.sar_transmitter.sar_uni_retrans_invl_step + 1) * 25)
#define SAR_UNICAST_RETRANS_TIME_MS				((model_sig_cfg_s.sar_transmitter.sar_uni_retrans_invl_step + 1) * 25 + (model_sig_cfg_s.ttl_def ? ((model_sig_cfg_s.sar_transmitter.sar_uni_retrans_invl_incre+1)*25*(model_sig_cfg_s.ttl_def-1)) : 0)	)
#define SAR_MULTICAST_RETRANS_CNT				(model_sig_cfg_s.sar_transmitter.sar_multi_retrans_cnt)
#define SAR_MULTICAST_RETRANS_INVL				((model_sig_cfg_s.sar_transmitter.sar_multi_retrans_invl + 1) * 25)
#else
#define SAR_SEG_INVL_STEP_MS					((SAR_SEG_INVL_STEP_DEF+1)*10)
#define SAR_UNICAST_RETRANS_CNT					(SAR_UNICAST_RETRANS_CNT_DEF)
#define SAR_UNICAST_RETRANS_CNT_NO_ACK			(SAR_UNICAST_RETRANS_CNT_NO_ACK_DEF)
#define SAR_UNICAST_RETRANS_INVL_STEP_MS		((SAR_UNICAST_RETRANS_INVL_STEP_DEF + 1) * 25)
#define SAR_UNICAST_RETRANS_TIME_MS				(SAR_UNICAST_RETRANS_INVL_STEP_MS + (SAR_UNICAST_RETRANS_INVL_INCRE+1)*25*(5-1)) // (5-1):5 is ttl
#define SAR_MULTICAST_RETRANS_CNT				(SAR_MULTICAST_RETRANS_CNT_DEF)
#define SAR_MULTICAST_RETRANS_INVL				((SAR_MULTICAST_RETRANS_INVL_DEF + 1) * 25)
#endif

#define SEG_TX_ACK_WAIT_MS     					(is_unicast_adr(mesh_tx_seg_par.match_type.mat.adr_dst) ? SAR_UNICAST_RETRANS_TIME_MS:SAR_MULTICAST_RETRANS_INVL) // should be( >= 200 + 50*TTL)

#define PUBLISH_RETRANSMIT_CNT			(TRANSMIT_CNT_DEF)
#define PUBLISH_RETRANSMIT_INVL_STEPS	(0) // 0 should be better:retransmission interval = (Publish Retransmit Interval Steps + 1) * 50ms

#define TRANSMIT_DEF_PAR_BEACON	(TRANSMIT_DEF_PAR)

#define REPEATE_CNT_UNRELIABLE  (1)		// use network retransmit instead.

#define ALONE_BUFF_FOR_RELAY_EN (1)

#define MESH_ADV_CMD_BUF_CNT    (8)
#if WIN32 
#define MESH_ADV_BUF_RELAY_CNT  (8)
#else
	#if (MCU_CORE_TYPE == CHIP_TYPE_8269 )
		#define MESH_ADV_BUF_RELAY_CNT  (8)
	#else
		#define MESH_ADV_BUF_RELAY_CNT  (8)
	#endif
#endif

#define TTL_TEST_ACK            (0x0b)
#if MI_API_ENABLE
#define TTL_DEFAULT             (5)
#else
#define TTL_DEFAULT             (10)         // max relay count = TTL_DEFAULT - 1
#endif
#define TTL_MAX                 (0x7F)
#define TTL_PUB_USE_DEFAULT     (0xFF)

#define SEC_NW_BC_BROADCAST_DEFAULT     (NW_BEACON_BROADCASTING)	// security network beacon

#define NODE_IDENTITY_TIMEOUT_S         (60)    // should be less than 60 second by spec.

#define MESH_ADV_TYPE_PRO	    (0x29)
#define MESH_ADV_TYPE_MESSAGE   (0x2A)
#define MESH_ADV_TYPE_BEACON 	(0x2B)

#define MESH_ADV_TYPE_ONLINE_ST 	    (0x62)  // can't use BIT7 now, because is use for response delay flag.
#define MESH_ADV_TYPE_TESTCASE_ID (0xF7)

#define MESH_ADV_TYPE_PROXY_FLAG 		(0xE0)  // for FLAG
#define MESH_ADV_TYPE_PROXY_CFG_FLAG 	(0xE1)  // for FLAG

// --- 
enum{
    PREPARE_HANDLE_ADV_LEGACY   = 1,
    PREPARE_HANDLE_ADV_EXTEND   = 2,    // use type of AUX EXTENDED ADV to send.
};

// ---
#define MESH_CMD_ACCESS_LEN_MAX (380)   //  (128)
#define ACCESS_WITH_MIC_LEN_MAX (MESH_CMD_ACCESS_LEN_MAX + 4)

#define SZMIC_TRNS_UNSEG        (4)
#define SZMIC_TRNS_CONTROL      (0) // no transport MIC and security for control pkt in upper transport layer
#define SZMIC_TRNS_SEG32        (4)
#define SZMIC_TRNS_SEG64        (8)

#define SZMIC_NW32              (4)
#define SZMIC_NW64              (8)

#define SZMIC_SEG_FLAG          (0)     // 0 means to use SZMIC_TRNS_SEG32, 1 means to use SZMIC_TRNS_SEG64.
#define GET_SEG_SZMIC(szmic)    (szmic ? SZMIC_TRNS_SEG64 : SZMIC_TRNS_SEG32)

enum{
	MASTER, 
	FRIENDSHIP,
	DIRECTED,
};

enum{
	SEG_RX_STATE_ING		= 0,
	SEG_RX_STATE_COMPLETED	= 1,
	SEG_RX_STATE_TIMEOUT	= 2,
};

enum{
    SWAP_TYPE_NONE = 0,
    SWAP_TYPE_NW = 1,               // just sno/src/dst
    SWAP_TYPE_LT_UNSEG,             // no swap
    SWAP_TYPE_LT_SEG,
    SWAP_TYPE_LT_CTL_UNSEG,         // no swap
    SWAP_TYPE_LT_CTL_SEG,
    SWAP_TYPE_LT_CTL_SEG_ACK,
};

enum{
    DST_MATCH_NONE = 0, // must 0
    DST_MATCH_MAC,
    DST_MATCH_GROUP,
    DST_MATCH_VIRTUAL,
    DST_MATCH_FIXED_GROUP,	// include all nodes
};

enum{
	GENERIC_ERROR_FLAG = -1,
	SEND2PROXY_CLIENT_FLAG = -2,
};

enum{
	TX_ERRNO_SUCCESS					= 0,
	TX_ERRNO_DEV_OR_APP_KEY_NOT_FOUND	= 1,/* device key or app key not found */
	TX_ERRNO_GET_UT_TX_BUF_FAIL			= 2,/* get the upper layer tx buffer fail */
	TX_ERRNO_ADDRESS_INVALID			= 3,/* source address or destination address invalid */
	TX_ERRNO_PAR_LEN_OVER_FLOW			= 4,/* parameters length > 378 */
	TX_ERRNO_TX_BUSY					= 5,/* segment busy, reliable busy,... */
	TX_ERRNO_TX_FIFO_FULL				= 6,/*  */
	TX_ERRNO_ALL_OTHER_ERR 				= -1,/* default error */
};

typedef struct{
	u8 par_type;	// bear_tx_par_type_t // it's better that the first byte is not a variable in bit filed format.
	u8 val[3];			// value for par_type
}bear_head_t;


typedef struct{
	union{
		u8 *par;        // it is used before mesh_tx_cmd2_access_()
		u8 *p_ac;       // it is used after mesh_tx_cmd2_access_()
	};
	union{
		u32 par_len;    // it is used before mesh_tx_cmd2_access_()
		u32 len_ac;     // it is used after mesh_tx_cmd2_access_()
	};
	u16 adr_src;
	u16 adr_dst;
	u8* uuid;           // uuid of virtual address
	model_common_t *pub_md;
	u32 rsp_max;
	u32 op_rsp;     // include vendor id
	bear_head_t tx_head;
	u16 op;
	u8 immutable_flag;  // immutable-credentials tag which indicate whether or not to changing key between master and DF.
	u8 not_use_extend_adv_flag; // active only when support extend adv
	u8 nk_array_idx;
	u8 ak_array_idx;
	u8 seg_must;   // 1:  force use segment flow to tx short messages.
	u8 retry_cnt;
	u8 tid_pos_vendor_app;
}material_tx_cmd_t;

typedef struct{
	u8* model;
	u32 id;
	material_tx_cmd_t mat;
	u8 sig;
	u8 filter_cfg;
	mesh_transmit_t trans;
    union{
        u32 val;
        struct{
        	u8 type;
        	u8 local;
        	u16 F2L;	// read by bit
        };
    };
}mesh_match_type_t;

typedef struct{
	u8 ele_idx;
	u8 rsp_max;
	u16 op;
	u8 par[MESH_CMD_ACCESS_LEN_MAX];
}mesh_bulk_cmd2model_t;

typedef struct{
	u16 nk_idx;
	u16 ak_idx;
	u8 retry_cnt;   // only for reliable command // for op "distribute start" of gateway mesh OTA, it is reliable retry interval for LPN. // retry_intv_for_lpn_100ms
	u8 rsp_max;     // only for reliable command
	u16 adr_dst;
	u8 op;
	u8 par[MESH_CMD_ACCESS_LEN_MAX];
}mesh_bulk_cmd_par_t;

typedef struct{
	u16 nk_idx;
	u16 ak_idx;
	u8 retry_cnt;   // only for reliable command
	u8 rsp_max;     // only for reliable command
	u16 adr_dst;
	u8 op;
	u16 vendor_id;
	u8 op_rsp; // expected response opcode, 0 means not_ack cmd
	u8 tid_pos; // par[tid_pos-1] is tid, 0 means not tid
	u8 par[MESH_CMD_ACCESS_LEN_MAX-5];
}mesh_bulk_vd_cmd_par_t;

typedef struct{
	u8 op;
	u16 vendor_id;
	u8 op_rsp;
	u8 tid_pos; // n+1: 00 means no tid, 01 means the least byte
	u8 par[MESH_CMD_ACCESS_LEN_MAX];
}mesh_vendor_par_ini_t;

typedef struct{
    u8 type;
    u8 par[MESH_CMD_ACCESS_LEN_MAX];
}mesh_bulk_set_par_t;

//----------------------------------- access layer
#define OP_TYPE_SIG1 		1
#define OP_TYPE_SIG2 		2
#define OP_TYPE_VENDOR 		3

#define SIZE_OF_OP(op)	((op & BIT(7)) ? ((op & BIT(6)) ? OP_TYPE_VENDOR : OP_TYPE_SIG2) : OP_TYPE_SIG1)
#define GET_OP_TYPE(op)	(SIZE_OF_OP(op))
#define IS_VENDOR_OP(op)    (GET_OP_TYPE(op) == OP_TYPE_VENDOR)

enum{
    GET_OP_FAILED           = 0,
    GET_OP_SUCCESS_NORMAL,
    GET_OP_SUCCESS_EXTEND,
};

u16 get_op_u16(u8 *p_op);

typedef struct{
	u8 op;
    u8 data[ACCESS_WITH_MIC_LEN_MAX - 1];    // max: payload 0 ~ 379Byte
}mesh_cmd_ac_sig1_t;

typedef struct{
	u16 op;
    u8 data[ACCESS_WITH_MIC_LEN_MAX - 2];    // max: payload 0 ~ 378Byte
}mesh_cmd_ac_sig2_t;

typedef struct{
	u8 op;
	u16 vd_id;
    u8 data[ACCESS_WITH_MIC_LEN_MAX - 3];    // max: payload 0 ~ 377Byte
}mesh_cmd_ac_vd_t;


//----------------------------------- upper transport layer
typedef struct{
    u8 data[15];    // max: payload + TransportMIC(4)
}mesh_cmd_ut_unseg_t;

typedef struct{
    u8 data[1];    // max: payload + TransportMIC:  (1--(380+4)Byte) // can not use to creat a object, just for rx
}mesh_cmd_ut_seg_t;

typedef struct{
	union{
		struct{
			union{
				struct{		// big endian, for control and proxy pdu
					u16 range_start_b:15;	
					u16 length_present_b:1;		
				};

				struct{		// little endian, for access pdu
					u16 length_present_l:1;
					u16 range_start_l:15;				
				};
			};
			u8 	range_length;
		};
		u16 multicast_addr;
	};
}addr_range_t;


//----------------------------------- lower transport layer
// max size of segmented or unsegmented message is 16Byte
typedef struct{
	u8 aid  :6;
	u8 akf  :1;
	u8 seg  :1;
    u8 data[15];    // max: payload + TransportMIC(4)
}mesh_cmd_lt_unseg_t;

#define ACCESS_WITH_MIC_LEN_MAX_UNSEG   (sizeof(mesh_cmd_lt_unseg_t) - 1)   // 15
#define ACCESS_NO_MIC_LEN_MAX_UNSEG     (ACCESS_WITH_MIC_LEN_MAX_UNSEG - SZMIC_TRNS_UNSEG) // 11

//---------- iv update
#define IV_UPDATE_SKIP_96HOUR_EN        (TESTCASE_FLAG_ENABLE || PTS_TEST_EN)

#if IV_UPDATE_SKIP_96HOUR_EN
#define SEC_NW_BC_INV_DEF_100MS       	(20)	// security network beacon interval default
#else
	#if (MI_API_ENABLE || SPIRIT_PRIVATE_LPN_EN)
#define SEC_NW_BC_INV_DEF_100MS       	(200)   // set the mi secure beacon to 20s	
	#else
#define SEC_NW_BC_INV_DEF_100MS       	(100)	// between 100~6000(unit:100ms), security network beacon interval default
	#endif
#endif

typedef struct{
	u32 aid  :6;
	u32 akf  :1;
	u32 seg  :1;
	u32 segN    :5;     // need little endianness to big: from segN to szmic (total 24bit)
	u32 segO    :5;
	u32 seqzero :13;
	u32 szmic   :1;
    u8 data[12];    // max: payload + (TransportMIC(4/8))
}mesh_cmd_lt_seg_t;

// max size of control message is 12Byte
typedef struct{
	u8 opcode   :7;
	u8 seg      :1;
    u8 data[11];    // max: payload (no TransportMIC)
}mesh_cmd_lt_ctl_unseg_t;

typedef struct{
	u32 opcode  :7;
	u32 seg     :1;
	u32 segN    :5;     // need little endianness to big: from segN to szmic (total 24bit)
	u32 segO    :5;
	u32 seqzero :13;
	u32 rfu     :1;
    u8 data[8];    // max: payload (no TransportMIC)
}mesh_cmd_lt_ctl_seg_t;

typedef struct{
	u8 opcode   :7;
	u8 seg      :1;
	u16 rfu     :2;     // need little endianness to big: from rfu to obo (total 16bit)
	u16 seqzero :13;
	u16 obo     :1;     // Friend on behalf of a Low Power node
	u8 seg_map[4];
}mesh_cmd_lt_ctl_seg_ack_t;

//----------------------------------- network layer
typedef struct{     // need little endianness to big
	u8 nid  :7;
	u8 ivi  :1;
	u8 ttl  :7;
	u8 ctl  :1;
    u8 sno[3];
    u16 src;
    u16 dst;
    u8 data[20];    // max: payload + NeworkMIC(4/8)
}mesh_cmd_nw_t;

typedef struct{
    u8 type;
    u8 data[28];
}mesh_beacon_t;	// max size : 31 - 2 

typedef struct{
	u8 nid  :7;
	u8 nid2 :1;
    u8 data[28];
}online_st_adv_t;	// max size : 31 - 2 

//----------------------------------- bearer layer (fifo)
typedef enum{
	BEAR_TX_PAR_TYPE_NONE		= 0,	// must 0
	BEAR_TX_PAR_TYPE_PUB_FLAG	= 1,
	BEAR_TX_PAR_TYPE_DELAY		= 2,
	BEAR_TX_PAR_TYPE_REMAINING_TIMES = 3,
	// user type from 0x80 to 0xff
	BEAR_TX_PAR_TYPE_USER_1		= 0x80,
}bear_tx_par_type_t;

typedef struct{
	bear_head_t tx_head;	// parameters for tx bear that can be read in app_advertise_prepare_handler_()
    u8 trans_par_val;	// report_type
    u8 len;
    u8 type;    // adv type
    union{
        mesh_cmd_nw_t nw;
        struct{
            u8 nw_pre[9];    // 9 = OFFSETOF(mesh_cmd_nw_t, data)
            union{
                // message
                mesh_cmd_lt_unseg_t lt_unseg;
                mesh_cmd_lt_seg_t lt_seg;
                struct{
                    u8 lt_pre_unseg[1];    // 1 = OFFSETOF(mesh_cmd_lt_unseg_t, data)
                    mesh_cmd_ut_unseg_t ut_unseg;
                };
                struct{
                    u8 lt_pre_seg[4];   // 4 = OFFSETOF(mesh_cmd_lt_seg_t, data)
                    mesh_cmd_ut_seg_t ut_seg;
                };
                // control
                mesh_cmd_lt_ctl_unseg_t lt_ctl_unseg;
                mesh_cmd_lt_ctl_seg_t lt_ctl_seg;
                mesh_cmd_lt_ctl_seg_ack_t lt_ctl_ack;
            };
        };
        mesh_beacon_t beacon;
        online_st_adv_t online_st_adv;
    };
}mesh_cmd_bear_t;


#define GET_ALIGN_CEIL(A, B)        ((((A) + (B) - 1) / (B)) * B)

#define ADV_EXTEND_PAYLOAD_MAX      (255 - (6 + 4)) //  6: mac; 4: extend header;
#define BEAR_EXTEND_MAX             (ADV_EXTEND_PAYLOAD_MAX + OFFSETOF(mesh_cmd_bear_t, len))

#define CONST_DELTA_EXTEND_AND_NORMAL   (ADV_EXTEND_PAYLOAD_MAX - 31)

#if MESH_DLE_MODE
#define DLE_RX_FIFO_SIZE            GET_ALIGN_CEIL(DLE_LEN_MAX_RX + 24, 16) //max: 288
#define DLE_TX_FIFO_SIZE            GET_ALIGN_CEIL(DLE_LEN_MAX_TX + 12, 4)  //max: 264
#else
#define DLE_RX_FIFO_SIZE            0   // use 0 to show compile warning
#define DLE_TX_FIFO_SIZE            0   // use 0 to show compile warning
#endif

#if WIN32
#define GET_DELTA_EXTEND_BEAR       (isVC_DLEModeExtendBearer() ? CONST_DELTA_EXTEND_AND_NORMAL : 0)
#define MESH_BEAR_SIZE              (sizeof(mesh_cmd_bear_t) + CONST_DELTA_EXTEND_AND_NORMAL) // buffer default support DLE length
#else
#define GET_DELTA_EXTEND_BEAR       (tx_bear_extend_en ? CONST_DELTA_EXTEND_AND_NORMAL : 0) // used in library
#define MESH_BEAR_SIZE              (sizeof(mesh_cmd_bear_t) + GET_DELTA_EXTEND_BEAR)
#endif

#define MESH_UNSEGMENT_MAX_SIZE     (ACCESS_WITH_MIC_LEN_MAX_UNSEG + GET_DELTA_EXTEND_BEAR)

#if ((MESH_DLE_MODE == MESH_DLE_MODE_EXTEND_BEAR)&&(!GATT_LPN_EN))
#define DELTA_EXTEND_AND_NORMAL_ALIGN4_BUF  GET_ALIGN_CEIL(CONST_DELTA_EXTEND_AND_NORMAL, 4)
#else
#define DELTA_EXTEND_AND_NORMAL_ALIGN4_BUF  0
#endif



typedef struct{
    union{
        u16 valid;
        struct{
            u8 cnt;
            u8 tick_10ms;
        };
    };
    u8 rsv[2];
    mesh_cmd_bear_t bear;
}mesh_relay_buf_t;

typedef struct{
	u16 src;
	u16 dst;
	u8 data[12];
}ut_log_t;

typedef struct{
	u16 len;
	u16 src;
	u16 dst;
	u8 data[ACCESS_WITH_MIC_LEN_MAX];	// access layer(op code, parameters)
}mesh_rc_rsp_t;

typedef struct{
    u32 tick;
    u16 invl_ms;
    u8 busy;
    u8 retry_cnt;
    material_tx_cmd_t mat;
    u32 rsp_cnt;		// current received
    mesh_match_type_t match_type;
    u8 ac_par[ACCESS_WITH_MIC_LEN_MAX];  // if want to reuse mesh_cmd_ut_tx_seg, unsegment or unreliable cmd is forbidden when reliable flow.
}mesh_tx_reliable_t;

typedef struct{
    u16 op;
    u16 src;
}status_record_t;

extern _align_4_ status_record_t slave_status_record[];
#if DEBUG_CFG_CMD_GROUP_AK_EN
	#if WIN32
#define RELIABLE_RETRY_CNT_DEF              2
	#else
#define RELIABLE_RETRY_CNT_DEF              2
	#endif
#else
#define RELIABLE_RETRY_CNT_DEF              2
#endif
#define RELIABLE_RETRY_CNT_MAX              20
extern _align_4_ mesh_rc_rsp_t slave_rsp_buf[];
extern _align_4_ mesh_tx_reliable_t mesh_tx_reliable;

int mesh_notifyfifo_rxfifo();
void rf_link_slave_read_status_start ();
void rf_link_slave_read_status_stop ();
void rf_link_slave_read_status_update ();
int	rf_link_slave_add_status (mesh_rc_rsp_t *p_rsp, u16 op);
int mesh_rsp_handle(mesh_rc_rsp_t *p_rsp);
int is_busy_tx_seg(u16 adr_dst);
int is_busy_reliable_cmd(u16 adr_dst);
u8 * get_ut_tx_buf(material_tx_cmd_t *p, u8 *ac2self, u32 ac2self_max, int seg_must);

int is_unicast_adr(u16 adr);
int is_virtual_adr(u16 adr);
int is_group_adr(u16 adr);
static inline int is_seg_block_ack(u16 adr_dst)
{
    return is_unicast_adr(adr_dst);
}

//----------------------------------- segment parameters
typedef enum{
	SEG_TX_DST_TYPE_NORMAL = 0,
	SEG_TX_DST_TYPE_GATT_ONLY,
	SEG_TX_DST_TYPE_LPN,
}mesh_tx_seg_dst_type;	// mesh TX segment destination address type.

typedef struct{
	u32 timeout;
	u32 tick_wait_ack;
	u32 tick_irq_ev_one_pkt_completed; // don't use for check next round, because time cost of one packet should be too long without refresh tick and then cause next round sending.
	u32 seg_map_rec;
	u32 len_ut;
	mesh_match_type_t match_type;
	u16 time_cost_last_transmit_ms;
	u16 seqzero     :13;
	u16 rfu         :3;
	u8 tx_segO_next;
	u8 retrans_cnt;
	u8 unicast_retrans_cnt_no_ack; 
	u8 cmd_cnt_in_buf;  // get the command count in the buffer when push the last segment packet.
	u8 ack_received;
	u8 ctl;
	u8 busy;
	u8 local_lpn_only;
}mesh_tx_seg_par_t;

typedef struct{
	u32 seg_map;
	u32 tick_last;		// tick for the latest rx.
	u32 tick_seg_idle;	// tick for block ack retry
	u32 seqAuth;	//record in cache_buf, if needed.  just sno, not include iv index now, initialize seqAuth when iv update
	u32 len_ut_total;
	u16 src;
	u16 dst;
	u16 rfu         :2;
	u16 seqzero     :13;
	u16 obo         :1;
	u8 nid  :7;         // nw par
	u8 ivi  :1;
	u8 nk_array_idx;    // decryption key
	u8 len_ut_max_single_seg;
	u8 par_saved;
	u8 status;
	u8 seg_cnt;
	u8 seg_N;
	u8 seg_ack_cnt;
	// u8 busy;	// use "is_busy_rx_segment_flow" to indicate busy.
}mesh_rx_seg_par_t;

#if RX_SEGMENT_REJECT_CACHE_EN
typedef struct{
	u16 src;
	u32 seqAuth;
	u32 tick;
}rx_seg_reject_cache_t;

#define RX_SEG_REJECT_CACHE_MAX				(16)	// user can redefine
#define RX_SEG_REJECT_CACHE_TIMEOUT_MS		(3000)	// user can redefine
#endif

extern _align_4_ mesh_tx_seg_par_t mesh_tx_seg_par;
extern _align_4_ mesh_rx_seg_par_t mesh_rx_seg_par;
void mesh_seg_tx_set_one_pkt_completed(mesh_tx_seg_dst_type dst_type);

//----------------------------------- control packet
// LPN send(publish) message on its own(exclude ACK), use key depend on Publish Friendship Credentials Flag
#define CMD_CTL_ACK                 (0x00)
#define CMD_CTL_POLL                (0x01)	//using the friendship security credentials in V1.0
#define CMD_CTL_UPDATE              (0x02)	//using the friendship security credentials in V1.0
#define CMD_CTL_REQUEST             (0x03)	//using the master security credentials in V1.0
#define CMD_CTL_OFFER               (0x04)	//using the master security credentials in V1.0
#define CMD_CTL_CLEAR               (0x05)	//using the master security credentials in V1.0
#define CMD_CTL_CLR_CONF            (0x06)	//using the master security credentials in V1.0
#define CMD_CTL_SUBS_LIST_ADD       (0x07)	//using the friendship security credentials in V1.0
#define CMD_CTL_SUBS_LIST_REMOVE    (0x08)	//using the friendship security credentials in V1.0
#define CMD_CTL_SUBS_LIST_CONF      (0x09)	//using the friendship security credentials in V1.0
#define CMD_CTL_HEARTBEAT			(0x0a)
#define CMD_CTL_PATH_REQUEST				(0x0b)
#define CMD_CTL_PATH_REPLY					(0x0c)
#define CMD_CTL_PATH_CONFIRMATION			(0x0d)
#define CMD_CTL_PATH_ECHO_REQUEST			(0x0e)
#define CMD_CTL_PATH_ECHO_REPLY				(0x0f)
#define CMD_CTL_DEPENDENT_NODE_UPDATE		(0x10)
#define CMD_CTL_PATH_REQUEST_SOLICITATION	(0x11)

#define CMD_ST_NORMAL_TX         	(0xFFF0)
//#define CMD_ST_NORMAL_SEG         (0xFFF1)
#define CMD_ST_SLEEP                (0xFFF2)
#define CMD_ST_POLL_MD              (0xFFF3)

#define FN_RSSI_INVALID             (0x7F)

#define FRI_REC_DELAY_MS            (FRI_ESTABLISH_REC_DELAY_MS + 50)	// larger than FRI_ESTABLISH_REC_DELAY_MS should be better
#if WIN32
#define FRI_REC_WIN_MS              (0xff)  // WIN32 can't not response at once.
#else
#define FRI_REC_WIN_MS              (20)    // (0xfa) // (0x32) // if there is many LPN, set longer should be better.
#endif
#define FRI_REC_WIN_FAC             (1)
#define FRI_REC_RSSI_FAC            (2)

#define FRI_REQ_RETRY_MAX           (0)     // not use this flow now
#define FRI_GET_UPDATE_RETRY_MAX    (4)     // should not set too much because FN only listern 1s after send offer.
#define FRI_POLL_RETRY_MAX          (8)
#define FRI_POLL_DELAY_FOR_MD_MS    (100) // (500)	// delay for next poll when receive a 'md' pkt.

#define FRIEND_KEY_NO_SECU_FLAG     (0x7F)

#define SUB_LIST_MAX_LPN            (5)     //  max contain 5 adr in one unsegment control message
#define SUB_LIST_MAX_IN_ONE_MSG    	(5)
#if PTS_TEST_EN
#define FN_CACHE_SIZE_LOG           (3)		//  FN BV08 required
#else
#define FN_CACHE_SIZE_LOG           (2)
#endif
#define LPN_CACHE_OFFER_MAX         (8)
#define FRI_ESTABLISH_REC_DELAY_MS  (100)	// must 100ms, define by spec
#if (MESH_USER_DEFINE_MODE == MESH_GN_ENABLE)
#define FRI_ESTABLISH_WIN_MS        (50)
#else
#define FRI_ESTABLISH_WIN_MS        (1000)	// must 1000ms, define by spec
#endif
#define FRI_ESTABLISH_PERIOD_MS     (FRI_ESTABLISH_REC_DELAY_MS + FRI_ESTABLISH_WIN_MS)
#define FRI_ESTABLISH_OFFER_MS      (1000+100) //FN use only, spec define, don't modify //
#define FRI_FIRST_CLR_INTERVAL_100MS	(10)
#define FRI_LPN_WAIT_FN_NW_TRANSMIT_MS	(200)	// 
#define FRI_LPN_WAIT_SEG_ACK_MS     (500)	// 

#define LPN_MIN_CACHE_SIZE_LOG      (1)     // at lease 2 messages
#define LPN_POLL_TIMEOUT_100MS      (10*10) // unit: 100ms, 0x0A~0x34BBFF
#define FRI_POLL_INTERVAL_MS        (2000)	// auto send poll interval
#define FRI_POLL_INTERVAL_MS_MESH_OTA	(400)		// used when mesh ota actived

#define FRI_REQ_TIMEOUT_MS          (2000)  // must larger than 1100ms
#define FRI_REQ_RETRY_IDLE_MS       (FRI_REQ_TIMEOUT_MS - FRI_ESTABLISH_PERIOD_MS)	// auto trigger next FRI_REQ_RETRY_MAX request interval 

#define LPN_SCAN_PROVISION_START_TIMEOUT_MS (60*1000)   // from power up to this time, if didn't receive provision start command, LPN will auto enter sleep.
#define LPN_WORKING_TIMEOUT_MS   	        (60*1000)   // Prevent abnormal working time.
#define LPN_START_REQUEST_AFTER_BIND_MS     (GATT_LPN_EN ? (6*1000) : (3*1000))    // 

#define LPN_ADV_EN                  0
#define LPN_ADV_INTERVAL_MS         (2000)
#define LPN_ADV_INTERVAL_EN         0 // (LPN_ADV_INTERVAL_MS <= FRI_REQ_TIMEOUT_MS /2) // if enable, it would be some err when BLE disconnect, comfirm later.

enum{
    FRI_ST_IDLE = 0,
    FRI_ST_REQUEST,
    FRI_ST_OFFER,
    FRI_ST_POLL,
    FRI_ST_UPDATE,
    FRI_ST_TIMEOUT_CHECK,
};

typedef struct{
    u8 flag;
    u16 LPNAdr;
    u16 FriAdr;
    u16 LPNCounter;
    u16 FriCounter;
}mesh_fri_sec_par_t;

typedef struct{
    u16 LPNAdr;
    u16 FriAdr;
    my_fifo_t *p_cache;    // cache total bearer.
    u16 SubsList[SUB_LIST_MAX_LPN];
    u8 nk_sel_dec_fn;
    u8 TransNo;
	u8 cache_overwrite;
    //u8 cache_non_replace_cnt; // for segment,  not use now
    u8 link_ok;
}mesh_fri_ship_other_t;

typedef struct{
    u32 req_tick;
    u32 poll_tick;
    u32 poll_retry;
    u8 status;
    u8 req_retrys;
}mesh_fri_ship_proc_lpn_t;

typedef struct{
    u32 offer_tick;
    u32 offer_delay;
    u32 poll_tick;          // unit: 100ms
    u32 clear_by_lpn_tick;  // clear by LPN
    u32 clear_start_tick;
    u32 clear_cmd_tick;
    u32 clear_int_100ms;    // clear command interval
    u8 clear_poll;          // clear by other FN
    u8 clear_delay_cnt;
    u8 status;
}mesh_fri_ship_proc_fn_t;

typedef struct{
	u8 FSN  :1;
	u8 RFU  :7;
}mesh_ctl_fri_poll_t;

typedef struct{
	u8 KeyRefresh   :1;
	u8 IVUpdate     :1;
	u8 RFU          :6;
}mesh_ctl_fri_update_flag_t;

typedef struct{
	mesh_ctl_fri_update_flag_t flag;
	u8 IVIndex[4];
	u8 md;
}mesh_ctl_fri_update_t;

typedef struct{
	u8 MinCacheSizeLog  :3;     // 1~7: 2^n
	u8 RecWinFac        :2;     // 0b00: *1, 0b01: *1.5, 0b10: *2, 0b11: *2.5, 
	u8 RSSIFac          :2;     // 0b00: *1, 0b01: *1.5, 0b10: *2, 0b11: *2.5, 
	u8 RFU              :1;
}mesh_ctl_fri_req_cri_t;

typedef struct{
	mesh_ctl_fri_req_cri_t Criteria;
	u32 RecDelay		:8;			// unit: 1ms, 0x0A~0xFF
	u32 PollTimeout     :24;        // unit: 100ms, 0x0A~0x34BBFF // max:345599s = 96hour
	u16 PreAdr;
	u8 NumEle;						// 1~255, calculate the range of unicast addresses assigned to LPN
	u16 LPNCounter;
}mesh_ctl_fri_req_t;

typedef struct{
	u8 RecWin;          // 1~255; unit: 1ms
	u8 CacheSize;       // larger than MinCacheSizeLog
	u8 SubsListSize;    // 
	s8 RSSI;
	u16 FriCounter;
}mesh_ctl_fri_offer_t;

typedef struct{
	u16 LPNAdr;
	u16 LPNCounter;
}mesh_ctl_fri_clear_t;

typedef struct{
	u16 LPNAdr;
	u16 LPNCounter;
}mesh_ctl_fri_clear_conf_t;

typedef struct{
    u16 adr_fri;
	mesh_ctl_fri_offer_t offer;
}mesh_lpn_offer_buf_t;

enum{
    SUBSC_ADD = 1,
    SUBSC_REMOVE,
};

typedef struct{
	u16 adr[SUB_LIST_MAX_LPN];
}lpn_adr_list_t;

typedef struct{
    u8 rsv;         // for align
    u8 TransNo;
	u16 adr[SUB_LIST_MAX_IN_ONE_MSG];
}mesh_ctl_fri_subsc_list_t;

typedef struct{
    u8 rsv;         // for align
    u8 TransNo;
	u16 adr[SUB_LIST_MAX_LPN];
}mesh_lpn_subsc_list_t;

typedef struct{
    u32 tick;
    u8 retry_type;
    u8 retry_cnt;
    u8 subsc_cnt;
    u8 TransNo;
	u16 adr[SUB_LIST_MAX_IN_ONE_MSG];
}mesh_subsc_list_retry_t;

typedef struct{
    u32 tick;
    u8 *poll_rsp;
    u16 adr_dst;
    u16 par_val;
    u8 nk_array_idx;    // pkt decryption key
    u8 delay_type;
}fn_ctl_rsp_delay_t;

typedef struct{
    u32 tick_tx;
    u32 sleep_ms;      // sleep time ms
    u16 op;
    u8 sleep_ready;
}mesh_lpn_sleep_t;

typedef struct{
    u16 LPNAdr;
    u16 FriAdr;
	mesh_ctl_fri_req_t req;
	mesh_ctl_fri_offer_t offer;
	mesh_ctl_fri_poll_t poll;
	mesh_ctl_fri_update_t update;
    u8 link_ok;
}mesh_lpn_par_t;

//--------------directed forwarding-------------------//
enum{
	DIRECTED_FORWARDING_DISABLE,
	DIRECTED_FORWARDING_ENABLE,
};

enum{
	DIRECTED_RELAY_DISABLE,
	DIRECTED_RELAY_ENABLE,	
};

enum{
	DIRECTED_PROXY_DISABLE,
	DIRECTED_PROXY_ENABLE,
	DIRECTED_PROXY_NOT_SUPPORT,
	DIRECTED_PROXY_IGNORE = 0xff,
};

enum{
	DIRECTED_PROXY_DEFAULT_DISABLE,
	DIRECTED_PROXY_DEFAULT_ENABLE,
	DIRECTED_PROXY_DEFAULT_NOT_SUPPORT,
	DIRECTED_PROXY_DEFAULT_IGNORE = 0xff,
};

enum{
	DIRECTED_FRIEND_DISABLE,
	DIRECTED_FRIEND_ENABLE,
	DIRECTED_FRIEND_NOT_SUPPORT,
	DIRECTED_FRIEND_IGNORE = 0xff,
};

typedef struct{
	u8 prohibited:1;
	u8 discovery_interval:1;
	u8 path_lifetime:2;
	u8 path_metric_type:3;
	u8 on_behalf_of_dependent_origin:1;	
	u8 forwarding_number;
	u8 origin_path_metric;
	u16 destination;
	u8 addr_par[6]; // path origin and dependent path origin
}mesh_ctl_path_req_t;

typedef struct{
	u8 prohibit:5;
	u8 confirmation_request:1;
	u8 on_behalf_of_dependent_target:1;
	u8 unicast_destination:1;	
	u16 path_origin;
	u8 forwarding_number;
	u8 addr_par[6]; // path target and dependent target
}mesh_ctl_path_reply_t;

typedef struct{
	u16 path_origin;
	u16 path_target;
}mesh_ctl_path_confirmation_t;

typedef struct{
	u16 destination;
}mesh_ctl_path_echo_reply_t;

typedef struct{
	u8 prohibited:7;
	u8 type:1;
	u16 path_endpoint;
	addr_range_t  dependent_addr; 
}mesh_ctl_dependent_node_update_t;

typedef struct{
	u16 addr_list[1];
}mesh_ctl_path_request_solication_t;

typedef struct{
	u8 directed_proxy;
	u8 use_directed;
}directed_proxy_capa_sts;

typedef struct{
	u8 use_directed;
	addr_range_t addr_range;
}directed_proxy_ctl_t;

//---------------------
#define ELE_CNT_MAX_LIB     (16)
typedef struct{
    u16 adr[ELE_CNT_MAX_LIB];
    u16 adr_cnt;
}mesh_adr_list_t;

/**
 * @brief  mesh callback function parameter structure definition
 */
typedef struct{
	u8 *model;              /*!< Point to the model in the message */
	u8 *res;                /*!< mesh_op_resource_t */ 
	mesh_cmd_nw_t *p_nw;    /*!< It may be NULL, must check not NULL 
	                             before used. */
	u32 op_rsp;             /*!< Opcode to reply if needed */
	u16 op;                 /*!< Opcode of the current message */
	u16 adr_src;            /*!< Source address */
	u16 adr_dst;            /*!< Destination address */
	u8 retransaction;       /*!< Flag indicating whether this command 
	                             is a retransmission */
	u8 model_idx;           /*!< Model index */
	// rssi of packet, please refert to "rssi_pkt"
}_align_type_4_ mesh_cb_fun_par_t;

typedef struct{
    u16 adr_src;
    u16 adr_dst;
    u8 retransaction;
}mesh_cb_fun_par_vendor_t;

typedef int (*cb_cmd_sig2_t) (u8 *p, int len, mesh_cb_fun_par_t *cb_par);

typedef struct{
    u16 op;
    u16 status_cmd; // receive status message even though there is not server model        // u16 for align
    u32 model_id_tx;
    u32 model_id_rx;
    cb_cmd_sig2_t cb;
    u32 op_rsp;     // -1 for no rsp and ensure 4BYTE align
	#if LOG_OP_STRING_EN
    char *op_string;	// should be at last
	#endif
}mesh_cmd_sig_func_t;

typedef struct{
    u16 vd_id;
    mesh_cmd_sig_func_t *func;
    u32 num;
}mesh_vd_func_t;

typedef struct{
    u32 model_id;   // u32 for align
    u8 *p_model;
    cb_pub_st_t cb_pub_st;
    u16 size;       // sizeof model[0]
    u8 model_cnt;   // 
    u8 multy_flag;  // if 0, means only one in a node.
}mesh_model_resource_t;

typedef struct{
    cb_cmd_sig2_t cb;
    u32 op_rsp;
	u8* model[ELE_CNT_MAX_LIB];
	u32 model_cnt;
	u32 id;
	char *op_string;	// can not use LOG_OP_STRING_EN, because of library
	u8 model_idx[ELE_CNT_MAX_LIB];
	u8 sig;
	u8 status_cmd;
}_align_type_4_ mesh_op_resource_t;

int is_support_op(mesh_op_resource_t *op_res, u16 op, u16 adr_dst, u8 tx_flag);
int is_subscription_adr(model_common_t *p_model, u16 adr);
u8 * mesh_get_model_par_by_op_dst(u16 op, u16 ele_adr);

//---------------------
typedef enum{
	ST_BLOCK_ACK_RX_ALL		= 0,	// RX node has received all segments.
	ST_BLOCK_ACK_MISSING	= 1,	// RX node only received some segments.
	ST_BLOCK_ACK_BUSY		= 2,	// RX node is receiving another segment flow, so current tx segment flow should be stopped and retry later.
	ST_BLOCK_ACK_TIMEOUT	= 3,	// tx segment flow timeout.
	ST_BLOCK_ACK_UNKNOW		= 4,	// 
}st_block_ack_t;

int mesh_seg_block_ack_cb(const mesh_cmd_bear_t *p_bear_ack, st_block_ack_t st);

//---------------------
typedef struct{
    u8 id;
    u8 pol;
    u16 val;
}mesh_light_control_t;

//-------------------------
extern _align_4_ my_fifo_t mesh_fri_cache_fifo[MAX_LPN_NUM];
extern _align_4_ mesh_ctl_fri_poll_t fn_poll[MAX_LPN_NUM];
extern _align_4_ mesh_ctl_fri_update_t fn_update[MAX_LPN_NUM];
extern _align_4_ mesh_ctl_fri_req_t fn_req[MAX_LPN_NUM];
extern _align_4_ mesh_ctl_fri_offer_t fn_offer[MAX_LPN_NUM];

extern _align_4_ mesh_fri_ship_other_t fn_other_par[MAX_LPN_NUM];
extern _align_4_ fn_ctl_rsp_delay_t fn_ctl_rsp_delay[MAX_LPN_NUM];
extern _align_4_ mesh_fri_ship_proc_fn_t fri_ship_proc_fn[MAX_LPN_NUM];
extern _align_4_ mesh_fri_ship_proc_lpn_t fri_ship_proc_lpn;
extern _align_4_ mesh_lpn_par_t mesh_lpn_par;

void friend_ship_establish_ok_cb_lpn();
void friend_ship_disconnect_cb_lpn();
int is_friend_ship_link_ok_fn(u8 lpn_idx);
int is_friend_ship_link_ok_lpn();
void iv_update_set_with_update_flag_ture(u8 *iv_idx, u32 search_mode);
int iv_update_key_refresh_rx_handle(mesh_ctl_fri_update_flag_t *p_ivi_flag, u8 *p_iv_idx);
u32 get_poll_timeout_fn(u16 lpn_adr);
u32 get_current_poll_timeout_timer_fn(u16 lpn_adr);
void mesh_friend_response_delay_proc_fn(u8 lpn_idx);
mesh_fri_ship_other_t * mesh_fri_cmd2cache(u8 *p_bear_big, u8 len_nw, u8 adv_type, u8 trans_par_val, u16 F2L_bit);
void mesh_rc_segment_handle_fn(mesh_match_type_t *p_match_type, mesh_cmd_nw_t *p_nw);
#if FEATURE_FRIEND_EN
int mesh_cmd_action_need_friend(mesh_match_type_t *p_match_type);
#endif
void mesh_rc_segment2friend_cache(mesh_cmd_nw_t *p_nw_rc, u16 F2L_bit);

// key refresh CFG client flow
enum{
	KR_CFGCL_NORMAL				= 0,
	KR_CFGCL_GET_CPS,
	KR_CFGCL_ADD_APPKEY,
	KR_CFGCL_BIND_APPKEY_MD2,		// 3
	KR_CFGCL_BIND_APPKEY_MD3,
	KR_CFGCL_GET_PHASE0,
	KR_CFGCL_SEND_NEW_NETKEY,		// 6
	KR_CFGCL_SEND_NEW_APPKEY,
	KR_CFGCL_SET_PHASE2,
	KR_CFGCL_SET_PHASE3,			//9
	KR_CFGCL_BIND_DEFAULT_APPKEY,
	KR_CFGCL_BIND_APPKEY,
	//----------- trigger by security beacon
	KR_CFGCL_BC_KR_ON			= 20,
	KR_CFGCL_BC_KR_OFF			= 21,
};

typedef struct{
	u8 ak[16];
	u8 nk_new[16];
	u8 ak_new[16];
	u16 nk_idx;
	u16 ak_idx;
	u8 ele_bind_index;
	u8 model_bind_index;
	u32 model_id;
	u8 is_new_model;
	u16 node_adr;
	u8 st;
	u8 next_st_flag;
	u8 retry_cnt;
	u8 fast_bind;
}key_refresh_cfgcl_proc_t;

extern _align_4_ key_refresh_cfgcl_proc_t key_refresh_cfgcl_proc;

void mesh_kr_cfgcl_check_set_next_flag();
void mesh_kr_cfgcl_proc();
void mesh_kr_cfgcl_start(u16 node_adr);
int key_refresh_phase_set_by_index(u16 nk_idx, u8 phase_set);

//----------------------------------- beacon
#define IV_UPDATE_DISABLE               0

typedef struct{
    mesh_ctl_fri_update_flag_t flag;
    u8 nwId[8];     // store in big endianness
    u8 iv_idx[4];
	u8 auth_val[8];
}mesh_beacon_sec_nw_t;

typedef struct{
	u8 dev_uuid[16];
	u8 oob_info[2];
	u8 uri_hash[4];
}mesh_beacon_unprov_t;

typedef struct{
	u8 random[13];
	u8 obfuse[5];
	u8 auth_tag[8];
}mesh_beacon_privacy_t;
//---------------

typedef enum{
	MESH_BEAR_UNASSIGNED = 0,
	MESH_BEAR_ADV = BIT(0),
	MESH_BEAR_GATT = BIT(1),
}bear_index_t;

#define MESH_BEAR_SUPPORT	(MESH_BEAR_ADV|MESH_BEAR_GATT)

#define GATEWAY_OTA_MESH		0
#define GATEWAY_OTA_SLEF		1 // initial is ota self


// 
#if 1 // use ST_SUCCESS instead
typedef enum{
	MESH_CFG_STS_SUCCESS=0,
	MESH_CFG_STS_INVALID_ADR,
	MESH_CFG_STS_INVALID_MOD,
	MESH_CFG_STS_INVALID_APPKEY_IND,
	MESH_CFG_STS_INVALID_NETKEY_IND,
	MESH_CFG_STS_INSUFFICENT_RES,
	MESH_CFG_STS_KEY_IND_STORED,
	MESH_CFG_STS_INVALID_PUB_PARA,
	MESH_CFG_STS_NOT_SUB_MOD,
	MESH_CFG_STS_SOTRAGE_FAIL,
	MESH_CFG_STS_FEATURE_NOT_SUP,
	MESH_CFG_STS_CAN_NOT_UPDATE,
	MESH_CFG_STS_CAN_NOT_REMOVE,
	MESH_CFG_STS_CAN_NOT_BIND,
	MESH_CFG_STS_TMP_CHANGE_STATE,
	MESH_CFG_STS_CAN_NOT_SET,
	MESH_CFG_STS_UNSPECIFY_ERR,
	MESH_CFG_STS_INVAILD_BINDING,
	MESH_CFG_STS_RFU,
}mesh_sts_code_e;
#endif
//--------------- inline function

static inline int is_cfg_model(u32 model_id, int sig_flag)
{
    if(sig_flag && ((SIG_MD_CFG_SERVER == model_id)
                  ||(SIG_MD_CFG_CLIENT == model_id))){
    	return 1;
	}
	return 0;
}

int is_use_device_key(u32 model_id, int sig_flag);

static inline u16 get_u16_not_aligned(u8 *par)
{
    return (par[0] + (par[1]<<8));
}

static inline u32 get_u32_not_aligned(u8 *par)
{
    return (par[0] + (par[1]<<8) + (par[2]<<16) + (par[3]<<24));
}

static inline int is_tx_cmd_err_number_fatal(int err)
{
	return ((TX_ERRNO_DEV_OR_APP_KEY_NOT_FOUND == err) || (TX_ERRNO_ADDRESS_INVALID == err)
		|| (TX_ERRNO_PAR_LEN_OVER_FLOW == err));
}
//--------------- declaration
int mesh_rsp_err_st_pub_status(u8 st, u16 ele_adr, u32 model_id, bool4 sig_model, u16 adr_dst);

int mesh_cmd_sig_cfg_cps_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_sec_nw_bc_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_sec_nw_bc_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_def_ttl_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_def_ttl_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_friend_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_friend_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_gatt_proxy_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_gatt_proxy_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_key_phase_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_key_phase_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_relay_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_relay_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_nw_transmit_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_nw_transmit_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_node_identity_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_node_identity_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_lpn_poll_timeout_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_netkey_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_netkey_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_appkey_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_appkey_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_model_app_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
u8 mesh_appkey_bind(u16 op, u16 ele_adr, u32 model_id, bool4 sig_model, u16 appkey_idx);
int mesh_cmd_sig_cfg_bind(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_node_reset(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_model_pub_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_model_pub_set_vr(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_cfg_model_pub_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
u8 mesh_sub_search_and_set(u16 op, u16 ele_adr, u16 sub_adr, u8 *uuid, u32 model_id, bool4 sig_model);
int mesh_cmd_sig_cfg_model_sub_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_sig_model_sub_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_vendor_model_sub_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);

//-------- config model end -------
enum{
	EV_TRAVERSAL_SET_MD_ELE_ADR		= 0,
	EV_TRAVERSAL_UNBIND_APPKEY		= 1,
	EV_TRAVERSAL_PUB_PERIOD			= 2,
    EV_TRAVERSAL_BIND_APPKEY        = 3,
};

extern u8 key_bind_list_cnt;
void mesh_kr_cfgcl_status_update(mesh_rc_rsp_t *rsp);
int mesh_sec_msg_dec_virtual_ll(u16 ele_adr, u32 model_id, bool4 sig_model, 
			u8 *key, u8 *nonce, u8 *dat, int n, int mic_length, u16 adr_dst, const u8 *dat_org);
void mesh_set_model_ele_adr(u16 ele_adr, u32 model_id, bool4 sig_model);
void mesh_tx_pub_period(u16 ele_adr, u32 model_id, bool4 sig_model);
void ev_handle_traversal_cps_ll(u32 ev, u8 *par, u16 ele_adr, u32 model_id, bool4 sig_model);
void ev_handle_traversal_cps(u32 ev, u8 *par);
void traversal_cps_reset_vendor_id(u16 vd_id);
int mesh_search_model_id_by_op(mesh_op_resource_t *op_res, u16 op, u8 tx_flag);
void get_cps_from_keybind_list(mesh_page0_t *page0_out, mesh_page0_t *p_rsp_page0);
u32 get_cps_ele_len(const mesh_element_head_t *p_ele);
u8 is_model_in_key_bind_list(u16 *p_mode_id);
int mesh_tx_cmd_rsp_cfg_model(u16 op, u8 *par, u32 par_len, u16 adr_dst);
u32 get_blt_conn_interval_us();
u32 get_ele_idx(u16 ele_adr);
int mesh_rsp_err_st_sub_list(u8 st, u16 ele_adr, u32 model_id, bool4 sig_model, u16 adr_dst);
u8 mesh_add_pkt_model_sub_list(u16 ele_adr, u32 model_id, bool4 sig_model, u16 adr_src);
u32 get_reliable_interval_ms(material_tx_cmd_t *p);

//--------
int blc_hci_handler (u8 *p, int n);
void usb_bulk_drv_init (void *p);
void bls_set_advertise_prepare (void *p);
void bls_set_update_chn_cb(void *p);
void blc_reset_ll_initScan_module();
void show_ota_result(int result);
int factory_reset_handle ();
int factory_reset_cnt_check ();
int factory_reset();
void kick_out(int led_en);
int is_fixed_group(u16 adr_dst);

int my_fifo_push_adv (my_fifo_t *f, u8 *p, u8 n, u8 ow);
int my_fifo_push_relay_ll (my_fifo_t *f, mesh_cmd_bear_t *p_in, u8 n, u8 ow);
int my_fifo_push_relay (mesh_cmd_bear_t *p_in, u8 n, u8 ow);
int my_fifo_push_hci_tx_fifo (u8 *p, u16 n, u8 *head, u8 head_len);
int relay_adv_prepare_handler(rf_packet_adv_t * p, int rand_en);
void my_fifo_poll_relay(my_fifo_t *f);
u8* get_adv_cmd();
mesh_relay_buf_t * my_fifo_get_relay(my_fifo_t *f);
int is_busy_mesh_tx_cmd(u16 adr_dst);
int mesh_tx_cmd_add_packet(u8 *p_bear);
int mesh_tx_cmd_add_packet_fn2lpn(u8 *p_bear);
int mesh_rc_data_layer_network (u8 *p_payload, int src_type, u8 need_proxy_and_trans_par_val);
int mesh_tx_cmd2_access(material_tx_cmd_t *p, int reliable, mesh_match_type_t *p_match_type);
void mesh_rc_data_action(mesh_cmd_nw_t *p_nw, u8 len_nw);
void keyboard_handle_mesh();
int mesh_tx_cmd_layer_acccess(u8 *p_ac_hci, u32 len_ac, u16 adr_src, u16 adr_dst, int reliable, mesh_match_type_t *p_match_type);
void mesh_match_group_mac(mesh_match_type_t *p_match_type, u16 adr_dst, u32 op_ut, int app_tx, u16 adr_src);
u16 mesh_mac_match_friend(u16 adr);
u16 mesh_group_match_friend(u16 adr);
u8 get_tx_nk_arr_idx_friend(u16 adr, u16 op);
u8* mesh_find_ele_resource_in_model(u16 ele_adr, u32 model_id, bool4 sig_model, u8 *idx_out, int set_flag);
u8 find_ele_support_model_and_match_dst(mesh_adr_list_t *adr_list, u16 adr_dst, u32 model_id, bool4 sig_model);
int mesh_sec_msg_dec_virtual (u8 *key, u8 *nonce, u8 *dat, int n, int mic_length, u16 adr_dst, const u8 *dat_org);
int mesh_rc_data_layer_access(u8 *ac, int len_ac, mesh_cmd_nw_t *p_nw);
void mesh_seg_ack_poll();
int is_busy_segment_or_reliable_flow();
int is_tx_seg_one_round_ok();
void cache_init(u16 ele_adr);
int is_exist_in_cache(u8 *p, u8 friend_key_flag, int save);
u16 get_mesh_current_cache_num(); // Note, there may be several elements in a node, but there is often only one element that is in cache.

void mesh_friend_ship_proc_LPN(u8 *bear);
void mesh_friend_ship_proc_FN(u8 *bear);
void friend_cmd_send_fn(u8 lpn_idx, u8 op);
int friend_cache_check_replace(u8 lpn_idx, mesh_cmd_bear_t *bear_big);
void mesh_friend_ship_set_st_lpn(u8 st);
int is_in_mesh_friend_st_lpn();
void mesh_friend_ship_proc_init_fn(u8 lpn_idx);
int is_poll_cmd(mesh_cmd_nw_t *p_nw);
void mesh_friend_ship_start_poll();
void mesh_friend_ship_stop_poll();
int is_in_mesh_friend_st_fn(u8 lpn_idx);
void mesh_friend_ship_init_all();
int is_unicast_friend_msg_from_lpn(mesh_cmd_nw_t *p_nw);
int is_unicast_friend_msg_to_lpn(mesh_cmd_nw_t *p_nw);
int is_unicast_friend_msg_to_fn(mesh_cmd_nw_t *p_nw);
int is_use_friend_key_lpn(mesh_cmd_nw_t *p_nw, int Credential_Flag);
u8 get_sec_key_type(mesh_cmd_nw_t *p_nw);
static inline int is_must_use_friend_key_msg(mesh_cmd_nw_t *p_nw)
{
	return (FRIENDSHIP == get_sec_key_type(p_nw));
}
int is_not_cache_ctl_msg_fn(mesh_cmd_nw_t *p_nw);
int is_cmd2lpn(u16 adr_dst);
void lpn_quick_send_adv();
void mesh_friend_ship_clear_LPN();
int mesh_rc_segment_handle(mesh_cmd_bear_t *p_bear, u32 ctl, mesh_match_type_t *p_match_type, int src_type);
void mesh_tx_reliable_finish();
void VC_check_next_segment_pkt();

u8 mesh_subsc_adr_cnt_get (mesh_cmd_bear_t *p_br);
void friend_subsc_add(u16 *adr_list, u32 subsc_cnt);
void friend_subsc_rmv(u16 *adr_list, u32 subsc_cnt);

u8 mesh_lpn_tx_network_cb(mesh_match_type_t *p_match_type, u8 sec_type);
void mesh_lpn_proc_suspend ();
void mesh_lpn_sleep_prepare(u16 op);
void mesh_friend_ship_proc_init_lpn();

void suspend_enter(u32 sleep_ms, int deep_retention_flag);
int mesh_tx_cmd_layer_upper_ctl(u8 op, u8 *par, u32 len_par, u16 adr_src, u16 adr_dst,u8 filter_cfg);
int mesh_tx_cmd_layer_upper_ctl_specified_key(u8 op, u8 *par, u32 len_par, u16 adr_src, u16 adr_dst,u8 filter_cfg, u16 netkey_index);
int mesh_tx_cmd_layer_cfg_primary(u8 op, u8 *par, u32 len_par, u16 adr_dst);
int mesh_tx_cmd_layer_cfg_primary_specified_key(u8 op, u8 *par, u32 len_par, u16 adr_dst, u16 netkey_index);
int mesh_tx_cmd_layer_upper_ctl_primary_specified_key(u8 op, u8 *par, u32 len_par, u16 adr_dst, u16 netkey_index);
int mesh_rc_data_cfg_gatt(u8 *bear);
int check_and_send_next_segment_pkt();
void mesh_global_var_init_fn_buf();
int is_friend_ship_link_ok_lpn();

void mesh_beacon_poll_100ms();
int mesh_rc_data_beacon_sec (u8 *p_payload, u32 t);
void mesh_iv_update_st_poll_s();
void mesh_iv_update_start_check();
void mesh_iv_update_set_start_flag(int keep_search_flag);
void mesh_iv_idx_init(u8 *iv_index, int rst_sno);
void mesh_iv_update_enter_search_mode();
void mesh_iv_update_enter_update_progress();
void mesh_iv_update_enter_update2normal();
void mesh_iv_update_enter_normal();
void mesh_check_and_set_iv_update_rx_flag(u8 *p_ivi);
void mesh_iv_update_start_poll();
void mesh_tx_reliable_proc();
void mesh_model_ele_adr_init();
void mesh_set_all_model_ele_adr();
void mesh_unbind_by_del_appkey(u16 appkey_index);
int mesh_bulk_cmd2model(u8 *par, u32 len);
int mesh_bulk_cmd(mesh_bulk_cmd_par_t *par, u32 len);
int mesh_bulk_set_par(u8 *par, u32 len);
int mesh_bulk_set_par2usb(u8 *par, u32 len);
u32 get_op_rsp(u16 op);
void mesh_tid_timeout_check();
void is_cmd_with_tid_check_and_set(u16 ele_adr, u16 op, u8 *par, u32 par_len, u8 tid_pos_vendor_app);
int is_cmd_with_tid(u8 *tid_pos_out, u16 op, u8 *par, u8 tid_pos_vendor_app);
u32 rf_link_get_op_by_ac(u8 *ac);
int is_reliable_cmd(u16 op, u32 vd_op_rsp);
u8  cal_node_identity_by_proxy_sts(u8 proxy_sts);

void mesh_tx_reliable_check_and_init(material_tx_cmd_t *p);
int mesh_tx_cmd_reliable(material_tx_cmd_t *p);
int mesh_tx_cmd(material_tx_cmd_t *p);
int mesh_tx_cmd_rsp(u16 op, u8 *par, u32 par_len, u16 adr_src, u16 adr_dst, u8 *uuid, model_common_t *pub_md);
void set_material_tx_cmd(material_tx_cmd_t *p_mat, u16 op, u8 *par, u32 par_len, u16 adr_src, u16 adr_dst, u8 retry_cnt, int rsp_max, u8 *uuid, u8 nk_array_idx, u8 ak_array_idx, model_common_t *pub_md, u8 immutable_flag, bear_head_t *p_tx_head);
int mesh_tx_cmd2normal(u16 op, u8 *par, u32 par_len, u16 adr_src, u16 adr_dst, int rsp_max);
int mesh_tx_cmd2normal_specified_key(u16 op, u8 *par, u32 par_len, u16 adr_src, u16 adr_dst, int rsp_max, u16 netkey_index, u16 appkey_index);
int mesh_tx_cmd2normal_primary(u16 op, u8 *par, u32 par_len, u16 adr_dst, int rsp_max);
int mesh_tx_cmd2normal_primary_specified_key(u16 op, u8 *par, u32 par_len, u16 adr_dst, int rsp_max, u16 netkey_index, u16 appkey_index);
int mesh_tx_cmd2uuid(u16 op, u8 *par, u32 par_len, u16 adr_src, u16 adr_dst, int rsp_max, u8 *uuid);
int mesh_tx_cmd_layer_bear(u8 *p_bear, u8 trans_par_val);
void mesh_tx_reliable_tick_refresh_proc(int rx_seg_flag, u16 adr_src);
u8 key_refresh_phase_get(u8 nk_array_idx);
int mesh_key_refresh_phase_handle(u8 phase_set, mesh_net_key_t *p_net_key);
void mesh_key_refresh_rx_fri_update(u8 KeyRefresh);
int is_key_refresh_use_new_key(u8 nk_array_idx, bool4 rx_flag);
int is_key_refresh_use_old_and_new_key(u8 nk_array_idx);
void key_refresh_phase_set(u8 phase, mesh_net_key_t *p_net_key);
void mesh_nk_update_self_and_change2phase2(const u8 *nk, u16 key_idx);
void get_iv_update_key_refresh_flag(mesh_ctl_fri_update_flag_t *p_flag_out, u8 *iv_idx_out, u8 key_phase);

void mesh_loop_process();
void mesh_init_all();
void memcpy4(void * d, const void * s, unsigned int length);
void * memset4(void * dest, int val, unsigned int len);
int app_event_handler_adv_monitor(u8 *p_payload);
void RefreshStatusNotifyByHw(unsigned char *p, int len);
int app_event_report_provision2usb(u8 *prov_par);
void mesh_kr_cfgcl_mode_set(u16 addr, u8 mode,u16 nk_idx);
int mesh_adr_check(u16 adr_src, u16 adr_dst);
int mesh_adr_check_src_own_rx(u16 adr_src);
int mesh_proxy_adv2gatt(u8 *bear,u8 adv_type);

/************************** mesh_dongle_adv_report2vc **************************
function : copy the dec mesh message to hci tx fifo  
para:
	p_payload: pointer of the message, point to GET_ADV_PAYLOAD_FROM_BEAR(mesh_cmd_bear_t)
ret: 0  means push fifo OK 
	-1 or other value means err 
****************************************************************************/
int mesh_dongle_adv_report2vc(u8 *p_payload, u8 report_cmd);
int mesh_bear_tx2mesh(u8 *bear, u8 trans_par_val);
int mesh_bear_tx2gatt(u8 *bear,u8 adv_type);
int mesh_bear_tx2mesh_and_gatt(u8 *bear,u8 adv_type, u8 trans_par_val);
int mesh_cmd_action_need_local(mesh_match_type_t *p_match_type);

int debug_mesh_vc_adv_report2gatt(u8 *bear);
int debug_mesh_dongle_adv_bear2usb(u8 *bear);
int debug_mesh_report_provision_par2usb(u8 *net_info);
int debug_mesh_report_BLE_st2usb(u8 connect);
int debug_mesh_report_one_pkt_completed();
int mesh_get_netkey_idx_appkey_idx(mesh_bulk_cmd_par_t *p_cmd);
int mesh_tx_cmd_unreliable(material_tx_cmd_t *p);
void mesh_rsp_delay_set(u32 delay_step, u8 is_seg_ack);
bind_key_t * is_exist_bind_key(model_common_t *p_model, u16 appkey_idx);

//
u32 get_transition_100ms(trans_time_t *p_trans_time);
u8 get_transition_step_res(u32 transition_100ms);
int is_level_move_set_op(u16 op);
int is_level_delta_set_op(u16 op);
s16 get_target_level_by_op(s16 target_level, s32 level, u16 op, int light_idx, int st_trans_type);

// callback function
u8 mesh_appkey_bind(u16 op, u16 ele_adr, u32 model_id, bool4 sig_model, u16 appkey_idx);

// publish status--------------
void mesh_model_cb_pub_st_register();
void model_pub_check_set(int level_set_st, u8 *model, int priority);

int mesh_tx_cmd_g_level_st(u8 idx, u16 ele_adr, u16 dst_adr, u8 *uuid, model_common_t *pub_md);
int mesh_g_onoff_st_publish(u8 idx);
int mesh_g_level_st_publish(u8 idx);
int mesh_g_power_st_publish(u8 idx);
int mesh_health_cur_sts_publish(u8 idx);
void mesh_kc_cfgcl_mode_para(u16 apk_idx,u8 *p_appkey);
void mesh_kc_cfgcl_mode_para_set(u16 apk_idx,u8 *p_appkey,u16 unicast,u16 nk_idx,u8 fast_bind);

int is_busy_tx_segment_or_reliable_flow();
int is_busy_rx_segment_flow();


// mesh and VC common
#define REPORT_INTERVAL_ADV_MS		2
#define REPORT_ADV_BUF_SIZE			(64) // max 64 if HCI_ACCESS == HCI_USE_USB


// rc function part 
void proc_rc_ui_suspend();

#if 1
// VC project
int OnAppendLog_vs(unsigned char *p, int len);
#endif

#if (IS_VC_PROJECT)
	#if IOS_APP_ENABLE||ANDROID_APP_ENABLE	
		#define ModuleDlg_close(...);
	#else
		void ModuleDlg_close();
	#endif 
#else
#endif
// level part 

int tl_log_msg(u32 level_module,u8 *pbuf,int len,char  *format,...);
void tl_log_msg_err(u16 module,u8 *pbuf,int len,char *format,...);
void tl_log_msg_warn(u16 module,u8 *pbuf,int len,char  *format,...);
void tl_log_msg_info(u16 module,u8 *pbuf,int len,char  *format,...);
void tl_log_msg_dbg(u16 module,u8 *pbuf,int len,char  *format,...);
void user_log_info(u8 *pbuf,int len,char  *format,...);
#if WIN32
void tl_log_file(u32 level_module,u8 *pbuf,int len,char  *format,...);
#endif

#if 0// add nor base part ,we can demo from this part 
#define LOG_SRC_BEARER          (1 <<  0) /**< Receive logs from the bearer layer. */
#define LOG_SRC_NETWORK         (1 <<  1) /**< Receive logs from the network layer. */
#define LOG_SRC_TRANSPORT       (1 <<  2) /**< Receive logs from the transport layer. */
#define LOG_SRC_PROV            (1 <<  3) /**< Receive logs from the provisioning module. */
#define LOG_SRC_PACMAN          (1 <<  4) /**< Receive logs from the packet manager. */
#define LOG_SRC_INTERNAL        (1 <<  5) /**< Receive logs from the internal event module. */
#define LOG_SRC_API             (1 <<  6) /**< Receive logs from the Mesh API. */
#define LOG_SRC_DFU             (1 <<  7) /**< Receive logs from the DFU module. */
#define LOG_SRC_BEACON          (1 <<  8) /**< Receive logs from the beacon module. */
#define LOG_SRC_TEST            (1 <<  9) /**< Receive logs from unit tests. */
#define LOG_SRC_ENC             (1 << 10) /**< Receive logs from the encryption module. */
#define LOG_SRC_TIMER_SCHEDULER (1 << 11) /**< Receive logs from the timer scheduler. */
#define LOG_SRC_CCM             (1 << 12) /**< Receive logs from the CCM module. */
#define LOG_SRC_ACCESS          (1 << 13) /**< Receive logs from the access layer. */
#define LOG_SRC_APP             (1 << 14) /**< Receive logs from the application. */
#define LOG_SRC_SERIAL          (1 << 15) /**< Receive logs from the serial module. */

#define LOG_LEVEL_ASSERT ( 0) /**< Log level for assertions */
#define LOG_LEVEL_ERROR  ( 1) /**< Log level for error messages. */
#define LOG_LEVEL_WARN   ( 2) /**< Log level for warning messages. */
#define LOG_LEVEL_REPORT ( 3) /**< Log level for report messages. */
#define LOG_LEVEL_INFO   ( 4) /**< Log level for information messages. */
#define LOG_LEVEL_DBG1   ( 5) /**< Log level for debug messages (debug level 1). */
#define LOG_LEVEL_DBG2   ( 6) /**< Log level for debug messages (debug level 2). */
#define LOG_LEVEL_DBG3   ( 7) /**< Log level for debug messages (debug level 3). */
#define EVT_LEVEL_BASE   ( 8) /**< Base level for event logging. For internal use only. */
#define EVT_LEVEL_ERROR  ( 9) /**< Critical error event logging level. For internal use only. */
#define EVT_LEVEL_INFO   (10) /**< Normal event logging level. For internal use only. */
#define EVT_LEVEL_DATA   (11) /**< Event data logging level. For internal use only. */
#endif 

#define TL_LOG_LEVEL_DISABLE	  0
#define TL_LOG_LEVEL_USER         1U    // never use in library.
#define TL_LOG_LEVEL_LIB          2U    // it will not be optimized in library; some important log.
#define TL_LOG_LEVEL_ERROR        3U
#define TL_LOG_LEVEL_WARNING      4U
#define TL_LOG_LEVEL_INFO         5U
#define TL_LOG_LEVEL_DEBUG        6U
#define TL_LOG_LEVEL_MAX          TL_LOG_LEVEL_DEBUG

#if WIN32
	#if DEBUG_PROXY_FRIEND_SHIP
#define TL_LOG_LEVEL              TL_LOG_LEVEL_INFO // TL_LOG_LEVEL_WARNING
	#else
#define TL_LOG_LEVEL              TL_LOG_LEVEL_DEBUG
	#endif
#else
#define TL_LOG_LEVEL              TL_LOG_LEVEL_ERROR // TL_LOG_LEVEL_INFO	// Note firmware size
#endif
#define MESH_KR_CFG_RETRY_MAX_CNT 	2 

typedef enum{
	TL_LOG_MESH         = 0,
	TL_LOG_PROVISION    = 1,
	TL_LOG_LOWPOWER     = 2,
	TL_LOG_FRIEND	    = 3,
	TL_LOG_PROXY	    = 4,
	TL_LOG_GATT_PROVISION	= 5,
	TL_LOG_WIN32		=6,
	TL_LOG_GATEWAY		= 7,
	TL_LOG_KEY_BIND		= 8,
	TL_LOG_NODE_SDK		= 9,
	TL_LOG_NODE_BASIC   = 10,
	TL_LOG_REMOTE_PROV  = 11,
	TL_LOG_DIRECTED_FORWARDING = 12,
	TL_LOG_CMD_RSP		,
	TL_LOG_COMMON		,
	TL_LOG_CMD_NAME		,
	TL_LOG_NODE_SDK_NW_UT   ,
    TL_LOG_IV_UPDATE    ,
    TL_LOG_GW_VC_LOG	,
    TL_LOG_USER         ,	// never use in library.
	TL_LOG_MAX,
}printf_module_enum;

#define MAX_MODULE_STRING_CNT	16  // don't set too large to save firmware size
#define MAX_LEVEL_STRING_CNT	12  // don't set too large to save firmware size

#ifndef TL_LOG_SEL_VAL
#if WIN32
#define MAX_STRCAT_BUF		1024
	#if DEBUG_PROXY_FRIEND_SHIP
#define TL_LOG_SEL_VAL	(BIT(TL_LOG_COMMON)|BIT(TL_LOG_LOWPOWER)|BIT(TL_LOG_FRIEND)|BIT(TL_LOG_CMD_RSP)|BIT(TL_LOG_IV_UPDATE))
	#else
#define TL_LOG_SEL_VAL	(BIT(TL_LOG_PROVISION)|BIT(TL_LOG_GATT_PROVISION)|BIT(TL_LOG_GATEWAY)   \
                        |BIT(TL_LOG_COMMON)|BIT(TL_LOG_KEY_BIND)|BIT(TL_LOG_CMD_RSP)|BIT(TL_LOG_CMD_NAME)|\
                        BIT(TL_LOG_WIN32)|BIT(TL_LOG_IV_UPDATE)|BIT(TL_LOG_NODE_BASIC)|BIT(TL_LOG_REMOTE_PROV)|BIT(TL_LOG_GW_VC_LOG))
	#endif
#else
// just for node part 
#define MAX_STRCAT_BUF		(MAX_MODULE_STRING_CNT + MAX_LEVEL_STRING_CNT + 4)  //
	#if (1 && (0 == DEBUG_LOG_SETTING_DEVELOP_MODE_EN))
		#if (MESH_USER_DEFINE_MODE == MESH_IRONMAN_MENLO_ENABLE)
#define TL_LOG_SEL_VAL  ((BIT(TL_LOG_USER))|(BIT(TL_LOG_NODE_SDK)))//|BIT(TL_LOG_FRIEND)|BIT(TL_LOG_IV_UPDATE)) // |BIT(TL_LOG_NODE_SDK_NW_UT)
		#else
#define TL_LOG_SEL_VAL  (BIT(TL_LOG_USER))//(BIT(TL_LOG_NODE_SDK)|BIT(TL_LOG_FRIEND)|BIT(TL_LOG_IV_UPDATE)) // |BIT(TL_LOG_NODE_SDK_NW_UT)
//#define TL_LOG_SEL_VAL  (BIT(TL_LOG_USER))//(BIT(TL_LOG_NODE_SDK)|BIT(TL_LOG_FRIEND)|BIT(TL_LOG_IV_UPDATE)) // |BIT(TL_LOG_NODE_SDK_NW_UT)
		#endif
	#else
#define TL_LOG_SEL_VAL  (BIT(TL_LOG_USER)|BIT(TL_LOG_REMOTE_PROV)|BIT(TL_LOG_PROVISION)|BIT(TL_LOG_FRIEND)|BIT(TL_LOG_NODE_SDK)|BIT(TL_LOG_NODE_BASIC)|BIT(TL_LOG_IV_UPDATE)|BIT(TL_LOG_CMD_NAME))
	#endif
#endif
#endif

#define LOG_GET_LEVEL_MODULE(level, module)     ((level << 5) | module) // use 8bit to decrease firmware size
#define LOG_GET_MODULE(level_module)            (level_module & 0x1F)
#define LOG_GET_LEVEL(level_module)             ((level_module >> 5) & 0x07)

#define LOG_MSG_FUNC_EN(log_type_err, module)	(LOG_FW_FUNC_EN && (log_type_err || TL_LOG_ALL_MODULE_EN || (BIT(module) & TL_LOG_SEL_VAL)))

#if (LOG_FW_FUNC_EN && HCI_LOG_FW_EN && (TL_LOG_LEVEL >= TL_LOG_LEVEL_USER))
#define LOG_USER_MSG_INFO(pbuf,len,format,...)  do{int val; val = (LOG_MSG_FUNC_EN(0,TL_LOG_USER) && tl_log_msg(LOG_GET_LEVEL_MODULE(TL_LOG_LEVEL_USER,TL_LOG_USER),pbuf,len,format,__VA_ARGS__));}while(0)
#else
#define LOG_USER_MSG_INFO(pbuf,len,format,...)
#endif

#if (TL_LOG_LEVEL >= TL_LOG_LEVEL_LIB)
#define LOG_MSG_LIB(module,pbuf,len,format,...)  do{int val; val = (LOG_MSG_FUNC_EN(0,module) && tl_log_msg(LOG_GET_LEVEL_MODULE(TL_LOG_LEVEL_LIB,module),pbuf,len,format,__VA_ARGS__));}while(0)
#else
#define LOG_MSG_LIB(module,pbuf,len,format,...) 
#endif 

#if (TL_LOG_LEVEL >= TL_LOG_LEVEL_ERROR)
#define LOG_MSG_ERR(module,pbuf,len,format,...)  do{int val; val = (LOG_MSG_FUNC_EN(1,module) && tl_log_msg(LOG_GET_LEVEL_MODULE(TL_LOG_LEVEL_ERROR,module),pbuf,len,format,__VA_ARGS__));}while(0)
#else
#define LOG_MSG_ERR(module,pbuf,len,format,...) 
#endif 

#if (TL_LOG_LEVEL >= TL_LOG_LEVEL_WARNING)
#define LOG_MSG_WARN(module,pbuf,len,format,...) do{int val; val = (LOG_MSG_FUNC_EN(0,module) && tl_log_msg(LOG_GET_LEVEL_MODULE(TL_LOG_LEVEL_WARNING,module),pbuf,len,format,__VA_ARGS__));}while(0)
#else
#define LOG_MSG_WARN(module,pbuf,len,format,...) 
#endif 

#if (TL_LOG_LEVEL >= TL_LOG_LEVEL_INFO)
#define LOG_MSG_INFO(module,pbuf,len,format,...) do{int val; val = (LOG_MSG_FUNC_EN(0,module) && tl_log_msg(LOG_GET_LEVEL_MODULE(TL_LOG_LEVEL_INFO,module),pbuf,len,format,__VA_ARGS__));}while(0)
#else
#define LOG_MSG_INFO(module,pbuf,len,format,...) 
#endif

#if (TL_LOG_LEVEL >= TL_LOG_LEVEL_DEBUG)
#define LOG_MSG_DBG(module,pbuf,len,format,...)	do{int val; val = (LOG_MSG_FUNC_EN(0,module) && tl_log_msg(LOG_GET_LEVEL_MODULE(TL_LOG_LEVEL_DEBUG,module),pbuf,len,format,__VA_ARGS__));}while(0)
#else
#define LOG_MSG_DBG(module,pbuf,len,format,...) 
#endif 

#if WIN32 
#define LOG_MSG_WIN32_FILE(module,pbuf,len,format,...)  tl_log_file(LOG_GET_LEVEL_MODULE(TL_LOG_LEVEL_DEBUG,module),pbuf,len,format,__VA_ARGS__)
#else
#define LOG_MSG_WIN32_FILE(module,pbuf,len,format,...)
#endif

#if PTS_TEST_OTA_EN || 1
#define LOG_MSG_FUNC_NAME()     do{LOG_MSG_LIB (TL_LOG_NODE_BASIC, 0, 0, "%s", __FUNCTION__);}while(0)
#else
#define LOG_MSG_FUNC_NAME()     do{LOG_MSG_INFO (TL_LOG_CMD_NAME, 0, 0, "%s", __FUNCTION__);}while(0)
#endif
void MessageBoxVC(const char *str);
int isVC_DLEModeExtendBearer();

FLASH_ADDRESS_EXTERN;
extern _align_4_ my_fifo_t mesh_adv_cmd_fifo;
extern _align_4_ my_fifo_t mesh_adv_fifo_relay;
extern _align_4_ my_fifo_t mesh_adv_fifo_fn2lpn;
extern _align_4_ my_fifo_t hci_rx_fifo;
extern _align_4_ my_fifo_t hci_tx_fifo;
extern u8 hci_rx_fifo_b[];
extern u8 uart_hw_tx_buf[];      // not for user

extern u8 mesh_adv_cmd_fifo_b[];
extern u8 tbl_mac[6];
extern u32 mesh_adv_tx_cmd_sno;
extern u8 monitor_mode_en;
extern u8 monitor_filter_sno_en;
extern u8 lpn_provision_ok;
extern u8 mesh_init_flag ;
extern u16 app_adr;
extern u8 misc_flag ;
extern u8 pub_flag;
#if 0
#define LAYER_PARA_DEBUG(buf_name)  {        \
    static u8 buf_name =0;                           \
	buf_name++;\
}
#else
#define LAYER_PARA_DEBUG(buf_name)
#endif

#if 0
#define LAYER_DATA_DEBUG(buf_name, p_src, size)  {        \
    static u8 buf_name[size];                           \
    memcpy(buf_name, p_src, sizeof(buf_name));          \
}
#else
#define LAYER_DATA_DEBUG(buf_name, p_src, size)    
#endif

#define while_1_test        do{while(1);}while(0)
#define is_relay_support_and_en	(FEATURE_RELAY_EN && (RELAY_SUPPORT_ENABLE == mesh_get_relay()))
#if WIN32
#define is_proxy_support_and_en	 0
#else
#if TESTCASE_FLAG_ENABLE
#define is_proxy_support_and_en	(FEATURE_PROXY_EN   \
								 && mesh_is_proxy_ready())
#else
#define is_proxy_support_and_en	(FEATURE_PROXY_EN && (GATT_PROXY_SUPPORT_ENABLE == mesh_get_gatt_proxy()) \
								 && mesh_is_proxy_ready())
#endif
#endif
#define is_fn_support_and_en	(FEATURE_FRIEND_EN && (FRIEND_SUPPORT_ENABLE == mesh_get_friend()))
#define is_lpn_support_and_en	(FEATURE_LOWPOWER_EN && lpn_provision_ok)

#if BLE_REMOTE_PM_ENABLE
#define ENABLE_SUSPEND_MASK 	bls_pm_setSuspendMask(SUSPEND_ADV | DEEPSLEEP_RETENTION_ADV | SUSPEND_CONN | DEEPSLEEP_RETENTION_CONN)
#else
#define ENABLE_SUSPEND_MASK
#endif

int mesh_construct_adv_bear_with_nw(u8 *bear, u8 *nw, u8 len_nw);
u8 gateway_provision_rsp_cmd(u16 unicast_adr);
int gateway_common_cmd_rsp(u8 code,u8 *p_par,u16 len );

u8 gateway_model_cmd_rsp(u8 *para,u16 len );
u8 gateway_heartbeat_cb(u8 *para,u8 len );
void check_mesh_kr_cfgcl_timeout();
u8 gateway_keybind_rsp_cmd(u8 opcode );
int mesh_construct_adv_bear_with_bc(u8 *bear, u8 *nw, u8 len_nw);
int mesh_rsp_sub_status(u8 st, mesh_cfg_model_sub_set_t *p_set, bool4 sig_model, u16 adr_dst);
void mesh_friend_logmsg(mesh_cmd_bear_t *p_bear_big, u8 len);
void friend_subsc_list_add_adr(lpn_adr_list_t *adr_list_src, lpn_adr_list_t *adr_list_add, u32 cnt);
void friend_subsc_list_rmv_adr(lpn_adr_list_t *adr_list_src, lpn_adr_list_t *adr_list_rmv, u32 cnt);
u8 get_reliable_retry_cnt(material_tx_cmd_t *p);


static inline u8 mesh_nw_len_get_by_bear (const mesh_cmd_bear_t *p_br){
    return (p_br->len - 1);	// 1 : sizeof(p_br->type)
}

static inline u8 mesh_lt_len_get_by_bear (const mesh_cmd_bear_t *p_br)
{
    const mesh_cmd_nw_t *p_nw = &p_br->nw;
    int len_nw = mesh_nw_len_get_by_bear(p_br);
    return (len_nw - OFFSETOF(mesh_cmd_nw_t, data)- (p_nw->ctl ? SZMIC_NW64 : SZMIC_NW32));
}

static inline u8 mesh_bear_len_get (const mesh_cmd_bear_t *p_br){	// exclude trans_par_val
    return (p_br->len + OFFSETOF(mesh_cmd_bear_t, len) + sizeof(p_br->len));
}

static inline u8 mesh_adv_payload_len_get (const mesh_cmd_bear_t *p_br){
    return (p_br->len + OFFSETOF(mesh_cmd_bear_t, len));
}

static inline mesh_cmd_bear_t *GET_BEAR_FROM_ADV_PAYLOAD (u8 *p_payload){
	return (mesh_cmd_bear_t *)(p_payload - OFFSETOF(mesh_cmd_bear_t,len));
}

static inline u8 *GET_ADV_PAYLOAD_FROM_BEAR (mesh_cmd_bear_t *bear){
	return (&bear->len);
}

static inline adv_report_extend_t *get_adv_report_extend (u8 *p_rf_len){
    event_adv_report_t *pa = CONTAINER_OF(p_rf_len,event_adv_report_t,data[0]);
    adv_report_extend_t *p_extend = (adv_report_extend_t *)(pa->data+((*p_rf_len)+1));
    return p_extend;
}

static inline u8 transmit_cnt_get(u8 tran_val){
    mesh_transmit_t *p_trans_par = (mesh_transmit_t *)&tran_val;
	return p_trans_par->count;
}

static inline u8 transmit_invl_step_get(u8 tran_val){
    mesh_transmit_t *p_trans_par = (mesh_transmit_t *)&tran_val;
	return p_trans_par->invl_steps;
}

static inline u8 transmit_invl_step_get_10ms(u8 tran_val){
	return transmit_invl_step_get(tran_val) + 1;
}

extern u8 model_need_key_bind_whitelist(u16 *key_bind_list_buf,u8 *p_list_cnt,u8 max_cnt);

extern u8 gateway_provision_send_pid_mac(u8 *p_id,u8 *p_mac);

extern int mesh_rsp_handle_proc_win32(mesh_rc_rsp_t *p_rsp);
extern int mesh_json_update_ivi_index(u8 *p_index);

#if 1
#define ONLINE_ST_INTERVAL_MS       (320)
#define ONLINE_ST_NODE_MAX_NUM      (64) // not equal to MESH_NODE_MAX_NUM, because LPN is no online status.
#define	MESH_NODE_ST_PAR_LEN        (3)  //MAX: 7, because sizeof(node_data)  in mesh_node_flush_status()      //lumen-rsv, exclude dev_adr and sn.
//Note: ONLINE_STATUS_TIMEOUT may be set longer after set ONLINE_ST_NODE_MAX_NUM or MESH_NODE_ST_VAL_LEN larger, 
#define ONLINE_STATUS_TIMEOUT       3000 //ms

#define MESH_RSSI_RECORD_EN		    0

#define NODE_STATUS_VALID_FLAG      0xA5
#define ADV_PAYLOAD_LEN_MAX         (31)

#define ONLINE_ST_MIC_LEN           (4)     // ADV
#define ONLINE_ST_MIC_LEN_GATT      (2)     // GATT, 2 bytes is enough for GATT

typedef struct{
    u16 dev_adr :15;     // don't change include type
    u16 rsv     :1;
    u8 sn;          // don't change include type
    u8 par[MESH_NODE_ST_PAR_LEN];  //lumen-rsv,
}mesh_node_st_val_t;

#define	MESH_NODE_ST_VAL_LEN        (sizeof(mesh_node_st_val_t))

typedef struct{
    u16 tick;       // don't change include type
    mesh_node_st_val_t val;
	#if MESH_RSSI_RECORD_EN
    u8 rssi;        // must after "val", and don't use in library
	#endif
}mesh_node_st_t;

typedef struct{
    u8 type;
    u8 len_node_st_val  : 4;
    u8 rsv              : 4;
    u16 sno;        // for iv index
    mesh_node_st_val_t node[2];
    u8 mic[ONLINE_ST_MIC_LEN_GATT];
}online_st_report_t;    // for GATT report

#define ONLINE_ST_IV_LEN            (5)     // ADV
#define ONLINE_ST_IV_LEN_GATT       (OFFSETOF(online_st_report_t,node))     // GATT

enum{
	FLD_SYNCED =				BIT(7),
};

extern u8 mesh_node_st_val_len;
extern u8 mesh_node_st_par_len;
extern u8 mesh_node_st_len;
extern u16 online_st_node_max_num;
extern u16 mesh_node_max;    // count of current mesh

void ll_device_status_update (u8 *st_val_par, u8 len);			//call this function whenever device need update status
void mesh_node_buf_init ();
void rssi_online_status_pkt_cb(mesh_node_st_t *p_node_st, u8 rssi, int online_again);
void mesh_node_online_st_init ();
void mesh_send_online_status ();
void mesh_node_report_init();
void mesh_report_status_enable(u8 mask);
void mesh_node_flush_status ();
int mesh_node_report_status (u8 *p, int ns);
void online_st_proc();
void device_status_update();
void mesh_report_status_enable_mask(u8 *val, u16 len);
void mesh_node_update_status (u8 *p, int ns);
void online_st_rc_mesh_pkt(u8 *p_payload);
int online_st_adv_dec (u8 *playload);
int online_st_adv_enc (u8 *playload);
int online_st_gatt_enc (u8 *report, u8 len);
int online_st_gatt_dec (u8 *report, u8 len);
u8 get_online_st_adv_type();
int mesh_node_check_force_notify(u16 dst, u8 par0);
void online_st_force_notify_check(mesh_cmd_bear_t *p_bear, u8 *ut_dec, int src_type);
void online_st_force_notify_check3(mesh_cmd_bear_t *p_bear, u8 *ut_dec);
int mesh_add_seg_ack(const mesh_rx_seg_par_t *p_rx_seg_par);
int mesh_rx_seg_ack_handle(int tx_flag);
void mesh_tx_segment_finished();
void init_ecc_key_pair(u8 force_en);
ble_sts_t bls_ll_setAdvParam_interval(u16 min_ms, u16 rand_ms);
void aes_ll_encryption(u8 *key, u8 *plaintext, u8 *result);
int dualmode_ota_auth_value_check (const u8 key[8], u8 uuid[16], const u8 auth_app[16]);
void cmd_ota_mesh_hk_login_handle(const u8 auth_app[16]);
void mesh_seg_filter_adr_set(u16 adr);
u32 mesh_max_payload_get (u32 ctl, bool4 extend_adv_short_unseg);
int  is_not_use_extend_adv(u16 op);
int is_not_use_extend_adv_ctl(u16 ctl_op);
void tx_busy_seg_ack(mesh_cmd_bear_t *p_bear, mesh_match_type_t *p_match_type);

extern u8 mesh_node_report_enable;
extern u32 online_status_timeout;

#endif

#if ATT_REPLACE_PROXY_SERVICE_EN
#define proxy_Out_ccc			provision_Out_ccc
#define proxy_In_ccc			provision_In_ccc
#endif

#if DEBUG_CFG_CMD_GROUP_AK_EN
record_info_t *get_existed_nw_notify_record(u16 addr);
record_info_t * add_nw_notify_record(u8 *p_payload);
void update_nw_notify_num(u8 * p_rf_pkt, u8 next_buffer);
#endif
//--------------- include
#include "../../vendor/common/mesh_node.h"
#include "../../proj_lib/mesh_crypto/mesh_crypto.h"
#include "../../vendor/common/config_model.h"
#include "../../vendor/common/system_time.h"
#include "../../vendor/common/app_beacon.h"
#include "../../vendor/common/app_provison.h"
#include "../../vendor/common/app_proxy.h"
#include "../../vendor/common/mesh_test_cmd.h"
#include "../../vendor/common/mesh_common.h"
#include "../../vendor/common/vendor_model.h"
#include "../../vendor/common/fast_provision_model.h"
#if MI_API_ENABLE
#include "../../vendor/common/mi_api/mi_vendor/vendor_model_mi.h"
#include "mi_config.h"
#endif
#include "../../vendor/common/cmd_interface.h"
#include "../../vendor/common/sensors_model.h"
#include "../../vendor/common/op_agg_model.h"

#endif /* APP_MESH_H_ */


