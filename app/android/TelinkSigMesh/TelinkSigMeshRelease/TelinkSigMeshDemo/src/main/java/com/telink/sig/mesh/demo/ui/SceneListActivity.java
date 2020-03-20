/********************************************************************************************************
 * @file SceneListActivity.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2010
 *
 * @par Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
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
package com.telink.sig.mesh.demo.ui;

import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;
import android.view.View;

import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.demo.TelinkMeshApplication;
import com.telink.sig.mesh.demo.model.Scene;
import com.telink.sig.mesh.demo.ui.adapter.BaseRecyclerViewAdapter;
import com.telink.sig.mesh.demo.ui.adapter.SceneListAdapter;
import com.telink.sig.mesh.demo.ui.adapter.SchedulerListAdapter;
import com.telink.sig.mesh.event.Event;
import com.telink.sig.mesh.event.EventListener;
import com.telink.sig.mesh.event.NotificationEvent;
import com.telink.sig.mesh.light.MeshService;
import com.telink.sig.mesh.light.SceneRegisterNotificationParser;
import com.telink.sig.mesh.model.DeviceInfo;
import com.telink.sig.mesh.model.NotificationInfo;
import com.telink.sig.mesh.util.Arrays;
import com.telink.sig.mesh.util.TelinkLog;

import java.util.List;

/**
 * 场景列表
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

        sceneList = TelinkMeshApplication.getInstance().getMesh().scenes;
        mAdapter = new SceneListAdapter(this, sceneList);

        mAdapter.setOnItemLongClickListener(new BaseRecyclerViewAdapter.OnItemLongClickListener() {
            @Override
            public boolean onLongClick(final int position) {

                tarScene = sceneList.get(position);
                boolean hasOffline = false;
                for (Scene.SceneState state : tarScene.states) {
                    // 离线
                    DeviceInfo localDevice = TelinkMeshApplication.getInstance().getMesh().getDeviceByMeshAddress(state.address);
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

        TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_SCENE_REGISTER_STATUS, this);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        TelinkMeshApplication.getInstance().removeEventListener(this);
    }

    private void onAddClick() {
        startActivity(new Intent(SceneListActivity.this, SceneSettingActivity.class));
    }

    private void deleteScene() {
        if (tarScene.states.size() == 0) {
            sceneList.remove(tarScene);
            TelinkMeshApplication.getInstance().getMesh().saveOrUpdate(getApplicationContext());
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
            TelinkMeshApplication.getInstance().getMesh().saveOrUpdate(getApplicationContext());
            tarScene = null;
            mAdapter.notifyDataSetChanged();
            dismissWaitingDialog();
            toastMsg("scene deleted");
        } else {
            DeviceInfo deviceInfo = TelinkMeshApplication.getInstance().getMesh()
                    .getDeviceByMeshAddress(tarScene.states.get(deleteIndex).address);

            // 离线设备直接删除
            if (deviceInfo.getOnOff() == -1) {
//                tarScene.deleteDevice(deviceInfo);
                deleteIndex++;
                deleteNextDevice();
            } else {
                handler.removeCallbacks(cmdTimeoutCheckTask);
                handler.postDelayed(cmdTimeoutCheckTask, 2000);
//                MeshService.getInstance().cmdSceneDelete(deviceInfo.meshAddress, 1, tarScene.id, 1);
                MeshService.getInstance().deleteScene(deviceInfo.meshAddress, true, 1, tarScene.id, null);
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
        super.performed(event);
        switch (event.getType()) {
            case NotificationEvent.EVENT_TYPE_SCENE_REGISTER_STATUS:
                if (tarScene == null) return;
                NotificationInfo notificationInfo = ((NotificationEvent) event).getNotificationInfo();
                byte[] params = notificationInfo.params;

                SceneRegisterNotificationParser.SceneRegisterInfo registerInfo = SceneRegisterNotificationParser.create().parse(params);

                int status = registerInfo == null ? -1 : registerInfo.status;
                if (status == 0 && notificationInfo.srcAdr == tarScene.states.get(deleteIndex).address) {
                    handler.removeCallbacks(cmdTimeoutCheckTask);
                    deleteIndex++;
                    deleteNextDevice();
                } else {
                    dismissWaitingDialog();
                    TelinkLog.e("scene setting err!");
                }

                break;
        }
    }

    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.btn_add) {
            onAddClick();
        }
    }
}
