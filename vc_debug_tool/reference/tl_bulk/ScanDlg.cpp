/********************************************************************************************************
 * @file	ScanDlg.cpp
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
#include "stdafx.h"
#include "tl_ble_module.h"
#include "ScanDlg.h"
#include "tl_ble_moduleDlg.h"
#include "TLMeshDlg.h"
#include "usbprt.h"
#include "./lib_file/gatt_provision.h"
#include "CTL_provision.h"
#include "./aes_ecb/aes_ecb.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScanDlg dialog


CScanDlg::CScanDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CScanDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScanDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_mac[0] = 1;
	nmac = 0;
}


void CScanDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScanDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScanDlg, CDialog)
	//{{AFX_MSG_MAP(CScanDlg)
	ON_BN_CLICKED(IDC_CONNECT, OnConnect)
	ON_LBN_DBLCLK(IDC_SCANLIST, OnDblclkScanlist)
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()



BOOL CScanDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CTl_ble_moduleDlg *parentDlg =  (CTl_ble_moduleDlg *)GetParent();
	
	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CScanDlg message handlers
u8  sigmesh_node_uuid[16];
u8  sigmesh_node_mac[6];
u8  certify_base_en =0;

void CScanDlg::OnConnect() 
{
	// TODO: Add your control notification handler code here
	int n = ((CListBox *) GetDlgItem (IDC_SCANLIST))->GetCurSel ();
	char buff[256];

    if(prov_mode_is_rp_mode()){
        u8 uuid[256];
        u16 unicast;
        ((CListBox *) GetDlgItem (IDC_SCANLIST))->GetText (n, buff);
		sscanf_s (  buff, "%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x", 
		            (u32 *)uuid, (u32 *)(uuid+1), (u32 *)(uuid+2), (u32 *)(uuid+3), 
					(u32 *)(uuid+4), (u32 *)(uuid+5), (u32 *)(uuid+6), (u32 *)(uuid+7),
					(u32 *)(uuid+8), (u32 *)(uuid+9), (u32 *)(uuid+10), (u32 *)(uuid+11),
					(u32 *)(uuid+12), (u32 *)(uuid+13), (u32 *)(uuid+14), (u32 *)(uuid+15));
		// use the uuid to find the unicast adr 
		unicast = remote_prov_client_find_adr_by_uuid(uuid);
		if(unicast == -1){
            AfxMessageBox("can not find the uuid of the unicast adr ");  
            return ;
		}
        // send link open and start provision part 
		if(ble_module_id_is_gateway()){
			gateway_send_link_open(unicast,uuid);
			LOG_MSG_INFO(TL_LOG_GATT_PROVISION,uuid,16,"gw rp link open\r\n");
			#if WIN32 
			memcpy(sigmesh_node_uuid,uuid,16);
			memcpy(sigmesh_node_mac,uuid+10,6);
			memcpy(gatt_provision_mag.device_uuid,sigmesh_node_uuid,16);
			#endif
			WaitForSingleObject (NULLEVENT2, 2000);
		}else{
			LOG_MSG_INFO(TL_LOG_GATT_PROVISION,uuid,16,"provision data is  \r\n");
			json_del_mesh_vc_node_info(uuid);
			#if WIN32 
			memcpy(sigmesh_node_uuid,uuid,16);
			memcpy(sigmesh_node_mac,uuid+10,6);
			memcpy(gatt_provision_mag.device_uuid,sigmesh_node_uuid,16);
			#endif
			WaitForSingleObject (NULLEVENT2, 3000);
			mesh_rp_start_settings(unicast,uuid,0);
		}
        OnOK();
        return ;
    }
	int d0, d1, d2, d3, d4, d5;
	if (n >= 0) {
			((CListBox *) GetDlgItem (IDC_SCANLIST))->GetText (n, buff);
			sscanf_s (buff, "%x %x %x %x %x %x", &d0, &d1, &d2, &d3, &d4, &d5);
			m_mac[1] = d0;
			m_mac[2] = d1;
			m_mac[3] = d2;
			m_mac[4] = d3;
			m_mac[5] = d4;
			m_mac[6] = d5;

			int uuid_idx;
			uuid_idx = find_idx_by_mac((u8 *)(m_mac+1));
			if(ble_module_id_is_gateway()){
				gateway_vc_set_adv_filter((unsigned char *)m_mac+1);
				// printf the uuid part 
				if(uuid_idx == -1){
					return ;
				}
				LOG_MSG_INFO(TL_LOG_GATT_PROVISION,gatt_mac_list.uuid[uuid_idx],16,"CScanDlg::provision link:the device uuid is \r\n");
				//memcpy the device uuid to the gatt_mac_list_part 
				memcpy(gatt_provision_mag.device_uuid,gatt_mac_list.uuid[uuid_idx],16);
				u16 uni = json_del_mesh_vc_node_info(gatt_provision_mag.device_uuid);
				// need to send the cmd with the unicast adr to notice to del the info 
				if(uni>0){
                    gateway_send_cmd_to_del_node_info(uni);
				}
			}else{
				report_adv_opera(0);
				online_st_write_handle = 0; // wait to find again.
				WaitForSingleObject (NULLEVENT2, 100);
				set_special_white_list((unsigned char *)m_mac+1);
				show_disconnect_box = connect_flag =1;
				memcpy(mac_str.mac_tmp,m_mac+1,6);				
				// printf the uuid part 
				if(uuid_idx == -1){
					OnOK ();
					LOG_MSG_INFO(TL_LOG_GATT_PROVISION,(unsigned char *)m_mac+1,6,"on connect mac adr is  \r\n");
					return ;
				}
				LOG_MSG_INFO(TL_LOG_GATT_PROVISION,gatt_mac_list.uuid[uuid_idx],16,"CScanDlg::OnConnect:the device uuid is \r\n");
				//memcpy the device uuid to the gatt_mac_list_part 
				memcpy(gatt_provision_mag.device_uuid,gatt_mac_list.uuid[uuid_idx],16);               
		       
			}
			memcpy(sigmesh_node_uuid,gatt_provision_mag.device_uuid,16);
		    memcpy(sigmesh_node_mac,gatt_mac_list.mac[uuid_idx],6);
			if(gatt_mac_list.oob[uuid_idx] &(BIT(OOB_PROV_RECORD)|BIT(OOB_CERT_BASE))){
				certify_base_en = 1;
			}else{
				certify_base_en = 0;
			}	
			OnOK ();
	}
	else
	{
		OnCancel ();
	}
	
}


void CScanDlg::AddMac(unsigned char * p_mac, int rssi) 
{
	p_mac[6] = 0;
	int i=0;
	for ( i=0; i<nmac; i++) {
		if (strcmp (amac+i*8, (char *) p_mac) == 0) {
			return;
		}
	}
	nmac++;
	memcpy (amac+i*8, p_mac, 6);
	amac[i*8+6] = 0;
	CString str;
	str.Format ("%02x %02x %02x %02x %02x %02x %d dBm", 
		p_mac[0], p_mac[1], p_mac[2], p_mac[3], p_mac[4], p_mac[5], rssi);
	((CListBox *) GetDlgItem (IDC_SCANLIST))->AddString (str);
}
#define SIG_MESH_PROVISION_SRV_VAL			0x1827
#define SIG_MESH_PROXY_SRV_VAL				0x1828


gatt_mac_list_str gatt_mac_list;
int find_idx_by_mac(u8 *p_mac)
{
	int i=0;
	for(i=0;i<MAX_MAC_LIST_NUM;i++){
		if(!memcmp(gatt_mac_list.mac[i],p_mac,6)){
			return i;
		}
	}
	return -1;
}
unsigned short serv_uuid_dat;


mesh_net_key_t * vc_get_netkey()
{
	foreach(i,NET_KEY_MAX){
		mesh_net_key_t *p_netkey_base = &mesh_key.net_key[i][0];
		if(p_netkey_base->valid){
			return p_netkey_base;
		}
	}
	return 0;
}

int check_hash_right_or_not(u8 identity,u8* phash,u8 *p_random,mesh_net_key_t * p_netkey,u16 ele_adr)
{
	u8 hash[8];
	u8 *p_key = p_netkey->idk;
	if(identity == NODE_IDENTITY_TYPE){
		proxy_adv_calc_with_node_identity(p_random,p_key,ele_adr,hash);
	}else if (identity == PRIVATE_NETWORK_ID_TYPE){
		proxy_adv_calc_with_private_net_id(p_random,p_netkey->nw_id,p_key,hash);
	}else if(identity == PRIVATE_NODE_IDENTITY_TYPE){
		proxy_adv_calc_with_private_node_identity(p_random,p_key,ele_adr,hash);
	}else{
		return 0;
	}
	if(!memcmp(hash,phash,8)){
		return 1;
	}else{
		return 0;
	}
}


void CScanDlg::AddDevice(unsigned char * p, int n) 
{
    if(prov_mode_is_rp_mode()){
        remote_prov_scan_report_win32 *p_repwin32 =(remote_prov_scan_report_win32 *)p;
        remote_prov_scan_report *p_report = (remote_prov_scan_report *)&(p_repwin32->scan_report);
		int idx = remote_prov_proc_client_scan_report(p_repwin32);
        if(-1 != idx){
            // need to add the infomation to the scan dlg part 
            u8 *p_uuid = p_report->uuid;
            CString str;
			str.Format ("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %d dBm ",
			p_uuid[0], p_uuid[1], p_uuid[2], p_uuid[3], p_uuid[4], p_uuid[5], p_uuid[6],p_uuid[7],
			p_uuid[8], p_uuid[9], p_uuid[10], p_uuid[11], p_uuid[12], p_uuid[13], p_uuid[14], p_uuid[15],p_report->rssi);
			((CListBox*)GetDlgItem(IDC_SCANLIST))->DeleteString(idx);
			((CListBox*)GetDlgItem(IDC_SCANLIST))->InsertString(idx, str);
         }
    }
	else if(ble_module_id_is_gateway()){
		unsigned char *p_mac;
		if(p[1]== HCI_GATEWAY_CMD_UPDATE_MAC){
			p_mac =p+2;
			int j=0;
			for (j=0; j<nmac; j++) {
				if (memcmp (amac+j*8, (char *) p_mac,6) == 0) {
				return;
				}
			}
			LOG_MSG_INFO(TL_LOG_GATEWAY,p, n,"HCI_GATEWAY_CMD_UPDATE_MAC \r\n");
			nmac++;
			memcpy (amac+j*8, p_mac, 6);
			u8 len =0;
			len = p[8];
			signed char rssi = p[8+len+1];
			signed short dc = p[8+len+2]|(p[8+len+3] << 8);
			CString str;
			str.Format ("%02x %02x %02x %02x %02x %02x %d dBm %d K ", 
			p_mac[0], p_mac[1], p_mac[2], p_mac[3], p_mac[4], p_mac[5], rssi, dc);
			LOG_MSG_INFO (TL_LOG_COMMON, p+8, len+1, "the unprovision beacon is ", 0);
			((CListBox *) GetDlgItem (IDC_SCANLIST))->AddString (str);
			// should add the uuid and the mac into the list too 
			beacon_data_pk *p_beacon = (beacon_data_pk *)(p+8);
			if (gatt_mac_list.max_idx >= MAX_MAC_LIST_NUM) {
				memset((u8 *)&gatt_mac_list,0,sizeof(gatt_mac_list));	
			}
		    memcpy(gatt_mac_list.mac[gatt_mac_list.max_idx],p_mac,6);
		    memcpy(gatt_mac_list.uuid[gatt_mac_list.max_idx],p_beacon->device_uuid,16);
		    gatt_mac_list.max_idx++;
		}else{
			return ;
		}
	
	}else{
		if(n<29){
			return; 
		}
		unsigned char p_mac[8];
		//unsigned char test[40];
		unsigned char dst_mac[6]={0x15,0x00,0xff,0xff,0xff,0xff};
		u8 adv_data_len = *(p+13);
		u8 *p_adv_len = (p+13);
		signed char rssi = p_adv_len[adv_data_len+1];
		signed short dc = p_adv_len[adv_data_len+2] | (p_adv_len[adv_data_len+3] << 8);
	
		char scan_req[] = "scan request";
		char connect_req[] = "connect request";
		char *ps;
		u16 service_uuid=0;
		service_uuid = (p[20]<<8)|p[19];
		if(service_uuid != SIG_MESH_PROVISION_SRV_VAL && SIG_MESH_PROXY_SRV_VAL != service_uuid)
		{
			return ;
		}
	
		if (p[0] != 0x04 || p[1] != 0x3e || p[3] != 0x02)
		{
			return;
		}
		memcpy (p_mac, p + 7, 6);
		p_mac[6] = 0;
		int j=0;
		for (j=0; j<nmac; j++) {
			if (memcmp (amac+j*8, (char *) p_mac, 6) == 0) {
				return;
			}
		}
		#if TEST_ENABLE
		if(!memcmp(dst_mac,p_mac,6)){
			memcpy(test,p,30);;

		}
		#endif
		nmac++;
		memcpy (amac+j*8, p_mac, 6);
		amac[j*8+6] = 0;

		//-------------- find device name -------------------------
		int i;
		for (i = 14; i < n; )
		{
			if (p[i + 1] == 9)
			{
				p[i + p[i] + 1] = 0;
				break;
			}
			i += p[i] + 1;
			if (!p[i])
			{
				i = n;
			}
		}

		if (i >= n)
		{
			p[i + 2] = 0;
		}

		if (p[5] == 0x03)
			ps = scan_req;
		else if (p[5] == 0x05)
			ps = connect_req;
		else
			ps = (char *)p + i + 2;
		//-------------- device name ------------------------------
		CString str;
		str.Format ("%02x %02x %02x %02x %02x %02x %d dBm %d K (%s)", 
			p_mac[0], p_mac[1], p_mac[2], p_mac[3], p_mac[4], p_mac[5], rssi, dc, ps);
		//unsigned char mac_idx;
		// set the mac and the uuid part 
		if(service_uuid == SIG_MESH_PROVISION_SRV_VAL){
			if (gatt_mac_list.max_idx >= MAX_MAC_LIST_NUM) {
				memset((u8 *)&gatt_mac_list,0,sizeof(gatt_mac_list));	
			}
			memcpy(gatt_mac_list.mac[gatt_mac_list.max_idx],p_mac,6);
			memcpy(gatt_mac_list.uuid[gatt_mac_list.max_idx],p + 13 +12 ,16);
			memcpy((u8*)(gatt_mac_list.oob + gatt_mac_list.max_idx),p + 13 +28 ,2);// need to switch to oob info part 
			gatt_mac_list.max_idx++;
			((CListBox *) GetDlgItem (IDC_SCANLIST))->AddString (str);
		}else if(service_uuid == SIG_MESH_PROXY_SRV_VAL){
			if (gatt_mac_list.max_idx >= MAX_MAC_LIST_NUM) {
				memset((u8 *)&gatt_mac_list,0,sizeof(gatt_mac_list));	
			}
			// if the provisioner has not provisioned itself ,no device add the tag  provisioned 
			if(pro_self_flag){
				// if the proxy adv is network id part 
				proxy_adv_node_identity * p_proxy ;
				p_proxy = (proxy_adv_node_identity *)(p+7+6+1);
				//if the proxy adv is node identity packet,need to check whether is belongs to the network.				
				if(p_proxy->identify_type == NETWORK_ID_TYPE){
					proxy_adv_net_id *p_net_adv = (proxy_adv_net_id *)(p+7+6+1);
					mesh_net_key_t *p_netkey_base = vc_get_netkey();
					if(!memcmp(p_netkey_base->nw_id,p_net_adv->net_id,sizeof(p_net_adv->net_id))){
						str+=_T("provisioned");
						((CListBox *) GetDlgItem (IDC_SCANLIST))->AddString (str);
					}
				}else{
					mesh_net_key_t *p_netkey = vc_get_netkey();
					foreach(i,MESH_NODE_MAX_NUM){
						VC_node_info_t *p_node_info = &VC_node_info[i];
						if(check_hash_right_or_not(p_proxy->identify_type,p_proxy->hash,p_proxy->random,p_netkey,p_node_info->node_adr)){
							memcpy(gatt_mac_list.mac[gatt_mac_list.max_idx],p_mac,6);
							memcpy(gatt_mac_list.uuid[gatt_mac_list.max_idx],json_get_uuid(p_node_info->node_adr) ,16);
							gatt_mac_list.max_idx++;
							str+=_T("provisioned");
							((CListBox *) GetDlgItem (IDC_SCANLIST))->AddString (str);
							break;
						}
					}	
				}
			}
		  }
	
	}

	int count = ((CListBox *) GetDlgItem (IDC_SCANLIST))->GetCount();
	CString str;
	str.Format(_T("%d Device Scaned"), count);
	SetWindowText(str);
}

void CScanDlg::OnDblclkScanlist() 
{
	// TODO: Add your control notification handler code here
	OnConnect ();
}


void CScanDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialog::OnClose();
}


void CScanDlg::OnDestroy()
{
	CTl_ble_moduleDlg *parentDlg =  (CTl_ble_moduleDlg *)GetParent();

	ShowWindow(SW_HIDE);
	//CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}

void CScanDlg::OnDestroyReally()
{
	CDialog::OnDestroy();
}

void CScanDlg::OnCancel()
{
	ShowWindow(SW_HIDE);
	//CDialog::OnCancel();
}

void CScanDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	if (!prov_mode_is_rp_mode()) {
		fifo_push_vc_cmd2dongle_usb((u8*)m_mac, 7);
	}
	WaitForSingleObject (NULLEVENT2, 10);
	m_status = 1;

	ShowWindow(SW_HIDE);
	//CDialog::OnOK();
}



void CScanDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: 在此处添加消息处理程序代码
	((CListBox *) GetDlgItem (IDC_SCANLIST))->ResetContent();
	memset(&gatt_mac_list, 0, sizeof(gatt_mac_list));
	nmac = 0;

	memset(amac, 0, sizeof(amac));
	memset(m_mac, 0, sizeof(m_mac));

	RECT parentRect;
	GetParent()->GetWindowRect(&parentRect);
	int parentWidth = parentRect.right - parentRect.left;
	int parentHeight = parentRect.bottom - parentRect.top;

	RECT rect;
	GetWindowRect(&rect);
	int thisWidth = rect.right - rect.left;
	int thisHeight = rect.bottom - rect.top;

	int left = (parentWidth - thisWidth)/2;
	int top = (parentHeight - thisHeight)/2;

	MoveWindow(parentRect.left + left, parentRect.top + top, thisWidth, thisHeight, TRUE);
}
