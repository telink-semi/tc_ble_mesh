/********************************************************************************************************
 * @file     DeviceOtaActivity.java 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
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
package com.telink.ble.mesh.ui;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.GattOtaEvent;
import com.telink.ble.mesh.foundation.parameter.GattOtaParameters;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.ui.file.FileSelectActivity;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteOrder;

/**
 *
 */

public class DeviceOtaActivity extends BaseActivity implements View.OnClickListener, EventListener<String> {

//    private Button btn_back;

    private TextView tv_select_file, tv_log, tv_progress, tv_version_info, tv_info;
    private Button btn_start_ota;

    private byte[] mFirmware;
    private NodeInfo mNodeInfo;

    private final static int REQUEST_CODE_GET_FILE = 1;
    private final static int MSG_PROGRESS = 11;
    private final static int MSG_INFO = 12;


    private Handler mInfoHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            if (msg.what == MSG_PROGRESS) {
                tv_progress.setText(msg.obj + "%");
            } else if (msg.what == MSG_INFO) {
                tv_log.append("\n" + msg.obj);
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_device_ota);
        setTitle("OTA");
        enableBackNav(true);

        Intent intent = getIntent();
        if (intent.hasExtra("meshAddress")) {
            int meshAddress = intent.getIntExtra("meshAddress", -1);
            mNodeInfo = TelinkMeshApplication.getInstance().getMeshInfo().getDeviceByMeshAddress(meshAddress);
        } else {
            toastMsg("device error");
            finish();
            return;
        }
        initViews();
        TelinkMeshApplication.getInstance().addEventListener(GattOtaEvent.EVENT_TYPE_OTA_SUCCESS, this);
        TelinkMeshApplication.getInstance().addEventListener(GattOtaEvent.EVENT_TYPE_OTA_PROGRESS, this);
        TelinkMeshApplication.getInstance().addEventListener(GattOtaEvent.EVENT_TYPE_OTA_FAIL, this);

        MeshService.getInstance().idle(false);
    }


    private void initViews() {
        tv_info = findViewById(R.id.tv_info);
        tv_info.setText(String.format("Node Info\naddress: %04X \nUUID: %s", mNodeInfo.meshAddress, Arrays.bytesToHexString(mNodeInfo.deviceUUID)));
        tv_select_file = findViewById(R.id.tv_select_file);
        tv_log = findViewById(R.id.log);
        tv_progress = findViewById(R.id.progress);
        tv_version_info = findViewById(R.id.tv_version_info);
        tv_version_info.setText(getString(R.string.version, "null"));
        if (mNodeInfo.compositionData != null) {
            int vid = mNodeInfo.compositionData.vid;
            byte[] vb = MeshUtils.integer2Bytes(vid, 2, ByteOrder.LITTLE_ENDIAN);

            int pid = mNodeInfo.compositionData.pid;
            byte[] pb = MeshUtils.integer2Bytes(pid, 2, ByteOrder.LITTLE_ENDIAN);
            tv_info.append("\n");
            tv_info.append(getString(R.string.node_version, Arrays.bytesToHexString(pb, ":"), Arrays.bytesToHexString(vb, ":")));
        } else {
            tv_info.append("\n");
            tv_info.append(getString(R.string.node_version, "--", "--"));
        }

        btn_start_ota = findViewById(R.id.btn_start_ota);

        tv_select_file.setOnClickListener(this);
        btn_start_ota.setOnClickListener(this);

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        TelinkMeshApplication.getInstance().removeEventListener(this);
    }


    @Override
    public void onClick(View v) {
        switch (v.getId()) {

            case R.id.btn_start_ota:

                if (mFirmware == null) {
                    toastMsg("select firmware!");
                    return;
                }

                tv_log.setText("start OTA");
                tv_progress.setText("");
                btn_start_ota.setEnabled(false);
                GattOtaParameters parameters = new GattOtaParameters(mNodeInfo.meshAddress, mFirmware);
                MeshService.getInstance().startGattOta(parameters);
                break;

            case R.id.tv_select_file:
                startActivityForResult(new Intent(this, FileSelectActivity.class).putExtra(FileSelectActivity.KEY_SUFFIX, ".bin"), REQUEST_CODE_GET_FILE);
                break;
        }
    }

    private void onOtaComplete() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                btn_start_ota.setEnabled(true);
            }
        });
    }

    @Override
    public void performed(final Event<String> event) {
        switch (event.getType()) {
            case GattOtaEvent.EVENT_TYPE_OTA_SUCCESS:
                MeshService.getInstance().idle(false);
                mInfoHandler.obtainMessage(MSG_INFO, "OTA_SUCCESS").sendToTarget();
                onOtaComplete();
                break;

            case GattOtaEvent.EVENT_TYPE_OTA_FAIL:
                MeshService.getInstance().idle(true);
                mInfoHandler.obtainMessage(MSG_INFO, "OTA_FAIL").sendToTarget();
                onOtaComplete();
                break;


            case GattOtaEvent.EVENT_TYPE_OTA_PROGRESS:
                int progress = ((GattOtaEvent) event).getProgress();
                mInfoHandler.obtainMessage(MSG_PROGRESS, progress).sendToTarget();
                break;
        }

    }

    private void readFirmware(String fileName) {
        try {
            InputStream stream = new FileInputStream(fileName);
            int length = stream.available();
            mFirmware = new byte[length];
            stream.read(mFirmware);
            stream.close();

            byte[] version = new byte[4];
            System.arraycopy(mFirmware, 2, version, 0, 4);
//            String firmVersion = new String(version);
            String firmVersion = Arrays.bytesToHexString(version, ":");
            tv_version_info.setText(getString(R.string.version, firmVersion));
            tv_select_file.setText(fileName);
        } catch (IOException e) {
            e.printStackTrace();
            mFirmware = null;
            tv_version_info.setText(getString(R.string.version, "null"));
            tv_select_file.setText("file error");
        }
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (resultCode != Activity.RESULT_OK || requestCode != REQUEST_CODE_GET_FILE)
            return;

        String mPath = data.getStringExtra(FileSelectActivity.KEY_RESULT);
        MeshLogger.log("select: " + mPath);
//        File f = new File(mPath);
        readFirmware(mPath);
    }
}
