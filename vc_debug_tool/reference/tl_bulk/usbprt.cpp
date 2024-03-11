/********************************************************************************************************
 * @file	usbprt.cpp
 *
 * @brief	for TLSR chips
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
/* Code to find the device path for a usbprint.sys controlled 
 * usb printer and print to it
 */

#include "stdafx.h"
#include <setupapi.h>

#include <winioctl.h>
#include "usbprt.h"

#define USBPRINT_IOCTL_INDEX  0x0000


#define IOCTL_USBPRINT_GET_LPT_STATUS  CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   USBPRINT_IOCTL_INDEX+12,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)                                                           

#define IOCTL_USBPRINT_GET_1284_ID     CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   USBPRINT_IOCTL_INDEX+13,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)                                                           

#define IOCTL_USBPRINT_VENDOR_SET_COMMAND CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   USBPRINT_IOCTL_INDEX+14,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)                                                           

#define IOCTL_USBPRINT_VENDOR_GET_COMMAND CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   USBPRINT_IOCTL_INDEX+15,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)                                                           

#define IOCTL_USBPRINT_SOFT_RESET         CTL_CODE(FILE_DEVICE_UNKNOWN,  \
                                                   USBPRINT_IOCTL_INDEX+16,\
                                                   METHOD_BUFFERED,  \
                                                   FILE_ANY_ACCESS)                                                           


/* This define is required so that the GUID_DEVINTERFACE_USBPRINT variable is
 * declared an initialised as a static locally, since windows does not include it in any
 * of its libraries
 */
/*
#define SS_DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
static const GUID DECLSPEC_SELECTANY name \
= { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

SS_DEFINE_GUID(GUID_DEVINTERFACE_USBPRINT, 0x28d78fad, 0x5a12, 0x11D1, 0xae, 0x5b, 0x00, 0x00, 0xf8, 0x03, 0xa8, 0xc2);
*/

//GUID DECLSPEC_SELECTANY GUID_DEVINTERFACE_USBPRINT = { 0x28d78fad, 0x5a12, 0x11D1, { 0xae, 0x5b, 0x00, 0x00, 0xf8, 0x03, 0xa8, 0xc2 } };
GUID GUID_DEVINTERFACE_USBPRINT = { 0x28d78fad, 0x5a12, 0x11D1, { 0xae, 0x5b, 0x00, 0x00, 0xf8, 0x03, 0xa8, 0xc2 } };

int is_B91_gw_usb_id(char * interfacename, unsigned int dev_RAM);

extern HANDLE NULLEVENT2;
HANDLE GetPrintDeviceHandle(unsigned short id)
{
  HDEVINFO devs;
  DWORD devcount;
  SP_DEVINFO_DATA devinfo;
  SP_DEVICE_INTERFACE_DATA devinterface;
  DWORD size;
  GUID intfce;
  PSP_DEVICE_INTERFACE_DETAIL_DATA interface_detail;
  HANDLE usbHandle = 0;
  HANDLE last_handle = 0;

  intfce = GUID_DEVINTERFACE_USBPRINT;
  devs = SetupDiGetClassDevs(&intfce, 0, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
  if (devs == INVALID_HANDLE_VALUE) {
    return NULL;
  }
  devcount = 0;
  devinterface.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
  while (SetupDiEnumDeviceInterfaces(devs, 0, &intfce, devcount, &devinterface)) {
    /* The following buffers would normally be malloced to he correct size
     * but here we just declare them as large stack variables
     * to make the code more readable
     */
    char driverkey[2048];
    char interfacename[2048];
    char location[2048];
    //char description[2048];

    /* If this is not the device we want, we would normally continue onto the next one
     * so something like if (!required_device) continue; would be added here
     */
    devcount++;
    size = 0;
    /* See how large a buffer we require for the device interface details */
    SetupDiGetDeviceInterfaceDetail(devs, &devinterface, 0, 0, &size, 0);
    devinfo.cbSize = sizeof(SP_DEVINFO_DATA);
    interface_detail = (struct _SP_DEVICE_INTERFACE_DETAIL_DATA_A *)calloc(1, size);
    if (interface_detail) {
      interface_detail->cbSize = sizeof (SP_DEVICE_INTERFACE_DETAIL_DATA);
      devinfo.cbSize = sizeof(SP_DEVINFO_DATA);
      if (!SetupDiGetDeviceInterfaceDetail(devs, &devinterface, interface_detail, size, 0, &devinfo)) {
		free(interface_detail);
		SetupDiDestroyDeviceInfoList(devs);
		return NULL;
      }
      /* Make a copy of the device path for later use */
      strcpy_s(interfacename, interface_detail->DevicePath);
      free(interface_detail);
      /* And now fetch some useful registry entries */
      size = sizeof(driverkey);
      driverkey[0] = 0;
      //if (!SetupDiGetDeviceRegistryProperty(devs, &devinfo, SPDRP_DRIVER, &dataType, (LPBYTE)driverkey, size, 0)) {
	  if (!SetupDiGetDeviceRegistryProperty(devs, &devinfo, SPDRP_DRIVER, NULL, (LPBYTE)driverkey, size, 0)) {
		SetupDiDestroyDeviceInfoList(devs);
		return NULL;
      }
      size = sizeof(location);
      location[0] = 0;
      //if (!SetupDiGetDeviceRegistryProperty(devs, &devinfo, SPDRP_LOCATION_INFORMATION, &dataType, (LPBYTE)location, size, 0)) {
	  //if (!SetupDiGetDeviceRegistryProperty(devs, &devinfo, SPDRP_LOCATION_INFORMATION, NULL, (LPBYTE)location, size, 0)) {
		//SetupDiDestroyDeviceInfoList(devs);
		//return NULL;
      //}
    int gw_B91_flag = 0;
	char * str_tlk = strstr (interfacename, "vid_248a&pid_08d4"); // manual usb descriptor
	if(str_tlk){
		gw_B91_flag = 1;
	}else{
		str_tlk = strstr (interfacename, "vid_248a&pid_");
	}
	if (str_tlk) {
		last_handle = usbHandle;
		usbHandle = CreateFile(interfacename, 
						GENERIC_READ | GENERIC_WRITE,
						FILE_SHARE_READ | FILE_SHARE_WRITE,
						NULL, 
						OPEN_ALWAYS, 
						0, //FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, 
						//FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, 
						NULL);
      // usbHandle = CreateFile(interfacename, GENERIC_WRITE, FILE_SHARE_READ,
	  	//		     NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (usbHandle != INVALID_HANDLE_VALUE) {
		unsigned int dev = 0;
		unsigned int reg_addr_usb_id = 0x7e; // 0x801401fe; // 
		ReadMem (usbHandle, reg_addr_usb_id, (LPBYTE) &dev, 4, 2);
		if (gw_B91_flag || (dev == id) || (id == 0xffff)) {
			SetupDiDestroyDeviceInfoList(devs);
			  return usbHandle;
		}
		else{
			ReadMem2 (usbHandle, reg_addr_usb_id, (LPBYTE) &dev, 4, 2);
			if ((dev == id) || (id == 0xffff)) { // (dev != 0x0818) // (dev == id)
				SetupDiDestroyDeviceInfoList(devs);
				return usbHandle;
			}
		}

		#if 1 // use manual usb descriptor
		str_tlk = strstr (interfacename, "vid_248a&pid_5320"); // mcu default print device
		if(str_tlk && is_B91_gw_usb_id(interfacename, dev)){
			SetupDiDestroyDeviceInfoList(devs);
			return usbHandle;
		}
		#endif
		CloseHandle(usbHandle); // close should be better when insert EVK.
     }
    }
  }
  }
  SetupDiDestroyDeviceInfoList(devs);
	return 0;
}


int WriteUSBMem(HANDLE hdev, int addr, LPBYTE lpB, int len, int fifo=0, int maxlen=32)
{
  
	DWORD	nBytes;
	 
	BYTE	buff[4096];
	if (fifo)
		buff[0] = 0x03;
	else
		buff[0] = 0x02;
	buff[1] = (addr>>8) & 0xff;
	buff[2] = addr & 0xff;
	buff[3] = lpB[0];
	buff[4] = lpB[0];
	buff[5] = lpB[0];
	buff[6] = lpB[0];
	buff[7] = lpB[0];
	

	int nW = (len > maxlen ? maxlen : len);
	for (int i=0; i<nW; i++)
		buff[8+i] = lpB[i];
	

	int bRet = DeviceIoControl(
		hdev, 
		//IOCTL_USBPRINT_GET_1284_ID, // dwIoControlCode
		IOCTL_USBPRINT_VENDOR_SET_COMMAND,	// dwIoControlCode
		buff,					// lpInBuffer
		nW+8,									// nInBufferSize
		NULL,
		0,
		(LPDWORD)  &nBytes,
		NULL 
	);
	if (!bRet)
		return 0;
	else
		return nW;
}

int ReadUSBMem(HANDLE hdev, int addr, LPBYTE lpB, int len, int fifo=0, int maxlen=32)
{
  
	DWORD	nBytes;
	BYTE	buff[16];
	if (fifo)
		buff[0] = 0x03;
	else
		buff[0] = 0x02;
	buff[1] = (addr>>8) & 0xff;
	buff[2] = addr & 0xff;
	buff[3] = lpB[0];
	
	DWORD nR = 8 + (len > maxlen ? maxlen :  len<1 ? 1 : len);
	
  
	//HANDLE hh = GetPrintDeviceHandle ();
	int bRet = DeviceIoControl(
		hdev, 
		//IOCTL_USBPRINT_GET_1284_ID, // dwIoControlCode
		IOCTL_USBPRINT_VENDOR_GET_COMMAND,	// dwIoControlCode
		buff,					// lpInBuffer
		16,									// nInBufferSize
		lpB,
		nR,
		(LPDWORD)  &nBytes,
		NULL 
	);

	//if (nBytes != nR - 8 || (lpB[nBytes-1]&4))
	if (!bRet || nBytes != nR - 8)
		return 0;
	else
		return nBytes;
}

int LogMsg (LPSTR sz,...);

int WriteUSBMemCheck(HANDLE hdev, int addr, LPBYTE lpB, int len, int fifo=0, int maxlen=32)
{
	BYTE rbuff[4096];
	int nc = 10;
	int l = 0;
	int retw, retr;

	while (l++<nc) { 
		retw = WriteUSBMem (hdev, addr, lpB, len, fifo, maxlen);
		if (len <= 8) 
			return retw;
		
		int ok = 1;
		retr = ReadUSBMem  (hdev, addr, rbuff, len, fifo, maxlen);
		for (int k=0; k<len; k++) {
			if (lpB[k] != rbuff[k]) {
				ok = 0;
				//LogMsg ("USB write error at adrress %x, retry", addr + k);
				break;
			}
		}
		if (ok) 
			return retw;
		else
			ok = ok;
	}
	return 0;
}

int WriteMem(HANDLE hdev, int addr, LPBYTE lpB, int len, int type=1)
{
  
	BYTE ah = addr>>16;
	//WriteI2CMem (hdev, 0xffff, &ah, 1, 1);
	int ret = 0;

	int al = addr & 0xffff;
	int step = 1024;
	//int step = 3072;

	for (int i=0; i<len; i+=step) {
		int n = len - i > step ? step : len - i;
		int fadr = al + i;
		if (type & 0x200) fadr = al;
		//ret += WriteUSBMem (hdev, fadr, lpB+i, n, type & 0x200, step);
		int rw = WriteUSBMemCheck (hdev, fadr, lpB+i, n, type & 0x200, step);
		if (rw)
			ret += rw;
		else 
			break;
	}
	return ret;
}

int ReadMem(HANDLE hdev, int addr, LPBYTE lpB, int len, int type=1)
{
  
	BYTE ah = addr>>16;
	//WriteI2CMem (hdev, 0xff08, &ah, 1, 1);
	int ret = 0;

	int al = addr & 0xffff;
	//int step = 32;
	int step = 3072;

	for (int i=0; i<len; i+=step) {
		int n = len - i > step ? step : len - i;
		int fadr = al + i;
		if (type & 0x200) fadr = al;
		ret += ReadUSBMem (hdev, fadr, lpB+i, n, type & 0x200, step);
	}
	return ret;
}

int ReadUartMem(HANDLE hdev, LPBYTE lpB)
{
  
	DWORD	nB;
	int ret = ReadFile(hdev, lpB, 2048, &nB, NULL);
	if (ret==0)
		return -1;
	else
		return nB;
	
}

int WriteUSBReg(HANDLE hdev, int addr, int dat)
{
  
	BYTE buff[16];
	*((int *) buff) = dat;
	
	int bRet = WriteUSBMem(hdev, addr, buff, 1);
	return bRet;
}

int ReadUSBReg(HANDLE hdev, int addr, int  & dat)
{
  
	BYTE buff[16];
	int bRet = ReadUSBMem(hdev, addr, buff, 1);
	dat = buff[0];
	return bRet;
}

//************************************************************************************************
//************************************************************************************************
//  Next functions are for 5562 chips
//  the function name is ****2()
//  the name is the same as the upper functions
//************************************************************************************************
//************************************************************************************************

HANDLE GetPrintDeviceHandle2(unsigned short id)
{
  HDEVINFO devs;
  DWORD devcount;
  SP_DEVINFO_DATA devinfo;
  SP_DEVICE_INTERFACE_DATA devinterface;
  DWORD size;
  GUID intfce;
  PSP_DEVICE_INTERFACE_DETAIL_DATA interface_detail;
  HANDLE usbHandle = 0;
  HANDLE last_handle = 0;

  intfce = GUID_DEVINTERFACE_USBPRINT;
  devs = SetupDiGetClassDevs(&intfce, 0, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
  if (devs == INVALID_HANDLE_VALUE) {
    return NULL;
  }
  devcount = 0;
  devinterface.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
  while (SetupDiEnumDeviceInterfaces(devs, 0, &intfce, devcount, &devinterface)) {
    /* The following buffers would normally be malloced to he correct size
     * but here we just declare them as large stack variables
     * to make the code more readable
     */
    char driverkey[2048];
    char interfacename[2048];
    char location[2048];
    //char description[2048];

    /* If this is not the device we want, we would normally continue onto the next one
     * so something like if (!required_device) continue; would be added here
     */
    devcount++;
    size = 0;
    /* See how large a buffer we require for the device interface details */
    SetupDiGetDeviceInterfaceDetail(devs, &devinterface, 0, 0, &size, 0);
    devinfo.cbSize = sizeof(SP_DEVINFO_DATA);
    interface_detail = (struct _SP_DEVICE_INTERFACE_DETAIL_DATA_A *)calloc(1, size);
    if (interface_detail) {
      interface_detail->cbSize = sizeof (SP_DEVICE_INTERFACE_DETAIL_DATA);
      devinfo.cbSize = sizeof(SP_DEVINFO_DATA);
      if (!SetupDiGetDeviceInterfaceDetail(devs, &devinterface, interface_detail, size, 0, &devinfo)) {
		free(interface_detail);
		SetupDiDestroyDeviceInfoList(devs);
		return NULL;
      }
      /* Make a copy of the device path for later use */
      strcpy_s(interfacename, interface_detail->DevicePath);
      free(interface_detail);
      /* And now fetch some useful registry entries */
      size = sizeof(driverkey);
      driverkey[0] = 0;
      //if (!SetupDiGetDeviceRegistryProperty(devs, &devinfo, SPDRP_DRIVER, &dataType, (LPBYTE)driverkey, size, 0)) {
	  if (!SetupDiGetDeviceRegistryProperty(devs, &devinfo, SPDRP_DRIVER, NULL, (LPBYTE)driverkey, size, 0)) {
		SetupDiDestroyDeviceInfoList(devs);
		return NULL;
      }
      size = sizeof(location);
      location[0] = 0;
      //if (!SetupDiGetDeviceRegistryProperty(devs, &devinfo, SPDRP_LOCATION_INFORMATION, &dataType, (LPBYTE)location, size, 0)) {
	  //if (!SetupDiGetDeviceRegistryProperty(devs, &devinfo, SPDRP_LOCATION_INFORMATION, NULL, (LPBYTE)location, size, 0)) {
		//SetupDiDestroyDeviceInfoList(devs);
		//return NULL;
      //}
	  char * str_tlk = strstr (interfacename, "vid_248a&pid_");
	if (str_tlk) {
		last_handle = usbHandle;
	   usbHandle = CreateFile(interfacename, 
						GENERIC_READ | GENERIC_WRITE,
						FILE_SHARE_READ | FILE_SHARE_WRITE,
						NULL, 
						OPEN_ALWAYS, 
						0, //FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, 
						//FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, 
						NULL);
	  }
      // usbHandle = CreateFile(interfacename, GENERIC_WRITE, FILE_SHARE_READ,
	  	//		     NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
      if (usbHandle != INVALID_HANDLE_VALUE) {
			unsigned int dev = 0;
			ReadMem2 (usbHandle, 0x7e, (LPBYTE) &dev, 4, 2);
		if ((dev == id) || (id == 0xffff)) {
  			SetupDiDestroyDeviceInfoList(devs);
				return usbHandle;
		}
		
		
	}	
		
		
		
   }
 }
  SetupDiDestroyDeviceInfoList(devs);
	return 0;
}		


int IsEVKDevice2 (HANDLE h)
{
	DWORD id;
	int ret = ReadUSBMem2(h, 0x7c, (LPBYTE) &id, 4, 0, 1024);
	if ((id & 0xffff0000) == 0x08180000)
		return 4;
	else
		return 0;
}


int EVKCommand2 (HANDLE hdev, int addr, int len, int cmd, LPBYTE lpB, int * ptick)
{
	int cmdbuf[16];
	cmdbuf[0] = addr;
	cmdbuf[1] = (len & 0xffff) | (cmd<<16);
	int ret = WriteUSBMem2(hdev, 0x9ff8, (LPBYTE) cmdbuf, 8, 0, 1024);
	int rlen = len;
	//if ((cmd & 0x3f00) == UCMD_FLASH) rlen = 1;

	if (!ret) return 0;

	DWORD ts = GetTickCount ();
	int tl = 1000;
	//WaitForSingleObject (NULLEVENT2, 5);
	do {
	//for (int l=0; l<tl; l++) {
		ret = ReadUSBMemCheck2(hdev, 0x9ff0, (LPBYTE) cmdbuf, 16, 0, 1024);
		if (cmdbuf[0]&0xf0000000) {
			return 0;
		}
		else if (cmdbuf[0] == rlen) {
			if (ptick) *ptick = cmdbuf[1];
			return len;
		}
		else if (cmdbuf[0]) {
			//int t = l;
		}
		//WaitForSingleObject (NULLEVENT2, 1);
	} while (GetTickCount() - ts < (DWORD)tl);
	return 0;
}

int WriteUSBMem2(HANDLE hdev, int addr, LPBYTE lpB, int len, int fifo=0, int maxlen=32)
{
  
	DWORD	nBytes;
	 
	BYTE	buff[4096];
	int     adr_h = (addr>>16) & 0x3f;
	if (fifo)
		buff[0] = 0xc0 | adr_h;
	else
		buff[0] = 0x80 | adr_h;
	buff[2] = (addr>>8) & 0xff;
	buff[1] = addr & 0xff;
	buff[3] = lpB[0];
	buff[4] = lpB[0];
	buff[5] = lpB[0];
	buff[6] = lpB[0];
	buff[7] = lpB[0];
	

	int nW = (len > maxlen ? maxlen : len);
	for (int i=0; i<nW; i++)
		buff[8+i] = lpB[i];
	

	int bRet = DeviceIoControl(
		hdev, 
		//IOCTL_USBPRINT_GET_1284_ID, // dwIoControlCode
		IOCTL_USBPRINT_VENDOR_SET_COMMAND,	// dwIoControlCode
		buff,					// lpInBuffer
		nW+8,									// nInBufferSize
		NULL,
		0,
		(LPDWORD)  &nBytes,
		NULL 
	);
	if (!bRet)
		return 0;
	else
		return nW;
}

int ReadUSBMem2(HANDLE hdev, int addr, LPBYTE lpB, int len, int fifo=0, int maxlen=32)
{
  
	DWORD	nBytes;
	BYTE	buff[32];
	int     adr_h = (addr>>16) & 0x3f;
	if (fifo)
		buff[0] = 0xc0 | adr_h;
	else
		buff[0] = 0x80 | adr_h;
	buff[2] = (addr>>8) & 0xff;
	buff[1] = addr & 0xff;
	buff[3] = lpB[0];
	
	int nR = 8 + (len > maxlen ? maxlen :  len<1 ? 1 : len);
	
  
	//HANDLE hh = GetPrintDeviceHandle ();
	int bRet = DeviceIoControl(
		hdev, 
		//IOCTL_USBPRINT_GET_1284_ID, // dwIoControlCode
		IOCTL_USBPRINT_VENDOR_GET_COMMAND,	// dwIoControlCode
		buff,					// lpInBuffer
		16,									// nInBufferSize
		lpB,
		nR,
		(LPDWORD)  &nBytes,
		NULL 
	);

	//if (nBytes != nR - 8 || (lpB[nBytes-1]&4))
	if (!bRet || nBytes != nR - 8)
		return 0;
	else
		return nBytes;
}

int ReadUSBMemCheck2(HANDLE hdev, int addr, LPBYTE lpB, int len, int fifo=0, int maxlen=32)
{
  
	DWORD	nBytes = 0;
	BYTE	buff[32]; 
	int     adr_h = (addr>>16) & 0x3f;
	if (fifo)
		buff[0] = 0xc0 | adr_h;
	else
		buff[0] = 0x80 | adr_h;
	buff[2] = (addr>>8) & 0xff;
	buff[1] = addr & 0xff;
	buff[3] = lpB[0];
	
	int nR = 8 + (len > maxlen ? maxlen :  len<1 ? 1 : len);
	
	int retry = (nR + 8) / 4;
	BOOL bRet = FALSE;
  
	while (retry>0 && nR>0)
	{
		retry --;
		//HANDLE hh = GetPrintDeviceHandle ();
		bRet = DeviceIoControl(
			hdev, 
			//IOCTL_USBPRINT_GET_1284_ID, // dwIoControlCode
			IOCTL_USBPRINT_VENDOR_GET_COMMAND,	// dwIoControlCode
			buff,					// lpInBuffer
			16,									// nInBufferSize
			lpB,
			nR,
			(LPDWORD)  &nBytes,
			NULL 
		);
        if (!bRet) {
            return -1;
        }
		if (bRet && nBytes == nR - 8)
			return len;
		nR -= nBytes;
		lpB += nBytes;
		addr += nBytes;
		buff[1] = (addr>>8) & 0xff;
		buff[2] = addr & 0xff;
	}
	return 0;
}

//int LogMsg (LPSTR sz,...);

int WriteUSBMemCheck2(HANDLE hdev, int addr, LPBYTE lpB, int len, int fifo=0, int maxlen=32)
{
	BYTE rbuff[4096];
	int nc = 10;
	int l = 0;
	int retw, retr;

	while (l++<nc) { 
		retw = WriteUSBMem2 (hdev, addr, lpB, len, fifo, maxlen);
		if (len <= 8) 
			return retw;
		
		int ok = 1;
		retr = ReadUSBMemCheck2  (hdev, addr, rbuff, len, fifo, maxlen);
		//retr = ReadUSBMem  (hdev, addr, rbuff, len, fifo, maxlen);
		for (int k=0; k<len; k++) {
			if (lpB[k] != rbuff[k]) {
				ok = 0;
				//LogMsg ("USB write adrress %x", addr + k);
				break;
			}
		}
		return retw;
		if (ok) 
			return retw;
		else
			ok = ok;
	}
	return 0;
}

//#define		UCMD_CORE		0x0100
//#define		UCMD_FRD		0x8000
//#define		UCMD_FSWIRE		0x4000

int WriteMem2(HANDLE hdev, int addr, LPBYTE lpB, int len, int type=1)
{
  
	BYTE ah = addr>>16;
	//WriteI2CMem (hdev, 0xffff, &ah, 1, 1);
	int ret = 0;

	int al = addr ;//& 0xffff;
	int step = 1024;
	//int step = 3072;

	for (int i=0; i<len; i+=step) {
		int n = len - i > step ? step : len - i;
		int fadr = al + i;
		if (type & 0x200) fadr = al;
		//ret += WriteUSBMem (hdev, fadr, lpB+i, n, type & 0x200, step);
		int rw;
		rw = WriteUSBMemCheck2 (hdev, fadr, lpB+i, n, type & 0x200, step);
		
		if (rw)
			ret += rw;
		else 
			break;
	}
	return ret;
}

int ReadMem2(HANDLE hdev, int addr, LPBYTE lpB, int len, int type=1)
{
  
	BYTE ah = addr>>16;
	//WriteI2CMem (hdev, 0xff08, &ah, 1, 1);
	int ret = 0;

	int al = addr ;//& 0xffff;
	//int step = 32;
	int step = 3072;

	for (int i=0; i<len; i+=step) {
		int n = len - i > step ? step : len - i;
		int fadr = al + i;
		if (type & 0x200) fadr = al;
	
		//ret += ReadUSBMem (hdev, fadr, lpB+i, n, type & 0x200, step);
		int r = ReadUSBMemCheck2 (hdev, fadr, lpB+i, n, type & 0x200, step);
        if (r < 0) {
            return r;
        }
        ret += r;
	}
	return ret;
}
/*
int ReadUartMem(HANDLE hdev, LPBYTE lpB)
{
  
	DWORD	nB;
	int ret = ReadFile(hdev, lpB, 2048, &nB, NULL);
	if (ret==0)
		return -1;
	else
		return nB;
	
}
*/
int WriteUSBReg2(HANDLE hdev, int addr, int dat)
{
  
	BYTE buff[16];
	*((int *) buff) = dat;
	
	int bRet = WriteUSBMem2(hdev, addr, buff, 1);
	return bRet;
}

int ReadUSBReg2(HANDLE hdev, int addr, int  & dat)
{
  
	BYTE buff[16];
	int bRet = ReadUSBMem2(hdev, addr, buff, 1);
	dat = buff[0];
	return bRet;
}
