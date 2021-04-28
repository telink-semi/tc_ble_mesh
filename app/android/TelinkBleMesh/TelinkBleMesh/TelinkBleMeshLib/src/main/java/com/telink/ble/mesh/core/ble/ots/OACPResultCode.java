package com.telink.ble.mesh.core.ble.ots;

public enum OACPResultCode {
    Success(0x01),

    ;

    int code;

    OACPResultCode(int code) {
        this.code = code;
    }
}
