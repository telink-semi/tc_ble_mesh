/********************************************************************************************************
 * @file    app_l2cap_coc.c
 *
 * @brief   This is the source file for BLE SDK
 *
 * @author  BLE GROUP
 * @date    06,2022
 *
 * @par     Copyright (c) 2022, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
#include "tl_common.h"
#if MESH_CDTP_ENABLE
#include "drivers.h"
#include "stack/ble/ble.h"

#include "app.h"
#include "mesh_cdtp.h"
#include "stack/ble/host/signaling/signaling.h"
#include "proj_lib/sig_mesh/app_mesh.h"
#include "app_l2cap_coc.h"

typedef struct{
	u16 connHandle;
	u16 mtu;
	u16 srcCid;
	u16 dstCid;
} app_cocCid_t;

app_cocCid_t app_cocCid[COC_CID_COUNT];

static u8 cocBuffer[COC_MODULE_BUFFER_SIZE(CREATE_COC_CONNECT_ACL_COUNT, COC_CID_COUNT, COC_MTU_SIZE)];

void app_l2cap_coc_init(void)
{
	blc_coc_initParam_t regParam = {
		.MTU = COC_MTU_SIZE,
		.SPSM = 0x0080,
		.createConnCnt = 1,
		.cocCidCnt = COC_CID_COUNT,
	};
	/*int state = */blc_l2cap_registerCocModule(&regParam, cocBuffer, sizeof(cocBuffer));
}

/**
 * @brief      BLE host event handler call-back.
 * @param[in]  h       event type
 * @param[in]  para    Pointer point to event parameter buffer.
 * @param[in]  n       the length of event parameter.
 * @return
 */
int app_host_coc_event_callback (u32 h, u8 *para, int n)
{
	u8 event = h & 0xFF;

	switch(event)
	{
		case GAP_EVT_L2CAP_COC_CONNECT:
		{
			gap_l2cap_cocConnectEvt_t* cocConnEvt = (gap_l2cap_cocConnectEvt_t*)para;
			for(int i=0; i<ARRAY_SIZE(app_cocCid); i++)
			{
				if(!app_cocCid[i].connHandle)
				{
					app_cocCid[i].connHandle = cocConnEvt->connHandle;
					app_cocCid[i].mtu = cocConnEvt->mtu;
					app_cocCid[i].srcCid = gAppsCocSendScid = cocConnEvt->srcCid;
					app_cocCid[i].dstCid = cocConnEvt->dstCid;
					gAppsCocIsReady = true;
					break;
				}
			}
		}
		break;

		case GAP_EVT_L2CAP_COC_DISCONNECT:
		{
			gap_l2cap_cocDisconnectEvt_t* cocDisconnEvt = (gap_l2cap_cocDisconnectEvt_t*)para;
			for(int i=0; i<ARRAY_SIZE(app_cocCid); i++)
			{
				if(app_cocCid[i].connHandle == cocDisconnEvt->connHandle &&
						app_cocCid[i].srcCid == cocDisconnEvt->srcCid &&
						app_cocCid[i].dstCid == cocDisconnEvt->dstCid
				)
				{
					app_cocCid[i].connHandle = 0;
					app_cocCid[i].srcCid = gAppsCocSendScid = 0;
					app_cocCid[i].dstCid = 0;
					gAppsCocIsReady = true;
					break;
				}
			}
		}
		break;

		case GAP_EVT_L2CAP_COC_RECONFIGURE:
		{
			//gap_l2cap_cocReconfigureEvt_t* cocRecfgEvt = (gap_l2cap_cocReconfigureEvt_t*)para;
		}
		break;

		case GAP_EVT_L2CAP_COC_RECV_DATA:
		{
			gap_l2cap_cocRecvDataEvt_t* p_ev = (gap_l2cap_cocRecvDataEvt_t*)para;
			app_l2cap_coc_datacb(p_ev->connHandle, 0, p_ev->dstCid, p_ev->length, p_ev->data); // TODO: scid
		}
		break;

		case GAP_EVT_L2CAP_COC_SEND_DATA_FINISH:
		{
			//gap_l2cap_cocSendDataFinishEvt_t* cocSendDataFinishEvt = (gap_l2cap_cocSendDataFinishEvt_t*)para;
		}
		break;

		case GAP_EVT_L2CAP_COC_CREATE_CONNECT_FINISH:
		{
			//gap_l2cap_cocCreateConnectFinishEvt_t* cocCreateConnFinishEvt = (gap_l2cap_cocCreateConnectFinishEvt_t*)para;
		}
		break;

		default:
		break;
	}

	return 0;
}

int hci_slave_handle_valid(u16 handle) {
	if (!(handle & BLS_CONN_HANDLE)) {
		return 0;
	}
	return 1;
}

#endif
