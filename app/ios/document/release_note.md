# Demo App for TelinkSigMeshLib release notes

------------------------------------------------------

## v4.1.0.0
release date:	2023/11/10

>更新日志：
1.feat(ios): 添加多网络管理功能。
2.fix(ios): 修复SDK频繁收发数据低概率导致APP奔溃的bug。
3.fix(ios): 代码注释率到达40%。
4.fix(ios): 在获取到未入网节点的capabilities PDU后再给节点分配unicastAddress。
>
>release note:
1.feat(ios): add multiple network management features.
2.fix(ios): Fix the bug where the SDK frequently sends and receives data with a low probability of causing APP crashes.
3.fix(ios): The code annotation rate has reached 40%.
4.fix(ios): Assign a unicastAddress to the node after obtaining the capabilities PDU of the unprovision node.

------------------------------------------------------

## v3.3.3.6
release date:	2023/3/18

>更新日志：
1.fix(ios): 从分享的JSON文件中移除ivIndex字段。
2.fix(ios): 新增Telink自定义PID结构体`TelinkPID`，以适配不同的芯片的设备的图标显示。
3.fix(ios): 支持B91 SIG MESH SDK和B85 SIG MESH SDK。
>
>release note:
1.fix(ios): remove ivIndex in JSON storage.
2.fix(ios): update icon generation by check pid bitmask.
3.fix(ios): support B91 SIG MESH SDK and B85 SIG MESH SDK.

------------------------------------------------------

## v3.3.3.5
release date:	2022/4/8

>更新日志：
1.feat(ios):新增draft feature的功能private beacon和enhanced opcode aggregator。
2.fix(ios): 修复remote provision的bug。
3.docs(ios): 更新开源许可为Apache License 2.0。
>
>release note:
1.feat(ios): add support for private beacon and enhanced opcode aggregator in draft feature.
2.fix(ios): fix the bug of remote provision.
3.docs(ios): update license to Apache License 2.0.

------------------------------------------------------

## v3.3.4
release date:	2021/11/9

>更新日志：
1.fix(ios): 更新JSON文件格式至R11版本。
2.feat(ios): 添加遥控器设备支持。
3.feat(ios): LPN节点支持fast provision入网。
4.feat(ios): APP新增设备配置界面。
5.feat(ios): 新增串行指令队列，允许开发者同时压多条指令到SDK里面。
6.feat(ios): 更新mesh OTA，支持手机和直连节点做distributor。
>
>release note:
1.fix(ios): Update JSON database to version R11.
2.feat(ios): Add support for switch devices.
3.feat(ios): Add support for fast provision LPN.
4.feat(ios): APP add the new UI to get/set Device Config.
5.feat(ios): Add serial command queue, developer can push some commands to SDK at the same time.
6.feat(ios): Update mesh OTA, supports app and directly connected nodes as distributor.

------------------------------------------------------

## v3.3.2

2021/2/4
release date:	2021/2/4

>更新日志：
1.feat(ios): 新增subnet bridge功能。
2.fix(ios): 修复SDK未根据设备端返回的ivi进行解密的bug，所有客户都需要升级到最新版本。
3.fix(ios): 修复SDK的日志保存功能无法关闭的bug。
4.feat(ios): 新增配置参数defaultPublishPeriodModel。
>
>release note:
1.feat(ios): subnet bridge.
2.fix(ios): Fixed bug that SDK did not decrypt on the basis of IVI returned by device side, All customers need to upgrade to the latest version.
3.fix(ios): Fix the bug that the log saving function of SDK cannot be closed.
4.feat(ios): add configuration parameters `defaultPublishPeriodModel`.

------------------------------------------------------

## v3.3.0
release date:	2020/12/31

>更新日志：
1.feature(ios): 新增配置参数defaultUnsegmentedMessageLowerTransportPDUMaxLength，默认值为15，用于判断数据包使用segmentedMessage还是unSegmentedMessage。
2.feature(ios): 添加回包根据sequenceNumber进行过滤的机制。
3.fix(ios): 修复keyBind过程中关闭手机蓝牙时依然显示keyBind成功的bug。
4.fix(ios): 修复AppKey的AID为0时mesh数据包加解密异常的bug。
5.fix(ios): 修复多个线程同时发送SigSegmentAcknowledgmentMessage数据包时导致SDK发生crash的bug。
>
>release note:
1.feat(ios): A new configuration parameter ‘defaultUnsegmentedAccessMessageLowerTransportPDUMaxLength’, with a default value of 15, is added to determine whether the packet uses segmentedMessage or unSegmentedMessage.
2.feat(ios): Add the mechanism of filtering response package according to sequenceNumber.
3.fix(ios): fix the bug that keybind is still successful when Bluetooth is turned off during keybind. 
4.fix(ios): fix the bug of abnormal encryption and decryption of mesh packets when the AID of AppKey is 0.
5.fix(ios): fix the SDK crash bug when multiple threads send SigSegmentAcknowledgmentMessage packets at the same time.


------------------------------------------------------

## v3.2.3
release date:	2020/09/27

>更新日志：
1.breaking changes(ios): 移除C语言版本SDK。
2.fix(ios): 优化读取服务完成的判断逻辑。
3.fix(ios): 优化根据参数responseMax上报数据包的逻辑。
4.feature(ios): 添加新的NetKey和AppKey，切换不同的可以进行消息发送。
>
>release note:
1.breaking changes(ios): remove c-lib SDK.
2.fix(ios): Optimize the judgment logic of reading Bluetooth service.
3.fix(ios): Optimize the logic of reporting packets according to the parameter `responseMax`.
4.feature(ios): add new NetKey and AppKey, change current NetKey and AppKey of send message.

------------------------------------------------------

## v3.2.3 beta
release date:	2020/08/27

>更新日志：
1.修复添加cypress设备时crash的bug。
2.修复添加cypress设备成功率低的问题。
>
>release note:
1.fix: fix the crash bug when add cypress device.
2.fix: increase the success rate of adding cypress devices.

------------------------------------------------------

## v3.2.2
release date:	2020/07/31

>更新日志：
1.默认ivIndex由0x1223344修改0x0。
2.优化SDK公开的头文件，缩短SDK的文件夹路径。
3.APP支持static OOB数据导入功能。
4.MeshInfo界面新增按钮用于是否启用功能：如未在数据库找到支持static OOB设备的OOB数据时，尝试使用no OOB的发送进行设备的添加。
5.更新json文件数据结构。
>
>release note:
1.fix: change default ivIndex from 0x11223344 to 0x0.
2.fix: cut down file path,  current max size of file path is 118.
3.feat: add feature: support static oob database importing and use static oob if target device supported.
4.fix: add enable button of `add static OOB device by no OOB provision` in `Mesh Info` ViewController.
5.fix: update new json format.

------------------------------------------------------

## v3.2.1
release date:	2020/05/12

>更新日志：
1.源码库分为分为基础版本库和扩展库两个库，基础库不包含meshOTA和remote provision功能。.
2.修复remote provision多跳时添加异常的bug。
>
>release note:
1.delete source code file of `meshOTA` and `remote provision`, just reserved header file of them in default library `TelinkSigMeshLib`.
2.fix: multi-hop bug of remote provision.

------------------------------------------------------

## v3.2.0
release date:	update 2020/04/29

>更新日志：
1.降低OpenSSL文件大小。
2.GATT OTA需要执行nodeIdentifySet并根据nodeIdentity来直连设备。
3.上传源码版本SDK，更新源码版本SDK的接口说明文档。
>
>release note:
1.cut down the size of OpenSSL file.
2.add nodeIdentifySet in GATT OTA.
3.add sourceCode library TelinkSigMeshLib, update doc of sourceCode library TelinkSigMeshLib.

------------------------------------------------------

SigMesh iOS SDK of Telink Semiconductor (Shanghai) Co., Ltd.