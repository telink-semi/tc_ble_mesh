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
#include "proj_lib/ble/ll/ll.h"
#include "proj_lib/ble/blt_config.h"
#include "proj_lib/ble/ll/ll_whitelist.h"
#include "proj_lib/ble/trace.h"
#include "proj/mcu/pwm.h"
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
#include "../common/mesh_ota.h"
#include "proj/drivers/keyboard.h"
#include "app.h"
#include "app_ui.h"
#include "vendor/common/blt_soft_timer.h"
#include "proj/drivers/rf_pa.h"
#include "../common/remote_prov.h"
#include "../common/security_network_beacon.h"
#include "../common/mesh_access_layer.h"
#if SMART_PROVISION_ENABLE
#include "vendor/common/smart_provision.h"
#endif
#if (MESH_CDTP_ENABLE)
#include "mesh_cdtp.h"
#endif

#if (HCI_ACCESS==HCI_USE_UART)
#include "proj/drivers/uart.h"
#endif

#if(MCU_CORE_TYPE == MCU_CORE_8269) // 8269 ram limited
MYFIFO_INIT(blt_rxfifo, 64, 8);
MYFIFO_INIT(blt_txfifo, 40, 4);
#else
#define BLT_RX_FIFO_SIZE        (MESH_DLE_MODE ? DLE_RX_FIFO_SIZE : 64)
#define BLT_TX_FIFO_SIZE        (MESH_DLE_MODE ? DLE_TX_FIFO_SIZE : 40)

MYFIFO_INIT(blt_rxfifo, BLT_RX_FIFO_SIZE, 16);
MYFIFO_INIT(blt_txfifo, BLT_TX_FIFO_SIZE, 16); // 16 is enough for gateway. no need too much, especially when extend adv is enable.
#endif

//////////////////////////////////////////////////////////////////////////////
//	Initialization: MAC address, Adv Packet, Response Packet
//////////////////////////////////////////////////////////////////////////////

//----------------------- UI ---------------------------------------------
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

			#if 0 // TESTCASE_FLAG_ENABLE
			u8 mac_pts[] = {0xDA,0xE2,0x08,0xDC,0x1B,0x00};	// 0x001BDC08E2DA
			if(memcmp(pa->mac, mac_pts,6)){
				return 0;
			}
			#endif
			
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
			
			#if (MESH_CDTP_ENABLE)
			app_coc_ble_connect_cb(p);
			#endif
			
			proxy_cfg_list_init_upon_connection(BLS_HANDLE_MIN);
			mesh_service_change_report(BLS_HANDLE_MIN);
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
		
		#if (MESH_CDTP_ENABLE)
		app_coc_ble_disconnect_cb(p);
		#endif

		mesh_ble_disconnect_cb(p);
	}

	if (send_to_hci)
	{
		//blc_hci_send_data (h, p, n);
	}

	return 0;
}

/**
 * @brief      BLE host event handler call-back.
 * @param[in]  h       event type
 * @param[in]  para    Pointer point to event parameter buffer.
 * @param[in]  n       the length of event parameter.
 * @return
 */
int app_host_event_callback (u32 h, u8 *para, int n)
{
#if (MESH_CDTP_ENABLE)
	app_host_coc_event_callback(h, para, n);
#endif

	u8 event = h & 0xFF;

	switch(event)
	{
#if BLE_REMOTE_SECURITY_ENABLE
		case GAP_EVT_SMP_TK_DISPLAY:
		{
			//char pc[7];
			gap_smp_TkDisplayEvt_single_connect_t *p = (gap_smp_TkDisplayEvt_single_connect_t *)para;
			#if 1
			LOG_MSG_LIB(TL_LOG_NODE_SDK, 0, 0, "SMP TK display int: %06d", p->tk_pincode);
			#else
			u8 int_buf[10];
			u8 len = u32to_int_buf(p->tk_pincode, int_buf, sizeof(int_buf));
			my_dump_str_data(APP_LOG_EN, "SMP TK display int:", int_buf, len);
			my_dump_str_data(APP_LOG_EN, "SMP TK display u32:", &p->tk_pincode, 4);
			#endif
			#if (MESH_CDTP_ENABLE)
			gateway_ots_rx_rsp_cmd((u8 *)&p->tk_pincode, sizeof(p->tk_pincode), OTS_OACP_OPCODE_USER_SMP_TK_DISPLAY);
			#endif
			//my_uart_send_str_int("int is:", pinCode);
			//sprintf(pc, "%d", pinCode);
			//printf("TK display:%s\n", pc);
		}
		break;

		case GAP_EVT_SMP_TK_REQUEST_PASSKEY:
		{

		}
		break;

		case GAP_EVT_SMP_TK_REQUEST_OOB:
		{

		}
		break;

		case GAP_EVT_SMP_TK_NUMERIC_COMPARE:
		{

		}
		break;
#endif
#if (MCU_CORE_TYPE != MCU_CORE_8269)
		case GAP_EVT_ATT_EXCHANGE_MTU:
		{

		}
		break;
#endif
		default:
		break;
	}

	return 0;
}

/**
 * @brief       This function set up wakeup source: driver pin of keyboard
 * @return      none
 * @note        
 */
void gateway_ui_init()
{
    ////////// set up wakeup source: driver pin of keyboard  //////////
#if (UI_KEYBOARD_ENABLE)
	ui_keyboard_wakeup_io_init();
#endif
}

#if BLE_REMOTE_PM_ENABLE
void  gateway_set_sleep_wakeup (u8 e, u8 *p, int n)
{
	bls_pm_setWakeupSource(PM_WAKEUP_PAD);
	blc_ll_setScanEnable (0, 0); // not scan after suspend wakeup
}

/**
 * @brief       This function receive beacon timeout by soft timer
 * @return      -1 means delete this soft timer.
 * @note        
 */
int soft_timer_rcv_beacon_timeout()
{
#if SMART_PROVISION_ENABLE
	if(!is_smart_provision_running())
#endif
	{
		ENABLE_SUSPEND_MASK;
		blc_ll_setScanEnable (0, 0);
	}
	return -1;
}

/**
 * @brief       This function check gateway whether to enter sleep
 * @return      none
 * @note        
 */
void gateway_check_and_enter_sleep()
{
#if BLE_REMOTE_PM_ENABLE
	if(blc_tlkEvent_pending || del_node_tick || is_busy_tx_segment_or_reliable_flow() || blt_soft_timer_cur_num()){
		// wait proc_node_reset 
	}
	else if(0 == my_fifo_data_cnt_get(&mesh_adv_cmd_fifo)){
		if(key_released){
			cfg_led_event_stop();
			led_onoff_gpio(GPIO_LED, 0);
			u16 wakeup_src = is_provision_success() ? (PM_WAKEUP_PAD|PM_WAKEUP_TIMER) : PM_WAKEUP_PAD;
			
			u32 sleep_s = clock_time_s()-gateway_iv_updata_s; // switch_iv_updata_s is last update time
			if(sleep_s < GATEWAY_LONG_SLEEP_TIME_S){
				sleep_s = GATEWAY_LONG_SLEEP_TIME_S - sleep_s;
			}
			else{
				sleep_s = 1;
			}
			// user can set wakeup_tick of 32k rc by self. 
			cpu_long_sleep_wakeup(DEEPSLEEP_MODE_RET_SRAM_LOW32K, wakeup_src, sleep_s*1000*32);
		}
	}
#endif
}

/**
 * @brief       This function enable suspend in a connected state or ota
 * @return      none
 * @note        
 */
void gateway_proc_rc_ui_suspend()
{
	if((blc_ll_getCurrentState() == BLS_LINK_STATE_CONN) || is_tlk_gatt_ota_busy()){
		ENABLE_SUSPEND_MASK;
		return ;
	}
	
#if SMART_PROVISION_ENABLE
	if(!is_smart_provision_running())
#endif
	{
		gateway_check_and_enter_sleep();
	}
}
#endif

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
	mesh_loop_proc_prior();
	////////////////////////////////////// BLE entry /////////////////////////////////
	blt_sdk_main_loop ();


	////////////////////////////////////// UI entry /////////////////////////////////
	//  add spp UI task:
#if (BATT_CHECK_ENABLE)
    app_battery_power_check_and_sleep_handle(1);
#endif
	proc_ui();
	proc_led();
	factory_reset_cnt_check();
	
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

#if (TESTCASE_FLAG_ENABLE && (!__PROJECT_MESH_PRO__))
	test_case_key_refresh_patch();
#endif

#if MESH_CDTP_ENABLE
	mesh_cdtp_loop();
#endif

#if GW_SMART_PROVISION_REMOTE_CONTROL_PM_EN
	gateway_proc_rc_ui_suspend();
#endif
}

void user_init()
{
    #if (BATT_CHECK_ENABLE)
    app_battery_power_check_and_sleep_handle(0); //battery check must do before OTA relative operation
    #endif
	enable_mesh_provision_buf();
	mesh_global_var_init();

#if (APP_FLASH_PROTECTION_ENABLE)
	app_flash_protection_operation(FLASH_OP_EVT_APP_INITIALIZATION, 0, 0);
	blc_appRegisterStackFlashOperationCallback(app_flash_protection_operation); //register flash operation callback for stack
#endif
	set_blc_hci_flag_fun(0);// disable the hci part of for the lib .
	proc_telink_mesh_to_sig_mesh();		// must at first
	blc_app_loadCustomizedParameters();  //load customized freq_offset cap value and tp value
	
#if (HCI_ACCESS==HCI_USE_USB)
	usb_id_init();
	usb_log_init ();
	// need to have a simulate insert
	usb_dp_pullup_en (0);  //open USB enum
	gpio_set_func(GPIO_DP,AS_GPIO);
	gpio_set_output_en(GPIO_DP,1);
	gpio_write(GPIO_DP,0);
	sleep_us(20000);
	gpio_set_func(GPIO_DP,AS_USB);
	usb_dp_pullup_en (1);  //open USB enum
#endif
	////////////////// BLE stack initialization ////////////////////////////////////
	ble_mac_init();    

	//link layer initialization
	//bls_ll_init (tbl_mac);

	//Smp Initialization may involve flash write/erase(when one sector stores too much information,
	//	 is about to exceed the sector threshold, this sector must be erased, and all useful information
	//	 should re_stored) , so it must be done after battery check
#if (BLE_REMOTE_SECURITY_ENABLE)
	bls_smp_configParingSecurityInfoStorageAddr(FLASH_ADR_SMP_PARA_START); // must before blc_smp_peripheral_init().

	#if MESH_CDTP_ENABLE
		#if (CDTP_SMP_LEVEL == 3)	// CDTP spec require SMP >= level 3
	blc_smp_param_setBondingDeviceMaxNumber(4);    //if not set, default is : SMP_BONDING_DEVICE_MAX_NUM

	//set security level: "LE_Security_Mode_1_Level_3"
	blc_smp_setSecurityLevel(Authenticated_Paring_with_Encryption);  //if not set, default is : LE_Security_Mode_1_Level_2(Unauthenticated_Pairing_with_Encryption)
	blc_smp_enableAuthMITM(1);
	blc_smp_setBondingMode(Bondable_Mode);	// if not set, default is : Bondable_Mode
	blc_smp_setIoCapability(IO_CAPABILITY_DISPLAY_ONLY);	// if not set, default is : IO_CAPABILITY_NO_INPUT_NO_OUTPUT

	//Smp Initialization may involve flash write/erase(when one sector stores too much information,
	//   is about to exceed the sector threshold, this sector must be erased, and all useful information
	//   should re_stored) , so it must be done after battery check
	//Notice:if user set smp parameters: it should be called after usr smp settings
		#else
#error todo level 4
		#endif

	blc_smp_peripheral_init(); // must before blc_ll_initSlaveRole_module() and must be after all smp level setting.
	blc_smp_configSecurityRequestSending(SecReq_IMM_SEND, SecReq_PEND_SEND, 1000);	// for APP to show pair menu
	#else
	//Hid device on android7.0/7.1 or later version
	// New paring: send security_request immediately after connection complete
	// reConnect:  send security_request 1000mS after connection complete. If master start paring or encryption before 1000mS timeout, slave do not send security_request.
	blc_smp_configSecurityRequestSending(SecReq_IMM_SEND, SecReq_PEND_SEND, 1000); //if not set, default is:  send "security request" immediately after link layer connection established(regardless of new connection or reconnection )
	#endif
	
	blc_gap_registerHostEventHandler(app_host_event_callback);
#endif

#if (MCU_CORE_TYPE != MCU_CORE_8269)
	blc_gap_setEventMask(
						#if MESH_CDTP_ENABLE
						  GAP_EVT_MASK_L2CAP_COC_CONNECT			|  \
						  GAP_EVT_MASK_L2CAP_COC_DISCONNECT			|  \
						  GAP_EVT_MASK_L2CAP_COC_RECONFIGURE		|  \
						  GAP_EVT_MASK_L2CAP_COC_RECV_DATA			|  \
						  GAP_EVT_MASK_L2CAP_COC_SEND_DATA_FINISH	|  \
						  GAP_EVT_MASK_L2CAP_COC_CREATE_CONNECT_FINISH  |  \
						  GAP_EVT_MASK_SMP_TK_DISPLAY
						#else
						0
						#endif
	);
#endif

#if(MCU_CORE_TYPE == MCU_CORE_8269)
	blc_ll_initBasicMCU(tbl_mac);   //mandatory
#elif((MCU_CORE_TYPE == MCU_CORE_8258) || (MCU_CORE_TYPE == MCU_CORE_8278))
	blc_ll_initBasicMCU();                      //mandatory
	blc_ll_initStandby_module(tbl_mac);				//mandatory
#endif

#if (EXTENDED_ADV_ENABLE)
	mesh_blc_ll_initExtendedAdv();
#endif
	blc_ll_initAdvertising_module(tbl_mac); 	//adv module: 		 mandatory for BLE slave,
	blc_ll_initSlaveRole_module();				//slave module: 	 mandatory for BLE slave,

#if (BLE_REMOTE_PM_ENABLE)
	blc_ll_initPowerManagement_module();		//pm module:		 optional
	bls_pm_setSuspendMask (SUSPEND_ADV | DEEPSLEEP_RETENTION_ADV | SUSPEND_CONN | DEEPSLEEP_RETENTION_CONN);
	blc_pm_setDeepsleepRetentionThreshold(50, 30);
	blc_pm_setDeepsleepRetentionEarlyWakeupTiming(400);
	bls_pm_registerFuncBeforeSuspend(app_func_before_suspend);
	#if SMART_PROVISION_ENABLE
	bls_app_registerEventCallback (BLT_EV_FLAG_SUSPEND_ENTER, &gateway_set_sleep_wakeup);
	#endif
#else
	bls_pm_setSuspendMask (SUSPEND_DISABLE);//(SUSPEND_ADV | SUSPEND_CONN)
#endif

	//l2cap initialization
	//blc_l2cap_register_handler (blc_l2cap_packet_receive);
	blc_l2cap_register_handler (app_l2cap_packet_receive);
	///////////////////// USER application initialization ///////////////////

#if EXTENDED_ADV_ENABLE
	/*u8 status = */mesh_blc_ll_setExtAdvParamAndEnable();
#endif
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
#if BLE_REMOTE_PM_ENABLE
	gateway_trigger_iv_search_mode(1);
#endif
	
	app_enable_scan_all_device ();

	// mesh_mode and layer init
	mesh_init_all();
	// OTA init
	bls_ota_clearNewFwDataArea(0); //must

	//blc_ll_initScanning_module(tbl_mac);
	#if((MCU_CORE_TYPE == MCU_CORE_8258) || (MCU_CORE_TYPE == MCU_CORE_8278))
	blc_gap_peripheral_init();    //gap initialization
	#endif
	
	mesh_scan_rsp_init();
	my_att_init (provision_mag.gatt_mode);
	blc_att_setServerDataPendingTime_upon_ClientCmd(10);
	system_time_init();
	
	gateway_ui_init();
#if (BLT_SOFTWARE_TIMER_ENABLE)
	blt_soft_timer_init();
	//blt_soft_timer_add(&soft_timer_test0, 1*1000*1000);
#endif

#if MESH_CDTP_ENABLE
	mesh_cdtp_init();
	// blc_att_enableWriteReqReject(1); // TODO: response error code for OTS
#endif

#if MESH_RX_TEST	
	mesh_register_upper_transport_layer_callback(mesh_upper_transport_layer_cb);
#endif
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

	 gateway_ui_init();
}
#endif
