/********************************************************************************************************
 * @file     usbprt.h 
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
/* Code to find the device path for a usbprint.sys controlled 
 * usb printer and print to it
 */


HANDLE GetPrintDeviceHandle(unsigned short id);

int WriteUSBReg(HANDLE hdev, int addr, int dat);
int ReadUSBReg(HANDLE hdev, int addr, int  & dat);

int WriteUSBMem(HANDLE hdev, int addr, LPBYTE lpB, int len, int fifo, int maxlen);
int ReadUSBMem(HANDLE hdev, int addr, LPBYTE lpB, int len, int fifo, int maxlen);

int WriteMem(HANDLE hdev, int addr, LPBYTE lpB, int len, int type);
int ReadMem(HANDLE hdev, int addr, LPBYTE lpB, int len, int type);
int ReadUartMem(HANDLE hdev, LPBYTE lpB);

HANDLE GetPrintDeviceHandle2(unsigned short id);

int WriteUSBReg2(HANDLE hdev, int addr, int dat);
int ReadUSBReg2(HANDLE hdev, int addr, int  & dat);

int WriteUSBMem2(HANDLE hdev, int addr, LPBYTE lpB, int len, int fifo, int maxlen);
int ReadUSBMem2(HANDLE hdev, int addr, LPBYTE lpB, int len, int fifo, int maxlen);

int WriteMem2(HANDLE hdev, int addr, LPBYTE lpB, int len, int type);
int ReadMem2(HANDLE hdev, int addr, LPBYTE lpB, int len, int type);
int ReadUartMem2(HANDLE hdev, LPBYTE lpB);
int ReadUSBMemCheck2(HANDLE hdev, int addr, LPBYTE lpB, int len, int fifo, int maxlen);