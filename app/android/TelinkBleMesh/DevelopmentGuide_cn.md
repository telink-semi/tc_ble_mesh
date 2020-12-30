# TelinkBleMesh

## 关于
基于SIG标准协议实现的 mesh demo app. 
 

 ## 测试环境
 + Telink SIG mesh 设备
 + Android 手机 (4.3+)
 
------------------------------------

## 开始使用

1. 在 Android Studio 中打开 : file -> open -> select TelinkBleMesh;
 或者在Android Studio仅导入库: file -> new -> import module -> select TelinkBleMeshLib.

2. 创建自定义的 Application 类 继承自 `MeshApplication`;
    或者创建自定义类 implement `EventHandler` 并处理handle方法 (一般是发送事件， 更多信息参考 [details](#detail_event_handler))
3. 调用 `MeshService#init` 初始化lib; 
    再调用 `MeshService#setupMeshNetwork(MeshConfiguration)` 配置网络信息 (更多网络配置信息可以参考 see [details](#detail_config))
---
## 网络操作接口

### 配网认证(provision)
1. 监听以下事件 :
   + `ScanEvent.EVENT_TYPE_DEVICE_FOUND` (找到未配网节点)
   + `ScanEvent.EVENT_TYPE_SCAN_TIMEOUT` (扫描超时)
   + `ProvisioningEvent.EVENT_TYPE_PROVISION_SUCCESS` (节点配网成功)
   + `ProvisioningEvent.EVENT_TYPE_PROVISION_FAIL` (节点配网失败)
2. 调用 `MeshService#startScan` 以扫描未配网节点; 
3. 在设备找到时， 调用 `MeshService#startProvisioning` ， 传入分配的单播地址和static-OOB数据（OOB为可选， 若未设置， 则采用no-oob）. 调用完成后， 配网事件将在配网动作完成后触发;

### 绑定
绑定是指将选择的 application key 和 key index 与 选择的设备中的 model （未选择时， 为所有非config model）进行绑定操作， 以方便后续的控制。
1. 监听以下事件 :
   + `BindingEvent.EVENT_TYPE_BIND_SUCCESS` (绑定成功)
   + `BindingEvent.EVENT_TYPE_BIND_FAIL` (绑定失败)
2. 调用 `MeshService#startBinding`


### 网络控制
在完成model与application key绑定后， 即可通过发送指令控制model
+ 发送mesh消息: 
   - 调用 `MeshService#sendMeshMessage` 发送普通mesh消息 ,配置消息, 自定义消息等. 
   例如, 发送 `OnOffSetMessage` 来控制设备的 on/off 状态
     (更多关于mesh消息参考 [details](#detail_mesh_message))        
+ 接收mesh消息: 
   - 在发送带回复的消息时 , 设备状态改变时, 设备周期publish状态时 ,, 即可收到状态信息.
    此时， 会组装且发送出 `StatusNotificationEvent` 这个事件. 该事件的 `eventType` 由 MeshStatus#Container 中的配置决定。 一般采用className字符串作为eventType; 例如, on off 状态 的eventType是 `OnOffStatusMessage.class.getName()`

### 设备OTA (GATT， Telink private)
1. 监听以下事件:
    + `GattOtaEvent.EVENT_TYPE_OTA_SUCCESS` (OTA成功)
    + `GattOtaEvent.EVENT_TYPE_OTA_PROGRESS` (OTA进度)
    + `GattOtaEvent.EVENT_TYPE_OTA_FAIL` (OTA失败)
2. 调用 `MeshService#startGattOta`

### 扩展信息

##### 默认绑定
默认绑定是Telink私有的绑定动作， 意在将所有的model绑定动作更快完成。
+ 设置 `BindingDevice#defaultBound` 为 true 以使能默认绑定. 
##### 快速配网
快速认证是Telink私有的配网动作， 意在对周围的多个节点进行快速配网认证。
###### 前提: 
1. 设备支持快速配网
2. 已知设备的composition-data
###### 流程:
1. 监听以下事件 :
    + `FastProvisioningEvent.EVENT_TYPE_FAST_PROVISIONING_ADDRESS_SET` (配置地址信息完成)
    + `FastProvisioningEvent.EVENT_TYPE_FAST_PROVISIONING_FAIL` (快速配网失败)
    + `FastProvisioningEvent.EVENT_TYPE_FAST_PROVISIONING_SUCCESS` (快速配网成功)
2. 调用 `MeshService.#startFastProvision` 

##### Remote 配网
###### 前提: 
1. 设备支持remote-provision; 
2. 需直连上已配网完成的proxy节点
###### 流程:
1. 监听以下事件 :
    + `ScanReportStatusMessage` (扫描到remote设备)
    + `RemoteProvisioningEvent.EVENT_TYPE_REMOTE_PROVISIONING_SUCCESS` (remote配网成功)
    + `RemoteProvisioningEvent.EVENT_TYPE_REMOTE_PROVISIONING_FAIL` (remote配网失败)
    
2. 发送 `ScanStartMessage` mesh 消息, 在接收到 `ScanReportStatusMessage` 过滤并缓存remote设备信息
3. 调用 `MeshService#startRemoteProvisioning`

##### Mesh OTA (mesh 固件升级)
###### 前提:
1. 设备支持mesh-OTA;
2. 升级用的bin;
##### 流程:
1. 监听以下事件 :
    + `FirmwareInfoStatusMessage` (设备版本状态信息上报)
    + `MeshUpdatingEvent.EVENT_TYPE_UPDATING_PREPARED` (mesh ota准备完成 -- 发送block数据前上报)
    + `MeshUpdatingEvent.EVENT_TYPE_UPDATING_SUCCESS` (mesh ota 成功)
    + `MeshUpdatingEvent.EVENT_TYPE_UPDATING_FAIL` (mesh ota 失败)
    + `MeshUpdatingEvent.EVENT_TYPE_UPDATING_PROGRESS` (mesh ota 进度更新)
    + `MeshUpdatingEvent.EVENT_TYPE_DEVICE_SUCCESS` (单设备升级成功)
    + `MeshUpdatingEvent.EVENT_TYPE_DEVICE_FAIL` (单设备升级失败)
2. 调用 `MeshService#startMeshOta`


#### 补充说明
<span id="detail_config"></span>
##### MeshConfiguration 网络配置信息 : 
+ networkKey : 网络密钥
+ netKeyIndex : 网络密钥索引
+ appKeyMap : 应用层密钥和索引图表
+ ivIndex : iv-index
+ sequenceNumber : 消息序列号
+ localAddress : 本地地址， 应为合法的单播地址 (0x0001 - 0x7FFF)
+ deviceKeyMap : 设备密钥和设备地址图表
 
<span id="detail_mesh_message"></span>
##### MeshMessage mesh消息:
+ opcode : 应用层消息操作码
<span id="detail_mesh_message_params"></span>
+ params : 应用层消息参数
+ destinationAddress : 网络层目标地址
+ sourceAddress : 网络层消息源地址, **lib管理**, 取自 `MeshConfiguration#localAddress`
+ accessType : 消息类型 DEVICE 或 APPLICATION
+ accessKey : 消息应用层到传输层加密key , **lib管理**, 如果accessType是DEVICE , 该值为 device key , 否则为 application key
+ appKeyIndex: 当accessType==APPLICATION 使， 用于索引application key
+ ctl : 0 用于应用消息, 1 用于控制消息
+ ttl : 网络传输ttl, 默认 10

+ responseOpcode : 用于 **reliable message**,定义期望收到的消息操作码 , 例如 on/off-get-message 期望收到 on/off-status-message 
+ responseMax : 用于 **reliable message**,定义期望收到的消息回复总数, 如果没有收到回复或者收到的回复数少于该值， 则会进行消息重试
+ retryCnt : 用于 **reliable message**,用于定义最大重试次数

+ tidPosition : 用于 **tid message**,定义tid在参数区的位置 [params](#detail_mesh_message_params), 如果是合法地址， lib中会填入当前的tid， 且将tid值加1， 如果是非法地址， lib中就不会填入tid信息

<span id="detail_event_handler"></span>
##### EventHandler:
+ NetworkInfoUpdateEvent : 当 iv-index or sequence-number 更新时上报， app应缓存该值， 且在下次设置网络信息时带入对应的信息
+ OnlineStatusEvent : 当收到 online-status(telink private status) 时上报

+ StatusNotificationEvent : 当收到mesh状态消息时上报; 
    其中 eventType 由 StatusMessage 确定 (在 MeshStatus 中注册) 
+ MeshEvent : 当网络状态变化（如断开连接）时上报
+ 其它在执行动作时的事件:
    + ScanEvent
    + ProvisioningEvent
    + BindingEvent
    + AutoConnectEvent
    + GattOtaEvent
    + FastProvisioningEvent
    + RemoteProvisioningEvent
    + MeshUpdatingEvent