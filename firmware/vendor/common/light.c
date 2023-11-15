/********************************************************************************************************
 * @file	light.c
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
#include "proj_lib/ble/ll/ll.h"
#include "proj_lib/ble/blt_config.h"
#include "proj_lib/sig_mesh/app_mesh.h"
#include "proj_lib/ble/service/ble_ll_ota.h"
#include "vendor/common/lighting_model.h"
#include "vendor/common/lighting_model_HSL.h"
#include "vendor/common/lighting_model_xyL.h"
#include "vendor/common/lighting_model_LC.h"
#include "vendor/common/generic_model.h"
#include "vendor/common/scene.h"
#include "light.h"
#if HOMEKIT_EN
#include "vendor/common/led_cfg.h"
#endif
#if WIN32
#include <stdlib.h>
#else
#include "proj/mcu/watchdog_i.h"
#include "proj_lib/pm.h"
#endif


/** @addtogroup Mesh_Common
  * @{
  */
  
/** @defgroup Light
  * @brief Light Code.
  * @{
  */


#if ((LIGHT_TYPE_SEL == LIGHT_TYPE_CT) || (LIGHT_TYPE_SEL == LIGHT_TYPE_CT_HSL))
u8 ct_flag = 1; // default CT
#else   // (LIGHT_TYPE_SEL == LIGHT_TYPE_HSL)
u8 ct_flag = 0; // always HSL
#endif

#if (IS_VC_PROJECT)
#define RES_HW_PWM_R    {0, 2, 0, 0}
#define RES_HW_PWM_G    {0, 1, 0, 0}
#define RES_HW_PWM_B    {0, 3, 0, 0}
#define RES_HW_PWM_W    {0, 4, 0, 0}
#else
#define RES_HW_PWM_R    {PWM_R, PWMID_R, PWM_INV_R, PWM_FUNC_R}
#define RES_HW_PWM_G    {PWM_G, PWMID_G, PWM_INV_G, PWM_FUNC_G}
#define RES_HW_PWM_B    {PWM_B, PWMID_B, PWM_INV_B, PWM_FUNC_B}
#define RES_HW_PWM_W    {PWM_W, PWMID_W, PWM_INV_W, PWM_FUNC_W}
#endif

#if LIGHT_RES_HW_USER_EN
// user define
#else
#if (LIGHT_TYPE_SEL == LIGHT_TYPE_CT_HSL)
const light_res_hw_t light_res_hw[LIGHT_CNT][4] = {
	/*[0] = */{RES_HW_PWM_R, RES_HW_PWM_G, RES_HW_PWM_B, RES_HW_PWM_W}, // vc can't use "[0]="
};
#elif (LIGHT_TYPE_SEL == LIGHT_TYPE_HSL || LIGHT_TYPE_SEL == LIGHT_TYPE_XYL)
const light_res_hw_t light_res_hw[LIGHT_CNT][3] = {
	/*[0] = */{RES_HW_PWM_R, RES_HW_PWM_G, RES_HW_PWM_B},
};
#elif (LIGHT_TYPE_SEL == LIGHT_TYPE_CT)
	#if XIAOMI_MODULE_ENABLE
const light_res_hw_t light_res_hw[LIGHT_CNT][3] = {
	/*[0] = */{RES_HW_PWM_R, RES_HW_PWM_G, RES_HW_PWM_W},
};
	#else
const light_res_hw_t light_res_hw[LIGHT_CNT][2] = {
	/*[0] = */{RES_HW_PWM_R, RES_HW_PWM_G},
};
	#endif
#else
const light_res_hw_t light_res_hw[LIGHT_CNT][1] = {
	[0] = {RES_HW_PWM_R},
    #if (LIGHT_CNT >= 2)
    [1] = {RES_HW_PWM_G},
    #endif
    #if (LIGHT_CNT >= 3)
    [2] = {RES_HW_PWM_B},
    #endif
    #if (LIGHT_CNT >= 4)
    [3] = {RES_HW_PWM_W},
    #endif
    #if (LIGHT_CNT >= 5)
#error TODO: to add pwm or IO resource by user.
    #endif
};
#endif
#endif

const u32 GPIO_LED_INDEX = (GPIO_LED == PWM_R) ? 0 : ((GPIO_LED == PWM_G) ? 1 : ((GPIO_LED == PWM_B) ? 2 : ((GPIO_LED == PWM_W) ? 3 : 0)));

#define LIGHT_ADJUST_INTERVAL       (20)   // unit :ms;     min:20ms; max 100ms

light_res_sw_save_t light_res_sw_save[LIGHT_CNT] = {{{{0}}}};
light_res_sw_trans_t light_res_sw[LIGHT_CNT] ;

#define P_SW_LEVEL_SAVE(idx, type)	(&light_res_sw_save[idx].level[type])
#define P_ST_TRANS(idx, type)		(&light_res_sw[idx].trans[type])

u8 light_pub_trans_step = ST_PUB_TRANS_IDLE;    // 0
u8 *light_pub_model_priority = 0;


STATIC_ASSERT(PWM_MAX_TICK < 0x10000);  // 16 bit for PWM register. if use smaller frequence, please set reg_pwm_clk.
STATIC_ASSERT(LIGHTNESS_DEFAULT != 0);	// if want to set 0, please set ONOFF_DEFAULT to 0,
STATIC_ASSERT(LIGHTNESS_MIN != 0);
//STATIC_ASSERT(sizeof(light_res_sw_t) % 4 == 0); // for align
STATIC_ASSERT(LIGHT_ADJUST_INTERVAL <= 100);

// 0-100%  (pwm's value index: this is pwm compare value, and the pwm cycle is 255*256)
const u16 rgb_lumen_map[101] = {
  0,2*256+128,2*256+192,3*256,3*256+128,4*256,4*256+128,5*256,5*256+128,6*256,  7*256,
      8*256,  9*256, 10*256, 11*256, 12*256, 13*256, 14*256, 15*256, 16*256, 17*256,
     18*256, 19*256, 21*256, 23*256, 25*256, 27*256, 29*256, 31*256, 33*256, 35*256,
     37*256, 39*256, 41*256, 43*256, 45*256, 47*256, 49*256, 51*256, 53*256, 55*256,
     57*256, 59*256, 61*256, 63*256, 65*256, 67*256, 69*256, 71*256, 73*256, 75*256,
     78*256, 81*256, 84*256, 87*256, 90*256, 93*256, 96*256, 99*256,102*256,105*256, 
    108*256,111*256,114*256,117*256,120*256,123*256,126*256,129*256,132*256,135*256,
    138*256,141*256,144*256,147*256,150*256,154*256,158*256,162*256,166*256,170*256,
    174*256,178*256,182*256,186*256,190*256,194*256,198*256,202*256,206*256,210*256,
    214*256,218*256,222*256,226*256,230*256,235*256,240*256,245*256,250*256,255*256,// must less or equal than (255*256)
};


/**
 * @brief       This function set light ct mode for LIGHT_TYPE_CT_HSL
 * @param[in]   mode	- 0:  HSL mode; 1: CT mode.
 * @return      none
 * @note        
 */
void set_ct_mode(u8 mode)
{
    if(ct_flag != mode){
	    ct_flag = mode;
	    mesh_misc_store();
	}
}

#if MD_SERVER_EN

/**
 * @brief  Save Generic OnOff States for next power-on.
 * @param  idx: index of Light Count.
 * @param  st_trans_type: a enum value of ST_TRANS_TYPE.
 * @param  onoff: OnOff States.
 *     @arg G_OFF: Off state.
 *     @arg G_ON: On state.
 * @retval None
 */
void set_on_power_up_onoff(int idx, int st_trans_type, u8 onoff)
{
	sw_level_save_t *p_save = P_SW_LEVEL_SAVE(idx, st_trans_type);
	p_save->onoff = onoff;
	light_par_save(0);
}


/**
 * @brief       This function set last value which will be used when power up.
 * @param[in]   last	- last value
 * @param[out]  p_save	- pointer to save
 * @return      none
 * @note        
 */
void set_on_power_up_last(sw_level_save_t *p_save, s16 last)
{
	if(LEVEL_OFF == last){
		p_save->onoff = 0;  // active for all level. include CT, Hue, Sat...
	}else{
		p_save->onoff = 1;
		p_save->last = last;
	}
}


/**
 * @brief       This function get last value which will be used when power up.
 * @param[in]   p_save	- pointer to save
 * @return      if need to be on when power up: last value; if off: LEVEL_OFF
 * @note        
 */
s16 get_on_power_up_last(sw_level_save_t *p_save)
{
	return (p_save->onoff ? p_save->last : LEVEL_OFF);
}

#if (!WIN32)
#if 1 // KEEP_ONOFF_STATE_AFTER_OTA // always on, because lpn need to set in gatt adv mode after ota reboot.
#define OTA_REBOOT_CHECK_FLAG			(KEEP_ONOFF_STATE_AFTER_OTA ? FLD_OTA_REBOOT_FLAG : 0)


/**
 * @brief       This function save OTA satte when OTA, then it can keep light state after OTA reboot.
 * @return      none
 * @note        
 */
void set_keep_onoff_state_after_ota()
{
	analog_write(DEEP_ANA_REG0, analog_read(DEEP_ANA_REG0) | FLD_OTA_REBOOT_FLAG);
}


/**
 * @brief       This function clear OTA satte.
 * @return      none
 * @note        
 */
void clr_keep_onoff_state_after_ota()
{
	analog_write(DEEP_ANA_REG0, analog_read(DEEP_ANA_REG0) & (~ FLD_OTA_REBOOT_FLAG));
}


/**
 * @brief       This function get if it is ota state.
 * @return      none
 * @note        
 */
int is_state_after_ota()
{
	return (analog_read(DEEP_ANA_REG0) & FLD_OTA_REBOOT_FLAG);
}
#endif
#endif


/**
 * @brief       This function initial global variable for light software resource. as if it is default value for compile.
 * @return      none
 * @note        
 */
void mesh_global_var_init_light_sw()
{
	foreach_arr(i,light_res_sw){
		foreach_arr(k,light_res_sw[i].trans){
			sw_level_save_t *p_save = &light_res_sw_save[i].level[k];
			st_transition_t *p_trans = &light_res_sw[i].trans[k];
			s16 val_init = 0;
			if(ST_TRANS_LIGHTNESS == k){
				val_init = u16_to_s16(LIGHTNESS_DEFAULT);
				p_save->min = u16_to_s16(LIGHTNESS_MIN);
				p_save->max = u16_to_s16(LIGHTNESS_MAX);
			    p_save->def = u16_to_s16(0);	// use last, spec page172 requested.
			#if (LIGHT_TYPE_CT_EN)
			}else if(ST_TRANS_CTL_TEMP == k){
				p_save->min = u16_to_s16(CTL_TEMP_MIN);   // save as s16 temp value, not related to range
				p_save->max = u16_to_s16(CTL_TEMP_MAX);   // save as s16 temp value, not related to range
				val_init = get_level_from_ctl_temp(CTL_TEMP_DEFAULT, CTL_TEMP_MIN, CTL_TEMP_MAX);
			    p_save->def = u16_to_s16(CTL_TEMP_DEFAULT);
			}else if(ST_TRANS_CTL_D_UV == k){
				val_init = CTL_D_UV_DEFAULT;
				p_save->min = CTL_D_UV_MIN;
				p_save->max = CTL_D_UV_MAX;
			    p_save->def = val_init;
			#endif
			#if (LIGHT_TYPE_HSL_EN)
			}else if(ST_TRANS_HSL_HUE == k){
				val_init = u16_to_s16(HSL_HUE_DEFAULT);
				p_save->min = u16_to_s16(HSL_HUE_MIN);
				p_save->max = u16_to_s16(HSL_HUE_MAX);
			    p_save->def = val_init;
			}else if(ST_TRANS_HSL_SAT == k){
				val_init = u16_to_s16(HSL_SAT_DEFAULT);
				p_save->min = u16_to_s16(HSL_SAT_MIN);
				p_save->max = u16_to_s16(HSL_SAT_MAX);
			    p_save->def = val_init;
			#endif
			#if (LIGHT_TYPE_SEL == LIGHT_TYPE_XYL)
			}else if(ST_TRANS_XYL_X == k){
				val_init = u16_to_s16(XYL_X_DEFAULT);
				p_save->min = u16_to_s16(XYL_X_MIN);
				p_save->max = u16_to_s16(XYL_X_MAX);
			    p_save->def = val_init;
			}else if(ST_TRANS_XYL_Y == k){
				val_init = u16_to_s16(XYL_Y_DEFAULT);
				p_save->min = u16_to_s16(XYL_Y_MIN);
				p_save->max = u16_to_s16(XYL_Y_MAX);
			    p_save->def = val_init;
			#endif
			}
			
            p_save->last = val_init;    // must init "last", even though it's -32768.
			p_save->onoff = (ST_TRANS_LIGHTNESS == k) ? ONOFF_DEFAULT : 1;
            
			p_trans->present = p_trans->target = get_on_power_up_last(p_save);
		}

		ONPOWER_UP_VAL(i) = ONPOWER_UP_SELECT;
		g_def_trans_time_val(i) = PTS_TEST_EN ? 0 : TRANSITION_TIME_DEFAULT_VAL;
	}
}

/**
 * @brief       This function load software resource of the light from p_save which read from flash before.
 * @return      none
 * @note        
 */
void light_res_sw_load()
{
	foreach_arr(i,light_res_sw){
		foreach_arr(k,light_res_sw[i].trans){
			sw_level_save_t *p_save = &light_res_sw_save[i].level[k];
			st_transition_t *p_trans = &light_res_sw[i].trans[k];
			s16 level_poweron = 0;
			#if(WIN32)
			if(ONPOWER_UP_STORE == ONPOWER_UP_VAL(i)){
			#else
			if((ONPOWER_UP_STORE == ONPOWER_UP_VAL(i)) || ((ST_TRANS_LIGHTNESS == k) && (analog_read(DEEP_ANA_REG0)&(OTA_REBOOT_CHECK_FLAG|FLD_LOW_BATT_FLG)))){
			#endif
				level_poweron = get_on_power_up_last(p_save);
			}
			else if((ONPOWER_UP_OFF == ONPOWER_UP_VAL(i))&&(ST_TRANS_LIGHTNESS == k)){
			    // ONPOWER_UP_OFF only for lightness, others is same to default. please refer to LCTL/BV-06-C 
				level_poweron = LEVEL_OFF;
				set_on_power_up_last(p_save, level_poweron);
			}
			else{	// (ONPOWER_UP_DEFAULT == ONPOWER_UP_VAL(i))
                s16 def = light_g_level_def_get(i, k);
                if(ST_TRANS_LIGHTNESS == k){
                    if(LEVEL_OFF == def){
                        def = p_save->last;
                    }
                }
                #if (LIGHT_TYPE_CT_EN)
                else if(ST_TRANS_CTL_TEMP == k){
                    // "def" here is CT temp in s16, not level.
                    def = get_level_from_ctl_temp_light_idx(s16_to_u16(def), i);
                }
                #endif

                #if (0 == PTS_TEST_EN)  // PTS MMDL/SR/LCTL/BV-06 will failed because spec page 174, 6.1.3.2
			    if (ONPOWER_UP_DEFAULT == ONPOWER_UP_VAL(i) && (ST_TRANS_LIGHTNESS != k)){
			        level_poweron = get_on_power_up_last(p_save);
			    }else
			    #endif
			    {
                    level_poweron = def;
                    set_on_power_up_last(p_save, level_poweron);
                }
			} 
			
			p_trans->present = p_trans->target = level_poweron;
			#if (MD_LIGHT_CONTROL_EN)
			if(0 == i && ST_TRANS_LIGHTNESS == k){
				#if 0 // PTS_TEST_MMDL_SR_LLC_BV_11_C
				u8 onpowerup = ONPOWER_UP_VAL(i);
				u8 lc_mode = ((ONPOWER_UP_OFF == onpowerup)||(ONPOWER_UP_DEFAULT == onpowerup)) ? LC_MODE_OFF : model_sig_light_lc.mode[i];
				u8 lc_onoff = model_sig_light_lc.lc_onoff_target[i];
				LOG_MSG_LIB(TL_LOG_NODE_BASIC,0,0,"**** init **** lc mode:%d, level:%d, onpower par:%d, lc onoff:%d ****", lc_mode, level_poweron, ONPOWER_UP_VAL(i), lc_onoff);
				lc_mode_set_par(i, lc_mode);
					#if 1
				if(LC_MODE_ON == lc_mode){
					if(lc_onoff){
						#if 1 // TODO: why can not send lc onoff command ? 
						model_sig_light_lc.lc_onoff_target[i] = lc_onoff; 
						#else
						access_cmd_lc_onoff(ele_adr_primary, 0, lc_onoff, 0, 0); // why no tick set to g_lc_prop_proc_ ? now send message at "test_once_flag" of LC_property_proc_.
						#endif
					}
				}
					#endif
				#else
				lc_mode_set_par(i, model_sig_light_lc.mode[i]);
				#endif
			}
			#endif
		}
	}
}

/**
 * @brief       This function manual set light status parameter
 * @param[in]   onoff		- on or off
 * @param[in]   transit_t	- transition time parameter
 * @param[in]   light_idx	- light index if "LIGHT CNT > 1", or it is always 0.
 * @return      none
 * @note        
 */
void light_transition_onoff_manual(u8 onoff, u8 transit_t, u8 light_idx)
{
    mesh_cmd_g_onoff_set_t cmd_onoff = {0, 0, 0, 0};
    cmd_onoff.onoff = !!onoff;
    cmd_onoff.transit_t = transit_t;
    st_pub_list_t pub_list = {{0}};
	//LOG_MSG_LIB(TL_LOG_NODE_BASIC,0,0,"yyyyyyyy 666", 0);
#if (MD_LIGHT_CONTROL_EN)
	set_LC_lightness_flag = 1; // to not clear LC flow.
#endif
    g_onoff_set(&cmd_onoff, sizeof(cmd_onoff),1,light_idx,0, &pub_list);
#if (MD_LIGHT_CONTROL_EN)
	set_LC_lightness_flag = 0;
#endif
}

/**
 * @brief       This function check light edch parameter is exist
 * @return      0:not exist; 1:exist
 * @note        
 */
u8 edch_is_exist()
{
#if PROV_AUTH_LEAK_RECREATE_KEY_EN
#else
	u32 *p_edch = (u32 *) FLASH_ADR_EDCH_PARA;
	if(*p_edch == 0xffffffff){
		return 0;
	}	
#endif
	return 1;
}

/*
 * light pwm init() will cost 1.5ms when 16MHz. so it should not be called directly when retention wakeup.
 */
/**
 * @brief       This function initial light pwm register.
 * @return      none
 * @note        
 */
void light_pwm_init()
{
#if ((!IS_VC_PROJECT)&&(!__PROJECT_SPIRIT_LPN__))
    #if (MCU_CORE_TYPE == MCU_CORE_8267 || MCU_CORE_TYPE == MCU_CORE_8269)
    REG_ADDR8(0x5b1) = 0x0;     // set default  function Mux to PWM
    REG_ADDR8(0x5b4) |= 0x3;    // set default  function Mux to PWM for PE0/PE1
    #endif

    #if PWM_CLK_DIV_LIGHT
    reg_pwm_clk = PWM_CLK_DIV_LIGHT;    // default value of reg_pwm_clk is 0.
    #endif
    
    foreach(i, LIGHT_CNT){
    	foreach_arr(k,light_res_hw[0]){
			u16 level_def = 0;	// PWM_MAX_TICK;	 //
			const light_res_hw_t *p_hw = &light_res_hw[i][k];
	        pwm_set(p_hw->id, PWM_MAX_TICK, p_hw->invert ? (PWM_MAX_TICK - level_def) : level_def);
	        // light_dim_refresh(i);
	        pwm_start(p_hw->id);
	        #if((MCU_CORE_TYPE==MCU_CORE_8258) || (MCU_CORE_TYPE==MCU_CORE_8278))
	        gpio_set_func(p_hw->gpio, p_hw->func);
	        #else
	        gpio_set_func(p_hw->gpio, AS_PWM);
	        #endif
        }
        
        int onoff_present = light_g_onoff_present_get(i);
        #if (DUAL_MODE_ADAPT_EN || DUAL_MODE_WITH_TLK_MESH_EN)
        if(DUAL_MODE_SUPPORT_ENABLE == dual_mode_state){
            light_transition_onoff_manual(G_ON, 0, i);      // no transition for dual mode,because led flash: LGT_CMD_DUAL_MODE_MESH.
        }else
        #endif
        {
            light_transition_onoff_manual(G_OFF, 0, i);
            if(onoff_present && CB_USER_LIGHT_INIT_ON_CONDITION()){
				#if !MI_SWITCH_LPN_EN
				light_transition_onoff_manual(G_ON, (analog_read(DEEP_ANA_REG0)&(OTA_REBOOT_CHECK_FLAG|FLD_LOW_BATT_LOOP_FLG))?0:edch_is_exist()?g_def_trans_time_val(i):0, i);
				#endif
			}
        }
    }

    clr_keep_onoff_state_after_ota();
#endif
}
#else
void light_pwm_init(){}
void set_keep_onoff_state_after_ota(){}
#endif

static u32 tick_light_save;

/**
 * @brief       This function trigger countdown timer to save light parameter.
 * @param[in]   quick	- 1: save at once.
 * @return      none
 * @note        
 */
void light_par_save(int quick)
{
	tick_light_save = (quick ? (clock_time() - BIT(31)) : clock_time()) | 1;
}

/**
 * @brief       This function process countdown timer to save light parameter.
 * @return      none
 * @note        
 */
void light_par_save_proc()
{
	// save proc
	if(tick_light_save && clock_time_exceed(tick_light_save, 3*1000*1000)){
		tick_light_save = 0;
		if(!is_activated_factory_test_mode()){
		    mesh_common_store(FLASH_ADR_SW_LEVEL);
		}
	}
}

#if (MD_SCENE_EN)
/**
 * @brief       This function check all light scene status when change
 * @return      none
 * @note        
 */
void scene_status_change_check_all()
{
    #if MD_SERVER_EN
	foreach_arr(i,light_res_sw){
		foreach_arr(trans_type,light_res_sw[i].trans){
			st_transition_t *p_trans = P_ST_TRANS(i, trans_type);
        	scene_status_change_check(i, p_trans->present, trans_type);
	    }
	}
    #endif
}
#endif

/**
 * @brief       This function servers to set pwm CMP which determine PWM level.
 * @param[in]   id	- pwm id.
 * @param[in]   y	- variable of the CMP.
 * @param[in]   pol	- invert pwm or not.
 * @return      none
 * @note        
 */
void pwm_set_lum (int id, u16 y, int pol)
{
#if (IS_VC_PROJECT)
	#if DEBUG_MESH_DONGLE_IN_VC_EN
	extern int DebugMeshLed(int id, u16 y, int pol);
	DebugMeshLed(id, y, pol);
	#endif
#else
    u32 level = ((u32)y * PWM_MAX_TICK) / (255*256);

	pwm_set_cmp (id, pol ? PWM_MAX_TICK - level : level);
#endif
}

/**
 * @brief       This function to get pwm CMP determined by both color and luminance.
 * @param[in]   val	- value
 * @param[in]   lum	- rgb_lumen_map index
 * @return      pwm count status(CMP)
 * @note        
 */
u32 get_pwm_cmp(u8 val, u8 lum){
    if(lum >= ARRAY_SIZE(rgb_lumen_map) - 1){
        lum = ARRAY_SIZE(rgb_lumen_map) - 1;
    }
    u32 val_lumen_map = rgb_lumen_map[lum];
    
    return ((u32)val * val_lumen_map) / 255;
}

#define LIGHTNESS_AVERAGE_STEP	(65535 / (ARRAY_SIZE(rgb_lumen_map) - 1))
#define RGB_AVERAGE_STEP		(255 / (ARRAY_SIZE(rgb_lumen_map) - 1))
STATIC_ASSERT((LIGHTNESS_AVERAGE_STEP > 0) || (RGB_AVERAGE_STEP > 0));

/**
 * @brief       This function get smooth pwm value
 * @param[in]   lightness	- lightness
 * @param[in]   step		- lightness unit or step.
 * @return      pwm value
 * @note        
 */
u16 get_pwm_smooth(u16 lightness, u32 step)
{
	u8 lum = lightness / step;
	u16 lightness_remain = lightness % step;
	u16 pwm_remain = 0;
	u16 pwm_max = rgb_lumen_map[ARRAY_SIZE(rgb_lumen_map) - 1];

	if (lum >= ARRAY_SIZE(rgb_lumen_map) - 1) {
		lum = ARRAY_SIZE(rgb_lumen_map) - 1;
	}else {
		// if ((lightness_remain > 0) || (step <= 3))
		{
			pwm_remain = (lightness_remain * (rgb_lumen_map[lum + 1] - rgb_lumen_map[lum])) / (step);
		}
	}

	u32 pwm_val = rgb_lumen_map[lum] + pwm_remain;
	if (pwm_val > pwm_max) {
		pwm_val = pwm_max;
	}else if((lightness > 0) && (pwm_val < rgb_lumen_map[1])){ // pwm_val may be 0, when lightness equal to 1, so need to compare lightness, not pwm_val.
		pwm_val = rgb_lumen_map[1]; // if less than minimum PWM, some bulb may not work good.
	}

	return pwm_val;
}

/**
 * @brief  Control lights on hardware.
 * @param  idx: 1st index of light_res_hw array, also means index 
 *   of LIGHT_CNT.
 * @param  idx2: The 2nd index of light_res_hw array.
 * @param  val: Value.
 * @retval None
 */
void light_dim_set_hw(int idx, int idx2, u16 val)
{
    if((idx < ARRAY_SIZE(light_res_hw)) && (idx2 < ARRAY_SIZE(light_res_hw[0]))){
		const light_res_hw_t *p_hw = &light_res_hw[idx][idx2];
		#if FEATURE_LOWPOWER_EN
		led_onoff_gpio(p_hw->gpio, 0 != val);
		#else
        pwm_set_lum(p_hw->id, val, p_hw->invert);
        #endif
    }
}

#if (LIGHT_TYPE_HSL_EN)
typedef struct{
    float h;
    float s;
    float l;
}HSL_set;

typedef struct{
    u8 r;
    u8 g;
    u8 b;
}RGB_set;

float Hue_2_RGB(float v1,float v2,float vH){
    if(vH < 0){
        vH+=1;
    }
    if(vH > 1){
        vH-=1;
    }
    if((6.0*vH)<1){
        return (v1+(v2-v1)*6.0f*vH);
    }
    if((2.0*vH)<1){
        return (v2);
    }
    if((3.0*vH)<2){
        return (v1+(v2-v1)*((2.0f/3.0f)-vH)*6.0f);
    } 
    return(v1);
}

/**
 * @brief       This function switch HSL to RGB
 * @param[in]   hsl	- HSL
 * @param[in]   rgb	- RGB
 * @return      none
 * @note        
 */
void HslToRgb(HSL_set hsl, RGB_set *rgb)
{
	float m1,m2;
	if(hsl.s==0){	
	    rgb->r=rgb->g=rgb->b=(u8)(hsl.l*255);
	}
	else
	{
	    if(hsl.l<0.5){
		    m2=hsl.l*(1+hsl.s);
	    }
	    else{
		    m2=hsl.l+hsl.s-hsl.l*hsl.s;   	
	    }
	    m1=2*hsl.l-m2;
	    rgb->r=(u8)(255*Hue_2_RGB(m1,m2,hsl.h+(1.0f/3.0f)));
	    rgb->g=(u8)(255*Hue_2_RGB(m1,m2,hsl.h));
	    rgb->b=(u8)(255*Hue_2_RGB(m1,m2,hsl.h-(1.0f/3.0f)));
	}	
}
#endif 

#if MD_SERVER_EN
u16 g_op_access_layer_rx = 0;	// 0 means invalid op, not APPKEY_ADD.

/**
 * @brief       This function Set last Generic Level parameters for light.
 * @param[in]   idx				- index of Light Count.
 * @param[in]   st_trans_type	- A value in the enumeration type ST_TRANS_TYPE.
 * @return      none
 * @note        
 */
void light_res_sw_g_level_last_set(int idx, int st_trans_type)
{
	//set_level_current_type(idx, st_trans_type);
	sw_level_save_t *p_save = P_SW_LEVEL_SAVE(idx, st_trans_type);
	st_transition_t *p_trans = P_ST_TRANS(idx, st_trans_type);
	if(1 && (ST_TRANS_LIGHTNESS == st_trans_type) && (LEVEL_OFF == p_trans->target) && // TEST_MMDL_SR_LLN_BV10: set to "if(0)"
		is_dim2dark_set_op(g_op_access_layer_rx)){ // onoff/lightness set,etc commands have been changed to level set, so need to record op in 
		// to follow new spec, when dim to dark(lightness 0), should save min as last value but not the lightness before set to 0.
		p_save->onoff = 0;
		p_save->last = p_save->min;
	}else{
		set_on_power_up_last(p_save, p_trans->target);
	}
	light_par_save(0);
}
#endif

/**
 * @brief  Set Generic Level parameters(Global variables) for light.
 * @param  idx: index of Light Count.
 * @param  level: General Level value.
 * @param  init_time_flag: Reset transition parameter flag.
 *     @arg 0: Don't reset transition parameter.
 *     @arg 1: Reset transition parameter.
 * @param  st_trans_type: A value in the enumeration type ST_TRANS_TYPE.
 * @retval None
 */
void light_res_sw_g_level_set(int idx, s16 level, int init_time_flag, int st_trans_type)
{
	set_level_current_type(idx, st_trans_type);
	st_transition_t *p_trans = P_ST_TRANS(idx, st_trans_type);
	p_trans->present = level;
	if(init_time_flag){
		p_trans->target = level;
		p_trans->remain_t_ms = 0;
		p_trans->delay_ms = 0;
	}
}

/**
 * @brief       This function Set Generic Level target(Global variables) for light.
 * @param[in]   idx				- index of Light Count.
 * @param[in]   level			- General Level value.
 * @param[in]   st_trans_type	- A value in the enumeration type ST_TRANS_TYPE.
 * @return      none
 * @note        
 */
void light_res_sw_g_level_target_set(int idx, s16 level, int st_trans_type)	// only for move set command
{
	//set_level_current_type(idx, st_trans_type);
	st_transition_t *p_trans = P_ST_TRANS(idx, st_trans_type);
	p_trans->target = level;
	p_trans->remain_t_ms = 0;
	p_trans->delay_ms = 0;
}

/**
 * @brief       This function Control lights on hardware.
 * @param[in]   idx	- index of Light Count.
 * @return      none
 * @note        
 */
void light_dim_refresh_mi_ct(int idx)
{
	st_transition_t *p_trans_l = P_ST_TRANS(idx, ST_TRANS_LIGHTNESS);
    //calc the temp100 transfer for mi 
	u8 lum_100 = level2lum(p_trans_l->present);
    u16 mi_temp = light_ctl_temp_prensent_get(idx);
    u8 mi_ct = 0;
	#if LS_TEST_ENABLE
#define CT_MI_MIN		3000
#define CT_MI_MAX		6400
	#else
#define CT_MI_MIN		2700
#define CT_MI_MAX		6500
	#endif
    if(mi_temp<CT_MI_MIN){
        mi_ct =0;
    }else if (mi_temp > CT_MI_MAX){
        mi_ct = 100;
    }else {
        mi_ct = ((mi_temp-CT_MI_MIN)*100)/(CT_MI_MAX-CT_MI_MIN);
    }
    light_dim_set_hw(idx, 0, 0xffff);// turn on the filter part 
    light_dim_set_hw(idx, 1, get_pwm_cmp(0xff, mi_ct*lum_100/100));
    light_dim_set_hw(idx, 2, get_pwm_cmp(0xff, (100-mi_ct)*lum_100/100));
}

// --------------------------------
/**
 * @brief  Refresh the light status through the global variables 
 *   that have been set.
 * @param  idx: index of Light Count.
 * @retval None
 */
_USER_CAN_REDEFINE_ void light_dim_refresh(int idx) // idx: index of LIGHT_CNT.
{
	st_transition_t *p_trans_l = P_ST_TRANS(idx, ST_TRANS_LIGHTNESS);
	// u16 lightness = get_lightness_from_level(p_trans->present);
#if (LIGHT_TYPE_SEL != LIGHT_TYPE_HSL)
	u32 lightness_65535 = s16_to_u16(p_trans_l->present);
#endif

    //LOG_MSG_INFO(DEBUG_SHOW_VC_SELF_EN ? TL_LOG_COMMON : TL_LOG_MESH,0,0,"present_lum %d", lum_100);
	CB_NL_PAR_NUM_3(p_nl_level_state_changed,idx * ELE_CNT_EVERY_LIGHT + ST_TRANS_LIGHTNESS, p_trans_l->present, p_trans_l->target);
#if (FEATURE_LOWPOWER_EN || GATT_LPN_EN|| LPN_CONTROL_EN)
    foreach_arr(i,light_res_hw[LIGHT_CNT]){
        const light_res_hw_t *p_hw = &light_res_hw[idx][i];
        led_onoff_gpio(p_hw->gpio, 0 != lightness_65535);
    }
	return ;
#else
    #if(!(LIGHT_TYPE_CT_EN || LIGHT_TYPE_HSL_EN))
    light_dim_set_hw(idx, 0, get_pwm_smooth(lightness_65535, LIGHTNESS_AVERAGE_STEP));
    #else
	    #if (LIGHT_TYPE_CT_EN)
            #if (XIAOMI_MODULE_ENABLE&&!AIS_ENABLE)
    p_trans_l = p_trans_l; // will be optimized
    lightness_65535 = lightness_65535; // will be optimized
	light_dim_refresh_mi_ct(idx);
            #else
	st_transition_t *p_trans_ct = P_ST_TRANS(idx, ST_TRANS_CTL_TEMP);
	u32 ct_65535 = s16_to_u16(p_trans_ct->present);
    if(ct_flag && (lightness_65535 != 0)){
		#if 0 // to get color temperature value from level which store in flash.
	    u16 color_temperature = light_ctl_temp_prensent_get(idx); // range from CTL_TEMP_MIN(800) to CTL_TEMP_MAX(20000) by default.
	    u8 ct_100 = temp_to_temp100_hw(color_temperature);		  // transform to range from 0 to 100;
		static u8 debug_ct_100; debug_ct_100 = ct_100;
		#endif
    }

    if(ct_flag){
		u32 warn_led_lightness = ((65535 - ct_65535) * lightness_65535) / 65535;
		u32 cold_led_lightness = (ct_65535 * lightness_65535) / 65535;
		u32 warn_led_pwm = get_pwm_smooth(warn_led_lightness, LIGHTNESS_AVERAGE_STEP);
		u32 cold_led_pwm = get_pwm_smooth(cold_led_lightness, LIGHTNESS_AVERAGE_STEP);
		light_dim_set_hw(idx, 0, warn_led_pwm);
		light_dim_set_hw(idx, 1, cold_led_pwm); 
		
		//LOG_MSG_LIB(TL_LOG_NODE_SDK,0,0,"pwm:0x%04x,0x%04x", warn_led_pwm, cold_led_pwm);
    }
            #endif   
        #endif
        
        #if (LIGHT_TYPE_HSL_EN)
    if(!ct_flag){
        //u32 r = irq_disable();
        //u32 tick_1 = clock_time();
        st_transition_t *p_hue = P_ST_TRANS(idx, ST_TRANS_HSL_HUE);
        st_transition_t *p_sat = P_ST_TRANS(idx, ST_TRANS_HSL_SAT);
        HSL_set HSL;
        RGB_set RGB;
        HSL.h = ((float)(s16_to_u16(p_hue->present))/65535.0f);
        HSL.s = ((float)(s16_to_u16(p_sat->present))/65535.0f);
        HSL.l = ((float)(s16_to_u16(p_trans_l->present))/65535.0f);
        HslToRgb(HSL,&RGB);
        light_dim_set_hw(idx, 0, get_pwm_smooth(RGB.r, RGB_AVERAGE_STEP));
        light_dim_set_hw(idx, 1, get_pwm_smooth(RGB.g, RGB_AVERAGE_STEP));
        light_dim_set_hw(idx, 2, get_pwm_smooth(RGB.b, RGB_AVERAGE_STEP));
        //static u32 A_1[100];
        // static u32 A_1_cnt;
        //if(A_1_cnt < 100){
        //A_1[A_1_cnt++] = (clock_time()-tick_1)/32;
        //}
        //irq_restore(r);
    }
	    #endif
    #endif
#endif
}

/**
 * @brief      This function get light publish list
 * @param[in]  st_trans_type	- A value in the enumeration type ST_TRANS_TYPE.
 * @param[in]  present_level	- present level
 * @param[in]  target_level		- target level
 * @param[in]  pub_trans_flag	- publish transmit flag
 * @param[out] pub_list			- publish list
 * @return     none
 * @note        
 */
void get_light_pub_list(int st_trans_type, s16 present_level, s16 target_level, int pub_trans_flag, st_pub_list_t *pub_list)
{
    if(pub_trans_flag){
        pub_list->st[st_trans_type] = ST_G_LEVEL_SET_PUB_TRANS;
    }else{
        pub_list->st[st_trans_type] = (target_level != present_level) ? ST_G_LEVEL_SET_PUB_NOW : ST_G_LEVEL_SET_PUB_NONE;
    }
    if(ST_TRANS_LIGHTNESS == st_trans_type){
        if(pub_list->st[ST_TRANS_LIGHTNESS] && ((LEVEL_OFF == present_level) || (LEVEL_OFF == target_level))){
            pub_list->st[ST_TRANS_PUB_ONOFF] = pub_list->st[ST_TRANS_LIGHTNESS];
        }
    }
}

/**
 * @brief  Set Generic Level for light.
 * @param  idx: index of Light Count.
 * @param  level: General Level value.
 * @param  init_time_flag: Reset transition parameter flag.
 *     @arg 0: Don't reset transition parameter.
 *     @arg 1: Reset transition parameter.
 * @param  st_trans_type: A value in the enumeration type ST_TRANS_TYPE.
 * @param  pub_list: list of publication.
 * @retval Whether the function executed successfully
 *   (0: success; others: error)
 */
int light_g_level_set(int idx, s16 level, int init_time_flag, int st_trans_type, st_pub_list_t *pub_list)
{
	st_transition_t *p_trans = P_ST_TRANS(idx, st_trans_type);
    //if(level != p_trans->present){
        if(pub_list){
            get_light_pub_list(st_trans_type, p_trans->present, level, 0, pub_list);
        }
    	light_res_sw_g_level_set(idx, level, init_time_flag, st_trans_type);
        #if 0  // if support generic level server model
        u16 power = light_power_actual_get(idx);
        light_power_actual_set_par(idx, power, init_time_flag);
        #endif
        
        if((NULL == pub_list) || (0 == pub_list->no_dim_refresh_flag)){
            light_dim_refresh(idx);
        }
    //}
    return 0;
}

//------------------ model call back func
//------get function
/**
 * @brief       This function get light status change remain time
 * @param[in]   p_trans	- light parameter
 * @return      remain time
 * @note        
 */
u8 light_remain_time_get(st_transition_t *p_trans)
{
	u32 remain_ms = p_trans->remain_t_ms;
	u32 delay_ms = p_trans->delay_ms;

	u8 remain_t = get_transition_step_res((remain_ms+99)/100);
	if(0 == remain_t){
		remain_t = get_transition_step_res((delay_ms+99)/100);
	}
	return remain_t;
}

/**
 * @brief       This function get light level
 * @param[out]  rsp				- pointer to response data
 * @param[in]   idx				- light index if "LIGHT CNT > 1", or it is always 0.
 * @param[in]   st_trans_type	- A value in the enumeration type ST_TRANS_TYPE.
 * @return      0:get success; -1:get fail
 * @note        
 */
int light_g_level_get(u8 *rsp, int idx, int st_trans_type)
{
	if(idx < LIGHT_CNT){
		set_level_current_type(idx, st_trans_type);
		mesh_cmd_g_level_st_t *p_st = (mesh_cmd_g_level_st_t *)rsp;
		st_transition_t *p_trans = P_ST_TRANS(idx, st_trans_type);
		p_st->present_level = p_trans->present;
		p_st->target_level = p_trans->target;
		p_st->remain_t = light_remain_time_get(p_trans);
		return 0;
	}
	return -1;
}

/**
 * @brief  Get Light CTL Default Value.
 * @param  idx: index of Light Count.
 * @param  st_trans_type: A value in the enumeration type ST_TRANS_TYPE.
 * @retval Light CTL Default Value.
 */
s16 light_g_level_def_get(int idx, int st_trans_type)
{
	if(idx < LIGHT_CNT){
		sw_level_save_t *p_save = P_SW_LEVEL_SAVE(idx, st_trans_type);
		return p_save->def;
	}
	return 0;
}

/**
 * @brief  Get Light CTL Default Value.
 * @param  idx: index of Light Count.
 * @param  st_trans_type: A value in the enumeration type ST_TRANS_TYPE.
 * @retval Light CTL Default Value.
 */
u16 light_g_level_def_get_u16(int idx, int st_trans_type)
{
	return s16_to_u16(light_g_level_def_get(idx, st_trans_type));
}

/**
 * @brief  Set Light CTL Default Value.
 * @param  val: Light CTL Default Value.
 * @param  idx: index of Light Count.
 * @param  st_trans_type: A value in the enumeration type ST_TRANS_TYPE.
 * @retval Whether the function executed successfully
 *   (0: success; others: error)
 */
int light_g_level_def_set(s16 val, int idx, int st_trans_type)
{
	if(idx < LIGHT_CNT){
		//set_level_current_type(idx, st_trans_type);
		sw_level_save_t *p_save = P_SW_LEVEL_SAVE(idx, st_trans_type);
		p_save->def = val;
		light_par_save(1);
		return 0;
	}
	return -1;
}

/**
 * @brief  Set Light CTL Default Value.
 * @param  val: Light CTL Default Value.
 * @param  idx: index of Light Count.
 * @param  st_trans_type: A value in the enumeration type ST_TRANS_TYPE.
 * @retval Whether the function executed successfully
 *   (0: success; others: error)
 */
int light_g_level_def_set_u16(u16 val, int idx, int st_trans_type)
{
	return light_g_level_def_set(u16_to_s16(val), idx, st_trans_type);
}

/**
 * @brief  Get level range value.
 * @param  p_range: Point to Light Range value.
 * @param  idx: index of Light Count.
 * @param  st_trans_type: A value in the enumeration type ST_TRANS_TYPE.
 * @retval Whether the function executed successfully
 *   (0: success; others: error)
 */
int light_g_level_range_get(light_range_s16_t *p_range, int idx, int st_trans_type)
{
	if(idx < LIGHT_CNT){
		//set_level_current_type(idx, st_trans_type);
		sw_level_save_t *p_save = P_SW_LEVEL_SAVE(idx, st_trans_type);
		p_range->min = p_save->min;
		p_range->max = p_save->max;
		return 0;
	}else{
		memset(p_range, 0, sizeof(light_range_s16_t));
		return -1;
	}
}

/**
 * @brief  Get Light CTL Temperature Range value.
 * @param  p_range: Point to Light CTL Temperature Range value.
 * @param  idx: index of Light Count.
 * @param  st_trans_type: A value in the enumeration type ST_TRANS_TYPE.
 * @retval Whether the function executed successfully
 *   (0: success; others: error)
 */
int light_g_level_range_get_u16(light_range_u16_t *p_range, int idx, int st_trans_type)
{
	light_range_s16_t range_s16 = {0};
	int err = light_g_level_range_get(&range_s16, idx, st_trans_type);
	p_range->min = get_lightness_from_level(range_s16.min);
	p_range->max = get_lightness_from_level(range_s16.max);
	return err;
}


/**
 * @brief  Set Light (Lightness/CTL Temperature/HSL/xyL) Range value.
 * @param  min: Range Min.
 * @param  max: Range Max.
 * @param  idx: index of Light Count.
 * @param  st_trans_type: A value in the enumeration type ST_TRANS_TYPE.
 * @retval Whether the function executed successfully
 *   (0: success; others: error)
 */
int light_g_level_range_set(u16 min, u16 max, int idx, int st_trans_type)
{
	if(idx < LIGHT_CNT){
		//set_level_current_type(idx, st_trans_type);
		sw_level_save_t *p_save = P_SW_LEVEL_SAVE(idx, st_trans_type);
		p_save->min = get_level_from_lightness(min);
		p_save->max = get_level_from_lightness(max);
		light_par_save(1);
		return 0;
	}
	return -1;
}

/**
 * @brief  Get light target level value.
 * @param  idx: index of Light Count.
 * @param  st_trans_type: A value in the enumeration type ST_TRANS_TYPE.
 * @retval light target level value.
 */
s16 light_g_level_target_get(int idx, int st_trans_type)
{
	if(idx < LIGHT_CNT){
		set_level_current_type(idx, st_trans_type);
		st_transition_t *p_trans = P_ST_TRANS(idx, st_trans_type);
		return p_trans->target;
	}
	return 0;
}

/**
 * @brief  Get light present level value.
 * @param  idx: index of Light Count.
 * @param  st_trans_type: A value in the enumeration type ST_TRANS_TYPE.
 * @retval light present level value.
 */
s16 light_g_level_present_get(int idx, int st_trans_type)
{
	if(idx < LIGHT_CNT){
		set_level_current_type(idx, st_trans_type);
		st_transition_t *p_trans = P_ST_TRANS(idx, st_trans_type);
		return p_trans->present;
	}
	return 0;
}

/**
 * @brief       This function get light present level and transition to u16
 * @param[in]   idx				- light index if "LIGHT CNT > 1", or it is always 0.
 * @param[in]   st_trans_type	- A value in the enumeration type ST_TRANS_TYPE.
 * @return      light level
 * @note        
 */
u16 light_g_level_present_get_u16(int idx, int st_trans_type)
{
	return s16_to_u16(light_g_level_present_get(idx, st_trans_type));
}

/**
 * @brief       This function  get light present level
 * @param[in]   idx	- light index if "LIGHT CNT > 1", or it is always 0.
 * @return      light level
 * @note        
 */
u8 light_g_onoff_present_get(int idx)
{
    return (light_g_level_present_get(idx, ST_TRANS_LIGHTNESS) != LEVEL_OFF);
}

/**
 * @brief       This function get last light level
 * @param[in]   idx				- light index if "LIGHT CNT > 1", or it is always 0.
 * @param[in]   st_trans_type	- A value in the enumeration type ST_TRANS_TYPE.
 * @return      0:get fail; other:last light level
 * @note        
 */
s16 light_g_level_last_get(int idx, int st_trans_type)
{
	if(idx < LIGHT_CNT){
		//set_level_current_type(idx, st_trans_type);
		sw_level_save_t *p_save = P_SW_LEVEL_SAVE(idx, st_trans_type);
		return p_save->last;
	}
	return 0;
}

/**
 * @brief       This function get light level by onoff
 * @param[in]   idx				- light index if "LIGHT CNT > 1", or it is always 0.
 * @param[in]   on				- onoff cmd
 * @param[in]   st_trans_type	- A value in the enumeration type ST_TRANS_TYPE.
 * @param[in]   force_last		- the last value is enforced
 * @return      light level
 * @note        
 */
s16 get_light_g_level_by_onoff(int idx, int on, int st_trans_type, int force_last)
{
	if(on){
	    s16 last = light_g_level_last_get(idx, st_trans_type);
		if(ST_TRANS_LIGHTNESS == st_trans_type){
            s16 def = light_g_level_def_get(idx, st_trans_type);
		    return ((force_last || (LEVEL_OFF == def)) ? last : def); // refer to Lightness LLN-BV11-C
		}else{
			return last;
		}
	}else{
		return LEVEL_OFF;
	}
}

#if (LIGHT_TYPE_CT_EN)
/**
 * @brief       This function get light color temperature
 * @param[in]   light_idx	- light index if "LIGHT CNT > 1", or it is always 0.
 * @param[in]   target_flag	- is use target flag
 * @return      temp
 * @note        
 */
u16 light_ctl_temp_get_ll(int light_idx, int target_flag)
{
    u16 temp = 0;
	if(light_idx < LIGHT_CNT){
		st_transition_t *p_trans = P_ST_TRANS(light_idx, ST_TRANS_CTL_TEMP);
        light_range_u16_t range_u16 = {0};
        light_g_level_range_get_u16(&range_u16, light_idx, ST_TRANS_CTL_TEMP);
		temp = get_ctl_temp_from_level(target_flag ? p_trans->target : p_trans->present, range_u16.min, range_u16.max);
	}
	return temp;
}

/**
 * @brief       This function get present color temperature
 * @param[in]   light_idx	- light index if "LIGHT CNT > 1", or it is always 0.
 * @return      color temperature
 * @note        
 */
u16 light_ctl_temp_prensent_get(int light_idx)
{
	return light_ctl_temp_get_ll(light_idx, 0);
}

/**
 * @brief       This function target color temperature
 * @param[in]   light_idx	- light index if "LIGHT CNT > 1", or it is always 0.
 * @return      color temperature
 * @note        
 */
u16 light_ctl_temp_target_get(int light_idx)
{
	return light_ctl_temp_get_ll(light_idx, 1);
}
#endif

/**
 * @brief       This function lum to level
 * @param[in]   lum	- lum
 * @return      level
 * @note        
 */
s16 lum2level(u8 lum)
{
	if(lum > 100){
		lum  = 100;
	}
	return (-32768 + division_round(65535*lum,100));
}

/**
 * @brief       This function level to luminance which range 0 ~ 100.
 * @param[in]   level	- level value
 * @return      lum
 * @note        
 */
u8 level2lum(s16 level)
{
	u16 lightness = level + 32768;
	u32 fix_1p2 = 0;
	if(lightness){	// fix decimals
		#define LEVEL_UNIT_1P2	(65535/100/2)
		if(lightness < LEVEL_UNIT_1P2 + 2){     // +2 to fix accuracy missing
			lightness = LEVEL_UNIT_1P2 * 2;		// make sure lum is not zero when light on.
		}
		fix_1p2 = LEVEL_UNIT_1P2;
	}
	return (((lightness + fix_1p2)*100)/65535);
}

/**
 * @brief       This function get lightess from luminance which range 0 ~ 100.
 * @param[in]   lum	- lum value
 * @return      lightess
 * @note        
 */
u16 lum2_lightness(u8 lum)
{
	return (get_lightness_from_level(lum2level(lum)));
}

/**
 * @brief       This function get lum from lightess
 * @param[in]   lightness	- lightess value
 * @return      lum
 * @note        
 */
u8 lightness2_lum(u16 lightness)
{
	return (level2lum(get_level_from_lightness(lightness)));
}

/**
 * @brief       This function temp(0-100) to temp(CTL_TEMP_MIN - CTL_TEMP_MAX)
 * @param[in]   temp100	- temp100 value
 * @return      color temperature
 * @note        
 */
u16 temp100_to_temp(u8 temp100)
{
	if(temp100 > 100){
		temp100  = 100;
	}
	return (CTL_TEMP_MIN + ((CTL_TEMP_MAX - CTL_TEMP_MIN)*temp100)/100);
}

/**
 * @brief       This function temp(CTL_TEMP_MIN - CTL_TEMP_MAX) to temperature100(0-100)
 * @param[in]   temp- color temperature value
 * @return      temperature100
 * @note        
 */
u8 temp_to_temp100_hw(u16 temp) // use for driver pwm, 0--100 is absolute value, not related to temp range
{
	if(temp < CTL_TEMP_MIN){
		temp = CTL_TEMP_MIN;
	}
	
	if(temp > CTL_TEMP_MAX){
		temp = CTL_TEMP_MAX;
	}
	u32 fix_1p2 = (CTL_TEMP_MAX - CTL_TEMP_MIN)/100/2;	// fix decimals
	return (((temp - CTL_TEMP_MIN + fix_1p2)*100)/(CTL_TEMP_MAX - CTL_TEMP_MIN));   // temp100 can be zero.
}

/**
 * @brief       This function temp(CTL_TEMP_MIN - CTL_TEMP_MAX) to temperature100(0-100)
 * @param[in]   temp- temperature
 * @return      color temperature value(0-100)
 * @note        
 */
u8 temp_to_temp100(u16 temp)
{
	return temp_to_temp100_hw(temp);   // confirm later, related with temp range
}

/**
 * @brief       This function get light luminance
 * @param[in]   idx			- light index if "LIGHT CNT > 1", or it is always 0.
 * @param[in]   target_flag	- is get target flag
 * @return      lum
 * @note        
 */
u8 light_lum_get(int idx, int target_flag)
{
	st_transition_t *p_trans = P_ST_TRANS(idx, ST_TRANS_LIGHTNESS);
    return level2lum(target_flag? p_trans->target : p_trans->present);
}

#if (LIGHT_TYPE_CT_EN)
/**
 * @brief       This function get temp100
 * @param[in]   idx			- light index if "LIGHT CNT > 1", or it is always 0.
 * @param[in]   target_flag	- is get target flag
 * @return      temp100
 * @note        
 */
u8 light_ct_lum_get(int idx, int target_flag)
{
    return temp_to_temp100(light_ctl_temp_get_ll(idx, target_flag));
}
#endif

//------set function
#if 1
/**
 * @brief       This function set light level
 * @param[in]   idx				- light index if "LIGHT CNT > 1", or it is always 0.
 * @param[in]   on				- on or off
 * @param[in]   init_time_flag	- is init target flag
 * @return      0
 * @note        
 */
int light_onoff_idx(int idx, int on, int init_time_flag){
    if(idx < LIGHT_CNT){
    	int st_trans_type = ST_TRANS_LIGHTNESS;
    	st_pub_list_t pub_list = {{0}};
    	light_g_level_set(idx, get_light_g_level_by_onoff(idx, on, st_trans_type, 1), init_time_flag, st_trans_type, &pub_list);
    }
    return 0;
}
#endif

/**
 * @brief  Set Generic Level for light by index.
 * @param  idx: index of Light Count.
 * @param  level: General Level value.
 * @param  init_time_flag: Reset transition parameter flag.
 *     @arg 0: Don't reset transition parameter.
 *     @arg 1: Reset transition parameter.
 * @param  st_trans_type: A value in the enumeration type ST_TRANS_TYPE.
 * @param  pub_list: list of publication.
 * @retval Whether the function executed successfully
 *   (0: success; others: error)
 */
int light_g_level_set_idx(int idx, s16 level, int init_time_flag, int st_trans_type, st_pub_list_t *pub_list)
{
    if(idx < LIGHT_CNT){
        light_g_level_set(idx, level, init_time_flag, st_trans_type, pub_list);
    }
    return 0;
}

#if MD_SERVER_EN
/**
 * @brief       This function set level with transition time.
 * @param[in]   set_trans		- transition time parameters
 * @param[in]   idx				- light index if "LIGHT CNT > 1", or it is always 0.
 * @param[in]   st_trans_type	- transition type
 * @param[in]   hsl_set_cmd_flag- flag of HSL set command.
 * @return      none
 * @note        
 */
void light_g_level_set_idx_with_trans(u8 *set_trans, int idx, int st_trans_type, int hsl_set_cmd_flag)
{
    if(idx < LIGHT_CNT){
		set_level_current_type(idx, st_trans_type);
		st_transition_t *p_trans = P_ST_TRANS(idx, st_trans_type);
		mesh_set_trans_t *p_set = (mesh_set_trans_t *)set_trans;
		p_trans->target = p_set->target_val;
		if(0x3F == (p_set->transit_t & 0x3F)){
			p_trans->remain_t_ms = -1;
			p_trans->delay_ms =  p_set->delay * 5; // 5 is unit define by spec
		}else{
			#if 0 // no lc_prop_time_ms now, because property time has been set to transition parameter of access_cmd_set_LC_lightness_().
		    if(p_set->lc_prop_time_ms){
			    p_trans->remain_t_ms = p_set->lc_prop_time_ms;
			}else
			#endif
			{
                p_trans->remain_t_ms = 100 * get_transition_100ms((trans_time_t *)&p_set->transit_t) - p_set->dim2dark_delay_ms;
			}
			
			if(p_trans->remain_t_ms){
                if(is_level_move_set_op(p_set->op)){
                    // have been make sure (target_val != present_val) and (level_move != 0) before.
				    p_trans->step_1p32768 = ((p_set->level_move * 32768) /(s32)(p_trans->remain_t_ms)) * LIGHT_ADJUST_INTERVAL;
                    u32 abs_step = abs(p_set->level_move);
                    u32 abs_delta = abs(p_set->target_val - p_set->present_val);
                    u32 mod = abs_delta % abs_step;
                    u32 remain_t_ms_org = p_trans->remain_t_ms; // max value is (RES_10MIN * 64)
                    u32 val = 0;
                    u32 multi_value = 1;
                    if(remain_t_ms_org >= (65536 * 2)){
                    	multi_value = 1024;
                    	remain_t_ms_org /= multi_value;
                    }

                    val = (remain_t_ms_org * mod) / abs_step;
                    p_trans->remain_t_ms = ((remain_t_ms_org * abs_delta) / abs_step) * multi_value + val;
					//LOG_MSG_LIB(TL_LOG_NODE_BASIC,0,0,"move time, result: %d, remain_t_ms_org: %d, delta: %d, step: %d, val: %d", p_trans->remain_t_ms, remain_t_ms_org, abs_delta, abs_step, val);
                }else{
					s32 delta = (p_trans->target - p_trans->present);
					#if LIGHT_TYPE_HSL_EN
                    st_transition_t *p_trans_lightness = P_ST_TRANS(idx, ST_TRANS_LIGHTNESS);
                    st_transition_t *p_trans_hue = P_ST_TRANS(idx, ST_TRANS_HSL_HUE);
                    st_transition_t *p_trans_sat = P_ST_TRANS(idx, ST_TRANS_HSL_SAT);
                    int flag_keep_color = (hsl_set_cmd_flag && ((ST_TRANS_HSL_HUE == st_trans_type) || (ST_TRANS_HSL_SAT == st_trans_type)));
					if(flag_keep_color){
					    if((LEVEL_OFF == p_trans_lightness->present)
					    && (LEVEL_OFF == p_trans_hue->present)
					    && (LEVEL_OFF == p_trans_sat->present)){ // ON by HSL set command
                            p_trans_hue->present = p_trans_hue->target;
                            p_trans_sat->present = p_trans_sat->target;
					    }
					}
					
					#if(TESTCASE_FLAG_ENABLE == 0)//MMDL/SR/LHSLH/BV-04-C need process of pts
					if(ST_TRANS_HSL_HUE == st_trans_type){
					    // hue is a circular parameter from 0~360 degree
					    delta = get_Hue_delta_value(s16_to_u16(p_trans->target), s16_to_u16(p_trans->present));
					}
					#endif
					#endif
					
				    p_trans->step_1p32768 = ((delta * 32768) /(s32)(p_trans->remain_t_ms)) * LIGHT_ADJUST_INTERVAL;

                    #if LIGHT_TYPE_HSL_EN
					if(flag_keep_color){
					    if((LEVEL_OFF == p_trans_lightness->target)
					    && (LEVEL_OFF == p_trans_hue->target)
					    && (LEVEL_OFF == p_trans_sat->target)){ // OFF by HSL set command
                            p_trans_hue->step_1p32768 = 0; // will be set to target directly when transition complete
                            p_trans_sat->step_1p32768 = 0;
					    }
					}
                    #endif
				}
			}else{
				p_trans->step_1p32768 = 0;
			}
			
			p_trans->present_1p32768 = 0; // init
			p_trans->delay_ms = min(p_set->delay * 5 + p_set->dim2dark_delay_ms, BIT_MASK_LEN(sizeof(p_trans->delay_ms) * 8)); // "* 5" is unit define by spec
		}
    }
}
#endif

/**
 * @brief       This function set all light onoff
 * @param[in]   on	- on or off
 * @return      none
 * @note        
 */
void light_onoff_all(u8 on){
    foreach(i, LIGHT_CNT){
        light_onoff_idx(i, on, 1);
    }
}

#if CMD_LINEAR_EN
/**
 * @brief       This function set light linear
 * @param[in]   idx		- light index if "LIGHT CNT > 1", or it is always 0.
 * @param[in]   linear	- The target value of the Light Lightness Linear state
 * @return      0
 * @note        
 */
int set_light_linear_flag(int idx,u16 linear)
{
	if(idx < LIGHT_CNT){
	    light_res_sw[idx].linear_set_flag = 1;
	    light_res_sw[idx].linear = linear;
	}
	return 0;
}

/**
 * @brief       This function clear light liner flag
 * @param[in]   idx	- light index if "LIGHT CNT > 1", or it is always 0.
 * @return      0
 * @note        
 */
int clear_light_linear_flag(int idx)
{
	if(idx < LIGHT_CNT){
	    light_res_sw[idx].linear_set_flag = 0;
	    light_res_sw[idx].linear = 0;
	}
	return 0;
}

/**
 * @brief       This function get light linear value
 * @param[in]   idx	- light index if "LIGHT CNT > 1", or it is always 0.
 * @return      linear value
 * @note        
 */
u16 get_light_linear_val(int idx)
{
    return light_res_sw[idx].linear;
}

/**
 * @brief       This function light is exist linear flag
 * @param[in]   idx	- light index if "LIGHT CNT > 1", or it is always 0.
 * @return      0:not linear_flag; other:linear_flag
 * @note        
 */
int is_linear_flag(int idx)
{
	if(idx < LIGHT_CNT){
	    return light_res_sw[idx].linear_set_flag;
	}
	return 0;
}
#endif

/**
 * @brief       This function set light level
 * @param[in]   id	- light index if "LIGHT CNT > 1", or it is always 0.
 * @param[in]   en	- on or off
 * @return      none
 * @note        
 */
void app_led_en (int id, int en)
{
    if(id < LIGHT_CNT){
        light_onoff_idx(id, en, 1);
    }
}

#if MD_SERVER_EN
// transition and delay proc
/**
 * @brief       This function get next step light level
 * @param[in]   idx				- light level
 * @param[in]   st_trans_type	- A value in the enumeration type ST_TRANS_TYPE.
 * @return      next step light level
 * @note        
 */
s16 light_get_next_level(int idx, int st_trans_type)
{
    st_transition_t *p_trans = P_ST_TRANS(idx, st_trans_type);
    sw_level_save_t *p_save = P_SW_LEVEL_SAVE(idx, st_trans_type);
	s32 adjust_1p32768 = p_trans->step_1p32768+ p_trans->present_1p32768;
	s32 result = p_trans->present + (adjust_1p32768 / 32768);
	p_trans->present_1p32768 = adjust_1p32768 % 32768;

	#if LIGHT_TYPE_HSL_EN
	if(ST_TRANS_HSL_HUE == st_trans_type){   // hue is a circular parameter from 0~360 degree
	    #if (HSL_HUE_MAX < 0xffff)
        result = s16_to_u16((s16)result);
	    u16 logic_hue_max = HSL_HUE_MAX;
        if(result > logic_hue_max){
            if(adjust_1p32768 < 0){
                result = HSL_HUE_MAX - (0xffff - result);
            }
            result %= (HSL_HUE_MAX + 1);
        }
        result = u16_to_s16((u16)result);
	    #endif
	}else
	#endif
	{
        result = get_val_with_check_range(result, LEVEL_OFF, p_save->max, st_trans_type, G_LEVEL_SET_NOACK); // use LEVEL_MIN but not p_save->min, because it is during transition process.
        if((LEVEL_OFF == result) && (ST_TRANS_LIGHTNESS == st_trans_type) && (p_trans->step_1p32768 > 0)){
        	// result = LEVEL_OFF + 1; //
        }
    }
	
	return (s16)result;
}

void light_transition_log(int st_trans_type, s16 present_level)
{
#if 0
	if(ST_TRANS_LIGHTNESS == st_trans_type){
		LOG_MSG_LIB(TL_LOG_NODE_BASIC,0,0,"present_lightness 0x%04x", s16_to_u16(present_level));
	#if (LIGHT_TYPE_CT_EN)
	}else if(ST_TRANS_CTL_TEMP == st_trans_type){
		LOG_MSG_LIB(TL_LOG_NODE_BASIC,0,0,"present_ctl_temp 0x%04x", s16_to_u16(present_level));
	}else if(ST_TRANS_CTL_D_UV == st_trans_type){
		LOG_MSG_LIB(TL_LOG_NODE_BASIC,0,0,"present_ctl_D_UV %d", present_level);
	#endif
	#if (LIGHT_TYPE_HSL_EN)
	}else if(ST_TRANS_HSL_HUE == st_trans_type){
		LOG_MSG_LIB(TL_LOG_NODE_BASIC,0,0,"present_hsl_hue 0x%04x", s16_to_u16(present_level));
	}else if(ST_TRANS_HSL_SAT == st_trans_type){
		LOG_MSG_LIB(TL_LOG_NODE_BASIC,0,0,"present_hsl_sat 0x%04x", s16_to_u16(present_level));
	#endif
	#if (LIGHT_TYPE_SEL == LIGHT_TYPE_XYL)
	}else if(ST_TRANS_XYL_X == st_trans_type){
		LOG_MSG_LIB(TL_LOG_NODE_BASIC,0,0,"present_xyl_x 0x%04x", s16_to_u16(present_level));
	}else if(ST_TRANS_XYL_Y == st_trans_type){
		LOG_MSG_LIB(TL_LOG_NODE_BASIC,0,0,"present_xyl_y 0x%04x", s16_to_u16(present_level));
	#endif
	}else{
		LOG_MSG_LIB(TL_LOG_NODE_BASIC,0,0,"xxxx 0x%04x", s16_to_u16(present_level));
	}
#endif
}

/**
 * @brief       This function process light transition which include delay time and transition time.
 * @return      0:gradient compete; 1:gradient  not compete
 * @note        
 */
int light_transition_proc()
{
	int transiting_flag = 0;
    int all_trans_ok = 1;   // include no transition
	foreach_arr(i,light_res_sw){
	    int dim_refresh_flag = 0;
		foreach_arr(trans_type,light_res_sw[i].trans){
			st_transition_t *p_trans = P_ST_TRANS(i, trans_type);
			int complete_level = 0;
			if(p_trans->delay_ms){
				p_trans->delay_ms--;
				transiting_flag = 1;
				if((0 == p_trans->delay_ms) && (0 == p_trans->remain_t_ms)){
					complete_level = 1;
				}else{
				    all_trans_ok = 0;
				}
			}else{
				if(p_trans->remain_t_ms){
					transiting_flag = 1;
					if(p_trans->present != p_trans->target){
						if(0 == (p_trans->remain_t_ms % LIGHT_ADJUST_INTERVAL)){
                            s16 next_val = light_get_next_level(i, trans_type);
                            st_pub_list_t pub_list = {{0}};
                            pub_list.no_dim_refresh_flag = 1;
                            dim_refresh_flag = 1;
							light_g_level_set_idx(i, next_val, 0, trans_type, &pub_list);
							light_transition_log(trans_type, p_trans->present);
						}
					}

					#if 1
					p_trans->remain_t_ms--; // should keep remain time for Binary state transitions from 0x00 to 0x01,.
					#else
					if(p_trans->present != p_trans->target){
						p_trans->remain_t_ms--;
					}else{
						p_trans->remain_t_ms = 0;
					}
					#endif
					
					if(0 == p_trans->remain_t_ms){
						complete_level = 1;	// make sure the last status is same with target
					}else{
				        all_trans_ok = 0;
				    }
				}
			}

			if(complete_level){
                st_pub_list_t pub_list = {{0}};
                pub_list.no_dim_refresh_flag = 1;
                dim_refresh_flag = 1;
				light_g_level_set_idx(i, p_trans->target, 0, trans_type, &pub_list);
				light_transition_log(trans_type, p_trans->present);

                #if MD_SCENE_EN
				scene_target_complete_check(i);
				#endif
				
                #if MD_LIGHT_CONTROL_EN
                if(ST_TRANS_LIGHTNESS == trans_type){
					// LOG_MSG_LIB(TL_LOG_NODE_BASIC,0,0,"wwwww light transition complete", 0);
                    LC_light_transition_complete_handle(i);
                }
                #endif
			}
		}
		
		if(dim_refresh_flag){
            light_dim_refresh(i);   // dim refresh only when all level set ok
		}
	}

	if(all_trans_ok){
	    if(light_pub_trans_step){
	        light_pub_trans_step = ST_PUB_TRANS_ALL_OK;
	    }
	}

	return transiting_flag;
}

/**
 * @brief       This function stop transition process.
 * @param[in]   light_idx		- light index if "LIGHT CNT > 1", or it is always 0.
 * @param[in]   st_trans_type	- A value in the enumeration type ST_TRANS_TYPE.
 * @return      none
 * @note        
 */
void light_transition_proc_stop(int light_idx, int st_trans_type) // only used for move set now
{
	if(light_idx < LIGHT_CNT){
		memset(&light_res_sw[light_idx].trans[st_trans_type], 0, sizeof(light_res_sw[light_idx].trans[st_trans_type]));
	}
}
#endif


// LED function
/**
 * @brief       This function refresh all the light status through the global variables 
 * @return      none
 * @note        
 */
void light_dim_refresh_all()
{
    foreach(i, LIGHT_CNT){
        light_dim_refresh(i);
    }
}


typedef void (*fp_proc_led)(void);
fp_proc_led 				p_vendor_proc_led 				= 0;


static u32 led_event_pending;
static int led_count = 0;

/**
 * @brief       This function configure the frequency and times of LED flashing
 * @param[in]   e	- LED flashing event type
 * @return      none
 * @note        
 */
void cfg_led_event (u32 e)
{
	led_event_pending = e;
}

/**
 * @brief       This function stop the frequency and time of LED flashing
 * @return      none
 * @note        
 */
void cfg_led_event_stop ()
{
	led_event_pending = led_count = 0;
}

/**
 * @brief       This function check led is busy
 * @return      0:not busy; 1:busy
 * @note        
 */
int is_led_busy()
{
    return (!(!led_count && !led_event_pending));
}

/**
 * @brief       This function config gpio of led
 * @param[in]   gpio- gpio
 * @param[in]   on	- on or off
 * @return      none
 * @note        
 */
void led_onoff_gpio(u32 gpio, u8 on){
#if (FEATURE_LOWPOWER_EN || PM_DEEPSLEEP_RETENTION_ENABLE)
    gpio_set_func (gpio, AS_GPIO);
    gpio_set_output_en (gpio, 0);
    gpio_write(gpio, 0);
    gpio_setup_up_down_resistor(gpio, on ? PM_PIN_PULLUP_10K : PM_PIN_PULLDOWN_100K);
#else
    gpio_set_func (gpio, AS_GPIO);
    gpio_set_output_en (gpio, 1);
    gpio_write(gpio, on);
#endif
}

#if (__PROJECT_MESH_SWITCH__ || PM_DEEPSLEEP_RETENTION_ENABLE || ((SMART_PROVISION_ENABLE || FAST_PROVISION_ENABLE) && !MD_SERVER_EN))
/**
 * @brief       This function is led indicator event polling function
 * @return      none
 * @note        
 */
void proc_led()
{
	if(p_vendor_proc_led){
		p_vendor_proc_led();
		return;
	}
	static	u32 led_ton;
	static	u32 led_toff;
	static	int led_sel;						//
	static	u32 led_tick;
	static	int led_no;
	static	int led_is_on;

	if(!led_count && !led_event_pending) {
		return;  //led flash finished
	}

	if (led_event_pending)
	{
		// new event
		led_ton = (led_event_pending & 0xff) * 64000 * CLOCK_SYS_CLOCK_1US;
		led_toff = ((led_event_pending>>8) & 0xff) * 64000 * CLOCK_SYS_CLOCK_1US;
		led_count = (led_event_pending>>16) & 0xff;
		led_sel = led_event_pending>>24;

		led_event_pending = 0;
		led_tick = clock_time () + 30000000 * CLOCK_SYS_CLOCK_1US;
		led_no = 0;
		led_is_on = 0;
	}

	if( 1 ){
		if( (u32)(clock_time() - led_tick) >= (led_is_on ? led_ton : led_toff) ){
			led_tick = clock_time ();
			
			led_is_on = !led_is_on;
			if (led_is_on)
			{
				led_no++;
				if (led_no - 1 == led_count)
				{
					led_count = led_no = 0;
					#if (!__PROJECT_MESH_SWITCH__)
					light_dim_refresh_all(); // should not report online status again
					#endif
					return ;
				}
			}
			
			int led_off = (!led_is_on || !led_ton) && led_toff;
			int led_on = led_is_on && led_ton;
			
			if( led_off || led_on  ){
				if (led_sel & BIT(0))
				{
					led_onoff_gpio(GPIO_LED, led_on);
				}
            }
        }
	}

}
#else

/**
 * @brief       This function process led
 * @param[in]   void- none
 * @return      none
 * @note        
 */
void proc_led(void)
{
	if(p_vendor_proc_led){
		p_vendor_proc_led();
		return;
	}
	static	u32 led_ton;
	static	u32 led_toff;
	static	int led_sel;						//
	static	u32 led_tick;
	static	int led_no;
	static	int led_is_on;

	if(!led_count && !led_event_pending) {
		return;  //led flash finished
	}

	if (led_event_pending)
	{
		// new event
		led_ton = (led_event_pending & 0xff) * 64000 * CLOCK_SYS_CLOCK_1US;
		led_toff = ((led_event_pending>>8) & 0xff) * 64000 * CLOCK_SYS_CLOCK_1US;
		led_count = (led_event_pending>>16) & 0xff;
		led_sel = led_event_pending>>24;

		led_event_pending = 0;
		led_tick = clock_time () + 30000000 * CLOCK_SYS_CLOCK_1US;
		led_no = 0;
		led_is_on = 0;
	}

	if( 1 ){
		if( (u32)(clock_time() - led_tick) >= (led_is_on ? led_ton : led_toff) ){
			led_tick = clock_time ();
			int led_off = (led_is_on || !led_ton) && led_toff;
			int led_on = !led_is_on && led_ton;

			led_is_on = !led_is_on;
			if (led_is_on)
			{
				led_no++;
				//led_dbg++;
				if (led_no - 1 == led_count)
				{
					led_count = led_no = 0;
					light_dim_refresh_all(); // should not report online status again
					return ;
				}
			}
			
			if( led_off || led_on  ){
                if (led_sel & BIT(0))
                {
                    #ifdef CB_USER_PROC_LED_ONOFF_DRIVER  // user can define in "user_app_config.h"
                    CB_USER_PROC_LED_ONOFF_DRIVER(led_on);
                    #else
                    light_dim_set_hw(GPIO_LED_INDEX, 0, LED_INDICATE_VAL * led_on);
                    #endif
                }
                #if 0
                if (led_sel & BIT(1))
                {
                    light_dim_set_hw(1, LED_INDICATE_VAL * led_on);
                }
                if (led_sel & BIT(2))
                {
                    light_dim_set_hw(2, LED_INDICATE_VAL * led_on);
                }
                #endif
            }
        }
	}

}
#endif 

/**
 * @brief       This function is callback function of light event.
 * @param[in]   status	- rx Flashing event
 * @return      none
 * @note        
 */
_USER_CAN_REDEFINE_ void rf_link_light_event_callback (u8 status)
{
#ifdef WIN32
	return ;
#endif

#if FEATURE_LOWPOWER_EN
    if(lpn_provision_ok){
        #if 1 // for test
        if(LGT_CMD_FRIEND_SHIP_OK == status){
            cfg_led_event(LED_EVENT_FLASH_4HZ_3T);
        }else{
            cfg_led_event_stop(); // LED_EVENT_FLASH_4HZ_2T
        }
		
        while(is_led_busy()){
            proc_led();
            wd_clear();
        }
        #else
        cfg_led_event(0); // don't show LED
        #endif
        
        return ;
    }
#endif

	if(status == LGT_CMD_SET_MESH_INFO){
	    #ifdef CFG_LED_EVENT_SET_MESH_INFO
	    CFG_LED_EVENT_SET_MESH_INFO;
	    #else
        cfg_led_event(LED_EVENT_FLASH_1HZ_4S);
        #endif
        #if LLSYNC_ENABLE
		llsync_stop_silence_adv_timeout_check();
		#endif
    }else if(status == LGT_CMD_SET_SUBSCRIPTION){
	    #ifdef CFG_LED_EVENT_SET_SUBSCRIPTION
	    CFG_LED_EVENT_SET_SUBSCRIPTION;
	    #else
        cfg_led_event(LED_EVENT_FLASH_1HZ_3S);
        #endif
#if DEBUG_BLE_EVENT_ENABLE
    }else if(status == LGT_CMD_BLE_ADV){
		cfg_led_event(LED_EVENT_FLASH_1HZ_2S);
	}else if(status == LGT_CMD_BLE_CONN){
		cfg_led_event(LED_EVENT_FLASH_4HZ_3T);
#endif
#if __PROJECT_MESH_SWITCH__
	}else if(status == LGT_CMD_SWITCH_POWERON){
		cfg_led_event(LED_EVENT_FLASH_1HZ_1S);
	}else if(status == LGT_CMD_SWITCH_PROVISION){
		cfg_led_event(LED_EVENT_FLASH_4HZ_3T);
	}else if(status == LGT_CMD_SWITCH_CMD){
		cfg_led_event(LED_EVENT_FLASH_4HZ_1T);
#endif
#if DONGLE_PROVISION_EN
	}else if(status == PROV_START_LED_CMD){
		cfg_led_event(LED_EVENT_FLASH_4HZ_3T);
	}else if(status == PROV_END_LED_CMD){
		cfg_led_event(LED_EVENT_FLASH_STOP);
#endif
#if (DUAL_MODE_ADAPT_EN || DUAL_MODE_WITH_TLK_MESH_EN)
	}else if(status == LGT_CMD_DUAL_MODE_MESH){
		cfg_led_event(DUAL_MODE_WITH_TLK_MESH_EN ? LED_EVENT_FLASH_2HZ_2T : LED_EVENT_FLASH_2HZ_1T);
#endif
	}
}

#ifndef WIN32
/**
 * @brief       This function run LED indicator event with sleep_us.
 * @param[in]   count			- flashing num
 * @param[in]   half_cycle_us	- flashing step
 * @return      none
 * @note        
 */
void light_ev_with_sleep(u32 count, u32 half_cycle_us)
{
	gpio_set_func(GPIO_LED, AS_GPIO);
	gpio_set_output_en(GPIO_LED, 1);
	gpio_set_input_en(GPIO_LED, 0);
	
	for(u32 i=0; i< count;i++){
#if(MODULE_WATCHDOG_ENABLE)
        wd_clear();
#endif
		gpio_write(GPIO_LED, 0);
		sleep_us(half_cycle_us);
#if(MODULE_WATCHDOG_ENABLE)
        wd_clear();
#endif
		gpio_write(GPIO_LED, 1);
		sleep_us(half_cycle_us);
	}

	gpio_write(GPIO_LED, 0);
}

/**
 * @brief       This function flash LED for ota event.
 * @param[in]   result	- ota result
 * @return      none
 * @note        
 */
_USER_CAN_REDEFINE_ void show_ota_result(int result)
{
	if(result == OTA_REBOOT_NO_LED){
		// nothing
	}else if(result == OTA_SUCCESS){
		light_ev_with_sleep(3, 1000*1000);	//0.5Hz shine for  6 second
	}else{
		light_ev_with_sleep(30, 100*1000);	//5Hz shine for  6 second
		//write_reg8(0x8000,result); ;while(1);  //debug which err lead to OTA fail
	}
}

/**
 * @brief       This function flash LED for factory reset event
 * @return      none
 * @note        
 */
_USER_CAN_REDEFINE_ void show_factory_reset()
{
	light_ev_with_sleep(8, 500*1000);	//1Hz shine for  8 second
}
#endif

/**
  * @}
  */
    
/**
  * @}
  */


