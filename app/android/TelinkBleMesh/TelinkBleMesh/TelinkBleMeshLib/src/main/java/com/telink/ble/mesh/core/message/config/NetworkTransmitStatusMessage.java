package com.telink.ble.mesh.core.message.config;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.message.StatusMessage;

/**
 * Created by kee on 2019/9/10.
 */

public class NetworkTransmitStatusMessage extends StatusMessage implements Parcelable {

    private int count;

    private int intervalSteps;


    public NetworkTransmitStatusMessage() {
    }


    protected NetworkTransmitStatusMessage(Parcel in) {
        count = in.readInt();
        intervalSteps = in.readInt();
    }

    public static final Creator<NetworkTransmitStatusMessage> CREATOR = new Creator<NetworkTransmitStatusMessage>() {
        @Override
        public NetworkTransmitStatusMessage createFromParcel(Parcel in) {
            return new NetworkTransmitStatusMessage(in);
        }

        @Override
        public NetworkTransmitStatusMessage[] newArray(int size) {
            return new NetworkTransmitStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] data) {
        int index = 0;
        this.count = data[0] & 0b111;
        this.intervalSteps = (data[0] & 0xFF) >> 3;
    }

    public int getCount() {
        return count;
    }

    public int getIntervalSteps() {
        return intervalSteps;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(count);
        dest.writeInt(intervalSteps);
    }
}
