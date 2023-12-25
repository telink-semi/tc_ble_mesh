/********************************************************************************************************
 * @file DeviceProvisionActivity.java
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

import android.content.Intent;
import android.os.Bundle;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;

import androidx.appcompat.widget.Toolbar;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.assist.ProvisionAssist;
import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.config.ModelPublicationStatusMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.entity.AdvertisingDevice;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.ProvisioningEvent;
import com.telink.ble.mesh.foundation.event.ScanEvent;
import com.telink.ble.mesh.foundation.parameter.ScanParameters;
import com.telink.ble.mesh.model.AppSettings;
import com.telink.ble.mesh.model.NetworkingDevice;
import com.telink.ble.mesh.model.NetworkingState;
import com.telink.ble.mesh.model.TelinkPlatformUuidInfo;
import com.telink.ble.mesh.ui.adapter.DeviceProvisionListAdapter;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;
import com.telink.ble.mesh.web.entity.MeshNetworkDetail;

import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.List;

/**
 * scan for unprovision device and provision selected device
 * Created by kee on 2020/11/12.
 */
public class DeviceProvisionActivity extends BaseActivity implements View.OnClickListener, EventListener<String> {

    /**
     * found by bluetooth scan
     */
    private List<NetworkingDevice> devices = new ArrayList<>();

    /**
     * data adapter
     */
    private DeviceProvisionListAdapter mListAdapter;

    /**
     * all all, click to provision/bind all NetworkingDevice
     */
    private Button btn_add_all;

    /**
     * local mesh info
     */
    private MeshNetworkDetail mesh;

    /**
     * title refresh icon
     */
    private MenuItem refreshItem;

    /**
     * is scanning for unprovisioned device
     */
    private boolean isScanning = false;

    private ProvisionAssist provisionAssist;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        initView();
        addEventListeners();
        mesh = TelinkMeshApplication.getInstance().getMeshInfo();
        initAssist();
        startScan();
    }

    private void initAssist() {
        provisionAssist = new ProvisionAssist(this);
    }

    private void initView() {
        setContentView(R.layout.activity_device_provision);
        findViewById(R.id.btn_add_all).setVisibility(View.VISIBLE);
        initTitle();
        RecyclerView rv_devices = findViewById(R.id.rv_devices);
        devices = new ArrayList<>();

        mListAdapter = new DeviceProvisionListAdapter(this, devices);
        rv_devices.setLayoutManager(new LinearLayoutManager(this));

        rv_devices.setAdapter(mListAdapter);
        btn_add_all = findViewById(R.id.btn_add_all);
        btn_add_all.setOnClickListener(this);
        findViewById(R.id.tv_log).setOnClickListener(this);
    }

    private void addEventListeners() {
        TelinkMeshApplication.getInstance().addEventListener(ScanEvent.EVENT_TYPE_SCAN_TIMEOUT, this);
        TelinkMeshApplication.getInstance().addEventListener(ScanEvent.EVENT_TYPE_DEVICE_FOUND, this);
        TelinkMeshApplication.getInstance().addEventListener(ProvisioningEvent.EVENT_TYPE_CAPABILITY_RECEIVED, this);
        TelinkMeshApplication.getInstance().addEventListener(ModelPublicationStatusMessage.class.getName(), this);
    }

    private void initTitle() {
        Toolbar toolbar = findViewById(R.id.title_bar);
        toolbar.inflateMenu(R.menu.device_scan);
        setTitle("Device Scan");
        refreshItem = toolbar.getMenu().findItem(R.id.item_refresh);
        refreshItem.setVisible(false);

        toolbar.setOnMenuItemClickListener(item -> {
            if (item.getItemId() == R.id.item_refresh) {
                startScan();
            }
            return false;
        });
    }


    /**
     * scan for unprovisioned devices
     */
    private void startScan() {
        enableUI(false);
        ScanParameters parameters = ScanParameters.getDefault(false, false);
        parameters.setScanTimeout(10 * 1000);
        MeshService.getInstance().startScan(parameters);
    }

    /**
     * unprovisioned device found
     * FF:FF:BB:CC:DD:51
     * device uuid : 11020100010051DDCCBBFFFF000000C8 -- oobInfo: 0 -- certSupported?false
     * <p>
     * FF:FF:BB:CC:DD:52
     * device uuid : 11020100010052DDCCBBFFFF000000C9 -- oobInfo: 0 -- certSupported?false
     */
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

//        MeshNode nodeInfo = new MeshNode();
//        nodeInfo.deviceUuid = Arrays.bytesToHexString(deviceUUID);
//        MeshLogger.d("device found -> device uuid : " + Arrays.bytesToHexString(deviceUUID) + " -- oobInfo: " + oobInfo + " -- certSupported?" + MeshUtils.isCertSupported(oobInfo));

        TelinkPlatformUuidInfo uuidInfo = TelinkPlatformUuidInfo.parseFromUuid(deviceUUID);
        if (uuidInfo == null) {
            MeshLogger.e("device uuid platform info parse error");
            return;
            // for test
//            uuidInfo = new TelinkPlatformUuidInfo();
//            uuidInfo.tcPid = 1;
//            uuidInfo.tcVendorId = 1;
        } else {
            MeshLogger.d("uuidInfo - " + uuidInfo.toString());
        }


        NetworkingDevice processingDevice = new NetworkingDevice();
        processingDevice.uuidInfo = uuidInfo;
        processingDevice.bluetoothDevice = advertisingDevice.device;
        if (AppSettings.DRAFT_FEATURES_ENABLE) {
            processingDevice.oobInfo = oobInfo;
        }
        processingDevice.address = -1;
        processingDevice.deviceUUID = deviceUUID;
        processingDevice.state = NetworkingState.IDLE;
        processingDevice.addLog(NetworkingDevice.TAG_SCAN, "device found");
        devices.add(processingDevice);
        mListAdapter.notifyDataSetChanged();
    }

    @Override
    public void finish() {
        super.finish();
        MeshService.getInstance().idle(false);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (provisionAssist != null) {
            provisionAssist.clear();
        }
        TelinkMeshApplication.getInstance().removeEventListener(this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_add_all:
                if (addAll()) {
                    btn_add_all.setEnabled(false);
                    mListAdapter.setProcessing(true);
                    provisionNext();
                } else {
                    toastMsg("no available device found");
                }
                break;
            case R.id.tv_log:
                startActivity(new Intent(this, LogActivity.class));
                break;
        }
    }

    public void provisionNext() {
        enableUI(false);
        NetworkingDevice waitingDevice = getNextWaitingDevice();
        if (waitingDevice == null) {
            MeshLogger.d("no waiting device found");
            enableUI(true);
            return;
        }
//        startProvision(waitingDevice);
        provisionAssist.start(waitingDevice, PV_CB);
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
            provisionNext();
        }
    };


    private NetworkingDevice getNextWaitingDevice() {
        for (NetworkingDevice device : devices) {
            if (device.state == NetworkingState.WAITING) {
                return device;
            }
        }
        return null;
    }

    private boolean addAll() {
        boolean anyValid = false;
        for (NetworkingDevice device : devices) {
            if (device.state == NetworkingState.IDLE) {
                anyValid = true;
                device.state = NetworkingState.WAITING;
            }
        }
        return anyValid;
    }

    private void enableUI(final boolean enable) {
        MeshService.getInstance().idle(false);
        runOnUiThread(() -> {
            enableBackNav(enable);
            btn_add_all.setEnabled(enable);
            refreshItem.setVisible(enable);
            mListAdapter.setProcessing(!enable);
        });

    }

    @Override
    public void performed(final Event<String> event) {
        super.performed(event);
        runOnUiThread(() -> {
            if (event.getType().equals(ScanEvent.EVENT_TYPE_SCAN_TIMEOUT)) {
                enableUI(true);
            } else if (event.getType().equals(ScanEvent.EVENT_TYPE_DEVICE_FOUND)) {
                AdvertisingDevice device = ((ScanEvent) event).getAdvertisingDevice();
                onDeviceFound(device);
            }
        });
    }

    /**
     * @param state target state,
     * @return processing device
     */
    private NetworkingDevice getCurrentDevice(NetworkingState state) {
        for (NetworkingDevice device : devices) {
            if (device.state == state) {
                return device;
            }
        }
        return null;
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
