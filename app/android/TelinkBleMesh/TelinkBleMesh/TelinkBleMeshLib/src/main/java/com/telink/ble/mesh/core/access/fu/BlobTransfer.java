package com.telink.ble.mesh.core.access.fu;

import android.os.Handler;
import android.os.HandlerThread;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.access.MeshFirmwareParser;
import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.core.message.NotificationMessage;
import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.firmwareupdate.UpdateStatus;
import com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer.BlobBlockGetMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer.BlobBlockStartMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer.BlobBlockStatusMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer.BlobChunkTransferMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer.BlobInfoGetMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer.BlobInfoStatusMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer.BlobPartialBlockReportMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer.BlobTransferGetMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer.BlobTransferStartMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer.BlobTransferStatusMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer.TransferMode;
import com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer.TransferStatus;
import com.telink.ble.mesh.core.networking.NetworkingController;
import com.telink.ble.mesh.entity.FirmwareUpdateConfiguration;
import com.telink.ble.mesh.entity.MeshUpdatingDevice;
import com.telink.ble.mesh.util.MeshLogger;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

/**
 * used in Firmware Update Flow (MeshOTA)
 * initiator send blob to distributor
 * distributor send blob to initiator
 */
class BlobTransfer {

    private final String LOG_TAG = "BlobTransfer";


    /**
     * blob transfer get
     */
    private static final int STEP_IDLE = 0;

    /**
     * blob transfer get
     */
    private static final int STEP_BLOB_TRANSFER_GET = 5;

    /**
     * get blob info
     */
    private static final int STEP_GET_BLOB_INFO = 6;

    /**
     * blob transfer start
     */
    private static final int STEP_BLOB_TRANSFER_START = 7;

    /**
     * blob block transfer start
     */
    private static final int STEP_BLOB_BLOCK_START = 8;

    /**
     * sending chunk data
     */
    private static final int STEP_BLOB_CHUNK_SENDING = 9;

    /**
     * get block
     *
     * @see Opcode#BLOB_BLOCK_STATUS
     */
    private static final int STEP_GET_BLOB_BLOCK = 10;

    private int step = STEP_IDLE;

    /**
     * operation index
     */
    private int nodeIndex;

    /**
     * received missing chunk number
     */
    private ArrayList<Integer> missingChunks = new ArrayList<>();

    /**
     * direction of blob sending
     */
    private BlobTransferType transferType = BlobTransferType.LOCAL_INIT;


    private Handler delayHandler;
    private MeshFirmwareParser firmwareParser = new MeshFirmwareParser();

    /**
     * transfer target devices
     */
    List<TargetDevice> targetDevices = new ArrayList<>();
    //    List<MeshUpdatingDevice> deviceList;
    byte[] firmwareData;

    /**
     * sending missing chunk
     */
    private int missingChunkIndex = 0;

    /**
     * mix format : mix all format received after checkMissingChunks {@link #checkMissingChunks()}
     */
    private int mixFormat = -1;

    private int dleLength = 11;

    private BlobTransferCallback transferCallback;

    private long blobId;

    private int appKeyIndex;

    private int groupAddress;

    /**
     * max missing chunk number in each block for progress calculate
     */
    private int pullMaxChunkNumber;

    private int pullProgress = -1;

    /**
     * last
     */
    private TransferMode transferMode = TransferMode.PUSH;

    public BlobTransfer(HandlerThread handlerThread, BlobTransferCallback callback) {
        this.delayHandler = new Handler(handlerThread.getLooper());
        this.transferCallback = callback;
    }

    /**
     * @param type          BlobTransferType
     * @param directAddress transfer blob to local or direct connected address used when type is BlobTransferType.LOCAL or BlobTransferType.GATT,
     *                      otherwise not used
     */
    public void resetParams(FirmwareUpdateConfiguration configuration, BlobTransferType type, int directAddress) {
        this.firmwareData = configuration.getFirmwareData();
        this.dleLength = configuration.getDleLength();
        this.blobId = configuration.getBlobId();
        this.appKeyIndex = configuration.getAppKeyIndex();
        this.groupAddress = configuration.getGroupAddress();

        this.targetDevices.clear();
        this.transferType = type;
        if (type == BlobTransferType.LOCAL_INIT || type == BlobTransferType.GATT_INIT) {
            this.targetDevices.add(new TargetDevice(directAddress));
        } else {
            // BlobTransferType.MESH
            for (MeshUpdatingDevice device : configuration.getUpdatingDevices()) {
                this.targetDevices.add(new TargetDevice(device.meshAddress));
            }
        }
    }

    public void begin() {
        if (this.step != STEP_IDLE) {
            onTransferComplete(false, "transfer not idle");
            return;
        }

        log("blob transfer begin -- " + this.transferType);
        step = STEP_BLOB_TRANSFER_GET;
        this.pullProgress = -1;
        nextAction();
    }


    void clear() {
        this.step = STEP_IDLE;
        this.delayHandler.removeCallbacksAndMessages(null);
    }

    private void nextAction() {
        log("action: " + getStepDesc(step) + " -- node index -- " + nodeIndex);

        if (nodeIndex >= targetDevices.size()) {
            // all nodes executed
            log("current step complete: " + getStepDesc(step));
            removeFailedDevices();
            // check if has available nodes
            if (targetDevices.size() != 0) {
                nodeIndex = 0;
                if (step == STEP_GET_BLOB_BLOCK) {
                    onGetBlobBlockComplete();
                } else {
                    log("next step: " + getStepDesc(step + 1));
                    step++;
                    nextAction();
                }
            } else {
                onTransferComplete(false, "all nodes failed when executing action");
            }
        } else {
            int meshAddress = targetDevices.get(nodeIndex).address;
            log(String.format("action executing: " + getStepDesc(step) + " -- %04X", meshAddress));
            switch (this.step) {

                case STEP_GET_BLOB_INFO:
                    onTransferMessagePrepared(BlobInfoGetMessage.getSimple(meshAddress, appKeyIndex));
                    break;

                case STEP_BLOB_TRANSFER_START:
                    int objectSize = firmwareParser.getObjectSize();
                    byte blockSizeLog = (byte) MeshUtils.mathLog2(firmwareParser.getBlockSize());
                    int mtu = 380;

                    BlobTransferStartMessage blobTransferStartMessage = BlobTransferStartMessage.getSimple(meshAddress, appKeyIndex,
                            blobId, objectSize, blockSizeLog, mtu);
                    blobTransferStartMessage.setTransferMode(transferMode);
                    onTransferMessagePrepared(blobTransferStartMessage);
                    break;

                case STEP_BLOB_TRANSFER_GET:
                    onTransferMessagePrepared(BlobTransferGetMessage.getSimple(meshAddress, appKeyIndex));
                    break;

                case STEP_BLOB_BLOCK_START:
                    if (nodeIndex == 0) {
                        if (firmwareParser.hasNextBlock()) {
                            firmwareParser.nextBlock();
                        } else {
                            log("all blocks sent complete at: block -- " + firmwareParser.currentBlockIndex());
//                            step = STEP_UPDATE_GET;
                            onTransferComplete(true, "blob transfer complete");
                            return;
                        }
                    }
                    int blockNumber = firmwareParser.currentBlockIndex();
                    int chunkSize = firmwareParser.getChunkSize();
                    onTransferMessagePrepared(BlobBlockStartMessage.getSimple(meshAddress, appKeyIndex,
                            blockNumber, chunkSize));
                    break;

                case STEP_BLOB_CHUNK_SENDING:
                    if (transferMode == TransferMode.PUSH) {
                        sendChunks();
                    } else {
                        log("waiting for pull request");
                    }

                    break;

                case STEP_GET_BLOB_BLOCK:
                    onTransferMessagePrepared(BlobBlockGetMessage.getSimple(meshAddress, appKeyIndex));
                    break;

                default:
                    break;
            }
        }
    }

    private void onGetBlobBlockComplete() {
        // check if has missing chunk
        switch (mixFormat) {
            case BlobBlockStatusMessage.FORMAT_NO_CHUNKS_MISSING:
                log("no chunks missing");
                step = STEP_BLOB_BLOCK_START;
                pullMaxChunkNumber = 0;
                nextAction();
                break;

            case BlobBlockStatusMessage.FORMAT_ALL_CHUNKS_MISSING:
                // resend all chunks
                log("all chunks missing");
                step = STEP_BLOB_CHUNK_SENDING;
                firmwareParser.resetBlock();
                nextAction();
                break;
            case BlobBlockStatusMessage.FORMAT_SOME_CHUNKS_MISSING:
            case BlobBlockStatusMessage.FORMAT_ENCODED_MISSING_CHUNKS:
                // resend missing chunks
                log("resend missing chunks");
                missingChunkIndex = 0;
                sendMissingChunks();
                break;
        }
    }

    private void removeFailedDevices() {
        Iterator<TargetDevice> iterator = targetDevices.iterator();
        TargetDevice updatingNode;
        while (iterator.hasNext()) {
            updatingNode = iterator.next();
            if (updatingNode.state == MeshUpdatingDevice.STATE_FAIL) {
                iterator.remove();
            }
        }
    }


    private void onTransferMessagePrepared(MeshMessage meshMessage) {

        /*if (meshMessage.getDestinationAddress() != MeshUtils.LOCAL_MESSAGE_ADDRESS) {
            // for test
            nodeIndex++;
            nextAction();
            return;
        }*/

        meshMessage.setRetryCnt(10);
        log("mesh message prepared: " + meshMessage.getClass().getSimpleName()
                + String.format(" opcode: 0x%04X -- dst: 0x%04X", meshMessage.getOpcode(), meshMessage.getDestinationAddress()));
        if (transferCallback != null) {
            boolean isMessageSent = transferCallback.onMeshMessagePrepared(meshMessage);
            if (!isMessageSent) {
                if (meshMessage instanceof BlobChunkTransferMessage) {
                    onTransferComplete(false, "chunk transfer message sent error");
                } else {
                    if (targetDevices.size() > nodeIndex) {
                        onDeviceFail(targetDevices.get(nodeIndex), String.format("mesh message sent error -- opcode: 0x%04X", meshMessage.getOpcode()));
                    }
                }

            }
        }
    }

    /**
     * device fail
     *
     * @param device updating device
     */
    private void onDeviceFail(TargetDevice device, String desc) {
        log(String.format("node updating fail: %04X -- " + desc, device.address));
        device.state = TargetDevice.STATE_FAIL;
        if (transferCallback != null) {
            transferCallback.onTransferringDeviceFail(device.address, String.format("node updating fail: %04X -- ", device.address));
        }
    }

    /**
     * no device success
     */
    private void onTransferComplete(boolean success, String desc) {
        log("blob transfer complete: " + " -- " + desc);
        clear();
        if (transferCallback != null) {
            transferCallback.onTransferComplete(success, desc);
        }
    }

    public void onTransferCommandFail(int opcode) {
        log("transfer command fail");
        boolean fail = (step == STEP_BLOB_TRANSFER_GET && opcode == Opcode.BLOB_TRANSFER_GET.value)
                || (step == STEP_GET_BLOB_INFO && opcode == Opcode.BLOB_INFORMATION_GET.value)
                || (step == STEP_BLOB_TRANSFER_START && opcode == Opcode.BLOB_TRANSFER_START.value)
                || (step == STEP_BLOB_BLOCK_START && opcode == Opcode.BLOB_BLOCK_START.value)
                || (step == STEP_GET_BLOB_BLOCK && opcode == Opcode.BLOB_BLOCK_GET.value);
        if (fail) {
            onDeviceFail(targetDevices.get(nodeIndex), "device transfer command send fail");
            nodeIndex++;
            nextAction();
        } else if ((step == STEP_BLOB_CHUNK_SENDING && opcode == Opcode.BLOB_CHUNK_TRANSFER.value)) {
            onTransferComplete(false, "chunk transfer command send fail");
        }
    }

    public void onTransferSegmentComplete(boolean success) {
        if (transferType == BlobTransferType.GATT_INIT && step == STEP_BLOB_CHUNK_SENDING) {
            if (success) {
                if (transferMode == TransferMode.PUSH) {
                    sendChunks();
                }
            } else {
                onTransferComplete(false, "chunk send fail -- segment message send fail");
            }
        }
    }

    public void onTransferNotification(NotificationMessage message) {
        Opcode opcode = Opcode.valueOf(message.getOpcode());
        log("message notification: " + opcode);
        if (step == STEP_IDLE) {
            log("notification when idle");
            return;
        }
        if (opcode == null) return;
        final int src = message.getSrc();

        if (targetDevices.size() <= nodeIndex) {
            log("node index overflow", MeshLogger.LEVEL_WARN);
            return;
        }

        if (targetDevices.get(nodeIndex).address != src) {
            log("unexpected notification src", MeshLogger.LEVEL_WARN);
            return;
        }

        switch (opcode) {

            case BLOB_INFORMATION_STATUS:
                onBlobInfoStatus((BlobInfoStatusMessage) message.getStatusMessage());
                break;

            case BLOB_TRANSFER_STATUS:
                onBlobTransferStatus((BlobTransferStatusMessage) message.getStatusMessage());
                break;

            case BLOB_BLOCK_STATUS:
                onBlobBlockStatus(message);
                break;

            case BLOB_PARTIAL_BLOCK_REPORT:
                onBlobPartialBlockReport((BlobPartialBlockReportMessage) message.getStatusMessage());
                break;
        }
    }

    private void sendChunks() {
        byte[] chunkData = firmwareParser.nextChunk();
        final int chunkIndex = firmwareParser.currentChunkIndex();
        if (chunkData != null) {
            validateUpdatingProgress();
            BlobChunkTransferMessage blobChunkTransferMessage = generateChunkTransferMessage(chunkIndex, chunkData);
            log("next chunk transfer msg: " + blobChunkTransferMessage.toString());
            onTransferMessagePrepared(blobChunkTransferMessage);
            if (transferType == BlobTransferType.GATT_INIT) {

                int len = chunkData.length + 3;
                boolean isSegment = len > dleLength;
                if (!isSegment) {
                    // if not segmented message, send next
                    sendChunks();
                }
            } else {
                // for LOCAL or MESH type, send chunk by timer
                delayHandler.postDelayed(chunkSendingTask, getChunkSendingInterval());
            }
        } else {
            log("chunks sent complete at: block -- " + firmwareParser.currentBlockIndex()
                    + " chunk -- " + firmwareParser.currentChunkIndex());
            checkMissingChunks();
        }
    }

    /**
     * validate blob transfer progress
     */
    private void validateUpdatingProgress() {
        if (transferMode == TransferMode.PUSH) {
            if (firmwareParser.validateProgress()) {
                final int progress = firmwareParser.getProgress();
                log("chunk sending progress: " + progress);
                transferCallback.onTransferProgressUpdate(progress, transferType);
//            onStateUpdate(STATE_PROGRESS, "progress update", progress);
            }
        } else if (transferMode == TransferMode.PULL) {
            int progress = firmwareParser.getProgressAt(firmwareParser.currentBlockIndex(), pullMaxChunkNumber);
            if (this.pullProgress != progress) {
                this.pullProgress = progress;
                transferCallback.onTransferProgressUpdate(progress, transferType);
            }
        }

    }


    private void checkMissingChunks() {
        log("check missing chunks");
        missingChunks.clear();
        mixFormat = -1;
        step = STEP_GET_BLOB_BLOCK;
        nodeIndex = 0;
        nextAction();
    }


    private void sendMissingChunks() {
        if (missingChunkIndex >= missingChunks.size()) {
            // all missing chunk sent
            log("all missing chunks sent complete: " + missingChunkIndex);
            if (transferMode == TransferMode.PUSH) {
                checkMissingChunks();
            }
        } else {
            int chunkNumber = missingChunks.get(missingChunkIndex);
            log("send missing chunk at : " + firmwareParser.currentBlockIndex() + " - " + chunkNumber);
            byte[] chunkData = firmwareParser.chunkAt(chunkNumber);
            if (chunkData == null) {
                log("chunk index overflow when resending chunk: " + chunkNumber);
                return;
            }
            if (transferMode == TransferMode.PULL) {
                if (pullMaxChunkNumber < chunkNumber) {
                    pullMaxChunkNumber = chunkNumber;
                }
                validateUpdatingProgress();
            }
            BlobChunkTransferMessage blobChunkTransferMessage = generateChunkTransferMessage(chunkNumber, chunkData);
            onTransferMessagePrepared(blobChunkTransferMessage);
            delayHandler.removeCallbacks(missingChunkSendingTask);
            delayHandler.postDelayed(missingChunkSendingTask, getChunkSendingInterval());
        }
    }

    /**
     * mix missing chunks
     *
     * @param chunks chunks from remote device
     */
    private void mixMissingChunks(List<Integer> chunks) {
        if (this.mixFormat == BlobBlockStatusMessage.FORMAT_ALL_CHUNKS_MISSING) return;
        if (chunks != null) {
            for (int chunkNumber : chunks) {
                if (!missingChunks.contains(chunkNumber)) {
                    missingChunks.add(chunkNumber);
                }
            }
        }
    }

    /**
     * mix format info
     *
     * @param format format from remote device
     */
    private void mixFormat(int format) {
        if (this.mixFormat == BlobBlockStatusMessage.FORMAT_ALL_CHUNKS_MISSING) {
            return;
        }
        if (this.mixFormat == -1) {
            this.mixFormat = format;
        } else if (this.mixFormat != format && format != BlobBlockStatusMessage.FORMAT_NO_CHUNKS_MISSING) {
            this.mixFormat = format;
        }
    }


    private long getChunkSendingInterval() {
        // relay 320 ms

        if (this.transferType == BlobTransferType.LOCAL_INIT) {
            log("chunk sending interval: " + 30);
            return 3;
        } else if (this.transferType == BlobTransferType.GATT_INIT || this.transferType == BlobTransferType.GATT_DIST) {
            if (transferMode == TransferMode.PULL) {
                log("chunk sending interval: " + 10);
                return 10;
            }

            log("chunk sending interval: " + 100);
            return 100;
        }

        // 12
        // 208
        // chunk size + opcode(1 byte)
        final int chunkMsgLen = firmwareParser.getChunkSize() + 1;
        final int unsegLen = NetworkingController.unsegmentedAccessLength;
        final int segLen = unsegLen + 1;
        int segmentCnt = chunkMsgLen == unsegLen ? 1 : (chunkMsgLen % segLen == 0 ? chunkMsgLen / segLen : (chunkMsgLen / segLen + 1));
        long interval = segmentCnt * NetworkingController.NETWORKING_INTERVAL;
//        final long min = 5 * 1000;
        // use 5000 when DLE disabled, use 300 when DLE enabled
        final long min = unsegLen == NetworkingController.UNSEGMENTED_ACCESS_PAYLOAD_MAX_LENGTH_DEFAULT ? 5 * 1000 : 300;
        interval = Math.max(min, interval);
        log("chunk sending interval: " + interval);
        return interval;
    }

    private Runnable missingChunkSendingTask = new Runnable() {
        @Override
        public void run() {
            missingChunkIndex++;
            sendMissingChunks();
        }
    };

    private Runnable chunkSendingTask = new Runnable() {
        @Override
        public void run() {
            sendChunks();
        }
    };

    private BlobChunkTransferMessage generateChunkTransferMessage(int chunkNumber, byte[] chunkData) {
        int address;
        if (this.transferType == BlobTransferType.LOCAL_INIT
                || this.transferType == BlobTransferType.GATT_INIT
                || this.transferType == BlobTransferType.GATT_DIST) {
            // only one device
            address = this.targetDevices.get(0).address;
        } else {
            address = groupAddress;
        }
        return BlobChunkTransferMessage.getSimple(address, appKeyIndex, chunkNumber, chunkData);
    }

    /**
     * response of {@link BlobTransferStartMessage}
     */
    private void onBlobTransferStatus(BlobTransferStatusMessage transferStatusMessage) {
        log("blob transfer status: " + transferStatusMessage.toString());
        if (step == STEP_BLOB_TRANSFER_START || step == STEP_BLOB_TRANSFER_GET) {
            UpdateStatus status = UpdateStatus.valueOf(transferStatusMessage.getStatus());

            if (status != UpdateStatus.SUCCESS) {
                onDeviceFail(targetDevices.get(nodeIndex), "object transfer status err");
            }
            nodeIndex++;
            nextAction();
        }

        /*byte status = transferStatusMessage.getStatus();
        if (status != ObjectTransferStatusMessage.STATUS_READY
                && status != ObjectTransferStatusMessage.STATUS_BUSY_ACTIVE) {
            onDeviceFail(nodes.get(nodeIndex), "object transfer status err");
        }*/

    }

    /**
     * response of BLOCK_GET
     * {@link #checkMissingChunks()}
     * {@link BlobBlockStartMessage}
     */
    private void onBlobBlockStatus(NotificationMessage message) {
        if (step != STEP_GET_BLOB_BLOCK && step != STEP_BLOB_BLOCK_START) {
            return;
        }
        BlobBlockStatusMessage blobBlockStatusMessage = (BlobBlockStatusMessage) message.getStatusMessage();
        log("block status: " + blobBlockStatusMessage.toString());
        int srcAddress = message.getSrc();
        TransferStatus transferStatus = TransferStatus.valueOf(blobBlockStatusMessage.getStatus() & 0xFF);
        if (transferStatus != TransferStatus.SUCCESS) {
            onDeviceFail(targetDevices.get(nodeIndex), "block status err");
        } else {
            // only check chunk missing when STEP_GET_BLOB_BLOCK
            if (step == STEP_GET_BLOB_BLOCK || (step == STEP_BLOB_BLOCK_START && transferMode == TransferMode.PULL)) {
                int format = blobBlockStatusMessage.getFormat();

                mixFormat(format);

                switch (format) {
                    case BlobBlockStatusMessage.FORMAT_ALL_CHUNKS_MISSING:
                        log(String.format("all chunks missing: %04X", srcAddress));
                        break;

                    case BlobBlockStatusMessage.FORMAT_NO_CHUNKS_MISSING:
                        log(String.format("no chunks missing: %04X", srcAddress));
                        break;

                    case BlobBlockStatusMessage.FORMAT_SOME_CHUNKS_MISSING:
                        mixMissingChunks(blobBlockStatusMessage.getMissingChunks());
                        break;

                    case BlobBlockStatusMessage.FORMAT_ENCODED_MISSING_CHUNKS:
                        mixMissingChunks(blobBlockStatusMessage.getEncodedMissingChunks());
                        break;
                }
            }
        }
        nodeIndex++;
        nextAction();
    }

    /**
     * only used at pull mode
     */
    private void onBlobPartialBlockReport(BlobPartialBlockReportMessage reportMessage) {

        final ArrayList<Integer> encodedMissingChunks = reportMessage.getEncodedMissingChunks();
        if (encodedMissingChunks.size() == 0) {
            log("no missing chunks in PartialBlockReport ");
            pullMaxChunkNumber = 0;
            checkMissingChunks();
        } else {
            StringBuilder missingDesc = new StringBuilder();
            for (int index : encodedMissingChunks) {
                missingDesc.append(" -- ").append(index);
            }
            log("PartialBlockReport " + firmwareParser.currentBlockIndex() + " - " + encodedMissingChunks.size() + " - " + missingDesc);
            delayHandler.removeCallbacks(missingChunkSendingTask);
            this.missingChunks = encodedMissingChunks;
            this.missingChunkIndex = 0;
            this.mixFormat = BlobBlockStatusMessage.FORMAT_ENCODED_MISSING_CHUNKS;
            sendMissingChunks();
        }
    }

    /**
     * response of {@link BlobInfoStatusMessage}
     */
    private void onBlobInfoStatus(BlobInfoStatusMessage blobInfoStatusMessage) {


        log("object info status: " + blobInfoStatusMessage.toString());
        if (step != STEP_GET_BLOB_INFO) {
            log("not at STEP_GET_BLOB_INFO");
            return;
        }
        int blockSize = (int) Math.pow(2, blobInfoStatusMessage.getMaxBlockSizeLog());
        int chunkSize = blobInfoStatusMessage.getMaxChunkSize();

        final boolean pushSupported = blobInfoStatusMessage.isPushModeSupported();
        final boolean pullSupported = blobInfoStatusMessage.isPullModeSupported();

        // use push mode when transfer to direct connected device
        final boolean gattTransfer = this.transferType == BlobTransferType.GATT_DIST || this.transferType == BlobTransferType.GATT_INIT;
        if (pullSupported) {
            if (pushSupported) {
                this.transferMode = gattTransfer ? TransferMode.PUSH : TransferMode.PULL;
            } else {
                this.transferMode = TransferMode.PULL;
            }
        } else if (pushSupported) {
            this.transferMode = TransferMode.PUSH;
        }


        log("refresh transfer mode: " + this.transferMode);

        log("chunk size : " + chunkSize + " block size: " + blockSize);

        this.firmwareParser.reset(firmwareData, blockSize, chunkSize);
        nodeIndex++;
        nextAction();
    }

    /**
     * get step description
     */
    private String getStepDesc(int step) {
        switch (step) {

            case STEP_BLOB_TRANSFER_START:
                return "blob-transfer-start";

            case STEP_BLOB_TRANSFER_GET:
                return "blob-transfer-get";

            case STEP_GET_BLOB_INFO:
                return "get-blob-info";

            case STEP_BLOB_BLOCK_START:
                return "blob-block-transfer-start";

            case STEP_BLOB_CHUNK_SENDING:
                return "blob-chunk-sending";

            case STEP_GET_BLOB_BLOCK:
                return "get-blob-block";
        }
        return "unknown";
    }

    private void log(String logInfo) {
        log(logInfo, MeshLogger.LEVEL_DEBUG);
    }

    private void log(String logInfo, int level) {
        transferCallback.onTransferLog(LOG_TAG, logInfo, level);
    }


    public static class TargetDevice {
        private static final int STATE_INITIAL = 0;
        private static final int STATE_SUCCESS = 1;
        private static final int STATE_FAIL = 2;

        private int address;

        private int state = STATE_INITIAL;

        public TargetDevice(int address) {
            this.address = address;
        }
    }
}
