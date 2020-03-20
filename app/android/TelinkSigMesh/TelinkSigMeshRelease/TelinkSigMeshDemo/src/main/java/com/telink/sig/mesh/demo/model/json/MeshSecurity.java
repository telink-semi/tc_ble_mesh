package com.telink.sig.mesh.demo.model.json;

/**
 * Created by kee on 2018/12/17.
 */

public enum MeshSecurity {
    Low("low"),
    High("high");


    private String desc;

    public String getDesc() {
        return desc;
    }

    MeshSecurity(String desc) {
        this.desc = desc;
    }
}
