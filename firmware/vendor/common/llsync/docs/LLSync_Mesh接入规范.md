
本文在蓝牙技术联盟发布的Mesh Profile的基础上，增加了部分协议规则，作为LLSync Mesh接入规范，指导芯片厂家对接腾讯云物联网开发平台。考虑到多数芯片厂商已经实现了蓝牙Mesh协议栈，LLSync Mesh协议不包含蓝牙Mesh协议栈，仅在Model层上做接入扩展。

## 术语定义


|术语 |说明 |
|-----|-----|
|CID |Company Identifier，公司标识符。腾讯的标识符为0x013A。 |
|PB-ADV |一种通过蓝牙Mesh beacon给Mesh设备进行配网的交互方式。 |
|PB-GATT |一种通过蓝牙GATT给Mesh设备进行配网的交互方式。 |
|Provisioning |配网过程，设备加入蓝牙Mesh网络的过程。 |
|Unprovisioned Device |未配网设备，还未加入蓝牙Mesh网络的设备。 |
|Node |节点，加入蓝牙Mesh网络的设备。 |
|PID |产品ID，腾讯云物联网开发平台创建产品时定义。 |
|Device Name |设备名称，腾讯云物联网开发平台创建设备时定义。 |


## 接入要求


- 蓝牙Mesh设备必须支持PB-GATT、PB-ADV。
- 蓝牙Mesh设备必须支持Proxy功能。
- 其他功能不做要求，用户可以根据需求灵活实现。


## UUID格式

UUID应用于Unprovisioned Device的PB-ADV广播和PB-GATT广播中，用于小程序识别设备，


- UUID中各字段均采用大端模式存储。
- UUID长度为16字节，无法包含设备三元组，所以广播包必须支持扫描应答，通过扫描应答包将Device Name回复给小程序。


|Field |Size(Octets) |Notes |
|-----|-----|----|
|CID |2 |Company Identifier. 0x013A。 |
|Flag |1 |bit3~0: 协议版本号，目前是0x01。<br>bit4: 广播状态，0是未配网广播，1是静默广播。<br>bit5 - bit7: RFU。 |
|Flag2 |1 |RFU，填充0x00。 |
|PID |10 |物联网开发平台创建的产品ID。 |
|RFU |2 |RFU，填充0x0000。 |


- 小程序扫描到未配网广播后，发出扫描请求，设备端收到扫描请求后使用扫描应答包回复Device Name。


|AD Type |Total Length |Notes |
|-----|-----|-----|
|《Flags》 |3 | |
|《Complete Local Name》 |8 |使用MAC地址作为设备名称。 |


- 规定使用设备MAC地址作为设备名称。设备MAC地址为 `11:22:33:44:55:66` 时设备名称为 `112233445566` 。


## LLSync Mesh广播包格式

小程序不支持PB-ADV，因此要求设备必须支持PB-GATT广播。


- 未配网广播 


LLSync Mesh设备上电后如处于未配网状态，需要广播未配网广播包。单次广播时长不小于150ms，广播间隔500ms，广播持续时间默认10分钟，具体产品的广播启动方式和广播持续时间以产品需求为准。如果超时后仍未被配网，设备可以进入静默广播状态，也可以停止广播。
未配网广播数据示例如下：

|AD Type |Total Length |Notes |
|-----|-----|-----|
|《Flags》 |3 |02 01 06 |
|《Service UUID List》 |4 |03 03 18 27 |
|《Service Data》 |22 |15 16 18 27 01 3A 01 00 11 22 33 44 55 66 77 88 99 AA 00 00 00 00 |


- 静默广播


静默广播只用于让Provisioner发现设备，LLSync Mesh设备如进入静默广播状态，设备在收到Provisioner发出的配网消息时不进行响应。静默广播每次广播时长不小于150ms，广播间隔60s。
设备静如静默广播状态时，需要修改UUID中静默广播标记位后开始广播。静默广播退出行为由用户根据产品定义。
静默广播数据示例如下：

|AD Type |Total Length |Notes |
|-----|-----|-----|
|《Flags》 |3 |02 01 06 |
|《Service UUID List》 |4 |03 03 18 27 |
|《Service Data》 |22 |15 16 18 27 01 3A 09 00 11 22 33 44 55 66 77 88 99 AA 00 00 00 00 |


## 广播包发送规则


- 未配网设备上电后，设备需要进入未配网广播状态，超时未配网设备可以进入静默广播状态或停止广播。
- 如果设备通过小程序解除配网状态，或设备通过硬件复位清除配网信息，设备需要重新进入未配网广播状态。
- 当设备响应建立连接请求（PB-GATT建立连接）后，设备进入配网状态。设备配网完成后进入已配网状态；否则恢复至未配网状态。
- 当设备进入已配网状态后，要求设备不再进行未配网广播和静默广播。


## 配网流程

LLSync Mesh遵循蓝牙Mesh标准配网流程，考虑到需要进行设备认证，将三元组作为OOB信息引入参与身份认证过程。 


- 要求设备必须支持Static OOB方式。 AuthValue的计算方式如下：


AuthValueProvisioner=  `hmac256(Random Provisioner;Product ID;Device Name;Psk)` 
AuthValueDevice=  `hmac256(Random Device;Product ID;Device Name;Psk)` 


- AuthValue计算后长度为32字节，将其前后16字节做异或运算后得到AuthValueProvisioner和AuthValueDevice。


## LLSync Mesh数据发送和接收


- 数据接收


通常情况下，我们建议设备端的scan window、interval、period分别为10ms、10ms、100ms。


- 数据发送


Mesh数据发包每次持续时间200ms左右，每次发包间隔20ms，每次打包向37、38、39三个广播频段同时发送数据。根据需要可等待若干毫秒后进行重传。重传间隔不能小于400ms，建议按照重传间隔=（200ms+100ms * TTL）来计算。发送数据TTL建议不大于3，重传次数不大于3。

## TID规范

当Mesh消息中包含TID字段（如Vendor Message Attr Set指令等）时，需要遵守以下规则。


- 所有有关联的业务使用相同的TID，比如设备接收到设置或查询指令时，设备回复到状态消息的TID（如果有）必须与下行到设置或查询指令中包含到TID一致。
- 设备主动上行使用的TID范围为128~191（0x80~0xBF）。
- 当Mesh设备收到一条发送给本设备（或本设备属于目标组）时，记录该TID；如果设备在在5秒内收到相同TID的消息，忽略后收到的消息（需要回复对应的状态消息，但业务层面不处理该消息）；被记录的TID在5秒后丢弃。
- 当消息当目标设备不是本设备（或本设备不属于目标组）时，忽略该消息，并且不记录TID。


## 心跳功能

小程序暂不支持heartbeat功能。

## OTA升级

LLSync Mesh暂不支持BLE OTA升级。

## 扩展消息定义

由于SIG Mesh的模型目前尚未覆盖所有的智能家居设备，因此需要采用厂商自定义模型（Vendor Model）来实现智能家居设备的控制和状态上报。


- SIG定义Vendor Model格式为4字节（其中2字节的Company ID和2字节的Vendor-assigned Model ID），其中腾讯的Company ID为0x013A，如下表所示。


|Field |Size(Octets) |Notes |
|-----|-----|----|
|CID |2 |0x013A |
|vendor-assigned model identifier |2 | |


- 下表为两个VendorModel ID，用于消息扩展用。


|Model Name |Model ID |
|-----|-----|
|Vendor Model Server |0x013A0000 |
|Vendor Model Client |0x013A0001 |

通常Mesh设备作为Vendor Model Server，小程序作为Vendor Model Client。


- 扩展消息操作码


使用3字节操作码，如下表所示。

|Vendor Message |Opcode |
|-----|-----|
|Vendor Message Attribute Set |0xC03A01 |
|Vendor Message Attribute Get |0xC13A01 |
|Vendor Message Attribute Set Unacknowledged |0xC23A01 |
|Vendor Message Attribute Status |0xC33A01 |
|Vendor Message Attribute Indication |0xC43A01 |
|Vendor Message Attribute Confirmation |0xC53A01 |

Vendor Message里的数据都使用小端优先方式传输。


- Vendor Message Attribute Set


该消息用于Vendor Model Client设置Vendor Model Server的一个或多个属性值，消息格式如下。

|Field |Size(Octets) |Notes |
|-----|-----|------|
|Opcode |3 |0xC03A01 |
|TID |1 |Transaction Identifier，每条新消息递增 |
|Attribute Type |2 |设置的Attribute类型 |
|Attribute Parameter |N |设置的Attribute参数 |

Attribute Type和Attribute Parameter最多可有15个。当Vendor Model Server收到Attribute Set消息后，必须向Vendor Model Client回复Attribute Status。如Vendor Model Client在下发该命令之后未收到Vendor Model Server返回的Attribute Status，可以再次下发该指令。


- Vendor Message Attribute Get


该消息用于Vendor Model Client获取Vendor Model Server的一个或多个属性值，消息格式如下。

|Field |Size(Octets) |Notes |
|-----|-----|------|
|Opcode |3 |0xC13A01 |
|TID |1 |Transaction Identifier，每条新消息递增 |
|Attribute Type |2 |读取的Attribute类型 |

Attribute Type最多可有15个。当Vendor Model Server收到Attribute Get消息后，必须向Vendor Model Client回复Attribute Status。如Vendor Model Client在下发该命令之后未收到Vendor Model Server返回的Attribute Status，可以再次下发该指令。


- Vendor Message Attribute Set Unacknowledged


该消息用于Vendor model Client设置Vendor Model Server的一个或多个属性值，消息格式如下。

|Field |Size(Octets) |Notes |
|-----|-----|-----|
|Opcode |3 |0xC23A01 |
|TID |1 |Transaction Identifier，每条新消息递增 |
|Attribute Type |2 |设置的Attribute类型 |
|Attribute Parameter |N |设置的Attribute参数 |

Attribute Type和Attribute Parameter最多可有15个。当Vendor Model Server收到Attribute Set Unacknowledged消息后，不需要向Vendor Model Client发送Attribute Status消息。


- Vendor Message Attribute Status


该消息用于Vendor Model Server回复Attribute Get和Attribute Set命令或上报设备状态信息给Vendor Model Client，消息格式如下。

|Field |Size(Octets) |Notes |
|-----|-----|----|
|Opcode |3 |0xC33A01 |
|TID |1 |Transaction Identifier，每条新消息递增 |
|Attribute Type |2 |上报的Attribute类型 |
|Attribute Parameter |N |上报的Attribute参数 |

Vendor Model Client收到Attribute Status后，不需要回复消息给Vendor Model Server。


- Vendor Message Attribute Indication


该消息用于Vendor Model Server发送属性给Vendor Model Client，消息格式如下。

|Field |Size(Octets) |Notes |
|-----|-----|------|
|Opcode |3 |0xC43A01 |
|TID |1 |Transaction Identifier，每条新消息递增，回复控制命令的TID为下发消息的TID<br>设备状态主动改变上报的TID从128到191循环 |
|Attribute Type |2 |上报的Attribute类型 |
|Attribute Parameter |N |上报的Attribute参数 |

Attribute Type和Attribute Parameter最多可有15个。当Vendor Model Client收到Attribute Indication消息后，必须向Vendor Model Server回复Attribute Confirmation。如Vendor Model Server在发出该命令之后未收到Vendor Model Client回复的confirmation，可以再次发送该指令。


- Vendor Message Attribute Confirmation


该消息用于Vendor Model Client回复给Vendor Model Server，用于表示已收到Vendor Model Server发出的Indication，消息格式如下。

|Field |Size(Octets) |Notes |
|-----|-----|-----|
|Opcode |3 |0xC53A01 |
|TID |1 |Transaction Identifier，收到的Indication消息的TID |


## 属性表


|Attr Class |Attr Name |Attr Type |Data Struct |Unit |Accuracy |Notes |
|-----|-----|----|------|-----|-----|----|
|系统属性(0xFFxx)|错误码|0xFF00|uint8 Error_Code|-|-|Error Code：错误码|
|系统属性(0xFFxx)|版本信息|0xFF01|uint32 version_number|-|-|version_number：与OTA的版本一致|
|系统属性(0xFFxx) |设备绑定 |0xFF02 |u32 Nonce u32 time |- |- |用于小程序绑定设备 |
|系统属性 (0xFFxx)|设备绑定回复 |0xFF03 |u8 sign[32] |- |- |sign=hmacsha256(psk, product_id+ device_name+';'+nonce+';'+timestamp) |
|通用属性(0xF0xx)|开关|0xF000|bool OnOff|-|-|设备开关状态,与generic onoff绑定|
|通用属性(0xF0xx)|Unix时间戳|0xF001|uint8 unix_time[6]|毫秒|-|unix时间戳|
|通用属性(0xF0xx)|电量|0xF002|uint8 power_percent|百分比|-|电池电量百分比|
|电工照明(0xF1xx)|颜色|0xF100|uint16 HSL[3]|-|-|设备颜色，与Light HSL Model值一致 Color[0]= Lightness Color[1] = Hue Color[2] = Saturation|
|电工照明(0xF1xx)|亮度|0xF101|uint16 lightness_level|-|-|设备亮度等级，与Lightness Model值一致|
|电工照明(0xF1xx)|色温|0xF102|uint16 color_temperature|开尔文|-|设备色温值，与Light CTL Model值一致|

## 设备 Mesh 协议栈资源要求
|资源类目|要求|涉及操作|备注|
|-----|-----|-----|------|
|Proxy filter list 长度上限|>=2|Add Addresses to Filter|小程序作为 Proxy Client，通过 GATT 连接支持 Proxy 能力的设备（Proxy Server）后，需要设置 Proxy filter list，其中包含 2 个地址:<br>1.当前小程序用户的单播地址；<br>2.所有小程序用户共同订阅的组地址（0xf000）|
|节点可添加的 NetKey 数量|>=2|Config NetKey Add和Provisioning Data| 1.小程序对设备配网期间，通过 Provisioning Data 下发一个初始 NetKey<br>2.小程序将设备绑定到连连家庭后，会通过 Config NetKey Add 重新设置一个 NetKey（小程序后续会增加该行为，目前暂时使用初始 NetKey）|
|节点可添加的 AppKey 数量|>=2|Config AppKey Add|1.小程序对设备配网后，会向设备添加一个 AppKey，用于小程序与设备的一对一控制<br>2.小程序建立设备组时，会向设备添加一个 AppKey，用于设备组的控制|
|Model 可绑定的 AppKey 数量|>=2|Config Model App Bind|1.小程序对设备配网后，会向设备相关的 Model 绑定一个 AppKey，用于小程序与设备的一对一控制<br>2.小程序建立设备组时，会向设备相关的 Model 绑定一个 AppKey，用于设备组的控制|
|Model 可订阅的组地址数量|>=2|Config Model Subscription Add|1.LLSync Mesh 要求设备相关 Model 默认订阅对应产品品类的一个组地址<br>2.小程序建立设备组时，会向设备相关的 Model 添加一个组地址订阅，用于设备组的控制|


