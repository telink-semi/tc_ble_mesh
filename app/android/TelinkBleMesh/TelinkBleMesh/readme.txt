

version record:

V3.3.3.5

1. add support for private beacon and enhanced opcode aggregator in draft feature

----------------

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


