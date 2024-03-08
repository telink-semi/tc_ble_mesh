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
package com.telink.ble.mesh.ui.activity;

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

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONException;
import com.google.android.material.bottomsheet.BottomSheetDialog;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.access.fu.BlobTransferType;
import com.telink.ble.mesh.core.access.fu.DistributorType;
import com.telink.ble.mesh.core.access.fu.FUCallback;
import com.telink.ble.mesh.core.access.fu.FUState;
import com.telink.ble.mesh.core.access.fu.UpdatePolicy;
import com.telink.ble.mesh.core.message.MeshSigModel;
import com.telink.ble.mesh.core.message.NotificationMessage;
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
import com.telink.ble.mesh.ui.adapter.FUDeviceAdapter;
import com.telink.ble.mesh.ui.adapter.LogInfoAdapter;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.FileSystem;
import com.telink.ble.mesh.util.LogInfo;
import com.telink.ble.mesh.util.MeshLogger;
import com.telink.ble.mesh.web.ResponseInfo;
import com.telink.ble.mesh.web.TelinkHttpClient;
import com.telink.ble.mesh.web.WebUtils;
import com.telink.ble.mesh.web.entity.MeshNetworkDetail;
import com.telink.ble.mesh.web.entity.MeshNode;
import com.telink.ble.mesh.web.entity.MeshProductInfo;
import com.telink.ble.mesh.web.entity.MeshProductVersionInfo;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.List;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.FormBody;
import okhttp3.Response;

/**
 * firmware update by mesh
 * Created by kee on 2018/9/18.
 * 设备只能选择
 */
public class FUActivity extends BaseActivity implements View.OnClickListener,
        EventListener<String>, FUCallback {

    public final static String URL_GET_LATEST_VERSION_INFO = TelinkHttpClient.URL_BASE + "mesh-node/getLatestVersionInfo";

    public final static String URL_UPDATE_NODE_VERSION = TelinkHttpClient.URL_BASE + "mesh-node/updateNodeVersion";

    public final static String URL_DOWNLOAD_BIN = TelinkHttpClient.URL_BASE + "mesh-node/downloadBin";

    public static final String KEY_FU_CONTINUE = "com.telink.ble.mesh.fu-continue";

    /**
     * group mesh address used in mesh-OTA procedure
     */
    private static final int MESH_OTA_GROUP_ADDRESS = 0xC00F;

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
    private MeshNetworkDetail mesh;

    /**
     * local devices
     */
//    private List<NodeInfo> devices;

    /**
     * updating devices
     */
    private List<MeshUpdatingDevice> updatingDevices = new ArrayList<>();

    /**
     * UIView
     */
    private Button btn_start, btn_get_version;
    private RecyclerView rv_device;
    private RadioGroup rg_dist; // rg_policy
    private RadioButton rb_device, rb_phone, rb_plc_ver_apl, rb_plc_ver;
    private ProgressBar pb_init, pb_dist;
    private LinearLayout ll_policy;
    private TextView tv_select_device,
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
     * vid in firmware data
     */
    private int binVid = 0;

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
        DialogInterface.OnClickListener dialogBtnClick = new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                if (which == DialogInterface.BUTTON_POSITIVE) {
                    // continue
                    continueFirmwareUpdate();
                } else if (which == DialogInterface.BUTTON_NEGATIVE) {
                    // start new fu processing
                    FUCacheService.getInstance().clear(FUActivity.this);
                    startNewUpdating();
                }
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
//        readFirmware("/storage/emulated/0/.a0kee/8258_mesh_LPN_noRebootWhenError.bin"); // lpn

//        readFirmware("/storage/emulated/0/kee/sigMesh/20210422_mesh_OTA/8258_mesh_V4.3_for_OTA.bin");
        appendLog("IDLE");
    }

    private void continueFirmwareUpdate() {
        MeshLogger.d("continue mesh OTA  --- ");
        enableUI(false);
        FUCache fuCache = FUCacheService.getInstance().get();
        if (fuCache == null) {
            appendLog("firmware update cache error");
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

            MeshNetworkDetail meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();

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
            appendLog("continue firmware update...");
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
        if (mesh.nodeList != null) {
            for (MeshNode deviceInfo : mesh.nodeList) {
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


                appendLog("Mesh OTA preparing...");
                enableUI(false);

                MeshNetworkDetail meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();

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
            tv_select_device.setEnabled(enable);
        });
    }

    private void appendLog(String logInfo) {
        MeshLogger.d("mesh-OTA appendLog: " + logInfo);
        logInfoList.add(new LogInfo("FW-UPDATE", logInfo, MeshLogger.LEVEL_DEBUG));
        runOnUiThread(() -> {
            tv_info.setText(logInfo);
            if (bottomDialog.isShowing()) {
                logInfoAdapter.notifyDataSetChanged();
                rv_log.smoothScrollToPosition(logInfoList.size() - 1);
            }
        });

    }

    private final Runnable RECONNECT_TASK = () -> showConfirmDialog("Device reconnect fail, quit mesh OTA ? ", (dialog, which) -> {
        MeshService.getInstance().idle(true);
        isComplete = true;
        appendLog("Quit !!!");
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
            appendLog("GATT disconnected");
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
        if (requestCode == REQUEST_CODE_SELECT_DEVICE) {
            ArrayList<Integer> selectedAddresses = data.getIntegerArrayListExtra(FUDeviceSelectActivity.KEY_SELECTED_DEVICES);
            refreshUpdatingDevices(selectedAddresses);
        }
    }

    private void refreshUpdatingDevices(ArrayList<Integer> selectedAddresses) {
        this.updatingDevices.clear();
        MeshProductInfo productInfo = null;
        MeshUpdatingDevice device;
        for (int address : selectedAddresses) {
            MeshNode node = mesh.getNodeByAddress(address);
            device = new MeshUpdatingDevice();
            device.meshAddress = (node.address);
            device.updatingEleAddress = (node.getTargetEleAdr(MeshSigModel.SIG_MD_OBJ_TRANSFER_S.modelId));
            device.isSupported = node.getTargetEleAdr(MeshSigModel.SIG_MD_FW_UPDATE_S.modelId) != -1;
            device.pidInfo = node.getPidDesc();
            device.vid = node.compositionData().vid;
            device.pid = node.compositionData().pid;
            device.isLpn = node.isLpn();
            updatingDevices.add(device);
            if (productInfo == null) {
                productInfo = node.productInfo;
            }
        }

        this.deviceAdapter.resetDevices(updatingDevices);
        if (updatingDevices.size() != 0) {
            getVersionInfo(productInfo);
        }
    }

    /**
     * get version by selected product
     */
    private void getVersionInfo(MeshProductInfo productInfo) {
//        enableStartButton(false);
        appendLog("fu get version - start");
        showIndeterminateLoading();
        FormBody formBody = new FormBody.Builder()
                .add("productId", productInfo.getId() + "")
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
                ResponseInfo responseInfo = WebUtils.parseResponse(response, FUActivity.this);
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
                    appendLog(String.format("get latest version success: %04X", versionInfo.getVid()));
                    updateVersionInfo(versionInfo);
                } catch (JSONException e) {
                    e.getStackTrace();
                }
            }
        });
    }

    private void updateVersionInfo(MeshProductVersionInfo versionInfo) {
        int remoteVid = versionInfo.getVid();
        boolean anyNeedUpdate = false;
        for (MeshUpdatingDevice device : updatingDevices) {
            device.needUpdate = remoteVid > device.vid;
            if (!device.needUpdate) {
                appendLog(String.format("The device is the latest version : %04X", device.meshAddress));
            } else {
                anyNeedUpdate = true;
                appendLog(String.format("Find the new version available - 0x%04X - %s", remoteVid, versionInfo.getName()));
            }
        }
        runOnUiThread(() -> deviceAdapter.notifyDataSetChanged());
        if (anyNeedUpdate) {
            checkBinFileState(versionInfo);
        } else {

            appendLog("no device need update");
        }
    }


    private void checkBinFileState(MeshProductVersionInfo versionInfo) {
        appendLog("check bin file");
        int productId = versionInfo.getProductId();
        int vid = versionInfo.getVid();
        File file = FileSystem.getBinPath(productId, vid);
        if (file.exists()) {
            appendLog("bin file exists - " + file.getAbsolutePath());
            readFirmware(file);
        } else {
            downloadBin(versionInfo);
        }
    }


    private void downloadBin(MeshProductVersionInfo versionInfo) {
        appendLog("download bind - start");

        String url = URL_DOWNLOAD_BIN + "?path=" + versionInfo.getBinFilePath();
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
                    file = FileSystem.getBinDownloadingPath(versionInfo.getProductId(), versionInfo.getVid());
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
                    File toFile = FileSystem.getBinPath(versionInfo.getProductId(), versionInfo.getVid());
                    boolean result = file.renameTo(toFile);
                    if (!result) {
                        appendLog("save bin error");
                    } else {
                        appendLog("save bin success");
                        readFirmware(toFile);
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


    private void readFirmware(File file) {
        firmwareData = FileSystem.readByteArray(file);
        if (firmwareData == null) {
            firmwareId = null;
            tv_version_info.setText(getString(R.string.version, "null"));
            return;
        }
        firmwareId = new byte[4];
        System.arraycopy(firmwareData, 2, firmwareId, 0, 4);
        byte[] pid = new byte[2];
        byte[] vid = new byte[2];
        System.arraycopy(firmwareData, 2, pid, 0, 2);
        this.binPid = MeshUtils.bytes2Integer(pid, ByteOrder.LITTLE_ENDIAN);
        System.arraycopy(firmwareData, 4, vid, 0, 2);
        this.binVid = MeshUtils.bytes2Integer(vid, ByteOrder.BIG_ENDIAN); //  yes , big endian
        String pidInfo = Arrays.bytesToHexString(pid, ":");
        String vidInfo = Arrays.bytesToHexString(vid, ":");
        String firmVersion = "pid-" + pidInfo + " vid-" + vidInfo;
        tv_version_info.setText(getString(R.string.version, firmVersion));
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
        appendLog("fu state update: " + info);
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
            updateCloudVersion(device.meshAddress);
            if (addInfo == AdditionalInformation.NODE_UNPROVISIONED) {
                appendLog("device will be removed : " + device.meshAddress);
                MeshService.getInstance().removeDevice(device.meshAddress);
                TelinkMeshApplication.getInstance().getMeshInfo().removeNode(device.meshAddress);
            } else if (addInfo == AdditionalInformation.CPS_CHANGED_1 || addInfo == AdditionalInformation.CPS_CHANGED_2) {
                //

            }
        }
    }

    private void updateCloudVersion(int meshAddress) {
        MeshNode node = mesh.getNodeByAddress(meshAddress);

        if (node != null) {
            node.updateCloudVersion(binVid, new Callback() {
                @Override
                public void onFailure(Call call, IOException e) {
                    appendLog(String.format("update node version to cloud error: %04X", meshAddress));
                }

                @Override
                public void onResponse(Call call, Response response) throws IOException {

                    ResponseInfo responseInfo = WebUtils.parseResponse(response, null);
                    if (responseInfo == null) {
                        appendLog(String.format("update node version to cloud rsp error: %04X", meshAddress));
                        return;
                    }
                    try {
                        MeshProductVersionInfo versionInfo = JSON.parseObject(responseInfo.data, MeshProductVersionInfo.class);
                        if (versionInfo == null) {
                            appendLog(String.format("update node version to cloud rsp parse error: %04X", meshAddress));
                            return;
                        }
                        appendLog(String.format("update node version to cloud success: %04X", meshAddress));
                    } catch (JSONException e) {
                        e.getStackTrace();
                    }
                }
            });
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
