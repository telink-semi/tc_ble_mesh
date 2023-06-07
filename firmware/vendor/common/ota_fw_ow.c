/********************************************************************************************************
 * @file	ota_fw_ow.c
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
#include "proj/mcu/watchdog_i.h"
#include "drivers/8258/flash.h"
#include "vendor/common/user_config.h"
#include "proj_lib/rf_drv.h"
#include "proj_lib/pm.h"
#include "proj_lib/ble/blt_config.h"

#if (FLASH_1M_ENABLE && PINGPONG_OTA_DISABLE && (0 == FW_START_BY_BOOTLOADER_EN))
static inline void ota_reboot(void){
#if 0
    static volatile u32 reboot_0key;
    while (!reboot_0key){
        static volatile u32 reboot_1;reboot_1++;
    };
#endif
	//REG_ADDR8(0x602) = 0x84;				//reboot, sometimes can not reboot from flash but RAM.
	REG_ADDR8(0x6f) = 0x20;  //reboot
	while (1);
}

_attribute_ram_code_ void ota_fw_check_over_write (void)    //must run in ramcode
{
	#if SWITCH_FW_ENABLE
	u32 flash_adr_new_fw = (*(u8 *)FLASH_ADR_BOOT_FLAG == 0xa5)?FLASH_ADR_LIGHT_TELINK_MESH:FLASH_ADR_UPDATE_NEW_FW;
	#else
	u32 flash_adr_new_fw = FLASH_ADR_UPDATE_NEW_FW;
	#endif
    u32 adr_flag = flash_adr_new_fw + 8;
    if(0x544c4e4b != *(u32 *)adr_flag){
        return ;
    }

	write_reg8(0x60, 0x00);
	write_reg8(0x61, 0x00);
	write_reg8(0x62, 0x00);
	write_reg8(0x63, 0xff);
	write_reg8(0x64, 0xff);
	write_reg8(0x65, 0xff);

	analog_write(0x82, 0x64);
	analog_write(0x34, 0x80);
	analog_write(0x0b, 0x38);

	analog_write(0x8c, 0x02);  //to enable external 24M crystal
	analog_write(0x02, 0xa2);
    REG_ADDR8(0x74f) = 0x01;        //enable system tick for flash API;

	//irq_disable ();
	
#if 1
    u8 buff[256];// = {0};
    int n;

	#if SWITCH_FW_ENABLE
	if(flash_adr_new_fw != FLASH_ADR_UPDATE_NEW_FW){//backup current running fw
		flash_read_page (0, 256, buff);
		n = *(u32 *)(buff + 0x18);
		
		if(n <= FLASH_ADR_AREA_FIRMWARE_END){
			
	        for (int i=0; i<n; i+=256)
	        {
	            flash_read_page (i, 256, buff);
	            flash_write_page (FLASH_ADR_UPDATE_NEW_FW+i, 256, buff);
	        }
		}
	}
    #endif
	
    flash_read_page (flash_adr_new_fw, 256, buff);
    n = *(u32 *)(buff + 0x18);
    if(n <= FLASH_ADR_UPDATE_NEW_FW){
        for (int i=0; i<n; i+=256)
        {
            if ((i & 0xfff) == 0)
            {
                flash_erase_sector (i);
            }

            flash_read_page (flash_adr_new_fw + i, 256, buff);
            flash_write_page (i, 256, buff);
        }
    }

    buff[0] = 0;
    flash_write_page (adr_flag, 1, buff);   //clear OTA flag
    #if SWITCH_FW_ENABLE
	if(*(u8 *)FLASH_ADR_BOOT_FLAG != 0xff){
    	flash_erase_sector(FLASH_ADR_BOOT_FLAG);//clear boot flag
	}
    #endif
#endif

    ota_reboot();
}
#endif

