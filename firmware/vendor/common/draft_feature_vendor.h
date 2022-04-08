/********************************************************************************************************
 * @file	draft_feature_vendor.h
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

//---------------------------------- model ID -----------------------
#define SIG_MD_REMOTE_PROV_SERVER       ((0x0004 << 16) | (VENDOR_ID))
#define SIG_MD_REMOTE_PROV_CLIENT       ((0x0005 << 16) | (VENDOR_ID))

#define SIG_MD_FW_UPDATE_S              ((0xFE00 << 16) | (VENDOR_ID))
#define SIG_MD_FW_UPDATE_C              ((0xFE01 << 16) | (VENDOR_ID))
#define SIG_MD_FW_DISTRIBUT_S           ((0xFE02 << 16) | (VENDOR_ID))
#define SIG_MD_FW_DISTRIBUT_C           ((0xFE03 << 16) | (VENDOR_ID))
#define SIG_MD_BLOB_TRANSFER_S        	((0xFF00 << 16) | (VENDOR_ID))
#define SIG_MD_BLOB_TRANSFER_C         	((0xFF01 << 16) | (VENDOR_ID))

//----------------------------------- op code -----------------------
// op cmd 11xxxxxx yyyyyyyy yyyyyyyy (vendor id)

//----------- Mesh Remote Provision -----------
#define REMOTE_PROV_SCAN_CAPA_GET       0xC0
#define REMOTE_PROV_SCAN_CAPA_STS       0xC1
#define REMOTE_PROV_SCAN_GET            0xC2
#define REMOTE_PROV_SCAN_START          0xC3
#define REMOTE_PROV_SCAN_STOP           0xC4
#define REMOTE_PROV_SCAN_STS            0xC5
#define REMOTE_PROV_SCAN_REPORT         0xC6
#define REMOTE_PROV_EXTEND_SCAN_START   0xC7
#define REMOTE_PROV_EXTEND_SCAN_REPORT  0xC8
#define REMOTE_PROV_LINK_GET            0xC9
#define REMOTE_PROV_LINK_OPEN           0xCA
#define REMOTE_PROV_LINK_CLOSE          0xCB
#define REMOTE_PROV_LINK_STS            0xCC
#define REMOTE_PROV_LINK_REPORT         0xCD
#define REMOTE_PROV_PDU_SEND            0xCE
#define REMOTE_PROV_PDU_OUTBOUND_REPORT 0xCF
#define REMOTE_PROV_PDU_REPORT          0xD0
//----------- Mesh OTA ---------------------
#define FW_UPDATE_INFO_GET		        0xDC
#define FW_UPDATE_INFO_STATUS		    0xDD
#define FW_UPDATE_METADATA_CHECK	    0xDE
#define FW_UPDATE_METADATA_CHECK_STATUS 0xDF
#define FW_UPDATE_GET		        	0xE0
#define FW_UPDATE_START		        	0xE1
#define FW_UPDATE_CANCEL		        0xE2
#define FW_UPDATE_APPLY		        	0xE3
#define FW_UPDATE_STATUS		        0xE4
#if WIN32
#define FW_DISTRIBUT_SUSPEND		    0xF7
#define FW_DISTRIBUT_GET		        0xF8    // only be use in VC.
#define FW_DISTRIBUT_START		        0xF9
#define FW_DISTRIBUT_CANCEL		        0xFA
#define FW_DISTRIBUT_APPLY		        0xFB
#define FW_DISTRIBUT_STATUS		        0xFC
#define FW_DISTRIBUT_RECEIVERS_GET		0xFD
#define FW_DISTRIBUT_RECEIVERS_LIST		0xFE
#endif

#define BLOB_TRANSFER_GET		        0xD1
#define BLOB_TRANSFER_START		        0xD2
#define BLOB_TRANSFER_CANCEL		    0xD3
#define BLOB_TRANSFER_STATUS		    0xD4
#define BLOB_BLOCK_GET		            0xD5
#define BLOB_BLOCK_START		        0xD6
#define BLOB_BLOCK_STATUS		        0xD7
#define BLOB_PARTIAL_BLOCK_REPORT	    0xDB    // used for pull mode(LPN)
#define BLOB_CHUNK_TRANSFER		        0xD8
#define BLOB_INFO_GET		            0xD9
#define BLOB_INFO_STATUS		        0xDA




