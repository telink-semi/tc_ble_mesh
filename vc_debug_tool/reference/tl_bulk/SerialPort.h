/********************************************************************************************************
 * @file     SerialPort.h 
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
