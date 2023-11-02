/********************************************************************************************************
 * @file	mesh_log.c
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
#include "proj/tl_common.h"
#include "proj/common/tstring.h"
#if !WIN32
#include "proj/mcu/watchdog_i.h"
#endif 
#include "myprintf.h"
#include "proj_lib/ble/ll/ll.h"
#include "proj_lib/ble/blt_config.h"
#include "vendor/common/user_config.h"
#include "proj_lib/ble/service/ble_ll_ota.h"
#include "app_health.h"
#include "proj_lib/sig_mesh/app_mesh.h"
#include "app_provison.h"
#include "lighting_model.h"
#include "vendor/common/sensors_model.h"
#include "vendor/common/remote_prov.h"
#include "proj_lib/mesh_crypto/sha256_telink.h"
#include "proj_lib/mesh_crypto/le_crypto.h"
#include "lighting_model_LC.h"
#include "mesh_ota.h"
#include "mesh_common.h"
#include "mesh_config.h"
#include "directed_forwarding.h"

#if (HCI_ACCESS==HCI_USE_UART)
#include "proj/drivers/uart.h"
#endif


/**
  * @}
  */

const char OP_STRING_NOT_FOUND[] = {"?"};
#if LOG_FW_FUNC_EN
#if 1
const char * get_op_string_aligned(const char *p_str_ret)
{
	static char s_op_string_buff[40] = {0};
	#define OP_STRING_BUFF_LEN_MAX		(sizeof(s_op_string_buff) - 1)
	#define OP_STRING_BUFF_LEN_ALIGN	(16) // for "LIGHTNESS_STATUS"
	if(p_str_ret){
		int str_len_log = min(strlen(p_str_ret), OP_STRING_BUFF_LEN_MAX);
		memcpy(s_op_string_buff, p_str_ret, str_len_log);
		
		if((OP_STRING_BUFF_LEN_MAX >= OP_STRING_BUFF_LEN_ALIGN) && (OP_STRING_BUFF_LEN_ALIGN > str_len_log)){
			memset(s_op_string_buff + str_len_log, ' ', OP_STRING_BUFF_LEN_ALIGN - str_len_log);
			s_op_string_buff[OP_STRING_BUFF_LEN_ALIGN] = '\0';
		}else{
			s_op_string_buff[OP_STRING_BUFF_LEN_MAX] = '\0';
		}
		p_str_ret = s_op_string_buff;
	}

	return p_str_ret;
}

const char * get_op_string(u16 op, const char *str_in)
{
	const char *p_str_ret = 0;
	if(str_in){
		p_str_ret = str_in;
	}else{
		mesh_op_resource_t op_res;
		if(0 == mesh_search_model_id_by_op(&op_res, op, 1)){
			p_str_ret = op_res.op_string;
		}
	}

	#if 0 // WIN32	// aligned
	if(p_str_ret){
		p_str_ret = get_op_string_aligned(p_str_ret);
	}
	#endif
	
	return p_str_ret ? p_str_ret : OP_STRING_NOT_FOUND;
}
#else
typedef struct{
	u16 op;
	char string[20];
}mesh_op_string_t;

const mesh_op_string_t mesh_op_string[] = {
    {HEALTH_FAULT_GET, 			"HEALTH_FAULT_GET  "},
    {HEALTH_FAULT_STATUS, 		"HEALTH_FAULT_ST   "},
    // ---- generic ----
    {G_ONOFF_GET, 				"ONOFF GET         "},
    {G_ONOFF_SET, 				"ONOFF SET         "},
    {G_ONOFF_SET_NOACK, 		"ONOFF SET NACK    "},
    {G_ONOFF_STATUS, 			"ONOFF ST          "},
    // ---- lightness ----
    {LIGHTNESS_GET, 			"LIGHTNESS GET     "},
    {LIGHTNESS_SET, 			"LIGHTNESS SET     "},
    {LIGHTNESS_SET_NOACK, 		"LIGHTNESS SET NACK"},
    {LIGHTNESS_STATUS, 			"LIGHTNESS ST      "},
#if LIGHT_TYPE_CT_EN
    // ---- CTL ----
    {LIGHT_CTL_GET, 			"CTL GET           "},
    {LIGHT_CTL_SET, 			"CTL SET           "},
    {LIGHT_CTL_SET_NOACK, 		"CTL SET_NACK      "},
    {LIGHT_CTL_STATUS, 			"CTL ST            "},
    {LIGHT_CTL_TEMP_GET, 		"TEMP GET          "},
    {LIGHT_CTL_TEMP_SET, 		"TEMP SET          "},
    {LIGHT_CTL_TEMP_SET_NOACK, 	"TEMP SET NACK     "},
    {LIGHT_CTL_TEMP_STATUS, 	"TEMP ST           "},
#endif
#if LIGHT_TYPE_HSL_EN
    // ---- HSL ----
    {LIGHT_HSL_GET, 			"HSL GET           "},
    {LIGHT_HSL_HUE_GET, 		"HSL HUE GET       "},
    {LIGHT_HSL_HUE_SET, 		"HSL HUE SET       "},
    {LIGHT_HSL_HUE_SET_NOACK, 	"HSL HUE SET NACK  "},
    {LIGHT_HSL_HUE_STATUS, 		"HSL HUE ST        "},
    {LIGHT_HSL_SAT_GET, 		"HSL SAT GET       "},
    {LIGHT_HSL_SAT_SET, 		"HSL SAT SET       "},
    {LIGHT_HSL_SAT_SET_NOACK, 	"HSL SAT SET NACK  "},
    {LIGHT_HSL_SAT_STATUS, 		"HSL SAT ST        "},
    {LIGHT_HSL_SET, 			"HSL SET           "},
    {LIGHT_HSL_SET_NOACK, 		"HSL SET NACK      "},
    {LIGHT_HSL_STATUS, 			"HSL ST            "},
#endif
};

const char * get_op_string(u16 op, const char *str_in)
{
	foreach_arr(i, mesh_op_string){
		const mesh_op_string_t *p = &mesh_op_string[i];
		if(p->op == op){
			return p->string;
		}
	}
	return OP_STRING_NOT_FOUND;
}
#endif


#else
const char * get_op_string(u16 op, const char *str_in)
{
	return OP_STRING_NOT_FOUND;
}
#endif	

/**
  * @}
  */


