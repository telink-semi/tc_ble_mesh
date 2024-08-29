/********************************************************************************************************
 * @file    flash_prot.c
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
#include "proj_lib/sig_mesh/app_mesh.h"

#include "flash_prot.h"


_attribute_ble_data_retention_	_attribute_aligned_(4)	flash_prot_t	blc_flashProt;

/* this pointer is used only by stack, when OTA write or erase flash, will give a callback to application layer */
_attribute_ble_data_retention_	_attribute_aligned_(4)	flash_prot_op_callback_t 		flash_prot_op_cb = NULL;

_attribute_data_retention_	unsigned int  blc_flash_mid = 0;
_attribute_data_retention_	unsigned char blc_flash_capacity;


#if (APP_FLASH_PROTECTION_ENABLE)

_attribute_ble_data_retention_	_attribute_aligned_(4)	flash_lock_t 				flash_lock_mid = NULL;
_attribute_ble_data_retention_	_attribute_aligned_(4)	flash_unlock_t 				flash_unlock_mid = NULL;
_attribute_ble_data_retention_	_attribute_aligned_(4)	flash_get_lock_status_t 	flash_get_lock_status_mid = NULL;


_attribute_ble_data_retention_	u16	flash_unlock_status; //record flash unlock status for different flash type


/**
 * @brief      this function is used to register callback for stack, when OTA write or erase flash, send event to application layer
 * @param[in]  cb - flash protection operation callback function
 * @return     none
 */
void blc_appRegisterStackFlashOperationCallback(flash_prot_op_callback_t cb)
{
	flash_prot_op_cb = cb;
}


#if 0 // (!MCU_SUPPORT_MULTI_PRIORITY_IRQ)
/**
 * @brief 		this function is used to replace Flash driver API "flash_write_status", solving BLE connection issue on MCU do not support multiple priority IRQ
 * @param[in]  	type	- the type of status.8 bit or 16 bit.
 * @param[in]  	data	- the value of status.
 * @return 		none.
 */
void flash_write_status(flash_status_typedef_e type , unsigned short data)
{
	/* If MCU do not support multiple priority IRQ, Flash write status duration influencing BLE RF IRQ, then lead to BLE data error
	 * use  "blc ll_write_flash_status" to solve the issue, SDK internal will find idle timing to write safely. */
	blc_ll_write_flash_status(type, data);
}
#endif


/**
 * @brief      this function is used to initialize flash protection.Block size of lock is a sample, user can change it according to bin size.
 * 				 This function only shows the flash supported by TELINK testing. If user use other flash, please follow the code below.
 * @param[in]  none
 * @return     none
 */
void flash_protection_init(void)
{
	if(!blc_flash_mid){
		blc_flash_mid = flash_read_mid();
		blc_flash_capacity = ((blc_flash_mid & 0x00ff0000)>>16);
        LOG_FLASH_PROT_DEBUG(0, 0, "[FLASH] flash id: 0x%x", blc_flash_mid);
	}

	/* According to the flash mid, execute the corresponding lock flash API. */
	switch(blc_flash_mid)
	{
	#if (0 == __TLSR_RISCV_EN__)
		#if (FLASH_ZB25WD40B_SUPPORT_EN) // 512K capacity
			case MID13325E:
				flash_lock_mid = (flash_lock_t)flash_lock_mid13325e;
				flash_unlock_mid = flash_unlock_mid13325e;
				flash_get_lock_status_mid = (flash_get_lock_status_t)flash_get_lock_block_mid13325e;
				flash_unlock_status = FLASH_LOCK_NONE_MID13325E;
				break;
		#endif

		#if (FLASH_ZB25WD80B_SUPPORT_EN) // 1M capacity
			case MID14325E:
				flash_lock_mid = (flash_lock_t)flash_lock_mid14325e;
				flash_unlock_mid = flash_unlock_mid14325e;
				flash_get_lock_status_mid = (flash_get_lock_status_t)flash_get_lock_block_mid14325e;
				flash_unlock_status = FLASH_LOCK_NONE_MID14325E;
				break;
		#endif


		#if (FLASH_GD25LD40C_SUPPORT_EN || FLASH_GD25LD40E_SUPPORT_EN) // 512K capacity
			case MID1360C8:
				flash_lock_mid = (flash_lock_t)flash_lock_mid1360c8;
				flash_unlock_mid = flash_unlock_mid1360c8;
				flash_get_lock_status_mid = (flash_get_lock_status_t)flash_get_lock_block_mid1360c8;
				flash_unlock_status = FLASH_LOCK_NONE_MID1360C8;
				break;
		#endif

		#if(FLASH_P25Q80U_SUPPORT_EN) // 1M capacity
			case MID146085:
				flash_lock_mid = (flash_lock_t)flash_lock_mid146085;
				flash_unlock_mid = flash_unlock_mid146085;
				flash_get_lock_status_mid = (flash_get_lock_status_t)flash_get_lock_block_mid146085;
				flash_unlock_status = FLASH_LOCK_NONE_MID146085;
				break;
		#endif

		#if (FLASH_GD25LD80C_SUPPORT_EN || FLASH_GD25LD80E_SUPPORT_EN) // 1M capacity
			case MID1460C8:
				flash_lock_mid = (flash_lock_t)flash_lock_mid1460c8;
				flash_unlock_mid = flash_unlock_mid1460c8;
				flash_get_lock_status_mid = (flash_get_lock_status_t)flash_get_lock_block_mid1460c8;
				flash_unlock_status = FLASH_LOCK_NONE_MID1460C8;
				break;
		#endif
	#else
		#if(FLASH_P25Q80U_SUPPORT_EN) // 1M capacity
			case MID146085:
				flash_lock_mid = (flash_lock_t)flash_lock_mid146085;
				flash_unlock_mid = flash_unlock_mid146085;
				flash_get_lock_status_mid = (flash_get_lock_status_t)flash_get_lock_block_mid146085;
				flash_unlock_status = FLASH_LOCK_NONE_MID146085;
				break;
		#endif
		#if (FLASH_P25Q16SU_SUPPORT_EN) // 2M capacity
			case MID156085:
				flash_lock_mid = (flash_lock_t)flash_lock_mid156085;
				flash_unlock_mid = flash_unlock_mid156085;
				flash_get_lock_status_mid = (flash_get_lock_status_t)flash_get_lock_block_mid156085;
				flash_unlock_status = FLASH_LOCK_NONE_MID156085;
				break;
		#endif
		#if (FLASH_P25Q32SU_SUPPORT_EN) // 4M capacity
			case MID166085:
				flash_lock_mid = (flash_lock_t)flash_lock_mid166085;
				flash_unlock_mid = flash_unlock_mid166085;
				flash_get_lock_status_mid = (flash_get_lock_status_t)flash_get_lock_block_mid166085;
				flash_unlock_status = FLASH_LOCK_NONE_MID166085;
				break;
		#endif

		#if 1//(MCU_CORE_TYPE == MCU_CORE_B92) //B92
		#if (FLASH_P25Q128L_SUPPORT_EN) // 16M capacity
			case MID186085:
				flash_lock_mid = (flash_lock_t)flash_lock_mid186085;
				flash_unlock_mid = flash_unlock_mid186085;
				flash_get_lock_status_mid = (flash_get_lock_status_t)flash_get_lock_block_mid186085;
				flash_unlock_status = FLASH_LOCK_NONE_MID186085;
				break;
		#endif
		#if (FLASH_GD25LQ16E_SUPPORT_EN) // 2M capacity
			case MID1560c8:
				flash_lock_mid = (flash_lock_t)flash_lock_mid1560c8;
				flash_unlock_mid = flash_unlock_mid1560c8;
				flash_get_lock_status_mid = (flash_get_lock_status_t)flash_get_lock_block_mid1560c8;
				flash_unlock_status = FLASH_LOCK_NONE_MID1560c8;
				break;
		#endif
		#endif
	#endif

		default:
			blc_flashProt.init_err = 1; // must, if not, flash_lock() will reboot in unexpected if no match flash id. 
			LOG_FLASH_PROT_DEBUG(0, 0, "[FLASH] error, flash init failed, id: 0x%x", blc_flash_mid);
			break;
	}
}

#if 0
/**
 * @brief      this function is used to change application lock block definition to flash lock block definition according to flash type used
 * 			   attention: only processed a few lock block size for SDK demo, if you have more lock block size, please
 * 			              add more code by yourself
 * @param[in]  app_lock_block - application lock block definition
 * @return     flash_lock_block_size - The size of the lock block size of flash.
 */
u16 flash_change_app_lock_block_to_flash_lock_block(flash_app_lock_e app_lock_block)
{
	u32 flash_lock_block_size = 0;

	switch(blc_flash_mid)
	{
		#if (FLASH_ZB25WD40B_SUPPORT_EN) //512K capacity
			case MID13325E:
				if(app_lock_block == FLASH_LOCK_FW_LOW_256K){
					flash_lock_block_size = FLASH_LOCK_LOW_256K_MID13325E;
					LOG_FLASH_PROT_DEBUG(0, 0, "[FLASH][PROT] flash lock low 256K block!\n");
				}
				else if(app_lock_block == FLASH_LOCK_FW_LOW_512K){
					/* attention 1: use can change this value according to application
					 * attention 2: can not lock stack SMP data storage area
					 * attention 3: firmware size under protection is not 512K, user should calculate
					 * demo code: choose 448K, leave at 64K for system data(SMP storage data & calibration data & MAC address) and user data,
					 *            now firmware size under protection is 448K - 256K = 192K
					 * if this demo can not meet your requirement, you should change !!!*/
					flash_lock_block_size = FLASH_LOCK_LOW_448K_MID13325E;
					LOG_FLASH_PROT_DEBUG(0, 0, "[FLASH][PROT] flash lock low 448K block!\n");
				}
				else{
					blc_flashProt.init_err = 1; //can not use LOCK LOW 1M for 512K capacity flash
				}
				break;
		#endif

		#if (FLASH_ZB25WD80B_SUPPORT_EN) //1M capacity
			case MID14325E:
				if(app_lock_block == FLASH_LOCK_FW_LOW_256K || app_lock_block == FLASH_LOCK_FW_LOW_512K){
					/* attention that :This flash type, minimum lock size is 768K, do not support 256K or other value
					 * demo code will lock 768K when user set OTA 128K or 256K as multiple boot address,
					 * system data(SMP storage data & calibration data & MAC address) is OK;
					 * user data must be stored in flash address bigger than 768K !!!
					 * if this demo code lock area do not meet your requirement, you can change it !!!*/
					flash_lock_block_size = FLASH_LOCK_LOW_768K_MID14325E;
					LOG_FLASH_PROT_DEBUG(0, 0, "[FLASH][PROT] flash lock low 768K block!\n");
				}
				else if(app_lock_block == FLASH_LOCK_FW_LOW_1M){
					/* attention 1: use can change this value according to application
					 * attention 2: can not lock stack SMP data storage area
					 * attention 3: firmware size under protection is not 1M, user should calculate
					 * demo code: choose 960K, leave 64K for system data(SMP storage data & calibration data & MAC address) and user data,
					 * 			  now firmware size under protection is 960K - 512K = 448K
					 * if this demo can not meet your requirement, you should change !!! */
					flash_lock_block_size = FLASH_LOCK_LOW_960K_MID14325E;
					LOG_FLASH_PROT_DEBUG(0, 0, "[FLASH][PROT] flash lock low 960K block!\n");
				}
				break;
		#endif


		#if (FLASH_GD25LD40C_SUPPORT_EN || FLASH_GD25LD40E_SUPPORT_EN) //512K capacity
			case MID1360C8:
				if(app_lock_block == FLASH_LOCK_FW_LOW_256K){
					flash_lock_block_size = FLASH_LOCK_LOW_256K_MID1360C8;
					LOG_FLASH_PROT_DEBUG(0, 0, "[FLASH][PROT] flash lock low 256K block!\n");
				}
				else if(app_lock_block == FLASH_LOCK_FW_LOW_512K){
					/* attention 1: use can change this value according to application
					 * attention 2: can not lock stack SMP data storage area
					 * attention 3: firmware size under protection is not 512K, user should calculate
					 * demo code: choose 448K, leave at 64K for system data(SMP storage data & calibration data & MAC address) and user data,
					 *            now firmware size under protection is 448K - 256K = 192K
					 * if this demo can not meet your requirement, you should change !!!*/
					flash_lock_block_size = FLASH_LOCK_LOW_448K_MID1360C8;
					LOG_FLASH_PROT_DEBUG(0, 0, "[FLASH][PROT] flash lock low 448K block!\n");
				}
				else{
					blc_flashProt.init_err = 1; //can not use LOCK LOW 1M for 512K capacity flash
				}
				break;
		#endif

		#if(FLASH_P25Q80U_SUPPORT_EN)
			case MID146085:
				if(app_lock_block == FLASH_LOCK_FW_LOW_256K){
					flash_lock_block_size = FLASH_LOCK_LOW_256K_MID146085;
					LOG_FLASH_PROT_DEBUG(0, 0, "[FLASH][PROT] flash lock low 256K block!\n");
				}
				else if(app_lock_block == FLASH_LOCK_FW_LOW_512K){
					flash_lock_block_size = FLASH_LOCK_LOW_512K_MID146085;
					LOG_FLASH_PROT_DEBUG(0, 0, "[FLASH][PROT] flash lock low 512K block!\n");
				}
				else if(app_lock_block == FLASH_LOCK_FW_LOW_1M){
					/* attention 1: use can change this value according to application
					 * attention 2: can not lock stack SMP data storage area
					 * attention 3: firmware size under protection is not 1M, user should calculate
					 * demo code: choose 960K, leave 64K for system data(SMP storage data & calibration data & MAC address) and user data,
					 * 			  now firmware size under protection is 960K - 512K = 448K
					 * if this demo can not meet your requirement, you should change !!! */
					flash_lock_block_size = FLASH_LOCK_LOW_960K_MID146085;
					LOG_FLASH_PROT_DEBUG(0, 0, "[FLASH][PROT] flash lock low 960K block!\n");
				}
				break;
		#endif

		#if (FLASH_GD25LD80C_SUPPORT_EN || FLASH_GD25LD80E_SUPPORT_EN) //1M capacity
			case MID1460C8:
				if(app_lock_block == FLASH_LOCK_FW_LOW_256K || app_lock_block == FLASH_LOCK_FW_LOW_512K){
					/* attention that :This flash type, minimum lock size is 768K, do not support 256K or other value
					 * demo code will lock 768K when user set OTA 128K or 256K as multiple boot address,
					 * system data(SMP storage data & calibration data & MAC address) is OK;
					 * user data must be stored in flash address bigger than 768K !!!
					 * if this demo code lock area do not meet your requirement, you can change it !!!*/
					flash_lock_block_size = FLASH_LOCK_LOW_768K_MID1460C8;
					LOG_FLASH_PROT_DEBUG(0, 0, "[FLASH][PROT] flash lock low 768K block!\n");
				}
				else if(app_lock_block == FLASH_LOCK_FW_LOW_1M){
					/* attention 1: use can change this value according to application
					 * attention 2: can not lock stack SMP data storage area
					 * attention 3: firmware size under protection is not 1M, user should calculate
					 * demo code: choose 960K, leave 64K for system data(SMP storage data & calibration data & MAC address) and user data,
					 * 			  now firmware size under protection is 960K - 512K = 448K
					 * if this demo can not meet your requirement, you should change !!! */
					flash_lock_block_size = FLASH_LOCK_LOW_960K_MID1460C8;
					LOG_FLASH_PROT_DEBUG(0, 0, "[FLASH][PROT] flash lock low 960K block!\n");
				}
				break;
		#endif

		default:
			break;
	}

	return flash_lock_block_size;
}
#endif

static _attribute_data_retention_ u32 flash_current_lockAddr_end = 0; // not include self address.

/**
 * @brief      this function is used to lock flash.
 * @param[in]  lock_addr_end- flash lock address end
 * @param[in]  ceiling_flag	- get lock setting by ceiling or floor if not found. 1: ceiling. 0: floor.
 *             ceiling_flag must be 0, when it is going to write/erase flash.
 * @return     none
 * @note       
 */
void flash_lock(unsigned int lock_addr_end, int ceiling_flag)
{
	if(blc_flashProt.init_err || (NULL == flash_get_lock_status_mid)){
		return;
	}
	
	u16 flash_lock_cmd = flash_change_address_to_flash_lock_reg(lock_addr_end, ceiling_flag, &flash_current_lockAddr_end);
	u16 cur_lock_status = flash_get_lock_status_mid();
	LOG_FLASH_PROT_DEBUG(0, 0, "[FLASH] flash locking %s, cur_lock_reg: 0x%02x, target_lock_reg: 0x%02x, target lock addr end: 0x%x", (cur_lock_status == flash_lock_cmd) ? "unchange" : "  change", cur_lock_status, flash_lock_cmd, flash_current_lockAddr_end);

	if(cur_lock_status == flash_lock_cmd){ //lock status is what we want, no need lock again

	}
	else{ //unlocked or locked block size is not what we want
		#if 0 // no need unlock first
		if(cur_lock_status != flash_unlock_status){ //locked block size is not what we want, need unlock first
			for(int i = 0; i < 3; i++){ //Unlock flash up to 3 times to prevent failure.
				flash_unlock_mid();
				cur_lock_status = flash_get_lock_status_mid();

				if(cur_lock_status == flash_unlock_status){ //unlock success
					break;
				}
			}
		}
		#endif

		for(int i = 0; i < 3; i++) //Lock flash up to 3 times to prevent failure.
		{
			flash_lock_mid(flash_lock_cmd);
			cur_lock_status = flash_get_lock_status_mid();
			if(cur_lock_status == flash_lock_cmd){  //lock OK
				break;
			}
		}
	}
}

#if 0 // use flash_lock_() instead. // if use this function, should call flash_change_address_to_flash_lock_reg_ to update flash_current_lockAddr_end
/**
 * @brief      this function is used to unlock flash.
 * @param[in]  none
 * @return     none
 */
void flash_unlock(void)
{
	if(blc_flashProt.init_err || (NULL == flash_get_lock_status_mid)){
		return;
	}

	u16 cur_lock_status = flash_get_lock_status_mid();

	if(cur_lock_status != flash_unlock_status){ //not in lock status
		flash_current_lockAddr_end = 0; // unlock state.
		for(int i = 0; i < 3; i++){ //Unlock flash up to 3 times to prevent failure.
			flash_unlock_mid();
			cur_lock_status = flash_get_lock_status_mid();

			if(cur_lock_status == flash_unlock_status){ //unlock success
				break;
			}
		}
	}
}
#endif

// ---------------------------- extend protection by mesh SDK ---------------------
#ifdef FLASH_ADR_AREA_1_END
STATIC_ASSERT(FLASH_SIZE_MAX_SW >= FLASH_ADR_AREA_1_END);
#endif
#ifdef FLASH_ADR_AREA_2_END
STATIC_ASSERT(FLASH_SIZE_MAX_SW >= FLASH_ADR_AREA_2_END);
#endif
#ifdef MESH_FLASH_PROTECTION_EN
STATIC_ASSERT(MESH_FLASH_PROTECTION_EN == 0); // can not enable both.
#endif

#if (0 == __TLSR_RISCV_EN__)
/**
 * @brief      This function is used to read current used multiple boot address.
 * 			   return value is set by API "blc ota_setFirmwareSizeAndBootAddress"
 * @param[in]  none
 * @return     multiple boot address
 */
static inline u32  blc_ota_getCurrentUsedMultipleBootAddress(void)
{
	return ota_program_bootAddr;
}
#endif

/**
 * @brief       This function will be called when ota begin to unlock flash.
 * @return      none
 * @note        
 */
void app_flash_protection_ota_begin()
{
	if(flash_prot_op_cb){
		/* ota write data, destroy old boot mark, so 0 ~ ota_program_bootAddr + ota_firmware_max_size */
		flash_prot_op_cb(FLASH_OP_EVT_STACK_OTA_WRITE_NEW_FW_BEGIN, 0, ota_program_bootAddr + ota_firmware_size_k * 1024); // lock none due to both two firmware area need to be modify: new firmware and 4B of old fw.
	}
}

/**
 * @brief       This function will be called when ota finished to lock flash.
 * @param[in]   ota_err_code - OTA error code when OTA completed.
 * @return      none
 * @note        
 */
void app_flash_protection_ota_end(u32 ota_err_code)
{
	if(OTA_SUCCESS == ota_err_code){
		// prevent this case: if new firmware disabe flash protect function unintentionally, it will can not ota any more.
		// if protect here, it may not support OTA downgrade firmware version.
		// and it will protect flash in user init of new firmware.
	}else{
		// for some SDK, such as B91m SDK, they do not reboot if OTA failed, and just erase flash and init OTA flow.
		// so need flash protect here.
		if(flash_prot_op_cb){
			flash_prot_op_cb(FLASH_OP_EVT_STACK_OTA_WRITE_NEW_FW_END, 0, 0);
		}
	}
}

/**
 * @brief       This function will be called when write mesh parameter to one of these sectors.
 * @param[in]   op_addr_begin - operating flash address range begin value
 * @param[in]   op_addr_end - operating flash address range end value
 * @return      backup address of lock area.
 * @note        
 */
u32 app_flash_protection_mesh_par_modify_much_sectors_begin(u32 op_addr_begin, u32 op_addr_end)
{
	u32 backup_addr = flash_current_lockAddr_end; // may be modified when in flash_prot_op_cb_ function.
	if(flash_prot_op_cb){
		flash_prot_op_cb(FLASH_OP_EVT_MESH_PAR_MODIFY_MUCH_SECTORS_BEGIN, op_addr_begin, op_addr_end);
	}
	return backup_addr;
}

/**
 * @brief       This function will be called when finish writing mesh parameter to one of these sectors.
 * @param[in]   backup_lock_end_addr	- backup address of lock area. Warning! it is not operation flash address.
 * @return      none
 * @note        
 */
void app_flash_protection_mesh_par_modify_much_sectors_end(u32 backup_lock_end_addr)
{
	if(flash_prot_op_cb){
		flash_prot_op_cb(FLASH_OP_EVT_MESH_PAR_MODIFY_MUCH_SECTORS_END, backup_lock_end_addr, 0);
	}
}

typedef struct{
	u16 lock_cmd;
	u16 lock_addr_end_4k; // unit 4k.
}lock_area_t;

typedef struct{
	u32 flash_id;
	lock_area_t lock_area[4];
}flash_lock_area_support_t;

const flash_lock_area_support_t FLASH_LOCK_AREA_SUPPORT[] = { // area must be sorted from low to high if not zero.
#if (0 == __TLSR_RISCV_EN__)
	#if (FLASH_ZB25WD40B_SUPPORT_EN) //512K capacity
	{MID13325E, {{FLASH_LOCK_LOW_256K_MID13325E, (256/4)}, {FLASH_LOCK_LOW_448K_MID13325E, (448/4)},
			     {FLASH_LOCK_ALL_512K_MID13325E, (512/4)}}},
	#endif
	#if (FLASH_ZB25WD80B_SUPPORT_EN) //1M capacity
	{MID14325E, {{FLASH_LOCK_LOW_768K_MID14325E, (768/4)}, {FLASH_LOCK_LOW_896K_MID14325E, (896/4)},
			     {FLASH_LOCK_ALL_1M_MID14325E, 	(1024/4)}}},
	#endif
	#if (FLASH_GD25LD40C_SUPPORT_EN || FLASH_GD25LD40E_SUPPORT_EN) //512K capacity
	{MID1360C8, {{FLASH_LOCK_LOW_256K_MID1360C8, (256/4)}, {FLASH_LOCK_LOW_448K_MID1360C8, (448/4)},
			     {FLASH_LOCK_ALL_512K_MID1360C8, (512/4)}}},
	#endif
	#if(FLASH_P25Q80U_SUPPORT_EN) //1M capacity
	{MID146085, {{FLASH_LOCK_LOW_128K_MID146085, (128/4)}, // use 128k for (FLASH_ADR_RESET_CNT 0x23000) of dual mode
	             {FLASH_LOCK_LOW_512K_MID146085, (512/4)}, // use 512 but not 768(0xC0000), becasue of (FLASH_ADR_AREA_1_START 0xB4000)
				 {FLASH_LOCK_LOW_896K_MID146085, (896/4)}, {FLASH_LOCK_ALL_1M_MID146085, (1024/4)}}},
	#endif
	#if (FLASH_GD25LD80C_SUPPORT_EN || FLASH_GD25LD80E_SUPPORT_EN) //1M capacity
	{MID1460C8, {{FLASH_LOCK_LOW_768K_MID1460C8, (768/4)}, {FLASH_LOCK_LOW_896K_MID1460C8, (896/4)},
				 {FLASH_LOCK_ALL_1M_MID1460C8, 	(1024/4)}}},
	#endif
#else // B91 B92
	#if (FLASH_P25Q80U_SUPPORT_EN) //1M capacity
	{MID146085, {{FLASH_LOCK_LOW_256K_MID146085, (256/4)}, {FLASH_LOCK_LOW_512K_MID146085, (512/4)},
			     {FLASH_LOCK_LOW_896K_MID146085, (896/4)}, {FLASH_LOCK_ALL_1M_MID146085, (1024/4)}}},
	#endif
	#if (FLASH_P25Q16SU_SUPPORT_EN) // 2M capacity
	{MID156085, {{FLASH_LOCK_LOW_256K_MID156085, (256/4)}, {FLASH_LOCK_LOW_512K_MID156085, (512/4)},
			     {FLASH_LOCK_LOW_1M_MID156085, (1024/4)}, {FLASH_LOCK_ALL_2M_MID156085, (2048/4)}}},
	#endif
	#if (FLASH_P25Q32SU_SUPPORT_EN) // 4M capacity
	{MID166085, {{FLASH_LOCK_LOW_256K_MID166085, (256/4)}, {FLASH_LOCK_LOW_512K_MID166085, (512/4)},
			     {FLASH_LOCK_LOW_1M_MID166085, (1024/4)}, {FLASH_LOCK_ALL_4M_MID166085, (4096/4)}}},
	#endif
	
	#if 1//(MCU_CORE_TYPE == MCU_CORE_B92) //B92
	#if (FLASH_P25Q128L_SUPPORT_EN) // 16M capacity
	{MID186085, {{FLASH_LOCK_LOW_4M_MID186085, (4096/4)}, {FLASH_LOCK_LOW_8M_MID186085, (8192/4)},
				 {FLASH_LOCK_LOW_12M_MID186085, (12228/4)}, {FLASH_LOCK_ALL_16M_MID186085, (16384/4)}}},
	#endif
	#if (FLASH_GD25LQ16E_SUPPORT_EN) // 2M capacity
	{MID1560c8, {{FLASH_LOCK_LOW_256K_MID1560c8, (256/4)}, {FLASH_LOCK_LOW_512K_MID1560c8, (512/4)},
				 {FLASH_LOCK_LOW_1M_MID1560c8, (1024/4)}, {FLASH_LOCK_ALL_2M_MID1560c8, (2048/4)}}},
	#endif
	#endif
#endif
};

/**
 * @brief       This function get lock register value from protect end address which start from address 0.
 * @param[in]   lock_addr_end_in	- lock end address, and start address is 0.
 * @param[out]  lock_addr_end_out	- final result of lock end address which flash support.
 * @param[in]   ceiling_flag		- get lock setting by ceiling or floor if not found. 1: ceiling. 0: floor.
 *              ceiling_flag must be 0, when it is going to write/erase flash.
 * @return      
 * @note        this function will replace flash_change_app_lock_block_to_flash_lock_block()
 */
u16 flash_change_address_to_flash_lock_reg(u32 lock_addr_end_in, int ceiling_flag, u32 *lock_addr_end_out)
{
	*lock_addr_end_out = 0;
	u16 lock_cmd = 0;
	if(0 == lock_addr_end_in || blc_flashProt.init_err){
		return 0;
	}

	foreach_arr(i, FLASH_LOCK_AREA_SUPPORT){ // TO get the flash lock area support in initial function should be better ?
		const flash_lock_area_support_t * p_lock_area_support = &FLASH_LOCK_AREA_SUPPORT[i];
		if(blc_flash_mid == p_lock_area_support->flash_id){
			#if (FLASH_PROTECTION_CEILING_EN)
			if(ceiling_flag){
				foreach_arr(j, p_lock_area_support->lock_area){
					const lock_area_t *p_lock_area = &p_lock_area_support->lock_area[j];
					u32 lock_addr_end_support = p_lock_area->lock_addr_end_4k * 0x1000;
					if(lock_addr_end_support){
						*lock_addr_end_out = lock_addr_end_support;
						lock_cmd = p_lock_area->lock_cmd; // get the last if not found.
						if(lock_addr_end_support >= lock_addr_end_in){
							break;
						}
					}
				}
			}else
			#endif
			{
				foreach_arr(j, p_lock_area_support->lock_area){
					const lock_area_t *p_lock_area = &p_lock_area_support->lock_area[ARRAY_SIZE(p_lock_area_support->lock_area) - 1 - j];
					u32 lock_addr_end_support = p_lock_area->lock_addr_end_4k * 0x1000;
					if(lock_addr_end_support){
						if(lock_addr_end_support <= lock_addr_end_in){
							*lock_addr_end_out = lock_addr_end_support;
							lock_cmd = p_lock_area->lock_cmd; // return 0 if not found.
							break;
						}
					}
				}
			}

			break;
		}
	}

	return lock_cmd;
}

/**
 * @brief       This function change flash lock register when the area is locked.
 * @param[in]   op_addr_begin	- operating flash address range begin value
 * @return      0: mean no need to change unlock flash register, otherwise needed, and the value is lock address end before change.
 * @note        
 */
u32 flash_unlock_with_check_current_st(u32 op_addr_begin)
{
	if(op_addr_begin >= flash_current_lockAddr_end){
		// LOG_FLASH_PROT_DEBUG(0, 0, "flash API: unchange lock reg! op_addr: 0x%x", op_addr_begin);
		return 0; // no need to change unlock flash register because the target address in not at locked state.
	}else{
		u32 backup_addr = flash_current_lockAddr_end; // may be modified when in flash_lock_ function.
		LOG_FLASH_PROT_DEBUG(0, 0, "flash API: change lock reg, op_addr: 0x%x, backup_addr: 0x%x", op_addr_begin, backup_addr);
		flash_lock(op_addr_begin, 0);
		return backup_addr;
	}
}

/**
 * @brief      flash protection operation, including all locking & unlocking for application
 * 			   handle all flash write & erase action for this demo code. use should add more more if they have more flash operation.
 * @param[in]  flash_op_evt - flash operation event, including application layer action and stack layer action event(OTA write & erase)
 * 			   attention 1: if you have more flash write or erase action, you should should add more type and process them
 * 			   attention 2: for "end" event, no need to pay attention on op_addr_begin & op_addr_end, we set them to 0 for
 * 			   			    stack event, such as stack OTA write new firmware end event
 * @param[in]  op_addr_begin - operating flash address range begin value
 * @param[in]  op_addr_end - operating flash address range end value
 * 			   attention that, we use: [op_addr_begin, op_addr_end)
 * 			   e.g. if we write flash sector from 0x10000 to 0x20000, actual operating flash address is 0x10000 ~ 0x1FFFF
 * 			   		but we use [0x10000, 0x20000):  op_addr_begin = 0x10000, op_addr_end = 0x20000
 * @return     none
 */
//_attribute_data_retention_ u16  flash_lockBlock_cmd = 0;
#if 1 // (0 == __TLSR_RISCV_EN__)
void app_flash_protection_operation(u8 flash_op_evt, u32 op_addr_begin, u32 op_addr_end)
{
	static _attribute_data_retention_ u32	flash_default_lockAddr_end = 0;
	if(flash_op_evt == FLASH_OP_EVT_APP_INITIALIZATION)
	{
		/* ignore "op addr_begin" and "op addr_end" for initialization event
		 * must call "flash protection_init" first, will choose correct flash protection relative API according to current internal flash type in MCU */
		flash_protection_init();

		#if 1
		    #if defined(FLASH_ADR_PROTECT_END)
		u32 lock_addr_end = FLASH_ADR_PROTECT_END;
			#elif ((0 == FLASH_PLUS_ENABLE) && (0 == __TLSR_RISCV_EN__))
		u32 lock_addr_end = FLASH_ADR_AREA_2_START; // b85m 512k flash
			#else
		u32 lock_addr_end = FLASH_ADR_AREA_1_START; // for dual mode, FLASH_ADR_RESET_CNT is smaller than FLASH_ADR_AREA_1_START.
			#endif

		lock_addr_end = min(lock_addr_end, min3(FLASH_ADR_RESET_CNT, FLASH_ADR_MISC, FLASH_ADR_SW_LEVEL)); // frequent operation sectors
		
		flash_change_address_to_flash_lock_reg(lock_addr_end, FLASH_PROTECTION_CEILING_EN, &flash_default_lockAddr_end);
		LOG_FLASH_PROT_DEBUG(0, 0, "[FLASH][PROT] initialization, lock flash address end input: 0x%x, result: 0x%x", lock_addr_end, flash_default_lockAddr_end);
		flash_lock(flash_default_lockAddr_end, FLASH_PROTECTION_CEILING_EN);
		#else
		/* just sample code here, protect all flash area for old firmware and OTA new firmware.
		 * user can change this design if have other consideration */
		u32  app_lockBlock = 0;
		#if 1 // always enable OTA
			u32 multiBootAddress = blc_ota_getCurrentUsedMultipleBootAddress();
			if(multiBootAddress == MULTI_BOOT_ADDR_0x40000){
				/* attention that 512K capacity flash can not lock all 512K area, should leave some upper sector
				 * for system data(SMP storage data & calibration data & MAC address) and user data
				 * will use a approximate value */
				app_lockBlock = FLASH_LOCK_FW_LOW_512K;
			}
			#if(MCU_CORE_TYPE == MCU_CORE_8278)
			else if(multiBootAddress == MULTI_BOOT_ADDR_0x80000){
				#if 0 // not be used now.
				if(blc_flash_capacity < FLASH_SIZE_1M){ //for flash capacity smaller than 1M, OTA can not use 512K as multiple boot address
					blc_flashProt.init_err = 1;
				}
				else{
					/* attention that 1M capacity flash can not lock all 1M area, should leave some upper sector for
					 * system data(SMP storage data & calibration data & MAC address) and user data
					 * will use a approximate value */
					app_lockBlock = FLASH_LOCK_FW_LOW_1M;
				}
				#endif
			}
			#endif
		#else
			app_lockBlock = FLASH_LOCK_FW_LOW_256K; //just demo value, user can change this value according to application
		#endif


		flash_lockBlock_cmd = flash_change_app_lock_block_to_flash_lock_block(app_lockBlock);

		if(blc_flashProt.init_err){
			LOG_FLASH_PROT_DEBUG(0, 0, "[FLASH][PROT] flash protection initialization error!!!\n");
		}
		LOG_FLASH_PROT_DEBUG(0, 0, "[FLASH][PROT] initialization, lock flash\n");
		flash_lock(flash_lockBlock_cmd);
		#endif
	}
#if 1
	else if(flash_op_evt == FLASH_OP_EVT_STACK_OTA_CLEAR_OLD_FW_BEGIN)
	{
		/* OTA clear old firmware begin event is triggered by stack, in "blc ota_initOtaServer_module", rebooting from a successful OTA.
		 * Software will erase whole old firmware for potential next new OTA, need unlock flash if any part of flash address from
		 * "op addr_begin" to "op addr_end" is in locking block area.
		 * In this sample code, we protect whole flash area for old and new firmware, so here we do not need judge "op addr_begin" and "op addr_end",
		 * must unlock flash */
		LOG_FLASH_PROT_DEBUG(0, 0, "[FLASH][PROT] OTA clear old FW begin, unlock flash");
		flash_lock(op_addr_begin, 0); 
	}
	else if(flash_op_evt == FLASH_OP_EVT_STACK_OTA_CLEAR_OLD_FW_END)
	{
		/* ignore "op addr_begin" and "op addr_end" for END event
		 * OTA clear old firmware end event is triggered by stack, in "blc ota_initOtaServer_module", erasing old firmware data finished.
		 * In this sample code, we need lock flash again, because we have unlocked it at the begin event of clear old firmware */
		LOG_FLASH_PROT_DEBUG(0, 0, "[FLASH][PROT] OTA clear old FW end, restore flash locking");
		flash_lock(flash_default_lockAddr_end, FLASH_PROTECTION_CEILING_EN);
	}
	else if(flash_op_evt == FLASH_OP_EVT_STACK_OTA_WRITE_NEW_FW_BEGIN)
	{
		/* OTA write new firmware begin event is triggered by stack, when receive first OTA data PDU.
		 * Software will write data to flash on new firmware area,  need unlock flash if any part of flash address from
		 * "op addr_begin" to "op addr_end" is in locking block area.
		 * In this sample code, we protect whole flash area for old and new firmware, so here we do not need judge "op addr_begin" and "op addr_end",
		 * must unlock flash */
		LOG_FLASH_PROT_DEBUG(0, 0, "[FLASH][PROT] OTA write new FW begin, unlock flash");
		flash_lock(op_addr_begin, 0);
	}
	else if(flash_op_evt == FLASH_OP_EVT_STACK_OTA_WRITE_NEW_FW_END)
	{
		/* ignore "op addr_begin" and "op addr_end" for END event
		 * OTA write new firmware end event is triggered by stack, after OTA end or an OTA error happens, writing new firmware data finished.
		 * In this sample code, we need lock flash again, because we have unlocked it at the begin event of write new firmware */
		LOG_FLASH_PROT_DEBUG(0, 0, "[FLASH][PROT] OTA write new FW end, restore flash locking");
		flash_lock(flash_default_lockAddr_end, FLASH_PROTECTION_CEILING_EN);
	}
	else if(flash_op_evt == FLASH_OP_EVT_MESH_PAR_MODIFY_MUCH_SECTORS_BEGIN)
	{
		LOG_FLASH_PROT_DEBUG(0, 0, "\n[FLASH][PROT] mesh par write begin, unlock flash: 0x%x", op_addr_begin);
		flash_lock(op_addr_begin, 0);
	}
	else if(flash_op_evt == FLASH_OP_EVT_MESH_PAR_MODIFY_MUCH_SECTORS_END)
	{
		LOG_FLASH_PROT_DEBUG(0, 0, "[FLASH][PROT] mesh par write end, restore flash locking: 0x%x", op_addr_begin);
		flash_lock(op_addr_begin, FLASH_PROTECTION_CEILING_EN); // must restore original value, especially during OTA.
	}
#endif
	/* add more flash protection operation for your application if needed */
}
#endif


#endif
