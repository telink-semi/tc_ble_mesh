/********************************************************************************************************
 * @file DeviceOtaActivity.java
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
import android.widget.ProgressBar;
import android.widget.TextView;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONException;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.entity.ConnectionFilter;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.GattOtaEvent;
import com.telink.ble.mesh.foundation.parameter.GattOtaParameters;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.FileSystem;
import com.telink.ble.mesh.util.MeshLogger;
import com.telink.ble.mesh.web.ResponseInfo;
import com.telink.ble.mesh.web.TelinkHttpClient;
import com.telink.ble.mesh.web.WebUtils;
import com.telink.ble.mesh.web.entity.MeshNode;
import com.telink.ble.mesh.web.entity.MeshProductVersionInfo;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteOrder;
import java.text.SimpleDateFormat;
import java.util.Date;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.FormBody;
import okhttp3.Response;

/**
 * device firmware update by gatt ota
 */
public class DeviceOtaActivity extends BaseActivity implements View.OnClickListener, EventListener<String> {
    private SimpleDateFormat dateFormat = new SimpleDateFormat("HH:mm:ss");
    public final static String URL_GET_LATEST_VERSION_INFO = TelinkHttpClient.URL_BASE + "mesh-node/getLatestVersionInfo";

    public final static String URL_UPDATE_NODE_VERSION = TelinkHttpClient.URL_BASE + "mesh-node/updateNodeVersion";

    public final static String URL_DOWNLOAD_BIN = TelinkHttpClient.URL_BASE + "mesh-node/downloadBin";

    private TextView tv_log, tv_progress, tv_info;
    private ProgressBar pb_ota;
    private Button btn_start_ota;
    private byte[] mFirmware;
    private MeshNode mNodeInfo;
    private int binVid;
    private final static int MSG_PROGRESS = 11;
    private final static int MSG_INFO = 12;

    // get from cloud
    private MeshProductVersionInfo latestVersion;

    private Handler mInfoHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            if (msg.what == MSG_PROGRESS) {
                tv_progress.setText(msg.obj + "%");
                pb_ota.setProgress((int) msg.obj);
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

        getVersionInfo();
    }


    private void appendLog(String info) {
        mInfoHandler.obtainMessage(MSG_INFO, dateFormat.format(new Date()) + " => " + info).sendToTarget();
    }


    private void initViews() {
        tv_info = findViewById(R.id.tv_info);
        tv_info.setText(String.format("Node Info\naddress: %04X \nUUID: %s", mNodeInfo.address, mNodeInfo.deviceUuid));
        tv_log = findViewById(R.id.log);
        tv_progress = findViewById(R.id.progress);
        pb_ota = findViewById(R.id.pb_ota);
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
                appendLog("start OTA");
                tv_progress.setText("");
                btn_start_ota.setEnabled(false);
                ConnectionFilter connectionFilter = new ConnectionFilter(ConnectionFilter.TYPE_MESH_ADDRESS, mNodeInfo.address);
                GattOtaParameters parameters = new GattOtaParameters(connectionFilter, mFirmware);
                MeshService.getInstance().startGattOta(parameters);
                break;
        }
    }

    private void onOtaComplete() {
        runOnUiThread(() -> {
            btn_start_ota.setEnabled(true);
            mNodeInfo.versionInfo = latestVersion;
            updateNodeVersion();
        });
    }

    /**
     * update vid to cloud
     */
    private void updateNodeVersion() {
        FormBody formBody = new FormBody.Builder()
                .add("nodeId", mNodeInfo.id + "")
                .add("vid", binVid + "")
                .build();
        TelinkHttpClient.getInstance().post(URL_UPDATE_NODE_VERSION, formBody, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                appendLog("update version onFailure");
                dismissIndeterminateLoading();
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                dismissIndeterminateLoading();
                ResponseInfo responseInfo = WebUtils.parseResponse(response, DeviceOtaActivity.this);
                if (responseInfo == null) {
                    appendLog("response parse error");
                    return;
                }
                try {

                    MeshProductVersionInfo versionInfo = JSON.parseObject(responseInfo.data, MeshProductVersionInfo.class);
                    if (versionInfo == null) {
                        appendLog("version info parse error in response");
                        return;
                    }
                } catch (JSONException e) {
                    e.getStackTrace();
                }
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

    private void parseFirmware(File file) {
        try {
            InputStream stream = new FileInputStream(file);
            int length = stream.available();
            mFirmware = new byte[length];
            stream.read(mFirmware);
            stream.close();

            byte[] pid = new byte[2];
            byte[] vid = new byte[2];
            System.arraycopy(mFirmware, 2, pid, 0, 2);
//            this.binPid = MeshUtils.bytes2Integer(pid, ByteOrder.LITTLE_ENDIAN);

            System.arraycopy(mFirmware, 4, vid, 0, 2);

            String pidInfo = Arrays.bytesToHexString(pid, ":");
            String vidInfo = Arrays.bytesToHexString(vid, ":");
            String firmVersion = " pid-" + pidInfo + " vid-" + vidInfo;
            appendLog(getString(R.string.version, firmVersion));
            enableStartButton(true);
            binVid = MeshUtils.bytes2Integer(vid, ByteOrder.BIG_ENDIAN);
            return;
        } catch (IOException e) {
            e.printStackTrace();
            mFirmware = null;
        }
        appendLog("parse firmware error");
    }

    private void getVersionInfo() {
        enableStartButton(false);
        appendLog("get version - start");
        showIndeterminateLoading();
        FormBody formBody = new FormBody.Builder()
                .add("productId", mNodeInfo.productId + "")
                .build();
        TelinkHttpClient.getInstance().post(URL_GET_LATEST_VERSION_INFO, formBody, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                appendLog("get version onFailure");
                dismissIndeterminateLoading();
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                dismissIndeterminateLoading();
                ResponseInfo responseInfo = WebUtils.parseResponse(response, DeviceOtaActivity.this);
                if (responseInfo == null) {
                    appendLog("response parse error");
                    return;
                }
                try {
                    MeshProductVersionInfo versionInfo = JSON.parseObject(responseInfo.data, MeshProductVersionInfo.class);
                    if (versionInfo == null) {
                        appendLog("version info parse error in response");
                        return;
                    }
                    latestVersion = versionInfo;
                    checkVersionState();
                } catch (JSONException e) {
                    e.getStackTrace();
                }
            }
        });
    }

    // compare version info
    private void checkVersionState() {
        int localVid = mNodeInfo.versionInfo.getVid();
        int remoteVid = latestVersion.getVid();
        appendLog("get version - success");
        appendLog(String.format("version compare - local=%04X cloud=%04X", localVid, remoteVid));
        if (remoteVid <= localVid) {
            appendLog("The device is the latest version");
        } else {
            appendLog(String.format("Find the new version available - 0x%04X - %s", latestVersion.getVid(), latestVersion.getName()));
            checkBinFileState();
        }
    }

    private void checkBinFileState() {
        appendLog("check bin file");
        int productInfoId = mNodeInfo.productInfo.getId();
        int vid = latestVersion.getVid();
        File file = FileSystem.getBinPath(productInfoId, vid);
        if (file.exists()) {
            appendLog("bin file exists - " + file.getAbsolutePath());
            parseFirmware(file);
        } else {
            downloadBin(productInfoId, vid);
        }
    }


    private void downloadBin(int productId, int vid) {
        appendLog("download bind - start");

        String url = URL_DOWNLOAD_BIN + "?path=" + latestVersion.getBinFilePath();
        TelinkHttpClient.getInstance().get(url, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                appendLog("download bin fail");
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                long totalLen = response.body().contentLength();
                appendLog("download doc response " + response.code() + " len-- " + totalLen);
                if (response.code() != 200) {
                    appendLog("response error - " + response.code());
                    return;
                }

                InputStream is = null;
                byte[] buf = new byte[2048];
                int len = 0;
                FileOutputStream fos = null;
                File file = null;
                try {
                    is = response.body().byteStream();
                    file = FileSystem.getBinDownloadingPath(productId, vid);
                    int progress = 0;
                    int rcvLen = 0;
                    File parent = file.getParentFile();
                    if (!parent.exists()) {
                        if (!parent.mkdirs()) {
                            appendLog("parent dir mkdirs error");
                            return;
                        }
                    }
                    if (!file.exists()) {
                        boolean re = file.createNewFile();
                        if (!re) {
                            appendLog("download bin error - create file error");
                        }
                    }
//                    appendLog("start save bin");
                    fos = new FileOutputStream(file);

                    while ((len = is.read(buf)) != -1) {
                        MeshLogger.d("write : " + len);
                        rcvLen += len;
                        progress = (int) Math.min(100, rcvLen * 100 / totalLen);
//                        mHandler.obtainMessage(MSG_DOC_CN_DOWNLOAD_PROGRESS, progress).sendToTarget();
//                        appendLog("download progress: " + progress);
                        fos.write(buf, 0, len);
                    }

                    fos.flush();
                    appendLog("download bin success");
                    File toFile = FileSystem.getBinPath(productId, vid);
                    boolean result = file.renameTo(toFile);
                    if (!result) {
                        appendLog("save bin error");
                    } else {
                        appendLog("save bin success");
                        parseFirmware(toFile);
                    }
                } catch (Exception e) {
                    appendLog("download error " + e.getMessage());
                    if (file != null) {
                        file.delete();
                    }
                } finally {
                    try {
                        if (is != null) is.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                    try {
                        if (fos != null) fos.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }
        });
    }

    private void enableStartButton(boolean enable) {
        runOnUiThread(() -> {
            if (enable) {
                btn_start_ota.setText("start");

            } else {
//                btn_start_ota.setText("");
            }
            btn_start_ota.setEnabled(enable);
        });
    }
}
