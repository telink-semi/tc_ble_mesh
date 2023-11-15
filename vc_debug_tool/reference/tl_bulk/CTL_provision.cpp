/********************************************************************************************************
 * @file	CTL_provision.cpp
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
// CTL_provision.cpp : implementation file
//
#include "stdafx.h"
#include "tl_ble_module.h"
#include "ScanDlg.h"
#include "tl_ble_moduleDlg.h"
#include "TLMeshDlg.h"
#include "usbprt.h"
#include "CTL_provision.h"
#include "rapid_json_interface.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
CTL_provision *p_ctl_pro;
CComboBox* cFil_Type_Sel;
CEdit *ce_mac_list;	
/////////////////////////////////////////////////////////////////////////////
int netidx =0;
int pro_self_flag =0;
provison_net_info_str net_info;
provision_appkey_t vc_dlg_appkey;

unsigned char list_filter_type=0;
u8 set_mac_enable =1;
CString csNetKey=_T("11 22 c2 c3 c4 c5 c6 c7 d8 d9 da db dc dd de df");
CString csNetKeyIndex=_T("00 00");				// NET_KEY_PRIMARY
CString csAppKey=_T("60 96 47 71 73 4f bd 76 e3 b4 05 19 d1 d9 4a 48");
CString csAppKeyIndex=_T("00 00");				// APP_KEY_PRIMARY
CString csIvi_update_index=_T("00 00 00 01");
CString csUnicast=_T("01 00");
CString csCmd=_T("a3 ff 00 00 00 00 02 00 ff ff 82 02 01 00"); 
CString csListData = _T("01 00 ff ff");
CString csNode_adr=_T("02 00");


u16 get_win32_prov_unicast_adr()
{
    return net_info.unicast_address;
}

int set_win32_prov_unicast_adr(u16 adr)
{
    net_info.unicast_address = adr;
    return 1;
}

unsigned char dkri_check_en =0;
unsigned char Cmd_dat[40];
unsigned char tmp_cmd[42];
unsigned char tmp_cmd_len;

void set_dkri_check_proc(u8 en)
{
	dkri_check_en = en;
}

CTL_provision::CTL_provision(CWnd* pParent /*=NULL*/)
	: CDialog(CTL_provision::IDD, pParent)
	, m_fast_prov_mode(FALSE)
{
	//{{AFX_DATA_INIT(CTL_provision)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	p_ctl_pro =this;
}
int vc_ivi_update_flag =0;
void CTL_provision::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NETWORK_KEY, csNetKey);
	DDV_MaxChars(pDX, csNetKey,256);
	DDX_Text(pDX, IDC_KEY_INDEX,csNetKeyIndex);
	DDV_MaxChars(pDX, csNetKeyIndex, 256);
	DDX_Text(pDX, IDC_APPKEY, csAppKey);
	DDV_MaxChars(pDX, csAppKey,256);
	DDX_Text(pDX, IDC_APPKEY_INDEX, csAppKeyIndex);
	DDV_MaxChars(pDX, csAppKeyIndex,256);
	DDX_Text(pDX, IDC_IV_UPDATE_FLAG,vc_ivi_update_flag);
	DDX_Text(pDX, IDC_IV_INDEX,csIvi_update_index);
	DDV_MaxChars(pDX, csIvi_update_index, 256);
	DDX_Text(pDX, IDC_UNICAST_ADR,csUnicast);
	DDV_MaxChars(pDX, csUnicast, 256);

	// list dispatch part 
	//	DDX_Text(pDX, IDC_COM_FILTER_TYPE,list_filter_type);
	DDX_Text(pDX, IDC_EDIT_FILTER_DATA, csListData);
	DDV_MaxChars(pDX, csListData,256);

	DDX_Text(pDX, IDC_CMD_DAT,csCmd);
	DDV_MaxChars(pDX, csUnicast, 256);

	DDX_Text(pDX, IDC_NODE_ADR,csNode_adr);
	DDV_MaxChars(pDX, csNode_adr, 256);
	//{{AFX_DATA_MAP(CTL_provision)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	//set_unicast_dlg_part();
	if(net_info.unicast_address ==0){
		net_info.unicast_address =1;
	}
	//	set_unicast_dlg_part();
	DDX_Check(pDX, IDC_FAST_PROV, m_fast_prov_mode);
}

#define MSG_UPDATE_DATA   12345

BEGIN_MESSAGE_MAP(CTL_provision, CDialog)
	//{{AFX_MSG_MAP(CTL_provision)
	ON_BN_CLICKED(IDC_PROVISION_START, OnProvisionStart)
	ON_BN_CLICKED(IDC_PRO_DISCONNECT, OnProDisconnect)
	ON_BN_CLICKED(IDC_START_SEND, OnStartSend)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUT_ADD_MAC, &CTL_provision::OnBnClickedButAddMac)
	ON_BN_CLICKED(IDC_BUT_RM_MAC, &CTL_provision::OnBnClickedButRmMac)
	ON_BN_CLICKED(IDC_BUT_GET_FILTER_STS, &CTL_provision::OnBnClickedButGetFilterSts)
	ON_BN_CLICKED(IDC_BUT_SET_FILTER_TYPE, &CTL_provision::OnBnClickedButSetFilterType)
	ON_BN_CLICKED(IDC_SET_PRO, &CTL_provision::OnBnClickedSetPro)
	ON_BN_CLICKED(IDC_APPKEY_BIND_ALL, &CTL_provision::OnBnClickedAppkeyBindAll)
	ON_BN_CLICKED(IDC_DKRI_PROV, &CTL_provision::OnBnClickedDkriProv)
	ON_BN_CLICKED(IDC_CANDI_EN, &CTL_provision::OnBnClickedCandiEn)
	ON_MESSAGE(MSG_UPDATE_DATA, &OnUpdateData)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTL_provision message handlers

LRESULT CTL_provision::OnUpdateData(WPARAM wparam, LPARAM lparam)
{
	UpdateData(TRUE);
	return 0;
}

void add_mac_to_buf()
{
	unsigned char i=0;
	for(i=0;i<mac_str.index;i++){
		if(!memcmp(mac_str.mac+6*i,mac_str.mac_tmp,6)){
			return;
		}
	}
	memcpy(mac_str.mac+6*mac_str.index,mac_str.mac_tmp,6);
	mac_str.index++;
}
u8 set_provision_dat(u8 *tmp_dat)
{
	memcpy(tmp_dat,prov_newkey,16);
	memcpy(tmp_dat+16,prov_key_index,2);
	tmp_dat[18] = (u8)vc_ivi_update_flag;
	memcpy(tmp_dat+19,prov_iv_index,4);
	memcpy(tmp_dat+23,prov_unicast_address,2);	
	return 1;
}
u8 set_provision_key_data()
{
	u8 tmp_dat[25];
	set_provision_dat(tmp_dat);
	set_app_key_pro_data(tmp_dat,25);
	return 1;
}

BOOL CTL_provision::SetButton_sts()
{
	GetDlgItem(IDC_APPKEY_BIND_ALL)->EnableWindow(0);
	if(ble_module_id_is_gateway()){
		if(pro_self_flag){
				((CButton*)GetDlgItem(IDC_SET_PRO))->EnableWindow(0);
				((CButton*)GetDlgItem(IDC_PROVISION_START))->EnableWindow(1);
				GetDlgItem(IDC_NETWORK_KEY)->EnableWindow(FALSE);
				GetDlgItem(IDC_KEY_INDEX)->EnableWindow(FALSE);
				GetDlgItem(IDC_IV_INDEX)->EnableWindow(FALSE);
				GetDlgItem(IDC_IV_UPDATE_FLAG)->EnableWindow(FALSE);
				Restore_provision_data();	
		}else{
				((CButton*)GetDlgItem(IDC_SET_PRO))->EnableWindow(1);
				((CButton*)GetDlgItem(IDC_PROVISION_START))->EnableWindow(0);
				((CButton*)GetDlgItem(IDC_PRO_DISCONNECT))->EnableWindow(0);
				GetDlgItem(IDC_NETWORK_KEY)->EnableWindow(TRUE);
				GetDlgItem(IDC_KEY_INDEX)->EnableWindow(TRUE);
				GetDlgItem(IDC_IV_INDEX)->EnableWindow(TRUE);
				GetDlgItem(IDC_IV_UPDATE_FLAG)->EnableWindow(FALSE);
 				provison_net_info_str *p_net = &net_info;
				csUnicast.Format("%02x %02x",p_net->unicast_address&0xff,p_net->unicast_address>>8);
				// generate netkey randomly 
				win32_create_rand_buf(p_net->net_work_key,sizeof(p_net->net_work_key));
				set_provision_dlg_part(p_net->net_work_key,sizeof(p_net->net_work_key),
				&csNetKey,IDC_NETWORK_KEY);
		}
	}else{
		if(pro_self_flag){
			((CButton*)GetDlgItem(IDC_SET_PRO))->EnableWindow(0);
			((CButton*)GetDlgItem(IDC_PROVISION_START))->EnableWindow(1);
			// use json internal parameter part 
			GetDlgItem(IDC_NETWORK_KEY)->EnableWindow(FALSE);
			GetDlgItem(IDC_KEY_INDEX)->EnableWindow(FALSE);
			GetDlgItem(IDC_IV_INDEX)->EnableWindow(FALSE);
			GetDlgItem(IDC_IV_UPDATE_FLAG)->EnableWindow(FALSE);
			set_provision_dlg_part(net_info.net_work_key,sizeof(net_info.net_work_key),
				&csNetKey,IDC_NETWORK_KEY);
			set_provision_dlg_part((u8 *)&(net_info.key_index),sizeof(net_info.key_index),
				&csNetKeyIndex,IDC_KEY_INDEX);
			set_provision_dlg_part((u8 *)(net_info.iv_index),sizeof(net_info.iv_index),
				&csIvi_update_index,IDC_IV_INDEX);
			set_provision_dlg_part((u8 *)&(net_info.unicast_address),sizeof(net_info.unicast_address),
				&csUnicast,IDC_UNICAST_ADR);
			set_provision_dlg_part((u8 *)&(vc_dlg_appkey.apk_idx),sizeof(vc_dlg_appkey.apk_idx),
				&csAppKeyIndex,IDC_APPKEY_INDEX);
			set_provision_dlg_part((u8 *)&(vc_dlg_appkey.app_key),sizeof(vc_dlg_appkey.app_key),
				&csAppKey,IDC_APPKEY);
			GetDlgItem(IDC_APPKEY_INDEX)->EnableWindow(FALSE);
			GetDlgItem(IDC_APPKEY)->EnableWindow(FALSE);
			
			
		}else{
			if(is_exist_valid_network_key()){
				((CButton*)GetDlgItem(IDC_SET_PRO))->EnableWindow(0);
				GetDlgItem(IDC_NETWORK_KEY)->EnableWindow(FALSE);
				GetDlgItem(IDC_KEY_INDEX)->EnableWindow(FALSE);
				GetDlgItem(IDC_IV_INDEX)->EnableWindow(FALSE);
				GetDlgItem(IDC_IV_UPDATE_FLAG)->EnableWindow(FALSE);
				Restore_provision_data();	
			}else{
				((CButton*)GetDlgItem(IDC_PROVISION_START))->EnableWindow(0);
				((CButton*)GetDlgItem(IDC_PRO_DISCONNECT))->EnableWindow(0);
				GetDlgItem(IDC_IV_UPDATE_FLAG)->EnableWindow(FALSE);
				provison_net_info_str *p_net = &net_info;
				// generate netkey randomly 
				win32_create_rand_buf(p_net->net_work_key,sizeof(p_net->net_work_key));
				set_provision_dlg_part(p_net->net_work_key,sizeof(p_net->net_work_key),
				&csNetKey,IDC_NETWORK_KEY);
				u8 tmp[128], result[128];
				sprintf_s ((char *)tmp, sizeof(tmp), "%s", (const char *)csAppKeyIndex);
				Text2Bin(result, tmp);
				vc_dlg_appkey.apk_idx = result[0] + (result[1]<<8);
				sprintf_s ((char *)tmp, sizeof(tmp), "%s", (const char*)csAppKey);
				Text2Bin(vc_dlg_appkey.app_key, tmp);
				csUnicast.Format("%02x %02x",p_net->unicast_address&0xff,p_net->unicast_address>>8);
			}	
		}
	}
	return TRUE;
}

void CTL_provision::update_ivi_edit(u8 *p_ivi)
{
	provison_net_info_str *p_net = &net_info;
	memcpy(p_net->iv_index,p_ivi,sizeof(p_net->iv_index));
	GetDlgItem(IDC_IV_INDEX)->EnableWindow(TRUE);
	set_provision_dlg_part((u8 *)(net_info.iv_index),sizeof(net_info.iv_index),
		&csIvi_update_index,IDC_IV_INDEX);
	GetDlgItem(IDC_IV_INDEX)->EnableWindow(FALSE);
}



BOOL CTL_provision::OnInitDialog() 
{
	CDialog::OnInitDialog();
	rp_dkri_para_init();
	cFil_Type_Sel = (CComboBox*)GetDlgItem(IDC_COM_FILTER_TYPE);
	cFil_Type_Sel->SetCurSel(0);
	ce_mac_list = (CEdit *)GetDlgItem(IDC_EDIT_FILTER_DATA);
	return TRUE;
}

int CTL_provision::GetProvision_para()
{
	unsigned char tmp_str[128];
	sprintf_s ((char *)tmp_str, sizeof(tmp_str), "%s", (const char*)csUnicast);
	u16 address;
	Text2Bin((u8 *)&address, tmp_str);
	if(is_own_ele(address)){
	    AfxMessageBox("invalid unicast addr: same with VC addr\r\n");
	    return 0;
	}
	memset(prov_unicast_address, 0x00, sizeof(prov_unicast_address));
	u8 unicast_adr_cnt =0;
	unicast_adr_cnt = Text2Bin(prov_unicast_address, tmp_str);
	if(unicast_adr_cnt!=2){
		AfxMessageBox("the unicast adr length is 2 ");
	    return 0;
	}else{
		u16 unicast_adr_temp =0;
		unicast_adr_temp = prov_unicast_address[0]+(prov_unicast_address[1]<<8);
		if(unicast_adr_temp==0 || unicast_adr_temp>=0x8000){
			AfxMessageBox("the unicast range should be 1~0x7fff");
	    	return 0;
		}
	}
	u8 netkey_len;
	sprintf_s ((char *)tmp_str, sizeof(tmp_str), "%s", (const char*)csNetKey);
	netkey_len = Text2Bin(prov_newkey, tmp_str);
	if(netkey_len!=16){
		AfxMessageBox("the length of the netkey should be 16");
	    return 0;
	}
	u8 key_index_len =0;
	sprintf_s ((char *)tmp_str, sizeof(tmp_str), "%s", (const char*)csNetKeyIndex);
	key_index_len = Text2Bin(prov_key_index, tmp_str);
	if(key_index_len !=2){
		AfxMessageBox("the length of the key index should be 2");
	    return 0;
	}else{
		u16 netkey_idx =0;
		netkey_idx = prov_key_index[0]+(prov_key_index[1]<<8);
		if(netkey_idx>4096){
			AfxMessageBox("the range of the netkey idx range is 0~4095");
	   	 	return 0;
		}
	}
	u8 iv_index_len =0;
	sprintf_s ((char *)tmp_str, sizeof(tmp_str), "%s", (const char *)csIvi_update_index);
	iv_index_len = Text2Bin(prov_iv_index, tmp_str);
	if(iv_index_len!=4){
		AfxMessageBox("the length of the iv_index length is 4");
	    return 0;
	}
	if(vc_ivi_update_flag>0xff ||vc_ivi_update_flag<0){
		AfxMessageBox("the ivi update flag range is from 0 to 255");
	    return 0;
	}
	return 1;
}

int send_static_oob2gateway_with_check(u8 *node_uuid)
{
	u8 static_oob[32];
	u8 oob_len = get_auth_value_by_uuid(node_uuid, static_oob);
	if (oob_len != 16 && oob_len != 32) {
		// invalid len ,and do nothing
		return 0;
	}
	
	// send the cmd into the dongle
	unsigned char gateway_buf[64];
	gateway_buf[0] = HCI_CMD_GATEWAY_CTL & 0xFF;
	gateway_buf[1] = (HCI_CMD_GATEWAY_CTL >> 8) & 0xFF;
	gateway_buf[2] = HCI_GATEWAY_CMD_STATIC_OOB_RSP;
	memcpy(gateway_buf + 3, static_oob, oob_len);
	WriteFile_host_handle(gateway_buf, oob_len + 3);

	return oob_len;
}

void CTL_provision::OnProvisionStart() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(!GetProvision_para()){
		return ;
	}
	if(json_can_do_provision() == 0){
		LOG_MSG_INFO(TL_LOG_GATT_PROVISION,0,0,"it can not do provision now ,need to connect proxy device to eable ivi");
		return ;
	}
	LOG_MSG_INFO(TL_LOG_GATT_PROVISION,0,0,"start provision for the device");
	if(!ble_module_id_is_gateway()&&!m_fast_prov_mode){

	//json_del_mesh_vc_node_info(sigmesh_node_uuid);
		if(is_provision_working()){
			LOG_MSG_ERR(TL_LOG_GATT_PROVISION,0,0,"vc provision is in process");
			return ;
		}
	    if(is_rp_working()){
			LOG_MSG_ERR(TL_LOG_REMOTE_PROV,0,0,"vc remote-prov is in process");
			return ;
		}
	}
	set_provision_key_data();
	gatt_pro_para_mag_init();
	net_info.unicast_address = provision_mag.pro_net_info.unicast_address = prov_unicast_address[0]|(prov_unicast_address[1]<<8);
	if(provision_mag.pro_net_info.unicast_address>0x7fff){
		AfxMessageBox("the unicast adr should be 1~0x7fff");
	    return ;
	}

	if(dkri_check_en == 0){
		LOG_MSG_INFO(TL_LOG_COMMON,0, 0,"is this new address 0x%04x already used in VC node info ?", provision_mag.pro_net_info.unicast_address);
	}
	
	if(dkri_check_en == 0 && get_VC_node_info(provision_mag.pro_net_info.unicast_address, 0) !=0){
		// only int the normal mode ,need to check the provision net information part 
		AfxMessageBox("the unicast adr has alreay been used");
	    return ;
	}
	if(!unicast_is_in_range_or_not(net_info.unicast_address,vc_uuid)){// detect the unicast is valid or not 
		return ;
	}
	cache_init(ADR_ALL_NODES);
	if(filter_send_flag){
	    filter_send_flag = 0;
	}

	u16 ak_idx=0;
	u8 app_key[255];	
	if(m_fast_prov_mode){	
		APP_reset_vendor_id(0x0211);		
		ak_idx = (u16)strtol(csAppKeyIndex,NULL,16);
		unsigned char tmp_str[128];

		u8 ak_idx_len =0;
		sprintf_s ((char *)tmp_str, sizeof(tmp_str), "%s", (const char*)csAppKeyIndex);
		ak_idx_len = Text2Bin((u8 *)(&ak_idx), tmp_str);
		if(ak_idx_len!=2 ||ak_idx>4095 ){
			AfxMessageBox("the apk idx len is 2, and the value should be  0~4095");
			return ;
		}

		u8 app_key_len =0;
		sprintf_s ((char *)tmp_str, sizeof(tmp_str), "%s", (const char*)csAppKey);
		app_key_len = Text2Bin(app_key, tmp_str);
		if(app_key_len!=16){
			AfxMessageBox("the appkey len is 16 bytes");
			return ;
		}
	}

	if(ble_module_id_is_gateway()){
		if(m_fast_prov_mode){
			cfg_cmd_ak_add(ele_adr_primary, netidx, ak_idx, app_key);
			gateway_start_fast_provision(0xffff, net_info.unicast_address);
		}
		else{
			u8 gateway_provisionee_buf[25];
			set_provision_dat(gateway_provisionee_buf);
			if(prov_mode_is_rp_mode()){
				gateway_send_rp_start(gateway_provisionee_buf);
			}else{
				gateway_set_node_provision_para(gateway_provisionee_buf);
			}	
			send_static_oob2gateway_with_check(sigmesh_node_uuid);
		}		
	}else{
		if(m_fast_prov_mode){					
			mesh_app_key_set_and_bind(netidx, app_key, ak_idx, 1);
			mesh_fast_prov_start(0xffff, net_info.unicast_address);
		}
		else{
			//json_del_mesh_vc_node_info(sigmesh_node_uuid);
			start_provision_process();
		}
	}
	p_ctl_pro->UpdateData();
}

void CTL_provision::OnProDisconnect() 
{
	// TODO: Add your control notification handler code here
	clear_all_white_list();
	disconnect_device();
	connect_flag =0;
	report_adv_opera(0);
	this->OnCancel();
	set_mac_enable =1;

}

BOOL CTL_provision::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CDialog::PreTranslateMessage(pMsg);
}

void CTL_provision::OnStartSend() 
{
	// TODO: Add your control notification handler code here
	u8 len;
	unsigned char tmp_str[128];
	p_ctl_pro->UpdateData();
	sprintf_s ((char *)tmp_str, sizeof(tmp_str), "%s", (const char*)csCmd);
	len = Text2Bin(Cmd_dat, tmp_str);
	WriteFile_handle(Cmd_dat,len, 0, 0);
	return;

}
void CTL_provision::SendCmd(u8 opcode){
	UpdateData();
	u8 filter_type;
	filter_type =cFil_Type_Sel->GetCurSel();
	
	unsigned char tmp_str[128];
	sprintf_s ((char *)tmp_str, sizeof(tmp_str), "%s", (const char*)csListData);
	tmp_cmd_len = Text2Bin(Cmd_dat, tmp_str);	
	mesh_proxy_set_filter_cmd(opcode,filter_type,Cmd_dat,tmp_cmd_len);
}

void CTL_provision::OnBnClickedButAddMac()
{
	// TODO: 在此添加控件通知处理程序代码
	SendCmd(PROXY_FILTER_ADD_ADR);
}


void CTL_provision::OnBnClickedButRmMac()
{
	// TODO: 在此添加控件通知处理程序代码
	SendCmd(PROXY_FILTER_RM_ADR);
}


void CTL_provision::OnBnClickedButGetFilterSts()
{
	// TODO: 在此添加控件通知处理程序代码
	//PROXY_FILTER_STATUS
//	u8 cmd_dat[3]={0,0,0};
	// for test ,and wait for done 
//	mesh_tx_cmd_layer_cfg_primary(PROXY_FILTER_STATUS,cmd_dat,1,0x7755);

}
void CTL_provision::SetFilterSts(u8 *p_dat,u8 len)
{
	//the first byte is filter type
	cFil_Type_Sel->SetCurSel(p_dat[1]);
	unsigned char str[4069];
    Bin2Text(str, p_dat+2, len-2);
	CString csMac;
	csMac.Format("%s",str);
	ce_mac_list->SetWindowTextA(csMac);
}

void CTL_provision::OnBnClickedButSetFilterType()
{
	// TODO: 在此添加控件通知处理程序代码
	SendCmd(PROXY_FILTER_SET_TYPE);

}
void CTL_provision::Restore_provision_data()
{
	int i=0;
	provison_net_info_str *p_net;
	p_net = &(net_info);
	CString csIviFlag;
	csIviFlag.Format("%02x",p_net->flags);
	csNetKeyIndex.Format("%02x %02x",(p_net->key_index)>>8,p_net->key_index&0xff);
	csIvi_update_index.Format("%02x %02x %02x %02x",p_net->iv_index[0],p_net->iv_index[1],
								p_net->iv_index[2],p_net->iv_index[3]);
	csUnicast.Format("%02x %02x",p_net->unicast_address&0xff,p_net->unicast_address>>8);
	csNetKey.Empty();
	for(i=0;i<16;i++){
		CString csTmp;
		csTmp.Format("%02x ",p_net->net_work_key[i]);
		csNetKey += csTmp;
	}
	set_provision_dlg_part((u8 *)&(vc_dlg_appkey.apk_idx),sizeof(vc_dlg_appkey.apk_idx),
				&csAppKeyIndex,IDC_APPKEY_INDEX);
	set_provision_dlg_part((u8 *)&(vc_dlg_appkey.app_key),sizeof(vc_dlg_appkey.app_key),
				&csAppKey,IDC_APPKEY);
	GetDlgItem(IDC_APPKEY_INDEX)->EnableWindow(FALSE);
	GetDlgItem(IDC_APPKEY)->EnableWindow(FALSE);
	((CEdit *)GetDlgItem(IDC_KEY_INDEX))->SetWindowText(csNetKeyIndex);
	((CEdit *)GetDlgItem(IDC_IV_INDEX))->SetWindowText(csIvi_update_index);
	((CEdit *)GetDlgItem(IDC_UNICAST_ADR))->SetWindowText(csUnicast);
	((CEdit *)GetDlgItem(IDC_IV_UPDATE_FLAG))->SetWindowText(csIviFlag);
	((CEdit *)GetDlgItem(IDC_NETWORK_KEY))->SetWindowText(csNetKey);
	//UpdateData(FALSE);
}
void CTL_provision::set_unicast_dlg_part()
{
	BYTE prov_str[30];
	Bin2Text(prov_str,(u8 *)(&(net_info.unicast_address)),2);
	csUnicast.SetString((const char *)prov_str);
	CEdit *ce_dlg = (CEdit *)GetDlgItem(IDC_UNICAST_ADR);
	ce_dlg->SetWindowTextA(csUnicast);
}

void CTL_provision::set_provision_dlg_part(u8 *data,int len ,CString *cs_edit,int id)
{
	BYTE prov_str[256];
	Bin2Text(prov_str,data,len);
	cs_edit->SetString((const char *)prov_str);
	CEdit *ce_dlg = (CEdit *)GetDlgItem(id);
	ce_dlg->SetWindowTextA(*cs_edit);
}

void CTL_provision::OnBnClickedSetPro() // provision self
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_pMeshDlg->InitStatus();
	if(!GetProvision_para()){
		return ;
	}
	if(json_can_do_provision() == 0){
		LOG_MSG_INFO(TL_LOG_GATT_PROVISION,0,0,"it can not do provision slef now ,need to connect proxy device to eable ivi");
		return ;
	}	
	unsigned char tmp_str[128];
	u16 ak_idx = (u16)strtol(csAppKeyIndex,NULL,16);
	u8 ak_idx_len =0;
	sprintf_s ((char *)tmp_str, sizeof(tmp_str), "%s", (const char*)csAppKeyIndex);
	ak_idx_len = Text2Bin((u8 *)(&ak_idx), tmp_str);
	if(ak_idx_len!=2 ||ak_idx>4095 ){
		AfxMessageBox("the apk idx len is 2, and the value should be  0~4095");
		return ;
	}
	vc_dlg_appkey.apk_idx = ak_idx;
	if(vc_ivi_update_flag>0xff ||vc_ivi_update_flag<0){
		AfxMessageBox("the ivi update flag range is from 0 to 255");
	    return ;
	}
    BYTE tmp[256] = {0};
    sprintf_s ((char *)tmp, sizeof(tmp), "%s", (const char*)csAppKey);
    Text2Bin(vc_dlg_appkey.app_key, tmp);
	// set the prov_unicast_address to special part 
	// set the info 
	provison_net_info_str net_info_tab;
	provison_net_info_str *p_net_str = &net_info_tab;
	set_provision_dat((u8 *)(p_net_str));
#if JSON_FILE_ENABLE	
	if(ble_module_id_is_gateway()){
		gateway_set_provisioner_para((u8 *)(p_net_str));//provision the gateway part 
		gateway_set_prov_devkey(p_net_str->unicast_address,vc_dev_key);
	}
	if(!pro_self_flag){
		netidx = prov_key_index[0]+(prov_key_index[1]<<8);
	    provision_self_to_json(p_net_str,&netidx,vc_uuid,vc_dev_key);
		//set the dev key part 
		set_dev_key(vc_dev_key);
		// need to update the part of the unicast adr part 
		set_provision_dlg_part((u8 *)&(p_net_str->unicast_address),sizeof(p_net_str->unicast_address),
					&csUnicast,IDC_UNICAST_ADR);
        json_add_provisioner_info(p_net_str,vc_uuid);
        win32_create_rand_buf(json_database.mesh_uuid, sizeof(json_database.mesh_uuid));
		write_json_file_doc(FILE_MESH_DATA_BASE);
		set_ini_provioner_mesh_uuid(json_database.mesh_uuid);
		pro_self_flag=1;
	}
#endif
    if(mesh_provision_par_set_dir(p_net_str)!=0){
		AfxMessageBox("please input the unicast adr, the highest bit should be 0");
		return;
	}
	((CButton*)GetDlgItem(IDC_PROVISION_START))->EnableWindow(1);
		((CButton*)GetDlgItem(IDC_PRO_DISCONNECT))->EnableWindow(1);
	LOG_MSG_INFO(TL_LOG_GATT_PROVISION,0,0,"Set internal provision success");
	GetDlgItem(IDC_SET_PRO)->EnableWindow(FALSE);
	// update the net info part 
	if(ble_module_id_is_gateway()){
		g_ele_cnt = vc_node_ele_cnt;
	}
	memcpy(&net_info,(u8 *)(p_net_str),sizeof(provison_net_info_str));
	#if DEBUG_SHOW_VC_SELF_EN
	OnBnClickedAppkeyBindAll();
	#endif
	
	net_info.unicast_address= prov_unicast_address[0]|(prov_unicast_address[1]<<8);
	net_info.unicast_address += g_ele_cnt;
	set_unicast_dlg_part();
	mesh_misc_store();
	ele_adr_primary = prov_unicast_address[0]|(prov_unicast_address[1]<<8);
	VC_node_dev_key_save(ele_adr_primary, mesh_key.dev_key,g_ele_cnt);
	VC_node_cps_save(gp_page0, ele_adr_primary, SIZE_OF_PAGE0_LOCAL);
#if JSON_FILE_ENABLE
    if(ble_module_id_is_gateway()){
         mesh_json_set_node_info(ele_adr_primary,gw_mac);
    }else{
        // we can use the tbl_mac as the mac adr 
        mesh_json_set_node_info(ele_adr_primary,tbl_mac);
    } 
    json_set_appkey_bind_self_proc(vc_uuid, swap_u16_data(ak_idx));
	write_json_file_doc(FILE_MESH_DATA_BASE);
#endif
	GetDlgItem(IDC_NETWORK_KEY)->EnableWindow(FALSE);
	GetDlgItem(IDC_KEY_INDEX)->EnableWindow(FALSE);
	GetDlgItem(IDC_IV_INDEX)->EnableWindow(FALSE);
	GetDlgItem(IDC_IV_UPDATE_FLAG)->EnableWindow(FALSE);
	GetDlgItem(IDC_APPKEY_INDEX)->EnableWindow(FALSE);
	UpdateData(FALSE);
}
void CTL_provision::set_keybind_window_enable(u8 en)
{
	GetDlgItem(IDC_APPKEY_BIND_ALL)->EnableWindow(en);
}

void CTL_provision::set_provision_window_enable(u8 en)
{
	GetDlgItem(IDC_PROVISION_START)->EnableWindow(en);
}

void provision_end_callback(u8 reason)
{
	if(reason == PROV_NORMAL_RET){
		u16 adr_max;
		if(ble_module_id_is_gateway() && !p_ctl_pro->m_fast_prov_mode){
			adr_max = net_info.unicast_address + vc_node_ele_cnt;
		}else{
			adr_max = net_info.unicast_address +gatt_get_node_ele_cnt(net_info.unicast_address);
		}
		if(adr_max>get_ini_unicast_max()){
			set_ini_unicast_max(adr_max);
		}

		if (!p_ctl_pro->m_fast_prov_mode)
		{
			if(g_module_dlg->m_fastbind){
				//p_ctl_pro->OnBnClickedAppkeyBindAll();
				p_ctl_pro->set_keybind_window_enable(1);
				p_ctl_pro->set_provision_window_enable(0);
			}
			else{
				p_ctl_pro->set_keybind_window_enable(1);
				p_ctl_pro->set_provision_window_enable(0);	
			}
		}		

		// add the information into the doc part
#if JSON_FILE_ENABLE
		if(ble_module_id_is_gateway()){
            json_add_net_info_doc(gw_dev_uuid,gw_dev_mac);// doult for the node adr part 
		}else{
			LOG_MSG_INFO(TL_LOG_WIN32,0,0,"provision_end_callback PROV_NORMAL_RET",0);
    		json_add_net_info_doc(sigmesh_node_uuid,sigmesh_node_mac);
    	}
    	write_json_file_doc(FILE_MESH_DATA_BASE);
#endif
	}else{

	}
}

// fast provision default composition data
u8 composition_data_CT[]={0x11,0x02,0x01,0x00,0x33,0x33,0x69,0x00,0x07,0x00,0x00,0x00,0x0c,0x01,0x00,0x00,
	0x02,0x00,0x03,0x00,0x00,0x10,0x02,0x10,0x04,0x10,0x06,0x10,0x07,0x10,0x00,0x13, 
	0x01,0x13,0x03,0x13,0x04,0x13,0x11,0x02,0x00,0x00,0x00,0x00,0x02,0x00,0x02,0x10, 
	0x06,0x13
};

u8 composition_data_HSL[]={0x11,0x02,0x02,0x00,0x33,0x33,0x69,0x00,0x07,0x00,0x00,0x00,0x0c,0x01,0x00,0x00,
	0x02,0x00,0x03,0x00,0x00,0x10,0x02,0x10,0x04,0x10,0x06,0x10,0x07,0x10,0x00,0x13,
	0x01,0x13,0x07,0x13,0x08,0x13,0x11,0x02,0x00,0x00,0x00,0x00,0x02,0x00,0x02,0x10,
	0x0a,0x13,0x00,0x00,0x02,0x00,0x02,0x10,0x0b,0x13
};

u8 composition_data_CT_HSL[]={0x11,0x02,0x05,0x00,0x33,0x33,0x69,0x00,0x07,0x00,0x00,0x00,0x0e,0x01,0x00,0x00,
	0x02,0x00,0x03,0x00,0x00,0x10,0x02,0x10,0x04,0x10,0x06,0x10,0x07,0x10,0x00,0x13,
	0x01,0x13,0x03,0x13,0x04,0x13,0x07,0x13,0x08,0x13,0x11,0x02,0x00,0x00,0x00,0x00,
	0x02,0x00,0x02,0x10,0x06,0x13,0x00,0x00,0x02,0x00,0x02,0x10,0x0a,0x13,0x00,0x00,
	0x02,0x00,0x02,0x10,0x0b,0x13
};

u8 composition_data_XYL[]={0x11,0x02,0x03,0x00,0x33,0x33,0x69,0x00,0x07,0x00,0x00,0x00,0x0c,0x01,0x00,0x00,
	0x02,0x00,0x03,0x00,0x00,0x10,0x02,0x10,0x04,0x10,0x06,0x10,0x07,0x10,0x00,0x13,
	0x01,0x13,0x0c,0x13,0x0d,0x13,0x11,0x02,0x00,0x00,0x00,0x00,0x01,0x00,0x02,0x10,
	0x00,0x00,0x01,0x00,0x02,0x10
};

u8 composition_data_PANNEL[]={0x11,0x02,0x07,0x00,0x33,0x33,0x69,0x00,0x07,0x00,0x00,0x00,0x04,0x02,0x00,0x00,
	0x02,0x00,0x03,0x00,0x00,0x10,0x11,0x02,0x00,0x00,0x11,0x02,0x01,0x00,0x00,0x00,
	0x01,0x01,0x00,0x10,0x11,0x02,0x00,0x00,0x00,0x00,0x01,0x01,0x00,0x10,0x11,0x02,
	0x00,0x00
};

u8 composition_data_LPN[]= {0x11,0x02,0x01,0x02,0x33,0x36,0x69,0x00,0x0a,0x00,0x00,0x00,0x05,0x01,0x00,0x00,
    0x02,0x00,0x03,0x00,0x00,0x10,0x02,0x10,0x11,0x02,0x00,0x00
};

extern void uuid_create_by_mac(u8* mac, u8* uuid);
extern u32 get_cps_vd_model_id(const mesh_element_head_t *p_ele, u32 index);
void mesh_fast_prov_node_info_callback(u8 *dev_key, u16 node_addr, u16 pid)
{
	provison_net_info_str *p_net_info = (provison_net_info_str *)gatt_pro_dat;
	u8 dev_uuid_fast[16];
#if 1
	uuid_create_by_mac(dev_key, dev_uuid_fast); // low 6 bytes of dev_key is Mac
#else
	memcpy(dev_uuid_fast, dev_key, 16);
#endif

	p_net_info->unicast_address = node_addr;  
	if(ble_module_id_is_gateway()){
		memcpy(gw_dev_uuid, dev_uuid_fast, 16);
		memcpy(gw_dev_mac, dev_key, 6);
		memcpy(gw_dev_key, dev_key, 16);	
	}
	else{
		memcpy(sigmesh_node_uuid, dev_uuid_fast, 16);
		memcpy(sigmesh_node_mac, dev_key, 6);
		memcpy(gatt_dev_key, dev_key, 16);	
	}
	
	provision_end_callback(PROV_NORMAL_RET);
	net_info.unicast_address = node_addr + gatt_get_node_ele_cnt(net_info.unicast_address);
#if JSON_FILE_ENABLE	
	mesh_node_str* p_node =0;
	p_node = json_mesh_find_node(node_addr);
	mesh_page0_t * p_mesh_page = 0;
	int mesh_page_size = 0;
	pid &= BIT_MASK_LEN(PID_DEV_TYPE_LEN); // ignore mcu chip type
	if(LIGHT_TYPE_CT == pid){
		p_mesh_page = (mesh_page0_t *)&composition_data_CT;
		mesh_page_size = sizeof(composition_data_CT);
		VC_node_cps_save((mesh_page0_t *)&composition_data_CT, node_addr, sizeof(composition_data_CT));
	}
	else if (LIGHT_TYPE_HSL == pid){
		p_mesh_page = (mesh_page0_t *)&composition_data_HSL;
		mesh_page_size = sizeof(composition_data_HSL);
		VC_node_cps_save((mesh_page0_t *)&composition_data_HSL, node_addr, sizeof(composition_data_HSL));
	}
	else if(LIGHT_TYPE_XYL == pid){
		p_mesh_page = (mesh_page0_t *)&composition_data_XYL;
		mesh_page_size = sizeof(composition_data_XYL);
		VC_node_cps_save((mesh_page0_t *)&composition_data_XYL, node_addr, sizeof(composition_data_XYL));
	}
	else if(LIGHT_TYPE_CT_HSL == pid){
		p_mesh_page = (mesh_page0_t *)&composition_data_CT_HSL;
		mesh_page_size = sizeof(composition_data_CT_HSL);
		VC_node_cps_save((mesh_page0_t *)&composition_data_CT_HSL, node_addr, sizeof(composition_data_CT_HSL));
	}
	else if(LIGHT_TYPE_PANEL == pid){
		p_mesh_page = (mesh_page0_t *)&composition_data_PANNEL;
		mesh_page_size = sizeof(composition_data_PANNEL);
		VC_node_cps_save((mesh_page0_t *)&composition_data_PANNEL, node_addr, sizeof(composition_data_PANNEL));
	}
	else if ((PID_LPN & BIT_MASK_LEN(PID_DEV_TYPE_LEN)) == pid) {
		p_mesh_page = (mesh_page0_t*)&composition_data_LPN;
		mesh_page_size = sizeof(composition_data_LPN);
		VC_node_cps_save((mesh_page0_t*)&composition_data_LPN, node_addr, sizeof(composition_data_LPN));
	}
	else{
		p_mesh_page = (mesh_page0_t *)&composition_data_CT;
		mesh_page_size = sizeof(composition_data_CT);
		VC_node_cps_save((mesh_page0_t *)&composition_data_CT, node_addr, sizeof(composition_data_CT));
	}

	VC_node_cps_save(p_mesh_page, node_addr, mesh_page_size);
	if(p_node){
		json_set_model_doc(p_node);
		write_json_file_doc(FILE_MESH_DATA_BASE);
	}
	else{
		return;
	}
	// set default bind info in json file
	mesh_nodes_model_str *p_model;
	u8 tmp_str[128];
	u16 ak_idx=0;
	u8 app_key[128];
	sprintf_s ((char *)tmp_str, sizeof(tmp_str), "%s", (const char*)csAppKey);
	Text2Bin(app_key, tmp_str);
	sprintf_s ((char *)tmp_str, sizeof(tmp_str), "%s", (const char*)csAppKeyIndex);
	Text2Bin((u8 *)(&ak_idx), tmp_str);
	u16 app_key_idx = GET_NETKEY_INDEX(fast_prov.net_info.appkey_set.net_app_idx);
	mesh_json_appkey_add( p_node, app_key, prov_key_index[0]+(prov_key_index[1]<<8), ak_idx);

	int pos = 0;
	int offset_ele = OFFSETOF(mesh_page0_t, ele);
	const mesh_element_head_t *p_ele = &p_mesh_page->ele;
	int total_len_ele = mesh_page_size - offset_ele;
	u16 ele_adr = node_addr;
	while (pos < total_len_ele)
	{
		foreach(i, p_ele->nums){
			p_model = json_mesh_find_model(p_node,node_addr,p_ele->md_sig[i]);
			if(p_model==0){
				continue;
			}
			json_model_app_bind(p_model,app_key_idx);
		}

		foreach(i, p_ele->numv){
			u32 vd_model_id = get_cps_vd_model_id(p_ele, i);
			p_model = json_mesh_find_model(p_node,node_addr,vd_model_id);
			if(p_model==0){
				continue;
			}
			json_model_app_bind(p_model,app_key_idx);
		}

		int len_ele = get_cps_ele_len(p_ele);
		p_ele = (mesh_element_head_t *)((u8 *)p_ele + len_ele);
		pos += len_ele;
		ele_adr += 1;
	}
	write_json_file_doc(FILE_MESH_DATA_BASE);
#endif
	return;
}

int App_key_bind_end_callback(u8 event)
{
	if(event == MESH_APP_KEY_BIND_EVENT_SUC){
		if (!p_ctl_pro->m_fast_prov_mode)
		{	
			net_info.unicast_address= prov_unicast_address[0]|(prov_unicast_address[1]<<8);
			//use the ele cnt 
			if(ble_module_id_is_gateway()){
				net_info.unicast_address += vc_node_ele_cnt;
			}else{
				net_info.unicast_address += gatt_get_node_ele_cnt(net_info.unicast_address);
			}
		}
		p_ctl_pro->set_unicast_dlg_part();
		p_ctl_pro->set_keybind_window_enable(0);
		p_ctl_pro->set_provision_window_enable(1);
		mesh_misc_store();
	}else{
		
	}

	return 1;
}

u8 vc_get_bind_mode_by_uuid(u8 fast_bind,u8 *p_uuid)
{

 // dispatch by the uuid part 
    u8 bind_mode =0;
	if(fast_bind){
       if(prov_uuid_fastbind_mode(p_uuid)){
           bind_mode =1;
       }else{
           bind_mode =0;
       }
	}else{
            // provision use the normal flow 
       bind_mode = 0;
	}    
	return bind_mode;
}


void CTL_provision::OnBnClickedAppkeyBindAll()
{
	// TODO: 在此添加控件通知处理程序代码
	//UpdateData(TRUE);
	SendMessage(MSG_UPDATE_DATA, (WPARAM)0, 0);
	u16 ak_idx=0;
	u8 app_key[255];
	if(set_mac_enable){
		add_mac_to_buf();
	}
	unsigned char tmp_str[128];
	
	u8 ak_idx_len =0;
	sprintf_s ((char *)tmp_str, sizeof(tmp_str), "%s", (const char*)csAppKeyIndex);
	ak_idx_len = Text2Bin((u8 *)(&ak_idx), tmp_str);
	if(ak_idx_len!=2 ||ak_idx>4095 ){
		AfxMessageBox("the apk idx len is 2, and the value should be  0~4095");
		return ;
	}
	
	u8 app_key_len =0;
	sprintf_s ((char *)tmp_str, sizeof(tmp_str), "%s", (const char*)csAppKey);
	app_key_len = Text2Bin(app_key, tmp_str);
	if(app_key_len!=16){
		AfxMessageBox("the appkey len is 16 bytes");
		return ;
	}
	
	sprintf_s ((char *)tmp_str, sizeof(tmp_str), "%s", (const char*)csUnicast);
	Text2Bin(prov_unicast_address, tmp_str);
	u16 node_addr = prov_unicast_address[0] | (prov_unicast_address[1]<<8);
	LOG_MSG_INFO(TL_LOG_KEY_BIND,app_key,16,"start key bind and the appkey is \r\n");
 	u16 netkey_index_val =(prov_key_index[1]<<8)+prov_key_index[0];
	u8 bind_mode = 0;
	#if JSON_FILE_ENABLE
 		u16 appkey_index_little = swap_u16_data(ak_idx);
        json_proc_appkey_self(appkey_index_little,vc_uuid);
	#endif

	#if DEBUG_SHOW_VC_SELF_EN
	if(1 == node_addr){
		mesh_app_key_set_and_bind(netkey_index_val, app_key, ak_idx, 1);
		return ;
	}
	#endif
	if(ble_module_id_is_gateway()){
        bind_mode = vc_get_bind_mode_by_uuid(g_module_dlg->m_fastbind,gatt_provision_mag.device_uuid);
		gateway_set_start_keybind(bind_mode,(u8 *)(&ak_idx),app_key);		
	}else{
	    // dispatch by the uuid part 
	    bind_mode = vc_get_bind_mode_by_uuid(g_module_dlg->m_fastbind,gatt_provision_mag.device_uuid);
        mesh_kc_cfgcl_mode_para_set(ak_idx,app_key,node_addr,netkey_index_val,bind_mode);
	}
}

void CTL_provision::rp_dkri_para_init()
{
	((CComboBox*)GetDlgItem(IDC_RP_DKRI))->AddString("KEY_REFRESH");
	((CComboBox*)GetDlgItem(IDC_RP_DKRI))->AddString("ADR_REFRESH");
	((CComboBox*)GetDlgItem(IDC_RP_DKRI))->AddString("CPS_REFRESH");
	((CComboBox*)GetDlgItem(IDC_RP_DKRI))->SetCurSel(0);
}

u16 CTL_provision::Getunicast_adr_in_edit()
{
	UpdateData(TRUE);
	// get the unicast adr .
	unsigned char tmp_str[128];
	u8 prov_uni_len =0;
	sprintf_s ((char *)tmp_str, sizeof(tmp_str), "%s", (const char*)csUnicast);
	prov_uni_len = Text2Bin(prov_unicast_address, tmp_str);
	if(prov_uni_len!=2){
		AfxMessageBox("the unicast len should be 2");
	    return 0;
	}
	u16 prov_unicast_temp = (prov_unicast_address[1]<<8)+prov_unicast_address[0];
	if(prov_unicast_temp == 0 || prov_unicast_temp>=0x8000){
		AfxMessageBox("the provision unicast adr range should from 1 to 7fff");
	    return 0;
	}
	return prov_unicast_temp;
}

u16 CTL_provision::Get_node_adr_in_edit()
{
	UpdateData(TRUE);
	// get the unicast adr .
	unsigned char tmp_str[128];
	u8 node_adr[2];
	u8 prov_uni_len =0;
	sprintf_s ((char *)tmp_str, sizeof(tmp_str), "%s", (const char*)csNode_adr);
	prov_uni_len = Text2Bin(node_adr, tmp_str);
	if(prov_uni_len!=2){
		AfxMessageBox("the unicast len should be 2");
	    return 0;
	}
	u16 prov_unicast_temp = (node_adr[1]<<8)+node_adr[0];
	if(prov_unicast_temp == 0 || prov_unicast_temp>=0x8000){
		AfxMessageBox("the provision unicast adr range should from 1 to 7fff");
	    return 0;
	}
	return prov_unicast_temp;
}


void CTL_provision::OnBnClickedDkriProv()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	HANDLE NULLEVENT_PROV = CreateEvent( NULL, TRUE, FALSE, NULL ) ;
	u8 dkri = ((CComboBox*)GetDlgItem(IDC_RP_DKRI))->GetCurSel();
	u16 node_adr = Get_node_adr_in_edit();
	
	if(node_adr == 0){
		AfxMessageBox("the node adr can not be 0");
		return ;
	}
	if(get_ele_offset_by_model_VC_node_info(node_adr,SIG_MD_REMOTE_PROV_SERVER,1)== MODEL_NOT_FOUND){
		AfxMessageBox("this node not support the remote prov server model");
		return ;
	}
	// set the provision parameters update proc part 
	if(!GetProvision_para()){
		AfxMessageBox("the dkri para is invalid ");
		return ;
	}
	// set the uuid and the adr part 
	#if JSON_FILE_ENABLE
	if(dkri == RP_DKRI_NODE_ADR_REFRESH){
		u8 *p_uuid = json_get_uuid(node_adr);
		if(ble_module_id_is_gateway()){
			memcpy(gw_dev_uuid,p_uuid,16);
			memcpy(gw_dev_mac,p_uuid+10,6);
		}else{
			memcpy(sigmesh_node_uuid,p_uuid,16);
			memcpy(sigmesh_node_mac,p_uuid+10,6);
    	}
	}
	#endif
	// send the link open dkri cmd
	mesh_prov_set_cli_dkri(dkri);
	mesh_rp_start_settings(node_adr,NULL,dkri);
	WaitForSingleObject (NULLEVENT_PROV, 1000);// wait 1s,the link sts will rsp ,and then start provision procedure.
	set_dkri_check_proc(1);
	OnProvisionStart();
	set_dkri_check_proc(0);
}


void CTL_provision::OnBnClickedCandiEn()
{
	UpdateData(TRUE);
	// TODO: 在此添加控件通知处理程序代码
	u16 prov_unicast = Get_node_adr_in_edit();
	if(prov_unicast == 0){
		return ;
	}
	mesh_tx_en_devkey_candi(1);// force to send ttl ,use the device candi to proc .
	cfg_cmd_ttl_get(prov_unicast);
}
