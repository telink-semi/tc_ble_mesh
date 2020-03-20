package com.telink.ble.mesh.foundation.parameter;

/**
 * Created by kee on 2017/11/23.
 */

public class GattOtaParameters extends Parameters {


    public GattOtaParameters(int targetMeshAddress, byte[] firmware) {
        this.set(COMMON_PROXY_FILTER_INIT_NEEDED, true);
        this.setTarget(targetMeshAddress);
        this.setFirmware(firmware);
    }

    public void setTarget(int meshAddress) {
        this.set(ACTION_OTA_MESH_ADDRESS, meshAddress);
    }

    public void setFirmware(byte[] firmware) {
        this.set(ACTION_OTA_FIRMWARE, firmware);
    }

}
