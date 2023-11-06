/********************************************************************************************************
 * @file MainActivity.java
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
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.view.MenuItem;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentManager;

import com.google.android.material.bottomnavigation.BottomNavigationView;
import com.telink.ble.mesh.SharedPreferenceHelper;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.Encipher;
import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.NotificationMessage;
import com.telink.ble.mesh.core.message.firmwaredistribution.DistributionPhase;
import com.telink.ble.mesh.core.message.firmwaredistribution.FDCancelMessage;
import com.telink.ble.mesh.core.message.firmwaredistribution.FDStatusMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.DistributionStatus;
import com.telink.ble.mesh.core.message.generic.OnOffGetMessage;
import com.telink.ble.mesh.core.message.time.TimeSetMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.AutoConnectEvent;
import com.telink.ble.mesh.foundation.event.MeshEvent;
import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;
import com.telink.ble.mesh.foundation.parameter.AutoConnectParameters;
import com.telink.ble.mesh.model.AppSettings;
import com.telink.ble.mesh.model.CertCacheService;
import com.telink.ble.mesh.model.FUCache;
import com.telink.ble.mesh.model.FUCacheService;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.OnlineState;
import com.telink.ble.mesh.model.UnitConvert;
import com.telink.ble.mesh.model.db.MeshInfoService;
import com.telink.ble.mesh.ui.fragment.DeviceFragment;
import com.telink.ble.mesh.ui.fragment.GroupFragment;
import com.telink.ble.mesh.ui.fragment.NetworkFragment;
import com.telink.ble.mesh.ui.fragment.SettingFragment;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;

/**
 * MainActivity include DeviceFragment & GroupFragment
 * Created by kee on 2017/8/18.
 */
public class MainActivity extends BaseActivity implements BottomNavigationView.OnNavigationItemSelectedListener, EventListener<String> {

    private FragmentManager fm;
    private DeviceFragment deviceFragment;
    private Fragment groupFragment;
    private NetworkFragment networkFragment;
    private SettingFragment settingFragment;
    private Handler mHandler = new Handler();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        initBottomNav();
        addEventListeners();
        startMeshService();
        resetNodeState();

        FUCacheService.getInstance().load(this); // load FirmwareUpdate cache
        CertCacheService.getInstance().load(this); // load cert cache
        testMd5();
    }

    private void testMd5(){
        byte[] re = Encipher.calcUuidByMac(Arrays.hexToBytes("FFFFBBCCDD82")); // FFFFBBCCDD82 - 192E11381215CFE0BF44D816BE0E421C
//        byte[] re = Encipher.md5(Arrays.hexToBytes("A4C1385DAE0D")); // A422D068CF4A9533ABF263EB74A7EADA
//        Encipher.md5();
    }

    private void addEventListeners() {
        TelinkMeshApplication.getInstance().addEventListener(AutoConnectEvent.EVENT_TYPE_AUTO_CONNECT_LOGIN, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_DISCONNECTED, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_MESH_EMPTY, this);
        TelinkMeshApplication.getInstance().addEventListener(FDStatusMessage.class.getName(), this);
    }

    private void initBottomNav() {
        BottomNavigationView bottomNavigationView = findViewById(R.id.bottom_nav);
        bottomNavigationView.setOnNavigationItemSelectedListener(this);
        fm = getSupportFragmentManager();
        deviceFragment = new DeviceFragment();
        groupFragment = new GroupFragment();
        networkFragment = new NetworkFragment();
        settingFragment = new SettingFragment();
        fm.beginTransaction()
                .add(R.id.fl_container, deviceFragment)
                .add(R.id.fl_container, groupFragment)
                .add(R.id.fl_container, networkFragment)
                .add(R.id.fl_container, settingFragment)
                .show(deviceFragment).hide(groupFragment).hide(networkFragment).hide(settingFragment)
                .commit();
        curFragment = deviceFragment;
    }


    /**
     * init and setup mesh network
     */
    private void startMeshService() {
        // init
        MeshService.getInstance().init(this, TelinkMeshApplication.getInstance());

        // convert meshInfo to mesh configuration
        MeshInfo meshInfo = MeshInfoService.getInstance().getById(SharedPreferenceHelper.getSelectedMeshId(this));
        TelinkMeshApplication.getInstance().setupMesh(meshInfo);

//        MeshConfiguration meshConfiguration = TelinkMeshApplication.getInstance().getMeshInfo().convertToConfiguration();
//        MeshService.getInstance().setupMeshNetwork(meshConfiguration);

        // check if system bluetooth enabled
        MeshService.getInstance().checkBluetoothState();

        /// set DLE enable
        MeshService.getInstance().resetExtendBearerMode(SharedPreferenceHelper.getExtendBearerMode(this));
    }

    // set all devices to offline
    private void resetNodeState() {
        MeshInfo mesh = TelinkMeshApplication.getInstance().getMeshInfo();
        if (mesh.nodes != null) {
            for (NodeInfo deviceInfo : mesh.nodes) {
                deviceInfo.setOnlineState(OnlineState.OFFLINE);
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
        MeshInfo meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
        if (meshInfo.nodes.size() == 0) {
            MeshService.getInstance().idle(true);
        } else {
            int directAdr = MeshService.getInstance().getDirectConnectedNodeAddress();
            NodeInfo nodeInfo = meshInfo.getDeviceByMeshAddress(directAdr);
            if (nodeInfo != null && nodeInfo.compositionData != null && AppSettings.isRemote(nodeInfo.compositionData.pid)) {
                // if direct connected device is remote-control, disconnect
                MeshService.getInstance().idle(true);
            }
            MeshService.getInstance().autoConnect(new AutoConnectParameters());
        }

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
            mHandler.postDelayed(() -> checkMeshOtaState(), 3 * 1000);
        } else if (event.getType().equals(MeshEvent.EVENT_TYPE_DISCONNECTED)) {
            mHandler.removeCallbacksAndMessages(null);
        } else if (event.getType().equals(FDStatusMessage.class.getName())) {
            NotificationMessage notificationMessage = ((StatusNotificationEvent) event).getNotificationMessage();
            int msgSrc = notificationMessage.getSrc();
            FUCache fuCache = FUCacheService.getInstance().get();
            if (fuCache != null && fuCache.distAddress == msgSrc) {
                FDStatusMessage fdStatusMessage = (FDStatusMessage) notificationMessage.getStatusMessage();
                MeshLogger.d("FDStatus in main : " + fdStatusMessage.toString());
                if (fdStatusMessage.status == DistributionStatus.SUCCESS.code) {
                    if (fdStatusMessage.distPhase == DistributionPhase.IDLE.value) {
                        MeshLogger.d("clear meshOTA state");
                        FUCacheService.getInstance().clear(this);
                    } else if (fdStatusMessage.distPhase == DistributionPhase.CANCELING_UPDATE.value) {
                        // if canceling, resend cancel
                        FDCancelMessage cancelMessage = FDCancelMessage.getSimple(fuCache.distAddress, 0);
                        MeshService.getInstance().sendMeshMessage(cancelMessage);
                    }

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
            MeshLogger.d("FU cache: distAdr-" + fuCache.distAddress);
            showMeshOTATipsDialog(fuCache.distAddress);
        } else {
            MeshLogger.d("FU cache: not found");
        }
    }

    public void showMeshOTATipsDialog(final int distributorAddress) {

        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        DialogInterface.OnClickListener dialogBtnClick = (dialog, which) -> {
            if (which == DialogInterface.BUTTON_POSITIVE) {
                // GO
                startActivity(new Intent(MainActivity.this, FUActivity.class)
                        .putExtra(FUActivity.KEY_FU_CONTINUE, true));
            } else if (which == DialogInterface.BUTTON_NEGATIVE) {
                // STOP
                FDCancelMessage cancelMessage = FDCancelMessage.getSimple(distributorAddress, 0);
                MeshService.getInstance().sendMeshMessage(cancelMessage);
            } else if (which == DialogInterface.BUTTON_NEUTRAL) {
                // CLEAR
                FDCancelMessage cancelMessage = FDCancelMessage.getSimple(distributorAddress, 0);
                MeshService.getInstance().sendMeshMessage(cancelMessage);
                FUCacheService.getInstance().clear(MainActivity.this);
            }
        };

        builder.setTitle("Warning - MeshOTA is still running")
                .setMessage("MeshOTA distribution is still running, continue?\n" +
                        "click GO to enter MeshOTA processing page \n"
                        + "click STOP to stop distribution \n" +
                        "click CLEAR to clear cache(Used when an Mesh OTA upgrade exception occurs)")
                .setPositiveButton("GO", dialogBtnClick)
                .setNegativeButton("STOP", dialogBtnClick)
                .setNeutralButton("CLEAR", dialogBtnClick);
        builder.show();
    }

    public void sendTimeStatus() {
        mHandler.postDelayed(() -> {
            long time = MeshUtils.getTaiTime();
            int offset = UnitConvert.getZoneOffset();
            final int address = 0xFFFF;
            MeshInfo meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
            TimeSetMessage timeSetMessage = TimeSetMessage.getSimple(address, meshInfo.getDefaultAppKeyIndex(), time, offset, 1);
            timeSetMessage.setAck(false);
            MeshService.getInstance().sendMeshMessage(timeSetMessage);
        }, 1500);
    }


    @Override
    public boolean onNavigationItemSelected(@NonNull MenuItem item) {
        switch (item.getItemId()) {
            case R.id.item_device:
                showFragment(deviceFragment);
                break;
            case R.id.item_group:
                showFragment(groupFragment);
                break;
            case R.id.item_network:
                showFragment(networkFragment);
                break;
            case R.id.item_setting:
                showFragment(settingFragment);
                break;
        }
        return true;
    }


    Fragment curFragment;

    private void showFragment(Fragment fragment) {
        if (curFragment == fragment) {
            return;
        }
        fm.beginTransaction().hide(curFragment).show(fragment).commit();
        curFragment = fragment;
    }


}
