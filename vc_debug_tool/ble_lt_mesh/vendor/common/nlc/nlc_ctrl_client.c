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

#define NLC_DICMP_GROUP_ADDR_START 					0xC000


/**
 * when App enable "Enable Subscription Level Service model Id" of "home page" -- "Setting", 
 * 0xD000 ~ 0xD00F is belong to level model for "Living room"
 *                 0xD000 for level of lightness, 0xD001 for level of temperature, 
 *                 0xD002 for level of Hue, 0xD003 for level of Saturation, others reserved for future.
 * 0xD010 ~ 0xD01F is belong to level model for "Kitchen room"
 *                 0xD010 for level of lightness, 0xD011 for level of temperature, 
 *                 0xD012 for level of Hue, 0xD013 for level of Saturation, others reserved for future.
 * ......
 */
#define NLC_DICMP_GROUP_ADDR_START_LEVEL_MODEL 		0xD000


void mesh_scan_rsp_add_local_name(u8 *p_name, u32 len_max)
{
	memcpy(p_name, NLC_DICMP_LOCAL_NAME, min(len_max, sizeof(NLC_DICMP_LOCAL_NAME) - 1));
}

u16 dicmp_get_publish_addr(u32 ele_offset, u32 model_id_sig_key, bool4 is_level_model)
{
	u16 pub_addr = is_level_model ? NLC_DICMP_GROUP_ADDR_START_LEVEL_MODEL : NLC_DICMP_GROUP_ADDR_START;
	model_common_t *p_model;
	u8 model_idx = 0;
	p_model = (model_common_t *)mesh_find_ele_resource_in_model(ele_adr_primary + ele_offset, model_id_sig_key, 1, &model_idx, 0);
	if(p_model && p_model->pub_adr){
		pub_addr = p_model->pub_adr;
	}else{
		pub_addr += ele_offset * (is_level_model ? 16 : 1); // adapt for app setting. refer to "Networked Lighting Control(NLC)" of "AN_17120401-C5_Telink SIG Mesh SDK Developer Handbook.pdf"
	}
	return pub_addr;
}

extern u16 sw_tx_src_addr_offset;
void dicmp_switch_send_publish_command(u32 ele_offset, bool4 onoff, u32 select_pub_model_key)
{
#if NLCP_BSS_EN
	if(3 == ele_offset){
		u32 ele_offset_scene = 0;
		if(onoff){	// RC_KEY_4_ON
			ele_offset_scene = 0;
			u16 pub_addr = dicmp_get_publish_addr(ele_offset_scene, SIG_MD_SCENE_C, 0);
			sw_tx_src_addr_offset = ele_offset_scene;
			access_cmd_scene_store(pub_addr, 0, 1, 0);
		}else{		// RC_KEY_4_OFF
			ele_offset_scene = 0;
			u16 pub_addr = dicmp_get_publish_addr(ele_offset_scene, SIG_MD_SCENE_C, 0);
			sw_tx_src_addr_offset = ele_offset_scene;
			access_cmd_scene_recall(pub_addr, 0, 1, 0, 0);
		}
	}
	else
#endif
	{
#if NLCP_DIC_EN
		//sw_tx_src_addr_offset = ele_offset;
		if((RC_KEY_UP == select_pub_model_key)||(RC_KEY_DN == select_pub_model_key)){
			s32 delta = DICMP_LEVEL_DELTA_VALUE;
			if(!onoff){
				delta *= -1;
			}
			u16 pub_addr = dicmp_get_publish_addr(ele_offset, SIG_MD_G_LEVEL_C, 1);
			access_cmd_set_delta(pub_addr, 0, delta, CMD_NO_ACK, 0);
		}else if((RC_KEY_L == select_pub_model_key)||(RC_KEY_R == select_pub_model_key)){
			s16 move = 65536/4;
			if(!onoff){
				move *= -1;
			}
			u16 pub_addr = dicmp_get_publish_addr(ele_offset, SIG_MD_G_LEVEL_C, 1);
			transition_par_t trs;
			trs.delay = 0;
			trs.res = TRANSITION_STEP_RES_100MS;
			trs.steps = 10;
			access_cmd_set_level_move(pub_addr, 0, move, CMD_NO_ACK, &trs);
		}else{ // SIG_MD_G_ONOFF_C
			u16 pub_addr = dicmp_get_publish_addr(ele_offset, SIG_MD_G_ONOFF_C, 0);
			access_cmd_onoff(pub_addr, 0, onoff ? G_ON : G_OFF, CMD_NO_ACK, 0);
		}
#endif
	}
}
#endif

