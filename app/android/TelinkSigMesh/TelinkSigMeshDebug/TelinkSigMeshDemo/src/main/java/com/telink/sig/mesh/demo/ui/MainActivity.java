/********************************************************************************************************
 * @file MainActivity.java
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

import android.app.Fragment;
import android.app.FragmentManager;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.support.annotation.NonNull;
import android.support.design.widget.BottomNavigationView;
import android.support.v7.app.AlertDialog;
import android.view.MenuItem;

import com.telink.sig.mesh.demo.ActivityManager;
import com.telink.sig.mesh.demo.AppSettings;
import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.demo.TelinkMeshApplication;
import com.telink.sig.mesh.demo.model.Mesh;
import com.telink.sig.mesh.demo.ui.fragment.DeviceFragment;
import com.telink.sig.mesh.demo.ui.fragment.GroupFragment;
import com.telink.sig.mesh.demo.ui.fragment.SettingFragment;
import com.telink.sig.mesh.event.CommandEvent;
import com.telink.sig.mesh.event.Event;
import com.telink.sig.mesh.event.MeshEvent;
import com.telink.sig.mesh.event.NotificationEvent;
import com.telink.sig.mesh.light.LeBluetooth;
import com.telink.sig.mesh.light.MeshController;
import com.telink.sig.mesh.light.MeshService;
import com.telink.sig.mesh.light.parameter.AutoConnectParameters;
import com.telink.sig.mesh.model.DeviceInfo;
import com.telink.sig.mesh.util.Arrays;
import com.telink.sig.mesh.util.MeshUtils;
import com.telink.sig.mesh.util.TelinkLog;
import com.telink.sig.mesh.util.UnitConvert;

import java.util.HashSet;
import java.util.List;
import java.util.Set;

/**
 * MainActivity include DeviceFragment & GroupFragment
 * Created by kee on 2017/8/18.
 */

public class MainActivity extends BaseActivity implements BottomNavigationView.OnNavigationItemSelectedListener {

    private FragmentManager fm;
    DeviceFragment deviceFragment;
    Fragment groupFragment;
    SettingFragment settingFragment;
    private boolean isServiceCreated = false;
    private Handler handler = new Handler();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        BottomNavigationView bottomNavigationView = findViewById(R.id.bottom_nav);
        bottomNavigationView.setOnNavigationItemSelectedListener(this);
        fm = getFragmentManager();
        deviceFragment = new DeviceFragment();
        groupFragment = new GroupFragment();
        settingFragment = new SettingFragment();
        fm.beginTransaction()
                .add(R.id.fl_container, deviceFragment).add(R.id.fl_container, groupFragment).add(R.id.fl_container, settingFragment)
                .show(deviceFragment).hide(groupFragment).hide(settingFragment)
                .commit();

        if (!LeBluetooth.getInstance().isSupport(getApplicationContext())) {
            toastMsg("ble not support");
            finish();
        }

        Mesh mesh = TelinkMeshApplication.getInstance().getMesh();
        if (mesh.devices != null) {
            for (DeviceInfo deviceInfo : mesh.devices) {
                deviceInfo.setOnOff(-1);
                deviceInfo.lum = 0;
                deviceInfo.temp = 0;
            }
        }


        Intent serviceIntent = new Intent(this, MeshService.class);
        startService(serviceIntent);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_DISCONNECTED, this);
        TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_DEVICE_ON_OFF_STATUS, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_MESH_EMPTY, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshController.EVENT_TYPE_SERVICE_CREATE, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshController.EVENT_TYPE_SERVICE_DESTROY, this);
        TelinkMeshApplication.getInstance().addEventListener(CommandEvent.EVENT_TYPE_CMD_PROCESSING, this);
        TelinkMeshApplication.getInstance().addEventListener(CommandEvent.EVENT_TYPE_CMD_COMPLETE, this);
        TelinkMeshApplication.getInstance().addEventListener(CommandEvent.EVENT_TYPE_CMD_ERROR_BUSY, this);
        TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_KICK_OUT_CONFIRM, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_AUTO_CONNECT_LOGIN, this);

        byte[] cps = TelinkMeshApplication.getInstance().getMeshLib().getLocalCpsData();
        TelinkLog.d("local cps: " + "len: " + cps.length + " -- " + Arrays.bytesToHexString(cps, ","));
    }

    @Override
    protected void onStart() {
        super.onStart();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        TelinkMeshApplication.getInstance().removeEventListener(this);
        handler.removeCallbacksAndMessages(null);
        if (!ActivityManager.getInstance().isApplicationForeground()){
            Intent serviceIntent = new Intent(this, MeshService.class);
            stopService(serviceIntent);
        }
    }


    @Override
    protected void onResume() {
        super.onResume();
        TelinkLog.d("main resume -- service created: " + isServiceCreated);
        if (!LeBluetooth.getInstance().isEnabled()) {
            showBleOpenDialog();
        } else {
            if (isServiceCreated) {
//                MeshService.getInstance().getOnOff(0xFFFF, 0, null);
                this.autoConnect(false);
            }
        }
    }

    AlertDialog.Builder mDialogBuilder;

    private void showBleOpenDialog() {
        if (mDialogBuilder == null) {
            mDialogBuilder = new AlertDialog.Builder(this);
            mDialogBuilder.setCancelable(false);
            mDialogBuilder.setMessage("Enable Bluetooth!");
            mDialogBuilder.setNegativeButton("cancel", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    finish();
                }
            });
            mDialogBuilder.setPositiveButton("enable", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    LeBluetooth.getInstance().enable(MainActivity.this);
                }
            });
        }

        mDialogBuilder.show();
    }


    private void autoConnect(boolean update) {
        TelinkLog.d("main auto connect");
        List<DeviceInfo> deviceInfoList = TelinkMeshApplication.getInstance().getMesh().devices;

        Set<String> targets = new HashSet<>();
        if (deviceInfoList != null) {
            for (DeviceInfo deviceInfo : deviceInfoList) {
                targets.add(deviceInfo.macAddress);
            }
        }

        AutoConnectParameters parameters = AutoConnectParameters.getDefault(targets);
        parameters.setScanMinPeriod(0);
        if (update) {
            MeshService.getInstance().updateAutoConnectParams(parameters);
        } else {
            MeshService.getInstance().autoConnect(parameters);
        }

    }

    /*private void autoConnect(boolean update) {
        TelinkLog.d("main auto connect");
        final Mesh mesh = TelinkMeshApplication.getInstance().getMesh();
        List<DeviceInfo> deviceInfoList = mesh.devices;

        int[] targets = null;
        if (deviceInfoList != null) {
            targets = new int[deviceInfoList.size()];
            for (int i = 0; i < deviceInfoList.size(); i++) {
                targets[i] = deviceInfoList.get(i).meshAddress;
            }
        }

        AutoConnectParameters parameters = AutoConnectParameters.getDefault(targets, mesh.networkKey);
        parameters.setScanMinPeriod(0);
        if (update) {
            MeshService.getInstance().updateAutoConnectParams(parameters);
        } else {
            MeshService.getInstance().autoConnect(parameters);
        }

    }*/

    @Override
    public void performed(Event<String> event) {
        super.performed(event);
        switch (event.getType()) {
            case MeshEvent.EVENT_TYPE_MESH_EMPTY:
                TelinkLog.d(TAG + "#EVENT_TYPE_MESH_EMPTY");
                break;
            case MeshEvent.EVENT_TYPE_DISCONNECTED:
                handler.removeCallbacksAndMessages(null);
                break;
            case MeshEvent.EVENT_TYPE_AUTO_CONNECT_LOGIN:
                // get all device on off status when auto connect success
//
                AppSettings.ONLINE_STATUS_ENABLE = MeshService.getInstance().getOnlineStatus();
                if (!AppSettings.ONLINE_STATUS_ENABLE) {
                    MeshService.getInstance().getOnOff(0xFFFF, 0, null);
                }
                sendTimeStatus();
                break;
            case MeshController.EVENT_TYPE_SERVICE_DESTROY:
                TelinkLog.d(TAG + "#EVENT_TYPE_SERVICE_DESTROY");
                break;
            case MeshController.EVENT_TYPE_SERVICE_CREATE:
                TelinkLog.d(TAG + "#EVENT_TYPE_SERVICE_CREATE");
                autoConnect(false);
                isServiceCreated = true;
                break;
            case CommandEvent.EVENT_TYPE_CMD_PROCESSING:
//                showWaitingDialog("CMD processing");
                break;
            case CommandEvent.EVENT_TYPE_CMD_COMPLETE:
                dismissWaitingDialog();
                break;

            case CommandEvent.EVENT_TYPE_CMD_ERROR_BUSY:
                TelinkLog.w("CMD busy");
                /*runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        toastMsg("CMD fail, Busy!");
                    }
                });*/
                break;

            case NotificationEvent.EVENT_TYPE_KICK_OUT_CONFIRM:
                autoConnect(true);
                break;
        }
    }


    public void sendTimeStatus() {
        handler.postDelayed(new Runnable() {
            @Override
            public void run() {
                long time = MeshUtils.getTaiTime();
                int offset = UnitConvert.getZoneOffset();
                final int eleAdr = 0xFFFF;
                MeshService.getInstance().sendTimeStatus(eleAdr, 1, time, offset, null);
            }
        }, 1500);

    }

    @Override
    public boolean onNavigationItemSelected(@NonNull MenuItem item) {
        switch (item.getItemId()) {
            case R.id.item_device:
                fm.beginTransaction().hide(groupFragment).hide(settingFragment).show(deviceFragment).commit();
                break;
            case R.id.item_group:
                fm.beginTransaction().hide(deviceFragment).hide(settingFragment).show(groupFragment).commit();
                break;
            case R.id.item_setting:
                fm.beginTransaction().hide(deviceFragment).hide(groupFragment).show(settingFragment).commit();
                break;

        }
        return true;
    }
}
