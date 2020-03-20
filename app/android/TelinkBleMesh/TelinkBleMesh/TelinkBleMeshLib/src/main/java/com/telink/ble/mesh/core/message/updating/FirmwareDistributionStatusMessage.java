package com.telink.ble.mesh.core.message.updating;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;

/**
 * @deprecated
 */
public class FirmwareDistributionStatusMessage extends StatusMessage implements Parcelable {

    /**
     * ready, distribution is not active
     */
    public static final byte STATUS_READY = 0x00;

    /**
     * distribution is active
     */
    public static final byte STATUS_ACTIVE = 0x01;

    /**
     * no such Company ID and Firmware ID combination
     */
    public static final byte STATUS_ID_ERR = 0x02;

    /**
     * busy with different distribution
     */
    public static final byte STATUS_BUSY = 0x03;

    /**
     * update nodes list is too long
     */
    public static final byte STATUS_LIST_ERR = 0x04;

    /**
     * size: 1 bytes
     */
    private byte status;

    /**
     * size: 2 bytes
     */
    private int companyId;

    /**
     * size: 1 + N bytes
     */
    private int firmwareId;


    public FirmwareDistributionStatusMessage() {
    }


    protected FirmwareDistributionStatusMessage(Parcel in) {
        status = in.readByte();
        companyId = in.readInt();
        firmwareId = in.readInt();
    }

    public static final Creator<FirmwareDistributionStatusMessage> CREATOR = new Creator<FirmwareDistributionStatusMessage>() {
        @Override
        public FirmwareDistributionStatusMessage createFromParcel(Parcel in) {
            return new FirmwareDistributionStatusMessage(in);
        }

        @Override
        public FirmwareDistributionStatusMessage[] newArray(int size) {
            return new FirmwareDistributionStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.status = params[index++];
        this.companyId = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        this.firmwareId = MeshUtils.bytes2Integer(params, index, 4, ByteOrder.LITTLE_ENDIAN);
    }




    public byte getStatus() {
        return status;
    }

    public int getCompanyId() {
        return companyId;
    }

    public int getFirmwareId() {
        return firmwareId;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(status);
        dest.writeInt(companyId);
        dest.writeInt(firmwareId);
    }
}
