package com.telink.ble.mesh.core.message.firmwareupdate;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;

public class FirmwareUpdateStatusMessage extends StatusMessage implements Parcelable {


    /**
     * @see UpdateStatus
     * 3 lower bits in first byte
     */
    private int status;

    /**
     * 3 higher bits in first byte (2 bits rfu)
     */
    private int phase;

    /**
     * Time To Live value to use during firmware image transfer
     * 1 byte
     */
    private byte updateTtl;


    /**
     * 5 bits (3 bits rfu)
     */
    private int additionalInfo;

    /**
     * Used to compute the timeout of the firmware image transfer
     * Client Timeout = [12,000 * (Client Timeout Base + 1) + 100 * Transfer TTL] milliseconds
     * using blockSize
     * 2 bytes
     */
    private int updateTimeoutBase;

    /**
     * BLOB identifier for the firmware image
     * 8 bytes
     */
    private long updateBLOBID;

    /**
     * length: 1 byte
     */
    private int updateFirmwareImageIndex;

    /**
     * If the Update TTL field is present,
     * the Additional Information field, Update Timeout field, BLOB ID field, and Installed Firmware ID field shall be present;
     * otherwise, the Additional Information field, Update Timeout field, BLOB ID field, and Installed Firmware ID field shall not be present.
     */
    private boolean isComplete = false;

    public FirmwareUpdateStatusMessage() {
    }


    protected FirmwareUpdateStatusMessage(Parcel in) {
        status = in.readInt();
        phase = in.readInt();
        updateTtl = in.readByte();
        additionalInfo = in.readInt();
        updateTimeoutBase = in.readInt();
        updateBLOBID = in.readLong();
        updateFirmwareImageIndex = in.readInt();
        isComplete = in.readByte() != 0;
    }

    public static final Creator<FirmwareUpdateStatusMessage> CREATOR = new Creator<FirmwareUpdateStatusMessage>() {
        @Override
        public FirmwareUpdateStatusMessage createFromParcel(Parcel in) {
            return new FirmwareUpdateStatusMessage(in);
        }

        @Override
        public FirmwareUpdateStatusMessage[] newArray(int size) {
            return new FirmwareUpdateStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        int index = 0;

        this.status = params[index] & 0x07;

        this.phase = (params[index++] & 0xFF) >> 5;

        this.updateTtl = params[index];

        isComplete = params.length > 2;
        if (!isComplete) return;
        index++;

        this.additionalInfo = (params[index++] & 0x1F);

        this.updateTimeoutBase = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;

        this.updateBLOBID = MeshUtils.bytes2Integer(params, index, 8, ByteOrder.LITTLE_ENDIAN);
        index += 8;

        this.updateFirmwareImageIndex = params[index] & 0xFF;

    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(status);
        dest.writeInt(phase);
        dest.writeByte(updateTtl);
        dest.writeInt(additionalInfo);
        dest.writeInt(updateTimeoutBase);
        dest.writeLong(updateBLOBID);
        dest.writeInt(updateFirmwareImageIndex);
        dest.writeByte((byte) (isComplete ? 1 : 0));
    }

    public int getStatus() {
        return status;
    }

    public int getPhase() {
        return phase;
    }

    public byte getUpdateTtl() {
        return updateTtl;
    }

    public int getAdditionalInfo() {
        return additionalInfo;
    }

    public int getUpdateTimeoutBase() {
        return updateTimeoutBase;
    }

    public long getUpdateBLOBID() {
        return updateBLOBID;
    }

    public int getUpdateFirmwareImageIndex() {
        return updateFirmwareImageIndex;
    }

    public boolean isComplete() {
        return isComplete;
    }
}
