package com.telink.ble.mesh.core.ble;

/**
 * Created by kee on 2019/9/9.
 */

public class LeScanSetting {

    /**
     * time between last scanning start time
     */
    public long spacing;

    /**
     * time of scanning
     */
    public long timeout;

    public static LeScanSetting getDefault() {
        LeScanSetting setting = new LeScanSetting();
        setting.spacing = 5 * 1000;
        setting.timeout = 10 * 1000;
        return setting;
    }

    public LeScanSetting() {
    }

    public LeScanSetting(long spacing, long during) {
        this.spacing = spacing;
        this.timeout = during;
    }
}
