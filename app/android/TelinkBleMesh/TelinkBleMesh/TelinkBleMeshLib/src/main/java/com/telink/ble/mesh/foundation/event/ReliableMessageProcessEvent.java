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
 * This class represents an event related to the processing of reliable messages in a mesh network.
 * It extends the Event class and implements the Parcelable interface for serialization purposes.
 */
public class ReliableMessageProcessEvent extends Event<String> implements Parcelable {

    /**
     * mesh message send fail, because of busy or key not found
     * Event type for when a mesh message processing error occurs.
     */
    public static final String EVENT_TYPE_MSG_PROCESS_ERROR = "com.telink.sig.mesh.EVENT_TYPE_CMD_ERROR";

    /**
     * Event type for when a mesh message is being processed.
     */
    public static final String EVENT_TYPE_MSG_PROCESSING = "com.telink.sig.mesh.EVENT_TYPE_CMD_PROCESSING";

    /**
     * mesh message send complete, success or retry max
     * Event type for when a mesh message processing is complete, either successfully or after reaching the maximum number of retries.
     */
    public static final String EVENT_TYPE_MSG_PROCESS_COMPLETE = "com.telink.sig.mesh.EVENT_TYPE_CMD_COMPLETE";

    private boolean success; // Indicates if the message processing was successful
    private int opcode; // The opcode of the message
    private int rspMax; // The maximum number of responses expected
    private int rspCount; // The number of responses received
    private String desc; // Description of the event

    /**
     * Constructor for creating a ReliableMessageProcessEvent object.
     *
     * @param sender the object that sends the event
     * @param type   the type of the event
     */
    public ReliableMessageProcessEvent(Object sender, String type) {
        super(sender, type);
    }

    /**
     * Constructor for creating a ReliableMessageProcessEvent object with additional parameters.
     *
     * @param sender   the object that sends the event
     * @param type     the type of the event
     * @param success  indicates if the message processing was successful
     * @param opcode   the opcode of the message
     * @param rspMax   the maximum number of responses expected
     * @param rspCount the number of responses received
     * @param desc     the description of the event
     */
    public ReliableMessageProcessEvent(Object sender, String type, boolean success, int opcode, int rspMax, int rspCount, String desc) {
        super(sender, type);
        this.success = success;
        this.opcode = opcode;
        this.rspMax = rspMax;
        this.rspCount = rspCount;
        this.desc = desc;
    }

    /**
     * Constructor for creating a ReliableMessageProcessEvent object from a Parcel.
     *
     * @param in the Parcel object to read from
     */
    protected ReliableMessageProcessEvent(Parcel in) {
        success = in.readByte() != 0;
        opcode = in.readInt();
        rspMax = in.readInt();
        rspCount = in.readInt();
        desc = in.readString();
    }

    /**
     * Creator object for Parcelable implementation.
     */
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

    /**
     * Getter for the success property.
     *
     * @return true if the message processing was successful, false otherwise
     */
    public boolean isSuccess() {
        return success;
    }

    /**
     * Setter for the success property.
     *
     * @param success the new value for the success property
     */
    public void setSuccess(boolean success) {
        this.success = success;
    }

    /**
     * Getter for the opcode property.
     *
     * @return the opcode of the message
     */
    public int getOpcode() {
        return opcode;
    }

    /**
     * Setter for the opcode property.
     *
     * @param opcode the new value for the opcode property
     */
    public void setOpcode(int opcode) {
        this.opcode = opcode;
    }

    /**
     * Getter for the rspMax property.
     *
     * @return the maximum number of responses expected
     */
    public int getRspMax() {
        return rspMax;
    }

    /**
     * Setter for the rspMax property.
     *
     * @param rspMax the new value for the rspMax property
     */
    public void setRspMax(int rspMax) {
        this.rspMax = rspMax;
    }

    /**
     * Getter for the rspCount property.
     *
     * @return the number of responses received
     */
    public int getRspCount() {
        return rspCount;
    }

    /**
     * Setter for the rspCount property.
     *
     * @param rspCount the new value for the rspCount property
     */
    public void setRspCount(int rspCount) {
        this.rspCount = rspCount;
    }

    /**
     * Getter for the desc property.
     *
     * @return the description of the event
     */
    public String getDesc() {
        return desc;
    }

    /**
     * Setter for the desc property.
     *
     * @param desc the new value for the desc property
     */
    public void setDesc(String desc) {
        this.desc = desc;
    }

    /**
     * Implementation of the describeContents method from the Parcelable interface.
     *
     * @return always returns 0
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Implementation of the writeToParcel method from the Parcelable interface.
     *
     * @param dest  the Parcel object to write to
     * @param flags additional flags for writing
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte((byte) (success ? 1 : 0));
        dest.writeInt(opcode);
        dest.writeInt(rspMax);
        dest.writeInt(rspCount);
        dest.writeString(desc);
    }
}