package com.telink.ble.mesh.core.message.rp;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;

/**
 * Created by kee on 2019/8/20.
 */

public class ScanReportStatusMessage extends StatusMessage implements Parcelable {

    private byte rssi;

    // 16 bytes
    private byte[] uuid;

    // 2 bytes
    private int oob;

    public ScanReportStatusMessage() {
    }

    protected ScanReportStatusMessage(Parcel in) {
        rssi = in.readByte();
        uuid = in.createByteArray();
        oob = in.readInt();
    }

    public static final Creator<ScanReportStatusMessage> CREATOR = new Creator<ScanReportStatusMessage>() {
        @Override
        public ScanReportStatusMessage createFromParcel(Parcel in) {
            return new ScanReportStatusMessage(in);
        }

        @Override
        public ScanReportStatusMessage[] newArray(int size) {
            return new ScanReportStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.rssi = params[index++];
        this.uuid = new byte[16];
        System.arraycopy(params, index, this.uuid, 0, this.uuid.length);
        index += this.uuid.length;
        this.oob = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(rssi);
        dest.writeByteArray(uuid);
        dest.writeInt(oob);
    }

    public byte getRssi() {
        return rssi;
    }

    public byte[] getUuid() {
        return uuid;
    }

    public int getOob() {
        return oob;
    }
}
