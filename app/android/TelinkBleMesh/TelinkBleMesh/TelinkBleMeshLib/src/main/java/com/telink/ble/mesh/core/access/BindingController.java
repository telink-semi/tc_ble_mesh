package com.telink.ble.mesh.core.access;

import android.os.Handler;
import android.os.HandlerThread;

import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.core.message.MeshSigModel;
import com.telink.ble.mesh.core.message.NotificationMessage;
import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.config.AppKeyAddMessage;
import com.telink.ble.mesh.core.message.config.AppKeyStatusMessage;
import com.telink.ble.mesh.core.message.config.CompositionDataGetMessage;
import com.telink.ble.mesh.core.message.config.CompositionDataStatusMessage;
import com.telink.ble.mesh.core.message.config.ModelAppBindMessage;
import com.telink.ble.mesh.core.message.config.ModelAppStatusMessage;
import com.telink.ble.mesh.entity.BindingDevice;
import com.telink.ble.mesh.entity.CompositionData;
import com.telink.ble.mesh.util.TelinkLog;

import java.util.ArrayList;
import java.util.List;

/**
 * binding flow,
 * 1. get composition data
 * 2. add app key
 * 3. bind app key for selected models or all models
 * Created by kee on 2019/9/10.
 */

public class BindingController {

    private final String TAG = "Binding -- ";

    public static final int STATE_FAIL = 0;

    public static final int STATE_SUCCESS = 1;

    private int netKeyIndex;
    private byte[] appKey;

    private BindingDevice bindingDevice;
    private int modelIndex = 0;
    //    private int appKeyIndex;
    private List<BindingModel> bindingModels = new ArrayList<>();
    private AccessBridge accessBridge;

    private Handler delayHandler;

    private static final long BINDING_TIMEOUT = 30 * 1000;

    public BindingController(HandlerThread handlerThread) {
        this.delayHandler = new Handler(handlerThread.getLooper());
    }

    public void register(AccessBridge accessBridge) {
        this.accessBridge = accessBridge;
    }

    public BindingDevice getBindingDevice() {
        return bindingDevice;
    }

    public void begin(int netKeyIndex, byte[] appKey, BindingDevice device) {
        this.netKeyIndex = netKeyIndex;
        this.bindingDevice = device;
        this.appKey = appKey;
        this.bindingModels.clear();
        this.modelIndex = 0;

        delayHandler.removeCallbacks(bindingTimeoutTask);
        delayHandler.postDelayed(bindingTimeoutTask, BINDING_TIMEOUT);

        TelinkLog.d(TAG + "binding begin: defaultBound? " + device.isDefaultBound());
        if (bindingDevice.isDefaultBound()) {
            addAppKey();
        } else {
            this.getCompositionData();
        }
    }

    public void clear() {
        if (delayHandler != null) {
            delayHandler.removeCallbacksAndMessages(null);
        }
    }

    private void getCompositionData() {
        onMeshMessagePrepared(new CompositionDataGetMessage(this.bindingDevice.getMeshAddress()));
    }

    private void addAppKey() {
        TelinkLog.d(TAG + "add app key");
        AppKeyAddMessage command = new AppKeyAddMessage(this.bindingDevice.getMeshAddress());
        command.setNetKeyIndex(this.netKeyIndex);
        command.setAppKeyIndex(this.bindingDevice.getAppKeyIndex());
        command.setAppKey(this.appKey);
        onMeshMessagePrepared(command);
    }

    private void onMeshMessagePrepared(MeshMessage meshMessage) {
        if (accessBridge != null) {
            boolean isMessageSent = accessBridge.onAccessMessagePrepared(meshMessage, AccessBridge.MODE_BINDING);
            if (!isMessageSent) {
                onBindFail("binding message sent error");
            }
        }
    }

    private void bindNextModel() {

        if (bindingModels.size() > modelIndex) {
            BindingModel bindingModel = bindingModels.get(modelIndex);
            int modelId = bindingModel.modelId;
            ModelAppBindMessage command = new ModelAppBindMessage(bindingDevice.getMeshAddress());

            int eleAdr = bindingDevice.getMeshAddress() + bindingModel.elementOffset;
            command.setElementAddress(eleAdr);
            command.setAppKeyIndex(bindingDevice.getAppKeyIndex());
            command.setSigModel(bindingModel.sig);
            command.setModelIdentifier(modelId);
            TelinkLog.d(TAG + "bind next model: " + Integer.toHexString(modelId) + " at: " + Integer.toHexString(eleAdr));
            onMeshMessagePrepared(command);

        } else {
            onBindSuccess();
        }
    }

    private void onCompositionDataReceived(CompositionData compositionData) {
        // for test , false
        List<BindingModel> modelsInCps = getAllModels(compositionData);
        if (modelsInCps == null || modelsInCps.size() == 0) {
            onBindFail("no models in composition data");
            return;
        }
        this.bindingModels.clear();
        this.modelIndex = 0;
        if (bindingDevice.getModels() == null) {
            this.bindingModels.addAll(modelsInCps);
        } else {
            // models filter
            outer:
            for (BindingModel bindingModel : modelsInCps) {
                for (int modelId : bindingDevice.getModels()) {
                    if (bindingModel.modelId == modelId) {
                        this.bindingModels.add(bindingModel);
                        continue outer;
                    }
                }
            }
        }

        if (this.bindingModels.size() == 0) {
            onBindFail("no target models found");
        } else {

            TelinkLog.d(TAG + "models prepared: " + this.bindingModels.size());
            /*for (BindingModel bindingModel :
                    bindingModels) {
                TelinkLog.d(TAG + "model - " + bindingModel.modelId);
            }*/
            bindingDevice.setCompositionData(compositionData);
            addAppKey();
        }
    }

    private List<BindingModel> getAllModels(CompositionData compositionData) {
        if (compositionData.elements == null) return null;

        List<BindingModel> models = new ArrayList<>();

        int offset = 0;
        for (CompositionData.Element ele : compositionData.elements) {
            if (ele.sigModels != null) {
                for (int modelId : ele.sigModels) {
                    if (!MeshSigModel.isConfigurationModel(modelId)) {
                        models.add(new BindingModel(modelId, offset, true));
                    }
                }

            }
            if (ele.vendorModels != null) {
                for (int modelId : ele.vendorModels) {
                    models.add(new BindingModel(modelId, offset, false));
                }
            }
            offset++;
        }

        return models;
    }

    public void onMessageNotification(NotificationMessage message) {
        Opcode opcode = Opcode.valueOf(message.getOpcode());
        if (opcode == null) return;
        switch (opcode) {
            case COMPOSITION_DATA_STATUS:
                CompositionData compositionData = ((CompositionDataStatusMessage) message.getStatusMessage()).getCompositionData();
                onCompositionDataReceived(compositionData);
                break;

            case APPKEY_STATUS:
                AppKeyStatusMessage appKeyStatusMessage = ((AppKeyStatusMessage) message.getStatusMessage());
                if (appKeyStatusMessage.getStatus() == 0) {
                    TelinkLog.d(TAG + "app key add success");
                    if (bindingDevice.isDefaultBound()) {
                        TelinkLog.d(TAG + "default bound complete");
                        onBindSuccess();
                    } else {
                        bindNextModel();
                    }

                } else {
                    onBindFail("app key status error");
                }
                break;

            case MODE_APP_STATUS:
                ModelAppStatusMessage appStatus = ((ModelAppStatusMessage) message.getStatusMessage());
                if (appStatus.getStatus() == 0) {
                    modelIndex++;
                    bindNextModel();
                } else {
                    onBindFail("model app status error");
                }
                break;
        }
    }

    private void onBindFail(String desc) {
        TelinkLog.d(TAG + "binding fail: " + desc);
        onBindState(STATE_FAIL, desc);
    }

    private void onBindSuccess() {
        delayHandler.removeCallbacks(bindingTimeoutTask);
        onBindState(STATE_SUCCESS, "binding success");
    }

    private void onBindState(int state, String desc) {
        if (accessBridge != null) {
            accessBridge.onAccessStateChanged(state, desc, AccessBridge.MODE_BINDING, null);
        }
    }

    private class BindingModel {
        int modelId;
        int elementOffset;
        boolean sig;

        public BindingModel(int modelId, int offset, boolean sig) {
            this.modelId = modelId;
            this.elementOffset = offset;
            this.sig = sig;
        }
    }

    private Runnable bindingTimeoutTask = new Runnable() {
        @Override
        public void run() {
            onBindFail("binding timeout");
        }
    };
}
