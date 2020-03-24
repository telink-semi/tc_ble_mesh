/********************************************************************************************************
 * @file     hk_TopUUID.h
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
#ifndef _HK_TOP_UUID_H_
#define _HK_TOP_UUID_H_


/* ------------------------------------
    Header Files
   ------------------------------------ */
//#include "app_att.h"

/* ------------------------------------
    Macro Definitions
   ------------------------------------ */
#undef EXTERN

#ifdef _HK_TOP_UUID_C_
    #define EXTERN
#else
    #define EXTERN  extern
#endif

/** @defgroup SERVICES_ENABLE Top level for each services enable definitions
 * @{
 */
/////////////////////////////////////////////////////////////
// Top level for each services enable definitions
/////////////////////////////////////////////////////////////
#define ACCESSORY_INFO_SERVICE_ENABLE                0   // 01
#define FAN_SERVICE_ENABLE                           0   // 02
#define GARAGEDOOROPENER_SERVICE_ENABLE              0   // 03
#define LIGHTBULB_SERVICE_ENABLE                     1   // 04
#define LOCKMANAGEMENT_SERVICE_ENABLE                0   // 05
#define LOCKMECHANISM_SERVICE_ENABLE                 0   // 06
#define OUTLET_SERVICE_ENABLE                        0   // 07
#define SWITCH_SERVICE_ENABLE                        0   // 08
#define THERMOSTAT_SERVICE_ENABLE                    0   // 09
#define SENSOR_AIR_QUALITY_SERVICE_ENABLE            0   // 10
#define SECURITY_SERVICE_ENABLE                      0   // 11
#define SENSOR_CARBON_MONOXIDE_SERVICE_ENABLE        0   // 12
#define SENSOR_CONTACT_SERVICE_ENABLE                0   // 13
#define DOOR_SERVICE_ENABLE                          0   // 14
#define SENSOR_HUMIDITY_SERVICE_ENABLE               0   // 15
#define SENSOR_LEAK_SERVICE_ENABLE                   0   // 16
#define SENSOR_LIGHT_SERVICE_ENABLE                  0   // 17
#define SENSOR_MOTION_SERVICE_ENABLE                 0   // 18
#define SENSOR_OCCUPANCY_SERVICE_ENABLE              0   // 19
#define SENSOR_SMOKE_SERVICE_ENABLE                  0   // 20
#define STATELESS_PROG_SW_SERVICE_ENABLE             0   // 21
#define SENSOR_TEMPERATURE_SERVICE_ENABLE            0   // 22
#define WINDOW_SERVICE_ENABLE                        0   // 23
#define WINDOW_COVERING_SERVICE_ENABLE               0   // 24
#define BATTERY_SERVICE_ENABLE                       0   // 25
#define SENSOR_CARBON_DIOXIDE_SERVICE_ENABLE         0   // 26
#define FAN_V2_SERVICE_ENABLE                        0   // 31
#define SLAT_SERVICE_ENABLE                          0   // 32
#define FILTER_MAINTENANCE_SERVICE_ENABLE            0   // 33
#define AIR_PURIFIER_SERVICE_ENABLE                  0   // 34
#define SERVICE_LABEL_SERVICE_ENABLE                 0   // 35
#define HEATER_COOLER_SERVICE_ENABLE                 0   // 36
#define HUMIDIFIER_DEHUMIDIFIER_SERVICE_ENABLE       0   // 37
#define IRRIGATION_SYSTEM_SERVICE_ENABLE             0   // 38
#define VALVE_SERVICE_ENABLE                         0   // 39
#define FAUCET_SERVICE_ENABLE                        0   // 40

//custom service: homekit OTA.
#define OTA_SERVICE_ENABLE                        	 0   // 41

#define STATEFUL_PROG_SW_SERVICE_ENABLE              0   //!< unsupported Service on R9 Spec.
#define CAMERA_RTP_STREAM_MANAGEMENT_SERVICE_ENABLE  0   // 27 //!< it didn't support camera service.
#define MICROPHONE_SERVICE_ENABLE                    0   // 28
#define SPEAKER_SERVICE_ENABLE                       0   // 29
#define DOORBELL_SERVICE_ENABLE                      0   // 30 // require Video Doorbell Secondary Services, ref. R9 Spec. Sec.11.3.2.

/** @} End SERVICES_ENABLE **/


/////////////////////////////////////////////////////////////
// Assign Only ONE service as primary service
// and enable Hidden services
/////////////////////////////////////////////////////////////
#define ACCESSORY_INFO_SERVICE_PRIMARY_HIDDEN_PROPERTY                0   // 01
#define FAN_SERVICE_PRIMARY_HIDDEN_PROPERTY                           0   // 02
#define GARAGEDOOROPENER_SERVICE_PRIMARY_HIDDEN_PROPERTY              0   // 03
#define LIGHTBULB_SERVICE_PRIMARY_HIDDEN_PROPERTY                     1   // 04
#define LOCKMANAGEMENT_SERVICE_PRIMARY_HIDDEN_PROPERTY                0   // 05
#define LOCKMECHANISM_SERVICE_PRIMARY_HIDDEN_PROPERTY                 0   // 06
#define OUTLET_SERVICE_PRIMARY_HIDDEN_PROPERTY                        0   // 07
#define SWITCH_SERVICE_PRIMARY_HIDDEN_PROPERTY                        0   // 08
#define THERMOSTAT_SERVICE_PRIMARY_HIDDEN_PROPERTY                    0   // 09
#define SENSOR_AIR_QUALITY_SERVICE_PRIMARY_HIDDEN_PROPERTY            0   // 10
#define SECURITY_SERVICE_PRIMARY_HIDDEN_PROPERTY                      0   // 11
#define SENSOR_CARBON_MONOXIDE_SERVICE_PRIMARY_HIDDEN_PROPERTY        0   // 12
#define SENSOR_CONTACT_SERVICE_PRIMARY_HIDDEN_PROPERTY                0   // 13
#define DOOR_SERVICE_PRIMARY_HIDDEN_PROPERTY                          0   // 14
#define SENSOR_HUMIDITY_SERVICE_PRIMARY_HIDDEN_PROPERTY               0   // 15
#define SENSOR_LEAK_SERVICE_PRIMARY_HIDDEN_PROPERTY                   0   // 16
#define SENSOR_LIGHT_SERVICE_PRIMARY_HIDDEN_PROPERTY                  0   // 17
#define SENSOR_MOTION_SERVICE_PRIMARY_HIDDEN_PROPERTY                 0   // 18
#define SENSOR_OCCUPANCY_SERVICE_PRIMARY_HIDDEN_PROPERTY              0   // 19
#define SENSOR_SMOKE_SERVICE_PRIMARY_HIDDEN_PROPERTY                  0   // 20
#define STATELESS_PROG_SW_SERVICE_PRIMARY_HIDDEN_PROPERTY             0   // 21
#define SENSOR_TEMPERATURE_SERVICE_PRIMARY_HIDDEN_PROPERTY            0   // 22
#define WINDOW_SERVICE_PRIMARY_HIDDEN_PROPERTY                        0   // 23
#define WINDOW_COVERING_SERVICE_PRIMARY_HIDDEN_PROPERTY               0   // 24
#define BATTERY_SERVICE_PRIMARY_HIDDEN_PROPERTY                       0   // 25
#define SENSOR_CARBON_DIOXIDE_SERVICE_PRIMARY_HIDDEN_PROPERTY         0   // 26
#define MICROPHONE_SERVICE_PRIMARY_HIDDEN_PROPERTY                    0   // 28
#define SPEAKER_SERVICE_PRIMARY_HIDDEN_PROPERTY                       0   // 29
#define DOORBELL_SERVICE_PRIMARY_HIDDEN_PROPERTY                      0   // 30 // require Video Doorbell Secondary Services, ref. R9 Spec. Sec.11.3.2.
#define FAN_V2_SERVICE_PRIMARY_HIDDEN_PROPERTY                        0   // 31
#define SLAT_SERVICE_PRIMARY_HIDDEN_PROPERTY                          0   // 32
#define FILTER_MAINTENANCE_SERVICE_PRIMARY_HIDDEN_PROPERTY            0   // 33
#define AIR_PURIFIER_SERVICE_PRIMARY_HIDDEN_PROPERTY                  0   // 34
#define SERVICE_LABEL_SERVICE_PRIMARY_HIDDEN_PROPERTY                 0   // 35
#define HEATER_COOLER_SERVICE_PRIMARY_HIDDEN_PROPERTY                 0   // 36
#define HUMIDIFIER_DEHUMIDIFIER_SERVICE_PRIMARY_HIDDEN_PROPERTY       0   // 37
#define IRRIGATION_SYSTEM_SERVICE_PRIMARY_HIDDEN_PROPERTY             0   // 38
#define VALVE_SERVICE_PRIMARY_HIDDEN_PROPERTY                         0   // 39
#define FAUCET_SERVICE_PRIMARY_HIDDEN_PROPERTY                        0   // 40

//custom service: homekit OTA.
#define OTA_SERVICE_PRIMARY_HIDDEN_PROPERTY                        	  0   // 41


#define STATEFUL_PROG_SW_SERVICE_PRIMARY_HIDDEN_PROPERTY              0   //!< unsupported Service on R9 Spec.
#define CAMERA_RTP_STREAM_MANAGEMENT_SERVICE_PRIMARY_HIDDEN_PROPERTY  0   // 27 //!< it didn't support camera service.


/////////////////////////////////////////////////////////////
#define HOMEKIT_INFO_SERVICE_NO    21
#define HOMEKIT_INFORMATION_SERVICE_ATT_TABLE   \
    /* homekit accessory protocol for LE*/  \
    {HOMEKIT_INFO_SERVICE_NO,ATT_PERMISSIONS_READ,2,sizeof(informationServiceUUID),sizeof(informationServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(&informationServiceUUID), 0, 0}, \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},   /* homekit accessory protocol for LE, chapter 12.2*/    \
        {0,ATT_PERMISSIONS_READ,sizeof(serviceInstanceUUID),2,2, (u8*)(&serviceInstanceUUID), (u8*)(insts + 0), 0, 0},   /* add callback to return instances*/   \
        {0,ATT_PERMISSIONS_READ,2,1,0, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_identify, 0}, \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof (identify), 0, (u8*)(identifyUUID), (u8*)(&identify), identifyWriteCallback, 0},  \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + 1), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_string, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(manufacturer), 0, (u8*)(manufacturerUUID), (u8*)(manufacturer), 0, HAPCommonSetSessionTimeoutReadCB},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + 2), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_string, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(model), 0, (u8*)(modelUUID), (u8*)(model), 0, HAPCommonSetSessionTimeoutReadCB}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + 3), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_string, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(name), 0, (u8*)(nameUUID), (u8*)(name), 0, HAPCommonSetSessionTimeoutReadCB},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + 4), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_string, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(serialNumber), 0, (u8*)(&serialNumberUUID), (u8*)(serialNumber), 0, HAPCommonSetSessionTimeoutReadCB},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + 5), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_string, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(firmware_revision), 0, (u8*)(&FirmwareRevUUID), (u8*)(firmware_revision), 0, HAPCommonSetSessionTimeoutReadCB},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + 6), 0, 0},

#define HOMEKIT_ACCESSORY_PROTOCOL_NO    15
#define HOMEKIT_ACCESSORY_PROTOCOL_ATT_TABLE    \
    /* homekit accessory protocol pairing spec.*/   \
    {HOMEKIT_ACCESSORY_PROTOCOL_NO,ATT_PERMISSIONS_READ,2,sizeof(pairingServiceUUID),sizeof(pairingServiceUUID), (u8*)(&primaryServiceUUID), (u8*)(&pairingServiceUUID), 0, 0}, \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},\
        {0,ATT_PERMISSIONS_READ,sizeof(serviceInstanceUUID),2,2, (u8*)(&serviceInstanceUUID), (u8*)(insts + 7), 0, 0},\
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), (att_readwrite_callback_t)(&hap_desc_pairSetup), 0},    \
        {0,ATT_PERMISSIONS_RDWR,16,1,254, (u8*)(pairSetupUUID), 0, pairSetupWriteCallback, pairSetupReadCallback},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + 8), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), (att_readwrite_callback_t)(&hap_desc_pairVerify), 0},   \
        {0,ATT_PERMISSIONS_RDWR,16,1,254, (u8*)(pairVerifyUUID), 0, pairVerifyWriteCallback, pairVerifyReadCallback},    \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + 9), 0, 0},    \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), (att_readwrite_callback_t*)(&hap_desc_pairFeature), 0}, \
        {0,ATT_PERMISSIONS_RDWR,16,1,254, (u8*)(pairFeatureUUID), (u8*)&my_pairFeature, 0, 0},   \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + 10), 0, 0},   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), (att_readwrite_callback_t)(&hap_desc_pairPairing), 0},  \
        {0,ATT_PERMISSIONS_RDWR,16,0,0, (u8*)(pairPairingUUID), 0, pairPairWriteCallback, pairPairReadCallback}, \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + 11), 0, 0},


#define HOMEKIT_HAP_BLE_PROTOCOL_INFO_NO    6+3
#define HOMEKIT_HAP_BLE_PROTOCOL_INFORMATION_TABLE  \
    {HOMEKIT_HAP_BLE_PROTOCOL_INFO_NO,ATT_PERMISSIONS_READ,2,sizeof(HapBleInfoUUID),sizeof(HapBleInfoUUID), (u8*)(&primaryServiceUUID), (u8*)(&HapBleInfoUUID), 0, 0},  \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readOnlyProp), 0, 0},   \
        {0,ATT_PERMISSIONS_READ,sizeof(serviceInstanceUUID),2,2, (u8*)(&serviceInstanceUUID), (u8*)(insts + 12), 0, 0},  \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_ble_version, 0},  \
        {0,ATT_PERMISSIONS_RDWR,16, sizeof(hap_ble_version), 0, (u8*)(&charVersionUUID), (u8*)(hap_ble_version), 0, HAPCommonSetSessionTimeoutReadCB},  \
            {0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + 13), 0, 0},   \
        {0,ATT_PERMISSIONS_READ,2,1,1, (u8*)(&characterUUID), (u8*)(&readWriteProp), &hap_desc_char_protocol_info_A5_R_S, &hap_desc_service_protocol_info_A5_R_S},   \
		{0,ATT_PERMISSIONS_RDWR,16,sizeof (hap_desc_char_protocol_info_temp_var),0, (u8*)(HapServiceSignatureCharUUID), (u8*)&hap_desc_char_protocol_info_temp_var, HAPProtocolInfoWriteCallback, HAPProtocolInfoReadCallback},    \
			{0,ATT_PERMISSIONS_READ,16,2,2, (u8*)(charInstanceUUID), (u8*)(insts + 14), 0, 0},


#if FAN_SERVICE_ENABLE                          // 01
    #include "hk_FanService.h"
#else
    #undef FAN_STORE_CHAR_VALUE_T
    #undef FAN_STORE_CHAR_TO_FLASH
    #undef FAN_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_FAN_SERVICE_ATT_TABLE
    #undef FAN_NO
    #undef FAN_NO_DELTA
    #undef FAN_HANDLE_NUM
    #define FAN_STORE_CHAR_VALUE_T
    #define FAN_STORE_CHAR_TO_FLASH
    #define FAN_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_FAN_SERVICE_ATT_TABLE
    #define FAN_NO                              0
    #define FAN_NO_DELTA                        0
    #define FAN_HANDLE_NUM                      0
#endif

#if GARAGEDOOROPENER_SERVICE_ENABLE             // 02
    #include "hk_GarageDoorOpenerService.h"
#else
    #undef GARAGEDOOROPENER_STORE_CHAR_VALUE_T
    #undef GARAGEDOOROPENER_STORE_CHAR_TO_FLASH
    #undef GARAGEDOOROPENER_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_GARAGEDOOROPENER_SERVICE_ATT_TABLE
    #undef GARAGEDOOROPENER_NO
    #undef GARAGEDOOROPENER_NO_DELTA
    #undef GARAGEDOOROPENER_HANDLE_NUM
    #define GARAGEDOOROPENER_STORE_CHAR_VALUE_T
    #define GARAGEDOOROPENER_STORE_CHAR_TO_FLASH
    #define GARAGEDOOROPENER_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_GARAGEDOOROPENER_SERVICE_ATT_TABLE
    #define GARAGEDOOROPENER_NO                 0
    #define GARAGEDOOROPENER_NO_DELTA           0
    #define GARAGEDOOROPENER_HANDLE_NUM         0
#endif

#if LIGHTBULB_SERVICE_ENABLE                    // 03
    #include "hk_LightBulbService.h"
#else
    #undef LIGHTBULB_STORE_CHAR_VALUE_T
    #undef LIGHTBULB_STORE_CHAR_TO_FLASH
    #undef LIGHTBULB_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_LIGHTBULB_SERVICE_ATT_TABLE
    #undef LIGHTBULB_NO
    #undef LIGHTBULB_NO_DELTA
    #undef LIGHTBULB_HANDLE_NUM
    #define LIGHTBULB_STORE_CHAR_VALUE_T
    #define LIGHTBULB_STORE_CHAR_TO_FLASH
    #define LIGHTBULB_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_LIGHTBULB_SERVICE_ATT_TABLE
    #define LIGHTBULB_NO                        0
    #define LIGHTBULB_NO_DELTA                  0
    #define LIGHTBULB_HANDLE_NUM                0
#endif

#if LOCKMANAGEMENT_SERVICE_ENABLE               // 04
    #include "hk_LockManagementService.h"
#else
    #undef LOCKMANAGEMENT_STORE_CHAR_VALUE_T
    #undef LOCKMANAGEMENT_STORE_CHAR_TO_FLASH
    #undef LOCKMANAGEMENT_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_LOCKMANAGEMENT_SERVICE_ATT_TABLE
    #undef LOCKMANAGEMENT_NO
    #undef LOCKMANAGEMENT_NO_DELTA
    #undef LOCKMANAGEMENT_HANDLE_NUM
    #define LOCKMANAGEMENT_STORE_CHAR_VALUE_T
    #define LOCKMANAGEMENT_STORE_CHAR_TO_FLASH
    #define LOCKMANAGEMENT_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_LOCKMANAGEMENT_SERVICE_ATT_TABLE
    #define LOCKMANAGEMENT_NO                   0
    #define LOCKMANAGEMENT_NO_DELTA             0
    #define LOCKMANAGEMENT_HANDLE_NUM           0
#endif

#if LOCKMECHANISM_SERVICE_ENABLE                // 05
    #include "hk_LockMechanismService.h"
#else
    #undef LOCKMECHANISM_STORE_CHAR_VALUE_T
    #undef LOCKMECHANISM_STORE_CHAR_TO_FLASH
    #undef LOCKMECHANISM_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_LOCKMECHANISM_SERVICE_ATT_TABLE
    #undef LOCKMECHANISM_NO
    #undef LOCKMECHANISM_NO_DELTA
    #undef LOCKMECHANISM_HANDLE_NUM
    #define LOCKMECHANISM_STORE_CHAR_VALUE_T
    #define LOCKMECHANISM_STORE_CHAR_TO_FLASH
    #define LOCKMECHANISM_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_LOCKMECHANISM_SERVICE_ATT_TABLE
    #define LOCKMECHANISM_NO                    0
    #define LOCKMECHANISM_NO_DELTA              0
    #define LOCKMECHANISM_HANDLE_NUM            0
#endif

#if OUTLET_SERVICE_ENABLE                       // 06
    #include "hk_OutletService.h"
#else
    #undef OUTLET_STORE_CHAR_VALUE_T
    #undef OUTLET_STORE_CHAR_TO_FLASH
    #undef OUTLET_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_OUTLET_SERVICE_ATT_TABLE
    #undef OUTLET_NO
    #undef OUTLET_NO_DELTA
    #undef OUTLET_HANDLE_NUM
    #define OUTLET_STORE_CHAR_VALUE_T
    #define OUTLET_STORE_CHAR_TO_FLASH
    #define OUTLET_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_OUTLET_SERVICE_ATT_TABLE
    #define OUTLET_NO                           0
    #define OUTLET_NO_DELTA                     0
    #define OUTLET_HANDLE_NUM                   0
#endif

#if SWITCH_SERVICE_ENABLE                       // 07
    #include "hk_SwitchService.h"
#else
    #undef SWITCH_STORE_CHAR_VALUE_T
    #undef SWITCH_STORE_CHAR_TO_FLASH
    #undef SWITCH_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_SWITCH_SERVICE_ATT_TABLE
    #undef SWITCH_NO
    #undef SWITCH_NO_DELTA
    #undef SWITCH_HANDLE_NUM
    #define SWITCH_STORE_CHAR_VALUE_T
    #define SWITCH_STORE_CHAR_TO_FLASH
    #define SWITCH_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_SWITCH_SERVICE_ATT_TABLE
    #define SWITCH_NO                           0
    #define SWITCH_NO_DELTA                     0
    #define SWITCH_HANDLE_NUM                   0
#endif

#if THERMOSTAT_SERVICE_ENABLE                   // 08
    #include "hk_ThermostatService.h"
#else
    #undef THERMOSTAT_STORE_CHAR_VALUE_T
    #undef THERMOSTAT_STORE_CHAR_TO_FLASH
    #undef THERMOSTAT_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_THERMOSTAT_SERVICE_ATT_TABLE
    #undef THERMOSTAT_NO
    #undef THERMOSTAT_NO_DELTA
    #undef THERMOSTAT_HANDLE_NUM
    #define THERMOSTAT_STORE_CHAR_VALUE_T
    #define THERMOSTAT_STORE_CHAR_TO_FLASH
    #define THERMOSTAT_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_THERMOSTAT_SERVICE_ATT_TABLE
    #define THERMOSTAT_NO                       0
    #define THERMOSTAT_NO_DELTA                 0
    #define THERMOSTAT_HANDLE_NUM               0
#endif

#if SECURITY_SERVICE_ENABLE                     // 09
    #include "hk_SecurityService.h"
#else
    #undef SECURITY_STORE_CHAR_VALUE_T
    #undef SECURITY_STORE_CHAR_TO_FLASH
    #undef SECURITY_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_SECURITY_SERVICE_ATT_TABLE
    #undef SECURITY_NO
    #undef SECURITY_NO_DELTA
    #undef SECURITY_HANDLE_NUM
    #define SECURITY_STORE_CHAR_VALUE_T
    #define SECURITY_STORE_CHAR_TO_FLASH
    #define SECURITY_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_SECURITY_SERVICE_ATT_TABLE
    #define SECURITY_NO                         0
    #define SECURITY_NO_DELTA                   0
    #define SECURITY_HANDLE_NUM                 0
#endif

#if DOOR_SERVICE_ENABLE                         // 10
    #include "hk_DoorService.h"
#else
    #undef DOOR_STORE_CHAR_VALUE_T
    #undef DOOR_STORE_CHAR_TO_FLASH
    #undef DOOR_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_DOOR_SERVICE_ATT_TABLE
    #undef DOOR_NO
    #undef DOOR_NO_DELTA
    #undef DOOR_HANDLE_NUM
    #define DOOR_STORE_CHAR_VALUE_T
    #define DOOR_STORE_CHAR_TO_FLASH
    #define DOOR_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_DOOR_SERVICE_ATT_TABLE
    #define DOOR_NO                             0
    #define DOOR_NO_DELTA                       0
    #define DOOR_HANDLE_NUM                     0
#endif

#if STATEFUL_PROG_SW_SERVICE_ENABLE             // 11
    #include "hk_StatefulProgSwService.h"
#else
    #undef STATEFUL_PROG_SW_STORE_CHAR_VALUE_T
    #undef STATEFUL_PROG_SW_STORE_CHAR_TO_FLASH
    #undef STATEFUL_PROG_SW_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_STATEFUL_PROG_SW_SERVICE_ATT_TABLE
    #undef STATEFUL_PROG_SW_NO
    #undef STATEFUL_PROG_SW_NO_DELTA
    #undef STATEFUL_PROG_SW_HANDLE_NUM
    #define STATEFUL_PROG_SW_STORE_CHAR_VALUE_T
    #define STATEFUL_PROG_SW_STORE_CHAR_TO_FLASH
    #define STATEFUL_PROG_SW_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_STATEFUL_PROG_SW_SERVICE_ATT_TABLE
    #define STATEFUL_PROG_SW_NO                 0
    #define STATEFUL_PROG_SW_NO_DELTA           0
    #define STATEFUL_PROG_SW_HANDLE_NUM         0
#endif

#if STATELESS_PROG_SW_SERVICE_ENABLE            // 12
    #include "hk_StatelessProgSwService.h"
#else
    #undef STATELESS_PROG_SW_STORE_CHAR_VALUE_T
    #undef STATELESS_PROG_SW_STORE_CHAR_TO_FLASH
    #undef STATELESS_PROG_SW_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_STATELESS_PROG_SW_SERVICE_ATT_TABLE
    #undef STATELESS_PROG_SW_NO
    #undef STATELESS_PROG_SW_NO_DELTA
    #undef STATELESS_PROG_SW_HANDLE_NUM
    #define STATELESS_PROG_SW_STORE_CHAR_VALUE_T
    #define STATELESS_PROG_SW_STORE_CHAR_TO_FLASH
    #define STATELESS_PROG_SW_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_STATELESS_PROG_SW_SERVICE_ATT_TABLE
    #define STATELESS_PROG_SW_NO                0
    #define STATELESS_PROG_SW_NO_DELTA          0
    #define STATELESS_PROG_SW_HANDLE_NUM        0
#endif

#if WINDOW_SERVICE_ENABLE                       // 13
    #include "hk_WindowService.h"
#else
    #undef WINDOW_STORE_CHAR_VALUE_T
    #undef WINDOW_STORE_CHAR_TO_FLASH
    #undef WINDOW_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_WINDOW_SERVICE_ATT_TABLE
    #undef WINDOW_NO
    #undef WINDOW_NO_DELTA
    #undef WINDOW_HANDLE_NUM
    #define WINDOW_STORE_CHAR_VALUE_T
    #define WINDOW_STORE_CHAR_TO_FLASH
    #define WINDOW_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_WINDOW_SERVICE_ATT_TABLE
    #define WINDOW_NO                           0
    #define WINDOW_NO_DELTA                     0
    #define WINDOW_HANDLE_NUM                   0
#endif

#if WINDOW_COVERING_SERVICE_ENABLE              // 14
    #include "hk_WindowCoveringService.h"
#else
    #undef WINDOW_COVERING_STORE_CHAR_VALUE_T
    #undef WINDOW_COVERING_STORE_CHAR_TO_FLASH
    #undef WINDOW_COVERING_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_WINDOW_COVERING_SERVICE_ATT_TABLE
    #undef WINDOW_COVERING_NO
    #undef WINDOW_COVERING_NO_DELTA
    #undef WINDOW_COVERING_HANDLE_NUM
    #define WINDOW_COVERING_STORE_CHAR_VALUE_T
    #define WINDOW_COVERING_STORE_CHAR_TO_FLASH
    #define WINDOW_COVERING_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_WINDOW_COVERING_SERVICE_ATT_TABLE
    #define WINDOW_COVERING_NO                  0
    #define WINDOW_COVERING_NO_DELTA            0
    #define WINDOW_COVERING_HANDLE_NUM          0
#endif

#if BATTERY_SERVICE_ENABLE                      // 15
    #include "hk_BatteryService.h"
#else
    #undef BATTERY_STORE_CHAR_VALUE_T
    #undef BATTERY_STORE_CHAR_TO_FLASH
    #undef BATTERY_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_BATTERY_SERVICE_ATT_TABLE
    #undef BATTERY_NO
    #undef BATTERY_NO_DELTA
    #undef BATTERY_HANDLE_NUM
    #define BATTERY_STORE_CHAR_VALUE_T
    #define BATTERY_STORE_CHAR_TO_FLASH
    #define BATTERY_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_BATTERY_SERVICE_ATT_TABLE
    #define BATTERY_NO                          0
    #define BATTERY_NO_DELTA                    0
    #define BATTERY_HANDLE_NUM                  0
#endif

#if SENSOR_AIR_QUALITY_SERVICE_ENABLE           // 16
    #include "hk_SensorAirQualityService.h"
#else
    #undef SENSOR_AIR_QUALITY_STORE_CHAR_VALUE_T
    #undef SENSOR_AIR_QUALITY_STORE_CHAR_TO_FLASH
    #undef SENSOR_AIR_QUALITY_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_SENSOR_AIR_QUALITY_SERVICE_ATT_TABLE
    #undef SENSOR_AIR_QUALITY_NO
    #undef SENSOR_AIR_QUALITY_NO_DELTA
    #undef SENSOR_AIR_QUALITY_HANDLE_NUM
    #define SENSOR_AIR_QUALITY_STORE_CHAR_VALUE_T
    #define SENSOR_AIR_QUALITY_STORE_CHAR_TO_FLASH
    #define SENSOR_AIR_QUALITY_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_SENSOR_AIR_QUALITY_SERVICE_ATT_TABLE
    #define SENSOR_AIR_QUALITY_NO               0
    #define SENSOR_AIR_QUALITY_NO_DELTA         0
    #define SENSOR_AIR_QUALITY_HANDLE_NUM       0
#endif

#if SENSOR_CARBON_MONOXIDE_SERVICE_ENABLE       // 17
    #include "hk_SensorCarbonMonoxideService.h"
#else
    #undef SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T
    #undef SENSOR_CARBON_MONOXIDE_STORE_CHAR_TO_FLASH
    #undef SENSOR_CARBON_MONOXIDE_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_SENSOR_CARBON_MONOXIDE_SERVICE_ATT_TABLE
    #undef SENSOR_CARBON_MONOXIDE_NO
    #undef SENSOR_CARBON_MONOXIDE_NO_DELTA
    #undef SENSOR_CARBON_MONOXIDE_HANDLE_NUM
    #define SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T
    #define SENSOR_CARBON_MONOXIDE_STORE_CHAR_TO_FLASH
    #define SENSOR_CARBON_MONOXIDE_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_SENSOR_CARBON_MONOXIDE_SERVICE_ATT_TABLE
    #define SENSOR_CARBON_MONOXIDE_NO           0
    #define SENSOR_CARBON_MONOXIDE_NO_DELTA     0
    #define SENSOR_CARBON_MONOXIDE_HANDLE_NUM   0
#endif

#if SENSOR_CONTACT_SERVICE_ENABLE               // 18
    #include "hk_SensorContactService.h"
#else
    #undef SENSOR_CONTACT_STORE_CHAR_VALUE_T
    #undef SENSOR_CONTACT_STORE_CHAR_TO_FLASH
    #undef SENSOR_CONTACT_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_SENSOR_CONTACT_SERVICE_ATT_TABLE
    #undef SENSOR_CONTACT_NO
    #undef SENSOR_CONTACT_NO_DELTA
    #undef SENSOR_CONTACT_HANDLE_NUM
    #define SENSOR_CONTACT_STORE_CHAR_VALUE_T
    #define SENSOR_CONTACT_STORE_CHAR_TO_FLASH
    #define SENSOR_CONTACT_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_SENSOR_CONTACT_SERVICE_ATT_TABLE
    #define SENSOR_CONTACT_NO                   0
    #define SENSOR_CONTACT_NO_DELTA             0
    #define SENSOR_CONTACT_HANDLE_NUM           0
#endif

#if SENSOR_HUMIDITY_SERVICE_ENABLE              // 19
    #include "hk_SensorHumidityService.h"
#else
    #undef SENSOR_HUMIDITY_STORE_CHAR_VALUE_T
    #undef SENSOR_HUMIDITY_STORE_CHAR_TO_FLASH
    #undef SENSOR_HUMIDITY_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_SENSOR_HUMIDITY_SERVICE_ATT_TABLE
    #undef SENSOR_HUMIDITY_NO
    #undef SENSOR_HUMIDITY_NO_DELTA
    #undef SENSOR_HUMIDITY_HANDLE_NUM
    #define SENSOR_HUMIDITY_STORE_CHAR_VALUE_T
    #define SENSOR_HUMIDITY_STORE_CHAR_TO_FLASH
    #define SENSOR_HUMIDITY_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_SENSOR_HUMIDITY_SERVICE_ATT_TABLE
    #define SENSOR_HUMIDITY_NO                  0
    #define SENSOR_HUMIDITY_NO_DELTA            0
    #define SENSOR_HUMIDITY_HANDLE_NUM          0
#endif

#if SENSOR_LEAK_SERVICE_ENABLE                  // 20
    #include "hk_SensorLeakService.h"
#else
    #undef SENSOR_LEAK_STORE_CHAR_VALUE_T
    #undef SENSOR_LEAK_STORE_CHAR_TO_FLASH
    #undef SENSOR_LEAK_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_SENSOR_LEAK_SERVICE_ATT_TABLE
    #undef SENSOR_LEAK_NO
    #undef SENSOR_LEAK_NO_DELTA
    #undef SENSOR_LEAK_HANDLE_NUM
    #define SENSOR_LEAK_STORE_CHAR_VALUE_T
    #define SENSOR_LEAK_STORE_CHAR_TO_FLASH
    #define SENSOR_LEAK_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_SENSOR_LEAK_SERVICE_ATT_TABLE
    #define SENSOR_LEAK_NO                      0
    #define SENSOR_LEAK_NO_DELTA                0
    #define SENSOR_LEAK_HANDLE_NUM              0
#endif

#if SENSOR_LIGHT_SERVICE_ENABLE                 // 21
    #include "hk_SensorLightService.h"
#else
    #undef SENSOR_LIGHT_STORE_CHAR_VALUE_T
    #undef SENSOR_LIGHT_STORE_CHAR_TO_FLASH
    #undef SENSOR_LIGHT_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_SENSOR_LIGHT_SERVICE_ATT_TABLE
    #undef SENSOR_LIGHT_NO
    #undef SENSOR_LIGHT_NO_DELTA
    #undef SENSOR_LIGHT_HANDLE_NUM
    #define SENSOR_LIGHT_STORE_CHAR_VALUE_T
    #define SENSOR_LIGHT_STORE_CHAR_TO_FLASH
    #define SENSOR_LIGHT_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_SENSOR_LIGHT_SERVICE_ATT_TABLE
    #define SENSOR_LIGHT_NO                     0
    #define SENSOR_LIGHT_NO_DELTA               0
    #define SENSOR_LIGHT_HANDLE_NUM             0
#endif

#if SENSOR_MOTION_SERVICE_ENABLE                // 22
    #include "hk_SensorMotionService.h"
#else
    #undef SENSOR_MOTION_STORE_CHAR_VALUE_T
    #undef SENSOR_MOTION_STORE_CHAR_TO_FLASH
    #undef SENSOR_MOTION_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_SENSOR_MOTION_SERVICE_ATT_TABLE
    #undef SENSOR_MOTION_NO
    #undef SENSOR_MOTION_NO_DELTA
    #undef SENSOR_MOTION_HANDLE_NUM
    #define SENSOR_MOTION_STORE_CHAR_VALUE_T
    #define SENSOR_MOTION_STORE_CHAR_TO_FLASH
    #define SENSOR_MOTION_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_SENSOR_MOTION_SERVICE_ATT_TABLE
    #define SENSOR_MOTION_NO                    0
    #define SENSOR_MOTION_NO_DELTA              0
    #define SENSOR_MOTION_HANDLE_NUM            0
#endif

#if SENSOR_OCCUPANCY_SERVICE_ENABLE             // 23
    #include "hk_SensorOccupancyService.h"
#else
    #undef SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T
    #undef SENSOR_OCCUPANCY_STORE_CHAR_TO_FLASH
    #undef SENSOR_OCCUPANCY_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_SENSOR_OCCUPANCY_SERVICE_ATT_TABLE
    #undef SENSOR_OCCUPANCY_NO
    #undef SENSOR_OCCUPANCY_NO_DELTA
    #undef SENSOR_OCCUPANCY_HANDLE_NUM
    #define SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T
    #define SENSOR_OCCUPANCY_STORE_CHAR_TO_FLASH
    #define SENSOR_OCCUPANCY_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_SENSOR_OCCUPANCY_SERVICE_ATT_TABLE
    #define SENSOR_OCCUPANCY_NO                 0
    #define SENSOR_OCCUPANCY_NO_DELTA           0
    #define SENSOR_OCCUPANCY_HANDLE_NUM         0
#endif

#if SENSOR_SMOKE_SERVICE_ENABLE                 // 24
    #include "hk_SensorSmokeService.h"
#else
    #undef SENSOR_SMOKE_STORE_CHAR_VALUE_T
    #undef SENSOR_SMOKE_STORE_CHAR_TO_FLASH
    #undef SENSOR_SMOKE_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_SENSOR_SMOKE_SERVICE_ATT_TABLE
    #undef SENSOR_SMOKE_NO
    #undef SENSOR_SMOKE_NO_DELTA
    #undef SENSOR_SMOKE_HANDLE_NUM
    #define SENSOR_SMOKE_STORE_CHAR_VALUE_T
    #define SENSOR_SMOKE_STORE_CHAR_TO_FLASH
    #define SENSOR_SMOKE_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_SENSOR_SMOKE_SERVICE_ATT_TABLE
    #define SENSOR_SMOKE_NO                     0
    #define SENSOR_SMOKE_NO_DELTA               0
    #define SENSOR_SMOKE_HANDLE_NUM             0
#endif

#if SENSOR_TEMPERATURE_SERVICE_ENABLE           // 25
    #include "hk_SensorTemperatureService.h"
#else
    #undef SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T
    #undef SENSOR_TEMPERATURE_STORE_CHAR_TO_FLASH
    #undef SENSOR_TEMPERATURE_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_SENSOR_TEMPERATURE_SERVICE_ATT_TABLE
    #undef SENSOR_TEMPERATURE_NO
    #undef SENSOR_TEMPERATURE_NO_DELTA
    #undef SENSOR_TEMPERATURE_HANDLE_NUM
    #define SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T
    #define SENSOR_TEMPERATURE_STORE_CHAR_TO_FLASH
    #define SENSOR_TEMPERATURE_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_SENSOR_TEMPERATURE_SERVICE_ATT_TABLE
    #define SENSOR_TEMPERATURE_NO               0
    #define SENSOR_TEMPERATURE_NO_DELTA         0
    #define SENSOR_TEMPERATURE_HANDLE_NUM       0
#endif

#if SENSOR_CARBON_DIOXIDE_SERVICE_ENABLE        // 26
    #include "hk_SensorCarbonDioxideService.h"
#else
    #undef SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T
    #undef SENSOR_CARBON_DIOXIDE_STORE_CHAR_TO_FLASH
    #undef SENSOR_CARBON_DIOXIDE_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_SENSOR_CARBON_DIOXIDE_SERVICE_ATT_TABLE
    #undef SENSOR_CARBON_DIOXIDE_NO
    #undef SENSOR_CARBON_DIOXIDE_NO_DELTA
    #undef SENSOR_CARBON_DIOXIDE_HANDLE_NUM
    #define SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T
    #define SENSOR_CARBON_DIOXIDE_STORE_CHAR_TO_FLASH
    #define SENSOR_CARBON_DIOXIDE_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_SENSOR_CARBON_DIOXIDE_SERVICE_ATT_TABLE
    #define SENSOR_CARBON_DIOXIDE_NO            0
    #define SENSOR_CARBON_DIOXIDE_NO_DELTA      0
    #define SENSOR_CARBON_DIOXIDE_HANDLE_NUM    0
#endif

#if CAMERA_RTP_STREAM_MANAGEMENT_SERVICE_ENABLE // 27
    #include "hk_CameraRtpStreamManagementService.h"
#else
    #undef CAM_RTP_STR_MANAGEMENT_STORE_CHAR_VALUE_T
    #undef CAM_RTP_STR_MANAGEMENT_STORE_CHAR_TO_FLASH
    #undef CAM_RTP_STR_MANAGEMENT_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_CAM_RTP_STR_MANAGEMENT_SERVICE_ATT_TABLE
    #undef CAM_RTP_STR_MANAGEMENT_NO
    #undef CAM_RTP_STR_MANAGEMENT_NO_DELTA
    #undef CAM_RTP_STR_MANAGEMENT_HANDLE_NUM
    #define CAM_RTP_STR_MANAGEMENT_STORE_CHAR_VALUE_T
    #define CAM_RTP_STR_MANAGEMENT_STORE_CHAR_TO_FLASH
    #define CAM_RTP_STR_MANAGEMENT_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_CAM_RTP_STR_MANAGEMENT_SERVICE_ATT_TABLE
    #define CAM_RTP_STR_MANAGEMENT_NO           0
    #define CAM_RTP_STR_MANAGEMENT_NO_DELTA     0
    #define CAM_RTP_STR_MANAGEMENT_HANDLE_NUM   0
#endif

#if MICROPHONE_SERVICE_ENABLE                   // 28
    #include "hk_MicrophoneService.h"
#else
    #undef MICROPHONE_STORE_CHAR_VALUE_T
    #undef MICROPHONE_STORE_CHAR_TO_FLASH
    #undef MICROPHONE_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_MICROPHONE_SERVICE_ATT_TABLE
    #undef MICROPHONE_NO
    #undef MICROPHONE_NO_DELTA
    #undef MICROPHONE_HANDLE_NUM
    #define MICROPHONE_STORE_CHAR_VALUE_T
    #define MICROPHONE_STORE_CHAR_TO_FLASH
    #define MICROPHONE_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_MICROPHONE_SERVICE_ATT_TABLE
    #define MICROPHONE_NO                       0
    #define MICROPHONE_NO_DELTA                 0
    #define MICROPHONE_HANDLE_NUM               0
#endif

#if SPEAKER_SERVICE_ENABLE                      // 29
    #include "hk_SpeakerService.h"
#else
    #undef SPEAKER_STORE_CHAR_VALUE_T
    #undef SPEAKER_STORE_CHAR_TO_FLASH
    #undef SPEAKER_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_SPEAKER_SERVICE_ATT_TABLE
    #undef SPEAKER_NO
    #undef SPEAKER_NO_DELTA
    #undef SPEAKER_HANDLE_NUM
    #define SPEAKER_STORE_CHAR_VALUE_T
    #define SPEAKER_STORE_CHAR_TO_FLASH
    #define SPEAKER_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_SPEAKER_SERVICE_ATT_TABLE
    #define SPEAKER_NO                          0
    #define SPEAKER_NO_DELTA                    0
    #define SPEAKER_HANDLE_NUM                  0
#endif

#if DOORBELL_SERVICE_ENABLE                     // 30
    #include "hk_DoorbellService.h"
#else
    #undef DOORBELL_STORE_CHAR_VALUE_T
    #undef DOORBELL_STORE_CHAR_TO_FLASH
    #undef DOORBELL_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_DOORBELL_SERVICE_ATT_TABLE
    #undef DOORBELL_NO
    #undef DOORBELL_NO_DELTA
    #undef DOORBELL_HANDLE_NUM
    #define DOORBELL_STORE_CHAR_VALUE_T
    #define DOORBELL_STORE_CHAR_TO_FLASH
    #define DOORBELL_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_DOORBELL_SERVICE_ATT_TABLE
    #define DOORBELL_NO                         0
    #define DOORBELL_NO_DELTA                   0
    #define DOORBELL_HANDLE_NUM                 0
#endif

#if FAN_V2_SERVICE_ENABLE                       // 31
    #include "hk_FanV2Service.h"
#else
    #undef FAN_V2_STORE_CHAR_VALUE_T
    #undef FAN_V2_STORE_CHAR_TO_FLASH
    #undef FAN_V2_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_FAN_V2_SERVICE_ATT_TABLE
    #undef FAN_V2_NO
    #undef FAN_V2_NO_DELTA
    #undef FAN_V2_HANDLE_NUM
    #define FAN_V2_STORE_CHAR_VALUE_T
    #define FAN_V2_STORE_CHAR_TO_FLASH
    #define FAN_V2_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_FAN_V2_SERVICE_ATT_TABLE
    #define FAN_V2_NO                         0
    #define FAN_V2_NO_DELTA                   0
    #define FAN_V2_HANDLE_NUM                 0
#endif

#if SLAT_SERVICE_ENABLE                         // 32
    #include "hk_SlatService.h"
#else
    #undef SLAT_STORE_CHAR_VALUE_T
    #undef SLAT_STORE_CHAR_TO_FLASH
    #undef SLAT_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_SLAT_SERVICE_ATT_TABLE
    #undef SLAT_NO
    #undef SLAT_NO_DELTA
    #undef SLAT_HANDLE_NUM
    #define SLAT_STORE_CHAR_VALUE_T
    #define SLAT_STORE_CHAR_TO_FLASH
    #define SLAT_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_SLAT_SERVICE_ATT_TABLE
    #define SLAT_NO                         0
    #define SLAT_NO_DELTA                   0
    #define SLAT_HANDLE_NUM                 0
#endif

#if FILTER_MAINTENANCE_SERVICE_ENABLE           // 33
    #include "hk_FilterMaintenanceService.h"
#else
    #undef FILTER_MAINTENANCE_STORE_CHAR_VALUE_T
    #undef FILTER_MAINTENANCE_STORE_CHAR_TO_FLASH
    #undef FILTER_MAINTENANCE_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_FILTER_MAINTENANCE_SERVICE_ATT_TABLE
    #undef FILTER_MAINTENANCE_NO
    #undef FILTER_MAINTENANCE_NO_DELTA
    #undef FILTER_MAINTENANCE_HANDLE_NUM
    #define FILTER_MAINTENANCE_STORE_CHAR_VALUE_T
    #define FILTER_MAINTENANCE_STORE_CHAR_TO_FLASH
    #define FILTER_MAINTENANCE_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_FILTER_MAINTENANCE_SERVICE_ATT_TABLE
    #define FILTER_MAINTENANCE_NO                         0
    #define FILTER_MAINTENANCE_NO_DELTA                   0
    #define FILTER_MAINTENANCE_HANDLE_NUM                 0
#endif

#if AIR_PURIFIER_SERVICE_ENABLE                 // 34
    #include "hk_AirPurifierService.h"
#else
    #undef AIR_PURIFIER_STORE_CHAR_VALUE_T
    #undef AIR_PURIFIER_STORE_CHAR_TO_FLASH
    #undef AIR_PURIFIER_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_AIR_PURIFIER_SERVICE_ATT_TABLE
    #undef AIR_PURIFIER_NO
    #undef AIR_PURIFIER_NO_DELTA
    #undef AIR_PURIFIER_HANDLE_NUM
    #define AIR_PURIFIER_STORE_CHAR_VALUE_T
    #define AIR_PURIFIER_STORE_CHAR_TO_FLASH
    #define AIR_PURIFIER_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_AIR_PURIFIER_SERVICE_ATT_TABLE
    #define AIR_PURIFIER_NO                         0
    #define AIR_PURIFIER_NO_DELTA                   0
    #define AIR_PURIFIER_HANDLE_NUM                 0
#endif

#if SERVICE_LABEL_SERVICE_ENABLE                // 35
    #include "hk_ServiceLabelService.h"
#else
    #undef SERVICE_LABEL_STORE_CHAR_VALUE_T
    #undef SERVICE_LABEL_STORE_CHAR_TO_FLASH
    #undef SERVICE_LABEL_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_SERVICE_LABEL_SERVICE_ATT_TABLE
    #undef SERVICE_LABEL_NO
    #undef SERVICE_LABEL_NO_DELTA
    #undef SERVICE_LABEL_HANDLE_NUM
    #define SERVICE_LABEL_STORE_CHAR_VALUE_T
    #define SERVICE_LABEL_STORE_CHAR_TO_FLASH
    #define SERVICE_LABEL_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_SERVICE_LABEL_SERVICE_ATT_TABLE
    #define SERVICE_LABEL_NO                         0
    #define SERVICE_LABEL_NO_DELTA                   0
    #define SERVICE_LABEL_HANDLE_NUM                 0
#endif

#if HEATER_COOLER_SERVICE_ENABLE                // 36
    #include "hk_HeaterCoolerService.h"
#else
    #undef HEATERCOOLER_STORE_CHAR_VALUE_T
    #undef HEATERCOOLER_STORE_CHAR_TO_FLASH
    #undef HEATERCOOLER_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_HEATERCOOLER_SERVICE_ATT_TABLE
    #undef HEATERCOOLER_NO
    #undef HEATERCOOLER_NO_DELTA
    #undef HEATERCOOLER_HANDLE_NUM
    #define HEATERCOOLER_STORE_CHAR_VALUE_T
    #define HEATERCOOLER_STORE_CHAR_TO_FLASH
    #define HEATERCOOLER_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_HEATERCOOLER_SERVICE_ATT_TABLE
    #define HEATERCOOLER_NO                         0
    #define HEATERCOOLER_NO_DELTA                   0
    #define HEATERCOOLER_HANDLE_NUM                 0
#endif

#if HUMIDIFIER_DEHUMIDIFIER_SERVICE_ENABLE      // 37
    #include "hk_HumidifierDehumidifierService.h"
#else
    #undef HUMIDIFIERDEHUMIDIFIER_STORE_CHAR_VALUE_T
    #undef HUMIDIFIERDEHUMIDIFIER_STORE_CHAR_TO_FLASH
    #undef HUMIDIFIERDEHUMIDIFIER_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_HUMIDIFIERDEHUMIDIFIER_SERVICE_ATT_TABLE
    #undef HUMIDIFIERDEHUMIDIFIER_NO
    #undef HUMIDIFIERDEHUMIDIFIER_NO_DELTA
    #undef HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM
    #define HUMIDIFIERDEHUMIDIFIER_STORE_CHAR_VALUE_T
    #define HUMIDIFIERDEHUMIDIFIER_STORE_CHAR_TO_FLASH
    #define HUMIDIFIERDEHUMIDIFIER_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_HUMIDIFIERDEHUMIDIFIER_SERVICE_ATT_TABLE
    #define HUMIDIFIERDEHUMIDIFIER_NO               0
    #define HUMIDIFIERDEHUMIDIFIER_NO_DELTA         0
    #define HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM       0
#endif

#if IRRIGATION_SYSTEM_SERVICE_ENABLE			// 38
    #include "hk_IrrigationSystemService.h"
#else
    #undef IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T
    #undef IRRIGATIONSYSTEM_STORE_CHAR_TO_FLASH
    #undef IRRIGATIONSYSTEM_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_IRRIGATIONSYSTEM_SERVICE_ATT_TABLE
    #undef IRRIGATIONSYSTEM_NO
    #undef IRRIGATIONSYSTEM_NO_DELTA
    #undef IRRIGATIONSYSTEM_HANDLE_NUM
    #define IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T
    #define IRRIGATIONSYSTEM_STORE_CHAR_TO_FLASH
    #define IRRIGATIONSYSTEM_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_IRRIGATIONSYSTEM_SERVICE_ATT_TABLE
    #define IRRIGATIONSYSTEM_NO               0
    #define IRRIGATIONSYSTEM_NO_DELTA         0
    #define IRRIGATIONSYSTEM_HANDLE_NUM       0
#endif

#if VALVE_SERVICE_ENABLE						// 39
    #include "hk_ValveService.h"
#else
    #undef VALVE_STORE_CHAR_VALUE_T
    #undef VALVE_STORE_CHAR_TO_FLASH
    #undef VALVE_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_VALVE_SERVICE_ATT_TABLE
    #undef VALVE_NO
    #undef VALVE_NO_DELTA
    #undef VALVE_HANDLE_NUM
    #define VALVE_STORE_CHAR_VALUE_T
    #define VALVE_STORE_CHAR_TO_FLASH
    #define VALVE_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_VALVE_SERVICE_ATT_TABLE
    #define VALVE_NO               0
    #define VALVE_NO_DELTA         0
    #define VALVE_HANDLE_NUM       0
#endif

#if FAUCET_SERVICE_ENABLE						// 40
    #include "hk_FaucetService.h"
#else
    #undef FAUCET_STORE_CHAR_VALUE_T
    #undef FAUCET_STORE_CHAR_TO_FLASH
    #undef FAUCET_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_FAUCET_SERVICE_ATT_TABLE
    #undef FAUCET_NO
    #undef FAUCET_NO_DELTA
    #undef FAUCET_HANDLE_NUM
    #define FAUCET_STORE_CHAR_VALUE_T
    #define FAUCET_STORE_CHAR_TO_FLASH
    #define FAUCET_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_FAUCET_SERVICE_ATT_TABLE
    #define FAUCET_NO               0
    #define FAUCET_NO_DELTA         0
    #define FAUCET_HANDLE_NUM       0
#endif

//custom service: homekit OTA.
#if OTA_SERVICE_ENABLE						// 41
    #include "hk_OtaService.h"
#else
    #undef OTA_STORE_CHAR_VALUE_T
    #undef OTA_STORE_CHAR_TO_FLASH
    #undef OTA_RESTORE_CHAR_FROM_FLASH
    #undef HOMEKIT_OTA_SERVICE_ATT_TABLE
    #undef OTA_NO
    #undef OTA_NO_DELTA
    #undef OTA_HANDLE_NUM
    #define OTA_STORE_CHAR_VALUE_T
    #define OTA_STORE_CHAR_TO_FLASH
    #define OTA_RESTORE_CHAR_FROM_FLASH
    #define HOMEKIT_OTA_SERVICE_ATT_TABLE
    #define OTA_NO               0
    #define OTA_NO_DELTA         0
    #define OTA_HANDLE_NUM       0
#endif


enum{
    FAN_INST_NO = 16,
    GARAGEDOOROPENER_INST_NO = FAN_INST_NO + (FAN_NO / 3),
    LIGHTBULB_INST_NO = GARAGEDOOROPENER_INST_NO +(GARAGEDOOROPENER_NO / 3),
    LOCKMANAGEMENT_INST_NO = LIGHTBULB_INST_NO + (LIGHTBULB_NO / 3),
    LOCKMECHANISM_INST_NO = LOCKMANAGEMENT_INST_NO + (LOCKMANAGEMENT_NO / 3),
    OUTLET_INST_NO = LOCKMECHANISM_INST_NO + (LOCKMECHANISM_NO / 3),
    SWITCH_INST_NO = OUTLET_INST_NO + (OUTLET_NO / 3),
    THERMOSTAT_INST_NO = SWITCH_INST_NO + (SWITCH_NO / 3),
    SECURITY_INST_NO = THERMOSTAT_INST_NO + (THERMOSTAT_NO / 3),
    DOOR_INST_NO = SECURITY_INST_NO + (SECURITY_NO / 3),
    STATEFUL_PROG_SW_INST_NO = DOOR_INST_NO + (DOOR_NO / 3),
    STATELESS_PROG_SW_INST_NO = STATEFUL_PROG_SW_INST_NO + (STATEFUL_PROG_SW_NO / 3),
    WINDOW_INST_NO = STATELESS_PROG_SW_INST_NO + (STATELESS_PROG_SW_NO / 3),
    WINDOW_COVERING_INST_NO = WINDOW_INST_NO + (WINDOW_NO / 3),
    BATTERY_INST_NO = WINDOW_COVERING_INST_NO + (WINDOW_COVERING_NO / 3),
    SENSOR_AIR_QUALITY_INST_NO = BATTERY_INST_NO + (BATTERY_NO / 3),
    SENSOR_CARBON_MONOXIDE_INST_NO = SENSOR_AIR_QUALITY_INST_NO + (SENSOR_AIR_QUALITY_NO / 3),
    SENSOR_CONTACT_INST_NO = SENSOR_CARBON_MONOXIDE_INST_NO + (SENSOR_CARBON_MONOXIDE_NO / 3),
    SENSOR_HUMIDITY_INST_NO = SENSOR_CONTACT_INST_NO + (SENSOR_CONTACT_NO / 3),
    SENSOR_LEAK_INST_NO = SENSOR_HUMIDITY_INST_NO + (SENSOR_HUMIDITY_NO / 3),
    SENSOR_LIGHT_INST_NO = SENSOR_LEAK_INST_NO + (SENSOR_LEAK_NO / 3),
    SENSOR_MOTION_INST_NO = SENSOR_LIGHT_INST_NO + (SENSOR_LIGHT_NO / 3),
    SENSOR_OCCUPANCY_INST_NO = SENSOR_MOTION_INST_NO + (SENSOR_MOTION_NO / 3),
    SENSOR_SMOKE_INST_NO = SENSOR_OCCUPANCY_INST_NO + (SENSOR_OCCUPANCY_NO / 3),
    SENSOR_TEMPERATURE_INST_NO = SENSOR_SMOKE_INST_NO + (SENSOR_SMOKE_NO / 3),
    SENSOR_CARBON_DIOXIDE_INST_NO = SENSOR_TEMPERATURE_INST_NO + (SENSOR_TEMPERATURE_NO / 3),
    CAM_RTP_STR_MANAGEMENT_INST_NO = SENSOR_CARBON_DIOXIDE_INST_NO + (SENSOR_CARBON_DIOXIDE_NO / 3),
    MICROPHONE_INST_NO = CAM_RTP_STR_MANAGEMENT_INST_NO + (CAM_RTP_STR_MANAGEMENT_NO / 3),
    SPEAKER_INST_NO = MICROPHONE_INST_NO + (MICROPHONE_NO / 3),
    DOORBELL_INST_NO = SPEAKER_INST_NO + (SPEAKER_NO / 3),
    FAN_V2_INST_NO = DOORBELL_INST_NO + (DOORBELL_NO / 3),
    SLAT_INST_NO = FAN_V2_INST_NO + (FAN_V2_NO / 3),
    FILTER_MAINTENANCE_INST_NO = SLAT_INST_NO + (SLAT_NO / 3),
    AIR_PURIFIER_INST_NO = FILTER_MAINTENANCE_INST_NO + (FILTER_MAINTENANCE_NO / 3),
    SERVICE_LABEL_INST_NO = AIR_PURIFIER_INST_NO + (AIR_PURIFIER_NO / 3),
    HEATERCOOLER_INST_NO = SERVICE_LABEL_INST_NO + (SERVICE_LABEL_NO / 3),
    HUMIDIFIERDEHUMIDIFIER_INST_NO = HEATERCOOLER_INST_NO + (HEATERCOOLER_NO / 3),
    IRRIGATIONSYSTEM_INST_NO = HUMIDIFIERDEHUMIDIFIER_INST_NO + (HUMIDIFIERDEHUMIDIFIER_NO / 3),
    VALVE_INST_NO = IRRIGATIONSYSTEM_INST_NO + (IRRIGATIONSYSTEM_NO / 3),
    FAUCET_INST_NO = VALVE_INST_NO + (VALVE_NO / 3),
    LAST_INSTANCE_ID = FAUCET_INST_NO + (FAUCET_NO / 3),
};

//for constant instance id of OTA custom service in hk_OTAService.h.
#define OTA_INST_NO			1000

enum{
    FAN_HANDLE_START_NUM = 5 +HOMEKIT_INFO_SERVICE_NO +HOMEKIT_ACCESSORY_PROTOCOL_NO +HOMEKIT_HAP_BLE_PROTOCOL_INFO_NO,
    GARAGEDOOROPENER_HANDLE_START_NUM = FAN_HANDLE_START_NUM + (FAN_HANDLE_NUM),
    LIGHTBULB_HANDLE_START_NUM = GARAGEDOOROPENER_HANDLE_START_NUM +(GARAGEDOOROPENER_HANDLE_NUM),
    LOCKMANAGEMENT_HANDLE_START_NUM = LIGHTBULB_HANDLE_START_NUM + (LIGHTBULB_HANDLE_NUM),
    LOCKMECHANISM_HANDLE_START_NUM = LOCKMANAGEMENT_HANDLE_START_NUM + (LOCKMANAGEMENT_HANDLE_NUM),
    OUTLET_HANDLE_START_NUM = LOCKMECHANISM_HANDLE_START_NUM + (LOCKMECHANISM_HANDLE_NUM),
    SWITCH_HANDLE_START_NUM = OUTLET_HANDLE_START_NUM + (OUTLET_HANDLE_NUM),
    THERMOSTAT_HANDLE_START_NUM = SWITCH_HANDLE_START_NUM + (SWITCH_HANDLE_NUM),
    SECURITY_HANDLE_START_NUM = THERMOSTAT_HANDLE_START_NUM + (THERMOSTAT_HANDLE_NUM),
    DOOR_HANDLE_START_NUM = SECURITY_HANDLE_START_NUM + (SECURITY_HANDLE_NUM),
    STATEFUL_PROG_SW_HANDLE_START_NUM = DOOR_HANDLE_START_NUM + (DOOR_HANDLE_NUM),
    STATELESS_PROG_SW_HANDLE_START_NUM = STATEFUL_PROG_SW_HANDLE_START_NUM + (STATEFUL_PROG_SW_HANDLE_NUM),
    WINDOW_HANDLE_START_NUM = STATELESS_PROG_SW_HANDLE_START_NUM + (STATELESS_PROG_SW_HANDLE_NUM),
    WINDOW_COVERING_HANDLE_START_NUM = WINDOW_HANDLE_START_NUM + (WINDOW_HANDLE_NUM),
    BATTERY_HANDLE_START_NUM = WINDOW_COVERING_HANDLE_START_NUM + (WINDOW_COVERING_HANDLE_NUM),
    SENSOR_AIR_QUALITY_HANDLE_START_NUM = BATTERY_HANDLE_START_NUM + (BATTERY_HANDLE_NUM),
    SENSOR_CARBON_MONOXIDE_HANDLE_START_NUM = SENSOR_AIR_QUALITY_HANDLE_START_NUM + (SENSOR_AIR_QUALITY_HANDLE_NUM),
    SENSOR_CONTACT_HANDLE_START_NUM = SENSOR_CARBON_MONOXIDE_HANDLE_START_NUM + (SENSOR_CARBON_MONOXIDE_HANDLE_NUM),
    SENSOR_HUMIDITY_HANDLE_START_NUM = SENSOR_CONTACT_HANDLE_START_NUM + (SENSOR_CONTACT_HANDLE_NUM),
    SENSOR_LEAK_HANDLE_START_NUM = SENSOR_HUMIDITY_HANDLE_START_NUM + (SENSOR_HUMIDITY_HANDLE_NUM),
    SENSOR_LIGHT_HANDLE_START_NUM = SENSOR_LEAK_HANDLE_START_NUM + (SENSOR_LEAK_HANDLE_NUM),
    SENSOR_MOTION_HANDLE_START_NUM = SENSOR_LIGHT_HANDLE_START_NUM + (SENSOR_LIGHT_HANDLE_NUM),
    SENSOR_OCCUPANCY_HANDLE_START_NUM = SENSOR_MOTION_HANDLE_START_NUM + (SENSOR_MOTION_HANDLE_NUM),
    SENSOR_SMOKE_HANDLE_START_NUM = SENSOR_OCCUPANCY_HANDLE_START_NUM + (SENSOR_OCCUPANCY_HANDLE_NUM),
    SENSOR_TEMPERATURE_HANDLE_START_NUM = SENSOR_SMOKE_HANDLE_START_NUM + (SENSOR_SMOKE_HANDLE_NUM),
    SENSOR_CARBON_DIOXIDE_HANDLE_START_NUM = SENSOR_TEMPERATURE_HANDLE_START_NUM + (SENSOR_TEMPERATURE_HANDLE_NUM),
    CAM_RTP_STR_MANAGEMENT_HANDLE_START_NUM = SENSOR_CARBON_DIOXIDE_HANDLE_START_NUM + (SENSOR_CARBON_DIOXIDE_HANDLE_NUM),
    MICROPHONE_HANDLE_START_NUM = CAM_RTP_STR_MANAGEMENT_HANDLE_START_NUM + (CAM_RTP_STR_MANAGEMENT_HANDLE_NUM),
    SPEAKER_HANDLE_START_NUM = MICROPHONE_HANDLE_START_NUM + (MICROPHONE_HANDLE_NUM),
    DOORBELL_HANDLE_START_NUM = SPEAKER_HANDLE_START_NUM + (SPEAKER_HANDLE_NUM),
    FAN_V2_HANDLE_START_NUM = DOORBELL_HANDLE_START_NUM + (DOORBELL_HANDLE_NUM),
    SLAT_HANDLE_START_NUM = FAN_V2_HANDLE_START_NUM + (FAN_V2_HANDLE_NUM),
    FILTER_MAINTENANCE_HANDLE_START_NUM = SLAT_HANDLE_START_NUM + (SLAT_HANDLE_NUM),
    AIR_PURIFIER_HANDLE_START_NUM = FILTER_MAINTENANCE_HANDLE_START_NUM + (FILTER_MAINTENANCE_HANDLE_NUM),
    SERVICE_LABEL_HANDLE_START_NUM = AIR_PURIFIER_HANDLE_START_NUM + (AIR_PURIFIER_HANDLE_NUM),
    HEATERCOOLER_HANDLE_START_NUM = SERVICE_LABEL_HANDLE_START_NUM + (SERVICE_LABEL_HANDLE_NUM),
    HUMIDIFIERDEHUMIDIFIER_HANDLE_START_NUM = HEATERCOOLER_HANDLE_START_NUM + (HEATERCOOLER_HANDLE_NUM),
    IRRIGATIONSYSTEM_HANDLE_START_NUM = HUMIDIFIERDEHUMIDIFIER_HANDLE_START_NUM + (HUMIDIFIERDEHUMIDIFIER_HANDLE_NUM),
    VALVE_HANDLE_START_NUM = IRRIGATIONSYSTEM_HANDLE_START_NUM + (IRRIGATIONSYSTEM_HANDLE_NUM),
	FAUCET_HANDLE_START_NUM = VALVE_HANDLE_START_NUM + (VALVE_HANDLE_NUM),
	OTA_HANDLE_START_NUM = FAUCET_HANDLE_START_NUM + (FAUCET_HANDLE_NUM),
	LAST_HANDLE_NUM = OTA_HANDLE_START_NUM + (OTA_HANDLE_NUM),
};

typedef struct{
    u32 save_flag:5;
    FAN_STORE_CHAR_VALUE_T
    GARAGEDOOROPENER_STORE_CHAR_VALUE_T
    LIGHTBULB_STORE_CHAR_VALUE_T
    LOCKMANAGEMENT_STORE_CHAR_VALUE_T
    LOCKMECHANISM_STORE_CHAR_VALUE_T
    OUTLET_STORE_CHAR_VALUE_T
    SWITCH_STORE_CHAR_VALUE_T
    THERMOSTAT_STORE_CHAR_VALUE_T
    SECURITY_STORE_CHAR_VALUE_T
    DOOR_STORE_CHAR_VALUE_T
    STATEFUL_PROG_SW_STORE_CHAR_VALUE_T
    STATELESS_PROG_SW_STORE_CHAR_VALUE_T
    WINDOW_STORE_CHAR_VALUE_T
    WINDOW_COVERING_STORE_CHAR_VALUE_T
    BATTERY_STORE_CHAR_VALUE_T
    SENSOR_AIR_QUALITY_STORE_CHAR_VALUE_T
    SENSOR_CARBON_MONOXIDE_STORE_CHAR_VALUE_T
    SENSOR_CONTACT_STORE_CHAR_VALUE_T
    SENSOR_HUMIDITY_STORE_CHAR_VALUE_T
    SENSOR_LEAK_STORE_CHAR_VALUE_T
    SENSOR_LIGHT_STORE_CHAR_VALUE_T
    SENSOR_MOTION_STORE_CHAR_VALUE_T
    SENSOR_OCCUPANCY_STORE_CHAR_VALUE_T
    SENSOR_SMOKE_STORE_CHAR_VALUE_T
    SENSOR_TEMPERATURE_STORE_CHAR_VALUE_T
    SENSOR_CARBON_DIOXIDE_STORE_CHAR_VALUE_T
    CAM_RTP_STR_MANAGEMENT_STORE_CHAR_VALUE_T
    MICROPHONE_STORE_CHAR_VALUE_T
    SPEAKER_STORE_CHAR_VALUE_T
    DOORBELL_STORE_CHAR_VALUE_T
    FAN_V2_STORE_CHAR_VALUE_T
    SLAT_STORE_CHAR_VALUE_T
    FILTER_MAINTENANCE_STORE_CHAR_VALUE_T
    AIR_PURIFIER_STORE_CHAR_VALUE_T
    SERVICE_LABEL_STORE_CHAR_VALUE_T
    HEATERCOOLER_STORE_CHAR_VALUE_T
    HUMIDIFIERDEHUMIDIFIER_STORE_CHAR_VALUE_T
    IRRIGATIONSYSTEM_STORE_CHAR_VALUE_T
    VALVE_STORE_CHAR_VALUE_T
    FAUCET_STORE_CHAR_VALUE_T
    OTA_STORE_CHAR_VALUE_T
}store_char_change_value_t;


#define __store_flash_struct_var(flash_struct_var)   (char_save_store.flash_struct_var)
#define STORE_CHAR(flash_struct_var,var) (__store_flash_struct_var(flash_struct_var) = (var));

#define __restore_flash_struct_var(flash_struct_var)   (char_save_restore->flash_struct_var)
#define RESTORE_CHAR(flash_struct_var,var) (var = __restore_flash_struct_var(flash_struct_var));

#define RESTORE_CHAR_FROM_FLASH  \
        LIGHTBULB_RESTORE_CHAR_FROM_FLASH  \
        GARAGEDOOROPENER_RESTORE_CHAR_FROM_FLASH  \
        FAN_RESTORE_CHAR_FROM_FLASH  \
        LOCKMANAGEMENT_RESTORE_CHAR_FROM_FLASH  \
        LOCKMECHANISM_RESTORE_CHAR_FROM_FLASH  \
        OUTLET_RESTORE_CHAR_FROM_FLASH  \
        SWITCH_RESTORE_CHAR_FROM_FLASH  \
        THERMOSTAT_RESTORE_CHAR_FROM_FLASH  \
        SECURITY_RESTORE_CHAR_FROM_FLASH  \
        DOOR_RESTORE_CHAR_FROM_FLASH  \
        STATEFUL_PROG_SW_RESTORE_CHAR_FROM_FLASH  \
        STATELESS_PROG_SW_RESTORE_CHAR_FROM_FLASH  \
        WINDOW_RESTORE_CHAR_FROM_FLASH  \
        WINDOW_COVERING_RESTORE_CHAR_FROM_FLASH  \
        BATTERY_RESTORE_CHAR_FROM_FLASH  \
        SENSOR_AIR_QUALITY_RESTORE_CHAR_FROM_FLASH  \
        SENSOR_CARBON_MONOXIDE_RESTORE_CHAR_FROM_FLASH  \
        SENSOR_CONTACT_RESTORE_CHAR_FROM_FLASH  \
        SENSOR_HUMIDITY_RESTORE_CHAR_FROM_FLASH  \
        SENSOR_LEAK_RESTORE_CHAR_FROM_FLASH  \
        SENSOR_LIGHT_RESTORE_CHAR_FROM_FLASH  \
        SENSOR_MOTION_RESTORE_CHAR_FROM_FLASH  \
        SENSOR_OCCUPANCY_RESTORE_CHAR_FROM_FLASH  \
        SENSOR_SMOKE_RESTORE_CHAR_FROM_FLASH  \
        SENSOR_TEMPERATURE_RESTORE_CHAR_FROM_FLASH  \
        SENSOR_CARBON_DIOXIDE_RESTORE_CHAR_FROM_FLASH  \
        CAM_RTP_STR_MANAGEMENT_RESTORE_CHAR_FROM_FLASH  \
        MICROPHONE_RESTORE_CHAR_FROM_FLASH  \
        SPEAKER_RESTORE_CHAR_FROM_FLASH  \
        DOORBELL_RESTORE_CHAR_FROM_FLASH  \
        FAN_V2_RESTORE_CHAR_FROM_FLASH  \
        SLAT_RESTORE_CHAR_FROM_FLASH  \
        FILTER_MAINTENANCE_RESTORE_CHAR_FROM_FLASH  \
        AIR_PURIFIER_RESTORE_CHAR_FROM_FLASH  \
        SERVICE_LABEL_RESTORE_CHAR_FROM_FLASH  \
        HEATERCOOLER_RESTORE_CHAR_FROM_FLASH  \
        HUMIDIFIERDEHUMIDIFIER_RESTORE_CHAR_FROM_FLASH  \
        IRRIGATIONSYSTEM_RESTORE_CHAR_FROM_FLASH  \
        VALVE_RESTORE_CHAR_FROM_FLASH  \
        FAUCET_RESTORE_CHAR_FROM_FLASH  \
        OTA_RESTORE_CHAR_FROM_FLASH


#define STORE_CHAR_TO_FLASH  \
        char_save_store.save_flag = CHAR_SAVE_FLAG;  \
        LIGHTBULB_STORE_CHAR_TO_FLASH  \
        GARAGEDOOROPENER_STORE_CHAR_TO_FLASH  \
        FAN_STORE_CHAR_TO_FLASH  \
        LOCKMANAGEMENT_STORE_CHAR_TO_FLASH  \
        LOCKMECHANISM_STORE_CHAR_TO_FLASH  \
        OUTLET_STORE_CHAR_TO_FLASH  \
        SWITCH_STORE_CHAR_TO_FLASH  \
        THERMOSTAT_STORE_CHAR_TO_FLASH  \
        SECURITY_STORE_CHAR_TO_FLASH  \
        DOOR_STORE_CHAR_TO_FLASH  \
        STATEFUL_PROG_SW_STORE_CHAR_TO_FLASH  \
        STATELESS_PROG_SW_STORE_CHAR_TO_FLASH  \
        WINDOW_STORE_CHAR_TO_FLASH  \
        WINDOW_COVERING_STORE_CHAR_TO_FLASH  \
        BATTERY_STORE_CHAR_TO_FLASH  \
        SENSOR_AIR_QUALITY_STORE_CHAR_TO_FLASH  \
        SENSOR_CARBON_MONOXIDE_STORE_CHAR_TO_FLASH  \
        SENSOR_CONTACT_STORE_CHAR_TO_FLASH  \
        SENSOR_HUMIDITY_STORE_CHAR_TO_FLASH  \
        SENSOR_LEAK_STORE_CHAR_TO_FLASH  \
        SENSOR_LIGHT_STORE_CHAR_TO_FLASH  \
        SENSOR_MOTION_STORE_CHAR_TO_FLASH  \
        SENSOR_OCCUPANCY_STORE_CHAR_TO_FLASH  \
        SENSOR_SMOKE_STORE_CHAR_TO_FLASH  \
        SENSOR_TEMPERATURE_STORE_CHAR_TO_FLASH  \
        SENSOR_CARBON_DIOXIDE_STORE_CHAR_TO_FLASH  \
        CAM_RTP_STR_MANAGEMENT_STORE_CHAR_TO_FLASH  \
        MICROPHONE_STORE_CHAR_TO_FLASH  \
        SPEAKER_STORE_CHAR_TO_FLASH  \
        DOORBELL_STORE_CHAR_TO_FLASH  \
        FAN_V2_STORE_CHAR_TO_FLASH  \
        SLAT_STORE_CHAR_TO_FLASH  \
        FILTER_MAINTENANCE_STORE_CHAR_TO_FLASH  \
        AIR_PURIFIER_STORE_CHAR_TO_FLASH  \
        SERVICE_LABEL_STORE_CHAR_TO_FLASH  \
        HEATERCOOLER_STORE_CHAR_TO_FLASH  \
        HUMIDIFIERDEHUMIDIFIER_STORE_CHAR_TO_FLASH  \
        IRRIGATIONSYSTEM_STORE_CHAR_TO_FLASH  \
        VALVE_STORE_CHAR_TO_FLASH  \
        FAUCET_STORE_CHAR_TO_FLASH  \
        OTA_STORE_CHAR_TO_FLASH


//////////////////////////////////////////////////////////
#define HOMEKIT_NO  \
        HOMEKIT_INFO_SERVICE_NO  \
        +HOMEKIT_ACCESSORY_PROTOCOL_NO  \
        +HOMEKIT_HAP_BLE_PROTOCOL_INFO_NO  \
        +FAN_NO                      +FAN_NO_DELTA  \
        +GARAGEDOOROPENER_NO         +GARAGEDOOROPENER_NO_DELTA  \
        +LIGHTBULB_NO                +LIGHTBULB_NO_DELTA  \
        +LOCKMANAGEMENT_NO           +LOCKMANAGEMENT_NO_DELTA  \
        +LOCKMECHANISM_NO            +LOCKMECHANISM_NO_DELTA  \
        +OUTLET_NO                   +OUTLET_NO_DELTA  \
        +SWITCH_NO                   +SWITCH_NO_DELTA  \
        +THERMOSTAT_NO               +THERMOSTAT_NO_DELTA  \
        +SECURITY_NO                 +SECURITY_NO_DELTA  \
        +DOOR_NO                     +DOOR_NO_DELTA  \
        +STATEFUL_PROG_SW_NO         +STATEFUL_PROG_SW_NO_DELTA  \
        +STATELESS_PROG_SW_NO        +STATELESS_PROG_SW_NO_DELTA  \
        +WINDOW_NO                   +WINDOW_NO_DELTA  \
        +WINDOW_COVERING_NO          +WINDOW_COVERING_NO_DELTA  \
        +BATTERY_NO                  +BATTERY_NO_DELTA  \
        +SENSOR_AIR_QUALITY_NO       +SENSOR_AIR_QUALITY_NO_DELTA  \
        +SENSOR_CARBON_MONOXIDE_NO   +SENSOR_CARBON_MONOXIDE_NO_DELTA  \
        +SENSOR_CONTACT_NO           +SENSOR_CONTACT_NO_DELTA  \
        +SENSOR_HUMIDITY_NO          +SENSOR_HUMIDITY_NO_DELTA  \
        +SENSOR_LEAK_NO              +SENSOR_LEAK_NO_DELTA  \
        +SENSOR_LIGHT_NO             +SENSOR_LIGHT_NO_DELTA  \
        +SENSOR_MOTION_NO            +SENSOR_MOTION_NO_DELTA  \
        +SENSOR_OCCUPANCY_NO         +SENSOR_OCCUPANCY_NO_DELTA  \
        +SENSOR_SMOKE_NO             +SENSOR_SMOKE_NO_DELTA  \
        +SENSOR_TEMPERATURE_NO       +SENSOR_TEMPERATURE_NO_DELTA  \
        +SENSOR_CARBON_DIOXIDE_NO    +SENSOR_CARBON_DIOXIDE_NO_DELTA  \
        +CAM_RTP_STR_MANAGEMENT_NO   +CAM_RTP_STR_MANAGEMENT_NO_DELTA  \
        +MICROPHONE_NO               +MICROPHONE_NO_DELTA  \
        +SPEAKER_NO                  +SPEAKER_NO_DELTA  \
        +DOORBELL_NO                 +DOORBELL_NO_DELTA  \
        +FAN_V2_NO                   +FAN_V2_NO_DELTA  \
        +SLAT_NO                     +SLAT_NO_DELTA  \
        +FILTER_MAINTENANCE_NO       +FILTER_MAINTENANCE_NO_DELTA  \
        +AIR_PURIFIER_NO             +AIR_PURIFIER_NO_DELTA  \
        +SERVICE_LABEL_NO            +SERVICE_LABEL_NO_DELTA  \
        +HEATERCOOLER_NO             +HEATERCOOLER_NO_DELTA  \
        +HUMIDIFIERDEHUMIDIFIER_NO   +HUMIDIFIERDEHUMIDIFIER_NO_DELTA  \
        +IRRIGATIONSYSTEM_NO         +IRRIGATIONSYSTEM_NO_DELTA  \
        +VALVE_NO                    +VALVE_NO_DELTA  \
        +FAUCET_NO                   +FAUCET_NO_DELTA  \
        +OTA_NO                      +OTA_NO_DELTA  \

#define HOMEKIT_TOP_ATT_TABLE \
        HOMEKIT_INFORMATION_SERVICE_ATT_TABLE  \
        HOMEKIT_ACCESSORY_PROTOCOL_ATT_TABLE  \
        HOMEKIT_HAP_BLE_PROTOCOL_INFORMATION_TABLE  \
        HOMEKIT_FAN_SERVICE_ATT_TABLE  \
        HOMEKIT_GARAGEDOOROPENER_SERVICE_ATT_TABLE  \
        HOMEKIT_LIGHTBULB_SERVICE_ATT_TABLE  \
        HOMEKIT_LOCKMANAGEMENT_SERVICE_ATT_TABLE  \
        HOMEKIT_LOCKMECHANISM_SERVICE_ATT_TABLE  \
        HOMEKIT_OUTLET_SERVICE_ATT_TABLE  \
        HOMEKIT_SWITCH_SERVICE_ATT_TABLE  \
        HOMEKIT_THERMOSTAT_SERVICE_ATT_TABLE  \
        HOMEKIT_SECURITY_SERVICE_ATT_TABLE  \
        HOMEKIT_DOOR_SERVICE_ATT_TABLE  \
        HOMEKIT_STATEFUL_PROG_SW_SERVICE_ATT_TABLE  \
        HOMEKIT_STATELESS_PROG_SW_SERVICE_ATT_TABLE  \
        HOMEKIT_WINDOW_SERVICE_ATT_TABLE  \
        HOMEKIT_WINDOW_COVERING_SERVICE_ATT_TABLE  \
        HOMEKIT_BATTERY_SERVICE_ATT_TABLE  \
        HOMEKIT_SENSOR_AIR_QUALITY_SERVICE_ATT_TABLE  \
        HOMEKIT_SENSOR_CARBON_MONOXIDE_SERVICE_ATT_TABLE  \
        HOMEKIT_SENSOR_CONTACT_SERVICE_ATT_TABLE  \
        HOMEKIT_SENSOR_HUMIDITY_SERVICE_ATT_TABLE  \
        HOMEKIT_SENSOR_LEAK_SERVICE_ATT_TABLE  \
        HOMEKIT_SENSOR_LIGHT_SERVICE_ATT_TABLE  \
        HOMEKIT_SENSOR_MOTION_SERVICE_ATT_TABLE  \
        HOMEKIT_SENSOR_OCCUPANCY_SERVICE_ATT_TABLE  \
        HOMEKIT_SENSOR_SMOKE_SERVICE_ATT_TABLE  \
        HOMEKIT_SENSOR_TEMPERATURE_SERVICE_ATT_TABLE  \
        HOMEKIT_SENSOR_CARBON_DIOXIDE_SERVICE_ATT_TABLE  \
        HOMEKIT_CAM_RTP_STR_MANAGEMENT_SERVICE_ATT_TABLE  \
        HOMEKIT_MICROPHONE_SERVICE_ATT_TABLE  \
        HOMEKIT_SPEAKER_SERVICE_ATT_TABLE  \
        HOMEKIT_DOORBELL_SERVICE_ATT_TABLE  \
        HOMEKIT_FAN_V2_SERVICE_ATT_TABLE  \
        HOMEKIT_SLAT_SERVICE_ATT_TABLE  \
        HOMEKIT_FILTER_MAINTENANCE_SERVICE_ATT_TABLE  \
        HOMEKIT_AIR_PURIFIER_SERVICE_ATT_TABLE  \
        HOMEKIT_SERVICE_LABEL_SERVICE_ATT_TABLE  \
        HOMEKIT_HEATERCOOLER_SERVICE_ATT_TABLE  \
        HOMEKIT_HUMIDIFIERDEHUMIDIFIER_SERVICE_ATT_TABLE  \
        HOMEKIT_IRRIGATIONSYSTEM_SERVICE_ATT_TABLE  \
        HOMEKIT_VALVE_SERVICE_ATT_TABLE  \
        HOMEKIT_FAUCET_SERVICE_ATT_TABLE  \
        HOMEKIT_OTA_SERVICE_ATT_TABLE  \


/* ------------------------------------
    Type Definitions
   ------------------------------------ */
#define MAC_ADDR_RANDOM_ENABLE          0
#define SERIAL_NUMBER_RANDOM_ENABLE     0
#define SETUP_ID_RANDOM_ENABLE          0

#define SETUP_CODE_RANDOM_ENABLE        0
#define SETUP_CODE_WRITE_CNTS_MAX       2	//max = 0xFF = 255
#define SETUP_CODE_READ_CNTS_MAX        2	//max = (4096 - 8)/4 = 1022


/* ------------------------------------
    Variables Definitions/Declarations
   ------------------------------------ */

EXTERN const u8 manufacturer[19];
EXTERN const u8 model[24];
EXTERN const u8 name[20];
EXTERN const u8 my_OtaName[3];
EXTERN const u8 spp_pairname[4];
EXTERN const u8 my_devName[20];
EXTERN const u8 hap_ble_version[5];
EXTERN const u8 firmware_revision[6];
EXTERN const u8 ServiceChangedRange[4];

EXTERN const u16 insts[100];


#if SERIAL_NUMBER_RANDOM_ENABLE
EXTERN u8 serialNumber[10];
#else
EXTERN const u8 serialNumber[9];
#endif

#if SETUP_CODE_RANDOM_ENABLE
EXTERN u32 SetupCodeData;
EXTERN u8 SetupCodeWriteCnts;
EXTERN u8 SetupCodeReadCnts;
#endif

EXTERN u32 SetupIdData;

/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
void assign_current_flash_char_value_addr(void);
void blt_adv_set_global_state ();
void store_char_value_to_flash(void);
#include "homekit_src/hk_indicate_queue.h"



#endif /* _HK_TOP_UUID_H_ */

/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
