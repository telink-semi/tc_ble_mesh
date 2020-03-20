package com.telink.ble.mesh.core.message.updating;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;


public class ObjectInfoStatusMessage extends StatusMessage implements Parcelable {


    /**
     * size: 1 bytes
     */
    private byte minBlockSizeLog;

    private byte maxBlockSizeLog;

    private int maxChunksNumber;

    public ObjectInfoStatusMessage() {
    }


    protected ObjectInfoStatusMessage(Parcel in) {
        minBlockSizeLog = in.readByte();
        maxBlockSizeLog = in.readByte();
        maxChunksNumber = in.readInt();
    }

    public static final Creator<ObjectInfoStatusMessage> CREATOR = new Creator<ObjectInfoStatusMessage>() {
        @Override
        public ObjectInfoStatusMessage createFromParcel(Parcel in) {
            return new ObjectInfoStatusMessage(in);
        }

        @Override
        public ObjectInfoStatusMessage[] newArray(int size) {
            return new ObjectInfoStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.minBlockSizeLog = params[index++];
        this.maxBlockSizeLog = params[index++];
        maxChunksNumber = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(minBlockSizeLog);
        dest.writeByte(maxBlockSizeLog);
        dest.writeInt(maxChunksNumber);
    }

    public byte getMinBlockSizeLog() {
        return minBlockSizeLog;
    }

    public byte getMaxBlockSizeLog() {
        return maxBlockSizeLog;
    }

    public int getMaxChunksNumber() {
        return maxChunksNumber;
    }

    @Override
    public String toString() {
        return "ObjectInfoStatusMessage{" +
                "minBlockSizeLog=" + minBlockSizeLog +
                ", maxBlockSizeLog=" + maxBlockSizeLog +
                ", maxChunksNumber=" + maxChunksNumber +
                '}';
    }
}
