/********************************************************************************************************
 * @file FUActivity.java
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
package com.telink.ble.mesh.ui;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.ProgressBar;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;

import androidx.appcompat.app.AlertDialog;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.google.android.material.bottomsheet.BottomSheetDialog;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.access.fu.BlobTransferType;
import com.telink.ble.mesh.core.access.fu.DistributorType;
import com.telink.ble.mesh.core.access.fu.FUCallback;
import com.telink.ble.mesh.core.access.fu.FUState;
import com.telink.ble.mesh.core.access.fu.UpdatePolicy;
import com.telink.ble.mesh.core.message.NotificationMessage;
import com.telink.ble.mesh.core.message.config.NetworkTransmitGetMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.AdditionalInformation;
import com.telink.ble.mesh.core.message.firmwareupdate.FirmwareUpdateInfoGetMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.FirmwareUpdateInfoStatusMessage;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.entity.FirmwareUpdateConfiguration;
import com.telink.ble.mesh.entity.MeshUpdatingDevice;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.MeshEvent;
import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;
import com.telink.ble.mesh.foundation.parameter.MeshOtaParameters;
import com.telink.ble.mesh.model.FUCache;
import com.telink.ble.mesh.model.FUCacheService;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.NodeInfo;
import com.telink.ble.mesh.ui.adapter.FUDeviceAdapter;
import com.telink.ble.mesh.ui.adapter.LogInfoAdapter;
import com.telink.ble.mesh.ui.file.FileSelectActivity;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.LogInfo;
import com.telink.ble.mesh.util.MeshLogger;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

/**
 * firmware update by mesh
 * Created by kee on 2018/9/18.
 */
public class FUActivity extends BaseActivity implements View.OnClickListener,
        EventListener<String>, FUCallback {

    public static final String KEY_FU_CONTINUE = "com.telink.ble.mesh.fu-continue";

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
    private FUDeviceAdapter deviceAdapter;

    /**
     * local mesh info
     */
    private MeshInfo mesh;

    /**
     * local devices
     */
//    private List<NodeInfo> devices;

    /**
     * updating devices
     */
    private List<MeshUpdatingDevice> updatingDevices;

    /**
     * UIView
     */
    private Button btn_start, btn_get_version;
    private RecyclerView rv_device;
    private RadioGroup rg_dist; // rg_policy
    private RadioButton rb_device, rb_phone, rb_plc_ver_apl, rb_plc_ver;
    private ProgressBar pb_init, pb_dist;
    private LinearLayout ll_policy;
    private TextView tv_select_device, tv_file_path,
            tv_version_info, // version read from file
            tv_init_prg, // initiate progress
            tv_dist_prg, // distribute progress
            tv_info; // latest log info
    private BottomSheetDialog bottomDialog;
    private RecyclerView rv_log;

    private AlertDialog.Builder exitWarningAlert;
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
    private boolean isComplete = true;


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
                String info = msg.obj.toString();
                tv_info.setText(info);

                // insert log
                appendLog(info);
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
        setContentView(R.layout.activity_fu);
        mesh = TelinkMeshApplication.getInstance().getMeshInfo();
        setTitle("Mesh OTA");
        enableBackNav(true);
        initView();
        addEventListeners();
        initBottomSheetDialog();

        checkIsContinue();
        getNetworkTransmit();
    }

    private void getNetworkTransmit() {
        int address = MeshService.getInstance().getDirectConnectedNodeAddress();
        if (address == 0) {
            MeshLogger.d("get transmit params error: mesh not connected");
            return;
        }
        MeshService.getInstance().sendMeshMessage(new NetworkTransmitGetMessage(address));
    }

    private void showWarningDialog() {
        if (exitWarningAlert == null) {
            exitWarningAlert = new AlertDialog.Builder(this);
            exitWarningAlert.setCancelable(true);
            exitWarningAlert.setTitle("Warning");
            exitWarningAlert.setMessage("Mesh OTA is still running, stop it ? \n click STOP to stop updating; \n click FORCE CLOSE to exit; click CANCEL to dismiss dialog");
            exitWarningAlert.setNegativeButton("Stop", (dialog, which) -> MeshService.getInstance().stopMeshOta());
            exitWarningAlert.setNeutralButton("FORCE CLOSE", (dialog, which) -> finish());
            exitWarningAlert.setPositiveButton("Cancel", (dialog, which) -> dialog.dismiss());
        }
        exitWarningAlert.show();
    }

    private void showCompleteDialog() {
        AlertDialog.Builder completeDialog = new AlertDialog.Builder(this);
        completeDialog.setCancelable(false);
        completeDialog.setTitle("Warning");
        int sucCnt = 0;
        for (MeshUpdatingDevice dev : updatingDevices) {
            if (dev.state == MeshUpdatingDevice.STATE_SUCCESS) {
                sucCnt += 1;
            }
        }
        int failCnt = updatingDevices.size() - sucCnt;
        completeDialog.setMessage(String.format(Locale.getDefault(), "Mesh OTA Complete \nsuccess: %d \nfail: %d", sucCnt, failCnt));
        completeDialog.setPositiveButton("OK", (dialog, which) -> dialog.dismiss());
        completeDialog.show();
    }

    private void checkIsContinue() {
        Intent intent = getIntent();
        boolean isContinue = intent.getBooleanExtra(KEY_FU_CONTINUE, false);
        if (isContinue) {
            continueFirmwareUpdate();
        } else {
            FUCache fuCache = FUCacheService.getInstance().get();
            if (fuCache != null) {
                enableUI(false);
                showContinueDialog();
            } else {
                startNewUpdating();
            }
        }
    }

    private void showContinueDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        DialogInterface.OnClickListener dialogBtnClick = (dialog, which) -> {
            if (which == DialogInterface.BUTTON_POSITIVE) {
                // continue
                continueFirmwareUpdate();
            } else if (which == DialogInterface.BUTTON_NEGATIVE) {
                // start new fu processing
                FUCacheService.getInstance().clear(FUActivity.this);
                startNewUpdating();
            }
        };

        builder.setTitle("Warning - previous firmware-update is still running")
                .setMessage("continue previous firmware-update processing?\n" +
                        "click CONTINUE to continue \n"
                        + "click NEW to clear cache and start new processing \n")
                .setPositiveButton("CONTINUE", dialogBtnClick)
                .setNegativeButton("NEW", dialogBtnClick);
        builder.show();
    }

    private void startNewUpdating() {
        enableUI(true);
        // for test
//        readFirmware("/storage/emulated/0/kee/sigMesh/8258_mesh_test_OTA.bin"); // little
        readFirmware("/storage/emulated/0/.a0kee/8258_mesh_LPN_noRebootWhenError.bin"); // lpn

//        readFirmware("/storage/emulated/0/kee/sigMesh/20210422_mesh_OTA/8258_mesh_V4.3_for_OTA.bin");


        infoHandler.obtainMessage(MSG_INFO, "IDLE").sendToTarget();
    }

    private void continueFirmwareUpdate() {
        MeshLogger.d("continue mesh OTA  --- ");
        enableUI(false);
        FUCache fuCache = FUCacheService.getInstance().get();
        if (fuCache == null) {
            infoHandler.obtainMessage(MSG_INFO, "firmware update cache error").sendToTarget();
        } else {
            MeshLogger.d("fuCache : " + fuCache.toString());
            this.distAddress = fuCache.distAddress;
            this.updatePolicy = fuCache.updatePolicy;
            this.updatingDevices = fuCache.updatingDeviceList;
            this.firmwareId = fuCache.firmwareId;

            this.distributorType = DistributorType.DEVICE;

            rb_device.setChecked(true);
            infoHandler.obtainMessage(MSG_INIT_PROGRESS, 100, -1).sendToTarget();
            rb_plc_ver_apl.setChecked(this.updatePolicy == UpdatePolicy.VerifyAndApply);
            rb_plc_ver.setChecked(this.updatePolicy == UpdatePolicy.VerifyOnly);
            deviceAdapter.resetDevices(this.updatingDevices);

            MeshInfo meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();

            FirmwareUpdateConfiguration configuration = new FirmwareUpdateConfiguration(getUpdatingDevices(),
                    this.firmwareData, this.metadata,
                    meshInfo.getDefaultAppKeyIndex(), MESH_OTA_GROUP_ADDRESS);
            configuration.setUpdatePolicy(updatePolicy);
            configuration.setDistributorType(distributorType);
            configuration.setDistributorAddress(distAddress);
            configuration.setCallback(this);
            configuration.setFirmwareId(firmwareId);
            configuration.setContinue(true);

            MeshOtaParameters meshOtaParameters = new MeshOtaParameters(configuration);
            isComplete = false;
            MeshService.getInstance().startMeshOta(meshOtaParameters);

            infoHandler.obtainMessage(MSG_INFO, "continue firmware update...").sendToTarget();
        }
    }


    private List<MeshUpdatingDevice> getUpdatingDevices() {
        List<MeshUpdatingDevice> meshUpdatingDevices = new ArrayList<>();
        try {
            for (MeshUpdatingDevice device :
                    updatingDevices) {
                meshUpdatingDevices.add((MeshUpdatingDevice) device.clone());
            }
            return meshUpdatingDevices;
        } catch (CloneNotSupportedException e) {
            e.printStackTrace();
        }
        return null;
    }

    private void initView() {
        rv_device = findViewById(R.id.rv_device);

        rv_device.setLayoutManager(new LinearLayoutManager(this));
        if (mesh.nodes != null) {
            for (NodeInfo deviceInfo : mesh.nodes) {
                deviceInfo.selected = false;
            }
        }

        deviceAdapter = new FUDeviceAdapter(this);
        rv_device.setAdapter(deviceAdapter);
        tv_select_device = findViewById(R.id.tv_select_device);
        tv_select_device.setOnClickListener(this);
        btn_start = findViewById(R.id.btn_start);
        btn_start.setOnClickListener(this);
        btn_get_version = findViewById(R.id.btn_get_version);
        btn_get_version.setOnClickListener(this);
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
//        rg_policy = findViewById(R.id.rg_policy);
        rg_dist.setOnCheckedChangeListener((group, checkedId) -> {
            ll_policy.setVisibility(checkedId == R.id.rb_device ? View.VISIBLE : View.INVISIBLE);
            distributorType = checkedId == R.id.rb_device ? DistributorType.DEVICE : DistributorType.PHONE;
        });
        rb_plc_ver_apl.setOnCheckedChangeListener((buttonView, isChecked) -> {
            if (isChecked)
                updatePolicy = UpdatePolicy.VerifyAndApply;
        });
        rb_plc_ver.setOnCheckedChangeListener((buttonView, isChecked) -> {
            if (isChecked)
                updatePolicy = UpdatePolicy.VerifyOnly;
        });


        logInfoAdapter = new LogInfoAdapter(this, logInfoList);

    }

    private void addEventListeners() {

        // firmware info
        TelinkMeshApplication.getInstance().addEventListener(FirmwareUpdateInfoStatusMessage.class.getName(), this);
//        TelinkMeshApplication.getInstance().addEventListener(FirmwareMetadataStatusMessage.class.getName(), this);
//        TelinkMeshApplication.getInstance().addEventListener(NodeStatusChangedEvent.EVENT_TYPE_NODE_STATUS_CHANGED, this);
        TelinkMeshApplication.getInstance().addEventListener(MeshEvent.EVENT_TYPE_DISCONNECTED, this);
    }

    @Override
    public void onBackPressed() {
        if (isComplete) {
            super.onBackPressed();
        } else {
            showWarningDialog();
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

            case R.id.tv_select_device:
                startActivityForResult(new Intent(this, FUDeviceSelectActivity.class), REQUEST_CODE_SELECT_DEVICE);
                break;

            case R.id.tv_info:
                logInfoAdapter.notifyDataSetChanged();
                bottomDialog.show();
                break;

            case R.id.btn_start:

                final int directAddress = MeshService.getInstance().getDirectConnectedNodeAddress();
                if (directAddress == 0) {
                    showTipDialog("No device connected\n , pls wait for device connected");
                    return;
                }
                if (firmwareData == null) {
                    toastMsg("pls select file");
                    return;
                }

                // for test - start
//                firmwareData = new byte[4 * 1024];
//                for (int i = 0; i < firmwareData.length; i++) {
//                    firmwareData[i] = (byte) (i % 0x0F);
//                }
                // for test - end

                if (updatingDevices == null || updatingDevices.size() == 0) {
                    toastMsg("pls select at least ONE device");
                    return;
                }

                if (distributorType == DistributorType.DEVICE) {
                    distAddress = directAddress;
                } else {
                    distAddress = 0;
                    FUCacheService.getInstance().clear(this);
                }


                infoHandler.obtainMessage(MSG_INFO, "Mesh OTA preparing...").sendToTarget();
                enableUI(false);

                MeshInfo meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();

                this.metadata = new byte[8];
                System.arraycopy(this.firmwareData, 2, this.metadata, 0, 4);

                FirmwareUpdateConfiguration configuration = new FirmwareUpdateConfiguration(getUpdatingDevices(),
                        this.firmwareData, this.metadata,
                        meshInfo.getDefaultAppKeyIndex(), MESH_OTA_GROUP_ADDRESS);
                configuration.setUpdatePolicy(updatePolicy);
                configuration.setDistributorType(distributorType);
                configuration.setDistributorAddress(distAddress);
                configuration.setProxyAddress(directAddress);
                configuration.setCallback(this);
                configuration.setFirmwareId(firmwareId);

                MeshOtaParameters meshOtaParameters = new MeshOtaParameters(configuration);
                isComplete = false;
                MeshService.getInstance().startMeshOta(meshOtaParameters);
                break;

            case R.id.tv_file_path:
                startActivityForResult(new Intent(this, FileSelectActivity.class).putExtra(FileSelectActivity.KEY_SUFFIX, ".bin"), REQUEST_CODE_GET_FILE);
                break;

            case R.id.btn_get_version:
                meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
                int address;
                if (updatingDevices != null && updatingDevices.size() == 1) {
                    address = updatingDevices.get(0).meshAddress;
                } else {
                    address = 0xFFFF;
                }

                FirmwareUpdateInfoGetMessage infoGetMessage = FirmwareUpdateInfoGetMessage.getSimple(address,
                        meshInfo.getDefaultAppKeyIndex());
                infoGetMessage.setResponseMax(0);
                if (MeshService.getInstance().sendMeshMessage(infoGetMessage)) {
                    if (updatingDevices != null) {
                        for (MeshUpdatingDevice device : updatingDevices) {
                            device.firmwareId = null;
                        }
                        deviceAdapter.notifyDataSetChanged();
                    }
                } else {
                    toastMsg("get firmware fail");
                }
                break;
        }
    }

    private void enableUI(final boolean enable) {
        runOnUiThread(() -> {
            btn_start.setEnabled(enable);
            btn_get_version.setEnabled(enable);
            rb_device.setEnabled(enable);
            rb_phone.setEnabled(enable);
            rb_plc_ver_apl.setEnabled(enable);
            rb_plc_ver.setEnabled(enable);
            tv_file_path.setEnabled(enable);
            tv_select_device.setEnabled(enable);
        });
    }

    private void appendLog(String logInfo) {
        MeshLogger.d("mesh-OTA appendLog: " + logInfo);
        logInfoList.add(new LogInfo("FW-UPDATE", logInfo, MeshLogger.LEVEL_DEBUG));
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (bottomDialog.isShowing()) {
                    logInfoAdapter.notifyDataSetChanged();
                    rv_log.smoothScrollToPosition(logInfoList.size() - 1);
                }
            }
        });

    }

    private final Runnable RECONNECT_TASK = () -> showConfirmDialog("Device reconnect fail, quit mesh OTA ? ", (dialog, which) -> {
        MeshService.getInstance().idle(true);
        isComplete = true;
        infoHandler.obtainMessage(MSG_INFO, "Quit !!!").sendToTarget();
    });

    /****************************************************************
     * events - start
     ****************************************************************/
    @Override
    public void performed(Event<String> event) {

        final String eventType = event.getType();
        if (eventType.equals(MeshEvent.EVENT_TYPE_DISCONNECTED)) {
            if (!isComplete) {
                delayHandler.removeCallbacks(RECONNECT_TASK);
                delayHandler.postDelayed(RECONNECT_TASK, 3 * 60 * 1000);
            }
            infoHandler.obtainMessage(MSG_INFO, "GATT disconnected").sendToTarget();
        } else if (eventType.equals(FirmwareUpdateInfoStatusMessage.class.getName())) {
            final NotificationMessage notificationMessage = ((StatusNotificationEvent) event).getNotificationMessage();
            FirmwareUpdateInfoStatusMessage infoStatusMessage = (FirmwareUpdateInfoStatusMessage) notificationMessage.getStatusMessage();
            FirmwareUpdateInfoStatusMessage.FirmwareInformationEntry firstEntry = infoStatusMessage.getFirstEntry();
            if (firstEntry != null) {
                final byte[] firmwareId = firstEntry.currentFirmwareID;
                final int src = notificationMessage.getSrc();
                boolean updated = false;
                if (updatingDevices != null) {
                    for (MeshUpdatingDevice device : updatingDevices) {
                        if (device.meshAddress == src) {
                            if (!Arrays.equals(device.firmwareId, firmwareId)) {
                                device.firmwareId = firmwareId;
                                updated = true;
                            } else {
                                MeshLogger.d("the same firmware id");
                            }
                            break;
                        }
                    }
                }
                if (updated) {
                    MeshLogger.d("firmware updated ");
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            deviceAdapter.notifyDataSetChanged();
                        }
                    });
                } else {
                    MeshLogger.d("firmware not updated");
                }

            }

        }
    }


    /****************************************************************
     * events - end
     ****************************************************************/

    private void onDeviceOtaFail(MeshUpdatingDevice updatingDevice, String desc) {
        updatingDevice.state = MeshUpdatingDevice.STATE_FAIL;
        appendLog("device update fail - " + updatingDevice.meshAddress + " --- " + desc);
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                deviceAdapter.notifyDataSetChanged();
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
            this.updatingDevices = data.getParcelableArrayListExtra(FUDeviceSelectActivity.KEY_SELECTED_DEVICES);
            deviceAdapter.resetDevices(this.updatingDevices);
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
//        MeshLogger.log(log, tag, logLevel);
        /*
        logInfoList.add(new LogInfo(tag, log, logLevel));
        if (bottomDialog.isShowing()) {
            logInfoAdapter.notifyDataSetChanged();
            rv_log.smoothScrollToPosition(logInfoList.size() - 1);
        }*/
    }

    @Override
    public void onStateUpdated(FUState state, String extraInfo) {
        String info = state.desc + (extraInfo == null ? "" : (" - " + extraInfo));
        dismissConfirmDialog();
        delayHandler.removeCallbacks(RECONNECT_TASK);
        infoHandler.obtainMessage(MSG_INFO, info).sendToTarget();
        if (state == FUState.DISTRIBUTING_BY_DEVICE && distributorType == DistributorType.DEVICE) {
            FUCache fuCache = new FUCache();
            fuCache.distAddress = distAddress;
            fuCache.updatePolicy = updatePolicy;
            fuCache.updatingDeviceList = updatingDevices;
            fuCache.firmwareId = firmwareId;

            FUCacheService.getInstance().save(this, fuCache);
            // If the direct connected device is selected as distributor, check distribution progress after initiate distributor success
        }
        if (state == FUState.UPDATE_FAIL) {
            for (MeshUpdatingDevice device : updatingDevices) {
                onDeviceOtaFail(device, "update fail");
            }
        }
        if (state == FUState.UPDATE_COMPLETE || state == FUState.UPDATE_FAIL) {
            this.isComplete = true;
            runOnUiThread(this::showCompleteDialog);

        }
    }

    @Override
    public void onDeviceStateUpdate(MeshUpdatingDevice device, String desc) {
        appendLog("device state update - adr: " + device.meshAddress + " - state: " + device.state + " - " + desc);
        for (MeshUpdatingDevice dev : updatingDevices) {
            if (dev.meshAddress == device.meshAddress) {
                dev.state = device.state;
                dev.additionalInformation = device.additionalInformation;
                break;
            }
        }
        runOnUiThread(() -> deviceAdapter.notifyDataSetChanged());
        if (device.state == MeshUpdatingDevice.STATE_SUCCESS) {
            final AdditionalInformation addInfo = device.additionalInformation;
            if (addInfo == AdditionalInformation.NODE_UNPROVISIONED) {
                appendLog("device will be removed : " + device.meshAddress);
                mesh.removeNode(mesh.getDeviceByMeshAddress(device.meshAddress));
                mesh.saveOrUpdate();
            } else if (addInfo == AdditionalInformation.CPS_CHANGED_1 || addInfo == AdditionalInformation.CPS_CHANGED_2) {
                //

            }
        }
    }

    @Override
    public void onTransferProgress(int progress, BlobTransferType transferType) {
        onLog("null", "transfer progress update: " + progress + " type - " + transferType, MeshLogger.DEFAULT_LEVEL);
        if (transferType == BlobTransferType.GATT_INIT || transferType == BlobTransferType.LOCAL_INIT) {
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
