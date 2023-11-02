/********************************************************************************************************
 * @file	sha1_telink.h
 *
 * @brief	for TLSR chips
 *
 * @author	telink
 * @date	Sep. 30, 2010
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
#ifndef _SHA1_TELINK_H
#define _SHA1_TELINK_H

//#include <string.h>
//#include <stddef.h>


#include <stdint.h>
#ifndef NULL
#define NULL 	0
#endif


/********
SHA-1 
********/
typedef struct{
	uint32_t total[2];			/*!< number of bytes processed	*/
	uint32_t state[5];			/*!< intermediate digest state	*/
	unsigned char buffer[64];	/*!< data block being processed */
}mbedtls_sha1_context;

void mbedtls_sha1_init( mbedtls_sha1_context *ctx );

void mbedtls_sha1_free( mbedtls_sha1_context *ctx );

void mbedtls_sha1_clone( mbedtls_sha1_context *dst,
                         const mbedtls_sha1_context *src );

void mbedtls_sha1_starts( mbedtls_sha1_context *ctx );
void mbedtls_sha1_update( mbedtls_sha1_context *ctx, const unsigned char *input, size_t ilen );

void mbedtls_sha1_finish( mbedtls_sha1_context *ctx, unsigned char output[20] );
void mbedtls_sha1_process( mbedtls_sha1_context *ctx, const unsigned char data[64] );
void mbedtls_sha1( const unsigned char *input, size_t ilen, unsigned char output[20] );

#endif
