/********************************************************************************************************
 * @file	app_config_tc321x.h
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

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

#include "vendor/common/version.h"    // include mesh_config.h inside.
//////////////////board sel/////////////////////////////////////
#define BOARD_TC321X_EVK_C1T357A20_V1_2      1
#define BOARD_TC321X_EVK_C1T357A20_V2_0      2

#ifndef BOARD_SELECT
#define BOARD_SELECT                        BOARD_TC321X_EVK_C1T357A20_V1_2
#endif


#define _USER_CONFIG_DEFINED_	1	// must define this macro to make others known
#define	__LOG_RT_ENABLE__		0

#define GATEWAY_ENABLE			1
//#define	__DEBUG_PRINT__			0

#define APP_FLASH_PROTECTION_ENABLE     1

#define SAVE_SNO_CACHE_EN				0
#if SAVE_SNO_CACHE_EN
// if save cache once receive a new message, it will need to do many flash save action when such as get all lightness, all publish to all.
#define SAVE_SNO_CACHE_ONLY_WHEN_DST_ADDR_MATCH_EN      1 // 0 means save cache once receiving a new message. 1 means save only when need to handle this message.
#endif

//////////// product  Information  //////////////////////////////
#define ID_VENDOR				0x248a			// for report
#define ID_PRODUCT_BASE			0x880C
#define STRING_VENDOR			L"Telink"
#define STRING_PRODUCT			L"BLE Mesh"
#define STRING_SERIAL			L"TLSR825X"

#define DEV_NAME                "SigMesh"

#define APPLICATION_DONGLE		0					// or else APPLICATION_DEVICE
#define	USB_PRINTER				1
#define	FLOW_NO_OS				1

///////////////////// CDTP ///////////////////////////////////////
#define MESH_CDTP_ENABLE							0
#if (MESH_CDTP_ENABLE)
#define LE_AUDIO_DEMO_ENABLE                    	1
#define L2CAP_CREDIT_BASED_FLOW_CONTROL_MODE_EN     1
#define SLAVE_COC_ENABLE                            1
#define APP_LOG_EN									1

#define CDTP_SMP_LEVEL								3 // must 3 or 4 // CDTP spec require SMP >= level 3
#endif

/////////////////////HCI ACCESS OPTIONS///////////////////////////////////////
#define HCI_USE_NONE	0
#define HCI_USE_UART	1
#define HCI_USE_USB		2 // just for compile, tc321x doesn't support USB

#ifndef HCI_ACCESS
    #if SMART_PROVISION_ENABLE
#define HCI_ACCESS		HCI_USE_UART  // for reuse hci_rx_fifo and hci_tx_fifo callback
    #else
#define HCI_ACCESS		HCI_USE_UART
    #endif
#endif

#if (HCI_ACCESS==HCI_USE_UART)
    #if (BOARD_SELECT == BOARD_TC321X_EVK_C1T357A20_V1_2)
#define UART_TX_PIN		GPIO_PD0
#define UART_RX_PIN		GPIO_PD1
    #else
#define UART_TX_PIN		GPIO_PD0
#define UART_RX_PIN		GPIO_PD1
    #endif
#elif(HCI_ACCESS==HCI_USE_USB)
#error "CHIP TC321X doesn't support USB!"
#endif

///////////////////////// DEBUG PIN Configuration ////////////////////////////////////////////////
#ifndef HCI_LOG_FW_EN
#define HCI_LOG_FW_EN               (0 || DEBUG_LOG_SETTING_DEVELOP_MODE_EN)
#if HCI_LOG_FW_EN
	#if(BOARD_SELECT == BOARD_TC321X_EVK_C1T357A20_V1_2)
#define DEBUG_INFO_TX_PIN           GPIO_PB2	
	#else
#define DEBUG_INFO_TX_PIN           GPIO_PB2
	#endif
#define PRINT_DEBUG_INFO            1
#endif
#endif

#define BATT_CHECK_ENABLE           1   //must enable
#if (BATT_CHECK_ENABLE)
//telink device: you must choose one gpio with adc function to output high level(voltage will equal to vbat), then use adc to measure high level voltage
	//use PB1 output high level, then adc measure this high level voltage
	#define GPIO_VBAT_DETECT		SD_ADC_GPIO_PB5P // corresponding sd_adc_p_input_pin_def_e
#endif

#define ADC_ENABLE		0
#if ADC_ENABLE
#define ADC_BASE_MODE	1	//GPIO voltage
#define ADC_VBAT_MODE	2	//Battery Voltage

#define ADC_MODE		ADC_VBAT_MODE
#define ADC_CHNM_ANA_INPUT 		GPIO_PB3 // one of ADC_GPIO_tab[]
#define ADC_PRESCALER	ADC_PRESCALER_1F8
#endif

#define ONLINE_STATUS_EN        0
/////////////////// mesh project config /////////////////////////////////
#ifndef TRANSITION_TIME_DEFAULT_VAL
#define TRANSITION_TIME_DEFAULT_VAL (0x00)  // 0x41: 1 second // 0x00: means no default transition time
#endif

#define PROVISIONER_GATT_ADV_EN	    (0 || MESH_CDTP_ENABLE)

#if EXTENDED_ADV_ENABLE
#define MESH_DLE_MODE               MESH_DLE_MODE_EXTEND_BEAR
#define DLE_LEN_MAX_RX              (MAX_OCTETS_DATA_LEN_EXTENSION) // must MAX_OCTETS_DATA_LEN_EXTENSION
#define DLE_LEN_MAX_TX              (40)
#endif

#define FW_LOAD_BY_BOOTLOADER_AND_NO_OTA_EN     0
#if FW_LOAD_BY_BOOTLOADER_AND_NO_OTA_EN
#define FW_START_BY_BOOTLOADER_ADDR     	    (0x20000)
#endif

/////////////////// MODULE /////////////////////////////////
#define BLE_REMOTE_PM_ENABLE			(0 || GW_SMART_PROVISION_REMOTE_CONTROL_PM_EN)
#if BLE_REMOTE_PM_ENABLE
#define PM_DEEPSLEEP_RETENTION_ENABLE   1   // must
#else
#define PM_DEEPSLEEP_RETENTION_ENABLE   0
#endif
#define BLE_REMOTE_SECURITY_ENABLE      (0 || MESH_CDTP_ENABLE)   // 1 for smp,  0 no security
#define BLE_IR_ENABLE					0
#if FW_LOAD_BY_BOOTLOADER_AND_NO_OTA_EN
#define BLE_OTA_SERVER_ENABLE			0
#else
#define BLE_OTA_SERVER_ENABLE			1
#endif

#if GW_SMART_PROVISION_REMOTE_CONTROL_PM_EN
#define BLT_SOFTWARE_TIMER_ENABLE		1
#endif

#ifndef BLT_SOFTWARE_TIMER_ENABLE
#define BLT_SOFTWARE_TIMER_ENABLE		0
#endif

//----------------------- keyboard --------------------------------
#if (SMART_PROVISION_ENABLE || MESH_RX_TEST)
#define UI_KEYBOARD_ENABLE				1
#endif

#ifndef UI_KEYBOARD_ENABLE
#define UI_KEYBOARD_ENABLE				0
#endif

#if UI_KEYBOARD_ENABLE
#define	MATRIX_ROW_PULL					PM_PIN_PULLDOWN_100K // drive pin pull
#define	MATRIX_COL_PULL					PM_PIN_PULLUP_10K    // scan pin pull

#define	KB_LINE_HIGH_VALID				0   // dirve pin output 0 when keyscan(no drive pin in KB_LINE_MODE=1), scanpin read 0 is valid
#define DEEPBACK_FAST_KEYSCAN_ENABLE	1   //proc fast scan when deepsleep back triggered by key press, in case key loss
#define KEYSCAN_IRQ_TRIGGER_MODE		0
#define LONG_PRESS_KEY_POWER_OPTIMIZE	1   //lower power when pressing key without release

#define	KB_MAP_NUM				KB_MAP_NORMAL
#define	KB_MAP_FN				KB_MAP_NORMAL

#if 0 // reserve for dongle board
// key mode, KB_LINE_MODE default 0(key matrix), set to 1 in button mode.
#define KB_LINE_MODE			1 			
			
// keymap
#define KEY_SW1					1
#define KEY_SW2					2
#define KB_MAP_NORMAL			{{KEY_SW1},	{KEY_SW2}}
			
#define KB_DRIVE_PINS			{GPIO_PD6} 	// make no sense, just for compile, not driver pin in KB_LINE_MODE=1.
#define KB_SCAN_PINS			{GPIO_PD6, GPIO_PD5}
			
// scan pin as gpio
#define PD5_FUNC				AS_GPIO
#define PD6_FUNC				AS_GPIO
			
//scan	pin pullup
#define PULL_WAKEUP_SRC_PD6     PM_PIN_PULLUP_1M	//btn
#define PULL_WAKEUP_SRC_PD5     PM_PIN_PULLUP_1M	//btn
			
//scan pin open input to read gpio level
#define PD6_INPUT_ENABLE		1
#define PD5_INPUT_ENABLE		1
#elif(BOARD_SELECT == BOARD_TC321X_EVK_C1T357A20_V1_2)
// keymap
#define KEY_SW1		1
#define KEY_SW2		2
#define KEY_SW3		3
#define KEY_SW4		4
#define KB_MAP_NORMAL           {                           /*PCB mark*/ \
                                {KEY_SW1,   KEY_SW2},       /*SW3, SW4*/ \
                                {KEY_SW3,   KEY_SW4},}      /*SW5, SW6*/

#define KB_DRIVE_PINS 		 	{GPIO_PA4, GPIO_PA2}
#define KB_SCAN_PINS   			{GPIO_PB0, GPIO_PA1}

#define	SW1_GPIO				GPIO_PB0
#define	SW2_GPIO				GPIO_PA1

//drive pin as gpio
#define	PA4_FUNC				AS_GPIO
#define	PA2_FUNC				AS_GPIO

//drive pin need 100K pulldown
#define	PULL_WAKEUP_SRC_PA4		MATRIX_ROW_PULL
#define	PULL_WAKEUP_SRC_PA2		MATRIX_ROW_PULL

//drive pin open input to read gpio wakeup level
#define PA4_INPUT_ENABLE		1
#define PA2_INPUT_ENABLE		1

//scan pin as gpio
#define	PB0_FUNC				AS_GPIO
#define	PA1_FUNC				AS_GPIO

//scan  pin need 10K pullup
#define	PULL_WAKEUP_SRC_PB0		MATRIX_COL_PULL
#define	PULL_WAKEUP_SRC_PA1		MATRIX_COL_PULL

//scan pin open input to read gpio level
#define PB0_INPUT_ENABLE		1
#define PA1_INPUT_ENABLE		1
#elif(BOARD_SELECT == BOARD_TC321X_EVK_C1T357A20_V2_0)
// keymap
#define KEY_SW1		1
#define KEY_SW2		2
#define KEY_SW3		3
#define KEY_SW4		4
#define KB_MAP_NORMAL           {                           /*PCB mark*/ \
                                {KEY_SW1,   KEY_SW2},       /*SW3, SW4*/ \
                                {KEY_SW3,   KEY_SW4},}      /*SW5, SW6*/

#define KB_DRIVE_PINS 		 	{GPIO_PD7, GPIO_PD5}
#define KB_SCAN_PINS   			{GPIO_PB3, GPIO_PB1}

#define	SW1_GPIO				GPIO_PB3
#define	SW2_GPIO				GPIO_PB1

//drive pin as gpio
#define	PD7_FUNC				AS_GPIO
#define	PD5_FUNC				AS_GPIO

//drive pin need 100K pulldown
#define	PULL_WAKEUP_SRC_PD7		MATRIX_ROW_PULL
#define	PULL_WAKEUP_SRC_PD5		MATRIX_ROW_PULL

//drive pin open input to read gpio wakeup level
#define PD7_INPUT_ENABLE		1
#define PD5_INPUT_ENABLE		1

//scan pin as gpio
#define	PB3_FUNC				AS_GPIO
#define	PB1_FUNC				AS_GPIO

//scan  pin need 10K pullup
#define	PULL_WAKEUP_SRC_PB3		MATRIX_COL_PULL
#define	PULL_WAKEUP_SRC_PB1		MATRIX_COL_PULL

//scan pin open input to read gpio level
#define PB3_INPUT_ENABLE		1
#define PB1_INPUT_ENABLE		1
#else
	#error "Current board do not support keyboard !"
#endif
#endif

//---------------  LED / PWM
#if(BOARD_SELECT == BOARD_TC321X_EVK_C1T357A20_V1_2)
#define PWM_R       GPIO_PB6		//red
#define PWM_G       GPIO_PB7		//green
#define PWM_B       GPIO_PC4		//blue
#define PWM_W       GPIO_PB4		//white
#elif(BOARD_SELECT == BOARD_TC321X_EVK_C1T357A20_V2_0)
#define PWM_R       GPIO_PB7		//red
#define PWM_G       GPIO_PB6		//green
#define PWM_B       GPIO_PD2		//blue
#define PWM_W       GPIO_PB4		//white
#endif

#define PWM_FUNC_R  PWM0
#define PWM_FUNC_G  PWM1
#define PWM_FUNC_B  PWM2
#define PWM_FUNC_W  PWM3

#define PWMID_R     PWM0_ID
#define PWMID_G     PWM1_ID
#define PWMID_B     PWM2_ID
#define PWMID_W     PWM3_ID

#define PWM_INV_R   (GET_PWM_INVERT_VAL(PWM_R))
#define PWM_INV_G   (GET_PWM_INVERT_VAL(PWM_G))
#define PWM_INV_B   (GET_PWM_INVERT_VAL(PWM_B))
#define PWM_INV_W   (GET_PWM_INVERT_VAL(PWM_W))

#ifndef GPIO_LED
#define GPIO_LED	PWM_R
#endif

/////////////////// Clock  /////////////////////////////////
#define CLOCK_SYS_CLOCK_HZ  	48000000

/////////////////// watchdog  //////////////////////////////
#define MODULE_WATCHDOG_ENABLE		1
#define WATCHDOG_INIT_TIMEOUT		2000  // unit:ms

/////////////////// set mesh   ////////////////
#define DONGLE_PROVISION_EN			1

/////////////////// set default   ////////////////

#include "../common/default_config.h"

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
