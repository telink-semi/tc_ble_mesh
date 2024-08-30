/********************************************************************************************************
 * @file	tlv_flash.c
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
#include "tlv_flash.h"
#include "app_provison.h"
#include "mesh_common.h"
#if TLV_ENABLE
	#if APP_FLASH_PROTECTION_ENABLE
#error TODO flash protection section setting
	#endif

extern unsigned short crc16(unsigned char *pD, int len);
STATIC_ASSERT((sizeof(tlv_str_t)+TLV_REC_WHOLE_PACKET_BUF) == (sizeof(tlv_str_max_t)+TLV_REC_MAX_BUF_CNT)); // make sure the two heads are same.

void tlv_rec_err_proc(u32 evt_id)
{
	LOG_MSG_ERR(TL_LOG_NODE_SDK,0,0,"tlv_rec_err_proc,%x",evt_id);
}

// this function will be optimized if log is disable.
void log_error_single_record_max_len(u32 len)
{
	LOG_MSG_ERR(TL_LOG_NODE_SDK,0,0,"excced the max len for single record,len is %x",len);
}

tlv_rec_mag_t tlv_rec;

void tlv_rec_para_init()
{
	tlv_rec.start = TLV_FLASH_START_ADR;
	tlv_rec.end = TLV_FLASH_START_ADR+TLV_SEC_SIZE;
	tlv_rec.recycle_adr = TLV_FLASH_LAST_SECTOR;
}

u16 tlv_calc_fun(u8 type,u8*p_buf,u16 len)
{
	if(type == 0){
		return crc16(p_buf,len);
	}else{
	// reserve for other check type
		return 0;
	}
}

u32 tlv_get_round_adr(u32 adr)
{
	if(adr>TLV_FLASH_REC_MAX_ADR){
		return (adr - TLV_FLASH_MAX_INTER);
	}else if(adr<TLV_FLASH_START_ADR){
		return TLV_FLASH_START_ADR;
	}else{
		return adr;
	}
}


u8 tlv_content_is_valid(tlv_str_max_t *tlv_max)
{
	u16 len = tlv_max->len;
	u8* p_crc = tlv_max->buf+len;
	u16 crc = p_crc[0]|(p_crc[1]<<8);
	if(tlv_max == 0){
		return FALSE;
	}
	if(crc == tlv_calc_fun(0,(u8*)tlv_max,len+TLV_HEADER_LEN)){
		return TRUE;
	}else{
		return FALSE;
	}
}

u8 tlv_check_data_valid(u32 start_adr)
{
	tlv_str_max_t tlv_data;
	// get the header first 
	flash_read_page(start_adr, TLV_HEADER_LEN, (u8*)&tlv_data);
	if( tlv_data.valid == TLV_STORE_VALID_FLAG && 
		tlv_data.len<=TLV_REC_WHOLE_PACKET_BUF &&
		((u16)(~tlv_data.len) ==tlv_data.len_check) ){
		// read all the packet
		flash_read_page(start_adr, TLV_CONTENT_LEN+tlv_data.len,(u8*)&tlv_data);
		//when do the check ,we should suppose the enable 
		tlv_data.en=1;
		return (tlv_content_is_valid(&tlv_data));
	}
	return FALSE;
}

u8 tlv_check_len_valid(u32 start_adr)
{
	tlv_str_t tlv_data;
	flash_read_page(start_adr, TLV_HEADER_LEN, (u8*)&tlv_data);
	if( tlv_data.valid == TLV_STORE_VALID_FLAG && 
		tlv_data.len<=TLV_REC_WHOLE_PACKET_BUF &&
		((u16)~tlv_data.len ==tlv_data.len_check)  ){
		return TRUE;
	}else{
		return FALSE;
	}
}
void tlv_disable_record(u32 start_adr)
{
	tlv_str_t tlv_data;
	// get the header first 
	flash_read_page(start_adr, TLV_HEADER_LEN, (u8*)&tlv_data);
	tlv_data.en = 0;
	flash_write_page(start_adr, TLV_HEADER_LEN, (u8*)&tlv_data);
}

void tlv_set_write_para(tlv_str_max_t *p_tlv,u32 id,u8 *p_buf,u16 len)
{
	u16 crc;
	u8* p_crc = p_tlv->buf+len;
	p_tlv->valid = TLV_STORE_VALID_FLAG;
	p_tlv->id = id;
	p_tlv->ver = TLV_FLASH_VERSION;
	p_tlv->en =1;
	p_tlv->rsv =0;
	memset(p_tlv->rfu,0,sizeof(p_tlv->rfu));
	p_tlv->len_check = ~len;
	p_tlv->len = len;
	// write the tlv header part .
	memcpy(p_tlv->buf,p_buf,len);
	crc = tlv_calc_fun(0,(u8*)p_tlv,TLV_HEADER_LEN + len);
	p_crc[0]=crc&0xff;
	p_crc[1]=crc>>8;
}

u8 tlv_flash_write(u32 id,u8 *p_buf,u16 len )
{
	tlv_str_max_t tlv_data;
// 	LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"tlv1_flash_write %x,len is %x",tlv_rec.end,len);
	if(len > sizeof(tlv_data.buf)){
		log_error_single_record_max_len(len);
		return FALSE;
	}
	if(p_buf==0 ||len==0){
		return FALSE;
	}
	tlv_set_write_para(&tlv_data,id,p_buf,len);
	flash_write_page(tlv_rec.end,(u32)(len+TLV_CONTENT_LEN),(u8*)&tlv_data);
	if(tlv_check_data_valid(tlv_rec.end)){
		tlv_rec.end += len+TLV_CONTENT_LEN;
//		LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"tlv2_flash_write %x,",tlv_rec.end);
		return 1;
	}else{
		tlv_disable_record(tlv_rec.end);
		tlv_rec.end += len+TLV_CONTENT_LEN;
		tlv_rec_err_proc(1);
		start_reboot();
		return 0;
	}
}

u8 get_tlv_record_adr(u32 record_id,u32 *rec_adr,u32 *rec_last,u32 *p_rec_len)
{
	u32 start = tlv_rec.start;
	u32 end = tlv_rec.end;
	u8 valid =0;
	u32 adr_cnt =0;
	u32 total_cnt =0;
	u8 skip_flag =0;
	if(start < end){
		total_cnt = end -start;
	}else{
		total_cnt = end+TLV_FLASH_MAX_INTER-start;
	}
	while(start != end){
		// need to find the last rec id ?
		tlv_str_t tlv_data;
//		LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"get_tlv_record_adr start is %x,end is %x",start,end);
		flash_read_page(start,sizeof(tlv_data),(u8*)&tlv_data);
		if(tlv_check_len_valid(start)){
			if(tlv_data.en && tlv_data.id == record_id){
				*rec_last = *rec_adr;
				*rec_adr = start;
				*p_rec_len = tlv_data.len;
				valid =1;
			}
			// move to the next rec start adr .
			start += TLV_CONTENT_LEN + tlv_data.len;
			adr_cnt+=TLV_CONTENT_LEN + tlv_data.len;
			#if (MODULE_WATCHDOG_ENABLE)
			wd_clear(); //clear watch dog
			#endif
		}else if (tlv_data.id == TLV_EMPTY_ID){
			// get the adr cnt first 
			adr_cnt +=(TLV_SEC_SIZE-start%TLV_SEC_SIZE);	
			// jump to next sector
			start = tlv_get_round_adr(TLV_SECTOR_START(start)+TLV_SEC_SIZE);
			continue;
		}
		else{
			// if exist invalid block ,we should jump one by one until find valid 
			skip_flag =1;
		}
		if((start+TLV_CONTENT_LEN)>=(TLV_SECTOR_START(start)+TLV_FLASH_SECTOR_END_ADR)){
			// get the adr cnt first 
			adr_cnt +=(TLV_SEC_SIZE-start%TLV_SEC_SIZE);
			// jump to next sector
			start = tlv_get_round_adr(TLV_SECTOR_START(start)+TLV_SEC_SIZE);
		}else if (skip_flag){
			skip_flag =0;
			start =tlv_get_round_adr(start+1);
			adr_cnt++;
		}
		if(adr_cnt>=total_cnt){
			// already accomplish all the record.
			break;
		}
	}
	return valid;
}

u8 flash_sector_is_empty(u32 start_adr)
{
	u32 adr = start_adr&0xfffff000; 
	u32 start=0;
	u32	end =0; 
	flash_read_page(adr, sizeof(start), (u8*)&start);
	flash_read_page(adr+TLV_FLASH_SECTOR_END_ADR, sizeof(end),(u8*)&end);
	if(start == TLV_EMPTY_ID && end == TLV_EMPTY_ID){
		return 1;
	}else{
		return 0;
	}
}

u8 tlv_empty_cnt_get(u32* p_first)
{
	u8 cnt =0;
	for(int i=0;i<(TLV_FLASH_MAX_INTER/TLV_SEC_SIZE);i++){
		if(flash_sector_is_empty(TLV_FLASH_START_ADR+i*TLV_SEC_SIZE)){
			if(cnt == 0){
				*p_first = TLV_FLASH_START_ADR+i*TLV_SEC_SIZE;
			}
			cnt++;
		}
	}
	return cnt;
}

u8 tlv_sector_need_recycle()
{
	u32 first =0;
 	u8 cnt = tlv_empty_cnt_get(&first);
	if(cnt>=2){
		return 0;
	}else{
		return 1;
	}
}

u8 flash_sector_is_unfinish(u32 start_adr)
{
	u32 adr = start_adr&0xfffff000;
	u16 end =0;
	flash_read_page(adr+TLV_FLASH_SECTOR_END_ADR, sizeof(end), (u8*)&end);
	if(end == TLV_END_MIDDLE_VALUE ){
		return 1;
	}else{
		return 0;
	}
}

void tlv_invalid_sector_get(u32* p_first)
{
	for(int i=0;i<(TLV_FLASH_MAX_INTER/TLV_SEC_SIZE);i++){
		if(flash_sector_is_unfinish(TLV_FLASH_START_ADR+i*TLV_SEC_SIZE)){
			*p_first = TLV_FLASH_START_ADR+i*TLV_SEC_SIZE;
			break;
		}
	}
	return;
}

u32 get_tlv_end_by_inter(u32 start,u32 end)
{
	u8 skip_flag =0;
	while(start < end){
		// need to find the last rec id ?
		tlv_str_t tlv_data;
		flash_read_page(start,sizeof(tlv_data),(u8*)&tlv_data);
		if (tlv_data.id == TLV_EMPTY_ID){//suppose is the end 
			return start;
		}else if(tlv_check_len_valid(start)){
			start += (TLV_HEADER_LEN+TLV_CRC_LEN+ tlv_data.len);
			#if (MODULE_WATCHDOG_ENABLE)
			wd_clear(); //clear watch dog
			#endif
		}else{
			// if exist invalid block ,we should jump one by one until find valid 
			skip_flag = 1;
		}	
		if((start+TLV_CONTENT_LEN)>=end){
			return start;
			// don't have enough space for single strcut
		}else if(skip_flag){
			start++;
			skip_flag = 0;
		}
	}
	return end;

}


void save_map_len_check()
{
	static u32 A_debug_save_map_check=0;
	for(int i=0;i<mesh_save_map_array;i++){
		if(mesh_save_map[i].size_save_par > TLV_REC_WHOLE_PACKET_BUF){
			A_debug_save_map_check++;
			LOG_MSG_ERR(TL_LOG_NODE_SDK,0,0,"tlv buffer error ,save map is %x,len is %x",i,mesh_save_map[i].size_save_par);
		}
	}
}


void tlv_init()
{
	// find the empty cnt in the record flash part 
	u32 first_empty;
	save_map_len_check();
	tlv_rec_para_init();
	u8 cnt =tlv_empty_cnt_get(&first_empty);
	if(cnt == TLV_FLASH_MAX_INTER/TLV_SEC_SIZE){
		// all empty
		tlv_rec.start = TLV_FLASH_START_ADR;
		tlv_rec.end = TLV_FLASH_START_ADR;
		tlv_rec.recycle_adr = TLV_FLASH_LAST_SECTOR;
		return ;
	}else if(cnt>=2){
		// 1st round to proc the tlv 
		tlv_rec.start = TLV_FLASH_START_ADR;
		tlv_rec.end = first_empty;//move the pre adr to find.
		tlv_rec.recycle_adr = TLV_FLASH_LAST_SECTOR;
	}else if (cnt == 1){		
		tlv_rec.start = tlv_get_round_adr(first_empty+TLV_SEC_SIZE);//move the pre adr to find.
		tlv_rec.end = first_empty;
		tlv_rec.recycle_adr = first_empty;
	}else if(cnt == 0){
		// suppose the recycle error ,when recycle not accomplish ,and it power off 
		u32 invalid_adr =0;
		tlv_invalid_sector_get(&invalid_adr);
		if(invalid_adr == 0){
			invalid_adr = TLV_FLASH_LAST_SECTOR;// if can not find select last sector
			tlv_rec_err_proc(2);// must at least have one sector
		}
		flash_erase_sector(invalid_adr);
		tlv_rec.start = tlv_get_round_adr(invalid_adr+TLV_SEC_SIZE);//move the pre adr to find.
		tlv_rec.end = invalid_adr;
		tlv_rec.recycle_adr = invalid_adr;
	}
	// need to find the first empty adr for the valid sector,only find in one sector
	if(cnt != TLV_FLASH_MAX_INTER/TLV_SEC_SIZE){
		u32 start=0;
		u32 end =0;
		if(tlv_rec.end == TLV_FLASH_START_ADR){
			start = TLV_FLASH_LAST_SECTOR;
			end = TLV_FLASH_REC_MAX_ADR;
		}else{
			start = tlv_rec.end-TLV_SEC_SIZE;
			end = tlv_rec.end;
		}
		tlv_rec.end = get_tlv_end_by_inter(start,end);
	}
	LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"tlv_init,start %x,end %x,empty cnt is %x ",tlv_rec.start,tlv_rec.end,cnt);
}

void tlv_rec_clear(u32 id)
{
	u32 rec_adr =0;
	u32 rec_adr_last=0;
	u32 rec_len =0;
	if(get_tlv_record_adr(id,&rec_adr,&rec_adr_last,&rec_len)){
//		LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"rec clear id is %x,last adr is %x",id,rec_adr_last);
		if(rec_adr_last!=0){
			LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"rec clear id is %x,last adr is %x",id,rec_adr_last);
			tlv_disable_record(rec_adr_last);
		}
	}
}

void tlv_recycle_write(u8*p_buf,u16 len)
{
	u32 recycle_middle = TLV_END_MIDDLE_VALUE;
	if(p_buf==0 ||len==0){
		return ;
	}
	if((tlv_rec.recycle_adr%TLV_SEC_SIZE) == 0){
		// need to write the recycle middle state
		flash_write_page(tlv_rec.recycle_adr+TLV_FLASH_SECTOR_END_ADR,sizeof(recycle_middle),(u8*)&recycle_middle);
	}
//	LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"recycle_adr is %x,len is %x",tlv_rec.recycle_adr,len);
	flash_write_page(tlv_rec.recycle_adr,len,p_buf);
	if(tlv_check_data_valid(tlv_rec.recycle_adr)){
		tlv_rec.recycle_adr+=len;
	}else{
		tlv_rec_err_proc(3);
		// can not recycle ,should erase and reboot
		flash_erase_sector(tlv_rec.recycle_adr);
		start_reboot();
	}
}


void tlv_recycle_by_adr()
{
	u32 idx =0;
	u8 skip_flag =0;
	//u32 recycle_adr = tlv_rec.recycle_adr;
	u32 rec_adr = tlv_get_round_adr(TLV_SECTOR_START(tlv_rec.recycle_adr)+TLV_SEC_SIZE);
	tlv_str_max_t tlv_data;
	// make sure the recycle sector is empty
	if(!flash_sector_is_empty(tlv_rec.recycle_adr)){
		tlv_rec_err_proc(4);
		flash_erase_sector(tlv_rec.recycle_adr);
		//start_reboot();// no need to reboot.
	}
	// recycle the next sector to the empty sector
	while(idx <= TLV_FLASH_SECTOR_END_ADR){
		// need to loop all the adr .
		// get header part .
		flash_read_page(rec_adr+idx,TLV_CONTENT_LEN,(u8*)&tlv_data);
		if (tlv_data.id == TLV_EMPTY_ID){
			// suppose is the end
			break;
		}else{
			if(tlv_data.len <= TLV_REC_WHOLE_PACKET_BUF){
				if(tlv_check_len_valid(rec_adr+idx)){
					if(tlv_data.en && ((idx+TLV_CONTENT_LEN+tlv_data.len)<TLV_FLASH_SECTOR_END_ADR)){
						//LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"rec idx is %x,len is %x",idx,tlv_data.len);
						flash_read_page(rec_adr+idx,TLV_CONTENT_LEN+tlv_data.len,(u8*)&tlv_data);
						LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"tlv_max rec idx is %x,len is %x",idx,tlv_data.len);
						tlv_recycle_write((u8*)&tlv_data,TLV_CONTENT_LEN+tlv_data.len);
					}
					// move to the next rec start adr .
					idx += (TLV_CONTENT_LEN+ tlv_data.len);
					#if (MODULE_WATCHDOG_ENABLE)
					wd_clear(); //clear watch dog
					#endif
				}else{
					// if exist invalid block ,we should jump one by one until find valid 
					skip_flag =1;
				}
			}else{
				log_error_single_record_max_len(tlv_data.len);
				return ;
			}
		}
		if((idx+TLV_CONTENT_LEN)>=TLV_FLASH_SECTOR_END_ADR){
			break;
 		}else if(skip_flag){
 			idx++;
			skip_flag = 0;
 		}
	}
	// set the tlv_rec end first 
	tlv_rec.end = tlv_rec.recycle_adr;
	// after finish the recycle part . erase the rec sector first
	flash_erase_sector(rec_adr);
	u32 recycle_valid = TLV_END_VALID_VALUE;
//	LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"end is %x,rec_adr is %x,recycle adr is %x",tlv_rec.end,rec_adr,tlv_rec.recycle_adr);
	flash_write_page(TLV_SECTOR_START(tlv_rec.recycle_adr)+TLV_FLASH_SECTOR_END_ADR,sizeof(recycle_valid),(u8*)&recycle_valid);
	tlv_rec.recycle_adr = rec_adr;
	// set the tlv_rec start first.move the start to first of the next sector
	tlv_rec.start = tlv_get_round_adr(rec_adr+TLV_SEC_SIZE);
}

void tlv_recycle_proc(u16 len)
{
	// current sector is enough or not 
	while((tlv_rec.end+len+TLV_CONTENT_LEN) > (TLV_SECTOR_START(tlv_rec.end)+TLV_FLASH_SECTOR_END_ADR)){
		// if not have enough space it will trigger the recycle ,until it have enough space
		tlv_recycle_by_adr();
	}
}

void tlv_rec_move_next_sector()
{
	tlv_rec.end = TLV_SECTOR_START(tlv_rec.end)+TLV_SEC_SIZE;
	// check the sector is empty or not 
	if(!flash_sector_is_empty(tlv_rec.end)){
		// to void the rewrite part .
		flash_erase_sector(tlv_rec.end);
	}
}

u8 tlv_rec_write(u32 id,u8 *p_buf,u16 len )
{	
	if(p_buf==0 ||len==0){
		return FALSE;
	}
	if((tlv_rec.end+len+TLV_CONTENT_LEN) > (TLV_SECTOR_START(tlv_rec.end)+TLV_FLASH_SECTOR_END_ADR)){
		if(tlv_sector_need_recycle()){
			LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"recycle start %x,",tlv_rec.end);
			tlv_recycle_proc(len);
			LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"recycle end %x,",tlv_rec.end);
		}else{
			// suppose the sector is empty
			tlv_rec_move_next_sector();
			LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"tlv move to next %x,",tlv_rec.end);
		}
	}
	if(tlv_rec.end %TLV_SEC_SIZE == 0){
		u32 valid_flag = TLV_END_VALID_VALUE;
		flash_write_page(tlv_rec.end+TLV_FLASH_SECTOR_END_ADR,sizeof(valid_flag),(u8*)&valid_flag);
	}
	tlv_flash_write(id,p_buf,len);
	tlv_rec_clear(id);
	return TRUE;
}

u8 tlv_rec_read(u32 id,u8 *p_buf,u16 len)
{
	u32 rec_adr =0;
	u32 rec_adr_last =0;
	u32 rec_len =0;
	u16 cpy_len;
	if(p_buf==0 ||len==0){
		return FALSE;
	}
	if(get_tlv_record_adr(id,&rec_adr,&rec_adr_last,&rec_len)){
		// if have more than one valid record,it will disable the last
		if(rec_adr_last!=0){
			tlv_disable_record(rec_adr_last);
		}
		if(len > TLV_REC_WHOLE_PACKET_BUF){
			log_error_single_record_max_len(len);
			return FALSE;
		}
		// get the data first ,and check the valid flag;
		if(tlv_check_data_valid(rec_adr)){
			// data is valid 
			if(len>=rec_len){
				cpy_len = rec_len;
			}else{
				cpy_len = len;
			}
			tlv_str_max_t tlv_data;
			flash_read_page(rec_adr,rec_len+TLV_CONTENT_LEN, (u8*)&tlv_data);
			memcpy(p_buf,tlv_data.buf,cpy_len);
			return TRUE;
		}else{
			return FALSE;
		}
	}
	return FALSE;
}

#if 0
u8 A_debug_test1[sizeof(model_common_t)];
u8 A_debug_test2[sizeof(model_common_t)];
void tlv_test_init()
{
	LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"tlv_init start");
	tlv_init();
	LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"tlv_init end");
	u8 test1[sizeof(model_common_t)];
	u8 test2[sizeof(model_common_t)];
	memset(test1,1,sizeof(test1));
	memset(test2,2,sizeof(test2));
	LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"tlv_write start");
	tlv_rec_write(3,test1,sizeof(test1));
	LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"tlv_write end");
	tlv_rec_write(4,test2,sizeof(test2));
	LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"tlv_read start");
	tlv_rec_read(3,A_debug_test1,sizeof(A_debug_test1));
	LOG_MSG_INFO(TL_LOG_NODE_SDK,0,0,"tlv_read end");
	tlv_rec_read(4,A_debug_test2,sizeof(A_debug_test2));
	irq_disable();
	while(1){
		wd_clear();
	}
}
#endif


#endif

