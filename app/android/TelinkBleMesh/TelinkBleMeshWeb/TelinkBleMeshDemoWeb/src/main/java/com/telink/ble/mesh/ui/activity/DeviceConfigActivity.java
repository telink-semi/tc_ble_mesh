/********************************************************************************************************
 * @file DeviceConfigActivity.java
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
package com.telink.ble.mesh.ui.activity;

import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.EditText;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.widget.AppCompatSpinner;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONException;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.core.message.NotificationMessage;
import com.telink.ble.mesh.core.message.config.BeaconGetMessage;
import com.telink.ble.mesh.core.message.config.BeaconSetMessage;
import com.telink.ble.mesh.core.message.config.BeaconStatusMessage;
import com.telink.ble.mesh.core.message.config.DefaultTTLGetMessage;
import com.telink.ble.mesh.core.message.config.DefaultTTLSetMessage;
import com.telink.ble.mesh.core.message.config.DefaultTTLStatusMessage;
import com.telink.ble.mesh.core.message.config.FriendGetMessage;
import com.telink.ble.mesh.core.message.config.FriendSetMessage;
import com.telink.ble.mesh.core.message.config.FriendStatusMessage;
import com.telink.ble.mesh.core.message.config.GattProxyGetMessage;
import com.telink.ble.mesh.core.message.config.GattProxySetMessage;
import com.telink.ble.mesh.core.message.config.GattProxyStatusMessage;
import com.telink.ble.mesh.core.message.config.KeyRefreshPhaseGetMessage;
import com.telink.ble.mesh.core.message.config.KeyRefreshPhaseStatusMessage;
import com.telink.ble.mesh.core.message.config.NetworkTransmitGetMessage;
import com.telink.ble.mesh.core.message.config.NetworkTransmitSetMessage;
import com.telink.ble.mesh.core.message.config.NetworkTransmitStatusMessage;
import com.telink.ble.mesh.core.message.config.NodeIdentity;
import com.telink.ble.mesh.core.message.config.NodeIdentityGetMessage;
import com.telink.ble.mesh.core.message.config.NodeIdentitySetMessage;
import com.telink.ble.mesh.core.message.config.NodeIdentityStatusMessage;
import com.telink.ble.mesh.core.message.config.RelayGetMessage;
import com.telink.ble.mesh.core.message.config.RelaySetMessage;
import com.telink.ble.mesh.core.message.config.RelayStatusMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.ReliableMessageProcessEvent;
import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;
import com.telink.ble.mesh.model.ConfigState;
import com.telink.ble.mesh.model.DeviceConfig;
import com.telink.ble.mesh.model.NodeStatusChangedEvent;
import com.telink.ble.mesh.model.web.MeshNodeConfig;
import com.telink.ble.mesh.ui.adapter.DeviceConfigListAdapter;
import com.telink.ble.mesh.util.MeshLogger;
import com.telink.ble.mesh.web.ResponseInfo;
import com.telink.ble.mesh.web.TelinkHttpClient;
import com.telink.ble.mesh.web.WebUtils;
import com.telink.ble.mesh.web.entity.MeshNode;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.Response;

/**
 * container for device control , group,  device settings
 * Created by kee on 2017/8/30.
 */
public class DeviceConfigActivity extends BaseActivity implements EventListener<String> {
    public final static String URL_GET_CONFIGS = TelinkHttpClient.URL_BASE + "node-config/getConfigs";

    public final static String URL_UPDATE_CONFIGS = TelinkHttpClient.URL_BASE + "node-config/updateConfigs";

    private MeshNode deviceInfo;
    private MeshNodeConfig configs;
    private Handler delayHandler = new Handler();
    private DeviceConfigListAdapter adapter;
    private List<DeviceConfig> deviceConfigList = new ArrayList<>();

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
        adapter = new DeviceConfigListAdapter(this, deviceConfigList);
        rv_dev_cfg.setAdapter(adapter);

        TelinkMeshApplication.getInstance().addEventListener(NetworkTransmitStatusMessage.class.getName(), this);
        TelinkMeshApplication.getInstance().addEventListener(BeaconStatusMessage.class.getName(), this);
        TelinkMeshApplication.getInstance().addEventListener(DefaultTTLStatusMessage.class.getName(), this);
        TelinkMeshApplication.getInstance().addEventListener(FriendStatusMessage.class.getName(), this);
        TelinkMeshApplication.getInstance().addEventListener(GattProxyStatusMessage.class.getName(), this);
        TelinkMeshApplication.getInstance().addEventListener(KeyRefreshPhaseStatusMessage.class.getName(), this);

        TelinkMeshApplication.getInstance().addEventListener(ReliableMessageProcessEvent.EVENT_TYPE_MSG_PROCESS_COMPLETE, this);

        TelinkMeshApplication.getInstance().addEventListener(RelayStatusMessage.class.getName(), this);
        TelinkMeshApplication.getInstance().addEventListener(NodeIdentityStatusMessage.class.getName(), this);

        refreshUI();
        getConfigs();
    }

    private void getConfigs() {
        MeshLogger.d("get configs - start");
        showIndeterminateLoading();
        HashMap<String, String> params = new HashMap<>();
        params.put("nodeId", deviceInfo.id + "");
        TelinkHttpClient.getInstance().get(URL_GET_CONFIGS, params, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                showError("get configs onFailure");
                dismissIndeterminateLoading();
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                dismissIndeterminateLoading();
                ResponseInfo responseInfo = WebUtils.parseResponse(response, DeviceConfigActivity.this);
                if (responseInfo == null) {
//                    showError("config response parse error");
                    return;
                }
                try {
                    MeshNodeConfig config = JSON.parseObject(responseInfo.data, MeshNodeConfig.class);
                    if (config == null) {
                        showError("config info parse error in response");
                        return;
                    }
                    configs = config;
                    initConfigs();
                } catch (JSONException e) {
                    e.getStackTrace();
                }
            }
        });
    }

    private void initConfigs() {
//        deviceConfigList = new ArrayList<>();
        deviceConfigList.clear();
        DeviceConfig deviceConfig;
        for (ConfigState cfg : ConfigState.values()) {
            deviceConfig = new DeviceConfig(cfg);
            switch (cfg) {
                case DEFAULT_TTL:
                    deviceConfig.desc = getTTLDesc(configs.defaultTtl);
                    break;

                case RELAY:
                    deviceConfig.desc = getRelayDesc(configs.relay, configs.relayRetransmit);
                    break;

                case SECURE_NETWORK_BEACON:
                    deviceConfig.desc = getBeaconDesc(configs.secureNetworkBeacon);
                    break;

                case PROXY:
                    deviceConfig.desc = getProxyDesc(configs.gattProxy);
                    break;

                case NODE_IDENTITY:
                    deviceConfig.desc = getNodeIdDesc(0);
                    break;

                case FRIEND:
                    deviceConfig.desc = getFriendDesc(configs.friend);
                    break;

                case KEY_REFRESH_PHASE:
                    deviceConfig.desc = getKeyRefreshPhase(-1);
                    break;

                case NETWORK_TRANSMIT:
                    deviceConfig.desc = getNetworkTransmitDesc(configs.networkRetransmit);
                    break;
            }
            deviceConfigList.add(deviceConfig);
        }
        runOnUiThread(() -> adapter.notifyDataSetChanged());

    }

    private String getTTLDesc(int ttl) {
        return "" + ttl;
    }

    private String getRelayDesc(int relay, int relayRetransmit) {
        return String.format("%s \nretransmit count: %02X\nretransmit interval steps: %02X",
                relay != 0 ? "enabled" : "disabled",
                relayRetransmit & 0b111, (relayRetransmit >> 3) & 0x1F);
    }

    private String getBeaconDesc(int beacon) {
        return (beacon != 0) ? "opened" : "closed";
    }

    private String getProxyDesc(int gattProxy) {
        return gattProxy != 0 ? "enabled" : "disabled";
    }

    private String getNodeIdDesc(int nodeIdentity) {
        return nodeIdentity == NodeIdentity.RUNNING.code ? "started" : "stopped";
    }

    private String getFriendDesc(int friend) {
        return friend != 0 ? "enabled" : "disabled";
    }


    private String getKeyRefreshPhase(int phase) {
        if (phase == -1) return "phase: unknown";
        return "phase: " + phase;
    }

    private String getNetworkTransmitDesc(int networkTransmit) {
        return String.format("\ntransmit count: %02X\ntransmit interval steps: %02X",
                networkTransmit & 0b111, (networkTransmit >> 3) & 0x1F);
    }

    public void onGetTap(int position) {
        MeshLogger.d("get tap : " + position);
        DeviceConfig config = deviceConfigList.get(position);
        String name = config.configState.name;
        MeshMessage meshMessage = null;
        int adr = deviceInfo.address;
        int netKeyIndex = TelinkMeshApplication.getInstance().getMeshInfo().getDefaultNetKey().index;
        switch (config.configState) {
            case DEFAULT_TTL:
                meshMessage = new DefaultTTLGetMessage(adr);
                break;

            case RELAY:
                meshMessage = new RelayGetMessage(adr);
                break;

            case SECURE_NETWORK_BEACON:
                meshMessage = new BeaconGetMessage(adr);
                break;

            case PROXY:
                meshMessage = new GattProxyGetMessage(adr);
                break;

            case NODE_IDENTITY:
                meshMessage = new NodeIdentityGetMessage(adr, netKeyIndex);
                break;

            case FRIEND:
                meshMessage = new FriendGetMessage(adr);
                break;

            case KEY_REFRESH_PHASE:
                meshMessage = KeyRefreshPhaseGetMessage.getSimple(adr, netKeyIndex);
                break;

            case NETWORK_TRANSMIT:
                meshMessage = new NetworkTransmitGetMessage(adr);
                break;
        }
        boolean cmdSent = MeshService.getInstance().sendMeshMessage(meshMessage);
        if (cmdSent) {
            showSendWaitingDialog("getting " + name + "...");
        } else {
            toastMsg("get message send error ");
        }
    }

    public void onSetTap(int position) {
        MeshLogger.d("set tap : " + position);
        DeviceConfig config = deviceConfigList.get(position);

//        startActivity(new Intent(this, DeviceConfigSettingActivity.class).putExtra("meshAddress", deviceInfo.meshAddress).putExtra("deviceConfig", config));
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("Set " + config.configState.name).setMessage("input new value");
        builder.setNegativeButton("cancel", null);
        switch (config.configState) {
            case DEFAULT_TTL:
                showDefaultTTLInputDialog(builder);
                break;

            case RELAY:
                showRelayInputDialog(builder);
                break;

            case SECURE_NETWORK_BEACON:
            case PROXY:
            case FRIEND:
            case NODE_IDENTITY:
                showSingleSelectDialog(builder, config.configState);
                break;

            case NETWORK_TRANSMIT:
                showNetworkInputDialog(builder);
                break;
            default:
                return;
        }
        builder.show();
    }


    /*

     */
    private void showDefaultTTLInputDialog(AlertDialog.Builder builder) {
        View view = LayoutInflater.from(this).inflate(R.layout.layout_config_ttl, null);
        final EditText et_input = view.findViewById(R.id.et_input);
        builder.setView(view).setPositiveButton("confirm", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.dismiss();
                String ttlInput = et_input.getText().toString().trim();
                if (TextUtils.isEmpty(ttlInput)) {
                    toastMsg("input empty");
                    return;
                }
                byte ttl = Byte.parseByte(ttlInput, 16);
                DefaultTTLSetMessage setMessage = DefaultTTLSetMessage.getSimple(deviceInfo.address, ttl);
                boolean cmdSent = MeshService.getInstance().sendMeshMessage(setMessage);
                if (cmdSent) {
                    showSendWaitingDialog("setting TTL ...");
                } else {
                    toastMsg("set message send error ");
                }
            }
        });
    }


    private void showRelayInputDialog(AlertDialog.Builder builder) {
        View view = LayoutInflater.from(this).inflate(R.layout.layout_cfg_relay, null);
        final EditText et_ret_cnt = view.findViewById(R.id.et_ret_cnt);
        final EditText et_ret_step = view.findViewById(R.id.et_ret_step);
        final String[] RELAY_OPTIONS = new String[]{"Enabled", "Disabled"};
        final int[] selectedIndex = {0};
        final AppCompatSpinner spinner = view.findViewById(R.id.sp);
        initSpinner(spinner, RELAY_OPTIONS, new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                selectedIndex[0] = position;
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });
        builder.setView(view).setPositiveButton("confirm", (dialog, which) -> {
            dialog.dismiss();

            String countInput = et_ret_cnt.getText().toString().trim();
            if (countInput.isEmpty()) {
                toastMsg("pls input count");
                return;
            }
            int count = Integer.parseInt(countInput, 16);

            String stepInput = et_ret_step.getText().toString().trim();
            if (stepInput.isEmpty()) {
                toastMsg("pls input steps");
                return;
            }
            int steps = Integer.parseInt(stepInput, 16);

            byte value = (byte) (selectedIndex[0] == 0 ? 1 : 0);
            RelaySetMessage setMessage = RelaySetMessage.getSimple(deviceInfo.address,
                    value, (byte) count, (byte) steps);
            boolean cmdSent = MeshService.getInstance().sendMeshMessage(setMessage);
            if (cmdSent) {
                showSendWaitingDialog("setting Relay ...");
            } else {
                toastMsg("set message send error ");
            }
        });
    }


    private void showSingleSelectDialog(AlertDialog.Builder builder, final ConfigState state) {
        final String[] values;
        final int address = deviceInfo.address;
        if (state == ConfigState.SECURE_NETWORK_BEACON) {
            values = new String[]{"Open the Broadcasting", "Close the Broadcasting"};
        } else if (state == ConfigState.PROXY || state == ConfigState.FRIEND) {
            values = new String[]{"Enabled", "Disabled"};
        } else if (state == ConfigState.NODE_IDENTITY) {
            // start 1, stop 0
            values = new String[]{"Start", "Stop"};
        } else {
            return;
        }
        View view = LayoutInflater.from(this).inflate(R.layout.layout_cfg_single_select, null);
        final int[] selectedIndex = {0};
        final AppCompatSpinner spinner = view.findViewById(R.id.sp);
        initSpinner(spinner, values, new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                selectedIndex[0] = position;
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });
        builder.setView(view).setPositiveButton("confirm", (dialog, which) -> {
            dialog.dismiss();
            byte value = (byte) (selectedIndex[0] == 0 ? 1 : 0);
            MeshMessage message = null;
            switch (state) {
                case SECURE_NETWORK_BEACON:
                    message = BeaconSetMessage.getSimple(address, value);
                    break;
                case PROXY:
                    message = GattProxySetMessage.getSimple(address, value);
                    break;

                case FRIEND:
                    message = FriendSetMessage.getSimple(address, value);
                    break;

                case NODE_IDENTITY:
                    int netKeyIndex = TelinkMeshApplication.getInstance().getMeshInfo().getDefaultNetKey().index;
                    message = NodeIdentitySetMessage.getSimple(address, netKeyIndex, value);
                    break;
            }
            boolean cmdSent = MeshService.getInstance().sendMeshMessage(message);
            if (cmdSent) {
                showSendWaitingDialog("setting proxy ...");
            } else {
                toastMsg("set message send error ");
            }
        });
    }


    private void showNetworkInputDialog(AlertDialog.Builder builder) {
        View view = LayoutInflater.from(this).inflate(R.layout.layout_cfg_network_trans, null);

        final EditText et_trans_cnt = view.findViewById(R.id.et_trans_cnt);
        final EditText et_trans_step = view.findViewById(R.id.et_trans_step);

        builder.setView(view).setPositiveButton("confirm", (dialog, which) -> {
            dialog.dismiss();
            String countInput = et_trans_cnt.getText().toString().trim();
            if (countInput.isEmpty()) {
                toastMsg("pls input count");
                return;
            }
            int count = Integer.parseInt(countInput, 16);

            String stepInput = et_trans_step.getText().toString().trim();
            if (stepInput.isEmpty()) {
                toastMsg("pls input steps");
                return;
            }
            int steps = Integer.parseInt(stepInput, 16);
            MeshMessage message = NetworkTransmitSetMessage.getSimple(deviceInfo.address, count, steps);
            boolean cmdSent = MeshService.getInstance().sendMeshMessage(message);
            if (cmdSent) {
                showSendWaitingDialog("setting network transmit ...");
            } else {
                toastMsg("set message send error ");
            }
        });
    }


    private void initSpinner(AppCompatSpinner spinner, String[] values, AdapterView.OnItemSelectedListener itemSelectedListener) {
        spinner.setDropDownWidth(400);
        spinner.setDropDownHorizontalOffset(100);
        spinner.setDropDownVerticalOffset(100);
        spinner.setSelection(0);
        ArrayAdapter<String> spinnerAdapter = new ArrayAdapter<String>(this,
                R.layout.layout_config_spinner_item, R.id.tv_name, values);
        spinner.setAdapter(spinnerAdapter);
        spinner.setOnItemSelectedListener(itemSelectedListener);
    }

    private void showSendWaitingDialog(String message) {
        showWaitingDialog(message);
        delayHandler.postDelayed(MSG_TIMEOUT_TASK, 6 * 1000);
    }

    private final Runnable MSG_TIMEOUT_TASK = this::dismissWaitingDialog;

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (delayHandler != null) {
            delayHandler.removeCallbacksAndMessages(null);
        }
        TelinkMeshApplication.getInstance().removeEventListener(this);
    }


    @Override
    public void performed(Event<String> event) {
        if (event instanceof StatusNotificationEvent) {
            MeshLogger.d("config - " + event.getType());
            StatusNotificationEvent notificationEvent = (StatusNotificationEvent) event;
            NotificationMessage notificationMessage = notificationEvent.getNotificationMessage();
            if (notificationMessage.getSrc() != deviceInfo.address) {
                return;
            }
            if (configs == null) return;
            DeviceConfig config = null;
            String desc = null;
            if (event.getType().equals(DefaultTTLStatusMessage.class.getName())) {

                DefaultTTLStatusMessage statusMessage = (DefaultTTLStatusMessage) notificationMessage.getStatusMessage();
                configs.resetDefaultTtl((int) statusMessage.ttl);
                config = getDeviceConfig(ConfigState.DEFAULT_TTL);
                if (config != null) {
                    config.desc = getTTLDesc(configs.defaultTtl);
                }
            } else if (event.getType().equals(RelayStatusMessage.class.getName())) {
                RelayStatusMessage relayStatusMessage = (RelayStatusMessage) notificationMessage.getStatusMessage();
                configs.resetRelay((int) relayStatusMessage.relay);
                configs.resetRelayRetransmit((relayStatusMessage.retransmitCount & 0b111) | (relayStatusMessage.retransmitIntervalSteps << 3));
                config = getDeviceConfig(ConfigState.RELAY);
                if (config != null) {
                    config.desc = getRelayDesc(configs.relay, configs.relayRetransmit);
                }
            } else if (event.getType().equals(BeaconStatusMessage.class.getName())) {
                BeaconStatusMessage beaconStatusMessage = (BeaconStatusMessage) notificationMessage.getStatusMessage();
                configs.resetSecureNetworkBeacon((int) beaconStatusMessage.beacon);
                config = getDeviceConfig(ConfigState.SECURE_NETWORK_BEACON);
                if (config != null) {
                    config.desc = getBeaconDesc(beaconStatusMessage.beacon);
                }
            } else if (event.getType().equals(GattProxyStatusMessage.class.getName())) {
                GattProxyStatusMessage proxyStatusMessage = (GattProxyStatusMessage) notificationMessage.getStatusMessage();
                configs.resetGattProxy((int) proxyStatusMessage.gattProxy);
                config = getDeviceConfig(ConfigState.PROXY);
                if (config != null) {
                    config.desc = getProxyDesc(configs.gattProxy);
                }
            } else if (event.getType().equals(NodeIdentityStatusMessage.class.getName())) {
                config = getDeviceConfig(ConfigState.NODE_IDENTITY);
                NodeIdentityStatusMessage statusMessage = (NodeIdentityStatusMessage) notificationMessage.getStatusMessage();
                if (config != null) {
                    config.desc = getNodeIdDesc(statusMessage.identity);
                }
            } else if (event.getType().equals(FriendStatusMessage.class.getName())) {
                FriendStatusMessage friendStatusMessage = (FriendStatusMessage) notificationMessage.getStatusMessage();
                configs.resetFriend((int) friendStatusMessage.friend);
                config = getDeviceConfig(ConfigState.FRIEND);
                if (config != null) {
                    config.desc = getFriendDesc(friendStatusMessage.friend);
                }
            } else if (event.getType().equals(KeyRefreshPhaseStatusMessage.class.getName())) {
                KeyRefreshPhaseStatusMessage statusMessage = (KeyRefreshPhaseStatusMessage) notificationMessage.getStatusMessage();
                byte phase = statusMessage.phase;
                config = getDeviceConfig(ConfigState.KEY_REFRESH_PHASE);
                if (config != null) {
                    config.desc = getKeyRefreshPhase(phase);
                }
            } else if (event.getType().equals(NetworkTransmitStatusMessage.class.getName())) {
                NetworkTransmitStatusMessage statusMessage = (NetworkTransmitStatusMessage) notificationMessage.getStatusMessage();

                int networkRetransmit = (statusMessage.count & 0b111) | (statusMessage.intervalSteps << 3);
                configs.resetNetworkRetransmit(networkRetransmit);

                config = getDeviceConfig(ConfigState.NETWORK_TRANSMIT);
                if (config != null) {
                    config.desc = getNetworkTransmitDesc(configs.networkRetransmit);
                }
            }
        }


        runOnUiThread(() -> {
            dismissWaitingDialog();
            adapter.notifyDataSetChanged();
        });
        delayHandler.removeCallbacks(MSG_TIMEOUT_TASK);
        if (event.getType().equals(NodeStatusChangedEvent.EVENT_TYPE_NODE_STATUS_CHANGED)) {
            refreshUI();
        } else if (event.getType().equals(ReliableMessageProcessEvent.EVENT_TYPE_MSG_PROCESS_COMPLETE)) {
            runOnUiThread(this::dismissWaitingDialog);
        }
    }

    private void refreshUI() {
        runOnUiThread(() -> adapter.notifyDataSetChanged());
    }

    private DeviceConfig getDeviceConfig(ConfigState configState) {
        for (DeviceConfig config : deviceConfigList) {
            if (config.configState == configState) {
                return config;
            }
        }
        return null;
    }

}
