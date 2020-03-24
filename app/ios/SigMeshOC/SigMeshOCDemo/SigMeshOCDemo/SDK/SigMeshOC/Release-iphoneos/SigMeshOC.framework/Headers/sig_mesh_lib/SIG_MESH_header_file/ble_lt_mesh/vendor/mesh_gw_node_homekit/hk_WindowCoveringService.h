/********************************************************************************************************
 * @file     hk_WindowCoveringService.h
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
#ifndef _HK_WINDOW_COVERING_SERVICE_H_
#define _HK_WINDOW_COVERING_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_WINDOW_COVERING_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif


#define CHAR_WINDOWCOVERING_SVC_SIGN_ENABLE      0
#if CHAR_WINDOWCOVERING_SVC_SIGN_ENABLE
    #define WINDOWCOVERING_AS_PRIMARY_SERVICE    	0
    #define WINDOWCOVERING_IS_HIDDEN_SERVICE     	0
    #define WINDOWCOVERING_LINKED_SERVICES_ENABLE	0
    EXTERN const hap_characteristic_desc_t hap_desc_char_windowcovering_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_windowcovering_A5_R_S;
    EXTERN unsigned char VAR_WINDOWCOVERING_UNKOWN;
#else
    #define WINDOWCOVERING_AS_PRIMARY_SERVICE    	0
    #define WINDOWCOVERING_IS_HIDDEN_SERVICE     	0
    #define WINDOWCOVERING_LINKED_SERVICES_ENABLE	0
#endif

#define CHAR_WINDOWCOVERING_HOLDPOSITION_ENABLE          0
#define CHAR_WINDOWCOVERING_CURRENTHTA_ENABLE            0
#define CHAR_WINDOWCOVERING_TARGETHTA_ENABLE             0
#define CHAR_WINDOWCOVERING_CURRENTVTA_ENABLE            0
#define CHAR_WINDOWCOVERING_TARGETVTA_ENABLE             0
#define CHAR_WINDOWCOVERING_OBSTRUCTIONDETECTED_ENABLE   0

#define WINDOWCOVERING_TARGETPOSITION_IND_ENABLE        0
#define WINDOWCOVERING_CURRENTPOSITION_IND_ENABLE       0
#define WINDOWCOVERING_POSITIONSTATE_IND_ENABLE         0
#define WINDOWCOVERING_CURRENTHTA_IND_ENABLE            0
#define WINDOWCOVERING_TARGETHTA_IND_ENABLE             0
#define WINDOWCOVERING_CURRENTVTA_IND_ENABLE            0
#define WINDOWCOVERING_TARGETVTA_IND_ENABLE             0
#define WINDOWCOVERING_OBSTRUCTIONDETECTED_IND_ENABLE   0

#define WINDOWCOVERING_TARGETPOSITION_BRC_ENABLE        0
#define WINDOWCOVERING_CURRENTPOSITION_BRC_ENABLE       0
#define WINDOWCOVERING_POSITIONSTATE_BRC_ENABLE         0
#define WINDOWCOVERING_CURRENTHTA_BRC_ENABLE            0
#define WINDOWCOVERING_TARGETHTA_BRC_ENABLE             0
#define WINDOWCOVERING_CURRENTVTA_BRC_ENABLE            0
#define WINDOWCOVERING_TARGETVTA_BRC_ENABLE             0
#define WINDOWCOVERING_OBSTRUCTIONDETECTED_BRC_ENABLE   0


#define WC_TARGETPOSITION_NO 3
#if WINDOWCOVERING_TARGETPOSITION_IND_ENABLE
    #define WINDOWCOVERING_TARGETPOSITION_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hWindow_CoveringTargetPosition), ccc_hWindow_CoveringTargetPositionWriteCB, 0},
    #define WINDOWCOVERING_TARGETPOSITION_IND_NO  1
#else
    #define WINDOWCOVERING_TARGETPOSITION_IND_DESC
    #define WINDOWCOVERING_TARGETPOSITION_IND_NO  0
#endif

#define WC_CURRENTPOSITION_NO 3
#if WINDOWCOVERING_CURRENTPOSITION_IND_ENABLE
    #define WINDOWCOVERING_CURRENTPOSITION_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hWindow_CoveringCurrentPosition), ccc_hWindow_CoveringCurrentPositionWriteCB, 0},
    #define WINDOWCOVERING_CURRENTPOSITION_IND_NO  1
#else
    #define WINDOWCOVERING_CURRENTPOSITION_IND_DESC
    #define WINDOWCOVERING_CURRENTPOSITION_IND_NO  0
#endif

#define WC_POSITIONSTATE_NO 3
#if WINDOWCOVERING_POSITIONSTATE_IND_ENABLE
    #define WINDOWCOVERING_POSITIONSTATE_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hWindow_CoveringPositionState), ccc_hWindow_CoveringPositionStateWriteCB, 0},
    #define WINDOWCOVERING_POSITIONSTATE_IND_NO  1
#else
    #define WINDOWCOVERING_POSITIONSTATE_IND_DESC
    #define WINDOWCOVERING_POSITIONSTATE_IND_NO  0
#endif

#if CHAR_WINDOWCOVERING_HOLDPOSITION_ENABLE
    #define WC_HOLDPOSITION_NO 3
    #define CHAR_WINDOWCOVERING_HOLDPOSITION_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_hWindow_CoveringHoldPosition_R_S, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hWindow_CoveringHoldPosition),0, (u8*)(charHoldPositionUUID), (u8*)&hWindow_CoveringHoldPosition, hWindow_CoveringHoldPositionWriteCallback, 0},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + WINDOW_COVERING_INST_NO + 5), 0, 0},
#else
    #define WC_HOLDPOSITION_NO 0
    #define CHAR_WINDOWCOVERING_HOLDPOSITION_ATT_TABLE
#endif


#if CHAR_WINDOWCOVERING_CURRENTHTA_ENABLE
    #define WC_CURRENTHTA_NO 3
    #define CHAR_WINDOWCOVERING_CURRENTHTA_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hWindow_CoveringCurrentHTA_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hWindow_CoveringCurrentHTA),0, (u8*)(charCurrentHTA_UUID), (u8*)&hWindow_CoveringCurrentHTA, 0, hWindow_CoveringCurrentHTAReadCallback},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + WINDOW_COVERING_INST_NO + 6), 0, 0},  \
            WINDOWCOVERING_CURRENTHTA_IND_DESC

	#if WINDOWCOVERING_CURRENTHTA_IND_ENABLE
	    #define WINDOWCOVERING_CURRENTHTA_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hWindow_CoveringCurrentHTA), ccc_hWindow_CoveringCurrentHTAWriteCB, 0},
	    #define WINDOWCOVERING_CURRENTHTA_IND_NO  1
	#else
	    #define WINDOWCOVERING_CURRENTHTA_IND_DESC
	    #define WINDOWCOVERING_CURRENTHTA_IND_NO  0
	#endif
#else
    #define WC_CURRENTHTA_NO 0
    #define CHAR_WINDOWCOVERING_CURRENTHTA_ATT_TABLE
    #define WINDOWCOVERING_CURRENTHTA_IND_NO  0
#endif

#if CHAR_WINDOWCOVERING_TARGETHTA_ENABLE
    #define WC_TARGETHTA_NO 3
    #define CHAR_WINDOWCOVERING_TARGETHTA_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hWindow_CoveringTargetHTA_R_S, 0},   \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hWindow_CoveringTargetHTA),0, (u8*)(charTargetHTA_UUID), (u8*)&hWindow_CoveringTargetHTA, hWindow_CoveringTargetHTAWriteCallback, hWindow_CoveringTargetHTAReadCallback},  \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + WINDOW_COVERING_INST_NO + 7), 0, 0},  \
            WINDOWCOVERING_TARGETHTA_IND_DESC

	#if WINDOWCOVERING_TARGETHTA_IND_ENABLE
	    #define WINDOWCOVERING_TARGETHTA_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hWindow_CoveringTargetHTA), ccc_hWindow_CoveringTargetHTAWriteCB, 0},
	    #define WINDOWCOVERING_TARGETHTA_IND_NO  1
	#else
	    #define WINDOWCOVERING_TARGETHTA_IND_DESC
	    #define WINDOWCOVERING_TARGETHTA_IND_NO  0
	#endif
#else
    #define WC_TARGETHTA_NO 0
    #define CHAR_WINDOWCOVERING_TARGETHTA_ATT_TABLE
    #define WINDOWCOVERING_TARGETHTA_IND_NO  0
#endif

#if CHAR_WINDOWCOVERING_CURRENTVTA_ENABLE
    #define WC_CURRENTVTA_NO 3
    #define CHAR_WINDOWCOVERING_CURRENTVTA_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hWindow_CoveringCurrentVTA_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hWindow_CoveringCurrentVTA),0, (u8*)(charCurrentVTA_UUID), (u8*)&hWindow_CoveringCurrentVTA, 0, hWindow_CoveringCurrentVTAReadCallback},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + WINDOW_COVERING_INST_NO + 8), 0, 0},  \
            WINDOWCOVERING_CURRENTVTA_IND_DESC

	#if WINDOWCOVERING_CURRENTVTA_IND_ENABLE
	    #define WINDOWCOVERING_CURRENTVTA_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hWindow_CoveringCurrentVTA), ccc_hWindow_CoveringCurrentVTAWriteCB, 0},
	    #define WINDOWCOVERING_CURRENTVTA_IND_NO  1
	#else
	    #define WINDOWCOVERING_CURRENTVTA_IND_DESC
	    #define WINDOWCOVERING_CURRENTVTA_IND_NO  0
	#endif
#else
    #define WC_CURRENTVTA_NO 0
    #define CHAR_WINDOWCOVERING_CURRENTVTA_ATT_TABLE
    #define WINDOWCOVERING_CURRENTVTA_IND_NO  0
#endif

#if CHAR_WINDOWCOVERING_TARGETVTA_ENABLE
    #define WC_TARGETVTA_NO 3
    #define CHAR_WINDOWCOVERING_TARGETVTA_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hWindow_CoveringTargetVTA_R_S, 0},   \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hWindow_CoveringTargetVTA),0, (u8*)(charTargetVTA_UUID), (u8*)&hWindow_CoveringTargetVTA, hWindow_CoveringTargetVTAWriteCallback, hWindow_CoveringTargetVTAReadCallback},  \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + WINDOW_COVERING_INST_NO + 9), 0, 0},  \
            WINDOWCOVERING_TARGETVTA_IND_DESC

	#if WINDOWCOVERING_TARGETVTA_IND_ENABLE
	    #define WINDOWCOVERING_TARGETVTA_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hWindow_CoveringTargetVTA), ccc_hWindow_CoveringTargetVTAWriteCB, 0},
	    #define WINDOWCOVERING_TARGETVTA_IND_NO  1
	#else
	    #define WINDOWCOVERING_TARGETVTA_IND_DESC
	    #define WINDOWCOVERING_TARGETVTA_IND_NO  0
	#endif
#else
    #define WC_TARGETVTA_NO 0
    #define CHAR_WINDOWCOVERING_TARGETVTA_ATT_TABLE
    #define WINDOWCOVERING_TARGETVTA_IND_NO  0
#endif

#if CHAR_WINDOWCOVERING_OBSTRUCTIONDETECTED_ENABLE
    #define WC_OBSTRUCTIONDETECTED_NO 3
    #define CHAR_WINDOWCOVERING_OBSTRUCTIONDETECTED_ATT_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hWindow_CoveringObstructionDetected_R_S, 0}, \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hWindow_CoveringObstructionDetected),0, (u8*)(charObstructionDetectedUUID), (u8*)&hWindow_CoveringObstructionDetected, 0, hWindow_CoveringObstructionDetectedReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + WINDOW_COVERING_INST_NO + 10), 0, 0}, \
            WINDOWCOVERING_OBSTRUCTIONDETECTED_IND_DESC

	#if WINDOWCOVERING_OBSTRUCTIONDETECTED_IND_ENABLE
	    #define WINDOWCOVERING_OBSTRUCTIONDETECTED_IND_DESC \
				{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hWindow_CoveringObstructionDetected), ccc_hWindow_CoveringObstructionDetectedWriteCB, 0},
	    #define WINDOWCOVERING_OBSTRUCTIONDETECTED_IND_NO  1
	#else
	    #define WINDOWCOVERING_OBSTRUCTIONDETECTED_IND_DESC
	    #define WINDOWCOVERING_OBSTRUCTIONDETECTED_IND_NO  0
	#endif
#else
    #define WC_OBSTRUCTIONDETECTED_NO 0
    #define CHAR_WINDOWCOVERING_OBSTRUCTIONDETECTED_ATT_TABLE
    #define WINDOWCOVERING_OBSTRUCTIONDETECTED_IND_NO  0
#endif


#if CHAR_WINDOWCOVERING_SVC_SIGN_ENABLE
#define CHAR_WINDOWCOVERING_SVC_A5_SIGN_TABLE   \
    {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_windowcovering_A5_R_S, &hap_desc_service_windowcovering_A5_R_S},  \
    {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_WINDOWCOVERING_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_WINDOWCOVERING_UNKOWN, 0, 0},    \
        {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + WINDOW_COVERING_INST_NO + 11), 0, 0},
#define WINDOWCOVERING_SVC_SIGN_NO   3
#else
#define CHAR_WINDOWCOVERING_SVC_A5_SIGN_TABLE
#define WINDOWCOVERING_SVC_SIGN_NO   0
#endif


#define WINDOW_COVERING_NO_DELTA    WINDOWCOVERING_TARGETPOSITION_IND_NO   \
                                    +WINDOWCOVERING_CURRENTPOSITION_IND_NO  \
                                    +WINDOWCOVERING_POSITIONSTATE_IND_NO    \
                                    +WINDOWCOVERING_CURRENTHTA_IND_NO   \
                                    +WINDOWCOVERING_TARGETHTA_IND_NO    \
                                    +WINDOWCOVERING_CURRENTVTA_IND_NO   \
                                    +WINDOWCOVERING_TARGETVTA_IND_NO    \
                                    +WINDOWCOVERING_OBSTRUCTIONDETECTED_IND_NO

#define WINDOW_COVERING_NO          15 +WC_HOLDPOSITION_NO \
									+WC_CURRENTHTA_NO   \
                                    +WC_TARGETHTA_NO    \
                                    +WC_CURRENTVTA_NO   \
                                    +WC_TARGETVTA_NO    \
                                    +WC_OBSTRUCTIONDETECTED_NO \
									+WINDOWCOVERING_SVC_SIGN_NO
									
#define WINDOW_COVERING_HANDLE_NUM (WINDOW_COVERING_NO + WINDOW_COVERING_NO_DELTA)


#define HOMEKIT_WINDOW_COVERING_SERVICE_ATT_TABLE   \
    {WINDOW_COVERING_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(window_coveringServiceUUID),sizeof(window_coveringServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(window_coveringServiceUUID), 0, 0},   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(insts + WINDOW_COVERING_INST_NO), 0, 0},   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_NAME, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(hWindow_CoveringServiceName), 0, (u8*)(nameUUID), (u8*)(hWindow_CoveringServiceName), 0, 0},  \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + WINDOW_COVERING_INST_NO + 1), 0, 0}, \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hWindow_CoveringTargetPosition_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hWindow_CoveringTargetPosition),0, (u8*)(charTargetPositionUUID), (u8*)&hWindow_CoveringTargetPosition, hWindow_CoveringTargetPositionWriteCallback, hWindow_CoveringTargetPositionReadCallback},  \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + WINDOW_COVERING_INST_NO + 2), 0, 0},  \
            WINDOWCOVERING_TARGETPOSITION_IND_DESC \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hWindow_CoveringCurrentPosition_R_S, 0}, \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hWindow_CoveringCurrentPosition),0, (u8*)(charCurrentPositionUUID), (u8*)&hWindow_CoveringCurrentPosition, 0, hWindow_CoveringCurrentPositionReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + WINDOW_COVERING_INST_NO + 3), 0, 0},  \
            WINDOWCOVERING_CURRENTPOSITION_IND_DESC \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_hWindow_CoveringPositionState_R_S, 0},   \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hWindow_CoveringPositionState),0, (u8*)(charPositionStateUUID), (u8*)&hWindow_CoveringPositionState, 0, hWindow_CoveringPositionStateReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + WINDOW_COVERING_INST_NO + 4), 0, 0},  \
            WINDOWCOVERING_POSITIONSTATE_IND_DESC \
            CHAR_WINDOWCOVERING_HOLDPOSITION_ATT_TABLE \
            CHAR_WINDOWCOVERING_CURRENTHTA_ATT_TABLE \
            CHAR_WINDOWCOVERING_TARGETHTA_ATT_TABLE \
            CHAR_WINDOWCOVERING_CURRENTVTA_ATT_TABLE \
            CHAR_WINDOWCOVERING_TARGETVTA_ATT_TABLE \
            CHAR_WINDOWCOVERING_OBSTRUCTIONDETECTED_ATT_TABLE \
        	CHAR_WINDOWCOVERING_SVC_A5_SIGN_TABLE


#define WINDOWCOVERING_HANDLE_COMMON_NUM     8

#define WINDOWCOVERING_HANDLE_NUM_TARGETPOSITION        WINDOW_COVERING_HANDLE_START_NUM +WINDOWCOVERING_HANDLE_COMMON_NUM
#define WINDOWCOVERING_HANDLE_NUM_CURRENTPOSITION       WINDOWCOVERING_HANDLE_NUM_TARGETPOSITION +WC_TARGETPOSITION_NO +WINDOWCOVERING_TARGETPOSITION_IND_NO
#define WINDOWCOVERING_HANDLE_NUM_POSITIONSTATE         WINDOWCOVERING_HANDLE_NUM_CURRENTPOSITION +WC_CURRENTPOSITION_NO +WINDOWCOVERING_CURRENTPOSITION_IND_NO
#define WINDOWCOVERING_HANDLE_NUM_HOLDPOSITION          WINDOWCOVERING_HANDLE_NUM_POSITIONSTATE +WC_POSITIONSTATE_NO +WINDOWCOVERING_POSITIONSTATE_IND_NO
#define WINDOWCOVERING_HANDLE_NUM_CURRENTHTA            WINDOWCOVERING_HANDLE_NUM_HOLDPOSITION +WC_HOLDPOSITION_NO +0
#define WINDOWCOVERING_HANDLE_NUM_TARGETHTA             WINDOWCOVERING_HANDLE_NUM_CURRENTHTA +WC_CURRENTHTA_NO +WINDOWCOVERING_CURRENTHTA_IND_NO
#define WINDOWCOVERING_HANDLE_NUM_CURRENTVTA            WINDOWCOVERING_HANDLE_NUM_TARGETHTA +WC_TARGETHTA_NO +WINDOWCOVERING_TARGETHTA_IND_NO
#define WINDOWCOVERING_HANDLE_NUM_TARGETVTA             WINDOWCOVERING_HANDLE_NUM_CURRENTVTA +WC_CURRENTVTA_NO +WINDOWCOVERING_CURRENTVTA_IND_NO
#define WINDOWCOVERING_HANDLE_NUM_OBSTRUCTIONDETECTED   WINDOWCOVERING_HANDLE_NUM_TARGETVTA +WC_TARGETVTA_NO +WINDOWCOVERING_TARGETVTA_IND_NO
#define WINDOWCOVERING_HANDLE_NUM_THE_END


#define WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGTARGETPOSITION                u32 Window_CoveringTargetPosition:7;    /* 0~100 */
#define WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGTARGETPOSITION_STORE              STORE_CHAR(Window_CoveringTargetPosition,hWindow_CoveringTargetPosition)
#define WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGTARGETPOSITION_RESTORE            RESTORE_CHAR(Window_CoveringTargetPosition,hWindow_CoveringTargetPosition)

#define WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGCURRENTPOSITION               u32 Window_CoveringCurrentPosition:7;    /* 0~100 */
#define WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGCURRENTPOSITION_STORE             STORE_CHAR(Window_CoveringCurrentPosition,hWindow_CoveringCurrentPosition)
#define WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGCURRENTPOSITION_RESTORE           RESTORE_CHAR(Window_CoveringCurrentPosition,hWindow_CoveringCurrentPosition)

#define WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGPOSITIONSTATE                 u32 Window_CoveringPositionState:2;    /* 0~2 */
#define WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGPOSITIONSTATE_STORE               STORE_CHAR(Window_CoveringPositionState,hWindow_CoveringPositionState)
#define WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGPOSITIONSTATE_RESTORE             RESTORE_CHAR(Window_CoveringPositionState,hWindow_CoveringPositionState)

#define WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGHOLDPOSITION                  u32 Window_CoveringHoldPosition:1;    /* 0~1 */
#define WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGHOLDPOSITION_STORE                STORE_CHAR(Window_CoveringHoldPosition,hWindow_CoveringHoldPosition)
#define WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGHOLDPOSITION_RESTORE              RESTORE_CHAR(Window_CoveringHoldPosition,hWindow_CoveringHoldPosition)

#define WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGCURRENTHTA                    u32 Window_CoveringCurrentHTA;  /* int */
#define WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGCURRENTHTA_STORE                  STORE_CHAR(Window_CoveringCurrentHTA,hWindow_CoveringCurrentHTA)
#define WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGCURRENTHTA_RESTORE                RESTORE_CHAR(Window_CoveringCurrentHTA,hWindow_CoveringCurrentHTA)

#define WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGTARGETHTA                     u32 Window_CoveringTargetHTA;  /* int */
#define WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGTARGETHTA_STORE                   STORE_CHAR(Window_CoveringTargetHTA,hWindow_CoveringTargetHTA)
#define WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGTARGETHTA_RESTORE                 RESTORE_CHAR(Window_CoveringTargetHTA,hWindow_CoveringTargetHTA)

#define WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGCURRENTVTA                    u32 Window_CoveringCurrentVTA;  /* int */
#define WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGCURRENTVTA_STORE                  STORE_CHAR(Window_CoveringCurrentVTA,hWindow_CoveringCurrentVTA)
#define WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGCURRENTVTA_RESTORE                RESTORE_CHAR(Window_CoveringCurrentVTA,hWindow_CoveringCurrentVTA)

#define WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGTARGETVTA                     u32 Window_CoveringTargetVTA;  /* int */
#define WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGTARGETVTA_STORE                   STORE_CHAR(Window_CoveringTargetVTA,hWindow_CoveringTargetVTA)
#define WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGTARGETVTA_RESTORE                 RESTORE_CHAR(Window_CoveringTargetVTA,hWindow_CoveringTargetVTA)

#define WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGOBSTRUCTIONDETECTED           u32 Window_CoveringObstructionDetected:1;    /* 0~1 */
#define WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGOBSTRUCTIONDETECTED_STORE         STORE_CHAR(Window_CoveringObstructionDetected,hWindow_CoveringObstructionDetected)
#define WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGOBSTRUCTIONDETECTED_RESTORE       RESTORE_CHAR(Window_CoveringObstructionDetected,hWindow_CoveringObstructionDetected)

#define WINDOW_COVERING_STORE_CHAR_VALUE_T   \
                    WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGTARGETPOSITION  \
                    WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGCURRENTPOSITION   \
                    WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGPOSITIONSTATE  \
                    WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGHOLDPOSITION   \
                    WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGCURRENTHTA  \
                    WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGTARGETHTA   \
                    WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGCURRENTVTA  \
                    WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGTARGETVTA   \
                    WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGOBSTRUCTIONDETECTED

#define WINDOW_COVERING_STORE_CHAR_TO_FLASH  \
                    WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGTARGETPOSITION_STORE    \
                    WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGCURRENTPOSITION_STORE \
                    WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGPOSITIONSTATE_STORE    \
                    WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGHOLDPOSITION_STORE \
                    WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGCURRENTHTA_STORE    \
                    WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGTARGETHTA_STORE \
                    WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGCURRENTVTA_STORE    \
                    WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGTARGETVTA_STORE \
                    WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGOBSTRUCTIONDETECTED_STORE

#define WINDOW_COVERING_RESTORE_CHAR_FROM_FLASH  \
                    WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGTARGETPOSITION_RESTORE  \
                    WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGCURRENTPOSITION_RESTORE   \
                    WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGPOSITIONSTATE_RESTORE  \
                    WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGHOLDPOSITION_RESTORE   \
                    WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGCURRENTHTA_RESTORE  \
                    WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGTARGETHTA_RESTORE   \
                    WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGCURRENTVTA_RESTORE  \
                    WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGTARGETVTA_RESTORE   \
                    WINDOW_COVERING_STORE_CHAR_VALUE_T_WINDOW_COVERINGOBSTRUCTIONDETECTED_RESTORE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN unsigned char hWindow_CoveringTargetPosition;
EXTERN unsigned char hWindow_CoveringCurrentPosition;
EXTERN unsigned char hWindow_CoveringPositionState;
EXTERN unsigned char hWindow_CoveringHoldPosition;
EXTERN int hWindow_CoveringCurrentHTA;
EXTERN int hWindow_CoveringTargetHTA;
EXTERN int hWindow_CoveringCurrentVTA;
EXTERN int hWindow_CoveringTargetVTA;
EXTERN unsigned char hWindow_CoveringObstructionDetected;

EXTERN unsigned short ccc_hWindow_CoveringTargetPosition;
EXTERN unsigned short ccc_hWindow_CoveringCurrentPosition;
EXTERN unsigned short ccc_hWindow_CoveringPositionState;
EXTERN unsigned short ccc_hWindow_CoveringCurrentHTA;
EXTERN unsigned short ccc_hWindow_CoveringTargetHTA;
EXTERN unsigned short ccc_hWindow_CoveringCurrentVTA;
EXTERN unsigned short ccc_hWindow_CoveringTargetVTA;
EXTERN unsigned short ccc_hWindow_CoveringObstructionDetected;

#if WINDOWCOVERING_TARGETPOSITION_BRC_ENABLE
    #define BC_PARA_WINDOWCOVERING_TARGETPOSITION ((u8*)&bc_para_hWindow_CoveringTargetPosition)
    EXTERN unsigned char bc_para_hWindow_CoveringTargetPosition;
#else
    #define BC_PARA_WINDOWCOVERING_TARGETPOSITION 0
#endif

#if WINDOWCOVERING_CURRENTPOSITION_BRC_ENABLE
    #define BC_PARA_WINDOWCOVERING_CURRENTPOSITION ((u8*)&bc_para_hWindow_CoveringCurrentPosition)
    EXTERN unsigned char bc_para_hWindow_CoveringCurrentPosition;
#else
    #define BC_PARA_WINDOWCOVERING_CURRENTPOSITION 0
#endif

#if WINDOWCOVERING_POSITIONSTATE_BRC_ENABLE
    #define BC_PARA_WINDOWCOVERING_POSITIONSTATE ((u8*)&bc_para_hWindow_CoveringPositionState)
    EXTERN unsigned char bc_para_hWindow_CoveringPositionState;
#else
    #define BC_PARA_WINDOWCOVERING_POSITIONSTATE 0
#endif

#if WINDOWCOVERING_CURRENTHTA_BRC_ENABLE
    #define BC_PARA_WINDOWCOVERING_CURRENTHTA ((u8*)&bc_para_hWindow_CoveringCurrentHTA)
    EXTERN unsigned char bc_para_hWindow_CoveringCurrentHTA;
#else
    #define BC_PARA_WINDOWCOVERING_CURRENTHTA 0
#endif

#if WINDOWCOVERING_TARGETHTA_BRC_ENABLE
    #define BC_PARA_WINDOWCOVERING_TARGETHTA ((u8*)&bc_para_hWindow_CoveringTargetHTA)
    EXTERN unsigned char bc_para_hWindow_CoveringTargetHTA;
#else
    #define BC_PARA_WINDOWCOVERING_TARGETHTA 0
#endif

#if WINDOWCOVERING_CURRENTVTA_BRC_ENABLE
    #define BC_PARA_WINDOWCOVERING_CURRENTVTA ((u8*)&bc_para_hWindow_CoveringCurrentVTA)
    EXTERN unsigned char bc_para_hWindow_CoveringCurrentVTA;
#else
    #define BC_PARA_WINDOWCOVERING_CURRENTVTA 0
#endif

#if WINDOWCOVERING_TARGETVTA_BRC_ENABLE
    #define BC_PARA_WINDOWCOVERING_TARGETVTA ((u8*)&bc_para_hWindow_CoveringTargetVTA)
    EXTERN unsigned char bc_para_hWindow_CoveringTargetVTA;
#else
    #define BC_PARA_WINDOWCOVERING_TARGETVTA 0
#endif

#if WINDOWCOVERING_OBSTRUCTIONDETECTED_BRC_ENABLE
    #define BC_PARA_WINDOWCOVERING_OBSTRUCTIONDETECTED ((u8*)&bc_para_hWindow_CoveringObstructionDetected)
    EXTERN unsigned char bc_para_hWindow_CoveringObstructionDetected;
#else
    #define BC_PARA_WINDOWCOVERING_OBSTRUCTIONDETECTED 0
#endif

#if WINDOWCOVERING_LINKED_SERVICES_ENABLE
	#define WINDOWCOVERING_LINKED_SVCS			hWindowCovering_linked_svcs
	#define WINDOWCOVERING_LINKED_SVCS_LENGTH 	(sizeof(hWindowCovering_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * hWindowCovering_linked_svcs[];
#else
	#define WINDOWCOVERING_LINKED_SVCS			0
	#define WINDOWCOVERING_LINKED_SVCS_LENGTH 	0
#endif

EXTERN const u8 hWindow_CoveringServiceName[22];

EXTERN const hap_characteristic_desc_t hap_desc_char_hWindow_CoveringTargetPosition_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hWindow_CoveringCurrentPosition_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hWindow_CoveringPositionState_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hWindow_CoveringHoldPosition_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hWindow_CoveringCurrentHTA_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hWindow_CoveringTargetHTA_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hWindow_CoveringCurrentVTA_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hWindow_CoveringTargetVTA_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_hWindow_CoveringObstructionDetected_R_S;


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN int hWindow_CoveringTargetPositionReadCallback(void *pp);
EXTERN int hWindow_CoveringTargetPositionWriteCallback(void *pp);
EXTERN int hWindow_CoveringCurrentPositionReadCallback(void *pp);
EXTERN int hWindow_CoveringPositionStateReadCallback(void *pp);
EXTERN int hWindow_CoveringHoldPositionWriteCallback(void *pp);
EXTERN int hWindow_CoveringCurrentHTAReadCallback(void *pp);
EXTERN int hWindow_CoveringTargetHTAReadCallback(void *pp);
EXTERN int hWindow_CoveringTargetHTAWriteCallback(void *pp);
EXTERN int hWindow_CoveringCurrentVTAReadCallback(void *pp);
EXTERN int hWindow_CoveringTargetVTAReadCallback(void *pp);
EXTERN int hWindow_CoveringTargetVTAWriteCallback(void *pp);
EXTERN int hWindow_CoveringObstructionDetectedReadCallback(void *pp);
EXTERN int ccc_hWindow_CoveringTargetPositionWriteCB(void *pp);
EXTERN int ccc_hWindow_CoveringCurrentPositionWriteCB(void *pp);
EXTERN int ccc_hWindow_CoveringPositionStateWriteCB(void *pp);
EXTERN int ccc_hWindow_CoveringCurrentHTAWriteCB(void *pp);
EXTERN int ccc_hWindow_CoveringTargetHTAWriteCB(void *pp);
EXTERN int ccc_hWindow_CoveringCurrentVTAWriteCB(void *pp);
EXTERN int ccc_hWindow_CoveringTargetVTAWriteCB(void *pp);
EXTERN int ccc_hWindow_CoveringObstructionDetectedWriteCB(void *pp);


#endif /* _HK_WINDOW_COVERING_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
