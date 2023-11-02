/********************************************************************************************************
 * @file	vc_mesh_func.h
 *
 * @brief	This is the header file for BLE SDK
 *
 * @author	Mesh Group
 * @date	2017
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
#include "SerialPort.h"
#include "./lib_file/app_config.h"
#include "../../ble_lt_mesh/vendor/common/lighting_model.h"
#include "../../ble_lt_mesh/vendor/common/lighting_model_HSL.h"
#include "../../ble_lt_mesh/vendor/common/lighting_model_xyL.h"
#include "../../ble_lt_mesh/vendor/common/lighting_model_LC.h"
#include "../../ble_lt_mesh/vendor/common/scene.h"
#include "../../ble_lt_mesh/vendor/common/mesh_ota.h"
#include "../../ble_lt_mesh/vendor/common/remote_prov.h"


class vc_mesh_func
{
public:
	vc_mesh_func(void);
	~vc_mesh_func(void);
};
#define MAXB2TSIZE  1024*64
u8 win32_create_rand_buf(u8 *p_buf,u8 len );
int Hex2Text (LPBYTE lpD, LPBYTE lpS, int n);
int Bin2Text (LPBYTE lpD, LPBYTE lpS, int n);
int bin2text_normal(LPBYTE lpD, LPBYTE lpS, int n);
int bin2text_clean(LPBYTE lpD, LPBYTE lpS, int n);
int bin2text_clean_uuid(LPBYTE lpD, LPBYTE lpS, int n);

int Text2Bin(LPBYTE lpD, LPBYTE lpS);
int Time2Text (LPBYTE lpD, unsigned int d);
void gatt_provision_net_info_callback();
u32 new_fw_read(u8 *data_out, u32 max_len);
void new_fw_write_file(u8 *data_in, u32 len);
int GetCPUid(BYTE *an);
int vc_get_guid(u8 *p_uuid);
void vc_get_dev_key(u8 *p_devkey);
void vc_get_gateway_dev_key(u8 *p_dev_key);
void test_utc_to_tai();


