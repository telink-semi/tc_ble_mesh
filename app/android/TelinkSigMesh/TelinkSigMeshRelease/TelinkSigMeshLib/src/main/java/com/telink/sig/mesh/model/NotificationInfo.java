package com.telink.sig.mesh.model;

import com.telink.sig.mesh.util.Arrays;

import java.io.Serializable;

/**
 * decrypted notification info from mesh
 * Created by kee on 2019/3/28.
 */
public class NotificationInfo implements Serializable {
    public int srcAdr;
    public int destAdr;
    public int opcode;
    public boolean isVendor;
    public byte[] params;

    public NotificationInfo(int srcAdr, int destAdr, int opcode, boolean isVendor, byte[] params) {
        this.srcAdr = srcAdr;
        this.destAdr = destAdr;
        this.opcode = opcode;
        this.params = params;
        this.isVendor = isVendor;
    }

    @Override
    public String toString() {
        return "NotificationInfo{" +
                "srcAdr=" + String.format("%04X", srcAdr) +
                ", destAdr=" + String.format("%04X", destAdr) +
                ", opcode=" + String.format("%06X", opcode) +
                ", isVendor=" + isVendor +
                ", params=" + Arrays.bytesToHexString(params, ":") +
                '}';
    }
}
