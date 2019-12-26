/*
 * ota_vendor_cmd.c
 *
 *  Created on: 2019-2-15
 *      Author: Administrator
 */

/******************************************************
 * 				ota vendor cmd
 *****************************************************/

#include "ota_vendor_cmd.h"
#include "../common.h"
#include "../../../homekit_src/hk_ble_ll_ota.h"
#include "homekit_src/hk_adv_packet.h"
#include "proj_lib/ble/blt_config.h"
#include "../hk_LightBulbService.h"
#include "vendor/common/cmd_interface.h"

extern void cfg_led_event(u32 e);


extern hkble_ota_startCb_t		hkble_otaStartCb;
extern int hkble_ota_adr_index;
extern u8 hkble_ota_data_check;
short app_get_rssi(void);

void CustVendorCmdFunctions(void *p)
{
    rf_packet_att_data_t *req = (rf_packet_att_data_t*)p;


    u16 CmdNo =  req->dat[0] | (req->dat[1]<<8);
    u8 *parameter = &(req->dat[2]);

    int nDataLen = req->l2cap - 5;

    memset(otabuffer, 0, 20);

    if(CmdNo >= 0xFF00){
        // for OTA cust. flow
        if(CmdNo == HKBLE_CMD_OTA_FW_VERSION){
            //to be add
            //log_event(TR_T_ota_version);
        }
        else if(CmdNo == HKBLE_CMD_OTA_START){
			extern hkble_ota_service_t hkble_blcOta;
			extern hkble_ota_service_t hkble_hk_blcOta;
			hkble_blcOta.ota_start_flag = 1;   //set flag
			hkble_hk_blcOta.ota_start_flag = 1;   //set flag
            if(hkble_otaStartCb){
                hkble_otaStartCb();
            }

			#if (HKBLE_OTA_FW_CHECK_EN)
			hkble_blcOta.fw_crc_last_index = 0xFFFF;
			hkble_hk_blcOta.fw_crc_last_index = 0xFFFF;
			hkble_ota_data_check = HKBLE_OTA_DATA_START;
			#endif
        }
        else if(CmdNo == HKBLE_CMD_OTA_END){
			u8 err_flg = HKBLE_OTA_SUCCESS;
			u16 adrIndex_max	   = req->dat[2] | (req->dat[3]<<8);
			u16 adrIndex_max_check = req->dat[4] | (req->dat[5]<<8);

			//if no index_max check, set ota success directly, otherwise we check if any index_max match
			if( req->l2cap == 9 && (adrIndex_max ^ adrIndex_max_check) == 0xffff){  //index_max valid, we can check
				if(adrIndex_max != hkble_ota_adr_index){  //last one or more packets missed
					err_flg = HKBLE_OTA_DATA_UNCOMPLETE;
				}
			}

			if(!err_flg){
				#if (!HKBLE_OTA_DATA_VERIFY_AFTER_SAVE_EN)
				hkble_ota_set_flag ();

				extern void dev_mac_addr_update(void);
				dev_mac_addr_update();
            	//R10 clear broadcast event param.
            	//blt_clean_broadcast_param();
            	bls_ll_terminateConnection(0x13);
				rf_led_ota_ok();
				start_reboot();
				hkble_ota_data_check = HKBLE_OTA_DATA_NONE;
				#else
		        //extern void hkble_ota_verify_data_from_flash(void);
				//hkble_ota_verify_data_from_flash();
				hkble_ota_data_check = HKBLE_OTA_DATA_END;
				#endif
			}else{
				if (hkble_ota_adr_index >= 0)
				{
					for(int i=0; i<=hkble_ota_adr_index; i+=256)
					{
						// 4K/16 = 256
						#if(__TL_LIB_8258__ || (MCU_CORE_TYPE && MCU_CORE_TYPE == MCU_CORE_8258))
						extern u32 ota_program_offset;
						flash_erase_sector(ota_program_offset + (i<<4));
						#else
						flash_erase_sector(NEW_FW_ADR + (i<<4));
						#endif
					}
				}
				bls_ll_terminateConnection(0x13);
				rf_led_ota_error();
				start_reboot();
			}

        }
    }else if(CmdNo >= 0xFE00)
    {
        //just for store and restore mac addr, serial num, setup code.
        enum{
            CMD_SET_MAC_ADDR            = (0xFE00),
            CMD_GET_MAC_ADDR            = (0xFE01),
            CMD_SET_SERIAL_NUM          = (0xFE02),
            CMD_GET_SERIAL_NUM          = (0xFE03),
            CMD_SET_SETUP_CODE          = (0xFE04),
            CMD_GET_SETUP_CODE          = (0xFE05),
            CMD_SET_SETUP_ID            = (0xFE06),
            CMD_GET_SETUP_ID            = (0xFE07),
			CMD_GET_MFI_I2C_ID			= (0xFE08),
			CMD_TEST_LED				= (0xFE09),
			CMD_GET_DEVICE_ID			= (0xFE0A),
			CMD_GET_HASH_ID				= (0xFE0B),
			CMD_GET_RSSI				= (0xFE0C),

        };

        enum{
            CMD_SET_CUSTOM_VALUE_SUCCESS    = (0x0),
            CMD_SET_CUSTOM_VALUE_ERROR      = (0x01),
            CMD_SET_CUSTOM_VALUE_REJECT     = (0x02),
        };

        u8 data[12];

        memcpy(data, &req->dat[0], 2);

        switch(CmdNo)
        {
            #if MAC_ADDR_RANDOM_ENABLE
            case CMD_SET_MAC_ADDR:
                if(nDataLen == 6)
                {
                    u32 *p_mac = (u32 *) flash_adr_layout.flash_adr_mac;

                    if (*p_mac != 0xffffffff)
                    {
                        flash_erase_sector(flash_adr_layout.flash_adr_mac);
                    }

                    flash_write_page(flash_adr_layout.flash_adr_mac, 6, parameter);
                    memcpy(tbl_mac, parameter, 6);

                    data[2] = CMD_SET_CUSTOM_VALUE_SUCCESS;
                    memcpy(otabuffer, data, 3);
                }
                break;

            case CMD_GET_MAC_ADDR:
                if(nDataLen == 0)
                {
                    memcpy(data + 2, tbl_mac, 6);
                    memcpy(otabuffer, data, 8);
                }
                break;
            #endif

            #if SERIAL_NUMBER_RANDOM_ENABLE
            case CMD_SET_SERIAL_NUM:
                if(nDataLen == 10)
                {

                    u32 *p_serial_num = (u32 *) flash_adr_layout.flash_adr_serial_number;

                    if (*p_serial_num != 0xffffffff)
                    {
                        flash_erase_sector(flash_adr_layout.flash_adr_serial_number);
                    }

                    flash_write_page(flash_adr_layout.flash_adr_serial_number, sizeof(serialNumber), parameter);

                    memcpy(serialNumber, parameter, sizeof(serialNumber));

                    data[2] = CMD_SET_CUSTOM_VALUE_SUCCESS;
                    memcpy(otabuffer, data, 3);
                }
                break;

            case CMD_GET_SERIAL_NUM:
                if(nDataLen == 0)
                {
                    memcpy(data+2, serialNumber, sizeof(serialNumber));
                    memcpy(otabuffer, data, (2 + sizeof(serialNumber)));
                }
                break;
            #endif

            #if SETUP_CODE_RANDOM_ENABLE
            case CMD_SET_SETUP_CODE:
                if(nDataLen == 4)
                {
                    if(SetupCodeWriteCnts < SETUP_CODE_WRITE_CNTS_MAX)
                    {
                        u32 *p_setup_code = (u32 *) flash_adr_layout.flash_adr_setupcode;

                        if (*p_setup_code != 0xffffffff)
                        {
                            flash_erase_sector(flash_adr_layout.flash_adr_setupcode);
                        }

                        SetupCodeWriteCnts++;
                        parameter[4] = SetupCodeWriteCnts;
                        flash_write_page(flash_adr_layout.flash_adr_setupcode, 5, parameter);
                        memcpy((u8 *)&SetupCodeData, parameter, 4);
                        if(SetupCodeReadCnts > 0)
                            flash_subregion_write_val(flash_adr_layout.flash_adr_setupcode_read_cnt, 0, SetupCodeReadCnts, SETUP_CODE_READ_CNTS_MAX);

                        data[2] = CMD_SET_CUSTOM_VALUE_SUCCESS;

                        /* execute factory reset.*/
                        irq_disable();
                        factory_reset();
						
						#if HK_SALTCHAR_RANDOM_FOR_SRP_ENABLE
						flash_erase_sector(flash_adr_layout.flash_adr_saltchar);
						#endif
						
                        irq_disable ();
                        REG_ADDR8(0x6F) = 0x20; // rst all(act as power on reset)
                        while (1);
                    }
                    else
                    {
                        /*exceed the range of times of write setup code*/
                        data[2] = CMD_SET_CUSTOM_VALUE_REJECT;
                    }
                    memcpy(otabuffer, data, 3);
                }
                break;

            case CMD_GET_SETUP_CODE:
                if(nDataLen == 0)
                {
                    if(SetupCodeReadCnts < SETUP_CODE_READ_CNTS_MAX)
                    {
                        SetupCodeReadCnts++;
                        flash_subregion_write_val(flash_adr_layout.flash_adr_setupcode_read_cnt, 0, SetupCodeReadCnts, SETUP_CODE_READ_CNTS_MAX);
                        memcpy(data + 2, (u8 *)&SetupCodeData, 4);
                        memcpy(otabuffer, data, 6);
                    }
                    else
                    {
                        /*exceed the range of times of read setup code*/
                        data[2] = CMD_SET_CUSTOM_VALUE_REJECT;
                        memcpy(otabuffer, data, 3);
                    }
                }
                break;
            #endif

            #if SETUP_ID_RANDOM_ENABLE
            case CMD_SET_SETUP_ID:
                if(nDataLen == 4)
                {
                    u32 *p_setup_id = (u32 *) flash_adr_layout.flash_adr_setup_id;

                    if (*p_setup_id != 0xffffffff)
                    {
                        flash_erase_sector(flash_adr_layout.flash_adr_setup_id);
                    }

                    flash_write_page(flash_adr_layout.flash_adr_setup_id, 4, parameter);
                    memcpy((u8 *)&SetupIdData, parameter, 4);
                    data[2] = CMD_SET_CUSTOM_VALUE_SUCCESS;
                    memcpy(otabuffer, data, 3);

                    // apply new setup ID to generate new setup HASH code.
                    dev_hash_deviceid_set();
                    u8 hk_setupHash[4];
                    flash_read_page((u32)(flash_adr_layout.flash_adr_hash_id), 4, hk_setupHash);
                    blt_adv_update_pkt(ADV_PKT_SETUP_HASH, hk_setupHash);
                }
                break;

            case CMD_GET_SETUP_ID:
                if(nDataLen == 0)
                {
                    memcpy(data + 2, (u8 *)&SetupIdData, 4);
                    memcpy(otabuffer, data, 6);
                }
                break;
            #endif

			case CMD_GET_MFI_I2C_ID:
                if(nDataLen == 0)
                {
                	u8 dev_id[4] = {0};
					if(app_verify_mfi_i2c(dev_id))
					{
						data[2] = CMD_SET_CUSTOM_VALUE_SUCCESS;
						memcpy(data + 3, dev_id, 4);
						memcpy(otabuffer, data, 7);
					}
					else
					{
						data[2] = CMD_SET_CUSTOM_VALUE_ERROR;
						memcpy(otabuffer, data, 3);
					}
				}
				break;

			case CMD_TEST_LED:
                if(nDataLen == 1)
                {
					#define LED_OFF		0
                	#define LED_ON		1
					#define LED_FLASH	2

					if((parameter[0] == LED_OFF) || (parameter[0] == LED_ON))
					{
						access_cmd_onoff(ele_adr_primary, 0, parameter[0], 0, 0);
					}
					else if(parameter[0] == LED_FLASH)
					{
						cfg_led_event(LED_EVENT_FLASH_2HZ_5TIMES);
					}
					data[2] = parameter[0];
					data[3] = CMD_SET_CUSTOM_VALUE_SUCCESS;
                    memcpy(otabuffer, data, 4);
                }
				break;

			case CMD_GET_DEVICE_ID:
                if(nDataLen == 0)
                {
					u8 hk_device_id[6];
					flash_read_page((u32)flash_adr_layout.flash_adr_device_id, 6, hk_device_id);
                    memcpy(data + 2, hk_device_id, 6);
                    memcpy(otabuffer, data, 8);
                }
				break;

			case CMD_GET_HASH_ID:
                if(nDataLen == 0)
                {
					u8 hk_setupHash[4];
					flash_read_page((u32)(flash_adr_layout.flash_adr_hash_id), 4, hk_setupHash);
					memcpy(data + 2, hk_setupHash, 4);
                    memcpy(otabuffer, data, 6);
                }
				break;
			case CMD_GET_RSSI:
				if(nDataLen == 0)
				{
					short rssi_tmp = app_get_rssi();
					memcpy(data + 2, (u8 *)&rssi_tmp, 2);
					memcpy(otabuffer, data, 4);
				}
				break;

            default:
                break;
        }
    }else if(CmdNo >= 0xFC00){

        enum{
            CMD_CUST00_GET_PRJ_MODEL    = (0xFC00),
            CMD_CUST01                  = (0xFC01),
            CMD_CUST02                  = (0xFC02),
            CMD_CUST03                  = (0xFC03),
            CMD_CUST04                  = (0xFC04),
            CMD_CUST05                  = (0xFC05),
            CMD_CUST99_REBOOT           = (0xFC99),
        };

        if(CmdNo == CMD_CUST00_GET_PRJ_MODEL){
            const char PRJ_MODEL[]="MODEL_201709";
            memcpy(otabuffer, PRJ_MODEL, (sizeof(PRJ_MODEL)-1));
        }else if(CmdNo == CMD_CUST01){
            ;
        }else if(CmdNo == CMD_CUST02){
            ;
            //-printf("got CMD_CUST02(%d)\n",nDataLen);
        }else if(CmdNo == CMD_CUST03){
            ;
            //-printf("got CMD_CUST03(%d)\n",nDataLen);
        }else if(CmdNo == CMD_CUST04){
            ;
            //-printf("got CMD_CUST04(%d)\n",nDataLen);
        }else if(CmdNo == CMD_CUST05){
            if(nDataLen > 0)
                memcpy(otabuffer, parameter, nDataLen);
            //-printf("got CMD_CUST05(%d)\n",nDataLen);
            //-for(; nDataLen>0; nDataLen--)
            //-    printf("%2X\t", parameter[nDataLen-1]);
            //-printf("\n");
        }else if(CmdNo == CMD_CUST99_REBOOT){
            //R10 clear broadcast event param.
            blt_clean_broadcast_param();
            rf_led_ota_ok();
            start_reboot();
        }
    }
}
