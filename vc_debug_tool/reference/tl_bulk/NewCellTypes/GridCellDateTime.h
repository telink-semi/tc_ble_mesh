/********************************************************************************************************
 * @file	GridCellDateTime.h
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
#if !defined(AFX_DATETIMECELL_H__A0B7DA0A_0AFE_4D28_A00E_846C96D7507A__INCLUDED_)
#define AFX_DATETIMECELL_H__A0B7DA0A_0AFE_4D28_A00E_846C96D7507A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../GridCtrl_src/GridCell.h"
#include "afxdtctl.h"	// for CDateTimeCtrl

class CGridCellDateTime : public CGridCell  
{
  friend class CGridCtrl;
  DECLARE_DYNCREATE(CGridCellDateTime)

  CTime m_cTime;
  DWORD m_dwStyle;

public:
	CGridCellDateTime();
	CGridCellDateTime(DWORD dwStyle);
	virtual ~CGridCellDateTime();
    virtual CSize GetCellExtent(CDC* pDC);

  // editing cells
public:
	void Init(DWORD dwStyle);
	virtual BOOL  Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
	virtual CWnd* GetEditWnd() const;
	virtual void  EndEdit();


	CTime* GetTime() {return &m_cTime;};
	void   SetTime(CTime time);
};

class CInPlaceDateTime : public CDateTimeCtrl
{
// Construction
public:
	CInPlaceDateTime(CWnd* pParent,         // parent
                   CRect& rect,           // dimensions & location
                   DWORD dwStyle,         // window/combobox style
                   UINT nID,              // control ID
                   int nRow, int nColumn, // row and column
                   COLORREF crFore, COLORREF crBack,  // Foreground, background colour
                   CTime* pcTime,
          		   UINT nFirstChar);      // first character to pass to control

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceList)
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInPlaceDateTime();
    void EndEdit();

// Generated message map functions
protected:
	//{{AFX_MSG(CInPlaceList)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG
	//afx_msg void OnSelendOK();

	DECLARE_MESSAGE_MAP()

private:
    CTime*   m_pcTime;
	int		 m_nRow;
	int		 m_nCol;
 	UINT     m_nLastChar; 
	BOOL	 m_bExitOnArrows; 
    COLORREF m_crForeClr, m_crBackClr;
};

#endif // !defined(AFX_DATETIMECELL_H__A0B7DA0A_0AFE_4D28_A00E_846C96D7507A__INCLUDED_)
