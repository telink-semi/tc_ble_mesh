/********************************************************************************************************
 * @file ModelAppStatusMessage.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2010
 *
 * @par Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *
 *******************************************************************************************************/
package com.telink.ble.mesh.core.message.config;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;

/**
 * The Config Key Refresh Phase Status is an unacknowledged message used to report the current Key Refresh Phase state of the identified network key
 * <p>
 * response of {@link KeyRefreshPhaseGetMessage} {@link KeyRefreshPhaseSetMessage}
 *
 * @see com.telink.ble.mesh.core.message.Opcode#CFG_KEY_REFRESH_PHASE_STATUS
 */
public class KeyRefreshPhaseStatusMessage extends StatusMessage implements Parcelable {


    public byte status;

    public int netKeyIndex;

    public byte phase;

    public KeyRefreshPhaseStatusMessage() {
    }


    protected KeyRefreshPhaseStatusMessage(Parcel in) {
        status = in.readByte();
        netKeyIndex = in.readInt();
        phase = in.readByte();
    }

    public static final Creator<KeyRefreshPhaseStatusMessage> CREATOR = new Creator<KeyRefreshPhaseStatusMessage>() {
        @Override
        public KeyRefreshPhaseStatusMessage createFromParcel(Parcel in) {
            return new KeyRefreshPhaseStatusMessage(in);
        }

        @Override
        public KeyRefreshPhaseStatusMessage[] newArray(int size) {
            return new KeyRefreshPhaseStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        int index = 0;
        status = params[index++];
        netKeyIndex = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        phase = params[index];
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel parcel, int i) {
        parcel.writeByte(status);
        parcel.writeInt(netKeyIndex);
        parcel.writeByte(phase);
    }
}
