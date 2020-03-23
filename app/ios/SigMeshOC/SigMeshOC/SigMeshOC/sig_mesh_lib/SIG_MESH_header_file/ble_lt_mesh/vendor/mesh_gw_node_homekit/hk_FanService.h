/********************************************************************************************************
 * @file     hk_FanService.h
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
#ifndef _HK_FAN_SERVICE_H_
#define _HK_FAN_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_FAN_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif

#define CHAR_FAN_SVC_SIGN_ENABLE      0
#if CHAR_FAN_SVC_SIGN_ENABLE
    #define FAN_AS_PRIMARY_SERVICE    	0
    #define FAN_IS_HIDDEN_SERVICE     	0
    #define FAN_LINKED_SERVICES_ENABLE	0
    EXTERN const hap_characteristic_desc_t hap_desc_char_fan_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_fan_A5_R_S;
    EXTERN unsigned char VAR_FAN_UNKOWN;
#else
    #define FAN_AS_PRIMARY_SERVICE    	0
    #define FAN_IS_HIDDEN_SERVICE     	0
    #define FAN_LINKED_SERVICES_ENABLE	0
#endif

#define CHAR_ROTATION_DIRECTION_ENABLE      0
#define CHAR_ROTATION_SPEED_ENABLE          0

#define FAN_ON_IND_ENABLE               1
#define FAN_ROTATIONDIRECT_IND_ENABLE   0
#define FAN_ROTATIONSPEED_IND_ENABLE    0

#define FAN_ON_BRC_ENABLE               0
#define FAN_ROTATIONDIRECT_BRC_ENABLE   0
#define FAN_ROTATIONSPEED_BRC_ENABLE    0


#define FAN_ONOFF_NO      3
#if FAN_ON_IND_ENABLE
    #define FAN_ON_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hFanOn), ccc_hFanOnWriteCB, 0},
    #define FAN_ON_IND_NO  1
#else
    #define FAN_ON_IND_DESC
    #define FAN_ON_IND_NO  0
#endif

#if CHAR_ROTATION_DIRECTION_ENABLE
    #define ROTA_DIR_NO 3
    #define CHAR_ROTATION_DIRECTION_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_ROTATION_DIRECTION_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof (hFanRotationDirect), 0, (u8*)(&charRotationDirectionUUID), (u8*)&hFanRotationDirect, hFanRotationDirectWriteCallback, hFanRotationDirectReadCallback},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + FAN_INST_NO + 3), 0, 0}, \
            FAN_ROTATIONDIRECT_IND_DESC

    #if FAN_ROTATIONDIRECT_IND_ENABLE
        #define FAN_ROTATIONDIRECT_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hFanRotationDirect), ccc_hFanRotationDirectWriteCB, 0},
        #define FAN_ROTATIONDIRECT_IND_NO  1
    #else
        #define FAN_ROTATIONDIRECT_IND_DESC
        #define FAN_ROTATIONDIRECT_IND_NO  0
    #endif
#else
    #define ROTA_DIR_NO 0
    #define CHAR_ROTATION_DIRECTION_ATT_TABLE
    #define FAN_ROTATIONDIRECT_IND_NO  0
#endif

#if CHAR_ROTATION_SPEED_ENABLE
    #define ROTA_SPD_NO 3
    #define CHAR_ROTATION_SPEED_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_ROTATION_SPEED_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof (hFanRotationSpeed), 0, (u8*)(&charRotationSpeedUUID), (u8*)&hFanRotationSpeed, hFanRotationSpeedWriteCallback, hFanRotationSpeedReadCallback},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + FAN_INST_NO + 4), 0, 0}, \
            FAN_ROTATIONSPEED_IND_DESC

    #if FAN_ROTATIONSPEED_IND_ENABLE
        #define FAN_ROTATIONSPEED_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hFanRotationSpeed), ccc_hFanRotationSpeedWriteCB, 0},
        #define FAN_ROTATIONSPEED_IND_NO  1
    #else
        #define FAN_ROTATIONSPEED_IND_DESC
        #define FAN_ROTATIONSPEED_IND_NO  0
    #endif
#else
    #define ROTA_SPD_NO 0
    #define CHAR_ROTATION_SPEED_ATT_TABLE
    #define FAN_ROTATIONSPEED_IND_NO  0
#endif

#if CHAR_FAN_SVC_SIGN_ENABLE
    #define FAN_SVC_SIGN_NO   3
    #define CHAR_FAN_SVC_A5_SIGN_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_fan_A5_R_S, &hap_desc_service_fan_A5_R_S},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_FAN_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_FAN_UNKOWN, 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + FAN_INST_NO + 5), 0, 0},
#else
    #define FAN_SVC_SIGN_NO   0
    #define CHAR_FAN_SVC_A5_SIGN_TABLE
#endif

#define FAN_NO_DELTA    FAN_ON_IND_NO  \
                        +FAN_ROTATIONDIRECT_IND_NO  \
                        +FAN_ROTATIONSPEED_IND_NO
#define FAN_NO          9 + ROTA_DIR_NO + ROTA_SPD_NO + FAN_SVC_SIGN_NO
#define FAN_HANDLE_NUM (FAN_NO + FAN_NO_DELTA)


#define HOMEKIT_FAN_SERVICE_ATT_TABLE   \
    {FAN_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(fanServiceUUID),sizeof(fanServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(fanServiceUUID), 0, 0},   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(insts + FAN_INST_NO), 0, 0},   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_NAME, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(hFanServiceName), 0, (u8*)(nameUUID), (u8*)(hFanServiceName), 0, 0},  \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + FAN_INST_NO + 1), 0, 0}, \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_FAN_ON_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,1,0, (u8*)(charOnUUID), (u8*)&hFanOn, FanOnWriteCallback, FanOnReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + FAN_INST_NO + 2), 0, 0},  \
            FAN_ON_IND_DESC \
        CHAR_ROTATION_DIRECTION_ATT_TABLE \
        CHAR_ROTATION_SPEED_ATT_TABLE   \
        CHAR_FAN_SVC_A5_SIGN_TABLE


#define FAN_HANDLE_COMMON_NUM     8

#define FAN_HANDLE_NUM_HFANON               FAN_HANDLE_START_NUM +FAN_HANDLE_COMMON_NUM
#define FAN_HANDLE_NUM_HFANROTATIONDIRECT   FAN_HANDLE_NUM_HFANON +FAN_ONOFF_NO +FAN_ON_IND_NO
#define FAN_HANDLE_NUM_HFANROTATIONSPEED    FAN_HANDLE_NUM_HFANROTATIONDIRECT +ROTA_DIR_NO +FAN_ROTATIONDIRECT_IND_NO
#define FAN_HANDLE_NUM_THE_END


#define FAN_STORE_CHAR_VALUE_T_FANON                                u32 FanOn:1;    /* 0~1 */
#define FAN_STORE_CHAR_VALUE_T_FANON_STORE                              STORE_CHAR(FanOn,hFanOn)
#define FAN_STORE_CHAR_VALUE_T_FANON_RESTORE                            RESTORE_CHAR(FanOn,hFanOn)

#if CHAR_ROTATION_DIRECTION_ENABLE
#define FAN_STORE_CHAR_VALUE_T_CHAR_FANROTATIONDIRECT_ENABLE        u32 FanRotationDirect:1;   /* 0~1 */
#define FAN_STORE_CHAR_VALUE_T_CHAR_FANROTATIONDIRECT_ENABLE_STORE      STORE_CHAR(FanRotationDirect,hFanRotationDirect)
#define FAN_STORE_CHAR_VALUE_T_CHAR_FANROTATIONDIRECT_ENABLE_RESTORE    RESTORE_CHAR(FanRotationDirect,hFanRotationDirect)
#else
#define FAN_STORE_CHAR_VALUE_T_CHAR_FANROTATIONDIRECT_ENABLE
#define FAN_STORE_CHAR_VALUE_T_CHAR_FANROTATIONDIRECT_ENABLE_STORE
#define FAN_STORE_CHAR_VALUE_T_CHAR_FANROTATIONDIRECT_ENABLE_RESTORE
#endif

#if CHAR_ROTATION_SPEED_ENABLE
#define FAN_STORE_CHAR_VALUE_T_CHAR_FANROTATIONSPEED_ENABLE         u32 FanRotationSpeed;     /* float */
#define FAN_STORE_CHAR_VALUE_T_CHAR_FANROTATIONSPEED_ENABLE_STORE       STORE_CHAR(FanRotationSpeed,hFanRotationSpeed)
#define FAN_STORE_CHAR_VALUE_T_CHAR_FANROTATIONSPEED_ENABLE_RESTORE     RESTORE_CHAR(FanRotationSpeed,hFanRotationSpeed)
#else
#define FAN_STORE_CHAR_VALUE_T_CHAR_FANROTATIONSPEED_ENABLE
#define FAN_STORE_CHAR_VALUE_T_CHAR_FANROTATIONSPEED_ENABLE_STORE
#define FAN_STORE_CHAR_VALUE_T_CHAR_FANROTATIONSPEED_ENABLE_RESTORE
#endif

#define FAN_STORE_CHAR_VALUE_T            \
                    FAN_STORE_CHAR_VALUE_T_FANON    \
                    FAN_STORE_CHAR_VALUE_T_CHAR_FANROTATIONDIRECT_ENABLE    \
                    FAN_STORE_CHAR_VALUE_T_CHAR_FANROTATIONSPEED_ENABLE

#define FAN_STORE_CHAR_TO_FLASH           \
                    FAN_STORE_CHAR_VALUE_T_FANON_STORE  \
                    FAN_STORE_CHAR_VALUE_T_CHAR_FANROTATIONDIRECT_ENABLE_STORE  \
                    FAN_STORE_CHAR_VALUE_T_CHAR_FANROTATIONSPEED_ENABLE_STORE

#define FAN_RESTORE_CHAR_FROM_FLASH       \
                    FAN_STORE_CHAR_VALUE_T_FANON_RESTORE    \
                    FAN_STORE_CHAR_VALUE_T_CHAR_FANROTATIONDIRECT_ENABLE_RESTORE    \
                    FAN_STORE_CHAR_VALUE_T_CHAR_FANROTATIONSPEED_ENABLE_RESTORE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN unsigned char hFanOn;
EXTERN unsigned int hFanRotationDirect;
EXTERN unsigned int hFanRotationSpeed;

EXTERN const u8 hFanServiceName[10];

EXTERN unsigned short ccc_hFanOn;
EXTERN unsigned short ccc_hFanRotationDirect;
EXTERN unsigned short ccc_hFanRotationSpeed;

#if FAN_ON_BRC_ENABLE
    #define BC_PARA_FAN_ON ((u8*)&bc_para_hFanOn)
    EXTERN unsigned char bc_para_hFanOn;
#else
    #define BC_PARA_FAN_ON 0
#endif

#if FAN_ROTATIONDIRECT_BRC_ENABLE
    #define BC_PARA_FAN_ROTATIONDIRECT ((u8*)&bc_para_hFanRotationDirect)
    EXTERN unsigned char bc_para_hFanRotationDirect;
#else
    #define BC_PARA_FAN_ROTATIONDIRECT 0
#endif

#if FAN_ROTATIONSPEED_BRC_ENABLE
    #define BC_PARA_FAN_ROTATIONSPEED ((u8*)&bc_para_hFanRotationSpeed)
    EXTERN unsigned char bc_para_hFanRotationSpeed;
#else
    #define BC_PARA_FAN_ROTATIONSPEED 0
#endif

#if FAN_LINKED_SERVICES_ENABLE
	#define FAN_LINKED_SVCS			hFan_linked_svcs
	#define FAN_LINKED_SVCS_LENGTH 	(sizeof(hFan_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * hFan_linked_svcs[];
#else
	#define FAN_LINKED_SVCS			0
	#define FAN_LINKED_SVCS_LENGTH 	0
#endif

EXTERN const hap_characteristic_desc_t hap_desc_char_FAN_ON_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_ROTATION_DIRECTION_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_ROTATION_SPEED_R_S;


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN int FanOnReadCallback(void *pp);
EXTERN int FanOnWriteCallback(void *pp);
EXTERN int hFanRotationDirectReadCallback(void *pp);
EXTERN int hFanRotationDirectWriteCallback(void *pp);
EXTERN int hFanRotationSpeedReadCallback(void *pp);
EXTERN int hFanRotationSpeedWriteCallback(void *pp);
EXTERN int ccc_hFanOnWriteCB(void *pp);
EXTERN int ccc_hFanRotationDirectWriteCB(void *pp);
EXTERN int ccc_hFanRotationSpeedWriteCB(void *pp);

#endif /* _HK_FAN_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
