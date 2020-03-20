package com.telink.ble.mesh.foundation.event;


import android.os.Parcel;

import com.telink.ble.mesh.entity.MeshUpdatingDevice;
import com.telink.ble.mesh.foundation.Event;

/**
 * Created by kee on 2017/8/30.
 */

public class MeshUpdatingEvent extends Event<String> {

    public static final String EVENT_TYPE_UPDATING_SUCCESS = "com.telink.sig.mesh.EVENT_TYPE_UPDATING_SUCCESS";

    public static final String EVENT_TYPE_UPDATING_FAIL = "com.telink.sig.mesh.EVENT_TYPE_UPDATING_FAIL";

    public static final String EVENT_TYPE_UPDATING_PROGRESS = "com.telink.sig.mesh.EVENT_TYPE_UPDATING_PROGRESS";

    public static final String EVENT_TYPE_UPDATING_STOPPED = "com.telink.sig.mesh.EVENT_TYPE_UPDATING_STOPPED";

    public static final String EVENT_TYPE_DEVICE_SUCCESS = "com.telink.sig.mesh.EVENT_TYPE_DEVICE_SUCCESS";

    public static final String EVENT_TYPE_DEVICE_FAIL = "com.telink.sig.mesh.EVENT_TYPE_DEVICE_FAIL";

    private MeshUpdatingDevice updatingDevice;
    private int progress;
    private String desc;

    public MeshUpdatingEvent(Object sender, String type) {
        super(sender, type);
    }

    protected MeshUpdatingEvent(Parcel in) {
        updatingDevice = in.readParcelable(MeshUpdatingDevice.class.getClassLoader());
        progress = in.readInt();
        desc = in.readString();
    }

    public static final Creator<MeshUpdatingEvent> CREATOR = new Creator<MeshUpdatingEvent>() {
        @Override
        public MeshUpdatingEvent createFromParcel(Parcel in) {
            return new MeshUpdatingEvent(in);
        }

        @Override
        public MeshUpdatingEvent[] newArray(int size) {
            return new MeshUpdatingEvent[size];
        }
    };

    public void setUpdatingDevice(MeshUpdatingDevice updatingDevice) {
        this.updatingDevice = updatingDevice;
    }

    public void setProgress(int progress) {
        this.progress = progress;
    }

    public void setDesc(String desc) {
        this.desc = desc;
    }

    public MeshUpdatingDevice getUpdatingDevice() {
        return updatingDevice;
    }

    public int getProgress() {
        return progress;
    }

    public String getDesc() {
        return desc;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeParcelable(updatingDevice, flags);
        dest.writeInt(progress);
        dest.writeString(desc);
    }
}
