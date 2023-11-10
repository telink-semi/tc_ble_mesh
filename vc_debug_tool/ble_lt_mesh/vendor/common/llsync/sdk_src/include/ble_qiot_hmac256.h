
/*
 * hmac-sha256.h
 * Copyright (C) 2017 Adrian Perez <aperez@igalia.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef TENCENTCLOUD_IOT_EXPLORER_BLE_MESH_SDK_EMBEDDED_SDK_INCLUDE_BLE_QIOT_HMAC256_H_
#define TENCENTCLOUD_IOT_EXPLORER_BLE_MESH_SDK_EMBEDDED_SDK_INCLUDE_BLE_QIOT_HMAC256_H_

#include <stddef.h>
#include <stdint.h>

#define HMAC_SHA256_DIGEST_SIZE 32 /* Same as SHA-256's output size. */

void llsync_hmac_sha256(uint8_t out[HMAC_SHA256_DIGEST_SIZE], const uint8_t *data, size_t data_len, const uint8_t *key,
                        size_t key_len);

#endif  // TENCENTCLOUD_IOT_EXPLORER_BLE_MESH_SDK_EMBEDDED_SDK_INCLUDE_BLE_QIOT_HMAC256_H_
