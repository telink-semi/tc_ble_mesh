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

import com.telink.ble.mesh.core.message.StatusMessage;

/**
 * The Config Friend Status is an unacknowledged message used to report the current Friend state of a node
 * <p>
 * response of {@link FriendGetMessage} {@link FriendSetMessage}
 *
 * @see com.telink.ble.mesh.core.message.Opcode#CFG_FRIEND_STATUS
 */
public class FriendStatusMessage extends StatusMessage implements Parcelable {


    /**
     * Friend state
     */
    public byte friend;


    public FriendStatusMessage() {
    }


    protected FriendStatusMessage(Parcel in) {
        friend = in.readByte();
    }

    public static final Creator<FriendStatusMessage> CREATOR = new Creator<FriendStatusMessage>() {
        @Override
        public FriendStatusMessage createFromParcel(Parcel in) {
            return new FriendStatusMessage(in);
        }

        @Override
        public FriendStatusMessage[] newArray(int size) {
            return new FriendStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        friend = params[0];
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(friend);
    }
}
