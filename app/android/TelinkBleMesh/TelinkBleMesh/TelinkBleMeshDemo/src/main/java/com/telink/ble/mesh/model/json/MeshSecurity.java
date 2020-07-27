package com.telink.ble.mesh.model.json;

/**
 * Created by kee on 2018/12/17.
 */

public enum MeshSecurity {
    Secure("secure"),
    Insecure("insecure");


    private String desc;

    public String getDesc() {
        return desc;
    }

    MeshSecurity(String desc) {
        this.desc = desc;
    }
}
