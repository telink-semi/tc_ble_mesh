/********************************************************************************************************
 * @file DeviceProvisionActivity.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2010
 *
 * @par Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *
 *******************************************************************************************************/
package com.telink.sig.mesh.demo.ui;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.ParcelUuid;
import android.support.v4.util.ArraySet;
import android.support.v7.widget.GridLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;

import com.telink.sig.mesh.ble.AdvertisingDevice;
import com.telink.sig.mesh.ble.MeshScanRecord;
import com.telink.sig.mesh.ble.UnprovisionedDevice;
import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.demo.SharedPreferenceHelper;
import com.telink.sig.mesh.demo.TelinkMeshApplication;
import com.telink.sig.mesh.demo.model.Mesh;
import com.telink.sig.mesh.demo.model.PrivateDevice;
import com.telink.sig.mesh.demo.model.ProvisioningDevice;
import com.telink.sig.mesh.demo.ui.adapter.DeviceProvisionListAdapter;
import com.telink.sig.mesh.event.CommandEvent;
import com.telink.sig.mesh.event.Event;
import com.telink.sig.mesh.event.EventListener;
import com.telink.sig.mesh.event.MeshEvent;
import com.telink.sig.mesh.event.NotificationEvent;
import com.telink.sig.mesh.event.ScanEvent;
import com.telink.sig.mesh.light.MeshService;
import com.telink.sig.mesh.light.Opcode;
import com.telink.sig.mesh.light.ProvisionDataGenerator;
import com.telink.sig.mesh.light.RemoteScanReportParser;
import com.telink.sig.mesh.light.ScanParameters;
import com.telink.sig.mesh.light.UuidInfo;
import com.telink.sig.mesh.light.parameter.KeyBindParameters;
import com.telink.sig.mesh.light.parameter.ProvisionParameters;
import com.telink.sig.mesh.light.parameter.RemoteProvisionParameters;
import com.telink.sig.mesh.model.DeviceBindState;
import com.telink.sig.mesh.model.DeviceInfo;
import com.telink.sig.mesh.model.MeshCommand;
import com.telink.sig.mesh.model.NodeInfo;
import com.telink.sig.mesh.model.RemoteDevice;
import com.telink.sig.mesh.util.MeshUtils;
import com.telink.sig.mesh.util.TelinkLog;

import java.util.ArrayList;
import java.util.List;

/**
 * remote provision
 * actions:
 * 1.scan -- success ->
 * 2.connect -- success ->
 * 3.provision -- success ->
 * 4.KeyBind -- success ->
 * send scan
 * Created by kee on 2019/3/25.
 */
public class RemoteProvisionActivity extends BaseActivity implements View.OnClickListener, EventListener<String> {

    private List<ProvisioningDevice> devices;
    private DeviceProvisionListAdapter mListAdapter;

    private Button btn_back;
    private Mesh mesh;

    private UnprovisionedDevice targetDevice;

    /**
     * if proxy node bind success
     */
    private boolean proxyComplete;

    /**
     * device scanned at remote provision mode
     */
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
        mesh = TelinkMeshApplication.getInstance().getMesh();

        remoteDevices = new ArraySet<>();
        actionStart();
    }


    private void actionStart() {
        TelinkLog.d("remote provision action start");
        if (MeshService.getInstance().isOffline()) {
            startScan();
        } else {
            proxyComplete = true;
//            directDevice = getAnotherDevice(MeshService.getInstance().getCurDeviceMac());
            directDevice = mesh.getDeviceByMacAddress(MeshService.getInstance().getCurDeviceMac());
            // device selected used as didRemoteScan
            directDevice.selected = false;
            startRemoteScan();
        }
    }


    // test for remote relay
    private DeviceInfo getAnotherDevice(String mac) {
        for (DeviceInfo nodeInfo : mesh.devices) {
            if (!nodeInfo.macAddress.equals(mac)) {
                return nodeInfo;
            }
        }
        return null;
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
                    /*if (firstRemoteDevice == null) {
                        firstRemoteDevice = ((MeshEvent) event).getDeviceInfo();
                    }*/
                    onKeyBindSuccess((MeshEvent) event);
                    onRemoteComplete();
                } else if (eventType.equals(MeshEvent.EVENT_TYPE_REMOTE_BIND_FAIL)) {
                    onKeyBindFail((MeshEvent) event);
                    onRemoteComplete();
                } else if (eventType.equals(CommandEvent.EVENT_TYPE_CMD_COMPLETE)) {
                    /*if (waitingForCmdComplete) {
                        waitingForCmdComplete = false;
                        actionStart();
                    }*/
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
        // for test
//        if (!remoteDevice.mac.toUpperCase().contains("FF:FF:BB:CC:DD")) return;
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
            /*if (devices != null && devices.size() >= 2 && devices.get(1).state == ProvisioningDevice.STATE_BIND_SUCCESS) {
                startRemoteScan(devices.get(1).unicastAddress);
            } else {
                startRemoteScan(targetDevice.unicastAddress);
            }*/

        } else {
            provisionNextRemoteDevice(remoteDevices.valueAt(0));
        }
    }


    private int scanningAddress = -1;

    /**
     * remote provision scan
     */
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
        /*if (MeshService.getInstance().sendMeshCommand(meshCommand)) {
            delayHandler.removeCallbacksAndMessages(null);
            delayHandler.postDelayed(remoteScanTimeoutTask, REMOTE_SCAN_TIMEOUT);
        } else {
            waitingForCmdComplete = true;
        }*/

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

    /**
     * get target server device for remote scan
     * deviceInfo.selected been used as didRemoteScan
     *
     * @return device : bind success and not did remote scan
     */
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
    }
}
