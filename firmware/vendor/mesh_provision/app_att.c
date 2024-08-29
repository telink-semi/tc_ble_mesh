/********************************************************************************************************
 * @file	app_att.c
 *
 * @brief	for TLSR chips
 *
 * @author	telink
 * @date	Sep. 30, 2010
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
#include "tl_common.h"
#include "proj_lib/ble/ll/ll.h"
#include "proj_lib/ble/blt_config.h"
#include "proj_lib/ble/service/ble_ll_ota.h"
#include "../common/app_provison.h"
#include "../common/app_proxy.h"
#include "proj_lib/sig_mesh/app_mesh.h"
#include "../common/app_beacon.h"

#if(1)

typedef struct
{
  /** Minimum value for the connection event (interval. 0x0006 - 0x0C80 * 1.25 ms) */
  u16 intervalMin;
  /** Maximum value for the connection event (interval. 0x0006 - 0x0C80 * 1.25 ms) */
  u16 intervalMax;
  /** Number of LL latency connection events (0x0000 - 0x03e8) */
  u16 latency;
  /** Connection Timeout (0x000A - 0x0C80 * 10 ms) */
  u16 timeout;
} gap_periConnectParams_t;

const u16 clientCharacterCfgUUID = GATT_UUID_CLIENT_CHAR_CFG;

const u16 extReportRefUUID = GATT_UUID_EXT_REPORT_REF;

const u16 reportRefUUID = GATT_UUID_REPORT_REF;

const u16 characterPresentFormatUUID = GATT_UUID_CHAR_PRESENT_FORMAT;

const u16 my_primaryServiceUUID = GATT_UUID_PRIMARY_SERVICE;

static const u16 my_characterUUID = GATT_UUID_CHARACTER;

const u16 my_devServiceUUID = SERVICE_UUID_DEVICE_INFORMATION;

const u16 my_PnPUUID = CHARACTERISTIC_UUID_PNP_ID;

const u16 my_devNameUUID = GATT_UUID_DEVICE_NAME;

//device information
const u16 my_gapServiceUUID = SERVICE_UUID_GENERIC_ACCESS;
// Device Name Characteristic Properties
static u8 my_devNameCharacter = CHAR_PROP_READ | CHAR_PROP_NOTIFY;
// Appearance Characteristic Properties
const u16 my_appearanceUIID = 0x2a01;
const u16 my_periConnParamUUID = 0x2a04;
static u8 my_appearanceCharacter = CHAR_PROP_READ;
// Peripheral Preferred Connection Parameters Characteristic Properties
static u8 my_periConnParamChar = CHAR_PROP_READ;
u16 my_appearance = GAP_APPEARE_UNKNOWN;
gap_periConnectParams_t my_periConnParameters = {20, 40, 0, 1000};


extern u8  ble_devName[];

// Device Name Characteristic Properties
static u8 my_PnPCharacter = CHAR_PROP_READ;


const u8	my_PnPtrs [] = {0x02, 0x8a, 0x24, 0x66, 0x82, 0x01, 0x00};

u8 my_fwRevisionCharacter = CHAR_PROP_READ;

//////////////////////// Battery /////////////////////////////////////////////////
const u16 my_batServiceUUID       			= SERVICE_UUID_BATTERY;
//static u8 my_batProp 						= CHAR_PROP_READ | CHAR_PROP_NOTIFY;
const u16 my_batCharUUID       				= CHARACTERISTIC_UUID_BATTERY_LEVEL;
u8 		  my_batVal[1] 						= {99};

/////////////////////////////////////////////////////////
const u16 userdesc_UUID		= GATT_UUID_CHAR_USER_DESC;

/////////////////////////////////////////spp/////////////////////////////////////
#if (TELIK_SPP_SERVICE_ENABLE)
u8 TelinkSppServiceUUID[16]	      = TELINK_SPP_UUID_SERVICE;
u8 TelinkSppDataServer2ClientUUID[16]    = TELINK_SPP_DATA_SERVER2CLIENT;
u8 TelinkSppDataClient2ServerUUID[16]    = TELINK_SPP_DATA_CLIENT2SERVER;

// Spp data from Server to Client characteristic variables
static u8 SppDataServer2ClientProp = CHAR_PROP_READ | CHAR_PROP_NOTIFY;
u8  SppDataServer2ClientData[ATT_MTU_SIZE - 3];
static u8 SppDataServer2ClientDataCCC[2] = {0};

// Spp data from Client to Server characteristic variables
//CHAR_PROP_WRITE: Need response from slave, low transmission speed
static u8 SppDataClient2ServerProp = CHAR_PROP_READ | CHAR_PROP_WRITE_WITHOUT_RSP; //CHAR_PROP_WRITE;
u8  SppDataClient2ServerData[ATT_MTU_SIZE - 3];

//SPP data descriptor
const u8 TelinkSPPS2CDescriptor[] = "Telink SPP: Module->Phone";
const u8 TelinkSPPC2SDescriptor[] = "Telink SPP: Phone->Module";

u32 	spp_err = 0;
void	spp_test_read (u8 *p, int n)
{
	static u32 spp_err_st;
	static u32 spp_read = 0;
	u32 seq;
	memcpy (&seq, p, 4);
	if (spp_read != seq)
	{
		spp_err++;
	}
	else
	{
		for (int i=4; i<n; i++)
		{
			if ((u8)(p[0] + i) != p[i])
			{
				spp_err++;
				spp_err_st += BIT(16);
				break;
			}
		}
	}
	spp_read = seq - 1;

	if (0 && spp_err > 1)
	{
#if UART_8266_EN
		gpio_set_input_en(GPIO_URX, 0);
#else
		gpio_set_input_en(GPIO_URXB3, 0);
#endif
		irq_disable ();
		while (1);
	}
}

void module_onReceiveData(rf_packet_att_write_t *p)
{
	u32 n;
	u8 len = p->l2capLen - 3;
	if(len > 0)
	{
#if 0
		static u32 sn = 0;
		memcpy (&n, &p->value, 4);
		if (sn != n)
		{
			sn = 0;
			bls_ll_terminateConnection (0x13);
		}
		else
		{
			sn = n + 1;
		}
#endif
		u32 header;
		header = 0x07a0;		//data received event
		header |= (3 << 16) | (1<<24);
		spp_test_read (&p->value, len);
		blc_hci_send_data(header, &p->opcode, len + 3);		//HCI_FLAG_EVENT_TLK_MODULE
	}
}
#endif
// Include attribute (Battery service)
//static u16 include[3] = {0x0026, 0x0028, SERVICE_UUID_BATTERY};
//const u8 ATT_PERMISSIONS_READ_AUTHOR
u8 att_perm_auth_read = ATT_PERMISSIONS_READ_AUTHOR;
u8 att_perm_auth_write = ATT_PERMISSIONS_WRITE_AUTHOR;
u8 att_perm_auth_rdwd  = ATT_PERMISSIONS_RDWD_AUTHOR;


#define MESH_PROVISON_DATA	{0xce,0x7f}
#define MESH_PROXY_DATA		{0xcf,0x7f}

const u8 my_OtaServiceUUID[16]		= TELINK_OTA_UUID_SERVICE;
const u8 my_OtaUUID[16]		= TELINK_SPP_DATA_OTA;

static u8 my_OtaProp		= CHAR_PROP_READ | CHAR_PROP_WRITE_WITHOUT_RSP | CHAR_PROP_NOTIFY;
const u8  my_OtaName[] = {'O', 'T', 'A'};
u8	 	my_OtaData 		= 0x00;
// pb-gatt 
u8 my_pb_gattUUID[2]=SIG_MESH_PROVISION_SERVICE;

const u8 my_pb_gatt_out_UUID[2]= SIG_MESH_PROVSIION_DATA_OUT;
//static u8 my_pb_gatt_out_prop = CHAR_PROP_NOTIFY;
static u8 my_pb_gatt_out_prop = CHAR_PROP_NOTIFY;

const u8 my_pb_gattOutName[]={'P','B','G','A','T','T','-','O','U','T'};
u8 	my_pb_gattOutData[2] =MESH_PROVISON_DATA;

const u8 my_pb_gatt_in_UUID[2]= SIG_MESH_PROVISION_DATA_IN;
static u8 my_pb_gatt_in_prop =  CHAR_PROP_WRITE_WITHOUT_RSP;
const u8 my_pb_gattInName[]={'P','B','G','A','T','T','-','I','N'};
u8 	my_pb_gattInData[2] =MESH_PROVISON_DATA;

u8 my_proxy_gattUUID[2]= SIG_MESH_PROXY_SERVICE;

const u8 my_proxy_out_UUID[2]= SIG_MESH_PROXY_DATA_OUT;
static u8 my_proxy_out_prop = CHAR_PROP_NOTIFY;
const u8 my_proxy_out_Name[]={'P','R','O','X','Y','-','O','U','T'};
u8 my_proxy_outData[2] =MESH_PROXY_DATA;

const u8 my_proxy_in_UUID[2]= SIG_MESH_PROXY_DATA_IN;
static u8 my_proxy_in_prop = CHAR_PROP_WRITE_WITHOUT_RSP;
const u8 my_proxy_in_Name[]={'P','R','O','X','Y','-','I','N'};
u8 my_proxy_inData[2] =MESH_PROXY_DATA;

#if USER_DEFINE_SET_CCC_ENABLE
const  u8 my_userdefine_service_UUID[16]= TELINK_USERDEFINE_GATT;
static u8 my_userdefine_prop		= CHAR_PROP_READ | CHAR_PROP_WRITE_WITHOUT_RSP|CHAR_PROP_NOTIFY|CHAR_PROP_INDICATE;
u8	 	  my_userdefine_dat 		= 0x00;
const u8  my_userderdefine[4] = {'U', 'S', 'E','R'};
const u8  my_userdefine_UUID[16]= TELINK_USERDEFINE_UUID;
#endif

const u16  mi_generic_service  = SERVICE_UUID_GENERIC_ATTRIBUTE;
const u16 mi_service_change_uuid = 0x2a05;
static u8 mi_service_change_prop = CHAR_PROP_INDICATE;
static u8 mi_service_change_buf[4];
const u8 mi_service_change_str[]="service change";
u8 mi_service_change_ccc[2]=	{0x00,0x00};
u8 mi_service_change_char_perm = ATT_PERMISSIONS_READ;
u8 mi_service_change_buf_perm = ATT_PERMISSIONS_READ;
u8 mi_service_change_ccc_perm = ATT_PERMISSIONS_RDWR;

#if MI_API_ENABLE 
const u16 mi_primary_service_uuid = 0xfe95;
u8 mi_pri_service_perm = ATT_PERMISSIONS_READ_AUTHOR;

const u16 mi_version_uuid = 0x0004;
static u8 mi_version_prop = CHAR_PROP_READ;
static u8 mi_version_buf[20]="0.0.1_0000";
const u8 mi_version_str[]="Version";
u8 mi_version_perm = ATT_PERMISSIONS_RDWD_AUTHOR;

const u16 mi_ctrlp_uuid = 0x0010;
static u8 mi_ctrlp_prop = CHAR_PROP_WRITE_WITHOUT_RSP|CHAR_PROP_NOTIFY;
static u8 mi_ctrlp_buf[4];
const u8 mi_ctrlp_str[]="control point";
u8 mi_sec_ctrlp_ccc[2]=	{0x00,0x00};
u8 mi_sec_ctrlp_buf_perm = ATT_PERMISSIONS_RDWD_AUTHOR;
u8 mi_sec_ctrlp_ccc_perm = ATT_PERMISSIONS_RDWD_AUTHOR;


const u16 mi_sec_auth_uuid = 0x0016;
static u8 mi_sec_auth_prop = CHAR_PROP_WRITE_WITHOUT_RSP|CHAR_PROP_NOTIFY;
static u8 mi_sec_auth_buf[20];
const u8 mi_sec_auth_str[]="Security Auth";
u8 mi_sec_auth_ccc[2]=	{0x00,0x00};
u8 mi_sec_auth_buf_perm = ATT_PERMISSIONS_RDWD_AUTHOR;
u8 mi_sec_auth_ccc_perm = ATT_PERMISSIONS_RDWD_AUTHOR;

const u16 mi_ota_ctrl_uuid = 0x0017;
static u8 mi_ota_ctrl_prop = CHAR_PROP_WRITE|CHAR_PROP_NOTIFY;
static u8 mi_ota_ctrl_buf[20];
const u8 mi_ota_ctrl_str[]="Ota ctrl";
u8 mi_ota_ctrl_ccc[2]=	{0x00,0x00};
u8 mi_ota_ctrl_buf_perm = ATT_PERMISSIONS_RDWD_AUTHOR;
u8 mi_ota_ctrl_ccc_perm = ATT_PERMISSIONS_RDWD_AUTHOR;

const u16 mi_ota_data_uuid = 0x0018;
static u8 mi_ota_data_prop = CHAR_PROP_WRITE_WITHOUT_RSP|CHAR_PROP_NOTIFY;
static u8 mi_ota_data_buf[20];
const u8 mi_ota_data_str[]="Ota data";
u8 mi_ota_data_ccc[2]=	{0x00,0x00};
u8 mi_ota_data_buf_perm = ATT_PERMISSIONS_RDWD_AUTHOR;
u8 mi_ota_data_ccc_perm = ATT_PERMISSIONS_RDWD_AUTHOR;
#endif 

#if(AIS_ENABLE)
const u16 ais_pri_service_uuid = 0xfeb3;
const u16 ais_read_uuid = 0xfed4;
const u8 ais_read_prop = CHAR_PROP_READ;
const u16 ais_write_uuid = 0xfed5;
const u8 ais_write_prop = CHAR_PROP_READ|CHAR_PROP_WRITE;
const u16 ais_indicate_uuid = 0xfed6;
const u8 ais_indicate_prop = CHAR_PROP_READ|CHAR_PROP_INDICATE;
const u16 ais_write_without_rsp_uuid = 0xfed7;
const u8 ais_write_without_rsp_prop = CHAR_PROP_READ|CHAR_PROP_WRITE_WITHOUT_RSP;
const u16 ais_notify_uuid = 0xfed8;
const u8 ais_notify_prop = CHAR_PROP_READ|CHAR_PROP_NOTIFY;
const u8 ais_service_desc[]="Alibaba IoT Service";

u8 ais_data_buf[2];
#endif


#define MAX_SERVICE_GAP                 (7)
#define MAX_SERVICE_DEVICE_INFO         (5)
#define MAX_SERVICE_GATT_OTA            (4)
#define MAX_SERVICE_PROVISION           (9)
#define MAX_SERVICE_PROXY               (9)
#define MAX_USER_DEFINE_SET_CCC_ATT_NUM (USER_DEFINE_SET_CCC_ENABLE ? 4 : 0)
#define MAX_MI_ATT_NUM                  (MI_API_ENABLE ? 20 : 0)
#define MAX_SERVICE_CHANGE_ATT_NUM      (5)
#define MAX_AIS_ATT_NUM 	            (AIS_ENABLE ? 12 : 0)
#define MAX_ONLINE_ST_ATT_NUM 	        (ONLINE_STATUS_EN ? 4 : 0)	// ONLINE_STATUS_EN always 0 now.

//---
#define ATT_NUM_START_GAP                   (1)     // line of ATT, start from 0.
#define ATT_NUM_START_DEVICE_INFO           (ATT_NUM_START_GAP + MAX_SERVICE_GAP)
#define ATT_NUM_START_GATT_OTA              (ATT_NUM_START_DEVICE_INFO + MAX_SERVICE_DEVICE_INFO)
#define ATT_NUM_START_PROVISION             (ATT_NUM_START_GATT_OTA + MAX_SERVICE_GATT_OTA)
#define ATT_NUM_START_PROXY                 (ATT_NUM_START_PROVISION + MAX_SERVICE_PROVISION)
#define ATT_NUM_START_USER_DEFINE_SET_CCC   (ATT_NUM_START_PROXY + MAX_SERVICE_PROXY)
#define ATT_NUM_START_MI_API                (ATT_NUM_START_USER_DEFINE_SET_CCC + MAX_USER_DEFINE_SET_CCC_ATT_NUM)
#define ATT_NUM_START_SERVICE_CHANGE        (ATT_NUM_START_MI_API + MAX_MI_ATT_NUM)
#define ATT_NUM_START_AIS                   (ATT_NUM_START_SERVICE_CHANGE + MAX_SERVICE_CHANGE_ATT_NUM)
#define ATT_NUM_START_ONLINE_ST             (ATT_NUM_START_AIS + MAX_AIS_ATT_NUM)
#define ATT_NUM_START_OTS             		(ATT_NUM_START_ONLINE_ST + MAX_ONLINE_ST_ATT_NUM)


#if (MESH_CDTP_ENABLE)
#include "./mesh_cdtp.h"
#if 1//(BLC_AUDIO_OTP_ROLE_SWITCH_ENABLE)	
	////////////////////////////////////// OTS /////////////////////////////////////////////////////
	#if 1//(BLC_AUDIO_OTS_ENABLE)
		#if (CDTP_SMP_LEVEL == 3)
#define CDTP_ATT_PERMISSIONS_READ_V			ATT_PERMISSIONS_AUTHEN_READ
#define CDTP_ATT_PERMISSIONS_WRITE_V		ATT_PERMISSIONS_AUTHEN_WRITE
#define CDTP_ATT_PERMISSIONS_RDWR_V			ATT_PERMISSIONS_AUTHEN_RDWR
		#else // level 4
#define CDTP_ATT_PERMISSIONS_READ_V			ATT_PERMISSIONS_SECURE_CONN_READ
#define CDTP_ATT_PERMISSIONS_WRITE_V		ATT_PERMISSIONS_SECURE_CONN_WRITE
#define CDTP_ATT_PERMISSIONS_RDWR_V			ATT_PERMISSIONS_SECURE_CONN_RDWR
		#endif

u8 CDTP_ATT_PERMISSIONS_READ_VAR = CDTP_ATT_PERMISSIONS_READ_V;
u8 CDTP_ATT_PERMISSIONS_WRITE_VAR = CDTP_ATT_PERMISSIONS_WRITE_V;
u8 CDTP_ATT_PERMISSIONS_RDWR_VAR = CDTP_ATT_PERMISSIONS_RDWR_V;

#define CDTP_ATT_PERMISSIONS_READ			(&CDTP_ATT_PERMISSIONS_READ_VAR)
#define CDTP_ATT_PERMISSIONS_WRITE			(&CDTP_ATT_PERMISSIONS_WRITE_VAR)
#define CDTP_ATT_PERMISSIONS_RDWR			(&CDTP_ATT_PERMISSIONS_RDWR_VAR)

STATIC_ASSERT(CDTP_SMP_LEVEL >= 3);

int app_att_audioOtsRead(void *p);
int app_att_audioOtsWrite(void *p);


enum{ // not be used for b85 mesh SDK, just for compile
	//// OTS ////
	/**********************************************************************************************/
	OTS_PS_H = ATT_NUM_START_OTS, // must not be real handle value and can not be 0, because each handle value enum will be used in blc_audio_setServiceByRole().
	
	OTS_FEATURE_CD_H, //M Mandatory:Read; Optional:
	OTS_FEATURE_DP_H,
	OTS_OBJECT_NAME_CD_H, //M Mandatory:Read; Optional:Write
	OTS_OBJECT_NAME_DP_H,
	OTS_OBJECT_TYPE_CD_H, //M Mandatory:Read; Optional:
	OTS_OBJECT_TYPE_DP_H,
	OTS_OBJECT_SIZE_CD_H, //M Mandatory:Read; Optional:
	OTS_OBJECT_SIZE_DP_H,
	OTS_OBJECT_FIRST_CREATED_CD_H, //O Mandatory:Read; Optional:Write
	OTS_OBJECT_FIRST_CREATED_DP_H,
	OTS_OBJECT_LAST_CREATED_CD_H,  //O Mandatory:Read; Optional:Write
	OTS_OBJECT_LAST_CREATED_DP_H,
	OTS_OBJECT_ID_CD_H, //O Mandatory:Read; Optional:
	OTS_OBJECT_ID_DP_H,
	OTS_OBJECT_PROPERTIES_CD_H, //M Mandatory:Read; Optional:Write
	OTS_OBJECT_PROPERTIES_DP_H,
	OTS_OBJECT_ACTION_CP_CD_H, //M Mandatory:Write,Indicate; Optional:
	OTS_OBJECT_ACTION_CP_DP_H,
	OTS_OBJECT_ACTION_CP_CCC_H,
	OTS_OBJECT_LIST_CP_CD_H, //O Mandatory:Write,Indicate; Optional:
	OTS_OBJECT_LIST_CP_DP_H,
	OTS_OBJECT_LIST_CP_CCC_H,
	OTS_OBJECT_LIST_FILTER_CD_H, //O Mandatory:Read,Write; Optional:
	OTS_OBJECT_LIST_FILTER_DP_H,
	OTS_OBJECT_CHANGED_CD_H, //O Mandatory:Indicate; Optional:
	OTS_OBJECT_CHANGED_DP_H,
	OTS_OBJECT_CHANGED_CCC_H,
};

static const u16 my_OtsServiceUUID = SERVICE_UUID_OBJECT_TRANSFER;
//    OTS Feature    Mandatory:Read; Optional:
static const u16 my_OtsFeatureUUID = CHARACTERISTIC_UUID_OTS_FEATURE;
static const u8 my_OtsFeatureChar[5] = {
	CHAR_PROP_READ,
	U16_LO(OTS_FEATURE_DP_H), U16_HI(OTS_FEATURE_DP_H),
	U16_LO(CHARACTERISTIC_UUID_OTS_FEATURE), U16_HI(CHARACTERISTIC_UUID_OTS_FEATURE),
};

#if 1//OTS_FOR_CDTP_EN // for CDTP
u8 my_OtsFeatureValue[8] = {OACP_CalculateChecksumOpCodeSupported | OACP_ReadOpCodeSupported | OACP_WriteOpCodeSupported};
#else
u8 my_OtsFeatureValue[8] = {
	0xFF, 0x03, 0x00, 0x00, // OACP_Features_field_e // OACP Features Field, all supported. <OTS_V10.pdf> P16
	0x0F, 0x00, 0x00, 0x00, // OLCP_Features_field_e // OLCP Features Field, all supported. <OTS_V10.pdf> P17
};
#endif
//    Object Name    Mandatory:Read; Optional:Write
static const u16 my_OtsObjectNameUUID = CHARACTERISTIC_UUID_OTS_OBJECT_NAME;
static const u8 my_OtsObjectNameChar[5] = {
	CHAR_PROP_READ | CHAR_PROP_WRITE,
	U16_LO(OTS_OBJECT_NAME_DP_H), U16_HI(OTS_OBJECT_NAME_DP_H),
	U16_LO(CHARACTERISTIC_UUID_OTS_OBJECT_NAME), U16_HI(CHARACTERISTIC_UUID_OTS_OBJECT_NAME),
};
u8 my_OtsObjectNameValue[] = {
	'O','b','j','e','c','t',' ','2',
//		'T','e','l','i','n','k',
//		'T','e','l','i','n','k',
//		'T','e','l','i','n','k',
//		'T','e','l','i','n','k',
//		'T','e','l','i','n','k',
//		'T','e','l','i','n','k',
//		'T','e','l','i','n','k',
//		'T','e','l','i','n','k',
};
//    Object Type    Mandatory:Read; Optional:
static const u16 my_OtsObjectTypeUUID = CHARACTERISTIC_UUID_OTS_OBJECT_TYPE;
static const u8 my_OtsObjectTypeChar[5] = {
	CHAR_PROP_READ,
	U16_LO(OTS_OBJECT_TYPE_DP_H), U16_HI(OTS_OBJECT_TYPE_DP_H),
	U16_LO(CHARACTERISTIC_UUID_OTS_OBJECT_TYPE), U16_HI(CHARACTERISTIC_UUID_OTS_OBJECT_TYPE),
};
u8 my_OtsObjectTypeValue[2] = {0xFF,0x7F};
//    Object Size    Mandatory:Read; Optional:
static const u16 my_OtsObjectSizeUUID = CHARACTERISTIC_UUID_OTS_OBJECT_SIZE;
static const u8 my_OtsObjectSizeChar[5] = {
	CHAR_PROP_READ,
	U16_LO(OTS_OBJECT_SIZE_DP_H), U16_HI(OTS_OBJECT_SIZE_DP_H),
	U16_LO(CHARACTERISTIC_UUID_OTS_OBJECT_SIZE), U16_HI(CHARACTERISTIC_UUID_OTS_OBJECT_SIZE),
};
//    Object First-Created    Mandatory:Read; Optional:Write
static const u16 my_OtsFirstCreatedUUID = CHARACTERISTIC_UUID_OTS_OBJECT_FIRST_CREATED;
static const u8 my_OtsFirstCreatedChar[5] = {
	CHAR_PROP_READ | CHAR_PROP_WRITE,
	U16_LO(OTS_OBJECT_FIRST_CREATED_DP_H), U16_HI(OTS_OBJECT_FIRST_CREATED_DP_H),
	U16_LO(CHARACTERISTIC_UUID_OTS_OBJECT_FIRST_CREATED), U16_HI(CHARACTERISTIC_UUID_OTS_OBJECT_FIRST_CREATED),
};

u8 my_OtsFirstCreatedValue[] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //2021-01-21 09:04:46
};
//    Object Last-Modified    Mandatory:Read; Optional:Write
static const u16 my_OtsLastCreatedUUID = CHARACTERISTIC_UUID_OTS_OBJECT_LAST_CREATED;
static const u8 my_OtsLastCreatedChar[5] = {
	CHAR_PROP_READ | CHAR_PROP_WRITE,
	U16_LO(OTS_OBJECT_LAST_CREATED_DP_H), U16_HI(OTS_OBJECT_LAST_CREATED_DP_H),
	U16_LO(CHARACTERISTIC_UUID_OTS_OBJECT_LAST_CREATED), U16_HI(CHARACTERISTIC_UUID_OTS_OBJECT_LAST_CREATED),
};
u8 my_OtsLastCreatedValue[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //2021-01-21 09:08:21
};
//    Object ID    Mandatory:Read; Optional:
static const u16 my_OtsObjectIDUUID = CHARACTERISTIC_UUID_OTS_OBJECT_ID;
static const u8 my_OtsObjectIDChar[5] = {
	CHAR_PROP_READ,
	U16_LO(OTS_OBJECT_ID_DP_H), U16_HI(OTS_OBJECT_ID_DP_H),
	U16_LO(CHARACTERISTIC_UUID_OTS_OBJECT_ID), U16_HI(CHARACTERISTIC_UUID_OTS_OBJECT_ID),
};
u8 my_OtsObjectIDValue[] = {
	0x00, 0x01, 0x00, 0x00, 0x00, 0x00,  //0x000000000100 to 0xFFFFFFFFFFFF.
};
//    Object Properties    Mandatory:Read; Optional:Write
static const u16 my_OtsObjectPropertiesUUID = CHARACTERISTIC_UUID_OTS_OBJECT_PROPERTIES;
static const u8 my_OtsObjectPropertiesChar[5] = {
	CHAR_PROP_READ | CHAR_PROP_WRITE,
	U16_LO(OTS_OBJECT_PROPERTIES_DP_H), U16_HI(OTS_OBJECT_PROPERTIES_DP_H),
	U16_LO(CHARACTERISTIC_UUID_OTS_OBJECT_PROPERTIES), U16_HI(CHARACTERISTIC_UUID_OTS_OBJECT_PROPERTIES),
};
u8 my_OtsObjectPropertiesValue[] = {
	0x8F, 0x00, 0x00, 0x00, //Object Properties, all supported. <OTS_V10.pdf> P22
};
//    Object Action Control Point    Mandatory:Write,Indicate; Optional:
static const u16 my_OtsActionControlPointUUID = CHARACTERISTIC_UUID_OTS_OBJECT_ACTION_CONTROL_POINT;
static const u8 my_OtsActionControlPointChar[5] = {
	CHAR_PROP_WRITE | CHAR_PROP_INDICATE,
	U16_LO(OTS_OBJECT_ACTION_CP_DP_H), U16_HI(OTS_OBJECT_ACTION_CP_DP_H),
	U16_LO(CHARACTERISTIC_UUID_OTS_OBJECT_ACTION_CONTROL_POINT), U16_HI(CHARACTERISTIC_UUID_OTS_OBJECT_ACTION_CONTROL_POINT),
};
u8 my_OtsActionControlPointValue[21] = {0};
u16 my_OtsActionControlPointCCC = 0x0000;
//    Object List Control Point    Mandatory:Write,Indicate; Optional:
static const u16 my_OtsListControlPointUUID = CHARACTERISTIC_UUID_OTS_OBJECT_LIST_CONTROL_POINT;
static const u8 my_OtsListControlPointChar[5] = {
	CHAR_PROP_WRITE | CHAR_PROP_INDICATE,
	U16_LO(OTS_OBJECT_LIST_CP_DP_H), U16_HI(OTS_OBJECT_LIST_CP_DP_H),
	U16_LO(CHARACTERISTIC_UUID_OTS_OBJECT_LIST_CONTROL_POINT), U16_HI(CHARACTERISTIC_UUID_OTS_OBJECT_LIST_CONTROL_POINT),
};
u8 my_OtsListControlPointValue[7] = { 0x00, };
u16 my_OtsListControlPointCCC = 0;
//    Object List Filter    Mandatory:Read,Write; Optional:
static const u16 my_OtsObjectListFilterUUID = CHARACTERISTIC_UUID_OTS_OBJECT_LIST_FILTER;
static const u8 my_OtsObjectListFilterChar[5] = {
	CHAR_PROP_READ | CHAR_PROP_WRITE,
	U16_LO(OTS_OBJECT_LIST_FILTER_DP_H), U16_HI(OTS_OBJECT_LIST_FILTER_DP_H),
	U16_LO(CHARACTERISTIC_UUID_OTS_OBJECT_LIST_FILTER), U16_HI(CHARACTERISTIC_UUID_OTS_OBJECT_LIST_FILTER),
};
u8 my_OtsObjectListFilterValue[15] = { 0x00//ItemList: FilterValue(1Byte)+Size(1Byte)+String, 0x00--No Filter (everything passes)
//		0x01, 0x06, 'T','e','l','i','n','k', //0x01-Name Starts With
//		0x02, 0x06, 'T','e','l','i','n','k', //0x02-Name Ends With
//		0x03, 0x06, 'T','e','l','i','n','k', //0x03-Name Contains
//		0x04, 0x06, 'T','e','T','T','T','T', //0x04-Name is Exactly
};
//    Object Changed    Mandatory:Indicate; Optional:
static const u16 my_OtsObjectChangedUUID = CHARACTERISTIC_UUID_OTS_OBJECT_CHANGED;
static const u8 my_OtsObjectChangedChar[5] = {
	CHAR_PROP_INDICATE,
	U16_LO(OTS_OBJECT_CHANGED_DP_H), U16_HI(OTS_OBJECT_CHANGED_DP_H),
	U16_LO(CHARACTERISTIC_UUID_OTS_OBJECT_CHANGED), U16_HI(CHARACTERISTIC_UUID_OTS_OBJECT_CHANGED),
};
u8 my_OtsObjectChangedValue[7] = {
	0x00, //Flags
	0x00, 0x01, 0x00, 0x00, 0x00, 0x00, //Object ID
};
u16 my_OtsObjectChangedCCC = 0;

	#endif
#endif //#if (BLC_AUDIO_OTP_ROLE_SWITCH_ENABLE)

#define MAX_OTS_ATT_NUM 	            (OTS_OBJECT_CHANGED_CCC_H-OTS_PS_H+1)
#else
#define MAX_OTS_ATT_NUM 	            (0)
#endif

#define ATTRIBUTE_TOTAL_NUM                 (ATT_NUM_START_OTS + MAX_OTS_ATT_NUM - 1)

/*const */u8 PROVISION_ATT_HANDLE = (ATT_NUM_START_PROVISION + 2);  // slave
/*const */u8 GATT_PROXY_HANDLE = (ATT_NUM_START_PROXY + 2);  // slave
const u8 SERVICE_GATT_OTA_HANDLE = (ATT_NUM_START_GATT_OTA + 2);
const u8 SERVICE_CHANGE_ATT_HANDLE_SLAVE = (ATT_NUM_START_SERVICE_CHANGE + 2);
const u8 ONLINE_ST_ATT_HANDLE_SLAVE = (ATT_NUM_START_ONLINE_ST + 2);



#define MY_ATTRIBUTE_BASE0           \
    {ATTRIBUTE_TOTAL_NUM,0,0,0,0,0}, /* total num of attribute*/   \
	/* 0001 - 0007	gap*/   \
    {MAX_SERVICE_GAP,&att_perm_auth_read,2,2,(u8*)(&my_primaryServiceUUID),   (u8*)(&my_gapServiceUUID), 0},\
    {0,&att_perm_auth_read,2,1,(u8*)(&my_characterUUID),        (u8*)(&my_devNameCharacter), 0},\
    {0,&att_perm_auth_read,2,MAX_DEV_NAME_LEN, (u8*)(&my_devNameUUID), (u8*)(&ble_devName), 0},\
    {0,&att_perm_auth_read,2,1,(u8*)(&my_characterUUID),        (u8*)(&my_appearanceCharacter), 0},\
    {0,&att_perm_auth_read,2,sizeof (my_appearance), (u8*)(&my_appearanceUIID),     (u8*)(&my_appearance), 0},\
    {0,&att_perm_auth_read,2,1,(u8*)(&my_characterUUID),        (u8*)(&my_periConnParamChar), 0},\
    {0,&att_perm_auth_read,2,sizeof (my_periConnParameters),(u8*)(&my_periConnParamUUID),   (u8*)(&my_periConnParameters), 0},\
    /* 0008 - 000c  device Information Service*/   \
    {MAX_SERVICE_DEVICE_INFO,&att_perm_auth_read,2,2,(u8*)(&my_primaryServiceUUID),   (u8*)(&my_devServiceUUID), 0},\
    {0,&att_perm_auth_read,2,1,(u8*)(&my_characterUUID),        (u8*)(&my_PnPCharacter), 0},\
    {0,&att_perm_auth_read,2,sizeof (my_PnPtrs),(u8*)(&my_PnPUUID), (u8*)(my_PnPtrs), 0},\
	\
    {0,&att_perm_auth_read,2,1,(u8*)(&my_characterUUID),        (u8*)(&my_fwRevisionCharacter), 0},\
    {0,&att_perm_auth_read,2,FW_REVISION_VALUE_LEN,(u8*)(&my_fwRevisionUUID), (u8*)(my_fwRevision_value), 0},\
    /* 000d - 0010  OTA*/   \
    {MAX_SERVICE_GATT_OTA,&att_perm_auth_read, 2,16,(u8*)(&my_primaryServiceUUID),     (u8*)(&my_OtaServiceUUID), 0},\
    {0,&att_perm_auth_read, 2, 1,(u8*)(&my_characterUUID),      (u8*)(&my_OtaProp), 0}, /*prop*/   \
    {0,&att_perm_auth_rdwd,16,sizeof(my_OtaData),(u8*)(&my_OtaUUID),    (&my_OtaData), &otaWrite, &otaRead}, /*value*/   \
    {0,&att_perm_auth_read, 2,sizeof (my_OtaName),(u8*)(&userdesc_UUID), (u8*)(my_OtaName), 0},

#define MY_ATTRIBUTE_PB_GATT_CHAR           \
    {0,&att_perm_auth_read, 2, 1,(u8*)(&my_characterUUID),      (u8*)(&my_pb_gatt_out_prop), 0}, /*prop*/   \
    {0,&att_perm_auth_rdwd, 2,sizeof(my_pb_gattOutData),(u8*)(&my_pb_gatt_out_UUID),    (my_pb_gattOutData), 0, 0}, /*value*/   \
    {0,&att_perm_auth_read, 2,sizeof (my_pb_gattOutName),(u8*)(&userdesc_UUID), (u8*)(my_pb_gattOutName), 0},\
    {0,&att_perm_auth_rdwd, 2, sizeof(provision_Out_ccc),(u8*)(&clientCharacterCfgUUID),    (u8*)(provision_Out_ccc), &pb_gatt_provision_out_ccc_cb,0}, /*value*/   \
	\
    {0,&att_perm_auth_read, 2, 1,(u8*)(&my_characterUUID),      (u8*)(&my_pb_gatt_in_prop), 0}, /*prop*/   \
    {0,&att_perm_auth_rdwd, 2,sizeof(my_pb_gattInData),(u8*)(&my_pb_gatt_in_UUID),  (my_pb_gattInData), &pb_gatt_Write, 0}, /*value*/   \
    {0,&att_perm_auth_read, 2,sizeof (my_pb_gattInName),(u8*)(&userdesc_UUID), (u8*)(my_pb_gattInName), 0},\
    {0,&att_perm_auth_rdwd, 2, sizeof(provision_In_ccc),(u8*)(&clientCharacterCfgUUID),     (u8*)(provision_In_ccc), 0}, /*value*/

#define MY_ATTRIBUTE_PROXY_GATT_CHAR           \
    {0,&att_perm_auth_read, 2, 1,(u8*)(&my_characterUUID),      (u8*)(&my_proxy_out_prop), 0}, /*prop*/   \
    {0,&att_perm_auth_rdwd, 2,1,(u8*)(&my_proxy_out_UUID),  (my_proxy_outData), 0, 0}, /*value*/   \
    {0,&att_perm_auth_read, 2,sizeof (my_proxy_out_Name),(u8*)(&userdesc_UUID), (u8*)(my_proxy_out_Name), 0},\
    {0,&att_perm_auth_rdwd, 2, sizeof(proxy_Out_ccc),(u8*)(&clientCharacterCfgUUID),    (u8*)(proxy_Out_ccc), &proxy_out_ccc_cb,0}, /*value*/   \
	\
    {0,&att_perm_auth_read, 2, 1,(u8*)(&my_characterUUID),      (u8*)(&my_proxy_in_prop), 0}, /*prop*/   \
    {0,&att_perm_auth_rdwd, 2,1,(u8*)(&my_proxy_in_UUID),   (my_proxy_inData), &proxy_gatt_Write, 0}, /*value*/   \
    {0,&att_perm_auth_read, 2,sizeof (my_proxy_in_Name),(u8*)(&userdesc_UUID), (u8*)(my_proxy_in_Name), 0},\
    {0,&att_perm_auth_rdwd, 2, sizeof(proxy_In_ccc),(u8*)(&clientCharacterCfgUUID),     (u8*)(proxy_In_ccc), 0}, /*value*/   

#if USER_DEFINE_SET_CCC_ENABLE
#define MY_ATTRIBUTE_USER_DEFINE_SET_CCC           \
	{MAX_USER_DEFINE_SET_CCC_ATT_NUM,&att_perm_auth_read, 2,16,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_userdefine_service_UUID), 0},\
	{0,&att_perm_auth_read, 2, 1,(u8*)(&my_characterUUID),		(u8*)(&my_userdefine_prop), 0}, /*prop*/   \
	{0,&att_perm_auth_rdwd,16,sizeof(my_userdefine_dat),(u8*)(&my_userdefine_UUID), (&my_userdefine_dat), &pb_gatt_provision_out_ccc_cb, 0}, /*value*/   \
	{0,&att_perm_auth_read, 2,sizeof (my_userderdefine),(u8*)(&userdesc_UUID), (u8*)(my_userderdefine), 0},
#endif

#if MI_API_ENABLE
#define MY_ATTRIBUTE_MI_API                        \
	{MAX_MI_ATT_NUM,&mi_pri_service_perm, 2,2,(u8*)(&my_primaryServiceUUID),	(u8*)(&mi_primary_service_uuid), 0},\
	{0,&att_perm_auth_read, 2, 1,(u8*)(&my_characterUUID),		(u8*)(&mi_version_prop), 0}, /*prop*/   \
	{0,&mi_version_perm, 2,sizeof(mi_version_buf),(u8*)(&mi_version_uuid),	(mi_version_buf), 0, 0}, /*value*/   \
	{0,&att_perm_auth_read, 2,sizeof (mi_version_str),(u8*)(&userdesc_UUID), (u8*)(mi_version_str), 0},\
    \
	{0,&att_perm_auth_read, 2, 1,(u8*)(&my_characterUUID),		(u8*)(&mi_ctrlp_prop), 0}, /*prop*/   \
	{0,&mi_sec_ctrlp_buf_perm, 2,sizeof(mi_ctrlp_buf),(u8*)(&mi_ctrlp_uuid),	(mi_ctrlp_buf), 0, 0}, /*value*/   \
	{0,&att_perm_auth_read, 2,sizeof (mi_ctrlp_str),(u8*)(&userdesc_UUID), (u8*)(mi_ctrlp_str), 0},\
	{0,&mi_sec_ctrlp_ccc_perm, 2, sizeof(mi_sec_ctrlp_ccc),(u8*)(&clientCharacterCfgUUID), 	(u8*)(mi_sec_ctrlp_ccc), 0}, /*value*/   \
    \
	{0,&att_perm_auth_read, 2, 1,(u8*)(&my_characterUUID),		(u8*)(&mi_sec_auth_prop), 0}, /*prop*/   \
	{0,&mi_sec_auth_buf_perm, 2,sizeof(mi_sec_auth_buf),(u8*)(&mi_sec_auth_uuid),	(mi_sec_auth_buf), 0, 0}, /*value*/   \
	{0,&att_perm_auth_read, 2,sizeof (mi_sec_auth_str),(u8*)(&userdesc_UUID), (u8*)(mi_sec_auth_str), 0},	\
	{0,&mi_sec_auth_ccc_perm, 2, sizeof(mi_sec_auth_ccc),(u8*)(&clientCharacterCfgUUID),	(u8*)(mi_sec_auth_ccc), 0}, /*value*/   \
    \
	{0,&att_perm_auth_read, 2, 1,(u8*)(&my_characterUUID),		(u8*)(&mi_ota_ctrl_prop), 0}, /*prop*/   \
	{0,&mi_ota_ctrl_buf_perm, 2,sizeof(mi_ota_ctrl_buf),(u8*)(&mi_ota_ctrl_uuid),	(mi_ota_ctrl_buf), 0, 0}, /*value*/   \
	{0,&att_perm_auth_read, 2,sizeof (mi_ota_ctrl_str),(u8*)(&userdesc_UUID), (u8*)(mi_ota_ctrl_str), 0},	\
	{0,&mi_ota_ctrl_ccc_perm, 2, sizeof(mi_ota_ctrl_ccc),(u8*)(&clientCharacterCfgUUID),	(u8*)(mi_ota_ctrl_ccc), 0}, /*value*/   \
    \
	{0,&att_perm_auth_read, 2, 1,(u8*)(&my_characterUUID),		(u8*)(&mi_ota_data_prop), 0}, /*prop*/   \
	{0,&mi_ota_data_buf_perm, 2,sizeof(mi_ota_data_buf),(u8*)(&mi_ota_data_uuid),	(mi_ota_data_buf), 0, 0}, /*value*/   \
	{0,&att_perm_auth_read, 2,sizeof (mi_ota_data_str),(u8*)(&userdesc_UUID), (u8*)(mi_ota_data_str), 0},	\
	{0,&mi_ota_data_ccc_perm, 2, sizeof(mi_ota_data_ccc),(u8*)(&clientCharacterCfgUUID),	(u8*)(mi_ota_data_ccc), 0}, /*value*/   
#endif

#define MY_ATTRIBUTE_SERVICE_CHANGE                        \
	{MAX_SERVICE_CHANGE_ATT_NUM,&att_perm_auth_read,2,2,(u8*)(&my_primaryServiceUUID),	(u8*)(&mi_generic_service), 0},\
	{0,&mi_service_change_char_perm, 2, 1,(u8*)(&my_characterUUID), 	(u8*)(&mi_service_change_prop), 0}, /*prop*/   \
	{0,&mi_service_change_buf_perm, 2,sizeof(mi_service_change_buf),(u8*)(&mi_service_change_uuid), (mi_service_change_buf), 0, 0}, /*value*/   \
	{0,&att_perm_auth_read, 2,sizeof (mi_service_change_str),(u8*)(&userdesc_UUID), (u8*)(mi_service_change_str), 0},	\
	{0,&mi_service_change_ccc_perm, 2, sizeof(mi_service_change_ccc),(u8*)(&clientCharacterCfgUUID),	(u8*)(mi_service_change_ccc), 0}, /*value*/   

#if (AIS_ENABLE)
#define MY_ATTRIBUTE_AIS                        \
	{MAX_AIS_ATT_NUM,&att_perm_auth_read, 2,2,(u8*)(&my_primaryServiceUUID),	(u8*)(&ais_pri_service_uuid), 0},\
	{0,&att_perm_auth_read, 2, 1,(u8*)(&my_characterUUID),		(u8*)(&ais_read_prop), 0}, /*prop*/   \
	{0,&att_perm_auth_rdwd, 2,sizeof(ais_data_buf),(u8*)(&ais_read_uuid),	(ais_data_buf), 0, 0}, /*value*/   \
	{0,&att_perm_auth_read, 2, 1,(u8*)(&my_characterUUID),		(u8*)(&ais_write_prop), 0}, /*prop*/   \
	{0,&att_perm_auth_rdwd, 2,sizeof(ais_data_buf),(u8*)(&ais_write_uuid),	(ais_data_buf), &ais_write_pipe, 0}, /*value*/   \
	{0,&att_perm_auth_read, 2, 1,(u8*)(&my_characterUUID),		(u8*)(&ais_indicate_prop), 0}, /*prop*/   \
	{0,&att_perm_auth_read, 2,sizeof(ais_data_buf),(u8*)(&ais_indicate_uuid),	(ais_data_buf), 0, 0}, /*value*/   \
	{0,&att_perm_auth_read, 2, 1,(u8*)(&my_characterUUID),		(u8*)(&ais_write_without_rsp_prop), 0}, /*prop*/   \
	{0,&att_perm_auth_rdwd, 2,sizeof(ais_data_buf),(u8*)(&ais_write_without_rsp_uuid),	(ais_data_buf), &ais_otaWrite, &otaRead}, /*value*/   \
	{0,&att_perm_auth_read, 2, 1,(u8*)(&my_characterUUID),		(u8*)(&ais_notify_prop), 0}, /*prop*/   \
	{0,&att_perm_auth_read, 2,sizeof(ais_data_buf),(u8*)(&ais_notify_uuid),	(ais_data_buf), 0, 0}, /*value*/   \
	{0,&att_perm_auth_read, 2,sizeof (ais_service_desc),(u8*)(&userdesc_UUID), (u8*)(ais_service_desc), 0},
#endif

const attribute_t my_Attributes[] = {
	MY_ATTRIBUTE_BASE0
	
    /* 0011 - 0019      PB-GATT*/
    {9,&att_perm_auth_read, 2,2,(u8*)(&my_primaryServiceUUID),  (u8*)(&my_pb_gattUUID), 0},
    MY_ATTRIBUTE_PB_GATT_CHAR
    
    /* 001a - 0022  PROXY_GATT PART*/
    {9,&att_perm_auth_read, 2,2,(u8*)(&my_primaryServiceUUID),  (u8*)(&my_proxy_gattUUID), 0},
    MY_ATTRIBUTE_PROXY_GATT_CHAR

#if USER_DEFINE_SET_CCC_ENABLE
	// 0023 - 0026	userdefine 
	MY_ATTRIBUTE_USER_DEFINE_SET_CCC
#endif

#if MI_API_ENABLE
    MY_ATTRIBUTE_MI_API
#endif

    MY_ATTRIBUTE_SERVICE_CHANGE
    
#if (AIS_ENABLE)
	// 002c - 0037
	MY_ATTRIBUTE_AIS
#endif
	
#if (MESH_CDTP_ENABLE)//(BLC_AUDIO_OTP_ROLE_SWITCH_ENABLE)
		////////////////////////////////////// OTS /////////////////////////////////////////////////////
	#if 1//(BLC_AUDIO_OTS_ENABLE)
	//	OTS_PS_H, // 0x2c(44)
	{OTS_OBJECT_CHANGED_CCC_H-OTS_PS_H+1,CDTP_ATT_PERMISSIONS_RDWR,2,2,(u8*)(&my_primaryServiceUUID), (u8*)(&my_OtsServiceUUID), 0},
	//	  OTS Feature	 Mandatory:Read; Optional:
	{0,CDTP_ATT_PERMISSIONS_READ,2,sizeof(my_OtsFeatureChar),(u8*)(&my_characterUUID), (u8*)(my_OtsFeatureChar), 0}, //0x2d(45)
	{0,CDTP_ATT_PERMISSIONS_READ,2,sizeof(my_OtsFeatureValue),(u8*)(&my_OtsFeatureUUID), (u8*)(&my_OtsFeatureValue), 0, 0}, // 0x2e(46)
	//	  Object Name	 Mandatory:Read; Optional:Write
	{0,CDTP_ATT_PERMISSIONS_READ,2,sizeof(my_OtsObjectNameChar),(u8*)(&my_characterUUID), (u8*)(my_OtsObjectNameChar), 0},
	{0,CDTP_ATT_PERMISSIONS_RDWR,2,sizeof(my_OtsObjectNameValue),(u8*)(&my_OtsObjectNameUUID), (u8*)(&my_OtsObjectNameValue), app_att_audioOtsWrite, app_att_audioOtsRead}, // 0x30(48)
	//	  Object Type	 Mandatory:Read; Optional:
	{0,CDTP_ATT_PERMISSIONS_READ,2,sizeof(my_OtsObjectTypeChar),(u8*)(&my_characterUUID), (u8*)(my_OtsObjectTypeChar), 0},
	{0,CDTP_ATT_PERMISSIONS_READ,2,sizeof(my_OtsObjectTypeValue),(u8*)(&my_OtsObjectTypeUUID), (u8*)(&my_OtsObjectTypeValue), 0, app_att_audioOtsRead}, // 0x32(50)
	//	  Object Size	 Mandatory:Read; Optional:
	{0,CDTP_ATT_PERMISSIONS_READ,2,sizeof(my_OtsObjectSizeChar),(u8*)(&my_characterUUID), (u8*)(my_OtsObjectSizeChar), 0},
	{0,CDTP_ATT_PERMISSIONS_READ,2,sizeof(my_OtsObjectSizeValue),(u8*)(&my_OtsObjectSizeUUID), (u8*)(&my_OtsObjectSizeValue), 0, app_att_audioOtsRead}, // 0x34(52)
	//	  Object First-Created	  Mandatory:Read; Optional:Write
	{0,CDTP_ATT_PERMISSIONS_READ,2,sizeof(my_OtsFirstCreatedChar),(u8*)(&my_characterUUID), (u8*)(my_OtsFirstCreatedChar), 0},
	{0,CDTP_ATT_PERMISSIONS_RDWR,2,sizeof(my_OtsFirstCreatedValue),(u8*)(&my_OtsFirstCreatedUUID), (u8*)(&my_OtsFirstCreatedValue), app_att_audioOtsWrite, app_att_audioOtsRead},
	//	  Object Last-Modified	  Mandatory:Read; Optional:Write
	{0,CDTP_ATT_PERMISSIONS_READ,2,sizeof(my_OtsLastCreatedChar),(u8*)(&my_characterUUID), (u8*)(my_OtsLastCreatedChar), 0},
	{0,CDTP_ATT_PERMISSIONS_RDWR,2,sizeof(my_OtsLastCreatedValue),(u8*)(&my_OtsLastCreatedUUID), (u8*)(&my_OtsLastCreatedValue), app_att_audioOtsWrite, app_att_audioOtsRead},
	//	  Object ID    Mandatory:Read; Optional:
	{0,CDTP_ATT_PERMISSIONS_READ,2,sizeof(my_OtsObjectIDChar),(u8*)(&my_characterUUID), (u8*)(my_OtsObjectIDChar), 0},
	{0,CDTP_ATT_PERMISSIONS_READ,2,sizeof(my_OtsObjectIDValue),(u8*)(&my_OtsObjectIDUUID), (u8*)(&my_OtsObjectIDValue), 0, app_att_audioOtsRead},
	//	  Object Properties    Mandatory:Read; Optional:Write
	{0,CDTP_ATT_PERMISSIONS_READ,2,sizeof(my_OtsObjectPropertiesChar),(u8*)(&my_characterUUID), (u8*)(my_OtsObjectPropertiesChar), 0},
	{0,CDTP_ATT_PERMISSIONS_RDWR,2,sizeof(my_OtsObjectPropertiesValue),(u8*)(&my_OtsObjectPropertiesUUID), (u8*)(&my_OtsObjectPropertiesValue), app_att_audioOtsWrite, app_att_audioOtsRead}, // 0x3c(60): only read
	//	  Object Action Control Point	 Mandatory:Write,Indicate; Optional:
	{0,CDTP_ATT_PERMISSIONS_READ,2,sizeof(my_OtsActionControlPointChar),(u8*)(&my_characterUUID), (u8*)(my_OtsActionControlPointChar), 0},
	{0,CDTP_ATT_PERMISSIONS_WRITE,2,sizeof(my_OtsActionControlPointValue),(u8*)(&my_OtsActionControlPointUUID), (u8*)(&my_OtsActionControlPointValue), app_att_audioOtsWrite, 0},//0x3e(62)
	{0,CDTP_ATT_PERMISSIONS_RDWR,2,sizeof(my_OtsActionControlPointCCC),(u8*)(&clientCharacterCfgUUID), (u8*)(&my_OtsActionControlPointCCC), 0}, // 0x3f(63)
	//	  Object List Control Point    Mandatory:Write,Indicate; Optional:
	{0,CDTP_ATT_PERMISSIONS_READ,2,sizeof(my_OtsListControlPointChar),(u8*)(&my_characterUUID), (u8*)(my_OtsListControlPointChar), 0},
	{0,CDTP_ATT_PERMISSIONS_WRITE,2,sizeof(my_OtsListControlPointValue),(u8*)(&my_OtsListControlPointUUID), (u8*)(&my_OtsListControlPointValue), app_att_audioOtsWrite, 0},
	{0,CDTP_ATT_PERMISSIONS_RDWR,2,sizeof(my_OtsListControlPointCCC),(u8*)(&clientCharacterCfgUUID), (u8*)(&my_OtsListControlPointCCC), 0},
	//	  Object List Filter	Mandatory:Read,Write; Optional:
	{0,CDTP_ATT_PERMISSIONS_READ,2,sizeof(my_OtsObjectListFilterChar),(u8*)(&my_characterUUID), (u8*)(my_OtsObjectListFilterChar), 0},
	{0,CDTP_ATT_PERMISSIONS_RDWR,2,sizeof(my_OtsObjectListFilterValue),(u8*)(&my_OtsObjectListFilterUUID), (u8*)(&my_OtsObjectListFilterValue), app_att_audioOtsWrite, app_att_audioOtsRead},
	//	  Object Changed	Mandatory:Indicate; Optional:
	{0,CDTP_ATT_PERMISSIONS_READ,2,sizeof(my_OtsObjectChangedChar),(u8*)(&my_characterUUID), (u8*)(my_OtsObjectChangedChar), 0},
	{0, 			   0x00,2,sizeof(my_OtsObjectChangedValue),(u8*)(&my_OtsObjectChangedUUID), (u8*)(&my_OtsObjectChangedValue), 0, 0},
	{0,CDTP_ATT_PERMISSIONS_RDWR,2,sizeof(my_OtsObjectChangedCCC),(u8*)(&clientCharacterCfgUUID), (u8*)(&my_OtsObjectChangedCCC), 0},
	#endif
#endif //#if (BLC_AUDIO_OTP_ROLE_SWITCH_ENABLE)

};

void my_att_init(u8 mode)
{
	u8 device_name[] = DEV_NAME;
	bls_att_setDeviceName(device_name, sizeof(DEV_NAME));
	bls_att_setAttributeTable ((u8 *)my_Attributes);
#if ATT_TAB_SWITCH_ENABLE
    u8 unused_gattUUID[2] = SIG_MESH_ATT_UNUSED;
	if(mode == GATT_PROVISION_MODE){
        u8 pb_gattUUID[2]=SIG_MESH_PROVISION_SERVICE;
		memcpy(my_pb_gattUUID, pb_gattUUID, sizeof(my_pb_gattUUID));
		memcpy(my_proxy_gattUUID, unused_gattUUID, sizeof(my_proxy_gattUUID));
	}else if(mode == GATT_PROXY_MODE){
        u8 proxy_gattUUID[2]= SIG_MESH_PROXY_SERVICE;
		memcpy(my_pb_gattUUID, unused_gattUUID, sizeof(my_pb_gattUUID));
		memcpy(my_proxy_gattUUID, proxy_gattUUID, sizeof(my_proxy_gattUUID));
	}
#endif 
}

#if (MESH_CDTP_ENABLE)
int blc_audio_setServiceByRole(u8 role, BLC_AUDIO_SERVICE_ENUM srvType, attribute_t *pService);

void app_audio_gatt_init(void)
{
	int ret;

	//bls_att_setAttributeTable((u8 *)my_Attributes);

	ret = 0;
	#if 1//(BLC_AUDIO_OTP_ROLE_SWITCH_ENABLE)
		#if 1//(BLC_AUDIO_OTS_ENABLE)
			ret += blc_audio_setServiceByRole(BLC_AUDIO_ROLE_SERVER, BLC_AUDIO_SERVICE_OTS, (attribute_t*)&my_Attributes[ATT_NUM_START_OTS]); // OTS_PS_H
		#endif //(BLC_AUDIO_TBS_ENABLE)
	#endif //#if (BLC_AUDIO_OTP_ROLE_SWITCH_ENABLE)
	if(ret != 0){
		
	}
	my_dump_str_data(APP_LOG_EN, "app_audio_gatt_init_ return: ", (u8 *)&ret, 4);
	//printf("app_audios_gatt_init: %d\r\n", ret);
}

int blc_audio_otpAttRead(u16 connHandle, void* p);
int blc_audio_otpAttWrite(u16 connHandle, void* p);

int app_att_audioOtsRead(void *p)
{
	my_dump_str_data(APP_LOG_EN, "ots att_read_enter:", NULL, 0); // p, sizeof(rf_packet_att_read_t));
	rf_packet_att_read_t *p_r = (rf_packet_att_read_t *)p; // include dma len for BLE mesh library, not include dma len for new ble generic sdk.
	if(blc_audio_otpAttRead(BLS_CONN_HANDLE, &p_r->type) == BLC_AUDIO_SUCCESS)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int app_att_audioOtsWrite(void *p)
{
	my_dump_str_data(APP_LOG_EN, "ots att_write_enter:", NULL, 0); // p, sizeof(rf_packet_att_write_t));
	rf_packet_att_write_t *p_w = (rf_packet_att_write_t *)p; // include dma len for BLE mesh library, not include dma len for new ble generic sdk.
	return blc_audio_otpAttWrite(BLS_CONN_HANDLE, &p_w->type);
}
#endif


#endif
