package com.telink.ble.mesh.core.message.updating;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;

public class FirmwareUpdateStatusMessage extends StatusMessage implements Parcelable {

    /**
     * No DFU update in progress
     */
    public static final byte PHASE_IDLE = 0x00;

    /**
     * DFU update is prepared and awaiting start
     */
    public static final byte PHASE_PREPARED = 0x01;

    /**
     * DFU update is in progress
     */
    public static final byte PHASE_IN_PROGRESS = 0x02;

    /**
     * DFU upload is finished and waiting to be applied
     */
    public static final byte PHASE_READY = 0x03;

    /**
     * bit 0,
     * When set to 1 the device be in unprovisioned state after the new FW is applied
     * (possibly due to new models added).
     */
    private static final byte ADDITIONAL_INFO_BIT_MASK = 0b01;

    /**
     * success
     */
    public static final byte STATUS_SUCCESS = 0x00;

    /**
     * wrong Company ID and Firmware ID combination
     */
    public static final byte STATUS_ID_WRONG = 0x01;

    /**
     * different object transfer already ongoing
     */
    public static final byte STATUS_OBJECT_ERR = 0x02;

    /**
     * Company ID and Firmware ID combination apply failed
     */
    public static final byte STATUS_APPLY_FAILED = 0x03;

    /**
     * Company ID and Firmware ID combination permanently rejected, newer firmware version present
     */
    public static final byte STATUS_PERMANENTLY_REJECTED = 0x04;

    /**
     * Company ID and Firmware ID combination temporary rejected,
     * node is not able to accept new firmware now, try again later
     */
    public static final byte STATUS_TEMPORARY_REJECTED = 0x05;

    /**
     * size: 1 bytes
     */
    private byte status;

    /**
     * 3 bits
     */
    private byte phase;

    /**
     * 5 bits
     */
    private byte additionalInfo;

    /**
     * size: 2 bytes
     */
    private int companyId;

    /**
     * size: 1 + N bytes
     * 4 currently
     */
    private int firmwareId;

    /**
     * 8 bytes, optional
     */
    private long objectId;

    private boolean isComplete = false;

    public FirmwareUpdateStatusMessage() {
    }


    protected FirmwareUpdateStatusMessage(Parcel in) {
        status = in.readByte();
        phase = in.readByte();
        additionalInfo = in.readByte();
        companyId = in.readInt();
        firmwareId = in.readInt();
        objectId = in.readLong();
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
        this.status = params[index++];

        this.phase = (byte) (params[index] & 0x07);
        this.additionalInfo = (byte) ((params[index] & 0xFF) >> 3);
        index += 1;

        this.companyId = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;

        this.firmwareId = MeshUtils.bytes2Integer(params, index, 4, ByteOrder.LITTLE_ENDIAN);
        index += 4;

        if (params.length > index) {
            this.isComplete = true;
            this.objectId = MeshUtils.bytes2Long(params, index, 8, ByteOrder.LITTLE_ENDIAN);
        }
    }

    public boolean provisionNeeded() {
        return (this.additionalInfo & ADDITIONAL_INFO_BIT_MASK) != 0;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {

        dest.writeByte(status);
        dest.writeByte(phase);
        dest.writeByte(additionalInfo);
        dest.writeInt(companyId);
        dest.writeInt(firmwareId);
        dest.writeLong(objectId);
        dest.writeByte((byte) (isComplete ? 1 : 0));
    }


    public byte getStatus() {
        return status;
    }

    public byte getPhase() {
        return phase;
    }

    public byte getAdditionalInfo() {
        return additionalInfo;
    }

    public int getCompanyId() {
        return companyId;
    }

    public int getFirmwareId() {
        return firmwareId;
    }

    public long getObjectId() {
        return objectId;
    }

    public boolean isComplete() {
        return isComplete;
    }

    @Override
    public String toString() {
        return "FirmwareUpdateStatusMessage{" +
                "status=" + status +
                ", phase=" + phase +
                ", additionalInfo=" + additionalInfo +
                ", companyId=0x" + Integer.toHexString(companyId) +
                ", firmwareId=0x" + Integer.toHexString(firmwareId) +
                ", objectId=0x" + Long.toHexString(objectId) +
                ", isComplete=" + isComplete +
                '}';
    }
}
