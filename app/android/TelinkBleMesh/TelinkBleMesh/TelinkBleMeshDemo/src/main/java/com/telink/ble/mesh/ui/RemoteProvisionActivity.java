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
import com.telink.ble.mesh.core.ble.UuidInfo;
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
import com.telink.ble.mesh.util.TelinkLog;

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
     * scanned devices during remote-scanning
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
        boolean proxyLogin = MeshService.getInstance().isProxyLogin();
        TelinkLog.d("remote provision action start: login? " + proxyLogin);
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

        TelinkLog.d("alloc address: " + address);
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
                TelinkLog.d("private device");
                final byte[] cpsData = device.getCpsData();
                nodeInfo.compositionData = CompositionData.from(cpsData);
                defaultBound = true;
            } else {
                TelinkLog.d("private device null");
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
        final byte SCAN_TIMEOUT = 3;
        int availableServerAddress = getAvailableServerAddress();
        if (availableServerAddress == -1) {
            TelinkLog.d(TAG + " -- ALL device did remote scan !!!");
            enableUI(true);
            return;
        }

        this.serverAddress = availableServerAddress;
        ScanStartMessage remoteScanMessage = ScanStartMessage.getSimple(serverAddress, meshInfo.getDefaultAppKeyIndex(),
                0, SCAN_LIMIT, SCAN_TIMEOUT);
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
            if (deviceInfo.state == NodeInfo.STATE_BIND_SUCCESS && !deviceInfo.selected) {
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
//        TelinkLog.d("on remote device found: " + Arrays.bytesToHexString(scanReportStatusMessage.getUuid(), ":"));
        RemoteProvisioningDevice remoteProvisioningDevice = new RemoteProvisioningDevice(scanReportStatusMessage, src);
//        if (!remoteProvisioningDevice.getMac().contains("FF:FF:BB:CC:DD")) return;

        NodeInfo nodeInfo = getNodeByMac(remoteProvisioningDevice.getMac());
        if (nodeInfo != null) {
            return;
        }

        TelinkLog.d("remote device found: " + Arrays.bytesToHexString(remoteProvisioningDevice.getUuid()));
        remoteDevices.add(remoteProvisioningDevice);
    }


    private void provisionNextRemoteDevice(RemoteProvisioningDevice device) {
        TelinkLog.d(String.format("provision next: server -- %04X uuid -- %s",
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
                TelinkLog.d("no device found by: " + serverAddress);
                // set device didRemoteScan = true
                updateDeviceState();
                // scan device by next server
                startRemoteScan();
            } else {
                TelinkLog.d("remote devices scanned: " + remoteDevices.size());
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
        TelinkLog.d("remote act success: " + remote.getMac());

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
        TelinkLog.d("remote act fail: " + event.getRemoteProvisioningDevice().getMac());

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
        byte[] serviceData = sr.getServiceData(ParcelUuid.fromString(UuidInfo.PROVISION_SERVICE_UUID.toString()));
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


    /*private List<ProvisioningDevice> devices;
    private DeviceProvisionListAdapter mListAdapter;

    private Button btn_back;
    private Mesh mesh;

    private UnprovisionedDevice targetDevice;

    *//**
     * if proxy node bind success
     *//*
    private boolean proxyComplete;

    *//**
     * device scanned at remote provision mode
     *//*
    private ArraySet<RemoteDevice> remoteDevices;


    private boolean waitingForCmdComplete = false;
//    private Map<RemoteDevice, Integer> ignoreRemoteList;

    private Handler delayHandler = new Handler();

    private static final int REMOTE_SCAN_TIMEOUT = (3 + 2) * 1000;

    // checked when onCreate
    private DeviceInfo directDevice;

//    private DeviceInfo firstRemoteDevice;

    private boolean complete = false;

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
        findViewById(R.id.tv_log).setOnClickListener(this);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_PROVISION_SUCCESS, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_PROVISION_FAIL, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_KEY_BIND_SUCCESS, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_KEY_BIND_FAIL, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_DISCONNECTED, this);
        TelinkMeshApplication.getInstance().addEventListener(ScanEvent.SCAN_TIMEOUT, this);
        TelinkMeshApplication.getInstance().addEventListener(ScanEvent.DEVICE_FOUND, this);
        TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_REMOTE_SCAN_REPORT, this);

        TelinkMeshApplication.getInstance().addEventListener(CommandEvent.EVENT_TYPE_CMD_COMPLETE, this);

        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_REMOTE_PROVISION_SUCCESS, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_REMOTE_PROVISION_FAIL, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_REMOTE_BIND_SUCCESS, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_REMOTE_BIND_FAIL, this);
        mesh = TelinkMeshApplication.getInstance().getMeshInfo();

        remoteDevices = new ArraySet<>();
        actionStart();
    }


    private void actionStart() {
        TelinkLog.d("remote provision action start");
        if (MeshService.getInstance().isOffline()) {
            startScan();
        } else {
            proxyComplete = true;
            directDevice = mesh.getDeviceByMacAddress(MeshService.getInstance().getCurDeviceMac());
            // device selected used as didRemoteScan
            directDevice.selected = false;
            startRemoteScan();
        }
    }

    private void initTitle() {
        Toolbar toolbar = findViewById(R.id.title_bar);
        toolbar.inflateMenu(R.menu.device_scan);
        setTitle("Device Scan(Remote)");

        MenuItem refreshItem = toolbar.getMenu().findItem(R.id.item_refresh);
        refreshItem.setVisible(false);
        toolbar.setNavigationIcon(null);
    }


    private void startScan() {
        enableUI(false);

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


    private void onDeviceFound(AdvertisingDevice device) {
        int address = mesh.pvIndex + 1;
        TelinkLog.d("alloc address: " + address);
        if (address > MeshUtils.DEVICE_ADDRESS_MAX) {
            enableUI(true);
            return;
        }

        ProvisioningDevice pvDevice = new ProvisioningDevice();
        pvDevice.uuid = null;
        pvDevice.macAddress = device.device.getAddress();
        pvDevice.state = ProvisioningDevice.STATE_PROVISIONING;
        pvDevice.unicastAddress = address;
//        pvDevice.elementCnt = remote.elementCnt;
        pvDevice.failReason = null;
        devices.add(pvDevice);
        mListAdapter.notifyDataSetChanged();


        targetDevice = new UnprovisionedDevice(device, address);
        byte[] provisionData = ProvisionDataGenerator.getProvisionData(mesh.networkKey, mesh.netKeyIndex, mesh.ivUpdateFlag, mesh.ivIndex, address);
        ProvisionParameters parameters = ProvisionParameters.getDefault(provisionData, targetDevice);
        MeshService.getInstance().startProvision(parameters);
    }

    @Override
    public void finish() {
        super.finish();
        if (!complete) {
            MeshService.getInstance().idle(true);
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (delayHandler != null) {
            delayHandler.removeCallbacksAndMessages(null);
        }
        TelinkMeshApplication.getInstance().removeEventListener(this);
    }


    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_back:
                finish();
                break;
            case R.id.tv_log:
                startActivity(new Intent(this, LogActivity.class));
                break;
        }
    }


    private void enableUI(boolean enable) {
        delayHandler.removeCallbacksAndMessages(null);
        complete = enable;
        TelinkLog.d("enable ui: " + enable);
        btn_back.setEnabled(enable);
//        refreshItem.setVisible(enable);
    }

    @Override
    public void performed(final Event<String> event) {
        super.performed(event);

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                String eventType = event.getType();
                if (eventType.equals(MeshEvent.EVENT_TYPE_PROVISION_SUCCESS)) {
                    onProvisionSuccess((MeshEvent) event);
                } else if (eventType.equals(ScanEvent.SCAN_TIMEOUT)) {
                    enableUI(true);
                } else if (eventType.equals(MeshEvent.EVENT_TYPE_PROVISION_FAIL)) {
                    onProvisionFail((MeshEvent) event);
                    startScan();
                } else if (eventType.equals(MeshEvent.EVENT_TYPE_KEY_BIND_SUCCESS)) {
                    onKeyBindSuccess((MeshEvent) event);
//                    directDevice = ((MeshEvent) event).getDeviceInfo();
                    if (proxyComplete) {
                        TelinkLog.e("key bind success when proxy complete !!");
                    } else {
                        proxyComplete = true;
                        startRemoteScan();
                    }
                } else if (eventType.equals(MeshEvent.EVENT_TYPE_KEY_BIND_FAIL)) {
                    onKeyBindFail((MeshEvent) event);
                    enableUI(true);
                } else if (eventType.equals(ScanEvent.DEVICE_FOUND)) {
                    AdvertisingDevice device = ((ScanEvent) event).advertisingDevice;
                    onDeviceFound(device);
                    TelinkLog.d("Scan device: " + device.device.getAddress());
                } else if (eventType.equals(NotificationEvent.EVENT_TYPE_REMOTE_SCAN_REPORT)) {
//                    TelinkLog.d(TAG + " -- EVENT_TYPE_REMOTE_SCAN_REPORT");
//                    delayHandler.removeCallbacks(remoteScanTimeoutTask);
//                    delayHandler.postDelayed(remoteScanTimeoutTask, REMOTE_SCAN_TIMEOUT);

                    NotificationEvent notificationEvent = (NotificationEvent) event;
                    final byte[] para = notificationEvent.getNotificationInfo().params;

                    if (para != null && para.length != 0) {
                        RemoteDevice remoteDevice = RemoteScanReportParser.create().parse(para);
                        TelinkLog.d(TAG + " -- EVENT_TYPE_REMOTE_SCAN_REPORT -- " + notificationEvent.getNotificationInfo().toString());
                        if (remoteDevice != null) {
                            remoteDevice.src = notificationEvent.getNotificationInfo().srcAdr;
                            TelinkLog.d("remote src: " + remoteDevice.src);
                            // not in ignore list
                            onRemoteDeviceFound(remoteDevice);
                        }
                    }
                } else if (eventType.equals(MeshEvent.EVENT_TYPE_REMOTE_PROVISION_SUCCESS)) {
                    onProvisionSuccess((MeshEvent) event);
                } else if (eventType.equals(MeshEvent.EVENT_TYPE_REMOTE_PROVISION_FAIL)) {
                    onProvisionFail((MeshEvent) event);
                    onRemoteComplete();
                } else if (eventType.equals(MeshEvent.EVENT_TYPE_REMOTE_BIND_SUCCESS)) {
                    *//*if (firstRemoteDevice == null) {
                        firstRemoteDevice = ((MeshEvent) event).getDeviceInfo();
                    }*//*
                    onKeyBindSuccess((MeshEvent) event);
                    onRemoteComplete();
                } else if (eventType.equals(MeshEvent.EVENT_TYPE_REMOTE_BIND_FAIL)) {
                    onKeyBindFail((MeshEvent) event);
                    onRemoteComplete();
                } else if (eventType.equals(CommandEvent.EVENT_TYPE_CMD_COMPLETE)) {
                    *//*if (waitingForCmdComplete) {
                        waitingForCmdComplete = false;
                        actionStart();
                    }*//*
                } else if (eventType.equals(MeshEvent.EVENT_TYPE_DISCONNECTED)) {
                    if (proxyComplete) {
                        enableUI(true);
                    }
                }
            }
        });

    }


    // skip processed devices
    private void onRemoteDeviceFound(RemoteDevice remoteDevice) {
        for (ProvisioningDevice pd : devices) {
            if (pd.macAddress.equals(remoteDevice.mac)) {
                return;
            }
        }
        remoteDevices.add(remoteDevice);
    }


    private void onRemoteComplete() {
        if (MeshService.getInstance().isOffline()) {
            enableUI(true);
            return;
        }
        if (remoteDevices.size() > 0) {
            remoteDevices.removeAt(0);
        }

        if (remoteDevices.size() == 0) {
            startRemoteScan();
            *//*if (devices != null && devices.size() >= 2 && devices.get(1).state == ProvisioningDevice.STATE_BIND_SUCCESS) {
                startRemoteScan(devices.get(1).unicastAddress);
            } else {
                startRemoteScan(targetDevice.unicastAddress);
            }*//*

        } else {
            provisionNextRemoteDevice(remoteDevices.valueAt(0));
        }
    }


    private int scanningAddress = -1;

    *//**
     * remote provision scan
     *//*
    private void startRemoteScan() {


        // scan for 2 device
        final int SCAN_LIMIT = 2;

        // second
        final int SCAN_TIMEOUT = 3;
        // directDevice.meshAddress
        int address = getAvailableServerAddress();
        if (address == -1) {
            TelinkLog.d(TAG + " -- ALL device did remote scan !!!");
            enableUI(true);
            return;
        }
        this.scanningAddress = address;
//        int address = firstRemoteDevice == null ? directDevice.meshAddress : firstRemoteDevice.meshAddress;
        TelinkLog.d("startRemoteScan: " + address);
        MeshCommand meshCommand = MeshCommand.newInstance(mesh.netKeyIndex,
                mesh.appKeyIndex,
                1,
                address,
                Opcode.REMOTE_PROV_SCAN_START.getValue());
        meshCommand.params = new byte[]{SCAN_LIMIT, SCAN_TIMEOUT};

        MeshService.getInstance().sendMeshCommand(meshCommand);
        delayHandler.removeCallbacksAndMessages(null);
        delayHandler.postDelayed(remoteScanTimeoutTask, REMOTE_SCAN_TIMEOUT);
        *//*if (MeshService.getInstance().sendMeshCommand(meshCommand)) {
            delayHandler.removeCallbacksAndMessages(null);
            delayHandler.postDelayed(remoteScanTimeoutTask, REMOTE_SCAN_TIMEOUT);
        } else {
            waitingForCmdComplete = true;
        }*//*

    }

    private Runnable remoteScanTimeoutTask = new Runnable() {
        @Override
        public void run() {
            if (remoteDevices.size() == 0) {
                TelinkLog.d("no device found by: " + scanningAddress);
                // set device didRemoteScan = true
                updateDeviceState();
                // scan device by next server
                startRemoteScan();
//                enableUI(true);
            } else {
                TelinkLog.d("remote devices scanned: " + remoteDevices.size());
                provisionNextRemoteDevice(remoteDevices.valueAt(0));
            }
        }
    };

    private void updateDeviceState() {
        if (directDevice != null && directDevice.meshAddress == scanningAddress) {
            directDevice.selected = true;
            return;
        }
        if (devices != null) {
            for (ProvisioningDevice device : devices) {
                if (device.unicastAddress == scanningAddress) {
                    device.didRemoteScan = true;
                }
            }
        }
    }

    private void provisionNextRemoteDevice(RemoteDevice remoteDevice) {
        int address = mesh.pvIndex + 1;
        TelinkLog.d("alloc address: " + address);
        if (address > MeshUtils.DEVICE_ADDRESS_MAX) {
            enableUI(true);
            return;
        }

        ProvisioningDevice pvDevice = new ProvisioningDevice();
        pvDevice.uuid = null;
        pvDevice.macAddress = remoteDevice.mac;
        pvDevice.state = ProvisioningDevice.STATE_PROVISIONING;
        pvDevice.unicastAddress = address;
//        pvDevice.elementCnt = remote.elementCnt;
        pvDevice.failReason = null;
        devices.add(pvDevice);
        mListAdapter.notifyDataSetChanged();

        remoteDevice.pvAddress = address;
        TelinkLog.d("provision next remote device: " + remoteDevice.toString());

        byte[] provisionData = ProvisionDataGenerator.getProvisionData(
                mesh.networkKey, mesh.netKeyIndex, mesh.ivUpdateFlag, mesh.ivIndex, address);
        MeshService.getInstance().startRemoteProvision(RemoteProvisionParameters.getDefault(provisionData, remoteDevice));
    }


    private void onProvisionFail(MeshEvent event) {
        DeviceInfo deviceInfo = event.getDeviceInfo();
        ProvisioningDevice pvDevice = getDeviceByMac(deviceInfo.macAddress);
        if (pvDevice == null) return;
        pvDevice.state = ProvisioningDevice.STATE_PROVISION_FAIL;
//        pvDevice.unicastAddress = deviceInfo.meshAddress;
//        pvDevice.elementCnt = deviceInfo.elementCnt;
        pvDevice.failReason = null;
        mListAdapter.notifyDataSetChanged();

    }

    private void onProvisionSuccess(MeshEvent event) {

        DeviceInfo remote = event.getDeviceInfo();
        remote.bindState = DeviceBindState.BINDING;

        mesh.insertDevice(remote);
        mesh.pvIndex = remote.meshAddress + remote.elementCnt - 1;
        mesh.saveOrUpdate(RemoteProvisionActivity.this);

        ProvisioningDevice pvDevice = getDeviceByMac(remote.macAddress);
        if (pvDevice == null) return;
        pvDevice.uuid = null;
        pvDevice.macAddress = remote.macAddress;
        pvDevice.state = ProvisioningDevice.STATE_BINDING;
        pvDevice.unicastAddress = remote.meshAddress;
        pvDevice.elementCnt = remote.elementCnt;
        pvDevice.failReason = null;
        mListAdapter.notifyDataSetChanged();

        // check if private mode opened
        final boolean privateMode = SharedPreferenceHelper.isPrivateMode(this);

        // check if device support fast bind
        boolean fastBind = false;
        if (privateMode && targetDevice != null && targetDevice.scanRecord != null) {
            PrivateDevice device = getPrivateDevice(targetDevice.scanRecord);
            if (device != null) {
                NodeInfo nodeInfo = new NodeInfo();

                nodeInfo.nodeAdr = remote.meshAddress;
                nodeInfo.elementCnt = remote.elementCnt;
                nodeInfo.deviceKey = remote.deviceKey;
                final byte[] cpsData = device.getCpsData();
                nodeInfo.cpsData = NodeInfo.CompositionData.from(cpsData);
                nodeInfo.cpsDataLen = cpsData.length;
                remote.nodeInfo = nodeInfo;
                fastBind = true;
            }
        }

        KeyBindParameters parameters = KeyBindParameters.getDefault(remote,
                mesh.appKey, mesh.appKeyIndex, mesh.netKeyIndex, fastBind);
        if (!proxyComplete) {
            MeshService.getInstance().startKeyBind(parameters);
        } else {
            MeshService.getInstance().startRemoteBind(parameters);
        }

    }


    private PrivateDevice getPrivateDevice(byte[] scanRecord) {
        if (scanRecord == null) return null;
        MeshScanRecord sr = MeshScanRecord.parseFromBytes(scanRecord);
        byte[] serviceData = sr.getServiceData(ParcelUuid.fromString(UuidInfo.PROVISION_SERVICE_UUID.toString()));
        if (serviceData == null) return null;
        return PrivateDevice.filter(serviceData);
    }


    private void onKeyBindSuccess(MeshEvent event) {
        DeviceInfo remote = event.getDeviceInfo();
        DeviceInfo local = mesh.getDeviceByMacAddress(remote.macAddress);
        if (local == null) return;

        ProvisioningDevice deviceInList = getDeviceByMac(remote.macAddress);
        if (deviceInList == null) return;

        deviceInList.state = ProvisioningDevice.STATE_BIND_SUCCESS;
        deviceInList.nodeInfo = remote.nodeInfo;
        mListAdapter.notifyDataSetChanged();

        local.bindState = DeviceBindState.BOUND;
        local.boundModels = remote.boundModels;
        local.nodeInfo = remote.nodeInfo;
        mesh.saveOrUpdate(RemoteProvisionActivity.this);
    }

    private void onKeyBindFail(MeshEvent event) {
        DeviceInfo remote = event.getDeviceInfo();
        DeviceInfo local = mesh.getDeviceByMacAddress(remote.macAddress);
        if (local == null) return;

        ProvisioningDevice deviceInList = getDeviceByMac(remote.macAddress);
        if (deviceInList == null) return;

        deviceInList.state = ProvisioningDevice.STATE_BIND_FAIL;
        mListAdapter.notifyDataSetChanged();

        local.bindState = DeviceBindState.UNBIND;
        local.boundModels = remote.boundModels;
        mesh.saveOrUpdate(RemoteProvisionActivity.this);
    }

    private ProvisioningDevice getDeviceByMac(String mac) {
        if (devices == null) return null;
        for (ProvisioningDevice deviceInfo : devices) {
            if (deviceInfo.macAddress.equals(mac))
                return deviceInfo;
        }
        return null;
    }

    *//**
     * get target server device for remote scan
     * deviceInfo.selected been used as didRemoteScan
     *
     * @return device : bind success and not did remote scan
     *//*
    private int getAvailableServerAddress() {
        if (directDevice != null && !directDevice.selected) {
            return directDevice.meshAddress;
        }
        if (devices == null) return -1;
        for (ProvisioningDevice deviceInfo : devices) {
            if (deviceInfo.state == ProvisioningDevice.STATE_BIND_SUCCESS && !deviceInfo.didRemoteScan) {
                return deviceInfo.unicastAddress;
            }
        }
        return -1;
    }*/
}
