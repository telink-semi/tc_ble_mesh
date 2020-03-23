/********************************************************************************************************
 * @file     app_config_8258.h 
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
#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

#include "../../vendor/common/version.h"    // include mesh_config.h inside.
//////////////////board sel/////////////////////////////////////
#define PCBA_8258_DONGLE_48PIN          1
#define PCBA_8258_C1T139A30_V1_0        2
#define PCBA_8258_C1T139A30_V1_2        3
#define PCBA_8258_SEL			PCBA_8258_C1T139A30_V1_2


#define _USER_CONFIG_DEFINED_	1	// must define this macro to make others known
#define	__LOG_RT_ENABLE__		0

#define FLASH_1M_ENABLE         1   // must
#if FLASH_1M_ENABLE
#define PINGPONG_OTA_DISABLE    1 // it can disable only when 1M flash.
#endif

#define GATEWAY_ENABLE			1

//#define	__DEBUG_PRINT__			0
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
#define HCI_ACCESS		HCI_USE_USB

#if (HCI_ACCESS==HCI_USE_UART)
#define UART_TX_PIN		UART_TX_PB1
#define UART_RX_PIN		UART_RX_PB0
#endif

#define HCI_LOG_FW_EN   0
#if HCI_LOG_FW_EN
#define DEBUG_INFO_TX_PIN           		GPIO_PC7
#define PRINT_DEBUG_INFO                    1
#endif

/////////////////// mesh project config /////////////////////////////////
#define TRANSITION_TIME_DEFAULT_VAL (0x00)  // 0x41: 1 second // 0x00: means no default transition time

/////////////////// MODULE /////////////////////////////////
#define BLE_REMOTE_PM_ENABLE			0
#define PM_DEEPSLEEP_RETENTION_ENABLE   0
#define BLE_REMOTE_SECURITY_ENABLE      0
#define BLE_IR_ENABLE					0

#ifndef BLT_SOFTWARE_TIMER_ENABLE
#define BLT_SOFTWARE_TIMER_ENABLE		0
#endif

//////////////////////////// KEYSCAN/MIC  GPIO //////////////////////////////////
#define	MATRIX_ROW_PULL					PM_PIN_PULLDOWN_100K
#define	MATRIX_COL_PULL					PM_PIN_PULLUP_10K

#define	KB_LINE_HIGH_VALID				0   //dirve pin output 0 when keyscan, scanpin read 0 is valid
#define DEEPBACK_FAST_KEYSCAN_ENABLE	1   //proc fast scan when deepsleep back trigged by key press, in case key loss
#define KEYSCAN_IRQ_TRIGGER_MODE		0
#define LONG_PRESS_KEY_POWER_OPTIMIZE	1   //lower power when pressing key without release

//stuck key
#define STUCK_KEY_PROCESS_ENABLE		0
#define STUCK_KEY_ENTERDEEP_TIME		60  //in s

//repeat key
#define KB_REPEAT_KEY_ENABLE			0
#define	KB_REPEAT_KEY_INTERVAL_MS		200
#define KB_REPEAT_KEY_NUM				1
//

//----------------------- GPIO for UI --------------------------------
//---------------  Button 
#if (PCBA_8258_SEL == PCBA_8258_DONGLE_48PIN)
#define PULL_WAKEUP_SRC_PD6     PM_PIN_PULLUP_1M	//btn
#define PULL_WAKEUP_SRC_PD5     PM_PIN_PULLUP_1M	//btn
#define PD6_INPUT_ENABLE		1
#define PD5_INPUT_ENABLE		1
#define	SW1_GPIO				GPIO_PD6
#define	SW2_GPIO				GPIO_PD5
#else   // PCBA_8258_DEVELOPMENT_BOARD
//#define PULL_WAKEUP_SRC_PD2     PM_PIN_PULLUP_1M	//btn
//#define PULL_WAKEUP_SRC_PD1     PM_PIN_PULLUP_1M	//btn
//#define PD2_INPUT_ENABLE		1
//#define PD1_INPUT_ENABLE		1
#define	SW1_GPIO				GPIO_PD2
#define	SW2_GPIO				GPIO_PD1
#endif

//---------------  LED / PWM
#if(PCBA_8258_SEL == PCBA_8258_DONGLE_48PIN)
#define PWM_R       GPIO_PWM1A3		//red
#define PWM_G       GPIO_PWM0A2		//green
#define PWM_B       GPIO_PWM3B0		//blue
#define PWM_W       GPIO_PWM4B1		//white
#elif(PCBA_8258_SEL == PCBA_8258_C1T139A30_V1_0)   // PCBA_8258_DEVELOPMENT_BOARD
#define PWM_R       GPIO_PWM1ND3	//red
#define PWM_G       GPIO_PWM2ND4	//green
#define PWM_B       GPIO_PD5		//blue
#define PWM_W       GPIO_PWM3D2		//white
#elif(PCBA_8258_SEL == PCBA_8258_C1T139A30_V1_2)
#define PWM_R       GPIO_PD5	//red
#define PWM_G       GPIO_PWM1ND3	//green
#define PWM_B       GPIO_PWM3D2		//blue
#define PWM_W       GPIO_PWM2ND4		//white

#endif

#define PWM_FUNC_R  AS_PWM  // AS_PWM_SECOND
#define PWM_FUNC_G  AS_PWM  // AS_PWM_SECOND
#define PWM_FUNC_B  AS_PWM  // AS_PWM_SECOND
#define PWM_FUNC_W  AS_PWM  // AS_PWM_SECOND

#define PWMID_R     (GET_PWMID(PWM_R, PWM_FUNC_R))
#define PWMID_G     (GET_PWMID(PWM_G, PWM_FUNC_G))
#define PWMID_B     (GET_PWMID(PWM_B, PWM_FUNC_B))
#define PWMID_W     (GET_PWMID(PWM_W, PWM_FUNC_W))
                    
#define PWM_INV_R   (GET_PWM_INVERT_VAL(PWM_R, PWM_FUNC_R))
#define PWM_INV_G   (GET_PWM_INVERT_VAL(PWM_G, PWM_FUNC_G))
#define PWM_INV_B   (GET_PWM_INVERT_VAL(PWM_B, PWM_FUNC_B))
#define PWM_INV_W   (GET_PWM_INVERT_VAL(PWM_W, PWM_FUNC_W))

#define GPIO_LED	PWM_R


/////////////open SWS digital pullup to prevent MCU err, this is must ////////////
#define PA7_DATA_OUT			1


typedef volatile struct{
	volatile unsigned char * flash_adr_char_value;
	volatile unsigned char * flash_adr_sw_uuid;
	volatile unsigned char * flash_adr_sw_token;
	volatile unsigned char * flash_adr_device_id;
	volatile unsigned char * flash_adr_hash_id;
	volatile unsigned char * flash_adr_setupcode;
	volatile unsigned char * flash_adr_setupcode_write_cnt;
	volatile unsigned char * flash_adr_setupcode_read_cnt;
	volatile unsigned char * flash_adr_setup_id;
	volatile unsigned char * flash_adr_serial_number;
	volatile unsigned char * flash_adr_accessory_ltsk;
	volatile unsigned char * flash_adr_srp_key;
	volatile unsigned char * flash_adr_srp_key_crc_data;
	volatile unsigned char * flash_adr_id_info;
	volatile unsigned char * flash_adr_factory_reset_cnt;
	volatile unsigned char * flash_adr_global_state;
	volatile unsigned char * flash_adr_mac;
	volatile unsigned char * flash_adr_dc;
	volatile unsigned char * flash_adr_tp_low;
	volatile unsigned char * flash_adr_tp_high;
	volatile unsigned char * flash_adr_service;
	volatile unsigned char * flash_adr_fw_update_cnt;
	volatile unsigned char * flash_adr_adv_encryption_key;
	volatile unsigned char * flash_adr_new_id;
	volatile unsigned char * flash_adr_broadcast_gsn;
	volatile unsigned char * flash_adr_srp_key2;
	volatile unsigned char * flash_adr_srp_key2_crc_data;
	volatile unsigned char * flash_adr_saltchar;
}flash_adr_layout_def;


/////////////////// Clock  /////////////////////////////////
#define	USE_SYS_TICK_PER_US
#define CLOCK_SYS_TYPE  		CLOCK_TYPE_PLL	//  one of the following:  CLOCK_TYPE_PLL, CLOCK_TYPE_OSC, CLOCK_TYPE_PAD, CLOCK_TYPE_ADC
#define CLOCK_SYS_CLOCK_HZ  	48000000

//////////////////Extern Crystal Type///////////////////////
#define CRYSTAL_TYPE			XTAL_12M		//  extern 12M crystal

/////////////////// watchdog  //////////////////////////////
#define MODULE_WATCHDOG_ENABLE		1
#define WATCHDOG_INIT_TIMEOUT		5000  //ms

/////////////////// set mesh   ////////////////
#define DONGLE_PROVISION_EN			1

///////////////////homekit//////////////////////////
#define BLE_APP_PM_ENABLE					0
//#define PM_DEEPSLEEP_RETENTION_ENABLE		0
//#define BLE_REMOTE_SECURITY_ENABLE      	0
#define HK_SALTCHAR_RANDOM_FOR_SRP_ENABLE	1

#if HK_SALTCHAR_RANDOM_FOR_SRP_ENABLE
#define HK_DIFF_SRP_IN_PAIR_SETUP_ENABLE		1	//for TCH084
#else
#define HK_DIFF_SRP_IN_PAIR_SETUP_ENABLE		0
#endif

///////////////////// MFI Enable //////////////////////////
#if MFI_ENABLE          // for 8258 EVK with MFi
    #define PIN_MFI_I2C_RST     	GPIO_PA2
    #define PD1_FUNC                AS_GPIO
    #define PD1_INPUT_ENABLE        0
    #define PD1_OUTPUT_ENABLE       1
    #define PD1_DATA_STRENGTH       1
    #define PD1_DATA_OUT            1
    #define PULL_WAKEUP_SRC_PD1     PM_PIN_PULLUP_10K

    #define PIN_I2C_SDA         	GPIO_PA4
    #define PA3_FUNC                AS_GPIO
    #define PA3_INPUT_ENABLE        1
    #define PA3_OUTPUT_ENABLE       1
    #define PA3_DATA_STRENGTH       1
    #define PA3_DATA_OUT            1
    #define PULL_WAKEUP_SRC_PA3     PM_PIN_PULLUP_10K

    #define PIN_I2C_SCL         	GPIO_PA3
    #define PA4_FUNC                AS_GPIO
    #define PA4_INPUT_ENABLE        0
    #define PA4_OUTPUT_ENABLE       1
    #define PA4_DATA_STRENGTH       1
    #define PA4_DATA_OUT            1
    #define PULL_WAKEUP_SRC_PA4     PM_PIN_PULLUP_10K
#endif  //MFI_ENABLE

#define FLASH_FLAG_VALID            	0x3132

// === HAP AUTH CHIP VERSION ===
#define MFI_AUTH_2_0_C						1	//MFI2.0C
#define MFI_AUTH_3_0						2	//MFI3.0

#if (MFI_ENABLE == 0x22)
#define MFI_AUTH_CHIP_VER					MFI_AUTH_2_0_C
#elif (MFI_ENABLE == 0x20)
#define MFI_AUTH_CHIP_VER					MFI_AUTH_3_0
#else
#define MFI_AUTH_CHIP_VER					0
#endif

#ifndef SOFTWARE_AUTHENTICATION_ENABLE
	#define SOFTWARE_AUTHENTICATION_ENABLE		0
#endif


/////////////////// BOARD PINs Define ////////////////////////////

#define BOARD_8267DONGLE_C1T82A3_V1_0   	1
#define BOARD_8267DONGLE_C1T82A3_V1_1   	2
#define BOARD_8267EVK_C1T80A30_V1_0     	3
#define BOARD_8267EVK_C1T80A30_V1_1     	4
#define BOARD_8269EVK_C1T80A30_V1_0			5	//only for 8269.
#define BOARD_8258EVK_C1T139A30_V1_2		6	//8258 EVK C1T139A30_V1.2.
#define BOARD_CUST_VERSION_1            	51
#define BOARD_CUST_VERSION_2            	52
#define BOARD_CUST_VERSION_3            	53
#define BOARD_CUST_VERSION_4            	54
#define BOARD_CUST_VERSION_5            	55
#define BOARD_CUST_VERSION_6            	56

//////////				 Select Support Board, here!	////////
#define BOARD_DEF       BOARD_8258EVK_C1T139A30_V1_2

// note !!!DEVICE_SHORT_NAME max 7 bytes for iOS9/iOS10
#define DEVICE_SHORT_NAME   	"R1x 0123456789"
#define DEVICE_FULL_NAME    	"R1x EVK SDK 20190719"

typedef enum
{
	ATT_H_START = 0,

	ATT_END_H,

}ATT_HANDLE;

enum{
    CHAR_SAVE_FLAG = 0x15,
};

/////////////////// set default   ////////////////

#include "../common/default_config.h"


/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
