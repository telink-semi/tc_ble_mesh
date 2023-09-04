/********************************************************************************************************
 * @file	tl_ble_module.cpp
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
		AfxMessageBox(_T("Another sig_mesh_tool.exe may be running! Please close first!"));
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
