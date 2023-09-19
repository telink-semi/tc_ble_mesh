/********************************************************************************************************
 * @file	generic_model.h
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

#include "../../proj/tl_common.h"
#include "../../proj_lib/sig_mesh/app_mesh.h"

/** @addtogroup Mesh_Common
  * @{
  */


/** @addtogroup General_Model
  * @brief General Models Code.
  * @{
  */


//----------------------------------- op code
// op cmd 0xxxxxxx (SIG)

// op cmd 10xxxxxx xxxxxxxx (SIG)

//------------------vendor op end-------------------

extern u16 size_page0;


int g_onoff_set(mesh_cmd_g_onoff_set_t *p_set, int par_len, int force_last, int idx, bool4 retransaction, st_pub_list_t *pub_list);
int mesh_tx_cmd_g_onoff_st(u8 idx, u16 ele_adr, u16 dst_adr, u8 *uuid, model_common_t *pub_md, u16 op_rsp);


/**
  * @}
  */
    
/**
  * @}
  */


#if (!MD_SERVER_EN)
//----generic onoff
#define mesh_cmd_sig_g_onoff_get            (0)
#define mesh_cmd_sig_g_onoff_set            (0)
//----generic level
#define mesh_cmd_sig_g_level_get            (0)
//generic default transition time
#define mesh_cmd_sig_def_trans_time_get     (0)
#define mesh_cmd_sig_def_trans_time_set     (0)
//generic power onoff
#define mesh_cmd_sig_g_on_powerup_get       (0)
#define mesh_cmd_sig_g_on_powerup_set       (0)
//----generic power
#define mesh_cmd_sig_g_power_get            (0)
#define mesh_cmd_sig_g_power_set            (0)
//----generic power last
#define mesh_cmd_sig_g_power_last_get       (0)
//----generic power default
#define mesh_cmd_sig_g_power_def_get        (0)
#define mesh_cmd_sig_g_power_def_set        (0)
//----generic power range
#define mesh_cmd_sig_g_power_range_get      (0)
#define mesh_cmd_sig_g_power_range_set      (0)
//battery model
#define mesh_cmd_sig_battery_get            (0)
#define mesh_cmd_sig_g_battery_get          (0)
//location model
#define mesh_cmd_sig_g_location_global_get  (0)
#define mesh_cmd_sig_g_location_local_get   (0)
#define mesh_cmd_sig_g_location_global_set  (0)
#define mesh_cmd_sig_g_global_local_set     (0)
#endif

#if (!MD_CLIENT_EN)
#define mesh_cmd_sig_g_level_status         (0)
#define mesh_cmd_sig_def_trans_time_status  (0)
#define mesh_cmd_sig_g_on_powerup_status    (0)
#define mesh_cmd_sig_g_power_status         (0)
#define mesh_cmd_sig_g_power_last_status    (0)
#define mesh_cmd_sig_g_power_def_status     (0)
#define mesh_cmd_sig_g_power_range_status   (0)
#define mesh_cmd_sig_g_battery_status       (0)
#define mesh_cmd_sig_g_location_global_status   (0)
#define mesh_cmd_sig_g_location_local_status    (0)
#endif
// -----------

