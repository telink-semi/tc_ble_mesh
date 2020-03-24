package com.telink.ble.mesh.core.message.updating;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;

public class ObjectTransferStatusMessage extends StatusMessage implements Parcelable {

    /**
     * ready, object transfer is not active
     */
    public static final byte STATUS_READY = 0x00;

    /**
     * busy, object transfer is active
     */
    public static final byte STATUS_BUSY_ACTIVE = 0x01;

    /**
     * busy, with different transfer
     */
    public static final byte STATUS_BUSY_DIFF_TRANSFER = 0x02;

    /**
     * object is too big to be stored
     */
    public static final byte STATUS_TOO_BIG_ERR = 0x03;


    /**
     * size: 1 bytes
     */
    private byte status;

    /**
     * size: 8 bytes
     */
    private long objectId;

    /**
     * size: 4 bytes
     */
    private int objectSize;


    /**
     * Size of the block timeout this transfer
     * size: 1 byte
     */
    private int blockSizeLog;


    public ObjectTransferStatusMessage() {
    }

    protected ObjectTransferStatusMessage(Parcel in) {
        status = in.readByte();
        objectId = in.readLong();
        objectSize = in.readInt();
        blockSizeLog = in.readInt();
    }

    public static final Creator<ObjectTransferStatusMessage> CREATOR = new Creator<ObjectTransferStatusMessage>() {
        @Override
        public ObjectTransferStatusMessage createFromParcel(Parcel in) {
            return new ObjectTransferStatusMessage(in);
        }

        @Override
        public ObjectTransferStatusMessage[] newArray(int size) {
            return new ObjectTransferStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.status = params[index++];
        this.objectId = MeshUtils.bytes2Long(params, index, 8, ByteOrder.LITTLE_ENDIAN);
        index += 8;

        this.objectSize = MeshUtils.bytes2Integer(params, index, 4, ByteOrder.LITTLE_ENDIAN);
        index += 4;

        this.blockSizeLog = params[index];
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(status);
        dest.writeLong(objectId);
        dest.writeInt(objectSize);
        dest.writeInt(blockSizeLog);
    }


    public byte getStatus() {
        return status;
    }

    public long getObjectId() {
        return objectId;
    }

    public int getObjectSize() {
        return objectSize;
    }

    public int getBlockSizeLog() {
        return blockSizeLog;
    }

    @Override
    public String toString() {
        return "ObjectTransferStatusMessage{" +
                "status=" + status +
                ", objectId=0x" + Long.toHexString(objectId) +
                ", objectSize=" + objectSize +
                ", blockSizeLog=" + blockSizeLog +
                '}';
    }
}
