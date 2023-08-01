package com.telink.ble.mesh.model;

import java.io.Serializable;

import io.objectbox.annotation.Entity;
import io.objectbox.annotation.Id;
import io.objectbox.relation.ToOne;

@Entity
public class SceneState implements Serializable {

    @Id
    public long id;

    /**
     * address
     * device unicast address(0x01 -- 0x7FFF) or group address (C000 - 0xFEFF)
     */
    public ToOne<NodeInfo> nodeInfo;

    /**
     * on off value
     * -1 unknown
     */
    public int onOff;

    /**
     * lum(lightness 0-100) value
     * -1 unknown
     */
    public int lum;

    /**
     * temperature value
     * -1 unknown
     */
    public int temp;

    public SceneState() {
    }

    public SceneState(NodeInfo nodeInfo) {
        this.nodeInfo.setTarget(nodeInfo);
        this.onOff = -1;
        this.lum = -1;
        this.temp = -1;
    }
}