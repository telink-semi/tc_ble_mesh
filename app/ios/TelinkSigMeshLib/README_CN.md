# TelinkSigMeshLib

version: v3.3.4

## 关于

TelinkSigMeshLib库用于SIG设备的组网、绑定、收发消息。 

> SIG的官方协议可通过以下链接进行下载: https://www.bluetooth.com/specifications/mesh-specifications/

当前TelinkSigMeshLib库是基于SIG官方蓝牙mesh协议v1.0.1上进行开发的。

## 示例APP

示例APP用于演示TelinkSigMeshLib库是如何使用的.

## 支持的功能

1. 支持通过GATT的方式添加no OOB的设备和static OOB的设备（SIG协议v1.0.1）。
2. 支持通过Fast provision的方式添加no OOB的设备(需要设备支持Telink私有协议)。
3. 支持普通KeyBind，流程：连接节点->读取服务->打开notify->设置白名单->获取设备构造数据->添加AppKey->绑定modelID到AppKey（SIG协议v1.0.1）。
4. 支持快速KeyBind，又名Default Bound，流程：连接节点->读取服务->打开notify->设置白名单->添加AppKey（需要设备支持Telink私有协议）。
5. 支持通过节点地址和组地址进行设备控制，包括：开关、亮度、色温、级别（level）、颜色（HSL）。
6. 支持public和取消public一个modelID。 
7. 支持订阅和取消订阅一个modelID。
8. 支持添加设备到组和从组移除设备。
9. 支持LPN设备。
10. 支持通过GATT的方式进行设备固件升级（需要设备支持Telink私有协议）。
11. 支持闹钟模型。
12. 支持场景模型。
13. 支持Proxy黑白名单过滤器。

## 要求环境

* Xcode 11.0及以上.
* 一个支持蓝牙功能的系统是iOS9及以上的苹果设备。

## 集成步骤

* 拷贝文件夹`TelinkSigMeshLib`到开发者自己工程文件夹，将TelinkSigMeshLib.xcodepro拖进开发者自己的工程中，添加头文件搜索路径（即与文件TelinkSigMeshLib.xcodepro同级的文件夹TelinkSigMeshLib的路径），然后编译选项中链接二进制库`TelinkSigMeshLib.framework`。

## 开发接口

* 1. 启动SDK.

```Object-C
[SDKLibCommand startMeshSDK];
```
* 2. 设置SDK的日志记录登记.

```Object-C
[SigLogger.share setSDKLogLevel:SigLogLevelDebug];
```
* 3. 通过GATT添加设备的API1 (自动添加下一个，直达扫描不到可添加的设备。)

```Object-C
- (void)startAddDeviceWithNextAddress:(UInt16)address networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex appkeyModel:(SigAppkeyModel *)appkeyModel unicastAddress:(UInt16)unicastAddress uuid:(nullable NSData *)uuid keyBindType:(KeyBindTpye)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData isAutoAddNextDevice:(BOOL)isAuto provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail finish:(AddDeviceFinishCallBack)finish;
```
* 4. 通过GATT添加设备的API2 (单个设备添加)

```Object-C
- (void)startAddDeviceWithNextAddress:(UInt16)address networkKey:(NSData *)networkKey netkeyIndex:(UInt16)netkeyIndex appkeyModel:(SigAppkeyModel *)appkeyModel peripheral:(CBPeripheral *)peripheral provisionType:(ProvisionTpye)provisionType staticOOBData:(nullable NSData *)staticOOBData keyBindType:(KeyBindTpye)type productID:(UInt16)productID cpsData:(nullable NSData *)cpsData provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail;
```
* 5. 通过GATT添加设备的API3: 便捷接口 (单个设备添加)

```Object-C
- (void)startAddDeviceWithSigAddConfigModel:(SigAddConfigModel *)configModel provisionSuccess:(addDevice_prvisionSuccessCallBack)provisionSuccess provisionFail:(ErrorBlock)provisionFail keyBindSuccess:(addDevice_keyBindSuccessCallBack)keyBindSuccess keyBindFail:(ErrorBlock)keyBindFail;
```
* 6. 通过fast provision添加设备的接口

```Object-C
- (void)startFastProvisionWithProvisionAddress:(UInt16)provisionAddress productId:(UInt16)productId compositionData:(NSData *)compositionData currentConnectedNodeIsUnprovisioned:(BOOL)unprovisioned addSingleDeviceSuccessCallback:(AddSingleDeviceSuccessOfFastProvisionCallBack)singleSuccess finish:(ErrorBlock)finish;
```
* 7. 开始连接当前mesh的设备(该接口会每5秒重试一次直到连接成功或者调用了取消mesh连接接口`stopMeshConnectWithComplete:`。)

```Object-C
- (void)startMeshConnectWithComplete:(nullable startMeshConnectResultBlock)complete;
```
* 8. 取消mesh连接接口

```Object-C
- (void)stopMeshConnectWithComplete:(nullable stopMeshConnectResultBlock)complete;
```
* 9. 控制设备: OnOffSet(more control api in `SDKLibCommand`)

```Object-C
+ (SigMessageHandle *)genericOnOffSetWithDestination:(UInt16)destination isOn:(BOOL)isOn retryCount:(NSInteger)retryCount responseMaxCount:(NSInteger)responseMaxCount ack:(BOOL)ack successCallback:(responseGenericOnOffStatusMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
```
* 10. 发送自定义的ini指令API1(api reference c-lib SDK, iniData like "a3ff000000000200ffffc21102c4020100".设备端回复可以参考接口12.1.。)

```Object-C
+ (nullable NSError *)sendOpINIData:(NSData *)iniData successCallback:(responseAllMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
```
* 11. 发送自定义的ini指令API2（设备端回复可以参考接口12.1.。）

```Object-C
+ (nullable NSError *)sendIniCommandModel:(IniCommandModel *)model successCallback:(responseAllMessageBlock)successCallback resultCallback:(resultBlock)resultCallback;
```
* 12. TelinkSigMeshLib库的消息的生命周期(开发者需要设置如下代理 `SigMeshLib.share.delegateForDeveloper = self;`)

>12.1. APP收到了消息的代理回调。
```Object-C
- (void)didReceiveMessage:(SigMeshMessage *)message sentFromSource:(UInt16)source toDestination:(UInt16)destination;
```
>12.2. APP发送消息完成的代理回调。
```Object-C
- (void)didSendMessage:(SigMeshMessage *)message fromLocalElement:(SigElementModel *)localElement toDestination:(UInt16)destination;
```
>12.3. APP发送消息失败的代理回调。
```Object-C
- (void)failedToSendMessage:(SigMeshMessage *)message fromLocalElement:(SigElementModel *)localElement toDestination:(UInt16)destination error:(NSError *)error;
```
>12.4. APP收到SigProxyConfiguration类型的消息的代理回调。（当前只有setFilter相关的指令是SigProxyConfiguration类型的消息）
```Object-C
- (void)didReceiveSigProxyConfigurationMessage:(SigProxyConfigurationMessage *)message sentFromSource:(UInt16)source toDestination:(UInt16)destination;
```

SigMesh iOS SDK of Telink Semiconductor (Shanghai) Co., Ltd.
