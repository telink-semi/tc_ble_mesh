; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CTL_privision
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "tl_ble_module.h"
LastPage=0

ClassCount=6
Class1=CScanDialog
Class2=CScanDlg
Class3=CTl_ble_moduleApp
Class4=CTl_ble_moduleDlg
Class5=CTLMeshDlg

ResourceCount=4
Resource1=IDD_PROVISION
Resource2=IDD_TL_BLE_MODULE_DIALOG (English (U.S.))
Resource3=IDD_TL_MESH_DLG
Class6=CTL_privision
Resource4=IDD_TL_SCAN_DLG (English (U.S.))

[CLS:CScanDialog]
Type=0
BaseClass=CDialog
HeaderFile=ScanDialog.h
ImplementationFile=ScanDialog.cpp
LastObject=CScanDialog

[CLS:CScanDlg]
Type=0
BaseClass=CDialog
HeaderFile=ScanDlg.h
ImplementationFile=ScanDlg.cpp

[CLS:CTl_ble_moduleApp]
Type=0
BaseClass=CWinApp
HeaderFile=tl_ble_module.h
ImplementationFile=tl_ble_module.cpp

[CLS:CTl_ble_moduleDlg]
Type=0
BaseClass=CDialog
HeaderFile=tl_ble_moduleDlg.h
ImplementationFile=tl_ble_moduleDlg.cpp

[CLS:CTLMeshDlg]
Type=0
BaseClass=CDialog
HeaderFile=TLMeshDlg.h
ImplementationFile=TLMeshDlg.cpp
LastObject=CTLMeshDlg
Filter=D
VirtualFilter=dWC

[DLG:IDD_TL_BLE_SCAN]
Type=1
Class=CScanDialog

[DLG:IDD_TL_SCAN_DLG]
Type=1
Class=CScanDlg

[DLG:IDD_TL_BLE_MODULE_DIALOG]
Type=1
Class=CTl_ble_moduleDlg

[DLG:IDD_TL_MESH_DLG]
Type=1
Class=CTLMeshDlg
ControlCount=7
Control1=IDC_STATIC,button,1342177287
Control2=IDC_GRID,MFCGridCtrl,1342242816
Control3=IDC_STATIC,button,1342177287
Control4=IDC_GRID_GROUP,MFCGridCtrl,1342242816
Control5=IDC_NODE,button,1342242816
Control6=IDC_GROUP,button,1342242816
Control7=IDC_LIGHT,static,1342308352

[DLG:IDD_TL_SCAN_DLG (English (U.S.))]
Type=1
Class=?
ControlCount=3
Control1=IDCANCEL,button,1342242816
Control2=IDC_SCANLIST,listbox,1352728833
Control3=IDC_CONNECT,button,1342242816

[DLG:IDD_TL_BLE_MODULE_DIALOG (English (U.S.))]
Type=1
Class=?
ControlCount=25
Control1=IDC_CLOSEDLG,button,1342242817
Control2=IDC_LOGCLEAR,button,1342242816
Control3=IDC_COMMAND,listbox,1084293377
Control4=IDC_INIFILE,combobox,1344340226
Control5=IDC_LOG,edit,1353713860
Control6=IDC_OPEN,button,1342242816
Control7=IDC_SCAN,button,1073807360
Control8=IDC_CHANNEL,combobox,1075904514
Control9=IDC_CONN_INTERVAL,edit,1082196096
Control10=IDC_CONN_TIMEOUT,edit,1082196096
Control11=IDC_CONN_CHNMASK,edit,1082196096
Control12=IDC_STOP,button,1073807360
Control13=IDC_LOGSAVE,button,1342242816
Control14=IDC_LOGSAVEFILE,button,1342242816
Control15=IDC_LOGEN,button,1342242819
Control16=IDC_TEXT_BO,edit,1350635652
Control17=IDC_BULKOUT,button,1342242816
Control18=IDC_HEX,button,1342242819
Control19=IDC_CMDINPUT,edit,1350631552
Control20=IDC_CMDSEL,button,1342242816
Control21=IDC_Device,button,1342177280
Control22=IDC_PROVISION,button,1342242816
Control23=IDC_SCAN_STOP,button,1342242816
Control24=IDC_MESH_DISPLAY,button,1342242816
Control25=IDC_ADV_MODE,button,1342242819

[DLG:IDD_PROVISION]
Type=1
Class=CTL_privision
ControlCount=15
Control1=IDC_network_key_text,static,1342308352
Control2=IDC_NETWORK_KEY,edit,1350631552
Control3=IDC_STATIC,static,1342308352
Control4=IDC_KEY_INDEX,edit,1350631552
Control5=IDC_STATIC,static,1342308352
Control6=IDC_IV_UPDATE_FLAG,edit,1350631552
Control7=IDC_STATIC,static,1342308352
Control8=IDC_IV_INDEX,edit,1350631552
Control9=IDC_STATIC,static,1342308352
Control10=IDC_UNICAST_ADR,edit,1350631552
Control11=IDC_PROVISION_START,button,1342242816
Control12=IDC_PRO_DISCONNECT,button,1342242816
Control13=IDC_CMD_DAT,edit,1350631552
Control14=IDC_CMD,static,1342308352
Control15=IDC_START_SEND,button,1342242816

[CLS:CTL_privision]
Type=0
HeaderFile=CTL_privision.h
ImplementationFile=CTL_privision.cpp
BaseClass=CDialog
Filter=D
LastObject=CTL_privision
VirtualFilter=dWC

