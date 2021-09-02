package com.telink.ble.mesh.model;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;

/**
 * device config values, such as TTL, relay
 */
public class DeviceConfig implements Serializable {

    public ConfigState configState;

    public String desc;

    public boolean expanded = false;

    public DeviceConfig(ConfigState configState) {
        this.configState = configState;
    }

}
