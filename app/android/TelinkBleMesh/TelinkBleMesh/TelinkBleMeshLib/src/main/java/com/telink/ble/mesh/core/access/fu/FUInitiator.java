/********************************************************************************************************
 * @file FUInitiator.java
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

import android.os.HandlerThread;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.core.message.MeshSigModel;
import com.telink.ble.mesh.core.message.NotificationMessage;
import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.config.ConfigStatus;
import com.telink.ble.mesh.core.message.config.ModelSubscriptionSetMessage;
import com.telink.ble.mesh.core.message.config.ModelSubscriptionStatusMessage;
import com.telink.ble.mesh.core.message.firmwaredistribution.FDCapabilitiesGetMessage;
import com.telink.ble.mesh.core.message.firmwaredistribution.FDCapabilitiesStatusMessage;
import com.telink.ble.mesh.core.message.firmwaredistribution.FDReceiversAddMessage;
import com.telink.ble.mesh.core.message.firmwaredistribution.FDReceiversStatusMessage;
import com.telink.ble.mesh.core.message.firmwaredistribution.FDStartMessage;
import com.telink.ble.mesh.core.message.firmwaredistribution.FDStatusMessage;
import com.telink.ble.mesh.core.message.firmwaredistribution.FDUploadStartMessage;
import com.telink.ble.mesh.core.message.firmwaredistribution.FDUploadStatusMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.AdditionalInformation;
import com.telink.ble.mesh.core.message.firmwareupdate.DistributionStatus;
import com.telink.ble.mesh.core.message.firmwareupdate.FirmwareMetadataCheckMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.FirmwareMetadataStatusMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.FirmwareUpdateInfoGetMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.FirmwareUpdateInfoStatusMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.UpdateStatus;
import com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer.TransferMode;
import com.telink.ble.mesh.entity.FirmwareUpdateConfiguration;
import com.telink.ble.mesh.entity.MeshUpdatingDevice;
import com.telink.ble.mesh.util.MeshLogger;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

/**
 * send mesh message to distributor
 * actions:
 * 1. firmware distribution capabilities get (to distributor)
 * 2. firmware update info get (to nodes)
 * 3. firmware update metadata check (to nodes)
 * 4. subscription set (to nodes)
 * 5. firmware distribution receivers add (to distributor)
 * 6. firmware distribution upload start (to distributor)
 * 7. blob transfer (to distributor)
 */
class FUInitiator implements BlobTransferCallback {

    private final String LOG_TAG = "FU-Initiator";

    /**
     * not in initiator processing
     */
    private static final int STEP_IDLE = 0;

    /**
     * get firmware distribution capabilities
     */
    private static final int STEP_DST_CAP_GET = 1;

    /**
     * get firmware update info
     */
    private static final int STEP_UPDATE_INFO_GET = 2;

    /**
     * get update metadata check
     */
    private static final int STEP_UPDATE_METADATA_CHECK = 3;

    /**
     * sub set (grouping)
     */
    private static final int STEP_SUB_SET = 4;

    /**
     * distribution receivers add
     */
    private static final int STEP_DST_RECEIVERS_ADD = 5;

    /**
     * distribution upload start
     */
    private static final int STEP_DST_UPLOAD_START = 6;


    /**
     * distribution upload start
     */
    private static final int STEP_BLOB_TRANSFER = 7;

    /**
     * send distribution start
     */
    public static final int STEP_DST_START = 8;

    private int step = STEP_IDLE;

//    private List<MeshUpdatingDevice> updatingDevices;


    /**
     * node address | ignore
     */
    private List<MeshUpdatingDevice> updatingDevices;

//    private int[] nodeAddresses;

    private int distributorAddress;

    /**
     * blob transfer type, may be gatt or local
     */
    private BlobTransferType type;

    /**
     * group address for subscription
     */
    private int groupAddress;

    /**
     * app key index that updating model bound with
     */
    private int appKeyIndex = 0;

    /**
     * operation index
     */
    private int deviceIndex;

    private int metadataIndex = 0;

    private byte[] metadata = new byte[8];

    private byte[] firmwareId;

    private long blobId;

    private int firmwareSize;

    private UpdatePolicy updatePolicy = UpdatePolicy.VerifyOnly;

    private FUActionHandler actionHandler;

    private BlobTransfer transfer;

    FUInitiator(HandlerThread handlerThread, FUActionHandler actionHandler) {
        transfer = new BlobTransfer(handlerThread, this);
        this.actionHandler = actionHandler;
    }

    /**
     * begin distributor initiation
     */
    void begin(FirmwareUpdateConfiguration configuration,
               int distributorAddress) {
        this.updatingDevices = configuration.getUpdatingDevices();
        this.metadata = configuration.getMetadata();
        this.groupAddress = configuration.getGroupAddress();
        this.appKeyIndex = configuration.getAppKeyIndex();
        this.blobId = configuration.getBlobId();
        if (!configuration.isContinue()) {
            this.firmwareSize = configuration.getFirmwareData().length;
        }

        this.firmwareId = configuration.getFirmwareId();
        this.updatePolicy = configuration.getUpdatePolicy();
        this.distributorAddress = distributorAddress;
        this.type = distributorAddress == MeshUtils.LOCAL_MESSAGE_ADDRESS ?
                BlobTransferType.LOCAL_INIT : BlobTransferType.GATT_INIT;
        transfer.resetParams(configuration, type, distributorAddress);
        log("initiator begin : node size - " + updatingDevices.size() + " -- distAdr - " + distributorAddress);

        this.step = STEP_DST_CAP_GET;
        nextAction();
    }

    void clear() {
        transfer.clear();
        /*if (this.step == STEP_BLOB_TRANSFER) {
            transfer.clear();
        }*/
        this.step = STEP_IDLE;
    }

    boolean isRunning() {
        return step != STEP_IDLE;
    }

    private void onInitComplete(boolean success, String desc) {
        if (success) {
            onTransferProgressUpdate(100, type);
        }
        step = STEP_IDLE;
        actionHandler.onActionComplete(success, FUAction.INITIATE, desc);
    }

    public BlobTransfer getTransfer() {
        return transfer;
    }

    /**
     * execute updating actions(one by one && step by step)
     * one device by one device
     * if all devices executed, then next step
     */
    // draft feature
    private void nextAction() { }


    /**
     * filter out initiator messages
     *
     * @param message notification message
     */
    public void onMessageNotification(NotificationMessage message) {
        Opcode opcode = Opcode.valueOf(message.getOpcode());
        log("message notification: " + opcode);
        if (step == STEP_IDLE) {
            log("notification when idle");
            return;
        }
        if (step == STEP_BLOB_TRANSFER) {
            log("notification trans to blob-transfer ");
            transfer.onTransferNotification(message);
            return;
        }
        if (opcode == null) return;
        final int src = message.getSrc();

        // message received from updating devices
        if (step == STEP_UPDATE_INFO_GET || step == STEP_UPDATE_METADATA_CHECK || step == STEP_SUB_SET) {
            if (updatingDevices.size() <= deviceIndex) {
                log("node index overflow");
                return;
            }

            if (updatingDevices.get(deviceIndex).meshAddress != src) {
                log("unexpected notification src");
                return;
            }
        }
        switch (opcode) {

            case FD_CAPABILITIES_STATUS:
                onCapStatus((FDCapabilitiesStatusMessage) message.getStatusMessage());
                break;

            case FIRMWARE_UPDATE_INFORMATION_STATUS:
                onFirmwareInfoStatus((FirmwareUpdateInfoStatusMessage) message.getStatusMessage());
                break;

            case FIRMWARE_UPDATE_FIRMWARE_METADATA_STATUS:
                onMetadataStatus((FirmwareMetadataStatusMessage) message.getStatusMessage());
                break;

            case CFG_MODEL_SUB_STATUS:
                onSubscriptionStatus((ModelSubscriptionStatusMessage) message.getStatusMessage());
                break;

            case FD_RECEIVERS_STATUS:
                onRevStatus((FDReceiversStatusMessage) message.getStatusMessage());
                break;

            case FD_UPLOAD_STATUS:
                onUploadStatus((FDUploadStatusMessage) message.getStatusMessage());
                break;

            case FD_STATUS:
                onFDStatus((FDStatusMessage) message.getStatusMessage());
                break;
        }
    }

    public void onInitiateCommandFailed(int opcode) {
        if ((step == STEP_DST_CAP_GET && opcode == Opcode.FD_CAPABILITIES_GET.value)
                || (step == STEP_DST_RECEIVERS_ADD && opcode == Opcode.FD_RECEIVERS_ADD.value)
                || (step == STEP_DST_UPLOAD_START && opcode == Opcode.FD_UPLOAD_STATUS.value
                || step == STEP_DST_START && opcode == Opcode.FD_START.value)) {
            // to distributor
            onInitComplete(false, "distribution command fail");
        } else if ((step == STEP_UPDATE_INFO_GET && opcode == Opcode.FIRMWARE_UPDATE_INFORMATION_GET.value)
                || (step == STEP_UPDATE_METADATA_CHECK && opcode == Opcode.FIRMWARE_UPDATE_FIRMWARE_METADATA_STATUS.value)
                || (step == STEP_SUB_SET && opcode == Opcode.CFG_MODEL_SUB_STATUS.value)) {
            onDeviceFail(updatingDevices.get(deviceIndex), "updating node init command fail");
            deviceIndex++;
            nextAction();
        } else if (step == STEP_BLOB_TRANSFER) {
            transfer.onTransferCommandFail(opcode);
        }
    }

    private void onFDStatus(FDStatusMessage statusMessage) {
        if (step != STEP_DST_START) {
            log("not at STEP_UPDATE_START");
            return;
        }
        if (statusMessage.status == DistributionStatus.SUCCESS.code) {
            // start distribute success, then the distributor distribute firmware data to updating nodes

            onInitComplete(true, "initiate distributor success");
        } else {
            DistributionStatus distributionStatus = DistributionStatus.valueOf(statusMessage.status);
            onInitComplete(false,
                    "initiate distributor fail: distribution status - " + distributionStatus.desc);
        }
    }

    /**
     * firmware distribute capability status
     */
    private void onCapStatus(FDCapabilitiesStatusMessage statusMessage) {
        if (step != STEP_DST_CAP_GET) {
            log("not at STEP_DST_CAP_GET");
            return;
        }
        // todo check capability
        step++;
        nextAction();
    }

    private void onFirmwareInfoStatus(FirmwareUpdateInfoStatusMessage firmwareInfoStatusMessage) {
        log("firmware info status: " + firmwareInfoStatusMessage.toString());
        if (step != STEP_UPDATE_INFO_GET) {
            log("not at STEP_GET_FIRMWARE_INFO");
            return;
        }

        int firstIndex = firmwareInfoStatusMessage.getFirstIndex();
        int companyId = firmwareInfoStatusMessage.getListCount();
        List<FirmwareUpdateInfoStatusMessage.FirmwareInformationEntry> firmwareInformationList
                = firmwareInfoStatusMessage.getFirmwareInformationList();
        deviceIndex++;
        nextAction();
    }

    private void onMetadataStatus(FirmwareMetadataStatusMessage metadataStatusMessage) {
        UpdateStatus status = UpdateStatus.valueOf(metadataStatusMessage.getStatus());
        if (step != STEP_UPDATE_METADATA_CHECK) {
            log("not at STEP_UPDATE_METADATA_CHECK");
            return;
        }
        if (status != UpdateStatus.SUCCESS) {
            onDeviceFail(updatingDevices.get(deviceIndex), "metadata check error: " + status.desc);
        } else {
            MeshUpdatingDevice device = updatingDevices.get(deviceIndex);
            device.additionalInformation = AdditionalInformation.valueOf(metadataStatusMessage.getAdditionalInformation());
            device.state = (MeshUpdatingDevice.STATE_METADATA_RSP);
            if (actionHandler != null) {
                actionHandler.onDeviceUpdate(device, "device metadata check complete");
            }
        }


        deviceIndex++;
        nextAction();
    }

    private void onSubscriptionStatus(ModelSubscriptionStatusMessage subscriptionStatusMessage) {
        log("subscription status: " + subscriptionStatusMessage.toString());
        if (step != STEP_SUB_SET) {
            log("not at STEP_SUB_SET");
            return;
        }
        if (subscriptionStatusMessage.getStatus() != ConfigStatus.SUCCESS.code) {
            onDeviceFail(updatingDevices.get(deviceIndex), "grouping status err " + subscriptionStatusMessage.getStatus());
        }
        deviceIndex++;
        nextAction();
    }

    private void onRevStatus(FDReceiversStatusMessage statusMessage) {
        if (step != STEP_DST_RECEIVERS_ADD) {
            log("not at STEP_DST_RECEIVERS_ADD");
            return;
        }
        step++;
        nextAction();
    }

    private void onUploadStatus(FDUploadStatusMessage uploadStatusMessage) {
        log("upload status: " + uploadStatusMessage.toString());
        if (step != STEP_DST_UPLOAD_START) {
            log("not at STEP_DST_UPLOAD_START");
            return;
        }
        step++;
        nextAction();
//        onInitComplete(true, "initiator action success");
    }


    /****************************************************************
     * {@link BlobTransferCallback} start
     ****************************************************************/

    @Override
    public void onTransferProgressUpdate(int progress, BlobTransferType transferType) {
        actionHandler.onTransferProgress(progress, transferType);
    }

    @Override
    public void onTransferStart(TransferMode transferMode) {
        actionHandler.onTransferStart(transferMode);
    }

    @Override
    public void onTransferComplete(boolean success, String desc) {
        log("initiator transfer complete: success?" + success);
        step++;
        nextAction();
    }

    /**
     * device fail
     *
     * @param address updating device address
     */
    @Override
    public void onTransferringDeviceFail(int address, String desc) {
        log(String.format("node updating fail: %04X -- " + desc, address));
        MeshUpdatingDevice device = getDeviceByAddress(address);
        if (device != null) {
            onDeviceFail(device, String.format("node updating fail: %04X -- ", device.meshAddress));
        }
    }

    @Override
    public boolean onMeshMessagePrepared(MeshMessage meshMessage) {
        MeshLogger.d("initiator send message to : " + meshMessage.getDestinationAddress());
        /*if (meshMessage.getDestinationAddress() != MeshUtils.LOCAL_MESSAGE_ADDRESS) {
            // for test
            nodeIndex++;
            nextAction();
            return true;
        }*/
        return this.actionHandler.onMessagePrepared(meshMessage);
    }

    @Override
    public void onTransferLog(String tag, String log, int logLevel) {
        this.actionHandler.onActionLog(tag, log, logLevel);
    }

    /****************************************************************
     * {@link BlobTransferCallback} end
     ****************************************************************/

    private void onDeviceFail(MeshUpdatingDevice device, String desc) {
        device.state = (MeshUpdatingDevice.STATE_FAIL);
        if (actionHandler != null) {
            actionHandler.onDeviceUpdate(device, desc);
        }
    }

    private void removeFailedDevices() {
        Iterator<MeshUpdatingDevice> iterator = updatingDevices.iterator();
        MeshUpdatingDevice updatingNode;
        while (iterator.hasNext()) {
            updatingNode = iterator.next();
            if (updatingNode.state == MeshUpdatingDevice.STATE_FAIL) {
                iterator.remove();
            }
        }
    }

    private MeshUpdatingDevice getDeviceByAddress(int address) {
        for (MeshUpdatingDevice device :
                this.updatingDevices) {
            if (device.meshAddress == address) return device;
        }
        return null;
    }

    private void log(String logInfo) {
        log(logInfo, MeshLogger.DEFAULT_LEVEL);
    }

    private void log(String logInfo, int logLevel) {
        actionHandler.onActionLog(LOG_TAG, logInfo, logLevel);
    }


}
