/********************************************************************************************************
 * @file DeviceConfigActivity.java
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
package com.telink.ble.mesh.ui;

import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.util.AttributeSet;
import android.view.View;
import android.widget.EditText;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AlertDialog;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentPagerAdapter;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;
import androidx.viewpager.widget.ViewPager;

import com.google.android.material.tabs.TabLayout;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.core.message.config.DefaultTTLGetMessage;
import com.telink.ble.mesh.core.message.config.DefaultTTLSetMessage;
import com.telink.ble.mesh.core.message.config.RelaySetMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.ReliableMessageProcessEvent;
import com.telink.ble.mesh.model.ConfigState;
import com.telink.ble.mesh.model.DeviceConfig;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.NodeStatusChangedEvent;
import com.telink.ble.mesh.ui.adapter.DeviceConfigListAdapter;
import com.telink.ble.mesh.ui.fragment.DeviceControlFragment;
import com.telink.ble.mesh.ui.fragment.DeviceGroupFragment;
import com.telink.ble.mesh.ui.fragment.DeviceSettingFragment;
import com.telink.ble.mesh.ui.widget.EditableRecyclerView;
import com.telink.ble.mesh.util.MeshLogger;

import java.util.ArrayList;
import java.util.List;

/**
 * container for device control , group,  device settings
 * Created by kee on 2017/8/30.
 */
public class DeviceConfigActivity extends BaseActivity implements EventListener<String> {

    NodeInfo deviceInfo;
    private Handler delayHandler = new Handler();
    private DeviceConfigListAdapter adapter;
    private List<DeviceConfig> deviceConfigList;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_device_config);
        setTitle("Device Config");
        enableBackNav(true);
        final Intent intent = getIntent();
        if (intent.hasExtra("meshAddress")) {
            int address = intent.getIntExtra("meshAddress", -1);
            deviceInfo = TelinkMeshApplication.getInstance().getMeshInfo().getDeviceByMeshAddress(address);
        } else {
            toastMsg("device address null");
            finish();
            return;
        }

        if (deviceInfo == null) {
            toastMsg("device info null");
            finish();
            return;
        }

        RecyclerView rv_dev_cfg = findViewById(R.id.rv_dev_cfg);
        rv_dev_cfg.setLayoutManager(new LinearLayoutManager(this));
        initConfigs();
        adapter = new DeviceConfigListAdapter(this, deviceConfigList);
        rv_dev_cfg.setAdapter(adapter);


        TelinkMeshApplication.getInstance().addEventListener(NodeStatusChangedEvent.EVENT_TYPE_NODE_STATUS_CHANGED, this);
        refreshUI();
    }

    private void initConfigs() {
        deviceConfigList = new ArrayList<>();
        DeviceConfig deviceConfig;
        for (ConfigState cfg : ConfigState.values()) {
            deviceConfig = new DeviceConfig(cfg);
            deviceConfigList.add(deviceConfig);
        }
    }

    public void onGetTap(int position) {
        MeshLogger.d("get tap : " + position);
        DeviceConfig config = deviceConfigList.get(position);
        String name = config.configState.name;
        MeshMessage meshMessage = null;
        switch (config.configState) {
            case DEFAULT_TTL:
                meshMessage = new DefaultTTLGetMessage(deviceInfo.meshAddress);
            case RELAY:
        }
        if (meshMessage != null) {
            boolean cmdSent = MeshService.getInstance().sendMeshMessage(meshMessage);
            if (cmdSent) {
                showWaitingDialog("getting " + name + "...");
            } else {
                toastMsg("get message send error ");
            }
        } else {
            MeshLogger.d("message null");
        }
    }

    public void onSetTap(int position) {
        MeshLogger.d("set tap : " + position);
        DeviceConfig config = deviceConfigList.get(position);
        startActivity(new Intent(this, DeviceConfigSettingActivity.class).putExtra("meshAddress", deviceInfo.meshAddress)
                .putExtra("deviceConfig", config));
//        showSettingDialog(config);
    }

    private void showSettingDialog(final DeviceConfig config) {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        EditableRecyclerView inputRecyclerView = new EditableRecyclerView(this);


        builder.setTitle("Set " + config.configState.name).setMessage("input new value(hex)")
                .setView(inputRecyclerView)
                .setPositiveButton("confirm", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {

                        String name = config.configState.name;

                        MeshMessage meshMessage = null;
                        switch (config.configState) {
                            case DEFAULT_TTL:
//                                meshMessage = DefaultTTLSetMessage.getSimple(deviceInfo.meshAddress, Byte.parseByte(valueInput.getText().toString().trim()));
                                break;
                            case RELAY:
//                                meshMessage = RelaySetMessage.getSimple(deviceInfo.meshAddress, )
                                break;
                        }
                        dialog.dismiss();
                        if (meshMessage != null) {
                            boolean cmdSent = MeshService.getInstance().sendMeshMessage(meshMessage);
                            if (cmdSent) {
                                showWaitingDialog("setting " + name + "...");
                            } else {
                                toastMsg("set message send error ");
                            }
                        } else {
                            MeshLogger.d("message null");
                        }

                    }
                })
                .setNegativeButton("cancel", null);
        builder.show();
    }

    private void showSendWaitingDialog(String message) {
        showWaitingDialog(message);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (delayHandler != null) {
            delayHandler.removeCallbacksAndMessages(null);
        }
        TelinkMeshApplication.getInstance().removeEventListener(this);
    }


    @Override
    public void performed(Event event) {
        if (event.getType().equals(NodeStatusChangedEvent.EVENT_TYPE_NODE_STATUS_CHANGED)) {
            refreshUI();
        } else if (event.getType().equals(ReliableMessageProcessEvent.EVENT_TYPE_MSG_PROCESS_COMPLETE)) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    dismissWaitingDialog();
                }
            });
        }
    }

    private void refreshUI() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                adapter.notifyDataSetChanged();
            }
        });
    }


    static class ConfigSetView extends RecyclerView {

        public ConfigSetView(@NonNull Context context) {
            super(context);
        }

        public ConfigSetView(@NonNull Context context, @Nullable AttributeSet attrs) {
            super(context, attrs);
        }

        public ConfigSetView(@NonNull Context context, @Nullable AttributeSet attrs, int defStyle) {
            super(context, attrs, defStyle);
        }

        public void init(Context context) {
            this.setLayoutManager(new LinearLayoutManager(context));

        }

    }

}
