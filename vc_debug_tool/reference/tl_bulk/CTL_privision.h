/********************************************************************************************************
 * @file     CTL_privision.h 
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
#if !defined(AFX_CTL_PRIVISION_H__4DB5BB1C_112D_4478_AF05_E137ABFCA738__INCLUDED_)
#define AFX_CTL_PRIVISION_H__4DB5BB1C_112D_4478_AF05_E137ABFCA738__INCLUDED_
#include "./lib_file/gatt_provision.h"
#include "Sig_mesh_json_info.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CTL_privision.h : header file
//
extern int netidx ;
extern provison_net_info_str net_info;
extern int pro_self_flag ;
extern provision_appkey_t vc_dlg_appkey;
extern CString csAppKey;
extern CString csAppKeyIndex;			

typedef enum{
    PROV_DIRECT_MODE = 0,
    PROV_REMOTE_MODE,
}PROV_MODE_ENUM;

/////////////////////////////////////////////////////////////////////////////
// CTL_privision dialog

class CTL_privision : public CDialog
{
// Construction
public:
	CTL_privision(CWnd* pParent = NULL);   // standard constructor
	BOOL OnInitDialog();
	void SendCmd(u8 opcode);
	void SetFilterSts(u8 *p_dat,u8 len);
	BOOL SetButton_sts();
	void set_unicast_dlg_part();
	void update_ivi_edit(u8 *p_ivi);
	void set_keybind_window_enable(u8 en);
	void set_provision_window_enable(u8 en);
	void set_provision_dlg_part(u8 *data,int len ,CString *cs_edit,int id);
	void rp_dkri_para_init();
	u16 Getunicast_adr_in_edit();
	u16 Get_node_adr_in_edit();
	int GetProvision_para();
	LRESULT OnUpdateData(WPARAM wparam, LPARAM lparam);
// Dialog Data
	//{{AFX_DATA(CTL_privision)
	enum { IDD = IDD_PROVISION };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	void Restore_provision_data();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTL_privision)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTL_privision)
	afx_msg void OnProvisionStart();
	afx_msg void OnProDisconnect();
	afx_msg void OnStartSend();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnBnClickedButAddMac();
	afx_msg void OnBnClickedButRmMac();
	afx_msg void OnBnClickedButGetFilterSts();
	afx_msg void OnBnClickedButSetFilterType();
	afx_msg void OnBnClickedSetPro();
	afx_msg void OnBnClickedAppkeyBindAll();
	afx_msg void OnBnClickedDkriProv();
	afx_msg void OnBnClickedCandiEn();
	BOOL m_fast_prov_mode;
};
u8 set_provision_dat(u8 *tmp_dat);
int App_key_bind_end_callback(u8 event);

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTL_PRIVISION_H__4DB5BB1C_112D_4478_AF05_E137ABFCA738__INCLUDED_)
