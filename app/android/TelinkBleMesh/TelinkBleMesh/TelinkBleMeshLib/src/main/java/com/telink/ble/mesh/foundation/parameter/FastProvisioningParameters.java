package com.telink.ble.mesh.foundation.parameter;

import com.telink.ble.mesh.core.ble.UUIDInfo;
import com.telink.ble.mesh.entity.FastProvisioningConfiguration;

/**
 * Created by kee on 2019/9/5.
 */

public class FastProvisioningParameters extends Parameters {

    public FastProvisioningParameters(FastProvisioningConfiguration fastProvisioningConfiguration) {
        this.setScanFilter(createScanFilter(UUIDInfo.PROVISION_SERVICE_UUID));
        this.set(ACTION_FAST_PROVISION_CONFIG, fastProvisioningConfiguration);
        this.set(COMMON_PROXY_FILTER_INIT_NEEDED, false);
    }


}
