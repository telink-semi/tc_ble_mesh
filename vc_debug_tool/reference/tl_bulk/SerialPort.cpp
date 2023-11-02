/********************************************************************************************************
 * @file	SerialPort.cpp
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
#include "stdafx.h"
#include "SerialPort.h"


#include "tl_ble_module.h"
#include "ScanDlg.h"
#include "TLMeshDlg.h"
#include "tl_ble_moduleDlg.h"
// CSerialPort

int CSerialPort::GetComList_Reg( CComboBox * CCombox )
{
    HKEY hkey;  
    int result;
    int i = 0;

    CString strComName;//串口名称
    CString strDrName;//串口详细名称

    result = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                            _T( "Hardware\\DeviceMap\\SerialComm" ),
                            NULL,
                            KEY_READ,
                            &hkey );

    if( ERROR_SUCCESS == result )   //   打开串口注册表   
    {   
        TCHAR portName[ 0x100 ], commName[ 0x100 ];
        DWORD dwLong, dwSize;

        CCombox->ResetContent();
        do
        {   
            dwSize = sizeof( portName ) / sizeof( TCHAR );
            dwLong = dwSize;
            result = RegEnumValue( hkey, i, portName, &dwLong, NULL, NULL, ( LPBYTE )commName, &dwSize );
            if( ERROR_NO_MORE_ITEMS == result )
            {
                break;   //   commName就是串口名字"COM2"
            }

            strComName=commName;
            strDrName = portName;

            // 从右往左边开始查找第一个'\\'，获取左边字符串的长度
            int len = strDrName.ReverseFind( '\\');
            // 获取'\\'左边的字符串
            CString strFilePath = strDrName.Left( len+1 );
            // 获取'\\'右边的字符串
            CString fileName = strDrName.Right(strDrName.GetLength() - len-1);
            fileName = strComName;// + _T(": ")+fileName;

            CCombox->AddString(fileName);
            i++;   
        } while ( 1 );

        RegCloseKey( hkey );   
    }
    CCombox->SetCurSel(0);
    return i;
}


CSerialPort::CSerialPort(HWND TheParentWnd,int MsgStartCode)
{
	pp.ParentWnd=TheParentWnd;//保存主窗口句柄
	pp.MsgCode=MsgStartCode;
}

CSerialPort::~CSerialPort()
{
}


// CSerialPort 成员函数

bool CSerialPort::Open232Port(CString ComNum, DWORD BandRate, BYTE ByteSize, BYTE Parity, BYTE StopBits)
{
	//打开ComNum串口为异步串口，在线程中监听事件
	PortFile=INVALID_HANDLE_VALUE;
	IsPortOpen=false;
	EventForThreadOff=NULL;
	ComNum=_T("//./") +ComNum;
	PortFile=CreateFile(ComNum,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,NULL);
	if( INVALID_HANDLE_VALUE==PortFile){
		return false;
	}
	pp.hCom=PortFile;//保存串口数据到传入线程的参数
	SetCommMask(PortFile,EV_RXCHAR|EV_TXEMPTY);
	SetupComm(PortFile,1024,1024);
	if((GetCommState(PortFile, &m_dcb))==0){
		CloseHandle(PortFile);
		return false;}
	m_dcb.BaudRate=BandRate;//波特率
	m_dcb.ByteSize=ByteSize;//数据位
	m_dcb.Parity=Parity ;//奇偶校验
	m_dcb.StopBits =StopBits;//停止位
	m_dcb.fBinary=TRUE;//二进制形式
	m_dcb.fDsrSensitivity=false;//对DSR信号线不敏感
	m_dcb.fParity=false;//如果为真奇偶校验出错将产生错误。因未使用奇偶校验，因此为假
	m_dcb.fOutX=false;//不做发送字符控制
	m_dcb.fInX=false;//不做接收控制
	m_dcb.fNull=false;//保留NULL字符
	m_dcb.fAbortOnError=false;//发送错误后，继续进行下面的读写操作
	m_dcb.fOutxCtsFlow=false;//不用CTS检测发送流控制
	m_dcb.fOutxDsrFlow=false;//不用DSR检测发送流控制
	m_dcb.fDtrControl=DTR_CONTROL_DISABLE;//禁止DTR流量控制
	m_dcb.fDsrSensitivity=false;//对DTR信号线不敏感
	m_dcb.fRtsControl=RTS_CONTROL_DISABLE;//禁止RTS流量控制
	m_dcb.fOutxCtsFlow=false;//不用CTS检测发送流控制
	m_dcb.fOutxDsrFlow=false;//不用DSR检测发送流控制
	if(SetCommState(PortFile, &m_dcb)==false){
		CloseHandle(PortFile);
		return false;}
	
	//设置时间
	if(!GetCommTimeouts(PortFile, &m_CommTimeouts)){
		return false;
	}

	m_CommTimeouts.ReadIntervalTimeout=MAXDWORD;//字符允许间隔ms   该参数如果为最大值，会使readfile命令立即返回
    m_CommTimeouts.ReadTotalTimeoutMultiplier=0;//总的超时时间(对单个字节) 
    m_CommTimeouts.ReadTotalTimeoutConstant=0;//多余的超时时间ms 

	m_CommTimeouts.WriteTotalTimeoutMultiplier =0;//总的超时时间(对单个字节) 
	m_CommTimeouts.WriteTotalTimeoutConstant = 1000;//多余的超时时间

	if(!SetCommTimeouts (PortFile, &m_CommTimeouts)){
		CloseHandle(PortFile);
		return false;}
	PurgeComm(PortFile, PURGE_TXCLEAR|PURGE_RXCLEAR);
	memset(&OverlappedWrite,0,sizeof(OVERLAPPED));
	OverlappedWrite.hEvent=CreateEvent(NULL,false,false,NULL);
	memset(&OverlappedRead,0,sizeof(OVERLAPPED));
	OverlappedRead.hEvent=CreateEvent(NULL,false,false,NULL);
	EventForThreadOff=CreateEvent(NULL, FALSE, FALSE, NULL);//创建一个无信号的事件对象
	pp.OffEvent=EventForThreadOff;//保存关线程事件对象
/*
	if(!AfxBeginThread(ComStatusThread,&pp,0,0,0,0)){
		CloseHandle(PortFile);
		return false;
	}*/
	IsPortOpen=true;
	return	true;
}

DWORD CSerialPort::ReadBytes(BYTE *DataPTR,DWORD DataLen)
{
	DWORD	BytesRead=0;
	ReadFile(PortFile,DataPTR,DataLen,&BytesRead,&OverlappedRead);
	if(WaitForSingleObject(OverlappedRead.hEvent,10)==WAIT_TIMEOUT)
	{
		BytesRead=0;
	}
	return BytesRead;
}


DWORD CSerialPort::WriteBytes(BYTE *DataPTR,DWORD DataLen)
{
	DWORD BytesWritted=0;


	if(!IsPortOpen)
	{
		return 0;
	}

    TRACE("UART TX[%02d]:", DataLen);
    ((CTl_ble_moduleDlg*)(CTl_ble_moduleDlg::FromHandle(pp.ParentWnd)))->LogMsg("TX[%d]:", DataLen);
	for(DWORD i = 0; i < DataLen; i++)
	{
	    TRACE("%02x ", DataPTR[i]);
		((CTl_ble_moduleDlg*)(CTl_ble_moduleDlg::FromHandle(pp.ParentWnd)))->LogMsg("%02x ", DataPTR[i]);
	}
	((CTl_ble_moduleDlg*)(CTl_ble_moduleDlg::FromHandle(pp.ParentWnd)))->LogMsg("\n");
    TRACE("\n");

	WriteFile(PortFile,DataPTR,DataLen,&BytesWritted,&OverlappedWrite);
	//写操作直接进行阻塞，如果100ms未写完成则表明失败
	if(WaitForSingleObject(OverlappedWrite.hEvent,100)==WAIT_TIMEOUT)
	{
		BytesWritted=0;
	}
	return BytesWritted;
}

bool CSerialPort::Close232Port(void)
{
	if(!IsPortOpen)
		return true;
	//关闭线程
	if(!SetEvent(EventForThreadOff))
		return false;
	if(!CloseHandle(PortFile))
		return false;
	return true;
}

#define	ppPTR	((sParams*)(pParam))
UINT  CSerialPort::ComStatusThread( LPVOID pParam )
{
	DWORD	Error;
	COMSTAT	comStat;
	while(WAIT_TIMEOUT==WaitForSingleObject(ppPTR->OffEvent,0))//有信号退出线程
	{
		ClearCommError(ppPTR->hCom,&Error,&comStat);
		if(comStat.cbInQue)
		{
			::PostMessage(ppPTR->ParentWnd,ppPTR->MsgCode,(WPARAM)(&Error),(LPARAM)(&comStat));//发送消息到主窗口,告知已可进行数据接收操作
		}
		Sleep(1);
	}
	return 1;
}
