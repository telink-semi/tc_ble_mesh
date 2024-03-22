/********************************************************************************************************
 * @file LightingControlActivity.java
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
import android.text.method.DigitsKeyListener;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.Switch;

import androidx.appcompat.app.AlertDialog;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.DeviceProperty;
import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.MeshSigModel;
import com.telink.ble.mesh.core.message.lighting.LcLightOnOffGetMessage;
import com.telink.ble.mesh.core.message.lighting.LcLightOnOffSetMessage;
import com.telink.ble.mesh.core.message.lighting.LcLightOnOffStatusMessage;
import com.telink.ble.mesh.core.message.lighting.LcModeGetMessage;
import com.telink.ble.mesh.core.message.lighting.LcModeSetMessage;
import com.telink.ble.mesh.core.message.lighting.LcModeStatusMessage;
import com.telink.ble.mesh.core.message.lighting.LcOmGetMessage;
import com.telink.ble.mesh.core.message.lighting.LcOmSetMessage;
import com.telink.ble.mesh.core.message.lighting.LcOmStatusMessage;
import com.telink.ble.mesh.core.message.lighting.LcPropertyGetMessage;
import com.telink.ble.mesh.core.message.lighting.LcPropertySetMessage;
import com.telink.ble.mesh.core.message.lighting.LcPropertyStatusMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.NodeLcProps;
import com.telink.ble.mesh.model.db.MeshInfoService;
import com.telink.ble.mesh.ui.adapter.LcPropertyListAdapter;
import com.telink.ble.mesh.util.MeshLogger;

import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.List;

/**
 * device firmware update by gatt ota
 */
public class LightingControlActivity extends BaseActivity implements View.OnClickListener, EventListener<String> {

    //    private Button btn_back;
    private Switch switch_mode, switch_lc_om, switch_set_on_off;
    private NodeInfo nodeInfo;
    private MeshInfo meshInfo;
    private int lcEleAdr;
    private NodeLcProps nodeLcProps;
    private List<LcPropItem> lcPropItems;
    private LcPropertyListAdapter adapter;
    private Handler delayHandler = new Handler();

    /**
     * Enable LC Occupancy mode
     */
    private boolean lcOmEnabled = true;

    /**
     * Enable LC mode
     */
    private boolean lcEnabled = true;

    private boolean lcOn = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_lighting_control);
        setTitle("Lighting Control");
        enableBackNav(true);

        Intent intent = getIntent();
        if (intent.hasExtra("meshAddress")) {
            int meshAddress = intent.getIntExtra("meshAddress", -1);
            nodeInfo = TelinkMeshApplication.getInstance().getMeshInfo().getDeviceByMeshAddress(meshAddress);
            lcEleAdr = nodeInfo.getTargetEleAdr(MeshSigModel.SIG_MD_LIGHT_LC_S.modelId);
        } else {
            toastMsg("device error");
            finish();
            return;
        }
        meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
        initProps();
        initViews();
        TelinkMeshApplication.getInstance().addEventListener(LcModeStatusMessage.class.getName(), this);
        TelinkMeshApplication.getInstance().addEventListener(LcOmStatusMessage.class.getName(), this);
        TelinkMeshApplication.getInstance().addEventListener(LcLightOnOffStatusMessage.class.getName(), this);
        TelinkMeshApplication.getInstance().addEventListener(LcPropertyStatusMessage.class.getName(), this);
        MeshService.getInstance().idle(false);

        getOnOffState();
    }


    private void getOnOffState() {
        MeshService.getInstance().sendMeshMessage(LcOmGetMessage.getSimple(nodeInfo.meshAddress, meshInfo.getDefaultAppKeyIndex(), 1));
        delayHandler.postDelayed(() -> MeshService.getInstance().sendMeshMessage(LcModeGetMessage.getSimple(nodeInfo.meshAddress, meshInfo.getDefaultAppKeyIndex(), 1)), 500);
        delayHandler.postDelayed(() -> MeshService.getInstance().sendMeshMessage(LcLightOnOffGetMessage.getSimple(nodeInfo.meshAddress, meshInfo.getDefaultAppKeyIndex(), 1)), 500);
    }

    private void initViews() {
        switch_mode = findViewById(R.id.switch_mode);
        switch_lc_om = findViewById(R.id.switch_lc_om);
        switch_set_on_off = findViewById(R.id.switch_set_on_off);
//        updateLcModeStatus();
        switch_mode.setOnCheckedChangeListener(SWITCH_CHECK_LSN);
        switch_lc_om.setOnCheckedChangeListener(SWITCH_CHECK_LSN);
        switch_set_on_off.setChecked(false);
        switch_set_on_off.setOnCheckedChangeListener(SWITCH_CHECK_LSN);

        adapter = new LcPropertyListAdapter(this, lcPropItems);

        RecyclerView rv_lc_prop = findViewById(R.id.rv_lc_prop);
        rv_lc_prop.setLayoutManager(new LinearLayoutManager(this));
        rv_lc_prop.setAdapter(adapter);
//        findViewById(R.id.btn_set_on).setOnClickListener(this);
//        findViewById(R.id.btn_set_off).setOnClickListener(this);
    }

    private void initProps() {
        lcPropItems = new ArrayList<>();
        nodeLcProps = nodeInfo.getLcProps();
        MeshLogger.d("nodeLcProps#id : " + nodeLcProps.id);
        LcPropItem item;
        for (DeviceProperty prop : DeviceProperty.getLcProperties()) {
            item = new LcPropItem();
            item.property = prop;
            item.value = nodeLcProps.getPropertyValue(prop);
            item.expanded = false;
            lcPropItems.add(item);
        }
    }

    private void saveProps() {
        MeshInfoService.getInstance().updateNodeLcProps(nodeLcProps);
    }

    public void onGetTap(int position) {
        MeshLogger.d("get tap : " + position);
        LcPropItem config = lcPropItems.get(position);
        String name = config.property.name;
//        int adr = nodeInfo.meshAddress;
        int adr = lcEleAdr;
        int propId = config.property.id;
        LcPropertyGetMessage message = LcPropertyGetMessage.getSimple(adr, meshInfo.getDefaultAppKeyIndex(), 1, propId);
        boolean cmdSent = MeshService.getInstance().sendMeshMessage(message);
        if (cmdSent) {
            showSendWaitingDialog("getting " + name + "...");
        } else {
            toastMsg("get message send error ");
        }
    }

    public void onSetTap(int position) {
        MeshLogger.d("set tap : " + position);
        LcPropItem item = lcPropItems.get(position);
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("Set " + item.property.name).setMessage("input new value");
        builder.setNegativeButton("cancel", null);
        View view = LayoutInflater.from(this).inflate(R.layout.dialog_single_input, null);
        final EditText et_single_input = view.findViewById(R.id.et_single_input);
        et_single_input.setKeyListener(DigitsKeyListener.getInstance(getString(R.string.number_input)));
        builder.setView(view).setPositiveButton("Confirm", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                String input = et_single_input.getText().toString();
                int val;
                try {
                    val = Integer.parseInt(input);
                } catch (Exception e) {
                    e.printStackTrace();
                    toastMsg("input error");
                    return;
                }
                byte[] value = MeshUtils.integer2Bytes(val, item.property.len, ByteOrder.LITTLE_ENDIAN);
                LcPropertySetMessage setMessage = LcPropertySetMessage.getSimple(
                        lcEleAdr, meshInfo.getDefaultAppKeyIndex(), item.property.id,
                        value, true, 1);
                MeshService.getInstance().sendMeshMessage(setMessage);
            }
        })
        ;
        builder.show();
    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
        TelinkMeshApplication.getInstance().removeEventListener(this);
        if (delayHandler != null) {
            delayHandler.removeCallbacksAndMessages(null);
        }
    }

    private void showSendWaitingDialog(String message) {
        showWaitingDialog(message);
        delayHandler.postDelayed(MSG_TIMEOUT_TASK, 6 * 1000);
    }

    private final Runnable MSG_TIMEOUT_TASK = this::dismissWaitingDialog;

    private CompoundButton.OnCheckedChangeListener SWITCH_CHECK_LSN = new CompoundButton.OnCheckedChangeListener() {
        @Override
        public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
            if (buttonView == switch_mode) {
                if (isChecked && !lcEnabled) {
                    MeshService.getInstance().sendMeshMessage(
                            LcModeSetMessage.getSimple(lcEleAdr, meshInfo.getDefaultAppKeyIndex(),
                                    (byte) 1, true, 1)
                    );
                } else if (!isChecked && lcEnabled) {
                    MeshService.getInstance().sendMeshMessage(
                            LcModeSetMessage.getSimple(lcEleAdr, meshInfo.getDefaultAppKeyIndex(),
                                    (byte) 0, true, 1)
                    );
                }
            } else if (buttonView == switch_set_on_off) {
                MeshService.getInstance().sendMeshMessage(
                        LcLightOnOffSetMessage.getSimple(lcEleAdr, meshInfo.getDefaultAppKeyIndex(),
                                (byte) (isChecked ? 1 : 0), true, 1)
                );
                delayedRefresh();
            } else if (buttonView == switch_lc_om) {
                if (isChecked && !lcOmEnabled) {
                    MeshService.getInstance().sendMeshMessage(
                            LcOmSetMessage.getSimple(lcEleAdr, meshInfo.getDefaultAppKeyIndex(),
                                    (byte) 1, true, 1)
                    );
                } else if (!isChecked && lcOmEnabled) {
                    MeshService.getInstance().sendMeshMessage(
                            LcOmSetMessage.getSimple(lcEleAdr, meshInfo.getDefaultAppKeyIndex(),
                                    (byte) 0, true, 1)
                    );
                }
            }
        }
    };

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_set_on:
                LcLightOnOffSetMessage onMessage = new LcLightOnOffSetMessage(lcEleAdr, meshInfo.getDefaultAppKeyIndex());
                onMessage.setLightOnOff((byte) 1);
                MeshService.getInstance().sendMeshMessage(onMessage);
                break;

            case R.id.btn_set_off:
                LcLightOnOffSetMessage offMessage = new LcLightOnOffSetMessage(lcEleAdr, meshInfo.getDefaultAppKeyIndex());
                offMessage.setLightOnOff((byte) 0);
                MeshService.getInstance().sendMeshMessage(offMessage);
                break;
        }
    }

    private void delayedRefresh() {
        delayHandler.removeCallbacks(REFRESH_TASK);
        delayHandler.postDelayed(REFRESH_TASK, getRefreshDelay());
    }

    private Runnable REFRESH_TASK = new Runnable() {
        @Override
        public void run() {
            MeshService.getInstance().sendMeshMessage(
                    LcLightOnOffGetMessage.getSimple(lcEleAdr, meshInfo.getDefaultAppKeyIndex(), 1));
        }
    };

    private long getRefreshDelay() {
        NodeLcProps prop = nodeInfo.getLcProps();
        long timeFadeOn = prop.timeFadeOn == -1 ? 2000 : prop.timeFadeOn;
        long timeRunOn = prop.timeRunOn == -1 ? 5000 : prop.timeRunOn;
        long timeFade = prop.timeFade == -1 ? 2000 : prop.timeFade;
        long timeProlong = prop.timeProlong == -1 ? 4000 : prop.timeProlong;
        long timeFadeStandbyAuto = prop.timeFadeStandbyAuto == -1 ? 3000 : prop.timeFadeStandbyAuto;
        return timeFadeOn + timeRunOn + timeFade + timeProlong + timeFadeStandbyAuto;
    }

    @Override
    public void performed(final Event<String> event) {
        if (event.getType().equals(LcModeStatusMessage.class.getName())) {
            //
            LcModeStatusMessage statusMessage = (LcModeStatusMessage) ((StatusNotificationEvent) event).getNotificationMessage().getStatusMessage();
            lcEnabled = statusMessage.getMode() == 1;

            runOnUiThread(() -> {
                switch_mode.setChecked(lcEnabled);
            });
        } else if (event.getType().equals(LcLightOnOffStatusMessage.class.getName())) {

        } else if (event.getType().equals(LcPropertyStatusMessage.class.getName())) {
            delayHandler.removeCallbacks(MSG_TIMEOUT_TASK);
            runOnUiThread(this::dismissWaitingDialog);

            LcPropertyStatusMessage statusMessage = (LcPropertyStatusMessage) ((StatusNotificationEvent) event).getNotificationMessage().getStatusMessage();
            boolean updateRe = updateItem(statusMessage.getPropertyID(), statusMessage.getPropertyValue());
            if (updateRe) {
                MeshLogger.d("node info save");
                saveProps();
                runOnUiThread(() -> adapter.notifyDataSetChanged());
            }
        } else if (event.getType().equals(LcOmStatusMessage.class.getName())) {
            LcOmStatusMessage statusMessage = (LcOmStatusMessage) ((StatusNotificationEvent) event).getNotificationMessage().getStatusMessage();
            lcOn = statusMessage.getMode() == 1;
            runOnUiThread(() -> switch_lc_om.setChecked(lcOn));
        }
    }

    private boolean updateItem(int id, byte[] newVal) {
        int val = MeshUtils.bytes2Integer(newVal, ByteOrder.LITTLE_ENDIAN);
        for (LcPropItem item : lcPropItems) {
            if (item.property.id == id) {
                if (item.value != val) {
                    item.value = val;
                    nodeLcProps.updatePropertyValue(item.property, val);
                    return true;
                } else {
                    return false;
                }
            }
        }
        return false;
    }


    public class LcPropItem {
        public DeviceProperty property;

        public boolean expanded = false;

        public int value;
    }

}
