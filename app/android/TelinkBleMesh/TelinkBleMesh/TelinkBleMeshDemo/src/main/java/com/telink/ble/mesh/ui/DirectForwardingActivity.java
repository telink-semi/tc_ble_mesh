/********************************************************************************************************
 * @file SubnetBridgeActivity.java
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

    private static final int REQ_CODE_SELECT_TARGET = 0x01;

    private static final int REQ_CODE_SELECT_ROUTE = 0x02;

    private MeshInfo meshInfo;
    private Handler handler = new Handler();

    private NodeInfo selectedTarget;

    private List<NodeInfo> nodesOnRoute;

    private int settingIndex = -1;

    private ImageView iv_target;

    private TextView tv_target;

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
        updateTargetInfo();
        meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
        TelinkMeshApplication.getInstance().addEventListener(DirectedControlStatusMessage.class.getName(), this);
        TelinkMeshApplication.getInstance().addEventListener(ForwardingTableStatusMessage.class.getName(), this);
    }


    private void initView() {
        findViewById(R.id.tv_select_target).setOnClickListener(this);
        findViewById(R.id.tv_select_route).setOnClickListener(this);

        findViewById(R.id.btn_save).setOnClickListener(this);

        iv_target = findViewById(R.id.iv_target);
        tv_target = findViewById(R.id.tv_target);

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
        if (selectedTarget == null) {
            toastMsg("select target");
            return;
        }

        if (nodesOnRoute == null || nodesOnRoute.size() == 0) {
            toastMsg("select nodes on route");
            return;
        }
        showWaitingDialog("setting direct forwarding...");
        handler.postDelayed(timeoutTask, 10 * 1000);
        settingIndex = 0;
        setNextDevice();
    }

    private void setNextDevice() {
        MeshLogger.d("set next device: " + settingIndex);
        if (settingIndex >= nodesOnRoute.size()) {
            toastMsg("set complete");
            dismissWaitingDialog();
            DirectForwardingInfo info = new DirectForwardingInfo();
            info.originAdr = meshInfo.localAddress;
            info.target = this.selectedTarget;
            info.nodesOnRoute = this.nodesOnRoute;
            DirectForwardingInfoService.getInstance().addItem(info);
            setResult(RESULT_OK);
            finish();
            return;
        }
        NodeInfo node = nodesOnRoute.get(settingIndex);
        ForwardingTableAddMessage tableAddMessage = new ForwardingTableAddMessage(node.meshAddress);
        tableAddMessage.netKeyIndex = meshInfo.getDefaultNetKey().index;
        tableAddMessage.unicastDestinationFlag = 1;
        tableAddMessage.backwardPathValidatedFlag = 1;

        // origin address : use local address
        tableAddMessage.pathOriginUnicastAddrRange = MeshUtils.getUnicastRange(meshInfo.localAddress, 1);

        // target address range
        tableAddMessage.pathTargetUnicastAddrRange = MeshUtils.getUnicastRange(selectedTarget.meshAddress, node.elementCnt);

        if (node.meshAddress == selectedTarget.meshAddress) {
            tableAddMessage.bearerTowardPathTarget = (byte) 0; // unsigned
        } else {
            tableAddMessage.bearerTowardPathTarget = (byte) 1; // adv
        }
        tableAddMessage.bearerTowardPathOrigin = (byte) 1; // adv

        MeshService.getInstance().sendMeshMessage(tableAddMessage);
    }

    @Override
    public void performed(Event<String> event) {
        if (event.getType().equals(DirectedControlStatusMessage.class.getName())) {
            SubnetBridgeStatusMessage bridgeStatusMessage = (SubnetBridgeStatusMessage) ((StatusNotificationEvent) event).getNotificationMessage().getStatusMessage();
            int state = bridgeStatusMessage.getSubnetBridgeState();
            TelinkMeshApplication.getInstance().getMeshInfo().saveOrUpdate(this);
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

    private void updateTargetInfo() {
        if (selectedTarget == null) {
            iv_target.setVisibility(View.INVISIBLE);
            tv_target.setVisibility(View.INVISIBLE);
            return;
        } else {
            iv_target.setVisibility(View.VISIBLE);
            tv_target.setVisibility(View.VISIBLE);
        }
        int pid = selectedTarget.compositionData != null ? selectedTarget.compositionData.pid : 0;
        iv_target.setImageResource(IconGenerator.getIcon(pid, OnlineState.ON));
        tv_target.setText(String.format("Node-%04X", selectedTarget.meshAddress));
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (resultCode != RESULT_OK || data == null) return;
        ArrayList<Integer> adrList = data.getIntegerArrayListExtra(DeviceSelectActivity.KEY_SELECTED_DEVICES);
        if (adrList == null) {
            return;
        }
        if (requestCode == REQ_CODE_SELECT_TARGET) {
            if (adrList.size() != 1) {
                toastMsg("select only one node");
                return;
            }
            selectedTarget = meshInfo.getDeviceByMeshAddress(adrList.get(0));
            updateTargetInfo();
//            tv_target.setText(String.format("selected device: \n%04X", selectedTarget.meshAddress));
        } else if (requestCode == REQ_CODE_SELECT_ROUTE) {
            nodesOnRoute = new ArrayList<>(adrList.size());
            StringBuilder routeText = new StringBuilder("selected route: \n");
            for (int i = 0; i < adrList.size(); i++) {
                nodesOnRoute.add(meshInfo.getDeviceByMeshAddress(adrList.get(i)));
                routeText.append(String.format("%04X", adrList.get(i)));
                if (i != adrList.size() - 1) {
                    routeText.append("\n");
                }
            }
            deviceAdapter.resetDevices(nodesOnRoute);
        }
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.tv_select_target:
                startActivityForResult(new Intent(this, DeviceSelectActivity.class), REQ_CODE_SELECT_TARGET);
                break;

            case R.id.tv_select_route:
                startActivityForResult(new Intent(this, DeviceSelectActivity.class), REQ_CODE_SELECT_ROUTE);
                break;

            case R.id.btn_save:
                save();
                break;

        }
    }
}
