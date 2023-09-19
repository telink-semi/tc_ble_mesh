/********************************************************************************************************
 * @file RemoteProvisionActivity.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2017
 *
 * @par Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/
package com.telink.ble.mesh.ui;

import android.os.Bundle;
import android.os.Handler;
import android.view.MenuItem;

import androidx.appcompat.widget.Toolbar;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.SharedPreferenceHelper;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.access.BindingBearer;
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
import com.telink.ble.mesh.model.CertCacheService;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.NetworkingDevice;
import com.telink.ble.mesh.model.NetworkingState;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.PrivateDevice;
import com.telink.ble.mesh.model.db.MeshInfoService;
import com.telink.ble.mesh.ui.adapter.DeviceAutoProvisionListAdapter;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;
import java.util.List;

/**
 * remote provision
 * actions:
 * 1. remote scan ->
 * 2. remote scan rsp, remote device found <-
 * 3. start remote provision ->
 * 4. remote provision event (if success , start key-binding) <-
 * 5. remote scan -> ...
 */

public class RemoteProvisionActivity extends BaseActivity implements EventListener<String> {

    private MeshInfo meshInfo;

    /**
     * ui devices
     */
    private List<NetworkingDevice> devices = new ArrayList<>();

    private DeviceAutoProvisionListAdapter mListAdapter;

    /**
     * scanned devices timeout remote-scanning
     */
    private ArrayList<RemoteProvisioningDevice> remoteDevices = new ArrayList<>();

    private Handler delayHandler = new Handler();

    private boolean proxyComplete = false;

    private static final byte THRESHOLD_REMOTE_RSSI = -85;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_device_provision);
        initTitle();
        RecyclerView rv_devices = findViewById(R.id.rv_devices);

        mListAdapter = new DeviceAutoProvisionListAdapter(this, devices);
        rv_devices.setLayoutManager(new GridLayoutManager(this, 2));
        rv_devices.setAdapter(mListAdapter);

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
        setTitle("Device Scan", "Remote");

        MenuItem refreshItem = toolbar.getMenu().findItem(R.id.item_refresh);
        refreshItem.setVisible(false);
        toolbar.setNavigationIcon(null);
    }

    private void actionStart() {
        enableUI(false);

        boolean proxyLogin = MeshService.getInstance().isProxyLogin();
        MeshLogger.log("remote provision action start: login? " + proxyLogin);
        if (proxyLogin) {
            proxyComplete = true;
            startRemoteScan();
        } else {
            proxyComplete = false;
            startScan();
        }
    }


    private void enableUI(boolean enable) {
        MeshLogger.d("remote - enable ui: " + enable);
        enableBackNav(enable);
    }

    /******************************************************************************
     * normal provisioning
     ******************************************************************************/
    private void startScan() {
        ScanParameters parameters = ScanParameters.getDefault(false, false);
        parameters.setScanTimeout(10 * 1000);
        MeshService.getInstance().startScan(parameters);
    }

    private void onDeviceFound(AdvertisingDevice advertisingDevice) {
        // provision service data: 15:16:28:18:[16-uuid]:[2-oobInfo]
        byte[] serviceData = MeshUtils.getMeshServiceData(advertisingDevice.scanRecord, true);
        if (serviceData == null || serviceData.length < 16) {
            MeshLogger.log("serviceData error", MeshLogger.LEVEL_ERROR);
            return;
        }
        final int uuidLen = 16;
        byte[] deviceUUID = new byte[uuidLen];


        System.arraycopy(serviceData, 0, deviceUUID, 0, uuidLen);
        NetworkingDevice localNode = getNodeByUUID(deviceUUID);
        if (localNode != null) {
            MeshLogger.d("device exists");
            return;
        }
        MeshService.getInstance().stopScan();

        int address = meshInfo.getProvisionIndex();

        MeshLogger.d("alloc address: " + address);
        if (address == -1) {
            enableUI(true);
            return;
        }

        ProvisioningDevice provisioningDevice = new ProvisioningDevice(advertisingDevice.device, deviceUUID, address);

        // check if oob exists
        byte[] oob = MeshInfoService.getInstance().getOobByDeviceUUID(deviceUUID);
        if (oob != null) {
            provisioningDevice.setAuthValue(oob);
        } else {
            final boolean autoUseNoOOB = SharedPreferenceHelper.isNoOOBEnable(this);
            provisioningDevice.setAutoUseNoOOB(autoUseNoOOB);
        }
        provisioningDevice.setRootCert(CertCacheService.getInstance().getRootCert());
        ProvisioningParameters provisioningParameters = new ProvisioningParameters(provisioningDevice);
        if (MeshService.getInstance().startProvisioning(provisioningParameters)) {
            NodeInfo nodeInfo = new NodeInfo();
            nodeInfo.meshAddress = address;
            nodeInfo.macAddress = advertisingDevice.device.getAddress();
            nodeInfo.deviceUUID = deviceUUID;
            NetworkingDevice device = new NetworkingDevice(nodeInfo);
            device.bluetoothDevice = advertisingDevice.device;
            device.state = NetworkingState.PROVISIONING;
            devices.add(device);
            mListAdapter.notifyDataSetChanged();
        } else {
            MeshLogger.d("provisioning busy");
        }
    }

    private void onProvisionSuccess(ProvisioningEvent event) {

        ProvisioningDevice remote = event.getProvisioningDevice();

        NetworkingDevice networkingDevice = getProcessingNode();

        networkingDevice.state = NetworkingState.BINDING;
        networkingDevice.nodeInfo.elementCnt = remote.getDeviceCapability().eleNum;
        networkingDevice.nodeInfo.deviceKey = remote.getDeviceKey();
        networkingDevice.nodeInfo.netKeyIndexes.add(MeshUtils.intToHex2(meshInfo.getDefaultNetKey().index));
        meshInfo.insertDevice(networkingDevice.nodeInfo, true);

        // check if private mode opened
        final boolean privateMode = SharedPreferenceHelper.isPrivateMode(this);

        // check if device support fast bind
        boolean defaultBound = false;
        if (privateMode && remote.getDeviceUUID() != null) {
            PrivateDevice device = PrivateDevice.filter(remote.getDeviceUUID());
            if (device != null) {
                MeshLogger.log("private device");
                final byte[] cpsData = device.getCpsData();
                networkingDevice.nodeInfo.compositionData = CompositionData.from(cpsData);
                defaultBound = true;
            } else {
                MeshLogger.log("private device null");
            }
        }

        networkingDevice.nodeInfo.setDefaultBind(defaultBound);
        mListAdapter.notifyDataSetChanged();
        int appKeyIndex = meshInfo.getDefaultAppKeyIndex();
        BindingDevice bindingDevice = new BindingDevice(networkingDevice.nodeInfo.meshAddress, networkingDevice.nodeInfo.deviceUUID, appKeyIndex);
        bindingDevice.setDefaultBound(defaultBound);
        MeshService.getInstance().startBinding(new BindingParameters(bindingDevice));
    }

    private void onProvisionFail(ProvisioningEvent event) {
        ProvisioningDevice deviceInfo = event.getProvisioningDevice();
        NetworkingDevice pvDevice = getProcessingNode();
        pvDevice.state = NetworkingState.PROVISION_FAIL;
        pvDevice.addLog("Provisioning", event.getDesc());
        mListAdapter.notifyDataSetChanged();
    }

    private void onKeyBindSuccess(BindingEvent event) {
        BindingDevice remote = event.getBindingDevice();
        NetworkingDevice deviceInList = getProcessingNode();
        deviceInList.state = NetworkingState.BIND_SUCCESS;
        deviceInList.nodeInfo.bound = true;
        // if is default bound, composition data has been valued ahead of binding action
        if (!remote.isDefaultBound()) {
            deviceInList.nodeInfo.compositionData = remote.getCompositionData();
        }

        mListAdapter.notifyDataSetChanged();
        deviceInList.nodeInfo.save();
    }

    private void onKeyBindFail(BindingEvent event) {
        BindingDevice remote = event.getBindingDevice();
        NetworkingDevice deviceInList = getProcessingNode();
        deviceInList.state = NetworkingState.BIND_FAIL;
        deviceInList.addLog("Binding", event.getDesc());
        mListAdapter.notifyDataSetChanged();
        meshInfo.saveOrUpdate();
    }


    /******************************************************************************
     * remote provisioning
     ******************************************************************************/
    private void startRemoteScan() {
        // scan for max 2 devices
        final byte SCAN_LIMIT = 2;
        // scan for 5 seconds
        final byte SCAN_TIMEOUT = 5;
//        final int SERVER_ADDRESS = 0xFFFF;

        HashSet<Integer> serverAddresses = getAvailableServerAddresses();
        if (serverAddresses.size() == 0) {
            MeshLogger.e("no Available server address");
            return;
        }
        for (int address : serverAddresses) {
            ScanStartMessage remoteScanMessage = ScanStartMessage.getSimple(address, 1, SCAN_LIMIT, SCAN_TIMEOUT);
            MeshService.getInstance().sendMeshMessage(remoteScanMessage);
        }

        delayHandler.removeCallbacksAndMessages(null);
        delayHandler.postDelayed(remoteScanTimeoutTask, (SCAN_TIMEOUT + 5) * 1000);
    }

    private void onRemoteComplete() {
        MeshLogger.d("remote prov - remote complete : rest - " + remoteDevices.size());
        if (!MeshService.getInstance().isProxyLogin()) {
            enableUI(true);
            return;
        }
        remoteDevices.clear();
        startRemoteScan();
        /*if (remoteDevices.size() > 0) {
            remoteDevices.remove(0);
        }

        if (remoteDevices.size() == 0) {
            startRemoteScan();
        } else {
            delayHandler.removeCallbacksAndMessages(null);
            delayHandler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    provisionNextRemoteDevice(remoteDevices.get(0));
                }
            }, 500);

        }*/
    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
        TelinkMeshApplication.getInstance().removeEventListener(this);
        delayHandler.removeCallbacksAndMessages(null);
    }

    private void onRemoteDeviceScanned(int src, ScanReportStatusMessage scanReportStatusMessage) {
        final byte rssi = scanReportStatusMessage.getRssi();
        final byte[] uuid = scanReportStatusMessage.getUuid();
        MeshLogger.log("remote device found: " + Integer.toHexString(src) + " -- " + Arrays.bytesToHexString(uuid) + " -- rssi: " + rssi);
        /*if (rssi < THRESHOLD_REMOTE_RSSI) {
            MeshLogger.log("scan report ignore because of RSSI limit");
            return;
        }*/
        RemoteProvisioningDevice remoteProvisioningDevice = new RemoteProvisioningDevice(rssi, uuid, src);
//        if (!Arrays.bytesToHexString(remoteProvisioningDevice.getUuid(), ":").contains("DD:CC:BB:FF:FF")) return;

        // check if device exists
        NetworkingDevice networkingDevice = getNodeByUUID(remoteProvisioningDevice.getUuid());
        if (networkingDevice != null) {
            MeshLogger.d("device already exists");
            return;
        }


        int index = remoteDevices.indexOf(remoteProvisioningDevice);
        if (index >= 0) {
            // exists
            RemoteProvisioningDevice device = remoteDevices.get(index);
            if (device != null) {
                if (device.getRssi() < remoteProvisioningDevice.getRssi() && device.getServerAddress() != remoteProvisioningDevice.getServerAddress()) {
                    MeshLogger.log("remote device replaced");
                    device.setRssi(remoteProvisioningDevice.getRssi());
                    device.setServerAddress(device.getServerAddress());
                }
            }
        } else {
            MeshLogger.log("remote device add");
            remoteDevices.add(remoteProvisioningDevice);
        }

        Collections.sort(remoteDevices, (o1, o2) -> o2.getRssi() - o1.getRssi());
        for (RemoteProvisioningDevice device :
                remoteDevices) {
            MeshLogger.log("sort remote device: " + " -- " + Arrays.bytesToHexString(device.getUuid()) + " -- rssi: " + device.getRssi());
        }
    }


    private void provisionNextRemoteDevice(RemoteProvisioningDevice device) {
        MeshLogger.log(String.format("provision next: server -- %04X uuid -- %s",
                device.getServerAddress(),
                Arrays.bytesToHexString(device.getUuid())));
        int address = meshInfo.getProvisionIndex();
        if (address > MeshUtils.UNICAST_ADDRESS_MAX) {
            enableUI(true);
            return;
        }

        device.setUnicastAddress(address);
        MeshLogger.d("remote allocated adr: " + address);
        NodeInfo nodeInfo = new NodeInfo();
        nodeInfo.deviceUUID = device.getUuid();

        byte[] macBytes = new byte[6];
        System.arraycopy(nodeInfo.deviceUUID, 10, macBytes, 0, macBytes.length);
        macBytes = Arrays.reverse(macBytes);
        nodeInfo.macAddress = Arrays.bytesToHexString(macBytes, ":").toUpperCase();

        nodeInfo.meshAddress = address;
        NetworkingDevice networkingDevice = new NetworkingDevice(nodeInfo);
        networkingDevice.state = NetworkingState.PROVISIONING;
        devices.add(networkingDevice);
        mListAdapter.notifyDataSetChanged();

        // check if oob exists -- remote support
        byte[] oob = MeshInfoService.getInstance().getOobByDeviceUUID(device.getUuid());
        if (oob != null) {
            device.setAuthValue(oob);
        } else {
            final boolean autoUseNoOOB = SharedPreferenceHelper.isNoOOBEnable(this);
            device.setAutoUseNoOOB(autoUseNoOOB);
        }

        MeshService.getInstance().startRemoteProvisioning(device);
    }

    private Runnable remoteScanTimeoutTask = () -> {
        if (remoteDevices.size() == 0) {
            MeshLogger.log("no device found by remote scan");
            enableUI(true);
        } else {
            MeshLogger.log("remote devices scanned: " + remoteDevices.size());
            RemoteProvisioningDevice dev = remoteDevices.get(0);
            if (dev.getRssi() < THRESHOLD_REMOTE_RSSI) {
                StringBuilder sb = new StringBuilder("All devices are weak-signal : \n");
                for (RemoteProvisioningDevice rpd : remoteDevices) {
                    sb.append(" uuid-").append(Arrays.bytesToHexString(rpd.getUuid()))
                            .append(" rssi-").append(rpd.getRssi())
                            .append(" server-").append(Integer.toHexString(rpd.getServerAddress()))
                            .append("\n");
                }
                showTipDialog(sb.toString());
                enableUI(true);
            } else {
                provisionNextRemoteDevice(remoteDevices.get(0));
            }
        }
    };

    @Override
    public void performed(final Event<String> event) {
        super.performed(event);
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
            onRemoteProvisioningSuccess((RemoteProvisioningEvent) event);
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
        MeshLogger.log("remote act success: " + Arrays.bytesToHexString(remote.getUuid()));
        NetworkingDevice networkingDevice = getProcessingNode();
        networkingDevice.state = NetworkingState.BINDING;
        networkingDevice.nodeInfo.elementCnt = remote.getDeviceCapability().eleNum;
        networkingDevice.nodeInfo.deviceKey = remote.getDeviceKey();
        meshInfo.insertDevice(networkingDevice.nodeInfo, true);
        networkingDevice.nodeInfo.setDefaultBind(false);
        mListAdapter.notifyDataSetChanged();
        int appKeyIndex = meshInfo.getDefaultAppKeyIndex();
        final BindingDevice bindingDevice = new BindingDevice(networkingDevice.nodeInfo.meshAddress, networkingDevice.nodeInfo.deviceUUID, appKeyIndex);
        bindingDevice.setBearer(BindingBearer.Any);
        delayHandler.removeCallbacksAndMessages(null);
        delayHandler.postDelayed(new Runnable() {
            @Override
            public void run() {
                MeshService.getInstance().startBinding(new BindingParameters(bindingDevice));
            }
        }, 3000);

    }

    private void onRemoteProvisioningFail(RemoteProvisioningEvent event) {
        //
        MeshLogger.log("remote act fail: " + Arrays.bytesToHexString(event.getRemoteProvisioningDevice().getUuid()));

        RemoteProvisioningDevice deviceInfo = event.getRemoteProvisioningDevice();
        NetworkingDevice pvDevice = getProcessingNode();
        pvDevice.state = NetworkingState.PROVISION_FAIL;
        pvDevice.addLog("remote-provision", event.getDesc());
        mListAdapter.notifyDataSetChanged();
    }

    private NetworkingDevice getProcessingNode() {
        return this.devices.get(this.devices.size() - 1);
    }

    private NetworkingDevice getNodeByUUID(byte[] deviceUUID) {
        for (NetworkingDevice networkingDevice : this.devices) {
            if (Arrays.equals(deviceUUID, networkingDevice.nodeInfo.deviceUUID)) {
                return networkingDevice;
            }
        }
        return null;
    }

    private HashSet<Integer> getAvailableServerAddresses() {
        HashSet<Integer> serverAddresses = new HashSet<>();
        for (NodeInfo nodeInfo : meshInfo.nodes) {
            if (!nodeInfo.isOffline()) {
                serverAddresses.add(nodeInfo.meshAddress);
            }
        }

        for (NetworkingDevice networkingDevice : devices) {
            serverAddresses.add(networkingDevice.nodeInfo.meshAddress);
        }
        return serverAddresses;
    }
}
