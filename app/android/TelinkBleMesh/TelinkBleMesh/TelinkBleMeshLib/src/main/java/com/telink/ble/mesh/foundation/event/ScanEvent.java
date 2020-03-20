package com.telink.ble.mesh.foundation.event;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.entity.AdvertisingDevice;
import com.telink.ble.mesh.foundation.Event;

/**
 * Created by kee on 2019/9/5.
 */

public class ScanEvent extends Event<String> implements Parcelable {

    public static final String EVENT_TYPE_DEVICE_FOUND = "com.telink.ble.mesh.EVENT_TYPE_DEVICE_FOUND";

    public static final String EVENT_TYPE_SCAN_TIMEOUT = "com.telink.ble.mesh.EVENT_TYPE_SCAN_TIMEOUT";

    public static final String EVENT_TYPE_SCAN_FAIL = "com.telink.ble.mesh.EVENT_TYPE_SCAN_FAIL";

    private AdvertisingDevice advertisingDevice;

    public ScanEvent(Object sender, String type, AdvertisingDevice advertisingDevice) {
        super(sender, type);
        this.advertisingDevice = advertisingDevice;
    }

    public ScanEvent(Object sender, String type) {
        super(sender, type);
    }

    protected ScanEvent(Parcel in) {
        advertisingDevice = in.readParcelable(AdvertisingDevice.class.getClassLoader());
    }

    public static final Creator<ScanEvent> CREATOR = new Creator<ScanEvent>() {
        @Override
        public ScanEvent createFromParcel(Parcel in) {
            return new ScanEvent(in);
        }

        @Override
        public ScanEvent[] newArray(int size) {
            return new ScanEvent[size];
        }
    };

    public AdvertisingDevice getAdvertisingDevice() {
        return advertisingDevice;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeParcelable(advertisingDevice, flags);
    }
}
