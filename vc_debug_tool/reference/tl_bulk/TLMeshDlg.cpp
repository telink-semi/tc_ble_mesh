/********************************************************************************************************
 * @file	TLMeshDlg.cpp
 *
 * @brief	for TLSR chips
 *
 * @author	Mesh Group
 * @date	2017
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
#include "stdafx.h"
#include "tl_ble_module.h"
#include "ScanDlg.h"
#include "TLMeshDlg.h"
#include "tl_ble_moduleDlg.h"
#include "lib_file/gatt_provision.h"
#include "lib_file/host_fifo.h"

#include <vector>
#include <array>


#include "rapidjson/document.h"  
#include "rapidjson/filereadstream.h"  
#include "rapidjson/filewritestream.h" 
#include "rapidjson/prettywriter.h"  
#include "rapidjson/stringbuffer.h"

using namespace RAPIDJSON_NAMESPACE;

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TEST_SCHEDULER_CMD_EN   0

typedef unsigned char u8;

/////////////////////////////////////////////////////////////////////////////

#define action_default         (0xfe)   // flag of no display UI.


#define MAX_SCENE_CNT           16

typedef struct{
    u8 st;
	u16 current_id;
	u16 id[SCENE_CNT_MAX];
}scene_t;

scene_t scene_data;

#define ALARM_CNT_MAX           16

typedef struct{
    u32 wait_status_tick;
    u16 total_shedule_bit;
	u8 schd_num;
}shedule_get_proc_t;

shedule_get_proc_t shedule_get_proc;

typedef struct{
	u64 idx	        : 4;
	u64 year		: 7;
	u64 month		: 12;
	u64 day			: 5;
	u64 hour		: 5;
	u64 minute		: 6;
	u64 second		: 6;
	u64 week        : 7;    // bit0 means monday,
	u64 action		: 4;
	u64 trans_t	    : 8;    // transition time
	u16 scene_id;
}alarm_ev_t;

typedef struct{
	u16 idx;
}alarm_ev_idx;
alarm_ev_t alarm_list[ALARM_CNT_MAX];

void CTLMeshDlg::get_time_shedule_proc(){
    if(AUTO_GET_ST_GROUP == m_pMeshDlg->shedule_init){
        LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "auto get subscription list to refresh UI......",0);
        cfg_cmd_sub_get(NODE_ADR_AUTO, mesh_sel, SIG_MD_G_ONOFF_S);
        shedule_init = AUTO_GET_ST_WAIT_GROUP;
    }else if(AUTO_GET_ST_SCENE == m_pMeshDlg->shedule_init){
        if(is_support_model_dst(mesh_sel,SIG_MD_SCENE_S,1)){
            LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "auto get scene to refresh UI......",0);
            access_cmd_scene_reg_get(mesh_sel,1);
            shedule_init = AUTO_GET_ST_WAIT_SCENE;
    	}else{
            shedule_init = AUTO_GET_ST_TIME;
            // LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "scene model not found, no need auto get scene when double click node......",0);
        }
    }else if(AUTO_GET_ST_TIME == m_pMeshDlg->shedule_init){
        if(is_support_model_dst(mesh_sel,SIG_MD_TIME_S,1)){
            LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "auto get time to refresh UI......",0);
            access_cmd_time_get(mesh_sel,1);
            shedule_init = AUTO_GET_ST_WAIT_TIME;
        }else{
            shedule_init = AUTO_GET_ST_SCHEDULER;
        }
    }else if(AUTO_GET_ST_SCHEDULER == m_pMeshDlg->shedule_init){
        if(is_support_model_dst(mesh_sel,SIG_MD_SCHED_S,1)){
            LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "auto get scheduler to refresh UI......",0);
            access_cmd_schd_get(mesh_sel, 1);
        }
        shedule_init=0; // end
    }
}

void schedule_get_proc_tick_check_and_clear()
{
    if(shedule_get_proc.wait_status_tick){
        shedule_get_proc.wait_status_tick = 0;
    }
}

void schedule_get_proc()
{
    u16 mesh_sel = (u16)m_pMeshDlg->mesh_sel;
    shedule_get_proc_t *p_proc = &shedule_get_proc;
    while(p_proc->total_shedule_bit){
        //if(ble_moudle_id_is_gateway()){ // because IsSendOpBusy() doesn't make sense in gateway.
            if(p_proc->wait_status_tick){
                if(clock_time_exceed(p_proc->wait_status_tick, 3500*1000)){ //
                    p_proc->wait_status_tick = 0;
                }else{
                    break;
                }
            }
        //}

        if(IsSendOpBusy(1, (u16)mesh_sel)){
            break;
        }
        
        int tx_flag = 0;
        if(p_proc->total_shedule_bit&0x0001){
            tx_flag = 1;
            int err = access_cmd_schd_action_get(mesh_sel, 1,p_proc->schd_num);
            if(err){ // include tx busy
                break;  // do nothing, and retry after 200ms.
            }else{
                p_proc->wait_status_tick = clock_time()|1;
            }
        }
        p_proc->total_shedule_bit=(p_proc->total_shedule_bit>>1);
        p_proc->schd_num++;
        if(tx_flag){
            break;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
// CTLMeshDlg dialog

void CTLMeshDlg::InitCfgPar()
{
	m_pub_addr = _T("");
	m_sec_nw_bc = _T("");
	m_ttl = _T("");
	m_transimit = _T("");
	m_relay_val = _T("");
	m_friend_val = _T("");
	m_proxy_val = _T("");
	m_light_val = _T("");
	m_ct_val = _T("");
}

void CTLMeshDlg::InitStatusSelNode()
{
	m_Light.Format ("%04x", mesh_sel);
	InitCfgPar();
	
	memset(group_status, 0, sizeof(group_status));
	
	int i = 0;
	for (i=0; i<ALARM_CNT_MAX+1; i++)
	{
		
		SetItem (2, i, 0, GVIF_TEXT, "", 0);
		SetItem (2, i, 1, GVIF_TEXT, "", 0);
		SetItem (2, i, 2, GVIF_TEXT, "", 0);
		SetItem (2, i, 3, GVIF_TEXT, "", 0);
		SetItem (2, i, 4, GVIF_IMAGE, "", 5);
		if (i==0)
		{
		SetItem (2, i, 0, GVIF_TEXT, "id", 0);
		SetItem (2, i, 1, GVIF_TEXT, "action", 0);	   
		}	
	}
	m_GridShedule.AutoSize();
	for (i=0; i<MAX_SCENE_CNT+1; i++)
	{
		SetItem (3, i, 0, GVIF_TEXT, "", 0);
		SetItem (3, i, 1, GVIF_TEXT, "", 0);
		SetItem (3, i, 2, GVIF_TEXT, "", 0);
		SetItem (3, i, 3, GVIF_TEXT, "", 0);
		SetItem (3, i, 4, GVIF_IMAGE, "", 5);
		if (i==0)
		{
		SetItem (3, i, 0, GVIF_TEXT, "id", 0);
		SetItem (3, i, 1, GVIF_TEXT, "scene_number", 0);	   
		}	
	}
	m_GridScene.AutoSize();
	UpdateGroupStatus ();
    
    for (int i=0; i<ALARM_CNT_MAX; i++)
    {
        memset(&alarm_list[i], 0, sizeof(alarm_ev_t));
        alarm_list[i].action=action_default;
        alarm_list[i].year=SCHD_YEAR_ANY;
        alarm_list[i].month=BIT_RNG(0,11);//SCHD_MONTH_ANY;
        alarm_list[i].day=SCHD_DAY_ANY;//SCHD_DAY_ANY;
        alarm_list[i].week=BIT_RNG(0,6);//SCHD_WEEK_ANY;
        alarm_list[i].hour=8;//SCHD_HOUR_ANY;
        alarm_list[i].minute=0;//SCHD_MIN_ANY;
        alarm_list[i].second=0;//SCHD_SEC_ANY;
    }

    ShowSchedule(0);    // current_shedule=0;
    GetDlgItem(IDC_currenttime)->SetWindowTextA("");
	//UpdateSheduleStatus ();
	//UpdateSceneStatus ();
	UpdateData(FALSE);
}

void CTLMeshDlg::InitStatus()
{
	mesh_num = 0;
	mesh_sel = 0xffff;
	InitStatusSelNode();
	//int i;
	//for (i=0; i<MESH_NODE_MAX_NUM; i++)
	//{
		memset(mesh_status, 0, sizeof(mesh_status));
	//}

	UpdateOnlineStatus ();
}

CTLMeshDlg::CTLMeshDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTLMeshDlg::IDD, pParent)
	, m_reliable(RSP_TYPE_RELIABLE)
	, m_pub_addr(_T(""))
	, m_sec_nw_bc(_T(""))
	, m_ttl(_T(""))
	, m_transimit(_T(""))
	, m_relay_val(_T(""))
	, m_friend_val(_T(""))
	, m_proxy_val(_T(""))
	,m_light_val(_T(""))
	,m_ct_val (_T(""))
	, m_radio_on(0)
	//, m_all(FALSE)
	, m_friday(FALSE)
	, m_monday(FALSE)
	, m_saturday(FALSE)
	, m_sunday(FALSE)
	, m_thursday(FALSE)
	, m_tuesday(FALSE)
	, m_wednesday(FALSE)
	, m_yearsel(1)
	, m_daysel(0)
	, m_hoursel(0)
	, m_minutesel(0)
	, m_secondsel(0)
	, m_April(FALSE)
	, m_August(FALSE)
	, m_December(FALSE)
	, m_February(FALSE)
	, m_hour(0)
	, m_January(FALSE)
	, m_July(FALSE)
	, m_June(FALSE)
	, m_March(FALSE)
	, m_May(FALSE)
	, m_minute(0)
	, m_November(FALSE)
	, m_October(FALSE)
	, m_seconed(0)
	, m_September(FALSE)
	, m_year(0)
	, m_day(0)
	, m_recall(0)
	, m_scene_number(1)
{
	//{{AFX_DATA_INIT(CTLMeshDlg)
	m_Light = _T("ffff");
	//}}AFX_DATA_INIT
	m_pParent = pParent;
	m_nID = CTLMeshDlg::IDD;
	mesh_num = 0;
	mesh_sel = 0xffff;
	int i;
	memset(group_status, 0, sizeof(group_status));
	for (i=0; i<MESH_NODE_MAX_NUM; i++)
	{
		memset(mesh_status, 0, sizeof(mesh_status));
	}
	mLite = 0;
	current_shedule = 0;
	current_scene = 0;
	shedule_init = 0;
    memset(&UI_time, 0, sizeof(UI_time));
}

int is_light_model_client(u32 model_id, bool4 sig_model)
{
	if(sig_model){
		return ((SIG_MD_G_ONOFF_C == model_id)/*||(SIG_MD_G_LEVEL_C == model_id)*/);
	}
	return 0;
}

int is_light_model_server(u32 model_id, bool4 sig_model)
{
	if(sig_model){
		return ((SIG_MD_G_ONOFF_S == model_id)/*||(SIG_MD_G_LEVEL_S == model_id)*/);
	}
	return 0;
}

int is_light_model(u32 model_id, bool4 sig_model)
{
	return (is_light_model_client(model_id, sig_model)||is_light_model_server(model_id, sig_model));
}

unsigned char notify_adr_buf[256];
unsigned int  notify_adr_buf_idx;

void CTLMeshDlg::refresh_time_ui()
{
    mesh_UTC_t get_utc = {0};
    if((mesh_sel != 0xffff) && UI_time.TAI_sec){
        get_UTC((UI_time.TAI_sec + get_time_zone_offset_min(UI_time.zone_offset)*60),&get_utc);
    }else{
        LOG_MSG_ERR (TL_LOG_COMMON, 0, 0, "time value is invalid, please set time......",0);
    }
    CString msg;
    msg.Format("%d-%02d-%02d %02d:%02d:%02d", get_utc.year,get_utc.month,get_utc.day,get_utc.hour,get_utc.minute,get_utc.second);
    GetDlgItem(IDC_currenttime)->SetWindowTextA(msg);
}

void sys_timer_refresh_time_ui()
{
    if((m_pMeshDlg->mesh_sel != 0xffff) && m_pMeshDlg->UI_time.TAI_sec){
        m_pMeshDlg->UI_time.TAI_sec++;
        m_pMeshDlg->refresh_time_ui();
    }
}

void CTLMeshDlg::StatusNotify (unsigned char *p, int len)
{
	mesh_rc_rsp_t rsp;
	rsp.len = len;
	memcpy(&rsp.src, p, len);
    LOG_MSG_INFO (TL_LOG_CMD_RSP, (u8 *)&rsp.src, rsp.len, "Status Rsp______________");
	
	u16 op = rf_link_get_op_by_ac(rsp.data);
	u32 size_op = SIZE_OF_OP(op);
    u8 *par = rsp.data + size_op;
    u16 par_len = GET_PAR_LEN_FROM_RSP(rsp.len, size_op);
	
    #if MD_MESH_OTA_EN // VC_DISTRIBUTOR_UPDATE_CLIENT_EN
	if(mesh_ota_master_rx(&rsp, op, size_op)){
	    return ;
	}
	#endif
	if(COMPOSITION_DATA_STATUS == op){
		VC_node_cps_save((mesh_page0_t *)(rsp.data+2),rsp.src, rsp.len-6);
		return;
	}

	if ((G_ONOFF_STATUS == op) || (G_LEVEL_STATUS == op) || (LIGHTNESS_STATUS == op)
	 || (LIGHT_CTL_STATUS == op) || (LIGHTNESS_LINEAR_STATUS == op) || (G_POWER_LEVEL_STATUS == op)
	 || (LIGHT_HSL_STATUS == op) || (LIGHT_XYL_STATUS == op) || (LIGHT_LC_ONOFF_STATUS == op)){
		UpdateNode (&rsp, op, size_op);
		UpdateOnlineStatus ();
	}

    VC_node_info_t *p_info = 0;
    if(mesh_sel && is_unicast_adr(mesh_sel)){
        p_info = get_VC_node_info(mesh_sel, 0);
    }
    
    if(p_info && is_ele_in_node(rsp.src, p_info->node_adr, p_info->element_cnt))
	{
		if (CFG_SIG_MODEL_SUB_LIST == op){
			UpdateGroupList (&rsp, op, size_op);
			UpdateGroupStatus ();
		    if(shedule_init){
		        shedule_init = AUTO_GET_ST_SCENE;
		    }
		}else if (CFG_MODEL_SUB_STATUS == op){
			UpdateGroupSetRsp (&rsp, op, size_op);
			UpdateGroupStatus ();
		}else if (CFG_MODEL_PUB_STATUS == op){
			cfg_cmd_pub_status_handle (&rsp, op, size_op);
		}else if (CFG_BEACON_STATUS == op){
			u8 en = rsp.data[2];
			m_sec_nw_bc.Format("%x", en);
			SetDlgItemText(IDC_SEC_NW_BC, m_sec_nw_bc);
		}else if (CFG_DEFAULT_TTL_STATUS == op){
			u8 val = rsp.data[2];
			m_ttl.Format("%x", val);
			SetDlgItemText(IDC_TTl_VAL, m_ttl);
		}else if (CFG_NW_TRANSMIT_STATUS == op){
			u8 val = rsp.data[2];
			m_transimit.Format("%x", val);
			SetDlgItemText(IDC_Transimit, m_transimit);
		}else if (CFG_RELAY_STATUS == op){
			u8 val = rsp.data[2];
			m_relay_val.Format("%x", val);
			SetDlgItemText(IDC_RELAY_VAL, m_relay_val);
		}else if (CFG_FRIEND_STATUS == op){
			u8 val = rsp.data[2];
			m_friend_val.Format("%x", val);
			SetDlgItemText(IDC_FRIEND_VAL, m_friend_val);
		}else if (CFG_GATT_PROXY_STATUS == op){
			u8 val = rsp.data[2];
			m_proxy_val.Format("%x", val);
			SetDlgItemText(IDC_PROXY_VAL, m_proxy_val);
 		}else if (LIGHT_CTL_TEMP_STATUS == op){
            mesh_cmd_light_ctl_temp_st_t *p_temp_st = (mesh_cmd_light_ctl_temp_st_t *)par;
            int remain_flag = par_len >= sizeof(mesh_cmd_light_ctl_st_t);
            u8 tmp = temp_to_temp100(remain_flag ? p_temp_st->target_temp : p_temp_st->present_temp);
            m_ct_val.Format("%x", tmp);
            SetDlgItemText(IDC_CT_VAL, m_ct_val);
        }else if(SCHD_STATUS==op){
            alarm_ev_idx *alarm_idx = (alarm_ev_idx *)par;
            memset(&shedule_get_proc, 0, sizeof(shedule_get_proc));
			shedule_get_proc.total_shedule_bit=(alarm_idx->idx);
			KillTimer(1);
			SetTimer(1,50,NULL);
		}else if(SCHD_ACTION_STATUS==op){    
    		alarm_ev_t *alarm_staus = (alarm_ev_t *)par;

    		memcpy(&alarm_list[alarm_staus->idx], alarm_staus, sizeof(alarm_ev_t));
            if(ble_moudle_id_is_gateway()){
                schedule_get_proc_tick_check_and_clear();
            }else{
                //shedule_get_proc.wait_status_tick = 0; // can't clear here, because reliable is busy here.
            }
    		UpdateSheduleStatus();	
		}else if(SCENE_REG_STATUS==op){
		    if(shedule_init){
		        shedule_init = AUTO_GET_ST_TIME;
		    }
			memset(&scene_data,0,sizeof(scene_data));	
			memcpy(&(scene_data.id[0]), (par+3),(par_len-3));
		    UpdateSceneStatus();
		}else if(TIME_STATUS==op){		
		    if(shedule_init){
		        shedule_init = AUTO_GET_ST_SCHEDULER;
		    }
    		time_status_t *gettime= (time_status_t *)par;
    		memcpy(&UI_time, gettime, sizeof(UI_time));
    		refresh_time_ui();
		}
	}	
}

void UpdateNode_level(mesh_status_t *p_st, mesh_cmd_g_level_st_t *p_level, int par_len)
{
	s16 level_ui;
	if(par_len > 2){
		level_ui = p_level->target_level;
	}else{
		level_ui = p_level->present_level;
	}
	
	if(LEVEL_OFF == level_ui){
		p_st->onoff = 0;
	}else{
		p_st->onoff = 1;
		p_st->level = level2lum(level_ui);
	}
	p_st->online_sn = 1;    // be sure online when rx status.
}

void CTLMeshDlg::UpdateNode (mesh_rc_rsp_t *rsp, u16 op, u32 size_op)
{
	int i;
	#if(!DEBUG_SHOW_VC_SELF_EN)
	if((ele_adr_primary== rsp->src) || (rsp->dst == rsp->src)){
		return ;
	}
	#endif
	
	for (i=0; i<mesh_num; i++)
	{
		if (rsp->src == 0xffff || rsp->src == 0)
		{
			return;
		}
		if (rsp->src == mesh_status[i].adr)
		{
			break;
		}
	}
	if (i < MESH_NODE_MAX_NUM)
	{
		mesh_status_t *p_st = (mesh_status_t *)&mesh_status[i];
		if (i == mesh_num)
		{
			memset(p_st, 0, sizeof(mesh_status[0]));
			p_st->adr = rsp->src;
			mesh_num ++;
		}
		u8 *par = rsp->data + size_op;
		u16 par_len = GET_PAR_LEN_FROM_RSP(rsp->len, size_op);
		if((G_ONOFF_STATUS == op)||(LIGHT_LC_ONOFF_STATUS == op)){
			mesh_cmd_g_onoff_st_t *p_onoff = (mesh_cmd_g_onoff_st_t *)par;
			if(par_len >= sizeof(mesh_cmd_g_onoff_st_t)){
				p_st->onoff = p_onoff->target_onoff;
			}else{
				p_st->onoff = p_onoff->present_onoff;
			}
            p_st->online_sn = 1;    // be sure online when rx status.
		}else if(G_LEVEL_STATUS == op){
			UpdateNode_level(p_st, (mesh_cmd_g_level_st_t *)par, par_len);
		}else if((LIGHTNESS_STATUS) == op || (LIGHTNESS_LINEAR_STATUS == op)
			  || (G_POWER_LEVEL_STATUS == op)){
			mesh_cmd_lightness_st_t status;
			memcpy(&status, par, sizeof(status));
			if(LIGHTNESS_LINEAR_STATUS == op){
				status.present = linear_to_lightness(status.present);
				status.target = linear_to_lightness(status.target);
			}
			
			mesh_cmd_g_level_st_t level_tmp;
			int remain_flag = par_len >= sizeof(mesh_cmd_lightness_st_t);
			int len_tmp = 2;
			level_tmp.present_level = get_level_from_lightness(status.present);
			if(remain_flag){
				len_tmp = sizeof(mesh_cmd_g_level_st_t);
				level_tmp.target_level = get_level_from_lightness(status.target);;
				level_tmp.remain_t = status.remain_t;
			}
			
			UpdateNode_level(p_st, &level_tmp, len_tmp);
		}else if(LIGHT_CTL_STATUS == op){
			mesh_cmd_light_ctl_st_t *p_ctl_st = (mesh_cmd_light_ctl_st_t *)par;
			mesh_cmd_g_level_st_t level_tmp;
			int remain_flag = par_len >= sizeof(mesh_cmd_light_ctl_st_t);
			int len_tmp = 2;
			level_tmp.present_level = get_level_from_lightness(p_ctl_st->present_lightness);
			if(remain_flag){
				len_tmp = sizeof(mesh_cmd_g_level_st_t);
				level_tmp.target_level = get_level_from_lightness(p_ctl_st->target_lightness);;
				level_tmp.remain_t = p_ctl_st->remain_t;
			}
			
			UpdateNode_level(p_st, &level_tmp, len_tmp);
		}else if((LIGHT_HSL_STATUS == op) || (LIGHT_XYL_STATUS == op)){
			mesh_cmd_light_hsl_st_t *p_hsl_st = (mesh_cmd_light_hsl_st_t *)par;
			mesh_cmd_g_level_st_t level_tmp = {0};
			level_tmp.present_level = get_level_from_lightness(p_hsl_st->lightness);		
			UpdateNode_level(p_st, &level_tmp, 2);
		}
	}
}

void CTLMeshDlg::UpdateNodeByOnlineST (unsigned char *p)
{
	int i;
	for (i=0; i<mesh_num; i++)
	{
        mesh_node_st_val_t *p_st = (mesh_node_st_val_t *)p;
		int src = p_st->dev_adr;
		int valid = p[0] | (p[1] << 8); // check 2 bytes when dev adr is one byte
		if (valid == 0xffff || src == 0)
		{
			return;
		}

		if (src == mesh_status[i].adr)
		{
			break;
		}
	}
	if (i < MESH_NODE_MAX_NUM)
	{
		if (i == mesh_num)
		{
			mesh_num ++;
		}

		mesh_node_st_val_t *p_st = (mesh_node_st_val_t *)p;
		mesh_status[i].adr = p_st->dev_adr;
		mesh_status[i].online_sn = p_st->sn;
		mesh_status[i].level = p_st->par[0];
		mesh_status[i].online_ct = p_st->par[1];
		mesh_status[i].onoff = (mesh_status[i].level != 0);
	}
}

void update_online_st_pkt(u8 *p, int len)
{
    online_st_report_t *p_st_report = (online_st_report_t *)p;
    int ns = (len - OFFSETOF(online_st_report_t,node))/p_st_report->len_node_st_val; // "ns" may be minus for bad packet.
    for(int i = 0; i < (ns); ++i){
        m_pMeshDlg->UpdateNodeByOnlineST((u8 *)(p_st_report->node)+i*p_st_report->len_node_st_val);
    }
    m_pMeshDlg->UpdateOnlineStatus ();
}

u32 CTLMeshDlg::get_all_online_node(u16 *list, u32 max_cnt)
{
    u32 cnt = 0;
	int i = 0;
    if(mesh_num){
        u16 addr_connected = APP_get_GATT_connect_addr();
        int is_gatt_mode = IsMasterMode() && (addr_connected != 0);

    	for (i=0; i<mesh_num; i++)
    	{
    		mesh_status_t *p_st = &mesh_status[i];
    		if (p_st->adr && (p_st->adr != ele_adr_primary) && p_st->online_sn)
    		{
    		    if((0 == is_gatt_mode) || p_st->adr != addr_connected){
                    if((cnt + (is_gatt_mode ? 1 : 0)) >= max_cnt){
                        LOG_MSG_ERR(TL_LOG_WIN32,0,0,"error: nodes count over flow!");
                        break;
                    }
                    list[cnt++] = p_st->adr;
                }
    		}
        }

        if(is_gatt_mode){
            list[cnt++] = addr_connected;   // BLE local node last. No need now, because it will be handle when tx apply.
        }
    }
    return cnt;
}

int CTLMeshDlg::reliable_rsp_check(u16 *missing_addr, u16 max_num)
{
	unsigned int i, j;
	u16 missing_cnt=0;
	if ((mesh_tx_reliable.mat.adr_dst != ADR_ALL_NODES) || (mesh_tx_reliable.mat.rsp_max<=4)) return 0;
	for(i=0; i<mesh_tx_reliable.mat.rsp_max; i++)
	{
		for(j=0; j<mesh_tx_reliable.rsp_cnt;j++)
		{
			if (mesh_status[i].adr == slave_status_record[j].src)
			{
				break;
			}			
		}
		if (j == mesh_tx_reliable.rsp_cnt)
		{
			missing_addr[missing_cnt++] = mesh_status[i].adr;
		}
		if(missing_cnt >= max_num) break;
	}
	return missing_cnt;
}

int vc_check_reliable_rsp(u16 *missing_addr, u16 max_num)
{
	return m_pMeshDlg->reliable_rsp_check(missing_addr, max_num);
}

int vc_get_grp_idx_by_md(u16 md_id)
{
	return (SIG_MD_G_ONOFF_C == md_id);
}

u16 vc_get_sig_model_id_by_column(int column)
{
	if(3 == column){
		return SIG_MD_G_ONOFF_S;
	}else{
		return SIG_MD_G_ONOFF_C;
	}
}

void CTLMeshDlg::UpdateGroupList (mesh_rc_rsp_t *rsp, u16 op, u32 size_op)
{
	mesh_cfg_model_sub_list_sig_t *p_list = (mesh_cfg_model_sub_list_sig_t *)(rsp->data+size_op);

	if((ST_SUCCESS == p_list->status) && is_light_model(p_list->model_id, 1)){
		if (p_list->ele_adr != mesh_sel || mesh_sel == 0xffff)
			return;

		int i;
		int grp_idx = vc_get_grp_idx_by_md(p_list->model_id);
		int cnt = (rsp->len - ((int)p_list - ((int)rsp + sizeof(rsp->len))) -OFFSETOF(mesh_cfg_model_sub_list_sig_t,sub_adr)) / 2;
		if(cnt > 22){
			cnt = 22;
		}

		for (i=0; i<cnt ; i++)
		{
			u16 group_id = (p_list->sub_adr[i]);
			if(is_group_adr(group_id)){
				group_id &= (~0xC000);
				if (group_id < 22)
				{
					group_status[grp_idx][group_id + 1] = 1;
				}
			}else if(is_virtual_adr(group_id)){
				// virtual
			}
		}
	}
}

void CTLMeshDlg::UpdateGroupSetRsp (mesh_rc_rsp_t *rsp, u16 op, u32 size_op)
{
	mesh_cfg_model_sub_status_t *p_st = (mesh_cfg_model_sub_status_t *)(rsp->data+size_op);

	if(ST_SUCCESS == p_st->status){
		if (p_st->set.ele_adr != mesh_cfg_cmd_sub_set_par.ele_adr){
			return;
		}
		int op_sub_set = mesh_cfg_cmd_sub_set_par.op;

		bool4 sig_model = (rsp->len - ((int)p_st - (int)rsp) >= sizeof(mesh_cfg_model_sub_status_t)) ? 0 : 1;
		u32 model_id = sig_model ? (p_st->set.model_id & 0xffff) : p_st->set.model_id;
		if(sig_model){
			if(!is_light_model(model_id, sig_model)){
				return ;
			}
			
			u16 group_id = p_st->set.sub_adr;
			if(0 == group_id){
				if(CFG_MODEL_SUB_DEL_ALL == op_sub_set){
					int grp_idx = vc_get_grp_idx_by_md(model_id);
					memset(group_status[grp_idx], 0, sizeof(group_status[grp_idx]));
				}
			}else if(is_group_adr(group_id)){
				group_id &= (~0xC000);
				if(group_id < 22){
					int grp_idx = vc_get_grp_idx_by_md(model_id);
					if(CFG_MODEL_SUB_ADD == op_sub_set){
						group_status[grp_idx][group_id + 1] = 1;
					}else if(CFG_MODEL_SUB_DEL == op_sub_set){
						group_status[grp_idx][group_id + 1] = 0;
					}else if(CFG_MODEL_SUB_OVER_WRITE == op_sub_set){
					}
				}
			}else if(is_virtual_adr(group_id)){
				if(CFG_MODEL_SUB_VIRTUAL_ADR_ADD == op_sub_set){
				}else if(CFG_MODEL_SUB_VIRTUAL_ADR_DEL == op_sub_set){
				}else if(CFG_MODEL_SUB_VIRTUAL_ADR_OVER_WRITE == op_sub_set){
				}
			}
		}else{
			// vendor model
		}
	}else if(ST_INSUFFICIENT_RES == p_st->status){
		LOG_MSG_ERR(TL_LOG_WIN32,0,0,"error: ST_INSUFFICIENT_RES ");
		//AfxMessageBox("error: ST_INSUFFICIENT_RES");
	}else{
		LOG_MSG_ERR(TL_LOG_WIN32,0,0,"error: other error status ");
		//AfxMessageBox("");
	}

	mesh_cfg_cmd_sub_set_par.op = -1;
	mesh_cfg_cmd_sub_set_par.ele_adr = 0;
}

void CTLMeshDlg::cfg_cmd_pub_status_handle (mesh_rc_rsp_t *rsp, u16 op, u32 size_op)
{
	mesh_cfg_model_pub_st_t *p_st = (mesh_cfg_model_pub_st_t *)(rsp->data+size_op);

	if(ST_SUCCESS == p_st->status){
		bool4 sig_model = (rsp->len - ((int)p_st - (int)rsp) >= sizeof(mesh_cfg_model_pub_st_t)) ? 0 : 1;
		u32 model_id = sig_model ? (p_st->set.model_id & 0xffff) : p_st->set.model_id;
		if(sig_model){
			if(is_light_model_server(model_id, sig_model)){
				//write to model data base in master later
				PubAddrUpdate(p_st->set.pub_adr);
			}
		}else{
			// vendor model
		}
	}else if(ST_INSUFFICIENT_RES == p_st->status){
		LOG_MSG_ERR(TL_LOG_WIN32,0,0,"error: ST_INSUFFICIENT_RES ");
		//AfxMessageBox("error: ST_INSUFFICIENT_RES");
	}else{
		LOG_MSG_ERR(TL_LOG_WIN32,0,0,"error: other error status ");
		//AfxMessageBox("");
	}
}

void CTLMeshDlg::UpdateOnlineStatus ()
{
	// gatt node put to the first.
	if (mesh_num > 0) {
		u16 connect_addr = APP_get_GATT_connect_addr();
		for(int i=0;i<mesh_num;i++) {
			if (mesh_status[i].adr == connect_addr) {
				if (i != 0) {
					mesh_status_t tmp;
					tmp = mesh_status[0];
					mesh_status[0] = mesh_status[i];
					mesh_status[i] = tmp;
				}
				break;
			}
		}
	}


    // rank by address
    int start_idx = ble_moudle_id_is_kmadongle() ? 1 : 0;   // GATT conneced node at first.   
    for(int i=start_idx;i<mesh_num;i++) {
        for(int j=i;j<mesh_num;j++) {
            if (mesh_status[i].adr > mesh_status[j].adr) {
                mesh_status_t tmp = mesh_status[i];
                mesh_status[i] = mesh_status[j];
                mesh_status[j] = tmp;
            }
        }
    }


	char buff[128];
	int cur_idx = 0;
	int i=0;
	for (i=0; i<MESH_NODE_MAX_NUM; i++)
	{
		mesh_status_t *p_st = &mesh_status[i];
		int adr 	= p_st->adr;
		int online 	= (p_st->online_sn != 0);
		if (i < mesh_num && adr)
		{
			sprintf_s (buff, sizeof(buff), "%03d", (i+1));
			SetItem (0, cur_idx, 0, GVIF_TEXT, buff, 0);
			sprintf_s (buff, sizeof(buff), "%04x", adr);
			SetItem (0, cur_idx, 1, GVIF_TEXT, buff, 0);
			SetItem (0, cur_idx, 2, GVIF_TEXT, "On", 0);
			SetItem (0, cur_idx, 3, GVIF_TEXT, "Off", 0);
			SetItem (0, cur_idx, 4, GVIF_IMAGE, "", online ? (p_st->onoff ? 0 : 1) : 2);
			sprintf_s (buff, sizeof(buff), "%d", p_st->onoff ? p_st->level : LUM_OFF);
			SetItem (0, cur_idx, 5, GVIF_TEXT, online ? buff : "", 0);
			++cur_idx;
			if (mesh_sel == adr){
				m_light_val.Format("%x", p_st->level);
				SetDlgItemText(IDC_LEVEL_VAL, m_light_val);
			}			
		}
		else
		{
			SetItem (0, cur_idx, 0, GVIF_TEXT, "", 0);
			SetItem (0, cur_idx, 1, GVIF_TEXT, "", 0);
			SetItem (0, cur_idx, 2, GVIF_TEXT, "", 0);
			SetItem (0, cur_idx, 3, GVIF_TEXT, "", 0);
			SetItem (0, cur_idx, 4, GVIF_IMAGE, "", 2);
			SetItem (0, cur_idx, 5, GVIF_TEXT, "", 0);
		}
	}
	for (i=cur_idx; i<MESH_NODE_MAX_NUM; i++)
	{
		SetItem (0, i, 0, GVIF_TEXT, "", 0);
		SetItem (0, i, 1, GVIF_TEXT, "", 0);
		SetItem (0, i, 2, GVIF_TEXT, "", 0);
		SetItem (0, i, 3, GVIF_TEXT, "", 0);
		SetItem (0, i, 4, GVIF_IMAGE, "", 2);
		SetItem (0, i, 5, GVIF_TEXT, "", 0);
	}
	//m_Grid.Invalidate ();
	m_Grid.AutoSize();
}

void CTLMeshDlg::InitOnlineStatusOffline ()
{
	int i=0;

	int swap_ok = 0;
	mesh_status_t node_1st_backup = {0};
	memcpy(&node_1st_backup, &mesh_status[0], sizeof(mesh_status_t));
	for (i=0; i<MESH_NODE_MAX_NUM; i++)
	{
		mesh_status_t *p_st = &mesh_status[i];
		if (i < mesh_num && p_st->adr)
		{
            p_st->online_sn = 0;
            if(!swap_ok && IsMasterMode() && (i && (connect_addr_gatt == p_st->adr))){
                memcpy(&mesh_status[0], p_st, sizeof(mesh_status_t));
                memcpy(p_st, &node_1st_backup, sizeof(mesh_status_t));
                swap_ok = 1;
            }
		}
    }
	UpdateOnlineStatus ();
}

u32 CTLMeshDlg::GetOnlineStatusRspMax ()
{
    int rsp_max = 0;
	int i=0;
	for (i=0; i<MESH_NODE_MAX_NUM; i++)
	{
		mesh_status_t *p_st = &mesh_status[i];
		if (i < mesh_num && p_st->adr && p_st->online_sn)
		{
            rsp_max++;
		}
    }
    return rsp_max;
}

void CTLMeshDlg::UpdateGroupStatus ()
{
	char buff[128];
	for (int i=0; i<24; i++)
	{
		sprintf_s (buff, sizeof(buff), "%d", i - 1);
		if (i==0){
			SetItem (1, i, 0, GVIF_TEXT, "All", 0);
			SetItem (1, i, 3, GVIF_TEXT, "Svr", 0);
			SetItem (1, i, 4, GVIF_TEXT, "Clnt", 0);
		}
		else{
			SetItem (1, i, 0, GVIF_TEXT, buff, 0);
			SetItem (1, i, 3, GVIF_IMAGE, "", group_status[0][i] ? 4 : 5);
			SetItem (1, i, 4, GVIF_IMAGE, "", group_status[1][i] ? 4 : 5);
		}
		SetItem (1, i, 1, GVIF_TEXT, "On", 0);
		SetItem (1, i, 2, GVIF_TEXT, "Off", 0);
		
	}
	//m_GridGroup.Invalidate ();
	m_GridGroup.AutoSize();
}

void CTLMeshDlg::UpdateSheduleStatus ()
{
	char buff[128];
	int i = 0;	
	if(mesh_sel!=0xffff){
    	for(i=0;i<ALARM_CNT_MAX;i++){
            ///////////////////////////////////////////////
            sprintf_s (buff, "%d", i);
        	SetItem (2, i+1, 0, GVIF_TEXT, buff, 0);//idx
          
        	if(alarm_list[i].action!=action_default){
            	if(alarm_list[i].action==SCHD_ACTION_OFF){
            	    SetItem (2, i+1, 1, GVIF_TEXT, "off", 0);//action
            	}
            	else if(alarm_list[i].action==SCHD_ACTION_ON){	
            	    SetItem (2, i+1, 1, GVIF_TEXT, "on", 0);	
            	}
            	else if(alarm_list[i].action==SCHD_ACTION_SCENE){	
                    CString str = _T("");
                	str.Format("Scene%02d",alarm_list[i].scene_id);
                	SetItem (2, i+1, 1, GVIF_TEXT, (LPSTR)(LPCTSTR)str, 0);
            	}
            	else if(alarm_list[i].action==SCHD_ACTION_NONE  ){	
            	    SetItem (2, i+1, 1, GVIF_TEXT, "no action", 0);	
            	}
            	/////////////////////////////////////////
        	}
        }
	}
	m_GridShedule.AutoSize();
}

void CTLMeshDlg::UpdateSceneStatus ()
{
	char buff[128];
	int i = 0;
	if(mesh_sel!=0xffff){	
      for (i=0; i<(MAX_SCENE_CNT); i++)
	{
		if(scene_data.id[i]){
        sprintf_s (buff, sizeof(buff), "%02d", i);
        SetItem (3, i+1, 0, GVIF_TEXT, buff, 0);
		sprintf_s (buff, sizeof(buff), "%02d", scene_data.id[i]);
		SetItem (3, i+1, 1, GVIF_TEXT, buff, 0);
		}
		else{
		SetItem (3, i+1, 0, GVIF_TEXT, "", 0);
		SetItem (3, i+1, 1, GVIF_TEXT, "", 0);
		}
	}
	}
	m_GridScene.AutoSize();
}

void SendOpPara_vc(u8 *cmd, int len)
{
	m_pMeshDlg->SendOpPara(cmd,len);
}

void CTLMeshDlg::SendOpPara (u8 *cmd, int len)
{
	g_module_dlg->ExecOpPara (cmd, len);
}
CString csSetMac=_T("16,00,00,00,00,00");

void CTLMeshDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTLMeshDlg)
	DDX_Text(pDX, IDC_EDIT_MAC,csSetMac);
	DDV_MaxChars(pDX, csSetMac, 256);
	DDX_Text(pDX, IDC_LIGHT, m_Light);
	DDX_Control(pDX, IDC_GRID, m_Grid);
	DDX_Control(pDX, IDC_GRID_GROUP, m_GridGroup);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_PUB_ADDR, m_pub_addr);
	DDX_Text(pDX, IDC_SEC_NW_BC, m_sec_nw_bc);
	DDV_MaxChars(pDX, m_sec_nw_bc, 1);
	DDX_Text(pDX, IDC_TTl_VAL, m_ttl);
	DDX_Text(pDX, IDC_Transimit, m_transimit);
	DDX_Text(pDX, IDC_RELAY_VAL, m_relay_val);
	DDX_Text(pDX, IDC_FRIEND_VAL, m_friend_val);
	DDX_Text(pDX, IDC_PROXY_VAL, m_proxy_val);
	DDX_Text(pDX, IDC_LEVEL_VAL, m_light_val);
	DDX_Text(pDX, IDC_CT_VAL, m_ct_val);
	DDX_Radio(pDX, IDC_ON, m_radio_on);
	//DDX_Check(pDX, IDC_ALL, m_all);
	DDX_Check(pDX, IDC_FRIDAY, m_friday);
	DDX_Control(pDX, IDC_GRID_SCENE, m_GridScene);
	DDX_Control(pDX, IDC_GRID_SHEDULE, m_GridShedule);
	DDX_Check(pDX, IDC_MONDAY, m_monday);
	DDX_Check(pDX, IDC_SATURDAY, m_saturday);
	DDX_Check(pDX, IDC_SUNDAY, m_sunday);
	DDX_Check(pDX, IDC_THURSDAY, m_thursday);
	DDX_Check(pDX, IDC_TUESDAY, m_tuesday);
	DDX_Check(pDX, IDC_WEDNESDAY, m_wednesday);
	DDX_Radio(pDX, IDC_ANY_YEAR, m_yearsel);
	DDX_Radio(pDX, IDC_ANY_DAY, m_daysel);
	DDX_Radio(pDX, IDC_ANY_HOUR, m_hoursel);
	DDX_Radio(pDX, IDC_ANY_MINUTE, m_minutesel);
	DDX_Radio(pDX, IDC_ANY_SECOND, m_secondsel);
	DDX_Check(pDX, IDC_APR, m_April);
	DDX_Check(pDX, IDC_AUG, m_August);
	DDX_Check(pDX, IDC_DEC, m_December);
	DDX_Check(pDX, IDC_FEB, m_February);
	DDX_Text(pDX, IDC_HOUR, m_hour);
	DDX_Check(pDX, IDC_JAN, m_January);
	DDX_Check(pDX, IDC_JUL, m_July);
	DDX_Check(pDX, IDC_JUN, m_June);
	DDX_Check(pDX, IDC_MAR, m_March);
	DDX_Check(pDX, IDC_MAY, m_May);
	DDX_Text(pDX, IDC_MINUTE, m_minute);
	DDX_Check(pDX, IDC_NOV, m_November);
	DDX_Check(pDX, IDC_OCT, m_October);
	DDX_Text(pDX, IDC_SECOND, m_seconed);
	DDX_Check(pDX, IDC_SEP, m_September);
	DDX_Text(pDX, IDC_YEAR, m_year);
	DDX_Text(pDX, IDC_EDIT_DAY, m_day);
	DDX_Text(pDX, IDC_EDIT_RECALL, m_recall);
	DDX_Text(pDX, IDC_scenenumber, m_scene_number);
	DDX_Control(pDX, IDC_ONLINE_SEL, m_online_box);
}


BEGIN_MESSAGE_MAP(CTLMeshDlg, CDialog)
	//{{AFX_MSG_MAP(CTLMeshDlg)
	ON_BN_CLICKED(IDC_NODE, OnNode)
	ON_BN_CLICKED(IDC_GROUP, OnButtonGetgroup)
	//}}AFX_MSG_MAP
    ON_NOTIFY(NM_DBLCLK, IDC_GRID, OnGridDblClick)
    ON_NOTIFY(NM_CLICK, IDC_GRID, OnGridClick)
	ON_NOTIFY(NM_RCLICK, IDC_GRID, OnGridRClick)

    ON_NOTIFY(NM_DBLCLK, IDC_GRID_GROUP, OnGridGroupDblClick)
    ON_NOTIFY(NM_CLICK, IDC_GRID_GROUP, OnGridGroupClick)
	ON_NOTIFY(NM_CLICK, IDC_GRID_SHEDULE, OnGridSheduleClick)
	ON_NOTIFY(NM_CLICK, IDC_GRID_SCENE, OnGridSceneClick)
	ON_NOTIFY(NM_RCLICK, IDC_GRID_GROUP, OnGridGroupRClick)
	ON_BN_CLICKED(IDC_SET_MAC, &CTLMeshDlg::OnBnClickedSetMac)
ON_BN_CLICKED(IDC_GROUP2, &CTLMeshDlg::OnBnClickedGroup2)
ON_BN_CLICKED(IDC_GET_PUB, &CTLMeshDlg::OnBnClickedGetPub)
ON_BN_CLICKED(IDC_GET_SEC_NW_BC, &CTLMeshDlg::OnBnClickedGetSecNwBc)
ON_BN_CLICKED(IDC_GetTTL, &CTLMeshDlg::OnBnClickedGetttl)
ON_BN_CLICKED(IDC_GET_Transimit, &CTLMeshDlg::OnBnClickedGetTransimit)
ON_BN_CLICKED(IDC_GET_RELAY, &CTLMeshDlg::OnBnClickedGetRelay)
ON_BN_CLICKED(IDC_GET_FRIEND, &CTLMeshDlg::OnBnClickedGetFriend)
ON_BN_CLICKED(IDC_GET_PROXY, &CTLMeshDlg::OnBnClickedGetProxy)
ON_BN_CLICKED(IDC_GET_CPS, &CTLMeshDlg::OnBnClickedGetCps)
ON_BN_CLICKED(IDC_DEL_NODE, &CTLMeshDlg::OnBnClickedDelNode)
ON_BN_CLICKED(IDC_GrpDelAll_S, &CTLMeshDlg::OnBnClickedGrpdelallS)
ON_BN_CLICKED(IDC_GrpDelAll_C, &CTLMeshDlg::OnBnClickedGrpdelallC)
ON_BN_CLICKED(IDC_GET_LEVEL, &CTLMeshDlg::OnBnClickedGetLevel)
ON_BN_CLICKED(IDC_GET_CT, &CTLMeshDlg::OnBnClickedGetCt)
ON_BN_CLICKED(IDC_ANY_YEAR, &CTLMeshDlg::OnClickedAnyYear)
ON_BN_CLICKED(IDC_CUSTOM_YEAR, &CTLMeshDlg::OnClickedAnyYear)
ON_BN_CLICKED(IDC_ANY_DAY, &CTLMeshDlg::OnClickedAnyDay)
ON_BN_CLICKED(IDC_CUSTOM_DAY, &CTLMeshDlg::OnClickedAnyDay)
ON_BN_CLICKED(IDC_ONCE_DAY, &CTLMeshDlg::OnClickedAnyDay)
ON_BN_CLICKED(IDC_ANY_HOUR, &CTLMeshDlg::OnClickedAnyHour)
ON_BN_CLICKED(IDC_ONCE_DAY, &CTLMeshDlg::OnClickedAnyHour)
ON_BN_CLICKED(IDC_CUSTOM_HOUR, &CTLMeshDlg::OnClickedAnyHour)
ON_BN_CLICKED(IDC_ANY_MINUTE, &CTLMeshDlg::OnClickedAnyMinute)
ON_BN_CLICKED(IDC_EVERY_15_MIN, &CTLMeshDlg::OnClickedAnyMinute)
ON_BN_CLICKED(IDC_EVERY_20_MIN, &CTLMeshDlg::OnClickedAnyMinute)
ON_BN_CLICKED(IDC_ONCE_HOUR, &CTLMeshDlg::OnClickedAnyMinute)
ON_BN_CLICKED(IDC_CUSTOM_MIN, &CTLMeshDlg::OnClickedAnyMinute)
ON_BN_CLICKED(IDC_ANY_SECOND, &CTLMeshDlg::OnClickedAnySecond)
ON_BN_CLICKED(IDC_EVERY_15_SEC, &CTLMeshDlg::OnClickedAnySecond)
ON_BN_CLICKED(IDC_EVERY_20_SEC, &CTLMeshDlg::OnClickedAnySecond)
ON_BN_CLICKED(IDC_ONCE_MIN, &CTLMeshDlg::OnClickedAnySecond)
ON_BN_CLICKED(IDC_CUSTOM_SEC, &CTLMeshDlg::OnClickedAnySecond)
ON_BN_CLICKED(IDC_ON, &CTLMeshDlg::OnClickedOn)
ON_BN_CLICKED(IDC_OFF, &CTLMeshDlg::OnClickedOn)
ON_BN_CLICKED(IDC_NO_ACTION, &CTLMeshDlg::OnClickedOn)
ON_BN_CLICKED(IDC_SCENE_RECALL, &CTLMeshDlg::OnClickedOn)
ON_BN_CLICKED(IDC_ADD, &CTLMeshDlg::OnBnClickedAdd)
ON_BN_CLICKED(IDC_SCENE_DEL, &CTLMeshDlg::OnBnClickedSceneDel)
ON_BN_CLICKED(IDC_SCENE_LOAD, &CTLMeshDlg::OnBnClickedSceneRecall)
ON_BN_CLICKED(IDC_SCENE_ADD, &CTLMeshDlg::OnBnClickedSceneAdd)
ON_BN_CLICKED(IDC_SET_TIME, &CTLMeshDlg::OnBnClickedSetTime)
ON_BN_CLICKED(IDC_GET_TIME, &CTLMeshDlg::OnBnClickedGetTime)
ON_WM_TIMER()
ON_EN_KILLFOCUS(IDC_YEAR, &CTLMeshDlg::OnEnKillfocusYear)
ON_EN_KILLFOCUS(IDC_EDIT_DAY, &CTLMeshDlg::OnEnKillfocusEditDay)
ON_EN_KILLFOCUS(IDC_HOUR, &CTLMeshDlg::OnEnKillfocusHour)
ON_EN_KILLFOCUS(IDC_MINUTE, &CTLMeshDlg::OnEnKillfocusMinute)
ON_EN_KILLFOCUS(IDC_SECOND, &CTLMeshDlg::OnEnKillfocusSecond)
ON_EN_KILLFOCUS(IDC_EDIT_RECALL, &CTLMeshDlg::OnEnKillfocusEditRecall)
ON_EN_KILLFOCUS(IDC_scenenumber, &CTLMeshDlg::OnEnKillfocusscenenumber)
ON_CBN_SELCHANGE(IDC_ONLINE_SEL, &CTLMeshDlg::OnSelchangeOnlineSel)
ON_WM_CLOSE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTLMeshDlg message handlers

void CTLMeshDlg::SetItem(int group, int row, int col, int type, char *text, int image)
{

	GV_ITEM Item;
	
	Item.mask = GVIF_TEXT;
	Item.row = row;
	Item.col = col;
	Item.crBkClr = RGB(128, 128, 128);         
	Item.crFgClr = RGB(255,255,0);		
	if (type == GVIF_IMAGE)
	{
		Item.mask = GVIF_TEXT | GVIF_BKCLR | GVIF_FGCLR | GVIF_IMAGE;
		Item.iImage = image;
	}
	else
	{
		Item.mask = GVIF_TEXT | GVIF_BKCLR | GVIF_FGCLR;
		Item.strText = text;
	}

	if (group == 1)
	{
		m_GridGroup.SetItem(&Item);
	}
	else if (group == 2)
	{
		m_GridShedule.SetItem(&Item);
	}
	else if (group == 3)
	{
		m_GridScene.SetItem(&Item);
	}
	else
	{
		m_Grid.SetItem(&Item);
	}
}

BOOL CTLMeshDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_ImageList.Create(MAKEINTRESOURCE(IDB_IMAGES), 16, 1, RGB(255,255,255));
	m_Grid.SetImageList(&m_ImageList);

	//m_Grid.EnableDragAndDrop(TRUE);
	m_Grid.GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(0xFF, 0xFF, 0xE0));
	m_Grid.SetEditable(0);
	m_Grid.SetVirtualMode(0);

	m_Grid.SetAutoSizeStyle();
	m_Grid.SetRowCount(MESH_NODE_MAX_NUM);
	m_Grid.SetColumnCount(6);
	

	m_GridGroup.SetImageList(&m_ImageList);

	//m_GridGroup.EnableDragAndDrop(TRUE);
	m_GridGroup.GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(0xFF, 0xFF, 0xE0));
	m_GridGroup.SetEditable(0);
	m_GridGroup.SetVirtualMode(0);

	m_GridGroup.SetAutoSizeStyle();
	m_GridGroup.SetRowCount(MESH_NODE_MAX_NUM);
	m_GridGroup.SetColumnCount(5);

	//m_GridGroup.EnableDragAndDrop(TRUE);
	m_GridShedule.GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(0xFF, 0xFF, 0xE0));
	m_GridShedule.SetEditable(0);
	m_GridShedule.SetVirtualMode(0);

	m_GridShedule.SetAutoSizeStyle();
	m_GridShedule.SetRowCount(ALARM_CNT_MAX+1);
	m_GridShedule.SetColumnCount(5);

	m_GridScene.SetImageList(&m_ImageList);

	//m_GridGroup.EnableDragAndDrop(TRUE);
	m_GridScene.GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(0xFF, 0xFF, 0xE0));
	m_GridScene.SetEditable(0);
	m_GridScene.SetVirtualMode(0);

	m_GridScene.SetAutoSizeStyle();
	m_GridScene.SetRowCount(MAX_SCENE_CNT+1);
	m_GridScene.SetColumnCount(5);

	m_online_box.AddString("unreliable");
	m_online_box.AddString("reliable");
	m_online_box.AddString("online status");
	m_online_box.SetCurSel(1);
	InitStatus ();
	((CEdit*)GetDlgItem(IDC_PUB_ADDR))->SetLimitText(4);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CTLMeshDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// TODO: Add your specialized code here and/or call the base class
    if (wParam == (WPARAM)m_Grid.GetDlgCtrlID())
    {
        *pResult = 1;
        GV_DISPINFO *pDispInfo = (GV_DISPINFO*)lParam;
        if (GVN_GETDISPINFO == pDispInfo->hdr.code)
        {
            //TRACE2("Getting Display info for cell %d,%d\n", pDispInfo->item.row, pDispInfo->item.col);
            pDispInfo->item.strText.Format(_T("Message %d,%d"),pDispInfo->item.row, pDispInfo->item.col);
            return TRUE;
        }
        else if (GVN_ODCACHEHINT == pDispInfo->hdr.code)
        {
            GV_CACHEHINT *pCacheHint = (GV_CACHEHINT*)pDispInfo;
        }
    }
    
	return CDialog::OnNotify(wParam, lParam, pResult);
}

void CTLMeshDlg::OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
    //Trace(_T("Double Clicked on row %d, col %d\n"), pItem->iRow, pItem->iColumn);
	// add custom code here
	CString str = "";
	CString type, key, value;
	char stdbuff[] = "stdout.txt";

	type = m_Grid.GetItemText (pItem->iRow, 0);
	if (pItem->iColumn == 0 && pItem->iRow < mesh_num)			// light on
	{
        #if 1
        InitStatusSelNode();
        #else
		OnButtonGetgroup ();
        #endif
		int adr = mesh_status[pItem->iRow].adr;
		m_Light.Format ("%04x", adr);
		UpdateData(FALSE);
		mesh_sel = adr;
		if(is_support_model_dst(mesh_sel,SIG_MD_SCHED_S,1)){
            UpdateSheduleStatus (); // show schedule index in UI.
        }

        shedule_init = AUTO_GET_ST_GROUP;
	}
}
void CTLMeshDlg::OnGridSceneClick(NMHDR *pNotifyStruct, LRESULT* pResult){
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	if(((pItem->iRow)>0)&&((pItem->iRow)<17) ){
	current_scene=_ttoi(m_GridScene.GetItemText (pItem->iRow, 1));
	}
}

void CTLMeshDlg::ShowSchedule(int shedule_index)
{
    if((shedule_index >= 16) || (shedule_index < 0)){
        LOG_MSG_ERR(TL_LOG_COMMON, 0, 0, "shedule_index error",0);
        return ;
    }
	current_shedule=shedule_index ;
    
    if(alarm_list[current_shedule].year==SCHD_YEAR_ANY){//year
    GetDlgItem(IDC_YEAR)->EnableWindow(FALSE);
    m_yearsel=0;
    m_year=0;
    }
    else {
    m_yearsel=1;
    GetDlgItem(IDC_YEAR)->EnableWindow(TRUE);
    m_year=alarm_list[current_shedule].year;
    }
////////////////////////////////////////////////////////
    if(alarm_list[current_shedule].month&0x0001){
    m_January=1;
    }
    else{
    m_January=0;
    }
    if(alarm_list[current_shedule].month&(0x0001<<1)){
    m_February=1;
    }
    else{
    m_February=0;
    }
    if(alarm_list[current_shedule].month&(0x0001<<2)){
    m_March=1;
    }
    else{
    m_March=0;
    }
    if(alarm_list[current_shedule].month&(0x0001<<3)){
    m_April=1;
    }
    else{
    m_April=0;
    }
    if(alarm_list[current_shedule].month&(0x0001<<4)){
    m_May=1;
    }
    else{
    m_May=0;
    }
    if(alarm_list[current_shedule].month&(0x0001<<5)){
    m_June=1;
    }
    else{
    m_June=0;
    }
    if(alarm_list[current_shedule].month&(0x0001<<6)){
    m_July=1;
    }
    else{
    m_July=0;
    }
    if(alarm_list[current_shedule].month&(0x0001<<7)){
    m_August=1;
    }
    else{
    m_August=0;
    }
    if(alarm_list[current_shedule].month&(0x0001<<8)){
    m_September=1;
    }
    else{
    m_September=0;
    }
    if(alarm_list[current_shedule].month&(0x0001<<9)){
    m_October=1;
    }
    else{
    m_October=0;
    }
    if(alarm_list[current_shedule].month&(0x0001<<10)){
    m_November=1;
    }
    else{
    m_November=0;
    }
    if(alarm_list[current_shedule].month&(0x0001<<11)){
    m_December=1;
    }
    else{
    m_December=0;
    }
////////////////////////////////////////////////////////
    if(alarm_list[current_shedule].day== SCHD_DAY_ANY){//day
    GetDlgItem(IDC_EDIT_DAY)->EnableWindow(FALSE);
    m_daysel=0; 
    m_day=1;
    }
    else{   
    GetDlgItem(IDC_EDIT_DAY)->EnableWindow(TRUE);
    m_day=alarm_list[current_shedule].day;
    }
    if(alarm_list[current_shedule].hour==SCHD_HOUR_ANY){//hour
    GetDlgItem(IDC_HOUR)->EnableWindow(FALSE);
    m_hoursel=0;
    m_hour=0;
    }
    else if(alarm_list[current_shedule].hour==SCHD_HOUR_RANDOM){
    GetDlgItem(IDC_HOUR)->EnableWindow(FALSE);
    m_hoursel=1;
    m_hour=0;
    }
    else{
    GetDlgItem(IDC_HOUR)->EnableWindow(TRUE);
    m_hoursel=2;
    m_hour=alarm_list[current_shedule].hour;
    }
    if(alarm_list[current_shedule].minute==SCHD_MIN_ANY){//minute
    GetDlgItem(IDC_MINUTE)->EnableWindow(FALSE);
    m_minutesel=0;
    m_minute=0;
    }
    else if(alarm_list[current_shedule].minute==SCHD_MIN_EVERY15){
    GetDlgItem(IDC_MINUTE)->EnableWindow(FALSE);
    m_minutesel=1;
    m_minute=0;
    }
    else if(alarm_list[current_shedule].minute==SCHD_MIN_EVERY20){
    GetDlgItem(IDC_MINUTE)->EnableWindow(FALSE);
    m_minutesel=2;
    m_minute=0;
    }
    else if(alarm_list[current_shedule].minute==SCHD_MIN_RANDOM){
    GetDlgItem(IDC_MINUTE)->EnableWindow(FALSE);
    m_minutesel=3;
    m_minute=0;
    }
    else{
    GetDlgItem(IDC_MINUTE)->EnableWindow(TRUE);
    m_minutesel=4;
    m_minute=alarm_list[current_shedule].minute;
    }
    if(alarm_list[current_shedule].second== SCHD_SEC_ANY){//second
    GetDlgItem(IDC_SECOND)->EnableWindow(FALSE);
    m_secondsel=0;
    m_seconed=0;
    }
    else if(alarm_list[current_shedule].second==SCHD_SEC_EVERY15){
    GetDlgItem(IDC_SECOND)->EnableWindow(FALSE);
    m_secondsel=1;
    m_seconed=0;
    }
    else if(alarm_list[current_shedule].second== SCHD_SEC_EVERY20){
    GetDlgItem(IDC_SECOND)->EnableWindow(FALSE);
    m_secondsel=2;
    m_seconed=0;
    }
    else if(alarm_list[current_shedule].second==SCHD_SEC_RANDOM){
    GetDlgItem(IDC_SECOND)->EnableWindow(FALSE);
    m_secondsel=3;
    m_seconed=0;
    }
    else {
    GetDlgItem(IDC_SECOND)->EnableWindow(TRUE);
    m_secondsel=4;
    m_seconed=alarm_list[current_shedule].second;   
    }
    //////////////////////////////////////////////
    if(alarm_list[current_shedule].week&0x01){
    m_monday=1;
    }
    else{
    m_monday=0;
    }
    if(alarm_list[current_shedule].week&(0x01<<1)){
    m_tuesday=1;
    }
    else{
    m_tuesday=0;
    }
    if(alarm_list[current_shedule].week&(0x01<<2)){
    m_wednesday=1;
    }
    else{
    m_wednesday=0;
    }
    if(alarm_list[current_shedule].week&(0x01<<3)){
    m_thursday=1;
    }
    else{
    m_thursday=0;
    }
    if(alarm_list[current_shedule].week&(0x01<<4)){
    m_friday=1;
    }
    else{
    m_friday=0;
    }
    if(alarm_list[current_shedule].week&(0x01<<5)){
    m_saturday=1;
    }
    else{
    m_saturday=0;
    }
    if(alarm_list[current_shedule].week&(0x01<<6)){
    m_sunday=1;
    }
    else{
    m_sunday=0;
    }
    
    //////////////////////////////////////////////
    if(alarm_list[current_shedule].action==SCHD_ACTION_ON){//action
    m_recall=1;
    GetDlgItem(IDC_EDIT_RECALL)->EnableWindow(FALSE);
    m_radio_on=0;
    }
    else if(alarm_list[current_shedule].action==SCHD_ACTION_OFF){
    m_recall=1;
    GetDlgItem(IDC_EDIT_RECALL)->EnableWindow(FALSE);
    m_radio_on=1;
    }
    else if(alarm_list[current_shedule].action==SCHD_ACTION_NONE){
    m_recall=1;
    GetDlgItem(IDC_EDIT_RECALL)->EnableWindow(FALSE);
    m_radio_on=2;
    }
    else if(alarm_list[current_shedule].action==SCHD_ACTION_SCENE){
    GetDlgItem(IDC_EDIT_RECALL)->EnableWindow(TRUE);
    m_radio_on=3;
    m_recall=alarm_list[current_shedule].scene_id;
    }
    else{
    m_recall=1;
    GetDlgItem(IDC_EDIT_RECALL)->EnableWindow(FALSE);
    m_radio_on=0;
    }
    setcheckmonth();
    setcheckweek();
    
    UpdateData(FALSE);
}

void CTLMeshDlg::OnGridSheduleClick(NMHDR *pNotifyStruct, LRESULT* pResult){
	if(0 != Sel_Ele_Check()){
		return;
	}

	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
    //Trace(_T("Clicked on row %d, col %d\n"), pItem->iRow, pItem->iColumn);
	if(((pItem->iRow)>0)&&((pItem->iRow)<17) ){
        ShowSchedule((pItem->iRow)-1);
    }
}
void CTLMeshDlg::OnGridClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
    //Trace(_T("Clicked on row %d, col %d\n"), pItem->iRow, pItem->iColumn);
	if (pItem->iRow >= mesh_num)
		return;

	u16 adr_dst = mesh_status[pItem->iRow].adr;

	if ((pItem->iColumn == 2)||(pItem->iColumn == 3))			// light on / off
	{
	//	LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "access cmd on off ");
		access_cmd_onoff (adr_dst, 1, (pItem->iColumn == 2), (RSP_TYPE_RELIABLE == m_reliable), 0);
	}
}

// NM_RCLICK
void CTLMeshDlg::OnGridRClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
    //Trace(_T("Right button click on row %d, col %d\n"), pItem->iRow, pItem->iColumn);
}


void CTLMeshDlg::OnGridGroupDblClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
    //Trace(_T("Double Clicked on row %d, col %d\n"), pItem->iRow, pItem->iColumn);
	// add custom code here
	CString str = "";
	CString type, key, value;
	char stdbuff[] = "stdout.txt";

	type = m_Grid.GetItemText (pItem->iRow, 0);

}

//00 02 14 10 00 04 00 12 15 00 11 11 23 00 00 00 00 d9 11 02 01 03 80
void CTLMeshDlg::OnGridGroupClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
    //Trace(_T("Clicked on row %d, col %d\n"), pItem->iRow, pItem->iColumn);

	u16 adr_dst;
	u8 rsp_max;
	if(pItem->iRow == 0){
		adr_dst = 0xffff;
		rsp_max = GetOnlineStatusRspMax ();
	}else{
		adr_dst = (pItem->iRow - 1) | 0xc000;
		rsp_max = 0;
	}

	if ((pItem->iColumn == 1)||(pItem->iColumn == 2))				// light on / off
	{
		access_cmd_onoff (adr_dst, rsp_max, (pItem->iColumn == 1), (RSP_TYPE_RELIABLE == m_reliable), 0);
	}
	else if (pItem->iColumn == 3 || (pItem->iColumn == 4))			// Add group
	{
		if(0 != Sel_Ele_Check()){
			return;
		}

		u16 md_id = vc_get_sig_model_id_by_column(pItem->iColumn);
		cfg_cmd_sub_set(CFG_MODEL_SUB_ADD, NODE_ADR_AUTO, mesh_sel, adr_dst, md_id, 1);
	}
}

// NM_RCLICK
void CTLMeshDlg::OnGridGroupRClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
    NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
    //Trace(_T("Right button click on row %d, col %d\n"), pItem->iRow, pItem->iColumn);
	u16 adr_dst;
	if(pItem->iRow == 0){
		adr_dst = 0xffff;
	}else{
		adr_dst = (pItem->iRow - 1) | 0xc000;
	}
	
	if(0 != Sel_Ele_Check()){
		return;
	}

	if (pItem->iColumn == 3 || (pItem->iColumn == 4)){			// Delete group
		u16 md_id = vc_get_sig_model_id_by_column(pItem->iColumn);
		cfg_cmd_sub_set(CFG_MODEL_SUB_DEL, NODE_ADR_AUTO, mesh_sel, adr_dst, md_id, 1);
	}
}

void CTLMeshDlg::update_mesh_sts()
{
    if(!g_module_dlg->m_bLogEn){
        AfxMessageBox("Note: Log disable now");
    }
    
	if(RSP_TYPE_ONLINE_ST == m_reliable){
	    SendPktOnlineStOn();
        InitStatus ();
	}else{
        u32 rsp_max = mesh_num ? GetOnlineStatusRspMax () : 1;
    	u16 adr_tmp = 0xffff;
    	int err = access_cmd_get_lightness(adr_tmp,rsp_max);
        if(0 == err){
            InitOnlineStatusOffline(); // keep offline nodes
        }
	}
}
void CTLMeshDlg::OnNode() 
{
	// TODO: Add your control notification handler code here
	update_mesh_sts();
}

int CTLMeshDlg::OnButtonGetSubscription(int model_id) 
{
	// TODO: Add your control notification handler code here
	if(0 != Sel_Ele_Check()){
		return -1;
	}

	if(is_light_model_server(model_id, 1)){
		memset(group_status[0], 0, sizeof(group_status[0]));
	}else if(is_light_model_client(model_id, 1)){
		memset(group_status[1], 0, sizeof(group_status[0]));
	}
	
	UpdateGroupStatus ();

	cfg_cmd_sub_get(NODE_ADR_AUTO, mesh_sel, model_id);

	return 0;
}

void CTLMeshDlg::OnButtonGetgroup() 
{
	// TODO: Add your control notification handler code here
	OnButtonGetSubscription(SIG_MD_G_ONOFF_S);
}

void CTLMeshDlg::OnBnClickedGroup2()
{
	// TODO: 在此添加控件通知处理程序代码
	OnButtonGetSubscription(SIG_MD_G_ONOFF_C);
}

void CTLMeshDlg::OnBnClickedSetMac()
{
	// TODO: 在此添加控件通知处理程序代码
	
	u8 mac[6];
	u8 cmd_tmp[16];
	unsigned char tmp_str[128];
	UpdateData();
	sprintf_s ((char *)tmp_str, sizeof(tmp_str), "%s", (const char*)csSetMac);
	Text2Bin(mac, tmp_str);

	cmd_tmp[0]= HCI_CMD_BULK_SET_PAR & 0xFF;
	cmd_tmp[1]= (HCI_CMD_BULK_SET_PAR >> 8) & 0xFF;
	cmd_tmp[2]= PAR_TYPE_SET_MAC;
	memcpy(cmd_tmp+3,mac,6);
	WriteFile_handle(cmd_tmp, 9, 0, 0);	
}

void RefreshStatusNotifyByHw(unsigned char *p, int len)
{
	#if REFRESH_ST_BY_HW_EN || DEBUG_SHOW_VC_SELF_EN
    LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "Report by RefreshStatusNotifyByHw():");
	m_pMeshDlg->StatusNotify (p, len);
	#endif
}


//void CTLMeshDlg::OnBnClickedReliable()
//{
//	// TODO: 在此添加控件通知处理程序代码
//}

int CTLMeshDlg::Sel_Ele_Check()
{
	// TODO: 在此添加控件通知处理程序代码

	if (mesh_sel == 0xffff)				// all group
	{
		AfxMessageBox("Please select a node");
		return -1;
	}
	return 0;
}


void CTLMeshDlg::OnBnClickedReliable()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData ();
}

//-------------publish addr
void CTLMeshDlg::PubAddrUpdate(u16 adr)
{
	// TODO: 在此添加控件通知处理程序代码

	m_pub_addr.Format("%x", adr);
	SetDlgItemText(IDC_PUB_ADDR, m_pub_addr);
}

void CTLMeshDlg::OnBnClickedGetPub()
{
	// TODO: 在此添加控件通知处理程序代码
	if(0 != Sel_Ele_Check()){
		return ;
	}

	m_pub_addr.Format("%x", 0xffff);
	cfg_cmd_pub_get(NODE_ADR_AUTO, mesh_sel, SIG_MD_G_ONOFF_S, 1);
}

//-------------secutity network beacon set
void CTLMeshDlg::OnBnClickedGetSecNwBc()
{
	// TODO: 在此添加控件通知处理程序代码
	if(0 != Sel_Ele_Check()){
		return ;
	}
	
	m_pub_addr.Format("%x", 0xff);
	cfg_cmd_sec_nw_bc_get(mesh_sel);
}

void CTLMeshDlg::OnBnClickedGetttl()
{
	// TODO: 在此添加控件通知处理程序代码
	if(0 != Sel_Ele_Check()){
		return ;
	}
	
	m_pub_addr.Format("%x", 0xff);
	cfg_cmd_ttl_get(mesh_sel);
}

BOOL CTLMeshDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (WM_KEYDOWN ==pMsg->message && VK_RETURN == pMsg->wParam)
    {
	    if (GetFocus() == GetDlgItem(IDC_PUB_ADDR)) //根据不同控件焦点判断是那个在执行 
        {
			if(0 != Sel_Ele_Check()){
				return TRUE;
			}
            UpdateData(TRUE);
            u16 g_addr_value = (u16)strtol(m_pub_addr,NULL,16);
            mesh_model_pub_par_t pub_par = {0};
            pub_par.appkey_idx = mesh_key.net_key[0][0].app_key[0].index;
            pub_par.ttl = TTL_PUB_USE_DEFAULT;
            pub_par.period.steps = 0;
            pub_par.period.res = 0;
            pub_par.transmit.count = PUBLISH_RETRANSMIT_CNT;
            pub_par.transmit.invl_steps = PUBLISH_RETRANSMIT_INVL_STEPS;
            cfg_cmd_pub_set(NODE_ADR_AUTO, mesh_sel, g_addr_value, &pub_par, SIG_MD_G_ONOFF_S, 1);
			return TRUE;
        }
		else if(GetFocus() == GetDlgItem(IDC_SEC_NW_BC))
		{
			if(0 != Sel_Ele_Check()){
				return TRUE;
			}
			UpdateData(TRUE);
            u8 val = (u8)strtol(m_sec_nw_bc,NULL,16);
            cfg_cmd_sec_nw_bc_set(mesh_sel, val);
			return TRUE;
		}
		else if(GetFocus() == GetDlgItem(IDC_TTl_VAL))
		{
			if(0 != Sel_Ele_Check()){
				return TRUE;
			}
			UpdateData(TRUE);
            u8 val = (u8)strtol(m_ttl,NULL,16);
            cfg_cmd_ttl_set(mesh_sel, val);
			return TRUE;
		}
		else if(GetFocus() == GetDlgItem(IDC_Transimit))
		{
			if(0 != Sel_Ele_Check()){
				return TRUE;
			}
			UpdateData(TRUE);
            u8 val = (u8)strtol(m_transimit,NULL,16);
            mesh_transmit_t *p_tran = (mesh_transmit_t *)&val;
            cfg_cmd_nw_transmit_set(mesh_sel, p_tran->count+1, p_tran->invl_steps+1);
			return TRUE;
		}
		else if(GetFocus() == GetDlgItem(IDC_RELAY_VAL))
		{
			if(0 != Sel_Ele_Check()){
				return TRUE;
			}
			UpdateData(TRUE);
			
			#if TEST_SCHEDULER_CMD_EN
            u32 val = (u32)strtol(m_relay_val,NULL,10);
            time_status_t time_set = {0};
            time_set.TAI_sec = val;
            time_set.zone_offset = TIME_ZONE_0;
            mesh_time_set(&time_set);
			#else
            u8 val = (u8)strtol(m_relay_val,NULL,16);
			mesh_cfg_model_relay_set_t relay_set;
			relay_set.relay = val;
			relay_set.transmit = model_sig_cfg_s.relay_retransmit;
            cfg_cmd_relay_set(mesh_sel, relay_set);
			#endif
			
			return TRUE;
		}
		else if(GetFocus() == GetDlgItem(IDC_FRIEND_VAL))
		{
			if(0 != Sel_Ele_Check()){
				return TRUE;
			}
			UpdateData(TRUE);
            u8 val = (u8)strtol(m_friend_val,NULL,16);
            cfg_cmd_friend_set(mesh_sel, val);
			return TRUE;
		}
		else if(GetFocus() == GetDlgItem(IDC_PROXY_VAL))
		{
			if(0 != Sel_Ele_Check()){
				return TRUE;
			}
			UpdateData(TRUE);
            u8 val = (u8)strtol(m_proxy_val,NULL,16);
            cfg_cmd_proxy_set(mesh_sel, val);
			return TRUE;
		}
		else if(GetFocus() == GetDlgItem(IDC_LEVEL_VAL))
		{
			if(0 != Sel_Ele_Check()){
				return TRUE;
			}
			UpdateData(TRUE);
            u8 g_addr_value = (u8)strtol(m_light_val,NULL,16);
            if((g_addr_value < 0) || (g_addr_value > 0x64)){
				AfxMessageBox("the level value should only be 0x00~0x64");
            }
            access_set_lum(mesh_sel,1,g_addr_value,1);
			return TRUE;
		}
		else if(GetFocus() == GetDlgItem(IDC_CT_VAL))
		{
			if(0 != Sel_Ele_Check()){
				return TRUE;
			}
			UpdateData(TRUE);
            u8 g_ct_value = (u8)strtol(m_ct_val,NULL,16);
			if((g_ct_value < 0) || (g_ct_value > 0x64)){
				AfxMessageBox("the level value should only be 0x00~0x64");
            }
			#if (LIGHT_TYPE_CT_EN)
			u8 offset=0;
			if(ble_moudle_id_is_gateway()){
				offset =1;
			}else{
				offset = get_ele_offset_by_model_VC_node_info(mesh_sel, SIG_MD_LIGHT_CTL_TEMP_S, 1);
			} 
            if(offset != MODEL_NOT_FOUND){
			    access_cmd_set_light_ctl_temp_100(mesh_sel + offset, 1, g_ct_value, 1);
            }
			#endif
			return TRUE;
		}
		else if(GetFocus() == GetDlgItem(IDC_PUB_ADDR10))
		{
			if(0 != Sel_Ele_Check()){
				return TRUE;
			}
			UpdateData(TRUE);
            //u16 g_addr_value = (u16)strtol(m_pub_addr,NULL,16);
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CTLMeshDlg::OnBnClickedGetTransimit()
{
	// TODO: 在此添加控件通知处理程序代码
	if(0 != Sel_Ele_Check()){
		return ;
	}

    #if TEST_SCHEDULER_CMD_EN
    test_schd_action_set_off(mesh_sel);
    #else
	m_transimit.Format("%x", 0xff);
	cfg_cmd_nw_transmit_get(mesh_sel);
	#endif
}

#if TEST_RELAY_BUFF_EN
rf_packet_adv_t T_relay_1;
u32 T_relay_2, T_relay_3,T_relay_4;

void relay_push_test()
{
    mesh_cmd_bear_t bear_relay = {{0}};
    bear_relay.trans_par_val = 0x25;
    bear_relay.len = 16;
    foreach(i,2){
        bear_relay.beacon.type = ++T_relay_4;
        my_fifo_push_relay(&bear_relay, mesh_bear_len_get(&bear_relay), 0);
    }
	
    #if 1
    bear_relay.trans_par_val = 0x13;
    foreach(i,2){
        bear_relay.beacon.type = ++T_relay_4;
        my_fifo_push_relay(&bear_relay, mesh_bear_len_get(&bear_relay), 0);
    }
    #endif
}

void relay_poll_10ms_test()
{
    static u32 tick_relay;
    if(clock_time_exceed(tick_relay, 10*1000)){
        tick_relay = clock_time();
        int ret = relay_adv_prepare_handler(&T_relay_1, 0);
        if(ret){
            T_relay_2++;
        }else{
            T_relay_3++;
        }
    }
}
#endif

void CTLMeshDlg::OnBnClickedGetRelay()
{
	// TODO: 在此添加控件通知处理程序代码
	if(0 != Sel_Ele_Check()){
		return ;
	}

    #if TEST_SCHEDULER_CMD_EN
    test_schd_action_set_on(mesh_sel);
    #elif TEST_RELAY_BUFF_EN
	relay_push_test();
    #else
	m_relay_val.Format("%x", 0xff);
	cfg_cmd_relay_get(mesh_sel);
	#endif
}

void CTLMeshDlg::OnBnClickedGetFriend()
{
	// TODO: 在此添加控件通知处理程序代码
	if(0 != Sel_Ele_Check()){
		return ;
	}
	
	m_friend_val.Format("%x", 0xff);
	cfg_cmd_friend_get(mesh_sel);
}

void CTLMeshDlg::OnBnClickedGetProxy()
{
	// TODO: 在此添加控件通知处理程序代码
	if(0 != Sel_Ele_Check()){
		return ;
	}
	
	m_proxy_val.Format("%x", 0xff);
	cfg_cmd_proxy_get(mesh_sel);
}

void CTLMeshDlg::OnBnClickedGetCps()
{
	// TODO: 在此添加控件通知处理程序代码
	if(0 != Sel_Ele_Check()){
		return ;
	}

    #if TEST_SCHEDULER_CMD_EN
    test_schd_action_set_scene(mesh_sel);
    #else
	cfg_cmd_cps_get(mesh_sel, 0);
	#endif
}

void CTLMeshDlg::OnBnClickedDelNode()
{
	// TODO: 在此添加控件通知处理程序代码
	#if 0
	mesh_kr_cfgcl_start(ele_adr_primary);
	return ;
	#endif
	
	if(0 != Sel_Ele_Check()){
		return ;
	}

    #if TEST_SCHEDULER_CMD_EN
    CTime time2(2000,1,1,8,0,1);    // should be later than 2000/1/1 8:00:00  for east 8 zone
    u32 nTSeconds2 = (u32)time2.GetTime() - OFFSET_1970_2000;
    TIME_ZONE_INFORMATION tz;
    u32 dwRet = GetTimeZoneInformation(&tz);
    #if 0
    if(dwRet == TIME_ZONE_ID_STANDARD ||
        dwRet == TIME_ZONE_ID_UNKNOWN)    
        wprintf(L"%s\n", tz.StandardName);
    else if( dwRet == TIME_ZONE_ID_DAYLIGHT )
        wprintf(L"%s\n", tz.DaylightName);
    else
    {
        printf("GTZI failed (%d)\n", GetLastError());
        return ;
    }
    #endif
   
    time_status_t time_set = {0};
    time_set.TAI_sec = nTSeconds2;
    time_set.zone_offset = get_time_zone_offset(-tz.Bias);	// 96 means east 8 zone
    access_cmd_time_set(mesh_sel, 0, &time_set);
    #else
	cfg_cmd_reset_node(mesh_sel);
	if(ble_moudle_id_is_gateway()){
		extern void json_use_adr_to_del_info(u16 adr);
		json_use_adr_to_del_info(mesh_sel);
	}
	// clear selected node info.
	if (mesh_num > 0) {
		for(int i=0;i<mesh_num-1;i++) {
			if (mesh_status[i].adr == mesh_sel) {
				memcpy(&mesh_status[i], &mesh_status[i+1], (mesh_num-1-i)*sizeof(mesh_status_t));
				break;
			}
		}
		mesh_num --;
		mesh_sel = 0xFFFF;
		UpdateOnlineStatus();
	}

	#endif
}

void CTLMeshDlg::OnBnClickedGrpdelallS()
{
	// TODO: 在此添加控件通知处理程序代码
	if(0 != Sel_Ele_Check()){
		return ;
	}
	
	cfg_cmd_sub_set(CFG_MODEL_SUB_DEL_ALL, NODE_ADR_AUTO, mesh_sel, 0, SIG_MD_G_ONOFF_S, 1);
}

void CTLMeshDlg::OnBnClickedGrpdelallC()
{
	// TODO: 在此添加控件通知处理程序代码
	if(0 != Sel_Ele_Check()){
		return ;
	}
	
	cfg_cmd_sub_set(CFG_MODEL_SUB_DEL_ALL, NODE_ADR_AUTO, mesh_sel, 0, SIG_MD_G_ONOFF_C, 1);
}



void CTLMeshDlg::OnBnClickedGetLevel()
{
	// TODO: 在此添加控件通知处理程序代码
	if(0 != Sel_Ele_Check()){
		return ;
	}
	
	m_proxy_val.Format("%x", 0xff);
	
	access_get_lum(mesh_sel,1);
	
}


void CTLMeshDlg::OnBnClickedGetCt()
{
	// TODO: 在此添加控件通知处理程序代码
	if(0 != Sel_Ele_Check()){
		return ;
	}
	
	m_proxy_val.Format("%x", 0xff);
	
	#if (LIGHT_TYPE_CT_EN)
    u8 offset = get_ele_offset_by_model_VC_node_info(mesh_sel, SIG_MD_LIGHT_CTL_TEMP_S, 1);
    if(offset != MODEL_NOT_FOUND){
        access_cmd_get_light_ctl_temp(mesh_sel + offset, 1);
    }
	#endif
}


void CTLMeshDlg::OnClickedAnyYear()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	switch (m_yearsel)
	{
	case 0:
		GetDlgItem(IDC_YEAR)->EnableWindow(FALSE);
		break;
	case 1:
		GetDlgItem(IDC_YEAR)->EnableWindow(TRUE);
		break;
	default:
		break;
	}
}


void CTLMeshDlg::OnClickedAnyDay()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	switch (m_daysel)
	{
	case 0:
		GetDlgItem(IDC_EDIT_DAY)->EnableWindow(FALSE);
		break;
	case 1:
		GetDlgItem(IDC_EDIT_DAY)->EnableWindow(TRUE);
		break;
	default:
		break;
	}
}


void CTLMeshDlg::OnClickedAnyHour()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	switch (m_hoursel)
	{
	case 0:
	case 1:
		GetDlgItem(IDC_HOUR)->EnableWindow(FALSE);
		break;
	case 2:
		GetDlgItem(IDC_HOUR)->EnableWindow(TRUE);
		break;
	default:
		break;
	}
}


void CTLMeshDlg::OnClickedAnyMinute()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	switch (m_minutesel)
	{
	case 0:
	case 1:
	case 2:
	case 3:
		GetDlgItem(IDC_MINUTE)->EnableWindow(FALSE);
		break;
	case 4:
		GetDlgItem(IDC_MINUTE)->EnableWindow(TRUE);
		break;
	default:
		break;
	}
}


void CTLMeshDlg::OnClickedAnySecond()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	switch (m_secondsel)
	{
	case 0:
	case 1:
	case 2:
	case 3:
		GetDlgItem(IDC_SECOND)->EnableWindow(FALSE);
		break;
	case 4:
		GetDlgItem(IDC_SECOND)->EnableWindow(TRUE);
		break;
	default:
		break;
	}
}


void CTLMeshDlg::OnClickedOn()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	switch (m_radio_on)
	{
	case 0:
	case 1:
	case 2:
		GetDlgItem(IDC_EDIT_RECALL)->EnableWindow(FALSE);
		break;
	case 3:
		GetDlgItem(IDC_EDIT_RECALL)->EnableWindow(TRUE);
		break;
	default:
		break;
	}
}

scheduler_t CTLMeshDlg::get_current_scheduler()
{
	scheduler_t return_scheduler={0};
	UpdateData(TRUE);
	if(m_yearsel==0){	
    return_scheduler.year=SCHD_YEAR_ANY;
	}
	else{	
	return_scheduler.year=m_year;
	}
    if(m_daysel==0){	
	return_scheduler.day=SCHD_DAY_ANY;
	}
	else{	
	return_scheduler.day=m_day;
	}
	if(m_hoursel==0){	
    return_scheduler.hour=SCHD_HOUR_ANY ;
	}
	else if(m_hoursel==1){
	return_scheduler.hour=SCHD_HOUR_RANDOM  ;
	}
	else if(m_hoursel==2){
	return_scheduler.hour=m_hour ;	
	}
    if(m_minutesel==0){	
	return_scheduler.minute=SCHD_MIN_ANY;
	}
	else if(m_minutesel==1){
	return_scheduler.minute=SCHD_MIN_EVERY15;
	}
	else if(m_minutesel==2){
	return_scheduler.minute=SCHD_MIN_EVERY20;
	}
	else if(m_minutesel==3){	
	return_scheduler.minute=SCHD_MIN_RANDOM ;
	}
	else if(m_minutesel==4){	
	return_scheduler.minute=m_minute;
	}
	if(m_secondsel==0){	
	return_scheduler.second=SCHD_SEC_ANY;
	}
	else if(m_secondsel==1){
	return_scheduler.second=SCHD_SEC_EVERY15 ;
	}
	else if(m_secondsel==2){
	return_scheduler.second=SCHD_SEC_EVERY20 ;
	}
    else if(m_secondsel==3){
	return_scheduler.second=SCHD_SEC_RANDOM;
	}
    else if(m_secondsel==4){
	return_scheduler.second=m_seconed;
	}
	if(m_radio_on==0){	
	return_scheduler.action=SCHD_ACTION_ON;
	}
	else if(m_radio_on==1){	
	return_scheduler.action=SCHD_ACTION_OFF ;
	}
	else if(m_radio_on==2){	
	return_scheduler.action=SCHD_ACTION_NONE;
	}
	else if(m_radio_on==3){	
	return_scheduler.action=SCHD_ACTION_SCENE ;
	return_scheduler.scene_id=m_recall;
	}
	if(m_January){
	return_scheduler.month|=0x0001;
	}
	if(m_February){
	return_scheduler.month|=(0x0001<<1);
	}
	if(m_March){
	return_scheduler.month|=(0x0001<<2);
	}
	if(m_April){
	return_scheduler.month|=(0x0001<<3);
	}
	if(m_May){
	return_scheduler.month|=(0x0001<<4);
	}
	if(m_June){
	return_scheduler.month|=(0x0001<<5);
	}
	if(m_July){
	return_scheduler.month|=(0x0001<<6);
	}
	if(m_August){
	return_scheduler.month|=(0x0001<<7);
	}
	if(m_September){
	return_scheduler.month|=(0x0001<<8);
	}
	if(m_October){
	return_scheduler.month|=(0x0001<<9);
	}
	if(m_November){
	return_scheduler.month|=(0x0001<<10);
	}
	if(m_December){
	return_scheduler.month|=(0x0001<<11);
	}
	
	if(m_monday){
	return_scheduler.week|=0x01;
	}
	if(m_tuesday){
	return_scheduler.week|=(0x01<<1);
	}
	if(m_wednesday){
	return_scheduler.week|=(0x01<<2);
	}
	if(m_thursday){
	return_scheduler.week|=(0x01<<3);
	}
	if(m_friday){
	return_scheduler.week|=(0x01<<4);
	}
	if(m_saturday){
	return_scheduler.week|=(0x01<<5);
	}
	if(m_sunday){
	return_scheduler.week|=(0x01<<6);
	}
	UpdateData(FALSE);
	return return_scheduler;
}
void CTLMeshDlg::OnBnClickedAdd()
{
	// TODO: 在此添加控件通知处理程序代码
	if(0 != Sel_Ele_Check()){
		return ;
	}
	scheduler_t c_schd = {0};
	c_schd=get_current_scheduler();	
    c_schd.valid_flag_or_idx=current_shedule;
    if(is_support_model_dst(mesh_sel,SIG_MD_SCHED_S,1)){
        access_cmd_schd_action_set(mesh_sel, 0, &c_schd, 1);
    }else{
        LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "Node not support scheduler model",0);
    }
}

void CTLMeshDlg::setcheckweek()
{
	
	if(m_monday){
	((CButton *)GetDlgItem(IDC_MONDAY))->SetCheck(1);
	}
	else{
	((CButton *)GetDlgItem(IDC_MONDAY))->SetCheck(0);
	}
	if(m_tuesday){
	((CButton *)GetDlgItem(IDC_TUESDAY))->SetCheck(1);
	}
	else{
	((CButton *)GetDlgItem(IDC_TUESDAY))->SetCheck(0);
	}
	if(m_wednesday){
	((CButton *)GetDlgItem(IDC_WEDNESDAY))->SetCheck(1);
	}
	else{
	((CButton *)GetDlgItem(IDC_WEDNESDAY))->SetCheck(0);
	}
	if(m_thursday){
	((CButton *)GetDlgItem(IDC_THURSDAY))->SetCheck(1);
	}
	else{
	((CButton *)GetDlgItem(IDC_THURSDAY))->SetCheck(0);
	}
	if(m_friday){
	((CButton *)GetDlgItem(IDC_FRIDAY))->SetCheck(1);
	}
	else{
	((CButton *)GetDlgItem(IDC_FRIDAY))->SetCheck(0);
	}
	if(m_saturday){
	((CButton *)GetDlgItem(IDC_SATURDAY))->SetCheck(1);
	}
	else{
	((CButton *)GetDlgItem(IDC_SATURDAY))->SetCheck(0);
	}
	if(m_sunday){
	((CButton *)GetDlgItem(IDC_SUNDAY))->SetCheck(1);
	}
	else{
	((CButton *)GetDlgItem(IDC_SUNDAY))->SetCheck(0);
	}

}
void CTLMeshDlg::setcheckmonth()
{
	
	if(m_January){
	((CButton *)GetDlgItem(IDC_JAN))->SetCheck(1);
	}
	else{
	((CButton *)GetDlgItem(IDC_JAN))->SetCheck(0);
	}
	if(m_February){
	((CButton *)GetDlgItem(IDC_FEB))->SetCheck(1);
	}
	else{
	((CButton *)GetDlgItem(IDC_FEB))->SetCheck(0);
	}	
	if(m_March){
	((CButton *)GetDlgItem(IDC_MAR))->SetCheck(1);
	}
	else{
	((CButton *)GetDlgItem(IDC_MAR))->SetCheck(0);
	}	
	if(m_April){
	((CButton *)GetDlgItem(IDC_APR))->SetCheck(1);
	}
	else{
	((CButton *)GetDlgItem(IDC_APR))->SetCheck(0);
	}
	if(m_May){
	((CButton *)GetDlgItem(IDC_MAY))->SetCheck(1);
	}
	else{
	((CButton *)GetDlgItem(IDC_MAY))->SetCheck(0);
	}
	if(m_June){
	((CButton *)GetDlgItem(IDC_JUN))->SetCheck(1);
	}
	else{
	((CButton *)GetDlgItem(IDC_JUN))->SetCheck(0);
	}
	if(m_July){
	((CButton *)GetDlgItem(IDC_JUL))->SetCheck(1);
	}
	else{
	((CButton *)GetDlgItem(IDC_JUL))->SetCheck(0);
	}
	if(m_August){
	((CButton *)GetDlgItem(IDC_AUG))->SetCheck(1);
	}
	else{
	((CButton *)GetDlgItem(IDC_AUG))->SetCheck(0);
	}	
	if(m_September){
	((CButton *)GetDlgItem(IDC_SEP))->SetCheck(1);
	}
	else{
	((CButton *)GetDlgItem(IDC_SEP))->SetCheck(0);
	}	
	if(m_October){
	((CButton *)GetDlgItem(IDC_OCT))->SetCheck(1);
	}
	else{
	((CButton *)GetDlgItem(IDC_OCT))->SetCheck(0);
	}
	if(m_November){
	((CButton *)GetDlgItem(IDC_NOV))->SetCheck(1);
	}
	else{
	((CButton *)GetDlgItem(IDC_NOV))->SetCheck(0);
	}
	if(m_December){
	((CButton *)GetDlgItem(IDC_DEC))->SetCheck(1);
	}
	else{
	((CButton *)GetDlgItem(IDC_DEC))->SetCheck(0);
	}


}
void CTLMeshDlg::OnBnClickedSceneDel()
{
	// TODO: 在此添加控件通知处理程序代码
		if(0 != Sel_Ele_Check()){
		return ;
	}
	access_cmd_scene_del(mesh_sel, 0, current_scene, 1);
}


void CTLMeshDlg::OnBnClickedSceneRecall()
{
	// TODO: 在此添加控件通知处理程序代码
		if(0 != Sel_Ele_Check()){
		return ;
	}
	transition_par_t temp_transtion={0};
	access_cmd_scene_recall(ADR_ALL_NODES, 0, current_scene,1, &temp_transtion);
}


void CTLMeshDlg::OnBnClickedSceneAdd()
{
	// TODO: 在此添加控件通知处理程序代码
		if(0 != Sel_Ele_Check()){
		return ;
	}
	UpdateData();
	if((m_scene_number<=0xffff)&&(m_scene_number>0)){
        if(is_support_model_dst(mesh_sel,SIG_MD_SCENE_S,1)){
            access_cmd_scene_store(mesh_sel, 0, m_scene_number, 1);
        }else{
            LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "Node not support scene model",0);
        }
	}
}


void CTLMeshDlg::OnBnClickedSetTime()
{
	// TODO: 在此添加控件通知处理程序代码
	if(0 != Sel_Ele_Check()){
		return ;
	}
	time_status_t settime={0};
	//mesh_UTC_t set_utc;
	CTime time = CTime::GetCurrentTime(); 
	/*set_utc.year=time.GetYear();
	set_utc.month=time.GetMonth();;
	set_utc.day=time.GetDay();
	set_utc.hour=time.GetHour();
	set_utc.minute=time.GetMinute();
	set_utc.second=time.GetSecond();
	set_utc.week=time.GetDayOfWeek();
	settime.TAI_sec=get_TAI_sec(&set_utc);
	*/
	u32 nTSeconds2 = (u32)time.GetTime();
	settime.TAI_sec=nTSeconds2-OFFSET_1970_2000;
	TIME_ZONE_INFORMATION tz;
	u32 dwRet = GetTimeZoneInformation(&tz);
    settime.zone_offset = get_time_zone_offset(-tz.Bias);
    if(is_support_model_dst(mesh_sel,SIG_MD_TIME_S,1)){
        access_cmd_time_set(mesh_sel, 0, &settime);
    }else{
        LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "Node not support time model",0);
    }
}


void CTLMeshDlg::OnBnClickedGetTime()
{
	// TODO: 在此添加控件通知处理程序代码
	if(0 != Sel_Ele_Check()){
		return ;
	}

    if(is_support_model_dst(mesh_sel,SIG_MD_TIME_S,1)){
        access_cmd_time_get(mesh_sel,1);
    }else{
        LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "Node not support time model",0);
    }
}

void CTLMeshDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	switch (nIDEvent){
	case 1:
		{
            KillTimer(1);

            SetTimer(1,200,NULL);
		}
		break;

	default:
		break;	
	}
	CDialog::OnTimer(nIDEvent);
}


void CTLMeshDlg::OnEnKillfocusYear()
{
	// TODO: 在此添加控件通知处理程序代码
	int aa = m_year;
    UpdateData(true);
    if (m_year>99)
    {
        MessageBoxA(_T("The range of years is 0 to 99!"), _T("错误"), MB_OK);
        m_year = aa;
        UpdateData(false);
    }
}


void CTLMeshDlg::OnEnKillfocusEditDay()
{
	// TODO: 在此添加控件通知处理程序代码
		int aa = m_day;
    UpdateData(true);
    if ((m_day<1) || (m_day>31))
    {
        MessageBoxA(_T("The range of days is 1 to 31!"), _T("错误"), MB_OK);
        m_day = aa;
        UpdateData(false);
    }
}


void CTLMeshDlg::OnEnKillfocusHour()
{
	// TODO: 在此添加控件通知处理程序代码
	int aa = m_hour;
    UpdateData(true);
    if (m_hour>23)
    {
        MessageBoxA(_T("The range of hours is 0 to 23!"), _T("错误"), MB_OK);
        m_hour = aa;
        UpdateData(false);
    }
}


void CTLMeshDlg::OnEnKillfocusMinute()
{
	// TODO: 在此添加控件通知处理程序代码
		int aa = m_minute;
    UpdateData(true);
    if (m_minute>59)
    {
        MessageBoxA(_T("The range of minutes is 0 to 59!"), _T("错误"), MB_OK);
        m_minute = aa;
        UpdateData(false);
    }
}


void CTLMeshDlg::OnEnKillfocusSecond()
{
	// TODO: 在此添加控件通知处理程序代码
		int aa = m_seconed;
    UpdateData(true);
    if (m_seconed>59)
    {
        MessageBoxA(_T("The range of seconds is 0 to 59!"), _T("错误"), MB_OK);
        m_seconed = aa;
        UpdateData(false);
    }
}


void CTLMeshDlg::OnEnKillfocusEditRecall()
{
	// TODO: 在此添加控件通知处理程序代码
		int aa = m_recall;
    UpdateData(true);
    if ((!m_recall)||(m_recall>65535))
    {
        MessageBoxA(_T("The range of scenes is 1 to 65535!"), _T("错误"), MB_OK);
        m_recall = aa;
        UpdateData(false);
    }
}


void CTLMeshDlg::OnEnKillfocusscenenumber()
{
	// TODO: 在此添加控件通知处理程序代码
	int aa = m_scene_number;
    UpdateData(true);
    if ((!m_scene_number)||(m_scene_number>65535))
    {
        MessageBoxA(_T("The range of scene_numbers is 1 to 65535!"), _T("错误"), MB_OK);
        m_scene_number = aa;
        UpdateData(false);
    }
}

void CTLMeshDlg::OnSelchangeOnlineSel()
{
	// TODO: 在此添加控件通知处理程序代码
	m_reliable = m_online_box.GetCurSel();
	if(RSP_TYPE_ONLINE_ST == m_reliable){
	    SendPktOnlineStOn();
	}
}


void CTLMeshDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	ShowWindow(SW_HIDE);

	//CDialog::OnClose();
}

