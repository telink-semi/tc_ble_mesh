/********************************************************************************************************
 * @file     SceneListActivity.java 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/
package com.telink.ble.mesh.ui;

import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.view.MenuItem;
import android.view.View;

import com.telink.ble.mesh.core.message.config.ConfigStatus;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.Scene;
import com.telink.ble.mesh.ui.adapter.BaseRecyclerViewAdapter;
import com.telink.ble.mesh.ui.adapter.SceneListAdapter;
import com.telink.ble.mesh.core.message.MeshSigModel;
import com.telink.ble.mesh.core.message.NotificationMessage;
import com.telink.ble.mesh.core.message.config.ConfigMessage;
import com.telink.ble.mesh.core.message.scene.SceneDeleteMessage;
import com.telink.ble.mesh.core.message.scene.SceneRegisterStatusMessage;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;
import com.telink.ble.mesh.util.MeshLogger;


import java.util.List;

import androidx.appcompat.widget.Toolbar;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

/**
 * Scene List
 * Created by kee on 2018/9/18.
 */
public class SceneListActivity extends BaseActivity implements EventListener<String>, View.OnClickListener {
    private SceneListAdapter mAdapter;
    List<Scene> sceneList;
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
        findViewById(R.id.btn_add).setOnClickListener(this);
        Toolbar toolbar = findViewById(R.id.title_bar);
        toolbar.inflateMenu(R.menu.common_list);
        setTitle("Scene List");
        enableBackNav(true);
        toolbar.setOnMenuItemClickListener(new Toolbar.OnMenuItemClickListener() {
            @Override
            public boolean onMenuItemClick(MenuItem item) {
                if (item.getItemId() == R.id.item_add) {
                    onAddClick();
                }
                return false;
            }
        });

        sceneList = TelinkMeshApplication.getInstance().getMeshInfo().scenes;
        mAdapter = new SceneListAdapter(this, sceneList);

        mAdapter.setOnItemLongClickListener(new BaseRecyclerViewAdapter.OnItemLongClickListener() {
            @Override
            public boolean onLongClick(final int position) {

                tarScene = sceneList.get(position);
                boolean hasOffline = false;
                for (Scene.SceneState state : tarScene.states) {
                    // offline
                    NodeInfo localDevice = TelinkMeshApplication.getInstance().getMeshInfo().getDeviceByMeshAddress(state.address);
                    if (localDevice != null && localDevice.getOnOff() == -1) {
                        hasOffline = true;
                    }
                }
                showConfirmDialog("Confirm to delete scene " + (hasOffline ? "(contains offline device)" : "") + "?", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        deleteScene();
                    }
                });
                return false;
            }
        });
        RecyclerView rv_scheduler = findViewById(R.id.rv_common);
        rv_scheduler.setLayoutManager(new LinearLayoutManager(this));
        rv_scheduler.setAdapter(mAdapter);
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

    private void onAddClick() {
        startActivity(new Intent(SceneListActivity.this, SceneSettingActivity.class));
    }

    private void deleteScene() {
        if (tarScene.states.size() == 0) {
            sceneList.remove(tarScene);
            TelinkMeshApplication.getInstance().getMeshInfo().saveOrUpdate(getApplicationContext());
            mAdapter.notifyDataSetChanged();
        } else {
            showWaitingDialog("deleting...");
            deleteIndex = 0;
            deleteNextDevice();
        }
    }

    private void deleteNextDevice() {
        if (deleteIndex > tarScene.states.size() - 1) {
            sceneList.remove(tarScene);
            TelinkMeshApplication.getInstance().getMeshInfo().saveOrUpdate(getApplicationContext());
            tarScene = null;
            mAdapter.notifyDataSetChanged();
            dismissWaitingDialog();
            toastMsg("scene deleted");
        } else {
            NodeInfo deviceInfo = TelinkMeshApplication.getInstance().getMeshInfo()
                    .getDeviceByMeshAddress(tarScene.states.get(deleteIndex).address);

            // remove offline device
            if (deviceInfo.getOnOff() == -1) {
//                tarScene.deleteDevice(deviceInfo);
                deleteIndex++;
                deleteNextDevice();
            } else {
                handler.removeCallbacks(cmdTimeoutCheckTask);
                handler.postDelayed(cmdTimeoutCheckTask, 2000);

                final int eleAdr = deviceInfo.getTargetEleAdr(MeshSigModel.SIG_MD_SCENE_S.modelId);
                int appKeyIndex = TelinkMeshApplication.getInstance().getMeshInfo().getDefaultAppKeyIndex();
                SceneDeleteMessage deleteMessage = SceneDeleteMessage.getSimple(eleAdr,
                        appKeyIndex,
                        tarScene.id,
                        true, 1);
                MeshService.getInstance().sendMeshMessage(deleteMessage);
                // mesh interface
//                MeshService.getInstance().deleteScene(deviceInfo.meshAddress, true, 1, tarScene.id, null);
            }
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
        if (sceneList == null || sceneList.size() == 0) {
            ll_empty.setVisibility(View.VISIBLE);
        } else {
            ll_empty.setVisibility(View.GONE);
        }
        mAdapter.notifyDataSetChanged();
    }

    @Override
    public void performed(Event<String> event) {
        if (event.getType().equals(SceneRegisterStatusMessage.class.getName())) {
            StatusNotificationEvent statusEvent = (StatusNotificationEvent) event;
            NotificationMessage notificationMessage = statusEvent.getNotificationMessage();

            SceneRegisterStatusMessage sceneMessage = (SceneRegisterStatusMessage) notificationMessage.getStatusMessage();
            if (sceneMessage.getStatusCode() == ConfigStatus.SUCCESS.code
                    && notificationMessage.getSrc() == tarScene.states.get(deleteIndex).address) {
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
}
