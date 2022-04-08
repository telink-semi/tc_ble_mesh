/********************************************************************************************************
 * @file BindingController.java
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
package com.telink.ble.mesh.core.access;

import android.os.Handler;
import android.os.HandlerThread;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.core.message.MeshSigModel;
import com.telink.ble.mesh.core.message.NotificationMessage;
import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.StatusMessage;
import com.telink.ble.mesh.core.message.aggregator.OpcodeAggregatorSequenceMessage;
import com.telink.ble.mesh.core.message.aggregator.OpcodeAggregatorStatusMessage;
import com.telink.ble.mesh.core.message.config.AppKeyAddMessage;
import com.telink.ble.mesh.core.message.config.AppKeyStatusMessage;
import com.telink.ble.mesh.core.message.config.CompositionDataGetMessage;
import com.telink.ble.mesh.core.message.config.CompositionDataStatusMessage;
import com.telink.ble.mesh.core.message.config.ModelAppBindMessage;
import com.telink.ble.mesh.core.message.config.ModelAppStatusMessage;
import com.telink.ble.mesh.core.networking.AccessType;
import com.telink.ble.mesh.entity.BindingDevice;
import com.telink.ble.mesh.entity.CompositionData;
import com.telink.ble.mesh.util.MeshLogger;

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

    private final String LOG_TAG = "Binding";

    public static final int STATE_FAIL = 0;

    public static final int STATE_SUCCESS = 1;

    /**
     * init
     */
    private static final int STEP_INIT = 0;

    /**
     * getting composition data
     */
    private static final int STEP_GET_CPS = 1;

    /**
     * app key adding
     */
    private static final int STEP_APP_KEY_ADD = 2;

    /**
     * key binding one by one
     */
    private static final int STEP_APP_KEY_BIND = 3;

    /**
     * send app key add and model bind by opcode-aggregator
     */
    private static final int STEP_SEND_OP_AGG = 4;

    private int step = STEP_INIT;

    private int netKeyIndex;

    private byte[] appKey;

    /**
     * binding target node
     */
    private BindingDevice bindingDevice;

    /**
     * binding models index
     */
    private int modelIndex = 0;
    //    private int appKeyIndex;
    /**
     * target models in binding device
     */
    private List<BindingModel> bindingModels = new ArrayList<>();

    // for command sent
    private AccessBridge accessBridge;

    private Handler delayHandler;

    /**
     * check is the cps contains Opcode Aggregator server model
     *
     * @see MeshSigModel#SIG_MD_CFG_OP_AGG_S
     */
    private boolean isAggSupported = false;

    private static final long BINDING_TIMEOUT_GATT = 30 * 1000;

    private static final long BINDING_TIMEOUT_ADV = 60 * 1000;

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
        delayHandler.postDelayed(bindingTimeoutTask,
                isGattBearer() ? BINDING_TIMEOUT_GATT : BINDING_TIMEOUT_ADV);

        log("binding begin: defaultBound? " + device.isDefaultBound());
        if (bindingDevice.isDefaultBound()) {
            addAppKey();
        } else {
            if (bindingDevice.getCompositionData() == null) {
                this.getCompositionData();
            } else {
                onCompositionDataReceived(bindingDevice.getCompositionData());
            }

        }
    }

    public void clear() {
        if (delayHandler != null) {
            delayHandler.removeCallbacksAndMessages(null);
        }
        modelIndex = 0;
        step = STEP_INIT;
        this.bindingModels.clear();
    }

    private boolean isGattBearer() {
        return bindingDevice != null && bindingDevice.getBearer() == BindingBearer.GattOnly;
    }

    private void getCompositionData() {
        updateStep(STEP_GET_CPS);
        onMeshMessagePrepared(new CompositionDataGetMessage(this.bindingDevice.getMeshAddress()));
    }

    private void addAppKey() {
        log("add app key");
        updateStep(STEP_APP_KEY_ADD);
        AppKeyAddMessage command = new AppKeyAddMessage(this.bindingDevice.getMeshAddress());
        command.setNetKeyIndex(this.netKeyIndex);
        command.setAppKeyIndex(this.bindingDevice.getAppKeyIndex());
        command.setAppKey(this.appKey);
        onMeshMessagePrepared(command);
    }

    private void onMeshMessagePrepared(MeshMessage meshMessage) {
        if (accessBridge != null) {
            if (!isGattBearer()) {
                meshMessage.setRetryCnt(8);
            }
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
            log("bind next model: " + Integer.toHexString(modelId) + " at: " + Integer.toHexString(eleAdr));
            onMeshMessagePrepared(command);
        } else {
            onBindSuccess();
        }
    }

    private void updateStep(int step) {
        log("update bind step: " + step);
        this.step = step;
    }

    private void onCompositionDataReceived(CompositionData compositionData) {
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
            log("models prepared: " + this.bindingModels.size());
            bindingDevice.setCompositionData(compositionData);
            //  draft feature
            isAggSupported = false;
            if (isAggSupported) {
                log("bind app key by opcode-aggregator");
                sendOpAggMessage();
            } else {
                addAppKey();
            }
        }
    }

    /**
     * add app key,
     */
    private void sendOpAggMessage() {
        updateStep(STEP_SEND_OP_AGG);

        List<MeshMessage> aggMsgs = new ArrayList<>();
        // B8090600000000009F2F5D99D2B563E4CA8390D116DC7C73803D060000000200803D060000000300803D060000000010803D060000000210803D060000000410803D060000000610803D060000000710803D060000000013803D060000000113803D060000000313803D060000000413803D0600000011020000803D070000000210803D070000000613
        AppKeyAddMessage appKeyAddMsg = new AppKeyAddMessage(this.bindingDevice.getMeshAddress());
        appKeyAddMsg.setNetKeyIndex(this.netKeyIndex);
        appKeyAddMsg.setAppKeyIndex(this.bindingDevice.getAppKeyIndex());
        appKeyAddMsg.setAppKey(this.appKey);
        // insert app key add message
        aggMsgs.add(appKeyAddMsg);

        ModelAppBindMessage bindMsg;
        for (BindingModel bindingModel : bindingModels) {
            int modelId = bindingModel.modelId;
            bindMsg = new ModelAppBindMessage(bindingDevice.getMeshAddress());

            int eleAdr = bindingDevice.getMeshAddress() + bindingModel.elementOffset;
            bindMsg.setElementAddress(eleAdr);
            bindMsg.setAppKeyIndex(bindingDevice.getAppKeyIndex());
            bindMsg.setSigModel(bindingModel.sig);
            bindMsg.setModelIdentifier(modelId);
            aggMsgs.add(bindMsg);
        }
        int elementAddress = this.bindingDevice.getMeshAddress();
        byte[] params = MeshUtils.aggregateMessages(elementAddress, aggMsgs);
        MeshMessage aggSeqMsg = new OpcodeAggregatorSequenceMessage(this.bindingDevice.getMeshAddress(), AccessType.DEVICE, bindingDevice.getAppKeyIndex(), params);
        onMeshMessagePrepared(aggSeqMsg);
    }


    private List<BindingModel> getAllModels(CompositionData compositionData) {
        if (compositionData.elements == null) return null;

        List<BindingModel> models = new ArrayList<>();
        isAggSupported = false;
        int offset = 0;
        for (CompositionData.Element ele : compositionData.elements) {
            if (ele.sigModels != null) {
                for (int modelId : ele.sigModels) {
                    if (!MeshSigModel.useDeviceKeyForEnc(modelId)) {
                        models.add(new BindingModel(modelId, offset, true));
                    }
                    if (modelId == MeshSigModel.SIG_MD_CFG_OP_AGG_S.modelId) {
                        isAggSupported = true;
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
                if (step != STEP_GET_CPS) {
                    log("step not getting cps");
                    return;
                }
                CompositionData compositionData = ((CompositionDataStatusMessage) message.getStatusMessage()).getCompositionData();
                onCompositionDataReceived(compositionData);
                break;

            case APPKEY_STATUS: {
                if (step != STEP_APP_KEY_ADD) {
                    log("step not app key adding");
                    return;
                }
                AppKeyStatusMessage appKeyStatusMessage = ((AppKeyStatusMessage) message.getStatusMessage());
                onAppKeyStatus(appKeyStatusMessage);
                break;
            }
            case MODE_APP_STATUS: {
                if (step != STEP_APP_KEY_BIND) {
                    log("step not app key binding");
                    return;
                }
                ModelAppStatusMessage appStatus = ((ModelAppStatusMessage) message.getStatusMessage());
                onModelAppStatus(appStatus);
                break;
            }
            case OP_AGGREGATOR_STATUS: {
                if (step != STEP_SEND_OP_AGG) {
                    log("step not at sending op agg");
                    return;
                }
                OpcodeAggregatorStatusMessage opAggStatusMsg = (OpcodeAggregatorStatusMessage) message.getStatusMessage();
                if (opAggStatusMsg.status != 0) {
                    onBindFail("opcode aggregator status error: " + opAggStatusMsg.status);
                    return;
                }

                List<StatusMessage> statusMessageList = MeshUtils.parseOpcodeAggregatorStatus(opAggStatusMsg);
                if (statusMessageList == null || statusMessageList.size() == 0) {
                    onBindFail("opcode agg status parse error");
                    return;
                }

                StatusMessage statusMessage;
                for (int i = 0; i < statusMessageList.size(); i++) {
                    statusMessage = statusMessageList.get(i);
                    if (statusMessage instanceof AppKeyStatusMessage) {
                        AppKeyStatusMessage appKeyStatusMessage = (AppKeyStatusMessage) statusMessage;
                        if (appKeyStatusMessage.getStatus() == 0) {
                            log("app key add success (op agg)");
                        } else {
                            onBindFail("app key status error (op agg)");
                            return;
                        }
                    } else if (statusMessage instanceof ModelAppStatusMessage) {
                        ModelAppStatusMessage appStatus = ((ModelAppStatusMessage) statusMessage);
                        int modelId = appStatus.getModelIdentifier();
                        boolean isSig = isSigModel(modelId);
                        if (!isSig || appStatus.getStatus() == 0) {
                            log(String.format("model bind success (op agg) modelId-%06X isSig-%b", modelId, isSig));
                        } else {
                            onBindFail("mode app status error (op agg)");
                            return;
                        }
                    }
                }
                log("bind complete by op agg");
                onBindSuccess();
            }
            break;
        }
    }

    private boolean isSigModel(int modelId) {
        for (BindingModel model : bindingModels) {
            if (model.modelId == modelId) {
                return model.sig;
            }
        }
        return false;
    }


    private void onAppKeyStatus(AppKeyStatusMessage appKeyStatusMessage) {
        if (appKeyStatusMessage.getStatus() == 0) {
            log("app key add success");
            if (bindingDevice.isDefaultBound()) {
                log("default bound complete");
                onBindSuccess();
            } else {
                updateStep(STEP_APP_KEY_BIND);
                bindNextModel();
            }
        } else {
            onBindFail("app key status error");
        }

    }

    private void onModelAppStatus(ModelAppStatusMessage appStatus) {
        if (bindingModels.size() > modelIndex) {
            final int modelId = bindingModels.get(modelIndex).modelId;
            final boolean sig = bindingModels.get(modelIndex).sig;
            if (modelId == appStatus.getModelIdentifier()) {
                if (!sig || appStatus.getStatus() == 0) {
                    modelIndex++;
                    bindNextModel();
                } else {
                    onBindFail("mode app status error");
                }
            } else {
                log("model id error");
                modelIndex++;
                bindNextModel();
            }
        }
    }

    public void onBindingCommandComplete(boolean success, int opcode, int rspMax, int rspCount) {
        if (!success) {
            onBindFail("binding command send fail");
        }
    }

    private void onBindFail(String desc) {
        log("binding fail: " + desc);
        onBindingComplete();
        onBindState(STATE_FAIL, desc);
    }

    private void onBindSuccess() {
        onBindingComplete();
        onBindState(STATE_SUCCESS, "binding success");
    }

    private void onBindingComplete() {
        clear();
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

    private void log(String logInfo) {
        log(logInfo, MeshLogger.LEVEL_DEBUG);
    }

    private void log(String logMessage, int level) {
        MeshLogger.log(logMessage, LOG_TAG, level);
    }
}
