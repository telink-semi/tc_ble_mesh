/********************************************************************************************************
 * @file SchedulerActionStatusMessage.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date     Sep. 30, 2017
 *
 * @par     Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/
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
