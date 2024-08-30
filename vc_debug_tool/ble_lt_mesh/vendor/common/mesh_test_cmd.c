/********************************************************************************************************
 * @file	mesh_test_cmd.c
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
#include "proj_lib/sig_mesh/app_mesh.h"
#include "proj_lib/mesh_crypto/mesh_crypto.h"
#include "app_proxy.h"
#include "app_health.h"
#include "mesh_test_cmd.h"

#if 0
#if SEC_MES_DEBUG_EN
mesh_cmd_bear_t B_test_cmd = {
    /*.trans_par_val = */0,
    /*.len =       */0,
    /*.type =     */MESH_ADV_TYPE_MESSAGE,
};

void mesh_message6()
{
    mesh_cmd_bear_t *p_bear = (mesh_cmd_bear_t *)&B_test_cmd;
    if(mesh_adv_tx_cmd_sno < 0x3129ab){
        mesh_adv_tx_cmd_sno = 0x3129ab;
    }
    
    // lower layer
    //p_bear->lt_seg.aid = 0;
    p_bear->lt_seg.akf = 0;
    //p_bear->lt_seg.seg = 1;
    //p_bear->lt_seg.segN = 1;
    //p_bear->lt_seg.segO = 0;
    //p_bear->lt_seg.seqzero = sno;// & BIT_MASK_LEN(13);
    p_bear->lt_seg.szmic = 0;
    
    // network layer
    //p_bear->nw.nid = 0x68;
    //p_bear->nw.ivi = 0;       // ivi is least significant bit
    p_bear->nw.ttl = 4;
    p_bear->nw.ctl = 0;
    #if 0
    p_bear->nw.src = 0x0003;
    p_bear->nw.dst = 0x1201;
    #else
    p_bear->nw.src = ele_adr_primary;
    if(ADR_LPN1 == ele_adr_primary){
        p_bear->nw.dst = ADR_FND2;
    }else{
        p_bear->nw.dst = ADR_LPN1;
    }
    #endif
    
    u8 r_apl[] = {0x02,0x23,0x61,0x45,0x63,0x96,0x47,0x71, 0x73,0x4f,0xbd,0x76,0xe3,0xb4,0x05,0x19,
				  0xd1,0xd9,0x4a,0x48,
				  #if 0 // DEBUG_SEG_RX
				  0x00,0x00,0x00,0x00,0x33,0x33,0x00,0x00,0x00,0x00,0x00,0x00,
				  0x00,0x00,0x00,0x00,0x44,0x44,0x00,0x00,0x00,0x00,0x00,0x00,
				  0x00,0x00,0x00,0x00,0x55,0x55,0x00,0x00,0x00,0x00,0x00,0x00,
				  0x00,0x00,0x00,0x00,0x66,0x66,0x00,0x00,0x00,0x00,0x00,0x00,
				  #endif
				  };

    mesh_match_type_t match_type;
    mesh_match_group_mac(&match_type, p_bear->nw.dst, rf_link_get_op_by_ac(r_apl), 1, ele_adr_primary);

    mesh_tx_cmd_layer_access(r_apl, sizeof(r_apl), ele_adr_primary, p_bear->nw.dst, 0, &match_type);
}
#endif

#if FRI_SAMPLE_EN
void mesh_message_fri_msg()
{
#if 1
    mesh_message_fri_msg_ctl_seg();
    return ;
#endif

#if SEC_MES_DEBUG_EN
    mesh_cmd_bear_t *p_bear = (mesh_cmd_bear_t *)&B_test_cmd;

    // lower layer
    //p_bear->lt_seg.aid = 0;
    p_bear->lt_seg.akf = 0;
    //p_bear->lt_seg.seg = 1;
    //p_bear->lt_seg.segN = 1;
    //p_bear->lt_seg.segO = 0;
    //p_bear->lt_seg.seqzero = sno;// & BIT_MASK_LEN(13);
    p_bear->lt_seg.szmic = 0;
    
    // network layer
    //p_bear->nw.nid = 0x68;
    //p_bear->nw.ivi = 0;       // ivi is least significant bit
    p_bear->nw.ttl = 4;
    p_bear->nw.ctl = 0;

    p_bear->nw.src = ele_adr_primary;
    p_bear->nw.dst = ADR_LPN1;
    
    u8 r_apl[] = {0x02,0x23,0x61,0x45,0x63,0x96,0x47,0x71, 0x73,0x4f,0xbd,
                  #if 1 // segment
                  0x76,0xe3,0xb4,0x05,0x19,0xd1,0xd9,0x4a,0x48,
                  #endif
				  #if 0 // DEBUG_SEG_RX
				  0x00,0x00,0x00,0x00,0x33,0x33,0x00,0x00,0x00,0x00,0x00,0x00,
				  0x00,0x00,0x00,0x00,0x44,0x44,0x00,0x00,0x00,0x00,0x00,0x00,
				  0x00,0x00,0x00,0x00,0x55,0x55,0x00,0x00,0x00,0x00,0x00,0x00,
				  0x00,0x00,0x00,0x00,0x66,0x66,0x00,0x00,0x00,0x00,0x00,0x00,
				  #endif
				  };

    SEG_DEBUG_LED(0);
    mesh_match_type_t match_type;
    mesh_match_group_mac(&match_type, p_bear->nw.dst, rf_link_get_op_by_ac(r_apl), 1, ele_adr_primary);
    
    mesh_tx_cmd_layer_access(r_apl, sizeof(r_apl), ele_adr_primary, p_bear->nw.dst, 0, &match_type);
#endif
}

static const u8 B_test_ctl_par_org[] = {
    0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
    0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,
    0x21,0x22,0x23,0x24,//0x25,0x26,0x27,0x28,
    //0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,
};

void mesh_message_fri_msg_ctl_seg()
{
    static u8 B_test_ctl_par[sizeof(B_test_ctl_par_org)] = {0};
    
    static u8 msg_idx;
    foreach_arr(i,B_test_ctl_par){
        B_test_ctl_par[i] = B_test_ctl_par_org[i] + msg_idx*0x30;
    }

    SEG_DEBUG_LED(0);
    int err = mesh_tx_cmd_layer_upper_ctl_primary(0x3F, B_test_ctl_par, sizeof(B_test_ctl_par), ADR_FND2);

    if(!err){
        if(msg_idx >= 4){
            msg_idx = 0;
        }else{
            msg_idx++;
        }
    }
}
#endif

#if 0
void keyboard_handle_mesh()
{
    u16 adr_dst = ADR_LPN1;
    adr_dst = adr_dst;
#if 1
    if(is_fn_support_and_en){
        mesh_message_fri_msg();
    }else if(0){
        mesh_message6();
        SEG_DEBUG_LED(0);
    }else if(is_lpn_support_and_en){
        // send Poll command after suspend
    }
#elif 0
    mesh_model_cfg_cmd_test();
#else   // on off
    u8 par[11-3] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
    static u8 cnt;
    par[0] = (++cnt) & 1;
    mesh_tx_cmd2normal_primary(VD_GROUP_G_SET, par, 1, adr_dst, 1);
#endif
}
#endif


#if FRI_SAMPLE_EN
void friend_ship_sample_message_test()
{
    mesh_lpn_par.LPNAdr= ADR_LPN1;
    mesh_lpn_par.FriAdr = ADR_FND1;

    // sample data        
    mesh_ctl_fri_req_t *p_req = &mesh_lpn_par.req;
    p_req->Criteria.MinCacheSizeLog = 1; // 3;
    p_req->Criteria.RecWinFac = FRI_REC_WIN_FAC;
    p_req->Criteria.RSSIFac = FRI_REC_RSSI_FAC;
    p_req->RecDelay = FRI_REC_DELAY_MS;
    p_req->PollTimeout = 0x057e40;
    p_req->PreAdr = 0;
    p_req->NumEle = 1;
    p_req->LPNCounter = 0; 
    mesh_ctl_fri_update_t *p_update = &fn_update[0];
    get_iv_big_endian(p_update->IVIndex, (u8 *)&iv_idx_st.iv_tx);

    mesh_ctl_fri_offer_t *p_offer = &mesh_lpn_par.offer;
    p_offer->RecWin = FRI_REC_WIN_MS;
    p_offer->CacheSize = 0x03;
    p_offer->SubsListSize = 0x08;
    p_offer->RSSI = -70;
    p_offer->FriCounter = 0x072f;
    mesh_friend_key_update_all_nk(0);
    friend_cmd_send_request();
}

void friend_cmd_send_sample_message(u8 op)
{
    u16 dev_adr_temp = ele_adr_primary;

    if(CMD_CTL_REQUEST == op){
        ele_adr_primary = ADR_LPN1;
        mesh_adv_tx_cmd_sno = 0x000001;
        friend_cmd_send_request();
    }else if(CMD_CTL_OFFER == op){
        ele_adr_primary = ADR_FND1;
        mesh_adv_tx_cmd_sno = 0x014820;
        friend_cmd_send_offer(0);
    }else if(CMD_CTL_POLL == op){
        ele_adr_primary = ADR_LPN1;
        mesh_adv_tx_cmd_sno = 0x000002;
        friend_cmd_send_poll();
    }else if(CMD_CTL_UPDATE == op){
        ele_adr_primary = ADR_FND1;
        mesh_adv_tx_cmd_sno = 0x014834;
        friend_cmd_send_update(0);
    }
    
    ele_adr_primary = dev_adr_temp;  // restore
}
#endif
#endif

void test_cmd_tdebug()
{
	static volatile u8 A_key = 0,A_key_seg = 1,A_key_virtual = 0;
	static volatile u16 A_key_adr = 0x0001;//0xffff;
	if(A_key){
		A_key = 0;
		static u8 test_onoff;
		u32 len = OFFSETOF(mesh_cmd_g_onoff_set_t,transit_t);	// no delay 
		u8 cmd_buf[32] = {0};
		//memset(cmd_buf, 0xaa, sizeof(cmd_buf));
		mesh_cmd_g_onoff_set_t *cmd = (mesh_cmd_g_onoff_set_t *)cmd_buf;
		cmd->onoff = (test_onoff++) & 1;
		cmd->tid = 0;
		cmd->transit_t = 0;
		cmd->delay = 0;
		if(A_key_seg){
			len += 13;	// test segment;
		}

		if(A_key_virtual){
			mesh_tx_cmd2uuid(G_ONOFF_SET_NOACK, (u8 *)cmd, len, ele_adr_primary, 0, 0, prov_para.device_uuid);
		}else{
			mesh_tx_cmd2normal_primary(G_ONOFF_SET_NOACK, (u8 *)cmd, len, A_key_adr, 0);
		}
	}
}

#if 0
void power_on_io_proc(u8 i)
{
#if !WIN32
#define GPIO_DEBUG 	GPIO_PB4
	gpio_set_func(GPIO_DEBUG,AS_GPIO);
	gpio_set_output_en(GPIO_DEBUG,1);
	while(i){
		gpio_write(GPIO_DEBUG,1);
		NOP_TEST_BYTE_100;
		gpio_write(GPIO_DEBUG,0);
		NOP_TEST_BYTE_100;
		i--;
	}
#endif
}
#endif

#if IV_UPDATE_TEST_EN
void mesh_iv_update_test_initiate(u8 key_code)
{
	if(KEY_SW2 == key_code){ // dispatch just when you press the button 
		if(IV_UPDATE_NORMAL == iv_idx_st.update_proc_flag){
			#if 1
			if(mesh_adv_tx_cmd_sno < IV_UPDATE_START_SNO){
				mesh_adv_tx_cmd_sno = IV_UPDATE_START_SNO;
			}
	        LOG_MSG_LIB(TL_LOG_IV_UPDATE,(u8 *)&iv_idx_st, sizeof(iv_idx_st),"SW:trigger to step1 later,current iv is: ");
			#else
			mesh_increase_ivi(iv_idx_st.cur);
	        mesh_iv_update_set_start_flag(1);   // should be keep search flag when trigger by self after power up.
	        LOG_MSG_INFO(TL_LOG_IV_UPDATE,(u8 *)&iv_idx_st, sizeof(iv_idx_st),"SW:IV index enter step1: ");
	        #endif
		}
		else{
			LOG_MSG_INFO(TL_LOG_IV_UPDATE,(u8 *)&iv_idx_st, sizeof(iv_idx_st),"IV is in step:%d! ",iv_idx_st.update_proc_flag);
		}
		cfg_led_event(LED_EVENT_FLASH_1HZ_1S);
	}
	
	if(KEY_SW1 == key_code){ // dispatch just when you press the button 
		LOG_MSG_INFO(TL_LOG_IV_UPDATE,(u8 *)&iv_idx_st, sizeof(iv_idx_st),"printf IV:step:%d! ",iv_idx_st.update_proc_flag);
	}
}
#endif

#if DEBUG_PUBLISH_REDUCE_COLLISION_TEST_EN
#define PUB_TEST_DEBUG_LOG_EN				1	
#error 111111111
typedef struct{
	u8 onoff;
	u8 tid;
	u8 transit_t;
	u8 delay;		// unit 5ms
	u32 noack_num;
	u32 system_time_s;
}mesh_cmd_g_onoff_set_test_t;

#define GW_RSP_ACK_NUM	60
typedef struct{
	u16 adr_dst;
	u32 gw_rx_system_time_s;
	u32 rx_code_num; 	// rx poll number of node
	u32 rx_node_num;	// rx count
	u16 tx_noack_num;	// gateway tx ack
}mesh_cmd_g_onoff_set_test_t_log;
mesh_cmd_g_onoff_set_test_t_log mesh_node_test_log[GW_RSP_ACK_NUM];

/**
 * @brief       This function test the success rate of gw receiving a period publish and send an ack
 * @param[in]   adr_dst				- destination address
 * @param[in]   rsp_max				- max number of response expected from nodes which match destination address, after sending the opcode. 
 * @param[in]   onoff				- on or off
 * @param[in]   ack					- select G_ONOFF_SET or G_ONOFF_SET_NOACK
 * @param[in]   trs_par				- parameter of transition time and delay. NULL mean no parameter.
 * @param[in]   roll_code			- Packet sending times of a node
 * @param[in]   gw_rx_system_s		- time when gw receives packets from a node
 * @param[in]   node_rsp_rx_ack_num	- number of gw ack packets received by the node
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int access_cmd_onoff_with_pub_test_result(u16 adr_dst, u8 rsp_max, u8 onoff, int ack, transition_par_t *trs_par, u32 roll_code, u32 gw_rx_system_s, int node_rsp_rx_ack_num)
{
	mesh_cmd_g_onoff_set_test_t par = {0};
	u32 par_len = OFFSETOF(mesh_cmd_g_onoff_set_test_t,transit_t);
	par.onoff = onoff;
	//if(trs_par){
		par_len = sizeof(mesh_cmd_g_onoff_set_test_t);
		//memcpy(&par.transit_t, trs_par, 2);
	//}
	int i = 0;
	for(i = 0; i < GW_RSP_ACK_NUM; i++){
		if(mesh_node_test_log[i].adr_dst == adr_dst){		
			break;
		}else if(0 == mesh_node_test_log[i].adr_dst){
			mesh_node_test_log[i].adr_dst = adr_dst;
			break;
		}
	}

	int cycle_time = 0;
	int total_success = 0;

	if(i < GW_RSP_ACK_NUM){
		mesh_node_test_log[i].rx_code_num = roll_code;
		mesh_node_test_log[i].rx_node_num++;
		par.noack_num = ++mesh_node_test_log[i].tx_noack_num;
		cycle_time = gw_rx_system_s - mesh_node_test_log[i].gw_rx_system_time_s;
		mesh_node_test_log[i].gw_rx_system_time_s = gw_rx_system_s;
		if(mesh_node_test_log[i].rx_code_num <= 1){
			total_success = 0;
		}else{
			total_success = 5 * (node_rsp_rx_ack_num * 100) / (mesh_node_test_log[i].rx_code_num - 1); // add 1,because roll code status of the next time
		}
		
		if(mesh_node_test_log[i].rx_node_num > mesh_node_test_log[i].rx_code_num){
			mesh_node_test_log[i].rx_node_num = mesh_node_test_log[i].rx_code_num; // The node may lose power, and the number of roll codes sent is smaller than the number of nodes received by gw
		}
		
		par.delay = 0;
		par.transit_t = 0;
		par.system_time_s = system_time_s;
			if((mesh_node_test_log[i].rx_code_num % 5) == 1){
			#if PUB_TEST_DEBUG_LOG_EN
			LOG_USER_MSG_INFO(0, 0, "node adr:0x%04x, rx status sno:%3d, rx status cnt:%3d, success1: %3d, gw tx ack sno:%3d, system_time_s: %3d, pub period:%2d, node rx gw ack cnt:%3d, success2: %3d", adr_dst, mesh_node_test_log[i].rx_code_num, mesh_node_test_log[i].rx_node_num, (mesh_node_test_log[i].rx_node_num*100)/mesh_node_test_log[i].rx_code_num, mesh_node_test_log[i].tx_noack_num, system_time_s, cycle_time, node_rsp_rx_ack_num, total_success);
			#endif
			
			return SendOpParaDebug(adr_dst, rsp_max, ack ? G_ONOFF_SET : G_ONOFF_SET_NOACK, (u8 *)&par, par_len);
		}
	}

	return 0;
}
#endif

