/********************************************************************************************************
 * @file	solicitation_rpl_cfg_model.h
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
#pragma once

#include "proj/tl_common.h"
#include "proj_lib/sig_mesh/app_mesh.h"

#define SOLI_WITH_RPL_TYPE		0

#define SOLI_RPL_MAX	8

#define SOLICI_PDU_INTERVAL_MS 		50
#define SOLICI_PDU_CNT				20

typedef struct{
	u8 id_type;
	u8 id_para[17]; // OFFSETOF(mesh_cmd_nw_t, 8) +MIC(8)
}soli_ser_dat_t;

typedef struct {
	u8 flag_len;
	u8 flag_type;
	u8 flag_content;
	u8 len;
	u8 type;
	u16 uuid;
	struct{
		u8 service_len;
		u8 service_type;
		u16 service_uuid;
		soli_ser_dat_t service_data;
	};
}soli_pdu_pkt_t;

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
extern u32 soli_sno_tx;

extern soli_ser_dat_t soli_service_data;
extern int soli_pdu_adv_cnt;

int is_exist_in_soli_rpl(u8 *p);
int set_adv_solicitation(rf_packet_adv_t * p);

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

