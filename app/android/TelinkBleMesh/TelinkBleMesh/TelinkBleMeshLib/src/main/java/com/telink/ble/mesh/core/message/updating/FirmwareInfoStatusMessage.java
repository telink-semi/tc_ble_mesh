package com.telink.ble.mesh.core.message.updating;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;
import java.util.Arrays;

public class FirmwareInfoStatusMessage extends StatusMessage implements Parcelable {

    /**
     * size: 2 bytes
     */
    private int companyId;

    /**
     * size: 1 + N bytes
     */
    private int firmwareId;

    /**
     * size: N bytes
     * optional
     */
    private byte[] updateUrl;

    private boolean isComplete = false;

    public FirmwareInfoStatusMessage() {
    }


    protected FirmwareInfoStatusMessage(Parcel in) {
        companyId = in.readInt();
        firmwareId = in.readInt();
        updateUrl = in.createByteArray();
        isComplete = in.readByte() != 0;
    }

    public static final Creator<FirmwareInfoStatusMessage> CREATOR = new Creator<FirmwareInfoStatusMessage>() {
        @Override
        public FirmwareInfoStatusMessage createFromParcel(Parcel in) {
            return new FirmwareInfoStatusMessage(in);
        }

        @Override
        public FirmwareInfoStatusMessage[] newArray(int size) {
            return new FirmwareInfoStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.companyId = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;

        this.firmwareId = MeshUtils.bytes2Integer(params, index, 4, ByteOrder.LITTLE_ENDIAN);
        index += 4;

        if (params.length > index) {
            this.isComplete = true;
            int restLen = params.length - index;
            this.updateUrl = new byte[restLen];
            System.arraycopy(params, index, this.updateUrl, 0, restLen);
        }
    }


    public int getCompanyId() {
        return companyId;
    }

    public int getFirmwareId() {
        return firmwareId;
    }

    public byte[] getUpdateUrl() {
        return updateUrl;
    }

    public boolean isComplete() {
        return isComplete;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(companyId);
        dest.writeInt(firmwareId);
        dest.writeByteArray(updateUrl);
        dest.writeByte((byte) (isComplete ? 1 : 0));
    }

    @Override
    public String toString() {
        return "FirmwareInfoStatusMessage{" +
                "companyId=" + companyId +
                ", firmwareId=" + firmwareId +
                ", updateUrl=" + Arrays.toString(updateUrl) +
                ", isComplete=" + isComplete +
                '}';
    }
}
