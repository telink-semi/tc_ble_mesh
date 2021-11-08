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

// CSerialPort ����Ŀ��

class CSerialPort : public CObject
{
public:
	//���븸���ھ����Ϊ�򸸴��ڷ�����Ϣ,�ڶ�������Ϊ��Ϣ��
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
	bool	IsPortOpen;//�Ƿ����232��
	OVERLAPPED	OverlappedWrite;
	OVERLAPPED	OverlappedRead;
	static	UINT __cdecl ComStatusThread( LPVOID pParam );//���ڼ�ض˿�״̬���߳�
	HANDLE	EventForThreadOff;//���ڹر��̵߳��¼�����
	sParams	pp;
};
