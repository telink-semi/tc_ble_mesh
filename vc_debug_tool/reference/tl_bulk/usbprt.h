/********************************************************************************************************
 * @file	usbprt.h
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