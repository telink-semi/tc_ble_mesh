package com.telink.ble.mesh.core.message.scheduler;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.message.StatusMessage;
import com.telink.ble.mesh.entity.Scheduler;

/**
 * Created by kee on 2019/8/20.
 */

public class SchedulerActionStatusMessage extends StatusMessage implements Parcelable {

    private Scheduler scheduler;

    public SchedulerActionStatusMessage() {
    }

    protected SchedulerActionStatusMessage(Parcel in) {
        scheduler = in.readParcelable(Scheduler.class.getClassLoader());
    }

    public static final Creator<SchedulerActionStatusMessage> CREATOR = new Creator<SchedulerActionStatusMessage>() {
        @Override
        public SchedulerActionStatusMessage createFromParcel(Parcel in) {
            return new SchedulerActionStatusMessage(in);
        }

        @Override
        public SchedulerActionStatusMessage[] newArray(int size) {
            return new SchedulerActionStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        this.scheduler = Scheduler.fromBytes(params);
    }

    public Scheduler getScheduler() {
        return scheduler;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeParcelable(scheduler, flags);
    }
}
