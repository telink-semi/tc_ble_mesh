package com.telink.ble.mesh.core.message.config;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.message.StatusMessage;

/**
 * node reset status is empty message
 * Created by kee on 2019/9/18.
 */

public class NodeResetStatusMessage extends StatusMessage implements Parcelable {

    public NodeResetStatusMessage() {
    }

    protected NodeResetStatusMessage(Parcel in) {

    }

    public static final Creator<NodeResetStatusMessage> CREATOR = new Creator<NodeResetStatusMessage>() {
        @Override
        public NodeResetStatusMessage createFromParcel(Parcel in) {
            return new NodeResetStatusMessage(in);
        }

        @Override
        public NodeResetStatusMessage[] newArray(int size) {
            return new NodeResetStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {

    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
    }
}
