/********************************************************************************************************
 * @file	user_du.c
 *
 * @brief	This is the source file for BLE SDK
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

#include "user_ali.h"
#include "user_du.h"
#include "app_health.h"
#include "proj_lib/sig_mesh/app_mesh.h"
#include "proj_lib/mesh_crypto/sha256_telink.h"
#include "vendor_model.h"
#include "fast_provision_model.h"
#include "proj_lib/ble/service/ble_ll_ota.h"
#include "proj_lib/mesh_crypto/aes_cbc.h"
#include "vendor/common/mi_api/telink_sdk_mible_api.h"
#include "vendor/common/mesh_ota.h"
#include "blt_soft_timer.h"
#include "vendor_model.h"
#define SHORT_LOACL_NAME  "DUMESH"
const u8 du_local_name[6]=SHORT_LOACL_NAME;
#define DU_MAGIC_CODE 0x44496F54
#define MAX_DU_BUF_SIZE	240
#define MAX_DU_BUF_CNT	16
#define MAX_DU_OTA_BUF	(MAX_DU_BUF_SIZE*MAX_DU_BUF_CNT)
_attribute_no_retention_bss_  u8  du_ota_buf[MAX_DU_OTA_BUF];
#if DU_ENABLE
#if DU_ULTRA_PROV_EN
u32 RandomC = 0;
u32 ultra_prov_tick;
#if DU_APP_ADV_CONTROL_EN
int mesh_rsp2_app_msg_id;
mesh_bear_rsp2_app_t bear_rsp2_app; // tx
#endif
genie_nw_cache_t genie_nw_cache; // rx
#endif

int convert_num2_char(u8 *p_in, u32 idx, u32 len, char *p_out)
{
	for(int i=0;i<len;i++){
		p_out[idx++] = num2char [(p_in[i]>>4) & 15];
		p_out[idx++] = num2char [p_in[i] & 15];
	}
	return idx;
}

void du_create_input_string(char *p_input,u16 rand,u32 pid,u8 *p_mac,u8 *p_secret)
{
	u8 idx =0;
	u8 p_rand[2];
	u8 p_pid[4];
	u8 mac[6];
	swap16(p_rand,(u8*)(&rand));
	swap32(p_pid,(u8*)&pid);
	swap48(mac,p_mac);
	for(int i=0;i<2;i++){
		p_input[idx++] = num2char [(p_rand[i]>>4) & 15];
		p_input[idx++] = num2char [p_rand[i] & 15];
	}
	p_input[idx++]=',';
	for(int i=0;i<4;i++){
		p_input[idx++] = num2char [(p_pid[i]>>4) & 15];
		p_input[idx++] = num2char [p_pid[i] & 15];
	}
	p_input[idx++]=',';
	for(int i=0;i<6;i++){
		p_input[idx++] = num2char [(mac[i]>>4) & 15];
		p_input[idx++] = num2char [mac[i] & 15];
	}
	p_input[idx++]=',';
	for(int i=0;i<16;i++){// need to change to string .
		p_input[idx++] = num2char [(p_secret[i]>>4) & 15];
		p_input[idx++] = num2char [p_secret[i] & 15];
	}
}
uint32_t du_soft_crc32(uint8_t const * p_data, uint32_t size, uint32_t init_crc)
{
    uint32_t crc;

    crc = (init_crc == 0) ? 0xFFFFFFFF : ~(init_crc);
    for (uint32_t i = 0; i < size; i++)
    {
        crc = crc ^ p_data[i];
        for (uint32_t j = 8; j > 0; j--)
        {
            crc = (crc >> 1) ^ (0xEDB88320U & ((crc & 1) ? 0xFFFFFFFF : 0));
        }
    }
    return ~crc;
}

du_ota_str du_ota;
du_ota_str *p_ota = &du_ota;


void du_sha256_out(char *p_input,u16 rand,u32 pid,u8 *p_mac,u8 *p_secret)
{
	u8 du_sha_in[59];
	u8 du_sha_out[32];
	du_create_input_string((char *)du_sha_in,rand,pid,p_mac,p_secret);
	mbedtls_sha256(du_sha_in,sizeof(du_sha_in),du_sha_out,0);
	memcpy(p_input,du_sha_out,16);
}

void du_adv_conn_init(rf_packet_adv_t * p)
{
	p->header.type = LL_TYPE_ADV_IND;
	memcpy(p->advA,tbl_mac,6);
	p->rf_len = 6 + sizeof(du_ota_adv_str);
	p->dma_len = p->rf_len + 2; 
	du_ota_adv_str *p_ota_adv = (du_ota_adv_str *)(p->data);
	p_ota_adv->len1 = 2;
	p_ota_adv->ad_type1 = 1;
	p_ota_adv->ad_dat1 = 6;
	p_ota_adv->len2 = 3;
	p_ota_adv->ad_type2 = 2;
	p_ota_adv->ad_dat2 = 0xffb0;
	p_ota_adv->len3 = 7;
	p_ota_adv->ad_type3 = 8;
	memcpy(p_ota_adv->ad_dat3,du_local_name,6);
	p_ota_adv->len4 = 0x0c;
	p_ota_adv->ad_type4= 0xff;
	p_ota_adv->cid = VENDOR_ID;
	p_ota_adv->dev_type = 3;// mesh type
	p_ota_adv->ver = DU_FW_VER;
	memcpy(p_ota_adv->ios_mac,tbl_mac,6);
}

void du_adv_ota_end_init(rf_packet_adv_t * p)
{
	p->header.type = LL_TYPE_ADV_NONCONN_IND;
	memcpy(p->advA,tbl_mac,6);
	p->rf_len = 6 + sizeof(du_ota_end_adv_str);
	p->dma_len = p->rf_len + 2; 
	du_ota_end_adv_str *p_ota_end = (du_ota_end_adv_str *)(p->data);
	p_ota_end->len = 12;
	p_ota_end->ad_type = 0xff;
	p_ota_end->cid = VENDOR_ID;
	p_ota_end->dev_type = 3;// mesh type
	p_ota_end->ver = DU_FW_VER;
	memcpy(p_ota_end->ios_mac,tbl_mac,6);
}

u8 du_adv_send_en =1;

void du_adv_set_send_flag(u8 en)
{
	du_adv_send_en = en;
}

u8 du_ota_reboot_flag = 0;
u8 du_adv_proc(rf_packet_adv_t * p)
{
	if(du_ota_reboot_flag){
		du_adv_ota_end_init(p);	
	}else{
		#if (LPN_CONTROL_EN || DU_ULTRA_PROV_EN)
		if(provision_mag.gatt_mode == GATT_PROVISION_MODE){
			set_adv_provision(p);
		}else{
			set_adv_proxy(p);
		}
		#else
		du_adv_conn_init(p);
		#endif
	}
	if(du_adv_send_en){
		return 1;
	}else{
		return 0;
	}
}

extern int bls_du_notify_rsp(u8*p_buf,int len);

int bls_du_cmd_rsp(u8 op,u8 *p_buf,int len)
{
    #define DU_BUF_MAX_LEN 0x40
	u8 buf[DU_BUF_MAX_LEN];
	if(len>(DU_BUF_MAX_LEN-1)){
		return 0;
	}
	buf[0]=op;
	memcpy(buf+1,p_buf,len);
	return bls_du_notify_rsp(buf,len+1);
}

void cert_rand_rsp_auth(u8 *pbuf)
{
	u16 rand;
	u8 auth[16];
	rand = pbuf[0]|(pbuf[1]<<8);
	du_sha256_out((char *)auth,rand,con_product_id,tbl_mac,con_sec_data);
	bls_du_cmd_rsp(DU_CERT_IOT_DEV_RSP,auth,sizeof(auth));
}


void read_dev_rsp_verison(u8 *pbuf)
{
	du_dev_version_rsp_str rsp;
	rsp.dev_type = 3;// mesh type
	rsp.ver      = DU_FW_VER;
	bls_du_cmd_rsp(DU_READ_DEV_VERSION_RSP,(u8*)&rsp,sizeof(rsp));
}

void resume_break_proc(u8 *pbuf)
{
	
	du_break_point_cmd_str *p_cmd = (du_break_point_cmd_str *)pbuf;
	du_break_point_rsp_str rsp;
	if(p_cmd->rand !=0 &&!memcmp(pbuf,(u8*)&(p_ota->rand_code),4)){//is the same 
		rsp.break_point = p_ota->break_point;
		p_ota->rand_code = 0;
	}else{
		rsp.break_point = 0;
	// not want to store the rand info ,and the ota time is short ,we change the rand every time ,not allow retrans 
		rsp.new_rand = rand();
		p_ota->rand_code = rsp.new_rand;
	}
	bls_du_cmd_rsp(DU_TRANS_RESUME_BREAK_RSP,(u8*)&rsp,sizeof(rsp));
}



void exchange_chk_size_proc(u8 *pbuf)
{
	//du_exchange_str* p_cmd = (du_exchange_str*)pbuf;
	du_exchange_str rsp;
	rsp.seg_size = p_ota->seg_size = MAX_DU_BUF_SIZE;
	rsp.buf_size = p_ota->buf_size = MAX_DU_OTA_BUF;
	bls_du_cmd_rsp(DU_EXCHANGE_BUF_CHK_SIZE_RSP,(u8*)&rsp,sizeof(rsp));

}

static u8 du_ota_flag = 0;
static u32 tick_du_ota_monitor = 0;
void du_ota_set_flag(u8 value)
{
	if(value){
		du_ota_flag = 1;
	}else{
		du_ota_flag = 0;
	}
}

u8   du_ota_get_flag(void)
{
	return du_ota_flag;
}

void du_ota_update_monitor_tick(void)
{
	tick_du_ota_monitor = clock_time()|1;
}

#define DU_OTA_TIMEOUT_S  5
void du_ota_monitor_process(void)
{
	if(du_ota_flag&&clock_time_exceed(tick_du_ota_monitor,DU_OTA_TIMEOUT_S*1000*1000)){
		du_ota_set_flag(0);
		LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"ota timeout, reboot!",0);
		start_reboot();
	}
}


void start_ota_proc(u8*pbuf)
{
	du_start_ota_str *p_start = (du_start_ota_str *)pbuf;	
	du_start_ota_sts_str ota_sts;
	p_ota->image_size = p_start->image_size;
	p_ota->image_offset = p_start->offset;
	ota_sts.sts =1;
	#if DU_ENABLE
	clock_init(SYS_CLK_16M_Crystal);
	//blc_ll_setScanEnable (0, 0);
	LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"connect suc",0);
	#endif
	#if (ZBIT_FLASH_WRITE_TIME_LONG_WORKAROUND_EN)
		check_and_set_1p95v_to_zbit_flash();
	#endif
	du_ota_set_flag(1);
	#if LPN_CONTROL_EN
		#if LPN_FAST_OTA_EN  // it will not sleep
			bls_l2cap_requestConnParamUpdate (24, 30, 0, 500);
		#else // it will enter suspend mode 
			bls_l2cap_requestConnParamUpdate (48, 56, 0, 500);
		#endif
	#endif
	LOG_MSG_INFO(TL_LOG_NODE_SDK,(u8*)&ota_sts,sizeof(ota_sts),"start_ota_proc",0);
	bls_du_cmd_rsp(DU_START_OTA_RSP,(u8*)&ota_sts,sizeof(ota_sts));
}

typedef struct{
	u32 fw_len;
	u32 crc;
	u32 crc_total;
	u8 auth_type;
	u8 rfu[3];
}telink_fw_check;
telink_fw_check du_chk;
u32 soft_crc32_du_ota_flash(u32 addr, u32 len, u32 crc_init,u32 *out_crc_type1_blk)
{
    clock_switch_to_highest();
    u32 crc_type1_blk = 0;
    u8 buf[64];      // CRC_SIZE_UNIT
    u32 crc_total=0;
    while(len){
        u32 len_read = (len > sizeof(buf)) ? sizeof(buf) : len;
        mesh_ota_read_data(addr, len_read, buf);
        if(0 == addr){
            buf[8] = 0x4b;
            du_chk.auth_type= get_ota_check_type();
            if(FW_CHECK_AGTHM2 == du_chk.auth_type){
                crc_total = 0xffffffff;  // crc init
                if(out_crc_type1_blk){
                    *out_crc_type1_blk = crc_total;
                }
            }
        }
		if(out_crc_type1_blk){
		    if(FW_CHECK_AGTHM2 == du_chk.auth_type){
		        u32 crc_check_len = len_read;
		        *out_crc_type1_blk = get_blk_crc_tlk_type2(*out_crc_type1_blk, buf, crc_check_len);
		    }else{
        	    crc_type1_blk += get_blk_crc_tlk_type1(buf, len_read, addr);	// use to get total crc of total firmware.
        	}
		}
        
        len -= len_read;
        addr += len_read;
        #if MODULE_WATCHDOG_ENABLE
        wd_clear();
        #endif
    }
    if(out_crc_type1_blk){
        if(FW_CHECK_AGTHM2 != du_chk.auth_type){
		    *out_crc_type1_blk = crc_type1_blk;
		}
	}
    clock_switch_to_normal();
    return crc_init;
}

extern int mesh_flash_write_check(u32 adr, const u8 *in, u32 size);

void buffer_chk_cmd_proc(u8*pbuf)
{
	du_buf_chk_str *p_chk = (du_buf_chk_str *)pbuf;
	du_buf_chk_sts_str chk_sts;
	//when we receive all the segment buf part
	if(du_soft_crc32(du_ota_buf,p_ota->buf_idx,0) == p_chk->crc32){
		// if the image_offset is 0,we need to change the buf to 0xff,and after finish ,we need to change back to 0x4f
		if(p_ota->image_offset <=8){
			u32 pid = (du_ota_buf[5]<<24)|(du_ota_buf[4]<<16)|(du_ota_buf[3]<<8)|du_ota_buf[2];
			if(pid != con_product_id){
				LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"check pid err. device pid = %x ,ota file pid = %x",con_product_id,pid);
				start_reboot();
			}
			p_ota->crc = du_soft_crc32(du_ota_buf,p_ota->buf_idx,0);
			du_ota_buf[8]= 0xff;
		}else{
			p_ota->crc = du_soft_crc32(du_ota_buf,p_ota->buf_idx,p_ota->crc);
		}
		flash_write_page(ota_program_offset+p_ota->image_offset,p_ota->buf_idx,du_ota_buf);
		if(mesh_flash_write_check(ota_program_offset+p_ota->image_offset,du_ota_buf,p_ota->buf_idx)){
			LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"flash data check err,image %d,buf_idx %d, fw len is %d",p_ota->image_offset,p_ota->buf_idx,du_chk.fw_len);
			start_reboot();
		}
		
		if(p_ota->image_offset == 0){
			u8 err=0;
			memset(&du_chk,0,sizeof(du_chk));
			if(du_ota_buf[6]!= 0x5d){
				err = 1;
			}
			if(!(du_ota_buf[7]== FW_CHECK_AGTHM1 || du_ota_buf[7]== FW_CHECK_AGTHM2)){
				err = 2;
			}
			if(err){
				LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"telink ota chk err %x",err);
				start_reboot();
			}
			du_chk.fw_len = get_fw_len();
			if(du_chk.fw_len > 0x30000){
				LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"overflow err,ota file len is %x, larger than 0x30000!",du_chk.fw_len);
				start_reboot();
			}
		}
		if((p_ota->image_offset + p_ota->buf_idx)> du_chk.fw_len){
			LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"ota fw overflow ",0);
			start_reboot();
		}
		if(p_ota->image_offset + p_ota->buf_idx == du_chk.fw_len){
			// the last packet 
			LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"buffer check last packet",0);
			soft_crc32_du_ota_flash(p_ota->image_offset,p_ota->buf_idx-4,0,&(du_chk.crc));
		}else{
			soft_crc32_du_ota_flash(p_ota->image_offset,p_ota->buf_idx,0,&(du_chk.crc));
		}
		if(FW_CHECK_AGTHM1 == du_chk.auth_type){
			du_chk.crc_total += du_chk.crc;
		}else if(FW_CHECK_AGTHM2 == du_chk.auth_type){
			du_chk.crc_total = du_chk.crc;
		}
		LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"image %d,buf_idx %d, fw len is %d",p_ota->image_offset,p_ota->buf_idx,du_chk.fw_len);
		p_ota->image_offset+=p_ota->buf_idx;
		p_ota->buf_idx =0 ;// clear all the fw part
		chk_sts.sts = 1;
		chk_sts.adjust_offset = 0;
		LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"buffer check suc %x",p_ota->image_offset);
	}else{
		p_ota->buf_idx =0 ;// clear all the fw part
		chk_sts.sts = 0;
		chk_sts.adjust_offset = p_ota->image_offset;
		LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"buffer check fail %x",p_ota->image_offset);
	}
	
	bls_du_cmd_rsp(DU_BUFFER_CHK_RSP,(u8*)&chk_sts,sizeof(chk_sts));
}

int use_flash_to_get_crc32()
{
	// get the first 8 bytes
	u8 *p_buf = du_ota_buf;
	u32 buf_size = sizeof(du_ota_buf);
	u32 img_size = p_ota->image_size;
	u32 idx =0;
	u32 crc=0;
	flash_read_page(ota_program_offset,buf_size,p_buf);
	idx+=buf_size;
	img_size -= buf_size;
	// change the 8th byte to 0x4b
	p_buf[8]=0x4b;
	crc =du_soft_crc32(du_ota_buf,buf_size,0);
	while(img_size){
		if(img_size >= buf_size){
			flash_read_page(ota_program_offset+idx,buf_size,p_buf);
			crc =du_soft_crc32(du_ota_buf,buf_size,crc);
			img_size-=buf_size;
			idx+=buf_size;
		}else{
			flash_read_page(ota_program_offset+idx,img_size,p_buf);
			crc =du_soft_crc32(du_ota_buf,img_size,crc);
			img_size =0;
		}
#if (MODULE_WATCHDOG_ENABLE)
		wd_clear(); //clear watch dog
#endif
	}
	return crc;
}

void whole_img_chk_proc(u8*pbuf)
{
	du_whole_img_chk_sts_str chk;
	// check all the firmware by the flash content
	chk.sts =1;
	u32 crc_flash = use_flash_to_get_crc32();
	if(crc_flash != p_ota->crc){
		LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"whole_img_chk_proc crc err %x",p_ota->image_offset);
		start_reboot();
	}
	// check the information by telink .
	u32 crc_telink = get_total_crc_type1_new_fw();
	if(crc_telink != du_chk.crc_total){
		LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"telink fw check fail ",0);
		start_reboot();
	}
	LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"whole_img_chk_proc crc suc %x",p_ota->image_offset);
	chk.crc = p_ota->crc;
	bls_du_cmd_rsp(DU_WHOLE_IMG_CHK_RSP,(u8*)&chk,sizeof(chk));
}

void du_ota_set_reboot_flag(void)
{
	u16 ota_reboot_flag = 1;
	flash_erase_sector(DU_OTA_REBOOT_ADR);
	flash_write_page(DU_OTA_REBOOT_ADR,sizeof(ota_reboot_flag),(u8 *)(&ota_reboot_flag));
}

void du_ota_clr_reboot_flag(void)
{
	flash_erase_sector(DU_OTA_REBOOT_ADR);
}


u16 du_ota_get_reboot_flag(void)
{
	u16 ota_reboot_flag = 0;
	flash_read_page(DU_OTA_REBOOT_ADR,sizeof(ota_reboot_flag),(u8 *)(&ota_reboot_flag));
	if(ota_reboot_flag == 1){
		return 1;
	}
	return 0;
}
void du_ota_suc_reboot(void)
{
	ota_set_flag();
	du_ota_set_reboot_flag();
	start_reboot();

}

static u32 du_loop_tick =0;

static u32 du_bind_success_flag =0;



du_store_str du_store;
#define DU_PROV_EN_FLAG	0xa55a
void du_set_gateway_adr(u16 adr)
{
	du_store.gw_adr = adr;
}

u16 du_get_gateway_adr(void)
{
	return du_store.gw_adr;
}

u16 du_set_gateway_roll_flag()
{
	du_store.flag = DU_PROV_EN_FLAG;	
	return du_store.flag;
}
void du_store_proc_init()
{
	flash_read_page(DU_STORE_ADR,sizeof(du_store),(u8 *)(&du_store));
	if(du_store.gw_adr == 0xffff && du_store.flag == 0xffff  ){
		if(is_unicast_adr(du_store.adr)){
			// transfer to new store mode .
			du_set_gateway_adr(du_store.adr);
			#if DU_BIND_CHECK_EN
			du_set_gateway_roll_flag();
			#endif
			flash_write_page(DU_STORE_ADR, sizeof(du_store),(u8 *)(&du_store));
			return ;
		}
	}	
}

void du_enable_gateway_adr(u8 check_bind)
{
	flash_erase_sector(DU_STORE_ADR);
	if(check_bind){
		du_set_gateway_roll_flag();
	}
	flash_write_page(DU_STORE_ADR,sizeof(du_store),(u8 *)(&du_store));		
}

u8  du_get_bind_flag(void)
{
	if(is_unicast_adr(du_store.gw_adr)&&du_store.flag == DU_PROV_EN_FLAG){
		return 1;
	}
	return 0;
}

/*
it is used to contrl the mcu to be low power mode when du provision have complete.
the last cmd is different by every pid ,it should be check by yourself.
for example : the pid is 0x006b2d1d,the last cmd is HEARTBEAT_PUB_SET.
*/
void du_bind_end_proc(u16 gw_adr,u32 time_s)
{
	update_du_busy_s(time_s);	
	du_bind_success_flag = 1;
	du_set_gateway_adr(gw_adr);
	LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"du bind end callback ",0);
}


u32 du_busy_tick =0;
u32 du_busy_s =0;

void update_du_busy_s(u8 delay_s)
{
	du_busy_tick = clock_time()|1;
	du_busy_s = delay_s;
}

void du_busy_proc()
{
	if(
		#if DU_LPN_EN
		(mi_mesh_get_state()||blt_state == BLS_LINK_STATE_CONN)&&
		#endif
		du_busy_tick&& clock_time_exceed(du_busy_tick,du_busy_s*1000*1000)
	){
		du_busy_tick = 0;

		#if DU_LPN_EN
		mi_mesh_state_set(0);
			#if !LPN_CONTROL_EN
		beacon_str_disable();
		if(is_provision_success()){
			du_adv_set_send_flag(1);
		}else{
			du_adv_set_send_flag(0);
		}
			#endif
		#endif

		if(du_ota_reboot_flag){
			du_ota_reboot_flag = 0;
			du_ota_clr_reboot_flag();
		}

		if(du_bind_success_flag){
			du_bind_success_flag = 0;
			if(is_unicast_adr(du_get_gateway_adr())){
				du_enable_gateway_adr(DU_BIND_CHECK_EN);
				LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"store gateway adr",0);
			}
			du_loop_tick = clock_time()-60*CLOCK_SYS_CLOCK_1S;
		}
					
		#if DU_BIND_CHECK_EN
		du_prov_bind_check();
		#endif

		
		LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"back to normal mode",0);
	}
}


#if DU_LPN_EN
// to control the adv send part .
void du_bls_ll_setAdvEnable(int adv_enable)
{
	// not allow to changes the adv state in the lpn control mode .
	#if !LPN_CONTROL_EN
	bls_ll_setAdvEnable(adv_enable); 

	if(blt_state == BLS_LINK_STATE_IDLE || blt_state == BLS_LINK_STATE_ADV )
	{

		if(adv_enable)  //enable
		{
		}
		else  //disable
		{
			if(blt_state == BLS_LINK_STATE_IDLE)  //adv/conn_slave -> idle
			{
				blt_state = BLS_LINK_STATE_ADV;
				blta.adv_begin_tick = clock_time();  //update
			}
		}
	}
	#endif
}

typedef struct{
	u8 sw;
	u8 sw_last;
	u8 press_down;
	u8 press_on;
	u32 on_tick;
	u32 down_tick;
}sw_proc_t;
sw_proc_t sw_but;


void du_key_board_long_press_detect()
{
	sw_but.sw = !gpio_read(SW1_GPIO);
	if(!(sw_but.sw_last)&&sw_but.sw){// press on 
	   	sw_but.press_on++;
		sw_but.on_tick = clock_time();
		sw_but.down_tick = clock_time();
		LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"press on ",0);
	}
	if(sw_but.sw_last && !sw_but.sw){// press down
		sw_but.press_down++;
		sw_but.down_tick = clock_time();
		LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"press down ",0);
	}
	sw_but.sw_last = sw_but.sw;
	if(sw_but.press_on && clock_time_exceed(sw_but.on_tick,5*1000*1000)){
		// long press trigger to enter provision mode 
		sw_but.press_on = 0;
		sw_but.on_tick = clock_time();
		LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"long_press",0);
		mi_mesh_state_set(1);
		update_du_busy_s(60);
		beacon_str_init();
		#if !LPN_CONTROL_EN
		if(is_provision_success()){
			du_adv_set_send_flag(1);
		}else{
			du_adv_set_send_flag(0);
		}
		#endif
	}
	if(sw_but.press_down && clock_time_exceed(sw_but.down_tick,1*1000*1000)){
		
		if(sw_but.press_on == 1){
			//single press
			du_time_req_start_proc();
			LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"single_press",0);
		}else if (sw_but.press_on == 2){
			// twice press
			LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"twice_press",0);
			kick_out(1);
		}
		// clear all the sts ,press end 
		memset(&sw_but,0,sizeof(sw_but));
	}
}



void du_key_board_proc()
{
	static u32 du_log_tick =0;
	if(clock_time_exceed(du_log_tick,40*1000))//40ms print every time 
	{
		du_log_tick = clock_time();
		du_key_board_long_press_detect();
	}
}
#endif

void du_normal_send_demo_event()
{
	u8 retry_times =4;
	u16 du_time_tid = 0;
	vd_du_event_str htp_str;
	htp_para* p_htp = 0;
	du_time_tid++;
	htp_str.tid = du_time_tid;
	htp_str.op = VD_DU_THP_CMD;
	p_htp = (htp_para*)htp_str.val;
	
	p_htp->humi = 20;
	p_htp->temp = 27315+2000;
	p_htp->power= 100;
	
	while(retry_times--){
		mesh_tx_cmd2normal(VD_LPN_REPROT,(u8*)&htp_str,sizeof(vd_du_event_str),ele_adr_primary,du_get_gateway_adr(),0);
	}
	LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"du_vd_htp_event_proc",0);

}


void du_loop_proc()
{
	du_busy_proc();
	du_ota_monitor_process();
	#if DU_ULTRA_PROV_EN
	mesh_du_ultra_prov_loop();
	#else
	#if DU_LPN_EN
	du_key_board_proc();
	// demo send cmd.
	if(is_provision_success()&&
		!mi_mesh_get_state()&&
		clock_time_exceed(du_loop_tick,60*1000*1000)){
		du_loop_tick = clock_time();
		#if !LPN_CONTROL_EN
		du_vd_send_loop_proc();
		#endif
	}
		
	#else
	
	if(is_provision_success()&&
		clock_time_exceed(du_loop_tick,60*1000*1000)){
		du_loop_tick = clock_time();
		du_normal_send_demo_event();
	}
	#endif
	#endif
}

void du_lpn_suspend_enter()
{
	bls_pm_setWakeupSource(PM_WAKEUP_PAD);  // GPIO_WAKEUP_MODULE needs to be wakened
}
void du_ui_proc_init()
{
	du_store_proc_init();// transfer the mode 
#if DU_BIND_CHECK_EN
	du_prov_bind_check();
#endif

	gpio_set_wakeup (SW1_GPIO, Level_Low, 1);
	cpu_set_gpio_wakeup (SW1_GPIO, Level_Low,1);  //drive pin pad high wakeup deepsleep
	bls_pm_setWakeupSource(PM_WAKEUP_PAD);  // GPIO_WAKEUP_MODULE needs to be wakened
	bls_app_registerEventCallback (BLT_EV_FLAG_SUSPEND_ENTER, (blt_event_callback_t)&du_lpn_suspend_enter);
	if(du_ota_get_reboot_flag()){
		
		#if DU_LPN_EN
		mi_mesh_state_set(1);
		#endif
		
		update_du_busy_s(4);
		
		du_ota_reboot_flag = 1;

		//LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"ota reboot ,send version adv!",0);
		
	}else{
		#if DU_LPN_EN
		du_bls_ll_setAdvEnable(0);
		#endif
	}

}

void du_ui_proc_init_deep()
{
	gpio_set_wakeup (SW1_GPIO, Level_Low, 1);
	cpu_set_gpio_wakeup (SW1_GPIO, Level_Low,1);  //drive pin pad high wakeup deepsleep
	bls_pm_setWakeupSource(PM_WAKEUP_PAD);	// GPIO_WAKEUP_MODULE needs to be wakened
}

void magic_code_chk_proc(u8* pbuf)
{
	du_magic_code_str *p_magic = (du_magic_code_str *)pbuf;
	du_magic_code_rsp_str rsp;
	du_ota_set_flag(0);	
	if(p_magic->magic == DU_MAGIC_CODE){
		rsp.sts =1;
		//we need to write back the flag part
		p_ota->ota_suc =1;
		//later we will need to reboot
	}else{
		// magic number error will reboot
		start_reboot();
		rsp.sts =0;
	}
	bls_du_cmd_rsp(DU_OVERWRITE_IMG_RSP,(u8*)&rsp,sizeof(rsp));
	if(p_ota->ota_suc){
		// need to terminate the connection 
		bls_ll_terminateConnection (0x13);
	}
}

int	du_ctl_Write (void *p)
{
	rf_packet_att_write_t *pw = (rf_packet_att_write_t *)(p);
	du_cmd_str *p_cmd = (du_cmd_str *)(&pw->value);
	du_ota_update_monitor_tick();
	switch(p_cmd->opcode){
		case DU_CERT_IOT_DEV_CMD:
			cert_rand_rsp_auth(p_cmd->buf);
			break;
		case DU_READ_DEV_VERSION_CMD:
   			read_dev_rsp_verison(p_cmd->buf);
   			break;
		case DU_TRANS_RESUME_BREAK_CMD:
			resume_break_proc(p_cmd->buf);
			break;
		case DU_EXCHANGE_BUF_CHK_SIZE_CMD:
			exchange_chk_size_proc(p_cmd->buf);
			break;
		case DU_START_OTA_CMD:
			start_ota_proc(p_cmd->buf);
			break;
		case DU_BUFFER_CHK_CMD:
			buffer_chk_cmd_proc(p_cmd->buf);
			break;
		case DU_WHOLE_IMG_CHK_CMD:
			whole_img_chk_proc(p_cmd->buf);
			break;
		case DU_OVERWRITE_IMG_CMD:
			magic_code_chk_proc(p_cmd->buf);
			break;
		default:
			break;
	}
	return 1;
}

_attribute_ram_code_ int du_fw_proc(void *p)
{
	rf_packet_att_data_t *p_w = (rf_packet_att_data_t*)p;
	
	u32 len = p_w->l2cap-3;
	du_ota_update_monitor_tick();
	//LOG_MSG_INFO(TL_LOG_NODE_SDK,p_w->dat,16,"rcv fw proc ",0);
	if(p_ota->buf_idx+len >MAX_DU_OTA_BUF) {
		return 0;
	}
	memcpy(du_ota_buf+p_ota->buf_idx,p_w->dat,len);
	p_ota->buf_idx +=len;

	return 1;
}




/*
it is used to check that the du provision is not complete.
it can be used by setting the macro of DU_BIND_CHECK_EN.
*/

void du_prov_bind_check()
{
	//provision suc ,but provision fail.
	if(is_provision_success() && (!du_get_bind_flag())){
		
		LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"provision is not complete , bind failed!",0);
		
		// need to rool back .
		kick_out(0);
	}
}


int du_vd_event_send(u8*p_buf,u8 len,u16 dst)
{
	return mesh_tx_cmd2normal(VD_LPN_REPROT, (u8 *)p_buf, len,ele_adr_primary,dst,0);
}

u8 du_time_tid =0;

void du_time_req_start_proc()
{
	u8 retry_times =4;
	du_time_req time_req;
	du_time_tid++;
	time_req.tid = du_time_tid;
	time_req.op = VD_DU_TIME;
	while(retry_times--){
		mesh_tx_cmd2normal(VD_TIME_REQ,(u8*)&time_req,sizeof(time_req),ele_adr_primary,du_get_gateway_adr(),0);
	}

	#if DU_LPN_EN
	mi_mesh_state_set(1);
	update_du_busy_s(2);// the time proc will last for about 2s 
	#endif
}

vd_du_event_t vd_du;

int soft_timer_proc_event()
{
	static u32 send_total_cnt =0;
	int err = -1;
	if(send_total_cnt >= 8){
		send_total_cnt = 0;
		if(!mi_mesh_get_state()){
   			du_bls_ll_setAdvEnable(0);
  		}
  		return -1;
 	}
	if(send_total_cnt == 0){
  		err = du_vd_event_send((u8*)&(vd_du.op_str),vd_du.op_str_len,vd_du.gateway_adr);
  		if(err == 0){
   			send_total_cnt++;
  		}
 	}
	if(send_total_cnt < 7){
		err = du_vd_event_send((u8*)&(vd_du.op_str),vd_du.op_str_len,vd_du.gateway_adr);
		if(err == 0){
			send_total_cnt++;
		}
 	}else{
  		send_total_cnt++;
 	}
 	return 0;
}

int du_vd_temp_event_send(vd_du_event_t* event)
{
	static u8 du_tid =0;
	du_tid++;
	memcpy((u8*)&vd_du,event,sizeof(vd_du_event_t));
	vd_du.op_str.tid = du_tid;
	vd_du.op_str_len += 3;
	if(vd_du.op_str_len > 8){
		return 0;
	}
	// add softtimer to send the cmd part 
	blt_soft_timer_add((blt_timer_callback_t)&soft_timer_proc_event, 90*1000);
	return 1;
}


void du_vd_send_loop_proc()
{
	if(blt_state != BLS_LINK_STATE_CONN && (!mi_mesh_get_state())&&is_unicast_adr(du_get_gateway_adr())){

		vd_du_event_t du_event;

		du_event.gateway_adr = du_get_gateway_adr();
		du_event.op_str.op = VD_DU_THP_CMD;
	
	    du_event.op_str_len = 0;
	    htp_para* p_htp = (htp_para*)du_event.op_str.val;
		
		p_htp->temp = 27315+2000;
		du_event.op_str_len += 2;

		p_htp->humi = 20;
		du_event.op_str_len += 2;
		
		p_htp->power = 100;
		du_event.op_str_len += 1;
				
		//(data/ 100 - 273.15),20 degree is 27315+2000
		du_bls_ll_setAdvEnable(1);// enable the sending part 
		du_vd_temp_event_send(&du_event);
	}
}

#if DU_ULTRA_PROV_EN
#define dev_key_pad		pro_random
#define randomc_pad		pro_comfirm
void mesh_du_ultra_prov_loop()
{
	if(genie_nw_cache.tick && clock_time_exceed(genie_nw_cache.tick, 2*1000*1000)){//genie APP rx timeout 
		genie_nw_cache.tick = 0;
	}

	if(ultra_prov_tick && clock_time_exceed(ultra_prov_tick, 10*1000*1000)){ // provision timeout
		ultra_prov_tick = 0;
		prov_para.provison_rcv_state = LINK_UNPROVISION_STATE;
	}
}

void create_sha256_input_string_wihout_comma(char *p_input,u8 *pid,u8 *p_mac,u8 *p_secret)
{
	u8 idx =0;
	u8 con_product_id_rev[4];
	swap32(con_product_id_rev,pid);
	for(int i=0;i<4;i++){
		p_input[idx++] = num2char [(con_product_id_rev[i]>>4) & 15];
		p_input[idx++] = num2char [con_product_id_rev[i] & 15];
	}

	for(int i=0;i<6;i++){
		p_input[idx++] = num2char [(p_mac[i]>>4) & 15];
		p_input[idx++] = num2char [p_mac[i] & 15];
	}

	for(int i=0;i<16;i++){// need to change to string .
		p_input[idx++] = num2char [(p_secret[i]>>4) & 15];
		p_input[idx++] = num2char [p_secret[i] & 15];
	}
}

void mesh_du_ble_adv(u8 type, u8 *par, int par_len)
{
	mesh_cmd_bear_t cmd_bear_tmp;
	cmd_bear_tmp.tx_head.par_type =  BEAR_TX_PAR_TYPE_REMAINING_TIMES;
	cmd_bear_tmp.tx_head.val[0] = 200;
	cmd_bear_tmp.trans_par_val = TRANSMIT_DEF_PAR;
	genie_manu_factor_data_t *p_manu_factor = (genie_manu_factor_data_t *) &cmd_bear_tmp.len;
	p_manu_factor->len_flag = (par_len+7);
	p_manu_factor->flag_type = GAP_ADTYPE_FLAGS;
	p_manu_factor->flags = 0x06;
	
	du_manu_data_t *p_manu = (du_manu_data_t *) &p_manu_factor->manu_len;
	p_manu->len = par_len + 4;
	p_manu->ad_type = GAP_ADTYPE_MANUFACTURER_SPECIFIC;
	p_manu->cid = VENDOR_ID;
	p_manu->data_type = type;
	memcpy(p_manu->data, par, par_len);

	my_fifo_reset(&mesh_adv_cmd_fifo);
	mesh_tx_cmd_add_packet((u8 *)(&cmd_bear_tmp));
}

int mesh_du_rcv_random(u8 *p_payload)
{
	u8 S1[16];
	char shar256_in[96];
	u8 shar256_out[32];
	du_manu_data_t *p_manu_data = (du_manu_data_t *)p_payload;
	du_random_t *p_du_random = (du_random_t *)p_manu_data->data;

	if(p_manu_data->len >= 22){
		if(memcmp(p_manu_data->data, tbl_mac, 2)){
			return -1;
		}
		p_du_random = (du_random_t *)(p_manu_data->data+2);
	}
	#if RANDOM_OLD_SAVE_EN
	memcpy(du_random_a, p_du_random->random_a+6, 2);
	memcpy(du_random_b, p_du_random->random_b+6, 2);
	foreach(i, RANDOM_CACHE_NUM){
		if(!memcmp(du_random_a, random_old.random_a[i], sizeof(random_old.random_a[i])) && !memcmp(du_random_b, random_old.random_b[i], sizeof(random_old.random_b[i]))){
			return -1;
		}
	}
	#endif

	#if DU_SAMPLE_DATA_TEST_EN
	RandomC = 0x5a36C781;
	#else
	RandomC = myrand();
	#endif
// RandomC	
	LOG_MSG_LIB(TL_LOG_NODE_SDK, randomc_pad, 23, "randomc_pad pre:", 0);
	memcpy(randomc_pad+19, &RandomC, sizeof(RandomC));
	endianness_swap_u32(randomc_pad+19);

	#if 1//DU_SAMPLE_DATA_TEST_EN
	LOG_MSG_LIB(TL_LOG_NODE_SDK, p_du_random->random_a, 8, "RandomA:", 0);
	LOG_MSG_LIB(TL_LOG_NODE_SDK, p_du_random->random_b, 8, "RandomB:", 0);
	#endif
// S1	
	convert_num2_char(p_du_random->random_a, 0, 8, shar256_in);
	convert_num2_char(p_du_random->random_b, 0, 8, shar256_in+16);
	u8 mac[6];
	swap48(mac, tbl_mac);
	create_sha256_input_string_wihout_comma(shar256_in+32, (u8 *)&con_product_id, mac, con_sec_data);
	mbedtls_sha256((u8 *)shar256_in, 84, shar256_out, 0);
	memcpy(S1, shar256_out, 16);
	#if DU_SAMPLE_DATA_TEST_EN
	LOG_USER_MSG_INFO( S1, 16, "S1:", 0);
	#endif
// K1	
	convert_num2_char(p_du_random->random_b, 0, 8, shar256_in);
	create_sha256_input_string_wihout_comma(shar256_in+16, (u8 *)&con_product_id, mac, con_sec_data);
	mbedtls_sha256((u8 *)shar256_in, 68, shar256_out, 0);
	#if DU_SAMPLE_DATA_TEST_EN
	LOG_USER_MSG_INFO( shar256_out, 16, "K1:", 0);
	#endif
// D1
	tn_aes_cmac(shar256_out, S1, 16, dev_comfirm);
	#if DU_SAMPLE_DATA_TEST_EN
	LOG_USER_MSG_INFO( dev_comfirm, 16, "D1:", 0);
	#endif
// device key
	convert_num2_char(p_du_random->random_a, 0, 8, shar256_in);
	#if 1
	// have copyed in K1, no need to copy again to save code size.
	#else
	create_sha256_input_string(shar256_in+16, (u8 *)&p_three_par->pid, p_three_par->mac, p_three_par->sec_data);
	#endif
	mbedtls_sha256((u8 *)shar256_in, 68, shar256_out, 0);
	memcpy(mesh_key.dev_key, shar256_out, 16);
	memcpy(dev_key_pad+7, shar256_out, 16);
	LOG_MSG_LIB(TL_LOG_NODE_SDK, mesh_key.dev_key, 16, "rcv_random,device key:", 0);
	du_rancomc_t *p_rancomc = (du_rancomc_t *) p_manu_data;
	p_rancomc->mac[0] = tbl_mac[0];
	p_rancomc->mac[1] = tbl_mac[1];
	memcpy(p_rancomc->device_confirm, dev_comfirm, 16);
	p_rancomc->randomC = RandomC;
	LOG_MSG_LIB(TL_LOG_NODE_SDK, randomc_pad, 23, "randomc_pad:", 0);
	mesh_du_ble_adv(p_manu_data->data_type+1, p_rancomc->mac, sizeof(du_rancomc_t)-OFFSETOF(du_rancomc_t, mac));
	return 0;
}

int mesh_du_rcv_prov_data(u8 *p_payload)
{
	char shar256_in[64];
	u8 shar256_out[32];
	du_manu_data_t *p_manu_data = (du_manu_data_t *)p_payload;
	du_prov_data_t *p_prov_data = (du_prov_data_t *)(p_manu_data->data);
	#if DU_SAMPLE_DATA_TEST_EN
	LOG_USER_MSG_INFO(randomc_pad, 23, "RandomC':", 0);
	LOG_USER_MSG_INFO(dev_key_pad, 23, "DeviceKey':", 0);
	LOG_USER_MSG_INFO(p_manu_data->data, 23, "%s raw_data:", __func__);
	#endif
// d3
	foreach(i, 23)
	{
		p_manu_data->data[i] = randomc_pad[i]^dev_key_pad[i] ^ p_manu_data->data[i];
	}
	#if DU_SAMPLE_DATA_TEST_EN
	LOG_USER_MSG_INFO(randomc_pad, 23, "RandomC':", 0);
	LOG_USER_MSG_INFO(dev_key_pad, 23, "DeviceKey':", 0);
	LOG_USER_MSG_INFO(p_manu_data->data, 23, "D3:", 0);
	#endif
// s4 
	u8 salt[] = "XiaoduIoTTinyMesh";
	memcpy(shar256_in, salt, sizeof(salt)-1);
	convert_num2_char(dev_comfirm, 0, 16, shar256_in+sizeof(salt)-1);
	mbedtls_sha256((u8 *)shar256_in, sizeof(salt)-1+32, shar256_out, 0);
	#if DU_SAMPLE_DATA_TEST_EN
	LOG_USER_MSG_INFO(shar256_out, 23, "S4:", 0);
	#endif
// s2, prov data
	foreach(i, 23){
		p_manu_data->data[i] = shar256_out[i] ^ p_manu_data->data[i];
	}
	
	if((p_prov_data->mac[0] != tbl_mac[1]) || (p_prov_data->mac[1] != tbl_mac[0])){
		return -1;
	}
	LOG_MSG_LIB(TL_LOG_NODE_SDK, (u8 *)p_prov_data, 23, "S2:", 0);	

	endianness_swap_u16((u8 *)&p_prov_data->unicast_addr);	
	#if !DU_SAMPLE_DATA_TEST_EN
	provison_net_info_str net_info;
	memset(&net_info, 0, sizeof(net_info));
	memcpy(net_info.net_work_key, p_prov_data->net_key, 16);
	net_info.flags = p_prov_data->flag;
	memcpy(net_info.iv_index+2, p_prov_data->iv_index, 2);
	net_info.unicast_address = p_prov_data->unicast_addr;
	mesh_provision_par_set((u8 *)&net_info);
	#endif

	mesh_du_ble_adv(p_manu_data->data_type+1, prov_para.device_uuid, 16);
	return 0;
}

void mesh_du_rcv_appkey(u8 *p_payload)
{
	du_manu_data_t *p_manu_data = (du_manu_data_t *)p_payload;
	du_appkey_t *p_appkey = (du_appkey_t *)p_manu_data->data;

	if(p_manu_data->len >= 25){
		if(memcmp(p_manu_data->data, tbl_mac, 2)){
			return;
		}
		memcpy(p_manu_data->data, p_manu_data->data+2, 19); // drop mac
	}
	#if DU_SAMPLE_DATA_TEST_EN
	LOG_USER_MSG_INFO(p_manu_data->data, 23, "%s raw_data:", __func__);
	#endif
	u8 dev_key_tmp[19]={0};
	memcpy(dev_key_tmp, mesh_key.dev_key, 16);
	foreach(i, 19)
	{
		p_manu_data->data[i] = dev_key_tmp[i] ^ p_manu_data->data[i];
	}
	
	LOG_MSG_LIB(TL_LOG_NODE_SDK, p_appkey->net_app_idx, 19, "Add AppKey:", 0);
	#if !DU_SAMPLE_DATA_TEST_EN
	mesh_app_key_set(APPKEY_ADD, p_appkey->appkey, GET_APPKEY_INDEX(p_appkey->net_app_idx),
								GET_NETKEY_INDEX(p_appkey->net_app_idx), 1);
	#endif
	memcpy(p_manu_data->data, tbl_mac, 2);
	p_manu_data->data[2] = ST_SUCCESS;
	mesh_du_ble_adv(p_manu_data->data_type+1, p_manu_data->data, 3);
}

void mesh_du_adv_proc(u8 *p_payload)
{	
	du_manu_data_t *p_manu_data = (du_manu_data_t *)p_payload;	
	if(is_provision_success()){
		if(DU_ANDROID_APPKEY == p_manu_data->data_type){
			mesh_du_rcv_appkey(p_payload);
		}
		return;
	}
//	LOG_USER_MSG_INFO(0, 0, "data type:0x%x", p_manu_data->data_type);
	if(LINK_UNPROVISION_STATE == prov_para.provison_rcv_state){		
		if((DU_TYPE_RANDOM == p_manu_data->data_type) || (DU_ANDROID_RANDOM == p_manu_data->data_type)){
			ultra_prov_tick = clock_time()|1;
			if(0 == mesh_du_rcv_random(p_payload)){
				prov_para.provison_rcv_state = STATE_PRO_DATA;
			}
		}
	}
	else if(STATE_PRO_DATA == prov_para.provison_rcv_state){
		if((DU_TYPE_PROV_DATA == p_manu_data->data_type) || (DU_ANDROID_PROV_DATA == p_manu_data->data_type)){
			if(0 == mesh_du_rcv_prov_data(p_payload)){	
				ultra_prov_tick = 0;
			}
		}
	}	
}

int genie_manu_nw_package(genie_nw_cache_t *p)
{	
	
	if(genie_nw_cache.tick){
		if((genie_nw_cache.msg_id == p->msg_id)){
			genie_nw_cache.tick = clock_time()|1;
			if((genie_nw_cache.seg_o == p->seg_o) && ((genie_nw_cache.len_payload+p->len_payload) <= sizeof(genie_nw_cache.payload))){
				memcpy(genie_nw_cache.payload+genie_nw_cache.len_payload, p->payload, p->len_payload);
				genie_nw_cache.len_payload += p->len_payload;

				LOG_USER_MSG_INFO((u8 *)p, p->len_payload+OFFSETOF(genie_nw_cache_t, payload), "rcv_seg%d:", genie_nw_cache.seg_o);
				if(genie_nw_cache.seg_o == p->seg_N){
					genie_nw_cache.tick = 0;
					return 1;
				}
				genie_nw_cache.seg_o++;
			}
		}
	}
	else{
		if((0 == genie_nw_cache.tick)&&(genie_nw_cache.msg_id != p->msg_id)){// new msg
			if((1 == p->seg_o) && (p->len_payload <= sizeof(genie_nw_cache.payload))){// first packet				
				genie_nw_cache.tick = clock_time()|1;
				memcpy(&genie_nw_cache, p, p->len_payload+OFFSETOF(genie_nw_cache_t, payload));
				genie_nw_cache.seg_o = 2; // first packet is 1, next should be 2
				LOG_USER_MSG_INFO((u8 *)p, p->len_payload+OFFSETOF(genie_nw_cache_t, payload),"rcv seg1:",0);
				if(1 == p->seg_N){
					genie_nw_cache.tick = 0;
					return 1;
				}

			}
		}
	}
	return 0;
}

int app_event_handler_ultra_prov(u8 *p_payload)
{
	genie_manu_factor_data_t *p_manu_data = (genie_manu_factor_data_t *)p_payload;
	#if DU_APP_ADV_CONTROL_EN
	u8 is_control = 0;
	#endif
	ios_app_data_t *p_ios_data = (ios_app_data_t *)p_payload;
	if((GAP_ADTYPE_MANUFACTURER_SPECIFIC==p_manu_data->manu_type) && (VENDOR_ID == p_manu_data->cid)){//Manufacturer Specific Data
		du_manu_data_t *p_du = (du_manu_data_t *) &p_manu_data->manu_len;
		if(p_du->data_type < DU_ANDROID_CONTROL){
			mesh_du_adv_proc(&p_manu_data->manu_len);
		}
		#if DU_APP_ADV_CONTROL_EN
		else if((DU_ANDROID_CONTROL == p_du->data_type) && (p_du->nw.nid == mesh_key.net_key[0][0].nid_m)){
			if(genie_manu_nw_package((genie_nw_cache_t *)&p_du->nw.msg_id)){
				is_control = 1;
			}
		}
		#endif
	}
	else if((p_ios_data->uuid_type == GAP_ADTYPE_128BIT_COMPLETE) && (VENDOR_ID == p_ios_data->cid) ){// 128-bit service UUIDs		
		if(p_ios_data->msg_type < DU_ANDROID_CONTROL){
			if(0 == memcmp(p_ios_data->ios_prov.mac, tbl_mac, 2)){ 
				if(genie_manu_nw_package((genie_nw_cache_t *)&p_ios_data->ios_prov.msg_id)){
					p_ios_data->uuid_len = genie_nw_cache.len_payload + 4;
					memcpy(&p_ios_data->data, genie_nw_cache.payload, genie_nw_cache.len_payload);  
					mesh_du_adv_proc(&p_ios_data->uuid_len);
				}
			}
		}
		#if DU_APP_ADV_CONTROL_EN
		else if((DU_ANDROID_CONTROL == p_ios_data->msg_type) && (p_ios_data->ios_nw.nid == mesh_key.net_key[0][0].nid_m)){
			if(genie_manu_nw_package((genie_nw_cache_t *)&p_ios_data->ios_nw.msg_id)){
				is_control = 1;
			}
		}
		#endif
	}
	#if DU_APP_ADV_CONTROL_EN
	if(is_control){
		genie_nw_cache.pkt_num = genie_nw_cache.len_payload+1;
		genie_nw_cache.len_payload = MESH_ADV_TYPE_MESSAGE;						
		mesh_rc_data_layer_network(&genie_nw_cache.pkt_num, MESH_BEAR_GATT, TRANSMIT_DEF_PAR);
	}
	#endif

	return 1;
}

void du_sample_data_run()
{
	u8 RandomA[8] = {0x79,0x48,0x10,0xe7,0x72,0x8a,0x08,0x39};
	u8 RandomB[8] = {0xa1,0x45,0x0b,0xc1,0xbc,0xda,0xf1,0xf3};
#if 1 // dec
	LOG_USER_MSG_INFO(randomc_pad, sizeof(randomc_pad), "mesh_cmd_ut_rx_seg_union:", 0) ;
	du_manu_data_t manu_random = {
		.len = 20,
		.ad_type = GAP_ADTYPE_MANUFACTURER_SPECIFIC,
		.cid = HTON16(VENDOR_ID),
		.data_type = DU_TYPE_RANDOM,
	};
	memcpy(manu_random.data, RandomA, 8);
	memcpy(manu_random.data+8, RandomB, 8);
	mesh_du_rcv_random((u8 *)&manu_random);

	du_manu_data_t manu_prov_data = {
		.len = 27,
		.ad_type = GAP_ADTYPE_MANUFACTURER_SPECIFIC,
		.cid = VENDOR_ID,
		.data_type = DU_TYPE_PROV_DATA,
	};
	u8 d3_pad[23] = {0x6c,0x65,0x29,0x8d,0xc6,0xeb,0xb3,0x1c,0x79,0x9a,0x0a,0x88,0xc7,0xb3,0xf2,0x5c,0x17,0x69,0x36,0x92,0xb0,0x20,0x58};
	memcpy(manu_prov_data.data, d3_pad, 23);
	mesh_du_rcv_prov_data((u8 *)&manu_prov_data);

	manu_prov_data.len = 23;
	manu_prov_data.data_type = DU_ANDROID_APPKEY;
	du_appkey_t *p_appkey = (du_appkey_t *)&manu_prov_data.data;
	u8 appkey[19] = {0x28,0xd3,0x7f,0xce,0x11,0xdd,0x08,0xf7,0xe5,0xa9,0x81,0x7b,0x41,0x71,0x64,0x29,0x91,0xf0,0x08};
	memcpy(p_appkey->net_app_idx, appkey, 19);
	mesh_du_rcv_appkey((u8 *)&manu_prov_data);
#else // enc
	u8 K1[16];
	u8 S4[23];
	u8 S1[16];
	u8 D1[16];
	u8 D3[23];
	u8 device_key[16];
	char salt[] = "XiaoduIoTTinyMesh";
 	static u8 shar256_in[96];
	u8 shar256_out[32];
	genie_three_par_t *p_three_par = (genie_three_par_t *)genie_three_par;

// K1
	convert_num2_char(RandomB, 0, 8, shar256_in);
	create_sha256_input_string(shar256_in+16, (u8 *)&p_three_par->pid, p_three_par->mac, p_three_par->sec_data);
	mbedtls_sha256(shar256_in, 68, shar256_out);
	memcpy(K1, shar256_out, 16);
	mini_printf(K1, 16, "K1:", 0);
	
// S1
	convert_num2_char(RandomA, 0, 8, shar256_in);
	convert_num2_char(RandomB, 0, 8, shar256_in+16);
	create_sha256_input_string(shar256_in+32, (u8 *)&p_three_par->pid, p_three_par->mac, p_three_par->sec_data);
	mbedtls_sha256(shar256_in, 84, shar256_out);
	memcpy(S1, shar256_out, 16);
	mini_printf(S1, 16, "S1:", 0);

// D1
	tn_aes_cmac(K1, S1, 16, D1);
	mini_printf(D1, 16, "D1:", 0);

// S4
	memcpy(shar256_in, salt, strlen(salt));
	convert_num2_char(D1, 0, 16, shar256_in+strlen(salt));
	mbedtls_sha256(shar256_in, strlen(salt)+32, shar256_out);
	memcpy(S4, shar256_out, 23);
	mini_printf(S4, 23, "S4:", 0);

// D3
	du_prov_data_t du_prov_data = {
		.mac = {0x2b, 0x00},
		.net_key = {0xa4,0x75,0x7c,0xf9,0x7b,0x0f,0xaf,0xac,0x3d,0x1d,0x49,0x45,0xb1,0xe9,0x5e,0xd5},
		.unicast_addr = 0x0001,
		.iv_index = {0x03, 0x96},
		.flag = 0,
	};
	endianness_swap_u16((u8 *)&du_prov_data.unicast_addr);

	foreach_arr(i,S4){
		D3[i] = S4[i] ^ *(u8 *)((u8 *)&du_prov_data + i); 
	}
	mini_printf((u8 *)&du_prov_data, 23, "S2:", 0);
	mini_printf(D3, 23, "D3:", 0);

// device key
	convert_num2_char(RandomA, 0, 8, shar256_in);
	create_sha256_input_string(shar256_in+16, (u8 *)&p_three_par->pid, p_three_par->mac, p_three_par->sec_data);
	mbedtls_sha256(shar256_in, 68, shar256_out);
	memcpy(device_key, shar256_out, 16);
	mini_printf(device_key, 16, "device_key:", 0);
#endif
}

#endif
#endif

