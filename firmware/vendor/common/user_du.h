/********************************************************************************************************
 * @file	user_du.h
 *
 * @brief	This is the header file for BLE SDK
 *
 * @author	Mesh Group
 * @date	2021
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
#ifndef __USER_DU_H
#define __USER_DU_H
#include "tl_common.h"
#if __TLSR_RISCV_EN__
#include "stack/ble/ble.h"
#else
#include "proj_lib/ble/ll/ll.h"
#endif
#include "proj_lib/ble/blt_config.h"
#include "vendor/common/user_config.h"

#define	DU_SAMPLE_DATA_TEST_EN			0
#define DU_APP_ADV_CONTROL_EN			0

#define DU_CERT_IOT_DEV_CMD				0x80
#define DU_CERT_IOT_DEV_RSP				0x81
#define DU_TRANS_RESUME_BREAK_CMD		0x82
#define DU_TRANS_RESUME_BREAK_RSP		0x83
#define DU_EXCHANGE_BUF_CHK_SIZE_CMD	0x84
#define DU_EXCHANGE_BUF_CHK_SIZE_RSP	0x85
#define DU_START_OTA_CMD				0x86
#define DU_START_OTA_RSP				0x87

#define DU_BUFFER_CHK_CMD				0x88
#define DU_BUFFER_CHK_RSP				0x89
#define DU_READ_DEV_VERSION_CMD   		0x8a
#define DU_READ_DEV_VERSION_RSP   		0x8b

#define DU_WHOLE_IMG_CHK_CMD			0x90
#define DU_WHOLE_IMG_CHK_RSP			0x91
#define DU_OVERWRITE_IMG_CMD			0x92
#define DU_OVERWRITE_IMG_RSP			0x93

typedef struct{
	u8 len1;//connect
	u8 ad_type1;
	u8 ad_dat1;
	u8 len2;// uuid
	u8 ad_type2;
	u16 ad_dat2;
	u8 len3;//shortname
	u8 ad_type3;
	u8 ad_dat3[6];
	u8 len4;
	u8 ad_type4;
	u16 cid;
	u8 dev_type;
	u16 ver;
	u8 ios_mac[6];
}du_ota_adv_str;

typedef struct{
	u8 len;
	u8 ad_type;
	u16 cid;
	u8 dev_type;
	u16 ver;
	u8 ios_mac[6];
}du_ota_end_adv_str;

typedef struct{
	u8 opcode;
	u8 buf[1];
}du_cmd_str;

typedef struct{
	u32 rand;
}du_break_point_cmd_str;

typedef struct{
	u32 break_point;
	u32 new_rand;
}du_break_point_rsp_str;

typedef struct{
	u32 seg_size;
	u32 buf_size;
}du_exchange_str;

typedef struct{
	u32 image_size;
	u32 offset;
}du_start_ota_str;

typedef struct{
	u8 sts;
}du_start_ota_sts_str;

typedef struct{
	u32 crc32;
}du_buf_chk_str;

typedef struct{
	u8 sts;
	u32 adjust_offset;
}du_buf_chk_sts_str;

typedef struct{
	u8 sts;
	u32 crc;
}du_whole_img_chk_str;

typedef struct{
	u8 sts;
	u32 crc;
}du_whole_img_chk_sts_str;

typedef struct{
	u32 magic;
}du_magic_code_str;

typedef struct{
	u8 sts;
}du_magic_code_rsp_str;

typedef struct{
	u8  dev_type;
	u16 ver;
}du_dev_version_rsp_str;

typedef struct{
	u32 break_point;
	u32 rand_code;
	u32 seg_size;// suppose the segbuf is 244
	u32 buf_size;
	u32 buf_idx;
	u32 image_size;
	u32 image_offset;
	u32 crc;
	u32 ota_suc;	
}du_ota_str;
extern du_ota_str *p_ota;
void test_du_sha256_cal();
#if BLE_MULTIPLE_CONNECTION_ENABLE
int	du_ctl_Write (u16 conn_handle, void *p);
int du_fw_proc(u16 conn_handle, void *p);
#else
int	du_ctl_Write (void *p);
int du_fw_proc(void *p);
#endif
u8 du_adv_proc(rf_packet_adv_t * p);
int du_vd_event_set(u8*p_buf,u8 len,u16 dst);
//int du_vd_temp_event_send(u16 op,u16 val,u16 dst);
void du_loop_proc();
void du_vd_send_loop_proc();
void du_ui_proc_init();
void du_ui_proc_init_deep();
void du_bind_end_proc(u16 gw_adr,u32 time_s);
void du_bls_ll_setAdvEnable(int adv_enable);
void du_adv_enable_proc(void);
void du_prov_bind_check();
void du_time_req_start_proc();

void update_du_busy_s(u8 delay_s);

void du_set_gateway_adr(u16 adr);
u16  du_get_gateway_adr(void);
void du_enable_gateway_adr(u8 check_bind);
u8   du_get_bind_flag(void);



#define VD_DU_TEMP_CMD 	0x010d
#define VD_DU_HUMI_CMD 	0x010F
#define VD_DU_POWER_CMD	0x0549
#define VD_DU_THP_CMD	0x0A01
#define VD_DU_TIME		0XF01F

typedef struct{
	u8 tid;
	u16 op;
}du_time_req;


#define VD_DU_GROUP_DST	0xF000
typedef struct{
	u8  tid;
	u16 op;
	u8  val[5];
}vd_du_event_str;

typedef struct{
	vd_du_event_str op_str;
	u8              op_str_len;
	u16             gateway_adr;
}vd_du_event_t;

typedef struct{
	u16 temp;
	u16 humi;
	u8  power;
}htp_para;

typedef struct{
	u16 adr;
	u8 rfu[2];
	u16 flag;
	u16 gw_adr;
}du_store_str;

enum{
	// xiaodu speaker
	DU_TYPE_SCAN_EN 	= 0xc0,
	DU_TYPE_RANDOM 		= 0xd0,
	DU_TYPE_RANDOMC 	= 0xd1,
	DU_TYPE_PROV_DATA 	= 0xd2, // D3
	DU_TYPE_UUID 		= 0xd3,

	// android app
	DU_ANDROID_RANDOM 		= 0xe0,
	DU_ANDROID_RANDOMC 		= 0xe1,
	DU_ANDROID_PROV_DATA 	= 0xe2,
	DU_ANDROID_UUID 		= 0xe3,
	DU_ANDROID_APPKEY 		= 0xe4,
	DU_ANDROID_APPKEY_ST 	= 0xe5,

	DU_ANDROID_CONTROL	= 0xe6,
	DU_CONTROL_ACK		= 0xe7,
};

enum{
	PROV_TYPE_RANDOM,
	PROV_TYPE_CONFIRM,
	PROV_TYPE_PROV_DATA,
	PROV_TYPE_CPMPLETE,
	PROV_TYPE_CONFIG,
	PROV_TYPE_CONFIG_ACK,
	GENIE_CONTROL_MSG,
	GENIE_CONTROL_RSP,
};

typedef struct{
	u8 msg_id;
	union{
		u8 pkt_num;
		struct{
			u8 seg_o:4; 
			u8 seg_N:4;
		};
	};
	u8 len_payload;
	u8 payload[29]; 
	u32 tick;	
}genie_nw_cache_t;

typedef struct{
	u8 nid;
	u8 msg_id;
	union{
		u8 pkt_num;
		struct{
			u8 seg_o:4; 
			u8 seg_N:4;
		};
	};
	u8 len_payload;
	#if DU_ENABLE
	u8 payload[19];
	#elif AIS_ENABLE
	u8 payload[18]; 
	#endif
}genie_nw_t;

typedef struct{
    u8 trans_par_val;	
    u8 len_flag;
	u8 flag_type;
	u8 flags;
	u8 manu_len;
	u8 manu_type;
	u16 cid;
	#if AIS_ENABLE
	u8 vid;
	#endif
	u8 msg_type;
	genie_nw_t genie_nw;
}mesh_bear_rsp2_app_t;

typedef struct{ 
	u8 len_flag;
	u8 flag_type;
	u8 flags;
	u8 manu_len;
	u8 manu_type;
	u16 cid;
	#if AIS_ENABLE
	u8 vid;
	#endif
	u8 msg_type;
	union{		
		u8 payload[22];
		genie_nw_t genie_nw;
	};
}genie_manu_factor_data_t;

typedef struct{
	u8 len;
	u8 ad_type;
	u16 cid;
	u8 data_type;

	union{
		u8 data[23];
		genie_nw_t nw;
	};
}du_manu_data_t;

typedef struct{
	u16 cid;
	#if AIS_ENABLE
	u8 vid;
	#endif
	u8 msg_type;
	u8 mac[2];
	u8 msg_id;
	union{
		u8 pkt_num;
		struct{
			u8 seg_o:4; 
			u8 seg_N:4;
		};
	};
	u8 len_payload;
	#if AIS_ENABLE
	u8 payload[7];
	#elif DU_ENABLE
	u8 payload[8];
	#endif
} ios_prov_t;

typedef struct{
	u16 cid;
	#if AIS_ENABLE
	u8 vid;
	#endif
	u8 msg_type;
	u8 nid;
	u8 msg_id;
	union{
		u8 pkt_num;
		struct{
			u8 seg_o:4; 
			u8 seg_N:4;
		};
	};
	u8 len_payload;
	#if AIS_ENABLE
	u8 payload[8];
	#elif DU_ENABLE
	u8 payload[9];
	#endif
} ios_nw_t;

typedef struct{ 
	u8 len_flag;
	u8 flag_type;
	u8 flags;
#if 0 // ios below version 12 does't have power type 	
	u8 power_len;
	u8 power_type;
	u8 tx_power;
#endif
	u8 uuid_len;
	u8 uuid_type;
	union{
		u8 uuid[16];	
		ios_prov_t ios_prov; 
		ios_nw_t ios_nw;
		
		struct{
			u16 cid;
			#if AIS_ENABLE
			u8 vid;
			#endif
			u8 msg_type;
			#if AIS_ENABLE
			u8 mac[2];
			#endif
			u8 data[1];
		};
	};
}ios_app_data_t;

typedef struct{
	u8 random_a[8];
	u8 random_b[8];
}du_random_t;

typedef struct{
	u8 net_app_idx[3];		// can not set 24bit var IN vc
	u8 appkey[16];
}du_appkey_t;

typedef struct{
	u8 len;
	u8 ad_type;
	u16 cid;
	u8 data_type;
	u8 mac[2];
	u8 device_confirm[16];
	u32 randomC;
}du_rancomc_t;

typedef struct{
	u8 mac[2];
	u8 net_key[16];
	u16 unicast_addr;
	u8 iv_index[2];// big endian
	u8 flag;
}du_prov_data_t;

extern int mesh_rsp2_app_msg_id;
extern _align_4_ genie_nw_cache_t genie_nw_cache;
extern _align_4_ mesh_bear_rsp2_app_t bear_rsp2_app;

extern u8 du_ota_reboot_flag;
void du_ota_set_reboot_flag(void);
void du_ota_clr_reboot_flag(void);
u16  du_ota_get_reboot_flag(void);
void du_ota_suc_reboot(void);

void du_ota_set_flag(u8 value);
u8   du_ota_get_flag(void);

int genie_manu_nw_package(genie_nw_cache_t *p);
int app_event_handler_ultra_prov(u8 *p_payload);
void mesh_du_ultra_prov_loop();

#endif
