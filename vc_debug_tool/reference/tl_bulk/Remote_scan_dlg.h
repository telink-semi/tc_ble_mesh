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
