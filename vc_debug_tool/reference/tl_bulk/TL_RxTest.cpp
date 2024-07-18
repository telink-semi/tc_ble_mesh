/********************************************************************************************************
 * @file	TL_RxTest.cpp
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
#include "TL_RxTest.h"
#include "afxdialogex.h"

#include "tl_ble_moduleDlg.h"

// CTL_RxTest �Ի���
mesh_rcv_t mesh_rx_test_result[MESH_NODE_MAX_NUM];
int mesh_rx_test_result_cnt;

IMPLEMENT_DYNAMIC(CTL_RxTest, CDialog)

CTL_RxTest::CTL_RxTest(CWnd* pParent /*=NULL*/)
	: CDialog(CTL_RxTest::IDD, pParent)
	, m_cur_cnt(0)
	, m_interval(500)
	, m_total_counts(100)
	, m_transmit(_T("0x15"))
	, m_RcvCnts(0)
	, m_testing(false)
	, m_GetRevCnts(0)
	, m_total_nodes(50)
	, m_sel_mode(2)
	, m_max_time(0)
	, m_min_time(0)
	, m_average_time(0)
	, m_pkts_send(8)
	, m_pkts_receive(8)
	, m_csDstAddr(_T("0xffff"))
	, m_csAddrGet(_T("0xffff"))
	, m_clear_result(0)
	, m_rsp_cnt(0)
{
}

int mesh_rx_test_update(u16 addr, mesh_rcv_t *p_rcv)
{
	for(int i = 0; i < mesh_rx_test_result_cnt; i++)
	{
		if (addr == mesh_rx_test_result[i].src_addr) {
			return 0;
		}
	}

	if (mesh_rx_test_result_cnt < MESH_NODE_MAX_NUM) {
		memcpy(&mesh_rx_test_result[mesh_rx_test_result_cnt], p_rcv, sizeof(mesh_rcv_t));
		mesh_rx_test_result[mesh_rx_test_result_cnt].src_addr = addr;	
		mesh_rx_test_result_cnt++;
	}

	return 1;
}

void CTL_RxTest::StatusNotify (unsigned char *p, int len)
{
	mesh_rc_rsp_t rsp;
	rsp.len = len;
	memcpy(&rsp.src, p, len);

	u16 op = rf_link_get_op_by_ac(rsp.data);
	u32 size_op = SIZE_OF_OP(op);

	if ((G_ONOFF_STATUS == op) || (G_LEVEL_STATUS == op)){
		u8 *par = rsp.data + size_op;
		u16 par_len = rsp.len - 4 - size_op;
		if(G_ONOFF_STATUS == op){			
		}else if(G_LEVEL_STATUS == op){
			mesh_rcv_t* p_rcv = (mesh_rcv_t*)par;
			u16 dst_addr = (u16)strtol(m_csAddrGet, NULL, 16);
			if (m_clear_result) {
				m_clear_result = 0;
				GetDlgItem(IDC_EDIT_CLEAR)->SetWindowText("clear success");
			}
			else if (is_unicast_adr(dst_addr)) {
				if (rsp.src == dst_addr) {
					m_RcvCnts = p_rcv->rcv_cnt;
					m_max_time = p_rcv->max_time;
					m_min_time = p_rcv->min_time;
					m_average_time = p_rcv->avr_time;
					m_rsp_cnt = 1;

					float percent = (float)m_RcvCnts * 100 / m_total_counts;
					CString csRcvCnt;
					csRcvCnt.Format(_T("%d(%0.2f%%)"), m_RcvCnts, percent);
					GetDlgItem(IDC_RESULT)->SetWindowText(csRcvCnt);
					csRcvCnt.Format(_T("%d"), m_max_time);
					GetDlgItem(IDC_MAX)->SetWindowText(csRcvCnt);
					csRcvCnt.Format(_T("%d"), m_min_time);
					GetDlgItem(IDC_MIN)->SetWindowText(csRcvCnt);
					csRcvCnt.Format(_T("%d"), m_average_time);
					GetDlgItem(IDC_AVERAGE)->SetWindowText(csRcvCnt);
					csRcvCnt.Format(_T("%d"), m_rsp_cnt);
					GetDlgItem(IDC_RSP_CNT)->SetWindowText(csRcvCnt);
				}
			}
			else{
				mesh_rx_test_update(rsp.src, p_rcv);
				if (mesh_rx_test_result_cnt) {
					m_RcvCnts = mesh_rx_test_result[0].rcv_cnt;
					m_max_time = mesh_rx_test_result[0].max_time;
					m_min_time = mesh_rx_test_result[0].min_time;
					m_average_time = mesh_rx_test_result[0].avr_time;
					m_rsp_cnt = mesh_rx_test_result_cnt;

					for (int i = 1; i < mesh_rx_test_result_cnt; i++)
					{
						if (m_min_time > mesh_rx_test_result[i].min_time) {
							m_min_time = mesh_rx_test_result[i].min_time;
						}

						if (m_max_time < mesh_rx_test_result[i].max_time) {
							m_max_time = mesh_rx_test_result[i].max_time;
						}

						m_average_time += mesh_rx_test_result[i].avr_time;
						m_RcvCnts += mesh_rx_test_result[i].rcv_cnt;
					}

					m_average_time = m_average_time / mesh_rx_test_result_cnt;
					m_RcvCnts = m_RcvCnts / mesh_rx_test_result_cnt;

					float percent = (float)m_RcvCnts * 100 / m_total_counts;
					CString csRcvCnt;
					csRcvCnt.Format(_T("%d(%0.2f%%)"), m_RcvCnts, percent);
					GetDlgItem(IDC_RESULT)->SetWindowText(csRcvCnt);
					csRcvCnt.Format(_T("%d"), m_max_time);
					GetDlgItem(IDC_MAX)->SetWindowText(csRcvCnt);
					csRcvCnt.Format(_T("%d"), m_min_time);
					GetDlgItem(IDC_MIN)->SetWindowText(csRcvCnt);
					csRcvCnt.Format(_T("%d"), m_average_time);
					GetDlgItem(IDC_AVERAGE)->SetWindowText(csRcvCnt);
					csRcvCnt.Format(_T("%d"), m_rsp_cnt);
					GetDlgItem(IDC_RSP_CNT)->SetWindowText(csRcvCnt);
				}
			}		
		}
	}else if (CFG_SIG_MODEL_SUB_LIST == op){
	
	}else if (CFG_MODEL_SUB_STATUS == op){
		
	}else if (CFG_MODEL_PUB_STATUS == op){
		
	}else if (CFG_BEACON_STATUS == op){		

	}else if (CFG_DEFAULT_TTL_STATUS == op){
		
	}else if (CFG_NW_TRANSMIT_STATUS == op){
		m_transmit.Format(_T("0x%02x"), rsp.data[2]);
		GetDlgItem(IDC_TRANSMIT_SET)->SetWindowText(m_transmit);		
	}else if (CFG_RELAY_STATUS == op){
	
	}else if (CFG_FRIEND_STATUS == op){
		
	}else if (CFG_GATT_PROXY_STATUS == op){
	
	}
}

CTL_RxTest::~CTL_RxTest()
{
}

BOOL CTL_RxTest::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if (WM_KEYDOWN ==pMsg->message && VK_RETURN == pMsg->wParam)
	{
		if (GetFocus() == GetDlgItem(IDC_TRANSMIT_SET)) //���ݲ�ͬ�ؼ������ж����Ǹ���ִ�� 
		{
			UpdateData(TRUE);
			u8 transmit = (u8)strtol(m_transmit,NULL,16);
			mesh_transmit_t *p_tran = (mesh_transmit_t *)&transmit;
			u16 dst_addr = (u16)strtol(m_csDstAddr,NULL,16);
			cfg_cmd_nw_transmit_set(dst_addr, p_tran->count+1, p_tran->invl_steps+1);
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CTL_RxTest::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_INTERVAL, m_interval);
	DDX_Text(pDX, IDC_TOTAL_COUNTS, m_total_counts);
	//	DDV_MinMaxInt(pDX, m_total_counts, 0, 200);
	DDX_Text(pDX, IDC_DST_ADDR, m_csDstAddr);
	DDX_Text(pDX, IDC_TRANSMIT_SET, m_transmit);
	DDX_Text(pDX, IDC_RESULT, m_RcvCnts);
	DDX_Text(pDX, IDC_NODES, m_total_nodes);
	DDV_MinMaxInt(pDX, m_total_nodes, 0, 512);
	DDX_Radio(pDX, IDC_UNICAST, m_sel_mode);
	DDX_Text(pDX, IDC_MAX, m_max_time);
	DDX_Text(pDX, IDC_MIN, m_min_time);
	DDX_Text(pDX, IDC_AVERAGE, m_average_time);
	DDX_Text(pDX, IDC_PKT_NUMS, m_pkts_send);
	DDV_MinMaxByte(pDX, m_pkts_send, 8, 223);
	DDX_Text(pDX, IDC_PKT_RECEIVE, m_pkts_receive);
	DDV_MinMaxByte(pDX, m_pkts_receive, 8, 223);
	DDX_Text(pDX, IDC_ADDR_GET, m_csAddrGet);
	DDX_Text(pDX, IDC_RSP_CNT, m_rsp_cnt);
}


BEGIN_MESSAGE_MAP(CTL_RxTest, CDialog)
	ON_BN_CLICKED(IDC_START, &CTL_RxTest::OnBnClickedStart)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_TRANSMIT, &CTL_RxTest::OnBnClickedTransmit)
	ON_BN_CLICKED(IDC_GET_RESULT, &CTL_RxTest::OnBnClickedGetResult)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CTL_RxTest::OnBnClickedButtonClear)
END_MESSAGE_MAP()


// CTL_RxTest ��Ϣ�������


void CTL_RxTest::OnBnClickedStart()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	
	if (m_testing)//stop test
	{
		m_testing = FALSE;
		KillTimer(1);
		GetDlgItem(IDC_START)->SetWindowTextA("start");
		GetDlgItem(IDC_GET_RESULT)->EnableWindow(TRUE);		
	}
	else
	{//start test
		u16 dst_addr = 0xffff;//(u16)strtol(m_csDstAddr,NULL,16);
		access_cmd_set_level(dst_addr, 0, 0x1ff, 0, 0);		
		m_cur_cnt = 0; 
		m_GetRevCnts = 0;
		m_testing = TRUE;
		mesh_rx_test_result_cnt = 0;
		SetTimer(1, m_interval, NULL);
		GetDlgItem(IDC_GET_RESULT)->EnableWindow(FALSE);		
	}
}

void CTL_RxTest::OnOK() 
{
	// TODO: Add extra validation here
	//CDialog::OnOK();
	UpdateData (TRUE);
}

void CTL_RxTest::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	switch (nIDEvent)
	{
	case 1:
		if(m_GetRevCnts * m_interval < 2000) 
		{
			m_GetRevCnts++;
			return;
		}
		if (m_cur_cnt<m_total_counts)
		{
			CString cur_cnt;
			cur_cnt.Format(_T("stop(%d)"), m_cur_cnt);
			CWnd *p_start = GetDlgItem(IDC_START);
			p_start->SetWindowTextA(cur_cnt);
			u16 dst_addr = (u16)strtol(m_csDstAddr,NULL,16);
			u8 ack = 1;
			u8 rsp_max = 1;
			if(((CButton *)GetDlgItem(IDC_UNICAST))->GetCheck()){
				ack = 1;
			}
			else if (((CButton *)GetDlgItem(IDC_UNICAST_NO_ACK))->GetCheck()) {
				ack = 0;
			}
			else if(((CButton *)GetDlgItem(IDC_BROCAST_NOACK))->GetCheck()){
				dst_addr = 0xffff;
				ack = 0;
			}
			else if(((CButton *)GetDlgItem(IDC_BROCAST_ACK))->GetCheck()){
				dst_addr = 0xffff;
				ack = 1;
				rsp_max = m_total_nodes;
			}
			unsigned char cmd_buf[sizeof(mesh_bulk_cmd_par_t) + HCI_CMD_LEN];

			if(ble_module_id_is_gateway()){
				cmd_buf[0]= HCI_GATEWAY_CMD_MESH_RX_TEST & 0xFF;
				cmd_buf[1]= (HCI_GATEWAY_CMD_MESH_RX_TEST >> 8) & 0xFF;
			}else{
				cmd_buf[0]= HCI_CMD_BULK_CMD2DEBUG & 0xFF;
				cmd_buf[1]= (HCI_CMD_BULK_CMD2DEBUG >> 8) & 0xFF;
			}
			cmd_buf[2] = (u8)dst_addr;
			cmd_buf[3] = dst_addr>>8;
			cmd_buf[4] = rsp_max;
			cmd_buf[5] = ack;
			cmd_buf[6] = m_cur_cnt;
			cmd_buf[7] = m_cur_cnt >> 8;
			cmd_buf[8] = m_pkts_send;
			cmd_buf[9] = m_pkts_receive;
			u16 filter_address = (u16)strtol(m_csDstAddr,NULL,16);
			cmd_buf[10] = (u8)filter_address;
			cmd_buf[11] = filter_address>>8;

			if (ble_module_id_is_gateway()) {
				void gateway_VC_send_cmd(u8 cmd, u8 * data, int len);
				gateway_VC_send_cmd(HCI_GATEWAY_CMD_MESH_RX_TEST, cmd_buf, 11);
			}
			else {
				void write_no_rsps_pkts(u8 * p, u16 len, u16 handle, u8 msg_type);
				write_no_rsps_pkts(cmd_buf, 11, proxy_write_handle, MSG_RX_TEST_PDU);
			}
			m_cur_cnt++;
		} 
		else
		{
			m_cur_cnt = 0;
			m_testing = FALSE;
			KillTimer(1);
			GetDlgItem(IDC_START)->SetWindowTextA("start");
			GetDlgItem(IDC_GET_RESULT)->EnableWindow(TRUE);
		}
		break;
	}
	CDialog::OnTimer(nIDEvent);
}


void CTL_RxTest::OnBnClickedTransmit()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData();
	u16 dst_addr = (u16)strtol(m_csDstAddr,NULL,16);
	cfg_cmd_nw_transmit_get(dst_addr);
}


void CTL_RxTest::OnBnClickedGetResult()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	m_GetRevCnts = TRUE;
	u16 dst_addr = (u16)strtol(m_csAddrGet,NULL,16);
	access_cmd_get_level(dst_addr, 1);
}


void CTL_RxTest::OnBnClickedButtonClear()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	u16 dst_addr = (u16)strtol(m_csAddrGet,NULL,16);;//(u16)strtol(m_csDstAddr,NULL,16);

	access_cmd_set_level(dst_addr, 1, 0x1ff, 1, 0);	
	GetDlgItem(IDC_EDIT_CLEAR)->SetWindowText("");
	m_clear_result = 1;
	mesh_rx_test_result_cnt = 0;
}
