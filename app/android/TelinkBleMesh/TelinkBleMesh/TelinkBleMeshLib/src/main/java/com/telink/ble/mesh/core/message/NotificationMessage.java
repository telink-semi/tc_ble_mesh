/********************************************************************************************************
 * @file NotificationMessage.java
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
package com.telink.ble.mesh.core.message;

import android.os.Parcel;
import android.os.Parcelable;

/**
 * The NotificationMessage class is used to notify changes in the mesh status.
 * <p>
 * It contains information such as the source, destination, opcode, and raw parameters data.
 * The statusMessage field stores the parsed message based on the opcode and parameters.
 * If the opcode is registered in the MeshStatus.Container class, the statusMessage will be populated.
 * Otherwise, it will be null.
 * <p>
 * This class implements the Parcelable interface to allow for easy serialization and deserialization.
 * <p>
 * Created by kee on 2019/9/3.
 */
public class NotificationMessage implements Parcelable {
    private int src;
    private int dst;
    private int opcode;

    // params raw data
    private byte[] params;

    /**
     * parsed message by params, if opcode is registered in {@link MeshStatus.Container}
     * otherwise statusMessage will be null
     */
    private StatusMessage statusMessage;

    /**
     * Constructs a new NotificationMessage with the specified source, destination, opcode, and parameters.
     *
     * @param src    the source of the message
     * @param dst    the destination of the message
     * @param opcode the opcode of the message
     * @param params the raw parameters data of the message
     */
    public NotificationMessage(int src, int dst, int opcode, byte[] params) {
        this.src = src;
        this.dst = dst;
        this.opcode = opcode;
        this.params = params;
        parseStatusMessage();
    }

    private void parseStatusMessage() {
        this.statusMessage = StatusMessage.createByAccessMessage(opcode, params);
    }

    /**
     * Constructs a new NotificationMessage by deserializing it from a Parcel.
     *
     * @param in the Parcel to read from
     */
    protected NotificationMessage(Parcel in) {
        src = in.readInt();
        dst = in.readInt();
        opcode = in.readInt();
        params = in.createByteArray();
        statusMessage = in.readParcelable(StatusMessage.class.getClassLoader());
    }

    /**
     * A Creator object that is used to create instances of NotificationMessage from a Parcel.
     */
    public static final Creator<NotificationMessage> CREATOR = new Creator<NotificationMessage>() {
        @Override
        public NotificationMessage createFromParcel(Parcel in) {
            return new NotificationMessage(in);
        }

        @Override
        public NotificationMessage[] newArray(int size) {
            return new NotificationMessage[size];
        }
    };

    /**
     * Returns a bitmask indicating the set of special object types
     * marshaled by this Parcelable object instance.
     *
     * @return a bitmask indicating the set of special object types
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Writes the NotificationMessage object's data to the passed-in Parcel.
     *
     * @param dest  the Parcel to write to
     * @param flags additional flags about how the object should be written
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(src);
        dest.writeInt(dst);
        dest.writeInt(opcode);
        dest.writeByteArray(params);
        dest.writeParcelable(statusMessage, flags);
    }

    /**
     * Returns the source of the message.
     *
     * @return the source of the message
     */
    public int getSrc() {
        return src;
    }

    /**
     * Sets the source of the message.
     *
     * @param src the source of the message
     */
    public void setSrc(int src) {
        this.src = src;
    }

    /**
     * Returns the destination of the message.
     *
     * @return the destination of the message
     */
    public int getDst() {
        return dst;
    }

    /**
     * Sets the destination of the message.
     *
     * @param dst the destination of the message
     */
    public void setDst(int dst) {
        this.dst = dst;
    }

    /**
     * Returns the opcode of the message.
     *
     * @return the opcode of the message
     */
    public int getOpcode() {
        return opcode;
    }

    /**
     * Sets the opcode of the message.
     *
     * @param opcode the opcode of the message
     */
    public void setOpcode(int opcode) {
        this.opcode = opcode;
    }

    /**
     * Returns the raw parameters data of the message.
     *
     * @return the raw parameters data of the message
     */
    public byte[] getParams() {
        return params;
    }

    /**
     * Sets the raw parameters data of the message.
     *
     * @param params the raw parameters data of the message
     */
    public void setParams(byte[] params) {
        this.params = params;
    }

    /**
     * Returns the parsed status message based on the opcode and parameters.
     *
     * @return the parsed status message
     */
    public StatusMessage getStatusMessage() {
        return statusMessage;
    }

    /**
     * Sets the parsed status message.
     *
     * @param statusMessage the parsed status message
     */
    public void setStatusMessage(StatusMessage statusMessage) {
        this.statusMessage = statusMessage;
    }
}
