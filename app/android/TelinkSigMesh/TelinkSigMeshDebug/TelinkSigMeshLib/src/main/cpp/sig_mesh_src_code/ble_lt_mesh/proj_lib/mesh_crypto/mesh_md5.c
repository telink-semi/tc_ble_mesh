/********************************************************************************************************
 * @file     mesh_md5.c 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/
#include "mesh_md5.h"

/* Constants for MD5Transform routine.
 */

	
extern void MD5Init(struct MD5Context *ctx);
extern void  MD5Update(struct MD5Context *ctx, unsigned char *buf, unsigned len);
extern void MD5Final(unsigned char digest[16],struct MD5Context *ctx);
extern void MD5Transform(uint32 buf[4],uint32 in[16]) ;
typedef struct MD5Context MD5_CTX;
	
	
#define DO1(buf) crc = crc_table(((int)crc ^ (*buf++)) & 0xff) ^ (crc >> 8);
#define DO2(buf)  DO1(buf); DO1(buf);
#define DO4(buf)  DO2(buf); DO2(buf);
#define DO8(buf)  DO4(buf); DO4(buf);


/////////////////// CRC 32 ////////////////////////////////
#if WIN32
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
#endif

static uint32_t crc_table(uint32_t index)
{
	uint32_t c = index;
	uint32_t poly = 0xedb88320L;
	int k;

	for (k = 0; k < 8; k++)
		c = c & 1 ? poly ^ (c >> 1) : c >> 1;

	return c;
}

uint32_t weixin_crc32(uint32_t crc, const uint8_t *buf, int len)
{
	if (buf == 0) return 0L;

	crc = crc ^ 0xffffffffL;
	while (len >= 8)
	{
	  DO8(buf);
	  len -= 8;
	}
	if (len) do {
	  DO1(buf);
	} while (--len);
	return crc ^ 0xffffffffL;
}

#define CHECK_HARDWARE_PROPERTIES
	
	
#ifndef HIGHFIRST
#define byteReverse(buf, len)	/* Nothing */
#else

/*
 * Note: this code is harmless on little-endian machines.
 */
void byteReverse(unsigned char *buf,unsigned longs)
{
	uint32 t;
	do {
	t = (uint32) ((unsigned) buf[3] << 8 | buf[2]) << 16 |
		((unsigned) buf[1] << 8 | buf[0]);
	*(uint32 *) buf = t;
	buf += 4;
	} while (--longs);
}
#endif

/*
 * Start MD5 accumulation.	Set bit count to 0 and buffer to mysterious
 * initialization constants.
 */
void MD5Init(struct MD5Context *ctx)
{
	ctx->buf[0] = 0x67452301;
	ctx->buf[1] = 0xefcdab89;
	ctx->buf[2] = 0x98badcfe;
	ctx->buf[3] = 0x10325476;

	ctx->bits[0] = 0;
	ctx->bits[1] = 0;
}

/*
 * Update context to reflect the concatenation of another buffer full
 * of bytes.
 */
void MD5Update(struct MD5Context *ctx, unsigned char *buf, unsigned len)
{
	uint32 t;

	/* Update bitcount */

	t = ctx->bits[0];
	if ((ctx->bits[0] = t + ((uint32) len << 3)) < t)
	ctx->bits[1]++; 	/* Carry from low to high */
	ctx->bits[1] += len >> 29;

	t = (t >> 3) & 0x3f;	/* Bytes already in shsInfo->data */

	/* Handle any leading odd-sized chunks */

	if (t) {
	unsigned char *p = (unsigned char *) ctx->in + t;

	t = 64 - t;
	if (len < t) {
		memcpy(p, buf, len);
		return;
	}
	memcpy(p, buf, t);
	byteReverse(ctx->in, 16);
	MD5Transform(ctx->buf, (uint32 *) ctx->in);
	buf += t;
	len -= t;
	}
	/* Process data in 64-byte chunks */

	while (len >= 64) {
	memcpy(ctx->in, buf, 64);
	byteReverse(ctx->in, 16);
	MD5Transform(ctx->buf, (uint32 *) ctx->in);
	buf += 64;
	len -= 64;
	}

	/* Handle any remaining bytes of data. */

	memcpy(ctx->in, buf, len);
}

/*
 * Final wrapup - pad to 64-byte boundary with the bit pattern 
 * 1 0* (64-bit count of bits processed, MSB-first)
 */
void MD5Final(unsigned char digest[16],struct MD5Context *ctx)
{
	unsigned count;
	unsigned char *p;

	/* Compute number of bytes mod 64 */
	count = (ctx->bits[0] >> 3) & 0x3F;

	/* Set the first char of padding to 0x80.  This is safe since there is
	   always at least one byte free */
	p = ctx->in + count;
	*p++ = 0x80;

	/* Bytes of padding needed to make 64 bytes */
	count = 64 - 1 - count;

	/* Pad out to 56 mod 64 */
	if (count < 8) {
	/* Two lots of padding:  Pad the first block to 64 bytes */
	memset(p, 0, count);
	byteReverse(ctx->in, 16);
	MD5Transform(ctx->buf, (uint32 *) ctx->in);

	/* Now fill the next block with 56 bytes */
	memset(ctx->in, 0, 56);
	} else {
	/* Pad block to 56 bytes */
	memset(p, 0, count - 8);
	}
	byteReverse(ctx->in, 14);

	/* Append length in bits and transform */
	((uint32 *) ctx->in)[14] = ctx->bits[0];
	((uint32 *) ctx->in)[15] = ctx->bits[1];

	MD5Transform(ctx->buf, (uint32 *) ctx->in);
	byteReverse((unsigned char *) ctx->buf, 4);
	memcpy(digest, ctx->buf, 16);
	memset(ctx, 0, sizeof(ctx));		/* In case it's sensitive */
}


/* The four core functions - F1 is optimized somewhat */

/* #define F1(x, y, z) (x & y | ~x & z) */
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))
	
#if 1
	/* This is the central step in the MD5 algorithm. */
#define MD5STEP(f, w, x, y, z, data, s) \
		( w += f(x, y, z) + data,  t = w<<s, w = w>>(32-s),  w += t + x )
#else
#define MD5STEP(f, w, x, y, z, data, s) \
		( w += f(x, y, z) + data,  w = ((w<<s)&(0xffffffff<<s)) + ((w>>(32-s))),  w += x )
#endif
/*
 * The core of the MD5 algorithm, this alters an existing MD5 hash to
 * reflect the addition of 16 longwords of new data.  MD5Update blocks
 * the data and converts bytes into longwords for this routine.
 */
void MD5Transform(uint32 buf[4],uint32 in[16])	
{
	unsigned int t;
	register uint32 a, b, c, d;

	a = buf[0];
	b = buf[1];
	c = buf[2];
	d = buf[3];

	MD5STEP(F1, a, b, c, d, in[0] + 0xd76aa478, 7);
	MD5STEP(F1, d, a, b, c, in[1] + 0xe8c7b756, 12);
	MD5STEP(F1, c, d, a, b, in[2] + 0x242070db, 17);
	MD5STEP(F1, b, c, d, a, in[3] + 0xc1bdceee, 22);
	MD5STEP(F1, a, b, c, d, in[4] + 0xf57c0faf, 7);
	MD5STEP(F1, d, a, b, c, in[5] + 0x4787c62a, 12);
	MD5STEP(F1, c, d, a, b, in[6] + 0xa8304613, 17);
	MD5STEP(F1, b, c, d, a, in[7] + 0xfd469501, 22);
	MD5STEP(F1, a, b, c, d, in[8] + 0x698098d8, 7);
	MD5STEP(F1, d, a, b, c, in[9] + 0x8b44f7af, 12);
	MD5STEP(F1, c, d, a, b, in[10] + 0xffff5bb1, 17);
	MD5STEP(F1, b, c, d, a, in[11] + 0x895cd7be, 22);
	MD5STEP(F1, a, b, c, d, in[12] + 0x6b901122, 7);
	MD5STEP(F1, d, a, b, c, in[13] + 0xfd987193, 12);
	MD5STEP(F1, c, d, a, b, in[14] + 0xa679438e, 17);
	MD5STEP(F1, b, c, d, a, in[15] + 0x49b40821, 22);

	MD5STEP(F2, a, b, c, d, in[1] + 0xf61e2562, 5);
	MD5STEP(F2, d, a, b, c, in[6] + 0xc040b340, 9);
	MD5STEP(F2, c, d, a, b, in[11] + 0x265e5a51, 14);
	MD5STEP(F2, b, c, d, a, in[0] + 0xe9b6c7aa, 20);
	MD5STEP(F2, a, b, c, d, in[5] + 0xd62f105d, 5);
	MD5STEP(F2, d, a, b, c, in[10] + 0x02441453, 9);
	MD5STEP(F2, c, d, a, b, in[15] + 0xd8a1e681, 14);
	MD5STEP(F2, b, c, d, a, in[4] + 0xe7d3fbc8, 20);
	MD5STEP(F2, a, b, c, d, in[9] + 0x21e1cde6, 5);
	MD5STEP(F2, d, a, b, c, in[14] + 0xc33707d6, 9);
	MD5STEP(F2, c, d, a, b, in[3] + 0xf4d50d87, 14);
	MD5STEP(F2, b, c, d, a, in[8] + 0x455a14ed, 20);
	MD5STEP(F2, a, b, c, d, in[13] + 0xa9e3e905, 5);
	MD5STEP(F2, d, a, b, c, in[2] + 0xfcefa3f8, 9);
	MD5STEP(F2, c, d, a, b, in[7] + 0x676f02d9, 14);
	MD5STEP(F2, b, c, d, a, in[12] + 0x8d2a4c8a, 20);

	MD5STEP(F3, a, b, c, d, in[5] + 0xfffa3942, 4);
	MD5STEP(F3, d, a, b, c, in[8] + 0x8771f681, 11);
	MD5STEP(F3, c, d, a, b, in[11] + 0x6d9d6122, 16);
	MD5STEP(F3, b, c, d, a, in[14] + 0xfde5380c, 23);
	MD5STEP(F3, a, b, c, d, in[1] + 0xa4beea44, 4);
	MD5STEP(F3, d, a, b, c, in[4] + 0x4bdecfa9, 11);
	MD5STEP(F3, c, d, a, b, in[7] + 0xf6bb4b60, 16);
	MD5STEP(F3, b, c, d, a, in[10] + 0xbebfbc70, 23);
	MD5STEP(F3, a, b, c, d, in[13] + 0x289b7ec6, 4);
	MD5STEP(F3, d, a, b, c, in[0] + 0xeaa127fa, 11);
	MD5STEP(F3, c, d, a, b, in[3] + 0xd4ef3085, 16);
	MD5STEP(F3, b, c, d, a, in[6] + 0x04881d05, 23);
	MD5STEP(F3, a, b, c, d, in[9] + 0xd9d4d039, 4);
	MD5STEP(F3, d, a, b, c, in[12] + 0xe6db99e5, 11);
	MD5STEP(F3, c, d, a, b, in[15] + 0x1fa27cf8, 16);
	MD5STEP(F3, b, c, d, a, in[2] + 0xc4ac5665, 23);

	MD5STEP(F4, a, b, c, d, in[0] + 0xf4292244, 6);
	MD5STEP(F4, d, a, b, c, in[7] + 0x432aff97, 10);
	MD5STEP(F4, c, d, a, b, in[14] + 0xab9423a7, 15);
	MD5STEP(F4, b, c, d, a, in[5] + 0xfc93a039, 21);
	MD5STEP(F4, a, b, c, d, in[12] + 0x655b59c3, 6);
	MD5STEP(F4, d, a, b, c, in[3] + 0x8f0ccc92, 10);
	MD5STEP(F4, c, d, a, b, in[10] + 0xffeff47d, 15);
	MD5STEP(F4, b, c, d, a, in[1] + 0x85845dd1, 21);
	MD5STEP(F4, a, b, c, d, in[8] + 0x6fa87e4f, 6);
	MD5STEP(F4, d, a, b, c, in[15] + 0xfe2ce6e0, 10);
	MD5STEP(F4, c, d, a, b, in[6] + 0xa3014314, 15);
	MD5STEP(F4, b, c, d, a, in[13] + 0x4e0811a1, 21);
	MD5STEP(F4, a, b, c, d, in[4] + 0xf7537e82, 6);
	MD5STEP(F4, d, a, b, c, in[11] + 0xbd3af235, 10);
	MD5STEP(F4, c, d, a, b, in[2] + 0x2ad7d2bb, 15);
	MD5STEP(F4, b, c, d, a, in[9] + 0xeb86d391, 21);

	buf[0] += a;
	buf[1] += b;
	buf[2] += c;
	buf[3] += d;
}

void	tl_md5 (unsigned char *in, int len, unsigned char * out)
{
	struct MD5Context ctx;
	MD5Init (&ctx);
	MD5Update (&ctx, in, len);
	MD5Final (out, &ctx);
		
}

/* format_uuid_v3or5 -- make a UUID from a (pseudo)random 128-bit
   number */
void swapX_mesh(const u8 *src, u8 *dst, int len)
{
    int i;
    for (i = 0; i < len; i++)
        dst[len - 1 - i] = src[i];
}
u32 ntohl_mesh(u32 dat_32)
{
	u32 swap_dat;
	swapX_mesh((u8 *)&dat_32,(u8 *)&swap_dat,4);
	return swap_dat;
}
u16 ntohs_mesh(u16 dat_16)
{
	u16 swap_dat;
	swapX_mesh((u8 *)&dat_16,(u8 *)&swap_dat,2);
	return swap_dat;
}
u32 htonl_mesh(u32 dat_32)
{
	return ntohl_mesh(dat_32);
}

u16 htons_mesh(u16 dat_16)
{
	return ntohs_mesh(dat_16);
}
void format_uuid_v3or5(uuid_mesh_t *uuid, unsigned char hash[16], int v)
{
    /* convert UUID to local byte order */
	memset(uuid->node,0,6);
    memcpy(uuid, hash, sizeof *uuid);
  //  uuid->time_low = ntohl_mesh(uuid->time_low);
  //  uuid->time_mid = ntohs_mesh(uuid->time_mid);
  //  uuid->time_hi_and_version = ntohs_mesh(uuid->time_hi_and_version);

    /* put in the variant and version bits */
    uuid->time_hi_and_version &= 0x0FFF;
    uuid->time_hi_and_version |= (v << 12);
    uuid->clock_seq_hi_and_reserved &= 0x3F;
    uuid->clock_seq_hi_and_reserved |= 0x80;
}

/* uuid_create_md5_from_name -- create a version 3 (MD5) UUID using a
   "name" from a "name space" */
void uuid_create_md5_from_name(uuid_mesh_t *uuid, uuid_mesh_t nsid, void *name,
                               int namelen)
{
    MD5_CTX c;
    unsigned char hash[16];
    uuid_mesh_t net_nsid;
	//static uuid_mesh_t net_nsid_tmp;		// save RAM
	//static unsigned char hash_tmp[16];	// save RAM
    /* put name space ID in network byte order so it hashes the same
       no matter what endian machine we're on */
   // memcpy(&net_nsid,&nsid,sizeof(uuid_mesh_t));
    net_nsid = nsid;
  //  net_nsid.time_low = htonl_mesh(net_nsid.time_low);
   // net_nsid.time_mid = htons_mesh(net_nsid.time_mid);
  //  net_nsid.time_hi_and_version = htons_mesh(net_nsid.time_hi_and_version);
	//memcpy(&net_nsid_tmp,&net_nsid,sizeof(uuid_mesh_t));
    MD5Init(&c);
    MD5Update(&c, (u8 *)&net_nsid, sizeof  net_nsid);
    MD5Update(&c, (u8 *)name, namelen);
    MD5Final(hash, &c);
	//memcpy(hash_tmp,hash,16);
    /* the hash is in network byte order at this point */
    format_uuid_v3or5(uuid, hash, 3);
}


