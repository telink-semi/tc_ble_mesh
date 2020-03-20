package com.telink.ble.mesh.core.message;

import com.telink.ble.mesh.core.MeshUtils;

public enum OpcodeType {
    SIG_1(1),
    SIG_2(2),
    VENDOR(3);

    public final int length;

    OpcodeType(int length) {
        this.length = length;
    }

    /**
     * @param opFst first byte of opcode
     */
    public static OpcodeType getByFirstByte(byte opFst) {
        return (opFst & MeshUtils.bit(7)) != 0
                ?
                ((opFst & MeshUtils.bit(6)) != 0 ? VENDOR : SIG_2)
                :
                SIG_1;
    }
}
