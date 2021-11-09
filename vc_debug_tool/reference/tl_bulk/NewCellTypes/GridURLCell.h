/********************************************************************************************************
 * @file     GridURLCell.h 
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

#if !defined(AFX_GRIDURLCELL_H__9F4A50B4_D773_11D3_A439_F7E60631F563__INCLUDED_)
#define AFX_GRIDURLCELL_H__9F4A50B4_D773_11D3_A439_F7E60631F563__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../GridCtrl_src/GridCell.h"

typedef struct {
    LPCTSTR szURLPrefix;
    size_t  nLength;
} URLStruct;



class CGridURLCell : public CGridCell  
{
    DECLARE_DYNCREATE(CGridURLCell)

public:
	CGridURLCell();
	virtual ~CGridURLCell();

    virtual BOOL     Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd = TRUE);
    virtual BOOL     Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
    virtual LPCTSTR  GetTipText() { return NULL; }
	void SetAutoLaunchUrl(BOOL bLaunch = TRUE) { m_bLaunchUrl = bLaunch;	}
	BOOL GetAutoLaunchUrl() { return m_bLaunchUrl;	}

protected:
    virtual BOOL OnSetCursor();
    virtual void OnClick(CPoint PointCellRelative);

	BOOL HasUrl(CString str);
    BOOL OverURL(CPoint& pt, CString& strURL);

protected:
#ifndef _WIN32_WCE
    static HCURSOR g_hLinkCursor;		// Hyperlink mouse cursor
	HCURSOR GetHandCursor();
#endif
    static URLStruct g_szURIprefixes[];

protected:
	COLORREF m_clrUrl;
	BOOL     m_bLaunchUrl;
    CRect    m_Rect;
};

#endif // !defined(AFX_GRIDURLCELL_H__9F4A50B4_D773_11D3_A439_F7E60631F563__INCLUDED_)
