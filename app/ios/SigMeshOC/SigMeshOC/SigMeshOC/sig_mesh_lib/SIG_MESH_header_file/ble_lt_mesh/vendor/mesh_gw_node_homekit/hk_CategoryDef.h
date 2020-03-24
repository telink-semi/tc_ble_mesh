/********************************************************************************************************
 * @file     hk_CategoryDef.h
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
#ifndef _HK_CATEGORY_DEF_H_
#define _HK_CATEGORY_DEF_H_


/* ------------------------------------
    Header Files
   ------------------------------------ */


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_CATEGORY_DEF_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif

/** @defgroup HK_CATEGORY Top level for category configuration
 * @{
 */

/**
 *  @brief  Definition for Accessory Categories Value by Apple HAP define
 */

/* Accessory Categories Value by Apple HAP define */
#define CATEGORY_OTHER                  1
#define CATEGORY_BRIDGE                 2
#define CATEGORY_FAN                    3
#define CATEGORY_GARAGE                 4
#define CATEGORY_LIGHTBULB              5
#define CATEGORY_DOOR_LOCK              6
#define CATEGORY_OUTLET                 7
#define CATEGORY_SWITCH                 8
#define CATEGORY_THERMOSTAT             9
#define CATEGORY_SENSOR                 10
#define CATEGORY_SECURITY_SYSTEM        11
#define CATEGORY_DOOR                   12
#define CATEGORY_WINDOW                 13
#define CATEGORY_WINDOW_COVERING        14
#define CATEGORY_PROGRAMMABLE_SWITCH    15
//-#define CATEGORY_RANGE_EXTENDER         16
#define CATEGORY_RESERVED_16            16
#define CATEGORY_IP_CAMERA              17
#define CATEGORY_VIDEO_DOOR_BELL        18
#define CATEGORY_AIR_PURIFIER           19
#define CATEGORY_HEATERS                20
#define CATEGORY_AIR_CONDITIONERS       21
#define CATEGORY_HUMIDIFIERS            22
#define CATEGORY_DEHUMIDIFIERS          23
#define CATEGORY_RESERVED_24            24
#define CATEGORY_RESERVED_25            25
#define CATEGORY_RESERVED_26            26
#define CATEGORY_RESERVED_27            27
#define CATEGORY_SPRINKLERS             28
#define CATEGORY_FAUCETS                29
#define CATEGORY_SHOWER_SYSTEMS         30
#define CATEGORY_RESERVED_999           31


#define HK_CATEGORY     CATEGORY_LIGHTBULB  //!< HK_CATEGORY configuration

/** @} End HK_CATEGORY */


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */


#endif /* _HK_CATEGORY_DEF_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
