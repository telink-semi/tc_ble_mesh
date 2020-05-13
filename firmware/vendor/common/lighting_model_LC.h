/********************************************************************************************************
 * @file     lighting_model_LC.h 
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

#include "../../proj/tl_common.h"
#include "../../proj_lib/sig_mesh/app_mesh.h"

//----------------------------------- op code
// op cmd 0xxxxxxx (SIG)

// op cmd 10xxxxxx xxxxxxxx (SIG)
#define LIGHT_LC_MODE_GET		        0x9182
#define LIGHT_LC_MODE_SET		        0x9282
#define LIGHT_LC_MODE_SET_NOACK		    0x9382
#define LIGHT_LC_MODE_STATUS		    0x9482
#define LIGHT_LC_OM_GET		        	0x9582
#define LIGHT_LC_OM_SET		        	0x9682
#define LIGHT_LC_OM_SET_NOACK		    0x9782
#define LIGHT_LC_OM_STATUS		    	0x9882
#define LIGHT_LC_ONOFF_GET		        0x9982
#define LIGHT_LC_ONOFF_SET		        0x9A82
#define LIGHT_LC_ONOFF_SET_NOACK		0x9B82
#define LIGHT_LC_ONOFF_STATUS		    0x9C82
#define LIGHT_LC_PROPERTY_GET		    0x9D82
#define LIGHT_LC_PROPERTY_SET		    0x62
#define LIGHT_LC_PROPERTY_SET_NOACK		0x63
#define LIGHT_LC_PROPERTY_STATUS		0x64

//------op parameters

//------------------vendor op end-------------------

//------------------ LC property id number
#define LC_PROP_ID_LuxLevelOn               0x002B
#define LC_PROP_ID_LuxLevelProlong          0x002C
#define LC_PROP_ID_LuxLevelStandby          0x002D
#define LC_PROP_ID_LightnessOn              0x002E
#define LC_PROP_ID_LightnessProlong         0x002F
#define LC_PROP_ID_LightnessStandby         0x0030
#define LC_PROP_ID_RegulatorAccuracy        0x0031
#define LC_PROP_ID_RegulatorKid             0x0032
#define LC_PROP_ID_RegulatorKiu             0x0033
#define LC_PROP_ID_RegulatorKpd             0x0034
#define LC_PROP_ID_RegulatorKpu             0x0035
#define LC_PROP_ID_TimeFade                 0x0036
#define LC_PROP_ID_TimeFadeOn               0x0037
#define LC_PROP_ID_TimeFadeStandbyAuto      0x0038
#define LC_PROP_ID_TimeFadeStandbyManual    0x0039
#define LC_PROP_ID_TimeOccupancyDelay       0x003A
#define LC_PROP_ID_TimeProlong              0x003B
#define LC_PROP_ID_TimeRunOn                0x003C

// lc_prop_type_t
#define LC_PROP_TYPE_LUXLEVEL       (0)
#define LC_PROP_TYPE_LIGHTNESS      (1)
#define LC_PROP_TYPE_ACCURACY       (2)
#define LC_PROP_TYPE_REGULATOR      (3)
#define LC_PROP_TYPE_TIME           (4)


//------------------ LC property id parameter
#define LC_PROP_VAL_LuxLevelOn              (0) // confirm later
#define LC_PROP_VAL_LuxLevelProlong         (0) // confirm later
#define LC_PROP_VAL_LuxLevelStandby         (0) // confirm later
#define LC_PROP_VAL_LightnessOn             (LIGHTNESS_MAX)
#define LC_PROP_VAL_LightnessProlong        (LIGHTNESS_MAX / 2)
#define LC_PROP_VAL_LightnessStandby        (LIGHTNESS_MAX / 10)
#define LC_PROP_VAL_RegulatorAccuracy       (0) // confirm later
#define LC_PROP_VAL_RegulatorKid            (0) // confirm later
#define LC_PROP_VAL_RegulatorKiu            (0) // confirm later
#define LC_PROP_VAL_RegulatorKpd            (0) // confirm later
#define LC_PROP_VAL_RegulatorKpu            (0) // confirm later
#define LC_PROP_VAL_TimeFade                (2*1000)    // unit: ms
#define LC_PROP_VAL_TimeFadeOn              (2*1000)    // unit: ms
#define LC_PROP_VAL_TimeFadeStandbyAuto     (2*1000)    // unit: ms
#define LC_PROP_VAL_TimeFadeStandbyManual   (2*1000)    // unit: ms
#define LC_PROP_VAL_TimeOccupancyDelay      (2*1000)    // unit: ms
#define LC_PROP_VAL_TimeProlong             (2*1000)    // unit: ms
#define LC_PROP_VAL_TimeRunOn               (2*1000)    // unit: ms

enum{
    LC_MODE_OFF = 0,
    LC_MODE_ON,
    LC_MODE_MAX,
};

enum{
    LC_OM_DISABLE = 0,
    LC_OM_EN,
    LC_OM_MAX,
};

typedef struct{
	u16 id;
	u8 val[4];      // max 4: LEN_LC_PROP_MAX
}lc_prop_set_t;

enum{
    //LC_STATE_OFF    = 0, // use lc mode enable instead of.
    LC_STATE_STANDBY = 0,   // should be 0
    LC_STATE_FADE_ON,
    LC_STATE_RUN,
    LC_STATE_FADE,
    LC_STATE_PROLONG,
    LC_STATE_FADE_STANDBY_AUTO,
    LC_STATE_FADE_STANDBY_MANUAL,
    LC_STATE_MAX,
};

// -----------
int is_light_lc_onoff(u16 op);
u32 get_lc_onoff_prop_time_ms(int light_idx, int op_lc_onoff_type);
void scene_get_lc_par(scene_data_t *p_scene, int light_idx);
void scene_load_lc_par(scene_data_t *p_scene, int light_idx);
void LC_property_tick_set(int idx);
void LC_property_st_and_tick_set(int light_idx, u8 st);
void LC_property_light_onoff(int light_idx, u8 onoff);
void LC_property_proc();
void light_LC_global_init();
int mesh_lc_prop_st_publish(u8 idx);
int mesh_lc_mode_st_publish(u8 idx);
int mesh_lc_onoff_st_publish(u8 idx);

#if MD_SERVER_EN
int mesh_cmd_sig_lc_mode_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_lc_mode_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_lc_om_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_lc_om_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_lc_prop_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_lc_prop_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
#else
#define mesh_cmd_sig_lc_mode_get                (0)
#define mesh_cmd_sig_lc_mode_set                (0)
#define mesh_cmd_sig_lc_om_get                  (0)
#define mesh_cmd_sig_lc_om_set                  (0)
#define mesh_cmd_sig_lc_prop_get                (0)
#define mesh_cmd_sig_lc_prop_set                (0)
#endif

#if MD_CLIENT_EN
int mesh_cmd_sig_lc_mode_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_lc_om_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_lc_prop_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
#else
#define mesh_cmd_sig_lc_mode_status             (0)
#define mesh_cmd_sig_lc_om_status               (0)
#define mesh_cmd_sig_lc_prop_status             (0)
#endif


