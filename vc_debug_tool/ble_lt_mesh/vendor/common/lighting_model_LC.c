/********************************************************************************************************
 * @file	lighting_model_LC.c
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
#ifndef WIN32
#include "proj/mcu/watchdog_i.h"
#endif 
#include "proj_lib/ble/ll/ll.h"
#include "proj_lib/ble/blt_config.h"
#include "vendor/common/user_config.h"
#include "app_health.h"
#include "proj_lib/sig_mesh/app_mesh.h"
#include "lighting_model_LC.h"
#include "lighting_model.h"
#include "sensors_model.h"
#include "light.h"


#if (MD_LIGHT_CONTROL_EN)
model_light_lc_t       model_sig_light_lc;
u32 mesh_md_light_lc_addr = FLASH_ADR_MD_LIGHT_LC;
u16 prop_publish_id_sel = LC_PROP_ID_LightnessOn;

#if MD_SERVER_EN
#if !TESTCASE_FLAG_ENABLE
// STATIC_ASSERT(MD_SENSOR_CLIENT_EN); // light control server model can also process sensor status, so no need to enable sensor client model.
#endif
STATIC_ASSERT(MD_SENSOR_EN == 0); // due to callback function of sensor status will be conflic in mesh_cmd_sig_func_.

#define GET_LC_PROP_LEN(type)   \
    ((LC_PROP_TYPE_LUXLEVEL == type) ? LEN_LC_PROP_LUXLEVEL :\
       ((LC_PROP_TYPE_LIGHTNESS == type) ? LEN_LC_PROP_LIGHTNESS :\
          ((LC_PROP_TYPE_ACCURACY == type) ? LEN_LC_PROP_ACCURACY :\
            ((LC_PROP_TYPE_REGULATOR == type) ? LEN_LC_PROP_REGULATOR :\
              ((LC_PROP_TYPE_TIME == type) ? LEN_LC_PROP_TIME :0)))))

#define INIT_LC_PROP_ARRAY(id,member,type,val)    \
{GET_LC_PROP_LEN(type),id,&model_sig_light_lc.propty[0].member,sizeof(model_sig_light_lc.propty[0].member),type,val}

const lc_prop_info_t lc_prop_info[] = {
    INIT_LC_PROP_ARRAY(LC_PROP_ID_LuxLevelOn,LuxLevelOn,LC_PROP_TYPE_LUXLEVEL,LC_PROP_VAL_LuxLevelOn),
    INIT_LC_PROP_ARRAY(LC_PROP_ID_LuxLevelProlong,LuxLevelProlong,LC_PROP_TYPE_LUXLEVEL,LC_PROP_VAL_LuxLevelProlong),
    INIT_LC_PROP_ARRAY(LC_PROP_ID_LuxLevelStandby,LuxLevelStandby,LC_PROP_TYPE_LUXLEVEL,LC_PROP_VAL_LuxLevelStandby),
    INIT_LC_PROP_ARRAY(LC_PROP_ID_LightnessOn,LightnessOn,LC_PROP_TYPE_LIGHTNESS,LC_PROP_VAL_LightnessOn),
    INIT_LC_PROP_ARRAY(LC_PROP_ID_LightnessProlong,LightnessProlong,LC_PROP_TYPE_LIGHTNESS,LC_PROP_VAL_LightnessProlong),
    INIT_LC_PROP_ARRAY(LC_PROP_ID_LightnessStandby,LightnessStandby,LC_PROP_TYPE_LIGHTNESS,LC_PROP_VAL_LightnessStandby),
    INIT_LC_PROP_ARRAY(LC_PROP_ID_RegulatorAccuracy,RegAccuracy,LC_PROP_TYPE_ACCURACY,LC_PROP_VAL_RegulatorAccuracy),
    INIT_LC_PROP_ARRAY(LC_PROP_ID_RegulatorKid,RegKid,LC_PROP_TYPE_REGULATOR,LC_PROP_VAL_RegulatorKid),
    INIT_LC_PROP_ARRAY(LC_PROP_ID_RegulatorKiu,RegKiu,LC_PROP_TYPE_REGULATOR,LC_PROP_VAL_RegulatorKiu),
    INIT_LC_PROP_ARRAY(LC_PROP_ID_RegulatorKpd,RegKpd,LC_PROP_TYPE_REGULATOR,LC_PROP_VAL_RegulatorKpd),
    INIT_LC_PROP_ARRAY(LC_PROP_ID_RegulatorKpu,RegKpu,LC_PROP_TYPE_REGULATOR,LC_PROP_VAL_RegulatorKpu),
    INIT_LC_PROP_ARRAY(LC_PROP_ID_TimeOccupancyDelay,TimeOccupancyDelay,LC_PROP_TYPE_TIME,LC_PROP_VAL_TimeOccupancyDelay),
    INIT_LC_PROP_ARRAY(LC_PROP_ID_TimeFadeOn,TimeFadeOn,LC_PROP_TYPE_TIME,LC_PROP_VAL_TimeFadeOn),
    INIT_LC_PROP_ARRAY(LC_PROP_ID_TimeRun,TimeRun,LC_PROP_TYPE_TIME,LC_PROP_VAL_TimeRun),
    INIT_LC_PROP_ARRAY(LC_PROP_ID_TimeFade,TimeFade,LC_PROP_TYPE_TIME,LC_PROP_VAL_TimeFade),
    INIT_LC_PROP_ARRAY(LC_PROP_ID_TimeProlong,TimeProlong,LC_PROP_TYPE_TIME,LC_PROP_VAL_TimeProlong),
    INIT_LC_PROP_ARRAY(LC_PROP_ID_TimeFadeStandbyAuto,TimeStandbyAuto,LC_PROP_TYPE_TIME,LC_PROP_VAL_TimeFadeStandbyAuto),
    INIT_LC_PROP_ARRAY(LC_PROP_ID_TimeFadeStandbyManual,TimeStandbyManual,LC_PROP_TYPE_TIME,LC_PROP_VAL_TimeFadeStandbyManual),
};

typedef struct{
    u32 tick_ms; 		// tick for light control state
    u32 tick_ms_sensor;	// tick for sensor event.
    u8 occupancy_on;
    u8 st;
}lc_property_proc_t;

lc_property_proc_t g_lc_prop_proc[LIGHT_CNT];


static inline void * get_lc_val(const lc_prop_info_t *p_prop, int light_idx)
{
    return ((u8 *)p_prop->p_val + light_idx * p_prop->len_p_val);
}
static inline void set_lc_val(const lc_prop_info_t *p_prop, int light_idx, const void *val)
{
    memcpy(get_lc_val(p_prop, light_idx), val, p_prop->len - 2);
}

void light_LC_global_init()
{
    foreach(i,LIGHT_CNT){
        foreach_arr(k,lc_prop_info){
            const lc_prop_info_t *p_prop = &lc_prop_info[k];
            set_lc_val(p_prop, i, &p_prop->val_init);
        }
        model_sig_light_lc.om[i] = 1; // default as 1 by spec.
    }
}

const lc_prop_info_t * get_lc_prop_info(u16 id)    // get pointer
{
    foreach_arr(i,lc_prop_info){
        const lc_prop_info_t * p_prop = &lc_prop_info[i];
        if(p_prop->id == id){
            return p_prop;
        }
    }
    return 0;
}

int is_light_lc_onoff(u16 op)
{
    return (set_LC_lightness_flag || (LIGHT_LC_ONOFF_SET == op)||(LIGHT_LC_ONOFF_SET_NOACK == op));
}

/*
	lighting model command callback function ----------------
*/	
int mesh_tx_cmd_lc_mode_st(u8 idx, u16 ele_adr, u16 dst_adr, u8 *uuid, model_common_t *pub_md)
{
	u8 *par_rsp = &model_sig_light_lc.mode[idx];
	return mesh_tx_cmd_rsp(LIGHT_LC_MODE_STATUS, par_rsp, 1, ele_adr, dst_adr, uuid, pub_md);
}

int mesh_lc_mode_st_publish(u8 idx)
{
	model_common_t *p_com_md = &model_sig_light_lc.srv[idx].com;
	u16 ele_adr = p_com_md->ele_adr;
	u16 pub_adr = p_com_md->pub_adr;
	if(!pub_adr){
		return -1;
	}
	u8 *uuid = get_virtual_adr_uuid(pub_adr, p_com_md);
	return mesh_tx_cmd_lc_mode_st(idx, ele_adr, pub_adr, uuid, p_com_md);
}

void scene_get_lc_par(scene_data_t *p_scene, int light_idx)
{
    p_scene->lc_mode = model_sig_light_lc.mode[light_idx];
    p_scene->lc_onoff = model_sig_light_lc.lc_onoff_target[light_idx];
    p_scene->lc_om = model_sig_light_lc.om[light_idx];
    memcpy(&p_scene->lc_propty, &model_sig_light_lc.propty[light_idx], sizeof(p_scene->lc_propty));
}

void scene_load_lc_par(scene_data_t *p_scene, int light_idx)
{
    if(p_scene->lc_mode != model_sig_light_lc.mode[light_idx]){
        model_sig_light_lc.mode[light_idx] = p_scene->lc_mode;
        mesh_lc_mode_st_publish(light_idx);
    }

#if 0 // PTS_TEST_MMDL_SR_LLC_BV_10_C
	static u32 test_llc_BV10_cnt;
	test_llc_BV10_cnt++;
    if(6 == test_llc_BV10_cnt){ // auto standby state
    	model_sig_light_lc.lc_onoff_target[light_idx] = 0;
    }else
#endif
    {
    	model_sig_light_lc.lc_onoff_target[light_idx] = p_scene->lc_onoff;
    }
    model_sig_light_lc.om[light_idx] = p_scene->lc_om;
	mesh_model_store(1, SIG_MD_LIGHT_LC_S);

#if 1
    light_lc_property_t *p_prop = &model_sig_light_lc.propty[light_idx];
    if(memcmp(&p_scene->lc_propty, p_prop, sizeof(p_scene->lc_propty))){
        memcpy(p_prop, &p_scene->lc_propty, sizeof(p_scene->lc_propty));
        mesh_lc_prop_st_publish(light_idx);
    }
#endif

#if 0 // TODO LLC_BV_10
	// action
	transition_par_t transition_par;
	memset(&transition_par, 0, sizeof(transition_par));
	access_cmd_lc_onoff(ele_adr_primary, 0, p_scene->lc_onoff, 0, &transition_par);
#endif
}

int mesh_lc_mode_st_rsp(mesh_cb_fun_par_t *cb_par)
{
	model_g_light_s_t *p_model = (model_g_light_s_t *)cb_par->model;
	return mesh_tx_cmd_lc_mode_st(cb_par->model_idx, p_model->com.ele_adr, cb_par->adr_src, 0, 0);
}

int mesh_cmd_sig_lc_mode_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	return mesh_lc_mode_st_rsp(cb_par);
}

void lc_mode_set_par(int light_idx, u8 lc_mode)
{
	LC_proc_init(light_idx);
	model_sig_light_lc.mode[light_idx] = lc_mode;
	LC_state_enter(light_idx, 0, (LC_MODE_ON == lc_mode) ? LC_STATE_STANDBY : LC_STATE_OFF);
}

int mesh_cmd_sig_lc_mode_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err = -1;
    u8 mode_new = par[0];
    u8 *p_mode = &model_sig_light_lc.mode[cb_par->model_idx];
    if( mode_new < LC_MODE_MAX){
    	if(*p_mode != mode_new){
			lc_mode_set_par(cb_par->model_idx, mode_new);
			mesh_model_store(1, SIG_MD_LIGHT_LC_S);
            model_pub_check_set(ST_G_LEVEL_SET_PUB_NOW, cb_par->model, 1);
            model_pub_st_cb_re_init_lc_srv(&mesh_lc_mode_st_publish);    // re-init
		}
	
		if(cb_par->op_rsp != STATUS_NONE){
			err = mesh_lc_mode_st_rsp(cb_par);
		}else{
			err = 0;
		}
    }
    return err;
}

int mesh_tx_cmd_lc_om_st(u8 idx, u16 ele_adr, u16 dst_adr, u8 *uuid, model_common_t *pub_md)
{
	u8 *par_rsp = &model_sig_light_lc.om[idx];
	return mesh_tx_cmd_rsp(LIGHT_LC_OM_STATUS, par_rsp, 1, ele_adr, dst_adr, uuid, pub_md);
}

int mesh_lc_om_st_publish(u8 idx)
{
	model_common_t *p_com_md = &model_sig_light_lc.srv[idx].com;
	u16 ele_adr = p_com_md->ele_adr;
	u16 pub_adr = p_com_md->pub_adr;
	if(!pub_adr){
		return -1;
	}
	u8 *uuid = get_virtual_adr_uuid(pub_adr, p_com_md);
	return mesh_tx_cmd_lc_om_st(idx, ele_adr, pub_adr, uuid, p_com_md);
}

int mesh_lc_om_st_rsp(mesh_cb_fun_par_t *cb_par)
{
	model_g_light_s_t *p_model = (model_g_light_s_t *)cb_par->model;
	return mesh_tx_cmd_lc_om_st(cb_par->model_idx, p_model->com.ele_adr, cb_par->adr_src, 0, 0);
}

int mesh_cmd_sig_lc_om_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	return mesh_lc_om_st_rsp(cb_par);
}

int mesh_cmd_sig_lc_om_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err = -1;
    u8 om_new = par[0];
    u8 *p_om = &model_sig_light_lc.om[cb_par->model_idx];
    if( om_new < LC_OM_MAX){
    	if(*p_om != om_new){
			*p_om = om_new;
			mesh_model_store(1, SIG_MD_LIGHT_LC_S);
            model_pub_check_set(ST_G_LEVEL_SET_PUB_NOW, cb_par->model, 1);
            model_pub_st_cb_re_init_lc_srv(&mesh_lc_om_st_publish);    // re-init
		}
        //LC_property_tick_set(cb_par->model_idx);  // why set tick here ?
	
		if(cb_par->op_rsp != STATUS_NONE){
			err = mesh_lc_om_st_rsp(cb_par);
		}else{
			err = 0;
		}
    }
    return err;
}

int mesh_tx_cmd_lc_prop_st(u8 idx, u16 id_get, u16 ele_adr, u16 dst_adr, u8 *uuid, model_common_t *pub_md)
{
	lc_prop_head_t rsp_prop = {0};
	const lc_prop_info_t * p_info = get_lc_prop_info(id_get);
	if(p_info && (p_info->len <= LEN_LC_PROP_MAX)){
	    rsp_prop.len = p_info->len;
        rsp_prop.id = id_get;
	    memcpy(rsp_prop.val, get_lc_val(p_info, idx), p_info->len);
        return mesh_tx_cmd_rsp(LIGHT_LC_PROPERTY_STATUS, (u8 *)&rsp_prop.id, rsp_prop.len, ele_adr, dst_adr, uuid, pub_md);
	}

	return 0;
}

int mesh_lc_prop_st_publish(u8 idx)
{
	model_common_t *p_com_md = &model_sig_light_lc.setup[idx].com;
	u16 ele_adr = p_com_md->ele_adr;
	u16 pub_adr = p_com_md->pub_adr;
	if(!pub_adr){
		return -1;
	}
	u8 *uuid = get_virtual_adr_uuid(pub_adr, p_com_md);
	
	return mesh_tx_cmd_lc_prop_st(idx, prop_publish_id_sel, ele_adr, pub_adr, uuid, p_com_md);
}

int mesh_cmd_sig_lc_prop_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	u16 id_get = par[0]+(par[1]<<8);
	model_g_light_s_t *p_model = (model_g_light_s_t *)cb_par->model;
    return mesh_tx_cmd_lc_prop_st(cb_par->model_idx, id_get, p_model->com.ele_adr, cb_par->adr_src, 0, 0);
}

int mesh_cmd_sig_lc_prop_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    // MMDL-SR-LLC-BV06 / 07 will be set value 2bytes.
    // MMDL-SR-LLCS-BI01 require egnore 1bytes.
    lc_prop_set_t *p_set = (lc_prop_set_t *)par;
	const lc_prop_info_t * p_prop = get_lc_prop_info(p_set->id);
	if(p_prop && (p_prop->len == par_len)){
        set_lc_val(p_prop, cb_par->model_idx, p_set->val);
		mesh_model_store(1, SIG_MD_LIGHT_LC_S);
		prop_publish_id_sel = p_set->id;
        model_pub_check_set(ST_G_LEVEL_SET_PUB_NOW, cb_par->model, 0);

        // update lightness of current state when set some properties with lightness.
        lc_property_proc_t *p_proc = &g_lc_prop_proc[cb_par->model_idx];
		LOG_LIGHT_LC_DEBUG(0,0,"LC current st: %d, prop id: 0x%04x", p_proc->st, p_prop->id);
        int action_flag = 0;
        if(LC_PROP_ID_LightnessOn == p_prop->id){
        	if(LC_STATE_RUN == p_proc->st){
        		action_flag = 1;
        	}
        }else if(LC_PROP_ID_LightnessProlong == p_prop->id){
        	if(LC_STATE_PROLONG == p_proc->st){
        		action_flag = 1;
        	}
        }else if(LC_PROP_ID_LightnessStandby == p_prop->id){
        	if((LC_STATE_STANDBY == p_proc->st) || (LC_STATE_FADE_STANDBY_AUTO == p_proc->st)
        	|| (LC_STATE_FADE_STANDBY_MANUAL == p_proc->st)){
        		action_flag = 1;
        	}
        }

        if(action_flag){
			u16 lightness = get_u16_not_aligned(p_set->val);
			access_cmd_set_LC_lightness(cb_par->model_idx, lightness, 0, 0); // no transition time of lightness at this state. so no worry that this command will cancel the transition flow of light control.
        }
        
		if(cb_par->op_rsp != STATUS_NONE){
			return mesh_cmd_sig_lc_prop_get(par, par_len, cb_par);
		}
	}

	return 0;
}

u32 get_lc_prop_time_ms(int light_idx, int LC_state)
{
	light_lc_property_t *propty = &model_sig_light_lc.propty[light_idx];
    if(LC_STATE_FADE_ON == LC_state){
        return propty->TimeFadeOn.val;
    }else if(LC_STATE_RUN == LC_state){
        return propty->TimeRun.val;
    }else if(LC_STATE_FADE == LC_state){
        return propty->TimeFade.val;
    }else if(LC_STATE_PROLONG == LC_state){
        return propty->TimeProlong.val;
    }else if(LC_STATE_FADE_STANDBY_AUTO == LC_state){
        return propty->TimeStandbyAuto.val;
    }else if(LC_STATE_FADE_STANDBY_MANUAL == LC_state){
        return propty->TimeStandbyManual.val;
    /*}else if(LC_STATE_OCCUPANCY_DELAY == LC_state){
        return propty->TimeOccupancyDelay.val;*/
    }

    return 0;
}

/**
 * @brief       This function get OccupancyDelay_time
 * @param[in]   light_idx	- light index if "LIGHT CNT > 1", or it is always 0.
 * @return      
 * @note        
 */
static inline u32 get_OccupancyDelay_time_ms(int light_idx)
{
	lc_prop_u24_t *p_val = &model_sig_light_lc.propty[light_idx].TimeOccupancyDelay;
	return p_val->val;
}

u16 get_lc_state_target_lightness(int light_idx, u8 lc_st)
{
	light_lc_property_t *propty = &model_sig_light_lc.propty[light_idx];
	u16 lightness_target;// 
	if(LC_STATE_OFF == lc_st){
		lightness_target = 0;
	}else if(LC_STATE_FADE_ON == lc_st || LC_STATE_RUN == lc_st){
		lightness_target = propty->LightnessOn;
		if(0 == lightness_target){
			light_range_u16_t range_u16 = {0};
			light_g_level_range_get_u16(&range_u16, light_idx, ST_TRANS_LIGHTNESS);
			lightness_target = range_u16.max; // lightness of "RUN" state should not be 0, refer to MMDL/SR/LLC/BV-13-C Light LC Light PI regulator
		}
	}else if(LC_STATE_FADE == lc_st || LC_STATE_PROLONG == lc_st){
		lightness_target = propty->LightnessProlong;
	}else{ // LC_STATE_STANDBY,LC_STATE_FADE_STANDBY_AUTO,LC_STATE_FADE_STANDBY_MANUAL
		lightness_target = propty->LightnessStandby;
	}
	
	return lightness_target;
}

void LC_property_tick_set(int light_idx)
{
    if(light_idx < ARRAY_SIZE(g_lc_prop_proc)){
        g_lc_prop_proc[light_idx].tick_ms = clock_time_ms();
    }
}

void LC_property_proc_st_set(int light_idx, u8 st) // such as LC_STATE_OFF
{
    //if(light_idx < ARRAY_SIZE(g_lc_prop_proc)){
        g_lc_prop_proc[light_idx].st = st;
        
        if(LC_STATE_STANDBY == st || LC_STATE_FADE_STANDBY_AUTO == st || LC_STATE_FADE_STANDBY_MANUAL == st){ // refer to round 8 of MMDL/SR/LLC/BV-11-C Light LC Server Power-Up Behavior
        	if(model_sig_light_lc.lc_onoff_target[light_idx] != G_OFF){
				model_sig_light_lc.lc_onoff_target[light_idx] = G_OFF;
				mesh_model_store(1, SIG_MD_LIGHT_LC_S);
			}
        }
    //}
}

void LC_proc_init(int light_idx)
{
	// LC proc initial
	if(light_idx < LIGHT_CNT){
		model_sig_light_lc.lc_onoff_target[light_idx] = G_OFF; // both mode on or off need to set lc_onoff to OFF state when change lc mode.
		memset(&g_lc_prop_proc[light_idx], 0, sizeof(g_lc_prop_proc[light_idx]));
	}
}

const char LC_STATE_STRING[][24] = {
    {"OFF"},
    {"STANDBY"},
    {"FADE_ON"},
    {"RUN"},
    {"FADE"},
    {"PROLONG"},
    {"FADE_STANDBY_AUTO"},
    {"FADE_STANDBY_MANUAL"},
    {"OCCUPANCY_DELAY"},
};

/**
 * @brief       This function set light control property state and record trigger tick.
 * @param[in]   light_idx	- light index if "LIGHT CNT > 1", or it is always 0.
 * @param[in]   st			- state of light control
 * @return      none
 * @note        
 */
void LC_property_st_and_tick_set(int light_idx, u8 st)
{
    if(light_idx < ARRAY_SIZE(g_lc_prop_proc)){
        LC_property_proc_st_set(light_idx, st); // g_lc_prop_proc[light_idx].st = st;
        g_lc_prop_proc[light_idx].tick_ms = clock_time_ms();
        u16 lightness_present = get_lightness_present(light_idx);
        LOG_MSG_LIB(TL_LOG_NODE_BASIC,0,0,"set LC_property_st:%s, time:%d ms, current lightness:0x%04x, current lc onoff:%d", LC_STATE_STRING[st], get_lc_prop_time_ms(light_idx, st), lightness_present, model_sig_light_lc.lc_onoff_target[light_idx]);
    }
}

/**
 * @brief       This function set occupancy value and trigger tick.
 * @param[in]   light_idx	- light index if "LIGHT CNT > 1", or it is always 0.
 * @param[in]   occupancy_on- 
 * @return      none
 * @note        
 */
void LC_occupancy_value_and_tick_set(int light_idx, u8 occupancy_on)
{
    if(light_idx < ARRAY_SIZE(g_lc_prop_proc)){
        g_lc_prop_proc[light_idx].tick_ms_sensor = clock_time_ms() | 1;
        g_lc_prop_proc[light_idx].occupancy_on = occupancy_on;
        LOG_LIGHT_LC_DEBUG(0,0,"set occupancy on:%d", occupancy_on);
    }
}

/**
 * @brief       This function clear "occupancy_on" state.
 * @param[in]   light_idx	- light index if "LIGHT CNT > 1", or it is always 0.
 * @return      none
 * @note        
 */
static inline void LC_occupancy_on_clear(int light_idx)
{
    if(light_idx < ARRAY_SIZE(g_lc_prop_proc)){
        g_lc_prop_proc[light_idx].tick_ms_sensor = g_lc_prop_proc[light_idx].occupancy_on = 0;
    }
}

static inline u8 get_lc_onoff_from_level(int st, int level)
{
    return (LC_STATE_FADE_STANDBY_MANUAL == st);
}

void mesh_lc_onoff_st_rsp_par_fill(mesh_cmd_lc_onoff_st_t *rsp, u8 light_idx)
{
    lc_property_proc_t *p_proc = &g_lc_prop_proc[light_idx];
    if((LC_STATE_FADE_ON == p_proc->st)||(LC_STATE_FADE_STANDBY_MANUAL == p_proc->st)){
    	mesh_cmd_g_level_st_t level_st; 
    	light_g_level_get((u8 *)&level_st, light_idx, ST_TRANS_LIGHTNESS);

        u16 lightness_present = get_lightness_present(light_idx);
    	rsp->present_onoff = !((model_sig_light_lc.propty[light_idx].LightnessStandby == lightness_present)
    	                        || (0 == lightness_present));
    	rsp->target_onoff = model_sig_light_lc.lc_onoff_target[light_idx];//(model_sig_light_lc.LightnessStandby != get_lightness_target(light_idx));
    	rsp->remain_t = level_st.remain_t;
	}else{
    	rsp->present_onoff = model_sig_light_lc.lc_onoff_target[light_idx];
    	//rsp->target_onoff = ;
    	rsp->remain_t = 0;
	}
}

int mesh_tx_cmd_lc_onoff_st(u8 light_idx, u16 ele_adr, u16 dst_adr, u8 *uuid, model_common_t *pub_md, u16 op_rsp)
{
    mesh_cmd_lc_onoff_st_t rsp = {0};
    mesh_lc_onoff_st_rsp_par_fill(&rsp, light_idx);
    u32 len = sizeof(rsp);

#if 0 // PTS_TEST_MMDL_SR_LLC_BV_11_C
	LOG_LIGHT_LC_DEBUG(0,0,"tx lc onoff status present:%d, target:%d", rsp.present_onoff, rsp.target_onoff);
	if(0 == rsp.remain_t){
		len -= 2;
	}else{
		if(G_ON == rsp.target_onoff){
			rsp.present_onoff = rsp.target_onoff; // at round 4, both present and target need to be set ON. don't know why, TODO.
		}
	}
#else
	if(0 == rsp.remain_t){
		len -= 2;
	}
#endif

    return mesh_tx_cmd_rsp(op_rsp, (u8 *)&rsp, len, ele_adr, dst_adr, uuid, pub_md);
}

int mesh_lc_onoff_st_rsp(mesh_cb_fun_par_t *cb_par)
{
    model_g_light_s_t *p_model = (model_g_light_s_t *)cb_par->model;
    return mesh_tx_cmd_lc_onoff_st(cb_par->model_idx, p_model->com.ele_adr, cb_par->adr_src, 0, 0, cb_par->op_rsp);
}

int mesh_lc_onoff_st_publish(u8 light_idx)
{
	model_common_t *p_com_md = &model_sig_light_lc.srv[light_idx].com;
	u16 ele_adr = p_com_md->ele_adr;
	u16 pub_adr = p_com_md->pub_adr;
	if(!pub_adr){
		return -1;
	}
	u8 *uuid = get_virtual_adr_uuid(pub_adr, p_com_md);
	
    return mesh_tx_cmd_lc_onoff_st(light_idx, ele_adr, pub_adr, uuid, p_com_md, LIGHT_LC_ONOFF_STATUS);
}

int mesh_cmd_sig_lc_onoff_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    return mesh_lc_onoff_st_rsp(cb_par);
}


/**
 * @brief       This function set light control entering to a state.
 * @param[in]   light_idx	- light index if "LIGHT CNT > 1", or it is always 0.
 * @param[ioooo]trs_par		- parameter of transmit time and delay. NULL mean no transmit parameter.
 * @param[in]   lc_st		- state of light control
 * @return      none
 * @note        
 */
void LC_state_enter(int light_idx, transition_par_t *trs_par, u8 lc_st)
{
	if(lc_st >= LC_STATE_MAX){
        LOG_LIGHT_LC_DEBUG(0,0,"LC current st: %d, new st: %d, so no change st.", g_lc_prop_proc[light_idx].st, lc_st);
		return ;
	}
	
    LC_property_st_and_tick_set(light_idx, lc_st);
    #if 0
    if(lc_st == LC_STATE_OCCUPANCY_DELAY){
    	// nothing, should not enter here.
    }else
    #endif
    {
	    u16 lightness_target = get_lc_state_target_lightness(light_idx, lc_st);
	    if(trs_par){
	        access_cmd_set_LC_lightness(light_idx, lightness_target, trs_par, 0);
	    }else{
	    	u32 time_ms = get_lc_prop_time_ms(light_idx, lc_st);
	        access_cmd_set_LC_lightness(light_idx, lightness_target, 0, time_ms);
	    }
    }
}

/*
 * refer to spec "Figure 6.8: Light LC State Machine - Part 2"
 */
static inline u8 LC_onoff_get_next_state(bool4 light_onoff, u8 lc_st_current)
{
	u8 lc_st_next = LC_STATE_KEEP_CURRENT_ST; // invalid state // 
	if(G_ON == light_onoff){
		if(LC_STATE_FADE_ON == lc_st_current){
			// nothing ? // keep state
		}else if(LC_STATE_RUN == lc_st_current){
			// nothing ? // keep state
		}else{
			lc_st_next = LC_STATE_FADE_ON;
			#if 0
			if((LC_STATE_OFF == p_proc->st) && (G_ON == light_g_onoff_present_get(light_idx))){
				LOG_MSG_ERR(TL_LOG_NODE_BASIC,0,0,"current is ON state by manual, so no trigger!!", 0);
			}else
			#endif
			{
			}
		}
	}
	else{ // if(G_OFF == light_onoff)
		if(LC_STATE_OFF == lc_st_current){
			// should not happen, because LC mod is on.
			lc_st_next = LC_STATE_STANDBY;
			#if 1 // MMDL/SR/LLC/BV-03-C
			/* PTS require publish LC ONOFF status, after setting LC OFF, THEN ON.
			 * if no action when receive LC OFF, it will not publish because lightness is not changed and keeping 0xffff when receive LC ON.
			 * TODO later
			 */
			//lc_onoff_set_state_without_lightness_change(light_idx, p_set->onoff, p_trs_par);
			#endif
		}else if((LC_STATE_STANDBY == lc_st_current)||(LC_STATE_FADE_STANDBY_MANUAL == lc_st_current)){
			lc_st_next = LC_STATE_KEEP_CURRENT_ST;
		}else{
			lc_st_next = LC_STATE_FADE_STANDBY_MANUAL;
		}
	}

	return lc_st_next;
}

/*
 * light control onoff also need to change lightness, please refer to "6.2.5.13.2 Lightness Out state".
 */
int mesh_cmd_sig_lc_onoff_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    mesh_cmd_lc_onoff_set_t *p_set = (mesh_cmd_lc_onoff_set_t *)par;
    int light_idx = cb_par->model_idx;
    transition_par_t *p_trs_par = 0;
    if(par_len >= sizeof(mesh_cmd_lc_onoff_set_t)){  // existed transit_t and delay.
        p_trs_par = (transition_par_t *)&p_set->transit_t;
    }
    
    if(p_trs_par){
        if(!is_valid_transition_step(p_trs_par->transit_t)){
            return -1;
        }
	}

	if(p_set->onoff >= G_ONOFF_RSV){
		return -1;
	}

    if((light_idx < ARRAY_SIZE(g_lc_prop_proc)) && (LC_MODE_ON == model_sig_light_lc.mode[light_idx])
       && (!cb_par->retransaction)){
        lc_property_proc_t *p_proc = &g_lc_prop_proc[light_idx];
		model_sig_light_lc.lc_onoff_target[light_idx] = p_set->onoff;
		mesh_model_store(1, SIG_MD_LIGHT_LC_S);

		u8 lc_st_next = LC_onoff_get_next_state(p_set->onoff, p_proc->st);
        if(lc_st_next < LC_STATE_MAX){
			LC_state_enter(light_idx, p_trs_par, lc_st_next);
		}
    }

    if(cb_par->op_rsp != STATUS_NONE){
        err = mesh_lc_onoff_st_rsp(cb_par);
    }
    
    return err;
}

static inline int lc_rx_sensor_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    sensor_mpid_B_t *p_st_B = (sensor_mpid_B_t *)par;
	LOG_LIGHT_LC_DEBUG(0, 0, "lc rx sensor status:", 0);
    
	int action_flag = 0;
	u8 occupancy_on = 0;
	
    if(SENSOR_DATA_FORMAT_A == p_st_B->format){
        //sensor_mpid_A_t *p_st_A = (sensor_mpid_A_t *)par;
    	//u8 len_val = p_st_A->length + 1;
    }else{ // SENSOR_DATA_FORMAT_B
    	u8 len_val = (FORMAT_B_VALUE_LEN_ZERO_FLAG == p_st_B->length) ? 0 : p_st_B->length + 1;
		LOG_LIGHT_LC_DEBUG(p_st_B->raw_value, len_val, "    property id: 0x%x, len: %d, raw value: ", p_st_B->prop_id, len_val);
        if(PROP_ID_OCCUPANCY_MOTION_SENSED == p_st_B->prop_id){
        	if(1 == len_val){
				action_flag = 1;
	            u8 percent = p_st_B->raw_value[0];
	            occupancy_on = !!percent;
            }
        }else if(PROP_ID_PEOPLE_COUNT == p_st_B->prop_id){
        	if(2 == len_val){
				action_flag = 1;
	            u16 people_cnt = get_u16_not_aligned(p_st_B->raw_value);
	            occupancy_on = !!people_cnt;
            }
        }else if(PROP_ID_PRESENCE_DETECTED == p_st_B->prop_id){
        	if(1 == len_val){
				action_flag = 1;
	            bool4 val = p_st_B->raw_value[0];
	            occupancy_on = !!val;
            }
        }else if(PROP_ID_TIME_SINCE_MOTION_SENSED == p_st_B->prop_id){
			if(3 == len_val){
				action_flag = 1;
				u32 time_cnt = get_u16_not_aligned(p_st_B->raw_value);
				occupancy_on = (time_cnt < get_OccupancyDelay_time_ms(cb_par->model_idx));
			}
		}
    }

	if(action_flag){
		if(occupancy_on){
			// Occupancy On event
			LC_occupancy_value_and_tick_set(cb_par->model_idx, occupancy_on);
		}else{
			// Occupancy Off event
			// TODO: nothing ?
		}
	}
	
	return 0;
}

int mesh_cmd_sig_sensor_status_lc(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	lc_rx_sensor_status(par, par_len, cb_par);

	return 0;
}

void LC_light_transition_complete_handle(int light_idx)
{
    if(light_idx < ARRAY_SIZE(g_lc_prop_proc)){
    	u8 lc_st_next = LC_STATE_KEEP_CURRENT_ST; // invalid
        lc_property_proc_t *p_proc = &g_lc_prop_proc[light_idx];
		LOG_LIGHT_LC_DEBUG(0,0,"zzzzz current transition complete, st:%d", p_proc->st);
        if(LC_STATE_FADE_ON == p_proc->st){
            lc_st_next = LC_STATE_RUN;
        }else if(LC_STATE_RUN == p_proc->st){
            lc_st_next = LC_STATE_FADE;
        }else if(LC_STATE_FADE == p_proc->st){
            lc_st_next = LC_STATE_PROLONG;
        }else if(LC_STATE_PROLONG == p_proc->st){
            lc_st_next = LC_STATE_FADE_STANDBY_AUTO;
        }else if((LC_STATE_FADE_STANDBY_AUTO == p_proc->st)
              || (LC_STATE_FADE_STANDBY_MANUAL == p_proc->st)){
            lc_st_next = LC_STATE_STANDBY;
        }

        LC_state_enter(light_idx, 0, lc_st_next);
    }
}

u8 set_LC_lightness_flag = 0;

/**
 * @brief  
 * @param  trs_par  is transition time in command.
 * @time_ms make no sense if trs_par is not Null.
 *   (0: success; others: error)
 */
void access_cmd_set_LC_lightness(int light_idx, u16 lightness, transition_par_t *trs_par, u32 time_ms)
{
    // can't use access_cmd_set_lightness_, because it can't not run at current loop, and just push fifo, so set_LC_lightness_flag will make no sense.
    u8 ac[2+sizeof(mesh_cmd_lightness_set_t)+8] = {0};
    u16 op = LIGHTNESS_SET_NOACK;
    u32 size_op = SIZE_OF_OP(op);
    ac[0] = op & 0xff;
    ac[1] = (op >> 8) & 0xff;
    mesh_cmd_lightness_set_t *p_set = (mesh_cmd_lightness_set_t *)(ac+size_op);
    p_set->lightness = lightness;

    int par_len = sizeof(mesh_cmd_lightness_set_t);
    if(trs_par){
        p_set->transit_t = trs_par->transit_t;
        p_set->delay = trs_par->delay;
    }else{
        st_transition_t st_transition = {0};
        st_transition.remain_t_ms = time_ms;
        p_set->transit_t = light_remain_time_get(&st_transition);
    }

	set_LC_lightness_flag = 1;
    if(0 == lightness){
		ac[0] = G_ONOFF_SET_NOACK & 0xff;
		ac[1] = (G_ONOFF_SET_NOACK >> 8) & 0xff;
		ac[2] = G_OFF;
		u8 len = sizeof(mesh_cmd_lightness_set_t) - OFFSETOF(mesh_cmd_lightness_set_t, tid);
		memcpy(ac+3, &p_set->tid, len);
	    mesh_tx_cmd2self_primary(light_idx, ac, 3 + len);
    }else{
	    mesh_tx_cmd2self_primary(light_idx, ac, size_op + par_len);
    }
	set_LC_lightness_flag = 0;
}

void LC_state_check_and_clear_by_user_command(int light_idx)
{
    if(0 == set_LC_lightness_flag){
        lc_property_proc_t *p_proc = &g_lc_prop_proc[light_idx];
        if(p_proc->st){
            memset(&g_lc_prop_proc[light_idx], 0, sizeof(g_lc_prop_proc[light_idx]));
            LOG_LIGHT_LC_DEBUG(0,0,"clear LC state by user command !!", LC_STATE_STRING[0], 0);
        }
    }
}

void LC_property_proc()
{
	#if 0 // PTS_TEST_MMDL_SR_LLC_BV_08_C
	static u8 test_initial_onoff; // need to bind Generic onoff state with LC onoff state.
	if(0 == test_initial_onoff){
		test_initial_onoff = 1;
		access_cmd_onoff(ele_adr_primary, 0, 0, 0, 0);
	}
	#endif

	#if 0 // PTS_TEST_MMDL_SR_LLC_BV_03_C
	return ;
	#endif

    foreach_arr(light_idx,g_lc_prop_proc){
        lc_property_proc_t *p_proc = &g_lc_prop_proc[light_idx];
        if(LC_MODE_ON != model_sig_light_lc.mode[light_idx]){
        	if(p_proc->tick_ms_sensor){
        		LC_occupancy_on_clear(light_idx);
        	}
        }else{ // mode == LC_MODE_ON
			#if 0 // PTS_TEST_MMDL_SR_LLC_BV_11_C
			static volatile u32 test_once_flag = 1;
			if(test_once_flag && clock_time_exceed(0, 1000*1000)){
				test_once_flag = 0;
				if(model_sig_light_lc.lc_onoff_target[light_idx]){
					transition_par_t transition_par;
					memset(&transition_par, 0, sizeof(transition_par));
					access_cmd_onoff(ele_adr_primary, 0, 0, 0, &transition_par); // set level to 0, to trigger transition of "fade on".
					access_cmd_lc_onoff(ele_adr_primary, 0, 1, 0, 0);
				}
			}
			#endif

			if(p_proc->tick_ms_sensor){
				if(clock_time_exceed_ms(p_proc->tick_ms_sensor, get_OccupancyDelay_time_ms(light_idx))){
					LOG_LIGHT_LC_DEBUG(0, 0, "light control occupancy delay time expired", 0);
					u16 adr_dst = ele_adr_primary + light_idx * ELE_CNT_EVERY_LIGHT;
					//p_proc->st = LC_STATE_STANDBY; // just make sure not stay at occupancy delay state any more.
					access_cmd_lc_onoff(adr_dst, 0, p_proc->occupancy_on, 0, 0); // will change to next LC state.
					
					LC_occupancy_on_clear(light_idx); // initial
				}
			}
			
        	u32 time_prop = get_lc_prop_time_ms(light_idx, p_proc->st);
            switch(p_proc->st){
                case LC_STATE_OFF:
                    // wait for light on or occupancy on
                    // trigger by LC light onoff() or ? to next phase
                    break;
                case LC_STATE_STANDBY:
                    // wait for light on or occupancy on
                    // trigger by LC light onoff() or ? to next phase
                    break;
                case LC_STATE_FADE_ON:
                	if(0 == time_prop){ // TODO: if LC ONOFF set with transition parameter, the transition time is 0, it will not enter next step.
                		LC_light_transition_complete_handle(light_idx);
                	}else{
						// trigger to next phase when light transition finished by calling LC_light_transition_complete_handle().
                	}
                    break;
                case LC_STATE_RUN:{
                	// can not trigger by LC_light_transition_complete_handle(), because "state run" has no transition process.
                    lc_prop_u24_t *p_val = &model_sig_light_lc.propty[light_idx].TimeRun;
                    if(clock_time_exceed_ms(p_proc->tick_ms, p_val->val)){
                    	LC_state_enter(light_idx, 0, LC_STATE_FADE);
                    }
                    break;
                }case LC_STATE_FADE:
                	if(0 == time_prop){
                		LC_light_transition_complete_handle(light_idx);
                	}else{
						// trigger to next phase when light transition finished by calling LC_light_transition_complete_handle().
                	}
                    break;
                case LC_STATE_PROLONG:{
                	// can not trigger by LC_light_transition_complete_handle(), because "state run" has no transition process.
                    lc_prop_u24_t *p_val = &model_sig_light_lc.propty[light_idx].TimeProlong;
                    if(clock_time_exceed_ms(p_proc->tick_ms, p_val->val)){
                    	LC_state_enter(light_idx, 0, LC_STATE_FADE_STANDBY_AUTO);
                    }
                    break;
                }case LC_STATE_FADE_STANDBY_AUTO:
                    // trigger to next phase when light transition finished by calling LC_light_transition_complete_handle().
                    break;
                case LC_STATE_FADE_STANDBY_MANUAL:
                    // trigger to next phase when light transition finished by calling LC_light_transition_complete_handle().
                    break;

                default:
                    break;
            }
        }
    }
}

#if LOG_LIGHT_LC_EN
void light_lc_debug_print_all_par()
{
	model_light_lc_t *p_lc = &model_sig_light_lc;
	LOG_LIGHT_LC_DEBUG(0, 0, "light control mode en: 0x%x, Occupancy Mode en: 0x%x", p_lc->mode[0], p_lc->om[0]);
	
	light_lc_property_t *p_propty = &p_lc->propty[0];
	LOG_LIGHT_LC_DEBUG(0, 0, "property LuxLevelOn:        0x%x", p_propty->LuxLevelOn.val);
	LOG_LIGHT_LC_DEBUG(0, 0, "property LuxLevelProlong:   0x%x", p_propty->LuxLevelProlong.val);
	LOG_LIGHT_LC_DEBUG(0, 0, "property LuxLevelStandby:   0x%x", p_propty->LuxLevelStandby.val);
	LOG_LIGHT_LC_DEBUG(0, 0, "property LightnessOn:       0x%x", p_propty->LightnessOn);
	LOG_LIGHT_LC_DEBUG(0, 0, "property LightnessProlong:  0x%x", p_propty->LightnessProlong);
	LOG_LIGHT_LC_DEBUG(0, 0, "property LightnessStandby:  0x%x", p_propty->LightnessStandby);
	LOG_LIGHT_LC_DEBUG(0, 0, "property RegAccuracy:       0x%x", p_propty->RegAccuracy);
	//float RegKid;
	//float RegKiu;
	//float RegKpd;
	//float RegKpu;
	LOG_LIGHT_LC_DEBUG(0, 0, "property TimeOccupancyDelay:0x%x", p_propty->TimeOccupancyDelay.val);
	LOG_LIGHT_LC_DEBUG(0, 0, "property TimeFadeOn:        0x%x", p_propty->TimeFadeOn.val);
	LOG_LIGHT_LC_DEBUG(0, 0, "property TimeRun:           0x%x", p_propty->TimeRun.val);
	LOG_LIGHT_LC_DEBUG(0, 0, "property TimeFade:          0x%x", p_propty->TimeFade.val);
	LOG_LIGHT_LC_DEBUG(0, 0, "property TimeProlong:       0x%x", p_propty->TimeProlong.val);
	LOG_LIGHT_LC_DEBUG(0, 0, "property TimeStandbyAuto:   0x%x", p_propty->TimeStandbyAuto.val);
	LOG_LIGHT_LC_DEBUG(0, 0, "property TimeStandbyManual: 0x%x", p_propty->TimeStandbyManual.val);
}
#endif
#endif

#if MD_CLIENT_EN
int mesh_cmd_sig_lc_mode_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_lc_om_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_lc_prop_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
    }
    return err;
}

int mesh_cmd_sig_lc_onoff_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
    int err = 0;
    if(cb_par->model){  // model may be Null for status message
        //model_client_common_t *p_model = (model_client_common_t *)(cb_par->model);
    }
    return err;
}
#endif

//---------- LC ONOFF--------
int access_cmd_lc_onoff(u16 adr_dst, u8 rsp_max, u8 onoff, int ack, transition_par_t *trs_par)
{
	mesh_cmd_lc_onoff_set_t par = {0};
	u32 par_len = OFFSETOF(mesh_cmd_g_onoff_set_t,transit_t);
	par.onoff = onoff;
	if(trs_par){
		par_len = sizeof(mesh_cmd_g_onoff_set_t);
		memcpy(&par.transit_t, trs_par, 2);
	}

	return SendOpParaDebug(adr_dst, rsp_max, ack ? LIGHT_LC_ONOFF_SET : LIGHT_LC_ONOFF_SET_NOACK, 
						   (u8 *)&par, par_len);
}

//---------- LC ONOFF end--------


#endif

