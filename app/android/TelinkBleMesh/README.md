Telink SIG mesh android app

# About
SIG mesh demo app based on Bluetooth Mesh Spec. 

 # Test Environment
 + Telink Mesh Device
 + Android phone (4.3+)

# Interface

## Library import

1. Open project int android studio : file -> open -> select TelinkBleMesh; Or import library in project in android studio: file -> new -> import module.

2. Create custom Application extends `MeshApplication`;
    Or create a custom class to deal with `EventHandler`(generally post event), refer to MeshApplication. (More details bout EventHandler see  [detail](#detail_event_handler))
3. Invoke `MeshService#init` for initlizing, and invoke `MeshService#setupMeshNetwork` for networking setup; (more deail about MeshConfiguration see [detail](#detail_config))

## Networking

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
2. Invoke `MeshService#startBinding` . 

##### tips: provisioning and binding sample code can be found in DeviceProvisionActivity in demo;

### Network control
+ Send mesh message: 
   - invoke `MeshService#sendMeshMessage` to send common model message ,config model message, vendor model message. For example, send `OnOffSetMessage` to set node on/off state ; 
+ Receive status message: 
   - when ack message sent , device status changed, device status publish ,, status message will be received. Meanwhile `StatusNotificationEvent` will be assembled and post. The `eventType` in StatusNotificationEvent is determined by MeshStatus#Container , generally it is the status message class name; For example, on off status event type is `OnOffStatusMessage.class.getName()`

### Device OTA (GATT)
1. Listening to these events :
    + `GattOtaEvent.EVENT_TYPE_OTA_SUCCESS` (when OTA succeed)
    + `GattOtaEvent.EVENT_TYPE_OTA_PROGRESS` (when OTA progress update)
    + `GattOtaEvent.EVENT_TYPE_OTA_FAIL` (when OTA fail)
2. Invoke `MeshService#startGattOta`

### Extension

#### Default bound
This is Telink custom action for faster application key binding, only appllication key adding action will be executed.
+ Set `BindingDevice#defaultBound` by true to enable default bound when start binding. 
#### Fast provision
This is Telink custom provisioning flow for faster and wider networking.
+  
#### Remote provision

#### Mesh OTA (mesh firmware update)


<span id="detail_event_handler"></span>
##### EventHandler:
    

<span id="detail_config"></span>
##### MeshConfiguration : 
    
    
