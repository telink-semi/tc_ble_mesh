package com.telink.ble.mesh.model;

import java.io.Serializable;

/**
 * Created by kee on 2017/8/18.
 */

public class GroupInfo implements Serializable {
    /**
     * Group name
     */
    public String name;

    /**
     * Group address
     */
    public int address;

    public boolean selected = false;
}
