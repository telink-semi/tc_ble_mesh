/********************************************************************************************************
 * @file	smart_provision.h
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
#include "app_provison.h"

#define SMART_SCAN_TIMEOUT_MS	(PROVISION_NORMAL_TIMEOUT_MS+1000) // should larger than PROVISION_NORMAL_TIMEOUT_MS

// mesh key info sample data use in smart_gateway_provision_data_set() and smart_provision_appkey_add(), user can self define rules.
#define GATEWAY_UNICAST_ADR 	((u8)ele_adr_primary<<7) // avoid multi gateway unicast address conflict
#define SMART_IV_INDEX			{0x00,0x00,0x00,0x00}
#define SMART_NETKEY_INDEX		0
#define SMART_APPKEY_INDEX		0
#define SMART_NETKEY			{'s','m','a','r','t',',','n','e','t','k','e','y',tbl_mac[3],tbl_mac[2],tbl_mac[1],tbl_mac[0]}
#define SMART_APPKEY			{'s','m','a','r','t',',','a','p','p','k','e','y',0,0,0,0}

#define GATEWAY_NET_KEY_HEAD			{(u8)HCI_CMD_GATEWAY_CTL, HCI_CMD_GATEWAY_CTL>>8, HCI_GATEWAY_CMD_SET_PRO_PARA}
#define GATEWAY_DEV_KEY_HEAD			{(u8)HCI_CMD_GATEWAY_CTL, HCI_CMD_GATEWAY_CTL>>8, HCI_GATEWAY_CMD_SET_DEV_KEY}
#define GATEWAY_SCAN_START  			{(u8)HCI_CMD_GATEWAY_CTL, HCI_CMD_GATEWAY_CTL>>8, HCI_GATEWAY_CMD_START, 0}
#define GATEWAY_SCAN_STOP				{(u8)HCI_CMD_GATEWAY_CTL, HCI_CMD_GATEWAY_CTL>>8, HCI_GATEWAY_CMD_START, 1}
#define GATEWAY_START_PROVISION_HEAD	{(u8)HCI_CMD_GATEWAY_CTL, HCI_CMD_GATEWAY_CTL>>8, HCI_GATEWAY_CMD_SET_NODE_PARA}
#define GATEWAY_APPKEY_ADD_HEAD			{(u8)HCI_CMD_GATEWAY_CTL, HCI_CMD_GATEWAY_CTL>>8, HCI_GATEWAY_CMD_START_KEYBIND, 1} // 1:device default bind mode

enum{
	SMART_CONFIG_IDLE,
	SMART_CONFIG_SCAN_START,
	SMART_CONFIG_SCANING,
	SMART_CONFIG_PROV_START,
	SMART_CONFIG_PROVING,
	SMART_CONFIG_BIND_START,
	SMART_CONFIG_BINDING,
};

enum{
	ST_PROVISION_TIMEOUT,
};

typedef struct{
	u8 mac[6];
	beacon_data_pk beacon;
}unprov_pkt_t;

typedef struct{
	u16 type;
	u8 cmd;
	u8 par[1];
}hci_cmd_t;

int smart_gateway_provision_data_set();
int smart_provision_gateway_devkey_set();
int smart_provision_appkey_add();
void mesh_smart_provision_start();
int is_smart_provision_running();
int mesh_smart_provision_st_get();
void mesh_smart_provision_proc();
int mesh_smart_provision_rsp_handle(u8 code, u8 *p, int len);
int is_existed_in_smart_scan_list(u8 *p_uuid, int is_whitelist);
