/*
 * mible_memory.h
 *
 *  Created on: 2021Äê3ÔÂ8ÈÕ
 *      Author: mi
 */

#ifndef MIJIA_BLE_LIBS_COMMON_MIBLE_MEMORY_H_
#define MIJIA_BLE_LIBS_COMMON_MIBLE_MEMORY_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "mible_log.h"

/**
 *@brief    memory alloc.
 *@param    [in] size: alloc memory byte.
 *@return   NULL: failure
 */
static inline void *mible_malloc(size_t size)
{
    size_t size64 = (size & 0xFFFFFFC0) + (size & 0x0000003F ? 0x40 : 0);
    //MI_LOG_INFO("My malloc %d byte(%p)!\n", size64, ptr);
    return malloc(size64);
}

/**
 *@brief    memory alloc.
 *@param    [in] size: alloc memory byte.
 *@return   NULL: failure
 */
static inline void mible_free(void *ptr)
{
    //MI_LOG_INFO("My free(%p)!\n", ptr);
    return free(ptr);
}

#endif /* MIJIA_BLE_LIBS_COMMON_MIBLE_MEMORY_H_ */
