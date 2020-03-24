/********************************************************************************************************
 * @file DeviceOtaActivity.java
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

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.demo.TelinkMeshApplication;
import com.telink.sig.mesh.demo.ui.file.FileSelectActivity;
import com.telink.sig.mesh.event.Event;
import com.telink.sig.mesh.event.EventListener;
import com.telink.sig.mesh.event.MeshEvent;
import com.telink.sig.mesh.event.OtaEvent;
import com.telink.sig.mesh.event.ScanEvent;
import com.telink.sig.mesh.light.MeshService;
import com.telink.sig.mesh.model.DeviceInfo;
import com.telink.sig.mesh.model.OtaDeviceInfo;
import com.telink.sig.mesh.util.Arrays;
import com.telink.sig.mesh.util.MeshUtils;
import com.telink.sig.mesh.util.TelinkLog;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteOrder;

/**
 *
 */

public class DeviceOtaActivity extends BaseActivity implements View.OnClickListener, EventListener<String> {

//    private Button btn_back;

    private TextView selectFile, info, progress, tv_version_info, tv_device_version;
    private Button startOta;

    //    private String mPath;
    private byte[] mFirmware;
    private DeviceInfo mDeviceInfo;

    private boolean otaComplete = true;

    private final static int REQUEST_CODE_GET_FILE = 1;
    private final static int MSG_PROGRESS = 11;
    private final static int MSG_INFO = 12;
    private byte[] binPid;
    private byte[] binVid;

    private Handler mInfoHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            if (msg.what == MSG_PROGRESS) {
                progress.setText(msg.obj + "%");
            } else if (msg.what == MSG_INFO) {
                info.append("\n" + msg.obj);
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_device_ota);
        setTitle("OTA");
        enableBackNav(true);
        Intent intent = getIntent();
        if (intent.hasExtra("meshAddress")) {
            int meshAddress = intent.getIntExtra("meshAddress", 0);
            mDeviceInfo = TelinkMeshApplication.getInstance().getMesh().getDeviceByMeshAddress(meshAddress);
        } else {
            toastMsg("device error");
            finish();
            return;
        }
        initViews();
        TelinkMeshApplication.getInstance().addEventListener(OtaEvent.EVENT_TYPE_OTA_SUCCESS, this);
        TelinkMeshApplication.getInstance().addEventListener(OtaEvent.EVENT_TYPE_OTA_PROGRESS, this);
        TelinkMeshApplication.getInstance().addEventListener(OtaEvent.EVENT_TYPE_OTA_FAIL, this);
        TelinkMeshApplication.getInstance().addEventListener(ScanEvent.SCAN_TIMEOUT, this);

        MeshService.getInstance().idle(false);
    }


    private void initViews() {
        selectFile = (TextView) findViewById(R.id.selectFile);
        info = (TextView) findViewById(R.id.log);
        progress = (TextView) findViewById(R.id.progress);
        tv_version_info = (TextView) findViewById(R.id.tv_version_info);
        tv_version_info.setText(getString(R.string.version, "--", "--"));
        tv_device_version = findViewById(R.id.tv_device_version);
        if (mDeviceInfo.nodeInfo != null) {
            int vid = mDeviceInfo.nodeInfo.cpsData.vid;
            byte[] vb = MeshUtils.integer2Bytes(vid, 2, ByteOrder.LITTLE_ENDIAN);

            int pid = mDeviceInfo.nodeInfo.cpsData.pid;
            byte[] pb = MeshUtils.integer2Bytes(pid, 2, ByteOrder.LITTLE_ENDIAN);
            tv_device_version.setText(getString(R.string.node_version, Arrays.bytesToHexString(pb, ":"), Arrays.bytesToHexString(vb, ":")));
        } else {
            tv_device_version.setText(getString(R.string.node_version, "--", "--"));
        }

        startOta = (Button) findViewById(R.id.startOta);

        selectFile.setOnClickListener(this);
        startOta.setOnClickListener(this);

    }

    @Override
    public void finish() {
        if (!otaComplete)
            MeshService.getInstance().idle(true);
        super.finish();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        TelinkMeshApplication.getInstance().removeEventListener(this);
    }


    @Override
    public void onClick(View v) {
        switch (v.getId()) {

            case R.id.startOta:

                if (mFirmware == null) {
//                    toastMsg("firmware error");
                    toastMsg("select firmware!");
                    return;
                }

                if (mDeviceInfo.nodeInfo != null) {
                    int pid = mDeviceInfo.nodeInfo.cpsData.pid;
                    byte[] pb = MeshUtils.integer2Bytes(pid, 2, ByteOrder.LITTLE_ENDIAN);

                    if (!Arrays.equals(binPid, pb)) {
                        toastMsg("pid not match!");
                        return;
                    }
                }

                info.setText("start OTA");

                progress.setText("");
                startOta.setEnabled(false);
                otaComplete = false;
                MeshService.getInstance().startOta(mDeviceInfo.macAddress, mFirmware);
//                mDevice.startOta(firmware);
                break;

            case R.id.selectFile:
                startActivityForResult(new Intent(this, FileSelectActivity.class).putExtra(FileSelectActivity.FILE_SUFFIX, ".bin"), REQUEST_CODE_GET_FILE);
                break;
        }
    }


    private void onOtaComplete() {
        otaComplete = true;
        if (mDeviceInfo.nodeInfo != null) {
            mDeviceInfo.nodeInfo.cpsData.vid = MeshUtils.bytes2Integer(binVid, ByteOrder.LITTLE_ENDIAN);
        }
        TelinkMeshApplication.getInstance().getMesh().saveOrUpdate(this);
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                startOta.setEnabled(true);
            }
        });
    }

    @Override
    public void performed(final Event<String> event) {
        super.performed(event);
        switch (event.getType()) {
            case OtaEvent.EVENT_TYPE_OTA_SUCCESS:
                MeshService.getInstance().idle(false);
                mInfoHandler.obtainMessage(MSG_INFO, "OTA_SUCCESS").sendToTarget();
                onOtaComplete();
                break;

            case OtaEvent.EVENT_TYPE_OTA_FAIL:
                MeshService.getInstance().idle(true);
                mInfoHandler.obtainMessage(MSG_INFO, "OTA_FAIL").sendToTarget();
                onOtaComplete();
                break;


            case OtaEvent.EVENT_TYPE_OTA_PROGRESS:
                OtaDeviceInfo deviceInfo = ((OtaEvent) event).getDeviceInfo();
                mInfoHandler.obtainMessage(MSG_PROGRESS, deviceInfo.progress).sendToTarget();
                break;


            case ScanEvent.SCAN_TIMEOUT:
                mInfoHandler.obtainMessage(MSG_INFO, "SCAN TIMEOUT").sendToTarget();
                MeshService.getInstance().idle(true);
                onOtaComplete();
                break;

            case MeshEvent.EVENT_TYPE_DISCONNECTED:
                mInfoHandler.obtainMessage(MSG_INFO, "DISCONNECTED").sendToTarget();
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

            binPid = new byte[2];
            binVid = new byte[2];
            System.arraycopy(mFirmware, 2, binPid, 0, binPid.length);
            System.arraycopy(mFirmware, 4, binVid, 0, binVid.length);
            String pidInfo = Arrays.bytesToHexString(binPid, ":");
            String firmVersion = Arrays.bytesToHexString(binVid, ":");
            tv_version_info.setText(getString(R.string.version, pidInfo, firmVersion));
            selectFile.setText(fileName);
        } catch (IOException e) {
            e.printStackTrace();
            mFirmware = null;
            tv_version_info.setText(getString(R.string.version, "--", "--"));
            selectFile.setText("file error");
        }
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (resultCode != Activity.RESULT_OK || requestCode != REQUEST_CODE_GET_FILE)
            return;

        String mPath = data.getStringExtra("path");
        TelinkLog.d("select: " + mPath);
//        File f = new File(mPath);
        readFirmware(mPath);
    }
}
