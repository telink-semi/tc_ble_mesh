/********************************************************************************************************
 * @file ModelPublication.java
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

package com.telink.ble.mesh.entity;


import android.os.Parcel;
import android.os.Parcelable;

import java.io.Serializable;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * publication entity used when send publicationSet message or receive publication status message
 * Created by kee on 2018/9/17.
 */
public final class ModelPublication implements Serializable, Parcelable {

    /**
     * changed from 1 to 0, because of device may not have friend key
     */
    public static final int CREDENTIAL_FLAG_DEFAULT = 0b0;

    public static final int RFU_DEFAULT = 0x00;


    // use default ttl in firmware
    public static final int TTL_DEFAULT = 0xFF;

    /**
     * default retransmit: 0x15
     * 0b 00010 101
     * count: 0x05, step: 0x02
     */
    public static final int RETRANSMIT_COUNT_DEFAULT = 0x05;


    public static final int RETRANSMIT_INTERVAL_STEP_DEFAULT = 0x02;


    /**
     * element address
     * 16 bits
     */
    public int elementAddress;

    /**
     * Value of the publish address
     * 16 bits
     */
    public int publishAddress;

    /**
     * Index of the application key
     * 12 bits
     */
    public int appKeyIndex;

    /**
     * Value of the Friendship Credential Flag
     * 1 bit
     * 0 Master security material is used for Publishing
     * 1 Friendship security material is used for Publishing， used on LPN
     */
    public int credentialFlag = CREDENTIAL_FLAG_DEFAULT;

    /**
     * 3 bits
     * reserved for future use
     */
    public int rfu = RFU_DEFAULT;

    /**
     * Default TTL value for the outgoing messages
     * 8 bits
     */
    public int ttl = TTL_DEFAULT;

    /**
     * Period for periodic status publishing
     * 8 bits
     */
    public byte period;

    /**
     * Number of retransmissions for each published message
     * 3 bits
     */
    public int retransmitCount = RETRANSMIT_COUNT_DEFAULT;

    /**
     * Number of 50-millisecond steps between retransmissions
     * 5 bits
     */
    public int retransmitIntervalSteps = RETRANSMIT_INTERVAL_STEP_DEFAULT;

    /**
     * SIG Model ID or Vendor Model ID
     * 16 or 32 bits
     */
    public int modelId;

    /**
     * if sig model
     * #modelId
     */
    public boolean sig;


    protected ModelPublication(Parcel in) {
        elementAddress = in.readInt();
        publishAddress = in.readInt();
        appKeyIndex = in.readInt();
        credentialFlag = in.readInt();
        rfu = in.readInt();
        ttl = in.readInt();
        period = in.readByte();
        retransmitCount = in.readInt();
        retransmitIntervalSteps = in.readInt();
        modelId = in.readInt();
        sig = in.readByte() != 0;
    }

    public static final Creator<ModelPublication> CREATOR = new Creator<ModelPublication>() {
        @Override
        public ModelPublication createFromParcel(Parcel in) {
            return new ModelPublication(in);
        }

        @Override
        public ModelPublication[] newArray(int size) {
            return new ModelPublication[size];
        }
    };

    public ModelPublication() {
    }

    public static ModelPublication createDefault(int elementAddress, int publishAddress, int appKeyIndex, long periodMillisecond, int modelId, boolean sig) {
        ModelPublication instance = new ModelPublication();
        instance.elementAddress = elementAddress;
        instance.publishAddress = publishAddress;
        instance.appKeyIndex = appKeyIndex;
        instance.period = TransitionTime.fromTime(periodMillisecond).getValue();
        instance.modelId = modelId;
        instance.sig = sig;
        return instance;
    }

    public byte[] toBytes() {
        final int len = sig ? 11 : 13;
        ByteBuffer bf = ByteBuffer.allocate(len).order(ByteOrder.LITTLE_ENDIAN);
        bf
                .putShort((short) elementAddress)
                .putShort((short) publishAddress)
                .putShort((short) ((appKeyIndex & 0xFFF) | ((credentialFlag & 0b01) << 12) | ((rfu & 0b111) << 13)))
                .put((byte) ttl)
                .put(period)
                .put((byte) ((retransmitCount & 0b111) | (retransmitIntervalSteps << 3)));
        if (sig) {
            bf.putShort((short) modelId);
        } else {
            bf.putInt(modelId);
        }
        return bf.array();
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(elementAddress);
        dest.writeInt(publishAddress);
        dest.writeInt(appKeyIndex);
        dest.writeInt(credentialFlag);
        dest.writeInt(rfu);
        dest.writeInt(ttl);
        dest.writeByte(period);
        dest.writeInt(retransmitCount);
        dest.writeInt(retransmitIntervalSteps);
        dest.writeInt(modelId);
        dest.writeByte((byte) (sig ? 1 : 0));
    }
}
