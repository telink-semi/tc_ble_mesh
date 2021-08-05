package com.telink.ble.mesh.model;

import java.io.Serializable;

public enum OnlineState implements Serializable {
    ON(1),
    OFF(0),
    OFFLINE(-1),
    ;
    // state
    public final int st;

    OnlineState(int st) {
        this.st = st;
    }

    public static OnlineState getBySt(int st) {
        for (OnlineState onlineState :
                values()) {
            if (st == onlineState.st) return onlineState;
        }
        return OFFLINE;
    }

}
