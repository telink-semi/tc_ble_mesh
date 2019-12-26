/********************************************************************************************************
 * @file     hk_TopUUID.c
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
#define _HK_TOP_UUID_C_
 
 
/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"

#include "../common/led_cfg.h"
#include "ble_UUID.h"
#include "hk_TopUUID.h"


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */


/* ------------------------------------
    Type Definitions
   ------------------------------------ */
#define UUID16_HOMEKIT_BASE     0xFE00
#define UUID96_HOMEKIT_BASE     0x91,0x52,0x76,0xBB,0x26,0x00,0x00,0x80,0x00,0x10,0x00,0x00


/* ------------------------------------
    Variables Definitions
   ------------------------------------ */

const u8 manufacturer[19]       = "WWW.TELINK-SEMI.COM";
const u8 model[24]              = "HomekitSolution_TLSR8258"; 
const u8 name[20]               = "AppleHomekitSolution";
const u8 spp_pairname[4]        = "Pair";
const u8 my_devName[20]         = DEVICE_FULL_NAME;
const u8 hap_ble_version[5]     = "2.2.0";
const u8 firmware_revision[6]   = "3.13.0";
const u8 ServiceChangedRange[4] = {1, 0, 0xFF, 0xFF};

const u16 insts[100] = { 1,  2,  3,  4,  5,  6,  7,  8,  9, 10,
                        11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
                        21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
                        31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
                        41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
                        51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
                        61, 62, 63, 64, 65, 66, 67, 68, 69, 70,
                        71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
                        81, 82, 83, 84, 85, 86, 87, 88, 89, 90,
                        91, 92, 93, 94, 95, 96, 97, 98, 99, 100,};


#if SERIAL_NUMBER_RANDOM_ENABLE
u8 serialNumber[10]         = "6369710810";
#else
const u8 serialNumber[9]    = "26558339";
#endif

#if SETUP_CODE_RANDOM_ENABLE
u32 SetupCodeData = 0x15001500; //setup code: 15001500
u8 SetupCodeWriteCnts = 0;
u8 SetupCodeReadCnts = 0;
#endif

u32 SetupIdData = 0x3941305A;  //setup id: "Z0A9"

/* ------------------------------------
    Function Prototypes
   ------------------------------------ */


/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
