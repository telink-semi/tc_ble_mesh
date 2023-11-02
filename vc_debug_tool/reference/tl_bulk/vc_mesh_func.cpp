/********************************************************************************************************
 * @file	vc_mesh_func.cpp
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
#include "stdafx.h"
#include "vc_mesh_func.h"
#include "tl_ble_module.h"
#include "ScanDlg.h"
#include "tl_ble_moduleDlg.h"
#include "TLMeshDlg.h"
#include "usbprt.h"
#include "./lib_file/gatt_provision.h"
#include "./lib_file/hw_fun.h"
#include "./lib_file/host_fifo.h"
#include "CTL_privision.h"
#include "TL_RxTest.h"
#include <stdio.h>
#include "../../ble_lt_mesh/vendor/common/app_provison.h"
#include "../../ble_lt_mesh/vendor/common/app_heartbeat.h"
#include "../../ble_lt_mesh/proj_lib/ble/service/ble_ll_ota.h"
#include "../../ble_lt_mesh/proj_lib/mesh_crypto/sha256_telink.h"
#include "rapid_json_interface.h"
#include "sig_mesh_json_info.h"

vc_mesh_func::vc_mesh_func(void)
{
}


vc_mesh_func::~vc_mesh_func(void)
{
}

#if MD_BIND_WHITE_LIST_EN
const u16 master_filter_list[]={
	SIG_MD_G_ONOFF_S,SIG_MD_G_LEVEL_S,SIG_MD_G_DEF_TRANSIT_TIME_S,SIG_MD_LIGHTNESS_S,
	SIG_MD_LIGHTNESS_SETUP_S,SIG_MD_LIGHT_CTL_S,SIG_MD_LIGHT_CTL_SETUP_S,SIG_MD_LIGHT_CTL_TEMP_S,
	SIG_MD_LIGHT_LC_S,SIG_MD_LIGHT_LC_SETUP_S,
    SIG_MD_LIGHT_HSL_S,SIG_MD_LIGHT_HSL_SETUP_S,SIG_MD_LIGHT_HSL_HUE_S,SIG_MD_LIGHT_HSL_SAT_S,
	SIG_MD_FW_UPDATE_S,SIG_MD_FW_UPDATE_C,SIG_MD_FW_DISTRIBUT_S,SIG_MD_FW_DISTRIBUT_C,SIG_MD_BLOB_TRANSFER_S,SIG_MD_BLOB_TRANSFER_C,
};
u8 model_need_key_bind_whitelist(u16 *key_bind_list_buf,u8 *p_list_cnt,u8 max_cnt)
{
	//*p_list_cnt =0;
	//return 1;
	if(ARRAY_SIZE(master_filter_list) >= max_cnt){
		return 0;
	}
	memcpy((u8 *)key_bind_list_buf,(u8 *)master_filter_list,sizeof(master_filter_list));
	*p_list_cnt= ARRAY_SIZE(master_filter_list);
	return 1;
}
#else
u8 model_need_key_bind_whitelist(u16 *key_bind_list_buf,u8 *p_list_cnt,u8 max_cnt)
{
    *p_list_cnt = 0;
	return 1;
}
#endif


char arrb2t[] = "0123456789abcdef";
char arrh2i[256] = {
		16, 16, 16, 16, 16, 16, 16, 16,  16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16,  16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16,  16, 16, 16, 16, 16, 16, 16, 16,
		 0,  1,  2,  3,  4,  5,  6,  7,   8,  9, 16, 16, 16, 16, 16, 16,
		
		16, 10, 11, 12, 13, 14, 15, 16,  16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16,  16, 16, 16, 16, 16, 16, 16, 16, 
		16, 10, 11, 12, 13, 14, 15, 16,  16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16,  16, 16, 16, 16, 16, 16, 16, 16,

		16, 16, 16, 16, 16, 16, 16, 16,  16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16,  16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16,  16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16,  16, 16, 16, 16, 16, 16, 16, 16,

		16, 16, 16, 16, 16, 16, 16, 16,  16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16,  16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16,  16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16,  16, 16, 16, 16, 16, 16, 16, 16
};


u8 win32_create_rand()
{
    srand(clock_time()); 
	return (u8)(rand()&0xff);
}

u8 win32_create_rand_buf(u8 *p_buf,u8 len )
{
	for(int i=0;i<len;i++){
		p_buf[i]= win32_create_rand();
	}
	return 0;
}
int Hex2Text (LPBYTE lpD, LPBYTE lpS, int n)
{
    int i = 0;
	int d = 0;
	
	if (n == 2) {
		lpD[d++] = arrb2t [(lpS[1]>>4) & 15];
		lpD[d++] = arrb2t [lpS[1] & 15];
		lpD[d++] = arrb2t [(lpS[0]>>4) & 15];
		lpD[d++] = arrb2t [lpS[0] & 15];
		lpD[d++] = ' ';
		return 5;

	}
	else if (n == 3) {
		lpD[d++] = arrb2t [(lpS[2]>>4) & 15];
		lpD[d++] = arrb2t [lpS[2] & 15];
		lpD[d++] = arrb2t [(lpS[1]>>4) & 15];
		lpD[d++] = arrb2t [lpS[1] & 15];
		lpD[d++] = arrb2t [(lpS[0]>>4) & 15];
		lpD[d++] = arrb2t [lpS[0] & 15];
		lpD[d++] = ' ';
		return 7;

	}
	else if (n == 4) {
		lpD[d++] = arrb2t [(lpS[3]>>4) & 15];
		lpD[d++] = arrb2t [lpS[3] & 15];
		lpD[d++] = arrb2t [(lpS[2]>>4) & 15];
		lpD[d++] = arrb2t [lpS[2] & 15];
		lpD[d++] = arrb2t [(lpS[1]>>4) & 15];
		lpD[d++] = arrb2t [lpS[1] & 15];
		lpD[d++] = arrb2t [(lpS[0]>>4) & 15];
		lpD[d++] = arrb2t [lpS[0] & 15];
		lpD[d++] = ' ';
		return 9;

	}
	else
	{
		lpD[d++] = arrb2t [(lpS[0]>>4) & 15];
		lpD[d++] = arrb2t [lpS[0] & 15];
		lpD[d++] = ' ';
		return 3;
	}
}

int Bin2Text (LPBYTE lpD, LPBYTE lpS, int n)
{
    int i = 0;
	int m = n;
	int d = 0;
	if (m>MAXB2TSIZE) m = MAXB2TSIZE;

	#define LINE_MAX_LOG        (5)
	if(m > BIT(LINE_MAX_LOG)){
		lpD[d++] = '\r';
		lpD[d++] = '\n';
		lpD[d++] = ' ';
		lpD[d++] = ' ';
	}
	for (i=0; i<m; i++) {
        if((0 == (i & BIT_MASK_LEN(LINE_MAX_LOG))) && i){
            lpD[d++] = '\r';
            lpD[d++] = '\n';
            lpD[d++] = ' ';
            lpD[d++] = ' ';
        }

		lpD[d++] = arrb2t [(lpS[i]>>4) & 15];
		lpD[d++] = arrb2t [lpS[i] & 15];
		if((i & BIT_MASK_LEN(LINE_MAX_LOG)) != BIT_MASK_LEN(LINE_MAX_LOG)){	// end of line
    		lpD[d++] = ' ';

            if(m > BIT(LINE_MAX_LOG)){
    		    if ((i&7)==7){
    		        lpD[d++] = ' ';
    		    }
    		}
		}
	}
	//lpD[d++] = '\r';
	//lpD[d++] = '\n';
	lpD[d++] = '\0';
	return d;
}
int bin2text_normal(LPBYTE lpD, LPBYTE lpS, int n)
{
	int d = 0;
	for(int i=0;i<n;i++){
	    if(i){
		    lpD[d++] = ' ';
		}
		lpD[d++] = arrb2t [(lpS[i]>>4) & 15];
		lpD[d++] = arrb2t [lpS[i] & 15];
	}
	return 1;
}

int bin2text_clean(LPBYTE lpD, LPBYTE lpS, int n)
{
	int d = 0;
	for(int i=0;i<n;i++){
		lpD[d++] = arrb2t [(lpS[i]>>4) & 15];
		lpD[d++] = arrb2t [lpS[i] & 15];
	}
	lpD[d++]='\0';
	return 1;
}

int bin2text_clean_uuid(LPBYTE lpD, LPBYTE lpS, int n)
{
	int d = 0;
	for(int i=0;i<n;i++){
		lpD[d++] = arrb2t [(lpS[i]>>4) & 15];
		lpD[d++] = arrb2t [lpS[i] & 15];
		if(i==3||i==5||i==7||i==9){
			lpD[d++]='-';
		}
	}
	lpD[d++]='\0';
	return 1;
}


#define TEXT_BIN_MAX_STR	48
int Text2Bin(LPBYTE lpD, LPBYTE lpS)
{
	// TODO: Add your control notification handler code here
		
	int n = 0;
	
	while (*lpS) {
		
		if (*lpS < TEXT_BIN_MAX_STR) {
			lpS++;
		}
		else {
			
			int dh = *lpS++;
			int dl = *lpS++;	
			*lpD++ = ( arrh2i[dh] << 4) + arrh2i[dl];
			n++;
		}		
	}

	return n;
}

int Time2Text (LPBYTE lpD, unsigned int d)
{
   d = d / 32;
   int us = d % 1000;
   d = (d - us) / 1000;
   int ms = d % 1000;
   d = (d - ms) / 1000;
   
   sprintf_s ((char *)lpD, 12, "%03d:%03d:%03d ", d, ms, us);
   return 12;
}

// just for callback test 
void gatt_provision_net_info_callback()
{
	return;
	u8 netkey[16]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
	u16 key_idx = 0x0001;
	u8 flag =0;
	u8 ivi[4]={0x22,0x33,0x44,0x55};
	u16 unicast_adr = 0x6677;
	set_gatt_provision_net_info(netkey,&key_idx,&flag,ivi,&unicast_adr);
}


u32 new_fw_read(u8 *data_out, u32 max_len)
{    
    errno_t err_fp;
    FILE *fp;
    
#if DEBUG_SHOW_VC_SELF_EN
    if(ota_filename.GetLength() == 0){
        err_fp=fopen_s(&fp,"8258_mesh_test_OTA.bin","rb");  // default bin by qifa // "D:/8258_mesh_PTS_test_new_fw.bin"
    }else
#endif
    {
        err_fp=fopen_s(&fp,ota_filename.GetBuffer(),"rb");
    	ota_filename.ReleaseBuffer();
	}
	
    if(err_fp != 0){
        AfxMessageBox("can't open ota firmware file");
        return 0;
    }
    
    u32 file_size = 0;

    fseek( fp, 0, SEEK_END );   //定位文件指针到文件末尾//成功返回0，不成功返回-1；
    file_size = ftell( fp );    //ftell：获得当前文件指针到文件首的大小
    fseek( fp, 0, SEEK_SET );   //再定位文件指针到文件头

    if((0 == file_size) || (file_size > max_len) || (file_size < 34)){
        AfxMessageBox("new firmware size is too large or too small");
        return 0;
    }
    
    fread(data_out,file_size,1,fp);
    fclose(fp);

    return file_size;
}

void new_fw_write_file(u8 *data_in, u32 len)
{
    #define NEW_FW_RX_NAME  "new_firmware_rx.bin"
	errno_t err_fp;
	FILE *fp;
	err_fp=fopen_s(&fp,NEW_FW_RX_NAME,"wb");
	if(err_fp == 0){
		fseek(fp,0,SEEK_SET );
		fwrite(data_in, len,1,fp);
		fclose(fp);
	}
}

int GetCPUid(BYTE *an)
{           
    unsigned long s1 = 0,s2=0;       
    _asm       
    {       
        mov     eax,01h       
        xor     edx,edx       
        cpuid       
        mov     s1,edx       
        mov     s2,eax       
    }
 
    memcpy(an,(BYTE *)&s1,4);
    memcpy(an+4,(BYTE *)&s2,4);
    return 8;
}

int vc_get_guid(u8 *p_uuid)
{  
    memset(p_uuid,0,16);
    GetCPUid(p_uuid);
    return 1;  
}

void vc_get_dev_key(u8 *p_devkey)
{
	memset(p_devkey,0,16);
	GetCPUid(p_devkey);
	// the other 8 bytes will use random num 
	for(int i=8;i<16;i++){
		p_devkey[i]=rand()&0xff;
	}
	return ;
}

void vc_get_gateway_dev_key(u8 *p_dev_key)
{
	memset(p_dev_key,0,16);
	for(int i=0;i<16;i++){
		p_dev_key[i]= rand()&0xff;
	}
}

void test_utc_to_tai()
{
#if 0	// TEST UTC to TAI
		static u32 B_1[2] = {0};
		CTime time1 = CTime::GetCurrentTime(); 
		u64 nTSeconds = time1.GetTime();
		CTime time2(2000,1,1,0,0,0); // 946656000
		u64 nTSeconds2 = time2.GetTime();
		
		LOG_MSG_INFO(TL_LOG_COMMON,0, 0,"test start......",0);
		mesh_UTC_t test_UTC = {0};
		//for(u64 i = 0; i < (TAI_SECOND_MAX); i++) 		// 0x100000000
		for(u64 i = 0; i < (TAI_SECOND_MAX); i += 3600*24)	// 0x100000000
		{
			get_UTC(i, &test_UTC);
			u64 sec_fw = get_TAI_sec(&test_UTC);
			CTime time2(test_UTC.year,test_UTC.month,test_UTC.day,test_UTC.hour,test_UTC.minute,test_UTC.second); 
			u64 nTSeconds3 = time2.GetTime() - OFFSET_1970_2000_EAST8;
			if((i == nTSeconds3) && (i == sec_fw)){
				B_1[0]++;
			}else{
				B_1[2]++;
			}
		}
		LOG_MSG_INFO(TL_LOG_COMMON,0, 0,"test end: ok 0x%x, error 0x%x,",B_1[0],B_1[1]);
		//while(1);
#endif

}

