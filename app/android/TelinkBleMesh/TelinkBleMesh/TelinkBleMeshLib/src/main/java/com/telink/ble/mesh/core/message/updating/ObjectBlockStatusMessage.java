package com.telink.ble.mesh.core.message.updating;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;
import java.util.Arrays;

public class ObjectBlockStatusMessage extends StatusMessage implements Parcelable {


    /**
     * All chunks received, checksum is valid, ready for the next block
     */
    public static final byte STATUS_ALL_CHUNKS_RECEIVED = 0x00;

    /**
     * Not all chunks received, checksum is not computed
     */
    public static final byte STATUS_NOT_ALL_CHUNKS_RECEIVED = 0x01;

    /**
     * All chunks received, computed checksum value is not equal to expected value
     */
    public static final byte STATUS_WRONG_CHECKSUM = 0x02;

    /**
     * Requested Object ID not active
     */
    public static final byte STATUS_WRONG_OBJECT_ID = 0x03;

    /**
     * Requested block not active
     */
    public static final byte STATUS_WRONG_BLOCK = 0x04;


    /**
     * size: 1 bytes
     */
    private byte status;

    private int[] missingChunksList;

    public ObjectBlockStatusMessage() {
    }


    protected ObjectBlockStatusMessage(Parcel in) {
        status = in.readByte();
        missingChunksList = in.createIntArray();
    }

    public static final Creator<ObjectBlockStatusMessage> CREATOR = new Creator<ObjectBlockStatusMessage>() {
        @Override
        public ObjectBlockStatusMessage createFromParcel(Parcel in) {
            return new ObjectBlockStatusMessage(in);
        }

        @Override
        public ObjectBlockStatusMessage[] newArray(int size) {
            return new ObjectBlockStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.status = params[index++];
        int listSize = (params.length - 1) / 2;
        if (listSize == 0) return;
        missingChunksList = new int[listSize];
        for (int i = 0; i < listSize; i++) {
            missingChunksList[i] = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
            index += 2;
        }
    }


    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(status);
        dest.writeIntArray(missingChunksList);
    }

    public byte getStatus() {
        return status;
    }

    public int[] getMissingChunksList() {
        return missingChunksList;
    }

    @Override
    public String toString() {
        return "ObjectBlockStatusMessage{" +
                "status=" + status +
                ", missingChunksList=" + Arrays.toString(missingChunksList) +
                '}';
    }
}
