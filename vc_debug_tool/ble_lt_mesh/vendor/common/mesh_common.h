/********************************************************************************************************
 * @file	mesh_common.h
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
#pragma once

#include "tl_common.h"
#include "mesh_lpn.h"
#if __TLSR_RISCV_EN__
#include "stack/ble/ble.h"
#else
#include "proj_lib/ble/ll/ll.h"
#endif
#include "mesh_fn.h"
#include "time_model.h"
#include "scheduler.h"
#include "mesh_property.h"
#include "vendor/common/battery_check.h"
#if (!WIN32 && EXTENDED_ADV_ENABLE)
#if __TLSR_RISCV_EN__
#include "stack/ble/controller/ll/ll_ext_adv.h"
#include "chip_adapt_layer/app_audio.h"
#else
#include "stack/ble/ll/ll_ext_adv.h"
#endif
#endif



#if (__PROJECT_MESH__ || WIN32)
#include "vendor/mesh/app.h"
#elif (__PROJECT_MESH_LPN__)
#include "vendor/mesh_lpn/app.h"
#elif (__PROJECT_MESH_SWITCH__)
#include "vendor/mesh_switch/app.h"
#elif (__PROJECT_MESH_PRO__ || __PROJECT_MESH_GW_NODE__)
#include "vendor/mesh_provision/app.h"
#elif (__PROJECT_SPIRIT_LPN__)
#include "vendor/spirit_lpn/app.h"
#elif (__PROJECT_BOOTLOADER__)
#include "vendor/boot_loader/app.h"
#elif (__PROJECT_ACL_PER_DEMO__)
#include "vendor/acl_peripheral_demo/app.h"
#else
//#error please include app.h
#endif

#include "vendor/user_app/user_app.h"
#include "vendor/common/nlc/mesh_nlc.h"

#if LLSYNC_ENABLE
#include "ble_qiot_export.h"
#include "vendor/common/llsync/samples/telink/main/ll_app_mesh.h"
#elif PLATFORM_TELINK_EN
#include "vendor/common/telink_platform/telink_platform.h"
#endif

/** @addtogroup Mesh_Common
  * @{
  */
  
/** @defgroup Mesh_Common
  * @brief Mesh Common Code.
  * @{
  */
#if (__TLSR_RISCV_EN__)
#define RETENTION_RAM_SIZE_USE			DEEPSLEEP_MODE_RET_SRAM_LOW64K
#else
#define RETENTION_RAM_SIZE_USE			DEEPSLEEP_MODE_RET_SRAM_LOW32K
#endif

#define RELAY_ROUTE_FILTE_TEST_EN		0

enum{
	RIGHT_PACKET_RET,
	PACKET_WAIT_COMPLETE,
	ERR_PACKET_RET,
	ERR_PACKET_LEN,
	ERR_TYPE_SAR,
};

typedef struct{
	u16 val;
}u16_struct;    // for u8 buffer which not sure u16 align.

typedef struct{
	u32 val;
}u32_struct;    // for u8 buffer which not sure u32 align.


#if 1   // test firmware size
#define NOP_TEST_BYTE_50       \
    CLOCK_DLY_5_CYC;CLOCK_DLY_5_CYC;CLOCK_DLY_5_CYC;CLOCK_DLY_5_CYC;CLOCK_DLY_5_CYC
#define NOP_TEST_BYTE_100       \
    NOP_TEST_BYTE_50;NOP_TEST_BYTE_50
#define NOP_TEST_BYTE_500       \
    NOP_TEST_BYTE_100;NOP_TEST_BYTE_100;NOP_TEST_BYTE_100;NOP_TEST_BYTE_100;NOP_TEST_BYTE_100
#define NOP_TEST_BYTE_1K        \
    NOP_TEST_BYTE_500;NOP_TEST_BYTE_500;\
    CLOCK_DLY_10_CYC;CLOCK_DLY_10_CYC;CLOCK_DLY_4_CYC
#define NOP_TEST_BYTE_5K        \
    NOP_TEST_BYTE_1K;NOP_TEST_BYTE_1K;NOP_TEST_BYTE_1K;NOP_TEST_BYTE_1K;NOP_TEST_BYTE_1K
#define NOP_TEST_BYTE_10K       \
    NOP_TEST_BYTE_5K;NOP_TEST_BYTE_5K
#endif

void i2c_sim_init(void);
void i2c_sim_write(u8 id, u8 addr, u8 dat);
u8 i2c_sim_read(u8 id, u8 addr);
void i2c_sim_burst_read(u8 id, u8 addr, u8 *p, int n);
void i2c_sim_burst_write(u8 id, u8 addr,u8 *p,int n);


#if (SPIRIT_PRIVATE_LPN_EN && AIS_ENABLE)
#define MESH_RSP_BASE_DELAY_STEP			120  //unit:ADV_INTERVAL_MIN(10ms)
#else
#define MESH_RSP_BASE_DELAY_STEP			18  //unit:ADV_INTERVAL_MIN(10ms)
#endif
#define MESH_RSP_RANDOM_DELAY_320ms			32 //unit:ADV_INTERVAL_MIN(10ms)			
#define MESH_RSP_RANDOM_DELAY_500ms			50 //unit:ADV_INTERVAL_MIN(10ms)			
#define MESH_RSP_RANDOM_DELAY_1S			100 //unit:ADV_INTERVAL_MIN(10ms)			
#define MESH_RSP_RANDOM_DELAY_2S			200 //unit:ADV_INTERVAL_MIN(10ms)
#define MESH_RSP_RANDOM_DELAY_3S			300 //unit:ADV_INTERVAL_MIN(10ms)

#define MESH_POWERUP_BASE_TIME				500 // when power up, after (this base time + random), the node will publish lightness model or other model, details refer to publish_when_powerup_();

#if (0 == USER_REDEFINE_SCAN_RSP_EN)        // user can define in user_app.h
typedef struct{
	u8 len;
	u8 type;            // 0xFF: manufacture data
	u16 vendor_id;      // vendor id follow spec
	u8 mac_adr[6];
	u16 adr_primary;
	#if (NLC_PROFILE_EN)
	// local name
	u8 len_name;
	u8 type_name;
	u8 name[NLC_LOCAL_NAME_LEN];
	#else
    u8 rsv_telink[8];	// not for user // TO be defined, no need to be sent now.
    u8 rsv_user[11];	// TO be defined, no need to be sent now.
    #endif
}mesh_scan_rsp_t;
#endif

typedef struct ais_pri_data{
	u8 length;
	u8 type;
	u16 cid;
	u8 vid;
	union{
		u8 fmsk;
		struct{
			u8 ble_version:2;//00: BLE4.0 01: BLE4.2 10: BLE5.0 11: BLE5.0 above
			u8 ota_support:1;
			u8 authen_en:1;
			u8 secret_type:1;// 0:one device type on key, 1:one device one key
			u8 prov_flag:1;
			u8 rfu:2;
		};
	};
	u32 pid;
	u8 mac[6];
} ais_pri_t;

//---------
extern u8 g_reliable_retry_cnt_def;
extern u16 g_reliable_retry_interval_min;
extern u16 g_reliable_retry_interval_max;
extern u8 pair_login_ok;
extern u16 mesh_tx_with_random_delay_ms;
extern const u16 UART_TX_LEN_MAX;
extern u16 gateway_seg_buf_len;
extern u8 gateway_seg_buf[];

static inline int is_valid_val_100(u8 val_100)
{
	return (val_100 <= 100);
}

static inline int mesh_get_proxy_hci_type()
{
    return PROXY_HCI_SEL;
}
//---------
void mesh_ble_connect_cb(u8 e, u8 *p, int n);
void mesh_ble_disconnect_cb(u8 *p);
void mesh_conn_param_update_req();
void vendor_id_check_and_update();
void mesh_global_var_init();
void mesh_tid_save(int ele_idx);
void mesh_vd_init();
void lpn_node_io_init();
void lpn_proc_keyboard (u8 e, u8 *p, int n);
void entry_ota_mode(void);
void set_mesh_ota_type();
void set_firmware_type_init();
void set_firmware_type_SIG_mesh();
void set_firmware_type_zb_with_factory_reset();
void set_ota_firmwaresize(int adr);
int ota_set_flag();
void mesh_ota_reboot_set(u8 type);
void mesh_ota_reboot_check_refresh();
void mesh_ota_reboot_proc();
u8 proc_telink_mesh_to_sig_mesh(void);
u8 send_vc_fifo(u8 cmd,u8 *pfifo,u8 cmd_len);
void app_enable_scan_all_device ();
/**
 * @brief       This function server to set adv enable and scan enable
 * @param[in]   adv_en	- 0: adv disable 1:adv enable
 * @param[in]   scan_en	- 0: scan disable 1:scan enable
 * @return      SUCCESS or FAILURE
 * @note        
 */
int mesh_set_adv_scan_enable(int adv_en, int scan_en);
int	app_device_mac_match (u8 *mac, u8 *mask);
int app_l2cap_packet_receive (u16 handle, u8 * raw_pkt);
int chn_conn_update_dispatch(u8 *p);
void sim_tx_cmd_node2node();
/**************************mesh_send_adv2scan_mode**************************
function : send adv immediately
para:
	tx_adv: true:send adv and switch to scan mode(if enable). 0: switch to scan mode((if enable)) without send adv 
ret: 1  means OK 
	 0 means err 
****************************************************************************/
int mesh_send_adv2scan_mode(int tx_adv);
#if (BLE_MULTIPLE_CONNECTION_ENABLE)
int blt_send_adv_cb();
#endif
int app_advertise_prepare_handler (rf_packet_adv_t * p);
void my_att_init(u8 mode);
void ble_mac_init();
void usb_id_init();
void uart_drv_init();
void adc_drv_init();
int blc_rx_from_uart (void);
int blc_hci_tx_to_uart ();
void mesh_scan_rsp_init();
int SendOpParaDebug(u16 adr_dst, u8 rsp_max, u16 op, u8 *par, int len);
int SendOpParaDebug_vendor(u16 adr_dst, u8 rsp_max, u16 op, u8 *par, int len, u8 rsp_op, u8 tid);
void share_model_sub_by_rx_cmd(u16 op, u16 ele_adr, u16 sub_adr, u16 dst_adr,u8 *uuid, u32 model_id, bool4 sig_model);

typedef enum{
	MODEL_SHARE_TYPE_NONE 					= 0,
	MODEL_SHARE_TYPE_ONOFF_SERVER_EXTEND 	= 1,
	MODEL_SHARE_TYPE_OTHERS_SERVER_EXTEND 	= 2,
}model_share_type_e;
model_share_type_e share_model_sub_onoff_server_extend(u16 op, u16 sub_adr, u8 *uuid, u16 ele_adr);
void APP_reset_vendor_id(u16 vd_id);
int mesh_rc_data_layer_access_cb(u8 *params, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_tx_cmd2self_primary(u32 light_idx, u8 *ac, int len_ac);
u32 get_mesh_pub_interval_ms(u32 model_id, bool4 sig_model, mesh_pub_period_t *period);
void publish_when_powerup();
int is_need_response_to_self(u16 adr_dst, u16 op);
int app_func_before_suspend();
void beacon_str_disable();

#if GATEWAY_ENABLE
typedef u8 (*access_layer_dst_addr)(mesh_cmd_nw_t *p_nw);
void register_access_layer_dst_addr_callback(void* p);
/**
 * @brief  the callback function for register_access_layer_dst_addr_callback
 *   gateway can set the dst addr valid by return true even not subscribe the addr
 * @param  p_nw: point to network message
 * @return: 0:invalid,  1:valid
 */
u8 mesh_access_layer_dst_addr_valid(mesh_cmd_nw_t *p_nw);
#endif

extern u8 gatt_adv_send_flag;
extern u16 g_vendor_id;
extern u16 g_msg_vd_id;
extern u32 g_vendor_md_light_vc_s;
extern u32 g_vendor_md_light_vc_s2;
extern u32 g_vendor_md_light_vc_c;
extern u16 publish_powerup_random_ms;

extern u16 sub_adr_onoff ;
void set_unprov_beacon_para(u8 *p_uuid ,u8 *p_info);
void set_provision_adv_data(u8 *p_uuid,u8 *oob_info);
#if !BLE_MULTIPLE_CONNECTION_ENABLE
void bls_set_adv_delay(u8 delay);	// unit : 625us
void bls_set_adv_retry_cnt(u8 cnt); // default :0
#endif
void set_random_adv_delay_normal_adv(u32 random_ms);
void rp_active_scan_req_proc();

void set_sec_req_send_flag(u8 flag);// set the sec req send or not 
#if BLE_MULTIPLE_CONNECTION_ENABLE
void  blc_att_setServerDataPendingTime_upon_ClientCmd(u16 num_10ms);
#else
void  blc_att_setServerDataPendingTime_upon_ClientCmd(u8 num_10ms);
#endif
void reliable_retry_cnt_def_set(u8 retry_cnt);
int mesh_rsp_handle_cb(mesh_rc_rsp_t *p_rsp);
int app_hci_cmd_from_usb (void);
int app_hci_cmd_from_usb_handle (u8 *buff, int n);
int gateway_sar_pkt_reassemble(u8 *buf,int len );
int gateway_sar_pkt_segment(u8 *par,int par_len, u16 valid_fifo_size, u8 *head, u8 head_len);

int mesh_send_cl_proxy_bv03(u16 node_adr);
int mesh_send_cl_proxy_bv04(u16 node_adr);
int mesh_send_cl_proxy_bv05(u16 node_adr);
int mesh_send_cl_proxy_bv06(u16 node_adr);
int mesh_send_cl_proxy_bv07(u16 node_adr);
u8 get_flash_data_is_valid(u8 *p_data,u16 len);
void erase_ecdh_sector_restore_info(u32 adr,u8 *p_data,u16 len);

#if WIN32
int SendOpParaDebug_VC(u16 adr_dst, u8 rsp_max, u16 op, u8 *par, int len);  // only for SIG command now
#endif
extern u8  mesh_user_define_mode ;

extern u8 PROVISION_ATT_HANDLE; 
extern u8 GATT_PROXY_HANDLE;
extern const u8 SERVICE_GATT_OTA_HANDLE;
extern const u8 SERVICE_CHANGE_ATT_HANDLE_SLAVE;
extern const u8 ONLINE_ST_ATT_HANDLE_SLAVE;
extern u8 proc_homekit_pair;
#if __TLSR_RISCV_EN__
extern volatile const u8 key_encode_bin[];
#else
extern const u8 key_encode_bin[];
#endif

enum{
	BLE_4_0 =0,
	BLE_4_2,
	BLE_5_0,
	BLE_5_0_HIGH,
};
typedef struct{
	mesh_page0_head_t cps_head;
	u8 	mac[6];
}simple_flow_dev_uuid_t;



// set dev key struct for gateway 
typedef struct{
    u16 unicast;
    u8 dev_key[16];
}mesh_gw_set_devkey_str;

typedef struct{
    u8 fastbind;
    u8 key_idx[2];
    u8 key[16];
}mesh_gw_appkey_bind_str;

typedef struct{
    u8 eve;
    u16 adr;
    u8 mac[6];
    u8 uuid[16];
}gateway_prov_event_t;
typedef struct{
    u16 op;
    u16 src;
    u8 ac_par[32];
}gateway_upload_mesh_src_t;

// gateway cmd event log part 
typedef struct{
			u16 src;
			u16 dst;
			u16 opcode;
			u8 para[20];
}gateway_upload_mesh_cmd_str;

typedef struct{
	u8 dsk[32];
	u8 dpk[64];
	u32 valid;
	u32 crc;
}mesh_ecdh_key_str;
#define ECDH_KEY_VALID_FLAG		0xa5

#define ECDH_KEY_STS_TWO_VALID	1
#define ECDH_KEY_STS_NONE_VALID	2
#define ECDH_KEY_STS_ONE_VALID	3

typedef struct {
	u32	tk_pincode;
} gap_smp_TkDisplayEvt_single_connect_t;

void provision_ecc_calc();
void set_dev_uuid_for_simple_flow( u8 *p_uuid);
u8 prov_uuid_fastbind_mode(u8 *p_uuid);
u8 is_homekit_pair_handle(u8 handle); 

u8 ota_condition_enable();
void create_sha256_input_string(char *p_input,u8 *pid,u8 *p_mac,u8 *p_secret);
extern u8 model_need_key_bind_whitelist(u16 *key_bind_list_buf,u8 *p_list_cnt,u8 max_cnt);
extern int LogMsgModuleDlg_and_buf(u8 *pbuf,int len,char *log_str,char *format, va_list list);
void mesh_node_prov_event_callback(u8 evt_code);
void wd_clear_lib();
void bls_ota_set_fwSize_and_fwBootAddr(int firmware_size_k, int boot_addr);
void mesh_cfg_cmd_force_seg_set(material_tx_cmd_t *p,mesh_match_type_t *p_match_type);
void mesh_secure_beacon_loop_proc();
int mesh_cmd_sig_cfg_model_sub_cb(u8 st,mesh_cfg_model_sub_set_t * p_sub_set,bool4 sig_model,u16 adr_src);
void start_reboot(void);
void blc_l2cap_register_pre_handler(void *p);
#if!WIN32
uint32_t soft_crc32_telink(const void *buf, size_t size, uint32_t crc);
#endif
void vendor_md_cb_pub_st_set2ali();
int pre_set_beacon_to_adv(rf_packet_adv_t *p);
u16 swap_u16_data(u16 swap);
void mesh_seg_must_en(u8 en);
int mesh_dev_key_candi_decrypt_cb( u16 src_adr,int dirty_flag ,const u8* ac_backup ,unsigned char *r_an, 
											       unsigned char* ac, int len_ut, int mic_length);
void mi_vendor_cfg_rsp_proc();
void set_random_adv_delay(int en);
void bls_l2cap_requestConnParamUpdate_Normal();
int telink_rand_num_generator(u8* p_buf, u8 len);
int is_need_send_sec_nw_beacon();
void tn_p256_dhkey_fast(u8 *r, u8 *s, u8 *x, u8 *y);
void mesh_gatt_adv_beacon_enable(u8 enable);
void mbedtls_sha256_flash( unsigned long addr, size_t ilen, unsigned char output[32], int is224 );

// ----------- mesh_log.c -------
const char * get_op_string(u16 op, const char *str_in);
const char * get_op_string_ctl(u8 op, int filter_cfg);
void print_log_mesh_tx_cmd_layer_upper_ctl_ll(material_tx_cmd_t *p_mat, int err, int filter_cfg);


// ------------ clock -----------
#if (0 == __TLSR_RISCV_EN__)
#if (CHIP_TYPE >= CHIP_TYPE_8258)
    #if (CLOCK_SYS_CLOCK_HZ == 16000000)
#define SYS_CLK_CRYSTAL     (SYS_CLK_16M_Crystal)
    #elif (CLOCK_SYS_CLOCK_HZ == 24000000)
#define SYS_CLK_CRYSTAL     (SYS_CLK_24M_Crystal)
    #elif (CLOCK_SYS_CLOCK_HZ == 32000000)
#define SYS_CLK_CRYSTAL     (SYS_CLK_32M_Crystal)
    #elif (CLOCK_SYS_CLOCK_HZ == 48000000)
#define SYS_CLK_CRYSTAL     (SYS_CLK_48M_Crystal)
    #else
#error clock not set properly
    #endif
#else
#define SYS_CLK_CRYSTAL     // NULL
#endif
#endif

void clock_switch_to_highest();
void clock_switch_to_normal();

static inline int is_tlk_gatt_ota_busy(){
#if __TLSR_RISCV_EN__
	return blt_ota_isOtaBusy();
#else
	return blcOta.ota_start_flag;
#endif
}

#if (__TLSR_RISCV_EN__)
	#if (!BLE_MULTIPLE_CONNECTION_ENABLE)
#include "stack/ble/controller/ll/ll_stack.h"

ble_sts_t blc_ll_setScanEnable (scan_en_t scan_enable, dupFilter_en_t filter_duplicate);

static inline void set_blt_state(u8 st)
{
	bltParam.blt_state = st;
}

static inline u8 get_ble_state()
{
	return bltParam.ble_state;
}

static inline u8 get_blt_busy()
{
	return bltParam.blt_busy;
}

static inline void set_blt_busy(u8 busy)
{
	bltParam.blt_busy = busy;
}

static inline void set_sdk_mainLoop_run_flag(u8 flag)
{
	bltParam.sdk_mainLoop_run_flag = flag; // 1 means already run mainloop.
}
	#endif
#else
extern u8 blt_busy;

static inline void set_blt_state(u8 st)
{
	blt_state = st;
}

static inline u8 get_ble_state()
{
	return ble_state;
}

static inline u8 get_blt_busy()
{
	return blt_busy;
}

static inline void set_blt_busy(u8 busy)
{
	blt_busy = busy;
}

extern int sdk_mainLoop_run_flag;
static inline void set_sdk_mainLoop_run_flag(u8 flag)
{
	sdk_mainLoop_run_flag = flag;	// 1 means already run mainloop.
}
#endif

#if (FW_START_BY_BOOTLOADER_EN)
void bootloader_ota_setNewFirmwwareStorageAddress();
#endif

// adv_filter_proc()
void enable_mesh_provision_buf();
void disable_mesh_provision_buf();
void enable_mesh_kr_cfg_filter();
void disable_mesh_kr_cfg_filter();

#if UI_KEYBOARD_ENABLE
extern u8 key_released;
void mesh_set_sleep_wakeup(u8 e, u8 *p, int n);
void ui_keyboard_wakeup_io_init();
#endif
/**
  * @}
  */

/**
  * @}
  */


