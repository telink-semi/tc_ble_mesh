/********************************************************************************************************
 * @file ReliableMessageProcessEvent.java
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
package com.telink.ble.mesh.foundation.event;


import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.foundation.Event;

/**
 * Created by kee on 2017/8/30.
 */

public class ReliableMessageProcessEvent extends Event<String> implements Parcelable {

    /**
     * mesh message send fail, because of busy or key not found
     */
    public static final String EVENT_TYPE_MSG_PROCESS_ERROR = "com.telink.sig.mesh.EVENT_TYPE_CMD_ERROR";

    /**
     * mesh message processing
     */
    public static final String EVENT_TYPE_MSG_PROCESSING = "com.telink.sig.mesh.EVENT_TYPE_CMD_PROCESSING";

    /**
     * mesh message send complete, success or retry max
     */
    public static final String EVENT_TYPE_MSG_PROCESS_COMPLETE = "com.telink.sig.mesh.EVENT_TYPE_CMD_COMPLETE";

    private boolean success;

    // message opcode
    private int opcode;
    // message response max
    private int rspMax;

    // message response count
    private int rspCount;

    // event description
    private String desc;


    public ReliableMessageProcessEvent(Object sender, String type) {
        super(sender, type);
    }

    public ReliableMessageProcessEvent(Object sender, String type, boolean success, int opcode, int rspMax, int rspCount, String desc) {
        super(sender, type);
        this.success = success;
        this.opcode = opcode;
        this.rspMax = rspMax;
        this.rspCount = rspCount;
        this.desc = desc;
    }


    protected ReliableMessageProcessEvent(Parcel in) {
        success = in.readByte() != 0;
        opcode = in.readInt();
        rspMax = in.readInt();
        rspCount = in.readInt();
        desc = in.readString();
    }

    public static final Creator<ReliableMessageProcessEvent> CREATOR = new Creator<ReliableMessageProcessEvent>() {
        @Override
        public ReliableMessageProcessEvent createFromParcel(Parcel in) {
            return new ReliableMessageProcessEvent(in);
        }

        @Override
        public ReliableMessageProcessEvent[] newArray(int size) {
            return new ReliableMessageProcessEvent[size];
        }
    };

    public boolean isSuccess() {
        return success;
    }

    public void setSuccess(boolean success) {
        this.success = success;
    }

    public int getOpcode() {
        return opcode;
    }

    public void setOpcode(int opcode) {
        this.opcode = opcode;
    }

    public int getRspMax() {
        return rspMax;
    }

    public void setRspMax(int rspMax) {
        this.rspMax = rspMax;
    }

    public int getRspCount() {
        return rspCount;
    }

    public void setRspCount(int rspCount) {
        this.rspCount = rspCount;
    }

    public String getDesc() {
        return desc;
    }

    public void setDesc(String desc) {
        this.desc = desc;
    }


    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte((byte) (success ? 1 : 0));
        dest.writeInt(opcode);
        dest.writeInt(rspMax);
        dest.writeInt(rspCount);
        dest.writeString(desc);
    }
}
