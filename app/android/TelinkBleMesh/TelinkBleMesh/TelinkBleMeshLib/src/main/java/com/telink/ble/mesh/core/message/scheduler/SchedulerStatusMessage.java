package com.telink.ble.mesh.core.message.scheduler;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;

/**
 * Created by kee on 2019/8/20.
 */

public class SchedulerStatusMessage extends StatusMessage implements Parcelable {


    /**
     * Bit field indicating defined Actions in the Schedule Register
     */
    private int schedules;

    public SchedulerStatusMessage() {
    }

    protected SchedulerStatusMessage(Parcel in) {
        schedules = in.readInt();
    }

    public static final Creator<SchedulerStatusMessage> CREATOR = new Creator<SchedulerStatusMessage>() {
        @Override
        public SchedulerStatusMessage createFromParcel(Parcel in) {
            return new SchedulerStatusMessage(in);
        }

        @Override
        public SchedulerStatusMessage[] newArray(int size) {
            return new SchedulerStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        this.schedules = MeshUtils.bytes2Integer(params, ByteOrder.LITTLE_ENDIAN);
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeLong(schedules);
    }

    public long getSchedules() {
        return schedules;
    }
}
