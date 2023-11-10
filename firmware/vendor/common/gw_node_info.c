 /********************************************************************************************************
  * @file	 gw_node_info.c
  *
  * @brief	 for TLSR chips
  *
  * @author  telink
  * @date	 Sep. 30, 2010
  *
  * @par	 Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
  * 		 All rights reserved.
  *
  * 		 Licensed under the Apache License, Version 2.0 (the "License");
  * 		 you may not use this file except in compliance with the License.
  * 		 You may obtain a copy of the License at
  *
  * 			 http://www.apache.org/licenses/LICENSE-2.0
  *
  * 		 Unless required by applicable law or agreed to in writing, software
  * 		 distributed under the License is distributed on an "AS IS" BASIS,
  * 		 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * 		 See the License for the specific language governing permissions and
  * 		 limitations under the License.
  *
  *******************************************************************************************************/
#include "tl_common.h"
#include "proj_lib/ble/blt_config.h"
#include "proj_lib/sig_mesh/app_mesh.h"
#include "gw_node_info.h"

#if (IS_VC_PROJECT_MASTER || DONGLE_PROVISION_EN)
 /*************hci_tx_fifo_poll ***********************
 device key list save and read
 ******************************************************/ 

 #if DONGLE_PROVISION_EN
VC_node_info_t VC_node_info[1];
gw_node_info_t gw_node_info[MESH_NODE_MAX_NUM];
VC_node_cps_t VC_node_cps;
u32 gw_node_info_addr_save = FLASH_ADR_VC_NODE_INFO;
#else
VC_node_info_t VC_node_info[MESH_NODE_MAX_NUM];        // 1000*(20+384) = 404000
#endif
 
#if WIN32 
STATIC_ASSERT(sizeof(VC_node_info) <= 4096*128);
#else
STATIC_ASSERT(ARRAY_SIZE(gw_node_info) <= (FLASH_ADR_VC_NODE_INFO_END - FLASH_ADR_VC_NODE_INFO)/sizeof(VC_node_info_t)); // make sure enough flash area to save
#endif

#if WIN32
void save_vc_node_info_all()
{
    // erase all the store vc node info part 
    // erase_vc_node_info(); // no need erase, write directly later
    flash_write_page(FLASH_ADR_VC_NODE_INFO, sizeof(VC_node_info), (u8 *)VC_node_info);
}

void save_vc_node_info_single(VC_node_info_t *p_info)
{
    // erase all the store vc node info part 
    // erase_vc_node_info(); // no need erase, write directly later
    u32 adr_save = FLASH_ADR_VC_NODE_INFO + (u8 *)p_info - (u8 *)&VC_node_info;
    flash_write_page(adr_save, sizeof(VC_node_info_t), (u8 *)p_info);
}
#endif

#if DONGLE_PROVISION_EN
int find_gw_node_empty_idx()
{
	foreach_arr(i, gw_node_info){
		if(0 == gw_node_info[i].node_adr){
			return i;
		}
	}

	return -1;
}

int get_gw_node_info_idx(u16 addr, int is_must_primary)
{
	int idx = -1;
	if(addr && is_unicast_adr(addr)){
		foreach_arr(i, gw_node_info){		
			if(is_must_primary){
				if(addr == gw_node_info[i].node_adr){
					idx = i;
					break;
				}
			}
			else{			
				if((is_ele_in_node(addr, gw_node_info[i].node_adr, gw_node_info[i].ele_cnt))){
					idx = i;
					break;
				}
			}
		}
	}

	return idx;
}

int restore_node_info_table()
{
	int idx = 0;
	u32 last_record = 0;
	VC_node_info_t node_info;
	memset((u8 *)(gw_node_info), 0x00, sizeof(gw_node_info));
	for(u32 addr_read = FLASH_ADR_VC_NODE_INFO; addr_read < FLASH_ADR_VC_NODE_INFO_END; addr_read += sizeof(VC_node_info_t)){ 
		flash_read_page(addr_read, sizeof(VC_node_info_t), (u8 *)&node_info);	
		gw_node_info_t *p_gw_node = &gw_node_info[idx];		
		if(node_info.node_adr && is_unicast_adr(node_info.node_adr)){
			int exist_idx = get_gw_node_info_idx(node_info.node_adr, 1);
			if(-1 != exist_idx){
				clear_gw_node_info(node_info.node_adr, 1); // set old record as invalid
				p_gw_node = &gw_node_info[exist_idx];
			}

			if(idx<ARRAY_SIZE(gw_node_info) || (-1 != exist_idx)){				
				p_gw_node->node_adr = node_info.node_adr;
				p_gw_node->ele_cnt = node_info.element_cnt;
				p_gw_node->index = (addr_read-FLASH_ADR_VC_NODE_INFO)/sizeof(VC_node_info_t);
				idx++;
			}
		}		
		
		if(node_info.node_adr != 0xffff){
			last_record = addr_read;
		}
	}

	gw_node_info_addr_save = last_record ? (last_record + sizeof(VC_node_info_t)) : FLASH_ADR_VC_NODE_INFO; // gw_node_info_addr_save set to next unused address
	
	return 0;
}

void rebuild_node_info_sector()
{	
	u32 write_addr = FLASH_ADR_VC_NODE_INFO, read_addr = FLASH_ADR_VC_NODE_INFO;
	int backup_wptr=0;
	VC_node_info_t node_info;
	
	flash_erase_sector(FLASH_ADR_FRIEND_SHIP); // erase backup sector
	for(; read_addr < FLASH_ADR_VC_NODE_INFO_END; read_addr += sizeof(VC_node_info_t)){
		flash_read_page(read_addr, sizeof(VC_node_info_t), (u8 *)&node_info);		
		
		if((read_addr+sizeof(VC_node_info_t))/FLASH_SECTOR_SIZE != read_addr/FLASH_SECTOR_SIZE){ // finish read 1 sector
			int len = 0;
			flash_erase_sector(read_addr/FLASH_SECTOR_SIZE*FLASH_SECTOR_SIZE);
			for(int backup_rptr=0; backup_rptr<backup_wptr; backup_rptr += PAGE_SIZE){ // restore node info from backup sector
				u8 buf[PAGE_SIZE];
				len = ((backup_wptr-backup_rptr)>PAGE_SIZE) ? PAGE_SIZE:(backup_wptr-backup_rptr);
				flash_read_page(FLASH_ADR_FRIEND_SHIP + backup_rptr, len, buf);
				flash_write_page(write_addr, len, buf);
				write_addr += len;
			}
			backup_wptr = 0;
			flash_erase_sector(FLASH_ADR_FRIEND_SHIP);

			if(node_info.node_adr == 0xffff){
				continue;
			}
			
			if(node_info.node_adr && is_unicast_adr(node_info.node_adr)){ // item in node_info is valid
				len = ((write_addr%FLASH_SECTOR_SIZE) + sizeof(VC_node_info_t) > FLASH_SECTOR_SIZE) ? (FLASH_SECTOR_SIZE - (write_addr%FLASH_SECTOR_SIZE)) : sizeof(VC_node_info_t);
				flash_write_page(write_addr, len, (u8 *)&node_info);
				write_addr += len;
				if(len < sizeof(VC_node_info_t)){
					flash_write_page(FLASH_ADR_FRIEND_SHIP, sizeof(VC_node_info_t) - len, (u8 *)&node_info + len);
					backup_wptr = sizeof(VC_node_info_t) - len;
				}
			}
		}
		else{
			if(node_info.node_adr && is_unicast_adr(node_info.node_adr)){
				flash_write_page(FLASH_ADR_FRIEND_SHIP + backup_wptr, sizeof(VC_node_info_t), (u8 *)&node_info);
				backup_wptr += sizeof(VC_node_info_t);
			}
		}
	}

	restore_node_info_table();
}

void clear_gw_node_info(u16 addr, int is_must_primary)
{
	if(ADR_ALL_NODES == addr){
		for (int i = 0; i < (FLASH_ADR_VC_NODE_INFO_END - FLASH_ADR_VC_NODE_INFO) / FLASH_SECTOR_SIZE; i++){
			u32 addr = FLASH_ADR_VC_NODE_INFO + i*0x1000;
			flash_erase_sector(addr);
		}
		gw_node_info_addr_save = FLASH_ADR_VC_NODE_INFO;
		memset((u8 *)(gw_node_info), 0x00, sizeof(gw_node_info));
	}
	else{
		int idx = get_gw_node_info_idx(addr, is_must_primary);
		if(-1 != idx){
			u8 zero[2]={0};
			flash_write_page(FLASH_ADR_VC_NODE_INFO + gw_node_info[idx].index*sizeof(VC_node_info_t), sizeof(zero), zero); // write 0 to set unvalid
			gw_node_info[idx].node_adr = 0;
		}
	}
}

int save_gw_node_info(VC_node_info_t *p_node_info)
{
	int ret = -1;

	if(!p_node_info->node_adr || !is_unicast_adr(p_node_info->node_adr) || gw_node_info_addr_save < FLASH_ADR_VC_NODE_INFO){
		return ret;
	}

	clear_gw_node_info(p_node_info->node_adr, 0);
	if(gw_node_info_addr_save > (FLASH_ADR_VC_NODE_INFO_END - sizeof(VC_node_info_t))){
		if((-1 == get_gw_node_info_idx(p_node_info->node_adr, 1)) && (-1 == find_gw_node_empty_idx())) 
		{
			return ret; // not enough resource
		}
		rebuild_node_info_sector();
	}
	
	int idx = get_gw_node_info_idx(p_node_info->node_adr, 1);
	if(-1 == idx){
		idx = find_gw_node_empty_idx();
	}

	if(-1 != idx){
		if(gw_node_info_addr_save <= (FLASH_ADR_VC_NODE_INFO_END - sizeof(VC_node_info_t))){
			flash_write_page(gw_node_info_addr_save, sizeof(VC_node_info_t), (u8 *)p_node_info);

			gw_node_info[idx].node_adr = p_node_info->node_adr;
			gw_node_info[idx].ele_cnt = p_node_info->element_cnt;
			gw_node_info[idx].index = (gw_node_info_addr_save - FLASH_ADR_VC_NODE_INFO)/sizeof(VC_node_info_t);		
			gw_node_info_addr_save += sizeof(VC_node_info_t);
			ret = 0;
		}
	}

	return ret;
}
#endif


void VC_cmd_clear_all_node_info(u16 adr)
{
#if WIN32
	if(adr == 0xffff){
		provision_mag.unicast_adr_last =1;// reset the adr part 
		provision_mag_cfg_s_store();// avoid restore part 
		memset((u8 *)(VC_node_info),0xff,sizeof(VC_node_info));
	}else if (is_unicast_adr(adr)){
		VC_node_info_t *p_info = get_VC_node_info(adr, 1);
		if(p_info){
            memset(p_info,0xff,sizeof(VC_node_info_t));
		}
	}else{
		return;
	}

	save_vc_node_info_all();
#else
	if(adr == ADR_ALL_NODES){
		provision_mag.unicast_adr_last = ele_adr_primary + g_ele_cnt;// reset the adr part 
		provision_mag_cfg_s_store();// avoid restore part 	
	}
	
	clear_gw_node_info(adr, 1);
#endif
}

VC_node_info_t * get_VC_node_info(u16 obj_adr, int is_must_primary)
{	
#if WIN32
	if(obj_adr && is_unicast_adr(obj_adr)){
        foreach(i,MESH_NODE_MAX_NUM){
            VC_node_info_t *p_info = &VC_node_info[i];
            if(p_info->node_adr){
                if(is_must_primary){
                    if(p_info->node_adr == obj_adr){    
                        return p_info;
                    }
                }else if(is_ele_in_node(obj_adr, p_info->node_adr, p_info->element_cnt)){
                    return p_info;
                }
            }
        }
	}
#else
	int idx = get_gw_node_info_idx(obj_adr, is_must_primary);
	if(-1 != idx){
		flash_read_page(FLASH_ADR_VC_NODE_INFO + gw_node_info[idx].index*sizeof(VC_node_info_t), sizeof(VC_node_info_t), (u8 *)VC_node_info);
		return VC_node_info;
	}
#endif

#if WIN32
    LOG_MSG_INFO(TL_LOG_COMMON,0, 0,"obj_adr 0x%04x, not found in VC node info", obj_adr);
#endif

    return 0;
}

void del_vc_node_info_by_unicast(u16 unicast)
{
#if WIN32
	VC_node_info_t * p_node_info = get_VC_node_info(unicast,1);        
    if(p_node_info!=0){
        // clear the vc node info part 
        memset(p_node_info,0,sizeof(VC_node_info_t));
    }
    save_vc_node_info_all();//save the vc node info part 
#else
	clear_gw_node_info(unicast, 1);
#endif
}

#if IS_VC_PROJECT_MASTER
void erase_vc_node_info()
{
    flash_erase_sector_VC(0, sizeof(VC_node_info));
}

/**
* function: get offset between object addr and element addr which include the model.
*/
u8 get_ele_offset_by_model_VC_node_info(u16 obj_adr, u32 model_id, bool4 sig_model)
{
    VC_node_info_t *p_info = get_VC_node_info(obj_adr, 0);
    if(p_info){
        return get_ele_offset_by_model((mesh_page0_t *)(&p_info->cps.page0_head), p_info->cps.len_cps, p_info->node_adr, obj_adr, model_id, sig_model);
    }
    LOG_MSG_ERR (TL_LOG_COMMON, 0, 0, "VC node info NOT_FOUND!........................",0);
    return MODEL_NOT_FOUND;
}
#endif
#if WIN32
static u32 mesh_vc_node_addr = FLASH_ADR_VC_NODE_INFO;
#endif

void VC_node_info_retrieve()
{
#if WIN32
    int err = mesh_par_retrieve((u8 *)VC_node_info, &mesh_vc_node_addr, FLASH_ADR_VC_NODE_INFO, sizeof(VC_node_info));
#else    
	restore_node_info_table();
#endif
}
 
int VC_node_dev_key_save(u16 adr, u8 *dev_key,u8 ele_cnt)
{
#if WIN32
	if(!is_unicast_adr(adr) || (!adr)){
        return -1;
    }

    VC_node_info_t *p_info = 0;    // save
    VC_node_info_t *p_info_1st_empty = 0;
    foreach(i,MESH_NODE_MAX_NUM){
        VC_node_info_t *p_info_tmp = &VC_node_info[i];
        if(p_info_tmp->node_adr == adr){    
            p_info = p_info_tmp;  // existed
            break;
        }else if((!p_info_1st_empty) && ((0 == p_info_tmp->node_adr)||(p_info_tmp->node_adr >= 0x8000))){
            p_info_1st_empty = p_info_tmp;
        }
    }

    if(0 == p_info){
        p_info = p_info_1st_empty;
    }
    if(p_info){
        if(!((adr == p_info->node_adr)&&(!memcmp(p_info->dev_key, dev_key, 16)))){
            p_info->node_adr = adr;
			p_info->element_cnt = ele_cnt;
            memcpy(p_info->dev_key, dev_key, 16);

            #if DONGLE_PROVISION_EN
            save_vc_node_info_all();
            #else
            save_vc_node_info_single(p_info);
            #endif
        }
        return 0;
    }

    return -1; 	 // full
#else
	VC_node_info_t node_info;
	VC_node_info_t *p_node_info = get_VC_node_info(adr, 1);
	if(0 == p_node_info){
		p_node_info = &node_info;
		memset(&node_info, 0x00, sizeof(node_info));	
	}
	
	p_node_info->node_adr = adr;
	p_node_info->element_cnt = ele_cnt;
	memcpy(p_node_info->dev_key, dev_key, sizeof(node_info.dev_key));
	return save_gw_node_info(p_node_info);
#endif
}
 
u8* VC_master_get_other_node_dev_key(u16 adr)
{
     if(is_actived_factory_test_mode()){
         return mesh_key.dev_key;
     }
     
	 VC_node_info_t *p_info = get_VC_node_info(adr, 1);
	 if(p_info){
	    return p_info->dev_key;
	 }
 
	 return 0;
}

#if (MD_REMOTE_PROV && DEVICE_KEY_REFRESH_ENABLE)
int VC_node_dev_key_candi_enable(u16 adr)
{
	int ret = -1;
	 if(!is_unicast_adr(adr) || (!adr)){
        return ret;
    }
	
	VC_node_info_t *p_node_info = get_VC_node_info(adr, 1);
	if(0 != p_node_info){
		memcpy(p_node_info->dev_key, p_node_info->dev_key_candi, sizeof(p_node_info->dev_key));
		memset(p_node_info->dev_key_candi, 0x00, sizeof(p_node_info->dev_key_candi));
		#if DONGLE_PROVISION_EN
		ret = save_gw_node_info(p_node_info);
		#endif
	}

	return ret;	
}


int VC_node_replace_devkey_candi_adr(u16 adr, u16 new_adr,u8 *dev_key_cadi)
{
	int ret = -1;
    if(!is_unicast_adr(adr) || (!adr)){
        return ret;
    }

	VC_node_info_t *p_node_info = get_VC_node_info(adr, 1);
	if(0 != p_node_info){
		p_node_info->node_adr = new_adr;
		memcpy(p_node_info->dev_key, dev_key_cadi, sizeof(p_node_info->dev_key));
		memset(p_node_info->dev_key_candi, 0x00, sizeof(p_node_info->dev_key_candi));
		#if DONGLE_PROVISION_EN
		ret = save_gw_node_info(p_node_info);
		#endif
	}

    return ret; 	
}
 
 int VC_node_dev_key_save_candi(u16 adr, u8 *dev_key_cadi)
 {
 	int ret = -1;
	if(!is_unicast_adr(adr) || (!adr)){
	 	return ret;
	}

	VC_node_info_t *p_node_info = get_VC_node_info(adr, 1);
	if(0 != p_node_info){
		if(memcmp(p_node_info->dev_key_candi, dev_key_cadi, 16)){
			memcpy(p_node_info->dev_key_candi, dev_key_cadi, sizeof(p_node_info->dev_key_candi));
			#if DONGLE_PROVISION_EN
			ret = save_gw_node_info(p_node_info);
			#endif
		}
	}	

	 return ret;
}
 


u8* VC_master_get_other_node_dev_key_candi(u16 adr)
{
     if(is_actived_factory_test_mode()){
         return mesh_key.dev_key;
     }
     
	 VC_node_info_t *p_info = get_VC_node_info(adr, 1);
	 if(p_info){
	    return p_info->dev_key_candi;
	 }
 
	 return 0;
}
#endif

u8 VC_node_cps_save(mesh_page0_t * p_page0,u16 unicast, u32 len_cps)
{
    VC_node_info_t *p_info = get_VC_node_info(unicast, 1); // have been sure no same node address in node info
    if(p_info){
        #if DONGLE_PROVISION_EN
        VC_node_cps_t *p_cps = &VC_node_cps;
        #else
		VC_node_cps_t *p_cps = &p_info->cps;
		if(unicast == ele_adr_primary){
            p_info->element_cnt = g_ele_cnt;
        }else{
        	u8 cps_ele_cnt =0;
            cps_ele_cnt = get_ele_cnt_by_traversal_cps(p_page0, len_cps);
			if(p_info->element_cnt == 0){
				p_info->element_cnt = cps_ele_cnt;
			}else if (cps_ele_cnt != p_info->element_cnt){
                LOG_MSG_ERR(TL_LOG_PROVISION,0, 0 ,"element count error!cps cnt:%d, capability cnt:%d",cps_ele_cnt, p_info->element_cnt);
    			p_info->element_cnt =0;
    		}
		}
        #endif
        p_cps->len_cps = len_cps;
        memcpy(&p_cps->page0_head, p_page0, len_cps);
        
        #if WIN32
        save_vc_node_info_single(p_info);
        #endif
        
        return 0;
    }
    // to save later
    return -1;
}

#endif

