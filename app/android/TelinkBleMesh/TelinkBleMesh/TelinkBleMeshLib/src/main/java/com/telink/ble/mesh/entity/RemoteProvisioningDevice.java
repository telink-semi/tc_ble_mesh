package com.telink.ble.mesh.entity;

import android.os.Parcel;

import com.telink.ble.mesh.core.message.rp.ScanReportStatusMessage;
import com.telink.ble.mesh.util.Arrays;

/**
 * Model for provisioning flow
 * Created by kee on 2019/9/4.
 */
// advertisingDevice is null
public class RemoteProvisioningDevice extends ProvisioningDevice {

    private byte[] uuid = null;

    // proxy address
    private int serverAddress;


    public RemoteProvisioningDevice(ScanReportStatusMessage scanReportStatusMessage, int serverAddress) {
        this.uuid = scanReportStatusMessage.getUuid();
        this.serverAddress = serverAddress;
    }

    protected RemoteProvisioningDevice(Parcel in) {
        super(in);
        uuid = in.createByteArray();
        serverAddress = in.readInt();
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        super.writeToParcel(dest, flags);
        dest.writeByteArray(uuid);
        dest.writeInt(serverAddress);
    }

    @Override
    public int describeContents() {
        return 0;
    }

    public static final Creator<RemoteProvisioningDevice> CREATOR = new Creator<RemoteProvisioningDevice>() {
        @Override
        public RemoteProvisioningDevice createFromParcel(Parcel in) {
            return new RemoteProvisioningDevice(in);
        }

        @Override
        public RemoteProvisioningDevice[] newArray(int size) {
            return new RemoteProvisioningDevice[size];
        }
    };

    public byte[] getUuid() {
        return uuid;
    }

    public int getServerAddress() {
        return serverAddress;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        RemoteProvisioningDevice device = (RemoteProvisioningDevice) o;
        return java.util.Arrays.equals(uuid, device.uuid);
    }

    @Override
    public int hashCode() {
        return java.util.Arrays.hashCode(uuid);
    }


}
