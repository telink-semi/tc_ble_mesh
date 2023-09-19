# TelinkSigMeshLib

version: v3.3.4

## About

The TelinkSigMeshLib library allows to provision、keybind and send messages to Bluetooth Mesh devices. 

> Bluetooth Mesh specification may be found here: https://www.bluetooth.com/specifications/mesh-specifications/

The library is compatible with version 1.0.1 of the Bluetooth Mesh Profile Specification.

## Sample app

The sample application demonstrates how to use the library.

## Supported features

1. GATT provisioning with no oob devices and static oob devices that available in Bluetooth Mesh Profile 1.0.1.
2. Fast provisioning with no oob devices that avilable the telink private protocol.
3. Normal keybind:connect node -> read servers -> open notify -> set filter -> get composition -> appkey add ->
bind model to appkey.
4. Fast keybind(Default Bound, node need avilable the telink private protocol):connect node -> read servers -> open notify -> set filter -> appkey add.
5. Control node with unicastAddress and groupAddress:onoff，light，temperature，level，HSL.
6. Setting and clearing publication to a Model.
7. Setting and removing subscriptions to a Model.
8. Add node to group and delete node from group.
9. Add LPN node.
10. GATT OTA: nodes need avilable the telink private protocol.
11. Scheduler models.
12. Scene models.
13. Proxy Filter.

## Requirements

* Xcode 11.0 or newer.
* An iOS 9.0 or newer device with BLE capabilities.

## Setting up the library

* copy folder `TelinkSigMeshLib` to your project, then `Link Binary With Libraries` the `TelinkSigMeshLib.framework`.

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
* 7. Start connect mesh network(this api will retry connect action every 5 seconds unless connect success or call `stopMeshConnectWithComplete:`)

```Object-C
- (void)startMeshConnectWithComplete:(nullable startMeshConnectResultBlock)complete;
```
* 8. Stop connect mesh network

```Object-C
- (void)stopMeshConnectWithComplete:(nullable stopMeshConnectResultBlock)complete;
```
* 9. Control node: OnOffSet(more control api in `SDKLibCommand`)

```Object-C
+ (SigMessageHandle *)genericOnOffSetWithDestination:(UInt16)destination isOn:(BOOL)isOn retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
```
* 10. Send ini command API1(api reference c-lib SDK, iniData like "a3ff000000000200ffffc21102c4020100".)

```Object-C
+ (nullable NSError *)sendOpINIData:(NSData *)iniData successCallback:(responseAllMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
```
* 11. Send ini command API2

```Object-C
+ (nullable NSError *)sendIniCommandModel:(IniCommandModel *)model successCallback:(responseAllMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
```
* 12. Message's live from TelinkSigMeshLib(developer need set `SigMeshLib.share.delegateForDeveloper = self;`)

>12.1. APP did Receive Message from node.
```Object-C
- (void)didReceiveMessage:(SigMeshMessage *)message sentFromSource:(UInt16)source toDestination:(UInt16)destination;
```
>12.2. APP did Send Message to node.
```Object-C
- (void)didSendMessage:(SigMeshMessage *)message fromLocalElement:(SigElementModel *)localElement toDestination:(UInt16)destination;
```
>12.3. APP failed To Send Message.
```Object-C
- (void)failedToSendMessage:(SigMeshMessage *)message fromLocalElement:(SigElementModel *)localElement toDestination:(UInt16)destination error:(NSError *)error;
```
>12.4. APP did Receive SigProxyConfiguration Message from node.
```Object-C
- (void)didReceiveSigProxyConfigurationMessage:(SigProxyConfigurationMessage *)message sentFromSource:(UInt16)source toDestination:(UInt16)destination;
```

SigMesh iOS SDK of Telink Semiconductor (Shanghai) Co., Ltd.
