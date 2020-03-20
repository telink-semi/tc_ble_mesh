package com.telink.ble.mesh.ui;

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
import com.telink.ble.mesh.core.message.updating.FirmwareInfoGetMessage;
import com.telink.ble.mesh.core.message.updating.FirmwareInfoStatusMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.entity.MeshUpdatingConfiguration;
import com.telink.ble.mesh.entity.MeshUpdatingDevice;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.AutoConnectEvent;
import com.telink.ble.mesh.foundation.event.MeshEvent;
import com.telink.ble.mesh.foundation.event.MeshUpdatingEvent;
import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;
import com.telink.ble.mesh.foundation.parameter.MeshOtaParameters;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.NodeStatusChangedEvent;
import com.telink.ble.mesh.ui.adapter.BaseSelectableListAdapter;
import com.telink.ble.mesh.ui.adapter.MeshOTADeviceSelectAdapter;
import com.telink.ble.mesh.util.TelinkLog;
import com.telink.file.selector.FileSelectActivity;

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
 * MeshOta in AutoConnect mode
 * Created by kee on 2018/9/18.
 */
//
public class MeshOTAActivity extends BaseActivity implements View.OnClickListener, BaseSelectableListAdapter.SelectStatusChangedListener, EventListener<String> {
    private MeshOTADeviceSelectAdapter mDeviceAdapter;
    private MeshInfo mesh;
    private List<NodeInfo> devices;
    private Map<Integer, String> versions;
    private Set<MeshUpdatingDevice> completeDevices;
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

    private List<NodeInfo> selectedNodes;

    private static final String DIST_STOP_TAG = "dist_stop";
    private boolean distStopped = false;

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
        mesh = TelinkMeshApplication.getInstance().getMeshInfo();
        setTitle("Mesh OTA");
        enableBackNav(true);
        cb_device = findViewById(R.id.cb_device);
        RecyclerView rv_device = findViewById(R.id.rv_device);

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
        findViewById(R.id.btn_get_version).setOnClickListener(this);
        cb_device.setOnClickListener(this);
        tv_version_info = findViewById(R.id.tv_version_info);
        tv_file_path = findViewById(R.id.tv_file_path);
        tv_file_path.setOnClickListener(this);
        tv_progress = findViewById(R.id.tv_progress);
        tv_info = findViewById(R.id.tv_info);
        pb_mesh_ota = findViewById(R.id.pb_mesh_ota);


        TelinkMeshApplication.getInstance().addEventListener(NodeStatusChangedEvent.EVENT_TYPE_NODE_STATUS_CHANGED, this);
        TelinkMeshApplication.getInstance().addEventListener(FirmwareInfoStatusMessage.class.getName(), this);

        TelinkMeshApplication.getInstance().addEventListener(MeshUpdatingEvent.EVENT_TYPE_UPDATING_SUCCESS, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshUpdatingEvent.EVENT_TYPE_UPDATING_FAIL, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshUpdatingEvent.EVENT_TYPE_UPDATING_PROGRESS, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshUpdatingEvent.EVENT_TYPE_UPDATING_STOPPED, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshUpdatingEvent.EVENT_TYPE_DEVICE_SUCCESS, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshUpdatingEvent.EVENT_TYPE_DEVICE_FAIL, this);

//        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_DISCONNECTED, this);
//        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_AUTO_CONNECT_LOGIN, this);


//        TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_MESH_OTA_FIRMWARE_UPDATE_STATUS, this);
//        TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_MESH_OTA_FIRMWARE_DISTRIBUTION_STATUS, this);
//        TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_MESH_OTA_FIRMWARE_INFO_STATUS, this);
//
//        TelinkMeshApplication.getInstance().addEventListener(CommandEvent.EVENT_TYPE_CMD_COMPLETE, this);
//
//        TelinkMeshApplication.getInstance().addEventListener(MeshOtaEvent.EVENT_TYPE_APPLY_STATUS, this);

    }

    @Override
    public void finish() {
        super.finish();
        if (!isComplete) {
            MeshService.getInstance().stopMeshOta();
//            TelinkMeshApplication.getInstance().getMeshLib().pauseMeshOta();
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
                List<NodeInfo> nodes = getSelectedNodes();
                if (nodes == null) {
                    toastMsg("Pls select at least ONE device");
                    return;
                }
                this.selectedNodes = nodes;

                MeshInfo meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
                List<MeshUpdatingDevice> updatingDevices = new ArrayList<>();
                MeshUpdatingDevice device;
                for (NodeInfo node : nodes) {
//                    if (node.getOnOff() == NodeInfo.ON_OFF_STATE_OFFLINE || node.macAddress .endsWith("BB:CC:DD:81")) continue;
                    device = new MeshUpdatingDevice();
                    device.setMeshAddress(node.meshAddress);
                    device.setUpdatingEleAddress(node.getTargetEleAdr(MeshSigModel.SIG_MD_OBJ_TRANSFER_S.modelId));
                    updatingDevices.add(device);
                }
                MeshUpdatingConfiguration configuration = new MeshUpdatingConfiguration(updatingDevices, mFirmware,
                        meshInfo.getDefaultAppKeyIndex(), 0xC00F);
                MeshOtaParameters meshOtaParameters = new MeshOtaParameters(configuration);
                MeshService.getInstance().startMeshOta(meshOtaParameters);
                break;

            case R.id.btn_get_version:
                meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
                FirmwareInfoGetMessage infoGetMessage = FirmwareInfoGetMessage.getSimple(0xFFFF,
                        meshInfo.getDefaultAppKeyIndex(), 0);
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
            if (deviceInfo.selected && deviceInfo.getOnOff() != -1) {
                if (nodes == null) {
                    nodes = new ArrayList<>();
                }
                nodes.add(deviceInfo);
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

    private void onMeshUpdatingEvent(MeshUpdatingEvent updatingEvent) {
        switch (updatingEvent.getType()) {
            case MeshUpdatingEvent.EVENT_TYPE_UPDATING_PROGRESS:

                if (isComplete) break;
                progress = updatingEvent.getProgress();
                if (progress == 100) {

                }
                infoHandler.obtainMessage(MSG_PROGRESS).sendToTarget();
                break;
            case MeshUpdatingEvent.EVENT_TYPE_UPDATING_SUCCESS:
                isComplete = true;
                infoHandler.obtainMessage(MSG_INFO, "OTA Complete").sendToTarget();
                break;

            case MeshUpdatingEvent.EVENT_TYPE_UPDATING_FAIL:
                isComplete = true;
                infoHandler.obtainMessage(MSG_INFO, "OTA Fail -- " + updatingEvent.getDesc()).sendToTarget();
                break;
            case MeshUpdatingEvent.EVENT_TYPE_UPDATING_STOPPED:
                isComplete = true;
                infoHandler.obtainMessage(MSG_INFO, "OTA Stopped").sendToTarget();
                break;

            case MeshUpdatingEvent.EVENT_TYPE_DEVICE_SUCCESS:
                onDeviceOtaSuccess(updatingEvent.getUpdatingDevice());
                break;

            case MeshUpdatingEvent.EVENT_TYPE_DEVICE_FAIL:
                onDeviceOtaFail(updatingEvent.getUpdatingDevice(), updatingEvent.getDesc());
                break;
        }
    }

    @Override
    public void performed(Event<String> event) {
        if (event instanceof MeshUpdatingEvent) {
            onMeshUpdatingEvent((MeshUpdatingEvent) event);
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
            } else if (eventType.equals(FirmwareInfoStatusMessage.class.getName())) {
                final NotificationMessage notificationMessage = ((StatusNotificationEvent) event).getNotificationMessage();
                FirmwareInfoStatusMessage infoStatusMessage = (FirmwareInfoStatusMessage) notificationMessage.getStatusMessage();
                final String fwVer = new String(MeshUtils.integer2Bytes(infoStatusMessage.getFirmwareId(), 4, ByteOrder.LITTLE_ENDIAN));
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        versions.put(notificationMessage.getSrc(), fwVer);
                        mDeviceAdapter.notifyDataSetChanged();
                    }
                });
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

            byte[] version = new byte[4];
            System.arraycopy(mFirmware, 2, version, 0, 4);
            String firmVersion = new String(version);
            tv_version_info.setText(getString(R.string.version, firmVersion));
            tv_file_path.setText(fileName);
        } catch (IOException e) {
            e.printStackTrace();
            mFirmware = null;
            tv_version_info.setText(getString(R.string.version, "null"));
            tv_file_path.setText("file error");
        }
    }

    private int getCurMeshAdr() {
        return mesh.getDeviceByMacAddress(MeshService.getInstance().getCurDeviceMac()).meshAddress;
    }
}
