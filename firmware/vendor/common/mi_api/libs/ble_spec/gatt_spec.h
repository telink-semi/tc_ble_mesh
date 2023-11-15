/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __MI_SPEC_MAIN_H__
#define __MI_SPEC_MAIN_H__

/* Includes ------------------------------------------------------------------*/
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "mible_api.h"
#include "mible_log.h"
#include "mible_trace.h"
#include "common/crc32.h"
#include "common/mible_rxfer.h"
#include "common/queue.h"
#include "common/mible_crypto.h"
#include "ble_spec/mi_spec_type.h"
#include "mijia_profiles/mi_service_server.h"

#define MIBLE_GATT_SPEC_TIMER_PERIOD                20
#define MIBLE_GATT_SPEC_VERSION                     2

#define MIBLE_GATT_SPEC_OP_SET_PROPERTY             0x00
#define MIBLE_GATT_SPEC_OP_SET_PROPERTY_RSP         0x01
#define MIBLE_GATT_SPEC_OP_GET_PROPERTY             0x02
#define MIBLE_GATT_SPEC_OP_GET_PROPERTY_RSP         0x03
#define MIBLE_GATT_SPEC_OP_PROPERTY_CHANGED         0x04
#define MIBLE_GATT_SPEC_OP_ACTION                   0x05
#define MIBLE_GATT_SPEC_OP_ACTION_RSP               0x06
#define MIBLE_GATT_SPEC_OP_EVENT_OCCURRED            0x07

#define MIBLE_GATT_SPEC_OP_PROTOCOL_EXCHANGE        0xF0

#define MIBLE_GATT_SPEC_LENGTH_STRING               1024

enum {
    MIBLE_GATT_SPEC_TYPE_BOOL = 0,
    MIBLE_GATT_SPEC_TYPE_UINT8,
    MIBLE_GATT_SPEC_TYPE_INT8,
    MIBLE_GATT_SPEC_TYPE_UINT16,
    MIBLE_GATT_SPEC_TYPE_INT16,
    MIBLE_GATT_SPEC_TYPE_UINT32,
    MIBLE_GATT_SPEC_TYPE_INT32,
    MIBLE_GATT_SPEC_TYPE_UINT64,
    MIBLE_GATT_SPEC_TYPE_INT64,
    MIBLE_GATT_SPEC_TYPE_FLOAT,
    MIBLE_GATT_SPEC_TYPE_STRING,
    MIBLE_GATT_SPEC_TYPE_UNKNOW = 0x0F,
};

#if defined ( __CC_ARM )
__PACKED struct spec_property_s{
    uint8_t         siid;
    uint16_t        piid;
    int16_t         code;
    uint8_t         type;
    uint16_t        len;
    void*           val;
};

__PACKED typedef struct {
    uint8_t         siid;
    uint16_t        piid;
    uint16_t        value_type;
    uint8_t         value[];
}gatt_spec_property_t;

__PACKED typedef struct {
    uint8_t         siid;
    uint16_t        piid;
    int16_t         code;
}gatt_spec_property_rsp_t;
#elif defined ( __GNUC__ )
struct __PACKED spec_property_s{
    uint8_t         siid;
    uint16_t        piid;
    int16_t         code;
    uint8_t         type;
    uint16_t        len;	  
    void*           val;
};

typedef struct __PACKED{
    uint8_t         siid;
    uint16_t        piid;
    uint16_t        value_type;
    uint8_t         value[];
}gatt_spec_property_t;

typedef struct __PACKED{
    uint8_t         siid;
    uint16_t        piid;
    int16_t         code;
}gatt_spec_property_rsp_t;
#endif
typedef struct spec_property_s spec_property_t;

typedef struct {
    uint8_t*        addr;
    uint8_t         len;
} mible_gatt_spec_data_t;

int gatt_send_property_changed(uint8_t siid, uint16_t piid, property_value_t *newValue);
int gatt_send_event_occurred(uint8_t siid, uint16_t eiid, arguments_t *newArgs);
mible_status_t mible_gatt_spec_init(property_operation_callback_t set_cb,
                                    property_operation_callback_t get_cb,
                                    action_operation_callback_t action_cb,
                                    uint16_t buflen);

#endif
