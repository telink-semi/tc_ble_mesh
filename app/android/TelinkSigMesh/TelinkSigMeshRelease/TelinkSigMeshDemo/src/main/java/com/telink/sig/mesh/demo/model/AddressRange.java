package com.telink.sig.mesh.demo.model;

import java.io.Serializable;

/**
 * Created by kee on 2019/2/27.
 */

public class AddressRange implements Serializable {
    public int low;
    public int high;

    public AddressRange() {
    }

    public AddressRange(int low, int high) {
        this.low = low;
        this.high = high;
    }
}
