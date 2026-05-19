/********************************************************************************************************
 * @file	ext_ble_common.c
 *
 * @brief	for TLSR chips
 *
 * @author	BLE GROUP
 * @date	2020.06
 *
 * @par     Copyright (c) 2020, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#include "ext_ble.h"
#include "stack/ble/ble.h"
#include "vendor/common/mesh_ota.h"

// pkt_Adv is used as parameter of app_advertise_prepare_handler(p)
_attribute_ble_data_retention_
rf_packet_adv_t pkt_Adv;

void set_adv_addr(u8 adv_handle, u8 *addr, u8 addr_type)
{
    u8 cmd_para[1 + BLE_ADDR_LEN];
    cmd_para[0] = addr_type;
    memcpy(cmd_para + 1, addr, BLE_ADDR_LEN);
    blt_set_adv_addrtype(cmd_para);
}

/**
 * @brief       This function serves to reboot chip.
 * @return      
 * @note 		To maintain IO high level during reboot, use a pull-up resistor. 
 *				Note that start_reboot_by_reg() does not preserve internal pull-up (set by gpio_setup_up_down_resistor()) during reboot. 
 *				If no external pull-up is present, use cpu_sleep_wakeup() instead in start_reboot().
 */
_attribute_ram_code_ void start_reboot(void)
{
#if SLEEP_FUNCTION_DISABLE
	start_reboot_by_reg();  // start reboot by REG_ADDR8(0x6f) = 0x20, can save ram.
#else
    #if (MCU_CORE_TYPE != MCU_CORE_TC321X)
	if(blt_miscParam.pad32k_en){
		analog_write(SYS_DEEP_ANA_REG, analog_read(SYS_DEEP_ANA_REG) & (~SYS_NEED_REINIT_EXT32K)); //clear
	}
    #endif

	irq_disable ();
	cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_TIMER, clock_time() + 4*1000*sys_tick_per_us);//reboot, sleep time need greater than EARLYWAKEUP_TIME_MS_DEEP
#endif
	while (1);
}

/**
 * @brief       This function server to get cpu wakeup source
 * @return      CPU_WATCHDOG_RESET: watchdog reset. 
 *				CPU_PAD_WAKEUP: gpio wakeup. 
 *				CPU_TIMER_WAKEUP: timer wakeup.
 *				CPU_POWER_RESET: power reset.
 * @note        calling this function must be after "cpu_wakeup init()" because can not read anlog,
                and must before wakeup io setting(if exist).
 */
int get_cpu_wakeup_source(void)
{
	if(read_reg8(0x72) & BIT(0)){
		write_reg8(0x72, BIT(0)); // manual clear watchdog reset flag after read.
		return CPU_WATCHDOG_RESET;
	}

#if (MCU_CORE_TYPE == MCU_CORE_TC321X)
    u8 val = analog_read(0x64);
#else
	u8 val = analog_read(0x44);
#endif

	if((val & WAKEUP_STATUS_TIMER_PAD ) == WAKEUP_STATUS_PAD){
		return CPU_PAD_WAKEUP;
	}
	else if(val & WAKEUP_STATUS_TIMER){
		return CPU_TIMER_WAKEUP;
	}

    if(is_state_after_ota()){
        #if MD_SERVER_EN
        // if MD_SERVER_EN 1, clr_keep_onoff_state_after_ota() is called in light_pwm_init().
        #else
        clr_keep_onoff_state_after_ota();
        #endif
        return CPU_OTA_RESET;
    }

	return CPU_POWER_RESET;
}

/**
 * @brief       This is the otaWrite() callback function to check OTA signature
 * @param[io]   p	- pointer of att data
 * @return      1: auth success    0: auth fail
 * @note        
 */
int ota_write_auth_callback(void *p)
{
    __UNUSED rf_packet_att_data_t *pAttDat = (rf_packet_att_data_t*)p;
    
    #if DUAL_OTA_NEED_LOGIN_EN
    u16 ota_adr =  pAttDat->dat[0] | (pAttDat->dat[1]<<8);
    if(ota_adr == CMD_OTA_START){
        cmd_ota_mesh_hk_login_handle(pAttDat->dat+2);
    }
    #endif

    return ota_condition_enable();
}

u8 fw_ota_value =0;
u8 get_fw_ota_value(void)
{
	return fw_ota_value;
}

/**
 * @brief       This is callback function for check ota data after crc16() in otaWrite()
 * @param[in]   data	- ota data bin file
 * @param[in]   flash_addr	- offset in bin file
 * @param[in]   len	- length of ota data
 * @return      OTA_SUCCESS: success    other: failure
 * @note        
 */
int ota_write_data_callback(u32 flash_addr, int len, u8 * data)
{
    int err_flg = OTA_SUCCESS;

    #if ENCODE_OTA_BIN_EN
    if(16 == len){
	    u8 key[16];
	    memcpy(key, key_encode_bin, sizeof(key));
	    aes_decrypt(key, data, data);
    }
    #endif

    if(flash_addr == 0){
        fw_ota_value = data[8];
        if(0 == ota_is_valid_pid_vid((fw_id_t *)(data+2), 1)){
        	#if GATEWAY_ENABLE
            if(0 == ota_reboot_flag){
				// means gateway is receiving firmware for other nodes, rather than gateway self.
        	}else
			#endif
			{
            	err_flg = OTA_FW_CHECK_ERR;
        	}
        }
        else{            
            #if ENCODE_OTA_BIN_EN
            u8 need_check_type = 0;
            if(data[6] == 0x5D){
    	    	need_check_type = data[7] ;
    	    }
            
    	    if(need_check_type != FW_CHECK_AGTHM2){
    	        err_flg = OTA_FW_CHECK_ERR;
    	    }
    	    #endif
        }
    }

    return err_flg;
}

/**
 * @brief       This is callback function in ota result
 * @param[io]   result	- 
 * @return      none
 * @note        
 */
void ota_set_result_callback(int result)
{
    #if GATEWAY_ENABLE
	gateway_upload_gatt_ota_sts(result);
    if(0 == ota_reboot_flag){
        blt_ota_reset(); // ota_reboot_flag == 0 means receive firmware not for gateway self, clear ota state to avoid setting boot flag and reboot
    }
	#endif    
}

/**
 * @brief       This function server to init ota callback function for mesh
 * @param[io]   void- 
 * @return      none
 * @note        
 */
void blc_ota_write_cb_init(void)
{
    blc_ota_register_ota_auth_cb(ota_write_auth_callback);
    blc_ota_register_ota_data_cb(ota_write_data_callback);
    blc_ota_register_ota_result_cb(ota_set_result_callback);
}

STATIC_ASSERT(BOOT_MARK_ADDR == 8); // for B85m.

int ota_set_flag(void)
{
    if(0 == is_valid_startup_flag(ota_program_offset + BOOT_MARK_ADDR, 0)){
        return -1; // invalid flag for the new firmware
    }

	u32 flag = 0x4b;
	flash_write_page(ota_program_offset + 8, 1, (u8 *)&flag);		//Set FW flag
	#if (PINGPONG_OTA_DISABLE || FW_START_BY_LEGACY_BOOTLOADER_EN)
	// not set invalid, because it may need to recover to old version when signature failed
	#else
	flag = 0;
	flash_write_page((ota_program_offset ? 0 : ota_program_bootAddr) + 8, 4, (u8 *)&flag);	//Invalid flag
	#endif

	return 0;
}

/**
 * @brief       This function server to protect firmware area.
 * @param[in]   addr- start address of the area.
 * @param[in]   len	- the length(in byte) of content needs to write into the flash.
 * @return      1: addr is valid. 0: addr is invalid.
 * @note        
 */
int is_valid_sector_addr(unsigned long addr, unsigned long len) // BLE_SRC_TELINK_MESH_EN
{
#if FW_LOAD_BY_BOOTLOADER_AND_NO_OTA_EN
    return 1;
#elif (FW_START_BY_LEGACY_BOOTLOADER_EN == 0)
	#if ((MCU_CORE_TYPE == MCU_CORE_825x) || (MCU_CORE_TYPE == MCU_CORE_827x) || (MCU_CORE_TYPE == MCU_CORE_TC321X))
        #if (PINGPONG_OTA_DISABLE)
    extern u8 ota_pingpong_disable_copy_fw_flag;
    if(ota_pingpong_disable_copy_fw_flag){
        return 1;
    }
    
	u32 fw_boot_addr = 0; // always 0
 		#elif (MCU_CORE_TYPE == MCU_CORE_825x)
	u32 fw_boot_addr = ((REG_ADDR8(0x63e) & 0x03) << 7) << 10; // offset unit:128k
		#elif(MCU_CORE_TYPE == MCU_CORE_827x)
	u32 fw_boot_addr = ((REG_ADDR8(0x63e) & 0x07) << 7) << 10;
    	#elif(MCU_CORE_TYPE == MCU_CORE_TC321X)
	u32 fw_boot_addr = ((REG_ADDR8(0x63e) & 0x0F) << 7) << 10;
		#endif

#if(MCU_CORE_TYPE == MCU_CORE_TC321X)
#define FLASH_MAX_SIZE      (2 * 1024 * 1024) // max 2M for MCU_CORE_TC321X mesh now
#else
#define FLASH_MAX_SIZE      (1 * 1024 * 1024) // max 1M for B85m mesh now
#endif

    u32 fl_max_size = min((1 << blc_flash_capacity), FLASH_MAX_SIZE); // The reason for using the FLASH_MAX_SIZE constant value for restriction is that blc_flash_capacity may be unexpectedly changed when the power supply is unstable.

	if(((addr >= fw_boot_addr + (FW_SIZE_MAX_K << 10)) && (addr + len <= fl_max_size)) // not use flash_capacity, because it may be changed unexpected.
	|| ((addr + len <= fw_boot_addr)/* && (addr < fw_boot_addr)*/) // implied (fw_boot_addr != 0)
	|| ((addr == (fw_boot_addr + BOOT_MARK_ADDR)) && (len <= 4))){ // for write OTA flag
		return 1;
	}

	return 0;
	#endif
 #endif

	return 1;
}

