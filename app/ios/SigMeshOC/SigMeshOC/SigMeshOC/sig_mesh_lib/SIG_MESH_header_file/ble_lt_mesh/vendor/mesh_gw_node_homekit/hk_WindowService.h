/********************************************************************************************************
 * @file     hk_WindowService.h
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
#ifndef _HK_WINDOW_SERVICE_H_
#define _HK_WINDOW_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_WINDOW_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif


#define CHAR_WINDOW_SVC_SIGN_ENABLE      0
#if CHAR_WINDOW_SVC_SIGN_ENABLE
    #define WINDOW_AS_PRIMARY_SERVICE    	0
    #define WINDOW_IS_HIDDEN_SERVICE     	0
    #define WINDOW_LINKED_SERVICES_ENABLE	0
    EXTERN const hap_characteristic_desc_t hap_desc_char_window_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_window_A5_R_S;
    EXTERN unsigned char VAR_WINDOW_UNKOWN;
#else
    #define WINDOW_AS_PRIMARY_SERVICE    	0
    #define WINDOW_IS_HIDDEN_SERVICE     	0
    #define WINDOW_LINKED_SERVICES_ENABLE	0
#endif


#define CHAR_WINDOW_HOLDPOSITION_ENABLE			0
#define CHAR_WINDOW_OBSTRUCTIONDETECTED_ENABLE	0

#define WINDOW_CURRENTPOSITION_IND_ENABLE     0
#define WINDOW_TARGETPOSITION_IND_ENABLE      0
#define WINDOW_POSITIONSTATE_IND_ENABLE       0
#define WINDOW_OBSTRUCTIONDETECTED_IND_ENABLE 0

#define WINDOW_CURRENTPOSITION_BRC_ENABLE     0
#define WINDOW_TARGETPOSITION_BRC_ENABLE      0
#define WINDOW_POSITIONSTATE_BRC_ENABLE       0
#define WINDOW_OBSTRUCTIONDETECTED_BRC_ENABLE 0


#define WINDOW_CURRENTPOSITION_NO 3
#define WINDOW_TARGETPOSITION_NO 3
#define WINDOW_POSITIONSTATE_NO 3

#if WINDOW_CURRENTPOSITION_IND_ENABLE
    #define WINDOW_CURRENTPOSITION_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hWindowCurrentPosition), ccc_hWindowCurrentPositionWriteCB, 0},
    #define WINDOW_CURRENTPOSITION_IND_NO  1
#else
    #define WINDOW_CURRENTPOSITION_IND_DESC
    #define WINDOW_CURRENTPOSITION_IND_NO  0
#endif

#if WINDOW_TARGETPOSITION_IND_ENABLE
    #define WINDOW_TARGETPOSITION_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hWindowTargetPosition), ccc_hWindowTargetPositionWriteCB, 0},
    #define WINDOW_TARGETPOSITION_IND_NO  1
#else
    #define WINDOW_TARGETPOSITION_IND_DESC
    #define WINDOW_TARGETPOSITION_IND_NO  0
#endif

#if WINDOW_POSITIONSTATE_IND_ENABLE
    #define WINDOW_POSITIONSTATE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hWindowPositionState), ccc_hWindowPositionStateWriteCB, 0},
    #define WINDOW_POSITIONSTATE_IND_NO  1
#else
    #define WINDOW_POSITIONSTATE_IND_DESC
    #define WINDOW_POSITIONSTATE_IND_NO  0
#endif


#if CHAR_WINDOW_HOLDPOSITION_ENABLE
    #define WINDOW_HOLDPOSITION_NO 3
    #define CHAR_WINDOW_HOLDPOSITION_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_hWindowHoldPosition_R_S, 0}, \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hWindowHoldPosition),0, (u8*)(charHoldPositionUUID), (u8*)&hWindowHoldPosition, hWindowHoldPositionWriteCallback, 0},  \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + WINDOW_INST_NO + 5), 0, 0},
#else
    #define WINDOW_HOLDPOSITION_NO 0
    #define CHAR_WINDOW_HOLDPOSITION_ATT_TABLE
#endif

#if CHAR_WINDOW_OBSTRUCTIONDETECTED_ENABLE
    #define WINDOW_OBSTRUCTIONDETECTED_NO 3
    #define CHAR_WINDOW_OBSTRUCTIONDETECTED_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hWindowObstructionDetected_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hWindowObstructionDetected),0, (u8*)(charObstructionDetectedUUID), (u8*)&hWindowObstructionDetected, 0, hWindowObstructionDetectedReadCallback},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + WINDOW_INST_NO + 6), 0, 0},   \
            WINDOW_OBSTRUCTIONDETECTED_IND_DESC

	#if WINDOW_OBSTRUCTIONDETECTED_IND_ENABLE
	    #define WINDOW_OBSTRUCTIONDETECTED_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hWindowObstructionDetected), ccc_hWindowObstructionDetectedWriteCB, 0},
	    #define WINDOW_OBSTRUCTIONDETECTED_IND_NO  1
	#else
	    #define WINDOW_OBSTRUCTIONDETECTED_IND_DESC
	    #define WINDOW_OBSTRUCTIONDETECTED_IND_NO  0
	#endif
#else
    #define WINDOW_OBSTRUCTIONDETECTED_NO 0
    #define CHAR_WINDOW_OBSTRUCTIONDETECTED_ATT_TABLE
    #define WINDOW_OBSTRUCTIONDETECTED_IND_NO  0
#endif


#if CHAR_WINDOW_SVC_SIGN_ENABLE
#define CHAR_WINDOW_SVC_A5_SIGN_TABLE   \
    {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_window_A5_R_S, &hap_desc_service_window_A5_R_S},  \
    {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_WINDOW_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_WINDOW_UNKOWN, 0, 0},    \
        {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + WINDOW_INST_NO + 7), 0, 0},
#define WINDOW_SVC_SIGN_NO   3
#else
#define CHAR_WINDOW_SVC_A5_SIGN_TABLE
#define WINDOW_SVC_SIGN_NO   0
#endif


#define WINDOW_NO_DELTA WINDOW_CURRENTPOSITION_IND_NO   \
                        +WINDOW_TARGETPOSITION_IND_NO   \
                        +WINDOW_POSITIONSTATE_IND_NO    \
                        +WINDOW_OBSTRUCTIONDETECTED_IND_NO

#define WINDOW_NO       15 +WINDOW_HOLDPOSITION_NO \
						+WINDOW_OBSTRUCTIONDETECTED_NO \
						+WINDOW_SVC_SIGN_NO
						
#define WINDOW_HANDLE_NUM (WINDOW_NO + WINDOW_NO_DELTA)


#define HOMEKIT_WINDOW_SERVICE_ATT_TABLE    \
    {WINDOW_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(windowServiceUUID),sizeof(windowServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(windowServiceUUID), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(insts + WINDOW_INST_NO), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_NAME, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(hWindowServiceName), 0, (u8*)(nameUUID), (u8*)(hWindowServiceName), 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + WINDOW_INST_NO + 1), 0, 0},  \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hWindowCurrentPosition_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hWindowCurrentPosition),0, (u8*)(charCurrentPositionUUID), (u8*)&hWindowCurrentPosition, 0, hWindowCurrentPositionReadCallback},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + WINDOW_INST_NO + 2), 0, 0},   \
            WINDOW_CURRENTPOSITION_IND_DESC \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hWindowTargetPosition_R_S, 0},   \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hWindowTargetPosition),0, (u8*)(charTargetPositionUUID), (u8*)&hWindowTargetPosition, hWindowTargetPositionWriteCallback, hWindowTargetPositionReadCallback},  \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + WINDOW_INST_NO + 3), 0, 0},   \
            WINDOW_TARGETPOSITION_IND_DESC \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hWindowPositionState_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hWindowPositionState),0, (u8*)(charPositionStateUUID), (u8*)&hWindowPositionState, 0, hWindowPositionStateReadCallback},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + WINDOW_INST_NO + 4), 0, 0},   \
            WINDOW_POSITIONSTATE_IND_DESC \
            CHAR_WINDOW_HOLDPOSITION_ATT_TABLE \
            CHAR_WINDOW_OBSTRUCTIONDETECTED_ATT_TABLE \
        	CHAR_WINDOW_SVC_A5_SIGN_TABLE


#define WINDOW_HANDLE_COMMON_NUM     8

#define WINDOW_HANDLE_NUM_CURRENTPOSITION       WINDOW_HANDLE_START_NUM +WINDOW_HANDLE_COMMON_NUM
#define WINDOW_HANDLE_NUM_TARGETPOSITION        WINDOW_HANDLE_NUM_CURRENTPOSITION +WINDOW_CURRENTPOSITION_NO +WINDOW_CURRENTPOSITION_IND_NO
#define WINDOW_HANDLE_NUM_POSITIONSTATE         WINDOW_HANDLE_NUM_TARGETPOSITION +WINDOW_TARGETPOSITION_NO +WINDOW_TARGETPOSITION_IND_NO
#define WINDOW_HANDLE_NUM_HOLDPOSITION   		WINDOW_HANDLE_NUM_POSITIONSTATE +WINDOW_POSITIONSTATE_NO +WINDOW_POSITIONSTATE_IND_NO
#define WINDOW_HANDLE_NUM_OBSTRUCTIONDETECTED   WINDOW_HANDLE_NUM_HOLDPOSITION +WINDOW_HOLDPOSITION_NO +0
#define WINDOW_HANDLE_NUM_THE_END


#define WINDOW_STORE_CHAR_VALUE_T_WINDOWCURRENTPOSITION             u32 WindowCurrentPosition:7;    /* 0~100 */
#define WINDOW_STORE_CHAR_VALUE_T_WINDOWCURRENTPOSITION_STORE           STORE_CHAR(WindowCurrentPosition,hWindowCurrentPosition)
#define WINDOW_STORE_CHAR_VALUE_T_WINDOWCURRENTPOSITION_RESTORE         RESTORE_CHAR(WindowCurrentPosition,hWindowCurrentPosition)

#define WINDOW_STORE_CHAR_VALUE_T_WINDOWTARGETPOSITION              u32 WindowTargetPosition:7;    /* 0~100 */
#define WINDOW_STORE_CHAR_VALUE_T_WINDOWTARGETPOSITION_STORE            STORE_CHAR(WindowTargetPosition,hWindowTargetPosition)
#define WINDOW_STORE_CHAR_VALUE_T_WINDOWTARGETPOSITION_RESTORE          RESTORE_CHAR(WindowTargetPosition,hWindowTargetPosition)

#define WINDOW_STORE_CHAR_VALUE_T_WINDOWPOSITIONSTATE               u32 WindowPositionState:2;  /* 0~2 */
#define WINDOW_STORE_CHAR_VALUE_T_WINDOWPOSITIONSTATE_STORE             STORE_CHAR(WindowPositionState,hWindowPositionState)
#define WINDOW_STORE_CHAR_VALUE_T_WINDOWPOSITIONSTATE_RESTORE           RESTORE_CHAR(WindowPositionState,hWindowPositionState)

#define WINDOW_STORE_CHAR_VALUE_T_WINDOWHOLDPOSITION                u32 WindowHoldPosition:1;   /* 0~1 */
#define WINDOW_STORE_CHAR_VALUE_T_WINDOWHOLDPOSITION_STORE              STORE_CHAR(WindowHoldPosition,hWindowHoldPosition)
#define WINDOW_STORE_CHAR_VALUE_T_WINDOWHOLDPOSITION_RESTORE            RESTORE_CHAR(WindowHoldPosition,hWindowHoldPosition)

#define WINDOW_STORE_CHAR_VALUE_T_WINDOWOBSTRUCTIONDETECTED         u32 WindowObstructionDetected:1;   /* 0~1 */
#define WINDOW_STORE_CHAR_VALUE_T_WINDOWOBSTRUCTIONDETECTED_STORE       STORE_CHAR(WindowObstructionDetected,hWindowObstructionDetected)
#define WINDOW_STORE_CHAR_VALUE_T_WINDOWOBSTRUCTIONDETECTED_RESTORE     RESTORE_CHAR(WindowObstructionDetected,hWindowObstructionDetected)

#define WINDOW_STORE_CHAR_VALUE_T   \
                    WINDOW_STORE_CHAR_VALUE_T_WINDOWCURRENTPOSITION  \
                    WINDOW_STORE_CHAR_VALUE_T_WINDOWTARGETPOSITION   \
                    WINDOW_STORE_CHAR_VALUE_T_WINDOWPOSITIONSTATE  \
                    WINDOW_STORE_CHAR_VALUE_T_WINDOWHOLDPOSITION   \
                    WINDOW_STORE_CHAR_VALUE_T_WINDOWOBSTRUCTIONDETECTED

#define WINDOW_STORE_CHAR_TO_FLASH  \
                    WINDOW_STORE_CHAR_VALUE_T_WINDOWCURRENTPOSITION_STORE    \
                    WINDOW_STORE_CHAR_VALUE_T_WINDOWTARGETPOSITION_STORE \
                    WINDOW_STORE_CHAR_VALUE_T_WINDOWPOSITIONSTATE_STORE    \
                    WINDOW_STORE_CHAR_VALUE_T_WINDOWHOLDPOSITION_STORE \
                    WINDOW_STORE_CHAR_VALUE_T_WINDOWOBSTRUCTIONDETECTED_STORE

#define WINDOW_RESTORE_CHAR_FROM_FLASH  \
                    WINDOW_STORE_CHAR_VALUE_T_WINDOWCURRENTPOSITION_RESTORE  \
                    WINDOW_STORE_CHAR_VALUE_T_WINDOWTARGETPOSITION_RESTORE   \
                    WINDOW_STORE_CHAR_VALUE_T_WINDOWPOSITIONSTATE_RESTORE  \
                    WINDOW_STORE_CHAR_VALUE_T_WINDOWHOLDPOSITION_RESTORE   \
                    WINDOW_STORE_CHAR_VALUE_T_WINDOWOBSTRUCTIONDETECTED_RESTORE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN unsigned char hWindowCurrentPosition;
EXTERN unsigned char hWindowTargetPosition;
EXTERN unsigned char hWindowPositionState;
EXTERN unsigned char hWindowHoldPosition;
EXTERN unsigned char hWindowObstructionDetected;

EXTERN unsigned short ccc_hWindowCurrentPosition;
EXTERN unsigned short ccc_hWindowTargetPosition;
EXTERN unsigned short ccc_hWindowPositionState;
EXTERN unsigned short ccc_hWindowObstructionDetected;

#if WINDOW_CURRENTPOSITION_BRC_ENABLE
    #define BC_PARA_WINDOW_CURRENTPOSITION ((u8*)&bc_para_hWindowCurrentPosition)
    EXTERN unsigned char bc_para_hWindowCurrentPosition;
#else
    #define BC_PARA_WINDOW_CURRENTPOSITION 0
#endif

#if WINDOW_TARGETPOSITION_BRC_ENABLE
    #define BC_PARA_WINDOW_TARGETPOSITION ((u8*)&bc_para_hWindowTargetPosition)
    EXTERN unsigned char bc_para_hWindowTargetPosition;
#else
    #define BC_PARA_WINDOW_TARGETPOSITION 0
#endif

#if WINDOW_POSITIONSTATE_BRC_ENABLE
    #define BC_PARA_WINDOW_POSITIONSTATE ((u8*)&bc_para_hWindowPositionState)
    EXTERN unsigned char bc_para_hWindowPositionState;
#else
    #define BC_PARA_WINDOW_POSITIONSTATE 0
#endif

#if WINDOW_OBSTRUCTIONDETECTED_BRC_ENABLE
    #define BC_PARA_WINDOW_OBSTRUCTIONDETECTED ((u8*)&bc_para_hWindowObstructionDetected)
    EXTERN unsigned char bc_para_hWindowObstructionDetected;
#else
    #define BC_PARA_WINDOW_OBSTRUCTIONDETECTED 0
#endif

#if WINDOW_LINKED_SERVICES_ENABLE
	#define WINDOW_LINKED_SVCS			hWindow_linked_svcs
	#define WINDOW_LINKED_SVCS_LENGTH 	(sizeof(hWindow_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * hWindow_linked_svcs[];
#else
	#define WINDOW_LINKED_SVCS			0
	#define WINDOW_LINKED_SVCS_LENGTH 	0
#endif

EXTERN const u8 hWindowServiceName[13];

EXTERN const hap_characteristic_desc_t hap_desc_char_hWindowCurrentPosition_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hWindowTargetPosition_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hWindowPositionState_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hWindowHoldPosition_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hWindowObstructionDetected_R_S;


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN int hWindowCurrentPositionReadCallback(void *pp);
EXTERN int hWindowTargetPositionReadCallback(void *pp);
EXTERN int hWindowTargetPositionWriteCallback(void *pp);
EXTERN int hWindowPositionStateReadCallback(void *pp);
EXTERN int hWindowHoldPositionWriteCallback(void *pp);
EXTERN int hWindowObstructionDetectedReadCallback(void *pp);
EXTERN int ccc_hWindowCurrentPositionWriteCB(void *pp);
EXTERN int ccc_hWindowTargetPositionWriteCB(void *pp);
EXTERN int ccc_hWindowPositionStateWriteCB(void *pp);
EXTERN int ccc_hWindowObstructionDetectedWriteCB(void *pp);


#endif /* _HK_WINDOW_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
