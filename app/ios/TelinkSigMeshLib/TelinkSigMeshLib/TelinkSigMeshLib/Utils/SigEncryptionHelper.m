/********************************************************************************************************
 * @file     SigEncryptionHelper.m
 *
 * @brief    for TLSR chips
 *
 * @author     telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *             The information contained herein is confidential and proprietary property of Telink
 *              Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *             of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *             Co., Ltd. and the licensee in separate contract or the terms described here-in.
 *           This heading MUST NOT be removed from this file.
 *
 *              Licensees are granted free, non-transferable use of the information in this
 *             file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/
//
//  SigEncryptionHelper.m
//  TelinkSigMeshLib
//
//  Created by Liangjiazhi on 2019/8/22.
//  Copyright © 2019年 Telink. All rights reserved.
//

#import "SigEncryptionHelper.h"
#import "SigLogger.h"
#import "GMEllipticCurveCrypto.h"

@interface SigEncryptionHelper ()
@property (nonatomic, strong) NSData *publicKeyLowIos10;
@property (nonatomic, strong) NSData *privateKeyLowIos10;
@property (nonatomic, strong) GMEllipticCurveCrypto *crypto;

@end

@implementation SigEncryptionHelper

+ (SigEncryptionHelper *)share{
    static SigEncryptionHelper *shareHelper = nil;
    static dispatch_once_t tempOnce=0;
    dispatch_once(&tempOnce, ^{
        shareHelper = [[SigEncryptionHelper alloc] init];
    });
    return shareHelper;
}

- (void)eccInit {
    __weak typeof(self) weakSelf = self;
    [self getECCKeyPair:^(NSData * _Nonnull publicKey, NSData * _Nonnull privateKey) {
        TeLogInfo(@"init ECC successful, publicKey=%@,privateKey=%@",weakSelf.publicKeyLowIos10,weakSelf.privateKeyLowIos10);
    }];
}

///返回手机端64字节的ECC公钥
- (NSData *)getPublicKeyData {
    return self.publicKeyLowIos10;
}

- (void)getECCKeyPair:(keyPair)pair{
    _crypto = [GMEllipticCurveCrypto generateKeyPairForCurve:GMEllipticCurveSecp256r1];
    _crypto.compressedPublicKey = NO;
    self.publicKeyLowIos10 = [_crypto.publicKey subdataWithRange:NSMakeRange(1, _crypto.publicKey.length-1)];
    self.privateKeyLowIos10 = _crypto.privateKey;
    if (pair) {
        pair(self.publicKeyLowIos10,self.privateKeyLowIos10);
    }
}

- (NSData *)getSharedSecretWithDevicePublicKey:(NSData *)devicePublicKey {
    UInt8 tem = 0x04;
    NSMutableData *devicePublicKeyData = [NSMutableData dataWithBytes:&tem length:1];
    [devicePublicKeyData appendData:devicePublicKey];
    GMEllipticCurveCrypto *deviceKeyCrypto = [GMEllipticCurveCrypto cryptoForKey:devicePublicKeyData];
    deviceKeyCrypto.compressedPublicKey = YES;
    NSData *sharedSecretKeyData = [_crypto sharedSecretForPublicKey:deviceKeyCrypto.publicKey];
    TeLogInfo(@"sharedSecretKeyData=%@",sharedSecretKeyData);
    return sharedSecretKeyData;
}

- (UInt8)aesAttDecryptionPacketOnlineStatusWithNetworkBeaconKey:(UInt8 *)key iv:(UInt8 *)iv ivLen:(UInt8)ivLen mic:(UInt8 *)mic micLen:(UInt8)micLen ps:(UInt8 *)ps psLen:(int)psLen {
    if(ivLen > 15){
        return 0;
    }

    if(psLen < 0){
        return 0;   // failed
    }

    UInt8 len = (UInt8)psLen;

    UInt8    e[16], r[16];

    ///////////////// calculate enc ////////////////////////
    memset (r, 0, 16);
    memcpy (r+1, iv, ivLen);
    for (int i=0; i<len; i++)
    {
        if ((i&15) == 0)
        {
            tn_aes_128 (key, r, e);
            r[0]++;
        }
        ps[i] ^= e[i & 15];
    }

    ///////////// calculate mic ///////////////////////
    memset (r, 0, 16);
    memcpy (r, iv, ivLen);
    r[ivLen] = len;
    tn_aes_128 (key, r, r);

    for (int i=0; i<len; i++)
    {
        r[i & 15] ^= ps[i];

        if ((i&15) == 15 || i == len - 1)
        {
            tn_aes_128 (key, r, r);
        }
    }

    for (int i=0; i<micLen; i++)
    {
        if (mic[i] != r[i])
        {
            return 0;            //Failed
        }
    }
    return 1;
}





void    mz_mul2 (unsigned int * r, unsigned int * a, int na, unsigned int b)
{
    int i=0;
    uint64_t m=0;
    uint64_t c=0;
    for (i=0; i<na; i++)
    {
        c += r[i];
        m = a[i] * (uint64_t)b + c;
        r[i] = (unsigned int)m;
        c = m >> 32;
    }
    r[i]= (unsigned int)c;
}

////////////////////////////////////////////////////////////////////////////////////
//        AES_CMAC
/////////////////////////////////////////////////////////////////////////////////////
const unsigned char const_Zero_Rb[20] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x87
};

#define        const_Rb            (const_Zero_Rb + 4)

void    mz_mul2 (unsigned int * r, unsigned int * a, int na, unsigned int b);

void tn_aes_xor_128(unsigned char *a, unsigned char *b, unsigned char *out)
{
    int i;
    for (i=0;i<16; i++)
    {
        out[i] = a[i] ^ b[i];
    }
}

/* AES-CMAC Generation Function */

void tn_aes_leftshift_onebit(unsigned char *input,unsigned char *output)
{
    int i;
    unsigned char overflow = 0;
    for ( i=15; i>=0; i-- ) {
        unsigned char in = input[i];
        output[i] = in << 1;
        output[i] |= overflow;
        overflow = (in & 0x80)?1:0;
    }
    return;
}

void tn_aes_padding ( unsigned char *lastb, unsigned char *pad, int length )
{
    int j;
    /* original last block */
    for ( j=0; j<16; j++ ) {
        if ( j < length ) {
            pad[j] = lastb[j];
        } else if ( j == length ) {
            pad[j] = 0x80;
        } else {
            pad[j] = 0x00;
        }
    }
}

void tn_aes_cmac ( unsigned char *key, unsigned char *input, int length,
                  unsigned char *mac )
{
    unsigned char       K[16]={}, L[16]={}, X[16]={};
    int         n, i, flag;
    
    n = (length+15) / 16;       /* n is number of rounds */
    
    if ( n == 0 ) {
        n = 1;
        flag = 0;
    } else {
        if ( (length%16) == 0 ) { /* last block is a complete block */
            flag = 1;
        } else { /* last block is not complete block */
            flag = 0;
        }
        
    }
    
    //////////////// Generate K1 or K2 => K ///////////////
    tn_aes_128(key,(unsigned char *)const_Zero_Rb,L);
    
    if ( (L[0] & 0x80) == 0 ) { /* If MSB(L) = 0, then K1 = L << 1 */
        tn_aes_leftshift_onebit(L,K);
    } else {    /* Else K1 = ( L << 1 ) (+) Rb */
        
        tn_aes_leftshift_onebit(L,K);
        tn_aes_xor_128(K,(unsigned char *)const_Rb,K);
    }
    
    if (!flag)
    {
        if ( (K[0] & 0x80) == 0 ) {
            tn_aes_leftshift_onebit(K,K);
        } else {
            tn_aes_leftshift_onebit(K,L);
            tn_aes_xor_128(L,(unsigned char *)const_Rb,K);
        }
    }
    ///////////////////////////////////////////////////////
    
    if ( flag ) { /* last block is complete block */
        tn_aes_xor_128(&input[16*(n-1)],K,K);
    } else {
        tn_aes_padding(&input[16*(n-1)],L,length%16);
        tn_aes_xor_128(L,K,K);
    }
    
    for ( i=0; i<16; i++ ) X[i] = 0;
    for ( i=0; i<n-1; i++ ) {
        tn_aes_xor_128(X,&input[16*i],X); /* Y := Mi (+) X  */
        tn_aes_128(key,X,X);      /* X := AES-128(KEY, Y); */
    }
    
    tn_aes_xor_128(X,K,X);
    tn_aes_128(key,X,X);
    memcpy (mac, X, 16);
}

////////////////////////////////////////////////////////////////////////////////////
const unsigned int tn_p256_nq[9] = { 0xeedf9bfe, 0x012ffd85, 0xdf1a6c21, 0x43190552,
    0xffffffff, 0xfffffffe, 0xffffffff, 0x00000000, 0x00000001};

const unsigned int tn_p256_n[10] = {1, 8,
    0xfc632551, 0xf3b9cac2, 0xa7179e84, 0xbce6faad,
    0xffffffff, 0xffffffff, 0x00000000, 0xffffffff };

const unsigned int tn_p256_pq[9] = { 0x00000003, 0x00000000, 0xffffffff, 0xfffffffe,
    0xfffffffe, 0xfffffffe, 0xffffffff, 0x00000000,  0x00000001};

const unsigned int tn_p256_pr[8] = {0x00000003, 0x00000000, 0xffffffff, 0xfffffffb,
    0xfffffffe, 0xffffffff, 0xfffffffd, 0x00000004};

const unsigned int tn_p256_p[10] = {    1, 8,
    0xffffffff, 0xffffffff, 0xffffffff, 0x00000000,
    0x00000000, 0x00000000, 0x00000001, 0xffffffff };

const tn_ecp_point tn_p256_ecp_g = {
    {    1,    8, {0xd898c296, 0xf4a13945, 0x2deb33a0, 0x77037d81, 0x63a440f2, 0xf8bce6e5, 0xe12c4247, 0x6b17d1f2} },
    {    1,    8, {0x37bf51f5, 0xcbb64068, 0x6b315ece, 0x2bce3357, 0x7c0f9e16, 0x8ee7eb4a, 0xfe1a7f9b, 0x4fe342e2} },
    {    1,    1, {1, 0, 0, 0,  0, 0, 0, 0 } }
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void    tn_mpn_mul (unsigned int * r, unsigned int * a, int na, unsigned int * b, int nb)
{
    int k, i;
    u8 tmp;
    unsigned int *pr, *pb;
    for (k=0; k<na + nb; k++)
    {
        r[k] = 0;
    }
    i = nb;
    pr = r;
    pb = b;
    tmp = irq_disable();
    while (i--)
    {
        mz_mul2 (pr++, a, na, *pb++);
    }
    irq_restore(tmp);
}

unsigned int tn_mpn_sub_1 (unsigned int * rp, unsigned int * ap, int n, unsigned int b)
{
    int i;
    unsigned int a, cy;
    i = 0;
    do
    {
        a = ap[i];
        cy = a < b;;
        rp[i] = a - b;
        b = cy;
    }
    while (++i < n);
    
    return b;
}

unsigned int tn_mpn_sub_n (unsigned int * rp, unsigned int * ap, unsigned int * bp, int n)
{
    int i;
    unsigned int cy, a, b;
    
    for (i = 0, cy = 0; i < n; i++)
    {
        a = ap[i]; b = bp[i];
        b += cy;
        cy = (b < cy);
        cy += (a < b);
        rp[i] = a - b;
    }
    return cy;
}

unsigned int tn_mpn_sub (unsigned int * rp, unsigned int * ap, int an, unsigned int * bp, int bn)
{
    unsigned int cy;
    
    cy = tn_mpn_sub_n (rp, ap, bp, bn);
    if (an > bn)
        cy = tn_mpn_sub_1 (rp + bn, ap + bn, an - bn, cy);
    return cy;
}

int tn_mpn_mod_p256p (unsigned int * r, int n)
{
    unsigned int    d, *pb;
    unsigned int    buff[28]={};
    unsigned int    *m2e16 = (unsigned int *)tn_p256_pq;
    unsigned int    *m = (unsigned int *)(tn_p256_p + 2);
    pb = buff + 1;
    pb[n + 6] = 0;
    pb[n + 7] = 0;
    pb[n + 8] = 0;
    
    if (n < 8)
        return n;
    
    tn_mpn_mul (pb, r, n, m2e16, 9);
    
    // r * m2^16 / (1 << 16)
    tn_mpn_mul (buff, m, 8, pb + 16, n - 7);
    
    // r - ma
    d = tn_mpn_sub (r, r, n, buff, n);
    
    for (; n > 0 && r[n-1] == 0; n--);
    
    if (n < 8 || d)
    {
        return n;
    }
    
    d = tn_mpn_sub (buff, r, n, m, 8);
    if (!d)        // > m
    {
        d = tn_mpn_sub (r, buff, n, m, 8);
        if (d)        // <m
        {
            memcpy (r, buff, 8 * 4);
        }
    }
    
    for (n = 8; n > 0 && r[n-1] == 0; n--);
    
    //    while (1);
    return n;
}

int tn_mpn_mod_p256n (unsigned int * r, int n)
{
    unsigned int    d, *pb;
    unsigned int    buff[28]={};
    unsigned int    *m2e16 = (unsigned int *)tn_p256_nq;
    unsigned int    *m = (unsigned int *)(tn_p256_n + 2);
    pb = buff + 1;
    pb[n + 6] = 0;
    pb[n + 7] = 0;
    pb[n + 8] = 0;
    
    if (n < 8)
        return n;
    
    tn_mpn_mul (pb, r, n, m2e16, 9);
    
    // r * m2^16 / (1 << 16)
    tn_mpn_mul (buff, m, 8, pb + 16, n - 7);
    
    // r - ma
    d = tn_mpn_sub (r, r, n, buff, n);
    
    for (; n > 0 && r[n-1] == 0; n--);
    
    if (n < 8 || d)
    {
        return n;
    }
    
    d = tn_mpn_sub (buff, r, n, m, 8);
    if (!d)        // > m
    {
        d = tn_mpn_sub (r, buff, n, m, 8);
        if (d)        // <m
        {
            memcpy (r, buff, 8 * 4);
        }
    }
    
    for (n = 8; n > 0 && r[n-1] == 0; n--);
    
    //    while (1);
    return n;
}

int    tn_mpn_mulmod (unsigned int * r, unsigned int * a, int na, unsigned int * b, int nb)
{
    int i=0;
    unsigned int    r16[16]={};
    tn_mpn_mul (r16, a, na, b, nb);
    i = tn_mpn_mod_p256p (r16, na + nb);
    memcpy (r, r16, 8 * 4);
    return i;
}

int    tn_mpn_mulmodn (unsigned int * r, unsigned int * a, int na, unsigned int * b, int nb)
{
    int i;
    unsigned int    r16[16]={};
    tn_mpn_mul (r16, a, na, b, nb);
    i = tn_mpn_mod_p256n (r16, na + nb);
    memcpy (r, r16, 8 * 4);
    return i;
}
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////

void tn_mpi_init( tn_mpi *X, int n)
{
    if( X == 0 )
        return;
    
    X->s = 1;
    X->n = n;
    //memset (X->p, 0, n * 4);
}


void tn_mpi_free( tn_mpi *X )
{
    if( X == 0 )
        return;
    
    memset( X->p, 0, X->n * 4 );
    X->s = 1;
}

int tn_mpi_lset( tn_mpi *X, int z )
{
    memset( X->p, 0, X->n * 4 );
    X->n = 8;
    X->p[0] = ( z < 0 ) ? -z : z;
    X->s    = ( z < 0 ) ? -1 : 1;
    return 0;
}

int tn_mpi_get_bit( const tn_mpi *X, int pos )
{
    if( X->n * biL <= pos )
        return( 0 );
    
    return( ( X->p[pos / biL] >> ( pos % biL ) ) & 0x01 );
}

int tn_mpi_cmp_mpi( const tn_mpi *X, const tn_mpi *Y )
{
    size_t i, j;
    
    for( i = X->n; i > 0; i-- )
        if( X->p[i - 1] != 0 )
            break;
    
    for( j = Y->n; j > 0; j-- )
        if( Y->p[j - 1] != 0 )
            break;
    
    if( i == 0 && j == 0 )
        return( 0 );
    
    if( i > j ) return(  X->s );
    if( j > i ) return( -Y->s );
    
    if( X->s > 0 && Y->s < 0 ) return(  1 );
    if( Y->s > 0 && X->s < 0 ) return( -1 );
    
    for( ; i > 0; i-- )
    {
        if( X->p[i - 1] > Y->p[i - 1] ) return(  X->s );
        if( X->p[i - 1] < Y->p[i - 1] ) return( -X->s );
    }
    
    return( 0 );
}

int tn_mpi_copy( tn_mpi *X, const tn_mpi *Y )
{
    //int ret;
    size_t i;
    
    if( X == Y )
        return( 0 );
    
    if( Y->p[0] == 0 || Y->n == 0 )
    {
        tn_mpi_free( X );
        return( 0 );
    }
    
    for( i = Y->n - 1; i > 0; i-- )
        if( Y->p[i] != 0 )
            break;
    i++;
    
    memset( X->p, 0, X->n * 4 );
    memcpy( X->p, Y->p, i * 4 );
    
    X->s = Y->s;
    X->n = Y->n;
    
    return( 0 );
}

int tn_mpi_cmp_abs( const tn_mpi *X, const tn_mpi *Y )
{
    size_t i, j;
    
    for( i = X->n; i > 0; i-- )
        if( X->p[i - 1] != 0 )
            break;
    
    for( j = Y->n; j > 0; j-- )
        if( Y->p[j - 1] != 0 )
            break;
    
    if( i == 0 && j == 0 )
        return( 0 );
    
    if( i > j ) return(  1 );
    if( j > i ) return( -1 );
    
    for( ; i > 0; i-- )
    {
        if( X->p[i - 1] > Y->p[i - 1] ) return(  1 );
        if( X->p[i - 1] < Y->p[i - 1] ) return( -1 );
    }
    
    return( 0 );
}

static void tn_mpi_sub_hlp( size_t n, const unsigned int *s, unsigned int *d )
{
    size_t i;
    unsigned int c, z;
    
    for( i = c = 0; i < n; i++, s++, d++ )
    {
        z = ( *d <  c );     *d -=  c;
        c = ( *d < *s ) + z; *d -= *s;
    }
    
    while( c != 0 )
    {
        z = ( *d < c ); *d -= c;
        c = z; i++; d++;
    }
}

int tn_mpi_sub_abs( tn_mpi *X, const tn_mpi *A, const tn_mpi *B )
{
    tn_mpi16 TB={};
    int ret;
    size_t n;
    
    if( tn_mpi_cmp_abs( A, B ) < 0 )
        return( TNDTLS_ERR_MPI_NEGATIVE_VALUE );
    
    tn_mpi_init((tn_mpi *) &TB, 16);
    
    if( X == B )
    {
        tn_mpi_copy( (tn_mpi *)&TB, B );
        B = (tn_mpi *)&TB;
    }
    
    if( X != A )
        tn_mpi_copy( X, A );
    
    X->s = 1;
    
    ret = 0;
    
    for( n = B->n; n > 0; n-- )
        if( B->p[n - 1] != 0 )
            break;
    
    tn_mpi_sub_hlp( n, B->p, X->p );
    
    return( ret );
}

int tn_mpi_add_abs( tn_mpi *X, const tn_mpi *A, const tn_mpi *B )
{
    int ret = 0;
    size_t i, j;
    unsigned int *o, *p, c;
    
    if( X == B )
    {
        const tn_mpi *T = A; A = X; B = T;
    }
    
    if( X != A )
    {
        tn_mpi_copy( X, A );
        X->p[A->n] = 0;
        X->n = A->n + 1;
    }
    
    X->s = 1;
    
    for( j = B->n; j > 0; j-- )
        if( B->p[j - 1] != 0 )
            break;
    
    if (X->n < (int)j)
        X->n = (int)j;
    
    o = (unsigned int *)B->p; p = X->p; c = 0;
    
    for( i = 0; i < j; i++, o++, p++ )
    {
        *p +=  c; c  = ( *p <  c );
        *p += *o; c += ( *p < *o );
    }
    
    while( c != 0 )
    {
        if( (int)i >= X->n )
        {
            X->n = (int)i + 1;
            p = X->p + i;
        }
        
        *p += c; c = ( *p < c ); i++; p++;
    }
    
    return( ret );
}

int tn_mpi_sub_mpi( tn_mpi *X, const tn_mpi *A, const tn_mpi *B )
{
    int s = A->s;
    
    if( A->s * B->s > 0 )
    {
        if( tn_mpi_cmp_abs( A, B ) >= 0 )
        {
            tn_mpi_sub_abs( X, A, B );
            X->s =  s;
        }
        else
        {
            tn_mpi_sub_abs( X, B, A );
            X->s = -s;
        }
    }
    else
    {
        tn_mpi_add_abs( X, A, B );
        X->s = s;
    }
    
    return 0;
}

int tn_mpi_add_mpi( tn_mpi *X, const tn_mpi *A, const tn_mpi *B )
{
    int s = A->s;
    
    if( A->s * B->s < 0 )
    {
        if( tn_mpi_cmp_abs( A, B ) >= 0 )
        {
            tn_mpi_sub_abs( X, A, B );
            X->s =  s;
        }
        else
        {
            tn_mpi_sub_abs( X, B, A );
            X->s = -s;
        }
    }
    else
    {
        tn_mpi_add_abs( X, A, B );
        X->s = s;
    }
    
    return( 0 );
}

void tn_mpi_modn_add (tn_mpi * px)
{
    int j;
    while( tn_mpi_cmp_mpi( px, (const tn_mpi *)tn_p256_n ) >= 0 )
    {
        tn_mpi_sub_abs( px, px, (const tn_mpi *)tn_p256_n );
    }
    for( j = px->n; j > 0; j-- )
        if( px->p[j - 1] != 0 )
            break;
    px->n = j;
}

void tn_mpi_modp_add (tn_mpi * px)
{
    int j;
    while( tn_mpi_cmp_mpi( px, (const tn_mpi *)tn_p256_p ) >= 0 )
    {
        tn_mpi_sub_abs( px, px, (const tn_mpi *)tn_p256_p );
    }
    for( j = px->n; j > 0; j-- )
        if( px->p[j - 1] != 0 )
            break;
    px->n = j;
}

void tn_mpi_modp_sub (tn_mpi * px)
{
    int j;
    while( px->s < 0 && tn_mpi_cmp_int( px, 0 ) != 0 )
    {
        tn_mpi_add_mpi( px, px, (const tn_mpi *)tn_p256_p );
    }
    for( j = px->n; j > 0; j-- )
        if( px->p[j - 1] != 0 )
            break;
    px->n = j;
}

void tn_mpi_modn_sub (tn_mpi * px)
{
    int j;
    while( px->s < 0 && tn_mpi_cmp_int( px, 0 ) != 0 )
    {
        tn_mpi_add_mpi( px, px, (const tn_mpi *)tn_p256_n );
    }
    for( j = px->n; j > 0; j-- )
        if( px->p[j - 1] != 0 )
            break;
    px->n = j;
}
int tn_mpi_modp( tn_mpi *R, const tn_mpi *A)
{
    tn_mpi_copy (R, A);
    
    while( tn_mpi_cmp_int( R, 0 ) < 0 )
    {
        tn_mpi_add_mpi( R, R, (const tn_mpi *)tn_p256_p );
    }
    
    while( tn_mpi_cmp_mpi( R, (const tn_mpi *)tn_p256_p ) >= 0 )
    {
        tn_mpi_sub_abs( R, R, (const tn_mpi *)tn_p256_p );
    }
    
    return 0;
}

int tn_mpi_sub_mpi_modn( tn_mpi *X, const tn_mpi *A, const tn_mpi *B )
{
    tn_mpi16 xx={};
    
    tn_mpi *px = (tn_mpi *)&xx;
    
    tn_mpi_init (px, 16);
    
    tn_mpi_sub_mpi (px, A, B);
    
    tn_mpi_modn_sub (px);
    
    tn_mpi_copy( X, px);
    
    return 0;
}

int tn_mpi_add_mpi_modp( tn_mpi *X, const tn_mpi *A, const tn_mpi *B )
{
    tn_mpi16 xx={};
    
    tn_mpi *px = (tn_mpi *)&xx;
    
    tn_mpi_init (px, 16);
    
    tn_mpi_add_mpi (px, A, B);
    
    tn_mpi_modp_add (px);
    
    tn_mpi_copy( X, px);
    
    return 0;
}

int tn_mpi_sub_mpi_modp( tn_mpi *X, const tn_mpi *A, const tn_mpi *B )
{
    tn_mpi16 xx={};
    
    tn_mpi *px = (tn_mpi *)&xx;
    
    tn_mpi_init (px, 16);
    
    tn_mpi_sub_mpi (px, A, B);
    
    tn_mpi_modp_sub (px);
    
    tn_mpi_copy( X, px);
    
    return 0;
}

int tn_mpi_cmp_int( tn_mpi *X, int z )
{
    tn_mpi Y={};
    
    tn_mpi_init (&Y, 8);
    tn_mpi_lset (&Y, z);
    
    return( tn_mpi_cmp_mpi( X, &Y ) );
}

// Conditionally assign X = Y, without leaking information
// about whether the assignment was made or not.
// (Leaking information about the respective sizes of X and Y is ok however.)
int tn_mpi_safe_cond_assign( tn_mpi *X, const tn_mpi *Y, unsigned char assign )
{
    int ret = 0;
    size_t i;
    
    /* make sure assign is 0 or 1 in a time-constant manner */
    assign = (assign | (unsigned char)-assign) >> 7;
    
    X->s = X->s * ( 1 - assign ) + Y->s * assign;
    
    for( i = 0; (int)(i) < Y->n; i++ )
        X->p[i] = X->p[i] * ( 1 - assign ) + Y->p[i] * assign;
    
    for( ; (int)(i) < X->n; i++ )
        X->p[i] *= ( 1 - assign );
    
    return( ret );
}

//////////////////////////////////////////////////////////////////////////////
int tn_ecp_copy( tn_ecp_point *P, const tn_ecp_point *Q )
{
    tn_mpi_copy( &P->X, &Q->X );
    tn_mpi_copy( &P->Y, &Q->Y );
    tn_mpi_copy( &P->Z, &Q->Z );
    return 0;
}

// X = A * B mod n
int tn_mpi_mul_mpi_modn( tn_mpi *X, const tn_mpi *A, const tn_mpi *B )
{
    int i, j;
    
    for( i = A->n; i > 0; i-- )
        if( A->p[i - 1] != 0 )
            break;
    
    for( j = B->n; j > 0; j-- )
        if( B->p[j - 1] != 0 )
            break;
    
    X->n = tn_mpn_mulmodn (X->p, (unsigned int *)A->p, i, (unsigned int *)B->p, j);
    
    X->s = A->s * B->s;
    
    return 0;
}

int tn_mpi_neg_modn( tn_mpi *X )
{
    X->s = 1;
    tn_mpi_sub_mpi( X, (const tn_mpi *)tn_p256_n, X );
    return 0;
}

// X = A * B mod n
int tn_mpi_mul_mpi_modp( tn_mpi *X, const tn_mpi *A, const tn_mpi *B )
{
    int i, j;
    
    for( i = A->n; i > 0; i-- )
        if( A->p[i - 1] != 0 )
            break;
    
    for( j = B->n; j > 0; j-- )
        if( B->p[j - 1] != 0 )
            break;
    
    X->n = tn_mpn_mulmod (X->p, (unsigned int *)A->p, i, (unsigned int *)B->p, j);
    
    X->s = A->s * B->s;
    
    return 0;
}

int tn_mpi_mul_int_modp( tn_mpi *X, const tn_mpi *A,  int n )
{
    int i;
    tn_mpi B={};
    tn_mpi_init (&B, 8);
    tn_mpi_lset (&B, n);
    
    for( i = A->n; i > 0; i-- )
        if( A->p[i - 1] != 0 )
            break;
    
    X->n = tn_mpn_mulmod (X->p, (unsigned int *)A->p, i, (unsigned int *)&B.p, 1);
    
    X->s = A->s * B.s;
    
    return 0;
}

int tn_mpi_bitlen( const tn_mpi *X )
{
    int i, j;
    
    if( X->n == 0 )
        return( 0 );
    
    for( i = X->n - 1; i > 0; i-- )
        if( X->p[i] != 0 )
            break;
    
    for( j = 32; j > 0; j-- )
        if( ( ( X->p[i] >> ( j - 1 ) ) & 1 ) != 0 )
            break;
    
    return( ( i * 32 ) + j );
}

int tn_mpi_shift_l( tn_mpi *X, size_t count )
{
    int i, v0, t1;
    unsigned int r0 = 0, r1;
    
    v0 = (int)count / (biL    );
    t1 = count & (biL - 1);
    
    i = tn_mpi_bitlen( X ) + (int)count;
    
    if( X->n * biL < i )
    {
        X->n = (i) / 32 + ( (i) % 32 != 0 );
    }
    
    /*
     * shift by count / limb_size
     */
    if( v0 > 0 )
    {
        for( i = X->n; i > v0; i-- )
            X->p[i - 1] = X->p[i - v0 - 1];
        
        for( ; i > 0; i-- )
            X->p[i - 1] = 0;
    }
    
    /*
     * shift by count % limb_size
     */
    if( t1 > 0 )
    {
        for( i = v0; i < X->n; i++ )
        {
            r1 = X->p[i] >> (biL - t1);
            X->p[i] <<= t1;
            X->p[i] |= r0;
            r0 = r1;
        }
    }
    
    return( 0 );
}

int tn_mpi_shift_r( tn_mpi *X, int count )
{
    size_t i, v0, v1;
    unsigned int r0 = 0, r1;
    
    v0 = count /  biL;
    v1 = count & (biL - 1);
    
    if( (int)(v0) > X->n || ( v0 == X->n && v1 > 0 ) )
        return tn_mpi_lset( X, 0 );
    
    /*
     * shift by count / limb_size
     */
    if( v0 > 0 )
    {
        for( i = 0; i < X->n - v0; i++ )
            X->p[i] = X->p[i + v0];
        
        for( ;(int)( i) < X->n; i++ )
            X->p[i] = 0;
    }
    
    /*
     * shift by count % limb_size
     */
    if( v1 > 0 )
    {
        for( i = X->n; i > 0; i-- )
        {
            r1 = X->p[i - 1] << (biL - v1);
            X->p[i - 1] >>= v1;
            X->p[i - 1] |= r0;
            r0 = r1;
        }
    }
    
    return( 0 );
}

//int tn_mpi_div_mpi( tn_mpi *Q, tn_mpi *R, const tn_mpi *A, const tn_mpi *B );

int tn_mpi_shift_l_modp( tn_mpi *X, size_t count )
{
    tn_mpi16 xx={};
    
    tn_mpi *px = (tn_mpi *)&xx;
    tn_mpi_init (px, 16);
    
    tn_mpi_copy (px, X);
    
    tn_mpi_shift_l (px, count);
    
    tn_mpi_modp_add (px);
    
    tn_mpi_copy( X, px);
    
    return 0;
}

int tn_mpi_add_mpi_modp2( tn_mpi *X, const tn_mpi *A, const tn_mpi *B )
{
    tn_mpi16 xx={};
    
    tn_mpi *px = (tn_mpi *)&xx;
    
    tn_mpi_init (px, 8);
    
    tn_mpi_add_mpi (px, A, B);
    
    tn_mpi_modp_add (px);
    
    tn_mpi_copy( X, px);
    
    return 0;
}

// Modular inverse: X = A^-1 mod p
int tn_mpi_inv_mod( tn_mpi *X, const tn_mpi *A )
{
    //int ret;
    tn_mpi *N;
    tn_mpi TA={}, TB={};
    tn_mpi16 TU={}, U1={}, U2={}, TV={}, V1={}, V2={};
    int nntn = 0;
    
    N = (tn_mpi *)tn_p256_p;
    
    tn_mpi_init( &TA, 8 ); tn_mpi_init( &TB, 8);
    tn_mpi_init( (tn_mpi *)&TU, 16 ); tn_mpi_init( (tn_mpi *)&U1, 16 ); tn_mpi_init((tn_mpi *) &U2, 16 );
    tn_mpi_init( (tn_mpi *)&TV, 16 ); tn_mpi_init( (tn_mpi *)&V1, 16 ); tn_mpi_init( (tn_mpi *)&V2, 16 );
    
    tn_mpi_modp( &TA, A );
    tn_mpi_copy( (tn_mpi *)&TU, &TA );
    tn_mpi_copy( &TB, N );
    tn_mpi_copy( (tn_mpi *)&TV, N );
    
    tn_mpi_lset( (tn_mpi *)&U1, 1 );
    tn_mpi_lset( (tn_mpi *)&U2, 0 );
    tn_mpi_lset( (tn_mpi *)&V1, 0 );
    tn_mpi_lset( (tn_mpi *)&V2, 1 );
    
    do
    {
        while( ( TU.p[0] & 1 ) == 0 )
        {
            tn_mpi_shift_r( (tn_mpi *)&TU, 1 );
            
            if( ( U1.p[0] & 1 ) != 0 || ( U2.p[0] & 1 ) != 0 )
            {
                tn_mpi_add_mpi( (tn_mpi *)&U1, (tn_mpi *)&U1, &TB );
                tn_mpi_sub_mpi( (tn_mpi *)&U2, (tn_mpi *)&U2, &TA );
            }
            
            tn_mpi_shift_r( (tn_mpi *)&U1, 1 );
            tn_mpi_shift_r( (tn_mpi *)&U2, 1 );
        }
        
        while( ( TV.p[0] & 1 ) == 0 )
        {
            tn_mpi_shift_r( (tn_mpi *)&TV, 1 );
            
            if( ( V1.p[0] & 1 ) != 0 || ( V2.p[0] & 1 ) != 0 )
            {
                tn_mpi_add_mpi( (tn_mpi *)&V1, (tn_mpi *)&V1, &TB );
                tn_mpi_sub_mpi( (tn_mpi *)&V2, (tn_mpi *)&V2, &TA );
            }
            
            tn_mpi_shift_r( (tn_mpi *)&V1, 1 );
            tn_mpi_shift_r( (tn_mpi *)&V2, 1 );
        }
        
        if( tn_mpi_cmp_mpi( (tn_mpi *)&TU, (tn_mpi *)&TV ) >= 0 )
        {
            tn_mpi_sub_mpi( (tn_mpi *)&TU, (tn_mpi *)&TU, (tn_mpi *)&TV );
            tn_mpi_sub_mpi( (tn_mpi *)&U1, (tn_mpi *)&U1, (tn_mpi *)&V1 );
            tn_mpi_sub_mpi( (tn_mpi *)&U2, (tn_mpi *)&U2, (tn_mpi *)&V2 );
        }
        else
        {
            tn_mpi_sub_mpi( (tn_mpi *)&TV, (tn_mpi *)&TV, (tn_mpi *)&TU );
            tn_mpi_sub_mpi( (tn_mpi *)&V1, (tn_mpi *)&V1, (tn_mpi *)&U1 );
            tn_mpi_sub_mpi( (tn_mpi *)&V2, (tn_mpi *)&V2, (tn_mpi *)&U2 );
        }
        nntn ++;
        if (nntn == 5)
        {
            //tn_mpi_copy( X, (tn_mpi *)&V1 );
            //return 0;
        }
    }
    while( tn_mpi_cmp_int( (tn_mpi *)&TU, 0 ) != 0 );
    
    while( tn_mpi_cmp_int( (tn_mpi *)&V1, 0 ) < 0 )
        tn_mpi_add_mpi( (tn_mpi *)&V1, (tn_mpi *)&V1, N );
    
    while( tn_mpi_cmp_mpi( (tn_mpi *)&V1, N ) >= 0 )
        tn_mpi_sub_mpi( (tn_mpi *)&V1, (tn_mpi *)&V1, N );
    
    V1.n = 8;
    tn_mpi_copy( X, (tn_mpi *)&V1 );
    
    return 0;
}

int tn_ecp_double_jac( tn_ecp_point *R, const tn_ecp_point *P )
{

    tn_mpi M={}, S={}, T={}, U={};
    
    tn_mpi_init( &M, 8 ); tn_mpi_init( &S, 8 ); tn_mpi_init( &T, 8 ); tn_mpi_init( &U, 8 );
    
    // Special case for A = -3:   M = 3(X + Z^2)(X - Z^2)
    tn_mpi_mul_mpi_modp ( &S,  &P->Z,  &P->Z   );
    tn_mpi_add_mpi_modp ( &T,  &P->X,  &S      );
    
    tn_mpi_sub_mpi_modp ( &U,  &P->X,  &S      ) ;
    tn_mpi_mul_mpi_modp ( &S,  &T,     &U      );
    tn_mpi_mul_int_modp ( &M,  &S,     3       );
    
    // S = 4.X.Y^2
    tn_mpi_mul_mpi_modp( &T,  &P->Y,  &P->Y   );
    tn_mpi_shift_l_modp( &T,  1               );
    tn_mpi_mul_mpi_modp( &S,  &P->X,  &T      );
    tn_mpi_shift_l_modp( &S,  1               );
    
    // U = 8.Y^4
    tn_mpi_mul_mpi_modp( &U,  &T,     &T      );
    tn_mpi_shift_l_modp( &U,  1               );
    
    // T = M^2 - 2.S
    tn_mpi_mul_mpi_modp( &T,  &M,     &M      );
    tn_mpi_sub_mpi_modp( &T,  &T,     &S      );
    tn_mpi_sub_mpi_modp( &T,  &T,     &S      );
    
    // S = M(S - T) - U
    tn_mpi_sub_mpi_modp( &S,  &S,     &T      );
    tn_mpi_mul_mpi_modp( &S,  &S,     &M      );
    tn_mpi_sub_mpi_modp( &S,  &S,     &U      );
    
    // U = 2.Y.Z
    tn_mpi_mul_mpi_modp( &U,  &P->Y,  &P->Z   );
    tn_mpi_shift_l_modp( &U,  1               );
    //tn_mpi_copy( &R->X, &U ); return 0;
    
    tn_mpi_copy( &R->X, &T );
    tn_mpi_copy( &R->Y, &S );
    tn_mpi_copy( &R->Z, &U );
    
    return 0;
}

int tn_ecp_add_mixed( tn_ecp_point *R, const tn_ecp_point *P, const tn_ecp_point *Q )
{
    tn_mpi T1={}, T2={}, T3={}, T4={}, X={}, Y={}, Z={};

    //Trivial cases: P == 0 or Q == 0 (case 1)
    if( tn_mpi_cmp_int( (tn_mpi*)&P->Z, 0 ) == 0 )
        return( tn_ecp_copy( R, Q ) );
    
//    if( 0 && tn_mpi_cmp_int( (tn_mpi*)&Q->Z, 0 ) == 0 )
//        return( tn_ecp_copy( R, P ) );
//    
//    // Make sure Q coordinates are normalized
//    if(0 && tn_mpi_cmp_int( (tn_mpi*)&Q->Z, 1 ) != 0 )
//        return( TNDTLS_ERR_MPI_BAD_INPUT_DATA );
    
    
    tn_mpi_init( &T1, 8 );
    tn_mpi_init( &T2, 8 );
    tn_mpi_init( &T3, 8 );
    tn_mpi_init( &T4, 8 );
    tn_mpi_init( &X, 8 );
    tn_mpi_init( &Y, 8 );
    tn_mpi_init( &Z, 8 );
    
    tn_mpi_mul_mpi_modp( &T1,  &P->Z,  &P->Z );
    tn_mpi_mul_mpi_modp( &T2,  &T1,    &P->Z );
    tn_mpi_mul_mpi_modp( &T1,  &T1,    &Q->X );
    tn_mpi_mul_mpi_modp( &T2,  &T2,    &Q->Y );
    tn_mpi_sub_mpi_modp( &T1,  &T1,    &P->X );
    tn_mpi_sub_mpi_modp( &T2,  &T2,    &P->Y );
    
    /* Special cases (2) and (3) */
    if( tn_mpi_cmp_int( &T1, 0 ) == 0 )
    {
        if( tn_mpi_cmp_int( &T2, 0 ) == 0 )
        {
            return tn_ecp_double_jac( R, P );
        }
        else
        {
            tn_mpi_lset( &R->X , 1 );
            tn_mpi_lset( &R->Y , 1 );
            tn_mpi_lset( &R->Z , 0 );
            return 0;
        }
    }
    
    tn_mpi_mul_mpi_modp( &Z,   &P->Z,  &T1   );
    tn_mpi_mul_mpi_modp( &T3,  &T1,    &T1   );
    tn_mpi_mul_mpi_modp( &T4,  &T3,    &T1   );
    tn_mpi_mul_mpi_modp( &T3,  &T3,    &P->X );
    tn_mpi_mul_int_modp( &T1,  &T3,    2     );
    tn_mpi_mul_mpi_modp( &X,   &T2,    &T2   );
    tn_mpi_sub_mpi_modp( &X,   &X,     &T1   );
    tn_mpi_sub_mpi_modp( &X,   &X,     &T4   );
    tn_mpi_sub_mpi_modp( &T3,  &T3,    &X    );
    tn_mpi_mul_mpi_modp( &T3,  &T3,    &T2   );
    tn_mpi_mul_mpi_modp( &T4,  &T4,    &P->Y );
    tn_mpi_sub_mpi_modp( &Y,   &T3,    &T4   );
    
    tn_mpi_copy( &R->X, &X );
    tn_mpi_copy( &R->Y, &Y );
    tn_mpi_copy( &R->Z, &Z );
    
    return 0;
}

////////////////////////////////////////////////////////
// mult
////////////////////////////////////////////////////////
int tn_ecp_normalize_jac( tn_ecp_point *pt )
{
    tn_mpi Zi={}, ZZi={};

    if( tn_mpi_cmp_int( &pt->Z, 0 ) == 0 )
        return( 0 );
    
    tn_mpi_init( &Zi, 8 ); tn_mpi_init( &ZZi, 8 );
    
    /*
     * X = X / Z^2  mod p
     */
    tn_mpi_inv_mod( &Zi,      &pt->Z );
    tn_mpi_mul_mpi_modp( &ZZi,     &Zi,        &Zi     );
    tn_mpi_mul_mpi_modp( &pt->X,   &pt->X,     &ZZi    );
    
    /*
     * Y = Y / Z^3  mod p
     */
    tn_mpi_mul_mpi_modp( &pt->Y,   &pt->Y,     &ZZi    );
    tn_mpi_mul_mpi_modp( &pt->Y,   &pt->Y,     &Zi     );
    
    /*
     * Z = 1
     */
    tn_mpi_lset( &pt->Z, 1 );
    
    return 0;
}

int tn_ecp_mul(tn_ecp_point *R, const tn_mpi *m, const tn_ecp_point *P)
{
    int i, b;
    tn_mpi_init (&R->X, 8);
    tn_mpi_init (&R->Y, 8);
    tn_mpi_init (&R->Z, 8);
    tn_mpi_lset (&R->X, 1);
    tn_mpi_lset (&R->Y, 1);
    tn_mpi_lset (&R->Z, 0);
    
    i = tn_mpi_bitlen( m );
    while( i-- > 0 )
    {
        tn_ecp_double_jac( R, R );
        b = tn_mpi_get_bit( m, i );
        if (b)
        {
            tn_ecp_add_mixed( R, R, P );
        }
    }
    tn_ecp_normalize_jac (R);
    return 0;
}

/*
 * R = m * P with shortcuts for m == 1 and m == -1
 * NOT constant-time - ONLY for short Weierstrass!
 */
static int tn_ecp_mul_shortcuts( tn_ecp_point *R, const tn_mpi *m, const tn_ecp_point *P )
{
    if( tn_mpi_cmp_int( (tn_mpi*)m, 1 ) == 0 )
    {
        tn_ecp_copy( R, P );
    }
    else if( tn_mpi_cmp_int( (tn_mpi*)m, -1 ) == 0 )
    {
        tn_ecp_copy( R, P );
        if( tn_mpi_cmp_int( &R->Y, 0 ) != 0 )
            tn_mpi_sub_mpi( &R->Y, (const tn_mpi *)tn_p256_p, &R->Y );
    }
    else
    {
        tn_ecp_mul( R, m, P);
    }
    
    return 0;
}

void tn_ecp_point_init( tn_ecp_point *pt )
{
    if( pt == 0 )
        return;
    
    tn_mpi_init( &pt->X, 8 );
    tn_mpi_init( &pt->Y, 8 );
    tn_mpi_init( &pt->Z, 8 );
    tn_mpi_lset (&pt->Z, 1 );
}


int tn_ecp_muladd( tn_ecp_point *R, const tn_mpi *m, const tn_ecp_point *P, const tn_mpi *n, const tn_ecp_point *Q )
{
    tn_ecp_point mP={};
    
    tn_ecp_point_init( &mP );
    
    tn_ecp_mul_shortcuts( &mP, m, P );
    tn_ecp_mul_shortcuts( R,   n, Q );
    
    tn_ecp_add_mixed( R, &mP, R );
    tn_ecp_normalize_jac( R );
    
    return 0;
}

/////////////////////////////////////////////////////////////////////
//  add3
//////////////////////////////////////////////////////////////////////
// Compute the sum of three points R = A + B + C
/*static */int tn_ecp_add3( tn_ecp_point *R, const tn_ecp_point *A, const tn_ecp_point *B, const tn_ecp_point *C )
{
    int ret = 0;
    tn_mpi one={};
    
    tn_mpi_init( &one, 8 );
    
    tn_mpi_lset( &one, 1 );
    tn_ecp_muladd( R, &one, A, &one, B );
    tn_ecp_muladd( R, &one, R, &one, C );
    
    return( ret );
}


/////////////////////////////////////////////////////////////////////
//  TLS_ECJPAKE_WITH_AES_128_CCM_8
//////////////////////////////////////////////////////////////////////
static int tn_ecjpake_write_byte32( unsigned char *pd, unsigned char *ps )
{
    int i;
    for (i=0; i<32; i++)
    {
        pd[i] = ps[31 - i];
    }
    return 32;
}


int    tn_mpi_random (tn_mpi * m,u8 *random,u8 random_len)
{
    unsigned char b;
    unsigned char *p;
    int i;
    if(random_len==0){
        static unsigned int x = 42;
        for (i=0; i<8; i++)
        {
            x = 1664525 * x + 1013904223;
            m->p[i] = x;
        }
        p = (unsigned char *)m->p;
        for (i=0; i<16; i++)
        {
            b = p[i];
            p[i] = p[31 - i];
            p[31 - i] = b;
        }
    }else{
        memcpy(m->p,random,random_len);
    }
    return  0;
}

/////////////////////////////////
void tn_ecp_gen_keypair (tn_mpi *d, tn_ecp_point *Q)
{
    d->n = 8;
    d->s = 1;
    tn_mpi_random (d,NULL,0);
    tn_mpi_modn_add (d);
    tn_ecp_mul( Q, d, (tn_ecp_point *)&tn_p256_ecp_g);
}
void tn_p256_keypair_sk(unsigned char *s,unsigned char *x,unsigned char *y)
{
    tn_mpi d={};
    tn_ecp_point /*k, */q={};
    d.n = 8;
    d.s = 1;
    unsigned char s_r[32]={};
    tn_ecjpake_write_byte32 (s_r, s);
    memcpy((unsigned char *)(d.p),s_r,32);
    tn_ecp_mul( &q, &d, (tn_ecp_point *)&tn_p256_ecp_g);
    tn_ecjpake_write_byte32 (x, (unsigned char *)q.X.p);
    tn_ecjpake_write_byte32 (y, (unsigned char *)q.Y.p);
}
void tn_p256_keypair (unsigned char *s, unsigned char *x, unsigned char *y)
{
    tn_mpi d={};
    tn_ecp_point /*k, */q={};
    d.n = 8;
    d.s = 1;
    tn_mpi_random (&d,NULL,0);
    tn_mpi_modn_add (&d);
    tn_ecp_mul( &q, &d, (tn_ecp_point *)&tn_p256_ecp_g);
    tn_ecjpake_write_byte32 (s, (unsigned char *)d.p);
    tn_ecjpake_write_byte32 (x, (unsigned char *)q.X.p);
    tn_ecjpake_write_byte32 (y, (unsigned char *)q.Y.p);
}

void tn_p256_keypair_mac (unsigned char *s, unsigned char *x, unsigned char *y,u8 *mac,u8 len )
{
    tn_mpi d = {};
    tn_ecp_point /*k, */q = {};
    d.n = 8;
    d.s = 1;
    tn_mpi_random (&d,mac,len);
    tn_mpi_modn_add (&d);
    tn_ecp_mul( &q, &d, (tn_ecp_point *)&tn_p256_ecp_g);
    tn_ecjpake_write_byte32 (s, (unsigned char *)d.p);
    tn_ecjpake_write_byte32 (x, (unsigned char *)q.X.p);
    tn_ecjpake_write_byte32 (y, (unsigned char *)q.Y.p);
}

void tn_p256_dhkey (unsigned char *r, unsigned char *s, unsigned char * x, unsigned char *y)
{
    tn_mpi d={};
    tn_ecp_point k={}, q={};
    
    tn_mpi_init (&d, 8);
    tn_ecjpake_write_byte32 ((unsigned char *)d.p, s);
    
    tn_mpi_init (&k.X, 8);
    tn_mpi_init (&k.Y, 8);
    tn_mpi_init (&k.Z, 8);
    tn_mpi_lset (&k.Z, 1);
    tn_ecjpake_write_byte32 ((unsigned char *)k.X.p, x);
    tn_ecjpake_write_byte32 ((unsigned char *)k.Y.p, y);
    tn_ecp_mul( &q, &d, &k);
    tn_ecjpake_write_byte32 (r, (unsigned char *)q.X.p);
}

static void    swap_order (int *p)
{
    int i;
    unsigned char * pc;
    for (i=0; i<8; i++)
    {
        pc = (unsigned char *)(p + i);
        p[i] = (pc[3]) | (pc[2] << 8) | (pc[1] << 16) | (pc[0] << 24);
    }
}
int test_const_sk_calc()
{
    u32 A_debug_sk_calc = 0;
    int ret;
    unsigned char x0[32]={}, x1[32]={}, y0[32]={}, y1[32]={}, k0[32]={}, k1[32]={};
    
    unsigned char s0[32] = {
        0x20,0xb0,0x03,0xd2,0xf2,0x97,0xbe,0x2c,0x5e,0x2c,0x83,0xa7,0xe9,0xf9,0xa5,0xb9,
        0xef,0xf4,0x91,0x11,0xac,0xf4,0xfd,0xdb,0xcc,0x03,0x01,0x48,0x0e,0x35,0x9d,0xe6};
    
    unsigned char s1[32] = {
        0x55,0x18,0x8b,0x3d,0x32,0xf6,0xbb,0x9a,0x90,0x0a,0xfc,0xfb,0xee,0xd4,0xe7,0x2a,
        0x59,0xcb,0x9a,0xc2,0xf1,0x9d,0x7c,0xfb,0x6b,0x4f,0xdd,0x49,0xf4,0x7f,0xc5,0xfd};
    //tn_ecjpake_write_byte32(s0_r,s0);
    //tn_ecjpake_write_byte32(s1_r,s1);
    tn_p256_keypair_sk(s0, x0, y0);
    tn_p256_keypair_sk(s1, x1, y1);
    tn_p256_dhkey (k0, s0, x1, y1);
    tn_p256_dhkey (k1, s1, x0, y0);
    ret = memcmp (k0, k1, 16);
    if(ret == 0){
        A_debug_sk_calc++;
    }else{
        A_debug_sk_calc=0x55;
    }
    return ret;
}

int test_dhkey()
{
    unsigned char s0[32]={}, s1[32]={}, x0[32]={}, x1[32]={}, y0[32]={}, y1[32]={}, k0[32]={}, k1[32]={}, dha[32]={}, dhb[32]={};
    //tn_mpi d0, d1;
    //tn_ecp_point e0, e1, r0, r1, r2, r3;
    //tn_ecp_point r;
    int ea, eb, ret;
    
    int a[8]    = {0x3f49f6d4, 0xa3c55f38, 0x74c9b3e3, 0xd2103f50, 0x4aff607b, 0xeb40b799, 0x5899b8a6, 0xcd3c1abd};
    
    int ay[8]    = {0xdc809c49, 0x652aeb6d, 0x63329abf, 0x5a52155c, 0x766345c2, 0x8fed3024, 0x741c8ed0, 0x1589d28b};
    int bx[8]    = {0x1ea1f0f0, 0x1faf1d96, 0x09592284, 0xf19e4c00, 0x47b58afd, 0x8615a69f, 0x559077b2, 0x2faaa190};
    int by[8]    = {0x4c55f33e, 0x429dad37, 0x7356703a, 0x9ab85160, 0x472d1130, 0xe28e3676, 0x5f89aff9, 0x15b1214a};
    int dh[8]    = {0xec0234a3, 0x57c8ad05, 0x341010a6, 0x0a397d9b, 0x99796b13, 0xb4f866f1, 0x868d34f3, 0x73bfa698};
    unsigned char ax[32] = {
        0x20,0xb0,0x03,0xd2,0xf2,0x97,0xbe,0x2c,0x5e,0x2c,0x83,0xa7,0xe9,0xf9,0xa5,0xb9,
        0xef,0xf4,0x91,0x11,0xac,0xf4,0xfd,0xdb,0xcc,0x03,0x01,0x48,0x0e,0x35,0x9d,0xe6};
    
    unsigned char b[32] = {
        0x55,0x18,0x8b,0x3d,0x32,0xf6,0xbb,0x9a,0x90,0x0a,0xfc,0xfb,0xee,0xd4,0xe7,0x2a,
        0x59,0xcb,0x9a,0xc2,0xf1,0x9d,0x7c,0xfb,0x6b,0x4f,0xdd,0x49,0xf4,0x7f,0xc5,0xfd};
    
    swap_order (a);
    swap_order (ay);
    
    swap_order (bx);
    swap_order (by);
    
    swap_order (dh);
    
    tn_p256_dhkey(dha, (unsigned char *)a, (unsigned char *)bx, (unsigned char *)by);
    //tn_p256_dhkey(dhb, (unsigned char *)b, (unsigned char *)ax, (unsigned char *)ay);
    tn_p256_dhkey(dhb, (unsigned char *)b, (unsigned char *)ax, (unsigned char *)ay);
    
    ea = memcmp (dha, dh, 32);
    if (ea)
        return 1;
    eb = memcmp (dhb, dh, 32);
    if (eb)
        return 2;
    
    
    tn_p256_keypair (s0, x0, y0);
    tn_p256_keypair (s1, x1, y1);
    tn_p256_dhkey (k0, s0, x1, y1);
    tn_p256_dhkey (k1, s1, x0, y0);
    ret = memcmp (k0, k1, 16);
    
    return ret;
}

//////////////////////////////////////////////////////////////////////////////////
//    cyprto function
//////////////////////////////////////////////////////////////////////////////////

void tn_aes_cmac ( unsigned char *key, unsigned char *input, int length,
                  unsigned char *mac );

//-- used for generate commite value Ca, Cab: f4(U, V, X, Z) = AES-CMACX (U || V || Z)  -------------------
int tn_crypto_f4 (unsigned char *r, unsigned char u[32], unsigned char v[32], unsigned char x[16], unsigned char z)
{
    unsigned char d[68]={};
    memcpy (d, u, 32);
    memcpy (d + 32, v, 32);
    d[64] = z;
    tn_aes_cmac (x, d, 65, r);
    return 0;
}

//-- used for numberic verification g(U, V, X, Y) = SHA-256(U || V || X || Y)  -------------------
unsigned int tn_crypto_g2 (unsigned char u[32], unsigned char v[32], unsigned char x[16], unsigned char y[16])
{
    unsigned char d[80]={}, z[32]={};
    memcpy (d, u, 32);
    memcpy (d + 32, v, 32);
    memcpy (d + 64, y, 16);
    
    tn_aes_cmac (x, d, 80, z);
    
    return z[12] | (z[13] << 8) | (z[14] << 16) | (z[15] << 24);;
}

//--------- f5(W, N1, N2, KeyID, A1, A2) = HMAC-SHA-256W (N1 || N2|| KeyID || A1|| A2) ---------------------
int tn_crypto_f5 (unsigned char *mac, unsigned char *ltk, unsigned char w[32], unsigned char n1[16], unsigned char n2[16],
                  unsigned char a1[7], unsigned char a2[7])
{
    unsigned char d[56]={}, t[16]={};
    const unsigned char s[16] = {0x6C,0x88, 0x83,0x91, 0xAA,0xF5, 0xA5,0x38, 0x60,0x37, 0x0B,0xDB, 0x5A,0x60, 0x83,0xBE};
    //const unsigned charkeyID[4] = {0x65, 0x6c, 0x74, 0x62};        //0x62746c65;
    //const unsigned charkeyID[4] = {0x62, 0x74, 0x6c, 0x65};        //0x62746c65;
    
    tn_aes_cmac ((unsigned char *)s, w, 32, t);
    
    d[0] = 0;
    d[1] = 0x62;
    d[2] = 0x74;
    d[3] = 0x6c;
    d[4] = 0x65;
    memcpy (d + 5, n1, 16);
    memcpy (d + 21, n2, 16);
    memcpy (d + 37, a1, 7);
    memcpy (d + 44, a2, 7);
    d[51] = 1;
    d[52] = 0;
    
    tn_aes_cmac (t, d, 53, mac);
    
    d[0] = 1;
    tn_aes_cmac (t, d, 53, ltk);
    
    return 0;
}

//------ f6(W, N1, N2, R, IOcap, A1, A2) = AES-CMACw (N1 || N2 || R || IOcap || A1 || A2)
int tn_crypto_f6 (unsigned char *e, unsigned char w[16], unsigned char n1[16], unsigned char n2[16],
                  unsigned char r[16], unsigned char iocap[3], unsigned char a1[7], unsigned char a2[7])
{
    unsigned char d[68]={};
    
    memcpy (d + 0, n1, 16);
    memcpy (d + 16, n2, 16);
    memcpy (d + 32, r, 16);
    memcpy (d + 48, iocap, 3);
    memcpy (d + 51, a1, 7);
    memcpy (d + 58, a2, 7);
    
    tn_aes_cmac (w, d, 65, e);
    
    return 0;
}

//------------ h6(W, keyID) = AES-CMACW(keyID ---------------------------
int tn_crypto_h6 (unsigned char *r, unsigned char key[16], unsigned char id[4])
{
    tn_aes_cmac (key, id, 4, r);
    return 0;
}


void swapN (unsigned char *p, int n)
{
    int i, c;
    for (i=0; i<n/2; i++)
    {
        c = p[i];
        p[i] = p[n - 1 - i];
        p[n - 1 - i] = c;
    }
}

int test_crypto_func()
{
    unsigned char u[32] = {
        0x20,0xb0,0x03,0xd2,0xf2,0x97,0xbe,0x2c, 0x5e,0x2c,0x83,0xa7,0xe9,0xf9,0xa5,0xb9,
        0xef,0xf4,0x91,0x11,0xac,0xf4,0xfd,0xdb, 0xcc,0x03,0x01,0x48,0x0e,0x35,0x9d,0xe6};
    unsigned char v[32] = {
        0x55,0x18,0x8b,0x3d,0x32,0xf6,0xbb,0x9a, 0x90,0x0a,0xfc,0xfb,0xee,0xd4,0xe7,0x2a,
        0x59,0xcb,0x9a,0xc2,0xf1,0x9d,0x7c,0xfb, 0x6b,0x4f,0xdd,0x49,0xf4,0x7f,0xc5,0xfd};
    unsigned char x[16] = {0xd5,0xcb,0x84,0x54,0xd1,0x77,0x73,0x3e,0xff,0xff,0xb2,0xec,0x71,0x2b,0xae,0xab};
    unsigned char y[16] = {0xa6,0xe8,0xe7,0xcc,0x25,0xa7,0x5f,0x6e,0x21,0x65,0x83,0xf7,0xff,0x3d,0xc4,0xcf};
    
    unsigned char f_z00[16] = {0xf2,0xc9,0x16,0xf1 ,0x07,0xa9,0xbd,0x1c ,0xf1,0xed,0xa1,0xbe ,0xa9,0x74,0x87,0x2d};
    //unsigned char f_z00[16] = {0xD3,0x01,0xCE,0x92,0xCC,0x7B,0x9E,0x3F,0x51,0xD2,0x92,0x4B,0x8B,0x33,0xFA,0xCA};
    //unsigned char f_z80[16] = {0x7E,0x43,0x11,0x12,0xC1,0x0D,0xE8,0xA3,0x98,0x4C,0x8A,0xC8,0x14,0x9F,0xF6,0xEC};
    
    unsigned char mac[16]={}, ltk[16]={};
    
    //-------
    unsigned char w[32] = {
        0xec,0x02,0x34,0xa3,0x57,0xc8,0xad,0x05,0x34,0x10,0x10,0xa6,0x0a,0x39,0x7d,0x9b,
        0x99,0x79,0x6b,0x13,0xb4,0xf8,0x66,0xf1,0x86,0x8d,0x34,0xf3,0x73,0xbf,0xa6,0x98};
    unsigned char n1[16] = {0xd5,0xcb,0x84,0x54,0xd1,0x77,0x73,0x3e,0xff,0xff,0xb2,0xec,0x71,0x2b,0xae,0xab};
    unsigned char n2[16] = {0xa6,0xe8,0xe7,0xcc,0x25,0xa7,0x5f,0x6e,0x21,0x65,0x83,0xf7,0xff,0x3d,0xc4,0xcf};
    //unsigned char keyID = 0x62746c65;
    unsigned char a1[7] = {0x00,0x56,0x12,0x37,0x37,0xbf,0xce};
    unsigned char a2[7] = {0x00,0xa7,0x13,0x70,0x2d,0xcf,0xc1};
    unsigned char f5_ltk[16] = {0x69,0x86,0x79,0x11 ,0x69,0xd7,0xcd,0x23 ,0x98,0x05,0x22,0xb5 ,0x94,0x75,0x0a,0x38};
    //unsigned char f5[16] = {0x47,0x30,0x0b,0xb9,0x5c,0x74,0x04,0x12,0x94,0x50,0x67,0x4b,0x17,0x41,0x10,0x4d};
    unsigned char f5_mac[16] = {0x29,0x65,0xf1,0x76 ,0xa1,0x08,0x4a,0x02 ,0xfd,0x3f,0x6a,0x20 ,0xce,0x63,0x6e,0x20};
    
    unsigned char r[16] = {0x12,0xa3,0x34,0x3b ,0xb4,0x53,0xbb,0x54 ,0x08,0xda,0x42,0xd2 ,0x0c,0x2d,0x0f,0xc8};
    unsigned char iocap[3] = {0x01, 0x01, 0x02};
    unsigned char f6[16] = {0xe3,0xc4,0x73,0x98 ,0x9c,0xd0,0xe8,0xc5 ,0xd2,0x6c,0x0b,0x09 ,0xda,0x95,0x8f,0x61};
    
    unsigned char g2[16] = {0x15,0x36,0xd1,0x8d ,0xe3,0xd2,0x0d,0xf9 ,0x9b,0x70,0x44,0xc1 ,0x2f,0x9e,0xd5,0xba};
    unsigned int g2int;
    
    //---- h6
    unsigned char h6_key[16] = {0xec,0x02,0x34,0xa3 ,0x57,0xc8,0xad,0x05 ,0x34,0x10,0x10,0xa6 ,0x0a,0x39,0x7d,0x9b};
    unsigned char h6_keyid[4] = {0x6c,0x65,0x62,0x72};
    unsigned char h6[16] = {0x2d,0x9a,0xe1,0x02 ,0xe7,0x6d,0xc9,0x1c ,0xe8,0xd3,0xa9,0xe2 ,0x80,0xb1,0x63,0x99};
    int ret;
    
    tn_crypto_f4 (mac, u, v, x, 0);
    ret = memcmp (mac, f_z00, 16);
    if (ret)
        return ret;
    
    tn_crypto_f5 (mac, ltk, w, n1, n2, a1, a2);
    ret = memcmp (mac, f5_mac, 16);
    if (ret)
        return ret;
    ret = memcmp (ltk, f5_ltk, 16);
    if (ret)
        return ret;
    
    tn_crypto_f6 (ltk, mac, n1, n2, r, iocap, a1, a2);
    ret = memcmp (ltk, f6, 16);
    if (ret)
        return ret;
    
    g2int = tn_crypto_g2 (u, v, x, y);
    ret = memcmp (g2 + 12, &g2int, 4);
    if (ret)
        return ret;
    
    tn_crypto_h6 (ltk, h6_key, h6_keyid);
    ret = memcmp (ltk, h6, 16);
    
    return ret;
}

static int irq_in_handler = 0;

static inline u8 irq_disable(){
    u8 r =0;
    return r;
}

static inline void irq_restore(u8 en){
    irq_in_handler = en;
}

static const word8 aes_sw_S[256] = {
    99, 124, 119, 123, 242, 107, 111, 197,  48,   1, 103,  43, 254, 215, 171, 118,
    202, 130, 201, 125, 250,  89,  71, 240, 173, 212, 162, 175, 156, 164, 114, 192,
    183, 253, 147,  38,  54,  63, 247, 204,  52, 165, 229, 241, 113, 216,  49,  21,
    4, 199,  35, 195,  24, 150,   5, 154,   7,  18, 128, 226, 235,  39, 178, 117,
    9, 131,  44,  26,  27, 110,  90, 160,  82,  59, 214, 179,  41, 227,  47, 132,
    83, 209,   0, 237,  32, 252, 177,  91, 106, 203, 190,  57,  74,  76,  88, 207,
    208, 239, 170, 251,  67,  77,  51, 133,  69, 249,   2, 127,  80,  60, 159, 168,
    81, 163,  64, 143, 146, 157,  56, 245, 188, 182, 218,  33,  16, 255, 243, 210,
    205,  12,  19, 236,  95, 151,  68,  23, 196, 167, 126,  61, 100,  93,  25, 115,
    96, 129,  79, 220,  34,  42, 144, 136,  70, 238, 184,  20, 222,  94,  11, 219,
    224,  50,  58,  10,  73,   6,  36,  92, 194, 211, 172,  98, 145, 149, 228, 121,
    231, 200,  55, 109, 141, 213,  78, 169, 108,  86, 244, 234, 101, 122, 174,   8,
    186, 120,  37,  46,  28, 166, 180, 198, 232, 221, 116,  31,  75, 189, 139, 138,
    112,  62, 181, 102,  72,   3, 246,  14,  97,  53,  87, 185, 134, 193,  29, 158,
    225, 248, 152,  17, 105, 217, 142, 148, 155,  30, 135, 233, 206,  85,  40, 223,
    140, 161, 137,  13, 191, 230,  66, 104,  65, 153,  45,  15, 176,  84, 187,  22,
};

static const word8 aes_sw_Si[256] = {
    82,   9, 106, 213,  48,  54, 165,  56, 191,  64, 163, 158, 129, 243, 215, 251,
    124, 227,  57, 130, 155,  47, 255, 135,  52, 142,  67,  68, 196, 222, 233, 203,
    84, 123, 148,  50, 166, 194,  35,  61, 238,  76, 149,  11,  66, 250, 195,  78,
    8,  46, 161, 102,  40, 217,  36, 178, 118,  91, 162,  73, 109, 139, 209,  37,
    114, 248, 246, 100, 134, 104, 152,  22, 212, 164,  92, 204,  93, 101, 182, 146,
    108, 112,  72,  80, 253, 237, 185, 218,  94,  21,  70,  87, 167, 141, 157, 132,
    144, 216, 171,   0, 140, 188, 211,  10, 247, 228,  88,   5, 184, 179,  69,   6,
    208,  44,  30, 143, 202,  63,  15,   2, 193, 175, 189,   3,   1,  19, 138, 107,
    58, 145,  17,  65,  79, 103, 220, 234, 151, 242, 207, 206, 240, 180, 230, 115,
    150, 172, 116,  34, 231, 173,  53, 133, 226, 249,  55, 232,  28, 117, 223, 110,
    71, 241,  26, 113,  29,  41, 197, 137, 111, 183,  98,  14, 170,  24, 190,  27,
    252,  86,  62,  75, 198, 210, 121,  32, 154, 219, 192, 254, 120, 205,  90, 244,
    31, 221, 168,  51, 136,   7, 199,  49, 177,  18,  16,  89,  39, 128, 236,  95,
    96,  81, 127, 169,  25, 181,  74,  13,  45, 229, 122, 159, 147, 201, 156, 239,
    160, 224,  59,  77, 174,  42, 245, 176, 200, 235, 187,  60, 131,  83, 153,  97,
    23,  43,   4, 126, 186, 119, 214,  38, 225, 105,  20,  99,  85,  33,  12, 125,
};

static const word8 aes_sw_rcon[30] = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
    0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f,
    0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4,
    0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91
};

void tn_aes_128(u8 *key, u8 *plaintext, u8 *result)
{
    word8    aes_sw_k0[4][4] = {{0}};
    word8    aes_sw_k10[4][4] = {{0}};
    _rijndaelSetKey (key, aes_sw_k0, aes_sw_k10);
    
    memcpy (result, plaintext, 16);
    
    _rijndaelEncrypt (result, aes_sw_k0);
}

void _rijndaelSetKey (unsigned char *k, unsigned char aes_sw_k0[4][4], unsigned char aes_sw_k10[4][4])
{
    int i, j, l;
    for(j = 0; j < 4; j++)
        for(i = 0; i < 4; i++) {
            aes_sw_k0[i][j] = k[j*4 + i];
            aes_sw_k10[i][j] = k[j*4 + i];
        }
    
    signed char aes_sw_rconptr = 0;
    for(l = 0; l < 10; l ++)
        aes_sw_nextkey(aes_sw_k10[0], 0, &aes_sw_rconptr);
}

void _rijndaelEncrypt(unsigned char *a, unsigned char aes_sw_k0[4][4])
{
    int i, r;
    word8    aes_sw_ktmp[4][4] = {{0}};
    unsigned char *pt = aes_sw_ktmp[0];
    unsigned char *pk = aes_sw_k0[0];
    
    int aes_sw_mode = 0;
    
    aes_sw_SwapRowCol (a);
    
    
    for (i=0; i<16; i++)
    {
        pt[i] = pk[i];
    }
    
    signed char aes_sw_rconptr = 0;
    
    for(r = 0; r < 11; r++) {
        if(r != 0) {
            aes_sw_Substitution(a, aes_sw_mode);
        }
        
        if(r != 0) {
            aes_sw_ShiftRow(a, aes_sw_mode);
        }
        
        if(r != 0 && r != 10) {
            aes_sw_MixColumn(a, aes_sw_mode);
        }
        
        aes_sw_KeyAddition(a, aes_sw_ktmp[0]);
        
        aes_sw_nextkey(aes_sw_ktmp[0], 0, &aes_sw_rconptr);
    }
    
    aes_sw_SwapRowCol (a);
}

static void aes_sw_nextkey(unsigned char *a, int dir, signed char    *p_aes_sw_rconptr) {
    int i, j;
    
    if(dir == 0) {
        for(i = 0; i < 4; i++)
            a[i * 4 + 0] ^= aes_sw_S[a[((i+1)&3) * 4 + 3]];
        a[0] ^= aes_sw_rcon[*p_aes_sw_rconptr];
        *p_aes_sw_rconptr += 1;
        for(j = 1; j < 4; j++)
            for(i = 0; i < 4; i++) a[i * 4 + j] ^= a[i * 4 + j-1];
    }
    else {
        for(j = 3; j > 0; j--)
            for(i = 0; i < 4; i++) a[i * 4 + j] ^= a[i * 4 + j-1];
        a[0] ^= aes_sw_rcon[*p_aes_sw_rconptr];
        *p_aes_sw_rconptr -= 1;
        for(i = 0; i < 4; i++)
            a[i * 4 + 0] ^= aes_sw_S[a[((i+1)&3) * 4 + 3]];
    }
}

void aes_sw_SwapRowCol (unsigned char * a)
{
    unsigned char t, i, j;
    for (i=0; i<4; i++)
        for (j=i+1; j<4; j++)
        {
            t = a[i * 4 + j];
            a[i * 4 + j] = a[j * 4 + i];
            a[j * 4 + i] = t;
        }
}

static void aes_sw_Substitution(unsigned char * a, int aes_sw_mode) {
    int i;
    for(i = 0; i < 16; i++) a[i] = (aes_sw_mode == 0)?aes_sw_S[a[i]]:aes_sw_Si[a[i]];
}

static void aes_sw_ShiftRow(unsigned char *a, int aes_sw_mode) {
    int i, j;
    
    for(i = 1; i < 4; i++) {
        for(j = 0; j < 4; j++) {
            word8 tmp[2];
            int s = (aes_sw_mode == 0)?i:(4-i);
            int v = (s + j) >> 2;
            int _j = (s + j) & 3;
            
            if(j == 0 || (j == 2 && s == 3))
                tmp[0] = a[i * 4 + j];
            if(j == 1)
                tmp[1] = a[i * 4 + j];
            a[i * 4 + j] = v?tmp[_j & 1]:a[i * 4 + _j];
        }
    }
}

static void aes_sw_MixColumn(unsigned char *a, int aes_sw_mode) {
    unsigned char b[16]={};
    int i, j;
    
    for(j = 0; j < 4; j++)
    {
        for(i = 0; i < 4; i++)
        {
            b[i * 4 + j] = aes_sw_mul((aes_sw_mode == 0)?2:0xe, a[i * 4 + j])
            ^ aes_sw_mul((aes_sw_mode == 0)?3:0xb, a[((i + 1) & 3) * 4 + j])
            ^ aes_sw_mul((aes_sw_mode == 0)?1:0xd, a[((i + 2) & 3) * 4 + j])
            ^ aes_sw_mul((aes_sw_mode == 0)?1:0x9, a[((i + 3) & 3) * 4 + j]);
        }
    }
    for(i = 0; i < 16; i++)
        a[i] = b[i];
}

static void aes_sw_KeyAddition(unsigned char * a, unsigned char *k) {
    int i;
    for(i = 0; i < 16; i++)
        a[i] ^= k[i];
}

static word8 aes_sw_mul(word8 a, word8 b) {
    word8 x0, x1, x2, x3, x4, x5, x6, x7;
    
    x0 = a;
    x1 = (x0 & 0x80)?((x0 << 1) ^ 0x1b) : (x0 << 1);
    x2 = (x1 & 0x80)?((x1 << 1) ^ 0x1b) : (x1 << 1);
    x3 = (x2 & 0x80)?((x2 << 1) ^ 0x1b) : (x2 << 1);
    x4 = (x3 & 0x80)?((x3 << 1) ^ 0x1b) : (x3 << 1);
    x5 = (x4 & 0x80)?((x4 << 1) ^ 0x1b) : (x4 << 1);
    x6 = (x5 & 0x80)?((x5 << 1) ^ 0x1b) : (x5 << 1);
    x7 = (x6 & 0x80)?((x6 << 1) ^ 0x1b) : (x6 << 1);
    
    return ((b & 0x80)?x7:0) ^ ((b & 0x40)?x6:0) ^
    ((b & 0x20)?x5:0) ^ ((b & 0x10)?x4:0) ^
    ((b & 0x08)?x3:0) ^ ((b & 0x04)?x2:0) ^
    ((b & 0x02)?x1:0) ^ ((b & 0x01)?x0:0);
}


@end
