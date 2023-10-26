/********************************************************************************************************
 * @file DeviceSettingFragment.java
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
package com.telink.ble.mesh.ui.fragment;

import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CheckBox;
import android.widget.TextView;

import androidx.appcompat.app.AlertDialog;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.message.MeshSigModel;
import com.telink.ble.mesh.core.message.config.ConfigStatus;
import com.telink.ble.mesh.core.message.config.ModelPublicationSetMessage;
import com.telink.ble.mesh.core.message.config.ModelPublicationStatusMessage;
import com.telink.ble.mesh.core.message.config.NodeResetMessage;
import com.telink.ble.mesh.core.message.config.NodeResetStatusMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.entity.ModelPublication;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.MeshEvent;
import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.PublishModel;
import com.telink.ble.mesh.ui.CompositionDataActivity;
import com.telink.ble.mesh.ui.DeviceConfigActivity;
import com.telink.ble.mesh.ui.DeviceOtaActivity;
import com.telink.ble.mesh.ui.ModelSettingActivity;
import com.telink.ble.mesh.ui.NodeNetKeyActivity;
import com.telink.ble.mesh.ui.SchedulerListActivity;
import com.telink.ble.mesh.ui.SubnetBridgeActivity;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;

/**
 * device settings
 * Created by kee on 2018/10/10.
 */

public class DeviceSettingFragment extends BaseFragment implements View.OnClickListener, EventListener<String> {

    private AlertDialog confirmDialog;
    private boolean kickDirect;
    private NodeInfo deviceInfo;
    private Handler delayHandler = new Handler();
    private CheckBox cb_pub, cb_relay;
    private PublishModel pubModel;
    private TextView tv_pub;
    private static final int PUB_INTERVAL = 20 * 1000;

    private static final int PUB_ADDRESS = 0xFFFF;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_device_setting, null);
    }

    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        int address = getArguments().getInt("address");
        tv_pub = view.findViewById(R.id.tv_pub);
        deviceInfo = TelinkMeshApplication.getInstance().getMeshInfo().getDeviceByMeshAddress(address);
        initPubModel();
        TextView tv_mac = view.findViewById(R.id.tv_mac);
        tv_mac.setText("UUID: " + Arrays.bytesToHexString(deviceInfo.deviceUUID));
        view.findViewById(R.id.view_scheduler).setOnClickListener(this);
        cb_pub = view.findViewById(R.id.cb_pub);
        cb_relay = view.findViewById(R.id.cb_relay);
        cb_pub.setChecked(deviceInfo.isPubSet());
        cb_relay.setChecked(deviceInfo.relayEnable);
        view.findViewById(R.id.view_cps).setOnClickListener(this);
        view.findViewById(R.id.view_pub).setOnClickListener(this);
        view.findViewById(R.id.view_config).setOnClickListener(this);
        view.findViewById(R.id.view_sub).setOnClickListener(this);
        view.findViewById(R.id.view_ota).setOnClickListener(this);
        view.findViewById(R.id.view_net_key).setOnClickListener(this);
        view.findViewById(R.id.btn_kick).setOnClickListener(this);
        view.findViewById(R.id.view_subnet).setOnClickListener(this);


        TelinkMeshApplication.getInstance().addEventListener(ModelPublicationStatusMessage.class.getName(), this);
        TelinkMeshApplication.getInstance().addEventListener(NodeResetStatusMessage.class.getName(), this);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_DISCONNECTED, this);
    }

    /**
     * check available publish model
     * check device has ctl model, if not -> check hsl, if not -> check onoff
     */
    public void initPubModel() {

        final int pubInterval = PUB_INTERVAL;
        final int address = PUB_ADDRESS;
//        int address = TelinkMeshApplication.getInstance().getMeshInfo().localAddress;
        int modelId = MeshSigModel.SIG_MD_LIGHT_CTL_S.modelId;
        int pubEleAdr = deviceInfo.getTargetEleAdr(modelId);
        String desc = null;

        if (pubEleAdr != -1) {
            pubModel = new PublishModel(pubEleAdr, modelId, address, pubInterval);
            desc = "CTL";
            tv_pub.setText(getString(R.string.publication_setting, String.format("%04X", pubEleAdr), desc));
            return;
        }

        modelId = MeshSigModel.SIG_MD_LIGHT_HSL_S.modelId;
        pubEleAdr = deviceInfo.getTargetEleAdr(modelId);
        if (pubEleAdr != -1) {
            pubModel = new PublishModel(pubEleAdr, modelId, address, pubInterval);
            desc = "HSL";
            tv_pub.setText(getString(R.string.publication_setting, String.format("%04X", pubEleAdr), desc));
            return;
        }

        modelId = MeshSigModel.SIG_MD_LIGHTNESS_S.modelId;
        pubEleAdr = deviceInfo.getTargetEleAdr(modelId);
        if (pubEleAdr != -1) {
            pubModel = new PublishModel(pubEleAdr, modelId, address, pubInterval);
            desc = "LIGHTNESS";
            tv_pub.setText(getString(R.string.publication_setting, String.format("%04X", pubEleAdr), desc));
            return;
        }

        modelId = MeshSigModel.SIG_MD_G_ONOFF_S.modelId;
        pubEleAdr = deviceInfo.getTargetEleAdr(modelId);
        if (pubEleAdr != -1) {
            pubModel = new PublishModel(pubEleAdr, modelId, address, pubInterval);
            desc = "ONOFF";
            tv_pub.setText(getString(R.string.publication_setting, String.format("%04X", pubEleAdr), desc));
            return;
        }

        tv_pub.setText("Publication (no available model)");


    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        TelinkMeshApplication.getInstance().removeEventListener(this);
        delayHandler.removeCallbacksAndMessages(null);
    }

    private void showKickConfirmDialog() {
        if (confirmDialog == null) {
            AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
            builder.setCancelable(true);
            builder.setTitle("Warn");
            builder.setMessage("Confirm to remove device?");
            builder.setPositiveButton("Confirm", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    kickOut();
                }
            });

            builder.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    dialog.dismiss();
                }
            });
            confirmDialog = builder.create();
        }
        confirmDialog.show();
    }

    private void kickOut() {
        // send reset message
        MeshService.getInstance().sendMeshMessage(new NodeResetMessage(deviceInfo.meshAddress));
        kickDirect = deviceInfo.meshAddress == MeshService.getInstance().getDirectConnectedNodeAddress();
        showWaitingDialog("kick out processing");
        if (!kickDirect) {
            delayHandler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    onKickOutFinish();
                }
            }, 3 * 1000);
        } else {
            delayHandler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    onKickOutFinish();
                }
            }, 10 * 1000);
        }
    }

    private void onKickOutFinish() {
        delayHandler.removeCallbacksAndMessages(null);
        MeshService.getInstance().removeDevice(deviceInfo.meshAddress);
        TelinkMeshApplication.getInstance().getMeshInfo().removeNode(deviceInfo);
//        TelinkMeshApplication.getInstance().getMeshInfo().saveOrUpdate(getActivity().getApplicationContext());
        dismissWaitingDialog();
        getActivity().finish();
    }

    @Override
    public void performed(Event<String> event) {
        if (event.getType().equals(MeshEvent.EVENT_TYPE_DISCONNECTED)) {
            if (kickDirect) {
                onKickOutFinish();
            } else {
//                refreshUI();
            }
        } else if (event.getType().equals(ModelPublicationStatusMessage.class.getName())) {
            final ModelPublicationStatusMessage statusMessage = (ModelPublicationStatusMessage) ((StatusNotificationEvent) event).getNotificationMessage().getStatusMessage();
            if (statusMessage.getStatus() == ConfigStatus.SUCCESS.code) {
                getActivity().runOnUiThread(() -> {
                    delayHandler.removeCallbacks(cmdTimeoutTask);
                    dismissWaitingDialog();
                    boolean settle = statusMessage.getPublication().publishAddress != 0;
                    cb_pub.setChecked(settle);
                    deviceInfo.setPublishModel(settle ? pubModel : null);
                    deviceInfo.save();
//                    TelinkMeshApplication.getInstance().getMeshInfo().saveOrUpdate(getActivity());
                });

            } else {
                MeshLogger.log("publication err: " + statusMessage.getStatus());
            }


        } else if (event.getType().equals(NodeResetStatusMessage.class.getName())) {
            if (!kickDirect) {
                onKickOutFinish();
            }
        }
    }


    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.view_ota:
                Intent otaIntent = new Intent(getActivity(), DeviceOtaActivity.class);
                otaIntent.putExtra("meshAddress", deviceInfo.meshAddress);
                startActivity(otaIntent);
                getActivity().finish();
                break;

            case R.id.btn_kick:
                showKickConfirmDialog();
                break;

            case R.id.view_cps:
                Intent cpsIntent = new Intent(getActivity(), CompositionDataActivity.class);
                cpsIntent.putExtra("meshAddress", deviceInfo.meshAddress);
                startActivity(cpsIntent);
                break;

            case R.id.view_scheduler:
                if (deviceInfo.getTargetEleAdr(MeshSigModel.SIG_MD_SCHED_S.modelId) == -1) {
                    toastMsg("scheduler model not found");
                    return;
                }
                Intent schedulerIntent = new Intent(getActivity(), SchedulerListActivity.class);
                schedulerIntent.putExtra("address", deviceInfo.meshAddress);
                startActivity(schedulerIntent);
                break;

            case R.id.view_sub:
                Intent intent = new Intent(getActivity(), ModelSettingActivity.class);
                intent.putExtra("mode", 1);
                startActivity(intent);
                break;

            case R.id.view_pub:
                if (deviceInfo.isOffline() || pubModel == null) return;

                int pubAdr = 0;
                if (cb_pub.isChecked()) {
                    MeshLogger.log("cancel publication");
                } else {
                    MeshLogger.log("set publication");
                    pubAdr = pubModel.address;
                }
                int appKeyIndex = TelinkMeshApplication.getInstance().getMeshInfo().getDefaultAppKeyIndex();
                int modelId = pubModel.modelId;
                int eleAdr = deviceInfo.getTargetEleAdr(modelId);
                ModelPublication modelPublication = ModelPublication.createDefault(eleAdr, pubAdr, appKeyIndex, pubModel.period, pubModel.modelId, true);
                ModelPublicationSetMessage publicationSetMessage = new ModelPublicationSetMessage(deviceInfo.meshAddress, modelPublication);
                boolean result = MeshService.getInstance().sendMeshMessage(publicationSetMessage);
                if (result) {
                    showWaitingDialog("processing...");
                    delayHandler.removeCallbacks(cmdTimeoutTask);
                    delayHandler.postDelayed(cmdTimeoutTask, 5 * 1000);
                }
                break;

            case R.id.view_config:
                startActivity(new Intent(getActivity(), DeviceConfigActivity.class).putExtra("meshAddress", deviceInfo.meshAddress));
                break;

            case R.id.view_net_key:
                startActivity(new Intent(getActivity(), NodeNetKeyActivity.class)
                        .putExtra("meshAddress", deviceInfo.meshAddress));
                break;

            case R.id.view_subnet:
                startActivity(new Intent(getActivity(), SubnetBridgeActivity.class)
                        .putExtra("meshAddress", deviceInfo.meshAddress));
                break;
        }
    }

    private Runnable cmdTimeoutTask = new Runnable() {
        @Override
        public void run() {
            toastMsg("pub timeout");
            dismissWaitingDialog();
        }
    };

}
