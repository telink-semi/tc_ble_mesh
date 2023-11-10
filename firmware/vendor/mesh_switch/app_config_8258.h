/********************************************************************************************************
 * @file	app_config_8258.h
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
#define PCBA_8258_SW_C1T140A5_V1_1      1	// switch key board
#define PCBA_8258_DONGLE_48PIN          2
#define PCBA_8258_C1T140A3_V1_1         3   // 32pin dongle

#ifndef PCBA_8258_SEL // user can define in user_app_config.h
#if (MESH_USER_DEFINE_MODE == MESH_IRONMAN_MENLO_ENABLE)
#define PCBA_8258_SEL			PCBA_8258_C1T140A3_V1_1	//
#else
#define PCBA_8258_SEL			PCBA_8258_SW_C1T140A5_V1_1	// PCBA_8258_DONGLE_48PIN
#endif
#endif


#define _USER_CONFIG_DEFINED_	1	// must define this macro to make others known
#define	__LOG_RT_ENABLE__		0
//#define	__DEBUG_PRINT__			0

#if DUAL_MESH_ZB_BL_EN
#define FLASH_1M_ENABLE         1
#elif DUAL_VENDOR_EN
#define FLASH_1M_ENABLE         0
#else
#define FLASH_1M_ENABLE         0
#endif

#if FLASH_1M_ENABLE
#if DUAL_MESH_ZB_BL_EN
#define PINGPONG_OTA_DISABLE    1 // it can disable only when 1M flash.
#else
#define PINGPONG_OTA_DISABLE    0 // it can disable only when 1M flash.
#endif
#if	PINGPONG_OTA_DISABLE
#define SWITCH_FW_ENABLE		0 // set to 0, just for particular customer 
#endif
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

/////////////////////HCI ACCESS OPTIONS///////////////////////////////////////
#define HCI_USE_NONE	0
#define HCI_USE_UART	1
#define HCI_USE_USB		2
#define HCI_ACCESS		HCI_USE_NONE

#if (HCI_ACCESS==HCI_USE_UART)
#define UART_TX_PIN		UART_TX_PB1
#define UART_RX_PIN		UART_RX_PB0
#endif

#define HCI_LOG_FW_EN   (0 || DEBUG_LOG_SETTING_DEVELOP_MODE_EN)
#if HCI_LOG_FW_EN
	#if (PCBA_8258_SEL == PCBA_8258_DONGLE_48PIN)
#define DEBUG_INFO_TX_PIN           		GPIO_PB2
	#elif (PCBA_8258_SEL == PCBA_8258_C1T140A3_V1_1)
#define DEBUG_INFO_TX_PIN           		GPIO_PB6
	#else // PCBA_8258_SW_C1T140A5_V1_1
#define DEBUG_INFO_TX_PIN           		GPIO_PC3
	#endif
#define PRINT_DEBUG_INFO                    1
#endif

#define BATT_CHECK_ENABLE       			1   //must enable
#if (BATT_CHECK_ENABLE)
//telink device: you must choose one gpio with adc function to output high level(voltage will equal to vbat), then use adc to measure high level voltage
	//use PC5 output high level, then adc measure this high level voltage
	#define GPIO_VBAT_DETECT				GPIO_PC5
	#define PC5_FUNC						AS_GPIO
	#define PC5_INPUT_ENABLE				0
	#define ADC_INPUT_PCHN					C5P    //corresponding  ADC_InputPchTypeDef in adc.h
#endif

#define ADC_ENABLE		0
#if ADC_ENABLE
#define ADC_BASE_MODE	1	//GPIO voltage
#define ADC_VBAT_MODE	2	//Battery Voltage

#define ADC_MODE		ADC_VBAT_MODE
#define ADC_CHNM_ANA_INPUT 		GPIO_PB3 // one of ADC_GPIO_tab[]
#define ADC_PRESCALER	ADC_PRESCALER_1F8
#endif

#if DUAL_MESH_ZB_BL_EN
#define DUAL_MODE_ADAPT_EN 			1   // dual mode as master with Zigbee
#else
#define DUAL_MODE_ADAPT_EN 			0   // dual mode as master with Zigbee
#endif
#if (0 == DUAL_MODE_ADAPT_EN)
#define DUAL_MODE_WITH_TLK_MESH_EN  0   // dual mode as slave with Telink mesh
#endif

/////////////////// mesh project config /////////////////////////////////
#define TRANSITION_TIME_DEFAULT_VAL (0x00)  // 0x41: 1 second // 0x00: means no default transition time

/////////////////// MODULE /////////////////////////////////
#if (SWITCH_ALWAYS_MODE_GATT_EN)
#define BLE_REMOTE_PM_ENABLE			0
#else
#define BLE_REMOTE_PM_ENABLE			1
#endif
#if BLE_REMOTE_PM_ENABLE
#define PM_DEEPSLEEP_RETENTION_ENABLE   1   // must
#else
#define PM_DEEPSLEEP_RETENTION_ENABLE   0
#endif
#define BLE_REMOTE_SECURITY_ENABLE      0
#define BLE_IR_ENABLE					0

#ifndef BLT_SOFTWARE_TIMER_ENABLE
#define BLT_SOFTWARE_TIMER_ENABLE		1
#endif

//---------------  LED / PWM //invalid code
#if 1
#define PWM_R       GPIO_PB1	//red
#define PWM_G       GPIO_PB1	//green
#define PWM_B       GPIO_PB1		//blue
#define PWM_W       GPIO_PB1		//white

//#define PWM_FUNC_R  AS_PWM  // AS_PWM_SECOND
//#define PWM_FUNC_G  AS_PWM  // AS_PWM_SECOND
//#define PWM_FUNC_B  AS_PWM  // AS_PWM_SECOND
//#define PWM_FUNC_W  AS_PWM  // AS_PWM_SECOND

#define PWMID_R     (GET_PWMID(PWM_R, PWM_FUNC_R))
#define PWMID_G     (GET_PWMID(PWM_G, PWM_FUNC_G))
#define PWMID_B     (GET_PWMID(PWM_B, PWM_FUNC_B))
#define PWMID_W     (GET_PWMID(PWM_W, PWM_FUNC_W))
                    
#define PWM_INV_R   (GET_PWM_INVERT_VAL(PWM_R, PWM_FUNC_R))
#define PWM_INV_G   (GET_PWM_INVERT_VAL(PWM_G, PWM_FUNC_G))
#define PWM_INV_B   (GET_PWM_INVERT_VAL(PWM_B, PWM_FUNC_B))
#define PWM_INV_W   (GET_PWM_INVERT_VAL(PWM_W, PWM_FUNC_W))
#endif

#if (PCBA_8258_SEL == PCBA_8258_DONGLE_48PIN)
#define GPIO_LED	GPIO_PA3
#elif (PCBA_8258_SEL == PCBA_8258_C1T140A3_V1_1)
#define GPIO_LED	GPIO_PD4
#else // PCBA_8258_SW_C1T140A5_V1_1
#define GPIO_LED	GPIO_PD4
#endif


/////////////open SWS digital pullup to prevent MCU err, this is must ////////////
#define PA7_DATA_OUT			1


/////////////////// Clock  /////////////////////////////////
#define	USE_SYS_TICK_PER_US
#define CLOCK_SYS_TYPE  		CLOCK_TYPE_PLL	//  one of the following:  CLOCK_TYPE_PLL, CLOCK_TYPE_OSC, CLOCK_TYPE_PAD, CLOCK_TYPE_ADC

#if DUAL_MESH_ZB_BL_EN // keep same with zb
#define CLOCK_SYS_CLOCK_HZ  	32000000
#elif 0 // EXTENDED_ADV_ENABLE
#define CLOCK_SYS_CLOCK_HZ  	48000000		// need to process rx buffer quickly
#else
#define CLOCK_SYS_CLOCK_HZ  	32000000
#endif
//////////////////Extern Crystal Type///////////////////////
#define CRYSTAL_TYPE			XTAL_12M		//  extern 12M crystal

/////////////////// watchdog  //////////////////////////////
#define MODULE_WATCHDOG_ENABLE		1
#define WATCHDOG_INIT_TIMEOUT		2000  //ms


#define	REGA_TID			0x3A

//----------------------- keyboard --------------------------------
#define UI_KEYBOARD_ENABLE				1
			
#if UI_KEYBOARD_ENABLE
#define	MATRIX_ROW_PULL					PM_PIN_PULLDOWN_100K // drive pin pull
#define	MATRIX_COL_PULL					PM_PIN_PULLUP_10K    // scan pin pull
			
#define	KB_LINE_HIGH_VALID				0   // dirve pin output 0 when keyscan(no drive pin in KB_LINE_MODE=1), scanpin read 0 is valid
#define DEEPBACK_FAST_KEYSCAN_ENABLE	1   //proc fast scan when deepsleep back trigged by key press, in case key loss
#define KEYSCAN_IRQ_TRIGGER_MODE		0
#define LONG_PRESS_KEY_POWER_OPTIMIZE	1   //lower power when pressing key without release

//stuck key
#define STUCK_KEY_PROCESS_ENABLE		0
#define STUCK_KEY_ENTERDEEP_TIME		60  //in s

//repeat key
#define KB_REPEAT_KEY_ENABLE			0
#define	KB_REPEAT_KEY_INTERVAL_MS		200
#define KB_REPEAT_KEY_NUM				4

#define	KB_MAP_REPEAT			{RC_KEY_UP,RC_KEY_DN,RC_KEY_R,RC_KEY_L}

#define	KB_MAP_NUM				KB_MAP_NORMAL
#define	KB_MAP_FN				KB_MAP_NORMAL
			
	#if (PCBA_8258_SEL == PCBA_8258_DONGLE_48PIN)
// key mode, KB_LINE_MODE default 0(key matrix), set to 1 in button mode.
#define KB_LINE_MODE			1 			
			
// keymap
#define KEY_RESET				1
#define KEY_CMD					2
#define KB_MAP_NORMAL			{{KEY_RESET},	{KEY_CMD}}
			
#define KB_DRIVE_PINS			{GPIO_PD6} 	// just for compile, not driver pin in KB_LINE_MODE=1.
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
	#elif (PCBA_8258_SEL == PCBA_8258_C1T140A3_V1_1)
// key mode, KB_LINE_MODE default 0(key matrix), set to 1 in button mode.
#define KB_LINE_MODE			1 			
			
// keymap
#define KEY_RESET				1
#define KEY_CMD					2
#define KB_MAP_NORMAL			{{KEY_RESET},	{KEY_CMD}}
			
#define KB_DRIVE_PINS			{GPIO_PD7} 	// just for compile, not driver pin in KB_LINE_MODE=1.
#define KB_SCAN_PINS			{GPIO_PD7, GPIO_PA1}
			
// scan pin as gpio
#define PD7_FUNC				AS_GPIO
#define PA1_FUNC				AS_GPIO
			
//scan	pin pullup
#define PULL_WAKEUP_SRC_PD7     PM_PIN_PULLUP_1M	//btn
#define PULL_WAKEUP_SRC_PA1     PM_PIN_PULLUP_1M	//btn
			
//scan pin open input to read gpio level
#define PD7_INPUT_ENABLE		1
#define PA1_INPUT_ENABLE		1
	#elif(PCBA_8258_SEL == PCBA_8258_C1T139A30_V1_2)
// keymap
#define KEY_SW1					1
#define KEY_SW2					2
#define KEY_SW3					3
#define KEY_SW4					4
#define	KB_MAP_NORMAL			{\
								{KEY_SW1,	KEY_SW2},	 \
								{KEY_SW3,	KEY_SW4},	 }
			
#define KB_DRIVE_PINS 		 	{GPIO_PB4, GPIO_PB5}
#define KB_SCAN_PINS   			{GPIO_PB2, GPIO_PB3}
			
//drive pin as gpio
#define	PB4_FUNC				AS_GPIO
#define	PB5_FUNC				AS_GPIO
			
//drive pin need 100K pulldown
#define	PULL_WAKEUP_SRC_PB4		MATRIX_ROW_PULL
#define	PULL_WAKEUP_SRC_PB5		MATRIX_ROW_PULL
			
//drive pin open input to read gpio wakeup level
#define PB4_INPUT_ENABLE		1
#define PB5_INPUT_ENABLE		1
			
//scan pin as gpio
#define	PB2_FUNC				AS_GPIO
#define	PB3_FUNC				AS_GPIO
			
//scan	pin need 10K pullup
#define	PULL_WAKEUP_SRC_PB2		MATRIX_COL_PULL
#define	PULL_WAKEUP_SRC_PB3		MATRIX_COL_PULL
			
//scan pin open input to read gpio level
#define PB2_INPUT_ENABLE		1
#define PB3_INPUT_ENABLE		1
	#elif(PCBA_8258_SEL == PCBA_8258_C1T139A30_V1_0)    // PCBA_8258_C1T139A30_V1_0
// key mode, KB_LINE_MODE default 0(key matrix), set to 1 in button mode.
#define KB_LINE_MODE			1 			
			
// keymap
#define KEY_RESET				1
#define KEY_CMD					2
#define KB_MAP_NORMAL			{{KEY_RESET},	{KEY_CMD}}
			
#define KB_DRIVE_PINS			{GPIO_PD2} 	// just for compile, not driver pin in KB_LINE_MODE=1.
#define KB_SCAN_PINS			{GPIO_PD2, GPIO_PD1}
			
// scan pin as gpio
#define PD2_FUNC				AS_GPIO
#define PD1_FUNC				AS_GPIO
			
//scan	pin pullup
#define PULL_WAKEUP_SRC_PD2     PM_PIN_PULLUP_1M	//btn
#define PULL_WAKEUP_SRC_PD1     PM_PIN_PULLUP_1M	//btn
			
//scan pin open input to read gpio level
#define PD2_INPUT_ENABLE		1
#define PD1_INPUT_ENABLE		1
	#else
// keymap
#define	KB_MAP_NORMAL			{\
								{RC_KEY_UP, 		RC_KEY_R,			RC_KEY_4_ON}, \
								{RC_KEY_L,			RC_KEY_DN,			RC_KEY_4_OFF}, \
								{RC_KEY_1_ON,		RC_KEY_2_ON,		RC_KEY_3_ON}, \
								{RC_KEY_1_OFF,		RC_KEY_2_OFF,		RC_KEY_3_OFF}, \
								{RC_KEY_A_ON,		RC_KEY_A_OFF,		RC_KEY_M}, }
									
#define KB_DRIVE_PINS  			{GPIO_PB7, GPIO_PD7, GPIO_PA1}
#define KB_SCAN_PINS   			{GPIO_PB4, GPIO_PB5, GPIO_PB6, GPIO_PC1, GPIO_PD3}
							
//drive pin as gpio
#define	PB7_FUNC				AS_GPIO
#define	PD7_FUNC				AS_GPIO
#define	PA1_FUNC				AS_GPIO	

//drive pin need 100K pulldown
#define	PULL_WAKEUP_SRC_PB7		MATRIX_ROW_PULL
#define	PULL_WAKEUP_SRC_PD7		MATRIX_ROW_PULL
#define	PULL_WAKEUP_SRC_PA1		MATRIX_ROW_PULL

//drive pin open input to read gpio wakeup level
#define PB7_INPUT_ENABLE		1
#define PD7_INPUT_ENABLE		1
#define PA1_INPUT_ENABLE		1

//scan pin as gpio
#define	PB4_FUNC				AS_GPIO
#define	PB5_FUNC				AS_GPIO
#define	PB6_FUNC				AS_GPIO
#define	PC1_FUNC				AS_GPIO
#define	PD3_FUNC				AS_GPIO

//scan	pin need 10K pullup
#define	PULL_WAKEUP_SRC_PB4		MATRIX_COL_PULL
#define	PULL_WAKEUP_SRC_PB5		MATRIX_COL_PULL
#define	PULL_WAKEUP_SRC_PB6		MATRIX_COL_PULL
#define	PULL_WAKEUP_SRC_PC1		MATRIX_COL_PULL
#define	PULL_WAKEUP_SRC_PD3		MATRIX_COL_PULL

//scan pin open input to read gpio level
#define PB4_INPUT_ENABLE		1
#define PB5_INPUT_ENABLE		1
#define PB6_INPUT_ENABLE		1
#define PC1_INPUT_ENABLE		1
#define PD3_INPUT_ENABLE		1
	#endif
#endif

/////////////////// set default   ////////////////

#include "../common/default_config.h"

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
