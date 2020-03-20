package com.telink.ble.mesh.entity;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.provisioning.pdu.ProvisioningCapabilityPDU;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * Model for provisioning flow
 * Created by kee on 2019/9/4.
 */

public class ProvisioningDevice implements Parcelable {

    /**
     * 16: key
     * 2: key index
     * 1: flags
     * 4: iv index
     * 2: unicast adr
     */
    private static final int DATA_PDU_LEN = 16 + 2 + 1 + 4 + 2;

    private AdvertisingDevice advertisingDevice;

    protected byte[] networkKey;

    protected int networkKeyIndex;

    /**
     * 1 bit
     */
    protected byte keyRefreshFlag;

    /**
     * 1 bit
     */
    protected byte ivUpdateFlag;

    /**
     * 4 bytes
     */
    protected int ivIndex;

    /**
     * unicast address for primary element
     * 2 bytes
     */
    protected int unicastAddress;


    /**
     * auth value for static oob AuthMethod
     * {@link com.telink.ble.mesh.core.provisioning.AuthenticationMethod#StaticOOB}
     */
    protected byte[] authValue = null;

//    private ProvisioningParams provisioningParams;

    protected int provisioningState;

    /**
     * valued when generating provisioning data
     */
    protected byte[] deviceKey = null;

    protected ProvisioningCapabilityPDU deviceCapability = null;

    public ProvisioningDevice(AdvertisingDevice advertisingDevice, int unicastAddress) {
        this.advertisingDevice = advertisingDevice;
        this.unicastAddress = unicastAddress;
    }

    public ProvisioningDevice(AdvertisingDevice advertisingDevice, byte[] networkKey, int networkKeyIndex, byte keyRefreshFlag, byte ivUpdateFlag, int ivIndex, int unicastAddress) {
        this.advertisingDevice = advertisingDevice;
        this.networkKey = networkKey;
        this.networkKeyIndex = networkKeyIndex;
        this.keyRefreshFlag = keyRefreshFlag;
        this.ivUpdateFlag = ivUpdateFlag;
        this.ivIndex = ivIndex;
        this.unicastAddress = unicastAddress;
    }

    protected ProvisioningDevice(Parcel in) {
        advertisingDevice = in.readParcelable(AdvertisingDevice.class.getClassLoader());
        networkKey = in.createByteArray();
        networkKeyIndex = in.readInt();
        keyRefreshFlag = in.readByte();
        ivUpdateFlag = in.readByte();
        ivIndex = in.readInt();
        unicastAddress = in.readInt();
        authValue = in.createByteArray();
        provisioningState = in.readInt();
        deviceKey = in.createByteArray();
    }

    public String getMac() {
        return advertisingDevice == null ? "null" : advertisingDevice.device.getAddress();
    }

    public static final Creator<ProvisioningDevice> CREATOR = new Creator<ProvisioningDevice>() {
        @Override
        public ProvisioningDevice createFromParcel(Parcel in) {
            return new ProvisioningDevice(in);
        }

        @Override
        public ProvisioningDevice[] newArray(int size) {
            return new ProvisioningDevice[size];
        }
    };

    public ProvisioningDevice() {
    }

    public byte[] generateProvisioningData() {
        byte flags = (byte) ((keyRefreshFlag & 0b01) | (ivUpdateFlag & 0b10));
        ByteBuffer buffer = ByteBuffer.allocate(DATA_PDU_LEN).order(ByteOrder.BIG_ENDIAN);
        buffer.put(networkKey)
                .putShort((short) networkKeyIndex)
                .put(flags)
                .putInt(ivIndex)
                .putShort((short) unicastAddress);
        return buffer.array();
    }


    public AdvertisingDevice getAdvertisingDevice() {
        return advertisingDevice;
    }

    public byte[] getNetworkKey() {
        return networkKey;
    }

    public void setNetworkKey(byte[] networkKey) {
        this.networkKey = networkKey;
    }

    public int getNetworkKeyIndex() {
        return networkKeyIndex;
    }

    public void setNetworkKeyIndex(int networkKeyIndex) {
        this.networkKeyIndex = networkKeyIndex;
    }

    public byte getKeyRefreshFlag() {
        return keyRefreshFlag;
    }

    public void setKeyRefreshFlag(byte keyRefreshFlag) {
        this.keyRefreshFlag = keyRefreshFlag;
    }

    public byte getIvUpdateFlag() {
        return ivUpdateFlag;
    }

    public void setIvUpdateFlag(byte ivUpdateFlag) {
        this.ivUpdateFlag = ivUpdateFlag;
    }

    public int getIvIndex() {
        return ivIndex;
    }

    public void setIvIndex(int ivIndex) {
        this.ivIndex = ivIndex;
    }

    public int getUnicastAddress() {
        return unicastAddress;
    }

    public byte[] getAuthValue() {
        return authValue;
    }

    public void setAuthValue(byte[] authValue) {
        this.authValue = authValue;
    }

    public int getProvisioningState() {
        return provisioningState;
    }

    public void setProvisioningState(int provisioningState) {
        this.provisioningState = provisioningState;
    }

    public byte[] getDeviceKey() {
        return deviceKey;
    }

    public void setDeviceKey(byte[] deviceKey) {
        this.deviceKey = deviceKey;
    }

    public ProvisioningCapabilityPDU getDeviceCapability() {
        return deviceCapability;
    }

    public void setDeviceCapability(ProvisioningCapabilityPDU deviceCapability) {
        this.deviceCapability = deviceCapability;
    }

    public void setUnicastAddress(int unicastAddress) {
        this.unicastAddress = unicastAddress;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeParcelable(advertisingDevice, flags);
        dest.writeByteArray(networkKey);
        dest.writeInt(networkKeyIndex);
        dest.writeByte(keyRefreshFlag);
        dest.writeByte(ivUpdateFlag);
        dest.writeInt(ivIndex);
        dest.writeInt(unicastAddress);
        dest.writeByteArray(authValue);
        dest.writeInt(provisioningState);
        dest.writeByteArray(deviceKey);
    }
}