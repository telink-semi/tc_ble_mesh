package com.telink.ble.mesh.core.networking;


/**
 * Access Command
 * Created by kee on 2019/8/12.
 */
public enum AccessType {
    /**
     * for common model and vendor model
     * use application key for encryption/decryption
     */
    APPLICATION(1),

    /**
     * for config model settings
     * use device key for encryption/decryption
     */
    DEVICE(0);


    public static AccessType getByAkf(byte akf) {
        for (AccessType at :
                values()) {
            if (at.akf == akf) {
                return at;
            }
        }
        return null;
    }

    public final byte akf;

    AccessType(int akf) {
        this.akf = (byte) akf;
    }


}
