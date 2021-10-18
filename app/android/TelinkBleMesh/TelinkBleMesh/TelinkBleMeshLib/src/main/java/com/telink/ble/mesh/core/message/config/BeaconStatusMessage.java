/********************************************************************************************************
 * @file BeaconStatusMessage.java
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

import com.telink.ble.mesh.core.message.StatusMessage;

/**
 * The Config Beacon Status is an unacknowledged message used to report the current Secure Network Beacon state of a node
 * <p>
 * response of {@link BeaconGetMessage} {@link BeaconSetMessage}
 * <p>
 * {@link com.telink.ble.mesh.core.message.Opcode#CFG_BEACON_STATUS}
 */
public class BeaconStatusMessage extends StatusMessage implements Parcelable {


    public byte beacon;


    public BeaconStatusMessage() {
    }


    protected BeaconStatusMessage(Parcel in) {
        beacon = in.readByte();
    }

    public static final Creator<BeaconStatusMessage> CREATOR = new Creator<BeaconStatusMessage>() {
        @Override
        public BeaconStatusMessage createFromParcel(Parcel in) {
            return new BeaconStatusMessage(in);
        }

        @Override
        public BeaconStatusMessage[] newArray(int size) {
            return new BeaconStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        beacon = params[0];
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(beacon);
    }
}
