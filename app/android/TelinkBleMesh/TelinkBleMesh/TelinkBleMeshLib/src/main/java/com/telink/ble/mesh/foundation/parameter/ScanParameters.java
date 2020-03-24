package com.telink.ble.mesh.foundation.parameter;


import com.telink.ble.mesh.core.ble.LeScanFilter;
import com.telink.ble.mesh.core.ble.UUIDInfo;

import java.util.UUID;

/**
 * Scan params
 * Created by kee on 2017/11/23.
 */

public class ScanParameters extends Parameters {
    private LeScanFilter filter;

    public ScanParameters() {
        filter = new LeScanFilter();
    }

    public static ScanParameters getDefault(boolean provisioned, boolean single) {
        ScanParameters parameters = new ScanParameters();

        if (provisioned) {
            parameters.filter.uuidInclude = new UUID[]{UUIDInfo.PROXY_SERVICE_UUID};
        } else {
            parameters.filter.uuidInclude = new UUID[]{UUIDInfo.PROVISION_SERVICE_UUID};
        }
        parameters.setScanFilter(parameters.filter);
        parameters.singleMode(single);
        return parameters;
    }

    public void setIncludeMacs(String[] macs) {
        if (filter != null)
            filter.macInclude = macs;
    }

    public void setExcludeMacs(String[] macs) {
        if (filter != null)
            filter.macExclude = macs;
    }

    public void singleMode(boolean single) {
        this.set(SCAN_SINGLE_MODE, single);
    }

}
