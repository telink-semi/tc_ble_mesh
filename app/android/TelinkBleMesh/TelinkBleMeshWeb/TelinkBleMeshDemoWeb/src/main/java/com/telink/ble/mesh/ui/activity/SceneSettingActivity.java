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
package com.telink.ble.mesh.ui.activity;

import android.os.Bundle;
import android.os.Handler;
import android.view.View;

import androidx.appcompat.widget.Toolbar;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONException;
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
import com.telink.ble.mesh.model.web.MeshScene;
import com.telink.ble.mesh.ui.adapter.SceneDeviceAdapter;
import com.telink.ble.mesh.util.MeshLogger;
import com.telink.ble.mesh.web.ResponseInfo;
import com.telink.ble.mesh.web.TelinkHttpClient;
import com.telink.ble.mesh.web.WebUtils;
import com.telink.ble.mesh.web.entity.MeshNetworkDetail;
import com.telink.ble.mesh.web.entity.MeshNode;

import java.io.IOException;
import java.util.List;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.FormBody;
import okhttp3.Response;

/**
 * Scene Setting
 * Created by kee on 2018/9/18.
 */
public class SceneSettingActivity extends BaseActivity implements View.OnClickListener, EventListener<String> {

    public final static String URL_GET_INNER_NODES = TelinkHttpClient.URL_BASE + "mesh-scene/getInnerNodes";

    public final static String URL_ADD_NODE_TO_SCENE = TelinkHttpClient.URL_BASE + "mesh-scene/addNodeToScene";

    public final static String URL_REMOVE_NODE_FROM_SCENE = TelinkHttpClient.URL_BASE + "mesh-scene/removeNodeFromScene";

    public final static String URL_DELETE_SCENE = TelinkHttpClient.URL_BASE + "mesh-scene/deleteScene";

    private SceneDeviceAdapter mDeviceAdapter;

    private MeshScene scene;
    private MeshNetworkDetail mesh;
    private MeshNode settingNode;

    private Handler delayHandler = new Handler();


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_mesh_scene_setting);
        mesh = TelinkMeshApplication.getInstance().getMeshInfo();
        scene = (MeshScene) getIntent().getSerializableExtra("scene");
        setTitle("Scene Setting");
        enableBackNav(true);
        Toolbar toolbar = findViewById(R.id.title_bar);
        toolbar.inflateMenu(R.menu.menu_two_action);

        toolbar.setOnMenuItemClickListener(item -> {
            if (item.getItemId() == R.id.item_action_1) {
                showConfirmDialog("delete scene?", (dialog, which) -> deleteScene());
            } else if (item.getItemId() == R.id.item_action_2) {
                getInnerNodes();
            }
            return false;
        });

        RecyclerView rv_device = findViewById(R.id.rv_device);

        rv_device.setLayoutManager(new LinearLayoutManager(this));

        mDeviceAdapter = new SceneDeviceAdapter(this, mesh.nodeList, this::setScene);
        rv_device.setAdapter(mDeviceAdapter);
        TelinkMeshApplication.getInstance().addEventListener(SceneRegisterStatusMessage.class.getName(), this);

        getInnerNodes();
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

    private void setScene(MeshNode node) {
        settingNode = node;
        int appKeyIndex = TelinkMeshApplication.getInstance().getMeshInfo().getDefaultAppKeyIndex();
        MeshMessage meshMessage;
        if (!node.selected) {
            meshMessage = SceneStoreMessage.getSimple(node.address,
                    appKeyIndex,
                    scene.getSceneId(),
                    true, 1);
        } else {
            meshMessage = SceneDeleteMessage.getSimple(node.address,
                    appKeyIndex,
                    scene.getSceneId(),
                    true, 1);
        }
        MeshService.getInstance().sendMeshMessage(meshMessage);

        MeshLogger.log("set scene -  address: " + node.address + " -- " + node.selected);

        // F0:0D:02:00:01:00:82:45:00:01:00:01:00:37:00
//        delayHandler.removeCallbacks(cmdTimeoutCheckTask);
//        delayHandler.postDelayed(cmdTimeoutCheckTask, 2000);

        MeshService.getInstance().sendMeshMessage(meshMessage);
    }


    private Runnable cmdTimeoutCheckTask = () -> {
    };


    @Override
    public void performed(Event<String> event) {
        if (event.getType().equals(SceneRegisterStatusMessage.class.getName())) {
            StatusNotificationEvent statusNotificationEvent = (StatusNotificationEvent) event;
            NotificationMessage notificationMessage = statusNotificationEvent.getNotificationMessage();
            SceneRegisterStatusMessage sceneRegisterStatusMessage = (SceneRegisterStatusMessage) notificationMessage.getStatusMessage();
            if (settingNode == null) {
                return;
            }
            if (sceneRegisterStatusMessage.getStatusCode() == ConfigStatus.SUCCESS.code
                    && notificationMessage.getSrc() == settingNode.address) {
                delayHandler.removeCallbacks(cmdTimeoutCheckTask);
                settingNode.selected = !settingNode.selected;
                updateNodeStateToCloud(settingNode);
            }
        }
    }

    public void deleteScene() {
        FormBody formBody = new FormBody.Builder()
                .add("sceneId", scene.getId() + "")
                .build();
        TelinkHttpClient.getInstance().post(URL_DELETE_SCENE, formBody, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                dismissIndeterminateLoading();
                MeshLogger.e("call fail");
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                dismissIndeterminateLoading();
                ResponseInfo responseInfo = WebUtils.parseResponse(response, SceneSettingActivity.this);
                if (responseInfo == null) return;
                if (!responseInfo.isSuccess()) {
                    showError(responseInfo.message);
                    return;
                }
                runOnUiThread(() -> {
                    toastMsg("delete scene success");
                    setResult(RESULT_OK);
                    finish();
                });

            }
        });
    }

    public void getInnerNodes() {
        showIndeterminateLoading();
        FormBody formBody = new FormBody.Builder()
                .add("sceneId", scene.getId() + "")
                .build();
        TelinkHttpClient.getInstance().post(URL_GET_INNER_NODES, formBody, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                dismissIndeterminateLoading();
                MeshLogger.e("call fail");
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                dismissIndeterminateLoading();
                ResponseInfo responseInfo = WebUtils.parseResponse(response, SceneSettingActivity.this);
                if (responseInfo == null) return;
                if (!responseInfo.isSuccess()) {
                    showError(responseInfo.message);
                    return;
                }

                try {
                    List<MeshNode> list = JSON.parseArray(responseInfo.data, MeshNode.class);
                    runOnUiThread(() -> mDeviceAdapter.updateInnerNodes(list));
                } catch (JSONException e) {
                    e.printStackTrace();
                }
            }
        });
    }


    private void updateNodeStateToCloud(MeshNode node) {
        String url = node.selected ? URL_ADD_NODE_TO_SCENE : URL_REMOVE_NODE_FROM_SCENE;
        FormBody formBody = new FormBody.Builder()
                .add("nodeId", node.id + "")
                .add("sceneId", scene.getId() + "")
                .build();
        TelinkHttpClient.getInstance().post(url, formBody, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                MeshLogger.d("update node scene call onFailure");
                dismissIndeterminateLoading();
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                dismissIndeterminateLoading();
                ResponseInfo responseInfo = WebUtils.parseResponse(response, SceneSettingActivity.this);
                if (responseInfo == null) {
                    return;
                }
                MeshLogger.d("update node scene call response");
                runOnUiThread(() -> mDeviceAdapter.updateNodeState(settingNode));
            }
        });
    }


}
