/********************************************************************************************************
 * @file	remote_prov_gatt.h
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
#ifndef _REMOTE_PROV_GATT_H
#define _REMOTE_PROV_GATT_H

#include "tl_common.h"
#include "mesh_lpn.h"
#include "mesh_fn.h"
#include "time_model.h"
#include "scheduler.h"
#include "mesh_property.h"
void rp_gatt_hci_rcv(u8 *pu,u8 len);
void mesh_prov_server_gatt_send(u8 *par,u8 len);
void rp_gatt_connect(u8 *pmac);
void rp_gatt_disconnect();


#endif

