package com.telink.ble.mesh.foundation.parameter;

import com.telink.ble.mesh.core.ble.UUIDInfo;
import com.telink.ble.mesh.entity.BindingDevice;

/**
 * Created by kee on 2019/9/5.
 */

public class BindingParameters extends Parameters {

    /**
     * @param device target device
     */
    public BindingParameters(BindingDevice device){
        this.setScanFilter(createScanFilter(UUIDInfo.PROXY_SERVICE_UUID));
        this.set(ACTION_BINDING_TARGET, device);
    }


}
