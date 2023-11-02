/********************************************************************************************************
 * @file SceneSettingActivity.java
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
import android.text.TextUtils;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.widget.Toolbar;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.google.android.material.textfield.TextInputEditText;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.core.message.NotificationMessage;
import com.telink.ble.mesh.core.message.config.ConfigStatus;
import com.telink.ble.mesh.core.message.scene.SceneDeleteMessage;
import com.telink.ble.mesh.core.message.scene.SceneRegisterStatusMessage;
import com.telink.ble.mesh.core.message.scene.SceneStoreMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.Scene;
import com.telink.ble.mesh.model.db.MeshInfoService;
import com.telink.ble.mesh.ui.adapter.NodeElementSelectAdapter;
import com.telink.ble.mesh.util.MeshLogger;

import java.util.List;

/**
 * Scene Setting
 * Created by kee on 2018/9/18.
 */
public class SceneSettingActivity extends BaseActivity implements View.OnClickListener, EventListener<String> {

    private NodeElementSelectAdapter mDeviceAdapter;
    //    private int sceneId;
    private Scene scene;
    private MeshInfo mesh;
    private List<NodeInfo> devices;
    private TextView tv_scene_name;

    private Handler delayHandler = new Handler();

    /**
     * add or remove from list
     */
    private SettingModel settingModel;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_scene_setting);
        mesh = TelinkMeshApplication.getInstance().getMeshInfo();

        int sceneId = getIntent().getIntExtra("sceneId", -1);
        if (sceneId == -1) {
            finish();
            Toast.makeText(getApplicationContext(), "no available scene id", Toast.LENGTH_SHORT).show();
            return;
        }

        scene = mesh.getSceneById(sceneId);

        initView();

        // mesh interface
        TelinkMeshApplication.getInstance().addEventListener(SceneRegisterStatusMessage.class.getName(), this);
    }

    private void initView() {
        setTitle("Scene Setting", scene.name);
        enableBackNav(true);
        Toolbar toolbar = findViewById(R.id.title_bar);
        toolbar.inflateMenu(R.menu.menu_single);
        toolbar.getMenu().findItem(R.id.item_add).setIcon(R.drawable.ic_edit);
        toolbar.setOnMenuItemClickListener(item -> {
            showEditNameDialog();
            return false;
        });


        RecyclerView rv_device = findViewById(R.id.rv_device);

        rv_device.setLayoutManager(new LinearLayoutManager(this));
        initData();
        mDeviceAdapter = new NodeElementSelectAdapter(this, devices);
        rv_device.setAdapter(mDeviceAdapter);
    }

    TextInputEditText et_single_input;

    private void showEditNameDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("Update Scene Name")
                .setView(R.layout.dialog_single_input)
                .setPositiveButton("Confirm", (dialog, which) -> updateSceneName(et_single_input.getText().toString()))
                .setNegativeButton("Cancel", (dialog, which) -> dialog.cancel());
        AlertDialog dialog = builder.show();
        et_single_input = dialog.findViewById(R.id.et_single_input);
        et_single_input.setText(scene.name);
        et_single_input.setHint("please input new scene name");
    }

    private void updateSceneName(String sceneName) {
        if (TextUtils.isEmpty(sceneName)) {
            toastMsg("scene name can not be null");
            return;
        }
        scene.name = sceneName;
        MeshInfoService.getInstance().updateScene(scene);
        this.setSubTitle(sceneName);
    }

    private void initData() {
        devices = mesh.nodes;

        // init element selection state
        for (NodeInfo node : devices) {
            if (node.compositionData == null) continue;
            for (int i = 0; i < node.compositionData.elements.size(); i++) {
                node.compositionData.elements.get(i).selected = scene.contains(node.meshAddress + i);
            }
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        delayHandler.removeCallbacksAndMessages(null);
        TelinkMeshApplication.getInstance().removeEventListener(this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {

        }
    }

    public void setScene(int elementAddress, boolean selected, NodeInfo nodeInfo) {
        this.settingModel = new SettingModel(elementAddress, selected, nodeInfo);
        MeshMessage meshMessage;
        int appKeyIndex = TelinkMeshApplication.getInstance().getMeshInfo().getDefaultAppKeyIndex();
        if (selected) {
            meshMessage = SceneStoreMessage.getSimple(elementAddress,
                    appKeyIndex,
                    scene.sceneId,
                    true, 1);
        } else {
            meshMessage = SceneDeleteMessage.getSimple(elementAddress,
                    appKeyIndex,
                    scene.sceneId,
                    true, 1);
        }
        showWaitingDialog("setting...");
        delayHandler.postDelayed(cmdTimeoutCheckTask, 5 * 1000);
        MeshService.getInstance().sendMeshMessage(meshMessage);
    }

    private Runnable cmdTimeoutCheckTask = this::dismissWaitingDialog;


    @Override
    public void performed(Event<String> event) {
        if (event.getType().equals(SceneRegisterStatusMessage.class.getName())) {
            StatusNotificationEvent statusNotificationEvent = (StatusNotificationEvent) event;
            NotificationMessage notificationMessage = statusNotificationEvent.getNotificationMessage();
            SceneRegisterStatusMessage sceneRegisterStatusMessage = (SceneRegisterStatusMessage) notificationMessage.getStatusMessage();
            SettingModel model = this.settingModel;
            if (model == null) {
                MeshLogger.d("scene setting null");
                return;
            }
            if (sceneRegisterStatusMessage.getStatusCode() == ConfigStatus.SUCCESS.code
                    && notificationMessage.getSrc() == model.address) {
                onSettingComplete(model);
            }
        }
    }

    private void onSettingComplete(SettingModel settingModel) {
        delayHandler.removeCallbacks(cmdTimeoutCheckTask);
        if (settingModel.add) {
            scene.save(settingModel.address);
        } else {
            scene.remove(settingModel.address);
        }
        settingModel.nodeInfo.compositionData.elements.get(settingModel.address - settingModel.nodeInfo.meshAddress).selected
                = settingModel.add;
        runOnUiThread(() -> {
            dismissWaitingDialog();
            mDeviceAdapter.notifyDataSetChanged();
        });

    }

    private static class SettingModel {
        int address;
        boolean add;
        NodeInfo nodeInfo;

        public SettingModel(int address, boolean add, NodeInfo nodeInfo) {
            this.address = address;
            this.add = add;
            this.nodeInfo = nodeInfo;
        }
    }
}
