package com.telink.ble.mesh.core.provisioning;

/**
 * Created by kee on 2019/9/4.
 */

public enum AuthenticationMethod {
    NoOOB((byte) 0x00),
    StaticOOB((byte) 0x01);

    // output oob and input oob are not supported

    public final byte value;

    AuthenticationMethod(byte value) {
        this.value = value;
    }
}
