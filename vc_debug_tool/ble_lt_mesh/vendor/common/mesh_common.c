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
#if !WIN32
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

#if !WIN32
#include "third_party/micro-ecc/uECC.h"
#include "vendor/common/mi_api/telink_sdk_mible_api.h"
#endif
#if DU_ENABLE
#include "third_party/micro-ecc/uECC.h"
#include "user_du.h"
#endif
#if MI_API_ENABLE
#include "vendor/common/mi_api/telink_sdk_mible_api.h"
#include "vendor/common/mi_api/telink_sdk_mesh_api.h"
#endif

#if GATT_RP_EN
#include "remote_prov_gatt.h"
#endif

#if(__TL_LIB_8258__ || (MCU_CORE_TYPE == MCU_CORE_8258))
#include "stack/ble/ble.h"
#elif(MCU_CORE_TYPE == MCU_CORE_8278)
#include "stack/ble_8278/ble.h"
#endif
#if WIN32 
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

int set_adv_solicitation(rf_packet_adv_t * p) ;


#ifndef WIN32
#if (0 == __TLSR_RISCV_EN__)
#if BLC_PM_DEEP_RETENTION_MODE_EN
	#if PM_DEEPSLEEP_RETENTION_ENABLE
_attribute_no_retention_bss_ 
	#endif
_align_4_ u8 irq_stk[__IRQ_STACK_SIZE__] = {0};
#endif
#endif

#if PM_DEEPSLEEP_RETENTION_ENABLE
asm(".equ __PM_DEEPSLEEP_RETENTION_ENABLE,    1");
#else
asm(".equ __PM_DEEPSLEEP_RETENTION_ENABLE,    0");
#endif
asm(".global     __PM_DEEPSLEEP_RETENTION_ENABLE");

#if FLASH_PLUS_ENABLE // && (0 == FW_START_BY_LEGACY_BOOTLOADER_EN) // dual mode with bootloader can set to 256K.
asm(".equ __FLASH_512K_ENABLE,    0");
#else
asm(".equ __FLASH_512K_ENABLE,    1");
#endif
asm(".global     __FLASH_512K_ENABLE");

#if __PROJECT_BOOTLOADER__
asm(".equ __FW_OFFSET,      0"); // must be 0
#elif (FW_START_BY_LEGACY_BOOTLOADER_EN)
    #if (DUAL_MODE_FW_ADDR_SIGMESH == 0x80000)
asm(".equ __FW_OFFSET,      0x80000");  // must be equal to DUAL_MODE_FW_ADDR_SIGMESH
    #endif
#elif (FW_START_BY_LEGACY_BOOTLOADER_EN)
    #if (FW_START_BY_BOOTLOADER_ADDR == 0x10000)
asm(".equ __FW_OFFSET,      0x10000");  // must be equal to FW_START_BY_BOOTLOADER_ADDR
    #endif
#else
asm(".equ __FW_OFFSET,      0");
#endif
asm(".global     __FW_OFFSET");

#if (FW_START_BY_LEGACY_BOOTLOADER_EN)
asm(".equ __FW_START_BY_LEGACY_BOOTLOADER_EN,  1");
#else
asm(".equ __FW_START_BY_LEGACY_BOOTLOADER_EN,  0");
#endif
asm(".global     __FW_START_BY_LEGACY_BOOTLOADER_EN");

#if __PROJECT_BOOTLOADER__
asm(".equ __BOOT_LOADER_EN,         1");
#else
asm(".equ __BOOT_LOADER_EN,         0");
#endif
asm(".global     __BOOT_LOADER_EN");

#if (FW_RAMCODE_SIZE_MAX == 0x4000)
asm(".equ __FW_RAMCODE_SIZE_MAX,    0x4000");   // must be equal to FW_RAMCODE_SIZE_MAX
#endif
asm(".global     __FW_RAMCODE_SIZE_MAX");

#if (MCU_RUN_SRAM_WITH_CACHE_EN || MCU_RUN_SRAM_EN)
asm(".equ __MCU_RUN_SRAM_EN,         1");
#else
asm(".equ __MCU_RUN_SRAM_EN,         0");
#endif
asm(".global     __MCU_RUN_SRAM_EN");

#if (__TLSR_RISCV_EN__)
#if SPEECH_ENABLE
asm(".equ __SPEECH_ENABLE,         1");
#else
asm(".equ __SPEECH_ENABLE,         0");
#endif
asm(".global     __SPEECH_ENABLE");
#endif
#endif

STATIC_ASSERT(TRANSMIT_CNT_DEF < 8);
STATIC_ASSERT(TRANSMIT_CNT_DEF_RELAY < 8);
STATIC_ASSERT(sizeof(mesh_scan_rsp_t) <= 31);
#if (CHIP_TYPE == CHIP_TYPE_8269)
STATIC_ASSERT((0 == FLASH_PLUS_ENABLE) && (0 == PINGPONG_OTA_DISABLE));
#endif
#if (PINGPONG_OTA_DISABLE)
STATIC_ASSERT(1 == FLASH_PLUS_ENABLE);
#else
STATIC_ASSERT(FLASH_ADR_AREA_FIRMWARE_END <= ((MCU_CORE_TYPE == MCU_CORE_9518)? 0x7F000 : 0x3F000));
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
#endif
#endif

#if (!WIN32)    // also used for shell file.
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
#if WIN32
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
void beacon_str_init()
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
void beacon_str_disable()
{
	beacon_send.en = 0;
}

static u8 mesh_seg_must_enable =0;

/**
 * @brief       This function force to use segment flow to send message even though access layer length is less than 11bytes.
 * @param[in]   en	- 1: force to use segment flow to send message; 0: not force.
 * @return      none
 * @note        
 */
void mesh_seg_must_en(u8 en)
{
	mesh_seg_must_enable = en;
}
	

/**
 * @brief       This function force to use segment flow to send message even though access layer length is less than 11bytes.
 * @param[in]   p				- all input setting material used to send message.
 * @param[in]   p_match_type	- all parameters used to send message, include info of destination match type.
 * @return      none
 * @note        
 */
void mesh_cfg_cmd_force_seg_set(material_tx_cmd_t *p,mesh_match_type_t *p_match_type)
{
#if MI_API_ENABLE
	if(p_match_type->sig && (SIG_MD_CFG_SERVER == p_match_type->id)&&is_unicast_adr(p->adr_dst)){
		#if MI_SWITCH_LPN_EN
		p->seg_must = 0;// in the mi_mode ,when the lpn mode enable ,the segment should change to 0
		#else
		p->seg_must = 1;// in the mi_mode should set the segment to 1
		#endif	
	}
#else
	if(is_unicast_adr(p->adr_dst)){
		p->seg_must = mesh_seg_must_enable;
	}else{
		p->seg_must = 0;
	}
#endif
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

#if !WIN32

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
	if(len > RAND_NUM_MAX_LEN){
		return MI_ERR_NO_MEM;
	}
	for(int i=0;i<len;i++){
		p_buf[i]=rand()&0xff;
	}
	return MI_SUCCESS;
}


/**
 * @brief       This function is random function for ecc.
 * @param[out]  dest	- store random numbers
 * @param[in]   size	- how many random numbers are generated
 * @return      1
 * @note        
 */
static int ecc_rng(uint8_t *dest, unsigned size)
{
    telink_rand_num_generator(dest, (uint32_t) size);
    return 1;
}


/**
 * @brief       This function swap endianness.
 * @param[in]   in		- data to be swapped.
 * @param[out]  out		- swap result
 * @param[in]   size	- The size of bytes to be swapped
 * @return      -1:swap error; 0:swap success
 * @note        
 */
static int telink_swap_endian(const uint8_t *in, uint8_t *out, uint32_t size)
{
    if (out < in + size && in < out + size)
        return -1;

    for(int i = 0; i < size; i++)
        out[i] = in[size-1-i];

    return 0;
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
	const struct uECC_Curve_t * p_curve;
	uECC_set_rng(ecc_rng);
    p_curve = uECC_secp256r1();
	mesh_ecdh_key_str key_tmp;
    uECC_make_key(key_tmp.dpk, key_tmp.dsk, p_curve);
	telink_swap_endian(key_tmp.dsk,dsk, 32);
    telink_swap_endian(key_tmp.dpk, dpk, 32);
    telink_swap_endian(key_tmp.dpk+32,dpk+32,32);
}




/**
 * @brief       This function process public key creation.
 * @return      none
 * @note        
 */
void pubkey_create_proc()
{
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
void mesh_secure_beacon_loop_proc()
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
int is_need_send_sec_nw_beacon()
{
	return 	((NW_BEACON_BROADCASTING == model_sig_cfg_s.sec_nw_beacon));
}


/**
 * @brief       This function poll to tx network security beacon and private beacon if the time arrives.
 * @return      none
 * @note        
 */
void mesh_beacon_poll_1s()
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

#if !WIN32
// adv_filter has been deleted, if want to change filter rules, please set in user_adv_filter_proc().
u8 adv_mesh_en_flag = 0;
u8 mesh_kr_filter_flag =0;
u8 mesh_provisioner_buf_enable =0;

/**
 * @brief       This function enable mesh adv filter. 
 * @return      none
 * @note        
 */
void enable_mesh_adv_filter()
{
	#if (FEATURE_FRIEND_EN && GATEWAY_ENABLE)
	if(is_in_mesh_friend_st_fn_all()){
		// in the friend state can not enable the adv mesh en flag .
		return ;
	}
	#endif
	#if TESTCASE_FLAG_ENABLE
	adv_mesh_en_flag = 0;
	#else
	adv_mesh_en_flag = 1;
	#endif
}


/**
 * @brief       This function disable mesh adv filter.
 * @return      none
 * @note        
 */
void disable_mesh_adv_filter()
{
	#if TESTCASE_FLAG_ENABLE
	adv_mesh_en_flag = 0;
	#else
	adv_mesh_en_flag = 0;
	#endif	
}


/**
 * @brief       This function enable key refresh filter
 * @return      none
 * @note        
 */
void enable_mesh_kr_cfg_filter()
{
	mesh_kr_filter_flag = 1;
}


/**
 * @brief       This function disable key refresh filter
 * @return      none
 * @note        
 */
void disable_mesh_kr_cfg_filter()
{
	mesh_kr_filter_flag = 0;
}


/**
 * @brief       This function enable provision buffer
 * @return      none
 * @note        
 */
void  enable_mesh_provision_buf()
{
	mesh_provisioner_buf_enable =1;
}

/**
 * @brief       This function disable provision buffer
 * @return      none
 * @note        
 */
void  disable_mesh_provision_buf()
{
	mesh_provisioner_buf_enable =0;
}

extern attribute_t* gAttributes;

#if !BLE_MULTIPLE_CONNECTION_ENABLE
/**
 * @brief       This function get attribute permission.
 * @param[in]   handle	- ATT handle of gAttributes in "my Attributes"
 * @return      permission
 * @note        
 */
u8 get_att_permission(u8 handle)
{
#if HOMEKIT_EN
	return gAttributes[handle].perm;
#else
	return *gAttributes[handle].p_perm;
#endif
}
#endif

#if HOMEKIT_EN

/**
 * @brief       This function advertise filter for homekit.
 * @param[in]   raw_pkt	- raw data of advertise packet
 * @return      none
 * @note        
 */
_attribute_ram_code_ void adv_homekit_filter(u8 *raw_pkt)
{
	extern u8 blt_pair_ok;
	extern u8 proc_homekit_pair;
	if((!blt_pair_ok) && (get_ble_state() == BLE_STATE_BRX_S)){// homekit pair proc in irq
		rf_packet_att_write_t *p = (rf_packet_att_write_t*)raw_pkt;
		if(p->rf_len){
			u8 type = p->type&3;
			if(type && (type != LLID_CONTROL)){// l2cap pkt
				if(p->chanId == L2CAP_CID_ATTR_PROTOCOL){
					if( !(p->opcode & 0x01) || p->opcode == ATT_OP_EXCHANGE_MTU_RSP)		//att server handler
					{
						if((ATT_OP_WRITE_CMD == p->opcode) || ( ATT_OP_WRITE_REQ == p->opcode)	|| ( ATT_OP_EXECUTE_WRITE_REQ == p->opcode) \
						 || ( ATT_OP_READ_REQ == p->opcode) || (ATT_OP_READ_BLOB_REQ == p->opcode)){
							if(is_homekit_pair_handle(p->handle)){
								proc_homekit_pair = 1;
								static u32 A_homekit_pair=0;A_homekit_pair++;
							}
						}
						else{

						}
					}
				}
			}
		}
	}
}
#endif

#ifndef USER_ADV_FILTER_EN
#define USER_ADV_FILTER_EN		0
#endif

#if (USER_ADV_FILTER_EN)
/**
 * @brief:return 1 means keep this packet, return 0 to discard.
 * @Note: user should not keep all adv packets, because they are too much. only keep those necessary packets by comparing playload, the less the better.
 *        so that the blt_rxfifo_ can be more efficient. 
 *        if "fifo_free_cnt" is less than 4, the packet should not be kept, or it may cause rx fifo overflowed.
 */
_attribute_ram_code_ u8 user_adv_filter_proc(u8 * p_rf_pkt)
{	
	#if 0 // demo
	u8 *p_payload = ((rf_packet_adv_t *)p_rf_pkt)->data;
	#if EXTENDED_ADV_ENABLE
    rf_pkt_aux_adv_ind_1 *p_ext = (rf_pkt_aux_adv_ind_1 *)p_rf_pkt;
	if(LL_TYPE_AUX_ADV_IND == p_ext->type){
	    p_payload = p_ext->dat;
	}
	#endif

	PB_GATT_ADV_DAT *p_pb_adv = (PB_GATT_ADV_DAT *)p_payload;
	u8 temp_uuid[2]=SIG_MESH_PROVISION_SERVICE;
	if(!memcmp(temp_uuid, p_pb_adv->uuid_pb_uuid, sizeof(temp_uuid))){
		static u32 A_pb_adv_cnt = 0;
		A_pb_adv_cnt++;
		return 1;
	}
	#endif
	
	return 0;
}
#endif

#if DU_ULTRA_PROV_EN
/**
 * @brief       This function check whether it is an ultra provision adv payload which is user define.
 * @param[in]   p_payload	- rx advertise valid data
 * @return      1: yes; 0: no
 * @note        
 */
static inline int is_ultra_prov_adv(u8 *p_payload)
{
	genie_manu_factor_data_t *p_manu_data = (genie_manu_factor_data_t *)p_payload;
    if(p_manu_data->flag_type == GAP_ADTYPE_FLAGS){
    	if((p_manu_data->manu_type == GAP_ADTYPE_MANUFACTURER_SPECIFIC) 
		#if DU_ENABLE
		&& (p_manu_data->cid == VENDOR_ID)
		#elif AIS_ENABLE
		&& (p_manu_data->cid == HTON16(VENDOR_ID))
		#endif
		) {
	        return 1;
	    }
	    
		ios_app_data_t *p_ios_data = (ios_app_data_t *)p_payload;
		if((p_ios_data->uuid_len == 2) && (p_ios_data->uuid_type == GAP_ADTYPE_TX_POWER_LEVEL)){// ios with tx power
			memcpy(&p_ios_data->uuid_len, &p_ios_data->uuid_len+3, 18);
		}
		
		#if DU_ENABLE
		if((p_ios_data->uuid_type == GAP_ADTYPE_128BIT_COMPLETE) && (p_ios_data->uuid[14] == U16_HI(VENDOR_ID)) && (p_ios_data->uuid[15] == U16_LO(VENDOR_ID)))
		#elif AIS_ENABLE
		if((p_ios_data->uuid_type == GAP_ADTYPE_128BIT_COMPLETE) && (p_ios_data->uuid[14] == U16_LO(VENDOR_ID)) && (p_ios_data->uuid[15] == U16_HI(VENDOR_ID)))
		#endif
		{
			endianness_swap_u128(p_ios_data->uuid); // uuid:little endian
			return 1;
		}
    }
    return 0;
}

_attribute_ram_code_ u8 du_ultra_prov_adv_filter_proc(u8 * p_rf_pkt)
{
	u8 *p_payload = ((rf_packet_adv_t *)p_rf_pkt)->data;
	#if EXTENDED_ADV_ENABLE
	rf_pkt_aux_adv_ind_1 *p_ext = (rf_pkt_aux_adv_ind_1 *)p_rf_pkt;
	if(LL_TYPE_AUX_ADV_IND == p_ext->type){
		p_payload = p_ext->dat;
	}
	#endif

	return is_ultra_prov_adv(p_payload);
}
#endif

#if REMOTE_PROV_SCAN_GATT_EN
	#if ACTIVE_SCAN_ENABLE
static u8 scan_req_mac[6];


/**
 * @brief       This function : remote provision scan request process.
 * @return      none
 * @note        
 */
_attribute_ram_code_ void rp_active_scan_req_proc()
{
	if(reg_rf_irq_status & FLD_RF_IRQ_RX){
		u8 * raw_pkt = (u8 *) (blt_rxfifo_b + (blt_rxfifo.wptr++ & (blt_rxfifo.num-1)) * blt_rxfifo.size);
#if ((MCU_CORE_TYPE == MCU_CORE_8258) || (MCU_CORE_TYPE == MCU_CORE_8278))
		u8 *p_rf_pkt =	(raw_pkt + 0);
#elif (MCU_CORE_TYPE == MCU_CORE_8269)
		u8 *p_rf_pkt =	(raw_pkt + 8);
#endif
		rf_packet_adv_t * p_adv = (rf_packet_adv_t *)(p_rf_pkt);
		PB_GATT_ADV_DAT *p_pb_adv = (PB_GATT_ADV_DAT *)(p_adv->data);
		if(p_adv->header.type ==LL_TYPE_ADV_IND ){
			rf_ble_tx_on ();
			u32 t = reg_rf_timestamp + ((((raw_pkt[DMA_RFRX_OFFSET_RFLEN]+5)<<3) + 28) << 4);
			tx_settle_adjust(LL_SCAN_TX_SETTLE);
			pkt_scan_req.advA[0]=p_adv->advA[0];
			pkt_scan_req.advA[1]=p_adv->advA[1];
			pkt_scan_req.advA[2]=p_adv->advA[2];
			pkt_scan_req.advA[3]=p_adv->advA[3];
			pkt_scan_req.advA[4]=p_adv->advA[4];
			pkt_scan_req.advA[5]=p_adv->advA[5];
			rf_start_stx ((void *)&pkt_scan_req, t);
			// send the scan req ,for the time is not enough ,so can not get the scan req
			u8 temp_uuid[2]=SIG_MESH_PROVISION_SERVICE;
			if(!memcmp(temp_uuid, p_pb_adv->uuid_pb_uuid, sizeof(temp_uuid))&&
				!memcmp(rp_mag.rp_extend[0].uuid, p_pb_adv->service_data, sizeof(p_pb_adv->service_data))){
				memcpy(scan_req_mac,p_adv->advA,sizeof(p_adv->advA));
				// remember the mac adr which we will send the scan_req
				
			}else{
				STOP_RF_STATE_MACHINE;
			}
			rf_set_rxmode();
		}
		blt_rxfifo.wptr--;
	}

}
	#endif


/**
 * @brief       This function is: remote provision connect adv filter process.
 * @param[in]   p_rf_pkt	- Received packet data
 * @return      0: discard this packet; 1: keep.
 * @note        
 */
_attribute_ram_code_ u8 rp_conn_adv_filter_proc(u8 * p_rf_pkt)
{
	rf_packet_adv_t * p_adv = (rf_packet_adv_t *)(p_rf_pkt);
	u8 *p_payload = p_adv->data;
	PB_GATT_ADV_DAT *p_pb_adv = (PB_GATT_ADV_DAT *)p_payload;
	u8 adv_type = p_adv->header.type;
	if(adv_type ==LL_TYPE_ADV_IND){
		u8 temp_uuid[2]=SIG_MESH_PROVISION_SERVICE;
		if(!memcmp(temp_uuid, p_pb_adv->uuid_pb_uuid, sizeof(temp_uuid))&&
			!memcmp(rp_mag.rp_extend[0].uuid, p_pb_adv->service_data, sizeof(p_pb_adv->service_data))){
			return 1;
		}else if (!memcmp(temp_uuid, p_pb_adv->uuid_pb_uuid, sizeof(temp_uuid))&&
			rp_mag.rp_scan_sts.scannedItemsLimit == MAX_SCAN_ITEMS_UUID_CNT){//unlimited scan
			return 1;
		}else{
			return 0;
		}
	}else if (adv_type ==LL_TYPE_SCAN_RSP){
		#if ACTIVE_SCAN_ENABLE
		// only the mac adr is the same with the scan_req
		if(!memcmp(scan_req_mac,p_adv->advA,sizeof(p_adv->advA))){
			return 1;
		}else{
			return 0;
		}
		#else
		return 0;
		#endif
	}
	return 0;
}
#endif

#if((MCU_CORE_TYPE == MCU_CORE_8258) || (MCU_CORE_TYPE == MCU_CORE_8278))
#define reg_rf_chn_current  		REG_ADDR8(0x40d)
#endif

#if RELAY_ROUTE_FILTE_TEST_EN
#if (__TLSR_RISCV_EN__)
#define FLASH_ADR_RELAY_ROUTE_MAC		0xFD000
#else
#define FLASH_ADR_RELAY_ROUTE_MAC		0x7D000
#endif
#define RELAY_ROUTE_MAC_MAX				2

u32 mac_filter_by_button = 0;

u8 relay_route_en = 0;
u32 relay_route_mac[RELAY_ROUTE_MAC_MAX];
_attribute_ram_code_ static int is_in_relay_route_white_list(u8 *pkt_mac)
{
	if(mac_filter_by_button){
		return (0 == memcmp(&mac_filter_by_button, pkt_mac, sizeof(mac_filter_by_button)));
	}else{
		if(0 == relay_route_en){
			return 1;
		}
		
		foreach_arr(i, relay_route_mac){
			if(0 == memcmp(&relay_route_mac[i], pkt_mac, sizeof(relay_route_mac[0]))){
				return 1;
			}
		}
	}
	return 0;
}
#endif

/**
 * @brief       This function keep some packet when adv_type_accept_flag is 0, such as the packet is typt of LL_TYPE_ADV_IND. 
 * @param[in]   p_rf_pkt- pointer to rf packet
 * @return      0: discard this packet; 1: keep.
 * @note        It can not return 1 always, because the ble rx fifo will be push too much ADV of LL_TYPE_ADV_IND. then the rx fifo
 *              will not be enough to receive mesh message.
 */
_attribute_ram_code_ int adv_2nd_filter_for_other_packet(u8 * p_rf_pkt)
{
#if (REMOTE_PROV_SCAN_GATT_EN)
	if(rp_conn_adv_filter_proc(p_rf_pkt)){
		return 1;
	}
#endif
#if (MD_ON_DEMAND_PROXY_EN && VENDOR_IOS_SOLI_PDU_EN)
	if(is_ios_soli_pdu(((rf_packet_adv_t *)p_rf_pkt)->data)){
		return 1;
	}	
#endif
#if DU_ULTRA_PROV_EN
	if(du_ultra_prov_adv_filter_proc(p_rf_pkt){
		return 1;
	}	
#endif

#if (USER_ADV_FILTER_EN)
	if(user_adv_filter_proc(p_rf_pkt)){ // must at last of this function.
		return 1;
	}
#endif

	return 0;	// next_buffer = 0;
}

/**
 * @brief       This function Filters the received broadcast packets
 * @param[in]   blt_sts	- ble link layer state
 * @param[in]   raw_pkt	- raw_data packet
 * @return      0: discards the adv packet; 1: save the adv packet
 * @note        LL_TYPE_ADV_EXT_IND is pre filtered by type in mesh_blc_aux_adv_filter()
 */
_attribute_ram_code_ u8 adv_filter_proc(u8 *raw_pkt ,u8 blt_sts)
{
#if SCAN_ADV_BUF_INDEPENDENT_EN
	#define ADV_ST_FREE_FIFO_MIN_CNT	(2) // to make sure not overflow
	#define BLE_RCV_FIFO_MAX_CNT		(0)	// 
#else
	#define ADV_ST_FREE_FIFO_MIN_CNT	(4) // to make sure not overflow
	#if FEATURE_LOWPOWER_EN
	// -2: means no need to reserve too much fifo for BLE, especially for LPN with only 8 fifo in some cases.
	#define BLE_RCV_FIFO_MAX_CNT	(8-2)	// refer to default buffer count of BLE generic SDK which is 8.
	#else
    #define BLE_RCV_FIFO_MAX_CNT 	(6)		// set to 6 to keep more fifo to receive ADV // refer to default buffer count of BLE generic SDK which is 8.
	#endif
#endif
	u8 next_buffer =1;
	u8 adv_type = 0;
	u8 mesh_msg_type = 0;
	__UNUSED u8 *p_mac = 0;
	#if ((MCU_CORE_TYPE == MCU_CORE_8258) || (MCU_CORE_TYPE == MCU_CORE_8278)||(MCU_CORE_TYPE == MCU_CORE_9518))
	u8 *p_rf_pkt =  (raw_pkt + 0);
	#elif (MCU_CORE_TYPE == MCU_CORE_8269)
	u8 *p_rf_pkt =  (raw_pkt + 8);
	#endif
	
	rf_packet_adv_t * pAdv = (rf_packet_adv_t *)p_rf_pkt;
	adv_type = pAdv->header.type;
	mesh_msg_type = pAdv->data[1];
	p_mac = pAdv->advA;

	#if RELAY_ROUTE_FILTE_TEST_EN
	if(0 == is_in_relay_route_white_list(p_mac)){
		//#if (0 == GATEWAY_ENABLE)
		return 0;
		//#endif
	}
	#endif
	
	#if DUAL_MESH_SIG_PVT_EN
	if((37 == raw_pkt[5]) && dual_mode_tlk_ac[1] == read_reg32 (0x408)){ // private mesh pkt
		if(pair_dec_packet_mesh(raw_pkt)){
			tlk_mesh_rf_att_cmd_t * pp = (tlk_mesh_rf_att_cmd_t*)raw_pkt;
			tlk_mesh_rf_att_value_t *p_payload = (tlk_mesh_rf_att_value_t *)pp->value;
			if((LGT_CMD_MESH_PAIR == p_payload->val[0]) && (MESH_PAIR_DEFAULT_MESH == p_payload->val[3])){
				dual_mode_mesh_found = DUAL_MODE_NETWORK_PVT_MESH;	
			}			
		}	
		return 0;
	}
	#endif
	int adv_type_accept_flag = ((LL_TYPE_ADV_NONCONN_IND == adv_type)
                        #if EXTENDED_ADV_ENABLE
                        || (LL_TYPE_AUX_ADV_IND == adv_type)
                        #endif
						#if MD_ON_DEMAND_PROXY_EN
						|| ((LL_TYPE_ADV_SCAN_IND == adv_type) && mesh_on_demand_is_valid_st_to_rx_solicitation()) // for Android service data.
						#endif
	                    );
	
	#if MESH_MONITOR_EN
	if((blt_sts != BLS_LINK_STATE_CONN) || (ble_state == BLE_STATE_BRX_E)){
		if(adv_type_accept_flag){ 	// add report adv channel
			pAdv->advA[pAdv->rf_len + 3] = reg_rf_chn_current; // advA[pAdv->rf_len + (0 ~ 2)] will be set to "rssi(1 byte) + RF dc(2 byte)" later. advA[pAdv->rf_len + 3] is CRC ok flag before set to channel here.
		}
	}else{
		// TODO: GATT data: need to be handled for SMP mode and no SMP mode
	}
	#endif

	#if MESH_RX_TEST
	mesh_cmd_bear_t *p_br = GET_BEAR_FROM_ADV_PAYLOAD(pAdv->data);
	if((mesh_msg_type == MESH_ADV_TYPE_MESSAGE) && (p_br->nw.nid == mesh_key.net_key[0][0].nid_m)){
	u32 timeStamp = reg_rf_timestamp;
	u8 channel_rx = reg_rf_chn_current;
	if(37 == channel_rx || 38 == channel_rx || 39 == channel_rx){
		timeStamp -= ((channel_rx % 36) *  500 * sys_tick_per_us); // compensation for rf packet sending time.
	}else{
		// TODO: LL_TYPE_AUX_ADV_IND
	}
	
	if((DMA_RFRX_OFFSET_TIME_STAMP(raw_pkt) + sizeof(timeStamp)) < blt_rxfifo.size)
		memcpy(raw_pkt + DMA_RFRX_OFFSET_TIME_STAMP(raw_pkt), &timeStamp, sizeof(timeStamp));
	}
	#endif
	
	// "fifo_free_cnt" here means if accepte this packet, there still is the number of "fifo_free_cnt" remained, because wptr has been ++.
	#if BLE_MULTIPLE_CONNECTION_ENABLE
	u8 fifo_free_cnt = blc_ll_getFreeScanFifo();
	#else
	u8 fifo_free_cnt = blt_rxfifo.num - ((u8)(blt_rxfifo.wptr - blt_rxfifo.rptr)&(blt_rxfifo.num-1));
	if(blt_sts == BLS_LINK_STATE_CONN){
		if(get_ble_state() != BLE_STATE_BRX_S){
			if(fifo_free_cnt < max2(BLE_RCV_FIFO_MAX_CNT, 2)){
				next_buffer = 0;
			}else if(0 == adv_type_accept_flag){
				next_buffer = adv_2nd_filter_for_other_packet(p_rf_pkt);
			}
			
			#if DEBUG_CFG_CMD_GROUP_AK_EN
			update_nw_notify_num(p_rf_pkt, next_buffer);
			#endif			
		}else{			
			#if(HOMEKIT_EN)
			adv_homekit_filter(raw_pkt);			
			#endif
			if(fifo_free_cnt < 1){
                write_reg8(0x800f00, 0x80);         // stop ,just stop BLE state machine is enough 
			    //next_buffer = 0;	// should not discard
			    //LOG_MSG_LIB(TL_LOG_MESH,0,0,"FIFO FULL");
			}
		}
	}else
	#endif
	{
		if(0 == adv_type_accept_flag){
			next_buffer = adv_2nd_filter_for_other_packet(p_rf_pkt);
			
			if (fifo_free_cnt < ADV_ST_FREE_FIFO_MIN_CNT){
				// can not make the fifo overflow 
				next_buffer = 0;
			}
		}else{
			// add the filter part last 
			if(adv_mesh_en_flag){
				if( (mesh_msg_type != MESH_ADV_TYPE_PRO && mesh_msg_type != MESH_ADV_TYPE_MESSAGE)|| 
					(fifo_free_cnt < 2)){	
					// not need to reserve fifo for the ble part 
					next_buffer = 0;
				}
			#if __PROJECT_MESH_PRO__
			}else if(mesh_kr_filter_flag){
				// keybind filter flag ,to improve the environment of the gateway part
				if( mesh_msg_type != MESH_ADV_TYPE_MESSAGE || (fifo_free_cnt < 4)){
					next_buffer = 0;
				}
			}else if (mesh_provisioner_buf_enable){
				if(fifo_free_cnt < 2){
					// special dispatch for the provision only 
					next_buffer = 0;
				}
			#endif
			}else if (fifo_free_cnt < ADV_ST_FREE_FIFO_MIN_CNT){
					// can not make the fifo overflow 
				next_buffer = 0;
			}else{
			}
			
			#if DEBUG_CFG_CMD_GROUP_AK_EN
			update_nw_notify_num(p_rf_pkt, next_buffer);
			#endif
		}
	}
	return next_buffer;
}
#endif

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

#if (!WIN32 && MD_BIND_WHITE_LIST_EN)
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

// SUBSCRIPTION SHARE
STATIC_ASSERT(SUBSCRIPTION_BOUND_STATE_SHARE_EN == 1);

#if SUBSCRIPTION_BOUND_STATE_SHARE_EN
	#ifndef SHARE_ALL_LIGHT_STATE_MODEL_EN      // user can define in user_app_config.h
#define SHARE_ALL_LIGHT_STATE_MODEL_EN      (AIS_ENABLE)
	#endif

#define LOG_SHARE_MODEL_DEBUG(pbuf, len, format, ...)		//LOG_MSG_LIB(TL_LOG_NODE_BASIC, pbuf, len, format, ##__VA_ARGS__)


/**
 * @brief       all sig models that have extend relation ship with onoff server model 
 *              they should share the same subscription list.
 *              please refer to "4.2.4 Subscription List" of MshMDL_v1.1.pdf.
 */
const u16 sub_share_model_sig_onoff_server_extend[] = { // model list for onoff extend. // should share when it is extend model.
    #if MD_SERVER_EN
        #if MD_ONOFF_EN
    SIG_MD_G_ONOFF_S, 
        #endif
        #if MD_LEVEL_EN
    SIG_MD_G_LEVEL_S, 
        #endif
        #if MD_LIGHTNESS_EN
    SIG_MD_LIGHTNESS_S, 
            #if 1 // SHARE_ALL_LIGHT_STATE_MODEL_EN 		// should share when it is extend model.
    SIG_MD_LIGHTNESS_SETUP_S, 								// SIG_MD_LIGHTNESS_SETUP_S extend lightness server.
            #endif
        #endif
        #if LIGHT_TYPE_CT_EN
    SIG_MD_LIGHT_CTL_S, SIG_MD_LIGHT_CTL_TEMP_S,
            #if 1 // SHARE_ALL_LIGHT_STATE_MODEL_EN 		// should share when it is extend model.
    SIG_MD_LIGHT_CTL_SETUP_S, 
            #endif
        #endif
        #if LIGHT_TYPE_HSL_EN
    SIG_MD_LIGHT_HSL_S, SIG_MD_LIGHT_HSL_HUE_S, SIG_MD_LIGHT_HSL_SAT_S,
            #if 1 // SHARE_ALL_LIGHT_STATE_MODEL_EN 		// should share when it is extend model.
    SIG_MD_LIGHT_HSL_SETUP_S, 
            #endif
        #endif
        #if MD_LIGHT_CONTROL_EN
    SIG_MD_LIGHT_LC_S, SIG_MD_LIGHT_LC_SETUP_S,
        #endif
    	#if MD_DEF_TRANSIT_TIME_EN
    SIG_MD_G_DEF_TRANSIT_TIME_S, 							// lightness setup extend power onoff setup extend default transition time server.
        #endif
    	#if MD_POWER_ONOFF_EN
    SIG_MD_G_POWER_ONOFF_S, SIG_MD_G_POWER_ONOFF_SETUP_S,	// SIG_MD_G_POWER_ONOFF_SETUP_S extend default transition time server.
        #endif
    	#if (LIGHT_TYPE_SEL == LIGHT_TYPE_POWER)
    SIG_MD_G_POWER_LEVEL_S, SIG_MD_G_POWER_LEVEL_SETUP_S,	// SIG_MD_G_POWER_LEVEL_S extend level.
        #endif
    	#if MD_SCENE_EN
        	#if 1// SHARE_ALL_LIGHT_STATE_MODEL_EN
    SIG_MD_SCENE_S, SIG_MD_SCENE_SETUP_S, 					// SIG_MD_SCENE_SETUP_S extend default transition time server.
        	#endif
        #endif
        #if (SHARE_ALL_LIGHT_STATE_MODEL_EN)
			#if MD_LOCATION_EN
	SIG_MD_G_LOCATION_S, SIG_MD_G_LOCATION_SETUP_S,
			#endif
			#if MD_PROPERTY_EN
	SIG_MD_G_USER_PROP_S, SIG_MD_G_ADMIN_PROP_S, SIG_MD_G_MFG_PROP_S, SIG_MD_G_CLIENT_PROP_S, //  is root model and is not extended by any other model.
			#endif
			#if MD_SENSOR_EN
	SIG_MD_SENSOR_S, SIG_MD_SENSOR_SETUP_S,
			#endif
			#if MD_TIME_EN
	SIG_MD_TIME_S, SIG_MD_TIME_SETUP_S,
			#endif
			#if MD_SCHEDULE_EN
	SIG_MD_SCHED_S, SIG_MD_SCHED_SETUP_S,
			#endif
			#if MD_MESH_OTA_EN
			// should not include Mesh OTA model.
			#endif
        #endif
    #endif
    
    #if MD_CLIENT_EN
    // most client models are root model, except OTA which set in sub_share_model_sig_others_server_extend_[]: both Firmware Update Client and The Firmware Distribution Client model extend BLOB Transfer Client model.
    #endif

    #if WIN32 
    0, //  because WIN32 can't assigned 0 size array.
    #endif
};

/**
 * @brief       sig models of each array inside that have extend relation ship, the first model is root model,
 *              they should share the same subscription list.
 *              such as SIG_MD_TIME_S is root model, SIG_MD_TIME_SETUP_S extend SIG_MD_TIME_S, so the should share the same subscription list.
 *              but SIG_MD_SENSOR_S should not share the same subscription list with SIG_MD_TIME_S.
 */
const u16 sub_share_model_sig_others_server_extend[][3] = { // model list for others. // should share when it is extend model.
#if MD_SERVER_EN
	// server model, the first model of each array is root model.
	#if MD_LOCATION_EN
	{SIG_MD_G_LOCATION_S, SIG_MD_G_LOCATION_SETUP_S},
	#endif
	#if MD_PROPERTY_EN
	{SIG_MD_G_USER_PROP_S, SIG_MD_G_ADMIN_PROP_S, SIG_MD_G_MFG_PROP_S}, // SIG_MD_G_CLIENT_PROP_S is root model and is not extended by any other model.
	#endif
	#if MD_SENSOR_EN
	{SIG_MD_SENSOR_S, SIG_MD_SENSOR_SETUP_S},
	#endif
	#if MD_TIME_EN
	{SIG_MD_TIME_S, SIG_MD_TIME_SETUP_S},
	#endif
	#if MD_SCHEDULE_EN
	{SIG_MD_SCHED_S, SIG_MD_SCHED_SETUP_S},
	#endif
#endif
	
	// client model
#if MD_MESH_OTA_EN
	#if 1 // (MD_SERVER_EN || MD_CLIENT_EN) // because SIG_MD_BLOB_TRANSFER_C and SIG_MD_FW_UPDATE_C will be used in light node.
	{SIG_MD_BLOB_TRANSFER_S, SIG_MD_FW_UPDATE_S, SIG_MD_FW_DISTRIBUT_S},
	{SIG_MD_BLOB_TRANSFER_C, SIG_MD_FW_UPDATE_C, SIG_MD_FW_DISTRIBUT_C},
	#endif
#endif
};

const u32 sub_share_model_vendor_server_extend[] = {
    #if MD_SERVER_EN
        #if (SHARE_ALL_LIGHT_STATE_MODEL_EN)
    VENDOR_MD_LIGHT_S,
            #if MD_VENDOR_2ND_EN
    VENDOR_MD_LIGHT_S2,
            #endif
        #endif
    #endif
    
    #if WIN32 
    0, //  because WIN32 can't assigned 0 size array.
    #endif
};



/**
 * @brief       This function set subscription for model which extend onoff model.
 * @param[in]   ele_adr	- element address
 * @param[in]   op		- opcode
 * @param[in]   sub_adr	- subscription list
 * @param[in]   uuid	- if sub_adr is virtual address, it is the Label UUID of it. if not virtual address, set to NULL.
 * @return      1: MODEL_SHARE_TYPE_ONOFF_SERVER_EXTEND
 * @note        
 */
model_share_type_e share_model_sub_onoff_server_extend(u16 op, u16 sub_adr, u8 *uuid, u16 ele_adr)
{
	//for(light_idx = 0; light_idx < (LIGHT_CNT); ++light_idx) // should not share to other element. if not, level command will not be able to send to group address.
	{			
		foreach_arr(i,sub_share_model_sig_onoff_server_extend){
			u32 model_list_set = sub_share_model_sig_onoff_server_extend[i];
			__UNUSED u8 st = 0;
			st = mesh_sub_search_and_set(op, ele_adr, sub_adr, uuid, model_list_set, 1);
			LOG_SHARE_MODEL_DEBUG(0, 0,"share  onoff model sub addr:0x%04x, ele_adr:0x%04x, model id:0x%04x, st:%d", sub_adr, ele_adr, model_list_set, st);
		}
			
		foreach_arr(i,sub_share_model_vendor_server_extend){
			u32 model_list_set = sub_share_model_vendor_server_extend[i]; // must u32 for vendor model id
			__UNUSED u8 st = 0;
			st = mesh_sub_search_and_set(op, ele_adr, sub_adr, uuid, model_list_set, 0);
			LOG_SHARE_MODEL_DEBUG(0, 0,"share vendor model sub addr:0x%04x, ele_adr:0x%04x, model id:0x%04x, st:%d", sub_adr, ele_adr, model_list_set, st);
		}
			
#if 0 // (LEVEL_STATE_CNT_EVERY_LIGHT >= 2) // no need to set twice. have been set in sub_share_model_sig.
   	#if (LIGHT_TYPE_CT_EN)
		mesh_sub_search_and_set(op, ele_adr, sub_adr, uuid, SIG_MD_LIGHT_CTL_TEMP_S, 1);
    #endif
    #if (LIGHT_TYPE_HSL_EN)
		mesh_sub_search_and_set(op, ele_adr, sub_adr, uuid, SIG_MD_LIGHT_HSL_HUE_S, 1);
		mesh_sub_search_and_set(op, ele_adr, sub_adr, uuid, SIG_MD_LIGHT_HSL_SAT_S, 1);
    #endif
#endif
	}

	return MODEL_SHARE_TYPE_ONOFF_SERVER_EXTEND;
}



/**
 * @brief       This function Check whether it is extend model of onoff model.
 * @param[in]   model_id	- model id
 * @param[in]   sig_model	- model id is sig model(1) or vendor model(0).
 * @return      0:no; 1:yes
 * @note        
 */
int is_need_share_model_sub_onoff_server_extend(u32 model_id, bool4 sig_model)
{
    if(sig_model){
        foreach_arr(i,sub_share_model_sig_onoff_server_extend){
            if(sub_share_model_sig_onoff_server_extend[i] == model_id){
                return 1;
            }
        }
    }else{
    	#if (SHARE_ALL_LIGHT_STATE_MODEL_EN)
		foreach_arr(i,sub_share_model_vendor_server_extend){
            if(sub_share_model_vendor_server_extend[i] == model_id){
                return 1;
            }
		}
		#endif
	}
	
    return 0;
}

/**
 * @brief       This function set subscription for current model and its extend model.
 * @param[in]   op			- opcode
 * @param[in]   sub_adr		- subscription address.
 * @param[in]   uuid		- if sub_adr is virtual address, it is the Label UUID of it. if not virtual address, set to NULL.
 * @param[in]   ele_adr		- element address
 * @param[in]   model_id	- model id
 * @param[in]   sig_model	- 1: model id is sig model; 0: vendor model.
 * @return      
 * @note        
 */
static inline model_share_type_e subscription_check_and_set_share_model(u16 op, u16 sub_adr, u8 *uuid, u16 ele_adr, u32 model_id, bool4 sig_model)
{
	if(is_need_share_model_sub_onoff_server_extend(model_id, sig_model)){
		return share_model_sub_onoff_server_extend(op, sub_adr, uuid, ele_adr);
	}else if(sig_model){
        foreach_arr(i,sub_share_model_sig_others_server_extend){
        	foreach_arr(j, sub_share_model_sig_others_server_extend[0]){
        		u32 model_list_search = sub_share_model_sig_others_server_extend[i][j];
        		if(model_list_search == model_id){
		        	foreach_arr(k, sub_share_model_sig_others_server_extend[i]){
						u32 model_list_set = sub_share_model_sig_others_server_extend[i][k];
						if(0 != model_list_set){ // 0 is config server model, and it is also invalid model here.
							__UNUSED u8 st = 0;
							st = mesh_sub_search_and_set(op, ele_adr, sub_adr, uuid, model_list_set, 1);
							LOG_SHARE_MODEL_DEBUG(0, 0,"share others model sub addr:0x%04x, ele_adr:0x%04x, model id:0x%04x, st:%d", sub_adr, ele_adr, model_list_set, st);
						}
					}
		        	return MODEL_SHARE_TYPE_OTHERS_SERVER_EXTEND;
	            }
            }
        }
    }
    
    return MODEL_SHARE_TYPE_NONE;
}


/**
 * @brief       This function set subscription address for extend model which also said share model.
 * @param[in]   op			- opcode
 * @param[in]   ele_adr		- element address
 * @param[in]   sub_adr		- subscription address
 * @param[in]   dst_adr		- destination address
 * @param[in]   uuid		- if sub_adr is virtual address, it is the Label UUID of it. if not virtual address, set to NULL.
 * @param[in]   model_id	- model id
 * @param[in]   sig_model	- model id is sig model(1) or vendor model(0).
 * @return      none
 * @note        
 */
_USER_CAN_REDEFINE_ void share_model_sub_by_rx_cmd(u16 op, u16 ele_adr, u16 sub_adr, u16 dst_adr,u8 *uuid, u32 model_id, bool4 sig_model)
{
    #if DUAL_VENDOR_EN
	if(DUAL_VENDOR_ST_ALI == provision_mag.dual_vendor_st)
    #endif
    {
        if(is_own_ele(ele_adr)){
            //u32 light_idx = (ele_adr - ele_adr_primary) / ELE_CNT_EVERY_LIGHT;
            subscription_check_and_set_share_model(op, sub_adr, uuid, ele_adr, model_id, sig_model);
        }
    }
}
#endif


/**
 * @brief       This function return to subscription status
 * @param[in]   st			- status
 * @param[in]	p_sub_set	- subscription parameters
 * @param[in]   sig_model	- model id is sig model(1) or vendor model(0).
 * @param[in]   adr_src		- source address
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int mesh_cmd_sig_cfg_model_sub_cb(u8 st,mesh_cfg_model_sub_set_t * p_sub_set,bool4 sig_model,u16 adr_src)
{
	return mesh_rsp_sub_status(st, p_sub_set, sig_model, adr_src);
}


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
#if WIN32
#else
    if( evt_code == EVENT_MESH_NODE_RC_LINK_START ||
        evt_code == EVENT_MESH_PRO_RC_LINK_START ){
        #if(!GATEWAY_ENABLE)
        if(blc_ll_getCurrentState() == BLS_LINK_STATE_CONN){
            blc_ll_setScanEnable (0, 0);
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
    	app_enable_scan_all_device ();
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
    	sw_version_big_endian_t *p_new = (sw_version_big_endian_t *)p_fw_id;
    	sw_version_big_endian_t *p_local = (sw_version_big_endian_t *)&fw_id_local.pid;
		u16 ver_new_little = get_little_end_version(p_fw_id->pid);
		u16 ver_local_little = get_little_end_version(fw_id_local.pid);
        if(ver_new_little > ver_local_little){
            accept = 1;
        }
        #else
		accept = 1;
        #endif
    }
    return accept;
    #else
    return 1;
    #endif
}

#if !WIN32
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
	blc_ll_setScanEnable (0, 0);
	#if __TLSR_RISCV_EN__
	ble_sts_t blc_ota_setOtaProcessTimeout(int timeout_second);
	blc_ota_setOtaProcessTimeout(OTA_CMD_INTER_TIMEOUT_S);
	#else
	bls_ota_setTimeout(OTA_CMD_INTER_TIMEOUT_S * 1000000); //set OTA timeout
	#endif
	#if GATT_LPN_EN
	bls_l2cap_requestConnParamUpdate_Normal();
	#endif
	
	#if (DUAL_MODE_ADAPT_EN || DUAL_MODE_WITH_TLK_MESH_EN)
	dual_mode_disable();
	// bls_ota_clearNewFwDataArea(0); // may disconnect
	#endif

	#if MESH_FLASH_PROTECTION_EN
	mesh_flash_unlock();
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
_USER_CAN_REDEFINE_ u8 ota_condition_enable()
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
void set_firmware_type_SIG_mesh()
{
    set_firmware_type(TYPE_SIG_MESH);
}

#if FW_START_BY_LEGACY_BOOTLOADER_EN

/**
 * @brief       This function set firmware type to "TYPE DUAL MODE ZIGBEE RESET"
 * @return      none
 * @note        
 */
void set_firmware_type_zb_with_factory_reset()
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
void set_firmware_type_init()
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
		
        irq_disable();
        if(OTA_SUCCESS_DEBUG == ota_reboot_type){
            while(1){show_ota_result(OTA_SUCCESS);}
        }else{
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
		
			#if 0 // !WIN32
			if(bls_ll_isConnectState()){
				bls_ll_terminateConnection (0x13); // add terminate cmd
				if(OTA_REBOOT_NO_LED == ota_reboot_type){
					sleep_us(200*1000);	// no need sleep, will delay at later show ota result. // wait terminate send completed.
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
void bls_l2cap_requestConnParamUpdate_Normal()
{
    if(blc_ll_getCurrentState() == BLS_LINK_STATE_CONN){
		#if BLE_MULTIPLE_CONNECTION_ENABLE
		bls_l2cap_requestConnParamUpdate (BLS_HANDLE_MIN, 16, 32, 0, 500);
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
	__UNUSED u16 conn_handle = BLS_HANDLE_MIN;
#if BLE_MULTIPLE_CONNECTION_ENABLE
	hci_le_enhancedConnCompleteEvt_t *pConnEvt = (hci_le_enhancedConnCompleteEvt_t *)p;
	conn_handle = pConnEvt->connHandle;
	bls_l2cap_setMinimalUpdateReqSendingTime_after_connCreate(conn_handle, 3000);
	bls_l2cap_requestConnParamUpdate (conn_handle, 16, 32, 0, 500);
#else
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
	mesh_node_prov_event_callback(EVENT_MESH_NODE_CONNECT);

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
	u8 conn_idx = 0;
	__UNUSED event_disconnection_t	*pd = (event_disconnection_t *)p;
#if BLE_MULTIPLE_CONNECTION_ENABLE
	conn_idx = get_slave_idx_by_conn_handle(pd->connHandle);
#endif
	app_adr[conn_idx] = 0;
	pair_login_ok = 0;
	reset_all_ccc();
    mesh_node_prov_event_callback(EVENT_MESH_NODE_DISCONNECT);
	#if MD_REMOTE_PROV
	mesh_rp_para_init();// avoid unexpected disconnect 
	#endif
	#if (ONLINE_STATUS_EN && !GATEWAY_ENABLE)
    mesh_report_status_enable(0);
	#endif
	#if (MD_DF_CFG_SERVER_EN && !FEATURE_LOWPOWER_EN)
	directed_proxy_dependent_node_delete(conn_idx);
	#endif
#if HOMEKIT_EN
	proc_homekit_pair = 0;
	ble_remote_terminate(0, 0, 0);
#endif
#if FEATURE_LOWPOWER_EN
	if(LPN_MODE_GATT_OTA == lpn_mode){
	    lpn_mode_tick = clock_time();
	    lpn_mode_set(LPN_MODE_NORMAL);
	}
	
	mesh_lpn_gatt_adv_refresh(pd->reason);
#endif

#if MD_ON_DEMAND_PROXY_EN
	mesh_on_demand_private_gatt_proxy_start();
#endif

	LOG_MSG_LIB(TL_LOG_NODE_SDK, 0, 0, "%s connHandle:0x%x", __func__, pd->connHandle);
}

#if (BLE_REMOTE_PM_ENABLE)

/**
 * @brief       This function will be called before suspend.
 * @param[in]   wakeup_tick	- suspend tick
 * @return      1
 * @note        
 */
int app_func_before_suspend(u32 wakeup_tick)
{
	if((wakeup_tick-clock_time() > 10*CLOCK_SYS_CLOCK_1MS)){// makesure enough time
		mesh_notifyfifo_rxfifo(); // Quick response in next interval, especially for long connect interval.
	}
	return 1;
}
#endif

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
void app_enable_scan_all_device ()
{
#if (__TLSR_RISCV_EN__ && BLE_MULTIPLE_CONNECTION_ENABLE)
	blc_ll_setScanEnable (BLC_SCAN_ENABLE, DUP_FILTER_DISABLE);
#else
	blc_ll_setScanEnable (BLS_FLAG_SCAN_ENABLE | BLS_FLAG_ADV_IN_SLAVE_MODE, 0);
#endif
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
	if(blc_ll_getCurrentState() != BLS_LINK_STATE_CONN){
		#if BLE_MULTIPLE_CONNECTION_ENABLE
		blc_ll_setAdvEnable(adv_en);
		#else
		bls_ll_setAdvEnable(adv_en);
		#endif

		if(scan_en){
			app_enable_scan_all_device();
		}
		else{
			rf_set_tx_rx_off();
			blc_ll_setScanEnable (0, 0);
		}	

		return SUCCESS;
	}

	return FAILURE;
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
	    #if AUDIO_MESH_MULTY_NODES_TX_EN
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

#if (0 == __TLSR_RISCV_EN__)
#if(__TL_LIB_8258__ || (MCU_CORE_TYPE && MCU_CORE_TYPE == MCU_CORE_8258) || (MCU_CORE_TYPE == MCU_CORE_8278)) 

/**
 * @brief       This function set firmware Size and firmware Boot Address
 * @param[in]   boot_addr		- boot start Address
 * @param[in]   firmware_size_k	- firmware size, unit kbyte.
 * @return      none
 * @note        
 */
void bls_ota_set_fwSize_and_fwBootAddr(int firmware_size_k, int boot_addr)
{
	ota_firmware_size_k = firmware_size_k;
	ota_program_bootAddr = boot_addr;
}
#endif
#endif

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


/**
 * @brief       This function is a test function.
 * @return      0
 * @note        
 */
int mesh_adv_tx_test()
{
	static u8 send_test_cnt;
 	if(send_test_cnt){
		send_test_cnt--;
		unprov_beacon_send(MESH_UNPROVISION_BEACON_WITH_URI,0);
		/*
		set_pb_gatt_adv(p->data,5);
		p->header.type = LL_TYPE_ADV_IND;
		memcpy(p->advA,tbl_mac,6);
		memcpy(p->data, p->data, 29);
		p->rf_len = 6 + 29;
		p->dma_len = p->rf_len + 2;
		return 1 ;
		*/
	}
	
	return 0;
}


/**
 * @brief       This function check if it is tx adv every time when "app advertise prepare handler" is called.
 * @return      0: no; 1: yes
 * @note        
 */
static inline int send_adv_every_prepare_cb()
{
    return (MI_SWITCH_LPN_EN || SPIRIT_PRIVATE_LPN_EN || (GATT_LPN_EN && !FEATURE_LOWPOWER_EN) ||DU_LPN_EN
            #if (FEATURE_LOWPOWER_EN && !BLE_MULTIPLE_CONNECTION_ENABLE)
            || is_lpn_support_and_en
            #endif
			#if __PROJECT_MESH_SWITCH__
            || 1
			#endif
            );
}

#if PROVISION_SUCCESS_QUICK_RECONNECT_ENABLE
#define PROVISION_SUCCESS_QUICK_RECONNECT_ADV_INTERVAL_US 	(30*1000)
#define PROVISION_SUCCESS_QUICK_RECONNECT_TIMEOUT_US 		(6 *1000*1000)

u32 g_provision_success_adv_quick_reconnect_tick = 0;
#endif

#if __PROJECT_MESH_SWITCH__
u8 gatt_adv_send_flag = 0;
#else
u8 gatt_adv_send_flag = 1;
#endif

/**
 * @brief       This function set advertise parameters for GATT adv.
 * @param[out]  p			- advertise to be send
 * @param[in]   rand_flag	- 1. need advertise random for current adv; 0: no need,
 * @return      0: no need to send adv. 1: need.
 * @note        
 */
static int gatt_adv_prepare_handler2(rf_packet_adv_t * p, int rand_flag)
{
#if PROVISION_SUCCESS_QUICK_RECONNECT_ENABLE
	if(g_provision_success_adv_quick_reconnect_tick
	&& clock_time_exceed(g_provision_success_adv_quick_reconnect_tick, PROVISION_SUCCESS_QUICK_RECONNECT_TIMEOUT_US)){
		g_provision_success_adv_quick_reconnect_tick = 0;
	}
#endif

    int ret = 0;

    if(is_provision_working()){
        return 0;
    }
    
    // dispatch gatt part 
#if   (!__PROJECT_MESH_PRO__ || PROVISIONER_GATT_ADV_EN)
	#if BLE_MULTIPLE_CONNECTION_ENABLE
	if((gatt_adv_send_flag && !blc_ll_isAllSlaveConnected())
	#else
    if((gatt_adv_send_flag && (blc_ll_getCurrentState() != BLS_LINK_STATE_CONN))
	#endif
	){		
		static u32 gatt_adv_tick = 0;
        static u32 gatt_adv_inv_us = 0;// send adv for the first time
        static u32 gatt_adv_cnt = 0;
        int send_now_flag = send_adv_every_prepare_cb();
        u32 interval_check_us = gatt_adv_inv_us;
        #if PROVISION_SUCCESS_QUICK_RECONNECT_ENABLE
        if(g_provision_success_adv_quick_reconnect_tick){
        	interval_check_us = PROVISION_SUCCESS_QUICK_RECONNECT_ADV_INTERVAL_US;
        }
        #endif
        if(send_now_flag || clock_time_exceed(gatt_adv_tick, interval_check_us)){
            if(!send_now_flag && gatt_adv_inv_us){
                if(rand_flag){
                    set_random_adv_delay(1);    // random 10~20ms
                }
                gatt_adv_inv_us = 0;   // TX in next loop
            }else{
                #if (FEATURE_LOWPOWER_EN)
 				if(is_lpn_support_and_en){
                    set_random_adv_delay_normal_adv(ADV_INTERVAL_RANDOM_MS);
                }
				#elif(GATT_LPN_EN)
					set_random_adv_delay_normal_adv(ADV_INTERVAL_RANDOM_MS);
                #endif
            
                gatt_adv_tick = clock_time();
    			gatt_adv_cnt++;
    			u32 adv_inter =0;
    			#if MI_API_ENABLE
    			if(is_provision_success()){
    				adv_inter = ADV_INTERVAL_MS_PROVED;
    			}else{
    				adv_inter = ADV_INTERVAL_MS;
    			}
    			#else
    				adv_inter = ADV_INTERVAL_MS;
    			#endif
    			
    			#if (MI_SWITCH_LPN_EN||DU_LPN_EN)
    			gatt_adv_inv_us = adv_inter; // use const 20ms mi_beacon interval 
    			#else
                gatt_adv_inv_us = (adv_inter - 10 - 5 + ((rand() % 3)*10)) * 1000; // random (0~20 + 0~10)ms // -10: next loop delay, -5: margin for clock_time_exceed.
                #endif
                
                #if (MI_API_ENABLE && !AIS_ENABLE)
                set_adv_mi_prehandler(p);
                ret = 1;
                #else
                if(0){  // just for compile
                }
    				#if HOMEKIT_EN
    			else if(gatt_adv_cnt%ADV_SWITCH_MESH_TIMES){

    				///////////////////// modify adv packet//////////////////////////////////////
    				blt_set_normal_adv_pkt(HK_CATEGORY, DEVICE_SHORT_NAME);
    				u16 hk_category = HK_CATEGORY;
    				blt_adv_update_pkt(ADV_PKT_CATEGORY_FIELD, (const u8 *)&hk_category);
    				extern u8 hk_setupHash[4];
    				blt_adv_update_pkt(ADV_PKT_SETUP_HASH, hk_setupHash);
    			
    				task_adv_pre(p);
    				ret=1;
    			}
    				#endif
    				#if (DUAL_VENDOR_EN)
    			else if((DUAL_VENDOR_ST_MI == provision_mag.dual_vendor_st)
    			    || ((DUAL_VENDOR_ST_STANDBY == provision_mag.dual_vendor_st) && (gatt_adv_cnt%4))){ // 3/4 adv for mi
    				set_adv_mi_prehandler(p);
                	ret = 1;
    			}
    				#endif
					
					#if DUAL_MESH_SIG_PVT_EN
					if((DUAL_MODE_SUPPORT_ENABLE == dual_mode_state) && (gatt_adv_cnt%2)){
						set_private_mesh_adv(p);
						adv_inter = ADV_INTERVAL_MS/2;
						ret = 1;
					}					
					#endif
					
					#if PROVISIONER_GATT_ADV_EN
				else{
					ret = set_adv_provisioner(p);
				}
					#else
                    	#if PB_GATT_ENABLE
                else if(provision_mag.gatt_mode == GATT_PROVISION_MODE){
					#if DU_ENABLE
					ret = du_adv_proc(p);
					#else
						#if URI_DATA_ADV_ENABLE
						static u32 uri_cnt=0;
						uri_cnt++;
						if(uri_cnt%2){
							set_adv_provision(p);
						}else{
							set_adv_uri_unprov_beacon(p);
						}
						#else
                    set_adv_provision(p);
						#endif
					ret = 1;
					#endif
                    // dispatch proxy part adv 
                }
                    	#endif 
    					#if FEATURE_PROXY_EN
                else if (provision_mag.gatt_mode == GATT_PROXY_MODE){
					#if DU_ENABLE
					ret = du_adv_proc(p);
					#else
                    ret = set_adv_proxy(p);
					#endif
                }
    					#endif
                else{
                }
					#endif
                #endif
            }
        }
        else{
        }
    }
#endif 

    return ret;
}


/**
 * @brief       This function set advertise parameters for GATT adv.
 * @param[out]  p			- advertise to be send
 * @param[in]   rand_flag	- 1. need advertise random for current adv; 0: no need,
 * @return      0: no need to send adv. 1: need.
 * @note        
 */
int gatt_adv_prepare_handler(rf_packet_adv_t * p, int rand_flag)
{
	int ret;
#if (PTS_TEST_EN && FEATURE_RELAY_EN) // SWITCH_ALWAYS_MODE_GATT_EN
    ret = relay_adv_prepare_handler(p, rand_flag); // for NLC which need relay, and use switch project will never be relay if it is after gatt adv prepare.
    if(ret){
        return ret;
    }
#endif

	ret = gatt_adv_prepare_handler2(p, rand_flag); // always return 1 for switch project, because prepare interval is equal to gatt adv interval.
	if(ret){
		return ret;
	}

#if (!PTS_TEST_EN && FEATURE_RELAY_EN)
    ret = relay_adv_prepare_handler(p, rand_flag); // lower than GATT ADV should be better. if not, adv can not be send when too much relay packet, such as there are many nodes that are publishing with a low period.
    if(ret){
        return ret;
    }
#endif

#if(PTS_TEST_EN && MD_CLIENT_EN && MD_SOLI_PDU_RPL_EN)
	ret = set_adv_solicitation(p);
    if(ret){
        return ret;
    }
#endif

#if(BEACON_ENABLE)
    if(0 == ret){   // priority is lowest
        ret = pre_set_beacon_to_adv(p);
    }
#endif

	return ret;
}

extern int rf_link_time_allow (u32 us);

/**
 * @brief       This function switch to scan mode after send ADV.
 * @param[in]   tx_adv	- tx adv flag
 * @return      0: switch fail; 1: switch success
 * @note        
 */
int mesh_send_adv2scan_mode(int tx_adv)
{
#if BLE_MULTIPLE_CONNECTION_ENABLE
	blc_ll_setAdvEnable(BLC_ADV_DISABLE);	
	bls_ll_setAdvParam_interval(GET_ADV_INTERVAL_MS(ADV_INTERVAL_10MS), GET_ADV_INTERVAL_MS(ADV_INTERVAL_10MS));
	blc_ll_setAdvEnable(BLC_ADV_ENABLE);
#else
	if((0 == tx_adv) || (BLS_LINK_STATE_ADV == blc_ll_getCurrentState()) || ((BLS_LINK_STATE_CONN == blc_ll_getCurrentState()) && (BLE_STATE_BRX_S != get_ble_state()) && rf_link_time_allow(EXTENDED_ADV_ENABLE?5000:3000))){
		blt_adv_expect_time_refresh(0); // disable refresh blt_advExpectTime
		blt_send_adv2scan_mode(tx_adv);
		blt_adv_expect_time_refresh(1);
		return 1;
	}
#endif

	return 0;
}

/**
 * @brief       This function is called when adv interval arrives.
 * @param[out]  p	- advertise to be send.
 * @return      0: no need to send adv. 1: need.
 * @note        
 */
int app_advertise_prepare_handler (rf_packet_adv_t * p)
{
#if EXTENDED_ADV_ENABLE
    if(blc_ll_getCurrentState() == BLS_LINK_STATE_CONN){
        if(abs( (int)(bltc.connExpectTime - clock_time()) ) < 5000 * sys_tick_per_us){
    		return 0;
    	}
	}
#endif

    int ret = 0;			// default not send ADV
    static u8 mesh_tx_cmd_busy_cnt;
	static u32 mesh_tx_cmd_tick = 0;
	static u32 adv_sn = 0;
	adv_sn++;
#if MD_PRIVACY_BEA
	if(p->header.txAddr){
		memcpy(p->advA,tbl_mac,6);	// may have been changed to random value, so need to re-init.
		p->header.txAddr = 0;
	}
#endif

	set_random_adv_delay(0);
	bls_set_adv_retry_cnt(0);
    
	#if 0
	if(mesh_adv_tx_test()){
	    return 1;
	}
	#endif
	
    my_fifo_t *p_fifo = 0;
    u8 *p_buf = 0;

	#if (MESH_MONITOR_EN)
	if(monitor_mode_en){
		p_fifo = &mesh_adv_cmd_fifo;
        p_buf = my_fifo_get(p_fifo);
        if(p_buf){
			my_fifo_pop(p_fifo); // not send adv in monitor mode
		}
		return 0;
	}
	#endif
	
    #if FEATURE_FRIEND_EN
    p_fifo = &mesh_adv_fifo_fn2lpn;
    p_buf = my_fifo_get(p_fifo);
    if(p_buf){
        mesh_cmd_bear_t *p_bear = (mesh_cmd_bear_t *)p_buf;
        __UNUSED mesh_transmit_t *p_trans_par = (mesh_transmit_t *)&p_bear->trans_par_val;
        ret = mesh_adv_cmd_set((u8 *)p, (u8 *)p_bear);
        bls_set_adv_retry_cnt(p_trans_par->count);
		
		#if BLE_MULTIPLE_CONNECTION_ENABLE
		if(p_trans_par->count){
			p_trans_par->count = 0; // send twice.
		}
		else
		#endif
        {
       	    my_fifo_pop(p_fifo);
		}
		
		if (mesh_tx_seg_par.busy && mesh_tx_seg_par.local_lpn_only) {
			mesh_seg_tx_set_one_pkt_completed(SEG_TX_DST_TYPE_LPN);
		}
        return ret;
    }
    #endif
    
	if(mesh_tx_cmd_busy_cnt && clock_time_exceed(mesh_tx_cmd_tick, (mesh_tx_cmd_busy_cnt + 1) * 10 * 1000)){
		mesh_tx_cmd_busy_cnt = 0;
	}
	
    p_fifo = &mesh_adv_cmd_fifo;
    p_buf = 0;
    if(0 == mesh_tx_cmd_busy_cnt){
        p_buf = my_fifo_get(p_fifo);
        if(!p_buf){
		    int ret2 = gatt_adv_prepare_handler(p, 1);
            if(ret2){
                return ret2;    // not only 1.
            }
        }
    }else{
        return gatt_adv_prepare_handler(p, 0);
    }

    if(p_buf){  // network layer + beacon
        mesh_cmd_bear_t *p_bear = (mesh_cmd_bear_t *)p_buf;
        mesh_transmit_t *p_trans_par = (mesh_transmit_t *)&p_bear->trans_par_val;
		mesh_tx_cmd_tick = clock_time();

		if(BEAR_TX_PAR_TYPE_DELAY == p_bear->tx_head.par_type){
			bear_delay_t *p_delay = (bear_delay_t *)&p_bear->tx_head;
			if(p_delay->count && !clock_time_exceed(p_delay->start_tick<<16,get_mesh_tx_delay_ms(p_delay)*1000)){
				return gatt_adv_prepare_handler(p, 1);
			}else{
				p_delay->count =0;
			}
		}
		#if (!SPIRIT_PRIVATE_LPN_EN)
		if(BEAR_TX_PAR_TYPE_REMAINING_TIMES != p_bear->tx_head.par_type){
       		set_random_adv_delay(1);
		}
		#endif
        int adv_retry_flag = 0
        			#if FEATURE_LOWPOWER_EN
        			|| mesh_lpn_quick_tx_flag
        			#endif
                            ;
        #if MESH_RX_TEST
		mesh_cmd_bear_t bear_backup;;
		#endif
		
        if(!adv_retry_flag){
        	if(MI_SWITCH_LPN_EN || SPIRIT_PRIVATE_LPN_EN||DU_LPN_EN){ // BLS_LINK_STATE_CONN == blc_ll_getCurrentState() || should not depend CI.
        	    mesh_tx_cmd_busy_cnt = 0;
            }
            else{
            	if(BEAR_TX_PAR_TYPE_PUB_FLAG == p_bear->tx_head.par_type){
					mesh_tx_cmd_busy_cnt = (p_trans_par->invl_steps+1)*5-1; // publish unit is 50ms
				}
				else if(BEAR_TX_PAR_TYPE_REMAINING_TIMES == p_bear->tx_head.par_type){
					mesh_tx_cmd_busy_cnt = 0;
				}
				#if MESH_RX_TEST
				else if(BEAR_TX_PAR_TYPE_MESH_RX_TEST == p_bear->tx_head.par_type){
					mesh_tx_cmd_busy_cnt = p_trans_par->invl_steps;
					memcpy(&bear_backup, p_bear, sizeof(bear_backup));
					bear_ttc_head_t *p_ttc_bear = (bear_ttc_head_t *)&p_bear->tx_head;
					
					if(p_bear->lt_ctl_seg.seg){
						;// TBD
					}
					else{
						cmd_ctl_ttc_t *p_ttc = (cmd_ctl_ttc_t *)p_bear->lt_ctl_unseg.data;
						u16 delta_100us = ((clock_time() - (p_ttc_bear->tick_base << RX_TEST_BASE_TIME_SHIFT)) / sys_tick_per_us / 100) + 4; 	// 4: mesh_sec_msg_enc_nw() cost 400us.
						p_ttc->ttc_100us = delta_100us;
						p_ttc->transmit_index = model_sig_cfg_s.nw_transmit.count - p_trans_par->count + 1; 	// tx index start from 1.
						mesh_cmd_nw_t *p_nw = &p_bear->nw;
						u8 len_nw = p_bear->len - 1; // 1:type
						u8 len_lt = len_nw - OFFSETOF(mesh_cmd_nw_t, data) - (p_nw->ctl ? SZMIC_NW64 : SZMIC_NW32);
						mesh_sec_msg_enc_nw((u8 *)p_nw, len_lt, SWAP_TYPE_NONE, MASTER, 0, len_nw, MESH_ADV_TYPE_MESSAGE, NONCE_TYPE_NETWORK, 0, 0);
					}					
				}
				#endif
				else{
            		mesh_tx_cmd_busy_cnt = p_trans_par->invl_steps;
				}
            }
        }


        ret = mesh_adv_cmd_set((u8 *)p, (u8 *)p_bear);

//		LOG_MSG_INFO(TL_LOG_NODE_SDK, 0, 0, "mesh_adv_cmd_fifo:%d adv_count:%d", my_fifo_data_cnt_get(&mesh_adv_cmd_fifo), p_trans_par->count);
		#if MESH_RX_TEST
		if(BEAR_TX_PAR_TYPE_MESH_RX_TEST == p_bear->tx_head.par_type){
			memcpy(p_bear, &bear_backup, sizeof(bear_backup));
		}
		#endif
		
		#if (PTS_TEST_EN && MD_SAR_EN)
		bls_set_adv_retry_cnt(1);// resend to improve pts dongle receive performance   
		#endif
        if(adv_retry_flag){
            bls_set_adv_retry_cnt(p_trans_par->count);
            p_trans_par->count = 0;
            mesh_tx_cmd_busy_cnt = 0;   // no need
        }

		if((BEAR_TX_PAR_TYPE_REMAINING_TIMES == p_bear->tx_head.par_type) && p_bear->tx_head.val[0]){
			p_bear->tx_head.val[0]--;
		}
        else if(0 == p_trans_par->count){
			//LOG_MSG_LIB(TL_LOG_NODE_SDK,0, 0,"pop cnt:%d,len:0x%x,type:0x%x",my_fifo_data_cnt_get(p_fifo),p_bear->len,p_bear->type);
            my_fifo_pop(p_fifo);
            if(p_fifo == &mesh_adv_cmd_fifo){   // only cmd fifo check segment,
                #if DEBUG_MESH_DONGLE_IN_VC_EN
                debug_mesh_report_one_pkt_completed();
                #else
                if(mesh_tx_seg_par.busy){
					mesh_seg_tx_set_one_pkt_completed(SEG_TX_DST_TYPE_NORMAL);	// just set flag, don't do too much function in irq, because irq stack.
                }
                #endif
            }  
        }else{
			p_trans_par->count--;
        }
		
		#if FEATURE_LOWPOWER_EN
		if(is_lpn_support_and_en){
			mesh_tx_cmd_busy_cnt = 0; // lpn use soft timer to send message after provision
		}
		#endif
		#if __PROJECT_MESH_SWITCH__
		mesh_tx_cmd_busy_cnt = 0;
		#endif
    }
	return ret;		//ready to send ADV packet
}

#if !BLE_MULTIPLE_CONNECTION_ENABLE
int app_l2cap_packet_receive (u16 handle, u8 * raw_pkt)
{
	#if((HCI_ACCESS != HCI_USE_NONE) && (! DEBUG_MESH_DONGLE_IN_VC_EN && !TESTCASE_FLAG_ENABLE))
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

_USER_CAN_REDEFINE_ void mesh_scan_rsp_init()
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

	bls_ll_setScanRspData((u8 *)&tbl_scanRsp, rsp_len);
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
void publish_when_powerup()
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
	#if !WIN32
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

#if WIN32
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
void vendor_id_check_and_update() //confirm cps and vendor model
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
#if !WIN32
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
	#if WIN32
	model_sig_cfg_s.frid = FEATURE_FRIEND_EN ? FRIEND_SUPPORT_ENABLE : FRIEND_NOT_SUPPORT;
	#else
	model_sig_cfg_s.frid = FRIEND_NOT_SUPPORT;
	#endif
	#if(SPIRIT_PRIVATE_LPN_EN)
	model_sig_cfg_s.frid = FRIEND_SUPPORT_DISABLE;
	#endif
#else
	#if (NODE_CAN_SEND_ADV_FLAG)
		#if MI_API_ENABLE // because mi mode have many mode ,it hard to distingwish the mode .
	model_sig_cfg_s.frid = FRIEND_NOT_SUPPORT;
		#else
	model_sig_cfg_s.frid = FEATURE_FRIEND_EN ? FRIEND_SUPPORT_ENABLE : FRIEND_NOT_SUPPORT;
		#endif
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
	#if (/*MD_PRIVACY_BEA &&  */MD_SERVER_EN && !WIN32) 	//  can not use MD_PRIVACY_BEA,  due to OTA.
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
#if (MD_DF_CFG_SERVER_EN && !WIN32)
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
typedef struct {
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
typedef struct {
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
typedef struct {
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
	p_mat->conn_handle = conn_handle;
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
}

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
	set_material_tx_cmd(&mat, op, par, par_len, adr_src, adr_dst, g_reliable_retry_cnt_def, rsp_max, 0, nk_array_idx, ak_array_idx, 0, BLS_HANDLE_MIN, immutable_flag, p_tx_head);
	return mesh_tx_cmd(&mat);
}

int mesh_tx_cmd2normal(u16 op, u8 *par, u32 par_len, u16 adr_src, u16 adr_dst, int rsp_max)
{
	return mesh_tx_cmd2normal_2(op, par, par_len, adr_src, adr_dst, rsp_max, 0);
}

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
	set_material_tx_cmd(&mat, op, par, par_len, adr_src, adr_dst, g_reliable_retry_cnt_def, rsp_max, 0, nk_array_idx, ak_array_idx, 0, BLS_HANDLE_MIN, immutable_flag, 0);
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

	set_material_tx_cmd(&mat, op, par, par_len, adr_src, adr_dst, g_reliable_retry_cnt_def, rsp_max, uuid, nk_array_idx, ak_array_idx, 0, BLS_HANDLE_MIN, immutable_flag, 0);
	return mesh_tx_cmd(&mat);
}

int SendOpParaDebug(u16 adr_dst, u8 rsp_max, u16 op, u8 *par, int len)
{
	#if WIN32
	return SendOpParaDebug_VC(adr_dst, rsp_max, op, par, len);
	#else
	LOG_MSG_LIB(TL_LOG_NODE_SDK,0, 0,"call send cmd API, sno:0x%06x op:0x%04x",mesh_adv_tx_cmd_sno,op);
	return mesh_tx_cmd2normal_primary(op, par, len, adr_dst, rsp_max);
	#endif
}

int SendOpParaDebug_vendor(u16 adr_dst, u8 rsp_max, u16 op, u8 *par, int len, u8 rsp_op, u8 tid_pos)
{
#if WIN32
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
#if!WIN32
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

#if GATEWAY_ENABLE
u8 mesh_access_layer_dst_addr_valid(mesh_cmd_nw_t *p_nw )
{
	#if 0
	if(0xc000 == p_nw->dst){
		return 1;
	}
	#endif
	return 0;
}
#endif
/**
* when receive a message, this function would be called if op supported and address match.
* @params: pointer to access layer which exclude op code.
* @p_res->cb: callback function define in mesh_cmd_sig_func[] or mesh_cmd_vd_func[]
*/

/** @addtogroup Mesh_Common
  * @{
  */
  
/** @defgroup Mesh_Common
  * @brief Mesh Common Code.
  * @{
  */

/**
 * @brief  when received a message, this function would be called 
 *   if opcode supported and address matched.
 * @param  params: Pointer to message data (excluding Opcode).
 * @param  par_len: The length of the message data.
 * @param  cb_par: Some information about function callbacks.
 * @retval Whether the message was processed
 *   (0 Message processed or -1 Message not processed)
 */
int mesh_rc_data_layer_access_cb(u8 *params, int par_len, mesh_cb_fun_par_t *cb_par)
{
    __UNUSED int log_len = par_len;
    #if HCI_LOG_FW_EN
    if(log_len > 10){
        if(BLOB_CHUNK_TRANSFER == cb_par->op){
            log_len = 10;
        }
    }
    #endif
	mesh_op_resource_t *p_res = (mesh_op_resource_t *)cb_par->res;
	#if AUDIO_MESH_EN
	if(VD_ASYNC_AUDIO_DATA != cb_par->op)
	#endif
	{
    LOG_MSG_LIB(TL_LOG_NODE_SDK,params, log_len,"rcv access layer,retransaction:%d,ttl:%d,src:0x%04x,dst:0x%04x sno:0x%06x op:0x%04x(%s),par_len:%d,par:",
            cb_par->retransaction,cb_par->p_nw->ttl,cb_par->adr_src,cb_par->adr_dst, cb_par->p_nw->sno[0]+(cb_par->p_nw->sno[1]<<8)+(cb_par->p_nw->sno[2]<<16), cb_par->op, get_op_string(cb_par->op,p_res->op_string), par_len);
	}

    if(!is_use_device_key(p_res->id, p_res->sig) || DEBUG_CFG_CMD_GROUP_USE_AK(cb_par->adr_dst)){ // user should not handle config model op code
        #if (VENDOR_MD_NORMAL_EN)
            #if ((VENDOR_OP_MODE_SEL == VENDOR_OP_MODE_DEFAULT)&&(DRAFT_FEATURE_VENDOR_TYPE_SEL == DRAFT_FEATURE_VENDOR_TYPE_NONE))
        if(IS_VENDOR_OP(cb_par->op)){
            if((cb_par->op >= VD_OP_RESERVE_FOR_TELINK_START) && (cb_par->op <= VD_OP_RESERVE_FOR_TELINK_END)){
                return -1;
            }
        }
            #endif
        #endif
        
        #if (!WIN32 && !FEATURE_LOWPOWER_EN)
        if(0 == mesh_tx_with_random_delay_ms){
            if((blc_ll_getCurrentState() == BLS_LINK_STATE_ADV) && (cb_par->op_rsp != STATUS_NONE)){
				u8 random_delay_step = 0;
				if(is_group_adr(cb_par->adr_dst)){
						#if DEBUG_CFG_CMD_GROUP_AK_EN
					random_delay_step = MESH_RSP_BASE_DELAY_STEP + (rand() % max_time_10ms);    // unit : ADV_INTERVAL_MIN(10ms)
						#else
					u32 random_range = MESH_RSP_RANDOM_DELAY_320ms;   // unit: ADV_INTERVAL_MIN == 10ms
					if(ADR_ALL_NODES == cb_par->adr_dst){
			        	if(get_mesh_current_cache_num() <= 20){
							
						}
						else if(get_mesh_current_cache_num() <= 50){
							random_range = MESH_RSP_RANDOM_DELAY_500ms;
						}
						else if(get_mesh_current_cache_num() <= 100){
							random_range = MESH_RSP_RANDOM_DELAY_1S;
						}
                       	else if(get_mesh_current_cache_num() < 205){
							random_range = MESH_RSP_RANDOM_DELAY_2S;
						}
						else{
							random_range = MESH_RSP_RANDOM_DELAY_3S;
						}
					}else{
					    // default, 0x20;
					}
					random_delay_step = MESH_RSP_BASE_DELAY_STEP + (rand() % random_range);
						#endif
				}else if (is_unicast_adr(cb_par->adr_dst)){
					#if MI_SWITCH_LPN_EN
					random_delay_step = 120 + (rand() %10);    // random delay between 1200~1300ms
					#endif
				}

				#if FAST_PROVISION_ENABLE
				if(VD_MESH_ADDR_SET == cb_par->op){
					random_delay_step = 0;	// not need random delay when reply VD_MESH_ADDR_SET_STS.
				}
				#endif
						
				mesh_rsp_delay_set(random_delay_step, 0); // set mesh_tx_with_random_delay_ms inside.
            }
        }
        #endif
        
        // TODO
    }
	
	#if DU_ULTRA_PROV_EN
	if(APPKEY_ADD == cb_par->op){
		my_fifo_reset(&mesh_adv_cmd_fifo); // response appkey status quickly
	}
	#endif
	
    int err = 0;
    /*! p_res->cb: callback function define in mesh_cmd_sig_func[] 
     or mesh_cmd_vd_func[] */
    #if MI_API_ENABLE
    if(p_res->cb && p_res->id == SIG_MD_CFG_SERVER){ // for the cb ,only proc the cfg model.
        err = p_res->cb(params, par_len, cb_par);   // use mesh_need_random_delay in this function in library.
    }
	mesh_mi_rx_cb(params, par_len, cb_par);
	#else
	if(p_res->cb){ // have been check in library, check again.
		#if MD_SERVER_EN
		g_op_access_layer_rx = cb_par->op;
		#endif
        err = p_res->cb(params, par_len, cb_par);   // use mesh_tx_with_random_delay_ms in this function in library.
		#if MD_SERVER_EN
		g_op_access_layer_rx = 0; // reset to invalid.
		#endif
    }
	#endif
    mesh_tx_with_random_delay_ms = 0; // must be clear here 
    #if DF_TEST_MODE_EN
	if(DIRECTED == mesh_key.sec_type_sel){
		mesh_df_led_event(mesh_key.net_key[mesh_key.netkey_sel_dec][0].nid_d, 0);
	}
	#endif

	#if (NO_TX_RSP2SELF_EN && DISTRIBUTOR_UPDATE_SERVER_EN)
	if(((mesh_op_resource_t *)(cb_par->res))->status_cmd){
    	u8 buf[sizeof(mesh_rc_rsp_t)] = {0};   // because block status may be very long. so set the size of buf to max.
        u32 size_op = SIZE_OF_OP(cb_par->op);
    	mesh_rc_rsp_t *p_rc_rsp = (mesh_rc_rsp_t *)buf;
    	if((par_len + size_op) < (sizeof(buf) - OFFSETOF(mesh_rc_rsp_t,data))){
        	p_rc_rsp->len = GET_RSP_LEN_FROM_PAR(par_len, size_op);
        	p_rc_rsp->src = cb_par->adr_src;
        	p_rc_rsp->dst = cb_par->adr_dst;
        	memcpy(p_rc_rsp->data, &cb_par->op, size_op);
        	memcpy(p_rc_rsp->data + size_op, params, par_len);
        	
        	mesh_ota_master_rx(p_rc_rsp, cb_par->op, size_op);
    	}
	}
	#endif
	
    return err;
}

int mesh_seg_block_ack_cb(const mesh_cmd_bear_t *p_bear_ack, st_block_ack_t st)
{
	const mesh_cmd_lt_ctl_seg_ack_t *p_lt_ctl_seg_ack = (mesh_cmd_lt_ctl_seg_ack_t *)&p_bear_ack->lt_ctl_ack;
	u16 op = mesh_tx_seg_par.match_type.mat.op;
	op = op;								// will be optimized
	p_lt_ctl_seg_ack = p_lt_ctl_seg_ack;	// will be optimized
	if(ST_BLOCK_ACK_BUSY == st){			// destination address has been confirmed that it is a unicast address before.
		mesh_tx_segment_finished();			// spec define to cancel in "Segmentation behavior" // if not cancel, it will cause too much block busy ack.
		LOG_MSG_LIB(TL_LOG_NODE_BASIC,0,0,"RX node segment is busy,so tx flow cancel");
	}

	return 0;
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

#if GATEWAY_ENABLE||WIN32
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
	memcpy(head+2, p_head, (head_len+2>sizeof(head))?(sizeof(head)-2):head_len);
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
_align_4_ u8 uart_hw_tx_buf[160 + UART_HW_HEAD_LEN]; // not for user
#else
_align_4_ u8 uart_hw_tx_buf[HCI_TX_FIFO_SIZE_USABLE + UART_HW_HEAD_LEN]; // not for user;  2: sizeof(fifo.len)
#endif
const u16 UART_TX_LEN_MAX = (sizeof(uart_hw_tx_buf) - UART_HW_HEAD_LEN);

void uart_drv_init()
{
#if(MCU_CORE_TYPE == MCU_CORE_8258 || (MCU_CORE_TYPE == MCU_CORE_8278))
//note: dma addr must be set first before any other uart initialization! (confirmed by sihui)
	u8 *uart_rx_addr = hci_rx_fifo.p + (hci_rx_fifo.wptr & (hci_rx_fifo.num-1)) * hci_rx_fifo.size;
	uart_recbuff_init( uart_rx_addr, hci_rx_fifo.size, uart_hw_tx_buf);
	uart_gpio_set(UART_TX_PIN, UART_RX_PIN);

	uart_reset();  //will reset uart digital registers from 0x90 ~ 0x9f, so uart setting must set after this reset

	//baud rate: 115200
	#if (CLOCK_SYS_CLOCK_HZ == 16000000)
		uart_init(9, 13, PARITY_NONE, STOP_BIT_ONE);
	#elif (CLOCK_SYS_CLOCK_HZ == 24000000)
		uart_init(12, 15, PARITY_NONE, STOP_BIT_ONE);
	#elif (CLOCK_SYS_CLOCK_HZ == 32000000)
		uart_init(30, 8, PARITY_NONE, STOP_BIT_ONE);
	#elif (CLOCK_SYS_CLOCK_HZ == 48000000)
		uart_init(25, 15, PARITY_NONE, STOP_BIT_ONE);
	#endif

	uart_dma_enable(1, 1); 	//uart data in hardware buffer moved by dma, so we need enable them first

	irq_set_mask(FLD_IRQ_DMA_EN);
	dma_chn_irq_enable(FLD_DMA_CHN_UART_RX | FLD_DMA_CHN_UART_TX, 1);   	//uart Rx/Tx dma irq enable
#elif __TLSR_RISCV_EN__
	uart_drv_init_B91m();
#else

	//todo:uart init here
	uart_io_init(UART_GPIO_SEL);
#if(CLOCK_SYS_CLOCK_HZ == 32000000)
	CLK32M_UART115200;
#elif(CLOCK_SYS_CLOCK_HZ == 16000000)
	CLK16M_UART115200;
#endif
	uart_BuffInit(hci_rx_fifo_b, hci_rx_fifo.size, uart_hw_tx_buf);
#endif
}

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
			if(0 == my_fifo_push_hci_tx_fifo(p->data, rx_len, 0, 0));
			#else
			if(uart_Send(p->data, rx_len))
			#endif
			{
				my_fifo_pop(&hci_rx_fifo);
			}
			#else
			app_hci_cmd_from_usb_handle(p->data, rx_len);
			my_fifo_pop(&hci_rx_fifo);
			#endif
		}
	}
	return 0;
}

int blc_hci_tx_to_uart ()
{
	my_fifo_buf_t *p = (my_fifo_buf_t *)my_fifo_get (&hci_tx_fifo);
	#if 1
	#if  __TLSR_RISCV_EN__
#define uart_Send(p_data,len_data)	uart_Send_dma_with_busy_hadle(p_data, len_data)

	uart_tx_busy_timeout_poll();
	
	if (p && !uart_tx_is_busy_dma_tick())
	#else
	if (p && !uart_tx_is_busy ())
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
			if (uart_Send((u8 *)p, p->len+2))
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
	
	return 0;
	
#if  __TLSR_RISCV_EN__
#undef uart_Send
#endif
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


void wd_clear_lib()
{
	#if (MODULE_WATCHDOG_ENABLE&&!WIN32)
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

/*************** log *************/
const char  TL_LOG_STRING[TL_LOG_LEVEL_MAX][MAX_LEVEL_STRING_CNT] = {
    {"[USER]:"},
    {"[LIB]:"},    // library and important log
    {"[ERR]:"},
    {"[WARN]:"},
    {"[INFO]:"},
    {"[DEBUG]:"},
};

const char tl_log_module_mesh[TL_LOG_MAX][MAX_MODULE_STRING_CNT] ={
	"(mesh)","(provision)","(lowpower)","(friend)",
	"(proxy)","(GattProv)","(log_win32)","(GATEWAY)",
	"(KEYBIND)","(sdk)","(Basic)","(RemotePro)","(directed)","(cmd_rsp)",
	"(common)","(cmd_name)","(sdk_nw_ut)","(iv_update)","(gw_vc_log)","(USER)"
};

STATIC_ASSERT(TL_LOG_LEVEL_MAX < 8); // because only 3bit, please refer to LOG_GET_LEVEL_MODULE
STATIC_ASSERT(TL_LOG_MAX < 32); // because 32bit, and LOG_GET_LEVEL_MODULE

_PRINT_FUN_RAMCODE_ int tl_log_msg_valid(char *pstr,u32 len_max,u32 module)
{
	int ret =1;
	if (module >= TL_LOG_MAX){
		ret =0;
	}else if (!(BIT(module)& TL_LOG_SEL_VAL)){
		ret =0;
	}
	if(ret){
		#if WIN32
			#if VC_APP_ENABLE
			strcat_s(pstr,len_max,tl_log_module_mesh[module]);
			#else 
			strcat(pstr,tl_log_module_mesh[module]);
			#endif 
		#else
		    u32 str_len = strlen(pstr);
		    str_len = min(str_len, MAX_LEVEL_STRING_CNT);
		    if((str_len + MAX_MODULE_STRING_CNT) <= len_max ){
			    memcpy(pstr+str_len,tl_log_module_mesh[module],MAX_MODULE_STRING_CNT);
			}
		#endif
	}
	return ret;
}

#if WIN32 
void set_log_windown_en (u8 en);
void tl_log_file(u32 level_module,u8 *pbuf,int len,char  *format,...)
{
	char tl_log_str[MAX_STRCAT_BUF] = {0};
	u32 module = LOG_GET_MODULE(level_module);
	u32 log_level = LOG_GET_LEVEL(level_module);
	set_log_windown_en(0);
	if((0 == log_level) || (log_level > TL_LOG_LEVEL_MAX)){
	    return ;
	}else{
        memcpy(tl_log_str,TL_LOG_STRING[log_level - 1],MAX_LEVEL_STRING_CNT);
	}
	
	if(!tl_log_msg_valid(tl_log_str,sizeof(tl_log_str), module)){
	    if(log_level != TL_LOG_LEVEL_ERROR){
		    return ;
		}
	}
	
	va_list list;
	va_start( list, format );
	LogMsgModuleDlg_and_buf(pbuf,len,tl_log_str,format,list);
	set_log_windown_en(1);
}

#endif

_PRINT_FUN_RAMCODE_ int tl_log_msg(u32 level_module, void *pbuf, int len, char *format, ...)
{
#if (WIN32 || HCI_LOG_FW_EN)
	char tl_log_str[MAX_STRCAT_BUF] = {0};
	u32 module = LOG_GET_MODULE(level_module);
	u32 log_level = LOG_GET_LEVEL(level_module);
	
	if((0 == log_level) || (log_level > TL_LOG_LEVEL_MAX)){
	    return -1;
	}else{
        memcpy(tl_log_str,TL_LOG_STRING[log_level - 1],MAX_LEVEL_STRING_CNT);
	}
	
	if(!tl_log_msg_valid(tl_log_str,sizeof(tl_log_str), module)){
	    if(log_level != TL_LOG_LEVEL_ERROR){
		    return -1;
		}
	}
	
	va_list list;
	va_start( list, format );
	LogMsgModuleDlg_and_buf((u8 *)pbuf,len,tl_log_str,format,list);	
#endif

    return 0;
}

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
		#if DONGLE_PROVISION_EN ||WIN32
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


#if 0
void tl_log_msg_err(u16 module,u8 *pbuf,int len,char  *format,...)
{
#if (WIN32 || HCI_LOG_FW_EN)
	char tl_log_str[MAX_STRCAT_BUF] = TL_LOG_ERROR_STRING;
	if(!tl_log_msg_valid(tl_log_str,sizeof(tl_log_str), module)){
		//return ;
	}
	va_list list;
	va_start( list, format );
	LogMsgModuleDlg_and_buf(pbuf,len,tl_log_str,format,list);	
#endif
}

void tl_log_msg_warn(u16 module,u8 *pbuf,int len,char  *format,...)
{
#if (WIN32 || HCI_LOG_FW_EN)
	char tl_log_str[MAX_STRCAT_BUF] = TL_LOG_WARNING_STRING;
	if(!tl_log_msg_valid(tl_log_str,sizeof(tl_log_str),module)){
		return ;
	}
	va_list list;
	va_start( list, format );
	LogMsgModuleDlg_and_buf(pbuf,len,tl_log_str,format,list);	
#endif
}

void tl_log_msg_info(u16 module,u8 *pbuf,int len,char  *format,...)
{
#if (WIN32 || HCI_LOG_FW_EN)
	char tl_log_str[MAX_STRCAT_BUF] = TL_LOG_INFO_STRING;
	if(!tl_log_msg_valid(tl_log_str,sizeof(tl_log_str),module)){
		return ;
	}
	va_list list;
	va_start( list, format );
	LogMsgModuleDlg_and_buf(pbuf,len,tl_log_str,format,list);	
#endif
}

void user_log_info(u8 *pbuf,int len,char  *format,...)
{
    char tl_log_str[MAX_STRCAT_BUF] = TL_LOG_INFO_STRING;
	if(!tl_log_msg_valid(tl_log_str,sizeof(tl_log_str),TL_LOG_USER)){
		return ;
	}
	va_list list;
	va_start( list, format );
	LogMsgModuleDlg_and_buf(pbuf,len,tl_log_str,format,list);	
}

void tl_log_msg_dbg(u16 module,u8 *pbuf,int len,char  *format,...)
{
#if (WIN32 || HCI_LOG_FW_EN)
	char tl_log_str[MAX_STRCAT_BUF] = TL_LOG_DEBUG_STRING;
	if(!tl_log_msg_valid(tl_log_str,sizeof(tl_log_str),module)){
		return ;
	}
	va_list list;
	va_start( list, format );
	LogMsgModuleDlg_and_buf(pbuf,len,tl_log_str,format,list);	
#endif
}
#endif

#if !WIN32
#if HCI_LOG_FW_EN
_attribute_no_retention_bss_ char log_dst[EXTENDED_ADV_ENABLE ? 512 : 256];// make sure enough RAM
int LogMsgModdule_uart_mode(u8 *pbuf,int len,char *log_str,char *format, va_list list)
{
    #if (GATEWAY_ENABLE)
	return 1;
	#endif
	#if (HCI_ACCESS == HCI_USE_UART)    
	char *p_buf;
	char **pp_buf;
	p_buf = log_dst+2;
	pp_buf = &(p_buf);

	memset(log_dst, 0, sizeof(log_dst));
	u32 head_len = print(pp_buf,log_str, 0)+print(pp_buf,format,list);   // log_dst[] is enough ram.
	head_len += 2;  // sizeof(log_dst[0]) + sizeof(log_dst[1]) 
	if(head_len > sizeof(log_dst)){
	    while(1){   // assert, RAM conflict, 
	        show_ota_result(OTA_DATA_CRC_ERR);
	    }
	}
	
	if(head_len > HCI_TX_FIFO_SIZE_USABLE){
		return 0;
	}

	log_dst[0] = HCI_LOG; // type
	log_dst[1] = head_len;
	u8 data_len_max = HCI_TX_FIFO_SIZE_USABLE - head_len;
	if(len > data_len_max){
		len = data_len_max;
	}
	memcpy(log_dst+head_len, pbuf, len);
	my_fifo_push_hci_tx_fifo((u8 *)log_dst, head_len+len, 0, 0);
	if(is_lpn_support_and_en){
		blc_hci_tx_to_uart ();
			#if PTS_TEST_EN
		while(uart_tx_is_busy ());
			#endif
	}
	#endif
    return 1;
}

_PRINT_FUN_RAMCODE_ int LogMsgModule_io_simu(u8 *pbuf,int len,char *log_str,char *format, va_list list)
{
	char *p_buf;
	char **pp_buf;
	p_buf = log_dst;
	pp_buf = &(p_buf);
	u32 head_len = print(pp_buf,log_str, 0)+print(pp_buf,format,list);   // log_dst[] is enough ram.
	if((head_len + get_len_Bin2Text(len))> sizeof(log_dst)){
        // no need, have been check buf max in printf_Bin2Text. // return 0;
	}
	u32 dump_len = printf_Bin2Text((char *)(log_dst+head_len), sizeof(log_dst) - head_len, (char *)(pbuf), len);
	uart_simu_send_bytes((u8 *)log_dst, head_len+dump_len);
	return 1;
}
#endif


_PRINT_FUN_RAMCODE_ int LogMsgModuleDlg_and_buf(u8 *pbuf,int len,char *log_str,char *format, va_list list)
{
    #if (0 == HCI_LOG_FW_EN)
    return 1;
    #else
        #if 1
    return LogMsgModule_io_simu(pbuf,len,log_str,format,list);
        #else
    return LogMsgModdule_uart_mode(pbuf,len,log_str,format,list);
        #endif
    #endif
}
#endif	

#if !WIN32
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

void bootloader_ota_setNewFirmwwareStorageAddress()
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

#if UI_KEYBOARD_ENABLE
/**
 * @brief      callback function of LinkLayer Event "BLT_EV_FLAG_SUSPEND_ENTER"
 * @param[in]  e - LinkLayer Event type
 * @param[in]  p - data pointer of event
 * @param[in]  n - data length of event
 * @return     none
 */
void mesh_set_sleep_wakeup(u8 e, u8 *p, int n)
{
	if(key_released){
		bls_pm_setWakeupSource(PM_WAKEUP_PAD);
	}
}

/**
 * @brief       keyboard initialization
 * @return      none
 * @note        
 */
void ui_keyboard_wakeup_io_init()
{
	/////////// keyboard gpio wakeup init ////////
	#if KB_LINE_MODE
	u32 pin[] = KB_SCAN_PINS;
	for (int i=0; i<(sizeof (pin)/sizeof(*pin)); i++)
	{
		cpu_set_gpio_wakeup (pin[i], 0, 1);  //scan pin pad low wakeup deepsleep
	}
	#else
	u32 pin[] = KB_DRIVE_PINS;
	for (int i=0; i<(sizeof (pin)/sizeof(*pin)); i++)
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


