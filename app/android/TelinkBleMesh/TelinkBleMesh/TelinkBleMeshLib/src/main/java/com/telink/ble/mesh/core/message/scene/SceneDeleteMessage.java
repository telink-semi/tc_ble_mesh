package com.telink.ble.mesh.core.message.scene;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.generic.GenericMessage;

/**
 * scene store
 * Created by kee on 2019/8/14.
 */
public class SceneDeleteMessage extends GenericMessage {

    // scene id
    private int sceneNumber;

    private boolean ack;

    public static SceneDeleteMessage getSimple(int address, int appKeyIndex, int sceneNumber, boolean ack, int rspMax) {
        SceneDeleteMessage message = new SceneDeleteMessage(address, appKeyIndex);
        message.sceneNumber = sceneNumber;
        message.ack = ack;
        message.setResponseMax(rspMax);
        return message;
    }

    public SceneDeleteMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getResponseOpcode() {
        return ack ? Opcode.SCENE_REG_STATUS.value : super.getResponseOpcode();
    }

    @Override
    public int getOpcode() {
        return ack ? Opcode.SCENE_DEL.value : Opcode.SCENE_DEL_NOACK.value;
    }

    @Override
    public byte[] getParams() {
        return new byte[]{(byte) sceneNumber, (byte) (sceneNumber >> 8)};
    }

    public void setSceneNumber(int sceneNumber) {
        this.sceneNumber = sceneNumber;
    }

    public void setAck(boolean ack) {
        this.ack = ack;
    }
}
