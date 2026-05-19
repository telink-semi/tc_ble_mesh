/********************************************************************************************************
 * @file	mesh_common.c
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
#include "tl_common.h"
#include "stack/ble/ble.h"
#ifndef WIN32
#include "proj_lib/mesh_crypto/mesh_md5.h"
#endif 
#include "myprintf.h"
#include "proj_lib/ble/blt_config.h"
#include "vendor/common/user_config.h"
#include "app_health.h"
#include "proj_lib/sig_mesh/app_mesh.h"
#include "app_provison.h"
#include "lighting_model.h"
#include "vendor/common/sensors_model.h"
#include "vendor/common/remote_prov.h"
#include "proj_lib/mesh_crypto/sha256_telink.h"
#include "proj_lib/mesh_crypto/le_crypto.h"
#include "lighting_model_LC.h"
#include "mesh_ota.h"
#include "mesh_common.h"
#include "mesh_config.h"
#include "directed_forwarding.h"
#include "certify_base/certify_base_crypto.h"
#include "dual_mode_adapt.h"

#if DU_ENABLE
#include "user_du.h"
#endif

#if GATT_RP_EN
#include "remote_prov_gatt.h"
#endif

#ifdef WIN32 
#include "../../../reference/tl_bulk/lib_file/host_fifo.h"
#endif

#if FAST_PROVISION_ENABLE
#include "vendor/common/fast_provision_model.h"
#endif

#if (HCI_ACCESS==HCI_USE_UART)
#include "drivers.h"
#endif

#if HOMEKIT_EN
#include "../mesh_gw_node_homekit/hk_CategoryDef.h"
#include "homekit_src/homekit_inc.h"
#include "homekit_src/hk_adv_packet.h"
#endif

#if MD_ON_DEMAND_PROXY_EN
#include "vendor/common/solicitation_rpl_cfg_model.h"
#endif

STATIC_ASSERT(TRANSMIT_CNT_DEF < 8);
STATIC_ASSERT(TRANSMIT_CNT_DEF_RELAY < 8);
STATIC_ASSERT(sizeof(mesh_scan_rsp_t) <= 31);
#if (CHIP_TYPE == CHIP_TYPE_8269)
STATIC_ASSERT((0 == FLASH_PLUS_ENABLE) && (0 == PINGPONG_OTA_DISABLE));
#endif

#if (FW_LOAD_BY_BOOTLOADER_AND_NO_OTA_EN)
//STATIC_ASSERT(FLASH_ADR_AREA_FIRMWARE_END <= );
#elif (PINGPONG_OTA_DISABLE)
STATIC_ASSERT(1 == FLASH_PLUS_ENABLE);
#else
    #if __TLSR_RISCV_EN__
STATIC_ASSERT(FLASH_ADR_AREA_FIRMWARE_END <=       0x7F000);
    #else
STATIC_ASSERT(FLASH_ADR_AREA_FIRMWARE_END <= 0x3F000);
    #endif
#endif
#ifdef FLASH_ADR_AREA_FIRMWARE_END
STATIC_ASSERT(FLASH_ADR_AREA_FIRMWARE_END%0x1000 == 0);
#endif
#if BLE_REMOTE_PM_ENABLE
STATIC_ASSERT(PM_DEEPSLEEP_RETENTION_ENABLE);
#endif
STATIC_ASSERT(PID_CHIP_TYPE <= 15);


#if((HCI_ACCESS==HCI_USE_UART)&&(MCU_CORE_TYPE == MCU_CORE_8269))
#if (UART_GPIO_SEL == UART_GPIO_8267_PC2_PC3)
#define PWM_UART_IO_CONFLICT_PC2    ((PWM_R == GPIO_PC2)||(PWM_G == GPIO_PC2)||(PWM_B == GPIO_PC2)||(PWM_W == GPIO_PC2))
#define PWM_UART_IO_CONFLICT_PC3    ((PWM_R == GPIO_PC3)||(PWM_G == GPIO_PC3)||(PWM_B == GPIO_PC3)||(PWM_W == GPIO_PC3))
//STATIC_ASSERT((!PWM_UART_IO_CONFLICT_PC2) && (!PWM_UART_IO_CONFLICT_PC3));
//TODO("PWM IO was conflicted with UART IO. Please redefine PWM IO !!!");
// ASSERT
typedef char assert_PWM_IO_is_conflicted_with_UART_IO_Please_redefine_PWM_IO[(!!(!(PWM_UART_IO_CONFLICT_PC2 || PWM_UART_IO_CONFLICT_PC3)))*2-1];
#endif
#endif

#if TESTCASE_FLAG_ENABLE
#define HCI_TX_FIFO_SIZE	(132)
#define HCI_TX_FIFO_NUM		(8)
#else
#if (IS_VC_PROJECT)
#define HCI_TX_FIFO_SIZE	(256)
#define HCI_TX_FIFO_NUM		(128)
#else
#if (FEATURE_PROXY_EN)
	#if (HCI_ACCESS == HCI_USE_USB)
#define HCI_TX_FIFO_SIZE	(REPORT_ADV_BUF_SIZE)
	#else
	    #if (MESH_MONITOR_EN)
#define HCI_TX_FIFO_SIZE	(400)
		#elif(HCI_LOG_FW_EN)
#define HCI_TX_FIFO_SIZE	(80)		
        #else
#define HCI_TX_FIFO_SIZE	(64)
        #endif
	#endif
	#if __PROJECT_SPIRIT_LPN__
#define HCI_TX_FIFO_NUM		(4)
	#else
#define HCI_TX_FIFO_NUM		(16)
	#endif
#else
#define HCI_TX_FIFO_SIZE	(128)
#define HCI_TX_FIFO_NUM		(8)
#endif
#endif
#endif

#define HCI_TX_FIFO_SIZE_USABLE     (HCI_TX_FIFO_SIZE - 2)  // 2: sizeof(fifo.len)

#if(HCI_ACCESS != HCI_USE_NONE)
MYFIFO_INIT(hci_tx_fifo, HCI_TX_FIFO_SIZE, HCI_TX_FIFO_NUM); // include adv pkt and other event


#if (IS_VC_PROJECT)
MYFIFO_INIT(hci_rx_fifo, 512, 4);   // max play load 382
#else
#define UART_DATA_SIZE              (EXTENDED_ADV_ENABLE ? 280 : 72)    // increase or decrease 16bytes for each step.
#define HCI_RX_FIFO_SIZE            (UART_DATA_SIZE + 4 + 4)    // 4: sizeof DMA len;  4: margin reserve(can't receive valid data, because UART_DATA_SIZE is max value of DMA len)
STATIC_ASSERT(HCI_RX_FIFO_SIZE % 16 == 0);

MYFIFO_INIT(hci_rx_fifo, HCI_RX_FIFO_SIZE, 4);

#if (__TLSR_RISCV_EN__ && UART_SECOND_EN)
MYFIFO_INIT(hci_rx_fifo_2nd, HCI_RX_FIFO_SIZE, 4);
MYFIFO_INIT(hci_tx_fifo_2nd, HCI_TX_FIFO_SIZE, 4);
#endif

#endif
#endif

#ifndef WIN32    // also used for shell file.
__WEAK void function_null_compile(const void *p){}// just for avoid being optimized

#if ENCODE_OTA_BIN_EN
/**
 * key_encode_bin : Encryption key for OTA firmware.
 * eclipse will create two firmwares: *.bin and *_enc.bin . "*_enc.bin" is only used for OTA.
 * The key is used to encrypted in eclipse and decrypted in firmware.
 * Key size must be 16 bytes in hex and should never be changed any more for this product.
*/
#ifndef ENCODE_BIN_USER_KEY     // please define in "user_app_config.h"
#define ENCODE_BIN_USER_KEY     {0} //{0x51,0x03,0x1f,0x03,0x57,0x81,0x7b,0x5c,0x48,0x83,0x93,0xae,0xa8,0xc6,0x5d,0x9a,} // 
#endif
#if __TLSR_RISCV_EN__
volatile _attribute_no_retention_data_ // for not be optimized
#endif
const u8 key_encode_bin[] = ENCODE_BIN_USER_KEY;    // must const
STATIC_ASSERT(sizeof(key_encode_bin) == 16);

#ifdef ENCODE_BIN_USER_NAME     // please define in "user_app_config.h" if need.
#if __TLSR_RISCV_EN__
volatile _attribute_no_retention_data_ // for not be optimized
#endif
const u8 ENCODE_BIN_USER_NAME_CONST[] = ENCODE_BIN_USER_NAME; // {"8258_mesh_AES"}
STATIC_ASSERT(sizeof(ENCODE_BIN_USER_NAME_CONST) >= 1);
	#if __TLSR_RISCV_EN__
#define FUNC_NULL_KEEP_ENCODE_BIN_USER_NAME()   // no need to be called, because kept in boot link.
	#else
#define FUNC_NULL_KEEP_ENCODE_BIN_USER_NAME()   do{function_null_compile(ENCODE_BIN_USER_NAME_CONST);}while(0)
	#endif
#endif
#endif

#ifndef FUNC_NULL_KEEP_ENCODE_BIN_USER_NAME
#define FUNC_NULL_KEEP_ENCODE_BIN_USER_NAME()    
#endif

#ifdef NORMAL_BIN_USER_NAME    // please define in "user_app_config.h" if need.
#if __TLSR_RISCV_EN__
volatile _attribute_no_retention_data_ // for not be optimized
#endif
const u8 NORMAL_BIN_USER_NAME_CONST[] = NORMAL_BIN_USER_NAME; // {"8258_mesh_normal"}
STATIC_ASSERT(sizeof(NORMAL_BIN_USER_NAME_CONST) >= 1);
	#if __TLSR_RISCV_EN__
#define FUNC_NULL_KEEP_NORMAL_BIN_USER_NAME()   // no need to be called, because kept in boot link.
	#else
#define FUNC_NULL_KEEP_NORMAL_BIN_USER_NAME()     do{function_null_compile(NORMAL_BIN_USER_NAME_CONST);}while(0)
	#endif
#else
#define FUNC_NULL_KEEP_NORMAL_BIN_USER_NAME()     
#endif

#define FUNC_NULL_KEEP_VAL()    FUNC_NULL_KEEP_ENCODE_BIN_USER_NAME(); \
                                FUNC_NULL_KEEP_NORMAL_BIN_USER_NAME();
#else
#define FUNC_NULL_KEEP_VAL()    
#endif



u8 g_reliable_retry_cnt_def = RELIABLE_RETRY_CNT_DEF;
u8 pair_login_ok = 0;

u16 g_vendor_id = VENDOR_ID;
u16 g_msg_vd_id = VENDOR_ID;// record the vendor id of vendor message
#ifdef WIN32
u32 g_vendor_md_light_vc_s = VENDOR_MD_LIGHT_S;
u32 g_vendor_md_light_vc_c = VENDOR_MD_LIGHT_C;
    #if MD_VENDOR_2ND_EN
u32 g_vendor_md_light_vc_s2 = VENDOR_MD_LIGHT_S2;
    #endif
#endif
u8  mesh_user_define_mode = MESH_USER_DEFINE_MODE;  // for library use

u16 mesh_tx_with_random_delay_ms = 0; // max 12000

#define MAX_BEACON_SEND_INTERVAL 	2000*1000
beacon_send_str beacon_send;


/**
 * @brief       This function set the parameters for sending beacon packets
 * @return      none
 * @note        
 */
void beacon_str_init(void)
{
	beacon_send.start_time_s = system_time_s;
	beacon_send.tick = clock_time();
	beacon_send.en = 1;
	beacon_send.inter = MAX_BEACON_SEND_INTERVAL;
	return ;
}

/**
 * @brief       This function disable the sending beacon packet
 * @return      none
 * @note        
 */
void beacon_str_disable(void)
{
	beacon_send.en = 0;
}

/**
 * @brief       This function determine whether the data is valid
 * @param[in]   len		- data length
 * @param[in]   p_data	- data
 * @return      1: valid data; 0: invalid data
 * @note        
 */
u8 get_flash_data_is_valid(u8 *p_data,u16 len)
{
	for(int i=0;i<len;i++){
		if(p_data[i] != 0xff){
			return 1;
		}
	}
	return 0;
}


/**
 * @brief       This function rapid visa inspection
 * @param[out]  r	- ecdh secret
 * @param[in]   s	- private key
 * @param[in]   x	- Public key of high 32 oneself
 * @param[in]   y	- Public key of low 32 oneself
 * @return      
 * @note        
 */
void tn_p256_dhkey_fast(u8 *r, u8 *s, u8 *x, u8 *y)
{
	clock_switch_to_highest();
	tn_p256_dhkey(r,s,x,y);
	clock_switch_to_normal();
}

#ifndef WIN32

/**
 * @brief       This function check if crc of ecdh is valid.
 * @param[in]   p_key	- parameters required for verification
 * @return      1: crc valid; 0: crc invalid
 * @note        
 */
u8 check_ecdh_crc(mesh_ecdh_key_str *p_key)
{
	u32 crc = crc16(p_key->dsk,sizeof(p_key->dsk)+sizeof(p_key->dpk));
	if(crc == p_key->crc){
		return 1;
	}else{
		return 0;
	}
}


/**
 * @brief       This function write new ecdh data with keeping other info of this sector.
 * @param[in]   adr		- the write address of ecdh data
 * @param[in]   len		- the size of ecdh data
 * @param[in]   p_data	- ecdh data
 * @return      none
 * @note        
 */
void erase_ecdh_sector_restore_info(u32 adr,u8 *p_data,u16 len)
{
	u32 sec_adr = (adr>>12);
	sec_adr = sec_adr << 12;// get the base sector adr
	u8 temp_buf[SECTOR_PAR_SIZE_MAX];
 	u16 offset = (adr - sec_adr);
	flash_read_page(sec_adr,SECTOR_PAR_SIZE_MAX,temp_buf);
	if(p_data == NULL || len == 0 || len>0x100){
		memset(temp_buf+offset,0xff,0x100);
	}else{
		memcpy(temp_buf+offset,p_data,len);
	}
	flash_erase_sector(sec_adr);
	flash_write_page(sec_adr,SECTOR_PAR_SIZE_MAX,temp_buf);
}


/**
 * @brief       This function check if ecdh key is existed in flash, and if it is valid.
 * @param[in]   p_key	- pub_key,private_key
 * @return      1: two keys are invalid; 2: none keys are invalid; 3: one keys are invalid
 * @note        
 */
u8 get_ecdh_key_sts(mesh_ecdh_key_str *p_key)
{
	u8 dsk_valid = get_flash_data_is_valid(p_key->dsk,sizeof(p_key->dsk));
	u8 dpk_valid = get_flash_data_is_valid(p_key->dpk,sizeof(p_key->dpk));
	if(dsk_valid && dpk_valid ){
		return ECDH_KEY_STS_TWO_VALID;
	}else if(!dsk_valid && !dpk_valid){
		return ECDH_KEY_STS_NONE_VALID;
	}else{
		return ECDH_KEY_STS_ONE_VALID;
	}
}


/**
 * @brief       This function check whether the pub key and private key are valid.
 * @param[in]   p_key	- key
 * @return      1: key valid; 0: key invalid
 * @note        
 */
u8 cal_dsk_dpk_is_valid_or_not(mesh_ecdh_key_str *p_key)
{
	
	u8 dev_dsk[32]={ 0x52,0x9a,0xa0,0x67,0x0d,0x72,0xcd,0x64, 0x97,0x50,0x2e,0xd4,0x73,0x50,0x2b,0x03,
					 0x7e,0x88,0x03,0xb5,0xc6,0x08,0x29,0xa5, 0xa3,0xca,0xa2,0x19,0x50,0x55,0x30,0xba};
	u8 dev_dpk[64]={ 0xf4,0x65,0xe4,0x3f,0xf2,0x3d,0x3f,0x1b, 0x9d,0xc7,0xdf,0xc0,0x4d,0xa8,0x75,0x81,
					 0x84,0xdb,0xc9,0x66,0x20,0x47,0x96,0xec, 0xcf,0x0d,0x6c,0xf5,0xe1,0x65,0x00,0xcc,
					 0x02,0x01,0xd0,0x48,0xbc,0xbb,0xd8,0x99, 0xee,0xef,0xc4,0x24,0x16,0x4e,0x33,0xc2,
					 0x01,0xc2,0xb0,0x10,0xca,0x6b,0x4d,0x43, 0xa8,0xa1,0x55,0xca,0xd8,0xec,0xb2,0x79};
	u8 k0[32],k1[32];
	tn_p256_dhkey_fast (k0, dev_dsk, p_key->dpk, p_key->dpk+0x20);
	tn_p256_dhkey_fast (k1, p_key->dsk, dev_dpk, dev_dpk+0x20);
	if(!memcmp(k0,k1,sizeof(k0))){
		return 1;
	}else{
		return 0;
	}
}

mesh_ecdh_key_str key_str;

void check_and_handle_ecdh_key(mesh_ecdh_key_str *p_ecdh_key)
{
	if(0 == check_ecdh_crc(p_ecdh_key)){// crc is invalid.
		erase_ecdh_sector_restore_info(FLASH_ADR_EDCH_PARA,0,0);
		start_reboot();
	}
}


/**
 * @brief       This function get private key
 * @param[out]  p_private_key	- 
 * @return      none
 * @note        
 */
void get_private_key(u8 *p_private_key)
{
#if (PROV_AUTH_LEAK_RECREATE_KEY_EN || CERTIFY_BASE_ENABLE)
	memcpy(p_private_key, key_str.dsk, 32);
#else
	mesh_ecdh_key_str key_str;
	flash_read_page(FLASH_ADR_EDCH_PARA,sizeof(key_str),(u8*)&key_str);
	check_and_handle_ecdh_key(&key_str); // will reboot if check error.

	memcpy(p_private_key, key_str.dsk, 32);
#endif
}


/**
 * @brief       This function get public key
 * @param[in]   p_public_key	- 
 * @return      none
 * @note        
 */
void get_public_key(u8 *p_public_key)
{
#if (PROV_AUTH_LEAK_RECREATE_KEY_EN || CERTIFY_BASE_ENABLE)
	memcpy(p_public_key, key_str.dpk, 64);
#else
	mesh_ecdh_key_str key_str;
	flash_read_page(FLASH_ADR_EDCH_PARA,sizeof(key_str),(u8*)&key_str);
	check_and_handle_ecdh_key(&key_str); // will reboot if check error.
	
	memcpy(p_public_key, key_str.dpk, 64);
#endif
}


/**
 * @brief       This function create rand with mac
 * @param[in]   p_mac	- mac
 * @param[out]  p_rand	- output(rand+mac)
 * @return      
 * @note        
 */
void create_rand_by_mac(u8* p_rand,u8* p_mac)
{
	p_rand[0]=rand();
	p_rand[1]=rand();
	memcpy(p_rand+2,p_mac,6);
	return ;
}


/**
 * @brief       This function generate random number
 * @param[in]   len		- how many random numbers are generated
 * @param[out]  p_buf	- store random numbers
 * @return      0: generator rand num success; 4: failed
 * @note        
 */
int telink_rand_num_generator(uint8_t* p_buf, uint8_t len)
{
	for(int i=0;i<len;i++){
		p_buf[i]=rand()&0xff;
	}
	return SUCCESS;
}

/**
 * @brief       This function create ecc key with fast way.
 * @param[out]  dpk	- pub_key
 * @param[out]  dsk	- private_key
 * @return      none
 * @note        
 */
void ecc_create_key_fast(u8* dsk,u8* dpk)
{
    blt_ecc_init();
    blt_ecc_gen_key_pair(dpk, dsk, ECC_use_secp256r1, 0);
}




/**
 * @brief       This function process public key creation.
 * @return      none
 * @note        
 */
void pubkey_create_proc(void)
{
    // b85 48Mhz cost 594ms
	clock_switch_to_highest();
	#if TESTCASE_FLAG_ENABLE
	// for in the testcase mode ,the pubkey should be const ,or can not test with oob with pubkey
	u8 rand_input[6];
	memcpy(rand_input,tbl_mac,6);
	#if PROV_AUTH_LEAK_RECREATE_KEY_EN
	rand_input[0]=rand()&0xff;
	rand_input[1]=rand()&0xff;
	rand_input[2]=rand()&0xff;
	#endif
	tn_p256_keypair_mac(key_str.dsk,key_str.dpk,key_str.dpk+32,rand_input,6);// create the key part 
	#else
	ecc_create_key_fast(key_str.dsk,key_str.dpk);
	#endif
	clock_switch_to_normal();
}


/**
 * @brief       This function get valid private and public key of ECC from flash or re-create.
 * @param[in]   force_en	- force to re-create private and public key.
 * @return      none
 * @note        
 */
void cal_private_and_public_key(u8 force_en)
{
	// first need to change the format to key(32+64),flag,crc(use crc16) 
	// get all the para data first 
	#if CERTIFY_BASE_ENABLE
	if(prov_para.cert_base_en ){
		#if(GATEWAY_ENABLE)
			force_en = 0;
		#else
			// node mode , force to use the certify part .
			cert_base_set_key(key_str.dpk,key_str.dsk);	
			key_str.valid = ECDH_KEY_VALID_FLAG;
			key_str.crc = crc16(key_str.dsk,sizeof(key_str.dsk)+sizeof(key_str.dpk));
			return ;
		#endif
	}
	#endif
	if(force_en){
		// need to recreate the key info every time
		pubkey_create_proc();
		return ;
	}
	flash_read_page(FLASH_ADR_EDCH_PARA,sizeof(key_str),(u8*)&key_str);
	if(key_str.valid == 0xffffffff){
		u8 key_sts  = get_ecdh_key_sts(&key_str);//certify
		if(key_sts == ECDH_KEY_STS_NONE_VALID){
			// create the key flag crc and valid 
			u32 irq_res = irq_disable();
			#if CERTIFY_BASE_ENABLE
			if(prov_para.cert_base_en){
				#if GATEWAY_ENABLE
				pubkey_create_proc();
				#endif
			}else{
				pubkey_create_proc();
			}	
			#else
				pubkey_create_proc();// create the key part 
			#endif
			irq_restore(irq_res); 
			key_str.valid = ECDH_KEY_VALID_FLAG;
			key_str.crc = crc16(key_str.dsk,sizeof(key_str.dsk)+sizeof(key_str.dpk));
			flash_write_page(FLASH_ADR_EDCH_PARA,sizeof(key_str),(u8*)&key_str);

		}else if (key_sts == ECDH_KEY_STS_TWO_VALID){ //cetify
			// need to dispatch the key is valid or not ,use init the dsk and dpk to calculate is valid or not 
			if(cal_dsk_dpk_is_valid_or_not(&key_str)){// key is valid
				key_str.valid = ECDH_KEY_VALID_FLAG;
				key_str.crc = crc16(key_str.dsk,sizeof(key_str.dsk)+sizeof(key_str.dpk));
				flash_write_page(FLASH_ADR_EDCH_PARA+96,8,(u8*)&(key_str.valid));
			}else{
				erase_ecdh_sector_restore_info(FLASH_ADR_EDCH_PARA,0,0);
				start_reboot();
			}
		}else{// there is only one key in  the valid ,need to erase and reboot 
			erase_ecdh_sector_restore_info(FLASH_ADR_EDCH_PARA,0,0);
			start_reboot();
		}
	}
	
}
#endif 


/**
 * @brief       This function process security beacon and private beacon with beacon interval.
 * @return      none
 * @note        
 */
void mesh_secure_beacon_loop_proc(void)
{
	static u32 beacon_1s_tick = (u32)(SEC_NW_BC_INV_DEF_1S - 2); // send secure beacon when power up.
	if(clock_time_exceed_s(beacon_1s_tick, SEC_NW_BC_INV_DEF_1S) && is_provision_success()){
   		beacon_1s_tick = clock_time_s();
		#if (!DEBUG_PUBLISH_REDUCE_COLLISION_TEST_EN)
   		mesh_tx_sec_private_beacon_proc(0);	
		#endif
	}
}

/**
 * @brief       This function determine whether to send security network beacon
 * @return      1: need; 0: no need
 * @note        
 */
int is_need_send_sec_nw_beacon(void)
{
	return 	((NW_BEACON_BROADCASTING == model_sig_cfg_s.sec_nw_beacon));
}


/**
 * @brief       This function poll to tx network security beacon and private beacon if the time arrives.
 * @return      none
 * @note        
 */
void mesh_beacon_poll_1s(void)
{
#if (IV_UPDATE_DISABLE)
    return ;
#endif

#if (0 == NODE_CAN_SEND_ADV_FLAG)
	return ;	// for test
#endif

	if(is_fn_support_and_en){
        //if(fri_ship_proc_lpn.status || is_in_mesh_friend_st_lpn()){
        //}
    	foreach(i,g_max_lpn_num){
			mesh_fri_ship_proc_fn_t *proc_fn = &fri_ship_proc_fn[i];
	        if(proc_fn->status && (FRI_ST_TIMEOUT_CHECK != proc_fn->status)){
	            return ;
	        }
        }
    }
	#if (0 == SECURE_NETWORK_BEACON_LOOP_DISABLE)
    mesh_secure_beacon_loop_proc();
	#endif
}


/**
 * @brief       This function save tid. just for node with low power in deep sleep. 
 *              because TID re-transaction will be invalid after 6 seconds.
 * @param[in]   ele_idx	- element index
 * @return      none
 * @note        
 */
void mesh_tid_save(int ele_idx)
{
#if __PROJECT_MESH_SWITCH__
	// analog_write(REGA_TID, mesh_tid.tx[ele_idx]); // no need, because use deep retention mode.
#endif
}

/**
 * @brief       This function Set the default retry count for reliable command.
 * @param[in]   retry_cnt	- retry times
 * @return      none
 * @note        
 */
void reliable_retry_cnt_def_set(u8 retry_cnt)
{
    g_reliable_retry_cnt_def = retry_cnt > RELIABLE_RETRY_CNT_MAX ? RELIABLE_RETRY_CNT_MAX : retry_cnt;
}

#if (!defined(WIN32) && MD_BIND_WHITE_LIST_EN)
const u16 master_filter_list[]={
	SIG_MD_G_ONOFF_S,SIG_MD_G_LEVEL_S,SIG_MD_G_DEF_TRANSIT_TIME_S,SIG_MD_LIGHTNESS_S,
	SIG_MD_LIGHTNESS_SETUP_S,SIG_MD_LIGHT_CTL_S,SIG_MD_LIGHT_CTL_SETUP_S,SIG_MD_LIGHT_CTL_TEMP_S,
	SIG_MD_LIGHT_LC_S,SIG_MD_LIGHT_LC_SETUP_S,
    SIG_MD_LIGHT_HSL_S,SIG_MD_LIGHT_HSL_SETUP_S,SIG_MD_LIGHT_HSL_HUE_S,SIG_MD_LIGHT_HSL_SAT_S,
	SIG_MD_FW_UPDATE_S,SIG_MD_FW_UPDATE_C,SIG_MD_FW_DISTRIBUT_S,SIG_MD_FW_DISTRIBUT_C,SIG_MD_BLOB_TRANSFER_S,SIG_MD_BLOB_TRANSFER_C,
};

/**
 * @brief       This function get model list which need to bind.
 * @param[out]  key_bind_list_buf	- save model_id
 * @param[out]  p_list_cnt			- model id num
 * @param[in]   max_cnt				- model id max num
 * @return      0: get fail; 1: success.
 * @note        none
 */
u8 model_need_key_bind_whitelist(u16 *key_bind_list_buf,u8 *p_list_cnt,u8 max_cnt)
{
	if(ARRAY_SIZE(master_filter_list) >= max_cnt){
		return 0;
	}
	memcpy((u8 *)key_bind_list_buf,(u8 *)master_filter_list,sizeof(master_filter_list));
	*p_list_cnt= ARRAY_SIZE(master_filter_list);
	return 1;
}
#endif

/**
 * @brief       This function is callback function for provision event of mesh node in each provision state.
 * @param[in]   evt_code	- event code
 *Provisionee:	EVENT_MESH_NODE_RC_LINK_CLOSE: link close.
 *				EVENT_MESH_NODE_RC_LINK_START: link establish, enter provisioning state 
 *				EVENT_MESH_NODE_RC_LINK_FAIL_CODE: link close by provision fail.
 *				EVENT_MESH_NODE_RC_LINK_TIMEOUT: link close by timeout.
 *				EVENT_MESH_NODE_RC_LINK_SUC: link close success, means provision complete.
 *
 *Provisioner:  EVENT_MESH_PRO_RC_LINK_START: provision link establish.
 *              EVENT_MESH_PRO_RC_LINK_TIMEOUT: provision link close by timeout.
 *              EVENT_MESH_PRO_RC_LINK_SUC: provision link close success, means provision complete.
 *
 *pb_gatt:		EVENT_MESH_NODE_CONNECT: ble gatt connect.
 *              EVENT_MESH_NODE_DISCONNECT: ble gatt disconnect.
 * @return     	none 
 * @note        
 */
void mesh_node_prov_event_callback(u8 evt_code)
{
#ifdef WIN32
#else
    if( evt_code == EVENT_MESH_NODE_RC_LINK_START ||
        evt_code == EVENT_MESH_PRO_RC_LINK_START ){
        #if(!GATEWAY_ENABLE)
        if(blc_ll_getCurrentState() == BLS_LINK_STATE_CONN){
            mesh_set_scan_enable(0, 0);
			blc_att_setServerDataPendingTime_upon_ClientCmd(1);
			set_prov_timeout_tick(clock_time()|1); 
        }else
		#endif
		{
             // disable the mesh provision filter part 
		    enable_mesh_adv_filter();
		    //set_prov_timeout_tick(clock_time()|1); // disable the prov timeout proc ,only enable in gatt mode 
        }
		#if SPIRIT_PRIVATE_LPN_EN
		bls_pm_setSuspendMask (SUSPEND_DISABLE);
		#endif
		#if __PROJECT_MESH_SWITCH__
		switch_mode_tick = clock_time(); // refresh time for provision		
		#endif
		#if DUAL_MESH_SIG_PVT_EN
		dual_mode_mesh_found = DUAL_MODE_NETWORK_SIG_MESH;
		#endif
    }else{
#if (!BLE_REMOTE_PM_ENABLE || SPIRIT_PRIVATE_LPN_EN || PTS_TEST_EN)
    	mesh_set_scan_enable(1, 0);
#endif
		blc_att_setServerDataPendingTime_upon_ClientCmd(10);
		set_prov_timeout_tick(0);
		disable_mesh_adv_filter();
		mesh_provision_para_reset();
		if( evt_code == EVENT_MESH_NODE_RC_LINK_TIMEOUT ||
	        evt_code == EVENT_MESH_NODE_DISCONNECT||
	        evt_code == EVENT_MESH_NODE_RC_LINK_SUC||
	        evt_code == EVENT_MESH_NODE_RC_LINK_CLOSE ||
	        evt_code == EVENT_MESH_NODE_CONNECT){

			if(evt_code == EVENT_MESH_NODE_RC_LINK_SUC){
				#if (AIS_ENABLE)
				mesh_scan_rsp_init(); // set prov_flag	
				#endif
				#if __PROJECT_MESH_SWITCH__
				switch_mode_tick = clock_time(); // refresh time for appkey bind		
				#endif
			}
	    }
    }

	#if (AIS_ENABLE)
	beacon_send.start_time_s = system_time_s;
	#endif
#endif 
}


/**
 * @brief       This function get little endianness version number.
 * @param[in]   ver_big	- big endianness version number
 * @return      little : little endianness version number
 * @note        
 */
u16 get_little_end_version(u16 ver_big)
{
	u16 little = 0;
	swap16((u8 *)&little, (u8 *)&ver_big);
	return little;
}

const fw_id_t fw_id_local = {
#if PTS_TEST_OTA_EN
	0x0011, 0x1100,
#else
    MESH_PID_SEL,   // BUILD_VERSION, also mark in firmware_address[2:5]
    MESH_VID,
#endif
};


/**
 * @brief       This function check if new firmware has a valid PID(product ID) and VID(Version IS).
 * @param[in]   p_fw_id		- firmware ID
 * @param[in]   gatt_flag	- 1: it is GATT OTA.
 * @return      
 * @note        for both GATT and MESH ADV OTA
 */
_USER_CAN_REDEFINE_ int ota_is_valid_pid_vid(fw_id_t *p_fw_id, int gatt_flag)
{
    #if (OTA_ADOPT_RULE_CHECK_PID_EN)
    // user can change this policy
    int accept = 0;
    if(p_fw_id->pid == fw_id_local.pid){
    	#if OTA_ADOPT_RULE_CHECK_VID_EN
		u16 ver_new_little = get_little_end_version(p_fw_id->vid);
		u16 ver_local_little = get_little_end_version(fw_id_local.vid);
        if(ver_new_little > ver_local_little){
            accept = 1;
        }
        #else
		accept = 1;
        #endif
    }else{
        /* if accept when diffrend pid, it is set to unprovision state after OTA completed.
           because the number and types of the model in composition data maybe changed. 
           if no change, and do not want to set to unprovision state, please change ADDITIONAL_NODE_UNPROVISIONED to 
           ADDITIONAL_CPS_CHANGE_NO_REMOTE_PROVISIOIN or ADDITIONAL_CPS_CHANGE_REMOTE_PROVISIOIN in get_mesh_ota_additional_info*/
        #if OTA_ADOPT_RULE_ALLOW_DIFFERENT_PID_EN
		accept = 1;
        #endif
    }
    
    return accept;
    #else
    return 1;
    #endif
}

#ifndef WIN32
//----------------------- OTA --------------------------------------------
u8 	ui_ota_is_working = 0;
u8 ota_reboot_type = OTA_FW_CHECK_ERR;
u32 ota_reboot_later_tick = 0;


/**
 * @brief       This function This function is called back when the OTA start command is received.
 * @return      none
 * @note        
 */
void entry_ota_mode(void)
{
	ui_ota_is_working = 1;
	mesh_set_scan_enable(0, 0);

	#if GATT_LPN_EN
	bls_l2cap_requestConnParamUpdate_Normal();
	#endif
	
	#if (DUAL_MODE_ADAPT_EN || DUAL_MODE_WITH_TLK_MESH_EN)
	dual_mode_disable();
	// bls_ota_clearNewFwDataArea(0); // may disconnect
	#endif

	#if APP_FLASH_PROTECTION_ENABLE
	app_flash_protection_ota_begin();
	#endif
}


/**
 * @brief       This function get if it is allowed OTA.
 * @return      0: no authentication; 1: authentication
 * @note        
 */
_USER_CAN_REDEFINE_ u8 ota_condition_enable(void)
{
#if 1
	return pair_login_ok; // 1;
#else
	return (pair_login_ok || !is_provision_success());
#endif
}

#if DUAL_OTA_NEED_LOGIN_EN
#ifndef DUALMODE_OTA_KEY_USER
#define DUALMODE_OTA_KEY_USER   {0x01,0x02,0x03,0x04,'t','l','n','k',}
#endif
const u8 dualmode_ota_key_user[8] = DUALMODE_OTA_KEY_USER;
extern u8 my_OtaData; // error code in read response


/**
 * @brief       This function check auth for homekit OTA.
 * @param[in]   auth_app	- auth from app.
 * @return      none
 * @note        
 */
void cmd_ota_mesh_hk_login_handle(const u8 auth_app[16])
{
    if(!is_provision_success()){
        int ok = dualmode_ota_auth_value_check(dualmode_ota_key_user, prov_para.device_uuid, auth_app);
        if(ok){
            pair_login_ok = 1;
            my_OtaData = 0; // success
        }else{
            my_OtaData = 1; // error code
        }
    }
}
#endif

#if (0 == __TLSR_RISCV_EN__) // process flash parameter when OTA between different SDK.

/**
 * @brief       This function set firmware type
 * @param[in]   sdk_type	- sdk type
 * @return      none
 * @note        
 */
void set_firmware_type(u32 sdk_type)
{
    u32 mesh_type = 0;
    flash_read_page(FLASH_ADR_MESH_TYPE_FLAG, sizeof(mesh_type), (u8 *)&mesh_type);
	if(mesh_type != 0xffffffff){
		flash_erase_sector(FLASH_ADR_MESH_TYPE_FLAG);
	}
	mesh_type = sdk_type;
	flash_write_page (FLASH_ADR_MESH_TYPE_FLAG, 4, (u8 *)&mesh_type);
}


/**
 * @brief       This function set firmware type to "TYPE SIG MESH"
 * @return      none
 * @note        
 */
void set_firmware_type_SIG_mesh(void)
{
    set_firmware_type(TYPE_SIG_MESH);
}

#if FW_START_BY_LEGACY_BOOTLOADER_EN

/**
 * @brief       This function set firmware type to "TYPE DUAL MODE ZIGBEE RESET"
 * @return      none
 * @note        
 */
void set_firmware_type_zb_with_factory_reset(void)
{
    set_firmware_type(TYPE_DUAL_MODE_ZIGBEE_RESET);
}
#endif

#if DUAL_MODE_WITH_TLK_MESH_EN

/**
 * @brief       This function set firmware type to recover.
 * @return      none
 * @note        
 */
void set_firmware_type_recover()
{
    set_firmware_type(TYPE_DUAL_MODE_RECOVER);
}
#endif


/**
 * @brief       This function set firmware type is FLASH_ADR_MESH_TYPE_FLAG
 * @return      none
 * @note        
 */

/**
 * @brief       This function firmware type init
 * @return      none
 * @note        
 */
void set_firmware_type_init(void)
{
    flash_erase_sector(FLASH_ADR_MESH_TYPE_FLAG);
}

#if (DUAL_MODE_ADAPT_EN)
#define TLK_MESH_NO_TYPE_CHECK_EN   0 // must 0
#else
#define TLK_MESH_NO_TYPE_CHECK_EN   ((0 == FLASH_PLUS_ENABLE) && (CFG_SECTOR_ADR_MAC_CODE == CFG_ADR_MAC_512K_FLASH))
#endif


/**
 * @brief       This function adopt different settings for different SDK.
 * @return      1: sig mesh; 0: other
 * @note        
 */
u8 proc_telink_mesh_to_sig_mesh(void)
{
    #if (DUAL_VENDOR_EN || GATEWAY_ENABLE)
    return 1;
    #endif
    u32 mesh_type;
	flash_read_page(FLASH_ADR_MESH_TYPE_FLAG,sizeof(mesh_type),(u8*)&mesh_type);

	#if DUAL_MODE_ADAPT_EN
    LOG_MSG_LIB(TL_LOG_NODE_SDK,0, 0,"sdk type 0x%x:0x%x", FLASH_ADR_MESH_TYPE_FLAG, mesh_type);
	if(TYPE_DUAL_MODE_STANDBY == mesh_type){
		return 0;
	}if(0xffffffff == mesh_type){
        set_firmware_type(TYPE_DUAL_MODE_STANDBY);
        LOG_MSG_LIB(TL_LOG_NODE_SDK,0, 0,"sdk type: Factory status");
		return 0;
	}else if(TYPE_DUAL_MODE_RECOVER == mesh_type){
        factory_reset();
        set_firmware_type(TYPE_DUAL_MODE_STANDBY);
        LOG_MSG_LIB(TL_LOG_NODE_SDK,0, 0,"sdk type: Recover from Zigbee");
        return 0;
	}
	#endif

	if(TYPE_SIG_MESH == mesh_type){
		return 1;
	}
	
	u8 ret = 0;
	#if (TLK_MESH_NO_TYPE_CHECK_EN)
	u32 sig_cust_cap_flag = 0;
	// 0x77004 is fixed address of telink mesh, not depend on flash size.
	flash_read_page(0x77004, 4, (u8 *)&sig_cust_cap_flag);	// reserve 3 byte for CUST_CAP_INFO
    #endif
    
	if((TYPE_TLK_BLE_SDK == mesh_type) || (TYPE_TLK_ZIGBEE == mesh_type)){
		ret = 1;
	}else if((TYPE_TLK_MESH == mesh_type)
	#if (TLK_MESH_NO_TYPE_CHECK_EN)
	||(-1 != sig_cust_cap_flag)
	#endif
	){
	    #if (TLK_MESH_NO_TYPE_CHECK_EN)
	    if(CFG_ADR_MAC_512K_FLASH == flash_sector_mac_address){ // DC and TP is in the same address when 1M flash.
    		flash_erase_sector(flash_sector_calibration);

    		u8 flash_data = 0;
    		flash_read_page(flash_sector_mac_address + 0x10, 1, &flash_data);
    		flash_write_page(flash_sector_calibration + CALIB_OFFSET_CAP_INFO, 1, &flash_data);

            #if ((MCU_CORE_TYPE == MCU_CORE_8267)||(MCU_CORE_TYPE == MCU_CORE_8269))
    		flash_read_page(flash_sector_mac_address + 0x11, 1, &flash_data);
    		flash_write_page(flash_sector_calibration + CALIB_OFFSET_TP_INFO, 1, &flash_data);

    		flash_read_page(flash_sector_mac_address + 0x12, 1, &flash_data);
    		flash_write_page(flash_sector_calibration + CALIB_OFFSET_TP_INFO + 1, 1, &flash_data);
    		// no RC32K_CAP_INFO
    		#endif
		}
		#endif
		
		ret = 1;
	}
	if(ret == 1){
		factory_reset();
	}

	#if DUAL_MODE_WITH_TLK_MESH_EN
	if(DUAL_MODE_SUPPORT_ENABLE == dual_mode_state){
	    set_firmware_type_recover();    // keep recover mode before provision ok
	}else
	#endif
	{
	    set_firmware_type_SIG_mesh();
	}
	
	return ret;
}
#else
u8 proc_telink_mesh_to_sig_mesh(void){return 0;}
#endif


/**
 * @brief       This function trigger ota reboot flow.
 * @param[in]   type	- ota reboot type 
 * @return      none
 * @note        
 */
void mesh_ota_reboot_set(u8 type)
{
    ota_reboot_later_tick = clock_time() | 1;
    ota_reboot_type = type;
}


/**
 * @brief       This function refresh tick of timeout countdown.
 * @return      none
 * @note        
 */
void mesh_ota_reboot_check_refresh()
{
    if(ota_reboot_later_tick){
        ota_reboot_later_tick = clock_time() | 1;
    }
}


/**
 * @brief       This function ota reboot process
 * @return      none
 * @note        
 */
void mesh_ota_reboot_proc()
{
    #if PTS_TEST_OTA_EN
    #define MESH_OTA_REBOOT_STANDBY_MS   (9000) // stay enough time before reboot, because PTS will sent firmware start after firmware apply.
    #else
    #define MESH_OTA_REBOOT_STANDBY_MS   (3000)
    #endif

	u32 reboot_standby_ms = MESH_OTA_REBOOT_STANDBY_MS;
    #if (DISTRIBUTOR_UPDATE_SERVER_EN)
    if(is_rx_upload_start_before()){
    	reboot_standby_ms = max2(reboot_standby_ms, 6000);
    }
    #endif
    
    if(ota_reboot_later_tick && clock_time_exceed(ota_reboot_later_tick, reboot_standby_ms*1000)){ // for 4 hops or more
		set_keep_onoff_state_after_ota();

		#if FEATURE_LOWPOWER_EN
		if(BLS_LINK_STATE_CONN == blc_ll_getCurrentState()){
        	set_ota_gatt_connected_flag_lpn(); // for lpn, need send connectable adv after reboot.
    	}
		#endif
		
        if(OTA_SUCCESS_DEBUG == ota_reboot_type){
            irq_disable();
            while(1){show_ota_result(OTA_SUCCESS);}
        }else{
            //irq_disable(); // should not disable irq, because need to send terminate to App later.
            if(OTA_SUCCESS == ota_reboot_type){
            	__UNUSED int err = 0;
                err = ota_set_flag();
                #if MD_MESH_OTA_EN
				extern fw_update_srv_proc_t    fw_update_srv_proc;
				//LOG_MSG_LIB(TL_LOG_NODE_SDK,0, 0,"reboot additional_info is:%d",fw_update_srv_proc.additional_info);
                if((0 == err) && (ADDITIONAL_NODE_UNPROVISIONED == fw_update_srv_proc.additional_info)){
					send_and_wait_completed_reset_node_status();
                	kick_out(1);	// will reboot inside // will protect flash by event of factory reset end. 
                }
                #endif
            }

			#if (APP_FLASH_PROTECTION_ENABLE)
			app_flash_protection_ota_end(ota_reboot_type);/* do it before led indication*/
			#endif
		
			#if !defined(WIN32)
			if(bls_ll_isConnectState()){
				bls_ll_terminateConnection (0x13); // add terminate cmd
				if(OTA_REBOOT_NO_LED == ota_reboot_type){
					sleep_us(200*1000);	// no need sleep, will delay at later show ota result. // wait terminate to be sent completely.
				}
			}	
			#endif
            show_ota_result(ota_reboot_type);
            start_reboot();
        }
    }
}


/**
 * @brief       This function trigger sending update parameters after GATT connection
 * @return      none
 * @note        
 */
void bls_l2cap_requestConnParamUpdate_Normal(void)
{
    if(blc_ll_getCurrentState() == BLS_LINK_STATE_CONN){
		#if BLE_MULTIPLE_CONNECTION_ENABLE
		for(int i = ACL_CENTRAL_MAX_NUM; i < (ACL_CENTRAL_MAX_NUM + ACL_PERIPHR_MAX_NUM); i++) { //peripheral index is from "ACL_CENTRAL_MAX_NUM" to "ACL_CENTRAL_MAX_NUM + ACL_PERIPHR_MAX_NUM - 1"
			if(conn_dev_list[i].conn_state) {
				bls_l2cap_requestConnParamUpdate(conn_dev_list[i].conn_handle, 16, 32, 0, 500);
			}
		}
		#else
	    bls_l2cap_requestConnParamUpdate (16, 32, 0, 500);
		#endif
	}
}

//----------------------- ble connect callback --------------------------------------------

/**
 * @brief       This function is called when GATT connect success.
 * @param[in]   e	- event code BLT_EV_FLAG_CONNECT
 * @param[in]   p	- data len
 * @param[in]   n	- raw data point to DATA after rf_len
 * @return      none
 * @note        
 */
void mesh_ble_connect_cb(u8 e, u8 *p, int n)
{
#if BLE_MULTIPLE_CONNECTION_ENABLE
	hci_le_connectionCompleteEvt_t *pConnEvt = (hci_le_connectionCompleteEvt_t *)p;
	__UNUSED u16 conn_handle = pConnEvt->connHandle;
	dev_char_info_insert_by_conn_event(pConnEvt);
	bls_l2cap_setMinimalUpdateReqSendingTime_after_connCreate(conn_handle, 3000);
	bls_l2cap_requestConnParamUpdate (conn_handle, 16, 32, 0, 500);
#else
	__UNUSED u16 conn_handle = BLS_CONN_HANDLE;
	#if __TLSR_RISCV_EN__
	bls_l2cap_setMinimalUpdateReqSendingTime_after_connCreate(GATT_LPN_EN ? 3000 : 2000);
	#endif

	#if GATT_LPN_EN
	bls_l2cap_requestConnParamUpdate (320, 400, 0, 500);
	#else
	bls_l2cap_requestConnParamUpdate (16, 32, 0, 500);
	#endif
#endif

	#if GATT_LPN_EN
    //blc_ll_exchangeDataLength(LL_LENGTH_REQ , DLE_LEN_MAX_TX);    // master will sent request if supported.
	#endif
	
	#if ONLINE_STATUS_EN
    mesh_node_report_init ();
	#endif
    mesh_event_handler(EVENT_MESH_NODE_CONNECT, 0, 0);

#if AIS_ENABLE
	ais_gatt_auth_init();
#endif	
#if HOMEKIT_EN
	task_connect(e, p, n);
#endif
#if PAIR_PROVISION_ENABLE
	pair_login_ok = 1; // because want to use generic APP to do OTA.
#endif
	
	LOG_MSG_LIB(TL_LOG_NODE_SDK, 0, 0, "%s connHandle:0x%x", __func__, conn_handle);
    CB_USER_BLE_CONNECT(e, p, n);
}


/**
 * @brief       This function After GATT disconnect, this function is called back.
 * @param[in]   reason	- disconnect reason
 * @return      none
 * @note        
 */
void mesh_ble_disconnect_cb(u8 *p)
{
	__UNUSED u8 conn_idx = 0;
	__UNUSED event_disconnection_t	*pd = (event_disconnection_t *)p;
#if BLE_MULTIPLE_CONNECTION_ENABLE
	conn_idx = get_periphr_idx_by_conn_handle(pd->connHandle);
	if(conn_idx == INVALID_CONN_IDX){
		return;
	}
#endif

    #if BLE_MULTIPLE_CONNECTION_ENABLE
	if(0 == blc_ll_getCurrentSlaveRoleNumber())
	#endif
    {   
	    pair_login_ok = 0;
    }
    
    proxy_filter_list_init(pd->connHandle);
	reset_all_ccc();
    mesh_event_handler(EVENT_MESH_NODE_DISCONNECT, 0, 0);

	#if MD_REMOTE_PROV
	mesh_rp_para_init();// avoid unexpected disconnect 
	#endif
    
	#if (ONLINE_STATUS_EN && !GATEWAY_ENABLE)
    mesh_report_status_enable(0);
	#endif
    
	#if (MD_DF_CFG_SERVER_EN && !FEATURE_LOWPOWER_EN)
	directed_proxy_dependent_node_delete(conn_idx);
	#endif
    
#if FEATURE_LOWPOWER_EN
	if(LPN_MODE_GATT_OTA == lpn_mode){
	    lpn_mode_tick = clock_time();
	    lpn_mode_set(LPN_MODE_NORMAL);
	}
	
	mesh_lpn_gatt_adv_refresh();
#endif

#if MD_ON_DEMAND_PROXY_EN
	mesh_on_demand_private_gatt_proxy_start();
#endif

#if BLE_MULTIPLE_CONNECTION_ENABLE
	dev_char_info_delete_by_connhandle(pd->connHandle);
    #if EXTENDED_ADV_ENABLE
    blc_ll_setExtAdvEnable(BLC_ADV_ENABLE, ADV_HANDLE0, 0 , 0); // enable advertising set again.
    #endif
#endif

	LOG_MSG_LIB(TL_LOG_NODE_SDK, 0, 0, "%s connHandle:0x%x", __func__, pd->connHandle);
}

#if DEBUG_VC_FUNCTION

/**
 * @brief       This function push message to fifo for VC(sig mesh tool).
 * @param[in]   cmd		- command
 * @param[in]   pfifo	- send data
 * @param[in]   cmd_len	- command length
 * @return      1: success; others: error.
 * @note        
 */
u8 send_vc_fifo(u8 cmd,u8 *pfifo,u8 cmd_len)
{
	if(cmd_len>0x50){
		return 0;
	}
	u8 head[2] = {0};
	head[0] = cmd;
	head[1] = cmd_len;
	my_fifo_push_hci_tx_fifo(pfifo, cmd_len, head, 2);
	return 1;
}
#endif

//u8		dev_mac[12] = {0x00, 0xff, 0xff, 0xff, 0xff, 0xff,  0xc4,0xe1,0xe2,0x63, 0xe4,0xc7};
//s8		dev_rssi_th = 0x7f;

/**
 * @brief       This function enable device scan
 * @return      none
 * @note        
 */
void app_enable_scan_all_device (void)
{
#if (BLE_MULTIPLE_CONNECTION_ENABLE)
    #if EXTENDED_ADV_ENABLE
    blc_ll_setExtScanEnable(BLC_SCAN_ENABLE, DUPE_FLTR_DISABLE, SCAN_DURATION_CONTINUOUS, SCAN_WINDOW_CONTINUOUS);
    #else
	blc_ll_setScanEnable (BLC_SCAN_ENABLE, DUP_FILTER_DISABLE);
    #endif
#else
	blc_ll_setScanEnable (BLC_SCAN_ENABLE, DUP_FILTER_DISABLE);
#endif
}


/**
 * @brief       This function servers to enable/disable advertise.
 * @param[in]   adv_en	- 0: enable advertise  1: disable advertise
 * @return      0: set success; 1: fail
 * @note        this function will disable all advertisement. if want to disable unprovision beacon and connectable advertisement only, use mesh_gatt_adv_beacon_enable() instead.
 */
int mesh_set_adv_enable(int adv_en)
{
	if(blc_ll_getCurrentState() != BLS_LINK_STATE_CONN){
		#if BLE_MULTIPLE_CONNECTION_ENABLE
            #if EXTENDED_ADV_ENABLE
        blc_ll_setExtAdvEnable(adv_en, GATT_ADV_HANDLE, 0, 0);
            #else
		blc_ll_setAdvEnable(adv_en);
            #endif
		#else
		bls_ll_setAdvEnable(adv_en);
		#endif

		return SUCCESS;
	}

	return FAILURE;
}

/**
 * @brief       This function servers to enable/disable scan
 * @param[in]   immediate	- 0: take effect immediately after next adv. 1: take effect immediately.
 * @param[in]   scan_en	- 0: disable scan. 1: enable scan.
 * @return      SUCCESS
 * @note        
 */
int mesh_set_scan_enable(int scan_en, int immediate)
{
	if(scan_en){
		app_enable_scan_all_device();
	}
    else{
        #if (BLE_MULTIPLE_CONNECTION_ENABLE && EXTENDED_ADV_ENABLE)
        blc_ll_setExtScanEnable(BLC_SCAN_DISABLE, DUPE_FLTR_DISABLE, SCAN_DURATION_CONTINUOUS, SCAN_WINDOW_CONTINUOUS);
        #else
        blc_ll_setScanEnable (BLC_SCAN_DISABLE, DUP_FILTER_DISABLE);
        #endif
    }

    if(immediate){
        if(blc_ll_getCurrentState() != BLS_LINK_STATE_CONN){
            if(scan_en){
                mesh_send_adv2scan_mode(0);
            }
            else{
                rf_set_tx_rx_off();
            }
        }
    }

	return SUCCESS;
}

/**
 * @brief       This function enable advertise and scan function.
 * @param[in]   adv_en	- advertise enable
 * @param[in]   scan_en	- scan enable
 * @return      0: set success; 1: fail
 * @note        
 */
 int mesh_set_adv_scan_enable(int adv_en, int scan_en)
{
    mesh_set_scan_enable(scan_en, 0);
    int ret = mesh_set_adv_enable(adv_en);

    return ret;
}

/**
 * @brief       This function check whether mac addresses are matched.
 * @param[in]   mac		- mac address 
 * @param[in]   mask	- mask
 * @return      0: no matched; 1: matched
 * @note        
 */
int	app_device_mac_match (u8 *mac, u8 *mask)
{
	u8 m[6];
	for (int i=0; i<6; i++)
	{
		m[i] = mac[i] & mask[i];
	}
	return memcmp (m, mask + 6, 6) == 0;
}

// ------------set adv buffer--------

/**
 * @brief       This function set advertise random delay 
 * @param[in]   en	- delay enable flag
 * @return      none
 * @note        
 */
void set_random_adv_delay(int en)   // 0-10ms for mesh network PDU random delay
{
	// random from 0--10ms
	__UNUSED u8 cnt = 0;
	if(en){
	    if((blc_ll_getCurrentState() == BLS_LINK_STATE_CONN)
	    #if AUDIO_MESH_MULTI_NODES_TX_EN
	    || audio_mesh_is_reduce_relay_random()
	    #endif
	    ){
	        #if (!SIG_MESH_LOOP_PROC_10MS_EN) // no need for 8269
	        cnt = (rand() % 12) + 1;    // because ble window. a little shorter should be better
	        #endif
	    }else{
		    cnt = (rand() & 0x0F) + 1;	// unit : 625us; CMD_RAND_DELAY_MAX_MS
		}
	}
	bls_set_adv_delay(cnt);
}


/**
 * @brief       This function set random.
 * @param[in]   random_ms	- random time, unit ms.
 * @return      none
 * @note        
 */
void set_random_adv_delay_normal_adv(u32 random_ms)
{
    u32 cnt_max = GET_ADV_INTERVAL_LEVEL(random_ms);
	if(cnt_max){
		bls_set_adv_delay((rand() % cnt_max) + 1);	// unit : 625us; 
	}
}

#if 0
void set_ota_firmwaresize(int adr)  // if needed, must be called before cpu_wakeup_init()
{
	int firmware_size_k =0;
	firmware_size_k = (adr>>12)<<2;
	#if(MCU_CORE_TYPE == MCU_CORE_8269)
	bls_ota_setFirmwareSizeAndOffset(firmware_size_k,0x40000);
	#elif((MCU_CORE_TYPE == MCU_CORE_8258) || (MCU_CORE_TYPE == MCU_CORE_8278))
	    #if(PINGPONG_OTA_DISABLE)
    bls_ota_set_fwSize_and_fwBootAddr(firmware_size_k,adr);
	    #else
	bls_ota_set_fwSize_and_fwBootAddr(firmware_size_k,0x40000);
	    #endif
	#endif
}
#endif

#if !BLE_MULTIPLE_CONNECTION_ENABLE
int app_l2cap_packet_receive (u16 handle, u8 * raw_pkt)
{
	#if 0 //((HCI_ACCESS != HCI_USE_NONE) && (! DEBUG_MESH_DONGLE_IN_VC_EN && !TESTCASE_FLAG_ENABLE))
	u8 *p = raw_pkt + 12;
	u8 conn = 0x40;
	blc_hci_send_data (conn | HCI_FLAG_ACL_BT_STD, p, p[1]);	//dump packet to txFIFO
	#endif

	blc_l2cap_packet_receive (handle, raw_pkt);

	return 0;
}
#endif

int chn_conn_update_dispatch(u8 *p)
{
#if 0
	static u8 A_mesh_dbg_idle[40];
	static u8 A_mesh_dgb_cnt=0;
	memcpy(A_mesh_dbg_idle,p,sizeof(A_mesh_dbg_idle));
	A_mesh_dgb_cnt++;
#endif
	return 1;
}

void sim_tx_cmd_node2node()
{
#if 1
	static u32 sim_tx_last_time = 0;
	if(clock_time_exceed(sim_tx_last_time, 3000*1000)){
		sim_tx_last_time = clock_time();
		#if 0
		static u8 sim_key_code;
        if(0 == vd_cmd_key_report(ADR_ALL_NODES, sim_key_code)){
        	sim_key_code++;
        }
		#else
		static u8 sim_onoff;
        if(0 == access_cmd_onoff(0xffff, 1, sim_onoff, CMD_NO_ACK, 0)){
        	sim_onoff = !sim_onoff;
        }
        #endif
	}
#endif
}

void usb_id_init()
{
#if(MCU_CORE_TYPE == MCU_CORE_8269)
	REG_ADDR8(0x74) = 0x53;
#elif((MCU_CORE_TYPE == MCU_CORE_8258) || (MCU_CORE_TYPE == MCU_CORE_8278))
	REG_ADDR8(0x74) = 0x62;
#endif
	#if(MCU_CORE_TYPE == MCU_CORE_8269)
	#if MESH_MONITOR_EN
    REG_ADDR16(0x7e) = 0x08a1;			//mesh ID   0x08a1 means 0x08a0
	#else
		#if GATEWAY_ENABLE
		REG_ADDR16(0x7e) = 0x08d5;			//mesh ID   0x08d5 means 0x08d4
		#else
    	REG_ADDR16(0x7e) = 0x08d3;			//mesh ID   0x08d3 means 0x08d2
		#endif
    #endif
	#else
	#if MESH_MONITOR_EN
    REG_ADDR16(0x7e) = 0x08a0;			//mesh ID   0x08a1 means 0x08a0
	#else
		#if GATEWAY_ENABLE
		REG_ADDR16(0x7e) = 0x08d4;			//mesh ID   0x08d5 means 0x08d4
		#else
    	REG_ADDR16(0x7e) = 0x08d2;			//mesh ID   0x08d3 means 0x08d2
		#endif
	#endif
	#endif
	REG_ADDR8(0x74) = 0x00;
}

void ble_mac_init()
{
	#if RELAY_ROUTE_FILTE_TEST_EN
	flash_read_page(FLASH_ADR_RELAY_ROUTE_MAC, sizeof(relay_route_mac), (u8 *)relay_route_mac);
	u8 max_null[4] = {0xff,0xff,0xff,0xff};
	if(0 != memcmp(max_null, &relay_route_mac[0], sizeof(max_null))){
		relay_route_en = 1;
	}
	#endif

	if(AIS_ENABLE || LLSYNC_ENABLE || flash_sector_mac_address == 0){
		return ;
	}
	u8 mac_read[8];
	flash_read_page(flash_sector_mac_address, 8, mac_read);

	u8 ff_six_byte[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	if ( memcmp(mac_read, ff_six_byte, 6) ) {
		memcpy(tbl_mac, mac_read, 6);  //copy public address from flash
	}
	else{  //no public address on flash
        u8 value_rand[5];
        generateRandomNum(5, value_rand);
		tbl_mac[0] = value_rand[0];
		tbl_mac[1] = value_rand[1];
		tbl_mac[2] = value_rand[2];

		#if(MCU_CORE_TYPE == MCU_CORE_8258)
			tbl_mac[3] = 0x38;             //company id: 0xA4C138
			tbl_mac[4] = 0xC1;
			tbl_mac[5] = 0xA4;
		#elif(MCU_CORE_TYPE == MCU_CORE_8278)
			tbl_mac[3] = 0xD1;             //company id: 0xC119D1
			tbl_mac[4] = 0x19;
			tbl_mac[5] = 0xC4;
        #elif(MCU_CORE_TYPE == MCU_CORE_TC321X)
			tbl_mac[3] = 0x4A;             //company id: 0xD0AB4A
			tbl_mac[4] = 0xAB;
			tbl_mac[5] = 0xD0;
		#else
			tbl_mac[3] = U32_BYTE0(PDA_COMPANY_ID);
			tbl_mac[4] = U32_BYTE1(PDA_COMPANY_ID);
			tbl_mac[5] = U32_BYTE2(PDA_COMPANY_ID);
		#endif

		flash_write_page (flash_sector_mac_address, 6, tbl_mac);
	}
}

#if(AIS_ENABLE)
STATIC_ASSERT(sizeof(mesh_scan_rsp_t) >= 31);
#endif

_USER_CAN_REDEFINE_ void mesh_scan_rsp_init(void)
{
#if LLSYNC_PROVISION_AUTH_OOB
	llsync_tlk_init_scan_rsp();
	return ;
#elif (0 == USER_REDEFINE_SCAN_RSP_EN)
	mesh_scan_rsp_t tbl_scanRsp={0};
	tbl_scanRsp.type = GAP_ADTYPE_MANUFACTURER_SPECIFIC;	// manufacture data
	tbl_scanRsp.vendor_id = g_vendor_id;
	tbl_scanRsp.adr_primary = ele_adr_primary;
	memcpy(tbl_scanRsp.mac_adr, tbl_mac, sizeof(tbl_scanRsp.mac_adr));
	#if 0
	foreach(i,sizeof(tbl_scanRsp.rsv_user)){
		tbl_scanRsp.rsv_user[i] = 1 + i;
	}
	#endif
	
	#if (NLC_PROFILE_EN)
	tbl_scanRsp.len = OFFSETOF(mesh_scan_rsp_t, len_name) - 1;
	tbl_scanRsp.len_name = 1 + sizeof(tbl_scanRsp.name);
	tbl_scanRsp.type_name = GAP_ADTYPE_LOCAL_NAME_COMPLETE;
	void mesh_scan_rsp_add_local_name(u8 *p_name, u32 len_max);
	mesh_scan_rsp_add_local_name(tbl_scanRsp.name, sizeof(tbl_scanRsp.name));
	#else
	tbl_scanRsp.len = sizeof(mesh_scan_rsp_t) - 1;
	#endif
	
	u8 rsp_len = sizeof(mesh_scan_rsp_t);
	
	#if(AIS_ENABLE)
	rsp_len = ais_pri_data_set(&tbl_scanRsp.len);	
	#endif

    #if (BLE_MULTIPLE_CONNECTION_ENABLE && EXTENDED_ADV_ENABLE)
    blc_ll_setExtScanRspData(GATT_ADV_HANDLE, rsp_len, (const u8 *)&tbl_scanRsp);
    #else
	bls_ll_setScanRspData((u8 *)&tbl_scanRsp, rsp_len);
    #endif
#endif
}
#else
void mesh_ota_reboot_set(u8 type){}
void mesh_ota_reboot_check_refresh(){}
#endif

/**
 * @brief       This function is to push ADV bearer to fifo.
 * @param[in]   f	- pointer to FIFO resources
 * @param[in]   p	- buffer to be pushed
 * @param[in]   n	- length of buffer
 * @param[in]   ow	- if 1: means can be over write the oldest buffer in fifo when fifo if full.
 * @return      0: success. others: fail.
 * @note        different between my_fifo_push and my_fifo_push_adv:
 *              my_fifo_push:          no len in data, add len when push.
 *              my_fifo_push_adv: have len in data,  just use for mesh_cmd_bear.
 */
int my_fifo_push_adv(my_fifo_t* f, u8* p, u8 n, u8 ow)    // ow: can be over_write
{
	if (n > f->size)
	{
		return -1;
	}

	if (((f->wptr - f->rptr) & 255) >= f->num)
	{
		if (ow) {
			my_fifo_pop(f);
		}
		else {
			return -1;
		}
	}

	u32 r = irq_disable();
	u8* pd = f->p + (f->wptr & (f->num - 1)) * f->size;
	//*pd++ = n;
	//*pd++ = n >> 8;
	memcpy(pd, p, n);
	f->wptr++;			// should be last for VC
	irq_restore(r);
	return 0;
}

/**
 * @brief       This function pushed packet to fifo, then fifo will be sent by app_advertise_prepare_handler_.
 * @param[in]   p_bear	- packet to be pushed to fifo.
 * @return      0: success; others: error code of tx_errno_e.
 * @note
 */
int mesh_tx_cmd_add_packet(u8* p_bear)
{
	int err = 0;
#if FEATURE_FRIEND_EN
	if (use_mesh_adv_fifo_fn2lpn) {
		err = mesh_tx_cmd_add_packet_fn2lpn(p_bear);
	}
	else
#endif

#if (FEATURE_LOWPOWER_EN && LPN_PREPARE_NEXT_POLL_EN)
    if(use_mesh_adv_fifo_fn2lpn){
        err = mesh_tx_cmd_add_packet_fn_poll_cache(p_bear);
    }
    else
#endif
	{
		mesh_cmd_bear_t* p = (mesh_cmd_bear_t*)p_bear;
		//if((p->type == 0x2a) && (p->len == 0x15)){
			//LOG_MSG_LIB(TL_LOG_NODE_BASIC,0, 0,"into fifo: %d",my_fifo_data_cnt_get(&mesh_adv_cmd_fifo));
		//}
		err = my_fifo_push_adv(&mesh_adv_cmd_fifo, p_bear, mesh_bear_len_get(p), 0);
	}

	if (err) {
		err = TX_ERRNO_TX_FIFO_FULL;
	}
	return err;
}

u8 random_enable =1;
void set_random_enable(u8 en)
{
	random_enable = en;
	return;
}

#if MD_SERVER_EN
void publish_when_powerup(void)
{
	#if 0 // PAIR_PROVISION_ENABLE
	if(0 == is_provision_success()){
		// should not send message, because cache data will record the node address and sno, which will block message from provioner if they use the same node address.
		// no need return now, because provisioner and provisionee use different address range. refer to PAIR_PROV_UNPROV_ADDRESS_START.
		return ;
	}
	#endif
	
    #if SEND_STATUS_WHEN_POWER_ON
    user_power_on_proc();
    #endif

    st_pub_list_t pub_list = {{0}};
    foreach_arr(i,pub_list.st){
        pub_list.st[i] = ST_G_LEVEL_SET_PUB_NOW;
    }
    mesh_publish_all_manual(&pub_list, SIG_MD_LIGHTNESS_S, 1);
}
#endif

void mesh_vd_init()
{
#if MD_SERVER_EN
	light_res_sw_load();
	light_pwm_init();
	#ifndef WIN32
	    #if (FEATURE_LOWPOWER_EN)
	publish_when_powerup();
	    #else
	publish_powerup_random_ms = rand() % 1500;  // 0--1500ms
	STATIC_ASSERT(MESH_POWERUP_BASE_TIME >=200);    /* do not be too short, because when power up, after (this base time + random), 
						the node will publish lightness model or other model, then will trigger saving tx sequence number, then trigger flash write.
						it's better not to trigger flash write too early when power up, because the power supply may be not stable.*/
	publish_powerup_random_ms += MESH_POWERUP_BASE_TIME; // 200ms: base time.
	    #endif
	#endif
#endif
#if FAST_PROVISION_ENABLE
	mesh_fast_prov_val_init();
#endif

#if 1 // (ENCODE_OTA_BIN_EN || NORMAL_BIN_USER_NAME)
    FUNC_NULL_KEEP_VAL();
#endif
}

#ifdef WIN32
void APP_reset_vendor_id(u16 vd_id)
{
    g_vendor_id = vd_id;
    g_vendor_md_light_vc_s = (g_vendor_md_light_vc_s & 0xFFFF0000) | vd_id;
	#if MD_VENDOR_2ND_EN
    g_vendor_md_light_vc_s2 = (g_vendor_md_light_vc_s2 & 0xFFFF0000) | vd_id;
	#endif
    g_vendor_md_light_vc_c = (g_vendor_md_light_vc_c & 0xFFFF0000) | vd_id;
    APP_set_vd_id_mesh_save_map(vd_id);
    APP_set_vd_id_mesh_cmd_vd_func(vd_id);
}
#endif

#if (DUAL_VENDOR_EN)
void vendor_id_check_and_update(void) //confirm cps and vendor model
{
    if(DUAL_VENDOR_ST_ALI == provision_mag.dual_vendor_st){
        traversal_cps_reset_vendor_id(VENDOR_ID);
        vendor_md_cb_pub_st_set2ali();
    }
}
#endif

#if (MESH_USER_DEFINE_MODE == MESH_IRONMAN_MENLO_ENABLE)
//STATIC_ASSERT(STATIC_ADDR_MAC_MESH == (STATIC_DEV_INFO_ADR + 0x09));        // just for confirm, make sure not change
//STATIC_ASSERT(STATIC_ADDR_MESH_STATIC_OOB == (STATIC_DEV_INFO_ADR + 0x17)); // just for confirm, make sure not change

/**
 * this update the tx power by reading from flash offset
 *
 */
void app_txPowerCal()
{
    u8 tx_cal_power;
    flash_read_page(STATIC_ADDR_TX_PWR_OFFSET, 1, &tx_cal_power);

    /* this range will need to be reviewed */
    if (tx_cal_power >= RF_POWER_P3p23dBm &&
        tx_cal_power <= RF_POWER_P10p46dBm){
        my_rf_power_index = tx_cal_power;
    }else{
        /* default value is set in mesh_node.c */
        // my_rf_power_index = MY_RF_POWER_INDEX;
    }
}
#endif

/*
mesh_global_var_init(): run in mesh_init_all() and before read parameters in flash.
                                  it's used to set default value for compilation.
*/
void mesh_global_var_init()
{
    //get_fw_id();    // must first
#if __TLSR_RISCV_EN__ // others call in lib.
	/* random number generator must be initiated here( in the beginning of user_init_nromal).
	 * When deepSleep retention wakeUp, no need initialize again */
	random_generator_init();  //this is must. for B85m, it is called in rf drv init.
#endif
#ifndef WIN32
	blc_readFlashSize_autoConfigCustomFlashSector();
#endif
    mesh_factory_test_mode_en(FACTORY_TEST_MODE_ENABLE);
    user_mesh_cps_init();
    u8 dev_key_def[16] = DEVKEY_DEF;
    memcpy(mesh_key.dev_key, dev_key_def, sizeof(mesh_key.dev_key));
#if (SECURE_NETWORK_BEACON_LOOP_DISABLE || GATT_LPN_EN)
	model_sig_cfg_s.sec_nw_beacon = NW_BEACON_NOT_BROADCASTING;
#else
	model_sig_cfg_s.sec_nw_beacon = SEC_NW_BC_BROADCAST_DEFAULT; // (NODE_CAN_SEND_ADV_FLAG) ? SEC_NW_BC_BROADCAST_DEFAULT : NW_BEACON_NOT_BROADCASTING;
#endif
	model_sig_cfg_s.ttl_def = TTL_DEFAULT;
	
#if DEBUG_PROXY_FRIEND_SHIP
	#ifdef WIN32
	model_sig_cfg_s.frid = FEATURE_FRIEND_EN ? FRIEND_SUPPORT_ENABLE : FRIEND_NOT_SUPPORT;
	#else
	model_sig_cfg_s.frid = FRIEND_NOT_SUPPORT;
	#endif
	#if(SPIRIT_PRIVATE_LPN_EN)
	model_sig_cfg_s.frid = FRIEND_SUPPORT_DISABLE;
	#endif
#else
	#if (NODE_CAN_SEND_ADV_FLAG)
	model_sig_cfg_s.frid = FEATURE_FRIEND_EN ? FRIEND_SUPPORT_ENABLE : FRIEND_NOT_SUPPORT;
	#else
	model_sig_cfg_s.frid = FEATURE_FRIEND_EN ? FRIEND_SUPPORT_DISABLE : FRIEND_NOT_SUPPORT;
	#endif
#endif

	model_sig_cfg_s.gatt_proxy = FEATURE_PROXY_EN ? GATT_PROXY_SUPPORT_ENABLE : GATT_PROXY_NOT_SUPPORT;
	model_sig_cfg_s.nw_transmit.count = TRANSMIT_CNT_DEF;
	model_sig_cfg_s.nw_transmit.invl_steps = TRANSMIT_INVL_STEPS_DEF;
#if 0   // TEST_CASE_NODE_CFG_CFGR_BV01_EN in pts7_3_1.exe
	model_sig_cfg_s.relay = RELAY_NOT_SUPPORT;
#else
	model_sig_cfg_s.relay_retransmit.count = TRANSMIT_CNT_DEF_RELAY;
	model_sig_cfg_s.relay_retransmit.invl_steps = TRANSMIT_INVL_STEPS_DEF_RELAY;
	model_sig_cfg_s.relay = FEATURE_RELAY_EN ? RELAY_SUPPORT_ENABLE : RELAY_NOT_SUPPORT;
#endif
	
#if MESH_MODEL_MISC_SAVE_EN
	#if (/*MD_PRIVACY_BEA &&  */MD_SERVER_EN && !defined(WIN32)) 	//  can not use MD_PRIVACY_BEA,  due to OTA.
	mesh_privacy_beacon_save_t *p_beacon_srv = &g_mesh_model_misc_save.privacy_bc;
	//p_beacon_srv->beacon_sts = PRIVATE_BEACON_DISABLE;			// default value of globle has been 0. no need set again to save code.
	p_beacon_srv->random_inv_step = 0x3c;	// 10 min
	//p_beacon_srv->proxy_sts = PRIVATE_PROXY_DISABLE; 				// default value of globle has been 0. no need set again to save code.
	//p_beacon_srv->identity_sts = PRIVATE_NODE_IDENTITY_DISABLE; 	// default value of globle has been 0. no need set again to save code.
	#endif

	#if 1 // can not use MD_SAR_EN, because if disable MD_SAR_EN first, then OTA a new firmware with enable MD_SAR_EN will cause that all parameters will be 0.
	g_mesh_model_misc_save.sar_transmitter.sar_seg_invl_step = SAR_SEG_INVL_STEP_DEF;
	g_mesh_model_misc_save.sar_transmitter.sar_uni_retrans_cnt = SAR_UNICAST_RETRANS_CNT_DEF;
	g_mesh_model_misc_save.sar_transmitter.sar_uni_retrans_cnt_no_ack = SAR_UNICAST_RETRANS_CNT_NO_ACK_DEF;
	g_mesh_model_misc_save.sar_transmitter.sar_uni_retrans_invl_step = SAR_UNICAST_RETRANS_INVL_STEP_DEF;
	g_mesh_model_misc_save.sar_transmitter.sar_uni_retrans_invl_incre = SAR_UNICAST_RETRANS_INVL_INCRE;
	g_mesh_model_misc_save.sar_transmitter.sar_multi_retrans_cnt = SAR_MULTICAST_RETRANS_CNT_DEF;
	g_mesh_model_misc_save.sar_transmitter.sar_multi_retrans_invl = SAR_MULTICAST_RETRANS_INVL_DEF;  
		
	g_mesh_model_misc_save.sar_receiver.sar_seg_thres = SAR_SEG_THRESHOLD_DEF;
	g_mesh_model_misc_save.sar_receiver.sar_ack_delay_inc = SAR_ACK_DELAY_INC_DEF;
	g_mesh_model_misc_save.sar_receiver.sar_discard_timeout = SAR_DISCARD_TIMEOUT_DEF;
	g_mesh_model_misc_save.sar_receiver.sar_ack_retrans_cnt = SAR_ACK_RETRANS_CNT_DEF;
	g_mesh_model_misc_save.sar_receiver.sar_rcv_seg_invl_step = SAR_RCV_SEG_INVL_STEP_DEF;

	g_mesh_model_misc_save.on_demand_proxy = ON_DEMAND_PRIVATE_GATT_PROXY_S;
	#endif
#endif
#if (MD_DF_CFG_SERVER_EN && !defined(WIN32))
	mesh_directed_forwarding_default_val_init();
#endif	
#if DUAL_VENDOR_EN
	provision_mag.dual_vendor_st = DUAL_VENDOR_ST_STANDBY;
#endif
#if (FEATURE_FRIEND_EN)
	mesh_global_var_init_fn_buf();
#endif
#if MD_SERVER_EN
	mesh_global_var_init_light_sw();
    #if (MD_SENSOR_SERVER_EN)
	mesh_global_var_init_sensor_descrip();
    #endif
    #if (MD_PROPERTY_EN)
    mesh_property_global_init();
    #endif
    #if (MD_LIGHT_CONTROL_EN)
    light_LC_global_init();
    #endif
#endif

#if (MESH_USER_DEFINE_MODE == MESH_IRONMAN_MENLO_ENABLE)
    /* apply tx power calibration value */
    app_txPowerCal();
#endif
}
/****************************************************
struct of the unprovisioned beacon 
typedef struct __attribute__((packed)) {
	mesh_beacon_header header;
	u8 beacon_type;
	u8 device_uuid[16];
	u8 oob_info[2];
	u8 uri_hash[4];
}beacon_data_pk;
*******************************************************/
void set_unprov_beacon_para(u8 *p_uuid ,u8 *p_info)
{
	if(p_uuid!=0){
		memcpy(prov_para.device_uuid,p_uuid,16);
	}
	if(p_info !=0){
		memcpy(prov_para.oob_info,p_info,2);
	}

	#if 0   // please modify URI_DATA directly.
	if(uri_para !=0){
		memcpy(uri_dat,uri_para,uri_len);
	}
	#endif
}
/**************************************************
typedef struct __attribute__((packed)) {
	u8 flag_len;
	u8 flag_type;
	u8 flag_content;
	u8 uuid_len;
	u8 uuid_type;
	u8 uuid_pb_uuid[2];
	u8 service_len;
	u8 service_type;
	u8 service_uuid[2];
	u8 service_data[16];
	u8 oob_info[2];
}PB_GATT_ADV_DAT;
**************************************************/
void set_provision_adv_data(u8 *p_uuid,u8 *oob_info)
{
	if(p_uuid != 0){
		memcpy(prov_para.device_uuid,p_uuid,16);
	}
	if(oob_info!=0){
		memcpy(prov_para.oob_info,oob_info,2);
	}
	return ;
}
/************************************************
typedef struct __attribute__((packed)) {
	u8 flag_len;
	u8 flag_type;
	u8 flag_data;
	u8 uuid_len;
	u8 uuid_type;
	u8 uuid_data[2];
	u8 serv_len;
	u8 serv_type;
	u8 serv_uuid[2];
	u8 identify_type;
	u8 hash[8];
	u8 random[8];
}proxy_adv_node_identity;
*************************************************/


/* 
	tx cmd interface.
	don't modify this interfaces, library use internal.
	use nk_array_idx instead of nk_idx, because no need to calculate nk index for response.
*/
void set_material_tx_cmd(material_tx_cmd_t *p_mat, u16 op, u8 *par, u32 par_len,
							u16 adr_src, u16 adr_dst, u8 retry_cnt, int rsp_max, u8 *uuid, 
							u8 nk_array_idx, u8 ak_array_idx, model_common_t *pub_md,
							u16 conn_handle, u8 immutable_flag, bear_head_t *p_tx_head)
{
	mesh_op_resource_t op_res;
	memset(p_mat, 0, sizeof(material_tx_cmd_t));
	p_mat->conn_handle = conn_handle; 	// if adr_dst is not 0, connection handle dependent on destination address when push fifo in mesh_nw_pdu_report_to_gatt().
	p_mat->immutable_flag = immutable_flag;
	p_mat->op = op;
	p_mat->p_ac = par;
	p_mat->len_ac = par_len;
	p_mat->adr_src = adr_src;
	p_mat->adr_dst = adr_dst;
	p_mat->retry_cnt = retry_cnt;
	p_mat->rsp_max = rsp_max;
#if SPIRIT_PRIVATE_LPN_EN 
	if(0 == p_tx_head)
	{
		bear_head_t tx_head = {0};
		p_tx_head = &tx_head;
	}
	p_tx_head->par_type = BEAR_TX_PAR_TYPE_REMAINING_TIMES;
	p_tx_head->val[0] = 16;// 20 - (TRANSMIT_CNT + 1); // extend count, keep sending (16+7)*10ms
#elif (0) // keep sending 2.5s if need
	if(0x0002 == adr_dst){
		if(0 == p_tx_head)
		{
			bear_head_t tx_head = {0};
			p_tx_head = &tx_head;
		}
		p_tx_head->par_type = BEAR_TX_PAR_TYPE_REMAINING_TIMES;
		p_tx_head->val[0] = 255;// extend count, interval 10ms
	}
#elif (0 && GATEWAY_ENABLE) // for spirit lpn test
	if(0 == p_tx_head)
	{
		bear_head_t tx_head = {0};
		p_tx_head = &tx_head;
	}
	p_tx_head->par_type = BEAR_TX_PAR_TYPE_REMAINING_TIMES;
	p_tx_head->val[0] = 100;// extend count, interval 10ms
	
	if(0 == mesh_search_model_id_by_op(&op_res, op, 1)){
		if(is_cfg_model(op_res.id, 1) && ((COMPOSITION_DATA_GET == op) || (APPKEY_ADD == op) || (MODE_APP_BIND == op))){ 
			p_tx_head->par_type = BEAR_TX_PAR_TYPE_NONE;
		}
	}	
#endif
#if DU_ENABLE
	if(0 == p_tx_head)
	{
		bear_head_t tx_head = {0};
		p_tx_head = &tx_head;
	}
	p_tx_head->par_type = BEAR_TX_PAR_TYPE_REMAINING_TIMES;
	p_tx_head->val[0] = 16;
#endif
#if MESH_RX_TEST
	if((op >= CMD_CTL_TTC_CMD) && (op <= CMD_CTL_TTC_CMD_STATUS)){
		if(0 == p_tx_head)
		{
			bear_head_t tx_head = {0};
			p_tx_head = &tx_head;
		}
		p_tx_head->par_type = BEAR_TX_PAR_TYPE_MESH_RX_TEST;
		u32 base_tick = clock_time() >> RX_TEST_BASE_TIME_SHIFT;
		p_tx_head->val[0] = (u8)base_tick;
		p_tx_head->val[1] = base_tick >> 8;
		p_tx_head->val[2] = base_tick >> 16;
	}
#endif
#if GATEWAY_ENABLE
	if(OP_TYPE_VENDOR == GET_OP_TYPE(op)){
	    // initial as local vendor id at first, and it will be replaced if command is from INI.
	    u32 op_rsp = get_op_rsp(op);
	    if(STATUS_NONE != op_rsp){
	        p_mat->op_rsp = op_rsp & 0xFF;
	    }
	    p_mat->op_rsp |= (g_vendor_id << 8);
	}

	#if MD_MESH_OTA_EN
	if(!is_own_ele(adr_dst) && (0 == mesh_search_model_id_by_op(&op_res, op, 1))){
		if((SIG_MD_FW_UPDATE_C == op_res.id) || (SIG_MD_FW_DISTRIBUT_C == op_res.id) || (SIG_MD_BLOB_TRANSFER_C == op_res.id) ||
			((CFG_MODEL_SUB_ADD == op) && (MASTER_OTA_ST_SUBSCRIPTION_SET == fw_distribut_srv_proc.st_distr))){ 
			nk_array_idx = fw_distribut_srv_proc.netkey_sel_enc;
			ak_array_idx = fw_distribut_srv_proc.appkey_sel_enc;
		}
	}
	#endif
#endif
	
	if((0 == nk_array_idx)&&(0 == ak_array_idx)){	// use the first valid key
		nk_array_idx = get_nk_arr_idx_first_valid();
		ak_array_idx = get_ak_arr_idx_first_valid(nk_array_idx);
	}
	p_mat->nk_array_idx = nk_array_idx;
	p_mat->ak_array_idx = ak_array_idx;
	if(is_own_ele(adr_dst) || !is_unicast_adr(adr_dst)){
		mesh_key.netkey_sel_dec = nk_array_idx;
		mesh_key.appkey_sel_dec = ak_array_idx;

		if(!is_own_ele(adr_src)){
			if(0 == mesh_search_model_id_by_op(&op_res, op, 1)){
				if(is_use_device_key(op_res.id, op_res.sig)){ 
					mesh_key.appkey_sel_dec = -1;	// need set to -1(means decryption by decive key) when convert vendor message into sending config message to self without changing source address which maybe src addr of gateway.
				}
			}
		}
	}
	p_mat->pub_md = pub_md;
	if(p_tx_head){
		memcpy(&p_mat->tx_head, p_tx_head, sizeof(p_mat->tx_head));
	}
	
	if(pub_md){
		p_mat->tx_head.par_type = BEAR_TX_PAR_TYPE_PUB_FLAG; // model publish flag // will cover other type
	}else if(mesh_tx_with_random_delay_ms){
		// par_type will be set to "BEAR_TX_PAR_TYPE_DELAY" in library
	}
	
	if(uuid){
		p_mat->uuid = uuid;
		if(is_virtual_adr(adr_dst)){
			// no need to recalculate
		}else{
			p_mat->adr_dst = cala_vir_adr_by_uuid(uuid);
		}
	}

#if DEPRECATED_MESH_OTA_COMPAT
    p_mat->op = mesh_ota_adapt_tx_opcode(op);
#endif
}

/**
 * @brief       This function tx command.
 * @param[in]   p	- tx parameters
 * @return      0: success. others fail, please refer to "tx_errno_e"
 * @note        
 */
int mesh_tx_cmd(material_tx_cmd_t *p)
{
	if(mesh_adr_check(p->adr_src, p->adr_dst, 1)){
	    LOG_MSG_ERR(TL_LOG_MESH,0, 0 ,"src or dst is invalid");
		return TX_ERRNO_ADDRESS_INVALID;
	}

	#if RELIABLE_CMD_EN
	int reliable = is_reliable_cmd(p->op, p->op_rsp);
	
    if(reliable){	
		if(is_unicast_adr(p->adr_dst)){
			p->rsp_max = 1;
		}
		
        return mesh_tx_cmd_reliable(p);
    }else
    #endif
    {
        //if(mesh_tx_reliable.busy){
            //mesh_tx_reliable_finish();     // can send reliable and unreliable command at the same time.
        //}
        return mesh_tx_cmd_unreliable(p);
    }
}

static inline int mesh_tx_cmd2normal_2(u16 op, u8 *par, u32 par_len, u16 adr_src, u16 adr_dst, int rsp_max, bear_head_t *p_tx_head)
{
	material_tx_cmd_t mat;
    u8 nk_array_idx = get_nk_arr_idx_first_valid();
    u8 ak_array_idx = get_ak_arr_idx_first_valid(nk_array_idx);

	u8 immutable_flag = 0;
	set_material_tx_cmd(&mat, op, par, par_len, adr_src, adr_dst, g_reliable_retry_cnt_def, rsp_max, 0, nk_array_idx, ak_array_idx, 0, MESH_CONN_HANDLE_AUTO, immutable_flag, p_tx_head);
	return mesh_tx_cmd(&mat);
}

int mesh_tx_cmd2normal(u16 op, u8 *par, u32 par_len, u16 adr_src, u16 adr_dst, int rsp_max)
{
	return mesh_tx_cmd2normal_2(op, par, par_len, adr_src, adr_dst, rsp_max, 0);
}

/**
 * @brief       This function tx command with primary address as source address
 * @param[in]   op		- opcode
 * @param[in]   par		- parameter
 * @param[in]   par_len	- parameter length
 * @param[in]   adr_dst	- destination address
 * @param[in]   rsp_max	- max number of response expected from nodes which match destination address, after sending the opcode.
 * @return      0: success. others fail, please refer to "tx_errno_e"
 * @note        
 */
int mesh_tx_cmd2normal_primary(u16 op, u8 *par, u32 par_len, u16 adr_dst, int rsp_max)
{
	#if __PROJECT_MESH_SWITCH__
	extern u16 sw_tx_src_addr_offset;
	u16 addr_src = ele_adr_primary + sw_tx_src_addr_offset;
	sw_tx_src_addr_offset = 0;
	#else
	u16 addr_src = ele_adr_primary;
	#endif
	
	return mesh_tx_cmd2normal(op, par, par_len, addr_src, adr_dst, rsp_max);
}

int mesh_tx_cmd2normal_with_tx_head(u16 op, u8 *par, u32 par_len, u16 adr_src, u16 adr_dst, int rsp_max, bear_head_t *p_tx_head)
{
	return mesh_tx_cmd2normal_2(op, par, par_len, adr_src, adr_dst, rsp_max, p_tx_head);
}

#if 1//(MD_DF_CFG_SERVER_EN || MD_DF_CFG_CLIENT_EN)
int mesh_tx_cmd2normal_specified_key(u16 op, u8 *par, u32 par_len, u16 adr_src, u16 adr_dst, int rsp_max, u16 netkey_index, u16 appkey_index)
{
	material_tx_cmd_t mat;

	u8 nk_array_idx = get_nk_arr_idx(netkey_index);
    u8 ak_array_idx = get_ak_arr_idx(nk_array_idx, appkey_index);
    if(nk_array_idx >= NET_KEY_MAX){
        nk_array_idx = ak_array_idx = 0;    // use first valid key later
    }
	else if(ak_array_idx >= APP_KEY_MAX){
		ak_array_idx = get_ak_arr_idx_first_valid(nk_array_idx);
	}
	u8 immutable_flag = 0;
	set_material_tx_cmd(&mat, op, par, par_len, adr_src, adr_dst, g_reliable_retry_cnt_def, rsp_max, 0, nk_array_idx, ak_array_idx, 0, MESH_CONN_HANDLE_AUTO, immutable_flag, 0);
	return mesh_tx_cmd(&mat);
}

int mesh_tx_cmd2normal_primary_specified_key(u16 op, u8 *par, u32 par_len, u16 adr_dst, int rsp_max, u16 netkey_index, u16 appkey_index)
{
	return mesh_tx_cmd2normal_specified_key(op, par, par_len, ele_adr_primary, adr_dst, rsp_max, netkey_index, appkey_index);
}
#endif

int mesh_tx_cmd2uuid(u16 op, u8 *par, u32 par_len, u16 adr_src, u16 adr_dst, int rsp_max, u8 *uuid)
{
	material_tx_cmd_t mat;
    u8 nk_array_idx = get_nk_arr_idx_first_valid();
    u8 ak_array_idx = get_ak_arr_idx_first_valid(nk_array_idx);
	u8 immutable_flag = 1;

	set_material_tx_cmd(&mat, op, par, par_len, adr_src, adr_dst, g_reliable_retry_cnt_def, rsp_max, uuid, nk_array_idx, ak_array_idx, 0, MESH_CONN_HANDLE_AUTO, immutable_flag, 0);
	return mesh_tx_cmd(&mat);
}

int SendOpParaDebug(u16 adr_dst, u8 rsp_max, u16 op, u8 *par, int len)
{
	#ifdef WIN32
	return SendOpParaDebug_VC(adr_dst, rsp_max, op, par, len);
	#else
	LOG_MSG_LIB(TL_LOG_NODE_SDK,0, 0,"call send cmd API, sno:0x%06x op:0x%04x",mesh_adv_tx_cmd_sno,op);
	return mesh_tx_cmd2normal_primary(op, par, len, adr_dst, rsp_max);
	#endif
}

int SendOpParaDebug_vendor(u16 adr_dst, u8 rsp_max, u16 op, u8 *par, int len, u8 rsp_op, u8 tid_pos)
{
#ifdef WIN32
	u8 par_tmp[MESH_CMD_ACCESS_LEN_MAX+2];
	par_tmp[0] = (u8)g_vendor_id;
	par_tmp[1] = g_vendor_id>>8;
	par_tmp[2] = rsp_op;
	par_tmp[3] = tid_pos;
	memcpy(par_tmp+4, par, len);
	return SendOpParaDebug_VC(adr_dst, rsp_max, op, par_tmp, len+4);
#else
	LOG_MSG_LIB(TL_LOG_NODE_SDK,par, len,"send access cmd vendor op:0x%02x,par:",op);
	return mesh_tx_cmd2normal_primary(op, par, len, adr_dst, rsp_max);
#endif
}

/********
    user can use mesh_tx_cmd() or mesh_tx_cmd2self_primary() to send command to self.
*/
int mesh_tx_cmd2self_primary(u32 light_idx, u8 *ac, int len_ac)    // ac: op code + par
{
    if(light_idx >= g_ele_cnt){
        return -1;
    }
    
    mesh_cmd_nw_t nw = {0};
    nw.src = ele_adr_primary;
    nw.dst = ele_adr_primary + light_idx;
    u32 size_op = SIZE_OF_OP(ac[0]);
    u16 op = get_op_u16(ac);
    is_cmd_with_tid_check_and_set(nw.src, op, ac+size_op, len_ac - size_op, 0);
    return mesh_rc_data_layer_access(ac, len_ac, &nw);
}

int is_need_response_to_self(u16 adr_dst, u16 op)
{
#if (!MD_CLIENT_EN)
    if(is_own_ele(adr_dst)){
        if(0){   // is resp_op
            return 0;
        }
    }
#endif

    return 1;
}
//-------------------------------- end

void mesh_rsp_delay_set(u32 delay_step, u8 is_seg_ack)
{	
#ifndef WIN32
#if SPIRIT_PRIVATE_LPN_EN
	if(lpn_provision_ok){ 
		delay_step = MESH_RSP_BASE_DELAY_STEP + (rand() %10);
	}
#endif
	if(delay_step == 0 || (blc_ll_getCurrentState() == BLS_LINK_STATE_CONN)){
		return;
	}
	mesh_tx_with_random_delay_ms = delay_step*10;
#endif
}

// -------------------------------  uart module---------
int my_fifo_push_hci_tx_fifo (u8 *p, u16 n, u8 *head, u8 head_len)
{
#if (HCI_ACCESS != HCI_USE_NONE)
	int ret = my_fifo_push(&hci_tx_fifo, p, n, head, head_len);
	if (-1 == ret){
		LOG_MSG_INFO(TL_LOG_MESH,0, 0,"my_fifo_push_hci_tx_fifo:debug printf tx FIFO overflow %d",ret);
	}
	return ret;
#else
	return -1;
#endif
}

#if UART_SECOND_EN
int my_fifo_push_hci_tx_fifo_2nd (u8 *p, u16 n, u8 *head, u8 head_len)
{
    int ret = my_fifo_push(&hci_tx_fifo_2nd, p, n, head, head_len);
    if (-1 == ret){
        LOG_MSG_INFO(TL_LOG_MESH,0, 0,"my_fifo_push_hci_tx_fifo_2nd:debug printf tx FIFO overflow %d",ret);
    }
    
    return ret;
}
#endif

int hci_send_data_user (u8 *para, int n)
{
    if(n > (HCI_TX_FIFO_SIZE - 2 - 1)){ // 2: size of length,  1: size of type
        return -1;
    }
    
	u8 head[1] = {HCI_RSP_USER};
	return my_fifo_push_hci_tx_fifo(para, n, head, 1);
}

#if GATEWAY_ENABLE
void mesh_tx_reliable_stop_report(u16 op,u16 dst, u32 rsp_max, u32 rsp_cnt)
{
	LOG_MSG_LIB(TL_LOG_NODE_SDK,0,0,"mesh_tx_reliable_stop: op:0x%04x,rsp_max:%d,rsp_cnt:%d", op, rsp_max, rsp_cnt);
	if(op == NODE_RESET){
		del_vc_node_info_by_unicast(dst);  
	}
}
#endif

#if (GATEWAY_ENABLE || defined(WIN32))
#define MAX_SEG_NUM_CNT max2(ACCESS_WITH_MIC_LEN_MAX,MESH_OTA_UPDATE_NODE_MAX*2+0x10)
u8 gateway_seg_buf[MAX_SEG_NUM_CNT];
u16 gateway_seg_buf_len;
int gateway_sar_pkt_reassemble(u8 *buf,int len )
{
	u8 type =0;
	type = buf[0];
	if((len <= 0) || (len >= MAX_SEG_NUM_CNT)){
		gateway_seg_buf_len =0;
		return ERR_PACKET_LEN;
	}
	if(type == SAR_COMPLETE){
		memcpy(gateway_seg_buf,buf+1,len-1);
		gateway_seg_buf_len = len-1;
		return  RIGHT_PACKET_RET;
	}else if (type == SAR_START){
		gateway_seg_buf_len =0;
		memcpy(gateway_seg_buf,buf+1,len-1);
		gateway_seg_buf_len += len-1;
		return PACKET_WAIT_COMPLETE;
	}else if (type == SAR_CONTINUE){
		memcpy(gateway_seg_buf+gateway_seg_buf_len,buf+1,len-1);
		gateway_seg_buf_len += len-1;
		if(gateway_seg_buf_len >= MAX_SEG_NUM_CNT){
			gateway_seg_buf_len =0;
			return ERR_PACKET_LEN;
		}
		return PACKET_WAIT_COMPLETE;
	}else if (type == SAR_END){
		if( (gateway_seg_buf_len +len-1 )>= MAX_SEG_NUM_CNT){
			gateway_seg_buf_len =0;
			return ERR_PACKET_LEN;
		}
		memcpy(gateway_seg_buf+gateway_seg_buf_len,buf+1,len-1);
		gateway_seg_buf_len +=len-1;
		return RIGHT_PACKET_RET;
	}else{
		return ERR_TYPE_SAR;
	}
}

#if VC_APP_ENABLE
extern void WriteFile_handle(u8 *buff, int n, u8 *head, u8 head_len);
#endif
int gateway_sar_pkt_segment(u8 *p_par,int par_len, u16 valid_fifo_size, u8 *p_head, u8 head_len)
{
	int ret = -1;
	u8 head[8] = {HCI_GATEWAY_CMD_SAR_MSG, SAR_START}; // add sar head
	u16 seg_par_len = 0;
	memcpy(head+2, p_head, ((u32)(head_len+2)>sizeof(head))?(sizeof(head)-2):head_len);
	head_len += 2;
	seg_par_len = valid_fifo_size - head_len;
	
	if(valid_fifo_size >= (par_len + head_len)){// whole packet 
		head[1] = SAR_COMPLETE;
		my_fifo_push_hci_tx_fifo(p_par,par_len, head, head_len);
		return 1;
	}else
	{// first packet
		head[1] = SAR_START;
		#if VC_APP_ENABLE		
		WriteFile_handle(p_par,seg_par_len, head, head_len);
		#else
		ret = my_fifo_push_hci_tx_fifo(p_par,seg_par_len, head, head_len);
		#endif
		par_len -= seg_par_len;
		p_par += seg_par_len;
	}

	seg_par_len = valid_fifo_size - 2;
	while(par_len){
		if(par_len > seg_par_len){//continue packet 
			head[1] = SAR_CONTINUE;
			#if VC_APP_ENABLE
			WriteFile_handle(p_par,seg_par_len, head, head_len);
			#else
			ret = my_fifo_push_hci_tx_fifo(p_par,seg_par_len, head, 2);
			#endif
			par_len -= seg_par_len;
			p_par += seg_par_len;
			
		}else{
			head[1] = SAR_END;// last packet 
			#if VC_APP_ENABLE
			WriteFile_handle(p_par,par_len, head, head_len);
			#else
			ret = my_fifo_push_hci_tx_fifo(p_par,par_len, head, 2);
			#endif
			par_len =0;
		}
	}

	return ret;
}

#endif

//------------ command from upper  -------------------------------------------------
#if (!IS_VC_PROJECT)
int app_hci_cmd_from_usb (void)	// be called in blt_sdk_main_loop()
{
	//int ret = 0;
	static u8 buff[72];
	extern int usb_bulk_out_get_data (u8 *p);
	int n = usb_bulk_out_get_data (buff);
	if (n){
		/*ret =*/ app_hci_cmd_from_usb_handle(buff, n);
	}
	return 0;
}
#endif

int app_hci_cmd_from_usb_handle (u8 *buff, int n) // for both usb and uart
{
	int ret = -1;
	if(n <= 2){		// error len
		return ret;
	}

	static u32 app_uuu;
    app_uuu++;
#if TESTCASE_FLAG_ENABLE    
    mesh_rc_cmd_tscript(buff, n);//2bytes:0xa0 0xff 
#endif

#if GATT_RP_EN
	rp_gatt_hci_rcv(buff+2,buff[0]);	
#endif
	
	if(buff[0] == HCI_GATEWAY_CMD_SAR_MSG){
		#if (GATEWAY_ENABLE)
		if(gateway_sar_pkt_reassemble(buff+1,n-1) == RIGHT_PACKET_RET){
			n = gateway_seg_buf_len;
			buff = gateway_seg_buf;
			ret = 0;
		}
		else{
			return ret;
		}
		#endif
	}

	u16 type = buff[0] + (buff[1] << 8);
    u8 *hci_data = buff + HCI_CMD_LEN;
    u16 hci_data_len = n - HCI_CMD_LEN;
	
	if (HCI_CMD_BULK_CMD2MODEL == type)
	{
        ret = mesh_bulk_cmd2model(hci_data, hci_data_len);
	}
	else if (HCI_CMD_BULK_CMD2DEBUG == type)
	{
        ret = mesh_bulk_cmd((mesh_bulk_cmd_par_t *)hci_data, hci_data_len);
	}
	#if (!IS_VC_PROJECT)
	#if (PROXY_HCI_SEL == PROXY_HCI_USB)
	else if(HCI_CMD_ADV_PKT == type){
		ret = mesh_nw_pdu_from_gatt_handle(hci_data);
	}
	#endif
	#if DEBUG_MESH_DONGLE_IN_VC_EN
	else if(HCI_CMD_ADV_DEBUG_MESH_DONGLE2BEAR == type){
		ret = mesh_tx_cmd_add_packet(hci_data);
	}
	else if(HCI_CMD_ADV_DEBUG_MESH_DONGLE2GATT == type){
		if(mesh_is_proxy_ready()){	// is_proxy_support_and_en_active
			ret = mesh_proxy_adv2gatt(hci_data, MESH_ADV_TYPE_MESSAGE);
		}
	}
	else if(HCI_CMD_ADV_DEBUG_MESH_LED == type){
		mesh_light_control_t *p = (mesh_light_control_t *)(hci_data);
		pwm_set_lum(p->id, p->val, p->pol);
		ret = 0;
	}
	#endif
	#endif
	else if (HCI_CMD_BULK_SET_PAR == type)
	{
		// set parameters of VC node
    	ret = mesh_bulk_set_par(hci_data, hci_data_len);
	}
	else if (HCI_CMD_BULK_SET_PAR2USB == type)
	{
		#if IS_VC_PROJECT
		ret = fifo_push_vc_cmd2dongle_usb(buff, n);
		#else
    	ret = mesh_bulk_set_par2usb(hci_data, hci_data_len);
    	#endif
	}
	#if INITIATOR_CLIENT_EN
	else if (HCI_CMD_MESH_OTA_INITIATOR_START == type)
	{
	    mesh_ota_initiator_start(hci_data, hci_data_len);
	}
	#endif
	#if (PTS_TEST_EN || PTS_TEST_OTA_EN)
	else if (HCI_CMD_MESH_PTS_TEST_CASE_CMD == type)
	{
		void gw_ini_pts_test_case_cmd(u8 *par, int par_len); // for VC compile
	    gw_ini_pts_test_case_cmd(hci_data, hci_data_len);
	}
	#endif
	else if (HCI_CMD_PROVISION == type)
	{
		#if FEATURE_PROV_EN
		ret = mesh_provision_cmd_process();   
	   	#endif
	}
	#if 0
	else if (buff[0] == 0xe0 && buff[1] == 0xff)			//set mac address filter
	{
		#if 0
		extern s8 dev_rssi_th;
		extern u8 dev_mac[];
		dev_rssi_th = buff[4];
		memcpy (dev_mac, buff + 5, 12);
		#endif
	}
	#endif
	else if (HCI_CMD_USER_DEFINE == type)	//user defined command
	{
		app_led_en (buff[4], buff[5]);
	}else if (HCI_CMD_GATEWAY_CMD == type){
		//gateway cmd
		#if IS_VC_PROJECT
		ret = fifo_push_vc_cmd2dongle_usb(buff, n);
		#else
		#if GATEWAY_ENABLE
		// if the cmd is the node reset cmd ,need to proc the vc node info part
		ret = mesh_bulk_cmd((mesh_bulk_cmd_par_t *)(hci_data), hci_data_len);
			#if (MD_MESH_OTA_EN && (DISTRIBUTOR_UPDATE_CLIENT_EN || DISTRIBUTOR_UPDATE_SERVER_EN))
		if(0 == ret){
			mesh_ota_check_and_set_lpn_retry_intv((mesh_bulk_cmd_par_t *)(hci_data), hci_data_len);
		}
			#endif
		#endif
		#endif
	}else if (HCI_CMD_GATEWAY_CTL == type){
		#if IS_VC_PROJECT
		ret = fifo_push_vc_cmd2dongle_usb(buff, n);
		#else
		#if GATEWAY_ENABLE
    	ret = gateway_cmd_from_host_ctl(hci_data, hci_data_len);
    	#endif
		#endif 
	}else if (HCI_CMD_GATEWAY_OTA == type){
		#if GATEWAY_ENABLE
		ret = gateway_cmd_from_host_ota(hci_data, hci_data_len);
		#endif
	}else if (HCI_CMD_MESH_OTA == type){
		#if GATEWAY_ENABLE
		ret = gateway_cmd_from_host_mesh_ota(hci_data, hci_data_len);
		#endif
	}
	else if(HCI_CMD_GATEWAY_ONLINE_SET == type){
		#if ONLINE_STATUS_EN
		mesh_report_status_enable(hci_data[0]);
		#elif GATEWAY_ENABLE
		u8 data[1] = {0};
		gateway_common_cmd_rsp(HCI_GATEWAY_CMD_ONLINE_ST , data, 0);
		#endif
	}
	else{
		#if IS_VC_PROJECT
		ret = fifo_push_vc_cmd2dongle_usb(buff, n);
		#elif (__PROJECT_8267_MASTER_KMA_DONGLE__)
		ret = blc_hci_handler (buff, n);
		#endif
	}
	return ret;
}


#if (HCI_ACCESS==HCI_USE_UART)
#define UART_HW_HEAD_LEN    (4 + 2) //4:uart dma_len,  2: uart margin

#if(HCI_LOG_FW_EN)
_align_4_ u8 uart_hw_tx_buf[160 + UART_HW_HEAD_LEN]; // not for user application buffer, but can redefine size.
#else
_align_4_ u8 uart_hw_tx_buf[HCI_TX_FIFO_SIZE_USABLE + UART_HW_HEAD_LEN]; // not for user application buffer;  2: sizeof(fifo.len)
#endif
const u16 UART_TX_LEN_MAX = (sizeof(uart_hw_tx_buf) - UART_HW_HEAD_LEN);


int blc_rx_from_uart (void)
{
	uart_ErrorCLR();
	
	uart_data_t* p = (uart_data_t *)my_fifo_get(&hci_rx_fifo);
	if(p){
		u32 rx_len = p->len & 0xff; //usually <= 255 so 1 byte should be sufficient
		if (rx_len)
		{
			#if 0 // Serial loop test
			#if 1
			if(0 == my_fifo_push_hci_tx_fifo(p->data, rx_len, 0, 0))
			#else
			if(uart_Send(p->data, rx_len))
			#endif
			{
				my_fifo_pop(&hci_rx_fifo);
			}
			#else
            //uart_send(UART0, p->data, rx_len);
			app_hci_cmd_from_usb_handle(p->data, rx_len);
			my_fifo_pop(&hci_rx_fifo);
			#endif
		}
	}

    #if UART_SECOND_EN  
    p = (uart_data_t *)my_fifo_get(&hci_rx_fifo_2nd);
    
    if(p){
        u32 rx_len = p->len & 0xff; //usually <= 255 so 1 byte should be sufficient
        #if 1 // Serial loop test
        if(0 == my_fifo_push_hci_tx_fifo_2nd(p->data, rx_len, 0, 0))
        {
            my_fifo_pop(&hci_rx_fifo_2nd);
        }
        #endif
    }
    #endif
    
	return 0;
}

int blc_hci_tx_to_uart (void)
{
	my_fifo_buf_t *p = (my_fifo_buf_t *)my_fifo_get (&hci_tx_fifo);
	
#if  __TLSR_RISCV_EN__
	uart_tx_busy_timeout_poll();
	
	if (p && !uart_tx_is_busy_dma_tick(UART_NUM)){
        #if 1//(MESH_MONITOR_EN || (GATEWAY_ENABLE&&(HCI_ACCESS == HCI_USE_UART )))
        memcpy(uart_hw_tx_buf, p, min2(UART_TX_LEN_MAX, p->len + OFFSETOF(my_fifo_buf_t, data))); // dma need 4 bytes align
        if (uart_Send_dma_with_busy_hadle(UART_NUM, uart_hw_tx_buf, p->len + OFFSETOF(my_fifo_buf_t, data)))
        {
            my_fifo_pop (&hci_tx_fifo);
        }
        #else
        memcpy(uart_hw_tx_buf, p->data, min2(UART_TX_LEN_MAX, p->len)); // dma need 4 bytes align
        if (uart_Send_dma_with_busy_hadle(UART_NUM, uart_hw_tx_buf, p->len))
        {
            my_fifo_pop (&hci_tx_fifo);
        }
        #endif
    }

    #if UART_SECOND_EN
    _align_4_ static u8 uart_hw_tx_buf_2nd[sizeof(uart_hw_tx_buf)]; // dma need 4 bytes align
    p = (my_fifo_buf_t *)my_fifo_get (&hci_tx_fifo_2nd);
    if (p && !uart_tx_is_busy_dma_tick(UART_NUM_SECOND)){
        // include 2 bytes length in head
        memcpy(uart_hw_tx_buf_2nd, p, min2(UART_TX_LEN_MAX, p->len + OFFSETOF(my_fifo_buf_t, data)));
        if (uart_Send_dma_with_busy_hadle(UART_NUM_SECOND, uart_hw_tx_buf_2nd, p->len + OFFSETOF(my_fifo_buf_t, data)))
        {
            my_fifo_pop (&hci_tx_fifo_2nd);
        }
    }
    #endif
#else
    #if 1
    #if(MCU_CORE_TYPE == MCU_CORE_TC321X)
    if(p)
    #else
	if(p && !uart_tx_is_busy())
    #endif
	{
		#if 0//(1 == HCI_LOG_FW_EN) // use simulate UART with DEBUG_INFO_TX_PIN to print log.
	    if(p->data[0] == HCI_LOG){// printf part ,not send the hci log part 
			u8 uart_tx_buf[UART_TX_LEN_MAX + 8];
			u8 head_len = p->data[1];
			if(head_len > UART_TX_LEN_MAX){
			    return -1;
			}
			memcpy(uart_tx_buf, p->data,head_len);
			
			u8 data_len = p->len - head_len;
			u32 len_usable = UART_TX_LEN_MAX - (head_len - 2);
			u8 dump_len = printf_Bin2Text((char *)(uart_tx_buf+head_len), len_usable, (char *)(p->data+head_len), data_len);
			if (uart_Send(uart_tx_buf+2, (head_len-2)+dump_len))
			{
				my_fifo_pop (&hci_tx_fifo);
			}
		}else
		#endif
		{
			#if (MESH_MONITOR_EN || (GATEWAY_ENABLE&&(HCI_ACCESS == HCI_USE_UART )))
			if (uart_Send((u8 *)p, p->len + OFFSETOF(my_fifo_buf_t, data)))
			{
				my_fifo_pop (&hci_tx_fifo);
			}
			#else
			if (uart_Send(p->data, p->len))
			{
				my_fifo_pop (&hci_tx_fifo);
			}
			#endif
			
		}
	}
	#else	// only report user data
	if(p){
		u8 hci_type = p->data[0];
		if((hci_type >= HCI_RSP_USER_START) && (hci_type <= HCI_RSP_USER_END)){
			if (!uart_tx_is_busy ()){
				if (uart_Send(p->data+1, p->len-1)){
					my_fifo_pop (&hci_tx_fifo);
				}
			}
		}else{
			my_fifo_pop (&hci_tx_fifo);	// not report
		}
	}
	#endif
#endif
	
	return 0;
}
#endif


u16 swap_u16_data(u16 swap)
{
	u16 result = (swap &0xff)<<8;
	result += swap>>8;
	return result;
}

STATIC_ASSERT(sizeof(simple_flow_dev_uuid_t) <= 16);   // because sizeof dev uuid is 16
void set_dev_uuid_for_simple_flow( u8 *p_uuid)
{
    simple_flow_dev_uuid_t *p_dev_uuid = (simple_flow_dev_uuid_t *)p_uuid;
    memcpy(&p_dev_uuid->cps_head, &gp_page0->head, sizeof(p_dev_uuid->cps_head));
    memcpy(p_dev_uuid->mac, tbl_mac, sizeof(p_dev_uuid->mac));
    // set uuid
}
u8 prov_uuid_fastbind_mode(u8 *p_uuid)
{
    simple_flow_dev_uuid_t *p_dev_uuid = (simple_flow_dev_uuid_t *)p_uuid;
    if(p_dev_uuid->cps_head.cid == VENDOR_ID){
        return 1;
    }else{
        return 0;
    }
}


void wd_clear_lib(void)
{
	#if (MODULE_WATCHDOG_ENABLE && !defined(WIN32))
	wd_clear();
	#endif
}

//-------------------------------- uart end
// add the test interface for the part of the proxy client test part 
// MESH/CL/PROXY/BV-01C receive complete msg to lower tester

// MESH/CL/PROXY/BV-02C receive segmented msg to lower tester 
// wait for the part of the terminate cmd when the proxy msg is not complete
// MESH/CL/PROXY/BV-03C send complete msg to lower tester

int mesh_send_cl_proxy_bv03(u16 node_adr)
{
	u8 key_add_tmp=0;
	return SendOpParaDebug(node_adr, 0, APPKEY_ADD, (u8 *)&key_add_tmp, 0);
}

//  MESH/CL/PROXY/BV-04C send segment msg to lower tester
int mesh_send_cl_proxy_bv04(u16 node_adr)
{
	u8 app_key_idx[3]={0x00,0x01,0xff};
	u8 app_key[16];
	memset(app_key,2,sizeof(app_key));
	return cfg_cmd_sig_model_app_set(node_adr,app_key_idx,app_key);
}

// MESH/CL/PROXY/BV-05C send filter proxy cfg msg 
int mesh_send_cl_proxy_bv05(u16 node_adr)
{
	return mesh_proxy_set_filter_type(FILTER_BLACK_LIST);
}

// MESH/CL/PROXY/BV-06C add address to filter config msg 
int mesh_send_cl_proxy_bv06(u16 node_adr)
{
	return mesh_proxy_filter_add_adr(0x0011);
}

// MESH/CL/PROXY/BV-07C remove address to filter config msg 
int mesh_send_cl_proxy_bv07(u16 node_adr)
{
	return mesh_proxy_filter_remove_adr(0x0011);
}

// MESH/CL/PROXY/BV-08C receive the secure beacon when connected 
// MESH/CL/PROXY/BV-09-C [Send Secure Network Beacon When IV Index Updated]

// MESH/CL/PROX/BI-01-C [Ignore Invalid Message Type] ,ignore the invalid msg 

int mesh_dev_key_candi_decrypt_cb(u16 src_adr, int dirty_flag , const u8* ac_backup ,unsigned char *r_an, 
											       unsigned char* ac, int len_ut, int mic_length)
{
	int err =-1;
	// both the vc and the node will use the remote prov candi decrypt
#if (MD_REMOTE_PROV && DEVICE_KEY_REFRESH_ENABLE)
	//roll back the src data to the ac part 
	u8 *p_devkey = mesh_cfg_cmd_dev_key_candi_get(src_adr);
	if(is_buf_zero(p_devkey,16)){
		// the devkey candi is empty 
		return err;
	}				
	if(dirty_flag){
		LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"dirty flag and roll back ");
		memcpy(ac, ac_backup, len_ut); // restore access data
	}
	
	err = mesh_sec_msg_dec(p_devkey, r_an, ac, len_ut, mic_length);
	if(err){
		LOG_MSG_ERR(TL_LOG_MESH,0, 0 ,"device key candi decryption error ");
	}else{
		#if (DONGLE_PROVISION_EN || defined(WIN32))
		// update the vc node info part 
		LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"provisioner decrypt suc");
		VC_node_dev_key_candi_enable(src_adr);// if decrypt suc ,the provisioner will update the candidate.
		#else
		// if decrypt success ,we need to update the devkey by the devkey candi 
		LOG_MSG_INFO(TL_LOG_REMOTE_PROV,0,0,"node decrypt suc");
		memcpy(mesh_key.dev_key,node_devkey_candi,16);
		// clear the dev_key_candi 
		memset(node_devkey_candi,0,16);
		#endif
	}
	return err;
#else
	return err; // not support so it will decrypt err;
#endif
}

#ifndef WIN32
/**
 * @brief  calculate sha256 for firmware in flash. SDK need to be equal or greater than V3.3.4.
 */
#if 1
void mbedtls_sha256_flash( unsigned long addr, size_t ilen, unsigned char output[32], int is224 )
{
	int ret;
	mbedtls_sha256_context ctx;

	mbedtls_sha256_init( &ctx );

	if( ( ret = mbedtls_sha256_starts_ret( &ctx, is224 ) ) != 0 )
		goto exit;

#if 0
    if( ( ret = mbedtls_sha256_update_ret( &ctx, input, ilen ) ) != 0 )
        goto exit;
#else
	while(ilen){
		#if(MODULE_WATCHDOG_ENABLE)
		wd_clear();
		#endif
		
		u8 buf_in[64];// = {0};	// must 64, no need initial to save time.
		u16 ilen_2;
		if(ilen < 64){
			 ilen_2 = ilen;
			 ilen = 0;
		}else{
			ilen_2 = 64;
			ilen -= 64;
		}

		flash_read_page(addr, ilen_2, buf_in);
		addr += ilen_2;
		
		if( ( ret = mbedtls_sha256_update_ret( &ctx, buf_in, ilen_2 ) ) != 0 ){
			goto exit;
		}
	}
#endif

	if( ( ret = mbedtls_sha256_finish_ret( &ctx, output ) ) != 0 )
		goto exit;

exit:
	mbedtls_sha256_free( &ctx );
}


#else
/**
 * @brief  calculate sha256 for firmware in flash. for SDK equal or lower than V3.3.3.
 */
void mbedtls_sha256_flash( unsigned long addr, size_t ilen, unsigned char output[32], int is224 )
{
    mbedtls_sha256_context ctx;

    mbedtls_sha256_init( &ctx );
    mbedtls_sha256_starts( &ctx, is224 );
#if 0
	mbedtls_sha256_update( &ctx, input, ilen );
#else
	while(ilen){
		#if(MODULE_WATCHDOG_ENABLE)
		wd_clear();
		#endif
		
		u8 buf_in[64];// = {0}; // must 64, no need initial to save time.
		u16 ilen_2;
		if(ilen < 64){
			 ilen_2 = ilen;
			 ilen = 0;
		}else{
			ilen_2 = 64;
			ilen -= 64;
		}

		flash_read_page(addr, ilen_2, buf_in);
		addr += ilen_2;
		
		mbedtls_sha256_update( &ctx, buf_in, ilen_2 );
	}
#endif
    mbedtls_sha256_finish( &ctx, output );
    mbedtls_sha256_free( &ctx );
}
#endif

#endif

#if (__TLSR_RISCV_EN__ && FW_START_BY_BOOTLOADER_EN) // for security bootloader with security bootloader SDK.
#include "common/static_assert.h"
#include "stack/ble/service/ota/ota_stack.h"

#define ADDR_DELTA_TWO_FIRMWARE     (OTA_PROGRAM_BOOTADDR_USER - FW_START_BY_BOOTLOADER_ADDR)
// reserve PingPong mode for future.
STATIC_ASSERT((ADDR_DELTA_TWO_FIRMWARE == 0x20000)||(ADDR_DELTA_TWO_FIRMWARE == 0x40000)||(ADDR_DELTA_TWO_FIRMWARE == 0x80000));
STATIC_ASSERT(ADDR_DELTA_TWO_FIRMWARE >= FW_SIZE_MAX_USER_K * 1024);
//STATIC_ASSERT((FW_START_BY_BOOTLOADER_ADDR != 0x20000) && (FW_START_BY_BOOTLOADER_ADDR != 0x40000)); // can't set 0x20000 or 0x40000,because it still can run without bootloader.
STATIC_ASSERT((FW_OFFSET_MAX_K % 4 == 0) && (FW_OFFSET_MAX_K <= 508));   // ping pong size in eagle should be "<= 508".

#define reg_swire_ctrl1			REG_ADDR8(0x100c01)

_attribute_no_retention_data_ volatile u8 C_HASH_SHA256_EN = HASH_SHA256_EN; // just for EXE to check sha256 mode.
_attribute_no_retention_data_ volatile u8 C_LITTLE_ENDIAN_KEY_SIGNATURE_EN = LITTLE_ENDIAN_KEY_SIGNATURE_EN;	// just for EXE to check endian mode.

/*
  * return : 0(unlock), 1(lock)
*/
int is_bootloader_lock_state()
{
    return (0 == (reg_swire_ctrl1 & FLD_SWIRE_USB_EN));
}

void bootloader_ota_setNewFirmwareStorageAddress()
{
    ota_program_bootAddr = OTA_PROGRAM_BOOTADDR_USER;
    if(0 == ota_program_offset){
        ota_program_offset = FW_START_BY_BOOTLOADER_ADDR;
    }else{
        ota_program_offset = OTA_PROGRAM_BOOTADDR_USER;
    }
    
    ota_firmware_max_size = FW_SIZE_MAX_USER_K * 1024;
    blt_ota_reset();// to set "blotaSvr.last_adr_index = ota_firmware_max_size/16;" // should not extern blotaSvr, because they maybe differences between different SDKs.

    bls_ota_clearNewFwDataArea(0);

	extern u8 not_set_boot_mark_flag;
	not_set_boot_mark_flag = 1;
}
#endif

/**
 * @brief      callback function of LinkLayer Event "BLT_EV_FLAG_SUSPEND_ENTER"
 * @param[in]  e - LinkLayer Event type
 * @param[in]  p - data pointer of event
 * @param[in]  n - data length of event
 * @return     none
 */
void mesh_set_sleep_wakeup(u8 e, u8 *p, int n)
{
#if UI_KEYBOARD_ENABLE
	if(key_released){
		bls_pm_setWakeupSource(PM_WAKEUP_PAD);
	}
#endif

#if GATT_LPN_EN
    mesh_notifyfifo_rxfifo(); // Quick response in next interval, especially for long connect interval.
#endif
}

#if UI_KEYBOARD_ENABLE
/**
 * @brief       keyboard initialization
 * @return      none
 * @note        
 */
void ui_keyboard_wakeup_io_init(void)
{
	/////////// keyboard gpio wakeup init ////////
	#if KB_LINE_MODE
	u32 pin[] = KB_SCAN_PINS;
	foreach_arr(i, pin)
	{
		cpu_set_gpio_wakeup (pin[i], 0, 1);  //scan pin pad low wakeup deepsleep
	}
	#else
	u32 pin[] = KB_DRIVE_PINS;
	foreach_arr(i, pin)
	{
		cpu_set_gpio_wakeup (pin[i], 1, 1);  //drive pin pad high wakeup deepsleep
	}
	#endif
}
#endif
/**
  * @}
  */

/**
  * @}
  */


