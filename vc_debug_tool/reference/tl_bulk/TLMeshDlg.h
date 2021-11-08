/********************************************************************************************************
 * @file     TLMeshDlg.h 
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
#if !defined(AFX_TLMESHDLG1_H__1FD58D80_A799_45FE_BA3A_0AC63C133B93__INCLUDED_)
#define AFX_TLMESHDLG1_H__1FD58D80_A799_45FE_BA3A_0AC63C133B93__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TLMeshDlg.h : header file
//
#include "GridCtrl_src/GridCtrl.h"
#include "../../ble_lt_mesh/proj_lib/mesh_crypto/aes_att.h"
#include "../../ble_lt_mesh/proj_lib/sig_mesh/app_mesh.h"

/////////////////////////////////////////////////////////////////////////////
// CTLMeshDlg dialog

typedef struct{
	u16 adr;
	s16 level;
	u8 onoff;
	u8 online_sn;
	u8 online_ct;
	u8 rfu[1];
}mesh_status_t;

class CTLMeshDlg : public CDialog
{
// Construction
public:
	CTLMeshDlg(CWnd* pParent = NULL);   // standard constructor


	CWnd* m_pParent;
	int m_nID;

	void StatusNotify (unsigned char *p, int len);
	void UpdateNode (mesh_rc_rsp_t *rsp, u16 op, u32 size_op);
	void UpdateNodeByOnlineST (unsigned char *p);
	void UpdateGroupList (mesh_rc_rsp_t *rsp, u16 op, u32 size_op);
	void UpdateGroupSetRsp (mesh_rc_rsp_t *rsp, u16 op, u32 size_op);
	void cfg_cmd_pub_status_handle (mesh_rc_rsp_t *rsp, u16 op, u32 size_op);
	void UpdateOnlineStatus();
	void InitOnlineStatusOffline ();
	u32 GetOnlineStatusRspMax ();
	void UpdateGroupStatus ();
	void UpdateSheduleStatus ();
	void UpdateSceneStatus ();
	void SendOpPara (u8 *cmd, int par_len);
	void InitStatus ();
	void InitStatusSelNode();
	void InitCfgPar ();
	int Sel_Ele_Check();
	void PubAddrUpdate(u16 adr);
	void update_mesh_sts();
	int reliable_rsp_check(u16 *missing_addr, u16 max_num);
	u32 get_all_online_node(u16 *list, u32 max_cnt);
	mesh_status_t mesh_status[MESH_NODE_MAX_NUM];	
	int	mesh_num;
	int	mesh_sel;
	int	mLite;
	int current_shedule;
	int current_scene;
	int shedule_init;
	time_status_t UI_time;
	void setcheckmonth();
	void setcheckweek();
	void get_time_shedule_proc();
	scheduler_t get_current_scheduler();
	void ShowSchedule(int shedule_index);
	void refresh_time_ui();
	enum { IDD = IDD_TL_MESH_DLG };
	CString	m_Light;
	CGridCtrl m_Grid, m_GridGroup;//, m_GridShedule, m_GridScene;

	void SetItem(int group, int row, int col, int type, char *text, int image);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTLMeshDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	CImageList m_ImageList;

	// Generated message map functions
	//{{AFX_MSG(CTLMeshDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnNode();
	afx_msg int OnButtonGetSubscription(int model_id);
	afx_msg void OnButtonGetgroup();
	//}}AFX_MSG

    afx_msg void OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult);
    afx_msg void OnGridClick(NMHDR *pNotifyStruct, LRESULT* pResult);
    afx_msg void OnGridRClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridSheduleClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridSceneClick(NMHDR *pNotifyStruct, LRESULT* pResult);
    afx_msg void OnGridGroupDblClick(NMHDR *pNotifyStruct, LRESULT* pResult);
    afx_msg void OnGridGroupClick(NMHDR *pNotifyStruct, LRESULT* pResult);
    afx_msg void OnGridGroupRClick(NMHDR *pNotifyStruct, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSetMac();
//	afx_msg void OnBnClickedReliable();
//	BOOL reliable;
	int m_reliable;
	afx_msg void OnBnClickedReliable();
	afx_msg void OnBnClickedGroup2();
	CString m_pub_addr;
	afx_msg void OnBnClickedGetPub();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedGetSecNwBc();
	CString m_sec_nw_bc;
	CString m_ttl;
	CString m_transimit;
	afx_msg void OnBnClickedGetttl();
	afx_msg void OnBnClickedGetTransimit();
	afx_msg void OnBnClickedGetRelay();
	CString m_relay_val;
	CString m_friend_val;
	CString m_light_val;
	CString m_ct_val;
	afx_msg void OnBnClickedGetFriend();
	CString m_proxy_val;
	afx_msg void OnBnClickedGetProxy();
	afx_msg void OnBnClickedGetCps();
	afx_msg void OnBnClickedDelNode();
	afx_msg void OnBnClickedGrpdelallS();
	afx_msg void OnBnClickedGrpdelallC();
	afx_msg void OnBnClickedGetLevel();
	afx_msg void OnBnClickedGetCt();
//	CSliderCtrl m_slider_bn;
//	CSliderCtrl m_slider_b;
//	CSliderCtrl m_slider_g;
//	CSliderCtrl m_slider_r;
	int m_radio_on;
	//BOOL m_all;
//	int m_b;
//	int m_bn;
//	int m_g;
//	int m_r;
	BOOL m_friday;
	CGridCtrl m_GridScene;
	CGridCtrl m_GridShedule;
	BOOL m_monday;
	BOOL m_saturday;
	BOOL m_sunday;
	BOOL m_thursday;
	BOOL m_tuesday;
	BOOL m_wednesday;
	int m_yearsel;
	afx_msg void OnClickedAnyYear();
	int m_daysel;
	afx_msg void OnClickedAnyDay();
	int m_hoursel;
	int m_minutesel;
	int m_secondsel;
	afx_msg void OnClickedAnyHour();
	afx_msg void OnClickedAnyMinute();
	afx_msg void OnClickedAnySecond();
	afx_msg void OnClickedOn();
	BOOL m_April;
	BOOL m_August;
	BOOL m_December;
	BOOL m_February;
	int m_hour;
	BOOL m_January;
	BOOL m_July;
	BOOL m_June;
	BOOL m_March;
	BOOL m_May;
	int m_minute;
	BOOL m_November;
	BOOL m_October;
	int m_seconed;
	BOOL m_September;
	int m_year;
	int m_day;
	int m_recall;
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedSceneDel();
	afx_msg void OnBnClickedSceneRecall();
	afx_msg void OnBnClickedSceneAdd();
	afx_msg void OnBnClickedSetTime();
	afx_msg void OnBnClickedGetTime();
	int m_scene_number;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnEnKillfocusYear();
	afx_msg void OnEnKillfocusEditDay();
	afx_msg void OnEnKillfocusHour();
	afx_msg void OnEnKillfocusMinute();
	afx_msg void OnEnKillfocusSecond();
	afx_msg void OnEnKillfocusEditRecall();
	afx_msg void OnEnKillfocusscenenumber();
	CComboBox m_online_box;
	afx_msg void OnSelchangeOnlineSel();
	afx_msg void OnClose();
};

enum{
    RSP_TYPE_UNRELIABLE  = 0,
    RSP_TYPE_RELIABLE,
    RSP_TYPE_ONLINE_ST,
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#include "./lib_file/app_config.h"
#include "afxcmn.h"
#include "afxdtctl.h"
#include "gridctrl_src\gridctrl.h"
#include "gridctrl_src\gridctrl.h"
#include "afxwin.h"
void RefreshStatusNotifyByHw(unsigned char *p, int len);

#define TEST_RELAY_BUFF_EN      0
void relay_poll_10ms_test();
int IsMasterMode();
void schedule_get_proc_tick_check_and_clear();
void schedule_get_proc();

extern CTLMeshDlg * m_pMeshDlg;

enum{
    AUTO_GET_ST_IDLE   = 0,
    AUTO_GET_ST_GROUP,
    AUTO_GET_ST_WAIT_GROUP,
    AUTO_GET_ST_SCENE,
    AUTO_GET_ST_WAIT_SCENE,
    AUTO_GET_ST_TIME,
    AUTO_GET_ST_WAIT_TIME,
    AUTO_GET_ST_SCHEDULER,
    AUTO_GET_ST_WAIT_SCHEDULER,
};

#endif // !defined(AFX_TLMESHDLG1_H__1FD58D80_A799_45FE_BA3A_0AC63C133B93__INCLUDED_)
