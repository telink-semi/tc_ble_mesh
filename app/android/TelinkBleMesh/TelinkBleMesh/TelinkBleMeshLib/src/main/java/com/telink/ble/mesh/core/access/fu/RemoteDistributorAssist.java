/********************************************************************************************************
 * @file RemoteDistributorAssist.java
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
package com.telink.ble.mesh.core.access.fu;

import android.os.Handler;
import android.os.HandlerThread;

import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.core.message.NotificationMessage;
import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.firmwaredistribution.DistributionPhase;
import com.telink.ble.mesh.core.message.firmwaredistribution.FDApplyMessage;
import com.telink.ble.mesh.core.message.firmwaredistribution.FDGetMessage;
import com.telink.ble.mesh.core.message.firmwaredistribution.FDStatusMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.DistributionStatus;
import com.telink.ble.mesh.core.message.firmwareupdate.FirmwareUpdateInfoGetMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.FirmwareUpdateInfoStatusMessage;
import com.telink.ble.mesh.entity.FirmwareUpdateConfiguration;
import com.telink.ble.mesh.entity.MeshUpdatingDevice;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;

import java.util.List;

/**
 * remote distributor assistance
 * 1. get remote distribution progress
 * 2. send apply if needed
 */
public class RemoteDistributorAssist {
    private Handler handler;

    private static final int STEP_IDLE = 0;

    private static final int STEP_DIST_APPLYING = 1;

    /**
     * get distribution status after distribute apply send complete
     * if phase == completed, action complete
     */
    private static final int STEP_DIST_GET = 2;

    private static final int STEP_RECHECK = 3;

    private int step = STEP_IDLE;


    private static final long CONFIRM_INTERVAL = 10 * 1000;

    private static final long APPLY_INTERVAL = 10 * 1000;

    private int deviceIndex = 0;

    private FUActionHandler actionHandler;

    private List<MeshUpdatingDevice> devices;

    private int appKeyIndex;

    private int distributorAddress;

    private byte[] newFirmwareId;

    RemoteDistributorAssist(HandlerThread handlerThread, FUActionHandler actionHandler) {
        handler = new Handler(handlerThread.getLooper());
        this.actionHandler = actionHandler;
    }

    /**
     * @param configuration updating configs
     */
    void resetConfig(FirmwareUpdateConfiguration configuration) {
        this.distributorAddress = configuration.getDistributorAddress();
        this.devices = configuration.getUpdatingDevices();
        this.appKeyIndex = configuration.getAppKeyIndex();
        this.newFirmwareId = configuration.getFirmwareId();
        deviceIndex = 0;
    }

    void clear() {
        this.step = STEP_IDLE;
        handler.removeCallbacksAndMessages(null);
    }


    void applyDistribute() {
        log("apply distribution");
        step = STEP_DIST_APPLYING;
        deviceIndex = 0;
        executeAction();
    }

    private final Runnable APPLY_TASK = new Runnable() {
        @Override
        public void run() {
            applyDistribute();
        }
    };

    void confirmDistribute() {
        // check distribute util distribute phase is completed
        log("confirm distribute after(ms) : " + CONFIRM_INTERVAL);
        step = STEP_DIST_GET;
        handler.removeCallbacks(DIST_GET_TASK);
        handler.postDelayed(DIST_GET_TASK, CONFIRM_INTERVAL);
    }


    private final Runnable DIST_GET_TASK = new Runnable() {
        @Override
        public void run() {
            log("confirm distribute");
            FDGetMessage fdGetMessage = FDGetMessage.getSimple(distributorAddress, appKeyIndex);
            onMeshMessagePrepared(fdGetMessage);
        }
    };


    void recheckFirmware(boolean immediate) {
        log("recheck firmware");
        step = STEP_RECHECK;
        deviceIndex = 0;
        long delay = immediate ? 0 : 10 * 1000;
        handler.postDelayed(new Runnable() {
            @Override
            public void run() {
                executeAction();
            }
        }, delay);

    }


    private void onRecheckComplete() {
        log("recheck firmware info complete");
        onAssistComplete(true, "recheck complete");
    }

    private void executeAction() {
        log("execute action : " + step);
        if (step == STEP_DIST_APPLYING) {
            FDApplyMessage fdApplyMessage = FDApplyMessage.getSimple(this.distributorAddress, this.appKeyIndex);
            onMeshMessagePrepared(fdApplyMessage);
        } else if (step == STEP_RECHECK) {
            if (deviceIndex >= devices.size()) {
                onRecheckComplete();
            } else {
                MeshUpdatingDevice device = devices.get(deviceIndex);
                FirmwareUpdateInfoGetMessage infoGetMessage = FirmwareUpdateInfoGetMessage.getSimple(device.meshAddress,
                        appKeyIndex);
                onMeshMessagePrepared(infoGetMessage);
            }
        }
    }

    /**
     * response of
     * distribute apply
     * or
     * distribute get
     *
     * @param message
     */
    private void onDistributeStatus(NotificationMessage message) {
        if (step == STEP_DIST_APPLYING) {
            FDStatusMessage fdStatusMessage = (FDStatusMessage) message.getStatusMessage();
            if (fdStatusMessage.status == DistributionStatus.WRONG_PHASE.code && fdStatusMessage.distPhase == DistributionPhase.TRANSFER_ACTIVE.value) {
                log("waiting for next apply action...");
                handler.removeCallbacks(APPLY_TASK);
                handler.postDelayed(APPLY_TASK, APPLY_INTERVAL);
            } else {
                onAssistComplete(fdStatusMessage.status == DistributionStatus.SUCCESS.code, "distribute apply complete");
            }

        } else if (step == STEP_DIST_GET) {
            FDStatusMessage fdStatusMessage = (FDStatusMessage) message.getStatusMessage();
            if (fdStatusMessage.status == DistributionStatus.SUCCESS.code) {
                final int phase = fdStatusMessage.distPhase;
                if (phase == DistributionPhase.COMPLETED.value) {
                    onAssistComplete(true, "distribute confirm complete");
                } else if (phase == DistributionPhase.APPLYING_UPDATE.value) {
                    confirmDistribute();
                } else {
                    onAssistComplete(false, "distribute phase error");
                }
            } else {
                onAssistComplete(false, "distribute status error");
            }
        }

    }


    private void onFirmwareInfoStatus(NotificationMessage message) {
        FirmwareUpdateInfoStatusMessage infoStatusMessage = (FirmwareUpdateInfoStatusMessage) message.getStatusMessage();
        FirmwareUpdateInfoStatusMessage.FirmwareInformationEntry infoEntry = infoStatusMessage.getFirstEntry();
        int src = message.getSrc();
        if (devices == null) return;
        for (MeshUpdatingDevice device : devices) {
            if (device.meshAddress == src) {
                if (Arrays.equals(newFirmwareId, infoEntry.currentFirmwareID)) {
                    device.firmwareId = (infoEntry.currentFirmwareID);
                    onDeviceStateUpdate(device, MeshUpdatingDevice.STATE_SUCCESS, "device firmware id updated");
                } else {
                    onDeviceStateUpdate(device, MeshUpdatingDevice.STATE_FAIL, "device firmware id not updated");
                }
                break;
            }
        }
        deviceIndex++;
        executeAction();
    }


    private void onAssistComplete(boolean success, String desc) {
        this.step = STEP_IDLE;
        this.handler.removeCallbacksAndMessages(null);
        if (actionHandler != null) {
            actionHandler.onActionComplete(success, FUAction.DISTRIBUTE_ASSIST, desc);
        }
    }

    private void onDeviceFail(MeshUpdatingDevice device, String desc) {
        onDeviceStateUpdate(device, MeshUpdatingDevice.STATE_FAIL, desc);
    }

    private void onDeviceStateUpdate(MeshUpdatingDevice device, int state, String desc) {
        log(String.format("node updating changed : %04X -- " + desc, device.meshAddress));
        device.state = (state);
        if (actionHandler != null) {
            actionHandler.onDeviceUpdate(device, String.format("node(%04X)  state update : -- %02X -- %s",
                    device.meshAddress,
                    state,
                    desc));
        }
    }

    /**
     *
     */
    public void onMessageNotification(NotificationMessage message) {
        if (step == STEP_IDLE) {
            log("notification when idle");
            return;
        }
        if (message.getOpcode() == Opcode.FD_STATUS.value) {
            onDistributeStatus(message);
        } else if (step == STEP_RECHECK && message.getOpcode() == Opcode.FIRMWARE_UPDATE_INFORMATION_STATUS.value) {
            onFirmwareInfoStatus(message);
        }
    }

    /**
     *
     */
    public void onAssistCommandFailed(int opcode) {
        if ((step == STEP_DIST_APPLYING && opcode == Opcode.FIRMWARE_UPDATE_APPLY.value)) {
            // to updating nodes
            onDeviceFail(devices.get(deviceIndex), "apply command send fail");
            deviceIndex++;
            executeAction();
        } else if (step == STEP_RECHECK && opcode == Opcode.FIRMWARE_UPDATE_INFORMATION_GET.value) {
            onDeviceFail(devices.get(deviceIndex), "recheck command fail");
            deviceIndex++;
            executeAction();
        }
    }


    public boolean onMeshMessagePrepared(MeshMessage meshMessage) {
        MeshLogger.d("initiator send message to : " + meshMessage.getDestinationAddress());
        return this.actionHandler.onMessagePrepared(meshMessage);
    }


    boolean isRunning() {
        return step != STEP_IDLE;
    }


    private void log(String logInfo) {
        actionHandler.onActionLog("Assist", logInfo, MeshLogger.LEVEL_DEBUG);
    }
}
