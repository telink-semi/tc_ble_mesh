/********************************************************************************************************
 * @file     tl_ble_module.cpp 
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
// tl_ble_module.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "tl_ble_module.h"
#include "ScanDlg.h"
#include "TLMeshDlg.h"
#include "tl_ble_moduleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTl_ble_moduleApp

BEGIN_MESSAGE_MAP(CTl_ble_moduleApp, CWinApp)
	//{{AFX_MSG_MAP(CTl_ble_moduleApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTl_ble_moduleApp construction

CTl_ble_moduleApp::CTl_ble_moduleApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTl_ble_moduleApp object

CTl_ble_moduleApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CTl_ble_moduleApp initialization

BOOL CTl_ble_moduleApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	//Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	//Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
	CString str(::GetCommandLine());
	int index = str.Find("Restart");
	if (index >= 0) {
		Sleep(2000);
	}

	HANDLE m_hMutex = CreateMutex(NULL, FALSE, __FUNCTION__);
	DWORD nRet = GetLastError();
	if (nRet == ERROR_ALREADY_EXISTS) {
		AfxMessageBox(_T("应用程序已打开"));
		CloseHandle(m_hMutex);
		m_hMutex = NULL;
		return FALSE;
	}

	CTl_ble_moduleDlg dlg;
	m_pMainWnd = (CWnd *)&dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
