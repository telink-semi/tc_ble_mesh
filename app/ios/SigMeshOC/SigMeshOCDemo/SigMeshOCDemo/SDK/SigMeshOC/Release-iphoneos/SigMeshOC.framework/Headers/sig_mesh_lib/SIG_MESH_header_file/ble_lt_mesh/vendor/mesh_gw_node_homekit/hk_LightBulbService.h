/********************************************************************************************************
 * @file     hk_LightBulbService.h
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
#ifndef _HK_LIGHTBULB_SERVICE_H_
#define _HK_LIGHTBULB_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_LIGHTBULB_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif

#define CHAR_LIGHTBULB_SVC_SIGN_ENABLE      1
#if CHAR_LIGHTBULB_SVC_SIGN_ENABLE
    #define LIGHTBULB_AS_PRIMARY_SERVICE    	1
    #define LIGHTBULB_IS_HIDDEN_SERVICE     	0
    #define LIGHTBULB_LINKED_SERVICES_ENABLE	0
    EXTERN const hap_characteristic_desc_t hap_desc_char_lightbulb_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_lightbulb_A5_R_S;
    EXTERN unsigned char VAR_LIGHTBULB_UNKOWN;
#else
    #define LIGHTBULB_AS_PRIMARY_SERVICE    	0
    #define LIGHTBULB_IS_HIDDEN_SERVICE     	0
    #define LIGHTBULB_LINKED_SERVICES_ENABLE	0
#endif

#define CHAR_BRIGHTNESS_ENABLE          1
#define CHAR_HUE_ENABLE                 1
#define CHAR_SATURATION_ENABLE          1
#define CHAR_COLOR_TEMPERATURE_ENABLE   0

#define LIGHTBULB_ONOFF_IND_ENABLE              1
#define LIGHTBULB_BRIGHTNESS_IND_ENABLE         1
#define LIGHTBULB_HUE_IND_ENABLE                1
#define LIGHTBULB_SATURATION_IND_ENABLE         1
#define LIGHTBULB_COLOR_TEMPERATURE_IND_ENABLE  0

#define LIGHTBULB_ONOFF_BRC_ENABLE              1
#define LIGHTBULB_BRIGHTNESS_BRC_ENABLE         1
#define LIGHTBULB_HUE_BRC_ENABLE                1
#define LIGHTBULB_SATURATION_BRC_ENABLE         1
#define LIGHTBULB_COLOR_TEMPERATURE_BRC_ENABLE  0


#define LGTONOFFNO      3

#if CHAR_BRIGHTNESS_ENABLE
#define BRINO   3
#if LIGHTBULB_BRIGHTNESS_IND_ENABLE
    #define LIGHTBULB_BRIGHTNESS_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_light_bri), ccc_light_briWriteCB, 0},
    #define LIGHTBULB_BRIGHTNESS_IND_NO  1
#else
    #define LIGHTBULB_BRIGHTNESS_IND_DESC
    #define LIGHTBULB_BRIGHTNESS_IND_NO  0
#endif
#define CHAR_BRIGHTNESS_ATT_TABLE   \
    {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_BRIGHTNESS_R_S, 0},  \
    {0,ATT_PERMISSIONS_RDWR,16, sizeof (light_brightness), 0, (u8*)(&charBrightnessUUID), (u8*)&light_brightness, lightBrightnessWriteCallback, lightBrightnessReadCallback},    \
        {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + LIGHTBULB_INST_NO + 3), 0, 0},   \
        LIGHTBULB_BRIGHTNESS_IND_DESC
#else
#define CHAR_BRIGHTNESS_ATT_TABLE
#define BRINO   0
#define LIGHTBULB_BRIGHTNESS_IND_NO  0
#endif

#if CHAR_HUE_ENABLE
#define HUENO   3
#if LIGHTBULB_HUE_IND_ENABLE
    #define LIGHTBULB_HUE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_light_hue), ccc_light_hueWriteCB, 0},
    #define LIGHTBULB_HUE_IND_NO  1
#else
    #define LIGHTBULB_HUE_IND_DESC
    #define LIGHTBULB_HUE_IND_NO  0
#endif
#define CHAR_HUE_ATT_TABLE  \
    {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_HUE_R_S, 0}, \
    {0,ATT_PERMISSIONS_RDWR,16, sizeof (hue_value), 0, (u8*)(&charHueUUID), (u8*)&hue_value, lightHueWriteCallback, lightHueReadCallback},   \
        {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + LIGHTBULB_INST_NO + 4), 0, 0},   \
        LIGHTBULB_HUE_IND_DESC
#else
#define CHAR_HUE_ATT_TABLE
#define HUENO   0
#define LIGHTBULB_HUE_IND_NO  0
#endif

#if CHAR_SATURATION_ENABLE
#define SATNO   3
#if LIGHTBULB_SATURATION_IND_ENABLE
    #define LIGHTBULB_SATURATION_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_light_satu), ccc_light_satuWriteCB, 0},
    #define LIGHTBULB_SATURATION_IND_NO  1
#else
    #define LIGHTBULB_SATURATION_IND_DESC
    #define LIGHTBULB_SATURATION_IND_NO  0
#endif
#define CHAR_SATURATION_ATT_TABLE   \
    {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_SATURATION_R_S, 0},  \
    {0,ATT_PERMISSIONS_RDWR,16, sizeof (saturation_value), 0, (u8*)(&charSaturationUUID), (u8*)&saturation_value, ColorSaturationWriteCallback, ColorSaturationReadCallback},    \
        {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + LIGHTBULB_INST_NO + 5), 0, 0},   \
        LIGHTBULB_SATURATION_IND_DESC
#else
#define CHAR_SATURATION_ATT_TABLE
#define SATNO   0
#define LIGHTBULB_SATURATION_IND_NO  0
#endif

#if CHAR_COLOR_TEMPERATURE_ENABLE
#define COL_TEMP_NO   3
#if LIGHTBULB_COLOR_TEMPERATURE_IND_ENABLE
    #define LIGHTBULB_COLOR_TEMPERATURE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_light_col_temp), ccc_light_col_tempWriteCB, 0},
    #define LIGHTBULB_COLOR_TEMPERATURE_IND_NO  1
#else
    #define LIGHTBULB_COLOR_TEMPERATURE_IND_DESC
    #define LIGHTBULB_COLOR_TEMPERATURE_IND_NO  0
#endif
#define CHAR_COLOR_TEMPERATURE_ATT_TABLE   \
    {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_COLOR_TEMPERATURE_R_S, 0},  \
    {0,ATT_PERMISSIONS_RDWR,16, sizeof (color_temperature), 0, (u8*)(&charColorTemperatureUUID), (u8*)&color_temperature, ColorTemperatureWriteCallback, ColorTemperatureReadCallback},    \
        {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + LIGHTBULB_INST_NO + 6), 0, 0},   \
        LIGHTBULB_COLOR_TEMPERATURE_IND_DESC
#else
#define CHAR_COLOR_TEMPERATURE_ATT_TABLE
#define COL_TEMP_NO   0
#define LIGHTBULB_COLOR_TEMPERATURE_IND_NO  0
#endif

#if LIGHTBULB_ONOFF_IND_ENABLE
    #define LIGHTBULB_ONOFF_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_light_onoff), ccc_light_onoffWriteCB, 0},
    #define LIGHTBULB_ONOFF_IND_NO  1
#else
    #define LIGHTBULB_ONOFF_IND_DESC
    #define LIGHTBULB_ONOFF_IND_NO  0
#endif


#if CHAR_LIGHTBULB_SVC_SIGN_ENABLE
#define CHAR_LIGHTBULB_SVC_A5_SIGN_TABLE   \
    {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_lightbulb_A5_R_S, &hap_desc_service_lightbulb_A5_R_S},  \
    {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_LIGHTBULB_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_LIGHTBULB_UNKOWN, 0, 0},    \
        {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + LIGHTBULB_INST_NO + 7), 0, 0},
#define LIGHTBULB_SVC_SIGN_NO   3
#else
#define CHAR_LIGHTBULB_SVC_A5_SIGN_TABLE
#define LIGHTBULB_SVC_SIGN_NO   0
#endif


#define LIGHTBULB_NO_DELTA  LIGHTBULB_ONOFF_IND_NO          \
                            +LIGHTBULB_BRIGHTNESS_IND_NO    \
                            +LIGHTBULB_HUE_IND_NO           \
                            +LIGHTBULB_SATURATION_IND_NO    \
                            +LIGHTBULB_COLOR_TEMPERATURE_IND_NO

#define LIGHTBULB_NO        9 +BRINO +HUENO +SATNO +COL_TEMP_NO +LIGHTBULB_SVC_SIGN_NO
#define LIGHTBULB_HANDLE_NUM (LIGHTBULB_NO+LIGHTBULB_NO_DELTA)


#define HOMEKIT_LIGHTBULB_SERVICE_ATT_TABLE     \
    {LIGHTBULB_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(lightBulbServiceUUID),sizeof(lightBulbServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(lightBulbServiceUUID), 0, 0}, \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},   /* homekit accessory protocol for LE, chapter 12.2*/    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(insts + LIGHTBULB_INST_NO), 0, 0},         \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_NAME, 0},            \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(my_lbServiceName), 0, (u8*)(nameUUID), (u8*)(my_lbServiceName), 0, 0},        \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + LIGHTBULB_INST_NO + 1), 0, 0},        \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_LIGHT_ON_R_S, 0},      \
        {0,ATT_PERMISSIONS_RDWR,16,1,0, (u8*)(charOnUUID), (u8*)&light_onoff, lightOnWriteCallback, lightOnReadCallback},\
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + LIGHTBULB_INST_NO + 2), 0, 0},        \
            LIGHTBULB_ONOFF_IND_DESC \
        CHAR_BRIGHTNESS_ATT_TABLE   \
        CHAR_HUE_ATT_TABLE          \
        CHAR_SATURATION_ATT_TABLE   \
        CHAR_COLOR_TEMPERATURE_ATT_TABLE  \
        CHAR_LIGHTBULB_SVC_A5_SIGN_TABLE


#define LIGHTBULB_HANDLE_COMMON_NUM     8

#define LIGHTBULB_HANDLE_NUM_LIGHT_ONOFF                LIGHTBULB_HANDLE_START_NUM +LIGHTBULB_HANDLE_COMMON_NUM
#define LIGHTBULB_HANDLE_NUM_LIGHT_BRIGHTNESS           LIGHTBULB_HANDLE_NUM_LIGHT_ONOFF +LGTONOFFNO +LIGHTBULB_ONOFF_IND_NO
#define LIGHTBULB_HANDLE_NUM_HUE_VALUE                  LIGHTBULB_HANDLE_NUM_LIGHT_BRIGHTNESS +BRINO +LIGHTBULB_BRIGHTNESS_IND_NO
#define LIGHTBULB_HANDLE_NUM_SATURATION_VALUE           LIGHTBULB_HANDLE_NUM_HUE_VALUE +HUENO +LIGHTBULB_HUE_IND_NO
#define LIGHTBULB_HANDLE_NUM_COLOR_TEMPERATURE_VALUE    LIGHTBULB_HANDLE_NUM_SATURATION_VALUE +SATNO +LIGHTBULB_SATURATION_IND_NO
#define LIGHTBULB_HANDLE_NUM_THE_END


#define LIGHTBULB_STORE_CHAR_VALUE_T_LGTONOFFNO                         u32 OnOff:1;    /* 0~1 */
#define LIGHTBULB_STORE_CHAR_VALUE_T_LGTONOFFNO_STORE                       STORE_CHAR(OnOff,light_onoff)
#define LIGHTBULB_STORE_CHAR_VALUE_T_LGTONOFFNO_RESTORE                     RESTORE_CHAR(OnOff,light_onoff)

#if CHAR_BRIGHTNESS_ENABLE
#define LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_BRIGHTNESS_ENABLE             u32 Bright:7;   /* 0~100 */
#define LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_BRIGHTNESS_ENABLE_STORE           STORE_CHAR(Bright,light_brightness)
#define LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_BRIGHTNESS_ENABLE_RESTORE         RESTORE_CHAR(Bright,light_brightness)
#else
#define LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_BRIGHTNESS_ENABLE
#define LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_BRIGHTNESS_ENABLE_STORE
#define LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_BRIGHTNESS_ENABLE_RESTORE
#endif

#if CHAR_HUE_ENABLE
#define LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_HUE_ENABLE                    u32 Hue;      /* float */
#define LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_HUE_ENABLE_STORE                  STORE_CHAR(Hue,hue_value)
#define LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_HUE_ENABLE_RESTORE                RESTORE_CHAR(Hue,hue_value)
#else
#define LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_HUE_ENABLE
#define LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_HUE_ENABLE_STORE
#define LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_HUE_ENABLE_RESTORE
#endif

#if CHAR_SATURATION_ENABLE
#define LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_SATURATION_ENABLE             u32 Satu;     /* float */
#define LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_SATURATION_ENABLE_STORE           STORE_CHAR(Satu,saturation_value)
#define LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_SATURATION_ENABLE_RESTORE         RESTORE_CHAR(Satu,saturation_value)
#else
#define LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_SATURATION_ENABLE
#define LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_SATURATION_ENABLE_STORE
#define LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_SATURATION_ENABLE_RESTORE
#endif

#if CHAR_COLOR_TEMPERATURE_ENABLE
#define LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_COLOR_TEMPERATURE_ENABLE      u32 ColTemp:9;     /* 50~400 */
#define LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_COLOR_TEMPERATURE_ENABLE_STORE    STORE_CHAR(ColTemp,color_temperature)
#define LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_COLOR_TEMPERATURE_ENABLE_RESTORE  RESTORE_CHAR(ColTemp,color_temperature)
#else
#define LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_COLOR_TEMPERATURE_ENABLE
#define LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_COLOR_TEMPERATURE_ENABLE_STORE
#define LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_COLOR_TEMPERATURE_ENABLE_RESTORE
#endif

#define LIGHTBULB_STORE_CHAR_VALUE_T            \
                    LIGHTBULB_STORE_CHAR_VALUE_T_LGTONOFFNO                 \
                    LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_BRIGHTNESS_ENABLE     \
                    LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_HUE_ENABLE            \
                    LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_SATURATION_ENABLE     \
                    LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_COLOR_TEMPERATURE_ENABLE

#define LIGHTBULB_STORE_CHAR_TO_FLASH           \
                    LIGHTBULB_STORE_CHAR_VALUE_T_LGTONOFFNO_STORE                 \
                    LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_BRIGHTNESS_ENABLE_STORE     \
                    LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_HUE_ENABLE_STORE            \
                    LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_SATURATION_ENABLE_STORE     \
                    LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_COLOR_TEMPERATURE_ENABLE_STORE

#define LIGHTBULB_RESTORE_CHAR_FROM_FLASH       \
                    LIGHTBULB_STORE_CHAR_VALUE_T_LGTONOFFNO_RESTORE                 \
                    LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_BRIGHTNESS_ENABLE_RESTORE     \
                    LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_HUE_ENABLE_RESTORE            \
                    LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_SATURATION_ENABLE_RESTORE     \
                    LIGHTBULB_STORE_CHAR_VALUE_T_CHAR_COLOR_TEMPERATURE_ENABLE_RESTORE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */
typedef struct RGB_set {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    int DUMMY_FOR_COMPILE_BUG_DO_NOT_USE_IT;
} RGB_set;

typedef struct HSV_set {
    unsigned int h;
    unsigned char s;
    unsigned char v;
} HSV_set;

/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN u8 light_onoff;
EXTERN int light_brightness;
EXTERN unsigned int hue_value;
EXTERN unsigned int saturation_value;
EXTERN unsigned int color_temperature;

EXTERN const hap_characteristic_desc_t hap_desc_char_LIGHT_ON_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_BRIGHTNESS_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_HUE_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_SATURATION_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_COLOR_TEMPERATURE_R_S;

EXTERN const u8 my_lbServiceName[23];

EXTERN u16 ccc_light_onoff;
EXTERN u16 ccc_light_bri;
EXTERN u16 ccc_light_hue;
EXTERN u16 ccc_light_satu;
EXTERN u16 ccc_light_col_temp;

#if LIGHTBULB_ONOFF_BRC_ENABLE
    #define BC_PARA_LIGHTBULB_ONOFF ((u8*)&bc_para_light_on)
    EXTERN unsigned char bc_para_light_on;
#else
    #define BC_PARA_LIGHTBULB_ONOFF 0
#endif

#if LIGHTBULB_BRIGHTNESS_BRC_ENABLE
    #define BC_PARA_LIGHTBULB_BRIGHTNESS ((u8*)&bc_para_light_bri)
    EXTERN unsigned char bc_para_light_bri;
#else
    #define BC_PARA_LIGHTBULB_BRIGHTNESS 0
#endif

#if LIGHTBULB_HUE_BRC_ENABLE
    #define BC_PARA_LIGHTBULB_HUE ((u8*)&bc_para_light_hue)
    EXTERN unsigned char bc_para_light_hue;
#else
    #define BC_PARA_LIGHTBULB_HUE 0
#endif

#if LIGHTBULB_SATURATION_BRC_ENABLE
    #define BC_PARA_LIGHTBULB_SATURATION ((u8*)&bc_para_light_satu)
    EXTERN unsigned char bc_para_light_satu;
#else
    #define BC_PARA_LIGHTBULB_SATURATION 0
#endif

#if LIGHTBULB_COLOR_TEMPERATURE_BRC_ENABLE
    #define BC_PARA_LIGHTBULB_COLOR_TEMP ((u8*)&bc_para_light_color)
    EXTERN unsigned char bc_para_light_color;
#else
    #define BC_PARA_LIGHTBULB_COLOR_TEMP 0
#endif

#if LIGHTBULB_LINKED_SERVICES_ENABLE
	#define LIGHTBULB_LINKED_SVCS			hLightBulb_linked_svcs
	#define LIGHTBULB_LINKED_SVCS_LENGTH 	(sizeof(hLightBulb_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * hLightBulb_linked_svcs[];
#else
	#define LIGHTBULB_LINKED_SVCS			0
	#define LIGHTBULB_LINKED_SVCS_LENGTH 	0
#endif

/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN void light_enable (int en);
EXTERN int lightOnReadCallback(void *pp);
EXTERN int lightOnWriteCallback(void *pp);
EXTERN int lightHueReadCallback(void *pp);
EXTERN int lightHueWriteCallback(void *pp);
EXTERN int ColorSaturationReadCallback(void *pp);
EXTERN int ColorSaturationWriteCallback(void *pp);
EXTERN int ColorTemperatureReadCallback(void *pp);
EXTERN int ColorTemperatureWriteCallback(void *pp);
EXTERN int lightBrightnessReadCallback(void *pp);
EXTERN int lightBrightnessWriteCallback(void *pp);
EXTERN int ccc_light_onoffWriteCB(void *pp);
EXTERN int ccc_light_briWriteCB(void *pp);
EXTERN int ccc_light_hueWriteCB(void *pp);
EXTERN int ccc_light_satuWriteCB(void *pp);
EXTERN int ccc_light_col_tempWriteCB(void *pp);

#endif /* _HK_LIGHTBULB_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
