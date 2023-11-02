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
#define CellIdentifiers_AddDeviceCellID  @"AddDeviceCell"
#define CellIdentifiers_ProxyFilterCellID  @"ProxyFilterCell"
#define CellIdentifiers_FilterAddressCellID  @"FilterAddressCell"


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
#define ViewControllerIdentifiers_RemoteAddVCID   @"RemoteAddVC"
#define ViewControllerIdentifiers_FastProvisionAddViewControllerID  @"FastProvisionAddVC"
#define ViewControllerIdentifiers_AddDeviceVCID   @"AddDeviceVC"
#define ViewControllerIdentifiers_AutoAddDeviceVCID @"AutoAddDeviceVC"

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
#define ViewControllerIdentifiers_ForwardingTableVCID  @"ForwardingTableVC"
#define ViewControllerIdentifiers_DeviceSubscriptionListViewControllerID  @"DeviceSubscriptionListViewController"
#define ViewControllerIdentifiers_SensorVCID  @"SensorVC"
#define ViewControllerIdentifiers_ShareTipsVCID @"ShareTipsVC"

#define kCommandInterval    (0.32)

//TimeOut
#define TimeOut_KickoutDelayResponseTime    (1.5)
#define TimeOut_KickoutConnectedDelayResponseTime   (5.0)

//Tip
#define Tip_CommandBusy @"Sending command busy···"
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

#define TipsTitle_QRCodeAndBLETransferJSON @"QRCode + BLE Transfer JSON"
#define TipsTitle_QRCodeAndCloudTransferJSON @"QRCode + Cloud Transfer JSON"
#define TipsTitle_JSONFile @"JSON File"
//#define TipsMessage_QRCodeAndBLETransferJSON @"1.不需要云端的解决方案，推荐用户使用。\n\n2.实现原理：\n①分享端A随机生成一个16字节长度的UUID并转换为二维码，接收端B扫描二维码。\n②接收端B生成一个虚拟蓝牙外设并将这个UUID赋值到蓝牙广播包的ServiceUUIDs字段。\n③分享端A(Master)扫描并连接接收端B(Slave)的虚拟蓝牙外设，接收端B返回本身的唯一标识UUID，分享端A根据接收端B的UUID生成一份包含该UUID的Provisioner的临时Mesh数据。\n④分享端A通过WriteWithoutResponse的方式将临时Mesh数据传输给接收端B，如果传输成功，这两个手机都切换到这份临时Mesh数据去。\n⑤为了减少BLE传输的数据量，APP对JSON数据进行zip压缩处理。\n⑥为了安全性，生成的虚拟蓝牙外设使用了SMP加密。\n\n3.缺点1(已经由APP增加限制规则来解决)-由于不同手机之间JSON数据不同步的问题：分享端A新添加设备，接收端B是不知道的，需要分享端A再次分享Mesh数据给接收端B，但是没有云端的情况下，分享不能及时进行。如果手机A和手机B同时添加了设备或者删除了设备，还要需要考虑JSON数据合并的问题。所以当前这种方案Demo APP做以下限制：\n①手机A分享给手机B后，手机B作为Mesh的访客的角色。手机B只能进行控制操作，手机B不能再分享出去，手机B也不支持加入节点、添加分组等会修改JSON数据的操作。\n②手机A为Mesh的管理员的角色，可以进行任何操作。"
//#define TipsMessage_QRCodeAndCloudTransferJSON @"1.内部测试使用。\n\n2.实现原理：分享端导出JSON数据并上传云端，云端返回一个UUID字符串，分享端再将UUID字符串生成二维码供接收端进行扫描，接收端获取到这个UUID后再通过到云端请求到完整的JSON数据。\n\n3.缺点1-不同手机之间JSON数据不同步的问题（和模式1的缺点1相同，目前APP没有针对性解决该问题）：详情请看`QRCode+BLE Transfer JSON`的提示说明。\n\n4.缺点2-Provisioner地址冲突问题（目前APP没有针对性解决该问题）：手机A分享JSON数据给手机B后，手机B会添加一个Provisioner到JSON数据里面，手机B需要将更新后的JSON数据回传给手机A；如果手机B未将更新后的JSON数据回传给手机A，手机A却再次分享JSON数据给手机C，就会出现由于手机B和手机C使用相同的Provisioner而造成的地址冲突问题。\n\n5.如果客户需要使用这种方案作为落地解决方案，需要做以下限制：\n①手机A分享给手机B后，手机B只能进行控制操作，手机B不能再分享出去，手机B也不支持加入节点、添加分组等会修改JSON数据的操作。"
//#define TipsMessage_JSONFile @"1.内部测试使用。\n\n2.实现原理：分享端导出JSON数据后生成一份*.json为后缀的JSON文件，用户再通过文件拷贝的方式传输到接收端。\n\n3.缺点1-不同手机之间JSON数据不同步的问题（和模式1的缺点1相同，目前APP没有针对性解决该问题）：详情请看`QRCode+BLE Transfer JSON`的提示说明。\n\n4.缺点2-Provisioner地址冲突问题（和模式2的缺点2相同，目前APP没有针对性解决该问题）：详情请看`QRCode+Cloud Transfer JSON`的提示说明。\n\n5.缺点3-*.json为后缀的JSON文件的传输问题：需要用户通过QQ、微信等聊天工具或者电脑U盘等硬件传输JSON文件，操作流程不友好。\n\n6.如果客户需要使用这种方案作为落地解决方案，需要做以下限制：\n①手机A分享给手机B后，手机B只能进行控制操作，手机B不能再分享出去，手机B也不支持加入节点、添加分组等会修改JSON数据的操作。"
#define TipsMessage_QRCodeAndBLETransferJSON @"1. Cloud based solutions are not required, recommended for customers to use.\n\n2. Implementation process:\n① Mobile phone A (network manager) randomly generates a 16 byte UUID and converts it into a QR code, while mobile phone B (receiving JSON file) scans the QR code.\n② Mobile phone B generates a virtual Bluetooth peripheral and assigns this UUID to the ServiceUUIDs field of the Bluetooth broadcast package.\n③ Mobile phone A (BLE Master role) scans and connects to the virtual Bluetooth peripheral of mobile phone B (BLE Slave role). Mobile phone B returns its unique identifier UUID, and mobile phone A generates a temporary Mesh data containing the Provisioner of mobile phone B based on its UUID.\n④ Mobile phone A transfers temporary Mesh data to mobile phone B through WriteWithoutResponse. If the transfer is successful, both phones will switch to this temporary Mesh data.\n⑤ In order to reduce the amount of data transmitted by BLE, the APP performs zip compression on JSON data.\n⑥ For security reasons, the generated virtual Bluetooth peripherals are encrypted using SMP.\n\n3. Disadvantage 1 (already resolved by adding restriction rules in the app) - Due to the issue of asynchronous JSON data between different phones: mobile phone A adds a new device, and mobile phone B is unaware of it. Mobile phone A needs to share Mesh data with mobile phone B again, but without the cloud, sharing cannot be done in a timely manner. If both mobile phone A and mobile phone B have added or deleted devices at the same time, it is also necessary to consider the issue of JSON data merging. So the current demo app has the following limitations:\n① After mobile phone A shares mesh with mobile phone B, mobile phone B plays the role of a visitor to Mesh. Mobile B can only perform control operations, and cannot share it anymore. Mobile B also does not support operations that modify JSON data such as adding nodes or groups.\n② Mobile phone A plays the role of Mesh administrator and can perform any operation.\n\n4. Handling process after Mobile phone A is lost:\n① Demo APP processing process: manually reset all nodes and rebuild the network.\n② If the customer can develop it themselves, optimization can be carried out, such as changing mobile phone B from a visitor role to a network manager role, but it is necessary to pay attention to whether the database information is complete, and it is best to ensure that only one mobile phone is changed to a network manager role.\n\n\n\n1.不需要云端的解决方案，推荐客户使用。\n\n2.实现流程：\n①手机A（网络管理者）随机生成一个16字节长度的UUID并转换为二维码，手机B（接收JSON文件端）扫描二维码。\n②手机B生成一个虚拟蓝牙外设并将这个UUID赋值到蓝牙广播包的ServiceUUIDs字段。\n③手机A(BLE Master角色)扫描并连接手机B(BLE Slave角色)的虚拟蓝牙外设，手机B返回本身的唯一标识UUID，手机A根据手机B的UUID生成一份包含该UUID的Provisioner的临时Mesh数据。\n④手机A通过WriteWithoutResponse的方式将临时Mesh数据传输给手机B，如果传输成功，这两个手机都切换到这份临时Mesh数据去。\n⑤为了减少BLE传输的数据量，APP对JSON数据进行zip压缩处理。\n⑥为了安全性，生成的虚拟蓝牙外设使用了SMP加密。\n\n3.缺点1(已经由APP增加限制规则来解决)-由于不同手机之间JSON数据不同步的问题：手机A新添加设备，手机B是不知道的，需要手机A再次分享Mesh数据给手机B，但是没有云端的情况下，分享不能及时进行。如果手机A和手机B同时添加了设备或者删除了设备，还要需要考虑JSON数据合并的问题。所以当前这种方案Demo APP做以下限制：\n①手机A分享给手机B后，手机B作为Mesh的访客的角色。手机B只能进行控制操作，手机B不能再分享出去，手机B也不支持加入节点、添加分组等会修改JSON数据的操作。\n②手机A为Mesh的管理员的角色，可以进行任何操作。\n\n4.手机A丢失后的处理流程：\n①Demo APP处理流程：手动复位所有节点，重新组建网络。\n②客户如果可以自己开发，可以进行优化，比如：将手机B由访客角色变更为网络管理者角色，但要注意数据库信息是否完整，最好保证只变更一个手机为网络管理者角色。"
#define TipsMessage_QRCodeAndCloudTransferJSON @"1. For internal testing use, the Telink cloud source code is not open. If existing cloud functions need to be modified, customers need to develop them themselves.\n\n2. Implementation process:\n① Mobile phone A exports JSON data and uploads it to the cloud, which returns a UUID string. Mobile phone A then generates a QR code from the UUID string.\n② Scan the QR code on mobile phone B to obtain the UUID string.\n③ Mobile B uses UUID to request complete JSON data from the cloud.\n\n3. Disadvantage 1- Problem of asynchronous JSON data between different phones (similar to Disadvantage 1 of `QRCode + BLE Transfer JSON`, currently the APP does not specifically address this issue): For more details, please refer to the prompt explanation of 'QRCode + BLE Transfer JSON'.\n\n4. Disadvantage 2- Provisioner address conflict issue (currently the APP does not specifically address this issue): After mobile phone A shares JSON data with mobile phone B, mobile phone B will add a Provisioner to the JSON data, and mobile phone B needs to send the updated JSON data back to mobile phone A; If mobile phone B does not send the updated JSON data back to mobile phone A, but mobile phone A shares the JSON data with mobile phone C again, there will be an address conflict problem caused by mobile phone B and mobile phone C using the same Provisioner.\n\n5. If the customer uses this mode of the Demo app for testing, the following restrictions need to be imposed on the operation behavior:\n① After mobile phone A shares it with mobile phone B, mobile phone B can only perform control operations, and mobile phone B cannot share it anymore. Mobile phone B also does not support operations that modify JSON data such as adding nodes or groups.\n\n6. If the customer wants to solve the two shortcomings described above, they need to implement more comprehensive cloud server functions themselves: such as adding devices, adding groups, and other operations to modify JSON data, and then upload Mesh data to the cloud in real-time, and notify other online mobile devices to update Mesh data in real-time.\n\n\n\n1.内部测试使用，Telink云端源代码不开放，如果需要修改现有云端功能则需要客户自行开发。\n\n2.实现流程：\n①手机A导出JSON数据并上传云端，云端返回一个UUID字符串，手机A再将UUID字符串生成二维码。\n②手机B扫描二维码获取到UUID字符串。\n③手机B使用UUID从云端请求到完整的JSON数据。\n\n3.缺点1-不同手机之间JSON数据不同步的问题（和模式1的缺点1相同，目前APP没有针对性解决该问题）：详情请看`QRCode+BLE Transfer JSON`的提示说明。\n\n4.缺点2-Provisioner地址冲突问题（目前APP没有针对性解决该问题）：手机A分享JSON数据给手机B后，手机B会添加一个Provisioner到JSON数据里面，手机B需要将更新后的JSON数据回传给手机A；如果手机B未将更新后的JSON数据回传给手机A，手机A却再次分享JSON数据给手机C，就会出现由于手机B和手机C使用相同的Provisioner而造成的地址冲突问题。\n\n5.如果客户使用Demo APP的这种模式进行测试，需要对操作行为做以下限制：\n①手机A分享给手机B后，手机B只能进行控制操作，手机B不能再分享出去，手机B也不支持加入节点、添加分组等会修改JSON数据的操作。\n\n6.客户如果要解决上面描述的两个缺点，则需要客户自己实现更加完善的云端服务器功能：如进行添加设备、添加分组等修改JSON数据的操作后将Mesh数据实时上传云端，并实时通知其它在线的手机端更新Mesh数据。"
#define TipsMessage_JSONFile @"1. Internal testing use.\n2. Implementation process:\n① After exporting JSON data from mobile phone A, generate a JSON file with a *. JSON suffix.\n\n② The user then transfers it to mobile phone B through file copying.\n\n3. Disadvantage 1- Problem of asynchronous JSON data between different phones (similar to Disadvantage 1 of `QRCode + BLE Transfer JSON`, currently the APP does not specifically address this issue): For more details, please refer to the prompt explanation of 'QRCode + BLE Transfer JSON'.\n\n4. Disadvantage 2- Provisioner address conflict issue (similar to Disadvantage 2 of `QRCode + Cloud Transfer JSON`, currently the APP does not specifically address this issue): For more details, please refer to the prompt description of 'QRCode + Cloud Transfer JSON'.\n\n5. Disadvantage 3- *. JSON file transfer problem with JSON suffix: Users need to transfer JSON files through chat tools such as QQ and WeChat, or hardware such as computer USB drives, which is not user-friendly in the operation process.\n\n6. If the customer needs to use this solution as a landing solution, the following restrictions need to be imposed:\n① After mobile phone A shares it with mobile phone B, mobile phone B can only perform control operations, and mobile phone B cannot share it anymore. Mobile phone B also does not support operations that modify JSON data such as adding nodes or groups.\n\n\n\n1.内部测试使用。\n\n2.实现流程：\n①手机A导出JSON数据后生成一份*.json为后缀的JSON文件。\n②用户再通过文件拷贝的方式传输到手机B。\n\n3.缺点1-不同手机之间JSON数据不同步的问题（和模式1的缺点1相同，目前APP没有针对性解决该问题）：详情请看`QRCode+BLE Transfer JSON`的提示说明。\n\n4.缺点2-Provisioner地址冲突问题（和模式2的缺点2相同，目前APP没有针对性解决该问题）：详情请看`QRCode+Cloud Transfer JSON`的提示说明。\n\n5.缺点3-*.json为后缀的JSON文件的传输问题：需要用户通过QQ、微信等聊天工具或者电脑U盘等硬件传输JSON文件，操作流程不友好。\n\n6.如果客户需要使用这种方案作为落地解决方案，需要做以下限制：\n①手机A分享给手机B后，手机B只能进行控制操作，手机B不能再分享出去，手机B也不支持加入节点、添加分组等会修改JSON数据的操作。"
/// Tips Title of AutoProvision
#define kAutoProvisionTitle  @"Auto Provision"
/// Tips message of AutoProvision
#define kAutoProvisionMessage  @"Enabled for scanning, provisioning and key-binding one by one; Disabled for selectable networking."
/// Tips Title of Remote Provision
#define kRemoteProvisionTitle  @"Remote Provision"
/// Tips message of Remote Provision
#define kRemoteProvisionMessage  @"If remote provision enabled, provision process will be:\n\n1. Scan for an unprovisioned device, connect and provision;\n\n2. Keep the connection state, send scan command to the device;\n\n3. Some remote devices info will be upload;\n\n4. Check and do provision one by one."
/// Tips Title of Fast Provision
#define kFastProvisionTitle  @"Fast Provision"
/// Tips message of Fast Provision
#define kFastProvisionMessage  @"Telink private provision profile, scan and setting device by direct connected mesh devcie."
/// Tips Title of Default Bound
#define kDefaultBoundTitle  @"Private Mode(Default Bound)"
/// Tips message of Default Bound
#define kDefaultBoundMessage  @"Enable for default bound when key-binding."
/// Tips Title of Online Status
#define kOnlineStatusTitle  @"Online Status"
/// Tips message of Online Status
#define kOnlineStatusMessage  @"Telink private profile for get the status of all nodes, including on、 off and offline status."
/// Tips Title of subscribe level server modelID
#define kSubscribeLevelServerModelIDTitle  @"Enable Subscribe Level Server ModelID"
/// Tips message of subscribe level server modelID
#define kSubscribeLevelServerModelIDMessage  @"The flag added in demo v4.1.0.0 and later indicates whether the Service Level ModelID (0x1003) is also subscribed when subscribing to the group address.\nIf the customer has a Mesh remote control product and the Mesh remote control product has a Level control function, this function needs to be enabled on the app side. The default value is off."
/// Tips Title of Use No-OOB Automatically
#define kUseNoOOBAutomaticallyTitle  @"Use No-OOB Automatically"
/// Tips message of Use No-OOB Automatically
#define kUseNoOOBAutomaticallyMessage  @"Use No-OOB if device UUID not found when device is static-OOB supported."
/// Tips Title of Directed Security
#define kDirectedSecurityTitle  @"Directed Security"
/// Tips message of Directed Security
#define kDirectedSecurityMessage  @"A new configuration item is added in demo v3.3.3.6 and later to mark whether the data sent by the APP is encrypted using The directed security material. The default value is NO."


//分享使用方式(是否使用蓝牙点对点传输，YES为二维码加蓝牙点对点，NO为存二维码)
#define kShareWithBluetoothPointToPoint (YES)
//setting界面显示
#define kShowScenes (YES)
#define kshowLog        (YES)
#define kshowShare    (YES)
#define kshowMeshInfo    (YES)
#define kshowMeshSettings    (YES)

#define kshowTest        (NO)

#define kKeyBindType  @"kKeyBindType"
#define kRemoteAddType  @"kRemoteAddType"
#define kFastAddType  @"kFastAddType"
#define kDLEType  @"kDLEType"
#define kGetOnlineStatusType  @"kGetOnlineStatusType"
#define kSubscribeLevelServiceModelID  @"kSubscribeLevelServiceModelID"
#define kAddStaticOOBDeviceByNoOOBEnable  @"kAddStaticOOBDeviceByNoOOBEnable"
#define kDistributorAddress  @"kDistributorAddress"
#define kDistributorPolicy  @"kDistributorPolicy"
#define kUpdateNodeAddresses  @"kUpdateNodeAddresses"
#define kRootCertificateName  @"kRootCertificateName"
#define kDirectedSecurityEnable  @"kDirectedSecurityEnable"
#define kFilter  @"kFilter"
/// 记录是否进行自动加灯
#define kAutoProvision  @"kAutoProvision"

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
