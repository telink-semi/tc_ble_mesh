/*
 * Copyright (C) 2019 THL A29 Limited, a Tencent company. All rights reserved.
 * Licensed under the MIT License (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifdef __cplusplus
extern "C" {
#endif
#include "ble_qiot_template_vendor.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "ble_qiot_common.h"
#include "ble_qiot_log.h"

#if LLSYNC_ENABLE
typedef struct {
    llsync_mesh_onoff             mesh_onoff;
    llsync_mesh_unix_time         mesh_unix_time;
    llsync_mesh_power_percent     mesh_power_percent;
    llsync_mesh_HSL               mesh_HSL;
    llsync_mesh_lightness_level   mesh_lightness_level;
    llsync_mesh_color_temperature mesh_color_temperature;
} LLSyncMeshDataTemplate;

static LLSyncMeshDataTemplate sg_mesh_property_data;

static int llsync_mesh_onoff_set(const char *data, uint16_t len)
{
    memcpy(&sg_mesh_property_data.mesh_onoff.onoff, data, len);
    ble_qiot_log_i("Mesh set data is OnOff:%d", sg_mesh_property_data.mesh_onoff.onoff);
    access_cmd_onoff(ele_adr_primary, 0, sg_mesh_property_data.mesh_onoff.onoff, 0, 0); // light_onoff_all(onoff);
    return 0;
}

static int llsync_mesh_onoff_get(char *data, uint16_t len)
{
    mesh_cmd_g_onoff_st_t rsp = {0};
    mesh_g_onoff_st_rsp_par_fill(&rsp, 0);
	sg_mesh_property_data.mesh_onoff.onoff = rsp.target_onoff;
	
    memcpy(data, &sg_mesh_property_data.mesh_onoff.onoff, sizeof(sg_mesh_property_data.mesh_onoff.onoff));
    return sizeof(llsync_mesh_onoff);
}

static int llsync_mesh_unix_time_set(const char *data, uint16_t len)
{
    uint8_t i = 0;

    memcpy(sg_mesh_property_data.mesh_unix_time.unix_time, data, len);

    for (i = 0; i < 6; i++) {
        ble_qiot_log_i("Mesh set data is unix_time:%d-0x%02x", i, sg_mesh_property_data.mesh_unix_time.unix_time[i]);
    }
    return 0;
}

static int llsync_mesh_unix_time_get(char *data, uint16_t len)
{
    memcpy(data, sg_mesh_property_data.mesh_unix_time.unix_time,
           sizeof(sg_mesh_property_data.mesh_unix_time.unix_time));
    return sizeof(llsync_mesh_unix_time);
}

static int llsync_mesh_power_percent_set(const char *data, uint16_t len)
{
    memcpy(&sg_mesh_property_data.mesh_power_percent.power_percent, data, len);
    ble_qiot_log_i("Mesh set data is power_percent:%d", sg_mesh_property_data.mesh_power_percent.power_percent);
    return 0;
}

static int llsync_mesh_power_percent_get(char *data, uint16_t len)
{
    memcpy(data, &sg_mesh_property_data.mesh_power_percent.power_percent,
           sizeof(sg_mesh_property_data.mesh_power_percent.power_percent));
    return sizeof(llsync_mesh_power_percent);
}

#if LIGHT_TYPE_HSL_EN
static int llsync_mesh_HSL_set(const char *data, uint16_t len)
{
    uint8_t i = 0;
    memcpy(sg_mesh_property_data.mesh_HSL.HSL, data, len);
    for (i = 0; i < (sizeof(sg_mesh_property_data.mesh_HSL.HSL) / sizeof(sg_mesh_property_data.mesh_HSL.HSL[0])); i++) {
        sg_mesh_property_data.mesh_HSL.HSL[i] = HTONS(sg_mesh_property_data.mesh_HSL.HSL[i]);
    }

	u16 *p_HSL = &sg_mesh_property_data.mesh_HSL.HSL[0];
	access_cmd_set_light_hsl(ele_adr_primary, 0, p_HSL[0], p_HSL[1], p_HSL[2], 0, 0);

    return 0;
}

static int llsync_mesh_HSL_get(char *data, uint16_t len)
{
	mesh_cmd_light_hsl_st_t rsp = {0};
	mesh_light_hsl_st_rsp_par_fill(&rsp, 0, LIGHT_HSL_TARGET_STATUS);
	memcpy(&sg_mesh_property_data.mesh_HSL.HSL, &rsp.lightness, sizeof(sg_mesh_property_data.mesh_HSL.HSL));

	llsync_mesh_HSL HSL_big;
    foreach_arr(i, HSL_big.HSL){
        HSL_big.HSL[i] = HTONS(sg_mesh_property_data.mesh_HSL.HSL[i]);
    }
    memcpy(data, &HSL_big, min(sizeof(HSL_big), len));
    return sizeof(HSL_big);
}
#endif

#if MD_LIGHTNESS_EN
static int llsync_mesh_lightness_level_set(const char *data, uint16_t len)
{
    memcpy(&sg_mesh_property_data.mesh_lightness_level.lightness_level, data, len);
    sg_mesh_property_data.mesh_lightness_level.lightness_level =
        HTONS(sg_mesh_property_data.mesh_lightness_level.lightness_level);
    ble_qiot_log_i("Mesh set data is lightness_level:%d", sg_mesh_property_data.mesh_lightness_level.lightness_level);

	access_cmd_set_lightness(ele_adr_primary, 0, sg_mesh_property_data.mesh_lightness_level.lightness_level, 0, 0);

    return 0;
}

static int llsync_mesh_lightness_level_get(char *data, uint16_t len)
{
	mesh_cmd_lightness_st_t rsp = {0};
	mesh_level_u16_st_rsp_par_fill(&rsp, 0, ST_TRANS_LIGHTNESS);
	sg_mesh_property_data.mesh_lightness_level.lightness_level = rsp.target;

	llsync_mesh_lightness_level lightness_big;
    lightness_big.lightness_level = HTONS(sg_mesh_property_data.mesh_lightness_level.lightness_level);
    memcpy(data, &lightness_big, min(sizeof(lightness_big), len));
    return sizeof(lightness_big);
}
#endif

#if LIGHT_TYPE_CT_EN
static int llsync_mesh_color_temperature_set(const char *data, uint16_t len)
{
    memcpy(&sg_mesh_property_data.mesh_color_temperature.color_temperature, data, len);
    sg_mesh_property_data.mesh_color_temperature.color_temperature =
        HTONS(sg_mesh_property_data.mesh_color_temperature.color_temperature);

	int ele_addr_offset = ELE_CNT_EVERY_LIGHT > 1 ? 1 : 0;
	access_cmd_set_light_ctl_temp(ele_adr_primary + ele_addr_offset, 0, sg_mesh_property_data.mesh_color_temperature.color_temperature, 0, 0);
	
    return 0;
}

static int llsync_mesh_color_temperature_get(char *data, uint16_t len)
{
	mesh_cmd_light_ctl_temp_st_t rsp = {0};
	mesh_light_ctl_temp_st_rsp_par_fill(&rsp, 0);
	sg_mesh_property_data.mesh_color_temperature.color_temperature = rsp.target_temp;

	llsync_mesh_color_temperature color_temp_big;
    color_temp_big.color_temperature = HTONS(sg_mesh_property_data.mesh_color_temperature.color_temperature);
    memcpy(data, &color_temp_big, min(sizeof(color_temp_big), len));
    return sizeof(color_temp_big);
}
#endif

ble_property_t sg_ble_property_array[] = { // 6
    {llsync_mesh_onoff_set, llsync_mesh_onoff_get, LLSYNC_MESH_VENDOR_ONOFF_TYPE, sizeof(llsync_mesh_onoff)},
    {llsync_mesh_unix_time_set, llsync_mesh_unix_time_get, LLSYNC_MESH_VENDOR_UNIX_TIME_TYPE,
     sizeof(llsync_mesh_unix_time)},
    {llsync_mesh_power_percent_set, llsync_mesh_power_percent_get, LLSYNC_MESH_VENDOR_POWER_PERCENT_TYPE,
     sizeof(llsync_mesh_power_percent)},
#if LIGHT_TYPE_HSL_EN
    {llsync_mesh_HSL_set, llsync_mesh_HSL_get, LLSYNC_MESH_VENDOR_HSL_TYPE, sizeof(llsync_mesh_HSL)},
#endif
#if MD_LIGHTNESS_EN
    {llsync_mesh_lightness_level_set, llsync_mesh_lightness_level_get, LLSYNC_MESH_VENDOR_LIGHTNESS_LEVEL_TYPE,
     sizeof(llsync_mesh_lightness_level)},
#endif
#if LIGHT_TYPE_CT_EN
    {llsync_mesh_color_temperature_set, llsync_mesh_color_temperature_get, LLYNSC_MESH_VENDOR_COLOR_TEMPRETURE_TYPE,
     sizeof(llsync_mesh_color_temperature)},
#endif
};

uint16_t llsync_mesh_vendor_size_get(void)
{
    return sizeof(sg_ble_property_array) / sizeof(sg_ble_property_array[0]);
}

ble_property_t *llsync_mesh_property_array_get(void)
{
    return sg_ble_property_array;
}
#endif

#ifdef __cplusplus
}
#endif
