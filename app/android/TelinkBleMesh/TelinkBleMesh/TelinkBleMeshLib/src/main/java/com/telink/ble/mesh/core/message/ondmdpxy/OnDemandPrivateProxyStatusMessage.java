/********************************************************************************************************
 * @file CompositionDataStatusMessage.java
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
package com.telink.ble.mesh.core.message.ondmdpxy;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.message.StatusMessage;

/**
 * OnDemandPrivateProxyStatusMessage
 */
public class OnDemandPrivateProxyStatusMessage extends StatusMessage implements Parcelable {
    /**
     * private proxy
     */
    public byte privateProxy;

    /**
     * Constructs a new OnDemandPrivateProxyStatusMessage object.
     */
    public OnDemandPrivateProxyStatusMessage() {
    }

    /**
     * Constructs a new OnDemandPrivateProxyStatusMessage object from a Parcel.
     *
     * @param in The Parcel object to read the values from.
     */
    protected OnDemandPrivateProxyStatusMessage(Parcel in) {
        privateProxy = in.readByte();
    }

    /**
     * A Creator object that is used to create instances of OnDemandPrivateProxyStatusMessage from a Parcel.
     */
    public static final Creator<OnDemandPrivateProxyStatusMessage> CREATOR = new Creator<OnDemandPrivateProxyStatusMessage>() {
        @Override
        public OnDemandPrivateProxyStatusMessage createFromParcel(Parcel in) {
            return new OnDemandPrivateProxyStatusMessage(in);
        }

        @Override
        public OnDemandPrivateProxyStatusMessage[] newArray(int size) {
            return new OnDemandPrivateProxyStatusMessage[size];
        }
    };

    /**
     * Parses the byte array to extract the page number of the Composition Data.
     *
     * @param params The byte array to parse.
     */
    @Override
    public void parse(byte[] params) {
        privateProxy = params[0];
    }

    /**
     * Describes the contents of the OnDemandPrivateProxyStatusMessage object.
     *
     * @return An integer value representing the description of the contents.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Writes the page number of the Composition Data to the Parcel object.
     *
     * @param dest  The Parcel object to write the values to.
     * @param flags Additional flags about how the object should be written.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(privateProxy);
    }
}