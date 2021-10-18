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
 * The Config Default TTL Status is an unacknowledged message used to report the current Default TTL state of a node
 * <p>
 * response of {@link DefaultTTLSetMessage} {@link DefaultTTLGetMessage}
 *
 * @see com.telink.ble.mesh.core.message.Opcode#CFG_DEFAULT_TTL_STATUS
 */
public class DefaultTTLStatusMessage extends StatusMessage implements Parcelable {

    /**
     * Default TTL
     */
    public byte ttl;


    public DefaultTTLStatusMessage() {
    }


    protected DefaultTTLStatusMessage(Parcel in) {
        ttl = in.readByte();
    }

    public static final Creator<DefaultTTLStatusMessage> CREATOR = new Creator<DefaultTTLStatusMessage>() {
        @Override
        public DefaultTTLStatusMessage createFromParcel(Parcel in) {
            return new DefaultTTLStatusMessage(in);
        }

        @Override
        public DefaultTTLStatusMessage[] newArray(int size) {
            return new DefaultTTLStatusMessage[size];
        }
    };

    @Override
    public void parse(byte[] params) {
        ttl = params[0];
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(ttl);
    }
}
