package com.telink.ble.mesh.core.access.fu;

import android.os.HandlerThread;

import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.core.message.NotificationMessage;
import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.firmwareupdate.FirmwareUpdateApplyMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.FirmwareUpdateGetMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.FirmwareUpdateStartMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.FirmwareUpdateStatusMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.UpdatePhase;
import com.telink.ble.mesh.core.message.firmwareupdate.UpdateStatus;
import com.telink.ble.mesh.entity.FirmwareUpdateConfiguration;
import com.telink.ble.mesh.entity.MeshUpdatingDevice;
import com.telink.ble.mesh.util.MeshLogger;

import java.util.Iterator;
import java.util.List;

/**
 * distribute by phone
 */
class FUDistributor implements BlobTransferCallback {

    private final String LOG_TAG = "FU-Distributor";

    /**
     * not in initiator processing
     */
    public static final int STEP_IDLE = 0;


    /**
     * update start
     */
    public static final int STEP_UPDATE_START = 2;


    /**
     * blob transfer start
     */
    public static final int STEP_BLOB_TRANSFER = 3;

    /**
     * update get
     */
    public static final int STEP_UPDATE_GET = 4;

    /**
     * update apply
     */
    public static final int STEP_UPDATE_APPLY = 5;

    public int step = STEP_IDLE;

    /**
     * params: update ttl
     */
    public static final byte UPDATE_TTL = (byte) 0xFF;

    /**
     * node address
     */
    private List<MeshUpdatingDevice> nodes;

    /**
     * app key index that updating model bound with
     */
    private int appKeyIndex = 0;

    /**
     * operation index
     */
    private int nodeIndex;

    private long blobId;

    /**
     * metadata
     */
    private byte[] metadata = new byte[8];

    /**
     * callback
     */
    private FUActionHandler actionHandler;

    private BlobTransfer transfer;

    FUDistributor(HandlerThread handlerThread, FUActionHandler actionHandler) {
        transfer = new BlobTransfer(handlerThread, this);
        this.actionHandler = actionHandler;
    }

    void begin(FirmwareUpdateConfiguration configuration, int connectedAddress) {
        this.appKeyIndex = configuration.getAppKeyIndex();

        log("begin - " + configuration.getUpdatingDevices().size());
        this.nodes = configuration.getUpdatingDevices();
        if (this.nodes.size() == 1 && this.nodes.get(0).meshAddress == connectedAddress) {
            // only direct connected node need update
            transfer.resetParams(configuration, BlobTransferType.GATT_DIST, connectedAddress);
        }else {
            // transfer by mesh
            transfer.resetParams(configuration, BlobTransferType.MESH_DIST, -1);
        }


        this.metadata = configuration.getMetadata();
        this.blobId = configuration.getBlobId();
        this.step = STEP_UPDATE_START;
        nodeIndex = 0;
        nextAction();
    }

    BlobTransfer getTransfer() {
        return transfer;
    }

    void clear() {
        if (this.step == STEP_BLOB_TRANSFER) {
            transfer.clear();
        }
        this.step = STEP_IDLE;
    }

    boolean isRunning() {
        return step != STEP_IDLE;
    }


    public void onDistributeCommandFailed(int opcode) {

        if ((step == STEP_UPDATE_START && opcode == Opcode.FIRMWARE_UPDATE_START.value)
                || (step == STEP_UPDATE_APPLY && opcode == Opcode.FIRMWARE_UPDATE_APPLY.value)) {
            // to updating nodes
            onDeviceFail(nodes.get(nodeIndex), "command send to updating node fail");
            nodeIndex++;
            nextAction();
        } else if (step == STEP_BLOB_TRANSFER) {
            transfer.onTransferCommandFail(opcode);
        }
    }

    /****************************************************************
     * {@link BlobTransferCallback} start
     ****************************************************************/

    @Override
    public void onTransferProgressUpdate(int progress, BlobTransferType transferType) {
        this.actionHandler.onTransferProgress(progress, transferType);
    }

    @Override
    public void onTransferComplete(boolean success, String desc) {
        log("distribute blob transfer complete : success?" + success);
        if (success) {
            nextStep();
        } else {
            onDistributeComplete(false, "blob transfer fail when distribute");
        }
    }

    @Override
    public void onTransferringDeviceFail(int address, String desc) {
        for (MeshUpdatingDevice device : nodes) {
            if (device.meshAddress == address) {
                onDeviceFail(device, desc);
                return;
            }
        }
    }

    @Override
    public boolean onMeshMessagePrepared(MeshMessage meshMessage) {
        return this.actionHandler.onMessagePrepared(meshMessage);
    }

    @Override
    public void onTransferLog(String tag, String log, int logLevel) {
        this.actionHandler.onActionLog(tag, log, logLevel);
    }

    /****************************************************************
     * {@link BlobTransferCallback} end
     ****************************************************************/

    private void onDistributeComplete(boolean success, String desc) {
        if (success) {
            actionHandler.onTransferProgress(100, BlobTransferType.MESH_DIST);
        }
        log("distribute complete : success ? " + success + " -- " + desc);
        this.step = STEP_IDLE;
        actionHandler.onActionComplete(success, FUAction.DISTRIBUTE, desc);
    }

    /**
     * process distribute actions step by step
     */
    private void nextStep() {
        step++;
        log("next step: " + step);
        nodeIndex = 0;
        nextAction();
    }

    /**
     * execute distribute actions(one by one && step by step)
     * one device by one device
     * if all devices executed, then next step
     */
    // draft feature
    private void nextAction() {

        if (nodeIndex >= nodes.size()) {
            // all nodes executed

            log("current step complete: " + (step));
            if (step == STEP_UPDATE_APPLY) {
                onDistributeComplete(true, "update apply complete");
                return;
            }

            removeFailedDevices();

            // check if has available nodes
            if (nodes.size() != 0) {
                nextStep();
            } else {
                onDistributeComplete(false, "all nodes failed when executing action");
            }
        } else {
            int meshAddress = nodes.get(nodeIndex).meshAddress;
            log(String.format("action executing: " + (step) + " -- %04X", meshAddress));
            if (this.step == STEP_UPDATE_START) {
                FirmwareUpdateStartMessage updateStartMessage = new FirmwareUpdateStartMessage(meshAddress, appKeyIndex);
                updateStartMessage.metadata = metadata;
                updateStartMessage.updateBLOBID = blobId;
                updateStartMessage.updateTtl = UPDATE_TTL;
                updateStartMessage.updateTimeoutBase = 0;
                updateStartMessage.updateFirmwareImageIndex = 0;
                onMeshMessagePrepared(updateStartMessage);
            } else if (this.step == STEP_UPDATE_GET) {
                FirmwareUpdateGetMessage getMessage = FirmwareUpdateGetMessage.getSimple(meshAddress, appKeyIndex);
                onMeshMessagePrepared(getMessage);
            } else if (this.step == STEP_UPDATE_APPLY) {
                FirmwareUpdateApplyMessage applyMessage = FirmwareUpdateApplyMessage.getSimple(meshAddress, appKeyIndex);
                onMeshMessagePrepared(applyMessage);
            } else if (this.step == STEP_BLOB_TRANSFER) {
                log("blob transfer start");
                transfer.begin();
            }
        }

    }

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
        if (opcode == null) return;
        final int src = message.getSrc();

        // message received from updating devices
        if (step == STEP_UPDATE_START) {
            if (nodes.size() <= nodeIndex) {
                log("node index overflow");
                return;
            }

            if (nodes.get(nodeIndex).meshAddress != src) {
                log("unexpected notification src");
                return;
            }
        }
        if ((opcode == Opcode.FIRMWARE_UPDATE_STATUS)
        ) {
            onFirmwareUpdateStatus((FirmwareUpdateStatusMessage) message.getStatusMessage());
        } else if (step == STEP_BLOB_TRANSFER) {
            transfer.onTransferNotification(message);
        }
    }

    private void onDeviceApplySuccess(MeshUpdatingDevice device) {
        log("device apply success");
        // should wait for recheck firmware
//        device.state = (MeshUpdatingDevice.STATE_SUCCESS);
//        actionHandler.onDeviceUpdate(device, "device update success");
    }


    /**
     * response of {@link FirmwareUpdateStartMessage} or {@link FirmwareUpdateApplyMessage}
     *
     * @param firmwareUpdateStatusMessage status
     */
    private void onFirmwareUpdateStatus(FirmwareUpdateStatusMessage firmwareUpdateStatusMessage) {
        log("firmware info status: " + firmwareUpdateStatusMessage.toString());
        final UpdateStatus status = UpdateStatus.valueOf(firmwareUpdateStatusMessage.getStatus() & 0xFF);

        if (status != UpdateStatus.SUCCESS) {
            onDeviceFail(nodes.get(nodeIndex), "firmware update status err");
        } else {
            final int step = this.step;
            /*boolean pass = (step == STEP_UPDATE_START && phase == FirmwareUpdateStatusMessage.PHASE_IN_PROGRESS)
                    || (step == STEP_UPDATE_GET && phase == FirmwareUpdateStatusMessage.PHASE_READY)
                    || (step == STEP_UPDATE_APPLY && phase == FirmwareUpdateStatusMessage.PHASE_IDLE);*/
            boolean pass = true;
            if (!pass) {
                onDeviceFail(nodes.get(nodeIndex), "firmware update phase err");
            } else {
                final UpdatePhase phase = UpdatePhase.valueOf(firmwareUpdateStatusMessage.getPhase() & 0xFF);
                if (step == STEP_UPDATE_APPLY) {
                    if (phase == UpdatePhase.VERIFICATION_SUCCESS
                            || phase == UpdatePhase.APPLYING_UPDATE) {
                        onDeviceApplySuccess(nodes.get(nodeIndex));
                    } else {
                        onDeviceFail(nodes.get(nodeIndex), "phase error when update apply");
                    }
                }
            }
        }
        nodeIndex++;
        nextAction();
    }


    /**
     * device fail
     *
     * @param device updating device
     */
    private void onDeviceFail(MeshUpdatingDevice device, String desc) {
        log(String.format("node updating fail: %04X -- " + desc, device.meshAddress));
        device.state = (MeshUpdatingDevice.STATE_FAIL);
        if (actionHandler != null) {
            actionHandler.onDeviceUpdate(device, String.format("node updating fail: %04X -- ", device.meshAddress));
        }
    }

    private void removeFailedDevices() {
        Iterator<MeshUpdatingDevice> iterator = nodes.iterator();
        MeshUpdatingDevice updatingNode;
        while (iterator.hasNext()) {
            updatingNode = iterator.next();
            if (updatingNode.state == MeshUpdatingDevice.STATE_FAIL) {
                iterator.remove();
            }
        }
    }


    private void log(String logInfo) {
        MeshLogger.log(logInfo, LOG_TAG, MeshLogger.LEVEL_DEBUG);
    }

}
