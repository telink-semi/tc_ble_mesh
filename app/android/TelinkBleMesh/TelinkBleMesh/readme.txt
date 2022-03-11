

direct forwarding 功能
1. 实现 DIRECTED_CONTROL_GET，DIRECTED_CONTROL_SET， DIRECTED_CONTROL_STATUS
    FORWARDING_TABLE_ADD， FORWARDING_TABLE_DELETE， FORWARDING_TABLE_STATUS

2. App需要保存信息， 不需要保存在Json文件中


DF, private beacon, enhanced opcode aggregator 都要放在 DraftFeature 里面

control


2022 0120 todo 如果收到的是 private beacon， 回复的也要是 private beacon




// todo
1. 添加标志位： 判断分享过来的json文件中是否包含 ivIndex；
    如果不包含， 则在获取到最新的ivIndex时， 不允许组网
2. 空网络不允许分享

商工：
@梁家誌 @柯昌维 @少年已不再年少 关于 去掉 IVI 的规则，有什么需要补充的 不 ？
json 文件 按没有 ivi 的方案实现：
VC 工具 的 IVI 初始值由 原来的 0x11223344 改为 0x00000001 (取非对称值)
导出的 JSON 文件需要按spec 去掉 IVI,
分享导出前，除本身外，如果没有其他节点(即空网络)，则不允许导出，并弹框提示“not allow to share empty network”。
分享得到的key后，(APP / VC) 在没有从 beacon 包获取到 IV 之前，不能组网，如果客户点击组网，则弹框提示“connect to the current network before add nodes”。


//
OPCODES_AGGREGATOR_SEQUENCE

    device key : ttl get /  friend get / relay get

    app key: 开灯 / 亮度调节到50


    key bind 启用 aggregator， 放在draft feature中
    1. get cps

    2. 判断是否包含 aggregator server

    3. app key add 和 key bind


// pin 码



version record:


V3.3.4 (skip V3.3.3 that only update firmware)

1. add support for switch devices
2. add support for subnet bridge
3. update mesh OTA, supports app and directly connected nodes as distributor
4. update JSON database to version R11
5. add device config page in device setting, used for send and receive config messages

----------------

V3.3.2 (skip V3.3.1 that only update firmware)

1. Add support for subnet bridge in draft feature;
2. Add support for certificate based provisioning in draft feature;
3. Add support for multiple network key and application key in mesh info;
4. Update local address range from 0x0100 to 0x0400.

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


