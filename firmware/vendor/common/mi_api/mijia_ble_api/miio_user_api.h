/*
 * miio_user_api.h
 *
 *  Created on: 2020��12��18��
 *      Author: mi
 */

#ifndef MIJIA_BLE_API_MIIO_USER_API_H_
#define MIJIA_BLE_API_MIIO_USER_API_H_

#include "mi_config.h"
#include "ble_spec/gatt_spec.h"
#include "ble_spec/mi_spec_type.h"
#include "mible_api.h"
#include "mijia_profiles/mi_service_server.h"
#include "gatt_dfu/mible_dfu_main.h"
#if defined(MI_MESH_ENABLED)
#include "mible_mesh_api.h"
#include "mesh_auth/mible_mesh_auth.h"
#include "mesh_auth/mible_mesh_device.h"
#include "mesh_auth/mible_mesh_operation.h"
#else
#if defined (MI_BLE_ENABLED) && (!HAVE_MSC)
#include "standard_auth/mible_standard_auth.h"
#elif defined (MI_BLE_ENABLED) && (HAVE_MSC)
#include "secure_auth/mible_secure_auth.h"
#endif
#include "common/mible_beacon.h"
#endif

#define MIBLE_USER_REC_ID_BASE                  0x50
#define SERVER_GMT_OFFSET                       1
#define SERVER_WEATHER                          2
#define SERVER_UTC_TIME                         3

/**
 *@brief    reboot device.
 *@return   0: success, negetive value: failure
 */
static inline int miio_system_reboot(void)
{
    return mible_reboot();
}

/**
 *@brief    restore device.
 *@return   0: success, negetive value: failure
 */
static inline int miio_system_restore(void)
{
    mi_scheduler_start(SYS_KEY_DELETE);
#if defined(MI_MESH_ENABLED)
    mible_mesh_node_reset();
#endif
    return mible_reboot();
}

/**
 *@brief    get system time.
 *@return   systicks in ms.
 */
static inline uint64_t miio_system_get_time(void)
{
    return mible_mesh_get_exact_systicks();
}

/**
 *@brief    set mibeacon advertising timeout, after timeout advertising will stop.
 *@param    [in] millsecond : adv timeout in ms, 0 is stop adv, 0xffffffff is always on.
 *@return   0: success, negetive value: failure
 */
static inline int miio_system_set_adv_timeout(uint32_t timeout)
{
#if defined(MI_MESH_ENABLED)
    return mible_mesh_device_adv_start(timeout);
#else
    return mibeacon_set_adv_timeout(timeout);
#endif
}

/**
 *@brief    set node tx power.
 *@param    [in] power : TX power in 0.1 dBm steps.
 *@return   0: success, negetive value: failure
 */
static inline int miio_system_set_tx_power(int16_t power)
{
    return mible_set_tx_power(power);
}

/**
 *@brief    register callback for report MCU Version/ SN or vendor info, you can ignore it if you don't need
 *@brief    sdk will callback when provision or login, you should fill in buffer and return code
 *@param    [in/out] cb : event type and require data
 *@note     DEV_INFO_SUPPORT(Must include): Info type your supported
 *@note     DEV_INFO_MCU_VERSION: Version of outside mcu, 4 byte string, eg. "0001"
 *@note     DEV_INFO_DEVICE_SN: Device Serial Number, maximum 20byte string , eg. "12345/A8Q600001"
 *@note     DEV_INFO_HARDWARE_VERSION: Device Hardware Version, maximum 20byte string
 *@note     DEV_INFO_VENDOR1: Vendor Info (app not support yet), maximum 20byte string
 *@note     DEV_INFO_VENDOR2: Vendor Info (app not support yet), maximum 20byte string
 *@return   0: success, negetive value: failure
 */
static inline int miio_system_info_callback_register(mi_service_devinfo_callback_t cb)
{
    return mi_service_devinfo_callback_register(cb);
}

/* FLASH related function*/

/*
 * @brief   Create a record in flash
 * @param   [in] record_id: identify a record in flash
 *          [in] len: record length
 * @return  MI_SUCCESS              Create successfully.
 *          MI_ERR_INVALID_LENGTH   Size was 0, or higher than the maximum
 *allowed size.
 *          MI_ERR_NO_MEM,          Not enough flash memory to be assigned
 *
 * */
static inline int miio_record_create(uint16_t record_id, uint8_t len)
{
    return mible_record_create(MIBLE_USER_REC_ID_BASE + record_id, len);
}

/*
 * @brief   Delete a record in flash
 * @param   [in] record_id: identify a record in flash
 * @return  MI_SUCCESS              Delete successfully.
 *          MI_ERR_INVALID_PARAM   Invalid record id supplied.
 * */
static inline int miio_record_delete(uint16_t record_id)
{
    return mible_record_delete(MIBLE_USER_REC_ID_BASE + record_id);
}

/*
 * @brief   Restore data to flash
 * @param   [in] record_id: identify an area in flash
 *          [out] p_data: pointer to data
 *          [in] len: data buffer length
 * @return  MI_SUCCESS              The command was accepted.
 *          MI_ERR_INVALID_LENGTH   Size was 0, or higher than the maximum
 *allowed size.
 *          MI_ERR_INVALID_PARAM   Invalid record id supplied.
 *          MI_ERR_INVALID_ADDR     Invalid pointer supplied.
 * */
static inline int miio_record_read(uint16_t record_id, uint8_t* p_data, uint8_t len)
{
    return mible_record_read(MIBLE_USER_REC_ID_BASE + record_id, p_data, len);
}

/*
 * @brief   Store data to flash
 * @param   [in] record_id: identify an area in flash
 *          [in] p_data: pointer to data
 *          [in] len: data length
 * @return  MI_SUCCESS              The command was accepted.
 *          MI_ERR_INVALID_LENGTH   Size was 0, or higher than the maximum
 * allowed size.
 *          MI_ERR_INVALID_PARAM   p_data is not aligned to a 4 byte boundary.
 * @note    Should use asynchronous mode to implement this function.
 *          The data to be written to flash has to be kept in memory until the
 * operation has terminated, i.e., an event is received.
 *          When record writing complete , call mible_arch_event_callback function and pass MIBLE_ARCH_EVT_RECORD_WRITE_CMP event and result.
 * */
static inline int miio_record_write(uint16_t record_id, const uint8_t *p_data, uint8_t len)
{
    return mible_record_write(MIBLE_USER_REC_ID_BASE + record_id, p_data, len);
}

/*TIMER related function*/

/*
 * @brief   Create a timer.
 * @param   [out] pp_timer: a pointer to timer handle address which can uniquely
 *  identify the timer.
 *          [in] timeout_handler: a function will be called when the timer expires.
 *          [in] mode: repeated or single shot.
 * @return  MI_SUCCESS             If the timer was successfully created.
 *          MI_ERR_INVALID_PARAM   Invalid pointer supplied.
 *          MI_ERR_INVALID_STATE   timer module has not been initialized or the
 * timer is running.
 *          MI_ERR_NO_MEM          timer pool is full.
 *
 * */
static inline int miio_timer_create(void** pp_timer, mible_timer_handler timeout_handler, mible_timer_mode mode)
{
    return mible_user_timer_create(pp_timer, timeout_handler, mode);
}

/*
 * @brief   Delete a timer.
 * @param   [in] timer_handle: unique index of the timer.
 * @return  MI_SUCCESS             If the timer was successfully deleted.
 *          MI_ERR_INVALID_PARAM   Invalid timer id supplied..
 * */
static inline int miio_timer_delete(void* timer_handle)
{
    return mible_timer_delete(timer_handle);
}

/*
 * @brief   Start a timer.
 * @param   [in] timer_handle: unique index of the timer.
 *          [in] time_ms: number of milliseconds to time-out event
 * (minimum 10 ms).
 *          [in] p_context: parameters that can be passed to
 * timeout_handler
 *
 * @return  MI_SUCCESS             If the timer was successfully started.
 *          MI_ERR_INVALID_PARAM   Invalid timer id supplied.
 *          MI_ERR_INVALID_STATE   If the application timer module has not been
 * initialized or the timer has not been created.
 *          MI_ERR_NO_MEM          If the timer operations queue was full.
 * @note    If the timer has already started, it will start counting again.
 * */
static inline int miio_timer_start(void* timer_handle, uint32_t time_ms, void* p_context)
{
    return mible_timer_start(timer_handle, time_ms, p_context);
}

/*
 * @brief   Stop a timer.
 * @param   [in] timer_handle: unique index of the timer.
 * @return  MI_SUCCESS             If the timer was successfully stopped.
 *          MI_ERR_INVALID_PARAM   Invalid timer id supplied.
 *
 * */
static inline int miio_timer_stop(void* timer_handle)
{
    return mible_timer_stop(timer_handle);
}

/* DFU related function*/

/**@brief Register DFU callback function used to receive DFU event.
 *
 * @param[in] cb    Callback function
 */
static inline int miio_dfu_callback_register(mible_dfu_callback_t cb)
{
    return mible_dfu_callback_register(cb);
}
static inline int miio_dfu_callback_unregister(mible_dfu_callback_t cb)
{
    return mible_dfu_callback_unregister(cb);
}
static inline int miio_dfu_mcu_init(void)
{
    return mible_dfu_mcu_init();
}

/* Spec operation related function*/

/**
 *@brief    malloc property value by type or argument for event / action.
 *@param    [in] value : value by type.
 *@return   NULL: out of memory, others: pointer of value or arguments
 */
property_value_t * property_value_new_bytype(property_format_t type, void *value);
property_value_t * property_value_new_boolean(bool value);
property_value_t * property_value_new_integer(int32_t value);
property_value_t * property_value_new_float(float value);
property_value_t * property_value_new_string(const char *value);
property_value_t * property_value_new_char(int8_t value);
property_value_t * property_value_new_uchar(uint8_t value);
property_value_t * property_value_new_short(int16_t value);
property_value_t * property_value_new_ushort(uint16_t value);
property_value_t * property_value_new_long(int32_t value);
property_value_t * property_value_new_ulong(uint32_t value);
property_value_t * property_value_new_longlong(int64_t value);
property_value_t * property_value_new_ulonglong(uint64_t value);
arguments_t * arguments_new(void);

/* Gatt spec related function*/

/**
 *@brief    register gatt spec control callback, and set buffer length
 *@param    [in] set_cb : set_properties callback
 *@param    [in] get_cb : get_properties callback
 *@param    [in] action_cb : action callback
 *@param    [in] buf_len : gatt buffer length(default 64byte, minimum 32byte)
 *@return   0: success, negetive value: failure
 */
static inline int miio_gatt_spec_init(property_operation_callback_t set_cb,
                                    property_operation_callback_t get_cb,
                                    action_operation_callback_t action_cb,
                                    uint16_t buflen)
{
    return mible_gatt_spec_init(set_cb, get_cb, action_cb, buflen);
}

/**
 *@brief    send properties_changed.
 *@param    [in] siid: service id.
 *@param    [in] piid: property id.
 *@param    [in] newValue: property value.
 *@return   0: success, negetive value: failure
 */
static inline int miio_gatt_properties_changed(uint16_t siid, uint16_t piid, property_value_t *newValue)
{
    return gatt_send_property_changed(siid, piid, newValue);
}

/**
 *@brief    send properties_changed.
 *@param    [in] siid: service id.
 *@param    [in] piid: property id.
 *@param    [in] newArgs: event args.
 *@return   0: success, negetive value: failure
 */
static inline int miio_gatt_event_occurred(uint16_t siid, uint16_t eiid, arguments_t *newArgs)
{
    return gatt_send_event_occurred(siid, eiid, newArgs);
}

/* Mesh related function*/

#if defined(MI_MESH_ENABLED)
/**
 *@brief    set node rx scan window.
 *@param    [in] level : 0: OFF 1% window for receive iv, 1: LOW 15% window,
 *          2: NORMAL 20% window, 3: HIGH 30% window, 4: FULL 100% window
 *@return   0: success, negetive value: failure
 */
static inline int miio_mesh_set_scan_level(uint8_t level)
{
    return mible_mesh_device_scan_set(level);
}

/**
 *@brief    sync method, register event callback
 *@param    [in] set_cb : set_properties callback, require siid/piid/ *value, need rsp code
 *@param    [in] get_cb : get_properties callback, require siid/piid, need rsp *value/code
 *@param    [in] action_cb : action callback, require siid/aiid/ *args, need rsp code
 *@param    [in] user_event_cb : when provision/login/connect state changed, execute callback
 *@return   0: success, negetive value: failure
 */
static inline int miio_mesh_user_callback_register(property_operation_callback_t set_cb,
                                                    property_operation_callback_t get_cb,
                                                    action_operation_callback_t action_cb,
                                                    mible_user_event_cb_t user_event_cb)
{
    mible_mesh_spec_callback_register(set_cb, get_cb, action_cb);
    return mible_mesh_user_event_register_event_callback(user_event_cb);
}

/**
 *@brief    period send properties_changed.
 *@param    [in] siid: service id.
 *@param    [in] piid: property id.
 *@param    [in] period: time in ms, 0: add to 3min circle pub,
 *          others: if period > 10min, send properties_changed to gateway every period
 *@return   0: success, negetive value: failure
 */
static inline int miio_mesh_properties_period_publish_init(uint16_t siid, uint16_t piid, uint32_t period)
{
    return mible_mesh_pub_add(siid, piid, period);
}

/**
 *@brief    send mesh properties_changed.
 *@param    [in] siid: service id.
 *@param    [in] piid: property id.
 *@param    [in] newValue: property value.
 *@return   0: success, negetive value: failure
 */
static inline int miio_mesh_properties_changed(uint16_t siid, uint16_t piid, property_value_t *newValue)
{
    return mesh_send_property_changed(siid, piid, newValue);
}

/**
 *@brief    send mesh event occurred.
 *@param    [in] siid: service id.
 *@param    [in] piid: property id.
 *@param    [in] newArgs: event args.
 *@return   0: success, negetive value: failure
 */
static inline int miio_mesh_event_occurred(uint16_t siid, uint16_t eiid, arguments_t *newArgs)
{
    return mesh_send_event_occurred(siid, eiid, newArgs);
}

/**
 *@brief    require service info: GMT offset, Weather or UTC time.
 *          callback event: on_property_set callback, data: int32.
 *@param    [in] type: 1: SERVER_GMT_OFFSET, 2: SERVER_WEATHER, 3: SERVER_UTC_TIME.
 *@return   0: success, negetive value: failure
 */
static inline int miio_mesh_request_property(uint8_t type)
{
    return mesh_send_property_request(128, type);
}

#else

/**
 *@brief    init ble adv
 *@param    [in] solicite_bind: if APP will connect to this device.
 */
static inline void miio_ble_adv_init(uint8_t solicite_bind)
{
    MI_LOG_INFO("advertising init...\n");

    // add user customized adv struct : complete local name
    uint8_t data[31], len;
    uint8_t str_len = MIN(29, strlen(MODEL_NAME));
    data[0] = 1 + str_len;
    data[1] = 9;  // complete local name

    strncpy((char*)&data[2], MODEL_NAME, str_len);
    len = 2 + str_len;

    if(MI_SUCCESS != mibeacon_adv_data_set(solicite_bind, 0, data, len)){
        MI_LOG_ERROR("mibeacon_data_set failed. \r\n");
    }
}

/**
 *@brief    set ble adv interval
 *@param    [in] adv_interval_ms: adv interval in millisecond.
 */
static inline void miio_ble_adv_start(uint16_t adv_interval_ms)
{
    MI_LOG_INFO("advertising start...\n");
    mibeacon_adv_start(adv_interval_ms);
}

/**
 *@brief    send ble properties_changed.
 *@param    [in] siid: service id.
 *@param    [in] piid: property id.
 *@param    [in] newValue: property value.
 *@param    [in] stop_adv: When the object queue is sent out, it will SHUTDOWN BLE advertising
 *@param    [in] isUrgent: if enqueue this object into a high priority queue
 *@return   0: success, negetive value: failure
 */
static inline int miio_ble_property_changed(uint16_t siid, uint16_t piid, property_value_t *newValue, uint8_t stop_adv, uint8_t isUrgent)
{
    int ret = mibeacon_property_changed(siid, piid, get_property_len(newValue), &(newValue->data), stop_adv, isUrgent);
    if (newValue != NULL){
        property_value_delete(newValue);
    }
    return ret;
}

/**
 *@brief    send ble event occurred.
 *@param    [in] siid: service id.
 *@param    [in] piid: property id.
 *@param    [in] newArgs: event args.
 *@param    [in] stop_adv: When the object queue is sent out, it will SHUTDOWN BLE advertising
 *@param    [in] isUrgent: if enqueue this object into a high priority queue
 *@return   0: success, negetive value: failure
 */
static inline int miio_ble_event_occurred(uint16_t siid, uint16_t eiid, arguments_t *newArgs, uint8_t stop_adv, uint8_t isUrgent)
{
    uint8_t p_num = (NULL == newArgs)? 0 : newArgs->size;
    int ret = -1;
    if(p_num > 0) {
        uint8_t buff[9] = {0};
        uint8_t len = 0;

        for(int i = 0; i < p_num; i++){
            property_value_t *newValue = newArgs->arguments[i].value;
            memcpy(buff + len, &(newValue->data), get_property_len(newValue));
            len += get_property_len(newValue);
            if(len > 9)
            {
                return -1;
            }
        }

        ret = mibeacon_event_occurred(siid, eiid, len, buff, stop_adv, isUrgent);
    }

    return ret;
}

static inline int miio_ble_get_registered_state(void)
{
    return mibeacon_get_registered_state();
}

#endif

#endif /* MIJIA_BLE_API_MIIO_USER_API_H_ */

