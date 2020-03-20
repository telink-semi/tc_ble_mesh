package com.telink.ble.mesh.core.access;

/**
 * bearer
 * Created by kee on 2019/9/5.
 */

public enum BindingBearer {
    /**
     * binding only when target device is direct connected
     */
    GattOnly,

    /**
     * binding if mesh proxy is connected
     */
    Any
}
