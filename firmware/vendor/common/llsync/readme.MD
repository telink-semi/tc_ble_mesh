## 概述

LLSync MESH SDK 提供了 LLSync MESH协议接入方案，SDK 内实现了 LLSync MESH协议用于和 APP(网关设备)进行通信，打通了 `BLE MESH设备-APP(网关设备)-物联网开发平台` 的数据链路，支持开发者快速接入 BLE MESH设备到物联网开发平台。

> 1. LLSync MESH SDK不包含BLE MESH协议栈，仅在Model层上做了接入扩展。
> 2. LLSync MESH SDK 符合蓝牙技术联盟发布的Mesh Profile规范

开发者接入 LLSync MESH SDK需要做的工作有：

1. 添加 LLSync MESH到 BLE MESH协议栈中
2. 通过脚本将数据模版转换为C代码，添加相应的数据处理逻辑

LLSync MESH SDK 封装了协议实现细节和数据传输过程，让开发者可以聚焦在数据处理上，以达到快速开发的目的。


## 软件架构

LLSync MESH SDK 结构框图:

![LLSyncMESH结构框图](https://s1.ax1x.com/2022/07/12/jgv881.png)

SDK 分三层设计，从上至下分别为应用层、LLSync核心层、HAL移植层。

* 应用层：LLSync MESH SDK 生成了数据模版的模版文件，用户需要根据需求做具体实现。
* LLSync MESH核心组件：实现了 BLE 设备和App (网关设备)之间的通信协议，身份认证，数据解析等功能，用户一般无需改动即可使用。
* HAL 移植层：主要是适配 BLE MESH 协议栈，用户需要进行移植和适配。

## 目录结构

```c
qcloud_iot_explorer_ble
  ├─config                            # SDK 配置文件
  ├─docs                              # 文档
  ├─inc                               # 头文件
  ├─samples                           # 示例工程
  │   ├─esp32                         # 基于esp32的示例工程
  └─src                               # LLSync源码
      ├─core                          # 核心代码
      ├─internal_inc                  # 内部头文件
      └─utils                         # 工具代码
```

## 移植指引

请参见 [LLSync mesh SDK移植说明](./docs/LLSync_Mesh_SDK移植说明.md) 和 [LLSync Mesh接入规范](./docs/LLSync_Mesh接入规范.md) 

## 最佳实践

请参见[ESP32移植实践](./samples/esp32/readme.MD)