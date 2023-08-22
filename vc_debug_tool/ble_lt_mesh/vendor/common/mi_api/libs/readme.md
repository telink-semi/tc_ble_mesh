### 0. 简介
**mijia ble libs** 是一个和硬件平台无关的中间层，它帮助开发者实现米家认证和绑定，安全通信，固件传输，mibeacon 事件广播等功能。libs 由纯 C 代码实现，不依赖运行环境，在 RTOS 以及裸机环境中均可工作。libs 利用 [**mijia ble api**](https://github.com/MiEcosystem/mijia_ble_api.git) 抽象层隔离硬件相关代码。因此在使用前，开发者需先下载 **mijia ble api** 仓库里对应 SoC 分支的 API 适配代码。（若所选 SoC 平台还未适配，可以参考 [API 文档](https://miecosystem.github.io/mijia_ble_api/) 进行适配）

```
.
├── common                  通用功能模块，例如 mibeacon，GATT层分包，GATT层加密通道。
├── cryptography            密码学模块，利用软件 (mbedtls) 或硬件（Mijia Secure Chip）实现 SHA256, ECC256, AESCCM 等常见算法。
├── gatt_dfu                GATT 固件更新，支持固件验签功能。
├── mesh_auth               米家 mesh 认证，私有 mesh 设备绑定流程。
├── mi_config.h             功能配置文件.
├── mijia_profiles          米家自定义服务，比如米家服务，锁服务。
├── secure_auth             米家安全认证，非 mesh 设备绑定流程。
└── third_party             第三方代码，例如 mbedtls，micro-ecc 等。
```

### 1. 设备认证
米家设备需通过米家认证，才可以绑定到米家用户账号，绑定成功后可以通过米家 App 或小爱同学进行控制。一个未绑定的设备，通过认证后绑定到米家账号的过程称为**注册**。

**mijia ble libs** 支持2类 BLE 设备认证：米家 BLE 安全认证 `secure auth` 和米家 mesh 认证 `mesh auth`。
其中，米家 BLE 安全认证适用于高安全级产品，比如智能门锁。米家 mesh 认证适用于支持 mesh 功能的产品，比如 mesh 灯泡。认证类型在小米 IoT 开发平台创建产品时进行选择，一经选择不可更改。另，米家安全认证`secure auth` 必须外接米家安全认证芯片才可以完成。

根据接入类型添加宏定义 `MI_BLE_ENABLED` 或 `MI_MESH_ENABLED`,以及产品ID `PRODUCT_ID=xxx`。若选择了 `MI_BLE_ENABLED`，则还需添加 `HAVE_MSC=1` 表明有安全认证芯片支持。

米家认证依靠 GATT 连接，利用小米服务 `mi service` 进行数据传输。认证过程中，双方会交换公钥和证书信息，同时还会进行 ECDH，ECDSA 运算。整个过程耗时较长，为了不阻塞系统的其他功能，引入了一个简易调度器 `mi_scheduler`。调度器将整个过程分为多个时间片，分次在 `mi_schd_process（）` 中执行，执行的结果通过回调函数通知应用层。因此，使用 `mi_scheduler_start(X)` 发起的过程均为异步操作，其返回值仅代表**能否发起** X 过程，而非 X 过程执行的**结果**。

#### 准备工作
创建小米服务：
添加 `./mijia_profiles/mi_service_server.c` 到自己工程，然后调用 `mi_service_init()` 即可完成小米服务创建。

初始化调度器：
根据所选的认证类型，添加 `./secure_auth/` 或 `./mesh_auth/` 里的文件到自己工程，调用 `mi_scheduler_init ()` 即可。初始化时，需要传入调度器工作间隔，回调函数。若有安全认证芯片，还需传入芯片电源管理函数以及 IIC 引脚信息。（工作间隔应**小于** BLE 最小连接间隔）

发送 mibeacon 广播：
米家认证依靠 BLE 连接进行数据传输。为了建立 BLE 连接，设备广播中需要包含小米服务数据(`ServiceData` UUID 0xFE95)，这样设备才会被米家 App 和米家网关发现。我们将`ServiceData`字段中的数据称为 [**mibeacon**](https://github.com/MiEcosystem/miio_open/blob/master/ble/米家BLE广播协议.md)。针对不同认证类型，设备需配置 mibeacon 中的特定标志位。[*详见 mibeacon 格式*](https://github.com/MiEcosystem/miio_open/blob/master/ble/米家BLE广播协议.md) 开发者可使用 `mibeacon_data_set()` 生成所需 mibeacon 数据，然后将 mibeacon 数据填充到广播包中即可。

#### 工作原理
为兼容 main loop 和 RTOS 两种运行环境，米家认证采用 `Event-Driven` 模式，依靠 BLE 和 timer 事件驱动。使用 `mi_scheduler_init ()` 初始化后，调度器会创建并维护一个 tick 定时器。认证过程由 `mi service` 里 auth 特征值写事件 (ATT WRITE) 触发。在没有 RTOS 的系统环境，需要将 `mi_schd_process()` 加入到 main loop 中执行，也可以通过设置宏定义 `MI_SCHD_PROCESS_IN_MAIN_LOOP=0`，使 `mi_schd_process()` 在定时器的超时处理函数中执行；在有 RTOS 调度的系统，可以创建一个 task，然后等待定时器超时信号，待收到信号后再调用 `mi_schd_process()`。

注册结束后，`mi_schd_process()` 会调用开发者注册的回调函数，用传参标识认证结果。若为 mesh 设备，成功后还需要将 mibeacon 中 mesh 状态变为可使用，并将收到的 mesh config 配置给 mesh 协议栈。

### 2. 安全通信
众所周知，BLE 在链路层提供了安全机制(Security Manager)，可以通过配对方式 (Pair) 生成秘钥(LTK)，并利用秘钥对链路层数据加解密。但是，此秘钥由手机系统负责保管，米家 App 无法获取，即无法通过米家账号体系同步。这导致用户更换手机后无法再操作已绑定的设备。

因此，米家设备没有使用 BLE 提供的安全机制，而选择在 GATT 层建立米家安全通道。注册成功后，设备会生成长期秘钥(LTMK)，此秘钥由 libs 保管，开发者无需关心。后续建立连接时，设备会使用此秘钥进行鉴权。这个过程称之为**登录**。同之前注册过程，登录的结果也会通过回调机制通知开发者。

登录成功后，设备和手机之间即建立了加密通道。可以利用 `mi_session_encrypt()` 和 `mi_session_decrypt()` 对传输数据进行加解密。需要注意的是，加密后的数据会比明文数据多出 6 字节附加信息。因为加密采用 AESCCM 模式，加密后会额外生成 4 字节完整性校验值(MIC)。此外为了防止重放攻击，消息头部又添加了 2 字节序号。

### 3. 固件传输
此功能暂时只支持 mesh 设备。
为保证固件的合法性，米家服务器会对每次上传的固件添加签名信息。具体流程如下：首先，开发者需要配置支持固件签名的最低版本；然后，开发者将固件上传到米家服务器，并注明对应固件版本号(需大于最低版本)；之后，米家服务器会自动生成当前固件的签名并追加到固件结尾。更新固件时，米家 App 会和设备建立连接并发起登录。登录成功后，App 读取当前设备版本信息，同时对比云端最新版本。若存在更新，App 会将新固件下载到手机本地，然后再通过 GATT 连接将已签名固件传输到设备上。传输完成后，设备会先进行固件验签，待验证通过后，设备再进行最终固件切换。

### 4. mibeacon 事件广播
BLE 是一种近场通信方式，如果要实现设备状态/事件信息的上报，则需要搭配一个能够连接互联网的桥接设备，这个设备我们称之为**蓝牙网关**。

如之前介绍，设备广播包中必须包含 mibeacon 才可以被米家蓝牙网关发现。当要上报信息时，设备需要给 mibeacon 增加 Object 字段。Object 字段中包含两种信息：**事件信息**、**状态信息**。事件信息不存在上报频率限制，具有高实时性，可以用来触发自动化场景，比如开门事件触发开灯；状态信息存在上报频率限制，具有高延迟，适用于周期上报变化缓慢的数据，比如温度。开发者可以使用 `mibeacon_obj_enque()` 将要发送的 object 放入发送队列，之后 libs 会自动发送带有 object 的 mibeacon。每条信息的发送间隔和总时长可以在 `mi_config.h` 进行修改。