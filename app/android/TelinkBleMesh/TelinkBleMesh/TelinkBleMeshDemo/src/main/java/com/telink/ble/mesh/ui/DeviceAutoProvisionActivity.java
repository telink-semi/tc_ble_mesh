/********************************************************************************************************
 * @file DeviceAutoProvisionActivity.java
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

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.view.MenuItem;
import android.view.View;

import androidx.appcompat.widget.Toolbar;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.SharedPreferenceHelper;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.access.BindingBearer;
import com.telink.ble.mesh.core.message.MeshSigModel;
import com.telink.ble.mesh.core.message.config.ConfigStatus;
import com.telink.ble.mesh.core.message.config.ModelPublicationSetMessage;
import com.telink.ble.mesh.core.message.config.ModelPublicationStatusMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.entity.AdvertisingDevice;
import com.telink.ble.mesh.entity.BindingDevice;
import com.telink.ble.mesh.entity.CompositionData;
import com.telink.ble.mesh.entity.ModelPublication;
import com.telink.ble.mesh.entity.ProvisioningDevice;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.BindingEvent;
import com.telink.ble.mesh.foundation.event.ProvisioningEvent;
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

import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.List;

/**
 * auto provision device one by one
 * actions:
 * 1.scan -- success ->
 * 2.connect -- success ->
 * 3.provision -- success ->
 * 4.KeyBind -- success ->
 * continue scan...
 * Created by kee on 2017/8/28.
 */

public class DeviceAutoProvisionActivity extends BaseActivity implements View.OnClickListener, EventListener<String> {

    private List<NetworkingDevice> devices;
    private DeviceAutoProvisionListAdapter mListAdapter;
    private MeshInfo mesh;
    private MenuItem refreshItem;
    private Handler mHandler = new Handler();
    private boolean isPubSetting = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_device_provision);
        initTitle();
        RecyclerView rv_devices = findViewById(R.id.rv_devices);
        devices = new ArrayList<>();

        mListAdapter = new DeviceAutoProvisionListAdapter(this, devices);
        rv_devices.setLayoutManager(new GridLayoutManager(this, 2));
        rv_devices.setAdapter(mListAdapter);
        findViewById(R.id.tv_log).setOnClickListener(this);
        TelinkMeshApplication.getInstance().addEventListener(ProvisioningEvent.EVENT_TYPE_PROVISION_SUCCESS, this);
        TelinkMeshApplication.getInstance().addEventListener(ProvisioningEvent.EVENT_TYPE_PROVISION_FAIL, this);
        TelinkMeshApplication.getInstance().addEventListener(BindingEvent.EVENT_TYPE_BIND_SUCCESS, this);
        TelinkMeshApplication.getInstance().addEventListener(BindingEvent.EVENT_TYPE_BIND_FAIL, this);
        TelinkMeshApplication.getInstance().addEventListener(ScanEvent.EVENT_TYPE_SCAN_TIMEOUT, this);
        TelinkMeshApplication.getInstance().addEventListener(ScanEvent.EVENT_TYPE_DEVICE_FOUND, this);
        TelinkMeshApplication.getInstance().addEventListener(ModelPublicationStatusMessage.class.getName(), this);
        mesh = TelinkMeshApplication.getInstance().getMeshInfo();

        startScan();
    }

    private void initTitle() {
        Toolbar toolbar = findViewById(R.id.title_bar);
        toolbar.inflateMenu(R.menu.device_scan);
        setTitle("Device Scan", "Auto");
//        toolbar.setSubtitle("provision -> bind");
        refreshItem = toolbar.getMenu().findItem(R.id.item_refresh);
        refreshItem.setVisible(false);
        toolbar.setNavigationIcon(null);

        toolbar.setOnMenuItemClickListener(new Toolbar.OnMenuItemClickListener() {
            @Override
            public boolean onMenuItemClick(MenuItem item) {
                if (item.getItemId() == R.id.item_refresh) {
                    startScan();
                }
                return false;
            }
        });
    }


    private void startScan() {
        enableUI(false);
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
        final int oobInfo = MeshUtils.bytes2Integer(serviceData, 16, 2, ByteOrder.LITTLE_ENDIAN);

        NetworkingDevice localNode = getNodeByUUID(deviceUUID);
        if (localNode != null) {
            MeshLogger.d("device exists");
            return;
        }
        MeshService.getInstance().stopScan();

        int address = mesh.getProvisionIndex();

        MeshLogger.d("alloc address: " + address);
        if (address == -1) {
            enableUI(true);
            return;
        }

        ProvisioningDevice provisioningDevice = new ProvisioningDevice(advertisingDevice.device, deviceUUID, address);
        provisioningDevice.setOobInfo(oobInfo);

        // check if oob exists
        byte[] oob = MeshInfoService.getInstance().getOobByDeviceUUID(deviceUUID);
        if (oob != null) {
            provisioningDevice.setAuthValue(oob);
        } else {
            final boolean autoUseNoOOB = SharedPreferenceHelper.isNoOOBEnable(this);
            provisioningDevice.setAutoUseNoOOB(autoUseNoOOB);
        }
        // for static oob test
        /*provisioningDevice.setAuthValue(new byte[]{
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
        });*/
        provisioningDevice.setRootCert(CertCacheService.getInstance().getRootCert());
        ProvisioningParameters provisioningParameters = new ProvisioningParameters(provisioningDevice);
        if (MeshService.getInstance().startProvisioning(provisioningParameters)) {

            NodeInfo nodeInfo = new NodeInfo();
            nodeInfo.meshAddress = address;
            nodeInfo.deviceUUID = deviceUUID;
            nodeInfo.macAddress = advertisingDevice.device.getAddress();

            NetworkingDevice device = new NetworkingDevice(nodeInfo);
            device.bluetoothDevice = advertisingDevice.device;
            device.state = NetworkingState.PROVISIONING;
            device.oobInfo = oobInfo;

            devices.add(device);
            mListAdapter.notifyDataSetChanged();
        } else {
            MeshLogger.d("provisioning busy");
        }
    }

    @Override
    public void finish() {
        super.finish();
        MeshService.getInstance().idle(false);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        TelinkMeshApplication.getInstance().removeEventListener(this);
    }


    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.tv_log:
                startActivity(new Intent(this, LogActivity.class));
                break;
        }
    }

    private void enableUI(boolean enable) {
        enableBackNav(enable);
        refreshItem.setVisible(enable);
    }

    @Override
    public void performed(final Event<String> event) {
        super.performed(event);
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (event.getType().equals(ProvisioningEvent.EVENT_TYPE_PROVISION_SUCCESS)) {
                    onProvisionSuccess((ProvisioningEvent) event);
                } else if (event.getType().equals(ScanEvent.EVENT_TYPE_SCAN_TIMEOUT)) {
                    enableUI(true);
                } else if (event.getType().equals(ProvisioningEvent.EVENT_TYPE_PROVISION_FAIL)) {
                    onProvisionFail((ProvisioningEvent) event);
                    startScan();
                } else if (event.getType().equals(BindingEvent.EVENT_TYPE_BIND_SUCCESS)) {
                    onKeyBindSuccess((BindingEvent) event);

                } else if (event.getType().equals(BindingEvent.EVENT_TYPE_BIND_FAIL)) {
                    onKeyBindFail((BindingEvent) event);
                    startScan();
                } else if (event.getType().equals(ScanEvent.EVENT_TYPE_DEVICE_FOUND)) {
                    AdvertisingDevice device = ((ScanEvent) event).getAdvertisingDevice();
                    onDeviceFound(device);
                } else if (event.getType().equals(ModelPublicationStatusMessage.class.getName())) {
                    MeshLogger.d("pub setting status: " + isPubSetting);
                    if (!isPubSetting) {
                        return;
                    }
                    mHandler.removeCallbacks(timePubSetTimeoutTask);
                    final ModelPublicationStatusMessage statusMessage = (ModelPublicationStatusMessage) ((StatusNotificationEvent) event).getNotificationMessage().getStatusMessage();

                    if (statusMessage.getStatus() == ConfigStatus.SUCCESS.code) {
                        onTimePublishComplete(true, "time pub set success");
                    } else {
                        onTimePublishComplete(false, "time pub set status err: " + statusMessage.getStatus());
                        MeshLogger.log("publication err: " + statusMessage.getStatus());
                    }
                }
            }
        });

    }


    private void onProvisionFail(ProvisioningEvent event) {
//        ProvisioningDevice deviceInfo = event.getProvisioningDevice();

        NetworkingDevice pvDevice = getProcessingNode();
        pvDevice.state = NetworkingState.PROVISION_FAIL;

        pvDevice.addLog("Provisioning", event.getDesc());
        mListAdapter.notifyDataSetChanged();
    }

    private void onProvisionSuccess(ProvisioningEvent event) {

        ProvisioningDevice remote = event.getProvisioningDevice();

        NetworkingDevice networkingDevice = getProcessingNode();
        NodeInfo nodeInfo = networkingDevice.nodeInfo;
        networkingDevice.state = NetworkingState.BINDING;
        nodeInfo.elementCnt = remote.getDeviceCapability().eleNum;
        nodeInfo.deviceKey = remote.getDeviceKey();
        nodeInfo.netKeyIndexes.add(MeshUtils.intToHex2(mesh.getDefaultNetKey().index));
        mesh.insertDevice(nodeInfo, true);

        // check if private mode opened
        final boolean privateMode = SharedPreferenceHelper.isPrivateMode(this);

        // check if device support fast bind
        boolean defaultBound = false;
        if (privateMode && remote.getDeviceUUID() != null) {
            PrivateDevice device = PrivateDevice.filter(remote.getDeviceUUID());
            if (device != null) {
                MeshLogger.d("private device");
                final byte[] cpsData = device.getCpsData();
                nodeInfo.compositionData = CompositionData.from(cpsData);
                defaultBound = true;
            } else {
                MeshLogger.d("private device null");
            }
        }

        nodeInfo.setDefaultBind(defaultBound);
        mListAdapter.notifyDataSetChanged();
        int appKeyIndex = mesh.getDefaultAppKeyIndex();
        BindingDevice bindingDevice = new BindingDevice(nodeInfo.meshAddress, nodeInfo.deviceUUID, appKeyIndex);
        bindingDevice.setDefaultBound(defaultBound);
        bindingDevice.setBearer(BindingBearer.GattOnly);
//        bindingDevice.setDefaultBound(false);
        MeshService.getInstance().startBinding(new BindingParameters(bindingDevice));
    }


    private void onKeyBindSuccess(BindingEvent event) {
        BindingDevice remote = event.getBindingDevice();
        NetworkingDevice deviceInList = getProcessingNode();
        deviceInList.state = NetworkingState.BIND_SUCCESS;

        // if is default bound, composition data has been valued ahead of binding action
        if (!remote.isDefaultBound()) {
            deviceInList.nodeInfo.compositionData = remote.getCompositionData();
        }
        deviceInList.nodeInfo.bound = true;
        mListAdapter.notifyDataSetChanged();
        deviceInList.nodeInfo.save();
//        mesh.saveOrUpdate(DeviceAutoProvisionActivity.this);

        if (setTimePublish(deviceInList.nodeInfo)) {
            isPubSetting = true;
            MeshLogger.d("waiting for time publication status");
        } else {
            startScan();
        }
    }

    private boolean setTimePublish(NodeInfo nodeInfo) {
        int modelId = MeshSigModel.SIG_MD_TIME_S.modelId;
        int pubEleAdr = nodeInfo.getTargetEleAdr(modelId);
        if (pubEleAdr != -1) {
            final int period = 30 * 1000;
            final int pubAdr = 0xFFFF;
            int appKeyIndex = TelinkMeshApplication.getInstance().getMeshInfo().getDefaultAppKeyIndex();
            ModelPublication modelPublication = ModelPublication.createDefault(pubEleAdr, pubAdr, appKeyIndex, period, modelId, true);

            ModelPublicationSetMessage publicationSetMessage = new ModelPublicationSetMessage(nodeInfo.meshAddress, modelPublication);
            boolean result = MeshService.getInstance().sendMeshMessage(publicationSetMessage);
            if (result) {
                mHandler.removeCallbacks(timePubSetTimeoutTask);
                mHandler.postDelayed(timePubSetTimeoutTask, 5 * 1000);
            }
            return result;
        } else {
            return false;
        }
    }

    private Runnable timePubSetTimeoutTask = new Runnable() {
        @Override
        public void run() {
            onTimePublishComplete(false, "time pub set timeout");
        }
    };

    private void onTimePublishComplete(boolean success, String desc) {
        MeshLogger.d("pub set complete: " + success + " -- " + desc);
        isPubSetting = false;
        NetworkingDevice deviceInList = getProcessingNode();
        if (deviceInList == null) return;

        deviceInList.state = success ? NetworkingState.TIME_PUB_SET_SUCCESS : NetworkingState.TIME_PUB_SET_FAIL;
        deviceInList.nodeInfo.timePublishConfigured = true;
        deviceInList.addLog("Time Publish", "desc");
        mListAdapter.notifyDataSetChanged();
        deviceInList.nodeInfo.save();
        startScan();
    }

    private void onKeyBindFail(BindingEvent event) {
//        BindingDevice remote = event.getBindingDevice();
        NetworkingDevice deviceInList = getProcessingNode();
        if (deviceInList == null) return;

        deviceInList.state = NetworkingState.BIND_FAIL;
        deviceInList.addLog("Binding", event.getDesc());
        mListAdapter.notifyDataSetChanged();
//        mesh.saveOrUpdate();
    }

    private NetworkingDevice getProcessingNode() {
        return this.devices.get(this.devices.size() - 1);
    }


    private NetworkingDevice getNodeByUUID(byte[] deviceUUID) {
        for (NetworkingDevice device : this.devices) {
            if (Arrays.equals(deviceUUID, device.nodeInfo.deviceUUID)) {
                return device;
            }
        }
        return null;
    }
}
