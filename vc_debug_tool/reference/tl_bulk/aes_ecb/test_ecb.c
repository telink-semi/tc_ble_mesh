/********************************************************************************************************
 * @file	test_ecb.c
 *
 * @brief	This is the source file for BLE SDK
 *
 * @author	Mesh Group
 * @date	2017
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

#include <stdio.h>
//#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "aes_ecb.h"
 #include "../../../ble_lt_mesh/vendor/common/app_provison.h"
//#define PaddingMode_None
#define PaddingMode_Zeros
//#define PaddingMode_PKCS5or7
 
#define MAXLEN 1024
void aes_win32(char *p, int plen, char *key);
void printASCCI(char *str, int len) {
    int c,i;
    for(i = 0; i < len; i++) {
        c = (int)*str++;
        c = c & 0x000000ff;
        printf("0x%x ", c);
    }
    printf("\n");
}

void vc_test_ecb()
{
	unsigned char key[16]={1,2,3,4,5,6,7,8,1,2,3,4,5,6,7,8};
	unsigned char random[16]={1,2,3,4,5,6,7,8,1,2,3,4,5,6,7,9};
	aes_win32((char *)random, sizeof(random), (char*)key);//AES¼ÓÃÜ
}
#if 0
int vc_cal_node_identity(u8 random[8],u8 node_key[16],u16 ele_adr,u8 hash[8])
{
	u8 adv_hash[16];
	u8 adv_para[16];
	memset(adv_hash,0,sizeof(adv_hash));
	memset(adv_para,0,sizeof(adv_para));
	endianness_swap_u16((u8 *)&ele_adr);
	memset(adv_para,0,6);
	memcpy(adv_para+6,random,8);
	memcpy(adv_para+14,(u8 *)&ele_adr,2);
	aes_win32((char *)adv_para,sizeof(adv_para),(char*)node_key);
	memcpy(hash,adv_para+8,8);
	return 1;
}
#endif



#if 0
int test_ecb() 
{ 
	char key[17] = "abcdefghigklmnop";
	int klen;
	char p[MAXLEN]="ABCDEFGHIGKLMN0";
	int plen = strlen(p);
	printf("AES...ing..................\n");
	if(plen == 0) 
	{
		printf("plen erro \n");
		return 0;
	}
 
#ifdef PaddingMode_Zeros
	if(plen % 16 != 0)
		plen=(16 - plen%16) + plen;
	else
		plen+=16;
#endif
 
#ifdef PaddingMode_PKCS5or7
	if(plen % 16 != 0)
	{
		int len_num = 16 - plen%16;
		int i = 0;
		for(i;i<len_num;i++)
		p[plen+i] = len_num;
		plen+=len_num;
	}
	else
	{
		int i = 0;
		for(i;i<16;i++)
		p[plen+i] = 16;
		plen+=16;
	}
#endif
 
	aes_win32(p, plen, key);//AES¼ÓÃÜ
	printf("After Aes ASCCI is:\n");
	printASCCI(p, plen);
	
	deAes(p, plen, key);//AES½âÃÜ
	printf("After DAes ASCCI is:\n");
	printASCCI(p, plen);
	return 0;
}
#endif