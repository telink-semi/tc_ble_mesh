/********************************************************************************************************
 * @file ModelAppStatusMessage.java
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
package com.telink.ble.mesh.core.message.config;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;

/**
 * The Config Model App Status is an unacknowledged message used to report a status for the requesting message,
 * based on the element address, the AppKeyIndex identifying the AppKey on the AppKey List, and the ModelIdentifier.
 * The ModelAppStatusMessage class is a Parcelable class that extends the StatusMessage class.
 * It represents a message containing the status of a model application.
 * The class provides methods to parse the message parameters from a byte array,
 * as well as getters to retrieve the status, element address, app key index, and model identifier.
 */
public class ModelAppStatusMessage extends StatusMessage implements Parcelable {
    private static final int MODEL_STATUS_SIG_LEN = 7;

    private static final int MODEL_STATUS_VENDOR_LEN = 9;

    private byte status;

    private int elementAddress;

    private int appKeyIndex;

    /**
     * 2 or 4 bytes
     */
    private int modelIdentifier;

    /**
     * Default constructor for the ModelAppStatusMessage class.
     */
    public ModelAppStatusMessage() {
    }

    /**
     * Constructor for the ModelAppStatusMessage class that takes a Parcel as input.
     * It reads the status, element address, app key index, and model identifier from the Parcel.
     *
     * @param in The Parcel object to read from.
     */
    protected ModelAppStatusMessage(Parcel in) {
        status = in.readByte();
        elementAddress = in.readInt();
        appKeyIndex = in.readInt();
        modelIdentifier = in.readInt();
    }

    /**
     * A Creator object that implements the Parcelable.Creator interface.
     * It is used to create instances of the ModelAppStatusMessage class from a Parcel.
     */
    public static final Creator<ModelAppStatusMessage> CREATOR = new Creator<ModelAppStatusMessage>() {
        @Override
        public ModelAppStatusMessage createFromParcel(Parcel in) {
            return new ModelAppStatusMessage(in);
        }

        @Override
        public ModelAppStatusMessage[] newArray(int size) {
            return new ModelAppStatusMessage[size];
        }
    };

    /**
     * Parses the message parameters from a byte array.
     * It sets the status, element address, app key index, and model identifier based on the byte array values.
     *
     * @param params The byte array containing the message parameters.
     */
    @Override
    public void parse(byte[] params) {
        int index = 0;
        status = params[index++];
        elementAddress = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        appKeyIndex = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        int modelIdLen;
        if (params.length == MODEL_STATUS_SIG_LEN) {
            modelIdLen = 2;
        } else {
            modelIdLen = 4;
        }
        modelIdentifier = MeshUtils.bytes2Integer(params, index, modelIdLen, ByteOrder.LITTLE_ENDIAN);
    }

    /**
     * Returns a bitmask indicating the set of special object types
     * contained in this Parcelable instance.
     *
     * @return Always returns 0, indicating that there are no special object types.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Writes the status, element address, app key index, and model identifier to a Parcel.
     *
     * @param dest  The Parcel object to write to.
     * @param flags Additional flags about how the object should be written.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(status);
        dest.writeInt(elementAddress);
        dest.writeInt(appKeyIndex);
        dest.writeInt(modelIdentifier);
    }

    /**
     * Returns the status of the model application.
     *
     * @return The status of the model application.
     */
    public byte getStatus() {
        return status;
    }

    /**
     * Returns the element address associated with the model application.
     *
     * @return The element address associated with the model application.
     */
    public int getElementAddress() {
        return elementAddress;
    }

    /**
     * Returns the app key index associated with the model application.
     *
     * @return The app key index associated with the model application.
     */
    public int getAppKeyIndex() {
        return appKeyIndex;
    }

    /**
     * Returns the model identifier of the model application.
     *
     * @return The model identifier of the model application.
     */
    public int getModelIdentifier() {
        return modelIdentifier;
    }
}