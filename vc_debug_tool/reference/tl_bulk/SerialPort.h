/********************************************************************************************************
 * @file	SerialPort.h
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
#pragma once



typedef	struct	Params{
	HANDLE	OffEvent;
	HWND	ParentWnd;
	HANDLE	hCom;
	DWORD	MsgCode;
}sParams;

// CSerialPort 命令目标

class CSerialPort : public CObject
{
public:
	//传入父窗口句柄是为向父窗口发送信息,第二个参数为消息码
	CSerialPort(HWND TheParentWnd,int MsgStartCode);
	virtual ~CSerialPort();
	bool Open232Port(CString ComNum, DWORD BandRate, BYTE ByteSize, BYTE Parity, BYTE StopBits);
	DWORD ReadBytes(BYTE *DataPTR,DWORD DataLen);
	DWORD WriteBytes(BYTE *DataPTR,DWORD DataLen);
    int CSerialPort::GetComList_Reg( CComboBox * CCombox );
public:
	
	HANDLE	PortFile;
	bool Close232Port(void);
private:
	DCB		m_dcb;
	COMMTIMEOUTS m_CommTimeouts;
	bool	IsPortOpen;//是否打开了232口
	OVERLAPPED	OverlappedWrite;
	OVERLAPPED	OverlappedRead;
	static	UINT __cdecl ComStatusThread( LPVOID pParam );//用于监控端口状态的线程
	HANDLE	EventForThreadOff;//用于关闭线程的事件对象
	sParams	pp;
};
