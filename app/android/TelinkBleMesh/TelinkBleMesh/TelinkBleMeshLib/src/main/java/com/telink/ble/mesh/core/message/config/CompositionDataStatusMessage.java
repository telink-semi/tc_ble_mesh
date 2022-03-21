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
package com.telink.ble.mesh.core.message.config;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.message.StatusMessage;
import com.telink.ble.mesh.entity.CompositionData;

/**
 * The Config Composition Data Status is an unacknowledged message used to report a single page of the Composition Data
 */
public class CompositionDataStatusMessage extends StatusMessage implements Parcelable {

    /**
     * Page number of the Composition Data
     */
    private byte page;

    /**
     * Composition Data for the identified page
     */
    private CompositionData compositionData;

    public CompositionDataStatusMessage() {
    }

    protected CompositionDataStatusMessage(Parcel in) {
        page = in.readByte();
        compositionData = in.readParcelable(CompositionData.class.getClassLoader());
    }

    public static final Creator<CompositionDataStatusMessage> CREATOR = new Creator<CompositionDataStatusMessage>() {
        @Override
        public CompositionDataStatusMessage createFromParcel(Parcel in) {
            return new CompositionDataStatusMessage(in);
        }

        @Override
        public CompositionDataStatusMessage[] newArray(int size) {
            return new CompositionDataStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        page = params[0];
        byte[] cpsData = new byte[params.length - 1];
        System.arraycopy(params, 1, cpsData, 0, cpsData.length);
        compositionData = CompositionData.from(cpsData);
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(page);
        dest.writeParcelable(compositionData, flags);
    }

    public byte getPage() {
        return page;
    }

    public CompositionData getCompositionData() {
        return compositionData;
    }
}
