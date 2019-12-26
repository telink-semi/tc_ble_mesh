/********************************************************************************************************
 * @file     hk_SensorAirQualityService.c
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
#define _HK_SENSOR_AIR_QUALITY_SERVICE_C_


/* ------------------------------------
    Header Files
   ------------------------------------ */
#include "tl_common.h"
#include "stack/ble/ble.h"
#include "hk_SensorAirQualityService.h"
#include "ble_UUID.h"
#include "hk_TopUUID.h"

extern u32 session_timeout_tick;
extern u8 EventNotifyCNT;
extern store_char_change_value_t* char_save_restore;


/* ------------------------------------
    Macro Definitions
   ------------------------------------ */


/* ------------------------------------
    Type Definitions
   ------------------------------------ */


/* ------------------------------------
    Variables Definitions
   ------------------------------------ */
unsigned char hSensor_Air_Quality_Indicator = 0;
unsigned int hSensor_Air_Quality_OzoneDensity = 0;
unsigned int hSensor_Air_Quality_NitrogenDioxideDensity = 0;
unsigned int hSensor_Air_Quality_SulphurDioxideDensity = 0;
unsigned int hSensor_Air_Quality_PM2_5Density = 0;
unsigned int hSensor_Air_Quality_PM10Density = 0;
unsigned int hSensor_Air_Quality_VOCDensity = 0;
unsigned char hSensor_Air_Quality_StatusActive = 0;
unsigned char hSensor_Air_Quality_StatusFault = 0;
unsigned char hSensor_Air_Quality_StatusTampered = 0;
unsigned char hSensor_Air_Quality_StatusLowBattery = 0;

unsigned char VAR_SENSOR_AIR_QUALITY_UNKOWN = 0;

unsigned short ccc_hSensorAirQuality_Indicator = 0;
unsigned short ccc_hSensorAirQuality_OzoneDensity = 0;
unsigned short ccc_hSensorAirQuality_NitrogenDioxideDensity = 0;
unsigned short ccc_hSensorAirQuality_SulphurDioxideDensity = 0;
unsigned short ccc_hSensorAirQuality_PM2_5Density = 0;
unsigned short ccc_hSensorAirQuality_PM10Density = 0;
unsigned short ccc_hSensorAirQuality_VOCDensity = 0;
unsigned short ccc_hSensorAirQuality_StatusActive = 0;
unsigned short ccc_hSensorAirQuality_StatusFault = 0;
unsigned short ccc_hSensorAirQuality_StatusTampered = 0;
unsigned short ccc_hSensorAirQuality_StatusLowBattery = 0;

#if SENSORAIRQUALITY_INDICATOR_BRC_ENABLE
    unsigned char bc_para_hSensorAirQuality_Indicator = 1;
#endif

#if SENSORAIRQUALITY_OZONE_DENSITY_BRC_ENABLE
    unsigned char bc_para_hSensorAirQuality_OzoneDensity = 1;
#endif

#if SENSORAIRQUALITY_NITROGEN_DIOXIDE_DENSITY_BRC_ENABLE
    unsigned char bc_para_hSensorAirQuality_NitrogenDioxideDensity = 1;
#endif

#if SENSORAIRQUALITY_SULPHUR_DIOXIDE_DENSITY_BRC_ENABLE
    unsigned char bc_para_hSensorAirQuality_SulphurDioxideDensity = 1;
#endif

#if SENSORAIRQUALITY_PM2_5_DENSITY_BRC_ENABLE
    unsigned char bc_para_hSensorAirQuality_PM2_5Density = 1;
#endif

#if SENSORAIRQUALITY_PM10_DENSITY_BRC_ENABLE
    unsigned char bc_para_hSensorAirQuality_PM10Density = 1;
#endif

#if SENSORAIRQUALITY_VOC_DENSITY_BRC_ENABLE
    unsigned char bc_para_hSensorAirQuality_VOCDensity = 1;
#endif

#if SENSORAIRQUALITY_STATUSACTIVE_BRC_ENABLE
    unsigned char bc_para_hSensorAirQuality_StatusActive = 1;
#endif

#if SENSORAIRQUALITY_STATUSFAULT_BRC_ENABLE
    unsigned char bc_para_hSensorAirQuality_StatusFault = 1;
#endif

#if SENSORAIRQUALITY_STATUSTAMPERED_BRC_ENABLE
    unsigned char bc_para_hSensorAirQuality_StatusTampered = 1;
#endif

#if SENSORAIRQUALITY_STATUSLOWBATTERY_BRC_ENABLE
    unsigned char bc_para_hSensorAirQuality_StatusLowBattery = 1;
#endif

const u8 hSensor_Air_QualityServiceName[25] = "Sensor_Air_QualityService";

const u8 VALID_RANGE_hSensor_Air_Quality_Indicator[2] = { 0x00, 0x05};
const u8 VALID_RANGE_hSensor_Air_Quality_OzoneDensity[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7A, 0x44};    // 0~1000 (float)
const u8 VALID_RANGE_hSensor_Air_Quality_NitrogenDioxideDensity[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7A, 0x44};    // 0~1000 (float)
const u8 VALID_RANGE_hSensor_Air_Quality_SulphurDioxideDensity[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7A, 0x44};    // 0~1000 (float)
const u8 VALID_RANGE_hSensor_Air_Quality_PM2_5Density[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7A, 0x44};    // 0~1000 (float)
const u8 VALID_RANGE_hSensor_Air_Quality_PM10Density[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7A, 0x44};    // 0~1000 (float)
const u8 VALID_RANGE_hSensor_Air_Quality_VOCDensity[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7A, 0x44};    // 0~1000 (float)
const u8 VALID_RANGE_hSensor_Air_Quality_StatusFault[2] = { 0x00, 0x01};
const u8 VALID_RANGE_hSensor_Air_Quality_StatusTampered[2] = { 0x00, 0x01};
const u8 VALID_RANGE_hSensor_Air_Quality_StatusLowBattery[2] = { 0x00, 0x01};

const u8 VALID_STEP_hSensor_Air_Quality_Indicator = 0x01;
const u8 VALID_STEP_hSensor_Air_Quality_StatusFault = 0x01;
const u8 VALID_STEP_hSensor_Air_Quality_StatusTampered = 0x01;
const u8 VALID_STEP_hSensor_Air_Quality_StatusLowBattery = 0x01;

const hap_characteristic_desc_t hap_desc_char_hSensor_Air_Quality_Indicator_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSensor_Air_Quality_Indicator, &VALID_STEP_hSensor_Air_Quality_Indicator, BC_PARA_SENSORAIRQUALITY_INDICATOR};
const hap_characteristic_desc_t hap_desc_char_hSensor_Air_Quality_OzoneDensity_R_S = {hap_PRead_NOTIFY, 0, CFG_FLOAT32, 0, 8, 0, 0, 0, VALID_RANGE_hSensor_Air_Quality_OzoneDensity, 0, BC_PARA_SENSORAIRQUALITY_OZONE_DENSITY};
const hap_characteristic_desc_t hap_desc_char_hSensor_Air_Quality_NitrogenDioxideDensity_R_S = {hap_PRead_NOTIFY, 0, CFG_FLOAT32, 0, 8, 0, 0, 0, VALID_RANGE_hSensor_Air_Quality_NitrogenDioxideDensity, 0, BC_PARA_SENSORAIRQUALITY_NITROGEN_DIOXIDE_DENSITY};
const hap_characteristic_desc_t hap_desc_char_hSensor_Air_Quality_SulphurDioxideDensity_R_S = {hap_PRead_NOTIFY, 0, CFG_FLOAT32, 0, 8, 0, 0, 0, VALID_RANGE_hSensor_Air_Quality_SulphurDioxideDensity, 0, BC_PARA_SENSORAIRQUALITY_SULPHUR_DIOXIDE_DENSITY};
const hap_characteristic_desc_t hap_desc_char_hSensor_Air_Quality_PM2_5Density_R_S = {hap_PRead_NOTIFY, 0, CFG_FLOAT32, 0, 8, 0, 0, 0, VALID_RANGE_hSensor_Air_Quality_PM2_5Density, 0, BC_PARA_SENSORAIRQUALITY_PM2_5_DENSITY};
const hap_characteristic_desc_t hap_desc_char_hSensor_Air_Quality_PM10Density_R_S = {hap_PRead_NOTIFY, 0, CFG_FLOAT32, 0, 8, 0, 0, 0, VALID_RANGE_hSensor_Air_Quality_PM10Density, 0, BC_PARA_SENSORAIRQUALITY_PM10_DENSITY};
const hap_characteristic_desc_t hap_desc_char_hSensor_Air_Quality_VOCDensity_R_S = {hap_PRead_NOTIFY, 0, CFG_FLOAT32, 0, 8, 0, 0, 0, VALID_RANGE_hSensor_Air_Quality_VOCDensity, 0, BC_PARA_SENSORAIRQUALITY_VOC_DENSITY};
const hap_characteristic_desc_t hap_desc_char_hSensor_Air_Quality_StatusActive_R_S = {hap_PRead_NOTIFY, 0, CFG_BOOL_UNITLESS, 0, 0, 0, 0, 0, 0, 0, BC_PARA_SENSORAIRQUALITY_STATUSACTIVE};
const hap_characteristic_desc_t hap_desc_char_hSensor_Air_Quality_StatusFault_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSensor_Air_Quality_StatusFault, &VALID_STEP_hSensor_Air_Quality_StatusFault, BC_PARA_SENSORAIRQUALITY_STATUSFAULT};
const hap_characteristic_desc_t hap_desc_char_hSensor_Air_Quality_StatusTampered_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSensor_Air_Quality_StatusTampered, &VALID_STEP_hSensor_Air_Quality_StatusTampered, BC_PARA_SENSORAIRQUALITY_STATUSTAMPERED};
const hap_characteristic_desc_t hap_desc_char_hSensor_Air_Quality_StatusLowBattery_R_S = {hap_PRead_NOTIFY, 0, CFG_UINT8, 0, 2, 1, 0, 0, VALID_RANGE_hSensor_Air_Quality_StatusTampered, &VALID_STEP_hSensor_Air_Quality_StatusLowBattery, BC_PARA_SENSORAIRQUALITY_STATUSLOWBATTERY};

const hap_characteristic_desc_t hap_desc_char_sensor_air_quality_A5_R_S = {hap_SecureRW_30, 0, CFG_DATA, 0, 0, 0, 0, 0, 0, 0, 0};

#if SENSOR_AIR_QUALITY_LINKED_SERVICES_ENABLE
/*fill the array with the pointer of linked services instance id.*/
const u16 * hSensorAirQuality_linked_svcs[] = {};
#endif

#if(SENSOR_AIR_QUALITY_IS_HIDDEN_SERVICE == 1)
const hap_service_desc_t hap_desc_service_sensor_air_quality_A5_R_S = {SENSOR_AIR_QUALITY_INST_NO+1, (0x02 + SENSOR_AIR_QUALITY_AS_PRIMARY_SERVICE), SENSOR_AIR_QUALITY_LINKED_SVCS_LENGTH, SENSOR_AIR_QUALITY_LINKED_SVCS};
#else
const hap_service_desc_t hap_desc_service_sensor_air_quality_A5_R_S = {SENSOR_AIR_QUALITY_INST_NO+1, (0x00 + SENSOR_AIR_QUALITY_AS_PRIMARY_SERVICE), SENSOR_AIR_QUALITY_LINKED_SVCS_LENGTH, SENSOR_AIR_QUALITY_LINKED_SVCS};
#endif


/* ------------------------------------
    Function Prototypes
   ------------------------------------ */
int hSensor_Air_Quality_IndicatorReadCallback(void *pp)
{
    if ((hSensor_Air_Quality_Indicator <= 5) && (hSensor_Air_Quality_Indicator >= 0)) {  // 0~5
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_AIR_QUALITY_SERVICE_ENABLE
        SENSOR_AIR_QUALITY_STORE_CHAR_VALUE_T_SENSOR_AIR_QUALITY_INDICATOR_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Air_Quality_OzoneDensityReadCallback(void *pp)
{
    if ((hSensor_Air_Quality_OzoneDensity <= 0x447A0000) && (hSensor_Air_Quality_OzoneDensity >= 0)) {  // 0~1000
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_AIR_QUALITY_SERVICE_ENABLE
        SENSOR_AIR_QUALITY_STORE_CHAR_VALUE_T_SENSOR_AIR_QUALITY_OZONE_DENSITY_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Air_Quality_NitrogenDioxideDensityReadCallback(void *pp)
{
    if ((hSensor_Air_Quality_NitrogenDioxideDensity <= 0x447A0000) && (hSensor_Air_Quality_NitrogenDioxideDensity >= 0)) {  // 0~1000
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_AIR_QUALITY_SERVICE_ENABLE
        SENSOR_AIR_QUALITY_STORE_CHAR_VALUE_T_SENSOR_AIR_QUALITY_NITROGEN_DIOXIDE_DENSITY_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Air_Quality_SulphurDioxideDensityReadCallback(void *pp)
{
    if ((hSensor_Air_Quality_SulphurDioxideDensity <= 0x447A0000) && (hSensor_Air_Quality_SulphurDioxideDensity >= 0)) {  // 0~1000
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_AIR_QUALITY_SERVICE_ENABLE
        SENSOR_AIR_QUALITY_STORE_CHAR_VALUE_T_SENSOR_AIR_QUALITY_SULPHUR_DIOXIDE_DENSITY_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Air_Quality_PM2_5DensityReadCallback(void *pp)
{
    if ((hSensor_Air_Quality_PM2_5Density <= 0x447A0000) && (hSensor_Air_Quality_PM2_5Density >= 0)) {  // 0~1000
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_AIR_QUALITY_SERVICE_ENABLE
        SENSOR_AIR_QUALITY_STORE_CHAR_VALUE_T_SENSOR_AIR_QUALITY_PM2_5_DENSITY_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Air_Quality_PM10DensityReadCallback(void *pp)
{
    if ((hSensor_Air_Quality_PM10Density <= 0x447A0000) && (hSensor_Air_Quality_PM10Density >= 0)) {  // 0~1000
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_AIR_QUALITY_SERVICE_ENABLE
        SENSOR_AIR_QUALITY_STORE_CHAR_VALUE_T_SENSOR_AIR_QUALITY_PM10_DENSITY_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Air_Quality_VOCDensityReadCallback(void *pp)
{
    if ((hSensor_Air_Quality_VOCDensity <= 0x447A0000) && (hSensor_Air_Quality_VOCDensity >= 0)) {  // 0~1000
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_AIR_QUALITY_SERVICE_ENABLE
        SENSOR_AIR_QUALITY_STORE_CHAR_VALUE_T_SENSOR_AIR_QUALITY_VOC_DENSITY_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Air_Quality_StatusActiveReadCallback(void *pp)
{
    if ((hSensor_Air_Quality_StatusActive <= 1) && (hSensor_Air_Quality_StatusActive >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_AIR_QUALITY_SERVICE_ENABLE
        SENSOR_AIR_QUALITY_STORE_CHAR_VALUE_T_SENSOR_AIR_QUALITY_STATUSACTIVE_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Air_Quality_StatusFaultReadCallback(void *pp)
{
    if ((hSensor_Air_Quality_StatusFault <= 1) && (hSensor_Air_Quality_StatusFault >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_AIR_QUALITY_SERVICE_ENABLE
        SENSOR_AIR_QUALITY_STORE_CHAR_VALUE_T_SENSOR_AIR_QUALITY_STATUSFAULT_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Air_Quality_StatusTamperedReadCallback(void *pp)
{
    if ((hSensor_Air_Quality_StatusTampered <= 1) && (hSensor_Air_Quality_StatusTampered >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_AIR_QUALITY_SERVICE_ENABLE
        SENSOR_AIR_QUALITY_STORE_CHAR_VALUE_T_SENSOR_AIR_QUALITY_STATUSTAMPERED_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int hSensor_Air_Quality_StatusLowBatteryReadCallback(void *pp)
{
    if ((hSensor_Air_Quality_StatusLowBattery <= 1) && (hSensor_Air_Quality_StatusLowBattery >= 0)) {  // 0~1
        store_char_value_to_flash();
    } else {
        assign_current_flash_char_value_addr();
    #if SENSOR_AIR_QUALITY_SERVICE_ENABLE
        SENSOR_AIR_QUALITY_STORE_CHAR_VALUE_T_SENSOR_AIR_QUALITY_STATUSLOWBATTERY_RESTORE
    #endif
    }

    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorAirQuality_IndicatorWriteCB(void *pp)
{
    static u16 ccc_hSensorAirQuality_Indicator_pre = 0;

    if(ccc_hSensorAirQuality_Indicator_pre != ccc_hSensorAirQuality_Indicator)
    {
        if(ccc_hSensorAirQuality_Indicator == 0)
        {
            ccc_hSensorAirQuality_Indicator_pre = ccc_hSensorAirQuality_Indicator;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORAIRQUALITY_HANDLE_NUM_INDICATOR);
        }
        else
        {
            ccc_hSensorAirQuality_Indicator_pre = ccc_hSensorAirQuality_Indicator;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORAIRQUALITY_HANDLE_NUM_INDICATOR);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorAirQuality_OzoneDensityWriteCB(void *pp)
{
    static u16 ccc_hSensorAirQuality_OzoneDensity_pre = 0;

    if(ccc_hSensorAirQuality_OzoneDensity_pre != ccc_hSensorAirQuality_OzoneDensity)
    {
        if(ccc_hSensorAirQuality_OzoneDensity == 0)
        {
            ccc_hSensorAirQuality_OzoneDensity_pre = ccc_hSensorAirQuality_OzoneDensity;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORAIRQUALITY_HANDLE_NUM_OZONE_DENSITY);
        }
        else
        {
            ccc_hSensorAirQuality_OzoneDensity_pre = ccc_hSensorAirQuality_OzoneDensity;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORAIRQUALITY_HANDLE_NUM_OZONE_DENSITY);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorAirQuality_NitrogenDioxideDensityWriteCB(void *pp)
{
    static u16 ccc_hSensorAirQuality_NitrogenDioxideDensity_pre = 0;

    if(ccc_hSensorAirQuality_NitrogenDioxideDensity_pre != ccc_hSensorAirQuality_NitrogenDioxideDensity)
    {
        if(ccc_hSensorAirQuality_NitrogenDioxideDensity == 0)
        {
            ccc_hSensorAirQuality_NitrogenDioxideDensity_pre = ccc_hSensorAirQuality_NitrogenDioxideDensity;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORAIRQUALITY_HANDLE_NUM_NITROGEN_DIOXIDE_DENSITY);
        }
        else
        {
            ccc_hSensorAirQuality_NitrogenDioxideDensity_pre = ccc_hSensorAirQuality_NitrogenDioxideDensity;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORAIRQUALITY_HANDLE_NUM_NITROGEN_DIOXIDE_DENSITY);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorAirQuality_SulphurDioxideDensityWriteCB(void *pp)
{
    static u16 ccc_hSensorAirQuality_SulphurDioxideDensity_pre = 0;

    if(ccc_hSensorAirQuality_SulphurDioxideDensity_pre != ccc_hSensorAirQuality_SulphurDioxideDensity)
    {
        if(ccc_hSensorAirQuality_SulphurDioxideDensity == 0)
        {
            ccc_hSensorAirQuality_SulphurDioxideDensity_pre = ccc_hSensorAirQuality_SulphurDioxideDensity;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORAIRQUALITY_HANDLE_NUM_SULPHUR_DIOXIDE_DENSITY);
        }
        else
        {
            ccc_hSensorAirQuality_SulphurDioxideDensity_pre = ccc_hSensorAirQuality_SulphurDioxideDensity;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORAIRQUALITY_HANDLE_NUM_SULPHUR_DIOXIDE_DENSITY);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorAirQuality_PM2_5DensityWriteCB(void *pp)
{
    static u16 ccc_hSensorAirQuality_PM2_5Density_pre = 0;

    if(ccc_hSensorAirQuality_PM2_5Density_pre != ccc_hSensorAirQuality_PM2_5Density)
    {
        if(ccc_hSensorAirQuality_PM2_5Density == 0)
        {
            ccc_hSensorAirQuality_PM2_5Density_pre = ccc_hSensorAirQuality_PM2_5Density;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORAIRQUALITY_HANDLE_NUM_PM2_5_DENSITY);
        }
        else
        {
            ccc_hSensorAirQuality_PM2_5Density_pre = ccc_hSensorAirQuality_PM2_5Density;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORAIRQUALITY_HANDLE_NUM_PM2_5_DENSITY);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorAirQuality_PM10DensityWriteCB(void *pp)
{
    static u16 ccc_hSensorAirQuality_PM10Density_pre = 0;

    if(ccc_hSensorAirQuality_PM10Density_pre != ccc_hSensorAirQuality_PM10Density)
    {
        if(ccc_hSensorAirQuality_PM10Density == 0)
        {
            ccc_hSensorAirQuality_PM10Density_pre = ccc_hSensorAirQuality_PM10Density;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORAIRQUALITY_HANDLE_NUM_PM10_DENSITY);
        }
        else
        {
            ccc_hSensorAirQuality_PM10Density_pre = ccc_hSensorAirQuality_PM10Density;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORAIRQUALITY_HANDLE_NUM_PM10_DENSITY);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorAirQuality_VOCDensityWriteCB(void *pp)
{
    static u16 ccc_hSensorAirQuality_VOCDensity_pre = 0;

    if(ccc_hSensorAirQuality_VOCDensity_pre != ccc_hSensorAirQuality_VOCDensity)
    {
        if(ccc_hSensorAirQuality_VOCDensity == 0)
        {
            ccc_hSensorAirQuality_VOCDensity_pre = ccc_hSensorAirQuality_VOCDensity;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORAIRQUALITY_HANDLE_NUM_VOC_DENSITY);
        }
        else
        {
            ccc_hSensorAirQuality_VOCDensity_pre = ccc_hSensorAirQuality_VOCDensity;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORAIRQUALITY_HANDLE_NUM_VOC_DENSITY);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorAirQuality_StatusActiveWriteCB(void *pp)
{
    static u16 ccc_hSensorAirQuality_StatusActive_pre = 0;

    if(ccc_hSensorAirQuality_StatusActive_pre != ccc_hSensorAirQuality_StatusActive)
    {
        if(ccc_hSensorAirQuality_StatusActive == 0)
        {
            ccc_hSensorAirQuality_StatusActive_pre = ccc_hSensorAirQuality_StatusActive;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORAIRQUALITY_HANDLE_NUM_STATUSACTIVE);
        }
        else
        {
            ccc_hSensorAirQuality_StatusActive_pre = ccc_hSensorAirQuality_StatusActive;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORAIRQUALITY_HANDLE_NUM_STATUSACTIVE);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorAirQuality_StatusFaultWriteCB(void *pp)
{
    static u16 ccc_hSensorAirQuality_StatusFault_pre = 0;

    if(ccc_hSensorAirQuality_StatusFault_pre != ccc_hSensorAirQuality_StatusFault)
    {
        if(ccc_hSensorAirQuality_StatusFault == 0)
        {
            ccc_hSensorAirQuality_StatusFault_pre = ccc_hSensorAirQuality_StatusFault;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORAIRQUALITY_HANDLE_NUM_STATUSFAULT);
        }
        else
        {
            ccc_hSensorAirQuality_StatusFault_pre = ccc_hSensorAirQuality_StatusFault;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORAIRQUALITY_HANDLE_NUM_STATUSFAULT);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorAirQuality_StatusTamperedWriteCB(void *pp)
{
    static u16 ccc_hSensorAirQuality_StatusTampered_pre = 0;

    if(ccc_hSensorAirQuality_StatusTampered_pre != ccc_hSensorAirQuality_StatusTampered)
    {
        if(ccc_hSensorAirQuality_StatusTampered == 0)
        {
            ccc_hSensorAirQuality_StatusTampered_pre = ccc_hSensorAirQuality_StatusTampered;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORAIRQUALITY_HANDLE_NUM_STATUSTAMPERED);
        }
        else
        {
            ccc_hSensorAirQuality_StatusTampered_pre = ccc_hSensorAirQuality_StatusTampered;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORAIRQUALITY_HANDLE_NUM_STATUSTAMPERED);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}

int ccc_hSensorAirQuality_StatusLowBatteryWriteCB(void *pp)
{
    static u16 ccc_hSensorAirQuality_StatusLowBattery_pre = 0;

    if(ccc_hSensorAirQuality_StatusLowBattery_pre != ccc_hSensorAirQuality_StatusLowBattery)
    {
        if(ccc_hSensorAirQuality_StatusLowBattery == 0)
        {
            ccc_hSensorAirQuality_StatusLowBattery_pre = ccc_hSensorAirQuality_StatusLowBattery;
            EventNotifyCNT--;
            indicate_handle_no_remove(SENSORAIRQUALITY_HANDLE_NUM_STATUSLOWBATTERY);
        }
        else
        {
            ccc_hSensorAirQuality_StatusLowBattery_pre = ccc_hSensorAirQuality_StatusLowBattery;
            EventNotifyCNT++;
            indicate_handle_no_reg(SENSORAIRQUALITY_HANDLE_NUM_STATUSLOWBATTERY);
        }
    }
    session_timeout_tick = clock_time () | 1;
    return 1;
}


/* %% End Of File %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
