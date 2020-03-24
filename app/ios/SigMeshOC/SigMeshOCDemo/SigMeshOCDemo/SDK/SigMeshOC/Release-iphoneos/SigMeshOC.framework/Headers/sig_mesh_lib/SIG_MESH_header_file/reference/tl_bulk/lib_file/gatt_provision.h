/********************************************************************************************************
 * @file     gatt_provision.h 
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
#ifndef __GATT_PROVISON_H
#define __GATT_PROVISON_H
#include "app_config.h"
#if VC_APP_ENABLE
#include "../resource.h"		// main symbols
#endif
#include "../../../ble_lt_mesh/vendor/common/app_provison.h"
#include "../../../ble_lt_mesh/proj/common/tutility.h"

// store data include into the database 
#define PROVISION_WRITE_HANDLER	0x15
#define PROXY_WRITE_HANDLER 	0x1e

typedef enum{
	PROV_NORMAL_RET =0,
	PROV_TIMEOUT_RET =1,
	PROV_COMFIRM_ERR =2,
	PROV_FAIL_CMD_ERR =3,
};

typedef struct{
	u8 provison_send_state;
	u8 provison_rcv_state;
	u8 initial_pro_roles;
	u8 pro_cmd;
    u8 pro_bearerCode;
	u8 pro_fail_code;
	u8  rsp_ack_transnum;
	u8  hash[8];
	u8  random[8];
	u8  oob_info[2];
	u8  device_uuid[16];
	u8 	gatt_mode; 
	u8  pro_stop_flag;
	provison_net_info_str pro_net_info;
	u8 oob_type;
	u8 oob_buf[16];
}gatt_pro_para_mag;

typedef struct{
	u8  test_u8;
	u16 test_u16;
	u32 test_u32;
}test_data_str;

// store data include into the database 
typedef struct{
	// ADV part ,get from the adv and the uuid 
	u8 service_uuid[2];
	u8 service_data[16];
	u8 oob_info[2];
	// basic function part 
	u8  net_work_key[16];
	u16  key_index;
	u8  flags;
	u8  iv_index[4];
	u8  unicast_address[2];
	// provision part data 
	u8 gatt_pro_ppk[64];
	u8 gatt_pro_psk[32];
	u8 gatt_pro_edch[32];
	u8 gatt_pro_random[16];
	u8 gatt_pro_session_key[16];
	u8 gatt_pro_session_nonce[16];
	u8 gatt_pro_dat[40];
}PRO_STORE_DATABASE;

extern u8 provision_write_handle;
extern u8 proxy_write_handle;
extern u8 online_st_write_handle;
extern u16 att_mtu;

extern unsigned char prov_newkey[16];
extern unsigned char prov_key_index[2];
extern unsigned char prov_ivi_update_flag;
extern unsigned char prov_iv_index[4];
extern unsigned char prov_unicast_address[2];

extern gatt_pro_para_mag  gatt_provision_mag;
extern u8  gatt_pro_auth[16];
extern u8 gatt_pro_random[16];

extern u8 gatt_pro_dat[0x40];
extern u8 gatt_salt[16];
extern u8 gatt_dev_key[16];
extern u8 gatt_para_pro[400];
extern u16 gatt_para_len;

#if WIN32
extern void check_prov_timeout();
extern void provision_end_callback(u8 reason);
#endif 
extern unsigned char store_data_to_app(PRO_STORE_DATABASE *p_base);
extern unsigned char app_to_restore_data(PRO_STORE_DATABASE *p_base);
extern void provision_dispatch(u8 *p,u8* proxy_buf,u16 proxy_len);

extern unsigned char test_pragma_pack();

extern unsigned int get_lib_version();

extern void provision_dispatch_pkt(u8 *p ,u8 len,u8* proxy_buf,u16 proxy_len);
extern u8 provision_dispatch_direct(u8 *p,u8 len,u8* proxy_buf,u16* p_proxy_len);
extern void start_provision_invite();
extern void set_provision_adv(u8 *p_service_uuid ,u8 *p_service_data, u8 *p_oob_info);
extern u8 set_app_key_pro_data(u8 *p_dat,u8 len);
extern int gatt_write_transaction_callback(u8 *p,u16 len,u8 msg_type);
extern void gatt_pro_para_mag_init();
extern void set_pro_psk_ppk(u8 * random,u8 random_len );
extern void gatt_rcv_pro_pkt_dispatch(u8 *p ,u8 len );
extern unsigned char SendPkt(unsigned short handle,unsigned char *p ,unsigned char  len);
extern void prov_timer_clr();
extern void set_provision_mag_oob_info(u8 oob_type, u8 *p_oob,u8 len );

// need to realize by outside part 

int mesh_set_prov_cloud_para(u8 *p_pid,u8 *p_mac);

int mesh_sec_prov_cloud_comfirm(u8* p_comfirm,u8 *p_comfirm_key,u8 *pro_random);

int mesh_cloud_dev_comfirm_check(u8 *p_comfirm_key,u8* p_dev_random,u8*p_dev_comfirm);
void set_gatt_provision_net_info(u8 p_netkey[16],u16*p_key_index,u8*p_flag,u8 p_ivi[4],u16*p_unicast);
void gatt_provision_net_info_callback();
extern void prov_timer_start();
extern int gatt_get_node_ele_cnt(u16 unicast ,u8 *p_ele);
extern u8 prov_write_data_trans(u8 *p,u16 len,u8 msg_type);

void mesh_rp_client_para_reset();
// gatt provision step function proc part 
void start_provision_invite();
void gatt_prov_send_invite();
void gatt_prov_rcv_pro_fail(mesh_pro_data_structer *p_rcv);
void gatt_prov_rcv_capa(mesh_pro_data_structer *p_rcv,mesh_pro_data_structer *p_send);
void gatt_prov_send_start(mesh_pro_data_structer *p_rcv,mesh_pro_data_structer *p_send);
void gatt_prov_send_pubkey(mesh_pro_data_structer *p_rcv,mesh_pro_data_structer *p_send);
void gatt_prov_rcv_pubkey(mesh_pro_data_structer *p_rcv,mesh_pro_data_structer *p_send);
void gatt_prov_send_comfirm(mesh_pro_data_structer *p_rcv,mesh_pro_data_structer *p_send);
void gatt_prov_rcv_comfirm(mesh_pro_data_structer *p_rcv,mesh_pro_data_structer *p_send);
void gatt_prov_send_random(mesh_pro_data_structer *p_rcv,mesh_pro_data_structer *p_send);
void gatt_prov_rcv_random(mesh_pro_data_structer *p_rcv,mesh_pro_data_structer *p_send);
void gatt_prov_send_pro_data(mesh_pro_data_structer *p_rcv,mesh_pro_data_structer *p_send);
void gatt_prov_rcv_pro_complete();
u8 win32_prov_working();


#include "app_config.h"
#include "host_fifo.h"
#include "hw_fun.h"
#endif 

