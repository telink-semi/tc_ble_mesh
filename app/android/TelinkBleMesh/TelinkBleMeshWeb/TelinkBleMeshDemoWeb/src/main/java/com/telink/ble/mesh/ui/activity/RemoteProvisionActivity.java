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
package com.telink.ble.mesh.ui.activity;

import android.os.Bundle;
import android.os.Handler;
import android.view.MenuItem;

import androidx.appcompat.widget.Toolbar;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.assist.ProvisionAssist;
import com.telink.ble.mesh.assist.RemoteProvisionAssist;
import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.NotificationMessage;
import com.telink.ble.mesh.core.message.rp.ScanReportStatusMessage;
import com.telink.ble.mesh.core.message.rp.ScanStartMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.entity.AdvertisingDevice;
import com.telink.ble.mesh.entity.RemoteProvisioningDevice;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.MeshEvent;
import com.telink.ble.mesh.foundation.event.ScanEvent;
import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;
import com.telink.ble.mesh.foundation.parameter.ScanParameters;
import com.telink.ble.mesh.model.NetworkingDevice;
import com.telink.ble.mesh.model.NetworkingState;
import com.telink.ble.mesh.model.RemoteNetworkingDevice;
import com.telink.ble.mesh.model.TelinkPlatformUuidInfo;
import com.telink.ble.mesh.ui.adapter.RemoteProvisionListAdapter;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;
import com.telink.ble.mesh.web.entity.MeshNetworkDetail;
import com.telink.ble.mesh.web.entity.MeshNode;

import java.nio.ByteOrder;
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

    private MeshNetworkDetail meshInfo;

    /**
     * ui devices
     */
    private List<RemoteNetworkingDevice> devices = new ArrayList<>();

    private RemoteProvisionListAdapter mListAdapter;

    /**
     * scanned devices timeout remote-scanning
     */
    private ArrayList<RemoteProvisioningDevice> remoteDevices = new ArrayList<>();

    private Handler delayHandler = new Handler();

    private boolean proxyComplete = false;

    private static final byte THRESHOLD_REMOTE_RSSI = -85;

    private ProvisionAssist provisionAssist;

    private RemoteProvisionAssist remoteProvisionAssist;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_device_provision);
        initTitle();
        RecyclerView rv_devices = findViewById(R.id.rv_devices);


        mListAdapter = new RemoteProvisionListAdapter(this, devices);
        rv_devices.setLayoutManager(new GridLayoutManager(this, 2));
        rv_devices.setAdapter(mListAdapter);

        meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_DISCONNECTED, this);

        TelinkMeshApplication.getInstance().addEventListener(ScanReportStatusMessage.class.getName(), this);
        TelinkMeshApplication.getInstance().addEventListener(ScanEvent.EVENT_TYPE_SCAN_TIMEOUT, this);
        TelinkMeshApplication.getInstance().addEventListener(ScanEvent.EVENT_TYPE_DEVICE_FOUND, this);

        initAssist();
        actionStart();
    }

    private void initAssist() {
        provisionAssist = new ProvisionAssist(this);
        remoteProvisionAssist = new RemoteProvisionAssist(this);
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
        MeshLogger.log("remote provision action start: login? " + proxyLogin);
        if (proxyLogin) {
            proxyComplete = true;
            startRemoteScan();
        } else {
            // first provision a node by gatt
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
        if (serviceData == null || serviceData.length < 17) {
            MeshLogger.log("serviceData error", MeshLogger.LEVEL_ERROR);
            return;
        }


        final int uuidLen = 16;
        byte[] deviceUUID = new byte[uuidLen];
        System.arraycopy(serviceData, 0, deviceUUID, 0, uuidLen);

        final int oobInfo = MeshUtils.bytes2Integer(serviceData, 16, 2, ByteOrder.LITTLE_ENDIAN);

        if (deviceExists(deviceUUID)) {
            MeshLogger.d("device exists");
            return;
        }

        TelinkPlatformUuidInfo uuidInfo = TelinkPlatformUuidInfo.parseFromUuid(deviceUUID);
        if (uuidInfo == null) {
            MeshLogger.e("device uuid platform info parse error");
            return;
        } else {
            MeshLogger.d("uuidInfo - " + uuidInfo.toString());
        }

        RemoteNetworkingDevice processingDevice = new RemoteNetworkingDevice();
        processingDevice.uuidInfo = uuidInfo;
        processingDevice.bluetoothDevice = advertisingDevice.device;
        processingDevice.oobInfo = oobInfo;
        processingDevice.address = -1;
        processingDevice.deviceUUID = deviceUUID;
        processingDevice.state = NetworkingState.IDLE;
        processingDevice.addLog(NetworkingDevice.TAG_SCAN, "device found");
        devices.add(processingDevice);
        mListAdapter.notifyDataSetChanged();

        startGattProvision(processingDevice);
    }

    public void startGattProvision(RemoteNetworkingDevice device) {
        enableUI(false);
        provisionAssist.start(device, PV_CB);
    }

    private ProvisionAssist.ProvisionAssistCallback PV_CB = new ProvisionAssist.ProvisionAssistCallback() {
        @Override
        public void onDeviceStateUpdate(NetworkingDevice device) {
            MeshLogger.d("ProvisionAssistCallback#onDeviceStateUpdate : " + device.state);
            runOnUiThread(() -> mListAdapter.notifyDataSetChanged());
        }

        @Override
        public void onError(String desc) {
            enableUI(true);
        }

        @Override
        public void onComplete(boolean success) {
            if (success) {
                proxyComplete = true;
                provisionAssist.clear();
                startRemoteScan();
            } else {
                enableUI(true);
            }
        }
    };

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
    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (provisionAssist != null) {
            provisionAssist.clear();
        }
        if (remoteProvisionAssist != null) {
            remoteProvisionAssist.clear();
        }
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
        TelinkPlatformUuidInfo platformUuidInfo = TelinkPlatformUuidInfo.parseFromUuid(uuid);
        if (platformUuidInfo == null) {
            MeshLogger.d("not platform device: " + Arrays.bytesToHexString(uuid));
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

        RemoteNetworkingDevice remoteNwkDevice = new RemoteNetworkingDevice();
        remoteNwkDevice.remoteProvisioningDevice = device;
        remoteNwkDevice.uuidInfo = TelinkPlatformUuidInfo.parseFromUuid(device.getUuid());
        remoteNwkDevice.address = -1;
        remoteNwkDevice.deviceUUID = device.getUuid();
        remoteNwkDevice.state = NetworkingState.IDLE;
        remoteNwkDevice.addLog(NetworkingDevice.TAG_SCAN, "device found");
        remoteNwkDevice.state = NetworkingState.PROVISIONING;
        remoteNwkDevice.remoteProvisioningDevice.setAutoStart(false);

        /*NodeInfo nodeInfo = new NodeInfo();
        nodeInfo.deviceUUID = device.getUuid();

        byte[] macBytes = new byte[6];
        System.arraycopy(nodeInfo.deviceUUID, 10, macBytes, 0, macBytes.length);
        macBytes = Arrays.reverse(macBytes);
        nodeInfo.macAddress = Arrays.bytesToHexString(macBytes, ":").toUpperCase();*/

        devices.add(remoteNwkDevice);
        mListAdapter.notifyDataSetChanged();

        remoteProvisionAssist.start(remoteNwkDevice, REMOTE_PV_CB);
        MeshService.getInstance().startRemoteProvisioning(device);
    }


    private final RemoteProvisionAssist.RemoteProvisionAssistCallback REMOTE_PV_CB = new RemoteProvisionAssist.RemoteProvisionAssistCallback() {
        @Override
        public void onDeviceStateUpdate(NetworkingDevice device) {
            MeshLogger.d("RemoteProvisionAssistCallback#onDeviceStateUpdate : " + device.state);
            runOnUiThread(() -> mListAdapter.notifyDataSetChanged());
        }

        @Override
        public void onError(String desc) {
            enableUI(true);
        }

        @Override
        public void onComplete(boolean success) {
            remoteDevices.clear();
            startRemoteScan();
        }
    };


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

        // normal provisioning
        else if (eventType.equals(ScanEvent.EVENT_TYPE_SCAN_TIMEOUT)) {
            enableUI(true);
        } else if (eventType.equals(ScanEvent.EVENT_TYPE_DEVICE_FOUND)) {
            AdvertisingDevice device = ((ScanEvent) event).getAdvertisingDevice();
            onDeviceFound(device);
        }
        // remote and normal binding
        if (eventType.equals(MeshEvent.EVENT_TYPE_DISCONNECTED)) {
            if (proxyComplete)
                enableUI(true);
        }
    }

    private NetworkingDevice getNodeByUUID(byte[] deviceUUID) {
        for (NetworkingDevice networkingDevice : this.devices) {
            if (Arrays.equals(deviceUUID, networkingDevice.deviceUUID)) {
                return networkingDevice;
            }
        }
        return null;
    }

    private HashSet<Integer> getAvailableServerAddresses() {
        HashSet<Integer> serverAddresses = new HashSet<>();
        for (MeshNode nodeInfo : meshInfo.nodeList) {
            if (!nodeInfo.isOffline()) {
                serverAddresses.add(nodeInfo.address);
            }
        }

        for (NetworkingDevice networkingDevice : devices) {
            serverAddresses.add(networkingDevice.address);
        }
        return serverAddresses;
    }

    /**
     * only find in unprovisioned list
     *
     * @param deviceUUID deviceUUID in unprovisioned scan record
     */
    private boolean deviceExists(byte[] deviceUUID) {
        for (NetworkingDevice device : this.devices) {
            if (device.state == NetworkingState.IDLE && Arrays.equals(deviceUUID, device.deviceUUID)) {
                return true;
            }
        }
        return false;
    }
}
