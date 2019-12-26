/********************************************************************************************************
 * @file     hk_LightBulbService.c
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2016, Telink Semiconductor (Shanghai) Co., Ltd.
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
#define _HK_LIGHTBULB_SERVICE_C_
 
 
/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"
#include "../common/led_cfg.h"
#include "common.h"
#include "hk_LightBulbService.h"
#include "ble_UUID.h"
#include "hk_TopUUID.h"
#include "vendor/common/cmd_interface.h"

extern u32 session_timeout_tick;
extern u8 EventNotifyCNT;
extern store_char_change_value_t* char_save_restore;

/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#define HUE_R   0
#define HUE_G   1
#define HUE_B   2

#if 1
const unsigned short GAMMA_2_2_TABLE[256]={     // using gamma r=2.2
        0x0   ,0x0   ,0x1   ,0x3   ,0x7   ,0xB   ,0x11  ,0x18  ,0x20  ,0x29  ,0x34  ,0x41  ,0x4E  ,0x5D  ,0x6E  ,0x80  ,
        0x94  ,0xA9  ,0xC0  ,0xD8  ,0xF2  ,0x10D ,0x12A ,0x149 ,0x169 ,0x18B ,0x1AF ,0x1D4 ,0x1FB ,0x224 ,0x24F ,0x27B ,
        0x2A9 ,0x2D9 ,0x30A ,0x33D ,0x372 ,0x3A9 ,0x3E2 ,0x41C ,0x459 ,0x497 ,0x4D7 ,0x519 ,0x55D ,0x5A2 ,0x5EA ,0x633 ,
        0x67E ,0x6CB ,0x71A ,0x76B ,0x7BE ,0x813 ,0x86A ,0x8C3 ,0x91D ,0x97A ,0x9D9 ,0xA39 ,0xA9C ,0xB01 ,0xB67 ,0xBD0 ,
        0xC3A ,0xCA7 ,0xD16 ,0xD86 ,0xDF9 ,0xE6E ,0xEE5 ,0xF5E ,0xFD9 ,0x1056,0x10D5,0x1156,0x11D9,0x125E,0x12E6,0x136F,
        0x13FB,0x1489,0x1519,0x15AA,0x163F,0x16D5,0x176D,0x1808,0x18A4,0x1943,0x19E4,0x1A87,0x1B2C,0x1BD4,0x1C7E,0x1D29,
        0x1DD7,0x1E87,0x1F3A,0x1FEE,0x20A5,0x215E,0x2219,0x22D7,0x2396,0x2458,0x251C,0x25E3,0x26AB,0x2776,0x2843,0x2912,
        0x29E4,0x2AB7,0x2B8E,0x2C66,0x2D40,0x2E1D,0x2EFC,0x2FDE,0x30C1,0x31A7,0x3290,0x337A,0x3467,0x3556,0x3648,0x373C,
        0x3832,0x392A,0x3A25,0x3B22,0x3C21,0x3D23,0x3E27,0x3F2E,0x4036,0x4141,0x424F,0x435F,0x4471,0x4585,0x469C,0x47B5,
        0x48D1,0x49EF,0x4B0F,0x4C32,0x4D57,0x4E7F,0x4FA9,0x50D5,0x5204,0x5335,0x5468,0x559E,0x56D7,0x5811,0x594E,0x5A8E,
        0x5BD0,0x5D14,0x5E5B,0x5FA4,0x60F0,0x623E,0x638F,0x64E2,0x6637,0x678F,0x68E9,0x6A46,0x6BA5,0x6D07,0x6E6B,0x6FD2,
        0x713B,0x72A7,0x7415,0x7585,0x76F8,0x786E,0x79E6,0x7B60,0x7CDD,0x7E5D,0x7FDF,0x8163,0x82EA,0x8473,0x85FF,0x878E,
        0x891F,0x8AB2,0x8C48,0x8DE1,0x8F7C,0x9119,0x92B9,0x945C,0x9601,0x97A9,0x9953,0x9B00,0x9CAF,0x9E61,0xA015,0xA1CC,
        0xA386,0xA542,0xA701,0xA8C2,0xAA85,0xAC4C,0xAE15,0xAFE0,0xB1AE,0xB37F,0xB552,0xB728,0xB900,0xBADB,0xBCB8,0xBE98,
        0xC07B,0xC260,0xC448,0xC632,0xC820,0xCA0F,0xCC01,0xCDF6,0xCFEE,0xD1E8,0xD3E5,0xD5E4,0xD7E6,0xD9EA,0xDBF1,0xDDFB,
        0xE008,0xE217,0xE428,0xE63D,0xE854,0xEA6D,0xEC89,0xEEA8,0xF0CA,0xF2EE,0xF515,0xF73E,0xF96A,0xFB99,0xFDCA,0xFFFF,
        };
#else
const unsigned short GAMMA_1_8_TABLE[256]={     // using gamma r=1.8
        0x0   ,0x3   ,0xA   ,0x16  ,0x25  ,0x37  ,0x4C  ,0x65  ,0x80  ,0x9F  ,0xC0  ,0xE4  ,0x10B ,0x134 ,0x160 ,0x18F ,
        0x1C0 ,0x1F4 ,0x22A ,0x263 ,0x29E ,0x2DC ,0x31C ,0x35E ,0x3A3 ,0x3EA ,0x433 ,0x47F ,0x4CD ,0x51D ,0x56F ,0x5C4 ,
        0x61B ,0x674 ,0x6CF ,0x72C ,0x78C ,0x7ED ,0x851 ,0x8B7 ,0x91F ,0x989 ,0x9F6 ,0xA64 ,0xAD4 ,0xB47 ,0xBBB ,0xC32 ,
        0xCAA ,0xD25 ,0xDA2 ,0xE20 ,0xEA1 ,0xF24 ,0xFA8 ,0x102F,0x10B7,0x1142,0x11CF,0x125D,0x12ED,0x1380,0x1414,0x14AA,
        0x1542,0x15DC,0x1678,0x1716,0x17B6,0x1858,0x18FB,0x19A0,0x1A48,0x1AF1,0x1B9C,0x1C49,0x1CF7,0x1DA8,0x1E5A,0x1F0F,
        0x1FC5,0x207D,0x2136,0x21F2,0x22AF,0x236E,0x242F,0x24F2,0x25B7,0x267D,0x2745,0x280F,0x28DB,0x29A9,0x2A78,0x2B49,
        0x2C1C,0x2CF1,0x2DC7,0x2E9F,0x2F79,0x3055,0x3132,0x3211,0x32F2,0x33D5,0x34B9,0x359F,0x3687,0x3770,0x385C,0x3949,
        0x3A37,0x3B28,0x3C1A,0x3D0D,0x3E03,0x3EFA,0x3FF3,0x40EE,0x41EA,0x42E8,0x43E7,0x44E9,0x45EC,0x46F0,0x47F7,0x48FF,
        0x4A09,0x4B14,0x4C21,0x4D30,0x4E40,0x4F52,0x5066,0x517B,0x5292,0x53AA,0x54C5,0x55E1,0x56FE,0x581D,0x593E,0x5A60,
        0x5B84,0x5CAA,0x5DD1,0x5EFA,0x6025,0x6151,0x627F,0x63AE,0x64DF,0x6612,0x6746,0x687C,0x69B3,0x6AEC,0x6C27,0x6D63,
        0x6EA1,0x6FE0,0x7121,0x7264,0x73A8,0x74EE,0x7635,0x777E,0x78C9,0x7A15,0x7B63,0x7CB2,0x7E03,0x7F55,0x80A9,0x81FE,
        0x8356,0x84AE,0x8608,0x8764,0x88C1,0x8A20,0x8B81,0x8CE3,0x8E46,0x8FAB,0x9112,0x927A,0x93E4,0x954F,0x96BC,0x982A,
        0x999A,0x9B0C,0x9C7F,0x9DF3,0x9F69,0xA0E1,0xA25A,0xA3D4,0xA550,0xA6CE,0xA84D,0xA9CE,0xAB50,0xACD4,0xAE59,0xAFE0,
        0xB168,0xB2F2,0xB47D,0xB60A,0xB798,0xB928,0xBAB9,0xBC4C,0xBDE1,0xBF76,0xC10E,0xC2A7,0xC441,0xC5DD,0xC77A,0xC919,
        0xCAB9,0xCC5B,0xCDFE,0xCFA3,0xD149,0xD2F1,0xD49A,0xD645,0xD7F1,0xD99F,0xDB4E,0xDCFE,0xDEB0,0xE064,0xE219,0xE3CF,
        0xE587,0xE741,0xE8FC,0xEAB8,0xEC76,0xEE35,0xEFF6,0xF1B8,0xF37C,0xF541,0xF708,0xF8D0,0xFA99,0xFC64,0xFE31,0xFFFF,
        };
#endif


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions
   ------------------------------------ */
RGB_set RGBset;
HSV_set HSVset;

u8 light_onoff = 1;

int light_brightness = 100;
unsigned int hue_value = 0x420C0000;	//35.0
unsigned int saturation_value = 0x429E0000;	//79.0
unsigned short cal_bri=0;
unsigned short cal_sat=0;
unsigned short cal_hue=0;
unsigned int color_temperature = 50;
unsigned char VAR_LIGHTBULB_UNKOWN=0;

u32 light_bn_w = 0;
u32 light_bn_r = 0;

u32 Saturation_read_dbg=0;
u32 Saturation_write_dbg=0;
u32 Temperature_read_dbg=0;

u32 hue_read_dbg=0;
u32 hue_write_dbg=0;

u32 lightOn_write;
u32 lightOn_read;

const u8 my_lbServiceName[23]   = {"Telink Semi Smart Light"};

u16 ccc_light_onoff = 0;
u16 ccc_light_bri = 0;
u16 ccc_light_hue = 0;
u16 ccc_light_satu = 0;
u16 ccc_light_col_temp = 0;

#if LIGHTBULB_ONOFF_BRC_ENABLE
    unsigned char bc_para_light_on = 1;
#endif

#if LIGHTBULB_BRIGHTNESS_BRC_ENABLE
    unsigned char bc_para_light_bri = 1;
#endif

#if LIGHTBULB_HUE_BRC_ENABLE
    unsigned char bc_para_light_hue = 1;
#endif

#if LIGHTBULB_SATURATION_BRC_ENABLE
    unsigned char bc_para_light_satu = 1;
#endif

#if LIGHTBULB_COLOR_TEMPERATURE_BRC_ENABLE
    unsigned char bc_para_light_color = 1;
#endif

const u8 BRIGHTNESS_VALID_RANGE[8]          = { 0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00};    // 0~100 (int32)
const u8 HUE_VALID_RANGE[8]                 = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB4, 0x43};    // 0~360 (float)
const u8 SATURATION_VALID_RANGE[8]          = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC8, 0x42};    // 0~100 (float)
const u8 COLOR_TEMPERATURE_VALID_RANGE[8]   = { 0x32, 0x00, 0x00, 0x00, 0x90, 0x01, 0x00, 0x00};    // 50~400 (uint32)

const u8 BRIGHTNESS_VALID_STEP[4]           = { 0x01, 0x00, 0x00, 0x00};
const u8 HUE_VALID_STEP[4]                  = { 0x00, 0x00, 0x80, 0x3F};
const u8 SATURATION_VALID_STEP[4]           = { 0x00, 0x00, 0x80, 0x3F};
const u8 COLOR_TEMPERATURE_VALID_STEP[4]    = { 0x01, 0x00, 0x00, 0x00};

const hap_characteristic_desc_t hap_desc_char_LIGHT_ON_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_BOOL_UNITLESS, 0, 0, 0, 0, 0, 0, 0, BC_PARA_LIGHTBULB_ONOFF};
const hap_characteristic_desc_t hap_desc_char_BRIGHTNESS_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_INT32_PERCENTAGE, 0, 8, 4, 0, 0, BRIGHTNESS_VALID_RANGE, BRIGHTNESS_VALID_STEP, BC_PARA_LIGHTBULB_BRIGHTNESS};
const hap_characteristic_desc_t hap_desc_char_HUE_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_FLOAT32_ARCDEGREE, 0, 8, 4, 0, 0, HUE_VALID_RANGE, HUE_VALID_STEP, BC_PARA_LIGHTBULB_HUE};
const hap_characteristic_desc_t hap_desc_char_SATURATION_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_FLOAT32_PERCENTAGE, 0, 8, 4, 0, 0, SATURATION_VALID_RANGE, SATURATION_VALID_STEP, BC_PARA_LIGHTBULB_SATURATION};
const hap_characteristic_desc_t hap_desc_char_COLOR_TEMPERATURE_R_S = {hap_PReadWrite_NOTIFY, 0, CFG_UINT32, 0, 8, 4, 0, 0, COLOR_TEMPERATURE_VALID_RANGE, COLOR_TEMPERATURE_VALID_STEP, BC_PARA_LIGHTBULB_COLOR_TEMP};

const hap_characteristic_desc_t hap_desc_char_lightbulb_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if LIGHTBULB_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * hLightBulb_linked_svcs[] = {};
#endif

#if(LIGHTBULB_IS_HIDDEN_SERVICE == 1)
//--u16 lightbulb_A5 = (0x02 + LIGHTBULB_AS_PRIMARY_SERVICE);
const hap_service_desc_t hap_desc_service_lightbulb_A5_R_S = {LIGHTBULB_INST_NO+1, (0x02 + LIGHTBULB_AS_PRIMARY_SERVICE), LIGHTBULB_LINKED_SVCS_LENGTH, LIGHTBULB_LINKED_SVCS};
#else
//--u16 lightbulb_A5 = (0x00 + LIGHTBULB_AS_PRIMARY_SERVICE);
const hap_service_desc_t hap_desc_service_lightbulb_A5_R_S = {LIGHTBULB_INST_NO+1, (0x00 + LIGHTBULB_AS_PRIMARY_SERVICE), LIGHTBULB_LINKED_SVCS_LENGTH, LIGHTBULB_LINKED_SVCS};
#endif


/////////////////////////////////////////////////////////////////////
// IEEE-754 32-bit floating point
// Sign Bits        : 1
// Exponent Bits    : 8
// Mantissa Bits    : 23
// Example1 : 0 10000111 0110100000.....0000 => 0x43B40000 => 360
// Example2 : 0 10000101 1001000000.....0000 => 0x42C80000 => 100
/////////////////////////////////////////////////////////////////////

/* ------------------------------------
    Function Prototypes
   ------------------------------------ */

void Set_GammaMap_light(u8 r,u8 g ,u8 b)
{
    u16 led_val_tbl[4];

    ////////////////////////////////////////////
    // It can use depedent GAMMA tables for R/G/B/Y/W die
    // just create NEW GAMMA tables and modify them here.
    led_val_tbl[0] = GAMMA_2_2_TABLE[r];
    led_val_tbl[1] = GAMMA_2_2_TABLE[g];
    led_val_tbl[2] = GAMMA_2_2_TABLE[b];
    led_val_tbl[3] = GAMMA_2_2_TABLE[(light_brightness*2)];

    if(light_onoff_status){
    	pwm_set_cycle_and_duty(PWMID_R,PMW_MAX_TICK,led_val_tbl[0]);
    	pwm_set_cycle_and_duty(PWMID_G,PMW_MAX_TICK,led_val_tbl[1]);
    	pwm_set_cycle_and_duty(PWMID_B,PMW_MAX_TICK,led_val_tbl[2]);
    	pwm_set_cycle_and_duty(PWMID_W,PMW_MAX_TICK,led_val_tbl[3]);
 //   	pwm_set_cycle_and_duty(PWMID_Y,PMW_MAX_TICK,led_val_tbl[3]);
    }

    led_val[0] = led_val_tbl[0]/256;
    led_val[1] = led_val_tbl[1]/256;
    led_val[2] = led_val_tbl[2]/256;
    led_val[3] = led_val_tbl[3]/256;
}

void HsvToRgb(HSV_set hsv, RGB_set *rgb)
{
    u8 region;
    u8 remainder;
    u8 p, q, t;

    if (hsv.s == 0)
    {
        rgb->r = hsv.v;
        rgb->g = hsv.v;
        rgb->b = hsv.v;
        return;
    }

    if(hsv.h < 360)
        region = hsv.h / 60;
    else
        region = 5;
    remainder = (hsv.h - (region * 60)) * 6;

    p = (hsv.v * (255 - hsv.s)) >> 8;
    q = (hsv.v * (255 - ((hsv.s * remainder) >> 8))) >> 8;
    t = (hsv.v * (255 - ((hsv.s * (255 - remainder)) >> 8))) >> 8;

    if (region == 0) {
        rgb->r = hsv.v;
        rgb->g = t;
        rgb->b = p;
    } else if (region == 1) {
        rgb->r = q;
        rgb->g = hsv.v;
        rgb->b = p;
    } else if (region == 2) {
        rgb->r = p;
        rgb->g = hsv.v;
        rgb->b = t;
    } else if (region == 3) {
        rgb->r = p;
        rgb->g = q;
        rgb->b = hsv.v;
    } else if (region == 4) {
        rgb->r = t;
        rgb->g = p;
        rgb->b = hsv.v;
    } else {
        rgb->r = hsv.v;
        rgb->g = p;
        rgb->b = q;
    }
}

void Convert_HSV2RGB(u32 * dbg)
{
    unsigned int AL_uint_v5=0;
    int AL_int_v6=0;
    unsigned short AL_cal_temp1=0;
    unsigned short AL_cal_temp2=0;
    unsigned short AL_cal_temp3=0;
    unsigned short AL_cal_temp4=0;
    unsigned short AL_cal_temp5=0;
    unsigned short AL_cal_temp6=0;

    *dbg+=1;

    AL_uint_v5 = ((hue_value & 0x007FFFFF)|0x00800000) >> (23-(((hue_value & 0x7F800000)>>23)-0x7F));
    AL_int_v6 = AL_uint_v5;
    cal_hue = (u16) AL_int_v6;
    HSVset.h = cal_hue;

    AL_uint_v5 = ((saturation_value & 0x007FFFFF)|0x00800000) >> (23-(((saturation_value & 0x7F800000)>>23)-0x7F));
    AL_int_v6 = AL_uint_v5;
    cal_sat = (u16) AL_int_v6;
    AL_cal_temp1 = cal_sat << 8;
    AL_cal_temp2 = AL_cal_temp1 - cal_sat;
    AL_cal_temp3 = AL_cal_temp2 / 100;
    HSVset.s = (u8) AL_cal_temp3;

    cal_bri = (u16) light_brightness;
    AL_cal_temp4 = cal_bri << 8;
    AL_cal_temp5 = AL_cal_temp4 - cal_bri;
    AL_cal_temp6 = AL_cal_temp5 / 100;
    HSVset.v = (u8) AL_cal_temp6;

    HsvToRgb(HSVset, &RGBset);

    led_val[HUE_R] = RGBset.r;
    led_val[HUE_G] = RGBset.g;
    led_val[HUE_B] = RGBset.b;
//--    memset(led_val+3 , 0xff ,3);

    Set_GammaMap_light(RGBset.r, RGBset.g, RGBset.b);
}

void light_enable (int en)
{
    light_onoff = en;
    pwm_light_enable (en);

////////////////////////////////////
// It should be removed here due to those services are independent. by Andrew Lee
#if LOCKMECHANISM_SERVICE_ENABLE
#if DOORLOCK_LED_DEMO_ENABLE
    if(Lock_Target_State)
        pwm_set_cycle_and_duty(PWMID_W, PMW_MAX_TICK, PMW_MAX_TICK);
    else
        pwm_set_cycle_and_duty(PWMID_W, PMW_MAX_TICK, 0);
#endif
#endif
}

int lightOnReadCallback(void *pp)       //
{
    lightOn_read++;
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int lightOnWriteCallback(void *pp)      //
{
    blt_adv_set_global_state ();
    lightOn_write++;

    if (hap_char_boundary_check()) {    // 0~1
        assign_current_flash_char_value_addr();
    #if LIGHTBULB_SERVICE_ENABLE
        LIGHTBULB_STORE_CHAR_VALUE_T_LGTONOFFNO_RESTORE
    #endif
    } else {
        if(light_onoff_status != light_onoff){
            store_char_value_to_flash();
            light_onoff_status = light_onoff;
        }
    }

//    light_enable (light_onoff);
	access_cmd_onoff(ele_adr_primary, 0, light_onoff, 0, 0);
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int lightHueReadCallback(void *pp)
{
    hue_read_dbg++;
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int lightHueWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {    // 0~360
        assign_current_flash_char_value_addr();
    #if LIGHTBULB_SERVICE_ENABLE
        LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_HUE_ENABLE_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    Convert_HSV2RGB(&hue_write_dbg);

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ColorSaturationReadCallback(void *pp)
{
    Saturation_read_dbg++;
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ColorSaturationWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~100
        assign_current_flash_char_value_addr();
    #if LIGHTBULB_SERVICE_ENABLE
        LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_SATURATION_ENABLE_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    Convert_HSV2RGB(&Saturation_write_dbg);

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ColorTemperatureReadCallback(void *pp)
{
    Temperature_read_dbg++;
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ColorTemperatureWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 50~400
        assign_current_flash_char_value_addr();
    #if LIGHTBULB_SERVICE_ENABLE
        LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_COLOR_TEMPERATURE_ENABLE_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int lightBrightnessReadCallback(void *pp)
{
    light_bn_r++;
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int lightBrightnessWriteCallback(void *pp)
{
    blt_adv_set_global_state ();

    if (hap_char_boundary_check()) {  // 0~100
        assign_current_flash_char_value_addr();
    #if LIGHTBULB_SERVICE_ENABLE
        LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_BRIGHTNESS_ENABLE_RESTORE
    #endif
    } else {
        store_char_value_to_flash();
    }

    Convert_HSV2RGB(&light_bn_w);

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_light_onoffWriteCB(void *pp)
{
    static u16 ccc_light_onoff_pre = 0;

    if(ccc_light_onoff_pre != ccc_light_onoff)
    {
        if(ccc_light_onoff == 0)
        {
            ccc_light_onoff_pre = ccc_light_onoff;
            EventNotifyCNT--;
            indicate_handle_no_remove(LIGHTBULB_HANDLE_NUM_LIGHT_ONOFF);
        }
        else
        {
            ccc_light_onoff_pre = ccc_light_onoff;
            EventNotifyCNT++;
            indicate_handle_no_reg(LIGHTBULB_HANDLE_NUM_LIGHT_ONOFF);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_light_briWriteCB(void *pp)
{
    static u16 ccc_light_bri_pre = 0;

    if(ccc_light_bri_pre != ccc_light_bri)
    {
        if(ccc_light_bri == 0)
        {
            ccc_light_bri_pre = ccc_light_bri;
            EventNotifyCNT--;
            indicate_handle_no_remove(LIGHTBULB_HANDLE_NUM_LIGHT_BRIGHTNESS);
        }
        else
        {
            ccc_light_bri_pre = ccc_light_bri;
            EventNotifyCNT++;
            indicate_handle_no_reg(LIGHTBULB_HANDLE_NUM_LIGHT_BRIGHTNESS);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_light_hueWriteCB(void *pp)
{
    static u16 ccc_light_hue_pre = 0;

    if(ccc_light_hue_pre != ccc_light_hue)
    {
        if(ccc_light_hue == 0)
        {
            ccc_light_hue_pre = ccc_light_hue;
            EventNotifyCNT--;
            indicate_handle_no_remove(LIGHTBULB_HANDLE_NUM_HUE_VALUE);
        }
        else
        {
            ccc_light_hue_pre = ccc_light_hue;
            EventNotifyCNT++;
            indicate_handle_no_reg(LIGHTBULB_HANDLE_NUM_HUE_VALUE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_light_satuWriteCB(void *pp)
{
    static u16 ccc_light_satu_pre = 0;

    if(ccc_light_satu_pre != ccc_light_satu)
    {
        if(ccc_light_satu == 0)
        {
            ccc_light_satu_pre = ccc_light_satu;
            EventNotifyCNT--;
            indicate_handle_no_remove(LIGHTBULB_HANDLE_NUM_SATURATION_VALUE);
        }
        else
        {
            ccc_light_satu_pre = ccc_light_satu;
            EventNotifyCNT++;
            indicate_handle_no_reg(LIGHTBULB_HANDLE_NUM_SATURATION_VALUE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_light_col_tempWriteCB(void *pp)
{
    static u16 ccc_light_col_temp_pre = 0;

    if(ccc_light_col_temp_pre != ccc_light_col_temp)
    {
        if(ccc_light_col_temp == 0)
        {
            ccc_light_col_temp_pre = ccc_light_col_temp;
            EventNotifyCNT--;
            indicate_handle_no_remove(LIGHTBULB_HANDLE_NUM_COLOR_TEMPERATURE_VALUE);
        }
        else
        {
            ccc_light_col_temp_pre = ccc_light_col_temp;
            EventNotifyCNT++;
            indicate_handle_no_reg(LIGHTBULB_HANDLE_NUM_COLOR_TEMPERATURE_VALUE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
