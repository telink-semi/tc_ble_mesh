/********************************************************************************************************
 * @file DeviceOtaLocalActivity.java
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

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.TextView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.entity.ConnectionFilter;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.GattOtaEvent;
import com.telink.ble.mesh.foundation.parameter.GattOtaParameters;
import com.telink.ble.mesh.ui.file.FileSelectActivity;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;
import com.telink.ble.mesh.web.entity.MeshNode;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteOrder;

/**
 * select local firmware and OTA
 */
public class DeviceOtaLocalActivity extends BaseActivity implements View.OnClickListener, EventListener<String> {

//    private Button btn_back;

    private TextView tv_select_file, tv_log, tv_progress, tv_version_info, tv_info;
    private Button btn_start_ota;
    private CheckBox cb_update;
    private byte[] mFirmware;
    private MeshNode mNodeInfo;
    private int binPid;
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
        tv_info.setText(String.format("Node Info\naddress: %04X \nUUID: %s", mNodeInfo.address, mNodeInfo.deviceUuid));
        tv_select_file = findViewById(R.id.tv_select_file);
        tv_log = findViewById(R.id.log);
        tv_progress = findViewById(R.id.progress);
        tv_version_info = findViewById(R.id.tv_version_info);
        tv_version_info.setText(getString(R.string.version, "null"));
        if (mNodeInfo.compositionData() != null) {
            int vid = mNodeInfo.compositionData().vid;
            byte[] vb = MeshUtils.integer2Bytes(vid, 2, ByteOrder.BIG_ENDIAN);

            int pid = mNodeInfo.compositionData().pid;
            byte[] pb = MeshUtils.integer2Bytes(pid, 2, ByteOrder.LITTLE_ENDIAN);
            tv_info.append("\n");
            tv_info.append(getString(R.string.node_version, Arrays.bytesToHexString(pb, ":"), Arrays.bytesToHexString(vb, ":")));
        } else {
            tv_info.append("\n");
            tv_info.append(getString(R.string.node_version, "null", "null"));
        }

        btn_start_ota = findViewById(R.id.btn_start_ota);

        tv_select_file.setOnClickListener(this);
        btn_start_ota.setOnClickListener(this);
        cb_update = findViewById(R.id.cb_update);
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

                if (binPid != mNodeInfo.compositionData().pid && !cb_update.isChecked()) {
                    toastMsg("[force upgrade] not checked");
                    return;
                }

                tv_log.setText("start OTA");
                tv_progress.setText("");
                btn_start_ota.setEnabled(false);
                ConnectionFilter connectionFilter = new ConnectionFilter(ConnectionFilter.TYPE_MESH_ADDRESS, mNodeInfo.address);
                GattOtaParameters parameters = new GattOtaParameters(connectionFilter, mFirmware);
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

            byte[] pid = new byte[2];
            byte[] vid = new byte[2];
            System.arraycopy(mFirmware, 2, pid, 0, 2);
            this.binPid = MeshUtils.bytes2Integer(pid, ByteOrder.LITTLE_ENDIAN);

            System.arraycopy(mFirmware, 4, vid, 0, 2);

            String pidInfo = Arrays.bytesToHexString(pid, ":");
            String vidInfo = Arrays.bytesToHexString(vid, ":");
            String firmVersion = " pid-" + pidInfo + " vid-" + vidInfo;
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

        if (resultCode != RESULT_OK || requestCode != REQUEST_CODE_GET_FILE)
            return;

        String mPath = data.getStringExtra(FileSelectActivity.KEY_RESULT);
        MeshLogger.log("select: " + mPath);
//        File f = new File(mPath);
        readFirmware(mPath);
    }
}
