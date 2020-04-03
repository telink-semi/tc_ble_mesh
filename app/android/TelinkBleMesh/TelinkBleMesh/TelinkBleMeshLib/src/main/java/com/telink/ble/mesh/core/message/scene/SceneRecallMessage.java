package com.telink.ble.mesh.core.message.scene;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.generic.GenericMessage;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * SCENE recall and XX no ack
 * by {@link #ack}
 * Created by kee on 2019/8/14.
 */
public class SceneRecallMessage extends GenericMessage {

    public int sceneNumber;

    // transition id
    public byte tid = 0;

    public byte transitionTime = 0;

    public byte delay = 0;

    public boolean ack = false;

    public boolean isComplete = false;

    public static SceneRecallMessage getSimple(int address, int appKeyIndex, int sceneNumber, boolean ack, int rspMax) {
        SceneRecallMessage message = new SceneRecallMessage(address, appKeyIndex);
        message.sceneNumber = sceneNumber;
        message.ack = ack;
        message.setResponseMax(rspMax);
        return message;
    }

    public SceneRecallMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
        setTidPosition(2);
    }

    @Override
    public int getResponseOpcode() {
        return ack ? Opcode.SCENE_STATUS.value : super.getResponseOpcode();
    }

    @Override
    public int getOpcode() {
        return ack ? Opcode.SCENE_RECALL.value : Opcode.SCENE_RECALL_NOACK.value;
    }

    @Override
    public byte[] getParams() {
        return
                isComplete ?
                        ByteBuffer.allocate(5).order(ByteOrder.LITTLE_ENDIAN)
                                .putShort((short) sceneNumber)
                                .put(tid)
                                .put(transitionTime)
                                .put(delay).array()
                        :
                        ByteBuffer.allocate(3).order(ByteOrder.LITTLE_ENDIAN)
                                .putShort((short) sceneNumber)
                                .put(tid).array();
    }

}
