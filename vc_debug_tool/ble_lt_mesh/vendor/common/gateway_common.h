/********************************************************************************************************
 * @file	gateway_common.h
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
#ifndef GATEWAY_COMMON_H
#define GATEWAY_COMMON_H

#include "tl_common.h"

int gateway_cmd_from_host_ctl(u8 *p, u16 len );
u8 gateway_upload_mac_address(u8 *p_mac,u8 *p_adv);
void set_gateway_provision_sts(unsigned char en);
unsigned char get_gateway_provisison_sts();
u8 gateway_upload_provision_suc_event(u8 evt,u16 adr,u8 *p_mac,u8 *p_uuid);
void set_gateway_provision_para_init();
u8 gateway_upload_keybind_event(u8 evt);
u8 gateway_upload_provision_self_sts(u8 sts);
u8 gateway_upload_node_ele_cnt(u8 ele_cnt);
u8 gateway_upload_node_info(u16 unicast);
#if BLE_REMOTE_PM_ENABLE
void gateway_iv_update_time_refresh();
void gateway_check_and_enter_sleep();
void gateway_proc_rc_ui_suspend();
void gateway_trigger_iv_search_mode(int force);
#endif

/**
 * @brief       This function service to report firmware download status
 * @param[in]   result	- ota result
 * @return      0 means report success. other means report failure
 * @note        
 */
int gateway_upload_gatt_ota_sts(u8 result);

u8 gateway_upload_mesh_ota_sts(u8 *p_dat,int len);
u8 gateway_upload_mesh_sno_val();
u8 gateway_upload_cmd_rsp(u8 *p_rsp,u8 len);
u8 gateway_cmd_from_host_ota(u8 *p, u16 len );
u8 gateway_cmd_from_host_mesh_ota(u8 *p, u16 len );
u8 gateway_upload_mesh_src_cmd(u16 op,u16 src,u8 *p_ac_par);
u8 gateway_upload_prov_cmd(u8 *p_cmd,u8 len);
u8 gateway_upload_prov_rsp_cmd(u8 *p_rsp,u8 len);
u8 gateway_upload_prov_cmd(u8 *p_cmd,u8 cmd);
u8 gateway_upload_prov_rsp_cmd(u8 *p_rsp,u8 cmd);
u8 gateway_upload_prov_link_cls(u8 *p_rsp,u8 len);
u8 gateway_upload_prov_link_open(u8 *p_cmd,u8 len);
u8 gateway_upload_log_info(u8 *p_buf,u8 len ,char *format,...); //gateway upload the print info to the vc
int fast_provision_upload_node_info(u16 unicast, u16 pid);

int mesh_tx_comm_cmd(u16 adr);
void mesh_ota_comm_test();
extern u32 comm_send_flag ;
extern u32 comm_send_tick ;

#endif
