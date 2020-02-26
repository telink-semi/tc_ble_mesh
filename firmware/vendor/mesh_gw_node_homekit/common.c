/********************************************************************************************************
 * @file     common.c
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2016, Telink Semiconductor (Shanghai) Co., Ltd.
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
#define _COMMON_C_


/* ------------------------------------
    Header Files
   ------------------------------------ */

#include "common.h"
#include "proj_lib/ble/blt_config.h"

#define 		FLASH_ADR_DC                	CUST_CAP_INFO_ADDR  // 0xFE000
#define 		FLASH_ADR_MAC               	CFG_ADR_MAC         // 0xFF000

/* ------------------------------------
    Macro Definitions
   ------------------------------------ */


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions
   ------------------------------------ */
flash_adr_layout_def   flash_adr_layout;

u8 led_val[6] = {0};
u8 light_onoff_status = 1;
_attribute_data_retention_ extern volatile	u32 flash_char_value_addr;

/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
void COMMON_Init (void)
{

	flash_adr_layout.flash_adr_char_value				= (volatile u8 *)(FLASH_ADR_CHAR_VALUE);
	flash_adr_layout.flash_adr_sw_uuid					= (volatile u8 *)(FLASH_ADR_SW_UUID);
	flash_adr_layout.flash_adr_sw_token 				= (volatile u8 *)(FLASH_ADR_SW_TOKEN);
	flash_adr_layout.flash_adr_device_id				= (volatile u8 *)(FLASH_ADR_DEVICE_ID);
	flash_adr_layout.flash_adr_hash_id					= (volatile u8 *)(FLASH_ADR_HASH_ID);
	flash_adr_layout.flash_adr_setupcode				= (volatile u8 *)(FLASH_ADR_SETUPCODE);
	flash_adr_layout.flash_adr_setupcode_write_cnt		= (volatile u8 *)(FLASH_ADR_SETUPCODE_WRITE_CNT); 
	flash_adr_layout.flash_adr_setupcode_read_cnt		= (volatile u8 *)(FLASH_ADR_SETUPCODE_READ_CNT);
	flash_adr_layout.flash_adr_setup_id 				= (volatile u8 *)(FLASH_ADR_SETUP_ID);
	flash_adr_layout.flash_adr_serial_number			= (volatile u8 *)(FLASH_ADR_SERIAL_NUMBER);
	flash_adr_layout.flash_adr_accessory_ltsk			= (volatile u8 *)(FLASH_ADR_ACCESSORY_LTSK);
	flash_adr_layout.flash_adr_srp_key					= (volatile u8 *)(FLASH_ADR_SRP_KEY);
	flash_adr_layout.flash_adr_srp_key_crc_data 		= (volatile u8 *)(FLASH_ADR_SRP_KEY_CRC_DATA);
	flash_adr_layout.flash_adr_id_info					= (volatile u8 *)(FLASH_ADR_ID_INFO);
	flash_adr_layout.flash_adr_factory_reset_cnt		= (volatile u8 *)(FLASH_ADR_FACTORY_RESET_CNT);
	flash_adr_layout.flash_adr_global_state 			= (volatile u8 *)(FLASH_ADR_GLOBAL_STATE);
	flash_adr_layout.flash_adr_mac						= (volatile u8 *)(FLASH_ADR_MAC);
	flash_adr_layout.flash_adr_dc						= (volatile u8 *)(FLASH_ADR_DC);
//	flash_adr_layout.flash_adr_tp_low					= (volatile u8 *)(FLASH_ADR_TP_LOW);
//	flash_adr_layout.flash_adr_tp_high					= (volatile u8 *)(FLASH_ADR_TP_HIGH);
	//flash_adr_layout.flash_adr_service					= (volatile u8 *)(FLASH_ADR_SERVICE);
	flash_adr_layout.flash_adr_fw_update_cnt			= (volatile u8 *)(FLASH_ADR_FW_UPDATE_CNT);
	flash_adr_layout.flash_adr_adv_encryption_key		= (volatile u8 *)(FLASH_ADR_ADV_ENCRYPTION_KEY);
	flash_adr_layout.flash_adr_new_id					= (volatile u8 *)(FLASH_ADR_NEW_ID);
	flash_adr_layout.flash_adr_broadcast_gsn			= (volatile u8 *)(FLASH_ADR_BROADCAST_GSN);
	flash_adr_layout.flash_adr_srp_key2					= (volatile u8 *)(FLASH_ADR_SRP_KEY2);
	flash_adr_layout.flash_adr_srp_key2_crc_data		= (volatile u8 *)(FLASH_ADR_SRP_KEY2_CRC_DATA);
	flash_adr_layout.flash_adr_saltchar					= (volatile u8 *)(FLASH_ADR_SALTCHAR);

	flash_char_value_addr = (u32)flash_adr_layout.flash_adr_char_value;

}
#if 0
void pwm_set_lum(int id, u8 y, int pol) {
    int lum = (y) * PMW_MAX_TICK_MULTI;
    pwm_set_cmp(id, pol ? PMW_MAX_TICK - lum : lum);
}
#endif
void pwm_light_enable(u8 on) {
    if (on) {
        light_onoff_status = 1;
        pwm_set_lum(PWMID_R, led_val[0], 0);
        pwm_set_lum(PWMID_G, led_val[1], 0);
        pwm_set_lum(PWMID_B, led_val[2], 0);
        pwm_set_lum(PWMID_W, led_val[3], 0);
    } else {
        light_onoff_status = 0;
        pwm_set_lum(PWMID_R, 0, 0);
        pwm_set_lum(PWMID_G, 0, 0);
        pwm_set_lum(PWMID_B, 0, 0);
        pwm_set_lum(PWMID_W, 0, 0);
    }
}

void rf_led_ota_ok(void) {
    gpio_set_func(OTA_LED, AS_GPIO);
    gpio_set_output_en(OTA_LED, 1);
    static u8 led_onoff = 0;
    foreach(i, 6)
    {
    	if(blc_ll_getCurrentState() == BLS_LINK_STATE_ADV)
    	{
    		irq_disable ();
    	}
        gpio_write(OTA_LED, led_onoff);
        led_onoff = !led_onoff;
        sleep_us(1000 * 1000);
    }
}

void rf_led_ota_error(void) {
    gpio_set_func(OTA_LED, AS_GPIO);
    gpio_set_output_en(OTA_LED, 1);
    static u8 led_onoff = 0;
    foreach(i, 60)
    {
    	if(blc_ll_getCurrentState() == BLS_LINK_STATE_ADV)
    	{
    		irq_disable ();
    	}
        gpio_write(OTA_LED, led_onoff);
        led_onoff = !led_onoff;
        sleep_us(100 * 1000);
    }
}


/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
