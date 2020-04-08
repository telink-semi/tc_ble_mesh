package com.telink.ble.mesh.entity;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.access.BindingBearer;

/**
 * Created by kee on 2019/9/6.
 */

public class BindingDevice implements Parcelable {
    /**
     * device unicast address
     */
    private int meshAddress;

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

    private CompositionData compositionData;

    public BindingDevice() {
    }

    public BindingDevice(int meshAddress, byte[] deviceUUID, int appKeyIndex) {
        this.meshAddress = meshAddress;
        this.deviceUUID = deviceUUID;
        this.appKeyIndex = appKeyIndex;
        this.models = null;
        this.bearer = BindingBearer.GattOnly;
    }

    public BindingDevice(int meshAddress, byte[] deviceUUID, int appKeyIndex, int[] models, BindingBearer bearer) {
        this.meshAddress = meshAddress;
        this.deviceUUID = deviceUUID;
        this.appKeyIndex = appKeyIndex;
        this.models = models;
        this.bearer = bearer;
    }

    protected BindingDevice(Parcel in) {
        meshAddress = in.readInt();
        deviceUUID = in.createByteArray();
        appKeyIndex = in.readInt();
        models = in.createIntArray();
        defaultBound = in.readByte() != 0;
        compositionData = in.readParcelable(CompositionData.class.getClassLoader());
    }

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

    public int getMeshAddress() {
        return meshAddress;
    }

    public void setMeshAddress(int meshAddress) {
        this.meshAddress = meshAddress;
    }

    public byte[] getDeviceUUID() {
        return deviceUUID;
    }

    public void setDeviceUUID(byte[] deviceUUID) {
        this.deviceUUID = deviceUUID;
    }

    public int getAppKeyIndex() {
        return appKeyIndex;
    }

    public void setAppKeyIndex(int appKeyIndex) {
        this.appKeyIndex = appKeyIndex;
    }

    public int[] getModels() {
        return models;
    }

    public void setModels(int[] models) {
        this.models = models;
    }

    public BindingBearer getBearer() {
        return bearer;
    }

    public void setBearer(BindingBearer bearer) {
        this.bearer = bearer;
    }

    public boolean isDefaultBound() {
        return defaultBound;
    }

    public void setDefaultBound(boolean defaultBound) {
        this.defaultBound = defaultBound;
    }

    public CompositionData getCompositionData() {
        return compositionData;
    }

    public void setCompositionData(CompositionData compositionData) {
        this.compositionData = compositionData;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(meshAddress);
        dest.writeByteArray(deviceUUID);
        dest.writeInt(appKeyIndex);
        dest.writeIntArray(models);
        dest.writeByte((byte) (defaultBound ? 1 : 0));
        dest.writeParcelable(compositionData, flags);
    }
}
