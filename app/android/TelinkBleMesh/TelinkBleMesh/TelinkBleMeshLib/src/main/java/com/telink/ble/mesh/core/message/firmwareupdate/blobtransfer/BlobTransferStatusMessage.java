/********************************************************************************************************
 * @file BlobTransferStatusMessage.java
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

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.List;

/**
 * This class represents a Blob Transfer Status Message, which is a type of Status Message.
 * It implements the Parcelable interface to allow for easy serialization and deserialization.
 * The Blob Transfer Status Message contains information about the status, transfer mode, transfer phase,
 * blob identifier, blob size, block size log, MTU size, and a bit field indicating blocks that were not received.
 */
public class BlobTransferStatusMessage extends StatusMessage implements Parcelable {

    /**
     * Status Code for the requesting message
     * lower 4 bits in first byte
     */
    private int status;

    /**
     * BLOB transfer mode
     * higher 2 bits in first byte
     */
    private int transferMode;

    /**
     * Transfer phase
     * 8 bits
     */
    private byte transferPhase;

    /**
     * BLOB identifier (Optional)
     * 64 bits
     */
    private long blobId;

    /**
     * BLOB size in octets
     * If the BLOB ID field is present, then the BLOB Size field may be present;
     * otherwise, the BLOB Size field shall not be present.
     * 32 bits
     */
    private int blobSize;

    /**
     * Indicates the block size
     * 8 bits
     * present if blobSize is present
     */
    private int blockSizeLog;

    /**
     * MTU size in octets
     * 16 bits
     * present if blobSize is present
     */
    private int transferMTUSize;

    /**
     * Bit field indicating blocks that were not received
     * length: Variable, currently 64 bits max
     * present if blobSize is present
     */
    private List<Integer> blocksNotReceived;

    /**
     * Default constructor for the BlobTransferStatusMessage class.
     */
    public BlobTransferStatusMessage() {
    }

    /**
     * Constructor for the BlobTransferStatusMessage class.
     *
     * @param in The Parcel from which to read the values.
     */
    protected BlobTransferStatusMessage(Parcel in) {
        status = in.readInt();
        transferMode = in.readInt();
        transferPhase = in.readByte();
        blobId = in.readLong();
        blobSize = in.readInt();
        blockSizeLog = in.readInt();
        transferMTUSize = in.readInt();
        blocksNotReceived = new ArrayList<>();
        in.readList(blocksNotReceived, null);
    }

    /**
     * A Creator object that implements the Parcelable.Creator interface.
     * It is used to create new instances of the BlobTransferStatusMessage class from a Parcel.
     */
    public static final Creator<BlobTransferStatusMessage> CREATOR = new Creator<BlobTransferStatusMessage>() {
        @Override
        public BlobTransferStatusMessage createFromParcel(Parcel in) {
            return new BlobTransferStatusMessage(in);
        }

        @Override
        public BlobTransferStatusMessage[] newArray(int size) {
            return new BlobTransferStatusMessage[size];
        }
    };

    /**
     * Parses the byte array and sets the values of the status.
     *
     * @param params The byte array to parse.
     */
    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.status = params[index] & 0x0F;
        this.transferMode = (params[index++] >> 6) & 0x03;
        this.transferPhase = params[index++];

        if (params.length < 10) return;

        this.blobId = MeshUtils.bytes2Long(params, index, 8, ByteOrder.LITTLE_ENDIAN);
        index += 8;

        if (params.length == 10) return;

        this.blobSize = MeshUtils.bytes2Integer(params, index, 4, ByteOrder.LITTLE_ENDIAN);
        index += 4;
        this.blockSizeLog = params[index++] & 0xFF;
        this.transferMTUSize = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;

        this.blocksNotReceived = MeshUtils.parseMissingBitField(params, index);
    }

    /**
     * Returns a bitmask indicating the set of special object types marshaled by this Parcelable object instance.
     *
     * @return Always returns 0, as there are no special object types.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Flattens the object into a Parcel.
     *
     * @param dest  The Parcel in which the object should be written.
     * @param flags Additional flags about how the object should be written.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(status);
        dest.writeInt(transferMode);
        dest.writeByte(transferPhase);
        dest.writeLong(blobId);
        dest.writeInt(blobSize);
        dest.writeInt(blockSizeLog);
        dest.writeInt(transferMTUSize);
        dest.writeList(blocksNotReceived);
    }

    /**
     * Returns the status of the firmware metadata.
     *
     * @return The status of the firmware metadata.
     */
    public int getStatus() {
        return status;
    }

    /**
     * Returns the mode of the transfer.
     *
     * @return The mode of the transfer.
     */
    public int getTransferMode() {
        return transferMode;
    }

    /**
     * Returns the phase of the transfer.
     *
     * @return The phase of the transfer.
     */
    public byte getTransferPhase() {
        return transferPhase;
    }

    /**
     * Returns the blob id.
     *
     * @return The blob id.
     */
    public long getBlobId() {
        return blobId;
    }

    /**
     * Returns the blob size.
     *
     * @return The blob size.
     */
    public int getBlobSize() {
        return blobSize;
    }

    /**
     * Returns the blob log.
     *
     * @return The blob log.
     */
    public int getBlockSizeLog() {
        return blockSizeLog;
    }

    /**
     * Returns the mtu size.
     *
     * @return The mtu size.
     */
    public int getTransferMTUSize() {
        return transferMTUSize;
    }

    /**
     * Returns the missing blocks.
     *
     * @return The missing blocks.
     */
    public List<Integer> getBlocksNotReceived() {
        return blocksNotReceived;
    }

    /**
     * Returns a string representation of the object.
     *
     * @return A string representation of the object.
     */
    @Override
    public String toString() {
        return "BlobTransferStatusMessage{" +
                "status=" + status +
                ", transferMode=" + transferMode +
                ", transferPhase=" + transferPhase +
                ", blobId=0x" + Long.toHexString(blobId) +
                ", blobSize=" + blobSize +
                ", blockSizeLog=" + blockSizeLog +
                ", transferMTUSize=" + transferMTUSize +
                ", blocksNotReceived=" + blocksNotReceived +
                '}';
    }
}
