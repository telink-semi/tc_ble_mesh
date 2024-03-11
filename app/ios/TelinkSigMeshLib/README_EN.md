# TelinkSigMeshLib

version: v4.1.0.0

APP store download url: https://apps.apple.com/cn/app/telinksigmesh/id1536722792

## About

The TelinkSigMeshLib library allows to provision、keybind and send messages to Bluetooth Mesh devices. 

> Bluetooth Mesh specification may be found here: https://www.bluetooth.com/specifications/mesh-specifications/

The library is compatible with version 1.1.0 of the Bluetooth Mesh Profile Specification.

## Sample app

The sample application demonstrates how to use the library.

## Supported features

1. GATT provisioning with no oob devices and static oob devices that is available in Bluetooth Mesh Protocol Specification 1.0.0.
2. Remote provisioning with no oob devices and static oob devices that is available in Bluetooth Mesh Protocol Specification 1.1.0.
3. Fast provisioning with no oob devices that avilable the telink private protocol.
4. Normal keybind:connect node -> read servers -> open notify -> set filter -> get composition -> appkey add -> bind model to appkey.
5. Fast keybind(Default Bound, node need avilable the telink private protocol):connect node -> read servers -> open notify -> set filter -> appkey add.
6. Control node with unicastAddress and groupAddress:onoff，light，temperature，level，HSL.
7. Setting and clearing publication to a Model.
8. Setting and removing subscriptions to a Model.
9. Add node to group and delete node from group.
10. Add LPN node.
11. GATT OTA: nodes need avilable the telink private protocol.
12. Mesh OTA: nodes need avilable the Mesh Device Firmware Update Protocol Specification. It is available in Bluetooth Mesh Protocol Specification 1.1.0.
13. Scheduler models.
14. Scene models.
15. Proxy Filter.

## Requirements

* Xcode 11.0 or newer.
* An iOS 11.0 or newer device with BLE capabilities.

## How to run the Sample app `TelinkSigMesh`？

* 1.Due to the project using pod to import third-party libraries, developers need to use the command line to enter the folders `telink_sig_mesh_sdk/app/ios/TelinkSigMeshLib/SigMeshOCDemo` and `telink_sig_mesh_sdk/app/ios/TelinkSigMeshLib/TelinkSigMeshLib` respectively, and then run the command `pod install` to download and configure the pod third-party library.
* 2.Double click to open the project icon for pod generation `telink_sig_mesh_sdk/app/ios/TelinkSigMeshLib/SigMeshOCDemo/SigMeshOCDemo.xcworkspace`.
* 3.Description of various Targets for project `SigMeshOCDemo`:
* * `SigMeshOCDemo` - The offline version of TelinkSigMesh App, with Mesh data stored locally on the phone, has been launched in the App Store.
* * `SigMeshCloud` - The cloud version of TelinkSigMesh App, stores Mesh data on Telink's cloud server.
* * `SigLoopAddDemo` - Loop test the testing app for adding and removing Mesh devices, displaying the average time spent on each stage of adding and the success rate of adding.

## Setting up the library

* 1.Due to the project using pod to import third-party libraries, developers need to use the command line to enter the folders `telink_sig_mesh_sdk/app/ios/TelinkSigMeshLib/SigMeshOCDemo` and `telink_sig_mesh_sdk/app/ios/TelinkSigMeshLib/TelinkSigMeshLib` respectively, and then run the command `pod install` to download and configure the pod third-party library.
* 2.copy folder `TelinkSigMeshLib` to your project, then `Link Binary With Libraries` the `TelinkSigMeshLib.framework`.

## Open API

* 1.Start SDK.

```Object-C
[SDKLibCommand startMeshSDK];
```
* 2.Set SDK log level.

```Object-C
[SigLogger.share setSDKLogLevel:SigLogLevelDebug];
```
* 3.GATT add nodes API1 (auto add)

```Object-C
- (void)startAddDeviceWithNextAddress:(UInt16)address networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex appkeyModel:(SigAppkeyModel *)appkeyModel unicastAddress:(UInt16)unicastAddress uuid:(nullable NSData *)uuid keyBindType:(KeyBindTpye)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData isAutoAddNextDevice:(BOOL)isAuto provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail finish:(AddDeviceFinishCallBack)finish;
```
* 4.GATT add node API2 (single add)

```Object-C
- (void)startAddDeviceWithNextAddress:(UInt16)address networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex appkeyModel:(SigAppkeyModel *)appkeyModel peripheral:(CBPeripheral *)peripheral provisionType:(ProvisionTpye)provisionType staticOOBData:(nullable NSData *)staticOOBData keyBindType:(KeyBindTpye)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail;
```
* 5.GATT add node API3: simple api (single add)

```Object-C
- (void)startAddDeviceWithSigAddConfigModel:(SigAddConfigModel *)configModel provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail;
```
* 6.Add nodes by fast provision

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
* 8.Start connect mesh network(this api will retry connect action every 5 seconds unless connect success or call `stopMeshConnectWithComplete:`)

```Object-C
- (void)startMeshConnectWithComplete:(nullable startMeshConnectResultBlock)complete;
```
* 9.Stop connect mesh network

```Object-C
- (void)stopMeshConnectWithComplete:(nullable stopMeshConnectResultBlock)complete;
```
* 10.Control node: OnOffSet(more control api in `SDKLibCommand`)

```Object-C
+ (SigMessageHandle *)genericOnOffSetWithDestination:(UInt16)destination isOn:(BOOL)isOn retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
```
* 11.Send ini command API1(api reference c-lib SDK, iniData like "a3ff000000000200ffffc21102c4020100".)

```Object-C
+ (nullable NSError *)sendOpINIData:(NSData *)iniData successCallback:(responseAllMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
```
* 12.Send ini command API2

```Object-C
+ (nullable NSError *)sendIniCommandModel:(IniCommandModel *)model successCallback:(responseAllMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
```
* 13.Message's live from TelinkSigMeshLib(developer need set `SigMeshLib.share.delegateForDeveloper = self;`)

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

* 14.How to configure whitelist addresses
TelinkSigMeshLib configures the proxy node as `SigProxyFilerType_whitelist` mode by default after connecting the BLE process complete, the default whitelist address is the provisioner address and 0xFFFF. If the customer needs to add some new whitelist addresses, he can call the following interface to add same new whitelist addresses after the `startMeshConnectWithComplete` callback succeeds.
```Object-C
+ (void)addAddressesToFilterWithAddresses:(NSArray <NSNumber *>*)addresses successCallback:(responseFilterStatusMessageBlock)successCallback failCallback:(resultBlock)failCallback;
```

SigMesh iOS SDK of Telink Semiconductor (Shanghai) Co., Ltd.
