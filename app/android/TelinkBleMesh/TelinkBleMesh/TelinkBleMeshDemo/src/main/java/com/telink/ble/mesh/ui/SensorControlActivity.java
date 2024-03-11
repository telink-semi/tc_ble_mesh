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

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;

import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.message.MeshSigModel;
import com.telink.ble.mesh.core.message.config.ConfigStatus;
import com.telink.ble.mesh.core.message.config.ModelPublicationSetMessage;
import com.telink.ble.mesh.core.message.config.ModelPublicationStatusMessage;
import com.telink.ble.mesh.core.message.sensor.SensorCadenceGetMessage;
import com.telink.ble.mesh.core.message.sensor.SensorCadenceStatusMessage;
import com.telink.ble.mesh.core.message.sensor.SensorDescriptorGetMessage;
import com.telink.ble.mesh.core.message.sensor.SensorDescriptorStatusMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.entity.ModelPublication;
import com.telink.ble.mesh.entity.SensorCadence;
import com.telink.ble.mesh.entity.SensorDescriptor;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.NodeSensorState;
import com.telink.ble.mesh.model.PublishModel;
import com.telink.ble.mesh.ui.adapter.SensorItemAdapter;
import com.telink.ble.mesh.util.MeshLogger;

import java.util.ArrayList;
import java.util.List;

/**
 * device firmware update by gatt ota
 */
public class SensorControlActivity extends BaseActivity implements View.OnClickListener, EventListener<String> {

    private NodeInfo nodeInfo;
    private MeshInfo meshInfo;

    private List<SensorItem> sensorItems;
    private SensorItemAdapter adapter;
    private Handler delayHandler = new Handler();
    private TextView tv_sensor_pub_title;
    private EditText et_pub_adr, et_period;
    PublishModel pubModel;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_sensor_control);
        setTitle("Sensor Control");
        enableBackNav(true);

        Intent intent = getIntent();
        if (intent.hasExtra("meshAddress")) {
            int meshAddress = intent.getIntExtra("meshAddress", -1);
            nodeInfo = TelinkMeshApplication.getInstance().getMeshInfo().getDeviceByMeshAddress(meshAddress);
        } else {
            toastMsg("device error");
            finish();
            return;
        }
        meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
        initSensor();
        initViews();
        updatePubSt();
        TelinkMeshApplication.getInstance().addEventListener(SensorDescriptorStatusMessage.class.getName(), this);
        TelinkMeshApplication.getInstance().addEventListener(SensorCadenceStatusMessage.class.getName(), this);
        TelinkMeshApplication.getInstance().addEventListener(ModelPublicationStatusMessage.class.getName(), this);

//        MeshService.getInstance().sendMeshMessage(SensorDescriptorGetMessage.getAll(nodeInfo.meshAddress, meshInfo.getDefaultAppKeyIndex()));
    }

    private void initViews() {

        adapter = new SensorItemAdapter(this, sensorItems);
        RecyclerView rv_sns_prop = findViewById(R.id.rv_sns_prop);

        rv_sns_prop.setLayoutManager(new LinearLayoutManager(this));
        rv_sns_prop.setAdapter(adapter);

        tv_sensor_pub_title = findViewById(R.id.tv_sensor_pub_title);
        et_pub_adr = findViewById(R.id.et_pub_adr);
        et_period = findViewById(R.id.et_period);

        findViewById(R.id.btn_set_pub).setOnClickListener(this);
    }

    private void updatePubSt() {
        PublishModel model = nodeInfo.getPublishModelTarget();
        if (model == null) {
            tv_sensor_pub_title.setText("Sensor Publication(Not Configured): ");
        } else {
            tv_sensor_pub_title.setText("Sensor Publication(Configured): ");
            et_pub_adr.setText(Integer.toHexString(model.address).toUpperCase());
            et_period.setText(model.period + "");
        }
    }

    private void initSensor() {
        nodeInfo.sensorStateList.size();
        sensorItems = new ArrayList<>();
//        nodeLcProps = nodeInfo.getLcProps();
//        MeshLogger.d("nodeLcProps#id : " + nodeLcProps.id);
        SensorItem item;
        for (NodeSensorState nodeSensorState : nodeInfo.sensorStateList) {
            item = new SensorItem();
            item.propertyId = nodeSensorState.propertyID;
            sensorItems.add(item);
        }
        /*for (DeviceProperty prop : DeviceProperty.values()) {
            item = new LcPropItem();
            item.property = prop;
            item.value = nodeLcProps.getPropertyValue(prop);
            item.expanded = false;
            lcPropItems.add(item);
        }*/
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

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_set_pub:
                if (!checkPubInput()) {
                    return;
                }
                int appKeyIndex = TelinkMeshApplication.getInstance().getMeshInfo().getDefaultAppKeyIndex();
                int modelId = pubModel.modelId;
                int eleAdr = nodeInfo.getTargetEleAdr(modelId);
                ModelPublication modelPublication = ModelPublication.createDefault(eleAdr, pubModel.address, appKeyIndex, pubModel.period, pubModel.modelId, true);
                ModelPublicationSetMessage publicationSetMessage = new ModelPublicationSetMessage(nodeInfo.meshAddress, modelPublication);
                boolean result = MeshService.getInstance().sendMeshMessage(publicationSetMessage);
                if (result) {
                    showWaitingDialog("setting pub ...");
                    delayHandler.removeCallbacks(pubCmdTimeoutTask);
                    delayHandler.postDelayed(pubCmdTimeoutTask, 5 * 1000);
                }
                break;
        }
    }


    private Runnable pubCmdTimeoutTask = () -> {
        toastMsg("pub timeout");
        dismissWaitingDialog();
    };


    private boolean checkPubInput() {
        try {
            String adrInput = et_pub_adr.getText().toString().trim();
            int address = Integer.parseInt(adrInput, 16);
            String periodInput = et_period.getText().toString().trim();
            int period = Integer.parseInt(periodInput, 10);
            int modelId = MeshSigModel.SIG_MD_SENSOR_S.modelId;
            int pubEleAdr = nodeInfo.getTargetEleAdr(modelId);
            pubModel = new PublishModel(pubEleAdr, modelId, address, period);
            return true;
        } catch (Exception e) {
            toastMsg("input error");
            e.printStackTrace();
            return false;
        }
    }

    private void updateCadence(SensorCadence cadence) {
        boolean updated = false;
        for (SensorItem item : sensorItems) {
            if (item.propertyId == cadence.propertyID) {
                item.cadence = cadence;
                updated = true;
                break;
            }
        }
        if (updated) {
            runOnUiThread(() -> adapter.notifyDataSetChanged());
        }
    }

    private void updateDescriptor(SensorDescriptor descriptor) {
        boolean updated = false;
        for (SensorItem item : sensorItems) {
            if (item.propertyId == descriptor.propertyID) {
                item.descriptor = descriptor;
                updated = true;
                break;
            }
        }
        if (updated) {
            runOnUiThread(() -> adapter.notifyDataSetChanged());
        }
    }

    public void getCadence(int propertyID) {
        SensorCadenceGetMessage message = SensorCadenceGetMessage.getSimple(nodeInfo.meshAddress, meshInfo.getDefaultAppKeyIndex(), propertyID);
        MeshService.getInstance().sendMeshMessage(message);
    }

    public void getDescriptor(int propertyID) {
        SensorDescriptorGetMessage message = SensorDescriptorGetMessage.getSimple(nodeInfo.meshAddress, meshInfo.getDefaultAppKeyIndex(), propertyID);
        MeshService.getInstance().sendMeshMessage(message);
    }


    @Override
    public void performed(final Event<String> event) {
        if (event.getType().equals(SensorDescriptorStatusMessage.class.getName())) {
            SensorDescriptorStatusMessage statusMessage = (SensorDescriptorStatusMessage) ((StatusNotificationEvent) event).getNotificationMessage().getStatusMessage();
            if (statusMessage.descriptorList.size() == 1) {
                updateDescriptor(statusMessage.descriptorList.get(0));
            }
        } else if (event.getType().equals(SensorCadenceStatusMessage.class.getName())) {
            SensorCadenceStatusMessage statusMessage = (SensorCadenceStatusMessage) ((StatusNotificationEvent) event).getNotificationMessage().getStatusMessage();
            updateCadence(statusMessage.cadence);
        } else if (event.getType().equals(ModelPublicationStatusMessage.class.getName())) {
            final ModelPublicationStatusMessage statusMessage = (ModelPublicationStatusMessage) ((StatusNotificationEvent) event).getNotificationMessage().getStatusMessage();
            if (statusMessage.getStatus() == ConfigStatus.SUCCESS.code) {
                runOnUiThread(() -> {
                    delayHandler.removeCallbacks(pubCmdTimeoutTask);
                    dismissWaitingDialog();
                    boolean settle = statusMessage.getPublication().publishAddress != 0;
                    nodeInfo.setPublishModel(settle ? pubModel : null);
                    nodeInfo.save();
                    updatePubSt();
                });
            } else {
                MeshLogger.log("publication err: " + statusMessage.getStatus());
            }


        }
    }

    public class SensorItem {
        public int propertyId;
        public SensorDescriptor descriptor;
        public SensorCadence cadence;
    }

}
