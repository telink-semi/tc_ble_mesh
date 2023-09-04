/********************************************************************************************************
 * @file	mesh_property.h
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
#define G_MFG_PROPERTIES_GET		    0x2A82
#define G_MFG_PROPERTIES_STATUS		    0x43
#define G_MFG_PROPERTY_GET		        0x2B82
#define G_MFG_PROPERTY_SET		        0x44
#define G_MFG_PROPERTY_SET_NOACK		0x45
#define G_MFG_PROPERTY_STATUS		    0x46

#define G_ADMIN_PROPERTIES_GET		    0x2C82
#define G_ADMIN_PROPERTIES_STATUS		0x47
#define G_ADMIN_PROPERTY_GET		    0x2D82
#define G_ADMIN_PROPERTY_SET		    0x48
#define G_ADMIN_PROPERTY_SET_NOACK		0x49
#define G_ADMIN_PROPERTY_STATUS		    0x4A

#define G_USER_PROPERTIES_GET		    0x2E82
#define G_USER_PROPERTIES_STATUS		0x4B
#define G_USER_PROPERTY_GET		        0x2F82
#define G_USER_PROPERTY_SET		        0x4C
#define G_USER_PROPERTY_SET_NOACK		0x4D
#define G_USER_PROPERTY_STATUS		    0x4E

#define G_CLIENT_PROPERTIES_GET		    0x4F
#define G_CLIENT_PROPERTIES_STATUS		0x50

//------op parameters

//------------------vendor op end-------------------

#define INVALID_PROP_ID         (0)

enum{
    MESH_ACCESS_NOT_USER        = 0,
    MESH_ACCESS_READ            = 1,
    MESH_ACCESS_WRITE           = 2,
    MESH_ACCESS_RW              = 3,
    MESH_ACCESS_MAX,
};

enum{
    MESH_USER_ACCESS_READ       = 1,
    MESH_USER_ACCESS_WRITE      = 2,
    MESH_USER_ACCESS_RW         = 3,
};

enum{
    MESH_ADMIN_ACCESS_NOT_USER  = 0,
    MESH_ADMIN_ACCESS_READ      = 1,
    MESH_ADMIN_ACCESS_WRITE     = 2,
    MESH_ADMIN_ACCESS_RW        = 3,
};

enum{
    MESH_MFG_ACCESS_NOT_USER    = 0,
    MESH_MFG_ACCESS_READ        = 1,
};

typedef struct{
	u16 id;
	u8 data[MESH_PROPERTY_STR_SIZE_MAX];
}mesh_property_set_user_t;

typedef struct{
	u16 id;
	u8 access;
	u8 data[MESH_PROPERTY_STR_SIZE_MAX];
}mesh_property_set_admin_t;

typedef struct{
	u16 id;
	u8 access;
}mesh_property_set_mfg_t;

//---
extern _align_4_ model_property_t	model_sig_property;
extern u32 mesh_md_property_addr;

//---
void mesh_property_global_init();

//--- rx
int mesh_property_st_publish_user(u8 idx);
int mesh_property_st_publish_admin(u8 idx);
int mesh_property_st_publish_mfg(u8 idx);

#if MD_SERVER_EN
int mesh_cmd_sig_properties_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_property_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_property_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
#else
#define mesh_cmd_sig_properties_get         (0)
#define mesh_cmd_sig_property_get           (0)
#define mesh_cmd_sig_property_set           (0)
#endif

#if MD_CLIENT_EN
int mesh_cmd_sig_properties_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_property_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
#else
#define mesh_cmd_sig_properties_status      (0)
#define mesh_cmd_sig_property_status        (0)
#endif

//--- tx

