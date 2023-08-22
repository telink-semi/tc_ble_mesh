/********************************************************************************************************
 * @file	mesh_ota.h
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

#include "tl_common.h"
#include "proj_lib/sig_mesh/app_mesh.h"

// ------
#ifndef CEIL_DIV
#define CEIL_DIV(A, B)                  (((A) + (B) - 1) / (B))
#endif
#ifndef CHAR_BIT
#define CHAR_BIT                        8
#endif

#define CEIL_8(val)                     CEIL_DIV(val, 8)
#define GET_BLOCK_SIZE(log)             (1 << log)
// ------
#define BLOB_TRANSFER_WITHOUT_FW_UPDATE_EN  (PTS_TEST_OTA_EN || 1)

#define MESH_OTA_R04                    1
#define MESH_OTA_R06                    2

#define MESH_OTA_V_SEL                  (MESH_OTA_R06)

#if(PTS_TEST_OTA_EN)
#define SIG_MD_FW_UPDATE_S              0x1402
#define SIG_MD_FW_UPDATE_C              0x1403
#define SIG_MD_FW_DISTRIBUT_S           0x1404
#define SIG_MD_FW_DISTRIBUT_C           0x1405
#define SIG_MD_BLOB_TRANSFER_S        	0x1400
#define SIG_MD_BLOB_TRANSFER_C         	0x1401

//----------------------------------- op code
// op cmd 0xxxxxxx (SIG)
// op cmd 10xxxxxx xxxxxxxx (SIG)

#define FW_UPDATE_INFO_GET		        0x0883
#define FW_UPDATE_INFO_STATUS		    0x2483
#define FW_UPDATE_METADATA_CHECK	    0x2583
#define FW_UPDATE_METADATA_CHECK_STATUS 0x2683
#define FW_UPDATE_GET		        	0x0983
#define FW_UPDATE_START		        	0x0A83
#define FW_UPDATE_CANCEL		        0x0B83
#define FW_UPDATE_APPLY		        	0x0C83
#define FW_UPDATE_STATUS		        0x1883
#define FW_DISTRIBUT_GET		        0x1183
#define FW_DISTRIBUT_START		        0x1C83
#define FW_DISTRIBUT_SUSPEND		    0x2783
#define FW_DISTRIBUT_CANCEL		        0x1283
#define FW_DISTRIBUT_APPLY		        0x1383
#define FW_DISTRIBUT_STATUS		        0x1D83
#define FW_DISTRIBUT_RECEIVERS_GET		0x0F83
#define FW_DISTRIBUT_RECEIVERS_LIST		0x1A83
#define FW_DISTRIBUT_RECEIVERS_ADD		0x1983
#define FW_DISTRIBUT_RECEIVERS_DELETE_ALL   0x0D83
#define FW_DISTRIBUT_RECEIVERS_STATUS	0x0E83
#define FW_DISTRIBUT_CAPABILITIES_GET	0x1083
#define FW_DISTRIBUT_CAPABILITIES_STATUS	0x1B83
#define FW_DISTRIBUT_UPLOAD_GET		    0x1483
#define FW_DISTRIBUT_UPLOAD_START		0x1E83
#define FW_DISTRIBUT_UPLOAD_OOB_START	0x1F83
#define FW_DISTRIBUT_UPLOAD_CANCEL		0x1583
#define FW_DISTRIBUT_UPLOAD_STATUS		0x2083
#define FW_DISTRIBUT_FW_GET		        0x2183
#define FW_DISTRIBUT_FW_STATUS		    0x2383
#define FW_DISTRIBUT_FW_GET_BY_INDEX    0x1683
#define FW_DISTRIBUT_FW_DELETE		    0x2283
#define FW_DISTRIBUT_FW_DELETE_ALL		0x1783

#define BLOB_TRANSFER_GET		        0x0083
#define BLOB_TRANSFER_START		        0x0183
#define BLOB_TRANSFER_CANCEL		    0x0283
#define BLOB_TRANSFER_STATUS		    0x0383
#define BLOB_BLOCK_GET		            0x0583
#define BLOB_BLOCK_START		        0x0483
#define BLOB_BLOCK_STATUS		        0x67
#define BLOB_PARTIAL_BLOCK_REPORT	    0x65    // used for pull mode(LPN)
#define BLOB_CHUNK_TRANSFER		        0x66
#define BLOB_INFO_GET		            0x0683
#define BLOB_INFO_STATUS		        0x0783
#elif DRAFT_FEAT_VD_MD_EN
#include "draft_feature_vendor.h"
#else
#define SIG_MD_FW_UPDATE_S              0xFE00
#define SIG_MD_FW_UPDATE_C              0xFE01
#define SIG_MD_FW_DISTRIBUT_S           0xFE02
#define SIG_MD_FW_DISTRIBUT_C           0xFE03
#define SIG_MD_BLOB_TRANSFER_S        	0xFF00
#define SIG_MD_BLOB_TRANSFER_C         	0xFF01

//----------------------------------- op code
// op cmd 0xxxxxxx (SIG)

// op cmd 10xxxxxx xxxxxxxx (SIG)
#define FW_UPDATE_INFO_GET		        0x01B6
#define FW_UPDATE_INFO_STATUS		    0x02B6
#define FW_UPDATE_METADATA_CHECK	    0x03B6
#define FW_UPDATE_METADATA_CHECK_STATUS 0x04B6
#define FW_UPDATE_GET		        	0x05B6
#define FW_UPDATE_START		        	0x06B6
#define FW_UPDATE_CANCEL		        0x07B6
#define FW_UPDATE_APPLY		        	0x08B6
#define FW_UPDATE_STATUS		        0x09B6
#define FW_DISTRIBUT_GET		        0x0AB6
#define FW_DISTRIBUT_START		        0x0BB6
#define FW_DISTRIBUT_SUSPEND		    0x3AB7
#define FW_DISTRIBUT_CANCEL		        0x0CB6
#define FW_DISTRIBUT_APPLY		        0x0DB6
#define FW_DISTRIBUT_STATUS		        0x0EB6
#define FW_DISTRIBUT_RECEIVERS_GET      0x0FB6
#define FW_DISTRIBUT_RECEIVERS_LIST		0x10B6
#define FW_DISTRIBUT_RECEIVERS_ADD		0x11B6
#define FW_DISTRIBUT_RECEIVERS_DELETE_ALL   0x12B6
#define FW_DISTRIBUT_RECEIVERS_STATUS	0x13B6
#define FW_DISTRIBUT_CAPABILITIES_GET	0x14B6
#define FW_DISTRIBUT_CAPABILITIES_STATUS	0x15B6
#define FW_DISTRIBUT_UPLOAD_GET		    0x16B6
#define FW_DISTRIBUT_UPLOAD_START		0x17B6
#define FW_DISTRIBUT_UPLOAD_OOB_START	0x18B6
#define FW_DISTRIBUT_UPLOAD_CANCEL		0x19B6
#define FW_DISTRIBUT_UPLOAD_STATUS		0x1AB6
#define FW_DISTRIBUT_FW_GET		        0x1BB6
#define FW_DISTRIBUT_FW_STATUS		    0x1CB6
#define FW_DISTRIBUT_FW_GET_BY_INDEX    0x1DB6
#define FW_DISTRIBUT_FW_DELETE		    0x1EB6
#define FW_DISTRIBUT_FW_DELETE_ALL		0x1FB6


#define BLOB_TRANSFER_GET		        0x01B7
#define BLOB_TRANSFER_START		        0x02B7
#define BLOB_TRANSFER_CANCEL		    0x03B7
#define BLOB_TRANSFER_STATUS		    0x04B7
#define BLOB_BLOCK_GET		            0x07B7
#define BLOB_BLOCK_START		        0x05B7
#define BLOB_BLOCK_STATUS		        0x7E
#define BLOB_PARTIAL_BLOCK_REPORT	    0x7C    // used for pull mode(LPN)
#define BLOB_CHUNK_TRANSFER		        0x7D
#define BLOB_INFO_GET		            0x0AB7
#define BLOB_INFO_STATUS		        0x0BB7
#endif

//------op parameters
#if WIN32
#define MESH_OTA_UPDATE_NODE_MAX        (MESH_NODE_MAX_NUM) // max: (380 - head)/2
#else
    #if (__TL_LIB_8269__ || MCU_CORE_TYPE == MCU_CORE_8269 )
#define MESH_OTA_UPDATE_NODE_MAX        (MESH_ELE_MAX_NUM)  // set small just for save RAM
    #elif GATEWAY_ENABLE
#define MESH_OTA_UPDATE_NODE_MAX        (MESH_NODE_MAX_NUM)
    #else
#define MESH_OTA_UPDATE_NODE_MAX        (40)    // to save ram for mesh project
    #endif
#endif

#define MESH_OTA_IMAGE_LIST_SIZE_MAX    (1)
#define DEFAULT_DISTRIBUTION_APPKEY_INDEX       (0xffff)

#if MD_SERVER_EN
	#if ((FEATURE_LOWPOWER_EN) && (!GATT_LPN_EN)) 	//  || DEBUG_SHOW_VC_SELF_EN
#define MESH_OTA_PULL_MODE_EN           1		// support push mode for GATT single OTA later.
	#else
#define MESH_OTA_PULL_MODE_EN           0
	#endif
#else
#define MESH_OTA_PULL_MODE_EN           0		// must 0
#endif

//---- transfer mode setting
#define MESH_OTA_TRANSFER_MODE_PUSH     BIT(0)
#define MESH_OTA_TRANSFER_MODE_PULL     BIT(1)      // for LPN

#define TRANSFER_MODE_PUSH_AND_PULL		(MESH_OTA_TRANSFER_MODE_PULL | MESH_OTA_TRANSFER_MODE_PUSH)

#if MESH_OTA_PULL_MODE_EN
	#if PTS_TEST_OTA_EN
#define MESH_OTA_TRANSFER_MODE_SEL      (MESH_OTA_TRANSFER_MODE_PULL) // only pull for PTS test
	#else
#define MESH_OTA_TRANSFER_MODE_SEL      (MESH_OTA_TRANSFER_MODE_PULL | MESH_OTA_TRANSFER_MODE_PUSH) // push mode for quick ota in gatt connected mode.
	#endif
#else
#define MESH_OTA_TRANSFER_MODE_SEL      (MESH_OTA_TRANSFER_MODE_PUSH)
#endif

#define MESH_OTA_BOTH_PUSH_PULL_EN		(MESH_OTA_TRANSFER_MODE_SEL == TRANSFER_MODE_PUSH_AND_PULL)


//---- blob transfer setting
#if MESH_OTA_PULL_MODE_EN
#define PULL_MODE_MAX_MISSING_CHUNK		(10) // (1 << FN_CACHE_SIZE_LOG) // can set to be greater than cache number to OTA faster, but should not be segment.
	#if PTS_TEST_OTA_EN
#define PULL_MODE_REPORT_TIMER_TIMEOUT_MS	(10*1000)
	#else
#define PULL_MODE_REPORT_TIMER_TIMEOUT_MS	(1200)	// 3 times of FRI_POLL_INTERVAL_MS_MESH_OTA
	#endif
#define PULL_MODE_REPORT_RETRY_MAX			(10)

#define PULL_CHUNK_TICK_TIMEOUT_MS			(10*1000) // must greater than FRI_POLL_INTERVAL_MS_MESH_OTA
#endif

#if DRAFT_FEAT_VD_MD_EN
#define CHUNK_SIZE_PUSH_MODE           	(256)	// for compatibility
#else
#define CHUNK_SIZE_PUSH_MODE           	(208)   // =(((CONST_DELTA_EXTEND_AND_NORMAL + 11 - 3)/16)*16) // need 16 align 
#endif

#if 0 // WIN32 test
#define MESH_OTA_CHUNK_SIZE             (16)   // because fifo size is not enough now: must less than (72 - 11)
#define MESH_OTA_BLOCK_SIZE_LOG_MIN     (8)
#define MESH_OTA_BLOCK_SIZE_LOG_MAX     (8)
#elif PTS_TEST_OTA_EN
	#if MESH_OTA_PULL_MODE_EN
#define MESH_OTA_CHUNK_SIZE             (256)     // unsegment packet
#define MESH_OTA_BLOCK_SIZE_LOG_MIN     (10)    // due to chunk max is less than 0x80(utf8 1byte), and sizeof(fw_update_srv_proc_t.miss_mask) is less than that of push mode.
#define MESH_OTA_BLOCK_SIZE_LOG_MAX     (10)    // due to chunk max is less than 0x80(utf8 1byte), and sizeof(fw_update_srv_proc_t.miss_mask) is less than that of push mode.
	#else
#define MESH_OTA_CHUNK_SIZE             (32)   	//
#define MESH_OTA_BLOCK_SIZE_LOG_MIN     (6)    	// FD-BV-05: must >= 6.
#define MESH_OTA_BLOCK_SIZE_LOG_MAX     (6)    	// PTS SR/FU/BV-03V: must <= 15. TBD.
	#endif
#elif (MESH_OTA_PULL_MODE_EN && !EXTENDED_ADV_ENABLE)
#define MESH_OTA_CHUNK_SIZE             (8)     // unsegment packet
#define MESH_OTA_BLOCK_SIZE_LOG_MIN     (10)    // due to chunk max is less than 0x80(utf8 1byte), and sizeof(fw_update_srv_proc_t.miss_mask) is less than that of push mode.
#define MESH_OTA_BLOCK_SIZE_LOG_MAX     (10)    // due to chunk max is less than 0x80(utf8 1byte), and sizeof(fw_update_srv_proc_t.miss_mask) is less than that of push mode.
#else
#define MESH_OTA_CHUNK_SIZE             (CHUNK_SIZE_PUSH_MODE)   // =(((CONST_DELTA_EXTEND_AND_NORMAL + 11 - 3)/16)*16) // need 16 align 
	#if DRAFT_FEAT_VD_MD_EN
#define MESH_OTA_BLOCK_SIZE_LOG_MIN     (12)	// for compatibility
#define MESH_OTA_BLOCK_SIZE_LOG_MAX     (12)	// for compatibility
	#else
#define MESH_OTA_BLOCK_SIZE_LOG_MIN     (18)    // in order to check missing only once
#define MESH_OTA_BLOCK_SIZE_LOG_MAX     (18)    // in order to check missing only once
	#endif
#endif

#define MESH_OTA_BLOCK_SIZE_MIN         (1 << MESH_OTA_BLOCK_SIZE_LOG_MIN)
#define MESH_OTA_BLOCK_SIZE_MAX         (1 << MESH_OTA_BLOCK_SIZE_LOG_MAX)
#define MESH_OTA_CHUNK_SIZE_MAX         (MESH_OTA_CHUNK_SIZE)
#define MESH_OTA_CHUNK_NUM_MAX          CEIL_DIV(MESH_OTA_BLOCK_SIZE_MAX, MESH_OTA_CHUNK_SIZE)
#define MESH_OTA_CHUNK_NUM_MAX_CEIL     (CEIL_8(MESH_OTA_CHUNK_NUM_MAX))
// #define MESH_OTA_CHUNK_MISS_MAX         (4)     // for unsegment // TODO
#if PTS_TEST_OTA_EN
#define MESH_OTA_BLOB_SIZE_MAX        	(5*1024)
#else
#define MESH_OTA_BLOB_SIZE_MAX        	(FW_SIZE_MAX_K * 1024)
#endif

#define MESH_OTA_BLOCK_MAX 	            CEIL_DIV(MESH_OTA_BLOB_SIZE_MAX, MESH_OTA_BLOCK_SIZE_MIN)

#if PTS_TEST_OTA_EN
#define MESH_OTA_BLOB_START_TIMEOUT_MS  (8 * 1000) // PTS MMDL/SR/BT/BV-04-C check 10s ? // /BT/BV-38-C check 9s ?
#define PTS_TEST_BLOB_TRANSFER_FLOW_EN	1

#define PULL_LPN_REACHABLE_TIMER_EN		1	// 
#endif

#if PULL_LPN_REACHABLE_TIMER_EN
#define PULL_LPN_REACHABLE_TIMER_TIMEOUT_S		(68)	// unit:s // PTS BV6 need to be more than 67.
#define PULL_LPN_REACHABLE_TIMER_RETRY_MAX		(4)		// PTS BV6 need to be 4.
#endif

#if WIN32
#define UPLOAD_OOB_START_FUN_EN		1
#else
#define UPLOAD_OOB_START_FUN_EN		PTS_TEST_OTA_EN
#define UPLOAD_MULTY_FW_TEST_EN		PTS_TEST_OTA_EN
#endif

#if MD_MESH_OTA_EN
    #if (MESH_USER_DEFINE_MODE == MESH_SPIRIT_ENABLE ||MESH_USER_DEFINE_MODE == MESH_TAIBAI_ENABLE)
#define FW_ADD_BYTE_EN      0
    #else
#define FW_ADD_BYTE_EN      0
    #endif
#endif

#if DISTRIBUTOR_UPDATE_SERVER_EN
#define DISTRIBUTOR_NO_UPDATA_START_2_SELF          (NO_TX_RSP2SELF_EN || DEBUG_SHOW_VC_SELF_EN)
#endif

#define MESH_OTA_TEST_SOME_CASE_EN					(DEBUG_LOG_SETTING_DEVELOP_MODE_EN || DEBUG_SHOW_VC_SELF_EN)

#if (0 == RELIABLE_CMD_EN)
#define NODE_DISTRIBUT_RETRY_FLOW_EN				1 // for mesh light
	#if NODE_DISTRIBUT_RETRY_FLOW_EN
#define NODE_DISTRIBUT_RETRY_INTERVAL_MS			(1500)
	#endif
#else
#define NODE_DISTRIBUT_RETRY_FLOW_EN				0 // must 0
#endif

/*distribute phase code*/
enum{
	DISTRIBUT_PHASE_IDLE                = 0,
	DISTRIBUT_PHASE_TRANSFER_ACTIVE     = 1,
	DISTRIBUT_PHASE_TRANSFER_SUCCESS    = 2,
	DISTRIBUT_PHASE_APPLYING_UPDATE     = 3,
	DISTRIBUT_PHASE_COMPLETED           = 4,
	DISTRIBUT_PHASE_FAILED              = 5,
	DISTRIBUT_PHASE_CANCELING_UPDATE    = 6,
	DISTRIBUT_PHASE_TRANSFER_SUSPEND    = 7,
	DISTRIBUT_PHASE_MAX,
};

/*distribute model status code*/
enum{
	DISTRIBUT_ST_SUCCESS                = 0,
	DISTRIBUT_ST_INSUFFICIENT_RESOURCE  = 1,
	DISTRIBUT_ST_WRONG_PHASE            = 2,
	DISTRIBUT_ST_INTERNAL_ERROR         = 3,
	DISTRIBUT_ST_FW_NOT_FOUND           = 4,
	DISTRIBUT_ST_INVALID_APPKEY_IDX     = 5,
	DISTRIBUT_ST_RECEIVES_LIST_EMPTY    = 6,
	DISTRIBUT_ST_BUSY_WITH_DISTRIBUTION = 7,
	DISTRIBUT_ST_BUSY_WITH_UPLOAD       = 8,
	DISTRIBUT_ST_URI_NOT_SUPPORT        = 9,
	DISTRIBUT_ST_URI_MALFORMED          = 0x0A,
	DISTRIBUT_ST_URI_UNREACHABLE        = 0x0B,
	DISTRIBUT_ST_NEW_FW_NOT_AVAILABLE   = 0x0C,
	DISTRIBUT_ST_SUSPEND_FAILED   		= 0x0D,
	DISTRIBUT_ST_MAX,
	DISTRIBUT_ST_KEEP_CURRENT,			// just for internal flag
};
/*distribute model status code end*/

enum{
	UPDATE_NODE_ST_SUCCESS          = 0,
	UPDATE_NODE_ST_IN_PROGRESS      = 1,
	UPDATE_NODE_ST_CANCEL           = 2,
};

enum{
    BIN_CRC_DONE_NONE       = 0,    // must 0
    BIN_CRC_DONE_OK         = 1,
    BIN_CRC_DONE_FAIL       = 2,
};

typedef struct{
	u16 pid;
	u16 vid;
}fw_id_t;

typedef struct{
    fw_id_t fw_id; // just for demo
    #if (!(WIN32 || DRAFT_FEAT_VD_MD_EN))
    u8 rsv[4];
    #endif
}fw_metadata_t;


typedef struct{
	u16 update_list[1]; // just for struct
}fw_update_list_t;

enum{
    UPDATE_POLICY_VERIFY_ONLY = 0,
    UPDATE_POLICY_VERIFY_APPLY = 1,
};

typedef struct{
    u16 distrib_app_key_idx;
    u8 distrib_ttl;
    u16 distrib_timeout_base;
    u8 distrib_transfer_mode    :2;
    u8 update_policy            :1;
    u8 rfu                      :5;
    u16 distrib_fw_image_index;
}fw_distribut_par_t;

#if DISTRIBUTOR_START_TLK_EN
typedef struct{
	u16 adr_group;
	u16 update_list[MESH_OTA_UPDATE_NODE_MAX];
}fw_distribut_start_tlk_t;
#endif

typedef struct{
    fw_distribut_par_t par;
	u16 adr_group; // or sub uuid
}fw_distribut_start_t;

typedef struct{
    u8 st;
    u8 distrib_phase;
    //-- optional
    u16 distrib_multicast_addr;
    fw_distribut_par_t par;
}fw_distribut_status_t;

typedef struct{
    u16 first_index;
    u16 entries_limit;
}fw_distribut_receiver_get_t;

#define TRANSFER_PROGRESS_MAX       (50) // unit: 2%

typedef struct{
	u32 adr                 :15;
	u32 update_phase        :4;
	u32 update_st           :3;
	u32 transfer_st         :4;
	u32 transfer_progress   :6; // unit: 2%
	u8 update_fw_image_index;
}fw_distribut_update_node_entry_t;

typedef struct{
    u16 list_cnt;
    u16 first_index;
	fw_distribut_update_node_entry_t node[min(MESH_OTA_UPDATE_NODE_MAX, (MESH_CMD_ACCESS_LEN_MAX - 6)/sizeof(fw_distribut_update_node_entry_t))]; // 6: op + offset(node)
}fw_distribut_receiver_list_t;

#if 1 // DISTRIBUTOR_UPDATE_SERVER_EN
enum{
    RETRIEVAL_OOB_NOT_SUPPORTED     = 0,
    RETRIEVAL_OOB_SUPPORTED         = 1,
    RETRIEVAL_OOB_MAX,
};

typedef struct{
    u16 max_receivers_size;
    u16 max_fw_image_list_size;
    u32 max_fw_image_size;
    u32 max_upload_space;
    u32 remain_upload_space;
    u8 oob_retrieval;
    //u8 URI_scheme_names[];
}fw_distribut_capabilities_status_t;

typedef struct{
    u16 addr;
    u8 update_fw_image_idx;
}fw_receive_entry_t;

typedef struct{
    fw_receive_entry_t entry[1];
}fw_distribut_receivers_add_t;

typedef struct{
    u8 st;
    u16 list_cnt;
}fw_distribut_receivers_status_t;

typedef struct{
    u8 upload_ttl;
    u16 upload_timeout_base;
    u8 upload_blob_id[8];
    u32 upload_fw_size;
    u8 fw_metadata_len;
    fw_metadata_t fw_metadata;  // variable len
    fw_id_t fw_id;              // variable len
}fw_distribut_upload_start_t;

typedef struct{
    u8 uri_len;
    u8 uri[1];			// variable len
    // fw_id_t fw_id;	// variable len, so need to use API to get pointer of fw id.
}fw_distribut_upload_oob_start_t;

/*firmware update model status code*/
enum{
	UPLOAD_PHASE_IDLE             		= 0,
	UPLOAD_PHASE_TRANSFER_ACTIVE		= 1,
	UPLOAD_PHASE_TRANSFER_ERROR			= 2,
	UPLOAD_PHASE_TRANSFER_SUCCESS		= 3,
	UPLOAD_ST_MAX,
};

enum{
	UPLOAD_TYPE_INBAND             		= 0,
	UPLOAD_TYPE_OUTOFBAND				= 1,
}; // bool

typedef struct{
    u8 st;		// distribute status code
    u8 upload_phase;
    // optional
    u8 upload_progress	:7;
    u8 upload_type		:1;
    fw_id_t fw_id;
}fw_distribut_upload_status_t;

#define FW_IMAGE_IDX_NOT_LISTED			(0xFFFF)

typedef struct{
    fw_id_t fw_id;
}fw_distribut_fw_get_t;

typedef struct{
    u16 dist_fw_image_idx;
}fw_distribut_fw_get_by_idx_t;

typedef struct{
    fw_id_t fw_id;
}fw_distribut_fw_del_t;

//typedef struct{
	// no parameter
//}fw_distribut_fw_del_all_t;

typedef struct{
	u8 st;		// distribution status code
    u16 entry_cnt;
    u16 dist_fw_image_idx;
    // optional: only for get, or be valid in firmware image list.
    fw_id_t fw_id;
}fw_distribut_fw_status_t;
#endif


enum{
    UPDATE_PHASE_IDLE               = 0,    // no DFU update in progress
    UPDATE_PHASE_TRANSFER_ERR       = 1,
    UPDATE_PHASE_TRANSFER_ACTIVE    = 2,
    UPDATE_PHASE_VERIFYING_UPDATE   = 3,	// DUF_READY
    UPDATE_PHASE_VERIFYING_SUCCESS  = 4,
    UPDATE_PHASE_VERIFYING_FAIL     = 5,
    UPDATE_PHASE_APPLYING_UPDATE    = 6,    // means device is applying new firmware, and will reboot later
    UPDATE_PHASE_MAX,
    UPDATE_PHASE_PTS_8	= 8,				// FD/BV-25, 41:update phase 8 is what ? // only for client: "Update Receivers"
};

enum{
    ADDITIONAL_CPS_NO_CHANGE                    = 0, // CPS: composition data
    ADDITIONAL_CPS_CHANGE_NO_REMOTE_PROVISIOIN  = 1,
    ADDITIONAL_CPS_CHANGE_REMOTE_PROVISIOIN     = 2,
    ADDITIONAL_NODE_UNPROVISIONED               = 3, // The node is unprovisioned after successful application of a verified firmware image.
    ADDITIONAL_MAX,
};

/*firmware update model status code*/
enum{
	UPDATE_ST_SUCCESS               = 0,
	UPDATE_ST_INSUFFICIENT_RESOURCE = 1,
	UPDATE_ST_WRONG_PHASE           = 2,
	UPDATE_ST_INTERNAL_ERROR        = 3,
	UPDATE_ST_WRONG_FW_INDEX        = 4,
	UPDATE_ST_METADATA_CHECK_FAIL   = 5,
	UPDATE_ST_TEMP_UNAVAILABLE      = 6,
	UPDATE_ST_BLOB_TRANSFER_BUSY    = 7,
	UPDATE_ST_MAX,
};
/*firmware update model status code end*/


typedef struct{
    u8 first_index;
    u8 entry_limit;
}fw_update_info_get_t;

typedef struct{
    u8 list_count;
    u8 first_index;
#if 1   // only one firmware for telink SDK
    u8 fw_id_len;
    fw_id_t fw_id;
    u8 uri_len; // set 0 now.
    // uri[];
#endif
}fw_update_info_status_t;

typedef struct{
    u8 image_index; // Index of the firmware image in the Firmware information List state that is being updated
    fw_metadata_t metadata;
}fw_update_metadata_check_t;

typedef struct{
	u8 st               :3;
	u8 additional_info  :5;
	u8 image_index;
}fw_update_metadata_check_status_t;

typedef struct{
    u8 ttl;
    u16 timeout_base; // unit: 10 second
    u8 blob_id[8];
    u8 image_index;  // Index of the firmware image in the Firmware information List state that is being updated
    fw_metadata_t metadata;
}fw_update_start_t;

typedef struct{
	u8 st               :3;
	u8 rfu              :2;
	u8 update_phase     :3;
	// --below is optional
	u8 ttl;
	u8 additional_info  :5;
	u8 rfu2             :3;
	u16 timeout_base;
	u8 blob_id[8];
	u8 image_index;
}fw_update_status_t;

static inline int is_valid_metadata_len(u32 len)
{
    return (len >= 0 && len <= sizeof(fw_metadata_t));
}

/*BLOB transfer update model status code*/
enum{ // include BLOB transfer get and BLOB block get, etc.
	BLOB_TRANS_ST_SUCCESS              	= 0,
	BLOB_TRANS_ST_INVALID_BK_NUM       	= 1,        // invalid block number, no previous block
	BLOB_TRANS_ST_INVALID_BK_SIZE      	= 2,        // bigger then Block Size Log [Object Transfer Start]
	BLOB_TRANS_ST_INVALID_CHUNK_SIZE   	= 3,        // bigger then Block Size divided by Max Chunks Number [Object Information Status]
	BLOB_TRANS_ST_WRONG_PHASE        	= 4,
	BLOB_TRANS_ST_INVALID_PAR         	= 5,
	BLOB_TRANS_ST_WRONG_BLOB_ID        	= 6,
	BLOB_TRANS_ST_BLOB_TOO_LARGE       	= 7,
	BLOB_TRANS_ST_UNSUPPORT_TRANS_MODE 	= 8,
	BLOB_TRANS_ST_INTERNAL_ERROR       	= 9,
	BLOB_TRANS_ST_INFO_UNAVAILABLE      = 0x0A,
	BLOB_TRANS_ST_MAX
};
/*BLOB transfer update model status code end*/

typedef struct{
    u8 rfu              :6;
    u8 transfer_mode    :2;
	u8 blob_id[8];
	u32 blob_size;
	u8 bk_size_log;
	u16 client_mtu_size;
}blob_transfer_start_t;

typedef struct{
	u8 blob_id[8];
}blob_transfer_cancel_t;

enum{
    BLOB_TRANS_PHASE_INACTIVE           = 0,    // be same with Idle ?
    BLOB_TRANS_PHASE_WAIT_START         = 1,
    BLOB_TRANS_PHASE_WAIT_NEXT_BLOCK    = 2,
    BLOB_TRANS_PHASE_WAIT_NEXT_CHUNK    = 3,
    BLOB_TRANS_PHASE_COMPLETE           = 4,    // The BLOB was transferred successfully.
    BLOB_TRANS_PHASE_SUSPEND            = 5,
    BLOB_TRANS_PHASE_MAX,
};

#define TRANSFER_MTU_SIZE_MIN			(23)

typedef struct{
	u8 st               :4;
	u8 rfu              :2;
	u8 transfer_mode    :2;
	u8 transfer_phase;
	// -- below is optional
	u8 blob_id[8];      // C1
	u32 blob_size;      // C2
	u8 bk_size_log;
	u16 transfer_mtu_size;
	u8 bk_not_receive[CEIL_8(MESH_OTA_BLOCK_MAX)];   // variable: bit field
}blob_transfer_status_t;

enum{
	BLOB_BLOCK_CHECK_SUM_TYPE_CRC32        	= 0,
	BLOB_BLOCK_CHECK_SUM_TYPE_MAX
};

typedef struct{
	u16 block_num;
	u16 chunk_size;
}blob_block_start_t;

typedef struct{
	u16 chunk_num;
	u8 data[MESH_OTA_CHUNK_SIZE];
}blob_chunk_transfer_t;

enum{
	BLOB_BLOCK_FORMAT_ALL_CHUNK_MISS   	= 0,
	BLOB_BLOCK_FORMAT_NO_CHUNK_MISS     = 1,    // 0x40
	BLOB_BLOCK_FORMAT_SOME_CHUNK_MISS  	= 2,    // 0x80
	BLOB_BLOCK_FORMAT_ENCODE_MISS_CHUNK	= 3,    // 0xc0
};

typedef struct{
	u8 st       :4;
	u8 rfu      :2;
	u8 format   :2;
#if (MESH_OTA_V_SEL == MESH_OTA_R04)
	u8 transfer_phase;
#endif
	u16 block_num;
	u16 chunk_size;
	// optional for block get
	#if MESH_OTA_PULL_MODE_EN
	u8 miss_chunk[5]; // just for no segment packet, because there is no need to report all chunk, and also no need to be same with partial report.
	#else
	u8 miss_chunk[MESH_OTA_CHUNK_NUM_MAX_CEIL];
	#endif
}blob_block_status_t;

typedef struct{
	u8 bk_size_log_min;
	u8 bk_size_log_max;
	u16 chunk_num_max;
	u16 chunk_size_max;
	u32 blob_size_max;  // max firmware size
	u16 server_mtu_size;
	#if 1
    u8 transfer_mode;
	#else
	union{
	    u8 transfer_mode;
	    struct{
        	u8 transfer_mode_push   :1;
        	u8 transfer_mode_pull   :1;
        	u8 transfer_mode_rsv    :6;
    	};
	};
	#endif
}blob_info_status_t;


//--- proc
enum{
	MASTER_OTA_ST_IDLE                      = 0,
	MASTER_OTA_ST_DISTRIBUT_RECEIVER_ADD_OR_UPLOAD_START, // RECEIVER_ADD and upload flow is in the same level, and can shift sequence between them.
	MASTER_OTA_ST_DISTRIBUT_START,
	MASTER_OTA_ST_UPDATE_METADATA_CHECK,
	MASTER_OTA_ST_SUBSCRIPTION_SET,		    // 4
	MASTER_OTA_ST_FW_UPDATE_INFO_GET,
	MASTER_OTA_ST_UPDATE_START,
	MASTER_OTA_ST_BLOB_TRANSFER_GET,
	MASTER_OTA_ST_BLOB_INFO_GET,		    // 8
	MASTER_OTA_ST_BLOB_TRANSFER_START,
	MASTER_OTA_ST_BLOB_BLOCK_START,
	MASTER_OTA_ST_BLOB_BLOCK_START_CHECK_RESULT,
	MASTER_OTA_ST_BLOB_CHUNK_START,			// 0x0c
	MASTER_OTA_ST_BLOB_BLOCK_GET,
	MASTER_OTA_ST_BLOB_PARTIAL_BLOCK_REPORT,
	MASTER_OTA_ST_UPDATE_GET,
	MASTER_OTA_ST_WAITING_INITIATOR_UPDATE_APPLY,// 0x10
	MASTER_OTA_ST_UPDATE_APPLY,
	MASTER_OTA_ST_FW_UPDATE_INFO_GET_AFTER_APPLY,
	MASTER_OTA_ST_DISTRIBUT_CANCEL,
	MASTER_OTA_ST_MAX,
};

#if INITIATOR_CLIENT_EN
enum{
	INITIATOR_OTA_ST_IDLE                   = 0,
	INITIATOR_OTA_ST_CAPABILITY_GET,
	INITIATOR_OTA_ST_FW_UPDATE_INFO_GET,
	INITIATOR_OTA_ST_UPDATE_METADATA_CHECK,
	INITIATOR_OTA_ST_SUBSCRIPTION_SET,		// 4
	INITIATOR_OTA_ST_DISTR_RECEIVER_ADD,
	INITIATOR_OTA_ST_DISTR_UPLOAD_START,
	INITIATOR_OTA_ST_BLOB_TRANSFER_GET,
	INITIATOR_OTA_ST_BLOB_INFO_GET,			// 8
	INITIATOR_OTA_ST_BLOB_TRANSFER_START,
	INITIATOR_OTA_ST_BLOB_BLOCK_START,
	INITIATOR_OTA_ST_BLOB_BLOCK_START_CHECK_RESULT,
	INITIATOR_OTA_ST_BLOB_CHUNK_START,	    // 0x0c
	INITIATOR_OTA_ST_BLOB_BLOCK_GET,
	INITIATOR_OTA_ST_DISTR_START,
	INITIATOR_OTA_ST_DISTR_RECEIVER_GET,
	INITIATOR_OTA_ST_DISTR_PRE_APPLY,       // 0x10
	INITIATOR_OTA_ST_DISTR_APPLY,
	INITIATOR_OTA_ST_DISTR_GET,
	INITIATOR_OTA_ST_DISTR_PRE_CANCEL,
	INITIATOR_OTA_ST_DISTR_CANCEL,
	INITIATOR_OTA_ST_MAX,                   // 0x15
};
#endif

#if UPLOAD_MULTY_FW_TEST_EN
typedef struct{
	u8 upload_1st_en;
	fw_id_t fw_id_1st;
}upload_multy_fw_id_t;
#endif

typedef struct{
	u16 adr;
	u8 update_fw_image_idx;
	u8 st_block_start;
	u8 st_block_get;
    u8 update_phase     :3;
    u8 additional_info  :5;
    u8 skip_flag        :1;
    u8 apply_flag       :1;
    u8 rsv              :6;
}fw_receiver_list_t;

typedef struct{
	u8 blob_id[8];
#if NODE_DISTRIBUT_RETRY_FLOW_EN
	u32 wait_ack_tick;
	u8 wait_ack_retry_cnt;
	u8 rsv_wait;	// for align
#endif
	u16 adr_group;
	fw_receiver_list_t list[MESH_OTA_UPDATE_NODE_MAX];
#if WIN32
	u8 miss_mask[max2(MESH_OTA_CHUNK_NUM_MAX_CEIL, 32)];   // set enough RAM
#else
	u8 miss_mask[max2(MESH_OTA_CHUNK_NUM_MAX_CEIL, 8)];
#endif
	blob_block_start_t block_start;
	fw_distribut_upload_start_t upload_start;
	u8 upload_phase;
	u8 upload_progress	:7;
    u8 upload_type		:1;
	u8 upload_crc_ok;
	fw_distribut_start_t distribut_start;
	u8 distribut_reboot_flag_backup;
	u8 len_fw_id_upload_start;
	u8 rx_upload_start_flag;
#if UPLOAD_OOB_START_FUN_EN
	u8 upload_oob_start_flag;
#endif
#if PULL_LPN_REACHABLE_TIMER_EN
	u32 tick_lpn_reachable_timer;
	u8 lpn_reachable_timer_retry_cnt;
#endif
	u8 bk_size_log;  // for 4 byte align
	u16 node_cnt;    // update node cnt
	u16 node_num;    // for index; for check all
	u32 blob_size;
	u32 bk_size_current;
	u16 chunk_num;
	u16 adr_distr_node;
	u8 miss_chunk_test_flag;
#if MESH_OTA_TEST_SOME_CASE_EN
	u8 test_missing_partial_report;
#endif
    u8 distribut_apply_en;
#if (DISTRIBUTOR_NO_UPDATA_START_2_SELF)
    u8 distribut_apply_self_ok;
#endif
    u8 distribut_update_phase;
#if PTS_TEST_OTA_EN
    u8 distribut_update_phase_keep_flag; // not use now, to be removed.  //can not clear phase, even receive distribute cancel many times.
    u32 tick_dist_canceling;
    u16 update_cancle_addr;
    #if PTS_TEST_BLOB_TRANSFER_FLOW_EN
    u8 blob_flow_flag;
    #endif
#endif
	u8 retry_intv_for_lpn_100ms;	// unit:100ms, reliable retry interval, usually used for LPN.
	u8 percent_last;    // distribute progress
	u8 suspend_flag;  	// should not set phase to suspend, because need to restore. also pause mesh ota tx flow when GATT disconnect, untill APP resume. 
	u8 st_wait_flag;
	u8 st_distr;
	u8 ota_rcv_flag;    // step 1: private method of receiving update address list,
	u8 adr_set_flag;    // step 2: have received address list before, so there is no list within distribute start command.
}fw_distribut_srv_proc_t;

typedef struct{
	u32 blob_size;
	u32 crc_total;
	fw_update_start_t start;            // 0x08
	blob_block_start_t block_start;     // 0x1C:
	u32 blob_trans_start_tick;			// 0x20: for both blob transfer and blob block start
	u32 bk_size_current;
	#if MESH_OTA_PULL_MODE_EN
	u32 report_timer_tick;
	u32 pull_chunk_tick;
	u16 report_partital_dst_adr;
	u16 report_chunk_idx_last;
	u8 report_retry_cnt;
	#endif
	u16 client_mtu_size;                // 0x28 for push mode
	u8 blk_crc_tlk_mask[(MESH_OTA_BLOCK_MAX + 7)/8];
	u8 miss_mask[MESH_OTA_CHUNK_NUM_MAX_CEIL];
	u8 blob_block_rx_ok;                // 
	u8 blob_trans_busy;
	u8 blob_trans_rx_start_error;
	u8 blob_trans_phase;
	u8 blob_trans_phase_backup;	// backup phase before suspend
	u8 blob_block_get_retry_flag;
	#if PTS_TEST_OTA_EN
	u8 blob_info_get_flag;
	#endif
	u8 transfer_mode;					// mode of start, is enum value not BIT, even though they are the same.
	u8 bk_size_log;
    u8 update_phase     :3;
    u8 additional_info  :5;
    u8 bin_crc_type;
    u8 bin_crc_done;
    u8 reboot_flag_backup;
    u8 metadata_len;
	u8 busy;
}fw_update_srv_proc_t;

typedef struct{
    
}blob_trans_srv_proc_t;

enum{
    VC_MESH_OTA_ONLY_ONE_NODE_SELF = 1,
    VC_MESH_OTA_ONLY_ONE_NODE_CONNECTED = 2,
};


#if INITIATOR_CLIENT_EN
typedef struct{
	u16 adr_distributor;	// "0x0000" means local GATT connected address.
    fw_distribut_start_t distribut_start;
	u8 upload_ttl;
    u16 upload_timeout_base;
    u8 upload_blob_id[8];
    //u16 adr_receivers_lsit[];
}fw_initiator_start_t;  // vendor struct

typedef struct{
	u8 blob_id[8];
	fw_initiator_start_t init_start;
	fw_receiver_list_t list[MESH_OTA_UPDATE_NODE_MAX];
	u8 miss_mask[max2(MESH_OTA_CHUNK_NUM_MAX_CEIL, 32)];   // set enough RAM
	fw_receiver_list_t list_distributor;
	fw_distribut_upload_start_t upload_start;
	blob_block_start_t block_start;
	u8 bk_size_log;  // for 4 byte align
	u16 node_cnt;
	u16 node_num;    // for index; for check all
	u32 blob_size;
	u32 bk_size_current;
	u16 chunk_num;
	u8 percent_last;    // progress
	u8 pause_flag;  // pause mesh ota tx flow when GATT disconnect, untill APP resume.
	u8 st_wait_flag;
	u8 st_initiator;
}fw_initiator_proc_t;
#endif

// -----------
extern _align_4_ fw_distribut_srv_proc_t fw_distribut_srv_proc;
//extern _align_4_ fw_initiator_proc_t fw_initiator_proc;

static inline int is_apply_phase_success(u8 update_phase)
{
    return (UPDATE_PHASE_APPLYING_UPDATE == update_phase || UPDATE_PHASE_VERIFYING_SUCCESS == update_phase);
}

static inline int is_mesh_ota_tx_client_model(u32 id, bool4 sig_model)
{
    return (sig_model && ((SIG_MD_FW_UPDATE_C == id) || (SIG_MD_BLOB_TRANSFER_C == id)
                        || (SIG_MD_FW_DISTRIBUT_C == id)));
}

static inline int is_mesh_ota_master_wait_ack()
{
    return (fw_distribut_srv_proc.st_wait_flag);
}

static inline void clr_mesh_ota_master_wait_ack()
{
    fw_distribut_srv_proc.st_wait_flag = 0;
}

static inline int is_rx_upload_start_before()
{
    return fw_distribut_srv_proc.rx_upload_start_flag;
}

u32 set_bit_by_cnt(u8 *out, u32 len, u32 cnt);

static inline int is_buf_bit_set(u8 *buf, u32 num)
{
    return (buf[num / 8] & BIT(num % 8));
}

static inline int buf_bit_clear(u8 *buf, u32 num)
{
    return (buf[num / 8] &= (~ BIT(num % 8)));
}

int mesh_cmd_sig_fw_update_info_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_fw_update_info_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_fw_update_metadata_check(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_fw_update_metadata_check_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);

#if (DISTRIBUTOR_UPDATE_SERVER_EN)
int mesh_cmd_sig_fw_distribut_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_fw_distribut_start(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_fw_distribut_suspend(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_fw_distribut_cancel(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_fw_distribut_apply(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_fw_distribut_receiver_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_fw_distribut_capabilities_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_fw_distribut_receivers_add(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_fw_distribut_receivers_del_all(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_fw_distribut_upload_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_fw_distribut_upload_start(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_fw_distribut_upload_oob_start(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_fw_distribut_upload_cancel(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_fw_distribut_fw_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_fw_distribut_fw_get_by_idx(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_fw_distribut_fw_del(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_fw_distribut_fw_del_all(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
#else
#define mesh_cmd_sig_fw_distribut_get               0
#define mesh_cmd_sig_fw_distribut_start             0
#define mesh_cmd_sig_fw_distribut_suspend           0
#define mesh_cmd_sig_fw_distribut_cancel            0
#define mesh_cmd_sig_fw_distribut_apply             0
#define mesh_cmd_sig_fw_distribut_receiver_get      0
#define mesh_cmd_sig_fw_distribut_capabilities_get  0
#define mesh_cmd_sig_fw_distribut_receivers_add     0
#define mesh_cmd_sig_fw_distribut_receivers_del_all 0
#define mesh_cmd_sig_fw_distribut_upload_get        0
#define mesh_cmd_sig_fw_distribut_upload_start      0
#define mesh_cmd_sig_fw_distribut_upload_oob_start  0
#define mesh_cmd_sig_fw_distribut_upload_cancel   	0
#define mesh_cmd_sig_fw_distribut_fw_get   			0
#define mesh_cmd_sig_fw_distribut_fw_get_by_idx   	0
#define mesh_cmd_sig_fw_distribut_fw_del   			0
#define mesh_cmd_sig_fw_distribut_fw_del_all   		0
#endif

#if (DISTRIBUTOR_UPDATE_CLIENT_EN)
int mesh_cmd_sig_fw_distribut_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_fw_distribut_receiver_list(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_fw_distribut_capabilities_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_fw_distribut_receivers_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_fw_distribut_upload_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_fw_distribut_fw_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
#else
#define mesh_cmd_sig_fw_distribut_status            0
#define mesh_cmd_sig_fw_distribut_receiver_list     0
#define mesh_cmd_sig_fw_distribut_capabilities_status   0
#define mesh_cmd_sig_fw_distribut_receivers_status  0
#define mesh_cmd_sig_fw_distribut_upload_status     0
#define mesh_cmd_sig_fw_distribut_fw_status     	0
#endif
int mesh_cmd_sig_fw_update_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_fw_update_start(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_fw_update_cancel(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_fw_update_apply(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_fw_update_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_blob_transfer_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_blob_transfer_handle(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_blob_transfer_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_blob_block_start(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_blob_partial_block_report(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_blob_chunk_transfer(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_blob_info_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_blob_info_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_blob_block_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_blob_block_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);

// ---
int access_cmd_fw_update_control(u16 adr_dst, u16 op, u8 rsp_max);

// ---
int mesh_ota_master_rx (mesh_rc_rsp_t *rsp, u16 op, u32 size_op);
u32 new_fw_read(u8 *data_out, u32 max_len);
void new_fw_write_file(u8 *data_in, u32 len);
void mesh_ota_master_proc();
void get_fw_id();
u32 get_fw_len();
void mesh_ota_master_ack_timeout_handle();
u8 get_ota_check_type();
u32 get_total_crc_type1_new_fw();
int is_valid_ota_check_type1();
u32 get_blk_crc_tlk_type1(u8 *data, u32 len, u32 addr);
int ota_file_check();
void APP_RefreshProgressBar(u16 bk_current, u16 bk_total, u16 chunk_cur, u16 chunk_total, u8 percent);
void APP_report_mesh_ota_apply_status(u16 adr_src, fw_update_status_t *p);
u16 APP_get_GATT_connect_addr();
void APP_set_mesh_ota_pause_flag(u8 val);
int mesh_ota_and_only_one_node_check();
u32 soft_crc32_ota_flash(u32 addr, u32 len, u32 crc_init,u32 *out_crc_type1_blk);
int mesh_ota_slave_need_ota(fw_metadata_t *p_metadata, int len);
unsigned short crc16(unsigned char *pD, int len);
void mesh_ota_read_data(u32 adr, u32 len, u8 * buf);
void mesh_ota_proc();
int is_blob_chunk_transfer_ready();
void mesh_fw_distibut_set(u8 en);
int is_par_distribut_start_tlk(u8 *par, int par_len);
int get_fw_metadata(fw_metadata_t *metadata_out);
u16 get_chunk_cmd_dst_addr();
int is_fw_update_start_before();
void mesh_ota_blob_transfer_flow_start(u8 *par, int par_len);
int is_mesh_ota_flow_with_user_define_intv(u16 op);
void mesh_ota_check_and_set_lpn_retry_intv(mesh_bulk_cmd_par_t *p_cmd, int len);
int mesh_ota_cb_tx_cmd_dev_app_key_not_found(material_tx_cmd_t *p_mat);
int is_ota_metadata_include_fw_id(fw_metadata_t *p_metadata, int len);
int is_use_ota_push_mode_for_lpn(fw_distribut_srv_proc_t *distr_proc);

#if INITIATOR_CLIENT_EN
#define DISTRIBUTION_APPKEY_INDEX       (DEFAULT_DISTRIBUTION_APPKEY_INDEX) // default
#define DISTRIBUTION_TIMEOUT_BASE_10MS  (0xffff) // unit: 10ms, client value: =(10,000*(Update Client Timeout Base+2))+(100*Update TTL) [milliseconds]
#define DISTRIBUTION_FW_IMAGE_INDEX     (0)      // only one image now

void mesh_ota_initiator_start(u8 *par, int par_len);
int mesh_ota_initiator_rx (mesh_rc_rsp_t *rsp, u16 op, u32 size_op);
void mesh_ota_initiator_proc();
int is_mesh_ota_initiator_tx_client_model(u32 id, bool4 sig_model);
int is_mesh_ota_initiator_wait_ack();
void clr_mesh_ota_initiator_wait_ack();
void mesh_ota_initiator_next_st_set(u8 st);
void mesh_ota_initiator_wait_ack_st_set();
void mesh_ota_initiator_wait_ack_st_return(int success);
void mesh_ota_initiator_ack_timeout_handle();

u8 get_fw_initiator_proc_st();

int is_fwid_match(fw_id_t *id1, fw_id_t *id2);
int is_blob_id_match(const u8 *blob_id1, const u8 *blob_id2);
u16 get_fw_block_cnt(u32 blob_size, u8 bk_size_log);
u32 get_block_size(u32 blob_size, u8 bk_size_log, u16 block_num);
u16 get_fw_chunk_cnt(u32 bk_size_current, u16 chunk_size_max);
u16 get_chunk_size(u32 bk_size_current, u16 chunk_size_max, u16 chunk_num);
u32 get_fw_data_position(u16 block_num, u8 bk_size_log, u16 chunk_num, u16 chunk_size_max);
void mesh_ota_tx_fw_data_read(u32 addr, u32 len, u8 *buf_out);
int read_ota_file2buffer();


int access_cmd_fw_update_info_get(u16 adr_dst);
int access_cmd_fw_update_metadata_check(u16 adr_dst, fw_metadata_t *metadata);
int access_cmd_blob_transfer_get(u16 adr_dst);
int access_cmd_blob_transfer_start(u16 adr_dst, u8 transfer_mode, u32 blob_size, u8 bk_size_log);
int access_cmd_blob_block_start(u16 adr_dst, u16 block_num);
int access_cmd_blob_chunk_transfer(u16 adr_dst, u8 *cmd, u32 len);
int access_cmd_blob_block_get(u16 adr_dst, u16 block_num);
int access_cmd_blob_info_get(u16 adr_dst);
int access_cmd_fw_distribut_cancel(u16 adr_dst);
#endif

#if 1 // utf8
u32 U16ToUTF8(u16 UniChar, u8 *OutUTFString, int OutUTFLenMax);
u32 UTF8StrToU16( const u8 * UTF8String, int UTF8StringLength, u8* OutUnicodeString, int UnicodeStringBufferSize, u16 *OutUTF8LenCost );
u16 UTF8StrGetOneU16( const u8 * UTF8String, int UTF8StringLength, u16 *OutU16 );
#define UTF8_SIZE_MAX_U16           (3)
#endif
void decode_miss_chunk(u8 *in_encode, int encode_len, u8 *out_mask, u32 out_mask_len);
int ota_is_valid_pid_vid(fw_id_t *p_fw_id, int gatt_flag);
u32 get_missing_chunk_utf8(u8 *out_utf8, int len_out_max, u32 out_cnt_max, u16 chunk_cnt_total, u16 *out_last_chunk);

extern _align_4_ const fw_id_t fw_id_local;
extern u32	ota_program_offset;


