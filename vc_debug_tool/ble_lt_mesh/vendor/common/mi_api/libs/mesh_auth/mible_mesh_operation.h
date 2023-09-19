/*
 * mible_mesh_spec.h
 *
 *  Created on: 2020Äê12ÔÂ10ÈÕ
 *      Author: mi
 */

#ifndef MIJIA_BLE_LIBS_MESH_AUTH_MIBLE_MESH_OPERATION_H_
#define MIJIA_BLE_LIBS_MESH_AUTH_MIBLE_MESH_OPERATION_H_

#include "ble_spec/mi_spec_type.h"
#include "mible_log.h"

int execute_property_operation(property_operation_type type, uint16_t siid, uint16_t piid,
                            property_value_t *newValue, void *ctx);
int execute_action_invocation(uint16_t siid, uint16_t aiid, arguments_t *newArgs, void *ctx);
int execute_property_request(uint16_t siid, uint16_t piid, property_value_t *newValue, void *ctx);
int mesh_send_property_changed(uint16_t siid, uint16_t piid, property_value_t *newValue);
int mesh_send_property_request(uint16_t siid, uint16_t piid);
int mesh_send_event_occurred(uint16_t siid, uint16_t eiid, arguments_t *newArgs);

int mible_mesh_spec_callback_register(property_operation_callback_t set_cb,
                                property_operation_callback_t get_cb,
                                action_operation_callback_t action_cb);

#endif /* MIJIA_BLE_LIBS_MESH_AUTH_MIBLE_MESH_OPERATION_H_ */
