/********************************************************************************************************
 * @file     SerialPort.cpp 
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

    CString strComName;//��������
    CString strDrName;//������ϸ����

    result = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                            _T( "Hardware\\DeviceMap\\SerialComm" ),
                            NULL,
                            KEY_READ,
                            &hkey );

    if( ERROR_SUCCESS == result )   //   �򿪴���ע���   
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
                break;   //   commName���Ǵ�������"COM2"
            }

            strComName=commName;
            strDrName = portName;

            // ��������߿�ʼ���ҵ�һ��'\\'����ȡ����ַ����ĳ���
            int len = strDrName.ReverseFind( '\\');
            // ��ȡ'\\'��ߵ��ַ���
            CString strFilePath = strDrName.Left( len+1 );
            // ��ȡ'\\'�ұߵ��ַ���
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
	pp.ParentWnd=TheParentWnd;//���������ھ��
	pp.MsgCode=MsgStartCode;
}

CSerialPort::~CSerialPort()
{
}


// CSerialPort ��Ա����

bool CSerialPort::Open232Port(CString ComNum, DWORD BandRate, BYTE ByteSize, BYTE Parity, BYTE StopBits)
{
	//��ComNum����Ϊ�첽���ڣ����߳��м����¼�
	PortFile=INVALID_HANDLE_VALUE;
	IsPortOpen=false;
	EventForThreadOff=NULL;
	ComNum=_T("//./") +ComNum;
	PortFile=CreateFile(ComNum,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,NULL);
	if( INVALID_HANDLE_VALUE==PortFile){
		return false;
	}
	pp.hCom=PortFile;//���洮�����ݵ������̵߳Ĳ���
	SetCommMask(PortFile,EV_RXCHAR|EV_TXEMPTY);
	SetupComm(PortFile,1024,1024);
	if((GetCommState(PortFile, &m_dcb))==0){
		CloseHandle(PortFile);
		return false;}
	m_dcb.BaudRate=BandRate;//������
	m_dcb.ByteSize=ByteSize;//����λ
	m_dcb.Parity=Parity ;//��żУ��
	m_dcb.StopBits =StopBits;//ֹͣλ
	m_dcb.fBinary=TRUE;//��������ʽ
	m_dcb.fDsrSensitivity=false;//��DSR�ź��߲�����
	m_dcb.fParity=false;//���Ϊ����żУ���������������δʹ����żУ�飬���Ϊ��
	m_dcb.fOutX=false;//���������ַ�����
	m_dcb.fInX=false;//�������տ���
	m_dcb.fNull=false;//����NULL�ַ�
	m_dcb.fAbortOnError=false;//���ʹ���󣬼�����������Ķ�д����
	m_dcb.fOutxCtsFlow=false;//����CTS��ⷢ��������
	m_dcb.fOutxDsrFlow=false;//����DSR��ⷢ��������
	m_dcb.fDtrControl=DTR_CONTROL_DISABLE;//��ֹDTR��������
	m_dcb.fDsrSensitivity=false;//��DTR�ź��߲�����
	m_dcb.fRtsControl=RTS_CONTROL_DISABLE;//��ֹRTS��������
	m_dcb.fOutxCtsFlow=false;//����CTS��ⷢ��������
	m_dcb.fOutxDsrFlow=false;//����DSR��ⷢ��������
	if(SetCommState(PortFile, &m_dcb)==false){
		CloseHandle(PortFile);
		return false;}
	
	//����ʱ��
	if(!GetCommTimeouts(PortFile, &m_CommTimeouts)){
		return false;
	}

	m_CommTimeouts.ReadIntervalTimeout=MAXDWORD;//�ַ�������ms   �ò������Ϊ���ֵ����ʹreadfile������������
    m_CommTimeouts.ReadTotalTimeoutMultiplier=0;//�ܵĳ�ʱʱ��(�Ե����ֽ�) 
    m_CommTimeouts.ReadTotalTimeoutConstant=0;//����ĳ�ʱʱ��ms 

	m_CommTimeouts.WriteTotalTimeoutMultiplier =0;//�ܵĳ�ʱʱ��(�Ե����ֽ�) 
	m_CommTimeouts.WriteTotalTimeoutConstant = 1000;//����ĳ�ʱʱ��

	if(!SetCommTimeouts (PortFile, &m_CommTimeouts)){
		CloseHandle(PortFile);
		return false;}
	PurgeComm(PortFile, PURGE_TXCLEAR|PURGE_RXCLEAR);
	memset(&OverlappedWrite,0,sizeof(OVERLAPPED));
	OverlappedWrite.hEvent=CreateEvent(NULL,false,false,NULL);
	memset(&OverlappedRead,0,sizeof(OVERLAPPED));
	OverlappedRead.hEvent=CreateEvent(NULL,false,false,NULL);
	EventForThreadOff=CreateEvent(NULL, FALSE, FALSE, NULL);//����һ�����źŵ��¼�����
	pp.OffEvent=EventForThreadOff;//������߳��¼�����
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
	//д����ֱ�ӽ������������100msδд��������ʧ��
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
	//�ر��߳�
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
	while(WAIT_TIMEOUT==WaitForSingleObject(ppPTR->OffEvent,0))//���ź��˳��߳�
	{
		ClearCommError(ppPTR->hCom,&Error,&comStat);
		if(comStat.cbInQue)
		{
			::PostMessage(ppPTR->ParentWnd,ppPTR->MsgCode,(WPARAM)(&Error),(LPARAM)(&comStat));//������Ϣ��������,��֪�ѿɽ������ݽ��ղ���
		}
		Sleep(1);
	}
	return 1;
}
