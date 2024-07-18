/********************************************************************************************************
 * @file	mesh_ota.c
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

#if MD_MESH_OTA_EN
#if WIN32
#include "proj/common/tstring.h"

u32 	ota_firmware_size_k = FW_SIZE_MAX_K;	// same with pm_8269.c
#endif

#define PTS_OTA_TODO_EN			(PTS_TEST_OTA_EN)

STATIC_ASSERT(FW_SIZE_MAX_K % 4 == 0);  // because ota_firmware_size_k is must 4k aligned.
STATIC_ASSERT(ACCESS_WITH_MIC_LEN_MAX >= (MESH_OTA_CHUNK_SIZE + 1 + SZMIC_TRNS_SEG64 + 7)); // 1: op code, 7:margin
STATIC_ASSERT(sizeof(fw_update_metadata_check_t) + SIZE_OF_OP(FW_UPDATE_METADATA_CHECK) <= 11); // use unsegment should be better.
#if (!PTS_TEST_OTA_EN && EXTENDED_ADV_ENABLE) // PTS no need to use extend adv.
STATIC_ASSERT(PROXY_GATT_MAX_LEN >= (MESH_OTA_CHUNK_SIZE + (sizeof(bear_head_t)+13)+1+2)); //OFFSETOF(mesh_cmd_bear_t,ut_unseg) + 1 + OFFSETOF(blob_chunk_transfer_t,data)
#endif
STATIC_ASSERT((MESH_OTA_BLOCK_SIZE_LOG_MAX >= MESH_OTA_BLOCK_SIZE_LOG_MIN) && (MESH_OTA_BLOCK_SIZE_LOG_MAX <= 20));
#if PTS_TEST_OTA_EN
STATIC_ASSERT(MESH_OTA_BLOB_SIZE_MAX > MESH_OTA_BLOCK_SIZE_MAX);	// SR/BT/BV02-C need to test more than 1 block.
#endif

#define BLOCK_CRC32_CHECKSUM_EN     (0)


#if 0   // use const now.
void get_fw_id()
{
#if !WIN32
    #if FW_START_BY_LEGACY_BOOTLOADER_EN
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

#if UPLOAD_MULTI_FW_TEST_EN
upload_multi_fw_id_t upload_multi_fw_id = {0};
#endif

extern _align_4_ fw_update_srv_proc_t    fw_update_srv_proc;
extern u8 ota_reboot_type; 
extern u32 ota_reboot_later_tick;

int block_crc32_check_current(u32 check_val);


//--- common
int is_fwid_match(fw_id_t *id1, fw_id_t *id2)
{
    return (0 == memcmp(id1, id2, sizeof(fw_id_t)));
}

int is_blob_id_match(const u8 *blob_id1, const u8 *blob_id2)
{
    return (0 == memcmp(blob_id1, blob_id2, 8));
}

u16 get_fw_block_cnt(u32 blob_size, u8 bk_size_log)
{
    u32 bk_size = (1 << bk_size_log);
    return (blob_size + bk_size - 1) / bk_size;
}

u32 get_block_size(u32 blob_size, u8 bk_size_log, u16 block_num)
{
    u32 bk_size = (1 << bk_size_log);
    u16 bk_cnt = get_fw_block_cnt(blob_size, bk_size_log);
    if(block_num + 1 < bk_cnt){
        return bk_size;
    }else{
        return (blob_size - block_num * bk_size);
    }
}

u16 get_fw_chunk_cnt(u32 bk_size_current, u16 chunk_size_max)
{
    return (bk_size_current + chunk_size_max - 1) / chunk_size_max;
}

u16 get_chunk_size(u32 bk_size_current, u16 chunk_size_max, u16 chunk_num)
{
    u16 chunk_cnt = get_fw_chunk_cnt(bk_size_current, chunk_size_max);
    if(chunk_num + 1 < chunk_cnt){
        return chunk_size_max;
    }else{
        return (bk_size_current - chunk_num * chunk_size_max);
    }
}

u32 get_fw_data_position(u16 block_num, u8 bk_size_log, u16 chunk_num, u16 chunk_size_max)
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

int is_mesh_ota_client_tx_op(u16 op)
{
	mesh_op_resource_t op_res_tx;
	if(0 == mesh_search_model_id_by_op(&op_res_tx, op, 1)){
		return is_mesh_ota_tx_client_model(op_res_tx.id, op_res_tx.sig);
	}
	return 0;
}

/***************************************** 
------- for distributor node
******************************************/
#if (DISTRIBUTOR_UPDATE_CLIENT_EN || DISTRIBUTOR_UPDATE_SERVER_EN)
// const u32 fw_id_new     = 0xff000021;   // set in distribution start
const u8  blob_id_new[8] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88};
//#define TEST_CHECK_SUM_TYPE     (BLOB_BLOCK_CHECK_SUM_TYPE_CRC32)

#define GROUP_VAL_FW_INFO_GET_ALL       (0xFFFF)	// because 0 means use unicast address

u32 new_fw_size = 0;

#if GATEWAY_ENABLE
u8* fw_ota_data_tx;
#elif WIN32
u8 fw_ota_data_tx[NEW_FW_MAX_SIZE];
u8 fw_ota_data_rx[NEW_FW_MAX_SIZE] = {1,2,3,4,5,};
#endif

STATIC_ASSERT(MESH_OTA_BLOCK_SIZE_MAX <= MESH_OTA_CHUNK_SIZE * 65536);
STATIC_ASSERT((MESH_OTA_BLOCK_SIZE_MIN % 16 == 0)&&(MESH_OTA_BLOCK_SIZE_MAX % 16 == 0)); // for CRC check
#if MESH_OTA_PULL_MODE_EN
STATIC_ASSERT((MESH_OTA_CHUNK_SIZE * 0x80 >= MESH_OTA_BLOCK_SIZE_MAX));    // due to utf8 encode size is ">= 2bytes" when chunk index is greater than 0x80.
STATIC_ASSERT((sizeof(fw_distribut_srv_proc.miss_mask) <= (CEIL_8(CEIL_DIV(1<<18, 208))))); // because default size of miss_mask of distributor.
#if !EXTENDED_ADV_ENABLE
STATIC_ASSERT(sizeof(blob_block_status_t) <= 11);	// for no segment. no need to be same with the number of partial report message.
#endif
STATIC_ASSERT((PULL_MODE_MAX_MISSING_CHUNK + 1) <= 11); // 1: sizeof op code BLOB_PARTIAL_BLOCK_REPORT, for no segment.
STATIC_ASSERT((MESH_OTA_CHUNK_SIZE % 16 == 0) || (0 == ENCODE_OTA_BIN_EN)); // no encryption bin mode for pull mode now.
#else
STATIC_ASSERT((MESH_OTA_CHUNK_SIZE % 16 == 0)); // for encryption bin
#endif
STATIC_ASSERT((MESH_OTA_BLOCK_SIZE_LOG_MIN >= 6) && (MESH_OTA_BLOCK_SIZE_LOG_MIN <= 20));	// for spec
STATIC_ASSERT((MESH_OTA_BLOCK_SIZE_LOG_MAX >= 6) && (MESH_OTA_BLOCK_SIZE_LOG_MAX <= 20));	// for spec
STATIC_ASSERT(MESH_OTA_BLOCK_SIZE_LOG_MAX >= MESH_OTA_BLOCK_SIZE_LOG_MIN);	// 

//STATIC_ASSERT(MESH_OTA_CHUNK_NUM_MAX <= 32);  // max bit of miss_mask in fw_distribut_srv_proc


fw_distribut_srv_proc_t fw_distribut_srv_proc = {{0}};      // for distributor (server + client) + updater client

#define master_ota_current_node_adr     (fw_distribut_srv_proc.list[fw_distribut_srv_proc.node_num].adr)

blob_info_status_t g_blob_info_status = {0};

int is_mesh_ota_flow_with_user_define_intv(u16 op)
{
	if(fw_distribut_srv_proc.st_distr && fw_distribut_srv_proc.retry_intv_for_lpn_100ms){
		if(is_mesh_ota_client_tx_op(op)){
			return 1;
		}
	}
	return 0;
}

void mesh_ota_check_and_set_lpn_retry_intv(mesh_bulk_cmd_par_t *p_cmd, int len_cmd)
{
	if(FW_DISTRIBUT_START == get_op_u16(&p_cmd->op)){
		if(fw_distribut_srv_proc.st_distr){
			#if DISTRIBUTOR_START_TLK_EN
			if(is_par_distribute_start_tlk(p_cmd->par+1, len_cmd - OFFSETOF(mesh_bulk_cmd_par_t,par) -1)){ // -1, due to distribute start is 2 bytes command.
				if(p_cmd->retry_cnt >= 10){ // because default cnt is 2.
					fw_distribut_srv_proc.retry_intv_for_lpn_100ms = get_transition_100ms((trans_time_t *)&p_cmd->retry_cnt);
				}
			}
			#endif
		}
	}
}

static void mesh_ota_master_next_st_set(u8 st)
{
#if 0 // (WIN32 && DEBUG_SHOW_VC_SELF_EN)
    LOG_MSG_INFO (TL_LOG_CMD_NAME, 0, 0, "OTA master next st: 0x%02x", st);
#endif

    fw_distribut_srv_proc.st_distr = st;
	clr_mesh_ota_master_wait_ack(); // when change st,it need to send command before waiting.
#if NODE_DISTRIBUT_RETRY_FLOW_EN
	fw_distribut_srv_proc.wait_ack_retry_cnt = fw_distribut_srv_proc.wait_ack_tick = 0; // init
#endif
}

#if (DISTRIBUTOR_UPDATE_CLIENT_EN || PTS_TEST_BLOB_TRANSFER_FLOW_EN)
inline int is_only_get_fw_info_fw_distribut_srv()
{
#if ((WIN32 || GATEWAY_ENABLE) && (!PTS_TEST_OTA_EN))
    return (GROUP_VAL_FW_INFO_GET_ALL == fw_distribut_srv_proc.adr_group);
#else
	return 0;	// PTS may set group to 0.
#endif
}
#endif

void mesh_ota_master_next_block()
{
    fw_distribut_srv_proc.block_start.block_num++;
    fw_distribut_srv_proc.node_num = fw_distribut_srv_proc.chunk_num = 0;
    memset(fw_distribut_srv_proc.miss_mask, 0, sizeof(fw_distribut_srv_proc.miss_mask));
	foreach_arr(i, fw_distribut_srv_proc.list){
		fw_distribut_srv_proc.list[i].no_missing_flag = 0;
	}
	
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

u32 is_distributor_all_receivers_invalid()
{
    foreach(i, fw_distribut_srv_proc.node_cnt){
        if(0 == fw_distribut_srv_proc.list[i].skip_flag
        #if DISTRIBUTOR_NO_UPDATA_START_2_SELF
        || is_own_ele(fw_distribut_srv_proc.list[i].adr)
        #endif
        ){
            return 0;
        }
    }

    return 1;
}

inline u16 distribut_get_fw_block_cnt()
{
    return get_fw_block_cnt(fw_distribut_srv_proc.blob_size, fw_distribut_srv_proc.bk_size_log);
}

inline u32 distribut_get_block_size(u16 block_num)
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

static inline int is_valid_fw_id_len(int len)
{
	return (len <= SIZEOF_MEMBER(fw_distribut_upload_start_t, fw_id));
}

#if ((ANDROID_APP_ENABLE || IOS_APP_ENABLE))
void APP_set_mesh_ota_pause_flag(u8 val)
{
    if(fw_distribut_srv_proc.st_distr){
        fw_distribut_srv_proc.suspend_flag = val;
    }
    
    if(fw_initiator_proc.st_initiator){
        fw_initiator_proc.pause_flag = val;
    }
}
#else
void APP_RefreshProgressBar(u16 bk_current, u16 bk_total, u16 chunk_cur, u16 chunk_total, u8 percent)
{
	#if INITIATOR_CLIENT_EN
	if(get_fw_initiator_proc_st() && (fw_distribut_srv_proc.st_distr <= MASTER_OTA_ST_DISTRIBUT_START)){
		LOG_MSG_INFO (TL_LOG_CMD_NAME, 0, 0, "block sum:%2d,cur:%2d,chunk sum:%2d,cur:%2d, upload Progress:%d%%", bk_total, bk_current, chunk_total, chunk_cur, percent);
	}else
	#endif
	{
    	LOG_MSG_INFO (TL_LOG_CMD_NAME, 0, 0, "OTA,block total:%2d,cur:%2d,chunk total:%2d,cur:%2d, Progress:%d%%", bk_total, bk_current, chunk_total, chunk_cur, percent);
		#if GATEWAY_ENABLE
		gateway_upload_log_info(0,0,"OTA, block sum:%2d,cur:%2d, chunk sum:%2d,cur:%2d, Progress:%2d%% ", bk_total, bk_current, chunk_total, chunk_cur, percent);
		#endif
	}
}

void APP_report_mesh_ota_apply_status(u16 adr_src, fw_update_status_t *p)
{
    // nothing for VC now
}

u16 APP_get_GATT_connect_addr()
{
    #if WIN32
    return connect_addr_gatt;
    #else
    return ele_adr_primary;
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
void distribut_srv_proc_init(int keep_new_fw_size_flag)
{
    memset(&fw_distribut_srv_proc, 0, sizeof(fw_distribut_srv_proc));
	memset(&fw_update_srv_proc, 0, sizeof(fw_update_srv_proc)); // because blob transfer may have run before.
    #if DISTRIBUTOR_UPDATE_SERVER_EN
    if(!keep_new_fw_size_flag){
    	new_fw_size = 0;
    }
    #endif
}

int is_distribute_st_with_optional_par(u8 st_response)
{
	return (!((DISTRIBUT_PHASE_IDLE == fw_distribut_srv_proc.distribut_update_phase) && (DISTRIBUT_ST_SUCCESS == st_response)) // FD/BV-35: if completed, should not report optional par in the last setp of distribution cancel.
	     && (fw_distribut_srv_proc.adr_group || (fw_distribut_srv_proc.st_distr >= MASTER_OTA_ST_DISTRIBUT_START))); // adr_group may be zero. 
}

int mesh_tx_cmd_fw_distribut_st(u8 idx, u16 ele_adr, u16 dst_adr, u8 st)
{
	fw_distribut_status_t rsp = {0};
	rsp.st = st;
	rsp.distrib_phase = fw_distribut_srv_proc.suspend_flag ? DISTRIBUT_PHASE_TRANSFER_SUSPEND : fw_distribut_srv_proc.distribut_update_phase;
	#if 0 // TEST_BV45_EN // TODO: PTS beta version expect a phase which is different from "TS doc". 
	if(DISTRIBUT_PHASE_APPLYING_UPDATE == rsp.distrib_phase){
		rsp.distrib_phase = DISTRIBUT_PHASE_TRANSFER_SUCCESS;
	}
	#endif
	u32 rsp_len = OFFSETOF(fw_distribut_status_t, distrib_multicast_addr);
	if(is_distribute_st_with_optional_par(st)){
        rsp_len = sizeof(fw_distribut_status_t);
        rsp.distrib_multicast_addr = fw_distribut_srv_proc.adr_group;
        memcpy(&rsp.par, &fw_distribut_srv_proc.distribut_start.par, sizeof(rsp.par));
	}

	return mesh_tx_cmd_rsp(FW_DISTRIBUT_STATUS, (u8 *)&rsp, rsp_len, ele_adr, dst_adr, 0, 0);
}

int mesh_fw_distribut_st_rsp(mesh_cb_fun_par_t *cb_par, u8 st)
{
	model_g_light_s_t *p_model = (model_g_light_s_t *)cb_par->model;
	return mesh_tx_cmd_fw_distribut_st(cb_par->model_idx, p_model->com.ele_adr, cb_par->adr_src, st);
}

int mesh_cmd_sig_fw_distribut_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	u8 st;
    if(fw_distribut_srv_proc.st_distr){
        st = DISTRIBUT_ST_SUCCESS;
    }else{
        st = DISTRIBUT_ST_SUCCESS;
    }

#if PTS_TEST_OTA_EN
	if(is_rx_upload_start_before() && (fw_distribut_srv_proc.st_distr >= MASTER_OTA_ST_FW_UPDATE_INFO_GET_AFTER_APPLY)){
        fw_distribut_srv_proc.node_num = 0;
		mesh_ota_master_next_st_set(MASTER_OTA_ST_FW_UPDATE_INFO_GET_AFTER_APPLY);	// FD/BV-41, need to send "fw update info get" again after "distribute get"
	}
#endif
    
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
        #if ENCODE_OTA_BIN_EN
        u8 fw_data[16];
		mesh_ota_read_data(0x10, sizeof(fw_data), fw_data);
        u8 key[16];
        memcpy(key, key_encode_bin, sizeof(key));
		aes_decrypt(key, fw_data, fw_data);
		memcpy(&new_fw_size, fw_data + 8, sizeof(new_fw_size));
        #else
        new_fw_size = get_fw_len();
        #endif
    #else
        new_fw_size = new_fw_read(fw_ota_data_tx, sizeof(fw_ota_data_tx));
    #endif
        
        if((0 == new_fw_size) || (-1 == new_fw_size)){
            #if GATEWAY_ENABLE
            gateway_upload_log_info(0,0,"GW OTA BIN size error! click (GwMeshOta) to download!\r\n");
            #endif
            return -1;
        }

        if(new_fw_size < 10*1024){
            fw_distribut_srv_proc.miss_chunk_test_flag = 2;
        }
#endif
    }

    return 0;
}

void mesh_fw_distibut_set(u8 en)
{
	fw_distribut_srv_proc.ota_rcv_flag = en;
}

void mesh_fw_distribute_adr(u8 en)
{
	fw_distribut_srv_proc.adr_set_flag = en;
}

void fw_distribut_update_list_init(fw_update_list_t *p_update_list, u32 update_node_cnt)
{
    foreach(i,update_node_cnt){
        fw_distribut_srv_proc.list[i].adr = p_update_list->update_list[i];
        fw_distribut_srv_proc.list[i].st_block_start = UPDATE_NODE_ST_IN_PROGRESS;
    }
    fw_distribut_srv_proc.node_cnt = update_node_cnt;
}

#if DISTRIBUTOR_START_TLK_EN
int is_par_distribute_start_tlk(u8 *par, int par_len)
{
	#if (GATEWAY_ENABLE || WIN32)
    fw_distribut_start_tlk_t *p_start_tlk = (fw_distribut_start_tlk_t *)par;
    fw_distribut_start_t *p_start_sig = (fw_distribut_start_t *)par;
    if(sizeof(fw_distribut_start_t) == par_len){
    	int valid_unicast_flag = (is_unicast_adr(p_start_sig->adr_group)&&(p_start_sig->adr_group != 0));
    	if(is_unicast_adr(p_start_tlk->adr_group)){
    		return (valid_unicast_flag);
    	}else{
    		if((DEFAULT_DISTRIBUTION_APPKEY_INDEX == p_start_sig->par.distrib_app_key_idx)&&(0 == valid_unicast_flag)){
    			return 0;
    		}
    		return 1; // because Distribution AppKey Index is 12 bit
    	}
    }else{
    	return 1;
    }
    #else
    return 0;
    #endif
}

extern const u8 BLOB_ID_LOCAL[8];

int mesh_cmd_sig_fw_distribut_start_tlk(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	#if (WIN32 && DEBUG_SHOW_VC_SELF_EN)
    memset(&fw_update_srv_proc, 0, sizeof(fw_update_srv_proc)); // clear last OTA data, due to no reboot for VC.
    #endif
    
	#if PTS_TEST_BLOB_TRANSFER_FLOW_EN
	int blob_trans_flow_flag = (0 == cb_par);
	#endif

	u8 st = DISTRIBUT_ST_INTERNAL_ERROR;
    fw_distribut_start_tlk_t *p_start = (fw_distribut_start_tlk_t *)par;
    u32 update_node_cnt = (par_len - OFFSETOF(fw_distribut_start_tlk_t,update_list)) / 2;
	if( fw_distribut_srv_proc.ota_rcv_flag && 
		fw_distribut_srv_proc.st_distr == MASTER_OTA_ST_IDLE){
		// if it have the ota_rcv_flag,we will only set the para part .
		distribut_srv_proc_init(0);
		update_node_cnt = par_len/2;
		if(update_node_cnt){
            fw_distribut_update_list_init((fw_update_list_t *)par, update_node_cnt);
        }
		mesh_fw_distibut_set(0);
		mesh_fw_distribute_adr(1);
		return 0; 
	}
	if(fw_distribut_srv_proc.adr_set_flag){
		update_node_cnt = fw_distribut_srv_proc.node_cnt;
	}
    if(update_node_cnt > MESH_OTA_UPDATE_NODE_MAX || update_node_cnt == 0){
        LOG_MSG_ERR(TL_LOG_COMMON,0, 0,"update node cnt error: %d", update_node_cnt);
    	st = DISTRIBUT_ST_INSUFFICIENT_RESOURCE;
    }else if(fw_distribut_srv_proc.st_distr){   // confirm later
        #if 0
        int retransmit = (0 == memcmp(p_start,&fw_distribut_srv_proc.fw_id, OFFSETOF(fw_distribut_start_tlk_t,update_list)));
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
		if(fw_distribut_srv_proc.adr_set_flag == 0){
			distribut_srv_proc_init(0);
		}

        fw_distribut_srv_proc.adr_group = p_start->adr_group;
		#if WIN32
        if(ble_module_id_is_kmadongle()){
            if((1 == update_node_cnt) && (p_start->adr_group != GROUP_VAL_FW_INFO_GET_ALL)){
                if((p_start->update_list[0] == APP_get_GATT_connect_addr())
                #if DEBUG_SHOW_VC_SELF_EN
                ||(p_start->update_list[0] == ele_adr_primary)
                #endif
                ){
                    fw_distribut_srv_proc.adr_group = p_start->update_list[0];  // use unicast address to send chunks to get segment ack
                }
            }
        }
		#endif
        
        fw_distribut_srv_proc.adr_distr_node = ele_adr_primary;
        if((0 == is_only_get_fw_info_fw_distribut_srv()) && (0 != read_ota_file2buffer())){
            distribut_srv_proc_init(0);
            return 0;   // error
        }
        
        if(update_node_cnt && fw_distribut_srv_proc.adr_set_flag == 0){
            fw_distribut_update_list_init((fw_update_list_t *)(p_start->update_list), update_node_cnt);
        }

        #if WIN32 
        if(is_only_get_fw_info_fw_distribut_srv()){
            mesh_ota_master_next_st_set(MASTER_OTA_ST_FW_UPDATE_INFO_GET);
        }else
        #endif
        {
        	mesh_fw_distribute_adr(0);
        	#if PTS_TEST_BLOB_TRANSFER_FLOW_EN
        	if (blob_trans_flow_flag){
            	mesh_ota_master_next_st_set(MASTER_OTA_ST_BLOB_INFO_GET);
            }else
            #endif
        	{
            	mesh_ota_master_next_st_set(MASTER_OTA_ST_DISTRIBUT_START);
            }
        }
        
	    st = DISTRIBUT_ST_SUCCESS;
	}

	#if PTS_TEST_BLOB_TRANSFER_FLOW_EN
	if(blob_trans_flow_flag){
		fw_distribut_srv_proc.blob_flow_flag = 1;
		memcpy(fw_distribut_srv_proc.blob_id, BLOB_ID_LOCAL, sizeof(fw_distribut_srv_proc.blob_id));
		return 0;
	}else
	#endif
	{
		return mesh_fw_distribut_st_rsp(cb_par, st);
	}
}
#endif

u8 check_fw_distribut_start_sig_st(fw_distribut_start_t *p_start)
{
	u8 st; // don't init to indicate warning.
	int same_start_flag = (0 == memcmp(&fw_distribut_srv_proc.distribut_start, p_start, sizeof(fw_distribut_start_t)));
	#if PTS_TEST_OTA_EN
	LOG_MSG_LIB(TL_LOG_NODE_BASIC, 0, 0,"same_start_flag:%d, st:%d, phase:%d", same_start_flag, fw_distribut_srv_proc.st_distr, fw_distribut_srv_proc.distribut_update_phase);
	#endif

	if(fw_distribut_srv_proc.suspend_flag){
		if(same_start_flag){
			st = DISTRIBUT_ST_KEEP_CURRENT;
		}else{
			st = DISTRIBUT_ST_BUSY_WITH_DISTRIBUTION;
		}
	}else if((p_start->par.distrib_app_key_idx != DEFAULT_DISTRIBUTION_APPKEY_INDEX)
    && (0 == is_exist_bind_key(&model_mesh_ota.fw_distr_srv.com, p_start->par.distrib_app_key_idx))){
	    st = DISTRIBUT_ST_INVALID_APPKEY_IDX;
    }else if((0 != p_start->par.distrib_fw_image_index) // only one firmware for telink
    		|| (0 == fw_distribut_srv_proc.upload_crc_ok)){ // pts may distribute many times, and command may retry // has been clear in mesh_cmd_sig_fw_distribut_start_sig_()
	    st = DISTRIBUT_ST_FW_NOT_FOUND;
    }else if(DISTRIBUT_PHASE_CANCELING_UPDATE == fw_distribut_srv_proc.distribut_update_phase){
    	st = DISTRIBUT_ST_BUSY_WITH_DISTRIBUTION;
    }else if(0 == fw_distribut_srv_proc.node_cnt){
    	st = DISTRIBUT_ST_RECEIVES_LIST_EMPTY;
    }else if(MASTER_OTA_ST_DISTRIBUT_RECEIVER_ADD_OR_UPLOAD_START == fw_distribut_srv_proc.st_distr){
	    st = DISTRIBUT_ST_SUCCESS;
    }else if((fw_distribut_srv_proc.st_distr >= MASTER_OTA_ST_DISTRIBUT_CANCEL)
    #if PTS_TEST_OTA_EN
    // because when in cancel state, it may be jump to MASTER_OTA_ST_FW_UPDATE_INFO_GET_AFTER_APPLY_ in mesh_cmd_sig_fw_distribut_get_()
    || ((fw_distribut_srv_proc.st_distr >= MASTER_OTA_ST_FW_UPDATE_INFO_GET_AFTER_APPLY)
    	&&(DISTRIBUT_PHASE_COMPLETED == fw_distribut_srv_proc.distribut_update_phase))
    #endif
    ){
    	if(DISTRIBUT_PHASE_COMPLETED == fw_distribut_srv_proc.distribut_update_phase){
    		if(same_start_flag){
    			st = DISTRIBUT_ST_KEEP_CURRENT;
    		}else{
    			st = DISTRIBUT_ST_SUCCESS; // allow a new distribute start.
    		}
    	}else if(DISTRIBUT_PHASE_FAILED == fw_distribut_srv_proc.distribut_update_phase){
    		st = DISTRIBUT_ST_SUCCESS; // allow a new distribute start.
    	}else{
    		if(same_start_flag){
				st = DISTRIBUT_ST_KEEP_CURRENT;
    		}else{
				st = DISTRIBUT_ST_BUSY_WITH_DISTRIBUTION;
			}
    	}
    }else if(MASTER_OTA_ST_IDLE == fw_distribut_srv_proc.st_distr){
	    st = DISTRIBUT_ST_WRONG_PHASE;
    }else{
		if(same_start_flag){
			st = DISTRIBUT_ST_KEEP_CURRENT;
		}else{
			st = DISTRIBUT_ST_BUSY_WITH_DISTRIBUTION; // allow a new distribute start.
		}
    }

    return st;
}

#if DEBUG_SHOW_VC_SELF_EN
int is_only_VC_self_OTA()
{
	if(1 == fw_distribut_srv_proc.node_cnt){
		if(fw_distribut_srv_proc.list[0].adr == ele_adr_primary){
			return 1;
		}
	}
	return 0;
}
#endif


int mesh_cmd_sig_fw_distribut_start_sig(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    fw_distribut_start_t *p_start = (fw_distribut_start_t *)par;
	u8 st = check_fw_distribut_start_sig_st(p_start);
    if(DISTRIBUT_ST_KEEP_CURRENT == st){
		fw_distribut_srv_proc.suspend_flag = 0;
	    st = DISTRIBUT_ST_SUCCESS;
    }else if(DISTRIBUT_ST_SUCCESS == st){
		//distribut_srv_proc_init(0);
		fw_distribut_srv_proc.distribut_reboot_flag_backup = fw_update_srv_proc.reboot_flag_backup;
        memset(&fw_update_srv_proc, 0, sizeof(fw_update_srv_proc)); // because blob transfer has run before.
		
        memcpy(&fw_distribut_srv_proc.distribut_start, p_start, sizeof(fw_distribut_srv_proc.distribut_start));
        fw_distribut_srv_proc.adr_group = p_start->adr_group;
        #if DEBUG_SHOW_VC_SELF_EN
        if(is_only_VC_self_OTA()){
        	fw_distribut_srv_proc.adr_group = ele_adr_primary;
        }
        #endif
        
        fw_distribut_srv_proc.adr_distr_node = ele_adr_primary;
        fw_distribut_srv_proc.distribut_update_phase = DISTRIBUT_PHASE_TRANSFER_ACTIVE;
        if(UPDATE_POLICY_VERIFY_APPLY == p_start->par.update_policy){
            fw_distribut_srv_proc.distribut_apply_en = 1;
        }
        
        #if WIN32
        memcpy(fw_ota_data_tx, fw_ota_data_rx, sizeof(fw_ota_data_tx));
        memset(fw_ota_data_rx, 0, sizeof(fw_ota_data_rx));
        #endif
        
        mesh_ota_master_next_st_set(MASTER_OTA_ST_DISTRIBUT_START);
	    st = DISTRIBUT_ST_SUCCESS;
	}

	return mesh_fw_distribut_st_rsp(cb_par, st);
}

int mesh_cmd_sig_fw_distribut_start(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	u8 st = DISTRIBUT_ST_INTERNAL_ERROR;
	u8 ak_arr_idx = APP_KEY_MAX; // invalid key index as default.
	
	#if DISTRIBUTOR_START_TLK_EN
	if(is_par_distribute_start_tlk(par, par_len)){
		fw_distribut_srv_proc.suspend_flag = 0;
	    st = mesh_cmd_sig_fw_distribut_start_tlk(par, par_len, cb_par);
	}else
	#endif
	{
	    st = mesh_cmd_sig_fw_distribut_start_sig(par, par_len, cb_par);
		
		fw_distribut_start_t *p_start = (fw_distribut_start_t *)par;
		ak_arr_idx = get_ak_arr_idx((u8)fw_distribut_srv_proc.netkey_sel_enc, p_start->par.distrib_app_key_idx);
	}

	fw_distribut_srv_proc.netkey_sel_enc = mesh_key.netkey_sel_dec;
	if(APP_KEY_MAX == ak_arr_idx){
		fw_distribut_srv_proc.appkey_sel_enc = mesh_key.appkey_sel_dec;
	}
	
	return st;
}

int mesh_cmd_sig_fw_distribut_apply(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	u8 st;
	if(is_distribute_st_with_optional_par(DISTRIBUT_ST_WRONG_PHASE) // suppose it would be a segment response
	 && is_busy_tx_seg(cb_par->adr_src)){
		return 0;	// can not tx status.
	}

	u8 distributor_st = fw_distribut_srv_proc.st_distr;
	if(fw_distribut_srv_proc.suspend_flag){
        st = DISTRIBUT_ST_WRONG_PHASE;
	}else if((MASTER_OTA_ST_WAITING_INITIATOR_UPDATE_APPLY == distributor_st)
     ||(MASTER_OTA_ST_UPDATE_GET == distributor_st)
     ||(distributor_st >= MASTER_OTA_ST_UPDATE_APPLY)){
        fw_distribut_srv_proc.distribut_apply_en = 1;
        if(DISTRIBUT_PHASE_TRANSFER_SUCCESS == fw_distribut_srv_proc.distribut_update_phase){
        	fw_distribut_srv_proc.distribut_update_phase = DISTRIBUT_PHASE_APPLYING_UPDATE;
        }

		if((DISTRIBUT_PHASE_TRANSFER_SUCCESS == fw_distribut_srv_proc.distribut_update_phase)
		|| (DISTRIBUT_PHASE_APPLYING_UPDATE == fw_distribut_srv_proc.distribut_update_phase)
		|| (DISTRIBUT_PHASE_COMPLETED == fw_distribut_srv_proc.distribut_update_phase)
		#if (!PTS_TEST_OTA_EN)
		// even though some nodes may have not received "update get", response success is ok due to distribut_apply_en.
		// if not, app will cause failed when receive wrong phase, because app only check 1st node in the list.
		|| (MASTER_OTA_ST_UPDATE_GET == distributor_st)
		#endif
		){
        	st = DISTRIBUT_ST_SUCCESS;
        }else{
			st = DISTRIBUT_ST_WRONG_PHASE;
        }
    }else{
        st = DISTRIBUT_ST_WRONG_PHASE;
    }

	return mesh_fw_distribut_st_rsp(cb_par, st);
}

#if GATEWAY_ENABLE
#define CLEAR_TEMP_FIRMWARE_EN		0 // should not auto reboot to clear firmware for gateway, and firmware in backup area will be replaced by button of "GwMeshOta" or by command "MESH_OTA_ERASE_CTL". 
#else
#define CLEAR_TEMP_FIRMWARE_EN		1
#endif

int mesh_cmd_sig_fw_distribut_suspend(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	u8 st = DISTRIBUT_ST_SUCCESS;
	if((fw_distribut_srv_proc.st_distr >= MASTER_OTA_ST_DISTRIBUT_START)
	&& ((fw_distribut_srv_proc.distribut_update_phase < DISTRIBUT_PHASE_TRANSFER_SUCCESS)||(fw_distribut_srv_proc.distribut_update_phase == DISTRIBUT_PHASE_TRANSFER_SUSPEND))){ // DFU/SR/FD/BV-58-C need to be suspend success if DISTRIBUT_PHASE_TRANSFER_SUCCESS. Firmware Distribution Suspend - Transfer Success Distribution Phase
		fw_distribut_srv_proc.suspend_flag = 1;
		st = DISTRIBUT_ST_SUCCESS;
	}else{
		st = DISTRIBUT_ST_WRONG_PHASE;
	}
	int err = mesh_fw_distribut_st_rsp(cb_par, st);
	return err;
}

int mesh_cmd_sig_fw_distribut_cancel(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int abort_flag = 0;
	u8 st = DISTRIBUT_ST_SUCCESS;

	#if PTS_TEST_OTA_EN
	LOG_MSG_LIB(TL_LOG_NODE_BASIC, 0, 0,"before cancel,dist st:%d,phase:%d, node 0 phase:%d", fw_distribut_srv_proc.st_distr, fw_distribut_srv_proc.distribut_update_phase, fw_distribut_srv_proc.list[0].update_phase);
	#endif
	
    u8 phase_backup = fw_distribut_srv_proc.distribut_update_phase;
    u8 st_distr_backup = fw_distribut_srv_proc.st_distr;

    if(fw_distribut_srv_proc.st_distr && fw_distribut_srv_proc.st_distr != MASTER_OTA_ST_MAX){	// when ST MAX, fw update apply or cancel have been sent, so that can not send update cancel command to other nodes here.
        abort_flag = 1;
    }

    if((DISTRIBUT_PHASE_IDLE == phase_backup) || (DISTRIBUT_PHASE_FAILED == phase_backup)){
    	#if PTS_OTA_TODO_EN
    	if((0 == fw_distribut_srv_proc.distribut_update_phase_keep_flag)) // FD/BV-41, TODO
    	#endif
		{distribut_srv_proc_init(0);}
	#if 1 // FN/BV-35:need to be idle,but FD/BV-41, still need to be able to cancel, even when DISTRIBUT_PHASE_COMPLETED.
    }else if((DISTRIBUT_PHASE_COMPLETED == phase_backup)
    	#if 0//PTS_OTA_TODO_EN	// only enable for FD/BV-41 // TS_CASE_FD_BV41_EN
    && 0	// update_phase of node 0 in BV35 is 6, and is 6 for BV41. FD/BV-41 need to be able to cancel not idle
    	#endif
    ){
    	fw_distribut_srv_proc.distribut_update_phase = DISTRIBUT_PHASE_IDLE;
    #endif
    }else{
    	fw_distribut_srv_proc.distribut_update_phase = DISTRIBUT_PHASE_CANCELING_UPDATE;
    }
    
	fw_distribut_srv_proc.suspend_flag = 0;

    if(is_distribute_st_with_optional_par(st) && is_busy_tx_seg(cb_par->adr_src)){
    	if(PTS_TEST_OTA_EN || BLOB_CHUNK_TRANSFER == mesh_tx_seg_par.match_type.mat.op){
        	memset(&mesh_tx_seg_par, 0, sizeof(mesh_tx_seg_par));    // init	// discard current tx segment. make sure distribution cancel can run.
		}else{
			LOG_MSG_ERR(TL_LOG_NODE_BASIC, 0, 0,"tx segment busy, can not tx response with segment");
			return 0;
		}
    }
        
	int err = mesh_fw_distribut_st_rsp(cb_par, st);

	if(DISTRIBUT_ST_SUCCESS == st){
		if(abort_flag
		#if PTS_OTA_TODO_EN
		|| (fw_distribut_srv_proc.distribut_update_phase_keep_flag) //  // FD/BV-41, TODO
		#endif
		){
			#if 1
				#if PTS_OTA_TODO_EN // FD/BV-35 need, because it can cause PTS failed to receive distribute status
			if((DISTRIBUT_PHASE_IDLE != fw_distribut_srv_proc.distribut_update_phase)
			&& (DISTRIBUT_PHASE_COMPLETED != fw_distribut_srv_proc.distribut_update_phase))
				#endif
			{
				#if (CLEAR_TEMP_FIRMWARE_EN)
				access_cmd_fw_update_control(ele_adr_primary, FW_UPDATE_CANCEL, 0);
				#endif
				u16 chunk_cmd_dst_addr = get_chunk_cmd_dst_addr();
				if((st_distr_backup != MASTER_OTA_ST_MAX) && chunk_cmd_dst_addr){
					#if RELIABLE_CMD_EN
			        memset(&mesh_tx_reliable, 0, sizeof(mesh_tx_reliable));
					#endif
					#if PTS_TEST_OTA_EN
					fw_distribut_srv_proc.update_cancel_addr = chunk_cmd_dst_addr;
					#endif
			        access_cmd_fw_update_control(chunk_cmd_dst_addr, FW_UPDATE_CANCEL, min(fw_distribut_srv_proc.node_cnt, 0xff));
		        }
	        }
			#else
			access_cmd_fw_update_control(ADR_ALL_NODES, FW_UPDATE_CANCEL, 0xff);
			#endif
		}else{
	        #if VC_APP_ENABLE
	        extern int disable_log_cmd;
	        disable_log_cmd = 1;   // mesh OTA finished
	        #else
	        if(MASTER_OTA_ST_MAX == st_distr_backup){
				#if CLEAR_TEMP_FIRMWARE_EN
				if(0 == ota_reboot_later_tick){
					mesh_ota_reboot_set(OTA_REBOOT_NO_LED); // to clear temporary fw data in flash.
				}
				#endif
	        }
	        #endif
		}

		#if 1
			#if PTS_TEST_OTA_EN
		// for FD/BV-34, because when rx distribute start during canceling, it need to response busy with distribute.
		if((0 == fw_distribut_srv_proc.tick_dist_canceling) && (DISTRIBUT_PHASE_CANCELING_UPDATE == fw_distribut_srv_proc.distribut_update_phase)){
			fw_distribut_srv_proc.tick_dist_canceling = clock_time()|1;
			// can't clear at once for FD/BV-37/38/40/41/51,  because it is expected be still canceling after the second distribute cancel. 
			// but must clear for FD/BV-37 before the 3rd "distribution cancel" and then response success with idle phase.
		}else
			#endif
		{
			distribut_srv_proc_init(0);
		}
		#else
		#if PTS_OTA_TODO_EN
		if(((DISTRIBUT_PHASE_CANCELING_UPDATE == fw_distribut_srv_proc.distribut_update_phase)
		        || (DISTRIBUT_PHASE_COMPLETED == phase_backup) // FD/BV-41
		)&&((fw_distribut_srv_proc.st_distr >= MASTER_OTA_ST_UPDATE_START) && (0 == is_distributor_all_receivers_invalid()))){
			/* in these cases, need to keep distribution canceling phase, but when to init ? TODO later
			 * FD/BV-40 active firmware update phase
			 * but FD/BV-37, 38 need init after 1st cancel, and were to get success with idle phase at 2nd cancel.
			*/
			//fw_distribut_srv_proc.distribut_update_phase_keep_flag = 1; // can't set for BV37,38,41
			// LOG_MSG_ERR(TL_LOG_NODE_BASIC, 0, 0,"xxxxxx need keep flag ??? ");
		}
		
		if((0 == fw_distribut_srv_proc.distribut_update_phase_keep_flag))
		#endif
		{
			distribut_srv_proc_init(0);
		}
		#endif
	}
	
	return err;
}

// -------
#if (DISTRIBUTOR_UPDATE_SERVER_EN)
#if (!WIN32)
STATIC_ASSERT(sizeof(fw_distribut_receiver_list_t) < MESH_CMD_ACCESS_LEN_MAX);
#endif

int is_distributor_and_in_apply_list(u16 addr, bool4 no_update_start)
{
    if((!fw_update_srv_proc.busy) && is_rx_upload_start_before()){
        foreach(i,fw_distribut_srv_proc.node_cnt){
            if(fw_distribut_srv_proc.list[i].adr == addr){
                if(no_update_start){
                    if(fw_distribut_srv_proc.list[i].skip_flag){
                        return 1;
                    }else{
                        return 0;
                    }
                }else{
                    return 1;
                }
            }
        }
    }
    return 0;
}

int is_distributor_in_apply_list_and_need_ota(u16 addr, bool4 no_update_start)
{
    if(is_distributor_and_in_apply_list(addr, no_update_start)){
        fw_distribut_upload_start_t *p_start = &fw_distribut_srv_proc.upload_start;
        if(mesh_ota_slave_need_ota(&p_start->fw_metadata, p_start->fw_metadata_len)){
            return 1;
        }
    }

    return 0;
}

#if DISTRIBUTOR_NO_UPDATA_START_2_SELF
u8 get_update_phase_for_distributor()
{
    u8 update_phase = UPDATE_PHASE_VERIFYING_UPDATE;

    if(fw_distribut_srv_proc.st_distr >= MASTER_OTA_ST_WAITING_INITIATOR_UPDATE_APPLY){
        if(fw_distribut_srv_proc.distribut_apply_self_ok){
            update_phase = UPDATE_PHASE_APPLYING_UPDATE;
        }else{ // update get, receiver get
            if(fw_distribut_srv_proc.node_cnt <= 1){
                update_phase = UPDATE_PHASE_VERIFYING_SUCCESS;
            }else{
                foreach(j,fw_distribut_srv_proc.node_cnt){
                    fw_receiver_list_t *p_list_other = &fw_distribut_srv_proc.list[j];
                    if(!is_own_ele(p_list_other->adr) && (!p_list_other->skip_flag)){
                        u8 update_phase = p_list_other->update_phase; // get phase from the first node, because there is no update start flow for distributor.
                        if(update_phase >= UPDATE_PHASE_VERIFYING_SUCCESS){
                            update_phase = UPDATE_PHASE_VERIFYING_SUCCESS;
                        }
                        break;
                    }
                }
            }
        }
    }

    return update_phase;
}
#endif

int mesh_cmd_sig_fw_distribut_receiver_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    fw_distribut_receiver_get_t *p_get = (fw_distribut_receiver_get_t *)par;
    fw_distribut_receiver_list_t rsp = {0};
    u32 rsp_size = OFFSETOF(fw_distribut_receiver_list_t,node);
    rsp.first_index = p_get->first_index;
    if(p_get->entries_limit > 0){
        if(0 == fw_distribut_srv_proc.st_distr){
            #if (!WIN32)
            if(is_mesh_ota_distribute_100_flag()){
                rsp_size += sizeof(fw_distribut_update_node_entry_t);
                rsp.node[0].adr = ele_adr_primary;
                rsp.node[0].transfer_progress = TRANSFER_PROGRESS_MAX;
            }
            #endif
        }else{
            u32 node_rsp_max = fw_distribut_srv_proc.node_cnt;
            if(node_rsp_max > ARRAY_SIZE(rsp.node)){
                node_rsp_max = ARRAY_SIZE(rsp.node);
            }
            rsp.list_cnt = (u16)node_rsp_max;
            
            LOG_MSG_LIB(TL_LOG_NODE_BASIC, 0, 0,"      receivers get cnt:%d,first:%d,max:%d",rsp.list_cnt,p_get->first_index,node_rsp_max);
            if(p_get->first_index < rsp.list_cnt){
                if(p_get->entries_limit > node_rsp_max){
                    p_get->entries_limit = node_rsp_max;
                }
                
                foreach(i, p_get->entries_limit){
                    fw_distribut_update_node_entry_t *p_node = &rsp.node[i];
                    fw_receiver_list_t *p_list = &fw_distribut_srv_proc.list[i+p_get->first_index];
                    rsp_size += sizeof(fw_distribut_update_node_entry_t);
                    p_node->adr = p_list->adr;
                    p_node->update_phase = p_list->update_phase;
                    #if DISTRIBUTOR_NO_UPDATA_START_2_SELF
                    int flag_ota_self = (is_own_ele(p_node->adr) && is_distributor_and_in_apply_list(p_node->adr, 1));
                    if(flag_ota_self){
                        p_node->update_phase = get_update_phase_for_distributor();
                    }
                    #endif
                    
                    if(UPDATE_PHASE_APPLYING_UPDATE == p_node->update_phase){
                    	#if PTS_TEST_OTA_EN
                    	if(fw_distribut_srv_proc.st_distr >= MASTER_OTA_ST_FW_UPDATE_INFO_GET_AFTER_APPLY)
                    	#endif
                    	{
                    		p_node->update_phase = p_list->verify_fail_flag ? RETRIEVED_UPDATA_PHASE_APPLY_FAILED : RETRIEVED_UPDATA_PHASE_APPLY_SUCCESS;
                    		#if PTS_TEST_OTA_EN
							fw_distribut_srv_proc.distribut_update_phase = DISTRIBUT_PHASE_COMPLETED;
							LOG_MSG_LIB (TL_LOG_NODE_BASIC, 0, 0, "dist phase to completed");
							#endif
                    	}
                    }

                    p_node->update_st = p_list->st_block_start;
                    //p_node->transfer_st = fw_distribut_srv_proc.list[i].;
                    // progress "+1" for setting to 50 while in state of "MASTER_OTA_ST_UPDATE_GET", because "apply" can be accepted during this time.
                    p_node->transfer_progress = min((fw_distribut_srv_proc.percent_last + 1)/2, TRANSFER_PROGRESS_MAX);
                    if(p_list->skip_flag && (p_list->update_phase <= UPDATE_PHASE_VERIFYING_SUCCESS)
                    #if DISTRIBUTOR_NO_UPDATA_START_2_SELF
                    && (0 == flag_ota_self)
                    #endif
                    ){
                    	p_node->update_phase = UPDATE_PHASE_TRANSFER_ERR;
                    }
                }
            }
        }
    }
    
	model_g_light_s_t *p_model = (model_g_light_s_t *)cb_par->model;
	return mesh_tx_cmd_rsp(FW_DISTRIBUT_RECEIVERS_LIST, (u8 *)&rsp, rsp_size, p_model->com.ele_adr, cb_par->adr_src, 0, 0);
}
#endif

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
    if(is_rx_upload_start_before()){
    	cmd.timeout_base = fw_distribut_srv_proc.distribut_start.par.distrib_timeout_base;
    }else{
    	cmd.timeout_base = 0; // not use timeout now.
    }
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
        LOG_MSG_INFO (TL_LOG_CMD_NAME, 0, 0, "access_cmd_fw_update_apply ");
    }else if(FW_UPDATE_CANCEL == op){
        LOG_MSG_INFO (TL_LOG_CMD_NAME, 0, 0, "access_cmd_fw_update_cancel ");
    }else{
        LOG_MSG_ERR(TL_LOG_COMMON,0, 0,"error control op code");
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

int access_cmd_blob_transfer_start(u16 adr_dst, u8 transfer_mode, u32 blob_size, u8 bk_size_log)
{
    LOG_MSG_FUNC_NAME();
    blob_transfer_start_t cmd = {0};
    cmd.transfer_mode = transfer_mode;
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
    p_bk_start->chunk_size = g_blob_info_status.chunk_size_max;
    fw_distribut_srv_proc.bk_size_current = distribut_get_block_size(block_num);

	return SendOpParaDebug(adr_dst, 1, BLOB_BLOCK_START, (u8 *)p_bk_start, sizeof(blob_block_start_t));
}

/**
 * @brief  
 * @param chunk_num: siries of chunk number
 * @param cnt: count of chunk number.
 * @ret val:
 */
#if MESH_OTA_PULL_MODE_EN
int access_cmd_blob_partial_block_report(u16 adr_dst, u16 chunk_num[], u32 cnt)
{
    u8 buf_enc[min(64, PULL_MODE_MAX_MISSING_CHUNK * UTF8_SIZE_MAX_U16)] = {0};
    u32 len_utf8 = 0;
    foreach(i,cnt){
        len_utf8 += U16ToUTF8(chunk_num[i], buf_enc + len_utf8, sizeof(buf_enc) - len_utf8);
    }

    if(len_utf8 > sizeof(buf_enc)){
        return -1;
    }
    
	return SendOpParaDebug(adr_dst, 0, BLOB_PARTIAL_BLOCK_REPORT, buf_enc, len_utf8);
}
#endif

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
fw_receiver_list_t * get_fw_node_receiver_list(u16 node_adr)
{
    foreach(i,fw_distribut_srv_proc.node_cnt){
        fw_receiver_list_t *p_list = &fw_distribut_srv_proc.list[i];
        if(p_list->adr == node_adr){
            return p_list;
        }
    }
    return 0;
}

int is_all_valid_receiver_applying_ok()
{
    foreach(i,fw_distribut_srv_proc.node_cnt){
        fw_receiver_list_t *p_list = &fw_distribut_srv_proc.list[i];
        if((0 == p_list->skip_flag) && (UPDATE_PHASE_VERIFYING_UPDATE == p_list->update_phase)){
            return 0;
        }
    }
    return 1;
}

u32 is_need_block_transfer()
{
    foreach(i,fw_distribut_srv_proc.node_cnt){
        fw_receiver_list_t *p_list = &fw_distribut_srv_proc.list[i];
        if(BLOB_TRANS_ST_SUCCESS == p_list->st_block_start){
            return 1;
        }
    }
    return 0;
}

#if DISTRIBUTOR_UPDATE_SERVER_EN
int is_busy_upload_receiver_change()
{
	u8 distributor_st = fw_distribut_srv_proc.st_distr;
	if(distributor_st >= MASTER_OTA_ST_DISTRIBUT_START){
		if(fw_distribut_srv_proc.distribut_update_phase >= DISTRIBUT_PHASE_COMPLETED){
			return 0;
		}
	}

	#if 1
	return (distributor_st >= MASTER_OTA_ST_DISTRIBUT_START); // should be able to delete all receivers before distribution start, because App may need to do that when OTA fail at upload state at last mesh OTA flow.
	#else
	return ((fw_distribut_srv_proc.rx_upload_start_flag && (UPLOAD_PHASE_TRANSFER_SUCCESS != fw_distribut_srv_proc.upload_phase)) || (distributor_st >= MASTER_OTA_ST_DISTRIBUT_START));
	#endif
}

int mesh_cmd_sig_fw_distribut_capabilities_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	model_g_light_s_t *p_model = (model_g_light_s_t *)cb_par->model;
    fw_distribut_capabilities_status_t rsp = {0};
    rsp.max_receivers_size = MESH_OTA_UPDATE_NODE_MAX;
    rsp.max_fw_image_list_size = MESH_OTA_IMAGE_LIST_SIZE_MAX;
    rsp.max_fw_image_size = MESH_OTA_BLOB_SIZE_MAX;
    rsp.max_upload_space = MESH_OTA_BLOB_SIZE_MAX;
    rsp.remain_upload_space = MESH_OTA_BLOB_SIZE_MAX;
    rsp.oob_retrieval = RETRIEVAL_OOB_NOT_SUPPORTED;
    
	return mesh_tx_cmd_rsp(FW_DISTRIBUT_CAPABILITIES_STATUS, (u8 *)&rsp, sizeof(rsp), p_model->com.ele_adr, cb_par->adr_src, 0, 0);
}

int mesh_fw_distribut_receivers_rsp(mesh_cb_fun_par_t *cb_par, u8 st)
{
	model_g_light_s_t *p_model = (model_g_light_s_t *)cb_par->model;
    fw_distribut_receivers_status_t rsp = {0};
    rsp.st = st;
    rsp.list_cnt = fw_distribut_srv_proc.node_cnt;

	return mesh_tx_cmd_rsp(FW_DISTRIBUT_RECEIVERS_STATUS, (u8 *)&rsp, sizeof(rsp), p_model->com.ele_adr, cb_par->adr_src, 0, 0);
}

int mesh_cmd_sig_fw_distribut_receivers_add(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    u8 st = DISTRIBUT_ST_SUCCESS;
    fw_distribut_receivers_add_t *p_add = (fw_distribut_receivers_add_t *)par;
    if(fw_distribut_srv_proc.st_distr){
		if(is_busy_upload_receiver_change()){
	    	st = DISTRIBUT_ST_BUSY_WITH_DISTRIBUTION;
	    	goto LABEL_RSP_DISTRIBUTE_RECEIVER;
		}else{
			// allow add
		}
    }else{
        distribut_srv_proc_init(1); // can not clear new firmware size
        mesh_ota_master_next_st_set(MASTER_OTA_ST_DISTRIBUT_RECEIVER_ADD_OR_UPLOAD_START);
	    st = DISTRIBUT_ST_SUCCESS;
	}
	
#if (!WIN32)
    if(is_mesh_ota_distribute_100_flag()){
        clr_mesh_ota_distribute_100_flag();
    }
#endif

    int par_valid_flag = 1;
    u16 entry_cnt = par_len / sizeof(fw_receive_entry_t);
    foreach(i,entry_cnt){
    	if(!is_unicast_adr(p_add->entry[i].addr) || (ADR_UNASSIGNED == p_add->entry[i].addr)){
			st = DISTRIBUT_ST_INTERNAL_ERROR;
			par_valid_flag = 0;
    		break;
    	}
    }

    if(par_valid_flag){
	    foreach(i,entry_cnt){
			fw_receiver_list_t *p_list_exist = 0;
			foreach(j_exist, (fw_distribut_srv_proc.node_cnt)){
				if( p_add->entry[i].addr == fw_distribut_srv_proc.list[j_exist].adr){
					p_list_exist = &fw_distribut_srv_proc.list[j_exist];;
					break;
				}
			}
			
	        if(p_list_exist || (fw_distribut_srv_proc.node_cnt < ARRAY_SIZE(fw_distribut_srv_proc.list))){
	            fw_receiver_list_t *p_list;
	            if(p_list_exist){
	                p_list = p_list_exist;
	            }else{
					p_list = &fw_distribut_srv_proc.list[fw_distribut_srv_proc.node_cnt];
	                fw_distribut_srv_proc.node_cnt++;
	            }
	            
	            p_list->adr = p_add->entry[i].addr;
	            p_list->update_fw_image_idx = p_add->entry[i].update_fw_image_idx;
	            #if DISTRIBUTOR_NO_UPDATA_START_2_SELF
	            if(is_own_ele(p_list->adr)){
	                p_list->skip_flag = 1;    // because no handle in is_tx_status_cmd2self(), and handle in the apply setp is enough.
	            }
	            #endif
	        }else{
				st = DISTRIBUT_ST_INSUFFICIENT_RESOURCE;
	            break;
	        }
	    }
    }

LABEL_RSP_DISTRIBUTE_RECEIVER:
	return mesh_fw_distribut_receivers_rsp(cb_par, st);
}

int mesh_cmd_sig_fw_distribut_receivers_del_all(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    u8 st = DISTRIBUT_ST_SUCCESS;
    if(is_busy_upload_receiver_change()){
		st = DISTRIBUT_ST_BUSY_WITH_DISTRIBUTION;
    }else{
	    memset(&fw_distribut_srv_proc.list, 0, sizeof(fw_distribut_srv_proc.list));
	    fw_distribut_srv_proc.node_cnt = fw_distribut_srv_proc.node_num = 0;
    }
        
	return mesh_fw_distribut_receivers_rsp(cb_par, st);
}

int mesh_fw_distribut_upload_st_rsp(mesh_cb_fun_par_t *cb_par, u8 st)
{
	model_g_light_s_t *p_model = (model_g_light_s_t *)cb_par->model;
	fw_distribut_upload_status_t rsp = {0};
	u32 rsp_len = 2; // OFFSETOF(fw_distribut_upload_status_t, upload_progress);
	rsp.st = st;
	rsp.upload_phase = fw_distribut_srv_proc.upload_phase;
	#if PTS_TEST_OTA_EN
	if((FW_DISTRIBUT_UPLOAD_GET == cb_par->op) && (BLOB_TRANS_PHASE_WAIT_NEXT_BLOCK == fw_update_srv_proc.blob_trans_phase)){
		if(MASTER_OTA_ST_DISTRIBUT_RECEIVER_ADD_OR_UPLOAD_START == fw_distribut_srv_proc.st_distr){
			//fw_distribut_srv_proc.upload_phase = UPLOAD_PHASE_TRANSFER_ERROR; // why ? // TBD: SR/FD/BV-07-C need error, but BV05 needs active
			//rsp.upload_phase = UPLOAD_PHASE_TRANSFER_ERROR; // why ? // TBD: SR/FD/BV-07-C need error // just for pre-condition setting ?
		}
	}
	#endif
	if(fw_distribut_srv_proc.rx_upload_start_flag){
        rsp_len = sizeof(fw_distribut_upload_status_t);
        rsp.upload_progress = fw_distribut_srv_proc.upload_progress;
        #if 0
        // BV11 need to be like this, which conflict with BV18
        if(FW_DISTRIBUT_UPLOAD_START == cb_par->op){
        	rsp.upload_type = UPLOAD_TYPE_INBAND; // because when busy, should not report the old one.
        }else if(FW_DISTRIBUT_UPLOAD_OOB_START == cb_par->op){
        	rsp.upload_type = UPLOAD_TYPE_OUTOFBAND;
        }else
        #endif
        {
        	rsp.upload_type = fw_distribut_srv_proc.upload_type;
        }
        memcpy(&rsp.fw_id, &fw_distribut_srv_proc.upload_start.fw_id, sizeof(&rsp.fw_id));
	}

	return mesh_tx_cmd_rsp(FW_DISTRIBUT_UPLOAD_STATUS, (u8 *)&rsp, rsp_len, p_model->com.ele_adr, cb_par->adr_src, 0, 0);
}

int mesh_cmd_sig_fw_distribut_upload_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	u8 st = DISTRIBUT_ST_SUCCESS;
#if UPLOAD_OOB_START_FUN_EN
	if(fw_distribut_srv_proc.upload_oob_start_flag){
		#if PTS_TEST_OTA_EN
		static u32 upload_get_cnt = 0;
		upload_get_cnt = upload_get_cnt;
		#if 0	// TEST_BV47_EN  // only for test BV47: OOB using HTTP. and "10" for not being triggered in other cases.
		if(++upload_get_cnt >= 2){
	    	fw_distribut_srv_proc.upload_phase = UPLOAD_PHASE_TRANSFER_ERROR;
	    	fw_distribut_srv_proc.upload_progress = 100;
		}else
		#endif
		{
	    	fw_distribut_srv_proc.upload_phase = UPLOAD_PHASE_TRANSFER_SUCCESS;
	    	fw_distribut_srv_proc.upload_progress = 100;
    	}
		#endif
    }else if(fw_distribut_srv_proc.rx_upload_start_flag){
		#if 1 // FD/BV 13
		if((BLOB_TRANS_PHASE_SUSPEND == fw_update_srv_proc.blob_trans_phase)
		&& (0 == fw_distribut_srv_proc.upload_progress)
		//&& (BLOB_TRANS_PHASE_WAIT_NEXT_BLOCK == fw_update_srv_proc.blob_trans_phase) // make sure have not received any firmware.
		){
	    	fw_distribut_srv_proc.upload_phase = UPLOAD_PHASE_TRANSFER_ERROR;
		}
		#endif
    }
#endif
	return mesh_fw_distribut_upload_st_rsp(cb_par, st);
}

int is_same_upload_start(fw_distribut_upload_start_t *p_start)
{
	return ((fw_distribut_srv_proc.upload_start.upload_fw_size == p_start->upload_fw_size)
			&& (0 == memcmp(&fw_distribut_srv_proc.upload_start.fw_id, &p_start->fw_id, sizeof(p_start->fw_id)))
			&& (fw_distribut_srv_proc.upload_start.upload_ttl  == p_start->upload_ttl)
			&& (fw_distribut_srv_proc.upload_start.upload_timeout_base  == p_start->upload_timeout_base)
			&& (fw_distribut_srv_proc.upload_start.fw_metadata_len  == p_start->fw_metadata_len)
			&& (0 == memcmp(&fw_distribut_srv_proc.upload_start.fw_metadata, &p_start->fw_metadata, p_start->fw_metadata_len)));
}

u8 check_fw_distribut_upload_start_st(fw_distribut_upload_start_t *p_start, int cover_allow)
{
    u8 st; // don't init to indicate warning
	if(fw_distribut_srv_proc.st_distr && is_rx_upload_start_before()){
		if(is_same_upload_start(p_start)){
			if(UPLOAD_PHASE_TRANSFER_ERROR == fw_distribut_srv_proc.upload_phase){
				fw_distribut_srv_proc.upload_phase = UPLOAD_PHASE_TRANSFER_ACTIVE; // FD/BV-07-C, FD/BV-08-C
			}
			st = DISTRIBUT_ST_KEEP_CURRENT;
		}else{
			st = cover_allow ? DISTRIBUT_ST_SUCCESS : DISTRIBUT_ST_BUSY_WITH_UPLOAD;
		}
	}else{
		st = DISTRIBUT_ST_SUCCESS;
	}

	return st;
}

int mesh_cmd_sig_fw_distribut_upload_start(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    u8 st = DISTRIBUT_ST_SUCCESS;
    fw_distribut_upload_start_t *p_start = (fw_distribut_upload_start_t *)par;
    int offset_fw_id = p_start->fw_metadata_len + OFFSETOF(fw_distribut_upload_start_t, fw_metadata);
    if(p_start->upload_fw_size > MESH_OTA_BLOB_SIZE_MAX){
    	st = DISTRIBUT_ST_INSUFFICIENT_RESOURCE;
#if UPLOAD_OOB_START_FUN_EN
    }else if(fw_distribut_srv_proc.upload_oob_start_flag){
    	st = DISTRIBUT_ST_BUSY_WITH_UPLOAD;
#endif
    }else{
	    if((par_len >= offset_fw_id) && (p_start->fw_metadata_len <= sizeof(p_start->fw_metadata))){
	    	int cover_allow = 0;
	    	#if PTS_TEST_OTA_EN
	    	cover_allow = ((MASTER_OTA_ST_DISTRIBUT_RECEIVER_ADD_OR_UPLOAD_START == fw_distribut_srv_proc.st_distr) && (UPLOAD_PHASE_TRANSFER_SUCCESS == fw_distribut_srv_proc.upload_phase)); // just allow cover for PTS
	    	#endif
	    	
	    	st = check_fw_distribut_upload_start_st(p_start, cover_allow);
	    	if(DISTRIBUT_ST_KEEP_CURRENT == st){
	    		st = DISTRIBUT_ST_SUCCESS;
	    	}else if(DISTRIBUT_ST_SUCCESS == st){
	    		#if UPLOAD_MULTI_FW_TEST_EN
	    		if(cover_allow){
	    			upload_multi_fw_id.upload_1st_en = 1;
					memcpy(&upload_multi_fw_id.fw_id_1st, &fw_distribut_srv_proc.upload_start.fw_id, sizeof(upload_multi_fw_id.fw_id_1st));
	    		}
	    		#endif
	    		
		    	if(0 == fw_distribut_srv_proc.st_distr || cover_allow){ // have not receive receiver add before
					distribut_srv_proc_init(0);
					mesh_ota_master_next_st_set(MASTER_OTA_ST_DISTRIBUT_RECEIVER_ADD_OR_UPLOAD_START);
		    	}
		    	
		        memcpy(&fw_distribut_srv_proc.upload_start, p_start, offset_fw_id);
		        new_fw_size = fw_distribut_srv_proc.upload_start.upload_fw_size;
		        if(par_len > offset_fw_id){
		            int len_id = par_len - offset_fw_id;
		            if(is_valid_fw_id_len(len_id)){
		                memcpy(&fw_distribut_srv_proc.upload_start.fw_id, &p_start->fw_id, len_id);
		                fw_distribut_srv_proc.upload_phase = UPLOAD_PHASE_TRANSFER_ACTIVE;
		                //fw_distribut_srv_proc.upload_type = UPLOAD_TYPE_INBAND;
		                fw_distribut_srv_proc.len_fw_id_upload_start = len_id;
		                fw_distribut_srv_proc.rx_upload_start_flag = 1;
		            }
		        }
	        }else{
	        	// report error status
	        }
	    }else{
			st = DISTRIBUT_ST_INTERNAL_ERROR;
	    }
    }
    
	return mesh_fw_distribut_upload_st_rsp(cb_par, st);
}

#if UPLOAD_OOB_START_FUN_EN
const u8 VALID_OOB_URI_HEAD[] = {"http"};	// include https
const u8 INVALID_OOB_URI_SAMPLE1[] = {"http://abc.com"};
const u8 INVALID_OOB_URI_SAMPLE2[] = {"https//abc.com"};

int is_valid_uri_path_suffix(fw_distribut_upload_oob_start_t *p_start, const u8 *p_suffix, u8 suffix_len)
{
	if(suffix_len){
		for(unsigned int i = 0; i + suffix_len <= (p_start->uri_len); ++i){
			if(0 == memcmp(p_start->uri+i, p_suffix, suffix_len)){
				return 1;
			}
		}
	}

	return 0;
}

#if 0
enum{
	URI_SUFFIX_CHECK = 0,
	URI_SUFFIX_GET,
};

int is_valid_oob_uri_suffix(fw_distribut_upload_oob_start_t *p_start, int suffix_type)
{
	// spec section of "8.2.2.1 Construction of the Request URI" and "8.2.3.1 Construction of the Request URI"
	// https://mesh.example.com/check-for-updates/check?cfwid=02FF10000203
	// https://mesh.example.com/check-for-updates/get?cfwid=02FF10000203
	// "/check?cfwid=" and "/get?cfwid="
	if(URI_SUFFIX_CHECK == suffix_type){
		const u8 str_check[] = {"/check?cfwid="};
		return is_valid_uri_path_suffix(p_start, str_check, sizeof(str_check) - 1);
	}else if(URI_SUFFIX_GET == suffix_type){
		const u8 str_get[] = {"/get?cfwid="};
		return is_valid_uri_path_suffix(p_start, str_get, sizeof(str_get) - 1);
	}

	return 0;
}
#endif
#endif

int mesh_cmd_sig_fw_distribut_upload_oob_start(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
#if (0 == UPLOAD_OOB_START_FUN_EN)
	return 0;
#else
    u8 st = DISTRIBUT_ST_SUCCESS;
    if(is_rx_upload_start_before() && ((UPLOAD_PHASE_TRANSFER_SUCCESS == fw_distribut_srv_proc.upload_phase)||(UPLOAD_PHASE_TRANSFER_ERROR == fw_distribut_srv_proc.upload_phase))){
		// FD/BV-13-C; 14-C;
    	distribut_srv_proc_init(0);
    }
    
    if(is_rx_upload_start_before()){
    	st = DISTRIBUT_ST_BUSY_WITH_UPLOAD;
    }else{
	    fw_distribut_upload_oob_start_t *p_start = (fw_distribut_upload_oob_start_t *)par;
	    u32 offset_fw_id = OFFSETOF(fw_distribut_upload_oob_start_t, uri) + p_start->uri_len;
	    if(par_len <= (int)offset_fw_id){
	    	st = DISTRIBUT_ST_INTERNAL_ERROR; // parameter error
	    }else{
		    u8 *p_fw_id = par + offset_fw_id;
		    u8 len_fw_id = par_len - offset_fw_id;
		    if(len_fw_id > sizeof(fw_distribut_srv_proc.upload_start.fw_id)){
				st = DISTRIBUT_ST_INTERNAL_ERROR; // parameter error
		    }else if(0 != memcmp(p_start->uri, VALID_OOB_URI_HEAD, sizeof(VALID_OOB_URI_HEAD) - 1)){
				st = DISTRIBUT_ST_URI_NOT_SUPPORT;
		    }else if((0 == memcmp(p_start->uri, INVALID_OOB_URI_SAMPLE1, sizeof(INVALID_OOB_URI_SAMPLE1) - 1))
		           || (0 == memcmp(p_start->uri, INVALID_OOB_URI_SAMPLE2, sizeof(INVALID_OOB_URI_SAMPLE2) - 1))){
				st = DISTRIBUT_ST_URI_MALFORMED;	// TODO
		    }else{
			    fw_distribut_srv_proc.rx_upload_start_flag = 1;
			    fw_distribut_srv_proc.upload_phase = UPLOAD_PHASE_TRANSFER_ACTIVE;
			    fw_distribut_srv_proc.upload_type = UPLOAD_TYPE_OUTOFBAND;
				fw_distribut_srv_proc.len_fw_id_upload_start = len_fw_id;
				memcpy(&fw_distribut_srv_proc.upload_start.fw_id, p_fw_id, len_fw_id);
				
			    fw_distribut_srv_proc.upload_oob_start_flag = 1;
				LOG_MSG_LIB(TL_LOG_NODE_SDK, 0, 0 ,"OOB URI len:%d: %s", p_start->uri_len,p_start->uri);
			}
		}
	}
    
	return mesh_fw_distribut_upload_st_rsp(cb_par, st);
#endif
}

int mesh_cmd_sig_fw_distribut_upload_cancel(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    u8 st = DISTRIBUT_ST_SUCCESS;
    distribut_srv_proc_init(0);
    
	return mesh_fw_distribut_upload_st_rsp(cb_par, st);
}

// --------
int mesh_fw_distribut_fw_st_rsp(mesh_cb_fun_par_t *cb_par, u8 st, u8 entry_cnt, u16 image_idx, u8 *p_fw_id, int len_id)
{
	model_g_light_s_t *p_model = (model_g_light_s_t *)cb_par->model;
	u8 buf[sizeof(fw_distribut_fw_status_t) + 16] = {0}; // suppose max size of fw id is 20.
	fw_distribut_fw_status_t *p_rsp = (fw_distribut_fw_status_t *)buf;
	u32 rsp_len = min(sizeof(buf), OFFSETOF(fw_distribut_fw_status_t, fw_id) + len_id);
	p_rsp->st = st;
	p_rsp->entry_cnt = entry_cnt;
	p_rsp->dist_fw_image_idx = image_idx;
	memcpy(&p_rsp->fw_id, p_fw_id, (u8)(rsp_len - OFFSETOF(fw_distribut_fw_status_t, fw_id)));

	return mesh_tx_cmd_rsp(FW_DISTRIBUT_FW_STATUS, (u8 *)p_rsp, rsp_len, p_model->com.ele_adr, cb_par->adr_src, 0, 0);
}

#if UPLOAD_MULTI_FW_TEST_EN
int is_match_upload_1st_fw_id(fw_id_t *p_fw_id, int len, int del_flag)
{
	if ((upload_multi_fw_id.upload_1st_en || del_flag) 
	&& (0 == memcmp(&upload_multi_fw_id.fw_id_1st, p_fw_id, len))){
	   return 1;
	}

	return 0;
}
#endif


int is_fw_id_exist_in_distributor(fw_id_t *p_fw_id, int len, int del_flag)
{
	if(is_rx_upload_start_before()){
		 if ((len == fw_distribut_srv_proc.len_fw_id_upload_start)
		 && (0 == memcmp(&fw_distribut_srv_proc.upload_start.fw_id, p_fw_id, len))){
		 	return 1;
		 }

		 #if UPLOAD_MULTI_FW_TEST_EN
		 return is_match_upload_1st_fw_id(p_fw_id, len, del_flag);
		 #endif
	 }

	 return 0;
}

u8 get_fw_entry_cnt_store_on_distributor()
{
	u8 cnt = (is_rx_upload_start_before() ? 1 : 0);
	
#if UPLOAD_MULTI_FW_TEST_EN
	if(cnt && upload_multi_fw_id.upload_1st_en){
		cnt++;
	}
#endif

	return cnt;
}

static inline int is_distributor_have_started()
{
	return (fw_distribut_srv_proc.st_distr >= MASTER_OTA_ST_DISTRIBUT_START);
}

int mesh_cmd_sig_fw_distribut_fw_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	u8 st = DISTRIBUT_ST_FW_NOT_FOUND;
	u16 image_idx = FW_IMAGE_IDX_NOT_LISTED;
	fw_distribut_fw_get_t *p_get = (fw_distribut_fw_get_t *)par;
	u8 entry_cnt = get_fw_entry_cnt_store_on_distributor();
	if(is_fw_id_exist_in_distributor(&p_get->fw_id, par_len, 0)){
		image_idx = 0;	// only support one image now
#if UPLOAD_MULTI_FW_TEST_EN
		if(upload_multi_fw_id.upload_1st_en && !is_match_upload_1st_fw_id(&p_get->fw_id, par_len, 0)){
		   image_idx = 1;
		}
#endif
		st = DISTRIBUT_ST_SUCCESS;
	}
	
	return mesh_fw_distribut_fw_st_rsp(cb_par, st, entry_cnt, image_idx, (u8 *)&p_get->fw_id, par_len);
}

int mesh_cmd_sig_fw_distribut_fw_get_by_idx(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    u8 st = DISTRIBUT_ST_FW_NOT_FOUND;
	u16 image_idx = FW_IMAGE_IDX_NOT_LISTED;
    fw_distribut_fw_get_by_idx_t *p_get = (fw_distribut_fw_get_by_idx_t *)par;
	u8 entry_cnt = 0;
	int len_id = 0;
	u8 *p_fw_id = (u8 *)&fw_distribut_srv_proc.upload_start.fw_id;
	
	image_idx = p_get->dist_fw_image_idx; // SR/BV23 20211108
	if(is_rx_upload_start_before()){
		#if UPLOAD_MULTI_FW_TEST_EN
		if(upload_multi_fw_id.upload_1st_en){
			entry_cnt = 2;
			image_idx = p_get->dist_fw_image_idx;
			if(1 == p_get->dist_fw_image_idx){
				len_id = fw_distribut_srv_proc.len_fw_id_upload_start;
				st = DISTRIBUT_ST_SUCCESS;
			}else if(0 == p_get->dist_fw_image_idx){
				len_id = sizeof(upload_multi_fw_id.fw_id_1st);
				st = DISTRIBUT_ST_SUCCESS;
			}
		}else
		#endif
		{
			entry_cnt = 1;
			if(0 == p_get->dist_fw_image_idx){	// only support one image now
				len_id = fw_distribut_srv_proc.len_fw_id_upload_start;
				image_idx = p_get->dist_fw_image_idx;	// only support one image now
				st = DISTRIBUT_ST_SUCCESS;
			}
		}
	}else{
		//st = DISTRIBUT_ST_FW_NOT_FOUND;
		// TODO
	}
	
	return mesh_fw_distribut_fw_st_rsp(cb_par, st, entry_cnt, image_idx, p_fw_id, len_id);
}

void fw_image_del()
{
	fw_distribut_srv_proc.rx_upload_start_flag = fw_distribut_srv_proc.len_fw_id_upload_start = 0;
}

int mesh_cmd_sig_fw_distribut_fw_del(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	u8 st = DISTRIBUT_ST_FW_NOT_FOUND;
	u16 image_idx = FW_IMAGE_IDX_NOT_LISTED;
    fw_distribut_fw_del_t *p_del = (fw_distribut_fw_del_t *)par;
	u8 entry_cnt = get_fw_entry_cnt_store_on_distributor();
	int len_id = par_len;
	
	if(is_fw_id_exist_in_distributor(&p_del->fw_id, par_len, 1)){
		if(is_distributor_have_started()){
			len_id = 0;
			st = DISTRIBUT_ST_BUSY_WITH_DISTRIBUTION;
		}else{
			#if UPLOAD_MULTI_FW_TEST_EN
			if(is_match_upload_1st_fw_id(&p_del->fw_id, par_len, 1)){
				upload_multi_fw_id.upload_1st_en = 0; // memset(&upload_multi_fw_id, 0, sizeof(upload_multi_fw_id));
				image_idx = FW_IMAGE_IDX_NOT_LISTED;
			}else
			#endif
			{
				fw_image_del();
				image_idx = 0;	// only support one image now
			}
			
			entry_cnt = get_fw_entry_cnt_store_on_distributor();
			st = DISTRIBUT_ST_SUCCESS;
		}
	}
	
	return mesh_fw_distribut_fw_st_rsp(cb_par, st, entry_cnt, image_idx, (u8 *)&p_del->fw_id, len_id);
}

int mesh_cmd_sig_fw_distribut_fw_del_all(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	u8 st = DISTRIBUT_ST_SUCCESS;
	u16 image_idx = FW_IMAGE_IDX_NOT_LISTED;
	u8 entry_cnt = get_fw_entry_cnt_store_on_distributor();
	
	if(entry_cnt && is_distributor_have_started()){
		st = DISTRIBUT_ST_BUSY_WITH_DISTRIBUTION;
	}else{
		entry_cnt = 0;
		fw_image_del();
	}
	
	return mesh_fw_distribut_fw_st_rsp(cb_par, st, entry_cnt, image_idx, 0, 0);
}



// --------
void mesh_ota_master_wait_ack_st_set()
{
	if(fw_distribut_srv_proc.st_distr){
		fw_distribut_srv_proc.st_wait_flag = 1;
	}

#if NODE_DISTRIBUT_RETRY_FLOW_EN
	if(0 == fw_distribut_srv_proc.wait_ack_tick){
		fw_distribut_srv_proc.wait_ack_retry_cnt = RELIABLE_RETRY_CNT_DEF + 1;
	}
	fw_distribut_srv_proc.wait_ack_tick = clock_time() | 1;
#endif
}

void mesh_ota_master_wait_ack_st_return(int success)
{
    if(!success){
        fw_distribut_srv_proc.list[fw_distribut_srv_proc.node_num].skip_flag = 1;
    }
    fw_distribut_srv_proc.node_num++;
	clr_mesh_ota_master_wait_ack();
}

void mesh_ota_set_block_get_state()
{
    fw_distribut_srv_proc_t *distr_proc = &fw_distribut_srv_proc;
	distr_proc->node_num = distr_proc->chunk_num = 0;
	memset(distr_proc->miss_mask, 0, sizeof(distr_proc->miss_mask));
	mesh_ota_master_next_st_set(MASTER_OTA_ST_BLOB_BLOCK_GET);
}

int mesh_ota_check_skip_current_node()
{
    fw_distribut_srv_proc_t *distr_proc = &fw_distribut_srv_proc;
    if(distr_proc->list[distr_proc->node_num].skip_flag){
        if(MASTER_OTA_ST_BLOB_BLOCK_START == distr_proc->st_distr){
            LOG_MSG_INFO(TL_LOG_COMMON,0,0,"access_cmd_blob_block_start, XXXXXX Skip addr:0x%04x", distr_proc->list[distr_proc->node_num].adr);
        }
        distr_proc->node_num++;
        return 1;
    }
    return 0;
}

#if VC_CHECK_NEXT_SEGMENT_EN
int mesh_ota_and_only_one_node_check()
{
    fw_distribut_srv_proc_t *distr_proc = &fw_distribut_srv_proc;
    if((MASTER_OTA_ST_BLOB_CHUNK_START == distr_proc->st_distr)
    || (MASTER_OTA_ST_BLOB_BLOCK_GET == distr_proc->st_distr)){	// for the last chunk
        if((1 == distr_proc->node_cnt) && is_unicast_adr(fw_distribut_srv_proc.adr_group)){
            if(distr_proc->list[0].adr == ele_adr_primary){
                return VC_MESH_OTA_ONLY_ONE_NODE_SELF;
            }else if(distr_proc->list[0].adr == APP_get_GATT_connect_addr()){
                return VC_MESH_OTA_ONLY_ONE_NODE_CONNECTED;
            }
        }
    }
    
    #if INITIATOR_CLIENT_EN
    if(INITIATOR_OTA_ST_BLOB_CHUNK_START == get_fw_initiator_proc_st()){
        return VC_MESH_OTA_ONLY_ONE_NODE_CONNECTED;
    }
    #endif
    
    return 0;
}
#endif

void mesh_ota_master_ack_timeout_handle()
{
    if(is_mesh_ota_master_wait_ack()){
        mesh_ota_master_wait_ack_st_return(0);
    }

    #if INITIATOR_CLIENT_EN
    mesh_ota_initiator_ack_timeout_handle();
    #endif
}

void decode_miss_chunk(u8 *in_encode, int encode_len, u8 *out_mask, u32 out_mask_len)
{
    u16 OutUTF8LenCost = 0;
    while(OutUTF8LenCost < encode_len){
        u16 miss_chunk_index = -1;
        u16 len_cost = UTF8StrGetOneU16(in_encode + OutUTF8LenCost, encode_len - OutUTF8LenCost, &miss_chunk_index);
        if(0 == len_cost){
            len_cost = 1;   // invalid utf8 head, skip.
        }else{
            if(miss_chunk_index < out_mask_len*8){
                out_mask[miss_chunk_index/8] |= BIT(miss_chunk_index & 7);
                LOG_MSG_INFO(TL_LOG_COMMON, 0, 0, "    DECODE MISS CHUNK: 0x%04x", miss_chunk_index);
            }
        }
        
        OutUTF8LenCost += len_cost;
    }
}

#if PULL_LPN_REACHABLE_TIMER_EN
void distr_lpn_reachable_timer_tick_start()
{
	//if(MESH_OTA_TRANSFER_MODE_PULL == g_blob_info_status.transfer_mode){
		fw_distribut_srv_proc.tick_lpn_reachable_timer = clock_time() | 1;
		//fw_distribut_srv_proc.lpn_reachable_timer_retry_cnt = 0;
	//}
}

void distr_lpn_reachable_timer_tick_stop()
{
	//if(MESH_OTA_TRANSFER_MODE_PULL == g_blob_info_status.transfer_mode){
		fw_distribut_srv_proc.tick_lpn_reachable_timer = 0;
		fw_distribut_srv_proc.lpn_reachable_timer_retry_cnt = 0;
	//}
}
#endif

int mesh_ota_master_rx (mesh_rc_rsp_t *rsp, u16 op, u32 size_op)
{
    int op_handle_ok = 0;
    u8 *par = rsp->data + size_op;
    u16 par_len = GET_PAR_LEN_FROM_RSP(rsp->len, size_op);
    fw_distribut_srv_proc_t *distr_proc = &fw_distribut_srv_proc;
    int adr_match = (rsp->src == master_ota_current_node_adr);
    int next_st = 0;
    int skip_flag = 0;

    #if INITIATOR_CLIENT_EN
    if(get_fw_initiator_proc_st() && (distr_proc->st_distr <= MASTER_OTA_ST_DISTRIBUT_START || (get_fw_initiator_proc_st() >= INITIATOR_OTA_ST_DISTR_START))){
        return mesh_ota_initiator_rx(rsp, op, size_op);
    }
    #endif
    
    if(FW_UPDATE_INFO_STATUS == op){
        if(adr_match && distr_proc->st_wait_flag && ((MASTER_OTA_ST_FW_UPDATE_INFO_GET == distr_proc->st_distr)
        				  			|| (MASTER_OTA_ST_FW_UPDATE_INFO_GET_AFTER_APPLY == distr_proc->st_distr))){
            //fw_update_info_status_t *p = (fw_update_info_status_t *)par;
            next_st = 1;
        }
        op_handle_ok = 1;
    }else if(FW_DISTRIBUT_STATUS == op){
        fw_distribut_status_t *p = (fw_distribut_status_t *)par;
        if(DISTRIBUT_ST_SUCCESS == p->st){
        }else if(DISTRIBUT_ST_INSUFFICIENT_RESOURCE == p->st){
        }

        if(distr_proc->adr_group){  // distribute start
            if(DISTRIBUT_ST_SUCCESS != p->st){
                LOG_MSG_ERR (TL_LOG_COMMON, 0, 0, "fw distribution status error:%d ", p->st);
            }
        }else{                      // distribute stop
            LOG_MSG_INFO(TL_LOG_CMD_NAME, 0, 0, "mesh OTA completed or get info ok!");
        }
        op_handle_ok = 1;
    }else if(FW_DISTRIBUT_RECEIVERS_LIST == op){
        op_handle_ok = 1;
    }else if(FW_UPDATE_METADATA_CHECK_STATUS == op){
        if(adr_match){
            fw_update_metadata_check_status_t *p = (fw_update_metadata_check_status_t *)par;
            if(distr_proc->st_wait_flag && (MASTER_OTA_ST_UPDATE_METADATA_CHECK == distr_proc->st_distr)){
				LOG_MSG_INFO(TL_LOG_COMMON, 0, 0, "metada check additional info:%d ", p->additional_info);
                if(UPDATE_ST_SUCCESS != p->st){
                    skip_flag = 1;
                    LOG_MSG_ERR (TL_LOG_COMMON, 0, 0, "fw update metada check error:%d ", p->st);
                    if(UPDATE_ST_METADATA_CHECK_FAIL == p->st){
						LOG_MSG_ERR (TL_LOG_COMMON, 0, 0, "PID or VID of new firmware error, please check firmware!");
                    }
                }
                next_st = 1;
            }
        }
        op_handle_ok = 1;
    }else if(FW_UPDATE_STATUS == op){
        if(adr_match && distr_proc->st_wait_flag){
            fw_update_status_t *p = (fw_update_status_t *)par;
            fw_receiver_list_t * p_list = get_fw_node_receiver_list(rsp->src);
            p_list->update_phase = p->update_phase;
            #if DISTRIBUTOR_UPDATE_SERVER_EN
            if(UPDATE_PHASE_VERIFYING_FAIL == p->update_phase){
            	p_list->verify_fail_flag = 1;
            }
            #endif
            int optional_par_flag = (par_len >= sizeof(fw_update_status_t));
            if(optional_par_flag){
            	p_list->additional_info = p->additional_info;
            }
            
            if(MASTER_OTA_ST_UPDATE_START == distr_proc->st_distr){
                next_st = 1;
            }else if(MASTER_OTA_ST_UPDATE_GET == distr_proc->st_distr){
                next_st = 1;
                #if PTS_TEST_OTA_EN // confirm later: FD/BV-25: why distribute get is before set phase in master proc.
                if((distr_proc->node_num + 1 >= distr_proc->node_cnt) && is_all_valid_receiver_applying_ok()){
                	if(DISTRIBUT_PHASE_TRANSFER_ACTIVE == fw_distribut_srv_proc.distribut_update_phase){
						fw_distribut_srv_proc.distribut_update_phase = DISTRIBUT_PHASE_TRANSFER_SUCCESS;
					}
                }
                #endif
            }else if(MASTER_OTA_ST_UPDATE_APPLY == distr_proc->st_distr){
                next_st = 1;
				if(UPDATE_ST_SUCCESS != p->st){
					fw_distribut_srv_proc.list[fw_distribut_srv_proc.node_num].skip_flag = 0;
				}
				APP_report_mesh_ota_apply_status(rsp->src, p);
				if(UPDATE_ST_SUCCESS == p->st && is_apply_phase_success(p->update_phase)){
                    LOG_MSG_INFO(TL_LOG_COMMON,0, 0,"fw update apply success!!!");
				}else{
                    LOG_MSG_ERR (TL_LOG_COMMON, 0, 0, "------------------------------!!! Firmware update apply ERROR !!!");
                }
            }
            
			if(UPDATE_ST_SUCCESS != p->st){
				if(MASTER_OTA_ST_UPDATE_APPLY != distr_proc->st_distr){
					skip_flag = 1;
				}
				LOG_MSG_ERR (TL_LOG_COMMON, 0, 0, "fw update status error:%d ", p->st);
			}
        }
        op_handle_ok = 1;
    }else if(BLOB_TRANSFER_STATUS == op){
        if(adr_match && distr_proc->st_wait_flag){
            blob_transfer_status_t *p = (blob_transfer_status_t *)par;
            if(MASTER_OTA_ST_BLOB_TRANSFER_GET == distr_proc->st_distr){
                next_st = 1;
            }else if(MASTER_OTA_ST_BLOB_TRANSFER_START == distr_proc->st_distr){
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
            int is_pull_mode_start = 0;
            if(MASTER_OTA_ST_BLOB_BLOCK_START == distr_proc->st_distr){
                distr_proc->list[distr_proc->node_num].st_block_start = p->st;
                if(par_len > OFFSETOF(blob_block_status_t, miss_chunk)){
                	is_pull_mode_start = 1;
                }
                next_st = 1;
            }

            if(is_pull_mode_start || (MASTER_OTA_ST_BLOB_BLOCK_GET == distr_proc->st_distr)){
                if(BLOB_TRANS_ST_SUCCESS == p->st){
                    distr_proc->list[distr_proc->node_num].st_block_get = p->format;
                    int miss_chunk_len = par_len - OFFSETOF(blob_block_status_t,miss_chunk);
                    if(miss_chunk_len >= 0 && miss_chunk_len <= sizeof(distr_proc->miss_mask)){
                        // distr_proc->miss_mask[] have been zero before block get and after end of MASTER_OTA_ST_BLOB_CHUNK_START.
                        if(BLOB_BLOCK_FORMAT_NO_CHUNK_MISS == p->format){
                        	distr_proc->list[distr_proc->node_num].no_missing_flag = 1;
                        }else if(BLOB_BLOCK_FORMAT_ALL_CHUNK_MISS == p->format){
                            set_bit_by_cnt(distr_proc->miss_mask, sizeof(distr_proc->miss_mask), distribut_get_fw_chunk_cnt()); // all need send
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
    }else if(BLOB_PARTIAL_BLOCK_REPORT == op){
        if(adr_match && distr_proc->st_wait_flag && (MASTER_OTA_ST_BLOB_PARTIAL_BLOCK_REPORT == distr_proc->st_distr)){
			#if MESH_OTA_TEST_SOME_CASE_EN
        	if(0 == distr_proc->test_missing_partial_report){
        		distr_proc->test_missing_partial_report++;
				LOG_MSG_ERR (TL_LOG_COMMON, 0, 0, "----OTA,missing partial report test: ----");
        		return 1;
        	}
			#endif

			#if PULL_LPN_REACHABLE_TIMER_EN
        	distr_lpn_reachable_timer_tick_stop();
			#endif
        	
        	if(par_len){
            	decode_miss_chunk(par, par_len, distr_proc->miss_mask, sizeof(distr_proc->miss_mask));
            }else{
            	// no missing, means chunk of one block transfer complete.
	        	mesh_ota_set_block_get_state();
				return 1;
            }
            next_st = 1;
        }
        op_handle_ok = 1;
    }else if(BLOB_INFO_STATUS == op){
        if(adr_match && distr_proc->st_wait_flag && (MASTER_OTA_ST_BLOB_INFO_GET == distr_proc->st_distr)){
            blob_info_status_t *p = (blob_info_status_t *)par;
            memcpy(&g_blob_info_status, p, sizeof(g_blob_info_status));
            u16 mesh_chunk_size_max = MESH_CMD_ACCESS_LEN_MAX - (1 + OFFSETOF(blob_chunk_transfer_t,data)); // 1: op
            if(g_blob_info_status.chunk_size_max > mesh_chunk_size_max){
                g_blob_info_status.chunk_size_max = mesh_chunk_size_max;
            }
            next_st = 1;
        }
        op_handle_ok = 1;
    }else if(CFG_MODEL_SUB_STATUS == op){
        if(adr_match && distr_proc->st_wait_flag && (MASTER_OTA_ST_SUBSCRIPTION_SET == distr_proc->st_distr)){
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
    }

    if(next_st){
        mesh_ota_master_wait_ack_st_return(!skip_flag);
    }
    
    return op_handle_ok;
}

#if WIN32
int mesh_ota_cb_tx_cmd_dev_app_key_not_found(material_tx_cmd_t *p_mat)
{
	if(CFG_MODEL_SUB_ADD == p_mat->op){
		#define SIZE_OP_SUB_ST 	(2)
		#if WIN32
		u8 buff[400] = {0};
		#else
		u8 buff[OFFSETOF(mesh_rc_rsp_t, data) + SIZE_OP_SUB_ST + sizeof(mesh_cfg_model_sub_status_t)] = {0};
		#endif
		mesh_rc_rsp_t *p_rsp = (mesh_rc_rsp_t *)buff;
		u32 sub_add_len = p_mat->par_len;
		sub_add_len = min(sub_add_len, sizeof(mesh_cfg_model_sub_set_t));

		p_rsp->len = GET_RSP_LEN_FROM_PAR(sub_add_len + OFFSETOF(mesh_cfg_model_sub_status_t, set), SIZE_OP_SUB_ST);
		p_rsp->src = p_mat->adr_dst;
		p_rsp->dst = p_mat->adr_src;
		u16 op_rsp = CFG_MODEL_SUB_STATUS;
		memcpy(p_rsp->data, &op_rsp, SIZE_OP_SUB_ST);
		u8 *par = p_rsp->data + SIZE_OP_SUB_ST;
		mesh_cfg_model_sub_status_t *p_st = (mesh_cfg_model_sub_status_t *)par;
		p_st->status = ST_CANNOT_GET;
		memcpy(&p_st->set, p_mat->par, sub_add_len);
		
		mesh_ota_master_rx(p_rsp, op_rsp, SIZE_OP_SUB_ST);
	}
	return 0;
}
#endif

#if (PTS_TEST_BLOB_TRANSFER_FLOW_EN && DISTRIBUTOR_START_TLK_EN)
void mesh_ota_blob_transfer_flow_start(u8 *par, int par_len)
{
	mesh_cmd_sig_fw_distribut_start_tlk(par, par_len, 0);
}
#endif

void mesh_ota_tx_fw_data_read(u32 addr, u32 len, u8 *buf_out)
{
    #if (!WIN32)
    flash_read_page(ota_program_offset + addr, len, buf_out);
    #else
    memcpy(buf_out, (u8 *)fw_ota_data_tx + addr, len);
    #endif
}

int get_fw_metadata(fw_metadata_t *metadata_out)
{
    mesh_ota_tx_fw_data_read(2, sizeof(metadata_out->fw_id),(u8 *)&metadata_out->fw_id);
    #if (!(WIN32 || DRAFT_FEAT_VD_MD_EN))
    memset(metadata_out->rsv, sizeof(metadata_out->rsv), 0);    // default zero
    #endif
    
    return 0;
}

void mesh_ota_missing_chunk_handle()
{
    fw_distribut_srv_proc_t *distr_proc = &fw_distribut_srv_proc;
	if(0 == is_buf_zero(distr_proc->miss_mask, sizeof(distr_proc->miss_mask))){
		distr_proc->chunk_num = 0;
		if(MASTER_OTA_ST_BLOB_PARTIAL_BLOCK_REPORT == distr_proc->st_distr){
			LOG_MSG_INFO (TL_LOG_CMD_NAME, 0, 0, "chunk transfer for partial report");
		}else{
			LOG_MSG_INFO (TL_LOG_CMD_NAME, 0, 0, "access_cmd_blob_chunk_transfer retry");
		}
		mesh_ota_master_next_st_set(MASTER_OTA_ST_BLOB_CHUNK_START);
	}else{
		mesh_ota_master_next_block();
	}
}

u16 get_chunk_cmd_dst_addr()
{
	u16 addr_dst = fw_distribut_srv_proc.adr_group;
	int pull_only_one_flag = 0;
	if(MESH_OTA_TRANSFER_MODE_PULL == g_blob_info_status.transfer_mode){
		if((1 == fw_distribut_srv_proc.node_cnt) && (fw_distribut_srv_proc.block_start.chunk_size <= 8)){
			pull_only_one_flag = 1;
		}
	}
	
	if((0 == addr_dst) || pull_only_one_flag){
		return fw_distribut_srv_proc.list[0].adr;
	}
	return addr_dst;
}

int is_all_receivers_invalid_and_set2distribut_cancel()
{
	int err = 0;
	if(is_distributor_all_receivers_invalid()){
		fw_distribut_srv_proc.distribut_update_phase = DISTRIBUT_PHASE_FAILED;
		LOG_MSG_ERR(TL_LOG_CMD_NAME, 0, 0, "all receivers failed!");
		mesh_ota_master_next_st_set(MASTER_OTA_ST_DISTRIBUT_CANCEL);
		err = 1;
	}

	return err;
}

void mesh_ota_master_proc()
{
#if (INITIATOR_CLIENT_EN)
    mesh_ota_initiator_proc();
#endif

    fw_distribut_srv_proc_t *distr_proc = &fw_distribut_srv_proc;
	if(0 == distr_proc->st_distr || !is_mesh_adv_cmd_fifo_empty()){
		return ;
	}
	
	if(is_busy_segment_or_reliable_flow()){
		#if NODE_DISTRIBUT_RETRY_FLOW_EN
		if(fw_distribut_srv_proc.wait_ack_tick){
			fw_distribut_srv_proc.wait_ack_tick = clock_time() | 1;
		}
		#endif
		return ;
	}
	
#if (WIN32 && (PROXY_HCI_SEL == PROXY_HCI_GATT))
    extern unsigned char connect_flag;
    if(!(pair_login_ok || DEBUG_SHOW_VC_SELF_EN) || distr_proc->suspend_flag){
        return ;
    }
#endif

	static u32 tick_ota_master_proc;
	if((distr_proc->st_distr != MASTER_OTA_ST_DISTRIBUT_START)
	&& (distr_proc->st_distr != MASTER_OTA_ST_BLOB_CHUNK_START)
	&& (distr_proc->st_distr != MASTER_OTA_ST_DISTRIBUT_RECEIVER_ADD_OR_UPLOAD_START)
	/*&& (distr_proc->st_distr != MASTER_OTA_ST_MAX)*/){ // for upload mode waiting for apply command from initiator.
	    if(clock_time_exceed(tick_ota_master_proc, 3000*1000)){
    	    tick_ota_master_proc = clock_time();
    	    int log_normal = 1;
    	    if(is_rx_upload_start_before()){
    	        if(distr_proc->st_distr == MASTER_OTA_ST_WAITING_INITIATOR_UPDATE_APPLY){
    	            log_normal = 0;
                    LOG_MSG_INFO(TL_LOG_COMMON,0, 0,"mesh_ota_master_proc wait for distributor apply, state: 0x%02x, wait flag:%d",distr_proc->st_distr,distr_proc->st_wait_flag);
                }else if(distr_proc->st_distr == MASTER_OTA_ST_DISTRIBUT_CANCEL){
    	            log_normal = 0;
                    LOG_MSG_INFO(TL_LOG_COMMON,0, 0,"mesh_ota_master_proc wait for distributor cancel, state: 0x%02x, wait flag:%d",distr_proc->st_distr,distr_proc->st_wait_flag);
                }
    	    }
    	    
    	    if(log_normal){
                LOG_MSG_INFO(TL_LOG_COMMON,0, 0,"mesh_ota_master_proc state: 0x%02x, wait flag:%d",distr_proc->st_distr,distr_proc->st_wait_flag);
            }
        }
	}else{
	    tick_ota_master_proc = clock_time();
	}

    if(distr_proc->st_wait_flag){
		#if PULL_LPN_REACHABLE_TIMER_EN
    	if(MASTER_OTA_ST_BLOB_PARTIAL_BLOCK_REPORT == distr_proc->st_distr){
    		if(fw_distribut_srv_proc.tick_lpn_reachable_timer
    		&& clock_time_exceed(fw_distribut_srv_proc.tick_lpn_reachable_timer, PULL_LPN_REACHABLE_TIMER_TIMEOUT_S*1000*1000)){
    			fw_distribut_srv_proc.tick_lpn_reachable_timer = 0;
    			if(fw_distribut_srv_proc.lpn_reachable_timer_retry_cnt++ >= PULL_LPN_REACHABLE_TIMER_RETRY_MAX){
					mesh_ota_master_wait_ack_st_return(0); // mesh_ota_master_next_st_set(MASTER_OTA_ST_DISTRIBUT_CANCEL);
    			}else{
					// retry to send chunk.
					fw_distribut_srv_proc.miss_mask[0] = 1; // only send the first one is enough.
					fw_distribut_srv_proc.st_distr = MASTER_OTA_ST_BLOB_CHUNK_START;
					clr_mesh_ota_master_wait_ack();
					LOG_MSG_INFO(TL_LOG_COMMON,0, 0,"LPN reachable timer cnt: %d",distr_proc->lpn_reachable_timer_retry_cnt);
				}
    		}else{
    			return ;
    		}
    	}
		#endif
    	
		#if NODE_DISTRIBUT_RETRY_FLOW_EN
		// have been sure before that it is not in segment flow of rx or tx.
		if(fw_distribut_srv_proc.wait_ack_tick && clock_time_exceed(fw_distribut_srv_proc.wait_ack_tick, NODE_DISTRIBUT_RETRY_INTERVAL_MS*1000)){
			fw_distribut_srv_proc.wait_ack_tick = clock_time() | 1;
			if(fw_distribut_srv_proc.wait_ack_retry_cnt){
				LOG_MSG_LIB(TL_LOG_NODE_SDK,0, 0,"*********************** distribute reliable command retry, st:%d,remained cnt:%d",distr_proc->st_distr, distr_proc->wait_ack_retry_cnt);
				fw_distribut_srv_proc.wait_ack_retry_cnt--;
				if(MASTER_OTA_ST_BLOB_PARTIAL_BLOCK_REPORT == distr_proc->st_distr){
					// pull mode: just wait, can not send chunk again, if not, it may cause LPN can not send partial report,
					// because report tick will be refreshed all the time when receive chunk data.
					return ;
				}else{
					distr_proc->st_wait_flag = 0;
				}
			}else{
				mesh_ota_master_wait_ack_st_return(0);
			}
		}else
		#endif
		{
        	return ;
        }
    }
	
	switch(distr_proc->st_distr){
		case MASTER_OTA_ST_DISTRIBUT_RECEIVER_ADD_OR_UPLOAD_START:
		    distr_proc->node_num = 0;
    		//mesh_ota_master_next_st_set();
			break;
			
		case MASTER_OTA_ST_DISTRIBUT_START:
		    // FW_DISTRIBUT_START was send by VC
		    distr_proc->node_num = 0;
		    if(is_rx_upload_start_before()){
    		    mesh_ota_master_next_st_set(MASTER_OTA_ST_UPDATE_START); // PTS flow
		    }
		#if DISTRIBUTOR_UPDATE_CLIENT_EN
		    else if(is_only_get_fw_info_fw_distribut_srv()){ // just for get fw info, not OTA flow.
    		    mesh_ota_master_next_st_set(MASTER_OTA_ST_FW_UPDATE_INFO_GET);
            }else{
    		    mesh_ota_master_next_st_set(MASTER_OTA_ST_UPDATE_METADATA_CHECK);
                APP_RefreshProgressBar(0, 0, 0, 0, 0);
            }
        #endif
			break;

		#if DISTRIBUTOR_UPDATE_CLIENT_EN	// to decrease code size.
		case MASTER_OTA_ST_UPDATE_METADATA_CHECK:   // it should be better that right after distribute and before other ota command 
		    if(distr_proc->node_num < distr_proc->node_cnt){
		        if(mesh_ota_check_skip_current_node()){ break;}

		        fw_metadata_t fw_metadata = {{0}};
		        get_fw_metadata(&fw_metadata);	// should never have received upload start before, so read from firmware
    	        if(0 == access_cmd_fw_update_metadata_check(master_ota_current_node_adr, &fw_metadata)){
    	            mesh_ota_master_wait_ack_st_set();
    	        }
	        }else{
                distr_proc->node_num = 0;
    	        mesh_ota_master_next_st_set(MASTER_OTA_ST_SUBSCRIPTION_SET);
	        }
			break;
			
		case MASTER_OTA_ST_SUBSCRIPTION_SET:
		    #if (!MESH_OTA_DEFAULT_SUB_EN)
		{
			int no_group_flag = is_unicast_adr(fw_distribut_srv_proc.adr_group);
			if(no_group_flag){
				LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "no need to set subscription");
				LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "only one updating node and it is GATT connected, so use unicast address to send chunks");
			}
			
		    if((distr_proc->node_num < distr_proc->node_cnt) && (!is_rx_upload_start_before()) && !no_group_flag){
		        if(mesh_ota_check_skip_current_node()){ break;}
		        	        
    	        int err = cfg_cmd_sub_set(CFG_MODEL_SUB_ADD, master_ota_current_node_adr, master_ota_current_node_adr, 
    	                        fw_distribut_srv_proc.adr_group, SIG_MD_BLOB_TRANSFER_S, !DRAFT_FEAT_VD_MD_EN);
    	        if(0 == err){
    	            mesh_ota_master_wait_ack_st_set();
    	        }else if(is_tx_cmd_err_number_fatal(err)){
    	        	mesh_ota_master_wait_ack_st_return(0);
    	        }else{
    	        	// busy, try again.
    	        }
	        }else
	        #endif
	        {
                distr_proc->node_num = 0;
    	        mesh_ota_master_next_st_set(MASTER_OTA_ST_FW_UPDATE_INFO_GET);
	        }
	    }
			break;
		#endif

		case MASTER_OTA_ST_FW_UPDATE_INFO_GET:
		case MASTER_OTA_ST_FW_UPDATE_INFO_GET_AFTER_APPLY:
		    if(distr_proc->node_num < distr_proc->node_cnt){
		        if(mesh_ota_check_skip_current_node()){ break;}
		        
    	        if(0 == access_cmd_fw_update_info_get(master_ota_current_node_adr)){
    	            mesh_ota_master_wait_ack_st_set();
    	        }
	        }else{
	        	#if DISTRIBUTOR_UPDATE_CLIENT_EN
	            if(is_only_get_fw_info_fw_distribut_srv()){
    	            distribut_srv_proc_init(0);  // stop
	            }else
	            #endif
	            {
                    distr_proc->node_num = 0;
                    #if PTS_TEST_OTA_EN
                    if(MASTER_OTA_ST_FW_UPDATE_INFO_GET_AFTER_APPLY == distr_proc->st_distr){
                    mesh_ota_master_next_st_set(MASTER_OTA_ST_DISTRIBUT_CANCEL); // come from st of MASTER_OTA_ST_UPDATE_APPLY
                    }else
                    #endif
                    {
                    mesh_ota_master_next_st_set(MASTER_OTA_ST_UPDATE_START);
                    }
    	        }
	        }
			break;
			
		case MASTER_OTA_ST_UPDATE_START:
		    if(distr_proc->node_num < distr_proc->node_cnt){
		        if(mesh_ota_check_skip_current_node()){ break;}
		        
		        fw_metadata_t fw_metadata = {{0}};
		        if(is_rx_upload_start_before()){
		        	memcpy(&fw_metadata, &fw_distribut_srv_proc.upload_start.fw_metadata, sizeof(fw_metadata)); // get by get_fw_metadata() for VC, but PTS.
		        }else{
					get_fw_metadata(&fw_metadata);
		        }

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
		        u8 all_mode_mask = (MESH_OTA_TRANSFER_MODE_PULL | MESH_OTA_TRANSFER_MODE_PUSH);
				if((g_blob_info_status.transfer_mode & all_mode_mask) == all_mode_mask) {
					#if WIN32
					if(is_use_ota_push_mode_for_lpn(distr_proc)){
						g_blob_info_status.transfer_mode = MESH_OTA_TRANSFER_MODE_PUSH;
						g_blob_info_status.chunk_size_max = CHUNK_SIZE_PUSH_MODE;
					}else
					#endif
					{
						g_blob_info_status.transfer_mode = MESH_OTA_TRANSFER_MODE_PULL;
					}
				}
				LOG_MSG_LIB (TL_LOG_NODE_SDK, 0, 0, "---transfer start mode:%d", g_blob_info_status.transfer_mode);
		        
    	        u8 bk_size_log = g_blob_info_status.bk_size_log_max;
          		#if PTS_TEST_OTA_EN
          		bk_size_log = g_blob_info_status.bk_size_log_min; // because PTS will report too much missing chunks.
          		#endif
             	if(0 == access_cmd_blob_transfer_start(master_ota_current_node_adr, g_blob_info_status.transfer_mode, new_fw_size, bk_size_log)){
    	            mesh_ota_master_wait_ack_st_set();
    	        }
	        }else{
                distr_proc->node_num = 0;
                if(is_all_receivers_invalid_and_set2distribut_cancel()){
					// have been set to MASTER_OTA_ST_DISTRIBUT_CANCEL;
                }else{
    	        	mesh_ota_master_next_st_set(MASTER_OTA_ST_BLOB_BLOCK_START);
    	        }
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
    		        u32 size = distribut_get_block_size(block_num_current);
					u32 crc =0;
					#if !WIN32
					if(block_num_current == 0){
						u8 crc_buf[16];
						flash_read_page(ota_program_offset,sizeof(crc_buf),crc_buf);
						crc_buf[BOOT_MARK_ADDR not 8]= get_fw_ota_value(); // TODO: can not use 8, but BOOT_MARK_ADDR for B91 and kite
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
                #if PTS_TEST_BLOB_TRANSFER_FLOW_EN
                if(fw_distribut_srv_proc.blob_flow_flag){
                	int rx_ok = 1;
                	#if DEBUG_SHOW_VC_SELF_EN
                	if(is_only_VC_self_OTA()){
	                	fw_ota_data_rx[BOOT_MARK_ADDR] = fw_update_srv_proc.reboot_flag_backup;
	                	rx_ok = (0 == memcmp(fw_ota_data_tx, fw_ota_data_rx, fw_distribut_srv_proc.blob_size));
                	}
                	#endif
					LOG_MSG_LIB (TL_LOG_COMMON, 0, 0, "---- blob transfer flow completed! result ok:%d ----", rx_ok);
					distribut_srv_proc_init(0);  // stop
                }else
                #endif
                {
                	mesh_ota_master_next_st_set(MASTER_OTA_ST_UPDATE_GET);
                }
	        }
			break;
		}
			
		case MASTER_OTA_ST_BLOB_BLOCK_START_CHECK_RESULT:
            if(is_need_block_transfer()){
                distr_proc->chunk_num = 0;
                if(g_blob_info_status.transfer_mode != MESH_OTA_TRANSFER_MODE_PULL){
                	set_bit_by_cnt(distr_proc->miss_mask, sizeof(distr_proc->miss_mask), distribut_get_fw_chunk_cnt()); // all need send
                }
    	        mesh_ota_master_next_st_set(MASTER_OTA_ST_BLOB_CHUNK_START);
	        }else{
	            mesh_ota_master_next_block();
	        }
			break;
			
		case MASTER_OTA_ST_BLOB_CHUNK_START:
		{
			#if FEATURE_FRIEND_EN
			mesh_cmd_nw_t nw_temp;
			nw_temp.src = ele_adr_primary;
			nw_temp.dst = get_chunk_cmd_dst_addr();
			int chunk2lpn = is_unicast_friend_msg_to_lpn(&nw_temp);
			#else
			int chunk2lpn = 0;
			#endif
		    if((g_blob_info_status.transfer_mode == MESH_OTA_TRANSFER_MODE_PULL)
		    && chunk2lpn){	// unsegment // (g_blob_info_status.chunk_size_max <= 8)
			    static u32 tick_chunk_send;
			    if(clock_time_exceed(tick_chunk_send, 180*1000)){ // if too short, it will cause cache replaced when FN is GATT connected or when GW is as FN.
			    	tick_chunk_send = clock_time();
			    }else{
			    	break;
			    }
		    }
			
		    u32 chunk_cnt = distribut_get_fw_chunk_cnt();
		    if(distr_proc->chunk_num < chunk_cnt){
		        if(is_buf_bit_set(distr_proc->miss_mask, distr_proc->chunk_num)){
		            u8 cmd_buf[MESH_CMD_ACCESS_LEN_MAX+16] = {0};
                    blob_chunk_transfer_t *p_cmd = (blob_chunk_transfer_t *)cmd_buf;
                    p_cmd->chunk_num = distr_proc->chunk_num;
                    u16 size = distribut_get_chunk_size(p_cmd->chunk_num);
                    if(size > g_blob_info_status.chunk_size_max){
                        size = g_blob_info_status.chunk_size_max;
                    }

                    u32 fw_pos = 0;
					u16 block_num_current = fw_distribut_srv_proc.block_start.block_num;
					
                    fw_pos = distribut_get_fw_data_position(p_cmd->chunk_num);
                    mesh_ota_tx_fw_data_read(fw_pos, size, p_cmd->data);
                    if(block_num_current == 0){
                    	#if (GATEWAY_ENABLE)
                    	if(1)
                    	#else
						if (is_rx_upload_start_before())
						#endif
						{
							u8 flag_backup = fw_distribut_srv_proc.distribut_reboot_flag_backup;
							#if GATEWAY_ENABLE
							if(!is_rx_upload_start_before()){
								flag_backup = get_fw_ota_value();
							}
							#endif
							
							if(g_blob_info_status.chunk_size_max == 8){
								if(p_cmd->chunk_num == (BOOT_MARK_ADDR/8)){
	    							p_cmd->data[0] = flag_backup;
	    						}
							}else if(g_blob_info_status.chunk_size_max < 8){
								// should not happen
							}else{
								if(p_cmd->chunk_num == 0){
									// have make sure sizeof(p_cmd->data) > 8
									// Just make sure that the boot mark flag store in flash is invalid, regardless of whether it is B85 or B91 firmware.
									// so use BOOT_MARK_ADDR instead of a variable which depend on firmware type.
									if(sizeof(p_cmd->data) > BOOT_MARK_ADDR){ // has assert chunk_size >= 0x24 before when not eual to 8.
										p_cmd->data[BOOT_MARK_ADDR] = flag_backup;
									}else{
										// should not happen here.
									}
								}
							}
						}else{
						}
					}
					
                    u16 bk_total = distribut_get_fw_block_cnt();
                    u8 percent = 1 + (fw_pos+size)*98/distr_proc->blob_size;
                    if(percent > distr_proc->percent_last){
                        distr_proc->percent_last = percent;
                        APP_RefreshProgressBar(block_num_current, bk_total, distr_proc->chunk_num, chunk_cnt, percent);
                    }

                    if(fw_distribut_srv_proc.miss_chunk_test_flag && (7 == distr_proc->chunk_num)){
                        LOG_MSG_ERR (TL_LOG_COMMON, 0, 0, "----OTA,missing chunk test: %2d----", distr_proc->chunk_num);
    		            distr_proc->chunk_num++;
    		            if(MESH_OTA_TRANSFER_MODE_PULL == g_blob_info_status.transfer_mode){
    		            	fw_distribut_srv_proc.miss_chunk_test_flag--;
    		            }else{
							fw_distribut_srv_proc.miss_chunk_test_flag = 0;
    		            }
                    }else if(0 == access_cmd_blob_chunk_transfer(get_chunk_cmd_dst_addr(), (u8 *)p_cmd, size+2)){
    		            distr_proc->chunk_num++;
    		        }
		        }else{
		            distr_proc->chunk_num++;
		        }
	        }

			// find next valid bit.
			for(unsigned int i = distr_proc->chunk_num; i < (chunk_cnt); ++i){
				if(is_buf_bit_set(distr_proc->miss_mask, distr_proc->chunk_num)){
					break;
				}
				distr_proc->chunk_num ++;
			}
			
			if(distr_proc->chunk_num >= chunk_cnt){
	        	mesh_ota_set_block_get_state();
	            if(g_blob_info_status.transfer_mode == MESH_OTA_TRANSFER_MODE_PULL){
					#if PULL_LPN_REACHABLE_TIMER_EN
	            	distr_lpn_reachable_timer_tick_start();
					#endif
	            	mesh_ota_master_next_st_set(MASTER_OTA_ST_BLOB_PARTIAL_BLOCK_REPORT);
					mesh_ota_master_wait_ack_st_set();
	            }
	        }
	    }
			break;
			
		case MASTER_OTA_ST_BLOB_PARTIAL_BLOCK_REPORT:
	        mesh_ota_missing_chunk_handle(); // set next st inside.
			break;
			
		case MASTER_OTA_ST_BLOB_BLOCK_GET:
		    if(distr_proc->node_num < distr_proc->node_cnt){
		        if(mesh_ota_check_skip_current_node()){ break;}
		        
		        if(distr_proc->list[distr_proc->node_num].no_missing_flag){
		        	distr_proc->node_num++;
		        	break;
		        }
		        
    	        if(0 == access_cmd_blob_block_get(master_ota_current_node_adr, distr_proc->block_start.block_num)){
    	            mesh_ota_master_wait_ack_st_set();
    	        }
	        }else{
	            mesh_ota_missing_chunk_handle(); // set next st inside.
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
                if(0 == is_all_valid_receiver_applying_ok()){
                	// retry update get cycle. // just valid for PTS now
                }else{
	                u8 next_st = MASTER_OTA_ST_UPDATE_APPLY;
	                if(is_rx_upload_start_before()){
	                    //if((1 == distr_proc->node_cnt) && distr_proc->list[0].skip_flag){
	                        fw_distribut_srv_proc.percent_last = 100;
	                    //}
	                    if(UPDATE_POLICY_VERIFY_ONLY == distr_proc->distribut_start.par.update_policy){
	                        next_st = MASTER_OTA_ST_WAITING_INITIATOR_UPDATE_APPLY;
	                    }
	                }

	                if(is_all_receivers_invalid_and_set2distribut_cancel()){
						// have been set to MASTER_OTA_ST_DISTRIBUT_CANCEL;
	                }else{
	                	fw_distribut_srv_proc.distribut_update_phase = DISTRIBUT_PHASE_TRANSFER_SUCCESS;
						mesh_ota_master_next_st_set(next_st);
	                }
                }
	        }
			break;
			
		case MASTER_OTA_ST_WAITING_INITIATOR_UPDATE_APPLY:
		    if(distr_proc->distribut_apply_en){
                mesh_ota_master_next_st_set(MASTER_OTA_ST_UPDATE_APPLY);
		    }else{
		        // nothing, just waiting
		    }
			break;
			
		case MASTER_OTA_ST_UPDATE_APPLY:
		    if(distr_proc->node_num < distr_proc->node_cnt){
                fw_distribut_srv_proc.distribut_update_phase = DISTRIBUT_PHASE_APPLYING_UPDATE;
		        //if(mesh_ota_check_skip_current_node()){ break;}
		        fw_receiver_list_t *p_list = &distr_proc->list[distr_proc->node_num];
				#if NODE_DISTRIBUT_RETRY_FLOW_EN
				if(0 == fw_distribut_srv_proc.wait_ack_tick)
				#endif
				{
			        if(p_list->apply_flag
			         || ((p_list->adr == APP_get_GATT_connect_addr()) && (distribut_get_not_apply_cnt() > 1))){
			            distr_proc->node_num++;
			            break ;
			        }
		        }
		        
		        p_list->apply_flag = 1;

                #if DISTRIBUTOR_NO_UPDATA_START_2_SELF
                int apply_distributor_self_flag = (p_list->skip_flag && p_list->adr == ele_adr_primary);
                #endif
		        
		        u16 op;
		        if(((UPDATE_PHASE_VERIFYING_SUCCESS == p_list->update_phase)||(UPDATE_PHASE_APPLYING_UPDATE == p_list->update_phase))
		        #if DISTRIBUTOR_NO_UPDATA_START_2_SELF
		            || apply_distributor_self_flag
		        #endif
		        ){
		            op = FW_UPDATE_APPLY;
		        }else{
		            op = FW_UPDATE_CANCEL;
		        }

    	        if(0 == access_cmd_fw_update_control(master_ota_current_node_adr, op, 1)){
				    #if DISTRIBUTOR_NO_UPDATA_START_2_SELF
		            if(apply_distributor_self_flag){    // due to no ack for distributor in firmware.
		            	mesh_ota_master_next_st_set(distr_proc->st_distr);	// just for clear some wait flag.
		            }else
		            #endif
		            {
    	                mesh_ota_master_wait_ack_st_set();
    	            }
    	        }
	        }else{
                distr_proc->node_num = 0;
                if(distribut_get_not_apply_cnt() == 0){
                    APP_print_connected_addr();
                    #if PTS_TEST_OTA_EN
                    mesh_ota_master_next_st_set(MASTER_OTA_ST_FW_UPDATE_INFO_GET_AFTER_APPLY);
                    #else // no need to check fw info, because node is rebooting, and it will be checked by APP.
                    fw_distribut_srv_proc.distribut_update_phase = DISTRIBUT_PHASE_COMPLETED;
                    mesh_ota_master_next_st_set(MASTER_OTA_ST_DISTRIBUT_CANCEL); // in another flow to check new firmware version after reboot.
                    #endif
                }else{
                    mesh_ota_master_next_st_set(MASTER_OTA_ST_UPDATE_APPLY);
                }
	        }
			break;
			
		case MASTER_OTA_ST_DISTRIBUT_CANCEL:
			#if GATEWAY_ENABLE
			{
				fw_receiver_list_t *p_list = fw_distribut_srv_proc.list;
				u8 mesh_ota_sts[1+2*MESH_OTA_UPDATE_NODE_MAX];
				u16 mesh_ota_idx =0;
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
				u32 st_back = distr_proc->st_distr;
				mesh_ota_master_next_st_set(MASTER_OTA_ST_MAX);	// must set before tx cmd, because gateway use it when rx this command.

                if(is_rx_upload_start_before()){
					if(0 == is_distributor_all_receivers_invalid()){
                    	fw_distribut_srv_proc.distribut_update_phase = DISTRIBUT_PHASE_COMPLETED;
                    }
		            // wait for app sending distribute cancel in "MASTER_OTA_ST_MAX".
		        }else{
    		        if(0 == access_cmd_fw_distribut_cancel(distr_proc->adr_distr_node)){
    		            // no need, ota flow have been stop in mesh_cmd_sig_fw_distribut_cancel(),
    	                APP_RefreshProgressBar(0, 0, 0, 0, 100);
    		        }else{
    		        	mesh_ota_master_next_st_set(st_back);
    		        }
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

#if (DISTRIBUTOR_UPDATE_CLIENT_EN)
int mesh_cmd_sig_fw_distribut_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_fw_distribut_receiver_list(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_fw_distribut_capabilities_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_fw_distribut_receivers_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_fw_distribut_upload_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_fw_distribut_fw_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

#endif

#endif


/***************************************** 
------- for updater node
******************************************/
#if 1
fw_update_srv_proc_t    fw_update_srv_proc = {0};         // for updater
#if PTS_TEST_OTA_EN
u8 A_2_blob_transfer_BV_31_C  = 0;
// TEST_BT_BV07_EN
#endif

#if BLOB_TRANSFER_WITHOUT_FW_UPDATE_EN
const u8 BLOB_ID_LOCAL[8] = {0x11,0x00,0x00,0x00,0x00,0x00,0x00,0x11};
const u8 BLOB_ID_VC_INITIATOR[8] = {0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68};
#endif

STATIC_ASSERT(MESH_OTA_CHUNK_SIZE >= 8); // if not, need to fix mesh_ota_save_data_()
#if (MESH_OTA_CHUNK_SIZE <= 0x24) // BOOT_MARK_ADDR is 0x20 for B91
STATIC_ASSERT((MESH_OTA_CHUNK_SIZE == 8) && (BOOT_MARK_ADDR % 8 == 0)); // BOOT_MARK_ADDR is 8 for B85/B87
#endif

void mesh_ota_save_data(u32 adr, u32 len, u8 * data){
	if(8 == fw_update_srv_proc.block_start.chunk_size){
		if (adr == BOOT_MARK_ADDR){
		    fw_update_srv_proc.reboot_flag_backup = data[0];
			data[0] = 0xff;					//FW flag invalid
		}
	}else{
		if (adr == 0){ // assert chunk_size >= 0x24 before several lines
		    fw_update_srv_proc.reboot_flag_backup = data[BOOT_MARK_ADDR];
			data[BOOT_MARK_ADDR] = 0xff;					//FW flag invalid
		}
	}

#if WIN32
    #if DISTRIBUTOR_UPDATE_SERVER_EN && VC_APP_ENABLE
    memcpy(fw_ota_data_rx + adr, data, len);
    #endif
#else
	flash_write_page(ota_program_offset + adr, len, data); // will readback to check in soft_crc32_ota_flash.
#endif
}

u32 soft_crc32_ota_flash(u32 addr, u32 len, u32 crc_init,u32 *out_crc_type1_blk)
{
#if (MESH_OTA_BLOCK_SIZE_MAX > 16 * 1024)
    clock_switch_to_highest();
#endif

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
            buf[BOOT_MARK_ADDR] = fw_update_srv_proc.reboot_flag_backup;
            fw_update_srv_proc.bin_crc_type = get_ota_check_type();
            if(FW_CHECK_AGTHM2 == fw_update_srv_proc.bin_crc_type){
                fw_update_srv_proc.crc_total = 0xffffffff;  // crc init
                if(out_crc_type1_blk){
                    *out_crc_type1_blk = fw_update_srv_proc.crc_total;
                }
            }
        }
        
        #if BLOCK_CRC32_CHECKSUM_EN
        crc_init = soft_crc32_telink(buf, len_read, crc_init);
        #endif
        
		if(out_crc_type1_blk){
		    if(FW_CHECK_AGTHM2 == fw_update_srv_proc.bin_crc_type){
		        u32 crc_check_len = len_read;
		        if(addr + len_read == fw_update_srv_proc.blob_size){ // the last packet
		            if(len_read % 16 == 0){ // encryption bin mode
		                crc_check_len -= 1;  // the last packet
		            }
		        }
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
        if(FW_CHECK_AGTHM2 != fw_update_srv_proc.bin_crc_type){
		    *out_crc_type1_blk = crc_type1_blk;
		}
	}

#if (MESH_OTA_BLOCK_SIZE_MAX > 16 * 1024)
    clock_switch_to_normal();
#endif

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

	u32 addr_boot_mark_new_fw = BOOT_MARK_ADDR;
    if(fw_update_srv_proc.reboot_flag_backup != 0x4B){ // BOOT_MARK_VALUE
    	#if (BOOT_MARK_ADDR == BOOT_MARK_ADDR_B85M)
    	addr_boot_mark_new_fw = BOOT_MARK_ADDR_B91M;
    	#elif (BOOT_MARK_ADDR == BOOT_MARK_ADDR_B91M)
    	addr_boot_mark_new_fw = BOOT_MARK_ADDR_B85M;
    	#endif
    }
    
    mesh_ota_read_data(addr_boot_mark_new_fw, sizeof(fw_flag), fw_flag);
	if(fw_update_srv_proc.reboot_flag_backup == 0x4B){ // if firmware type different, no need copy reboot flag
    	fw_flag[0] = fw_update_srv_proc.reboot_flag_backup;
	}
	
	if(!memcmp(fw_flag,fw_flag_telink, 4) && is_valid_mesh_ota_len(fw_update_srv_proc.blob_size)){
		return 1;
	}
	return 0;
}

int is_valid_mesh_ota_calibrate_val()
{
    // eclipse crc32 calibrate
    #if (0 == DEBUG_EVB_EN)
    if(!is_valid_telink_fw_flag()){
        return 0;
    }
    #endif

    u8 check_type = get_ota_check_type();
            
    #if ENCODE_OTA_BIN_EN
    if(check_type != FW_CHECK_AGTHM2){
        return 0;   // must check 2.
    }
    #endif
	if((FW_CHECK_AGTHM1 == check_type) || (FW_CHECK_AGTHM2 == check_type)){
	    if(0 == fw_update_srv_proc.bin_crc_done){
    	    u32 len = fw_update_srv_proc.blob_size;
    		int crc_ok = (is_valid_mesh_ota_len(len) 
    		          && (fw_update_srv_proc.crc_total == get_total_crc_type1_new_fw()));  // is_valid_ota_check_type1()
    		          
    		fw_update_srv_proc.bin_crc_done = crc_ok ? BIN_CRC_DONE_OK : BIN_CRC_DONE_FAIL;
    		#if DISTRIBUTOR_UPDATE_SERVER_EN
    		fw_distribut_srv_proc.upload_crc_ok = crc_ok; // because "fw_update_srv_proc.bin_crc_done" will be clear when rx distribute start.
    		#endif
		}
        return (BIN_CRC_DONE_OK == fw_update_srv_proc.bin_crc_done);
	}else{
		fw_update_srv_proc.bin_crc_done = BIN_CRC_DONE_FAIL; // take as error if not crc check // if want to remove CRC, must do readback checking when chunk data have been write.
		return 0;
	}
}


inline u16 updater_get_fw_block_cnt()
{
    return get_fw_block_cnt(fw_update_srv_proc.blob_size, fw_update_srv_proc.bk_size_log);
}

inline u32 updater_get_block_size(u16 block_num)
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
        #if DISTRIBUTOR_UPDATE_SERVER_EN
        if(is_rx_upload_start_before()){
            return is_blob_id_match(fw_distribut_srv_proc.upload_start.upload_blob_id, blob_id);
        }else
        #endif
        {
            return is_blob_id_match(BLOB_ID_LOCAL, blob_id);
        }
    }
#endif
    return is_updater_blob_id_match(blob_id);
}

void fw_update_srv_proc_init_keep_start_par_and_crc()
{
    fw_update_start_t start_backup;
    memcpy(&start_backup, &fw_update_srv_proc.start, sizeof(start_backup));
    u8 crc_backup = fw_update_srv_proc.bin_crc_done;
    u8 add_info_backup = fw_update_srv_proc.additional_info;
    memset(&fw_update_srv_proc, 0, sizeof(fw_update_srv_proc));
    memcpy(&fw_update_srv_proc.start, &start_backup, sizeof(fw_update_srv_proc.start)); // don't clear to handle retransmit here 
    if(BIN_CRC_DONE_OK == crc_backup){				  // sometimes need to calculate crc twice
        fw_update_srv_proc.bin_crc_done = crc_backup; // sometimes apply twice
    }
	fw_update_srv_proc.additional_info = add_info_backup;
}

void set_blob_trans_phase_suspend()
{
	fw_update_srv_proc.blob_trans_phase_backup = fw_update_srv_proc.blob_trans_phase;
	fw_update_srv_proc.blob_trans_phase = BLOB_TRANS_PHASE_SUSPEND;
}

void recover_blob_trans_phase_from_suspend()
{
	if(BLOB_TRANS_PHASE_SUSPEND == fw_update_srv_proc.blob_trans_phase){
		fw_update_srv_proc.blob_trans_phase = fw_update_srv_proc.blob_trans_phase_backup;
	}
}

void blob_block_erase(u16 block_num)
{
    // attention: block size may not integral multiple of 4K,
}

#if MESH_OTA_PULL_MODE_EN
void pull_chunk_tick_refresh()
{
	fw_update_srv_proc.pull_chunk_tick = clock_time() | 1;
}

void pull_mode_report_timer_refresh(bool4 retry_cnt_init)
{
	fw_update_srv_proc.report_timer_tick = clock_time() | 1;
	if(retry_cnt_init){
		fw_update_srv_proc.report_retry_cnt = PULL_MODE_REPORT_RETRY_MAX;
	}
}

void pull_mode_report_timer_stop()
{
	fw_update_srv_proc.report_timer_tick = 0;
}

void mesh_ota_set_partial_report(u16 adr_dst, bool4 tx_now_flag)
{
	pull_mode_report_timer_refresh(1);
	if(tx_now_flag){
		fw_update_srv_proc.report_timer_tick = clock_time() - ((PULL_MODE_REPORT_TIMER_TIMEOUT_MS - 200) * CLOCK_SYS_CLOCK_1MS);	// should not report at once, but add some delay.
	}
	fw_update_srv_proc.report_partital_dst_adr = adr_dst;
}

int mesh_ota_send_partial_report(u16 adr_dst, u16 *out_last_chunk_utf8)
{
	u16 chunk_cnt_total = updater_get_fw_chunk_cnt();
	u8 par_utf8[min(64, PULL_MODE_MAX_MISSING_CHUNK * UTF8_SIZE_MAX_U16)] = {0}; // 
	/* +2:because sending chunk to FN cache and sending cache to LPN can be at the same time,
	   so add redundant messeage can reduce number of partial report, 
	   but making no sense while GATT connected node is distributor.
	*/
	#if WIN32
	u16 cnt_max = (1 << FN_CACHE_SIZE_LOG) + 2;
	#else
	u16 cnt_max = mesh_lpn_par.offer.CacheSize + 2;
	#endif
	//if(cnt_max >= PULL_MODE_MAX_MISSING_CHUNK)
	{
		cnt_max = PULL_MODE_MAX_MISSING_CHUNK;	// make sure unsegment
	}
	
	u32 len_miss_utf8 = get_missing_chunk_utf8(par_utf8, sizeof(par_utf8), cnt_max, chunk_cnt_total, out_last_chunk_utf8);
	int err = SendOpParaDebug(adr_dst, 0, BLOB_PARTIAL_BLOCK_REPORT, par_utf8, len_miss_utf8);

	return err;
}
#else
#define pull_chunk_tick_refresh()		//
#endif

void mesh_ota_proc()
{
#if PTS_TEST_OTA_EN
    #if MESH_OTA_BLOB_START_TIMEOUT_MS
    if(fw_update_srv_proc.blob_trans_start_tick
    && clock_time_exceed(fw_update_srv_proc.blob_trans_start_tick, MESH_OTA_BLOB_START_TIMEOUT_MS * 1000)){
        fw_update_srv_proc.blob_trans_start_tick = 0;
        if(BLOB_TRANS_PHASE_WAIT_NEXT_BLOCK == fw_update_srv_proc.blob_trans_phase){
            set_blob_trans_phase_suspend();
            LOG_MSG_ERR(TL_LOG_COMMON,0, 0 ,"blob transfer timeout--");
        }
    }
    #endif
	
	#if (DISTRIBUTOR_UPDATE_CLIENT_EN || DISTRIBUTOR_UPDATE_SERVER_EN)
	/*for PTS, it must have a state of canceling which should last about 1 seconeds. especially for FD/BV-37/41*/
	#define PTS_CANCELING_TIME_COST_MS	(3000) // 5000 // FD/BV-34-C need 2.5s between cancel and start again.
	if(fw_distribut_srv_proc.tick_dist_canceling && clock_time_exceed(fw_distribut_srv_proc.tick_dist_canceling, PTS_CANCELING_TIME_COST_MS*1000)){
		#if PTS_TEST_OTA_EN
		if(fw_distribut_srv_proc.update_cancel_addr){
			// BV38: PTS need to receive update cancel after distribute status. but distribute status is segment packet,need to make sure distribute status send completely before sending update cancel.
			access_cmd_fw_update_control(fw_distribut_srv_proc.update_cancel_addr, FW_UPDATE_CANCEL, 0);
		}
		#endif
		distribut_srv_proc_init(0); // include clear fw_distribut_srv_proc.tick_dist_canceling_
		LOG_MSG_LIB (TL_LOG_NODE_BASIC, 0, 0, "dist phase canceling is timeout and to be completed");
	}
	#endif
#else
    // no blob transfer timeout now
#endif

#if MESH_OTA_PULL_MODE_EN
	if(fw_update_srv_proc.pull_chunk_tick && clock_time_exceed(fw_update_srv_proc.pull_chunk_tick, PULL_CHUNK_TICK_TIMEOUT_MS * 1000)){
		fw_update_srv_proc.pull_chunk_tick = 0; // stop
	}
	
	if(fw_update_srv_proc.report_timer_tick){
		if(is_busy_segment_or_reliable_flow()){
			pull_mode_report_timer_refresh(0);	// because chunk with 256byte need about 5second.
		}else{
			if(clock_time_exceed(fw_update_srv_proc.report_timer_tick, PULL_MODE_REPORT_TIMER_TIMEOUT_MS * 1000)){
				if(fw_update_srv_proc.report_partital_dst_adr){
					if((BLOB_TRANS_PHASE_WAIT_NEXT_CHUNK == fw_update_srv_proc.blob_trans_phase)
					#if PTS_TEST_OTA_EN // if suspend in complete phase, it will report suspend update phase for fw update get command, then cause distributor send fw apply failed when all firmware data has been sent.
					|| (BLOB_TRANS_PHASE_COMPLETE == fw_update_srv_proc.blob_trans_phase) // for BV10: Block Complete Timeout
					#endif
					){
						if(fw_update_srv_proc.report_retry_cnt <= (PULL_MODE_REPORT_RETRY_MAX - 1)){
							set_blob_trans_phase_suspend();
							//LOG_MSG_ERR(TL_LOG_COMMON,0, 0 ,"blob block pull mode timeout--");
						}
					}
					
					#if 0 // PTS_OTA_TODO_EN // TEST_BT_BV07_EN	// BT/BV-07 cannot send within 31s, but BV-08 need to send, why.
					if(clock_time_exceed(fw_update_srv_proc.report_timer_tick, 35000 * 1000))
					#endif
					{
						if(fw_update_srv_proc.report_retry_cnt){
							u16 last_chunk_utf8 = 0;
							int err = -1;
							#if FEATURE_LOWPOWER_EN
							if(is_friend_ship_link_ok_lpn()||(blc_ll_getCurrentState() == BLS_LINK_STATE_CONN))
							#endif
							{
								err = mesh_ota_send_partial_report(fw_update_srv_proc.report_partital_dst_adr, &last_chunk_utf8);
							}
							
							if(0 == err){
								fw_update_srv_proc.report_retry_cnt--;
								fw_update_srv_proc.report_chunk_idx_last = last_chunk_utf8;
								pull_mode_report_timer_refresh(0);
								LOG_MSG_LIB(TL_LOG_NODE_BASIC,0, 0 ,"ota partial report retry:%d",fw_update_srv_proc.report_retry_cnt);
							}
						}else{
							pull_mode_report_timer_stop();
							// stop blob transfer flow ?
						}
					}
				}
			}
		}
	}
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
u8 get_mesh_ota_additional_info(fw_metadata_t *p_meta, int len)
{
    u8 add_info = ADDITIONAL_CPS_NO_CHANGE;
    #if PTS_TEST_OTA_EN
    // cps no change
    #else
    if(is_ota_metadata_include_fw_id(p_meta, len))
    {
		if(p_meta->fw_id.pid != fw_id_local.pid){
			add_info = ADDITIONAL_NODE_UNPROVISIONED;
		}else{
			if(p_meta->fw_id.vid != fw_id_local.vid){
				add_info = ADDITIONAL_CPS_CHANGE_NO_REMOTE_PROVISIOIN;
			}
		}
	}
	#endif

	return add_info;
}

int mesh_cmd_sig_fw_update_metadata_check(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    fw_update_metadata_check_t *p_check = (fw_update_metadata_check_t *)par;
    u8 st;
    u8 add_info = ADDITIONAL_CPS_NO_CHANGE;
    if(0 == p_check->image_index){ // only one now
        u8 metadata_len = par_len - OFFSETOF(fw_update_metadata_check_t,metadata);
        if(is_valid_metadata_len(metadata_len)
        && mesh_ota_slave_need_ota(&p_check->metadata, metadata_len)){
            memcpy(&fw_update_srv_proc.start.metadata, &p_check->metadata, metadata_len);
            fw_update_srv_proc.metadata_len = metadata_len;
            // may check meta data again when update start.
            #if GATT_LPN_EN
            bls_l2cap_requestConnParamUpdate_Normal();
            #endif
            add_info = get_mesh_ota_additional_info(&p_check->metadata, metadata_len);
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
	rsp.additional_info = add_info;
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
    
	u32 rsp_len = OFFSETOF(fw_update_status_t,ttl);   // no blob_id
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
	    #if DISTRIBUTOR_NO_UPDATA_START_2_SELF
	    if(is_distributor_and_in_apply_list(cb_par->adr_dst, 1)){
	        rsp.update_phase = get_update_phase_for_distributor();
	    }
	    #endif
	}else{
	    rsp_len = sizeof(fw_update_status_t);
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

int is_ota_metadata_include_fw_id(fw_metadata_t *p_metadata, int len)
{
	if(len >= sizeof(fw_id_t)){
		return 1;
	}
	return 0;
}

int mesh_ota_slave_need_ota(fw_metadata_t *p_metadata, int len)
{
    #if (PTS_TEST_OTA_EN && !WIN32)
    int need_flag = 0;
    if(sizeof(BLOB_ID_LOCAL) == len){ // meta data in PTS is set to be same with BLOB ID now.
        need_flag = (0 == memcmp(BLOB_ID_LOCAL, p_metadata, sizeof(BLOB_ID_LOCAL)));
    }

    if(0 == need_flag){
    	// TODO for VC
	}
    
    return need_flag;
    #else
    if(is_ota_metadata_include_fw_id(p_metadata, len)){
        return ota_is_valid_pid_vid(&p_metadata->fw_id, 0);
    }else{
        return 1;   // always valid now // return 0;
    }
    #endif
}

#define FW_UPDATE_START_NOT_COMPARE_TTL_EN			0	// 0 means to compare all parameters.

int mesh_cmd_sig_fw_update_start(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	u8 st;
    fw_update_start_t *p_start = (fw_update_start_t *)par;
    u8 *par_err = 0;
    u8 metadata_len = par_len - OFFSETOF(fw_update_start_t,metadata);
    int force_flag = (UPDATE_PHASE_VERIFYING_FAIL == fw_update_srv_proc.update_phase);

#if MESH_FLASH_PROTECTION_EN
	mesh_flash_unlock();
#endif
    
#if (DISTRIBUTOR_UPDATE_SERVER_EN && DISTRIBUTOR_NO_UPDATA_START_2_SELF && (0 == DEBUG_SHOW_VC_SELF_EN))
	if(fw_distribut_srv_proc.st_distr != 0)
	{
		st = UPDATE_ST_TEMP_UNAVAILABLE;
		return mesh_fw_update_st_rsp(cb_par, st, par_err, par_len);
	}
#endif

    if((!force_flag) && ((fw_update_srv_proc.busy
          && (!fw_update_srv_proc.blob_trans_rx_start_error)) // MMDL/SR/FU/BV-17-C Receive Firmware Update Start-Transfer Error Phase
     ||(UPDATE_PHASE_VERIFYING_SUCCESS == fw_update_srv_proc.update_phase)
     ||(UPDATE_PHASE_APPLYING_UPDATE == fw_update_srv_proc.update_phase))){
        if((metadata_len != fw_update_srv_proc.metadata_len)
    #if FW_UPDATE_START_NOT_COMPARE_TTL_EN
        || (0 != memcmp(&fw_update_srv_proc.start.timeout_base, &p_start->timeout_base, par_len - OFFSETOF(fw_update_start_t, timeout_base)))
    #else
        || (0 != memcmp(&fw_update_srv_proc.start, par, par_len))
    #endif
        ){
        	st = UPDATE_ST_WRONG_PHASE;
        	par_err = par;
        }else{
            fw_update_srv_proc.update_phase = UPDATE_PHASE_TRANSFER_ACTIVE;
            #if FW_UPDATE_START_NOT_COMPARE_TTL_EN
            if(fw_update_srv_proc.start.ttl != p_start->ttl){
            	fw_update_srv_proc.start.ttl = p_start->ttl;	// because not compare ttl.
            }
            #endif
            recover_blob_trans_phase_from_suspend();
        	st = UPDATE_ST_SUCCESS;
        }
    }else{
        fw_update_srv_proc.blob_trans_rx_start_error = 0; // init
        /* must check again but not just compare meta data with the data received in meta data check.
            because master may skip the meta data check to pass the check*/
        if((!force_flag) && fw_update_srv_proc.blob_trans_busy){
    	    st = UPDATE_ST_BLOB_TRANSFER_BUSY;
        }else if(is_valid_metadata_len(metadata_len) // meta data over flow
	    && mesh_ota_slave_need_ota(&p_start->metadata, metadata_len)){
            #if (DUAL_MODE_ADAPT_EN || DUAL_MODE_WITH_TLK_MESH_EN)
            dual_mode_disable();
            // bls_ota_clearNewFwDataArea(0); // may disconnect
            #endif
            memset(&fw_update_srv_proc, 0, sizeof(fw_update_srv_proc));
            memcpy(&fw_update_srv_proc.start, p_start, sizeof(fw_update_start_t)); // par_len
            fw_update_srv_proc.metadata_len = metadata_len; // take it as real meta data, but not that in metadata check
            fw_update_srv_proc.additional_info = get_mesh_ota_additional_info(&p_start->metadata, metadata_len);
            fw_update_srv_proc.update_phase = UPDATE_PHASE_TRANSFER_ACTIVE;
            fw_update_srv_proc.blob_trans_phase = BLOB_TRANS_PHASE_WAIT_START;
            pull_chunk_tick_refresh();
            #if 0 // DISTRIBUTOR_UPDATE_SERVER_EN
            fw_distribut_srv_proc.rx_upload_start_flag = 0;	// clear is_rx_upload_start_before()
            #endif
            fw_update_srv_proc.busy = 1;
            st = UPDATE_ST_SUCCESS;
			#if (MESH_DLE_MODE && !WIN32)
				#if BLE_MULTIPLE_CONNECTION_ENABLE
			u16 maxRxOct = aclConn_param.maxRxOct;
			u16 maxTxOct = aclConn_param.maxTxOct_slave;
				#else
			u16 maxRxOct = bltData.connRemoteMaxRxOctets;
			u16 maxTxOct = bltData.connRemoteMaxTxOctets;
				#endif
            LOG_MSG_LIB(TL_LOG_NODE_SDK, 0, 0 , "DLE RemoteMaxRx: %d, DLE RemoteMaxTx: %d", maxRxOct, maxTxOct);
			#endif
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
				#if WIN32
				    #if (DISTRIBUTOR_UPDATE_SERVER_EN && VC_APP_ENABLE)
                fw_ota_data_rx[BOOT_MARK_ADDR] = fw_update_srv_proc.reboot_flag_backup;
                new_fw_write_file(fw_ota_data_rx, fw_update_srv_proc.blob_size);
                    #endif
				#else

				#if (DISTRIBUTOR_UPDATE_SERVER_EN && (!DISTRIBUTOR_NO_UPDATA_START_2_SELF))
                if(is_distributor_in_apply_list_and_need_ota(cb_par->adr_dst, 0)){
                    set_mesh_ota_distribute_100_flag();
                }
                #endif
				// blob_size will have been cleared if "update apply repeat" happens.
                mesh_ota_reboot_set(((fw_update_srv_proc.blob_size > 256) || (OTA_SUCCESS == ota_reboot_type)) ? OTA_SUCCESS : OTA_SUCCESS_DEBUG);
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
        
        #if (DISTRIBUTOR_NO_UPDATA_START_2_SELF)
        if(((MASTER_OTA_ST_UPDATE_APPLY == fw_distribut_srv_proc.st_distr) || (MASTER_OTA_ST_WAITING_INITIATOR_UPDATE_APPLY == fw_distribut_srv_proc.st_distr))){
            if(is_distributor_in_apply_list_and_need_ota(cb_par->adr_dst, 1)){
                set_mesh_ota_distribute_100_flag();
                fw_distribut_srv_proc.distribut_apply_self_ok = 1;
                fw_update_srv_proc.additional_info = get_mesh_ota_additional_info(&fw_distribut_srv_proc.upload_start.fw_metadata, fw_distribut_srv_proc.upload_start.fw_metadata_len);
                mesh_ota_reboot_set(OTA_SUCCESS);
                st = UPDATE_ST_SUCCESS;
            }
        }
        #endif
	}

	#if PTS_TEST_OTA_EN
	LOG_MSG_LIB(TL_LOG_NODE_BASIC,0, 0,"fw update apply is OK:%d",(UPDATE_ST_SUCCESS == st));
	#endif
	
    mesh_ota_reboot_check_refresh();
	return mesh_fw_update_st_rsp(cb_par, st, 0, 0);
}

int mesh_cmd_sig_fw_update_cancel(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	u8 st = UPDATE_ST_INTERNAL_ERROR;
    if(is_fw_update_start_before()
    	#if (0 == PTS_TEST_OTA_EN)
    || fw_update_srv_proc.blob_trans_busy
    		#if !WIN32
    || bls_ota_clearNewFwDataArea(1)
    		#endif
    	#endif
		#if (DISTRIBUTOR_UPDATE_SERVER_EN)
    || is_rx_upload_start_before()
    	#endif
    ){
    	u32 reboot_type = 0;	// 0 means no need to refresh reboot parameters here.
    	#if !WIN32
    	if(0 == ota_reboot_later_tick){
    		if(is_fw_update_start_before()
    		#if (DISTRIBUTOR_NO_UPDATA_START_2_SELF)
    		|| is_distributor_in_apply_list_and_need_ota(cb_par->adr_dst, 1)
    		#endif
    		){
				reboot_type = OTA_DATA_CRC_ERR;
    		}
    		#if (DISTRIBUTOR_UPDATE_SERVER_EN)
    		else{// if(is_rx_upload_start_before()){
				reboot_type = OTA_REBOOT_NO_LED;
			}
			#endif
		}
		#endif
        fw_update_srv_proc_init_keep_start_par_and_crc();
        // fw_update_srv_proc.busy = 0; // have been clear in init.
        #if (PTS_TEST_OTA_EN) // PTS send cancel at the beginning of every case, so no need reboot in some case.
        if(fw_update_srv_proc.blob_trans_busy || fw_update_srv_proc.bin_crc_done)
        #endif
        {
			if(reboot_type){	// because reboot_type should never be success here.
            	mesh_ota_reboot_set(reboot_type); // to clear temporary fw data in flash.
            }
			//LOG_MSG_LIB(TL_LOG_NODE_BASIC, 0, 0,"fw update cancel,reboot tyle:%d", reboot_type);
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
u32 set_bit_buf(u8 *buf, u32 buf_len, u32 start_bit_index, u32 bit_cnt_max)
{
    // buf have been set 0 before.
    u32 i = start_bit_index / CHAR_BIT;
    if(start_bit_index < bit_cnt_max){
        bit_cnt_max = min(bit_cnt_max, buf_len * 8);
        u32 bit_start_1st_byte = (start_bit_index % CHAR_BIT);
        u32 ns = CHAR_BIT - bit_start_1st_byte; // (addr & 0xff);
        while(start_bit_index < bit_cnt_max){
            u32 len = bit_cnt_max - start_bit_index;
            u32 nw = len > ns ? ns : len;
            if(bit_start_1st_byte){
                buf[i] = BIT_RNG(bit_start_1st_byte, bit_start_1st_byte + nw - 1);
                bit_start_1st_byte = 0;
            }else{
                buf[i] = BIT_MASK_LEN(nw);
            }
            ns = CHAR_BIT;
            start_bit_index += nw;
            i++;
        }
        return i;
    }else{
        return min(CEIL_8(bit_cnt_max),buf_len);
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
	
	#if 0 // MBT/SR/BT/BV-05-C, MBT/SR/BT/BV-20-C.//It will be suspended while testing, so it needs to be restored
	recover_blob_trans_phase_from_suspend();
	#endif
	
	rsp.transfer_phase = fw_update_srv_proc.blob_trans_phase;

    u32 rsp_len = OFFSETOF(blob_transfer_status_t, blob_id);
    /*MMDL/SR/BT/BV-04-C(BLOB Transfer Timeout), BV-07-C(BLOB Transfer Cancel-IUT in Waiting for Next Block Phase) : no blob id for cancel command*/
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
                    	#if 0 // MBT_BV_22_TEST_EN
                    if(1)
                    	#else
                    if(0 == fw_update_srv_proc.blob_info_get_flag)
                    	#endif
                    {
                        // MMDL/SR/BT/BV-22-C: IUT in Inactive Phase
                        //fw_update_srv_proc.blob_trans_phase = BLOB_TRANS_PHASE_INACTIVE;
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
                            st = BLOB_TRANS_ST_UNSUPPORTED_TRANS_MODE;
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
            			pull_chunk_tick_refresh();            
						#if DISTRIBUTOR_UPDATE_SERVER_EN
						//fw_distribut_srv_proc.upload_phase = UPLOAD_PHASE_TRANSFER_ERROR;	// why ? // TBD: SR/FD/BV-07-C need error, but BV05 needs active
						#endif
                        
						#if (ZBIT_FLASH_WRITE_TIME_LONG_WORKAROUND_EN)
						check_and_set_1p95v_to_zbit_flash();
						#endif
                        #if APP_FLASH_PROTECTION_ENABLE
						app_flash_protection_ota_begin();
						#endif
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
/*
 * mesh_tx_cmd_blob_block st() is only for blob block start now.
 */
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

static inline u16 get_max_chunk_size_check()
{
#if MESH_OTA_BOTH_PUSH_PULL_EN
	if(MESH_OTA_TRANSFER_MODE_PUSH == fw_update_srv_proc.transfer_mode){
		if(MESH_OTA_CHUNK_SIZE_MAX < CHUNK_SIZE_PUSH_MODE){
			return CHUNK_SIZE_PUSH_MODE;
		}
	}
#endif

	return MESH_OTA_CHUNK_SIZE_MAX;
}

u8 blob_block_start_par_check(blob_block_start_t *p_start)
{
	u8 st;// = BLOB_TRANS_ST_INTERNAL_ERROR;
    if(BLOB_TRANSFER_WITHOUT_FW_UPDATE_EN || fw_update_srv_proc.busy){
        if((BLOB_TRANSFER_WITHOUT_FW_UPDATE_EN || (UPDATE_PHASE_TRANSFER_ACTIVE == fw_update_srv_proc.update_phase))
        && fw_update_srv_proc.blob_trans_busy){
            u32 bk_size_max = (1 << fw_update_srv_proc.bk_size_log);
            int block_num_valid = p_start->block_num <= updater_get_block_rx_ok_cnt();
            if(!block_num_valid && ((fw_update_srv_proc.blob_trans_phase == BLOB_TRANS_PHASE_WAIT_NEXT_CHUNK) // MMDL/SR/BT/BV-21-C, start twice.
                                    || (fw_update_srv_proc.blob_trans_phase == BLOB_TRANS_PHASE_COMPLETE))){
                st = BLOB_TRANS_ST_WRONG_PHASE;
            }else if((p_start->chunk_size > get_max_chunk_size_check())
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
    if((BLOB_TRANS_ST_SUCCESS == st)
    #if 1 // when MBT_BV_26_TEST_EN, need to set to #if 0. BV26(Block Start,IUT in Complete Phase, Pull Mode) need to report chunk missing even though received all before when PTS resend block start again.
    && (p_start->block_num == updater_get_block_rx_ok_cnt()) // but why BV25(push mode) need to report no missing when received all ?
    #endif
    ){
        memcpy(&fw_update_srv_proc.block_start, p_start, sizeof(fw_update_srv_proc.block_start));
        blob_block_erase(p_start->block_num);
        #if 0 // PTS_TEST_OTA_EN// only test for BLOB Transfer Server model, can't enable in FW update serve.
        if(0 == A_2_blob_transfer_BV_31_C){ // MMDL/SR/BT/BV-31-C Chunk Transfer-Invalid Parameters must disable.
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

	#if MESH_OTA_PULL_MODE_EN
	if(MESH_OTA_TRANSFER_MODE_PULL == fw_update_srv_proc.transfer_mode){
		return mesh_cmd_sig_blob_block_get(par, par_len, cb_par);
	}
	#endif
	return mesh_blob_block_st_rsp(cb_par, st, p_start->block_num);
}

int mesh_cmd_sig_blob_partial_block_report(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

//------
int is_blob_chunk_transfer_ready()
{
    return ((BLOB_TRANSFER_WITHOUT_FW_UPDATE_EN || (fw_update_srv_proc.busy && (UPDATE_PHASE_TRANSFER_ACTIVE == fw_update_srv_proc.update_phase)))
            && fw_update_srv_proc.blob_trans_busy && ((BLOB_TRANS_PHASE_WAIT_NEXT_CHUNK == fw_update_srv_proc.blob_trans_phase)||(BLOB_TRANS_PHASE_SUSPEND == fw_update_srv_proc.blob_trans_phase)));
}

int is_blob_block_get_ready()
{
    return (fw_update_srv_proc.blob_trans_busy && (fw_update_srv_proc.blob_trans_phase >= BLOB_TRANS_PHASE_WAIT_NEXT_BLOCK));
}

void set_phase_block_all_chunk_rx_ok(int block_get_flag, int crc_ok)
{
	if(crc_ok){
		if(!fw_update_srv_proc.blob_block_rx_ok){
			block_crc32_check_current(0);
		}
		fw_update_srv_proc.blob_block_rx_ok = 1;
		fw_update_srv_proc.blob_trans_phase = BLOB_TRANS_PHASE_WAIT_NEXT_BLOCK;  // receive ok
	}
	
	if(updater_get_block_rx_ok_cnt() == updater_get_fw_block_cnt()){// all block rx ok
		fw_update_srv_proc.blob_trans_phase = BLOB_TRANS_PHASE_COMPLETE;
		#if DISTRIBUTOR_UPDATE_SERVER_EN
		if(block_get_flag){
			fw_distribut_srv_proc.upload_progress = 100;
			fw_distribut_srv_proc.upload_phase = UPLOAD_PHASE_TRANSFER_SUCCESS;
		}
		#endif
	}
	
	if(BLOB_TRANS_PHASE_WAIT_NEXT_BLOCK == fw_update_srv_proc.blob_trans_phase){
		fw_update_srv_proc.blob_trans_start_tick = clock_time()|1;	// start timer for blob block start check. 
	}
}

int is_valid_chunk_transfer_size(u16 chunk_num, u32 len)
{
	u32 block_size = fw_update_srv_proc.bk_size_current;
	u16 chunk_size_start = fw_update_srv_proc.block_start.chunk_size;
	u16 chunk_max = CEIL_DIV(block_size, chunk_size_start);
	u16 chunk_size_last = block_size % chunk_size_start;
	if(0 == chunk_size_last){
		chunk_size_last = chunk_size_start;
	}
	
	if(chunk_num >= chunk_max){
		return 0;
	}else if(chunk_num + 1 == chunk_max){ // the last chunk
		return (chunk_size_last == len);
	}else{
		return (chunk_size_start == len);
	}
}

int mesh_cmd_sig_blob_chunk_transfer(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    blob_chunk_transfer_t *p_chunk = (blob_chunk_transfer_t *)par;
    int fw_data_len = par_len - 2;
    
    if(is_blob_chunk_transfer_ready() && ((fw_data_len > 0) && (fw_data_len <= get_max_chunk_size_check()))){
		if(0 == is_valid_chunk_transfer_size(p_chunk->chunk_num, fw_data_len)){
			return 0;
		}
    
        #if (ENCODE_OTA_BIN_EN && (!WIN32))
        u8 key[16];
        memcpy(key, key_encode_bin, sizeof(key));
        int dec_pos = 0;
        while(dec_pos + 16 <= fw_data_len){
            aes_decrypt(key, p_chunk->data + dec_pos, p_chunk->data + dec_pos);
            dec_pos += 16;
        }
        #endif
        //u32 bit_chunk = BIT(p_chunk->chunk_num);
        //LOG_MSG_LIB(TL_LOG_NODE_BASIC,0, 0,"chunk_num: 0x%04x",p_chunk->chunk_num);
        if((p_chunk->chunk_num * fw_update_srv_proc.block_start.chunk_size + fw_data_len) > (1 << fw_update_srv_proc.bk_size_log)){
            #if 0 // no need, just discard message.
            // MMDL/SR/BT/BV-31-C BLOB Chunk Transfer-Invalid Parameters: discard all that have been received
            set_bit_by_cnt(fw_update_srv_proc.miss_mask, sizeof(fw_update_srv_proc.miss_mask), updater_get_fw_chunk_cnt());
            LOG_MSG_ERR(TL_LOG_NODE_BASIC,0, 0,"invalid chunk number:");
            #endif
        }else if(p_chunk->chunk_num <= sizeof(fw_update_srv_proc.miss_mask)*8){
            #if 1 // VC_DISTRIBUTOR_UPDATE_CLIENT_EN
            #if 0   // test
            static volatile u8 skip_test = 0;
            if(skip_test){ // (p_chunk->chunk_num == 1)
                skip_test = 0;
                if(0x0002 == ele_adr_primary){
                	return 0;
                }
            }
            #endif

            if(is_buf_bit_set(fw_update_srv_proc.miss_mask, p_chunk->chunk_num)){
                buf_bit_clear(fw_update_srv_proc.miss_mask, p_chunk->chunk_num);
                u32 pos = updater_get_fw_data_position(p_chunk->chunk_num);
                mesh_ota_save_data(pos, fw_data_len, p_chunk->data);
                fw_update_srv_proc.bin_crc_done = 0;
				if(is_buf_zero(fw_update_srv_proc.miss_mask, sizeof(fw_update_srv_proc.miss_mask))){
					set_phase_block_all_chunk_rx_ok(0, 1);
				}
            }

			#if MESH_OTA_PULL_MODE_EN
			if(MESH_OTA_TRANSFER_MODE_PULL == fw_update_srv_proc.transfer_mode){
	            if((p_chunk->chunk_num == fw_update_srv_proc.report_chunk_idx_last)
	            ||(is_buf_zero(fw_update_srv_proc.miss_mask, sizeof(fw_update_srv_proc.miss_mask)))){
	            	mesh_ota_set_partial_report(cb_par->adr_src, 1);
	            	#if FEATURE_LOWPOWER_EN
					fw_update_srv_proc.report_timer_tick = clock_time() + BIT(31);	// report at once
	            	mesh_ota_proc();
	            	#endif
	            }else{
					pull_mode_report_timer_refresh(1);
	            }

				pull_chunk_tick_refresh();

	            if(BLOB_TRANS_PHASE_SUSPEND == fw_update_srv_proc.blob_trans_phase){
					fw_update_srv_proc.blob_trans_phase = BLOB_TRANS_PHASE_WAIT_NEXT_CHUNK;
	            }
			}
            #endif
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
    u32 crc_type1_blk = (FW_CHECK_AGTHM2 == fw_update_srv_proc.bin_crc_type) ? fw_update_srv_proc.crc_total : 0;
    __UNUSED u32 crc32_cal = 0;
    crc32_cal = soft_crc32_ota_flash(adr, fw_update_srv_proc.bk_size_current, 0,&crc_type1_blk);
    #if BLOCK_CRC32_CHECKSUM_EN
    if(check_val == crc32_cal)
    #endif
    {
        // for telink mesh crc
        u8 *mask = fw_update_srv_proc.blk_crc_tlk_mask;
        u16 blk_num = fw_update_srv_proc.block_start.block_num; // have make sure blk_num is valid before.
        if(!is_array_mask_en(mask, blk_num)){
            if(FW_CHECK_AGTHM2 == fw_update_srv_proc.bin_crc_type){
                fw_update_srv_proc.crc_total = crc_type1_blk;
            }else{
                fw_update_srv_proc.crc_total += crc_type1_blk;
            }
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

int is_mesh_ota_all_missing(u8 *miss_mask, u16 chunk_cnt)
{
    for(unsigned int i = 0; i < (chunk_cnt); i+=8){
        u8 miss_val = miss_mask[i / 8];
        if(i + 8 <= chunk_cnt){
            if(miss_val != 0xff){
                return 0;
            }
        }else{
            if(miss_val != BIT_MASK_LEN(chunk_cnt - i)){
                return 0;
            }
        }
    }

    return 1;
}

u32 get_missing_chunk_utf8(u8 *out_utf8, int len_out_max, u32 out_cnt_max, u16 chunk_cnt_total, u16 *out_last_chunk)
{
	u32 len_miss_utf8 = 0;
	u32 out_cnt = 0;
	for(unsigned int i = 0; i < (chunk_cnt_total); i+=8){
		u8 mask = fw_update_srv_proc.miss_mask[i/8];
		if(mask){
			foreach(j,8){
				u32 pos = i+j;
				if(pos >= chunk_cnt_total){
					break;
				}
				
				if(mask & BIT(j & 7)){
					int OutUTFLenMax = len_out_max - len_miss_utf8;
					u32 len = U16ToUTF8(pos, out_utf8 + len_miss_utf8, OutUTFLenMax);
					if(OutUTFLenMax >= (int)len){
						len_miss_utf8 += len;
						*out_last_chunk = pos;
						if(++out_cnt >= out_cnt_max){
							return len_miss_utf8;
						}
					}else{
						return len_miss_utf8;
					}
				}
			}
		}
	}

	return len_miss_utf8;
}

int mesh_cmd_sig_blob_block_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	blob_block_status_t rsp = {0};
	u8 st;
	u32 rsp_len = OFFSETOF(blob_block_status_t, miss_chunk);
	if((BLOB_TRANS_PHASE_SUSPEND == fw_update_srv_proc.blob_trans_phase)){ // (0 == fw_update_srv_proc.block_start.chunk_size) && 
	    // MMDL/SR/BT/BV-36-C (BLOB Block Get-IUT in Suspended Phase): never have received block start before
		st = BLOB_TRANS_ST_INFO_UNAVAILABLE;
	}else if(is_blob_block_get_ready() || fw_update_srv_proc.blob_block_get_retry_flag){ // it may retry get command sometimes.
        // TODO fill missing chunk
        if(0 == is_buf_zero(fw_update_srv_proc.miss_mask, sizeof(fw_update_srv_proc.miss_mask))){
            u16 chunk_cnt_total = updater_get_fw_chunk_cnt();
            #if 1 // PTS_TEST_OTA_EN
            if(
			#if MESH_OTA_PULL_MODE_EN
			(MESH_OTA_TRANSFER_MODE_PULL != fw_update_srv_proc.transfer_mode) &&
			#endif
            is_mesh_ota_all_missing(fw_update_srv_proc.miss_mask, chunk_cnt_total)){
                rsp.format = BLOB_BLOCK_FORMAT_ALL_CHUNK_MISS;
                LOG_MSG_LIB(TL_LOG_NODE_BASIC,0, 0,"all miss:");
            }else
            #endif
            {
                u32 len_miss_mask = CEIL_8(chunk_cnt_total);
				#if 1// (MESH_OTA_PULL_MODE_EN || PTS_OTA_TODO_EN)
            	u32 out_cnt_max = 0xffff;
					#if 0 // MESH_OTA_PULL_MODE_EN
				if(MESH_OTA_TRANSFER_MODE_PULL == fw_update_srv_proc.transfer_mode){
					out_cnt_max = sizeof(rsp.miss_chunk); // no need to be same with the number of partial report message.
				}
					#endif
				u16 last_chunk_utf8 = 0;
            	u32 len_miss_utf8 = get_missing_chunk_utf8(rsp.miss_chunk, sizeof(rsp.miss_chunk), out_cnt_max, chunk_cnt_total, &last_chunk_utf8);
                #endif
                
				#if MESH_OTA_PULL_MODE_EN
				if(MESH_OTA_TRANSFER_MODE_PULL == fw_update_srv_proc.transfer_mode){
					rsp_len += len_miss_utf8;
					fw_update_srv_proc.report_chunk_idx_last = last_chunk_utf8;
					mesh_ota_set_partial_report(cb_par->adr_src, 0); // just set flag
					rsp.format = BLOB_BLOCK_FORMAT_ENCODE_MISS_CHUNK;
				}else
				#endif
				{
	                #if 1// (MESH_OTA_PULL_MODE_EN || PTS_OTA_TODO_EN) // gateway has support encode format now.
	                if(len_miss_utf8 < len_miss_mask){
	                    rsp_len += len_miss_utf8;
	                    rsp.format = BLOB_BLOCK_FORMAT_ENCODE_MISS_CHUNK;
	                }else
	                #endif
	                {
	                    rsp_len += len_miss_mask;
	                    // have been make sure no redundance bit in miss mask in mesh_cmd_sig_blob_block_start_
	                    memcpy(rsp.miss_chunk, &fw_update_srv_proc.miss_mask, min(sizeof(rsp.miss_chunk), sizeof(fw_update_srv_proc.miss_mask)));
	                    rsp.format = BLOB_BLOCK_FORMAT_SOME_CHUNK_MISS;
	                }
                }
            }
        }else{
            // also check telink crc 16 total inside.
            #if BLOCK_CRC32_CHECKSUM_EN
            int crc_ok = block_crc32_check_current(fw_update_srv_proc.block_start.bk_check_sum_val);
            if(0 == crc_ok){  // SIG block crc32 ok
                st = BLOB_BLOCK_ST_WRONG_CHECK_SUM;
            }else
            #else
            int crc_ok = 1; // crc_ok =1 means have received all chunks
            #endif
            {
                set_phase_block_all_chunk_rx_ok(1, crc_ok);
                fw_update_srv_proc.blob_block_get_retry_flag = 1;
                rsp.format = BLOB_BLOCK_FORMAT_NO_CHUNK_MISS;
            }
        }

        st = BLOB_TRANS_ST_SUCCESS;
        if((UPDATE_PHASE_IDLE == fw_update_srv_proc.update_phase) && // no fw update start before, only blob transfer.
           (BLOB_TRANS_PHASE_COMPLETE == fw_update_srv_proc.blob_trans_phase)){
            if(FW_CHECK_NONE != get_ota_check_type()){
                if(is_valid_telink_fw_flag() && !is_valid_mesh_ota_calibrate_val()){
                    st = BLOB_TRANS_ST_INTERNAL_ERROR;
                }
            }
        }
	}else{
	    st = BLOB_TRANS_ST_WRONG_PHASE;
	}
	model_g_light_s_t *p_model = (model_g_light_s_t *)cb_par->model;
	rsp.st = st;
	//rsp.format = // have been set before;// miss_cnt ? BLOB_BLOCK_FORMAT_SOME_CHUNK_MISS : BLOB_BLOCK_FORMAT_NO_CHUNK_MISS;
    
	if((BLOB_TRANS_ST_INFO_UNAVAILABLE == st)||((BLOB_TRANS_ST_WRONG_PHASE == st) && (0 == fw_update_srv_proc.block_start.chunk_size))){
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
	#if PTS_TEST_OTA_EN
	fw_update_srv_proc.blob_info_get_flag = 1;
	#endif
	
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
