/********************************************************************************************************
 * @file FUController.java
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

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.access.AccessBridge;
import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.core.message.NotificationMessage;
import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.firmwaredistribution.FDCancelMessage;
import com.telink.ble.mesh.core.message.firmwaredistribution.FDReceiversGetMessage;
import com.telink.ble.mesh.core.message.firmwaredistribution.FDReceiversListMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.UpdatePhase;
import com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer.TransferMode;
import com.telink.ble.mesh.entity.FirmwareUpdateConfiguration;
import com.telink.ble.mesh.entity.MeshUpdatingDevice;
import com.telink.ble.mesh.util.MeshLogger;

import java.util.List;

/**
 * implementation of firmware update flow
 * use FirmwareUpdateInitiator for initiation
 * use FirmwareUpdateDistributor for distribution
 */
public class FUController implements FUActionHandler {
    private final String LOG_TAG = "FU-Controller";


    private FUState currentState = FUState.IDLE;

    /**
     * firmware update initiator, used to send mesh message from initiator to distributor
     */
    private FUInitiator initiator = null;

    /**
     * firmware update distributor, used to send mesh message from distributor to updating nodes
     */
    private FUDistributor distributor = null;

    private RemoteDistributorAssist distributorAssist = null;

    private Handler handler;

    private FirmwareUpdateConfiguration configuration = null;

    /**
     * 0 means use APP as distributor
     */
    private int distributorAddress = 0;

    private int appKeyIndex = 0;

    private UpdatePolicy updatePolicy;

    private List<MeshUpdatingDevice> deviceList;

    /**
     * is update running
     * valued by true when update started
     * if distribute by Device, running stopped when distribute start success;
     * otherwise if distribute by Phone, running stopped when update apply success
     */

    private AccessBridge accessBridge;

    private boolean isDirectUpdating = false;

    private int connectedAddress;


    public FUController(HandlerThread thread) {
        handler = new Handler(thread.getLooper());
        initiator = new FUInitiator(thread, this);
        distributor = new FUDistributor(thread, this);
        distributorAssist = new RemoteDistributorAssist(thread, this);
    }

    /**
     * register
     */
    public void register(AccessBridge accessBridge) {
        this.accessBridge = accessBridge;
    }

    /**
     * start or continue firmware update flow
     */
    public void begin(FirmwareUpdateConfiguration configuration, int connectedAddress) {
        this.configuration = configuration;
        this.updatePolicy = configuration.getUpdatePolicy();
        this.deviceList = configuration.getUpdatingDevices();
        this.distributorAddress = configuration.getDistributorAddress();
        this.appKeyIndex = configuration.getAppKeyIndex();
        this.connectedAddress = connectedAddress;
        this.isDirectUpdating = isConnectedNodeUpdating(connectedAddress);
        this.distributorAssist.resetConfig(configuration);
        if (this.deviceList == null || this.deviceList.size() == 0) {
            onComplete(false, "device list empty");
            return;
        }
        log("FU begin - " + this.updatePolicy + "  isContinue ? " + configuration.isContinue() + " -- " + currentState + " -- " + configuration.toString());
        if (currentState == FUState.IDLE && !configuration.isContinue()) {
            startInitiate();
        } else {

            /*
             * may
             * 1. app reopened, check progress
             * 2. device disconnected and reconnect
             * 3. device apply complete
             */
//            currentState = FUState.DISTRIBUTING_BY_DEVICE;

            if (currentState == FUState.DISTRIBUTING_BY_PHONE) {
                //
                log("begin -> distributing by phone");
                onFUStateUpdate(FUState.DISTRIBUTING_BY_PHONE, null);
                distributor.begin(configuration, connectedAddress, true);
            } else if (currentState == FUState.UPDATE_RECHECKING) {
                log("begin -> rechecking firmware");
                distributorAssist.recheckFirmware(true);
            } else {
                onFUStateUpdate(FUState.DISTRIBUTING_BY_DEVICE, null);
                fetchProgressState();
            }
        }
    }

    public void stop() {
        if (isInitiating()) {
            onComplete(false, "initiate stopped");
        } else if (isDistributingByPhone()) {
            distributor.stop();
        } else if (isDistributingByDevice()) {
            cancelDistribution();
            onComplete(false, "device distribute stopped");
        }
    }

    public boolean isInitiating() {
        return currentState == FUState.INITIATING;
    }

    public boolean isDistributingByPhone() {
        return currentState == FUState.DISTRIBUTING_BY_PHONE;
    }

    public void hold() {
        log("hold dist " + distributor.isRunning());
        if (distributor.isRunning()) {
            distributor.holdTransfer();
        }
    }

    private void fetchProgressState() {
        FDReceiversGetMessage getMessage = FDReceiversGetMessage.getSimple(distributorAddress, appKeyIndex, 0, 1);
        onMessagePrepared(getMessage);
    }

    /**
     * check remote distribution state and progress
     */
    private void startProgressCheckTask() {
        log("start progress check task");
        handler.removeCallbacks(progressCheckTask);
        handler.postDelayed(progressCheckTask, 10 * 1000);
    }

    private final Runnable progressCheckTask = new Runnable() {
        @Override
        public void run() {
            fetchProgressState();
        }
    };

    private void onProgressState(NotificationMessage notificationMessage) {
        FDReceiversListMessage listMessage = (FDReceiversListMessage) notificationMessage.getStatusMessage();
        List<FDReceiversListMessage.DistributionReceiver> receiverList = listMessage.getReceiversList();

        if (receiverList != null && receiverList.size() != 0) {
            final FDReceiversListMessage.DistributionReceiver frsReceiver = receiverList.get(0);
            MeshLogger.d("first receiver : " + frsReceiver.toString());
            int transferProgress = frsReceiver.transferProgress * 2;
            onTransferProgress(transferProgress, BlobTransferType.MESH_DIST);
//            actionHandler.onTransferProgress(transferProgress, BlobTransferType.MESH);
            if (transferProgress >= 100) {
                int phase = frsReceiver.retrievedUpdatePhase;
                if (phase == UpdatePhase.IDLE.code) {
                    // rebooted, check firmware
                    onFUStateUpdate(FUState.UPDATE_RECHECKING, "device may reboot complete");
                    distributorAssist.recheckFirmware(true);
                } else if (phase == UpdatePhase.TRANSFER_ERROR.code) {
                    onComplete(false, "phase error");
                } else {
                    MeshLogger.d("onProgressState : " + currentState);

                    if (updatePolicy == UpdatePolicy.VerifyOnly) {
                        if (phase == UpdatePhase.VERIFICATION_FAILED.code || phase == UpdatePhase.VERIFICATION_SUCCESS.code) {
                            // apply all devices update
                            onFUStateUpdate(FUState.UPDATE_APPLYING, null);
                            distributorAssist.applyDistribute();
                        } else {
                            startProgressCheckTask();
                        }
                    } else {
                        if (isDirectUpdating) {
                            // gatt will be terminated
                            // waiting for reconnect
                            onFUStateUpdate(FUState.UPDATE_RECHECKING, null);
                            log("waiting for disconnect -- 1");
                        } else {
                            if (phase == UpdatePhase.APPLYING_UPDATE.code) {
                                onFUStateUpdate(FUState.DISTRIBUTE_CONFIRMING, null);
                                distributorAssist.confirmDistribute();
                            } else {
                                // distribute not complete
                                startProgressCheckTask();
                            }

                        }
                    }
                }

            } else {
                // check progress
                int phase = frsReceiver.retrievedUpdatePhase;
                if (phase == UpdatePhase.TRANSFER_ACTIVE.code) {
                    startProgressCheckTask();
                } else {
                    onComplete(false, "phase error - progress not 100 ");
                }

            }
        } else {
            // no receivers, recheck all firmware id
            onFUStateUpdate(FUState.UPDATE_RECHECKING, "no receivers found");
            distributorAssist.recheckFirmware(true);
        }
    }


    private boolean isConnectedNodeUpdating(int directAddress) {
        if (configuration == null || configuration.getUpdatingDevices() == null) return false;
        for (MeshUpdatingDevice device : configuration.getUpdatingDevices()) {
            if (device.meshAddress == directAddress) {
                return true;
            }
        }
        return false;
    }

    /**
     * if distributing by device or later, auto connect mesh
     */
    public boolean needAutoConnect() {
        return currentState.value >= FUState.DISTRIBUTING_BY_PHONE.value;
    }

    public boolean isRunning() {
        return currentState != FUState.IDLE;
    }

    /**
     * stop update actions and clear cache
     */
    public void clear() {
        currentState = FUState.IDLE;
        log("clear FU --------");
        if (initiator.isRunning()) {
            initiator.clear();
        }

        if (distributor.isRunning()) {
            distributor.clear();
        }

        if (configuration != null) {
            configuration.setCallback(null);
            this.configuration = null;
        }
    }

    public void dispatchError(String errorMsg) {
        onComplete(false, errorMsg);
    }

    /**
     * reliable command complete
     */
    public void onUpdatingCommandComplete(boolean success, int opcode, int rspMax, int rspCount) {
        log(String.format("updating command complete: opcode-%04X success?-%b", opcode, success));
        if (success) return; //  skip success state
        if (opcode == Opcode.FD_RECEIVERS_LIST.value) {
            onComplete(false, "receiver list command send fail");
            return;
        }
        if (initiator.isRunning()) {
            initiator.onInitiateCommandFailed(opcode);
        } else if (distributor.isRunning()) {
            distributor.onDistributeCommandFailed(opcode);
        } else if (distributorAssist.isRunning()) {
            distributorAssist.onAssistCommandFailed(opcode);
        }
    }

    /**
     * segmented message complete
     */
    public void onSegmentComplete(boolean success) {
        if (initiator.isRunning()) {
            initiator.getTransfer().onTransferSegmentComplete(success);
        } else if (distributor.isRunning()) {
            distributor.getTransfer().onTransferSegmentComplete(success);
        }
    }

    /**
     * receive mesh message notification
     */
    public void onMessageNotification(NotificationMessage notificationMessage) {
        log(String.format("rcv notification : %04X", notificationMessage.getOpcode()));
        if (notificationMessage.getOpcode() == Opcode.FD_RECEIVERS_LIST.value) {
            onProgressState(notificationMessage);
            return;
        }
        if (initiator.isRunning()) {
            log("retransmit to initiator");
            initiator.onMessageNotification(notificationMessage);
        } else if (distributor.isRunning()) {
            log("retransmit to distributor");
            distributor.onMessageNotification(notificationMessage);
        } else if (distributorAssist.isRunning()) {
            log("retransmit to assist");
            distributorAssist.onMessageNotification(notificationMessage);
        }
    }

    /**
     * current action: distributing by device
     */
    public boolean isDistributingByDevice() {
        return currentState == FUState.DISTRIBUTING_BY_DEVICE;
    }


    /**
     * start initiate by initiator
     */
    private void startInitiate() {
        initiator.clear();
        distributor.clear();
        onFUStateUpdate(FUState.INITIATING, null);
        initiator.begin(this.configuration, this.distributorAddress);
    }


    /**
     * firmware update state updated
     */
    private void onFUStateUpdate(FUState state, String extraInfo) {
        log("state update : " + state.desc);
        this.currentState = state;
        if (configuration != null) {
            configuration.dispatchFUState(state, extraInfo);
        }
        if (accessBridge != null) {
            accessBridge.onAccessStateChanged(state.value, state.desc, AccessBridge.MODE_FIRMWARE_UPDATING, null);
        }
    }

    private void onComplete(boolean success, String desc) {
        log("firmware update complete - " + desc + " -- " + success);
        cancelDistribution();
        handler.removeCallbacksAndMessages(null);
        final FUState fuState = success ? FUState.UPDATE_COMPLETE : FUState.UPDATE_FAIL;
        onFUStateUpdate(fuState, desc);
        clear();
    }

    /****************************************************************
     * {@link FUActionHandler} start
     ****************************************************************/
    @Override
    public void onDeviceUpdate(MeshUpdatingDevice device, String desc) {
        log("device state update : " + desc);
        configuration.dispatchDeviceState(device, desc);
//        onStateUpdate(STATE_DEVICE_FAIL, String.format("node updating fail: %04X -- ", device.getMeshAddress()), device);
    }

    /**
     * firmware update action complete
     */
    @Override
    public void onActionComplete(boolean success, FUAction action, String desc) {
        log("action complete - " + desc + " success ? " + success + " -- " + action);
        switch (action) {
            case INITIATE:
                if (!success) {
                    onFUStateUpdate(FUState.INITIATE_FAIL, null);
                    onComplete(false, desc);
                    return;
                }
                onFUStateUpdate(FUState.INITIATE_SUCCESS, null);
                if (distributorAddress == MeshUtils.LOCAL_MESSAGE_ADDRESS) {
                    // distribute by phone
                    log("start distribution by phone");
                    onFUStateUpdate(FUState.DISTRIBUTING_BY_PHONE, null);
                    distributor.begin(configuration, connectedAddress, false);
                } else {
                    // distribute by direct connected device, gatt connection can be terminated
                    log("start distribution by remote device -- > end");
                    onFUStateUpdate(FUState.DISTRIBUTING_BY_DEVICE, null);
                    startProgressCheckTask();
//                    fetchProgressState();
                }
                break;

            case DISTRIBUTE:
                if (success) {
                    // phone distribute complete, after send firmware update apply
                    onFUStateUpdate(FUState.UPDATE_RECHECKING, null);
                    if (!isDirectUpdating) {
                        distributorAssist.recheckFirmware(false);
                    } else {
                        // otherwise , gatt connection will be terminated
                        log("direct device will lose disconnection - 0");
                    }
                } else {
                    onComplete(false, desc);
                }

                break;

            case DISTRIBUTE_ASSIST:
                if (!success) {
                    onComplete(false, desc);
                    return;
                }
                if (currentState == FUState.UPDATE_APPLYING) {
                    // apply 完成, 设备重启或者不重启

                    if (!isDirectUpdating) {
                        onFUStateUpdate(FUState.DISTRIBUTE_CONFIRMING, null);
                        distributorAssist.confirmDistribute();
                    } else {
                        // otherwise , gatt connection will be terminated
                        log("direct device will lose disconnection - 1");
                        onFUStateUpdate(FUState.UPDATE_RECHECKING, null);
                    }


                } else if (currentState == FUState.DISTRIBUTE_CONFIRMING) {
                    // getting distribute complete and distribute phase is completed
                    onFUStateUpdate(FUState.UPDATE_RECHECKING, null);
                    distributorAssist.recheckFirmware(false);
                } else if (currentState == FUState.UPDATE_RECHECKING) {
                    // recheck firmware complete

                    onComplete(true, desc);
                }
                break;
        }
    }

    private void cancelDistribution() {
        FDCancelMessage cancelMessage = FDCancelMessage.getSimple(distributorAddress, appKeyIndex);
        cancelMessage.setAck(false);
        onMessagePrepared(cancelMessage);
    }

    @Override
    public boolean onMessagePrepared(MeshMessage meshMessage) {
        meshMessage.setRetryInterval(5 * 1000);
        return this.accessBridge.onAccessMessagePrepared(meshMessage, AccessBridge.MODE_FIRMWARE_UPDATING);
    }

    @Override
    public void onTransferStart(TransferMode transferMode) {
        if (configuration != null) {
            configuration.dispatchFUState(FUState.TRANSFER_START, transferMode.desc);
        }
    }

    @Override
    public void onTransferProgress(int progress, BlobTransferType transferType) {
        if (configuration != null) {
            configuration.dispatchTransferProgress(progress, transferType);
        }
    }

    @Override
    public void onActionLog(String tag, String log, int logLevel) {
        MeshLogger.log(log, LOG_TAG, logLevel);
        if (configuration != null) {
            configuration.dispatchLogInfo(tag, log, logLevel);
        }
    }

    /****************************************************************
     * {@link FUActionHandler} end
     ****************************************************************/

    private void log(String logInfo) {
        onActionLog(LOG_TAG, logInfo, MeshLogger.DEFAULT_LEVEL);
    }

}
