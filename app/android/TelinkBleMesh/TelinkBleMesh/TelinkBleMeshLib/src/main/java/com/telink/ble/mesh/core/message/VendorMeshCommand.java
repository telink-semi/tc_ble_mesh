package com.telink.ble.mesh.core.message;

/**
 * Created by kee on 2019/8/12.
 */

public class VendorMeshCommand extends MeshCommand {


    private int opcode;

    // custom params
    private byte[] params;

    public VendorMeshCommand(int opcode, byte[] params) {
        this.opcode = opcode;
        this.params = params;
    }

    @Override
    public int getOpcode() {
        return opcode;
    }


    @Override
    public byte[] getParams() {
        return params;
    }


}
