package com.telink.ble.mesh.core.proxy;

/**
 * Created by kee on 2019/8/26.
 */

public enum ProxyFilterType {
    WhiteList((byte) 0),
    BlackList((byte) 1);
    public final byte value;

    ProxyFilterType(byte value) {
        this.value = value;
    }
}
