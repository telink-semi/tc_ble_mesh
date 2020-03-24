/********************************************************************************************************
 * @file     hk_SlatService.h
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
#ifndef _HK_SLAT_SERVICE_H_
#define _HK_SLAT_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_SLAT_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif

#define CHAR_SLAT_SVC_SIGN_ENABLE      0
#if CHAR_SLAT_SVC_SIGN_ENABLE
    #define SLAT_AS_PRIMARY_SERVICE    		0
    #define SLAT_IS_HIDDEN_SERVICE     		0
    #define SLAT_LINKED_SERVICES_ENABLE		0
    EXTERN const hap_characteristic_desc_t hap_desc_char_slat_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_slat_A5_R_S;
    EXTERN unsigned char VAR_SLAT_UNKOWN;
#else
    #define SLAT_AS_PRIMARY_SERVICE    		0
    #define SLAT_IS_HIDDEN_SERVICE     		0
    #define SLAT_LINKED_SERVICES_ENABLE		0
#endif

#define CHAR_SLAT_SWING_MODE_ENABLE				0
#define CHAR_SLAT_CURRENT_TILT_ANGLE_ENABLE		0
#define CHAR_SLAT_TARGET_TILT_ANGLE_ENABLE		0

#define SLAT_CURRENT_SLAT_STATE_IND_ENABLE      0
#define SLAT_SWING_MODE_IND_ENABLE      		0
#define SLAT_CURRENT_TILT_ANGLE_IND_ENABLE      0
#define SLAT_TARGET_TILT_ANGLE_IND_ENABLE      	0

#define SLAT_CURRENT_SLAT_STATE_BRC_ENABLE      0
#define SLAT_SWING_MODE_BRC_ENABLE      		0
#define SLAT_CURRENT_TILT_ANGLE_BRC_ENABLE      0
#define SLAT_TARGET_TILT_ANGLE_BRC_ENABLE      	0

#define SLAT_CURRENT_SLAT_STATE_NO  3
#if SLAT_CURRENT_SLAT_STATE_IND_ENABLE
    #define SLAT_CURRENT_SLAT_STATE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hCurrentSlatState), ccc_hCurrentSlatStateWriteCB, 0},
    #define SLAT_CURRENT_SLAT_STATE_IND_NO  1
#else
    #define SLAT_CURRENT_SLAT_STATE_IND_DESC
    #define SLAT_CURRENT_SLAT_STATE_IND_NO  0
#endif

#define SLAT_SLAT_TYPE_NO  3

#if CHAR_SLAT_SWING_MODE_ENABLE
    #define SLAT_SWING_MODE_NO   3
    #define CHAR_SLAT_SWING_MODE_ATT_TABLE   \
            {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_SlatSwingMode_R_S, 0},    \
            {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSlatSwingMode),0, (u8*)(charSwingModeUUID), (u8*)&hSlatSwingMode, SlatSwingModeWriteCallback, SlatSwingModeReadCallback}, \
                {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SLAT_INST_NO + 4), 0, 0}, \
                SLAT_SWING_MODE_IND_DESC

    #if SLAT_SWING_MODE_IND_ENABLE
        #define SLAT_SWING_MODE_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSlatSwingMode), ccc_hSlatSwingModeWriteCB, 0},
        #define SLAT_SWING_MODE_IND_NO  1
    #else
        #define SLAT_SWING_MODE_IND_DESC
        #define SLAT_SWING_MODE_IND_NO  0
    #endif
#else
    #define SLAT_SWING_MODE_NO   0
    #define CHAR_SLAT_SWING_MODE_ATT_TABLE
    #define SLAT_SWING_MODE_IND_NO  0
#endif

#if CHAR_SLAT_CURRENT_TILT_ANGLE_ENABLE
    #define SLAT_CURRENT_TILT_ANGLE_NO   3
    #define CHAR_SLAT_CURRENT_TILT_ANGLE_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_SlatCurrentTiltAngle_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSlatCurrentTiltAngle),0, (u8*)(charCurrentTiltAngleUUID), (u8*)&hSlatCurrentTiltAngle, 0, SlatCurrentTiltAngleReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SLAT_INST_NO + 5), 0, 0},  \
                SLAT_CURRENT_TILT_ANGLE_IND_DESC

    #if SLAT_CURRENT_TILT_ANGLE_IND_ENABLE
        #define SLAT_CURRENT_TILT_ANGLE_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSlatCurrentTiltAngle), ccc_hSlatCurrentTiltAngleWriteCB, 0},
        #define SLAT_CURRENT_TILT_ANGLE_IND_NO  1
    #else
        #define SLAT_CURRENT_TILT_ANGLE_IND_DESC
        #define SLAT_CURRENT_TILT_ANGLE_IND_NO  0
    #endif
#else
    #define SLAT_CURRENT_TILT_ANGLE_NO   0
    #define CHAR_SLAT_CURRENT_TILT_ANGLE_ATT_TABLE
    #define SLAT_CURRENT_TILT_ANGLE_IND_NO  0
#endif

#if CHAR_SLAT_TARGET_TILT_ANGLE_ENABLE
    #define SLAT_TARGET_TILT_ANGLE_NO   3
    #define CHAR_SLAT_TARGET_TILT_ANGLE_ATT_TABLE   \
            {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_SlatTargetTiltAngle_R_S, 0},    \
            {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSlatTargetTiltAngle),0, (u8*)(charTargetTiltAngleUUID), (u8*)&hSlatTargetTiltAngle, SlatTargetTiltAngleWriteCallback, SlatTargetTiltAngleReadCallback}, \
                {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SLAT_INST_NO + 6), 0, 0}, \
                SLAT_TARGET_TILT_ANGLE_IND_DESC

    #if SLAT_TARGET_TILT_ANGLE_IND_ENABLE
        #define SLAT_TARGET_TILT_ANGLE_IND_DESC \
            {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hSlatTargetTiltAngle), ccc_hSlatTargetTiltAngleWriteCB, 0},
        #define SLAT_TARGET_TILT_ANGLE_IND_NO  1
    #else
        #define SLAT_TARGET_TILT_ANGLE_IND_DESC
        #define SLAT_TARGET_TILT_ANGLE_IND_NO  0
    #endif
#else
    #define SLAT_TARGET_TILT_ANGLE_NO   0
    #define CHAR_SLAT_TARGET_TILT_ANGLE_ATT_TABLE
    #define SLAT_TARGET_TILT_ANGLE_IND_NO  0
#endif


#if CHAR_SLAT_SVC_SIGN_ENABLE
    #define SLAT_SVC_SIGN_NO   3
    #define CHAR_SLAT_SVC_A5_SIGN_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_slat_A5_R_S, &hap_desc_service_slat_A5_R_S},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_SLAT_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_SLAT_UNKOWN, 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SLAT_INST_NO + 7), 0, 0},
#else
    #define SLAT_SVC_SIGN_NO   0
    #define CHAR_SLAT_SVC_A5_SIGN_TABLE
#endif


#define SLAT_NO_DELTA    SLAT_CURRENT_SLAT_STATE_IND_NO \
						+SLAT_SWING_MODE_IND_NO \
						+SLAT_CURRENT_TILT_ANGLE_IND_NO \
						+SLAT_TARGET_TILT_ANGLE_IND_NO
						
#define SLAT_NO          12 +SLAT_SWING_MODE_NO \
						+SLAT_CURRENT_TILT_ANGLE_NO \
						+SLAT_TARGET_TILT_ANGLE_NO \
						+SLAT_SVC_SIGN_NO
						
#define SLAT_HANDLE_NUM (SLAT_NO + SLAT_NO_DELTA)


#define HOMEKIT_SLAT_SERVICE_ATT_TABLE   \
    {SLAT_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(slatServiceUUID),sizeof(slatServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(slatServiceUUID), 0, 0},   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(insts + SLAT_INST_NO), 0, 0},   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_NAME, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(hSlatServiceName), 0, (u8*)(nameUUID), (u8*)(hSlatServiceName), 0, 0},  \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + SLAT_INST_NO + 1), 0, 0}, \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_CurrentSlatState_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hCurrentSlatState),0, (u8*)(charCurrentSlatStateUUID), (u8*)&hCurrentSlatState, 0, CurrentSlatStateReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SLAT_INST_NO + 2), 0, 0},  \
            SLAT_CURRENT_SLAT_STATE_IND_DESC \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_SlatType_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hSlatType),0, (u8*)(charSlatTypeUUID), (u8*)&hSlatType, 0, SlatTypeReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + SLAT_INST_NO + 3), 0, 0},  \
            CHAR_SLAT_SWING_MODE_ATT_TABLE \
            CHAR_SLAT_CURRENT_TILT_ANGLE_ATT_TABLE \
            CHAR_SLAT_TARGET_TILT_ANGLE_ATT_TABLE \
        	CHAR_SLAT_SVC_A5_SIGN_TABLE


#define SLAT_HANDLE_COMMON_NUM     8

#define SLAT_HANDLE_NUM_HSLAT_CURRENT_SLAT_STATE    SLAT_HANDLE_START_NUM +SLAT_HANDLE_COMMON_NUM
#define SLAT_HANDLE_NUM_HSLAT_SLAT_TYPE           	SLAT_HANDLE_NUM_HSLAT_CURRENT_SLAT_STATE +SLAT_CURRENT_SLAT_STATE_NO +SLAT_CURRENT_SLAT_STATE_IND_NO
#define SLAT_HANDLE_NUM_HSLAT_SWING_MODE            SLAT_HANDLE_NUM_HSLAT_SLAT_TYPE +SLAT_SLAT_TYPE_NO +0
#define SLAT_HANDLE_NUM_HSLAT_CURRENT_TILT_ANGLE	SLAT_HANDLE_NUM_HSLAT_SWING_MODE +SLAT_SWING_MODE_NO +SLAT_SWING_MODE_IND_NO
#define SLAT_HANDLE_NUM_HSLAT_TARGET_TILT_ANGLE		SLAT_HANDLE_NUM_HSLAT_CURRENT_TILT_ANGLE +SLAT_CURRENT_TILT_ANGLE_NO +SLAT_CURRENT_TILT_ANGLE_IND_NO
#define SLAT_HANDLE_NUM_THE_END


#define SLAT_STORE_CHAR_VALUE_T_HSLAT_CURRENT_SLAT_STATE            u32 CurrentSlatState:2;    /* 0~2 */
#define SLAT_STORE_CHAR_VALUE_T_HSLAT_CURRENT_SLAT_STATE_STORE          STORE_CHAR(CurrentSlatState,hCurrentSlatState)
#define SLAT_STORE_CHAR_VALUE_T_HSLAT_CURRENT_SLAT_STATE_RESTORE        RESTORE_CHAR(CurrentSlatState,hCurrentSlatState)

#define SLAT_STORE_CHAR_VALUE_T_HSLAT_SLAT_TYPE                     u32 SlatType:1;    /* 0~1 */
#define SLAT_STORE_CHAR_VALUE_T_HSLAT_SLAT_TYPE_STORE                   STORE_CHAR(SlatType,hSlatType)
#define SLAT_STORE_CHAR_VALUE_T_HSLAT_SLAT_TYPE_RESTORE                 RESTORE_CHAR(SlatType,hSlatType)

#define SLAT_STORE_CHAR_VALUE_T_HSLAT_SWING_MODE                 	u32 SlatSwingMode:1;    /* 0~1 */
#define SLAT_STORE_CHAR_VALUE_T_HSLAT_SWING_MODE_STORE               	STORE_CHAR(SlatSwingMode,hSlatSwingMode)
#define SLAT_STORE_CHAR_VALUE_T_HSLAT_SWING_MODE_RESTORE             	RESTORE_CHAR(SlatSwingMode,hSlatSwingMode)

#define SLAT_STORE_CHAR_VALUE_T_HSLAT_CURRENT_TILT_ANGLE            u32 SlatCurrentTiltAngle;  /* int */
#define SLAT_STORE_CHAR_VALUE_T_HSLAT_CURRENT_TILT_ANGLE_STORE          STORE_CHAR(SlatCurrentTiltAngle,hSlatCurrentTiltAngle)
#define SLAT_STORE_CHAR_VALUE_T_HSLAT_CURRENT_TILT_ANGLE_RESTORE        RESTORE_CHAR(SlatCurrentTiltAngle,hSlatCurrentTiltAngle)

#define SLAT_STORE_CHAR_VALUE_T_HSLAT_TARGET_TILT_ANGLE             u32 SlatTargetTiltAngle;  /* int */
#define SLAT_STORE_CHAR_VALUE_T_HSLAT_TARGET_TILT_ANGLE_STORE           STORE_CHAR(SlatTargetTiltAngle,hSlatTargetTiltAngle)
#define SLAT_STORE_CHAR_VALUE_T_HSLAT_TARGET_TILT_ANGLE_RESTORE         RESTORE_CHAR(SlatTargetTiltAngle,hSlatTargetTiltAngle)

#define SLAT_STORE_CHAR_VALUE_T  \
                    SLAT_STORE_CHAR_VALUE_T_HSLAT_CURRENT_SLAT_STATE  \
                    SLAT_STORE_CHAR_VALUE_T_HSLAT_SLAT_TYPE \
                    SLAT_STORE_CHAR_VALUE_T_HSLAT_SWING_MODE \
                    SLAT_STORE_CHAR_VALUE_T_HSLAT_CURRENT_TILT_ANGLE \
                    SLAT_STORE_CHAR_VALUE_T_HSLAT_TARGET_TILT_ANGLE

#define SLAT_STORE_CHAR_TO_FLASH  \
                    SLAT_STORE_CHAR_VALUE_T_HSLAT_CURRENT_SLAT_STATE_STORE  \
                    SLAT_STORE_CHAR_VALUE_T_HSLAT_SLAT_TYPE_STORE \
                    SLAT_STORE_CHAR_VALUE_T_HSLAT_SWING_MODE_STORE \
                    SLAT_STORE_CHAR_VALUE_T_HSLAT_CURRENT_TILT_ANGLE_STORE \
                    SLAT_STORE_CHAR_VALUE_T_HSLAT_TARGET_TILT_ANGLE_STORE

#define SLAT_RESTORE_CHAR_FROM_FLASH  \
                    SLAT_STORE_CHAR_VALUE_T_HSLAT_CURRENT_SLAT_STATE_RESTORE  \
                    SLAT_STORE_CHAR_VALUE_T_HSLAT_SLAT_TYPE_RESTORE \
                    SLAT_STORE_CHAR_VALUE_T_HSLAT_SWING_MODE_RESTORE \
                    SLAT_STORE_CHAR_VALUE_T_HSLAT_CURRENT_TILT_ANGLE_RESTORE \
                    SLAT_STORE_CHAR_VALUE_T_HSLAT_TARGET_TILT_ANGLE_RESTORE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN unsigned char hCurrentSlatState;
EXTERN unsigned char hSlatType;
EXTERN unsigned char hSlatSwingMode;
EXTERN int hSlatCurrentTiltAngle;
EXTERN int hSlatTargetTiltAngle;

EXTERN unsigned short ccc_hCurrentSlatState;
EXTERN unsigned short ccc_hSlatSwingMode;
EXTERN unsigned short ccc_hSlatCurrentTiltAngle;
EXTERN unsigned short ccc_hSlatTargetTiltAngle;


#if SLAT_CURRENT_SLAT_STATE_BRC_ENABLE
    #define BC_PARA_SLAT_CURRENT_SLAT_STATE ((u8*)&bc_para_hCurrentSlatState)
    EXTERN unsigned char bc_para_hCurrentSlatState;
#else
    #define BC_PARA_SLAT_CURRENT_SLAT_STATE 0
#endif

#if SLAT_SWING_MODE_BRC_ENABLE
    #define BC_PARA_SLAT_SWING_MODE ((u8*)&bc_para_hSlatSwingMode)
    EXTERN unsigned char bc_para_hSlatSwingMode;
#else
    #define BC_PARA_SLAT_SWING_MODE 0
#endif


#if SLAT_CURRENT_TILT_ANGLE_BRC_ENABLE
    #define BC_PARA_SLAT_CURRENT_TILT_ANGLE ((u8*)&bc_para_hSlatCurrentTiltAngle)
    EXTERN unsigned char bc_para_hSlatCurrentTiltAngle;
#else
    #define BC_PARA_SLAT_CURRENT_TILT_ANGLE 0
#endif

#if SLAT_TARGET_TILT_ANGLE_BRC_ENABLE
    #define BC_PARA_SLAT_TARGET_TILT_ANGLE ((u8*)&bc_para_hSlatTargetTiltAngle)
    EXTERN unsigned char bc_para_hSlatTargetTiltAngle;
#else
    #define BC_PARA_SLAT_TARGET_TILT_ANGLE 0
#endif

#if SLAT_LINKED_SERVICES_ENABLE
	#define SLAT_LINKED_SVCS			hSlat_linked_svcs
	#define SLAT_LINKED_SVCS_LENGTH 	(sizeof(hSlat_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * hSlat_linked_svcs[];
#else
	#define SLAT_LINKED_SVCS			0
	#define SLAT_LINKED_SVCS_LENGTH 	0
#endif

EXTERN const u8 hSlatServiceName[11];

EXTERN const hap_characteristic_desc_t hap_desc_char_CurrentSlatState_R_S ;
EXTERN const hap_characteristic_desc_t hap_desc_char_SlatType_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_SlatSwingMode_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_SlatCurrentTiltAngle_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_SlatTargetTiltAngle_R_S;

/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN int CurrentSlatStateReadCallback(void *pp);
EXTERN int SlatTypeReadCallback(void *pp);
EXTERN int SlatSwingModeReadCallback(void *pp);
EXTERN int SlatSwingModeWriteCallback(void *pp);
EXTERN int SlatCurrentTiltAngleReadCallback(void *pp);
EXTERN int SlatTargetTiltAngleReadCallback(void *pp);
EXTERN int SlatTargetTiltAngleWriteCallback(void *pp);
	
EXTERN int ccc_hCurrentSlatStateWriteCB(void *pp);
EXTERN int ccc_hSlatSwingModeWriteCB(void *pp);
EXTERN int ccc_hSlatCurrentTiltAngleWriteCB(void *pp);
EXTERN int ccc_hSlatTargetTiltAngleWriteCB(void *pp);


#endif /* _HK_SLAT_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
