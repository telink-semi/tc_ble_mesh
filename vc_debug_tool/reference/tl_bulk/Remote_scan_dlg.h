/********************************************************************************************************
 * @file	Remote_scan_dlg.h
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


// CRemoteScanPar 对话框

class CRemoteScanPar : public CDialog
{
	DECLARE_DYNAMIC(CRemoteScanPar)

public:
	CRemoteScanPar(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CRemoteScanPar();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RP_SCAN_CAP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	short scan_num;
	int timeout;
	afx_msg void OnBnClickedGetScanCap();
	void StatusNotify(unsigned char* p, int len);
};
