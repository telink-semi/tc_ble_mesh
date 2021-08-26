
直连节点meshOTA升级时, 强制使用 长包模式, 这样可以快速发包.
 例如












TODO : 1. network transmit , relay, ttl

log 改为默认打开

3.3.2@RC01
内部测试版本: 添加遥控器设备支持, 包名为 com.telink.ble.mesh.demo.rmt
    1.  遥控器设备, 添加可以配置 publish client model 功能 , 默认为四个
    2.  长按 remote, 需要直连到 遥控器设备, 退出后需要断开遥控器设备的连接

3.3.2@RC02
内部测试版本:  distribute start 指令中的 multicast Address 只有一个节点且为LPN时才设置为0, 其它设置为group address

3.3.2@RC03
内部测试版本:
    1. 更新遥控器设备连接规则: 添加MeshAdvFilter选项, 用于只连接nodeIdentity设备;
    2. 使能页面中modelId 可编辑状态, 并校验modelId是否存在

3.3.2@RC04
内部测试版本: 0816 10:07
    1. 直连节点meshOTA升级时, 强制使用 长包模式, 这样可以快速发包. 判断opcode 为 0x7D, 则...
    2. mesh light verify only 模式失败: 可能是app 发送apply时间早了点, 改为在 多次发送
    3. blobTransfer 中 TransferMode 如果同时支持 pull, push, 那优先选择push, 并且 chunkSize 强制改为 208


/// 添加遥控器设备类型:



// todo additional info 为 3 时, 不等回读版本号成功


// todo meshMessage 中添加对retry 间隔的配置

添加extend bearer


// meshOTA
main activity 中注释了 autoConnect
meshOTA activity 修改了start方法

meshMessage, 添加messageId,  中添加callback, 用来获取 回复和 消息发送完成(boolean: 是否发送成功)的状态


version record

subnet bridge:
1. App 需添加net add/remove 等管理
2. JSON 文件导出时, 需选择 network key. 不同的network key 代表不同的 subnet
3.

1. Enable Subnet Bridge
2. Bridging Table

// todo
需要有一个回复状态的统计, 发送全开全关指令, 统计回复所有设备状态的时间



300次开关灯, 500ms interval.





mesh OTA 进度:
1. initiator 和 distributor的流程基本梳理完成
2. firmware distribution (FD) 相关的 message(firmwaredistribution包中) 已经创建完成, 细节方面, 如 注释, 消息opcode, rspOpcode 都未检查;
3. 需打通 initiator , distributorType , updating nodes 各角色间的消息流转
4. 流程参考  firmware_update_flow.txt 文件

在 initiator 和 distributor 中都持有一个BlobTransfer

todo:
1. MeshOTA 页面中添加 app as distributorType
2. MeshOTA 页面中检查直连设备设备是否支持 distributorType, 如果设备不支持, 则使用app作为 distributorType



V3.3.2 (skip V3.3.1 that only update firmware)

1. Add support for subnet bridge in draft feature;
    draft feature中添加对subnet bridge 支持
2. Add support for certificate based provisioning in draft feature;
    draft feature中添加对certificate based 组网支持
3. Add support for multiple network key and application key in mesh info;
    添加对多network key 和 application key 支持
4. Update local address range from 0x0100 to 0x0400.
    本地组网地址区间从 0x0100 改为 0x0400

----------------

V3.3.0

1. Add DLE mode extend bearer support for sending long unsegmented mesh packet;
2. Add selectable device scanning mode;
3. Update color select UI in HSL mode.

----------------

V3.2.3

SIG mesh android app V3.2.3 release notes:
1. Add startGattConnection interface in MeshService for connect target mesh node, connection state will be uploaded by GattConnectionEvent
2. Add pid info  before OTA/MeshOTA;

----------------

V3.2.2

1. fix device provision timeout failure if device static-oob data not found when device support static-oob
2. fix app key binding failure when target vendor model does not support app key encryption
3. update json storage format

----------------

V3.2.1
1. support static oob database importing;
2. delete mesh OTA and remote provision;

----------------

V3.2.0

1. Switch from c-lib edition to java-source edition;
2. Update firmware-update flow according to R04-LBL35 version;
3. Optimize remote-provision;
4. Change transition time from none to default when sending command;
5. Add qrcode share by cloud.


// draft feature
