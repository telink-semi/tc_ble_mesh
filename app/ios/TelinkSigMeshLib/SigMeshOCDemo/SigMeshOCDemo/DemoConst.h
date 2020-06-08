/********************************************************************************************************
 * @file     DemoConst.h 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/
//
//  DemoConst.h
//  SigMeshOCDemo
//
//  Created by 梁家誌 on 2018/7/31.
//  Copyright © 2018年 Telink. All rights reserved.
//

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
#define CellIdentifiers_InfoButtonCellID  @"InfoButtonCell"

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
#define ViewControllerIdentifiers_SchedulerListViewControllerID  @"SchedulerListViewController"
#define ViewControllerIdentifiers_SchedulerDetailViewControllerID  @"SchedulerDetailViewController"
#define ViewControllerIdentifiers_EditModelIDsViewControllerID  @"EditModelIDsViewController"
#define ViewControllerIdentifiers_ShareViewControllerID  @"ShareVC"
#define ViewControllerIdentifiers_ShareOutViewControllerID  @"ShareOutVC"
#define ViewControllerIdentifiers_ShareInViewControllerID  @"ShareInVC"
#define ViewControllerIdentifiers_FileChooseViewControllerID  @"FileChooseVC"
#define ViewControllerIdentifiers_MeshInfoViewControllerID  @"MeshInfoVC"
#define ViewControllerIdentifiers_ChooseAndAddDeviceViewControllerID  @"ChooseAndAddDeviceVC"
#define ViewControllerIdentifiers_DebugViewControllerID  @"DebugViewController"
#define ViewControllerIdentifiers_ShowQRCodeViewControllerID  @"ShowQRCodeViewController"

#define ViewControllerIdentifiers_SingleDeviceViewControllerID  @"SingleDeviceViewController"
#define ViewControllerIdentifiers_DeviceControlViewControllerID  @"DeviceControlViewController"
#define ViewControllerIdentifiers_DeviceGroupViewControllerID  @"DeviceGroupViewController"
#define ViewControllerIdentifiers_DeviceSettingViewControllerID  @"DeviceSettingViewController"
#define ViewControllerIdentifiers_DeviceSubscriptionListViewControllerID  @"DeviceSubscriptionListViewController"
#define ViewControllerIdentifiers_SensorVCID  @"SensorVC"

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

//分享使用方式(是否使用蓝牙点对点传输，YES为二维码加蓝牙点对点，NO为存二维码)
#define kShareWithBluetoothPointToPoint (YES)
//setting界面显示
#define kShowScenes (YES)
#define kShowDebug  (NO)
#define kshowLog        (YES)
#define kshowShare    (YES)
#define kshowMeshInfo    (YES)
#define kshowChooseAdd    (YES)

//HSL发送数据包是使用RGB转HSL的模式
#define kControllerInHSL    (YES)
//HSL发送数据包是使用RGB转HSV的模式（iOS系统提供的接口就是RGB转HSV的接口）
//#define kControllerInHSL    (NO)

#define kKeyBindType  @"kKeyBindType"
#define kRemoteAddType  @"kRemoteAddType"
#define kFastAddType  @"kFastAddType"
#define kGetOnlineStatusType  @"kGetOnlineStatusType"
#define kAddStaticOOBDevcieByNoOOBEnable  @"kAddStaticOOBDevcieByNoOOBEnable"


//app通用蓝色
#define kDefultColor [UIColor colorWithRed:0x4A/255.0 green:0x87/255.0 blue:0xEE/255.0 alpha:1]
#define kDefultUnableColor [UIColor colorWithRed:0x4A/255.0 green:0x87/255.0 blue:0xEE/255.0 alpha:0.5]
//获取导航栏+状态栏的高度
#define kGetRectNavAndStatusHight  (self.navigationController.navigationBar.frame.size.height+[[UIApplication sharedApplication] statusBarFrame].size.height)

#define SCANSPACEOFFSET 0.15f

#define SCREENBOUNDS [UIScreen mainScreen].bounds
#define SCREEN_WIDTH CGRectGetWidth([UIScreen mainScreen].bounds)
#define SCREEN_HEIGHT CGRectGetHeight([UIScreen mainScreen].bounds)

#endif /* DemoConst_h */
