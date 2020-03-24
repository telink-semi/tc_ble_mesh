/********************************************************************************************************
 * @file     app_mesh.c 
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
#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../vendor/common/user_config.h"
#include "app_mesh.h"
#include "../../vendor/common/app_proxy.h"
#include "../../vendor/common/app_health.h"
#include "../../vendor/common/app_heartbeat.h"

#include "../../proj_lib/mesh_crypto/mesh_crypto.h"
#include "../../proj_lib/pm.h"
#include "../../vendor/common/ev.h"
#include "../../vendor/common/mesh_ota.h"
#include "../../vendor/common/generic_model.h"
#include "../../vendor/common/mesh_common.h"
#include "../../vendor/common/sensors_model.h"
#include "../../vendor/common/remote_prov.h"
#include "../../vendor/common/fast_provision_model.h"

#if WIN32 
#include "../../../reference/tl_bulk/lib_file/gatt_provision.h"


extern u32 VC_tick_next_segment;

u8 reg_simu_buffer[0x1000];

#if 0 // (DEBUG_SHOW_VC_SELF_EN)
#undef DEBUG_SHOW_VC_SELF_EN
#define DEBUG_SHOW_VC_SELF_EN			1
#endif

#endif

#define ONLINE_ST_LIB_EN    ((!WIN32) && (!FEATURE_LOWPOWER_EN))

u8  tbl_mac [6] = {0xe1, 0xe1, 0xe2, 0xe3, 0xcd, 0xab};
const u8	const_tbl_advData[22] = {
 0x08, 0x09, 'S', 'i', 'g', 'M','e','s','h',
 0x02, 0x01, 0x05,							// BLE limited discoverable mode and BR/EDR not supported
 0x09, 0xff, 0x11, 0x02, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
};
const u8	const_tbl_scanRsp [9] = {
	 0x08, 0x09, 'S', 'i', 'g', 'M','e','s','h'	//scan name " sigmesh"
};

//STATIC_ASSERT(sizeof(mesh_cmd_bear_unseg_t) % 4 == 0);
//STATIC_ASSERT(sizeof(mesh_cmd_bear_seg_t) % 4 == 0);
STATIC_ASSERT(sizeof(mesh_cmd_bear_unseg_t) == sizeof(mesh_cmd_bear_seg_t));

#if FEATURE_FRIEND_EN
MYFIFO_INIT(mesh_adv_fifo_fn2lpn, sizeof(mesh_cmd_bear_unseg_t), 4);
#endif
u8 use_mesh_adv_fifo_fn2lpn = 0;

u8 mesh_cmd_ut_tx_unseg[ACCESS_WITH_MIC_LEN_MAX_UNSEG];
u8 mesh_cmd_ut_tx_seg[ACCESS_WITH_MIC_LEN_MAX];
u8 mesh_cmd_ut_rx_unseg[ACCESS_WITH_MIC_LEN_MAX_UNSEG];
u8 mesh_cmd_ut_rx_seg[ACCESS_WITH_MIC_LEN_MAX];
u8 irq_ev_one_pkt_completed;
u16 app_adr = 0;

#if 0
rf_packet_adv_t	mesh_ble_adv = {
    //.dma_len = sizeof(rf_packet_adv_t) - 4;
    //.header.type = LL_TYPE_ADV_IND;
    //.rf_len = sizeof(rf_packet_adv_t) - 6;
};
#endif

u32 mesh_adv_tx_cmd_sno = 1;

mesh_tx_seg_par_t mesh_tx_seg_par;
mesh_rx_seg_par_t mesh_rx_seg_par;
mesh_cmd_bear_unseg_t cmd_bear;
mesh_cmd_bear_seg_t cmd_bear_seg_buf; // only for repeate segment message
typedef struct{
	u16 num;
	u16 addr[10];
}mesh_node_retry_st_t;

#if VC_APP_ENABLE
mesh_node_retry_st_t mesh_node_retry;
#endif

int is_unicast_adr(u16 adr) // include 0, don't modify
{
    return (!(adr & 0x8000));
}

int is_virtual_adr(u16 adr)
{
    return (0x8000 == (adr & 0xc000));
}

int is_group_adr(u16 adr)
{
    return (0xc000 == (adr & 0xc000));
}


/*
diffrent between my_fifo_push and my_fifo_push_adv:

my_fifo_push:          no len in data, add len when push.
my_fifo_push_adv: have len in data,  just use for mesh_cmd_bear.
*/
int my_fifo_push_adv (my_fifo_t *f, u8 *p, u8 n, u8 ow)    // ow: over_write
{
	if (n > f->size)
	{
		return -1;
	}
	
	if (((f->wptr - f->rptr) & 255) >= f->num)
	{
	    if(ow){
	        my_fifo_pop(f);
	    }else{
		    return -1;
		}
	}
	
	u8 r = irq_disable();
	u8 *pd = f->p + (f->wptr & (f->num-1)) * f->size;
	//*pd++ = n;
	//*pd++ = n >> 8;
	memcpy (pd, p, n);
	f->wptr++;			// should be last for VC
	irq_restore(r);
	return 0;
}

STATIC_ASSERT(sizeof(mesh_relay_buf_t) % 4 == 0);

int my_fifo_push_relay (my_fifo_t *f, mesh_cmd_bear_unseg_t *p_in, u8 n, u8 ow)    // ow: over_write
{
	if (n > f->size){
		return -1;
	}
	
	if (((f->wptr - f->rptr) & 255) >= f->num)
	{
	    if(ow){
	        my_fifo_pop(f);
	    }else{
		    return -1;
		}
	}

    u8 r = irq_disable();
	mesh_relay_buf_t *pd = (mesh_relay_buf_t *)(f->p + (f->wptr & (f->num-1)) * f->size);
    memcpy (&pd->bear, p_in, n);
    pd->cnt = transmit_cnt_get(p_in->trans_par_val);
    pd->tick_10ms = transmit_invl_step_get_10ms(p_in->trans_par_val);
	f->wptr++;			// should be last for VC
#if WIN32
    LOG_MSG_INFO(TL_LOG_NODE_BASIC, (u8 *)pd, OFFSETOF(mesh_relay_buf_t,bear)+n, "Relay buff push:", 0);
#endif
    irq_restore(r);
    
    return 0;
}

static inline int is_relay_transmit_ready(mesh_relay_buf_t *p_relay)
{
    return (p_relay->tick_10ms == transmit_invl_step_get_10ms(p_relay->bear.trans_par_val));
}

mesh_relay_buf_t * my_fifo_get_relay(my_fifo_t *f)
{
    mesh_relay_buf_t *p_relay = 0;
    
	u8 r = irq_disable();
	for (u8 i = 0; (u8)(f->rptr + i) != f->wptr; i++)
	{
		mesh_relay_buf_t *p_temp = (mesh_relay_buf_t *)(f->p + ((f->rptr + i) & (f->num-1)) * f->size);
        if(is_relay_transmit_ready(p_temp)){
            p_relay = p_temp;
            break;
        }else{
            #if WIN32
            if(0 == i){
                if(0 == p_temp->valid){
                    // no pop here, and pop in poll proccess in next cycle later.
                    // should not happen here, unless 2 continuous pop action was delay.because only one pop for each cycle now.
                    LOG_MSG_INFO(TL_LOG_NODE_BASIC, 0, 0, "Relay buffer warning, 2 continuous pop action was delay", 0);
                    //static u16 relay_buf_err_cnt;relay_buf_err_cnt++;
                }
            }
            #endif
        }
	}
	irq_restore(r);
	
	return p_relay;
}

void my_fifo_poll_relay(my_fifo_t *f)
{
	u8 r = irq_disable();
	if(f->rptr != f->wptr){
        int pop_flag = 0;
    	for (u8 i = 0; (u8)(f->rptr + i) != f->wptr; i++)
    	{
    		mesh_relay_buf_t *p_relay = (mesh_relay_buf_t *)(f->p + ((f->rptr + i) & (f->num-1)) * f->size);
            if(0 == p_relay->tick_10ms){
                if(p_relay->cnt){
                    p_relay->cnt--;
                    p_relay->tick_10ms = transmit_invl_step_get_10ms(p_relay->bear.trans_par_val);
                }else{
                    if(0 == i){
                        pop_flag = 1;   // happen when relay transmit parameter change.
                    }
                }
            }else{
                // can't minus 1 when ready, because there may be several relay ready at the same time, but just only one can be send at this cycle.
                if(!is_relay_transmit_ready(p_relay)){
                    p_relay->tick_10ms--;
                }
            }
    	}

    	if(pop_flag){
            f->rptr++;
            #if WIN32
            LOG_MSG_INFO(TL_LOG_COMMON, 0, 0, "Relay buffer pop in poll: was delay", 0);
            #endif
    	}
	}
	irq_restore(r);
}

void mesh_tx_cmd_status_report(int err, u8 *p_ini, u32 len)
{
#if IS_VC_PROJECT
    u8 report[400];
    if(len > sizeof(report)-1){
        len = sizeof(report)-1;
    }
    report[0] = MESH_TX_CMD_RUN_STATUS;
    if(p_ini && len){
        memcpy(report + 1, p_ini, len);
    }
	OnAppendLog_vs(report, len + 1);
#else
	#if DEBUG_MESH_DONGLE_IN_VC_EN
	u8 head[1] = {MESH_TX_CMD_RUN_STATUS};
	my_fifo_push_hci_tx_fifo((u8 *)&err, sizeof(err), head, 1);
	#endif
#endif
}

int mesh_construct_adv_bear_com(u8 *bear, u8 *nw, u8 len_nw, u8 type)
{
	mesh_cmd_bear_unseg_t *p_bear = (mesh_cmd_bear_unseg_t *)bear;
	p_bear->trans_par_val = 0;
	p_bear->len = len_nw + 1;
	p_bear->type = type;
	memcpy(&p_bear->nw, nw, len_nw);

	return mesh_bear_len_get(p_bear);
}

int mesh_construct_adv_bear_with_nw(u8 *bear, u8 *nw, u8 len_nw)
{
	return mesh_construct_adv_bear_com(bear, nw, len_nw, MESH_ADV_TYPE_MESSAGE);
}

int mesh_construct_adv_bear_with_bc(u8 *bear, u8 *nw, u8 len_nw)
{
	return mesh_construct_adv_bear_com(bear, nw, len_nw, MESH_ADV_TYPE_BEACON);
}

extern st_ll_conn_slave_t		bltc;
u32 get_blt_conn_interval_us()
{
#if WIN32
    return 50*1000;
#else
    return (bltc.conn_interval / sys_tick_per_us);
#endif
}

int mesh_bear_tx2mesh(u8 *bear, u8 trans_par_val)
{
	int err = 0;
	#if (DEBUG_MESH_DONGLE_IN_VC_EN)
	#if IS_VC_PROJECT
	err = mesh_tx_cmd_layer_bear((u8 *)bear, trans_par_val);
	#endif
	#else
	LOG_MSG_INFO(TL_LOG_MESH,0, 0,"mesh_bear_tx2mesh:enter",0);
	err = mesh_tx_cmd_layer_bear((u8 *)bear, trans_par_val);
	#endif

	return err;
}

#if (!IS_VC_PROJECT || DEBUG_MESH_DONGLE_IN_VC_EN)
int mesh_bear_tx2gatt(u8 *bear,u8 adv_type)
{
	int err = 0;
	#if (DEBUG_MESH_DONGLE_IN_VC_EN)
	#if IS_VC_PROJECT
	err = debug_mesh_vc_adv_report2gatt((u8 *)bear);
	#endif
	#else
	err = mesh_proxy_adv2gatt((u8 *)bear, adv_type);
	#endif

	return err;
}

#if (DEBUG_MESH_DONGLE_IN_VC_EN || MESH_MONITOR_EN)
int mesh_dongle_adv_report2vc(u8 *p_payload, u8 report_cmd)
{
	u8 len_payload = p_payload[0]+1;
	u8 head[1] = {0};
	head[0] = (report_cmd | TSCRIPT_MESH_RX); // report type
	return my_fifo_push_hci_tx_fifo(p_payload, len_payload, head, 1);
}
#endif

#if DEBUG_MESH_DONGLE_IN_VC_EN
int debug_mesh_vc_adv_report2gatt(u8 *bear)
{
	mesh_cmd_bear_unseg_t *p_bear = (mesh_cmd_bear_unseg_t *)bear;
	p_bear->type = MESH_ADV_TYPE_PROXY_FLAG;
	return mesh_tx_cmd_add_packet((u8 *)p_bear);
}

int debug_mesh_dongle_adv_bear2usb(u8 *bear)
{
	mesh_cmd_bear_unseg_t *p_bear = (mesh_cmd_bear_unseg_t *)bear;
	u8 len_bear = p_bear->len + 2;
	u8 head[1] = {0};
	head[0] = (MESH_ADV_BEAR_GATT | TSCRIPT_MESH_RX); // report type

	return my_fifo_push_hci_tx_fifo((u8 *)p_bear, len_bear, head, 1);
}

int debug_mesh_report_provision_par2usb(u8 *net_info)
{
	provison_net_info_str *prov_par = (provison_net_info_str *)net_info;
	u8 head[1] = {MESH_PROV | TSCRIPT_MESH_RX}; // report type
	my_fifo_push_hci_tx_fifo((u8 *)prov_par, sizeof(provison_net_info_str), head, 1);

	return 0;
}

int debug_mesh_report_BLE_st2usb(u8 connect)
{
	u8 head[16];
	head[0] = MESH_ADV_BLE_ST;
	head[1] = connect;
	
	return my_fifo_push_hci_tx_fifo(0,0,(u8 *)head, 2);
}

int debug_mesh_report_one_pkt_completed()
{
	u8 head[16];
	head[0] = MESH_ADV_ONE_PKT_COMPLETED;
	return my_fifo_push_hci_tx_fifo(0,0,(u8 *)head, 1);
}
#endif

#endif 


int mesh_bear_tx2mesh_and_gatt(u8 *bear,u8 adv_type, u8 trans_par_val)
{
	int err = mesh_bear_tx2mesh(bear, trans_par_val);
	#if (!IS_VC_PROJECT || DEBUG_MESH_DONGLE_IN_VC_EN)
	if(is_proxy_support_and_en){
		err |= mesh_bear_tx2gatt(bear, adv_type);
	}
	#endif

	return err;
}
u8 misc_flag =0;
void mesh_increase_sno(int err)
{
    if(0 == err){
		if(misc_flag){
			misc_flag =0;
			mesh_misc_store();
		}
        mesh_adv_tx_cmd_sno++;
    }
}

u8* get_adv_cmd()
{
    return my_fifo_get(&mesh_adv_cmd_fifo);
}

static inline u8 mesh_lt_len_get_by_bear (mesh_cmd_bear_unseg_t *p_br)
{
    mesh_cmd_nw_t *p_nw = &p_br->nw;
    int len_nw = mesh_nw_len_get_by_bear(p_br);
    return (len_nw - OFFSETOF(mesh_cmd_nw_t, data)- (p_nw->ctl ? SZMIC_NW64 : SZMIC_NW32));
}

inline u8 mesh_bear_len_get_by_seg_ut (u8 len_ut, u8 ctl)
{
    return (1 + OFFSETOF(mesh_cmd_nw_t, data) + OFFSETOF(mesh_cmd_lt_seg_t, data)
            + len_ut + (ctl ? SZMIC_NW64 : SZMIC_NW32));
}

u8 mesh_subsc_adr_cnt_get (mesh_cmd_bear_unseg_t *p_br)
{
    u32 len_lt = mesh_lt_len_get_by_bear(p_br);
    return (len_lt - 1)/2;
}

inline u32 mesh_max_payload_get (u32 ctl)
{
    mesh_cmd_lt_ctl_seg_t *p_lt_ctl_seg = NULL;
    mesh_cmd_lt_seg_t *p_lt_seg = NULL;
    return (ctl ? sizeof(p_lt_ctl_seg->data) : sizeof(p_lt_seg->data));
}

inline u32 mesh_msg_cnt_get (u32 len_ut, u32 ctl)
{
    u32 max_size = mesh_max_payload_get(ctl);
    return (len_ut + max_size - 1)/max_size;
}

int mesh_tx_cmd_add_packet(u8 *p_bear)
{
    #if FEATURE_FRIEND_EN
    if(use_mesh_adv_fifo_fn2lpn){
        return mesh_tx_cmd_add_packet_fn2lpn(p_bear);
    }else
    #endif
    {
        mesh_cmd_bear_unseg_t *p = (mesh_cmd_bear_unseg_t *)p_bear;
    	LOG_MSG_INFO(TL_LOG_MESH,0, 0,"mesh_tx_cmd_add_packet:push the adv cmd into fifo",0);
        return my_fifo_push_adv(&mesh_adv_cmd_fifo, p_bear, mesh_bear_len_get(p), 0);
    }
}

#if FEATURE_FRIEND_EN
u8 fn2lpn_no_retransmit = 0;
int mesh_tx_cmd_add_packet_fn2lpn(u8 *p_bear)
{
    #if WIN32	// WIN32 can't use mesh_adv_fifo_fn2lpn, because not call app_advertise_prepare_handler_
    use_mesh_adv_fifo_fn2lpn = 0;
    return mesh_tx_cmd_add_packet(p_bear);
    #else
    mesh_cmd_bear_unseg_t *p = (mesh_cmd_bear_unseg_t *)p_bear;
    if(pts_test_en || fn2lpn_no_retransmit){
	    p->trans_par_val = 0x10;    // FN-BV16 / BV19 require no more than NW PDU. comfirm later.
	}
	LOG_MSG_INFO(TL_LOG_MESH,0, 0,"mesh_tx_cmd_add_packet_fn2lpn",0);
    int err = my_fifo_push_adv(&mesh_adv_fifo_fn2lpn, p_bear, mesh_bear_len_get(p), 0);
    lpn_quick_send_adv();
    return err;
    #endif
}
#endif

int mesh_tx_cmd_layer_bear(u8 *p_bear, u8 trans_par_val)
{
    LAYER_DATA_DEBUG(A_buf4_nw, p_bear, sizeof(mesh_cmd_bear_unseg_t));
    mesh_cmd_bear_unseg_t *p_br = (mesh_cmd_bear_unseg_t *)p_bear;
    
    // fifo packet
    p_br->trans_par_val = trans_par_val;
    LAYER_DATA_DEBUG(A_buf5_br, p_bear, sizeof(mesh_cmd_bear_unseg_t));
	
	LOG_MSG_INFO(TL_LOG_MESH,0, 0,"mesh_tx_cmd_layer_bear:mesh_tx_cmd_add_packet",0);
	return mesh_tx_cmd_add_packet(p_bear);
}

// cache data should be not encryption. p_bear should have been big endianness
void mesh_friend_logmsg(mesh_cmd_bear_unseg_t *p_bear_big, u8 len)
{
	#if (DEBUG_PROXY_FRIEND_SHIP && WIN32)
	extern u8 log_en_lpn1,log_en_lpn2;
	u16 adr_dst = p_bear_big->nw.dst;
	endianness_swap_u16((u8 *)&adr_dst);
	if((log_en_lpn1 && (adr_dst == (PROXY_FRIEND_SHIP_MAC_LPN1 & 0xff)))
	 || (log_en_lpn2 && (adr_dst == (PROXY_FRIEND_SHIP_MAC_LPN2 & 0xff)))){
		LOG_MSG_WARN(TL_LOG_FRIEND,(u8 *)p_bear_big, len,"mesh_friend_logmsg:send message to LPN_win32: \r\n",0);
	}
	#else
	LOG_MSG_INFO(TL_LOG_FRIEND,(u8 *)p_bear_big, len,"mesh_friend_logmsg:send message to LPN: \r\n",0);
	#endif
}

mesh_fri_ship_other_t * mesh_fri_cmd2cache(u8 *p_bear_big, u8 len_nw, u8 adv_type, u8 trans_par_val, u16 F2L_bit)
{
    int err = 0;
    mesh_cmd_bear_unseg_t *p_br_big = (mesh_cmd_bear_unseg_t *)p_bear_big;
    mesh_fri_ship_other_t *p_other = 0;
    
    // fifo packet
    p_br_big->trans_par_val = trans_par_val;
    p_br_big->len = len_nw + 1;
    p_br_big->type = adv_type;

    foreach(i,g_max_lpn_num){
    	if(F2L_bit & BIT(i)){
    	    p_other = &fn_other_par[i];
		    if(!p_other->p_cache){
		        while_1_test;
		    }
		    
		    // big endianness and not encryption in FIFO
		    int replace = friend_cache_check_replace(i, p_br_big);
		    if(0 == replace){
		        err = my_fifo_push_adv(p_other->p_cache, (u8 *)p_br_big, mesh_bear_len_get(p_br_big), 1);
		        if(err){
		            p_other = 0;
		        }
		    }
		    break;
	    }
    }
    return p_other;
}

int is_pkt_notify_only(u16 dst_adr)
{
    return ((app_adr == dst_adr) && is_proxy_support_and_en);
}

int mesh_tx_cmd_layer_network(u8 *bear, u8 len_lt, u8 swap_type_lt, mesh_match_type_t *p_match_type)
{
    int err = 0;
    LAYER_DATA_DEBUG(A_buf3_lt_unseg, p_bear, sizeof(mesh_cmd_bear_unseg_t));
    mesh_cmd_bear_unseg_t *p_bear = (mesh_cmd_bear_unseg_t *)bear;
    mesh_cmd_nw_t *p_nw = &p_bear->nw;
    u8 len_nw = OFFSETOF(mesh_cmd_nw_t, data) + len_lt + (p_nw->ctl ? SZMIC_NW64 : SZMIC_NW32);
	int filter_cfg = p_match_type->filter_cfg;
	u16 dst_adr = p_nw->dst;
	
    #if 0
    mesh_cmd_lt_seg_t *p_lt_seg = &((mesh_cmd_bear_seg_t *)p_bear)->lt;
    if(ADR_UNASSIGNED == p_nw->dst){
        if(p_lt_seg->seg){
            mesh_tx_seg_par.busy = 0;
        }
        return 0;   // no need to send
    }
    #endif

    int use_friend_key = 0, sent_cmd_flag = 0, cache_flag = 0;
    if(is_lpn_support_and_en){
    	int credential_flag = 0;
    	if(p_match_type->model){
    		model_common_t *p_model = (model_common_t *)(p_match_type->model);
    		credential_flag = p_model->pub_par.credential_flag;
    	}
        use_friend_key = is_use_friend_key_lpn(p_nw, credential_flag);

		#if FEATURE_LOWPOWER_EN
        if(!p_nw->ctl){
        	p_match_type->trans.count = TRANSMIT_CNT_LPN_ACCESS_CMD;
        }
        #endif
    }else if(is_fn_support_and_en){
    	if(!filter_cfg){
			if(is_must_use_friend_key_msg(p_nw)){
				foreach(i,g_max_lpn_num){
					if(p_match_type->F2L & BIT(i)){
						use_friend_key = i + 1;
						break;
					}
				}
			}
		}
    }
    
    mesh_cmd_bear_unseg_t bear_tmp;
    mesh_cmd_bear_unseg_t *p_bear_tmp = &bear_tmp;
    u8 adv_type = MESH_ADV_TYPE_MESSAGE;
    if(is_fn_support_and_en && (!filter_cfg)){
        if(p_match_type->F2L){
            if(is_not_cache_ctl_msg_fn(p_nw)){
                sent_cmd_flag = 1;
            }else{
                cache_flag = 1;
                memcpy(p_bear_tmp, bear, sizeof(mesh_cmd_bear_unseg_t));
                sent_cmd_flag = !is_unicast_friend_msg_to_lpn(p_nw);
            }
        }else{
            sent_cmd_flag = 1;
        }
    }else{
        sent_cmd_flag = 1;
    }
    
	#if (DEBUG_SHOW_VC_SELF_EN == SHOW_VC_SELF_NW_ENC)
	int increase_sno_ok = 0;
	#endif

    if(sent_cmd_flag){
    	u8 nk_array_idx = p_match_type->mat.nk_array_idx;
    	int seg_flag = p_bear->lt.seg;
        err = mesh_sec_msg_enc_nw((u8 *)p_nw, len_lt, swap_type_lt, use_friend_key, len_nw, adv_type,filter_cfg, nk_array_idx);  // msut at last
		if(err){
		    LOG_MSG_ERR(TL_LOG_MESH,0, 0 ,"nw key err",0);
			return err;
		}

		#if (DEBUG_SHOW_VC_SELF_EN == SHOW_VC_SELF_NW_ENC)
		if(mesh_cmd_action_need_local(p_match_type)){
			increase_sno_ok = 1;
			mesh_increase_sno(err);		// must after app event handler adv ll
			mesh_cmd_bear_unseg_t bear2vc;
			memcpy(&bear2vc, p_bear, sizeof(bear2vc));
			app_event_handler_adv(&bear2vc.len, ADV_FROM_GATT, 0);	// no relay
			if((DST_MATCH_MAC == p_match_type->type)&&(p_match_type->local)){
				return 0;
			}
		}
		#endif

		if(filter_cfg){
			#if WIN32 
			prov_write_data_trans((u8 *)p_nw,len_nw,MSG_PROXY_CONFIG);
			#endif 
		}else{
			if(!is_pkt_notify_only(dst_adr)){
				LOG_MSG_INFO(TL_LOG_MESH,0, 0,"mesh_tx_cmd_layer_network:mesh_bear_tx2mesh_in_fifo",0);
				if(mesh_need_random_delay){
				    if((!use_mesh_adv_fifo_fn2lpn) && (!seg_flag)){
				        p_bear->type |= RSP_DELAY_FLAG; // record into fifo
				    }
				    mesh_need_random_delay = 0; // clear here should be more safe.
				}
				err = mesh_bear_tx2mesh((u8 *)p_bear, p_match_type->trans.val);
                p_bear->type &= ~RSP_DELAY_FLAG;  // clear after push fifo.
			}
		}

		#if FEATURE_PROXY_EN
		if(is_proxy_support_and_en){
			u8 adv_type = filter_cfg ? MESH_ADV_TYPE_PROXY_CFG_FLAG : MESH_ADV_TYPE_MESSAGE;
			if( (adv_type == MESH_ADV_TYPE_PROXY_CFG_FLAG)||
				(is_valid_adv_with_proxy_filter(dst_adr) && adv_type == MESH_ADV_TYPE_MESSAGE)){
				mesh_bear_tx2gatt((u8 *)p_bear,adv_type);
			}
		}
		#else
		dst_adr = dst_adr;	// just for compile warning
		#endif
    }
    
    if((0 == err) && cache_flag){
		#if FN_PRIVATE_SEG_CACHE_EN
        if(p_bear_tmp->lt.seg){
            p_bear_tmp->trans_par_val = p_match_type->trans.val;
            p_bear_tmp->len = len_nw + 1;
            p_bear_tmp->type = adv_type;
            
            mesh_match_type_t match_type_temp; 	// why create ?
            #if 1
            memcpy(&match_type_temp, p_match_type,sizeof(match_type_temp));
            #else
            memset(&match_type_temp, 0, sizeof(match_type_temp));
            match_type_temp.val = 0; // init
            match_type_temp.F2L = 1;
            match_type_temp.type = DST_MATCH_MAC;
            #endif
            mesh_rc_segment_handle((mesh_cmd_bear_seg_t *)p_bear_tmp, p_bear_tmp->nw.ctl, &match_type_temp);
        }else
		#endif
        {
            mesh_swap_nw_lt((u8 *)&p_bear_tmp->nw, swap_type_lt);
            mesh_fri_cmd2cache((u8 *)p_bear_tmp, len_nw, adv_type, p_match_type->trans.val, p_match_type->F2L);
        }
    }
    
	#if (DEBUG_SHOW_VC_SELF_EN == SHOW_VC_SELF_NW_ENC)
	if(!increase_sno_ok)
	#endif
	{
    mesh_increase_sno(err);
    }
    return err;
}

int mesh_tx_cmd_layer_lower_unseg(u8 *p_bear, u8 len_ut, u8 swap_type_lt, mesh_match_type_t *p_match_type)
{
    int err = -1;
    LAYER_DATA_DEBUG(A_buf2_ut, p_bear, sizeof(mesh_cmd_bear_unseg_t));
    mesh_cmd_lt_unseg_t *p_lt = &((mesh_cmd_bear_unseg_t *)p_bear)->lt;
    mesh_cmd_nw_t *p_nw = &((mesh_cmd_bear_unseg_t *)p_bear)->nw;
    p_lt->seg = 0;
    u8 len_lt=0;
    if(p_nw->ctl && (!p_match_type->filter_cfg)){
        mesh_cmd_lt_ctl_unseg_t *p_lt_ctl = (mesh_cmd_lt_ctl_unseg_t *)p_lt;
        if(0 == p_lt_ctl->opcode){  // ack
            len_lt = sizeof(mesh_cmd_lt_ctl_seg_ack_t);
        }else{
            len_lt = OFFSETOF(mesh_cmd_lt_ctl_unseg_t, data) + len_ut;
        }
    }else{
        len_lt = OFFSETOF(mesh_cmd_lt_unseg_t, data) + len_ut;
    }
    LOG_MSG_INFO(TL_LOG_MESH,0, 0,"mesh_tx_cmd_layer_lower_unseg:mesh_tx_cmd_layer_network",0);
    err = mesh_tx_cmd_layer_network((u8 *)p_bear, len_lt, swap_type_lt, p_match_type);
    if(!err){
//        mesh_cmd_bear_unseg_t *p_br = (mesh_cmd_bear_unseg_t *)p_bear;
    }
    
    return err;
}

int mesh_tx_cmd_layer_lower_seg(u8 *p_bear, u8 len_ut, mesh_match_type_t *p_match_type)
{
    //mesh_cmd_lt_seg_t *p_lt_seg = &((mesh_cmd_bear_seg_t *)p_bear)->lt;
    u8 len_lt = OFFSETOF(mesh_cmd_lt_seg_t, data) + len_ut;
    //p_lt_seg->seg = 1;    // have been set in upper

    return mesh_tx_cmd_layer_network((u8 *)p_bear, len_lt, SWAP_TYPE_LT_SEG, p_match_type);
}

u32 mesh_seg_add_ut(mesh_cmd_bear_seg_t *p_bear, u8 *p_ut, u32 len_ut, u32 segO)
{
    mesh_cmd_ut_seg_t *p_ut_seg = &p_bear->ut;
    mesh_cmd_nw_t *p_nw = &p_bear->nw;
    
    u32 max_size = mesh_max_payload_get(p_nw->ctl);
    u32 len_seg = 0;
    u32 pos = segO * max_size;
    if(pos + max_size < len_ut){
        len_seg = max_size;
    }else{
    	if(len_ut > pos){
        	len_seg = len_ut - pos;
        }
    }
    memcpy(p_ut_seg, p_ut + pos, len_seg);

    return len_seg;
}

int mesh_seg_add_pkt(mesh_cmd_bear_seg_t *p_bear, u8 *p_ac, u32 len_ut, u32 segO)
{
	if(segO > p_bear->lt.segN){
		return -1;
	}
	
#if VC_CHECK_NEXT_SEGMENT_EN
    VC_tick_next_segment = clock_time()|1;
#endif

    int err = 0;
    mesh_cmd_lt_seg_t *p_lt_seg = &p_bear->lt;

    // network
    mesh_cmd_nw_t *p_nw = &p_bear->nw;
    memcpy(p_nw->sno, &mesh_adv_tx_cmd_sno, sizeof(p_nw->sno));

    // upper
    u32 len_seg = mesh_seg_add_ut(p_bear, p_ac, len_ut, segO);
  
    // fill lower transport layer
    p_lt_seg->segO = segO;
    mesh_cmd_bear_seg_t cmd_bear_tmp;   // must use a new one, because other segment pkt use cmd_bear later
    memcpy(&cmd_bear_tmp, p_bear, sizeof(cmd_bear_tmp));
    err = mesh_tx_cmd_layer_lower_seg((u8 *)&cmd_bear_tmp, len_seg, &mesh_tx_seg_par.match_type);
	if(!err){
		mesh_tx_seg_par.tx_segO_next = segO + 1;
	}

	#if FEATURE_LOWPOWER_EN
	mesh_lpn_sleep_enter_normal_seg();
	#endif
	
    return err;
}

void mesh_seg_add_pkt_reset_timer(int cnt_pkt)
{
    mesh_tx_seg_par.timeout = clock_time()|1;
}

void mesh_tx_segment_finished()
{
    mesh_tx_seg_par.busy = 0;
    // don't clear other parameters for debug now

    #if FEATURE_LOWPOWER_EN
    if(is_lpn_support_and_en){
    	//if(!get_adv_cmd()){
			mesh_lpn_sleep_enter_later();
		//}
    }
    #endif
    
	SEG_DEBUG_LED(1);
}

int mesh_tx_segment_handle(mesh_cmd_bear_seg_t *p_bear, u8 *p_ac, u32 len_ut, u8 szmic_b, mesh_match_type_t *p_match_type)
{
    int err = 0;
    
    mesh_cmd_nw_t *p_nw = &p_bear->nw;
    mesh_cmd_lt_seg_t *p_lt_seg = &p_bear->lt;
        
    p_lt_seg->seg = 1;
    p_lt_seg->segN = mesh_msg_cnt_get(len_ut, p_nw->ctl) - 1;
    p_lt_seg->seqzero = mesh_adv_tx_cmd_sno;
    if(!p_nw->ctl){
        p_lt_seg->szmic = szmic_b;
    }
    
    memset(&mesh_tx_seg_par, 0, sizeof(mesh_tx_seg_par));    // init
    //if(is_unicast_adr(p_nw->dst)){ // finished in check_and_send_next_segment_pkt()
        mesh_tx_seg_par.busy = 1;
        mesh_seg_add_pkt_reset_timer(p_lt_seg->segN+1);
    //}
    mesh_tx_seg_par.seqzero = p_lt_seg->seqzero;
    //mesh_tx_seg_par.seg_map_ok = BIT_MASK_LEN((u32)(p_lt_seg->segN + 1));
    mesh_tx_seg_par.len_ut = len_ut;
    mesh_tx_seg_par.ctl = p_nw->ctl;
    
    memcpy(&mesh_tx_seg_par.match_type, p_match_type, sizeof(mesh_tx_seg_par.match_type));

    #if (0 == VC_CHECK_NEXT_SEGMENT_EN)
    if(is_pkt_notify_only(p_bear->nw.dst)){
        for(u32 i=0; i<(p_lt_seg->segN+1); i++){
            // fill upper transport layer
            err = mesh_seg_add_pkt(p_bear, p_ac, len_ut, i);
        }
        mesh_tx_segment_finished();     // already push all pkt to notify fifo,
        err = 0;
    }else
    #endif
    {
        err = mesh_seg_add_pkt(p_bear, p_ac, len_ut, 0);    // send first pkt;
        if(!err){
        }
        memcpy(&cmd_bear_seg_buf, p_bear, sizeof(cmd_bear_seg_buf));    // save for repeated message
    }

    return err;
}

int mesh_cmd_action_need_local(mesh_match_type_t *p_match_type)
{
    return (p_match_type->local && p_match_type->type);
}

#if FEATURE_LOWPOWER_EN
static inline int mesh_cmd_action_need_friend(mesh_match_type_t *p_match_type)
{
    return 0;	// use inline to decreace firmware size
}
#else
int mesh_cmd_action_need_friend(mesh_match_type_t *p_match_type)
{
    return (p_match_type->F2L && p_match_type->type);
}
#endif

#if RELIABLE_CMD_EN
// ----------------------------reliable flow
mesh_tx_reliable_t mesh_tx_reliable;

void mesh_tx_reliable_tick_refresh()
{
	mesh_tx_reliable.tick = clock_time();
}

void mesh_tx_reliable_tick_refresh_proc(int rx_seg_flag, u16 adr_src)
{
	if(mesh_tx_reliable.busy){
		if((!rx_seg_flag) || mesh_tx_reliable.mat.adr_dst == adr_src){
			mesh_tx_reliable_tick_refresh();
		}
	}
}

void mesh_tx_reliable_start(u8 retry_cnt)
{
    mesh_tx_reliable.busy = 1;
    mesh_tx_reliable_tick_refresh();
    mesh_tx_reliable.retry_cnt = retry_cnt;
	LOG_MSG_INFO(TL_LOG_MESH,0,0,"mesh_tx_reliable_start:start reliable cmd ",0);
    rf_link_slave_read_status_start();
}

void mesh_tx_reliable_finish()
{
#if VC_APP_ENABLE
		if(mesh_node_retry.num){
			mesh_node_retry.num--;
			mesh_tx_reliable.mat.adr_dst = mesh_node_retry.addr[mesh_node_retry.num];
			mesh_tx_reliable_start(g_reliable_retry_cnt_def);
			return;
		}
#endif

#if FAST_PROVISION_ENABLE
	mesh_fast_prov_reliable_finish_handle();
#endif

#if (__PROJECT_MESH_PRO__ || __PROJECT_MESH_GW_NODE__)
    int report_finish = 0;
    u16 op = 0;
    u32 rsp_max = 0, rsp_cnt = 0;
	u16 dst =0;
    if(mesh_tx_reliable.busy){
        report_finish = 1;
        op = mesh_tx_reliable.mat.op;
        rsp_max = mesh_tx_reliable.mat.rsp_max;
        rsp_cnt = mesh_tx_reliable.rsp_cnt;
		dst = mesh_tx_reliable.mat.adr_dst;
	}
#endif
	if(mesh_tx_reliable.busy){
    	if((NODE_RESET == mesh_tx_reliable.mat.op) && is_actived_factory_test_mode()){
    	    client_node_reset_cb(mesh_tx_reliable.mat.adr_dst);
    	}
    }
    
    //if(mesh_tx_reliable.busy){
        memset(&mesh_tx_reliable, 0, sizeof(mesh_tx_reliable));
        rf_link_slave_read_status_stop ();
    //}
    
#if (__PROJECT_MESH_PRO__ || __PROJECT_MESH_GW_NODE__) //gateway will also have the reliable stop report cmd 
    if(report_finish){  // shoulb be after init mesh tx reliable
        extern void mesh_tx_reliable_stop_report(u16 op,u16 adr_dst, u32 rsp_max, u32 rsp_cnt);
        mesh_tx_reliable_stop_report(op, dst,rsp_max, rsp_cnt);
    }
#endif
}

int mesh_tx_reliable_rc_rsp_handle(mesh_rc_rsp_t *p_rsp)
{
    int err = 0;
    if(mesh_tx_reliable.busy){
        err = mesh_rsp_handle(p_rsp);
    }
    return err;
}

#define RELIABLE_INTERVAL_MS_MIN       (2 * CMD_INTERVAL_MS + MESH_RSP_BASE_DELAY_STEP*10 + 300)	// relay + response
#define RELIABLE_INTERVAL_MS_MAX       (2000)

int mesh_tx_cmd_reliable(material_tx_cmd_t *p)
{
	int err = -1;
	
	#if (MD_MESH_OTA_EN && MD_CLIENT_EN)
    if(FW_DISTRIBUT_STOP == p->op){
        mesh_tx_reliable_finish();
        memset(&mesh_tx_seg_par, 0, sizeof(mesh_tx_seg_par));    // init
    }
	#endif
	
    if(is_busy_mesh_tx_cmd(p->adr_dst)){
        LOG_MSG_ERR(TL_LOG_MESH,0, 0 ,"tx reliable: cmd is busy",0);
        return -1;
    }
	LOG_MSG_LIB(TL_LOG_NODE_SDK,p->par, p->par_len,"tx cmd reliable,op:0x%x,dst:0x%x par:",p->op,p->adr_dst);
    mesh_match_type_t match_type;
    mesh_match_group_mac(&match_type, p->adr_dst, p->op, 1, p->adr_src);

    mesh_tx_reliable_finish();
    is_cmd_with_tid_check_and_set(p->adr_src, p->op, p->par, p->par_len, p->tid_pos_vendor_app);
    
    if(1){ //(p->par_len <= sizeof(mesh_tx_reliable.ac_par)){ // not segment
    	memcpy(&mesh_tx_reliable.mat, p, sizeof(material_tx_cmd_t));
    	
        memset(mesh_tx_reliable.ac_par, 0, sizeof(mesh_tx_reliable.ac_par));
        memcpy(mesh_tx_reliable.ac_par, p->par, p->par_len);
        mesh_tx_reliable.mat.par = mesh_tx_reliable.ac_par;
        
        memcpy(&mesh_tx_reliable.match_type, &match_type,sizeof(mesh_tx_reliable.match_type));
        #if VC_SUPPORT_ANY_VENDOR_CMD_EN
        if(IS_VENDOR_OP(mesh_tx_reliable.mat.op)){
            //mesh_tx_reliable.mat.op_rsp = mesh_tx_reliable.mat.op_rsp;
        }else
        #endif
        {
            mesh_tx_reliable.mat.op_rsp = get_op_rsp(p->op);
        }
        
        u32 t_ms = RELIABLE_INTERVAL_MS_MIN;
        if(p->rsp_max > 1 && (FW_UPDATE_ABORT != p->op)){
            t_ms += (p->rsp_max - 1) * CMD_INTERVAL_MS;
            if(t_ms > RELIABLE_INTERVAL_MS_MAX){
                t_ms = RELIABLE_INTERVAL_MS_MAX;
            }
        }
        mesh_tx_reliable.invl_ms = t_ms;

        err = mesh_tx_cmd2_access(p, 1, &match_type);
        if(err){	// cancle reliable flow
			memset(&mesh_tx_reliable, 0, sizeof(mesh_tx_reliable));
        }
    }else{
    	err = mesh_tx_cmd2_access(p, 0, &match_type);
    }

    return err;
}

int is_busy_reliable_cmd(u16 adr_dst){return (mesh_tx_reliable.busy && !is_own_ele(adr_dst));}
#else
int is_busy_reliable_cmd(u16 adr_dst){return 0;}

#endif

int mesh_tx_cmd_unreliable(material_tx_cmd_t *p)
{
    int err = -1;

    u8 repeat_cnt = REPEATE_CNT_UNRELIABLE;
    mesh_match_type_t match_type;
    mesh_match_group_mac(&match_type, p->adr_dst, p->op, 1, p->adr_src);
    if((DST_MATCH_MAC == match_type.type)	// local or LPN
    || (is_lpn_support_and_en)){
    	repeat_cnt = 1;
    }
    is_cmd_with_tid_check_and_set(p->adr_src, p->op, p->par, p->par_len, p->tid_pos_vendor_app);

    if(p->op & 0xff00){
        LOG_MSG_LIB(TL_LOG_NODE_SDK,p->par, p->par_len>16?16:p->par_len,"mesh tx NoAck,op:0x%04x,src:0x%04x,dst:0x%04x,par:",p->op,p->adr_src, p->adr_dst);
    }else{
        LOG_MSG_LIB(TL_LOG_NODE_SDK,p->par, p->par_len>16?16:p->par_len,"mesh tx NoAck,op:0x%02x,src:0x%04x,dst:0x%04x,par:",p->op,p->adr_src, p->adr_dst);
    }

    foreach(i,repeat_cnt){
        int err_temp = mesh_tx_cmd2_access(p, 0, &match_type);
        if(0 == i){
            err = err_temp;     // return the first result.
        }
        
        if(err_temp){
            break;
        }
    }
    return err;
}

int is_busy_tx_seg(u16 adr_dst)
{
    // mesh_cmd_ut_tx_seg is used for segment all the time when tx segment busy.  
    return (mesh_tx_seg_par.busy && !is_own_ele(adr_dst));
}

int is_busy_mesh_tx_cmd(u16 adr_dst)
{
	return (is_busy_reliable_cmd(adr_dst) || is_busy_tx_seg(adr_dst));
}


#if RELIABLE_CMD_EN
#if VC_APP_ENABLE
void reliable_rsp_check_ll(u16 *addr, u16 cnt)
{
	if(cnt){
		if(cnt > ARRAY_SIZE(mesh_node_retry.addr)){
			cnt = ARRAY_SIZE(mesh_node_retry.addr);
		}
		mesh_node_retry.num = cnt;
		memcpy(mesh_node_retry.addr, addr, cnt*2);
	}
}

void reliable_rsp_check_app()
{
	u16 adr_missing[ARRAY_SIZE(mesh_node_retry.addr)], len;
	extern int vc_check_reliable_rsp(u16 *missing_addr, u16 max_num);
	len = vc_check_reliable_rsp(adr_missing, ARRAY_SIZE(mesh_node_retry.addr));
	
	reliable_rsp_check_ll(adr_missing, len);
}

#endif

void mesh_tx_reliable_proc()
{
    if(mesh_tx_reliable.busy){
        if(clock_time_exceed(mesh_tx_reliable.tick, mesh_tx_reliable.invl_ms * 1000)){
			mesh_tx_reliable_tick_refresh();
            if(mesh_tx_reliable.retry_cnt){
				LOG_MSG_INFO(TL_LOG_COMMON,0,0,"mesh_tx_reliable_proc:retry cnt %d", mesh_tx_reliable.retry_cnt);
                mesh_tx_reliable.retry_cnt--;
                mesh_tx_cmd2_access(&mesh_tx_reliable.mat, 0, &mesh_tx_reliable.match_type);
            }else{            
				#if VC_APP_ENABLE
				if((mesh_tx_reliable.mat.op != FW_UPDATE_ABORT) && (mesh_fast_prov_sts_get() ==FAST_PROV_IDLE) ){
				    reliable_rsp_check_app();
				}
				#endif

                mesh_tx_reliable_finish();
				#if MD_MESH_OTA_EN
                mesh_ota_master_ack_timeout_handle();
				#endif
            }
        }
        
		rf_link_slave_read_status_update ();
    }
}

int is_rsp2tx_reliable(u32 op_rsp, u16 adr_src)
{
	if(op_rsp == mesh_tx_reliable.mat.op_rsp){
		if(is_unicast_adr(mesh_tx_reliable.mat.adr_dst)){
			if(mesh_tx_reliable.mat.adr_dst == adr_src){
				return 1;
			}
		}else{
			return 1;
		}
	}
	return 0;
}
#endif

int mesh_tx_cmd_layer_upper(material_tx_cmd_t *p, int reliable, mesh_match_type_t *p_match_type)
{
    int err = 0;
    if(is_tx_status_cmd2self(p->op, p->adr_dst)){
        return -1;
    }
    
    memset(&cmd_bear, 0, sizeof(cmd_bear));   // org_buf
    memcpy(&p_match_type->mat, p, sizeof(material_tx_cmd_t));
    
    mesh_cmd_bear_unseg_t *p_bear = &cmd_bear;
    mesh_cmd_nw_t *p_nw = &p_bear->nw;
    mesh_cmd_lt_unseg_t *p_lt_unseg = &p_bear->lt;
    u8 szmic_b = SZMIC_SEG_FLAG;
    
    #if SEC_MES_DEBUG_EN
    extern mesh_cmd_bear_seg_t B_test_cmd;
    p_nw->ttl = B_test_cmd.nw.ttl;
    p_nw->ctl = B_test_cmd.nw.ctl;
    p_nw->src = B_test_cmd.nw.src;
    p_nw->dst = B_test_cmd.nw.dst;
    p_lt_unseg->akf = B_test_cmd.lt.akf;
    szmic_b = B_test_cmd.lt.szmic;
    #else
    p_nw->ttl = model_sig_cfg_s.ttl_def;
    if(p->pub_md){
        if(p->pub_md->pub_par.ttl != TTL_PUB_USE_DEFAULT){
    	    p_nw->ttl = p->pub_md->pub_par.ttl;     // reinitialization
    	}
    	p_match_type->trans.val = p->pub_md->pub_par.transmit.val;  // reinitialization
    	if(pts_test_en){
    	    p_match_type->trans.count = 5;  // PTS may use 0 as nw transmit cnt,
    	}
    }
    //p_nw->ctl = 0;				// have been init to 0,
    p_nw->src = p->adr_src;
    p_nw->dst = p->adr_dst;
    if(is_cfg_model(p_match_type->id, p_match_type->sig)){
		#if 0
		p_lt_unseg->akf = 1;	// not use device key now, because app have not managed key.
		#else
		p_lt_unseg->akf = 0;	// have been init to 0,
		#endif
    }else{
		p_lt_unseg->akf = 1;
    }
    #endif

    memcpy(p_nw->sno, &mesh_adv_tx_cmd_sno, sizeof(p_nw->sno));
    
    u8 ctl_flag = p_nw->ctl;	// save
    #if 1 // WIN32,  not use dynamic request now.
	u8 ac_no_enc[ACCESS_WITH_MIC_LEN_MAX]; // not surpport dynamic request stack.
    #else
	u8 ac_no_enc[p->len_ac + 8];//  dynamic to save stack RAM. 
	#endif
	memcpy(ac_no_enc, p->p_ac, p->len_ac);

	u32 len_ut = 0;
	
	#if (DEBUG_SHOW_VC_SELF_EN == SHOW_VC_SELF_NO_NW_ENC)
	u8 ac_enc_back[ACCESS_WITH_MIC_LEN_MAX];
	#endif
	mesh_cmd_nw_t nw_little_dec_backup = {0};
	
    // not command to itself;    // not unsigned adr
    #if (DEBUG_SHOW_VC_SELF_EN != SHOW_VC_SELF_NW_ENC)
    if(((DST_MATCH_MAC == p_match_type->type)&&(p_match_type->local&&(!mesh_adv_txrx_self_en)))/*|| (0 == p_nw->dst)*/){
        if((DST_MATCH_MAC == p_match_type->type)){
            #if RELIABLE_CMD_EN
            if(reliable){
    			rf_link_slave_read_status_start();	// for set flag of receive response
            }
            #endif
        }
        
		#if (DEBUG_SHOW_VC_SELF_EN == SHOW_VC_SELF_NO_NW_ENC)
        int mic_length = GET_SEG_SZMIC(szmic_b);
        if(mesh_sec_msg_enc_apl((u8 *)p, (u8 *)p_bear, mic_length)){
			LOG_MSG_ERR(TL_LOG_MESH,0, 0,"mesh_tx_cmd_layer_upper:local not found device key",0);
        	return -1;	// not found device key.
        }
        len_ut = p->len_ac + mic_length;

		memcpy(ac_enc_back, p->p_ac, sizeof(ac_enc_back));
		#endif
		memcpy(&nw_little_dec_backup, p_nw, sizeof(nw_little_dec_backup));
    }else
	#endif
    {
        int mic_length = GET_SEG_SZMIC(szmic_b);		
		#if TESTCASE_FLAG_ENABLE
		tc_seg_buf[1] = MESH_ADV_TYPE_MESSAGE;
		memcpy(tc_seg_buf+2, (u8 *)p_nw,  OFFSETOF(mesh_cmd_nw_t, data));
		memcpy(tc_seg_buf+2+ OFFSETOF(mesh_cmd_nw_t, data),p->p_ac, p->len_ac);
		SET_TC_FIFO(TSCRIPT_MESH_TX,tc_seg_buf, OFFSETOF(mesh_cmd_nw_t, data)+2+p->len_ac);
		#endif		
        if(mesh_sec_msg_enc_apl((u8 *)p, (u8 *)p_bear, mic_length)){
			LOG_MSG_ERR(TL_LOG_MESH,0, 0,"mesh_tx_cmd_layer_upper:not found device or app key",0);
        	return -1;	// not found device key.
        }
        len_ut = p->len_ac + mic_length;

		#if (DEBUG_SHOW_VC_SELF_EN == SHOW_VC_SELF_NO_NW_ENC)
		memcpy(ac_enc_back, p->p_ac, sizeof(ac_enc_back));
		#endif
		memcpy(&nw_little_dec_backup, p_nw, sizeof(nw_little_dec_backup));

        #if RELIABLE_CMD_EN
		if(reliable){
			u8 retry_cnt = is_cmd2lpn(p_nw->dst) ? 0 : p->retry_cnt;
			if(retry_cnt > RELIABLE_RETRY_CNT_MAX){
			    retry_cnt = RELIABLE_RETRY_CNT_MAX; // prevent retry time too much
			}
			mesh_tx_reliable_start(retry_cnt);
		}
		#endif
		
        if((len_ut <= ACCESS_WITH_MIC_LEN_MAX_UNSEG) && (!p->seg_must)){  // unsegment
            LAYER_DATA_DEBUG(A_buf1_ac, p->p_ac, sizeof(mesh_cmd_bear_unseg_t));
            
            // fill upper transport layer
            mesh_cmd_ut_unseg_t *p_ut_unseg = &p_bear->ut;
            memcpy(p_ut_unseg, p->p_ac, len_ut);
			LOG_MSG_INFO(TL_LOG_MESH,0, 0,"mesh_tx_cmd_layer_upper:mesh_tx_cmd_layer_lower_unseg",0);
            err = mesh_tx_cmd_layer_lower_unseg((u8 *)p_bear, len_ut, SWAP_TYPE_LT_UNSEG, p_match_type);
		}else{          // segment
			LOG_MSG_INFO(TL_LOG_MESH,0,0,"mesh_tx_cmd_layer_upper:segment tx start ",0);
            err = mesh_tx_segment_handle((mesh_cmd_bear_seg_t *)p_bear, p->p_ac, len_ut, szmic_b, p_match_type);
        }
    }

    #if (DEBUG_SHOW_VC_SELF_EN != SHOW_VC_SELF_NW_ENC) // must at last of mesh_tx_cmd_layer_upper()
	// must after push message to buf, if not, "cmd_bear" may be replaced when there is status response from itself.
    if(mesh_cmd_action_need_local(p_match_type)){
        if(ctl_flag){
            // control message
        }else{
        	#if (DEBUG_SHOW_VC_SELF_EN == SHOW_VC_SELF_NO_NW_ENC)
            nw_little_dec_backup.ivi = iv_idx_st.tx[3]; // access layer encryp with tx current
			mesh_set_iv_idx_rx(nw_little_dec_backup.ivi);
			mesh_key.netkey_sel_dec = p_match_type->mat.nk_array_idx;
			mesh_key.appkey_sel_dec = p_match_type->mat.ak_array_idx;
			mesh_key.new_netkey_dec = 0;
			int err2 = mesh_sec_msg_dec_apl(ac_enc_back, len_ut, (u8 *)&nw_little_dec_backup);
			if(!err2){
            	mesh_rc_data_layer_access(ac_enc_back, p->len_ac, &nw_little_dec_backup);
            }
        	#else
            mesh_rc_data_layer_access((u8 *)ac_no_enc, p->len_ac, &nw_little_dec_backup);
            #endif
        }
    }
    #endif
    
    return err;
}

int mesh_tx_cmd_layer_upper_ctl_ll(u8 op, u8 *par, u32 len_par, u16 adr_src, u16 adr_dst,u8 filter_cfg)
{
    int err = 0;
    memset(&cmd_bear, 0, sizeof(cmd_bear));   // org_buf
    
    mesh_cmd_bear_unseg_t *p_bear = &cmd_bear;
    mesh_cmd_nw_t *p_nw = &p_bear->nw;
    mesh_cmd_lt_ctl_unseg_t *p_lt = &p_bear->lt_ctl_unseg;
    u32 len_ut = len_par;
    #if 1   // need send POLL also, when during send normal segmented message.
    if(is_busy_tx_seg(adr_dst)){
        if(len_ut > sizeof(p_lt->data)){
            err = -1;
            return err;
        }else{
            // just only POLL command, or other command not use mesh_cmd_ut_tx_seg.
        }
    }
    #endif

    p_nw->ctl = 1;
    if(is_lpn_support_and_en){
        if(p_nw->ctl && (len_par > sizeof(p_lt->data))){
            return -1;  // low power node should not sent segment control message
        }
    }

    u8 nk_array_idx = 0;  // use default key.  include LPN
    #if FEATURE_FRIEND_EN
    if((CMD_CTL_UPDATE == op)||(CMD_CTL_OFFER == op)||(CMD_CTL_CLEAR == op)
     ||(CMD_CTL_CLR_CONF == op)||(CMD_CTL_SUBS_LIST_CONF == op)){
        nk_array_idx = get_tx_nk_arr_idx_friend(adr_dst, op);
    }
    #endif
    
    // the configure proxy cmd don't have these two codes 
    if((CMD_CTL_CLEAR == op) || (CMD_CTL_CLR_CONF == op)){
        p_nw->ttl = TTL_MAX;
    }else if (CMD_CTL_HEARTBEAT == op){
		p_nw->ttl = model_sig_cfg_s.hb_pub.ttl;
		nk_array_idx = 0;   // use default key
	}else if(CMD_CTL_ACK == op){
        p_nw->ttl = model_sig_cfg_s.ttl_def;
        nk_array_idx = mesh_key.netkey_sel_dec; // mesh_add_seg_ack set netkey index too.
    }else{
        p_nw->ttl = 0;
    }
    p_nw->src = adr_src;
    p_nw->dst = adr_dst;
    p_lt->opcode = op;

    memcpy(p_nw->sno, &mesh_adv_tx_cmd_sno, sizeof(p_nw->sno));

    mesh_match_type_t match_type;
    mesh_match_group_mac(&match_type, p_nw->dst, -1, 0, 0);	// no subscription list for control command.
	match_type.filter_cfg = filter_cfg;
	
	set_material_tx_cmd(&match_type.mat, op, par, len_par, adr_src, adr_dst, g_reliable_retry_cnt_def, 0, 0, nk_array_idx, 0, 0);
	
	if(mesh_cmd_action_need_local(&match_type)){
        if(DST_MATCH_MAC == match_type.type){
            return err; // command to itself
        }
    }else if(0 == p_nw->dst){   // unsigned adr
        //return err;
    }
	#if TESTCASE_FLAG_ENABLE
	tc_seg_buf[1] = MESH_ADV_TYPE_MESSAGE;
	memcpy(tc_seg_buf+2, (u8 *)p_nw,  OFFSETOF(mesh_cmd_nw_t, data));
	tc_seg_buf[2+OFFSETOF(mesh_cmd_nw_t, data)] = op;
	memcpy(tc_seg_buf+2+ OFFSETOF(mesh_cmd_nw_t, data)+1,par, len_par);
	SET_TC_FIFO(TSCRIPT_MESH_TX,tc_seg_buf, OFFSETOF(mesh_cmd_nw_t, data)+3+len_par);
	#endif
    if(len_ut <= sizeof(p_lt->data)){  // unsegment       
        // fill upper transport layer
        memcpy(p_lt->data, par, len_par);
       	if(!match_type.filter_cfg){
	        if(p_nw->ctl && (CMD_CTL_OFFER == op)){
				mesh_friend_logmsg(p_bear, 0x19 + 2);
	        }
		}
		u8 swap_type =0;
		if(match_type.filter_cfg){
			swap_type = SWAP_TYPE_NONE;
		}else{
			swap_type = SWAP_TYPE_LT_CTL_UNSEG;
		}
		err = mesh_tx_cmd_layer_lower_unseg((u8 *)p_bear, len_ut, swap_type, &match_type);
	}else{          // segment
        u8 *p_ac = mesh_cmd_ut_tx_seg;
        memcpy(p_ac, par, len_par);
        err = mesh_tx_segment_handle((mesh_cmd_bear_seg_t *)p_bear, p_ac, len_ut, 0, &match_type);
    }
    
    return err;
}

int mesh_tx_cmd_layer_upper_ctl(u8 op, u8 *par, u32 len_par, u16 adr_src, u16 adr_dst,u8 filter_cfg)
{
    int err = mesh_tx_cmd_layer_upper_ctl_ll(op, par, len_par, adr_src, adr_dst, filter_cfg);
    #if FEATURE_LOWPOWER_EN
    if(is_lpn_support_and_en){  // must after encryption
        u32 sleep_ms = CMD_CTL_REQUEST==op ? FRI_ESTABLISH_REC_DELAY_MS : mesh_lpn_par.req.RecDelay;
        mesh_lpn_sleep_prepare(op, sleep_ms);
        lpn_quick_tx_and_suspend(1, op);
    }
    #endif

    return err;
}

int mesh_tx_cmd_layer_cfg_primary(u8 op, u8 *par, u32 len_par, u16 adr_dst)
{
	return mesh_tx_cmd_layer_upper_ctl(op, par, len_par, ele_adr_primary, adr_dst,1);
}

int mesh_tx_cmd_layer_upper_ctl_primary(u8 op, u8 *par, u32 len_par, u16 adr_dst)
{
    return mesh_tx_cmd_layer_upper_ctl(op, par, len_par, ele_adr_primary, adr_dst,0);
}

int mesh_tx_cmd_layer_upper_ctl_FN(u8 op, u8 *par, u32 len_par, u16 adr_dst)
{
    return mesh_tx_cmd_layer_upper_ctl_primary(op, par, len_par, adr_dst);
}

u8 * get_ut_tx_buf(u32 len_ac, u16 adr_dst, u8 *ac2self, u32 ac2self_max, int seg_must)
{
    if((!seg_must) && len_ac <= ACCESS_NO_MIC_LEN_MAX_UNSEG){
        return mesh_cmd_ut_tx_unseg;
    }else{
        if(len_ac + GET_SEG_SZMIC(SZMIC_SEG_FLAG) > ACCESS_WITH_MIC_LEN_MAX){
            LOG_MSG_ERR(TL_LOG_COMMON,0, 0,"get ut tx buffer failed: ac length is too big",0);
            return 0;
        }else if(is_busy_tx_seg(0)){
            #if (DEBUG_SHOW_VC_SELF_EN != SHOW_VC_SELF_NW_ENC)
            if(is_own_ele(adr_dst) && (len_ac + GET_SEG_SZMIC(SZMIC_SEG_FLAG) <= ac2self_max)){
                return ac2self;
            }else
            #endif
            {
                LOG_MSG_ERR(TL_LOG_COMMON,0, 0,"get ut tx buffer failed: tx segment busy",0);
                return 0;
            }
        }else{
            return mesh_cmd_ut_tx_seg;
        }
    }
}

#if 0
int mesh_tx_cmd_layer_acccess(u8 *p_ac_hci, u32 len_ac, u16 adr_src, u16 adr_dst, int reliable, mesh_match_type_t *p_match_type)
{
    u8 ac2self[48]; // don't set to larger, if not, stack will be too deep.
    u8 *p_ac = get_ut_tx_buf(len_ac, adr_dst, ac2self, sizeof(ac2self));
    if(!p_ac){
        return -1;
    }

    memcpy(p_ac, p_ac_hci, len_ac);
	
	material_tx_cmd_t mat;
	set_material_tx_cmd(&mat, -1, p_ac, len_ac, adr_src, adr_dst, g_reliable_retry_cnt_def, 0, 0, NET_KEY_SEL_DEF_, APP_KEY_SEL_DEF_, 0);
    return mesh_tx_cmd_layer_upper(&mat, reliable, p_match_type);
}
#endif

int mesh_tx_cmd2_access(material_tx_cmd_t *p, int reliable, mesh_match_type_t *p_match_type)
{
    if(is_tx_status_cmd2self(p->op, p->adr_dst)){
        // LOG_MSG_ERR(TL_LOG_MESH,0, 0 ,"send cmd to itself",0);
        return -1;
    }
	
	if(p_match_type->sig && (SIG_MD_CFG_SERVER == p_match_type->id)){
		mesh_mi_cfg_segmust_set(p);
    }
	
    //int err = -1;
    u32 size_op = SIZE_OF_OP(p->op);
    
    // redirect
    material_tx_cmd_t ac_mat;
    memcpy(&ac_mat, p, sizeof(ac_mat));
    ac_mat.len_ac += size_op;
    u8 ac2self[48]; // don't set to larger, if not, stack will be too deep.
    u8 *p_ac = get_ut_tx_buf(ac_mat.len_ac, ac_mat.adr_dst, ac2self, sizeof(ac2self), p->seg_must);
    if(!p_ac){
        LOG_MSG_ERR(TL_LOG_MESH,0, 0 ,"get the ut tx buffer fail",0);
        return -1;
    }
    
    ac_mat.p_ac = p_ac;
	LOG_MSG_INFO(TL_LOG_MESH,0, 0,"mesh_tx_cmd2_access:send access cmd",0);
    
    u32 op_tpye = GET_OP_TYPE(p->op);
    if(OP_TYPE_SIG1 == op_tpye){
		mesh_cmd_ac_sig1_t *ac_sig1 = (mesh_cmd_ac_sig1_t *)ac_mat.p_ac;
		ac_sig1->op = (u8)(p->op);    
		memcpy(ac_sig1->data, p->par, p->par_len);
    }else if(OP_TYPE_SIG2 == op_tpye){
		mesh_cmd_ac_sig2_t *ac_sig2 = (mesh_cmd_ac_sig2_t *)ac_mat.p_ac;
		ac_sig2->op = p->op;	 
		memcpy(ac_sig2->data, p->par, p->par_len);
    }else/* if(OP_TYPE_VENDOR == op_tpye)*/{
		mesh_cmd_ac_vd_t *ac_vd = (mesh_cmd_ac_vd_t *)ac_mat.p_ac;
		ac_vd->op = (u8)p->op;
		#if VC_SUPPORT_ANY_VENDOR_CMD_EN
		ac_vd->vd_id = (ac_mat.op_rsp >> 8) & 0xffff;
		#else
		ac_vd->vd_id = g_vendor_id;
		#endif
		memcpy(ac_vd->data, p->par, p->par_len);
    }
    #if WIN32
    if(p->op != OBJ_CHUNK_TRANSFER){
        LOG_MSG_INFO(TL_LOG_NODE_BASIC,p->par,p->par_len,"the mesh access tx cmd is 0x%04x ",p->op);
    }
	#endif
	#if GATEWAY_ENABLE
		extern u8 gateway_upload_mesh_cmd_back_vc(material_tx_cmd_t *p);
		gateway_upload_mesh_cmd_back_vc(p);		
	#endif
    return mesh_tx_cmd_layer_upper(&ac_mat, reliable, p_match_type);
}

u8 get_tx_nk_arr_idx_friend(u16 adr, u16 op)
{
	u8 nk_arr_idx = 0;
    if(is_fn_support_and_en && adr && is_unicast_adr(adr)){
        foreach(i, g_max_lpn_num){
            if(is_ele_in_node(adr, fn_other_par[i].LPNAdr, fn_req[i].NumEle)){
                if((CMD_CTL_OFFER == op) || is_in_mesh_friend_st_fn(i)){
                    if(fn_other_par[i].nk_sel_dec_fn < NET_KEY_MAX){
                        nk_arr_idx = fn_other_par[i].nk_sel_dec_fn;
                    }
                }
                break;
            }
        }
    }
    return nk_arr_idx;
}

u16 mesh_mac_match_friend(u16 adr)
{
	u16 match_F2L = 0;
    if(is_fn_support_and_en && is_unicast_adr(adr)){
        foreach(i, g_max_lpn_num){
            if(is_in_mesh_friend_st_fn(i) && is_ele_in_node(adr, fn_other_par[i].LPNAdr, fn_req[i].NumEle)){
                match_F2L |= BIT(i);
                break;
            }
        }
    }
    return match_F2L;
}

u16 mesh_group_match_friend(u16 adr)
{
	u16 match_F2L = 0;
    if(is_fn_support_and_en){
        foreach(i, g_max_lpn_num){
            if(is_in_mesh_friend_st_fn(i)){
                foreach_arr(m,fn_other_par[i].SubsList){
                    if(fn_other_par[i].SubsList[m] == adr){
                        match_F2L |= BIT(i);
                        break;
                    }
                }
            }
        }
    }
    return match_F2L;
}

int is_fixed_group(u16 adr_dst)
{
	return (adr_dst >= ADR_FIXED_GROUP_START);
}

void mesh_match_group_mac(mesh_match_type_t *p_match_type, u16 adr_dst, u32 op_ut, int app_tx, u16 adr_src){
	memset(p_match_type, 0, sizeof(mesh_match_type_t));  // init
	p_match_type->trans.val = TRANSMIT_DEF_PAR;	// init
	
	if(app_tx && is_unicast_adr(adr_src)){
		mesh_op_resource_t op_res;
		if(is_support_op(&op_res, op_ut, adr_src, 1)){
			if(op_res.model_cnt){
				foreach(i,op_res.model_cnt){
					p_match_type->model = op_res.model[i];
					p_match_type->id = op_res.id;
					p_match_type->sig = op_res.sig;
					break;
				}
			}
		}
	}

	u32 match_type = 0;
	u32 match_local = 0;
	u32 match_F2L = 0;
    
    if(ADR_UNASSIGNED == adr_dst){
    }else if(is_fixed_group(adr_dst)){
    	if(adr_dst >= ADR_ALL_PROXY){
    		int match_ok = 0;
	        if(ADR_ALL_NODES == adr_dst){
	        	match_ok = 1;
		        match_F2L = 0; //is_in_mesh_friend_st_fn(); // fix later
	        }else if(ADR_ALL_PROXY == adr_dst){
				match_ok = ((gp_page0->head.feature.proxy)
						 && (GATT_PROXY_SUPPORT_ENABLE == model_sig_cfg_s.gatt_proxy));
	        }else if(ADR_ALL_FRIEND == adr_dst){
				match_ok = ((gp_page0->head.feature.frid)
						 && (FRIEND_SUPPORT_ENABLE == model_sig_cfg_s.frid));
	        }else if(ADR_ALL_RELAY == adr_dst){
				match_ok = ((gp_page0->head.feature.relay)
						 && (RELAY_SUPPORT_ENABLE == model_sig_cfg_s.relay));
	        }

	        if(match_ok){
				match_type = DST_MATCH_FIXED_GROUP;
		        match_local = 1;
	        }
        }
    }else if(0 == (adr_dst & BIT(15))){
        if(is_own_ele(adr_dst)){
            match_local = 1;
            match_type = DST_MATCH_MAC;
        }else{
        	match_F2L = mesh_mac_match_friend(adr_dst);
        	if(match_F2L){
	            match_type = DST_MATCH_MAC;
            }
        }
    }else{
    	if(app_tx && op_ut == -1){
    		// control command, should not happen here
    		return ;
    	}
    	
        if(0 == (adr_dst & BIT(14))){
            // virtual address
			match_type = DST_MATCH_VIRTUAL;
			match_local = 1;
			match_F2L = 0;
        }else{
            match_type = DST_MATCH_GROUP;
            match_local = 1;    // no need compare here, match in mesh_rc_data_layer_access() later. because op is still encryption here.
            match_F2L = mesh_group_match_friend(adr_dst);
        }
    }

    p_match_type->type = match_type;
    p_match_type->local = match_local;
    p_match_type->F2L = match_F2L;
}

//---------------------------------------- model
// to increase efficiency
STATIC_ASSERT(sizeof(model_common_t) % 4 == 0);
STATIC_ASSERT(sizeof(model_g_light_s_t) % 4 == 0);

//-------- sig config model
model_sig_cfg_s_t 		model_sig_cfg_s;  // configuration server model
#if MD_CFG_CLIENT_EN
model_client_common_t   model_sig_cfg_c;
#endif

model_health_t 			model_sig_health;

static inline void mesh_model_store_cfg_s()
{
	mesh_model_store(1, SIG_MD_CFG_SERVER);
}

int mesh_cmd_sig_cfg_sec_nw_bc_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    return mesh_tx_cmd_rsp_cfg_model(CFG_BEACON_STATUS, &model_sig_cfg_s.sec_nw_beacon, 1, cb_par->adr_src);
}

int mesh_cmd_sig_cfg_sec_nw_bc_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    u8 sec_nw_bc = par[0];
    if(sec_nw_bc < NW_BEACON_ST_MAX){
        model_sig_cfg_s.sec_nw_beacon = sec_nw_bc;
		mesh_model_store_cfg_s();
		
		return mesh_cmd_sig_cfg_sec_nw_bc_get(par, par_len, cb_par);
    } 
	return -1;
}

int mesh_cmd_sig_cfg_sec_nw_bc_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_def_ttl_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    return mesh_tx_cmd_rsp_cfg_model(CFG_DEFAULT_TTL_STATUS, &model_sig_cfg_s.ttl_def, 1, cb_par->adr_src);
}

int mesh_cmd_sig_cfg_def_ttl_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    u8 ttl = par[0];
    if((ttl <= TTL_MAX) && (ttl != 1)){
        model_sig_cfg_s.ttl_def = ttl;
		mesh_model_store_cfg_s();
		return mesh_cmd_sig_cfg_def_ttl_get(par, par_len, cb_par);
    }
    return  -1;
}

int mesh_cmd_sig_cfg_def_ttl_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_friend_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    return mesh_tx_cmd_rsp_cfg_model(CFG_FRIEND_STATUS, &model_sig_cfg_s.frid, 1, cb_par->adr_src);
}

int mesh_cmd_sig_cfg_friend_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    u8 frid = par[0];
    if( frid < FRIEND_NOT_SUPPORT){
		if(model_sig_cfg_s.frid < FRIEND_NOT_SUPPORT){
			if(model_sig_cfg_s.frid != frid){
				model_sig_cfg_s.frid = frid;
				if(FRIEND_SUPPORT_DISABLE == frid){
				    mesh_friend_ship_init_all();
				}
				
				if(model_sig_cfg_s.hb_pub.feature & BIT(MESH_HB_FRI_BIT)){
					hb_sts_change = 1;
					hb_pub_100ms = clock_time_100ms();
				}
	            LOG_MSG_INFO(TL_LOG_FRIEND,0, 0,"friend feature change: %d",par[0]);
			}

			if((0 == pts_test_en) || frid){    // FN-BV10, only clear, no recover, so don't save
			    mesh_model_store_cfg_s();
			}
		}else{
			par[0] = model_sig_cfg_s.frid;
		}
		return mesh_cmd_sig_cfg_friend_get(par, par_len, cb_par);
    }
    return -1;
}

int mesh_cmd_sig_cfg_friend_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_gatt_proxy_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    return mesh_tx_cmd_rsp_cfg_model(CFG_GATT_PROXY_STATUS, &model_sig_cfg_s.gatt_proxy, 1, cb_par->adr_src);
}

int mesh_cmd_sig_cfg_gatt_proxy_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    u8 gatt_proxy = par[0];
    if(gatt_proxy < GATT_PROXY_NOT_SUPPORT){
		if(model_sig_cfg_s.gatt_proxy < GATT_PROXY_NOT_SUPPORT){
			if(model_sig_cfg_s.gatt_proxy != gatt_proxy){
				model_sig_cfg_s.gatt_proxy = gatt_proxy;
				if(model_sig_cfg_s.hb_pub.feature & BIT(MESH_HB_PROXY_BIT)){
					hb_sts_change = 1;
					hb_pub_100ms = clock_time_100ms();
				}	
			}
			mesh_model_store_cfg_s();
			// and the if the proxy state is being set to 0 ,the connection will be terminate 
			if (model_sig_cfg_s.gatt_proxy == GATT_PROXY_SUPPORT_DISABLE ){
				#ifndef WIN32
				// send terminate cmd 
				if(blt_state == BLS_LINK_STATE_CONN){
					bls_ll_terminateConnection(0x13);
				}
				#endif
			}	
		}else{
			par[0] = model_sig_cfg_s.gatt_proxy;
		}
		return mesh_cmd_sig_cfg_gatt_proxy_get(par, par_len, cb_par);
    }
	
    return -1;
}

int mesh_cmd_sig_cfg_gatt_proxy_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_key_phase_st_rsp(u16 nk_idx, u8 st, mesh_net_key_t *p_netkey, mesh_cb_fun_par_t *cb_par)
{
	mesh_key_refresh_phase_status_t rsp = {0};
	rsp.nk_idx = nk_idx;
	if(ST_SUCCESS == st){
		rsp.phase = p_netkey->key_phase;
	}
	rsp.st = st;
    return mesh_tx_cmd_rsp_cfg_model(CFG_KEY_REFRESH_PHASE_STATUS, (u8 *)&rsp, sizeof(rsp), cb_par->adr_src);
}

int mesh_cmd_sig_cfg_key_phase_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	u16 nk_idx = ((mesh_key_refresh_phase_get_t *)par)->nk_idx;
	mesh_net_key_t *p_netkey_exist = is_mesh_net_key_exist(nk_idx);
	u8 st = p_netkey_exist ? ST_SUCCESS : ST_INVALID_NETKEY;
    return mesh_key_phase_st_rsp(nk_idx, st, p_netkey_exist, cb_par);
}

int mesh_cmd_sig_cfg_key_phase_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	mesh_key_refresh_phase_set_t *p_set = (mesh_key_refresh_phase_set_t *)par;
	if((p_set->transition == KEY_REFRESH_TRANS2)||(p_set->transition == KEY_REFRESH_TRANS3)){
		mesh_net_key_t *p_netkey_exist = is_mesh_net_key_exist(p_set->nk_idx);
		int st = ST_SUCCESS;
		mesh_net_key_t *p_nk_dec = &mesh_key.net_key[mesh_key.netkey_sel_dec][0];
		if(p_netkey_exist && (p_netkey_exist->index == p_nk_dec->index)){
			st = mesh_key_refresh_phase_handle(p_set->transition, p_netkey_exist);
		}else{
			st = ST_INVALID_NETKEY;
		}
		
		if(st != -1){
			return mesh_key_phase_st_rsp(p_set->nk_idx, (u8)st, p_netkey_exist, cb_par);
		}
	}
	return 0;
}

int mesh_cmd_sig_cfg_key_phase_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    	mesh_key_refresh_phase_status_t *rsp = (mesh_key_refresh_phase_status_t *)par;
    	if(ST_SUCCESS == rsp->st){
    		mesh_kr_cfgcl_check_set_next_flag();
    	}
    }
    return err;
}

int mesh_cmd_sig_cfg_relay_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    return mesh_tx_cmd_rsp_cfg_model(CFG_RELAY_STATUS, &model_sig_cfg_s.relay, sizeof(mesh_cfg_model_relay_set_t), cb_par->adr_src);
}

int mesh_cmd_sig_cfg_relay_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    mesh_cfg_model_relay_set_t *p_set = (mesh_cfg_model_relay_set_t *)par;
    if(p_set->relay < RELAY_NOT_SUPPORT){
		if(model_sig_cfg_s.relay < RELAY_NOT_SUPPORT){
			if(model_sig_cfg_s.relay != p_set->relay){
				model_sig_cfg_s.relay = p_set->relay;
				//relay bit changes ,and need to send the heartbeat msg 
				if(model_sig_cfg_s.hb_pub.feature & BIT(MESH_HB_RELAY_BIT)){
					hb_sts_change = 1;
					hb_pub_100ms = clock_time_100ms();
				}
			}
			memcpy(&model_sig_cfg_s.relay_retransmit, &p_set->transmit, sizeof(model_sig_cfg_s.relay_retransmit));
			mesh_model_store_cfg_s();
		}
		else{
			memcpy(par, &model_sig_cfg_s.relay_retransmit, sizeof(model_sig_cfg_s.relay_retransmit));
		}
		return mesh_cmd_sig_cfg_relay_get(par, par_len, cb_par);
    }
    return -1;
}

int mesh_cmd_sig_cfg_relay_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_nw_transmit_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    return mesh_tx_cmd_rsp_cfg_model(CFG_NW_TRANSMIT_STATUS, (u8 *)&model_sig_cfg_s.nw_transmit, sizeof(mesh_transmit_t), cb_par->adr_src);
}

int mesh_cmd_sig_cfg_nw_transmit_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	#if 1	// can not set now
    memcpy(&model_sig_cfg_s.nw_transmit, par, sizeof(mesh_transmit_t));
	mesh_model_store_cfg_s();
	#endif
    return mesh_cmd_sig_cfg_nw_transmit_get(par, par_len, cb_par);
}

int mesh_cmd_sig_cfg_nw_transmit_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_node_identity_st_rsp(u16 nk_idx, u8 st, mesh_net_key_t *p_netkey, mesh_cb_fun_par_t *cb_par)
{
	mesh_node_identity_status_t rsp = {0};
	rsp.nk_idx = nk_idx;
	if(ST_SUCCESS == st){
		rsp.identity = p_netkey->node_identity;
	}
	rsp.st = st;
    return mesh_tx_cmd_rsp_cfg_model(NODE_ID_STATUS, (u8 *)&rsp, sizeof(rsp), cb_par->adr_src);
}

int mesh_cmd_sig_cfg_node_identity_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	u16 nk_idx = ((mesh_node_identity_get_t *)par)->nk_idx;
	mesh_net_key_t *p_netkey_exist = is_mesh_net_key_exist(nk_idx);
	u8 st = p_netkey_exist ? ST_SUCCESS : ST_INVALID_NETKEY;
    return mesh_node_identity_st_rsp(nk_idx, st, p_netkey_exist, cb_par);
}

int mesh_cmd_sig_cfg_node_identity_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	mesh_node_identity_set_t *p_set = (mesh_node_identity_set_t *)par;
	mesh_net_key_t *p_netkey_exist = is_mesh_net_key_exist(p_set->nk_idx);
	int st = -1;
	if(p_netkey_exist){
		if(p_netkey_exist->node_identity != NODE_IDENTITY_SUBNET_NOT_SUPPORT){
		 	if(p_set->identity < NODE_IDENTITY_SUBNET_NOT_SUPPORT){
				if(p_netkey_exist->node_identity != p_set->identity){
					p_netkey_exist->node_identity = p_set->identity;
					mesh_key_save();
				}
				st = ST_SUCCESS;
			}
		}else{
			st = ST_SUCCESS;
		}
	}else{
		st = ST_INVALID_NETKEY;
	}

	if(st != -1){
		return mesh_node_identity_st_rsp(p_set->nk_idx, (u8)st, p_netkey_exist, cb_par);
	}

	return 0;
}

int mesh_cmd_sig_cfg_node_identity_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_lpn_poll_timeout_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    mesh_lpn_poll_timeout_status_t rsp = {0};
    rsp.lpn_adr = get_u16_not_aligned(par);
    if((0 == rsp.lpn_adr)||(rsp.lpn_adr & 0x8000)){
        return 0;
    }
    
    rsp.timeout = get_poll_timeout_fn(rsp.lpn_adr);
    return mesh_tx_cmd_rsp_cfg_model(CFG_LPN_POLL_TIMEOUT_STATUS, (u8 *)&rsp, 5, cb_par->adr_src);
}

int mesh_cmd_sig_cfg_lpn_poll_timeout_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

// ----------------NET KEY
int mesh_cmd_sig_cfg_netkey_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	mesh_netkey_set_t *p_set = (mesh_netkey_set_t *)par;
	mesh_netkey_status_t status = {ST_INSUFFICIENT_RES};
	status.idx = p_set->idx;
	status.st = mesh_net_key_set(cb_par->op, p_set->key, p_set->idx, 1);

    return mesh_tx_cmd_rsp_cfg_model(NETKEY_STATUS, (u8 *)&status, sizeof(status), cb_par->adr_src);
}

int mesh_cmd_sig_cfg_netkey_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    	mesh_netkey_status_t *st = (mesh_netkey_status_t *)par;
    	if(ST_SUCCESS == st->st){
    		mesh_kr_cfgcl_check_set_next_flag();
    	}
    }
    return err;
}

int mesh_cmd_sig_cfg_netkey_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	mesh_netkey_list_t list;
	u32 cnt = mesh_net_key_get((u8 *)&list);
	u32 len = GET_KEY_INDEX_LEN(cnt);

    return mesh_tx_cmd_rsp_cfg_model(NETKEY_LIST, (u8 *)&list, len, cb_par->adr_src);
}

int mesh_cmd_sig_cfg_netkey_list(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

// ----------------APP KEY
int mesh_cmd_sig_cfg_appkey_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	mesh_appkey_set_t *p_set = (mesh_appkey_set_t *)par;
	mesh_appkey_status_t status;
	memcpy(&status.net_app_idx,&p_set->net_app_idx,sizeof(status.net_app_idx));
	status.st = mesh_app_key_set(cb_par->op, p_set->appkey, GET_APPKEY_INDEX(p_set->net_app_idx),
								GET_NETKEY_INDEX(p_set->net_app_idx), 1);

    return mesh_tx_cmd_rsp_cfg_model(APPKEY_STATUS, (u8 *)&status, sizeof(status), cb_par->adr_src);
}

int mesh_cmd_sig_cfg_appkey_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    	mesh_appkey_status_t *st = (mesh_appkey_status_t *)par;
    	if(ST_SUCCESS == st->st){
    		mesh_kr_cfgcl_check_set_next_flag();
    	}
    }
    return err;
}

int mesh_cmd_sig_cfg_appkey_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	mesh_appkey_get_t *p_get = (mesh_appkey_get_t *)par;
	mesh_appkey_list_t list;
	list.netkey_idx = p_get->netkey_idx;

	u32 cnt = mesh_app_key_get((u8 *)&list);
	u32 len = OFFSETOF(mesh_appkey_list_t,appkey_idx_enc) + GET_KEY_INDEX_LEN(cnt);

    return mesh_tx_cmd_rsp_cfg_model(APPKEY_LIST, (u8 *)&list, len, cb_par->adr_src);
}

int mesh_cmd_sig_cfg_appkey_list(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_model_app_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	mesh_md_app_get_t *p_get = (mesh_md_app_get_t *)par;
	mesh_sig_md_app_list_t list = {0};

	int sig_model = (SIG_MODEL_APP_GET == cb_par->op);
	u16 ele_adr = p_get->ele_adr;
	u32 model_id = sig_model ? (p_get->model_id & 0xffff) : p_get->model_id;
    u8 st = ST_UNSPEC_ERR;
	u32 cnt = 0;
		
	list.ele_adr = p_get->ele_adr;
	list.model_id = model_id;	// must before SET_KEY_INDEX

	if(is_cfg_model(model_id, sig_model)){
		st = ST_INVALID_MODEL;
	}else{
	    u8 model_idx = 0;
	    model_common_t *p_model = (model_common_t *)mesh_find_ele_resource_in_model(ele_adr, model_id, sig_model, &model_idx, 0);

	    if(p_model){
			memset(list.appkey_idx_enc, 0, sizeof(list.appkey_idx_enc));
			u8 *p_idx = (u8 *)(list.appkey_idx_enc) - FIX_SIZE(sig_model);
			foreach(i,g_bind_key_max){
				if(p_model->bind_key[i].bind_ok){
					if(cnt & 1){
						SET_KEY_INDEX_H(p_idx+((cnt/2)*3), p_model->bind_key[i].idx);
					}else{
						SET_KEY_INDEX_L(p_idx+((cnt/2)*3), p_model->bind_key[i].idx);
					}
		    		cnt++;
	    		}
    		}

			st = ST_SUCCESS;
		}else{
			if(is_own_ele(ele_adr)){
	    		st = ST_INVALID_MODEL;
			}else{
	    		st = ST_INVALID_ADR;
	    	}
	    }
    }
    
	list.st = st;
	
	u32 len = OFFSETOF(mesh_sig_md_app_list_t,appkey_idx_enc) + GET_KEY_INDEX_LEN(cnt) - FIX_SIZE(sig_model);
	u16 op_rsp = sig_model ? SIG_MODEL_APP_LIST : VENDOR_MODEL_APP_LIST;
    return mesh_tx_cmd_rsp_cfg_model(op_rsp, (u8 *)&list, len, cb_par->adr_src);
}

int mesh_cmd_sig_cfg_sig_model_app_list(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_cfg_vd_model_app_list(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int is_exist_mesh_appkey_idx(u16 appkey_idx)
{
	foreach(i,NET_KEY_MAX){
		foreach(j,APP_KEY_MAX){
		    // just compare old key is enough, because appkey_idx of old key is alway valid and same with new, if existed.
			mesh_app_key_t *p = &(mesh_key.net_key[i][0].app_key[j]);
			if(p->valid && (p->index == appkey_idx)){
				return 1;
			}
		}
	}
	return 0;
}

bind_key_t * is_exist_bind_key(model_common_t *p_model, u16 appkey_idx)
{
	foreach(i,g_bind_key_max){
		if(p_model->bind_key[i].bind_ok && (p_model->bind_key[i].idx == appkey_idx)){
			return (&(p_model->bind_key[i]));
		}
	}
	return 0;
}

bind_key_t * find_bind_key_empty(model_common_t *p_model)
{
	foreach(i,g_bind_key_max){
		if(!p_model->bind_key[i].bind_ok){
			return (&(p_model->bind_key[i]));
		}
	}
	return 0;
}

u32 get_bind_key_cnt(model_common_t *p_model)
{
	u32 cnt = 0;
	foreach(i,g_bind_key_max){
		if(p_model->bind_key[i].bind_ok){
			cnt++;
		}
	}
	return cnt;
}

void bind_key_set_par(bind_key_t *bind_key, u16 appkey_idx, u32 model_id, int sig_model)
{
	bind_key->bind_ok = 1;
	bind_key->idx = appkey_idx;
	mesh_model_store(sig_model, model_id);
}

void unbind_key_par(model_common_t *p_model, bind_key_t *bind_key, u32 model_id, int sig_model)
{
	if(p_model->pub_adr && (p_model->pub_par.appkey_idx == bind_key->idx)){
		p_model->pub_adr = 0;
		memset(p_model->pub_uuid, 0, sizeof(p_model->pub_uuid));
		memset(&p_model->pub_par, 0, sizeof(p_model->pub_par));
	}
	memset(bind_key, 0, sizeof(bind_key_t));
	mesh_model_store(sig_model, model_id);
}

u8 mesh_appkey_bind(u16 op, u16 ele_adr, u32 model_id, int sig_model, u16 appkey_idx)
{
	if(is_cfg_model(model_id, sig_model)){
		return ST_INVALID_MODEL;
	}

    if(0 == is_exist_mesh_appkey_idx(appkey_idx)){
    	return ST_INVALID_APPKEY;
    }
    
    u8 model_idx = 0;
    model_common_t *p_model = (model_common_t *)mesh_find_ele_resource_in_model(ele_adr, model_id, sig_model, &model_idx, 0);

    int st;
    if(p_model){
    	if(MODE_APP_BIND == op){
	    	if(is_exist_bind_key(p_model, appkey_idx)){
				st = ST_SUCCESS;
	    	}else{
	    		bind_key_t *bind_key = find_bind_key_empty(p_model);
		    	if(bind_key){
		    		bind_key_set_par(bind_key, appkey_idx, model_id, sig_model);
					st = ST_SUCCESS;
	            }else{
					st = ST_INSUFFICIENT_RES;
	            }
	    	}
    	}else{	// unbind
	    	bind_key_t *bind_key = is_exist_bind_key(p_model, appkey_idx);
	    	if(bind_key){
	    		unbind_key_par(p_model, bind_key, model_id, sig_model);
	    	}
			st = ST_SUCCESS;
    	}
    }else{
    	st = ST_INVALID_MODEL;
    }

    return st;
}

u8 mesh_appkey_bind_and_share(u16 op, u16 ele_adr, u32 model_id, int sig_model, u16 appkey_idx)
{
	u8 st = ST_SUCCESS;
	int share_flag = 0;
	#if 0 // SUBSCRIPTION_PUBLISH_SHARE_EN
	if(sig_model){
		if((model_id >= SIG_MD_G_ONOFF_S)&&(model_id <= SIG_MD_G_LEVEL_C)){
			share_flag = 1;
			for(unsigned int i = SIG_MD_G_ONOFF_S; i <= (SIG_MD_G_LEVEL_C); ++i){
				u8 st2 = mesh_appkey_bind(op, ele_adr, i, sig_model,appkey_idx);
				if(model_id == i){
					st = st2;
				}
			}
		}
	}
	#endif

	if(0 == share_flag){
	    st = mesh_appkey_bind(op, ele_adr, model_id, sig_model,appkey_idx);
	}

	return st;
}

int mesh_cmd_sig_cfg_bind(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    node_binding_tick = clock_time() | 1;
    
    //int err = -1;
    int sig_model = (sizeof(mesh_app_bind_t) - 2 == par_len);
	LAYER_PARA_DEBUG(A_debug_mode_keybind_enter);
    mesh_app_bind_t *p_bind = (mesh_app_bind_t *)par;
    u32 model_id = sig_model ? (p_bind->model_id & 0xFFFF) : p_bind->model_id;
    
    mesh_app_bind_status_t bind_rsp;
    memcpy(&bind_rsp.bind, p_bind, sizeof(mesh_app_bind_t));
    if(!is_unicast_adr(p_bind->ele_adr)){
        bind_rsp.st = ST_INVALID_ADR;
		LAYER_PARA_DEBUG(A_debug_mode_keybind_isnot_unicast_err);
    }else{
        mesh_adr_list_t adr_list;
        bind_rsp.st = find_ele_support_model_and_match_dst(&adr_list, p_bind->ele_adr, model_id, sig_model);

        if((adr_list.adr_cnt)){
            if(is_cfg_model(model_id, sig_model)){
                LAYER_PARA_DEBUG(A_debug_mode_keybind_iscfgmodel_err);
                bind_rsp.st = ST_CAN_NOT_BIND;
            }else{
                bind_rsp.st = mesh_appkey_bind_and_share(cb_par->op, p_bind->ele_adr, model_id, sig_model,p_bind->index);
                if(key_bind_all_ele_en && (g_ele_cnt > 1) && (p_bind->ele_adr == ele_adr_primary)){
                    foreach(i,(u32)(g_ele_cnt - 1)){
                        mesh_appkey_bind_and_share(cb_par->op, ele_adr_primary + 1 + i, model_id, sig_model,p_bind->index);
                    }
                }
            }
        }

        if(MODE_APP_UNBIND == cb_par->op){
        	//bind_rsp.st = ST_SUCCESS;	// always
        }
    }
	
    return mesh_tx_cmd_rsp_cfg_model(MODE_APP_STATUS, (u8 *)&bind_rsp, sizeof(bind_rsp)-FIX_SIZE(sig_model), cb_par->adr_src);
}

int mesh_cmd_sig_cfg_bind_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    	mesh_app_bind_status_t *p_st = (mesh_app_bind_status_t *)par;
    	if(ST_SUCCESS == p_st->st){
    		mesh_kr_cfgcl_check_set_next_flag();
    	}
    }
    return err;
}


int mesh_cmd_sig_cfg_node_reset(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	u32 rsp_flag = node_reset_start();
	if(is_actived_factory_test_mode() && (0 == rsp_flag)){
        return 0;
    }
    
    u8 par_status[1] = {0};
    return mesh_tx_cmd_rsp_cfg_model(NODE_RESET_STATUS, par_status, 0, cb_par->adr_src);
}

int mesh_cmd_sig_cfg_node_reset_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

// publish
#define PUB_ADR_AND_PAR_LEN     (2+sizeof(mesh_model_pub_par_t))
inline void mesh_copy_pub_adr_par(void * out,const void * in)
{
    memcpy(out, in, PUB_ADR_AND_PAR_LEN);
}

int mesh_save_pub_and_refresh_tick(model_common_t *p_model, mesh_cfg_model_pub_set_t *p_pub_set, u8 *pub_uuid)
{
    int change_flag = 0;
    if(memcmp(&p_model->pub_adr, &p_pub_set->pub_adr, PUB_ADR_AND_PAR_LEN)){
        mesh_copy_pub_adr_par(&p_model->pub_adr, &p_pub_set->pub_adr);
        change_flag = 1;
    }
    
    if(pub_uuid){
        if(memcmp(p_model->pub_uuid, pub_uuid, sizeof(p_model->pub_uuid))){
            memcpy(p_model->pub_uuid, pub_uuid, sizeof(p_model->pub_uuid));
            change_flag = 1;
        }
    }else{
        u8 zero[16] = {0};
        if(memcmp(p_model->pub_uuid, zero, sizeof(p_model->pub_uuid))){
            memset(p_model->pub_uuid, 0, sizeof(p_model->pub_uuid));
            change_flag = 1;
        }
    }
    
    p_model->cb_tick_ms = clock_time_ms();

    return change_flag;
}

int mesh_rsp_err_st_pub_status(u8 st, u16 ele_adr, u32 model_id, int sig_model, u16 adr_dst)
{
    mesh_cfg_model_pub_st_t pub_st;
    memset(&pub_st, 0, sizeof(pub_st));
    pub_st.status = st;
    pub_st.set.ele_adr = ele_adr;
    pub_st.set.model_id = model_id;
    return mesh_tx_cmd_rsp_cfg_model(CFG_MODEL_PUB_STATUS, (u8 *)&pub_st, sizeof(pub_st)-FIX_SIZE(sig_model), adr_dst);
}

u8 mesh_add_pkt_model_pub(u16 ele_adr, u32 model_id, int sig_model, u16 adr_src)
{
    u8 st = ST_SUCCESS;
    u8 model_idx = 0;
    model_common_t *p_model = (model_common_t *)mesh_find_ele_resource_in_model(ele_adr, model_id, sig_model, &model_idx, 0);

    if(!p_model){   // response error status after this function.
    	st = ST_INVALID_MODEL;
    }else if(p_model->no_pub){
    	st = ST_INVALID_MODEL;	// model not support publish
    }else{
        mesh_cfg_model_pub_st_t pub_st;
        pub_st.status = ST_SUCCESS;
        pub_st.set.ele_adr = ele_adr;
        mesh_copy_pub_adr_par(&pub_st.set.pub_adr, &p_model->pub_adr);
        pub_st.set.model_id = model_id;
        
        mesh_tx_cmd_rsp_cfg_model(CFG_MODEL_PUB_STATUS, (u8 *)&pub_st, sizeof(pub_st)-FIX_SIZE(sig_model), adr_src);
    }

    return st;
}

void mesh_set_model_ele_adr(u16 ele_adr, u32 model_id, int sig_model)
{
	model_common_t *p_model;
	u8 model_idx = 0;
	p_model = (model_common_t *)mesh_find_ele_resource_in_model(ele_adr,model_id,sig_model,&model_idx, 1);
	if(p_model){
		p_model->ele_adr = ele_adr;
	}
}

u8 get_transition_step_res(u32 transition_100ms)
{
	trans_time_t trans_time;
	trans_time.val = 0;
	if(transition_100ms < 64 * RES_100MS){
		trans_time.steps = transition_100ms;
		trans_time.res = TRANSITION_STEP_RES_100MS;
	}else if(transition_100ms < 64 * RES_1S){
		trans_time.steps = transition_100ms / RES_1S;
		trans_time.res = TRANSITION_STEP_RES_1S;
	}else if(transition_100ms < 64 * RES_10S){
		trans_time.steps = transition_100ms / RES_10S;
		trans_time.res = TRANSITION_STEP_RES_10S;
	}else if(transition_100ms < 64 * RES_10MIN){
		trans_time.steps = transition_100ms / RES_10MIN;
		trans_time.res = TRANSITION_STEP_RES_10MIN;
	}else{
		trans_time.val = 0xff;  // step 0x3f means higher than 0x3e
	}

	return trans_time.val;
}

u32 get_transition_100ms(trans_time_t *p_trans_time)
{
	u32 step_res_100ms = RES_10MIN;	// 10 min;
	if(TRANSITION_STEP_RES_100MS == p_trans_time->res){
		step_res_100ms = RES_100MS;
	}else if(TRANSITION_STEP_RES_1S == p_trans_time->res){
		step_res_100ms = RES_1S;
	}else if(TRANSITION_STEP_RES_10S == p_trans_time->res){
		step_res_100ms = RES_10S;
	}else if(TRANSITION_STEP_RES_10MIN == p_trans_time->res){
		step_res_100ms = RES_10MIN;	// 10 min;
	}
	return (p_trans_time->steps * step_res_100ms);
}

void mesh_tx_pub_period(u16 ele_adr, u32 model_id, int sig_model)
{
	model_common_t *p_model;
	u8 model_idx = 0;
	p_model = (model_common_t *)mesh_find_ele_resource_in_model(ele_adr,model_id,sig_model,&model_idx, 0);
	if(p_model){
		
		if(p_model->pub_adr){
		    int pub_flag = 0;
		    if(!light_pub_model_priority || ((u8 *)p_model == light_pub_model_priority)){
    		    if(p_model->pub_trans_flag && (ST_PUB_TRANS_ALL_OK == light_pub_trans_step)){
                    light_pub_model_priority = 0;
                    pub_flag = 1;
					// publish sts change publish
					#if !WIN32
					pub_step_proc_cb(1,p_model,model_id);
					#endif
    		    }
		    }
			// normal publish 
			#if !WIN32
			if(pub_step_proc_cb(0,p_model,model_id)){
				return;
			}
			#endif
		    if(p_model->pub_par.period.steps && p_model->cb_pub_st){
    			u32 pub_interval_ms = get_mesh_pub_interval_ms(model_id, sig_model, &p_model->pub_par.period);
    			if(pub_flag || clock_time_exceed_ms(p_model->cb_tick_ms, pub_interval_ms)){
    			    // can't make tick greater than now.
    				p_model->cb_tick_ms = clock_time_ms() - get_random_delay_pub_tick_ms(pub_interval_ms);
    				pub_flag = 1;
    			}
			}

			if(pub_flag){
                if((0 == p_model->cb_pub_st) || p_model->cb_pub_st(model_idx) != -1){
					p_model->pub_trans_flag = 0;
				}
			}
		}
	}
}

int mesh_sec_msg_dec_virtual_ll(u16 ele_adr, u32 model_id, int sig_model, 
			u8 *key, u8 *nonce, u8 *dat, int n, int mic_length, u16 adr_dst, u8 *dat_org)
{
	u8 model_idx = 0;
	model_common_t *p_model;
	p_model = (model_common_t *)mesh_find_ele_resource_in_model(ele_adr, model_id, sig_model, &model_idx, 0);
	if(p_model){
		foreach_arr(i,p_model->sub_list){
			if(p_model->sub_list[i] == adr_dst){
				if(!mesh_sec_msg_dec_ll(key, nonce, dat, n, p_model->sub_uuid[i], 16, mic_length)){
					return 0;
				}else{
					memcpy(dat, dat_org, n);	// recover data
				}
			}
		}
	}
	return -1;
}

int mesh_is_existed_share_model(mesh_model_id_t *md_out, u32 model_id, int sig_model)
{
	if(sig_model){
		u32 model_id_share = -1;
		if(SIG_MD_G_ONOFF_S == model_id){
			model_id_share = SIG_MD_G_LEVEL_S;
		}else if(SIG_MD_G_LEVEL_S == model_id){
			model_id_share = SIG_MD_G_ONOFF_S;
		}else if(SIG_MD_G_ONOFF_C == model_id){
			model_id_share = SIG_MD_G_LEVEL_C;
		}else if(SIG_MD_G_LEVEL_C == model_id){
			model_id_share = SIG_MD_G_ONOFF_C;
		}
		if(-1 != model_id_share){
			md_out->sig_model = 1;
			md_out->id = model_id_share;
			return 1;
		}
	}
	
	return 0;
}

u8 mesh_pub_par_check(model_common_t *p_model, mesh_cfg_model_pub_set_t *p_pub_set){
	u8 st = ST_SUCCESS;
	if(p_pub_set->pub_par.rfu != 0){
		p_pub_set->pub_par.rfu = 0;
	}
	
	if(p_pub_set->pub_adr == 0){
		memset((u8 *)&p_pub_set->pub_par, 0x00, sizeof(mesh_model_pub_par_t));
		return ST_SUCCESS;
	}

	if(!is_exist_bind_key(p_model, p_pub_set->pub_par.appkey_idx)){
		st = ST_INVALID_APPKEY;
	}else if(p_pub_set->pub_par.credential_flag == 1){
		// st = ST_FEATURE_NOT_SUPPORT;
	}
	
	return st;
}

u8 mesh_pub_search_and_set(u16 ele_adr, u8 *pub_set, u32 model_id, int sig_model, int vr_flag)
{
    u8 st = ST_UNSPEC_ERR;
    mesh_adr_list_t adr_list;
    st = find_ele_support_model_and_match_dst(&adr_list, ele_adr, model_id, sig_model);
    LOG_MSG_LIB(TL_LOG_NODE_SDK,0, 0,"xxx,adr:0x%04x,st:0x%x,",ele_adr,st);
	if(st != ST_SUCCESS) return st;

	int change_falg = 0;
    if(adr_list.adr_cnt){
        u8 model_idx = 0;
        model_common_t *p_model = (model_common_t *)mesh_find_ele_resource_in_model(ele_adr, model_id, sig_model,&model_idx, 0);
        if(p_model && (!p_model->no_pub)){
        	if(vr_flag){
				mesh_cfg_model_pub_virtual_set_t *p_pub_set_vr = (mesh_cfg_model_pub_virtual_set_t *)pub_set;
				mesh_cfg_model_pub_set_t pub_set_temp;
				pub_set_temp.pub_adr = cala_vir_adr_by_uuid(p_pub_set_vr->pub_uuid);
				memcpy(&pub_set_temp.pub_par, &p_pub_set_vr->pub_par, sizeof(mesh_model_pub_par_t));
				st = mesh_pub_par_check(p_model, &pub_set_temp);
				if(ST_SUCCESS == st){
				    change_falg = mesh_save_pub_and_refresh_tick(p_model, &pub_set_temp, p_pub_set_vr->pub_uuid);
				}
        	}else{
				mesh_cfg_model_pub_set_t *p_pub_set = (mesh_cfg_model_pub_set_t *)pub_set;
	        	if(is_virtual_adr(p_pub_set->pub_adr)){
                    LOG_MSG_LIB(TL_LOG_NODE_SDK,0, 0,"yyy:0x%x,",st);
	        		st = ST_INVALID_ADR;
	        	}else{
	        		st = mesh_pub_par_check(p_model, p_pub_set);
					if(ST_SUCCESS == st){
                        change_falg = mesh_save_pub_and_refresh_tick(p_model, p_pub_set, 0);
		            }
	            }
            }
        }else{
			st = ST_INVALID_PUB_PAR;
        }
    }

    if((ST_SUCCESS == st) && change_falg){
		mesh_model_store(sig_model, model_id);
    }
    LOG_MSG_LIB(TL_LOG_NODE_SDK,0, 0,"zzz:0x%x,",st);

	return st;
}

int mesh_pub_set(u8 *par, mesh_cb_fun_par_t *cb_par, u32 model_id, int sig_model, int vr_flag)
{
    u8 st = ST_SUCCESS;
    mesh_cfg_model_pub_set_t *p_pub_set = (mesh_cfg_model_pub_set_t *)par;
	u16 ele_adr = p_pub_set->ele_adr;

    if(is_unicast_adr(ele_adr)){
        st = mesh_pub_search_and_set(ele_adr, (u8 *)p_pub_set, model_id, sig_model, vr_flag);
        if(st == ST_SUCCESS){
            // retrieve the parameters
            st = mesh_add_pkt_model_pub(ele_adr, model_id, sig_model, cb_par->adr_src);
            if(st == ST_SUCCESS){
                //refresh cb_ticks
                if(SIG_MD_HEALTH_SERVER == model_id){
                    model_sig_health.srv.com.cb_tick_ms = clock_time_ms();
                }
                #if 0 // SUBSCRIPTION_PUBLISH_SHARE_EN
                mesh_model_id_t md_bind;    
                if(mesh_is_existed_share_model(&md_bind, model_id, sig_model)){
                    mesh_pub_search_and_set(ele_adr, (u8 *)p_pub_set, md_bind.id, md_bind.sig_model, vr_flag);
                }
                #endif
            }
        }
    }else{
        st = ST_INVALID_ADR;
    }

	if(st != ST_SUCCESS){
		mesh_rsp_err_st_pub_status(st, ele_adr, model_id, sig_model, cb_par->adr_src);
	}

	return 0;
}

int mesh_cmd_sig_cfg_model_pub_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int sig_model = (sizeof(mesh_cfg_model_pub_set_t) - 2 == par_len);
    mesh_cfg_model_pub_set_t *p_pub_set = (mesh_cfg_model_pub_set_t *)par;
    u32 model_id = sig_model ? (p_pub_set->model_id & 0xFFFF) : p_pub_set->model_id;
    return mesh_pub_set(par, cb_par, model_id, sig_model, 0);
}

int mesh_cmd_sig_cfg_model_pub_set_vr(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int sig_model = (sizeof(mesh_cfg_model_pub_virtual_set_t) - 2 == par_len);
    mesh_cfg_model_pub_virtual_set_t *p_pub_set = (mesh_cfg_model_pub_virtual_set_t *)par;
    u32 model_id = sig_model ? (p_pub_set->model_id & 0xFFFF) : p_pub_set->model_id;
    return mesh_pub_set(par, cb_par, model_id, sig_model, 1);
}

int mesh_cmd_sig_cfg_model_pub_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    u8 st = ST_UNSPEC_ERR;
    int sig_model = (sizeof(mesh_cfg_model_pub_get_t) - 2 == par_len);
    mesh_cfg_model_pub_get_t *p_pub_get = (mesh_cfg_model_pub_get_t *)par;
    u32 model_id = sig_model ? (p_pub_get->model_id & 0xFFFF) : p_pub_get->model_id;
    
    if(is_unicast_adr(p_pub_get->ele_adr)){
        mesh_adr_list_t adr_list;
        st = find_ele_support_model_and_match_dst(&adr_list, p_pub_get->ele_adr, model_id, sig_model);
    	
        if((adr_list.adr_cnt)){
            st = mesh_add_pkt_model_pub(p_pub_get->ele_adr, model_id, sig_model, cb_par->adr_src);
        }
    }else{
        st = ST_INVALID_ADR;
    }

	if(st != ST_SUCCESS){
        mesh_rsp_err_st_pub_status(st, p_pub_get->ele_adr, model_id, sig_model, cb_par->adr_src);
	}
    
    return 0;
}

int mesh_cmd_sig_cfg_model_pub_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_rsp_sub_status(u8 st, mesh_cfg_model_sub_set_t *p_set, int sig_model, u16 adr_dst)
{
    mesh_cfg_model_sub_status_t sub_status_rsp;
    sub_status_rsp.status = st;
    memcpy(&sub_status_rsp.set, p_set, sizeof(sub_status_rsp.set));
    
    return mesh_tx_cmd_rsp_cfg_model(CFG_MODEL_SUB_STATUS, (u8 *)&sub_status_rsp, sizeof(sub_status_rsp)-FIX_SIZE(sig_model), adr_dst);
}

#define SUB_ADR_DEF_VAL			(0xffff)
#define IS_VALID_SUB_ADR(adr)	(adr && (adr != 0xffff))

enum{
	SUB_SAVE_ERR = 0,
	SUB_SAVE_0000,		//  found the first 0x0000
	SUB_SAVE_FFFF,		//  found the first 0xffff
};

void mesh_sub_par_set(model_common_t *p_model, u32 sub_index, u16 sub_adr, u8 *uuid)
{
	p_model->sub_list[sub_index] = sub_adr;
	u32 len_uuid = sizeof(p_model->sub_uuid[sub_index]);
	if((0 == sub_adr)||(SUB_ADR_DEF_VAL == sub_adr)){
		memset(p_model->sub_uuid[sub_index], sub_adr, len_uuid);
	}else{
		if(uuid){
			memcpy(p_model->sub_uuid[sub_index], uuid, len_uuid);
		}else{
			memset(p_model->sub_uuid[sub_index], SUB_ADR_DEF_VAL, len_uuid);
		}
	}
}

static inline void mesh_sub_par_set2def_val(model_common_t *p_model, u32 sub_index)
{
	mesh_sub_par_set(p_model, sub_index, SUB_ADR_DEF_VAL, 0);
}

static inline void mesh_sub_par_del(model_common_t *p_model, u32 sub_index)
{
	mesh_sub_par_set(p_model, sub_index, 0, 0);
}

int is_existed_sub_adr(model_common_t *p_model, u32 sub_index, u16 sub_adr, u8 *uuid)
{
	return (sub_adr == p_model->sub_list[sub_index]
		&& (!uuid || ((0 == memcmp(uuid, p_model->sub_uuid[sub_index],16)))));
}

u8 mesh_cmd_sig_cfg_model_sub_set2(u16 op, u16 sub_adr, u8 *uuid, model_common_t *p_model, u32 model_id, int sig_model)
{
    int save_flash = 1;
    u8 st = ST_UNSPEC_ERR;
    if((CFG_MODEL_SUB_ADD == op)||(CFG_MODEL_SUB_VIRTUAL_ADR_ADD == op)){
        int add_ok = 0;
        foreach(i,SUB_LIST_MAX){
        	if(is_existed_sub_adr(p_model, i, sub_adr, uuid)){
				add_ok = 1;
				save_flash = 0;
				break;
        	}
        }

        // init empty subscription list to 0xffff, is used to save a new subscription adr.
        if(0 == add_ok){
			u32 save_type = SUB_SAVE_ERR;
			u32 save_index = 0;
			#if FEATURE_LOWPOWER_EN
            foreach(i,SUB_LIST_MAX_LPN)
			#else
            foreach(i,SUB_LIST_MAX)
            #endif
            {
				u16 sub = p_model->sub_list[i];
				if(0 == sub){
					if(SUB_SAVE_ERR == save_type){
						save_type = SUB_SAVE_0000;
						save_index = i; 	// the first empty
					}
				}else if(0xffff == sub){
					save_type = SUB_SAVE_FFFF;
					save_index = i;
					break;	// exist 0xffff, can save in this adr of flash directly.
				}
			}
			
			if(SUB_SAVE_ERR == save_type){
				// set ST_INSUFFICIENT_RES later
			}else{
				if(SUB_SAVE_FFFF == save_type){
				}else if(SUB_SAVE_0000 == save_type){
					foreach(i,SUB_LIST_MAX){
						if(0 == p_model->sub_list[i]){
							mesh_sub_par_set2def_val(p_model, i); // because it will use a new flash zone later.
						}
					}
				}
				
				mesh_sub_par_set(p_model, save_index, sub_adr, uuid);
				add_ok = 1;
			}
        }
        st = add_ok ? ST_SUCCESS : ST_INSUFFICIENT_RES;
    }else if((CFG_MODEL_SUB_DEL == op)||(CFG_MODEL_SUB_VIRTUAL_ADR_DEL == op)){
        foreach(i,SUB_LIST_MAX){
            if(is_existed_sub_adr(p_model, i, sub_adr, uuid)){
                mesh_sub_par_del(p_model, i);
            }
        }
        st = ST_SUCCESS;
    }else if(((CFG_MODEL_SUB_OVER_WRITE == op)||(CFG_MODEL_SUB_VIRTUAL_ADR_OVER_WRITE == op))
    	   ||(CFG_MODEL_SUB_DEL_ALL == op)){
        memset(p_model->sub_list, SUB_ADR_DEF_VAL, sizeof(p_model->sub_list));
        memset(p_model->sub_uuid, SUB_ADR_DEF_VAL, sizeof(p_model->sub_uuid));
        if(CFG_MODEL_SUB_DEL_ALL != op){
        	mesh_sub_par_set(p_model, 0, sub_adr, uuid);
        }
        st = ST_SUCCESS;
    }

	if(ST_SUCCESS == st){
	    if(save_flash){
		    mesh_model_store(sig_model, model_id);
		}
		rf_link_light_event_callback(LGT_CMD_SET_SUBSCRIPTION);
		#if FEATURE_LOWPOWER_EN
		lpn_subsc_list_update_by_sub_set_cmd(op, sub_adr);
		#endif
	}

    return st;
}

u8 mesh_sub_search_and_set(u16 op, u16 ele_adr, u16 sub_adr, u8 *uuid, u32 model_id, int sig_model)
{
    u8 st = ST_UNSPEC_ERR;
    mesh_adr_list_t adr_list;
    st = find_ele_support_model_and_match_dst(&adr_list, ele_adr, model_id, sig_model);
	if(st != ST_SUCCESS) return st;
	
    if((adr_list.adr_cnt)){
        u8 model_idx = 0;
        model_common_t *p_model = (model_common_t *)mesh_find_ele_resource_in_model(ele_adr, model_id, sig_model,&model_idx, 0);
        if(p_model && (!p_model->no_sub)){
        	if(((CFG_MODEL_SUB_DEL_ALL != op)&&(0 == uuid)&&(!is_group_adr(sub_adr)))
        	|| (is_fixed_group(sub_adr))){
        		st = ST_INVALID_ADR;
        	}else{
				st = mesh_cmd_sig_cfg_model_sub_set2(op, sub_adr, uuid, p_model, model_id, sig_model);
            }
        }else{
            st = ST_NOT_SUB_MODEL;
        }
    }
    
	return st;
}

int mesh_cmd_sig_cfg_model_sub_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    u8 st = ST_UNSPEC_ERR;
    mesh_cfg_model_sub_set_t sub_set;
    memset(&sub_set, 0, sizeof(sub_set));
    mesh_cfg_model_sub_set_t *p_sub_set = &sub_set;
    p_sub_set->ele_adr = ((mesh_cfg_model_sub_set_t *)par)->ele_adr;
    
    if(!is_unicast_adr(p_sub_set->ele_adr)){
        return -1;
    }
    u16 op = cb_par->op;
    int sig_model;
    u32 model_id;
    u8 *uuid = 0;
	u8 cfg_model_virtual=0;
    if(CFG_MODEL_SUB_DEL_ALL == op){
        mesh_cfg_model_sub_del_all_t *p_sub_del_all = (mesh_cfg_model_sub_del_all_t *)par;
        sig_model = (sizeof(mesh_cfg_model_sub_del_all_t) - 2 == par_len);
        model_id = sig_model ? (p_sub_del_all->model_id & 0xFFFF) : p_sub_del_all->model_id;
        
        p_sub_set->model_id = model_id;
    }else if((CFG_MODEL_SUB_VIRTUAL_ADR_ADD == op)
    	   ||(CFG_MODEL_SUB_VIRTUAL_ADR_DEL == op)
    	   ||(CFG_MODEL_SUB_VIRTUAL_ADR_OVER_WRITE == op)){
    	cfg_model_virtual = 1;
        mesh_cfg_model_sub_virtual_set_t *p_set_vr = (mesh_cfg_model_sub_virtual_set_t *)par;
        sig_model = (sizeof(mesh_cfg_model_sub_virtual_set_t) - 2 == par_len);
        model_id = sig_model ? (p_set_vr->model_id & 0xFFFF) : p_set_vr->model_id;
        
        p_sub_set->sub_adr = cala_vir_adr_by_uuid(p_set_vr->sub_uuid);
        p_sub_set->model_id = model_id;
        uuid = p_set_vr->sub_uuid;
    }else{
        memcpy(p_sub_set, par, par_len);
        sig_model = (sizeof(mesh_cfg_model_sub_set_t) - 2 == par_len);
        model_id = sig_model ? (p_sub_set->model_id & 0xFFFF) : p_sub_set->model_id;
    }

    st = mesh_sub_search_and_set(op, p_sub_set->ele_adr, p_sub_set->sub_adr, uuid, model_id, sig_model);
	if(ST_SUCCESS == st){
		share_model_sub_by_rx_cmd(op, p_sub_set->ele_adr, p_sub_set->sub_adr,cb_par->adr_dst, uuid, model_id, sig_model);
	}
	
	if((ST_SUCCESS != st)&&cfg_model_virtual){
		p_sub_set->sub_adr = 0;
	}
	return mesh_cmd_sig_cfg_model_sub_cb(st,p_sub_set,sig_model,cb_par->adr_src);
}

int mesh_cmd_sig_cfg_model_sub_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_rsp_err_st_sub_list(u8 st, u16 ele_adr, u32 model_id, int sig_model, u16 adr_dst)
{
    mesh_cfg_model_sub_list_vendor_t sub_list_rsp;
    
    memset(&sub_list_rsp, 0, sizeof(sub_list_rsp));
    sub_list_rsp.status = st;
    sub_list_rsp.ele_adr = ele_adr;
    sub_list_rsp.model_id = model_id;
    u32 len_rsp = sig_model ? 5 : 7;
    u16 op = sig_model ? CFG_SIG_MODEL_SUB_LIST : CFG_VENDOR_MODEL_SUB_LIST;
    return mesh_tx_cmd_rsp_cfg_model(op, (u8 *)&sub_list_rsp, len_rsp, adr_dst);
}

int mesh_sub_list_get(u8 *p_list, model_common_t *p_model)
{    
    int cnt = 0;
    u16 list[SUB_LIST_MAX];
    foreach(i, SUB_LIST_MAX){
        if(p_model->sub_list[i] && (0xffff != p_model->sub_list[i])){
            list[cnt++] = p_model->sub_list[i];
        }
    }
    memcpy(p_list, list, cnt * 2);

    return cnt;
}

u8 mesh_add_pkt_model_sub_list(u16 ele_adr, u32 model_id, int sig_model, u16 adr_src)
{
    u8 st = ST_UNSPEC_ERR;
    u8 model_idx = 0;
    model_common_t *p_model = (model_common_t *)mesh_find_ele_resource_in_model(ele_adr, model_id, sig_model,&model_idx, 0);
    if(p_model && (!p_model->no_sub)){
		st = ST_SUCCESS;
        mesh_cfg_model_sub_list_vendor_t sub_list_rsp;
        sub_list_rsp.status = ST_SUCCESS;
        sub_list_rsp.ele_adr = ele_adr;
        sub_list_rsp.model_id = model_id;
        u8 *p_list = (u8 *)(sub_list_rsp.sub_adr) - FIX_SIZE(sig_model);
        u32 cnt = mesh_sub_list_get(p_list, p_model);

        u32 len_rsp = cnt*2 + OFFSETOF(mesh_cfg_model_sub_list_vendor_t,sub_adr) - FIX_SIZE(sig_model);
        u16 op = sig_model ? CFG_SIG_MODEL_SUB_LIST : CFG_VENDOR_MODEL_SUB_LIST;
        mesh_tx_cmd_rsp_cfg_model(op, (u8 *)&sub_list_rsp, len_rsp, adr_src);
    }else{
    	st = ST_NOT_SUB_MODEL;
    }
    
    return st;
}

int mesh_cmd_sig_sig_model_sub_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    u8 st = ST_UNSPEC_ERR;
    mesh_cfg_model_sub_get_sig_t *p_sub_get = (mesh_cfg_model_sub_get_sig_t *)par;

    if(!is_unicast_adr(p_sub_get->ele_adr)){
        return -1;
    }
    if(sizeof(mesh_cfg_model_sub_get_sig_t) == par_len){
    
        mesh_adr_list_t adr_list;
        st = find_ele_support_model_and_match_dst(&adr_list, p_sub_get->ele_adr, p_sub_get->model_id, 1);
        if((adr_list.adr_cnt)){
            st = mesh_add_pkt_model_sub_list(p_sub_get->ele_adr, p_sub_get->model_id, 1, cb_par->adr_src);
        }
    }
    
    if(ST_SUCCESS != st){
        mesh_rsp_err_st_sub_list(st, p_sub_get->ele_adr, p_sub_get->model_id, 1, cb_par->adr_src);
    }
    return 0;
}

int mesh_cmd_sig_sig_model_sub_list(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_vendor_model_sub_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    u8 st = ST_UNSPEC_ERR;
    mesh_cfg_model_sub_get_vendor_t *p_sub_get = (mesh_cfg_model_sub_get_vendor_t *)par;

    if(!is_unicast_adr(p_sub_get->ele_adr)){
        return -1;
    }
    if(sizeof(mesh_cfg_model_sub_get_vendor_t) == par_len){
        mesh_adr_list_t adr_list;
        st = find_ele_support_model_and_match_dst(&adr_list, p_sub_get->ele_adr, p_sub_get->model_id, 0);
        if((adr_list.adr_cnt)){
            st = mesh_add_pkt_model_sub_list(p_sub_get->ele_adr, p_sub_get->model_id, 0, cb_par->adr_src);
        }
    }
    
    if(ST_SUCCESS != st){
        mesh_rsp_err_st_sub_list(st, p_sub_get->ele_adr, p_sub_get->model_id, 0, cb_par->adr_src);
    }
    return 0;
}

int mesh_cmd_sig_vendor_model_sub_list(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

//-------- config model end

int is_subscription_adr(model_common_t *p_model, u16 adr)
{
    foreach(i,SUB_LIST_MAX){
        if(adr == p_model->sub_list[i]){
            return 1;
        }
    }
    return 0;
}

int is_support_op(mesh_op_resource_t *op_res, u16 op, u16 adr_dst, u8 tx_flag)
{
    int support_flag = 0;
    //mesh_cmd_ac_sig2_t *p_ac = CONTAINER_OF(par,mesh_cmd_ac_sig2_t,data[0]);
    if(0 == mesh_search_model_id_by_op(op_res, op, tx_flag)){
        if(!is_unicast_adr(adr_dst)){
        	if(is_cfg_model(op_res->id, op_res->sig)){
        	    if(!is_actived_factory_test_mode() || !is_valid_cfg_op_when_factory_test(op)){
        	        #if (! __PROJECT_MESH_PRO__)
                    return 0;   // must unicast for config model
                    #endif
                }
            }
        }
        
        if(op_res->sig){
            op_res->id &= 0xffff;
        }
    
        mesh_adr_list_t adr_list;
        find_ele_support_model_and_match_dst(&adr_list, adr_dst, op_res->id, op_res->sig);
        if(adr_list.adr_cnt){
            support_flag = 1;
            foreach(i,adr_list.adr_cnt){
                u8 model_idx = 0;
                u8 *model = mesh_find_ele_resource_in_model(adr_list.adr[i], op_res->id, op_res->sig,&model_idx, 0);
				if(model){
                    op_res->model_idx[op_res->model_cnt] = model_idx;
                    op_res->model[op_res->model_cnt++] = model;
                }
            }
        }else{
            if((0 == tx_flag) && op_res->status_cmd){
                support_flag = 1;
            }
        }
    }else{
        #if VC_SUPPORT_ANY_VENDOR_CMD_EN
        if(!IS_VENDOR_OP(op))
        #endif
        {
            LOG_MSG_INFO(TL_LOG_COMMON,0,0,"not support op, TX: send with unreliable flow. RX: discard!", 0);
        }
    }
    return support_flag;
}

u8 * mesh_get_model_par_by_op_dst(u16 op, u16 ele_adr)
{
	u8 *model = 0;
	mesh_op_resource_t op_res;
	if(0 == mesh_search_model_id_by_op(&op_res, op, 1)){
		u8 model_idx = 0;
		model = mesh_find_ele_resource_in_model(ele_adr, op_res.id, op_res.sig,&model_idx, 0);
	}
	return model;
}

u16 mesh_get_ele_adr(u32 ele_idx)
{
    return (ele_adr_primary + ele_idx);     // setting later
}

u32 get_op_rsp(u16 op)
{
    mesh_op_resource_t op_res;
    if(0 == mesh_search_model_id_by_op(&op_res, op, 1)){
        return op_res.op_rsp;
    }
    return STATUS_NONE;
}

int is_reliable_cmd(u16 op, u32 vd_op_rsp)
{
#if 1
    #if VC_SUPPORT_ANY_VENDOR_CMD_EN
    if(IS_VENDOR_OP(op)){
        return ((vd_op_rsp & 0xff) != STATUS_NONE_VENDOR_OP_VC);
    }
    #endif
    
    return (STATUS_NONE != get_op_rsp(op));
#else
    int reliable = 1;
    switch(op){
        default:
            break;
        // status response of config model have been use mesh_tx_cmd_unrel_status_primary() directly.
        case HEALTH_FAULT_CLEAR_NOACK:
        case HEALTH_PERIOD_SET_NOACK:
        case HEALTH_FAULT_TEST_NOACK:
        
        case G_ONOFF_SET_NOACK:
        case G_LEVEL_SET_NOACK:
        case G_ON_POWER_UP_SET_NOACK:
        case G_POWER_LEVEL_SET_NOACK:
        case G_POWER_DEF_SET_NOACK:
        case G_POWER_LEVEL_RANGE_SET_NOACK:
            reliable = 0;
            break;
    }

    return reliable;
#endif
}

int mesh_bulk_cmd2model(u8 *par, u32 len){
    int err = -1;
    mesh_bulk_cmd2model_t *p_bulk = (mesh_bulk_cmd2model_t *)par;
    if(p_bulk->ele_idx < g_ele_cnt){
        u16 adr_src = mesh_get_ele_adr(p_bulk->ele_idx);
        u16 op = p_bulk->op;
        
        mesh_op_resource_t op_res;
        if(is_unicast_adr(adr_src) && is_support_op(&op_res, op, adr_src, 1)){
            if(op_res.model_cnt && op_res.model[0]){
                model_common_t *p_model = (model_common_t *)op_res.model[0];
                if(!p_model->no_pub){
                    u32 par_len = len - OFFSETOF(mesh_bulk_cmd2model_t,par);
                    return mesh_tx_cmd2normal(op, p_bulk->par, par_len, p_model->ele_adr, p_model->pub_adr, p_bulk->rsp_max);
                }
            }
        }
    }

    if(err){
        mesh_tx_cmd_status_report(err, 0, 0);
    }

    return err;
}

#define MIN_LEN_APP_VENDOR_INI  (OFFSETOF(mesh_bulk_cmd_par_t, op) + OFFSETOF(mesh_vendor_par_ini_t,par))

int mesh_bulk_cmd(mesh_bulk_cmd_par_t *par, u32 len){
    int err = -1;
    if(len < OFFSETOF(mesh_bulk_cmd_par_t,par)){
        return err;
    }
    
    mesh_bulk_cmd_par_t *p_cmd = (mesh_bulk_cmd_par_t *)par;
    
    int valid_cmd = 0;
    u32 par_len = len - OFFSETOF(mesh_bulk_cmd_par_t,par);
    material_tx_cmd_t mat;
    u8 nk_arr_idx = get_nk_arr_idx(p_cmd->nk_idx);
    u8 ak_arr_idx = get_ak_arr_idx(nk_arr_idx, p_cmd->ak_idx);
    if((nk_arr_idx >= NET_KEY_MAX)||(ak_arr_idx >= APP_KEY_MAX)){
        nk_arr_idx = ak_arr_idx = 0;    // use first valid key later
    }
    set_material_tx_cmd(&mat, p_cmd->op, p_cmd->par, par_len, ele_adr_primary, p_cmd->adr_dst, p_cmd->retry_cnt, p_cmd->rsp_max, 0, nk_arr_idx, ak_arr_idx, 0);
    // mat.seg_must = 1;    // for test
    
    u32 op_tpye = GET_OP_TYPE(p_cmd->op);
    if(OP_TYPE_SIG1 == op_tpye){    // default
        if(len >= OFFSETOF(mesh_bulk_cmd_par_t,par)){
            //mesh_cmd_ac_sig1_t *ac_sig1 = (mesh_cmd_ac_sig1_t *)(&p_cmd->op);
            valid_cmd = 1;
        }
    }else if(OP_TYPE_SIG2 == op_tpye){
        if(len >= OFFSETOF(mesh_bulk_cmd_par_t,par) + 1){
            valid_cmd = 1;
            mesh_cmd_ac_sig2_t *ac_sig2 = (mesh_cmd_ac_sig2_t *)(&p_cmd->op);
            mat.op = ac_sig2->op;
            mat.p_ac = ac_sig2->data;
            mat.len_ac = par_len - 1;
        }
    }else/* if(OP_TYPE_VENDOR == op_tpye)*/{
        if(len >= MIN_LEN_APP_VENDOR_INI){
            valid_cmd = 1;
            mesh_vendor_par_ini_t *p_vd_par = (mesh_vendor_par_ini_t *)(&p_cmd->op);
            
            mat.op = p_vd_par->op;
            mat.p_ac = p_vd_par->par;
            mat.len_ac = len - MIN_LEN_APP_VENDOR_INI;
            mat.op_rsp = p_vd_par->op_rsp | (p_vd_par->vendor_id << 8);
            mat.tid_pos_vendor_app = p_vd_par->tid_pos;
        }
    }

#if VC_APP_ENABLE
    if(valid_cmd){
        valid_cmd = is_support_op_dst_VC_APP(mat.op, p_cmd->adr_dst);
    }
#endif

    if(valid_cmd){
        err = mesh_tx_cmd(&mat);
    }

    if(err){
	    LOG_MSG_ERR(TL_LOG_MESH,0, 0 ,"mesh_bulk_cmd failed",0);
        mesh_tx_cmd_status_report(err, (u8 *)p_cmd, len);
    }

    return err;
}

int mesh_bulk_set_par(u8 *par, u32 len){
    int err = -1;
    mesh_bulk_set_par_t *p_bulk = (mesh_bulk_set_par_t *)par;
    if(len >= OFFSETOF(mesh_bulk_set_par_t, par)){
        err = 0;
        if(PAR_TYPE_PUB_ADR == p_bulk->type){
            #if MD_CFG_CLIENT_EN
            u16 pub_adr = p_bulk->par[0] + (p_bulk->par[1] << 8);
            model_sig_cfg_c.com.pub_adr = pub_adr;
            #endif
        }else if(PAR_TYPE_ONLINE_ALL == p_bulk->type){
        }else if(PAR_TYPE_SET_MAC == p_bulk->type){
        	#if (PROJECT_SEL == PROJECT_VC_DONGLE)
			flash_erase_512K();
			flash_write_page(0x76000, 6, p_bulk->par);
			ModuleDlg_close();
        	#endif
        }else if(PAR_TYPE_SET_NODE_ADR == p_bulk->type){
        	mesh_set_ele_adr(p_bulk->par[0] + (p_bulk->par[1] << 8));
        }
    }
    
    if(err){
        mesh_tx_cmd_status_report(err, 0, 0);
    }

    return err;
}

int mesh_bulk_set_par2usb(u8 *par, u32 len){
    mesh_bulk_set_par_t *p_bulk = (mesh_bulk_set_par_t *)par;
    if(len >= OFFSETOF(mesh_bulk_set_par_t, par)){
    	p_bulk = p_bulk;
    	#if MESH_MONITOR_EN
		if(PAR2USB_MONITOR_MODE == p_bulk->type){
        	monitor_mode_en = !!p_bulk->par[0];
        }else if(PAR2USB_MONITOR_FILTER_SNO == p_bulk->type){
			monitor_filter_sno_en = !!p_bulk->par[0];
        }
        #endif
    }

    return 0;
}

// message cache process
#define SIZE_SNO        3

typedef struct{
	int sno;
	u16 src;
	u8 ivi      :1;
	u8          :7;
	u8  rfu[1];
}cache_buf_t;

#if DONGLE_PROVISION_EN
#define CACHE_BUF_MAX              100 // 8
#else
#define CACHE_BUF_MAX              MESH_NODE_MAX_NUM // 8
#endif

cache_buf_t cache_buf[CACHE_BUF_MAX];

STATIC_ASSERT(sizeof(cache_buf_t) % 4 == 0);   // for compare sno quickly

#ifndef WIN32
STATIC_ASSERT(CACHE_BUF_MAX <= 110);
#endif

void cache_init(u16 ele_adr)
{
    int del_all = !is_unicast_adr(ele_adr);
    if(del_all){
        memset(cache_buf, 0, sizeof(cache_buf));
    }else{
        foreach(i, CACHE_BUF_MAX){
            if(ele_adr == cache_buf[i].src){
                memset(&cache_buf[i], 0, sizeof(cache_buf[i]));
                break;
            }
        }
    }
	
	if((mesh_rx_seg_par.src != 0) &&  (0 == mesh_rx_seg_par.tick_last)){
		memset(&mesh_rx_seg_par, 0, sizeof(mesh_rx_seg_par));
	}
}

void add2cache(u8 *p, int idx){    // can not use op code,because it is encryption now
    mesh_cmd_nw_t *p_nw = (mesh_cmd_nw_t *)p;

    //memset(&cache_buf[idx], 0, sizeof(cache_buf_t));    // init
    cache_buf[idx].src = p_nw->src;
    cache_buf[idx].ivi = p_nw->ivi;
    cache_buf[idx].sno = 0;
    memcpy(&cache_buf[idx].sno, p_nw->sno, SIZE_SNO);
}

#define TEST_CASE_HBS_BV_05_EN	        0   // for pts7.3.0, seems like PTS error
#define TEST_CASE_FN_BV19_EN			0	// just for pts7.3.0
//#define TEST_CASE_NODE_CFG_CFGR_BV01_EN	0	// just for pts7.3.1

int is_exist_in_cache(u8 *p, u8 friend_key_flag, int save){
#if MESH_MONITOR_EN
    if(!monitor_filter_sno_en){
        return 0;
    }
#endif

    mesh_cmd_nw_t *p_nw = (mesh_cmd_nw_t *)p;   // only ivi, src and sno can be use when "save" is 0.
    foreach(i, CACHE_BUF_MAX){
        if(p_nw->src == cache_buf[i].src){
            int sno = 0;
            memcpy(&sno, p_nw->sno, SIZE_SNO);
            #if FEATURE_LOWPOWER_EN
			static u32 lpn_skip_cache_cnt;
			#endif
			int exist_old = 0;
            if((sno <= cache_buf[i].sno) && (p_nw->ivi == cache_buf[i].ivi))
            {
            	#if FEATURE_LOWPOWER_EN
            	int lpn_seg_cache_flag = is_in_mesh_friend_st_lpn() && friend_key_flag;
            	if((0 == save) && lpn_seg_cache_flag){
            	    return 0;   // if "save" is 0, means "lt" is still in encryption state.
            	}
            	
                mesh_cmd_bear_unseg_t *p_bear = CONTAINER_OF(p_nw,mesh_cmd_bear_unseg_t,nw);
                if((save && p_bear->lt.seg) && lpn_seg_cache_flag){
                	if(sno == cache_buf[i].sno){// because Friend send cache message if from the last one.
                    	if(lpn_skip_cache_cnt++ < 16){
                    		exist_old = 1;
                    	}
                    }
                }else
                #endif
                {
                    if((!factory_test_cache_not_equal_mode_en) || !is_actived_factory_test_mode() || (sno == cache_buf[i].sno)){
                	    exist_old = 1;
                	}
                }
            }
            
            if(!exist_old && save){
                #if FEATURE_LOWPOWER_EN
            	lpn_skip_cache_cnt = 0;		// init
            	#endif
                add2cache(p, i);            // update
            }
            
            return exist_old;
        }
    }

    if(save){
        //new device found
        static u8 cache_idx = 0;
    	#if TESTCASE_FLAG_ENABLE 		// for test case 4.9.13 [Ignore Replayed Message]
    	if(cache_idx >= CACHE_BUF_MAX){
    		return 1;
    	}
    	#endif
    	
        add2cache(p, cache_idx);
        cache_idx = (cache_idx + 1) % CACHE_BUF_MAX;
    }
    return 0;
}

// TID process
void add2tid_cache(u16 adr, u8 tid, int idx){
    mesh_tid.rx[idx].adr = adr;
    mesh_tid.rx[idx].tid = tid;
    mesh_tid.rx[idx].tick_100ms = clock_time_100ms();
}

u32 get_ele_idx(u16 ele_adr)
{
	if(is_own_ele(ele_adr)){
		return (ele_adr - ele_adr_primary);
	}
	return -1;
}

void is_cmd_with_tid_check_and_set(u16 ele_adr, u16 op, u8 *par, u32 par_len, u8 tid_pos_vendor_app)
{
    u8 tid_pos;
    if(is_cmd_with_tid(&tid_pos, op, tid_pos_vendor_app)){
        if(0 == par[tid_pos]){  // user can define TID
        	u32 ele_idx = get_ele_idx(ele_adr);
        	if(ele_idx < g_ele_cnt){
                mesh_tid.tx[ele_idx]++;
                if(tid_pos < par_len){
                    par[tid_pos] = mesh_tid.tx[ele_idx];
                }

                mesh_tid_save(ele_idx);
            }
        }
    }
}

int is_retransaction(u16 adr, u8 tid){
    foreach(i, RX_TID_CNT){
        if(adr == mesh_tid.rx[i].adr){
            if(tid == mesh_tid.rx[i].tid){
                return 1;
            }else{
                add2tid_cache(adr, tid, i);            // update
                return 0;
            }
        }
    }

    //new device found
    static u32 tid_cache_idx = 0;
    add2tid_cache(adr, tid, tid_cache_idx);
    tid_cache_idx = (tid_cache_idx + 1) % RX_TID_CNT;
    return 0;
}

void mesh_tid_timeout_check(){
    foreach(i, RX_TID_CNT){
        if(mesh_tid.rx[i].adr){
            if(clock_time_exceed_100ms(mesh_tid.rx[i].tick_100ms, 60)){
                add2tid_cache(0, 0, i);            // clear
            }
        }
    }
}

int is_rx_seg_old_seqAuth(u16 src_adr, u32 seqAuth)
{
	return ((src_adr == mesh_rx_seg_par.src) && (seqAuth <= mesh_rx_seg_par.seqAuth));
}

int mesh_add_seg_ack(const mesh_rx_seg_par_t *p_rx_seg_par)
{
    if(is_lpn_support_and_en && (mesh_lpn_par.FriAdr != p_rx_seg_par->src)){	// PTS: should ack
        return 0;   // no ack for low power node
    }
    
    if(is_own_ele(p_rx_seg_par->src)){
        return 0;   // no ack for itself message
    }

    int err;
    mesh_cmd_bear_unseg_t mesh_seg_ack;
    memset(&mesh_seg_ack, 0, sizeof(mesh_cmd_bear_unseg_t));
    //lt
    mesh_cmd_lt_ctl_seg_ack_t *p_lt_ack = (mesh_cmd_lt_ctl_seg_ack_t *)&mesh_seg_ack.lt;
    p_lt_ack->seqzero = p_rx_seg_par->seqzero;
    p_lt_ack->obo = p_rx_seg_par->obo;
    p_lt_ack->seg = 0;
    u8 len_lt_ack = sizeof(mesh_cmd_lt_ctl_seg_ack_t);
    memcpy(p_lt_ack->seg_map, &p_rx_seg_par->seg_map, sizeof(p_lt_ack->seg_map));
    //nw
    mesh_cmd_nw_t *p_nw_ack = (mesh_cmd_nw_t *)&mesh_seg_ack.nw;
    p_nw_ack->nid = p_rx_seg_par->nid;
    p_nw_ack->ivi = p_rx_seg_par->ivi;
    p_nw_ack->ctl = 1;
    p_nw_ack->ttl = model_sig_cfg_s.ttl_def;
#if FRIEND_MSG_TEST_EN
    p_nw_ack->ttl = TTL_TEST_ACK;
    if(mesh_adv_tx_cmd_sno < 0x014835){
        mesh_adv_tx_cmd_sno = 0x014835;
    }
#endif

    memcpy(p_nw_ack->sno, &mesh_adv_tx_cmd_sno, sizeof(p_nw_ack->sno));
    p_nw_ack->src = p_rx_seg_par->obo ? ele_adr_primary : p_rx_seg_par->dst;
    p_nw_ack->dst = p_rx_seg_par->src;

	mesh_match_type_t match_type;
    mesh_match_group_mac(&match_type, p_nw_ack->dst, -1, 0, 0);	// contronl command use -1 as op
	match_type.mat.nk_array_idx = mesh_rx_seg_par.nk_array_idx;
	match_type.mat.ak_array_idx = -1;

    err = mesh_tx_cmd_layer_network((u8 *)&mesh_seg_ack, len_lt_ack, SWAP_TYPE_LT_CTL_SEG_ACK, &match_type);
    return err;
}

int mesh_tx_next_segment_pkt(u8 tx_segO_next)
{
	u32 i = 0;
	for(i = tx_segO_next; i <= cmd_bear_seg_buf.lt.segN; ++i){
		if(0 == (mesh_tx_seg_par.seg_map_rec & BIT(i))){
			mesh_seg_add_pkt(&cmd_bear_seg_buf, mesh_tx_seg_par.match_type.mat.p_ac, mesh_tx_seg_par.len_ut, i); // 
			return 1;
		}
	}
	mesh_tx_seg_par.tx_segO_next = i;	// end
	return 0;
}

int is_tx_seg_one_round_ok()
{
	return (mesh_tx_seg_par.tx_segO_next > cmd_bear_seg_buf.lt.segN);
}

int check_and_send_next_segment_pkt()
{
	if(mesh_tx_seg_par.busy){
		if(!is_tx_seg_one_round_ok()){
			mesh_tx_next_segment_pkt(mesh_tx_seg_par.tx_segO_next);
		}else{
			if(!is_unicast_adr(cmd_bear_seg_buf.nw.dst)){
				mesh_tx_segment_finished();
				LOG_MSG_INFO(TL_LOG_MESH,0,0,"check_and_send_next_segment_pkt:segment tx finished by GROUP ADR ",0);
			}else{
			    mesh_tx_seg_par.tick_wait_ack = clock_time()|1;
			}
		}
	}
	return 0;
}

#if VC_CHECK_NEXT_SEGMENT_EN
u32 VC_tick_next_segment;
#define VC_CHECK_NEXT_SEGMENT_INTERVAL_MS	(((TRANSMIT_CNT_DEF+1)*(TRANSMIT_INVL_STEPS_DEF+1+1)*10)+10) // 1: base 0; 1: random delay

void VC_check_next_segment_pkt()
{
    u32 time_ms = VC_CHECK_NEXT_SEGMENT_INTERVAL_MS;
    #if DEBUG_SHOW_VC_SELF_EN
    if(is_mesh_ota_and_only_VC_update()){
        time_ms = 50;   // sent firmware data more faster.
    }
    #endif
    
	if(VC_tick_next_segment && clock_time_exceed(VC_tick_next_segment, time_ms * 1000)){
		VC_tick_next_segment = 0;
		check_and_send_next_segment_pkt();
	}
}
#endif

int mesh_rx_seg_ack_handle(int tx_flag)
{
	mesh_tx_seg_par.tx_segO_next = 0; // init
	
    int cnt = 0;
    for(u32 i = 0; i <= cmd_bear_seg_buf.lt.segN; ++i){
        if(0 == (mesh_tx_seg_par.seg_map_rec & BIT(i))){
            cnt++;
        }
    }
    
	if(tx_flag && cnt){
		mesh_tx_next_segment_pkt(0);
	}
	
    return cnt;
}


void mesh_seg_ack_poll()
{
    mesh_seg_ack_poll_tx();
    mesh_seg_ack_poll_rx();
}

#define DEBUG_SEG_RX        0

#if DEBUG_SEG_RX
#define B_SNO_CNT   40
typedef struct{
    u16 B_sno[B_SNO_CNT];
    u16 B_sno_ms[B_SNO_CNT];
    u32 tick_last;
    u32 idx;
}mesh_debug_seg_rx_t;

mesh_debug_seg_rx_t B_debug_seg_rx;

void debug_rx_record(u8 *sno)
{
    if(B_debug_seg_rx.idx == 0){
        B_debug_seg_rx.B_sno_ms[B_debug_seg_rx.idx] = 0;
    }else{
        B_debug_seg_rx.B_sno_ms[B_debug_seg_rx.idx] = (clock_time() - B_debug_seg_rx.tick_last)/(10*32000);
    }
    memcpy(&B_debug_seg_rx.B_sno[B_debug_seg_rx.idx], sno, 2);
    B_debug_seg_rx.tick_last = clock_time();
    B_debug_seg_rx.idx++;
}
#endif

int mesh_adr_check(u16 adr_src, u16 adr_dst)
{
    if((ADR_UNASSIGNED == adr_src)||(ADR_UNASSIGNED == adr_dst)||(adr_src & 0x8000)||((adr_dst>=ADR_FIXED_GROUP_START)&&(adr_dst<ADR_ALL_PROXY))){
        return -1;
    }

    return 0;
}

int mesh_adr_check_src_own_rx(u16 adr_src)
{
    return ((is_own_ele(adr_src)&&(!mesh_adv_txrx_self_en)) && (DEBUG_SHOW_VC_SELF_EN != SHOW_VC_SELF_NW_ENC));
}

static inline int is_mesh_ota_cmd(u16 op)
{
    u8 op_low = (op & 0xff);
    return ((0xB6 == op_low)||(0xB7 == op_low)||(OBJ_CHUNK_TRANSFER == op)||(OBJ_BLOCK_GET == op));
}

int is_cmd_skip_for_vc_self(u16 adr_src, u16 op)
{
    return ((adr_src == ele_adr_primary) && !is_mesh_ota_cmd(op));
}

u32 get_rx_seg_seqAuth(u8 *sno, u16 seqzero)
{
    #define SEQ_ZERO_LEN    (13)
    
    seqzero = seqzero & BIT_RNG(0, SEQ_ZERO_LEN - 1);
    u32 seqAuth = 0;
    memcpy(&seqAuth, sno, 3);
    if((seqAuth & BIT_RNG(0, SEQ_ZERO_LEN - 1)) < seqzero){
        seqAuth -= (1 << SEQ_ZERO_LEN);
    }
    seqAuth = (seqAuth & BIT_RNG(SEQ_ZERO_LEN, 23)) | seqzero;
    
    return seqAuth;
}

int rf_link_get_op_para(u8 *ac,  int len_ac, u16 *op, u8 **params, int *par_len){
    u8 op_type = GET_OP_TYPE(ac[0]);
    if(OP_TYPE_SIG1 == op_type){
        mesh_cmd_ac_sig1_t *p_ac = (mesh_cmd_ac_sig1_t *)ac;
        *op = p_ac->op;
        *params = p_ac->data;
        *par_len = len_ac - 1;
    }else{
        if(OP_TYPE_SIG2 == op_type){
            mesh_cmd_ac_sig2_t *p_ac = (mesh_cmd_ac_sig2_t *)ac;
            *op = p_ac->op;
            *params = p_ac->data;
            *par_len = len_ac - 2;
        }else{
            mesh_cmd_ac_vd_t *p_ac = (mesh_cmd_ac_vd_t *)ac;
            
            *op = p_ac->op;
            *params = p_ac->data;
            *par_len = len_ac - 3;

            #if (0 == VC_SUPPORT_ANY_VENDOR_CMD_EN)
            if(p_ac->vd_id != g_vendor_id){
				*op = -1;
                return -1;
            }
            #endif
        }
    }

    return 0;
}

u32 rf_link_get_op_by_ac(u8 *ac)
{
	u16 op;
	u8 *params;
	int par_len;
	if(rf_link_get_op_para(ac, 3, &op, &params, &par_len)){
		return -1;
	}

	return op;
}

u32 rf_link_get_op_by_bear(u8 *bear)
{
	u32 op_ut = -1;
	mesh_cmd_bear_unseg_t *p_bear = (mesh_cmd_bear_unseg_t *)bear;
	mesh_cmd_nw_t *p_nw = &p_bear->nw;
	if(p_nw->ctl){
		op_ut = -1;
	}else{
		u8 *p_ac;
		if(p_bear->lt.seg){
			mesh_cmd_bear_seg_t *p_bear_seg = (mesh_cmd_bear_seg_t *)p_bear;
			p_ac = (u8 *)&p_bear_seg->ut;
		}else{
			p_ac = (u8 *)&p_bear->ut;
		}
		
		op_ut = rf_link_get_op_by_ac(p_ac);
	}

	return op_ut;
}

int mesh_rc_data_layer_access(u8 *ac, int len_ac, mesh_cmd_nw_t *p_nw)
{
    int err = -1;
    u16 adr_src = p_nw->src;
    u16 adr_dst = p_nw->dst;
    u16 op;
    u8 *params;
    int par_len;
	LAYER_PARA_DEBUG(A_debug_acess_layer_enter);
	#if (TL_LOG_SEL_VAL	 & (BIT(TL_LOG_NODE_SDK)))
	ut_log_t ut_log;
	int log_len = len_ac + 4;
	ut_log.src = adr_src;
	ut_log.dst = adr_dst;
	if(log_len > sizeof(ut_log.data)){
	    log_len = sizeof(ut_log.data);
	}
	memcpy(ut_log.data, ac, log_len);
    LOG_MSG_INFO(TL_LOG_NODE_SDK_NW_UT,(u8 *)&ut_log,log_len,"UT PDU:",0);
    #endif
    
    if(rf_link_get_op_para(ac, len_ac, &op, &params, &par_len)){
		LAYER_PARA_DEBUG(A_debug_acess_layer_opcode_ret);
		LOG_MSG_ERR(TL_LOG_MESH,0, 0 ,"acess_layer_opcode err",0);
        return -1;
    }

    #if (WIN32 && (!DEBUG_SHOW_VC_SELF_EN))
    if(is_cmd_skip_for_vc_self(adr_src, op)){
        // LOG_MSG_ERR(TL_LOG_MESH,0, 0 ,"is vc self adr",0); // don't print as normal.
    	return 0;
    }
    #endif
    
    #if WIN32
    if(!is_own_ele(adr_src)){
        LOG_MSG_INFO(TL_LOG_NODE_BASIC,ac,len_ac,"adr_src:0x%04x,adr_dst:0x%04x,access rx cmd is 0x%x ",adr_src, adr_dst, op);
    }
	#endif

    #if 0
	static u32 ts_B_11;
	static u8 ts_B_11_ac[20][16];
	u32 len_log = len_ac;
	if(len_log > 16){
	    len_log = 16;
	}
	memset(ts_B_11_ac[(ts_B_11)%ARRAY_SIZE(ts_B_11_ac)], 0, 16);
	memcpy(ts_B_11_ac[(ts_B_11++)%ARRAY_SIZE(ts_B_11_ac)], ac, len_log);
	#endif

    int is_support_flag = 0;
    int is_status_cmd = 0;

    #if VC_SUPPORT_ANY_VENDOR_CMD_EN
    if(IS_VENDOR_OP(op)){
        is_support_flag = 1;
        if(is_own_ele(adr_dst) || is_fixed_group(adr_dst)){
            is_status_cmd = 1;  // because reliable par may have been clear here.
            mesh_cb_fun_par_vendor_t cb_par = {0};
            cb_par.adr_src = adr_src;
            cb_par.adr_dst = adr_dst;
            cb_par.retransaction = 0;
            cb_app_vendor_all_cmd((mesh_cmd_ac_vd_t *)ac, len_ac, &cb_par);
        }
    }else
    #endif
    {
    mesh_op_resource_t op_res;
    if(is_support_op(&op_res, op, adr_dst, 0)){
        is_support_flag = 1;
		LAYER_PARA_DEBUG(A_debug_acess_layer_support_model);
        if(op_res.model_cnt){
            u8 retransaction = 0;
            
            foreach(i,op_res.model_cnt){
                model_common_t *p_model = (model_common_t *)op_res.model[i];
                if(is_cfg_model(op_res.id, op_res.sig)){	// cfg model
                	if(SECURITY_ENABLE && (adr_src != ele_adr_primary)){
                	    if(mesh_key.appkey_sel_dec < APP_KEY_MAX){ // decode by app key
                            LOG_MSG_ERR(TL_LOG_MESH,0, 0 ,"should not use app key to decryption config model",0);
                		    return -1;	// may be attacked.
                	    }else if(!is_actived_factory_test_mode() && (mesh_key.devkey_self_dec != DEC_BOTH_TWO_DEV_KEY)){
                	        /*(!(((SIG_MD_CFG_SERVER == op_res.id) && (1 == mesh_key.devkey_self_dec))
                	                            &&((SIG_MD_CFG_CLIENT == op_res.id) && (0 == mesh_key.devkey_self_dec))))*/
                	        if(op_res.id == mesh_key.devkey_self_dec){	// model id is for RX node
                                LOG_MSG_ERR(TL_LOG_MESH,0, 0 ,"use wrong device key",0);
                		        return -1;	// may be attacked.
                		    }
                		}
                	}
                	
                	if(!is_valid_cfg_op_when_factory_test(op)){
                        LOG_MSG_ERR(TL_LOG_MESH,0, 0 ,"factory mode unsupport this op:0x%04x",op);
                	    return -1;
                	}
                }else{
                    // just compare old key is enough, because bind key of old key is alway valid and same with new, if existed.
					mesh_app_key_t *app_key = &mesh_key.net_key[mesh_key.netkey_sel_dec][0].app_key[mesh_key.appkey_sel_dec];
	                if(!is_exist_bind_key(p_model, app_key->index)){
                        LOG_MSG_ERR(TL_LOG_MESH,0, 0 ,"model 0x%4x did't bind this key in element index %d",op_res.id, i);
	                	continue ;
	                }
                }

                if(is_unicast_adr(adr_dst)
                || is_fixed_group(adr_dst)  // have been checked feature before in mesh_match_group_mac_()
                || is_subscription_adr(p_model, adr_dst)){
                    if(op_res.cb){
                        mesh_cb_fun_par_t cb_par;
                        cb_par.model = op_res.model[i];
                        cb_par.model_idx = op_res.model_idx[i];;
                        cb_par.adr_src = adr_src;
                        cb_par.adr_dst = adr_dst;
                        cb_par.op = op;

                        #if 1
                        if(0 == i){ // check only once
                            u8 tid_pos;
                            if(
                            #if (!DEBUG_SHOW_VC_SELF_EN)
                            (adr_src != adr_dst) && 
                            #endif
                            is_cmd_with_tid(&tid_pos, op, TID_NONE_VENDOR_OP_VC)){
                                LAYER_PARA_DEBUG(A_debug_acess_layer_tid_ok);
                                if(is_retransaction(adr_src, params[tid_pos])){
                                    retransaction = 1;
                                }
                            }
                        }
                        #endif
                        cb_par.retransaction = retransaction;
                        cb_par.op_rsp = op_res.op_rsp;
                        cb_par.res = (u8 *)&op_res;
                        cb_par.p_nw = p_nw;
                        err = mesh_rc_data_layer_access_cb(params, par_len, &cb_par);
                        if(err){
                            LOG_MSG_ERR(TL_LOG_MESH,0, 0 ,"mesh_rc_data_layer_access_cb error! op:%04x",op);
                        }
                    }
                }
            }
        }else{
            if(op_res.status_cmd){  // always action for status message
                if(op_res.cb){
					#if MESH_RX_TEST
					mesh_cb_fun_par_t cb_par = {0};
					cb_par.adr_src = adr_src;
	                cb_par.op = op;
                    err = op_res.cb(params, par_len, &cb_par);
					#else
					    #if ((__PROJECT_MESH_PRO__ || __PROJECT_MESH_GW_NODE__)&& (!DEBUG_SHOW_VC_SELF_EN))
					err = op_res.cb(params, par_len, 0);    // should not for firmware
                    if(err){
                        LOG_MSG_ERR(TL_LOG_MESH,0, 0 ,"prov mesh_rc_data_layer_access_cb error! op:%04x",op);
                    }
					    #endif
					#endif
                }
            }
        }

        is_status_cmd = op_res.status_cmd;
    }else{
        LOG_MSG_ERR(TL_LOG_MESH,0, 0 ,"mesh_rc_data_layer_access: not support op!",0);
    }
    }

    #if RELIABLE_CMD_EN
    u32 op_rsp = op;    // include vendor id
    
    #if VC_SUPPORT_ANY_VENDOR_CMD_EN
    if(IS_VENDOR_OP(op)){
        memcpy(&op_rsp,ac,3);
    }
    #endif
    
    if(is_support_flag && is_status_cmd){
        // check more details later
        mesh_rc_rsp_t rc_rsp;
        if(len_ac <= sizeof(rc_rsp.data)){
            rc_rsp.src = adr_src;
            rc_rsp.dst = adr_dst;
            rc_rsp.len = len_ac + (OFFSETOF(mesh_rc_rsp_t,data) - OFFSETOF(mesh_rc_rsp_t,src));
            memcpy(rc_rsp.data, ac, len_ac);
            if(mesh_tx_reliable.busy && is_rsp2tx_reliable(op_rsp, adr_src)){
                rf_link_slave_add_status(&rc_rsp, op);
            }else{
            	//if(rc_rsp.len + 2 <= 100){	// USB buf size:64   // hci_tx_fifo.size
                	mesh_rsp_handle(&rc_rsp);
                //}
            }
        }
    }
    #endif

    return err;
}

mesh_fri_ship_other_t * mesh_friend_cache(const mesh_cmd_bear_unseg_t *bear, u16 F2L_bit)  // create new bearer
{
    mesh_cmd_bear_unseg_t bear_temp;
    memcpy(&bear_temp, bear, sizeof(mesh_cmd_bear_unseg_t));
    
    mesh_cmd_bear_unseg_t *p_bear = &bear_temp;
	mesh_cmd_nw_t *p_nw = &p_bear->nw;
    if(p_nw->ttl){
        p_nw->ttl--;
    }
    
    // swap to big endianness
    if(p_bear->lt.seg){
        endianness_swap((u8 *)p_nw, SWAP_TYPE_LT_SEG);
    }
    endianness_swap((u8 *)p_nw, SWAP_TYPE_NW);
    return mesh_fri_cmd2cache((u8 *)p_bear, mesh_nw_len_get_by_bear(p_bear), MESH_ADV_TYPE_MESSAGE, TRANSMIT_DEF_PAR, F2L_bit);
}

#if FN_PRIVATE_SEG_CACHE_EN
typedef struct{
	mesh_cmd_bear_seg_t bear;
	u32 sno_0;
	u32 len_ut;
	u8 ut[ACCESS_WITH_MIC_LEN_MAX]; // for friend ship special
}mesh_friend_seg_cache_t;

mesh_friend_seg_cache_t friend_seg_cache;
#define FRI_FN2LPN_SEG_FROM_FIRST_EN		1

int is_friend_seg_cache_busy();
{
    return (friend_seg_cache.len_ut != 0);
}

void mesh_rc_segment2fri_segment(mesh_cmd_nw_t *p_nw_rc)
{
    mesh_friend_seg_cache_t *p_fri_cache = &friend_seg_cache;
    mesh_cmd_bear_seg_t *p_bear = &p_fri_cache->bear;
    mesh_cmd_nw_t *p_nw = &p_bear->nw;
    mesh_cmd_lt_seg_t *p_lt_seg = (mesh_cmd_lt_seg_t *)(p_nw->data);
    
    memset4(p_fri_cache, 0, sizeof(mesh_friend_seg_cache_t));
    p_fri_cache->len_ut = mesh_rx_seg_par.len_ut_total;
    memcpy(p_fri_cache->ut, mesh_cmd_ut_rx_seg, p_fri_cache->len_ut);
    
    // copy header of mesh_cmd_nw_t and mesh_cmd_lt_seg_t
    memcpy(p_nw, p_nw_rc, OFFSETOF(mesh_cmd_nw_t,data) + OFFSETOF(mesh_cmd_lt_seg_t,data));
    p_fri_cache->sno_0 = 0;
    memcpy(&p_fri_cache->sno_0, p_nw_rc->sno, 3);

	#if FRI_FN2LPN_SEG_FROM_FIRST_EN
    mesh_cmd_lt_seg_t *p_lt_seg_rc = (mesh_cmd_lt_seg_t *)(p_nw_rc->data);
    p_fri_cache->sno_0 -= p_lt_seg_rc->segN;
    p_lt_seg->segO = 0;
    //p_lt_seg->segN = p_lt_seg_rc->segN;
    #else
    p_lt_seg->segO = p_lt_seg->segN;    // from the last
    #endif
}

void friend_seg_cache2friend_cache(u16 F2L_bit)
{
    mesh_friend_seg_cache_t *p_fri_cache = &friend_seg_cache;
    mesh_cmd_bear_seg_t *p_bear = &p_fri_cache->bear;
    mesh_cmd_nw_t *p_nw = &p_bear->nw;
    mesh_cmd_lt_seg_t *p_lt_seg = &p_bear->lt;
    // network
    #if FRI_FN2LPN_SEG_FROM_FIRST_EN
    u32 sno = p_fri_cache->sno_0 + p_lt_seg->segO;
    #else
    u32 sno = p_fri_cache->sno_0--;
    #endif
    memcpy(p_nw->sno, &sno, 3);
    
    // upper transport
    u32 len_seg = mesh_seg_add_ut(p_bear, p_fri_cache->ut, p_fri_cache->len_ut, p_lt_seg->segO);
    p_bear->len = mesh_bear_len_get_by_seg_ut(len_seg, p_nw->ctl);
    mesh_friend_cache((mesh_cmd_bear_unseg_t *)p_bear, F2L_bit);
    
    // lower transport for next
    #if FRI_FN2LPN_SEG_FROM_FIRST_EN
    if(p_lt_seg->segO < p_lt_seg->segN){
        p_lt_seg->segO++;   // for next segment
    }else{                  // finished
        memset(p_fri_cache, 0, sizeof(mesh_friend_seg_cache_t));
    }
    #else
    if(p_lt_seg->segO){
        p_lt_seg->segO--;   // for next segment
    }else{                  // finished
        memset(p_fri_cache, 0, sizeof(mesh_friend_seg_cache_t));
    }
    #endif
}

#else
#define FRI_FN2LPN_SEG_FROM_FIRST_EN		1   // FN-BV08 required.


void mesh_rc_segment2friend_cache(mesh_cmd_nw_t *p_nw_rc, u16 F2L_bit)
{
	mesh_cmd_bear_seg_t bear_temp = {0};
    mesh_cmd_nw_t *p_nw = &bear_temp.nw;
    mesh_cmd_lt_seg_t *p_lt_seg = (mesh_cmd_lt_seg_t *)(p_nw->data);
        
    // copy header of mesh_cmd_nw_t and mesh_cmd_lt_seg_t
    memcpy(p_nw, p_nw_rc, OFFSETOF(mesh_cmd_nw_t,data) + OFFSETOF(mesh_cmd_lt_seg_t,data));
	u32 pkt_cnt = p_lt_seg->segN + 1;
	#if FEATURE_FRIEND_EN
	if(pkt_cnt > mesh_get_fn_cache_size_log_cnt()){
		return ;
	}
	#endif
    u32 sno_last = 0;
    memcpy(&sno_last, p_nw_rc->sno, 3);
    #if FRI_FN2LPN_SEG_FROM_FIRST_EN
    sno_last = sno_last - pkt_cnt + 1;
    #endif
    
	mesh_fri_ship_other_t *p_other = 0;
    foreach(i,pkt_cnt){
		memcpy(p_nw->sno, &sno_last, 3);
		#if FRI_FN2LPN_SEG_FROM_FIRST_EN
		sno_last++;
    	p_lt_seg->segO = i;    // from first
		#else
		sno_last--;
    	p_lt_seg->segO = p_lt_seg->segN - i;    // from the last
		#endif
		// upper transport
		u32 len_seg = mesh_seg_add_ut(&bear_temp, mesh_cmd_ut_rx_seg, mesh_rx_seg_par.len_ut_total, p_lt_seg->segO);
		bear_temp.len = mesh_bear_len_get_by_seg_ut(len_seg, p_nw->ctl);
		p_other = mesh_friend_cache((mesh_cmd_bear_unseg_t *)&bear_temp, F2L_bit);
		
		#if (TEST_CASE_FN_BV19_EN)
		break;	// if not, will be error: The IUT should not transmit more than one Network PDU to LPN.
		#endif
    }

    if(p_other){
        //p_other->cache_non_replace_cnt = pkt_cnt;   // not use now
    }
}
#endif

void tx_busy_seg_ack(mesh_cmd_bear_seg_t *p_bear, mesh_match_type_t *p_match_type)
{
    mesh_cmd_nw_t *p_nw = &p_bear->nw;
    mesh_cmd_lt_seg_t *p_lt_seg = (mesh_cmd_lt_seg_t *)(&p_bear->lt);
	mesh_rx_seg_par_t par;
	par.seqzero = p_lt_seg->seqzero;
	par.seg_map = 0;
	par.obo = mesh_cmd_action_need_friend(p_match_type);
	par.src = p_nw->src;
	par.dst = p_nw->dst;

	mesh_add_seg_ack(&par);
}

static inline int is_valid_segment_pkt(mesh_cmd_lt_seg_t *p_lt_seg)
{
    return (p_lt_seg->segN < 32);
}

int mesh_rc_segment_handle(mesh_cmd_bear_seg_t *p_bear, u32 ctl, mesh_match_type_t *p_match_type)
{
	int err = 0;
    mesh_cmd_nw_t *p_nw = &p_bear->nw;
    mesh_cmd_lt_seg_t *p_lt_seg = (mesh_cmd_lt_seg_t *)(&p_bear->lt);
    if(!is_valid_segment_pkt(p_lt_seg)){
        LOG_MSG_ERR(TL_LOG_MESH,0, 0 ,"invalid segment pkt",0);
        return -1;
    }
    
    u32 max_size = mesh_max_payload_get(p_nw->ctl);
    u8 len_ut = (p_bear->len + OFFSETOF(mesh_cmd_bear_seg_t, type)) - OFFSETOF(mesh_cmd_bear_seg_t, ut)
                - (ctl ? SZMIC_NW64 : SZMIC_NW32);
    u32 pos = (p_lt_seg->segO * max_size);
    u32 seqAuth_pkt = get_rx_seg_seqAuth(p_nw->sno, p_lt_seg->seqzero);
    
    if((p_nw->src == mesh_rx_seg_par.src)&&(mesh_rx_seg_par.seqAuth == seqAuth_pkt)){
    	if(SEG_RX_STATE_TIMEOUT == mesh_rx_seg_par.status){
    	    LOG_MSG_ERR(TL_LOG_MESH,0, 0 ,"segment rx timeout",0);
    		return -1;	// not handler the last segment flow any more.
    	}
    	
        if((0 == mesh_rx_seg_par.tick_last)&&(is_unicast_adr(p_nw->dst))){     // have been successed
            mesh_add_seg_ack(&mesh_rx_seg_par);
            return 0;
        }
    }else{
    	if(is_rx_seg_old_seqAuth(p_nw->src, seqAuth_pkt)){
    	    // LOG_MSG_ERR(TL_LOG_MESH,0, 0 ,"rx seg old seqAuth",0);
    		return -1;
    	}
    	
    	if(mesh_rx_seg_par.tick_last && (p_nw->src != mesh_rx_seg_par.src)){
    		tx_busy_seg_ack(p_bear, p_match_type);
    	}else{
	        memset(&mesh_rx_seg_par, 0, sizeof(mesh_rx_seg_par));
	        mesh_rx_seg_par.seqzero = p_lt_seg->seqzero;
	        mesh_rx_seg_par.seqAuth = seqAuth_pkt;
	        mesh_rx_seg_par.nk_array_idx = mesh_key.netkey_sel_dec;
	        SEG_DEBUG_LED(0);
	        #if DEBUG_SEG_RX
	        memset(&B_debug_seg_rx, 0,sizeof(B_debug_seg_rx));
	        debug_rx_record(p_nw->sno);
	        #endif
        }
    }

    #if RELIABLE_CMD_EN
	mesh_tx_reliable_tick_refresh_proc(1, p_nw->src);
	#endif

    if(is_unicast_adr(p_nw->dst)){
        mesh_rx_seg_par.tick_last = mesh_rx_seg_par.tick_seg_idle = clock_time()|1;
    }
    if(mesh_rx_seg_par.seg_map & BIT((u32)(p_lt_seg->segO))){
        return 0;   // have been received.
    }
    mesh_rx_seg_par.seg_map |= BIT((u32)(p_lt_seg->segO));
    
    if(!mesh_rx_seg_par.par_saved){
        mesh_rx_seg_par.par_saved = 1;
        mesh_rx_seg_par.nid = p_nw->nid;
        mesh_rx_seg_par.ivi = p_nw->ivi;
        mesh_rx_seg_par.obo = mesh_cmd_action_need_friend(p_match_type);
        mesh_rx_seg_par.src = p_nw->src;
        mesh_rx_seg_par.dst = p_nw->dst;
    }
    
    if(p_lt_seg->segN == p_lt_seg->segO){
        mesh_rx_seg_par.len_ut_total = pos + len_ut;
    }
    
    memcpy(mesh_cmd_ut_rx_seg + pos, p_lt_seg->data, len_ut);
    
    if(mesh_rx_seg_par.seg_cnt == p_lt_seg->segN){   // all packet received
        LOG_MSG_INFO(TL_LOG_NODE_BASIC,0,0,"rx segment all packetes received",0);    
        mesh_rx_seg_par.tick_last = 0;  // clear timer
        mesh_rx_seg_par.status = SEG_RX_STATE_COMPLETED;
    
		if(DST_MATCH_MAC == p_match_type->type){	// ack need
            mesh_add_seg_ack(&mesh_rx_seg_par);
            //memset(&mesh_rx_seg_par, 0, sizeof(mesh_rx_seg_par));   // test
		}
        
        if(mesh_cmd_action_need_friend(p_match_type)){
        	#if FN_PRIVATE_SEG_CACHE_EN
            while(is_friend_seg_cache_busy()){  // push all cache of last segment to friend cache.
                friend_seg_cache2friend_cache(p_match_type->F2L);
            }
            
            mesh_rc_segment2fri_segment(p_nw);
            #else
            mesh_rc_segment2friend_cache(p_nw, p_match_type->F2L);
            #endif
            SEG_DEBUG_LED(1);
        }
        
        if(mesh_cmd_action_need_local(p_match_type)){
            int mic_length = 0;
            if(!ctl){
            	memcpy(p_nw->sno, &mesh_rx_seg_par.seqAuth, 3);
                mic_length = p_lt_seg->szmic ? SZMIC_TRNS_SEG64 : SZMIC_TRNS_SEG32;
                err = mesh_sec_msg_dec_apl(mesh_cmd_ut_rx_seg, mesh_rx_seg_par.len_ut_total, (u8 *)p_nw);				
            }
            if(err){
                // have been print error log in mesh_sec_msg_dec_apl_
            }else{
				#if TESTCASE_FLAG_ENABLE
				u8 nw_mic_len = (ctl ? SZMIC_NW64 : SZMIC_NW32);
				memcpy(tc_seg_buf, &p_bear->len, OFFSETOF(mesh_cmd_bear_seg_t,ut)-1);
				memcpy(tc_seg_buf+OFFSETOF(mesh_cmd_bear_seg_t,lt.data)-1, (u8 *)mesh_cmd_ut_rx_seg, mesh_rx_seg_par.len_ut_total);
				memcpy(tc_seg_buf+OFFSETOF(mesh_cmd_bear_seg_t,lt.data)-1+mesh_rx_seg_par.len_ut_total, (u8 *)&p_bear->nw+p_bear->len-nw_mic_len, nw_mic_len);				
				SET_TC_FIFO(TSCRIPT_MESH_RX, tc_seg_buf, mesh_rx_seg_par.len_ut_total+OFFSETOF(mesh_cmd_bear_seg_t,ut)-1+nw_mic_len);
				#endif
                err = mesh_rc_data_layer_access(mesh_cmd_ut_rx_seg, mesh_rx_seg_par.len_ut_total - mic_length, p_nw);
                SEG_DEBUG_LED(1);
            }
        }
        
        if(is_own_ele(p_nw->src)
        && (is_unicast_friend_msg_to_lpn(p_nw)||is_own_ele(p_nw->dst))){
            mesh_tx_segment_finished();
        }
    }
    mesh_rx_seg_par.seg_cnt++;      // must at last

    return err;
}

int mesh_rc_data_layer_lower_upper(mesh_cmd_bear_unseg_t *p_bear, int src_type)
{
	int err = 0;
	mesh_cmd_nw_t *p_nw = &p_bear->nw;

    mesh_match_type_t match_type;
	mesh_match_group_mac(&match_type, p_nw->dst, -1, 0, 0);	// op_ut is encryption here
	LAYER_PARA_DEBUG(A_debug_low_layer_enter);
    if(DST_MATCH_NONE == match_type.type){
		LAYER_PARA_DEBUG(A_debug_low_layer_not_match);
		#if ONLINE_ST_LIB_EN
        online_st_force_notify_check(p_bear, 0, src_type);
		#endif
		// LOG_MSG_ERR(TL_LOG_MESH,0, 0 ,"lower layer adr not match",0);
        return 0;     // for test
    }

	if(is_fn_support_and_en){
	    if(match_type.F2L){
	        if(p_nw->ttl < 2){
	            match_type.F2L = 0;
	            
	            if(!match_type.local){
	                return 0;
	            }
	        }
	    }
    }
#if FEATURE_LOWPOWER_EN
    else if(is_lpn_support_and_en){
        if(is_in_mesh_friend_st_lpn()){
            if((0 == p_nw->ctl) || (!is_not_cache_ctl_msg_fn(p_nw))){
        	    mesh_lpn_par.poll.FSN = !mesh_lpn_par.poll.FSN;
        	}
        	
            int fn_md = 1;
            if(p_nw->ctl){
                mesh_cmd_lt_ctl_unseg_t *p_lt_ctl_unseg = (mesh_cmd_lt_ctl_unseg_t *)&p_bear->lt;
                if(CMD_CTL_UPDATE == p_lt_ctl_unseg->opcode){
					mesh_ctl_fri_update_t *p_update = (mesh_ctl_fri_update_t *)(p_lt_ctl_unseg->data);
                    if(1 != p_update->md){
                    	fn_md = 0;
                    }
                }
            }
            
            if(fn_md){//(p_bear->lt.md){
                mesh_lpn_sleep_later_op(CMD_ST_POLL_MD, FRI_POLL_DELAY_FOR_MD_MS);
            }else{
            	mesh_friend_ship_stop_poll();
                if(mesh_tx_seg_par.busy){
                	mesh_rx_seg_ack_handle(1);
                	mesh_lpn_sleep_enter_normal_seg();
                }else{
                    mesh_lpn_sleep_enter_later();
                }
            }
        }else{
            // have been filter "OFFER" in mesh_sec_msg_dec_nw_
        }
    }
#endif

    if(p_bear->lt.seg){
        endianness_swap((u8 *)p_nw, SWAP_TYPE_LT_SEG);  // must before
        LAYER_PARA_DEBUG(A_debug_low_layer_segment_handler);
        err = mesh_rc_segment_handle((mesh_cmd_bear_seg_t *)p_bear, p_nw->ctl, &match_type);
    }else{
    	LAYER_PARA_DEBUG(A_debug_low_layer_no_segment_handler);
        endianness_swap((u8 *)p_nw, SWAP_TYPE_LT_UNSEG);
        if(mesh_cmd_action_need_friend(&match_type)){
            int replace = 0;
            if(p_nw->ctl){
                if(CMD_CTL_ACK == p_bear->lt_ctl_unseg.opcode){
                    mesh_cmd_bear_unseg_t bear_big;
                    memcpy(&bear_big, p_bear, sizeof(bear_big));
                    endianness_swap((u8 *)&bear_big.nw, SWAP_TYPE_LT_UNSEG);    // set to big endianness
                    endianness_swap((u8 *)&bear_big.nw, SWAP_TYPE_NW);          // set to big endianness

                    foreach(i,g_max_lpn_num){
                    	if(match_type.F2L & BIT(i)){
                    		replace = friend_cache_check_replace(i, &bear_big);
                    		break;	// always unicast for ack
                    	}
                    }
                }
            }
            
            if(0 == replace){
                mesh_fri_ship_other_t * p_other = mesh_friend_cache(p_bear, match_type.F2L);
                err = p_other ? 0 : -1;
            }else{
                return 0;
            }
        }
		// not need local dispatch 
        if(p_nw->ctl && p_bear->lt_ctl_unseg.opcode == CMD_CTL_HEARTBEAT){
			endianness_swap((u8 *)p_nw, SWAP_TYPE_LT_CTL_UNSEG);
			mesh_process_hb_sub(p_bear);
		}
        if(mesh_cmd_action_need_local(&match_type)){
			LAYER_PARA_DEBUG(A_debug_low_layer_local_dispatch);
            if(p_nw->ctl){
                mesh_cmd_lt_ctl_unseg_t *p_lt_ctl_unseg = (mesh_cmd_lt_ctl_unseg_t *)&p_bear->lt;
				SET_TC_FIFO(TSCRIPT_MESH_RX, (u8 *)&p_bear->len, p_bear->len+1);
                if(CMD_CTL_ACK == p_lt_ctl_unseg->opcode){
                    endianness_swap((u8 *)p_nw, SWAP_TYPE_LT_CTL_SEG_ACK);
                    mesh_cmd_lt_ctl_seg_ack_t *p_lt_ctl_seg_ack = (mesh_cmd_lt_ctl_seg_ack_t *)p_lt_ctl_unseg;
                    if(mesh_tx_seg_par.busy && (p_lt_ctl_seg_ack->seqzero == mesh_tx_seg_par.seqzero)){
                    	mesh_tx_seg_par.ack_received = 1;
                        if(p_lt_ctl_seg_ack->obo){
                            // ack from friend not the real destination addr
                        }
                        
                        memcpy(&mesh_tx_seg_par.seg_map_rec, p_lt_ctl_seg_ack->seg_map, 4);
                    	if(0 == mesh_tx_seg_par.seg_map_rec){
                            mesh_tx_segment_finished();
							LOG_MSG_INFO(TL_LOG_NODE_BASIC,0,0,"rc data layer upper:RX node segment is busy,so tx flow cancle",0);
                    	}else{
	                        int tx_flag = !(is_lpn_support_and_en);
	                        int cnt = mesh_rx_seg_ack_handle(tx_flag);
	                        if(0 == cnt){
	                            mesh_tx_segment_finished();
								LOG_MSG_INFO(TL_LOG_NODE_BASIC,p_lt_ctl_seg_ack->seg_map, 4,"rc data layer upper:segment tx success, map in ACK is",0);
                                if(p_lt_ctl_seg_ack->obo){
                                    LOG_MSG_INFO(TL_LOG_NODE_BASIC,0,0,"RX segment ACK with obo ",0);
                                }
	                        }else{
	                            mesh_seg_add_pkt_reset_timer(cnt);
	                        }
                        }
                    }
                }else{
                    if((p_nw->dst == ele_adr_primary)	// only primary support Friend
                    || (CMD_CTL_REQUEST == p_lt_ctl_unseg->opcode)){
                        endianness_swap((u8 *)p_nw, SWAP_TYPE_LT_CTL_UNSEG);
                        if(is_fn_support_and_en){
							LOG_MSG_INFO(TL_LOG_MESH,(u8 *)p_bear,p_bear->len + 2,
										"mesh_rc_data_layer_lower_upper:receive control command xxxxxxxxxxxxx: \r\n",0);
                            mesh_friend_ship_proc_FN((u8 *)p_bear);
                        }else if(is_lpn_support_and_en){
                            #if FEATURE_LOWPOWER_EN
                            mesh_friend_ship_proc_LPN((u8 *)p_bear);
                            #endif
                        }
                    }
                }
            }else{
                mesh_cmd_lt_unseg_t *p_lt_unseg = &p_bear->lt;
                u8 len_ut = mesh_bear_len_get(p_bear) - OFFSETOF(mesh_cmd_bear_unseg_t, ut) - SZMIC_NW32;
                memcpy(mesh_cmd_ut_rx_unseg, p_lt_unseg->data, len_ut);   // must, because relay will use ut layer in original pkt.
                err = mesh_sec_msg_dec_apl(mesh_cmd_ut_rx_unseg, len_ut, (u8 *)p_nw);
                if(err){
                    // have been print error log in mesh_sec_msg_dec_apl_
                }else{
					#if TESTCASE_FLAG_ENABLE
					memcpy(tc_seg_buf, &p_bear->len, OFFSETOF(mesh_cmd_bear_unseg_t,ut)-1);
					memcpy(tc_seg_buf+OFFSETOF(mesh_cmd_bear_unseg_t,lt.data)-1, (u8 *)mesh_cmd_ut_rx_unseg, len_ut);
					memcpy(tc_seg_buf+OFFSETOF(mesh_cmd_bear_unseg_t,lt.data)-1+len_ut, (u8 *)&p_bear->nw+p_bear->len-SZMIC_NW32, SZMIC_NW32);				
					SET_TC_FIFO(TSCRIPT_MESH_RX, tc_seg_buf, len_ut+OFFSETOF(mesh_cmd_bear_unseg_t,ut)-1+SZMIC_NW32);
					#endif
					#if 0
                    static u32 ts_A_22;
                    static u8 ts_A_22ac[20][16];
                    memcpy(ts_A_22ac[(ts_A_22++)%ARRAY_SIZE(ts_A_22ac)], mesh_cmd_ut_rx_unseg, 16);
					#endif
					LAYER_PARA_DEBUG(A_debug_low_layer_data_layer_enter);
                    err = mesh_rc_data_layer_access(mesh_cmd_ut_rx_unseg, len_ut - SZMIC_TRNS_SEG32, p_nw);
                    #if ONLINE_ST_LIB_EN
                    online_st_force_notify_check(p_bear, mesh_cmd_ut_rx_unseg, src_type);
                    #endif
                }
            }
        }
    }

    return err;
}
int mesh_rc_data_cfg_gatt(u8 *bear)
{
	int err =-1;
	mesh_cmd_bear_unseg_t *p_bear = (mesh_cmd_bear_unseg_t *)bear;
	mesh_cmd_nw_t *p_nw = &p_bear->nw;
    int len_dec_nw = mesh_bear_len_get(p_bear) - (OFFSETOF(mesh_cmd_bear_unseg_t, lt) - 2);
	u8 friend_key_flag = 0;
	err = mesh_sec_msg_dec_nw((u8 *)p_nw, len_dec_nw, p_nw->nid, &friend_key_flag,1,ADV_FROM_GATT);
	if(!err){
	    LOG_MSG_INFO(TL_LOG_NODE_BASIC,0,0,"mesh_rc_data_cfg_gatt dec suc",0);
        if(is_own_ele(p_nw->src) /*|| 0 != p_nw->dst*/){
            LOG_MSG_INFO(TL_LOG_NODE_BASIC,0,0,"mesh_rc_data_cfg_gatt src err",0);
            return -2;
        }
        
        if(is_exist_in_cache((u8 *)p_nw, friend_key_flag, 1)){ //  mainly for save cache.
            LOG_MSG_INFO(TL_LOG_NODE_BASIC,0,0,"mesh_rc_data_cfg_gatt exist cache err",0);
        	return -3;
        }
    }
	return err;	
}

int mesh_rc_data_layer_network (u8 *p_payload, int src_type, u8 need_proxy_and_trans_par_val)
{
    #if 0 // test
	static u8 A_rc_buf[12+31];
	static u32 A_rc_buf_cnt; A_rc_buf_cnt++;
	memcpy4(A_rc_buf, p, sizeof(A_rc_buf));
	#endif

	mesh_cmd_bear_unseg_t bear_enc_copy = {0};
	int need_proxy = is_proxy_support_and_en;
	if(need_proxy){
		bear_enc_copy.trans_par_val = need_proxy_and_trans_par_val;
		memcpy(&bear_enc_copy.len, p_payload, p_payload[0]+1);	// back up
	}
	
	int err = 0;

	mesh_cmd_bear_unseg_t *p_bear_enc = GET_BEAR_FROM_ADV_PAYLOAD(p_payload);
    mesh_cmd_nw_t *p_nw = &p_bear_enc->nw;
    #if DEBUG_SEG_RX
    debug_rx_record(p_nw->sno);
    #endif
    
    int len_dec_nw = mesh_bear_len_get(p_bear_enc) - (OFFSETOF(mesh_cmd_bear_unseg_t, lt) - 2);
    u8 friend_key_flag = 0;
	
	#if 0
	static u32 ts_A_1;ts_A_1++;
	#endif
	err = mesh_sec_msg_dec_nw((u8 *)p_nw, len_dec_nw, p_nw->nid, &friend_key_flag,0,src_type);
	#if (DEBUG_PROXY_FRIEND_SHIP && (!WIN32))
		event_adv_report_t *p = CONTAINER_OF(p_payload,event_adv_report_t,data[0]);
		u32 mac_src;
		memcpy(&mac_src, p->mac, sizeof(mac_src));
		#if FEATURE_LOWPOWER_EN
		if((ADV_FROM_MESH == src_type)&&(mac_src != PROXY_FRIEND_SHIP_MAC_FN)){
			return 0;	// mac filter
		}
		
	    if(err){return err;}
		#else
		if((ADV_FROM_MESH == src_type)
		 &&((mac_src != PROXY_FRIEND_SHIP_MAC_LPN1)&&(mac_src != PROXY_FRIEND_SHIP_MAC_LPN2))){
			return 0;	// mac filter
		}
		
		if(err || ((!SECURITY_ENABLE) && friend_key_flag)){
			if(ADV_FROM_MESH == src_type){
				mesh_bear_tx2gatt((u8 *)&bear_enc_copy, MESH_ADV_TYPE_MESSAGE);
			}else if(ADV_FROM_GATT == src_type){
				mesh_bear_tx2mesh((u8 *)&bear_enc_copy, need_proxy_and_trans_par_val);	// before send response should be better
			}
			return 0;
		}
		#endif
	#else
    if(err){return err;}
    #endif
	
	int relay = 0;

	#ifndef WIN32
	if(src_type == ADV_FROM_GATT){
		if(p_nw->src && (app_adr != p_nw->src)){
			app_adr = p_nw->src;
		}
	}else{
		relay =( app_adr != p_nw->dst);
	}
	#endif
	
	#if 0
	static u32 ts_A_12;
	static u8 ts_A_12nw[20][16];
	//memset(ts_A_12nw, 0, sizeof(ts_A_12nw));
	#if 1
	memcpy(ts_A_12nw[(ts_A_12++)%ARRAY_SIZE(ts_A_12nw)], p_nw, 16);
	#else
	if(ts_A_12 < ARRAY_SIZE(ts_A_12nw)){
		memcpy(ts_A_12nw[ts_A_12%ARRAY_SIZE(ts_A_12nw)], p_nw, 16);
	}
	ts_A_12++;
	#endif
	event_adv_report_t *event_adv = CONTAINER_OF(p_payload,event_adv_report_t,data[0]);
	static u8 ts_A_12_mac[6];
	memcpy(ts_A_12_mac,event_adv->mac, 6);
	#endif
	#if TESTCASE_FLAG_ENABLE
	LOG_MSG_INFO(TL_LOG_NODE_SDK_NW_UT,(u8 *)p_nw,16,"NW PDU:",0);
	#endif

    if(mesh_adr_check_src_own_rx(p_nw->src) || mesh_adr_check(p_nw->src, p_nw->dst)){
		LAYER_PARA_DEBUG(A_debug_network_src_err);
		return 0;
    }
    if(ADV_FROM_GATT == src_type && is_unicast_adr(p_nw->src)){
        proxy_proc_filter_mesh_cmd(p_nw->src);    
    }
	#if FEATURE_LOWPOWER_EN
	if(is_friend_ship_link_ok_lpn() && is_unicast_adr(p_nw->dst) && !is_own_ele(p_nw->dst)){
	    // no relay for LPN, discard quickly.
        // LOG_MSG_ERR(TL_LOG_MESH,0, 0 ,"LPN: unicast dst adr not match",0);
		return 0;
	}
	#endif
    

#if FRIEND_MSG_TEST_EN
    u32 sno = 0;
    memcpy(&sno, p_nw->sno, 3);
    if(p_bear_enc->lt.seg){
        static u8 rc_seg_first_sno1 = 1;
        if((is_fn_support_and_en) && (sno == 0x3129ab) && rc_seg_first_sno1){
            rc_seg_first_sno1 = 0;  // if destination of segment Msg is group, it may be same for every retry Msg.
            return -1;
        }
        
        static u8 rc_seg_first = 1;
        if((is_lpn_support_and_en) && rc_seg_first && (sno == 0x3129ad)){
            rc_seg_first = 0;   // just drop the first pkt, because sno of FN cache will not change. 
            return -1;
        }
    }
#endif
    
    // cache compare
    if(is_exist_in_cache((u8 *)p_nw, friend_key_flag, 1)){ // mainly for save cache.
        // have been checked when decryption. should not happen here.
        return 0;
    }
	// LOG_MSG_INFO(TL_LOG_FRIEND, &p_bear_enc->len, 16,"NW PDU: ",0);
#if TESTCASE_FLAG_ENABLE
	if((tc_par.tc_id == NODE_NET_BV_00))	{
		SET_TC_FIFO(TSCRIPT_MESH_RX_NW, &p_bear_enc->len, p_bear_enc->len+1);
	}
#endif

	int local_friend_match = (is_own_ele(p_nw->dst)) || mesh_mac_match_friend(p_nw->dst);
	if(!local_friend_match){
		#if FEATURE_PROXY_EN
		if(need_proxy){
			mesh_cmd_bear_unseg_t *p_br_dec = GET_BEAR_FROM_ADV_PAYLOAD(p_payload);
			if(ADV_FROM_MESH == src_type){
				if(is_valid_adv_with_proxy_filter(p_br_dec->nw.dst)){
				    // have been record error cnt in proxy_adv2gatt_err;
					mesh_bear_tx2gatt((u8 *)&bear_enc_copy, MESH_ADV_TYPE_MESSAGE);
				}
			}else if(ADV_FROM_GATT == src_type){
				// add the adr to the filter
				if(mesh_bear_tx2mesh((u8 *)&bear_enc_copy, need_proxy_and_trans_par_val)){	// before send response should be better
				    static u8 test_proxy2mesh_err_cnt;test_proxy2mesh_err_cnt++;
                    LOG_MSG_ERR(TL_LOG_MESH,0, 0 ,"proxy to mesh failed",0);
				}
			}
		}
		#endif

        #ifndef WIN32
        #if (0 == ALONE_BUFF_FOR_RELAY_EN)
		if((blt_state == BLS_LINK_STATE_CONN) &&my_fifo_data_cnt_get(&mesh_adv_cmd_fifo)){
			relay = 0;
		}
		#endif
		#endif
		
		if ((!is_busy_mesh_tx_cmd(0)) && relay && is_relay_support_and_en){
		    // relay
		    u8 len_nw = mesh_nw_len_get_by_bear(p_bear_enc);
		    if(p_nw->ttl >= 2){
	            mesh_cmd_bear_unseg_t bear_relay;   // must use a new one, because it will be re-encryption.
	            memcpy(&bear_relay, p_bear_enc, sizeof(mesh_cmd_bear_unseg_t));            
	            bear_relay.nw.ttl--;
	            u8 len_lt = len_nw - OFFSETOF(mesh_cmd_nw_t, data)- (p_nw->ctl ? SZMIC_NW64 : SZMIC_NW32);
	            mesh_sec_msg_enc_nw((u8 *)&bear_relay.nw, len_lt, SWAP_TYPE_NONE, 0, len_nw, bear_relay.type,0,mesh_key.netkey_sel_dec);
	            #if (ALONE_BUFF_FOR_RELAY_EN)
	            bear_relay.trans_par_val = model_sig_cfg_s.relay_retransmit.val;
	            my_fifo_push_relay(&mesh_adv_fifo_relay, &bear_relay, mesh_bear_len_get(&bear_relay), 0);
	            #else
	            mesh_bear_tx2mesh((u8 *)&bear_relay, model_sig_cfg_s.relay_retransmit.val);
	            #endif
		    }
	    }
	}
	LAYER_PARA_DEBUG(A_debug_network_exit_suc);
    err = mesh_rc_data_layer_lower_upper(p_bear_enc, src_type);

    return err;
}

#if (MESH_MONITOR_EN)
u8 monitor_mode_en = 1;
u8 monitor_filter_sno_en = 1;

int mesh_rc_data_layer_network_monitor (u8 *p_payload)
{
	int err = 0;

	mesh_cmd_bear_unseg_t *p_bear_enc = GET_BEAR_FROM_ADV_PAYLOAD(p_payload);
    mesh_cmd_nw_t *p_nw = &p_bear_enc->nw;
    
    int len_dec_nw = mesh_bear_len_get(p_bear_enc) - (OFFSETOF(mesh_cmd_bear_unseg_t, lt) - 2);
    u8 val = 0;
	err = mesh_sec_msg_dec_nw((u8 *)p_nw, len_dec_nw, p_nw->nid, &val,0,ADV_FROM_MESH);	// not use friend key all the time
    if(err){return err;}
    
    if(is_exist_in_cache((u8 *)p_nw, 0, 1)){  // mainly for save cache.
    	return 0;
    }
    
	mesh_cmd_bear_unseg_t *p_bear = p_bear_enc;
	mesh_cmd_bear_unseg_t bear_backup;
	memcpy(&bear_backup, p_bear_enc, sizeof(mesh_cmd_bear_unseg_t));
	
	if(p_bear->lt.seg){
        endianness_swap((u8 *)p_nw, SWAP_TYPE_LT_SEG);  // must before
	}else{
		endianness_swap((u8 *)p_nw, SWAP_TYPE_LT_UNSEG);
        if(p_nw->ctl){
			mesh_cmd_lt_ctl_unseg_t *p_lt_ctl_unseg = (mesh_cmd_lt_ctl_unseg_t *)&p_bear->lt;
			if(CMD_CTL_ACK == p_lt_ctl_unseg->opcode){
				endianness_swap((u8 *)p_nw, SWAP_TYPE_LT_CTL_SEG_ACK);
			}else{
                endianness_swap((u8 *)p_nw, SWAP_TYPE_LT_CTL_UNSEG);
            }
        }else{
            mesh_cmd_lt_unseg_t *p_lt_unseg = &p_bear->lt;
            u8 len_ut = mesh_bear_len_get(p_bear) - OFFSETOF(mesh_cmd_bear_unseg_t, ut) - SZMIC_NW32;
            err = mesh_sec_msg_dec_apl(p_lt_unseg->data, len_ut, (u8 *)p_nw);
            if(!err){
				mesh_dongle_adv_report2vc(GET_ADV_PAYLOAD_FROM_BEAR(p_bear), MESH_MONITOR_DATA);
				return 0;
			}
        }
	}
	// report org msg
	mesh_dongle_adv_report2vc(GET_ADV_PAYLOAD_FROM_BEAR(&bear_backup), MESH_MONITOR_DATA);

    return 0;
}

int app_event_handler_adv_monitor(u8 *p_payload)
{
	if(monitor_mode_en){
		mesh_cmd_bear_unseg_t *p_bear_enc = GET_BEAR_FROM_ADV_PAYLOAD(p_payload);
		u8 adv_type = p_bear_enc->type;
		if(adv_type == MESH_ADV_TYPE_MESSAGE){
			mesh_rc_data_layer_network_monitor(p_payload);
		}
	}
	return 0;
}

#endif

// lower power node
mesh_fri_ship_proc_lpn_t fri_ship_proc_lpn = {};	// for VC

#if FEATURE_LOWPOWER_EN
mesh_lpn_subsc_list_t lpn_subsc_list;
mesh_subsc_list_retry_t subsc_list_retry = {};  // for retry procedure

STATIC_ASSERT(((GET_ADV_INTERVAL_MS(ADV_INTERVAL_MAX))* TRANSMIT_CNT_DEF < FRI_ESTABLISH_REC_DELAY_MS) && (FRI_REC_DELAY_MS > FRI_ESTABLISH_REC_DELAY_MS));
#endif
// common (lower power node and friend node)

mesh_lpn_par_t mesh_lpn_par = {0};

// friend node
int friend_cache_check_replace(u8 lpn_idx, mesh_cmd_bear_unseg_t *bear_big)
{
    int replace = 0;
    if(bear_big->nw.ctl){
        u8 op = bear_big->lt_ctl_unseg.opcode;
        if((CMD_CTL_UPDATE==op)||(CMD_CTL_ACK==op)){
            u8 r = irq_disable();
            mesh_cmd_bear_unseg_t *p_buf_bear;
            u8 cnt = my_fifo_data_cnt_get(fn_other_par[lpn_idx].p_cache);
            foreach(i,cnt){
                p_buf_bear = (mesh_cmd_bear_unseg_t *)my_fifo_get_offset(fn_other_par[lpn_idx].p_cache, i);
                // is big endianness in cache buff
                u8 op_buf = p_buf_bear->lt_ctl_unseg.opcode;
                if(CMD_CTL_UPDATE==op_buf){
                    if(i){  // the last one have been sent, just wait for check need retry or not.
                        replace = 1;    // always 1
                        memcpy(p_buf_bear->nw.sno, bear_big->nw.sno, 3);
                        if(memcmp(p_buf_bear->lt_ctl_unseg.data, bear_big->lt_ctl_unseg.data,sizeof(mesh_ctl_fri_update_t))){
                            memcpy(&p_buf_bear->lt_ctl_unseg.data, bear_big->lt_ctl_unseg.data, sizeof(mesh_ctl_fri_update_t));
                        }
                        break;
                    }
                }else if(CMD_CTL_ACK==op_buf){
                    if(p_buf_bear->lt_ctl_ack.seqzero == bear_big->lt_ctl_ack.seqzero){
                        replace = 1;
                        memcpy(p_buf_bear->nw.sno, bear_big->nw.sno, 3);
                        memcpy(&p_buf_bear->lt_ctl_ack.seg_map, &bear_big->lt_ctl_ack.seg_map, 4);
                        break;
                    }
                }
            }
            irq_restore(r);
        }
    }

    return replace;
}

int is_friend_ship_link_ok_fn(u8 lpn_idx)
{
    return fn_other_par[lpn_idx].link_ok;
}

int is_friend_ship_link_ok_lpn()
{
	return mesh_lpn_par.link_ok;
}

void friend_cmd_send_request()
{
	LOG_MSG_INFO(TL_LOG_FRIEND,(u8 *)&mesh_lpn_par.req, sizeof(mesh_ctl_fri_req_t),"send friend request:",0);
	mesh_lpn_par.link_ok = 0;
    mesh_lpn_par.req.LPNCounter++;   // must before
    mesh_tx_cmd_layer_upper_ctl(CMD_CTL_REQUEST, (u8 *)&mesh_lpn_par.req, sizeof(mesh_ctl_fri_req_t), ele_adr_primary, ADR_ALL_NODES,0);
}

void friend_cmd_send_offer(u8 lpn_idx)
{
	LOG_MSG_INFO(TL_LOG_FRIEND,(u8 *)(fn_offer+lpn_idx), sizeof(mesh_ctl_fri_offer_t),"send friend offer:",0);
    fn_offer[lpn_idx].FriCounter++; // must before
    mesh_tx_cmd_layer_upper_ctl_FN(CMD_CTL_OFFER, (u8 *)(fn_offer+lpn_idx), sizeof(mesh_ctl_fri_offer_t), fn_other_par[lpn_idx].LPNAdr);
    mesh_friend_key_update_all_nk(lpn_idx, fn_other_par[lpn_idx].nk_sel_dec_fn);
}

void friend_cmd_send_poll()
{
	LOG_MSG_INFO(TL_LOG_FRIEND,(u8 *)(&mesh_lpn_par.poll), sizeof(mesh_ctl_fri_poll_t),"send friend poll:",0);
    fri_ship_proc_lpn.poll_tick = clock_time()|1;
    mesh_tx_cmd_layer_upper_ctl(CMD_CTL_POLL, (u8 *)(&mesh_lpn_par.poll), sizeof(mesh_ctl_fri_poll_t), ele_adr_primary, mesh_lpn_par.FriAdr,0);
}

void friend_cmd_send_update(u8 lpn_idx, u8 md)
{
    mesh_net_key_t *p_netkey = &mesh_key.net_key[fn_other_par[lpn_idx].nk_sel_dec_fn][0];
    get_iv_update_key_refresh_flag(&fn_update[lpn_idx].flag, fn_update[lpn_idx].IVIndex, p_netkey->key_phase);
    fn_update[lpn_idx].md = !!md;
    LOG_MSG_INFO(TL_LOG_FRIEND,(u8 *)(fn_update+lpn_idx), sizeof(mesh_ctl_fri_update_t),"send friend update:",0);
    mesh_tx_cmd_layer_upper_ctl_FN(CMD_CTL_UPDATE, (u8 *)(fn_update+lpn_idx), sizeof(mesh_ctl_fri_update_t), fn_other_par[lpn_idx].LPNAdr);
}

void friend_cmd_send_clear(u16 adr_dst, u8 *par, u32 len)
{	
	LOG_MSG_INFO(TL_LOG_FRIEND,par, len,"send friend clear:",0);
    mesh_tx_cmd_layer_upper_ctl(CMD_CTL_CLEAR, par, len, ele_adr_primary, adr_dst,0);
}

void friend_cmd_send_clear_conf(u16 adr_dst, u8 *par, u32 len)
{
	LOG_MSG_INFO(TL_LOG_FRIEND,par, len,"send friend clear confirm:",0);
    mesh_tx_cmd_layer_upper_ctl_FN(CMD_CTL_CLR_CONF, par, len, adr_dst);	
}

void friend_cmd_send_subsc_conf(u16 adr_dst, u8 transNo)
{
	LOG_MSG_INFO(TL_LOG_FRIEND, (u8 *)&transNo, 1,"send friend sub list confirm:",0);
    use_mesh_adv_fifo_fn2lpn = 1;
    mesh_tx_cmd_layer_upper_ctl_FN(CMD_CTL_SUBS_LIST_CONF, (u8 *)&transNo, 1, adr_dst);
    use_mesh_adv_fifo_fn2lpn = 0;
}

#if FEATURE_LOWPOWER_EN
void friend_cmd_send_subsc_add(u8 *par_subsc, u32 len)  // only LPN support
{
	LOG_MSG_INFO(TL_LOG_FRIEND,(u8 *)par_subsc, len,"send friend sub list add:",0);
    subsc_list_retry.tick = clock_time();
    mesh_tx_cmd_layer_upper_ctl(CMD_CTL_SUBS_LIST_ADD, (u8 *)par_subsc, len, ele_adr_primary, mesh_lpn_par.FriAdr,0);
}

void friend_cmd_send_subsc_rmv(u8 *par_subsc, u32 len)  // only LPN support
{
	LOG_MSG_INFO(TL_LOG_FRIEND,(u8 *)par_subsc, len,"send friend sub list remove:",0);
    subsc_list_retry.tick = clock_time();
    mesh_tx_cmd_layer_upper_ctl(CMD_CTL_SUBS_LIST_REMOVE, (u8 *)par_subsc, len, ele_adr_primary, mesh_lpn_par.FriAdr,0);
}

void friend_subsc_repeat()
{
    if(fri_ship_proc_lpn.poll_retry){
        fri_ship_proc_lpn.poll_tick = clock_time()|1;
    }
    
    if(SUBSC_ADD == subsc_list_retry.retry_type){
        friend_cmd_send_subsc_add(&subsc_list_retry.TransNo, 1+subsc_list_retry.subsc_cnt*2);
    }else if(SUBSC_REMOVE == subsc_list_retry.retry_type){
        friend_cmd_send_subsc_rmv(&subsc_list_retry.TransNo, 1+subsc_list_retry.subsc_cnt*2);
    }
}

inline void friend_subsc_stop()
{
    subsc_list_retry.retry_cnt = 0;
}

int friend_subsc_list_cmd_start(u8 type, u16 *adr_list, u32 subsc_cnt)
{
    if(subsc_cnt > SUB_LIST_MAX_IN_ONE_MSG){
        return -1;
    }
    
    subsc_list_retry.subsc_cnt = subsc_cnt;
    subsc_list_retry.TransNo = lpn_subsc_list.TransNo;
    memset(subsc_list_retry.adr, 0, sizeof(subsc_list_retry.adr));
    memcpy(subsc_list_retry.adr, adr_list, subsc_cnt * 2);
    subsc_list_retry.retry_cnt = lpn_get_poll_retry_max();
    subsc_list_retry.retry_type = type;
    
    lpn_subsc_list.TransNo++;

    friend_subsc_repeat();  // send first message

    return 0;
}
#endif

inline void friend_subsc_list_add_adr(lpn_adr_list_t *adr_list_src, lpn_adr_list_t *adr_list_add, u32 cnt)
{
    // should not use u16* as parameter,because it is not sure 2bytes aligned
    foreach(i,cnt){
        int exit_flag = 0;
        foreach(j,SUB_LIST_MAX_LPN){
            if(adr_list_add->adr[i] == adr_list_src->adr[j]){
                exit_flag = 1;
                break;
            }
        }
        
        if(!exit_flag){
            foreach(k,SUB_LIST_MAX_LPN){
                if(0 == adr_list_src->adr[k]){
                    adr_list_src->adr[k] = adr_list_add->adr[i];
                    break;
                }
            }
        }
    }
}

// use 'inline' should be better. 
inline void friend_subsc_list_rmv_adr(lpn_adr_list_t *adr_list_src, lpn_adr_list_t *adr_list_rmv, u32 cnt)
{
    // should not use u16* as parameter,because it is not sure 2bytes aligned
    foreach(i,cnt){
        foreach(j,SUB_LIST_MAX_LPN){
            if(adr_list_rmv->adr[i] == adr_list_src->adr[j]){
                adr_list_src->adr[j] = 0;
                //break;
            }
        }
    }
}

#if FEATURE_LOWPOWER_EN
void friend_subsc_add(u16 *adr_list, u32 subsc_cnt)
{
    friend_subsc_list_cmd_start(SUBSC_ADD, adr_list, subsc_cnt);
}

void friend_subsc_rmv(u16 *adr_list, u32 subsc_cnt)
{
    friend_subsc_list_cmd_start(SUBSC_REMOVE, adr_list, subsc_cnt);
    friend_subsc_list_rmv_adr((lpn_adr_list_t *)(&lpn_subsc_list.adr), (lpn_adr_list_t *)adr_list, subsc_cnt);
}
#endif

#if TEST_CASE_HBS_BV_05_EN
void friend_add_special_grp_addr()
{
	u16 sub_adr = 0xc100;
	friend_subsc_list_add_adr(fn_other_par[0].SubsList, &sub_adr, 1);
	return ;
}
#endif

void friend_cmd_send_fn(u8 lpn_idx, u8 op)
{
#if (FRI_SAMPLE_EN)
	friend_cmd_send_sample_message(op);
#else
    if(CMD_CTL_OFFER == op){
        #if FRIEND_MSG_TEST_EN
        if(mesh_adv_tx_cmd_sno < 0x014820){
            mesh_adv_tx_cmd_sno = 0x014820; // for test
        }
        #endif
        friend_cmd_send_offer(lpn_idx);
    }else if(CMD_CTL_UPDATE == op){
        #if FRIEND_MSG_TEST_EN
        if(mesh_adv_tx_cmd_sno < 0x014834){
            mesh_adv_tx_cmd_sno = 0x014834; // for test
        }
        #endif
        friend_cmd_send_update(lpn_idx, 0);
    }else if(CMD_CTL_CLEAR == op){
		mesh_ctl_fri_clear_t fri_clear;
		u16 adr_dst = 0;
		#if FEATURE_LOWPOWER_EN
		fri_clear.LPNAdr = mesh_lpn_par.LPNAdr;
		fri_clear.LPNCounter = 2;       // comfirm later, should use parameters in last request command.
		adr_dst = mesh_lpn_par.FriAdr;  // comfirm later, should use parameters in last request command.
		#else
		fri_clear.LPNAdr = fn_other_par[lpn_idx].LPNAdr;
		fri_clear.LPNCounter = fn_req[lpn_idx].LPNCounter;
		adr_dst = fn_req[lpn_idx].PreAdr;
		#endif
        friend_cmd_send_clear(adr_dst, (u8 *)&fri_clear, sizeof(mesh_ctl_fri_clear_t));
    }else if(CMD_CTL_HEARTBEAT == op){
		u16 feature =0;
		memcpy((u8 *)(&feature),(u8 *)&(gp_page0->head.feature),2);
		heartbeat_cmd_send_conf(model_sig_cfg_s.hb_pub.ttl,
			feature,model_sig_cfg_s.hb_pub.dst_adr);
	}
#endif
}

void mesh_friend_ship_set_st_lpn(u8 st)
{
    fri_ship_proc_lpn.status = st;
}

int is_in_mesh_friend_st_lpn()
{
	if(is_lpn_support_and_en){
    	return (is_friend_ship_link_ok_lpn() && (mesh_lpn_par.FriAdr != 0));
	}else{
		return 0;
	}
}

int is_unicast_friend_msg_to_fn(mesh_cmd_nw_t *p_nw)
{
    return (is_in_mesh_friend_st_lpn()
        && (mesh_lpn_par.LPNAdr == p_nw->src)
        && (mesh_lpn_par.FriAdr == p_nw->dst));    // must because of group address
}

int is_must_use_friend_key_msg(mesh_cmd_nw_t *p_nw)
{
    int friend_key = 0;
    if(p_nw->ctl){
        mesh_cmd_bear_unseg_t *p_br = CONTAINER_OF((mesh_cmd_nw_t *)p_nw, mesh_cmd_bear_unseg_t, nw);
        u8 op = p_br->lt_ctl_unseg.opcode;
        if((CMD_CTL_POLL == op)||(CMD_CTL_SUBS_LIST_ADD == op)
        || (CMD_CTL_SUBS_LIST_REMOVE == op)
        ||(CMD_CTL_SUBS_LIST_CONF == op)||(CMD_CTL_UPDATE == op)){	// friend
            friend_key = 1; // always
        }
    }
    return friend_key;
}

int is_not_cache_ctl_msg_fn(mesh_cmd_nw_t *p_nw)
{
    if(p_nw->ctl){
        mesh_cmd_bear_unseg_t *p_br = CONTAINER_OF((mesh_cmd_nw_t *)p_nw, mesh_cmd_bear_unseg_t, nw);
        u8 op = p_br->lt_ctl_unseg.opcode;
        if((CMD_CTL_OFFER == op)||(CMD_CTL_SUBS_LIST_CONF == op)||(CMD_CTL_CLR_CONF == op)){
            return 1;
        }
    }
    return 0;
}

int is_use_friend_key_lpn(mesh_cmd_nw_t *p_nw, int Credential_Flag)
{
    int friend_key = 0;
    if(p_nw->ctl){
        friend_key = is_must_use_friend_key_msg(p_nw);
    }else{
        if(Credential_Flag && is_friend_ship_link_ok_lpn()){ // refer to spec page 138 4.2.2.4 
            friend_key = 1;
        }
    }

    return friend_key;
}

void lpn_quick_send_adv()
{
#ifndef WIN32
	u8 r = irq_disable();
    if(blt_state == BLS_LINK_STATE_ADV || ((blt_state == BLS_LINK_STATE_CONN)&&(ble_state == BLE_STATE_BRX_E))){
        blt_send_adv2scan_mode(1);
    }
    irq_restore(r);
#endif
}

void mesh_friend_ship_proc_init_lpn()
{
    memset(&fri_ship_proc_lpn, 0, sizeof(mesh_fri_ship_proc_lpn_t));
}

void mesh_friend_ship_clear_LPN()
{
    mesh_ctl_fri_req_t req_bacup;
    memcpy(&req_bacup, &mesh_lpn_par.req, sizeof(req_bacup));
    memset(&mesh_lpn_par, 0, sizeof(mesh_lpn_par));
    memcpy(&mesh_lpn_par.req, &req_bacup, sizeof(req_bacup));
}

u32 mesh_get_val_with_factor(u32 val, u32 fac)
{
    u32 result;
    if(1 == fac){
        result = (val * 3) / 2;
    }else if(2 == fac){
        result = (val * 2);
    }else if(3 == fac){
        result = (val * 5) / 2;
    }else{
        result = val;
    }
    return result;
}

#define get_min_cache_size_by_log(n)    (1 << n)

int mesh_friend_request_is_valid(mesh_ctl_fri_req_t *p_req)
{
    return (/*(!p_req->Criteria.RFU)&&*/(p_req->Criteria.MinCacheSizeLog)&&(p_req->RecDelay >= 0x0A)
          &&((p_req->PollTimeout >= 0x0A)&&(p_req->PollTimeout <= 0x34BBFF))&&p_req->NumEle);
}

u32 get_poll_timeout_fn(u16 lpn_adr)
{
    foreach_arr(i,fn_other_par){
        if(is_friend_ship_link_ok_fn(i) && (lpn_adr == fn_other_par[i].LPNAdr)){
            return fn_req[i].PollTimeout;
        }
    }
    return 0;
}

static inline int mesh_friend_poll_is_valid(mesh_ctl_fri_poll_t *p_poll)
{
    return (0 == p_poll->RFU);
}

#if FEATURE_LOWPOWER_EN
void mesh_friend_ship_proc_LPN(u8 *bear)
{
    if(blt_state == BLS_LINK_STATE_CONN){
        return ;
    }
    
    static u32 t_rec_delay_and_win = 0;
    u32 poll_retry_interval_ms = t_rec_delay_and_win;
	u32 timeout_ms = (poll_retry_interval_ms*1000) * (2*2+1)/2;    // comfirm later
    #if (0 == DEBUG_PROXY_FRIEND_SHIP)
    if(pts_test_en && fri_ship_proc_lpn.status){
        // retry poll should be more quicklier during establish friend ship.
        #define RETRY_POLL_INTV_MS_MAX      170 // FRND-LPN-BI01 need retry 3 times during 1 second after get offer.
        if(poll_retry_interval_ms > RETRY_POLL_INTV_MS_MAX){
            poll_retry_interval_ms = RETRY_POLL_INTV_MS_MAX;
        }
    }
    #endif
    if(fri_ship_proc_lpn.poll_retry && (!bear)
     && clock_time_exceed(fri_ship_proc_lpn.poll_tick, poll_retry_interval_ms*1000)){
        fri_ship_proc_lpn.poll_retry--;
        if(0 == fri_ship_proc_lpn.poll_retry){
            if(FRI_ST_UPDATE == fri_ship_proc_lpn.status){
                // retry request in "case FRI_ST_UPDATE:"
            }else{
                friend_subsc_stop();
                mesh_cmd_sig_lowpower_heartbeat();
                friend_ship_disconnect_cb_lpn();
                mesh_friend_ship_set_st_lpn(FRI_ST_REQUEST);    // restart establish procedure
            }
        }else{
            friend_cmd_send_poll();  // retry
        }
    }
    else if(subsc_list_retry.retry_cnt && (!bear) && clock_time_exceed(subsc_list_retry.tick, timeout_ms)){
        subsc_list_retry.tick = clock_time();   // also refresh when send_subsc
        subsc_list_retry.retry_cnt--;
        if(0 == subsc_list_retry.retry_cnt){
            // whether restart establish procedure or not
            mesh_friend_ship_set_st_lpn(FRI_ST_REQUEST);    // restart establish procedure
        }else{
            LOG_MSG_INFO(TL_LOG_FRIEND, 0, 0,"friend_subsc_repeat_***********************",0);
            friend_subsc_repeat();
        }
    }
	
    mesh_cmd_bear_unseg_t *p_bear = (mesh_cmd_bear_unseg_t *)bear;
    //mesh_cmd_nw_t *p_nw = &p_bear->nw;
    mesh_cmd_lt_ctl_unseg_t *p_lt_ctl_unseg = &p_bear->lt_ctl_unseg;
    u8 op = -1;
    if(bear){
	    op = p_lt_ctl_unseg->opcode;
    }
    
    if(0 == fri_ship_proc_lpn.status){
        if(bear){
            if(CMD_CTL_SUBS_LIST_CONF == op){
                mesh_ctl_fri_subsc_list_t *p_subsc = CONTAINER_OF(p_lt_ctl_unseg->data,mesh_ctl_fri_subsc_list_t,TransNo);
                if(p_subsc->TransNo == (subsc_list_retry.TransNo)){   //TransNo have increased
                	LOG_MSG_INFO(TL_LOG_FRIEND,(u8 *)&p_subsc->TransNo, sizeof(p_subsc->TransNo),"rcv sub list confirm:",0);
                    if(SUBSC_ADD == subsc_list_retry.retry_type){
                        friend_subsc_list_add_adr((lpn_adr_list_t *)(&lpn_subsc_list.adr), (lpn_adr_list_t *)(&subsc_list_retry.adr), subsc_list_retry.subsc_cnt);
                    }
                    friend_subsc_stop();
                }
            }else if(CMD_CTL_UPDATE == op){
                mesh_ctl_fri_update_t *p_update = (mesh_ctl_fri_update_t *)(p_lt_ctl_unseg->data);
				LOG_MSG_INFO(TL_LOG_FRIEND,(u8 *)p_update, sizeof(mesh_ctl_fri_update_t),"NW_IVI %d, rcv friend update:",p_bear->nw.ivi);
                iv_update_key_refresh_rx_handle(&p_update->flag, p_update->IVIndex);

            }
        }else{
        }
    }else{
        switch(fri_ship_proc_lpn.status){   // Be true only during establish friend ship.
            case FRI_ST_REQUEST:
                fri_ship_proc_lpn.req_tick = clock_time();
                friend_cmd_send_request();

                // init par
                mesh_friend_ship_clear_LPN();
                mesh_lpn_par.LPNAdr = ele_adr_primary;
                mesh_friend_ship_set_st_lpn(FRI_ST_OFFER);
                break;
            case FRI_ST_OFFER:
                if(bear){
                    if(CMD_CTL_OFFER == p_lt_ctl_unseg->opcode){
                        if(0 != lpn_rx_offer_handle(bear)){
                            break;
                        }
                    }
                }else{
                    if(clock_time_exceed(fri_ship_proc_lpn.req_tick, FRI_ESTABLISH_PERIOD_MS*1000)){
                        if(mesh_lpn_par.FriAdr){
                            mesh_lpn_par.link_ok = 1;
                            mesh_friend_key_update_all_nk(0, 0);
                        }
                        mesh_friend_ship_set_st_lpn(FRI_ST_POLL);
                    }
                }
                break;
            case FRI_ST_POLL:
                if(is_friend_ship_link_ok_lpn()){
                    mesh_lpn_par.poll.FSN = 0;   // init
                    // send poll
                    fri_ship_proc_lpn.poll_retry = FRI_GET_UPDATE_RETRY_MAX + 1;
                    friend_cmd_send_poll();
                    
                    t_rec_delay_and_win = mesh_lpn_par.req.RecDelay + mesh_lpn_par.offer.RecWin;
                    mesh_friend_ship_set_st_lpn(FRI_ST_UPDATE);
                }else{
                    lpn_no_offer_handle();
                }
                break;
            case FRI_ST_UPDATE:
                if(bear){   // current state is establishing friend ship
                    if(CMD_CTL_UPDATE == p_lt_ctl_unseg->opcode){
                        mesh_ctl_fri_update_t *p_update = (mesh_ctl_fri_update_t *)(p_lt_ctl_unseg->data);
						LOG_MSG_INFO(TL_LOG_FRIEND,(u8 *)p_update, sizeof(mesh_ctl_fri_update_t),"rcv friend update:",0);
                        memcpy(&mesh_lpn_par.update, p_update, sizeof(mesh_ctl_fri_update_t));
                        //friend ship establish done
                        mesh_lpn_par.req.PreAdr = mesh_lpn_par.FriAdr;
						mesh_cmd_sig_lowpower_heartbeat();
                        iv_update_key_refresh_rx_handle(&p_update->flag, p_update->IVIndex);
                        mesh_friend_ship_proc_init_lpn();

                        friend_ship_establish_ok_cb_lpn();
                    }
                }else{
                    if(clock_time_exceed(fri_ship_proc_lpn.poll_tick, t_rec_delay_and_win*1000)){
                        mesh_friend_ship_retry();
                    }
                }
                break;
            default:
                break;
        }
    }
}
#endif

inline void mesh_friend_ship_set_st_fn(u8 lpn_idx, u8 st)
{
    fri_ship_proc_fn[lpn_idx].status = st;
}

void mesh_friend_ship_proc_init_fn(u8 lpn_idx)
{
    memset(&fri_ship_proc_fn[lpn_idx], 0, sizeof(mesh_fri_ship_proc_fn_t));
}

void mesh_friend_ship_clear_FN(u8 lpn_idx)
{
	mesh_friend_ship_proc_init_fn(lpn_idx);
    memset(fn_other_par+lpn_idx, 0, sizeof(fn_other_par[0]));
    memset(fn_req+lpn_idx, 0, sizeof(fn_req[0]));
    memset(fn_poll+lpn_idx, 0, sizeof(fn_poll[0]));
    memset(fn_ctl_rsp_delay+lpn_idx, 0, sizeof(fn_ctl_rsp_delay[0]));
    mesh_fri_cache_fifo[lpn_idx].wptr = mesh_fri_cache_fifo[lpn_idx].rptr = 0;
}

void friend_ship_disconnect_fn(u8 lpn_idx, int type)
{
    LOG_MSG_INFO(TL_LOG_FRIEND,0, 0,"Friend ship disconnect, LPN addr:0x%04x, type: %d ", fn_other_par[lpn_idx].LPNAdr, type);
    friend_ship_disconnect_cb_fn(lpn_idx, type);
    mesh_friend_ship_clear_FN(lpn_idx);
}

void mesh_friend_ship_init_all()
{
    foreach(i,g_max_lpn_num){
        mesh_friend_ship_clear_FN(i);
    }
}

inline u8* mesh_friend_ship_cache_check(my_fifo_t *f)
{
    return my_fifo_get(f);
}

int is_poll_cmd(mesh_cmd_nw_t *p_nw)
{
    mesh_cmd_bear_unseg_t *p_bear = CONTAINER_OF(p_nw,mesh_cmd_bear_unseg_t,nw);
    return (p_bear->nw.ctl && (CMD_CTL_POLL == p_bear->lt_ctl_unseg.opcode));
}

int is_in_mesh_friend_st_fn(u8 lpn_idx)
{
	if(is_fn_support_and_en){
    	return (is_friend_ship_link_ok_fn(lpn_idx) && (fn_other_par[lpn_idx].LPNAdr != 0));
	}else{
		return 0;
	}
}

int is_unicast_friend_msg_from_lpn(mesh_cmd_nw_t *p_nw)
{
	foreach(i,g_max_lpn_num){
	    if(is_in_mesh_friend_st_fn(i)
	        && (fn_other_par[i].LPNAdr == p_nw->src)
	        && (fn_other_par[i].FriAdr == p_nw->dst)){    // must because of group address
	        return 1;
	    }
	}
	return 0;
}

int is_unicast_friend_msg_to_lpn(mesh_cmd_nw_t *p_nw)
{
	if(is_unicast_adr(p_nw->dst)){
		foreach(i,g_max_lpn_num){
		    if(is_in_mesh_friend_st_fn(i)
		        && (fn_other_par[i].LPNAdr == p_nw->dst)
		        && (fn_other_par[i].FriAdr == p_nw->src)){    // must because of relay message
		        return 1;
		    }
		}
	}
	return 0;
}

int is_cmd2lpn(u16 adr_dst)
{
	foreach(i,g_max_lpn_num){
		if(fn_other_par[i].LPNAdr == adr_dst){
			return 1;
		}
	}
	return 0;
}

u32 mesh_friend_local_delay(u8 lpn_idx)		// for FN
{
    int t_delay = mesh_get_val_with_factor(fn_offer[lpn_idx].RecWin, fn_req[lpn_idx].Criteria.RecWinFac)
                - mesh_get_val_with_factor(fn_offer[lpn_idx].RSSI, fn_req[lpn_idx].Criteria.RSSIFac);

    if(t_delay < 100){
        t_delay = 100;    // spec required.
    }

    return t_delay;
}

inline void mesh_stop_clear_cmd(u8 lpn_idx)
{
	#if 1
	fri_ship_proc_fn[lpn_idx].clear_poll = 0;
	#else
	mesh_friend_ship_proc_init_fn(lpn_idx);
	#endif
}

void mesh_reset_poll_timeout_timer(u8 lpn_idx)
{
    fri_ship_proc_fn[lpn_idx].poll_tick = clock_time_100ms();
}

enum{
    DELAY_POLL = 1,
    DELAY_SUBSC_LIST,
    DELAY_CLEAR_CONF,
};

void mesh_friend_set_delay_par_poll(u8 lpn_idx, u8 *rsp, u32 timeStamp)
{
    fn_ctl_rsp_delay_t *p_delay = &fn_ctl_rsp_delay[lpn_idx];
    p_delay->delay_type = DELAY_POLL;
    p_delay->tick = timeStamp;
    p_delay->poll_rsp = rsp;

	mesh_friend_logmsg((mesh_cmd_bear_unseg_t *)rsp, ((mesh_cmd_bear_unseg_t *)rsp)->len + 2);
}

void mesh_friend_set_delay_par(u8 delay_type, u8 lpn_idx, u16 adr_dst, u16 par_val)
{
    fn_ctl_rsp_delay_t *p_delay = &fn_ctl_rsp_delay[lpn_idx];
    p_delay->delay_type = delay_type;
    p_delay->tick = clock_time();
    p_delay->adr_dst = adr_dst;
    p_delay->par_val = par_val;
}

void mesh_friend_response_delay_proc_fn(u8 lpn_idx)
{
    fn_ctl_rsp_delay_t *p_delay = &fn_ctl_rsp_delay[lpn_idx];
    
    if(p_delay->delay_type && clock_time_exceed(p_delay->tick, fn_req[lpn_idx].RecDelay * 1000 - 1800)){    // 1800us: encryption pkt time
        if(DELAY_POLL == p_delay->delay_type){
            if(p_delay->poll_rsp){
                mesh_cmd_bear_unseg_t bear_temp;
                memcpy(&bear_temp, p_delay->poll_rsp, sizeof(mesh_cmd_bear_unseg_t));
	            //LOG_MSG_INFO(TL_LOG_FRIEND,(u8 *)&bear_temp.len, bear_temp.len+1,"Data for poll:",0);
                
                mesh_sec_msg_enc_nw_rf_buf((u8 *)(&bear_temp.nw), mesh_lt_len_get_by_bear(&bear_temp), 1+lpn_idx,0,fn_other_par[lpn_idx].nk_sel_dec_fn);
                mesh_tx_cmd_add_packet_fn2lpn((u8 *)&bear_temp);
            }

            mesh_fri_ship_proc_fn_t *proc_fn = &fri_ship_proc_fn[lpn_idx];
            if(proc_fn->clear_delay_cnt){
                proc_fn->clear_delay_cnt--;
                if(0 == proc_fn->clear_delay_cnt){ // make sure establish friend ship success
                    friend_cmd_send_fn(lpn_idx, CMD_CTL_CLEAR); // use normal fifo
                    proc_fn->clear_cmd_tick = proc_fn->clear_start_tick = clock_time_100ms();
                    proc_fn->clear_int_100ms = FRI_FIRST_CLR_INTERVAL_100MS;
                    proc_fn->clear_poll = 1;
                }
            }
        }else if(DELAY_SUBSC_LIST == p_delay->delay_type){
            friend_cmd_send_subsc_conf(p_delay->adr_dst, (u8)p_delay->par_val);
        }else if(DELAY_CLEAR_CONF == p_delay->delay_type){
            mesh_ctl_fri_clear_t clear;
            clear.LPNAdr = fn_other_par[lpn_idx].LPNAdr;
            clear.LPNCounter = p_delay->par_val;
            use_mesh_adv_fifo_fn2lpn = 1;
            friend_cmd_send_clear_conf(clear.LPNAdr, (u8 *)&clear, sizeof(mesh_ctl_fri_clear_t));
            use_mesh_adv_fifo_fn2lpn = 0;
        }
        
        p_delay->delay_type = 0;
    }
}

void mesh_friend_ship_proc_FN(u8 *bear)
{
	foreach(i,g_max_lpn_num){
		mesh_fri_ship_proc_fn_t *proc_fn = &fri_ship_proc_fn[i];
	    if(!bear){
	        if(proc_fn->status){ // (FRI_ST_IDLE != proc_fn->status)
    	        if(FRI_ST_OFFER == proc_fn->status){
    	            if(clock_time_exceed(proc_fn->offer_tick, proc_fn->offer_delay*1000)){
    	                use_mesh_adv_fifo_fn2lpn = 1;
    	                friend_cmd_send_fn(i, CMD_CTL_OFFER);
    	                use_mesh_adv_fifo_fn2lpn = 0;
    	                proc_fn->offer_tick = clock_time()|1;
    	                mesh_friend_ship_set_st_fn(i, FRI_ST_POLL);
    	            }
    	        }else if(FRI_ST_POLL == proc_fn->status){
    	            // add 500ms, because handling response of POLL was delay some ten ms. 
    	            if(clock_time_exceed(proc_fn->offer_tick, (500+FRI_ESTABLISH_OFFER_MS)*1000)){
    	                mesh_friend_ship_proc_init_fn(i);
    	            }
    	        }else if(FRI_ST_TIMEOUT_CHECK == proc_fn->status){
    	            if(clock_time_exceed_100ms(proc_fn->poll_tick, (u32)(fn_req[i].PollTimeout))){
    	                friend_ship_disconnect_fn(i, FS_DISCONNECT_TYPE_POLL_TIMEOUT);
    	            }
    	        }
	        }
	        	        
	        if(proc_fn->clear_poll){    // clear by other FN
	            if(clock_time_exceed_100ms(proc_fn->clear_start_tick, (u32)(fn_req[i].PollTimeout)*2)){
	                mesh_stop_clear_cmd(i);
	            }else{
	                if(clock_time_exceed_100ms(proc_fn->clear_cmd_tick, proc_fn->clear_int_100ms)){
	                    proc_fn->clear_cmd_tick = clock_time_100ms();
	                    proc_fn->clear_int_100ms = proc_fn->clear_int_100ms << 1;
	                    friend_cmd_send_fn(i, CMD_CTL_CLEAR);
	                }
	            }
	        }

	        if(proc_fn->clear_by_lpn_tick && clock_time_exceed(proc_fn->clear_by_lpn_tick, 5*1000*1000)){
	            // because LPN may retry send clear command when not receive clear comfirm.
                friend_ship_disconnect_fn(i, FS_DISCONNECT_TYPE_CLEAR);
	        }
	    }else{
	        mesh_cmd_bear_unseg_t *p_bear = (mesh_cmd_bear_unseg_t *)bear;
	        mesh_cmd_nw_t *p_nw = &p_bear->nw;
	        mesh_cmd_lt_ctl_unseg_t *p_lt_ctl_unseg = &p_bear->lt_ctl_unseg;
	        u8 op = p_lt_ctl_unseg->opcode;
	        if(!((CMD_CTL_REQUEST == op)||(CMD_CTL_CLEAR == op)||(CMD_CTL_CLR_CONF == op))
			 && !(proc_fn->status && (fn_other_par[i].LPNAdr == p_nw->src))){
	        	continue ;
	        }

	        if(CMD_CTL_REQUEST == op){
	            mesh_ctl_fri_req_t *p_req = (mesh_ctl_fri_req_t *)(p_lt_ctl_unseg->data);
				#if DEBUG_SUSPEND
	            static u8 mesh_lpn_debug_req;mesh_lpn_debug_req++;
	            static mesh_ctl_fri_req_t mesh_lpn_debug_req_buf;
	            memcpy(&mesh_lpn_debug_req_buf, p_req, sizeof(mesh_ctl_fri_req_t));
				#endif
	            if(0 == mesh_friend_request_is_valid(p_req)){
                    LOG_MSG_INFO(TL_LOG_FRIEND,(u8 *)p_req, sizeof(mesh_ctl_fri_req_t),"Error:rcv Invalid friend request:",0);
	                return ;
	            }
				
				#if DEBUG_SUSPEND
	            static u8 mesh_lpn_debug_req2;mesh_lpn_debug_req2++;
				#endif

	            if(0 == proc_fn->status){
                    mesh_friend_ship_clear_FN(i);   // just init parameters
	            }else if(p_nw->src == fn_other_par[i].LPNAdr){  // was friend before
                    friend_ship_disconnect_fn(i, FS_DISCONNECT_TYPE_RX_REQUEST);
	            }else{
					continue;
	            }
				
				#if DEBUG_SUSPEND
	            static u8 mesh_lpn_debug_req3;mesh_lpn_debug_req3++;
				#endif
	            LOG_MSG_INFO(TL_LOG_FRIEND,(u8 *)p_req, sizeof(mesh_ctl_fri_req_t),"rcv friend request:",0);
	            fn_other_par[i].LPNAdr = p_nw->src;
	            fn_other_par[i].FriAdr = ele_adr_primary;
	            fn_other_par[i].p_cache = &mesh_fri_cache_fifo[i];
	            fn_other_par[i].nk_sel_dec_fn = mesh_key.netkey_sel_dec;
	            fn_other_par[i].TransNo = -1; // init to be different with the first TransNo.
	            fn_poll[i].FSN = 1;           // init to be different with the first poll FSN.
	            memcpy(fn_req+i, p_req,sizeof(mesh_ctl_fri_req_t));
	            proc_fn->offer_tick = clock_time()|1;
	            proc_fn->offer_delay = mesh_friend_local_delay(i);
	            mesh_friend_ship_set_st_fn(i, FRI_ST_OFFER);
	            return ;
	        }else if(CMD_CTL_POLL == op){
	            mesh_ctl_fri_poll_t *p_poll = (mesh_ctl_fri_poll_t *)(p_lt_ctl_unseg->data);
	            if(0 == mesh_friend_poll_is_valid(p_poll)){
                    LOG_MSG_INFO(TL_LOG_FRIEND,(u8 *)p_poll, sizeof(mesh_ctl_fri_poll_t),"Error:rcv Invalid friend poll:",0);
	                return ;
	            }
	            
                if(fn_other_par[i].FriAdr != p_nw->dst){
                    continue ;
                }
				
				#if 0
                static u8 AA_discard_cnt = 0;
                if(AA_discard_cnt){
                    AA_discard_cnt --;
                    return ;
                }
				#endif
	            LOG_MSG_INFO(TL_LOG_FRIEND,(u8 *)p_poll, sizeof(mesh_ctl_fri_poll_t),"rcv friend poll:",0);
	            if(proc_fn->offer_tick){
					fn_other_par[i].link_ok = 1;
	                mesh_friend_ship_proc_init_fn(i);    // init parameters after establish friend ship
	                if(fn_req[i].PreAdr && !is_own_ele(fn_req[i].PreAdr)){
	                    proc_fn->clear_delay_cnt = 1;   // 1: means no delay
	                }
	                friend_ship_establish_ok_cb_fn(i);
	            }else if(proc_fn->clear_by_lpn_tick){
                    friend_ship_disconnect_fn(i, FS_DISCONNECT_TYPE_CLEAR);
                    return ;
	            }
	            
	            my_fifo_t *f_cache = fn_other_par[i].p_cache;
	            u8 *p_br_cache = mesh_friend_ship_cache_check(f_cache);
	            
	            if(fn_poll[i].FSN != p_poll->FSN){
	                fn_poll[i].FSN = p_poll->FSN;
	                if(p_br_cache){
	                    my_fifo_pop(f_cache);
	                }
	                p_br_cache = mesh_friend_ship_cache_check(f_cache);
	                if(p_br_cache){
                        LOG_MSG_INFO(TL_LOG_FRIEND,p_br_cache+1, 16,"FN Cache message of NW(Big endian):",0);
	                }else{
	                	#if FN_PRIVATE_SEG_CACHE_EN
	                    if(is_friend_seg_cache_busy()){
	                        friend_seg_cache2friend_cache(BIT(i));
	                    }else
	                    #endif
	                    {
	                        friend_cmd_send_fn(i, CMD_CTL_UPDATE);    // push to cache
	                    }
	                    p_br_cache = mesh_friend_ship_cache_check(f_cache);
	                }
	            }else{
                    LOG_MSG_INFO(TL_LOG_FRIEND,0, 0,"FN Cache retry",0);
	            }

                event_adv_report_t *pa = CONTAINER_OF(&p_bear->len,event_adv_report_t,data[0]);
                adv_report_extend_t *p_extend = (adv_report_extend_t *)(pa->data+(p_bear->len+1));
                u32 timeStamp = p_extend->timeStamp;
                #ifndef WIN32
                if((u32)(clock_time() - timeStamp) > 100*1000*sys_tick_per_us){
                    timeStamp = clock_time();
                }
                #endif
                
	            mesh_friend_set_delay_par_poll(i, p_br_cache, timeStamp);

	            // start poll timeout check
	            mesh_friend_ship_set_st_fn(i, FRI_ST_TIMEOUT_CHECK);
	            mesh_reset_poll_timeout_timer(i);
	            return ;
	        }else if(CMD_CTL_CLEAR == op){
	        	mesh_ctl_fri_clear_t *p_fri_clear = (mesh_ctl_fri_clear_t *)p_lt_ctl_unseg->data;
	            if((fn_other_par[i].LPNAdr == p_fri_clear->LPNAdr)
	            && (p_fri_clear->LPNCounter - fn_req[i].LPNCounter <= 255)){
	            	LOG_MSG_INFO(TL_LOG_FRIEND,(u8 *)p_fri_clear, sizeof(mesh_ctl_fri_clear_t),"rcv friend clear:",0);
	            	if(fn_other_par[i].LPNAdr == p_nw->src){
                        mesh_friend_set_delay_par(DELAY_CLEAR_CONF, i, p_nw->src, p_fri_clear->LPNCounter);
                        proc_fn->clear_by_lpn_tick = clock_time()|1;
					}else{
                        friend_cmd_send_clear_conf(p_nw->src, (u8 *)p_fri_clear, sizeof(mesh_ctl_fri_clear_t));
                        friend_ship_disconnect_fn(i, FS_DISCONNECT_TYPE_CLEAR);
	                }
					return ;
	            }
	        }else if(CMD_CTL_CLR_CONF == op){
	        	mesh_ctl_fri_clear_conf_t *p_clear_conf = (mesh_ctl_fri_clear_conf_t *)p_lt_ctl_unseg->data;
	            if(proc_fn->clear_poll && (fn_other_par[i].LPNAdr == p_clear_conf->LPNAdr)){
					LOG_MSG_INFO(TL_LOG_FRIEND,(u8 *)p_clear_conf, sizeof(mesh_ctl_fri_clear_conf_t),"rcv friend clear confirm:",0);
	                mesh_stop_clear_cmd(i);
					return ;
	            }
	        }else if((CMD_CTL_SUBS_LIST_ADD == op) || (CMD_CTL_SUBS_LIST_REMOVE == op)){
				mesh_ctl_fri_subsc_list_t *p_subsc = CONTAINER_OF(p_lt_ctl_unseg->data,mesh_ctl_fri_subsc_list_t,TransNo);
	            mesh_reset_poll_timeout_timer(i);

	            //if(fn_other_par[i].TransNo != p_subsc->TransNo){
	                u32 subsc_cnt = mesh_subsc_adr_cnt_get(p_bear);
	                u16 adr[SUB_LIST_MAX_IN_ONE_MSG];
	                memcpy(adr, p_subsc->adr, sizeof(adr));
	                if(CMD_CTL_SUBS_LIST_ADD == op){
						LOG_MSG_INFO(TL_LOG_FRIEND,(u8 *)p_subsc->adr, subsc_cnt*2,"rcv friend sub list add:",0);
	                    friend_subsc_list_add_adr((lpn_adr_list_t *)(&fn_other_par[i].SubsList), (lpn_adr_list_t *)adr, subsc_cnt);
	                }else{  // (CMD_CTL_SUBS_LIST_REMOVE == op)
	                	LOG_MSG_INFO(TL_LOG_FRIEND,(u8 *)p_subsc->adr, subsc_cnt*2,"rcv friend sub list remove:",0);
	                    friend_subsc_list_rmv_adr((lpn_adr_list_t *)(&fn_other_par[i].SubsList), (lpn_adr_list_t *)adr, subsc_cnt);
	                }
	            //}
	            mesh_friend_set_delay_par(DELAY_SUBSC_LIST, i, p_nw->src, p_subsc->TransNo);
	            fn_other_par[i].TransNo = p_subsc->TransNo;
	        }else{
                LOG_MSG_INFO(TL_LOG_FRIEND,0, 0,"Error:rcv Invalid op code %2x:",op);
	        }
	    }
	}
}


#if IV_UPDATE_SKIP_96HOUR_EN
u8 iv_update_test_mode_en = 1;
u8 iv_idx_update_change2next_st = 0;
#endif

static u8 beacon_iv_update_pkt = 0;

int is_busy_segment_flow()
{
	return ((is_busy_reliable_cmd(0)||mesh_tx_seg_par.busy||mesh_rx_seg_par.tick_last));
}

int is_enough_time_keep_state()
{
	return (is_iv_update_keep_enough_time_ll()
		 #if IV_UPDATE_SKIP_96HOUR_EN
		 || (iv_update_test_mode_en && iv_idx_update_change2next_st)
		 #endif
		 );
}

void mesh_beacon_poll_100ms()
{
#if (IV_UPDATE_DISABLE)
    return ;
#endif

#if (0 == NODE_CAN_SEND_ADV_FLAG)
	return ;	// for test
#endif

    if((NW_BEACON_BROADCASTING != model_sig_cfg_s.sec_nw_beacon) || switch_project_flag){
        return ;
    }

    if(is_lpn_support_and_en){   
        //if(fri_ship_proc_lpn.status || is_in_mesh_friend_st_lpn()){
            return ;
        //}
    }else if(is_fn_support_and_en){
    	foreach(i,g_max_lpn_num){
			mesh_fri_ship_proc_fn_t *proc_fn = &fri_ship_proc_fn[i];
	        if(proc_fn->status && (FRI_ST_TIMEOUT_CHECK != proc_fn->status)){
	            return ;
	        }
        }
    }
    mesh_secure_beacon_loop_proc();
}

void mesh_iv_idx_init(u8 *iv_index, int rst_sno)
{
	int save_flag = 0;
	u8 update_flag_backup = iv_idx_st.update_trigger_by_save;
	memset(iv_idx_st.tx, 0, sizeof(iv_idx_st) - OFFSETOF(mesh_iv_idx_st_t, tx));
    if(iv_index != iv_idx_st.cur){  // not compare value, but pointer.
    	memcpy(iv_idx_st.cur, iv_index, 4);
    	save_flag = 1;
    }
    memcpy(iv_idx_st.tx, iv_idx_st.cur, 4);
    if(rst_sno){
        mesh_adv_tx_cmd_sno = 1;    // default value
        save_flag = 1;
    }else{
        iv_idx_st.update_trigger_by_save = update_flag_backup;
    }

    mesh_rx_seg_par.seqAuth = 0;
    
    #if IV_UPDATE_SKIP_96HOUR_EN
    iv_idx_update_change2next_st = 0;
    #endif

    if(save_flag){
    	mesh_misc_store();
    	#if WIN32
    	if(rst_sno){
    	}
    	#endif
    }
}

void mesh_iv_update_enter_search_mode()
{
    mesh_iv_idx_init(iv_idx_st.cur, 0);
    iv_idx_st.searching_flag = 1;
    LOG_MSG_INFO(TL_LOG_IV_UPDATE,(u8 *)&iv_idx_st, sizeof(iv_idx_st),"IV index searching:",0);
}

#if TESTCASE_FLAG_ENABLE
void mesh_iv_update_enter_normal()	// just for test case
{
	mesh_iv_idx_init(iv_idx_st.cur, 0);
}

void mesh_iv_update_enter_update_progress()
{
	iv_idx_st.rx_update = 1;
	iv_idx_st.searching_flag = 0;
	iv_idx_update_change2next_st = 0;
}
#endif

void mesh_iv_update_enter_update2normal()
{
	mesh_iv_idx_init(iv_idx_st.cur, 1);
	iv_idx_st.update_proc_flag = IV_UPDATE_STEP2;
    LOG_MSG_INFO(TL_LOG_IV_UPDATE,(u8 *)&iv_idx_st, sizeof(iv_idx_st),"IV index enter step2: ",0);
}

void mesh_check_and_set_iv_update_rx_flag(u8 *p_ivi)
{
    if(!iv_idx_st.update_proc_flag){
        if(mesh_ivi_equal(p_ivi, iv_idx_st.cur, 1)){
            iv_idx_st.rx_update = 1;
            beacon_iv_update_pkt = 1;
        }
    }
    // if push for every iv update messages, it would cause FN cache fifo be full with friend update messages.
}

void mesh_iv_update_report_between_gatt()
{
#if WIN32
    // if(connect_flag) // no need
#else
	if(blt_state == BLS_LINK_STATE_CONN)
#endif
	{
		mesh_tx_sec_nw_beacon_all_net(1);
	}
}

int mesh_iv_update_step_change()
{
	int err = -1;
	if(IV_UPDATE_STEP1 == iv_idx_st.update_proc_flag){
		// init parameters
		if(!is_busy_segment_flow()){
			mesh_iv_update_enter_update2normal();
            mesh_iv_update_report_between_gatt();
			err = 0;
		}
	}else if(IV_UPDATE_STEP2 == iv_idx_st.update_proc_flag){
		if(!is_busy_segment_flow()){
			iv_idx_st.update_proc_flag = 0;
			err = 0;
            LOG_MSG_INFO(TL_LOG_IV_UPDATE,(u8 *)&iv_idx_st, sizeof(iv_idx_st),"IV index enter Normal",0);
		}
	}
	
	#if IV_UPDATE_SKIP_96HOUR_EN
	if(0 == err){
		iv_idx_update_change2next_st = 0;
	}
	#endif

	return err;
}


void get_iv_update_key_refresh_flag(mesh_ctl_fri_update_flag_t *p_flag_out, u8 *iv_idx_out, u8 key_phase)
{
    p_flag_out->IVUpdate = (IV_UPDATE_STEP1 == iv_idx_st.update_proc_flag);
    p_flag_out->KeyRefresh = (KEY_REFRESH_PHASE2 == key_phase);
    memcpy(iv_idx_out, iv_idx_st.cur, sizeof(iv_idx_st.cur));
}

void iv_update_set_with_update_flag_ture(u8 *iv_idx, u32 search_mode)
{
    u8 iv_save[4];
    memcpy(iv_save, iv_idx, 4);
    mesh_decrease_ivi(iv_save);         // -1
    int rst_sno = memcmp(iv_save, iv_idx_st.cur, 4);
    mesh_iv_idx_init(iv_save, rst_sno);
    mesh_increase_ivi(iv_idx_st.cur);   // +1
    mesh_iv_update_set_start_flag(0);
    if(search_mode){
        LOG_MSG_INFO(TL_LOG_IV_UPDATE,(u8 *)&iv_idx_st, sizeof(iv_idx_st),"IV index enter step1 by search: ",0);
    }else{
        LOG_MSG_INFO(TL_LOG_IV_UPDATE,(u8 *)&iv_idx_st, sizeof(iv_idx_st),"IV index enter step1 by provision: ",0);
    }
}

int iv_update_key_refresh_rx_handle(mesh_ctl_fri_update_flag_t *p_flag, u8 *iv_idx)
{
    if(mesh_ivi_greater_or_equal(iv_idx, iv_idx_st.cur, 42+1)){
        return -1;
    }
    
    if(!mesh_ivi_greater_or_equal(iv_idx, iv_idx_st.cur, 0)){		// less
        return -1;
    }

    if(iv_idx_st.searching_flag){
        /*only (>=0 && <=1) is valid for APP, because filter set command is used before receive beacon.*/
        if(mesh_ivi_greater_or_equal(iv_idx, iv_idx_st.cur, 1)){
            if(p_flag->IVUpdate){
                iv_update_set_with_update_flag_ture(iv_idx, 1);
            }else{
                mesh_iv_idx_init(iv_idx, 1);
            }
        }else if(mesh_ivi_greater_or_equal(iv_idx, iv_idx_st.cur, 0)){
            if(IV_UPDATE_IDLE == iv_idx_st.update_proc_flag){
                // skip
            }else if(IV_UPDATE_STEP1 == iv_idx_st.update_proc_flag){
                if(p_flag->IVUpdate){
                    // skip
                }else{
                    mesh_iv_update_enter_update2normal();
                }
            }else if(IV_UPDATE_STEP2 == iv_idx_st.update_proc_flag){
                if(p_flag->IVUpdate){
                    // skip
                }else{
                    // keep 96 hour
                }
            }
        }

        iv_idx_st.searching_flag = 0;

        #if FEATURE_LOWPOWER_EN
        if(is_lpn_support_and_en){
            #ifndef WIN32
			if(BLS_LINK_STATE_CONN != blt_state)
			#endif
			{
            	suspend_enter(200, 0);		// delay for FN send beacon ok.
                mesh_friend_ship_set_st_lpn(FRI_ST_REQUEST);
            }
        }
        #endif
    }else{
    	if(IV_UPDATE_STEP1 != iv_idx_st.update_proc_flag){	// normal mode
    		if(mesh_ivi_greater_or_equal(iv_idx, iv_idx_st.cur, 1+1)){
    			mesh_iv_update_enter_search_mode();
    			return 0;
    		}
    	}
        
        /************************ key refresh handle ****************/
        mesh_key_refresh_rx_fri_update(p_flag->KeyRefresh);

		/************************ iv update handle ****************/
    	// iv index delta = + 1 or +0;
        if(p_flag->IVUpdate){
            mesh_net_key_t * p_nk = is_mesh_net_key_exist(NET_KEY_PRIMARY);
            if(p_nk){
                if(NET_KEY_PRIMARY != mesh_key.net_key[mesh_key.netkey_sel_dec][0].index){
                    LOG_MSG_INFO(TL_LOG_IV_UPDATE,0, 0,"ignore subnet beacon which iv update flag ==1",0);
                    return -1;  // ignore (iv update flag ==1) from subnet, IVU_BI04
                }
            }
            mesh_check_and_set_iv_update_rx_flag(iv_idx);
        }else{
			if(iv_idx_st.update_proc_flag){
				if(mesh_ivi_equal(iv_idx, iv_idx_st.cur, 0)){
					// enter normal mode at once ?
					if(IV_UPDATE_STEP1 == iv_idx_st.update_proc_flag){
					    #if 0 // for which case ??? comfirm later
                        int searching_trigger_update = (0 == memcmp(iv_idx_st.tx, iv_idx_st.cur, 4)); // should never ture
                        if(searching_trigger_update){   // why ?? 
                            LOG_MSG_INFO(TL_LOG_IV_UPDATE,0, 0,"into step2, and reset sno ",0);
    						mesh_iv_update_enter_update2normal();   // attention: will reset sno
						}
						#elif (IV_UPDATE_SKIP_96HOUR_EN)
						iv_idx_update_change2next_st = 1;   // skip 96 hour
						#else
						if(is_iv_update_keep_enough_time_rx()){
						    mesh_iv_update_step_change();   // if it's segment busy now, discard it. confirm later.
						}
						#endif
					}else if(IV_UPDATE_STEP2 == iv_idx_st.update_proc_flag){
						#if (IV_UPDATE_SKIP_96HOUR_EN)
						iv_idx_update_change2next_st = 1;   // skip 96 hour
						#endif
					}
				}else{	// delta == 1
					// discard
				}
			}else{
				if(mesh_ivi_equal(iv_idx, iv_idx_st.cur, 1)){
					// accept at once ? or discard ? or enter search mode ?
					mesh_receive_ivi_plus_one_in_normal_cb();
					// discard, refer to TestCase spec 4.10.8
				}else{	// delta == 0
					// discard
				}
			}
        }
    }

    return 0;
}

int mesh_rc_data_beacon_sec (u8 *p_payload, u32 t)
{
	int err = 0;
    mesh_cmd_bear_unseg_t *bc_bear = GET_BEAR_FROM_ADV_PAYLOAD(p_payload);
    mesh_beacon_t *p_bc = &bc_bear->beacon;
    if(is_lpn_support_and_en && is_in_mesh_friend_st_lpn()){
        return 0;    // LPN should not receive beacon msg when in friend state.
    }
    
    mesh_beacon_sec_nw_t *p_sec_nw = (mesh_beacon_sec_nw_t *)(p_bc->data);
    err = mesh_sec_beacon_dec((u8 *)&p_sec_nw->flag);
    if(err){return 100;}
	#if (TESTCASE_FLAG_ENABLE && (HCI_ACCESS == HCI_USE_USB))
	SET_TC_FIFO(TSCRIPT_MESH_RX, (u8 *)&bc_bear->len, bc_bear->len+1);
	#if WIN32
    LOG_MSG_INFO(TL_LOG_IV_UPDATE,&bc_bear->len, bc_bear->len+1,"RX beacon,nk arr idx:%d, new:%d, pkt:\r\n",mesh_key.netkey_sel_dec,mesh_key.new_netkey_dec);
    #endif
	#else
    //LOG_MSG_INFO(TL_LOG_IV_UPDATE,&bc_bear->len, bc_bear->len+1,"yyy RX beacon,nk arr idx:%d, new:%d, ",mesh_key.netkey_sel_dec,mesh_key.new_netkey_dec);
    #endif
    return iv_update_key_refresh_rx_handle(&p_sec_nw->flag, p_sec_nw->iv_idx);
}

void mesh_iv_update_set_start_flag(int keep_search_flag)
{
	iv_idx_st.update_proc_flag = IV_UPDATE_STEP1;
	iv_idx_st.keep_time_s = 0;
	if(!keep_search_flag){
	    iv_idx_st.searching_flag = 0;
	}

    if(!is_sno_exhausted()){
	    mesh_misc_store();	// save iv_update_trigger_flag by other nodes.
	}
	iv_idx_st.update_trigger_by_save = 0;   // only trigger once after power up.
	mesh_iv_update_report_between_gatt();
}

void mesh_iv_update_start_check()
{
#if IV_UPDATE_DISABLE
    return ;
#endif

    if((IV_UPDATE_STEP1 == iv_idx_st.update_proc_flag)
    || ((IV_UPDATE_STEP2 == iv_idx_st.update_proc_flag)&&(!is_enough_time_keep_state()))){
        iv_idx_st.rx_update = 0;   // ignored
    }else{
        if(!is_busy_segment_flow()){	// no need, wait to confirm
            mesh_increase_ivi(iv_idx_st.cur);
            mesh_iv_update_set_start_flag(1);   // should be keep search flag when trigger by self after power up.
            LOG_MSG_INFO(TL_LOG_IV_UPDATE,(u8 *)&iv_idx_st, sizeof(iv_idx_st),"IV index enter step1, sno:0x%06x, iv:",mesh_adv_tx_cmd_sno);
			// can't save for step 1, because if power restart will make iv index error.
        }else{
            // defer
        }
    }
}

void mesh_iv_update_start_poll()
{
    static u8 iv_update_by_sno = 0;
    int flag_sno_exhausted = is_sno_exhausted() || iv_idx_st.update_trigger_by_save;
    if(flag_sno_exhausted && (!iv_idx_st.update_proc_flag)){
        iv_update_by_sno = 1;
    }
    
    if(flag_sno_exhausted || iv_idx_st.rx_update){
        mesh_iv_update_start_check();
    }
    
    if(IV_UPDATE_STEP1 == iv_idx_st.update_proc_flag){
    	foreach(i, g_max_lpn_num){
	        if(is_in_mesh_friend_st_fn(i)){
	            if(iv_update_by_sno || beacon_iv_update_pkt){
	                friend_cmd_send_update(i, 1);//friend_cmd_send_fn(CMD_CTL_UPDATE);
	            }
	        }
        }
    }
    
	if(iv_idx_st.update_proc_flag){
        if(iv_update_by_sno){
            iv_update_by_sno = 0;
        }
    }
    beacon_iv_update_pkt = 0;    // reset
}

void mesh_iv_update_st_poll_s()
{
    iv_idx_st.keep_time_s++;
    
	if(iv_idx_st.update_proc_flag && is_enough_time_keep_state()){ // only for trigger role
	    mesh_iv_update_step_change();
	}
}

//---------- key refresh flow
u8 key_refresh_phase_get(u8 nk_array_idx)
{
	return mesh_key.net_key[nk_array_idx][0].key_phase;
}

void key_refresh_phase_set(u8 phase, mesh_net_key_t *p_net_key)
{
	if(phase > KEY_REFRESH_STATE_MAX){
		return ;
	}

	u8 nk_array_idx = GetNKArrayIdxByPointer(p_net_key);
    mesh_key.net_key[nk_array_idx][0].key_phase = phase;
}

int is_key_refresh_use_new_key(u8 nk_array_idx)
{
	return (KEY_REFRESH_PHASE2 == key_refresh_phase_get(nk_array_idx));
}

int is_key_refresh_use_old_and_new_key(u8 nk_array_idx)
{
	u8 phase = key_refresh_phase_get(nk_array_idx);
	return ((KEY_REFRESH_PHASE1 == phase)||(KEY_REFRESH_PHASE2 == phase));
}

void key_refresh_complete(mesh_net_key_t *p_net_key)
{
	mesh_net_key_t *p_net_key_new = p_net_key+1;
	memcpy(p_net_key, p_net_key_new, sizeof(mesh_net_key_t));
	memset(p_net_key_new, 0, sizeof(mesh_net_key_t));

	mesh_friend_key_RevokingOld(p_net_key_new);
	cache_init(ADR_ALL_NODES);
	
	mesh_key_save();
	
	key_refresh_phase_set(KEY_REFRESH_NORMAL, p_net_key);
}

int key_refresh_phase_set_by_index(u16 nk_idx, u8 phase_set)
{
	mesh_net_key_t *p_netkey_exist = is_mesh_net_key_exist(nk_idx);
	if(p_netkey_exist){
		return mesh_key_refresh_phase_handle(phase_set, p_netkey_exist);
	}
	return -1;
}

int mesh_key_refresh_phase_handle(u8 phase_set, mesh_net_key_t *p_net_key)
{
	int st = ST_SUCCESS;

	switch(p_net_key->key_phase){
		case KEY_REFRESH_NORMAL:
			if(KEY_REFRESH_PHASE1 == phase_set){
				key_refresh_phase_set(phase_set, p_net_key);
			}else if(KEY_REFRESH_PHASE2 == phase_set){
				st = -1; // ST_UNSPEC_ERR;
			}
			break;
		case KEY_REFRESH_PHASE1:
			if(KEY_REFRESH_PHASE2 == phase_set){
				key_refresh_phase_set(phase_set, p_net_key);
			}else if(KEY_REFRESH_PHASE3 == phase_set){
				key_refresh_complete(p_net_key);
			}
			break;
		case KEY_REFRESH_PHASE2:
			if(KEY_REFRESH_PHASE3 == phase_set){
				key_refresh_complete(p_net_key);
			}
			break;
		default :
			break;
	}

	return st;
}

void mesh_key_refresh_rx_fri_update(u8 KeyRefresh)
{
    if(mesh_key.new_netkey_dec){
        u8 phase_set = KeyRefresh ? KEY_REFRESH_PHASE2 : KEY_REFRESH_PHASE3;
        mesh_key_refresh_phase_handle(phase_set, &mesh_key.net_key[mesh_key.netkey_sel_dec][0]);
    }else{
        // all skip
    }
}

void mesh_nk_update_self_and_change2phase2(const u8 *nk, u16 key_idx)
{
	mesh_net_key_t * p_netkey = is_mesh_net_key_exist(key_idx);
	if(p_netkey){
		mesh_net_key_set(NETKEY_UPDATE, nk, key_idx, 0);
		key_refresh_phase_set(KEY_REFRESH_PHASE2, p_netkey);
	}
}

void mesh_kr_cfgcl_check_set_next_flag()    // compile need when client model enable in light project
{
#if 0	// auto by Tscript
	if(key_refresh_cfgcl_proc.st){
		if(key_refresh_cfgcl_proc.st < KR_CFGCL_SET_PHASE2){
			key_refresh_cfgcl_proc.next_st_flag = 1;
		}
	}
#endif
}

//---------- cache status response
#if RELIABLE_CMD_EN
#if GATEWAY_ENABLE
#define MESH_RSP_BUF_CNT        8   // only gateway use hci tx fifo
#else
#if WIN32
#define MESH_RSP_BUF_CNT        8   // should be >= (ELE CNT + 1),    // 2   // hci tx fifo is not used, because it's process immediately in main loop
#else
#define MESH_RSP_BUF_CNT        2   // should be >= (ELE CNT + 1),    // 2   // hci tx fifo is not used, because it's process immediately in main loop
#endif
#endif
#define MESH_RSP_LEN_MAX        (sizeof(mesh_rc_rsp_t) - OFFSETOF(mesh_rc_rsp_t,src))

STATIC_ASSERT(sizeof(mesh_rc_rsp_t) % 2 == 0);

mesh_rc_rsp_t slave_rsp_buf[MESH_RSP_BUF_CNT];
status_record_t slave_status_record[MESH_ELE_MAX_NUM];
u16 slave_status_record_size = sizeof(slave_status_record);
u16 slave_status_record_idx;

u8		slave_status_buffer_num = MESH_RSP_BUF_CNT;
u8 		slave_status_buffer_rptr = 0;
u8 		slave_status_buffer_wptr = 0;

void    rf_link_slave_read_status_par_init()
{
	slave_status_buffer_rptr = slave_status_buffer_wptr = 0;
}

void	rf_link_slave_read_status_start ()
{
    rf_link_slave_read_status_par_init();
	#if WIN32 
	memset(slave_rsp_buf, 0, (int)slave_status_buffer_num*(sizeof(mesh_rc_rsp_t)));
	#else
	memset4(slave_rsp_buf, 0, (int)slave_status_buffer_num*(sizeof(mesh_rc_rsp_t)));
	#endif
	slave_status_record_idx = 0;
	memset(slave_status_record, 0, slave_status_record_size);
}

int		is_slave_read_status_buf_empty ()
{
	return (slave_status_buffer_rptr == slave_status_buffer_wptr);
}

void	rf_link_slave_read_status_stop ()
{
    rf_link_slave_read_status_par_init();
}

int mesh_rsp_handle(mesh_rc_rsp_t *p_rsp)
{
	static u32 fifo_cnt=0;
	fifo_cnt++;
	LOG_MSG_INFO(TL_LOG_MESH,0,0,"mesh_rsp_handle:add to receive fifo  %d", fifo_cnt);
	int err = 0;
    #if IS_VC_PROJECT
	u8 pu_buf[1024];
	pu_buf[0] = (MESH_CMD_RSP|TSCRIPT_MESH_RX);
	if(p_rsp->len + 3 <= sizeof(pu_buf)){
    	memcpy(pu_buf+1, p_rsp, p_rsp->len + 2);
    	OnAppendLog_vs(pu_buf, p_rsp->len + 3);
	}
	#else
	//err = my_fifo_push_hci_tx_fifo((u8 *)(p_rsp), len+2, 0, 0); // no handle for slave node, now.
	#endif

#if ((IS_VC_PROJECT_MASTER || DONGLE_PROVISION_EN) && (0 == DEBUG_MESH_DONGLE_IN_VC_EN))
	mesh_kr_cfgcl_status_update(p_rsp);
#endif
#if WIN32
    mesh_rsp_handle_proc_win32(p_rsp);
#else
	mesh_rsp_handle_cb(p_rsp);
#endif
	mesh_fast_prov_rsp_handle(p_rsp);

    return err; // always return 0 for VC.
}

void	rf_link_slave_read_status_update ()
{
#if GATEWAY_ENABLE
    if(my_fifo_data_cnt_get(&hci_tx_fifo) > 0)
    {
        return;
    }
#endif
    
	while (slave_status_buffer_wptr != slave_status_buffer_rptr)
	{
	    mesh_rc_rsp_t *p_rsp = &slave_rsp_buf[(slave_status_buffer_rptr % slave_status_buffer_num)];
		if (0 == mesh_tx_reliable_rc_rsp_handle(p_rsp)){
            slave_status_buffer_rptr = (slave_status_buffer_rptr+1) % slave_status_buffer_num;
            if(++mesh_tx_reliable.rsp_cnt >= mesh_tx_reliable.mat.rsp_max){
            	if(is_slave_read_status_buf_empty()){	// report the remain node also.
	                mesh_tx_reliable_finish();
	                return;
                }
            }
		}else{
			return;
		}
	}
}

int	rf_link_slave_add_status (mesh_rc_rsp_t *p_rsp, u16 op)
{
    int err = -1;
    #if(IS_VC_PROJECT_MASTER && (!DEBUG_SHOW_VC_SELF_EN))
    if(is_cmd_skip_for_vc_self(p_rsp->src, op) && (!is_unicast_adr(mesh_tx_reliable.mat.adr_dst))){
    	return -1;
    }
    #endif

	foreach(i,slave_status_record_idx){
		if((p_rsp->src == slave_status_record[i].src)
		&& (op == slave_status_record[i].op)){
			return 0;
		}
	}

	if(slave_status_record_idx < MESH_ELE_MAX_NUM){
        if(p_rsp->len <= MESH_RSP_LEN_MAX){
            if(((slave_status_buffer_wptr+1) % slave_status_buffer_num) != slave_status_buffer_rptr){
                // record notify data to p_slave_status_buffer
                mesh_rc_rsp_t *p_dst = &slave_rsp_buf[slave_status_buffer_wptr % slave_status_buffer_num];
                slave_status_buffer_wptr = (slave_status_buffer_wptr+1) % slave_status_buffer_num;
                memcpy(p_dst, p_rsp, p_rsp->len + OFFSETOF(mesh_rc_rsp_t, src));
                //memset(p_dst + OFFSETOF(mesh_rc_rsp_t, src) + p_rsp->len, 0, MESH_RSP_LEN_MAX - p_rsp->len);   // for test
                
                // record address to slave_status_record
                slave_status_record[slave_status_record_idx].op = op;
                slave_status_record[slave_status_record_idx++].src = p_rsp->src;
                
                err = 0;
            }
        }
    }
    
	return err;
}
#endif

void init_ecc_key_pair()
{
#if WIN32
	u8 mac[8]={0x13,0x24,0x35,0x46,0x57,0x68,0x00,0x01};
	set_pro_psk_ppk(mac,sizeof(mac) );
#else
	#if (__PROJECT_MESH_PRO__ || __PROJECT_MESH_GW_NODE__)
	get_pro_pubkey_xy();
	#endif

	#if(!__PROJECT_MESH_PRO__)
		#if !TESTCASE_FLAG_ENABLE
		get_dsk_dpk_para();
		#endif 
	#endif 
#endif 
}


#if FEATURE_LOWPOWER_EN
#define PUB_ADR_TEST    0x0000
#else
#define PUB_ADR_TEST    0x61E1 // ADR_FND2
#endif

u8  cal_node_identity_by_proxy_sts(u8 proxy_sts)
{
	if(proxy_sts == GATT_PROXY_SUPPORT_DISABLE){
		return NODE_IDENTITY_SUBNET_SUPPORT_DISABLE;
	}else if (proxy_sts == GATT_PROXY_NOT_SUPPORT){
		return NODE_IDENTITY_SUBNET_NOT_SUPPORT;	
	}else if (proxy_sts == GATT_PROXY_SUPPORT_ENABLE){
		return NODE_IDENTITY_SUBNET_SUPPORT_ENABLE;	
	}else{
		return NODE_IDENTITY_SUBNET_ST_MAX;
	}
}

#if ONLINE_ST_LIB_EN
////////////////////////////////////////////////////////////////////////////////////////
//		online status management
////////////////////////////////////////////////////////////////////////////////////////
extern mesh_node_st_t	mesh_node_st[];
extern u32	mesh_node_mask[];

u16	mesh_node_max = 0;
u16	mesh_node_cur = 1;
u8	device_node_sn = 1;
u8	mesh_node_report_enable = 0;
//u8 mesh_send_online_status_flag = 1;

//u8 sync_time_enable = 0;

void ll_device_status_update (u8 *st_val_par, u8 len)			//call this function whenever device need update status
{
    memcpy(mesh_node_st[0].val.par, st_val_par, len);
	//if(sync_time_enable){
        //sync_set_flag(synced_flag);
    //}
	mesh_node_mask[0] |=  BIT(0);
	mesh_node_st[0].tick =  (clock_time () >> 16) | 1;			//make sure tick not equal 0
}

typedef void (*cb_mesh_node_status_t) (u8 *p, u8 new_node);

cb_mesh_node_status_t p_mesh_node_status_callback = 0;

void	register_mesh_node_status_callback (void *p)
{
	p_mesh_node_status_callback = (cb_mesh_node_status_t) p;
}

void mesh_node_online_st_init ()
{
	mesh_node_buf_init();
	mesh_node_st[0].val.dev_adr = (ele_adr_primary);
	mesh_node_st[0].val.sn = device_node_sn;
	mesh_node_max = 1;
}

void mesh_node_keep_alive ()
{
	device_node_sn++;
	if (device_node_sn == 0)
	{
		device_node_sn = 1;
	}

	mesh_node_st[0].val.sn = device_node_sn;
	mesh_node_st[0].tick =  (clock_time () >> 16) | 1;			//make sure tick not equal 0
}

void mesh_node_update_status (u8 *p, int ns)
{
	u16 t = (clock_time () >> 16) | 1;			//make sure tick not equal 0
	int i, j;
	for (i=0; i < ns * mesh_node_st_val_len; i += mesh_node_st_val_len)
	{
	    mesh_node_st_val_t *st_val = (mesh_node_st_val_t *)(p+i);
	    mesh_node_st_t *st_buf = (mesh_node_st_t *)(((u8 *)mesh_node_st) + mesh_node_st_len);
		u16 adr = st_val->dev_adr;
		if (adr == 0)
		{
			return;
		}
		if (adr == ele_adr_primary)				//no updating for current device
		{
			continue;
		}
		
		static u16 pno_online_st; pno_online_st++;	// for test
		for (j=1; j<mesh_node_max; j++)
		{
		    st_buf = (mesh_node_st_t *)(((u8 *)mesh_node_st) + j * mesh_node_st_len);
			if (adr == st_buf->val.dev_adr)
			{
				break;
			}
		}

		st_buf = (mesh_node_st_t *)(((u8 *)mesh_node_st) + j * mesh_node_st_len);
		int online_again = (0 == st_buf->tick);
        
		if (j == online_st_node_max_num)
		{
			//mesh_node_online_st_init ();				//device node hit max, re-initialize
			return;
		}
		else if (j == mesh_node_max)		//new device found
		{
			mesh_node_max++;
			memcpy (&(st_buf->val), st_val, mesh_node_st_val_len);
			st_buf->tick = t;
			mesh_node_mask[j>>5] |= BIT(j & 31);			// status changed
            if (p_mesh_node_status_callback)
            {
                p_mesh_node_status_callback ((u8 *)(&(st_buf->val)), 1);
            }
		}
		else if (j < mesh_node_max)							// update
		{
			u8 sn = st_val->sn - st_buf->val.sn;
			u16 d = memcmp(st_val->par, st_buf->val.par, mesh_node_st_par_len);
			extern u8 send_self_online_status_cycle;
			u32 timeout_reboot_check = 1500*1000;			// us
			if(send_self_online_status_cycle){
				timeout_reboot_check = online_status_timeout * 1000 / 2;
			}
			
			if ((sn  && sn < 64) ||
				(sn && (!st_buf->tick || (u16)(t - st_buf->tick) > ((timeout_reboot_check * sys_tick_per_us) >> 16))) )
			{
				memcpy (&(st_buf->val), st_val, mesh_node_st_val_len);
				if (d || !(st_buf->tick))					//online or status changed
				{
					mesh_node_mask[j>>5] |= BIT(j & 31);		// status changed
    				if (p_mesh_node_status_callback)
    				{
    					p_mesh_node_status_callback ((u8 *)(&(st_buf->val)), 0);
    				}
				}
				st_buf->tick = t;
			}
		}
		
        if(0 == i){ // packet source
            online_again = online_again;
            //rssi_online_status_pkt_cb(st_buf, light_rcv_rssi, online_again);
        }
	}
}

u32 online_status_timeout = ONLINE_STATUS_TIMEOUT;
void mesh_node_flush_status ()
{
	static u32		tick_node_report;
	if ((u32)(clock_time() - tick_node_report - 500000 *sys_tick_per_us) > BIT(30))
	{
		return;
	}
	tick_node_report = clock_time ();

	u16 t = (clock_time () >> 16) | 1;

	for (int i=1; i<mesh_node_max; i++)
	{
		//2 seconds no update, drop node
        mesh_node_st_t *st_buf = (mesh_node_st_t *)(((u8 *)mesh_node_st) + i * mesh_node_st_len);
		if (st_buf->tick && (u16)(t - st_buf->tick) > ((online_status_timeout * 1000 * sys_tick_per_us) >> 16))
		{
			st_buf->tick = 0;			//device offline
			mesh_node_mask[i>>5] |= BIT(i & 31);			//update status
			if (p_mesh_node_status_callback)
			{
			    static u8 node_data[10];    // use max len
			    memcpy(node_data, &(st_buf->val), mesh_node_st_val_len);
			    node_data[1] = 0;       // zero sn
				p_mesh_node_status_callback (node_data, 0);
			}
		}
	}
}

int mesh_node_report_status (u8 *p, int ns)
{
	if (!mesh_node_report_enable)
	{
		return 0;
	}
	int n = 0;
	memset (p, 0, ns * mesh_node_st_val_len);
	for (int j=0; j<((online_st_node_max_num+31)>>5); j++)
	{
		if (mesh_node_mask[j])
		{
			for (int k=0; k<32; k++)
			{
				int idx = (j<<5) + k;
				if(idx >= online_st_node_max_num){
				    break;
				}

				if (mesh_node_mask[j] & BIT(k))		//online device only
				{
					mesh_node_mask[j] &= ~ BIT(k);		//status send

                    mesh_node_st_val_t *st_val = (mesh_node_st_val_t *)(p + n * mesh_node_st_val_len);
                    mesh_node_st_t *st_buf = (mesh_node_st_t *)(((u8 *)mesh_node_st) + idx * mesh_node_st_len);
					memcpy (st_val, &(st_buf->val), mesh_node_st_val_len);
					//if(sync_time_enable){
					    //st_val->par[0] &= ~FLD_SYNCED;
					//}
					if (!st_buf->tick)
					{
						st_val->sn = 0;	//off-line indication
					}

					n ++;
					if (n == ns)
					{
						return n;
					}
				}
			}
		}
	}
	return n;
}

u8 send_self_online_status_cycle = 0;	// 6

int mesh_node_adv_status (u8 *p, int ns)
{
	memset (p, 0, ns * mesh_node_st_val_len);
	if (ns > mesh_node_max)
	{
		ns = mesh_node_max;
	}
	int nr = 0;
	static u8 send_online_status_cnt = 200;	// send self at once when start system
	send_online_status_cnt++;

	if(send_online_status_cnt >= send_self_online_status_cycle || mesh_node_max < 20)
	{
		send_online_status_cnt = 0;
		memcpy (p, &(mesh_node_st[0].val), mesh_node_st_val_len);
		mesh_node_keep_alive ();
		nr = 1;
	}

	int i=nr;
	for (; i<mesh_node_max; i++)
	{
	    mesh_node_st_t *st_buf = (mesh_node_st_t *)(((u8 *)mesh_node_st) + mesh_node_cur * mesh_node_st_len);
		if (mesh_node_cur < mesh_node_max && st_buf->tick)
		{
			memcpy (p + nr++ * mesh_node_st_val_len, &(st_buf->val), mesh_node_st_val_len);
		}
		if (++mesh_node_cur >= mesh_node_max)
		{
			mesh_node_cur = 1;
		}
		if (nr == ns)
		{
			break;
		}

	}
	return nr;
}

void mesh_report_status_enable(u8 mask)
{
	if (mask)
	{
		int i;
		for (i=0; i<mesh_node_max>>5; i++)
		{
			mesh_node_mask[i] = U32_MAX;
		}
		if (mesh_node_max & 31)
		{
			mesh_node_mask[i] = (1 << (mesh_node_max & 31)) - 1;
		}
	}

	mesh_node_report_enable = mask;
}

void mesh_report_status_enable_mask(u8 *val, u16 len)
{
    u8 en = val[0];
	if (en)
	{
		int i = 0, j = 0;
		u32 size_adr = OFFSETOF(mesh_node_st_val_t, sn);
		int ns = (len - 1)/size_adr;
		u8 *p_adr = val+1;
        for (j=0; j<ns; j++){
            u16 adr_report = p_adr[0] + ((2 == size_adr) ? (p_adr[1]*256) : 0);
    		for (i=0; i<mesh_node_max; i++){
        	    mesh_node_st_t *st_buf = (mesh_node_st_t *)(((u8 *)mesh_node_st) + mesh_node_st_len * i);
        		if(adr_report == st_buf->val.dev_adr){
                    mesh_node_mask[i >> 5] |= BIT(i & 31);
                    break;
        		}
    		}
            p_adr += ((2 == size_adr) ? 2 : 1);
		}
	}

	mesh_node_report_enable = en;
}

void device_status_report(u8 mask_idx)
{
    mesh_node_mask[mask_idx >> 5] |= BIT(mask_idx & 31);
}

int mesh_node_check_force_notify(u16 dst, u8 par0){
    if(mesh_node_report_enable){
    	for (int j=0; j<mesh_node_max; j++)
    	{
    	    mesh_node_st_t *st_buf = (mesh_node_st_t *)(((u8 *)mesh_node_st) + j * mesh_node_st_len);
    		if (dst == st_buf->val.dev_adr)
    		{
    		    if(st_buf->tick){ // online
        		    if(((G_ON == par0) && (st_buf->val.par[0]/* & (~FLD_SYNCED)*/))
        		     ||((G_OFF == par0) && (!(st_buf->val.par[0]/* & (~FLD_SYNCED)*/)))){
                        device_status_report(j);
        			}
    			}else{              // offline
                    device_status_report(j);
    			}
    		}
    	}
	}
	return 0;
}

void mesh_node_report_init()
{
	mesh_node_report_enable = 0;
	for (int i=0; i<((online_st_node_max_num+31)>>5); i++)
	{
		mesh_node_mask[i] = 0;
	}
}

//STATIC_ASSERT(ADV_PAYLOAD_LEN_MAX <= (sizeof(mesh_cmd_bear_unseg_t) - OFFSETOF(mesh_cmd_bear_unseg_t,len)));

void mesh_send_online_status ()
{
	//if(!mesh_send_online_status_flag){
		//return ;
	//}

	static u32	adv_st_sn = 0;

	//send mesh command
	mesh_node_flush_status ();
	mesh_cmd_bear_unseg_t bear = {0, sizeof(mesh_cmd_bear_unseg_t)-OFFSETOF(mesh_cmd_bear_unseg_t,type)};
	bear.type = get_online_st_adv_type();
	mesh_node_adv_status (bear.online_st_adv.data, (sizeof(bear.online_st_adv.data) - ONLINE_ST_MIC_LEN) / mesh_node_st_val_len);
	adv_st_sn++;
	if (SECURITY_ENABLE){
		online_st_adv_enc(&bear.len);
	}
	mesh_tx_cmd_add_packet((u8 *)&bear);//mesh_send_command (A_buff);
}

void online_st_force_notify_check2(u16 adr_dst, u8 *p_ut)
{
    u16 op = p_ut[0] + (p_ut[1]<<8);
    if(G_ONOFF_SET_NOACK == op){
        mesh_node_check_force_notify(adr_dst, p_ut[2]);
    }
}

void online_st_force_notify_check3(mesh_cmd_bear_unseg_t *p_bear, u8 *ut_dec)
{
    u16 adr_dst = p_bear->nw.dst;
    if(ut_dec){
        if(is_unicast_adr(adr_dst)){
            online_st_force_notify_check2(adr_dst, ut_dec);
        }
    }else{
        if((!p_bear->lt.seg) && (!p_bear->nw.ctl)
         && (((19+4) == p_bear->len)||((19+6) == p_bear->len)) && is_unicast_adr(adr_dst)){
            mesh_cmd_bear_unseg_t br_temp;
            memcpy(&br_temp, p_bear, sizeof(br_temp));
            endianness_swap((u8 *)&br_temp.nw, SWAP_TYPE_LT_UNSEG);
            u8 *ut_dec2 = (u8 *)&br_temp.ut;
            u8 len_ut = mesh_bear_len_get(&br_temp) - OFFSETOF(mesh_cmd_bear_unseg_t, ut) - SZMIC_NW32;
            int err = mesh_sec_msg_dec_apl(ut_dec2, len_ut, (u8 *)&br_temp.nw);
            if(!err){
                online_st_force_notify_check2(adr_dst, ut_dec2);
            }
        }
    }
}

#endif

#if 0   // just for opple API, suggest opple place this code in mesh_node.c, so can't active this code in library.
#if !WIN32
typedef unsigned int bool;

bool opp_assign_element_addr(u16 node_id);
bool opp_revise_default_net_key(u8 *fisrt_four_bytes, bool is_saved_to_nvram);
void opp_get_device_mac_address(u8 bda[6]);
void opp_get_current_net_key(u8 *p_default_netkey);
void opp_get_current_net_key_from_flash(u8 *p_flash_netkey);

/*
* assign the element address for the node
*/
bool opp_assign_element_addr(u16 node_id){
    mesh_set_ele_adr(node_id);
    return 0;
}

/*
* change the default net key to new one
* just changed the fist 4 bytes of the default net key
* must reset the node to reload the netkey Ziv: no need to reset
*/
bool opp_revise_default_net_key(u8 *fisrt_four_bytes, bool is_saved_to_nvram){
    // get key
    u8 net_key_temp[16] = {0};
    opp_get_current_net_key(net_key_temp);
    memcpy(net_key_temp,fisrt_four_bytes,4);

    // provision
    provison_net_info_str prov_par = {{0}};
    memcpy(prov_par.net_work_key, net_key_temp, sizeof(prov_par.net_work_key));
    prov_par.key_index = NET0;
    prov_par.flags = 0;
    memcpy(prov_par.iv_index, iv_idx_st.cur, sizeof(prov_par.iv_index));
    prov_par.unicast_address = ele_adr_primary;

    mesh_provision_par_set((u8 *)&prov_par);

    // app key add
    u8 ak[16] = APPKEY_A; // 
    u16 app_key_idx = 0;
    mesh_app_key_set(APPKEY_ADD, ak, app_key_idx, NET0, 1);

    // key bind all
    appkey_bind_all(1, app_key_idx, 0);

    return 1;
}

/*
* get the mac address of the node.
* attention: the provisoned and unprovisoned mac address are the same.
*/
void opp_get_device_mac_address(u8 bda[6]){
    memcpy(bda,tbl_mac,6);
}

/*
* get the current setting of netkey from nvram
* p_default_netkey: 16 bytes
*/
void opp_get_current_net_key(u8 *p_default_netkey){
	foreach(i,NET_KEY_MAX){
        mesh_net_key_t *p_nk_base = &mesh_key.net_key[i][0];
		if(p_nk_base->valid){   // return the first valid key
            u32 use_new_key_flag = (KEY_REFRESH_PHASE2 == p_nk_base->key_phase);
            memcpy(p_default_netkey, mesh_key.net_key[i][use_new_key_flag].key, 16);
			return;
		}
	}
}

/*
* get the current setting of netkey from flash
* p_default_netkey: 16 bytes
*/
void opp_get_current_net_key_from_flash(u8 *p_flash_netkey){
	mesh_key_save_t key_save;
    if(ST_SUCCESS==mesh_par_retrieve((u8 *)&key_save, &mesh_key_addr, FLASH_ADR_MESH_KEY, sizeof(key_save))){
        #if KEY_SAVE_ENCODE_ENABLE
        if(KEY_SAVE_ENCODE_FLAG == key_save.encode_flag){
            encode_decode_password((u8 *)&key_save, OFFSETOF(mesh_key_save_t,encode_flag), sizeof(key_save.mic_val), 0);
        }
        #endif
        memcpy(p_flash_netkey, key_save.net_key[0].key, 16);
    }else{
        opp_get_current_net_key(p_flash_netkey);    // memset(p_flash_netkey, 0xff, 16);
    }
}

int sub_set_traversal_cps(u16 sub_adr, int add_flag)
{
    u16 op = add_flag ? CFG_MODEL_SUB_ADD : CFG_MODEL_SUB_DEL;
	int pos = 0;
	int offset_ele = OFFSETOF(mesh_page0_t, ele);
    const mesh_element_head_t *p_ele = &gp_page0->ele;
    int total_len_ele = SIZE_OF_PAGE0_LOCAL - offset_ele;
    u16 ele_adr = ele_adr_primary;
	while (pos < total_len_ele)
	{
        foreach(i, p_ele->nums){
            mesh_sub_search_ele_and_set(op, ele_adr_primary, sub_adr, 0, p_ele->md_sig[i], 1);
        }

        foreach(i, p_ele->numv){
        	u32 vd_model_id = get_cps_vd_model_id(p_ele, i);
            mesh_sub_search_ele_and_set(op, ele_adr_primary, sub_adr, 0, vd_model_id, 0);
        }
        
        int len_ele = get_cps_ele_len(p_ele);
        p_ele = (mesh_element_head_t *)((u8 *)p_ele + len_ele);
		pos += len_ele;
		ele_adr += 1;
	}

	return 1;
}

bool opp_add_group_id(uint16_t group_id)
{
    return sub_set_traversal_cps(group_id, 1);
}

bool opp_delete_group_id(uint16_t group_id)
{
    return sub_set_traversal_cps(group_id, 0);
}
#endif
#endif


