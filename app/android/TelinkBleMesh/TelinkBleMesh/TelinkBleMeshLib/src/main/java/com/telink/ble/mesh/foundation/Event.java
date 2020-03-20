
package com.telink.ble.mesh.foundation;

import android.os.Parcel;
import android.os.Parcelable;

public abstract class Event<T> implements Parcelable {

    protected Object sender;
    protected T type;
    protected ThreadMode threadMode = ThreadMode.Default;


    public Event() {
    }

    public Event(Object sender, T type) {
        this(sender, type, ThreadMode.Default);
    }

    public Event(Object sender, T type, ThreadMode threadMode) {
        this.sender = sender;
        this.type = type;
        this.threadMode = threadMode;
    }

    public Object getSender() {
        return sender;
    }

    public T getType() {
        return type;
    }

    public ThreadMode getThreadMode() {
        return this.threadMode;
    }

    public Event<T> setThreadMode(ThreadMode mode) {
        this.threadMode = mode;
        return this;
    }

    public enum ThreadMode {
        Background, Main, Default,
        ;
    }
}