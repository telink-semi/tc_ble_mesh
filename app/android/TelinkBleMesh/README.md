# TelinkBleMesh

## About
SIG mesh demo app based on Bluetooth Mesh Spec.
 

 ## Test Environment
 + Telink SIG Mesh Device
 + Android phone (4.3+)
 
------------------------------------

## Start

1. Open project int Android Studio : file -> open -> select TelinkBleMesh;
 Or import library in project in android studio: file -> new -> import module -> select TelinkBleMeshLib.

2. Create custom Application extends `MeshApplication`;
    Or create a custom class to deal with `EventHandler`(generally post event, More details bout EventHandler see  [details](#detail_event_handler))
3. Invoke `MeshService#init` for library initializing; 
    And invoke `MeshService#setupMeshNetwork(MeshConfiguration)` for networking setup(more details about MeshConfiguration see [details](#detail_config))
---
## Networking Interface

### Provisioning
1. Listening to these events :
   + `ScanEvent.EVENT_TYPE_DEVICE_FOUND` (when unprovisioned device found)
   + `ScanEvent.EVENT_TYPE_SCAN_TIMEOUT` (when no device found)
   + `ProvisioningEvent.EVENT_TYPE_PROVISION_SUCCESS` (when device provision succeed)
   + `ProvisioningEvent.EVENT_TYPE_PROVISION_FAIL` (when device provision fail)
2. Invoke `MeshService#startScan` to scan for  unprovisioned devices; 
3. When device found, invoke `MeshService#startProvisioning` with allocated unicast address and static-OOB data (Optional) . Provisioning event will be posted when complete;

### Binding
Binding means add application key to the provisioned node and bind this application key with target models.
1. Listening to these events :
   + `BindingEvent.EVENT_TYPE_BIND_SUCCESS` (when bind succeed)
   + `BindingEvent.EVENT_TYPE_BIND_FAIL` (when bind fail)
2. Invoke `MeshService#startBinding`


### Network control
+ Send mesh message: 
   - invoke `MeshService#sendMeshMessage` to send common model message ,config model message, vendor model message. 
   For example, send `OnOffSetMessage` to set node on/off state
     (more details about mesh message see [details](#detail_mesh_message))        
+ Receive status message: 
   - when ack message sent , device status changed, device status publish ,, status message will be received. Meanwhile `StatusNotificationEvent` will be assembled and post. The `eventType` in StatusNotificationEvent is determined by MeshStatus#Container , generally it is the status message class name; For example, on off status event type is `OnOffStatusMessage.class.getName()`

### Device OTA (GATT)
1. Listening to these events :
    + `GattOtaEvent.EVENT_TYPE_OTA_SUCCESS` (when OTA succeed)
    + `GattOtaEvent.EVENT_TYPE_OTA_PROGRESS` (when OTA progress update)
    + `GattOtaEvent.EVENT_TYPE_OTA_FAIL` (when OTA fail)
2. Invoke `MeshService#startGattOta`

### Extension

##### Default bound
This is Telink private action for faster application key binding, only appllication key adding action will be executed.
+ Set `BindingDevice#defaultBound` by true to enable default bound when start binding. 
##### Fast provision
This is Telink private provisioning flow for faster and wider networking.
###### Precondition: 
1. Device is fast-provision supported 
2. Device's composition-data is known
###### Work flow
1. Listening to these events :
    + `FastProvisioningEvent.EVENT_TYPE_FAST_PROVISIONING_ADDRESS_SET` (when set device address complete)
    + `FastProvisioningEvent.EVENT_TYPE_FAST_PROVISIONING_FAIL` (when fast provision fail)
    + `FastProvisioningEvent.EVENT_TYPE_FAST_PROVISIONING_SUCCESS` (when fast provision succeed)
2. Invoke `MeshService.#startFastProvision` and waiting for `FastProvisioningEvent`

##### Remote provision
###### Precondition: 
1. Device is remote-provision supported; 
2. If no proxy device in mesh is connected, normal provisioning and binding is needed before remote-provisioning
###### Work flow:
1. Listening to these events :
    + `ScanReportStatusMessage` (when device found by remote scan)
    + `RemoteProvisioningEvent.EVENT_TYPE_REMOTE_PROVISIONING_SUCCESS` (when remote provision succeed)
    + `RemoteProvisioningEvent.EVENT_TYPE_REMOTE_PROVISIONING_FAIL` (when remote provision fail)
    
2. Send mesh message `ScanStartMessage`, and collect scan report when handling `ScanReportStatusMessage`
3. Invoke `MeshService#startRemoteProvisioning`, and waiting for `RemoteProvisioningEvent`
##### Mesh OTA (mesh firmware update)
###### Precondition:
1. Device is mesh-OTA supported;
2. Firmware bin;
##### Work flow:
1. Listening to these events :
    + `FirmwareInfoStatusMessage` (when device firmware version info received)
    + `MeshUpdatingEvent.EVENT_TYPE_UPDATING_PREPARED` (when mesh ota prepared -- before sending block)
    + `MeshUpdatingEvent.EVENT_TYPE_UPDATING_SUCCESS` (when mesh ota succeed)
    + `MeshUpdatingEvent.EVENT_TYPE_UPDATING_FAIL` (when mesh ota fail)
    + `MeshUpdatingEvent.EVENT_TYPE_UPDATING_PROGRESS` (when mesh ota progress update)
    + `MeshUpdatingEvent.EVENT_TYPE_DEVICE_SUCCESS` (when device update succeed)
    + `MeshUpdatingEvent.EVENT_TYPE_DEVICE_FAIL` (when device update fail)
2. Invoke `MeshService#startMeshOta`, and waiting for `MeshUpdatingEvent`
#### Details
<span id="detail_config"></span>
##### MeshConfiguration : 
+ networkKey : network key
+ netKeyIndex : network key index
+ appKeyMap : application key and index map, at least one element
+ ivIndex : iv-index
+ sequenceNumber : sequence-number
+ localAddress : provisioner address, should be valid unicast address (0x0001 - 0x7FFF)
+ deviceKeyMap : provisioned device address and deviceKey map, updated when device provisioned or device removed
 
<span id="detail_mesh_message"></span>
##### MeshMessage:
+ opcode : opcode of access layer
<span id="detail_mesh_message_params"></span>
+ params : params of access layer
+ destinationAddress : destination address of network layer
+ sourceAddress : source address of network layer, **managed by lib**, valued by `MeshConfiguration#localAddress`
+ accessType : define if message is DEVICE or APPLICATION
+ accessKey : encryption key used when upper transport encryption, **managed by lib**, if accessType is DEVICE , accessKey will be valued by device key , otherwise be valued by application key
+ appKeyIndex: only used when accessType==APPLICATION 
+ ctl : 0 for access message, 1 for control message
+ ttl : message ttl, default 10

+ responseOpcode : used in **reliable message**,defines expected response opcode , such as on/off-get-message expect on/off-status-message 
+ responseMax : used in **reliable message**,defines expected response max count, if no response received or response count is less than max, retry is needed
+ retryCnt : used in **reliable message**,defines max retry count if retry needed

+ tidPosition : used in **tid message**,defines tid position in [params](#detail_mesh_message_params), if it is valid position, target byte will be replaced by an auto-increment value in lib

<span id="detail_event_handler"></span>
##### EventHandler:
+ NetworkInfoUpdateEvent : posted when iv-index or sequence-number updated, 
    new value should be saved in ORM
+ OnlineStatusEvent : posted when online-status(telink private status) received  
+ StatusNotificationEvent : posted when mesh status notification received; 
    Its inner eventType is determined by the StatusMessage (registered in MeshStatus) 
+ MeshEvent : posted when mesh networking status changed
+ Action events are posted when action status changed:
    + ScanEvent
    + ProvisioningEvent
    + BindingEvent
    + AutoConnectEvent
    + GattOtaEvent
    + FastProvisioningEvent
    + RemoteProvisioningEvent
    + MeshUpdatingEvent