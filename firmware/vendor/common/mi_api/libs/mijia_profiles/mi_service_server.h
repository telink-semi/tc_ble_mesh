/* Copyright (c) 2010-2017 Xiaomi. All Rights Reserved.
 *
 * The information contained herein is property of Xiaomi.
 * Terms and conditions of usage are described in detail in 
 * STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/**@file
 *
 * @defgroup ble_mi Xiaomi Service
 * @{
 * @ingroup  ble_sdk_srv
 * @brief    Xiaomi Service implementation.
 *
 * @details The Xiaomi Service is a simple GATT-based service with many characteristics.
 *          Data received from the peer is passed to the application, and the data
 *          from the application of this service is sent to the peer as Handle Value
 *          Notifications. This module demonstrates how to implement a custom GATT-based
 *          service and characteristics using the BLE Stack. The service
 *          is used by the application to send and receive pub_key and MSC Cert from the
 *          peer.
 *
 * @note The application must propagate BLE Stack events to the Xiaomi Service module
 *       by calling the ble_mi_on_ble_evt() function from the ble_stack_handler callback.
 */

#ifndef __MI_SERVICE_SECURE_H__
#define __MI_SERVICE_SECURE_H__
#include <stdint.h>

#define BLE_MI_MAX_DATA_LEN (GATT_MTU_SIZE_DEFAULT - 3) /**< Maximum length of data (in bytes) that can be transmitted to the peer by the Xiaomi  service module. */

#define MIBLE_SYSINFO_SUPPORT_OPCODE_NUM    8
#define MIBLE_SYSINFO_SUPPORT_OPCODE        0x00
#define MIBLE_SYSINFO_MCU_VERSION           0x01
#define MIBLE_SYSINFO_DEVICE_SN             0x02
#define MIBLE_SYSINFO_HARDWARE_VERSION      0x03
#define MIBLE_SYSINFO_LATITUDE              0x04
#define MIBLE_SYSINFO_LONGITUDE             0x05
#define MIBLE_SYSINFO_VENDOR1               0x06
#define MIBLE_SYSINFO_VENDOR2               0x07
#define MIBLE_SYSINFO_ERROR_CODE            0xFF

typedef enum {
    DEV_INFO_SUPPORT     = 0x00,
    DEV_INFO_MCU_VERSION,
    DEV_INFO_DEVICE_SN,
    DEV_INFO_HARDWARE_VERSION,
    DEV_INFO_LATITUDE,
    DEV_INFO_LONGITUDE,
    DEV_INFO_VENDOR1,
    DEV_INFO_VENDOR2,
} dev_info_type_t;

typedef struct {
    int     code;
    uint8_t *buff;
    uint8_t len;
} dev_info_t;


typedef void (* mi_service_evt_handler_t)(uint8_t is_connected);

typedef void (* mi_service_devinfo_callback_t)(dev_info_type_t type, dev_info_t* data);

typedef void (* gatt_spec_init_t)(uint16_t conn_handle, uint16_t srv_handle, uint16_t read_handle, uint16_t write_handle);
typedef void (* gatt_spec_deinit_t)(void);
typedef void (* gatt_spec_write_t)(uint8_t *pdata, uint16_t len);
typedef void (* gatt_spec_recv_t)(uint8_t *pdata, uint16_t len);

typedef struct {
    gatt_spec_init_t    init;
    gatt_spec_deinit_t  deinit;
    gatt_spec_write_t   write;
    gatt_spec_recv_t    recv;
} mible_gatt_spec_func_t;

/**@brief Function for initializing the Xiaomi Service.
 */
uint32_t mi_service_init(void);

/**
 * @brief Function for report the wifi access status to App.
 *
 * @param[in] status: 0 no connection
 *                    1 do connecting
 *                    2 connected WiFi AP
 *                    3 connected server
 *                    4 wifi passwd incorrect
 *                    5 server auth failed
 *                    6 passport auth successful
 */
int report_wifi_status(uint8_t status);

/**
 * @brief Function for set local wifi status GATT char value.
 *
 * @param[in] status: 0 no connection
 *                    1 do connecting
 *                    2 connected WiFi AP
 *                    3 connected server
 *                    4 wifi passwd incorrect
 *                    5 server auth failed
 *                    6 passport auth successful
 */
int set_wifi_status(uint8_t status);

/* internal api */
void mi_service_event_register(mi_service_evt_handler_t h);
int mi_service_devinfo_callback_register(mi_service_devinfo_callback_t cb);
int mi_spec_callback_register(mible_gatt_spec_func_t cb);

uint32_t opcode_send(uint32_t status);
uint32_t opcode_recv(void);
uint32_t sysinfo_send(uint8_t* p_value, uint8_t len);
//void miio_bt_get_dev_info(dev_info_type_t type, dev_info_t* buf);

#endif // __MI_SERVICE_SECURE_H__
