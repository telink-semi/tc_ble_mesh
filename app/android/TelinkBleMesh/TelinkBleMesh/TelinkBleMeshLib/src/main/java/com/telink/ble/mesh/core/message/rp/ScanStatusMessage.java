package com.telink.ble.mesh.core.message.rp;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.message.StatusMessage;

/**
 * Created by kee on 2019/8/20.
 */

public class ScanStatusMessage extends StatusMessage implements Parcelable {

    private byte status;

    private byte rpScanningState;

    private byte scannedItemsLimit;

    private byte timeout;

    public ScanStatusMessage() {
    }


    protected ScanStatusMessage(Parcel in) {
        status = in.readByte();
        rpScanningState = in.readByte();
        scannedItemsLimit = in.readByte();
        timeout = in.readByte();
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(status);
        dest.writeByte(rpScanningState);
        dest.writeByte(scannedItemsLimit);
        dest.writeByte(timeout);
    }

    @Override
    public int describeContents() {
        return 0;
    }

    public static final Creator<ScanStatusMessage> CREATOR = new Creator<ScanStatusMessage>() {
        @Override
        public ScanStatusMessage createFromParcel(Parcel in) {
            return new ScanStatusMessage(in);
        }

        @Override
        public ScanStatusMessage[] newArray(int size) {
            return new ScanStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.status = params[index++];
        this.rpScanningState = params[index++];
        this.scannedItemsLimit = params[index++];
        this.timeout = params[index];
    }

    public byte getStatus() {
        return status;
    }

    public byte getRpScanningState() {
        return rpScanningState;
    }

    public byte getScannedItemsLimit() {
        return scannedItemsLimit;
    }

    public byte getTimeout() {
        return timeout;
    }
}
