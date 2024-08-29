/********************************************************************************************************
 * @file	ble_ll_ota.c
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
#if (0 == __TLSR_RISCV_EN__)
#include "tl_common.h"
#include "proj_lib/ble/ble_common.h"
#include "proj_lib/ble/trace.h"
#include "proj_lib/pm.h"
#include "drivers/8258/flash.h"
#include "proj/mcu/watchdog_i.h"
#include "proj_lib/ble/service/ble_ll_ota.h"
#include "proj_lib/ble/ll/ll.h"
#include "proj_lib/sig_mesh/app_mesh.h"
#include "mesh_ota.h"
#include "version.h"
#if AIS_ENABLE
#include "proj_lib/mesh_crypto/aes_cbc.h"
#include "user_ali.h"
#endif
#if NMW_ENABLE
#include "user_nmw.h"
#endif
#if GATEWAY_ENABLE
#include "../mesh_provision/app.h"
#endif

_attribute_data_retention_ int ota_adr_index = -1;
_attribute_data_retention_ u32 blt_ota_start_tick;
_attribute_data_retention_ u32 blt_ota_timeout_us = 30000000;  //default 30 second
u32 blt_ota_finished_time = 0;
u8  blt_ota_finished_flag = 0;
u8  blt_ota_terminate_flag = 0;

_attribute_data_retention_ ota_service_t blcOta;


#if(__TL_LIB_8267__ || (MCU_CORE_TYPE && MCU_CORE_TYPE == MCU_CORE_8267) || \
	__TL_LIB_8269__ || (MCU_CORE_TYPE && MCU_CORE_TYPE == MCU_CORE_8269))
	u32		ota_program_offset = 0x40000;
	u32 	ota_firmware_size_k = FW_SIZE_MAX_K;
#elif(__TL_LIB_8258__ || (MCU_CORE_TYPE && MCU_CORE_TYPE == MCU_CORE_8258) || (MCU_CORE_TYPE == MCU_CORE_8278))
    #if (FLASH_1M_ENABLE && PINGPONG_OTA_DISABLE) // FLASH_PLUS_ENABLE
	_attribute_data_retention_	u32		ota_program_bootAddr = FLASH_ADR_UPDATE_NEW_FW; // it will be used in cpu_wakeup init, and set value for ota_program_offset_
    #else
	_attribute_data_retention_	u32		ota_program_bootAddr = 0x40000;
    #endif
    
	_attribute_data_retention_	u32		ota_program_offset = 0;
	_attribute_data_retention_	u32 	ota_firmware_size_k = FW_SIZE_MAX_K;
#else  //8266
	u32		ota_program_offset = 0x40000;
	u32 	ota_firmware_size_k = FW_SIZE_MAX_K;
	u32		bls_ota_bootFlagAddr = 	0x40000;
#endif



_attribute_data_retention_	ota_startCb_t		otaStartCb = NULL;
_attribute_data_retention_	ota_versionCb_t 	otaVersionCb = NULL;
_attribute_data_retention_	ota_resIndicateCb_t otaResIndicateCb = NULL;



unsigned short crc16 (const unsigned char *pD, int len)
{

    static unsigned short poly[2]={0, 0xa001};              //0x8005 <==> 0xa001
    unsigned short crc = 0xffff;
    //unsigned char ds;
    int i,j;

    for(j=len; j>0; j--)
    {
        unsigned char ds = *pD++;
        for(i=0; i<8; i++)
        {
            crc = (crc >> 1) ^ poly[(crc ^ ds ) & 1];
            ds = ds >> 1;
        }
    }

     return crc;
}

#if 0
unsigned long crc32_cal(unsigned long crc, unsigned char* input, unsigned long* table, int len)
{
    unsigned char* pch = input;
    for(int i=0; i<len; i++)
    {
        crc = (crc>>8) ^ table[(crc^*pch) & 0xff];
        pch++;
    }

    return crc;
}
#endif

#if(MCU_CORE_TYPE == MCU_CORE_8269)
void bls_ota_setFirmwareSizeAndOffset(int firmware_size_k, u32 ota_offset)
{
	ota_firmware_size_k = firmware_size_k;

#if(__TL_LIB_8267__ || (MCU_CORE_TYPE && MCU_CORE_TYPE == MCU_CORE_8267) || \
	__TL_LIB_8269__ || (MCU_CORE_TYPE && MCU_CORE_TYPE == MCU_CORE_8269))

#else
	ota_program_offset = ota_offset;
#endif
}
#endif

void bls_ota_setBootFlagAddr(u32 bootFlag_addr)
{
	bls_ota_bootFlagAddr = bootFlag_addr;
}


void bls_ota_registerStartCmdCb(ota_startCb_t cb)
{
	otaStartCb = cb;
}

void bls_ota_registerVersionReqCb(ota_versionCb_t cb)
{
	otaVersionCb = cb;
}

void bls_ota_registerResultIndicateCb(ota_resIndicateCb_t cb)
{
	otaResIndicateCb = cb;
}

void bls_ota_setTimeout(u32 timeout_us)
{
	blt_ota_timeout_us = timeout_us;
}

_attribute_ram_code_ void start_reboot(void)
{
#if(__TL_LIB_8258__ || (MCU_CORE_TYPE && MCU_CORE_TYPE == MCU_CORE_8258) || (MCU_CORE_TYPE == MCU_CORE_8278))
	if(blt_miscParam.pad32k_en){
		analog_write(SYS_DEEP_ANA_REG, analog_read(SYS_DEEP_ANA_REG) & (~SYS_NEED_REINIT_EXT32K)); //clear
	}
#endif
	irq_disable ();
#if SLEEP_FUNCTION_DISABLE
	REG_ADDR8(0x6f) = 0x20;  //reboot
#else
	cpu_sleep_wakeup(1, PM_WAKEUP_TIMER, clock_time() + 1*1000*sys_tick_per_us);//reboot
#endif
	while (1);
}

STATIC_ASSERT(BOOT_MARK_ADDR == 8); // for B85m.

int ota_set_flag()
{
    if(0 == is_valid_startup_flag(ota_program_offset + BOOT_MARK_ADDR, 0)){
        return -1; // invalid flag for the new firmware
    }

#if(__TL_LIB_8267__ || (MCU_CORE_TYPE && MCU_CORE_TYPE == MCU_CORE_8267) || \
	__TL_LIB_8269__ || (MCU_CORE_TYPE && MCU_CORE_TYPE == MCU_CORE_8269))
	u32 flag = 0x4b;
	flash_write_page(ota_program_offset + 8, 1, (u8 *)&flag);		//Set FW flag
	#if (PINGPONG_OTA_DISABLE || FW_START_BY_LEGACY_BOOTLOADER_EN)
	// not set invalid, because it may need to recover to old version when signature failed
	#else
	flag = 0;
	flash_write_page((ota_program_offset ? 0 : 0x40000) + 8, 4, (u8 *)&flag);	//Invalid flag
	#endif
#elif(__TL_LIB_8258__ || (MCU_CORE_TYPE && MCU_CORE_TYPE == MCU_CORE_8258) || (MCU_CORE_TYPE == MCU_CORE_8278)) //8258
	u32 flag = 0x4b;
	flash_write_page(ota_program_offset + 8, 1, (u8 *)&flag);		//Set FW flag
	#if (PINGPONG_OTA_DISABLE || FW_START_BY_LEGACY_BOOTLOADER_EN)
	// not set invalid, because it may need to recover to old version when signature failed
	#else
	flag = 0;
	flash_write_page((ota_program_offset ? 0 : ota_program_bootAddr) + 8, 4, (u8 *)&flag);	//Invalid flag
	#endif
#endif

	return 0;
}
u8 fw_ota_value =0;
u8 get_fw_ota_value()
{
	return fw_ota_value;
}
void ota_save_data(u32 adr, u8 * data, u16 len){
#if 1 // (! PINGPONG_OTA_DISABLE)
	if (adr == 0)
	{
		fw_ota_value = data[8];
		data[8] = 0xff;					//FW flag invalid
	}
#endif

	flash_write_page(ota_program_offset + adr, len, data);
}

#if GATEWAY_ENABLE
u8 ota_reboot_flag = 1;// initial it will reboot 
void set_ota_reboot_flag(u8 flag)
{
	ota_reboot_flag = flag;
	return ;
}
#endif

int otaWrite(void * p)
{
#if 0 // DUAL_VENDOR_EN	// confirm later
    return 0;
#endif

	rf_packet_att_data_t *req = (rf_packet_att_data_t*)p;
	static u32 fw_check_val = 0;
	static u8 need_check_type = 0;//=1:crc val sum
	static u16 ota_pkt_total = 0;

	u8 err_flg = OTA_SUCCESS;
	u16 ota_adr =  req->dat[0] | (req->dat[1]<<8);
	u16 crc;
	
#if DUAL_OTA_NEED_LOGIN_EN
    if(ota_adr == CMD_OTA_START){
        cmd_ota_mesh_hk_login_handle(req->dat+2);
    }
#endif

	if(!ota_condition_enable()){
		return 0;
	}
	
	if(ota_adr == CMD_OTA_FW_VERSION){
		//to be add
		//log_event(TR_T_ota_version);
		blt_ota_start_tick = clock_time()|1;  //mark time
		if(otaVersionCb){
			otaVersionCb();
		}
	}
	else if(ota_adr == CMD_OTA_START){
		ota_adr_index = -1;
		#if GATEWAY_ENABLE
		if(ota_reboot_flag)
		#endif
		{
			blcOta.ota_start_flag = 1;   //set flag
			blt_ota_start_tick = clock_time()|1;  //mark time
			if(otaStartCb){
				otaStartCb();
			}
		}
		
		//log_event(TR_T_ota_start);
		#if (ZBIT_FLASH_WRITE_TIME_LONG_WORKAROUND_EN)
		check_and_set_1p95v_to_zbit_flash();
		#endif
		#if APP_FLASH_PROTECTION_ENABLE
		// app_flash_protection_ota_begin(); // has been done in entry_ota_mode_
		#endif
	}
	else if(ota_adr == CMD_OTA_END){
		//log_event(TR_T_ota_end);

		u16 adrIndex_max	   = req->dat[2] | (req->dat[3]<<8);
		u16 adrIndex_max_check = req->dat[4] | (req->dat[5]<<8);


		//if no index_max check, set ota success directly, otherwise we check if any index_max match
		if( req->l2cap == 9 && (adrIndex_max ^ adrIndex_max_check) == 0xffff){  //index_max valid, we can check
			if(adrIndex_max != ota_adr_index){  //last one or more packets missed
				err_flg = OTA_DATA_INCOMPLETE;
			}
		}
		if(ota_pkt_total != ota_adr_index + 1){
			err_flg = OTA_DATA_INCOMPLETE;
		}

		blt_ota_finished_flag_set(err_flg);
	}
	else{
		//log_task_begin(TR_T_ota_data);
		if(ota_adr_index + 1 == ota_adr){   //correct OTA data index
			blt_ota_start_tick = clock_time()|1;  //mark time
			crc = (req->dat[19]<<8) | req->dat[18];
			if(crc == crc16(req->dat, 18)){
			    #if ENCODE_OTA_BIN_EN
			    int nDataLen = req->l2cap - 7;
			    if(16 == nDataLen){
    			    u8 key[16];
    			    memcpy(key, key_encode_bin, sizeof(key));
    			    aes_decrypt(key, req->dat+2, req->dat+2);
			    }
			    #endif
				if(ota_adr == 0){
				    if(req->dat[8] == 0x5D){
				    	need_check_type = req->dat[9] ;
				    }
				    
				    if(need_check_type == FW_CHECK_AGTHM1){
				    	fw_check_val = 0;   // init
				    }else if(need_check_type == FW_CHECK_AGTHM2){
				        fw_check_val = 0xFFFFFFFF;  //crc init set to 0xFFFFFFFF
				    }
					else{
						err_flg = OTA_FW_CHECK_ERR;
					}

                    if(!is_valid_tlk_fw_buf(req->dat+10)){
                        err_flg = OTA_FW_CHECK_ERR;
                    #if 1 // GATT policy, depend on user
                    }else if(0 == ota_is_valid_pid_vid((fw_id_t *)(req->dat+4), 1)){
                    	#if GATEWAY_ENABLE
                        if(0 == ota_reboot_flag){
							// means gateway is receiving firmware for other nodes, rather than gateway self.
                    	}else
						#endif
						{
                        	err_flg = OTA_FW_CHECK_ERR;
                    	}
                    #endif
                    }else{
    				    #if ENCODE_OTA_BIN_EN
    				    if(need_check_type != FW_CHECK_AGTHM2){
    				        err_flg = OTA_FW_CHECK_ERR;
    				    }
    				    #endif
				    }
				}else if(ota_adr == 1){
					ota_pkt_total = (((req->dat[10]) |( (req->dat[11] << 8) & 0xFF00) | ((req->dat[12] << 16) & 0xFF0000) | ((req->dat[13] << 24) & 0xFF000000)) + 15)/16;
					if(ota_pkt_total < 3){
						// invalid fw
						err_flg = OTA_FW_SIZE_ERR;
					}
				}else if(ota_adr == ota_pkt_total - 1){
				    u32 crc_bin = ((req->dat[2]) |( (req->dat[3] << 8) & 0xFF00) | ((req->dat[4] << 16) & 0xFF0000) | ((req->dat[5] << 24) & 0xFF000000));
				    if((need_check_type == FW_CHECK_AGTHM1) || (need_check_type == FW_CHECK_AGTHM2)){
    					if(fw_check_val != crc_bin){
    						err_flg = OTA_FW_CHECK_ERR;
    					}
					}
				}

                if(((0 == ota_adr) || ota_adr < ota_pkt_total - 1)){// 
                    if(need_check_type == FW_CHECK_AGTHM1){
						fw_check_val += (req->dat[18] | req->dat[19]<<8);
                    }else if(need_check_type == FW_CHECK_AGTHM2){
    					fw_check_val = get_crc32_16bytes(fw_check_val, req->dat + 2);
					}
                }
				
				//log_data(TR_24_ota_adr,ota_adr);
				if((ota_adr<<4)>=(ota_firmware_size_k<<10)){
					err_flg = OTA_FW_SIZE_ERR;
				}else{
					ota_save_data (ota_adr<<4, req->dat + 2, 16);

					u8 flash_check[16];

					flash_read_page(ota_program_offset + (ota_adr<<4),16,flash_check);

					if(!memcmp(flash_check,req->dat + 2,16)){  //OK
						ota_adr_index = ota_adr;
					}
					else{ //flash write err
						err_flg = OTA_WRITE_FLASH_ERR;
					}
				}
				
			}
			else{  //crc err
				err_flg = OTA_DATA_CRC_ERR;
			}
		}
		else if(ota_adr_index >= ota_adr){  //maybe repeated OTA data, we neglect it, do not consider it ERR

		}
		else{  //adr index err, missing at least one OTA data
			err_flg = OTA_DATA_PACKET_SEQ_ERR;
		}
		//log_task_end(TR_T_ota_data);

	}

	if(err_flg){
		//log_event(TR_T_ota_err);
		blt_ota_finished_flag_set(err_flg);
	}

	return 0;
}

int otaRead(void * p)
{
	return 0;
}

#define OTA_ADD_MORE_CHECK_BEFORE_ERASE_FM_BAKUP_AREA		1

/*
 * @retval: 1: there is legacy ota data need to be clear. 0: no legacy data.
*/
int bls_ota_clearNewFwDataArea(int check_only)
{
#if (OTA_ADD_MORE_CHECK_BEFORE_ERASE_FM_BAKUP_AREA)
		blt_ota_software_check_flash_load_error(); // if any errors are found, will reboot inside
#endif

		int legacy_flag = 0;
#if (APP_FLASH_PROTECTION_ENABLE)
		u8	flash_clear_trigger = 0;
#endif
		
#if 1 //new, safer method
		//When the OTA is successfully restarted to erase the old firmware,
		//the 0 and 2k addresses of each sector in the old firmware area are polled, a
		//nd the sector is erased if the 4-byte data read out is not 0xFFFFFFFF.
		//A special situation, the address of the last sector 0 in the old firmware area is just 0xFFFFFFFF,
		//and the end of the firmware is less than 2k,
		//so the last sector is not erased after OTA succeeds,
		//which may cause the next OTA to read and write flash data in this area inconsistent and fail.
		u32 tmp1 = 0;
		u32 tmp2 = 0;
		u32 tmp3 = 0;
		u32 tmp4 = 0;
		u32 erase_consecutive_cnt = 0;
		int cur_flash_setor;
		for(u32 i = 0; i < (ota_firmware_size_k>>2); ++i)
		{
			cur_flash_setor = ota_program_offset + i*0x1000;
	
			int cur_erase_trigger = 0;
			if(erase_consecutive_cnt){
				cur_erase_trigger = 1;
				erase_consecutive_cnt --;
			}
	
			flash_read_page(cur_flash_setor,		4, (u8 *)&tmp1);
			flash_read_page(cur_flash_setor + 1024, 4, (u8 *)&tmp2);
			flash_read_page(cur_flash_setor + 2048, 4, (u8 *)&tmp3);
			flash_read_page(cur_flash_setor + 4000, 4, (u8 *)&tmp4);
	
			if(tmp1 != ONES_32 || tmp2 != ONES_32 || tmp3 != ONES_32 || tmp4 != ONES_32)
			{
				legacy_flag = 1;
				if(check_only){
					break;
				}
				
				cur_erase_trigger = 1;
				erase_consecutive_cnt = 2;
			}
	
			if(cur_erase_trigger){
				#if (APP_FLASH_PROTECTION_ENABLE)
					if(!flash_clear_trigger && flash_prot_op_cb){
						flash_clear_trigger = 1;
						flash_prot_op_cb(FLASH_OP_EVT_STACK_OTA_CLEAR_OLD_FW_BEGIN, ota_program_offset, ota_program_offset + ota_firmware_size_k * 1024);
					}
				#endif
				#if(MODULE_WATCHDOG_ENABLE)
				wd_clear();	// prevent flash_erase_sector_ without clear.
				#endif
				flash_erase_sector(cur_flash_setor);
			}
		}
#endif
		
#if (APP_FLASH_PROTECTION_ENABLE)
		if(flash_clear_trigger){
			flash_prot_op_cb(FLASH_OP_EVT_STACK_OTA_CLEAR_OLD_FW_END, 0, 0);
		}
#endif

		return legacy_flag;
}


void bls_ota_procTimeout(void)
{
	blt_slave_ota_finish_handle();

	if(blt_ota_start_tick && clock_time_exceed(blt_ota_start_tick , blt_ota_timeout_us)){  //OTA timeout
		rf_link_slave_ota_finish_led_and_reboot(OTA_TIMEOUT);
	}
#if NMW_ENABLE
	if(blt_ota_start_tick && nmw_ota_state.seg_timeout_tick && clock_time_exceed(nmw_ota_state.seg_timeout_tick, NMW_OTA_SEG_WAIT_MS*1000)){
		nmw_ota_seg_report();
	}
	
	if(nmw_ota_state.errno_pending){
		nmw_ota_st_report(nmw_ota_state.op, nmw_ota_state.errno);
	}
#endif
}

void blt_ota_finished_flag_set(u8 reset_flag)
{
	if(blcOta.ota_start_flag && (blt_ota_finished_time == 0)){
		blt_ota_finished_flag = reset_flag;
		blt_ota_finished_time = clock_time()|1;

		u8 temp_buffer[sizeof(ota_result_t)];
		ota_result_t *pResult = (ota_result_t *)temp_buffer;
		pResult->ota_cmd = CMD_OTA_RESULT;
		pResult->result = reset_flag;
		pResult->rsvd = 0;
		LOG_MSG_LIB(TL_LOG_NODE_BASIC, 0, 0, "result:%d", pResult->result );
		bls_att_pushNotifyData(SERVICE_GATT_OTA_HANDLE, temp_buffer, sizeof(temp_buffer));
	}
	#if GATEWAY_ENABLE
	gateway_upload_gatt_ota_sts(reset_flag);
	#endif
}

void rf_link_slave_ota_finish_led_and_reboot(u8 st)
{
	if(OTA_SUCCESS == st){
        ota_set_flag ();
    }
    else{
       if(ota_adr_index>=0){
			irq_disable();

			//for(int i=0;i<=ota_adr_index;i+=256)
			for(int i=(ota_adr_index&0x3ff00); i>=0; i-=256) //erase from end to head
			{  //4K/16 = 256
				flash_erase_sector(ota_program_offset + (i<<4));
			}
		}
    }
	
	set_keep_onoff_state_after_ota();
	
#if (APP_FLASH_PROTECTION_ENABLE)
	app_flash_protection_ota_end(st);/* do it before led indication*/
#endif

	if(otaResIndicateCb){
		otaResIndicateCb(st);   // should be better at last.
	}
    irq_disable ();
    start_reboot();
}

void blt_slave_ota_finish_handle()		
{	
    if(blt_ota_finished_time){
        static u8 terminate_cnt;
        u8 reboot_flag = 0;
        if((0 == terminate_cnt) && (blt_ota_terminate_flag)){
               terminate_cnt = 6;
               bls_ll_terminateConnection(0x13);
        }
        
        if(terminate_cnt){
            terminate_cnt--;
            if(!terminate_cnt){
                reboot_flag = 1;
            }
        }
        
        if((!blt_ota_terminate_flag)
         &&((u32)(clock_time() - blt_ota_finished_time) > 2000*1000 * sys_tick_per_us)){
            blt_ota_terminate_flag = 1;    // for ios: no last read command
        }
        
        if(((u32)(clock_time() - blt_ota_finished_time) > 4000*1000 * sys_tick_per_us)){
            reboot_flag = 1;
        }
        
        if(reboot_flag){
            rf_link_slave_ota_finish_led_and_reboot(blt_ota_finished_flag);
            // have been reboot
        }
    }
}
#endif
