/**
 * \file mbedtls_md.c
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

#include "md.h"
#include "md_internal.h"
#include "sha256.h"
#include "mible_port.h"

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdlib.h>
#define mbedtls_calloc    calloc
#define mbedtls_free       free
#endif

#include <string.h>

#if defined(MBEDTLS_FS_IO)
#include <stdio.h>
#endif
#if MI_API_ENABLE
/* Implementation that should never be optimized out by the compiler */
static void mbedtls_zeroize( void *v, size_t n ) {
    volatile unsigned char *p = v; while( n-- ) *p++ = 0;
}

__WEAK const mbedtls_md_info_t *mbedtls_md_info_from_string( const char *md_name )
{
    if( NULL == md_name )
        return( NULL );

#if defined(MBEDTLS_SHA256_C)
    if( !strcmp( "SHA256", md_name ) )
        return &mbedtls_sha256_info;
#endif
    return( NULL );
}

__WEAK int mbedtls_md_hmac_starts( mbedtls_md_context_t *ctx, const unsigned char *key, size_t keylen )
{
    int ret;
    unsigned char sum[MBEDTLS_MD_MAX_SIZE];
    unsigned char *ipad, *opad;
    size_t i;

    if( ctx == NULL || ctx->md_info == NULL || ctx->hmac_ctx == NULL )
        return( MBEDTLS_ERR_MD_BAD_INPUT_DATA );

    if( keylen > (size_t) ctx->md_info->block_size )
    {
        if( ( ret = ctx->md_info->starts_func( ctx->md_ctx ) ) != 0 )
            goto cleanup;
        if( ( ret = ctx->md_info->update_func( ctx->md_ctx, key, keylen ) ) != 0 )
            goto cleanup;
        if( ( ret = ctx->md_info->finish_func( ctx->md_ctx, sum ) ) != 0 )
            goto cleanup;

        keylen = ctx->md_info->size;
        key = sum;
    }

    ipad = (unsigned char *) ctx->hmac_ctx;
    opad = (unsigned char *) ctx->hmac_ctx + ctx->md_info->block_size;

    memset( ipad, 0x36, ctx->md_info->block_size );
    memset( opad, 0x5C, ctx->md_info->block_size );

    for( i = 0; i < keylen; i++ )
    {
        ipad[i] = (unsigned char)( ipad[i] ^ key[i] );
        opad[i] = (unsigned char)( opad[i] ^ key[i] );
    }

    if( ( ret = ctx->md_info->starts_func( ctx->md_ctx ) ) != 0 )
        goto cleanup;
    if( ( ret = ctx->md_info->update_func( ctx->md_ctx, ipad,
                                           ctx->md_info->block_size ) ) != 0 )
        goto cleanup;

cleanup:
    mbedtls_zeroize( sum, sizeof( sum ) );

    return( ret );
}

__WEAK int mbedtls_md_hmac_update( mbedtls_md_context_t *ctx, const unsigned char *input, size_t ilen )
{
    if( ctx == NULL || ctx->md_info == NULL || ctx->hmac_ctx == NULL )
        return( MBEDTLS_ERR_MD_BAD_INPUT_DATA );

    return( ctx->md_info->update_func( ctx->md_ctx, input, ilen ) );
}

__WEAK int mbedtls_md_hmac_finish( mbedtls_md_context_t *ctx, unsigned char *output )
{
    int ret;
    unsigned char tmp[MBEDTLS_MD_MAX_SIZE];
    unsigned char *opad;

    if( ctx == NULL || ctx->md_info == NULL || ctx->hmac_ctx == NULL )
        return( MBEDTLS_ERR_MD_BAD_INPUT_DATA );

    opad = (unsigned char *) ctx->hmac_ctx + ctx->md_info->block_size;

    if( ( ret = ctx->md_info->finish_func( ctx->md_ctx, tmp ) ) != 0 )
        return( ret );
    if( ( ret = ctx->md_info->starts_func( ctx->md_ctx ) ) != 0 )
        return( ret );
    if( ( ret = ctx->md_info->update_func( ctx->md_ctx, opad,
                                           ctx->md_info->block_size ) ) != 0 )
        return( ret );
    if( ( ret = ctx->md_info->update_func( ctx->md_ctx, tmp,
                                           ctx->md_info->size ) ) != 0 )
        return( ret );
    return( ctx->md_info->finish_func( ctx->md_ctx, output ) );
}

__WEAK int mbedtls_md_hmac( const mbedtls_md_info_t *md_info,
                     const unsigned char *key, size_t keylen,
                     const unsigned char *input, size_t ilen,
                     unsigned char *output )
{
    mbedtls_md_context_t ctx = {0};
    mbedtls_sha256_context sha = {0};
    uint8_t hmac[128] = {0};
    int ret;

    if( md_info == NULL )
        return( MBEDTLS_ERR_MD_BAD_INPUT_DATA );

    if( md_info->type == MBEDTLS_MD_SHA256 ) {
        ctx.md_info  = md_info;
        ctx.md_ctx   = &sha;
        ctx.hmac_ctx = hmac;
    } else {
        return( MBEDTLS_ERR_MD_BAD_INPUT_DATA );
    }

//    if( ( ret = mbedtls_md_setup( &ctx, md_info, 1 ) ) != 0 )
//        goto cleanup;
    if( ( ret = mbedtls_md_hmac_starts( &ctx, key, keylen ) ) != 0 )
        goto cleanup;
    if( ( ret = mbedtls_md_hmac_update( &ctx, input, ilen ) ) != 0 )
        goto cleanup;
    if( ( ret = mbedtls_md_hmac_finish( &ctx, output ) ) != 0 )
        goto cleanup;

cleanup:
//    mbedtls_md_free( &ctx );

    return( ret );
}

__WEAK int mbedtls_md_process( mbedtls_md_context_t *ctx, const unsigned char *data )
{
    if( ctx == NULL || ctx->md_info == NULL )
        return( MBEDTLS_ERR_MD_BAD_INPUT_DATA );

    return( ctx->md_info->process_func( ctx->md_ctx, data ) );
}

#endif /* MBEDTLS_MD_C */
#endif
