tip: update ninja if "too many path components" error occurs on windows

V3.1.6
1. change ttl from 1 to 10;

==================================================================


V3.1.3
1. Fix crash error when entering remote provisioning page;
2. Fix no device found error when remote scanning;

==================================================================

V3.1.1
1. remove rsv in MeshOTAModelStorage;
2. update static lib

==================================================================

V3.1.0
1. LPN node control: add on/off function;
2. mesh.json update: provisioner AddressRange changed to 1024 from 256;
3. add time publish if node contains time model when scanning and send time status when connect success;
4. update static lib

==================================================================

V3.0.0
1. Add nodeIdentity/networkId support on autoConnect mode;
2. Add fastProvision function;

==================================================================

V2.9.0
1. Fix state error when mesh ota processing;
2. Remove flash file in TelinkSigMeshLib, replaced by data store/load interface;
3. Add heartbeat status notification callback;
4. Update static lib;

==================================================================


V2.8.2
1. Add RGB slider in HSL device control page;
2. Add Mesh OTA func， click setting->Mesh OTA to enter Mesh OTA page;
3. Fix kicking and grouping error when app reopened;

==================================================================

V2.8.1

1. add online status function:
    when connect mesh success , app will auto enable online status if device contains target UUID;
2. add permission check in app;
3. update remote provision flow;
4. When import and compile project, if "too many path components" error occurs, updating ninja tool is needed.
    There is available version in attach, users can also download new version at  https://github.com/ninja-build/ninja/releases website;
