/********************************************************************************************************
 * @file     app.c 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
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
#include "proj/tl_common.h"
#include "proj_lib/rf_drv.h"
#include "proj_lib/pm.h"
#include "proj_lib/ble/ll/ll.h"
#include "proj_lib/ble/blt_config.h"
#include "proj_lib/ble/ll/ll_whitelist.h"
#include "proj_lib/ble/trace.h"
#include "proj/mcu/pwm.h"
#include "proj_lib/ble/service/ble_ll_ota.h"
#include "proj/drivers/adc.h"
#include "proj_lib/ble/blt_config.h"
#include "proj_lib/ble/ble_smp.h"
#include "proj_lib/mesh_crypto/mesh_crypto.h"
#include "proj_lib/mesh_crypto/mesh_md5.h"

#include "proj_lib/sig_mesh/app_mesh.h"
#include "../common/app_provison.h"
#include "../common/app_beacon.h"
#include "../common/app_proxy.h"
#include "../common/app_health.h"
#include "proj/drivers/keyboard.h"
#include "app.h"
#include "stack/ble/gap/gap.h"
#include "vendor/common/blt_soft_timer.h"
#include "proj/drivers/rf_pa.h"

#if (HCI_ACCESS==HCI_USE_UART)
#include "proj/drivers/uart.h"
#endif

/*homekit include file.*/
#include "common.h"
#include "hk_TopUUID.h"
#include "hk_CategoryDef.h"
#include "homekit_src/hk_adv_packet.h"
#include "homekit_src/hk_indicate_queue.h"
#include "homekit_src/hk_l2cap_handle.h"

#include "./mfi/mfi.h"
#include "./ota_vendor_cmd/ota_vendor_cmd.h"
#include "./led_event/led_ctrl.h"
#include "homekit_src/hk_ble_ll_ota.h"
#include "homekit_src/homekit_inc.h"
#include "homekit_src/hk_att_extend.h"
#include "app_att.h"


#if(__PROJECT_MESH_PRO__)
MYFIFO_INIT(blt_rxfifo, 64, 8);
MYFIFO_INIT(blt_txfifo, 40, 4);
#else
MYFIFO_INIT(blt_rxfifo, 64, 16);
MYFIFO_INIT(blt_txfifo, 40, 32);
#endif

//u8		peer_type;
//u8		peer_mac[12];

#if MD_SENSOR_EN
STATIC_ASSERT(FLASH_ADR_MD_SENSOR != FLASH_ADR_VC_NODE_INFO);   // address conflict
#endif
//////////////////////////////////////////////////////////////////////////////
//	Initialization: MAC address, Adv Packet, Response Packet
//////////////////////////////////////////////////////////////////////////////

/************************************************************************
 * 					procedure
 ***********************************************************************/

/*homekit define*/
#define 	ADV_IDLE_ENTER_DEEP_TIME			60  //60 s
#define 	CONN_IDLE_ENTER_DEEP_TIME			60  //60 s

#define 	MY_DIRECT_ADV_TMIE					2000000


#define     MY_APP_ADV_CHANNEL					BLT_ENABLE_ADV_37
#define 	MY_ADV_INTERVAL_MIN					ADV_INTERVAL_30MS
#define 	MY_ADV_INTERVAL_MAX					ADV_INTERVAL_35MS
#define 	MY_DISCONN_EVT_ADV_INTERVAL_MIN		ADV_INTERVAL_20MS
#define 	MY_DISCONN_EVT_ADV_INTERVAL_MAX		ADV_INTERVAL_20MS

#define		MY_RF_POWER_INDEX					RF_POWER_P3p01dBm


#define		BLE_DEVICE_ADDRESS_TYPE 			BLE_DEVICE_ADDRESS_RANDOM_STATIC

_attribute_data_retention_	own_addr_type_t 	app_own_address_type = OWN_ADDRESS_RANDOM;

#if 0
#define RX_FIFO_SIZE	64
#define RX_FIFO_NUM		8

#define TX_FIFO_SIZE	40
#define TX_FIFO_NUM		16


#if 0
	MYFIFO_INIT(blt_rxfifo, RX_FIFO_SIZE, RX_FIFO_NUM);
#else
_attribute_data_retention_  u8 		 	blt_rxfifo_b[RX_FIFO_SIZE * RX_FIFO_NUM] = {0};
_attribute_data_retention_	my_fifo_t	blt_rxfifo = {
												RX_FIFO_SIZE,
												RX_FIFO_NUM,
												0,
												0,
												blt_rxfifo_b,};
#endif


#if 0
	MYFIFO_INIT(blt_txfifo, TX_FIFO_SIZE, TX_FIFO_NUM);
#else
	_attribute_data_retention_  u8 		 	blt_txfifo_b[TX_FIFO_SIZE * TX_FIFO_NUM] = {0};
	_attribute_data_retention_	my_fifo_t	blt_txfifo = {
													TX_FIFO_SIZE,
													TX_FIFO_NUM,
													0,
													0,
													blt_txfifo_b,};
#endif
#endif

/************************************************************************
*		    Variables Definitions/Declarations
 ***********************************************************************/
_attribute_data_retention_	u32 broadcast_encryption_time_timeout = 0;
_attribute_data_retention_	u32 disconnect_event_time_timeout = 0;
_attribute_data_retention_ extern u8 blt_adv_global_state_modify_once;

_attribute_data_retention_	extern u8 EventNotifyCNT;

_attribute_data_retention_	u32 store_char_change_value_timeout= 0;

//_attribute_data_retention_	u32 led_event_pending;

_attribute_data_retention_	short rssi = 0;

_attribute_data_retention_	volatile u32 flash_char_value_addr = 0;

_attribute_data_retention_	store_char_change_value_t *char_save_restore = 0;

_attribute_data_retention_ u8 ReCheckADVStatusFlag = 1;

_attribute_data_retention_ u8 ReCheckAndRestoreFlashCharValueFlag = 0;

_attribute_data_retention_ u8 DoBroadcastEncrypDataCALL_flag = 0;

extern st_ll_conn_slave_t		bltc;
extern u8 blt_pair_ok;

extern void hkble_ota_set_flag(void);
extern void start_reboot(void);

extern u8 hkble_ota_data_check;

extern u32 session_timeout_tick;
extern u32 hap_transationID_valid_timeout;
extern u32 hap_timed_write_timeout;
extern u32 hap_procedure_valid_timeout;
extern u32 hap_remove_pairing_timeout;

u8 hk_setupHash[4];
extern void cfg_led_event(u32 e);
void blc_initMacAddress(int flash_addr, u8 *mac_public, u8 *mac_random_static);

/************************************************************************
*	 Adv Packet, Response Packet
 ***********************************************************************/
const u8	tbl_advData[] = {
	0x05, 0x09, 't', 'H', 'I', 'D',
	0x02, 0x01, 0x05, 							// BLE limited discoverable mode and BR/EDR not supported
	0x03, 0x19, 0x80, 0x01, 					// 384, Generic Remote Control, Generic category
	0x05, 0x02, 0x12, 0x18, 0x0F, 0x18,		// incomplete list of service class UUIDs (0x1812, 0x180F)
};

// homekit scan rsp
const u8 tbl_rsp[] = { 0x0, 6+31, 0xef, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5,	\
	                    0x1e,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


_attribute_data_retention_	int device_in_connection_state;

_attribute_data_retention_	u32 advertise_begin_tick;

_attribute_data_retention_	u32	interval_update_tick;

_attribute_data_retention_	u8	sendTerminate_before_enterDeep = 0;

_attribute_data_retention_	u32	latest_user_event_tick;

/************************************************************************
 * 					procedure
 ***********************************************************************/
#if 0
void cfg_led_event(u32 e)
{
    led_event_pending = e;
}
#endif
/************************************************************************
 * 					LUM ADJUST
 ***********************************************************************/

//save characteristic value, if the value was updated for a while
u8 dev_characteristic_store_to_flash(void){

    store_char_change_value_t char_save_store;

    flash_char_value_addr += sizeof(store_char_change_value_t);

    if((flash_char_value_addr >= (u32)(flash_adr_layout.flash_adr_char_value + FLASH_SECTOR_SIZE - sizeof(store_char_change_value_t)))){
        flash_erase_sector((u32)flash_adr_layout.flash_adr_char_value);
        flash_char_value_addr = (u32)flash_adr_layout.flash_adr_char_value;
    }else{
        flash_char_value_addr -= sizeof(store_char_change_value_t);
        u8 clear_save_flag = 0;
        flash_write_page(flash_char_value_addr, 1, (u8 *)(&clear_save_flag));
         flash_char_value_addr += sizeof(store_char_change_value_t);
    }

    STORE_CHAR_TO_FLASH

    flash_write_page(flash_char_value_addr, sizeof(char_save_store), (u8 *)(&char_save_store));

    return 1;
}


void dev_char_val_update_from_flash(void)
{
	int i;
	unsigned char do_effect_flag = 0;

	for(i = 0; i < (FLASH_SECTOR_SIZE); i += sizeof(store_char_change_value_t)){
		flash_char_value_addr = (u32)flash_adr_layout.flash_adr_char_value + i;

		char_save_restore = (store_char_change_value_t *)(flash_char_value_addr);
		if(0 == char_save_restore->save_flag){
			continue;
		}
		else if(CHAR_SAVE_FLAG == char_save_restore->save_flag)
		{
			RESTORE_CHAR_FROM_FLASH
			do_effect_flag = 1;
			break;
		}
//- 	else if(0x1F != char_save_restore->save_flag)
//- 	{
//- 		//invalid
//- 		continue;
//- 	}
		else
		{
			break;
		}
	}


	if(do_effect_flag == 0)
	{
		dev_characteristic_store_to_flash();
	}
}


//retrieve device characteristic value from flash
void dev_char_val_restore_from_flash(void){

	dev_char_val_update_from_flash();

    //effect
//--    if(do_effect_flag)
//--    {
//--        do_effect_flag = 0;
        /////////////////////////////////
        // do effect action here!
        /////////////////////////////////
#if LIGHTBULB_SERVICE_ENABLE
        extern void Convert_HSV2RGB(u32 * dbg);
        u32 dbg2 = 0;
        Convert_HSV2RGB(&dbg2);
		access_cmd_onoff(ele_adr_primary, 0, light_onoff, 0, 0);
#endif

#if LOCKMECHANISM_SERVICE_ENABLE
        Doorlock_Proc_Flg = 1;
        //-doorlock_proc();
#endif
//--    }
}

void assign_current_flash_char_value_addr(void)
{
    volatile u32 temp_flash_char_value_addr = (u32)flash_adr_layout.flash_adr_char_value;

    int i;
    for(i = 0; i < (FLASH_SECTOR_SIZE); i += sizeof(store_char_change_value_t)) {
        temp_flash_char_value_addr = (u32)flash_adr_layout.flash_adr_char_value + i;

        char_save_restore = (store_char_change_value_t *)(temp_flash_char_value_addr);
        if (0 == char_save_restore->save_flag){
            continue;
        } else if (CHAR_SAVE_FLAG == char_save_restore->save_flag) {
            #if 0   // for dbg
            static volatile store_char_change_value_t* light_brightness_addr = (store_char_change_value_t*) flash_adr_layout.flash_adr_char_value;
            light_brightness_addr = char_save_restore;
            #endif

            break;
//-        } else if (0x1F != char_save_restore->save_flag) {
//-            //invalid
//-            continue;
        } else {
            break;
        }
    }
}

void store_char_value_to_flash(void)
{
    store_char_change_value_timeout = clock_time()|1;
}

void set_broadcast_adv_interval(u8 bc_param)
{
	u16 interval = 0;
	enum{
		BROADCAST_INTERVAL_20MS			= 0x01,
		BROADCAST_INTERVAL_1280MS		= 0x02,
		BROADCAST_INTERVAL_2560MS		= 0x03,
	};
	
	bc_param &= ~(BIT(7));
	
	if(bc_param == BROADCAST_INTERVAL_20MS)
	{
		interval = ADV_INTERVAL_20MS;
	}
	else if(bc_param == BROADCAST_INTERVAL_1280MS)
	{
		interval = ADV_INTERVAL_1_28_S;
	}
	else if(bc_param == BROADCAST_INTERVAL_2560MS)
	{
		interval = ADV_INTERVAL_2_56_S;
	}
	else
	{
		interval = ADV_INTERVAL_20MS;	//default value.
	}

	bls_ll_setAdvInterval(interval, interval);
}

void proc_broadcast_event()
{

	if( broadcast_encryption_time_timeout && clock_time_exceed(broadcast_encryption_time_timeout,3*1100*1000) ){
		broadcast_encryption_time_timeout = 0;

		//	TODO:  reset adv packet to normal adv data.
		blt_set_normal_adv_pkt(HK_CATEGORY, DEVICE_SHORT_NAME);
		bls_ll_setAdvInterval(MY_DISCONN_EVT_ADV_INTERVAL_MIN, MY_DISCONN_EVT_ADV_INTERVAL_MAX); // disconnect event adv interval = 20ms
		disconnect_event_time_timeout = clock_time() | 1;			
	}

	if(disconnect_event_time_timeout && clock_time_exceed(disconnect_event_time_timeout, 3*1100*1000)){
		disconnect_event_time_timeout = 0;
		bls_ll_setAdvInterval(MY_ADV_INTERVAL_MIN, MY_ADV_INTERVAL_MAX);
	}
}

/************************************************************************
*							MAC  ADDR  SET
 ***********************************************************************/
/*user should update the mac addr after firmware update.*/
void dev_mac_addr_update(void)
{
	flash_erase_sector((u32)flash_adr_layout.flash_adr_mac);
	/*mac addr generate by randomly*/
	u8  mac_random_static[6];
	blc_initMacAddress(CFG_ADR_MAC, tbl_mac, mac_random_static);

#if(BLE_DEVICE_ADDRESS_TYPE == BLE_DEVICE_ADDRESS_PUBLIC)
		app_own_address_type = OWN_ADDRESS_PUBLIC;
#elif(BLE_DEVICE_ADDRESS_TYPE == BLE_DEVICE_ADDRESS_RANDOM_STATIC)
		app_own_address_type = OWN_ADDRESS_RANDOM;
		blc_ll_setRandomAddr(mac_random_static);
#endif

}

/************************************************************************
*						SETUP	CODE 	SET
 ***********************************************************************/
void dev_setup_code_set(void)
{
#if SETUP_CODE_RANDOM_ENABLE

    volatile unsigned char * p_setup_code = (flash_adr_layout.flash_adr_setupcode);

    if((p_setup_code[0]==0xFF)&&(p_setup_code[1]==0xFF)&&(p_setup_code[2]==0xFF)&&(p_setup_code[3]==0xFF))
    {
        u8 bits;
        u16 temp_data1 = myrand();
        u16 temp_data2 = myrand();
        bool setup_code_valid = true;

        foreach(i, 4)
        {
            bits = ((temp_data1 >> (i * 4)) & 0x0F);
            if(bits >= 0x0A)
            {
                temp_data1 &= ~(0x0F << (i * 4));
                temp_data1 |= ((bits - 0x0A) << (i * 4));
            }
            bits = ((temp_data2 >> (i * 4)) & 0x0F);
            if(bits >= 0x0A)
            {
                temp_data2 &= ~(0x0F << (i * 4));
                temp_data2 |= ((bits - 0x0A) << (i * 4));
            }
        }

        /* verify the randomly generated setup code if match with the request of HAP or not. */
        if(temp_data1 == temp_data2)
        {
            if(((temp_data1 >> 0) & 0xFF) == ((temp_data1 >> 8) & 0xFF))
                if(((temp_data1 >> 0) & 0x0F) == ((temp_data1 >> 4) & 0x0F))
                    setup_code_valid = false;
        }
        else
        {
            if(((((temp_data1 >> 0) & 0xFF) == 0x21) && (((temp_data1 >> 8) & 0xFF) == 0x43) && \
                (((temp_data2 >> 0) & 0xFF) == 0x65) && (((temp_data2 >> 8) & 0xFF) == 0x87)) || \
                ((((temp_data1 >> 0) & 0xFF) == 0x78) && (((temp_data1 >> 8) & 0xFF) == 0x56) && \
                (((temp_data2 >> 0) & 0xFF) == 0x34) && (((temp_data2 >> 8) & 0xFF) == 0x12)))
            {
                setup_code_valid = false;
            }
        }

        if(!setup_code_valid)
        {
            temp_data1 += 1;

            bits = ((temp_data1 >> 0) & 0x0F);
            if(bits >= 0x0A)
            {
                temp_data1 &= ~(0x0F << 0);
                temp_data1 |= ((bits - 0x0A) << 0);
            }
        }

        SetupCodeData = ((temp_data1 << 0 ) | (temp_data2 << 16 ));
        flash_write_page((u32)p_setup_code, 4, (u8 *)&SetupCodeData);

    }
    else
    {
        memcpy ((u8 *)&SetupCodeData, p_setup_code, 4);

        u8 wr_cnts = 0;
        flash_read_page(flash_adr_layout.flash_adr_setupcode_write_cnt, 1, &wr_cnts);
        SetupCodeWriteCnts = ((wr_cnts == 0xFF) ? 0 : wr_cnts);

        SetupCodeReadCnts = flash_subregion_read_val(flash_adr_layout.flash_adr_setupcode_read_cnt, 0, SETUP_CODE_READ_CNTS_MAX);
     }

#endif
}

/************************************************************************
*						SERIAL NUMBER SET
 ***********************************************************************/
void dev_serial_number_set(void)
{
#if SERIAL_NUMBER_RANDOM_ENABLE

    u32 *p_serial_num = (u32 *) flash_adr_layout.flash_adr_serial_number;

    if (*p_serial_num == 0xffffffff)
    {
        u8 byte;
        u8 sn_data;
        u16 data_array[5];

        /*0~9 : (0x30 - 0x39), A~Z : (0x41 - 0x5A), a~z : (0x61 - 0x7A)
          *serial number consist of Arabic numerals and uppercase and lowercase
          *that add up to 62(10 + 26 * 2).
          */

        foreach(i, 5)
        {
            data_array[i] = myrand();

            foreach(j, 2)
            {
                byte = ((data_array[i] >> (j * 8)) & 0xFF);
                sn_data = byte;

                if(byte < '0')
                {
                    sn_data = byte % 9 + '0';
                }
                else if((byte > '9') && (byte < 'A'))
                {
                    sn_data = byte % 9 + '0';
                }
                else if((byte > 'Z') && (byte < 'a'))
                {
                    sn_data = byte % 26 + 'A';
                }
                else if(byte > 'z')
                {
                    sn_data = byte % 26 + 'a';
                }
                //lowercase --> uppercase
                if((sn_data >= 'a') && (sn_data <= 'z'))
                {
                    sn_data -= 32;
                }

                data_array[i] &= ~(0xFF << (j * 8));
                data_array[i] |= (sn_data << (j * 8));
            }
            memcpy(serialNumber + (i * 2), (u8 *)&data_array[i], 2);
        }
        flash_write_page(flash_adr_layout.flash_adr_serial_number, sizeof(serialNumber), serialNumber);

    }
    else
    {
        flash_read_page(flash_adr_layout.flash_adr_serial_number, sizeof(serialNumber), serialNumber);
    }

#endif
}

/************************************************************************
*							SETUP ID SET
 ***********************************************************************/
void dev_setup_id_set(void)
{
#if SETUP_ID_RANDOM_ENABLE

    volatile unsigned char * p_setup_id = (flash_adr_layout.flash_adr_setup_id);

    if((p_setup_id[0]==0xFF)&&(p_setup_id[1]==0xFF)&&(p_setup_id[2]==0xFF)&&(p_setup_id[3]==0xFF))
    {
        u8 byte1;
        u8 setup_id_data;
        u16 data_array1[2];

        foreach(i, 2)
        {
            data_array1[i] = myrand();

            foreach(j, 2)
            {
                byte1 = ((data_array1[i] >> (j * 8)) & 0xFF);
                setup_id_data = byte1;

                if(byte1 < '0')
                {
                    setup_id_data = byte1 % 9 + '0';
                }
                else if((byte1 > '9') && (byte1 < 'A'))
                {
                    setup_id_data = byte1 % 9 + '0';
                }
                else if(byte1 > 'Z')
                {
                    setup_id_data = byte1 % 26 + 'A';
                }
                data_array1[i] &= ~(0xFF << (j * 8));
                data_array1[i] |= (setup_id_data << (j * 8));
            }
        }

        SetupIdData = ((data_array1[0] << 0) | (data_array1[1] << 16));
        flash_write_page((u32)p_setup_id, 4, (u8 *)&SetupIdData);
    }
    else
    {
        memcpy((u8 *)&SetupIdData, p_setup_id, 4);
    }

#endif
}

/************************************************************************
*						   SW_AUTHENTICATION ENABLE
 ***********************************************************************/
#if SOFTWARE_AUTHENTICATION_ENABLE
/*62DAD088-284B-4695-89FE-D650299BED25*/
u8 test_sw_uuid[] = {0x25, 0xED, 0x9B, 0x29, 0x50, 0xD6, 0xFE, 0x89,
						0x95, 0x46, 0x4B, 0x28, 0x88, 0xD0, 0xDA, 0x62};
/*
base64 encoded data:
MYGrMFACAQECAQEESDBGAiEA62fVk+Nerna7RHqpQ4AkpAlPcs7wsPSjX+rl++HCq5sCIQD9zklf9TYOYbKB9dul6raV52/BLHuvKFdWWVBFjbPzVzBXAgECAgEBBE8xTTAJAgFmAgEBBAEBMBACAWUCAQEECHmfK6NqAQAAMBQCAgDJAgEBBAsyNTAwMzMtMDAzNjAYAgFnAgEBBBASULBUjdVJpKhMe/fP4tOz

base64 decoded data:
*/
u8 test_sw_token[] = {
0x31, 0x81, 0xAB, 0x30, 0x50, 0x02, 0x01, 0x01, 0x02, 0x01, 0x01, 0x04, 0x48, 0x30, 0x46, 0x02,
0x21, 0x00, 0xEB, 0x67, 0xD5, 0x93, 0xE3, 0x5E, 0xAE, 0x76, 0xBB, 0x44, 0x7A, 0xA9, 0x43, 0x80,
0x24, 0xA4, 0x09, 0x4F, 0x72, 0xCE, 0xF0, 0xB0, 0xF4, 0xA3, 0x5F, 0xEA, 0xE5, 0xFB, 0xE1, 0xC2,
0xAB, 0x9B, 0x02, 0x21, 0x00, 0xFD, 0xCE, 0x49, 0x5F, 0xF5, 0x36, 0x0E, 0x61, 0xB2, 0x81, 0xF5,
0xDB, 0xA5, 0xEA, 0xB6, 0x95, 0xE7, 0x6F, 0xC1, 0x2C, 0x7B, 0xAF, 0x28, 0x57, 0x56, 0x59, 0x50,
0x45, 0x8D, 0xB3, 0xF3, 0x57, 0x30, 0x57, 0x02, 0x01, 0x02, 0x02, 0x01, 0x01, 0x04, 0x4F, 0x31,
0x4D, 0x30, 0x09, 0x02, 0x01, 0x66, 0x02, 0x01, 0x01, 0x04, 0x01, 0x01, 0x30, 0x10, 0x02, 0x01,
0x65, 0x02, 0x01, 0x01, 0x04, 0x08, 0x79, 0x9F, 0x2B, 0xA3, 0x6A, 0x01, 0x00, 0x00, 0x30, 0x14,
0x02, 0x02, 0x00, 0xC9, 0x02, 0x01, 0x01, 0x04, 0x0B, 0x32, 0x35, 0x30, 0x30, 0x33, 0x33, 0x2D,
0x30, 0x30, 0x33, 0x36, 0x30, 0x18, 0x02, 0x01, 0x67, 0x02, 0x01, 0x01, 0x04, 0x10, 0x12, 0x50,
0xB0, 0x54, 0x8D, 0xD5, 0x49, 0xA4, 0xA8, 0x4C, 0x7B, 0xF7, 0xCF, 0xE2, 0xD3, 0xB3
};

#endif

#if SOFTWARE_AUTHENTICATION_ENABLE
void dev_sw_auth_token_set(u8 set_flag, u8 * token_data, u16 token_len)
{

	bool store_to_flash = true;
	u16 flash_flag;

	if(set_flag == SW_AUTH_TOKEN_INIT)
	{
		flash_read_page(flash_adr_layout.flash_adr_sw_token, 2, (u8 *)&flash_flag);
		if(flash_flag == FLASH_FLAG_VALID)
		{
			store_to_flash = false;
		}
	}

	if(store_to_flash)
	{
		flash_erase_sector(flash_adr_layout.flash_adr_sw_token);
		flash_write_page(flash_adr_layout.flash_adr_sw_token + 16, token_len, token_data);
		flash_flag = FLASH_FLAG_VALID;
		flash_write_page(flash_adr_layout.flash_adr_sw_token, 2, &flash_flag);
		flash_write_page(flash_adr_layout.flash_adr_sw_token + 2, 2, &token_len);
	}

	return;
}

void dev_sw_auth_uuid_set(u8 set_flag, u8 * auth_uuid)
{

	bool store_to_flash = true;
	u16 flash_flag;

	if(set_flag == SW_AUTH_UUID_INIT)
	{
		flash_read_page(flash_adr_layout.flash_adr_sw_uuid, 2, (u8 *)&flash_flag);
		if(flash_flag == FLASH_FLAG_VALID)
		{
			store_to_flash = false;
		}
	}

	if(store_to_flash)
	{
		flash_erase_sector(flash_adr_layout.flash_adr_sw_uuid);
		flash_write_page(flash_adr_layout.flash_adr_sw_uuid + 16, 16, auth_uuid);
		flash_flag = FLASH_FLAG_VALID;
		flash_write_page(flash_adr_layout.flash_adr_sw_uuid, 2, &flash_flag);
	}

	return;
}
#endif

/************************************************************************
*						   		Proc RSSI
 ***********************************************************************/
unsigned char Rf_rssi_filter(unsigned char tmp_rssi)
{
	#define RSSI_NUM 5

	static unsigned char store_full_flag = 0,filter_first_flag = 0;
	static unsigned char store_rssi_value[RSSI_NUM] = {0};
	static unsigned char sort_rssi_value[RSSI_NUM] = {0};

	static unsigned int rssi_filter = 0,mid_cal_data = 0;
	static unsigned char rssi_index = 0;

	if((rssi_index < RSSI_NUM-1)&& (!store_full_flag)){
		store_rssi_value[rssi_index] = tmp_rssi;
		mid_cal_data = tmp_rssi;
	}
	else{
		store_full_flag = 1;						//receive 5 packets' flag
		store_rssi_value[rssi_index] = tmp_rssi;
		unsigned char copy_index = 0;

		for(copy_index = 0;copy_index<RSSI_NUM;copy_index++){
			sort_rssi_value[copy_index] = store_rssi_value[copy_index];
		}
		unsigned char i = 0,j = 0;
		unsigned char tmp_sort = 0;
		for(i=0;i<RSSI_NUM-1;i++){
			for(j=0;j<RSSI_NUM-1-i;j++){
				if(sort_rssi_value[j] > sort_rssi_value[j+1]){
					tmp_sort = sort_rssi_value[j];
					sort_rssi_value[j] = sort_rssi_value[j+1];
					sort_rssi_value[j+1] = tmp_sort;
				}
			}
		}
		mid_cal_data = sort_rssi_value[RSSI_NUM>>1];
	}

	if(!filter_first_flag){ 						///filter_first_flag == 0
		filter_first_flag = 1;
		rssi_filter = mid_cal_data;
	}
	else{											//filter_first_flag ==	1
													//+2 to rounding  +2/4
		rssi_filter = (3*rssi_filter + 1*mid_cal_data + 2)>>2;
	}

	rssi_index++;
	rssi_index %= RSSI_NUM; 						//from the header restore data
	return rssi_filter;
}

void Rf_rssi_proc(void)
{

	extern u8 * blt_get_rx_buffer (int write);
	u8 * prx = blt_get_rx_buffer (1);
	u8 rssi_temp = prx[4];

	rssi = Rf_rssi_filter(rssi_temp) - 110;//filter RSSI

	return;
}

short app_get_rssi(void)
{
	return rssi;
}

/************************************************************************
 *  						callback function.
 ***********************************************************************/

int task_adv_pre(rf_packet_adv_t * p)		// called before adv packet sent.
{
    u8 pairPaired_flag = 0;

    if (ReCheckADVStatusFlag == 1)
    {
        ReCheckADVStatusFlag = 0;
        pairPaired_flag = pairPaired();
        u8 paired_status_flag_tmp;
        if (pairPaired_flag)
        {
            paired_status_flag_tmp = 1;
            blt_adv_update_pkt(ADV_PKT_PAIR_STATUS_FLAG, &paired_status_flag_tmp);
        }
        else
        {
            paired_status_flag_tmp = 0;
            blt_adv_update_pkt(ADV_PKT_PAIR_STATUS_FLAG, &paired_status_flag_tmp);
        }
#if NTC_TEMP_ENABLE
        extern u8 cnt_run_fNtcTemperCheck;
        cnt_run_fNtcTemperCheck= 0; // trigger ntc reset
#endif
    }

	return 1;				// must return 1.
}

#if 0
int task_adv_post(rf_packet_adv_t * p)
{

    proc_ui ();     // lightweight proc
    proc_led();     // lightweight proc
    proc_broadcast_event();

	return 1;
}

void task_connect_brx(void)
{
#if NTC_TEMP_ENABLE
        extern u8 cnt_run_fNtcTemperCheck;
        cnt_run_fNtcTemperCheck= 0; // trigger ntc reset
#endif
    proc_ui ();     // lightweight proc
    proc_led();     // lightweight proc
    proc_broadcast_event();
}
#endif

void proc_ui();
void task_ui()
{
	proc_ui();
	proc_led();
	proc_broadcast_event();
}

/************************************************************************
 *  					Led callback function.
 ***********************************************************************/
// Begin1stBootInitKeyPhase hook customize function
void Begin1stBootInitKeyPhaseCB_pwm_hw_flash_on_cfg_1HZ(unsigned char HookPointNum, unsigned char * parameter) {
    cfg_led_event(LED_EVENT_FLASH_1HZ);
}

// Finish1stBootInitKeyPhase hook customize function
void Finish1stBootInitKeyPhaseCB_pwm_hw_flash_off(unsigned char HookPointNum, unsigned char * parameter) {
    cfg_led_event(LED_EVENT_FLASH_STOP);
}

// ReceivePairRequestPhase hook customize function
void ReceivePairRequestPhaseCB(unsigned char HookPointNum, unsigned char * parameter) {
    cfg_led_event(LED_EVENT_FLASH_5HZ);
}

// WaitSetupCodeInPhase hook customize function
void WaitSetupCodeInPhaseCB(unsigned char HookPointNum, unsigned char * parameter) {
    cfg_led_event(LED_EVENT_FLASH_1HZ_HK);
}

// FinishSetupCodeInOrTripleTryPhase hook customize function
void FinishSetupCodeInOrTripleTryPhaseCB(unsigned char HookPointNum, unsigned char * parameter) {
    cfg_led_event(LED_EVENT_FLASH_2HZ_3_SECOND);
}

// Register1stAdminKeyPhase hook customize function
void Register1stAdminKeyPhaseCB(unsigned char HookPointNum, unsigned char * parameter) {
    cfg_led_event(LED_EVENT_FLASH_4HZ_HK);
    sleep_us(1000);                         // delay for flash operation
    cfg_led_event(LED_EVENT_FLASH_2HZ_2TIMES);
}

// DoRemovePair1Phase hook customize function
void DoRemovePair1PhaseCB(unsigned char HookPointNum, unsigned char * parameter) {
    cfg_led_event(LED_EVENT_FLASH_5HZ);
    sleep_us(1000);                         // delay for flash operation
    cfg_led_event(LED_EVENT_FLASH_2HZ_2TIMES);
}

// DoRemovePair2Phase hook customize function
void DoRemovePair2PhaseCB(unsigned char HookPointNum, unsigned char * parameter) {
    cfg_led_event(LED_EVENT_FLASH_5HZ);
    extern u8 ReCheckADVStatusFlag;
    ReCheckADVStatusFlag = 1;
    sleep_us(1000);                         // delay for flash operation
    cfg_led_event(LED_EVENT_FLASH_2HZ_5TIMES);
}

// DoRemoveAllPairPhase hook customize function
void DoRemoveAllPairPhaseCB(unsigned char HookPointNum, unsigned char * parameter) {
    cfg_led_event(LED_EVENT_FLASH_5HZ);
    extern u8 ReCheckADVStatusFlag;
    ReCheckADVStatusFlag = 1;
    // trigger re-check and restore characterstic value, if empty, it'll auto re-initial.
    ReCheckAndRestoreFlashCharValueFlag = 1;
    sleep_us(1000);                         // delay for flash operation
    cfg_led_event(LED_EVENT_FLASH_2HZ_5TIMES);
}

/************************************************************************
 *  					.
 ***********************************************************************/

_attribute_ram_code_ void  ble_remote_set_sleep_wakeup (u8 e, u8 *p, int n)
{
	if( blc_ll_getCurrentState() == BLS_LINK_STATE_CONN && ((u32)(bls_pm_getSystemWakeupTick() - clock_time())) > 80 * CLOCK_16M_SYS_TIMER_CLK_1MS){  //suspend time > 30ms.add gpio wakeup
		bls_pm_setWakeupSource(PM_WAKEUP_PAD);  //gpio pad wakeup suspend/deepsleep
	}
}



void 	app_switch_to_indirect_adv(u8 e, u8 *p, int n)
{

	bls_ll_setAdvParam( MY_ADV_INTERVAL_MIN, MY_ADV_INTERVAL_MAX,
						ADV_TYPE_CONNECTABLE_UNDIRECTED, app_own_address_type,
						0,  NULL,
						MY_APP_ADV_CHANNEL,
						ADV_FP_NONE);

	bls_ll_setAdvEnable(1);  //must: set adv enable
}



void 	ble_remote_terminate(u8 e,u8 *p, int n) //*p is terminate reason
{
	device_in_connection_state = 0;
	extern u8 PS_Transient_cryption_Enable;
	PS_Transient_cryption_Enable = 0;

	session_timeout_tick = 0;
	hap_transationID_valid_timeout = 0;
	hap_timed_write_timeout = 0;
	hap_procedure_valid_timeout = 0;
	hap_remove_pairing_timeout = 0;
	
	blt_adv_global_state_modify_once = 1;

	blt_set_scan_rsp_pkt(tbl_mac,DEVICE_SHORT_NAME,(u8 *)tbl_rsp);
	blt_set_normal_adv_pkt(HK_CATEGORY, DEVICE_SHORT_NAME);

	if(*p == HCI_ERR_CONN_TIMEOUT){

	}
	else if(*p == HCI_ERR_REMOTE_USER_TERM_CONN){  //0x13

	}
	else if(*p == HCI_ERR_CONN_TERM_MIC_FAILURE){

	}
	else{

	}

#if (BLE_APP_PM_ENABLE)
	 //user has push terminate pkt to ble TX buffer before deepsleep
	if(sendTerminate_before_enterDeep == 1){
		sendTerminate_before_enterDeep = 2;
	}
#endif

	advertise_begin_tick = clock_time();
	//printfserial("ble disconnect",0,0);
}




_attribute_ram_code_ void	user_set_rf_power (u8 e, u8 *p, int n)
{
	rf_set_power_level_index (MY_RF_POWER_INDEX);
}



void	task_connect (u8 e, u8 *p, int n)
{
	//bls_l2cap_requestConnParamUpdate (8, 8, 19, 200);  // 200mS
//	bls_l2cap_requestConnParamUpdate (8, 8, 99, 400);  // 1 S
//	bls_l2cap_requestConnParamUpdate (8, 8, 149, 600);  // 1.5 S
//	bls_l2cap_requestConnParamUpdate (8, 8, 199, 800);  // 2 S
//	bls_l2cap_requestConnParamUpdate (8, 8, 249, 800);  // 2.5 S
//	bls_l2cap_requestConnParamUpdate (8, 8, 299, 800);  // 3 S

//	hap_procedure_valid_timeout = clock_time () | 1;

	latest_user_event_tick = clock_time();

	device_in_connection_state = 1;//

	blt_adv_global_state_modify_once = 1;

	//+for apple home-kit.
	hk_l2cap_att_init ();

	//-for apple home-kit.
	interval_update_tick = clock_time() | 1; //none zero

	if (ReCheckADVStatusFlag == 0){
		ReCheckADVStatusFlag = 1;
	}
	if( broadcast_encryption_time_timeout || disconnect_event_time_timeout){
		broadcast_encryption_time_timeout = 0;	// clear broadcast encryption notification.
		disconnect_event_time_timeout = 0;
		// Todo: reset adv packet.
		blt_set_normal_adv_pkt(HK_CATEGORY, DEVICE_SHORT_NAME);
		bls_ll_setAdvInterval(MY_ADV_INTERVAL_MIN, MY_ADV_INTERVAL_MAX);
	}
	//printfserial("ble connect",0,0);
}


void	task_conn_update_req (u8 e, u8 *p, int n)
{

}

void	task_conn_update_done (u8 e, u8 *p, int n)
{

}


//----------------------- UI ---------------------------------------------
#if (BLT_SOFTWARE_TIMER_ENABLE)
/**
 * @brief   This function is soft timer callback function.
 * @return  <0:delete the timer; ==0:timer use the same interval as prior; >0:timer use the return value as new interval. 
 */
int soft_timer_test0(void)
{
	//gpio 0 toggle to see the effect
	DBG_CHN4_TOGGLE;
	static u32 soft_timer_cnt0 = 0;
	soft_timer_cnt0++;
	return 0;
}
#endif


//----------------------- handle BLE event ---------------------------------------------
int app_event_handler (u32 h, u8 *p, int n)
{
	static u32 event_cb_num;
	event_cb_num++;
	int send_to_hci = 1;

	if (h == (HCI_FLAG_EVENT_BT_STD | HCI_EVT_LE_META))		//LE event
	{
		u8 subcode = p[0];

	//------------ ADV packet --------------------------------------------
		if (subcode == HCI_SUB_EVT_LE_ADVERTISING_REPORT)	// ADV packet
		{
			event_adv_report_t *pa = (event_adv_report_t *)p;
			if(LL_TYPE_ADV_NONCONN_IND != (pa->event_type & 0x0F)){
				return 0;
			}

			#if 0 // TESTCASE_FLAG_ENABLE
			u8 mac_pts[] = {0xDA,0xE2,0x08,0xDC,0x1B,0x00};	// 0x001BDC08E2DA
			if(memcmp(pa->mac, mac_pts,6)){
				return 0;
			}
			#endif
			
			#if DEBUG_MESH_DONGLE_IN_VC_EN
			send_to_hci = (0 == mesh_dongle_adv_report2vc(pa->data, MESH_ADV_PAYLOAD));
			#else
			send_to_hci = (0 == app_event_handler_adv(pa->data, MESH_BEAR_ADV, 1));
			#endif
		}

	//------------ connection complete -------------------------------------
		else if (subcode == HCI_SUB_EVT_LE_CONNECTION_COMPLETE)	// connection complete
		{
			event_connection_complete_t *pc = (event_connection_complete_t *)p;
			if (!pc->status)							// status OK
			{
				//app_led_en (pc->handle, 1);

				//peer_type = pc->peer_adr_type;
				//memcpy (peer_mac, pc->mac, 6);
			}
			#if DEBUG_BLE_EVENT_ENABLE
			rf_link_light_event_callback(LGT_CMD_BLE_CONN);
			#endif

			#if DEBUG_MESH_DONGLE_IN_VC_EN
			debug_mesh_report_BLE_st2usb(1);
			#endif
			proxy_cfg_list_init_upon_connection();
			mesh_service_change_report();
		}

	//------------ connection update complete -------------------------------
		else if (subcode == HCI_SUB_EVT_LE_CONNECTION_UPDATE_COMPLETE)	// connection update
		{
		}
	}

	//------------ disconnect -------------------------------------
	else if (h == (HCI_FLAG_EVENT_BT_STD | HCI_EVT_DISCONNECTION_COMPLETE))		//disconnect
	{

		event_disconnection_t	*pd = (event_disconnection_t *)p;
		//app_led_en (pd->handle, 0);
		//terminate reason
		if(pd->reason == HCI_ERR_CONN_TIMEOUT){

		}
		else if(pd->reason == HCI_ERR_REMOTE_USER_TERM_CONN){  //0x13

		}
		else if(pd->reason == SLAVE_TERMINATE_CONN_ACKED || pd->reason == SLAVE_TERMINATE_CONN_TIMEOUT){

		}
		#if DEBUG_BLE_EVENT_ENABLE
		rf_link_light_event_callback(LGT_CMD_BLE_ADV);
		#endif 

		#if DEBUG_MESH_DONGLE_IN_VC_EN
		debug_mesh_report_BLE_st2usb(0);
		#endif

		mesh_ble_disconnect_cb();
	}

	if (send_to_hci)
	{
		//blc_hci_send_data (h, p, n);
	}

	return 0;
}
u8 prov_end_status=0;
void proc_ui()
{
	static u32 tick, scan_io_interval_us = 40000;
	if (!clock_time_exceed (tick, scan_io_interval_us))
	{
		return;
	}
	tick = clock_time();

	#if 0
	static u8 st_sw1_last;	
	u8 st_sw1 = !gpio_read(SW1_GPIO);
	static u8 st_sw2_last;	
	u8 st_sw2 = !gpio_read(SW2_GPIO);
	if(st_sw1_last != st_sw1 || st_sw2_last != st_sw2 ) {
		if(st_sw1 && st_sw2){
			provision_mag.pro_stop_flag = ~ provision_mag.pro_stop_flag;
			set_provision_stop_flag_act(0);
			
		}else if(st_sw1){
			#if 1
			access_cmd_onoff(0xffff, 0, G_ON, CMD_NO_ACK, 0);
			#else
			 mesh_cfg_model_relay_set_t relay_set;
			relay_set.relay= 0x01;//relay = 0x01;
			relay_set.transmit.count = 0x00;//relay_retransmit=relay_retransmit=0;
			relay_set.transmit.invl_steps= 0x00;
			mesh_tx_cmd2normal_primary(CFG_RELAY_SET, (u8 *)&relay_set, sizeof(mesh_cfg_model_relay_set_t), 0x01, 0x01);
			#endif 
			scan_io_interval_us = 100*1000; // fix dithering
		}else if (st_sw2){
			access_cmd_onoff(0xffff, 0, G_OFF, CMD_NO_ACK, 0);
	    	scan_io_interval_us = 100*1000; // fix ditherin
		}
		st_sw1_last = st_sw1;
		st_sw2_last = st_sw2;
	}


	
	/*
	if((!(st_sw1_last)&&st_sw1) && (!(st_sw2_last)&&st_sw2)){
		prov_press_cnt++;
		prov_end_status = prov_end_status?0:1;
		st_sw1_last = st_sw1;
		st_sw2_last = st_sw2;
		return ;
	}
	if(!(st_sw1_last)&&st_sw1){
		access_cmd_onoff(0xffff, 0, G_ON, CMD_NO_ACK, 0);
	    scan_io_interval_us = 100*1000; // fix dithering
	}
	st_sw1_last = st_sw1;
	
	if(!(st_sw2_last)&&st_sw2){ // dispatch just when you press the button 
		access_cmd_onoff(0xffff, 0, G_OFF, CMD_NO_ACK, 0);
	    scan_io_interval_us = 100*1000; // fix dithering
	}
	st_sw2_last = st_sw2;
	*/
	#endif
}
#if GATEWAY_ENABLE

static u8 gateway_provision_para_enable=0;
void set_gateway_provision_sts(unsigned char en)
{
	gateway_provision_para_enable =en;
	return ;
}
unsigned char get_gateway_provisison_sts()
{
	unsigned char ret;
	ret = gateway_provision_para_enable;
	return ret;
}
void set_gateway_provision_para_init()
{
	gateway_adv_filter_init();
	set_provision_stop_flag_act(1);
	set_gateway_provision_sts(0);//disable the provision sts part 

}
u8 mesh_get_hci_tx_fifo_cnt()
{
#if (HCI_ACCESS == HCI_USE_USB)
	return hci_tx_fifo.size;
#elif (HCI_ACCESS == HCI_USE_UART)
	return hci_tx_fifo.size-0x10;
#else
	return 0;
#endif
}
int gateway_common_cmd_rsp(u8 code,u8 *p_par,u16 len )
{
	u8 head[2] = {TSCRIPT_GATEWAY_DIR_RSP};
	u8 head_len = 2;
	head[1] = code;
	u16 valid_fifo_size = mesh_get_hci_tx_fifo_cnt()-2; // 2: length
	if(len+head_len > valid_fifo_size){
		return gateway_sar_pkt_segment(p_par, len, valid_fifo_size, head, 2);
	}
	else{
		return my_fifo_push_hci_tx_fifo(p_par,len, head, 2); 
	}
}

u8 gateway_provision_rsp_cmd(u16 unicast_adr)
{
	return gateway_common_cmd_rsp(HCI_GATEWAY_RSP_UNICAST , (u8*)(&unicast_adr),2);
}
u8 gateway_keybind_rsp_cmd(u8 opcode )
{
	return gateway_common_cmd_rsp(HCI_GATEWAY_KEY_BIND_RSP , (u8*)(&opcode),1);
}

u8 gateway_model_cmd_rsp(u8 *para,u16 len )
{
	return gateway_common_cmd_rsp(HCI_GATEWAY_RSP_OP_CODE , para,len);
}

u8 gateway_heartbeat_cb(u8 *para,u8 len )
{
	//para reference to struct:  mesh_hb_msg_cb_t 
	return gateway_common_cmd_rsp(HCI_GATEWAY_CMD_HEARTBEAT , para,len);
}

u8 gateway_upload_mac_address(u8 *p_mac,u8 *p_adv)
{
	u8 para[40];//0~5 mac,adv ,6,rssi ,7~8 dc
	u8 len;
	len = p_adv[0];
	memcpy(para,p_mac,6);
	memcpy(para+6,p_adv,len+4);
	return gateway_common_cmd_rsp(HCI_GATEWAY_CMD_UPDATE_MAC,para,len+10);
}

u8 gateway_upload_provision_suc_event(u8 evt,u16 adr,u8 *p_mac,u8 *p_uuid)
{
    gateway_prov_event_t prov;
    prov.eve = evt;
    prov.adr = adr;
    memcpy(prov.mac,p_mac,6);
    memcpy(prov.uuid,p_uuid,16);
	return gateway_common_cmd_rsp(HCI_GATEWAY_CMD_PROVISION_EVT,(u8*)&prov,sizeof(prov));
}

u8 gateway_upload_keybind_event(u8 evt)
{
	return gateway_common_cmd_rsp(HCI_GATEWAY_CMD_KEY_BIND_EVT,&evt,1);
}

u8 gateway_upload_node_ele_cnt(u8 ele_cnt)
{
	return gateway_common_cmd_rsp(HCI_GATEWAY_CMD_SEND_ELE_CNT,&ele_cnt,1);
}
u8 gateway_upload_node_info(u16 unicast)
{
	VC_node_info_t * p_info;
	p_info = get_VC_node_info(unicast,1);
	return gateway_common_cmd_rsp(HCI_GATEWAY_CMD_SEND_NODE_INFO,(u8 *)p_info,sizeof(VC_node_info_t));
}

u8 gateway_upload_provision_self_sts(u8 sts)
{
	u8 buf[26];
	buf[0]=sts;
	if(sts){
		memcpy(buf+1,(u8 *)(&provision_mag.pro_net_info),25);
	}
	provison_net_info_str* p_net = (provison_net_info_str*)(buf+1);
	p_net->unicast_address = provision_mag.unicast_adr_last;
	return gateway_common_cmd_rsp(HCI_GATEWAY_CMD_PRO_STS_RSP,buf,sizeof(buf));
}

u8 gateway_upload_mesh_ota_sts(u8 *p_dat,int len)
{
	return gateway_common_cmd_rsp(HCI_GATEWAY_CMD_SEND_MESH_OTA_STS,p_dat,len);
}

u8 gateway_upload_mesh_sno_val()
{
    return gateway_common_cmd_rsp(HCI_GATEWAY_CMD_SEND_SNO_RSP,
                        (u8 *)&mesh_adv_tx_cmd_sno,sizeof(mesh_adv_tx_cmd_sno));

}
u8 gateway_upload_dev_uuid(u8 *p_uuid,u8 *p_mac)
{
    u8 uuid_mac[22];
    memcpy(uuid_mac,p_uuid,16);
    memcpy(uuid_mac+16,p_mac,6);
    return gateway_common_cmd_rsp(HCI_GATEWAY_CMD_SEND_UUID,
                        (u8 *)uuid_mac,sizeof(uuid_mac));
}

u8 gateway_upload_ividx(u8 *p_ivi)
{
    return gateway_common_cmd_rsp(HCI_GATEWAY_CMD_SEND_IVI,
                        p_ivi,4);
}

u8 gateway_upload_mesh_src_cmd(u16 op,u16 src,u8 *p_ac_par)
{
    gateway_upload_mesh_src_t cmd;
    cmd.op = op;
    cmd.src = src;
    memcpy(cmd.ac_par,p_ac_par,sizeof(cmd.ac_par));
    return gateway_common_cmd_rsp(HCI_GATEWAY_CMD_SEND_SRC_CMD,
                            (u8*)&cmd,sizeof(cmd));
}
#define GATEWAY_MAX_UPLOAD_CNT 0x20
u8 gateway_upload_prov_cmd(u8 *p_cmd,u8 cmd)
{
    u8 len =0;
    len = get_mesh_pro_str_len(cmd);
    if(len){
        if(len>GATEWAY_MAX_UPLOAD_CNT){
            len = GATEWAY_MAX_UPLOAD_CNT;
        }
        return gateway_common_cmd_rsp(HCI_GATEWAY_CMD_SEND,
                            (u8*)p_cmd,len);
    }
    return 0;
}

u8 gateway_upload_prov_rsp_cmd(u8 *p_rsp,u8 cmd)
{
    u8 len =0;
    len = get_mesh_pro_str_len(cmd);
    if(len){
        if(len>GATEWAY_MAX_UPLOAD_CNT){
            len = GATEWAY_MAX_UPLOAD_CNT;
        }
        return gateway_common_cmd_rsp(HCI_GATEWAY_DEV_RSP,
                            (u8*)p_rsp,len);
    }
    return 0;
}

u8 gateway_upload_prov_link_open(u8 *p_cmd,u8 len)
{
    return gateway_common_cmd_rsp(HCI_GATEWAY_CMD_LINK_OPEN,
                            (u8*)p_cmd,len);
}

u8 gateway_upload_prov_link_cls(u8 *p_rsp,u8 len)
{
    return gateway_common_cmd_rsp(HCI_GATEWAY_CMD_LINK_CLS,
                            (u8*)p_rsp,len);
}

u8 gateway_upload_mesh_cmd_back_vc(material_tx_cmd_t *p)
{
#if 0
	gateway_upload_mesh_cmd_str gateway_cmd;
	u8 len ;
	gateway_cmd.src = p->adr_src;
	gateway_cmd.dst = p->adr_dst;
	gateway_cmd.opcode = p->op;
	if(p->par_len >sizeof(gateway_cmd.para)){
		len = sizeof(gateway_cmd.para);
	}else{
		len = p->par_len;
	}
	memcpy(gateway_cmd.para , p->par , len);
	len+=6;
	return gateway_common_cmd_rsp(HCI_GATEWAY_CMD_SEND_BACK_VC,
                            (u8*)(&gateway_cmd),len);
#else
	return 0;
#endif
}

u8 gateway_cmd_from_host_ctl(u8 *p, u16 len )
{
	if(len<=0){
		return 0;
	}
	u8 op_code = p[0];
	if(op_code == HCI_GATEWAY_CMD_START){
		set_provision_stop_flag_act(0);
	}else if (op_code == HCI_GATEWAY_CMD_STOP){
		set_provision_stop_flag_act(1);
	}else if (op_code == HCI_GATEWAY_CMD_RESET){
        factory_reset();
        show_factory_reset();
        start_reboot();
	}else if (op_code == HCI_GATEWAY_CMD_CLEAR_NODE_INFO){
		// clear the provision store  information 
		VC_cmd_clear_all_node_info(0xffff);// clear all node
	}else if (op_code == HCI_GATEWAY_CMD_SET_ADV_FILTER){
		set_gateway_adv_filter(p+1);
	}else if (op_code == HCI_GATEWAY_CMD_SET_PRO_PARA){
		// set provisioner net info para 
		provison_net_info_str *p_net;
		p_net = (provison_net_info_str *)(p+1);
		set_provisioner_para(p_net->net_work_key,p_net->key_index,
								p_net->flags,p_net->iv_index,p_net->unicast_address);
		// use the para (node_unprovision_flag) ,and the flag will be 0 
		
	}else if (op_code == HCI_GATEWAY_CMD_SET_NODE_PARA){
		// set the provisionee's netinfo para 
		provison_net_info_str *p_net;
		p_net = (provison_net_info_str *)(p+1);
		// set the pro_data infomation 
		set_provisionee_para(p_net->net_work_key,p_net->key_index,
								p_net->flags,p_net->iv_index,p_net->unicast_address);
		provision_mag.unicast_adr_last = p_net->unicast_address;
		set_gateway_provision_sts(1);

	}else if (op_code == HCI_GATEWAY_CMD_START_KEYBIND){
		extern u8 pro_dat[40];
		provison_net_info_str *p_str = (provison_net_info_str *)pro_dat;
		mesh_gw_appkey_bind_str *p_bind = (mesh_gw_appkey_bind_str *)(p+1);
		mesh_cfg_keybind_start_trigger_event(p_bind->key_idx,p_bind->key,
			p_str->unicast_address,p_str->key_index,p_bind->fastbind);
	}else if (op_code == HCI_GATEWAY_CMD_SET_DEV_KEY){
        mesh_gw_set_devkey_str *p_set_devkey = (mesh_gw_set_devkey_str *)(p+1);
        set_dev_key(p_set_devkey->dev_key);
        #if (DONGLE_PROVISION_EN)
			VC_node_dev_key_save(p_set_devkey->unicast,p_set_devkey->dev_key,2);
	    #endif
	}else if (op_code == HCI_GATEWAY_CMD_GET_SNO){
        gateway_upload_mesh_sno_val();
	}else if (op_code == HCI_GATEWAY_CMD_SET_SNO){
        u32 sno;
        memcpy((u8 *)&sno,p+1,4);
        mesh_adv_tx_cmd_sno = sno;
        mesh_misc_store();
	}else if (op_code == HCI_GATEWAY_CMD_GET_PRO_SELF_STS){
		gateway_upload_provision_self_sts(is_provision_success());
	}else if (op_code == HCI_GATEWAY_CMD_STATIC_OOB_RSP){
		if(len-1>16){
			return 1;
		}
		mesh_set_pro_auth(p+1,len-1);
	}else if (op_code == HCI_GATEWAY_CMD_GET_UUID_MAC){
        // rsp the uuid part 
        gateway_upload_dev_uuid(prov_para.device_uuid,tbl_mac);
	}else if (op_code == HCI_GATEWAY_CMD_DEL_VC_NODE_INFO){
        u16 unicast = p[1]|(p[2]<<8);
        del_vc_node_info_by_unicast(unicast);
	}
	return 1;
}

u8 gateway_cmd_from_host_ota(u8 *p, u16 len )
{
	rf_packet_att_data_t local_ota;
	u8 dat_len ;
	dat_len = p[0];
	local_ota.dma_len = dat_len+9;
	local_ota.type = 0;
	local_ota.rf_len = dat_len+7;
	local_ota.l2cap = dat_len+3;
	local_ota.chanid = 4;
	local_ota.att = 0;
	local_ota.hl = 0;
	local_ota.hh = 0;
	memcpy(local_ota.dat,p+1,dat_len);
	// enable ota flag 
	pair_login_ok = 1;
	otaWrite((u8 *)&local_ota);
	return 1;
}

u8 gateway_cmd_from_host_mesh_ota(u8 *p, u16 len )
{
	u8 op_type =0;
	op_type = p[0];
	if(op_type == MESH_OTA_SET_TYPE){
		set_ota_reboot_flag(p[1]);
	}else if(op_type == MESH_OTA_ERASE_CTL){
		// need to erase the ota part 
		bls_ota_clearNewFwDataArea();
	}else{}
	return 1;
}
#endif


/////////////////////////////////////////////////////////////////////
// main loop flow
/////////////////////////////////////////////////////////////////////
void main_loop ()
{
	static u32 tick_loop;

	tick_loop ++;
#if (BLT_SOFTWARE_TIMER_ENABLE)
	blt_soft_timer_process(MAINLOOP_ENTRY);
#endif
	mesh_loop_proc_prior();
	////////////////////////////////////// BLE entry /////////////////////////////////
	blt_sdk_main_loop ();
	
	if (blt_state == BLS_LINK_STATE_CONN )
	{
		hk_l2cap_att_main_loop();
	}


	////////////////////////////////////// UI entry /////////////////////////////////
	//  add spp UI task:
#if (BATT_CHECK_ENABLE)
    app_battery_power_check_and_sleep_handle(1);
#endif
	proc_ui();
	proc_led();
	factory_reset_cnt_check();
	
	mesh_loop_process();
#if ADC_ENABLE
	static u32 adc_check_time;
    if(clock_time_exceed(adc_check_time, 1000*1000)){
        adc_check_time = clock_time();
		static u16 T_adc_val;
		#if(MCU_CORE_TYPE == MCU_CORE_8269)     
        T_adc_val = adc_BatteryValueGet();
		#else
		T_adc_val = adc_sample_and_get_result();
		#endif
    }  
#endif	
	#if (TESTCASE_FLAG_ENABLE && (!__PROJECT_MESH_PRO__))
	test_case_key_refresh_patch();
	#endif
}

void user_init()
{
    #if (BATT_CHECK_ENABLE)
    app_battery_power_check_and_sleep_handle(0); //battery check must do before OTA relative operation
    #endif
	user_sha256_data_proc();
	enable_mesh_provision_buf();
	mesh_global_var_init();
	set_blc_hci_flag_fun(0);// disable the hci part of for the lib .
	proc_telink_mesh_to_sig_mesh();		// must at first
	blc_app_loadCustomizedParameters();  //load customized freq_offset cap value and tp value

// homekit init.
	COMMON_Init();

	dev_setup_code_set();
	dev_serial_number_set();
	dev_setup_id_set();

#if	MODULE_WATCHDOG_ENABLE
	wd_stop(); // disable watchdog for homekit init key
#endif
    // srp key initialization take 10 seonds on first power on
    srpInitKey();
#if HK_DIFF_SRP_IN_PAIR_SETUP_ENABLE
	srpInitKey_2nd();
#endif
#if (MODULE_WATCHDOG_ENABLE)
	wd_clear();
	wd_start();
#endif

// lib func.
	dev_device_id_set();
	dev_hash_deviceid_set();

#if SOFTWARE_AUTHENTICATION_ENABLE
	dev_sw_auth_uuid_set(SW_AUTH_UUID_INIT, test_sw_uuid);
	dev_sw_auth_token_set(SW_AUTH_TOKEN_INIT, test_sw_token, sizeof(test_sw_token));
#endif

#if 0
	// test read broadcast encryption key.
	flash_read_long_val(flash_adr_layout.flash_adr_adv_encryption_key,0,bBroadcastEnKey,32);
#endif

	pairInitID();

	usb_id_init();
	usb_log_init ();
	usb_dp_pullup_en (1);  //open USB enum

	////////////////// BLE stack initialization ////////////////////////////////////
	ble_mac_init();    

	//link layer initialization
	//bls_ll_init (tbl_mac);
#if(MCU_CORE_TYPE == MCU_CORE_8269)
	blc_ll_initBasicMCU(tbl_mac);   //mandatory
#elif(MCU_CORE_TYPE == MCU_CORE_8258)
	blc_ll_initBasicMCU();                      //mandatory
	blc_ll_initStandby_module(tbl_mac);				//mandatory
#endif
	blc_ll_initAdvertising_module(tbl_mac); 	//adv module: 		 mandatory for BLE slave,
	blc_ll_initSlaveRole_module();				//slave module: 	 mandatory for BLE slave,
#if BLE_REMOTE_PM_ENABLE
	blc_ll_initPowerManagement_module();        //pm module:      	 optional
#endif

	//l2cap initialization
	//blc_l2cap_register_handler (blc_l2cap_packet_receive);
	blc_l2cap_register_handler (app_l2cap_packet_receive);
	///////////////////// USER application initialization ///////////////////	
	flash_read_page((u32)(flash_adr_layout.flash_adr_hash_id), 4, hk_setupHash);

	blt_set_scan_rsp_pkt(tbl_mac,DEVICE_SHORT_NAME,(u8 *)tbl_rsp);

	u8 status = bls_ll_setAdvParam( ADV_INTERVAL_MIN, ADV_INTERVAL_MAX, \
			 	 	 	 	 	     ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
			 	 	 	 	 	     0,  NULL,  BLT_ENABLE_ADV_ALL, ADV_FP_NONE);

	if(status != BLE_SUCCESS){  //adv setting err
		write_reg8(0x8000, 0x11);  //debug
		while(1);
	}
	
	// normally use this settings 
	blc_ll_setAdvCustomedChannel (37, 38, 39);


	rf_set_power_level_index (MY_RF_POWER_INDEX);
	bls_pm_setSuspendMask (SUSPEND_DISABLE);//(SUSPEND_ADV | SUSPEND_CONN)
    blc_hci_le_setEventMask_cmd(HCI_LE_EVT_MASK_ADVERTISING_REPORT|HCI_LE_EVT_MASK_CONNECTION_COMPLETE);

	////////////////// SPP initialization ///////////////////////////////////
#if (HCI_ACCESS != HCI_USE_NONE)
	#if (HCI_ACCESS==HCI_USE_USB)
	//blt_set_bluetooth_version (BLUETOOTH_VER_4_2);
	//bls_ll_setAdvChannelMap (BLT_ENABLE_ADV_ALL);
	usb_bulk_drv_init (0);
	blc_register_hci_handler (app_hci_cmd_from_usb, blc_hci_tx_to_usb);
	#elif (HCI_ACCESS == HCI_USE_UART)	//uart
	uart_drv_init();
	blc_register_hci_handler (blc_rx_from_uart, blc_hci_tx_to_uart);		//default handler
	//blc_register_hci_handler(rx_from_uart_cb,tx_to_uart_cb);				//customized uart handler
	#endif
#endif
#if ADC_ENABLE
	adc_drv_init();
#endif
	rf_pa_init();
	bls_app_registerEventCallback (BLT_EV_FLAG_CONNECT, (blt_event_callback_t)&mesh_ble_connect_cb);

    hook_callback_func_register(Begin1stBootInitKeyPhase, (void *)&Begin1stBootInitKeyPhaseCB_pwm_hw_flash_on_cfg_1HZ);
    hook_callback_func_register(Finish1stBootInitKeyPhase, (void *)&Finish1stBootInitKeyPhaseCB_pwm_hw_flash_off);

    hook_callback_func_register(ReceivePairRequestPhase, (void *)&ReceivePairRequestPhaseCB);
    hook_callback_func_register(WaitSetupCodeInPhase, (void *)&WaitSetupCodeInPhaseCB);
    hook_callback_func_register(FinishSetupCodeInOrTripleTryPhase, (void *)&FinishSetupCodeInOrTripleTryPhaseCB);
    hook_callback_func_register(Register1stAdminKeyPhase, (void *)&Register1stAdminKeyPhaseCB);

    hook_callback_func_register(DoRemovePair1Phase,(void *) &DoRemovePair1PhaseCB);
    hook_callback_func_register(DoRemoveAllPairPhase, (void *)&DoRemoveAllPairPhaseCB);
	
	blc_hci_registerControllerEventHandler(app_event_handler);		//register event callback
	//bls_hci_mod_setEventMask_cmd(0xffff);			//enable all 15 events,event list see ble_ll.h
	bls_set_advertise_prepare (app_advertise_prepare_handler);
	//bls_set_update_chn_cb(chn_conn_update_dispatch);	
	bls_ota_registerStartCmdCb(entry_ota_mode);
	bls_ota_registerResultIndicateCb(show_ota_result);

	app_enable_scan_all_device ();

	// mesh_mode and layer init
	mesh_init_all();
	// OTA init
	bls_ota_clearNewFwDataArea(); // confirm ota later

	//blc_ll_initScanning_module(tbl_mac);
	#if(MCU_CORE_TYPE == MCU_CORE_8258)
	blc_gap_peripheral_init();    //gap initialization
	#endif
	
	mesh_scan_rsp_init();
	my_att_init (provision_mag.gatt_mode);
	blc_att_setServerDataPendingTime_upon_ClientCmd(10);
	system_time_init();
	
    bls_ll_setAdvEnable(1);  //adv enable
	advertise_begin_tick = clock_time();

	//bls_ota_registerVendorCmdCb(&CustVendorCmdFunctions);			// ota vendor cmd, later proc.
	hkble_setCustVendorCmdCb(&CustVendorCmdFunctions);

	blt_pair_ok = pairPaired(); //homekit paired flag
    broadcast_encryption_time_timeout = 0;
#if (BLT_SOFTWARE_TIMER_ENABLE)
	blt_soft_timer_init();
	//blt_soft_timer_add(&soft_timer_test0, 1*1000*1000);
#endif
}

