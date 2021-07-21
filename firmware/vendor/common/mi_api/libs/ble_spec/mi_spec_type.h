/*
 * mi_spec_type.h
 *
 *  Created on: 2020Äê12ÔÂ21ÈÕ
 *      Author: mi
 */

#ifndef MIJIA_BLE_LIBS_MI_SPEC_TYPE_H_
#define MIJIA_BLE_LIBS_MI_SPEC_TYPE_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "mible_log.h"

#define DATA_STRING_MAX_LENGTH                  32
#define MAX_ARGUMENTS                           3

typedef enum _property_operation_type
{
    PROPERTY_OPERATION_GET = 0,
    PROPERTY_OPERATION_SET = 1,
} property_operation_type;

typedef enum _operation_code
{
    OPERATION_OK = 0,
    OPERATION_OK_ON = 1,
    OPERATION_ERROR_CANNOT_READ = -4001,
    OPERATION_ERROR_CANNOT_WRITE = -4002,
    OPERATION_INVALID = -4003,
    OPERATION_ERROR_INNER = -4004,
    OPERATION_ERROR_VALUE = -4005,
    OPERATION_ERROR_METHOD = -4006,
    OPERATION_ERROR_DID = -4007,
} operation_code_t;

typedef enum {
    PROPERTY_FORMAT_NUMBER = 0,
    PROPERTY_FORMAT_BOOL,
    PROPERTY_FORMAT_FLOAT,
    PROPERTY_FORMAT_STRING,
    PROPERTY_FORMAT_UCHAR = 0x80,
    PROPERTY_FORMAT_CHAR,
    PROPERTY_FORMAT_USHORT,
    PROPERTY_FORMAT_SHORT,
    PROPERTY_FORMAT_ULONG,
    PROPERTY_FORMAT_LONG,
    PROPERTY_FORMAT_ULONGLONG,
    PROPERTY_FORMAT_LONGLONG,
    PROPERTY_FORMAT_ERRORCODE = 0xFE,
    PROPERTY_FORMAT_UNDEFINED = 0xFF,
} property_format_t;

#if defined ( __CC_ARM )
__PACKED typedef struct _data_string
{
    char          value[DATA_STRING_MAX_LENGTH + 1];
    uint16_t      length;
} data_string_t;

__PACKED typedef union _data_number
{
    int32_t     integerValue;
    float       floatValue;

    int8_t      charValue;
    uint8_t     ucharValue;
    int16_t     shortValue;
    uint16_t    ushortValue;
    int32_t     longValue;
    uint32_t    ulongValue;
    int64_t     longlongValue;
    uint64_t    ulonglongValue;
} data_number_t;

__PACKED typedef union _property_data
{
    bool            boolean;
    data_string_t   string;
    data_number_t   number;
} property_data_t;

__PACKED typedef struct {
    property_data_t     data;
    property_format_t   format;
} property_value_t;

__PACKED typedef struct {
    uint16_t            piid;
    property_value_t*    value;
}argument_t;

__PACKED typedef struct {
    uint16_t      size;
    argument_t    arguments[MAX_ARGUMENTS];
} arguments_t;

__PACKED typedef struct {
    uint16_t            siid;
    uint16_t            piid;
    int                 code;
    property_value_t    *value;
}property_operation_t;

__PACKED typedef struct {
    uint16_t            siid;
    uint16_t            eiid;
    arguments_t         *arguments;
}event_operation_t;

__PACKED typedef struct {
    uint16_t            siid;
    uint16_t            aiid;
    int                 code;
    arguments_t         *in;
    arguments_t         *out;
}action_operation_t;
#elif defined   ( __GNUC__ )
typedef struct __PACKED _data_string
{
    char          value[DATA_STRING_MAX_LENGTH + 1];
    uint16_t      length;
} data_string_t;

typedef union __PACKED _data_number
{
    int32_t     integerValue;
    float       floatValue;

    int8_t      charValue;
    uint8_t     ucharValue;
    int16_t     shortValue;
    uint16_t    ushortValue;
    int32_t     longValue;
    uint32_t    ulongValue;
    int64_t     longlongValue;
    uint64_t    ulonglongValue;
} data_number_t;

typedef union __PACKED _property_data
{
    bool            boolean;
    data_string_t   string;
    data_number_t   number;
} property_data_t;

typedef struct __PACKED{
    property_data_t     data;
    property_format_t   format;
} property_value_t;

typedef struct __PACKED{
    uint16_t            piid;
    property_value_t*    value;
}argument_t;

typedef struct __PACKED{
    uint16_t      size;
    argument_t    arguments[MAX_ARGUMENTS];
} arguments_t;

typedef struct __PACKED{
    uint16_t            siid;
    uint16_t            piid;
    int                 code;
    property_value_t    *value;
}property_operation_t;

typedef struct __PACKED{
    uint16_t            siid;
    uint16_t            eiid;
    arguments_t         *arguments;
}event_operation_t;

typedef struct __PACKED{
    uint16_t            siid;
    uint16_t            aiid;
    int                 code;
    arguments_t         *in;
    arguments_t         *out;
}action_operation_t;
#endif

property_value_t * property_value_New(void);
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

void property_value_delete(property_value_t *thiz);
uint16_t get_property_len(property_value_t *prop);

arguments_t * arguments_new(void);
void arguments_delete(arguments_t *thiz);

typedef void (* property_operation_callback_t)(property_operation_t *o);
typedef void (* action_operation_callback_t)(action_operation_t *o);

property_operation_t * property_operation_new(void);
property_operation_t * property_operation_new_value(uint16_t siid, uint16_t piid, property_value_t *newValue);
void property_operation_delete(property_operation_t *thiz);

action_operation_t * action_operation_new_arguments(uint16_t siid, uint16_t aiid, arguments_t *newArgs);
void action_operation_delete(action_operation_t *thiz);

event_operation_t * event_operation_new_arguments(uint16_t siid, uint16_t eiid, arguments_t *newArgs);
void event_operation_delete(event_operation_t *thiz);

#endif /* MIJIA_BLE_LIBS_MI_SPEC_TYPE_H_ */
