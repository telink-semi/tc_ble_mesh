package com.telink.ble.mesh.core.access;

import android.os.Handler;
import android.os.HandlerThread;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.core.message.MeshSigModel;
import com.telink.ble.mesh.core.message.NotificationMessage;
import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.config.ConfigMessage;
import com.telink.ble.mesh.core.message.config.ModelSubscriptionSetMessage;
import com.telink.ble.mesh.core.message.config.ModelSubscriptionStatusMessage;
import com.telink.ble.mesh.core.message.updating.FirmwareInfoGetMessage;
import com.telink.ble.mesh.core.message.updating.FirmwareInfoStatusMessage;
import com.telink.ble.mesh.core.message.updating.FirmwareUpdateAbortMessage;
import com.telink.ble.mesh.core.message.updating.FirmwareUpdateApplyMessage;
import com.telink.ble.mesh.core.message.updating.FirmwareUpdateGetMessage;
import com.telink.ble.mesh.core.message.updating.FirmwareUpdatePrepareMessage;
import com.telink.ble.mesh.core.message.updating.FirmwareUpdateStartMessage;
import com.telink.ble.mesh.core.message.updating.FirmwareUpdateStatusMessage;
import com.telink.ble.mesh.core.message.updating.ObjectBlockGetMessage;
import com.telink.ble.mesh.core.message.updating.ObjectBlockStatusMessage;
import com.telink.ble.mesh.core.message.updating.ObjectBlockTransferStartMessage;
import com.telink.ble.mesh.core.message.updating.ObjectBlockTransferStatusMessage;
import com.telink.ble.mesh.core.message.updating.ObjectChunkTransferMessage;
import com.telink.ble.mesh.core.message.updating.ObjectInfoGetMessage;
import com.telink.ble.mesh.core.message.updating.ObjectInfoStatusMessage;
import com.telink.ble.mesh.core.message.updating.ObjectTransferStartMessage;
import com.telink.ble.mesh.core.message.updating.ObjectTransferStatusMessage;
import com.telink.ble.mesh.entity.MeshUpdatingConfiguration;
import com.telink.ble.mesh.entity.MeshUpdatingDevice;
import com.telink.ble.mesh.util.MeshLogger;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Locale;



/**
 * Mesh firmware updating
 * (Mesh OTA)
 * Created by kee on 2019/9/26.
 */
public class MeshUpdatingController {

    private final String LOG_TAG = "MeshUpdating";

    /**
     * all complete, fail or success
     */
    public static final int STATE_SUCCESS = 0x00;

    /**
     * chunks sending progress
     */
    public static final int STATE_PROGRESS = 0x01;

    /**
     * device succeed when apply status success
     */
    public static final int STATE_DEVICE_SUCCESS = 0x02;

    /**
     * device failed at every step status err
     */
    public static final int STATE_DEVICE_FAIL = 0x03;

    /**
     * mesh updating flow failed
     */
    public static final int STATE_FAIL = 0x04;


    /**
     * params check err
     */
    public static final int STATE_STOPPED = 0x05;

    /**
     * get firmware info err
     */
//    public static final int STATE_ACTION_EXECUTE_FAIL = 0x05;

    /**
     * initial
     */
    private static final int STEP_INITIAL = 0x00;

    /**
     * get firmware information
     */
    private static final int STEP_GET_FIRMWARE_INFO = 0x01;

    /**
     * set subscription at firmware-updating-model
     */
    private static final int STEP_SET_SUBSCRIPTION = 0x02;

    /**
     * get object info
     */
    private static final int STEP_GET_OBJECT_INFO = 0x03;

    /**
     * firmware updating prepare
     */
    private static final int STEP_UPDATE_PREPARE = 0x04;

    /**
     * firmware update start
     */
    private static final int STEP_UPDATE_START = 0x05;

    /**
     * object transfer start
     */
    private static final int STEP_OBJECT_TRANSFER_START = 0x06;

    /**
     * object block transfer start
     */
    private static final int STEP_OBJECT_BLOCK_TRANSFER_START = 0x07;

    /**
     * sending chunk data
     */
    private static final int STEP_OBJECT_CHUNK_SENDING = 0x08;

    /**
     * get block
     *
     * @see Opcode#OBJ_BLOCK_STATUS
     */
    private static final int STEP_GET_OBJECT_BLOCK = 0x09;

    /**
     * get firmware update after block sent complete
     */
    private static final int STEP_UPDATE_GET = 0x0A;

    /**
     * firmware update apply
     */
    private static final int STEP_UPDATE_APPLY = 0x0B;

    /**
     * all complete
     */
    private static final int STEP_UPDATE_COMPLETE = 0x0C;

    /**
     * manual stopping mesh updating flow
     */
    private static final int STEP_UPDATE_ABORTING = -0x10;

    /**
     * step
     */
    private int step = STEP_INITIAL;

    /**
     * node address | ignore
     */
    private List<MeshUpdatingDevice> nodes;

//    private int[] nodeAddresses;

    private int groupAddress;

    private int appKeyIndex;

    private int nodeIndex;

    private int companyId;

    private int firmwareId;

    private long objectId;

    private MeshFirmwareParser firmwareParser = new MeshFirmwareParser();

    /**
     * received missing chunk number
     */
    private ArrayList<Integer> missingChunks = new ArrayList<>();

    private int missingChunkIndex = 0;

    private Handler delayHandler;

    private AccessBridge accessBridge;

    public MeshUpdatingController(HandlerThread handlerThread) {
        delayHandler = new Handler(handlerThread.getLooper());
    }

    public void register(AccessBridge accessBridge) {
        this.accessBridge = accessBridge;
    }

    /**
     * ignore distribution start action
     * <p>
     * 1. get all nodes's firmware information ->
     * 2. subscribe target group address
     * 3. distribution start (ignore)
     * 4. send block
     */
    public void begin(MeshUpdatingConfiguration configuration) {
        if (configuration == null) {
            onUpdatingFail(STATE_FAIL, "updating params null");
            return;
        }
        firmwareParser.reset(configuration.getFirmwareData());
        log(" config -- " + configuration.toString());
        this.appKeyIndex = configuration.getAppKeyIndex();
        this.groupAddress = configuration.getGroupAddress();
        this.nodes = configuration.getUpdatingDevices();
        this.companyId = configuration.getCompanyId();
        this.firmwareId = configuration.getFirmwareId();
        this.objectId = configuration.getObjectId();
        this.nodeIndex = 0;
        if (nodes != null && nodes.size() != 0) {
            this.step = STEP_GET_FIRMWARE_INFO;
            executeUpdatingAction();
        } else {
            onUpdatingFail(STATE_FAIL, "params err when action begin");
        }
    }

    public void clear() {
        if (delayHandler != null) {
            delayHandler.removeCallbacksAndMessages(null);
        }
        step = STEP_INITIAL;
    }

    public void stop() {
        if (step == STEP_INITIAL) {
            log("mesh updating not running");
        } else {
            delayHandler.removeCallbacksAndMessages(null);
            delayHandler.postDelayed(stoppingCheckTask, 5 * 1000);
            step = STEP_UPDATE_ABORTING;
            FirmwareUpdateAbortMessage abortMessage = FirmwareUpdateAbortMessage.getSimple(0xFFFF,
                    appKeyIndex, nodes.size(), companyId, firmwareId);
            onMeshMessagePrepared(abortMessage);
        }
    }

    private Runnable stoppingCheckTask = new Runnable() {
        @Override
        public void run() {
            onUpdatingStopped();
        }
    };


    private void sendChunks() {
        byte[] chunkData = firmwareParser.nextChunk();
        if (chunkData != null) {
            validateUpdatingProgress();
            int chunkNumber = firmwareParser.currentChunkIndex();
            ObjectChunkTransferMessage objectChunkTransferMessage = generateChunkTransferMessage(chunkNumber, chunkData);

            log("next chunk transfer msg: " + objectChunkTransferMessage.toString());

            onMeshMessagePrepared(objectChunkTransferMessage);

            delayHandler.postDelayed(chunkSendingTask, getChunkSendingInterval());
        } else {
            log("chunks sent complete at: block -- " + firmwareParser.currentBlockIndex()
                    + " chunk -- " + firmwareParser.currentChunkIndex());
            checkMissingChunks();
        }
    }


    private void checkMissingChunks() {
        log("check missing chunks");
        missingChunks.clear();
        step = STEP_GET_OBJECT_BLOCK;
        nodeIndex = 0;
        executeUpdatingAction();
    }


    private void resendMissingChunks() {
        if (missingChunkIndex >= missingChunks.size()) {
            // all missing chunk sent
            log("all missing chunks sent complete: " + missingChunkIndex);
            checkMissingChunks();
        } else {
            int chunkNumber = missingChunks.get(missingChunkIndex);
            byte[] chunkData = firmwareParser.chunkAt(chunkNumber);
            ObjectChunkTransferMessage objectChunkTransferMessage = generateChunkTransferMessage(chunkNumber, chunkData);
            onMeshMessagePrepared(objectChunkTransferMessage);
            delayHandler.postDelayed(missingChunkSendingTask, getChunkSendingInterval());
        }
    }

    private int getChunkSendingInterval() {
        // relay 320 ms
        int interval = firmwareParser.getChunkSize() / 12 * 320;
        final int min = 5 * 1000;
        interval = (interval < min ? min : interval);
        log("resending interval: " + interval);
        return interval;
    }

    private Runnable missingChunkSendingTask = new Runnable() {
        @Override
        public void run() {
            missingChunkIndex++;
            resendMissingChunks();
        }
    };

    private Runnable chunkSendingTask = new Runnable() {
        @Override
        public void run() {
            sendChunks();
        }
    };

    private ObjectChunkTransferMessage generateChunkTransferMessage(int chunkNumber, byte[] chunkData) {
        return ObjectChunkTransferMessage.getSimple(groupAddress, appKeyIndex, chunkNumber, chunkData);
    }


    /**
     * execute updating actions(one by one && step by step)
     * one device by one device
     * if all devices executed, then next step
     */
    private void executeUpdatingAction() {
        log("action: " + getStepDesc(step) + " -- node index -- " + nodeIndex);
        if (nodeIndex >= nodes.size()) {
            // all nodes executed

            log("current step complete: " + getStepDesc(step));

            removeFailedDevices();

            // check if has available nodes
            if (nodes.size() != 0) {
                nodeIndex = 0;

                if (step == STEP_GET_OBJECT_BLOCK) {
                    // check if has missing chunk
                    if (missingChunks.size() == 0) {
                        // no missing chunks
                        step = STEP_OBJECT_BLOCK_TRANSFER_START;
                        executeUpdatingAction();
                    } else {
                        // resend missing chunks
                        missingChunkIndex = 0;
                        resendMissingChunks();
                    }
                } else {
                    log("next step: " + getStepDesc(step + 1));
                    step++;
                    executeUpdatingAction();
                }
            } else {
                onUpdatingFail(STATE_FAIL, "all node failed when executing action");
            }
        } else {
            int meshAddress = nodes.get(nodeIndex).getMeshAddress();
            log(String.format("action executing: " + getStepDesc(step) + " -- %04X", meshAddress));
            switch (this.step) {
                case STEP_GET_FIRMWARE_INFO:
                    onMeshMessagePrepared(FirmwareInfoGetMessage.getSimple(meshAddress, appKeyIndex, 1));
                    break;

                case STEP_SET_SUBSCRIPTION:
                    int eleAdr = nodes.get(nodeIndex).getUpdatingEleAddress();
                    int modelId = MeshSigModel.SIG_MD_OBJ_TRANSFER_S.modelId;
                    onMeshMessagePrepared(ModelSubscriptionSetMessage.getSimple(meshAddress,
                            ModelSubscriptionSetMessage.MODE_ADD,
                            eleAdr, groupAddress, modelId, true));
                    break;


                case STEP_GET_OBJECT_INFO:
                    onMeshMessagePrepared(ObjectInfoGetMessage.getSimple(meshAddress, appKeyIndex, 1));
                    break;

                case STEP_UPDATE_PREPARE:
                    onMeshMessagePrepared(FirmwareUpdatePrepareMessage.getSimple(meshAddress,
                            appKeyIndex, 1, companyId, firmwareId, objectId, null));
                    break;

                case STEP_UPDATE_START:
                    onMeshMessagePrepared(FirmwareUpdateStartMessage.getSimple(meshAddress, appKeyIndex, 1,
                            FirmwareUpdateStartMessage.POLICY_NONE, companyId, firmwareId));
                    break;

                case STEP_OBJECT_TRANSFER_START:

                    int objectSize = firmwareParser.getObjectSize();
                    byte blockSizeLog = (byte) MeshUtils.mathLog2(firmwareParser.getBlockSize());
                    onMeshMessagePrepared(ObjectTransferStartMessage.getSimple(meshAddress, appKeyIndex, 1,
                            objectId, objectSize, blockSizeLog));
                    break;

                case STEP_OBJECT_BLOCK_TRANSFER_START:
                    if (nodeIndex == 0) {
                        if (firmwareParser.hasNextBlock()) {
                            firmwareParser.nextBlock();
                        } else {
                            log("all blocks sent complete at: block -- " + firmwareParser.currentBlockIndex());
                            step = STEP_UPDATE_GET;
                            executeUpdatingAction();
                            return;
                        }
                    }
                    int blockNumber = firmwareParser.currentBlockIndex();
                    int chunkSize = firmwareParser.getChunkSize();
                    int blockChecksumValue = firmwareParser.getBlockChecksum();
                    int currentBlockSize = firmwareParser.getCurBlockSize();
                    onMeshMessagePrepared(ObjectBlockTransferStartMessage.getSimple(meshAddress, appKeyIndex, 1,
                            objectId, blockNumber,
                            chunkSize,
                            blockChecksumValue, currentBlockSize));

                    break;

                case STEP_OBJECT_CHUNK_SENDING:
                    sendChunks();
                    break;

                case STEP_GET_OBJECT_BLOCK:
                    onMeshMessagePrepared(ObjectBlockGetMessage.getSimple(meshAddress, appKeyIndex, 1,
                            objectId, firmwareParser.currentBlockIndex()));
                    break;

                case STEP_UPDATE_GET:

                    onMeshMessagePrepared(FirmwareUpdateGetMessage.getSimple(meshAddress,
                            appKeyIndex,
                            1,
                            companyId,
                            firmwareId));
                    break;

                case STEP_UPDATE_APPLY:
                    onMeshMessagePrepared(FirmwareUpdateApplyMessage.getSimple(meshAddress,
                            appKeyIndex,
                            1,
                            companyId,
                            firmwareId));
                    break;

                case STEP_UPDATE_COMPLETE:
                    onUpdatingSuccess();
                    break;
                default:
                    break;
            }
        }


    }


    private void removeFailedDevices() {
        Iterator<MeshUpdatingDevice> iterator = nodes.iterator();
        MeshUpdatingDevice updatingNode;
        while (iterator.hasNext()) {
            updatingNode = iterator.next();
            if (updatingNode.getState() == MeshUpdatingDevice.STATE_FAIL) {
                iterator.remove();
            }
        }
    }

    private void onMeshMessagePrepared(MeshMessage meshMessage) {
        log("mesh message prepared: " + meshMessage.getClass().getSimpleName()
                + String.format(" opcode: 0x%04X -- dst: 0x%04X", meshMessage.getOpcode(), meshMessage.getDestinationAddress()));
        if (accessBridge != null) {
            boolean isMessageSent = accessBridge.onAccessMessagePrepared(meshMessage, AccessBridge.MODE_FIRMWARE_UPDATING);
            if (!isMessageSent) {
                onUpdatingFail(-1, "updating message sent error");
            }
        }
    }

    public void onMessageNotification(NotificationMessage message) {
        Opcode opcode = Opcode.valueOf(message.getOpcode());
        log("message notification: " + opcode);
        if (step == STEP_INITIAL) {
            log("notification when idle");
            return;
        }
        if (opcode == null) return;
        switch (opcode) {

            case FW_INFO_STATUS:
                onFirmwareInfoStatus((FirmwareInfoStatusMessage) message.getStatusMessage());
                break;

            case CFG_MODEL_SUB_STATUS:
                onSubscriptionStatus((ModelSubscriptionStatusMessage) message.getStatusMessage());
                break;

            case OBJ_INFO_STATUS:
                onObjectInfoStatus((ObjectInfoStatusMessage) message.getStatusMessage());
                break;

            case FW_UPDATE_STATUS:
                onFirmwareUpdateStatus((FirmwareUpdateStatusMessage) message.getStatusMessage());
                break;

            case OBJ_TRANSFER_STATUS:
                onTransferStatus((ObjectTransferStatusMessage) message.getStatusMessage());
                break;


            case OBJ_BLOCK_TRANSFER_STATUS:
                onBlockTransferStatus((ObjectBlockTransferStatusMessage) message.getStatusMessage());
                break;

            case OBJ_BLOCK_STATUS:
                onBlockStatus(message);
                break;
        }
    }

    private void onFirmwareInfoStatus(FirmwareInfoStatusMessage firmwareInfoStatusMessage) {
        // todo, ignore company id / firmware id checking
        log("firmware info status: " + firmwareInfoStatusMessage.toString());
        int firmwareId = firmwareInfoStatusMessage.getFirmwareId();
        int companyId = firmwareInfoStatusMessage.getCompanyId();
        nodeIndex++;
        executeUpdatingAction();
    }

    private void onSubscriptionStatus(ModelSubscriptionStatusMessage subscriptionStatusMessage) {
        log("subscription status: " + subscriptionStatusMessage.toString());
        if (subscriptionStatusMessage.getStatus() != ConfigMessage.STATUS_SUCCESS) {
            onDeviceFail(nodes.get(nodeIndex), "grouping status err " + subscriptionStatusMessage.getStatus());
        }
        nodeIndex++;
        executeUpdatingAction();
    }


    /**
     * response of {@link ObjectInfoGetMessage}
     */
    private void onObjectInfoStatus(ObjectInfoStatusMessage objectInfoStatusMessage) {
        log("object info status: " + objectInfoStatusMessage.toString());
        // todo ignore at present
        nodeIndex++;
        executeUpdatingAction();
    }

    /**
     * response of {@link FirmwareUpdateStartMessage}
     * , {@link FirmwareUpdatePrepareMessage}, {@link FirmwareUpdateGetMessage}
     */
    private void onFirmwareUpdateStatus(FirmwareUpdateStatusMessage firmwareUpdateStatusMessage) {

        log("firmware update status: " + " at: " + getStepDesc(step)
                + " -- " + firmwareUpdateStatusMessage.toString());
        byte status = firmwareUpdateStatusMessage.getStatus();
        if (status != FirmwareUpdateStatusMessage.STATUS_SUCCESS) {
            onDeviceFail(nodes.get(nodeIndex), "firmware update status err");
        } else {
            final byte phase = firmwareUpdateStatusMessage.getPhase();
            final int step = this.step;
            boolean pass = (step == STEP_UPDATE_PREPARE && phase == FirmwareUpdateStatusMessage.PHASE_PREPARED)
                    || (step == STEP_UPDATE_START && phase == FirmwareUpdateStatusMessage.PHASE_IN_PROGRESS)
                    || (step == STEP_UPDATE_GET && phase == FirmwareUpdateStatusMessage.PHASE_READY)
                    || (step == STEP_UPDATE_APPLY && phase == FirmwareUpdateStatusMessage.PHASE_IDLE);

            if (!pass) {
                onDeviceFail(nodes.get(nodeIndex), "firmware update phase err");
            } else {
                if (step == STEP_UPDATE_APPLY) {
                    onDeviceSuccess(nodes.get(nodeIndex));
                }
            }
            nodeIndex++;
            executeUpdatingAction();
        }

    }

    /**
     * response of {@link ObjectTransferStartMessage}
     */
    private void onTransferStatus(ObjectTransferStatusMessage transferStatusMessage) {
        log("object transfer status: " + transferStatusMessage.toString());
        byte status = transferStatusMessage.getStatus();
        if (status != ObjectTransferStatusMessage.STATUS_READY
                && status != ObjectTransferStatusMessage.STATUS_BUSY_ACTIVE) {
            onDeviceFail(nodes.get(nodeIndex), "object transfer status err");
        }
        nodeIndex++;
        executeUpdatingAction();
    }

    /**
     * response of {@link ObjectBlockTransferStartMessage} before start chunks sending
     */
    private void onBlockTransferStatus(ObjectBlockTransferStatusMessage blockTransferStatusMessage) {
        log("block transfer status: " + blockTransferStatusMessage.toString());
        byte status = blockTransferStatusMessage.getStatus();
        if (status != ObjectBlockTransferStatusMessage.STATUS_ACCEPTED) {
            onDeviceFail(nodes.get(nodeIndex), "object block transfer status err");
        }
        nodeIndex++;
        executeUpdatingAction();
    }

    /**
     * response of BLOCK_GET
     * {@link #checkMissingChunks()}
     */
    private void onBlockStatus(NotificationMessage message) {
        ObjectBlockStatusMessage objectBlockStatusMessage = (ObjectBlockStatusMessage) message.getStatusMessage();
        log("block status: " + objectBlockStatusMessage.toString());
        int srcAddress = message.getSrc();
        switch (objectBlockStatusMessage.getStatus()) {
            case ObjectBlockStatusMessage.STATUS_ALL_CHUNKS_RECEIVED:
                // all chunks data received
                log("no chunks missing");
                nodeIndex++;
                executeUpdatingAction();
                break;

            case ObjectBlockStatusMessage.STATUS_NOT_ALL_CHUNKS_RECEIVED:
                // chunk missing
                int[] missingChunksList = objectBlockStatusMessage.getMissingChunksList();
                if (missingChunksList != null) {
                    for (int chunkNum : missingChunksList) {
                        missingChunks.add(chunkNum);
                    }
                    nodeIndex++;
                    executeUpdatingAction();
                } else {
                    log("missing chunk data not found at status: " + objectBlockStatusMessage.getStatus());
                }
                break;

            case ObjectBlockStatusMessage.STATUS_WRONG_CHECKSUM:
            case ObjectBlockStatusMessage.STATUS_WRONG_OBJECT_ID:
            case ObjectBlockStatusMessage.STATUS_WRONG_BLOCK:
                // err
                MeshUpdatingDevice device = getDeviceByAddress(srcAddress);
                if (device != null) {
                    onDeviceFail(device, "block status err");
                } else {
                    log(String.format("device not found , mesh address: %04X", srcAddress));
                }
                break;

        }
    }


    /**
     * reliable command complete
     */
    public void onUpdatingCommandComplete(boolean success, int opcode, int rspMax, int rspCount) {
        log(String.format("updating command complete: opcode-%04X success?" + success, opcode));
        if (!success) {
            // command timeout
            final boolean deviceFailed = (opcode == Opcode.FW_INFO_GET.value && step == STEP_GET_FIRMWARE_INFO)
                    || (opcode == Opcode.CFG_MODEL_SUB_ADD.value && step == STEP_SET_SUBSCRIPTION)
                    || (opcode == Opcode.OBJ_INFO_GET.value && step == STEP_GET_OBJECT_INFO)
                    || (opcode == Opcode.FW_UPDATE_PREPARE.value && step == STEP_UPDATE_PREPARE)
                    || (opcode == Opcode.FW_UPDATE_START.value && step == STEP_UPDATE_START)
                    || (opcode == Opcode.OBJ_TRANSFER_START.value && step == STEP_OBJECT_TRANSFER_START)
                    || (opcode == Opcode.OBJ_BLOCK_TRANSFER_START.value && step == STEP_OBJECT_BLOCK_TRANSFER_START)
                    || (opcode == Opcode.OBJ_BLOCK_GET.value && step == STEP_GET_OBJECT_BLOCK)
                    || (opcode == Opcode.FW_UPDATE_GET.value && step == STEP_UPDATE_GET)
                    || (opcode == Opcode.FW_UPDATE_APPLY.value && step == STEP_UPDATE_APPLY);
            if (deviceFailed) {
                String desc = String.format(Locale.getDefault(), "device failed at step: %02d when sending: 0x%04X", step, opcode);
                onDeviceFail(nodes.get(nodeIndex), desc);
                nodeIndex++;
                executeUpdatingAction();
            }
        }
    }

    private void validateUpdatingProgress() {
        if (firmwareParser.validateProgress()) {
            onStateUpdate(STATE_PROGRESS, "progress update", firmwareParser.getProgress());
        }
    }

    private MeshUpdatingDevice getDeviceByAddress(int meshAddress) {
        for (MeshUpdatingDevice device : nodes) {
            if (device.getMeshAddress() == meshAddress) return device;
        }
        return null;
    }


    private void onDeviceFail(MeshUpdatingDevice device, String desc) {
        log(String.format("node updating fail: %04X -- " + desc, device.getMeshAddress()));
        device.setState(MeshUpdatingDevice.STATE_FAIL);
        onStateUpdate(STATE_DEVICE_FAIL,
                String.format("node updating fail: %04X -- ", device.getMeshAddress()),
                device);
    }

    private void onDeviceSuccess(MeshUpdatingDevice device) {
        log(String.format("node updating success: %04X -- ", device.getMeshAddress()));
        device.setState(MeshUpdatingDevice.STATE_SUCCESS);
        onStateUpdate(STATE_DEVICE_SUCCESS,
                String.format("node updating success: %04X -- ", device.getMeshAddress()),
                device);
    }

    /**
     * at least one device success
     */
    private void onUpdatingSuccess() {
        log("updating complete");
        this.step = STEP_INITIAL;
        onStateUpdate(STATE_SUCCESS, "updating success", null);
    }

    /**
     * no device success
     */
    private void onUpdatingFail(int state, String desc) {
        log("updating failed: " + state + " -- " + desc);
        this.step = STEP_INITIAL;
        onStateUpdate(state, desc, null);
    }


    public void onUpdatingStopped() {
        log("updating stopped");
        this.step = STEP_INITIAL;
        onStateUpdate(STATE_STOPPED, "updating stopped", null);
    }

    private void onStateUpdate(int state, String desc, Object obj) {
        if (accessBridge != null) {
            accessBridge.onAccessStateChanged(state, desc, AccessBridge.MODE_FIRMWARE_UPDATING, obj);
        }
    }


    /**
     * get step description
     */
    private String getStepDesc(int step) {
        switch (step) {

            case STEP_INITIAL:
                return "initial";

            case STEP_GET_FIRMWARE_INFO:
                return "get-firmware-info";

            case STEP_SET_SUBSCRIPTION:
                return "set-subscription";

            case STEP_GET_OBJECT_INFO:
                return "get-object-info";

            case STEP_UPDATE_PREPARE:
                return "update-prepare";

            case STEP_UPDATE_START:
                return "update-start";

            case STEP_OBJECT_TRANSFER_START:
                return "object-transfer-start";

            case STEP_OBJECT_BLOCK_TRANSFER_START:
                return "block-transfer-start";

            case STEP_OBJECT_CHUNK_SENDING:
                return "object-chunk-sending";

            case STEP_GET_OBJECT_BLOCK:
                return "get-object-block";

            case STEP_UPDATE_GET:
                return "update-get";

            case STEP_UPDATE_APPLY:
                return "update-apply";

            case STEP_UPDATE_COMPLETE:
                return "update-complete";
        }
        return "unknown";
    }

    private void log(String logMessage) {
        log(logMessage, MeshLogger.LEVEL_DEBUG);
    }

    private void log(String logMessage, int level) {
        MeshLogger.log(logMessage, LOG_TAG, level);
    }


}
