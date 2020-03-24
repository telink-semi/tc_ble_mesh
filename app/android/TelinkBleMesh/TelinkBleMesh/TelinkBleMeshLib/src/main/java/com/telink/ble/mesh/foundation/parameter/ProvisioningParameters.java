package com.telink.ble.mesh.foundation.parameter;

import com.telink.ble.mesh.core.ble.UUIDInfo;
import com.telink.ble.mesh.entity.ProvisioningDevice;

/**
 * Created by kee on 2019/9/5.
 */

public class ProvisioningParameters extends Parameters {

    public ProvisioningParameters(ProvisioningDevice device) {
        this.setScanFilter(createScanFilter(UUIDInfo.PROVISION_SERVICE_UUID));
        this.set(ACTION_PROVISIONING_TARGET, device);
    }



}
