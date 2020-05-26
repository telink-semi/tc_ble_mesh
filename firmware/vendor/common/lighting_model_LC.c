/********************************************************************************************************
 * @file     lighting_model_LC.c 
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
#include "../../proj/tl_common.h"
#ifndef WIN32
#include "../../proj/mcu/watchdog_i.h"
#endif 
#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../vendor/common/user_config.h"
#include "app_health.h"
#include "../../proj_lib/sig_mesh/app_mesh.h"
#include "lighting_model_LC.h"
#include "lighting_model.h"


#if (MD_LIGHT_CONTROL_EN)
model_light_lc_t       model_sig_light_lc;
u32 mesh_md_light_lc_addr = FLASH_ADR_MD_LIGHT_LC;
u16 prop_publish_id_sel = LC_PROP_ID_LightnessOn;

#if MD_SERVER_EN

#define GET_LC_PROP_LEN(type)   \
    ((LC_PROP_TYPE_LUXLEVEL == type) ? LEN_LC_PROP_LUXLEVEL :\
       ((LC_PROP_TYPE_LIGHTNESS == type) ? LEN_LC_PROP_LIGHTNESS :\
          ((LC_PROP_TYPE_ACCURACY == type) ? LEN_LC_PROP_ACCURACY :\
            ((LC_PROP_TYPE_REGULATOR == type) ? LEN_LC_PROP_REGULATOR :\
              ((LC_PROP_TYPE_TIME == type) ? LEN_LC_PROP_TIME :0)))))

#define INIT_LC_PROP_ARRAY(id,member,type,val)    \
{GET_LC_PROP_LEN(type),id,&model_sig_light_lc.member,sizeof(model_sig_light_lc.member[0]),type,val}

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
    INIT_LC_PROP_ARRAY(LC_PROP_ID_TimeFadeOn,TimeFadeOn,LC_PROP_TYPE_TIME,LC_PROP_VAL_TimeOccupancyDelay),
    INIT_LC_PROP_ARRAY(LC_PROP_ID_TimeRunOn,TimeRunOn,LC_PROP_TYPE_TIME,LC_PROP_VAL_TimeRunOn),
    INIT_LC_PROP_ARRAY(LC_PROP_ID_TimeFade,TimeFade,LC_PROP_TYPE_TIME,LC_PROP_VAL_TimeFade),
    INIT_LC_PROP_ARRAY(LC_PROP_ID_TimeProlong,TimeProlong,LC_PROP_TYPE_TIME,LC_PROP_VAL_TimeProlong),
    INIT_LC_PROP_ARRAY(LC_PROP_ID_TimeFadeStandbyAuto,TimeStandbyAuto,LC_PROP_TYPE_TIME,LC_PROP_VAL_TimeFadeStandbyAuto),
    INIT_LC_PROP_ARRAY(LC_PROP_ID_TimeFadeStandbyManual,TimeStandbyManual,LC_PROP_TYPE_TIME,LC_PROP_VAL_TimeFadeStandbyManual),
};

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
    return ((LIGHT_LC_ONOFF_SET == op)||(LIGHT_LC_ONOFF_SET_NOACK == op));
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
    p_scene->lc_onoff = model_sig_light_lc.onoff[light_idx];
    //memcpy(&p_scene->lc_propty, &model_sig_light_lc.prop[light_idx], sizeof(p_scene->lc_propty));
}

void scene_load_lc_par(scene_data_t *p_scene, int light_idx)
{
    if(p_scene->lc_mode != model_sig_light_lc.mode[light_idx]){
        model_sig_light_lc.mode[light_idx] = p_scene->lc_mode;
        mesh_lc_mode_st_publish(light_idx);
    }
    
    model_sig_light_lc.onoff[light_idx] = p_scene->lc_onoff;

#if 0
    light_lc_property_t *p_prop = &model_sig_light_lc.prop[light_idx];
    if(memcmp(&p_scene->lc_propty, p_prop, sizeof(p_scene->lc_propty))){
        memcpy(p_prop, &p_scene->lc_propty, sizeof(p_scene->lc_propty));
        mesh_lc_prop_st_publish(light_idx);
    }
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

int mesh_cmd_sig_lc_mode_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err = -1;
    u8 mode_new = par[0];
    u8 *p_mode = &model_sig_light_lc.mode[cb_par->model_idx];
    if( mode_new < LC_MODE_MAX){
    	if(*p_mode != mode_new){
			*p_mode = mode_new;
			mesh_model_store(1, SIG_MD_LIGHT_LC_S);
            model_pub_check_set(ST_G_LEVEL_SET_PUB_NOW, cb_par->model, 1);
            model_pub_st_cb_re_init_lc_srv(&mesh_lc_mode_st_publish);    // re-init
		}
	
		if(cb_par->op_rsp != STATUS_NONE){
			err = mesh_lc_mode_st_rsp(cb_par);
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
		if(cb_par->op_rsp != STATUS_NONE){
			return mesh_cmd_sig_lc_prop_get(par, par_len, cb_par);
		}
	}

	return 0;
}

typedef struct{
    u32 tick_ms;
    u8 st;
}lc_property_proc_t;

lc_property_proc_t g_lc_prop_proc[LIGHT_CNT];

u32 get_lc_onoff_prop_time_ms(int light_idx, int op_lc_onoff_type)
{
    if(OP_LC_ONOFF_TYPE_ON == op_lc_onoff_type){
        return model_sig_light_lc.TimeFadeOn[light_idx].val;
    }else if(OP_LC_ONOFF_TYPE_OFF == op_lc_onoff_type){
        return model_sig_light_lc.TimeStandbyManual[light_idx].val;
    }

    return 0;
}

void LC_property_tick_set(int light_idx)
{
    if(light_idx < ARRAY_SIZE(g_lc_prop_proc)){
        g_lc_prop_proc[light_idx].tick_ms = clock_time_ms();
    }
}

void LC_property_st_and_tick_set(int light_idx, u8 st)
{
    if(light_idx < ARRAY_SIZE(g_lc_prop_proc)){
        g_lc_prop_proc[light_idx].st = st;
        g_lc_prop_proc[light_idx].tick_ms = clock_time_ms();
    }
}

void LC_property_light_onoff(int light_idx, u8 onoff)
{
    if(light_idx < ARRAY_SIZE(g_lc_prop_proc)){
        lc_property_proc_t *p_proc = &g_lc_prop_proc[light_idx];
        if(LC_MODE_ON == model_sig_light_lc.mode[light_idx]){ // lc mode = 1
            if(G_ON == onoff){
                if(LC_STATE_FADE_ON == p_proc->st){
                    // nothing ?
                }else if(LC_STATE_RUN == p_proc->st){
                    // nothing ?
                }else{
                    LC_property_st_and_tick_set(light_idx, LC_STATE_FADE_ON);
                }
            }else if(G_OFF == onoff){
                if(LC_STATE_STANDBY == p_proc->st){
                    // nothing ?
                }else if(LC_STATE_FADE_STANDBY_MANUAL == p_proc->st){
                    // nothing ?
                }else{
                    LC_property_st_and_tick_set(light_idx, LC_STATE_FADE_STANDBY_MANUAL);
                }
            }
        }
    }
}

void LC_property_proc()
{
    foreach_arr(light_idx,g_lc_prop_proc){
        lc_property_proc_t *p_proc = &g_lc_prop_proc[light_idx];
        if(LC_MODE_ON == model_sig_light_lc.mode[light_idx]){
            switch(p_proc->st){
                case LC_STATE_STANDBY:
                    // wait for light on or occupancy on
                    break;
                case LC_STATE_FADE_ON:{
                    lc_prop_u24_t *p_val = &model_sig_light_lc.TimeFadeOn[light_idx];
                    //const lc_prop_info_t * p_info = get_lc_prop_info(LC_PROP_ID_TimeFadeOn);
                    if(clock_time_exceed_ms(p_proc->tick_ms, p_val->val)){
                        LC_property_st_and_tick_set(light_idx, LC_STATE_RUN);
                    }
                    break;
                }case LC_STATE_RUN:{
                    lc_prop_u24_t *p_val = &model_sig_light_lc.TimeRunOn[light_idx];
                    if(clock_time_exceed_ms(p_proc->tick_ms, p_val->val)){
                        LC_property_st_and_tick_set(light_idx, LC_STATE_RUN);
                    }
                    break;
                }case LC_STATE_FADE:
                    break;
                case LC_STATE_PROLONG:
                    break;
                case LC_STATE_FADE_STANDBY_AUTO:
                    break;
                case LC_STATE_FADE_STANDBY_MANUAL:
                    break;
                default:
                    break;
                #if 0
                if(p_proc->tick && clock_time_exceed(p_proc->tick, 4000*1000)){ // comfirm later
                    p_proc->tick = 0;
                    #if (PTS_TEST_EN)
        			mesh_cmd_g_level_set_t level_set_tmp = {0};
        			int len_tmp = GET_LEVEL_PAR_LEN(0);
                    u16 lightness = p_prop->val[0]+(p_prop->val[1]<<8);
        			level_set_tmp.level = u16_to_s16(lightness);
                    st_pub_list_t pub_list = {{0}};
        			g_level_set_and_update_last((u8 *)&level_set_tmp, len_tmp, G_LEVEL_SET_NOACK, i, 0, ST_TRANS_LIGHTNESS, 0, &pub_list);
        			#endif
                }
                #endif
            }
        }
    }
}
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
#endif

//---------- LC ONOFF use generic onoff--------
//---------- LC ONOFF end--------


//--lighting model command interface-------------------
//-----------access command--------
// light HSL

//--lighting model command interface end----------------


#endif

