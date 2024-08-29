/********************************************************************************************************
 * @file	nlc_sensor_model_config.h
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

#include "../mesh_config.h"

/**
 * Note: do not define enum or struct here. because this file will be included in cStartup.S.
 */

#if (LIGHT_TYPE_SEL == LIGHT_TYPE_NLC_SENSOR)

#define NLCP_TYPE_ALS				1 // ALSNLCP: Ambient Light Sensor Network Lighting Profile
#define NLCP_TYPE_OCS 				2 // OCSNLCP: Occupancy Sensor Network Lighting Profile
#define NLCP_TYPE_ENM				3 // ENMNLCP: Energy Monitor Network Lighting Profile

#define NLC_SENSOR_TYPE_SEL			NLCP_TYPE_OCS
// ------------------------- NLC config --------------
#define NLC_PROFILE_EN				1
#if NLC_PROFILE_EN
	#if(NLC_SENSOR_TYPE_SEL == NLCP_TYPE_ALS)
#define NLC_LOCAL_NAME				"ALSMP"
#define NLC_PROFILE_ID				NLC_PROFILE_ID_ALSMP
	#elif(NLC_SENSOR_TYPE_SEL == NLCP_TYPE_OCS)
#define NLC_LOCAL_NAME				"OCSMP"
#define NLC_PROFILE_ID				NLC_PROFILE_ID_OCSMP
	#elif(NLC_SENSOR_TYPE_SEL == NLCP_TYPE_ENM)
#define NLC_LOCAL_NAME				"ENMMP"
#define NLC_PROFILE_ID				NLC_PROFILE_ID_ENMMP
	#endif
#define NLC_LOCAL_NAME_LEN			sizeof(NLC_LOCAL_NAME)
#endif

// ------------------------- model config --------------
#define LIGHT_TYPE_CT_EN            0

#define LIGHT_TYPE_HSL_EN           0

#if ((LIGHT_TYPE_SEL == LIGHT_TYPE_DIM) || (LIGHT_TYPE_CT_EN) || (LIGHT_TYPE_HSL_EN))
#define MD_LIGHT_CONTROL_EN			0
#else
#define MD_LIGHT_CONTROL_EN			0	// must 0
#endif

#define MD_LIGHTNESS_EN             0
#define MD_LEVEL_EN                 0

#define CMD_LINEAR_EN               1

#define CERTIFY_BASE_ENABLE			0

#ifndef MD_MESH_OTA_EN
#define MD_MESH_OTA_EN				0   // dufault disable before released by SIG.
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

#ifndef OTA_ADOPT_RULE_CHECK_PID_EN			// for both GATT and ADV
#define OTA_ADOPT_RULE_CHECK_PID_EN		1
#define OTA_ADOPT_RULE_CHECK_VID_EN		0
#endif

#if (DISTRIBUTOR_UPDATE_SERVER_EN && (DISTRIBUTOR_UPDATE_CLIENT_EN || PTS_TEST_OTA_EN)) // because PTS testing OTA update server will enable PTS_TEST_BLOB_TRANSFER_FLOW_EN. 
#define DISTRIBUTOR_START_TLK_EN        1   // only used in internal to be compatible with old version INI. 
#endif

#define MD_ONOFF_EN                 1
#define MD_PRIVACY_BEA				0
#define PRIVATE_PROXY_FUN_EN		0

// light node
#define MD_DEF_TRANSIT_TIME_EN      0
#define MD_POWER_ONOFF_EN           MD_DEF_TRANSIT_TIME_EN 	// because both of them save in same flash sector.

#define MD_TIME_EN                  0
#define MD_SCENE_EN                 0
#define MD_SCHEDULE_EN              MD_TIME_EN  // because both of them save in same flash sector.
#define MD_PROPERTY_EN				0
#define	MD_LOCATION_EN				0	// location,sensor,battery use same flash addr, but one sector max store 6 models
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
#define MD_CLIENT_EN                0   // just SIG models, and exclude sensor model.
#define MD_CLIENT_VENDOR_EN         0
#define MD_VENDOR_2ND_EN            0
#define MD_SENSOR_SERVER_EN			1	// MD_SERVER_EN
#define MD_SENSOR_CLIENT_EN		    0	// MD_CLIENT_EN

#ifndef MD_REMOTE_PROV
#define MD_REMOTE_PROV              0   // must 0, other project not support now. dufault disable before released by SIG.
#endif

#if	MD_REMOTE_PROV
#define REMOTE_PROV_SCAN_GATT_EN	0
#define URI_DATA_ADV_ENABLE			0
#define ACTIVE_SCAN_ENABLE  		0
	#if TESTCASE_FLAG_ENABLE
#define REMOTE_SET_RETRY_EN			0	
	#else
#define REMOTE_SET_RETRY_EN			1
	#endif
#else
#define REMOTE_PROV_SCAN_GATT_EN	0
#define URI_DATA_ADV_ENABLE			0
#define ACTIVE_SCAN_ENABLE  		0
#endif
#endif

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
