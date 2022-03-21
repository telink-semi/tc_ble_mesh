/********************************************************************************************************
 * @file MessageResponder.java
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
package com.telink.ble.mesh.core.message;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.firmwaredistribution.FDCapabilitiesStatusMessage;
import com.telink.ble.mesh.core.message.firmwaredistribution.FDReceiversStatusMessage;
import com.telink.ble.mesh.core.message.firmwaredistribution.FDStatusMessage;
import com.telink.ble.mesh.core.message.firmwaredistribution.FDUploadStatusMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.DistributionStatus;
import com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer.BlobBlockStatusMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer.BlobInfoStatusMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer.BlobTransferStatusMessage;
import com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer.TransferStatus;

/**
 * response local messages
 * only support distribution messages
 */
public class MessageResponder {

    public static NotificationMessage responseMessage(MeshMessage meshMessage) {
        final int src = MeshUtils.LOCAL_MESSAGE_ADDRESS;
        final int dst = MeshUtils.LOCAL_MESSAGE_ADDRESS;
        int opcode = meshMessage.getResponseOpcode();
        byte[] params = null;
        NotificationMessage notificationMessage = new NotificationMessage(src, dst, opcode, params);
        notificationMessage.setStatusMessage(generateStatusMessage(meshMessage));
        return notificationMessage;
    }

    public static StatusMessage generateStatusMessage(MeshMessage meshMessage) {
        int rspOpcode = meshMessage.getResponseOpcode();
        StatusMessage statusMessage = null;
        if (rspOpcode == Opcode.FD_CAPABILITIES_STATUS.value) {
            // for FDCapabilitiesGetMessage
            statusMessage = new FDCapabilitiesStatusMessage();
            return statusMessage;
        } else if (rspOpcode == Opcode.FD_RECEIVERS_STATUS.value) {
            // for FDReceiversAddMessage
            statusMessage = new FDReceiversStatusMessage();
        } else if (rspOpcode == Opcode.FD_UPLOAD_STATUS.value) {
            // for FDUploadStartMessage
            statusMessage = new FDUploadStatusMessage();
        } else if (rspOpcode == Opcode.BLOB_INFORMATION_STATUS.value) {
//            12 12 ed 04 d0 00 00 00 03 00 7c 01 01
            BlobInfoStatusMessage blobInfoStatusMessage = new BlobInfoStatusMessage();
            blobInfoStatusMessage.setMinBlockSizeLog(0x12);
            blobInfoStatusMessage.setMaxBlockSizeLog(0x12);
            blobInfoStatusMessage.setMaxTotalChunks(0x04ed);
            blobInfoStatusMessage.setMaxChunkSize(0x00d0);
            blobInfoStatusMessage.setMaxBLOBSize(0x0003);
            blobInfoStatusMessage.setServerMTUSize(0x017c);
            blobInfoStatusMessage.setSupportedTransferMode(0x01);
            statusMessage = blobInfoStatusMessage;
        } else if (rspOpcode == Opcode.BLOB_TRANSFER_STATUS.value) {
            // for BlobTransferStartMessage or BlobTransferGetMessage
            statusMessage = new BlobTransferStatusMessage();
        } else if (rspOpcode == Opcode.BLOB_BLOCK_STATUS.value) {
            // for BlobBlockStartMessage or BlobBlockGetMessage
            BlobBlockStatusMessage blockStatusMessage = new BlobBlockStatusMessage();
            blockStatusMessage.setStatus(TransferStatus.SUCCESS.code);
            blockStatusMessage.setFormat(BlobBlockStatusMessage.FORMAT_NO_CHUNKS_MISSING);
            statusMessage = blockStatusMessage;
        } else if (rspOpcode == Opcode.FD_STATUS.value) {
            FDStatusMessage fdStatusMessage = new FDStatusMessage();
            fdStatusMessage.status = DistributionStatus.SUCCESS.code;
            statusMessage = fdStatusMessage;
        }
        return statusMessage;
    }

}
