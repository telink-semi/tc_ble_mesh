/********************************************************************************************************
 * @file	app.c
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
#include "tl_common.h"
#include "proj_lib/rf_drv.h"
#include "proj_lib/pm.h"
#include "proj_lib/ble/service/ble_ll_ota.h"
#include "proj/drivers/adc.h"
#if(MCU_CORE_TYPE == MCU_CORE_8258)
#include "stack/ble/ble.h"
#elif(MCU_CORE_TYPE == MCU_CORE_8278)
#include "stack/ble_8278/ble.h"
#endif
#include "proj_lib/mesh_crypto/mesh_crypto.h"
#include "proj_lib/mesh_crypto/mesh_md5.h"

#include "proj_lib/sig_mesh/app_mesh.h"
#include "../common/app_provison.h"
#include "../common/app_beacon.h"
#include "../common/app_proxy.h"
#include "../common/app_health.h"
#include "app_ui.h"
#include "app.h"
#include "vendor/common/blt_soft_timer.h"
#include "vendor/common/nlc/mesh_nlc.h"
#include "proj/drivers/rf_pa.h"

#if (HCI_ACCESS==HCI_USE_UART)
#include "proj/drivers/uart.h"
#endif

#if AIS_ENABLE
#define BLT_RX_BUF_NUM	16
#elif SWITCH_ALWAYS_MODE_GATT_EN
#define BLT_RX_BUF_NUM	16	// if 8, it can not receive adv when GATT connect, due to adv filter proc().
#else
#define BLT_RX_BUF_NUM	8
#endif
MYFIFO_INIT(blt_rxfifo, 64, BLT_RX_BUF_NUM);
MYFIFO_INIT(blt_txfifo, 40, 16);

//----------------------- handle BLE event ---------------------------------------------
int app_event_handler (u32 h, u8 *p, int n)
{
	static u32 event_cb_num;
	event_cb_num++;
	int send_to_hci = 1;

	if (h == (HCI_FLAG_EVENT_BT_STD | HCI_EVT_LE_META))		//LE event
	{
		u8 subcode = p[0];

	//------------ ADV packet --------------------------------------------
		if (subcode == HCI_SUB_EVT_LE_ADVERTISING_REPORT)	// ADV packet
		{
			event_adv_report_t *pa = (event_adv_report_t *)p;
			if(LL_TYPE_ADV_NONCONN_IND != (pa->event_type & 0x0F)){
				return 0;
			}
			
			#if DEBUG_MESH_DONGLE_IN_VC_EN
			send_to_hci = (0 == mesh_dongle_adv_report2vc(pa->data, MESH_ADV_PAYLOAD));
			#else
			send_to_hci = (0 == app_event_handler_adv(pa->data, MESH_BEAR_ADV, 1));
			#endif
		}

	//------------ connection complete -------------------------------------
		else if (subcode == HCI_SUB_EVT_LE_CONNECTION_COMPLETE)	// connection complete
		{
			event_connection_complete_t *pc = (event_connection_complete_t *)p;
			if (!pc->status)							// status OK
			{
				//app_led_en (pc->handle, 1);

				//peer_type = pc->peer_adr_type;
				//memcpy (peer_mac, pc->mac, 6);
			}
			#if DEBUG_BLE_EVENT_ENABLE
			rf_link_light_event_callback(LGT_CMD_BLE_CONN);
			#endif

			#if DEBUG_MESH_DONGLE_IN_VC_EN
			debug_mesh_report_BLE_st2usb(1);
			#endif
			proxy_cfg_list_init_upon_connection(BLS_CONN_HANDLE);
			mesh_service_change_report(BLS_CONN_HANDLE);
		}

	//------------ connection update complete -------------------------------
		else if (subcode == HCI_SUB_EVT_LE_CONNECTION_UPDATE_COMPLETE)	// connection update
		{
		}
	}

	//------------ disconnect -------------------------------------
	else if (h == (HCI_FLAG_EVENT_BT_STD | HCI_EVT_DISCONNECTION_COMPLETE))		//disconnect
	{

		event_disconnection_t	*pd = (event_disconnection_t *)p;
		//app_led_en (pd->handle, 0);
		//terminate reason
		if(pd->reason == HCI_ERR_CONN_TIMEOUT){

		}
		else if(pd->reason == HCI_ERR_REMOTE_USER_TERM_CONN){  //0x13

		}
		else if(pd->reason == SLAVE_TERMINATE_CONN_ACKED || pd->reason == SLAVE_TERMINATE_CONN_TIMEOUT){

		}
		#if DEBUG_BLE_EVENT_ENABLE
		rf_link_light_event_callback(LGT_CMD_BLE_ADV);
		#endif 

		#if DEBUG_MESH_DONGLE_IN_VC_EN
		debug_mesh_report_BLE_st2usb(0);
		#endif
		if(switch_provision_ok){
			switch_mode_set(SWITCH_MODE_NORMAL);
		}
		mesh_ble_disconnect_cb(p);
	}

	if (send_to_hci)
	{
		//blc_hci_send_data (h, p, n);
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////
// main loop flow
/////////////////////////////////////////////////////////////////////
void main_loop ()
{
	static u32 tick_loop;
	// LOG_MSG_LIB(TL_LOG_NODE_SDK, 0, 0, "m %d", tick_loop);

	tick_loop ++;
#if (BLT_SOFTWARE_TIMER_ENABLE)
	blt_soft_timer_process(MAINLOOP_ENTRY);
	if(BLS_LINK_STATE_ADV == blc_ll_getCurrentState() || SWITCH_ALWAYS_MODE_GATT_EN){ // if no soft timer add, retransmit interval will be ADV interval.
		if(my_fifo_data_cnt_get(&mesh_adv_cmd_fifo)){
			if(!is_soft_timer_exist(&mesh_switch_send_mesh_adv)){
				blt_soft_timer_add(&mesh_switch_send_mesh_adv,get_mesh_adv_interval());	
			}
		}
	}
#endif
	#if DUAL_MODE_ADAPT_EN
	if(RF_MODE_BLE != dual_mode_proc()){    // should be before is mesh latency window()
        proc_ui();
        proc_led();
        //factory_reset_cnt_check();
		return ;
	}
	#endif
	
	////////////////////////////////////// BLE entry /////////////////////////////////
	blt_sdk_main_loop ();


	////////////////////////////////////// UI entry /////////////////////////////////
	//  add spp UI task:
#if (BATT_CHECK_ENABLE)
    app_battery_power_check_and_sleep_handle(1);
#endif
	proc_ui();
	proc_led();
	
	mesh_loop_process();
#if ADC_ENABLE
	static u32 adc_check_time;
	if(clock_time_exceed(adc_check_time, 1000*1000)){
		adc_check_time = clock_time();
		static u16 T_adc_val;
		
		#if (BATT_CHECK_ENABLE)
		app_battery_check_and_re_init_user_adc();
		#endif
	#if(MCU_CORE_TYPE == MCU_CORE_8269)     
		T_adc_val = adc_BatteryValueGet();
	#else
		T_adc_val = adc_sample_and_get_result();
	#endif
	}  
#endif

	proc_rc_ui_suspend();
}

void user_init()
{
    #if (BATT_CHECK_ENABLE)
    app_battery_power_check_and_sleep_handle(0); //battery check must do before OTA relative operation
    #endif
	deep_wakeup_proc();// fast detect key when powerup
	switch_provision_ok = is_net_key_save();
	mesh_global_var_init();

#if (APP_FLASH_PROTECTION_ENABLE)
	app_flash_protection_operation(FLASH_OP_EVT_APP_INITIALIZATION, 0, 0);
	blc_appRegisterStackFlashOperationCallback(app_flash_protection_operation); //register flash operation callback for stack
#endif
	set_blc_hci_flag_fun(0);// disable the hci part of for the lib .
	proc_telink_mesh_to_sig_mesh();		// must at first
		
	#if (DUAL_MODE_ADAPT_EN)
	dual_mode_en_init();    // must before factory_reset_handle, because "dual_mode_state" is used in it.
	#endif
	blc_app_loadCustomizedParameters();  //load customized freq_offset cap value and tp value

	////////////////// BLE stack initialization ////////////////////////////////////
	ble_mac_init();    

	//link layer initialization
	//bls_ll_init (tbl_mac);

	//Smp Initialization may involve flash write/erase(when one sector stores too much information,
	//	 is about to exceed the sector threshold, this sector must be erased, and all useful information
	//	 should re_stored) , so it must be done after battery check
#if (BLE_REMOTE_SECURITY_ENABLE)
	bls_smp_configParingSecurityInfoStorageAddr(FLASH_ADR_SMP_PARA_START); // must before blc_smp_peripheral_init().
	blc_smp_peripheral_init();

	//Hid device on android7.0/7.1 or later version
	// New paring: send security_request immediately after connection complete
	// reConnect:  send security_request 1000mS after connection complete. If master start paring or encryption before 1000mS timeout, slave do not send security_request.
	blc_smp_configSecurityRequestSending(SecReq_IMM_SEND, SecReq_PEND_SEND, 1000); //if not set, default is:  send "security request" immediately after link layer connection established(regardless of new connection or reconnection )
#endif

#if(MCU_CORE_TYPE == MCU_CORE_8269)
	blc_ll_initBasicMCU(tbl_mac);   //mandatory
#elif((MCU_CORE_TYPE == MCU_CORE_8258) || (MCU_CORE_TYPE == MCU_CORE_8278))
	blc_ll_initBasicMCU();                      //mandatory
	blc_ll_initStandby_module(tbl_mac);				//mandatory
#endif
	blc_ll_initAdvertising_module(tbl_mac); 	//adv module: 		 mandatory for BLE slave,
	blc_ll_initSlaveRole_module();				//slave module: 	 mandatory for BLE slave,
#if BLE_REMOTE_PM_ENABLE
	blc_ll_initPowerManagement_module();		//pm module:		 optional
	ENABLE_SUSPEND_MASK;
	blc_pm_setDeepsleepRetentionThreshold(50, 30); // threshold to enter retention
	blc_pm_setDeepsleepRetentionEarlyWakeupTiming(400); // retention early wakeup time		

	bls_app_registerEventCallback (BLT_EV_FLAG_SUSPEND_ENTER, &mesh_set_sleep_wakeup);
	bls_app_registerEventCallback (BLT_EV_FLAG_GPIO_EARLY_WAKEUP, &mesh_proc_keyboard);
#else
	bls_pm_setSuspendMask (SUSPEND_DISABLE);
#endif

	//l2cap initialization
	//blc_l2cap_register_handler (blc_l2cap_packet_receive);
	blc_l2cap_register_handler (app_l2cap_packet_receive);	
	///////////////////// USER application initialization ///////////////////

	u8 status = bls_ll_setAdvParam( ADV_INTERVAL_MIN, ADV_INTERVAL_MAX, \
			 	 	 	 	 	     ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
			 	 	 	 	 	     0,  NULL,  BLT_ENABLE_ADV_ALL, ADV_FP_NONE);

	if(status != BLE_SUCCESS){  //adv setting err
		write_reg8(0x8000, 0x11);  //debug
		while(1);
	}
	
	// normally use this settings 
	blc_ll_setAdvCustomedChannel (37, 38, 39);
	bls_ll_setAdvEnable(1);  //adv enable

	rf_set_power_level_index (my_rf_power_index);
    blc_hci_le_setEventMask_cmd(HCI_LE_EVT_MASK_ADVERTISING_REPORT|HCI_LE_EVT_MASK_CONNECTION_COMPLETE);

	////////////////// SPP initialization ///////////////////////////////////
#if (HCI_ACCESS != HCI_USE_NONE)
	#if (HCI_ACCESS==HCI_USE_USB)
	//blt_set_bluetooth_version (BLUETOOTH_VER_4_2);
	//bls_ll_setAdvChannelMap (BLT_ENABLE_ADV_ALL);
	usb_bulk_drv_init (0);
	blc_register_hci_handler (app_hci_cmd_from_usb, blc_hci_tx_to_usb);
	#elif (HCI_ACCESS == HCI_USE_UART)	//uart
	uart_drv_init();
	blc_register_hci_handler (blc_rx_from_uart, blc_hci_tx_to_uart);		//default handler
	//blc_register_hci_handler(rx_from_uart_cb,tx_to_uart_cb);				//customized uart handler
	#endif
#endif
#if ADC_ENABLE
	adc_drv_init();	// still init even though BATT_CHECK_ENABLE is enable, because battery check may not be called in user init.
#endif
	rf_pa_init();
	bls_app_registerEventCallback (BLT_EV_FLAG_CONNECT, (blt_event_callback_t)&mesh_ble_connect_cb);
	blc_hci_registerControllerEventHandler(app_event_handler);		//register event callback
	//bls_hci_mod_setEventMask_cmd(0xffff);			//enable all 15 events,event list see ble_ll.h
	bls_set_advertise_prepare (app_advertise_prepare_handler);
	//bls_set_update_chn_cb(chn_conn_update_dispatch);
	bls_ota_registerStartCmdCb(entry_ota_mode);
	bls_ota_registerResultIndicateCb(show_ota_result);

	// mesh_mode and layer init
	mesh_init_all();

	// OTA init
	#if (DUAL_MODE_ADAPT_EN && (0 == FW_START_BY_LEGACY_BOOTLOADER_EN) || DUAL_MODE_WITH_TLK_MESH_EN)
	if(DUAL_MODE_NOT_SUPPORT == dual_mode_state)
	#endif
	{bls_ota_clearNewFwDataArea(0);	 //must
	}
	mesh_switch_init();
#if SWITCH_ALWAYS_MODE_GATT_EN
	switch_mode_set(SWITCH_MODE_GATT);
#endif
	switch_trigger_iv_search_mode(1);
	//blc_ll_initScanning_module(tbl_mac);
	#if((MCU_CORE_TYPE == MCU_CORE_8258) || (MCU_CORE_TYPE == MCU_CORE_8278))
	blc_gap_peripheral_init();    //gap initialization
	#endif
	
	mesh_scan_rsp_init();
	my_att_init (provision_mag.gatt_mode);
	blc_att_setServerDataPendingTime_upon_ClientCmd(10);
#if (BLT_SOFTWARE_TIMER_ENABLE)
	blt_soft_timer_init();
	//blt_soft_timer_add(&soft_timer_test0, 1*1000*1000);
#endif
	/// LOG_MSG_LIB(TL_LOG_NODE_SDK, 0, 0, "init");
}

#if (PM_DEEPSLEEP_RETENTION_ENABLE)
extern u32 blt_advExpectTime;
_attribute_ram_code_ void user_init_deepRetn(void)
{
    blc_app_loadCustomizedParameters();
	blc_ll_initBasicMCU();   //mandatory
	rf_set_power_level_index (my_rf_power_index);

	blc_ll_recoverDeepRetention();	
	if(blc_ll_getCurrentState() == BLS_LINK_STATE_ADV){
		if(!(((blt_advExpectTime-clock_time())<blta.adv_interval) || ((clock_time()-blt_advExpectTime)<blta.adv_interval))){	
			blt_advExpectTime = clock_time();
		}
	}
	// should enable IRQ here, because it may use irq here, for example BLE connect.
    irq_enable();
	if(!iv_idx_st.update_proc_flag){
		iv_idx_st.searching_flag = 1;// always in search mode to process security beacon
	}

	DBG_CHN0_HIGH;    //debug

#if (HCI_ACCESS == HCI_USE_UART)	//uart
	uart_drv_init();
#endif
#if ADC_ENABLE
	adc_drv_init();
#endif

    mesh_switch_init();    
	// LOG_MSG_LIB(TL_LOG_NODE_SDK, 0, 0, "init ret");
}
#endif
