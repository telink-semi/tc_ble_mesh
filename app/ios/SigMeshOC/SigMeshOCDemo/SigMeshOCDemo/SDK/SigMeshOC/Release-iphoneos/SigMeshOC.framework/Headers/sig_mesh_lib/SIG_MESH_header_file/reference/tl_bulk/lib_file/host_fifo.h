/********************************************************************************************************
 * @file     host_fifo.h 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/
#ifndef __HOST_FIFO__
#define __HOST_FIFO__
#pragma pack(1)
#include "app_config.h"
#define TIME_BASE 	(40*1000)
#define USB_WRITE_ADV_INTERVAL 	(40*1000)
#define USB_DONGLE_WRITE_INTER	(15*1000)
#if VC_APP_ENABLE 
extern void WriteFile_host_handle(u8 *buff, int n);
#endif

extern int group_status[2][32];

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

#endif 
