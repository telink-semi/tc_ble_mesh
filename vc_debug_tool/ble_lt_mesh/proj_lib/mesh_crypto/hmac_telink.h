/********************************************************************************************************
 * @file	hmac_telink.h
 *
 * @brief	This is the header file for BLE SDK
 *
 * @author	Mesh Group
 * @date	2021
 *
 * @par     Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/
#ifndef MBEDTLS_HMAC_H
#define MBEDTLS_HMAC_H

//#include <string.h>
//#include <stddef.h>


#include <stdint.h>
#ifndef NULL
#define NULL 	0
#endif
typedef enum {
	MBEDTLS_MD_NONE=0,
	MBEDTLS_MD_MD2,
	MBEDTLS_MD_MD4,
	MBEDTLS_MD_MD5,
	MBEDTLS_MD_SHA1,
	MBEDTLS_MD_SHA224,
	MBEDTLS_MD_SHA256,
	MBEDTLS_MD_SHA384,
	MBEDTLS_MD_SHA512,
	MBEDTLS_MD_RIPEMD160,
} mbedtls_md_type_t;

#define MBEDTLS_ERR_MD_FEATURE_UNAVAILABLE                -0x5080  /**< The selected feature is not available. */
#define MBEDTLS_ERR_MD_BAD_INPUT_DATA                     -0x5100  /**< Bad input parameters to function. */
#define MBEDTLS_ERR_MD_ALLOC_FAILED                       -0x5180  /**< Failed to allocate memory. */
#define MBEDTLS_ERR_MD_FILE_IO_ERROR                      -0x5200  /**< Opening or reading of file failed. */
#define MBEDTLS_ERR_MD_HW_ACCEL_FAILED                    -0x5280  /**< MD hardware accelerator failed. */
#define MBEDTLS_MD_MAX_SIZE         32  /* longest known is SHA256 or less */
typedef struct
{
	/** Digest identifier */
	mbedtls_md_type_t type;
										
	/** Name of the message digest */
	const char * name;
										
	/** Output length of the digest function in bytes */
	int size;
										
	/** Block length of the digest function in bytes */
	int block_size;
										
	/** Digest initialisation function */
	int (*starts_func)( void *ctx );
										
	/** Digest update function */
	int (*update_func)( void *ctx, const unsigned char *input, size_t ilen );
										
	/** Digest finalisation function */
	int (*finish_func)( void *ctx, unsigned char *output );
										
	/** Generic digest function */
	int (*digest_func)( const unsigned char *input, size_t ilen,
	unsigned char *output );
	
#if 0	// BLE_SRC_TELINK_MESH_EN										
	/** Allocate a new context */
	void * (*ctx_alloc_func)( void );
										
	/** Free the given context */
	void (*ctx_free_func)( void *ctx );
#endif

	/** Clone state from a context */
	void (*clone_func)( void *dst, const void *src );
										
	/** Internal use only */
	int (*process_func)( void *ctx, const unsigned char *input );
}mbedtls_md_info_t;

typedef struct {
    /** Information about the associated message digest. */
    const mbedtls_md_info_t *md_info;

    /** The digest-specific context. */
    void *md_ctx;

    /** The HMAC part of the context. */
    void *hmac_ctx;
} mbedtls_md_context_t;

extern const mbedtls_md_info_t mbedtls_sha256_info;

int mbedtls_md_hmac( const mbedtls_md_info_t *md_info,
                     const unsigned char *key, size_t keylen,
                     const unsigned char *input, size_t ilen,
                     unsigned char *output );

unsigned int sha256_hkdf(unsigned char *key, unsigned int key_len,unsigned char *salt, unsigned int salt_len,
						unsigned char *info, unsigned int info_len, unsigned char *out, unsigned int out_len);

#endif

