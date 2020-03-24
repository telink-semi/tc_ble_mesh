package com.telink.ble.mesh.ui;

import android.os.Bundle;
import android.os.Handler;
import android.os.ParcelUuid;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;

import com.telink.ble.mesh.SharedPreferenceHelper;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.access.BindingBearer;
import com.telink.ble.mesh.core.ble.MeshScanRecord;
import com.telink.ble.mesh.core.ble.UUIDInfo;
import com.telink.ble.mesh.core.message.NotificationMessage;
import com.telink.ble.mesh.core.message.rp.ScanReportStatusMessage;
import com.telink.ble.mesh.core.message.rp.ScanStartMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.entity.AdvertisingDevice;
import com.telink.ble.mesh.entity.BindingDevice;
import com.telink.ble.mesh.entity.CompositionData;
import com.telink.ble.mesh.entity.ProvisioningDevice;
import com.telink.ble.mesh.entity.RemoteProvisioningDevice;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.BindingEvent;
import com.telink.ble.mesh.foundation.event.MeshEvent;
import com.telink.ble.mesh.foundation.event.ProvisioningEvent;
import com.telink.ble.mesh.foundation.event.RemoteProvisioningEvent;
import com.telink.ble.mesh.foundation.event.ScanEvent;
import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;
import com.telink.ble.mesh.foundation.parameter.BindingParameters;
import com.telink.ble.mesh.foundation.parameter.ProvisioningParameters;
import com.telink.ble.mesh.foundation.parameter.ScanParameters;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.PrivateDevice;
import com.telink.ble.mesh.ui.adapter.DeviceProvisionListAdapter;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;

import java.util.ArrayList;
import java.util.List;

import androidx.appcompat.widget.Toolbar;
import androidx.collection.ArraySet;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

/**
 * remote provision
 * actions:
 * 1. remote scan ->
 * 2. remote scan rsp, remote device found <-
 * 3. start remote provision ->
 * 4. remote provision event (if success , start key-binding) <-
 * 5. remote scan -> ...
 */

public class RemoteProvisionActivity extends BaseActivity implements View.OnClickListener, EventListener<String> {

    private MeshInfo meshInfo;

    /**
     * ui devices
     */
    private List<NodeInfo> devices = new ArrayList<>();

    private DeviceProvisionListAdapter mListAdapter;

    /**
     * scanned devices timeout remote-scanning
     */
    private ArraySet<RemoteProvisioningDevice> remoteDevices = new ArraySet<>();

    private Handler delayHandler = new Handler();

    /**
     * server address used in remote-scanning and remote-provisioning
     */
    private int serverAddress;

    private boolean proxyComplete = false;

    private NodeInfo directDevice;

    private Button btn_back;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_device_provision);
        initTitle();
        RecyclerView rv_devices = findViewById(R.id.rv_devices);
        devices = new ArrayList<>();

        mListAdapter = new DeviceProvisionListAdapter(this, devices);
        rv_devices.setLayoutManager(new GridLayoutManager(this, 2));
        rv_devices.setAdapter(mListAdapter);
        btn_back = findViewById(R.id.btn_back);
        btn_back.setOnClickListener(this);

        meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_DISCONNECTED, this);

        TelinkMeshApplication.getInstance().addEventListener(ScanReportStatusMessage.class.getName(), this);
        TelinkMeshApplication.getInstance().addEventListener(RemoteProvisioningEvent.EVENT_TYPE_REMOTE_PROVISIONING_SUCCESS, this);
        TelinkMeshApplication.getInstance().addEventListener(RemoteProvisioningEvent.EVENT_TYPE_REMOTE_PROVISIONING_FAIL, this);

        // event for normal provisioning
        TelinkMeshApplication.getInstance().addEventListener(ProvisioningEvent.EVENT_TYPE_PROVISION_SUCCESS, this);
        TelinkMeshApplication.getInstance().addEventListener(ProvisioningEvent.EVENT_TYPE_PROVISION_FAIL, this);
        TelinkMeshApplication.getInstance().addEventListener(BindingEvent.EVENT_TYPE_BIND_SUCCESS, this);
        TelinkMeshApplication.getInstance().addEventListener(BindingEvent.EVENT_TYPE_BIND_FAIL, this);
        TelinkMeshApplication.getInstance().addEventListener(ScanEvent.EVENT_TYPE_SCAN_TIMEOUT, this);
        TelinkMeshApplication.getInstance().addEventListener(ScanEvent.EVENT_TYPE_DEVICE_FOUND, this);

        actionStart();
    }

    private void initTitle() {
        Toolbar toolbar = findViewById(R.id.title_bar);
        toolbar.inflateMenu(R.menu.device_scan);
        setTitle("Device Scan(Remote)");

        MenuItem refreshItem = toolbar.getMenu().findItem(R.id.item_refresh);
        refreshItem.setVisible(false);
        toolbar.setNavigationIcon(null);
    }

    private void actionStart() {
        enableUI(false);

        for (NodeInfo nodeInfo : meshInfo.nodes) {
            nodeInfo.selected = false;
        }

        boolean proxyLogin = MeshService.getInstance().isProxyLogin();
        MeshLogger.log("remote provision action start: login? " + proxyLogin);
        if (proxyLogin) {

            int directAddress = MeshService.getInstance().getDirectConnectedNodeAddress();

            this.directDevice = meshInfo.getDeviceByMeshAddress(directAddress);
//            this.directDevice = getAnotherDevice(directAddress);
//            this.serverAddress = directDevice.meshAddress;
            this.directDevice.selected = false;
            proxyComplete = true;
            startRemoteScan();
        } else {
            proxyComplete = false;
            startScan();
        }
    }

    // for test
    private NodeInfo getAnotherDevice(int directAddress) {
        for (NodeInfo nodeInfo : meshInfo.nodes) {
            if (nodeInfo.meshAddress != directAddress) {
                return nodeInfo;
            }
        }
        return null;
    }

    private void enableUI(boolean enable) {
        btn_back.setEnabled(enable);
    }

    /******************************************************************************
     * normal provisioning
     ******************************************************************************/
    private void startScan() {
        ScanParameters parameters = ScanParameters.getDefault(false, true);
        if (devices.size() != 0) {
            String[] excludeMacs = new String[devices.size()];
            for (int i = 0; i < devices.size(); i++) {
                excludeMacs[i] = devices.get(i).macAddress;
            }
            parameters.setExcludeMacs(excludeMacs);
        }
        parameters.setScanTimeout(10 * 1000);
        MeshService.getInstance().startScan(parameters);
    }

    private void onDeviceFound(AdvertisingDevice advertisingDevice) {
        int address = meshInfo.provisionIndex;

        MeshLogger.log("alloc address: " + address);
        if (address == -1) {
            enableUI(true);
            return;
        }

        NodeInfo nodeInfo = new NodeInfo();
        nodeInfo.meshAddress = meshInfo.provisionIndex;
        nodeInfo.macAddress = advertisingDevice.device.getAddress();
        nodeInfo.state = NodeInfo.STATE_PROVISIONING;

        devices.add(nodeInfo);
        mListAdapter.notifyDataSetChanged();

        ProvisioningDevice provisioningDevice = new ProvisioningDevice(advertisingDevice, address);
        ProvisioningParameters provisioningParameters = new ProvisioningParameters(provisioningDevice);
        MeshService.getInstance().startProvisioning(provisioningParameters);
    }

    private void onProvisionSuccess(ProvisioningEvent event) {

        ProvisioningDevice remote = event.getProvisioningDevice();

        NodeInfo nodeInfo = getNodeByMac(remote.getAdvertisingDevice().device.getAddress());

        if (nodeInfo == null) return;

        nodeInfo.state = NodeInfo.STATE_BINDING;
        int elementCnt = remote.getDeviceCapability().eleNum;
        nodeInfo.elementCnt = elementCnt;
        nodeInfo.deviceKey = remote.getDeviceKey();
        meshInfo.insertDevice(nodeInfo);
        meshInfo.provisionIndex += elementCnt;
        meshInfo.saveOrUpdate(RemoteProvisionActivity.this);

        // check if private mode opened
        final boolean privateMode = SharedPreferenceHelper.isPrivateMode(this);

        // check if device support fast bind
        boolean defaultBound = false;
        if (privateMode && remote.getAdvertisingDevice().scanRecord != null) {
            PrivateDevice device = getPrivateDevice(remote.getAdvertisingDevice().scanRecord);
            if (device != null) {
                MeshLogger.log("private device");
                final byte[] cpsData = device.getCpsData();
                nodeInfo.compositionData = CompositionData.from(cpsData);
                defaultBound = true;
            } else {
                MeshLogger.log("private device null");
            }
        }

        nodeInfo.setDefaultBind(defaultBound);
        mListAdapter.notifyDataSetChanged();
        int appKeyIndex = meshInfo.getDefaultAppKeyIndex();
        BindingDevice bindingDevice = new BindingDevice(nodeInfo.meshAddress, nodeInfo.macAddress, appKeyIndex);
        bindingDevice.setDefaultBound(defaultBound);
        MeshService.getInstance().startBinding(new BindingParameters(bindingDevice));
    }

    private void onProvisionFail(ProvisioningEvent event) {
        ProvisioningDevice deviceInfo = event.getProvisioningDevice();

        NodeInfo pvDevice = getNodeByMac(deviceInfo.getAdvertisingDevice().device.getAddress());
        if (pvDevice == null) return;
        pvDevice.state = NodeInfo.STATE_PROVISION_FAIL;
        pvDevice.stateDesc = event.getDesc();
        mListAdapter.notifyDataSetChanged();
    }

    private void onKeyBindSuccess(BindingEvent event) {
        BindingDevice remote = event.getBindingDevice();

        NodeInfo deviceInList = getNodeByMac(remote.getMacAddress());
        if (deviceInList == null) return;

        deviceInList.state = NodeInfo.STATE_BIND_SUCCESS;

        // if is default bound, composition data has been valued ahead of binding action
        if (!remote.isDefaultBound()) {
            deviceInList.compositionData = remote.getCompositionData();
        }

        mListAdapter.notifyDataSetChanged();
        meshInfo.saveOrUpdate(RemoteProvisionActivity.this);
    }

    private void onKeyBindFail(BindingEvent event) {
        BindingDevice remote = event.getBindingDevice();
        NodeInfo deviceInList = getNodeByMac(remote.getMacAddress());
        if (deviceInList == null) return;

        deviceInList.state = NodeInfo.STATE_BIND_FAIL;
        deviceInList.stateDesc = event.getDesc();
        mListAdapter.notifyDataSetChanged();
        meshInfo.saveOrUpdate(RemoteProvisionActivity.this);
    }


    /******************************************************************************
     * remote provisioning
     ******************************************************************************/
    private void startRemoteScan() {
        // scan for max 2 devices
        final byte SCAN_LIMIT = 2;
        // scan for 3 seconds
        final byte SCAN_TIMEOUT = 8;
        int availableServerAddress = getAvailableServerAddress();
        if (availableServerAddress == -1) {
            MeshLogger.log(TAG + " -- ALL device did remote scan !!!");
            enableUI(true);
            return;
        }

        this.serverAddress = availableServerAddress;
        ScanStartMessage remoteScanMessage = ScanStartMessage.getSimple(serverAddress, meshInfo.getDefaultAppKeyIndex(),
                1, SCAN_LIMIT, SCAN_TIMEOUT);
        MeshService.getInstance().sendMeshMessage(remoteScanMessage);

        delayHandler.postDelayed(remoteScanTimeoutTask, (SCAN_TIMEOUT + 1) * 1000);
    }

    private void onRemoteComplete() {
        if (!MeshService.getInstance().isProxyLogin()) {
            enableUI(true);
            return;
        }
        if (remoteDevices.size() > 0) {
            remoteDevices.removeAt(0);
        }

        if (remoteDevices.size() == 0) {
            startRemoteScan();
        } else {
            delayHandler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    provisionNextRemoteDevice(remoteDevices.valueAt(0));
                }
            }, 500);

        }
    }


    /**
     * nodeInfo.selected used as did-remote-scan
     */
    private int getAvailableServerAddress() {
        if (directDevice != null && !directDevice.selected) {
            return directDevice.meshAddress;
        }
        if (devices == null) return -1;
        for (NodeInfo deviceInfo : devices) {
            if (deviceInfo.state == NodeInfo.STATE_BIND_SUCCESS &&
                    deviceInfo.getOnOff() != NodeInfo.ON_OFF_STATE_OFFLINE &&
                    !deviceInfo.selected) {
                return deviceInfo.meshAddress;
            }
        }
        return -1;
    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
        TelinkMeshApplication.getInstance().removeEventListener(this);
        delayHandler.removeCallbacksAndMessages(null);
    }

    private void onRemoteDeviceScanned(int src, ScanReportStatusMessage scanReportStatusMessage) {
//        MeshLogger.log("on remote device found: " + Arrays.bytesToHexString(scanReportStatusMessage.getUuid(), ":"));
        RemoteProvisioningDevice remoteProvisioningDevice = new RemoteProvisioningDevice(scanReportStatusMessage, src);
        if (!remoteProvisioningDevice.getMac().contains("11:22:33:11:22")) return;

        NodeInfo nodeInfo = getNodeByMac(remoteProvisioningDevice.getMac());
        if (nodeInfo != null) {
            return;
        }

        MeshLogger.log("remote device found: " + Arrays.bytesToHexString(remoteProvisioningDevice.getUuid()));
        remoteDevices.add(remoteProvisioningDevice);
    }


    private void provisionNextRemoteDevice(RemoteProvisioningDevice device) {
        MeshLogger.log(String.format("provision next: server -- %04X uuid -- %s",
                device.getServerAddress(),
                Arrays.bytesToHexString(device.getUuid())));
        int address = meshInfo.provisionIndex;
        if (address > MeshUtils.UNICAST_ADDRESS_MAX) {
            enableUI(true);
            return;
        }
        device.setUnicastAddress(address);
        NodeInfo nodeInfo = new NodeInfo();
        nodeInfo.macAddress = device.getMac();
        nodeInfo.meshAddress = address;
        nodeInfo.state = NodeInfo.STATE_PROVISIONING;
        devices.add(nodeInfo);
        mListAdapter.notifyDataSetChanged();

        MeshService.getInstance().startRemoteProvisioning(device);
    }

    private Runnable remoteScanTimeoutTask = new Runnable() {
        @Override
        public void run() {
            if (remoteDevices.size() == 0) {
                MeshLogger.log("no device found by: " + serverAddress);
                // set device didRemoteScan = true
                updateDeviceState();
                // scan device by next server
                startRemoteScan();
            } else {
                MeshLogger.log("remote devices scanned: " + remoteDevices.size());
                provisionNextRemoteDevice(remoteDevices.valueAt(0));
            }
        }
    };

    /**
     *
     */
    private void updateDeviceState() {
        if (directDevice != null && directDevice.meshAddress == serverAddress) {
            directDevice.selected = true;
            return;
        }
        if (devices != null) {
            for (NodeInfo device : devices) {
                if (device.meshAddress == serverAddress) {
                    device.selected = true;
                }
            }
        }
    }


    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.btn_back) {
            finish();
        }
    }

    @Override
    public void performed(final Event<String> event) {
        String eventType = event.getType();
        if (eventType.equals(ScanReportStatusMessage.class.getName())) {
            NotificationMessage notificationMessage = ((StatusNotificationEvent) event).getNotificationMessage();
            ScanReportStatusMessage scanReportStatusMessage = (ScanReportStatusMessage) notificationMessage.getStatusMessage();
            onRemoteDeviceScanned(notificationMessage.getSrc(), scanReportStatusMessage);
        }

        // remote provisioning
        else if (eventType.equals(RemoteProvisioningEvent.EVENT_TYPE_REMOTE_PROVISIONING_FAIL)) {
            onRemoteProvisioningFail((RemoteProvisioningEvent) event);
            onRemoteComplete();
        } else if (eventType.equals(RemoteProvisioningEvent.EVENT_TYPE_REMOTE_PROVISIONING_SUCCESS)) {
            delayHandler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    onRemoteProvisioningSuccess((RemoteProvisioningEvent) event);
                }
            }, 3000);

        }

        // normal provisioning
        else if (eventType.equals(ProvisioningEvent.EVENT_TYPE_PROVISION_SUCCESS)) {
            onProvisionSuccess((ProvisioningEvent) event);
        } else if (eventType.equals(ScanEvent.EVENT_TYPE_SCAN_TIMEOUT)) {
            enableUI(true);
        } else if (eventType.equals(ProvisioningEvent.EVENT_TYPE_PROVISION_FAIL)) {
            onProvisionFail((ProvisioningEvent) event);
            startScan();
        } else if (eventType.equals(ScanEvent.EVENT_TYPE_DEVICE_FOUND)) {
            AdvertisingDevice device = ((ScanEvent) event).getAdvertisingDevice();
            onDeviceFound(device);
        }

        // remote and normal binding
        else if (eventType.equals(BindingEvent.EVENT_TYPE_BIND_SUCCESS)) {
            onKeyBindSuccess((BindingEvent) event);
            if (proxyComplete) {
                onRemoteComplete();
            } else {
                proxyComplete = true;
                startRemoteScan();
            }
        } else if (eventType.equals(BindingEvent.EVENT_TYPE_BIND_FAIL)) {
            onKeyBindFail((BindingEvent) event);
            if (proxyComplete) {
                onRemoteComplete();
            } else {
                enableUI(true);
            }
        } else if (eventType.equals(MeshEvent.EVENT_TYPE_DISCONNECTED)) {
            if (proxyComplete)
                enableUI(true);
        }
    }

    private void onRemoteProvisioningSuccess(RemoteProvisioningEvent event) {
        // start remote binding
        RemoteProvisioningDevice remote = event.getRemoteProvisioningDevice();
        MeshLogger.log("remote act success: " + remote.getMac());

        NodeInfo nodeInfo = getNodeByMac(remote.getMac());
        if (nodeInfo == null) return;

        nodeInfo.state = NodeInfo.STATE_BINDING;
        int elementCnt = remote.getDeviceCapability().eleNum;
        nodeInfo.elementCnt = elementCnt;
        nodeInfo.deviceKey = remote.getDeviceKey();
        meshInfo.insertDevice(nodeInfo);
        meshInfo.provisionIndex += elementCnt;
        meshInfo.saveOrUpdate(RemoteProvisionActivity.this);
        nodeInfo.setDefaultBind(false);
        mListAdapter.notifyDataSetChanged();
        int appKeyIndex = meshInfo.getDefaultAppKeyIndex();
        BindingDevice bindingDevice = new BindingDevice(nodeInfo.meshAddress, nodeInfo.macAddress, appKeyIndex);
        bindingDevice.setBearer(BindingBearer.Any);
        MeshService.getInstance().startBinding(new BindingParameters(bindingDevice));
    }

    private void onRemoteProvisioningFail(RemoteProvisioningEvent event) {
        //
        MeshLogger.log("remote act fail: " + event.getRemoteProvisioningDevice().getMac());

        RemoteProvisioningDevice deviceInfo = event.getRemoteProvisioningDevice();

        NodeInfo pvDevice = getNodeByMac(deviceInfo.getMac());

        if (pvDevice == null) return;
        pvDevice.state = NodeInfo.STATE_PROVISION_FAIL;
        pvDevice.stateDesc = event.getDesc();
        mListAdapter.notifyDataSetChanged();
    }

    private PrivateDevice getPrivateDevice(byte[] scanRecord) {
        if (scanRecord == null) return null;
        MeshScanRecord sr = MeshScanRecord.parseFromBytes(scanRecord);
        byte[] serviceData = sr.getServiceData(ParcelUuid.fromString(UUIDInfo.PROVISION_SERVICE_UUID.toString()));
        if (serviceData == null) return null;
        return PrivateDevice.filter(serviceData);
    }

    private NodeInfo getNodeByMac(String mac) {
        for (NodeInfo nodeInfo :
                this.devices) {
            if (mac.equals(nodeInfo.macAddress)) {
                return nodeInfo;
            }
        }
        return null;
    }
}
