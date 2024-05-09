/********************************************************************************************************
 * @file DirectForwardingActivity.java
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
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.annotation.Nullable;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.config.SubnetBridgeStatusMessage;
import com.telink.ble.mesh.core.message.directforwarding.DirectedControlStatusMessage;
import com.telink.ble.mesh.core.message.directforwarding.ForwardingTableAddMessage;
import com.telink.ble.mesh.core.message.directforwarding.ForwardingTableStatusMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;
import com.telink.ble.mesh.model.DirectForwardingInfo;
import com.telink.ble.mesh.model.DirectForwardingInfoService;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.OnlineState;
import com.telink.ble.mesh.ui.adapter.SelectedDeviceAdapter;
import com.telink.ble.mesh.util.MeshLogger;

import java.util.ArrayList;
import java.util.List;

/**
 * network key in target device
 */
public class DirectForwardingActivity extends BaseActivity implements EventListener<String>, View.OnClickListener {

    private static final int REQ_CODE_SELECT_ORIGIN = 0x01;

    private static final int REQ_CODE_SELECT_TARGET = 0x02;

    private static final int REQ_CODE_SELECT_ROUTE = 0x03;

    private MeshInfo meshInfo;
    private Handler handler = new Handler();

    //    private NodeInfo selectedOrigin;
    private int selectedOrigin;

    //    private NodeInfo selectedTarget;
    private int selectedTarget;

    // origin + route + target
    private ArrayList<Integer> routeNodes = new ArrayList<>();

    private int settingIndex = -1;

    private ImageView iv_target, iv_origin;

    private TextView tv_target, tv_origin;

    private SelectedDeviceAdapter deviceAdapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_direct_forwarding);
        setTitle("Add Forwarding Table");
        enableBackNav(true);

        initView();
        updateOriginInfo();
        updateTargetInfo();
        meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
        TelinkMeshApplication.getInstance().addEventListener(DirectedControlStatusMessage.class.getName(), this);
        TelinkMeshApplication.getInstance().addEventListener(ForwardingTableStatusMessage.class.getName(), this);
    }


    private void initView() {
        findViewById(R.id.tv_select_origin).setOnClickListener(this);
        findViewById(R.id.tv_select_target).setOnClickListener(this);
        findViewById(R.id.tv_select_route).setOnClickListener(this);

        findViewById(R.id.btn_save).setOnClickListener(this);

        iv_target = findViewById(R.id.iv_target);
        tv_target = findViewById(R.id.tv_target);

        iv_origin = findViewById(R.id.iv_origin);
        tv_origin = findViewById(R.id.tv_origin);

        RecyclerView rv_route_nodes = findViewById(R.id.rv_route_nodes);
        rv_route_nodes.setLayoutManager(new LinearLayoutManager(this));

        deviceAdapter = new SelectedDeviceAdapter(this);
        rv_route_nodes.setAdapter(deviceAdapter);
    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
        handler.removeCallbacksAndMessages(null);
        TelinkMeshApplication.getInstance().removeEventListener(this);
    }

    private Runnable timeoutTask = () -> {
        toastMsg("processing timeout");
        dismissWaitingDialog();
    };


    private void save() {

        if (selectedOrigin == 0) {
            toastMsg("select origin");
            return;
        }

        if (selectedTarget == 0) {
            toastMsg("select target");
            return;
        }

        if (routeNodes == null || routeNodes.size() == 0) {
            toastMsg("select nodes on route");
            return;
        }

        boolean exist = DirectForwardingInfoService.getInstance().exists(selectedOrigin, selectedTarget);
        if (exist) {
            toastMsg("the same origin and target already exists");
            return;
        }
//        allNodes.add(0, selectedOrigin);
//        allNodes.add(selectedTarget);
        showWaitingDialog("setting direct forwarding...");
        handler.postDelayed(timeoutTask, 10 * 1000);
        settingIndex = -2;
        setNextDevice();
    }

    private void setNextDevice() {
        MeshLogger.d("set next device: " + settingIndex);
        if (settingIndex >= routeNodes.size()) {
            toastMsg("set complete");
            dismissWaitingDialog();
            DirectForwardingInfo info = new DirectForwardingInfo();
//            info.originAdr = meshInfo.localAddress;
            info.originAdr = selectedOrigin;
            info.target = this.selectedTarget;
            /*List<Integer> addresses = new ArrayList<>();
            for (NodeInfo node:this.nodesOnRoute) {
                addresses.add(node.meshAddress);
            }*/
            info.nodesOnRoute = this.routeNodes;

            DirectForwardingInfoService.getInstance().addItem(info);
            setResult(RESULT_OK);
            finish();
            return;
        }

        int address;
        if (settingIndex == -2) {
            address = selectedOrigin;
        } else if (settingIndex == -1) {
            address = selectedTarget;
        } else {
            address = routeNodes.get(settingIndex);
        }
        NodeInfo node = meshInfo.getDeviceByMeshAddress(address);

        ForwardingTableAddMessage tableAddMessage = new ForwardingTableAddMessage(node.meshAddress);
        tableAddMessage.netKeyIndex = meshInfo.getDefaultNetKey().index;
        tableAddMessage.unicastDestinationFlag = 1;
        tableAddMessage.backwardPathValidatedFlag = 1;

        // origin address : use local address
        tableAddMessage.pathOriginUnicastAddrRange = MeshUtils.getUnicastRange(selectedOrigin, 1);

        // target address range
        tableAddMessage.pathTargetUnicastAddrRange = MeshUtils.getUnicastRange(selectedTarget, node.elementCnt);

        if (node.meshAddress == selectedOrigin) { // origin
            tableAddMessage.bearerTowardPathOrigin = 0; // unsigned
            tableAddMessage.bearerTowardPathTarget = 1; // adv
        } else if (node.meshAddress == selectedTarget) { // target
            tableAddMessage.bearerTowardPathOrigin = 1; // unsigned
            tableAddMessage.bearerTowardPathTarget = 0; // adv
        } else {
            tableAddMessage.bearerTowardPathOrigin = 1; // adv
            tableAddMessage.bearerTowardPathTarget = 1; // adv
        }

        MeshService.getInstance().sendMeshMessage(tableAddMessage);
    }

    @Override
    public void performed(Event<String> event) {
        if (event.getType().equals(DirectedControlStatusMessage.class.getName())) {
            SubnetBridgeStatusMessage bridgeStatusMessage = (SubnetBridgeStatusMessage) ((StatusNotificationEvent) event).getNotificationMessage().getStatusMessage();
            int state = bridgeStatusMessage.getSubnetBridgeState();

//            TelinkMeshApplication.getInstance().getMeshInfo().saveOrUpdate(this);
            handler.removeCallbacksAndMessages(null);
        } else if (event.getType().equals(ForwardingTableStatusMessage.class.getName())) {
            ForwardingTableStatusMessage tableStatusMessage = (ForwardingTableStatusMessage) ((StatusNotificationEvent) event).getNotificationMessage().getStatusMessage();
            MeshLogger.d("tableStatusMessage - " + tableStatusMessage.toString());
            if (tableStatusMessage.status == 0) {
                settingIndex++;
                setNextDevice();
            }
        }
    }


    private void updateOriginInfo() {
        if (selectedOrigin == 0) {
            iv_origin.setVisibility(View.INVISIBLE);
            tv_origin.setVisibility(View.INVISIBLE);
            return;
        } else {
            iv_origin.setVisibility(View.VISIBLE);
            tv_origin.setVisibility(View.VISIBLE);
        }
        NodeInfo node = meshInfo.getDeviceByMeshAddress(selectedOrigin);
        int pid = node.compositionData != null ? node.compositionData.pid : 0;
        iv_origin.setImageResource(IconGenerator.getIcon(node, OnlineState.ON));
//        tv_origin.setText(String.format("Node-%04X", selectedOrigin));
        tv_origin.setText(String.format("Name-%s\nAdr-0x%04X", node.getName(), node.meshAddress));
    }

    private void updateTargetInfo() {
        if (selectedTarget == 0) {
            iv_target.setVisibility(View.INVISIBLE);
            tv_target.setVisibility(View.INVISIBLE);
            return;
        } else {
            iv_target.setVisibility(View.VISIBLE);
            tv_target.setVisibility(View.VISIBLE);
        }
        NodeInfo node = meshInfo.getDeviceByMeshAddress(selectedTarget);
        int pid = node.compositionData != null ? node.compositionData.pid : 0;
        iv_target.setImageResource(IconGenerator.getIcon(node, OnlineState.ON));
        tv_origin.setText(String.format("Name-%s\nAdr-0x%04X", node.getName(), node.meshAddress));
//        tv_target.setText(String.format("Node-%04X", selectedTarget));
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (resultCode != RESULT_OK || data == null) return;
        ArrayList<Integer> adrList = data.getIntegerArrayListExtra(DeviceSelectActivity.KEY_SELECTED_DEVICES);
        if (adrList == null) {
            return;
        }
        if (requestCode == REQ_CODE_SELECT_ORIGIN) {
            if (adrList.size() != 1) {
                toastMsg("select only one node");
                return;
            }
            selectedOrigin = adrList.get(0);
//            allNodes.add(selectedOrigin);
            updateOriginInfo();
//            tv_target.setText(String.format("selected device: \n%04X", selectedTarget.meshAddress));
        } else if (requestCode == REQ_CODE_SELECT_TARGET) {
            if (adrList.size() != 1) {
                toastMsg("select only one node");
                return;
            }
            selectedTarget = adrList.get(0);
//            allNodes.add(selectedTarget);
            updateTargetInfo();
//            tv_target.setText(String.format("selected device: \n%04X", selectedTarget.meshAddress));
        } else if (requestCode == REQ_CODE_SELECT_ROUTE) {
            routeNodes = (adrList);
            StringBuilder routeText = new StringBuilder("selected route: \n");
            List<NodeInfo> devices = new ArrayList<>();
            for (int i = 0; i < adrList.size(); i++) {
                devices.add(meshInfo.getDeviceByMeshAddress(adrList.get(i)));
                routeText.append(String.format("%04X", adrList.get(i)));
                if (i != adrList.size() - 1) {
                    routeText.append("\n");
                }
            }
            deviceAdapter.resetDevices(devices);
        }
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {

            case R.id.tv_select_origin:
                startActivityForResult(
                        new Intent(this, DeviceSelectActivity.class).putExtra(DeviceSelectActivity.KEY_TITLE, "Select Origin"),
                        REQ_CODE_SELECT_ORIGIN);
                break;

            case R.id.tv_select_target:
                startActivityForResult(
                        new Intent(this, DeviceSelectActivity.class).putExtra(DeviceSelectActivity.KEY_TITLE, "Select Target"),
                        REQ_CODE_SELECT_TARGET);
                break;

            case R.id.tv_select_route:
                startActivityForResult(
                        new Intent(this, DeviceSelectActivity.class).putExtra(DeviceSelectActivity.KEY_TITLE, "Select Route"),
                        REQ_CODE_SELECT_ROUTE);
                break;

            case R.id.btn_save:
                save();
                break;

        }
    }
}
