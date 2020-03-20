package com.telink.ble.mesh.core.message.updating;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.message.StatusMessage;


public class ObjectBlockTransferStatusMessage extends StatusMessage implements Parcelable {

    /**
     * block transfer accepted
     */
    public static final byte STATUS_ACCEPTED = 0x00;

    /**
     * block already transferred
     */
    public static final byte STATUS_ALREADY_TRANSFERRED = 0x01;

    /**
     * invalid block number, no previous block
     */
    public static final byte STATUS_NUMBER_INVALID = 0x02;

    /**
     * wrong current block size - bigger then Block Size Log [Object Transfer Start]
     */
    public static final byte STATUS_WRONG_BLOCK_SIZE = 0x03;

    /**
     * wrong Chunk Size - bigger then Block Size divided by Max Chunks Number [Object Information Status]
     */
    public static final byte STATUS_WRONG_CHUNK_SIZE = 0x04;

    /**
     * unknown checksum algorithm
     */
    public static final byte STATUS_UNKNOWN_CHECKSUM_ALGORITHM = 0x05;

    /**
     * block transfer rejected
     */
    public static final byte STATUS_BLOCK_TRANSFER_REJECTED = 0x0F;


    /**
     * size: 1 bytes
     */
    private byte status;

    public ObjectBlockTransferStatusMessage() {
    }

    protected ObjectBlockTransferStatusMessage(Parcel in) {
        status = in.readByte();
    }

    public static final Creator<ObjectBlockTransferStatusMessage> CREATOR = new Creator<ObjectBlockTransferStatusMessage>() {
        @Override
        public ObjectBlockTransferStatusMessage createFromParcel(Parcel in) {
            return new ObjectBlockTransferStatusMessage(in);
        }

        @Override
        public ObjectBlockTransferStatusMessage[] newArray(int size) {
            return new ObjectBlockTransferStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        this.status = params[0];
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(status);
    }

    public byte getStatus() {
        return status;
    }

    @Override
    public String toString() {
        return "ObjectBlockTransferStatusMessage{" +
                "status=" + status +
                '}';
    }
}
