/********************************************************************************************************
 * @file DeviceSettingFragment.java
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
package com.telink.sig.mesh.demo.ui.fragment;

import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.support.v7.app.AlertDialog;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CheckBox;
import android.widget.TextView;

import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.demo.TelinkMeshApplication;
import com.telink.sig.mesh.demo.ui.DeviceOtaActivity;
import com.telink.sig.mesh.demo.ui.DeviceSettingActivity;
import com.telink.sig.mesh.demo.ui.LpnSettingActivity;
import com.telink.sig.mesh.demo.ui.ModelSettingActivity;
import com.telink.sig.mesh.demo.ui.SchedulerListActivity;
import com.telink.sig.mesh.event.Event;
import com.telink.sig.mesh.event.EventListener;
import com.telink.sig.mesh.event.MeshEvent;
import com.telink.sig.mesh.event.NotificationEvent;
import com.telink.sig.mesh.light.MeshService;
import com.telink.sig.mesh.light.PublicationStatusParser;
import com.telink.sig.mesh.model.DeviceInfo;
import com.telink.sig.mesh.model.NotificationInfo;
import com.telink.sig.mesh.model.PublishModel;
import com.telink.sig.mesh.model.SigMeshModel;
import com.telink.sig.mesh.model.message.config.PubSetMessage;
import com.telink.sig.mesh.util.TelinkLog;

/**
 * 设备配置fragment
 * Created by kee on 2018/10/10.
 */

public class DeviceSettingFragment extends BaseFragment implements View.OnClickListener, EventListener<String> {

    private AlertDialog confirmDialog;
    private boolean kickDirect;
    DeviceInfo deviceInfo;
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
        deviceInfo = TelinkMeshApplication.getInstance().getMesh().getDeviceByMeshAddress(address);
        initPubModel();
        TextView tv_mac = view.findViewById(R.id.tv_mac);
        tv_mac.setText(String.format("Mac: %s", deviceInfo.macAddress));
        view.findViewById(R.id.view_scheduler).setOnClickListener(this);
        cb_pub = view.findViewById(R.id.cb_pub);
        cb_relay = view.findViewById(R.id.cb_relay);
        cb_pub.setChecked(deviceInfo.isPubSet());
        cb_relay.setChecked(deviceInfo.isRelayEnable());
        view.findViewById(R.id.view_pub).setOnClickListener(this);
        view.findViewById(R.id.view_relay).setOnClickListener(this);
        view.findViewById(R.id.view_sub).setOnClickListener(this);
        view.findViewById(R.id.view_ota).setOnClickListener(this);
        view.findViewById(R.id.btn_kick).setOnClickListener(this);

        if (deviceInfo.nodeInfo.cpsData.lowPowerSupport()) {
            view.findViewById(R.id.view_lpn).setOnClickListener(this);
        } else {
            view.findViewById(R.id.view_lpn).setVisibility(View.GONE);
        }

        TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_DEVICE_ON_OFF_STATUS, this);
        TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_CTL_STATUS_NOTIFY, this);
        TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_KICK_OUT_CONFIRM, this);
        TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_PUBLICATION_STATUS, this);
        TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_RELAY_STATUS, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_DISCONNECTED, this);
    }

    /**
     * check available publish model
     * check device has ctl model, if not -> check hsl, if not -> check onoff
     */
    public void initPubModel() {

        final int pubInterval = PUB_INTERVAL;
        final int address = PUB_ADDRESS;
        int modelId = SigMeshModel.SIG_MD_LIGHT_CTL_S.modelId;
        int pubEleAdr = deviceInfo.getTargetEleAdr(modelId);
        String desc = null;

        if (pubEleAdr != -1) {
            pubModel = new PublishModel(pubEleAdr, modelId, address, pubInterval);
            desc = "CTL";
            tv_pub.setText(getString(R.string.publication_setting, String.format("%04X", pubEleAdr), desc));
            return;
        }

        modelId = SigMeshModel.SIG_MD_LIGHT_HSL_S.modelId;
        pubEleAdr = deviceInfo.getTargetEleAdr(modelId);
        if (pubEleAdr != -1) {
            pubModel = new PublishModel(pubEleAdr, modelId, address, pubInterval);
            desc = "HSL";
            tv_pub.setText(getString(R.string.publication_setting, String.format("%04X", pubEleAdr), desc));
            return;
        }

        modelId = SigMeshModel.SIG_MD_LIGHTNESS_S.modelId;
        pubEleAdr = deviceInfo.getTargetEleAdr(modelId);
        if (pubEleAdr != -1) {
            pubModel = new PublishModel(pubEleAdr, modelId, address, pubInterval);
            desc = "LIGHTNESS";
            tv_pub.setText(getString(R.string.publication_setting, String.format("%04X", pubEleAdr), desc));
            return;
        }

        modelId = SigMeshModel.SIG_MD_G_ONOFF_S.modelId;
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
//        if (MeshService.getInstance().kickOut(deviceInfo))
        kickDirect = deviceInfo.macAddress.equals(MeshService.getInstance().getCurDeviceMac());
        boolean kickSent = MeshService.getInstance().resetNode(deviceInfo.meshAddress, null);
        showWaitingDialog("kick out processing");
        if (!kickDirect || !kickSent) {
            delayHandler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    onKickOutFinish();
                }
            }, 3 * 1000);
        }
    }

    private void onKickOutFinish() {
        delayHandler.removeCallbacksAndMessages(null);
        MeshService.getInstance().removeNodeInfo(deviceInfo.meshAddress);
        TelinkMeshApplication.getInstance().getMesh().removeDeviceByMeshAddress(deviceInfo.meshAddress);
        TelinkMeshApplication.getInstance().getMesh().saveOrUpdate(getActivity().getApplicationContext());
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
        } else if (event.getType().equals(NotificationEvent.EVENT_TYPE_CTL_STATUS_NOTIFY)) {

        } else if (event.getType().equals(NotificationEvent.EVENT_TYPE_KICK_OUT_CONFIRM)) {
            if (!kickDirect) {
                onKickOutFinish();
            }
        } else if (event.getType().equals(NotificationEvent.EVENT_TYPE_PUBLICATION_STATUS)) {
            NotificationInfo notificationInfo = ((NotificationEvent) event).getNotificationInfo();
            byte[] params = notificationInfo.params;
            final PublicationStatusParser.StatusInfo statusInfo = PublicationStatusParser.create().parse(params);
            if (pubModel == null) return;
            if (notificationInfo.srcAdr == deviceInfo.meshAddress && statusInfo != null && statusInfo.status == 0 && statusInfo.elementAddress == pubModel.elementAddress) {
                getActivity().runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        delayHandler.removeCallbacks(cmdTimeoutTask);
                        dismissWaitingDialog();
                        cb_pub.setChecked(statusInfo.publishAddress != 0);
                        deviceInfo.setPublishModel(statusInfo.publishAddress != 0 ? pubModel : null);
                        TelinkMeshApplication.getInstance().getMesh().saveOrUpdate(getActivity());
                    }
                });
            }
        } else if (event.getType().equals(NotificationEvent.EVENT_TYPE_RELAY_STATUS)) {
            getActivity().runOnUiThread(new Runnable() {
                @Override
                public void run() {
//                    deviceInfo.set
                    delayHandler.removeCallbacks(cmdTimeoutTask);
                    dismissWaitingDialog();
                    deviceInfo.setRelayEnable(!deviceInfo.isRelayEnable());
                    cb_relay.setChecked(deviceInfo.isRelayEnable());
                    TelinkMeshApplication.getInstance().getMesh().saveOrUpdate(getActivity());
                }
            });
        }
    }


    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.view_ota:

                if (deviceInfo.macAddress == null || deviceInfo.macAddress.equals("")) {
                    toastMsg("device no record");
                    return;
                }
                Intent otaIntent = new Intent(getActivity(), DeviceOtaActivity.class);
                otaIntent.putExtra("meshAddress", deviceInfo.meshAddress);
                startActivity(otaIntent);
                getActivity().finish();
                break;

            case R.id.btn_kick:
                showKickConfirmDialog();
                break;

            case R.id.view_scheduler:
                if (deviceInfo.getTargetEleAdr(SigMeshModel.SIG_MD_SCHED_S.modelId) == -1) {
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
                if (deviceInfo.getOnOff() == -1 || pubModel == null) return;

//                byte[] params = PublicationDataGenerator.getSettingData(pubModel, TelinkMeshApplication.getInstance().getMesh().appKeyIndex);
//                if (params == null) {
//                    toastMsg("invalid period");
//                }

                int pubAdr = 0;
                if (cb_pub.isChecked()) {
                    TelinkLog.d("cancel publication");
                } else {
                    TelinkLog.d("set publication");
                    pubAdr = pubModel.address;
                }
//                boolean result = MeshService.getInstance().cfgCmdPubSet(deviceInfo.meshAddress, pubModel.elementAddress, pubAdr, pubModel.modelId, params, true);
                final int appKeyIndex = TelinkMeshApplication.getInstance().getMesh().appKeyIndex;
                PubSetMessage pubSetMessage = PubSetMessage.createDefault(pubModel.elementAddress,
                        pubAdr, appKeyIndex, pubModel.period, pubModel.modelId, true);
                boolean result = MeshService.getInstance().setPublication(deviceInfo.meshAddress, pubSetMessage, null);
                if (result) {

                    final int finalPubAdr = pubAdr;
                    delayHandler.postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            TelinkLog.d("get publication");
                            MeshService.getInstance().getPublication(deviceInfo.meshAddress, finalPubAdr, pubModel.modelId, true, null);
                        }
                    }, 10 * 1000);

                    showWaitingDialog("processing...");
                    delayHandler.removeCallbacks(cmdTimeoutTask);
                    delayHandler.postDelayed(cmdTimeoutTask, 5 * 1000);
                }
                break;

            case R.id.view_relay:
//                boolean relayResult = MeshService.getInstance().cfgCmdRelaySet(deviceInfo.meshAddress, deviceInfo.isRelayEnable() ? 0 : 1);
                boolean relayResult = MeshService.getInstance().setRelay(deviceInfo.meshAddress, deviceInfo.isRelayEnable() ? 0 : 1, null);
                if (relayResult) {
                    showWaitingDialog("processing...");
                    delayHandler.removeCallbacks(cmdTimeoutTask);
                    delayHandler.postDelayed(cmdTimeoutTask, 5 * 1000);
                }
                break;

            case R.id.view_lpn:
                intent = new Intent(getActivity(), LpnSettingActivity.class);
                intent.putExtra("deviceAddress", deviceInfo.meshAddress);
                startActivity(intent);
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
