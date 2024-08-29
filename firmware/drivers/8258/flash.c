/********************************************************************************************************
 * @file	flash.c
 *
 * @brief	This is the source file for B85
 *
 * @author	Driver Group
 * @date	2018
 *
 * @par     Copyright (c) 2018, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#include "proj/drivers/spi.h"
#include "flash.h"
#include "proj/drivers/spi_i.h"
#include "proj/mcu/watchdog_i.h"
#include "proj_lib/ble/blt_config.h"

#if (MCU_CORE_TYPE == MCU_CORE_8278)
/**
 * If you add a new flash, you need to check the following location and add the corresponding flash model:
	#1 need to add flash_midxxx.c and flash_midxxx.h to the driver/flash directory, and introduce the header file flash_midxxx.h into flash_type.h.
	#2 need to add the corresponding mid to flash_mid_e in flash.h to check if Flash_CapacityDef contains the size of the added flash.
	#3 Check that flash_get_vendor() in flash.c and flash_vendor_e in flash.h already contain the added flash model.
	#4 need to add the corresponding mid in flash_support_mid[], and the Flash Type/uid CMD/MID/Company/Sector Erase Time(MAX) in the following table.
	#5 need to add a flash test area of corresponding size in Test_Demo/app_flash_test.c.
	#6 need to add the corresponding new flash to Flash_Deno/app.c in flash_lock()/flash_unlock(), as well as add the implementation of flash_midxxx_test()
	and add the case of flash_midxxx_test() in user_init().
	#7 If add ZB flash type,need to change the API zb_cam_modify(),and need to confirm the time sequence with ZB.

 	If add flash type, need pay attention to the read uid command and the bit number of status register.
	Flash Type			uid CMD			MID		Company		Sector Erase Time(MAX)
	P25Q80U    	 		0x4b        0x146085    PUYA        20ms
	GD25LD10C			0x4b(AN)	0x1160C8	GD			500ms
GD25LD40C/GD25LD40E		0x4b		0x1360C8	GD			500ms
GD25LD80C/GD25LD80E		0x4b(AN)	0x1460C8	GD			500ms
	ZB25WD10A			0x4b		0x11325E	ZB			500ms
	ZB25WD40B			0x4b		0x13325E	ZB			500ms
	ZB25WD80B			0x4b		0x14325E	ZB			500ms
	ZB25WD20A			0x4b		0x12325E	ZB			500ms	The actual capacity is 256K, but the nominal value is 128KB.
											The software cannot do capacity adaptation and requires special customer special processing.

	The uid of the early ZB25WD40B (mid is 0x13325E) is 8 bytes. If you read 16 bytes of uid,
	the next 8 bytes will be read as 0xff. Later, the uid of ZB25WD40B has been switched to 16 bytes.
 */
static const unsigned int flash_support_mid[] = {0x1160C8, 0x1360C8, 0x1460C8, 0x11325E, 0x12325E, 0x13325E, 0x14325E,0x146085};
#else // 8258 and others
/**
 *	If you add a new flash, you need to check the following location and add the corresponding flash model:
	#1 need to add flash_midxxx.c and flash_midxxx.h to the driver/flash directory, and introduce the header file flash_midxxx.h into flash_type.h.
	#2 need to add the corresponding mid to flash_mid_e in flash.h to check if Flash_CapacityDef contains the size of the added flash.
	#3 Check that flash_get_vendor() in flash.c and flash_vendor_e in flash.h already contain the added flash model.
	#4 need to add the corresponding mid in flash_support_mid[], and the Flash Type/uid CMD/MID/Company/Sector Erase Time(MAX) in the following table.
	#5 need to add a flash test area of corresponding size in Test_Demo/app_flash_test.c.
	#6 need to add the corresponding new flash to Flash_Deno/app.c in flash_lock()/flash_unlock(), as well as add the implementation of flash_midxxx_test()
	and add the case of flash_midxxx_test() in user_init().
	#7 If add ZB flash type,need to change the API zb_cam_modify(),and need to confirm the time sequence with ZB.

 	If add flash type, need pay attention to the read uid command and the bit number of status register.
	Flash Type	uid CMD			MID		Company		Sector Erase Time(MAX)
	ZB25WD40B	0x4b		0x13325E	ZB			500ms
	ZB25WD80B	0x4b		0x14325E	ZB			500ms
	GD25LD05C	0x4b(AN)	0x1060C8	GD			500ms
	GD25LD40C	0x4b		0x1360C8	GD			500ms
	GD25LD40E	0x4b		0x1360C8	GD			500ms
	GD25LD80C	0x4b(AN)	0x1460C8	GD			500ms
	GD25LD80E	0x4b(AN)	0x1460C8	GD			500ms
	GD25LE80C	0x4b		0x1460C8	GD			300ms
	GD25LQ80C	0x4b		0x1460C8	GD			300ms
	MD25D40D	0x4b(AN)	0x134051	GD			400ms
	P25Q40L		0x4b		0x136085	PUYA		12ms
	TH25D40LA	0x4b		0x1360EB	TH			12ms
	TH25D40UA	0x4b		0x1360EB	TH			12ms

	The uid of the early ZB25WD40B (mid is 0x13325E) is 8 bytes. If you read 16 bytes of uid,
	the next 8 bytes will be read as 0xff. Later, the uid of ZB25WD40B has been switched to 16 bytes.
 */
static const unsigned int flash_support_mid[] = {0x13325E, 0x14325E, 0x1060C8, 0x1360C8, 0x1460C8, 0x011460C8, 0x134051, 0x136085, 0x1360EB};
#endif
const unsigned int FLASH_CNT = sizeof(flash_support_mid)/sizeof(*flash_support_mid);

_attribute_data_retention_    unsigned char zbit_flash_flag = 0;
_attribute_data_retention_   _attribute_aligned_(4)	Flash_CapacityDef	flash_capacity;
/*******************************************************************************************************************
 *												Primary interface
 ******************************************************************************************************************/

/**
 * @brief		This function to determine whether the flash is busy..
 * @return		1:Indicates that the flash is busy. 0:Indicates that the flash is free
 */
_attribute_ram_code_sec_ static inline int flash_is_busy(){
	return mspi_read() & 0x01;		//the busy bit, pls check flash spec
}

/**
 * @brief		This function serves to set flash write command.
 * @param[in]	cmd	- set command.
 * @return		none.
 */
_attribute_ram_code_sec_noinline_ static void flash_send_cmd(unsigned char cmd){
	mspi_high();
	sleep_us(1);
	mspi_low();
	mspi_write(cmd);
	mspi_wait();
}

/**
 * @brief		This function serves to send flash address.
 * @param[in]	addr	- the flash address.
 * @return		none.
 */
_attribute_ram_code_sec_noinline_ static void flash_send_addr(unsigned int addr){
	mspi_write((unsigned char)(addr>>16));
	mspi_wait();
	mspi_write((unsigned char)(addr>>8));
	mspi_wait();
	mspi_write((unsigned char)(addr));
	mspi_wait();
}

/**
 * @brief     This function serves to wait flash done.(make this a asynchronous version).
 * @return    none.
 */
_attribute_ram_code_sec_noinline_ static void flash_wait_done(void)
{
	sleep_us(100);
	flash_send_cmd(FLASH_READ_STATUS_CMD_LOWBYTE);

	int i;
	for(i = 0; i < 10000000; ++i){
		if(!flash_is_busy()){
			break;
		}
	}
	mspi_high();
}

/**
 * @brief 		This function is used to read data from flash or read the status of flash.
 * @param[in]   cmd			- the read command.
 * @param[in]   addr		- starting address.
 * @param[in]   addr_en		- whether need to send an address.
 * @param[in]   dummy_cnt	- the length(in byte) of dummy.
 * @param[out]  data		- the start address of the data buffer.
 * @param[in]   data_len	- the length(in byte) of content needs to read out.
 * @return 		none.
 */
_attribute_ram_code_sec_noinline_ void flash_mspi_read_ram(unsigned char cmd, unsigned long addr, unsigned char addr_en, unsigned char dummy_cnt, unsigned char *data, unsigned long data_len)
{
	unsigned char r = irq_disable();

	flash_send_cmd(cmd);
	if(addr_en)
	{
		flash_send_addr(addr);
	}
	for(int i = 0; i < dummy_cnt; ++i)
	{
		mspi_write(0x00);		/* dummy */
		mspi_wait();
	}
	mspi_write(0x00);			/* to issue clock */
	mspi_wait();
	mspi_ctrl_write(0x0a);		/* auto mode */
	mspi_wait();
	for(int i = 0; i < data_len; ++i)
	{
		*data++ = mspi_get();
		mspi_wait();
	}
	mspi_high();

	irq_restore(r);
}

/**
 * @brief 		This function is used to write data or status to flash.
 * @param[in]   cmd			- the write command.
 * @param[in]   addr		- starting address.
 * @param[in]   addr_en		- whether need to send an address.
 * @param[out]  data		- the start address of the data buffer.
 * @param[in]   data_len	- the length(in byte) of content needs to be written.
 * @return 		none.
 * @note		important:  "data" must not reside at flash, such as constant string.If that case, pls copy to memory first before write.
 */
_attribute_ram_code_sec_noinline_ void flash_mspi_write_ram(unsigned char cmd, unsigned long addr, unsigned char addr_en, unsigned char *data, unsigned long data_len)
{
	unsigned char r = irq_disable();

	flash_send_cmd(FLASH_WRITE_ENABLE_CMD);
	flash_send_cmd(cmd);
	if(addr_en)
	{
		flash_send_addr(addr);
	}
	for(int i = 0; i < data_len; ++i)
	{
		mspi_write(data[i]);
		mspi_wait();
	}
	mspi_high();
	flash_wait_done();

	irq_restore(r);
}

#if MI_API_ENABLE
static inline int is_valid_sector_addr(u32 addr, unsigned long len)
{
    #if FLASH_1M_ENABLE // FLASH_PLUS_ENABLE
	if(((addr & 0xFFFFF000) == MI_BLE_MESH_CER_ADR) || (addr >= 0x100000))
    #else
	if(((addr & 0xFFFFF000) == MI_BLE_MESH_CER_ADR || (addr >= 0x80000))&&
	    (!(((flash_sector_mac_address == CFG_ADR_MAC_1M_FLASH) && (addr == FLASH_ADR_EDCH_PARA)))))
    #endif
	{
		return 0;
	}

	return 1;
}
#else
#define FLASH_MAX_SIZE		(1024 * 1024) // max 1M for B85m mesh now

/**
 * @brief       This function server to protect firmware area.
 * @param[in]   addr- start address of the area.
 * @param[in]   len	- the length(in byte) of content needs to write into the flash.
 * @return      1: addr is valid. 0: addr is invalid.
 * @note        
 */
static inline int is_valid_sector_addr(unsigned long addr, unsigned long len)
{
#if (FW_START_BY_LEGACY_BOOTLOADER_EN == 0)
	#if ((MCU_CORE_TYPE == MCU_CORE_8258) || (MCU_CORE_TYPE == MCU_CORE_8278))	
 		#if (MCU_CORE_TYPE == MCU_CORE_8258)
	u32 fw_boot_addr = ((REG_ADDR8(0x63e) & 0x03) << 7) << 10;
		#elif(MCU_CORE_TYPE == MCU_CORE_8278)
	u32 fw_boot_addr = ((REG_ADDR8(0x63e) & 0x07) << 7) << 10;
		#endif
		
	if(((addr >= fw_boot_addr + (FW_SIZE_MAX_K << 10)) && (addr + len <= FLASH_MAX_SIZE)) // not use flash_capacity, because it may be changed unexpected.
	|| ((addr + len <= fw_boot_addr)/* && (addr < fw_boot_addr)*/) // implied (fw_boot_addr != 0)
	|| ((addr == (fw_boot_addr + 8)) && (len <= 4))){ // for write OTA flag
		return 1;
	}

	return 0;
	#endif
 #endif

	return 1;
}
#endif

/**
 * @brief 		This function serves to erase a sector.
 * @param[in]   addr	- the start address of the sector needs to erase.
 * @return 		none.
 * @note        Attention: The block erase takes a long time, please pay attention to feeding the dog in advance.
 * 				The maximum block erase time is listed at the beginning of this document and is available for viewing.
 *
 * 				Attention: Before calling the FLASH function, please check the power supply voltage of the chip.
 *              Only if the detected voltage is greater than the safe voltage value, the FLASH function can be called.
 *              Taking into account the factors such as power supply fluctuations, the safe voltage value needs to be greater
 *              than the minimum chip operating voltage. For the specific value, please make a reasonable setting according
 *              to the specific application and hardware circuit.
 *
 *              Risk description: When the chip power supply voltage is relatively low, due to the unstable power supply,
 *              there may be a risk of error in the operation of the flash (especially for the write and erase operations.
 *              If an abnormality occurs, the firmware and user data may be rewritten, resulting in the final Product failure)
 */
#if (PINGPONG_OTA_DISABLE) // because functions of "ota_fw_ow.c" need all to be in ramcode now.
_attribute_ram_code_
#endif
void flash_erase_sector(unsigned long addr)
{
#if (1 || MI_API_ENABLE)
	if(0 == is_valid_sector_addr(addr, 1)){ // len 1 for compare erase end address.
        return ;
    }
#endif

#if(MODULE_WATCHDOG_ENABLE)
	wd_clear(); // add by BLE Team
#endif

#if (APP_FLASH_PROTECTION_ENABLE)
	u32 backup_lock_addr = flash_unlock_with_check_current_st(addr);
#endif
	flash_mspi_write_ram(FLASH_SECT_ERASE_CMD, addr, 1, NULL, 0);
#if (APP_FLASH_PROTECTION_ENABLE)
	if(backup_lock_addr){
		flash_lock(backup_lock_addr, FLASH_PROTECTION_CEILING_EN);
	}
#endif
}

/**
 * @brief 		This function reads the content from a page to the buf.
 * @param[in]   addr	- the start address of the page.
 * @param[in]   len		- the length(in byte) of content needs to read out from the page.
 * @param[out]  buf		- the start address of the buffer.
 * @return 		none.
 * @note        Attention: Before calling the FLASH function, please check the power supply voltage of the chip.
 *              Only if the detected voltage is greater than the safe voltage value, the FLASH function can be called.
 *              Taking into account the factors such as power supply fluctuations, the safe voltage value needs to be greater
 *              than the minimum chip operating voltage. For the specific value, please make a reasonable setting according
 *              to the specific application and hardware circuit.
 *
 *              Risk description: When the chip power supply voltage is relatively low, due to the unstable power supply,
 *              there may be a risk of error in the operation of the flash (especially for the write and erase operations.
 *              If an abnormality occurs, the firmware and user data may be rewritten, resulting in the final Product failure)
 */
#if (PINGPONG_OTA_DISABLE)
_attribute_ram_code_
#endif
void flash_read_page(unsigned long addr, unsigned long len, unsigned char *buf)
{
	flash_mspi_read_ram(FLASH_READ_CMD, addr, 1, 0, buf, len);
}

/**
 * @brief 		This function writes the buffer's content to the flash.
 * @param[in]   addr	- the start address of the area.
 * @param[in]   len		- the length(in byte) of content needs to write into the flash.
 * @param[in]   buf		- the start address of the content needs to write into.
 * @return 		none.
 * @note        the function support cross-page writing,which means the len of buf can bigger than 256.
 *
 *              Attention: Before calling the FLASH function, please check the power supply voltage of the chip.
 *              Only if the detected voltage is greater than the safe voltage value, the FLASH function can be called.
 *              Taking into account the factors such as power supply fluctuations, the safe voltage value needs to be greater
 *              than the minimum chip operating voltage. For the specific value, please make a reasonable setting according
 *              to the specific application and hardware circuit.
 *
 *              Risk description: When the chip power supply voltage is relatively low, due to the unstable power supply,
 *              there may be a risk of error in the operation of the flash (especially for the write and erase operations.
 *              If an abnormality occurs, the firmware and user data may be rewritten, resulting in the final Product failure)
 */
#if (PINGPONG_OTA_DISABLE)
_attribute_ram_code_
#endif
void flash_write_page(unsigned long addr, unsigned long len, unsigned char *buf)
{
#if (1 || MI_API_ENABLE)
	if(0 == is_valid_sector_addr(addr, len)){
        return ;
    }
#endif

#if (APP_FLASH_PROTECTION_ENABLE)
	u32 backup_lock_addr = flash_unlock_with_check_current_st(addr); // flash_write_status cost 5~6ms, flash_read_status cost 5~10us for flash mid 1360c8.
#endif
	unsigned int ns = PAGE_SIZE - (addr&(PAGE_SIZE - 1));
	int nw = 0;

	do{
		nw = len > ns ? ns :len;
		flash_mspi_write_ram(FLASH_WRITE_CMD, addr, 1, buf, nw);
		ns = PAGE_SIZE;
		addr += nw;
		buf += nw;
		len -= nw;
	}while(len > 0);
	
#if (APP_FLASH_PROTECTION_ENABLE)
	if(backup_lock_addr){
		flash_lock(backup_lock_addr, FLASH_PROTECTION_CEILING_EN);
	}
#endif
}

/**
 * @brief		This function reads the status of flash.
 * @param[in] 	cmd	- the cmd of read status.
 * @return 		the value of status.
 * @note        Attention: Before calling the FLASH function, please check the power supply voltage of the chip.
 *              Only if the detected voltage is greater than the safe voltage value, the FLASH function can be called.
 *              Taking into account the factors such as power supply fluctuations, the safe voltage value needs to be greater
 *              than the minimum chip operating voltage. For the specific value, please make a reasonable setting according
 *              to the specific application and hardware circuit.
 *
 *              Risk description: When the chip power supply voltage is relatively low, due to the unstable power supply,
 *              there may be a risk of error in the operation of the flash (especially for the write and erase operations.
 *              If an abnormality occurs, the firmware and user data may be rewritten, resulting in the final Product failure)
 */
unsigned char flash_read_status(unsigned char cmd)
{
	unsigned char status = 0;
	flash_mspi_read_ram(cmd, 0, 0, 0, &status, 1);
	return status;
}

/**
 * @brief 		This function write the status of flash.
 * @param[in]  	type	- the type of status.8 bit or 16 bit.
 * @param[in]  	data	- the value of status.
 * @return 		none.
 * @note        Attention: Before calling the FLASH function, please check the power supply voltage of the chip.
 *              Only if the detected voltage is greater than the safe voltage value, the FLASH function can be called.
 *              Taking into account the factors such as power supply fluctuations, the safe voltage value needs to be greater
 *              than the minimum chip operating voltage. For the specific value, please make a reasonable setting according
 *              to the specific application and hardware circuit.
 *
 *              Risk description: When the chip power supply voltage is relatively low, due to the unstable power supply,
 *              there may be a risk of error in the operation of the flash (especially for the write and erase operations.
 *              If an abnormality occurs, the firmware and user data may be rewritten, resulting in the final Product failure)
 *
 *              Because the operation of the write state takes a long time, and the interface of the write state will close the interrupt,
 *              it is not suitable for the situation that needs to respond to the interrupt in a timely manner.
 *              By using a weak definition for this interface, the application layer can redefine the interface according to its own needs.
 */
void flash_write_status(flash_status_typedef_e type , unsigned short data)
{
	unsigned char buf[2];

	buf[0] = data;
	buf[1] = data>>8;
	if(type == FLASH_TYPE_8BIT_STATUS){
		flash_mspi_write_ram(FLASH_WRITE_STATUS_CMD_LOWBYTE, 0, 0, buf, 1);
	}else if(type == FLASH_TYPE_16BIT_STATUS_ONE_CMD){
		flash_mspi_write_ram(FLASH_WRITE_STATUS_CMD_LOWBYTE, 0, 0, buf, 2);
	}
}


/* according to your application */
#if 0
/**
 * @brief 		This function serves to read data from the Security Registers of the flash.
 * @param[in]   addr	- the start address of the Security Registers.
 * @param[in]   len		- the length of the content to be read.
 * @param[out]  buf		- the starting address of the content to be read.
 * @return 		none.
 * @note        Attention: Before calling the FLASH function, please check the power supply voltage of the chip.
 *              Only if the detected voltage is greater than the safe voltage value, the FLASH function can be called.
 *              Taking into account the factors such as power supply fluctuations, the safe voltage value needs to be greater
 *              than the minimum chip operating voltage. For the specific value, please make a reasonable setting according
 *              to the specific application and hardware circuit.
 *
 *              Risk description: When the chip power supply voltage is relatively low, due to the unstable power supply,
 *              there may be a risk of error in the operation of the flash (especially for the write and erase operations.
 *              If an abnormality occurs, the firmware and user data may be rewritten, resulting in the final Product failure)
 */
void flash_read_otp(unsigned long addr, unsigned long len, unsigned char* buf)
{
	flash_mspi_read_ram(FLASH_READ_SECURITY_REGISTERS_CMD, addr, 1, 1, buf, len);
}

/**
 * @brief 		This function serves to write data to the Security Registers of the flash you choose.
 * @param[in]   addr	- the start address of the Security Registers.
 * @param[in]   len		- the length of content to be written.
 * @param[in]   buf		- the starting address of the content to be written.
 * @return 		none.
 * @note        Attention: Before calling the FLASH function, please check the power supply voltage of the chip.
 *              Only if the detected voltage is greater than the safe voltage value, the FLASH function can be called.
 *              Taking into account the factors such as power supply fluctuations, the safe voltage value needs to be greater
 *              than the minimum chip operating voltage. For the specific value, please make a reasonable setting according
 *              to the specific application and hardware circuit.
 *
 *              Risk description: When the chip power supply voltage is relatively low, due to the unstable power supply,
 *              there may be a risk of error in the operation of the flash (especially for the write and erase operations.
 *              If an abnormality occurs, the firmware and user data may be rewritten, resulting in the final Product failure)
 */
void flash_write_otp(unsigned long addr, unsigned long len, unsigned char *buf)
{
	unsigned int ns = PAGE_SIZE_OTP - (addr & (PAGE_SIZE_OTP - 1));
	int nw = 0;

	do{
		nw = len > ns ? ns :len;
		flash_mspi_write_ram(FLASH_WRITE_SECURITY_REGISTERS_CMD, addr, 1, buf, nw);
		ns = PAGE_SIZE_OTP;
		addr += nw;
		buf += nw;
		len -= nw;
	}while(len > 0);
}

/**
 * @brief 		This function serves to erase the data of the Security Registers that you choose.
 * @param[in]   addr	- the address that you want to erase.
 * @return 		none.
 * @Attention	Even you choose the middle area of the Security Registers,it will erase the whole area.
 * @note        Attention: Before calling the FLASH function, please check the power supply voltage of the chip.
 *              Only if the detected voltage is greater than the safe voltage value, the FLASH function can be called.
 *              Taking into account the factors such as power supply fluctuations, the safe voltage value needs to be greater
 *              than the minimum chip operating voltage. For the specific value, please make a reasonable setting according
 *              to the specific application and hardware circuit.
 *
 *              Risk description: When the chip power supply voltage is relatively low, due to the unstable power supply,
 *              there may be a risk of error in the operation of the flash (especially for the write and erase operations.
 *              If an abnormality occurs, the firmware and user data may be rewritten, resulting in the final Product failure)
 */
void flash_erase_otp(unsigned long addr)
{
	flash_mspi_write_ram(FLASH_ERASE_SECURITY_REGISTERS_CMD, addr, 1, NULL, 0);
}

/**
 * @brief	  	This function serves to read MID of flash(MAC id).
 * @return    	MID of the flash(3 bytes).
 * @note        Attention: Before calling the FLASH function, please check the power supply voltage of the chip.
 *              Only if the detected voltage is greater than the safe voltage value, the FLASH function can be called.
 *              Taking into account the factors such as power supply fluctuations, the safe voltage value needs to be greater
 *              than the minimum chip operating voltage. For the specific value, please make a reasonable setting according
 *              to the specific application and hardware circuit.
 *
 *              Risk description: When the chip power supply voltage is relatively low, due to the unstable power supply,
 *              there may be a risk of error in the operation of the flash (especially for the write and erase operations.
 *              If an abnormality occurs, the firmware and user data may be rewritten, resulting in the final Product failure)
 */
unsigned int flash_read_raw_mid(void)
{
	unsigned int flash_mid = 0;
	flash_mspi_read_ram(FLASH_GET_JEDEC_ID, 0, 0, 0, (unsigned char*)(&flash_mid), 3);
	return flash_mid;
}
#endif

/**
 * @brief	  	This function serves to read MID of flash(MAC id). Before reading UID of flash,
 * 				you must read MID of flash. and then you can look up the related table to select
 * 				the idcmd and read UID of flash.
 * @return    	MID of the flash(4 bytes).
 * @note        Attention: Before calling the FLASH function, please check the power supply voltage of the chip.
 *              Only if the detected voltage is greater than the safe voltage value, the FLASH function can be called.
 *              Taking into account the factors such as power supply fluctuations, the safe voltage value needs to be greater
 *              than the minimum chip operating voltage. For the specific value, please make a reasonable setting according
 *              to the specific application and hardware circuit.
 *
 *              Risk description: When the chip power supply voltage is relatively low, due to the unstable power supply,
 *              there may be a risk of error in the operation of the flash (especially for the write and erase operations.
 *              If an abnormality occurs, the firmware and user data may be rewritten, resulting in the final Product failure)
 */
flash_mid_e flash_read_mid(void)
{
	unsigned int flash_mid = 0;
	flash_mspi_read_ram(FLASH_GET_JEDEC_ID, 0, 0, 0, (unsigned char*)(&flash_mid), 3);

	#if (MCU_CORE_TYPE == MCU_CORE_8258)
	/*
	 * The mids of GD25LD80C/GD25LD80E and GD25LE80C are both 0x1460c8, but the status register of GD25LD80C/GD25LD80E are 8 bits,
	 * and the status register of GD25LE80C is 16 bits. The functions of the two chips are different.
	 * The software detection method is to read SFDP Signature. If it is 50444653H, it is GD25LE80C,
	 * if it is all zeros, it is GD25LD80C//GD25LD80E.
	 */
	if(flash_mid == 0x1460c8)
	{
		unsigned char buf[4]={0};
		flash_mspi_read_ram(0x5a, 0x00, 1, 1, buf, 4);
		if((buf[0] == 0x53)&&(buf[1] == 0x46)&&(buf[2] == 0x44)&&(buf[3] == 0x50))
		{
			flash_mid = 0x011460c8;
		}
	}
	#endif
	
	return flash_mid;
}

/**
 * @brief	  	This function serves to read UID of flash.Before reading UID of flash, you must read MID of flash.
 * 				and then you can look up the related table to select the idcmd and read UID of flash.
 * @param[in] 	idcmd	- different flash vendor have different read-uid command. E.g: GD/PUYA:0x4B; XTX: 0x5A.
 * @param[in] 	buf		- store UID of flash.
 * @return    	none.
 * @note        Attention: Before calling the FLASH function, please check the power supply voltage of the chip.
 *              Only if the detected voltage is greater than the safe voltage value, the FLASH function can be called.
 *              Taking into account the factors such as power supply fluctuations, the safe voltage value needs to be greater
 *              than the minimum chip operating voltage. For the specific value, please make a reasonable setting according
 *              to the specific application and hardware circuit.
 *
 *              Risk description: When the chip power supply voltage is relatively low, due to the unstable power supply,
 *              there may be a risk of error in the operation of the flash (especially for the write and erase operations.
 *              If an abnormality occurs, the firmware and user data may be rewritten, resulting in the final Product failure)
 */
void flash_read_uid(unsigned char idcmd, unsigned char *buf)
{
	if(idcmd == FLASH_READ_UID_CMD_GD_PUYA_ZB_TH)	//< GD/PUYA/ZB/TH
	{
		flash_mspi_read_ram(idcmd, 0x00, 1, 1, buf, 16);
	}
}

/*******************************************************************************************************************
 *												Secondary interface
 ******************************************************************************************************************/

/**
 * @brief		This function serves to read flash mid and uid,and check the correctness of mid and uid.
 * @param[out]	flash_mid	- Flash Manufacturer ID.
 * @param[out]	flash_uid	- Flash Unique ID.
 * @return		0: flash no uid or not a known flash model 	 1:the flash model is known and the uid is read.
 * @note        Attention: Before calling the FLASH function, please check the power supply voltage of the chip.
 *              Only if the detected voltage is greater than the safe voltage value, the FLASH function can be called.
 *              Taking into account the factors such as power supply fluctuations, the safe voltage value needs to be greater
 *              than the minimum chip operating voltage. For the specific value, please make a reasonable setting according
 *              to the specific application and hardware circuit.
 *
 *              Risk description: When the chip power supply voltage is relatively low, due to the unstable power supply,
 *              there may be a risk of error in the operation of the flash (especially for the write and erase operations.
 *              If an abnormality occurs, the firmware and user data may be rewritten, resulting in the final Product failure)
 */
int flash_read_mid_uid_with_check(unsigned int *flash_mid, unsigned char *flash_uid)
{
	unsigned char no_uid[16]={0x51,0x01,0x51,0x01,0x51,0x01,0x51,0x01,0x51,0x01,0x51,0x01,0x51,0x01,0x51,0x01};
	int i,f_cnt=0;
	*flash_mid = flash_read_mid();

	for(i=0; i<FLASH_CNT; i++){
		if(flash_support_mid[i] == *flash_mid){
			flash_read_uid(FLASH_READ_UID_CMD_GD_PUYA_ZB_TH, (unsigned char *)flash_uid);
			break;
		}
	}
	if(i == FLASH_CNT){
		return 0;
	}

	for(i=0; i<16; i++){
		if(flash_uid[i] == no_uid[i]){
			f_cnt++;
		}
	}

	if(f_cnt == 16){	//no uid flash
		return 0;
	}else{
		return 1;
	}
}

/**
 * @brief		This function serves to find whether it is zb flash.
 * @param[in]	none.
 * @return		1 - is zb flash;   0 - is not zb flash.
 */
unsigned char flash_is_zb(void)
{
	unsigned int flash_mid  = flash_read_mid();
	if((flash_mid == 0x13325E)||(flash_mid == 0x14325E) ||(flash_mid == 0x11325E)||(flash_mid == 0x12325E)) // the last two is belong to 827x
	{
		return 1;
	}
	return 0;
}

#if (MCU_CORE_TYPE == MCU_CORE_8258)
/**
 * @brief		This function serves to calibration the flash voltage(VDD_F),if the flash has the calib_value,we will use it,either will
 * 				trim vdd_f to 1.95V(2b'111 the max) if the flash is zb.
 * @param[in]	vol - the voltage which you want to set.
 * @return		none.
 */
void flash_vdd_f_calib(void)
{
	unsigned char calib_value = flash_get_vdd_f_calib_value();
	if((0xff == calib_value) || (0 != (calib_value & 0xf8)))
	{
		if(flash_is_zb())
		{
			analog_write(0x0c, ((analog_read(0x0c) & 0xf8) | FLASH_VOLTAGE_1V95));
		}
	}
	else
	{
		analog_write(0x0c, ((analog_read(0x0c) & 0xf8)  | calib_value));
	}
}
#elif (MCU_CORE_TYPE == MCU_CORE_8278)
/**
 * @brief		This function serves to calibration the flash voltage(VDD_F),if the flash has the calib_value,we will use it,either will
 * 				trim vdd_f to 1.95V(2b'111 the max) if the flash is zb.
 * @param[in]	vol - the voltage which you want to set.
 * @return		none.
 */
void flash_vdd_f_calib(void)
{
	unsigned short calib_value = flash_get_vdd_f_calib_value();
	if((0xffff == calib_value) || (0 != (calib_value & 0xf8f8)))
	{
		if(flash_is_zb())
		{
			analog_write(0x09, ((analog_read(0x09) & 0x8f) | (FLASH_VOLTAGE_1V95 << 4)));    		//ldo mode flash ldo trim 1.95V
			analog_write(0x0c, ((analog_read(0x0c) & 0xf8) | FLASH_VOLTAGE_1V9));					//dcdc mode flash ldo trim 1.90V
		}
	}
	else
	{
		analog_write(0x09, ((analog_read(0x09) & 0x8f)  | ((calib_value & 0xff00) >> 4) ));
		analog_write(0x0c, ((analog_read(0x0c) & 0xf8)  | (calib_value & 0xff)));
	}
}
#endif


/**
 * @brief		This function serves to get flash vendor.
 * @param[in]	none.
 * @return		0 - err, other - flash vendor.
 */
unsigned int flash_get_vendor(unsigned int flash_mid)
{
	switch(flash_mid&0x0000ffff)
	{
	case 0x0000325E:
		return FLASH_ETOX_ZB;
	case 0x000060C8:
		return FLASH_ETOX_GD;
	case 0x00004051:
		return FLASH_ETOX_GD;
	case 0x00006085:
		return FLASH_SONOS_PUYA;
	case 0x000060EB:
		return FLASH_SONOS_TH;
	case 0x000060CD:
		return FLASH_SST_TH;
	default:
		return 0;
	}
}

void flash_set_capacity(Flash_CapacityDef flash_cap)
{
	flash_capacity = flash_cap;
}

Flash_CapacityDef flash_get_capacity(void)
{
	return flash_capacity;
}
