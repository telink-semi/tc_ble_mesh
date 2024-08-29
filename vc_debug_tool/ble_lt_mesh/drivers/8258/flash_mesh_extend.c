/********************************************************************************************************
 * @file	flash_mesh_extend.c
 *
 * @brief	This is the source file for B85
 *
 * @author	Driver Group
 * @date	May 8,2018
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

u32 flash_sector_mac_address = CFG_SECTOR_ADR_MAC_CODE;
u32 flash_sector_calibration = CFG_SECTOR_ADR_CALIBRATION_CODE;

#if AUTO_ADAPT_MAC_ADDR_TO_FLASH_TYPE_EN
void blc_readFlashSize_autoConfigCustomFlashSector(void)
{
#if (((MCU_CORE_TYPE == MCU_CORE_8267)||(MCU_CORE_TYPE == MCU_CORE_8269)) \
    || (MESH_USER_DEFINE_MODE == MESH_IRONMAN_MENLO_ENABLE) || DUAL_MODE_ADAPT_EN)
    // always use fixed customized address
#else
	u8 *temp_buf;
	unsigned int mid = flash_read_mid();
	temp_buf = (u8 *)&mid;
	u8	flash_cap = temp_buf[2];

    if(CFG_ADR_MAC_512K_FLASH == CFG_SECTOR_ADR_MAC_CODE){
    	if(flash_cap == FLASH_SIZE_1M){
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
    	}
	}else if(CFG_ADR_MAC_1M_FLASH == CFG_SECTOR_ADR_MAC_CODE){
	    if(flash_cap != FLASH_SIZE_1M){
            while(1){ // please check your Flash size
                #if(MODULE_WATCHDOG_ENABLE)
                wd_clear();
                #endif
            }
		}
	}

	flash_set_capacity(flash_cap);
#endif
}
#endif



#if(HOMEKIT_EN)

void flash_write_data (unsigned long addr, unsigned long len, unsigned char *buf)
{
	int ns = 256 - (addr & 0xff);
	do {
		int nw = len > ns ? ns : len;
		flash_write_page (addr, nw, buf);
		ns = 256;
		addr += nw;
		buf += nw;
		len -= nw;
	} while (len > 0);
}

void flash_write_val(unsigned long adr, unsigned long flag_invalid, unsigned long dat)
{
	unsigned long p = 0xffffffff;
	int i;
	for (i=0; i<1024; i++)
	{
		flash_read_page(adr + i *4, 4, (unsigned char *)&p);
		if (p == 0xffffffff)
		{
			break;
		}
		else if (p != flag_invalid)
		{
			if (i == 1023)
			{
				flash_erase_sector (adr);
				i = 0;
			}
			else
			{
				flash_write_data (adr + i * 4, 4, (unsigned char *)&flag_invalid);
				i++;
			}
			break;
		}
	}
	flash_write_data (adr + i * 4, 4, (unsigned char *)&dat);
}

unsigned long flash_read_val(unsigned long adr, unsigned long flag_invalid)
{
	unsigned long p = 0xffffffff;
	int i = 0;
	for (i=0; i<1024; i++)
	{
		flash_read_page(adr + i *4, 4, (unsigned char *)&p);
		if (p == 0xffffffff)
		{
			break;
		}
		else if (p != flag_invalid)
		{
			return p;
		}
	}
	if( i>1000 )
		flash_erase_sector (adr);
	return flag_invalid;
}


void flash_write_long_val(unsigned long adr, unsigned long flag_invalid, unsigned char* dat, unsigned char len)
{
	unsigned long p = 0xffffffff;
	int i;
	int data_len = len / 4;
	for (i=0; i<(1024-data_len);)
	{
		flash_read_page(adr + i *4, 4, (unsigned char *)&p);
		if (p == 0xffffffff)
		{
			break;
		}
		else if (p != flag_invalid)
		{
			if (i >= 1024-data_len)
			{
				flash_erase_sector (adr);
				i = 0;
				break;
			}
			else
			{
				flash_write_data (adr + i * 4, 4, (unsigned char *)&flag_invalid);
				i+=data_len;
				break;
			}
		}
		i+=data_len;
	}
	flash_write_data (adr + i * 4, len, dat);

}

unsigned long flash_read_long_val(unsigned long adr, unsigned long flag_invalid, unsigned char* dat, unsigned char len)
{
	unsigned long p = 0xffffffff;

	int data_len = (len + 3) / 4;		//+3 to make len to be true len.
	int i = 0;
	for ( i=0; i<(1024-data_len); )
	{
		flash_read_page(adr + i *4, 4, (unsigned char *)&p);

		if( p == 0xffffffff )
		{
			break;
		}
		else if (p != flag_invalid)
		{
			flash_read_page(adr + i * 4, len, dat);
			return p;
		}
		i+=data_len;
	}
	if( i >= 1000 )
		flash_erase_sector (adr);
	return flag_invalid;
}

unsigned long flash_subregion_write_val (unsigned long adr, unsigned long flag_invalid, unsigned long dat, unsigned long num)
{
	unsigned long p = 0xffffffff;
	int i;
	for (i=0; i<num; i++)
	{
		flash_read_page(adr + i *4, 4, (unsigned char *)&p);
		if (p == 0xffffffff)
		{
			break;
		}
		else if (p != flag_invalid)
		{
			if (i >= (num - 1))
			{
				return flag_invalid;
			}
			else
			{
				flash_write_data (adr + i * 4, 4, (unsigned char *)&flag_invalid);
				i++;
			}
			break;
		}
	}
	flash_write_data (adr + i * 4, 4, (unsigned char *)&dat);

	return 1;
}

unsigned long flash_subregion_read_val (unsigned long adr, unsigned long flag_invalid, unsigned long num)
{
	unsigned long p = 0xffffffff;
	int i = 0;
	for (i=0; i<num; i++)
	{
		flash_read_page(adr + i *4, 4, (unsigned char *)&p);
		if (p == 0xffffffff)
		{
			break;
		}
		else if (p != flag_invalid)
		{
			return p;
		}
	}

	//if((i>1000) || (i>num))
	//	flash_erase_sector (adr);

	return flag_invalid;
}
#endif


