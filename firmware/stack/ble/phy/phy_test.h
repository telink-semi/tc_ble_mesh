/********************************************************************************************************
 * @file	phy_test.h
 *
 * @brief	for TLSR chips
 *
 * @author	BLE Group
 * @date	Sep. 18, 2015
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
#ifndef PHY_TEST_H_
#define PHY_TEST_H_

#include "tl_common.h"

#ifndef			PHYTEST_MODE_DISABLE
#define			PHYTEST_MODE_DISABLE					0
#endif

#ifndef			PHYTEST_MODE_THROUGH_2_WIRE_UART
#define 		PHYTEST_MODE_THROUGH_2_WIRE_UART		1   //Direct Test Mode through a 2-wire UART interface
#endif

#ifndef			PHYTEST_MODE_OVER_HCI_WITH_USB
#define 		PHYTEST_MODE_OVER_HCI_WITH_USB			2   //Direct Test Mode over HCI(UART hardware interface)
#endif

#ifndef			PHYTEST_MODE_OVER_HCI_WITH_UART
#define 		PHYTEST_MODE_OVER_HCI_WITH_UART			3   //Direct Test Mode over HCI(USB  hardware interface)
#endif


#define 		BLC_PHYTEST_DISABLE						0
#define 		BLC_PHYTEST_ENABLE						1



#define			PHY_CMD_SETUP							0
#define			PHY_CMD_RX								1
#define			PHY_CMD_TX								2
#define			PHY_CMD_END								3


#define 		PKT_TYPE_PRBS9 							0
#define 		PKT_TYPE_0X0F 							1
#define 		PKT_TYPE_0X55 							2
#define 		PKT_TYPE_0XFF 							3

#define			PKT_TYPE_HCI_PRBS9						0
#define			PKT_TYPE_HCI_0X0F						1
#define			PKT_TYPE_HCI_0X55 						2
#define			PKT_TYPE_HCI_PRBS15						3
#define			PKT_TYPE_HCI_0XFF 						4
#define			PKT_TYPE_HCI_0X00						5
#define			PKT_TYPE_HCI_0XF0						6
#define			PKT_TYPE_HCI_0XAA						7
enum{
	PHY_EVENT_STATUS	 = 0,
	PHY_EVENT_PKT_REPORT = 0x8000,
};

enum{
	PHY_STATUS_SUCCESS 	 = 0,
	PHY_STATUS_FAIL 	 = 0x0001,
};





typedef struct {
	u8 cmd;
	u8 tx_start;
	u16 pkts;

	u32 tick_tx;
}phy_data_t;








/******************************* User Interface  ************************************/
void 	  blc_phy_initPhyTest_module(void);

ble_sts_t blc_phy_setPhyTestEnable (u8 en);
bool 	  blc_phy_isPhyTestEnable(void);


//user for phy test 2 wire uart mode
int 	 phy_test_2_wire_rx_from_uart (void);
int 	 phy_test_2_wire_tx_to_uart (void);



/************************* Stack Interface, user can not use!!! ***************************/

int 	  blc_phy_test_main_loop(void);

int 	  blc_phytest_cmd_handler (u8 *p, int n);

ble_sts_t blc_phy_setReceiverTest (u8 rx_chn);
ble_sts_t blc_phy_setTransmitterTest (u8 tx_chn, u8 length, u8 pkt_type);
ble_sts_t blc_phy_setPhyTestEnd(u8 *pkt_num);

ble_sts_t blc_phy_reset(void);
void blc_phy_preamble_length_set(unsigned char len);
void phy_test_driver_init(RF_ModeTypeDef rf_mode);





#endif /* PHY_TEST_H_ */
