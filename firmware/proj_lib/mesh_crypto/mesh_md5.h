/********************************************************************************************************
 * @file	mesh_md5.h
 *
 * @brief	for TLSR chips
 *
 * @author	Mesh Group
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
#ifndef __MESH_MD6_H
#define __MESH_MD6_H

#include "le_crypto.h"
#include "mesh_crypto.h"
#include "../../proj_lib/sig_mesh/app_mesh.h"


typedef int 			S32;
typedef unsigned int 	U32;
typedef unsigned char 	U8;

typedef U32 		  UINT4;
typedef int 	      INT4;

typedef U8		*POINTER;
typedef unsigned long uint32;

/* MD5 context. */
struct MD5Context {
	uint32 buf[4];
	uint32 bits[2];
	unsigned char in[64];
};

typedef struct {
    u32  time_low;
    u16  time_mid;
    u16  time_hi_and_version;
    u8   clock_seq_hi_and_reserved;
    u8   clock_seq_low;
    u8   node[6];
} uuid_mesh_t;
void uuid_create_md5_from_name(uuid_mesh_t *uuid, uuid_mesh_t nsid, void *name,
                               int namelen);
void uuid_create_by_mac(u8 *mac,u8 *uuid);
#endif 
