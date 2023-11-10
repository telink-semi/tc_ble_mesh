/********************************************************************************************************
 * @file	nlc_sensor.c
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
#include "nlc_sensor.h"

/**
 *	include: ALSMP(Ambient Light Sensor Mesh Profile),ENMMP(Energy Monitor Mesh Profile),
 *			 OCSMP(Occupancy Sensor Mesh Profile).
 */

#if (LIGHT_TYPE_SEL == LIGHT_TYPE_NLC_SENSOR)

const u8 NLC_SENSOR_LOCAL_NAME[NLC_LOCAL_NAME_LEN] = {NLC_LOCAL_NAME};

void mesh_scan_rsp_add_local_name(u8 *p_name, u32 len_max)
{
	memcpy(p_name, NLC_SENSOR_LOCAL_NAME, min(len_max, NLC_LOCAL_NAME_LEN - 1));
}

#endif

