/********************************************************************************************************
 * @file     common.h
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
#ifndef _COMMON_H_
#define _COMMON_H_

#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "../common/led_cfg.h"

/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _COMMON_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif

// for OTA
#define OTA_LED                         PWM_W       // OTA_LED use gpio control LED


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN flash_adr_layout_def flash_adr_layout;

EXTERN u8 led_val[6];
EXTERN u8 light_onoff_status;

/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN void COMMON_Init(void);
//EXTERN void pwm_set_lum(int id, u8 y, int pol);
EXTERN void pwm_light_enable(u8 on);
EXTERN void rf_led_ota_ok(void);
EXTERN void rf_led_ota_error(void);



#endif /* _COMMON_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
