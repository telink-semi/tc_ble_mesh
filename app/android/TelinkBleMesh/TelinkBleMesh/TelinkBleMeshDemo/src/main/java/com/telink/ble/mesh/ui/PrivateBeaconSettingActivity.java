/********************************************************************************************************
 * @file SubnetBridgeActivity.java
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
import android.widget.CompoundButton;
import android.widget.Switch;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.core.message.config.BeaconSetMessage;
import com.telink.ble.mesh.core.message.config.BeaconStatusMessage;
import com.telink.ble.mesh.core.message.config.GattProxySetMessage;
import com.telink.ble.mesh.core.message.config.GattProxyStatusMessage;
import com.telink.ble.mesh.core.message.config.NodeIdentitySetMessage;
import com.telink.ble.mesh.core.message.config.NodeIdentityStatusMessage;
import com.telink.ble.mesh.core.message.privatebeacon.PrivateBeaconSetMessage;
import com.telink.ble.mesh.core.message.privatebeacon.PrivateBeaconStatusMessage;
import com.telink.ble.mesh.core.message.privatebeacon.PrivateGattProxySetMessage;
import com.telink.ble.mesh.core.message.privatebeacon.PrivateGattProxyStatusMessage;
import com.telink.ble.mesh.core.message.privatebeacon.PrivateNodeIdentitySetMessage;
import com.telink.ble.mesh.core.message.privatebeacon.PrivateNodeIdentityStatusMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.AutoConnectEvent;
import com.telink.ble.mesh.foundation.event.MeshEvent;
import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.util.MeshLogger;

/**
 * network key in target device
 */
public class PrivateBeaconSettingActivity extends BaseActivity implements EventListener<String> {

    private MeshInfo meshInfo;
    private int directAdr = 0;
    private NodeInfo nodeInfo;
    private Handler handler = new Handler();
    Switch sw_gatt_prx, sw_pvt_gatt_prx, sw_node_id, sw_pvt_node_id, sw_beacon, sw_pvt_beacon;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_private_beacon);
        setTitle("Private Beacon Setting");
        enableBackNav(true);
        initView();
        initData();
        TelinkMeshApplication.getInstance().addEventListener(GattProxyStatusMessage.class.getName(), this);
        TelinkMeshApplication.getInstance().addEventListener(PrivateGattProxyStatusMessage.class.getName(), this);
        TelinkMeshApplication.getInstance().addEventListener(NodeIdentityStatusMessage.class.getName(), this);
        TelinkMeshApplication.getInstance().addEventListener(PrivateNodeIdentityStatusMessage.class.getName(), this);
        TelinkMeshApplication.getInstance().addEventListener(BeaconStatusMessage.class.getName(), this);
        TelinkMeshApplication.getInstance().addEventListener(PrivateBeaconStatusMessage.class.getName(), this);
        TelinkMeshApplication.getInstance().addEventListener(AutoConnectEvent.EVENT_TYPE_AUTO_CONNECT_LOGIN, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_DISCONNECTED, this);
    }


    private void initView() {
        sw_gatt_prx = findViewById(R.id.sw_gatt_prx);
        sw_pvt_gatt_prx = findViewById(R.id.sw_pvt_gatt_prx);
        sw_node_id = findViewById(R.id.sw_node_id);
        sw_pvt_node_id = findViewById(R.id.sw_pvt_node_id);
        sw_beacon = findViewById(R.id.sw_beacon);
        sw_pvt_beacon = findViewById(R.id.sw_pvt_beacon);

    }

    private void initData() {
        meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
        directAdr = MeshService.getInstance().getDirectConnectedNodeAddress();
        getNode();

        sw_gatt_prx.setOnCheckedChangeListener(SWITCH_CHANGED);
        sw_pvt_gatt_prx.setOnCheckedChangeListener(SWITCH_CHANGED);
        sw_node_id.setOnCheckedChangeListener(SWITCH_CHANGED);
        sw_pvt_node_id.setOnCheckedChangeListener(SWITCH_CHANGED);
        sw_beacon.setOnCheckedChangeListener(SWITCH_CHANGED);
        sw_pvt_beacon.setOnCheckedChangeListener(SWITCH_CHANGED);
    }

    private void getNode() {
        if (directAdr == 0) {
            nodeInfo = null;
            enableUi(false);
            return;
        }
        nodeInfo = TelinkMeshApplication.getInstance().getMeshInfo().getDeviceByMeshAddress(directAdr);
        if (nodeInfo != null) {
            sw_gatt_prx.setChecked(nodeInfo.gattProxyEnable);
            sw_pvt_gatt_prx.setChecked(nodeInfo.privateGattProxyEnable);
            sw_beacon.setChecked(nodeInfo.beaconOpened);
            sw_pvt_beacon.setChecked(nodeInfo.privateBeaconOpened);
            enableUi(true);
        } else {
            enableUi(false);
        }
    }

    private void enableUi(boolean enable) {
        sw_gatt_prx.setEnabled(enable);
        sw_pvt_gatt_prx.setEnabled(enable);
        sw_node_id.setEnabled(enable);
        sw_pvt_node_id.setEnabled(enable);
        sw_beacon.setEnabled(enable);
        sw_pvt_beacon.setEnabled(enable);
    }

    private Runnable NODE_ID_RESET = new Runnable() {
        @Override
        public void run() {
            sw_node_id.setChecked(false);
        }
    };


    private Runnable PRIVATE_NODE_ID_RESET = new Runnable() {
        @Override
        public void run() {
            sw_pvt_node_id.setChecked(false);
        }
    };


    CompoundButton.OnCheckedChangeListener SWITCH_CHANGED = (view, isChecked) -> {
//        MeshLogger.d("SWITCH_CHANGED#" + isChecked);
        if (directAdr == 0) {
            toastMsg("please connect to mesh first");
            view.setChecked(!isChecked);
            return;
        }
        MeshMessage message = null;
        switch (view.getId()) {
            case R.id.sw_gatt_prx:
                message = GattProxySetMessage.getSimple(directAdr, (byte) (isChecked ? 1 : 0));
                break;

            case R.id.sw_pvt_gatt_prx:
                message = PrivateGattProxySetMessage.getSimple(directAdr, (byte) (isChecked ? 1 : 0));
                break;

            case R.id.sw_node_id:
                message = NodeIdentitySetMessage.getSimple(directAdr, meshInfo.getDefaultNetKey().index, (byte) (isChecked ? 1 : 0));
                handler.removeCallbacks(NODE_ID_RESET);
                if (isChecked) {
                    handler.postDelayed(NODE_ID_RESET, 10 * 1000);
                }
                break;

            case R.id.sw_pvt_node_id:
                message = PrivateNodeIdentitySetMessage.getSimple(directAdr, meshInfo.getDefaultNetKey().index, (byte) (isChecked ? 1 : 0));
                handler.removeCallbacks(PRIVATE_NODE_ID_RESET);
                if (isChecked) {
                    handler.postDelayed(PRIVATE_NODE_ID_RESET, 10 * 1000);
                }
                break;

            case R.id.sw_beacon:
                message = BeaconSetMessage.getSimple(directAdr, (byte) (isChecked ? 1 : 0));
                break;
            case R.id.sw_pvt_beacon:
                message = PrivateBeaconSetMessage.getSimple(directAdr, (byte) (isChecked ? 1 : 0));
                break;
        }
        if (message != null) {
            MeshService.getInstance().sendMeshMessage(message);
        }
    };

    @Override
    protected void onDestroy() {
        super.onDestroy();
        handler.removeCallbacksAndMessages(null);
        TelinkMeshApplication.getInstance().removeEventListener(this);
    }

    private Runnable timeoutTask = () -> {
        toastMsg("processing timeout");
        dismissWaitingDialog();
    };

    @Override
    public void performed(Event<String> event) {
        if (event.getType().equals(GattProxyStatusMessage.class.getName())) {
            GattProxyStatusMessage stMessage = (GattProxyStatusMessage) ((StatusNotificationEvent) event).getNotificationMessage().getStatusMessage();
            handler.removeCallbacksAndMessages(null);
            if (nodeInfo != null) {
                nodeInfo.gattProxyEnable = stMessage.gattProxy == 1;
                nodeInfo.save();
            }
        } else if (event.getType().equals(PrivateGattProxyStatusMessage.class.getName())) {
            PrivateGattProxyStatusMessage stMessage = (PrivateGattProxyStatusMessage) ((StatusNotificationEvent) event).getNotificationMessage().getStatusMessage();
            handler.removeCallbacksAndMessages(null);
            if (nodeInfo != null) {
                nodeInfo.privateGattProxyEnable = stMessage.privateGattProxy == 1;
                nodeInfo.save();
            }
        } else if (event.getType().equals(NodeIdentityStatusMessage.class.getName())) {
            NodeIdentityStatusMessage stMessage = (NodeIdentityStatusMessage) ((StatusNotificationEvent) event).getNotificationMessage().getStatusMessage();
            handler.removeCallbacksAndMessages(null);
        } else if (event.getType().equals(PrivateNodeIdentityStatusMessage.class.getName())) {
            PrivateNodeIdentityStatusMessage stMessage = (PrivateNodeIdentityStatusMessage) ((StatusNotificationEvent) event).getNotificationMessage().getStatusMessage();
            handler.removeCallbacksAndMessages(null);
        } else if (event.getType().equals(BeaconStatusMessage.class.getName())) {
            BeaconStatusMessage stMessage = (BeaconStatusMessage) ((StatusNotificationEvent) event).getNotificationMessage().getStatusMessage();
            handler.removeCallbacksAndMessages(null);
            if (nodeInfo != null) {
                nodeInfo.beaconOpened = stMessage.beacon == 1;
                nodeInfo.save();
            }
        } else if (event.getType().equals(PrivateBeaconStatusMessage.class.getName())) {
            PrivateBeaconStatusMessage stMessage = (PrivateBeaconStatusMessage) ((StatusNotificationEvent) event).getNotificationMessage().getStatusMessage();
            handler.removeCallbacksAndMessages(null);
            if (nodeInfo != null) {
                nodeInfo.privateBeaconOpened = stMessage.privateBeacon == 1;
                nodeInfo.save();
            }
        } else if (event.getType().equals(AutoConnectEvent.EVENT_TYPE_AUTO_CONNECT_LOGIN)) {
            AutoConnectEvent evt = (AutoConnectEvent) event;
            directAdr = evt.getConnectedAddress();
            getNode();
        } else if (event.getType().equals(MeshEvent.EVENT_TYPE_DISCONNECTED)) {
            directAdr = 0;
            getNode();
        }
    }


}
