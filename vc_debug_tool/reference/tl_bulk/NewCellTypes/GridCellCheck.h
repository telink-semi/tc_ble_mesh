/********************************************************************************************************
 * @file	GridCellCheck.h
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
#if !defined(AFX_GRIDCELLCHECK_H__ECD42822_16DF_11D1_992F_895E185F9C72__INCLUDED_)
#define AFX_GRIDCELLCHECK_H__ECD42822_16DF_11D1_992F_895E185F9C72__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include "../GridCtrl_src/GridCell.h"


class CGridCellCheck : public CGridCell
{
    friend class CGridCtrl;
    DECLARE_DYNCREATE(CGridCellCheck)

public:
    CGridCellCheck();

public:
	BOOL SetCheck(BOOL bChecked = TRUE);
	BOOL GetCheck();

// Operations
	virtual CSize GetCellExtent(CDC* pDC);
    virtual void OnClick( CPoint PointCellRelative);
    virtual BOOL GetTextRect( LPRECT pRect);

protected:
	CRect GetCheckPlacement();

    virtual BOOL Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd = TRUE);

protected:
    BOOL  m_bChecked;
    CRect m_Rect;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRIDCELLCHECK_H__ECD42822_16DF_11D1_992F_895E185F9C72__INCLUDED_)
