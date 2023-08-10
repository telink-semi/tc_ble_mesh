/********************************************************************************************************
 * @file	scene.h
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

//----------------------------------- op code
// op cmd 0xxxxxxx (SIG)

// op cmd 10xxxxxx xxxxxxxx (SIG)
#define SCENE_GET		        		0x4182
#define SCENE_RECALL		        	0x4282
#define SCENE_RECALL_NOACK		        0x4382
#define SCENE_STATUS		    		0x5E
#define SCENE_REG_GET		        	0x4482
#define SCENE_REG_STATUS		        0x4582
#define SCENE_STORE		        		0x4682
#define SCENE_STORE_NOACK		    	0x4782
#define SCENE_DEL		        		0x9E82
#define SCENE_DEL_NOACK		    		0x9F82

//------op parameters
enum{
	SCENE_ST_SUCCESS 	= 0,
	SCENE_ST_REG_FULL	= 1,
	SCENE_ST_NOT_FOUND	= 2,
	SCENE_ST_MAX,
};

enum{
	SCENE_ID_INVALID 	= 0,
};

typedef struct{
	u8 st;
	u16 current_id;
	u16 id[SCENE_CNT_MAX];
}scene_reg_status_t;

typedef struct{
	u16 id;
	u8 tid;
	u8 transit_t;
	u8 delay;		// unit 5ms
}scene_recall_t;

typedef struct{
	u8 st;
	u16 current_id;
	u16 target_id;
	u8 remain_t;
}scene_status_t;


typedef struct{
	u16 current_scene;
	u16 target_scene;
}scene_proc_t;


extern _align_4_ scene_proc_t	scene_proc[LIGHT_CNT];
extern u8 tansition_forced_by_recall_flag;
//------------------vendor op end-------------------


// -----------
void scene_target_complete_check(int idx);
void scene_status_change_check(int idx, s16 present_level, int st_trans_type);
int mesh_scene_st_publish(u8 idx);

#if MD_SERVER_EN
int mesh_cmd_sig_scene_reg_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_scene_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_scene_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_scene_recall(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
#else
#define mesh_cmd_sig_scene_reg_get              (0)
#define mesh_cmd_sig_scene_set                  (0)
#define mesh_cmd_sig_scene_get                  (0)
#define mesh_cmd_sig_scene_recall               (0)
#endif
#if MD_CLIENT_EN
int mesh_cmd_sig_scene_reg_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_scene_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
#else
#define mesh_cmd_sig_scene_reg_status           (0)
#define mesh_cmd_sig_scene_status               (0)
#endif

//-------------
int access_cmd_scene_store(u16 adr_dst, u32 rsp_max, u16 id, int ack);
int access_cmd_scene_recall(u16 adr_dst, u32 rsp_max, u16 id, int ack, transition_par_t *trs_par);
int access_cmd_scene_get(u16 adr, u32 rsp_max);
int access_cmd_scene_reg_get(u16 adr, u32 rsp_max);
int access_cmd_scene_del(u16 adr_dst, u32 rsp_max, u16 id, int ack);


