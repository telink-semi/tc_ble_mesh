package com.telink.ble.mesh.core.message.generic;

import com.telink.ble.mesh.core.message.Opcode;

/**
 * generic level get
 * Created by kee on 2019/8/20.
 */
public class LevelGetMessage extends GenericMessage {


    public LevelGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.G_LEVEL_GET.value;
    }

    @Override
    public byte[] getParams() {
        return null;
    }

    @Override
    public boolean isContainsTid() {
        return false;
    }

    @Override
    public void setTid(byte tid) {
        
    }
}
