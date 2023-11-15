/********************************************************************************************************
 * @file	tl_ble_moduleDlg.h
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
// tl_ble_moduleDlg.h : header file
//


#if !defined(AFX_TL_BLE_MODULEDLG_H__882B3038_F6FB_43FD_9579_899D142C510B__INCLUDED_)
#define AFX_TL_BLE_MODULEDLG_H__882B3038_F6FB_43FD_9579_899D142C510B__INCLUDED_
#include "SerialPort.h"
#include "./lib_file/app_config.h"
#include "../../ble_lt_mesh/vendor/common/lighting_model.h"
#include "../../ble_lt_mesh/vendor/common/lighting_model_HSL.h"
#include "../../ble_lt_mesh/vendor/common/lighting_model_xyL.h"
#include "../../ble_lt_mesh/vendor/common/lighting_model_LC.h"
#include "../../ble_lt_mesh/vendor/common/scene.h"
#include "../../ble_lt_mesh/vendor/common/mesh_ota.h"
#include "../../ble_lt_mesh/vendor/common/remote_prov.h"
#include "vc_mesh_func.h"

#include "TLMeshDlg.h"
#include "TL_RxTest.h"
#include "CTL_provision.h"
#include "ScanDlg.h"

#include "afxmt.h"

#include <vector>


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
extern u8 gateway_provision_sts ;
extern CSerialPort* serial_port_ptr;
extern u8 vc_node_ele_cnt ;
extern int m_status;
/////////////////////////////////////////////////////////////////////////////
// CTl_ble_moduleDlg dialog
class CTL_provision;
class CScanDlg;
class CTLMeshDlg;
class CTL_RxTest;

#include "afxmt.h"
#include <queue>
#include <memory>
#include "afxwin.h"
using namespace std;

extern CString		m_InitFile;

class CTl_ble_moduleDlg : public CDialog
{
// Construction
public:	
	// 增加串口功能
	CTl_ble_moduleDlg::~CTl_ble_moduleDlg();
	CComboBox *p_ComList;
	void 	FunctionCommon(unsigned char event);
	CTl_ble_moduleDlg(CWnd* pParent = NULL);	// standard constructor
	afx_msg LRESULT OnAppendLogHandle (WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnAppendLog (WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnUpdateData (WPARAM wParam, LPARAM lParam );

	CString		m_device_vidpid;
	CString		 m_InitStr, m_ExeDir, m_AppCmd, m_CurrentDir;
	void	CTl_ble_module_Init ();
	void 	log_msg_buf_init();
	void 	load_ini_init();
	void 	select_ini_auto();
	void 	adv_chn_init_settings();
	void 	class_pointer_init();
	void 	mesh_retry_cnt_init();
	void 	kma_dongle_init_log();
	int		LoadIniFile ();
	void	DoCmd(LPCTSTR cmd, LPSTR cmdline, LPCTSTR curdir, int nownd);
	int		ExecCmd(LPCTSTR cmd, int log_type);
	void	ExecStrCmd(char * cmd);
	void	ExecOpPara(unsigned char * p, int n);
	int		LogMsg (LPSTR sz,...);
	int		LogMsgTime ();
	void    OnLogDisable();
	int		m_nTextLen;
	void	SetHostInterface();
	void    FillTxPar(char *cmd);
	int		m_i2c_id;
	unsigned char pro_cmd_printing(unsigned char dat);
	int format_parse (unsigned char *ps, int len, unsigned char *pd);
	BOOL PreTranslateMessage(MSG* pMsg) ;
	void set_window_text(CString text);

	int		mLite;
// Dialog Data
	//{{AFX_DATA(CTl_ble_moduleDlg)
	enum { IDD = IDD_TL_BLE_MODULE_DIALOG };
	CEdit	m_textBO;
	CEdit	m_Log;
	int		m_conn_interval;
	int		m_conn_timeout;
	CString	m_conn_chnmask;
	BOOL	m_bLogEn;
	BOOL    m_fastbind;
	BOOL	m_bHex;
	BOOL    m_adv_mode;
	CString	m_cmd_input;
	//}}AFX_DATA
	CToolTipCtrl m_tooltip;

#define TIMER_APPEND_LOG      3
	CRITICAL_SECTION log_queue_cs;
	queue<shared_ptr<char>> log_queue;
	void append_log();

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTl_ble_moduleDlg)
	protected:
	virtual void OnOK();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	CToolTipCtrl m_tip;

	// Generated message map functions
	//{{AFX_MSG(CTl_ble_moduleDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelchangeInifile();
	afx_msg void OnSelchangeCommand();
	afx_msg void OnDblclkCommand();
	afx_msg void OnOpen();
	afx_msg void OnClosedlg();
	afx_msg void OnLogclear();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnScan();
	afx_msg void OnStop();
	afx_msg void OnLogsave();
	afx_msg void OnLogsavefile();
	afx_msg void OnLogen();
	afx_msg void OnBulkout();
	afx_msg void OnCmdsel();
	afx_msg void OnDevice();
	afx_msg void OnProvision();
	afx_msg void OnScanStop();
	afx_msg void OnMeshDisplay();
	//}}AFX_MSG
	afx_msg void sig_mesh_user_init();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOta();
	afx_msg void OnBnClickedRxTest();
	afx_msg void OnBnClickedSetRetry();
	afx_msg void OnBnClickedUartMode();
	afx_msg void CTl_ble_moduleDlg::OnBnClickedUsbMode();
	afx_msg void OnBnClickedUartConn();
	afx_msg void OnBnClickedGatewayOta();
	afx_msg void OnBnClickedMeshOta();
	afx_msg void OnBnClickedGatewatReset();
	afx_msg void OnBnClickedSearchfile();
	afx_msg void OnBnClickedRpScan();
	afx_msg void OnBnClickedChnSet();

	afx_msg void OnBnClickedInputDb();
	afx_msg void OnBnClickedOutputDb();
	BOOL m_use_directed;
	afx_msg void OnBnClickedUseDirected();
	afx_msg void OnClose();
	afx_msg void OnBnClickedextendscan();
    void mesh_rx_test();
    BOOL m_log_backup;
    BOOL m_json_backup;
	INT m_ExtendAdvOption;

    CTLMeshDlg m_MeshDlg;
    CTL_provision m_ProDlg;
    CTL_RxTest m_RxTestDlg;
	CScanDlg m_ScanDlg;

    typedef enum {
        THREAD_RUN,
        THREAD_EXIT,
        THREAD_EXITED,
    } thread_status_t;
    volatile thread_status_t bulk_thread_status;
    volatile thread_status_t main_thread_status;

    CWinThread *bulk_thread_handle;
    CWinThread *main_thread_handle;
	afx_msg void OnBnClickedFastbind();
	afx_msg void OnBnClickedCheckAutoSaveLog();
	CComboBox m_combo_extend_adv_option;
	int SaveExtendAdvOption(int Option_index);
	afx_msg void OnCbnSelchangeComboExtendAdvOption();
};
extern unsigned char SendPkt(unsigned short handle,unsigned char *p ,unsigned char  len);
unsigned char SendPktAttOp(unsigned char att_op,unsigned short handle,unsigned char *p ,unsigned char  len);
unsigned char SendPktOnlineStSet(unsigned char *p ,unsigned char  len);
unsigned char SendPktOnlineStOn();

extern unsigned char connect_flag;
extern unsigned char show_disconnect_box;
extern void clear_all_white_list();
extern void disconnect_device();
extern void set_special_white_list(unsigned char  *pmac);
extern void report_adv_opera(unsigned char dat);
extern void set_adv_chn_device(u8 para);
extern void update_online_st_pkt(u8 *p, int len);

extern CTl_ble_moduleDlg *  g_module_dlg;
#define INPUT_LOG_AND 0x80
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
extern void	 provision_flow_Log(unsigned char cmd ,unsigned char *p,unsigned int wLen);
extern int Bin2Text (LPBYTE lpD, LPBYTE lpS, int n);
extern int Text2Bin(LPBYTE lpD, LPBYTE lpS);
extern int bin2text_normal(LPBYTE lpD, LPBYTE lpS, int n);
extern unsigned char vc_uuid[16];
extern unsigned char vc_dev_key[16];

extern unsigned char gw_mac[6];
extern unsigned char gw_dev_uuid[16];
extern unsigned char gw_dev_mac[6];
extern unsigned char gw_dev_key[16];
extern u16 gw_dev_unicast;

extern void WriteFile_handle(u8 *buff, int n, u8 *head, u8 head_len);
extern HANDLE	m_hDevW ;
extern void set_provision_out_ccc();
extern HANDLE NULLEVENT2;
extern bool	connect_serial_port;
#define MAX_MAC_INDEX	0x100

#define VC_GATEWAY_USB_ID 0x08d4
#define VC_DONGLE_USB_ID 0x08d0

typedef struct{
	unsigned char index;
	unsigned char mac[MAX_MAC_INDEX*6];
	unsigned char mac_tmp[6];
	unsigned char uuid[16];
}provision_mac_str_t;
extern provision_mac_str_t mac_str;
#define TEST_ENABLE 	0
extern CTLMeshDlg * m_pMeshDlg;
extern CTL_provision *  m_proDlg;
extern CTL_RxTest *  m_pRxTestDlg;
extern unsigned char connect_flag_debug_mesh;
extern CString ota_filename;
int LogMsgModuleDlg_and_buf(u8 *pbuf,int len,char *log_str,char *format, va_list list);
int get_auth_value_by_uuid(u8 *uuid_in,u8 *oob_out);

// gateway fun
void gateway_VC_provision_start();
void gateway_VC_provision_stop();
unsigned char ble_module_id_is_gateway();
void gateway_vc_set_adv_filter(unsigned char  *p_mac);
void gateway_set_provisioner_para(unsigned char *p_net_info);
void gateway_set_node_provision_para(unsigned char *p_net_info);
void gateway_set_start_keybind(unsigned char fastbind, unsigned char *p_appid,unsigned char *p_key);
void gateway_get_provision_self_sts();
void gateway_start_fast_provision(u16 pid, u16 start_addr);

void write_file_uart(u8 *p_data,int len );
int read_file_uart(u8 *p_data,int len );
void gateway_send_ota_type(u8 type);
void gateway_send_ota_erase_ctl();
int gatt_write_transaction_callback(u8 *p,u16 len,u8 msg_type);
void write_no_rsps_pkts(u8 *p,u16 len,u16 handle,u8 msg_type);

void set_ini_unicast_max(u16 adr);
u16 get_ini_unicast_max();

#define MAX_REMOTE_PROV_NODE_CNT    200
typedef struct{
    u8 valid;
    u16 src;
    u8 mac[6];
    u8 uuid[16];
    s8 rssi;
}remote_prov_uuid_str;
typedef struct{
    int prov_mode;
    remote_prov_uuid_str uuid[MAX_REMOTE_PROV_NODE_CNT];
}remote_prov_cli_str;
typedef struct{
	u16 flag;
	mesh_bulk_cmd_par_t cmd;
}mesh_bulk_ini_vc_t;

extern remote_prov_cli_str rp_cli_win32;
extern unsigned char filter_send_flag;

int prov_mode_is_rp_mode();
void set_node_prov_mode(int mode);
int remote_prov_find_uuid_idx(remote_prov_scan_report *p_report);
int remote_prov_proc_client_scan_report(remote_prov_scan_report_win32 *p_reportwin32);
u16 remote_prov_client_find_adr_by_uuid(u8 *p_uuid_rep);
unsigned char Dongle_is_gateway_or_not() ;
int ota_file_check();
void gateway_get_dev_uuid_mac();
void gateway_get_sno();
void gateway_set_sno(u32 sno);
void gateway_set_extend_seg_mode(u8 mode);
void gateway_send_cmd_to_del_node_info(u16 uni);
void vc_get_dev_key(u8 *p_devkey);
void gateway_set_prov_devkey(u16 unicast,u8 *p_devkey);
unsigned char ble_module_id_is_kmadongle();
void    gw_json_update(int node_idx);
u8 unicast_is_in_range_or_not(u16 unicast,u8* p_uuid);
void read_version_rsp();
void gateway_set_rp_mode(u8 en);
void gateway_start_scan_start();
void gateway_send_link_open(u16 adr,u8 *p_uuid);
void gateway_send_rp_start(u8*p_net_info);
void gateway_update_netkey_by_json(u8 *p_netkey);
void set_ini_import_json_flag(int import_flag);
int get_ini_import_json_flag();

void set_ini_provioner_tx_cmd_sno(u32 sno);
void set_ini_provioner_mesh_uuid(u8* p_mesh_uuid);
u32 get_ini_provioner_mesh_uuid(u8 *p_mesh_uuid_out);
int is_match_ini_uuid(u8 *uuid_json);
int ini_get_and_writeback_tx_cmd_sno(u8* p_mesh_uuid);





#endif // !defined(AFX_TL_BLE_MODULEDLG_H__882B3038_F6FB_43FD_9579_899D142C510B__INCLUDED_)
