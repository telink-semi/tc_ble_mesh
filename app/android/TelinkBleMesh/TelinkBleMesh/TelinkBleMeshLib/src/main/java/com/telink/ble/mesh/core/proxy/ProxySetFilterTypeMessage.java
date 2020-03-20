package com.telink.ble.mesh.core.proxy;

/**
 * Created by kee on 2019/8/26.
 */

public class ProxySetFilterTypeMessage extends ProxyConfigurationMessage {

    private byte filterType;

    public ProxySetFilterTypeMessage(byte filterType) {
        this.filterType = filterType;
    }

    @Override
    public byte getOpcode() {
        return OPCODE_SET_FILTER_TYPE;
    }

    @Override
    public byte[] toByteArray() {
        return new byte[]{getOpcode(), filterType};
    }
}
