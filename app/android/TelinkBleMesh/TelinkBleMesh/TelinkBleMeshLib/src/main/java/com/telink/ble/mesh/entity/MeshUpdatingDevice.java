/********************************************************************************************************
 * @file MeshUpdatingDevice.java
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

import com.telink.ble.mesh.core.message.firmwareupdate.AdditionalInformation;

import java.io.Serializable;

/**
 * Mesh firmware updating device
 * Created by kee on 2019/10/10.
 */
public class MeshUpdatingDevice implements Serializable, Parcelable, Cloneable {

    public static final int STATE_INITIAL = 0;

    public static final int STATE_SUCCESS = 1;

    public static final int STATE_FAIL = 2;

    public static final int STATE_METADATA_RSP = 3;

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
     * AdditionalInformation in metadata
     */
    public AdditionalInformation additionalInformation = null;

    /**
     * is low power node
     */
    public int pid = 0;

    /**
     * contains firmware update models
     * and
     * device is online
     */
    public boolean isSupported = false;

    public boolean isOnline = false;

    public boolean selected = false;

    public boolean isLpn = false;

    public MeshUpdatingDevice() {
    }

    protected MeshUpdatingDevice(Parcel in) {
        meshAddress = in.readInt();
        updatingEleAddress = in.readInt();
        state = in.readInt();
        pidInfo = in.readString();
        firmwareId = in.createByteArray();
        pid = in.readInt();
        isSupported = in.readByte() != 0;
        isOnline = in.readByte() != 0;
        selected = in.readByte() != 0;
        isLpn = in.readByte() != 0;
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

    public String getStateDesc() {
        switch (state) {
            case STATE_INITIAL:
                return "INIT";
            case STATE_FAIL:
                return "Update Fail";
            case STATE_SUCCESS:
                return "Update Success";
            case STATE_METADATA_RSP:
                return "Metadata RSP";
        }
        return "";
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(meshAddress);
        dest.writeInt(updatingEleAddress);
        dest.writeInt(state);
        dest.writeString(pidInfo);
        dest.writeByteArray(firmwareId);
        dest.writeInt(pid);
        dest.writeByte((byte) (isSupported ? 1 : 0));
        dest.writeByte((byte) (isOnline ? 1 : 0));
        dest.writeByte((byte) (selected ? 1 : 0));
        dest.writeByte((byte) (isLpn ? 1 : 0));
    }

    @Override
    public Object clone() throws CloneNotSupportedException {
        return super.clone();
    }
}
