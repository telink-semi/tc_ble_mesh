/********************************************************************************************************
 * @file SchedulerRegister.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2017
 *
 * @par Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
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
package com.telink.ble.mesh.model;

import android.os.Parcel;
import android.os.Parcelable;

import java.io.Serializable;

public class SchedulerRegister implements Serializable, Parcelable {
    public long id;

    public long year;
    public long month;
    public long day;
    public long hour;
    public long minute;
    public long second;
    public long week;
    public long action;
    public long transTime;
    public int sceneId;

    public SchedulerRegister() {

    }

    protected SchedulerRegister(byte year,
                              short month,
                              byte day,
                              byte hour,
                              byte minute,
                              byte second,
                              byte week,
                              byte action,
                              byte transTime,
                              int sceneId) {
        this.year = year & 0xFF;
        this.month = month & 0xFFFF;
        this.day = day & 0xFF;
        this.hour = hour & 0xFF;
        this.minute = minute & 0xFF;
        this.second = second & 0xFF;
        this.week = week & 0xFF;
        this.action = action & 0xFF;
        this.transTime = transTime & 0xFF;
        this.sceneId = sceneId & 0xFFFF;
    }


    protected SchedulerRegister(Parcel in) {
        year = in.readLong();
        month = in.readLong();
        day = in.readLong();
        hour = in.readLong();
        minute = in.readLong();
        second = in.readLong();
        week = in.readLong();
        action = in.readLong();
        transTime = in.readLong();
        sceneId = in.readInt();
    }

    public static final Creator<SchedulerRegister> CREATOR = new Creator<SchedulerRegister>() {
        @Override
        public SchedulerRegister createFromParcel(Parcel in) {
            return new SchedulerRegister(in);
        }

        @Override
        public SchedulerRegister[] newArray(int size) {
            return new SchedulerRegister[size];
        }
    };

    public long getYear() {
        return year;
    }

    public long getMonth() {
        return month;
    }

    public long getDay() {
        return day;
    }

    public long getHour() {
        return hour;
    }

    public long getMinute() {
        return minute;
    }

    public long getSecond() {
        return second;
    }

    public long getWeek() {
        return week;
    }

    public long getAction() {
        return action;
    }

    public long getTransTime() {
        return transTime;
    }

    public int getSceneId() {
        return sceneId;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeLong(year);
        dest.writeLong(month);
        dest.writeLong(day);
        dest.writeLong(hour);
        dest.writeLong(minute);
        dest.writeLong(second);
        dest.writeLong(week);
        dest.writeLong(action);
        dest.writeLong(transTime);
        dest.writeInt(sceneId);
    }
}