# TelinkSigMeshLibExtensions

version: v3.2.2

## About

The TelinkSigMeshLib library allows to provision、keybind and send messages to Bluetooth Mesh devices. 

> Bluetooth Mesh specification may be found here: https://www.bluetooth.com/specifications/mesh-specifications/

The library is compatible with version 1.0.1 of the Bluetooth Mesh Profile Specification.

## Sample app

The sample application demonstrates how to use the library.

## Supported features

1. GATT provisioning with no oob devices and static oob devices that available in Bluetooth Mesh Profile 1.0.1.
2. Remote provisioning with no oob devices and static oob devices.
3. Fast provisioning with no oob devices that avilable the telink private protocol.
4. Normal keybind:connect node -> read servers -> open notify -> set filter -> get composition -> appkey add ->
bind model to appkey.
5. Fast keybind(Default Bound, node need avilable the telink private protocol):connect node -> read servers -> open notify -> set filter -> appkey add.
6. Control node with unicastAddress and groupAddress:onoff，light，temperature，level，HSL.
7. Setting and clearing publication to a Model.
8. Setting and removing subscriptions to a Model.
9. Add node to group and delete node from group.
10. Add LPN node.
11. GATT OTA: nodes need avilable the telink private protocol.
12. Mesh OTA: nodes need avilable the Mesh Firmware Update Proposal.
13. Scheduler models.
14. Scene models.
15. Proxy Filter.

## Requirements

* Xcode 10 or newer.
* An iOS 9.0 or newer device with BLE capabilities.

## Setting up the library

1. Refer to sample application, copy SDK source folder `TelinkSigMeshLib`(path is app app/ios/TelinkSigMeshLib/TelinkSigMeshLib) and extension folder `Extensions`(need to apply this folder from Telink's FAE) to the same level directory as app folder `SigMeshOCDemo`.
2. Open the `meshOTA` and `remote provision` functions to compile the macro. The steps are as follows: open the code file `TelinkSigMeshLib.h`(path: TelinkSigMeshLib/TelinkSigMeshLib/TelinkSigMeshLib.h), and uncomment the comment code `//#define kExist` in line 47.
3. Pull the source project `TelinkSigMeshLib.xcodeproj` to the customer's own app, and add the binary library link of the TelinkSigMeshLibExtensions to the customer's own app. The steps are as follows: TelinkSigMeshLibExtensions->Build Phases->Link Binary With Libraries-> click "+" - > select "TelinkSigMeshLibExtensions.framework" - > click "Add" to complete the integration.

## 集成扩展库TelinkSigMeshLibExtensions步骤

1. 参考Demo做法，将SDK源码文件夹`TelinkSigMeshLib`(路径为app/ios/TelinkSigMeshLib/TelinkSigMeshLib)和扩展功能文件夹`Extensions`(需要从Telink的FAE手上申请获取该文件夹)拷贝到与APP文件夹`SigMeshOCDemo`同一级的目录。
2. 打开meshOTA和remote provision功能编译宏。步骤为：打开代码文件`TelinkSigMeshLib.h`(路径为TelinkSigMeshLib/TelinkSigMeshLib/TelinkSigMeshLib.h)，取消注释第47行的注释代码`//#define kExist`。
3. 拉取库源码工程`TelinkSigMeshLib.xcodeproj`到客户自己的APP中，在客户自己的APP中添加TelinkSigMeshLibExtensions库的二进制库链接。步骤为：TelinkSigMeshLibExtensions->Build Phases->Link Binary With Libraries->点击“+”->选中“TelinkSigMeshLibExtensions.framework”->点击“Add”即可完成集成。

## Open API

* 1. Start SDK.

```Object-C
[SDKLibCommand startMeshSDK];
```
* 2. Set SDK log level.

```Object-C
[SigLogger.share setSDKLogLevel:SigLogLevelDebug];
```
* 3. GATT add nodes API1 (auto add)

```Object-C
- (void)startAddDeviceWithNextAddress:(UInt16)address networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex appkeyModel:(SigAppkeyModel *)appkeyModel unicastAddress:(UInt16)unicastAddress uuid:(nullable NSData *)uuid keyBindType:(KeyBindTpye)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData isAutoAddNextDevice:(BOOL)isAuto provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail finish:(AddDeviceFinishCallBack)finish;
```
* 4. GATT add node API2 (single add)

```Object-C
- (void)startAddDeviceWithNextAddress:(UInt16)address networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex appkeyModel:(SigAppkeyModel *)appkeyModel peripheral:(CBPeripheral *)peripheral provisionType:(ProvisionTpye)provisionType staticOOBData:(nullable NSData *)staticOOBData keyBindType:(KeyBindTpye)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail;
```
* 5. GATT add node API3: simple api (single add)

```Object-C
- (void)startAddDeviceWithSigAddConfigModel:(SigAddConfigModel *)configModel provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail;
```
* 6. Add nodes by fast provision

```Object-C
- (void)startFastProvisionWithProvisionAddress:(UInt16)provisionAddress productId:(UInt16)productId compositionData:(NSData *)compositionData currentConnectedNodeIsUnprovisioned:(BOOL)unprovisioned addSingleDeviceSuccessCallback:(AddSingleDeviceSuccessOfFastProvisionCallBack)singleSuccess finish:(ErrorBlock)finish;
```
* 7. Add nodes by remote provision

>7.1. scan remote provision node
```Object-C
- (void)startRemoteProvisionScanWithReportCallback:(remoteProvisioningScanReportCallBack)reportCallback resultCallback:(resultBlock)resultCallback;
```
>7.2. provision node
```Object-C
- (void)remoteProvisionWithNextProvisionAddress:(UInt16)provisionAddress reportNodeAddress:(UInt16)reportNodeAddress reportNodeUUID:(NSData *)reportNodeUUID networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex provisionType:(ProvisionTpye)provisionType staticOOBData:(nullable NSData *)staticOOBData provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess fail:(ErrorBlock)fail;
```
>7.3. keybind node
```Object-C
- (void)keyBind:(UInt16)address appkeyModel:(SigAppkeyModel *)appkeyModel keyBindType:(KeyBindTpye)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess fail:(ErrorBlock)fail;
```

* 8. Start connect mesh network(this api will retry connect action every 10 seconds unless connect success or call `stopMeshConnectWithComplete:`)

```Object-C
- (void)startMeshConnectWithComplete:(nullable startMeshConnectResultBlock)complete;
```
* 9. Stop connect mesh network

```Object-C
- (void)stopMeshConnectWithComplete:(nullable stopMeshConnectResultBlock)complete;
```
* 10. Control node: OnOffSet(more control api in `SDKLibCommand`)

```Object-C
+ (SigMessageHandle *)genericOnOffSetWithDestination:(UInt16)destination isOn:(BOOL)isOn retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
```
* 11. Send ini command API1(api reference c-lib SDK, iniData like "a3ff000000000200ffffc21102c4020100".)

```Object-C
+ (nullable NSError *)sendOpINIData:(NSData *)iniData successCallback:(responseAllMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
```
* 12. Send ini command API2

```Object-C
+ (nullable NSError *)sendIniCommandModel:(IniCommandModel *)model successCallback:(responseAllMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
```
* 13. Message's live from TelinkSigMeshLib(developer need set `SigMeshLib.share.delegateForDeveloper = self;`)

>13.1. APP did Receive Message from node.
```Object-C
- (void)didReceiveMessage:(SigMeshMessage *)message sentFromSource:(UInt16)source toDestination:(UInt16)destination;
```
>13.2. APP did Send Message to node.
```Object-C
- (void)didSendMessage:(SigMeshMessage *)message fromLocalElement:(SigElementModel *)localElement toDestination:(UInt16)destination;
```
>13.3. APP failed To Send Message.
```Object-C
- (void)failedToSendMessage:(SigMeshMessage *)message fromLocalElement:(SigElementModel *)localElement toDestination:(UInt16)destination error:(NSError *)error;
```
>13.4. APP did Receive SigProxyConfiguration Message from node.
```Object-C
- (void)didReceiveSigProxyConfigurationMessage:(SigProxyConfigurationMessage *)message sentFromSource:(UInt16)source toDestination:(UInt16)destination;
```

SigMesh iOS SDK of Telink Semiconductor (Shanghai) Co., Ltd.
