/********************************************************************************************************
 * @file FastProvisionActivity.java
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
import android.util.SparseIntArray;
import android.view.MenuItem;

import androidx.appcompat.widget.Toolbar;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.entity.CompositionData;
import com.telink.ble.mesh.entity.FastProvisioningConfiguration;
import com.telink.ble.mesh.entity.FastProvisioningDevice;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.FastProvisioningEvent;
import com.telink.ble.mesh.foundation.event.MeshEvent;
import com.telink.ble.mesh.foundation.parameter.FastProvisioningParameters;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.NetworkingDevice;
import com.telink.ble.mesh.model.NetworkingState;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.PrivateDevice;
import com.telink.ble.mesh.ui.adapter.DeviceAutoProvisionListAdapter;
import com.telink.ble.mesh.util.Arrays;

import java.util.ArrayList;
import java.util.List;

/**
 * fast provision
 */

public class FastProvisionActivity extends BaseActivity implements EventListener<String> {

    private MeshInfo meshInfo;

    /**
     * ui devices
     */
    private List<NetworkingDevice> devices = new ArrayList<>();

    private DeviceAutoProvisionListAdapter mListAdapter;

    private Handler delayHandler = new Handler();

    private PrivateDevice[] targetDevices = PrivateDevice.values();

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
//        btn_back = findViewById(R.id.btn_back);
//        btn_back.setOnClickListener(this);

        meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_DISCONNECTED, this);

        TelinkMeshApplication.getInstance().addEventListener(FastProvisioningEvent.EVENT_TYPE_FAST_PROVISIONING_ADDRESS_SET, this);
        TelinkMeshApplication.getInstance().addEventListener(FastProvisioningEvent.EVENT_TYPE_FAST_PROVISIONING_FAIL, this);
        TelinkMeshApplication.getInstance().addEventListener(FastProvisioningEvent.EVENT_TYPE_FAST_PROVISIONING_SUCCESS, this);

        actionStart();
    }

    private void initTitle() {
        Toolbar toolbar = findViewById(R.id.title_bar);
        toolbar.inflateMenu(R.menu.device_scan);
        setTitle("Device Scan(Fast)");

        MenuItem refreshItem = toolbar.getMenu().findItem(R.id.item_refresh);
        refreshItem.setVisible(false);
        toolbar.setNavigationIcon(null);
    }

    private void actionStart() {
        enableUI(false);

        int provisionIndex = meshInfo.getProvisionIndex();
        SparseIntArray targetDevicePid = new SparseIntArray(targetDevices.length);

        CompositionData compositionData;
        for (PrivateDevice privateDevice : targetDevices) {
            compositionData = CompositionData.from(privateDevice.getCpsData());
            targetDevicePid.put(privateDevice.getPid(), compositionData.elements.size());
        }
        MeshService.getInstance().startFastProvision(new FastProvisioningParameters(FastProvisioningConfiguration.getDefault(
                provisionIndex,
                targetDevicePid
        )));

    }

    private void enableUI(boolean enable) {
        enableBackNav(enable);
    }


    private void onDeviceFound(FastProvisioningDevice fastProvisioningDevice) {
        NodeInfo nodeInfo = new NodeInfo();
        nodeInfo.meshAddress = fastProvisioningDevice.getNewAddress();

        nodeInfo.deviceUUID = new byte[16];
        System.arraycopy(fastProvisioningDevice.getMac(), 0, nodeInfo.deviceUUID, 0, 6);
        nodeInfo.macAddress = Arrays.bytesToHexString(fastProvisioningDevice.getMac(), ":");
        nodeInfo.deviceKey = fastProvisioningDevice.getDeviceKey();
        nodeInfo.elementCnt = fastProvisioningDevice.getElementCount();
        nodeInfo.compositionData = CompositionData.from(getCompositionData(fastProvisioningDevice.getPid()));

        NetworkingDevice device = new NetworkingDevice(nodeInfo);
        device.state = NetworkingState.PROVISIONING;
        devices.add(device);
        mListAdapter.notifyDataSetChanged();
        meshInfo.increaseProvisionIndex(fastProvisioningDevice.getElementCount());
    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
        TelinkMeshApplication.getInstance().removeEventListener(this);
        delayHandler.removeCallbacksAndMessages(null);
    }

    @Override
    public void performed(final Event<String> event) {
        super.performed(event);
        String eventType = event.getType();

        if (eventType.equals(FastProvisioningEvent.EVENT_TYPE_FAST_PROVISIONING_ADDRESS_SET)) {
            onDeviceFound(((FastProvisioningEvent) event).getFastProvisioningDevice());
        } else if (eventType.equals(FastProvisioningEvent.EVENT_TYPE_FAST_PROVISIONING_FAIL)) {
            onFastProvisionComplete(false);
        } else if (eventType.equals(FastProvisioningEvent.EVENT_TYPE_FAST_PROVISIONING_SUCCESS)) {
            onFastProvisionComplete(true);
        }
    }

    private void onFastProvisionComplete(boolean success) {
        for (NetworkingDevice networkingDevice : devices) {
            if (success) {
                networkingDevice.state = NetworkingState.BIND_SUCCESS;
                networkingDevice.nodeInfo.bound = true;
                meshInfo.insertDevice(networkingDevice.nodeInfo, false);
            } else {
                networkingDevice.state = NetworkingState.PROVISION_FAIL;
            }
        }
        mListAdapter.notifyDataSetChanged();
        enableUI(true);
    }

    private byte[] getCompositionData(int pid) {
        for (PrivateDevice privateDevice : targetDevices) {
            if (pid == privateDevice.getPid()) {
                return privateDevice.getCpsData();
            }
        }
        return null;
    }
}
