/********************************************************************************************************
 * @file     app.c 
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
#include "../../proj/tl_common.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../proj_lib/ble/ll/ll_whitelist.h"
#include "../../proj_lib/ble/trace.h"
#include "../../proj/mcu/pwm.h"
#include "../../proj/mcu/watchdog_i.h"
#include "../../proj_lib/ble/service/ble_ll_ota.h"
#include "../../proj/drivers/adc.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../proj_lib/ble/ble_smp.h"
#include "../../proj_lib/mesh_crypto/mesh_crypto.h"
#include "../../proj_lib/mesh_crypto/mesh_md5.h"

#include "../../proj_lib/sig_mesh/app_mesh.h"
#include "../common/app_provison.h"
#include "../common/app_beacon.h"
#include "../common/app_proxy.h"
#include "../common/app_health.h"
#include "../../proj/drivers/keyboard.h"
#include "app.h"
#include "proj_lib/ble/gap.h"
#include "vendor/common/blt_soft_timer.h"

#if (HCI_ACCESS==HCI_USE_UART)
#include "../../proj/drivers/uart.h"
#endif

MYFIFO_INIT(blt_rxfifo, 64, 8);
MYFIFO_INIT(blt_txfifo, 40, 8);

//u8		peer_type;
//u8		peer_mac[12];

//////////////////////////////////////////////////////////////////////////////
//	Initialization: MAC address, Adv Packet, Response Packet
//////////////////////////////////////////////////////////////////////////////

//----------------------- UI ---------------------------------------------
void test_cmd_wakeup_lpn()
{
	static u8 test_onoff;
	#if 0
	u32 len = OFFSETOF(mesh_cmd_g_onoff_set_t,transit_t);	// no delay 
	u16 adr_dst = 0xffff; // 0x0016; //0x61e1; // 0x2fe3; //
	mesh_cmd_g_onoff_set_t cmd = {0};
	cmd.onoff = (test_onoff++) & 1;
	//len += 10;	// test segment;
	mesh_tx_cmd_primary(G_ONOFF_SET_NOACK, (u8 *)&cmd, len, adr_dst, 0);
	#else
	access_cmd_onoff(0xffff, 0, (test_onoff++) & 1, CMD_NO_ACK, 0);
	#endif
}

void friend_ship_establish_ok_cb_lpn()
{
	rf_link_light_event_callback(LGT_CMD_FRIEND_SHIP_OK);
    friend_send_current_subsc_list();
    #if LPN_VENDOR_SENSOR_EN
        mesh_vd_lpn_pub_set();
    #endif    
}

void friend_ship_disconnect_cb_lpn()
{

}

#if (BLT_SOFTWARE_TIMER_ENABLE)
/**
 * @brief   This function is soft timer callback function.
 * @return  <0:delete the timer; ==0:timer use the same interval as prior; >0:timer use the return value as new interval. 
 */
int soft_timer_test0(void)
{
	//gpio 0 toggle to see the effect
	DBG_CHN4_TOGGLE;
	static u32 soft_timer_cnt0 = 0;
	soft_timer_cnt0++;
	return 0;
}
#endif

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
			send_to_hci = mesh_dongle_adv_report2vc(pa->data, MESH_ADV_PAYLOAD);
			#else
			send_to_hci = app_event_handler_adv(pa->data, ADV_FROM_MESH, 1);
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
			proxy_cfg_list_init_upon_connection();
			mesh_service_change_report();
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

		mesh_ble_disconnect_cb();
	}

	if (send_to_hci)
	{
		//blc_hci_send_data (h, p, n);
	}

	return 0;
}

void proc_ui()
{
	static u32 tick, scan_io_interval_us = 40000;
	if (!clock_time_exceed (tick, scan_io_interval_us))
	{
		return;
	}
	tick = clock_time();

	//static u32 A_req_tick;
	static u8 fri_request_send = 1;
	if(fri_request_send/* || clock_time_exceed(A_req_tick, 2000*1000)*/){
		fri_request_send = 0;
		//A_req_tick = clock_time();
		//if((!is_in_mesh_friend_st_lpn()) && (!fri_ship_proc_lpn.status)){
			if(is_provision_success()){
        		mesh_friend_ship_set_st_lpn(FRI_ST_REQUEST);
        	}
        //}
	}

	#if 0
	static u8 st_sw1_last;	
	u8 st_sw1 = !gpio_read(SW1_GPIO);
	
	if(!(st_sw1_last)&&st_sw1){
	    scan_io_interval_us = 100*1000; // fix dithering
	}
	st_sw1_last = st_sw1;
	#endif

	#if 0
	static u8 st_sw2_last;	
	u8 st_sw2 = !gpio_read(SW2_GPIO);
	
	if(!(st_sw2_last)&&st_sw2){ // dispatch just when you press the button 
		//trigger the unprivison data packet 
		static u8 beacon_data_num;
		beacon_data_num =1;
		mesh_provision_para_reset();
		while(beacon_data_num--){
			unprov_beacon_send(MESH_UNPROVISION_BEACON_WITH_URI,0);
		}
		prov_para.initial_pro_roles = MESH_INI_ROLE_NODE;
	    scan_io_interval_us = 100*1000; // fix dithering
	}
	st_sw2_last = st_sw2;
	#endif
}

/////////////////////////////////////////////////////////////////////
// main loop flow
/////////////////////////////////////////////////////////////////////
void main_loop ()
{
	static u32 tick_loop;

	tick_loop ++;
#if (BLT_SOFTWARE_TIMER_ENABLE)
	blt_soft_timer_process(MAINLOOP_ENTRY);
#endif
	////////////////////////////////////// BLE entry /////////////////////////////////
	blt_sdk_main_loop ();


	////////////////////////////////////// UI entry /////////////////////////////////
	//  add spp UI task:
	proc_ui();
	proc_led();
	
	mesh_loop_process();
	
	#if PTS_TEST_EN
	pts_test_case_lpn();
	#endif
	
	mesh_lpn_proc_suspend(); // must at last of main_loop()
	#if LPN_VENDOR_SENSOR_EN
    sensor_proc_loop();
	#endif
}

void user_init()
{
	mesh_global_var_init();
	set_blc_hci_flag_fun(0);// disable the hci part of for the lib .
	lpn_provision_ok = is_net_key_save();
	proc_telink_mesh_to_sig_mesh();		// must at first

	blc_app_loadCustomizedParameters();  //load customized freq_offset cap value and tp value

	usb_id_init();
	usb_log_init ();
	usb_dp_pullup_en (1);  //open USB enum

	////////////////// BLE stack initialization ////////////////////////////////////
	ble_mac_init();    

	//link layer initialization
	//bls_ll_init (tbl_mac);
#if(MCU_CORE_TYPE == MCU_CORE_8269)
	blc_ll_initBasicMCU(tbl_mac);   //mandatory
#elif((MCU_CORE_TYPE == MCU_CORE_8258) || (MCU_CORE_TYPE == MCU_CORE_8278))
	blc_ll_initBasicMCU();                      //mandatory
	blc_ll_initStandby_module(tbl_mac);				//mandatory
#endif
	blc_ll_initAdvertising_module(tbl_mac); 	//adv module: 		 mandatory for BLE slave,
	blc_ll_initSlaveRole_module();				//slave module: 	 mandatory for BLE slave,
#if BLT_SOFTWARE_TIMER_ENABLE
	blc_ll_initPowerManagement_module();        //pm module:      	 optional
#endif

	//l2cap initialization
	//blc_l2cap_register_handler (blc_l2cap_packet_receive);
	blc_l2cap_register_handler (app_l2cap_packet_receive);
	///////////////////// USER application initialization ///////////////////

	mesh_scan_rsp_init();


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

	rf_set_power_level_index (MY_RF_POWER_INDEX);
	bls_pm_setSuspendMask (SUSPEND_DISABLE);//(SUSPEND_ADV | SUSPEND_CONN)
    blc_hci_le_setEventMask_cmd(HCI_LE_EVT_MASK_ADVERTISING_REPORT|HCI_LE_EVT_MASK_CONNECTION_COMPLETE);

	////////////////// SPP initialization ///////////////////////////////////
#if (HCI_ACCESS != HCI_NONE)
	#if (HCI_ACCESS==HCI_USE_USB)
	//blt_set_bluetooth_version (BLUETOOTH_VER_4_2);
	//bls_ll_setAdvChannelMap (BLT_ENABLE_ADV_ALL);
	usb_bulk_drv_init (0);
	blc_register_hci_handler (app_hci_cmd_from_usb, blc_hci_tx_to_usb);
	#else	//uart
	uart_drv_init();
	blc_register_hci_handler (blc_rx_from_uart, blc_hci_tx_to_uart);		//default handler
	//blc_register_hci_handler(rx_from_uart_cb,tx_to_uart_cb);				//customized uart handler
	#endif
#endif
	rf_pa_init();
	bls_app_registerEventCallback (BLT_EV_FLAG_CONNECT, (blt_event_callback_t)&mesh_ble_connect_cb);
	blc_hci_registerControllerEventHandler(app_event_handler);		//register event callback
	//bls_hci_mod_setEventMask_cmd(0xffff);			//enable all 15 events,event list see ble_ll.h
	bls_set_advertise_prepare (app_advertise_prepare_handler);
	//bls_set_update_chn_cb(chn_conn_update_dispatch);
	bls_ota_registerStartCmdCb(entry_ota_mode);
	bls_ota_registerResultIndicateCb(show_ota_result);

	app_enable_scan_all_device ();

	// mesh_mode and layer init
	mesh_init_all();
	// OTA init
	bls_ota_clearNewFwDataArea(); //must
	lpn_node_io_init();	// should be after mesh init all
	//blc_ll_initScanning_module(tbl_mac);
	#if((MCU_CORE_TYPE == MCU_CORE_8258) || (MCU_CORE_TYPE == MCU_CORE_8278))
	blc_gap_peripheral_init();    //gap initialization
	#endif
	my_att_init (provision_mag.gatt_mode);
	blc_att_setServerDataPendingTime_upon_ClientCmd(10);
	extern u32 system_time_tick;
	system_time_tick = clock_time();
#if (BLT_SOFTWARE_TIMER_ENABLE)
	blt_soft_timer_init();
	//blt_soft_timer_add(&soft_timer_test0, 1*1000*1000);
#endif
}

#if (PM_DEEPSLEEP_RETENTION_ENABLE)
_attribute_ram_code_ void user_init_deepRetn(void)
{
    blc_app_loadCustomizedParameters();
	blc_ll_initBasicMCU();   //mandatory
	rf_set_power_level_index (MY_RF_POWER_INDEX);

	blc_ll_recoverDeepRetention();

	DBG_CHN0_HIGH;    //debug

	light_pwm_init();
    lpn_node_io_init();

    // should enable IRQ here, because it may use irq here, for example BLE connect.
    irq_enable();
    
    if(HANDLE_RETENTION_DEEP_PRE == lpn_deep_handle.type){
        if(LPN_SUSPEND_EVENT_NEXT_POLL_INV == lpn_deep_handle.event){
            suspend_handle_next_poll_interval(HANDLE_RETENTION_DEEP_AFTER);
        }else if(LPN_SUSPEND_EVENT_WAKEUP_RX == lpn_deep_handle.event){
            suspend_handle_wakeup_rx(HANDLE_RETENTION_DEEP_AFTER);
        }
    }
    else if(HANDLE_RETENTION_DEEP_ADV_PRE == lpn_deep_handle.type){
        suspend_handle_next_poll_interval(HANDLE_RETENTION_DEEP_ADV_PRE);
        // while(1){}      // have been reboot if it's not BLE connected.
    }

    memset(&lpn_deep_handle, 0, sizeof(lpn_deep_handle)); // init
    lpn_wakeup_tick = clock_time();
}
#endif

