/********************************************************************************************************
 * @file SceneSettingActivity.java
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

import android.os.Bundle;
import android.os.Handler;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.Toolbar;
import android.view.MenuItem;
import android.view.View;
import android.widget.CheckBox;
import android.widget.Toast;

import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.demo.TelinkMeshApplication;
import com.telink.sig.mesh.demo.model.Mesh;
import com.telink.sig.mesh.demo.model.Scene;
import com.telink.sig.mesh.demo.ui.adapter.BaseSelectableListAdapter;
import com.telink.sig.mesh.demo.ui.adapter.DeviceSelectAdapter;
import com.telink.sig.mesh.event.Event;
import com.telink.sig.mesh.event.EventListener;
import com.telink.sig.mesh.event.NotificationEvent;
import com.telink.sig.mesh.light.MeshService;
import com.telink.sig.mesh.light.SceneRegisterNotificationParser;
import com.telink.sig.mesh.model.DeviceInfo;
import com.telink.sig.mesh.model.Group;
import com.telink.sig.mesh.model.NotificationInfo;
import com.telink.sig.mesh.model.SigMeshModel;
import com.telink.sig.mesh.util.Arrays;
import com.telink.sig.mesh.util.TelinkLog;

import java.util.ArrayList;
import java.util.List;

/**
 * 场景配置
 * Created by kee on 2018/9/18.
 */
public class SceneSettingActivity extends BaseActivity implements View.OnClickListener, BaseSelectableListAdapter.SelectStatusChangedListener, EventListener<String> {

    //    private GroupSelectAdapter mGroupAdapter;
    private DeviceSelectAdapter mDeviceAdapter;
    //    private int sceneId;
    private Scene scene;
    private Mesh mesh;
    private List<DeviceInfo> devices;
    //    private List<Group> groups;
    private CheckBox cb_device, cb_group;

    private Handler delayHandler = new Handler();
    /**
     * add or remove from list
     */
    private List<SettingModel> selectedAdrList;
    private int settingIndex;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_scene_setting);
        mesh = TelinkMeshApplication.getInstance().getMesh();

        int sceneId = getIntent().getIntExtra("sceneId", -1);
        if (sceneId == -1) {
            sceneId = mesh.allocSceneId();
            if (sceneId == -1) {
                finish();
                Toast.makeText(getApplicationContext(), "no available scene id", Toast.LENGTH_SHORT).show();
                return;
            }
            scene = new Scene();
            scene.id = sceneId;
        } else {
            scene = mesh.getSceneById(sceneId);
        }


        Toolbar toolbar = findViewById(R.id.title_bar);
        toolbar.inflateMenu(R.menu.check);
        toolbar.setOnMenuItemClickListener(new Toolbar.OnMenuItemClickListener() {
            @Override
            public boolean onMenuItemClick(MenuItem item) {
                if (item.getItemId() == R.id.item_check) {
                    saveStart();
                }
                return false;
            }
        });
        setTitle("Scene Setting");
        enableBackNav(true);
        cb_device = findViewById(R.id.cb_device);
        cb_group = findViewById(R.id.cb_group);
        RecyclerView rv_device = findViewById(R.id.rv_device);

        rv_device.setLayoutManager(new LinearLayoutManager(this));
        if (mesh.devices != null) {
            for (DeviceInfo deviceInfo : mesh.devices) {
                deviceInfo.selected = scene.contains(deviceInfo);
            }
        }
        devices = mesh.devices;
        mDeviceAdapter = new DeviceSelectAdapter(this, devices);
        mDeviceAdapter.setStatusChangedListener(this);
        rv_device.setAdapter(mDeviceAdapter);
        cb_device.setChecked(mDeviceAdapter.allSelected());
        findViewById(R.id.btn_save_scene).setOnClickListener(this);
        findViewById(R.id.cb_device).setOnClickListener(this);
        findViewById(R.id.cb_group).setOnClickListener(this);

        TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_SCENE_REGISTER_STATUS, this);
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
            case R.id.btn_save_scene:
                saveStart();
                break;

            case R.id.cb_device:
                mDeviceAdapter.setAll(!mDeviceAdapter.allSelected());
                break;

            case R.id.cb_group:
//                mGroupAdapter.setAll(!mGroupAdapter.allSelected());
                break;
        }
    }

    private void setNextAddress() {

        if (settingIndex > selectedAdrList.size() - 1) {
            dismissWaitingDialog();
            toastMsg("store scene complete");
//            finish();
            return;
        }
        SettingModel model = selectedAdrList.get(settingIndex);

        TelinkLog.d("set next address: " + model.address);

        // F0:0D:02:00:01:00:82:45:00:01:00:01:00:37:00
        delayHandler.removeCallbacks(cmdTimeoutCheckTask);
        delayHandler.postDelayed(cmdTimeoutCheckTask, 2000);
        if (model.add) {
//            MeshService.getInstance().cmdSceneStore(model.address, 1, scene.id, 1);
            MeshService.getInstance().storeScene(model.address, true, 1, scene.id, null);
        } else {
//            MeshService.getInstance().cmdSceneDelete(model.address, 1, scene.id, 1);
            MeshService.getInstance().deleteScene(model.address, true, 1, scene.id, null);
        }
    }

    private Runnable cmdTimeoutCheckTask = new Runnable() {
        @Override
        public void run() {
            settingIndex++;
            setNextAddress();
        }
    };

    private void saveStart() {
        selectedAdrList = new ArrayList<>();
        if (devices != null) {
            for (DeviceInfo deviceInfo : devices) {
                if (deviceInfo.getOnOff() == -1 || deviceInfo.getTargetEleAdr(SigMeshModel.SIG_MD_SCENE_S.modelId) == -1) {
                    TelinkLog.d("scene save: device check fail");
                    continue;
                }
                if (!deviceInfo.selected) {
                    if (scene.contains(deviceInfo))
                        selectedAdrList.add(new SettingModel(deviceInfo.meshAddress, false));
                } else {
                    selectedAdrList.add(new SettingModel(deviceInfo.meshAddress, true));
                }
            }
        }
        if (selectedAdrList.size() == 0) {
            toastMsg("select at least one item !");
        } else {
            showWaitingDialog("setting...");
            settingIndex = 0;
            setNextAddress();
        }
    }

    @Override
    public void onStatusChanged(BaseSelectableListAdapter adapter) {
        if (adapter == mDeviceAdapter) {
            cb_device.setChecked(mDeviceAdapter.allSelected());
        }
    }

    @Override
    public void performed(Event<String> event) {
        super.performed(event);
        switch (event.getType()) {
            case NotificationEvent.EVENT_TYPE_SCENE_REGISTER_STATUS:

                // F0:0D:02:00:01:00:82:45: 00:37:00:01:00:37:00
                NotificationInfo notificationInfo = ((NotificationEvent) event).getNotificationInfo();
                byte[] params = notificationInfo.params;

                TelinkLog.d("event params: " + Arrays.bytesToHexString(params, ":"));

                SceneRegisterNotificationParser.SceneRegisterInfo registerInfo = SceneRegisterNotificationParser.create().parse(params);
                int status = registerInfo == null ? -1 : registerInfo.status;

                if (status == 0 && notificationInfo.srcAdr == selectedAdrList.get(settingIndex).address) {

                    delayHandler.removeCallbacks(cmdTimeoutCheckTask);
                    DeviceInfo deviceInfo = mesh.getDeviceByMeshAddress(notificationInfo.srcAdr);
                    if (selectedAdrList.get(settingIndex).add) {
                        scene.saveFromDeviceInfo(deviceInfo);
                    } else {
                        scene.removeByAddress(deviceInfo.meshAddress);
                    }
                    mesh.saveScene(scene);
                    mesh.saveOrUpdate(this);
                    settingIndex++;
                    setNextAddress();
                }
                break;
        }
    }

    class SettingModel {
        int address;
        boolean add;

        SettingModel(int address, boolean add) {
            this.address = address;
            this.add = add;
        }
    }
}
