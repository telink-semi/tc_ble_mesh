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
import com.telink.ble.mesh.core.networking.beacon.MeshPrivateBeacon;
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
import com.telink.ble.mesh.model.CertCacheService;
import com.telink.ble.mesh.model.FUCache;
import com.telink.ble.mesh.model.FUCacheService;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.OnlineState;
import com.telink.ble.mesh.model.UnitConvert;
import com.telink.ble.mesh.ui.fragment.DeviceFragment;
import com.telink.ble.mesh.ui.fragment.GroupFragment;
import com.telink.ble.mesh.ui.fragment.SettingFragment;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;

import java.io.ByteArrayInputStream;
import java.nio.ByteBuffer;
import java.security.cert.CertificateFactory;
import java.security.cert.X509Certificate;

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
        CertCacheService.getInstance().load(this); // load cert cache
//        mHandler.postDelayed(this::testMeshPrivateBeacon, 1000);

        mHandler.postDelayed(this::testNetworkId, 1500);
//
//        mHandler.postDelayed(this::testNodeIdentity, 1800);

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

    private void testCert() {
        byte[] certData = Arrays.hexToBytes("3082027F30820224A003020102020103300A06082A8648CE3D04030230818F310B30090603550406130255533113301106035504080C0A57617368696E67746F6E31163014060355040A0C0D426C7565746F6F746820534947310C300A060355040B0C03505453311F301D06035504030C16496E7465726D65646961746520417574686F726974793124302206092A864886F70D0109011615737570706F727440626C7565746F6F74682E636F6D301E170D3139303731383138353533365A170D3330313030343138353533365A3077310B30090603550406130255533113301106035504080C0A57617368696E67746F6E31163014060355040A0C0D426C7565746F6F746820534947310C300A060355040B0C03505453312D302B06035504030C2430303142444330382D313032312D304230452D304130432D3030304230453041304330303059301306072A8648CE3D020106082A8648CE3D03010703420004F465E43FF23D3F1B9DC7DFC04DA8758184DBC966204796ECCF0D6CF5E16500CC0201D048BCBBD899EEEFC424164E33C201C2B010CA6B4D43A8A155CAD8ECB279A3818730818430090603551D1304023000300B0603551D0F040403020308301D0603551D0E04160414E262F3584AB688EC882EA528ED8E5C442A71369F301F0603551D230418301680144ABE293903A8BB49FF1D327CFEB80985F4109C21302A06146982E19DE491EAC0C283999CAA83FD8CC3D0D3670412041000000000000000000102030405060708300A06082A8648CE3D0403020349003046022100F7B504477EC2E5796644A0C5A95D864BF001CF96A5A180E243432CCE28FC5F9E0221008D816BEE11C36CDC1890189EDB85DF9A26998063EAC8EA55330B7F75003FEB98");
        CertificateFactory factory = null;
        try {
            factory = CertificateFactory.getInstance("X.509");

            X509Certificate certificate = (X509Certificate) factory.generateCertificate(new ByteArrayInputStream(certData));
            MeshLogger.d("certificate info: " + certificate.toString());
            Encipher.getStaticOOBInCert(certificate);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void testMeshPrivateBeacon() {
        MeshLogger.d("start generate beacon");
        byte[] netKey = Arrays.hexToBytes("f7a2a44f8e8a8029064f173ddc1e2b00");
        byte[] pvtBeaconKey = Encipher.generatePrivateBeaconKey(netKey);
        MeshLogger.d("private beacon key: " + Arrays.bytesToHexString(pvtBeaconKey));
        int ivIndex = 0x1010abcc;
        // 435f18f85cf78a3121f58478a561e488e7cbf3174f022a514741
        // 435F18F85CF78A3121F58478A5229FDBD6BFF3174F022A514741
        MeshPrivateBeacon beacon = MeshPrivateBeacon.createIvUpdatingBeacon(ivIndex, pvtBeaconKey, true);
        byte[] beaconData = beacon.toBytes();
        MeshLogger.d("beacon: " + Arrays.bytesToHexString(beaconData));

        /*
//        byte[] netKey = Arrays.hexToBytes("f7a2a44f8e8a8029064f173ddc1e2b00");
        byte[] netKey = TelinkMeshApplication.getInstance().getMeshInfo().getDefaultNetKey().key;
        byte[] pvtBeaconKey = Encipher.generatePrivateBeaconKey(netKey);
        MeshLogger.d("private beacon key: " + Arrays.bytesToHexString(pvtBeaconKey));
        int ivIndex = 0x00;

        MeshPrivateBeacon beacon = MeshPrivateBeacon.createIvUpdatingBeacon(ivIndex, pvtBeaconKey, false);
        byte[] beaconData = beacon.toBytes();*/
        MeshLogger.d("beacon: " + Arrays.bytesToHexString(beaconData));
        testParsePrivateBeacon(beaconData, pvtBeaconKey);
    }

    public void testParsePrivateBeacon(byte[] beaconData, byte[] pvtBeaconKey) {
        // 02A790C3BED192C44A048E0772C3F21F904B2566A8A8539464E13C
        // privateBeaconKey - F36BFD60435998616D37A67398E5D47E
//        beaconData = Arrays.hexToBytes("02A790C3BED192C44A048E0772C3F21F904B2566A8A8539464E13C");
//        pvtBeaconKey = Arrays.hexToBytes("F36BFD60435998616D37A67398E5D47E");
        MeshLogger.d("start parse beacon");
        MeshPrivateBeacon privateBeacon = MeshPrivateBeacon.from(beaconData, pvtBeaconKey);
        if (privateBeacon != null) {
            MeshLogger.d(String.format("private beacon parse success: %08X", privateBeacon.getIvIndex()));
        }
    }


    public void testNetworkId() {
        MeshLogger.d("start testNetworkId");
        byte[] netKey = Arrays.hexToBytes("7dd7364cd842ad18c17c2b820c84c3d6");
        byte[] random = Arrays.hexToBytes("34ae608fbbc1f2c6");
//        byte[] networkId  = Arrays.hexToBytes("3ecaff672f673370");
        byte[] networkId = Encipher.k3(netKey);
        MeshLogger.d("networkId: " + Arrays.bytesToHexString(networkId));
        byte[] identityKey = Encipher.generateIdentityKey(netKey);
        MeshLogger.d("identityKey: " + Arrays.bytesToHexString(identityKey));
        byte[] hash = Encipher.aes(ByteBuffer.allocate(16).put(networkId).put(random).array(), identityKey);
        // A19967973D8094ECD30F7229EF045435
        MeshLogger.d("hash: " + Arrays.bytesToHexString(hash));
    }

    public void testNodeIdentity() {
        MeshLogger.d("start testNodeIdentity");
        byte[] netKey = Arrays.hexToBytes("7dd7364cd842ad18c17c2b820c84c3d6");
        byte[] random = Arrays.hexToBytes("34ae608fbbc1f2c6");
        int address = 0x1201;
        byte[] identityKey = Encipher.generateIdentityKey(netKey);
        byte[] padding = Arrays.hexToBytes("0000000000");
        byte[] data = ByteBuffer.allocate(16).put(padding).put((byte) 0x03).put(random).putShort((short) address)
                .array();
        byte[] hash = Encipher.aes(data, identityKey);
        // 2EBA33B59D60593E2C64A8CBCA65BFE1
        MeshLogger.d("hash: " + Arrays.bytesToHexString(hash));
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
        MeshService.getInstance().resetExtendBearerMode(SharedPreferenceHelper.getExtendBearerMode(this));
    }

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
            if (nodeInfo != null && nodeInfo.compositionData != null && nodeInfo.compositionData.pid == AppSettings.PID_REMOTE) {
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
                FUCacheService.getInstance().clear(MainActivity.this);
            }
        };

        builder.setTitle("Warning - MeshOTA is still running")
                .setMessage("MeshOTA distribution is still running, continue?\n" +
                        "click GO to enter MeshOTA processing page \n"
                        + "click STOP to stop distribution \n" +
                        "click CLEAR to clear cache")
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
