/********************************************************************************************************
 * @file     GridDropTarget.h 
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

#if !defined(AFX_GRIDDROPTARGET_H__5C610981_BD36_11D1_97CD_00A0243D1382__INCLUDED_)
#define AFX_GRIDDROPTARGET_H__5C610981_BD36_11D1_97CD_00A0243D1382__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include <afxole.h>

class CGridCtrl;

/////////////////////////////////////////////////////////////////////////////
// CGridDropTarget command target

class CGridDropTarget : public COleDropTarget
{
public:
    CGridDropTarget();
    virtual ~CGridDropTarget();

// Attributes
public:
    CGridCtrl* m_pGridCtrl;
    BOOL       m_bRegistered;

// Operations
public:
    BOOL Register(CGridCtrl *pGridCtrl);
    virtual void Revoke();

    BOOL        OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
    DROPEFFECT  OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
    void        OnDragLeave(CWnd* pWnd);
    DROPEFFECT  OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
    DROPEFFECT  OnDragScroll(CWnd* pWnd, DWORD dwKeyState, CPoint point);

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CGridDropTarget)
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CGridDropTarget)
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRIDDROPTARGET_H__5C610981_BD36_11D1_97CD_00A0243D1382__INCLUDED_)
