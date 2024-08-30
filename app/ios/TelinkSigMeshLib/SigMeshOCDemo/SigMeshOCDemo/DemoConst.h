/********************************************************************************************************
 * @file     DemoConst.h
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2018/7/31
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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

typedef enum : UInt8 {
    //选择多个设备Provision
    ProvisionMode_normalSelectable = 0,
    //自动多个设备连续Provision
    ProvisionMode_normalAuto,
    //remote Provision
    ProvisionMode_remoteProvision,
    //fast Provision
    ProvisionMode_fastProvision,
} ProvisionMode;

typedef enum : UInt8 {
    //提示切换
    ImportSwitchMode_manual = 0,
    //自动切换
    ImportSwitchMode_auto,
} ImportSwitchMode;

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
#define CellIdentifiers_AddDeviceItemCellID  @"AddDeviceItemCell"
#define CellIdentifiers_OnOffItemCellID  @"OnOffItemCell"
#define CellIdentifiers_InfoSwitchCellID  @"InfoSwitchCell"
#define CellIdentifiers_ExtendBearerModeCellID  @"ExtendBearerModeCell"
#define CellIdentifiers_ColorModelCellID  @"ColorModelCell"
#define CellIdentifiers_OnOffModelCellID  @"OnOffModelCell"
#define CellIdentifiers_LevelAndSliderCellID  @"LevelAndSliderCell"
#define CellIdentifiers_RemoteElementCellID  @"RemoteElementCell"
#define CellIdentifiers_EntryCellID  @"EntryCell"
#define CellIdentifiers_AddDeviceCellID  @"AddDeviceCell"
#define CellIdentifiers_ProxyFilterCellID  @"ProxyFilterCell"
#define CellIdentifiers_FilterAddressCellID  @"FilterAddressCell"
#define CellIdentifiers_SettingDetailItemCellID  @"SettingDetailItemCell"


//ViewControllerIdentifiers
#define ViewControllerIdentifiers_LogViewControllerID   @"LogViewController"
#define ViewControllerIdentifiers_HomeViewControllerID  @"HomeViewController"
#define ViewControllerIdentifiers_ReKeyBindViewControllerID @"ReKeyBindViewController"
#define ViewControllerIdentifiers_GroupViewControllerID  @"GroupViewController"
#define ViewControllerIdentifiers_GroupDetailViewControllerID   @"GroupDetailViewController"
#define ViewControllerIdentifiers_SettingViewControllerID   @"SettingViewController"
#define ViewControllerIdentifiers_HSLViewControllerID   @"HSLViewController"
#define ViewControllerIdentifiers_FastProvisionAddViewControllerID  @"FastProvisionAddVC"
#define ViewControllerIdentifiers_AddDeviceVCID   @"AddDeviceVC"
#define ViewControllerIdentifiers_AutoAddDeviceVCID @"AutoAddDeviceVC"

#define ViewControllerIdentifiers_SceneListViewControllerID  @"SceneListViewController"
#define ViewControllerIdentifiers_SceneDetailViewControllerID  @"SceneDetailViewController"
#define ViewControllerIdentifiers_ChooseActionViewControllerID  @"ChooseActionViewController"
#define ViewControllerIdentifiers_ActionViewControllerID  @"ActionViewController"
#define ViewControllerIdentifiers_DeviceCompositionDataVCID @"DeviceCompositionDataVC"
#define ViewControllerIdentifiers_SubnetBridgeListVCID @"SubnetBridgeListVC"
#define ViewControllerIdentifiers_AddBridgeTableVCID @"AddBridgeTableVC"
#define ViewControllerIdentifiers_AddForwardingTableVCID @"AddForwardingTableVC"
#define ViewControllerIdentifiers_ChooseEntryVCID @"ChooseEntryVC"
#define ViewControllerIdentifiers_ShareOutViewControllerID  @"ShareOutVC"
#define ViewControllerIdentifiers_ShareInViewControllerID  @"ShareInVC"
#define ViewControllerIdentifiers_FileChooseViewControllerID  @"FileChooseVC"
#define ViewControllerIdentifiers_MeshInfoViewControllerID  @"MeshInfoVC"
#define ViewControllerIdentifiers_ShowQRCodeViewControllerID  @"ShowQRCodeViewController"
#define ViewControllerIdentifiers_TestVCID  @"TestVC"
#define ViewControllerIdentifiers_DirectControlListVCID  @"DirectControlListVC"
#define ViewControllerIdentifiers_CDTPExportVCID  @"CDTPExportVC"
#define ViewControllerIdentifiers_CDTPServiceListVCID  @"CDTPServiceListVC"

#define ViewControllerIdentifiers_SingleDeviceViewControllerID  @"SingleDeviceViewController"
#define ViewControllerIdentifiers_DeviceControlViewControllerID  @"DeviceControlViewController"
#define ViewControllerIdentifiers_DeviceRemoteVCID  @"DeviceRemoteVC"
#define ViewControllerIdentifiers_DeviceGroupViewControllerID  @"DeviceGroupViewController"
#define ViewControllerIdentifiers_DeviceSettingViewControllerID  @"DeviceSettingViewController"
#define ViewControllerIdentifiers_DeviceSubscriptionListViewControllerID  @"DeviceSubscriptionListViewController"
#define ViewControllerIdentifiers_SensorVCID  @"SensorVC"
#define ViewControllerIdentifiers_ShareTipsVCID @"ShareTipsVC"
#define ViewControllerIdentifiers_DirectForwardingVCID  @"DirectForwardingVC"
#define ViewControllerIdentifiers_NetworkListVCID  @"NetworkListVC"
#define ViewControllerIdentifiers_SettingsViewControllerID  @"SettingsViewController"

#define kCommandInterval    (0.32)

//TimeOut
#define TimeOut_KickoutDelayResponseTime    (1.5)
#define TimeOut_KickoutConnectedDelayResponseTime   (5.0)

//Tip
#define Tip_CommandBusy @"Sending command busy···"
#define Tip_DeviceOutline   @"device is outline"
#define Tip_DisconnectOrConnectFail @"diconnect or connect fail"
#define Tip_KickOutDevice   @"kick out ..."
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
#define Tip_SaveSceneFail   @"save scene fail ..."
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

#define TipsTitle_CDTPImport @"CDTP Import"
#define TipsTitle_CDTPExportToOtherPhone @"CDTP Export To Other Phone"
#define TipsTitle_CDTPExportToGateway @"CDTP Export To Gateway"
#define TipsTitle_QRCodeAndCloudTransferJSON @"QRCode + Cloud Transfer JSON"
#define TipsTitle_JSONFile @"JSON File"
#define TipsMessage_QRCodeAndCDTPTransferJSONToOtherPhone @"1. Cloud based solutions are not required, recommended for customers to use.\n\n2. Implementation process:\n① Phone A (CDTP Server) generates a virtual Bluetooth peripheral and assigns the value of Object Transfer Service UUID=0x1825 to the Service UUIDs field of the Bluetooth broadcast package.\n② The CDTP Client scans the ServiceUUIDs field of the broadcast package for devices with Object Transfer ServiceUUID=0x1825, and displays it as a CDTP Server list for users to choose from.\n③ Phone B (CDTP Client) selects a server from the CDTP Server list and clicks the Start button to start transmitting JSON data through the OTS protocol.\n④ When JSON data transmission is completed, both phones will prompt for successful sharing.\n⑤ In order to reduce the amount of data transmitted by CDTP, the APP performs zip compression on JSON data.\n⑥ For security reasons, the generated virtual Bluetooth peripherals are encrypted using SMP.\n\n3. Disadvantage 1- Problem of asynchronous JSON data between different mobile phones (currently the app does not specifically address this issue): Mobile A adds a new device and Mobile B is unaware of it. Mobile A needs to share Mesh data with Mobile B again, but without the cloud, sharing cannot be done in a timely manner. If both phone A and phone B have added or deleted devices at the same time, it is also necessary to consider the issue of JSON data merging.\nSolution suggestion 1: The customer stores JSON data in the cloud and pulls the JSON data from the cloud every time. After updating the JSON data locally on their mobile phone, they immediately upload it to the cloud.\nSolution suggestion 2: Customers should add the roles of Mesh administrator and visitor themselves. Visitors only have device control permissions, and administrators have permission to modify Mesh data and device control.\n\n4. Disadvantage 2- Provider address conflict issue (currently the APP does not specifically address this issue): After Phone A shares JSON data with Phone B, Phone B will add a Provisioner to the JSON data, and Phone B needs to send the updated JSON data back to Phone A; If Phone B does not send the updated JSON data back to Phone A, but Phone A shares the JSON data with Phone C again, there will be an address conflict problem caused by Phone B and Phone C using the same Provisioner.\n\n\n\n1.不需要云端的解决方案，推荐客户使用。\n\n2.实现流程：\n①手机A（CDTP Server）生成一个虚拟蓝牙外设并将ObjectTransferServiceUUID=0x1825赋值到蓝牙广播包的ServiceUUIDs字段。\n②手机B（CDTP Client）扫描广播包的ServiceUUIDs字段存在ObjectTransferServiceUUID=0x1825的设备，并显示为一个CDTP Server列表供用户选择。\n③手机B（CDTP Client）从CDTP Server列表里面选择一个Server，并点击Start按钮即可开始通过OTS协议进行JSON数据的传输。\n④当JSON数据传输完成，两个手机都会提示分享成功。\n⑤为了减少CDTP传输的数据量，APP对JSON数据进行zip压缩处理。\n⑥为了安全性，生成的虚拟蓝牙外设使用了SMP加密。\n\n3.缺点1-不同手机之间JSON数据不同步的问题（目前APP没有针对性解决该问题）：手机A新添加设备，手机B是不知道的，需要手机A再次分享Mesh数据给手机B，但是没有云端的情况下，分享不能及时进行。如果手机A和手机B同时添加了设备或者删除了设备，还要需要考虑JSON数据合并的问题。\n解决建议1：客户自己将JSON数据存储到云端，每次都拉取云端的JSON数据，手机本地更新JSON数据后立刻上传云端。\n解决建议2：客户自己新增Mesh管理员与访客的角色，访客只有设备控制权限，管理员有修改Mesh数据和设备控制的权限。\n\n4.缺点2-Provisioner地址冲突问题（目前APP没有针对性解决该问题）：手机A分享JSON数据给手机B后，手机B会添加一个Provisioner到JSON数据里面，手机B需要将更新后的JSON数据回传给手机A；如果手机B未将更新后的JSON数据回传给手机A，手机A却再次分享JSON数据给手机C，就会出现由于手机B和手机C使用相同的Provisioner而造成的地址冲突问题。"
#define TipsMessage_QRCodeAndCDTPTransferJSONToGateway @"1. Cloud based solutions are not required, recommended for customers to use.\n\n2. Implementation process:\n① The Gateway (CDTP Server) assigns Object Transfer Service UUID=0x1825 to the Service UUIDs field of the Bluetooth broadcast packet and starts Bluetooth broadcasting.\n② Phone A (CDTP Client) scans the ServiceUUIDs field of the broadcast package for devices with Object Transfer ServiceUUID=0x1825, and displays it as a CDTP Server list for users to choose from.\n③ Phone A (CDTP Client) selects a server from the CDTP Server list and clicks the Start button to start transmitting JSON data through the OTS protocol.\n④ When JSON data transmission is completed, phone A will prompt for successful sharing.\n⑤ In order to reduce the amount of data transmitted by CDTP, the APP performs zip compression on JSON data.\n⑥ For security reasons, the gateway Bluetooth peripheral uses SMP encryption.\n\n3. Disadvantage 1- Problem of asynchronous JSON data between different phones (currently the app does not specifically address this issue): Gateway has added a new device, and Phone A is unaware of it. Gateway needs to share Mesh data with Phone A again, but without the cloud, sharing cannot be done in a timely manner. If both Gateway and Mobile A add or delete devices at the same time, it is also necessary to consider the issue of JSON data merging.\n4. Disadvantage 2- Provisioner address conflict issue (currently the app does not specifically address this issue): After Phone A shares JSON data with Gateway, Gateway will add a Provisioner to the JSON data, and Gateway needs to send the updated JSON data back to Phone A; If Gateway does not send the updated JSON data back to Phone A, but Phone A shares the JSON data with Phone C again, there will be an address conflict problem caused by Gateway and Phone C using the same Provisioner.\n\n\n\n1.不需要云端的解决方案，推荐客户使用。\n\n2.实现流程：\n①Gateway（CDTP Server）将ObjectTransferServiceUUID=0x1825赋值到蓝牙广播包的ServiceUUIDs字段，并开始蓝牙广播。\n②手机A（CDTP Client）扫描广播包的ServiceUUIDs字段存在ObjectTransferServiceUUID=0x1825的设备，并显示为一个CDTP Server列表供用户选择。\n③手机A（CDTP Client）从CDTP Server列表里面选择一个Server，并点击Start按钮即可开始通过OTS协议进行JSON数据的传输。\n④当JSON数据传输完成，手机A会提示分享成功。\n⑤为了减少CDTP传输的数据量，APP对JSON数据进行zip压缩处理。\n⑥为了安全性，gateway蓝牙外设使用了SMP加密。\n\n3.缺点1-不同手机之间JSON数据不同步的问题（目前APP没有针对性解决该问题）：Gateway新添加设备，手机A是不知道的，需要Gateway再次分享Mesh数据给手机A，但是没有云端的情况下，分享不能及时进行。如果Gateway和手机A同时添加了设备或者删除了设备，还要需要考虑JSON数据合并的问题。\n4.缺点2-Provisioner地址冲突问题（目前APP没有针对性解决该问题）：手机A分享JSON数据给Gateway后，Gateway会添加一个Provisioner到JSON数据里面，Gateway需要将更新后的JSON数据回传给手机A；如果Gateway未将更新后的JSON数据回传给手机A，手机A却再次分享JSON数据给手机C，就会出现由于Gateway和手机C使用相同的Provisioner而造成的地址冲突问题。"
#define TipsMessage_QRCodeAndCloudTransferJSON @"1. For internal testing use, the Telink cloud source code is not open. If existing cloud functions need to be modified, customers need to develop them themselves.\n\n2. Implementation process:\n① Mobile phone A exports JSON data and uploads it to the cloud, which returns a UUID string. Mobile phone A then generates a QR code from the UUID string.\n② Scan the QR code on mobile phone B to obtain the UUID string.\n③ Mobile B uses UUID to request complete JSON data from the cloud.\n\n3. Disadvantage 1- Problem of asynchronous JSON data between different mobile phones (currently the app does not specifically address this issue): Mobile A adds a new device and Mobile B is unaware of it. Mobile A needs to share Mesh data with Mobile B again, but without the cloud, sharing cannot be done in a timely manner. If both phone A and phone B have added or deleted devices at the same time, it is also necessary to consider the issue of JSON data merging.\nSolution suggestion 1: The customer stores JSON data in the cloud and pulls the JSON data from the cloud every time. After updating the JSON data locally on their mobile phone, they immediately upload it to the cloud.\nSolution suggestion 2: Customers should add the roles of Mesh administrator and visitor themselves. Visitors only have device control permissions, and administrators have permission to modify Mesh data and device control.\n\n4. Disadvantage 2- Provisioner address conflict issue (currently the APP does not specifically address this issue): After mobile phone A shares JSON data with mobile phone B, mobile phone B will add a Provisioner to the JSON data, and mobile phone B needs to send the updated JSON data back to mobile phone A; If mobile phone B does not send the updated JSON data back to mobile phone A, but mobile phone A shares the JSON data with mobile phone C again, there will be an address conflict problem caused by mobile phone B and mobile phone C using the same Provisioner.\n\n5. If the customer uses this mode of the Demo app for testing, the following restrictions need to be imposed on the operation behavior:\n① After mobile phone A shares it with mobile phone B, mobile phone B can only perform control operations, and mobile phone B cannot share it anymore. Mobile phone B also does not support operations that modify JSON data such as adding nodes or groups.\n\n6. If the customer wants to solve the two shortcomings described above, they need to implement more comprehensive cloud server functions themselves: such as adding devices, adding groups, and other operations to modify JSON data, and then upload Mesh data to the cloud in real-time, and notify other online mobile devices to update Mesh data in real-time.\n\n\n\n1.内部测试使用，Telink云端源代码不开放，如果需要修改现有云端功能则需要客户自行开发。\n\n2.实现流程：\n①手机A导出JSON数据并上传云端，云端返回一个UUID字符串，手机A再将UUID字符串生成二维码。\n②手机B扫描二维码获取到UUID字符串。\n③手机B使用UUID从云端请求到完整的JSON数据。\n\n3.缺点1-不同手机之间JSON数据不同步的问题（目前APP没有针对性解决该问题）：手机A新添加设备，手机B是不知道的，需要手机A再次分享Mesh数据给手机B，但是没有云端的情况下，分享不能及时进行。如果手机A和手机B同时添加了设备或者删除了设备，还要需要考虑JSON数据合并的问题。\n解决建议1：客户自己将JSON数据存储到云端，每次都拉取云端的JSON数据，手机本地更新JSON数据后立刻上传云端。\n解决建议2：客户自己新增Mesh管理员与访客的角色，访客只有设备控制权限，管理员有修改Mesh数据和设备控制的权限。\n\n4.缺点2-Provisioner地址冲突问题（目前APP没有针对性解决该问题）：手机A分享JSON数据给手机B后，手机B会添加一个Provisioner到JSON数据里面，手机B需要将更新后的JSON数据回传给手机A；如果手机B未将更新后的JSON数据回传给手机A，手机A却再次分享JSON数据给手机C，就会出现由于手机B和手机C使用相同的Provisioner而造成的地址冲突问题。\n\n5.如果客户使用Demo APP的这种模式进行测试，需要对操作行为做以下限制：\n①手机A分享给手机B后，手机B只能进行控制操作，手机B不能再分享出去，手机B也不支持加入节点、添加分组等会修改JSON数据的操作。\n\n6.客户如果要解决上面描述的两个缺点，则需要客户自己实现更加完善的云端服务器功能：如进行添加设备、添加分组等修改JSON数据的操作后将Mesh数据实时上传云端，并实时通知其它在线的手机端更新Mesh数据。"
#define TipsMessage_JSONFile @"1. Internal testing use.\n2. Implementation process:\n① After exporting JSON data from mobile phone A, generate a JSON file with a *. JSON suffix.\n\n② The user then transfers it to mobile phone B through file copying.\n\n3. Disadvantage 1- Problem of asynchronous JSON data between different phones (similar to Disadvantage 1 of `QRCode + Cloud Transfer JSON`, currently the APP does not specifically address this issue): For more details, please refer to the prompt explanation of 'QRCode + Cloud Transfer JSON'.\n\n4. Disadvantage 2- Provisioner address conflict issue (similar to Disadvantage 2 of `QRCode + Cloud Transfer JSON`, currently the APP does not specifically address this issue): For more details, please refer to the prompt description of 'QRCode + Cloud Transfer JSON'.\n\n5. Disadvantage 3- *. JSON file transfer problem with JSON suffix: Users need to transfer JSON files through chat tools such as QQ and WeChat, or hardware such as computer USB drives, which is not user-friendly in the operation process.\n\n6. If the customer needs to use this solution as a landing solution, the following restrictions need to be imposed:\n① After mobile phone A shares it with mobile phone B, mobile phone B can only perform control operations, and mobile phone B cannot share it anymore. Mobile phone B also does not support operations that modify JSON data such as adding nodes or groups.\n\n\n\n1.内部测试使用。\n\n2.实现流程：\n①手机A导出JSON数据后生成一份*.json为后缀的JSON文件。\n②用户再通过文件拷贝的方式传输到手机B。\n\n3.缺点1-不同手机之间JSON数据不同步的问题（和模式1的缺点1相同，目前APP没有针对性解决该问题）：详情请看`QRCode+Cloud Transfer JSON`的提示说明。\n\n4.缺点2-Provisioner地址冲突问题（和模式1的缺点2相同，目前APP没有针对性解决该问题）：详情请看`QRCode+Cloud Transfer JSON`的提示说明。\n\n5.缺点3-*.json为后缀的JSON文件的传输问题：需要用户通过QQ、微信等聊天工具或者电脑U盘等硬件传输JSON文件，操作流程不友好。\n\n6.如果客户需要使用这种方案作为落地解决方案，需要做以下限制：\n①手机A分享给手机B后，手机B只能进行控制操作，手机B不能再分享出去，手机B也不支持加入节点、添加分组等会修改JSON数据的操作。"

#define kDefaultAlertTitle @"Warning"
#define kDefaultAlertOK @"OK"
#define kDefaultAlertCancel @"Cancel"


//分享使用方式(是否使用蓝牙点对点传输，YES为二维码加蓝牙点对点，NO为存二维码)
#define kShareWithBluetoothPointToPoint (YES)
//setting界面显示
#define kshowTest        (NO)
//Demo v4.1.0.0新增Provision类型的存储key
#define kProvisionMode  @"kProvisionMode"

#define kKeyBindType  @"kKeyBindType"
#define kDLEType  @"kDLEType"
#define kSubscribeLevelServiceModelID  @"kSubscribeLevelServiceModelID"
#define kAddStaticOOBDeviceByNoOOBEnable  @"kAddStaticOOBDeviceByNoOOBEnable"
#define kDistributorAddress  @"kDistributorAddress"
#define kDistributorPolicy  @"kDistributorPolicy"
#define kDistributorBinString  @"kDistributorBinString"
#define kUpdateNodeAddresses  @"kUpdateNodeAddresses"
#define kRootCertificateName  @"kRootCertificateName"
#define kFilter  @"kFilter"
/// 记录Demo端的Mesh列表的key
#define kCacheMeshListKey  @"kCacheMeshListKey"
/// 记录Demo端的当前正在使用的Mesh的MeshUUID
#define kCacheCurrentMeshUUIDKey  @"kCacheCurrentMeshUUIDKey"
/// demo v4.1.0.0新增配置项：导入Mesh后自动切换Mesh还是弹框提示用户选择是否切换Mesh。
#define kImportCompleteAction   @"kImportCompleteAction"
/// demo v4.1.0.1新增配置项：node节点的排序方式，默认是短地址升序。
#define kSortTypeOfNodeList     @"kSortTypeOfNodeList"
/// demo v4.1.0.2新增配置项：缓存Telink默认使用的服务器IP。
#define kDefaultBaseURL     @"kDefaultBaseURL"
//获取导航栏+状态栏的高度
#define kGetRectNavAndStatusHeight  (self.navigationController.navigationBar.frame.size.height+[[UIApplication sharedApplication] statusBarFrame].size.height)

#define SCANSPACEOFFSET 0.15f

#define SCREENBOUNDS [UIScreen mainScreen].bounds
#define SCREEN_WIDTH CGRectGetWidth([UIScreen mainScreen].bounds)
#define SCREEN_HEIGHT CGRectGetHeight([UIScreen mainScreen].bounds)

//默认指令的重试次数
#define kDefaultRetryCount  (2)

#define kPublishIntervalOfDemo  (20)

#endif /* DemoConst_h */
