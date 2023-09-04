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

    public BlobInfoStatusMessage() {
    }


    protected BlobInfoStatusMessage(Parcel in) {
        minBlockSizeLog = in.readInt();
        maxBlockSizeLog = in.readInt();
        maxTotalChunks = in.readInt();
        maxChunkSize = in.readInt();
        maxBLOBSize = in.readInt();
        serverMTUSize = in.readInt();
        supportedTransferMode = in.readInt();
    }

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

    @Override
    public int describeContents() {
        return 0;
    }

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

    public int getMinBlockSizeLog() {
        return minBlockSizeLog;
    }

    public int getMaxBlockSizeLog() {
        return maxBlockSizeLog;
    }

    public int getMaxTotalChunks() {
        return maxTotalChunks;
    }

    public int getMaxChunkSize() {
        return maxChunkSize;
    }

    public int getMaxBLOBSize() {
        return maxBLOBSize;
    }

    public int getServerMTUSize() {
        return serverMTUSize;
    }

    public int getSupportedTransferMode() {
        return supportedTransferMode;
    }

    public void setMinBlockSizeLog(int minBlockSizeLog) {
        this.minBlockSizeLog = minBlockSizeLog;
    }

    public void setMaxBlockSizeLog(int maxBlockSizeLog) {
        this.maxBlockSizeLog = maxBlockSizeLog;
    }

    public void setMaxTotalChunks(int maxTotalChunks) {
        this.maxTotalChunks = maxTotalChunks;
    }

    public void setMaxChunkSize(int maxChunkSize) {
        this.maxChunkSize = maxChunkSize;
    }

    public void setMaxBLOBSize(int maxBLOBSize) {
        this.maxBLOBSize = maxBLOBSize;
    }

    public void setServerMTUSize(int serverMTUSize) {
        this.serverMTUSize = serverMTUSize;
    }

    public void setSupportedTransferMode(int supportedTransferMode) {
        this.supportedTransferMode = supportedTransferMode;
    }

    public boolean isPushModeSupported() {
        return (this.supportedTransferMode & TransferMode.PUSH.value) != 0;
    }

    public boolean isPullModeSupported() {
        return (this.supportedTransferMode & TransferMode.PULL.value) != 0;
    }


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
