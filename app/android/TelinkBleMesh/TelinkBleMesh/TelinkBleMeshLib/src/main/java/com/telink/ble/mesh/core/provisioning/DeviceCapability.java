package com.telink.ble.mesh.core.provisioning;

/**
 * Created by kee on 2018/12/11.
 */

public class DeviceCapability {
    private static final int CPB_DATA_LEN = 11;

    private byte[] rawData;

    private DeviceCapability() {
    }

    public static DeviceCapability getCapability(byte[] data) {
        if (data == null || data.length != CPB_DATA_LEN) {
            return null;
        }
        DeviceCapability capability = new DeviceCapability();
        capability.rawData = data;
        return capability;
    }

    public int getElementCnt() {
        return rawData[0];
    }

    public int getAlgorithms() {
        return ((rawData[1] & 0xFF) << 8) | (rawData[2] & 0xFF);
    }

    public int getPublicKeyType() {
        return rawData[3];
    }

    public int getStaticOOBType() {
        return rawData[4];
    }

    public int getOutputOOBSize() {
        return rawData[5];
    }

    public int getOutputOOBAction() {
        return ((rawData[6] & 0xFF) << 8) | (rawData[7] & 0xFF);
    }

    public int getInputOOBSize() {
        return rawData[8];
    }

    public int getInputOOBAction() {
        return ((rawData[9] & 0xFF) << 8) | (rawData[10] & 0xFF);
    }
}
