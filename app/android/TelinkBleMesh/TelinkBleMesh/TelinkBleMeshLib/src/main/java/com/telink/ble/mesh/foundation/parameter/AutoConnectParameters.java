package com.telink.ble.mesh.foundation.parameter;

import com.telink.ble.mesh.core.ble.UuidInfo;

/**
 * Created by kee on 2019/9/5.
 */

public class AutoConnectParameters extends Parameters {

    public AutoConnectParameters(AutoConnectFilterType filterType) {
        this.setScanFilter(createScanFilter(UuidInfo.PROXY_SERVICE_UUID));
        this.set(ACTION_AUTO_CONNECT_FILTER_TYPE, filterType);
    }

}
