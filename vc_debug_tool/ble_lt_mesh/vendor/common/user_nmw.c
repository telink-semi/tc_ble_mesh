/********************************************************************************************************
 * @file	user_nmw.c
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
#include "proj/tl_common.h"
#include "proj_lib/sig_mesh/app_mesh.h"
#include "proj_lib/ble/service/ble_ll_ota.h"
#include "mesh_ota.h"
#include "user_ali.h"
#include "user_nmw.h"

#if NMW_ENABLE
const nmw_version_t nmw_fw_version = {
	.mcu_version = 0,
	.fw_version = NMW_MODULE_VERSION,
	.hw_version = 1,
};

const u8 nmw_fw_version_string[]={
	NMW_MAIN_VERSION_BYTE2 	NMW_MAIN_VERSION_BYTE1 	NMW_MAIN_VERSION_BYTE0
	'.',
	NMW_SUB_VERSION_BYTE2	NMW_SUB_VERSION_BYTE1	NMW_SUB_VERSION_BYTE0	
};

nmw_ota_st_t nmw_ota_state;

int nmw_ctl_rsp(u16 op, u8 *p, int len)
{
	nmw_rsp_t nmw_rsp;
	if(len > NMW_RSP_BUF_LEN){
		return -1;
	}

	nmw_rsp.op = op;
	memcpy(nmw_rsp.payload, p, len);
	return bls_att_pushIndicateData(NMW_CONTROL_HANDLE, (u8 *)&nmw_rsp,len+sizeof(nmw_rsp.op));
}

int nmw_ota_st_report(u16 op, u8 errno)
{
	int ret = nmw_ctl_rsp(op, &errno, sizeof(errno));
	if(ret){
		nmw_ota_state.op = op;
		nmw_ota_state.errno = errno;
		nmw_ota_state.errno_pending = 1;
	}
	else{
		nmw_ota_state.errno_pending = 0;
	}
	return ret;
}

int nmw_ota_seg_report()
{
	rec_seg_map_t seg_map;	
	nmw_ota_state.seg_timeout_tick = clock_time()|1;
	seg_map.start_index = (nmw_ota_state.ota_index/MAX_DEVICE_CACHE_NUM*MAX_DEVICE_CACHE_NUM);
	seg_map.seg_map = reverse_bit64(nmw_ota_state.seg_map);
	endianness_swap_u16((u8 *)&seg_map.start_index);
	endianness_swap_u64((u8 *)&seg_map.seg_map);
	return nmw_ctl_rsp(NMW_OTA_ST_REPORT, (u8 *)&seg_map, sizeof(seg_map));
}

nmw_tlv_t *nmw_get_tlv_by_type(u8 *payload, int len, int type)
{
	int offset = 0;
	while((payload[offset] != type) && (offset < len)){
		offset += (payload[offset+1] + 2);
	};

	if((payload[offset] == type) && (offset < len)){
		return (nmw_tlv_t *)(payload+offset);
	}
	else{
		return NULL;
	}
}


int	nmw_ctl_write (void *p)
{
	rf_packet_att_write_t *pw = (rf_packet_att_write_t *)(p);
	nmw_ctl_t *p_ctl = (nmw_ctl_t *)(&pw->value);
	switch(p_ctl->op){	
		case NMW_CACHE_NUM_GET:{
			u8 cache_num = MAX_DEVICE_CACHE_NUM;
			nmw_ctl_rsp(NMW_CACHE_RSP, &cache_num, 1);
			}
			break;
		case NMW_OTA_REQ:{
			u8 errno = OTA_SUPPORT;
			u32 pid=0;
			u16 fw_version=0;
			u32 fw_size=0;
			
			nmw_tlv_t *p_tlv = nmw_get_tlv_by_type(p_ctl->payload, pw->l2capLen-3-OFFSETOF(nmw_ctl_t, payload), TYPE_PID);
			if(p_tlv){
				pid = (p_tlv->value[0]<<24) + (p_tlv->value[1]<<16) + (p_tlv->value[2]<<8) + p_tlv->value[3];
			}

			p_tlv = nmw_get_tlv_by_type(p_ctl->payload, pw->l2capLen-3-OFFSETOF(nmw_ctl_t, payload), TYPE_FW_VER);
			if(p_tlv){		
				u8 ver_tmp = 0;
				foreach(i,p_tlv->len){				
					if(((p_tlv->value[i]>='0') && (p_tlv->value[i]<='9')) || (p_tlv->value[i]<='.')){						
						if('.' == p_tlv->value[i]){
							fw_version = (ver_tmp<<8); // version high byte
							ver_tmp = 0;
							continue;
						}
						ver_tmp = ver_tmp*10;
						ver_tmp += VER_CHAR2NUM(p_tlv->value[i]);		
					}
				}

				fw_version += ver_tmp; // add version low byte
			}

			p_tlv = nmw_get_tlv_by_type(p_ctl->payload, pw->l2capLen-3-OFFSETOF(nmw_ctl_t, payload), TYPE_FW_SIZE);
			if(p_tlv){
				foreach(i,p_tlv->len){
					fw_size = fw_size<<8;
					fw_size |= p_tlv->value[i];				
				}
			}

			if(pid != con_product_id){
				errno = OTA_PID_UNMATCH;
			}	
			else if(fw_version < nmw_fw_version.fw_version){
				errno = OTA_FW_VER_ERR;
			}
			else if(fw_version == nmw_fw_version.fw_version){
				errno = OTA_SAME_VER;
			}
			else if(fw_size > (FW_SIZE_MAX_K<<10)){
				errno = OTA_SIZE_OVERFLOW;
			}
			nmw_ctl_rsp(NMW_OTA_RSP, &errno, sizeof(errno));
			}
			break;
		case NMW_DEV_INFO_GET:{
			int offset = 0;
			u8 dev_info[NMW_RSP_BUF_LEN];
			u8 module_dev_name[] = NMW_MODULE_DEV_NAME;
//			u8 module_sub_name[] = NMW_MODULE_SUB_DEV_NAME;
			
			dev_info[offset++] = TYPE_MODULE_DEV_NAME;
			dev_info[offset++] = sizeof(module_dev_name);
			memcpy(dev_info+offset, module_dev_name, sizeof(module_dev_name));
			offset += sizeof(module_dev_name);

			u32 module_dev_type;
			swap32((u8 *)&module_dev_type, (u8 *)&con_product_id);
			dev_info[offset++] = TYPE_MODULE_DEV_TYPE;
			dev_info[offset++] = sizeof(module_dev_type);
			memcpy(dev_info+offset, &module_dev_type, sizeof(module_dev_type));
			offset += sizeof(module_dev_type);

			u32 module_dev_id;
			swap32((u8 *)&module_dev_id, (u8 *)&con_product_id);
			dev_info[offset++] = TYPE_MODULE_DEV_ID;
			dev_info[offset++] = sizeof(module_dev_id);
			memcpy(dev_info+offset, &module_dev_id, sizeof(module_dev_id));
			offset += sizeof(module_dev_id);

			dev_info[offset++] = TYPE_MODULE_FW_VERSION;
			dev_info[offset++] = sizeof(nmw_fw_version_string);
			memcpy(dev_info+offset, &nmw_fw_version_string, sizeof(nmw_fw_version_string));
			offset += sizeof(nmw_fw_version_string);
			nmw_ctl_rsp(NMW_DEV_INFO_RSP, dev_info, offset);
			}
			break;
		default:
			nmw_ota_st_report(NMW_ERR_REPORT, CMD_UNKNOWN);
			break;
	}

	return 0;
}

void nmw_ota_seg_map_update(u16 seg_index, int len)
{
	nmw_ota_state.image_offset += len;
	nmw_ota_state.seg_map |= BIT(seg_index%MAX_DEVICE_CACHE_NUM);
	nmw_ota_state.ota_index = seg_index;
}

u8 nmw_ota_data_save(u16 seg_index, u8 *p_data, int len)
{
	int err = NMW_OTA_SUCCESS;
	if((nmw_ota_state.ota_index + 1) == seg_index){		
		if((nmw_ota_state.file_type == MODULE_FILE) || (nmw_ota_state.file_type == MCU_FILE)){
			blt_ota_start_tick = clock_time()|1;
			if((seg_index == 0) && (nmw_ota_state.file_type == MODULE_FILE)){
	            if(!is_valid_tlk_fw_buf(p_data+8)){ // telink bin file check
	                err = OTA_OTA_SIGNATURE_ERR;
	            }
			}		
		
			if(nmw_ota_state.image_offset <= (ota_firmware_size_k<<10)){
			
				ota_save_data (nmw_ota_state.image_offset, p_data, len);

				u8 flash_check[256];
				flash_read_page(ota_program_offset + nmw_ota_state.image_offset, len, flash_check);
				if(!memcmp(flash_check, p_data,len)){  //OK
					nmw_ota_seg_map_update(seg_index, len);
				}
				else{ //flash write err
					err = NMW_OTA_WRITE_FLASH_ERR;
				}		
			}
		}		
		else{
			nmw_ota_seg_map_update(seg_index, len);
		}
	}
	else{
		//
	}

	return err;
}

int	nmw_ota_write(void *p)
{
	rf_packet_att_write_t *pw = (rf_packet_att_write_t *)(p);	
	nmw_ota_write_t *p_ota_write = (nmw_ota_write_t *)&pw->value;
	u8 err_flg = NMW_OTA_SUCCESS;
	
	endianness_swap_u16((u8 *)&p_ota_write->seg_index);

	u16 seg_index = p_ota_write->seg_index;
	u8 ota_len = 0;
	u8 *ota_data = 0;
	if(0 == seg_index){
		endianness_swap_u16((u8 *)&p_ota_write->tlv_size);
		endianness_swap_u32((u8 *)&p_ota_write->file_size);
		blcOta.ota_start_flag = 1; // ota start flag
		blt_ota_start_tick = clock_time()|1;  //mark time
		
		memset(&nmw_ota_state, 0x00, sizeof(nmw_ota_state));
		nmw_ota_state.ota_index = -1;
		
		nmw_ota_state.file_type = p_ota_write->file_type;
		
		nmw_ota_state.image_size = p_ota_write->file_size;	
		
		ota_data = p_ota_write->data;
		ota_len = (pw->l2capLen - 3) - OFFSETOF(nmw_ota_write_t, tag) - p_ota_write->tlv_size;
		
	}
	else{	
		ota_data = p_ota_write->seg_payload;
		ota_len = ((pw->l2capLen - 3)-OFFSETOF(nmw_ota_write_t, seg_payload));
	}

	err_flg = nmw_ota_data_save(seg_index, ota_data, ota_len);
	
	nmw_ota_state.seg_timeout_tick = clock_time()|1;
	if((nmw_ota_state.image_offset >= nmw_ota_state.image_size) ){	
		nmw_ota_seg_report();
		if((nmw_ota_state.file_type == MODULE_FILE) || (nmw_ota_state.file_type == MCU_FILE)){
			if((nmw_ota_state.file_type == MODULE_FILE) && (FW_CHECK_AGTHM1 == get_ota_check_type())){
				if(!is_valid_ota_check_type1()){
					err_flg = OTA_OTA_SIGNATURE_ERR;
				}
			}

			nmw_ota_state.seg_timeout_tick = 0;
			nmw_ota_st_report(NMW_OTA_RESULT_REPORT, err_flg);
			blt_ota_finished_flag_set(err_flg); // will process in bls_ota_procTimeout
		}
	}
	else if(0==(p_ota_write->seg_index+1)%MAX_DEVICE_CACHE_NUM){
		nmw_ota_seg_report();
		if(nmw_ota_state.ota_index == seg_index){ 
			nmw_ota_state.seg_map = 0; // clear for next round
		}
	}
	
	return 0;
}
#endif

