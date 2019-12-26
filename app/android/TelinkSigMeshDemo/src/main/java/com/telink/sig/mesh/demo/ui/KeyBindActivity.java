/********************************************************************************************************
 * @file KeyBindActivity.java
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
package com.telink.sig.mesh.demo.ui;

import android.content.DialogInterface;
import android.os.Bundle;
import android.os.Handler;
import android.view.View;
import android.widget.TextView;

import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.demo.TelinkMeshApplication;
import com.telink.sig.mesh.demo.model.Mesh;
import com.telink.sig.mesh.event.NotificationEvent;
import com.telink.sig.mesh.event.Event;
import com.telink.sig.mesh.event.EventListener;
import com.telink.sig.mesh.event.MeshEvent;
import com.telink.sig.mesh.light.parameter.KeyBindParameters;
import com.telink.sig.mesh.light.MeshService;
import com.telink.sig.mesh.model.DeviceBindState;
import com.telink.sig.mesh.model.DeviceInfo;

/**
 * single device bind key, for device which bind fail
 * Created by kee on 2018/6/5.
 */

public class KeyBindActivity extends BaseActivity implements View.OnClickListener, EventListener<String> {
    private DeviceInfo targetDevice;
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
        setContentView(R.layout.activity_key_bind);
        setTitle("Key Bind");
        enableBackNav(true);
        int address = getIntent().getIntExtra("deviceAddress", -1);
        targetDevice = TelinkMeshApplication.getInstance().getMesh().getDeviceByMeshAddress(address);
        tv_bind = findViewById(R.id.tv_bind);

        tv_kick = findViewById(R.id.tv_kick_out);

        tv_bind.setOnClickListener(this);
        tv_kick.setOnClickListener(this);
        tv_device_info = findViewById(R.id.tv_device_info);

        String info = "meshAddress:" + targetDevice.meshAddress
                + "\nmac:" + targetDevice.macAddress;
        tv_device_info.setText(info);
        tv_progress = findViewById(R.id.tv_progress);

        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_KEY_BIND_SUCCESS, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_KEY_BIND_FAIL, this);
//        TelinkMeshApplication.getInstance().addEventListener(ScanEvent.SCAN_TIMEOUT, this);
        TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_KICK_OUT_CONFIRM, this);
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
                TelinkMeshApplication.getInstance().getMesh().devices.remove(targetDevice);
                TelinkMeshApplication.getInstance().getMesh().saveOrUpdate(this);

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
        Mesh mesh = TelinkMeshApplication.getInstance().getMesh();
        KeyBindParameters parameters = KeyBindParameters.getDefault(targetDevice,
                mesh.appKey, mesh.appKeyIndex, mesh.netKeyIndex, false);

        MeshService.getInstance().startKeyBind(parameters);
        showWaitingDialog("binding...");
    }

    @Override
    public void performed(Event<String> event) {
        super.performed(event);
        if (event.getType().equals(MeshEvent.EVENT_TYPE_KEY_BIND_SUCCESS)) {

            onBindSuccess((MeshEvent) event);
        } else if (event.getType().equals(MeshEvent.EVENT_TYPE_KEY_BIND_FAIL)) {
            onBindFail();
        } else if (event.getType().equals(MeshEvent.EVENT_TYPE_DISCONNECTED)) {
            if (kickDirect) {
                onKickOutFinish();
                finish();
            }
        } else if (event.getType().equals(NotificationEvent.EVENT_TYPE_KICK_OUT_CONFIRM)) {
            if (!kickDirect) {
                onKickOutFinish();
            }
        }
    }

    private void kickOut() {
//        if (MeshService.getInstance().kickOut(targetDevice))
        MeshService.getInstance().resetNode(targetDevice.meshAddress, null);
        kickDirect = targetDevice.macAddress.equals(MeshService.getInstance().getCurDeviceMac());
        showWaitingDialog("kick out processing");
        if (!kickDirect) {
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
        TelinkMeshApplication.getInstance().getMesh().removeDeviceByMeshAddress(targetDevice.meshAddress);
        TelinkMeshApplication.getInstance().getMesh().saveOrUpdate(getApplicationContext());
        dismissWaitingDialog();
        finish();
    }

    private void onBindSuccess(MeshEvent event) {
        complete = true;
        DeviceInfo remote = event.getDeviceInfo();
        Mesh mesh = TelinkMeshApplication.getInstance().getMesh();
        DeviceInfo local = mesh.getDeviceByMacAddress(remote.macAddress);
        if (local == null) return;

        local.bindState = DeviceBindState.BOUND;
        local.boundModels = remote.boundModels;
        local.nodeInfo = remote.nodeInfo;
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
