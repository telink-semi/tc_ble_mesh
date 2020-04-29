/**
 * \file md_wrap.c
 *
 * \brief Generic message digest wrapper for mbed TLS
 *
 * \author Adriaan de Jong <dejong@fox-it.com>
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_MD_C)

#include "md_internal.h"

#if defined(MBEDTLS_SHA256_C)
#include "sha256.h"
#endif

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdlib.h>
#include "mible_log.h"
#define mbedtls_calloc    //calloc
#define mbedtls_free       //free
#endif
#if MI_API_ENABLE

/*
 * Wrappers for generic message digests
 */
#if defined(MBEDTLS_SHA256_C)

static int sha224_update_wrap( void *ctx, const unsigned char *input,
                                size_t ilen )
{
    return( mbedtls_sha256_update_ret( (mbedtls_sha256_context *) ctx,
                                       input, ilen ) );
}

static int sha224_finish_wrap( void *ctx, unsigned char *output )
{
    return( mbedtls_sha256_finish_ret( (mbedtls_sha256_context *) ctx,
                                       output ) );
}
static mbedtls_sha256_context ctx_str;
static void *sha224_ctx_alloc( void )
{
	#if 0
    void *ctx = mbedtls_calloc( 1, sizeof( mbedtls_sha256_context ) );
	#else
	memset(&ctx_str,0,sizeof(ctx_str));
    void *ctx = (void *)(&ctx_str);
	#endif

    if( ctx != NULL )
        mbedtls_sha256_init( (mbedtls_sha256_context *) ctx );

    return( ctx );
}

static void sha224_ctx_free( void *ctx )
{
    mbedtls_sha256_free( (mbedtls_sha256_context *) ctx );
    //mbedtls_free( ctx );
}

static void sha224_clone_wrap( void *dst, const void *src )
{
    mbedtls_sha256_clone( (mbedtls_sha256_context *) dst,
                    (const mbedtls_sha256_context *) src );
}

static int sha224_process_wrap( void *ctx, const unsigned char *data )
{
    return( mbedtls_internal_sha256_process( (mbedtls_sha256_context *) ctx,
                                             data ) );
}

static int sha256_starts_wrap( void *ctx )
{
    return( mbedtls_sha256_starts_ret( (mbedtls_sha256_context *) ctx, 0 ) );
}

static int sha256_wrap( const unsigned char *input, size_t ilen,
                        unsigned char *output )
{
    return( mbedtls_sha256_ret( input, ilen, output, 0 ) );
}

const mbedtls_md_info_t mbedtls_sha256_info = {
    MBEDTLS_MD_SHA256,
    "SHA256",
    32,
    64,
    sha256_starts_wrap,
    sha224_update_wrap,
    sha224_finish_wrap,
    sha256_wrap,
    sha224_ctx_alloc,
    sha224_ctx_free,
    sha224_clone_wrap,
    sha224_process_wrap,
};

#endif /* MBEDTLS_SHA256_C */

#endif /* MBEDTLS_MD_C */
#endif
