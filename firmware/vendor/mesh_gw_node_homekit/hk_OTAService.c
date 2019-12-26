/********************************************************************************************************
 * @file     hk_OTAService.c
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
#define _HK_OTA_SERVICE_C_
 
 
/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "common.h"
#include "hk_OtaService.h"
#include "ble_UUID.h"
#include "hk_TopUUID.h"
#include "../../homekit_src/hk_ble_ll_ota.h"


extern u32 session_timeout_tick;
extern u8 EventNotifyCNT;
extern store_char_change_value_t* char_save_restore;

extern void hkble_ota_save_data(u32 adr, u8 * data);
/* ------------------------------------
    Macro Definitions
   ------------------------------------ */


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions
   ------------------------------------ */
int hkble_hk_ota_adr_index = -1;
bool hkble_hk_ota_success_flag = false;
hkble_ota_service_t hkble_hk_blcOta;
u16 hkble_hk_fw_last_index = 0xFFFF;

const unsigned char otaServiceUUID[16]				       = {0x40,0x26,0x0d,0x0c,0x0b,0x0a,0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x02,0x01,0x00};
const unsigned char charOtaDataUUID[16]                    = {0x41,0x26,0x0d,0x0c,0x0b,0x0a,0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x02,0x01,0x00};
const unsigned char charOtaStatusFaultUUID[16]             = {0x42,0x26,0x0d,0x0c,0x0b,0x0a,0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x02,0x01,0x00};

extern u32 ota_program_offset;

const u16 hk_ota_insts[6] = {OTA_INST_NO, OTA_INST_NO+1, OTA_INST_NO+2, 
							OTA_INST_NO+3, OTA_INST_NO+4, OTA_INST_NO+5};

extern u8 hkble_ota_data_check;
extern u32 hkble_ota_data_timeout;

unsigned char hOtaData = 0;
unsigned char hOtaStatusFault = 0;

unsigned char VAR_OTA_UNKOWN=0;

unsigned short ccc_hOtaData = 0;
unsigned short ccc_hOtaStatusFault = 0;


#if OTA_DATA_BRC_ENABLE
    unsigned char bc_para_hOtaData = 1;
#endif

#if OTA_STATUSFAULT_BRC_ENABLE
    unsigned char bc_para_hOtaStatusFault = 1;
#endif

const u8 hOtaServiceName[10] = "OtaService";

const u8 OTA_STATUSFAULT_VALID_RANGE[2] = {0x00, 0x01};

const u8 OTA_STATUSFAULT_VALID_STEP = 0x01;


const hap_characteristic_desc_t hap_desc_char_OTA_DATA_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};
const hap_characteristic_desc_t hap_desc_char_OTA_STATUSFAULT_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, OTA_STATUSFAULT_VALID_RANGE, &OTA_STATUSFAULT_VALID_STEP, BC_PARA_OTA_STATUSFAULT};

const hap_characteristic_desc_t hap_desc_char_ota_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if OTA_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * hOta_linked_svcs[] = {};
#endif

#if(OTA_IS_HIDDEN_SERVICE == 1)
const hap_service_desc_t hap_desc_service_ota_A5_R_S = {OTA_INST_NO+1, (0x02 + OTA_AS_PRIMARY_SERVICE), OTA_LINKED_SVCS_LENGTH, OTA_LINKED_SVCS};
#else
const hap_service_desc_t hap_desc_service_ota_A5_R_S = {OTA_INST_NO+1, (0x00 + OTA_AS_PRIMARY_SERVICE), OTA_LINKED_SVCS_LENGTH, OTA_LINKED_SVCS};
#endif

/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
extern unsigned long crc32_half_cal(unsigned long crc, unsigned char* input, unsigned long* table, int len);

void hkble_hk_ota_verify_data_from_flash(void)
{
	u16 ota_adr = 0;
	u16 last_index = 1;
	u8 flash_check[16];
	u32 fw_crc_init = 0xFFFFFFFF;
	u8 error_flg = 0;
	//u8 r = irq_disable ();
	while(ota_adr <= (last_index + 1))
	{

#if (__TL_LIB_8258__ || (MCU_CORE_TYPE && MCU_CORE_TYPE == MCU_CORE_8258))
		flash_read_page(ota_program_offset + (ota_adr<<4),16,flash_check);
#else
		flash_read_page(NEW_FW_ADR + (ota_adr<<4),16,flash_check);
#endif

		if(ota_adr <= last_index)
		{
			u8 ota_dat[32];

			//restore to the char in bin file when save OTA data to flash.
			extern u8 hkble_ota_flag;
			if(ota_adr == 0) flash_check[8] = hkble_ota_flag;
			
			for(int i=0;i<16;i++){
				ota_dat[i*2] = flash_check[i]&0x0f;
				ota_dat[i*2+1] = flash_check[i]>>4;
			}
			fw_crc_init = crc32_half_cal(fw_crc_init, ota_dat, (unsigned long* )crc32_half_tbl, 32);
		}

		if(ota_adr == 0)
		{
			if((flash_check[6] != HKBLE_FLAG_FW_CHECK) || (flash_check[7] != HKBLE_FW_CHECK_AGTHM2))
			{
				error_flg = HKBLE_OTA_FW_CHECK_ERR;
				break;
			}
		}
		else if(ota_adr == 1)
		{
			u32 fw_size = ((flash_check[8]<<0) | (flash_check[9]<<8) | (flash_check[10]<<16) | (flash_check[11]<<24));
			if(fw_size){
				last_index = (fw_size>>4) - 1;
			}

			if((!fw_size) || (last_index < 2))
			{
				error_flg = HKBLE_OTA_FW_SIZE_ERR;
				break;
			}
		}
		else if(ota_adr == (last_index + 1))
		{
			u32 fw_check_value = ((flash_check[0]<<0) | (flash_check[1]<<8) | (flash_check[2]<<16) | (flash_check[3]<<24));

			if(fw_check_value != fw_crc_init){
				error_flg = HKBLE_OTA_DATA_CRC_ERR;
			}
			break;
		}

		if((++ota_adr) > (last_index + 1))
		{
			error_flg = HKBLE_OTA_DATA_INDEX_ERR;
			break;
		}
	}
	//irq_restore(r);
	if (!error_flg)
	{
		hkble_ota_set_flag ();
		dev_mac_addr_update();
    	//R10 clear broadcast event param.
    	//blt_clean_broadcast_param();
		bls_ll_terminateConnection(0x13);
		rf_led_ota_ok();
		start_reboot();
	}
	else
	{
		if (hkble_hk_ota_adr_index >= 0)
		{
			u16 sector_size = ((hkble_hk_ota_adr_index + 1) * OTA_VALID_DATA_CNT_MAX) / 4096;
			if(((hkble_hk_ota_adr_index + 1) * OTA_VALID_DATA_CNT_MAX) % 4096)
				sector_size += 1;
				
			for(u16 i=0; i<=sector_size; i+=1)
			{
#if(__TL_LIB_8258__ || (MCU_CORE_TYPE && MCU_CORE_TYPE == MCU_CORE_8258))
				flash_erase_sector(ota_program_offset + (i * 0x1000));
#else
				flash_erase_sector(NEW_FW_ADR + (i * 0x1000));
#endif
			}
		}
		bls_ll_terminateConnection(0x13);
		rf_led_ota_error();
		start_reboot();
	}

}
int OtaDataReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int OtaDataWriteCallback(void *pp)
{
	blt_adv_set_global_state ();

    rf_packet_att_data_t *req = (rf_packet_att_data_t*)pp;
	
	u8 hk_err_flg = HKBLE_OTA_SUCCESS;
	u16 hk_ota_adr = (req->dat[0] | (req->dat[1]<<8));
	
//    u16 crc;
    
    int nDataLen = req->l2cap - 3;
	u8 hk_ota_pkt_cn = req->dat[2];
	//hk_ota_pkt_cn = (nDataLen - 5) / 16;
	
	hkble_ota_data_timeout = clock_time() | 1;

	if(hk_ota_adr == HKBLE_CMD_OTA_END)
	{
		/*for OTA end,then reboot the device.*/
#if 0
		if(hkble_hk_ota_success_flag)
		{
			hkble_hk_ota_success_flag = false;
			hkble_ota_set_flag ();
			dev_mac_addr_update();
			rf_led_ota_ok();
			start_reboot();
		}
#endif
		hkble_ota_data_check = HKBLE_OTA_DATA_END;
	}
	else if((nDataLen == (hk_ota_pkt_cn * 16 + 5)) && (hk_ota_pkt_cn > 0))
	{
		/*OTA data packet process.*/


		if(hkble_hk_ota_adr_index + 1 == hk_ota_adr){	//correct OTA data index

	//		crc = (req->dat[nDataLen - 1]<<8) | req->dat[nDataLen - 2];
	//		extern unsigned short ota_crc16 (unsigned char *pD, int len);
	//		if(crc == ota_crc16(req->dat, (nDataLen - 2)))
			{
			#if(HKBLE_OTA_FW_CHECK_EN)
				//////////////读取OTA数据的0x06~0x07，判断是否需要做fw check ////////////
				if((!hk_ota_adr) && (hk_ota_pkt_cn >= 2)){  //第一笔OTA数据
					hkble_ota_data_check = HKBLE_OTA_DATA_START;

					if(req->dat[9] == HKBLE_FLAG_FW_CHECK && req->dat[10] == HKBLE_FW_CHECK_AGTHM2){ //0x06的值为0x5D，0x07的值为0x02
						hkble_hk_blcOta.fw_check_en = 1;
					}
					else{
						hkble_hk_blcOta.fw_check_en = 0;
					}
					hkble_hk_ota_success_flag = false;
					hkble_hk_blcOta.fw_crc_init = 0xFFFFFFFF;  //crc init set to 0xFFFFFFFF

					//第2笔数据，在0x0018 ~ 0x001b 获取fw_size
					u32 fw_size = (req->dat[3+16+8] | (req->dat[3+16+9]<<8) | (req->dat[3+16+10]<<16) | (req->dat[3+16+11]<<24));
					if(fw_size){
						hkble_hk_blcOta.fw_crc_last_index = (fw_size/OTA_VALID_DATA_CNT_MAX) - 1;
					}
					else{ //如果0x0018处没有firmware size，则取消fw check
						hkble_hk_blcOta.fw_crc_last_index = 0xFFFF;
						hkble_hk_blcOta.fw_check_en = 0;
					}
				}

#if 0
				if(hkble_hk_blcOta.fw_check_en){
					u8 hk_ota_crc32_pkt_cn = 0;
					if(hk_ota_adr<=hkble_hk_blcOta.fw_crc_last_index)
					{
						hk_ota_crc32_pkt_cn = hk_ota_pkt_cn;
					}
					else if(hk_ota_adr == (hkble_hk_blcOta.fw_crc_last_index + 1))
					{
						if(hk_ota_pkt_cn > 1)
						{
							hk_ota_crc32_pkt_cn = hk_ota_pkt_cn - 1;
						}
					}
					
					if(hk_ota_crc32_pkt_cn > 0)
					{
						//将 n byte OTA data拆成 2n 个 half byte计算CRC
						u8 ota_dat[OTA_VALID_DATA_CNT_MAX * 2];
						for(int i=0;i<(16 * hk_ota_crc32_pkt_cn);i++){
							ota_dat[i*2] = req->dat[i+3]&0x0f;
							ota_dat[i*2+1] = req->dat[i+3]>>4;
						}
						extern unsigned long crc32_half_cal(unsigned long crc, unsigned char* input, unsigned long* table, int len);
						hkble_hk_blcOta.fw_crc_init = crc32_half_cal(hkble_hk_blcOta.fw_crc_init, ota_dat, (unsigned long* )crc32_half_tbl, (32 * hk_ota_crc32_pkt_cn));
					}
					
				}

				int fw_check_ok = 1;

				if(hkble_hk_blcOta.fw_check_en && (hk_ota_adr == (hkble_hk_blcOta.fw_crc_last_index + 1))){  //比较check值

					u32 fw_check_value = (req->dat[3+(hk_ota_pkt_cn-1)*16] | (req->dat[3+(hk_ota_pkt_cn-1)*16+1]<<8) | (req->dat[3+(hk_ota_pkt_cn-1)*16+2]<<16) | (req->dat[3+(hk_ota_pkt_cn-1)*16+3]<<24));
					if(fw_check_value != hkble_hk_blcOta.fw_crc_init){
						fw_check_ok = 0;
					}
				}
#endif
				//if(fw_check_ok)
				{
					//u8 flash_check[16];
					
					foreach(i, hk_ota_pkt_cn)
					{
						hkble_ota_save_data ((hk_ota_adr * OTA_VALID_DATA_CNT_MAX + i * 16), (req->dat + (3 + i * 16)));
						#if 0
						flash_read_page(ota_program_offset + (hk_ota_adr * OTA_VALID_DATA_CNT_MAX + i * 16),16,flash_check);
						
						if(!memcmp(flash_check,(req->dat + (3 + i * 16)),16)){  //OK
							//hk_ota_adr_index = hk_ota_adr;
						}
						else{ //flash write err
							hk_err_flg = HKBLE_OTA_WRITE_FLASH_ERR;
							break;
						}
						#endif
					}
					
					//if(hk_err_flg != HKBLE_OTA_WRITE_FLASH_ERR)
						hkble_hk_ota_adr_index = hk_ota_adr;
				}
				//else{ //fw check err
				//	hk_err_flg = HKBLE_OTA_FW_CHECK_ERR;
				//}

			#else
				
				if((!hk_ota_adr) && (hk_ota_pkt_cn >= 2)){  //第一笔OTA数据
					hkble_hk_ota_success_flag = false;

					//第2笔数据，在0x0018 ~ 0x001b 获取fw_size
					u32 fw_size = (req->dat[3+16+8] | (req->dat[3+16+9]<<8) | (req->dat[3+16+10]<<16) | (req->dat[3+16+11]<<24));
					
					if(fw_size){
						hkble_hk_fw_last_index = (fw_size/OTA_VALID_DATA_CNT_MAX) - 1;
					}
					else{ //如果0x0018处没有firmware size，则取消fw check
						hkble_hk_fw_last_index = 0xFFFF;
					}
				}

				u8 flash_check[16];
				
				foreach(i, hk_ota_pkt_cn)
				{
					hkble_ota_save_data ((hk_ota_adr * OTA_VALID_DATA_CNT_MAX + i * 16), (req->dat + (3 + i * 16)));
					#if 0
					flash_read_page(ota_program_offset + (hk_ota_adr * OTA_VALID_DATA_CNT_MAX + i * 16),16,flash_check);
					
					if(!memcmp(flash_check,(req->dat + (3 + i * 16)),16)){	//OK
						//hkble_hk_ota_adr_index = hk_ota_adr;
					}
					else{ //flash write err
						hk_err_flg = HKBLE_OTA_WRITE_FLASH_ERR;
						break;
					}
					#endif
				}
				
				//if(hk_err_flg != HKBLE_OTA_WRITE_FLASH_ERR)
					hkble_hk_ota_adr_index = hk_ota_adr;
				
				#endif
			}
			//else{  //crc err
			//	hk_err_flg = HKBLE_OTA_DATA_CRC_ERR;
			//}
		}
		else
		{
			hk_err_flg = HKBLE_OTA_PACKET_LOSS;
		}
		
		if(!hk_err_flg)
		{
			#if(HKBLE_OTA_FW_CHECK_EN)
			if(hk_ota_adr == (hkble_hk_blcOta.fw_crc_last_index + 1))
				hkble_hk_ota_success_flag = true;
			#else
			if(hk_ota_adr == (hkble_hk_fw_last_index + 1))
				hkble_hk_ota_success_flag = true;
			#endif
		}
		else
		{
			if (hkble_hk_ota_adr_index >= 0)
			{
				u16 sector_size = ((hkble_hk_ota_adr_index + 1) * OTA_VALID_DATA_CNT_MAX) / 4096;
				if(((hkble_hk_ota_adr_index + 1) * OTA_VALID_DATA_CNT_MAX) % 4096)
					sector_size += 1;

				for(u16 i=0; i<=sector_size; i+=1)
				{
	#if(__TL_LIB_8258__ || (MCU_CORE_TYPE && MCU_CORE_TYPE == MCU_CORE_8258))
					flash_erase_sector(ota_program_offset + (i * 0x1000));
	#else
					flash_erase_sector(NEW_FW_ADR + (i * 0x1000));
	#endif
				}
			}
			bls_ll_terminateConnection(0x13);
			rf_led_ota_error();
			start_reboot();
		}
	}
	
	session_timeout_tick = clock_time () | 1;
	return 1;
}

int OtaStatusFaultReadCallback(void *pp)
{
    session_timeout_tick = clock_time () | 1;
    return 1;
}


int ccc_OtaDataWriteCB(void *pp)
{
	static u16 ccc_hOtaData_pre = 0;

	if(ccc_hOtaData_pre != ccc_hOtaData)
	{
		if(ccc_hOtaData == 0)
		{
			ccc_hOtaData_pre = ccc_hOtaData;
			EventNotifyCNT--;
			indicate_handle_no_remove(OTA_HANDLE_NUM_DATA);
		}
		else
		{
			ccc_hOtaData_pre = ccc_hOtaData;
			EventNotifyCNT++;
			indicate_handle_no_reg(OTA_HANDLE_NUM_DATA);
		}
	}
	session_timeout_tick = clock_time () | 1;
	return 1;
}

int ccc_OtaStatusFaultWriteCB(void *pp)
{
	static u16 ccc_hOtaStatusFault_pre = 0;

	if(ccc_hOtaStatusFault_pre != ccc_hOtaStatusFault)
	{
		if(ccc_hOtaStatusFault == 0)
		{
			ccc_hOtaStatusFault_pre = ccc_hOtaStatusFault;
			EventNotifyCNT--;
			indicate_handle_no_remove(OTA_HANDLE_NUM_STATUSFAULT);
		}
		else
		{
			ccc_hOtaStatusFault_pre = ccc_hOtaStatusFault;
			EventNotifyCNT++;
			indicate_handle_no_reg(OTA_HANDLE_NUM_STATUSFAULT);
		}
	}
	session_timeout_tick = clock_time () | 1;
	return 1;
}

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
