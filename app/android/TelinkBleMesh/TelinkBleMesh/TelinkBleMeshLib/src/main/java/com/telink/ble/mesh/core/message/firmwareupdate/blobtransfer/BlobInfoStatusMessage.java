/********************************************************************************************************
 * @file BlobInfoStatusMessage.java
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


/**
 * This class represents a status message containing information about a Blob.
 * It extends the StatusMessage class and implements the Parcelable interface for easy serialization.
 * The class includes various attributes such as the minimum and maximum block sizes supported by the server,
 * the maximum number of chunks in a block, the maximum chunk size, the maximum BLOB size, the MTU size supported by the server,
 * and the supported transfer modes.
 * The class provides methods to get and set these attributes, as well as methods to check if push and pull transfer modes are supported.
 * The class also overrides the parse, describeContents, writeToParcel, and toString methods for proper functionality and string representation.
 */
public class BlobInfoStatusMessage extends StatusMessage implements Parcelable {


    /**
     * Minimum block size supported by the server
     * 1 byte
     */
    private int minBlockSizeLog;

    /**
     * Maximum block size supported by the server
     * 1 byte
     */
    private int maxBlockSizeLog;

    /**
     * Maximum number of chunks in block supported by the server
     * 2 bytes
     */
    private int maxTotalChunks;

    /**
     * Maximum chunk size supported by the server
     * 2 bytes
     */
    private int maxChunkSize;

    /**
     * Maximum BLOB size supported by the server
     * 4 bytes
     */
    private int maxBLOBSize;

    /**
     * MTU size supported by the server
     * 2 bytes
     */
    private int serverMTUSize;

    /**
     * BLOB transfer modes supported by the server
     * 1 byte
     */
    private int supportedTransferMode;

    /**
     * Default constructor for the BlobInfoStatusMessage class.
     */
    public BlobInfoStatusMessage() {
    }


    /**
     * Constructor for the BlobInfoStatusMessage class that takes a Parcel as input.
     * It reads the attributes from the Parcel and initializes the object.
     *
     * @param in The Parcel object to read from.
     */
    protected BlobInfoStatusMessage(Parcel in) {
        minBlockSizeLog = in.readInt();
        maxBlockSizeLog = in.readInt();
        maxTotalChunks = in.readInt();
        maxChunkSize = in.readInt();
        maxBLOBSize = in.readInt();
        serverMTUSize = in.readInt();
        supportedTransferMode = in.readInt();
    }

    /**
     * Creator constant for the BlobInfoStatusMessage class.
     * It is used to create new instances of the class from a Parcel.
     */
    public static final Creator<BlobInfoStatusMessage> CREATOR = new Creator<BlobInfoStatusMessage>() {
        @Override
        public BlobInfoStatusMessage createFromParcel(Parcel in) {
            return new BlobInfoStatusMessage(in);
        }

        @Override
        public BlobInfoStatusMessage[] newArray(int size) {
            return new BlobInfoStatusMessage[size];
        }
    };

    /**
     * Parses the given byte array and sets the attributes of the BlobInfoStatusMessage object accordingly.
     *
     * @param params The byte array to parse.
     */
    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.minBlockSizeLog = params[index++] & 0xFF;
        this.maxBlockSizeLog = params[index++] & 0xFF;

        this.maxTotalChunks = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;

        this.maxChunkSize = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;

        this.maxBLOBSize = MeshUtils.bytes2Integer(params, index, 4, ByteOrder.LITTLE_ENDIAN);
        index += 4;

        this.serverMTUSize = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;

        this.supportedTransferMode = params[index];
    }

    /**
     * Returns a bitmask indicating the set of special object types marshaled by this Parcelable object instance.
     *
     * @return Always returns 0 as there are no special object types.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Flattens the object into a Parcel.
     *
     * @param dest  The Parcel object to write into.
     * @param flags Additional flags about how the object should be written.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(minBlockSizeLog);
        dest.writeInt(maxBlockSizeLog);
        dest.writeInt(maxTotalChunks);
        dest.writeInt(maxChunkSize);
        dest.writeInt(maxBLOBSize);
        dest.writeInt(serverMTUSize);
        dest.writeInt(supportedTransferMode);
    }

    /**
     * Returns the minimum block size log supported by the server.
     *
     * @return The minimum block size log.
     */
    public int getMinBlockSizeLog() {
        return minBlockSizeLog;
    }

    /**
     * Returns the maximum block size log supported by the server.
     *
     * @return The maximum block size log.
     */
    public int getMaxBlockSizeLog() {
        return maxBlockSizeLog;
    }

    /**
     * Returns the maximum number of chunks in a block supported by the server.
     *
     * @return The maximum number of chunks in a block.
     */
    public int getMaxTotalChunks() {
        return maxTotalChunks;
    }

    /**
     * Returns the maximum chunk size supported by the server.
     *
     * @return The maximum chunk size.
     */
    public int getMaxChunkSize() {
        return maxChunkSize;
    }

    /**
     * Returns the maximum BLOB size supported by the server.
     *
     * @return The maximum BLOB size.
     */
    public int getMaxBLOBSize() {
        return maxBLOBSize;
    }

    /**
     * Returns the MTU size supported by the server.
     *
     * @return The MTU size.
     */
    public int getServerMTUSize() {
        return serverMTUSize;
    }

    /**
     * Returns the supported transfer mode bitmask.
     *
     * @return The supported transfer mode bitmask.
     */
    public int getSupportedTransferMode() {
        return supportedTransferMode;
    }

    /**
     * Sets the minimum block size log supported by the server.
     *
     * @param minBlockSizeLog The minimum block size log to set.
     */
    public void setMinBlockSizeLog(int minBlockSizeLog) {
        this.minBlockSizeLog = minBlockSizeLog;
    }

    /**
     * Sets the maximum block size log supported by the server.
     *
     * @param maxBlockSizeLog The maximum block size log to set.
     */
    public void setMaxBlockSizeLog(int maxBlockSizeLog) {
        this.maxBlockSizeLog = maxBlockSizeLog;
    }

    /**
     * Sets the maximum number of chunks in a block supported by the server.
     *
     * @param maxTotalChunks The maximum number of chunks in a block to set.
     */
    public void setMaxTotalChunks(int maxTotalChunks) {
        this.maxTotalChunks = maxTotalChunks;
    }

    /**
     * Sets the maximum chunk size supported by the server.
     *
     * @param maxChunkSize The maximum chunk size to set.
     */
    public void setMaxChunkSize(int maxChunkSize) {
        this.maxChunkSize = maxChunkSize;
    }

    /**
     * Sets the maximum BLOB size supported by the server.
     *
     * @param maxBLOBSize The maximum BLOB size to set.
     */
    public void setMaxBLOBSize(int maxBLOBSize) {
        this.maxBLOBSize = maxBLOBSize;
    }

    /**
     * Sets the MTU size supported by the server.
     *
     * @param serverMTUSize The MTU size to set.
     */
    public void setServerMTUSize(int serverMTUSize) {
        this.serverMTUSize = serverMTUSize;
    }

    /**
     * Sets the supported transfer mode bitmask.
     *
     * @param supportedTransferMode The supported transfer mode bitmask to set.
     */
    public void setSupportedTransferMode(int supportedTransferMode) {
        this.supportedTransferMode = supportedTransferMode;
    }

    /**
     * Checks if the push transfer mode is supported.
     *
     * @return True if the push transfer mode is supported, false otherwise.
     */
    public boolean isPushModeSupported() {
        return (this.supportedTransferMode & TransferMode.PUSH.value) != 0;
    }

    /**
     * Checks if the pull transfer mode is supported.
     *
     * @return True if the pull transfer mode is supported, false otherwise.
     */
    public boolean isPullModeSupported() {
        return (this.supportedTransferMode & TransferMode.PULL.value) != 0;
    }

    /**
     * Returns a string representation of the BlobInfoStatusMessage object.
     *
     * @return A string representation of the object.
     */
    @Override
    public String toString() {
        return "BlobInfoStatusMessage{" +
                "minBlockSizeLog=" + minBlockSizeLog +
                ", maxBlockSizeLog=" + maxBlockSizeLog +
                ", maxTotalChunks=" + maxTotalChunks +
                ", maxChunkSize=" + maxChunkSize +
                ", maxBLOBSize=" + maxBLOBSize +
                ", serverMTUSize=" + serverMTUSize +
                ", supportedTransferMode=" + supportedTransferMode +
                ", isPushModeSupported=" + isPushModeSupported() +
                ", isPullModeSupported=" + isPullModeSupported() +
                '}';
    }
}