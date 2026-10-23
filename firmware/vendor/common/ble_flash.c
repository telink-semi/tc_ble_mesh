/********************************************************************************************************
 * @file    ble_flash.c
 *
 * @brief   This is the source file for BLE SDK
 *
 * @author  BLE GROUP
 * @date    06,2020
 *
 * @par     Copyright (c) 2020, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#include "drivers.h"
#include "ble_flash.h"
#include "stack/ble/ble.h"


/* global variable for some flash sector address, initial value use 512K flash.
 * attention that they will be changed in function "blc readFlashSize_autoConfigCustomFlashSector"  */
_attribute_data_retention_	unsigned int flash_sector_mac_address = CFG_SECTOR_ADR_MAC_CODE; // BLE_SRC_TELINK_MESH_EN
_attribute_data_retention_	unsigned int flash_sector_calibration = CFG_SECTOR_ADR_CALIBRATION_CODE; // BLE_SRC_TELINK_MESH_EN
#if BLE_REMOTE_SECURITY_ENABLE
_attribute_data_retention_	unsigned int flash_sector_smp_storage = FLASH_ADR_SMP_PARA_START; //for slave only // BLE_SRC_TELINK_MESH_EN
#endif
#if BLE_HOST_SMP_ENABLE
_attribute_data_retention_	unsigned int flash_sector_master_pairing = FLASH_ADR_MASTER_PAIRING_512K; //for master only
#endif
//_attribute_data_retention_	unsigned int  blc_flash_mid = 0; // BLE_SRC_TELINK_MESH_EN
_attribute_data_retention_	unsigned int  blc_flash_vendor = 0;
//_attribute_data_retention_	unsigned char blc_flash_capacity; // BLE_SRC_TELINK_MESH_EN


/**
 * @brief   BLE parameters on flash
 */
typedef struct{
	u8		cap_frqoffset_en;
	u8		cap_frqoffset_value;

	u8		vddf_calib_en;
	u8		vddf_calib_value0;
	u8		vddf_calib_value1;
}nv_para_t;

_attribute_data_retention_ 	nv_para_t 				blc_nvParam;


/**
 * @brief		This function is used to read flash mid, get flash vendor, and set flash capacity
 * @param[in]	none
 * @return      none
 */
void blc_flash_read_mid_get_vendor_set_capacity(void)
{
	/*
		Flash Type	uid CMD			MID		Company
		ZB25WD40B	0x4b		0x13325E	ZB
		ZB25WD80B	0x4b		0x14325E	ZB
		GD25LD40C	0x4b		0x1360C8	GD
		GD25LD40E	0x4b		0x1360C8	GD
		GD25LD80C	0x4b(AN)	0x1460C8	GD
		GD25LD80E	0x4b		0x1460C8	GD
	 */
	/* attention: blc_flash_mid/blc_flash_vendor/blc_flash_capacity will be used by application and stack later
	 * so do not change code here */
	blc_flash_mid = flash_read_mid();
	blc_flash_vendor = flash_get_vendor(blc_flash_mid);
	blc_flash_capacity = ((blc_flash_mid & 0x00ff0000)>>16);

}


/**
 * @brief		This function can automatically recognize the flash size,
 * 				and the system selects different customized sector according
 * 				to different sizes.
 * @param[in]	none
 * @return      none
 */
void blc_readFlashSize_autoConfigCustomFlashSector(void)
{
	blc_flash_read_mid_get_vendor_set_capacity();

#if AUTO_ADAPT_MAC_ADDR_TO_FLASH_TYPE_EN
#if (FLASH_ZB25WD40B_SUPPORT_EN || FLASH_GD25LD40C_SUPPORT_EN || FLASH_GD25LD40E_SUPPORT_EN	|| FLASH_P25D40SU_SUPPORT_EN || FLASH_ZB25WD40C_SUPPORT_EN)	//512K
	if(blc_flash_capacity == FLASH_SIZE_512K){
		flash_sector_mac_address = CFG_ADR_MAC_512K_FLASH;
		flash_sector_calibration = CFG_ADR_CALIBRATION_512K_FLASH;
        #if BLE_REMOTE_SECURITY_ENABLE
		flash_sector_smp_storage = FLASH_ADR_SMP_PAIRING_512K_FLASH;
            #if BLE_HOST_SMP_ENABLE
		flash_sector_master_pairing = FLASH_ADR_MASTER_PAIRING_512K;
            #endif
        #endif
		tlkapi_printf(APP_FLASH_INIT_LOG_EN, "[FLASH][INI] 512K Flash, MAC on 0x%x\n", flash_sector_mac_address);
	}
#endif
#if (FLASH_ZB25WD80B_SUPPORT_EN || FLASH_GD25LD80C_SUPPORT_EN || FLASH_GD25LD80E_SUPPORT_EN || FLASH_P25Q80U_SUPPORT_EN || \
	FLASH_P25Q80SU_SUPPORT_EN || FLASH_TH25Q80U_SUPPORT_EN || FLASH_ZB25WD80C_SUPPORT_EN)		//1M
	else if(blc_flash_capacity == FLASH_SIZE_1M){
		flash_sector_mac_address = CFG_ADR_MAC_1M_FLASH;
		flash_sector_calibration = CFG_ADR_CALIBRATION_1M_FLASH;
        #if BLE_REMOTE_SECURITY_ENABLE
		flash_sector_smp_storage = FLASH_ADR_SMP_PAIRING_1M_FLASH;
            #if BLE_HOST_SMP_ENABLE
		flash_sector_master_pairing = FLASH_ADR_MASTER_PAIRING_1M;
            #endif
        #endif
		tlkapi_printf(APP_FLASH_INIT_LOG_EN, "[FLASH][INI] 1M Flash, MAC on 0x%x\n", flash_sector_mac_address);
	}
#endif
#if (FLASH_P25Q16SU_SUPPORT_EN)		//2M
	else if(blc_flash_capacity == FLASH_SIZE_2M){
		flash_sector_mac_address = CFG_ADR_MAC_2M_FLASH;
		flash_sector_calibration = CFG_ADR_CALIBRATION_2M_FLASH;
        #if BLE_REMOTE_SECURITY_ENABLE
		flash_sector_smp_storage = FLASH_ADR_SMP_PAIRING_2M_FLASH;
            #if BLE_HOST_SMP_ENABLE
		flash_sector_master_pairing = FLASH_ADR_MASTER_PAIRING_2M;
            #endif
        #endif
		tlkapi_printf(APP_FLASH_INIT_LOG_EN, "[FLASH][INI] 2M Flash, MAC on 0x%x\n", flash_sector_mac_address);
	}
#endif
	else{
		/*This SDK do not support other flash size except what listed above
		  If code stop here, please check your Flash */
		tlkapi_printf(APP_FLASH_INIT_LOG_EN, "[FLASH][INI] flash size %x do not support !!!\n", blc_flash_capacity);
		while(1);
	}

    if(CFG_ADR_MAC_512K_FLASH == CFG_SECTOR_ADR_MAC_CODE){ // BLE_SRC_TELINK_MESH_EN
        if(blc_flash_capacity == FLASH_SIZE_1M){
            #define MAC_SIZE_CHECK      (6)
            u8 mac_null[MAC_SIZE_CHECK] = {0xff,0xff,0xff,0xff,0xff,0xff};
            u8 mac_512[MAC_SIZE_CHECK], mac_1M[MAC_SIZE_CHECK];
            flash_read_page(CFG_ADR_MAC_512K_FLASH, MAC_SIZE_CHECK, mac_512);
            flash_read_page(CFG_ADR_MAC_1M_FLASH, MAC_SIZE_CHECK, mac_1M);
            if((0 == memcmp(mac_512,mac_null, MAC_SIZE_CHECK))
             &&(0 != memcmp(mac_1M,mac_null, MAC_SIZE_CHECK))){
                flash_sector_mac_address = CFG_ADR_MAC_1M_FLASH;
                flash_sector_calibration = CFG_ADR_CALIBRATION_1M_FLASH;
            }
            else{
                flash_sector_mac_address = CFG_ADR_MAC_512K_FLASH;
                flash_sector_calibration = CFG_ADR_CALIBRATION_512K_FLASH;
            }
        }
    }else if(CFG_ADR_MAC_1M_FLASH == CFG_SECTOR_ADR_MAC_CODE){
        if(blc_flash_capacity != FLASH_SIZE_1M){
            while(1){ // please check your Flash size
                #if(MODULE_WATCHDOG_ENABLE)
                wd_clear();
                #endif
            }
        }
    }
#endif
}




/*
 *Kite: 	VVWWXX38C1A4YYZZ
 *Vulture:  VVWWXXD119C4YYZZ
 * public_mac:
 * 				Kite 	: VVWWXX 38C1A4
* 				Vulture : VVWWXX D119C4
 * random_static_mac: VVWWXXYYZZ C0
 */
/**
 * @brief		This function is used to initialize the MAC address
 * @param[in]	flash_addr - flash address for MAC address
 * @param[in]	mac_public - public address
 * @param[in]	mac_random_static - random static MAC address
 * @return      none
 */
void blc_initMacAddress(int flash_addr, u8 *mac_public, u8 *mac_random_static)
{
	if(flash_sector_mac_address == 0){
		return;
	}


	u8 mac_read[8];
	flash_read_page(flash_addr, 8, mac_read);

	u8 value_rand[5];
	generateRandomNum(5, value_rand);

	u8 ff_six_byte[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	if ( memcmp(mac_read, ff_six_byte, 6) ) {
		memcpy(mac_public, mac_read, 6);  //copy public address from flash
	}
	else{  //no public address on flash
		mac_public[0] = value_rand[0];
		mac_public[1] = value_rand[1];
		mac_public[2] = value_rand[2];

		#if(MCU_CORE_TYPE == MCU_CORE_825x)
			mac_public[3] = 0x38;             //company id: 0xA4C138
			mac_public[4] = 0xC1;
			mac_public[5] = 0xA4;
		#elif(MCU_CORE_TYPE == MCU_CORE_827x)
			mac_public[3] = 0xD1;             //company id: 0xC119D1
			mac_public[4] = 0x19;
			mac_public[5] = 0xC4;
		#elif(MCU_CORE_TYPE == MCU_CORE_TC321X)
			mac_public[3] = 0x4A;             //company id: 0xD0AB4A
			mac_public[4] = 0xAB;
			mac_public[5] = 0xD0;
		#endif


		flash_write_page (flash_addr, 6, mac_public);
	}





	mac_random_static[0] = mac_public[0];
	mac_random_static[1] = mac_public[1];
	mac_random_static[2] = mac_public[2];
	mac_random_static[5] = 0xC0; 			//for random static

	u16 high_2_byte = (mac_read[6] | mac_read[7]<<8);
	if(high_2_byte != 0xFFFF){
		memcpy( (u8 *)(mac_random_static + 3), (u8 *)(mac_read + 6), 2);
	}
	else{
		mac_random_static[3] = value_rand[3];
		mac_random_static[4] = value_rand[4];

		flash_write_page (flash_addr + 6, 2, (u8 *)(mac_random_static + 3) );
	}
}

/**
 * @brief		load customized parameters (from Flash) when MCU power on or wake_up from deepSleep mode
 * @param[in]	none
 * @return      none
 */
void blc_app_loadCustomizedParameters_normal(void)
{
	/* read and set RF frequency offset capacitance calibration value from Flash */
	if(!blt_miscParam.ext_cap_en){
		if(flash_sector_calibration){
			u8 cap_frqoft = *(unsigned char*) (flash_sector_calibration + CALIB_OFFSET_CAP_INFO);
			if( cap_frqoft != 0xff ){
				blc_nvParam.cap_frqoffset_en = 1;
				blc_nvParam.cap_frqoffset_value = cap_frqoft;
				rf_update_internal_cap(blc_nvParam.cap_frqoffset_value);
			}
		}
	}

	/* read and set VDD_F calibration value from Flash */
	unsigned char calib_value[2] = {0};
	flash_read_page(flash_sector_calibration + CALIB_OFFSET_FLASH_VREF, 2, calib_value);
	if(user_calib_vdd_f(calib_value)){ //set calibration value success
		/* attention that "calib_value" may be changed in "user calib_vdd_f", and that's what we want */
		blc_nvParam.vddf_calib_en = 1;
		blc_nvParam.vddf_calib_value0 = calib_value[0];
		#if(MCU_CORE_TYPE == MCU_CORE_827x)
			blc_nvParam.vddf_calib_value1 = calib_value[1];
		#endif
	}

	/* read and set ADC V_reference calibration value from Flash */
	#if (MCU_CORE_TYPE == MCU_CORE_825x || MCU_CORE_TYPE == MCU_CORE_827x)
		unsigned char adc_vref_calib_value_rd[7] = {0};
		flash_read_page(flash_sector_calibration + CALIB_OFFSET_ADC_VREF, 7, adc_vref_calib_value_rd);
		user_calib_adc_vref(adc_vref_calib_value_rd);
	#elif (MCU_CORE_TYPE == MCU_CORE_TC321X)
		sd_adc_calib_t sd_adc_calib_value;
		flash_read_page(flash_sector_calibration + CALIB_OFFSET_ADC_VREF, 12, (unsigned char*)&sd_adc_calib_value);
		user_calib_adc_vref(sd_adc_calib_value);
	#endif
}

/**
 * @brief		load customized parameters (from SRAM) when MCU wake_up from deepSleep_retention mode
 * @param[in]	none
 * @return      none
 */
_attribute_ram_code_
void blc_app_loadCustomizedParameters_deepRetn(void)
{
	/* set RF frequency offset capacitance calibration value with stored variables */
	if(blc_nvParam.cap_frqoffset_en) {
		rf_update_internal_cap(blc_nvParam.cap_frqoffset_value);
	}

	/* set VDD_F calibration value with stored variables*/
	if(blc_nvParam.vddf_calib_en) {
		#if(MCU_CORE_TYPE == MCU_CORE_825x)
			flash_set_vdd_f(blc_nvParam.vddf_calib_value0);
		#elif(MCU_CORE_TYPE == MCU_CORE_827x)
			flash_set_vdd_f(blc_nvParam.vddf_calib_value1, blc_nvParam.vddf_calib_value0);
		#endif
	}


	/* ADC V_reference calibration value is stored in variables on retention area, so no need set again */
}

