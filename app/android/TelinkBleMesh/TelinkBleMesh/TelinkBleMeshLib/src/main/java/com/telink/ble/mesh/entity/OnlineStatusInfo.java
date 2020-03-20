
package com.telink.ble.mesh.entity;

import android.os.Parcel;
import android.os.Parcelable;

/**
 * online status
 */
public class OnlineStatusInfo implements Parcelable {

    public int address;

//        byte rsv; // 1 bit

    // sn: 0 offline
    public byte sn;

    public byte[] status;

    public OnlineStatusInfo() {
    }

    protected OnlineStatusInfo(Parcel in) {
        address = in.readInt();
        sn = in.readByte();
        status = in.createByteArray();
    }

    public static final Creator<OnlineStatusInfo> CREATOR = new Creator<OnlineStatusInfo>() {
        @Override
        public OnlineStatusInfo createFromParcel(Parcel in) {
            return new OnlineStatusInfo(in);
        }

        @Override
        public OnlineStatusInfo[] newArray(int size) {
            return new OnlineStatusInfo[size];
        }
    };

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(address);
        dest.writeByte(sn);
        dest.writeByteArray(status);
    }
}
