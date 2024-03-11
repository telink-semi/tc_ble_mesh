/********************************************************************************************************
 * @file	sensors_model.h
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

#include "tl_common.h"
/****************************************************
Sensor Property ID: "Mesh Device Properties v1.0.pdf" or later
*****************************************************/
// NLCP_TYPE_ALS
#define PROP_ID_PRESENT_AMBIENT_LIGHT_LEVEL  			(0x004E) //Characteristic: Illuminance, uint24
#define PROP_ID_SENSOR_GAIN								(0x0074) // Characteristic: Coefficient

// NLCP_TYPE_OCS
#define PROP_ID_OCCUPANCY_MOTION_SENSED             	(0x0042) //Characteristic: Percentage 8, u8
#define PROP_ID_OCCUPANCY_MOTION_THRESHOLD          	(0x0043) //Characteristic: Percentage 8, u8
#define PROP_ID_PEOPLE_COUNT              				(0x004C) //Characteristic: Count 16, u16
#define PROP_ID_PRESENCE_DETECTED						(0x004D) //Characteristic: Boolean, u8
#define PROP_ID_TIME_SINCE_MOTION_SENSED				(0x0068) //Characteristic: Time(unit ms): u24

// NLCP_TYPE_ENM
#define PROP_ID_PRECISE_TOTAL_DEVICE_ENERGY_USE			(0x0072) //Characteristic: Energy32, u32


//------------------sensor setting start----------------------------

#define SENSOR_NUMS						1 // support only 1 now
#if (NLC_SENSOR_TYPE_SEL == NLCP_TYPE_ALS)
#define SENSOR_SETTINGS_NUMS			2
#else
#define SENSOR_SETTINGS_NUMS			1
#endif

// Sensor Property ID
#if (NLC_SENSOR_TYPE_SEL == NLCP_TYPE_ALS)
#define SENSOR_PROP_ID					PROP_ID_PRESENT_AMBIENT_LIGHT_LEVEL
#elif (NLC_SENSOR_TYPE_SEL == NLCP_TYPE_OCS)
#define SENSOR_PROP_ID					PROP_ID_OCCUPANCY_MOTION_SENSED
#elif (NLC_SENSOR_TYPE_SEL == NLCP_TYPE_ENM)
#define SENSOR_PROP_ID					PROP_ID_PRECISE_TOTAL_DEVICE_ENERGY_USE
#else
#define SENSOR_PROP_ID					PROP_ID_PRESENT_AMBIENT_LIGHT_LEVEL
#endif

// Sensor Setting Property ID
#define SENSOR_SETTING_PROP_ID0			SENSOR_PROP_ID
#if (NLC_SENSOR_TYPE_SEL == NLCP_TYPE_ALS)
#define SENSOR_SETTING_PROP_ID1			PROP_ID_SENSOR_GAIN
#endif

#define SENSOR_SETTING_RAW0_LEN			3 // Sensor Setting Raw length of SENSOR_SETTING_PROP_ID0
#if (NLC_SENSOR_TYPE_SEL == NLCP_TYPE_ALS)
#define SENSOR_SETTING_RAW1_LEN			4 // Sensor Setting Raw length of SENSOR_SETTING_PROP_ID1
#endif

#define SENSOR_SETTING_RAW0_START		0
#if (NLC_SENSOR_TYPE_SEL == NLCP_TYPE_ALS)
#define SENSOR_SETTING_RAW1_START		(SENSOR_SETTING_RAW0_START + SENSOR_SETTING_RAW0_LEN)
#define SENSOR_SETTING_RAW_TOTAL_LEN	(SENSOR_SETTING_RAW1_START + SENSOR_SETTING_RAW1_LEN)
#else
#define SENSOR_SETTING_RAW_TOTAL_LEN	(SENSOR_SETTING_RAW0_START + SENSOR_SETTING_RAW0_LEN)
#endif

// sensor data length, reference to Mesh Device Properties
#if (NLC_SENSOR_TYPE_SEL == NLCP_TYPE_ALS)
#define SENSOR_DATA_RAW0_LEN			3 // Sensor Measured Raw length of SENSOR_PROP_ID
#elif (NLC_SENSOR_TYPE_SEL == NLCP_TYPE_OCS)
	#if (SENSOR_PROP_ID == PROP_ID_PEOPLE_COUNT)
#define SENSOR_DATA_RAW0_LEN			2
	#else
#define SENSOR_DATA_RAW0_LEN			1
	#endif
#elif (NLC_SENSOR_TYPE_SEL == NLCP_TYPE_ENM)
#define SENSOR_DATA_RAW0_LEN			4
#else
#define SENSOR_DATA_RAW0_LEN			3
#endif
#define SENSOR_DATA_RAW_MAX_LEN			SENSOR_DATA_RAW0_LEN

// sensor cadence default value
enum{
	TRIGGER_TYPE_FORMAT,
	TRIGGER_TYPE_UNITLESS,	
};

#define MAX_FAST_CADENCE_PERIOD_DIV		15 // spec defined, not change.
#define MAX_MIN_INTERVAL				26 // spec defined, not change.

#define TRIGGER_TYPE_DEFAULT			TRIGGER_TYPE_FORMAT
#define FAST_CADENCE_PERIOD_DIV			2
#define TRIGGER_DELTA_DOWN_DEFAULT		0
#define TRIGGER_DELTA_UP_DEFAULT		0
#define MIN_INTERVAL_DEFAULT			4
#define FAST_CADENCE_LOW_DEFAULT		0
#define FAST_CADENCE_HIGH_DEFAULT		1

//------------------sensor setting end----------------------------

//----------------------------------- op code
// op cmd 0xxxxxxx (SIG)
// op cmd 10xxxxxx xxxxxxxx (SIG)

//sensor
#define SENSOR_DESCRIP_GET			0x3082
#define SENSOR_DESCRIP_STATUS		0x51
#define SENSOR_GET					0x3182
#define SENSOR_STATUS				0x52
#define SENSOR_COLUMN_GET			0x3282
#define SENSOR_COLUMN_STATUS		0x53
#define SENSOR_SERIES_GET			0x3382
#define SENSOR_SERIES_STATUS		0x54

//sensor setup
#define SENSOR_CANDECE_GET			0x3482
#define SENSOR_CANDECE_SET			0x55
#define SENSOR_CANDECE_SET_NOACK	0x56
#define SENSOR_CANDECE_STATUS		0x57
#define SENSOR_SETTINGS_GET			0x3582
#define SENSOR_SETTINGS_STATUS		0x58
#define SENSOR_SETTING_GET			0x3682
#define SENSOR_SETTING_SET			0x59
#define SENSOR_SETTING_SET_NOACK	0x5A
#define SENSOR_SETTING_STATUS		0x5B

typedef struct{
	u16 delta_down;
	u16 delta_up;
	u8 min_interval;
	u32 cadence_low:(SENSOR_DATA_RAW_MAX_LEN*8);
	u32 cadence_high:(SENSOR_DATA_RAW_MAX_LEN*8);
}cadence_unitless_t; // for tx message 

typedef struct{
	u32 delta_down;
	u32 delta_up;
	u8 min_interval;
	u32 cadence_low;
	u32 cadence_high;
}cadence_unit_t; // for save setting.

typedef struct{
	u8 fast_period_div:7;
	u8 trig_type:1;
	union{ 
		u8 par[17];
		cadence_unitless_t cadence_unitless;
		cadence_unit_t cadence_unit;
	};
}sensor_cadence_t;


typedef struct{
	u16 setting_id[SENSOR_SETTINGS_NUMS];
	u8  setting_access[SENSOR_SETTINGS_NUMS];
	u8  setting_raw[SENSOR_SETTING_RAW_TOTAL_LEN];
}sensor_setting_par_t;

typedef struct{
	u8 raw_val_X[12];
	u8 raw_val_W[12];
	u8 raw_val_Y[12];
}sensor_series_col_t;

typedef struct{
	u16 prop_id;
	sensor_cadence_t cadence;
	sensor_setting_par_t setting;
}sensor_states_t;

typedef struct{
#if MD_SERVER_EN
    #if MD_SENSOR_SERVER_EN
	model_g_light_s_t sensor_srv[LIGHT_CNT];			// serve
	model_g_light_s_t sensor_setup[LIGHT_CNT];			// setup
	sensor_states_t sensor_states[SENSOR_NUMS];
    #endif
    #if MD_BATTERY_EN
	model_g_light_s_t battery_srv[LIGHT_CNT];	// serve
    #endif
    #if MD_LOCATION_EN
	model_g_light_s_t location_srv[LIGHT_CNT];
	model_g_light_s_t location_setup[LIGHT_CNT];	
    #endif
#endif

#if MD_SENSOR_CLIENT_EN
	model_client_common_t sensor_clnt[1];		        // client
#endif
#if MD_CLIENT_EN
	#if MD_BATTERY_EN
	model_client_common_t battery_clnt[1];			// serve
	#endif
	#if MD_LOCATION_EN
	model_client_common_t location_clnt[1];			// serve
	#endif
#endif
}model_sensor_t;

typedef struct{
	u16 prop_id;
	u32 positive_tolerance:12;
	u32 negative_tolerance:12;
	u32 sampling_func:8;
	u8  measure_period;
	u8	update_interval;
}mesh_cmd_sensor_descript_st_t;

typedef struct{
	u16 setting_id;
	u8  setting_access;
	u8  len_raw;
	u8  *p_raw;
}sensor_setting_tbl_t;

typedef struct{
	u16 prop_id;
	u8  len_raw; // length of *p_raw
	u8  *p_raw;
	sensor_series_col_t *p_series_col;
}sensor_data_t;

enum{
	SENSOR_DATA_FORMAT_A = 0,
	SENSOR_DATA_FORMAT_B = 1,
};

#define FORMAT_B_VALUE_LEN_ZERO_FLAG		(0x7F)

typedef struct{
	u16 format:1;  // 0
	u16 length:4;
	u16 prop_id:11;
	u8  raw_value[SENSOR_DATA_RAW_MAX_LEN];
}sensor_mpid_A_t;

typedef struct{
 	u8 format:1;   // 1
	u8 length:7;
	u16 prop_id;
	u8 raw_value[SENSOR_DATA_RAW_MAX_LEN];
 }sensor_mpid_B_t;

typedef struct{
	u8 raw_len;
 	sensor_mpid_B_t sensor_mpid;
 }sensor_mpid_b_st_t;

typedef struct{
 	u8 format:1;   // 1
	u8 length:7;
	u8 raw_value[SENSOR_DATA_RAW_MAX_LEN];
 }sensor_npid_b_t;

typedef struct{
	u16 prop_id;
	u16 setting_prop_id[SENSOR_SETTINGS_NUMS];
} sensor_settings_st_t;

typedef struct{
	u16 prop_id;
	sensor_cadence_t cadence;
}sensor_cadence_st_t;

typedef struct{
	u16 prop_id;
	u16 setting_id;
}sensor_setting_get_t;

typedef struct{
	u16 prop_id;
	u16 setting_id;
	u8  setting_access;
	u8  setting_raw[SENSOR_SETTING_RAW_TOTAL_LEN];
}sensor_setting_st_t;

typedef struct{
	u16 prop_id;
	u16 setting_id;
	u8  setting_raw[SENSOR_SETTING_RAW_TOTAL_LEN];
}sensor_setting_set_t;

typedef struct{
	u16 prop_id;
	u8 raw_val_x1;
	u8 raw_val_x2;
}sensor_series_get_t;

typedef struct{
	u16 prop_id;
	sensor_series_col_t series_raw;
}sensor_series_st_t;

typedef struct{
	u16 prop_id;
	u16 raw_val_X;
}sensor_col_get_t;

//sensor sampling function
enum{
	UNSPECIFIED,
	INST,
	ARITHMETIC,
	RMS,
	MAX,
	MIN,
	ACCUMULATED,
	CNT,
};

enum{
	RFU_T,
	BOOLEAN_T,
	BIT2_T,
	NIBBLE_T,
	UINT8_T,
	UINT12_T,
	UINT16_T,
	UINT24_T,
	UINT32_T,
	UINT48_T,
	UINT64_T,
	UINT128_T,
};

enum{
	PROHIBITED_PROP_ID = 0,
};

enum{
	PROHIBITED=0,
	READ=1,
	READ_WRITE=3,
};

extern _align_4_ model_sensor_t			model_sig_sensor;
extern sensor_data_t sensor_data[SENSOR_NUMS];

void mesh_global_var_init_sensor_descrip();
int mesh_sensor_st_publish(u8 idx);
int mesh_sensor_setup_st_publish(u8 idx);
sensor_data_t *get_sensor_data(u16 prop_id);

#if MD_SENSOR_SERVER_EN
int mesh_cmd_sig_sensor_descript_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_sensor_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_sensor_cadence_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_sensor_cadence_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_sensor_settings_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_sensor_setting_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_sensor_setting_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_sensor_column_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_sensor_series_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
#else
#define mesh_cmd_sig_sensor_descript_get                (0)
#define mesh_cmd_sig_sensor_get                         (0)
#define mesh_cmd_sig_sensor_cadence_get                 (0)
#define mesh_cmd_sig_sensor_cadence_set                 (0)
#define mesh_cmd_sig_sensor_settings_get                (0)
#define mesh_cmd_sig_sensor_setting_get                 (0)
#define mesh_cmd_sig_sensor_setting_set                 (0)
#define mesh_cmd_sig_sensor_column_get                  (0)
#define mesh_cmd_sig_sensor_series_get                  (0)
#endif

#if MD_SENSOR_CLIENT_EN
int mesh_cmd_sig_sensor_descript_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_sensor_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_sensor_cadence_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_sensor_settings_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_sensor_setting_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_sensor_column_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
int mesh_cmd_sig_sensor_series_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par);
#else
#define mesh_cmd_sig_sensor_descript_status             (0)
#define mesh_cmd_sig_sensor_status                      (0)
#define mesh_cmd_sig_sensor_cadence_status              (0)
#define mesh_cmd_sig_sensor_settings_status             (0)
#define mesh_cmd_sig_sensor_setting_status              (0)
#define mesh_cmd_sig_sensor_column_status               (0)
#define mesh_cmd_sig_sensor_series_status               (0)
#endif

u32 sensor_measure_proc();
void sensor_lighting_ctrl_proc();
int access_cmd_sensor_occupancy_motion_sensed(u16 adr_dst, u8 percent);

extern _align_4_ model_sensor_t			model_sig_sensor;
extern u32 sensor_measure_quantity;
