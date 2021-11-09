/********************************************************************************************************
 * @file     op_agg_model.h 
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


#pragma once

#include "proj/tl_common.h"
#include "proj_lib/sig_mesh/app_mesh.h"

#define SOLI_WITH_RPL_TYPE		0

#define SOLI_RPL_MAX	8
typedef struct{
	u16 ssrc;
	u32 ssno     :24;
}soli_rpl_t;

typedef struct{
	#if MD_SERVER_EN
	model_g_light_s_t srv;			// server
    #endif
	#if MD_CLIENT_EN
	model_client_common_t clnt;		        // client
	#endif
}model_sig_soli_pdu_rpl_t;

extern model_sig_soli_pdu_rpl_t model_sig_soli_pdu_rpl;
extern u32 mesh_md_soli_pdu_rpl_addr;

int is_exist_in_soli_rpl(u8 *p);
#if (MD_SERVER_EN)
int mesh_cmd_sig_cfg_soli_rpl_clear(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
#else							
#define mesh_cmd_sig_cfg_soli_rpl_clear										(0)
#endif

#if MD_CLIENT_EN
int mesh_cmd_sig_cfg_soli_rpl_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
#else
#define mesh_cmd_sig_cfg_soli_rpl_status									(0)
#endif

