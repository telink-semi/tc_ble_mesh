/********************************************************************************************************
 * @file     ScanDlg.h 
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
#if !defined(AFX_SCANDLG_H__459B6637_BEE5_457A_973D_D1DEFD32026D__INCLUDED_)
#define AFX_SCANDLG_H__459B6637_BEE5_457A_973D_D1DEFD32026D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScanDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CScanDlg dialog

class CScanDlg : public CDialog
{
// Construction
public:
	CScanDlg(CWnd* pParent = NULL);   // standard constructor
	char m_mac[8];
	void AddMac (unsigned char * pmac, int rssi);
	void AddDevice (unsigned char * p, int n);
	char amac[1024*64];
	int	 nmac;

// Dialog Data
	//{{AFX_DATA(CScanDlg)
	enum { IDD = IDD_TL_SCAN_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScanDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CScanDlg)
	afx_msg void OnConnect();
	afx_msg void OnDblclkScanlist();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	void OnDestroyReally();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
#define MAX_MAC_LIST_NUM	2048
typedef struct{
	u32 max_idx;
	u8 mac[MAX_MAC_LIST_NUM][6];
	u8 uuid[MAX_MAC_LIST_NUM][16];
	u16 oob[MAX_MAC_LIST_NUM];
}gatt_mac_list_str;

extern unsigned short serv_uuid_dat;
extern gatt_mac_list_str gatt_mac_list;
int find_idx_by_mac(u8 *p_mac);
extern unsigned char  sigmesh_node_uuid[16];
extern unsigned char  sigmesh_node_mac[6];
extern u8  certify_base_en ;


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#include "./lib_file/app_config.h"

#endif // !defined(AFX_SCANDLG_H__459B6637_BEE5_457A_973D_D1DEFD32026D__INCLUDED_)
