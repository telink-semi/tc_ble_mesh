package com.telink.ble.mesh.core.message.fastpv;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;
import java.util.Arrays;

/**
 * Created by kee on 2019/8/20.
 */

public class MeshAddressStatusMessage extends StatusMessage implements Parcelable {

    private byte[] mac;

    private int pid;


    public MeshAddressStatusMessage() {
    }


    protected MeshAddressStatusMessage(Parcel in) {
        mac = in.createByteArray();
        pid = in.readInt();
    }

    public static final Creator<MeshAddressStatusMessage> CREATOR = new Creator<MeshAddressStatusMessage>() {
        @Override
        public MeshAddressStatusMessage createFromParcel(Parcel in) {
            return new MeshAddressStatusMessage(in);
        }

        @Override
        public MeshAddressStatusMessage[] newArray(int size) {
            return new MeshAddressStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        int index = 0;
        final int macLen = 6;
        this.mac = new byte[6];
        System.arraycopy(params, 0, this.mac, 0, macLen);
        index += macLen;
        this.pid = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
    }


    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByteArray(mac);
        dest.writeInt(pid);
    }

    public byte[] getMac() {
        return mac;
    }

    public int getPid() {
        return pid;
    }

    @Override
    public String toString() {
        return "MeshAddressStatusMessage{" +
                "mac=" + Arrays.toString(mac) +
                ", pid=" + pid +
                '}';
    }
}
