/********************************************************************************************************
 * @file     aes_att.c 
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
#include "../../proj/tl_common.h"
#include "aes_att.h"
//#include <memory.h>

void swapX(const u8 *src, u8 *dst, u8 len);
void aes_ll_encryption(u8 *key, u8 *plaintext, u8 *result);


static void aes_sw_nextkey(unsigned char *a, int dir, signed char	*p_aes_sw_rconptr) {
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

static void aes_sw_KeyAddition(unsigned char * a, unsigned char *k) {
	int i;
	for(i = 0; i < 16; i++)
			a[i] ^= k[i];
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

static void aes_sw_MixColumn(unsigned char *a, int aes_sw_mode) {
	unsigned char b[16];
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

#if 1
void _rijndaelDecrypt (unsigned char *a, unsigned char aes_sw_k10[4][4])
{
	int i, r;
    word8    aes_sw_ktmp[4][4] = {{0}};
	unsigned char *pt = aes_sw_ktmp[0];
	unsigned char *pk = aes_sw_k10[0];

	int aes_sw_mode = 1;

	aes_sw_SwapRowCol (a);

	for (i=0; i<16; i++)
	{
		pt[i] = pk[i];
	}

	signed char aes_sw_rconptr = 9;
	for(r = 0; r < 11; r++) {
		aes_sw_KeyAddition(a,aes_sw_ktmp[0]);
		if(r != 0 && r != 10) {
			aes_sw_MixColumn(a, aes_sw_mode);
		}
		if(r != 10) {
			aes_sw_Substitution(a, aes_sw_mode);
		}
		if(r != 10) {
			aes_sw_ShiftRow(a, aes_sw_mode);
		}

		aes_sw_nextkey(aes_sw_ktmp[0], 1, &aes_sw_rconptr);
	}

	aes_sw_SwapRowCol (a);

}
#endif

//typedef unsigned char u8;

void aes_att_swap (u8 *k)
{
	int i;
	for (i=0; i<8; i++)
	{
		u8 t = k[i];
		k[i] = k[15 - i];
		k[15 - i] = t;
	}
}

void tn_aes_128(u8 *key, u8 *plaintext, u8 *result)
{
#if WIN32 
    word8    aes_sw_k0[4][4] = {{0}};
    word8    aes_sw_k10[4][4] = {{0}};
	_rijndaelSetKey (key, aes_sw_k0, aes_sw_k10);

	 memcpy (result, plaintext, 16);

	 _rijndaelEncrypt (result, aes_sw_k0);
#else
	u8 aes_key[16];
 	u8 aes_plaintest[16];
	swapX (key, aes_key, 16);
	swapX (plaintext, aes_plaintest, 16);
	aes_ll_encryption (aes_key, aes_plaintest, result);
#endif 
}

#if 0
int		att_crypto_poly = 0;

void aes_att_encryption_poly (u8 *pk, u8 *pd, u8 *pr)
{
	unsigned char r[16];
    static unsigned short poly[2]={0, 0xa001};              //0x8005 <==> 0xa001
    unsigned short crc = 0xffff;
    unsigned char t = 0;
    int i,j;

    for(j=0; j<16; j++)
    {
        unsigned char ds = pk[j];
        for(i=0; i<8; i++)
        {
            crc = (crc >> 1) ^ poly[(crc ^ ds ) & 1];
            ds = ds >> 1;
        }
		t ^= crc ^ pd[j];
		r[15-j] = t;
    }
	memcpy (pr, r, 16);
}

void aes_att_decryption_poly (u8 *pk, u8 *pd, u8 *pr)
{

	unsigned char r[16];
    static unsigned short poly[2]={0, 0xa001};              //0x8005 <==> 0xa001
    unsigned short crc = 0xffff;
    unsigned char t = 0;
    int i,j;

    for(j=0; j<16; j++)
    {
        unsigned char ds = pk[j];
        for(i=0; i<8; i++)
        {
            crc = (crc >> 1) ^ poly[(crc ^ ds ) & 1];
            ds = ds >> 1;
        }
        //t ^= crc ^ pd[15 - j];
		r[j] = t ^ crc ^ pd[15 - j];
		t ^= crc ^ r[j]; 
    }
	memcpy (pr, r, 16);
}

void aes_att_encryption(u8 *key, u8 *plaintext, u8 *result)
{
	u8 sk[16];
	int i;

	if (att_crypto_poly)
	{
		aes_att_encryption_poly (key, plaintext, result);
		return;
	}
	

	aes_sw_mode = 0;
	for (i=0; i<16; i++)
	{
		sk[i] = key[i]; //key[15 - i];
	}
	_rijndaelSetKey (sk);

	for (i=0; i<16; i++)
	{
		sk[i] = plaintext[i]; //plaintext[15 - i];
	}
	_rijndaelEncrypt (sk);

	 memcpy (result, sk, 16);

	 aes_att_swap (result);
}

void aes_att_decryption(u8 *key, u8 *plaintext, u8 *result)
{
	u8 sk[16];
	int i;
	if (att_crypto_poly)
	{
		aes_att_decryption_poly (key, plaintext, result);
		return;
	}


	aes_sw_mode = 1;
	for (i=0; i<16; i++)
	{
		sk[i] = key[15 - i];
	}
	_rijndaelSetKey (sk);

	for (i=0; i<16; i++)
	{
		sk[i] = plaintext[15 - i];
	}
	_rijndaelDecrypt (sk);

	 memcpy (result, sk, 16);

	 aes_att_swap (result);
}

int	aes_att_er (unsigned char *pNetworkName, unsigned char *pPassword, unsigned char *prand, unsigned char *presult)
{
	unsigned char sk[16], d[16], r[16];
	int i;
	for (i=0; i<16; i++)
	{
		d[i] = pNetworkName[i] ^ pPassword[i];
	}
	memcpy (sk, prand, 8);
	memset (sk + 8, 0, 8);
	aes_att_encryption (sk, d, r);
	memcpy (presult, prand, 8);
	memcpy (presult+8, r, 8);
	
	return !(memcmp (prand, presult, 16));
}

void	aes_att_get_sk (unsigned char *pNetworkName, unsigned char *pPassword, unsigned char *prandm, unsigned char *prands, unsigned char *presult)
{
	unsigned char sk[16], d[16], r[16];
	int i;
	for (i=0; i<16; i++)
	{
		d[i] = pNetworkName[i] ^ pPassword[i];
	}
	memcpy (sk, prandm, 8);
	memcpy (sk + 8, prands, 8);
	aes_att_encryption (d, sk, r);
	memcpy (presult, r, 16);
}

int	aes_att_get_ltk (unsigned char *pNetworkName, unsigned char *pPassword, unsigned char *prand, unsigned char *ptext, unsigned char *pltk)
{
	unsigned char sk[16], r[16];
	int i;
	memcpy (sk, prand, 8);
	memset (sk + 8, 0, 8);
	for (i=0; i<16; i++)
	{
		sk[i] ^= pNetworkName[i] ^ pPassword[i];
	}
	
	aes_att_decryption (sk, ptext, r);

	memcpy (pltk, r, 16);
	return 1;
}

int	aes_att_enc_ltk (unsigned char *pNetworkName, unsigned char *pPassword, unsigned char *prand, unsigned char *ptext, unsigned char *pltk)
{
	unsigned char sk[16], r[16];
	int i;
	memcpy (sk, prand, 8);
	memset (sk + 8, 0, 8);
	for (i=0; i<16; i++)
	{
		sk[i] ^= pNetworkName[i] ^ pPassword[i];
	}
	
	aes_att_encryption (sk, ptext, r);

	memcpy (pltk, r, 16);
	return 1;
}
#endif
/////////////////////////////////////////////////////////////////////////////////

