/********************************************************************************************************
 * @file MeshUpdatingDevice.java
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
package com.telink.ble.mesh.entity;

import android.os.Parcel;
import android.os.Parcelable;

import java.io.Serializable;

/**
 * Mesh firmware updating device
 * Created by kee on 2019/10/10.
 */
public class MeshUpdatingDevice implements Serializable, Parcelable {

    public static final int STATE_INITIAL = 0;

    public static final int STATE_SUCCESS = 1;

    public static final int STATE_FAIL = 2;

    /**
     * firmware info updated
     */
//    public static final int STATE_FW_UPDATE = 3;

    /**
     * unicast address
     */
    public int meshAddress;

    /**
     * element address at updating model
     *
     * @see com.telink.ble.mesh.core.message.MeshSigModel#SIG_MD_OBJ_TRANSFER_S
     */
    public int updatingEleAddress;

    public int state = STATE_INITIAL;

    public String pidInfo;

    /**
     * latest firmware id
     */
    public byte[] firmwareId;

    /**
     * is low power node
     */
    public boolean isLpn = false;

    /**
     * contains firmware update models
     * and
     * device is online
     */
    public boolean isSupported = false;

    public boolean isOnline = false;

    public boolean selected = false;

    public MeshUpdatingDevice() {
    }

    public String getStateDesc() {
        switch (state) {
            case STATE_INITIAL:
                return "INIT";
            case STATE_FAIL:
                return "Update Fail";
            case STATE_SUCCESS:
                return "Update Success";
        }
        return "";
    }

    protected MeshUpdatingDevice(Parcel in) {
        meshAddress = in.readInt();
        updatingEleAddress = in.readInt();
        state = in.readInt();
        pidInfo = in.readString();
        firmwareId = in.createByteArray();
        isLpn = in.readByte() != 0;
        isSupported = in.readByte() != 0;
        isOnline = in.readByte() != 0;
        selected = in.readByte() != 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(meshAddress);
        dest.writeInt(updatingEleAddress);
        dest.writeInt(state);
        dest.writeString(pidInfo);
        dest.writeByteArray(firmwareId);
        dest.writeByte((byte) (isLpn ? 1 : 0));
        dest.writeByte((byte) (isSupported ? 1 : 0));
        dest.writeByte((byte) (isOnline ? 1 : 0));
        dest.writeByte((byte) (selected ? 1 : 0));
    }

    @Override
    public int describeContents() {
        return 0;
    }

    public static final Creator<MeshUpdatingDevice> CREATOR = new Creator<MeshUpdatingDevice>() {
        @Override
        public MeshUpdatingDevice createFromParcel(Parcel in) {
            return new MeshUpdatingDevice(in);
        }

        @Override
        public MeshUpdatingDevice[] newArray(int size) {
            return new MeshUpdatingDevice[size];
        }
    };
}
