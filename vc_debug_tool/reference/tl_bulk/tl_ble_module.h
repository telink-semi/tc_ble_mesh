/********************************************************************************************************
 * @file     tl_ble_module.h 
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
// tl_ble_module.h : main header file for the TL_BLE_MODULE application
//

#if !defined(AFX_TL_BLE_MODULE_H__AA2CDC3D_91E8_4FE1_A8BE_30135F105D8A__INCLUDED_)
#define AFX_TL_BLE_MODULE_H__AA2CDC3D_91E8_4FE1_A8BE_30135F105D8A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTl_ble_moduleApp:
// See tl_ble_module.cpp for the implementation of this class
//

class CTl_ble_moduleApp : public CWinApp
{
public:
	CTl_ble_moduleApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTl_ble_moduleApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTl_ble_moduleApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#include "./lib_file/app_config.h"

#endif // !defined(AFX_TL_BLE_MODULE_H__AA2CDC3D_91E8_4FE1_A8BE_30135F105D8A__INCLUDED_)
