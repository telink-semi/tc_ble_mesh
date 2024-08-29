/********************************************************************************************************
 * @file	mesh_network_layer.c
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
#include "mesh_network_layer.h"
#include "proj_lib/sig_mesh/app_mesh.h"

// ---- message relay protect list process
/**
 * @brief       This function servers to init item in RPL.
 * @param[in]   ele_adr	- element address in RPL. if it's a unicast address, just init the corresponding item, otherwise init all items.
 * @return      none
 * @note        
 */
void cache_init(u16 ele_adr)
{
    int del_all = !is_unicast_adr(ele_adr);
    if(del_all){
        mesh_network_cache_buf_init();
    }else{
        foreach(i, g_cache_buf_max){
            if(ele_adr == cache_buf[i].src){
                memset(&cache_buf[i], 0, sizeof(cache_buf[i]));
                break;
            }
        }
    }
	
	if((mesh_rx_seg_par.src != 0) &&  (0 == mesh_rx_seg_par.tick_last)){
		mesh_seg_rx_init();
	}
}


/**
 * @brief       This function servers to update RPL.
 * @param[in]   p	- pointer of network PDU.
 * @param[in]   idx	- index of item in RPL.
 * @return      none
 * @note        
 */
void mesh_update_rpl(u8 *p, int idx)
{    // can not use op code,because it is encryption now
    mesh_cmd_nw_t *p_nw = (mesh_cmd_nw_t *)p;

    //memset(&cache_buf[idx], 0, sizeof(cache_buf_t));    // init
    cache_buf[idx].src = p_nw->src;
    cache_buf[idx].ivi = (u8)iv_idx_st.iv_rx;
    u32 sno = 0;
    memcpy(&sno, p_nw->sno, SIZE_SNO);
    cache_buf[idx].sno = sno;
}

/**
 * @brief       This function servers to handle message against replay attacks and update replay protection list.
 * @param[in]   p				- pointer of network PDU.
 * @param[in]   friend_key_flag	- 0: the PDU is not decrypted by friend key. 1: the PDU is decrypted by friend key. 
 * @param[in]   save			- 0: not need to update RPL. 1: need to update RPL.
 * @return      1: the network PDU will be discarded since SEQ is old. 0: the network PDU will will not be discarded.
 * @note        if parameter "save" is 0, means the network PDU was just handled by network layer obfuscation, we can discard the PDU in advance because the source address and the sequence number of the network PDU are known after obfuscation.
 *				if parameter "save" is 1, means the network PDU has been successfully decrypted, need update RPL. 
 */
int is_exist_in_cache(u8 *p, u8 friend_key_flag, int save)
{
#define MESH_SR_PROX_BVXY3C 	0

#if MESH_MONITOR_EN
    if(!monitor_filter_sno_en){
        return 0;
    }
#endif

    mesh_cmd_nw_t *p_nw = (mesh_cmd_nw_t *)p;   // only ivi, src and sno can be use when "save" is 0.
    int exist_old = 0;

	foreach(i, g_cache_buf_max){
        if(p_nw->src == cache_buf[i].src){
            u32 sno = 0;
            memcpy(&sno, p_nw->sno, SIZE_SNO);
            #if FEATURE_LOWPOWER_EN
			static u32 lpn_skip_cache_cnt;
			#endif
			
			if(((u8)iv_idx_st.iv_rx) != cache_buf[i].ivi){
			    if(save){ // decryption ok
			        if(iv_idx_st.iv_rx != iv_idx_st.iv_cur){ // means ivi is less 1 than current ivi
			        // this case happen means ivi of cache is new, iv rx is old, which may happen when other nodes relay with old iv.
			        //such as some nodes may relay with small iv but not original iv in raw packet. for example, both cache and current have been 1, and node A tx command with 1 but this command may be relay with 0 and then cause this case.
			        	#if MESH_SR_PROX_BVXY3C // TODO: just for private beacon proxy bv-03c which haiwei think case is not reasonable. //TESTCASE_FLAG_ENABLE
						// private beacon proxy bv-03c, it need to relay by the old ivi.
						LOG_MSG_INFO(TL_LOG_MESH,0, 0 ,"is_exist_in_cache1 ivi %d",p_nw->ivi);
						// should only relay, but now it will both relay and handle. fix later.
						// but if relay without saving the sno, it may case sending too many relay packet.
						return 0;
						#else
			            return 1; // exist_old = 1;
						#endif
			        }else{
			        	// will call add2cache_() to clear and update cache. 
			        }
			    }
			}else if(sno <= cache_buf[i].sno){ // && (iv_idx_st.rx[3] == cache_buf[i].ivi)){
            	#if FEATURE_LOWPOWER_EN
            	int lpn_seg_cache_flag = is_in_mesh_friend_st_lpn() && friend_key_flag;
            	if((0 == save) && lpn_seg_cache_flag){
            	    return 0;   // if "save" is 0, means "lt" is still in encryption state.
            	}
            	
                mesh_cmd_bear_t *p_bear = CONTAINER_OF(p_nw,mesh_cmd_bear_t,nw);
                if((save && p_bear->lt_unseg.seg) && lpn_seg_cache_flag){
                	if(sno == cache_buf[i].sno){// because Friend send cache message if from the last one.
                    	if(lpn_skip_cache_cnt++ < 16){
                    		exist_old = 1;
                    	}
                    }
                }else
                #endif
                {
                    if((!factory_test_cache_not_equal_mode_en) || !is_activated_factory_test_mode() || (sno == cache_buf[i].sno)){
                	    exist_old = 1;
                	}
                }
            }
            
            if(!exist_old && save){
                #if FEATURE_LOWPOWER_EN
            	lpn_skip_cache_cnt = 0;		// init
            	#endif
                mesh_update_rpl(p, i);            // update
            }
            
            return exist_old;
        }
    }

    if(save){
        //new device found
        mesh_add_rpl(p);
    }
    return 0;
}

/**
 * @brief       This function servers to get the number of items in RPL.
 * @return      the number of items in RPL.
 * @note        
 */
u16 get_mesh_current_cache_num() // Note, there may be several elements in a node, but there is often only one element that is in cache.
{
    u16 num = 0;
    foreach(i, g_cache_buf_max){
        if(cache_buf[i].src != 0){
            num++;
        }
   }
    return num;
}
