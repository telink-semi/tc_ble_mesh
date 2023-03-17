/********************************************************************************************************
 * @file	lighting_model_xyL.h
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

#include "proj/tl_common.h"
#include "proj_lib/sig_mesh/app_mesh.h"

//----------------------------------- op code
// op cmd 0xxxxxxx (SIG)

// op cmd 10xxxxxx xxxxxxxx (SIG)
#define LIGHT_XYL_GET		        	0x8382
#define LIGHT_XYL_SET		        	0x8482
#define LIGHT_XYL_SET_NOACK		    	0x8582
#define LIGHT_XYL_STATUS		    	0x8682
#define LIGHT_XYL_TARGET_GET		    0x8782
#define LIGHT_XYL_TARGET_STATUS		    0x8882
#define LIGHT_XYL_DEF_GET		    	0x8982
#define LIGHT_XYL_DEF_STATUS		    0x8A82
#define LIGHT_XYL_RANGE_GET		    	0x8B82
#define LIGHT_XYL_RANGE_STATUS		    0x8C82
#define LIGHT_XYL_DEF_SET		    	0x8D82
#define LIGHT_XYL_DEF_SET_NOACK		    0x8E82
#define LIGHT_XYL_RANGE_SET		    	0x8F82
#define LIGHT_XYL_RANGE_SET_NOACK		0x9082

//------op parameters

//------------------vendor op end-------------------

typedef struct{
	u16 lightness;
	u16 x;
	u16 y;
	u8 tid;
	u8 transit_t;
	u8 delay;		// unit 5ms
}mesh_cmd_light_xyl_set_t;

typedef struct{
	u16 lightness;
	u16 x;
	u16 y;
	u8 remain_t;
}mesh_cmd_light_xyl_st_t;

typedef struct{
	u16 x;
	u8 tid;
	u8 transit_t;
	u8 delay;		// unit 5ms
}mesh_cmd_light_x_set_t;

typedef struct{
	u16 y;
	u8 tid;
	u8 transit_t;
	u8 delay;		// unit 5ms
}mesh_cmd_light_y_set_t;

typedef struct{
	u16 lightness;
	u16 x;
	u16 y;
}mesh_cmd_light_xyl_def_set_t;

typedef struct{
	u16 lightness;
	u16 x;
	u16 y;
}mesh_cmd_light_xyl_def_st_t;

typedef struct{
	u16 x_min;
	u16 x_max;
	u16 y_min;
	u16 y_max;
}mesh_cmd_light_xyl_range_set_t;

typedef struct{
	u8 st;
	u16 x_min;
	u16 x_max;
	u16 y_min;
	u16 y_max;
}mesh_cmd_light_xyl_range_st_t;


// -----------
int light_x_set(mesh_cmd_light_x_set_t *p_set, int par_len, u16 op, int idx, bool4 retransaction, st_pub_list_t *pub_list);
int light_y_set(mesh_cmd_light_y_set_t *p_set, int par_len, u16 op, int idx, bool4 retransaction, st_pub_list_t *pub_list);

int mesh_light_xyl_st_publish(u8 idx);
#if MD_SERVER_EN
int mesh_cmd_sig_light_xyl_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_light_xyl_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_light_xyl_target_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_light_xyl_def_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_light_xyl_def_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_light_xyl_range_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_light_xyl_range_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
#else
#define mesh_cmd_sig_light_xyl_get              (0)
#define mesh_cmd_sig_light_xyl_set              (0)
#define mesh_cmd_sig_light_xyl_target_get       (0)
#define mesh_cmd_sig_light_xyl_def_get          (0)
#define mesh_cmd_sig_light_xyl_def_set          (0)
#define mesh_cmd_sig_light_xyl_range_get        (0)
#define mesh_cmd_sig_light_xyl_range_set        (0)
#endif

#if MD_CLIENT_EN
int mesh_cmd_sig_light_xyl_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_light_xyl_target_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_light_xyl_def_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_light_xyl_range_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
#else
#define mesh_cmd_sig_light_xyl_status           (0)
#define mesh_cmd_sig_light_xyl_target_status    (0)
#define mesh_cmd_sig_light_xyl_def_status       (0)
#define mesh_cmd_sig_light_xyl_range_status     (0)
#endif

