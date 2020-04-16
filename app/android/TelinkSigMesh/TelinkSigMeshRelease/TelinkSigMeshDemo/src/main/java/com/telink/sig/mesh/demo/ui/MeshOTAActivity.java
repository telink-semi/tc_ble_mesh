/********************************************************************************************************
 * @file MeshOTAActivity.java
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
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.demo.TelinkMeshApplication;
import com.telink.sig.mesh.demo.model.Mesh;
import com.telink.sig.mesh.demo.ui.adapter.BaseSelectableListAdapter;
import com.telink.sig.mesh.demo.ui.adapter.MeshOTADeviceSelectAdapter;
import com.telink.sig.mesh.demo.ui.file.FileSelectActivity;
import com.telink.sig.mesh.event.CommandEvent;
import com.telink.sig.mesh.event.Event;
import com.telink.sig.mesh.event.EventListener;
import com.telink.sig.mesh.event.MeshEvent;
import com.telink.sig.mesh.event.MeshOtaApplyStatusEvent;
import com.telink.sig.mesh.event.MeshOtaEvent;
import com.telink.sig.mesh.event.MeshOtaProgressEvent;
import com.telink.sig.mesh.event.NotificationEvent;
import com.telink.sig.mesh.event.OnlineStatusEvent;
import com.telink.sig.mesh.light.MeshService;
import com.telink.sig.mesh.light.Opcode;
import com.telink.sig.mesh.model.DeviceInfo;
import com.telink.sig.mesh.model.MeshCommand;
import com.telink.sig.mesh.model.NotificationInfo;
import com.telink.sig.mesh.util.Arrays;
import com.telink.sig.mesh.util.TelinkLog;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

/**
 * 场景配置
 * Created by kee on 2018/9/18.
 */
public class MeshOTAActivity extends BaseActivity implements View.OnClickListener, BaseSelectableListAdapter.SelectStatusChangedListener, EventListener<String> {

    private MeshOTADeviceSelectAdapter mDeviceAdapter;
    private Mesh mesh;
    private List<DeviceInfo> devices;
    private Map<Integer, String> versions;
    private Set<Integer> completeDevices;
    private CheckBox cb_device;
    private Button btn_start;
    private ProgressBar pb_mesh_ota;

    private Handler delayHandler = new Handler();
    private static final int REQUEST_CODE_GET_FILE = 1;
    private TextView tv_file_path, tv_version_info,
            tv_progress, tv_info;
    private byte[] mFirmware;

    private int progress = 0;
    private boolean isComplete = false;

    private static final int MSG_INFO = 0;
    private static final int MSG_PROGRESS = 1;

    private int[] selectedNodes;

    private Handler infoHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            if (msg.what == MSG_INFO) {
                tv_info.setText(msg.obj.toString());
            } else if (msg.what == MSG_PROGRESS) {
                tv_progress.setText("" + progress);
                pb_mesh_ota.setProgress(progress);
            }

        }
    };


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_mesh_ota);
        mesh = TelinkMeshApplication.getInstance().getMesh();
        setTitle("Mesh OTA");
        enableBackNav(true);
        cb_device = findViewById(R.id.cb_device);
        RecyclerView rv_device = findViewById(R.id.rv_device);

        rv_device.setLayoutManager(new LinearLayoutManager(this));
        if (mesh.devices != null) {
            for (DeviceInfo deviceInfo : mesh.devices) {
                deviceInfo.selected = false;
            }
        }
        devices = mesh.devices;

        versions = new HashMap<>();
        mDeviceAdapter = new MeshOTADeviceSelectAdapter(this, devices, versions);
        mDeviceAdapter.setStatusChangedListener(this);
        rv_device.setAdapter(mDeviceAdapter);
        cb_device.setChecked(mDeviceAdapter.allSelected());
        btn_start = findViewById(R.id.btn_start);
        btn_start.setOnClickListener(this);
        findViewById(R.id.btn_get_version).setOnClickListener(this);
        cb_device.setOnClickListener(this);
        tv_version_info = findViewById(R.id.tv_version_info);
        tv_file_path = findViewById(R.id.tv_file_path);
        tv_file_path.setOnClickListener(this);
        tv_progress = findViewById(R.id.tv_progress);
        tv_info = findViewById(R.id.tv_info);
        pb_mesh_ota = findViewById(R.id.pb_mesh_ota);
        TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_DEVICE_ON_OFF_STATUS, this);
        TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_CTL_STATUS_NOTIFY, this);
        TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_LIGHTNESS_STATUS_NOTIFY, this);
        TelinkMeshApplication.getInstance().addEventListener(OnlineStatusEvent.ONLINE_STATUS_NOTIFY, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshOtaEvent.EVENT_TYPE_PROGRESS_UPDATE, this);

        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_DISCONNECTED, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_AUTO_CONNECT_LOGIN, this);

        TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_MESH_OTA_FIRMWARE_UPDATE_STATUS, this);
        TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_MESH_OTA_FIRMWARE_DISTRIBUTION_STATUS, this);
        TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_MESH_OTA_FIRMWARE_INFO_STATUS, this);

        TelinkMeshApplication.getInstance().addEventListener(CommandEvent.EVENT_TYPE_CMD_COMPLETE, this);

        TelinkMeshApplication.getInstance().addEventListener(MeshOtaEvent.EVENT_TYPE_APPLY_STATUS, this);

    }

    @Override
    public void finish() {
        super.finish();
        if (!isComplete) {
            TelinkMeshApplication.getInstance().getMeshLib().pauseMeshOta();
            MeshService.getInstance().stopMeshOTA(DIST_STOP_TAG);
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        isComplete = true;
        delayHandler.removeCallbacksAndMessages(null);
        TelinkMeshApplication.getInstance().removeEventListener(this);

    }


    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_start:
                if (mFirmware == null) {
                    toastMsg("Pls select file");
                    return;
                }
                List<Integer> nodeAddresses = getSelectedNodes();
                if (nodeAddresses == null) {
                    toastMsg("Pls select at least ONE device");
                    return;
                }
                int[] nodes = new int[nodeAddresses.size()];
                for (int i = 0; i < nodes.length; i++) {
                    nodes[i] = nodeAddresses.get(i);
                }
                this.selectedNodes = nodes;
                if (MeshService.getInstance().startMeshOTA(selectedNodes, mFirmware)) {
                    delayHandler.removeCallbacks(distributeStartTimeoutTask);
                    delayHandler.postDelayed(distributeStartTimeoutTask, 1000);


                    MeshService.getInstance().resetConnectedMeshAdr(getCurMeshAdr());
                    // waiting for cmd complete
                    btn_start.setEnabled(false);
                    mDeviceAdapter.setStarted(true);
                    mDeviceAdapter.notifyDataSetChanged();
                    cb_device.setEnabled(false);
                    infoHandler.obtainMessage(0, "Start Distribution").sendToTarget();
                }
                break;

            case R.id.btn_get_version:
                MeshCommand meshCommand = MeshCommand.newInstance(mesh.netKeyIndex, mesh.appKeyIndex,
                        0, 0xFFFF, Opcode.FW_INFO_GET.getValue());
                if (MeshService.getInstance().sendMeshCommand(meshCommand)) {
                    versions.clear();
                    mDeviceAdapter.notifyDataSetChanged();
                } else {
                    toastMsg("get firmware fail");
                }
                break;

            case R.id.cb_device:
                mDeviceAdapter.setAll(!mDeviceAdapter.allSelected());
                break;
            case R.id.tv_file_path:
                startActivityForResult(new Intent(this, FileSelectActivity.class).putExtra(FileSelectActivity.FILE_SUFFIX, ".bin"), REQUEST_CODE_GET_FILE);
                break;
        }
    }


    public List<Integer> getSelectedNodes() {
        List<Integer> nodes = null;
        for (DeviceInfo deviceInfo : mesh.devices) {
            if (deviceInfo.selected && deviceInfo.getOnOff() != -1) {
                if (nodes == null) {
                    nodes = new ArrayList<>();
                }
                nodes.add(deviceInfo.meshAddress);
            }
        }
        return nodes;
    }


    @Override
    public void onStatusChanged(BaseSelectableListAdapter adapter) {
        if (adapter == mDeviceAdapter) {
            cb_device.setChecked(mDeviceAdapter.allSelected());
        }
    }

    private static final String DIST_STOP_TAG = "dist_stop";
    private boolean distStopped = false;

    @Override
    public void performed(Event<String> event) {
        super.performed(event);
        switch (event.getType()) {
            case MeshEvent.EVENT_TYPE_DISCONNECTED:
                TelinkMeshApplication.getInstance().getMeshLib().pauseMeshOta();
                infoHandler.obtainMessage(0, "Device Disconnected").sendToTarget();
            case NotificationEvent.EVENT_TYPE_DEVICE_ON_OFF_STATUS:
            case NotificationEvent.EVENT_TYPE_CTL_STATUS_NOTIFY:
            case NotificationEvent.EVENT_TYPE_LIGHTNESS_STATUS_NOTIFY:
            case OnlineStatusEvent.ONLINE_STATUS_NOTIFY:
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        mDeviceAdapter.notifyDataSetChanged();
                    }
                });
                break;
            case MeshOtaEvent.EVENT_TYPE_PROGRESS_UPDATE:
                if (isComplete) break;
                progress = ((MeshOtaProgressEvent) event).getProgress();
                if (progress == 100) {
                    isComplete = true;
                    infoHandler.obtainMessage(MSG_INFO, "OTA Complete").sendToTarget();
                }
                infoHandler.obtainMessage(MSG_PROGRESS).sendToTarget();
                break;

            case MeshOtaEvent.EVENT_TYPE_APPLY_STATUS:
                byte[] status = ((MeshOtaApplyStatusEvent) event).getStatus();
                if (progress >= 99 && status != null && status.length > 0 && status[0] == 0) {
                    int applySrc = ((MeshOtaApplyStatusEvent) event).getSrc();
                    onDeviceOtaSuccess(applySrc);
                }
                break;

            /*case NotificationEvent.EVENT_TYPE_MESH_OTA_FIRMWARE_UPDATE_STATUS:
                NotificationInfo notificationInfo = ((NotificationEvent) event).getNotificationInfo();

                FirmwareUpdateStatusParser.FirmwareUpdateStatus status = FirmwareUpdateStatusParser.create().parse(notificationInfo.params);
                if (progress >= 99 && status != null && status.status == 0) {
                    onDeviceOtaSuccess(notificationInfo.srcAdr);
                }
                break;*/

            case NotificationEvent.EVENT_TYPE_MESH_OTA_FIRMWARE_DISTRIBUTION_STATUS:
                delayHandler.removeCallbacks(distributeStartTimeoutTask);

                NotificationInfo distNotification = ((NotificationEvent) event).getNotificationInfo();
                if (!isComplete && distNotification.params != null && distNotification.params.length > 0) {
                    int distSt = distNotification.params[0];
                    TelinkLog.d("distribute status: " + distSt);
                    if (distSt == 1) {
                        infoHandler.obtainMessage(MSG_INFO, "Mesh OTA processing...").sendToTarget();
                    } else {
                        if (distSt == 0) {
                            distStopped = true;
                            MeshService.getInstance().startMeshOTA(selectedNodes, mFirmware);
                        } else {
                            infoHandler.obtainMessage(MSG_INFO, "Distribution Err").sendToTarget();
                            if (!distStopped)
                                MeshService.getInstance().stopMeshOTA(DIST_STOP_TAG);
                        }

                    }
                }

                break;

            case NotificationEvent.EVENT_TYPE_MESH_OTA_FIRMWARE_INFO_STATUS:
                /*
                u16 cid，  (vendor id)
                u16 pid,   (设备类型)
                u16 vid    (版本id)
                 */
                final NotificationInfo fwInfoNotify = ((NotificationEvent) event).getNotificationInfo();
                if (fwInfoNotify.params.length < 6) {
                    return;
                }
                byte[] strArr = new byte[4];
                System.arraycopy(fwInfoNotify.params, 2, strArr, 0, 4);

                final String fwVer = new String(strArr) + "(" + Arrays.bytesToHexString(strArr, ":") + ")";

                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        versions.put(fwInfoNotify.srcAdr, fwVer);
                        mDeviceAdapter.notifyDataSetChanged();
                    }
                });

                break;

            case MeshEvent.EVENT_TYPE_AUTO_CONNECT_LOGIN:
                infoHandler.obtainMessage(MSG_INFO, "Device Login").sendToTarget();
                MeshService.getInstance().resetConnectedMeshAdr(getCurMeshAdr());
                if (!isComplete) {
                    TelinkMeshApplication.getInstance().getMeshLib().continueMeshOta();
                }
                break;

            case CommandEvent.EVENT_TYPE_CMD_COMPLETE:
                /*CommandEvent commandEvent = (CommandEvent) event;
                if (DIST_STOP_TAG.equals(commandEvent.getMeshCommand().tag)) {

                }*/
                break;
        }
    }

    private Runnable distributeStartTimeoutTask = new Runnable() {
        @Override
        public void run() {
            infoHandler.obtainMessage(MSG_INFO, "Distribute Start Timeout").sendToTarget();
        }
    };

    private void onDeviceOtaSuccess(int deviceAddress) {
        if (completeDevices == null) {
            completeDevices = new HashSet<Integer>();
        }
        completeDevices.add(deviceAddress);
        mDeviceAdapter.resetCompleteNodes(completeDevices);
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (resultCode != Activity.RESULT_OK || requestCode != REQUEST_CODE_GET_FILE)
            return;
        String mPath = data.getStringExtra("path");
        TelinkLog.d("select: " + mPath);
        readFirmware(mPath);
    }

    private void readFirmware(String fileName) {
        try {
            InputStream stream = new FileInputStream(fileName);
            int length = stream.available();
            mFirmware = new byte[length];
            stream.read(mFirmware);
            stream.close();

            byte[] binPid = new byte[2];
            byte[] version = new byte[2];
            System.arraycopy(mFirmware, 2, binPid, 0, binPid.length);
            System.arraycopy(mFirmware, 4, version, 0, version.length);
            String pidInfo = Arrays.bytesToHexString(binPid, ":");
            String firmVersion = Arrays.bytesToHexString(version, ":");
            tv_version_info.setText(getString(R.string.version, pidInfo, firmVersion));
            tv_file_path.setText(fileName);
        } catch (IOException e) {
            e.printStackTrace();
            mFirmware = null;
            tv_version_info.setText(getString(R.string.version, "--", "--"));
            tv_file_path.setText("file error");
        }
    }

    private int getCurMeshAdr() {
        return mesh.getDeviceByMacAddress(MeshService.getInstance().getCurDeviceMac()).meshAddress;
    }
}
