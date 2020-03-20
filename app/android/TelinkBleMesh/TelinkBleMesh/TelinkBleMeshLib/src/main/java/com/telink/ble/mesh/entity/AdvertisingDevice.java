
package com.telink.ble.mesh.entity;

import android.bluetooth.BluetoothDevice;
import android.os.Parcel;
import android.os.Parcelable;

/**
 * scanned devices
 */
public class AdvertisingDevice implements Parcelable {
    public BluetoothDevice device;
    public int rssi;
    public byte[] scanRecord;

    public AdvertisingDevice(BluetoothDevice device, int rssi, byte[] scanRecord) {
        this.device = device;
        this.rssi = rssi;
        this.scanRecord = scanRecord;
    }

    public static final Creator<AdvertisingDevice> CREATOR = new Creator<AdvertisingDevice>() {
        @Override
        public AdvertisingDevice createFromParcel(Parcel in) {
            return new AdvertisingDevice(in);
        }

        @Override
        public AdvertisingDevice[] newArray(int size) {
            return new AdvertisingDevice[size];
        }
    };

    public AdvertisingDevice(Parcel in) {
        this.device = in.readParcelable(getClass().getClassLoader());
        this.rssi = in.readInt();
        this.scanRecord = new byte[in.readInt()];
        in.readByteArray(this.scanRecord);
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeParcelable(this.device, 0);
        dest.writeInt(this.rssi);
        dest.writeInt(this.scanRecord.length);
        dest.writeByteArray(this.scanRecord);
    }

    @Override
    public boolean equals(Object obj) {
        return obj instanceof AdvertisingDevice && ((AdvertisingDevice) obj).device.equals(device);
    }
}
