package com.telink.ble.mesh.core.message.scene;

import com.telink.ble.mesh.core.message.Opcode;
import com.telink.ble.mesh.core.message.lighting.LightingMessage;

/**
 * Created by kee on 2019/9/19.
 */

public class SceneGetMessage extends LightingMessage {

    public static SceneGetMessage getSimple(int destinationAddress, int appKeyIndex, int rspMax) {
        SceneGetMessage message = new SceneGetMessage(destinationAddress, appKeyIndex);
        message.setResponseMax(rspMax);
        return message;
    }

    public SceneGetMessage(int destinationAddress, int appKeyIndex) {
        super(destinationAddress, appKeyIndex);
    }

    @Override
    public int getOpcode() {
        return Opcode.SCENE_GET.value;
    }

    @Override
    public int getResponseOpcode() {
        return Opcode.SCENE_STATUS.value;
    }
}
