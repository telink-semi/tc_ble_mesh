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
package com.telink.ble.mesh.ui;

import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.view.MenuItem;

import com.google.android.material.bottomnavigation.BottomNavigationView;
import com.telink.ble.mesh.SharedPreferenceHelper;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.core.message.NotificationMessage;
import com.telink.ble.mesh.core.message.firmwaredistribution.FDCancelMessage;
import com.telink.ble.mesh.core.message.firmwaredistribution.FDStartMessage;
import com.telink.ble.mesh.core.message.firmwaredistribution.FDStatusMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.DistributionStatus;
import com.telink.ble.mesh.core.message.generic.OnOffGetMessage;
import com.telink.ble.mesh.core.message.time.TimeSetMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshConfiguration;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.AutoConnectEvent;
import com.telink.ble.mesh.foundation.event.MeshEvent;
import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;
import com.telink.ble.mesh.foundation.parameter.AutoConnectParameters;
import com.telink.ble.mesh.model.AppSettings;
import com.telink.ble.mesh.model.FUCache;
import com.telink.ble.mesh.model.FUCacheService;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.UnitConvert;
import com.telink.ble.mesh.ui.fragment.DeviceFragment;
import com.telink.ble.mesh.ui.fragment.GroupFragment;
import com.telink.ble.mesh.ui.fragment.SettingFragment;
import com.telink.ble.mesh.util.MeshLogger;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentManager;

/**
 * MainActivity include DeviceFragment & GroupFragment
 * Created by kee on 2017/8/18.
 */
public class MainActivity extends BaseActivity implements BottomNavigationView.OnNavigationItemSelectedListener, EventListener<String> {

    private FragmentManager fm;
    private DeviceFragment deviceFragment;
    private Fragment groupFragment;
    private SettingFragment settingFragment;
    private Handler mHandler = new Handler();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        initBottomNav();
        TelinkMeshApplication.getInstance().addEventListener(AutoConnectEvent.EVENT_TYPE_AUTO_CONNECT_LOGIN, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_DISCONNECTED, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_MESH_EMPTY, this);
        TelinkMeshApplication.getInstance().addEventListener(FDStatusMessage.class.getName(), this);
        startMeshService();
        resetNodeState();

        FUCacheService.getInstance().load(this); // load FirmwareUpdate cache
    }

    private void initBottomNav() {
        BottomNavigationView bottomNavigationView = findViewById(R.id.bottom_nav);
        bottomNavigationView.setOnNavigationItemSelectedListener(this);
        fm = getSupportFragmentManager();
        deviceFragment = new DeviceFragment();
        groupFragment = new GroupFragment();
        settingFragment = new SettingFragment();
        fm.beginTransaction()
                .add(R.id.fl_container, deviceFragment).add(R.id.fl_container, groupFragment).add(R.id.fl_container, settingFragment)
                .show(deviceFragment).hide(groupFragment).hide(settingFragment)
                .commit();
    }

    /**
     * init and setup mesh network
     */
    private void startMeshService() {
        // init
        MeshService.getInstance().init(this, TelinkMeshApplication.getInstance());

        // convert meshInfo to mesh configuration
        MeshConfiguration meshConfiguration = TelinkMeshApplication.getInstance().getMeshInfo().convertToConfiguration();
        MeshService.getInstance().setupMeshNetwork(meshConfiguration);

        // check if system bluetooth enabled
        MeshService.getInstance().checkBluetoothState();

        /// set DLE enable
        MeshService.getInstance().resetDELState(SharedPreferenceHelper.isDleEnable(this));
    }

    private void resetNodeState() {
        MeshInfo mesh = TelinkMeshApplication.getInstance().getMeshInfo();
        if (mesh.nodes != null) {
            for (NodeInfo deviceInfo : mesh.nodes) {
                deviceInfo.setOnOff(-1);
                deviceInfo.lum = 0;
                deviceInfo.temp = 0;
            }
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        TelinkMeshApplication.getInstance().removeEventListener(this);
        MeshService.getInstance().clear();
    }


    @Override
    protected void onResume() {
        super.onResume();
        this.autoConnect();
//        showMeshOTATipsDialog(2);
    }


    private void autoConnect() {
        MeshLogger.log("main auto connect");
//        MeshService.getInstance().autoConnect(new AutoConnectParameters(AutoConnectFilterType.NODE_IDENTITY));
        MeshService.getInstance().autoConnect(new AutoConnectParameters());
    }


    @Override
    public void performed(Event<String> event) {
        super.performed(event);
        if (event.getType().equals(MeshEvent.EVENT_TYPE_MESH_EMPTY)) {
            MeshLogger.log(TAG + "#EVENT_TYPE_MESH_EMPTY");
        } else if (event.getType().equals(AutoConnectEvent.EVENT_TYPE_AUTO_CONNECT_LOGIN)) {
            // get all device on off status when auto connect success
            AppSettings.ONLINE_STATUS_ENABLE = MeshService.getInstance().getOnlineStatus();
            if (!AppSettings.ONLINE_STATUS_ENABLE) {
                MeshService.getInstance().getOnlineStatus();
                int rspMax = TelinkMeshApplication.getInstance().getMeshInfo().getOnlineCountInAll();
                int appKeyIndex = TelinkMeshApplication.getInstance().getMeshInfo().getDefaultAppKeyIndex();
                OnOffGetMessage message = OnOffGetMessage.getSimple(0xFFFF, appKeyIndex, rspMax);
                MeshService.getInstance().sendMeshMessage(message);
            } else {
                MeshLogger.log("online status enabled");
            }
            sendTimeStatus();
            mHandler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    checkMeshOtaState();
                }
            }, 3 * 1000);
        } else if (event.getType().equals(MeshEvent.EVENT_TYPE_DISCONNECTED)) {
            mHandler.removeCallbacksAndMessages(null);
        } else if (event.getType().equals(FDStatusMessage.class.getName())) {
            NotificationMessage notificationMessage = ((StatusNotificationEvent) event).getNotificationMessage();
            int msgSrc = notificationMessage.getSrc();
            FUCache fuCache = FUCacheService.getInstance().get();
            if (fuCache != null && fuCache.distAddress == msgSrc) {
                FDStatusMessage fdStatusMessage = (FDStatusMessage) notificationMessage.getStatusMessage();
                if (fdStatusMessage.status == DistributionStatus.SUCCESS.code && fdStatusMessage.distPhase == 0) {
                    MeshLogger.d("clear meshOTA state");
                    FUCacheService.getInstance().clear(this);
                }
            }
        }
    }

    /**
     * check if last mesh OTA flow completed,
     * if mesh OTA is still running , show MeshOTA tips dialog
     */
    public void checkMeshOtaState() {
        FUCache fuCache = FUCacheService.getInstance().get();
        if (fuCache != null) {
            MeshLogger.d("check FU state: distAdr-" + fuCache.distAddress);
            showMeshOTATipsDialog(fuCache.distAddress);
        } else {
            MeshLogger.d("FU state: idle");
        }
    }

    public void showMeshOTATipsDialog(final int distributorAddress) {

        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        DialogInterface.OnClickListener dialogBtnClick = new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                if (which == DialogInterface.BUTTON_POSITIVE) {
                    // GO
                    startActivity(new Intent(MainActivity.this, FUActivity.class)
                            .putExtra(FUActivity.KEY_FU_CONTINUE, true));
                } else if (which == DialogInterface.BUTTON_NEGATIVE) {
                    // STOP
                    FDCancelMessage cancelMessage = FDCancelMessage.getSimple(distributorAddress, 0);
                    MeshService.getInstance().sendMeshMessage(cancelMessage);
                } else if (which == DialogInterface.BUTTON_NEUTRAL) {
                    dialog.dismiss();
                }
            }
        };

        builder.setTitle("Warning - MeshOTA is still running")
                .setMessage("MeshOTA distribution is still running, continue?\n" +
                        "click GO to enter MeshOTA processing page \n"
                        + "click STOP to stop distribution \n" +
                        "click IGNORE to dismiss dialog")
                .setPositiveButton("GO", dialogBtnClick)
                .setNegativeButton("STOP", dialogBtnClick)
                .setNeutralButton("IGNORE", null);
        builder.show();
    }

    public void sendTimeStatus() {
        mHandler.postDelayed(new Runnable() {
            @Override
            public void run() {
                long time = MeshUtils.getTaiTime();
                int offset = UnitConvert.getZoneOffset();
                final int address = 0xFFFF;
                MeshInfo meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
                TimeSetMessage timeSetMessage = TimeSetMessage.getSimple(address, meshInfo.getDefaultAppKeyIndex(), time, offset, 1);
                timeSetMessage.setAck(false);
                MeshService.getInstance().sendMeshMessage(timeSetMessage);
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
