/********************************************************************************************************
 * @file	telink_platform.c
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
#include "tl_common.h"
#include "proj_lib/sig_mesh/app_mesh.h"
#include "telink_platform.h"


#if PLATFORM_TELINK_EN
#define TC_PROTOCOL_FLAG						(0x0211)

#define TLK_PLATFORM_CID						(0x0001)

typedef struct{
	u16 tc_protocol_flag;	// fixed 0x0211, tc means Telink cloud.
	// -- Protocol Feature 1byte
	u8 	uuid_ver			:2; // UUID version, initial version is 0
	u8 	static_oob_en	:1; // 0: no static oob; 1: static oob
	u8 	key_bind_needed	:1; // 0: no need to send key bind from APP. 1: need.
	u8 	feature_rfu		:4;
	// -- Protocol Feature end
	u16 tc_cid; 			// TC company ID, Telink assign. 0x0000 for reserve, 0x0001 ~ 0x000f for telink, 0x0010 ~ 0xfffe will be assigned to coustomer.
	u16 pid;				// product ID of composition data. // App will get model list by PID and VID of cps.
	u8	mac[6];				// mac address, little endianess // TODO: set random value to the highest 3 bytes of mac to enhance uniqueness of device UUID ? make no sense, because APP will failed to do GATT connect if there is two nodes with the same mac.
	u8 	rfu[2];				// 
	u8 	check_sum;
}platform_telink_dev_uuid_t;

STATIC_ASSERT(sizeof(platform_telink_dev_uuid_t) == 16);

u32 get_check_sum(const u8 *data, int len)
{
	u32 sum = 0;
	foreach(i, len){
		sum += data[i];
	}
	return sum;
}

void set_dev_uuid_for_platform_telink( u8 *p_uuid)
{
    platform_telink_dev_uuid_t *p_dev_uuid = (platform_telink_dev_uuid_t *)p_uuid;
    memset(p_uuid, 0, 16);
    p_dev_uuid->tc_protocol_flag = TC_PROTOCOL_FLAG;
    p_dev_uuid->tc_cid = TLK_PLATFORM_CID;
    p_dev_uuid->pid = gp_page0->head.pid;
    memcpy(p_dev_uuid->mac, tbl_mac, sizeof(p_dev_uuid->mac));
    // p_dev_uuid->uuid_ver = 0;
    // p_dev_uuid->static_oob_en = 0;
    #if (PROVISION_FLOW_SIMPLE_EN == 0)
    p_dev_uuid->key_bind_needed = 1;
    #endif
    p_dev_uuid->check_sum = (u8)get_check_sum((u8 *)p_dev_uuid, OFFSETOF(platform_telink_dev_uuid_t, check_sum));
}
#endif

