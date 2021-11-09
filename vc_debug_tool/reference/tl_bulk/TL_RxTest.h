/********************************************************************************************************
 * @file     TL_RxTest.h 
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
#pragma once
#include "../../ble_lt_mesh/proj_lib/sig_mesh/App_mesh.h"
#include "afxwin.h"

// CTL_RxTest 对话框

class CTL_RxTest : public CDialog
{
	DECLARE_DYNAMIC(CTL_RxTest)

public:
	CTL_RxTest(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTL_RxTest();

// 对话框数据
	enum { IDD = IDD_RX_TEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
