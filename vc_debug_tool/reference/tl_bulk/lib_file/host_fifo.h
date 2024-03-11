/********************************************************************************************************
 * @file	host_fifo.h
 *
 * @brief	for TLSR chips
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
#ifndef __HOST_FIFO__
#define __HOST_FIFO__
#pragma pack(1)
#include "app_config.h"
#define TIME_BASE 	(40*1000)
#define USB_WRITE_ADV_INTERVAL 	(40*1000)
#define USB_DONGLE_WRITE_INTER	(15*1000)

#define VC_HOST_FIFO_EN			1	// 1: can't get tx error number directly.

#if VC_APP_ENABLE 
extern void WriteFile_host_handle(u8 *buff, int n);
#endif

#define VC_UI_GROUP_CNT_MAX					(64)
extern int group_status[2][VC_UI_GROUP_CNT_MAX];

int fifo_push_vc_cmd2dongle_usb(u8*p_tc,u8 len );
void write_cmd_fifo_poll();
void write_dongle_cmd_fifo_poll();
void hci_rx_fifo_poll();
void hci_tx_fifo_poll();
void Thread_main_process();
int SendOpParaDebug_VC(u16 adr_dst, u8 rsp_max, u16 op, u8 *par, int len);
void push_notify_into_fifo(u8 *p ,u32 len );
void reset_host_fifo();
int IsSendOpBusy(int reliable, u16 adr_dst);


#include "../../../ble_lt_mesh/vendor/common/cmd_interface.h"

int VC_cb_tx_cmd_err(material_tx_cmd_t *p_mat, int err);

#endif 
