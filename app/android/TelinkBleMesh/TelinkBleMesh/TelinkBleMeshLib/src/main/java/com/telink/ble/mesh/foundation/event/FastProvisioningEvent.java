package com.telink.ble.mesh.foundation.event;


import android.os.Parcel;

import com.telink.ble.mesh.entity.FastProvisioningDevice;
import com.telink.ble.mesh.foundation.Event;

/**
 * Created by kee on 2017/8/30.
 */

public class FastProvisioningEvent extends Event<String> {

    public static final String EVENT_TYPE_FAST_PROVISIONING_ADDRESS_SET = "com.telink.sig.mesh.EVENT_TYPE_FAST_PROVISIONING_ADDRESS_SET";

    public static final String EVENT_TYPE_FAST_PROVISIONING_SUCCESS = "com.telink.sig.mesh.EVENT_TYPE_FAST_PROVISIONING_SUCCESS";

    public static final String EVENT_TYPE_FAST_PROVISIONING_FAIL = "com.telink.sig.mesh.EVENT_TYPE_FAST_PROVISIONING_FAIL";

    private FastProvisioningDevice fastProvisioningDevice;
    private String desc;

    public FastProvisioningEvent(Object sender, String type) {
        super(sender, type);
    }

    protected FastProvisioningEvent(Parcel in) {
        fastProvisioningDevice = in.readParcelable(FastProvisioningDevice.class.getClassLoader());
        desc = in.readString();
    }

    public static final Creator<FastProvisioningEvent> CREATOR = new Creator<FastProvisioningEvent>() {
        @Override
        public FastProvisioningEvent createFromParcel(Parcel in) {
            return new FastProvisioningEvent(in);
        }

        @Override
        public FastProvisioningEvent[] newArray(int size) {
            return new FastProvisioningEvent[size];
        }
    };

    public String getDesc() {
        return desc;
    }

    public void setDesc(String desc) {
        this.desc = desc;
    }

    public FastProvisioningDevice getFastProvisioningDevice() {
        return fastProvisioningDevice;
    }

    public void setFastProvisioningDevice(FastProvisioningDevice fastProvisioningDevice) {
        this.fastProvisioningDevice = fastProvisioningDevice;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeParcelable(fastProvisioningDevice, flags);
        dest.writeString(desc);
    }
}
