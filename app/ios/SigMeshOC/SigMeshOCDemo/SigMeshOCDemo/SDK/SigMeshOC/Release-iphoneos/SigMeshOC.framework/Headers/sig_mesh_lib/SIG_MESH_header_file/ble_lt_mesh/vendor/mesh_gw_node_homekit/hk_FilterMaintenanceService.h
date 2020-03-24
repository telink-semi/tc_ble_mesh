/********************************************************************************************************
 * @file     hk_FilterMaintenanceService.h
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
#ifndef _HK_FILTER_MAINTENANCE_SERVICE_H_
#define _HK_FILTER_MAINTENANCE_SERVICE_H_

#include "homekit_src/homekit_inc.h"

/* ------------------------------------
    Header Files
   ------------------------------------ */


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_FILTER_MAINTENANCE_SERVICE_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif

#define CHAR_FILTER_MAINTENANCE_SVC_SIGN_ENABLE      0
#if CHAR_FILTER_MAINTENANCE_SVC_SIGN_ENABLE
    #define FILTER_MAINTENANCE_AS_PRIMARY_SERVICE    	0
    #define FILTER_MAINTENANCE_IS_HIDDEN_SERVICE     	0
    #define FILTER_MAINTENANCE_LINKED_SERVICES_ENABLE	0
    EXTERN const hap_characteristic_desc_t hap_desc_char_filter_maintenance_A5_R_S;
    EXTERN const hap_service_desc_t hap_desc_service_filter_maintenance_A5_R_S;
    EXTERN unsigned char VAR_FILTER_MAINTENANCE_UNKOWN;
#else
    #define FILTER_MAINTENANCE_AS_PRIMARY_SERVICE    	0
    #define FILTER_MAINTENANCE_IS_HIDDEN_SERVICE     	0
    #define FILTER_MAINTENANCE_LINKED_SERVICES_ENABLE	0
#endif

#define CHAR_FILTER_MAINTENANCE_FILTER_LIFE_LEVEL_ENABLE			0
#define CHAR_FILTER_MAINTENANCE_RESET_FILTER_INDICATION_ENABLE		0

#define FILTER_MAINTENANCE_FILTER_CHANGE_INDICATION_IND_ENABLE      0
#define FILTER_MAINTENANCE_FILTER_LIFE_LEVEL_IND_ENABLE				0

#define FILTER_MAINTENANCE_FILTER_CHANGE_INDICATION_BRC_ENABLE      0
#define FILTER_MAINTENANCE_FILTER_LIFE_LEVEL_BRC_ENABLE      		0

#define FILTER_MAINTENANCE_FILTER_CHANGE_INDICATION_NO      3
#if FILTER_MAINTENANCE_FILTER_CHANGE_INDICATION_IND_ENABLE
    #define FILTER_MAINTENANCE_FILTER_CHANGE_INDICATION_IND_DESC \
        {0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hFilterMaintenanceFilterChangeIndication), ccc_hFilterMaintenanceFilterChangeIndicationWriteCB, 0},
    #define FILTER_MAINTENANCE_FILTER_CHANGE_INDICATION_IND_NO  1
#else
    #define FILTER_MAINTENANCE_FILTER_CHANGE_INDICATION_IND_DESC
    #define FILTER_MAINTENANCE_FILTER_CHANGE_INDICATION_IND_NO  0
#endif

#if CHAR_FILTER_MAINTENANCE_FILTER_LIFE_LEVEL_ENABLE
	#define FILTER_MAINTENANCE_FILTER_LIFE_LEVEL_NO 3
	#define CHAR_FILTER_MAINTENANCE_FILTER_LIFE_LEVEL_ATT_TABLE \
	{0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_FilterMaintenance_FilterLifeLevel_R_S, 0},  \
	{0,ATT_PERMISSIONS_RDWR,16,sizeof(hFilterMaintenanceFilterLifeLevel),0, (u8*)(charFilterLifeLevelUUID), (u8*)&hFilterMaintenanceFilterLifeLevel, 0, FilterMaintenanceFilterLifeLevelReadCallback}, \
		{0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + FILTER_MAINTENANCE_INST_NO + 3), 0, 0},  \
		FILTER_MAINTENANCE_FILTER_LIFE_LEVEL_IND_DESC

	#if FILTER_MAINTENANCE_FILTER_LIFE_LEVEL_IND_ENABLE
		#define FILTER_MAINTENANCE_FILTER_LIFE_LEVEL_IND_DESC \
			{0,ATT_PERMISSIONS_RDWR,2,2,2, (u8*)(&ClientCharConfig_UUID), (u8*)(&ccc_hFilterMaintenanceFilterLifeLevel), ccc_hFilterMaintenanceFilterLifeLevelWriteCB, 0},
		#define FILTER_MAINTENANCE_FILTER_LIFE_LEVEL_IND_NO	1
	#else
		#define FILTER_MAINTENANCE_FILTER_LIFE_LEVEL_IND_DESC
		#define FILTER_MAINTENANCE_FILTER_LIFE_LEVEL_IND_NO 0
	#endif
#else
	#define FILTER_MAINTENANCE_FILTER_LIFE_LEVEL_NO 0
	#define CHAR_FILTER_MAINTENANCE_FILTER_LIFE_LEVEL_ATT_TABLE
	#define FILTER_MAINTENANCE_FILTER_LIFE_LEVEL_IND_NO 0
#endif

#if CHAR_FILTER_MAINTENANCE_RESET_FILTER_INDICATION_ENABLE
	#define FILTER_MAINTENANCE_RESET_FILTER_INDICATION_NO 3
	#define CHAR_FILTER_MAINTENANCE_RESET_FILTER_INDICATION_ATT_TABLE \
	{0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_FilterMaintenance_ResetFilterIndication_R_S, 0},  \
	{0,ATT_PERMISSIONS_RDWR,16,sizeof(hFilterMaintenanceResetFilterIndication),0, (u8*)(charResetFilterIndicationUUID), (u8*)&hFilterMaintenanceResetFilterIndication, FilterMaintenanceResetFilterIndicationWriteCallback, 0}, \
		{0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + FILTER_MAINTENANCE_INST_NO + 4), 0, 0},

#else
	#define FILTER_MAINTENANCE_RESET_FILTER_INDICATION_NO 0
	#define CHAR_FILTER_MAINTENANCE_RESET_FILTER_INDICATION_ATT_TABLE
#endif


#if CHAR_FILTER_MAINTENANCE_SVC_SIGN_ENABLE
    #define FILTER_MAINTENANCE_SVC_SIGN_NO   3
    #define CHAR_FILTER_MAINTENANCE_SVC_A5_SIGN_TABLE   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_filter_maintenance_A5_R_S, &hap_desc_service_filter_maintenance_A5_R_S},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof (VAR_FILTER_MAINTENANCE_UNKOWN),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&VAR_FILTER_MAINTENANCE_UNKOWN, 0, 0},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + FILTER_MAINTENANCE_INST_NO + 5), 0, 0},
#else
    #define FILTER_MAINTENANCE_SVC_SIGN_NO   0
    #define CHAR_FILTER_MAINTENANCE_SVC_A5_SIGN_TABLE
#endif

#define FILTER_MAINTENANCE_NO_DELTA    FILTER_MAINTENANCE_FILTER_CHANGE_INDICATION_IND_NO \
										+FILTER_MAINTENANCE_FILTER_LIFE_LEVEL_IND_NO

#define FILTER_MAINTENANCE_NO          9 +FILTER_MAINTENANCE_FILTER_LIFE_LEVEL_NO \
										+FILTER_MAINTENANCE_RESET_FILTER_INDICATION_NO \
										+FILTER_MAINTENANCE_SVC_SIGN_NO

#define FILTER_MAINTENANCE_HANDLE_NUM (FILTER_MAINTENANCE_NO + FILTER_MAINTENANCE_NO_DELTA)


#define HOMEKIT_FILTER_MAINTENANCE_SERVICE_ATT_TABLE   \
    {FILTER_MAINTENANCE_HANDLE_NUM,ATT_PERMISSIONS_READ,2,sizeof(filter_maintenanceServiceUUID),sizeof(filter_maintenanceServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(filter_maintenanceServiceUUID), 0, 0},   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(serviceInstanceUUID), (u8*)(insts + FILTER_MAINTENANCE_INST_NO), 0, 0},   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_NAME, 0},    \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(hFilterMaintenanceServiceName), 0, (u8*)(nameUUID), (u8*)(hFilterMaintenanceServiceName), 0, 0},  \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(&charInstanceUUID), (u8*)(insts + FILTER_MAINTENANCE_INST_NO + 1), 0, 0}, \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteIndicateProp), &hap_desc_char_FilterMaintenance_FilterChangeIndication_R_S, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,sizeof(hFilterMaintenanceFilterChangeIndication),0, (u8*)(charFilterChangeIndicationUUID), (u8*)&hFilterMaintenanceFilterChangeIndication, 0, FilterMaintenanceFilterChangeIndicationReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + FILTER_MAINTENANCE_INST_NO + 2), 0, 0},  \
            FILTER_MAINTENANCE_FILTER_CHANGE_INDICATION_IND_DESC \
            CHAR_FILTER_MAINTENANCE_FILTER_LIFE_LEVEL_ATT_TABLE \
            CHAR_FILTER_MAINTENANCE_RESET_FILTER_INDICATION_ATT_TABLE \
        	CHAR_FILTER_MAINTENANCE_SVC_A5_SIGN_TABLE


#define FILTER_MAINTENANCE_HANDLE_COMMON_NUM     8

#define FILTER_MAINTENANCE_HANDLE_NUM_HFILTER_MAINTENANCE_FILTER_CHANGE_INDICATION  FILTER_MAINTENANCE_HANDLE_START_NUM +FILTER_MAINTENANCE_HANDLE_COMMON_NUM
#define FILTER_MAINTENANCE_HANDLE_NUM_HFILTER_MAINTENANCE_FILTER_LIFE_LEVEL  		FILTER_MAINTENANCE_HANDLE_NUM_HFILTER_MAINTENANCE_FILTER_CHANGE_INDICATION +FILTER_MAINTENANCE_FILTER_CHANGE_INDICATION_NO +FILTER_MAINTENANCE_FILTER_CHANGE_INDICATION_IND_NO
#define FILTER_MAINTENANCE_HANDLE_NUM_HFILTER_MAINTENANCE_RESET_FILTER_INDICATION	FILTER_MAINTENANCE_HANDLE_NUM_HFILTER_MAINTENANCE_FILTER_LIFE_LEVEL +FILTER_MAINTENANCE_FILTER_LIFE_LEVEL_NO +FILTER_MAINTENANCE_FILTER_LIFE_LEVEL_IND_NO
#define FILTER_MAINTENANCE_HANDLE_NUM_THE_END


#define FILTER_MAINTENANCE_STORE_CHAR_VALUE_T_HFILTER_MAINTENANCE_FILTER_CHANGE_INDICATION            u32 FilterMaintenanceFilterChangeIndication:1;    /* 0~1 */
#define FILTER_MAINTENANCE_STORE_CHAR_VALUE_T_HFILTER_MAINTENANCE_FILTER_CHANGE_INDICATION_STORE          STORE_CHAR(FilterMaintenanceFilterChangeIndication,hFilterMaintenanceFilterChangeIndication)
#define FILTER_MAINTENANCE_STORE_CHAR_VALUE_T_HFILTER_MAINTENANCE_FILTER_CHANGE_INDICATION_RESTORE        RESTORE_CHAR(FilterMaintenanceFilterChangeIndication,hFilterMaintenanceFilterChangeIndication)

#define FILTER_MAINTENANCE_STORE_CHAR_VALUE_T_HFILTER_MAINTENANCE_FILTER_LIFE_LEVEL            		  u32 FilterMaintenanceFilterLifeLevel;    /* float */
#define FILTER_MAINTENANCE_STORE_CHAR_VALUE_T_HFILTER_MAINTENANCE_FILTER_LIFE_LEVEL_STORE          		  STORE_CHAR(FilterMaintenanceFilterLifeLevel,hFilterMaintenanceFilterLifeLevel)
#define FILTER_MAINTENANCE_STORE_CHAR_VALUE_T_HFILTER_MAINTENANCE_FILTER_LIFE_LEVEL_RESTORE        		  RESTORE_CHAR(FilterMaintenanceFilterLifeLevel,hFilterMaintenanceFilterLifeLevel)

#define FILTER_MAINTENANCE_STORE_CHAR_VALUE_T_HFILTER_MAINTENANCE_RESET_FILTER_INDICATION             u32 FilterMaintenanceResetFilterIndication:1;    /* 1 */
#define FILTER_MAINTENANCE_STORE_CHAR_VALUE_T_HFILTER_MAINTENANCE_RESET_FILTER_INDICATION_STORE           STORE_CHAR(FilterMaintenanceResetFilterIndication,hFilterMaintenanceResetFilterIndication)
#define FILTER_MAINTENANCE_STORE_CHAR_VALUE_T_HFILTER_MAINTENANCE_RESET_FILTER_INDICATION_RESTORE         RESTORE_CHAR(FilterMaintenanceResetFilterIndication,hFilterMaintenanceResetFilterIndication)


#define FILTER_MAINTENANCE_STORE_CHAR_VALUE_T  \
                    FILTER_MAINTENANCE_STORE_CHAR_VALUE_T_HFILTER_MAINTENANCE_FILTER_CHANGE_INDICATION \
                    FILTER_MAINTENANCE_STORE_CHAR_VALUE_T_HFILTER_MAINTENANCE_FILTER_LIFE_LEVEL \
                    FILTER_MAINTENANCE_STORE_CHAR_VALUE_T_HFILTER_MAINTENANCE_RESET_FILTER_INDICATION

#define FILTER_MAINTENANCE_STORE_CHAR_TO_FLASH  \
                    FILTER_MAINTENANCE_STORE_CHAR_VALUE_T_HFILTER_MAINTENANCE_FILTER_CHANGE_INDICATION_STORE \
                    FILTER_MAINTENANCE_STORE_CHAR_VALUE_T_HFILTER_MAINTENANCE_FILTER_LIFE_LEVEL_STORE \
                    FILTER_MAINTENANCE_STORE_CHAR_VALUE_T_HFILTER_MAINTENANCE_RESET_FILTER_INDICATION_STORE

#define FILTER_MAINTENANCE_RESTORE_CHAR_FROM_FLASH  \
                    FILTER_MAINTENANCE_STORE_CHAR_VALUE_T_HFILTER_MAINTENANCE_FILTER_CHANGE_INDICATION_RESTORE \
                    FILTER_MAINTENANCE_STORE_CHAR_VALUE_T_HFILTER_MAINTENANCE_FILTER_LIFE_LEVEL_RESTORE \
                    FILTER_MAINTENANCE_STORE_CHAR_VALUE_T_HFILTER_MAINTENANCE_RESET_FILTER_INDICATION_RESTORE


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */
EXTERN unsigned char hFilterMaintenanceFilterChangeIndication;
EXTERN int hFilterMaintenanceFilterLifeLevel;
EXTERN unsigned char hFilterMaintenanceResetFilterIndication;

EXTERN unsigned short ccc_hFilterMaintenanceFilterChangeIndication;
EXTERN unsigned short ccc_hFilterMaintenanceFilterLifeLevel;

#if FILTER_MAINTENANCE_FILTER_CHANGE_INDICATION_BRC_ENABLE
    #define BC_PARA_FILTER_MAINTENANCE_FILTER_CHANGE_INDICATION ((u8*)&bc_para_hFilterMaintenanceFilterChangeIndication)
    EXTERN unsigned char bc_para_hFilterMaintenanceFilterChangeIndication;
#else
    #define BC_PARA_FILTER_MAINTENANCE_FILTER_CHANGE_INDICATION 0
#endif

#if FILTER_MAINTENANCE_FILTER_LIFE_LEVEL_BRC_ENABLE
    #define BC_PARA_FILTER_MAINTENANCE_FILTER_LIFE_LEVEL ((u8*)&bc_para_hFilterMaintenanceFilterLifeLevel)
    EXTERN unsigned char bc_para_hFilterMaintenanceFilterLifeLevel;
#else
    #define BC_PARA_FILTER_MAINTENANCE_FILTER_LIFE_LEVEL 0
#endif

#if FILTER_MAINTENANCE_LINKED_SERVICES_ENABLE
	#define FILTER_MAINTENANCE_LINKED_SVCS			hFilterMaintenance_linked_svcs
	#define FILTER_MAINTENANCE_LINKED_SVCS_LENGTH 	(sizeof(hFilterMaintenance_linked_svcs)/sizeof(size_t)*2)
	EXTERN const u16 * hFilterMaintenance_linked_svcs[];
#else
	#define FILTER_MAINTENANCE_LINKED_SVCS			0
	#define FILTER_MAINTENANCE_LINKED_SVCS_LENGTH 	0
#endif

EXTERN const u8 hFilterMaintenanceServiceName[24];

EXTERN const hap_characteristic_desc_t hap_desc_char_FilterMaintenance_FilterChangeIndication_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_FilterMaintenance_FilterLifeLevel_R_S;
EXTERN const hap_characteristic_desc_t hap_desc_char_FilterMaintenance_ResetFilterIndication_R_S;

/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
EXTERN int FilterMaintenanceFilterChangeIndicationReadCallback(void *pp);
EXTERN int FilterMaintenanceFilterLifeLevelReadCallback(void *pp);
EXTERN int FilterMaintenanceResetFilterIndicationWriteCallback(void *pp);

EXTERN int ccc_hFilterMaintenanceFilterChangeIndicationWriteCB(void *pp);
EXTERN int ccc_hFilterMaintenanceFilterLifeLevelWriteCB(void *pp);

#endif /* _HK_FILTER_MAINTENANCE_SERVICE_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
