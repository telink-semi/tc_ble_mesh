// CRemoteScanPar.cpp: 实现文件
//
#include "stdafx.h"
#include "afxdd_.h"
#include "afxdialogex.h"
#include <stdio.h>
#include "strsafe.h"
#include <memory>
//#include "pch.h"
#include "tl_ble_module.h"
#include "Remote_scan_dlg.h"
#include "afxdialogex.h"
#include "../../ble_lt_mesh/vendor/common/remote_prov.h"
#include "../../ble_lt_mesh/vendor/common/mesh_ota.h"

// CRemoteScanPar 对话框

IMPLEMENT_DYNAMIC(CRemoteScanPar, CDialog)

CRemoteScanPar::CRemoteScanPar(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_RP_SCAN_CAP, pParent)
	, scan_num(0)
	, timeout(4)
{
	
}

CRemoteScanPar::~CRemoteScanPar()
{
}

void CRemoteScanPar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_RP_SCAN_LIMIT, scan_num);
	DDX_Text(pDX, IDC_RP_SCAN_TIMEOUT, timeout);
}


BEGIN_MESSAGE_MAP(CRemoteScanPar, CDialog)
	ON_BN_CLICKED(IDC_GET_SCAN_CAP, &CRemoteScanPar::OnBnClickedGetScanCap)
END_MESSAGE_MAP()

void CRemoteScanPar::OnBnClickedGetScanCap()
{
	// TODO: 在此添加控件通知处理程序代码
	mesh_cmd_sig_rp_cli_send_capa(APP_get_GATT_connect_addr());
}

void CRemoteScanPar::StatusNotify(unsigned char* p, int len)
{
	CString tmp;
	
	mesh_rc_rsp_t rsp;
	rsp.len = len;
	memcpy(&rsp.src, p, len);

	u16 op = rf_link_get_op_by_ac(rsp.data);
	u32 size_op = SIZE_OF_OP(op);

	if (REMOTE_PROV_SCAN_CAPA_STS == op) {
		extern HANDLE REMOTE_SCAN_START_EVENT;
		SetEvent(REMOTE_SCAN_START_EVENT);
		remote_prov_scan_cap_sts* p_capa = (remote_prov_scan_cap_sts*)(rsp.data+ size_op);
		tmp.Format("%d", p_capa->maxScannedItems);
		((CEdit*)GetDlgItem(IDC_RP_SCAN_CAP_ST))->SetWindowText(tmp);
	}
}