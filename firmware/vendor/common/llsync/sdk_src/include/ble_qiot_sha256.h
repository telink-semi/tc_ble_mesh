/**
 *   SHA-256 implementation.
 *
 *   Copyright (c) 2010 Ilya O. Levin, http://www.literatecode.com
 *
 *   Permission to use, copy, modify, and distribute this software for any
 *   purpose with or without fee is hereby granted, provided that the above
 *   copyright notice and this permission notice appear in all copies.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *   WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *   MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *   ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef TENCENTCLOUD_IOT_EXPLORER_BLE_MESH_SDK_EMBEDDED_SDK_INCLUDE_BLE_QIOT_SHA256_H_
#define TENCENTCLOUD_IOT_EXPLORER_BLE_MESH_SDK_EMBEDDED_SDK_INCLUDE_BLE_QIOT_SHA256_H_

#ifdef _MSC_VER
#ifndef uint8_t
typedef unsigned __int8 uint8_t;
#endif
#ifndef uint32_t
typedef unsigned __int32 uint32_t;
#endif
#ifndef uint64_t
typedef __int64          int64_t;
typedef unsigned __int64 uint64_t;
#endif
#else
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define SHA256_DIGEST_SIZE 32

typedef struct {
    uint32_t buf[16];
    uint32_t hash[8];
    uint32_t len[2];
} sha256_context;

void sha256_init(sha256_context *);
void sha256_update(sha256_context *, const uint8_t * /* data */, uint32_t /* len */);
void sha256_final(sha256_context *, uint8_t * /* hash */);

#ifdef __cplusplus
}
#endif

#endif  // TENCENTCLOUD_IOT_EXPLORER_BLE_MESH_SDK_EMBEDDED_SDK_INCLUDE_BLE_QIOT_SHA256_H_
