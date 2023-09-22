/********************************************************************************************************
 * @file BindingDevice.java
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

import com.telink.ble.mesh.core.access.BindingBearer;

/**
 * Created by kee on 2019/9/6.
 */

public class BindingDevice implements Parcelable {

    /**
     * network key index,
     * if the index value is -1, it would be replaced by {@link com.telink.ble.mesh.foundation.MeshConfiguration#netKeyIndex}
     */
    private int netKeyIndex = -1;

    /**
     * device unicast address
     */
    private int meshAddress;

    /**
     * UUID of the device.
     */
    private byte[] deviceUUID;

    /**
     * model and appKey map, null means bind all models
     */
//    private SparseIntArray modelAppKeyMap;

    /**
     * app key index, should be contained in NetworkingController#appKeyMap
     */
    private int appKeyIndex;

    /**
     * models bound at this key
     */
    private int[] models;

    /**
     * binding bearer
     * {@link BindingBearer#GattOnly} and {@link BindingBearer#Any}
     */
    private BindingBearer bearer = BindingBearer.GattOnly;

    /**
     * default bound is private action defined by telink, for faster binding
     * if valued by true, when node received app key, it will bind all models automatically;
     */
    private boolean defaultBound = false;

    /**
     * Composition data of the device.
     */
    private CompositionData compositionData;

    /**
     * Default constructor.
     */
    public BindingDevice() {
    }

    /**
     * Constructor with parameters.
     *
     * @param meshAddress The unicast address of the device.
     * @param deviceUUID  The UUID of the device.
     * @param appKeyIndex The index of the application key to be used for binding.
     */
    public BindingDevice(int meshAddress, byte[] deviceUUID, int appKeyIndex) {
        this.meshAddress = meshAddress;
        this.deviceUUID = deviceUUID;
        this.appKeyIndex = appKeyIndex;
        this.models = null;
        this.bearer = BindingBearer.GattOnly;
    }

    /**
     * Constructor with parameters.
     *
     * @param meshAddress The unicast address of the device.
     * @param deviceUUID  The UUID of the device.
     * @param appKeyIndex The index of the application key to be used for binding.
     * @param models      Array of model IDs to be bound.
     * @param bearer      The binding bearer.
     */
    public BindingDevice(int meshAddress, byte[] deviceUUID, int appKeyIndex, int[] models, BindingBearer bearer) {
        this.meshAddress = meshAddress;
        this.deviceUUID = deviceUUID;
        this.appKeyIndex = appKeyIndex;
        this.models = models;
        this.bearer = bearer;
    }

    /**
     * Constructor for Parcelable implementation.
     *
     * @param in The Parcel object from which to read the values.
     */
    protected BindingDevice(Parcel in) {
        netKeyIndex = in.readInt();
        meshAddress = in.readInt();
        deviceUUID = in.createByteArray();
        appKeyIndex = in.readInt();
        models = in.createIntArray();
        defaultBound = in.readByte() != 0;
        compositionData = in.readParcelable(CompositionData.class.getClassLoader());
    }

    /**
     * Parcelable creator for creating instances of BindingDevice from a Parcel.
     */
    public static final Creator<BindingDevice> CREATOR = new Creator<BindingDevice>() {
        @Override
        public BindingDevice createFromParcel(Parcel in) {
            return new BindingDevice(in);
        }

        @Override
        public BindingDevice[] newArray(int size) {
            return new BindingDevice[size];
        }
    };

    /**
     * Getter for netKeyIndex.
     *
     * @return The network key index.
     */
    public int getNetKeyIndex() {
        return netKeyIndex;
    }

    /**
     * Setter for netKeyIndex.
     *
     * @param netKeyIndex The network key index to set.
     */
    public void setNetKeyIndex(int netKeyIndex) {
        this.netKeyIndex = netKeyIndex;
    }

    /**
     * Getter for meshAddress.
     *
     * @return The unicast address of the device.
     */
    public int getMeshAddress() {
        return meshAddress;
    }

    /**
     * Setter for meshAddress.
     *
     * @param meshAddress The unicast address to set.
     */
    public void setMeshAddress(int meshAddress) {
        this.meshAddress = meshAddress;
    }

    /**
     * Getter for deviceUUID.
     *
     * @return The UUID of the device.
     */
    public byte[] getDeviceUUID() {
        return deviceUUID;
    }

    /**
     * Setter for deviceUUID.
     *
     * @param deviceUUID The UUID to set.
     */
    public void setDeviceUUID(byte[] deviceUUID) {
        this.deviceUUID = deviceUUID;
    }

    /**
     * Getter for appKeyIndex.
     *
     * @return The index of the application key.
     */
    public int getAppKeyIndex() {
        return appKeyIndex;
    }

    /**
     * Setter for appKeyIndex.
     *
     * @param appKeyIndex The index of the application key to set.
     */
    public void setAppKeyIndex(int appKeyIndex) {
        this.appKeyIndex = appKeyIndex;
    }

    /**
     * Getter for models.
     *
     * @return The array of model IDs.
     */
    public int[] getModels() {
        return models;
    }

    /**
     * Setter for models.
     *
     * @param models The array of model IDs to set.
     */
    public void setModels(int[] models) {
        this.models = models;
    }

    /**
     * Getter for bearer.
     *
     * @return The binding bearer.
     */
    public BindingBearer getBearer() {
        return bearer;
    }

    /**
     * Setter for bearer.
     *
     * @param bearer The binding bearer to set.
     */
    public void setBearer(BindingBearer bearer) {
        this.bearer = bearer;
    }

    /**
     * Getter for defaultBound.
     *
     * @return True if default bound, false otherwise.
     */
    public boolean isDefaultBound() {
        return defaultBound;
    }

    /**
     * Setter for defaultBound.
     *
     * @param defaultBound The value to set for default bound.
     */
    public void setDefaultBound(boolean defaultBound) {
        this.defaultBound = defaultBound;
    }

    /**
     * Getter for compositionData.
     *
     * @return The composition data of the device.
     */
    public CompositionData getCompositionData() {
        return compositionData;
    }

    /**
     * Setter for compositionData.
     *
     * @param compositionData The composition data to set.
     */
    public void setCompositionData(CompositionData compositionData) {
        this.compositionData = compositionData;
    }

    /**
     * Implementation of describeContents() method of Parcelable interface.
     *
     * @return Always returns 0.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Implementation of writeToParcel() method of Parcelable interface.
     *
     * @param dest  The Parcel object to write the values to.
     * @param flags Additional flags for writing the parcel.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(netKeyIndex);
        dest.writeInt(meshAddress);
        dest.writeByteArray(deviceUUID);
        dest.writeInt(appKeyIndex);
        dest.writeIntArray(models);
        dest.writeByte((byte) (defaultBound ? 1 : 0));
        dest.writeParcelable(compositionData, flags);
    }
}