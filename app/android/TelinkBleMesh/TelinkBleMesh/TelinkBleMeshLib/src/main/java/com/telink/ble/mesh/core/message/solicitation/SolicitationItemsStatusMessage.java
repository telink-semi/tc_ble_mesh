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
package com.telink.ble.mesh.core.message.solicitation;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.message.StatusMessage;

/**
 * A LARGE_COMPOSITION_DATA_STATUS message is an unacknowledged message used to report a
 * portion of a page of the Composition Data
 */
public class SolicitationItemsStatusMessage extends StatusMessage implements Parcelable {

    /**
     * Page number of the Composition Data
     */
    public byte privateProxy;

    public SolicitationItemsStatusMessage() {
    }


    protected SolicitationItemsStatusMessage(Parcel in) {
        privateProxy = in.readByte();
    }

    public static final Creator<SolicitationItemsStatusMessage> CREATOR = new Creator<SolicitationItemsStatusMessage>() {
        @Override
        public SolicitationItemsStatusMessage createFromParcel(Parcel in) {
            return new SolicitationItemsStatusMessage(in);
        }

        @Override
        public SolicitationItemsStatusMessage[] newArray(int size) {
            return new SolicitationItemsStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        privateProxy = params[0];
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(privateProxy);
    }
}
