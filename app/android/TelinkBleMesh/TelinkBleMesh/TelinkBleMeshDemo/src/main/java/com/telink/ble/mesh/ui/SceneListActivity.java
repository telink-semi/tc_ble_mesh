/********************************************************************************************************
 * @file SceneListActivity.java
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
import android.widget.Toast;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.widget.Toolbar;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.google.android.material.textfield.TextInputEditText;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.NotificationMessage;
import com.telink.ble.mesh.core.message.config.ConfigStatus;
import com.telink.ble.mesh.core.message.scene.SceneDeleteMessage;
import com.telink.ble.mesh.core.message.scene.SceneRecallMessage;
import com.telink.ble.mesh.core.message.scene.SceneRegisterStatusMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.Scene;
import com.telink.ble.mesh.ui.adapter.SceneListAdapter;
import com.telink.ble.mesh.util.MeshLogger;

/**
 * Scene List
 * Created by kee on 2018/9/18.
 */
public class SceneListActivity extends BaseActivity implements EventListener<String>, View.OnClickListener {
    private SceneListAdapter mAdapter;
    private Scene tarScene;
    private int deleteIndex;
    private Handler handler = new Handler();
    private View ll_empty;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_common_list);
        ll_empty = findViewById(R.id.ll_empty);
        ll_empty.setVisibility(View.GONE);
        findViewById(R.id.btn_add).setOnClickListener(this);
        Toolbar toolbar = findViewById(R.id.title_bar);
        toolbar.inflateMenu(R.menu.common_list);
        setTitle("Scene List");
        enableBackNav(true);
        toolbar.setOnMenuItemClickListener(item -> {
            if (item.getItemId() == R.id.item_add) {
                onAddClick();
            }
            return false;
        });

        mAdapter = new SceneListAdapter(this);
        mAdapter.setOnItemLongClickListener(position -> {

            tarScene = mAdapter.get(position);
//            boolean hasOffline = false;
//            for (SceneState state : tarScene.states) {
//                // offline
//
////                    NodeInfo localDevice = TelinkMeshApplication.getInstance().getMeshInfo().getDeviceByMeshAddress(state.address);
//                NodeInfo localDevice = state.nodeInfo.getTarget();
//                if (localDevice != null && localDevice.isOffline()) {
//                    hasOffline = true;
//                }
//            }
            showConfirmDialog("Confirm to delete scene ", (dialog, which) -> deleteScene());
            return false;
        });
        RecyclerView rv_scheduler = findViewById(R.id.rv_common);
        rv_scheduler.setLayoutManager(new LinearLayoutManager(this));
        rv_scheduler.setAdapter(mAdapter);
        resetData();
    }

    @Override
    protected void onStart() {
        super.onStart();
        TelinkMeshApplication.getInstance().addEventListener(SceneRegisterStatusMessage.class.getName(), this);
    }

    @Override
    protected void onStop() {
        super.onStop();
        TelinkMeshApplication.getInstance().removeEventListener(this);
    }

    TextInputEditText et_single_input;

    private void onAddClick() {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("Create Scene")
                .setView(R.layout.dialog_single_input)
//                .setMessage("Input Network Name")
                .setPositiveButton("Confirm", (dialog, which) -> createScene(et_single_input.getText().toString()))
                .setNegativeButton("Cancel", (dialog, which) -> dialog.cancel());
        AlertDialog dialog = builder.show();
        et_single_input = dialog.findViewById(R.id.et_single_input); // et_network_name
    }

    private void createScene(String sceneName) {
        if (TextUtils.isEmpty(sceneName)) {
            toastMsg("scene name can not be null");
            return;
        }
        MeshInfo meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
        int sceneId = meshInfo.allocSceneId();
        if (sceneId == -1) {
            finish();
            Toast.makeText(getApplicationContext(), "no available scene id", Toast.LENGTH_SHORT).show();
            return;
        }
        Scene scene = new Scene();
        scene.sceneId = sceneId;
        scene.name = sceneName;
        toastMsg("create scene success");
        meshInfo.addScene(scene);
        resetData();
    }

    private void deleteScene() {
        if (tarScene.addressList == null || tarScene.addressList.size() == 0) {
            mAdapter.remove(tarScene);
            mAdapter.notifyDataSetChanged();
        } else {
            showWaitingDialog("deleting...");
            deleteIndex = 0;
            deleteNextDevice();
        }
    }

    private void deleteNextDevice() {
        if (tarScene == null || deleteIndex > tarScene.addressList.size() - 1) {
            mAdapter.remove(tarScene);
            mAdapter.notifyDataSetChanged();
            tarScene = null;
            dismissWaitingDialog();
            toastMsg("scene deleted");
        } else {
            int address = MeshUtils.hexToIntB(tarScene.addressList.get(deleteIndex));
            // remove offline device
            handler.removeCallbacks(cmdTimeoutCheckTask);
            handler.postDelayed(cmdTimeoutCheckTask, 2000);
            int appKeyIndex = TelinkMeshApplication.getInstance().getMeshInfo().getDefaultAppKeyIndex();
            SceneDeleteMessage deleteMessage = SceneDeleteMessage.getSimple(address,
                    appKeyIndex,
                    tarScene.sceneId,
                    true, 1);
            MeshService.getInstance().sendMeshMessage(deleteMessage);
        }
    }

    private Runnable cmdTimeoutCheckTask = new Runnable() {
        @Override
        public void run() {
            deleteIndex++;
            deleteNextDevice();
        }
    };

    @Override
    protected void onResume() {
        super.onResume();
        resetData();
    }

    private void resetData() {
        mAdapter.resetData();
        if (mAdapter.isEmpty()) {
            ll_empty.setVisibility(View.VISIBLE);
        } else {
            ll_empty.setVisibility(View.GONE);
        }
    }

    @Override
    public void performed(Event<String> event) {
        if (tarScene == null) return;
        if (event.getType().equals(SceneRegisterStatusMessage.class.getName())) {
            StatusNotificationEvent statusEvent = (StatusNotificationEvent) event;
            NotificationMessage notificationMessage = statusEvent.getNotificationMessage();
            if (tarScene.addressList == null || tarScene.addressList.size() <= deleteIndex)
                return;
            SceneRegisterStatusMessage sceneMessage = (SceneRegisterStatusMessage) notificationMessage.getStatusMessage();
            int address = MeshUtils.hexToIntB(tarScene.addressList.get(deleteIndex));
            if (sceneMessage.getStatusCode() == ConfigStatus.SUCCESS.code
                    && notificationMessage.getSrc() == address) {
                handler.removeCallbacks(cmdTimeoutCheckTask);
                deleteIndex++;
                deleteNextDevice();
            } else {
                dismissWaitingDialog();
                MeshLogger.e("scene setting err!");
            }
        }
    }

    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.btn_add) {
            onAddClick();
        }
    }

    public void recall(int address, int sceneId) {
        int appKeyIndex = TelinkMeshApplication.getInstance().getMeshInfo().getDefaultAppKeyIndex();
        SceneRecallMessage recallMessage = SceneRecallMessage.getSimple(address,
                appKeyIndex, sceneId, false, 0);
        MeshService.getInstance().sendMeshMessage(recallMessage);
    }
}
