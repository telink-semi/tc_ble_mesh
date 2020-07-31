/********************************************************************************************************
 * @file     mesh_ota.c 
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
#include "../../proj/tl_common.h"
#if !WIN32
#include "../../proj/mcu/watchdog_i.h"
#endif 
#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../vendor/common/user_config.h"
#include "app_health.h"
#include "../../proj_lib/sig_mesh/app_mesh.h"
#include "../../proj_lib/ble/service/ble_ll_ota.h"
#include "mesh_ota.h"
#include "../../proj_lib/mesh_crypto/sha256_telink.h"

#if MD_MESH_OTA_EN
#if WIN32
#include "../../proj/common/tstring.h"

u32 	ota_firmware_size_k = FW_SIZE_MAX_K;	// same with pm_8269.c
#endif

STATIC_ASSERT(FW_SIZE_MAX_K % 4 == 0);  // because ota_firmware_size_k is must 4k aligned.
STATIC_ASSERT(ACCESS_WITH_MIC_LEN_MAX >= (MESH_OTA_CHUNK_SIZE + 1 + SZMIC_TRNS_SEG64 + 7)); // 1: op code, 7:margin
STATIC_ASSERT(sizeof(fw_update_metadata_check_t) + SIZE_OF_OP(FW_UPDATE_METADATA_CHECK) <= 11); // use unsegment should be better.

#define BLOCK_CRC32_CHECKSUM_EN     (0)

const fw_id_t fw_id_local = {
    MESH_PID_SEL,   // BUILD_VERSION, also mark in firmware_address[2:5]
    MESH_VID,
};

#if 0   // use const now.
void get_fw_id()
{
#if !WIN32
    #if FW_START_BY_BOOTLOADER_EN
    u32 fw_adr = DUAL_MODE_FW_ADDR_SIGMESH;
    #else
    u32 fw_adr = ota_program_offset ? 0 : 0x40000;
    #endif
    flash_read_page(fw_adr+2, sizeof(fw_id_local), (u8 *)&fw_id_local); // == BUILD_VERSION
#endif
}
#endif

u32 set_bit_by_cnt(u8 *out, u32 len, u32 cnt)
{
    int byte_cnt = 0;
    if(cnt <= len*8){
        while(cnt){
            if(cnt >=8){
                out[byte_cnt++] = 0xff;
                cnt -= 8;
            }else{
                out[byte_cnt++] = BIT_MASK_LEN(cnt);
                cnt = 0;
            }
        }
    }
    return byte_cnt;
}

model_mesh_ota_t        model_mesh_ota;
u32 mesh_md_mesh_ota_addr = FLASH_ADR_MD_MESH_OTA;

//--- common
int is_fwid_match(fw_id_t *id1, fw_id_t *id2)
{
    return (0 == memcmp(id1, id2, sizeof(fw_id_t)));
}

int is_blob_id_match(const u8 *blob_id1, const u8 *blob_id2)
{
    return (0 == memcmp(blob_id1, blob_id2, 8));
}

inline u16 get_fw_block_cnt(u32 blob_size, u8 bk_size_log)
{
    u32 bk_size = (1 << bk_size_log);
    return (blob_size + bk_size - 1) / bk_size;
}

inline u16 get_block_size(u32 blob_size, u8 bk_size_log, u16 block_num)
{
    u16 bk_size = (1 << bk_size_log);
    u16 bk_cnt = get_fw_block_cnt(blob_size, bk_size_log);
    if(block_num + 1 < bk_cnt){
        return bk_size;
    }else{
        return (blob_size - block_num * bk_size);
    }
}

inline u16 get_fw_chunk_cnt(u16 bk_size_current, u16 chunk_size_max)
{
    return (bk_size_current + chunk_size_max - 1) / chunk_size_max;
}

inline u16 get_chunk_size(u16 bk_size_current, u16 chunk_size_max, u16 chunk_num)
{
    u16 chunk_cnt = get_fw_chunk_cnt(bk_size_current, chunk_size_max);
    if(chunk_num + 1 < chunk_cnt){
        return chunk_size_max;
    }else{
        return (bk_size_current - chunk_num * chunk_size_max);
    }
}

inline u32 get_fw_data_position(u16 block_num, u8 bk_size_log, u16 chunk_num, u16 chunk_size_max)
{
    return (block_num * (1 << bk_size_log) + chunk_num * chunk_size_max);
}

int is_mesh_ota_cid_match(u16 cid)
{
    #if WIN32 
    return 1;
    #else
    return (cid == cps_cid);
    #endif
}

/***************************************** 
------- for distributor node
******************************************/
#if DISTRIBUTOR_UPDATE_CLIENT_EN
// const u32 fw_id_new     = 0xff000021;   // set in distribution start
const u8  blob_id_new[8] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88};
#define TEST_CHECK_SUM_TYPE     (BLOB_BLOCK_CHECK_SUM_TYPE_CRC32)
#define TEST_BK_SIZE_LOG        (MESH_OTA_BLOCK_SIZE_LOG_MIN)

#define NEW_FW_MAX_SIZE     (FLASH_ADR_AREA_FIRMWARE_END) // = (192*1024)
u32 new_fw_size = 0;

#if GATEWAY_ENABLE
u8* fw_ota_data_tx;
#elif WIN32
u8 fw_ota_data_tx[NEW_FW_MAX_SIZE];
u8 fw_ota_data_rx[NEW_FW_MAX_SIZE] = {1,2,3,4,5,};
#endif

STATIC_ASSERT(MESH_OTA_BLOCK_SIZE_MAX <= MESH_OTA_CHUNK_SIZE * 32);

//STATIC_ASSERT(MESH_OTA_CHUNK_NUM_MAX <= 32);  // max bit of miss_mask in fw_distribut_srv_proc


fw_distribut_srv_proc_t fw_distribut_srv_proc = {{0}};      // for distributor (server + client) + updater client

#define master_ota_current_node_adr     (fw_distribut_srv_proc.list[fw_distribut_srv_proc.node_num].adr)


inline void mesh_ota_master_next_st_set(u8 st)
{
#if 0 // (WIN32 && DEBUG_SHOW_VC_SELF_EN)
    LOG_MSG_INFO (TL_LOG_CMD_NAME, 0, 0, "OTA master next st: 0x%02x", st);
#endif

    fw_distribut_srv_proc.st = st;
}

inline int is_only_get_fw_info_fw_distribut_srv()
{
    return (0 == fw_distribut_srv_proc.adr_group);
}

void mesh_ota_master_next_block()
{
    fw_distribut_srv_proc.block_start.block_num++;
    fw_distribut_srv_proc.node_num = fw_distribut_srv_proc.chunk_num = 0;
    memset(fw_distribut_srv_proc.miss_mask, 0, sizeof(fw_distribut_srv_proc.miss_mask));
    mesh_ota_master_next_st_set(MASTER_OTA_ST_BLOB_BLOCK_START);
}

u32 distribut_get_not_apply_cnt()
{
    u32 cnt = 0;
    foreach(i, fw_distribut_srv_proc.node_cnt){
        if(0 == fw_distribut_srv_proc.list[i].apply_flag){
            cnt++;
        }
    }

    return cnt;
}

inline u16 distribut_get_fw_block_cnt()
{
    return get_fw_block_cnt(fw_distribut_srv_proc.blob_size, fw_distribut_srv_proc.bk_size_log);
}

inline u16 distribut_get_block_size(u16 block_num)
{
    return get_block_size(fw_distribut_srv_proc.blob_size, fw_distribut_srv_proc.bk_size_log, block_num);
}

inline u16 distribut_get_fw_chunk_cnt()
{
    blob_block_start_t *bk_start = &fw_distribut_srv_proc.block_start;
    return get_fw_chunk_cnt(fw_distribut_srv_proc.bk_size_current, bk_start->chunk_size);
}

inline u16 distribut_get_chunk_size(u16 chunk_num)
{
    blob_block_start_t *bk_start = &fw_distribut_srv_proc.block_start;
    return get_chunk_size(fw_distribut_srv_proc.bk_size_current, bk_start->chunk_size, chunk_num);
}

inline u32 distribut_get_fw_data_position(u16 chunk_num)
{
    blob_block_start_t *bk_start = &fw_distribut_srv_proc.block_start;
    return get_fw_data_position(bk_start->block_num, fw_distribut_srv_proc.bk_size_log, chunk_num, bk_start->chunk_size);
}

#if ((ANDROID_APP_ENABLE || IOS_APP_ENABLE))
void APP_set_mesh_ota_pause_flag(u8 val)
{
    if(fw_distribut_srv_proc.st){
        fw_distribut_srv_proc.pause_flag = val;
    }
}
#else
void APP_RefreshProgressBar(u16 bk_current, u16 bk_total, u16 chunk_cur, u16 chunk_total, u8 percent)
{
    LOG_MSG_INFO (TL_LOG_CMD_NAME, 0, 0, "OTA,block total:%2d,cur:%2d,chunk total:%2d,cur:%2d, Progress:%d%%", bk_total, bk_current, chunk_total, chunk_cur, percent);
}

void APP_report_mesh_ota_apply_status(u16 adr_src, fw_update_status_t *p)
{
    // nothing for VC now
}

u16 APP_get_GATT_connect_addr()
{
    #if GATEWAY_ENABLE
    return ele_adr_primary;
    #else
    return connect_addr_gatt;
    #endif
}

#endif

#if WIN32
void APP_print_connected_addr()
{
    u16 connect_addr = APP_get_GATT_connect_addr();
    LOG_MSG_INFO(TL_LOG_CMD_NAME, 0, 0, "connected addr 0x%04x", connect_addr);
}
#else
#define APP_print_connected_addr()      
#endif

/*
	model command callback function ----------------
*/	
void distribut_srv_proc_init()
{
    memset(&fw_distribut_srv_proc, 0, sizeof(fw_distribut_srv_proc));
    #if DISTRIBUTOR_UPDATE_CLIENT_EN
    new_fw_size = 0;
    #endif
}

int mesh_tx_cmd_fw_distribut_st(u8 idx, u16 ele_adr, u16 dst_adr, u8 st)
{
	fw_distribut_status_t rsp = {0};
	rsp.st = st;

	return mesh_tx_cmd_rsp(FW_DISTRIBUT_STATUS, (u8 *)&rsp, sizeof(fw_distribut_status_t), ele_adr, dst_adr, 0, 0);
}

int mesh_fw_distribut_st_rsp(mesh_cb_fun_par_t *cb_par, u8 st)
{
	model_g_light_s_t *p_model = (model_g_light_s_t *)cb_par->model;
	return mesh_tx_cmd_fw_distribut_st(cb_par->model_idx, p_model->com.ele_adr, cb_par->adr_src, st);
}

int mesh_cmd_sig_fw_distribut_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	u8 st;
    if(fw_distribut_srv_proc.st){
        st = DISTRIBUT_ST_SUCCESS;
    }else{
        st = DISTRIBUT_ST_SUCCESS;
    }
	return mesh_fw_distribut_st_rsp(cb_par, st);
}

int read_ota_file2buffer()
{
#if VC_APP_ENABLE
    if(0 != ota_file_check()){
        return -1;
    }
    else
#endif
    {
#if DISTRIBUTOR_UPDATE_CLIENT_EN
    #if GATEWAY_ENABLE
        fw_ota_data_tx = (u8*)(ota_program_offset);//reflect to the flash part 
        new_fw_size = get_fw_len();
    #else
        new_fw_size = new_fw_read(fw_ota_data_tx, sizeof(fw_ota_data_tx));
    #endif
        
        if((0 == new_fw_size) || (-1 == new_fw_size)){
            return -1;
        }

        if(new_fw_size < 10*1024){
            fw_distribut_srv_proc.miss_chunk_test_flag = 1;
        }
#endif
    }

    return 0;
}

int mesh_cmd_sig_fw_distribut_start(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	u8 st = DISTRIBUT_ST_INTERNAL_ERROR;
    fw_distribut_start_t *p_start = (fw_distribut_start_t *)par;
    u32 update_node_cnt = (par_len - OFFSETOF(fw_distribut_start_t,update_list)) / 2;
    
    if(update_node_cnt > MESH_OTA_UPDATE_NODE_MAX || update_node_cnt == 0){
        LOG_MSG_ERR(TL_LOG_COMMON,0, 0,"update node cnt error: %d", update_node_cnt);
    	st = DISTRIBUT_ST_OUTOF_RESOURCE;
    }else if(fw_distribut_srv_proc.st){   // comfirm later
        #if 0
        int retransmit = (0 == memcmp(p_start,&fw_distribut_srv_proc.fw_id, OFFSETOF(fw_distribut_start_t,update_list)));
        if(retransmit){
            foreach(i,update_node_cnt){
                if(fw_distribut_srv_proc.list[i].adr != p_start->update_list[i]){
                    retransmit = 0;
                    break;
                }
            }
    	}
    	
    	st = retransmit ? DISTRIBUT_ST_SUCCESS : DISTRIBUT_ST_DISTRIBUTOR_BUSY;
    	#else
    	st = DISTRIBUT_ST_SUCCESS;
    	#endif
    }else{
        APP_print_connected_addr();
        distribut_srv_proc_init();
        fw_distribut_srv_proc.adr_group = p_start->adr_group;
        fw_distribut_srv_proc.adr_distr_node = ele_adr_primary;
        if(0 != read_ota_file2buffer()){
            distribut_srv_proc_init();
            return 0;   // error
        }
        
        if(update_node_cnt){
            foreach(i,update_node_cnt){
                fw_distribut_srv_proc.list[i].adr = p_start->update_list[i];
                fw_distribut_srv_proc.list[i].st_block_start = UPDATE_NODE_ST_IN_PROGRESS;
            }
            fw_distribut_srv_proc.node_cnt = update_node_cnt;
        }

        #if WIN32 
        if(is_only_get_fw_info_fw_distribut_srv()){
            mesh_ota_master_next_st_set(MASTER_OTA_ST_FW_UPDATE_INFO_GET);
        }else
        #endif
        {
            mesh_ota_master_next_st_set(MASTER_OTA_ST_DISTRIBUT_START);
        }
        
	    st = DISTRIBUT_ST_SUCCESS;
	}
	
	return mesh_fw_distribut_st_rsp(cb_par, st);
}

int mesh_cmd_sig_fw_distribut_cancel(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int abort_flag = 0;
	u8 st;
    if(fw_distribut_srv_proc.st != MASTER_OTA_ST_MAX){
        abort_flag = 1;
    }
    distribut_srv_proc_init();
    // fw_distribut_srv_proc.st = 0;
    
    st = DISTRIBUT_ST_SUCCESS;

	int err = mesh_fw_distribut_st_rsp(cb_par, st);
	if(abort_flag){
        access_cmd_fw_update_control(ADR_ALL_NODES, FW_UPDATE_CANCEL, 0xff);
	}else{
        #if VC_APP_ENABLE
        extern int disable_log_cmd;
        disable_log_cmd = 1;   // mesh OTA finished
        #endif
	}
	
	return err;
}

int mesh_cmd_sig_fw_distribut_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

// -------
int mesh_cmd_sig_fw_distribut_detail_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    fw_distribut_detail_list_t rsp = {{{0}}};
    u32 rsp_size = 0;
    if(fw_distribut_srv_proc.st){
        u32 node_cnt = fw_distribut_srv_proc.node_cnt;
        if(node_cnt > MESH_OTA_UPDATE_NODE_MAX){
            node_cnt = MESH_OTA_UPDATE_NODE_MAX;
        }
        
        rsp_size = node_cnt * sizeof(fw_distribut_node_t);
        foreach(i,node_cnt){
            rsp.node[i].adr = fw_distribut_srv_proc.list[i].adr;
            rsp.node[i].st = fw_distribut_srv_proc.list[i].st_block_start;
        }
    }
    
	model_g_light_s_t *p_model = (model_g_light_s_t *)cb_par->model;
	return mesh_tx_cmd_rsp(FW_DISTRIBUT_DETAIL_LIST, (u8 *)&rsp, rsp_size, p_model->com.ele_adr, cb_par->adr_src, 0, 0);
}

int mesh_cmd_sig_fw_distribut_detail_list(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

//--model command interface-------------------
//-----------access command--------
#if 0
int access_cmd_fw_distribut_start(u16 adr_dst)
{
	fw_distribut_start_t cmd;
	cmd.id
	cmd.adr_group
	foreach(i,n){
	    cmd.update_list[i] = ;
	}
	return SendOpParaDebug(adr_dst, 1, FW_DISTRIBUT_START, (u8 *)&cmd, sizeof(cmd));
}
#endif

int access_cmd_fw_distribut_cancel(u16 adr_dst)
{
    LOG_MSG_FUNC_NAME();
	u8 par[1] = {0};
	return SendOpParaDebug(adr_dst, 1, FW_DISTRIBUT_CANCEL, par, 0);
}

int access_cmd_fw_update_info_get(u16 adr_dst)
{
    LOG_MSG_FUNC_NAME();
	fw_update_info_get_t info_get = {0, 1};
	return SendOpParaDebug(adr_dst, 1, FW_UPDATE_INFO_GET, (u8 *)&info_get, sizeof(info_get));
}

int access_cmd_blob_info_get(u16 adr_dst)
{
    LOG_MSG_FUNC_NAME();
	u8 par[1] = {0};
	return SendOpParaDebug(adr_dst, 1, BLOB_INFO_GET, par, 0);
}

int access_cmd_fw_update_metadata_check(u16 adr_dst, fw_metadata_t *metadata)
{
    LOG_MSG_FUNC_NAME();
    fw_update_metadata_check_t cmd = {0};
    cmd.image_index = 0;
    if(metadata){
        memcpy(&cmd.metadata, metadata, sizeof(cmd.metadata));
    }else{
        // set metadata to 0.
    }
    return SendOpParaDebug(adr_dst, 1, FW_UPDATE_METADATA_CHECK, (u8 *)&cmd, sizeof(cmd));
}

int access_cmd_fw_update_start(u16 adr_dst, const u8 *blob_id, fw_metadata_t *metadata)
{
    LOG_MSG_FUNC_NAME();
    fw_update_start_t cmd = {0};
    cmd.ttl = TTL_PUB_USE_DEFAULT;
    cmd.timeout_base = 0; // not use timeout now.
    memcpy(cmd.blob_id, blob_id, sizeof(cmd.blob_id));
    memcpy(fw_distribut_srv_proc.blob_id, blob_id, sizeof(fw_distribut_srv_proc.blob_id));   // back up
    cmd.image_index = 0;
    if(metadata){
        memcpy(&cmd.metadata, metadata, sizeof(cmd.metadata));
    }else{
    }
	return SendOpParaDebug(adr_dst, 1, FW_UPDATE_START, (u8 *)&cmd, sizeof(cmd));
}

int access_cmd_fw_update_get(u16 adr_dst)
{
    LOG_MSG_FUNC_NAME();
	u8 par[1] = {0};
	return SendOpParaDebug(adr_dst, 1, FW_UPDATE_GET, par, 0);
}

int access_cmd_fw_update_control(u16 adr_dst, u16 op, u8 rsp_max)
{
    if(FW_UPDATE_APPLY == op){
        LOG_MSG_INFO (TL_LOG_CMD_NAME, 0, 0, "access_cmd_fw_update_apply ",0);
    }else if(FW_UPDATE_CANCEL == op){
        LOG_MSG_INFO (TL_LOG_CMD_NAME, 0, 0, "access_cmd_fw_update_cancel ",0);
    }else{
        LOG_MSG_ERR(TL_LOG_COMMON,0, 0,"error control op code",0);
        return -1;
    }

	u8 par[1] = {0};
	return SendOpParaDebug(adr_dst, rsp_max, op, par, 0);
}

int access_cmd_blob_transfer_get(u16 adr_dst)
{
    LOG_MSG_FUNC_NAME();
	u8 par[1] = {0};
	return SendOpParaDebug(adr_dst, 1, BLOB_TRANSFER_GET, par, 0);
}

int access_cmd_blob_transfer_start(u16 adr_dst, u32 blob_size, u8 bk_size_log)
{
    LOG_MSG_FUNC_NAME();
    blob_transfer_start_t cmd = {0};
    cmd.transfer_mode = MESH_OTA_TRANSFER_MODE_SEL;
    memcpy(&cmd.blob_id, fw_distribut_srv_proc.blob_id, sizeof(cmd.blob_id));
    cmd.blob_size = fw_distribut_srv_proc.blob_size = blob_size;
    cmd.bk_size_log = fw_distribut_srv_proc.bk_size_log = bk_size_log;
    cmd.client_mtu_size = MESH_CMD_ACCESS_LEN_MAX;
	return SendOpParaDebug(adr_dst, 1, BLOB_TRANSFER_START, (u8 *)&cmd, sizeof(cmd));
}

int access_cmd_blob_block_start(u16 adr_dst, u16 block_num)
{
    LOG_MSG_FUNC_NAME();
    blob_block_start_t *p_bk_start = &fw_distribut_srv_proc.block_start;  // record parameters
    p_bk_start->block_num = block_num;
    p_bk_start->chunk_size = MESH_OTA_CHUNK_SIZE;
    fw_distribut_srv_proc.bk_size_current = distribut_get_block_size(block_num);

	return SendOpParaDebug(adr_dst, 1, BLOB_BLOCK_START, (u8 *)p_bk_start, sizeof(blob_block_start_t));
}

int access_cmd_blob_chunk_transfer(u16 adr_dst, u8 *cmd, u32 len)
{
	return SendOpParaDebug(adr_dst, 0, BLOB_CHUNK_TRANSFER, cmd, len);
}

int access_cmd_blob_block_get(u16 adr_dst, u16 block_num)
{
    LOG_MSG_FUNC_NAME();
	u8 par[1] = {0};
	return SendOpParaDebug(adr_dst, 1, BLOB_BLOCK_GET, par, 0);
}

//--model command interface end----------------

//--mesh ota master proc
fw_detail_list_t * get_fw_node_detail_list(u16 node_adr)
{
    foreach(i,fw_distribut_srv_proc.node_cnt){
        fw_detail_list_t *p_list = &fw_distribut_srv_proc.list[i];
        if(p_list->adr == node_adr){
            return p_list;
        }
    }
    return 0;
}

u32 is_need_block_transfer()
{
    foreach(i,fw_distribut_srv_proc.node_cnt){
        fw_detail_list_t *p_list = &fw_distribut_srv_proc.list[i];
        if(BLOB_TRANS_ST_SUCCESS == p_list->st_block_start){
            return 1;
        }
    }
    return 0;
}

#if DISTRIBUTOR_UPDATE_CLIENT_EN
void mesh_ota_master_wait_ack_st_set()
{
    mesh_ota_master_next_st_set(fw_distribut_srv_proc.st | OTA_WAIT_ACK_MASK);
}

void mesh_ota_master_wait_ack_st_return(int success)
{
    if(!success){
        fw_distribut_srv_proc.list[fw_distribut_srv_proc.node_num].skip_flag = 1;
    }
    fw_distribut_srv_proc.node_num++;
    mesh_ota_master_next_st_set(fw_distribut_srv_proc.st & (~OTA_WAIT_ACK_MASK));
}

int mesh_ota_check_skip_current_node()
{
    fw_distribut_srv_proc_t *distr_proc = &fw_distribut_srv_proc;
    if(distr_proc->list[distr_proc->node_num].skip_flag){
        if(MASTER_OTA_ST_BLOB_BLOCK_START == distr_proc->st){
            LOG_MSG_INFO(TL_LOG_COMMON,0,0,"access_cmd_blob_block_start, XXXXXX Skip addr:0x%04x", distr_proc->list[distr_proc->node_num].adr);
        }
        distr_proc->node_num++;
        return 1;
    }
    return 0;
}

#if DEBUG_SHOW_VC_SELF_EN
int is_mesh_ota_and_only_VC_update()
{
    fw_distribut_srv_proc_t *distr_proc = &fw_distribut_srv_proc;
    if(MASTER_OTA_ST_BLOB_CHUNK_START == distr_proc->st){
        if((1 == distr_proc->node_cnt) && (distr_proc->list[0].adr == ele_adr_primary)){
            return 1;
        }
    }
    return 0;
}
#endif

void mesh_ota_master_ack_timeout_handle()
{
    if(is_mesh_ota_master_wait_ack()){
        mesh_ota_master_wait_ack_st_return(0);
    }
}

int mesh_ota_master_rx (mesh_rc_rsp_t *rsp, u16 op, u32 size_op)
{
    int op_handle_ok = 0;
    u8 *par = rsp->data + size_op;
    u16 par_len = GET_PAR_LEN_FROM_RSP(rsp->len, size_op);
    fw_distribut_srv_proc_t *distr_proc = &fw_distribut_srv_proc;
    int adr_match = (rsp->src == master_ota_current_node_adr);
    int next_st = 0;
    int skip_flag = 0;
    
    if(FW_UPDATE_INFO_STATUS == op){
        if(adr_match && ((MASTER_OTA_ST_FW_UPDATE_INFO_GET | OTA_WAIT_ACK_MASK) == distr_proc->st)){
            //fw_update_info_status_t *p = (fw_update_info_status_t *)par;
            next_st = 1;
        }
        op_handle_ok = 1;
    }else if(FW_DISTRIBUT_STATUS == op){
        fw_distribut_status_t *p = (fw_distribut_status_t *)par;
        if(DISTRIBUT_ST_SUCCESS == p->st){
        }else if(DISTRIBUT_ST_OUTOF_RESOURCE == p->st){
        }

        if(distr_proc->adr_group){  // distribute start
            if(DISTRIBUT_ST_SUCCESS != p->st){
                LOG_MSG_ERR (TL_LOG_COMMON, 0, 0, "fw distribution status error:%d ", p->st);
            }
        }else{                      // distribute stop
            LOG_MSG_INFO(TL_LOG_CMD_NAME, 0, 0, "mesh OTA completed or get info ok!", 0);
        }
        op_handle_ok = 1;
    }else if(FW_DISTRIBUT_DETAIL_LIST == op){
        op_handle_ok = 1;
    }else if(FW_UPDATE_METADATA_CHECK_STATUS == op){
        if(adr_match){
            fw_update_metadata_check_status_t *p = (fw_update_metadata_check_status_t *)par;
            if((MASTER_OTA_ST_UPDATE_METADATA_CHECK | OTA_WAIT_ACK_MASK) == distr_proc->st){
                if(UPDATE_ST_SUCCESS != p->st){
                    skip_flag = 1;
                    LOG_MSG_ERR (TL_LOG_COMMON, 0, 0, "fw update metada check error:%d ", p->st);
                }
                next_st = 1;
            }
        }
        op_handle_ok = 1;
    }else if(FW_UPDATE_STATUS == op){
        if(adr_match){
            fw_update_status_t *p = (fw_update_status_t *)par;
            fw_detail_list_t * p_list = get_fw_node_detail_list(rsp->src);
            p_list->update_phase = p->update_phase;
            // p_list->additional_info = p->additional_info; // optional
            
            if((MASTER_OTA_ST_UPDATE_START | OTA_WAIT_ACK_MASK) == distr_proc->st){
                next_st = 1;
            }else if((MASTER_OTA_ST_UPDATE_GET | OTA_WAIT_ACK_MASK) == distr_proc->st){
                next_st = 1;
            }else if((MASTER_OTA_ST_UPDATE_APPLY | OTA_WAIT_ACK_MASK) == distr_proc->st){
                next_st = 1;
				if(UPDATE_ST_SUCCESS != p->st){
					fw_distribut_srv_proc.list[fw_distribut_srv_proc.node_num].skip_flag = 0;
				}
				APP_report_mesh_ota_apply_status(rsp->src, p);
				if(UPDATE_ST_SUCCESS == p->st && is_apply_phase_success(p->update_phase)){
                    LOG_MSG_INFO(TL_LOG_COMMON,0, 0,"fw update apply sucess!!!",0);
				}else{
                    LOG_MSG_ERR (TL_LOG_COMMON, 0, 0, "------------------------------!!! Firmware update apply ERROR !!!",0);
                }
            }
        
            if(UPDATE_ST_SUCCESS != p->st){
                if((MASTER_OTA_ST_UPDATE_APPLY | OTA_WAIT_ACK_MASK) != distr_proc->st){
                    skip_flag = 1;
                }
                LOG_MSG_ERR (TL_LOG_COMMON, 0, 0, "fw update status error:%d ", p->st);
            }
        }
        op_handle_ok = 1;
    }else if(BLOB_TRANSFER_STATUS == op){
        if(adr_match){
            blob_transfer_status_t *p = (blob_transfer_status_t *)par;
            if((MASTER_OTA_ST_BLOB_TRANSFER_GET | OTA_WAIT_ACK_MASK) == distr_proc->st){
                next_st = 1;
            }else if((MASTER_OTA_ST_BLOB_TRANSFER_START | OTA_WAIT_ACK_MASK) == distr_proc->st){
                next_st = 1;
            }
        
            if(BLOB_TRANS_ST_SUCCESS != p->st){
                skip_flag = 1;
                LOG_MSG_ERR (TL_LOG_COMMON, 0, 0, "object transfer status error:%d ", p->st);
            }
        }
        op_handle_ok = 1;
    }else if(BLOB_BLOCK_STATUS == op){
        if(adr_match){
            blob_block_status_t *p = (blob_block_status_t *)par;
            if((MASTER_OTA_ST_BLOB_BLOCK_START | OTA_WAIT_ACK_MASK) == distr_proc->st){
                if((MASTER_OTA_ST_BLOB_BLOCK_START | OTA_WAIT_ACK_MASK) == distr_proc->st){
                    distr_proc->list[distr_proc->node_num].st_block_start = p->st;
                    next_st = 1;
                }
            }else if((MASTER_OTA_ST_BLOB_BLOCK_GET | OTA_WAIT_ACK_MASK) == distr_proc->st){
                distr_proc->list[distr_proc->node_num].st_block_get = p->format;
                int miss_chunk_len = par_len - OFFSETOF(blob_block_status_t,miss_chunk);
                if(miss_chunk_len >= 0 && miss_chunk_len <= sizeof(distr_proc->miss_mask)){
                    // distr_proc->miss_mask[] have been zero before block get and after end of MASTER_OTA_ST_BLOB_CHUNK_START.
                    if(BLOB_BLOCK_FORMAT_NO_CHUNK_MISS == p->format){
                    }else if(BLOB_BLOCK_FORMAT_ALL_CHUNK_MISS == p->format){
                        set_bit_by_cnt(distr_proc->miss_mask, sizeof(distr_proc->miss_mask), distribut_get_fw_chunk_cnt()); // all need send
                        LOG_MSG_LIB(TL_LOG_NODE_BASIC, 0, 0, "ALL CHUNK MISS", 0);
                    }else if(BLOB_BLOCK_FORMAT_SOME_CHUNK_MISS == p->format){
                        for(int i = 0; i < (miss_chunk_len); ++i){
                            distr_proc->miss_mask[i] |= p->miss_chunk[i];
                        }
                        LOG_MSG_LIB (TL_LOG_NODE_BASIC, p->miss_chunk, miss_chunk_len, "SOME MISS CHUNK ", 0);
                    }else if(BLOB_BLOCK_FORMAT_ENCODE_MISS_CHUNK == p->format){
                        // TODO : Note: use "or" but not "memcpy"
                        // distr_proc->miss_mask[i] |= ??;
                        LOG_MSG_ERR (TL_LOG_COMMON, 0, 0, "TODO: BLOB_BLOCK_FORMAT_ENCODE_MISS_CHUNK", 0);
                    }
                }else{
                    LOG_MSG_ERR (TL_LOG_COMMON, 0, 0, "TODO: MISS CHUNK LENGTH TOO LONG", 0);
                }
                next_st = 1;
            }
            
            if(p->st != BLOB_TRANS_ST_SUCCESS){
                skip_flag = 1;
                LOG_MSG_ERR (TL_LOG_COMMON, 0, 0, "object block status error:%d ", p->st);
            }
        }
        op_handle_ok = 1;
    }else if(BLOB_INFO_STATUS == op){
        if(adr_match && ((MASTER_OTA_ST_BLOB_INFO_GET | OTA_WAIT_ACK_MASK) == distr_proc->st)){
            //blob_info_status_t *p = (blob_info_status_t *)par;
            next_st = 1;
        }
        op_handle_ok = 1;
    }else if(CFG_MODEL_SUB_STATUS == op){
        if(adr_match && ((MASTER_OTA_ST_SUBSCRIPTION_SET | OTA_WAIT_ACK_MASK) == distr_proc->st)){
            mesh_cfg_model_sub_status_t *p = (mesh_cfg_model_sub_status_t *)par;
            if(SIG_MD_BLOB_TRANSFER_S == (p->set.model_id & (DRAFT_FEAT_VD_MD_EN ? 0xffffffff : 0xffff))){
                if(ST_SUCCESS == p->status){
                }else{
                    LOG_MSG_ERR(TL_LOG_COMMON,0, 0,"set group failed %x",p->set.ele_adr);
                }
                next_st = 1;
                op_handle_ok = 1;
            }
        }
    }

    if(next_st){
        mesh_ota_master_wait_ack_st_return(!skip_flag);
    }
    
    return op_handle_ok;
}

void mesh_ota_tx_fw_data_read(u32 addr, u32 len, u8 *buf_out)
{
    #if GATEWAY_ENABLE
    flash_read_page(ota_program_offset + addr, len, buf_out);
    #else
    memcpy(buf_out, (u8 *)fw_ota_data_tx + addr, len);
    #endif
}

int get_fw_metadata(fw_metadata_t *metadata_out)
{
    mesh_ota_tx_fw_data_read(2, sizeof(fw_metadata_t),(u8 *)metadata_out);
    return 0;
}

void check_and_clear_mesh_ota_master_wait_ack()
{
    fw_distribut_srv_proc_t *distr_proc = &fw_distribut_srv_proc;
    if(distr_proc->st & OTA_WAIT_ACK_MASK){
         distr_proc->st &= ~OTA_WAIT_ACK_MASK;
    }
}

void mesh_ota_master_proc()
{
    fw_distribut_srv_proc_t *distr_proc = &fw_distribut_srv_proc;
	if(0 == distr_proc->st || is_busy_segment_or_reliable_flow() || !is_mesh_adv_cmd_fifo_empty()){
		return ;
	}
	
#if (WIN32 && (PROXY_HCI_SEL == PROXY_HCI_GATT))
    extern unsigned char connect_flag;
    if(!(pair_login_ok || DEBUG_SHOW_VC_SELF_EN) || distr_proc->pause_flag){
        return ;
    }
#endif

	static u32 tick_ota_master_proc;
	if((distr_proc->st != MASTER_OTA_ST_DISTRIBUT_START)
	&& (distr_proc->st != MASTER_OTA_ST_BLOB_CHUNK_START)){
	    if(clock_time_exceed(tick_ota_master_proc, 3000*1000)){
    	    tick_ota_master_proc = clock_time();
            LOG_MSG_INFO(TL_LOG_COMMON,0, 0,"mesh_ota_master_proc state: 0x%02x",distr_proc->st);
        }
	}else{
	    tick_ota_master_proc = clock_time();
	}

    if(distr_proc->st & OTA_WAIT_ACK_MASK){
        return ;
    }
	
	switch(distr_proc->st){
		case MASTER_OTA_ST_DISTRIBUT_START:
		    // FW_DISTRIBUT_START was send by VC
		    distr_proc->node_num = 0;
		    if (is_only_get_fw_info_fw_distribut_srv()){ // just for get fw info, not OTA flow.
    		    mesh_ota_master_next_st_set(MASTER_OTA_ST_FW_UPDATE_INFO_GET);
            }else{
    		    mesh_ota_master_next_st_set(MASTER_OTA_ST_UPDATE_METADATA_CHECK);
                APP_RefreshProgressBar(0, 0, 0, 0, 0);
            }
			break;
			
		case MASTER_OTA_ST_UPDATE_METADATA_CHECK:   // it should be better that right after distribute and before other ota command 
		    if(distr_proc->node_num < distr_proc->node_cnt){
		        if(mesh_ota_check_skip_current_node()){ break;}

		        fw_metadata_t fw_metadata = {{0}};
		        get_fw_metadata(&fw_metadata);
    	        if(0 == access_cmd_fw_update_metadata_check(master_ota_current_node_adr, &fw_metadata)){
    	            mesh_ota_master_wait_ack_st_set();
    	        }
	        }else{
                distr_proc->node_num = 0;
    	        mesh_ota_master_next_st_set(MASTER_OTA_ST_SUBSCRIPTION_SET);
	        }
			break;
			
		case MASTER_OTA_ST_SUBSCRIPTION_SET:
		    if(distr_proc->node_num < distr_proc->node_cnt){
		        if(mesh_ota_check_skip_current_node()){ break;}
    	        if(0 == cfg_cmd_sub_set(CFG_MODEL_SUB_ADD, master_ota_current_node_adr, master_ota_current_node_adr, 
    	                        fw_distribut_srv_proc.adr_group, SIG_MD_BLOB_TRANSFER_S, !DRAFT_FEAT_VD_MD_EN)){
    	            mesh_ota_master_wait_ack_st_set();
    	        }
	        }else{
                distr_proc->node_num = 0;
    	        mesh_ota_master_next_st_set(MASTER_OTA_ST_FW_UPDATE_INFO_GET);
	        }
			break;
			
		case MASTER_OTA_ST_FW_UPDATE_INFO_GET:
		    if(distr_proc->node_num < distr_proc->node_cnt){
		        if(mesh_ota_check_skip_current_node()){ break;}
		        
    	        if(0 == access_cmd_fw_update_info_get(master_ota_current_node_adr)){
    	            mesh_ota_master_wait_ack_st_set();
    	        }
	        }else{
	            if(is_only_get_fw_info_fw_distribut_srv()){
    	            distribut_srv_proc_init();  // stop
	            }else{
                    distr_proc->node_num = 0;
                    mesh_ota_master_next_st_set(MASTER_OTA_ST_UPDATE_START);
    	        }
	        }
			break;
			
		case MASTER_OTA_ST_UPDATE_START:
		    if(distr_proc->node_num < distr_proc->node_cnt){
		        if(mesh_ota_check_skip_current_node()){ break;}
		        
		        fw_metadata_t fw_metadata = {{0}};
		        get_fw_metadata(&fw_metadata);
    	        if(0 == access_cmd_fw_update_start(master_ota_current_node_adr, blob_id_new, &fw_metadata)){
    	            mesh_ota_master_wait_ack_st_set();
    	        }
	        }else{
                distr_proc->node_num = 0;
    	        mesh_ota_master_next_st_set(MASTER_OTA_ST_BLOB_TRANSFER_GET);
	        }
			break;
			
		case MASTER_OTA_ST_BLOB_TRANSFER_GET:
		    if(distr_proc->node_num < distr_proc->node_cnt){
		        if(mesh_ota_check_skip_current_node()){ break;}
		        
    	        if(0 == access_cmd_blob_transfer_get(master_ota_current_node_adr)){
    	            mesh_ota_master_wait_ack_st_set();
    	        }
	        }else{
                distr_proc->node_num = 0;
    	        mesh_ota_master_next_st_set(MASTER_OTA_ST_BLOB_INFO_GET);
	        }
			break;
			
		case MASTER_OTA_ST_BLOB_INFO_GET:
		    if(distr_proc->node_num < distr_proc->node_cnt){
		        if(mesh_ota_check_skip_current_node()){ break;}
		        
    	        if(0 == access_cmd_blob_info_get(master_ota_current_node_adr)){
    	            mesh_ota_master_wait_ack_st_set();
    	        }
	        }else{
                distr_proc->node_num = 0;
    	        mesh_ota_master_next_st_set(MASTER_OTA_ST_BLOB_TRANSFER_START);
	        }
			break;
			
		case MASTER_OTA_ST_BLOB_TRANSFER_START:
		    if(distr_proc->node_num < distr_proc->node_cnt){
		        if(mesh_ota_check_skip_current_node()){ break;}
		        
    	        if(0 == access_cmd_blob_transfer_start(master_ota_current_node_adr, new_fw_size, TEST_BK_SIZE_LOG)){
    	            mesh_ota_master_wait_ack_st_set();
    	        }
	        }else{
                distr_proc->node_num = 0;
    	        mesh_ota_master_next_st_set(MASTER_OTA_ST_BLOB_BLOCK_START);
	        }
			break;
			
		case MASTER_OTA_ST_BLOB_BLOCK_START:
		{
            u16 block_num_current = fw_distribut_srv_proc.block_start.block_num;
		    if(block_num_current < distribut_get_fw_block_cnt()){
    		    if(distr_proc->node_num < distr_proc->node_cnt){
                    if(mesh_ota_check_skip_current_node()){ break;}
                    
                    #if 0 // BLOCK_CRC32_CHECKSUM_EN
    		        u32 adr = distribut_get_fw_data_position(0);
    		        u16 size = distribut_get_block_size(block_num_current);
					u32 crc =0;
					#if !WIN32
					if(block_num_current == 0){
						u8 crc_buf[16];
						flash_read_page(ota_program_offset,sizeof(crc_buf),crc_buf);
						crc_buf[8]= get_fw_ota_value();
						crc = soft_crc32_telink(crc_buf ,sizeof(crc_buf), 0);
						crc = soft_crc32_ota_flash(sizeof(crc_buf),size-16,crc,0);
					}else
					#endif
					{
						#if GATEWAY_ENABLE
						crc = soft_crc32_ota_flash(adr,size,0,0);
						#else
						crc = soft_crc32_telink(fw_ota_data_tx + adr, size, 0);
						#endif
					}
					#endif
					
        	        if(0 == access_cmd_blob_block_start(master_ota_current_node_adr, block_num_current)){
                        mesh_ota_master_wait_ack_st_set();
        	        }
    	        }else{
        	        mesh_ota_master_next_st_set(MASTER_OTA_ST_BLOB_BLOCK_START_CHECK_RESULT);
    	        }
	        }else{
                distr_proc->node_num = 0;
                mesh_ota_master_next_st_set(MASTER_OTA_ST_UPDATE_GET);
	        }
			break;
		}
			
		case MASTER_OTA_ST_BLOB_BLOCK_START_CHECK_RESULT:
            if(is_need_block_transfer()){
                distr_proc->chunk_num = 0;
                set_bit_by_cnt(distr_proc->miss_mask, sizeof(distr_proc->miss_mask), distribut_get_fw_chunk_cnt()); // all need send
    	        mesh_ota_master_next_st_set(MASTER_OTA_ST_BLOB_CHUNK_START);
	        }else{
	            mesh_ota_master_next_block();
	        }
			break;
			
		case MASTER_OTA_ST_BLOB_CHUNK_START:
		{
		    u32 chunk_cnt = distribut_get_fw_chunk_cnt();
		    if(distr_proc->chunk_num < chunk_cnt){
		        if(is_buf_bit_set(distr_proc->miss_mask, distr_proc->chunk_num)){
                    blob_chunk_transfer_t cmd = {0};
                    cmd.chunk_num = distr_proc->chunk_num;
                    u16 size = distribut_get_chunk_size(cmd.chunk_num);
                    if(size > MESH_OTA_CHUNK_SIZE){
                        size = MESH_OTA_CHUNK_SIZE;
                    }

                    u32 fw_pos = 0;
					u8 *data =0;
					u16 block_num_current = fw_distribut_srv_proc.block_start.block_num;
					
					#if !WIN32
					if(block_num_current == 0 && cmd.chunk_num == 0){
						u8 first_chunk[MESH_OTA_CHUNK_SIZE];
						flash_read_page(ota_program_offset,sizeof(first_chunk),first_chunk);
						first_chunk[8] = get_fw_ota_value();
						data = first_chunk;
						memcpy(cmd.data, data, size);
					}else
					#endif
					{
						fw_pos = distribut_get_fw_data_position(cmd.chunk_num);
                        mesh_ota_tx_fw_data_read(fw_pos, size, cmd.data);
					}
					
                    u16 bk_total = distribut_get_fw_block_cnt();
                    u8 percent = 1 + (fw_pos+size)*98/distr_proc->blob_size;
                    if(percent > distr_proc->percent_last){
                        distr_proc->percent_last = percent;
                        APP_RefreshProgressBar(block_num_current, bk_total, distr_proc->chunk_num, chunk_cnt, percent);
                    }

                    if(fw_distribut_srv_proc.miss_chunk_test_flag && (6 == distr_proc->chunk_num)){
                        LOG_MSG_ERR (TL_LOG_COMMON, 0, 0, "----OTA,missing chunk test: %2d----", distr_proc->chunk_num);
    		            distr_proc->chunk_num++;
    		            fw_distribut_srv_proc.miss_chunk_test_flag = 0;
                    }else if(0 == access_cmd_blob_chunk_transfer(fw_distribut_srv_proc.adr_group, (u8 *)&cmd, size+2)){
    		            distr_proc->chunk_num++;
    		        }
		        }else{
		            distr_proc->chunk_num++;
		        }
	        }else{
	            distr_proc->node_num = distr_proc->chunk_num = 0;
	            memset(distr_proc->miss_mask, 0, sizeof(distr_proc->miss_mask));
	            mesh_ota_master_next_st_set(MASTER_OTA_ST_BLOB_BLOCK_GET);
	        }
	    }
			break;
			
		case MASTER_OTA_ST_BLOB_BLOCK_GET:
		    if(distr_proc->node_num < distr_proc->node_cnt){
		        if(mesh_ota_check_skip_current_node()){ break;}
		        
    	        if(0 == access_cmd_blob_block_get(master_ota_current_node_adr, distr_proc->block_start.block_num)){
    	            mesh_ota_master_wait_ack_st_set();
    	        }
	        }else{
	            if(0 == is_buf_zero(distr_proc->miss_mask, sizeof(distr_proc->miss_mask))){
                    distr_proc->chunk_num = 0;
                    LOG_MSG_INFO (TL_LOG_CMD_NAME, 0, 0, "access_cmd_blob_chunk_transfer retry",0);
                    mesh_ota_master_next_st_set(MASTER_OTA_ST_BLOB_CHUNK_START);
	            }else{
                    mesh_ota_master_next_block();
    	        }
	        }
			break;
			
		case MASTER_OTA_ST_UPDATE_GET:
		    if(distr_proc->node_num < distr_proc->node_cnt){
		        if(mesh_ota_check_skip_current_node()){ break;}
		        
    	        if(0 == access_cmd_fw_update_get(master_ota_current_node_adr)){
    	            mesh_ota_master_wait_ack_st_set();
    	        }
	        }else{
                distr_proc->node_num = 0;
                mesh_ota_master_next_st_set(MASTER_OTA_ST_UPDATE_APPLY);
	        }
			break;
			
		case MASTER_OTA_ST_UPDATE_APPLY:
		    if(distr_proc->node_num < distr_proc->node_cnt){
		        //if(mesh_ota_check_skip_current_node()){ break;}
		        fw_detail_list_t *p_list = &distr_proc->list[distr_proc->node_num];
		        if(p_list->apply_flag
		         || ((p_list->adr == APP_get_GATT_connect_addr()) && (distribut_get_not_apply_cnt() > 1))){
		            distr_proc->node_num++;
		            break ;
		        }
		        
		        p_list->apply_flag = 1;
		        
		        u16 op;
		        if(UPDATE_PHASE_VERIFYING_SUCCESS == p_list->update_phase){
		            op = FW_UPDATE_APPLY;
		        }else{
		            op = FW_UPDATE_CANCEL;
		        }
		        
    	        if(0 == access_cmd_fw_update_control(master_ota_current_node_adr, op, 1)){
    	            mesh_ota_master_wait_ack_st_set();
    	        }
	        }else{
                distr_proc->node_num = 0;
                if(distribut_get_not_apply_cnt() == 0){
                    mesh_ota_master_next_st_set(MASTER_OTA_ST_DISTRIBUT_STOP);
                    APP_print_connected_addr();
                }else{
                    mesh_ota_master_next_st_set(MASTER_OTA_ST_UPDATE_APPLY);
                }
	        }
			break;
			
		case MASTER_OTA_ST_DISTRIBUT_STOP:
			#if GATEWAY_ENABLE
			{
				fw_detail_list_t *p_list = fw_distribut_srv_proc.list;
				u8 mesh_ota_sts[1+2*MESH_OTA_UPDATE_NODE_MAX];
				u8 mesh_ota_idx =0;
				mesh_ota_sts[0]=0;
				for(int i=0;i<fw_distribut_srv_proc.node_cnt;i++){
					p_list = &(fw_distribut_srv_proc.list[i]);
					if(p_list->skip_flag){
						mesh_ota_sts[0]++;
						memcpy(mesh_ota_sts+1+2*mesh_ota_idx,(u8 *)&(p_list->adr),2);
						mesh_ota_idx++;
					}
				}
				gateway_upload_mesh_ota_sts(mesh_ota_sts,1+(mesh_ota_sts[0])*2);
			}
			#endif
			{
				u32 st_back = distr_proc->st;
				mesh_ota_master_next_st_set(MASTER_OTA_ST_MAX);	// must set before tx cmd, because gateway use it when rx this command.
		        if(0 == access_cmd_fw_distribut_cancel(distr_proc->adr_distr_node)){
		            // no need, ota flow have been stop in mesh_cmd_sig_fw_distribut_cancel(),
	                APP_RefreshProgressBar(0, 0, 0, 0, 100);
		        }else{
		        	mesh_ota_master_next_st_set(st_back);
		        }
			}
			break;

		default :
		    distr_proc->node_num = distr_proc->node_num;
			break;
	}
}
#else
int mesh_ota_master_rx (mesh_rc_rsp_t *rsp, u16 op, u32 size_op){return 0;}
void mesh_ota_master_proc(){}
#endif
#endif


/***************************************** 
------- for updater node
******************************************/
#if 1
fw_update_srv_proc_t    fw_update_srv_proc = {0};         // for updater
#if PTS_TEST_OTA_EN
u8 A_1_blob_tranfer_BV_19_C_phase_inactive = 0;
u8 A_2_blob_tranfer_BV_31_C  = 0;
#endif

#if BLOB_TRANSFER_WITHOUT_FW_UPDATE_EN
const u8 BLOB_ID_LOCAL[8] = {0x11,0x00,0x00,0x00,0x00,0x00,0x00,0x11};
#endif

void mesh_ota_save_data(u32 adr, u32 len, u8 * data){
#if WIN32
    #if DISTRIBUTOR_UPDATE_CLIENT_EN
	if (adr == 0){
	    fw_update_srv_proc.reboot_flag_backup = data[8];
		data[8] = 0xff;					//FW flag invalid
	}
	#if VC_APP_ENABLE
    memcpy(fw_ota_data_rx + adr, data, len);
	#endif
    #endif
#else
    #if(__TL_LIB_8267__ || (MCU_CORE_TYPE && MCU_CORE_TYPE == MCU_CORE_8267) || \
	    __TL_LIB_8269__ || (MCU_CORE_TYPE && MCU_CORE_TYPE == MCU_CORE_8269) ||	\
	    __TL_LIB_8258__ || (MCU_CORE_TYPE && MCU_CORE_TYPE == MCU_CORE_8258) || \
	    (MCU_CORE_TYPE == MCU_CORE_8278))
	if (adr == 0){
	    fw_update_srv_proc.reboot_flag_backup = data[8];
		data[8] = 0xff;					//FW flag invalid
	}
    #endif
	flash_write_page(ota_program_offset + adr, len, data);
#endif
}

u32 soft_crc32_ota_flash(u32 addr, u32 len, u32 crc_init,u32 *out_crc_type1_blk)
{
    u32 crc_type1_blk = 0;
    u8 buf[64];      // CRC_SIZE_UNIT
    while(len){
        u32 len_read = (len > sizeof(buf)) ? sizeof(buf) : len;
        #if 1
        mesh_ota_read_data(addr, len_read, buf);
        #else
        flash_read_page(addr, len_read, buf);
        #endif
        if(0 == addr){
            buf[8] = fw_update_srv_proc.reboot_flag_backup;
        }
        
        #if BLOCK_CRC32_CHECKSUM_EN
        crc_init = soft_crc32_telink(buf, len_read, crc_init);
        #endif
        
		if(out_crc_type1_blk){
        	crc_type1_blk += get_blk_crc_tlk_type1(buf, len_read, addr);	// use to get total crc of total firmware.
		}
        
        len -= len_read;
        addr += len_read;
    }
    if(out_crc_type1_blk){
		*out_crc_type1_blk = crc_type1_blk;
	}
    return crc_init;
}

int is_valid_mesh_ota_len(u32 fw_len)
{
    #if FW_ADD_BYTE_EN
    u32 len_org = get_fw_len();
	return ((fw_len >= len_org) && (fw_len <= len_org + 20));
	#else
	return (fw_len == get_fw_len());
	#endif
}

int is_valid_telink_fw_flag()
{
    u8 fw_flag_telink[4] = {0x4B,0x4E,0x4C,0x54};
    u8 fw_flag[4] = {0};
    mesh_ota_read_data(8, sizeof(fw_flag), fw_flag);
    fw_flag[0] = fw_update_srv_proc.reboot_flag_backup;
	if(!memcmp(fw_flag,fw_flag_telink, 4) && is_valid_mesh_ota_len(fw_update_srv_proc.blob_size)){
		return 1;
	}
	return 0;
}

enum{
    BIN_CRC_DONE_NONE       = 0,    // must 0
    BIN_CRC_DONE_OK         = 1,
    BIN_CRC_DONE_FAIL       = 2,
};

int is_valid_mesh_ota_calibrate_val()
{
    // eclipse crc32 calibrate
    #if (0 == DEBUG_EVB_EN)
    if(!is_valid_telink_fw_flag()){
        return 0;
    }
    #endif

	if(OTA_CHECK_TYPE_TELINK_MESH == get_ota_check_type()){
	    if(0 == fw_update_srv_proc.bin_crc_done){
    	    u32 len = fw_update_srv_proc.blob_size;
    		int crc_ok = (is_valid_mesh_ota_len(len) 
    		          && (fw_update_srv_proc.crc_total == get_total_crc_type1_new_fw()));  // is_valid_ota_check_type1()
    		          
    		fw_update_srv_proc.bin_crc_done = crc_ok ? BIN_CRC_DONE_OK : BIN_CRC_DONE_FAIL;
		}
        return (BIN_CRC_DONE_OK == fw_update_srv_proc.bin_crc_done);
	}
	return 1;
}


inline u16 updater_get_fw_block_cnt()
{
    return get_fw_block_cnt(fw_update_srv_proc.blob_size, fw_update_srv_proc.bk_size_log);
}

inline u16 updater_get_block_size(u16 block_num)
{
    return get_block_size(fw_update_srv_proc.blob_size, fw_update_srv_proc.bk_size_log, block_num);
}

inline u16 updater_get_fw_chunk_cnt()
{
    blob_block_start_t *bk_start = &fw_update_srv_proc.block_start;
    return get_fw_chunk_cnt(fw_update_srv_proc.bk_size_current, bk_start->chunk_size);
}

inline u16 updater_get_chunk_size(u16 chunk_num)
{
    blob_block_start_t *bk_start = &fw_update_srv_proc.block_start;
    return get_chunk_size(fw_update_srv_proc.bk_size_current, bk_start->chunk_size, chunk_num);
}

inline u32 updater_get_fw_data_position(u16 chunk_num)
{
    blob_block_start_t *bk_start = &fw_update_srv_proc.block_start;
    return get_fw_data_position(bk_start->block_num, fw_update_srv_proc.bk_size_log, chunk_num, bk_start->chunk_size);
}

static u16 updater_get_block_rx_ok_cnt()
{
    u16 cnt = fw_update_srv_proc.block_start.block_num;
    if(fw_update_srv_proc.blob_block_rx_ok){
        cnt++;
    }
    return cnt;
}

int is_updater_blob_id_match(u8 *blob_id)
{
    return is_blob_id_match(fw_update_srv_proc.start.blob_id, blob_id);
}

int is_local_blob_id_match(u8 *blob_id)
{
#if BLOB_TRANSFER_WITHOUT_FW_UPDATE_EN
    if(!fw_update_srv_proc.busy){
        return is_blob_id_match(BLOB_ID_LOCAL, blob_id);
    }
#endif
    return is_updater_blob_id_match(blob_id);
}

void fw_update_srv_proc_init_keep_start_par_and_crc()
{
    fw_update_start_t start_backup;
    memcpy(&start_backup, &fw_update_srv_proc.start, sizeof(start_backup));
    u8 crc_backup = fw_update_srv_proc.bin_crc_done;
    memset(&fw_update_srv_proc, 0, sizeof(fw_update_srv_proc));
    memcpy(&fw_update_srv_proc.start, &start_backup, sizeof(fw_update_srv_proc.start)); // don't clear to handle retransmit here 
    if(BIN_CRC_DONE_OK == crc_backup){				  // sometimes need to caculate crc twice
        fw_update_srv_proc.bin_crc_done = crc_backup; // sometimes apply twice
    }
}

void blob_block_erase(u16 block_num)
{
    // attention: block size may not integral multiple of 4K,
}

void mesh_ota_proc()
{
#if PTS_TEST_OTA_EN
    #if MESH_OTA_BLOB_START_TIMEOUT_MS
    if(fw_update_srv_proc.blob_trans_start_tick
    && clock_time_exceed(fw_update_srv_proc.blob_trans_start_tick, MESH_OTA_BLOB_START_TIMEOUT_MS * 1000)){
        fw_update_srv_proc.blob_trans_start_tick = 0;
        if(BLOB_TRANS_PHASE_WAIT_NEXT_BLOCK == fw_update_srv_proc.blob_trans_phase){
            fw_update_srv_proc.blob_trans_phase = BLOB_TRANS_PHASE_SUSPEND;
            LOG_MSG_ERR(TL_LOG_COMMON,0, 0 ,"blob transfer timeout--",0);
        }
    }
    #endif
#else
    // no blob transfer timeout now
#endif
}

//---------
int mesh_cmd_sig_fw_update_info_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    fw_update_info_get_t *p_get = (fw_update_info_get_t *)par;
	model_g_light_s_t *p_model = (model_g_light_s_t *)cb_par->model;
	fw_update_info_status_t rsp = {0};
	u32 len_rsp = sizeof(rsp);
    rsp.first_index = p_get->first_index;
    rsp.list_count = 1;
    if(0 == p_get->first_index){
    	rsp.fw_id_len = sizeof(rsp.fw_id);
    	memcpy(&rsp.fw_id, &fw_id_local, sizeof(rsp.fw_id));
        rsp.uri_len = 0;
    }else{
        // only one entry now. 
        // Tell client there is no any more firmware entry by setting list count to be 1 and no firmware entry.
        len_rsp = OFFSETOF(fw_update_info_status_t, fw_id_len);
    }
	
	return mesh_tx_cmd_rsp(FW_UPDATE_INFO_STATUS, (u8 *)&rsp, len_rsp, p_model->com.ele_adr, cb_par->adr_src, 0, 0);
}

int mesh_cmd_sig_fw_update_info_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

//---------
int mesh_cmd_sig_fw_update_metadata_check(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    fw_update_metadata_check_t *p_check = (fw_update_metadata_check_t *)par;
    u8 st;
    if(0 == p_check->image_index){ // only one now
        u8 metadata_len = par_len - OFFSETOF(fw_update_metadata_check_t,metadata);
        if(is_valid_metadata_len(metadata_len)
        && mesh_ota_slave_need_ota(&p_check->metadata, metadata_len)){
            memcpy(&fw_update_srv_proc.start.metadata, &p_check->metadata, metadata_len);
            fw_update_srv_proc.metadata_len = metadata_len;
            // may check meta data again when update start.
            st = UPDATE_ST_SUCCESS;
        }else{
            st = UPDATE_ST_METADATA_CHECK_FAIL;
        }
    }else{
        st = UPDATE_ST_WRONG_FW_INDEX;
    }
    
	model_g_light_s_t *p_model = (model_g_light_s_t *)cb_par->model;
	fw_update_metadata_check_status_t rsp = {0};
	rsp.st = st;
	rsp.additional_info = ADDITIONAL_CPS_NO_CHANGE; // TBD
	rsp.image_index = p_check->image_index;
	
	return mesh_tx_cmd_rsp(FW_UPDATE_METADATA_CHECK_STATUS, (u8 *)&rsp, sizeof(rsp), p_model->com.ele_adr, cb_par->adr_src, 0, 0);
}

int mesh_cmd_sig_fw_update_metadata_check_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

// -------
void get_fw_update_rsp_data(fw_update_status_t *p_rsp, fw_update_start_t *p_start)
{
    p_rsp->ttl = p_start->ttl;
    p_rsp->timeout_base = p_start->timeout_base;
    memcpy(p_rsp->blob_id, p_start->blob_id,sizeof(p_rsp->blob_id));
    p_rsp->image_index = p_start->image_index;
}

/**
 * @brief  
 * @par_err: response error data
 * @retval 
 */
int mesh_fw_update_st_rsp(mesh_cb_fun_par_t *cb_par, u8 st, u8 *par_err, int par_len)
{
	model_g_light_s_t *p_model = (model_g_light_s_t *)cb_par->model;
	fw_update_status_t rsp = {0};
	fw_update_srv_proc.blob_block_get_retry_flag = 0; // init
	rsp.st = st;
    if(fw_update_srv_proc.busy && (fw_update_srv_proc.blob_trans_rx_start_error
                            || (fw_update_srv_proc.busy && (BLOB_TRANS_PHASE_SUSPEND == fw_update_srv_proc.blob_trans_phase)))){
        // MMDL/SR/FU/BV-13-C Receive Firmware Update Cancel-Transfer Error Phase
        rsp.update_phase = UPDATE_PHASE_TRANSFER_ERR;
    }else{
        rsp.update_phase = fw_update_srv_proc.update_phase;
    }
    
	u32 rsp_len = sizeof(fw_update_status_t);
	if((!fw_update_srv_proc.busy) // (UPDATE_ST_SUCCESS != st )
#if 0 // (0 == PTS_TEST_OTA_EN)
    // MMDL/SR/FU/BV-03-C Firmware Update-Verification Succeeded: need option par.
	 || (UPDATE_PHASE_IDLE == rsp.update_phase)
	 || (UPDATE_PHASE_VERIFYING_SUCCESS == rsp.update_phase) || (UPDATE_PHASE_VERIFYING_FAIL == rsp.update_phase)
#else
     // MMDL/SR/FU/BV-03-C Firmware Update-Verification Succeeded: need option par.
     && (!((UPDATE_PHASE_APPLYING_UPDATE == rsp.update_phase) || (UPDATE_PHASE_VERIFYING_FAIL == rsp.update_phase)))
#endif
	 ){
	    // node will reboot when receive apply, so unsegment status should be better
	    rsp_len -= sizeof(fw_update_status_t) - OFFSETOF(fw_update_status_t,ttl);   // no blob_id
	}else{
	    if(par_err){
	        if(FW_UPDATE_START == cb_par->op){
                get_fw_update_rsp_data(&rsp, (fw_update_start_t *)par_err);
	        }else{
	            // should not happen here.
                rsp_len -= sizeof(fw_update_status_t) - OFFSETOF(fw_update_status_t,ttl);
	        }
	    }else{
            get_fw_update_rsp_data(&rsp, &fw_update_srv_proc.start);
        }
		rsp.additional_info = fw_update_srv_proc.additional_info;
	}

	return mesh_tx_cmd_rsp(FW_UPDATE_STATUS, (u8 *)&rsp, rsp_len, p_model->com.ele_adr, cb_par->adr_src, 0, 0);
}

int mesh_cmd_sig_fw_update_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	u8 st = UPDATE_ST_SUCCESS;
	if(fw_update_srv_proc.busy){
	    if(fw_update_srv_proc.blob_trans_rx_start_error || (0 == fw_update_srv_proc.blob_size)){
	        // 
	    }else if(updater_get_block_rx_ok_cnt() == updater_get_fw_block_cnt()){// all block rx ok
            // MMDL/SR/FU/BV-03-C get verify value here, but not apply.
            int cali_ok = is_valid_mesh_ota_calibrate_val();
            fw_update_srv_proc.update_phase = cali_ok ? UPDATE_PHASE_VERIFYING_SUCCESS : UPDATE_PHASE_VERIFYING_FAIL;
        }
	}else{
	}
	return mesh_fw_update_st_rsp(cb_par, st, 0, 0);
}

int mesh_ota_slave_need_ota(fw_metadata_t *p_metadata, int len)
{
    #if (PTS_TEST_OTA_EN && !WIN32)
    if(sizeof(BLOB_ID_LOCAL) == len){ // meta data in PTS is set to be same with BLOB ID now.
        return (0 == memcmp(BLOB_ID_LOCAL, p_metadata, sizeof(BLOB_ID_LOCAL)));
    }
    return 0;
    #else
    if(sizeof(fw_metadata_t) == len){
        #if 0
        // TBD policy
        int accept = 0;
        if(p_metadata->fw_id.pid == fw_id_local.pid){
            u8 vid_new_1st = p_metadata->fw_id.vid & 0xFF;
            u8 vid_new_2nd = (p_metadata->fw_id.vid >> 8) & 0xFF;
            u8 vid_local_1st = fw_id_local.vid & 0xFF;
            u8 vid_local_2nd = (fw_id_local.vid >> 8) & 0xFF;
            if((vid_new_1st > vid_local_1st)
            || ((vid_new_1st == vid_local_1st)&&(vid_new_2nd > vid_local_2nd))){
                accept = 1;
            }
        }
        return accept;
        #else
        return 1;
        #endif
    }else{
        return 1;   // always valid now // return 0;
    }
    #endif
}

int mesh_cmd_sig_fw_update_start(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	u8 st;
    fw_update_start_t *p_start = (fw_update_start_t *)par;
    u8 *par_err = 0;
    u8 metadata_len = par_len - OFFSETOF(fw_update_start_t,metadata);
    int force_flag = (UPDATE_PHASE_VERIFYING_FAIL == fw_update_srv_proc.update_phase);
    if((!force_flag) && ((fw_update_srv_proc.busy
          && (!fw_update_srv_proc.blob_trans_rx_start_error)) // MMDL/SR/FU/BV-17-C Receive Firmware Update Start-Transfer Error Phase
     ||(UPDATE_PHASE_VERIFYING_SUCCESS == fw_update_srv_proc.update_phase)
     ||(UPDATE_PHASE_APPLYING_UPDATE == fw_update_srv_proc.update_phase))){
        if((metadata_len != fw_update_srv_proc.metadata_len)
        || (0 != memcmp(&fw_update_srv_proc.start, par, par_len))){
        	st = UPDATE_ST_WRONG_PHASE;
        	par_err = par;
        }else{
            fw_update_srv_proc.update_phase = UPDATE_PHASE_TRANSFER_ACTIVE;
        	st = UPDATE_ST_SUCCESS;
        }
    }else{
        fw_update_srv_proc.blob_trans_rx_start_error = 0; // init
        /* must check again but not just compare meta data with the data reveived in meta data check.
            because master may skip the meta data check to pass the check*/
        if((!force_flag) && fw_update_srv_proc.blob_trans_busy){
    	    st = UPDATE_ST_BLOB_TRANSFER_BUSY;
        }else if(is_valid_metadata_len(metadata_len) // meta data over flow
	    && mesh_ota_slave_need_ota(&p_start->metadata, metadata_len)){
            #if (DUAL_MODE_ADAPT_EN || DUAL_MODE_WITH_TLK_MESH_EN)
            dual_mode_disable();
            // bls_ota_clearNewFwDataArea(); // may disconnect
            #endif
            memset(&fw_update_srv_proc, 0, sizeof(fw_update_srv_proc));
            memcpy(&fw_update_srv_proc.start, p_start, sizeof(fw_update_start_t)); // par_len
            fw_update_srv_proc.metadata_len = metadata_len; // take it as real meta data, but not that in metadata check
            fw_update_srv_proc.update_phase = UPDATE_PHASE_TRANSFER_ACTIVE;
            fw_update_srv_proc.blob_trans_phase = BLOB_TRANS_PHASE_WAIT_START;
            fw_update_srv_proc.busy = 1;
            st = UPDATE_ST_SUCCESS;
	    }else{
    	    st = UPDATE_ST_METADATA_CHECK_FAIL;
	    }
	}
	
	return mesh_fw_update_st_rsp(cb_par, st, par_err, par_len);
}

int is_fw_update_start_before()
{
    if(fw_update_srv_proc.busy || fw_update_srv_proc.bin_crc_done){
        return 1;
    }
    
    fw_update_start_t zero = {0};
    return (0 != memcmp(&fw_update_srv_proc.start, &zero, OFFSETOF(fw_update_start_t,metadata)));
}

int mesh_cmd_sig_fw_update_apply(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	u8 st = UPDATE_ST_WRONG_PHASE;
    if(is_fw_update_start_before()){
        if((UPDATE_PHASE_VERIFYING_UPDATE == fw_update_srv_proc.update_phase)
        || (UPDATE_PHASE_VERIFYING_SUCCESS == fw_update_srv_proc.update_phase) // refresh reboot tick
        || (UPDATE_PHASE_VERIFYING_FAIL == fw_update_srv_proc.update_phase) // refresh reboot tick
        || (UPDATE_PHASE_APPLYING_UPDATE == fw_update_srv_proc.update_phase) // refresh reboot tick
        ){
            int cali_ok = 0;
            if((BIN_CRC_DONE_OK == fw_update_srv_proc.bin_crc_done)
             || is_valid_mesh_ota_calibrate_val()){
                #if DISTRIBUTOR_UPDATE_CLIENT_EN
				#if VC_APP_ENABLE
                fw_ota_data_rx[8] = fw_update_srv_proc.reboot_flag_backup;
                new_fw_write_file(fw_ota_data_rx, fw_update_srv_proc.blob_size);
				#endif
				#else
                mesh_ota_reboot_set((fw_update_srv_proc.blob_size > 256) ? OTA_SUCCESS : OTA_SUCCESS_DEBUG);
                #endif
                cali_ok = 1;
            }else{
                mesh_ota_reboot_set(OTA_DATA_CRC_ERR);
            }

            fw_update_srv_proc_init_keep_start_par_and_crc();
            
            if(cali_ok){
                fw_update_srv_proc.update_phase = UPDATE_PHASE_APPLYING_UPDATE;
                st = UPDATE_ST_SUCCESS;
            }else{
                fw_update_srv_proc.update_phase = UPDATE_PHASE_VERIFYING_FAIL;
                st = UPDATE_ST_WRONG_PHASE;
            }
        }else{
            st = UPDATE_ST_WRONG_PHASE;
        }
    }else{
	    st = UPDATE_ST_WRONG_PHASE; // not receive start before
	}
	
    mesh_ota_reboot_check_refresh();
	return mesh_fw_update_st_rsp(cb_par, st, 0, 0);
}

int mesh_cmd_sig_fw_update_cancel(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	u8 st = UPDATE_ST_INTERNAL_ERROR;
    if(is_fw_update_start_before()){
        fw_update_srv_proc_init_keep_start_par_and_crc();
        // fw_update_srv_proc.busy = 0; // have been clear in init.
        #if (PTS_TEST_OTA_EN) // PTS send cancel at the beginning of every case, so no need reboot in some case.
        if(fw_update_srv_proc.blob_trans_busy || fw_update_srv_proc.bin_crc_done)
        #endif
        {
            mesh_ota_reboot_set(OTA_DATA_CRC_ERR);
        }
        st = UPDATE_ST_SUCCESS;
    }else{
	    st = UPDATE_ST_SUCCESS; // UPDATE_ST_INTERNAL_ERROR;    // not receive start before
	}
	
    mesh_ota_reboot_check_refresh();
	return mesh_fw_update_st_rsp(cb_par, st, 0, 0);
}

int mesh_cmd_sig_fw_update_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

//------
u32 set_bit_buf(u8 *buf, u32 buf_len, u32 start_bit, u32 bit_max)
{
    // buf have been set 0 before.
    u32 i = start_bit / CHAR_BIT;
    if(start_bit < bit_max){
        bit_max = min(bit_max, buf_len * 8);
        u32 bit_start_1st_byte = (start_bit % CHAR_BIT);
        u32 ns = CHAR_BIT - bit_start_1st_byte; // (addr & 0xff);
        while(start_bit < bit_max){
            u32 len = bit_max - start_bit;
            u32 nw = len > ns ? ns : len;
            if(bit_start_1st_byte){
                buf[i] = BIT_RNG(bit_start_1st_byte, bit_start_1st_byte + nw);
                bit_start_1st_byte = 0;
            }else{
                buf[i] = BIT_MASK_LEN(nw);
            }
            ns = CHAR_BIT;
            start_bit += nw;
            i++;
        }
        return i;
    }else{
        return min(CEIL_8(bit_max),buf_len);
    }
}

static inline int is_blob_st_optional_C2(u8 st)
{
    return ((BLOB_TRANS_ST_SUCCESS == st) || (BLOB_TRANS_ST_WRONG_PHASE == st));
}

int mesh_tx_cmd_blob_transfer_st_C1(mesh_cb_fun_par_t *cb_par, u8 st, u8 *p_blob_id)
{
    blob_transfer_status_t rsp = {0};
    rsp.st = st;
    // MMDL/SR/BT/BV-27-C: BLOB Transfer Start - Incompatible Parameters
    rsp.transfer_phase = BLOB_TRANS_PHASE_WAIT_START; //  PTS will discard message if phase is INACTIVE.
    memcpy(rsp.blob_id, p_blob_id, sizeof(rsp.blob_id)); // BLOB_ID_LOCAL
    u32 rsp_len = OFFSETOF(blob_transfer_status_t, blob_size); // PTS will discard message if include blob size.
    model_g_light_s_t *p_model = (model_g_light_s_t *)cb_par->model;
    u16 ele_adr = p_model->com.ele_adr;
    u16 dst_adr = cb_par->adr_src;
    return mesh_tx_cmd_rsp(BLOB_TRANSFER_STATUS, (u8 *)&rsp, rsp_len, ele_adr, dst_adr, 0, 0);
}

int mesh_tx_cmd_blob_transfer_st(mesh_cb_fun_par_t *cb_par, u8 st)
{
	model_g_light_s_t *p_model = (model_g_light_s_t *)cb_par->model;
    u16 ele_adr = p_model->com.ele_adr;
    u16 dst_adr = cb_par->adr_src;

	blob_transfer_status_t rsp = {0};
	rsp.st = st;
	rsp.transfer_mode = fw_update_srv_proc.transfer_mode;
	rsp.transfer_phase = fw_update_srv_proc.blob_trans_phase;

    u32 rsp_len = OFFSETOF(blob_transfer_status_t, blob_id);
    /*MMDL/SR/BT/BV-04-C(BLOB Transfer Timeout), BV-07-C(BLOB Transfer Cancel-IUT in Waiting for Next Block Phase) : no blob id for cancle command*/
    /*MMDL/SR/BT/BV-29-C(BLOB Transfer Cancel-Invalid Parameters), BLOB_TRANS_ST_WRONG_BLOB_ID need blob id if active before*/
	if(/*is_blob_st_optional_C2(st) && */(fw_update_srv_proc.blob_trans_busy)){
	    rsp_len = OFFSETOF(blob_transfer_status_t, bk_not_receive);
        memcpy(&rsp.blob_id, &fw_update_srv_proc.start.blob_id, sizeof(rsp.blob_id));
        // If the BLOB ID field is present, then the BLOB Size field may be present; may be but not should be.
        rsp.blob_size = fw_update_srv_proc.blob_size;
        rsp.bk_size_log = fw_update_srv_proc.bk_size_log;
        rsp.transfer_mtu_size = MESH_CMD_ACCESS_LEN_MAX;
        
        u32 block_max = CEIL_DIV(rsp.blob_size, (1 << rsp.bk_size_log));
        u32 block_current = updater_get_block_rx_ok_cnt();
        rsp_len += set_bit_buf(rsp.bk_not_receive, sizeof(rsp.bk_not_receive),block_current,block_max);
	}else if(fw_update_srv_proc.busy){
	    return mesh_tx_cmd_blob_transfer_st_C1(cb_par, st, fw_update_srv_proc.start.blob_id);
	}
	
	return mesh_tx_cmd_rsp(BLOB_TRANSFER_STATUS, (u8 *)&rsp, rsp_len, ele_adr, dst_adr, 0, 0);
}

int mesh_blob_transfer_st_rsp(mesh_cb_fun_par_t *cb_par, u8 st)
{
	return mesh_tx_cmd_blob_transfer_st(cb_par, st);
}

int mesh_cmd_sig_blob_transfer_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    // blob transfer get should always success.
    u8 st = BLOB_TRANS_ST_SUCCESS;//fw_update_srv_proc.blob_trans_busy ? BLOB_TRANS_ST_SUCCESS : BLOB_TRANS_ST_WRONG_PHASE;
	
	return mesh_blob_transfer_st_rsp(cb_par, st);
}

int is_blob_transfer_start_same(blob_transfer_start_t *p_start)
{
    // blob id have been checked before
    return ((p_start->transfer_mode == fw_update_srv_proc.transfer_mode)
          &&(p_start->blob_size == fw_update_srv_proc.blob_size)
          &&(p_start->bk_size_log == fw_update_srv_proc.bk_size_log)
          &&(p_start->client_mtu_size == fw_update_srv_proc.client_mtu_size));
}

int mesh_cmd_sig_blob_transfer_handle(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	u8 st = BLOB_TRANS_ST_WRONG_PHASE;
    if(BLOB_TRANSFER_WITHOUT_FW_UPDATE_EN || fw_update_srv_proc.busy){    // can blob start without update start before.
        if(BLOB_TRANSFER_WITHOUT_FW_UPDATE_EN || UPDATE_PHASE_TRANSFER_ACTIVE == fw_update_srv_proc.update_phase){
            if(BLOB_TRANSFER_START == cb_par->op){
                blob_transfer_start_t *p_start = (blob_transfer_start_t *)par;
                if(is_local_blob_id_match(p_start->blob_id)){
                    #if PTS_TEST_OTA_EN
                    if(A_1_blob_tranfer_BV_19_C_phase_inactive){ // just for Test now.
                        // MMDL/SR/BT/BV-19-C: IUT in Inactive Phase
                        fw_update_srv_proc.blob_trans_phase = BLOB_TRANS_PHASE_INACTIVE;
                        st = BLOB_TRANS_ST_WRONG_PHASE;
                    }else
                    #endif
                    {
                    if(fw_update_srv_proc.blob_trans_busy && !is_blob_transfer_start_same(p_start)){
                        st = BLOB_TRANS_ST_WRONG_PHASE;//MMDL/SR/BT/BV-14-C start twice; MMDL/SR/BT/BV-28-C use a different par.
                    }else if(p_start->blob_size > MESH_OTA_BLOB_SIZE_MAX){
                        st = BLOB_TRANS_ST_BLOB_TOO_LARGE;
                    }else if(p_start->bk_size_log > MESH_OTA_BLOCK_SIZE_LOG_MAX
                        || (p_start->bk_size_log < MESH_OTA_BLOCK_SIZE_LOG_MIN)){
                        st = BLOB_TRANS_ST_INVALID_BK_SIZE;
                    }else if(p_start->transfer_mode & (~ MESH_OTA_TRANSFER_MODE_SEL)){
                        if(p_start->transfer_mode & MESH_OTA_TRANSFER_MODE_SEL){
                            return -1; // should not response: MMDL/SR/BT/BI-02-C Prohibited Transfer Mode
                        }else{
                            st = BLOB_TRANS_ST_UNSUPPORT_TRANS_MODE;
                        }
                    }else{
                        #if BLOB_TRANSFER_WITHOUT_FW_UPDATE_EN
                        if(!fw_update_srv_proc.busy){
                            memcpy(fw_update_srv_proc.start.blob_id, p_start->blob_id, sizeof(fw_update_srv_proc.start.blob_id));
                        }
                        #endif
                        fw_update_srv_proc.transfer_mode = p_start->transfer_mode;
                        fw_update_srv_proc.blob_size = p_start->blob_size;
                        fw_update_srv_proc.bk_size_log = p_start->bk_size_log;
                        fw_update_srv_proc.client_mtu_size = p_start->client_mtu_size;
                        fw_update_srv_proc.blob_trans_busy = 1;
                        if((fw_update_srv_proc.blob_trans_phase == BLOB_TRANS_PHASE_WAIT_NEXT_CHUNK)
                        || (fw_update_srv_proc.blob_trans_phase == BLOB_TRANS_PHASE_COMPLETE)){
                            // MMDL/SR/BT/BV-15-C (BLOB Transfer Start-IUT in Waiting for Next Chunk Phase): if it's wait Chunk, keep BLOB_TRANS_PHASE_WAIT_NEXT_CHUNK
                        }else{
                            fw_update_srv_proc.blob_trans_phase = BLOB_TRANS_PHASE_WAIT_NEXT_BLOCK;
                        }
                        fw_update_srv_proc.blob_trans_start_tick = clock_time()|1;
                        st = BLOB_TRANS_ST_SUCCESS;
                    }
                    }
                }else{
                    st = BLOB_TRANS_ST_WRONG_BLOB_ID;
                }

                fw_update_srv_proc.blob_trans_rx_start_error = (st != BLOB_TRANS_ST_SUCCESS);
                if(!is_blob_st_optional_C2(st)){
                    return mesh_tx_cmd_blob_transfer_st_C1(cb_par, st, p_start->blob_id);
                }
            }else if(BLOB_TRANSFER_CANCEL == cb_par->op){
                blob_transfer_cancel_t *p_cancel = (blob_transfer_cancel_t *)par;
                if(is_updater_blob_id_match(p_cancel->blob_id)){
                    fw_update_srv_proc.blob_trans_busy = 0;
                    fw_update_srv_proc.blob_trans_rx_start_error = 0;
                    if(fw_update_srv_proc.busy){
                        // MMDL/SR/FU/BV-06-C Firmware Update-Cancel BLOB
                        fw_update_srv_proc.update_phase = UPDATE_PHASE_TRANSFER_ERR;
                    }
                    fw_update_srv_proc.blob_trans_phase = BLOB_TRANS_PHASE_INACTIVE;
                    fw_update_srv_proc.transfer_mode = 0; // must, MMDL/SR/BT/BV-07-C (BLOB Transfer Cancel-IUT in Waiting for Next Block Phase)
                    fw_update_srv_proc.blob_trans_start_tick = 0;
                    st = BLOB_TRANS_ST_SUCCESS;
                }else{
                    if(BLOB_TRANS_PHASE_INACTIVE == fw_update_srv_proc.blob_trans_phase){
                        st = BLOB_TRANS_ST_SUCCESS;
                    }else{
                        st = BLOB_TRANS_ST_WRONG_BLOB_ID;
                    }
                }
            }
    	}else{
    	    st = BLOB_TRANS_ST_WRONG_PHASE;    // TODO
    	}
    }else{       
	    st = BLOB_TRANS_ST_WRONG_PHASE;    // TODO
	}

	return mesh_blob_transfer_st_rsp(cb_par, st);
}

int mesh_cmd_sig_blob_transfer_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

//------
int mesh_tx_cmd_blob_block_st(u8 idx, u16 ele_adr, u16 dst_adr, u8 st, u16 block_num)
{
	blob_block_status_t rsp = {0};
	u32 rsp_len = OFFSETOF(blob_block_status_t, miss_chunk);    // not for block get, so no miss chunk.
	rsp.st = st;
	if(block_num < updater_get_block_rx_ok_cnt()){
        rsp.format = BLOB_BLOCK_FORMAT_NO_CHUNK_MISS;   // only response for block start now.
	}else{
        rsp.format = BLOB_BLOCK_FORMAT_ALL_CHUNK_MISS;  // only response for block start now.
	}
	#if (MESH_OTA_V_SEL == MESH_OTA_R04)
	rsp.transfer_phase = fw_update_srv_proc.blob_trans_phase;
	#endif
	if(0 == fw_update_srv_proc.block_start.chunk_size){
	    // MMDL/SR/BT/BV-20-C (BLOB Block Start-IUT in Idle Phase): never have received block start before
    	rsp.block_num = 0xffff;
    	rsp.chunk_size = 0xffff;
	}else{
    	rsp.block_num = fw_update_srv_proc.block_start.block_num;
    	rsp.chunk_size = fw_update_srv_proc.block_start.chunk_size;
	}
	return mesh_tx_cmd_rsp(BLOB_BLOCK_STATUS, (u8 *)&rsp, rsp_len, ele_adr, dst_adr, 0, 0);
}

int mesh_blob_block_st_rsp(mesh_cb_fun_par_t *cb_par, u8 st, u16 block_num)
{
	model_g_light_s_t *p_model = (model_g_light_s_t *)cb_par->model;
	return mesh_tx_cmd_blob_block_st(cb_par->model_idx, p_model->com.ele_adr, cb_par->adr_src, st, block_num);
}

u8 blob_block_start_par_check(blob_block_start_t *p_start)
{
	u8 st;// = BLOB_TRANS_ST_INTERNAL_ERROR;
    if(BLOB_TRANSFER_WITHOUT_FW_UPDATE_EN || fw_update_srv_proc.busy){
        if((BLOB_TRANSFER_WITHOUT_FW_UPDATE_EN || (UPDATE_PHASE_TRANSFER_ACTIVE == fw_update_srv_proc.update_phase))
        && fw_update_srv_proc.blob_trans_busy){
            u16 bk_size_max = (1 << fw_update_srv_proc.bk_size_log);
            int block_num_valid = p_start->block_num <= updater_get_block_rx_ok_cnt();
            if(!block_num_valid && ((fw_update_srv_proc.blob_trans_phase == BLOB_TRANS_PHASE_WAIT_NEXT_CHUNK) // MMDL/SR/BT/BV-21-C, start twice.
                                    || (fw_update_srv_proc.blob_trans_phase == BLOB_TRANS_PHASE_COMPLETE))){
                st = BLOB_TRANS_ST_WRONG_PHASE;
            }else if((p_start->chunk_size > MESH_OTA_CHUNK_SIZE_MAX)
            || (get_fw_chunk_cnt(bk_size_max, p_start->chunk_size) > MESH_OTA_CHUNK_NUM_MAX)){
                if(fw_update_srv_proc.blob_trans_phase == BLOB_TRANS_PHASE_WAIT_NEXT_CHUNK){
                    st = BLOB_TRANS_ST_WRONG_PHASE; // MMDL/SR/BT/BV-30-C: par not be same with before.
                }else{
                    st = BLOB_TRANS_ST_INVALID_CHUNK_SIZE;
                }
            }else{
                #if (0 == BLOCK_CRC32_CHECKSUM_EN)
                if(block_num_valid){
                    st = BLOB_TRANS_ST_SUCCESS;
                }else{
                    st = BLOB_TRANS_ST_INVALID_BK_NUM;
                }
                #else
                if(p_start->bk_check_sum_type == BLOB_BLOCK_CHECK_SUM_TYPE_CRC32){
                    if(p_start->block_num == updater_get_block_rx_ok_cnt()){
                        st = BLOB_BLOCK_TRANS_ST_ACCEPTED;
                    }else if(p_start->block_num < updater_get_block_rx_ok_cnt()){
                        st = BLOB_BLOCK_TRANS_ST_ALREADY_RX;
                    }else{
                        st = BLOB_BLOCK_TRANS_ST_INVALID_BK_NUM;
                    }
                }else{
                    st = BLOB_BLOCK_TRANS_ST_UNKNOWN_CHECK_SUM_TYPE;
                }
                #endif
            }
    	}else{
    	    st = BLOB_TRANS_ST_WRONG_PHASE;    // TODO
    	}
    }else{       
	    st = BLOB_TRANS_ST_WRONG_PHASE;    // TODO
	}

	return st;
}

int mesh_cmd_sig_blob_block_start(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    blob_block_start_t *p_start = (blob_block_start_t *)par;
    fw_update_srv_proc.bk_size_current = updater_get_block_size(p_start->block_num);
    
    u8 st = blob_block_start_par_check(p_start);
    if((BLOB_TRANS_ST_SUCCESS == st) && (p_start->block_num == updater_get_block_rx_ok_cnt())){
        memcpy(&fw_update_srv_proc.block_start, p_start, sizeof(fw_update_srv_proc.block_start));
        blob_block_erase(p_start->block_num);
        #if 0 // PTS_TEST_OTA_EN// only test for BLOB Transfer Server model, can't enable in FW update serve.
        if(0 == A_2_blob_tranfer_BV_31_C){ // MMDL/SR/BT/BV-31-C Chunk Transfer-Invalid Parameters must disable.
            memset(fw_update_srv_proc.miss_mask, 0, sizeof(fw_update_srv_proc.miss_mask));
        }else
        #endif
        {
            set_bit_by_cnt(fw_update_srv_proc.miss_mask, sizeof(fw_update_srv_proc.miss_mask), updater_get_fw_chunk_cnt());
        }
		fw_update_srv_proc.blob_block_rx_ok = 0; // init
        fw_update_srv_proc.blob_trans_phase = BLOB_TRANS_PHASE_WAIT_NEXT_CHUNK;
        fw_update_srv_proc.blob_trans_start_tick = 0;
        fw_update_srv_proc.blob_block_get_retry_flag = 0;
    }
	
	return mesh_blob_block_st_rsp(cb_par, st, p_start->block_num);
}

//------
int is_blob_chunk_transfer_ready()
{
    return ((BLOB_TRANSFER_WITHOUT_FW_UPDATE_EN || (fw_update_srv_proc.busy && (UPDATE_PHASE_TRANSFER_ACTIVE == fw_update_srv_proc.update_phase)))
            && fw_update_srv_proc.blob_trans_busy && (BLOB_TRANS_PHASE_WAIT_NEXT_CHUNK == fw_update_srv_proc.blob_trans_phase));
}

int mesh_cmd_sig_blob_chunk_transfer(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    blob_chunk_transfer_t *p_chunk = (blob_chunk_transfer_t *)par;
    int fw_data_len = par_len - 2;
    
    if(is_blob_chunk_transfer_ready() && ((fw_data_len > 0) && (fw_data_len <= MESH_OTA_CHUNK_SIZE))){
        //u32 bit_chunk = BIT(p_chunk->chunk_num);
        //LOG_MSG_LIB(TL_LOG_NODE_BASIC,0, 0,"chunk_num: 0x%04x",p_chunk->chunk_num);
        if((p_chunk->chunk_num + 1) * fw_update_srv_proc.block_start.chunk_size > (1 << fw_update_srv_proc.bk_size_log)){
            #if 0 // no need, just discard message.
            // MMDL/SR/BT/BV-31-C BLOB Chunk Transfer-Invalid Parameters: discard all that have been received
            set_bit_by_cnt(fw_update_srv_proc.miss_mask, sizeof(fw_update_srv_proc.miss_mask), updater_get_fw_chunk_cnt());
            LOG_MSG_ERR(TL_LOG_NODE_BASIC,0, 0,"invalid chunk number:",0);
            #endif
        }else if(p_chunk->chunk_num <= sizeof(fw_update_srv_proc.miss_mask)*8){
            #if 1 // VC_DISTRIBUTOR_UPDATE_CLIENT_EN
            #if 0   // test
            static volatile u8 skip_test = 0;
            if(skip_test){ // (p_chunk->chunk_num == 1)
                skip_test = 0;
                return 0;
            }
            #endif
            
            if(is_buf_bit_set(fw_update_srv_proc.miss_mask, p_chunk->chunk_num)){
                buf_bit_clear(fw_update_srv_proc.miss_mask, p_chunk->chunk_num);
                u32 pos = updater_get_fw_data_position(p_chunk->chunk_num);
                mesh_ota_save_data(pos, fw_data_len, p_chunk->data);
                fw_update_srv_proc.bin_crc_done = 0;
            }
            #endif
        }
    }
    return 0;
}

//------
/**
 * @retval  return have not been receive "block get" before when BLOCK_CRC32_CHECKSUM_EN is 0,
 *             return crc valid when BLOCK_CRC32_CHECKSUM_EN is 1,
 */
int block_crc32_check_current(u32 check_val)
{
    u32 adr = updater_get_fw_data_position(0);
    u32 crc_type1_blk = 0;
    u32 crc32_cal = 0;
    crc32_cal = soft_crc32_ota_flash(adr, fw_update_srv_proc.bk_size_current, 0,&crc_type1_blk);
    #if BLOCK_CRC32_CHECKSUM_EN
    if(check_val == crc32_cal)
    #endif
    {
        // for telink mesh crc
        u8 *mask = fw_update_srv_proc.blk_crc_tlk_mask;
        u16 blk_num = fw_update_srv_proc.block_start.block_num; // have make sure blk_num is valid before.
        if(!is_array_mask_en(mask, blk_num)){
            fw_update_srv_proc.crc_total += crc_type1_blk;
            set_array_mask_en(mask, blk_num);
        }
        #if (0 == BLOCK_CRC32_CHECKSUM_EN)
        else{
            return 0;
        }
        #endif
        
        return 1;
    }
    
    return (BLOCK_CRC32_CHECKSUM_EN ? 0 : 1);
}

int mesh_cmd_sig_blob_block_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	blob_block_status_t rsp = {0};
	u8 st;
	u32 rsp_len = OFFSETOF(blob_block_status_t, miss_chunk);
	if(is_blob_chunk_transfer_ready() || fw_update_srv_proc.blob_block_get_retry_flag){ // it may retry get command sometimes.
        // TODO fill missing chunk
        if(0 == is_buf_zero(fw_update_srv_proc.miss_mask, sizeof(fw_update_srv_proc.miss_mask))){
            // have been make sure no redundance bit in miss mask in mesh_cmd_sig_blob_block_start_
            memcpy(rsp.miss_chunk, &fw_update_srv_proc.miss_mask, sizeof(rsp.miss_chunk));
            #if PTS_TEST_OTA_EN // always use some chunk miss now
            if(is_buf_all_one(fw_update_srv_proc.miss_mask, sizeof(fw_update_srv_proc.miss_mask))){
                rsp.format = BLOB_BLOCK_FORMAT_ALL_CHUNK_MISS;
                LOG_MSG_LIB(TL_LOG_NODE_BASIC,0, 0,"all miss:",0);
            }else
            #endif
            {
                rsp_len += sizeof(rsp.miss_chunk);
                rsp.format = BLOB_BLOCK_FORMAT_SOME_CHUNK_MISS;
            }
        }else{
            // also check telink crc 16 total inside.
            #if BLOCK_CRC32_CHECKSUM_EN
            int crc_ok = block_crc32_check_current(fw_update_srv_proc.block_start.bk_check_sum_val);
            if(0 == crc_ok){  // SIG block crc32 ok
                st = BLOB_BLOCK_ST_WRONG_CHECK_SUM;
            }else
            #else
            int crc_ok = block_crc32_check_current(0); // crc_ok =1 means have not been receive "block get" before
            #endif
            {
                if(crc_ok){
					fw_update_srv_proc.blob_block_rx_ok = 1;
                    fw_update_srv_proc.blob_trans_phase = BLOB_TRANS_PHASE_WAIT_NEXT_BLOCK;  // receive ok
                }
                
                if(updater_get_block_rx_ok_cnt() == updater_get_fw_block_cnt()){// all block rx ok
                    fw_update_srv_proc.blob_trans_phase = BLOB_TRANS_PHASE_COMPLETE;
                }
                
                fw_update_srv_proc.blob_block_get_retry_flag = 1;
                rsp.format = BLOB_BLOCK_FORMAT_NO_CHUNK_MISS;
            }
        }
        st = BLOB_TRANS_ST_SUCCESS;
	}else{
	    st = BLOB_TRANS_ST_WRONG_PHASE;
	}
	model_g_light_s_t *p_model = (model_g_light_s_t *)cb_par->model;
	rsp.st = st;
	//rsp.format = // have been set before;// miss_cnt ? BLOB_BLOCK_FORMAT_SOME_CHUNK_MISS : BLOB_BLOCK_FORMAT_NO_CHUNK_MISS;
	#if (MESH_OTA_V_SEL == MESH_OTA_R04)
    rsp.transfer_phase = fw_update_srv_proc.blob_trans_phase;
    #endif
    
	if(0 == fw_update_srv_proc.block_start.chunk_size){
	    // MMDL/SR/BT/BV-33-C (BLOB Block Get-IUT in Idle Phase): never have received block start before
    	rsp.block_num = 0xffff;
    	rsp.chunk_size = 0xffff;
	}else{
    	rsp.block_num = fw_update_srv_proc.block_start.block_num;
    	rsp.chunk_size = fw_update_srv_proc.block_start.chunk_size;
	}

	// use mesh_blob_block_st_rsp() later
	return mesh_tx_cmd_rsp(BLOB_BLOCK_STATUS, (u8 *)&rsp, rsp_len, p_model->com.ele_adr, cb_par->adr_src, 0, 0);
}

int mesh_cmd_sig_blob_block_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

//------
int mesh_cmd_sig_blob_info_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	blob_info_status_t rsp = {0};
	rsp.bk_size_log_min = MESH_OTA_BLOCK_SIZE_LOG_MIN;
	rsp.bk_size_log_max = MESH_OTA_BLOCK_SIZE_LOG_MAX;
	rsp.chunk_num_max = MESH_OTA_CHUNK_NUM_MAX;
	rsp.chunk_size_max = MESH_OTA_CHUNK_SIZE_MAX;
	rsp.blob_size_max = MESH_OTA_BLOB_SIZE_MAX;
	rsp.server_mtu_size = MESH_CMD_ACCESS_LEN_MAX;
	rsp.transfer_mode = MESH_OTA_TRANSFER_MODE_SEL;
	
	model_g_light_s_t *p_model = (model_g_light_s_t *)cb_par->model;
	return mesh_tx_cmd_rsp(BLOB_INFO_STATUS, (u8 *)&rsp, sizeof(rsp), p_model->com.ele_adr, cb_par->adr_src, 0, 0);
}

int mesh_cmd_sig_blob_info_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}
#endif
#endif
