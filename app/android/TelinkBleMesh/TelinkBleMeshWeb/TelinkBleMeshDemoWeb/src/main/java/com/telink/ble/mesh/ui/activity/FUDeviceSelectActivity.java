/********************************************************************************************************
 * @file FUDeviceSelectActivity.java
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
import android.util.ArrayMap;
import android.view.View;
import android.widget.Button;

import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.message.MeshSigModel;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.entity.MeshUpdatingDevice;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.event.MeshEvent;
import com.telink.ble.mesh.model.NodeStatusChangedEvent;
import com.telink.ble.mesh.ui.adapter.FUProductSelectAdapter;
import com.telink.ble.mesh.web.entity.MeshNetworkDetail;
import com.telink.ble.mesh.web.entity.MeshNode;
import com.telink.ble.mesh.web.entity.MeshProductInfo;

import java.util.ArrayList;
import java.util.List;


/**
 * select target devices for firmware-update
 */
public class FUDeviceSelectActivity extends BaseActivity implements View.OnClickListener, EventListener<String> {

    public static final String KEY_SELECTED_DEVICES = "update-selected-devices";


    /**
     * local mesh info
     */
    private MeshNetworkDetail mesh;

    /**
     * updating devices
     */
    private ArrayList<MeshUpdatingDevice> updatingDevices;

//   private List<MeshProductInfo> productInfoList;

    private ArrayMap<MeshProductInfo, List<MeshNode>> productDevMap;

    private FUProductSelectAdapter productSelectAdapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_fu_device_select);
        mesh = TelinkMeshApplication.getInstance().getMeshInfo();
        setTitle("Select Update Device");
        enableBackNav(true);
        initView();
        initData();
        addEventListeners();
    }

    private void initData() {
        mesh = TelinkMeshApplication.getInstance().getMeshInfo();
        productDevMap = new ArrayMap<>();
        MeshProductInfo productInfo;
        for (MeshNode deviceInfo : mesh.nodeList) {
            deviceInfo.selected = false;
            productInfo = deviceInfo.productInfo;
            List<MeshNode> nodes = productDevMap.get(productInfo);
            if (nodes == null) {
                nodes = new ArrayList<>();
            }
            nodes.add(deviceInfo);
            productDevMap.put(productInfo, nodes);

            /*if (deviceInfo.isLpn()) {
                lpnDevices.add(deviceInfo);
            } else {
                normalDevices.add(deviceInfo);
            }*/
        }

        productSelectAdapter.resetData(productDevMap);
    }

    private void initView() {

        RecyclerView rv_device = findViewById(R.id.rv_product);
        rv_device.setLayoutManager(new LinearLayoutManager(this));

        productSelectAdapter = new FUProductSelectAdapter(this);

        Button btn_confirm = findViewById(R.id.btn_confirm);
        btn_confirm.setOnClickListener(this);

    }

    private void addEventListeners() {
        TelinkMeshApplication.getInstance().addEventListener(NodeStatusChangedEvent.EVENT_TYPE_NODE_STATUS_CHANGED, this);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        TelinkMeshApplication.getInstance().removeEventListener(this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {

            case R.id.btn_confirm:
                List<MeshNode> nodes = getSelectedNodes();
                if (nodes == null) {
                    toastMsg("Pls select at least ONE device");
                    return;
                }
//                updatingDevices = new ArrayList<>();
                ArrayList<Integer> selectedAddresses = new ArrayList<>();
                MeshUpdatingDevice device;
                for (MeshNode node : nodes) {
                    selectedAddresses.add(node.address);
                    /*device = new MeshUpdatingDevice();
                    device.meshAddress = (node.address);
                    device.updatingEleAddress = (node.getTargetEleAdr(MeshSigModel.SIG_MD_OBJ_TRANSFER_S.modelId));
                    device.isSupported = node.getTargetEleAdr(MeshSigModel.SIG_MD_FW_UPDATE_S.modelId) != -1;
                    device.pidInfo = node.getPidDesc();
                    device.pid = node.compositionData().pid;
                    device.isLpn = node.isLpn();
                    updatingDevices.add(device);*/
                }

                Intent intent = new Intent();
                intent.putIntegerArrayListExtra(KEY_SELECTED_DEVICES, selectedAddresses);
//                intent.putParcelableArrayListExtra(KEY_SELECTED_DEVICES, updatingDevices);
                setResult(RESULT_OK, intent);
                finish();
                break;

        }
    }


    public List<MeshNode> getSelectedNodes() {
        List<MeshNode> nodes = null;
        for (MeshNode deviceInfo : TelinkMeshApplication.getInstance().getMeshInfo().nodeList) {
            // deviceInfo.getOnOff() != -1 &&
            if (deviceInfo.selected && deviceInfo.getTargetEleAdr(MeshSigModel.SIG_MD_FW_UPDATE_S.modelId) != -1) {
                if (nodes == null) {
                    nodes = new ArrayList<>();
                }
                nodes.add(deviceInfo);
            }
        }
        return nodes;
    }


    /****************************************************************
     * events - start
     ****************************************************************/
    @Override
    public void performed(Event<String> event) {
        final String eventType = event.getType();
        if (eventType.equals(MeshEvent.EVENT_TYPE_DISCONNECTED)
                || eventType.equals(NodeStatusChangedEvent.EVENT_TYPE_NODE_STATUS_CHANGED)) {
            runOnUiThread(() -> productSelectAdapter.notifyDataSetChanged());
        }
    }


}
