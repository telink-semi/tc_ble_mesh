/********************************************************************************************************
 * @file	TL_RxTest.h
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
#pragma once
#include "../../ble_lt_mesh/proj_lib/sig_mesh/App_mesh.h"
#include "afxwin.h"

// CTL_RxTest message box

class CTL_RxTest : public CDialog
{
	DECLARE_DYNAMIC(CTL_RxTest)

public:
	CTL_RxTest(CWnd* pParent = NULL);   // Standard construction function
	virtual ~CTL_RxTest();

// data of dialog box
	enum { IDD = IDD_RX_TEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void StatusNotify (unsigned char *p, int len);

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnOK();
	afx_msg void OnBnClickedStart();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	unsigned int m_cur_cnt;
	UINT m_interval;
	UINT m_total_counts;
	CString m_csDstAddr;
	CString m_transmit;
	afx_msg void OnBnClickedTransmit();
	afx_msg void OnBnClickedGetResult();
	UINT m_RcvCnts;
	bool m_testing;
	int m_GetRevCnts;
	int m_total_nodes;
	int m_sel_mode;
	int m_max_time;
	int m_min_time;
	int m_average_time;
	BYTE m_pkts_send;
	BYTE m_pkts_receive;
	CString m_csAddrGet;
	int m_clear_result;
	afx_msg void OnBnClickedButtonClear();
};
