package com.telink.sig.mesh.light;

/**
 * Created by kee on 2019/5/28.
 */

public enum StorageType {
    MESH_KEY(0x1005),
    MISC(0x1006),
    NODE_INFO(0x1007),
    CONFIG_MODEL(0x1008),
    MESH_OTA(0x1009);

    private final int typeVal;

    StorageType(int value) {
        this.typeVal = value;
    }

    public int getTypeVal() {
        return typeVal;
    }

    public static StorageType valueOf(int value) {
        for (StorageType type :
                values()) {
            if (value == type.typeVal) {
                return type;
            }
        }
        return null;
    }
}
