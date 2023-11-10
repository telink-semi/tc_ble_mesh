/********************************************************************************************************
 * @file	nlc_ctrl_client.c
 *
 * @brief	for TLSR chips
 *
 * @author	telink
 * @date	April. 21, 2023
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
#include "vendor/common/scene.h"
#include "nlc_ctrl_client.h"

/**
 *	include: DICMP(Dimming Control Mesh Profile),BSSMP(Basic Scene Selector Mesh Profile).
 */

#if (LIGHT_TYPE_SEL == LIGHT_TYPE_NLC_CTRL_CLIENT)

#define DICMP_LEVEL_DELTA_VALUE				(65536/20)
const u8 NLC_DICMP_LOCAL_NAME[NLC_LOCAL_NAME_LEN + 1] = {"DICMP"};

#define NLC_DICMP_GROUP_ADDR_START 			0xC000


void mesh_scan_rsp_add_local_name(u8 *p_name, u32 len_max)
{
	memcpy(p_name, NLC_DICMP_LOCAL_NAME, min(len_max, sizeof(NLC_DICMP_LOCAL_NAME) - 1));
}

u16 dicmp_get_publish_addr(u32 ele_offset, u32 model_id_sig_key)
{
	u16 pub_addr = NLC_DICMP_GROUP_ADDR_START;
	model_common_t *p_model;
	u8 model_idx = 0;
	p_model = (model_common_t *)mesh_find_ele_resource_in_model(ele_adr_primary + ele_offset, model_id_sig_key, 1, &model_idx, 0);
	if(p_model && p_model->pub_adr){
		pub_addr = p_model->pub_adr;
	}else{
		pub_addr += ele_offset;
	}
	return pub_addr;
}

void dicmp_switch_send_publish_command(u32 ele_offset, bool4 onoff, u32 select_pub_model_key)
{
#if NLCP_BSS_EN
	if(3 == ele_offset){
		if(onoff){	// RC_KEY_4_ON
			u16 pub_addr = dicmp_get_publish_addr(0, SIG_MD_SCENE_C);
			access_cmd_scene_recall(pub_addr, 0, 1, 0, 0);
		}else{		// RC_KEY_4_OFF
			u16 pub_addr = dicmp_get_publish_addr(1, SIG_MD_SCENE_C);
			access_cmd_scene_recall(pub_addr, 0, 2, 0, 0);
		}
	}
	else
#endif
	{
#if NLCP_DIC_EN
		if((RC_KEY_UP == select_pub_model_key)||(RC_KEY_DN == select_pub_model_key)){
			s32 delta = DICMP_LEVEL_DELTA_VALUE;
			if(!onoff){
				delta *= -1;
			}
			u16 pub_addr = dicmp_get_publish_addr(ele_offset, SIG_MD_G_LEVEL_C);
			access_cmd_set_delta(pub_addr, 0, delta, CMD_NO_ACK, 0);
		}else if((RC_KEY_L == select_pub_model_key)||(RC_KEY_R == select_pub_model_key)){
			s16 move = DICMP_LEVEL_DELTA_VALUE;
			if(!onoff){
				move *= -1;
			}
			u16 pub_addr = dicmp_get_publish_addr(ele_offset, SIG_MD_G_LEVEL_C);
			access_cmd_set_level_move(pub_addr, 0, move, CMD_NO_ACK, 0);
		}else{ // SIG_MD_G_ONOFF_C
			u16 pub_addr = dicmp_get_publish_addr(ele_offset, SIG_MD_G_ONOFF_C);
			access_cmd_onoff(pub_addr, 0, onoff ? G_ON : G_OFF, CMD_NO_ACK, 0);
		}
#endif
	}
}
#endif

