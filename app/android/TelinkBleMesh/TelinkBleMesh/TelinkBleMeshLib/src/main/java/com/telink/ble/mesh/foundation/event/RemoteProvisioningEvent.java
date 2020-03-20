package com.telink.ble.mesh.foundation.event;


import android.os.Parcel;

import com.telink.ble.mesh.entity.RemoteProvisioningDevice;
import com.telink.ble.mesh.foundation.Event;

/**
 * Created by kee on 2017/8/30.
 */

public class RemoteProvisioningEvent extends Event<String> {

    public static final String EVENT_TYPE_REMOTE_PROVISIONING_SUCCESS = "com.telink.sig.mesh.EVENT_TYPE_REMOTE_PROVISIONING_SUCCESS";

    public static final String EVENT_TYPE_REMOTE_PROVISIONING_FAIL = "com.telink.sig.mesh.EVENT_TYPE_REMOTE_PROVISIONING_FAIL";

    private RemoteProvisioningDevice remoteProvisioningDevice;
    private String desc;

    public RemoteProvisioningEvent(Object sender, String type) {
        super(sender, type);
    }

    protected RemoteProvisioningEvent(Parcel in) {
        remoteProvisioningDevice = in.readParcelable(RemoteProvisioningDevice.class.getClassLoader());
        desc = in.readString();
    }

    public static final Creator<RemoteProvisioningEvent> CREATOR = new Creator<RemoteProvisioningEvent>() {
        @Override
        public RemoteProvisioningEvent createFromParcel(Parcel in) {
            return new RemoteProvisioningEvent(in);
        }

        @Override
        public RemoteProvisioningEvent[] newArray(int size) {
            return new RemoteProvisioningEvent[size];
        }
    };

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeParcelable(remoteProvisioningDevice, flags);
        dest.writeString(desc);
    }

    public RemoteProvisioningDevice getRemoteProvisioningDevice() {
        return remoteProvisioningDevice;
    }

    public void setRemoteProvisioningDevice(RemoteProvisioningDevice remoteProvisioningDevice) {
        this.remoteProvisioningDevice = remoteProvisioningDevice;
    }

    public String getDesc() {
        return desc;
    }

    public void setDesc(String desc) {
        this.desc = desc;
    }
}
