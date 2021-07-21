#ifndef __USER_DU_H
#define __USER_DU_H
#include "proj/tl_common.h"
#include "proj_lib/ble/ll/ll.h"
#include "proj_lib/ble/blt_config.h"
#include "vendor/common/user_config.h"

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
	u32 seg_size;// supose the segbuf is 244
	u32 buf_size;
	u32 buf_idx;
	u32 image_size;
	u32 image_offset;
	u32 crc;
	u32 ota_suc;	
}du_ota_str;
extern du_ota_str *p_ota;
void test_du_sha256_cal();
int	du_ctl_Write (void *p);
int du_fw_proc(void *p);
u8 du_adv_proc(rf_packet_adv_t * p);
int du_vd_event_set(u8*p_buf,u8 len,u16 dst);
//int du_vd_temp_event_send(u16 op,u16 val,u16 dst);
void du_loop_proc();
void du_vd_send_loop_proc();
void du_ui_proc_init();
void du_ui_proc_init_deep();
void du_bind_end_proc(u16 adr);
void du_bls_ll_setAdvEnable(int adv_enable);
void du_adv_enable_proc(void);
void du_prov_bind_check();
void du_time_req_start_proc();

void update_du_busy_s(u8 delay_s);

void du_set_gateway_adr(u16 adr);
u16  du_get_gateway_adr(void);
void du_write_gateway_adr(u16 adr);
u16  du_read_gateway_adr(void);
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

extern u8 du_ota_reboot_flag;
void du_ota_set_reboot_flag(void);
void du_ota_clr_reboot_flag(void);
u16  du_ota_get_reboot_flag(void);
void du_ota_suc_reboot(void);

void du_ota_set_flag(u8 value);
u8   du_ota_get_flag(void);


#endif
