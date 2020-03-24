/********************************************************************************************************
 * @file     hk_DoorService.h
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
#ifndef _HK_DOOR_SERVICE_H_
#define _HK_DOOR_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_DOOR_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif


#define CHAR_DOOR_SVC_SIGN_ENABLE      0
#if CHAR_DOOR_SVC_SIGN_ENABLE
    #define DOOR_AS_PRIMARY_SERVICE    		0
    #define DOOR_IS_HIDDEN_SERVICE     		0
    #define DOOR_LINKED_SERVICES_ENABLE		0
    EXTERN const hap_characteristic_desc_t hap_desc_char_door_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_door_A5_R_S;
    EXTERN unsigned char VAR_DOOR_UNKOWN;
#else
    #define DOOR_AS_PRIMARY_SERVICE    		0
    #define DOOR_IS_HIDDEN_SERVICE     		0
    #define DOOR_LINKED_SERVICES_ENABLE		0
#endif


#define CHAR_DOOR_HOLDPOSITION_ENABLE			0
#define CHAR_DOOR_OBSTRUCTIONDETECTED_ENABLE	0

#define DOOR_CURRENTPOSITION_IND_ENABLE     0
#define DOOR_TARGETPOSITION_IND_ENABLE      0
#define DOOR_POSITIONSTATE_IND_ENABLE       0
#define DOOR_OBSTRUCTIONDETECTED_IND_ENABLE 0

#define DOOR_CURRENTPOSITION_BRC_ENABLE     0
#define DOOR_TARGETPOSITION_BRC_ENABLE      0
#define DOOR_POSITIONSTATE_BRC_ENABLE       0
#define DOOR_OBSTRUCTIONDETECTED_BRC_ENABLE 0


#define DOOR_CURRENTPOSITION_NO 3
#if DOOR_CURRENTPOSITION_IND_ENABLE
    #define DOOR_CURRENTPOSITION_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hDoorCurrentPosition), ccc_hDoorCurrentPositionWriteCB, 0},
    #define DOOR_CURRENTPOSITION_IND_NO  1
#else
    #define DOOR_CURRENTPOSITION_IND_DESC
    #define DOOR_CURRENTPOSITION_IND_NO  0
#endif

#define DOOR_TARGETPOSITION_NO 3
#if DOOR_TARGETPOSITION_IND_ENABLE
    #define DOOR_TARGETPOSITION_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hDoorTargetPosition), ccc_hDoorTargetPositionWriteCB, 0},
    #define DOOR_TARGETPOSITION_IND_NO  1
#else
    #define DOOR_TARGETPOSITION_IND_DESC
    #define DOOR_TARGETPOSITION_IND_NO  0
#endif

#define DOOR_POSITIONSTATE_NO 3
#if DOOR_POSITIONSTATE_IND_ENABLE
    #define DOOR_POSITIONSTATE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hDoorPositionState), ccc_hDoorPositionStateWriteCB, 0},
    #define DOOR_POSITIONSTATE_IND_NO  1
#else
    #define DOOR_POSITIONSTATE_IND_DESC
    #define DOOR_POSITIONSTATE_IND_NO  0
#endif


#if CHAR_DOOR_HOLDPOSITION_ENABLE
    #define DOOR_HOLDPOSITION_NO 3
    #define CHAR_DOOR_HOLDPOSITION_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_hDoorHoldPosition_R_S, 0},   \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hDoorHoldPosition),0, (u8*)(charHoldPositionUUID), (u8*)&hDoorHoldPosition, hDoorHoldPositionWriteCallback, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + DOOR_INST_NO + 5), 0, 0},
#else
    #define DOOR_HOLDPOSITION_NO 0
    #define CHAR_DOOR_HOLDPOSITION_ATT_TABLE
#endif

#if CHAR_DOOR_OBSTRUCTIONDETECTED_ENABLE
    #define DOOR_OBSTRUCTIONDETECTED_NO 3
    #define CHAR_DOOR_OBSTRUCTIONDETECTED_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hDoorObstructionDetected_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hDoorObstructionDetected),0, (u8*)(charObstructionDetectedUUID), (u8*)&hDoorObstructionDetected, 0, hDoorObstructionDetectedReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + DOOR_INST_NO + 6), 0, 0}, \
            DOOR_OBSTRUCTIONDETECTED_IND_DESC

	#if DOOR_OBSTRUCTIONDETECTED_IND_ENABLE
	    #define DOOR_OBSTRUCTIONDETECTED_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hDoorObstructionDetected), ccc_hDoorObstructionDetectedWriteCB, 0},
	    #define DOOR_OBSTRUCTIONDETECTED_IND_NO  1
	#else
	    #define DOOR_OBSTRUCTIONDETECTED_IND_DESC
	    #define DOOR_OBSTRUCTIONDETECTED_IND_NO  0
	#endif
#else
    #define DOOR_OBSTRUCTIONDETECTED_NO 0
    #define CHAR_DOOR_OBSTRUCTIONDETECTED_ATT_TABLE
    #define DOOR_OBSTRUCTIONDETECTED_IND_NO  0
#endif


#if CHAR_DOOR_SVC_SIGN_ENABLE
#define CHAR_DOOR_SVC_A5_SIGN_TABLE   \
    {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_door_A5_R_S, &hap_desc_service_door_A5_R_S},  \
    {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_DOOR_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_DOOR_UNKOWN, 0, 0},    \
        {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + DOOR_INST_NO + 7), 0, 0},
#define DOOR_SVC_SIGN_NO   3
#else
#define CHAR_DOOR_SVC_A5_SIGN_TABLE
#define DOOR_SVC_SIGN_NO   0
#endif


#define DOOR_NO_DELTA   DOOR_CURRENTPOSITION_IND_NO \
                        +DOOR_TARGETPOSITION_IND_NO \
                        +DOOR_POSITIONSTATE_IND_NO  \
                        +DOOR_OBSTRUCTIONDETECTED_IND_NO

#define DOOR_NO         15 +DOOR_HOLDPOSITION_NO \
						+DOOR_OBSTRUCTIONDETECTED_NO \
						+DOOR_SVC_SIGN_NO
						
#define DOOR_HANDLE_NUM (DOOR_NO + DOOR_NO_DELTA)

#define HOMEKIT_DOOR_SERVICE_ATT_TABLE  \
    {DOOR_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(doorServiceUUID),sizeof(doorServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(doorServiceUUID), 0, 0},  \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(insts + DOOR_INST_NO), 0, 0},  \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_NAME, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(hDoorServiceName), 0, (u8*)(nameUUID), (u8*)(hDoorServiceName), 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + DOOR_INST_NO + 1), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hDoorCurrentPosition_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hDoorCurrentPosition),0, (u8*)(charCurrentPositionUUID), (u8*)&hDoorCurrentPosition, 0, hDoorCurrentPositionReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + DOOR_INST_NO + 2), 0, 0}, \
            DOOR_CURRENTPOSITION_IND_DESC \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hDoorTargetPosition_R_S, 0}, \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hDoorTargetPosition),0, (u8*)(charTargetPositionUUID), (u8*)&hDoorTargetPosition, hDoorTargetPositionWriteCallback, hDoorTargetPositionReadCallback},  \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + DOOR_INST_NO + 3), 0, 0}, \
            DOOR_TARGETPOSITION_IND_DESC \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hDoorPositionState_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hDoorPositionState),0, (u8*)(charPositionStateUUID), (u8*)&hDoorPositionState, 0, hDoorPositionStateReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + DOOR_INST_NO + 4), 0, 0}, \
            DOOR_POSITIONSTATE_IND_DESC \
            CHAR_DOOR_HOLDPOSITION_ATT_TABLE	\
            CHAR_DOOR_OBSTRUCTIONDETECTED_ATT_TABLE \
        	CHAR_DOOR_SVC_A5_SIGN_TABLE


#define DOOR_HANDLE_COMMON_NUM     8

#define DOOR_HANDLE_NUM_CURRENTPOSITION     DOOR_HANDLE_START_NUM +DOOR_HANDLE_COMMON_NUM
#define DOOR_HANDLE_NUM_TARGETPOSITION      DOOR_HANDLE_NUM_CURRENTPOSITION +DOOR_CURRENTPOSITION_NO +DOOR_CURRENTPOSITION_IND_NO
#define DOOR_HANDLE_NUM_POSITIONSTATE       DOOR_HANDLE_NUM_TARGETPOSITION +DOOR_TARGETPOSITION_NO +DOOR_TARGETPOSITION_IND_NO
#define DOOR_HANDLE_NUM_HOLDPOSITION		DOOR_HANDLE_NUM_POSITIONSTATE +DOOR_POSITIONSTATE_NO +DOOR_POSITIONSTATE_IND_NO
#define DOOR_HANDLE_NUM_OBSTRUCTIONDETECTED DOOR_HANDLE_NUM_HOLDPOSITION +DOOR_HOLDPOSITION_NO +0
#define DOOR_HANDLE_NUM_THE_END


#define DOOR_STORE_CHAR_VALUE_T_DOORCURRENTPOSITION            u32 DoorCurrentPosition:7;   /* 0~100 */
#define DOOR_STORE_CHAR_VALUE_T_DOORCURRENTPOSITION_STORE        STORE_CHAR(DoorCurrentPosition,hDoorCurrentPosition)
#define DOOR_STORE_CHAR_VALUE_T_DOORCURRENTPOSITION_RESTORE      RESTORE_CHAR(DoorCurrentPosition,hDoorCurrentPosition)

#define DOOR_STORE_CHAR_VALUE_T_DOORTARGETPOSITION             u32 DoorTargetPosition:7;   /* 0~100 */
#define DOOR_STORE_CHAR_VALUE_T_DOORTARGETPOSITION_STORE         STORE_CHAR(DoorTargetPosition,hDoorTargetPosition)
#define DOOR_STORE_CHAR_VALUE_T_DOORTARGETPOSITION_RESTORE       RESTORE_CHAR(DoorTargetPosition,hDoorTargetPosition)

#define DOOR_STORE_CHAR_VALUE_T_DOORPOSITIONSTATE              u32 DoorPositionState:2; /* 0~2 */
#define DOOR_STORE_CHAR_VALUE_T_DOORPOSITIONSTATE_STORE          STORE_CHAR(DoorPositionState,hDoorPositionState)
#define DOOR_STORE_CHAR_VALUE_T_DOORPOSITIONSTATE_RESTORE        RESTORE_CHAR(DoorPositionState,hDoorPositionState)

#define DOOR_STORE_CHAR_VALUE_T_DOORHOLDPOSITION               u32 DoorHoldPosition:1;  /* 0~1 */
#define DOOR_STORE_CHAR_VALUE_T_DOORHOLDPOSITION_STORE           STORE_CHAR(DoorHoldPosition,hDoorHoldPosition)
#define DOOR_STORE_CHAR_VALUE_T_DOORHOLDPOSITION_RESTORE         RESTORE_CHAR(DoorHoldPosition,hDoorHoldPosition)

#define DOOR_STORE_CHAR_VALUE_T_DOOROBSTRUCTIONDETECTED        u32 DoorObstructionDetected:1;  /* 0~1 */
#define DOOR_STORE_CHAR_VALUE_T_DOOROBSTRUCTIONDETECTED_STORE    STORE_CHAR(DoorObstructionDetected,hDoorObstructionDetected)
#define DOOR_STORE_CHAR_VALUE_T_DOOROBSTRUCTIONDETECTED_RESTORE  RESTORE_CHAR(DoorObstructionDetected,hDoorObstructionDetected)

#define DOOR_STORE_CHAR_VALUE_T   \
                    DOOR_STORE_CHAR_VALUE_T_DOORCURRENTPOSITION  \
                    DOOR_STORE_CHAR_VALUE_T_DOORTARGETPOSITION   \
                    DOOR_STORE_CHAR_VALUE_T_DOORPOSITIONSTATE  \
                    DOOR_STORE_CHAR_VALUE_T_DOORHOLDPOSITION   \
                    DOOR_STORE_CHAR_VALUE_T_DOOROBSTRUCTIONDETECTED

#define DOOR_STORE_CHAR_TO_FLASH  \
                    DOOR_STORE_CHAR_VALUE_T_DOORCURRENTPOSITION_STORE    \
                    DOOR_STORE_CHAR_VALUE_T_DOORTARGETPOSITION_STORE \
                    DOOR_STORE_CHAR_VALUE_T_DOORPOSITIONSTATE_STORE    \
                    DOOR_STORE_CHAR_VALUE_T_DOORHOLDPOSITION_STORE \
                    DOOR_STORE_CHAR_VALUE_T_DOOROBSTRUCTIONDETECTED_STORE

#define DOOR_RESTORE_CHAR_FROM_FLASH  \
                    DOOR_STORE_CHAR_VALUE_T_DOORCURRENTPOSITION_RESTORE  \
                    DOOR_STORE_CHAR_VALUE_T_DOORTARGETPOSITION_RESTORE   \
                    DOOR_STORE_CHAR_VALUE_T_DOORPOSITIONSTATE_RESTORE  \
                    DOOR_STORE_CHAR_VALUE_T_DOORHOLDPOSITION_RESTORE   \
                    DOOR_STORE_CHAR_VALUE_T_DOOROBSTRUCTIONDETECTED_RESTORE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN unsigned char hDoorCurrentPosition;
EXTERN unsigned char hDoorTargetPosition;
EXTERN unsigned char hDoorPositionState;
EXTERN unsigned char hDoorHoldPosition;
EXTERN unsigned char hDoorObstructionDetected;

EXTERN unsigned short ccc_hDoorCurrentPosition;
EXTERN unsigned short ccc_hDoorTargetPosition;
EXTERN unsigned short ccc_hDoorPositionState;
EXTERN unsigned short ccc_hDoorObstructionDetected;

#if DOOR_CURRENTPOSITION_BRC_ENABLE
    #define BC_PARA_DOOR_CURRENTPOSITION ((u8*)&bc_para_hDoorCurrentPosition)
    EXTERN unsigned char bc_para_hDoorCurrentPosition;
#else
    #define BC_PARA_DOOR_CURRENTPOSITION 0
#endif

#if DOOR_TARGETPOSITION_BRC_ENABLE
    #define BC_PARA_DOOR_TARGETPOSITION ((u8*)&bc_para_hDoorTargetPosition)
    EXTERN unsigned char bc_para_hDoorTargetPosition;
#else
    #define BC_PARA_DOOR_TARGETPOSITION 0
#endif

#if DOOR_POSITIONSTATE_BRC_ENABLE
    #define BC_PARA_DOOR_POSITIONSTATE ((u8*)&bc_para_hDoorPositionState)
    EXTERN unsigned char bc_para_hDoorPositionState;
#else
    #define BC_PARA_DOOR_POSITIONSTATE 0
#endif

#if DOOR_OBSTRUCTIONDETECTED_BRC_ENABLE
    #define BC_PARA_DOOR_OBSTRUCTIONDETECTED ((u8*)&bc_para_hDoorObstructionDetected)
    EXTERN unsigned char bc_para_hDoorObstructionDetected;
#else
    #define BC_PARA_DOOR_OBSTRUCTIONDETECTED 0
#endif

#if DOOR_LINKED_SERVICES_ENABLE
	#define DOOR_LINKED_SVCS			hDoor_linked_svcs
	#define DOOR_LINKED_SVCS_LENGTH 	(sizeof(hDoor_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * hDoor_linked_svcs[];
#else
	#define DOOR_LINKED_SVCS			0
	#define DOOR_LINKED_SVCS_LENGTH 	0
#endif

EXTERN const u8 hDoorServiceName[11];

EXTERN const hap_characteristic_desc_t hap_desc_char_hDoorCurrentPosition_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hDoorTargetPosition_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hDoorPositionState_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hDoorHoldPosition_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hDoorObstructionDetected_R_S;


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN int hDoorCurrentPositionReadCallback(void *pp);
EXTERN int hDoorTargetPositionReadCallback(void *pp);
EXTERN int hDoorTargetPositionWriteCallback(void *pp);
EXTERN int hDoorPositionStateReadCallback(void *pp);
EXTERN int hDoorHoldPositionWriteCallback(void *pp);
EXTERN int hDoorObstructionDetectedReadCallback(void *pp);
EXTERN int ccc_hDoorCurrentPositionWriteCB(void *pp);
EXTERN int ccc_hDoorTargetPositionWriteCB(void *pp);
EXTERN int ccc_hDoorPositionStateWriteCB(void *pp);
EXTERN int ccc_hDoorObstructionDetectedWriteCB(void *pp);

#endif /* _HK_DOOR_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
