/********************************************************************************************************
 * @file BlobTransferGetMessage.java
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
package com.telink.ble.mesh.core.message.firmwareupdate.blobtransfer;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.firmwareupdate.UpdatingMessage;

/**
 * This class represents a BlobTransferGetMessage, which is used to request a blob transfer from a destination address
 * to a specified appKeyIndex. It extends the UpdatingMessage class.
 */
public class BlobTransferGetMessage extends UpdatingMessage {

    /**
     * Creates a new instance of BlobTransferGetMessage with the specified destination address and appKeyIndex.
     *
     * @param destinationAddress The destination address to send the blob transfer request to.
     * @param appKeyIndex        The index of the app key to be used for the blob transfer.
     */
    public BlobTransferGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    /**
     * Returns the opcode value for the BlobTransferGetMessage.
     *
     * @return The opcode value for the BlobTransferGetMessage.
     */
    @Override
    public int getOpcode() {
        return Opcode.BLOB_TRANSFER_GET.value;
    }

    /**
     * Returns the response opcode value for the BlobTransferGetMessage.
     *
     * @return The response opcode value for the BlobTransferGetMessage.
     */
    @Override
    public int getResponseOpcode() {
        return Opcode.BLOB_TRANSFER_STATUS.value;
    }

    /**
     * Creates and returns a simple BlobTransferGetMessage with the specified destination address and appKeyIndex.
     * The response max is set to 1 by default.
     *
     * @param destinationAddress The destination address to send the blob transfer request to.
     * @param appKeyIndex        The index of the app key to be used for the blob transfer.
     * @return A simple BlobTransferGetMessage with the specified destination address and appKeyIndex.
     */
    public static BlobTransferGetMessage getSimple(int destinationAddress, int appKeyIndex) {
        BlobTransferGetMessage message = new BlobTransferGetMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(1);
        return message;
    }
}