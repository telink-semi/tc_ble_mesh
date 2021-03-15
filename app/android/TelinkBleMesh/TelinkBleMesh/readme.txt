
version record

subnet bridge:
1. App 需添加net add/remove 等管理
2. JSON 文件导出时, 需选择 network key. 不同的network key 代表不同的 subnet
3.

1. Enable Subnet Bridge
2. Bridging Table




mesh OTA 进度:
1. initiator 和 distributor的流程基本梳理完成
2. firmware distribution (FD) 相关的 message(firmwaredistribution包中) 已经创建完成, 细节方面, 如 注释, 消息opcode, rspOpcode 都未检查;
3. 需打通 initiator , distributor , updating nodes 各角色间的消息流转
4. 流程参考  firmware_update_flow.txt 文件

todo:
1. MeshOTA 页面中添加 app as distributor
2. MeshOTA 页面中检查直连设备设备是否支持 distributor, 如果设备不支持, 则使用app作为 distributor
3. Take Over 接管



V3.3.2 (skip V3.3.1 which only update firmware)

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
