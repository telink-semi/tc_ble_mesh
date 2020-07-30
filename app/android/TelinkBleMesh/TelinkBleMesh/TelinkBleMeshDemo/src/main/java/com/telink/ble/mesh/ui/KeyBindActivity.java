package com.telink.ble.mesh.ui;

import android.content.DialogInterface;
import android.os.Bundle;
import android.os.Handler;
import android.view.View;
import android.widget.TextView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.access.BindingBearer;
import com.telink.ble.mesh.core.message.config.NodeResetMessage;
import com.telink.ble.mesh.core.message.config.NodeResetStatusMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.entity.BindingDevice;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.BindingEvent;
import com.telink.ble.mesh.foundation.event.MeshEvent;
import com.telink.ble.mesh.foundation.parameter.BindingParameters;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.util.Arrays;

/**
 * single device bind key, for device which bind fail
 * Created by kee on 2018/6/5.
 */

public class KeyBindActivity extends BaseActivity implements View.OnClickListener, EventListener<String> {
    private NodeInfo targetDevice;
    private TextView tv_bind, tv_kick, tv_device_info, tv_progress;

    private Handler handler = new Handler();
    private static final int MAX_DOT_CNT = 4;
    private static String[] SUFFIX = new String[]{"", ".", "..", "..."};
    private String infoTxt = "running";
    private int dotState = 0;
    private boolean kickProcessing = false;
    private boolean complete = false;
    private boolean kickDirect;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_key_bind);
        setTitle("Key Bind");
        enableBackNav(true);
        int address = getIntent().getIntExtra("deviceAddress", -1);
        targetDevice = TelinkMeshApplication.getInstance().getMeshInfo().getDeviceByMeshAddress(address);
        tv_bind = findViewById(R.id.tv_bind);

        tv_kick = findViewById(R.id.tv_kick_out);

        tv_bind.setOnClickListener(this);
        tv_kick.setOnClickListener(this);
        tv_device_info = findViewById(R.id.tv_device_info);

        String info = "MeshAddress:" + targetDevice.meshAddress
                + "\nUUID:" + Arrays.bytesToHexString(targetDevice.deviceUUID, ":");
        tv_device_info.setText(info);
        tv_progress = findViewById(R.id.tv_progress);

        TelinkMeshApplication.getInstance().addEventListener(BindingEvent.EVENT_TYPE_BIND_SUCCESS, this);
        TelinkMeshApplication.getInstance().addEventListener(BindingEvent.EVENT_TYPE_BIND_FAIL, this);
        TelinkMeshApplication.getInstance().addEventListener(NodeResetStatusMessage.class.getName(), this);
//        TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_KICK_OUT_CONFIRM, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_DISCONNECTED, this);
        handler.post(dotTask);

    }

    private Runnable dotTask = new Runnable() {
        @Override
        public void run() {
            tv_progress.setText(infoTxt + SUFFIX[dotState % MAX_DOT_CNT]);
            dotState++;
            handler.postDelayed(this, 400);
        }
    };

    @Override
    protected void onDestroy() {
        super.onDestroy();
        TelinkMeshApplication.getInstance().removeEventListener(this);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.tv_bind:
                if (complete) {
                    finish();
                    return;
                }
                startKeyBind();
                break;

            case R.id.tv_kick_out:
                kickOut();
                /*MeshService.getInstance().kickOut(targetDevice);
                TelinkMeshApplication.getInstance().getMeshInfo().devices.remove(targetDevice);
                TelinkMeshApplication.getInstance().getMeshInfo().saveOrUpdate(this);

                showWaitingDialog("kick out processing");
                if (targetDevice.macAddress.equals(MeshService.getInstance().getCurDeviceMac())) {
                    kickProcessing = true;
                } else {
                    handler.postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            toastMsg("Kick out success");
                            dismissWaitingDialog();
                            finish();
                        }
                    }, 3 * 1000);
                }*/

                break;
        }

    }

    private void startKeyBind() {
        MeshInfo mesh = TelinkMeshApplication.getInstance().getMeshInfo();

        BindingDevice bindingDevice = new BindingDevice(targetDevice.meshAddress, targetDevice.deviceUUID,
                mesh.getDefaultAppKeyIndex());
//        KeyBindParameters parameters = KeyBindParameters.getDefault(targetDevice,
//                mesh.appKey, mesh.appKeyIndex, mesh.netKeyIndex, false);
        bindingDevice.setBearer(BindingBearer.Any);
        MeshService.getInstance().startBinding(new BindingParameters(bindingDevice));
        showWaitingDialog("binding...");
    }

    @Override
    public void performed(Event<String> event) {
        if (event.getType().equals(BindingEvent.EVENT_TYPE_BIND_SUCCESS)) {

            onBindSuccess((BindingEvent) event);
        } else if (event.getType().equals(BindingEvent.EVENT_TYPE_BIND_FAIL)) {
            onBindFail();
        } else if (event.getType().equals(MeshEvent.EVENT_TYPE_DISCONNECTED)) {
            if (kickDirect) {
                onKickOutFinish();
                finish();
            }
        } else if (event.getType().equals(NodeResetStatusMessage.class.getName())) {
            if (!kickDirect) {
                onKickOutFinish();
            }
        }
    }

    private void kickOut() {
        // send reset message
        boolean cmdSent = MeshService.getInstance().sendMeshMessage(new NodeResetMessage(targetDevice.meshAddress));
        kickDirect = targetDevice.meshAddress == (MeshService.getInstance().getDirectConnectedNodeAddress());
        showWaitingDialog("kick out processing");
        if (!cmdSent || !kickDirect) {
            handler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    onKickOutFinish();
                }
            }, 3 * 1000);
        }
    }

    private void onKickOutFinish() {
        handler.removeCallbacksAndMessages(null);
        MeshService.getInstance().removeDevice(targetDevice.meshAddress);
        TelinkMeshApplication.getInstance().getMeshInfo().removeDeviceByMeshAddress(targetDevice.meshAddress);
        TelinkMeshApplication.getInstance().getMeshInfo().saveOrUpdate(getApplicationContext());
        dismissWaitingDialog();
        finish();
    }

    private void onBindSuccess(BindingEvent event) {
        complete = true;
        BindingDevice remote = event.getBindingDevice();
        MeshInfo mesh = TelinkMeshApplication.getInstance().getMeshInfo();
        NodeInfo local = mesh.getDeviceByUUID(remote.getDeviceUUID());
        if (local == null) return;

        local.state = NodeInfo.STATE_BIND_SUCCESS;
//        local. = remote.boundModels;
        local.compositionData = remote.getCompositionData();
        mesh.saveOrUpdate(this);

        infoTxt = "bind success";
        dotState = 0;
        handler.removeCallbacks(dotTask);
        handler.post(dotTask);

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                tv_bind.setText("Back");
                dismissWaitingDialog();
                finish();
            }
        });
    }

    private void onBindFail() {
        infoTxt = "bind fail";
        dotState = 0;
        handler.removeCallbacks(dotTask);
        handler.post(dotTask);
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                dismissWaitingDialog();
                showConfirmDialog("Bind Fail, retry?", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        startKeyBind();
                    }
                });
            }
        });
    }
}
