/********************************************************************************************************
 * @file	lighting_model.c
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
#include "proj_lib/ble/blt_config.h"
#include "vendor/common/user_config.h"
#include "app_health.h"
#include "proj_lib/sig_mesh/app_mesh.h"
#include "scene.h"
#include "lighting_model_LC.h"
#include "lighting_model.h"

/** @addtogroup Mesh_Common
  * @{
  */

/** @defgroup Lighting_Model
  * @brief Lighting Models Code.
  * @{
  */



model_lightness_t       model_sig_lightness;
#if (LIGHT_TYPE_CT_EN)
model_light_ctl_t       model_sig_light_ctl;
#endif

/*
	lighting model command callback function ----------------
*/

/**
 * @brief       This function square root
 * @param[in]   x	- parameter
 * @return      square root of x
 * @note        
 */
u16 mySqrt(u32 x) 
{
   if(x == 0 || x== 1)
        return x;
   //special dispatch for the 65535 val 
   u32 magic_num=0;
   magic_num = (u32)65535*65535;
   if(x >= magic_num){
		return 65535;
   }
   u32 start = 0;
   u32 end = 65535;
   while(start < end)
   {
       u32 mid = start + (end-start)/2;
       u32 res = mid*mid;
       if(res == x)
         return mid;
       else if(res > x)
         end = mid;
       else 
         start = mid;
       if(end -start == 1)
       break;    
    }
    return start;  
}

/**
 * @brief       This function lightness switch to linear
 * @param[in]   val	- lightness value
 * @return      linear
 * @note        
 */
u16 lightness_to_linear(u16 val)
{
	u32 linear =0;
	u32 linear_temp = 0;
	linear_temp = val*val;
	linear = (linear_temp)/65535;
	if((linear_temp)%65535){
		linear++;
	}
	return linear;
}

/**
 * @brief       This function linear switch to lightness
 * @param[in]   val	- linear value
 * @return      lightness
 * @note        
 */
u16 linear_to_lightness(u16 val)
{
	u16 lightness =0;
	u32 sqrt_val = 0;
	sqrt_val = val*65535;
	lightness = mySqrt(sqrt_val);
	return lightness;	
}

/**
 * @brief       This function check opcode is set level
 * @param[in]   op	- opcode
 * @return      0:not; 1:yes
 * @note        
 */
int is_level_set_op(u16 op)
{
	return ((op == G_LEVEL_SET) || (op == G_LEVEL_SET_NOACK));
}

/**
 * @brief       This function check opcode is set level delta
 * @param[in]   op	- opcode
 * @return      0:not; 1:yes
 * @note        
 */
int is_level_delta_set_op(u16 op)
{
	return ((op == G_DELTA_SET) || (op == G_DELTA_SET_NOACK));
}

/**
 * @brief       This function check opcode is set level move
 * @param[in]   op	- opcode
 * @return      0:not; 1:yes
 * @note        
 */
int is_level_move_set_op(u16 op)
{
	return ((op == G_MOVE_SET) || (op == G_MOVE_SET_NOACK));
}

/**
 * @brief       This function check opcode is set lightness
 * @param[in]   op	- opcode
 * @return      0:not; 1:yes
 * @note        
 */
int is_lightness_set_op(u16 op)
{
	return ((op == LIGHTNESS_SET) || (op == LIGHTNESS_SET_NOACK));
}

/**
 * @brief       This function check opcode is set light color temperature
 * @param[in]   op	- opcode
 * @return      0:not; 1:yes
 * @note        
 */
int is_light_ctl_set_op(u16 op)
{
	return ((op == LIGHT_CTL_SET) || (op == LIGHT_CTL_SET_NOACK));
}

/**
 * @brief       This function check if opcode is dim to dark.
 * @param[in]   op	- opcode
 * @return      0:not; 1:yes
 * @note        
 */
int is_dim2dark_set_op(u16 op)
{
	// (!(is_level_delta_set_op(op)||is_level_move_set_op(op)||is g onoff || is lc onoff || any other ?))
	return (is_level_set_op(op) || is_lightness_set_op(op)); //
}

#if MD_SERVER_EN
#if WIN32
/**
 * @brief       This function refresh level of UI for VC(sig_mesh_tool)
 * @param[in]   cb_par	- parameter data
 * @return      none
 * @note        
 */
void VC_RefreshUI_level(mesh_cb_fun_par_t *cb_par)
{
	// refresh UI
	model_g_light_s_t *p_model = (model_g_light_s_t *)cb_par->model;
	mesh_rc_rsp_t rsp = {0};
	rsp.src = p_model->com.ele_adr;
	rsp.dst = cb_par->adr_src;
	rsp.data[0] = G_LEVEL_STATUS & 0xff;
	rsp.data[1] = (G_LEVEL_STATUS >> 8) & 0xff;
	mesh_g_level_st_rsp_par_fill((mesh_cmd_g_level_st_t *)(&rsp.data[2]), cb_par->model_idx);
	RefreshStatusNotifyByHw((u8 *)(&rsp.src), 6 + sizeof(mesh_cmd_g_level_st_t)); // 6: src + dst + op
}
#endif

/**
 * @brief       This function get target level for move opcode
 * @param[in]   move_delta	- level
 * @param[in]   min			- level min of light_res_sw_save
 * @param[in]   max			- level max of light_res_sw_save
 * @return      target level
 * @note        
 */
static inline s16 get_target_level_for_move_set_with_check_range(s16 move_delta, s16 min, s16 max)
{
	return (move_delta >= 0) ? max : min;	// not allow wrap-around as default. // follow new rule of dim to dark.
}

/**
 * @brief       This function get target level by opcode
 * @param[in]   target_level	- target level
 * @param[in]   level			- light set level
 * @param[in]   op				- opcode
 * @param[in]   light_idx		- light index if "LIGHT CNT > 1", or it is always 0.
 * @param[in]   st_trans_type	- A value in the enumeration type ST_TRANS_TYPE.
 * @return      target level
 * @note        
 */
s16 get_target_level_by_op(s16 target_level, s32 level, u16 op, int light_idx, int st_trans_type)
{
	light_range_s16_t range_s16 = {0};
	light_g_level_range_get(&range_s16, light_idx, st_trans_type);
    if(is_level_delta_set_op(op)){
        return get_val_with_check_range(target_level + level, range_s16.min, range_s16.max, st_trans_type, op);
    }else if(is_level_move_set_op(op)){
        return get_target_level_for_move_set_with_check_range(level, range_s16.min, range_s16.max);
    }else{  // ((G_LEVEL_SET == op) || (G_LEVEL_SET_NOACK == op))
        return level;
    }
}

/**
 * @brief       This function set publish parameters to trigger publish flow.
 * @param[in]   level_set_st- level set publish state
 * @param[in]   light_idx	- light index if "LIGHT CNT > 1", or it is always 0.
 * @param[in]   model_id	- model id
 * @return      none
 * @note        
 */
static void model_pub_check_set_onoff(int level_set_st, int light_idx, u32 model_id)
{
    model_pub_check_set(level_set_st, (u8 *)(&(model_sig_g_onoff_level.onoff_srv[light_idx])), SIG_MD_G_ONOFF_S == model_id);
#if MD_LIGHT_CONTROL_EN
    model_pub_check_set(level_set_st, (u8 *)(&(model_sig_light_lc.srv[light_idx])), set_LC_lightness_flag);
#endif
}

/**
 * @brief       This function check all bind model of list and set publish step
 * @param[in]   pub_list- publish list
 * @param[in]   cb_par	- callback parameter
 * @param[in]   linear	- linear
 * @return      none
 * @note        
 */
void model_pub_check_set_bind_all(st_pub_list_t *pub_list, mesh_cb_fun_par_t *cb_par, int linear)
{
    mesh_op_resource_t *p_res = (mesh_op_resource_t *)cb_par->res;
    int light_idx = 0;
    if(SIG_MD_G_LEVEL_S == p_res->id){
        light_idx = get_light_idx_from_level_md_idx(cb_par->model_idx);
    }else{
        light_idx = cb_par->model_idx;
    }
    
    if((!PTS_TEST_EN) && pub_list->st[ST_TRANS_PUB_ONOFF]){
        model_pub_check_set_onoff(pub_list->st[ST_TRANS_PUB_ONOFF], light_idx, p_res->id);
    }
    
    if(pub_list->st[ST_TRANS_LIGHTNESS]){
#if MD_LIGHTNESS_EN
        int level_set_st = pub_list->st[ST_TRANS_LIGHTNESS];
        if(PTS_TEST_EN){    // PTS LLN-BV09C: request publish onoff status, even though onoff not change.
            model_pub_check_set_onoff(pub_list->st[ST_TRANS_PUB_ONOFF], light_idx, p_res->id);
        }
        model_sig_lightness.srv[light_idx].com.pub_2nd_state = linear;
        model_pub_check_set(level_set_st, (u8 *)(&(model_sig_lightness.srv[light_idx])), SIG_MD_LIGHTNESS_S == p_res->id);
#endif
#if MD_LIGHT_CONTROL_EN // if light control is enable, lightness must be enable.
        model_pub_check_set(level_set_st, (u8 *)(&(model_sig_light_lc.srv[light_idx])), (0 == set_LC_lightness_flag) && (SIG_MD_LIGHT_LC_S == p_res->id));
        if(is_light_lc_onoff(cb_par->op)){
            model_pub_st_cb_re_init_lc_srv(&mesh_lc_onoff_st_publish);    // re-init
        }
#endif
    }
    
#if MD_LEVEL_EN
    foreach(m,LIGHT_CNT){
        if(m == light_idx){
            foreach(i,LEVEL_STATE_CNT_EVERY_LIGHT){
                if(pub_list->st[i]){
                    int level_md_idx = m * LEVEL_STATE_CNT_EVERY_LIGHT + i;
            	    model_pub_check_set(pub_list->st[i], (u8 *)(&(model_sig_g_onoff_level.level_srv[level_md_idx])), SIG_MD_G_LEVEL_S == p_res->id);
                }
            }
            break;
        }
    }
#endif

#if LIGHT_TYPE_CT_EN
    if(pub_list->st[ST_TRANS_LIGHTNESS] || pub_list->st[ST_TRANS_CTL_TEMP]){
        int level_set_st = get_level_set_st(pub_list->st[ST_TRANS_LIGHTNESS], pub_list->st[ST_TRANS_CTL_TEMP]);
	    model_pub_check_set(level_set_st, (u8 *)(&(model_sig_light_ctl.srv[light_idx])), SIG_MD_LIGHT_CTL_S == p_res->id);    
    }
    if(pub_list->st[ST_TRANS_CTL_TEMP] || pub_list->st[ST_TRANS_CTL_D_UV]){
        int level_set_st = get_level_set_st(pub_list->st[ST_TRANS_CTL_TEMP], pub_list->st[ST_TRANS_CTL_D_UV]);
	    model_pub_check_set(level_set_st, (u8 *)(&(model_sig_light_ctl.temp[light_idx])), SIG_MD_LIGHT_CTL_TEMP_S == p_res->id);    
    }
#endif

#if LIGHT_TYPE_HSL_EN
    if(pub_list->st[ST_TRANS_LIGHTNESS] || pub_list->st[ST_TRANS_HSL_HUE] || pub_list->st[ST_TRANS_HSL_SAT]){
        int level_set_st = get_level_set_st(pub_list->st[ST_TRANS_LIGHTNESS], pub_list->st[ST_TRANS_HSL_HUE]);
        level_set_st = get_level_set_st(level_set_st, pub_list->st[ST_TRANS_HSL_SAT]);
	    model_pub_check_set(level_set_st, (u8 *)(&(model_sig_light_hsl.srv[light_idx])), SIG_MD_LIGHT_HSL_S == p_res->id);    
    }
    if(pub_list->st[ST_TRANS_HSL_HUE]){
	    model_pub_check_set(pub_list->st[ST_TRANS_HSL_HUE], (u8 *)(&(model_sig_light_hsl.hue[light_idx])), SIG_MD_LIGHT_HSL_HUE_S == p_res->id);    
    }
    if(pub_list->st[ST_TRANS_HSL_SAT]){
	    model_pub_check_set(pub_list->st[ST_TRANS_HSL_SAT], (u8 *)(&(model_sig_light_hsl.sat[light_idx])), SIG_MD_LIGHT_HSL_SAT_S == p_res->id);    
    }
#endif

#if (LIGHT_TYPE_SEL == LIGHT_TYPE_XYL)
    if(pub_list->st[ST_TRANS_LIGHTNESS] || pub_list->st[ST_TRANS_XYL_X] || pub_list->st[ST_TRANS_XYL_Y]){
        int level_set_st = get_level_set_st(pub_list->st[ST_TRANS_LIGHTNESS], pub_list->st[ST_TRANS_XYL_X]);
        level_set_st = get_level_set_st(level_set_st, pub_list->st[ST_TRANS_XYL_Y]);
	    model_pub_check_set(level_set_st, (u8 *)(&(model_sig_light_xyl.srv[light_idx])), SIG_MD_LIGHT_XYL_S == p_res->id);    
    }
#endif
}

/**
 * @brief  Set Generic Level States.
 * @param  par: Pointer to the set parameter structure
 *   (mesh_cmd_g_level_delta_t or mesh_cmd_g_level_set_t).
 * @param  par_len: The length of the parameter p_set.
 * @param  op: Opcode.
 * @param  idx: index of Light Count.
 * @param  retransaction: Retransmission flag.
 *     @arg 0: Non-retransmission.
 *     @arg 1: Retransmission.
 * @param  force: Only for G_LEVEL_SET_NOACK opcode. Force to set the 
 *   general level.
 *     @arg 0: Set the general level if needed.
 *     @arg 1: Force to set the general level.
 * @param  pub_list: list of publication.
 * @retval Whether the function executed successfully
 *   (0: success; others: error)
 */
int g_level_set(u8 *par, int par_len, u16 op, int idx, bool4 retransaction, int st_trans_type, int force, st_pub_list_t *pub_list)
{
    //model_g_light_s_t *p_model = (model_g_light_s_t *)cb_par->model;
	mesh_set_trans_t set_trans = {0};
	mesh_set_trans_t *p_set_trans = 0;

	s32 delta_offset = 0;
	set_trans.op = op;
	int is_move_flag = (is_level_move_set_op(op));
	
	s32 level_adjust;
	if(is_level_delta_set_op(set_trans.op)){
		mesh_cmd_g_level_delta_t *p_set = (mesh_cmd_g_level_delta_t *)par;
		static s32 delta_last = 0;
		if(retransaction){
			delta_offset = p_set->level - delta_last;   // delta may be different from last,so update too.
			level_adjust = delta_offset;
		}else{
			level_adjust = p_set->level;
		}
		delta_last = p_set->level;
		
		if(par_len >= sizeof(mesh_cmd_g_level_delta_t)){	// existed transit_t and delay.
			if(!is_valid_transition_step(p_set->transit_t)){
				return -1;
			}
			set_trans.transit_t = p_set->transit_t;
			set_trans.delay = p_set->delay;
			p_set_trans = &set_trans;
		}
	}else{ // level set / level move
		mesh_cmd_g_level_set_t *p_set = (mesh_cmd_g_level_set_t *)par;
		if(is_move_flag && (0 == p_set->level)){
			// stop move set changing the Generic Level state // refer to behavior of "3.3.2.2.4 Receiving Generic Move Set / Generic Move Set Unacknowledged messages"
			light_transition_proc_stop(idx, st_trans_type);
		    return 0;
		}
		level_adjust = p_set->level;
        set_trans.transit_t = p_set->transit_t;
		
		if(par_len >= sizeof(mesh_cmd_g_level_set_t)){	// existed transit_t and delay.
			if(!is_valid_transition_step(p_set->transit_t)){
				return -1;
			}
			set_trans.delay = p_set->delay;
			p_set_trans = &set_trans;
			
			if(is_move_flag){
				if(0 == GET_TRANSITION_STEP(p_set->transit_t)){
			    	return 0; //  refer to spec "move set" behavior.
			    }
			}
		}else if(is_move_flag){
			// return 0; // support no transition time filed. // refer to spec of "move set" behavior and "3.2Generic messages" -> 3.2.2.6Generic Move Set.
		}
	}

	if(p_set_trans){
		if(!is_transition_need(p_set_trans->transit_t, p_set_trans->delay)){
			p_set_trans = 0;
		}
	}else{
		u8 def_transit_t = g_def_trans_time_val(idx);
		if(GET_TRANSITION_STEP(def_transit_t)){
			set_trans.transit_t = def_transit_t;
			set_trans.delay = 0;
			par_len = sizeof(mesh_cmd_g_level_delta_t);
			p_set_trans = &set_trans;
		}
	}

	s16 target_level = light_g_level_target_get(idx, st_trans_type);
	set_trans.target_val = get_target_level_by_op(target_level, level_adjust, set_trans.op, idx, st_trans_type);

	#if 1 // dim to dark handle
	if(p_set_trans && ST_TRANS_LIGHTNESS == st_trans_type){
		s16 present_1 = light_g_level_present_get(idx, st_trans_type);
		if((LEVEL_OFF == present_1) && (present_1 != p_set_trans->target_val)){
			light_range_s16_t range_s16 = {0};
			light_g_level_range_get(&range_s16, idx, st_trans_type);
			if((range_s16.min > LEVEL_OFF + 1) && (p_set_trans->target_val >= range_s16.min)){
				light_res_sw_g_level_set(idx, range_s16.min, 0, st_trans_type);
				p_set_trans->dim2dark_delay_ms = get_transition_100ms((trans_time_t *)&p_set_trans->transit_t) * 100 * s16_to_u16(range_s16.min) / s16_to_u16(p_set_trans->target_val);
				//LOG_MSG_LIB(TL_LOG_NODE_BASIC,0,0,"dim2dark delay ms: %d", p_set_trans->dim2dark_delay_ms);
			}
		}
	}
	#endif
	
	s16 present_level = light_g_level_present_get(idx, st_trans_type);
    set_trans.present_val = present_level;
    set_trans.level_move = level_adjust;
    if(!retransaction || delta_offset){
    	// if no set_LC_lightness_flag, LC will not be triggered from "fade on" to "run" state, because of no transition procedure.
		if(p_set_trans && ((present_level != set_trans.target_val) || force
							#if (MD_LIGHT_CONTROL_EN)
							|| set_LC_lightness_flag
							#endif
			)){ // force only for G_LEVEL_SET_NOACK
			light_g_level_set_idx_with_trans((u8 *)p_set_trans, idx, st_trans_type, pub_list->hsl_set_cmd_flag);
			// no need publish at start time now.
            get_light_pub_list(st_trans_type, present_level, set_trans.target_val, 1, pub_list);
		}else{
		    #if MD_SCENE_EN
		    if(!tansition_forced_by_recall_flag){
			    scene_target_complete_check(idx);
			}
			#endif
			
			if(is_move_flag){
			    // no action if no transition time.
			}else{
        		light_g_level_set_idx(idx, set_trans.target_val, 1, st_trans_type, pub_list);
        	}
        }

        #if (MD_LIGHT_CONTROL_EN && MD_SERVER_EN)
        LC_state_check_and_clear_by_user_command(idx);
        #endif
    }

	return 0;
}

/**
 * @brief       This function get u16 lightness and get remain time of transition.
 * @param[in]   idx				- light index if "LIGHT CNT > 1", or it is always 0.
 * @param[in]   st_trans_type	- A value in the enumeration type ST_TRANS_TYPE.
 * @param[out]  p_remain_t		- remain time
 * @param[in]   target_flag		- flag of getting target level or present level
 * @return      lightness
 * @note        
 */
u16 get_u16_val_and_remain_time(u8 idx, int st_trans_type, u8 *p_remain_t, int target_flag)
{
	mesh_cmd_g_level_st_t level_st; 
	light_g_level_get((u8 *)&level_st, idx, st_trans_type);
    if(0 == *p_remain_t){
	    *p_remain_t = level_st.remain_t;
	}
	return get_lightness_from_level(target_flag ? level_st.target_level : level_st.present_level);
}

/**
 * @brief       This function get level target and make sure it is in range of min and max.
 * @param[in]   level_target	- level target
 * @param[in]   min				- level min
 * @param[in]   max				- level max
 * @param[in]   st_trans_type	- A value in the enumeration type ST_TRANS_TYPE.
 * @return      level target
 * @note        
 */
s16 get_val_with_check_range(s32 level_target, s16 min, s16 max, int st_trans_type, u16 op)
{
#if (LIGHT_TYPE_CT_EN)
    if(ST_TRANS_CTL_TEMP == st_trans_type){
        // level value is always valid for CT. 
        min = -32768;
        max = 32767;
    }
#endif
    if(level_target < min){
        // lightness would be set to 0 for PTS, which is like to OFF command, and 0 will not be allowed to set to lightness->last.
        if((ST_TRANS_LIGHTNESS == st_trans_type) && (LEVEL_OFF == level_target) && is_dim2dark_set_op(op)){
            //level_target = LEVEL_OFF; // level_target has been LEVEL_OFF.
        }else{
            level_target = min;
        }
    }else if(level_target > max){
        level_target = max;
    }
    return (s16)level_target;
}

/**
 * @brief  Set Generic Level States.
 * @param  par: Pointer to the set parameter structure.
 * @param  par_len: The length of the parameter p_set.
 * @param  op: Opcode.
 * @param  idx: index of Light Count.
 * @param  retransaction: Retransmission flag.
 *     @arg 0: Non-retransmission.
 *     @arg 1: Retransmission.
 * @param  st_trans_type: A value in the enumeration type ST_TRANS_TYPE.
 * @param  force: Only for G_LEVEL_SET_NOACK opcode. Force to set the 
 *   general level.
 * @param  pub_list: list of publication.
 * @retval Whether the function executed successfully
 *   (0: success; others: error)
 */
int g_level_set_and_update_last(u8 *par, int par_len, u16 op, int idx, bool4 retransaction, int st_trans_type, int force, st_pub_list_t *pub_list)
{
	mesh_cmd_g_level_delta_t set = {0};
	memcpy(&set, par, sizeof(set));
	light_range_s16_t range;
	light_g_level_range_get(&range, idx, st_trans_type);
	if(!is_level_delta_set_op(op) && !is_level_move_set_op(op)){
		mesh_cmd_g_level_set_t *p_level_set = (mesh_cmd_g_level_set_t *)&set;
		p_level_set->level = get_val_with_check_range(p_level_set->level, range.min, range.max, st_trans_type, op);
	}
	
	int err = g_level_set((u8 *)&set, par_len, op, idx, retransaction, st_trans_type, force, pub_list);
    if(!err){
	    light_res_sw_g_level_last_set(idx, st_trans_type);
	}
	return err;
}

// ----------- lightness-------------
/**
 * @brief       This function get if lightness value is valid 
 * @param[in]   lightness	- lightness value 
 * @return      1
 * @note        
 */
static inline int is_valid_lightness(u16 lightness)
{
	return 1;//(lightness != 0);
}

/**
 * @brief       This function get level with u16 format.
 * @param[out]  rsp				- pointer to response data
 * @param[in]   idx				- light index if "LIGHT CNT > 1", or it is always 0.
 * @param[in]   st_trans_type	- A value in the enumeration type ST_TRANS_TYPE.
 * @return      none
 * @note        
 */
void mesh_level_u16_st_rsp_par_fill(mesh_cmd_lightness_st_t *rsp, u8 idx, int st_trans_type)
{
	mesh_cmd_g_level_st_t level_st; 
	light_g_level_get((u8 *)&level_st, idx, st_trans_type);

	rsp->present = get_lightness_from_level(level_st.present_level);
	rsp->target = get_lightness_from_level(level_st.target_level);
	rsp->remain_t = level_st.remain_t;
}

/**
 * @brief       This function tx lightness status
 * @param[in]   idx		- light index if "LIGHT CNT > 1", or it is always 0.
 * @param[in]   ele_adr	- element address
 * @param[in]   dst_adr	- destination address
 * @param[in]   op_rsp	- opcode response
 * @param[in]   uuid	- if destination address is virtual address, it is the Label UUID of it. if not virtual address, set to NULL.
 * @param[in]   pub_md	- publish model
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int mesh_tx_cmd_lightness_st(u8 idx, u16 ele_adr, u16 dst_adr, u16 op_rsp, u8 *uuid, model_common_t *pub_md)
{
	mesh_cmd_lightness_st_t rsp = {0};
	mesh_level_u16_st_rsp_par_fill(&rsp, idx, ST_TRANS_LIGHTNESS);

#if CMD_LINEAR_EN	 				 
	if(LIGHTNESS_LINEAR_STATUS == op_rsp){
		if(is_linear_flag(idx)){
			u16 linear_present = lightness_to_linear(rsp.present);
			rsp.target = get_light_linear_val(idx);
			if(linear_present <= rsp.target){
				rsp.present = linear_present;
			}else{
				rsp.present = rsp.target;
			}
		}else{
			rsp.present = lightness_to_linear(rsp.present);
	 		rsp.target = lightness_to_linear(rsp.target);
		}
	}
#endif	
#if LS_TEST_ENABLE
	rsp.present = rsp.present/655;
	rsp.target = rsp.target/655;
#endif
	u32 len = sizeof(rsp);
	if(0 == rsp.remain_t){
		len -= 3;
	}

#if LIGHTNESS_DATA_REFORMAT_EN
    lightness_rsp_data_reformat(&rsp);
#endif
	return mesh_tx_cmd_rsp(op_rsp, (u8 *)&rsp, len, ele_adr, dst_adr, uuid, pub_md);
}

#if MD_LIGHTNESS_EN
/**
 * @brief       This function publish lightness status
 * @param[in]   idx		- light index if "LIGHT CNT > 1", or it is always 0.
 * @param[in]   op_rsp	- opcode response
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int mesh_lightness_st_publish_ll(u8 idx, u16 op_rsp)
{
	model_common_t *p_com_md = &model_sig_lightness.srv[idx].com;
	u16 ele_adr = p_com_md->ele_adr;
	u16 pub_adr = p_com_md->pub_adr;
	if(!pub_adr){
		return -1;
	}
	u8 *uuid = get_virtual_adr_uuid(pub_adr, p_com_md);
	#if 0 // PTS_TEST_EN,  fix SR/LLN/BV-06 in pts7_3_0.exe
	mesh_tx_cmd_lightness_st(idx, ele_adr, pub_adr, op_rsp, uuid, p_com_md);
	p_com_md->pub_2nd_state = 0;     // init
	op_rsp = LIGHTNESS_LINEAR_STATUS;
	return mesh_tx_cmd_lightness_st(idx, ele_adr, pub_adr, op_rsp, uuid, p_com_md);
	#else
	if(p_com_md->pub_2nd_state){
        p_com_md->pub_2nd_state = 0;     // init
        #if 0 // always publish lightness is OK in pts7_3_1.exe
	    op_rsp = LIGHTNESS_STATUS;
	    #endif
	}
	return mesh_tx_cmd_lightness_st(idx, ele_adr, pub_adr, op_rsp, uuid, p_com_md);
	#endif
	
}

/**
 * @brief       This function publish lightness status
 * @param[in]   idx	- light index if "LIGHT CNT > 1", or it is always 0.
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int mesh_lightness_st_publish(u8 idx)
{
	return mesh_lightness_st_publish_ll(idx, LIGHTNESS_STATUS);
}
#endif

/**
 * @brief       This function tx response with lightness status.
 * @param[in]   cb_par	- callback parameter
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int mesh_lightness_st_rsp(mesh_cb_fun_par_t *cb_par)
{
	model_g_light_s_t *p_model = (model_g_light_s_t *)cb_par->model;
	return mesh_tx_cmd_lightness_st(cb_par->model_idx, p_model->com.ele_adr, cb_par->adr_src, cb_par->op_rsp, 0, 0);
}

/**
 * @brief  When the Light Lightness Get message is received, this function 
 *   will be called
 * @param  par: Pointer to message data (excluding Opcode).
 * @param  par_len: The length of the message data.
 * @param  cb_par: Some parameters about function callbacks.
 * @retval Whether the function executed successfully
 *   (0: success; others: error)
 */
int mesh_cmd_sig_lightness_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	return mesh_lightness_st_rsp(cb_par);
}

/**
 * @brief       This function Set level.
 * @param[in]   p_set			- Pointer to the set parameter structure.
 * @param[in]   par_len			- The length of the parameter p_set.
 * @param[in]   op				- opcode
 * @param[in]   idx				- index of Light Count.
 * @param[in]   retransaction	- Retransmission flag.
 * @param[in]   st_trans_type	- A value in the enumeration type ST_TRANS_TYPE.
 * @param[in]   pub_list		- list of publish.
 * @return      0:set fail; 1:set success
 * @note        
 */
int level_s16_set(mesh_cmd_g_level_set_t *p_set, int par_len, u16 op, int idx, bool4 retransaction, int st_trans_type, st_pub_list_t *pub_list)
{
	return g_level_set_and_update_last((u8 *)&p_set, par_len, G_LEVEL_SET_NOACK, idx, retransaction, st_trans_type, 0, pub_list);
}

/**
 * @brief       This function  Set Generic Level States.
 * @param[in]   p_set			- Pointer to the set parameter structure.
 * @param[in]   par_len			- The length of the parameter p_set.
 * @param[in]   op				- opcode
 * @param[in]   idx				- index of Light Count.
 * @param[in]   retransaction	- Retransmission flag.
 * @param[in]   st_trans_type	- A value in the enumeration type ST_TRANS_TYPE.
 * @param[in]   pub_list		- list of publication.
 * @return      0:set fail; 1:set success
 * @note        
 */
int level_u16_set(mesh_cmd_lightness_set_t *p_set, int par_len, u16 op, int idx, bool4 retransaction, int st_trans_type, st_pub_list_t *pub_list)
{
	mesh_cmd_g_level_set_t level_set_tmp;
	memcpy(&level_set_tmp.tid, &p_set->tid, sizeof(mesh_cmd_g_level_set_t) - OFFSETOF(mesh_cmd_g_level_set_t,tid));
	level_set_tmp.level = get_level_from_lightness(p_set->lightness);
	int len_tmp = GET_LEVEL_PAR_LEN(par_len >= sizeof(mesh_cmd_lightness_set_t));
	return g_level_set_and_update_last((u8 *)&level_set_tmp, len_tmp, G_LEVEL_SET_NOACK, idx, retransaction, st_trans_type, 0, pub_list);
}

/**
 * @brief  Set the lightness of the light.
 * @param  p_set: Pointer to the set parameter structure
 *   (mesh_cmd_lightness_set_t).
 * @param  par_len: The length of the parameter p_set.
 * @param  op: Opcodes.
 * @param  idx: index of Light Count.
 * @param  retransaction: Retransmission flag.
 *     @arg 0: Non-retransmission.
 *     @arg 1: Retransmission.
 * @param  pub_list: list of publication.
 * @retval Whether the function executed successfully
 *   (0: success; others: error)
 */
int lightness_set(mesh_cmd_lightness_set_t *p_set, int par_len, u16 op, int idx, bool4 retransaction, st_pub_list_t *pub_list)
{
    #if LIGHTNESS_DATA_REFORMAT_EN
    lightness_set_data_reformat(p_set, par_len, op, idx, retransaction);
    #endif
    
	int err = -1;
	if(is_valid_lightness(p_set->lightness)){
		err = level_u16_set(p_set, par_len, op, idx, retransaction, ST_TRANS_LIGHTNESS, pub_list);
	}

#if CMD_LINEAR_EN
	if(0 == err){
        if(!((LIGHTNESS_LINEAR_SET == op) || (LIGHTNESS_LINEAR_SET_NOACK == op))){
            // need to clear the flag and the linear para part 
            clear_light_linear_flag(idx);
        }
	}
#endif

	return err;
}

/**
 * @brief  When the Light Lightness Set or Light Lightness Set Unacknowledged
 * message is received, this function will be called.
 * @param  par: Pointer to message data (excluding Opcode).
 * @param  par_len: The length of the message data.
 * @param  cb_par: Some parameters about function callbacks.
 * @retval Whether the function executed successfully
 *   (0: success; others: error)
 */
int mesh_cmd_sig_lightness_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err = 0;
    st_pub_list_t pub_list = {{0}};
	#if LS_TEST_ENABLE
	mesh_cmd_lightness_set_t *par_t = (mesh_cmd_lightness_set_t *)par;
	par_t->lightness = par_t->lightness*655;
	#endif
	err = lightness_set((mesh_cmd_lightness_set_t *)par, par_len, cb_par->op, cb_par->model_idx, cb_par->retransaction, &pub_list);
	if(err){
		return 0;
	}
	if(cb_par->op_rsp != STATUS_NONE){
		err = mesh_lightness_st_rsp(cb_par);
	}else{
		VC_RefreshUI_level(cb_par);
	}

	int linear = ((LIGHTNESS_LINEAR_SET == cb_par->op) || (LIGHTNESS_LINEAR_SET_NOACK == cb_par->op));
    model_pub_check_set_bind_all(&pub_list, cb_par, linear);

	return err;
}

// ----------- lightness linear-------------
/**
 * @brief       This function will be called when receive the opcode of "Light Lightness Linear Get"
 * @param[in]   par		- parameter of this message
 * @param[in]   par_len	- parameter length
 * @param[in]   cb_par	- parameters output by callback function which handle the opcode received.
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int mesh_cmd_sig_lightness_linear_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	return mesh_cmd_sig_lightness_get(par, par_len, cb_par);
}

/**
 * @brief       This function will be called when receive the opcode of "Light Lightness Linear Set"
 * @param[in]   par		- parameter of this message
 * @param[in]   par_len	- parameter length
 * @param[in]   cb_par	- parameters output by callback function which handle the opcode received.
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int mesh_cmd_sig_lightness_linear_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	mesh_cmd_lightness_set_t linear_set_tmp;
	memcpy(&linear_set_tmp, par, sizeof(linear_set_tmp));
	//use to set the linear and the flag part 
	set_light_linear_flag(cb_par->model_idx,linear_set_tmp.lightness);
	linear_set_tmp.lightness = linear_to_lightness(linear_set_tmp.lightness);
	return mesh_cmd_sig_lightness_set((u8 *)&linear_set_tmp, par_len, cb_par);
}

// ----------- lightness default-------------
/**
 * @brief       This function tx response with default level value
 * @param[in]   cb_par			- Some parameters about function callbacks.
 * @param[in]   st_trans_type	- A value in the enumeration type ST_TRANS_TYPE.
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int mesh_level_def_u16_st_rsp(mesh_cb_fun_par_t *cb_par, int st_trans_type)
{
    model_g_light_s_t *p_model = (model_g_light_s_t *)cb_par->model;
    mesh_cmd_lightness_def_st_t rsp = {0};
    rsp.lightness = light_g_level_def_get_u16(cb_par->model_idx, st_trans_type);
  
    return mesh_tx_cmd_rsp(cb_par->op_rsp, (u8 *)&rsp, sizeof(rsp), p_model->com.ele_adr, cb_par->adr_src, 0, 0);
}

/**
 * @brief       This function will be called when receive the opcode of "Light Lightness Default Get"Light Lightness Default Set
 * @param[in]   par		- parameter of this message
 * @param[in]   par_len	- parameter length
 * @param[in]   cb_par	- parameters output by callback function which handle the opcode received.
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int mesh_cmd_sig_lightness_def_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	return mesh_level_def_u16_st_rsp(cb_par, ST_TRANS_LIGHTNESS);
}

/**
 * @brief       This function will be called when receive the opcode of "Light Lightness Default Set"
 * @param[in]   par		- parameter of this message
 * @param[in]   par_len	- parameter length
 * @param[in]   cb_par	- parameters output by callback function which handle the opcode received.
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int mesh_cmd_sig_lightness_def_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err = 0;
	mesh_cmd_lightness_def_set_t *p_set = (mesh_cmd_lightness_def_set_t *)par;
	light_g_level_def_set_u16(p_set->lightness, cb_par->model_idx, ST_TRANS_LIGHTNESS);

	if(cb_par->op_rsp != STATUS_NONE){
		err = mesh_level_def_u16_st_rsp(cb_par, ST_TRANS_LIGHTNESS);
	}

	return err;
}

// ----------- lightness range -------------
/**
 * @brief  Send Light (Lightness/CTL Temperature) Range Status message.
 * @param  cb_par: Some parameters about function callbacks.
 * @param  st_trans_type: A value in the enumeration type ST_TRANS_TYPE. 
 * @retval Whether the function executed successfully
 *   (0: success; others: error)
 */
int mesh_range_st_rsp(u8 st, mesh_cb_fun_par_t *cb_par, int st_trans_type)
{
    model_g_light_s_t *p_model = (model_g_light_s_t *)cb_par->model;
    mesh_cmd_lightness_range_st_t rsp = {0};
    rsp.status = st;
    light_g_level_range_get_u16((light_range_u16_t *)&rsp.min, cb_par->model_idx, st_trans_type);
    return mesh_tx_cmd_rsp(cb_par->op_rsp, (u8 *)&rsp, sizeof(rsp), p_model->com.ele_adr, cb_par->adr_src, 0, 0);
}

/**
 * @brief  When the Light Lightness Range Get message is received, 
 *   this function will be called.
 * @param  par: Pointer to message data (excluding Opcode).
 * @param  par_len: The length of the message data.
 * @param  cb_par: Some parameters about function callbacks.
 * @retval Whether the function executed successfully
 *   (0: success; others: error)
 */
int mesh_cmd_sig_lightness_range_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	return mesh_range_st_rsp(RANGE_SET_SUCCESS, cb_par, ST_TRANS_LIGHTNESS);
}

/**
 * @brief  When the Light Lightness Range Set or Light Lightness Range Set 
 * Unacknowledged message is received, this function will be called.
 * @param  par: Pointer to message data (excluding Opcode).
 * @param  par_len: The length of the message data.
 * @param  cb_par: Some information about function callbacks.
 * @retval Whether the function executed successfully
 *   (0: success; others: error)
 */
int mesh_cmd_sig_lightness_range_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err = 0;
	u8 st = RANGE_SET_SUCCESS;
	mesh_cmd_lightness_range_set_t *p_set = (mesh_cmd_lightness_range_set_t *)par;
	if(0 == p_set->min){
		st = RANGE_CANNOT_SET_MIN;
	}else if(0 == p_set->max){
		st = RANGE_CANNOT_SET_MAX;
	}else if(p_set->min <= p_set->max){
		light_g_level_range_set(p_set->min, p_set->max, cb_par->model_idx, ST_TRANS_LIGHTNESS);
	}else{
		return 0;	// no response
	}

	if(cb_par->op_rsp != STATUS_NONE){
		err = mesh_range_st_rsp(st, cb_par, ST_TRANS_LIGHTNESS);
	}

	return err;
}

// ----------- lightness last-------------
/**
 * @brief       This function get last lightness
 * @param[in]   idx	- light count idx
 * @return      lightness value
 * @note        
 */
u16 lightness_last_get(int idx)
{
	return get_lightness_from_level(light_g_level_last_get(idx, ST_TRANS_LIGHTNESS));
}

/**
 * @brief       This function will be called when receive the opcode of "Light Lightness Last Get"
 * @param[in]   par		- parameter of this message
 * @param[in]   par_len	- parameter length
 * @param[in]   cb_par	- parameters output by callback function which handle the opcode received.
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int mesh_cmd_sig_lightness_last_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	model_g_light_s_t *p_model = (model_g_light_s_t *)cb_par->model;
	mesh_cmd_lightness_last_st_t rsp = {0};
	rsp.lightness = lightness_last_get(cb_par->model_idx);

	return mesh_tx_cmd_rsp(cb_par->op_rsp, (u8 *)&rsp, sizeof(mesh_cmd_lightness_last_st_t), p_model->com.ele_adr, cb_par->adr_src, 0, 0);
}

// ----------- light CTL model-------------
#if (LIGHT_TYPE_CT_EN)

/**
 * @brief  Fill in the parameters of the structure mesh_cmd_light_ctl_st_t.
 * @param  rsp: Pointer to structure mesh_cmd_light_ctl_st_t.
 * @param  idx: index of Light Count.
 * @retval None
 */
void mesh_light_ctl_st_rsp_par_fill(mesh_cmd_light_ctl_st_t *rsp, u8 idx)
{
	mesh_cmd_g_level_st_t level_st; 
	light_g_level_get((u8 *)&level_st, idx, ST_TRANS_LIGHTNESS);
	rsp->present_lightness = get_lightness_from_level(level_st.present_level);
	rsp->target_lightness = get_lightness_from_level(level_st.target_level);
	rsp->remain_t = level_st.remain_t;
	
	light_g_level_get((u8 *)&level_st, idx, ST_TRANS_CTL_TEMP);
	rsp->present_temp = light_ctl_temp_prensent_get(idx);
	rsp->target_temp = light_ctl_temp_target_get(idx);
	if(0 == rsp->remain_t){
	    rsp->remain_t = level_st.remain_t;
	}
}

/**
 * @brief  Send Light CTL Status message.
 * @param  idx: index of Light Count.
 * @param  ele_adr: Element address.
 * @param  dst_adr: Destination address.
 * @param  uuid: When publishing, and the destination address is a virtual 
 *   address, uuid needs to be passed in, otherwise fill in 0.
 * @param  pub_md: publish model. when publish, need to pass in a publish model. if not publish, set to NULL.
 *   For non-publish, pass in 0.
 * @retval Whether the function executed successfully
 *   (0: success; others: error)
 */
int mesh_tx_cmd_light_ctl_st(u8 idx, u16 ele_adr, u16 dst_adr, u8 *uuid, model_common_t *pub_md)
{
	mesh_cmd_light_ctl_st_t rsp = {0};
	mesh_light_ctl_st_rsp_par_fill(&rsp, idx);
	
	u32 len = sizeof(rsp);
	if(0 == rsp.remain_t){
		len -= 5;
	}

	return mesh_tx_cmd_rsp(LIGHT_CTL_STATUS, (u8 *)&rsp, len, ele_adr, dst_adr, uuid, pub_md);
}

/**
 * @brief  Publish Light CTL Status.
 * @param  idx: index of Light Count.
 * @retval Whether the function executed successfully
 *   (0: success; others: error)
 */
int mesh_light_ctl_st_publish(u8 idx)
{
	model_common_t *p_com_md = &model_sig_light_ctl.srv[idx].com;
	u16 ele_adr = p_com_md->ele_adr;
	u16 pub_adr = p_com_md->pub_adr;
	if(!pub_adr){
		return -1;
	}
	u8 *uuid = get_virtual_adr_uuid(pub_adr, p_com_md);
	return mesh_tx_cmd_light_ctl_st(idx, ele_adr, pub_adr, uuid, p_com_md);
}

/**
 * @brief  Send Light CTL Status message.
 * @param  cb_par  Some parameters about function callbacks.
 * @retval Whether the function executed successfully
 *   (0: success; others: error)
 */
int mesh_light_ctl_st_rsp(mesh_cb_fun_par_t *cb_par)
{
	model_g_light_s_t *p_model = (model_g_light_s_t *)cb_par->model;
	return mesh_tx_cmd_light_ctl_st(cb_par->model_idx, p_model->com.ele_adr, cb_par->adr_src, 0, 0);
}

/**
 * @brief  When the Light CTL Get message is received, this function 
 *   will be called
 * @param  par: Pointer to message data (excluding Opcode).
 * @param  par_len: The length of the message data.
 * @param  cb_par: Some parameters about function callbacks.
 * @retval Whether the function executed successfully
 *   (0: success; others: error)
 */
int mesh_cmd_sig_light_ctl_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	return mesh_light_ctl_st_rsp(cb_par);
}

/**
 * @brief       This function check tempture is valid
 * @param[in]   temp- tempture value
 * @return      0:invalid; 1:valid
 * @note        
 */
static inline int is_valid_ctl_temp(u16 temp)
{
	return ((temp >= CTL_TEMP_MIN)&&(temp <= CTL_TEMP_MAX));
}

/**
 * @brief       This function get tempture and check tempture is in range
 * @param[in]   temp		- tempture value
 * @param[in]   range_u16	- tempture Maximum and minimum structure
 * @return      tempture value
 * @note        
 */
u16 get_ctl_temp_with_check_range(u16 temp, light_range_u16_t *range_u16)
{
    if(temp < range_u16->min){
        return range_u16->min;
    }else if(temp > range_u16->max){
        return range_u16->max;
    }
    return temp;
}

/**
 * @brief       This function from tempture in light index get level 
 * @param[in]   temp		- tempture
 * @param[in]   light_idx	- light index if "LIGHT CNT > 1", or it is always 0. 
 * @return      level value
 * @note        
 */
s16 get_level_from_ctl_temp_light_idx(u16 temp, int light_idx)
{
    s16 level = 0;
    if(light_idx < LIGHT_CNT){
        light_range_u16_t range_u16 = {0};
        light_g_level_range_get_u16(&range_u16, light_idx, ST_TRANS_CTL_TEMP);    // always true
        temp = get_ctl_temp_with_check_range(temp, &range_u16);
        level = get_level_from_ctl_temp(temp, range_u16.min, range_u16.max);
    }
    return level;
}


/**
 * @brief  Set the color temperature of the light.
 * @param  p_set: Pointer to the set parameter structure
 *   (mesh_cmd_light_ctl_set_t).
 * @param  par_len: The length of the parameter p_set.
 * @param  op: Opcodes.
 * @param  idx: index of Light Count.
 * @param  retransaction: Retransmission flag.
 *     @arg 0: Non-retransmission.
 *     @arg 1: Retransmission.
 * @param  pub_list: list of publication.
 * @retval Whether the function executed successfully
 *   (0: success; others: error)
 */
int light_ctl_temp_set(mesh_cmd_light_ctl_set_t *p_set, int par_len, u16 op, int idx, bool4 retransaction, st_pub_list_t *pub_list)
{
	int err = -1;
	if(is_valid_ctl_temp(p_set->temp)){
		mesh_cmd_g_level_set_t level_set_tmp;
		memcpy(&level_set_tmp.tid, &p_set->tid, sizeof(mesh_cmd_g_level_set_t) - OFFSETOF(mesh_cmd_g_level_set_t,tid));
		int len_tmp = GET_LEVEL_PAR_LEN(par_len >= sizeof(mesh_cmd_light_ctl_set_t));

		// temp
        pub_list->no_dim_refresh_flag = 1;
		level_set_tmp.level = get_level_from_ctl_temp_light_idx(p_set->temp, idx);
		err = g_level_set_and_update_last((u8 *)&level_set_tmp, len_tmp, G_LEVEL_SET_NOACK, idx, retransaction, ST_TRANS_CTL_TEMP, 0, pub_list);
		if(err){
		    return err;
		}
		// delta uv
		level_set_tmp.level = p_set->delta_uv;
        pub_list->no_dim_refresh_flag = 0;   // dim refresh only when all level set ok
		err = g_level_set_and_update_last((u8 *)&level_set_tmp, len_tmp, G_LEVEL_SET_NOACK, idx, retransaction, ST_TRANS_CTL_D_UV, 0, pub_list);
	}
	return err;
}

/**
 * @brief  When the Light CTL Set or Light CTL Set Unacknowledged 
 *   message is received, this function will be called.
 * @param  par     Pointer to message data (excluding Opcode).
 * @param  par_len The length of the message data.
 * @param  cb_par  Some information about function callbacks.
 * @retval Whether the function executed successfully
 *   (0: success; others: error)
 */
int mesh_cmd_sig_light_ctl_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	set_ct_mode(1);
	int err = 0;
	mesh_cmd_light_ctl_set_t *p_set = (mesh_cmd_light_ctl_set_t *)par;
	if((!is_valid_ctl_temp(p_set->temp)) || (!is_valid_lightness(p_set->lightness))){
	    return -1;
	}
	
    st_pub_list_t pub_list = {{0}};
    pub_list.no_dim_refresh_flag = 1;
	mesh_cmd_lightness_set_t lightness_set_tmp = {0};
	lightness_set_tmp.lightness = p_set->lightness;
	int len_tmp = (par_len >= sizeof(mesh_cmd_light_ctl_set_t)) ? sizeof(mesh_cmd_lightness_set_t) : 2;
	memcpy(&lightness_set_tmp.tid, &p_set->tid, 3);
	err = lightness_set(&lightness_set_tmp, len_tmp, cb_par->op, cb_par->model_idx, cb_par->retransaction, &pub_list);
    if(err){
        return err;
    }
    pub_list.no_dim_refresh_flag = 0;   // dim refresh only when all level set ok
	err = light_ctl_temp_set(p_set, par_len, cb_par->op, cb_par->model_idx, cb_par->retransaction, &pub_list);
    if(err){
        return err;
    }
	if(cb_par->op_rsp != STATUS_NONE){
		err = mesh_light_ctl_st_rsp(cb_par);
	}else{
		VC_RefreshUI_level(cb_par);
	}
	
    model_pub_check_set_bind_all(&pub_list, cb_par, 0);

	return err;
}

// ----------- light ctl default-------------
/**
 * @brief  Send Light CTL Default Status message.
 * @param  cb_par  Some parameters about function callbacks.
 * @retval Whether the function executed successfully
 *   (0: success; others: error)
 */
int mesh_light_ctl_def_st_rsp(mesh_cb_fun_par_t *cb_par)
{
    model_g_light_s_t *p_model = (model_g_light_s_t *)cb_par->model;
    mesh_cmd_light_ctl_def_st_t rsp = {0};
    rsp.lightness = light_g_level_def_get_u16(cb_par->model_idx, ST_TRANS_LIGHTNESS);
    rsp.temp = light_g_level_def_get_u16(cb_par->model_idx, ST_TRANS_CTL_TEMP);
    rsp.delta_uv = light_g_level_def_get(cb_par->model_idx, ST_TRANS_CTL_D_UV);
    
    return mesh_tx_cmd_rsp(LIGHT_CTL_DEFAULT_STATUS, (u8 *)&rsp, sizeof(rsp), p_model->com.ele_adr, cb_par->adr_src, 0, 0);
}

/**
 * @brief  When the Light CTL Default Get message is received, this function 
 *   will be called
 * @param  par: Pointer to message data (excluding Opcode).
 * @param  par_len: The length of the message data.
 * @param  cb_par: Some parameters about function callbacks.
 * @retval Whether the function executed successfully
 *   (0: success; others: error)
 */
int mesh_cmd_sig_light_ctl_def_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	return mesh_light_ctl_def_st_rsp(cb_par);
}

/**
 * @brief  When the Light CTL Default Set or Light CTL Default Set
 *   Unacknowledged message is received, this function will be called.
 * @param  par: Pointer to message data (excluding Opcode).
 * @param  par_len: The length of the message data.
 * @param  cb_par: Some information about function callbacks.
 * @retval Whether the function executed successfully
 *   (0: success; others: error)
 */
int mesh_cmd_sig_light_ctl_def_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err = 0;
	mesh_cmd_light_ctl_def_set_t *p_set = (mesh_cmd_light_ctl_def_set_t *)par;
	if(is_valid_ctl_temp(p_set->temp)){
		light_g_level_def_set_u16(p_set->lightness, cb_par->model_idx, ST_TRANS_LIGHTNESS);
		light_g_level_def_set_u16(p_set->temp, cb_par->model_idx, ST_TRANS_CTL_TEMP);
		light_g_level_def_set(p_set->delta_uv, cb_par->model_idx, ST_TRANS_CTL_D_UV);
	}else{
		return 0;
	}

	if(cb_par->op_rsp != STATUS_NONE){
		err = mesh_light_ctl_def_st_rsp(cb_par);
	}

	return err;
}

/**
 * @brief  When the Light CTL Temperature Range Get message is received, 
 * this function will be called.
 * @param  par: Pointer to message data (excluding Opcode).
 * @param  par_len: The length of the message data.
 * @param  cb_par: Some parameters about function callbacks.
 * @retval Whether the function executed successfully
 *   (0: success; others: error)
 */
int mesh_cmd_sig_light_ctl_temp_range_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	return mesh_range_st_rsp(RANGE_SET_SUCCESS, cb_par, ST_TRANS_CTL_TEMP);
}

/**
 * @brief  When the Light CTL Temperature Range Set or Light CTL Temperature 
 *   Range Set Unacknowledged message is received, this function will be called.
 * @param  par     Pointer to message data (excluding Opcode).
 * @param  par_len The length of the message data.
 * @param  cb_par  Some information about function callbacks.
 * @retval Whether the function executed successfully
 *   (0: success; others: error)
 */
int mesh_cmd_sig_light_ctl_temp_range_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err = 0;
	int st = RANGE_SET_SUCCESS;
	mesh_cmd_light_ctl_temp_range_set_t *p_set = (mesh_cmd_light_ctl_temp_range_set_t *)par;

	if(!is_valid_ctl_temp(p_set->min)){
		st = RANGE_CANNOT_SET_MIN;
	}else if(!is_valid_ctl_temp(p_set->max)){
		st = RANGE_CANNOT_SET_MAX;
	}else if(p_set->min <= p_set->max){
		light_g_level_range_set(p_set->min, p_set->max, cb_par->model_idx, ST_TRANS_CTL_TEMP);
	}else{
		return 0;	// no response
	}

	if(cb_par->op_rsp != STATUS_NONE){
		err = mesh_range_st_rsp(st, cb_par, ST_TRANS_CTL_TEMP);
	}

	return err;
}

// ----------- light CTL temp model-------------
/**
 * @brief  Fill in the parameters of the structure mesh_cmd_light_ctl_temp_st_t.
 * @param  rsp: Pointer to structure mesh_cmd_light_ctl_temp_st_t.
 * @param  idx: index of Light Count.
 * @retval None
 */
void mesh_light_ctl_temp_st_rsp_par_fill(mesh_cmd_light_ctl_temp_st_t *rsp, u8 idx)
{
	mesh_cmd_g_level_st_t level_st = {0}; 
	light_g_level_get((u8 *)&level_st, idx, ST_TRANS_CTL_TEMP);
	rsp->present_temp = light_ctl_temp_prensent_get(idx);
	rsp->target_temp = light_ctl_temp_target_get(idx);
	rsp->remain_t = level_st.remain_t;

	light_g_level_get((u8 *)&level_st, idx, ST_TRANS_CTL_D_UV);
	rsp->present_delta_uv = level_st.present_level;
	rsp->target_delta_uv = level_st.target_level;
}

/**
 * @brief  Send Light CTL Temperature Status message.
 * @param  idx: index of Light Count.
 * @param  ele_adr: Element address.
 * @param  dst_adr: Destination address.
 * @param  uuid: When publishing, and the destination address is a virtual 
 *   address, uuid needs to be passed in, otherwise fill in 0.
 * @param  pub_md: publish model. when publish, need to pass in a publish model. if not publish, set to NULL.
 *   For non-publish, pass in 0.
 * @retval Whether the function executed successfully
 *   (0: success; others: error)
 */
int mesh_tx_cmd_light_ctl_temp_st(u8 idx, u16 ele_adr, u16 dst_adr, u8 *uuid, model_common_t *pub_md)
{
	mesh_cmd_light_ctl_temp_st_t rsp = {0};
	mesh_light_ctl_temp_st_rsp_par_fill(&rsp, idx);
	
	u32 len = sizeof(rsp);
	if(0 == rsp.remain_t){
		len -= 5;
	}

	return mesh_tx_cmd_rsp(LIGHT_CTL_TEMP_STATUS, (u8 *)&rsp, len, ele_adr, dst_adr, uuid, pub_md);
}

/**
 * @brief       This function publish Light CTL Temperature Status message.
 * @param[in]   idx	- index of Light Count.
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int mesh_light_ctl_temp_st_publish(u8 idx)
{
	model_common_t *p_com_md = &model_sig_light_ctl.temp[idx].com;
	u16 ele_adr = p_com_md->ele_adr;
	u16 pub_adr = p_com_md->pub_adr;
	if(!pub_adr){
		return -1;
	}
	u8 *uuid = get_virtual_adr_uuid(pub_adr, p_com_md);
	return mesh_tx_cmd_light_ctl_temp_st(idx, ele_adr, pub_adr, uuid, p_com_md);
}

/**
 * @brief  Send Light CTL Temperature Status message.
 * @param  cb_par  Some parameters about function callbacks.
 * @retval Whether the function executed successfully
 *   (0: success; others: error)
 */
int mesh_light_ctl_temp_st_rsp(mesh_cb_fun_par_t *cb_par)
{
	model_g_light_s_t *p_model = (model_g_light_s_t *)cb_par->model;
	return mesh_tx_cmd_light_ctl_temp_st(cb_par->model_idx, p_model->com.ele_adr, cb_par->adr_src, 0, 0);
}

/**
 * @brief  When the Light CTL Temperature Get message is received, 
 *   this function will be called.
 * @param  par: Pointer to message data (excluding Opcode).
 * @param  par_len: The length of the message data.
 * @param  cb_par: Some parameters about function callbacks.
 * @retval Whether the function executed successfully
 *   (0: success; others: error)
 */
int mesh_cmd_sig_light_ctl_temp_get(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	return mesh_light_ctl_temp_st_rsp(cb_par);
}

/**
 * @brief  When the Light CTL Temperature Set or Light CTL Temperature 
 *   Set Unacknowledged message is received, 
 * this function will be called.
 * @param  par: Pointer to message data (excluding Opcode).
 * @param  par_len: The length of the message data.
 * @param  cb_par: Some parameters about function callbacks.
 * @retval Whether the function executed successfully
 *   (0: success; others: error)
 */
int mesh_cmd_sig_light_ctl_temp_set(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	set_ct_mode(1);
	int err = 0;
    st_pub_list_t pub_list = {{0}};
	mesh_cmd_light_ctl_set_t *p_set = CONTAINER_OF((u16 *)par, mesh_cmd_light_ctl_set_t, temp);
	err = light_ctl_temp_set(p_set, par_len + 2, cb_par->op, cb_par->model_idx, cb_par->retransaction, &pub_list);
	if(err){
		return 0;
	}
	
	if(cb_par->op_rsp != STATUS_NONE){
		err = mesh_light_ctl_temp_st_rsp(cb_par);
	}else{
		// VC_RefreshUI_level(cb_par);
	}

	model_pub_check_set_bind_all(&pub_list, cb_par, 0);

	return err;
}
#endif
#endif

#if MD_CLIENT_EN
/**
 * @brief       This function will be called when receive the opcode of "Light Lightness Status"
 * @param[in]   par		- parameter of this message
 * @param[in]   par_len	- parameter length
 * @param[in]   cb_par	- parameters output by callback function which handle the opcode received.
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int mesh_cmd_sig_lightness_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err = 0;
	if(cb_par->model){	// model may be Null for status message
		//model_client_common_t *p_model = (model_client_common_t *)(cb_par->model);
	}
	return err;
}

/**
 * @brief       This function will be called when receive the opcode of "Light Lightness Last Status"
 * @param[in]   par		- parameter of this message
 * @param[in]   par_len	- parameter length
 * @param[in]   cb_par	- parameters output by callback function which handle the opcode received.
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int mesh_cmd_sig_lightness_last_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err = 0;
	if(cb_par->model){	// model may be Null for status message
		//model_client_common_t *p_model = (model_client_common_t *)(cb_par->model);
	}
	return err;
}

/**
 * @brief       This function will be called when receive the opcode of "Light Lightness Linear Status"
 * @param[in]   par		- parameter of this message
 * @param[in]   par_len	- parameter length
 * @param[in]   cb_par	- parameters output by callback function which handle the opcode received.
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int mesh_cmd_sig_lightness_linear_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err = 0;
	if(cb_par->model){	// model may be Null for status message
		//model_client_common_t *p_model = (model_client_common_t *)(cb_par->model);
	}
	return err;
}

/**
 * @brief       This function will be called when receive the opcode of "Light Lightness Default Status"
 * @param[in]   par		- parameter of this message
 * @param[in]   par_len	- parameter length
 * @param[in]   cb_par	- parameters output by callback function which handle the opcode received.
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int mesh_cmd_sig_lightness_def_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err = 0;
	if(cb_par->model){	// model may be Null for status message
		//model_client_common_t *p_model = (model_client_common_t *)(cb_par->model);
	}
	return err;
}

/**
 * @brief       This function will be called when receive the opcode of "Light Lightness Range Status"
 * @param[in]   par		- parameter of this message
 * @param[in]   par_len	- parameter length
 * @param[in]   cb_par	- parameters output by callback function which handle the opcode received.
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int mesh_cmd_sig_lightness_range_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err = 0;
	if(cb_par->model){	// model may be Null for status message
		//model_client_common_t *p_model = (model_client_common_t *)(cb_par->model);
	}
	return err;
}

/**
 * @brief       This function will be called when receive the opcode of "Light CTL Status"
 * @param[in]   par		- parameter of this message
 * @param[in]   par_len	- parameter length
 * @param[in]   cb_par	- parameters output by callback function which handle the opcode received.
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int mesh_cmd_sig_light_ctl_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err = 0;
	if(cb_par->model){	// model may be Null for status message
		//model_client_common_t *p_model = (model_client_common_t *)(cb_par->model);
	}
	return err;
}

/**
 * @brief       This function will be called when receive the opcode of "Light CTL Default Status"
 * @param[in]   par		- parameter of this message
 * @param[in]   par_len	- parameter length
 * @param[in]   cb_par	- parameters output by callback function which handle the opcode received.
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int mesh_cmd_sig_light_ctl_def_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err = 0;
	if(cb_par->model){	// model may be Null for status message
		//model_client_common_t *p_model = (model_client_common_t *)(cb_par->model);
	}
	return err;
}

/**
 * @brief       This function will be called when receive the opcode of "Light CTL Temperature Range Status"
 * @param[in]   par		- parameter of this message
 * @param[in]   par_len	- parameter length
 * @param[in]   cb_par	- parameters output by callback function which handle the opcode received.
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int mesh_cmd_sig_light_ctl_temp_range_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err = 0;
	if(cb_par->model){	// model may be Null for status message
		//model_client_common_t *p_model = (model_client_common_t *)(cb_par->model);
	}
	return err;
}

/**
 * @brief       This function will be called when receive the opcode of "Light CTL Temperature Status"
 * @param[in]   par		- parameter of this message
 * @param[in]   par_len	- parameter length
 * @param[in]   cb_par	- parameters output by callback function which handle the opcode received.
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int mesh_cmd_sig_light_ctl_temp_status(u8 *par, int par_len, mesh_cb_fun_par_t *cb_par)
{
	int err = 0;
	if(cb_par->model){	// model may be Null for status message
		//model_client_common_t *p_model = (model_client_common_t *)(cb_par->model);
	}
	return err;
}
#endif
//--lighting model command interface-------------------
//-----------access command--------
/**
 * @brief       This function Check if luminous is valid
 * @param[in]   lum	- luminous value
 * @return      0:invalid; 1:valid
 * @note        
 */
int is_valid_lum(u8 lum)
{
	return (lum <= 100);	// set lightness to 0 means Generic OFF,
}

/**
 * @brief       This function check if color temperature(0-100) is valid
 * @param[in]   temp100	- color temperature(0-100)
 * @return      0:invalid; 1:valid
 * @note        
 */
int is_valid_temp_100(u8 temp100)
{
	return (temp100 <= 100);
}

/**
 * @brief       This function get lightness
 * @param[in]   adr		- Destination address
 * @param[in]   rsp_max	- max number of response expected from nodes which match destination address, after sending the opcode.
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int access_cmd_get_lightness(u16 adr,u32 rsp_max)
{
	u8 par[1];
	return SendOpParaDebug(adr, rsp_max, LIGHTNESS_GET, par, 0);
}

/**
 * @brief       This function set light lightness
 * @param[in]   op			- opcode
 * @param[in]   adr			- Destination address
 * @param[in]   rsp_max		- max number of response expected from nodes which match destination address, after sending the opcode.
 * @param[in]   lightness	- lightness
 * @param[in]   trs_par		- parameter of transmit time and delay. NULL mean no parameter.
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int access_cmd_set_lightness_ll(u16 op, u16 adr, u8 rsp_max, u16 lightness, transition_par_t *trs_par)
{
	mesh_cmd_lightness_set_t par = {0};
	u32 par_len = OFFSETOF(mesh_cmd_lightness_set_t,transit_t);
	par.lightness = lightness;
	if(trs_par){
		par_len = sizeof(mesh_cmd_lightness_set_t);
		memcpy(&par.transit_t, trs_par, 2);
	}
	
	return SendOpParaDebug(adr, rsp_max, op, 
						   (u8 *)&par, par_len);
}

/**
 * @brief       This function set light lightness
 * @param[in]   adr			- Destination address
 * @param[in]   rsp_max		- max number of response expected from nodes which match destination address, after sending the opcode.
 * @param[in]   lightness	- lightness
 * @param[in]   ack			- acknowledged, 1: need node to respond; 0: no need.
 * @param[in]   trs_par		- parameter of transmit time and delay. NULL mean no parameter.
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int access_cmd_set_lightness(u16 adr, u8 rsp_max, u16 lightness, int ack, transition_par_t *trs_par)
{
	u16 op = ack ? LIGHTNESS_SET : LIGHTNESS_SET_NOACK;
	return access_cmd_set_lightness_ll(op, adr, rsp_max, lightness, trs_par);
}

/**
 * @brief       This function set light luminous
 * @param[in]   adr		- Destination address
 * @param[in]   rsp_max	- max number of response expected from nodes which match destination address, after sending the opcode.
 * @param[in]   lum		- luminous value
 * @param[in]   ack		- acknowledged, 1: need node to respond; 0: no need.
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int access_set_lum(u16 adr, u8 rsp_max, u8 lum, int ack)
{
	if(!is_valid_lum(lum)){
		return -2;
	}

	#if 1
	return access_cmd_set_lightness(adr, rsp_max, lum2_lightness(lum), ack, 0);
	#else
	return access_cmd_set_level(adr, rsp_max, lum2level(lum), ack, 0);
	#endif
}

/**
 * @brief       This function get lightness
 * @param[in]   adr		- Destination address
 * @param[in]   rsp_max	- how many response expected from nodes which match destination address, after sending this command
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int access_get_lum(u16 adr, u8 rsp_max)
{
	return access_cmd_get_lightness(adr, rsp_max);
}

/**
 * @brief      This function get lightness linear
 * @param[in]  adr		- Destination address
 * @param[in]  rsp_max	- how many response expected from nodes which match destination address, after sending this command
 * @return     0: success; others: error code of tx_errno_e
 * @note        
 */
int access_cmd_get_lightness_linear(u16 adr,u32 rsp_max)
{
	u8 par[1];
	return SendOpParaDebug(adr, rsp_max, LIGHTNESS_LINEAR_GET, par, 0);
}

/**
 * @brief       This function set lightness linear
 * @param[in]   adr		- Destination address
 * @param[in]   rsp_max	- how many response expected from nodes which match destination address, after sending this command
 * @param[in]   linear	- linear
 * @param[in]   ack		- acknowledged, 1: need node to respond; 0: no need.
 * @param[in]   trs_par	- parameter of transmit time and delay. NULL mean no parameter.
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int access_cmd_set_lightness_linear(u16 adr, u8 rsp_max, u16 linear, int ack, transition_par_t *trs_par)
{
	u16 op = ack ? LIGHTNESS_LINEAR_SET : LIGHTNESS_LINEAR_SET_NOACK;
	return access_cmd_set_lightness_ll(op, adr, rsp_max, linear, trs_par);
}

/**
 * @brief       This function get last lightness
 * @param[in]   adr		- Destination address
 * @param[in]   rsp_max	- how many response expected from nodes which match destination address, after sending this command
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int access_cmd_get_lightness_last(u16 adr,u32 rsp_max)
{
	u8 par[1];
	return SendOpParaDebug(adr, rsp_max, LIGHTNESS_LAST_GET, par, 0);
}

/**
 * @brief       This function set lightness default 
 * @param[in]   adr			- Destination address
 * @param[in]   rsp_max		- how many response expected from nodes which match destination address, after sending this command
 * @param[in]   lightness	- lightness
 * @param[in]   ack			- acknowledged, 1: need node to respond; 0: no need.
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int access_cmd_set_lightness_def(u16 adr, u8 rsp_max, u16 lightness, int ack)
{
	mesh_cmd_lightness_def_set_t par = {0};
	u32 par_len = sizeof(mesh_cmd_lightness_def_set_t);
	par.lightness = lightness;
	
	return SendOpParaDebug(adr, rsp_max, ack ? LIGHTNESS_DEFAULT_SET : LIGHTNESS_DEFAULT_SET_NOACK, 
						   (u8 *)&par, par_len);
}

/**
 * @brief       This function get lightness default 
 * @param[in]   adr		- Destination address
 * @param[in]   rsp_max	- how many response expected from nodes which match destination address, after sending this command
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int access_cmd_get_lightness_def(u16 adr,u32 rsp_max)
{
	u8 par[1];
	return SendOpParaDebug(adr, rsp_max, LIGHTNESS_DEFAULT_GET, par, 0);
}

/**
 * @brief       This function set lightness range
 * @param[in]   adr		- Destination address
 * @param[in]   rsp_max	- how many response expected from nodes which match destination address, after sending this command
 * @param[in]   min		- lightness min
 * @param[in]   max		- lightness max
 * @param[in]   ack		- acknowledged, 1: need node to respond; 0: no need.
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int access_cmd_set_lightness_range(u16 adr, u8 rsp_max, u16 min, u16 max, int ack)
{
	mesh_cmd_lightness_range_set_t par = {0};
	u32 par_len = sizeof(mesh_cmd_lightness_range_set_t);
	par.min = min;
	par.max = max;
	
	return SendOpParaDebug(adr, rsp_max, ack ? LIGHTNESS_RANGE_SET : LIGHTNESS_RANGE_SET_NOACK, 
						   (u8 *)&par, par_len);
}

/**
 * @brief       This function get lightness range
 * @param[in]   adr		- Destination address
 * @param[in]   rsp_max	- how many response expected from nodes which match destination address, after sending this command
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int access_cmd_get_lightness_range(u16 adr,u32 rsp_max)
{
	u8 par[1];
	return SendOpParaDebug(adr, rsp_max, LIGHTNESS_RANGE_GET, par, 0);
}

// light ctl
#if 1 // (LIGHT_TYPE_CT_EN)
/**
 * @brief       This function get light Color temperature
 * @param[in]   adr		- Destination address
 * @param[in]   rsp_max	- how many response expected from nodes which match destination address, after sending this command
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int access_cmd_get_light_ctl(u16 adr,u32 rsp_max)
{
	u8 par[1];
	return SendOpParaDebug(adr, rsp_max, LIGHT_CTL_GET, par, 0);
}

/**
 * @brief       This function set light Color temperature
 * @param[in]   adr			- Destination address
 * @param[in]   rsp_max		- how many response expected from nodes which match destination address, after sending this command
 * @param[in]   lightness	- lightness
 * @param[in]   temp		- tempure value
 * @param[in]   ack			- acknowledged, 1: need node to respond; 0: no need.
 * @param[in]   trs_par		- parameter of transmit time and delay. NULL mean no parameter.
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int access_cmd_set_light_ctl(u16 adr, u8 rsp_max, u16 lightness, u16 temp, int ack, transition_par_t *trs_par)
{
	mesh_cmd_light_ctl_set_t par = {0};
	u32 par_len = OFFSETOF(mesh_cmd_light_ctl_set_t,transit_t);
	par.lightness = lightness;
	par.temp = temp;
	par.delta_uv = 0;	// confirm later
	if(trs_par){
		par_len = sizeof(mesh_cmd_light_ctl_set_t);
		memcpy(&par.transit_t, trs_par, 2);
	}
	
	return SendOpParaDebug(adr, rsp_max, ack ? LIGHT_CTL_SET : LIGHT_CTL_SET_NOACK, 
						   (u8 *)&par, par_len);
}

/**
 * @brief       This function set color temperature(0-100) switch to color temperature(CTL_TEMP_MIN - CTL_TEMP_MAX)
 * @param[in]   adr		- Destination address
 * @param[in]   rsp_max	- how many response expected from nodes which match destination address, after sending this command
 * @param[in]   lum		- luminous value
 * @param[in]   temp100	- color temperature(0-100)
 * @param[in]   ack		- acknowledged, 1: need node to respond; 0: no need.
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int access_cmd_set_light_ctl_100(u16 adr, u8 rsp_max, u8 lum, u8 temp100, int ack)
{
	if((!is_valid_lum(lum)) || (!is_valid_temp_100(temp100))){
		return -2;
	}

	u16 lightness = lum2_lightness(lum);
	u16 temp = temp100_to_temp(temp100);
	return access_cmd_set_light_ctl(adr, rsp_max, lightness, temp, ack, 0);
}

/**
 * @brief       This function set light Color temperature
 * @param[in]   adr		- Destination address
 * @param[in]   rsp_max	- how many response expected from nodes which match destination address, after sending this command
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int access_cmd_get_light_ctl_temp(u16 adr,u32 rsp_max)
{
	u8 par[1];
	return SendOpParaDebug(adr, rsp_max, LIGHT_CTL_TEMP_GET, par, 0);
}

/**
 * @brief       This function set light Color temperature
 * @param[in]   adr		- Destination address
 * @param[in]   rsp_max	- how many response expected from nodes which match destination address, after sending this command
 * @param[in]   temp	- Color temperature
 * @param[in]   ack		- acknowledged, 1: need node to respond; 0: no need.
 * @param[in]   trs_par	- parameter of transmit time and delay. NULL mean no parameter
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int access_cmd_set_light_ctl_temp(u16 adr, u8 rsp_max, u16 temp, int ack, transition_par_t *trs_par)
{
	mesh_cmd_light_ctl_temp_set_t par = {0};
	u32 par_len = OFFSETOF(mesh_cmd_light_ctl_temp_set_t,transit_t);
	par.temp = temp;
	par.delta_uv = 0;	// confirm later
	if(trs_par){
		par_len = sizeof(mesh_cmd_light_ctl_temp_set_t);
		memcpy(&par.transit_t, trs_par, 2);
	}
	
	return SendOpParaDebug(adr, rsp_max, ack ? LIGHT_CTL_TEMP_SET : LIGHT_CTL_TEMP_SET_NOACK, 
						   (u8 *)&par, par_len);
}

/**
 * @brief       This function set light ctl temperature(0-100) switch to light ctl temperature(CTL_TEMP_MIN - CTL_TEMP_MAX)
 * @param[in]   adr		- Destination address
 * @param[in]   rsp_max	- how many response expected from nodes which match destination address, after sending this command
 * @param[in]   temp100	- color temperature(0-100)
 * @param[in]   ack		- acknowledged, 1: need node to respond; 0: no need.
 * @return      0: success; others: error code of tx_errno_e
 * @note        
 */
int access_cmd_set_light_ctl_temp_100(u16 adr, u8 rsp_max, u8 temp100, int ack)
{
	if(!is_valid_temp_100(temp100)){
		return -2;
	}

	u16 temp = temp100_to_temp(temp100);
	return access_cmd_set_light_ctl_temp(adr, rsp_max, temp, ack, 0);
}
#endif
//--lighting model command interface end----------------

/**
  * @}
  */

/**
  * @}
  */


