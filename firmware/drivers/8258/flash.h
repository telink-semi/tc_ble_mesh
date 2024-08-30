/********************************************************************************************************
 * @file	flash.h
 *
 * @brief	This is the header file for B85
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
#pragma once

#include "compiler.h"


#define PAGE_SIZE		256
#define PAGE_SIZE_OTP	256

/**
 * @brief     flash mid definition
 */
#if (MCU_CORE_TYPE == MCU_CORE_8278)
typedef enum{
	MID11325E   =   0x11325e,//ZB25WD10A
	MID1160C8   =   0x1160c8,//GD25LD10C
	MID13325E   =   0x13325e,//ZB25WD40B
	MID1360C8   =   0x1360c8,//GD25LD40C/GD25LD40E
	MID14325E   =   0x14325e,//ZB25WD80B
	MID146085   =   0x146085,//P25Q80U
	MID1460C8   =   0x1460c8,//GD25LD80C
}flash_mid_e;
#else
typedef enum{
	MID011460C8   =   0x011460c8,//GD25LE80C / GD25LQ80C
	MID1060C8     =   0x1060c8,//GD25LD05C
	MID13325E     =   0x13325e,//ZB25WD40B
	MID134051     =   0x134051,//MD25D40D
	MID136085     =   0x136085,//P25Q40L
	MID1360C8     =   0x1360c8,//GD25LD40C / GD25LD40E
	MID1360EB     =   0x1360eb,//TH25D40UA / TH25D40LA
	MID14325E     =   0x14325e,//ZB25WD80B
	MID1460C8     =   0x1460c8,//GD25LD80C / GD25LD80E
}flash_mid_e;
#endif

/**
 * @brief     flash command definition
 */
enum{
	//common cmd
	FLASH_WRITE_CMD						=	0x02,
	FLASH_READ_CMD						=	0x03,
	FLASH_WRITE_SECURITY_REGISTERS_CMD	=	0x42,
	FLASH_READ_SECURITY_REGISTERS_CMD	=	0x48,

	FLASH_SECT_ERASE_CMD				=	0x20,
	FLASH_ERASE_SECURITY_REGISTERS_CMD	=	0x44,

	FLASH_READ_UID_CMD_GD_PUYA_ZB_TH	=	0x4B,	//Flash Type = GD/PUYA/ZB/TH
	FLASH_READ_UID_CMD_XTX				=	0x5A,	//Flash Type = XTX

	FLASH_GET_JEDEC_ID					=	0x9F,

	//special cmd
	FLASH_WRITE_STATUS_CMD_LOWBYTE		=	0x01,
	FLASH_WRITE_STATUS_CMD_HIGHBYTE		=	0x31,

	FLASH_READ_STATUS_CMD_LOWBYTE		=	0x05,
	FLASH_READ_STATUS_CMD_HIGHBYTE		=	0x35,

	FLASH_WRITE_DISABLE_CMD 			= 	0x04,
	FLASH_WRITE_ENABLE_CMD 				= 	0x06,

};

/**
 * @brief     flash status type definition
 */
typedef enum{
	FLASH_TYPE_8BIT_STATUS   			= 0,
	FLASH_TYPE_16BIT_STATUS_ONE_CMD  	= 1,
	FLASH_TYPE_16BIT_STATUS_TWO_CMD  	= 2,
}flash_status_typedef_e;

/**
 * @brief     flash vendor and technology definition
 */
typedef enum{
	FLASH_ETOX_ZB  		= 0x0100325E,	// 325E		bit[24]:ETOX: Byte Program Time != Page Programming Time
	FLASH_ETOX_GD   	= 0x010060C8,	// 60C8/4051
	FLASH_SONOS_PUYA  	= 0x02006085,	// 6085		bit[25]:SONOS:Byte Program Time == Page Programming Time
	FLASH_SONOS_TH  	= 0x020060EB,	// 60EB
	FLASH_SST_TH  		= 0x040060CD,	// 60CD		bit[26]:SST:  Byte Program Time != Page Programming Time
}flash_vendor_e;


typedef enum{
	FLASH_ETOX_ARCH  	= 0x01000000,
	FLASH_SONOS_ARCH  	= 0x02000000,
}flash_architecture_e;

/**
 * @brief	flash capacity definition
 *			Call flash_read_mid function to get the size of flash capacity.
 *			Example is as follows:
 *			unsigned int mid = flash_read_mid();
 *			The value of (mid&0x00ff0000)>>16 reflects flash capacity.
 */
typedef enum {
    FLASH_SIZE_64K     = 0x10,
    FLASH_SIZE_128K    = 0x11,
    FLASH_SIZE_256K    = 0x12,
    FLASH_SIZE_512K    = 0x13,
    FLASH_SIZE_1M      = 0x14,
    FLASH_SIZE_2M      = 0x15,
    FLASH_SIZE_4M      = 0x16,
    FLASH_SIZE_8M      = 0x17,
} Flash_CapacityDef;

/**
 * @brief	flash voltage definition
 */
typedef enum {
    FLASH_VOLTAGE_1V95     = 0x07,
    FLASH_VOLTAGE_1V9      = 0x06,
    FLASH_VOLTAGE_1V85     = 0x05,
    FLASH_VOLTAGE_1V8      = 0x04,
    FLASH_VOLTAGE_1V75     = 0x03,
    FLASH_VOLTAGE_1V7      = 0x02,
    FLASH_VOLTAGE_1V65     = 0x01,
    FLASH_VOLTAGE_1V6      = 0x00,
} Flash_VoltageDef;

/*******************************************************************************************************************
 *												Primary interface
 ******************************************************************************************************************/

extern  unsigned char zbit_flash_flag;

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
void flash_erase_sector(unsigned long addr);

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
void flash_read_page(unsigned long addr, unsigned long len, unsigned char *buf);

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
void flash_write_page(unsigned long addr, unsigned long len, unsigned char *buf);


#if(HOMEKIT_EN)
void 			flash_write_val(unsigned long adr, unsigned long flag_invalid, unsigned long dat);
unsigned long 	flash_read_val(unsigned long adr, unsigned long flag_invalid);
void 			flash_write_long_val(unsigned long adr, unsigned long flag_invalid, unsigned char* dat, unsigned char len);
unsigned long 	flash_read_long_val(unsigned long adr, unsigned long flag_invalid, unsigned char* dat, unsigned char len);
unsigned long flash_subregion_write_val (unsigned long adr, unsigned long flag_invalid, unsigned long dat, unsigned long num);
unsigned long flash_subregion_read_val (unsigned long adr, unsigned long flag_invalid, unsigned long num);
#endif

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
unsigned int flash_read_raw_mid(void);

/**
 * @brief	  	This function serves to read MID of flash(MAC id). Before reading UID of flash,
 * 				you must read MID of flash. and then you can look up the related table to select
 * 				the idcmd and read UID of flash
 * @return    	MID of the flash.
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
flash_mid_e flash_read_mid(void);

/**
 * @brief	  	This function serves to read UID of flash.Before reading UID of flash, you must read MID of flash.
 * 				and then you can look up the related table to select the idcmd and read UID of flash.
 * @param[in] 	idcmd	- different flash vendor have different read-uid command. E.g: GD/PUYA:0x4B; XTX: 0x5A
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
void flash_read_uid(unsigned char idcmd, unsigned char *buf);

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
int flash_read_mid_uid_with_check( unsigned int *flash_mid, unsigned char *flash_uid);

/**
 * @brief		This function serves to find whether it is zb flash.
 * @param[in]	none.
 * @return		1 - is zb flash;   0 - is not zb flash.
 */
unsigned char flash_is_zb(void);

/**
 * @brief		This function serves to calibration the flash voltage(VDD_F),if the flash has the calib_value,we will use it,either will
 * 				trim vdd_f to 1.95V(2b'111 the max) if the flash is zb.
 * @param[in]	vol - the voltage which you want to set.
 * @return		none.
 */
void flash_vdd_f_calib(void);

#if (MCU_CORE_TYPE == MCU_CORE_8258)
/**
 * @brief		This function serves to get the vdd_f calibration value.
 * @param[in]	none.
 * @return		none.
 */
static inline unsigned char flash_get_vdd_f_calib_value(void)
{
	unsigned int mid = flash_read_mid();
	unsigned char dcdc_flash_voltage = 0;
	switch((mid & 0xff0000) >> 16)
	{
	case(FLASH_SIZE_64K):
		flash_read_page(0xe1c0, 1, &dcdc_flash_voltage);
		break;
	case(FLASH_SIZE_128K):
		flash_read_page(0x1e1c0, 1, &dcdc_flash_voltage);
		break;
	case(FLASH_SIZE_512K):
		flash_read_page(0x771c0, 1, &dcdc_flash_voltage);
		break;
	case(FLASH_SIZE_1M):
		flash_read_page(0xfe1c0, 1, &dcdc_flash_voltage);
		break;
	case(FLASH_SIZE_2M):
		flash_read_page(0x1fe1c0, 1, &dcdc_flash_voltage);
		break;
	default:
		dcdc_flash_voltage = 0xff;
		break;
	}
	return dcdc_flash_voltage;
}
#elif (MCU_CORE_TYPE == MCU_CORE_8278)
/**
 * @brief		This function serves to get the vdd_f calibration value.
 * @param[in]	none.
 * @return		none.
 */
static inline unsigned short flash_get_vdd_f_calib_value(void)
{
	unsigned int mid = flash_read_mid();
	unsigned short flash_voltage = 0;
	switch((mid & 0xff0000) >> 16)
	{
	case(FLASH_SIZE_64K):
		flash_read_page(0xe1c0, 2, (unsigned char*)&flash_voltage);
		break;
	case(FLASH_SIZE_128K):
		flash_read_page(0x1e1c0, 2, (unsigned char*)&flash_voltage);
		break;
	case(FLASH_SIZE_512K):
		flash_read_page(0x771c0, 2, (unsigned char*)&flash_voltage);
		break;
	case(FLASH_SIZE_1M):
		flash_read_page(0xfe1c0, 2, (unsigned char*)&flash_voltage);
		break;
	case(FLASH_SIZE_2M):
		flash_read_page(0x1fe1c0, 2, (unsigned char*)&flash_voltage);
		break;
	default:
		flash_voltage = 0xff;
		break;
	}
	return flash_voltage;
}
#endif

/**
 * @brief		This function serves to get flash vendor.
 * @param[in]	none.
 * @return		0 - err, other - flash vendor.
 */
unsigned int flash_get_vendor(unsigned int flash_mid);

void flash_set_capacity(Flash_CapacityDef flash_cap);

Flash_CapacityDef flash_get_capacity(void);
