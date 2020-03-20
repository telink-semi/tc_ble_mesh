package com.telink.ble.mesh.entity;

import java.util.Arrays;
import java.util.List;

/**
 * Created by kee on 2019/9/6.
 */

public class MeshUpdatingConfiguration {
    private List<MeshUpdatingDevice> updatingDevices;
    private byte[] firmwareData;
    private int appKeyIndex;
    private int groupAddress;

    private int companyId = 0x0211;

    private int firmwareId = 0xFF000021;

    private long objectId = 0x8877665544332211L;

    public MeshUpdatingConfiguration(List<MeshUpdatingDevice> updatingDevices,
                                     byte[] firmwareData,
                                     int appKeyIndex,
                                     int groupAddress) {
        this.updatingDevices = updatingDevices;
        this.firmwareData = firmwareData;
        this.appKeyIndex = appKeyIndex;
        this.groupAddress = groupAddress;
    }

    public List<MeshUpdatingDevice> getUpdatingDevices() {
        return updatingDevices;
    }

    public byte[] getFirmwareData() {
        return firmwareData;
    }

    public int getAppKeyIndex() {
        return appKeyIndex;
    }

    public int getGroupAddress() {
        return groupAddress;
    }

    public int getCompanyId() {
        return companyId;
    }

    public int getFirmwareId() {
        return firmwareId;
    }

    public long getObjectId() {
        return objectId;
    }

    @Override
    public String toString() {
        return "MeshUpdatingConfiguration{" +
                "updatingDevices=" + updatingDevices.size() +
                ", firmwareData=" + firmwareData.length +
                ", appKeyIndex=" + appKeyIndex +
                ", groupAddress=" + groupAddress +
                ", companyId=" + companyId +
                ", firmwareId=" + firmwareId +
                ", objectId=" + objectId +
                '}';
    }
}
