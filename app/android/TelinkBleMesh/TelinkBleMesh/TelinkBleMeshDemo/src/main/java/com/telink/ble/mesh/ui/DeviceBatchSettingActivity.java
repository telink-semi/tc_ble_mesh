/********************************************************************************************************
 * @file DeviceSelectActivity.java
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

import android.content.DialogInterface;
import android.os.Bundle;
import android.os.Handler;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.EditText;

import androidx.appcompat.app.AlertDialog;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.message.config.NodeResetMessage;
import com.telink.ble.mesh.core.message.config.NodeResetStatusMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.MeshEvent;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.NodeStatusChangedEvent;
import com.telink.ble.mesh.model.db.MeshInfoService;
import com.telink.ble.mesh.ui.adapter.DeviceInBatchAdapter;


/**
 * select device
 */
public class DeviceBatchSettingActivity extends BaseActivity implements View.OnClickListener,
        EventListener<String> {
    private Handler delayHandler = new Handler();
    private DeviceInBatchAdapter deviceSelectAdapter;

    /**
     * local mesh info
     */
    private MeshInfo mesh;
    private boolean kickDirect = false;
    private NodeInfo kickingNode = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_single_list);
        mesh = TelinkMeshApplication.getInstance().getMeshInfo();
        setTitle("Device Batch Setting");
        enableBackNav(true);
        initView();
        addEventListeners();
    }

    private void initView() {

        RecyclerView rv_device = findViewById(R.id.rv_toggle);
        rv_device.setLayoutManager(new LinearLayoutManager(this));

        deviceSelectAdapter = new DeviceInBatchAdapter(this, mesh.nodes);
        rv_device.setAdapter(deviceSelectAdapter);
    }

    private void addEventListeners() {
        TelinkMeshApplication.getInstance().addEventListener(NodeStatusChangedEvent.EVENT_TYPE_NODE_STATUS_CHANGED, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_DISCONNECTED, this);

        TelinkMeshApplication.getInstance().addEventListener(NodeResetStatusMessage.class.getName(), this);
    }

    private void kickOut(NodeInfo deviceInfo) {
        // send reset message
        kickingNode = deviceInfo;
        MeshService.getInstance().sendMeshMessage(new NodeResetMessage(deviceInfo.meshAddress));
        kickDirect = deviceInfo.meshAddress == MeshService.getInstance().getDirectConnectedNodeAddress();
        showWaitingDialog("kick out processing...");
        if (!kickDirect) {
            delayHandler.postDelayed(this::onKickOutFinish, 3 * 1000);
        } else {
            delayHandler.postDelayed(this::onKickOutFinish, 10 * 1000);
        }
    }

    private void onKickOutFinish() {
        deviceSelectAdapter.notifyDataSetChanged();
        delayHandler.removeCallbacksAndMessages(null);
        if (kickingNode != null) {
            MeshService.getInstance().removeDevice(kickingNode.meshAddress);
            TelinkMeshApplication.getInstance().getMeshInfo().removeNode(kickingNode);
            kickingNode = null;
        }
        dismissWaitingDialog();
    }

    public void showKickConfirmDialog(NodeInfo node) {
        showConfirmDialog(String.format("Delete Node(%s)", node.getName()), new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                kickOut(node);
            }
        });
    }

    public void showNameInputDialog(NodeInfo node) {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        View view = LayoutInflater.from(this).inflate(R.layout.dialog_single_input, null);
        final EditText et_input = view.findViewById(R.id.et_single_input);
        et_input.setText(node.name);
        et_input.setHint("please input name");
        builder.setTitle("Change Node Name");
        builder.setView(view).setPositiveButton("Confirm", (dialog, which) -> {
            dialog.dismiss();
            String name = et_input.getText().toString().trim();
            if (TextUtils.isEmpty(name)) {
                toastMsg("input empty");
                return;
            }
            node.updateName(name);
            MeshInfoService.getInstance().updateNodeInfo(node);
            deviceSelectAdapter.notifyDataSetChanged();
            toastMsg("rename success");
        }).setNegativeButton("Cancel", (dialog, which) -> dialog.dismiss());
        builder.show();
    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
        TelinkMeshApplication.getInstance().removeEventListener(this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {


        }
    }

    /****************************************************************
     * events - start
     ****************************************************************/
    @Override
    public void performed(Event<String> event) {
        final String eventType = event.getType();
        if (event.getType().equals(MeshEvent.EVENT_TYPE_DISCONNECTED)) {
            if (kickDirect) {
                onKickOutFinish();
            }
            runOnUiThread(() -> deviceSelectAdapter.notifyDataSetChanged());
        } else if (eventType.equals(NodeStatusChangedEvent.EVENT_TYPE_NODE_STATUS_CHANGED)) {
            runOnUiThread(() -> deviceSelectAdapter.notifyDataSetChanged());
        } else if (event.getType().equals(NodeResetStatusMessage.class.getName())) {
            if (!kickDirect) {
                onKickOutFinish();
            }
        }
    }


}
