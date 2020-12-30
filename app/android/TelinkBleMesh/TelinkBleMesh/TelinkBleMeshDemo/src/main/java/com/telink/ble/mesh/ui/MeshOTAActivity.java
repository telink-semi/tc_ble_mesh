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
package com.telink.ble.mesh.ui;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.MeshSigModel;
import com.telink.ble.mesh.core.message.NotificationMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.FirmwareUpdateInfoGetMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.FirmwareUpdateInfoStatusMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.entity.FirmwareUpdateConfiguration;
import com.telink.ble.mesh.entity.MeshUpdatingDevice;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.AutoConnectEvent;
import com.telink.ble.mesh.foundation.event.FirmwareUpdatingEvent;
import com.telink.ble.mesh.foundation.event.MeshEvent;
import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;
import com.telink.ble.mesh.foundation.parameter.MeshOtaParameters;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.NodeStatusChangedEvent;
import com.telink.ble.mesh.ui.adapter.BaseSelectableListAdapter;
import com.telink.ble.mesh.ui.adapter.MeshOTADeviceSelectAdapter;
import com.telink.ble.mesh.ui.file.FileSelectActivity;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

/**
 * batch firmware update by mesh
 * Created by kee on 2018/9/18.
 */
public class MeshOTAActivity extends BaseActivity implements View.OnClickListener, BaseSelectableListAdapter.SelectStatusChangedListener, EventListener<String> {

    /**
     * group mesh address used in mesh-OTA procedure
     */
    private static final int MESH_OTA_GROUP_ADDRESS = 0xC00F;

    /**
     * request code for file select
     */
    private static final int REQUEST_CODE_GET_FILE = 1;

    /**
     * message code : info
     */
    private static final int MSG_INFO = 0;

    /**
     * message code : progress update
     */
    private static final int MSG_PROGRESS = 1;

    /**
     * view adapter
     */
    private MeshOTADeviceSelectAdapter mDeviceAdapter;

    /**
     * local mesh info
     */
    private MeshInfo mesh;

    /**
     * local devices
     */
    private List<NodeInfo> devices;

    /**
     * device address & version key-value map
     */
    private Map<Integer, String> versions;

    /**
     * update complete devices
     */
    private Set<MeshUpdatingDevice> completeDevices;

    /**
     * UIView
     */
    private CheckBox cb_device;
    private Button btn_start, btn_get_version;
    private RecyclerView rv_device;
    private ProgressBar pb_mesh_ota;

    private TextView tv_file_path, tv_version_info,
            tv_progress, tv_info;

    /**
     * firmware info read from selected file
     */
    private byte[] firmwareData;

    /**
     * metadata, used in {@link com.telink.ble.mesh.core.message.firmwareupdate.FirmwareMetadataCheckMessage}
     * default valued by 2 bytes length PID , 2 bytes length VID and 4 bytes length custom data.
     * length NOT more than 8 is recommended
     */
    private byte[] metadata;

    /**
     * pid in firmware data
     */
    private int binPid;

    /**
     * mesh-OTA firmware updating progress
     * count begins when first chunk transfer sent {@link com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer.BlobChunkTransferMessage}
     */
    private int progress = 0;

    /**
     * is mesh-OTA complete
     */
    private boolean isComplete = false;


    /**
     * delay handler
     */
    private Handler delayHandler = new Handler();


    @SuppressLint("HandlerLeak")
    private Handler infoHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            if (msg.what == MSG_INFO) {
                // update info
                tv_info.setText(msg.obj.toString());
            } else if (msg.what == MSG_PROGRESS) {
                // update progress
                tv_progress.setText(String.valueOf(progress));
                pb_mesh_ota.setProgress(progress);
            }
        }
    };


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_mesh_ota);
        mesh = TelinkMeshApplication.getInstance().getMeshInfo();
        setTitle("Mesh OTA");
        enableBackNav(true);
        cb_device = findViewById(R.id.cb_device);
        rv_device = findViewById(R.id.rv_device);

        rv_device.setLayoutManager(new LinearLayoutManager(this));
        if (mesh.nodes != null) {
            for (NodeInfo deviceInfo : mesh.nodes) {
                deviceInfo.selected = false;
            }
        }
        devices = mesh.nodes;

        versions = new HashMap<>();
        mDeviceAdapter = new MeshOTADeviceSelectAdapter(this, devices, versions);
        mDeviceAdapter.setStatusChangedListener(this);
        rv_device.setAdapter(mDeviceAdapter);
        cb_device.setChecked(mDeviceAdapter.allSelected());
        btn_start = findViewById(R.id.btn_start);
        btn_start.setOnClickListener(this);
        btn_get_version = findViewById(R.id.btn_get_version);
        btn_get_version.setOnClickListener(this);
        cb_device.setOnClickListener(this);
        tv_version_info = findViewById(R.id.tv_version_info);
        tv_file_path = findViewById(R.id.tv_file_path);
        tv_file_path.setOnClickListener(this);
        tv_progress = findViewById(R.id.tv_progress);
        tv_info = findViewById(R.id.tv_info);
        pb_mesh_ota = findViewById(R.id.pb_mesh_ota);

        addEventListeners();
    }

    private void addEventListeners() {

        // firmware info
        TelinkMeshApplication.getInstance().addEventListener(FirmwareUpdateInfoStatusMessage.class.getName(), this);

        TelinkMeshApplication.getInstance().addEventListener(NodeStatusChangedEvent.EVENT_TYPE_NODE_STATUS_CHANGED, this);
        TelinkMeshApplication.getInstance().addEventListener(FirmwareUpdatingEvent.EVENT_TYPE_UPDATING_SUCCESS, this);
        TelinkMeshApplication.getInstance().addEventListener(FirmwareUpdatingEvent.EVENT_TYPE_UPDATING_FAIL, this);
        TelinkMeshApplication.getInstance().addEventListener(FirmwareUpdatingEvent.EVENT_TYPE_UPDATING_PROGRESS, this);
        TelinkMeshApplication.getInstance().addEventListener(FirmwareUpdatingEvent.EVENT_TYPE_UPDATING_STOPPED, this);
        TelinkMeshApplication.getInstance().addEventListener(FirmwareUpdatingEvent.EVENT_TYPE_DEVICE_SUCCESS, this);
        TelinkMeshApplication.getInstance().addEventListener(FirmwareUpdatingEvent.EVENT_TYPE_DEVICE_FAIL, this);
        TelinkMeshApplication.getInstance().addEventListener(FirmwareUpdatingEvent.EVENT_TYPE_UPDATING_PREPARED, this);
    }

    @Override
    public void finish() {
        super.finish();
        if (!isComplete) {
            MeshService.getInstance().stopMeshOta();
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
                if (firmwareData == null) {
                    toastMsg("Pls select file");
                    return;
                }
                List<NodeInfo> nodes = getSelectedNodes();
                if (nodes == null) {
                    toastMsg("Pls select at least ONE device");
                    return;
                }
                infoHandler.obtainMessage(MSG_INFO, "Mesh OTA preparing...").sendToTarget();
                enableUI(false);

                MeshInfo meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
                List<MeshUpdatingDevice> updatingDevices = new ArrayList<>();
                final int directAddress = MeshService.getInstance().getDirectConnectedNodeAddress();
                MeshUpdatingDevice device;
                MeshUpdatingDevice directDevice = null;
                for (NodeInfo node : nodes) {
//                    if (node.getOnOff() == NodeInfo.ON_OFF_STATE_OFFLINE || node.macAddress .endsWith("BB:CC:DD:81")) continue;
                    if (directAddress == node.meshAddress) {
                        directDevice = new MeshUpdatingDevice();
                        directDevice.setMeshAddress(node.meshAddress);
                        directDevice.setUpdatingEleAddress(node.getTargetEleAdr(MeshSigModel.SIG_MD_OBJ_TRANSFER_S.modelId));
                    } else {
                        device = new MeshUpdatingDevice();
                        device.setMeshAddress(node.meshAddress);
                        device.setUpdatingEleAddress(node.getTargetEleAdr(MeshSigModel.SIG_MD_OBJ_TRANSFER_S.modelId));
                        updatingDevices.add(device);
                    }
                }
                // put direct device to last
                if (directDevice != null) {
                    updatingDevices.add(directDevice);
                }

                this.metadata = new byte[8];
                System.arraycopy(this.firmwareData, 2, this.metadata, 0, 4);

                FirmwareUpdateConfiguration configuration = new FirmwareUpdateConfiguration(updatingDevices,
                        this.firmwareData, this.metadata,
                        meshInfo.getDefaultAppKeyIndex(), MESH_OTA_GROUP_ADDRESS);
                MeshOtaParameters meshOtaParameters = new MeshOtaParameters(configuration);
                MeshService.getInstance().startMeshOta(meshOtaParameters);

                break;

            case R.id.btn_get_version:
                meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
                FirmwareUpdateInfoGetMessage infoGetMessage = FirmwareUpdateInfoGetMessage.getSimple(0xFFFF,
                        meshInfo.getDefaultAppKeyIndex());
                infoGetMessage.setResponseMax(0);
                if (MeshService.getInstance().sendMeshMessage(infoGetMessage)) {
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
                startActivityForResult(new Intent(this, FileSelectActivity.class).putExtra(FileSelectActivity.KEY_SUFFIX, ".bin"), REQUEST_CODE_GET_FILE);
                break;
        }
    }


    public List<NodeInfo> getSelectedNodes() {
        List<NodeInfo> nodes = null;
        for (NodeInfo deviceInfo : mesh.nodes) {
            if (deviceInfo.selected && deviceInfo.getOnOff() != -1 && deviceInfo.getTargetEleAdr(MeshSigModel.SIG_MD_FW_UPDATE_S.modelId) != -1) {
                if (nodes == null) {
                    nodes = new ArrayList<>();
                }
                nodes.add(deviceInfo);
            }
        }
        return nodes;
    }

    private void enableUI(final boolean enable) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mDeviceAdapter.setStarted(!enable);
                cb_device.setEnabled(enable);
                btn_start.setEnabled(enable);
                btn_get_version.setEnabled(enable);
            }
        });
    }


    @Override
    public void onStatusChanged(BaseSelectableListAdapter adapter) {
        if (adapter == mDeviceAdapter) {
            cb_device.setChecked(mDeviceAdapter.allSelected());
        }
    }

    private void onMeshUpdatingEvent(FirmwareUpdatingEvent updatingEvent) {
        switch (updatingEvent.getType()) {
            case FirmwareUpdatingEvent.EVENT_TYPE_UPDATING_PROGRESS:
                if (isComplete) break;
                progress = updatingEvent.getProgress();
                infoHandler.obtainMessage(MSG_PROGRESS).sendToTarget();
                break;
            case FirmwareUpdatingEvent.EVENT_TYPE_UPDATING_SUCCESS:
                isComplete = true;
                infoHandler.obtainMessage(MSG_INFO, "Mesh OTA Complete").sendToTarget();
                break;

            case FirmwareUpdatingEvent.EVENT_TYPE_UPDATING_FAIL:
                isComplete = true;
                infoHandler.obtainMessage(MSG_INFO, "Mesh OTA Fail -- " + updatingEvent.getDesc()).sendToTarget();
                break;
            case FirmwareUpdatingEvent.EVENT_TYPE_UPDATING_STOPPED:
                isComplete = true;
                infoHandler.obtainMessage(MSG_INFO, "OTA Stopped").sendToTarget();
                break;

            case FirmwareUpdatingEvent.EVENT_TYPE_DEVICE_SUCCESS:
                onDeviceOtaSuccess(updatingEvent.getUpdatingDevice());
                break;

            case FirmwareUpdatingEvent.EVENT_TYPE_DEVICE_FAIL:
                onDeviceOtaFail(updatingEvent.getUpdatingDevice(), updatingEvent.getDesc());
                break;

            case FirmwareUpdatingEvent.EVENT_TYPE_UPDATING_PREPARED:
                infoHandler.obtainMessage(MSG_INFO, "Mesh OTA chunk sending...").sendToTarget();
                break;
        }
    }

    @Override
    public void performed(Event<String> event) {
        if (event instanceof FirmwareUpdatingEvent) {
            onMeshUpdatingEvent((FirmwareUpdatingEvent) event);
        } else {
            final String eventType = event.getType();
            if (eventType.equals(MeshEvent.EVENT_TYPE_DISCONNECTED)
                    || eventType.equals(NodeStatusChangedEvent.EVENT_TYPE_NODE_STATUS_CHANGED)) {
                if (eventType.equals(MeshEvent.EVENT_TYPE_DISCONNECTED)) {
                    infoHandler.obtainMessage(0, "Device Disconnected").sendToTarget();
                }
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        mDeviceAdapter.notifyDataSetChanged();
                    }
                });
            } else if (eventType.equals(FirmwareUpdateInfoStatusMessage.class.getName())) {
                final NotificationMessage notificationMessage = ((StatusNotificationEvent) event).getNotificationMessage();
                FirmwareUpdateInfoStatusMessage infoStatusMessage = (FirmwareUpdateInfoStatusMessage) notificationMessage.getStatusMessage();
                FirmwareUpdateInfoStatusMessage.FirmwareInformationEntry firstEntry = infoStatusMessage.getFirstEntry();
                if (firstEntry != null) {
                    byte[] firmwareId = firstEntry.currentFirmwareID;
                    final String fwVer = Arrays.bytesToHexString(firmwareId);
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            versions.put(notificationMessage.getSrc(), fwVer);
                            mDeviceAdapter.notifyDataSetChanged();
                        }
                    });
                }

            } else if (eventType.equals(AutoConnectEvent.EVENT_TYPE_AUTO_CONNECT_LOGIN)) {
                infoHandler.obtainMessage(MSG_INFO, "Device Login").sendToTarget();
            }
        }

    }

    private void onDeviceOtaFail(MeshUpdatingDevice updatingDevice, String desc) {
        if (completeDevices == null) {
            completeDevices = new HashSet<MeshUpdatingDevice>();
        }
        completeDevices.add(updatingDevice);
        mDeviceAdapter.resetCompleteNodes(completeDevices);
    }

    private void onDeviceOtaSuccess(MeshUpdatingDevice updatingDevice) {
        if (completeDevices == null) {
            completeDevices = new HashSet<MeshUpdatingDevice>();
        }
        completeDevices.add(updatingDevice);
        mDeviceAdapter.resetCompleteNodes(completeDevices);
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (resultCode != Activity.RESULT_OK || requestCode != REQUEST_CODE_GET_FILE)
            return;
        String mPath = data.getStringExtra(FileSelectActivity.KEY_RESULT);
        MeshLogger.log("select: " + mPath);
        readFirmware(mPath);
    }

    private void readFirmware(String fileName) {
        try {
            InputStream stream = new FileInputStream(fileName);
            int length = stream.available();
            firmwareData = new byte[length];
            stream.read(firmwareData);
            stream.close();

            byte[] pid = new byte[2];
            byte[] vid = new byte[2];
            System.arraycopy(firmwareData, 2, pid, 0, 2);
            this.binPid = MeshUtils.bytes2Integer(pid, ByteOrder.LITTLE_ENDIAN);
            System.arraycopy(firmwareData, 4, vid, 0, 2);

            String pidInfo = Arrays.bytesToHexString(pid, ":");
            String vidInfo = Arrays.bytesToHexString(vid, ":");
            String firmVersion = "pid-" + pidInfo + " vid-" + vidInfo;
            tv_version_info.setText(getString(R.string.version, firmVersion));
            tv_file_path.setText(fileName);
            mDeviceAdapter.selectPid(this.binPid);
        } catch (IOException e) {
            e.printStackTrace();
            firmwareData = null;
            tv_version_info.setText(getString(R.string.version, "null"));
            tv_file_path.setText("file error");
        }
    }

}
