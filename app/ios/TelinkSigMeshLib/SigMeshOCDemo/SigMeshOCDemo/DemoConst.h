/********************************************************************************************************
 * @file     DemoConst.h 
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/7/31
 *
 * @par     Copyright (c) [2021], Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
 *******************************************************************************************************/

#ifndef DemoConst_h
#define DemoConst_h

//CellIdentifiers
#define CellIdentifiers_HomeItemCellID  @"HomeItemCell"
#define CellIdentifiers_DeviceGroupListCellID  @"DeviceGroupListCell"
#define CellIdentifiers_GroupCellID  @"GroupCell"
#define CellIdentifiers_MeshOTAItemCellID  @"MeshOTAItemCell"
#define CellIdentifiers_ModelIDCellID  @"ModelIDCell"
#define CellIdentifiers_ChooseModelIDCellID  @"ChooseModelIDCell"
#define CellIdentifiers_ChooseBinCellID  @"ChooseBinCell"
#define CellIdentifiers_SceneItemCellID  @"SceneItemCell"
#define CellIdentifiers_SchedulerCellID  @"SchedulerCell"
#define CellIdentifiers_SettingItemCellID  @"SettingItemCell"
#define CellIdentifiers_ActionItemCellID  @"ActionItemCell"
#define CellIdentifiers_AddDeviceItemCellID  @"AddDeviceItemCell"
#define CellIdentifiers_OnOffItemCellID  @"OnOffItemCell"
#define CellIdentifiers_InfoSwitchCellID  @"InfoSwitchCell"
#define CellIdentifiers_InfoNextCellID  @"InfoNextCell"
#define CellIdentifiers_ExtendBearerModeCellID  @"ExtendBearerModeCell"
#define CellIdentifiers_InfoButtonCellID  @"InfoButtonCell"
#define CellIdentifiers_ColorModelCellID  @"ColorModelCell"
#define CellIdentifiers_OnOffModelCellID  @"OnOffModelCell"
#define CellIdentifiers_LevelAndSliderCellID  @"LevelAndSliderCell"
#define CellIdentifiers_RemoteElementCellID  @"RemoteElementCell"
#define CellIdentifiers_EntryCellID  @"EntryCell"


//ViewControllerIdentifiers
#define ViewControllerIdentifiers_LogViewControllerID   @"LogViewController"
#define ViewControllerIdentifiers_HomeViewControllerID  @"HomeViewController"
#define ViewControllerIdentifiers_ReKeyBindViewControllerID @"ReKeyBindViewController"
#define ViewControllerIdentifiers_SingleOTAViewControllerID @"SingleOTAViewController"
#define ViewControllerIdentifiers_GroupViewControllerID  @"GroupViewController"
#define ViewControllerIdentifiers_GroupDetailViewControllerID   @"GroupDetailViewController"
#define ViewControllerIdentifiers_SettingViewControllerID   @"SettingViewController"
#define ViewControllerIdentifiers_CMDViewControllerID   @"CMDViewController"
#define ViewControllerIdentifiers_HSLViewControllerID   @"HSLViewController"
#define ViewControllerIdentifiers_AddDeviceViewControllerID   @"AddDeviceViewController"
#define ViewControllerIdentifiers_RemoteAddVCID   @"RemoteAddVC"
#define ViewControllerIdentifiers_FastProvisionAddViewControllerID  @"FastProvisionAddVC"

#define ViewControllerIdentifiers_ModelIDListViewControllerID  @"ModelIDListViewController"
#define ViewControllerIdentifiers_SceneListViewControllerID  @"SceneListViewController"
#define ViewControllerIdentifiers_SceneDetailViewControllerID  @"SceneDetailViewController"
#define ViewControllerIdentifiers_ChooseActionViewControllerID  @"ChooseActionViewController"
#define ViewControllerIdentifiers_ActionViewControllerID  @"ActionViewController"
#define ViewControllerIdentifiers_DeviceCompositionDataVCID @"DeviceCompositionDataVC"
#define ViewControllerIdentifiers_SubnetBridgeListVCID @"SubnetBridgeListVC"
#define ViewControllerIdentifiers_AddBridgeTableVCID @"AddBridgeTableVC"
#define ViewControllerIdentifiers_AddForwardingTableVCID @"AddForwardingTableVC"
#define ViewControllerIdentifiers_ChooseEntryVCID @"ChooseEntryVC"
#define ViewControllerIdentifiers_SchedulerListViewControllerID  @"SchedulerListViewController"
#define ViewControllerIdentifiers_SchedulerDetailViewControllerID  @"SchedulerDetailViewController"
#define ViewControllerIdentifiers_EditModelIDsViewControllerID  @"EditModelIDsViewController"
#define ViewControllerIdentifiers_ShareViewControllerID  @"ShareVC"
#define ViewControllerIdentifiers_ShareOutViewControllerID  @"ShareOutVC"
#define ViewControllerIdentifiers_ShareInViewControllerID  @"ShareInVC"
#define ViewControllerIdentifiers_FileChooseViewControllerID  @"FileChooseVC"
#define ViewControllerIdentifiers_MeshInfoViewControllerID  @"MeshInfoVC"
#define ViewControllerIdentifiers_SettingsVCID  @"SettingsVC"
#define ViewControllerIdentifiers_ChooseAndAddDeviceViewControllerID  @"ChooseAndAddDeviceVC"
#define ViewControllerIdentifiers_ShowQRCodeViewControllerID  @"ShowQRCodeViewController"
#define ViewControllerIdentifiers_TestVCID  @"TestVC"
#define ViewControllerIdentifiers_DirectControlListVCID  @"DirectControlListVC"

#define ViewControllerIdentifiers_SingleDeviceViewControllerID  @"SingleDeviceViewController"
#define ViewControllerIdentifiers_DeviceControlViewControllerID  @"DeviceControlViewController"
#define ViewControllerIdentifiers_DeviceRemoteVCID  @"DeviceRemoteVC"
#define ViewControllerIdentifiers_DeviceGroupViewControllerID  @"DeviceGroupViewController"
#define ViewControllerIdentifiers_DeviceSettingViewControllerID  @"DeviceSettingViewController"
#define ViewControllerIdentifiers_ForwardingTableVCID  @"ForwardingTableVC"
#define ViewControllerIdentifiers_DeviceSubscriptionListViewControllerID  @"DeviceSubscriptionListViewController"
#define ViewControllerIdentifiers_SensorVCID  @"SensorVC"
#define ViewControllerIdentifiers_ShareTipsVCID @"ShareTipsVC"

#define kCommandInterval    (0.32)

//TimeOut
#define TimeOut_KickoutDelayResponseTime    (1.5)
#define TimeOut_KickoutConnectedDelayResponseTime   (5.0)

//Tip
#define Tip_CommandBusy @"SDK work in the sky, busy now"
#define Tip_DeviceOutline   @"device is outline"
#define Tip_DisconnectOrConnectFail @"diconnect or connect fail"
#define Tip_KickoutDevice   @"kick out ..."
#define Tip_ReKeyBindDevice @"reKeyBind ..."
#define Tip_ReKeyBindDeviceSuccess  @"reKeyBind success ..."
#define Tip_ReKeyBindDeviceFail @"KeyBind fail, retry?"
#define Tip_OTAFileError    @"OTA data error, not exist or missing"

#define Tip_AddNewDevices   @"ready scan devices ..."
#define Tip_AddNextDevices  @"add another device ..."
#define Tip_AddDeviceProvisionSuccess   @"provision success keyBinding ..."
#define Tip_AddDeviceKeyBindSuccess @"keyBind success, scan other ..."
#define Tip_AddDeviceFail   @"add device fail, scan other ..."
#define Tip_FinishAddNewDevices @"finish add devices ..."

#define Tip_EditGroup   @"editing group ..."
#define Tip_EditGroupFail   @"editing group fail ..."

#define Tip_SaveScene   @"save scene ..."
#define Tip_SaveSceneSuccess   @"save scene success ..."
#define Tip_DeleteScene   @"delete scene ..."
#define Tip_DeleteSceneSuccess   @"delete scene success ..."
#define Tip_SelectScene   @"select at least one item!"

#define Tip_GetComposition   @"get composition ..."
#define Tip_GetCompositionFail   @"get composition fail ..."
#define Tip_GetCompositionSuccess   @"get composition success ..."

#define Tip_GetModelsMetadata   @"get models Metadata ..."
#define Tip_GetModelsMetadataFail   @"get models Metadata fail ..."
#define Tip_GetModelsMetadataSuccess   @"get models Metadata success ..."

#define Tip_GetSARTransmitter   @"get SARTransmitter ..."
#define Tip_GetSARTransmitterFail   @"get SARTransmitter fail ..."
#define Tip_GetSARTransmitterSuccess   @"get SARTransmitter success ..."
#define Tip_SetSARTransmitter   @"set SARTransmitter ..."
#define Tip_SetSARTransmitterFail   @"set SARTransmitter fail ..."
#define Tip_SetSARTransmitterSuccess   @"set SARTransmitter success ..."

#define Tip_GetSARReceiver   @"get SARReceiver ..."
#define Tip_GetSARReceiverFail   @"get SARReceiver fail ..."
#define Tip_GetSARReceiverSuccess   @"get SARReceiver success ..."
#define Tip_SetSARReceiver   @"set SARReceiver ..."
#define Tip_SetSARReceiverFail   @"set SARReceiver fail ..."
#define Tip_SetSARReceiverSuccess   @"set SARReceiver success ..."

#define Tip_ClearSolicitationPduRplItems   @"clear solicitationPduRplItems ..."
#define Tip_ClearSolicitationPduRplItemsFail   @"clear solicitationPduRplItems fail ..."
#define Tip_ClearSolicitationPduRplItemsSuccess   @"clear solicitationPduRplItems success ..."
#define Tip_ClearSolicitationPduRplItemsUnacknowledged   @"clear solicitationPduRplItems unacknowledged ..."
#define Tip_ClearSolicitationPduRplItemsUnacknowledgedFail   @"clear solicitationPduRplItems unacknowledged fail ..."
#define Tip_ClearSolicitationPduRplItemsUnacknowledgedSuccess   @"clear solicitationPduRplItems unacknowledged success ..."

#define Tip_GetOnDemandPrivateProxy   @"get onDemandPrivateProxy ..."
#define Tip_GetOnDemandPrivateProxyFail   @"get onDemandPrivateProxy fail ..."
#define Tip_GetOnDemandPrivateProxySuccess   @"get onDemandPrivateProxy success ..."
#define Tip_SetOnDemandPrivateProxy   @"set onDemandPrivateProxy ..."
#define Tip_SetOnDemandPrivateProxyFail   @"set onDemandPrivateProxy fail ..."
#define Tip_SetOnDemandPrivateProxySuccess   @"set onDemandPrivateProxy success ..."

#define Tip_AddSubnetBridge   @"add subnet bridge ..."
#define Tip_AddSubnetBridgeFail   @"add subnet bridge fail ..."
#define Tip_AddSubnetBridgeSuccess   @"add subnet bridge success ..."

#define Tip_SetDirectControl   @"set direct control ..."
#define Tip_SetDirectControlFail   @"set direct control fail ..."
#define Tip_SetDirectControlSuccess   @"set direct control success ..."

#define Tip_AddFrowardingTable   @"add forwarding table ..."
#define Tip_AddFrowardingTableSomeFail   @"some node add forwarding table fail ..."
#define Tip_AddFrowardingTableAllFail   @"all node add forwarding table fail ..."
#define Tip_AddFrowardingTableSuccess   @"add forwarding table success ..."
#define Tip_EditFrowardingTable   @"edit forwarding table ..."
#define Tip_EditFrowardingTableSomeFail   @"some node edit forwarding table fail ..."
#define Tip_EditFrowardingTableAllFail   @"all node edit forwarding table fail ..."
#define Tip_EditFrowardingTableSuccess   @"edit forwarding table success ..."


//分享使用方式(是否使用蓝牙点对点传输，YES为二维码加蓝牙点对点，NO为存二维码)
#define kShareWithBluetoothPointToPoint (YES)
//setting界面显示
#define kShowScenes (YES)
#define kshowLog        (YES)
#define kshowShare    (YES)
#define kshowMeshInfo    (YES)
#define kshowMeshSettings    (YES)
#define kshowChooseAdd    (YES)
#define kshowTest        (YES)

#define kKeyBindType  @"kKeyBindType"
#define kRemoteAddType  @"kRemoteAddType"
#define kFastAddType  @"kFastAddType"
#define kDLEType  @"kDLEType"
#define kGetOnlineStatusType  @"kGetOnlineStatusType"
#define kAddStaticOOBDeviceByNoOOBEnable  @"kAddStaticOOBDeviceByNoOOBEnable"
#define kDistributorAddress  @"kDistributorAddress"
#define kDistributorPolicy  @"kDistributorPolicy"
#define kUpdateNodeAddresses  @"kUpdateNodeAddresses"
#define kRootCertificateName  @"kRootCertificateName"
#define kDirectedSecurityEnable  @"kDirectedSecurityEnable"


//app通用蓝色
#define kDefultColor [UIColor colorWithRed:0x4A/255.0 green:0x87/255.0 blue:0xEE/255.0 alpha:1]
#define kDefultUnableColor [UIColor colorWithRed:0x4A/255.0 green:0x87/255.0 blue:0xEE/255.0 alpha:0.5]
//获取导航栏+状态栏的高度
#define kGetRectNavAndStatusHight  (self.navigationController.navigationBar.frame.size.height+[[UIApplication sharedApplication] statusBarFrame].size.height)

#define SCANSPACEOFFSET 0.15f

#define SCREENBOUNDS [UIScreen mainScreen].bounds
#define SCREEN_WIDTH CGRectGetWidth([UIScreen mainScreen].bounds)
#define SCREEN_HEIGHT CGRectGetHeight([UIScreen mainScreen].bounds)

//默认指令的重试次数
#define kDefaultRetryCount  (2)

#define kPublishIntervalOfDemo  (20)

#endif /* DemoConst_h */
