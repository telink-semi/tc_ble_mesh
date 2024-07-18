/********************************************************************************************************
 * @file	mesh_ota_initiator.c
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
#include "proj_lib/ble/blt_config.h"
#include "vendor/common/user_config.h"
#include "app_health.h"
#include "proj_lib/sig_mesh/app_mesh.h"
#include "mesh_ota.h"
#include "proj_lib/mesh_crypto/sha256_telink.h"

#if (MD_MESH_OTA_EN && INITIATOR_CLIENT_EN)
#if WIN32
#include "proj/common/tstring.h"
#endif

static fw_initiator_proc_t fw_initiator_proc = {{0}};       // for distributor (server + client) + updater client
blob_info_status_t g_blob_info_status_initiator = {0};

#define fw_initiator_current_node_adr   (fw_initiator_proc.list[fw_initiator_proc.node_num].adr)
//#define fw_distribut_srv_proc           nnnnnnnnn           // just for remove extern "fw_distribut_srv_proc"

extern u32 new_fw_size;



int is_mesh_ota_initiator_tx_client_model(u32 id, bool4 sig_model)
{
    return (sig_model && (SIG_MD_FW_DISTRIBUT_C == id));
}

u8 get_fw_initiator_proc_st()
{
    return fw_initiator_proc.st_initiator;
}

int is_mesh_ota_initiator_wait_ack()
{
    return (fw_initiator_proc.st_wait_flag);
}

void clr_mesh_ota_initiator_wait_ack()
{
    fw_initiator_proc.st_wait_flag = 0;
}

void mesh_ota_initiator_next_st_set(u8 st)
{
    fw_initiator_proc.st_initiator = st;
    clr_mesh_ota_initiator_wait_ack(); // when change st,it need to send command before waiting.
}

// --- 
void mesh_ota_initiator_next_block()
{
    fw_initiator_proc.block_start.block_num++;
    fw_initiator_proc.node_num = fw_initiator_proc.chunk_num = 0;
    memset(fw_initiator_proc.miss_mask, 0, sizeof(fw_initiator_proc.miss_mask));
    mesh_ota_initiator_next_st_set(INITIATOR_OTA_ST_BLOB_BLOCK_START);
}

inline u16 initiator_get_fw_block_cnt()
{
    return get_fw_block_cnt(fw_initiator_proc.blob_size, fw_initiator_proc.bk_size_log);
}

inline u32 initiator_get_block_size(u16 block_num)
{
    return get_block_size(fw_initiator_proc.blob_size, fw_initiator_proc.bk_size_log, block_num);
}

inline u16 initiator_get_fw_chunk_cnt()
{
    blob_block_start_t *bk_start = &fw_initiator_proc.block_start;
    return get_fw_chunk_cnt(fw_initiator_proc.bk_size_current, bk_start->chunk_size);
}

inline u16 initiator_get_chunk_size(u16 chunk_num)
{
    blob_block_start_t *bk_start = &fw_initiator_proc.block_start;
    return get_chunk_size(fw_initiator_proc.bk_size_current, bk_start->chunk_size, chunk_num);
}

inline u32 initiator_get_fw_data_position(u16 chunk_num)
{
    blob_block_start_t *bk_start = &fw_initiator_proc.block_start;
    return get_fw_data_position(bk_start->block_num, fw_initiator_proc.bk_size_log, chunk_num, bk_start->chunk_size);
}

void initiator_proc_init()
{
    memset(&fw_initiator_proc, 0, sizeof(fw_initiator_proc));
    #if DISTRIBUTOR_UPDATE_SERVER_EN
    new_fw_size = 0;
    #endif
}

int is_initiator_st_to_updating_node()
{
    if((INITIATOR_OTA_ST_FW_UPDATE_INFO_GET == fw_initiator_proc.st_initiator)
    || (INITIATOR_OTA_ST_UPDATE_METADATA_CHECK == fw_initiator_proc.st_initiator)
    || (INITIATOR_OTA_ST_SUBSCRIPTION_SET == fw_initiator_proc.st_initiator)){
        return 0;
    }
    return 1;
}

void mesh_ota_initiator_wait_ack_st_set()
{
	if(fw_initiator_proc.st_initiator){
    	fw_initiator_proc.st_wait_flag = 1;
    }
}

void mesh_ota_initiator_wait_ack_st_return(int success)
{
    if(!success){
        fw_initiator_proc.list[fw_initiator_proc.node_num].skip_flag = 1;
    }
    fw_initiator_proc.node_num++;
    fw_initiator_proc.st_wait_flag = 0;
}

void mesh_ota_initiator_ack_timeout_handle()
{
    if(is_mesh_ota_initiator_wait_ack()){
        mesh_ota_initiator_wait_ack_st_return(0);
    	if((INITIATOR_OTA_ST_DISTR_GET == fw_initiator_proc.st_initiator)
    	|| (INITIATOR_OTA_ST_DISTR_RECEIVER_GET == fw_initiator_proc.st_initiator)){
			LOG_MSG_INFO(TL_LOG_COMMON, 0, 0, "distributor may have been reboot for being updated");
	        mesh_ota_initiator_next_st_set(INITIATOR_OTA_ST_DISTR_CANCEL);	// distributor may have been reboot for being updated.
    	}
    }
}

int mesh_ota_initiator_check_skip_current_node()
{
    fw_initiator_proc_t *distr_proc = &fw_initiator_proc;
    if(distr_proc->list[distr_proc->node_num].skip_flag){
        //if( == distr_proc->st_initiator){
            //LOG_MSG_INFO(TL_LOG_COMMON,0,0,"access_cmd_blob_block_start, XXXXXX Skip addr:0x%04x", distr_proc->list[distr_proc->node_num].adr);
        //}
        distr_proc->node_num++;
        return 1;
    }
    return 0;
}

u32 is_need_block_transfer_initiator()
{
    return (BLOB_TRANS_ST_SUCCESS == fw_initiator_proc.list_distributor.st_block_start);
}

void fw_initiator_updating_list_init(fw_update_list_t *p_update_list, u32 update_node_cnt)
{
    fw_initiator_proc.node_cnt = 0;
    foreach(i,update_node_cnt){
        u16 addr = p_update_list->update_list[i];
        if((fw_initiator_proc.node_cnt < ARRAY_SIZE(fw_initiator_proc.list)) && is_unicast_adr(addr)){
            fw_initiator_proc.list[i].adr = addr;
            fw_initiator_proc.node_cnt++;
        }
    }
}

#define INITIATOR_ALL_RECEIVER_INVALID_ERR_NO		(-99)

u32 get_1st_valid_receiver()
{
    foreach(i, fw_initiator_proc.node_cnt){
        if(!fw_initiator_proc.list[i].skip_flag){
	        return i;
	    }
	}
	return INITIATOR_ALL_RECEIVER_INVALID_ERR_NO;
}

int is_initiator_all_receivers_invalid()
{
	return (INITIATOR_ALL_RECEIVER_INVALID_ERR_NO == get_1st_valid_receiver());
}

#if (DEBUG_SHOW_VC_SELF_EN && DISTRIBUTOR_NO_UPDATA_START_2_SELF)
int is_only_initiator_as_receiver()
{
	if(1 == fw_initiator_proc.node_cnt){
		return (is_own_ele(fw_initiator_proc.list[0].adr));
	}
	return 0;
}
#endif


// tx access command API
int access_cmd_fw_distr_capbility_get(u16 adr_dst)
{
    LOG_MSG_FUNC_NAME();
	u8 par[1] = {0};
	return SendOpParaDebug(adr_dst, 1, FW_DISTRIBUT_CAPABILITIES_GET, par, 0);
}

int access_cmd_fw_distr_receivers_add(u16 adr_dst)
{
    LOG_MSG_FUNC_NAME();
    fw_receive_entry_t entry[MESH_OTA_UPDATE_NODE_MAX] = {0};
    unsigned int cnt = 0;
    for(unsigned int i = 0; i < (fw_initiator_proc.node_cnt); ++i){
        if((cnt < MESH_OTA_UPDATE_NODE_MAX) && !fw_initiator_proc.list[i].skip_flag){
	        entry[cnt].addr = fw_initiator_proc.list[i].adr;
	        entry[cnt].update_fw_image_idx = 0;
	        cnt++;
	    }
	}

	if(cnt){
		return SendOpParaDebug(adr_dst, 1, FW_DISTRIBUT_RECEIVERS_ADD, (u8 *)&entry, sizeof(fw_receive_entry_t) * cnt);
	}else{
		return INITIATOR_ALL_RECEIVER_INVALID_ERR_NO;
	}
}

int access_cmd_fw_distr_receivers_get(u16 adr_dst, u16 first_index, u16 entries_limit)
{
    LOG_MSG_FUNC_NAME();
    fw_distribut_receiver_get_t get = {0};
    get.first_index = first_index;
    get.entries_limit = entries_limit;
	return SendOpParaDebug(adr_dst, 1, FW_DISTRIBUT_RECEIVERS_GET, (u8 *)&get, sizeof(get));
}

int access_cmd_fw_distr_upload_start(u16 adr_dst)
{
    LOG_MSG_FUNC_NAME();
    fw_distribut_upload_start_t *p_start = &fw_initiator_proc.upload_start;
    p_start->upload_ttl = fw_initiator_proc.init_start.upload_ttl;
    p_start->upload_timeout_base = fw_initiator_proc.init_start.upload_timeout_base;
    memcpy(p_start->upload_blob_id, fw_initiator_proc.init_start.upload_blob_id, sizeof(p_start->upload_blob_id));

    p_start->upload_fw_size = new_fw_size;
    p_start->fw_metadata_len = sizeof(p_start->fw_metadata);
    get_fw_metadata(&p_start->fw_metadata);
    memcpy(&p_start->fw_id, &p_start->fw_metadata.fw_id, sizeof(p_start->fw_id));
    
	return SendOpParaDebug(adr_dst, 1, FW_DISTRIBUT_UPLOAD_START, (u8 *)p_start, sizeof(fw_distribut_upload_start_t));
}

int access_cmd_fw_distr_upload_oob_start(u16 adr_dst)
{
    LOG_MSG_FUNC_NAME();
    u8 oob_start[ACCESS_WITH_MIC_LEN_MAX] = {0};
    fw_distribut_upload_oob_start_t *p_start = (fw_distribut_upload_oob_start_t *)oob_start;
    u8 uri[] = {"http://www.dummy.com"};
    p_start->uri_len = sizeof(uri) - 1;
	fw_id_t fw_id = {0};
	u32 len_par = OFFSETOF(fw_distribut_upload_oob_start_t, uri) + p_start->uri_len + sizeof(fw_id);
    if(len_par > sizeof(oob_start)){
    	return -1; // buffer over flow
    }
	memcpy(p_start->uri, uri, p_start->uri_len);
    memcpy(p_start->uri + p_start->uri_len, &fw_id, sizeof(fw_id));
    
	return SendOpParaDebug(adr_dst, 1, FW_DISTRIBUT_UPLOAD_OOB_START, (u8 *)p_start, len_par);
}

int access_cmd_fw_distr_upload_get(u16 adr_dst)
{
    LOG_MSG_FUNC_NAME();
	u8 par[1] = {0};
	return SendOpParaDebug(adr_dst, 1, FW_DISTRIBUT_UPLOAD_GET, par, 0);
}

int access_cmd_fw_distribut_start(u16 adr_dst)
{
    LOG_MSG_FUNC_NAME();
    fw_distribut_start_t cmd = {0};
    #if 1
    memcpy(&cmd, &fw_initiator_proc.init_start.distribut_start, sizeof(cmd));
    #else
    cmd.adr_group = fw_initiator_proc.init_start.distribut_start.adr_group;
    cmd.par.distrib_app_key_idx = DISTRIBUTION_APPKEY_INDEX;
    cmd.par.distrib_ttl = TTL_PUB_USE_DEFAULT;
    cmd.par.distrib_timeout_base = DISTRIBUTION_TIMEOUT_BASE_10MS;
    cmd.par.distrib_transfer_mode = MESH_OTA_TRANSFER_MODE_PUSH;
    cmd.par.update_policy = fw_initiator_proc.init_start.distribut_start.par.update_policy;
    //cmd.par.rfu = 0;
    cmd.par.distrib_fw_image_index = DISTRIBUTION_FW_IMAGE_INDEX;
    #endif
    
	return SendOpParaDebug(adr_dst, 1, FW_DISTRIBUT_START, (u8 *)&cmd, sizeof(cmd));
}

int access_cmd_fw_distribut_get(u16 adr_dst)
{
    LOG_MSG_FUNC_NAME();
	u8 par[1] = {0};
	return SendOpParaDebug(adr_dst, 1, FW_DISTRIBUT_GET, par, 0);
}

int access_cmd_fw_distribut_apply(u16 adr_dst)
{
    LOG_MSG_FUNC_NAME();
	u8 par[1] = {0};
	return SendOpParaDebug(adr_dst, 1, FW_DISTRIBUT_APPLY, par, 0);
}

int access_cmd_blob_transfer_start_initiator(u16 adr_dst, u32 blob_size, u8 bk_size_log)
{
    LOG_MSG_FUNC_NAME();
    blob_transfer_start_t cmd = {0};
    cmd.transfer_mode = MESH_OTA_TRANSFER_MODE_SEL;
    memcpy(&cmd.blob_id, fw_initiator_proc.init_start.upload_blob_id, sizeof(cmd.blob_id));
    cmd.blob_size = fw_initiator_proc.blob_size = blob_size;
    cmd.bk_size_log = fw_initiator_proc.bk_size_log = bk_size_log;
    cmd.client_mtu_size = MESH_CMD_ACCESS_LEN_MAX;
	return SendOpParaDebug(adr_dst, 1, BLOB_TRANSFER_START, (u8 *)&cmd, sizeof(cmd));
}

int access_cmd_blob_block_start_initiator(u16 adr_dst, u16 block_num)
{
    LOG_MSG_FUNC_NAME();
    blob_block_start_t *p_bk_start = &fw_initiator_proc.block_start;  // record parameters
    p_bk_start->block_num = block_num;
    p_bk_start->chunk_size = g_blob_info_status_initiator.chunk_size_max;
    fw_initiator_proc.bk_size_current = initiator_get_block_size(block_num);

	return SendOpParaDebug(adr_dst, 1, BLOB_BLOCK_START, (u8 *)p_bk_start, sizeof(blob_block_start_t));
}

// end of tx access command API


void mesh_ota_initiator_start(u8 *par, int par_len)
{
    if(par_len < sizeof(fw_initiator_start_t)){
        return ;
    }
    LOG_MSG_FUNC_NAME();
    initiator_proc_init();
    if(0 != read_ota_file2buffer()){
        initiator_proc_init();
        return;   // error
    }
    
    fw_initiator_start_t *p_start = (fw_initiator_start_t *)par;
    memcpy(&fw_initiator_proc.init_start, p_start, sizeof(fw_initiator_start_t));
    if(par_len >= sizeof(fw_initiator_start_t)+2){
        u32 cnt = (par_len - sizeof(fw_initiator_start_t))/2;
        fw_initiator_updating_list_init((fw_update_list_t *)(par+sizeof(fw_initiator_start_t)), cnt);
    }
    fw_initiator_proc.list_distributor.st_block_start = UPDATE_NODE_ST_IN_PROGRESS;

    mesh_ota_initiator_next_st_set(INITIATOR_OTA_ST_CAPABILITY_GET);
}

int mesh_ota_initiator_rx (mesh_rc_rsp_t *rsp, u16 op, u32 size_op)
{
    int op_handle_ok = 0;
    u8 *par = rsp->data + size_op;
    u16 par_len = GET_PAR_LEN_FROM_RSP(rsp->len, size_op);
    fw_initiator_proc_t *distr_proc = &fw_initiator_proc;
    int adr_match_distributor = (rsp->src == distr_proc->init_start.adr_distributor);
    int adr_match = (rsp->src == fw_initiator_current_node_adr) || adr_match_distributor;
    int next_st = 0;
    int skip_flag = 0;
    
    if(FW_DISTRIBUT_CAPABILITIES_STATUS == op){
        if(adr_match && distr_proc->st_wait_flag && (INITIATOR_OTA_ST_CAPABILITY_GET == distr_proc->st_initiator)){
            next_st = 1;
        }
        op_handle_ok = 1;
    }else if(FW_UPDATE_INFO_STATUS == op){
        if(adr_match && distr_proc->st_wait_flag && (INITIATOR_OTA_ST_FW_UPDATE_INFO_GET == distr_proc->st_initiator)){
            //fw_update_info_status_t *p = (fw_update_info_status_t *)par;
            next_st = 1;
        }
        op_handle_ok = 1;
    }else if(FW_UPDATE_METADATA_CHECK_STATUS == op){
        if(adr_match && distr_proc->st_wait_flag){
            fw_update_metadata_check_status_t *p = (fw_update_metadata_check_status_t *)par;
            if(INITIATOR_OTA_ST_UPDATE_METADATA_CHECK == distr_proc->st_initiator){
				LOG_MSG_INFO(TL_LOG_COMMON, 0, 0, "metada check additional info:%d ", p->additional_info);
                if(UPDATE_ST_SUCCESS != p->st){
                    skip_flag = 1;
                    LOG_MSG_ERR (TL_LOG_COMMON, 0, 0, "fw update metada check error:%d ", p->st);
                }
                next_st = 1;
            }
        }
        op_handle_ok = 1;
    }else if(FW_DISTRIBUT_RECEIVERS_STATUS == op){
        if(adr_match && distr_proc->st_wait_flag && (INITIATOR_OTA_ST_DISTR_RECEIVER_ADD == distr_proc->st_initiator)){
            fw_distribut_receivers_status_t *p = (fw_distribut_receivers_status_t *)par;
            if(ST_SUCCESS == p->st){
            }else{
            }
            next_st = 1;
            op_handle_ok = 1;
        }
    }else if(FW_DISTRIBUT_UPLOAD_STATUS == op){
        if(adr_match && distr_proc->st_wait_flag && (INITIATOR_OTA_ST_DISTR_UPLOAD_START == distr_proc->st_initiator)){
            fw_distribut_upload_status_t *p = (fw_distribut_upload_status_t *)par;
            if(ST_SUCCESS == p->st){
                if(sizeof(fw_distribut_upload_status_t) == par_len){
                    LOG_MSG_INFO (TL_LOG_CMD_NAME, 0, 0, "OTA distribute Progress:%d%%", p->upload_progress);
                }
            }else{
            }
            next_st = 1;
            op_handle_ok = 1;
        }
    }else if(BLOB_TRANSFER_STATUS == op){
        if(adr_match && distr_proc->st_wait_flag){
            blob_transfer_status_t *p = (blob_transfer_status_t *)par;
            if(INITIATOR_OTA_ST_BLOB_TRANSFER_GET == distr_proc->st_initiator){
                next_st = 1;
            }else if(INITIATOR_OTA_ST_BLOB_TRANSFER_START == distr_proc->st_initiator){
                next_st = 1;
            }
        
            if(BLOB_TRANS_ST_SUCCESS != p->st){
                skip_flag = 1;
                LOG_MSG_ERR (TL_LOG_COMMON, 0, 0, "object transfer status error:%d ", p->st);
            }
        }
        op_handle_ok = 1;
    }else if(BLOB_BLOCK_STATUS == op){
        if(adr_match && distr_proc->st_wait_flag){
            blob_block_status_t *p = (blob_block_status_t *)par;
            if(INITIATOR_OTA_ST_BLOB_BLOCK_START == distr_proc->st_initiator){
                if(INITIATOR_OTA_ST_BLOB_BLOCK_START == distr_proc->st_initiator){
                    distr_proc->list_distributor.st_block_start = p->st;
                    next_st = 1;
                }
            }else if(INITIATOR_OTA_ST_BLOB_BLOCK_GET == distr_proc->st_initiator){
                if(BLOB_TRANS_ST_SUCCESS == p->st){
                    distr_proc->list_distributor.st_block_get = p->format;
                    int miss_chunk_len = par_len - OFFSETOF(blob_block_status_t,miss_chunk);
                    if(miss_chunk_len >= 0 && miss_chunk_len <= sizeof(distr_proc->miss_mask)){
                        // distr_proc->miss_mask[] have been zero before block get and after end of INITIATOR_OTA_ST_BLOB_CHUNK_START.
                        if(BLOB_BLOCK_FORMAT_NO_CHUNK_MISS == p->format){
                        }else if(BLOB_BLOCK_FORMAT_ALL_CHUNK_MISS == p->format){
                            set_bit_by_cnt(distr_proc->miss_mask, sizeof(distr_proc->miss_mask), initiator_get_fw_chunk_cnt()); // all need send
                            LOG_MSG_LIB(TL_LOG_NODE_BASIC, 0, 0, "ALL CHUNK MISS");
                        }else if(BLOB_BLOCK_FORMAT_SOME_CHUNK_MISS == p->format){
                            for(int i = 0; i < (miss_chunk_len); ++i){
                                distr_proc->miss_mask[i] |= p->miss_chunk[i];
                            }
                            LOG_MSG_LIB (TL_LOG_NODE_BASIC, p->miss_chunk, miss_chunk_len, "SOME MISS CHUNK ");
                        }else if(BLOB_BLOCK_FORMAT_ENCODE_MISS_CHUNK == p->format){
                            LOG_MSG_LIB (TL_LOG_NODE_BASIC, p->miss_chunk, miss_chunk_len, "ENCODE MISS CHUNK ");
                            decode_miss_chunk(p->miss_chunk, miss_chunk_len, distr_proc->miss_mask, sizeof(distr_proc->miss_mask));
                        }
                    }else{
                        LOG_MSG_ERR (TL_LOG_COMMON, 0, 0, "TODO: MISS CHUNK LENGTH TOO LONG");
                    }
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
        if(adr_match && distr_proc->st_wait_flag && (INITIATOR_OTA_ST_BLOB_INFO_GET == distr_proc->st_initiator)){
            blob_info_status_t *p = (blob_info_status_t *)par;
            memcpy(&g_blob_info_status_initiator, p, sizeof(g_blob_info_status_initiator));
            u16 mesh_chunk_size_max = MESH_CMD_ACCESS_LEN_MAX - (1 + OFFSETOF(blob_chunk_transfer_t,data)); // 1: op
            if(g_blob_info_status_initiator.chunk_size_max > mesh_chunk_size_max){
                g_blob_info_status_initiator.chunk_size_max = mesh_chunk_size_max;
            }
            next_st = 1;
        }
        op_handle_ok = 1;
    }else if(CFG_MODEL_SUB_STATUS == op){
        if(adr_match && distr_proc->st_wait_flag && (INITIATOR_OTA_ST_SUBSCRIPTION_SET == distr_proc->st_initiator)){
            mesh_cfg_model_sub_status_t *p = (mesh_cfg_model_sub_status_t *)par;
            if(SIG_MD_BLOB_TRANSFER_S == (p->set.model_id & (DRAFT_FEAT_VD_MD_EN ? 0xffffffff : 0xffff))){
                if(ST_SUCCESS == p->status){
                }else{
                	skip_flag = 1;
                    LOG_MSG_ERR(TL_LOG_COMMON,0, 0,"set group failed, addr: 0x%04x",p->set.ele_adr);
                }
                next_st = 1;
                op_handle_ok = 1;
            }
        }
    }else if(FW_DISTRIBUT_STATUS == op){
        if(adr_match && distr_proc->st_wait_flag){
            fw_distribut_status_t *p = (fw_distribut_status_t *)par;
            if(INITIATOR_OTA_ST_DISTR_START == distr_proc->st_initiator){
                if(ST_SUCCESS != p->st){
                    LOG_MSG_ERR(TL_LOG_COMMON,0, 0,"distribute start failed, st: %d", p->st);
                }
            }else if(INITIATOR_OTA_ST_DISTR_GET == distr_proc->st_initiator){
                if((ST_SUCCESS == p->st) && (DISTRIBUT_PHASE_COMPLETED == p->distrib_phase)){
                    LOG_MSG_LIB(TL_LOG_NODE_BASIC, 0, 0, "distribution completed !");
                    mesh_ota_initiator_next_st_set(INITIATOR_OTA_ST_DISTR_PRE_CANCEL);
                }
            }
            next_st = 1;
            op_handle_ok = 1;
        }
    }else if(FW_DISTRIBUT_RECEIVERS_LIST == op){
        if(adr_match && distr_proc->st_wait_flag && (INITIATOR_OTA_ST_DISTR_RECEIVER_GET == distr_proc->st_initiator)){
            fw_distribut_receiver_list_t *p = (fw_distribut_receiver_list_t *)par;
            if(par_len >= OFFSETOF(fw_distribut_receiver_list_t, node) + sizeof(fw_distribut_update_node_entry_t)){
				LOG_MSG_LIB(TL_LOG_NODE_BASIC, 0, 0, "_______node addr:0x%04x,update phase:%d, distribute progress: %2d%%", p->node[0].adr,p->node[0].update_phase,p->node[0].transfer_progress * 2);
                if(p->node[0].update_phase >= UPDATE_PHASE_VERIFYING_SUCCESS){
                    mesh_ota_initiator_next_st_set(INITIATOR_OTA_ST_DISTR_PRE_APPLY);
                }
                
            	foreach(i, distr_proc->node_cnt){
            		fw_receiver_list_t *p_list = &distr_proc->list[i];
            		if(p->node[0].adr == p_list->adr){
            			p_list->update_phase = p->node[0].update_phase;
            			if((UPDATE_PHASE_TRANSFER_ERR == p_list->update_phase)
            			|| ((UPDATE_PHASE_IDLE == p_list->update_phase) && p->node[0].transfer_progress)){ // may have been reboot.
            				p_list->skip_flag = 1;
            			}
            			break;
            		}
            	}
            }else{
				LOG_MSG_ERR(TL_LOG_NODE_BASIC, 0, 0, "xxxxxxxxxxxxxxx distributor may have been reboot");
				mesh_ota_initiator_next_st_set(INITIATOR_OTA_ST_DISTR_CANCEL);
            }
            next_st = 1;
            op_handle_ok = 1;
        }
    }
    
    if(next_st){
        mesh_ota_initiator_wait_ack_st_return(!skip_flag);
    }
    
    return op_handle_ok;
}

void mesh_ota_initiator_proc()
{
    fw_initiator_proc_t *distr_proc = &fw_initiator_proc;
    static u32 tick_receiver_get;
	if(0 == distr_proc->st_initiator || is_busy_segment_or_reliable_flow() || !is_mesh_adv_cmd_fifo_empty()){
		return ;
	}
	
#if (WIN32 && (PROXY_HCI_SEL == PROXY_HCI_GATT))
	extern unsigned char connect_flag;
	if(!(pair_login_ok || DEBUG_SHOW_VC_SELF_EN) || distr_proc->pause_flag){
		return ;
	}
#endif

    if(distr_proc->st_wait_flag){
        return ;
    }

	u16 adr_distributor = distr_proc->init_start.adr_distributor;
	switch(distr_proc->st_initiator){
		case INITIATOR_OTA_ST_CAPABILITY_GET:
		    if(0 == distr_proc->node_num){
                if(0 == access_cmd_fw_distr_capbility_get(adr_distributor)){
                	mesh_ota_initiator_wait_ack_st_set();
                }
            }else{
                distr_proc->node_num = 0;
    		    mesh_ota_initiator_next_st_set(INITIATOR_OTA_ST_FW_UPDATE_INFO_GET);
		    }
			break;
			
		case INITIATOR_OTA_ST_FW_UPDATE_INFO_GET:
		    if(distr_proc->node_num < distr_proc->node_cnt){
		        if(mesh_ota_initiator_check_skip_current_node()){ break;}
    	        if(0 == access_cmd_fw_update_info_get(fw_initiator_current_node_adr)){
    	            mesh_ota_initiator_wait_ack_st_set();
    	        }
	        }else{
                distr_proc->node_num = 0;
                mesh_ota_initiator_next_st_set(INITIATOR_OTA_ST_UPDATE_METADATA_CHECK);
	        }
			break;

        case INITIATOR_OTA_ST_UPDATE_METADATA_CHECK:
		    if(distr_proc->node_num < distr_proc->node_cnt){
		        if(mesh_ota_initiator_check_skip_current_node()){ break;}

		        fw_metadata_t fw_metadata = {{0}};
		        get_fw_metadata(&fw_metadata);
    	        if(0 == access_cmd_fw_update_metadata_check(fw_initiator_current_node_adr, &fw_metadata)){
    	            mesh_ota_initiator_wait_ack_st_set();
    	        }
	        }else{
                distr_proc->node_num = 0;
                mesh_ota_initiator_next_st_set(INITIATOR_OTA_ST_SUBSCRIPTION_SET);
	        }
            break;

        case INITIATOR_OTA_ST_SUBSCRIPTION_SET:
		    if(distr_proc->node_num < distr_proc->node_cnt && !is_unicast_adr(fw_initiator_proc.init_start.distribut_start.adr_group)){
		        if(mesh_ota_initiator_check_skip_current_node()){ break;}
		        
    	        if(0 == cfg_cmd_sub_set(CFG_MODEL_SUB_ADD, fw_initiator_current_node_adr, fw_initiator_current_node_adr, 
    	                        fw_initiator_proc.init_start.distribut_start.adr_group, SIG_MD_BLOB_TRANSFER_S, !DRAFT_FEAT_VD_MD_EN)){
    	            mesh_ota_initiator_wait_ack_st_set();
    	        }
	        }else{
                distr_proc->node_num = 0;
    	        mesh_ota_initiator_next_st_set(INITIATOR_OTA_ST_DISTR_RECEIVER_ADD);
	        }
            break;

        case INITIATOR_OTA_ST_DISTR_RECEIVER_ADD:
		    if(0 == distr_proc->node_num){
                int err = access_cmd_fw_distr_receivers_add(adr_distributor);
                if(INITIATOR_ALL_RECEIVER_INVALID_ERR_NO == err){
					LOG_MSG_ERR(TL_LOG_COMMON,0, 0,"xxxxxxxxxxxxxxxxx all receivers invalid"); // or error state
					distr_proc->node_num = 0;
					mesh_ota_initiator_next_st_set(INITIATOR_OTA_ST_DISTR_CANCEL);
                }else if(0 == err){
					mesh_ota_initiator_wait_ack_st_set();
                }
            }else{
                distr_proc->node_num = 0;
    		    mesh_ota_initiator_next_st_set(INITIATOR_OTA_ST_DISTR_UPLOAD_START);
		    }
            break;

        case INITIATOR_OTA_ST_DISTR_UPLOAD_START:
		    if(0 == distr_proc->node_num){
                if(0 == access_cmd_fw_distr_upload_start(adr_distributor)){
                	mesh_ota_initiator_wait_ack_st_set();
                }
            }else{
                distr_proc->node_num = 0;
    		    mesh_ota_initiator_next_st_set(INITIATOR_OTA_ST_BLOB_TRANSFER_GET);
		    }
            break;

		case INITIATOR_OTA_ST_BLOB_TRANSFER_GET:
		    if(0 == distr_proc->node_num){
    	        if(0 == access_cmd_blob_transfer_get(adr_distributor)){
    	            mesh_ota_initiator_wait_ack_st_set();
    	        }
	        }else{
                distr_proc->node_num = 0;
    	        mesh_ota_initiator_next_st_set(INITIATOR_OTA_ST_BLOB_INFO_GET);
	        }
			break;

		case INITIATOR_OTA_ST_BLOB_INFO_GET:
		    if(0 == distr_proc->node_num){
    	        if(0 == access_cmd_blob_info_get(adr_distributor)){
    	            mesh_ota_initiator_wait_ack_st_set();
    	        }
	        }else{
                distr_proc->node_num = 0;
    	        mesh_ota_initiator_next_st_set(INITIATOR_OTA_ST_BLOB_TRANSFER_START);
	        }
			break;

		case INITIATOR_OTA_ST_BLOB_TRANSFER_START:
		    if(0 == distr_proc->node_num){
    	        if(0 == access_cmd_blob_transfer_start_initiator(adr_distributor, new_fw_size, g_blob_info_status_initiator.bk_size_log_max)){
    	            mesh_ota_initiator_wait_ack_st_set();
    	        }
	        }else{
                distr_proc->node_num = 0;
                if(distr_proc->list[0].skip_flag){
					mesh_ota_initiator_next_st_set(INITIATOR_OTA_ST_DISTR_CANCEL);
                }else{
    	        	mesh_ota_initiator_next_st_set(INITIATOR_OTA_ST_BLOB_BLOCK_START);
    	        }
	        }
			break;

		case INITIATOR_OTA_ST_BLOB_BLOCK_START:
		{
            u16 block_num_current = distr_proc->block_start.block_num;
		    if(block_num_current < initiator_get_fw_block_cnt()){
    		    if(0 == distr_proc->node_num){
        	        if(0 == access_cmd_blob_block_start_initiator(adr_distributor, block_num_current)){
                        mesh_ota_initiator_wait_ack_st_set();
        	        }
    	        }else{
                    distr_proc->node_num = 0;
        	        mesh_ota_initiator_next_st_set(INITIATOR_OTA_ST_BLOB_BLOCK_START_CHECK_RESULT);
    	        }
	        }else{
                distr_proc->node_num = 0;
                if(distr_proc->list[0].skip_flag){
                	// may be crc error
					mesh_ota_initiator_next_st_set(INITIATOR_OTA_ST_DISTR_CANCEL);
                }else{
                	mesh_ota_initiator_next_st_set(INITIATOR_OTA_ST_DISTR_START);
                }
	        }
			break;
		}

		case INITIATOR_OTA_ST_BLOB_BLOCK_START_CHECK_RESULT:
            if(is_need_block_transfer_initiator()){
                distr_proc->chunk_num = 0;
                set_bit_by_cnt(distr_proc->miss_mask, sizeof(distr_proc->miss_mask), initiator_get_fw_chunk_cnt()); // all need send
    	        mesh_ota_initiator_next_st_set(INITIATOR_OTA_ST_BLOB_CHUNK_START);
	        }else{
	            mesh_ota_initiator_next_block();
	        }
			break;

		case INITIATOR_OTA_ST_BLOB_CHUNK_START:
		{
		    u32 chunk_cnt = initiator_get_fw_chunk_cnt();
		    if(distr_proc->chunk_num < chunk_cnt){
		        if(is_buf_bit_set(distr_proc->miss_mask, distr_proc->chunk_num)){
		            u8 cmd_buf[MESH_CMD_ACCESS_LEN_MAX+16] = {0};
                    blob_chunk_transfer_t *p_cmd = (blob_chunk_transfer_t *)cmd_buf;
                    p_cmd->chunk_num = distr_proc->chunk_num;
                    u16 size = initiator_get_chunk_size(p_cmd->chunk_num);
                    if(size > g_blob_info_status_initiator.chunk_size_max){
                        size = g_blob_info_status_initiator.chunk_size_max;
                    }

                    u32 fw_pos = 0;
					u8 *data =0;
					u16 block_num_current = distr_proc->block_start.block_num;
					
					fw_pos = initiator_get_fw_data_position(p_cmd->chunk_num);
                    mesh_ota_tx_fw_data_read(fw_pos, size, p_cmd->data);
					
                    u16 bk_total = initiator_get_fw_block_cnt();
                    u8 percent = 1 + (fw_pos+size)*98/distr_proc->blob_size;
                    if(percent > distr_proc->percent_last){
                        distr_proc->percent_last = percent;
                        APP_RefreshProgressBar(block_num_current, bk_total, distr_proc->chunk_num, chunk_cnt, percent);
                    }

                    if(0 == access_cmd_blob_chunk_transfer(adr_distributor, (u8 *)p_cmd, size+2)){
    		            distr_proc->chunk_num++;
    		        }
		        }else{
		            distr_proc->chunk_num++;
		        }
	        }else{
	            distr_proc->node_num = distr_proc->chunk_num = 0;
	            memset(distr_proc->miss_mask, 0, sizeof(distr_proc->miss_mask));
	            mesh_ota_initiator_next_st_set(INITIATOR_OTA_ST_BLOB_BLOCK_GET);
	        }
	    }
			break;

		case INITIATOR_OTA_ST_BLOB_BLOCK_GET:
		    if(0 == distr_proc->node_num){
    	        if(0 == access_cmd_blob_block_get(adr_distributor, distr_proc->block_start.block_num)){
    	            mesh_ota_initiator_wait_ack_st_set();
    	        }
	        }else{
	            if(0 == is_buf_zero(distr_proc->miss_mask, sizeof(distr_proc->miss_mask))){
                    distr_proc->chunk_num = 0;
                    LOG_MSG_INFO (TL_LOG_CMD_NAME, 0, 0, "access_cmd_blob_chunk_transfer retry");
                    mesh_ota_initiator_next_st_set(INITIATOR_OTA_ST_BLOB_CHUNK_START);
	            }else{
                    mesh_ota_initiator_next_block();
    	        }
	        }
			break;
			
        case INITIATOR_OTA_ST_DISTR_START:
            if(0 == distr_proc->node_num){
                if(0 == access_cmd_fw_distribut_start(adr_distributor)){
                    mesh_ota_initiator_wait_ack_st_set();
                }
            }else{
                distr_proc->node_num = 0;
                tick_receiver_get = clock_time();
    	        mesh_ota_initiator_next_st_set(INITIATOR_OTA_ST_DISTR_RECEIVER_GET);
            }
            break;
			
        case INITIATOR_OTA_ST_DISTR_RECEIVER_GET:
            if(1){
            	u32 index = get_1st_valid_receiver();
            	if(INITIATOR_ALL_RECEIVER_INVALID_ERR_NO == index){
					distr_proc->node_num = 0;
            		#if (DEBUG_SHOW_VC_SELF_EN && DISTRIBUTOR_NO_UPDATA_START_2_SELF)
            		if(is_only_initiator_as_receiver()){
                    	mesh_ota_initiator_next_st_set(INITIATOR_OTA_ST_DISTR_PRE_APPLY);
            		}else
            		#endif
            		{
						LOG_MSG_ERR(TL_LOG_COMMON,0, 0,"xxxxxxxxxxxxxxxxxxxxxxxx all receivers invalid"); // or error state
						mesh_ota_initiator_next_st_set(INITIATOR_OTA_ST_DISTR_CANCEL);
					}
                }else{
					if(clock_time_exceed(tick_receiver_get, 5*1000*1000)){
						tick_receiver_get = clock_time();
						if(0 == access_cmd_fw_distr_receivers_get(adr_distributor, (u16)index, 1)){ // just get the first one, due to avoid segment response
							mesh_ota_initiator_wait_ack_st_set();
						}
					}
                }
            }else{
    	        //mesh_ota_initiator_next_st_set(INITIATOR_OTA_ST_DISTR_PRE_APPLY); // set in "rx handle"
            }
            break;
			
        case INITIATOR_OTA_ST_DISTR_PRE_APPLY:
            distr_proc->node_num = 0;
            if(UPDATE_POLICY_VERIFY_ONLY == distr_proc->init_start.distribut_start.par.update_policy){
                mesh_ota_initiator_next_st_set(INITIATOR_OTA_ST_DISTR_APPLY);
            }else{
                mesh_ota_initiator_next_st_set(INITIATOR_OTA_ST_DISTR_GET);
            }
            break;
			
        case INITIATOR_OTA_ST_DISTR_APPLY:
            if(0 == distr_proc->node_num){
                if(0 == access_cmd_fw_distribut_apply(adr_distributor)){
                    mesh_ota_initiator_wait_ack_st_set();
                }
            }else{
                distr_proc->node_num = 0;
                tick_receiver_get = clock_time();
    	        mesh_ota_initiator_next_st_set(INITIATOR_OTA_ST_DISTR_GET);
            }
            break;
			
        case INITIATOR_OTA_ST_DISTR_GET:
            if(1){
                u32 interval_s = (distr_proc->node_cnt >= 3) ? 5 : 3;
                if(clock_time_exceed(tick_receiver_get, interval_s*1000*1000)){
                    tick_receiver_get = clock_time();
                    if(0 == access_cmd_fw_distribut_get(adr_distributor)){
                        mesh_ota_initiator_wait_ack_st_set();
                    }
                }
            }else{
    	        //mesh_ota_initiator_next_st_set(INITIATOR_OTA_ST_DISTR_CANCEL); // set in "mesh_ota_initiator_rx_" or "mesh_ota_initiator_ack_timeout_handle_"
            }
            break;
			
        case INITIATOR_OTA_ST_DISTR_PRE_CANCEL:
            distr_proc->node_num = 0;
	        mesh_ota_initiator_next_st_set(INITIATOR_OTA_ST_DISTR_CANCEL);
            break;
			
        case INITIATOR_OTA_ST_DISTR_CANCEL:
            if(0 == distr_proc->node_num){
                if(0 == access_cmd_fw_distribut_cancel(adr_distributor)){
                    mesh_ota_initiator_wait_ack_st_set();
                }
            }else{
                distr_proc->node_num = 0;
    	        mesh_ota_initiator_next_st_set(INITIATOR_OTA_ST_MAX);
            }
            break;

		default :
		    memset(distr_proc, 0, sizeof(fw_initiator_proc_t));
            LOG_MSG_INFO(TL_LOG_COMMON,0, 0,"flow completed"); // or error state
            #if VC_APP_ENABLE
            extern int disable_log_cmd;
            disable_log_cmd = 1;   // mesh OTA finished
            #endif
            
			break;
	}
}


#endif
