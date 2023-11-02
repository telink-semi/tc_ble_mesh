/*
 * mible_mesh_device.h
 *
 *  Created on: 2020Äê11ÔÂ26ÈÕ
 *      Author: mi
 */

#ifndef MIJIA_BLE_LIBS_MESH_AUTH_MIBLE_MESH_DEVICE_H_
#define MIJIA_BLE_LIBS_MESH_AUTH_MIBLE_MESH_DEVICE_H_

#include "mible_mesh_api.h"
#include "mible_mesh_auth.h"
#include "mible_mesh_operation.h"

typedef enum {
    MESH_SCAN_OFF = 0,      //start scan 30s/2hours for receive snb
    MESH_SCAN_LOW,          //15% scan window
    MESH_SCAN_NORMAL,       //20% scan window
    MESH_SCAN_HIGH,         //30% scan window
    MESH_SCAN_FULL,         //100% scan window, enable relay
    MESH_SCAN_WIRELESS,     //2% scan window, only for wireless switch mode
} mesh_scan_level_t;

int mible_mesh_device_init(void);
void mible_mesh_device_main_thread(void);
/**
 *@brief    set node rx scan window.
 *@param    [in] level : 0: OFF 1% window for receive iv, 1: LOW 15% window,
 *          2: NORMAL 20% window, 3: HIGH 30% window, 4: FULL 100% window
 *@return   0: success, negetive value: failure
 */
int mible_mesh_device_scan_set(uint8_t level);
uint8_t mible_mesh_device_scan_get(void);
int mible_mesh_device_adv_start(uint32_t timeout);

uint32_t rand_in(uint16_t min, uint16_t max);
int mible_mesh_spec_property_rsp(uint8_t siid, uint8_t piid, property_value_t *value, uint16_t dst_addr, uint32_t delay_ms);
int mible_mesh_spec_action_rsp(uint8_t siid, uint8_t aiid, arguments_t *out, uint16_t dst_addr, uint32_t delay_ms);
int mible_mesh_spec_property_changed(uint8_t siid, uint8_t piid, property_value_t *value, uint16_t dst_addr, uint32_t delay_ms);
int mible_mesh_spec_property_request(uint8_t siid, uint8_t piid, uint16_t dst_addr, uint32_t delay_ms);
int mible_mesh_spec_event_occured(uint8_t siid, uint8_t eiid, arguments_t *out, uint16_t dst_addr, uint32_t delay_ms);
int mible_mesh_single_pub_start(uint8_t siid, uint8_t piid);
int mible_mesh_pub_add(uint8_t siid, uint8_t piid, uint32_t period);

#endif /* MIJIA_BLE_LIBS_MESH_AUTH_MIBLE_MESH_DEVICE_H_ */
