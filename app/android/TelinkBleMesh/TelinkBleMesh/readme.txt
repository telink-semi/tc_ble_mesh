
version record:
versionName - version code - date


V4.1.0.2 - 9 - 20240722 (4.1.0.1 is used for B91)

### Bug Fixes

- feat: add support for scene configuration in multiple elements
- feat: add support for NLC and sensor
- feat: add support for device sort by name and address
- feat: add support for setting direct forwarding in batches
- feat: add support for on-demand gatt proxy and solicitation pdu
- perf: optimized the fast provision function and related event reporting
- fix: fix mesh OTA error that caused by the receiver index  error

----------------

V4.1.0.0 - 7 - 20231108

- Add support for multi-network, users can create, delete, switch networks. The network data store format is changed from serialize to objectBox
- Add the network subpage to display network information and related control functions.
- Add support for private beacon and added related configuration page
- Add support for enhanced feature: large cps, on-demand, sar and solicitation
- Add the default root certificate for verifying certificate based devices
- Optimize the setting page to display different configuration items by category
- Improve code comments

----------------


V3.3.3.7 - 6 - 20221130

1. update icon
2. update app name to TelinkSigMesh

----------------

V3.3.3.6

1. remove inIndex in JSON storage
2. update icon generation by check pid bitmask
3. update app icon
4. support B91 SIG MESH SDK 和 B85 SIG MESH SDK

----------------

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


