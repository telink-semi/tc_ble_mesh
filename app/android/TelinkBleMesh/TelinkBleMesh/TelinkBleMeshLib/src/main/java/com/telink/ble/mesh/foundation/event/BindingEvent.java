package com.telink.ble.mesh.foundation.event;

import android.os.Parcel;

import com.telink.ble.mesh.entity.BindingDevice;
import com.telink.ble.mesh.foundation.Event;

/**
 * Created by kee on 2019/9/4.
 */

public class BindingEvent extends Event<String> {
    public static final String EVENT_TYPE_BIND_SUCCESS = "com.telink.ble.mesh.EVENT_TYPE_BIND_SUCCESS";

    public static final String EVENT_TYPE_BIND_FAIL = "com.telink.ble.mesh.EVENT_TYPE_BIND_FAIL";

    private BindingDevice bindingDevice;

    private String desc;


    public BindingEvent(Object sender, String type, BindingDevice bindingDevice, String desc) {
        super(sender, type);
        this.bindingDevice = bindingDevice;
        this.desc = desc;
    }

    protected BindingEvent(Parcel in) {
        bindingDevice = in.readParcelable(BindingDevice.class.getClassLoader());
        desc = in.readString();
    }

    public static final Creator<BindingEvent> CREATOR = new Creator<BindingEvent>() {
        @Override
        public BindingEvent createFromParcel(Parcel in) {
            return new BindingEvent(in);
        }

        @Override
        public BindingEvent[] newArray(int size) {
            return new BindingEvent[size];
        }
    };

    public String getDesc() {
        return desc;
    }

    public void setDesc(String desc) {
        this.desc = desc;
    }

    public BindingDevice getBindingDevice() {
        return bindingDevice;
    }

    public BindingEvent(Object sender, String type) {
        super(sender, type);
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeParcelable(bindingDevice, flags);
        dest.writeString(desc);
    }
}
