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
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;

import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.google.android.material.bottomsheet.BottomSheetDialog;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.access.fu.DistributorType;
import com.telink.ble.mesh.core.access.fu.FUCallback;
import com.telink.ble.mesh.core.access.fu.FUState;
import com.telink.ble.mesh.core.access.fu.BlobTransferType;
import com.telink.ble.mesh.core.access.fu.UpdatePolicy;
import com.telink.ble.mesh.core.message.MeshSigModel;
import com.telink.ble.mesh.core.message.NotificationMessage;
import com.telink.ble.mesh.core.message.firmwaredistribution.FDReceiversListMessage;
import com.telink.ble.mesh.core.message.firmwaredistribution.FDUploadStatusMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.FirmwareUpdateInfoGetMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.FirmwareUpdateInfoStatusMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.FirmwareUpdateStatusMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.entity.FirmwareUpdateConfiguration;
import com.telink.ble.mesh.entity.MeshUpdatingDevice;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.AutoConnectEvent;
import com.telink.ble.mesh.foundation.event.MeshEvent;
import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;
import com.telink.ble.mesh.foundation.parameter.MeshOtaParameters;
import com.telink.ble.mesh.model.FUCache;
import com.telink.ble.mesh.model.FUCacheService;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.model.NodeStatusChangedEvent;
import com.telink.ble.mesh.ui.adapter.BaseSelectableListAdapter;
import com.telink.ble.mesh.ui.adapter.LogInfoAdapter;
import com.telink.ble.mesh.ui.adapter.FUDeviceSelectAdapter;
import com.telink.ble.mesh.ui.file.FileSelectActivity;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.LogInfo;
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

/**
 * batch firmware update by mesh
 * Created by kee on 2018/9/18.
 */
public class MeshOTAActivity extends BaseActivity implements View.OnClickListener,
        BaseSelectableListAdapter.SelectStatusChangedListener,
        EventListener<String>, FUCallback {

    /**
     * group mesh address used in mesh-OTA procedure
     */
    private static final int MESH_OTA_GROUP_ADDRESS = 0xC00F;

    /**
     * request code for file select
     */
    private static final int REQUEST_CODE_GET_FILE = 1;

    /**
     * request code for device select
     */
    private static final int REQUEST_CODE_SELECT_DEVICE = 2;


    /**
     * message code : info
     */
    private static final int MSG_INFO = 0;

    /**
     * message code : initiate progress update
     */
    private static final int MSG_INIT_PROGRESS = 1;

    /**
     * message code : distribute progress update
     */
    private static final int MSG_DIST_PROGRESS = 2;

    /**
     * view adapter
     */
    private FUDeviceSelectAdapter mDeviceAdapter;

    /**
     * local mesh info
     */
    private MeshInfo mesh;

    /**
     * local devices
     */
    private List<NodeInfo> devices;

    /**
     * updating devices
     */
    private List<MeshUpdatingDevice> updatingDevices;

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
    private RadioGroup rg_dist, rg_policy;
    private RadioButton rb_device, rb_phone, rb_plc_ver_apl, rb_plc_ver;
    private ProgressBar pb_init, pb_dist;
    private LinearLayout ll_policy;
    private TextView tv_file_path,
            tv_version_info, // version read from file
            tv_init_prg, // initiate progress
            tv_dist_prg, // distribute progress
            tv_info; // latest log info
    private BottomSheetDialog bottomDialog;
    private RecyclerView rv_log;

    /**
     * firmware info read from selected file
     */
    private byte[] firmwareData;

    private byte[] firmwareId;

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
     * checked distributor
     * direct connected device or phone
     */
//    private int checkedDist = DISTRIBUTOR_PHONE;

    private UpdatePolicy updatePolicy = UpdatePolicy.VerifyAndApply;

    /**
     * use phone as default
     */
    private DistributorType distributorType = DistributorType.PHONE;

    private int distAddress = 0;

    /**
     * mesh-OTA firmware updating progress
     * count begins when first chunk transfer sent {@link com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer.BlobChunkTransferMessage}
     */
//    private int progress = 0;

    /**
     * is mesh-OTA complete
     */
    private boolean isComplete = false;


    /**
     * delay handler
     */
    private Handler delayHandler = new Handler();

    private List<LogInfo> logInfoList = new ArrayList<>();

    private LogInfoAdapter logInfoAdapter;


    @SuppressLint("HandlerLeak")
    private Handler infoHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            if (msg.what == MSG_INFO) {
                // update info
                tv_info.setText(msg.obj.toString());
            } else if (msg.what == MSG_INIT_PROGRESS) {
                // update progress
                int progress = msg.arg1;
                tv_init_prg.setText(getString(R.string.init_progress, progress));
                pb_init.setProgress(progress);
            } else if (msg.what == MSG_DIST_PROGRESS) {
                int progress = msg.arg1;
                tv_dist_prg.setText(getString(R.string.dist_progress, progress));
                pb_dist.setProgress(progress);
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
        logInfoList.add(new LogInfo("NULL", "IDLE", MeshLogger.LEVEL_VERBOSE));
        logInfoAdapter = new LogInfoAdapter(this, logInfoList);
        initView();
        addEventListeners();
        initBottomSheetDialog();

        // test
//        readFirmware("/storage/emulated/0/kee/sigMesh/8258_mesh_test_OTA.bin");

        readFirmware("/storage/emulated/0/kee/sigMesh/20210422_mesh_OTA/8258_mesh_dist_apply0423.bin");
    }

    private void initView() {
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
//        mDeviceAdapter = new FUDeviceSelectAdapter(this, devices, versions);
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
        tv_dist_prg = findViewById(R.id.tv_dist_prg);
        tv_init_prg = findViewById(R.id.tv_init_prg);
        tv_info = findViewById(R.id.tv_info);
        tv_info.setOnClickListener(this);
        pb_dist = findViewById(R.id.pb_dist);
        pb_init = findViewById(R.id.pb_init);

        ll_policy = findViewById(R.id.ll_policy);
        rg_dist = findViewById(R.id.rg_dist);
        rb_device = findViewById(R.id.rb_device);
        rb_phone = findViewById(R.id.rb_phone);
        rb_plc_ver_apl = findViewById(R.id.rb_plc_ver_apl);
        rb_plc_ver = findViewById(R.id.rb_plc_ver);
        rg_policy = findViewById(R.id.rg_policy);
        rg_dist.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(RadioGroup group, int checkedId) {
                ll_policy.setVisibility(checkedId == R.id.rb_device ? View.VISIBLE : View.INVISIBLE);
                distributorType = checkedId == R.id.rb_device ? DistributorType.DEVICE : DistributorType.PHONE;
            }
        });
        rg_policy.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(RadioGroup group, int checkedId) {
                updatePolicy = checkedId == R.id.rb_plc_ver_apl ? UpdatePolicy.VerifyAndApply : UpdatePolicy.VerifyOnly;
            }
        });
    }

    private void addEventListeners() {

        // firmware info
        TelinkMeshApplication.getInstance().addEventListener(FirmwareUpdateInfoStatusMessage.class.getName(), this);
        TelinkMeshApplication.getInstance().addEventListener(FDReceiversListMessage.class.getName(), this);
        TelinkMeshApplication.getInstance().addEventListener(NodeStatusChangedEvent.EVENT_TYPE_NODE_STATUS_CHANGED, this);
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


    private void initBottomSheetDialog() {

        bottomDialog = new BottomSheetDialog(this);
        View view = getLayoutInflater().inflate(R.layout.dialog_bottom_list, null);
//        BottomSheetBehavior behavior = BottomSheetBehavior.from((View)dialog.getParent());
        MeshLogger.d("dialog view - " + view.toString());
        bottomDialog.setContentView(view);
        rv_log = view.findViewById(R.id.rv_log_sheet);
        rv_log.setLayoutManager(new LinearLayoutManager(this));
        rv_log.setAdapter(logInfoAdapter);
        view.findViewById(R.id.iv_close).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                bottomDialog.dismiss();
            }
        });
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.tv_info:
                bottomDialog.show();
                break;

            case R.id.btn_start:

                final int directAddress = MeshService.getInstance().getDirectConnectedNodeAddress();
                if (directAddress == 0) {
                    showTipDialog("No device connected\n , pls wait for device connected");
                    return;
                }
                if (firmwareData == null) {
                    toastMsg("Pls select file");
                    return;
                }

                // for test - start
//                firmwareData = new byte[4 * 1024];
//                for (int i = 0; i < firmwareData.length; i++) {
//                    firmwareData[i] = (byte) (i % 0x0F);
//                }
                // for test - end

                List<NodeInfo> nodes = getSelectedNodes();
                if (nodes == null) {
                    toastMsg("Pls select at least ONE device");
                    return;
                }

//                List<NodeInfo> nodes = mesh.nodes;  // for test


                if (distributorType == DistributorType.DEVICE) {
                    distAddress = directAddress;
                } else {
                    distAddress = 0;
                    FUCacheService.getInstance().clear(this);
                }


                infoHandler.obtainMessage(MSG_INFO, "Mesh OTA preparing...").sendToTarget();
                enableUI(false);

                MeshInfo meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
                updatingDevices = new ArrayList<>();
                MeshUpdatingDevice device;
                MeshUpdatingDevice directDevice = null;
                for (NodeInfo node : nodes) {
                    if (directAddress == node.meshAddress) {
                        directDevice = new MeshUpdatingDevice();
//                        directDevice.setMeshAddress(node.meshAddress);
//                        directDevice.setUpdatingEleAddress(node.getTargetEleAdr(MeshSigModel.SIG_MD_OBJ_TRANSFER_S.modelId));
                    } else {
                        device = new MeshUpdatingDevice();
//                        device.setMeshAddress(node.meshAddress);
//                        device.setUpdatingEleAddress(node.getTargetEleAdr(MeshSigModel.SIG_MD_OBJ_TRANSFER_S.modelId));
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
                configuration.setUpdatePolicy(updatePolicy);
                configuration.setDistributorType(distributorType);
                configuration.setDistributorAddress(distAddress);
                configuration.setCallback(this);
                configuration.setFirmwareId(firmwareId);

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
                rb_device.setEnabled(enable);
                rb_phone.setEnabled(enable);
                rb_plc_ver_apl.setEnabled(enable);
                rb_plc_ver.setEnabled(enable);
                tv_file_path.setEnabled(enable);
            }
        });
    }


    @Override
    public void onSelectStatusChanged(BaseSelectableListAdapter adapter) {
        if (adapter == mDeviceAdapter) {
            cb_device.setChecked(mDeviceAdapter.allSelected());
        }
    }

    /****************************************************************
     * events - start
     ****************************************************************/
    @Override
    public void performed(Event<String> event) {

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

        } else if (eventType.equals(FirmwareUpdateStatusMessage.class.getName())) {
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
        } else if (eventType.equals(FDUploadStatusMessage.class.getName())) {
            final NotificationMessage notificationMessage = ((StatusNotificationEvent) event).getNotificationMessage();
            FDUploadStatusMessage uploadStatusMessage = (FDUploadStatusMessage) notificationMessage.getStatusMessage();
            int uploadProgress = uploadStatusMessage.uploadProgress;
            if (DistributorType.DEVICE == distributorType) {
                onTransferProgress(uploadProgress, BlobTransferType.MESH);
            }
        }
    }


    /****************************************************************
     * events - end
     ****************************************************************/

    private void onDeviceOtaFail(MeshUpdatingDevice updatingDevice, String desc) {
        if (completeDevices == null) {
            completeDevices = new HashSet<MeshUpdatingDevice>();
        }
        completeDevices.add(updatingDevice);
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mDeviceAdapter.resetCompleteNodes(completeDevices);
            }
        });

    }

    private void onDeviceOtaSuccess(MeshUpdatingDevice updatingDevice) {
        if (completeDevices == null) {
            completeDevices = new HashSet<MeshUpdatingDevice>();
        }
        completeDevices.add(updatingDevice);
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mDeviceAdapter.resetCompleteNodes(completeDevices);
            }
        });

    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (resultCode != Activity.RESULT_OK)
            return;
        if (requestCode == REQUEST_CODE_GET_FILE) {
            String mPath = data.getStringExtra(FileSelectActivity.KEY_RESULT);
            MeshLogger.log("select: " + mPath);
            readFirmware(mPath);
        } else if (requestCode == REQUEST_CODE_SELECT_DEVICE) {
            ArrayList<MeshUpdatingDevice> selectedDevices = data.getParcelableArrayListExtra(FUDeviceSelectActivity.KEY_SELECTED_DEVICES);
            this.updatingDevices = selectedDevices;

        }

    }

    private void readFirmware(String fileName) {
        try {
            InputStream stream = new FileInputStream(fileName);
            int length = stream.available();
            firmwareData = new byte[length];
            stream.read(firmwareData);
            stream.close();

            firmwareId = new byte[4];
            System.arraycopy(firmwareData, 2, firmwareId, 0, 4);

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
            cb_device.setChecked(mDeviceAdapter.allSelected());
        } catch (IOException e) {
            e.printStackTrace();
            firmwareData = null;
            firmwareId = null;
            tv_version_info.setText(getString(R.string.version, "null"));
            tv_file_path.setText("file error");
        }
    }


    /****************************************************************
     * FUCallback start
     ****************************************************************/

    @Override
    public void onLog(String tag, String log, int logLevel) {
        MeshLogger.log(log, tag, logLevel);
        logInfoList.add(new LogInfo(tag, log, logLevel));
        if (bottomDialog.isShowing()) {
            logInfoAdapter.notifyDataSetChanged();
            rv_log.smoothScrollToPosition(logInfoList.size() - 1);
        }
    }

    @Override
    public void onStateUpdated(FUState state, String extraInfo) {
        infoHandler.obtainMessage(MSG_INFO, state.desc).sendToTarget();
        if (state == FUState.DISTRIBUTING_BY_DEVICE && distributorType == DistributorType.DEVICE) {


            FUCache fuCache = new FUCache();
            fuCache.distAddress = distAddress;
            fuCache.updatePolicy = updatePolicy;
            fuCache.updatingDeviceList = updatingDevices;
            FUCacheService.getInstance().save(this, fuCache);
            // If the direct connected device is selected as distributor, check distribution progress after initiate distributor success
            // switch to auto connect mode when remote distribution started
//            MeshLogger.d("switch to auto connect");
//            MeshService.getInstance().autoConnect(new AutoConnectParameters());
        }
    }

    @Override
    public void onDeviceStateUpdate(MeshUpdatingDevice device, String desc) {
        int state = device.state;
        if (state == MeshUpdatingDevice.STATE_SUCCESS) {
            onDeviceOtaSuccess(device);
        } else if (state == MeshUpdatingDevice.STATE_FAIL) {
            onDeviceOtaFail(device, desc);
        }
    }

    @Override
    public void onTransferProgress(int progress, BlobTransferType transferType) {
        onLog("null", "transfer progress update: " + progress + " type - " + transferType, MeshLogger.DEFAULT_LEVEL);
        if (transferType != BlobTransferType.MESH) {
            // to distributor
            infoHandler.obtainMessage(MSG_INIT_PROGRESS, progress, -1).sendToTarget();
        } else {
            // to updating nodes
            infoHandler.obtainMessage(MSG_DIST_PROGRESS, progress, -1).sendToTarget();
        }
    }

    /****************************************************************
     * FUCallback end
     ****************************************************************/

}
