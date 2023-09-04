/********************************************************************************************************
 * @file	tl_ble_moduleDlg.cpp
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
// tl_ble_moduleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "afxmt.h"
#include "tl_ble_module.h"
#include "ScanDlg.h"
#include "tl_ble_moduleDlg.h"
#include "TLMeshDlg.h"
#include "usbprt.h"
#include "./lib_file/gatt_provision.h"
#include "./lib_file/hw_fun.h"
#include "./lib_file/host_fifo.h"
#include "CTL_privision.h"
#include "TL_RxTest.h"
#include <stdio.h>
#include "../../ble_lt_mesh/vendor/common/app_provison.h"
#include "../../ble_lt_mesh/vendor/common/app_privacy_beacon.h"
#include "../../ble_lt_mesh/vendor/common/app_heartbeat.h"
#include "../../ble_lt_mesh/vendor/common/fast_provision_model.h"
#include "../../ble_lt_mesh/proj_lib/ble/service/ble_ll_ota.h"
#include "../../ble_lt_mesh/proj_lib/mesh_crypto/sha256_telink.h"
#include "rapid_json_interface.h"
#include "sig_mesh_json_info.h"
#include "stdio.h"
#include "strsafe.h"
#include <memory>

using namespace std;
CString		m_InitFile;

#if _DEBUG
#pragma comment(lib,"tl_bulk_libdebug.lib")
#pragma comment(lib,"tl_bulk_lib_draftdebug.lib")
#else
#pragma comment(lib,"tl_bulk_librelease.lib")
#pragma comment(lib,"tl_bulk_lib_draftrelease.lib")
#endif
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
CTLMeshDlg * m_pMeshDlg;
CTL_privision *  m_proDlg;
CTL_RxTest * m_pRxTestDlg;
CScanDlg	*m_pScanDlg;

unsigned char 	m_retry_cnt;
bool	connect_serial_port;
CSerialPort* serial_port_ptr = nullptr;

int		m_status;
int     adr_vc_adr_max;
unsigned char connect_flag=0;
unsigned char filter_send_flag =0;
unsigned short connect_addr_gatt = 0;
unsigned char show_disconnect_box = 1;
unsigned char connect_flag_debug_mesh=0;
unsigned char adv_enable =1;
int disable_log_cmd = 0;
CTl_ble_moduleDlg*  g_module_dlg=NULL;
provision_mac_str_t mac_str;
u8 mesh_lpn_rx_master_key = 0;

#if (DRAFT_FEATURE_VENDOR_TYPE_SEL != DRAFT_FEATURE_VENDOR_TYPE_NONE)
STATIC_ASSERT(0x0171 == VENDOR_ID);
#endif

#define WM_APPENDLOG (WM_APP + 104)
#define WM_UPDATEDATA (WM_APP + 105)

//#define	DONGLE_BLE_MODULE_ID		0x82bd dongle_ble_moudle_id

#define ID_KMA_DONGLE			(0x08D0)
#define ID_NODE					(0x08D2)
#define ID_GATEWAY_VC			0x08d4
int	dongle_ble_moudle_id =		0xffff;

int IsMasterMode()
{
	return (ID_KMA_DONGLE == dongle_ble_moudle_id);
}

volatile BOOL exit_thread = FALSE;

/////////////////////////////////////////////////////////////////////////////
// CTl_ble_moduleDlg dialog
CTl_ble_moduleDlg::~CTl_ble_moduleDlg()
{
	if(NULL != serial_port_ptr) {
		serial_port_ptr->Close232Port();
        delete serial_port_ptr;
    }
    exit_thread = TRUE;
    m_proDlg->DestroyWindow();

    m_pMeshDlg->DestroyWindow();

    m_pRxTestDlg->DestroyWindow();

	m_pScanDlg->OnDestroyReally();
}


CTl_ble_moduleDlg::CTl_ble_moduleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTl_ble_moduleDlg::IDD, pParent)
	, m_use_directed(FALSE)
{
	//{{AFX_DATA_INIT(CTl_ble_moduleDlg)
	m_conn_interval = 24;
	m_conn_timeout = 1000;
	m_retry_cnt = g_reliable_retry_cnt_def;
	m_conn_chnmask = _T("ff ff ff ff 1f");
	m_bLogEn = 1;
	m_bHex = 1;
	#if GATEWAY_300_NODES_TEST_ENABLE
	m_fastbind =1;
	#else
	m_fastbind =0;
	#endif
	m_cmd_input = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_status = 0;
}

void CTl_ble_moduleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTl_ble_moduleDlg)
	DDX_Control(pDX, IDC_TEXT_BO, m_textBO);
	DDX_Control(pDX, IDC_LOG, m_Log);
	DDX_Text(pDX, IDC_CONN_INTERVAL, m_conn_interval);	
	DDX_Text(pDX, IDC_CONN_TIMEOUT, m_conn_timeout);
	DDX_Text(pDX, IDC_CONN_CHNMASK, m_conn_chnmask);
	DDX_Check(pDX, IDC_LOGEN, m_bLogEn);
	DDX_Check(pDX, IDC_FASTBIND, m_fastbind);
	DDX_Check(pDX, IDC_HEX, m_bHex);
	DDX_Check(pDX, IDC_ADV_MODE, m_adv_mode);
	DDX_Text(pDX, IDC_CMDINPUT, m_cmd_input);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_USE_DIRECTED, m_use_directed);
	DDX_Control(pDX, IDC_COMBO_EXTEND_ADV_OPTION, m_combo_extend_adv_option);
}

BEGIN_MESSAGE_MAP(CTl_ble_moduleDlg, CDialog)
	//{{AFX_MSG_MAP(CTl_ble_moduleDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_INIFILE, OnSelchangeInifile)
	ON_LBN_SELCHANGE(IDC_COMMAND, OnSelchangeCommand)
	ON_LBN_DBLCLK(IDC_COMMAND, OnDblclkCommand)
	ON_BN_CLICKED(IDC_OPEN, OnOpen)
	ON_BN_CLICKED(IDC_CLOSEDLG, OnClosedlg)
	ON_BN_CLICKED(IDC_LOGCLEAR, OnLogclear)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SCAN, OnScan)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	ON_BN_CLICKED(IDC_LOGSAVE, OnLogsave)
	ON_BN_CLICKED(IDC_LOGSAVEFILE, OnLogsavefile)
	ON_BN_CLICKED(IDC_LOGEN, OnLogen)
	ON_BN_CLICKED(IDC_BULKOUT, OnBulkout)
	ON_BN_CLICKED(IDC_CMDSEL, OnCmdsel)
	ON_BN_CLICKED(IDC_Device, OnDevice)
	ON_BN_CLICKED(IDC_PROVISION, OnProvision)
	ON_BN_CLICKED(IDC_SCAN_STOP, OnScanStop)
	ON_BN_CLICKED(IDC_MESH_DISPLAY, OnMeshDisplay)
	//}}AFX_MSG_MAP

	ON_MESSAGE(WM_APPENDLOG, OnAppendLog)
	ON_MESSAGE(WM_UPDATEDATA, OnUpdateData)

	ON_BN_CLICKED(IDC_OTA, &CTl_ble_moduleDlg::OnBnClickedOta)
	ON_BN_CLICKED(IDC_RX_TEST, &CTl_ble_moduleDlg::OnBnClickedRxTest)
	ON_BN_CLICKED(IDC_SET_RETRY, &CTl_ble_moduleDlg::OnBnClickedSetRetry)
	ON_BN_CLICKED(IDC_UART_MODE, &CTl_ble_moduleDlg::OnBnClickedUartMode)
	ON_BN_CLICKED(IDC_USB_MODE, &CTl_ble_moduleDlg::OnBnClickedUsbMode)
	ON_BN_CLICKED(IDC_UART_CONN, &CTl_ble_moduleDlg::OnBnClickedUartConn)
	ON_BN_CLICKED(IDC_GATEWAY_OTA, &CTl_ble_moduleDlg::OnBnClickedGatewayOta)
	ON_BN_CLICKED(IDC_MESH_OTA, &CTl_ble_moduleDlg::OnBnClickedMeshOta)
	ON_BN_CLICKED(IDC_GATEWAT_RESET, &CTl_ble_moduleDlg::OnBnClickedGatewatReset)
	ON_BN_CLICKED(IDC_SEARCHFILE, &CTl_ble_moduleDlg::OnBnClickedSearchfile)
	ON_BN_CLICKED(IDC_RP_SCAN, &CTl_ble_moduleDlg::OnBnClickedRpScan)
	ON_BN_CLICKED(IDC_CHN_SET, &CTl_ble_moduleDlg::OnBnClickedChnSet)
	ON_BN_CLICKED(IDC_INPUT_DB, &CTl_ble_moduleDlg::OnBnClickedInputDb)
	ON_BN_CLICKED(IDC_OUTPUT_DB, &CTl_ble_moduleDlg::OnBnClickedOutputDb)
	ON_BN_CLICKED(IDC_USE_DIRECTED, &CTl_ble_moduleDlg::OnBnClickedUseDirected)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_extend_scan, &CTl_ble_moduleDlg::OnBnClickedextendscan)
	ON_BN_CLICKED(IDC_FASTBIND, &CTl_ble_moduleDlg::OnBnClickedFastbind)
	ON_BN_CLICKED(IDC_CHECK_AUTO_SAVE_LOG, &CTl_ble_moduleDlg::OnBnClickedCheckAutoSaveLog)
	ON_CBN_SELCHANGE(IDC_COMBO_EXTEND_ADV_OPTION, &CTl_ble_moduleDlg::OnCbnSelchangeComboExtendAdvOption)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
int		device_ok = 0;
HANDLE	m_hDev = NULL;
HANDLE	m_hDevW = NULL;

HANDLE NULLEVENT2 = CreateEvent( NULL, TRUE, FALSE, NULL ) ;
HANDLE EVENT_UPDATE_NODE_INFO = CreateEvent(NULL, TRUE, FALSE, _T("Event update node info"));

UINT ThreadBulkIn ( void* pParams )
{
	CTl_ble_moduleDlg *dlg = (CTl_ble_moduleDlg *)pParams;
	BYTE		buff[32*1024];
	DWORD		nB;
    while (dlg->bulk_thread_status == CTl_ble_moduleDlg::THREAD_RUN)    { 
		int bok;
		if (m_hDev != NULL||connect_serial_port) {
			if(connect_serial_port){
				bok = read_file_uart(buff,32*1024);
				nB = bok;
				//WaitForSingleObject (NULLEVENT2, 10);
			}else{
				bok = ReadFile(m_hDev, buff, 32*1024, &nB, NULL);
			}
			if (bok) {	// send to log window
				//::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_APPENDLOG, nB, (LPARAM)buff);
				//WaitForSingleObject (NULLEVENT2, REPORT_INTERVAL_ADV_MS - 1);
				if(nB > 0) {
					// after pull out master dongle, and the insert, the first packet is insert event, not a packet, and the length is 0.
					dlg->OnAppendLog(nB, (LPARAM)buff);
				}
			}
			else {
				m_hDev = NULL;
			}
		}
		else {
				WaitForSingleObject (NULLEVENT2, 200);
				
				m_hDev = GetPrintDeviceHandle(dongle_ble_moudle_id);
				m_hDevW = GetPrintDeviceHandle(dongle_ble_moudle_id);
		}
    }
    dlg->bulk_thread_status = CTl_ble_moduleDlg::THREAD_EXITED;
	return 1;
}


UINT ThreadMainLoop ( void* pParams )
{
    CTl_ble_moduleDlg *dlg = (CTl_ble_moduleDlg *)pParams;
	//BYTE		buff[256];
	//DWORD		nB;
    while(dlg->main_thread_status == CTl_ble_moduleDlg::THREAD_RUN){
        Thread_main_process();
        if(m_pMeshDlg->shedule_init){
		    m_pMeshDlg->get_time_shedule_proc();
		}
		
        schedule_get_proc();
        
        #if TEST_RELAY_BUFF_EN
        relay_poll_10ms_test();
        #endif
		WaitForSingleObject(NULLEVENT2, 1);
	}
    dlg->main_thread_status = CTl_ble_moduleDlg::THREAD_EXITED;
	return 1;
}

#define				MAX_FORMAT			256

int		format_buff[16*MAX_FORMAT];
CString	format_str[MAX_FORMAT];
int		format_num = 0;
/////////////////////////////////////////////////////////////////////////////
// CTl_ble_moduleDlg message handlers
void CTl_ble_moduleDlg::sig_mesh_user_init()
{
	//used to set a timer ,and use the
	SetTimer (2, TIME_BASE/1000, NULL);//reserve for the adv data send 
	APP_reset_vendor_id(VENDOR_ID);	// 0x0211 // 0x01A8 // set for mesh OTA
	mesh_init_all();
}

unsigned char vc_uuid[16];
unsigned char vc_dev_key[16];


unsigned char gw_mac[6];
unsigned char gw_dev_uuid[16];
unsigned char gw_dev_mac[6];// gateway will upload the device's mac address part 
unsigned char gw_dev_key[16];
u16 gw_dev_unicast;

mesh_tx_reliable_t vc_reliable;     // create for the proc of the gateway json proc part 

void CTl_ble_moduleDlg::load_ini_init()
{
	GetCurrentDirectory (512, m_CurrentDir.GetBuffer(512));
	m_CurrentDir.ReleaseBuffer();
	// load ini file
	WIN32_FIND_DATA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	CString strDir = m_CurrentDir + "\\*.ini";
	hFind = FindFirstFile(strDir, &ffd);
	int ns = 0;

	if (hFind != INVALID_HANDLE_VALUE) {
      
		((CComboBox *) GetDlgItem (IDC_INIFILE))->ResetContent();
		do	{
			if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ){
				//filesize.LowPart = ffd.nFileSizeLow;
				//filesize.HighPart = ffd.nFileSizeHigh;
				((CComboBox *) GetDlgItem (IDC_INIFILE))->AddString (ffd.cFileName);
				if (strcmp (ffd.cFileName, "wtcdb.ini") == 0) { 
					((CComboBox *) GetDlgItem (IDC_INIFILE))->SetCurSel(ns); 
				}
				ns++;
			}
		}	while (FindNextFile(hFind, &ffd) != 0);

		FindClose(hFind);
	}

}

void CTl_ble_moduleDlg::select_ini_auto()
{
	if(Dongle_is_gateway_or_not()){
		((CComboBox *) GetDlgItem (IDC_INIFILE))->SetCurSel (1);
	}else{
		((CComboBox *) GetDlgItem (IDC_INIFILE))->SetCurSel (0);
	}
	OnSelchangeInifile ();
}

void CTl_ble_moduleDlg::adv_chn_init_settings()
{
	((CComboBox*)GetDlgItem (IDC_CHANNEL))->SetCurSel (1);
	((CComboBox*)GetDlgItem(IDC_COMBO_ADV_CHN))->AddString("37");
	((CComboBox*)GetDlgItem(IDC_COMBO_ADV_CHN))->AddString("38");
	((CComboBox*)GetDlgItem(IDC_COMBO_ADV_CHN))->AddString("39");
	((CComboBox*)GetDlgItem(IDC_COMBO_ADV_CHN))->AddString("ALL");
	((CComboBox*)GetDlgItem(IDC_COMBO_ADV_CHN))->SetCurSel(3);
}

void CTl_ble_moduleDlg::log_msg_buf_init()
{
	InitializeCriticalSection(&log_queue_cs);
	m_nTextLen = 0x100000;
	m_Log.SetLimitText (m_nTextLen);
}

void CTl_ble_moduleDlg::class_pointer_init()
{
	g_module_dlg = this;    // because it will used print function in OnSelchangeInifile_()
    m_proDlg = &m_ProDlg;
//	m_proDlg = new CTL_privision (NULL);
	m_proDlg->Create(IDD_PROVISION);
    
    m_pMeshDlg = &m_MeshDlg;
//	m_pMeshDlg = new CTLMeshDlg (NULL);
	m_pMeshDlg->Create(IDD_TL_MESH_DLG);

    m_pRxTestDlg = &m_RxTestDlg;
//	m_pRxTestDlg = new CTL_RxTest(NULL);
	m_pRxTestDlg->Create(IDD_RX_TEST);
	m_pRxTestDlg->GetDlgItem(IDC_RESULT)->EnableWindow(FALSE);

	m_pScanDlg = &m_ScanDlg;
	m_pScanDlg->Create(IDD_TL_SCAN_DLG);
}

void CTl_ble_moduleDlg::mesh_retry_cnt_init()
{
	CString str;
	m_retry_cnt =2;// init the mesh retry cnt for the stack part 
    str.Format(_T("%d"),m_retry_cnt);
	((CEdit *)GetDlgItem(IDC_EDIT_RETRY))->SetWindowText(str);	
}

void CTl_ble_moduleDlg::kma_dongle_init_log()
{
	if(((CComboBox *) GetDlgItem (IDC_INIFILE))->GetCurSel ()==0){ //  kma dongle select 
		LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "SCAN STOP for proxy mode when init:");
		OnScanStop();
	}
}


UINT ThreadGatewayJsonUpdate ( void* pParams );
UINT ThreadUpdateGatewayNodesInfo ( void* pParams );

void gateway_json_init()
{
	LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "gateway json init start ");
	if(init_json(FILE_MESH_DATA_BASE,0)){
		int node_idx = provision_find_json_node_by_uuid(vc_uuid);
		LOG_MSG_INFO (TL_LOG_COMMON, vc_uuid, 16, "node_idx=%d, vc_uuid=", node_idx);
		// init the mesh_key info
		mesh_key_retrieve();
		if( prov_get_net_info_from_json(&net_info,&netidx,node_idx)){
			if(node_idx == -1){
				AfxBeginThread(ThreadGatewayJsonUpdate, NULL);
			}
			else{
				update_VC_info_from_json(netidx ,VC_node_info);// update VC_node_info
				//AfxBeginThread(ThreadUpdateGatewayNodesInfo, NULL);
			}
		}
		pro_self_flag =1;
		net_info.unicast_address = json_get_next_unicast_adr_val(vc_uuid);
		if( prov_get_net_info_from_json(&net_info,&netidx,node_idx)){
			if(get_app_key_idx_by_net_idx(net_info.key_index,&vc_dlg_appkey))
			{
				u8 tmp[128], result[128];
				sprintf_s ((char *)tmp, sizeof(tmp), "%s", (const char *)csAppKeyIndex);
				Text2Bin(result, tmp);
				vc_dlg_appkey.apk_idx = result[0] + (result[1]<<8);
				sprintf_s ((char *)tmp, sizeof(tmp), "%s", (const char*)csAppKey);
				Text2Bin(vc_dlg_appkey.app_key, tmp);	
			}
		}
	}else{
		pro_self_flag =0;
	}
	

}

void vc_json_init()
{
	vc_get_guid(vc_uuid);
	vc_get_dev_key(vc_dev_key);
	if(init_json(FILE_MESH_DATA_BASE,0)){
		int node_idx = provision_find_json_node_by_uuid(vc_uuid);
    	if( prov_get_net_info_from_json(&net_info,&netidx,node_idx)){
			if(FAILURE == get_app_key_idx_by_net_idx(net_info.key_index,&vc_dlg_appkey))
			{
				u8 tmp[128], result[128];
				sprintf_s ((char *)tmp, sizeof(tmp), "%s", (const char*)csAppKeyIndex);
				Text2Bin(result, tmp);
				vc_dlg_appkey.apk_idx = result[0] + (result[1]<<8);
				sprintf_s ((char *)tmp, sizeof(tmp), "%s", (const char*)csAppKey);
				Text2Bin(vc_dlg_appkey.app_key, tmp);	
			}
    	    if(node_idx == -1){// need provision itself
               	mesh_provision_par_set_dir((u8*)&net_info);

    		   	update_VC_info_from_json(netidx ,VC_node_info);
    			provision_self_to_json(&net_info,&netidx,vc_uuid,vc_dev_key);
				// set the ram part of the dev key 
				set_dev_key(vc_dev_key);
            	json_add_provisioner_info(&net_info,vc_uuid);
				ele_adr_primary = net_info.unicast_address;
				VC_node_dev_key_save(ele_adr_primary, mesh_key.dev_key,g_ele_cnt);
				VC_node_cps_save(gp_page0, ele_adr_primary, SIZE_OF_PAGE0_LOCAL);
				// we can use the tbl_mac as the mac adr 
				mesh_json_set_node_info(ele_adr_primary,tbl_mac);
				json_set_appkey_bind_self_proc(vc_uuid,netidx);
				write_json_file_doc(FILE_MESH_DATA_BASE);		
    		}
			pro_self_flag =1;	
    	}
    	net_info.unicast_address = json_get_next_unicast_adr_val(vc_uuid);

	}else{
		pro_self_flag =0;
	}
}

BOOL CTl_ble_moduleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	// log init part 
	log_msg_buf_init();

	m_combo_extend_adv_option.Clear();
	m_combo_extend_adv_option.InsertString(0, _T("None"));
	m_combo_extend_adv_option.InsertString(1, _T("OTA Only"));
	m_combo_extend_adv_option.InsertString(2, _T("All"));
	m_combo_extend_adv_option.SetCurSel(0);

	load_ini_init();
	adv_chn_init_settings();
	class_pointer_init();
	mesh_retry_cnt_init();
	master_clock_init();
	SetTimer (1, 100, NULL);
	SetTimer (TIMER_APPEND_LOG, 10, NULL);
	OnCmdsel ();
	kma_dongle_init_log();
	select_ini_auto();
   	EnableToolTips(TRUE);  
	m_tip.Create(this);
	m_tip.SetDelayTime(200);
	m_tip.AddTool(GetDlgItem(IDC_OTA), _T("need connected and provisioned!"));
// init the json data file 
	if(ble_moudle_id_is_gateway()){
	    init_json(FILE_MESH_DATA_BASE,0);//gateway_json_init();// in this condition we have not receice the gateway rsp data .
	}else if(ble_moudle_id_is_kmadongle()){
		vc_json_init();
	}
	sig_mesh_user_init();

	EnableToolTips(TRUE);
	m_tooltip.Create(this);
	m_tooltip.Activate(TRUE);

	m_tooltip.AddTool(GetDlgItem(IDC_GATEWAT_RESET), "Gateway Reset");
	m_tooltip.AddTool(GetDlgItem(IDC_MESH_OTA), "Gateway Download Firmware For Mesh OTA");
	m_tooltip.AddTool(GetDlgItem(IDC_GATEWAY_OTA), "Gateway OTA Itself");
	m_tooltip.AddTool(GetDlgItem(IDC_CHECK_AUTO_SAVE_LOG), "Save log to file automatic");
	m_tooltip.AddTool(GetDlgItem(IDC_FASTBIND), "Fast Bind");
	m_tooltip.AddTool(GetDlgItem(IDC_COMBO_EXTEND_ADV_OPTION), "Extend Advertising Options");

	// create thread
	bulk_thread_status = THREAD_RUN;
	main_thread_status = THREAD_RUN;
	bulk_thread_handle = AfxBeginThread( ThreadBulkIn, this, THREAD_PRIORITY_TIME_CRITICAL);	// THREAD_PRIORITY_TIME_CRITICAL // THREAD_PRIORITY_HIGHEST
	main_thread_handle = AfxBeginThread( ThreadMainLoop, this, THREAD_PRIORITY_ABOVE_NORMAL);	// should not highest
	
	SetWindowPos(&CWnd::wndTop,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTl_ble_moduleDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTl_ble_moduleDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

#define INIBUFFSIZE 1024*32

int CTl_ble_moduleDlg::LoadIniFile() 
{
	
	int case_no = 0;
	CString str_last;
	CString str_pk;
	CString str;
	char key[INIBUFFSIZE]="default";
	//char desc[128];
	DWORD nd;

	GetPrivateProfileString ("SET", NULL, "", key, INIBUFFSIZE, m_InitFile);

	format_num = 0;
	if (1) {
		nd = GetPrivateProfileString ("SET", NULL, "", key, INIBUFFSIZE, m_InitFile);
		char *pk = key;
		((CListBox  *) GetDlgItem (IDC_COMMAND))->ResetContent ();
		while (nd>0) {
			char * pdir = strstr (pk, "CMD-");

			if (pdir == pk) {
				str.Format ("%s", pk + 4);
				((CListBox  *) GetDlgItem (IDC_COMMAND))->AddString (str);
			}
			pdir = strstr (pk, "EVT-");
			if (pdir == pk) {

				format_str[format_num++].Format ("%s", pk+4);
			}

			while (nd>0 && *pk!=0) {pk++; nd--;}
			while (nd>0 && *pk==0) {pk++; nd--;}
		}

		((CComboBox  *) GetDlgItem (IDC_COMMAND))->SetCurSel (0);
		OnSelchangeCommand();
	}

#if 1
		//CString str;
		memset (format_buff, 0, sizeof (format_buff));
		for (int k=0; k<format_num; k++) {
			str.Format ("EVT-%s", format_str[k]);
			GetPrivateProfileString ("SET", str, "", key, 1024, m_InitFile);
			
			int pos = 0;
			int id = 0;
			int len = strlen (key);
			while (pos < len && id < 16) {
				while ((key[pos]==' ' || key[pos] == '\t') && pos < len) {pos++;}
				if (sscanf_s (key + pos, "%x", format_buff +id + k * 16) == 1)
				{
					id++;
					while ((key[pos]!=' ' && key[pos] != '\t') && pos < len) {pos++;}
				}
				else
				{
					break;
				} 
			}
			//sscanf_s (key, "%x %x", evt_mask + k, evt_val + k);
		}
#endif
	return 1;
}

int WritePrivateProfileInt( LPCTSTR lpAppName, LPCTSTR lpKeyName, INT Value, LPCTSTR lpFileName )
{
	TCHAR ValBuf[16];
	StringCbPrintf( ValBuf,sizeof(ValBuf) ,TEXT( "%i" ), Value);
	return( WritePrivateProfileString( lpAppName, lpKeyName, ValBuf, lpFileName ) );
}

void set_ini_unicast_max(u16 adr)
{
	WritePrivateProfileInt("SET", "json_adr_max",adr, m_InitFile);
}

u16 get_ini_unicast_max()
{
	return GetPrivateProfileInt("SET", "json_adr_max",0, m_InitFile);

}

void CTl_ble_moduleDlg::CTl_ble_module_Init() 
{
//	m_hDev = NULL;

	char buff[128];

	GetPrivateProfileString ("SET", "prnid", "ffff", buff, 1024, m_InitFile);
	sscanf_s (buff, "%x", &dongle_ble_moudle_id);
	m_hDev = NULL;

	GetPrivateProfileString ("SET", "i2c_id", "6e", buff, 1024, m_InitFile);
	sscanf_s (buff, "%x", &m_i2c_id);

	GetPrivateProfileString ("SET", "vidpid", "248a82bd", m_device_vidpid.GetBuffer(1024), 1024, m_InitFile);
	m_device_vidpid.ReleaseBuffer ();
	
 	UINT log_backup_value = GetPrivateProfileInt("SET", "log_backup", 0, m_InitFile);
    m_log_backup = (log_backup_value == 1);
	CButton *checkbox = (CButton *)GetDlgItem(IDC_CHECK_AUTO_SAVE_LOG);
	checkbox->SetCheck(m_log_backup);

 	UINT json_backup_value = GetPrivateProfileInt("SET", "json_backup", 0, m_InitFile);
    m_json_backup = (json_backup_value == 1);

	UINT fastbind_value = GetPrivateProfileInt("SET", "fastbind", 0, m_InitFile);
	checkbox = (CButton *)GetDlgItem(IDC_FASTBIND);
	checkbox->SetCheck(fastbind_value);

	int extend_adv_option = GetPrivateProfileInt("SET", "ExtendAdvOption", 0, m_InitFile);
	if (extend_adv_option >= m_combo_extend_adv_option.GetCount()) {
		extend_adv_option = 0;
	}
	m_ExtendAdvOption = extend_adv_option;
	m_combo_extend_adv_option.SetCurSel(extend_adv_option);

	//////////////////////////////////////////////
	// Load BIN setting
	//////////////////////////////////////////////
	LoadIniFile ();
	// clear the handle of the devices 
	m_hDev = GetPrintDeviceHandle(dongle_ble_moudle_id);
	m_hDevW = GetPrintDeviceHandle(dongle_ble_moudle_id);
#if JSON_FILE_ENABLE
	if(dongle_ble_moudle_id == VC_DONGLE_USB_ID){
        //set the vc uuid information 
        vc_get_guid(vc_uuid);
		vc_get_dev_key(vc_dev_key);
    }else if (dongle_ble_moudle_id == VC_GATEWAY_USB_ID){
        // get the gateway uuid information 
        if(m_hDev || connect_serial_port){
            gateway_get_dev_uuid_mac();  
			vc_get_gateway_dev_key(vc_dev_key);
        }
    }
#endif
}

int isVC_DLEModeExtendBearer()
{
	return g_module_dlg->m_ExtendAdvOption;
}

void CTl_ble_moduleDlg::OnSelchangeInifile() 
{
	// TODO: Add your control notification handler code here
	int ns = ((CComboBox *) GetDlgItem (IDC_INIFILE))->GetCurSel(); 
	((CComboBox *) GetDlgItem (IDC_INIFILE))->GetLBText (ns, m_InitStr.GetBuffer(256));
	m_InitStr.ReleaseBuffer ();
	m_InitFile = m_CurrentDir + "\\" + m_InitStr;
	if (ns >= 0) {
		m_cmd_input = "";
		CTl_ble_module_Init ();
	}	
	
}

void CTl_ble_moduleDlg::FillTxPar(char *cmd)
{
    unsigned char buff[1024];
    int len = Text2Bin((LPBYTE)buff,(LPBYTE)cmd);
    u16 hci_cmd_type = ((u16)(buff[1])<<8)+buff[0];
    if(hci_cmd_type == HCI_CMD_GATEWAY_CMD||hci_cmd_type == HCI_CMD_BULK_CMD2DEBUG){
        mesh_bulk_cmd_par_t *p_cmd = (mesh_bulk_cmd_par_t *)(buff+2);
        if(0 == p_cmd->retry_cnt){
            p_cmd->retry_cnt = m_retry_cnt;
        }else if(0xff == p_cmd->retry_cnt){
            p_cmd->retry_cnt = 0;
        }
        mesh_get_netkey_idx_appkey_idx(p_cmd);
        //memset(cmd,0,strlen(cmd));
        bin2text_normal((LPBYTE)cmd,(LPBYTE)buff,len);
    }
}

void CTl_ble_moduleDlg::OnSelchangeCommand() 
{
	// TODO: Add your control notification handler code here
	UpdateData ();
	short kc = GetKeyState(VK_LCONTROL) >> 8;
	short ks = GetKeyState(VK_LSHIFT) >> 8;

	char cmd[1024];
	int ns = ((CListBox *) GetDlgItem (IDC_COMMAND))->GetCurSel ();
	CString str;
	if (ns >= 0) {
		((CListBox *) GetDlgItem (IDC_COMMAND))->GetText (ns, cmd);
		
		str.Format ("CMD-%s", cmd);
		GetPrivateProfileString ("SET", str, "", cmd, 1024, m_InitFile);
        FillTxPar(cmd);
        
		m_cmd_input = cmd;
		UpdateData (FALSE);
	}				
}

void CTl_ble_moduleDlg::OnDblclkCommand() 
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	UpdateData ();
	short kc = GetKeyState(VK_LCONTROL) >> 8;
	short ks = GetKeyState(VK_LSHIFT) >> 8;

	char cmd[1024];
	int ns = ((CListBox *) GetDlgItem (IDC_COMMAND))->GetCurSel ();
	CString str;
	int step = 0;
	if (ns >= 0) {
		((CListBox *) GetDlgItem (IDC_COMMAND))->GetText (ns, cmd);

		LogMsg (".. %s\r\n", cmd);
		
		str.Format ("CMD-%s", cmd);
		GetPrivateProfileString ("SET", str, "", cmd, 1024, m_InitFile);
        FillTxPar(cmd);

		m_cmd_input = cmd;
		UpdateData (FALSE);

		ExecCmd (cmd, 0);
	}			
}
CString cslog;
CString csfile_name;
u8 log_win_en = 1;
void set_log_windown_en (u8 en)
{
	log_win_en = en;
}
int CTl_ble_moduleDlg::LogMsg (LPSTR sz,...)
{
    char ach[4096];

	if (!m_bLogEn)
		return 0;
    int n = wvsprintf(ach, sz, (LPSTR)(&sz+1));   /* Format the string , max output 1024*/
#if 1
	CString cstemp;
	cstemp.FormatMessage(ach,n);
	if(g_module_dlg->m_log_backup){
		cslog+=cstemp;
	}
#endif
	if(log_win_en){
		shared_ptr<char> log(new char[n+1]);
		memcpy(log.get(), ach, n);
		log.get()[n] = '\0';

		EnterCriticalSection(&log_queue_cs);
		log_queue.push(log);
		LeaveCriticalSection(&log_queue_cs);
	}
    return n;
}

int log_sno = 0;
int CTl_ble_moduleDlg::LogMsgTime ()
{
#if 0
	CTime time = CTime::GetCurrentTime();
	LogMsg ("%2d:%2d:%2d", time.GetHour(), time.GetMinute(), time.GetSecond());
#else
	SYSTEMTIME stUTC;  
	::GetLocalTime(&stUTC);
	LogMsg ("<%04d>%02d:%02d:%02d:%03d ", log_sno++, (stUTC.wHour), stUTC.wMinute, stUTC.wSecond, stUTC.wMilliseconds);
	csfile_name.Format("meshlog_%04d_%02d_%2d_%2d.txt",stUTC.wYear,stUTC.wMonth,stUTC.wDay,stUTC.wHour);
#endif

    return 0;
}

void CTl_ble_moduleDlg::append_log()
{
#define LOG_TMP_LEN_DEFAULT       4096
	static char print_buffer[LOG_TMP_LEN_DEFAULT];

	__try{
		EnterCriticalSection(&log_queue_cs);

		if (log_queue.empty()) {
			return;
		}

		// joint logs together.
		char *log_tmp = print_buffer;
		int log_len_max = LOG_TMP_LEN_DEFAULT;
		int log_count = 0;
		while(log_queue.size()) {
			char *msg =log_queue.front().get();
			int len = strlen(msg);
			if (log_count + len < log_len_max) {
				memcpy(&log_tmp[log_count], msg, len);
				log_count += len;

				log_queue.pop();
			}else if(log_count == 0){
				log_len_max = (len + LOG_TMP_LEN_DEFAULT-1) / LOG_TMP_LEN_DEFAULT * LOG_TMP_LEN_DEFAULT;
				log_tmp = new char[log_len_max];
			}else{
				break;
			}
		}
		log_tmp[log_count] = '\0';

		int len = m_Log.GetWindowTextLength();
		int nstep = m_nTextLen  / 8;
		if (len >= nstep * 7) {
			m_Log.SetSel (0, nstep*4);
			m_Log.ReplaceSel ("");
			len = m_Log.GetWindowTextLength();
		}
		m_Log.SetSel (len, len);
		m_Log.ReplaceSel (log_tmp);
		if (log_tmp != print_buffer) {
			delete[] log_tmp;
		}
	}
	__finally{
		LeaveCriticalSection(&log_queue_cs);
	}

}

void PrintLog(CString& str)
{
	static CMutex PrintLog_mtuex;

	if (str.GetLength()<1000)
	{
		return;
	}

	PrintLog_mtuex.Lock();

    CString path = ".\\backup\\";
    if (-1 == GetFileAttributes(path)) {
        CreateDirectory(path, NULL);
    }
    path = path + csfile_name;

	CStdioFile File;
	BOOL open_ret;
	open_ret = File.Open(path.GetString(), CFile::modeReadWrite|CFile::modeNoTruncate|CFile::modeCreate);
	if(open_ret == TRUE){
		File.SeekToEnd();
		File.WriteString(str );
		File.Close();
		str.Format("");
	}
	PrintLog_mtuex.Unlock();
}

int LogMsgModuleDlg_and_buf(u8 *pbuf,int len,char *log_str,char *format, va_list list)
{
	char buff[4069],buf_tmp[1024];
	unsigned int log_len =0;
	u32 buf_idx =0;
	log_len =strlen(log_str);
	memcpy(buff,log_str,log_len);
	buf_idx +=log_len;
    vsnprintf_s( buf_tmp ,1024, format, list);
	unsigned int len_str =0;
	len_str = strlen (buf_tmp);
	memcpy(buff+buf_idx,buf_tmp,len_str);
	buf_idx +=len_str;
	if(pbuf){
	    if(len > 0){
        	BYTE str[4069];
        	unsigned int len_buf;
            len_buf = Bin2Text(str, pbuf, len);
        	buff[buf_idx++]=':';
        	buff[buf_idx++]=' ';
        	memcpy(buff+buf_idx,str,len_buf); 
        	buf_idx += len_buf;
        	buff[buf_idx-1] = '\r'; // because the last char is 0,
        	buff[buf_idx++] = '\n';
    	}else{
            buff[buf_idx++] = 'N';
            buff[buf_idx++] = 'U';
            buff[buf_idx++] = 'L';
            buff[buf_idx++] = 'L';
        	buff[buf_idx++] = '\r';
        	buff[buf_idx++] = '\n';
    	}
	}else{
    	buff[buf_idx++] = '\r';
    	buff[buf_idx++] = '\n';
	}
	buff[buf_idx++] = '\0';     // must
	g_module_dlg->LogMsgTime();
	g_module_dlg->LogMsg("%s", buff);
    if (g_module_dlg->m_log_backup) {
	    PrintLog(cslog);
    }else{
        cslog.Empty();
    }
	return 0;
}
enum{
	EVENT_CLOSE,
};

void CTl_ble_moduleDlg::FunctionCommon(unsigned char event)
{
	if(EVENT_CLOSE == event){
		OnCancel();
		DestroyWindow();
	}
}

void ModuleDlg_close()
{
	g_module_dlg->FunctionCommon(EVENT_CLOSE);
}

int device_and_connect_check_valid()
{
#if ((PROXY_HCI_SEL == PROXY_HCI_GATT) && (!DEBUG_SHOW_VC_SELF_EN))
	if(!connect_flag && IsMasterMode()){
		#if VC_APP_ENABLE
		if(show_disconnect_box){
		    show_disconnect_box = 0;
            AfxMessageBox("Please connect first!");
            return 0;
		}else{
            LOG_MSG_ERR(TL_LOG_WIN32,0,0,"xxxxxxxxxxxxxxxxxxxxxxxxxxx Please connect first! xxxxxxxxx");
		}
		#endif
	}
#endif

	return 1;
}

void MessageBoxVC(const char *str)
{
    AfxMessageBox(str);
}

void mesh_tx_reliable_stop_report(u16 op, u16 adr_dst,u32 rsp_max, u32 rsp_cnt)
{
	if(op == NODE_RESET){
		extern void json_use_adr_to_del_info(u16 adr);
		json_use_adr_to_del_info(adr_dst);
	}
	LOG_MSG_INFO(TL_LOG_WIN32,0,0,
			"mesh_tx_reliable_stop: op 0x%04x rsp_max %d, rsp_cnt %d", op, rsp_max, rsp_cnt);

	
    if(m_pMeshDlg->shedule_init && (0 == rsp_cnt)){ // timeout
        m_pMeshDlg->shedule_init = 0;
    }

    if(ble_moudle_id_is_kmadongle()){
        schedule_get_proc_tick_check_and_clear();
    }
}

void WriteFile_handle(u8 *buff, int n, u8 *head, u8 head_len)
{
	if(!device_and_connect_check_valid()){
		return ;
	}

#if (PROJECT_SEL == PROJECT_VC_DONGLE)
	my_fifo_push(&hci_rx_fifo, buff, n, head, head_len);
#else
	DWORD	nB;
	if(connect_serial_port){
		write_file_uart(buff,n);		
	}else{
		int bok =  WriteFile(m_hDevW, buff, n, &nB, NULL);
	}
	
#endif
	WaitForSingleObject (NULLEVENT2, 10);

}

void gateway_set_reliable_cmd(mesh_tx_reliable_t *p_reli , mesh_bulk_cmd_par_t *p_bulk)
{
    u32 op_type = GET_OP_TYPE(p_bulk->op);
    u8 *p_ac_par ;
    if(op_type == OP_TYPE_SIG1){
        p_ac_par = p_bulk->par;
        p_reli->mat.op = p_bulk->op;
    }else if (op_type == OP_TYPE_SIG2){
        p_ac_par = p_bulk->par+1;
        p_reli->mat.op = p_bulk->op|((p_bulk->par[0])<<8);
    }else if (op_type == OP_TYPE_VENDOR){
        p_ac_par = p_bulk->par+2;
        p_reli->mat.op = p_bulk->par[1];
    }else{}
    p_reli->mat.adr_src = ele_adr_primary;
    memcpy(p_reli->ac_par,p_ac_par,MESH_CMD_ACCESS_LEN_MAX-(op_type-1));
    return ;
}

void WriteFile_host_handle(u8 *buff, int n)
{
	DWORD	nB;
	if(connect_serial_port){
		write_file_uart(buff,n);
		Sleep(2);
	}else{
		int bok =  WriteFile(m_hDevW, buff, n, &nB, NULL);
	}
}

void gateway_send_unicast_adr_grp(u16* adr_list,int node_cnt)
{
	u8 gateway_buf[MESH_OTA_UPDATE_NODE_MAX*2+0x20];
	int buf_cnt = 3+node_cnt*2;
	gateway_buf[0] =  HCI_CMD_GATEWAY_CTL & 0xFF;
	gateway_buf[1] = (HCI_CMD_GATEWAY_CTL >> 8) & 0xFF;
	gateway_buf[2] = HCI_GATEWAY_CMD_MESH_OTA_ADR_SEND;
	memcpy(gateway_buf+3,(u8*)(adr_list),node_cnt*2 );
	if((buf_cnt>REPORT_ADV_BUF_SIZE) && ble_moudle_id_is_gateway()){
		gateway_sar_pkt_segment(gateway_buf, buf_cnt, REPORT_ADV_BUF_SIZE, 0, 0);
	}else{
		WriteFile_handle(gateway_buf, buf_cnt, 0, 0);
	}
	WaitForSingleObject (NULLEVENT2, 10);
	return ;
}

#define ERROR_STRING_DISTRIBUTE_START_ALL	"No update node, please get online status before!!!"

void error_report_no_update_node()
{
	LOG_MSG_ERR (TL_LOG_COMMON, 0, 0, ERROR_STRING_DISTRIBUTE_START_ALL,0);
	AfxMessageBox(ERROR_STRING_DISTRIBUTE_START_ALL);
}

int vc_distribute_all_proc(mesh_bulk_ini_vc_t *p_vc_ini,int n)
{
	if(HCI_CMD_MESH_OTA_INITIATOR_START == p_vc_ini->flag){
		fw_initiator_start_t *p_start = (fw_initiator_start_t *)&p_vc_ini->cmd;
		if(0 == p_start->adr_distributor){
			p_start->adr_distributor = APP_get_GATT_connect_addr();
			u32 len_null = OFFSETOF(mesh_bulk_ini_vc_t,cmd)+sizeof(fw_initiator_start_t);
    		if(n == len_null){// the distribute is empty ,perhaps is the cmd of the distribute start all
    			u16 adr_list[MESH_OTA_UPDATE_NODE_MAX] = {0};
    			u32 update_node_cnt;
    			update_node_cnt = m_pMeshDlg->get_all_online_node(adr_list, MESH_OTA_UPDATE_NODE_MAX);
    			if(ble_moudle_id_is_gateway()){
                    LOG_MSG_ERR (TL_LOG_COMMON, 0, 0, "GATEWAY can not support initiator start now!",0);
    				return 0;	// TODO
    			}else if(update_node_cnt > 0){
					u8 *p_list = (u8 *)p_vc_ini + len_null;
    				memcpy(p_list,adr_list,update_node_cnt*2);
    				return (n+update_node_cnt*2);
    			}else{
    				error_report_no_update_node();
                    return 0;
    			}
    		}
		}

		return n;
	}

    u16 op_cmd = get_op_u16(&p_vc_ini->cmd.op);
    u32 op_len = GET_OP_TYPE(op_cmd);
    #if DRAFT_FEAT_VD_MD_EN
    if(OP_TYPE_VENDOR == op_len){
        op_len = 1;
    }
    #endif
    
	if(op_cmd == FW_DISTRIBUT_START){
	    #if DISTRIBUTOR_START_TLK_EN
	    u32 par_head = OFFSETOF(mesh_bulk_ini_vc_t,cmd)+OFFSETOF(mesh_bulk_cmd_par_t,op)+op_len;
        if(is_par_distribut_start_tlk(p_vc_ini->cmd.par+1, n-par_head)){
    	    if(ble_moudle_id_is_kmadongle()){
    	        if(p_vc_ini->cmd.adr_dst != ele_adr_primary){
        	        p_vc_ini->cmd.adr_dst = ele_adr_primary;
                    LOG_MSG_INFO(TL_LOG_COMMON, 0, 0, "auto change destination address",0);
                }
    	    }
    	    
    	    u32 len_null = par_head + OFFSETOF(fw_distribut_start_tlk_t,update_list);
    		if(n == len_null){// the distribute is empty ,perhaps is the cmd of the distribute start all
    			u16 adr_list[MESH_OTA_UPDATE_NODE_MAX] = {0};
                fw_distribut_start_tlk_t *p_start = (fw_distribut_start_tlk_t *)(((u8 *)&p_vc_ini->cmd.op)+op_len);
    			u32 update_node_cnt;
    			update_node_cnt = m_pMeshDlg->get_all_online_node(adr_list, MESH_OTA_UPDATE_NODE_MAX);
    			if(update_node_cnt >= 150)// it means the distribute cmd will over come 380,so we need to send the cmd by other cmd .
    			{
    				gateway_send_unicast_adr_grp(adr_list,update_node_cnt);// send special cmd first ,and then send the unicast grp 
    				return n;
    			}else if(update_node_cnt > 0){
    				memcpy(p_start->update_list,adr_list,update_node_cnt*2);
    				return (n+update_node_cnt*2);
    			}else{
    				error_report_no_update_node();
                    return 0;
    			}
    		}
		}else
		#endif
		{
		    //
		}
	}
	return n;
}

int CTl_ble_moduleDlg::ExecCmd(LPCTSTR cmd, int log_type) 
{
	BYTE	buff[1024];
	#if 1
	char usb_id[] = {"a3 ff"};
	char op_cmd[16] = {0};
	u8 op_temp = BLOB_CHUNK_TRANSFER;
	Bin2Text((BYTE *)op_cmd, &op_temp, 1);
	u8 *p_start = (u8 *)(cmd+4);
	if(log_type && (0 == memcmp(op_cmd, p_start+31, 2)) && (0 == memcmp(usb_id, p_start, 5))){
	    char log_buf[64] = {0};
	    memcpy(log_buf, p_start, 14*3);  // 7: chunk num
	    LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "ExecCmd: %s", log_buf);
	}else
	#endif
	{
	    LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "ExecCmd: %s", cmd);
	}
    int n = Text2Bin (buff, (LPBYTE) cmd);
	n = vc_distribute_all_proc((mesh_bulk_ini_vc_t *)buff,n);
	if(n){
		if((n>REPORT_ADV_BUF_SIZE) && ble_moudle_id_is_gateway()){
			gateway_sar_pkt_segment(buff, n, REPORT_ADV_BUF_SIZE, 0, 0);
		}
		else{
			WriteFile_handle(buff, n, 0, 0);
		}
	}

	return 1;
}

void CTl_ble_moduleDlg::ExecOpPara(unsigned char * p, int n) 
{
	char cmd[2048];

	Bin2Text ((BYTE *)cmd, p, n);
	m_cmd_input = cmd;

	ExecCmd (cmd, 1);
}

void CTl_ble_moduleDlg::ExecStrCmd(char * cmd) 
{
	m_cmd_input = cmd;
	UpdateData (FALSE);
	
	ExecCmd (cmd, 0);
}
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void CTl_ble_moduleDlg::OnOpen() 
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	char cmd[] = "C:\\Windows\\notepad.exe";
	char cmdline[1024];
	sprintf_s (cmdline,sizeof(cmdline), "C:\\Windows\\notepad.exe %s", (const char*)m_InitStr);

	DoCmd (cmd, cmdline, m_CurrentDir, 0);
}

void CTl_ble_moduleDlg::DoCmd(LPCTSTR cmd, LPSTR cmdline, LPCTSTR curdir, int nownd = 0)
{
	PROCESS_INFORMATION pif;  //Gives info on the thread and..
                           //..process for the new process
	STARTUPINFO si;          //Defines how to start the program

	ZeroMemory(&si,sizeof(si)); //Zero the STARTUPINFO struct
	si.cb = sizeof(si);         //Must set size of structure
	//si.dwFlags = STARTF_USESHOWWINDOW;

	BOOL bRet = CreateProcess(
		cmd,  
		cmdline, //NULL,   //Command string - not needed here
		NULL,   //Process handle not inherited
		NULL,   //Thread handle not inherited
		FALSE,  //No inheritance of handles
		nownd,      //No special flags
		NULL,   //Same environment block as this prog
		curdir,   //Current directory - no separate path
		&si,    //Pointer to STARTUPINFO
		&pif);   //Pointer to PROCESS_INFORMATION

	if(bRet == FALSE)  {
		AfxMessageBox("Unable to start program");
		return;
	}
	//TerminateProcess (pif.hProcess, -2);
	CloseHandle(pif.hProcess);   //Close handle to process
	CloseHandle(pif.hThread);    //Close handle to thread	
}

void CTl_ble_moduleDlg::OnOK() 
{
	// TODO: Add extra validation here
	//CDialog::OnOK();
	UpdateData (TRUE);

	LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, ".. %s", m_cmd_input);
	ExecCmd (m_cmd_input, 0);
}


void CTl_ble_moduleDlg::OnClosedlg() 
{
	// TODO: Add your control notification handler code here
	//OnClose();
	PostMessage(WM_CLOSE);
}

int CTl_ble_moduleDlg::format_parse (unsigned char *ps, int len, unsigned char *pd)
{
	if (ps[3] == 3)
	{
		ps[3] = 3;
	}
	unsigned char *po, *pi;
	for (int i=0; i<format_num; i++) 
	{
		int match = 1;
		int *pw = format_buff + i * 16;
		pi = ps; //+ 1;
		po = pd;
		for (int j=0; j<16 && pw[j]; j++)
		{
			////////////////// format check ////////////////
			unsigned char *pb = (unsigned char *)(pw + j);
			unsigned int d = pi[0] + pi[1] * 256;
			int nb = pb[3] & 15;
			if (nb >= 3)
				d += pi[2] << 16;
			if (nb == 4)
				d += pi[3] << 24;
			int s = pb[2] & 15;
			int e = pb[2] >> 4;
			int m = e < s ? 0 : ((1 << (e - s + 1)) - 1) << s ;
			//LogMsg ("%x %x %d %d %d\r\n", pw[j], d, s, e, m);
			if ( (d & m) != ((pb[0] + pb[1] * 256) & m) )
			{
				match = 0;
				break;
			}
			if (pb[3] == 0x84)			//time stamp
			{
				po += Time2Text (po, d);
			}
			else
			{
				po += Hex2Text (po, pi, nb);
			}

			pi += nb;
		}
		if (match)
		{
			int n = len - (pi - ps);
			if (n > 0) {
				Bin2Text (po, pi, n);
			}
			else {
				*po = 0;
			}
			return i;
		}
	
	}
	
	Bin2Text (pd, ps, len);
	return -1;
}

unsigned char CTl_ble_moduleDlg::pro_cmd_printing(unsigned char dat)
{
	switch(dat)
		{
			case PRO_INVITE:
				LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "The invite cmd is ");
				break;
			case PRO_CAPABLI:
				LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "The capa cmd is ");
				break;
			case PRO_START:
				LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "The start cmd is ");
				break;
			case PRO_PUB_KEY:
				LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "The pubkey cmd is ");
				break;
			case PRO_INPUT_COM:
				LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "The input cmd is ");
				break;
			case PRO_CONFIRM:
				LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "The confirm cmd is ");
				break;
			case PRO_RANDOM:
				LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "The random cmd is ");
				break;
			case PRO_DATA:
				LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "The provision data cmd is ");
				break;
			case PRO_COMPLETE:
				LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "The complete data cmd is ");
				break;
			default:
				break;
		}	
	return TRUE;
}



void 	provision_flow_Log(unsigned char cmd ,unsigned char *p,unsigned int wLen)
{	
	#if LOG_PROVISION_EN
	BYTE	buff[1024*32];
	Bin2Text (buff, p, wLen);
	if(cmd&INPUT_LOG_AND){
		LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, " =============  PROVISIONER  <<<<<<<<<<<<<<<<<< IUT===================  ");
	}else{
		LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, " =============  PROVISIONER  >>>>>>>>>>>>>>>>>> IUT===================  ");
	}
	g_module_dlg->pro_cmd_printing(p[0]);
	LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "provision: %s", buff);
	#else
	#endif 
	return ;
}

void prov_print_gateway_log(u8 dir,u8 type,u8 *p_cmd,u16 len)
{
    if(dir){
        LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, " =============  GATEWAY  >>>>>>>>>>>>>>>>>> IUT===================  ");
	}else{
        LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, " =============  GATEWAY  <<<<<<<<<<<<<<<<<< IUT===================  ");
    }
    if(type == PRO_INVITE){
        LOG_MSG_INFO (TL_LOG_COMMON, p_cmd, len, "prov invite cmd", 0);
    }else if (type == PRO_CAPABLI){
        LOG_MSG_INFO (TL_LOG_COMMON, p_cmd, len, "prov capa cmd", 0);
    }else if (type == PRO_START){
        LOG_MSG_INFO (TL_LOG_COMMON, p_cmd, len, "prov start cmd", 0);
    }else if (type == PRO_PUB_KEY){
        LOG_MSG_INFO (TL_LOG_COMMON, p_cmd, len, "prov pubkey cmd", 0);
    }else if (type == PRO_INPUT_COM){
        LOG_MSG_INFO (TL_LOG_COMMON, p_cmd, len, "prov input cmd", 0);
    }else if (type == PRO_CONFIRM){
        LOG_MSG_INFO (TL_LOG_COMMON, p_cmd, len, "prov comfirm cmd", 0);
    }else if (type == PRO_RANDOM){
        LOG_MSG_INFO (TL_LOG_COMMON, p_cmd, len, "prov random cmd", 0);
    }else if (type == PRO_DATA){
        LOG_MSG_INFO (TL_LOG_COMMON, p_cmd, len, "prov data cmd", 0);
    }else if (type == PRO_COMPLETE){
        LOG_MSG_INFO (TL_LOG_COMMON, p_cmd, len, "prov complete cmd", 0);
    }else if (type == PRO_FAIL){
        LOG_MSG_INFO (TL_LOG_COMMON, p_cmd, len, "prov fail cmd", 0);
    }else{};
    return ;
}


typedef	struct {
	u16		len;
	u8		data[1];
}	gateway_fifo_buf_t;

u8 gateway_total_buf[1024*32];
unsigned int gateway_total_idx=0;
unsigned int gateway_get_idx =0;
void init_gateway_para_for_buf()
{
	gateway_total_idx =0;
	gateway_get_idx =0;
	memset(gateway_total_buf,0,sizeof(gateway_total_buf));
}
LRESULT  CTl_ble_moduleDlg::OnAppendLog (WPARAM wParam, LPARAM lParam )
{
	int		n = (int) wParam;
	LPBYTE	pu = (unsigned char *) lParam;
	if(connect_serial_port){
		// merge the packet part into a single buf
		if((gateway_total_idx+n) > sizeof(gateway_total_buf)){ // avoid overflow
			init_gateway_para_for_buf();
			return 0;
		}
		else{
			memcpy(gateway_total_buf+gateway_total_idx,pu,n);
		}
		gateway_total_idx +=n;
		// fetch the whole packet 
		gateway_fifo_buf_t *p_fifo;
		while(1){
			p_fifo = (gateway_fifo_buf_t *)(gateway_total_buf+gateway_get_idx);
			if((p_fifo->len+sizeof(p_fifo->len)+gateway_get_idx)>gateway_total_idx){
				// not a whole packet 
				gateway_total_idx = gateway_total_idx - gateway_get_idx;
				memcpy(gateway_total_buf, gateway_total_buf+gateway_get_idx, gateway_total_idx);				
				gateway_get_idx = 0;
				return 0;
			}else if ((p_fifo->len+sizeof(p_fifo->len)+gateway_get_idx)==gateway_total_idx){
				// is a whole packet 
				push_notify_into_fifo(p_fifo->data, p_fifo->len);
				init_gateway_para_for_buf();
				return 0;
			}else{
				push_notify_into_fifo(p_fifo->data, p_fifo->len);
				gateway_get_idx +=p_fifo->len+sizeof(p_fifo->len);	
			}
		}
	
	}else{
 		push_notify_into_fifo(pu, n);
	}

 	if(disable_log_cmd){
 	    disable_log_cmd = 0;
 	    OnLogDisable();
 	}
	
	return 0;
}


LRESULT  CTl_ble_moduleDlg::OnUpdateData (WPARAM wParam, LPARAM lParam )
{
	UpdateData(wParam);

	return 0;
}

#define MAX_PROXY_BUF_LEN 400
u8 proxy_buf[MAX_PROXY_BUF_LEN];
u16 proxy_len=0;
/*********************** demo onappendloghandle*******************
******************************************************************/

u8 ble_sts =0;

u8 gateway_provision_sts =0;
u8 vc_node_ele_cnt =0;

void VC_mesh_cmd_rsp_handle(u8 *pu)
{
    u16 len = pu[1] + (pu[2] << 8);
    m_pMeshDlg->StatusNotify (pu+3, len);
    m_pRxTestDlg->StatusNotify(pu+3, len);
}
extern int mesh_rsp_handle_proc_fun(mesh_rc_rsp_t *p_rsp,mesh_tx_reliable_t *p_tx_rela);
LRESULT  CTl_ble_moduleDlg::OnAppendLogHandle (WPARAM wParam, LPARAM lParam )
{

	int		n = (int) wParam;
	LPBYTE	pu = (unsigned char *) lParam;
	BYTE	buff[1024*32];
	int		offset;

	if (!m_bHex)
	{
		pu[n] = 0;
		LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "rawdata: %s", pu);
		return 0;
	}

#if 0
	int c = n > 4096 ? 4096 : n;
	Bin2Text (buff, pu, c);
	// LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "%s", buff);
	n -= c;
#else
	if (m_pScanDlg && !prov_mode_is_rp_mode() && m_pScanDlg->IsWindowVisible())
	{
		m_pScanDlg->AddDevice (pu, n);
	}
#if 1
	int id = format_parse(pu+2, n-2, buff);
	if (id >= 0 && !ble_moudle_id_is_gateway())
	{
		LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "rawdata: %s", format_str[id]);
	}
	else
	{
		if(pu[0] == HCI_GATEWAY_CMD_SAR_MSG){
			if(gateway_sar_pkt_reassemble(pu+1,n-1) == RIGHT_PACKET_RET){
				n = gateway_seg_buf_len;
				pu = gateway_seg_buf;
			}
			else{
				return 0;
			}
		}

		u8 type = pu[0] & 0x7F;
		u8 len = pu[1];
		u8 ret_type;

        if(MESH_TX_CMD_RUN_STATUS == pu[0]){
            LOG_MSG_INFO (TL_LOG_COMMON, pu, n, "mesh bulk cmd error: ", 0);
            clr_mesh_ota_master_wait_ack(); // OTA tx command failed
            return 0;
        }
		
		if (pu[0]==MESH_CONNECTION_STS_REPROT){
			connect_flag = ble_sts = pu[2];
			if(connect_flag){
				mesh_tx_sec_nw_beacon_all_net(1);
				if(is_provision_success()){
				    filter_send_flag  =1; // delay set command
				}else{
				    mesh_proxy_set_filter_init(ele_adr_primary);
				}
			}else{
			    connect_addr_gatt = 0;
			}
		}else if (pu[0]==MESH_GATT_OTA_STATUS){
		    u8 result = pu[2];
			if(result == GATT_OTA_SUCCESS){
				LOG_MSG_INFO(TL_LOG_COMMON,0, 0,"GATT OTA completed: OK ");
			}else if(result == GATT_OTA_FW_SIZE_TOO_BIG){
				LOG_MSG_INFO(TL_LOG_COMMON,0, 0,"GATT OTA failed: firmware size too big ");
			}else if(result == GATT_OTA_START){
				LOG_MSG_INFO(TL_LOG_COMMON,0, 0,"GATT OTA start...... ");
			}else{ // if(result == GATT_OTA_TIMEOUT_DISCONNECT){
				LOG_MSG_INFO(TL_LOG_COMMON,0, 0,"GATT OTA failed: timeout or disconnected ");
			}
		}else if (pu[0] == TSCRIPT_GATEWAY_DIR_RSP){
			// gateway print part 
			static u16 gateway_rsp_len =0;
			static u8 gateway_rsp_buf[512];
			u8 type = pu[1];
			if(n < 2){
				return 0;
			}
			u16 rsp_len = n-2;
			if(type == HCI_GATEWAY_RSP_OP_CODE){
                // proc the json file and the rsp part 
				m_pMeshDlg->StatusNotify (pu+2, rsp_len);
                m_pRxTestDlg->StatusNotify(pu+2, rsp_len);
				#if JSON_FILE_ENABLE
                mesh_rc_rsp_t rsp;
				mesh_rc_rsp_t* p_rsp = &rsp;
	            rsp.len = rsp_len;
	            memcpy(&rsp.src, pu+2, rsp_len);
				    #if DEBUG_CFG_CMD_GROUP_AK_EN
				u16 op = rf_link_get_op_by_ac(p_rsp->data);
				u32 size_op = SIZE_OF_OP(op);
				typedef struct{
					u32 node_rsp_tick;
					u32 master_rcv_tick;
					u32 master_send_tick;
				}rcv_str;
				#define tick_per_inter	(657*16)// follow the logic analyze ,to get the tick inter between the node and the gateway part .
				if(op == VD_MESH_TRANS_TIME_STS){
					static u32 test_cnt =0; 
					rcv_str *p_time = (rcv_str *)(&p_rsp->data[size_op]);
					p_time->master_rcv_tick -= tick_per_inter;
					p_time->master_send_tick -= tick_per_inter;
					int master_send_us =(p_time->node_rsp_tick - p_time->master_send_tick)/16;
					int master_rcv_us = (p_time->master_rcv_tick - p_time->node_rsp_tick)/16 ;
					test_cnt++;
					LOG_MSG_INFO(TL_LOG_COMMON,0, 0,"test cnt is %d,master_send_us is %d,master_rcv_us is %d ",test_cnt,master_send_us,master_rcv_us);
				}
                    #endif
				
				mesh_rsp_handle_proc_fun(&rsp,&vc_reliable);
				#endif
				LOG_MSG_INFO(TL_LOG_GATEWAY,pu, rsp_len+2,"HCI_GATEWAY_RSP_OP_CODE \r\n");
			}else if (type == HCI_GATEWAY_RSP_UNICAST){
				LOG_MSG_INFO(TL_LOG_GATEWAY,pu, rsp_len+2,"HCI_GATEWAY_RSP_UNICAST \r\n");
			}else if (type == HCI_GATEWAY_KEY_BIND_RSP){
				if(pu[2]){
					LOG_MSG_INFO(TL_LOG_GATEWAY,pu, rsp_len+2,"HCI_GATEWAY_KEY_BIND_RSP fail \r\n");
				}else{
					LOG_MSG_INFO(TL_LOG_GATEWAY,pu, rsp_len+2,"HCI_GATEWAY_KEY_BIND_RSP success \r\n");
				}
				
			}else if (type == HCI_GATEWAY_CMD_PRO_STS_RSP){
				if(pu[2] == 1){
					// the gateway has already provisioned 
					gateway_provision_sts =1;
					// get the provision data info part 
					memcpy((u8 *)(&net_info),pu+3,sizeof(net_info));
					// use the gateway para 
					net_info.unicast_address = json_get_next_unicast_adr_val(vc_uuid);
					m_proDlg->SetButton_sts();	
					LOG_MSG_INFO(TL_LOG_GATEWAY,pu, rsp_len+2,"HCI_GATEWAY_CMD_PRO_STS_RSP provisioned \r\n");
				}else{
					// the gateway not provisioned 
					gateway_provision_sts =0;
					LOG_MSG_INFO(TL_LOG_GATEWAY,pu, rsp_len+2,"HCI_GATEWAY_CMD_PRO_STS_RSP unprovisioned \r\n");
				}
			}else if (type == HCI_GATEWAY_CMD_PROVISION_EVT){
			    gateway_prov_event_t *p_prov =(gateway_prov_event_t *)(pu+2) ;
                // set the information part 
                gw_dev_unicast = p_prov->adr;
                memcpy(gw_dev_mac,p_prov->mac,sizeof(p_prov->mac));
                memcpy(gw_dev_uuid,p_prov->uuid,sizeof(p_prov->uuid));
			    LOG_MSG_INFO(TL_LOG_GATEWAY,pu, rsp_len+2,"HCI_GATEWAY_CMD_PROVISION_EVT \r\n");
				provision_end_callback(PROV_NORMAL_RET);
			}else if (type == HCI_GATEWAY_CMD_KEY_BIND_EVT){
				LOG_MSG_INFO(TL_LOG_GATEWAY,pu, rsp_len+2,"HCI_GATEWAY_CMD_KEY_BIND_EVT \r\n");
				App_key_bind_end_callback(pu[2]);
			}else if (type == HCI_GATEWAY_CMD_SEND_ELE_CNT){
				LOG_MSG_INFO(TL_LOG_GATEWAY,pu, rsp_len+2,"HCI_GATEWAY_CMD_SEND_ELE_CNT \r\n");
				vc_node_ele_cnt = pu[2];
			}else if (type == HCI_GATEWAY_CMD_SEND_NODE_INFO){
				LOG_MSG_INFO(TL_LOG_GATEWAY,pu, rsp_len+2,"HCI_GATEWAY_CMD_SEND_NODE_INFO \r\n");			
				extern CTL_privision *p_ctl_pro;
				if(p_ctl_pro->m_fast_prov_mode){
					fast_prov_node_info_t *p_info_rc = (fast_prov_node_info_t *)(pu+2);
					VC_node_dev_key_save(p_info_rc->node_info.node_adr, p_info_rc->node_info.dev_key,p_info_rc->node_info.element_cnt);
					mesh_fast_prov_node_info_callback(p_info_rc->node_info.dev_key, p_info_rc->node_info.node_adr, p_info_rc->pid);
				}
				else{
					VC_node_info_t *p_info_rc = (VC_node_info_t *)(pu+2);
					VC_node_dev_key_save(p_info_rc->node_adr, p_info_rc->dev_key,p_info_rc->element_cnt);
					// update the gateway's vc node info dev key part 
					memcpy(gw_dev_key,p_info_rc->dev_key,16);
				}
                
			}else if (type == HCI_GATEWAY_CMD_SEND_MESH_OTA_STS){
				u32 fail_addr_cnt = pu[2];
				if(fail_addr_cnt == 0){
					//gateway_send_ota_erase_ctl();
					LOG_MSG_INFO(TL_LOG_GATEWAY,0, 0,"mesh OTA success \r\n");	
				}else{
					LOG_MSG_ERR(TL_LOG_GATEWAY,pu+3, fail_addr_cnt*2,"mesh OTA total failed: %d, adddr list: ",fail_addr_cnt);	
				}
			}else if (type == HCI_GATEWAY_CMD_SEND_UUID){
                memcpy(vc_uuid,pu+2,16);
                memcpy(gw_mac,pu+18,6);
                LOG_MSG_INFO(TL_LOG_GATEWAY,pu+2, 16,"the gateway uuid is \r\n");
                LOG_MSG_INFO(TL_LOG_GATEWAY,pu+18, 6,"the gateway mac adr is \r\n");
				gateway_json_init();
				ele_adr_primary = json_use_uuid_to_get_unicast(vc_uuid);
				gateway_set_extend_seg_mode(m_ExtendAdvOption);
			}else if (type == HCI_GATEWAY_CMD_SEND_VC_NODE_INFO) {
				SetEvent(EVENT_UPDATE_NODE_INFO);
			}
			else if (type == HCI_GATEWAY_CMD_SEND_IVI){
                u8 *p_ivi = pu+2;
				LOG_MSG_INFO(TL_LOG_IV_UPDATE,p_ivi, 4,"gateway dongle report IVI: \r\n", 0);
                #if JSON_FILE_ENABLE
                mesh_json_update_ivi_index(p_ivi);
                #endif
				m_proDlg->update_ivi_edit(p_ivi);
			}else if (type == HCI_GATEWAY_CMD_SEND_EXTEND_ADV_OPTION) {
			    u8 option_val = pu[2];
                LOG_MSG_INFO(TL_LOG_GATEWAY,0, 0,"the gateway Extend Adv option is:%d \r\n", pu[2]);
                if((option_val != m_combo_extend_adv_option.GetCurSel()) && option_val < EXTEND_ADV_OPTION_MAX){
                    m_ExtendAdvOption = option_val;
					m_combo_extend_adv_option.SetCurSel(option_val); // revert UI
					SaveExtendAdvOption(option_val);
                    LOG_MSG_ERR(TL_LOG_GATEWAY,0, 0,"set Extend Adv option failed \r\n", 0);
                }
			}else if (type == HCI_GATEWAY_CMD_SEND_SRC_CMD){
                gateway_upload_mesh_src_t *p_cmd = (gateway_upload_mesh_src_t *)(pu+2);
                vc_reliable.mat.op = p_cmd->op;
                vc_reliable.mat.adr_src = p_cmd->src;
                memcpy(vc_reliable.ac_par,p_cmd->ac_par,sizeof(p_cmd->ac_par));
			}else if (type == HCI_GATEWAY_CMD_SEND){
			    prov_print_gateway_log(1,pu[2],pu+2,rsp_len);
			}else if (type == HCI_GATEWAY_DEV_RSP){
			    prov_print_gateway_log(0,pu[2],pu+2,rsp_len);
				if (pu[2] == PRO_CAPABLI){

					mesh_prov_oob_str *p_capability = (mesh_prov_oob_str *)(pu + 2);
					vc_node_ele_cnt = p_capability->capa.ele_num;
					if(p_capability->capa.sta_oob.sta_oob_en){
						u8 static_oob[16];
						get_auth_value_by_uuid(sigmesh_node_uuid, static_oob);
						// send the cmd into the dongle
						unsigned char gateway_buf[64];
						gateway_buf[0] =  HCI_CMD_GATEWAY_CTL & 0xFF;
						gateway_buf[1] = (HCI_CMD_GATEWAY_CTL >> 8) & 0xFF;
						gateway_buf[2] = HCI_GATEWAY_CMD_STATIC_OOB_RSP;
						memcpy(gateway_buf+3,static_oob,16 );
						WriteFile_host_handle(gateway_buf ,19);	
					}
				}
			}else if (type == HCI_GATEWAY_CMD_LINK_OPEN){
                LOG_MSG_INFO(TL_LOG_GATEWAY,pu+2, rsp_len,"the node link open cmd is \r\n");
			}else if (type == HCI_GATEWAY_CMD_LINK_CLS){
                LOG_MSG_INFO(TL_LOG_GATEWAY,pu+2, rsp_len,"the node link close cmd is \r\n");
			}else if (type == HCI_GATEWAY_CMD_HEARTBEAT){
				u16 src = pu[2] + (pu[3]<<8);
				u16 dst = pu[4] + (pu[5]<<8);
                LOG_MSG_INFO(TL_LOG_GATEWAY,pu+6, rsp_len-4," src:%04x dst:%04x heart beat", src, dst);
			}else if (type == HCI_GATEWAY_CMD_SEND_BACK_VC){
				gateway_upload_mesh_cmd_str *p_gate_cmd = (gateway_upload_mesh_cmd_str *)(pu+2);
				vc_reliable.mat.op = p_gate_cmd->opcode;
				vc_reliable.mat.adr_src = p_gate_cmd->src;
				vc_reliable.mat.adr_dst = p_gate_cmd->dst;
				memcpy(vc_reliable.ac_par,p_gate_cmd->para,sizeof(p_gate_cmd->para));
				LOG_MSG_INFO(TL_LOG_GATEWAY,p_gate_cmd->para, rsp_len-6,
				"cmd sendback src:%04x dst:%04x,op %04x(%s)", p_gate_cmd->src,p_gate_cmd->dst,p_gate_cmd->opcode,get_op_string(p_gate_cmd->opcode,0));
			}else if (type == HCI_GATEWAY_CMD_LOG_BUF){
				char *p_buf = (char *)(pu+2);
				gateway_rsp_len = rsp_len;
				memcpy(gateway_rsp_buf,p_buf,gateway_rsp_len);
			}else if (type == HCI_GATEWAY_CMD_LOG_STRING){
				char *p_buf = (char *)(pu+2);
				p_buf[rsp_len]='\0';
				LOG_MSG_INFO(TL_LOG_GW_VC_LOG,gateway_rsp_buf,gateway_rsp_len,"%s",p_buf);
			}else if (type == HCI_GATEWAY_CMD_RP_SCAN_REPORT_RSP){
				LOG_MSG_INFO(TL_LOG_GW_VC_LOG,gateway_rsp_buf,gateway_rsp_len,"get the scan report",0);
				remote_prov_scan_report_cb(pu+2,(u8)rsp_len);
			}
			else if (type == HCI_GATEWAY_CMD_ONLINE_ST) {
				if(0 == rsp_len){
					AfxMessageBox("gateway Not support online status");
				}else{
					online_st_report_t* p_st_report = (online_st_report_t*)(pu + 2);
					if (MESH_ADV_TYPE_ONLINE_ST == p_st_report->type) {
						update_online_st_pkt((u8*)p_st_report, rsp_len - ONLINE_ST_MIC_LEN_GATT);
					}
				}
			}
			return 0;
		}
		if(connect_flag)
		{				
			if(pu[0]==DONGLE_REPORT_SPP_DATA){
#if 0
				provision_dispatch_pkt(pu+2,pu[1]);
#else
                u8 buff_bear[MESH_BEAR_SIZE];
                mesh_cmd_bear_t *p_bear_rx = (mesh_cmd_bear_t *)buff_bear;
				ret_type = provision_dispatch_direct(pu+2,pu[1],proxy_buf,&proxy_len);
				LOG_MSG_WIN32_FILE(TL_LOG_COMMON, pu+2, pu[1], "ret type is %d",ret_type);
				if(ret_type == MSG_PROVISION_PDU){
					gatt_rcv_pro_pkt_dispatch(proxy_buf,(u8)proxy_len);
				}else if(ret_type == MSG_NETWORK_PDU){
					LOG_MSG_WIN32_FILE(TL_LOG_COMMON, proxy_buf, proxy_len, "gatt proxy data whole packet is ");
					mesh_construct_adv_bear_with_nw((u8 *)p_bear_rx, proxy_buf, (u8)proxy_len);
					app_event_handler_adv(&p_bear_rx->len, MESH_BEAR_GATT, 0);

					// LogMsg("VC RX NETWORK_PDU in connect mode: %s\r\n", buff);
				}else if(ret_type == MSG_MESH_BEACON){
					mesh_construct_adv_bear_with_bc((u8 *)p_bear_rx, proxy_buf, (u8)proxy_len);
					if(UNPROVISION_BEACON == p_bear_rx->beacon.type){
						check_pkt_is_unprovision_beacon(proxy_buf);
					}else if(SECURE_BEACON == p_bear_rx->beacon.type){
						mesh_rc_data_beacon_sec(&(p_bear_rx->len), 0);
						if(filter_send_flag){
							mesh_proxy_set_filter_init(ele_adr_primary);
							filter_send_flag = 0;
						}
					}else if(PRIVACY_BEACON == p_bear_rx->beacon.type){
						#if MD_PRIVACY_BEA
						mesh_rc_data_beacon_privacy(&(p_bear_rx->len), 0);
						#endif
					}
					// LOG_MSG_INFO (TL_LOG_CMD_RSP, 0, 0, "VC RX BEACON in connect mode: %s", buff);
				}else if((ret_type == MSG_PROXY_CONFIG)){
					//m_proDlg->SetFilterSts(pu+2,pu[1]);
					//get the result of the cmd_bear
					u8 proxy_buf_tmp[MAX_PROXY_BUF_LEN+3];
					mesh_cmd_bear_t *p_bear = (mesh_cmd_bear_t *)proxy_buf_tmp;
					p_bear->trans_par_val=TRANSMIT_DEF_PAR;
					p_bear->len=proxy_len+1;
					p_bear->type=MESH_ADV_TYPE_MESSAGE;
					
					memcpy(&p_bear->nw,proxy_buf,proxy_len);
					int err = mesh_rc_data_cfg_gatt((u8 *)p_bear);
					proxy_config_pdu_sr *p_proxy_str;
    				p_proxy_str = (proxy_config_pdu_sr *)(p_bear->nw.data);
					if(err){
    					LOG_MSG_INFO(TL_LOG_WIN32,0,0," list status decryption err, may be iv index error");
					}else if(p_proxy_str->opcode == PROXY_FILTER_STATUS){   					
    					if(p_proxy_str->para[0] == FILTER_WHITE_LIST){
    						LOG_MSG_INFO(TL_LOG_WIN32,0,0," white list");
                            u16 cnt = p_proxy_str->para[1]*256 + p_proxy_str->para[2];//
                            if(cnt > 0){
                                pair_login_ok = 1;
                            }
    					}else if(p_proxy_str->para[0] == FILTER_BLACK_LIST){
    						LOG_MSG_INFO(TL_LOG_WIN32,0,0," black list");
                            pair_login_ok = 1;

    					}
    					// get the length of the white list 
    					int proxy_len;
    					/*******************************************************************
    					      (adv_type(1)+nid(1)+ttl(1)+sno(3)+src(2)+dst(2)+op(1)+filter_type(1))+enc(8)=20
    					      ***********/
    					proxy_len = p_bear->len - 20;
    					connect_addr_gatt = p_bear->nw.src;
    					int list_size = p_proxy_str->para[1]*256 + p_proxy_str->para[2];
    					LOG_MSG_INFO(TL_LOG_WIN32,0,0,"GATT addr 0x%04x, filter list status, ListSize is: %d", connect_addr_gatt, list_size);
    					//LOG_MSG_INFO (TL_LOG_CMD_RSP, 0, 0, "VC RX PROXY_CONFIG in connect mode: %s", buff);
					}else if(p_proxy_str->opcode == DIRECTED_PROXY_CAPA_STATUS){
						LOG_MSG_INFO(TL_LOG_WIN32,p_proxy_str->para,2,"DIRECTED_PROXY_CAPABILITIES_STATUS:", 0);
						directed_proxy_capa_sts *p_capa_sts = (directed_proxy_capa_sts *)p_proxy_str->para;
						if(p_capa_sts->directed_proxy && p_capa_sts->use_directed != m_use_directed){
							mesh_directed_proxy_control_set(m_use_directed, ele_adr_primary, g_ele_cnt);
						}
					}
				}	
#endif
			}
			else if (pu[0]==DONGLE_REPORT_PROVISION_UUID)
			{
				BYTE tmp1[20];
				//m_Log.SetWindowText("");
				LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "Mesh Provisioning Service:");
				Bin2Text (buff, pu+2, pu[1]-1);
				LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "uuid:%s", buff);
				Bin2Text (tmp1, pu+4, 1);
				LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "provision out handle:%s", tmp1);
				//set the last byte for the handle 
				if(pu[1]==4)// cmd_len is 4,means it's new version dongle.
				{
					provision_write_handle = pu[5];
					LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "provision write handle is %x", provision_write_handle);
				}else{
					provision_write_handle = pu[4]+4;
				}
				
				read_version_rsp();
			}
			
			else if (pu[0]==DONGLE_REPORT_PROXY_UUID)
			{
				BYTE tmp1[20];
				LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "Mesh Proxy Service:");
				Bin2Text (buff, pu+2, pu[1]-1);
				LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "uuid:%s", buff);
				Bin2Text (tmp1, pu+4, 1);
				LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "proxy out handle is :%s", tmp1);
				//set the last byte for the handle 
				if(pu[1]==4)// cmd_len is 4,means it's new version dongle.
				{
					proxy_write_handle = pu[5];
					LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "proxy_write_handle is %x", proxy_write_handle);
				}else{
					proxy_write_handle = pu[4]+4;
				}
			}
			else if (pu[0]==DONGLE_REPROT_READ_RSP){
				u8  fwRevision_value [FW_REVISION_VALUE_LEN]={0};
				memcpy(fwRevision_value,pu+2,pu[1]);
				LOG_MSG_INFO (TL_LOG_COMMON, fwRevision_value, 
					sizeof(fwRevision_value), "slave fw version is ", 0);
				u8 auth_en;
				auth_en = fwRevision_value[FW_REVISION_VALUE_LEN-2];
			}
			else if (pu[0]==DONGLE_REPORT_ONLINE_ST_UUID)
			{
				online_st_write_handle = pu[2];
				//LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "online status handle:%d", online_st_write_handle);
			}
			else if (pu[0]==DONGLE_REPORT_ONLINE_ST_DATA)
			{
			    online_st_report_t *p_st_report = (online_st_report_t *)(pu+2);
			    if(MESH_ADV_TYPE_ONLINE_ST == p_st_report->type){
			        if(pu[1] > (sizeof(online_st_report_t)-sizeof(p_st_report->node))){
    			        if(0 == online_st_gatt_dec((u8 *)p_st_report, pu[1])){
            			    update_online_st_pkt((u8 *)p_st_report, pu[1]-ONLINE_ST_MIC_LEN_GATT);
            				// LOG_MSG_INFO (TL_LOG_COMMON, pu+2, pu[1], "online status data:", 0);
        				}
    				}
				}
			}
			else if(DONGLE_REPORT_ATT_MTU == type)
			{
				att_mtu = pu[2] + (pu[3]<<8);
				if(att_mtu>54){ 
					att_mtu = 54;
				}
			}
			else if (MESH_CMD_RSP == type)
			{
				VC_mesh_cmd_rsp_handle(pu);
			}
		}else if(pu[0]==TSCRIPT_CMD_VC_DEBUG){
			mesh_provision_rcv_process(pu+2,0);
			LOG_MSG_INFO (TL_LOG_CMD_RSP, 0, 0, "tscript cmd vc debug: %s", buff);
		}
		else{
			if(m_adv_mode){
				 
			}else{
			}

			if(MESH_CMD_RSP == type){
				VC_mesh_cmd_rsp_handle(pu);
			}else{
				LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "adv pkt: %s", buff);
			}
		}
	}
#endif
	//Bin2Text (buff, pu+1, pu[0]);
	
	offset = pu[0] + 1;

	n -= offset;
	pu += offset;
#endif

	return 0;
}

int OnAppendLog_vs(unsigned char *p, int len)
{
	//(WPARAM wParam, LPARAM lParam )
	return g_module_dlg->OnAppendLogHandle((WPARAM)len, (LPARAM)p);
}

void CTl_ble_moduleDlg::OnLogclear() 
{
	// TODO: Add your control notification handler code here
	log_sno = 0;
	m_Log.SetWindowText ("");	
}

void CTl_ble_moduleDlg::set_window_text(CString text)
{
	CString version;
	u8 v1 = SW_VERSION_SPEC;
	u8 v2 = SW_VERSION_MAJOR;
	u8 v3 = SW_VERSION_MINOR;
	u8 v4 = SW_VERSION_2ND_MINOR;
	#if (DRAFT_FEATURE_VENDOR_TYPE_SEL != DRAFT_FEATURE_VENDOR_TYPE_NONE)
	version.Format("    V%X.%X.%X.%X   vendor OTA mode", v1, v2, v3, v4);
	#else
	version.Format("    V%X.%X.%X.%X", v1, v2, v3, v4);
	#endif
	SetWindowText(text + version);
}

// push the beacon data into the fifo 
void CTl_ble_moduleDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	
	// mesh_kr_cfgcl_proc();
	
	if (nIDEvent == 1) {
		if (m_hDev != NULL) {
			if (1 || !device_ok) {
				device_ok = 1;
				if(IsMasterMode()){
					set_window_text ("Telink master -- Found");
					// Todo:
					static BOOL startup_init = FALSE;
					if (!startup_init) {
						startup_init = TRUE;

						// Stop
						clear_all_white_list();
						disconnect_device();
						connect_flag = pair_login_ok = 0;

						// Start scan.
						report_adv_opera(1);

					}

				}else{
					set_window_text ("Telink sig_mesh -- Found");
				}
				((CListBox  *) GetDlgItem (IDC_COMMAND))->EnableWindow (1);

			}
		}
		else {
			if (1 || device_ok) {
				device_ok = 0;
				if(IsMasterMode()){
					set_window_text ("Telink master -- Not Found");
				}else{
					set_window_text ("Telink sig_mesh -- Not Found");
				}
			//	((CListBox  *) GetDlgItem (IDC_COMMAND))->EnableWindow (0);
				UpdateData (FALSE);
			}
		}

	}else if(nIDEvent == 2){
		//
	}else if (nIDEvent == TIMER_APPEND_LOG) {
		append_log();
	}
	CDialog::OnTimer(nIDEvent);
}


void CTl_ble_moduleDlg::SetHostInterface() 
{
	UpdateData ();
//	int dat = 0x80 | (m_bif_poll << 6) | m_bif_spi | (m_i2c_id<<8);
//	WriteMem (m_hDevW, 0x8008, (LPBYTE) &dat, 2, 2);
	((CListBox  *) GetDlgItem (IDC_COMMAND))->EnableWindow (1);
}

unsigned	char buff_mac[] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned	char buff_start[] = {0x02, 0x00};
unsigned	char buff_stop[] = {0x03, 0x00};
unsigned	char buff_conn[16] = {0x04, 38, 0x06, 0x00, 0x00, 0x04, 0xff, 0xff, 0xff, 0xff, 0x1f};


void CTl_ble_moduleDlg::OnScan() 
{
	// TODO: Add your control notification handler code here

	
		UpdateData ();
		int ns = ((CComboBox *) GetDlgItem (IDC_CHANNEL))->GetCurSel ();
		buff_conn[1] = 37 + ns;
		buff_conn[2] = m_conn_interval;
		buff_conn[3] = m_conn_interval >> 8;
		buff_conn[4] = m_conn_timeout;
		buff_conn[5] = m_conn_timeout >> 8;
		
		char buff_mask[32];
		sprintf_s (buff_mask, sizeof(buff_mask), "%s", (const char*)m_conn_chnmask);
		Text2Bin (buff_conn + 6, (LPBYTE) buff_mask);
		
		fifo_push_vc_cmd2dongle_usb(buff_mac, 6);
		WaitForSingleObject (NULLEVENT2, 10);
		
		fifo_push_vc_cmd2dongle_usb(buff_conn, 11);
		WaitForSingleObject (NULLEVENT2, 10);
		
		fifo_push_vc_cmd2dongle_usb(buff_start, 1);

		m_pScanDlg->ShowWindow(SW_NORMAL);
}

void CTl_ble_moduleDlg::OnStop() 
{
	// TODO: Add your control notification handler code here
	fifo_push_vc_cmd2dongle_usb(buff_stop, 1);
	WaitForSingleObject (NULLEVENT2, 10);
	m_status = 0;	
}

void mesh_proxy_master_terminate_cmd()
{
	fifo_push_vc_cmd2dongle_usb(buff_stop, 1);
}

void CTl_ble_moduleDlg::OnLogsave() 
{
	// TODO: Add your control notification handler code here

#if DEBUG_IV_UPDATE_TEST_EN
    iv_index_read_print_test();
    
    return ;
#endif

		UpdateData ();
	short kc = GetKeyState(VK_LCONTROL) >> 8;
	short ks = GetKeyState(VK_LSHIFT) >> 8;

	char cmd[1024];
	int ns = ((CListBox *) GetDlgItem (IDC_COMMAND))->GetCurSel ();
	CString str;
	int step = 0;
	if (ns >= 0) {
		((CListBox *) GetDlgItem (IDC_COMMAND))->GetText (ns, cmd);
		
		str = cmd;
		int n = str.Find("__");

		sscanf_s (cmd + n + 2, "%d", &step);
		str = str.Left (n);
		str = str.Right (str.GetLength () - str.Find(" ") - 1);
		str.Replace ('/', '.');
		
		CFile file;
		CFileException fe;

		if (!file.Open(m_CurrentDir + "\\log\\" + str + ".txt", CFile::modeCreate |
		  CFile::modeReadWrite | CFile::shareExclusive, &fe))
		{
			AfxMessageBox ("Cannot open output file");
			return;
		}

		CString text;
		m_Log.GetWindowText(text);
		file.Write(text, text.GetLength());
		file.Close();

	}			


		
}

void CTl_ble_moduleDlg::OnLogsavefile() 
{
	// TODO: Add your control notification handler code here
	static char BASED_CODE szFilter[] = "Log Files (*.txt)|*.txt|All Files (*.*)|*.*||";
	CFileDialog dlg(FALSE, NULL, NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);

 	if (dlg.DoModal()!=IDOK) return;

	CFile file;
	CFileException fe;

	if (!file.Open(dlg.GetPathName(), CFile::modeCreate |
	  CFile::modeReadWrite | CFile::shareExclusive, &fe))
	{
		AfxMessageBox ("Cannot open output file");
		return;
	}

	CString str;
	m_Log.GetWindowText(str);
	file.Write(str, str.GetLength());
	file.Close();		
}

void CTl_ble_moduleDlg::OnLogen() 
{
	// TODO: Add your control notification handler code here
	UpdateData ();
}

void CTl_ble_moduleDlg::OnLogDisable() 
{
	// TODO: Add your control notification handler code here
	m_bLogEn = 0;
	SendMessage(WM_UPDATEDATA, FALSE, NULL);
	AfxMessageBox("Log disable now\r\nMesh OTA flow finished!");
}

/***********************network save part ********************************/
#define MAX_LIGHT_IN_MESH  16
#define MAX_NETWORK_NUM		4
typedef struct{
	u16 unicast_adr;// the main feature of the light in the mesh 
	u8 mac_adr[6];
	u8 soft_ver[4];
	u16 key_index;
	// bright color 
	u8 bright;
	u8 color;
	u16 pub_group[8];
	u16 sub_group[8];
}share_light_str;
typedef struct{
	u8 network_key[16];
	u8 ivi_index[4];
	u8 mesh_node_num;
	share_light_str light_content[MAX_LIGHT_IN_MESH];
}share_network_str;
typedef struct{
	u8 net_mask;
	share_network_str net_str[MAX_NETWORK_NUM];
}share_whole_str;
share_whole_str share_str;
void process_share_str_save(u8 *p,u32 *p_len,share_whole_str *p_str )
{
	u32 tmp_len =0;
	u32 tmp_idx =0;
	u32 i;
	p[0]= p_str->net_mask;
	tmp_idx++;
	for(i=0;i<MAX_NETWORK_NUM;i++){
		if(p_str->net_mask & (1<<i)){
			u8 light_num;
			u32 cpy_cnt;
			share_network_str *p_net;
			p_net = &(p_str->net_str[i]);
			light_num = p_net->mesh_node_num;
			cpy_cnt = OFFSETOF(share_network_str, light_content)+light_num*sizeof(share_light_str);
			memcpy(p+tmp_idx,p_net,cpy_cnt);
			tmp_idx +=cpy_cnt;
		}
	}
	*p_len = tmp_idx ;
	return ;
}
u8  process_share_str_retrieve(u8 *p,u32 len,share_whole_str *p_str)
{
	int i;
	int tmp_idx =0;
	p_str->net_mask = p[0];
	tmp_idx++;
	
	for(i=0;i<MAX_NETWORK_NUM;i++){
		if(p_str->net_mask &(1<<i)){
			u32 cpy_cnt;
			share_network_str *p_net;
			p_net = &(p_str->net_str[i]);
			cpy_cnt = (p_net->mesh_node_num) *sizeof(share_light_str)+OFFSETOF(share_network_str, light_content);
			if(tmp_idx + cpy_cnt > len){
				return 0;
			}
			memcpy(p_net,p+tmp_idx,cpy_cnt);
			tmp_idx +=cpy_cnt;
		}
	}
	return 1;
}

unsigned char SendPktAttOp(unsigned char att_op,unsigned short handle,unsigned char *p ,unsigned char  len)
{
	unsigned char		pair_packet[256] = {0x02,0x00,0x20,0x08,0x00,0x04,0x00,0x04,0x00,0x52,0x1e,0x00,0x00};

    if(!handle){
        return 0;
    }
    
	// packet format 
	pair_packet[3] = len+7;
	pair_packet[5] = len+3; 
	pair_packet[9] = att_op; 
	pair_packet[10]= (unsigned char)(handle);
	memcpy(pair_packet+12,p,len);
	int bok = fifo_push_vc_cmd2dongle_usb(pair_packet, pair_packet[3]+5);
	WaitForSingleObject (NULLEVENT2, 10);
	return 1;
}

void read_version_rsp()
{
	#define ATT_VERSION_IDX		12
	SendPktAttOp(ATT_OP_READ_REQ,ATT_VERSION_IDX,0,0);
	LOG_MSG_INFO(TL_LOG_WIN32,0,0,"read rsp",0);
}

unsigned char SendPkt(unsigned short handle,unsigned char *p ,unsigned char  len)
{
	LOG_MSG_WIN32_FILE(TL_LOG_COMMON,(u8 *)p, len,"write data hanle is %d   ",handle);
	return SendPktAttOp(ATT_OP_WRITE_CMD, handle, p , len);
}

unsigned char SendPktOnlineStSet(unsigned char *p ,unsigned char  len)
{
	if (ble_moudle_id_is_kmadongle()) {
		if (!online_st_write_handle) {
			AfxMessageBox("Not support online status UUID");
			return 0;
		}
		return SendPktAttOp(ATT_OP_WRITE_REQ, online_st_write_handle, p, len);
	}
	else {
		unsigned char online_set[] = { (u8)HCI_CMD_GATEWAY_ONLINE_SET, HCI_CMD_GATEWAY_ONLINE_SET>>8, 1};
		WriteFile_host_handle(online_set, sizeof(online_set));
		return 1;
	}
}

unsigned char SendPktOnlineStOn()
{
    u8 val = 1; // enable online status
    return SendPktOnlineStSet(&val,1);
}

void set_provision_out_ccc()
{
	unsigned char set_para[2]={0x00,0x01};
	SendPkt(provision_write_handle-2,set_para,2);
	return;

}
void report_adv_opera(unsigned char dat)
{
	unsigned char report_adv_cmd[6]={0x01,0x0c,0x20,0x02,0x00,0x00};
	report_adv_cmd[4]=dat;
	int bok = fifo_push_vc_cmd2dongle_usb(report_adv_cmd, sizeof(report_adv_cmd));
	adv_enable = dat;
	WaitForSingleObject (NULLEVENT2, 10);
	return ;
}
void clear_all_white_list()
{
	unsigned char clear_cmd[6]={0x01,0x10,0x20,0x02,0x00,0x00}; 
	int bok = fifo_push_vc_cmd2dongle_usb(clear_cmd, sizeof(clear_cmd));
	WaitForSingleObject (NULLEVENT2, 10);
	return ;
}
void disconnect_device()
{
	unsigned char disconnect[7]={0x01,0x06,0x04,0x03,0x80,0x00,0x13}; 
	int bok = fifo_push_vc_cmd2dongle_usb(disconnect, sizeof(disconnect));
	WaitForSingleObject (NULLEVENT2, 10);
	return ;
}
void set_adv_chn_device(u8 para)
{
	unsigned char adv_chn_para[5]={0x01,0x20,0x04,0x01,0x00}; 
	adv_chn_para[4] = para;
	int bok = fifo_push_vc_cmd2dongle_usb(adv_chn_para, sizeof(adv_chn_para));
	WaitForSingleObject (NULLEVENT2, 10);
	return ;
}

void set_special_white_list(unsigned char  *pmac)
{
	unsigned char add_whitelist[11]={0x01,0x11,0x20,0x07,0x00,0x11,0x22,0x33,0x44,0x55,0x66};
	memcpy(add_whitelist+5,pmac,6);
	int bok = fifo_push_vc_cmd2dongle_usb(add_whitelist, sizeof(add_whitelist));
	unsigned char create_connection[0x1d] ={0x01,0x0d,0x20,0x19,0x30,0x00,0x20,0x00,   0x00,0x00,0xf3,0xe1,0xe2,0xe3,0xe4,0xc7,
										  0x00,0x20,0x00,0x30,0x00,0x00,0x00,0x80,   0x00,0x00,0x00,0x00,0x00};
	memcpy(create_connection+10,pmac,6);
	bok = fifo_push_vc_cmd2dongle_usb(create_connection, sizeof(create_connection));
	WaitForSingleObject (NULLEVENT2, 10);

	return ;

}
void CTl_ble_moduleDlg::OnBulkout() 
{
	// TODO: Add your control notification handler code here

	//UpdateData ();
	CString str;
	m_textBO.GetWindowText(str);
	
	DWORD nB;
	BYTE   buff[32*1024];
	nB = str.GetLength();
	if (nB > 32 * 1024)
		nB = 32 * 1024;
	memcpy (buff, str, nB);
	if(connect_serial_port){
		write_file_uart(buff,nB);
	}else{
		int bok =  WriteFile(m_hDevW, buff, nB, &nB, NULL);
	}

}



void CTl_ble_moduleDlg::OnCmdsel() 
{
	// TODO: Add your control notification handler code here
	static int bcmd = 0;
	bcmd = !bcmd;
	if (bcmd)
	{
		m_textBO.ShowWindow(FALSE);
		((CListBox *) GetDlgItem (IDC_COMMAND))->ShowWindow(TRUE);
		GetDlgItem (IDC_CMDSEL)->SetWindowText("CMD");
	}
	else
	{
		m_textBO.ShowWindow(TRUE);
		((CListBox *) GetDlgItem (IDC_COMMAND))->ShowWindow(FALSE);
		GetDlgItem (IDC_CMDSEL)->SetWindowText("ASCII");
	}
}

void CTl_ble_moduleDlg::OnDevice() 
{
	// TODO: Add your control notification handler code here

	if(!device_ok&&!connect_serial_port){
		AfxMessageBox("DEVICE NOT FOUND");
		return ;
	}
	memset((u8 *)&gatt_mac_list,0,sizeof(gatt_mac_list));
	set_node_prov_mode(PROV_DIRECT_MODE);
	gateway_set_rp_mode(0);
	//DWORD nB;
	if(ble_moudle_id_is_gateway()){
		gateway_VC_provision_start();
	}else{
		report_adv_opera(1);
	}
	{
		UpdateData ();

		m_pScanDlg->ShowWindow (SW_SHOW);
		
		//memset((u8 *)(&gatt_mac_list),0,sizeof(gatt_mac_list_str));
	}
}

void CTl_ble_moduleDlg::OnProvision() 
{
	// TODO: Add your control notification handler code here
	// add the timer part test 
	// test the bin file operation
	// suppose the data is about 64k
	
	m_proDlg->ShowWindow(SW_SHOW);
	WaitForSingleObject (NULLEVENT2, 300);
	m_proDlg->SetButton_sts();
	if(ble_moudle_id_is_gateway()){
		gateway_get_provision_self_sts();
		UpdateData(FALSE);
	}
	
}

void CTl_ble_moduleDlg::OnScanStop() 
{
	// TODO: Add your control notification handler code here
	set_node_prov_mode(PROV_DIRECT_MODE);
	if(ble_moudle_id_is_gateway()){
		gateway_VC_provision_stop();
	}else{
		clear_all_white_list();
		disconnect_device();
		connect_flag = pair_login_ok = 0;
		report_adv_opera(0);
	}
}

void CTl_ble_moduleDlg::OnMeshDisplay() 
{
	// TODO: Add your control notification handler code here

	if(!device_and_connect_check_valid()){
		return ;
	}

	int sel = GetPrivateProfileInt ("SET", "crypto_poly", 0, m_InitFile);
	//aes_att_set_crypto_poly (sel);
	g_module_dlg->mLite = m_pMeshDlg->mLite = sel;
	m_pMeshDlg->ShowWindow(SW_SHOW);
	// update status 
	m_pMeshDlg->update_mesh_sts();

}
BOOL CTl_ble_moduleDlg::PreTranslateMessage(MSG* pMsg)  
{   
	UpdateData();

	if (NULL != m_tooltip.GetSafeHwnd())
	{
		m_tooltip.RelayEvent(pMsg);
	}

	if(pMsg == NULL){
		return FALSE;
	}
	if (pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_ESCAPE)// 屏蔽esc键  
	{
		return TRUE;// 不作任何处理  
	}
    if (pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN)// 屏蔽enter键  
    {  
		if (GetFocus() == GetDlgItem(IDC_CMDINPUT)) //根据不同控件焦点判断是那个在执行 
		{
			
			CString csCmdinput;
			CEdit * p_ce;
			p_ce = (CEdit *)GetDlgItem(IDC_CMDINPUT);
			UpdateData (TRUE);
			char tmp_str[4096];
			unsigned int  Cmd_len =0;
			p_ce->GetWindowTextA(csCmdinput);
			sprintf_s ((char *)tmp_str, sizeof(tmp_str), "%s", (const char*)csCmdinput);
			ExecCmd(tmp_str, 0);
			
		} 
        return TRUE;// 不作任何处理  
    }  
	if (pMsg->message == WM_MOUSEMOVE)
	m_tip.RelayEvent(pMsg);  
    return CDialog::PreTranslateMessage(pMsg);  
}

u8 log_en_lpn1 = 1;
u8 log_en_lpn2 = 1;

void CTl_ble_moduleDlg::OnBnClickedOta()
{
	// TODO: 在此添加控件通知处理程序代码
	if(!device_and_connect_check_valid()){
		return ;
	}

    #if DEBUG_IV_UPDATE_TEST_EN
    iv_index_set_sno_test();
	return ;
	#elif DEBUG_PROXY_FRIEND_SHIP
	log_en_lpn1 = !log_en_lpn1;
	return ;
	#endif

	unsigned char ota_char[4]={0x05,0x00,0x00,0x00}; 
	int bok = fifo_push_vc_cmd2dongle_usb(ota_char, sizeof(ota_char));
	WaitForSingleObject (NULLEVENT2, 100);
	return ;
}


void CTl_ble_moduleDlg::OnBnClickedRxTest()
{
	// TODO: 在此添加控件通知处理程序代码
	if(!device_and_connect_check_valid()){
		return ;
	}

    #if DEBUG_IV_UPDATE_TEST_EN
    iv_index_set_keep_time_test();
	return ;
	#elif DEBUG_PROXY_FRIEND_SHIP
	log_en_lpn2 = !log_en_lpn2;
	return ;
	#endif
	
	m_pRxTestDlg->ShowWindow(SW_SHOW);
}


int mesh_set_prov_cloud_para(u8 *p_pid,u8 *p_mac)
{
	return 1;
}

int mesh_sec_prov_cloud_comfirm(u8* p_comfirm,u8 *p_comfirm_key,u8 *p_random)
{
	return 1;
}


int mesh_cloud_dev_comfirm_check(u8 *p_comfirm_key,u8* p_dev_random,u8*p_dev_comfirm)
{
	return 1;
}

// use the pid and mac to caculate the sha256 result ,and return the oob part 
void caculate_sha256_oob_by_para_tab(u8 *pid,u8 *p_mac,u8 *p_secret,u8 sha256_out[32])
{
	char sha256_in[54];
	create_sha256_input_string(sha256_in,pid,p_mac,p_secret);
	mbedtls_sha256((u8 *)sha256_in,sizeof(sha256_in),sha256_out,0);
}

int get_auth_value_by_uuid(u8 *uuid_in,u8 *oob_out)
{
	FILE * f_three_par;
	int err = -1;
	int line_index = 0;
	char three_par[256]={0};
	u8 pid_tmp[4*2+1];
	u32 pid;
	u8 mac[6],mac_tmp[6*2+1];
	u8 secret[16],secret_tmp[16*2+1];
	u8 uuid[16] = {0},uuid_tmp[16*2+80] = {0};
	u8 oob[16] = {0},oob_tmp[16*2+80] = {0};

	sha256_dev_uuid_str *p_ais_uuid = (sha256_dev_uuid_str *)uuid_in;
	if(p_ais_uuid->cid == SHA256_BLE_MESH_PID){
		if(fopen_s(&f_three_par,"three_para.TXT","r") == 0){
			while(1){
				fgets(three_par, 64, f_three_par);	
				if(sscanf_s( three_par, "%8[0-9]%*[^0-9a-f]%[0-9a-f]%*[^0-9a-f]%[0-9a-f]",pid_tmp, sizeof(pid_tmp), secret_tmp, sizeof(secret_tmp), mac_tmp, sizeof(mac_tmp)) == 3)
				{
					pid = atoi((char *)pid_tmp);
					Text2Bin(secret, secret_tmp);
					Text2Bin(mac, mac_tmp);
					u8 tmp_mac[6];
					swap48(tmp_mac, mac);
					if(!memcmp(p_ais_uuid->mac,tmp_mac,6)){
						u8 sha256_out[32];
						caculate_sha256_oob_by_para_tab((u8 *)&pid, mac, secret, sha256_out);
						memcpy(oob_out, sha256_out, 16);
						err = 0;
						break;
					}
				}
				if(feof(f_three_par)) break;
			}
			fclose(f_three_par);
		}
	}
	else{
		if(fopen_s(&f_three_par,"oob_database.TXT","r") == 0){
		while(1){
			fgets(three_par, sizeof(three_par), f_three_par);	
			if(sscanf_s( three_par, "%[0-9a-f]%*[^0-9a-f]%[0-9a-f]", uuid_tmp, sizeof(uuid_tmp), oob_tmp, sizeof(oob_tmp)) == 2)
			{
				Text2Bin(uuid, uuid_tmp);
				Text2Bin(oob, oob_tmp);

				if(!memcmp(uuid_in,uuid,16)){
				    memcpy(oob_out, oob, 16);
				    err = 0;
					break;
				}
			}
			if(feof(f_three_par)) break;
		}
		fclose(f_three_par);
	}
	}
	
	return err;
}


void CTl_ble_moduleDlg::OnBnClickedSetRetry()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	
	CEdit *ce_retry = (CEdit*)GetDlgItem(IDC_EDIT_RETRY);
	CString str;
	ce_retry->GetWindowText(str);
	m_retry_cnt = _tstoi(str);
	g_reliable_retry_cnt_def = m_retry_cnt;     // sync
	if(g_reliable_retry_cnt_def > RELIABLE_RETRY_CNT_MAX){
        g_reliable_retry_cnt_def = m_retry_cnt = RELIABLE_RETRY_CNT_MAX;     // sync
	    AfxMessageBox("max retry cnt is RELIABLE_RETRY_CNT_MAX");
    }
}

void gateway_VC_send_cmd(u8 cmd, u8 *data, int len)
{
	if (len > 64-3) {
		return;
	}
	u8 buff[64] = {0xe9, 0xff};
	buff[2] = cmd;
	memcpy(&buff[3], data, len);
	WriteFile_host_handle(buff, len+3);
}

// gateway opertaion function part 
void gateway_VC_provision_start()
{
	u8 buff[3]={0xe9,0xff};
	buff[2]= HCI_GATEWAY_CMD_START;
	LOG_MSG_INFO (TL_LOG_GATEWAY, buff, sizeof(buff), "HCI_GATEWAY_CMD_START", 0);
	WriteFile_host_handle(buff, sizeof(buff));
}

void gateway_VC_provision_stop()
{
	u8 buff[3]={0xe9,0xff};
	buff[2]= HCI_GATEWAY_CMD_STOP;
	LOG_MSG_INFO (TL_LOG_GATEWAY, buff, sizeof(buff), "HCI_GATEWAY_CMD_STOP", 0);
	WriteFile_host_handle(buff, sizeof(buff));
}

void gateway_VC_factory_reset()
{
    u8 buff[3]={0xe9,0xff};
    buff[2]= HCI_GATEWAY_CMD_RESET;
	LOG_MSG_INFO (TL_LOG_GATEWAY, buff, sizeof(buff), "HCI_GATEWAY_CMD_RESET", 0);
	WriteFile_host_handle(buff, sizeof(buff));
}


unsigned char ble_moudle_id_is_gateway()
{
	if(ID_GATEWAY_VC == dongle_ble_moudle_id && Dongle_is_gateway_or_not()){
		return 1;
	}else{
		return 0;
	}
}

unsigned char ble_moudle_id_is_kmadongle()
{
	if(ID_KMA_DONGLE == dongle_ble_moudle_id ){
		return 1;
	}else{
		return 0;
	}
}

unsigned char Dongle_is_gateway_or_not() 
{
    HANDLE	m_hdongle = NULL;
        // clear the handle of the devices 
    m_hdongle = GetPrintDeviceHandle(ID_GATEWAY_VC);
    if(m_hdongle!=NULL || connect_serial_port){// suppose serial port must be the gateway
        return 1;
    }else{
        return 0;
    }    
}

int is_use_ota_push_mode_for_lpn(fw_distribut_srv_proc_t *distr_proc)
{
	#if DEBUG_SHOW_VC_SELF_EN
	if((1 == distr_proc->node_cnt)&&(ele_adr_primary == distr_proc->list[0].adr)){
		return 1;
	}
	#endif
	
	if(ble_moudle_id_is_kmadongle()){
		if((1 == distr_proc->node_cnt)&&(APP_get_GATT_connect_addr() == distr_proc->list[0].adr)){
			return 1;
		}
	}
	return 0;
}

void gateway_vc_set_adv_filter(unsigned char  *p_mac)
{
	u8 buff[9]={0xe9,0xff,0,0,0,0,0,0,0};
	buff[2]=HCI_GATEWAY_CMD_SET_ADV_FILTER;
	memcpy(buff+3,p_mac,6);
	LOG_MSG_INFO (TL_LOG_GATEWAY, buff, sizeof(buff), "HCI_GATEWAY_CMD_SET_ADV_FILTER", 0);
	WriteFile_host_handle(buff, sizeof(buff));
	return ;
}

void gateway_set_provisioner_para(unsigned char *p_net_info)
{
	u8 buff[28];
	buff[0]=0xe9;
	buff[1]=0xff;
	buff[2]=HCI_GATEWAY_CMD_SET_PRO_PARA;
	memcpy(buff+3,p_net_info,25);
	LOG_MSG_INFO (TL_LOG_GATEWAY, buff, sizeof(buff), "HCI_GATEWAY_CMD_SET_PRO_PARA ", 0);
	WriteFile_host_handle(buff, sizeof(buff));
	return ;
}

void gateway_start_fast_provision(u16 pid, u16 start_addr)
{
	u8 buff[7];
	buff[0]=0xe9;
	buff[1]=0xff;
	buff[2]=HCI_GATEWAY_CMD_FAST_PROV_START;
	buff[3]=(u8)pid;
	buff[4]=pid>>8;
	buff[5]=(u8)start_addr;
	buff[6]=start_addr>>8;
	LOG_MSG_INFO (TL_LOG_GATEWAY, buff, sizeof(buff), "HCI_GATEWAY_CMD_FAST_PROV_START ", 0);
	WriteFile_host_handle(buff, sizeof(buff));
	return ;
}
void gateway_set_prov_devkey(u16 unicast,u8 *p_devkey)
{
	u8 buff[21];
	buff[0]=0xe9;
	buff[1]=0xff;
	buff[2]=HCI_GATEWAY_CMD_SET_DEV_KEY;
	memcpy(buff+3,(u8 *)&unicast,2);
	memcpy(buff+5,p_devkey,16);
	LOG_MSG_INFO (TL_LOG_GATEWAY, buff, sizeof(buff), "HCI_GATEWAY_CMD_SET_DEV_KEY ", 0);
	WriteFile_host_handle(buff, sizeof(buff));
	return ;
}

void gateway_set_node_provision_para(unsigned char *p_net_info)
{
	u8 buff[28];
	buff[0]=0xe9;
	buff[1]=0xff;
	buff[2]=HCI_GATEWAY_CMD_SET_NODE_PARA;
	memcpy(buff+3,p_net_info,25);
	LOG_MSG_INFO (TL_LOG_GATEWAY, buff, sizeof(buff), "HCI_GATEWAY_CMD_SET_NODE_PARA ", 0);
	WriteFile_host_handle(buff, sizeof(buff));
	return ;
}


void gateway_get_dev_uuid_mac()
{
	u8 buff[3];
	buff[0]=0xe9;
	buff[1]=0xff;
	buff[2]=HCI_GATEWAY_CMD_GET_UUID_MAC;
	LOG_MSG_INFO (TL_LOG_GATEWAY, buff, sizeof(buff), "HCI_GATEWAY_CMD_SET_NODE_PARA ", 0);
	WriteFile_host_handle(buff, sizeof(buff));
	return ;
}

void gateway_set_extend_seg_mode(u8 mode)
{
	u8 buff[4];
	buff[0]=0xe9;
	buff[1]=0xff;
	buff[2]=HCI_GATEWAY_CMD_SET_EXTEND_ADV_OPTION;
	buff[3]=mode;
	LOG_MSG_INFO (TL_LOG_GATEWAY, buff, sizeof(buff), "HCI_GATEWAY_CMD_SET_NODE_PARA ", 0);
	WriteFile_host_handle(buff, sizeof(buff));
}

void gateway_send_cmd_to_del_node_info(u16 uni)
{
    u8 buff[5];
	buff[0]=0xe9;
	buff[1]=0xff;
	buff[2]=HCI_GATEWAY_CMD_DEL_VC_NODE_INFO;
	buff[3]=uni&0xff;
	buff[4]=(uni>>8)&0xff;
	LOG_MSG_INFO (TL_LOG_GATEWAY, buff, sizeof(buff), "HCI_GATEWAY_CMD_SET_NODE_PARA ", 0);
	WriteFile_host_handle(buff, sizeof(buff));
	return ;
}

void gateway_set_start_keybind(unsigned char fastbind, unsigned char *p_appid,unsigned char *p_key)
{
	u8 buff[3+sizeof(mesh_gw_appkey_bind_str)];
	buff[0]=0xe9;
	buff[1]=0xff;
	buff[2]=HCI_GATEWAY_CMD_START_KEYBIND;
	mesh_gw_appkey_bind_str *p_keybind = (mesh_gw_appkey_bind_str *)(buff+3); 
	p_keybind->fastbind = fastbind;
	memcpy(p_keybind->key_idx,p_appid,2);
	memcpy(p_keybind->key,p_key,16);
	LOG_MSG_INFO (TL_LOG_GATEWAY, buff, sizeof(buff), "HCI_GATEWAY_CMD_START_KEYBIND ", 0);
	WriteFile_host_handle(buff, sizeof(buff));
	return ;
}

void gateway_get_provision_self_sts()
{
	u8 buff[3];
	buff[0]=0xe9;
	buff[1]=0xff;
	buff[2]=HCI_GATEWAY_CMD_GET_PRO_SELF_STS;
	LOG_MSG_INFO (TL_LOG_GATEWAY, buff, sizeof(buff), "HCI_GATEWAY_CMD_GET_PRO_SELF_STS  ", 0);
	WriteFile_host_handle(buff, sizeof(buff));
	return ;
}

void gateway_set_rp_mode(u8 en)
{
	u8 buff[4];
	buff[0]= 0xe9;
	buff[1]= 0xff;
	buff[2]= HCI_GATEWAY_CMD_RP_MODE_SET;
	buff[3]= en;
	LOG_MSG_INFO (TL_LOG_GATEWAY, buff, sizeof(buff), "gateway_set_rp_mode", 0);
	WriteFile_host_handle(buff, sizeof(buff));
	return ;
}

void gateway_start_scan_start()
{
	u8 buff[3];
	buff[0]=0xe9;
	buff[1]=0xff;
	buff[2]=HCI_GATEWAY_CMD_RP_SCAN_START_SET;
	LOG_MSG_INFO (TL_LOG_GATEWAY, buff, sizeof(buff), "gateway_start_scan_start", 0);
	WriteFile_host_handle(buff, sizeof(buff));
	return ;
}

void gateway_send_link_open(u16 adr,u8 *p_uuid)
{
	u8 buff[21];
	buff[0]=0xe9;
	buff[1]=0xff;
	buff[2]=HCI_GATEWAY_CMD_RP_LINK_OPEN;
	buff[3]=adr&0xff;
	buff[4]=adr>>8;
	memcpy(buff+5,p_uuid,16);
	LOG_MSG_INFO (TL_LOG_GATEWAY, buff, sizeof(buff), "gateway_send_link_open", 0);
	WriteFile_host_handle(buff, sizeof(buff));
}

void gateway_send_rp_start(u8*p_net_info)
{
	u8 buff[28];
	buff[0]=0xe9;
	buff[1]=0xff;
	buff[2]=HCI_GATEWAY_CMD_RP_START;
	memcpy(buff+3,p_net_info,25);
	LOG_MSG_INFO (TL_LOG_GATEWAY, buff, sizeof(buff), "HCI_GATEWAY_CMD_RP_START ", 0);
	WriteFile_host_handle(buff, sizeof(buff));
	return ;
}

void CTl_ble_moduleDlg::OnBnClickedUartMode()
{
	// TODO: 在此添加控件通知处理程序代码
	if(connect_serial_port){
		return ;
	}
	serial_port_ptr = new	CSerialPort(this->m_hWnd,WM_USER+1);
	serial_port_ptr->PortFile=INVALID_HANDLE_VALUE;
	if(serial_port_ptr){
		;
	}
	else{
		MessageBox(_T("fail to create Serial port object"));
		OnCancel();
	}
	connect_serial_port = false;
	p_ComList = (CComboBox *)GetDlgItem(IDC_COMM_COMBO);
    serial_port_ptr->GetComList_Reg(p_ComList);
	
}
int read_file_uart(u8 *p_data,int len )
{
	return (int)serial_port_ptr->ReadBytes(p_data,len);
}
void write_file_uart(u8 *p_data,int len )
{
	serial_port_ptr->WriteBytes(p_data,DWORD (len));
}

void CTl_ble_moduleDlg::OnBnClickedUsbMode()
{
	// TODO: 在此添加控件通知处理程序代码
	if(connect_serial_port){
		serial_port_ptr->Close232Port();
		connect_serial_port = false;
		GetDlgItem(IDC_UART_CONN)->SetWindowText(_T("Connect"));
	}
	m_hDev = GetPrintDeviceHandle(dongle_ble_moudle_id);
	m_hDevW = GetPrintDeviceHandle(dongle_ble_moudle_id);
}


void CTl_ble_moduleDlg::OnBnClickedUartConn()
{
	// TODO: 在此添加控件通知处理程序代码
	CString com_str;
	p_ComList->GetLBText(p_ComList->GetCurSel(), com_str);
	 if(!connect_serial_port){
		if(serial_port_ptr->Open232Port(com_str, CBR_115200, 8, NOPARITY, ONESTOPBIT)){
			connect_serial_port = true;
			init_gateway_para_for_buf();
            GetDlgItem(IDC_UART_CONN)->SetWindowText(_T("Disconnect"));
		}
		else{
            CString tmp;
            tmp.Format(_T("Open %s failed"), com_str);
			MessageBox(tmp);
		}
	}
	else{
		if(serial_port_ptr->Close232Port()){
			connect_serial_port = false;
            GetDlgItem(IDC_UART_CONN)->SetWindowText(_T("Connect"));
		}
	}
	
}
// GATEWAY DEMO 

typedef struct{
	u8  len;
	u16 adr_index;
	u8	data[16];
	u16 crc_16;
}rf_packet_gateway_ota_data_t;

typedef struct{
	u8  len;
	u16 ota_start;
}rf_packet_gateway_start_t;

typedef struct{
	u8 len ;
	u16 ota_end;
	u16 ota_adr;
	u16 ota_adr_rev;
}rf_packet_gateway_end_t;


unsigned short crc16_ota (unsigned char *pD, int len)
{
    static unsigned short poly[2]={0, 0xa001};              //0x8005 <==> 0xa001
    unsigned short crc = 0xffff;
    //unsigned char ds;
    int i,j;

    for(j=len; j>0; j--)
    {
        unsigned char ds = *pD++;
        for(i=0; i<8; i++)
        {
            crc = (crc >> 1) ^ poly[(crc ^ ds ) & 1];
            ds = ds >> 1;
        }
    }

     return crc;
}
void gateway_firmware_send(u8 *buf,int len )
{
	// need to add the header opcode for the gateway ota part 
	if(len>21){
		return ;
	}
	u8 tmp_dat[30];
	tmp_dat[0] = HCI_CMD_GATEWAY_OTA&0xff;
	tmp_dat[1] = (HCI_CMD_GATEWAY_OTA>>8)&0xff;
	memcpy(tmp_dat+2,buf,len);
	WriteFile_host_handle(tmp_dat,len+2);
	WaitForSingleObject (NULLEVENT2, 5);
	return ;
}

void gateway_send_vc_node_info(int index, VC_node_info_t *p_info)
{
    LOG_MSG_INFO (TL_LOG_COMMON, p_info->dev_key, 16, "Send VC node info: Index:%3d Addr: %04X DeviceKey: ", index, p_info->node_adr);
	u8 tmp_dat[23];
	tmp_dat[0]=0xe9;
	tmp_dat[1]=0xff;
	tmp_dat[2]=HCI_GATEWAY_CMD_SEND_VC_NODE_INFO;
	memcpy(tmp_dat+3,(u8*)p_info,20);
	ResetEvent(EVENT_UPDATE_NODE_INFO);
	//WriteFile_host_handle(tmp_dat,sizeof(tmp_dat));
	fifo_push_vc_cmd2dongle_usb(tmp_dat, sizeof(tmp_dat));
	WaitForSingleObject (EVENT_UPDATE_NODE_INFO, 500);
}

void gateway_send_all_vc_node_info()
{
	for(int i=0;i<MESH_NODE_MAX_NUM;i++){
		VC_node_info_t *p_info = (VC_node_info+i);
		if(is_unicast_adr(p_info->node_adr)&&(p_info->node_adr!=0)){
			gateway_send_vc_node_info(i, p_info);
		}else{
			return ;
		}
	}
}

void gateway_send_ota_type(u8 type)
{
	u8 tmp_dat[4];
	tmp_dat[0] = HCI_CMD_MESH_OTA&0xff;
	tmp_dat[1] = (HCI_CMD_MESH_OTA>>8)&0xff;
	tmp_dat[2] = MESH_OTA_SET_TYPE;
	tmp_dat[3] = type;
	WriteFile_host_handle(tmp_dat,4);
	WaitForSingleObject (NULLEVENT2, 40);
}

void gateway_send_ota_erase_ctl()
{
	u8 tmp_dat[3];
	tmp_dat[0] = HCI_CMD_MESH_OTA&0xff;
	tmp_dat[1] = (HCI_CMD_MESH_OTA>>8)&0xff;
	tmp_dat[2] = MESH_OTA_ERASE_CTL;
	WriteFile_host_handle(tmp_dat,3);
	WaitForSingleObject (NULLEVENT2, 40);
}

void download_gateway_send_start()
{
	rf_packet_gateway_start_t ota_start;
	ota_start.len =2;
	ota_start.ota_start = CMD_OTA_START;
	gateway_firmware_send((u8 *)&ota_start,sizeof(ota_start));
	WaitForSingleObject (NULLEVENT2, 5000);// wait for 5s for the gateway can erase the firmware part 
	return ;
}

void download_gateway_send_data(u8 *buf,int len,u32 idx)
{
	rf_packet_gateway_ota_data_t ota_data;
	ota_data.len = 20;
	ota_data.adr_index = (idx>>4);
	if(len == 16){
		memcpy(ota_data.data,buf,len);
	}else if(len<16){
		memset(ota_data.data,0,sizeof(ota_data.data));
		memcpy(ota_data.data,buf,len);
	}else{
		return ;
	}
	//caculate the crc part 
	ota_data.crc_16 = crc16_ota((u8 *)&ota_data.adr_index,18);
	gateway_firmware_send((u8 *)&ota_data,sizeof(ota_data));
	return ;
}

void download_gateway_send_all_data(u8 *buf,int len)
{
	u32 idx =0;
	int total ;
	u8 percent =0;
	total = len;
	while(len){
	    u8 percent_now = (idx*100)/total;
	    if(percent_now>percent){
             percent = percent_now;
             LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "firmware download process is %d percent",percent);
	    }
	   
		if(len>=16){
			download_gateway_send_data(buf+idx,16,idx);
			idx+=16;
			len-=16;
		}else{
			download_gateway_send_data(buf+idx,len,idx);
			idx = 0;
			len = 0;
		}
		MSG msg;
		while(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
			AfxGetApp()->PumpMessage();
		}
	}
	return ;
}

void download_gateway_send_end(u32 ota_adr)
{
	rf_packet_gateway_end_t ota_end;
	ota_end.len = 6;
	ota_end.ota_end = CMD_OTA_END;
	ota_end.ota_adr = (ota_adr>>4);
	ota_end.ota_adr_rev = ~(ota_adr>>4);
	gateway_firmware_send((u8 *)&ota_end,sizeof(ota_end));
	return ;
}

void download_gateway_firmware(u8 *buf,int len)
{
	// send ota start  
	download_gateway_send_start();
	// send ota data  
	download_gateway_send_all_data(buf,len);
	//send  ota end 
	download_gateway_send_end(len);
	LOG_MSG_INFO(TL_LOG_COMMON,0, 0,"gateway firmware load suc");
}

int ota_file_check()
{
	if((ota_filename.GetLength() == 0) && (0 == DEBUG_SHOW_VC_SELF_EN)){  // default bin by qifa
		AfxMessageBox(_T("select the ota firmware file first"));
		return -1;
	}
	return 0;
}

u8 unicast_is_in_range_or_not(u16 unicast,u8 *p_uuid)
{
	if(!json_unicast_is_in_range_or_not(unicast,p_uuid)){
		AfxMessageBox(_T("json_unicast is not in the range "));
		return 0;
	}else{
		return 1;
	}
}

void CTl_ble_moduleDlg::OnBnClickedGatewayOta()
{
	// TODO: 在此添加控件通知处理程序代码
    u8 firmware_buf[FLASH_ADR_AREA_FIRMWARE_END];
    u32 firmware_len;
	if(0 != ota_file_check()){
		return ;
	}

    firmware_len = new_fw_read(firmware_buf, sizeof(firmware_buf));
    if(firmware_len){
    	gateway_send_ota_type(GATEWAY_OTA_SLEF);
    	download_gateway_firmware(firmware_buf,firmware_len);
	}
}


void CTl_ble_moduleDlg::OnBnClickedMeshOta()
{
	// TODO: 在此添加控件通知处理程序代码
    u8 firmware_buf[FLASH_ADR_AREA_FIRMWARE_END];
    u32 firmware_len;
	if(0 != ota_file_check()){
		return ;
	}

    firmware_len = new_fw_read(firmware_buf, sizeof(firmware_buf));
    if(firmware_len){
    	gateway_send_ota_type(GATEWAY_OTA_MESH);
    	download_gateway_firmware(firmware_buf,firmware_len);
	}
}


void CTl_ble_moduleDlg::OnBnClickedGatewatReset()
{
	if(ble_moudle_id_is_gateway()){
        gateway_VC_factory_reset();
		// clear all the json file , and do init part for the gateway part 
		DeleteFile(FILE_MESH_DATA_BASE);
		init_json(FILE_MESH_DATA_BASE,0);
		pro_self_flag =0;
		provison_net_info_str *p_net = &net_info;
		memset(p_net->net_work_key,0,sizeof(p_net->net_work_key));
		p_net->unicast_address = 1;
		gateway_provision_sts =0;
		LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "wait the gateway finish shining,and the gateway will reset",0);
	}else{
		AfxMessageBox(_T("only the gateway will have this problem !"));
	}
	// TODO: 在此添加控件通知处理程序代码
}

CString ota_filename=_T("");

void CTl_ble_moduleDlg::OnBnClickedSearchfile()
{
	// TODO: 在此添加控件通知处理程序代码
	CString gReadFilePathName;  
	CFileDialog fileDlg(true, _T("bin"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("json/bin File(*.json *.bin)|*.json;*.bin|"), NULL);  
	if (fileDlg.DoModal() == IDOK)    //弹出对话框  
	{  
		gReadFilePathName = fileDlg.GetPathName();//得到完整的文件名和目录名拓展名  
		GetDlgItem(IDC_OTA_PATH)->SetWindowText(gReadFilePathName);//将路径显示  
		ota_filename = fileDlg.GetPathName();  
	}  
}



// mesh interface part 
// send multi packets 
u16 att_mtu = 23;
int gatt_write_transaction_callback(u8 *p,u16 len,u8 msg_type)
{
	u8 tmp[256];
	u16 pkt_no=0;
	u16 buf_idx =0;
	u16 total_len;
	u16 vaid_len = att_mtu - 4;// opcode 1 + handle_id 2 + sar 1
	u16 handle = proxy_write_handle;
	if(MSG_PROVISION_PDU == msg_type){
		handle = provision_write_handle;
	}

	total_len =len;
	pb_gatt_proxy_str *p_notify = (pb_gatt_proxy_str *)(tmp);
	provision_flow_Log(0,p,len);
	//can send in one packet
	
	if(len==0){
		return 1;
	}
	if(len>vaid_len){
		while(len){
			if(!pkt_no){
				//send the first pkt
				p_notify->sar = SAR_START;
				p_notify->msgType = msg_type;	
				memcpy(p_notify->data,p,vaid_len);
				SendPkt(handle,tmp,vaid_len+1);
				len = len-vaid_len;
				buf_idx +=vaid_len;
				pkt_no++;
			}else{
				// the last pkt 
				if(buf_idx+vaid_len>=total_len){
					p_notify->sar = SAR_END;
					p_notify->msgType = msg_type;
					memcpy(p_notify->data,p+buf_idx,total_len-buf_idx);
					SendPkt(handle,tmp,(unsigned char)(total_len-buf_idx+1));
					len =0;
				}else{
				// send the continus pkt 
					p_notify->sar = SAR_CONTINUS;
					p_notify->msgType = msg_type;
					memcpy(p_notify->data,p+buf_idx,vaid_len);
					SendPkt(handle,tmp,vaid_len+1);
					len = len-vaid_len;
					buf_idx +=vaid_len;
				}
			}
		}
	}else{
	// send the complete pkt 
		p_notify->sar = SAR_COMPLETE;
		p_notify->msgType = msg_type;
		memcpy(p_notify->data,p,len);
		SendPkt(handle,tmp,(unsigned char)(len+1));
	}
	return 1;
}

void write_no_rsps_pkts(u8 *p,u16 len,u16 handle,u8 msg_type)
{
	gatt_write_transaction_callback(p, len, msg_type);
	return ;
}

void CTl_ble_moduleDlg::OnBnClickedRpScan()
{
	// TODO: 在此添加控件通知处理程序代码
	if(!device_ok&&!connect_serial_port){
		AfxMessageBox("DEVICE NOT FOUND");
		return ;
	}
	set_node_prov_mode(PROV_REMOTE_MODE);
	//DWORD nB;
	if(prov_mode_is_rp_mode()){
		if(ble_moudle_id_is_gateway()){
			// in the gw mode ,need to send the cmd to proc to control the gw
			gateway_set_rp_mode(1);
			gateway_start_scan_start();
		}else{
			u16 adr_list[MESH_OTA_UPDATE_NODE_MAX] = {0};
			u32 update_node_cnt=0;
			update_node_cnt = m_pMeshDlg->get_all_online_node(adr_list, MESH_OTA_UPDATE_NODE_MAX);
			//loop to send the scan start .
			if(update_node_cnt == 0){
				LOG_MSG_ERR (TL_LOG_COMMON, 0, 0, "No update node, please get online status before!!!",0);
	            return ;
			}
			for(u32 i=0;i<update_node_cnt;i++){
				send_rp_scan_start(adr_list[i],2,4);// serarch 2 item,and the time limit is 6s
			}
		}
	}else{
        if(ble_moudle_id_is_gateway()){
            gateway_VC_provision_start();
        }else{
            report_adv_opera(1);
        }
	}
	
	{
		UpdateData ();

		m_pScanDlg->ShowWindow (SW_NORMAL);
	}
}

remote_prov_cli_str rp_cli_win32;
int remote_prov_find_uuid_idx(remote_prov_scan_report *p_report)
{
    
    int idx =-1;
    int empty_idx = -1;
    for(int i=0;i<MAX_REMOTE_PROV_NODE_CNT;i++){
        remote_prov_uuid_str *p_uuid = &(rp_cli_win32.uuid[i]);
        if(p_uuid->valid){
			if( !memcmp(p_uuid->uuid ,p_report->uuid,sizeof(p_report->uuid))){
                return -2;// find the same uuid 
            }
        }else if(empty_idx == -1){
            empty_idx = i;
        }
    }
    return empty_idx;
}


int remote_prov_proc_client_scan_report(remote_prov_scan_report_win32 *p_reportwin32)
{
    remote_prov_scan_report *p_rep = (remote_prov_scan_report *)&(p_reportwin32->scan_report);
    int idx = remote_prov_find_uuid_idx(p_rep);
    if(idx>=0){
        remote_prov_uuid_str *p_uuid = &(rp_cli_win32.uuid[idx]);
        p_uuid->valid =1;
        p_uuid->src = p_reportwin32->unicast;
        p_uuid->rssi = p_rep->rssi;
        memcpy(p_uuid->uuid,p_rep->uuid,sizeof(p_rep->uuid));
        return 1;
    }else{
        return 0;
    }
}

u16 remote_prov_client_find_adr_by_uuid(u8 *p_uuid_rep)
{
    for(int i=0;i<MAX_REMOTE_PROV_NODE_CNT;i++){
        remote_prov_uuid_str *p_uuid = &(rp_cli_win32.uuid[i]);
        if(p_uuid->valid){
			if( !memcmp(p_uuid->uuid ,p_uuid_rep,16)){
                return p_uuid->src;// find the same uuid 
            }
        }
    }
    return -1;
}

void set_node_prov_mode(int mode)
{
    rp_cli_win32.prov_mode = mode;
    memset(rp_cli_win32.uuid,0,sizeof(remote_prov_uuid_str)*MAX_REMOTE_PROV_NODE_CNT);
}
int prov_mode_is_rp_mode()
{
    if(rp_cli_win32.prov_mode == PROV_REMOTE_MODE){
        return 1;
    }else{
        return 0;
    }
}
void remote_prov_scan_report_cb(u8 *par,u8 len)
{
    if (m_pScanDlg && m_pScanDlg->IsWindowVisible())
	{
		m_pScanDlg->AddDevice (par, len);
	}
}
void remote_prov_capa_sts_cb(u8 max_item,u8 active_scan)
{
    return ;
}

int mesh_key_store_win32(mesh_key_save_t *p_key_save)
{
    mesh_node_str *p_node = json_get_mesh_node(vc_uuid);
    if(p_node == NULL){
        return -1;
    }
    u8 app_key_idx =0;
    u8 net_key_idx =0;
    memcpy(p_key_save->dev_key,p_node->deviceKey,sizeof(p_node->deviceKey));
    for(int i=0;i<NET_KEY_MAX;i++){
        if(p_key_save->net_key[i].valid){
            mesh_nodes_net_appkey_str *p_json_net = &p_node->netkeys[net_key_idx++];
            //mesh_nodes_net_appkey_str *p_json_app = &p_node->appkeys[i];
            mesh_net_key_save_t *p_save_net = &p_key_save->net_key[i];
            p_json_net->valid =1;
            p_json_net->index = p_save_net->index;
            mesh_app_key_t *p_appkey = p_save_net->app_key;
            for(int j=0;j<APP_KEY_MAX;j++){
                if(p_appkey[i].valid){
                    mesh_nodes_net_appkey_str *p_json_app = &p_node->appkeys[app_key_idx++];
                    p_json_app->valid = 1;
                    p_json_app->index = p_appkey[j].index;
                }
            }   
        }
    }
    write_json_file_doc(FILE_MESH_DATA_BASE);
    return 0;

}

int mesh_key_retrieve_win32(mesh_key_save_t *p_key_save)
{
    //memset(p_key_save,0,sizeof(mesh_key_save_t));
    mesh_node_str *p_node = json_get_mesh_node(vc_uuid);
    if(p_node == NULL){
        return -1;
    }
    u8 netkey_idx =0;
    u8 appkey_idx =0;
    memcpy(p_key_save->dev_key,p_node->deviceKey,sizeof(p_node->deviceKey));
    mesh_net_key_save_t *p_net = p_key_save->net_key;
   
    // proc json file's netkey part 
    for(int i=0;i<MAX_NETKEY_TOTAL_NUM;i++){
         mesh_nodes_net_appkey_str *p_json_net = &(p_node->netkeys[i]);
         appkey_idx =0;
         if(p_json_net->valid){
            p_net[netkey_idx].valid =1;
            p_net[netkey_idx].node_identity =1;
            p_net[netkey_idx].index = p_json_net[i].index;
            // use the netkey idx to find the netkey value part
            u8 *p_net_json = json_get_netkey_by_idx(p_json_net[i].index);
            if(p_net_json!=NULL){
                memcpy(p_net[netkey_idx].key,p_net_json,16);
            }
            // proc json file's appkey part 
            mesh_app_key_t *p_appkey_save = p_net[netkey_idx].app_key;
            for(int j=0;j<MAX_APPKEY_TOTAL_NUM;j++){
                mesh_nodes_net_appkey_str *p_json_app = &(p_node->appkeys[j]);
                if( p_json_app->valid &&
                    json_use_appkey_get_netkey(p_json_app->index)== p_net[netkey_idx].index){
                    // valid appkey 
                    p_appkey_save[appkey_idx].valid =1;
                    p_appkey_save[appkey_idx].index = p_json_app->index;
                    u8 *p_app_val = json_use_appkey_get_key_val(p_json_app->index);
                    if(p_app_val!= NULL){
                        memcpy(p_appkey_save[appkey_idx].key,p_app_val,16);
                    }
                    appkey_idx++;
                    if(appkey_idx>=APP_KEY_MAX){
                        break;
                    }
                } 
            }
            netkey_idx++;
            if(netkey_idx>=NET_KEY_MAX){
                break;
            }
         }
    }
    return 0;   
}

int mesh_misc_store_win32(misc_save_t *p_misc)
{
    mesh_node_str *p_node = json_get_mesh_node(vc_uuid);
    if(p_node == NULL){
        return -1;
    }
    p_node->sno = p_misc->sno;
    //memcpy(json_database.ivi_idx,p_misc->iv_index,sizeof(p_misc->iv_index));
    write_json_file_doc(FILE_MESH_DATA_BASE);
    return 0;
}

int mesh_misc_retrieve_win32(misc_save_t *p_misc)
{
    memset(p_misc,0,sizeof(misc_save_t));
    mesh_node_str *p_node = json_get_mesh_node(vc_uuid);
    if(p_node == NULL){
        return -1;
    }
    p_misc->sno = p_node->sno;
    memcpy(p_misc->iv_index,json_database.ivi_idx,sizeof(p_misc->iv_index));
    return 0;
}

mesh_nodes_model_str *json_get_model_pointer_by_model_id(mesh_node_str *p_node,u32 mode_id,u16 *p_ele_adr,u8 idx)
{
    u8 node_model_idx =0;
    for(int i=0;i<MAX_MESH_NODE_ELE_NUM;i++){
        mesh_nodes_ele_str *p_ele = &p_node->elements[i];
        for(int j=0;j<MAX_MODELS_IN_ELE_CNT;j++){
            mesh_nodes_model_str *p_model = &p_ele->models[j];
            if(p_model->valid && p_model->modelId == mode_id){
                if((node_model_idx++) == idx){
                    *p_ele_adr = p_node->unicastAddress + i;
                    return p_model;
                }
            }
        }
    }
    return NULL;
}


void mesh_common_model_retrieve_win(model_common_t *p_common,mesh_node_str *p_node,u32 mode_id,u8 idx)
{
    // use the mode_id to get the model pointer ,and the element idx 
    u16 ele_adr;
    u8 bind_idx =0;
    mesh_nodes_model_str *p_json_model = json_get_model_pointer_by_model_id(p_node,mode_id,&ele_adr,idx);
    if(p_json_model ==NULL){
        return ;
    }
    p_common->ele_adr = ele_adr;
    for(int i=0;i<MAX_BIND_ADDRESS_NUM;i++){
        mesh_model_bind_str *p_bind = &(p_json_model->bind[i]);
        if(p_bind->valid){
            p_common->bind_key[bind_idx].bind_ok =1;
            p_common->bind_key[bind_idx].idx = p_bind->bind;
            p_common->bind_key[bind_idx].rsv =0;
            bind_idx++;
            if(bind_idx>=BIND_KEY_MAX){
                break;
            }
        }
    }
    //sub proc part 
    memcpy(p_common->sub_list,p_json_model->subscribe,sizeof(p_common->sub_list));
    // pub proc part 
    mesh_model_pub_par_t *p_pub_common = &(p_common->pub_par);
    mesh_models_publish_str *p_pub_json = &(p_json_model->publish);
    p_common->pub_adr = p_pub_json->address;
    p_pub_common->appkey_idx = p_pub_json->index;
    p_pub_common->credential_flag = p_pub_json->credentials;
	if(p_pub_json->resolution == 100){
		p_pub_common->period.res =0;
	}else if (p_pub_json->resolution == 1000){
		p_pub_common->period.res =1;
	}else if (p_pub_json->resolution == 10000){
		p_pub_common->period.res =2;
	}else if (p_pub_json->resolution == 6000000){
		p_pub_common->period.res =3;
	}else{}
    p_pub_common->period.steps = p_pub_json->numberOfSteps;
    p_pub_common->ttl = p_pub_json->ttl;
    p_pub_common->transmit.count = p_pub_json->count;
    p_pub_common->transmit.invl_steps = p_pub_json->interval;
    
}

int mesh_model_cfg_retrieve_win32(model_sig_cfg_s_t *p_cfg)
{

    mesh_node_str *p_node = json_get_mesh_node(vc_uuid);
    if(p_node == NULL){
        return -1;
    }
//	memset(p_cfg,0,sizeof(model_sig_cfg_s_t)); // don't memset default value, because some member may not in json file
    mesh_common_model_retrieve_win(&p_cfg->com,p_node,SIG_MD_CFG_SERVER,0);
    p_cfg->sec_nw_beacon = p_node->secureNetworkBeacon;
    p_cfg->ttl_def = p_node->defaultTTL;
    p_cfg->gatt_proxy = p_node->features.proxy;
    p_cfg->frid = p_node->features.fri;
    p_cfg->relay = p_node->features.relay;
    p_cfg->nw_transmit.count = p_node->networkTransmit.count;
    p_cfg->nw_transmit.invl_steps = p_node->networkTransmit.interval;
    p_cfg->relay_retransmit.count = p_node->relayRetransmit.count;
    p_cfg->relay_retransmit.invl_steps = p_node->relayRetransmit.interval;
	p_cfg->com.ele_adr = p_node->unicastAddress;
	return 0;
}

int mesh_model_health_retrieve_win32(model_health_t * p_health)
{
    memset(p_health,0,sizeof(model_health_t));
    mesh_node_str *p_node = json_get_mesh_node(vc_uuid);
    if(p_node == NULL){
        return -1;
    }
    mesh_common_model_retrieve_win(&(p_health->clnt.com),p_node,SIG_MD_HEALTH_CLIENT,0);
    mesh_common_model_retrieve_win(&(p_health->srv.com),p_node,SIG_MD_HEALTH_SERVER,0);
    return 0;
}

int mesh_model_onoff_retrieve_win32(model_g_onoff_level_t *p_onoff)
{
    memset(p_onoff,0,sizeof(model_g_onoff_level_t));
    mesh_node_str *p_node = json_get_mesh_node(vc_uuid);
    if(p_node == NULL){
        return -1;
    }
#if MD_SERVER_EN
    for(int i=0;i<LIGHT_CNT;i++){
        model_g_light_s_t *p_light =&(p_onoff->onoff_srv[i]);
        mesh_common_model_retrieve_win(&(p_light->com),p_node,SIG_MD_G_ONOFF_S,i);
    }
    #if MD_LEVEL_EN
    for(int j=0;j<LIGHT_CNT * ELE_CNT_EVERY_LIGHT;j++){
        model_g_light_s_t *p_level = &(p_onoff->level_srv[j]);
        mesh_common_model_retrieve_win(&(p_level->com),p_node,SIG_MD_G_LEVEL_S,j);
    }
    #endif
#endif
#if MD_CLIENT_EN
    model_client_common_t *p_onoff_cli = (p_onoff->onoff_clnt);
    mesh_common_model_retrieve_win(&(p_onoff_cli->com),p_node,SIG_MD_G_ONOFF_C,0);
	#if MD_LEVEL_EN
	model_client_common_t *p_level_cli = (p_onoff->level_clnt);
	mesh_common_model_retrieve_win(&(p_level_cli->com),p_node,SIG_MD_G_LEVEL_C,0);
	#endif
#endif
    return 0;

}

int mesh_model_time_retrieve_win32(model_time_schedule_t *p_timer)
{
    memset(p_timer,0,sizeof(model_time_schedule_t));
    mesh_node_str *p_node = json_get_mesh_node(vc_uuid);
    if(p_node == NULL){
        return -1;
    }
#if MD_SERVER_EN
    #if MD_TIME_EN
    model_g_light_s_t *p_time_srv = p_timer->time_srv;
    model_g_light_s_t *p_time_setup = p_timer->time_setup;
    mesh_common_model_retrieve_win(&(p_time_srv->com),p_node,SIG_MD_TIME_S,0);
    mesh_common_model_retrieve_win(&(p_time_setup->com),p_node,SIG_MD_TIME_SETUP_S,0);
    #endif
    #if MD_SCHEDULE_EN
    for(int i=0;i<LIGHT_CNT;i++){
        model_g_light_s_t *p_sch_srv = &(p_timer->sch_srv[i]);
        model_g_light_s_t *p_sch_setup = &(p_timer->sch_setup[i]);
        mesh_common_model_retrieve_win(&(p_sch_srv->com),p_node,SIG_MD_SCHED_S,i);
        mesh_common_model_retrieve_win(&(p_sch_setup->com),p_node,SIG_MD_SCHED_SETUP_S,i);
    }
    #endif
#endif
#if MD_CLIENT_EN
    #if MD_TIME_EN
    model_client_common_t *p_time_clnt = (p_timer->time_clnt);
    mesh_common_model_retrieve_win(&(p_time_clnt->com),p_node,SIG_MD_TIME_C,0);
    #endif
    #if MD_SCHEDULE_EN
    model_client_common_t *p_sch_clnt = (p_timer->sch_clnt);
    mesh_common_model_retrieve_win(&(p_sch_clnt->com),p_node,SIG_MD_SCHED_C,0);
    #endif
#endif
    return 0;

}

int mesh_model_lightness_retrieve_win32(model_lightness_t *p_light)
{
    memset(p_light,0,sizeof(model_lightness_t));
    mesh_node_str *p_node = json_get_mesh_node(vc_uuid);
    if(p_node == NULL){
        return -1;
    }    
#if MD_LIGHTNESS_EN
    #if MD_SERVER_EN
    for(int i=0;i<LIGHT_CNT;i++){
        model_g_light_s_t *p_srv = &(p_light->srv[i]);
        model_g_light_s_t *p_setup = &(p_light->setup[i]);
        mesh_common_model_retrieve_win(&(p_srv->com),p_node,SIG_MD_LIGHTNESS_S,i);
        mesh_common_model_retrieve_win(&(p_setup->com),p_node,SIG_MD_LIGHTNESS_SETUP_S,i);
    }
	#endif
    #if MD_CLIENT_EN
    model_client_common_t *p_clnt = p_light->clnt;
    mesh_common_model_retrieve_win(&(p_clnt->com),p_node,SIG_MD_LIGHTNESS_C,0);
    #endif
#endif
    return 0;
}

int mesh_model_light_ctl_retrieve_win32(model_light_ctl_t *p_ctl)
{
    memset(p_ctl,0,sizeof(model_light_ctl_t));
    mesh_node_str *p_node = json_get_mesh_node(vc_uuid);
    if(p_node == NULL){
        return -1;
    } 
#if MD_SERVER_EN
    for(int i=0;i<LIGHT_CNT;i++){
        model_g_light_s_t *p_srv = &(p_ctl->srv[i]);
        model_g_light_s_t *p_setup = &(p_ctl->setup[i]);
        model_g_light_s_t *p_temp = &(p_ctl->temp[i]);
        mesh_common_model_retrieve_win(&(p_srv->com),p_node,SIG_MD_LIGHT_CTL_S,i);
        mesh_common_model_retrieve_win(&(p_setup->com),p_node,SIG_MD_LIGHT_CTL_SETUP_S,i);
        mesh_common_model_retrieve_win(&(p_temp->com),p_node,SIG_MD_LIGHT_CTL_TEMP_S,i);
    }
#endif
#if MD_CLIENT_EN
    model_client_common_t *p_clnt = p_ctl->clnt;
    mesh_common_model_retrieve_win(&(p_clnt->com),p_node,SIG_MD_LIGHT_CTL_C,0);
#endif
    return 0;

}

int mesh_model_light_lc_retrieve_win32(model_light_lc_t *p_lc)
{
    memset(p_lc,0,sizeof(model_light_lc_t));
    mesh_node_str *p_node = json_get_mesh_node(vc_uuid);
    if(p_node == NULL){
        return -1;
    }
#if MD_SERVER_EN
    for(int i=0;i<LIGHT_CNT;i++){
        model_g_light_s_t *p_srv = &p_lc->srv[i];
        model_g_light_s_t *p_setup = &p_lc->setup[i];
        mesh_common_model_retrieve_win(&(p_srv->com),p_node,SIG_MD_LIGHT_LC_S,i);
        mesh_common_model_retrieve_win(&(p_setup->com),p_node,SIG_MD_LIGHT_LC_SETUP_S,i);
    }
#endif
#if MD_CLIENT_EN
    model_client_common_t *p_clnt = p_lc->clnt;
	mesh_common_model_retrieve_win(&(p_clnt->com),p_node,SIG_MD_LIGHT_LC_C,0);
#endif
    return 0;
}

int mesh_model_light_hsl_retrieve_win32(model_light_hsl_t *p_hsl)
{
    memset(p_hsl,0,sizeof(model_light_hsl_t));
    mesh_node_str *p_node = json_get_mesh_node(vc_uuid);
    if(p_node == NULL){
        return -1;
    }    
#if MD_SERVER_EN
    for(int i=0;i<LIGHT_CNT;i++){
        model_g_light_s_t *p_srv = &(p_hsl->srv[i]);
        model_g_light_s_t *p_setup = &(p_hsl->setup[i]);
        model_g_light_s_t *p_hue = &(p_hsl->hue[i]);
        model_g_light_s_t *p_sat = &(p_hsl->sat[i]);
        mesh_common_model_retrieve_win(&(p_srv->com),p_node,SIG_MD_LIGHT_HSL_S,i);
        mesh_common_model_retrieve_win(&(p_setup->com),p_node,SIG_MD_LIGHT_HSL_SETUP_S,i);
        mesh_common_model_retrieve_win(&(p_hue->com),p_node,SIG_MD_LIGHT_HSL_HUE_S,i);
        mesh_common_model_retrieve_win(&(p_sat->com),p_node,SIG_MD_LIGHT_HSL_SAT_S,i);
    }
#endif
#if MD_CLIENT_EN
    model_client_common_t *p_clnt = p_hsl->clnt;
    mesh_common_model_retrieve_win(&(p_clnt->com),p_node,SIG_MD_LIGHT_HSL_C,0);
#endif    
    return 0;
}

int mesh_model_sensor_retrieve_win32(model_sensor_t *p_sensor)
{
    memset(p_sensor,0,sizeof(model_sensor_t));
    mesh_node_str *p_node = json_get_mesh_node(vc_uuid);
    if(p_node == NULL){
        return -1;
    } 
#if MD_SERVER_EN
    #if MD_SENSOR_SERVER_EN
        for(int i=0;i<LIGHT_CNT;i++){
            model_g_light_s_t *p_sensor_srv = &p_sensor->sensor_srv[i];
            model_g_light_s_t *p_sensor_setup = &p_sensor->sensor_setup[i];
            mesh_common_model_retrieve_win(&(p_sensor_srv->com),p_node,SIG_MD_SENSOR_S,i);
            mesh_common_model_retrieve_win(&(p_sensor_setup->com),p_node,SIG_MD_SENSOR_SETUP_S,i);
        }
    // not proc the sensor state part 
    #endif
    #if MD_BATTERY_EN
        for(int i=0;i<LIGHT_CNT;i++){
            model_g_light_s_t *p_bat_srv = &(p_sensor->battery_srv[i]);
            mesh_common_model_retrieve_win(&(p_bat_srv->com),p_node,SIG_MD_G_BAT_S,i);
        }
    #endif
    #if MD_LOCATION_EN
        for(int i=0;i<LIGHT_CNT;i++){
            model_g_light_s_t *p_loc_srv = &(p_sensor->location_srv[i]);
            model_g_light_s_t *p_loc_setup = &(p_sensor->location_setup[i]);
            mesh_common_model_retrieve_win(&(p_loc_srv->com),p_node,SIG_MD_G_LOCATION_S,i);
            mesh_common_model_retrieve_win(&(p_loc_setup->com),p_node,SIG_MD_G_LOCATION_SETUP_S,i);
        }
    #endif
#endif
    
#if MD_SENSOR_CLIENT_EN
        model_client_common_t *p_sensor_clnt = p_sensor->sensor_clnt;
	    mesh_common_model_retrieve_win(&(p_sensor_clnt->com),p_node,SIG_MD_SENSOR_C,0);
#endif
#if MD_CLIENT_EN
	#if MD_BATTERY_EN
	    model_client_common_t *p_bat_clnt = p_sensor->battery_clnt;
        mesh_common_model_retrieve_win(&(p_bat_clnt->com),p_node,SIG_MD_G_BAT_C,0);
	#endif
	#if MD_LOCATION_EN
        model_client_common_t *p_loc_clnt = p_sensor->location_clnt;
        mesh_common_model_retrieve_win(&(p_loc_clnt->com),p_node,SIG_MD_G_LOCATION_C,0);
	#endif
#endif
    return 0;
}

int mesh_model_power_onoff_win32(model_g_power_onoff_trans_time_t * p_power)
{
    memset(p_power,0,sizeof(model_g_power_onoff_trans_time_t));
    mesh_node_str *p_node = json_get_mesh_node(vc_uuid);
    if(p_node == NULL){
        return -1;
    }     
#if MD_SERVER_EN
    #if MD_POWER_ONOFF_EN
        for(int i=0;i<LIGHT_CNT;i++){
            model_g_light_s_t *p_pw_onoff_srv = &(p_power->pw_onoff_srv[i]);
            model_g_light_s_t *p_pw_onoff_setup = &(p_power->pw_onoff_setup[i]);
            mesh_common_model_retrieve_win(&(p_pw_onoff_srv->com),p_node,SIG_MD_G_POWER_ONOFF_S,i);
            mesh_common_model_retrieve_win(&(p_pw_onoff_setup->com),p_node,SIG_MD_G_POWER_ONOFF_SETUP_S,i);
        }
    #endif
    #if MD_DEF_TRANSIT_TIME_EN
        for(int i=0;i<LIGHT_CNT;i++){
            model_g_light_s_t *p_trans_time_srv = &(p_power->def_trans_time_srv[i]);
            mesh_common_model_retrieve_win(&(p_trans_time_srv->com),p_node,SIG_MD_G_DEF_TRANSIT_TIME_S,i);
        }
    #endif
#endif
#if MD_CLIENT_EN
    #if MD_POWER_ONOFF_EN
        model_client_common_t *p_onoff_clnt = p_power->pw_onoff_clnt;
        mesh_common_model_retrieve_win(&(p_onoff_clnt->com),p_node,SIG_MD_G_POWER_ONOFF_C,0);
    #endif
    #if MD_DEF_TRANSIT_TIME_EN
        model_client_common_t *p_trans_time_clnt = p_power->def_trans_time_clnt;
        mesh_common_model_retrieve_win(&(p_trans_time_clnt->com),p_node,SIG_MD_G_DEF_TRANSIT_TIME_C,0);
    #endif
#endif
	return 0;
}

int mesh_model_scene_win32(model_scene_t * p_scene)
{
    memset(p_scene,0,sizeof(model_scene_t));
    mesh_node_str *p_node = json_get_mesh_node(vc_uuid);
    if(p_node == NULL){
        return -1;
    }
#if MD_SERVER_EN
    for(int i=0;i<LIGHT_CNT;i++){
        model_g_light_s_t *p_srv = &(p_scene->srv[i]);
        model_g_light_s_t *p_setup = &(p_scene->setup[i]);
        mesh_common_model_retrieve_win(&(p_srv->com),p_node,SIG_MD_SCENE_S,i);
        mesh_common_model_retrieve_win(&(p_setup->com),p_node,SIG_MD_SCENE_SETUP_S,i);
    }
#endif
#if MD_CLIENT_EN
    model_client_common_t *p_clnt = p_scene->clnt;
    mesh_common_model_retrieve_win(&(p_clnt->com),p_node,SIG_MD_SCENE_C,0);
#endif 
    return 0;
}

int mesh_model_mesh_ota_win32(model_mesh_ota_t *p_ota)
{
    memset(p_ota,0,sizeof(model_mesh_ota_t));
    mesh_node_str *p_node = json_get_mesh_node(vc_uuid);
    if(p_node == NULL){
        return -1;
    }
#if 1 // MD_SERVER_EN
    model_g_light_s_t *p_fw_distr_srv = &(p_ota->fw_distr_srv);
    model_client_common_t *p_fw_update_clnt = &(p_ota->fw_update_clnt);
    model_client_common_t *p_blob_trans_clnt = &(p_ota->blob_trans_clnt);
    mesh_common_model_retrieve_win(&(p_fw_distr_srv->com),p_node,SIG_MD_FW_DISTRIBUT_S,0);
    mesh_common_model_retrieve_win(&(p_fw_update_clnt->com),p_node,SIG_MD_FW_UPDATE_C,0);
    mesh_common_model_retrieve_win(&(p_blob_trans_clnt->com),p_node,SIG_MD_BLOB_TRANSFER_C,0);
    #if MD_SERVER_EN
    model_g_light_s_t *p_fw_update_srv = &(p_ota->fw_update_srv);
    model_g_light_s_t *p_blob_trans_srv = &(p_ota->blob_trans_srv);
    mesh_common_model_retrieve_win(&(p_fw_update_srv->com),p_node,SIG_MD_FW_UPDATE_S,0);
    mesh_common_model_retrieve_win(&(p_blob_trans_srv->com),p_node,SIG_MD_BLOB_TRANSFER_S,0);
    #endif
#endif
#if MD_CLIENT_EN
    model_client_common_t *p_fw_distr_clnt = &(p_ota->fw_distr_clnt);
    mesh_common_model_retrieve_win(&(p_fw_distr_clnt->com),p_node,SIG_MD_FW_DISTRIBUT_C,0);
#endif
    return 0;
}

int mesh_model_remote_prov_win32(model_remote_prov_t *p_remote)
{
    memset(p_remote,0,sizeof(model_remote_prov_t));
    mesh_node_str *p_node = json_get_mesh_node(vc_uuid);
    if(p_node == NULL){
        return -1;
    }
#if MD_SERVER_EN
    model_rp_server_common_t *p_srv = p_remote->srv;
    mesh_common_model_retrieve_win(&(p_srv->com),p_node,SIG_MD_REMOTE_PROV_SERVER,0);
#endif
#if MD_CLIENT_EN
    model_rp_client_common_t *p_clnt = p_remote->client;
    mesh_common_model_retrieve_win(&(p_clnt->com),p_node,SIG_MD_REMOTE_PROV_CLIENT,0);
#endif
    return 0;
}

u8 win32_proved_state()
{
     mesh_node_str *p_node = json_get_mesh_node(vc_uuid);
     if(p_node == NULL){
        return 0;
     }else{
        return 1;
     }
}

void mesh_heartbeat_cb_data(u16 src, u16 dst,u8 *p_hb)
{
    LOG_MSG_INFO(TL_LOG_COMMON,p_hb,sizeof(mesh_hb_msg_t)+1,
        "heartbeat src adr is 0x%04x,dst adr is 0x%04x",src,dst);
}


int mesh_par_retrieve_store_win32(u8 *in_out, u32 *p_adr, u32 adr_base, u32 size,u8 flag)
{
    int err =-1;
    switch(adr_base){
        case FLASH_ADR_PROVISION_CFG_S:
        case FLASH_ADR_FRIEND_SHIP:
        case FLASH_ADR_RESET_CNT:
        case FLASH_ADR_MESH_TYPE_FLAG:
        case FLASH_ADR_SW_LEVEL:
            break;
        case FLASH_ADR_MESH_KEY:
        // get the json file's net key and the dev key to update
            if(flag == MESH_PARA_STORE_VAL){
               //err = mesh_key_store_win32((mesh_key_save_t *)in_out);
            }else if(flag == MESH_PARA_RETRIEVE_VAL){
                err = mesh_key_retrieve_win32((mesh_key_save_t *)in_out);
            }
            break;
        case FLASH_ADR_MISC:
        // update the sno ,and move the sno to the json file part 
            if(flag == MESH_PARA_STORE_VAL){
                mesh_misc_store_win32((misc_save_t *)in_out);
            }else if (flag == MESH_PARA_RETRIEVE_VAL){
                err = mesh_misc_retrieve_win32((misc_save_t *)in_out);
            }
            break;
        case FLASH_ADR_VC_NODE_INFO:
        // update the vc node info to json 
            if(flag == MESH_PARA_STORE_VAL){
                // json file is the newest ,no need to change by the store part .
            }else if (flag == MESH_PARA_RETRIEVE_VAL){
                err = update_VC_info_from_json(netidx ,(VC_node_info_t *)in_out);
            }
            break;
       // model proc part 
        case FLASH_ADR_MD_CFG_S:
            if(flag == MESH_PARA_RETRIEVE_VAL){
                err = mesh_model_cfg_retrieve_win32((model_sig_cfg_s_t *) in_out);
            }
            break;
        case FLASH_ADR_MD_HEALTH:
            if(flag == MESH_PARA_RETRIEVE_VAL){
                err = mesh_model_health_retrieve_win32((model_health_t *) in_out);
            }
            break;
        case FLASH_ADR_MD_G_ONOFF_LEVEL:
            if(flag == MESH_PARA_RETRIEVE_VAL){
                err = mesh_model_onoff_retrieve_win32((model_g_onoff_level_t *)in_out);
            }
            break;
        case FLASH_ADR_MD_TIME_SCHEDULE:
            if(flag == MESH_PARA_RETRIEVE_VAL){
                #if STRUCT_MD_TIME_SCHEDULE_EN
                err = mesh_model_time_retrieve_win32((model_time_schedule_t *)in_out);
                #endif
            }
            break;
        case FLASH_ADR_MD_LIGHTNESS:
            if(flag == MESH_PARA_RETRIEVE_VAL){
                #if MD_LIGHT_CONTROL_EN
                err = mesh_model_lightness_retrieve_win32((model_lightness_t *)in_out);
                #endif
            }
            break;
        case FLASH_ADR_MD_LIGHT_CTL:
            if(flag == MESH_PARA_RETRIEVE_VAL){
                #if (LIGHT_TYPE_CT_EN)
                err = mesh_model_light_ctl_retrieve_win32((model_light_ctl_t*)in_out);
                #endif
            }
            break;
        case FLASH_ADR_MD_LIGHT_LC:
            if(flag == MESH_PARA_RETRIEVE_VAL){
                #if MD_LIGHT_CONTROL_EN
                err = mesh_model_light_lc_retrieve_win32((model_light_lc_t *)in_out);
                #endif
            }
            break;
        case FLASH_ADR_MD_LIGHT_HSL:
            if(flag == MESH_PARA_RETRIEVE_VAL){
                #if LIGHT_TYPE_HSL_EN
                err = mesh_model_light_hsl_retrieve_win32((model_light_hsl_t *)in_out);
                #endif
            }
            break;
        case FLASH_ADR_MD_SENSOR:
            if(flag == MESH_PARA_RETRIEVE_VAL){
                #if(MD_SENSOR_EN || MD_BATTERY_EN || MD_LOCATION_EN) 
                err = mesh_model_sensor_retrieve_win32((model_sensor_t *)in_out);
                #endif
            }
            break;
        case FLASH_ADR_MD_G_POWER_ONOFF:
            if(flag == MESH_PARA_RETRIEVE_VAL){
                #if STRUCT_MD_DEF_TRANSIT_TIME_POWER_ONOFF_EN
                err = mesh_model_power_onoff_win32((model_g_power_onoff_trans_time_t *)in_out);
                #endif
            }
            break;
        case FLASH_ADR_MD_SCENE:
            if(flag == MESH_PARA_RETRIEVE_VAL){
                #if MD_SCENE_EN
                err = mesh_model_scene_win32((model_scene_t *) in_out);
                #endif
            }
            break;
        case FLASH_ADR_MD_MESH_OTA:
            if(flag == MESH_PARA_RETRIEVE_VAL){
                #if MD_MESH_OTA_EN
                err = mesh_model_mesh_ota_win32((model_mesh_ota_t *) in_out);
                #endif
            }
            break;
        case FLASH_ADR_MD_REMOTE_PROV:
            if(flag == MESH_PARA_RETRIEVE_VAL){
                #if MD_REMOTE_PROV
                err = mesh_model_remote_prov_win32((model_remote_prov_t *) in_out);
                #endif
            }
            break;
        default:
            break;
    }
    return err;
}

#if DEBUG_SHOW_VC_SELF_EN
// TODO
int directed_forwarding_dependents_update_start(u16 netkey_offset, u8 type, u16 path_enpoint, u16 dependent_addr, u8 dependent_ele_cnt){return 0;}
#endif

void CTl_ble_moduleDlg::OnBnClickedChnSet()
{
	// TODO: 在此添加控件通知处理程序代码
	int index = ((CComboBox*)GetDlgItem(IDC_COMBO_ADV_CHN))->GetCurSel();
	if(index<3){
		set_adv_chn_device(37+index);
	}else{
		set_adv_chn_device(0xff);
	}
}

void gw_json_update()
{
		// clear all the json file , and do init part for the gateway part 
        gateway_VC_factory_reset();
		// wait until the gateway finish the reset .
		LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "wait until it finish reset",0);
		WaitForSingleObject (NULLEVENT2, 5000);
		//  provision the gateway first . 
		gateway_get_provision_self_sts();
		WaitForSingleObject (NULLEVENT2, 500);
		// wait to get the information of the gateway part 
		mesh_json_netkeys_str *p_net = json_database.netKeys;
		// get the first node information first 
		prov_get_net_info_from_json(&net_info,&netidx,-1);
		get_app_key_idx_by_net_idx(net_info.key_index,&vc_dlg_appkey);
		update_VC_info_from_json(netidx ,VC_node_info);
		// find the max unicast adr .
		//net_info.unicast_address = json_get_next_unicast_adr_val(vc_uuid);
		// update the gateway provision information into the gateway 
		gateway_send_all_vc_node_info();
		// do the keybind information 
		gateway_set_provisioner_para((u8 *)(&net_info));//provision the gateway part 
		gateway_set_prov_devkey(net_info.unicast_address,vc_dev_key);
	    provision_self_to_json(&net_info,&netidx,vc_uuid,vc_dev_key);
		//set the dev key part 
		set_dev_key(vc_dev_key);
		// need to update the part of the unicast adr part 
        json_add_provisioner_info(&net_info,vc_uuid);
		mesh_misc_store();
		ele_adr_primary = net_info.unicast_address;
		VC_node_dev_key_save(ele_adr_primary, mesh_key.dev_key,g_ele_cnt);
		VC_node_cps_save(gp_page0, ele_adr_primary, SIZE_OF_PAGE0_LOCAL);
		LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "provision the gateway first ",0);
		mesh_json_set_node_info(ele_adr_primary,gw_mac);
    	json_set_appkey_bind_self_proc(vc_uuid,netidx);
		write_json_file_doc(FILE_MESH_DATA_BASE);
		gateway_set_start_keybind(1,(u8*)(&vc_dlg_appkey.apk_idx),vc_dlg_appkey.app_key);
		
		// then we will need to update all the information into the gateway part 
		LOG_MSG_INFO (TL_LOG_COMMON, 0, 0, "update the vc node info into the gateway ",0);
}

UINT ThreadGatewayJsonUpdate (void* pParams)
{
	gw_json_update();

	return 0;
}


UINT ThreadUpdateGatewayNodesInfo(void *pParam)
{
	gateway_send_all_vc_node_info();
	return 0;

}


void CTl_ble_moduleDlg::OnBnClickedInputDb()
{
	// TODO: 在此添加控件通知处理程序代码
	if(ota_filename.GetLength() == 0){
		AfxMessageBox(_T("empty json file "));
		return ;
	}
	set_ini_unicast_max(0);
	init_json( ota_filename.GetBuffer(),1);//read the json file into the  json_db_static
	ota_filename.ReleaseBuffer();
	database_static_to_normal();// update the json_db_static into the json_database
	// TODO: Add your control notification handler code here
    char buf[1024];
    ::GetModuleFileName(NULL,buf,sizeof(buf));
    CString strPath = buf;
    ShowWindow(SW_HIDE);//隐藏本对话框

    // Create New Process.
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    si.dwFlags = STARTF_USESHOWWINDOW;  // 指定wShowWindow成员有效
    si.wShowWindow = TRUE;              // 此成员设为TRUE的话则显示新建进程的主窗口，
    CreateProcess(strPath, "Restart", NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);
	//OnOK();//退出当前执行对话框程序
	TerminateProcess(GetCurrentProcess(), 0);
}
	

void CTl_ble_moduleDlg::OnBnClickedOutputDb()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog *fpdlg = new CFileDialog( FALSE , _T(""),_T(""),
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("Config File(*.json)|*.json|All File(*.*)|*.*||"),NULL);
	if(fpdlg->DoModal() == IDOK)
	{
		CString filepathname = fpdlg->GetPathName(); 
		if ( PathFileExists ( filepathname ) ){
			DeleteFile ( filepathname );
		}else{
			database_normal_to_static();//update the information into the jsosn_db_static 
			write_json_file_doc_static(filepathname.GetBuffer());// write the data into the json file part 
			filepathname.ReleaseBuffer();
		}	
	}
}


void CTl_ble_moduleDlg::OnBnClickedUseDirected()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	#if MD_DF_EN
	mesh_directed_proxy_control_set(m_use_directed, ele_adr_primary, g_ele_cnt);
	#endif
}

void CTl_ble_moduleDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值;
	if (m_hDev != NULL) {
		if(IsMasterMode()){
			disconnect_device();
		}
	}
	KillTimer (1);

    // 等待线程退出
    bulk_thread_status = THREAD_EXIT;
    main_thread_status = THREAD_EXIT;

#if 1
    ShowWindow(SW_HIDE);
    if (m_MeshDlg.IsWindowVisible()) {
        m_MeshDlg.ShowWindow(SW_HIDE);
    }
    if (m_ProDlg.IsWindowVisible()) {
        m_ProDlg.ShowWindow(SW_HIDE);
    }
    if (m_RxTestDlg.IsWindowVisible()) {
        m_RxTestDlg.ShowWindow(SW_HIDE);
    }
#endif
    UINT32 tick = GetTickCount();
    while(TRUE) {
        if (bulk_thread_status == THREAD_EXITED && main_thread_status == THREAD_EXITED) {
            break;
        }else{
            MSG msg;
            if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
                AfxGetApp()->PumpMessage();
            }
            Sleep(1);
        }
        
        // if threads not exited, terminate they.
        if (GetTickCount() - tick > 500) {
            if (bulk_thread_status != THREAD_EXITED) {
                TerminateThread(bulk_thread_handle, 0);
                bulk_thread_status = THREAD_EXITED;
            }
            if (main_thread_status != THREAD_EXITED) {
                TerminateThread(main_thread_handle, 0);
                main_thread_status = THREAD_EXITED;
            }
        }
    }

	CDialog::OnClose();
}


void CTl_ble_moduleDlg::OnBnClickedextendscan()
{
	// TODO: 在此添加控件通知处理程序代码
	// TODO: 在此添加控件通知处理程序代码
	if(!device_ok&&!connect_serial_port){
		AfxMessageBox("DEVICE NOT FOUND");
		return ;
	}
	set_node_prov_mode(PROV_REMOTE_MODE);
	//DWORD nB;
	if(prov_mode_is_rp_mode()){
		u16 adr_list[MESH_OTA_UPDATE_NODE_MAX] = {0};
		u32 update_node_cnt=0;
		update_node_cnt = m_pMeshDlg->get_all_online_node(adr_list, MESH_OTA_UPDATE_NODE_MAX);
		//loop to send the scan start .
		if(update_node_cnt == 0){
			LOG_MSG_ERR (TL_LOG_COMMON, 0, 0, "No update node, please get online status before!!!",0);
            return ;
		}
		for(u32 i=0;i<update_node_cnt;i++){
			u8 adv_type[1]={1};
			send_rp_extend_scan_start(adr_list[i],adv_type,sizeof(adv_type));// scan for about the adv filter part .
		}
        
	}
	{
		UpdateData ();

		m_pScanDlg->ShowWindow (SW_NORMAL);
	}
}



void CTl_ble_moduleDlg::OnBnClickedFastbind()
{
	// TODO: 在此添加控件通知处理程序代码
	CString current_ini_filename;
	((CComboBox *) GetDlgItem (IDC_INIFILE))->GetWindowText(current_ini_filename);
	if (current_ini_filename.IsEmpty()) {
		return;
	}

	CString current_ini_filepath = ".\\" + current_ini_filename;

	if (!PathFileExists(current_ini_filepath)) {
		return;
	}

	CButton *checkbox = (CButton *)GetDlgItem(IDC_FASTBIND);
	int checked = (int)checkbox->GetCheck();
	CString checked_string;
	checked_string.Format("%d", checked);

	WritePrivateProfileString("SET", "fastbind", checked_string, current_ini_filepath);

	//OnSelchangeInifile();
}


void CTl_ble_moduleDlg::OnBnClickedCheckAutoSaveLog()
{
	// TODO: 在此添加控件通知处理程序代码
	CString current_ini_filename;
	((CComboBox *) GetDlgItem (IDC_INIFILE))->GetWindowText(current_ini_filename);
	if (current_ini_filename.IsEmpty()) {
		return;
	}

	CString current_ini_filepath = ".\\" + current_ini_filename;

	if (!PathFileExists(current_ini_filepath)) {
		return;
	}

	CButton *checkbox = (CButton *)GetDlgItem(IDC_CHECK_AUTO_SAVE_LOG);
	int checked = (int)checkbox->GetCheck();
	CString checked_string;
	checked_string.Format("%d", checked);

	WritePrivateProfileString("SET", "log_backup", checked_string, current_ini_filepath);

	OnSelchangeInifile();
}

int CTl_ble_moduleDlg::SaveExtendAdvOption(int Option_index)
{
    CString current_ini_filename;
    ((CComboBox *) GetDlgItem (IDC_INIFILE))->GetWindowText(current_ini_filename);
    if (current_ini_filename.IsEmpty()) {
        return -1;
    }

    CString current_ini_filepath = ".\\" + current_ini_filename;

    if (!PathFileExists(current_ini_filepath)) {
        return -1;
    }

    int index = Option_index;
    CString string_to_write;
    string_to_write.Format("%d", index);
    WritePrivateProfileString("SET", "ExtendAdvOption", string_to_write, current_ini_filepath);

    return 0;
}

void CTl_ble_moduleDlg::OnCbnSelchangeComboExtendAdvOption()
{
	if (0 != SaveExtendAdvOption(m_combo_extend_adv_option.GetCurSel())) {
		return;
	}

	OnSelchangeInifile();
}


